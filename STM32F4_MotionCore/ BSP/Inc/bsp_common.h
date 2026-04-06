#ifndef BSP_COMMON_H
#define BSP_COMMON_H

typedef enum {
    DRV_OK = 0x00U,
    DRV_ERROR = 0x01U,
    DRV_BUSY = 0x02U,
    DRV_TIMEOUT = 0x03U,
    DRV_INIT_NEEDED = 0x04U,
} DevStatus_t;

#endif /* BSP_COMMON_H */
