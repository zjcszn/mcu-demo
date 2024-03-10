/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */
#include "shell_port.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "shell.h"
#include "dev_uart.h"
#include "elog.h"

Shell msh;
char shellBuffer[512];
static TaskHandle_t msh_task;

#if SHELL_USING_LOCK
static SemaphoreHandle_t msh_mutex;
#endif

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len) {
    return (short)uart_write(data, len);
}

/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len) {
    return (short)uart_read(data, len);
}

#if SHELL_USING_LOCK
/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell) {
    xSemaphoreTakeRecursive(msh_mutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellUnlock(Shell *shell) {
    xSemaphoreGiveRecursive(msh_mutex);
    return 0;
}
#endif

void userShellRecvNotice(void) {
    vTaskNotifyGiveFromISR(msh_task, NULL);
}

/**
 * @brief 用户shell初始化
 * 
 */
void bsp_shell_init(void) {
    dev_uart_init();
    msh.write = userShellWrite;
    msh.read = userShellRead;
#if SHELL_USING_LOCK
    msh_mutex = xSemaphoreCreateRecursiveMutex();
    msh.lock = userShellLock;
    msh.unlock = userShellUnlock;
#endif
    uart_set_rx_indi((uart_cbf_t)userShellRecvNotice);
    shellInit(&msh, shellBuffer, 512);
    msh.init_ok = 1;
    
    if (xTaskCreate(shellTask, "shell", 1024, &msh, 10, &msh_task) != pdPASS) {
        log_e("shell task creat failed");
    }
}

