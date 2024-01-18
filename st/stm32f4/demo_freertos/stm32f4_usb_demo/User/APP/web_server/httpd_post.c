/**
 * @file
 * HTTPD example for simple POST
 */

/*
* Copyright (c) 2017 Simon Goldschmidt
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Simon Goldschmidt <goldsimon@gmx.de>
*
*/

#include "lwip/opt.h"

#include "lwip/apps/httpd.h"
#include "lwip/apps/cookies.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "ff.h"
#include "lwip/sys.h"

#include <stdio.h>
#include <string.h>

#if LWIP_HTTPD_SUPPORT_POST

#define USER_PASS_BUFSIZE 16

#define UPLOAD_ROOT_PATH    "1:/"

#define FORM_DATA_FIELD_FILE_NAME           "name=\"FileName\"\r\n\r\n"
#define FORM_DATA_FIELD_FILE_NAME_LENGTH    (sizeof(FORM_DATA_FIELD_FILE_NAME) - 1)
#define FORM_DATA_FIELD_FILE_SIZE           "name=\"FileSize\"\r\n\r\n"
#define FORM_DATA_FIELD_FILE_SIZE_LENGTH    (sizeof(FORM_DATA_FIELD_FILE_SIZE) - 1)
#define FORM_DATA_FIELD_FILE_DATA           "name=\"FileData\";"
#define FORM_DATA_FIELD_FILE_DATA_LENGTH    (sizeof(FORM_DATA_FIELD_FILE_DATA) - 1)

typedef enum {
    POST_NONE,
    POST_LOGIN,
    POST_UPLOAD_FIRMWARE,
} post_index_t;

/*--------------------------------------------- upload handler ---------------------------------------------*/

typedef enum {
    UPLOAD_STATE_IDLE,
    UPLOAD_STATE_PARSE_HEADER,
    UPLOAD_STATE_PARSE_DATA,
    UPLOAD_STATE_ABORT,
    UPLOAD_STATE_COMPLETE,
} upload_state_t;

typedef struct {
    void *fd;       /* pointer of FIL struct  */
    char *fname;    /* path of upload file */
    u32_t fsize;    /* size of upload file */
    u16_t state;    /* upload state */
} upload_handle_t;

static upload_handle_t upload_handle;

static err_t parse_formdata_header(struct pbuf *p, u16_t *fdata_offset) {
    u16_t start, end;
    /* parse file name */
    start = pbuf_memfind(p, FORM_DATA_FIELD_FILE_NAME, FORM_DATA_FIELD_FILE_NAME_LENGTH, 0);
    if (start != 0xFFFF) {
        start += FORM_DATA_FIELD_FILE_NAME_LENGTH;
        end = pbuf_memfind(p, "\r\n", sizeof("\r\n") - 1, start);
        if (end != 0xFFFF) {
            u16_t fname_len = sizeof(UPLOAD_ROOT_PATH) + (end - start);
            upload_handle.fname = mem_malloc(fname_len);
            LWIP_ASSERT("malloc memory of fname failed", upload_handle.fname != NULL);
            snprintf(upload_handle.fname, fname_len, UPLOAD_ROOT_PATH"%s", (char *)(p->payload + start));
        }
    }
    if (!upload_handle.fname) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("parse file name failed!\r\n"));
        return ERR_VAL;
    }

    /* parse file size */
    start = pbuf_memfind(p, FORM_DATA_FIELD_FILE_SIZE, FORM_DATA_FIELD_FILE_SIZE_LENGTH, end);
    if (start != 0XFFFF) {
        start += FORM_DATA_FIELD_FILE_SIZE_LENGTH;
        end = pbuf_memfind(p, "\r\n", sizeof("\r\n") - 1, start);
        if (end != 0xFFFF) {
            upload_handle.fsize = strtol(p->payload + start, NULL, 10);
        }
    }
    if (!upload_handle.fsize) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("parse file size failed!\r\n"));
        return ERR_VAL;
    }

    /* parse file data */
    start = pbuf_memfind(p, FORM_DATA_FIELD_FILE_DATA, FORM_DATA_FIELD_FILE_DATA_LENGTH, end);
    if (start != 0xFFFF) {
        start += FORM_DATA_FIELD_FILE_DATA_LENGTH;
        start = pbuf_memfind(p, "\r\n\r\n", sizeof("\r\n\r\n") - 1, start);
        if (start != 0xFFFF) {
            start += sizeof("\r\n\r\n") - 1;
        }
    }
    if (start == 0xFFFF) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("parse file data failed!\r\n"));
        return ERR_VAL;
    }

    upload_handle.fd = mem_malloc(sizeof(FIL));
    LWIP_ASSERT("malloc file handle failed", upload_handle.fd != NULL);
    if (f_open(upload_handle.fd, upload_handle.fname, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
        LWIP_DEBUGF(HTTPD_DEBUG, ("create file failed!\r\n"));
        return ERR_VAL;
    }

    *fdata_offset = start;
    return ERR_OK;
}

static err_t upload_handler_on_begin(void *conn, char *response_uri, u16_t response_uri_len, u8_t *post_auto_wnd) {
    if (httpd_cookie_auth(conn) == 0 || upload_handle.state != UPLOAD_STATE_IDLE) {
        snprintf(response_uri, response_uri_len, "/json/status_denied.json");
        LWIP_DEBUGF(HTTPD_DEBUG, ("upload denied!\r\n"));
        return ERR_VAL;
    }

    *post_auto_wnd = 0;
    upload_handle.state = UPLOAD_STATE_PARSE_HEADER;
    return ERR_OK;
}

static err_t upload_handler_on_receive(struct pbuf *p) {
    UINT btw, bw;
    u16_t fdata_offset = 0;

    switch (upload_handle.state) {
        case UPLOAD_STATE_PARSE_HEADER:
            if (parse_formdata_header(p, &fdata_offset) == ERR_OK) {
                upload_handle.state = UPLOAD_STATE_PARSE_DATA;
            } else {
                upload_handle.state = UPLOAD_STATE_ABORT;
                return ERR_VAL;
            }

        case UPLOAD_STATE_PARSE_DATA:
            btw = p->len - fdata_offset;
            btw = upload_handle.fsize > btw ? btw : upload_handle.fsize;
            if (f_write(upload_handle.fd, p->payload + fdata_offset, btw, &bw) == FR_OK && bw == btw) {
                upload_handle.fsize -= btw;
                if (upload_handle.fsize != 0) {
                    return ERR_OK;
                } else {
                    upload_handle.state = UPLOAD_STATE_COMPLETE;
                    return ERR_VAL;
                }
            } else {
                LWIP_DEBUGF(HTTPD_DEBUG, ("write file failed!\r\n"));
            }

        default:
            upload_handle.state = UPLOAD_STATE_ABORT;
            return ERR_VAL;
    }
}

static void upload_handler_on_finish(char *response_uri, u16_t response_uri_len) {
    if (upload_handle.state == UPLOAD_STATE_COMPLETE) {
        snprintf(response_uri, response_uri_len, "/json/status_ok.json");
        LWIP_DEBUGF(HTTPD_DEBUG, ("receive upload post success!\r\n"));
    } else {
        f_unlink(upload_handle.fname);
        snprintf(response_uri, response_uri_len, "/json/status_fail.json");
        LWIP_DEBUGF(HTTPD_DEBUG, ("receive upload post failed!\r\n"));
    }
    /* release heap memory */
    if (upload_handle.fd) {
        f_close(upload_handle.fd);
        mem_free(upload_handle.fd);
    }
    if (upload_handle.fname) {
        mem_free(upload_handle.fname);
    }
    memset(&upload_handle, 0, sizeof(upload_handle_t));
}

/*--------------------------------------------- login handler ---------------------------------------------*/

static bool is_auth_ok;

static err_t login_handler_on_begin(char *response_uri, u16_t response_uri_len) {
    is_auth_ok = false;
    snprintf(response_uri, response_uri_len, "/login.cgi?login=false");

    return ERR_OK;
}

static err_t login_handler_on_receive(struct pbuf *p) {
    u16_t token_user = pbuf_memfind(p, "usr=", 4, 0);
    u16_t token_pass = pbuf_memfind(p, "pwd=", 4, 0);
    if ((token_user != 0xFFFF) && (token_pass != 0xFFFF)) {
        u16_t value_user = token_user + 4;
        u16_t value_pass = token_pass + 4;
        u16_t len_user = 0;
        u16_t len_pass = 0;
        u16_t tmp;
        /* find user len */
        tmp = pbuf_memfind(p, "&", 1, value_user);
        if (tmp != 0xFFFF) {
            len_user = tmp - value_user;
        } else {
            len_user = p->tot_len - value_user;
        }
        /* find pass len */
        tmp = pbuf_memfind(p, "&", 1, value_pass);
        if (tmp != 0xFFFF) {
            len_pass = tmp - value_pass;
        } else {
            len_pass = p->tot_len - value_pass;
        }
        if ((len_user > 0) && (len_user < USER_PASS_BUFSIZE) &&
            (len_pass > 0) && (len_pass < USER_PASS_BUFSIZE)) {
            /* provide contiguous storage if p is a chained pbuf */
            char buf_user[USER_PASS_BUFSIZE];
            char buf_pass[USER_PASS_BUFSIZE];
            char *user = (char *)pbuf_get_contiguous(p, buf_user, sizeof(buf_user), len_user, value_user);
            char *pass = (char *)pbuf_get_contiguous(p, buf_pass, sizeof(buf_pass), len_pass, value_pass);
            if (user && pass) {
                user[len_user] = 0;
                pass[len_pass] = 0;
                if (!strcmp(user, "admin") && !strcmp(pass, "password")) {
                    /* user and password are correct, create a "session" */
                    is_auth_ok = true;
                    return ERR_OK;
                }
            }
        }
    }
    return ERR_VAL;
}

static void login_handler_on_finish(void *connection, char *response_uri, u16_t response_uri_len) {
    if (is_auth_ok == true) {
        httpd_set_login_status(connection);
        snprintf(response_uri, response_uri_len, "/login.cgi?login=true");
    } else {
        snprintf(response_uri, response_uri_len, "/login.cgi?login=false");
    }
}

/*--------------------------------------------- httpd post ---------------------------------------------*/

typedef struct {
    void *conn;
    post_index_t post_idx;
} post_handle_t;

static post_handle_t post_handle;

err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd) {
    if (post_handle.conn != NULL) {
        return ERR_VAL;
    } else {
        post_handle.conn = connection;
    }

    if (!memcmp(uri, "/login.cgi", sizeof("/login.cgi"))) {
        post_handle.post_idx = POST_LOGIN;
        return login_handler_on_begin(response_uri, response_uri_len);
    } else if (!memcmp(uri, "/upload", sizeof("/upload"))) {
        post_handle.post_idx = POST_UPLOAD_FIRMWARE;
        return upload_handler_on_begin(connection, response_uri, response_uri_len, post_auto_wnd);
    }
    return ERR_VAL;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p) {
    err_t ret = ERR_VAL;

    if (connection == post_handle.conn) {
        switch (post_handle.post_idx) {
            case POST_LOGIN:
                ret = login_handler_on_receive(p);
                break;

            case POST_UPLOAD_FIRMWARE:
                ret = upload_handler_on_receive(p);
                break;

            default:
                break;
        }
    }

    /* this function must ALWAYS free the pbuf it is passed or it will leak memory */
    pbuf_free(p);
    return ret;
}

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len) {
    /* default page is "login failed" */
    if (connection == post_handle.conn) {
        switch (post_handle.post_idx) {
            case POST_LOGIN:
                login_handler_on_finish(connection, response_uri, response_uri_len);
                break;

            case POST_UPLOAD_FIRMWARE:
                upload_handler_on_finish(response_uri, response_uri_len);
                break;

            default:
                break;
        }
        memset(&post_handle, 0, sizeof(post_handle_t));
    }
}

#endif /* LWIP_HTTPD_EXAMPLE_SIMPLEPOST*/
