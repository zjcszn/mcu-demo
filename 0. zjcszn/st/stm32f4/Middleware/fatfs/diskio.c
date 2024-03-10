/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"            /* Obtains integer types */
#include "diskio.h"        /* Declarations of disk functions */
#include "bsp_sdcard.h"
#include "sfud.h"
#include "sfud_cfg.h"
#include "sfud_def.h"

/* Definitions of physical drive number for each drive */
#define DEV_SD          0    /* Example: Map Ramdisk to physical drive 0 */
#define DEV_FLASH       1

#define SDIO_POLLING_MODE           0
#define SDIO_DMA_TIMEOUT            0xFFFF

#define FLASH_SECTOR_SIZE           4096
#define SDCARD_BLOCK_SIZE           512

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv        /* Physical drive nmuber to identify the drive */
) {
    DSTATUS stat = STA_NOINIT;
    switch (pdrv) {
        case DEV_SD :
            stat &= ~STA_NOINIT;
            break;

        case DEV_FLASH:
            stat &= ~STA_NOINIT;
            break;

        default:
            break;
    }
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv                /* Physical drive nmuber to identify the drive */
) {
    DSTATUS stat = STA_NOINIT;
    switch (pdrv) {
        case DEV_SD :
            BSP_SD_DeInit();
            if (BSP_SD_Init() == MSD_OK) {
                stat &= ~STA_NOINIT;
            }
            break;

        case DEV_FLASH:
            stat &= ~STA_NOINIT;
            break;

        default:
            break;
    }
    return stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    BYTE *buff,        /* Data buffer to store read data */
    LBA_t sector,    /* Start sector in LBA */
    UINT count        /* Number of sectors to read */
) {
    DRESULT ret = RES_ERROR;

    switch (pdrv) {
        case DEV_SD :
            if (BSP_SD_ReadBlocks_DMA((uint32_t *)buff, sector, count) == MSD_OK) {
                uint32_t timeout = SDIO_DMA_TIMEOUT;
                while (BSP_SD_GetCardState() != SD_TRANSFER_OK) {
                    if (timeout-- == 0)
                        return RES_ERROR;
                }
                ret = RES_OK;
            }
            break;

        case DEV_FLASH:
            if (sfud_read(sfud_get_device(SFUD_W25Q_DEVICE_INDEX), sector * 4096, count * 4096, buff) == SFUD_SUCCESS) {
                ret = RES_OK;
            }
            break;

        default:
            ret = RES_PARERR;
            break;
    }

    return ret;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
    BYTE pdrv,            /* Physical drive nmuber to identify the drive */
    const BYTE *buff,    /* Data to be written */
    LBA_t sector,        /* Start sector in LBA */
    UINT count            /* Number of sectors to write */
) {
    DRESULT ret = RES_ERROR;

    switch (pdrv) {

        case DEV_SD :
            if (BSP_SD_WriteBlocks_DMA((uint32_t *)buff, sector, count) == MSD_OK) {
                uint32_t timeout = SDIO_DMA_TIMEOUT;
                while (BSP_SD_GetCardState() != SD_TRANSFER_OK) {
                    if (timeout-- == 0)
                        return RES_ERROR;
                }
                ret = RES_OK;
            }
            break;

        case DEV_FLASH:
            if (sfud_erase_write(sfud_get_device(SFUD_W25Q_DEVICE_INDEX), sector * 4096, count * 4096, buff)
                == SFUD_SUCCESS) {
                ret = RES_OK;
            }
            break;

        default:
            ret = RES_PARERR;
            break;
    }

    return ret;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv,          /* Physical drive nmuber (0..) */
    BYTE cmd,           /* Control code */
    void *buff          /* Buffer to send/receive control data */
) {

    DRESULT ret = RES_ERROR;

    switch (pdrv) {
        case DEV_SD :
            if (cmd == GET_SECTOR_SIZE) {
                *(WORD *)buff = SDCARD_BLOCK_SIZE;
            }
            ret = RES_OK;
            break;

        case DEV_FLASH:
            if (cmd == GET_SECTOR_SIZE) {
                *(WORD *)buff = FLASH_SECTOR_SIZE;
            }
            ret = RES_OK;
            break;

        default:
            ret = RES_PARERR;
            break;
    }

    return ret;
}