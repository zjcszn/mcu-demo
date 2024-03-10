#include <stdio.h>
#include <time.h>

#include "bsp_rtc.h"
#include "stm32f4xx_ll_usart.h"

__asm(".global __use_no_semihosting");

struct __FILE {
  int handle;
};

FILE __stdout, __stdin, __stderr;   // NOLINT(misc-non-copyable-objects)

int fputc(int ch, FILE *f) {
    while (!LL_USART_IsActiveFlag_TXE(USART1));
    LL_USART_TransmitData8(USART1, ch);
    return (ch);
}

int fgetc(FILE *f) {
    char ch;
    while (!LL_USART_IsActiveFlag_RXNE(USART1));
    ch = LL_USART_ReceiveData8(USART1);
    return ((int)ch);
}

int ferror(FILE *f) {
    (void)f;
    return EOF;
}

void _ttywrch(int ch) {
    (void)ch;
}

void _sys_exit(int x) {
    (void)x;
}

time_t time(time_t *timer) {
    time_t now = 0;
    if (timer) {
        now = *timer;
        rtc_time_set(now);
    } else {
        now = rtc_time_get();
    }
    return now;
}
