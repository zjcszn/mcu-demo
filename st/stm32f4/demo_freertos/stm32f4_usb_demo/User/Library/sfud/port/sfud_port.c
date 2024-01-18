/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#define LOG_TAG "SFUD"
#define LOG_LVL ELOG_LVL_INFO

#include <sfud.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "bsp_spi_dma.h"
#include "elog.h"
#include "FreeRTOS.h"
#include "task.h"

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size) {
    sfud_err result;
    /**
     * add your spi write and read code
     */
    int spi_status;
    if (read_buf != NULL) {
        spi_status = spi_master_write_then_read_dma((uint8_t *)write_buf, write_size, read_buf, read_size);
    } else {
        spi_status = spi_master_write_then_stop_dma((uint8_t *)write_buf, write_size);
    }
    if (spi_status == SPI_ERROR) {
        
        log_e("spi dma error, wrbuf addr: %p size: %u, rdbuf addr: %p, size: %u", write_buf, write_size, read_buf, read_size);
        log_e("thread name: %s", pcTaskGetName(xTaskGetCurrentTaskHandle()));
    }
    result = spi_status == SPI_OK ? SFUD_SUCCESS : SFUD_ERR;
    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    
    /**
     * add your qspi read flash data code
     */
    
    return result;
}
#endif /* SFUD_USING_QSPI */

static void sfud_spi_delayms(void) {
    vTaskDelay(pdMS_TO_TICKS(1));
}

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;
    
    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    
    spi_master_init();
    flash->spi.wr = spi_write_read;     //Required
    flash->retry.delay = sfud_spi_delayms;
    flash->retry.times = 2000;    //Required
    
    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;
    
    /* args point to the first variable parameter */
    va_start(args, format);
    log_d("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    log_d("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;
    
    /* args point to the first variable parameter */
    va_start(args, format);
    log_i("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    log_i("%s\n", log_buf);
    va_end(args);
}
