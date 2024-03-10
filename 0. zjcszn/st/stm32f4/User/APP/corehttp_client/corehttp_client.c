#define LOG_TAG "HTTP"
#define LOG_LVL ELOG_LVL_DEBUG

#include <stdio.h>
#include <string.h>

#include "corehttp_client.h"

#include "elog.h"
#include "FreeRTOS.h"
#include "lwmem/lwmem.h"

/* net interface includes */
#include "transport_mbedtls.h"
#include "transport_plaintext.h"
#include "core_http_client.h"
#include "transport_interface.h"

/* FATFS includes */
#include "ff.h"

#define httpclient_free                     lwmem_free
#define httpclient_malloc                   lwmem_malloc
#define httpclient_calloc                   lwmem_calloc

/* Max times of download error */
#define HTTPCLIENT_DOWNLOAD_MAXERROR        5

/* Size of internal buffer, for request or response */
#define HTTPCLIENT_REQ_BUFFER_SIZE          (1 * 1024)
#define HTTPCLIENT_REQ_RESP_BUFFER_SIZE     (8 * 1024)

/* Max range length of GET Request */
#define HTTPCLIENT_RANGE_REQUEST_LENGTH     (7 * 1024)

#if HTTPCLIENT_REQ_RESP_BUFFER_SIZE < (HTTPCLIENT_RANGE_REQUEST_LENGTH + 1024)
#error "Internal buffer size must be greater than (range request length + 1024)"
#endif

/* Socket send & recv timeout */
#define HTTPCLIENT_SEND_RECV_TIMEOUT_MS     5000

/* enable or disable SNI when use tls */
#define HTTPCLIENT_DISABLE_SNI              pdFALSE
#define HTTPCLIENT_ALPN_PROTOCOL_HTTP_1_1   "http/1.1"
#define HTTPCLIENT_DEFAULT_URL_PATH         "/"
#define HTTPCLIENT_DEFAULT_FILENAME         "default"

#define HEADER_FIELD_ACCEPT                 "Accept"
#define HEADER_FIELD_ACCEPT_SIZE            (sizeof (HEADER_FIELD_ACCEPT) - 1)
#define HEADER_FIELD_ACCEPT_RANGE           "Accept-Ranges"
#define HEADER_FIELD_ACCEPT_RANGE_SIZE      (sizeof (HEADER_FIELD_ACCEPT_RANGE) - 1)

#define HTTPCLIENT_HTTP_PORT                80
#define HTTPCLIENT_HTTPS_PORT               443

/*=========================== static variable ===========================*/

/* alpn protocol */
static const char *apln_list[] = {
    HTTPCLIENT_ALPN_PROTOCOL_HTTP_1_1,
    NULL
};

/* server root ca */
static NetworkCredentials_t root_ca = {
    .disableSni = HTTPCLIENT_DISABLE_SNI,
    .pAlpnProtos = apln_list,
    .pRootCa = (const uint8_t *)HTTPCLIENT_ROOT_CA,
    .rootCaSize = sizeof(HTTPCLIENT_ROOT_CA),
};

/*============================== interface ==============================*/

static httpclient_t *httpclient_create(uint32_t bufsz) {
    httpclient_t *client;
    client = httpclient_calloc(1, sizeof(httpclient_t));
    if (client == NULL)
        goto error1;

    client->buffer = httpclient_malloc(bufsz * sizeof(char));
    if (client->buffer == NULL)
        goto error2;

    client->bufsz = bufsz;
    client->context.tcpSocket = INVALID_SOCKET;
    client->request.pBuffer = client->buffer;
    client->request.bufferLen = bufsz;
    client->trans.pNetworkContext = &client->context;

    return client;

error2:
    httpclient_free(client);
error1:
    log_e("httpclient create failed, no memory for httpclient client!");
    return NULL;
}

static int httpclient_url_parser(httpclient_t *client, const char *url) {
    configASSERT((client != NULL) && (url != NULL));

    uint32_t host_len;
    const char *host_ptr, *path_ptr, *port_ptr;

    /* strip protocol(http or https) */
    if (strncmp(url, "http://", 7) == 0) {
        host_ptr = url + 7;
        client->over_tls = false;
    } else if (strncmp(url, "https://", 8) == 0) {
        host_ptr = url + 8;
        client->over_tls = true;
    } else {
        log_e("httpclient resolve url failed");
        return HTTPCLIENT_ERROR;
    }

    /* parse url path */
    path_ptr = strstr(host_ptr, "/");
    client->reqinfo.pPath = path_ptr ? path_ptr : HTTPCLIENT_DEFAULT_URL_PATH;
    client->reqinfo.pathLen = strlen(client->reqinfo.pPath);

    /* parse url port */
    port_ptr = strstr(host_ptr, ":");
    if (port_ptr && (!path_ptr || (port_ptr < path_ptr))) {
        client->port = strtol(port_ptr + 1, NULL, 10);
    }
    if (client->port == 0) {
        client->port = client->over_tls ? HTTPCLIENT_HTTPS_PORT : HTTPCLIENT_HTTP_PORT;
    }

    /* parse url host */
    if (port_ptr != NULL) {
        host_len = port_ptr - host_ptr;
    } else if (path_ptr != NULL) {
        host_len = path_ptr - host_ptr;
    } else {
        host_len = strlen(host_ptr);
    }

    host_len = host_len < HTTPCLIENT_MAX_HOST_LENGTH ? host_len : HTTPCLIENT_MAX_HOST_LENGTH;
    strncpy(client->host, host_ptr, host_len);
    client->host[host_len] = '\0';
    client->reqinfo.pHost = client->host;
    client->reqinfo.hostLen = host_len;
    client->reqinfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    return HTTPCLIENT_OK;
}

static uint32_t httpclient_get_filesize(httpclient_t *client, bool *accept_range) {
    configASSERT(client != NULL && accept_range != NULL);

    HTTPStatus_t http_status;
    const char *valueloc;
    uint32_t valuelen, fsize = 0;

    client->reqinfo.pMethod = HTTP_METHOD_HEAD;
    client->reqinfo.methodLen = sizeof(HTTP_METHOD_HEAD) - 1;

    http_status = HTTPClient_InitializeRequestHeaders(&client->request, &client->reqinfo);
    if (http_status == HTTPSuccess) {
        http_status = HTTPClient_Send(&client->trans, &client->request, NULL, 0, &client->response, 0);
    }
    if (http_status == HTTPSuccess && client->response.statusCode == 200) {
        fsize = client->response.contentLength;
        log_d("download file size: %u byte", fsize);
        http_status = HTTPClient_ReadHeader(&client->response,
                                            HEADER_FIELD_ACCEPT_RANGE,
                                            HEADER_FIELD_ACCEPT_RANGE_SIZE,
                                            &valueloc,
                                            &valuelen);
        if (http_status == HTTPSuccess && strncmp(valueloc, "bytes", valuelen) == 0) {
            *accept_range = true;
        } else {
            *accept_range = false;
        }
    } else {
        log_e("failed to get download file size");
    }
    return fsize;
}

static int httpclient_partial_download(httpclient_t *client, uint32_t offset, uint32_t size) {
    configASSERT(client != NULL);

    HTTPStatus_t http_status;

    client->reqinfo.pMethod = HTTP_METHOD_GET;
    client->reqinfo.methodLen = sizeof(HTTP_METHOD_GET) - 1;

    http_status = HTTPClient_InitializeRequestHeaders(&client->request, &client->reqinfo);
    if (http_status == HTTPSuccess) {
        http_status = HTTPClient_AddRangeHeader(&client->request, (int32_t)offset, (int32_t)(offset + size - 1));
    }
    if (http_status == HTTPSuccess) {
        http_status = HTTPClient_Send(&client->trans, &client->request, NULL, 0, &client->response, 0);
    }
    if (http_status == HTTPSuccess && client->response.statusCode == 206) {
        return HTTPCLIENT_OK;
    } else {
        return HTTPCLIENT_ERROR;
    }
}

static int httpclient_connect(httpclient_t *client, const char *url) {
    configASSERT(client != NULL && url != NULL);

    int connect_status;

    if (httpclient_url_parser(client, url) != HTTPCLIENT_OK) {
        return HTTPCLIENT_ERROR;
    }

    /* warning: check over_tls must after parse url */
    if (client->over_tls == true) {
        client->context.pParams = httpclient_calloc(1, sizeof(TlsTransportParams_t));
        if (client->context.pParams == NULL) {
            log_e("tls context create failed, no memory for mbedtls!");
            return HTTPCLIENT_ERROR;
        }
    }

    if (client->over_tls == true) {
        connect_status = Network_TLS_Connect(&client->context,
                                             client->host,
                                             client->port,
                                             &root_ca,
                                             HTTPCLIENT_SEND_RECV_TIMEOUT_MS,
                                             HTTPCLIENT_SEND_RECV_TIMEOUT_MS);
        client->trans.recv = Network_TLS_Recv;
        client->trans.send = Network_TLS_Send;
    } else {
        connect_status = Network_Plaintext_Connect(&client->context,
                                                   client->host,
                                                   client->port,
                                                   HTTPCLIENT_SEND_RECV_TIMEOUT_MS,
                                                   HTTPCLIENT_SEND_RECV_TIMEOUT_MS);
        client->trans.recv = Network_Plaintext_Recv;
        client->trans.send = Network_Plaintext_Send;
    }
    log_i("httpclient connect %s, host: %s", connect_status ? "failed" : "success", client->host);

    return connect_status ? HTTPCLIENT_ERROR : HTTPCLIENT_OK;
}

static void httpclient_close(httpclient_t *client) {
    if (client == NULL)
        return;

    /* close socket */
    if (client->context.tcpSocket != INVALID_SOCKET) {
        if (client->over_tls == true) {
            Network_TLS_Disconnect(&client->context);
        } else {
            Network_Plaintext_Disconnect(&client->context);
        }
    }

    /* free tls context, if not null */
    if (client->context.pParams) {
        httpclient_free(client->context.pParams);
    }

    /* free http buffer, if not null */
    if (client->buffer) {
        httpclient_free(client->buffer);
    }

    /* free http client context */
    httpclient_free(client);
}

int httpclient_download(const char *url, const char *fname) {
    int status = HTTPCLIENT_ERROR;

    FRESULT fret;
    uint32_t fsize;
    FIL *newfp = NULL;

    bool accept_range;
    httpclient_t *client;

#if HTTPCLIENT_DOWNLOAD_SPEED_TEST == 1
    TickType_t handshake_start, handshake_end, download_end;
#endif

    if (url == NULL) {
        log_e("httpclient download: url can't be null");
        return status;
    }

#if HTTPCLIENT_DOWNLOAD_SPEED_TEST == 1
    handshake_start = xTaskGetTickCount();
#endif

    client = httpclient_create(HTTPCLIENT_REQ_RESP_BUFFER_SIZE);
    if (client == NULL) {
        return status;
    }

    /* set response pBuffer to internal buffer */
    client->response.pBuffer = client->buffer;
    client->response.bufferLen = client->bufsz;

    newfp = httpclient_calloc(1, sizeof(FIL));
    if (newfp == NULL) {
        log_e("create FIL failed, no memory for fatfs");
        goto exit;
    }

    if (httpclient_connect(client, url) != HTTPCLIENT_OK) {
        goto exit;
    }

#if HTTPCLIENT_DOWNLOAD_SPEED_TEST == 1
    handshake_end = xTaskGetTickCount();
#endif

    fsize = httpclient_get_filesize(client, &accept_range);
    if (fsize == 0 || accept_range == false) {
        goto exit;
    }

    /* get download file name */
    if (fname == NULL) {
        fname = strrchr(url, '/');
        if (fname == NULL || strlen(fname + 1) == 0) {
            fname = HTTPCLIENT_DEFAULT_FILENAME;
        } else {
            fname = fname + 1;
        }
    }

    fret = f_open(newfp, fname, FA_WRITE | FA_CREATE_ALWAYS);
    if (fret == FR_OK) {
        f_lseek(newfp, 0);

        uint32_t bw, btw, errcnt, bytecnt, off;
        int cnt = 0;
        printf("\r\n");
        TickType_t s1, s2;
        for (off = 0, bytecnt = fsize; bytecnt; off += btw, bytecnt -= btw) {
            if (cnt == 0) {
                s1 = xTaskGetTickCount();
            }
            errcnt = 0;
            btw = bytecnt < HTTPCLIENT_RANGE_REQUEST_LENGTH ? bytecnt : HTTPCLIENT_RANGE_REQUEST_LENGTH;
            do {
                if (httpclient_partial_download(client, off, btw) == HTTPCLIENT_OK) {
                    fret = f_write(newfp, client->response.pBody, btw, &bw);
                    if (fret == FR_OK && bw == btw)
                        break;

                    log_e("fatfs write error, errcode: %u, byte write: %u", fret, bw);
                    goto close_fp;
                }
                if (++errcnt > HTTPCLIENT_DOWNLOAD_MAXERROR) {
                    log_e("httpclient download failed");
                    goto close_fp;
                }
            } while (1);
            f_sync(newfp);
            if (++cnt == 10) {
                s2 = xTaskGetTickCount();
                printf("download: %.1f%%  %.2f\r",
                       (((float)(fsize - bytecnt) / (float)fsize) * 100),
                       ((float)btw / 102.4f) / ((float)(s2 - s1) / 1000.0f));
                cnt = 0;
            }

        }

        printf("\r\n");
        /* http download end */
        status = HTTPCLIENT_OK;
        log_i("httpclient download file success");

#if HTTPCLIENT_DOWNLOAD_SPEED_TEST == 1
        download_end = xTaskGetTickCount();
        log_d("httpclient connect  time: %u ms", handshake_end - handshake_start);
        log_d("httpclient download time: %u ms", download_end - handshake_end);
        log_d("httpclient download speed: %.2f kB/s",
              ((float)fsize / 1024) / ((float)(download_end - handshake_start) / 1000));
#endif

    close_fp:
        fret = f_close(newfp);
        if (fret != FR_OK) {
            log_e("fatfs close file failed, errcode: %u", fret);
        }
    } else {
        log_e("fatfs create file failed, errcode: %u", fret);
    }

exit:
    if (newfp != NULL) {
        httpclient_free(newfp);
    }

    httpclient_close(client);
    return status;
}

int httpclient_get(const char *url,
                   uint8_t *resp_buf,
                   uint32_t resp_bufsz,
                   const uint8_t **resp_body,
                   uint32_t *resp_bodylen) {
    int status = HTTPCLIENT_ERROR;
    httpclient_t *client;

    client = httpclient_create(HTTPCLIENT_REQ_BUFFER_SIZE);
    if (client == NULL) {
        return status;
    }

    /* Set response pBuffer to ext resp_buf */
    client->response.pBuffer = resp_buf;
    client->response.bufferLen = resp_bufsz;

    if (httpclient_connect(client, url) != HTTPCLIENT_OK) {
        goto exit;
    }

    /* Init GET header & Send GET request */
    client->reqinfo.pMethod = HTTP_METHOD_GET;
    client->reqinfo.methodLen = sizeof(HTTP_METHOD_GET) - 1;

    if (HTTPClient_InitializeRequestHeaders(&client->request, &client->reqinfo) == HTTPSuccess) {
        if (HTTPClient_Send(&client->trans, &client->request, NULL, 0, &client->response, 0) == HTTPSuccess
            && client->response.statusCode == 200) {

            *resp_body = client->response.pBody;
            *resp_bodylen = client->response.bufferLen;
            status = HTTPCLIENT_OK;
            log_d("httpclient send GET request success");
        } else {
            log_e("httpclient send GET request failed");
        }
    }

exit:
    httpclient_close(client);
    return status;
}
