/**
 * @file
 * HTTPD custom file system example
 *
 * This file demonstrates how to add support for an external file system to httpd.
 * It provides access to the specified root directory and uses stdio.h file functions
 * to read files.
 *
 * ATTENTION: This implementation is *not* secure: no checks are added to ensure
 * files are only read below the specified root directory!
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
#include "httpd_userfs.h"

#include "lwip/apps/fs.h"
#include "lwmem/lwmem.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ff.h"
#include "lwip/def.h"

/** define LWIP_HTTPD_EXAMPLE_CUSTOMFILES_LIMIT_READ to the number of bytes
 * to read to emulate limited transfer buffers and don't read whole files in
 * one chunk.
 * WARNING: lowering this slows down the connection!
 */
#ifndef LWIP_HTTPD_CUSTOMFILES_LIMIT_READ
#define LWIP_HTTPD_CUSTOMFILES_LIMIT_READ 4096
#endif

struct fs_custom_data {
  FIL f;
};

const char *fs_ex_root_dir;

void fs_ex_init(const char *root_dir) {
    fs_ex_root_dir = root_dir;
}

int fs_open_custom(struct fs_file *file, const char *name) {
    FRESULT fret;
    int is_open = false;
    char fpath[256];
    struct fs_custom_data *data;
    
    LWIP_ASSERT("root dir not set", fs_ex_root_dir != NULL);
    data = (struct fs_custom_data *)fs_malloc(sizeof(struct fs_custom_data));
    LWIP_ASSERT("out of memory?", data != NULL);
    
    /* get full path of file */
    snprintf(fpath, 255, "%s%s", fs_ex_root_dir, name);
    
    fret = f_open(&data->f, fpath, FA_READ | FA_OPEN_EXISTING);
    if (fret != FR_OK)
        goto error;
    
    is_open = true;
    if (f_size(&data->f) == 0) {
        goto error;
    }
    
    fret = f_lseek(&data->f, 0);
    if (fret != FR_OK)
        goto error;
    
    memset(file, 0, sizeof(struct fs_file));
    file->flags = FS_FILE_FLAGS_HEADER_PERSISTENT;
    file->len = (int)f_size(&data->f);
    file->pextension = data;
    return 1;

error:
    if (is_open == true) {
        f_close(&data->f);
    }
    fs_free(data);
    return 0;
}

void fs_close_custom(struct fs_file *file) {
    if (file && file->pextension) {
        struct fs_custom_data *data = (struct fs_custom_data *)file->pextension;
        f_close(&data->f);
        fs_free(file->pextension);
        file->pextension = NULL;
    }
}

int fs_read_custom(struct fs_file *file, char *buffer, int count) {
    FRESULT fret;
    int br, btr;
    
    struct fs_custom_data *data = (struct fs_custom_data *)file->pextension;
    LWIP_ASSERT("data not set", data != NULL);
    
    btr = LWIP_MIN(count, LWIP_HTTPD_CUSTOMFILES_LIMIT_READ);
    
    fret = f_read(&data->f, buffer, btr, (unsigned int *)&br);
    if (fret == FR_OK && br) {
        file->index += (int)br;
        return br;
    }
    /* Return FS_READ_EOF if all bytes have been read */
    return FS_READ_EOF;
}

void *fs_state_init(struct fs_file *file, const char *name) {
    char *ret = NULL;
    LWIP_UNUSED_ARG(file);
    LWIP_UNUSED_ARG(name);
    if (strncmp(name, "/login.cgi", 10) == 0) {
        ret = (char *)fs_malloc(sizeof(struct fs_state));
        if (ret) {
            memset(ret, 0, sizeof(struct fs_state));
            ((struct fs_state*)ret)->login = true;
        }
    }
    return ret;
}

void fs_state_free(struct fs_file *file, void *state) {
    LWIP_UNUSED_ARG(file);
    if (state != NULL) {
        fs_free(state);
    }
}