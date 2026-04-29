#ifndef BSP_COMMON_H
#define BSP_COMMON_H

#include "stdint.h"
#include "stm32f1xx_hal.h"

typedef enum {
    DRV_OK = 0x00U,
    DRV_ERROR = 0x01U,
    DRV_BUSY = 0x02U,
    DRV_TIMEOUT = 0x03U,
    DRV_INIT_NEEDED = 0x04U,
} DevStatus_t;

typedef void (*bsp_callback_fun)(void);

void BSP_COMMON_PowerOff(void);

#endif /* BSP_COMMON_H */
