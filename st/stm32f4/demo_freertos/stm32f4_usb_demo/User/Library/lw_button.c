
#define     LOG_TAG "KEY"
#define     LOG_LVL ELOG_LVL_INFO
#include "lw_button.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/******************************** macro definition ********************************/

#define TICKS_INTERVAL          ( 10U)        // ticks interval
#define TICKS_FILTER            (  2U)        // ticks of button filter
#define TICKS_PRESS_REPEAT      ( 20U)        // ticks of repeat press
#define TICKS_LONG_PRESS        (100U)        // ticks of long press
#define TICKS_LONG_PRESS_HOLD   (  5U)        // ticks of long press hold

#define IDLE_LEVEL_L            (  0U)        // low  level when button actived
#define IDLE_LEVEL_H            (  1U)        // high level when button actived

#define HKEY_POS(i)             (1U << (i))
#define HKEY_STATUS_MASK        (~((~((hkey_status_t)0ULL)) << HKEY_COUNT))
#define HKEY_STATUS_READ(i)     ((hkey_status >> (i)) & 1U)

/**************************** key msg fifo configure ******************************/

#define KEY_MSG_QUEUE_LEN       4

static QueueHandle_t key_msg_handle;

void lwkey_message_init(void) {
    static StaticQueue_t static_queue;
    static uint8_t queue_buffer[KEY_MSG_QUEUE_LEN * sizeof(key_msg_t)];
    key_msg_handle = xQueueCreateStatic(KEY_MSG_QUEUE_LEN, sizeof(key_msg_t), queue_buffer, &static_queue);
    configASSERT(key_msg_handle);
}

/**
 * @brief write button event to fifo buffer
 *
 * @param btn_id  button id
 * @param btn_event button event
 * @return int success on 1 | failed on 0
 */
bool lwkey_message_post(uint8_t id, uint8_t event) {
    key_msg_t msg_tmp;
    msg_tmp.id = id;
    msg_tmp.event = event;
    return xQueueSend(key_msg_handle, &msg_tmp, 0);
}

/**
 * @brief read button event from fifo buffer
 *
 * @param buf pointer of recv buffer
 * @return int success on 1 | failed on 0
 */
bool lwkey_message_fetch(key_msg_t *buf) {
    return xQueueReceive(key_msg_handle, buf, 0);
}

/****************************** hardware configure ******************************/

static hardkey_t hkey_list[HKEY_COUNT] = {
        // [hkey id]   = {filter ticks， action level}
        [KEY0] = {0, IDLE_LEVEL_H},
        [KEY1] = {0, IDLE_LEVEL_H},
        [KEY2] = {0, IDLE_LEVEL_H},
        [WKUP] = {0, IDLE_LEVEL_L},
};

// hkey count <= sizeof(hkey_status_t) * 8
static volatile hkey_status_t hkey_status = 0;
static uint8_t (*hkey_input_read)(hardkey_id_t hkey_id) = NULL;

/****************************** software configure ******************************/

// button list
static softkey_t skey_list[SKEY_COUNT] = {
        // [button id]  = {button id， button type， initval of key state ， hkey 1， hkey 2， initval of ticks}
        [SKEY_KEY0] = {SKEY_KEY0, KEY_TYPE_NORMAL, KEY_STATE_IDLE, KEY0, NULL, 0},
        [SKEY_KEY1] = {SKEY_KEY1, KEY_TYPE_NORMAL, KEY_STATE_IDLE, KEY1, NULL, 0},
        [SKEY_KEY2] = {SKEY_KEY2, KEY_TYPE_NORMAL, KEY_STATE_IDLE, KEY2, NULL, 0},
        [SKEY_WKUP] = {SKEY_WKUP, KEY_TYPE_NORMAL, KEY_STATE_IDLE, WKUP, NULL, 0},

        [SKEY_COM0] = {SKEY_COM0, KEY_TYPE_COMPOSITE, KEY_STATE_IDLE, WKUP, KEY0, 0},
        [SKEY_COM2] = {SKEY_COM2, KEY_TYPE_COMPOSITE, KEY_STATE_IDLE, WKUP, KEY2, 0},
};


/************************** function implementatio ******************************/

/**
 * @brief check hkey status
 *
 * @param hkey_id hkey id
 * @return int status changed on true | status unchanged on false
 */
static inline bool hardkey_status_update(hardkey_id_t hkey_id) {
    int new_status;
    if (hkey_input_read(hkey_id) == hkey_list[hkey_id].idle_level) {
        new_status = 0;
    } else {
        new_status = 1;
    }
    return (HKEY_STATUS_READ(hkey_id) != new_status);
}

/**
 * @brief update hardware layer button status
 * 
 * @param 
 */
static void hardkey_handler(void) {
    hkey_status_t new_status = 0;
    for (int i = 0; i < HKEY_COUNT; i++) {
        if (hardkey_status_update(i) == true) {
            if (++(hkey_list[i].filter_cnt) >= TICKS_FILTER) {
                // reverse hkey status
                new_status |= HKEY_POS(i);
                hkey_list[i].filter_cnt = 0;
            }
        } else {
            hkey_list[i].filter_cnt = 0;
        }
    }
    hkey_status ^= new_status;
    hkey_status &= HKEY_STATUS_MASK;
}

/**
 * @brief get button action
 * 
 * @param  
 * @return int press up on 0 | press down on 1 | press break on 2
 */
static softkey_status_t softkey_status_get(softkey_t *key) {
    if (key->type == KEY_TYPE_NORMAL) {
        // single button: if other button press down, will break target button action
        if (hkey_status & ~(HKEY_POS(key->hkey1))) {
            return KEY_BREAK;
        } else {
            return HKEY_STATUS_READ(key->hkey1);
        }
    } else {
        // composite button: only when hkey 1 press down, can trigger combo button action
        if (hkey_status & ~(HKEY_POS(key->hkey1) | HKEY_POS(key->hkey2))) {
            return KEY_BREAK;
        } else {
            if (HKEY_STATUS_READ(key->hkey1)) {
                return HKEY_STATUS_READ(key->hkey2);
            }
            if (HKEY_STATUS_READ(key->hkey2)) {
                return KEY_BREAK;
            }
            return KEY_UP;
        }
    }
}

/**
 * @brief update state of button fsm
 * 
 * @param btn pointer of button
 */
static void softkey_state_handler(softkey_t *key) {
    int key_stat = softkey_status_get(key);
    switch (key->state) {
        case KEY_STATE_IDLE:
            if (key_stat == KEY_BREAK) {
                key->state = KEY_STATE_BLOCK;
            } else if (key_stat == KEY_DOWN) {
                lwkey_message_post(key->id, KEY_EVENT_PRESS_DOWN);
                key->ticks = 0;
                key->state = KEY_STATE_PRESS_DOWN;
            }
            break;

        case KEY_STATE_BLOCK:
            if (key_stat == KEY_UP) {
                key->state = KEY_STATE_IDLE;
            }
            break;

        case KEY_STATE_PRESS_DOWN:
            if (key_stat == KEY_BREAK) {
                key->state = KEY_STATE_PRESS_BREAK;
            } else if (key_stat == KEY_DOWN) {
                if (++key->ticks >= TICKS_LONG_PRESS) {
                    lwkey_message_post(key->id, KEY_EVENT_LONG_PRESS_START);
                    key->ticks = 0;
                    key->state = KEY_STATE_PRESS_LONG;
                }
            } else {
                lwkey_message_post(key->id, KEY_EVENT_RELEASE);
                key->state = KEY_STATE_IDLE;
            }
            break;

        case KEY_STATE_PRESS_LONG:
            if (key_stat == KEY_BREAK) {
                key->state = KEY_STATE_PRESS_BREAK;
            } else if (key_stat == KEY_DOWN) {
                if (++key->ticks >= TICKS_LONG_PRESS_HOLD) {
                    lwkey_message_post(key->id, KEY_EVENT_LONG_PRESS_HOLD);
                    key->ticks = 0;
                }
            } else {
                lwkey_message_post(key->id, KEY_EVENT_RELEASE);
                key->state = KEY_STATE_IDLE;
            }
            break;

        case KEY_STATE_PRESS_BREAK:
            if (key_stat == KEY_UP) {
                key->state = KEY_STATE_IDLE;
                lwkey_message_post(key->id, KEY_EVENT_RELEASE);
            }
            break;

        default:
            key->state = KEY_STATE_IDLE;
            break;
    }
}

static void softkey_handler(void) {
    for (int i = 0; i < SKEY_COUNT; i++) {
        softkey_state_handler(&skey_list[i]);
    }
}

/**
 * @brief lwkey task function
 *
 */
void lwkey_task(void *args) {
    (void)args;
    const char *key_name_str[] = {"SKEY_KEY0", "SKEY_KEY1", "SKEY_KEY2", "SKEY_WKUP", "SKEY_COM0", "SKEY_COM2"};
    const char *key_event_str[] = {"NULL", "PRESS DOWN", "LONG PRESS START", "LONG PRESS HOLD", "RELEASE"};

    key_msg_t key_msg;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        hardkey_handler();
        softkey_handler();
        if (lwkey_message_fetch(&key_msg) == pdTRUE) {
            configASSERT(key_msg.id < sizeof(key_name_str) / sizeof(key_name_str[0]));
            configASSERT(key_msg.event < sizeof(key_event_str) / sizeof(key_event_str[0]));
            log_i("%s %s", key_name_str[key_msg.id], key_event_str[key_msg.event]);
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TICKS_INTERVAL));
    }
}

/**
 * @brief init function
 *
 * @param hkey_read pointer of callback function
 */
void lwkey_init(hkey_read_func_t hkey_read) {
    hkey_input_read = hkey_read;
    lwkey_message_init();
    xTaskCreate(lwkey_task, "key main task", 128, NULL, 12, NULL);
}



