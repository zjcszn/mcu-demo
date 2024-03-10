//
// Created by zjcszn on 2023/8/18.
//

#include "http_server.h"
#include "ff.h"
#include "lwip/apps/fs.h"
#include "httpd_cgi.h"
#include "lwip/apps/httpd.h"
#include "lwmem/lwmem.h"
#include "httpd_userfs.h"
#include "httpd_ssi.h"
#include "lwip/apps/cookies.h"

void bsp_httpd_init(void) {
    httpd_cookie_init();
    fs_ex_init("1:/www");
    httpd_init();
    httpd_cgi_init();
    ssi_ex_init();
}