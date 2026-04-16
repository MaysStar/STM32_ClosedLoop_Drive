#ifndef BSP_DRIVER_RTC_H
#define BSP_DRIVER_RTC_H

#include <bsp_common.h>
#include "stm32f1xx_hal.h"

DevStatus_t BSP_RTC_Init(RTC_HandleTypeDef* prtc);
DevStatus_t BSP_RTC_SetDateTime(RTC_DateTypeDef date, RTC_TimeTypeDef time);

DevStatus_t BSP_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime);

#endif /* BSP_DRIVER_RTC_H */
