#ifndef __DEV_UART_H__
#define __DEV_UART_H__

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "fifo.h"

#include <stdbool.h>

#ifdef __cplusplus
extern C{
#endif

#ifndef _IO
#define _IO volatile
#endif

#define UART_RX_DMA_BUFSZ          128U
#define UART_RX_FIFO_BUFSZ         1024U
#define UART_TX_DMA_BUFSZ          128U
#define UART_TX_FIFO_BUFSZ         1024U

#define BLOCKTIME_DMA              pdMS_TO_TICKS(5000)
#define BLOCKTIME_UART             pdMS_TO_TICKS(1000)

typedef void (*uart_cbf_t)(void);

typedef struct {
    struct _fifo rx_fifo;
    struct _fifo tx_fifo;
    uint8_t *dmarx_buf;
    uint32_t dmarx_bufsz;
    uint32_t dmarx_idx;                 /* read idx for dmarx buffer */
    uint8_t *dmatx_buf;
    uint32_t dmatx_bufsz;
    uart_cbf_t rx_indi;                 /* callback of rx indicate */
    SemaphoreHandle_t dmatx_sem;        /* semp for tx dma trans complete notice */
    SemaphoreHandle_t write_sem;        /* semp for tx fifo buffer not empty */
    SemaphoreHandle_t wr_lock;          /* lock for write to  tx fifo */
    SemaphoreHandle_t rd_lock;          /* lock for read from rx fifo */
} dev_uart_t;

extern void dev_uart_init(void);

extern uint32_t uart_write(const void *src, uint32_t len);

extern uint32_t uart_read(void *dst, uint32_t len);

extern void *uart_set_rx_indi(uart_cbf_t rx_indi);

extern void uart_clear_rx_buffer(void);

extern void uart_clear_tx_buffer(void);

extern bool uart_write_lock(void);

extern bool uart_write_unlock(void);

extern bool uart_read_lock(void);

extern bool uart_read_unlock(void);

#ifdef __cplusplus
}
#endif

#endif