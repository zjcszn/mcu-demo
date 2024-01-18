#ifndef __BOOT_PARAMS_H__
#define __BOOT_PARAMS_H__

#include <stdint.h>
#include <stdbool.h>


/** 
 * @brief Constant parameter buffer identifier. This value is always located as the
 *        start of the buffer to validate the the RAM contains valid shared parameters.
 */
#define BOOT_PARAMS_MAGIC_WORD                (0xCE42E7A2U)


/** 
 * @brief Configuration macro for specifying the size of the data inside the parameter
 *        buffer. This is the length in bytes of the actual parameter data, so
 *        excluding the bufferId and checksum.
 */
#define BOOT_PARAMS_DATA_LENGTH               (3U)

#define BOOT_PARAMS_CHECKSUM_CALC_LENGTH      ((BOOT_PARAMS_DATA_LENGTH + 1U) << 1U)



typedef struct
{
    uint32_t magic_word;
    uint32_t data[BOOT_PARAMS_DATA_LENGTH];
    uint16_t checksum;
} boot_params_t;

typedef enum
{
    BOOT_FLAG_NONE        = 0x0000,
    BOOT_FLAG_JMP_TO_APP  = 0xABCD,
    BOOT_FLAG_JMP_TO_IAP  = 0x1234,
    BOOT_FLAG_UPDATE_APP  = 0X5678,
    BOOT_FLAG_MINI_SHELL  = 0x254E,
} boot_flag_t;

typedef enum
{
    PARAM_INDEX_BOOT_FLAG,
    /* other flags index */
} param_index_t;

bool boot_params_init(void);
void boot_params_reset(void);
bool boot_params_read(uint32_t idx, uint32_t *value);
bool boot_params_write(uint32_t idx, uint32_t value);


#endif