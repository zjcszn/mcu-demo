//
// Created by zjcszn on 2023/8/20.
//

#include "lwip/apps/fs.h"
#include "lwip/apps/httpd.h"
#include "httpd_cgi.h"
#include "ff.h"
#include "lwmem/lwmem.h"
#include "bsp_led.h"
#include <string.h>

static const char *cgi_login_handler(void *connection, int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    for (int i = 0; i < iNumParams; i++) {
        if ((strncmp(pcParam[i], "login", 5) == 0) && (strncmp(pcValue[i], "true", 4) == 0)) {
            log_i("http client login success");
            return "/home.shtml";
        }
    }
    return "/index.shtml";
}

static const char *cgi_led_handler(void *connection, int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    if (httpd_cookie_auth(connection) == 0) {
        return "./json/status_denied.json";
    }
    for (int i = 0; i < iNumParams; i++) {
        if (strncmp(pcParam[i], "led", 3) == 0) {
            if (strncmp(pcValue[i], "on", 2) == 0) {
                bsp_led_control(LED_RED, 1);
            } else if (strncmp(pcValue[i], "off", 3) == 0) {
                bsp_led_control(LED_RED, 0);
            }
        }
    }
    return "./json/status_ok.json";
}

static const tCGI cgi_handlers[] = {
        {
                "/login.cgi",
                cgi_login_handler,
        },
        {
                "/led.cgi",
                cgi_led_handler,
        }
};

void httpd_cgi_init(void) {
    http_set_cgi_handlers(cgi_handlers, 2);
}

