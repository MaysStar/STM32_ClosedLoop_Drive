#include "bsp_driver_rtc.h"

static RTC_HandleTypeDef* local_prtc = NULL;

/* RTC initialization */
DevStatus_t BSP_RTC_Init(RTC_HandleTypeDef* prtc)
{
	if(prtc != NULL)
	{
		local_prtc = prtc;
		return DRV_OK;
	}
	return DRV_INIT_NEEDED;
}

/* RTC set current time and date*/
DevStatus_t BSP_RTC_SetDateTime(RTC_DateTypeDef date, RTC_TimeTypeDef time)
{
	DevStatus_t ret;
	if(local_prtc != NULL)
	{
		ret = (DevStatus_t)HAL_RTC_SetTime(local_prtc, &time, RTC_FORMAT_BIN);

		if(ret != DRV_OK)
		{
			return ret;
		}
		ret = (DevStatus_t)HAL_RTC_SetDate(local_prtc, &date, RTC_FORMAT_BIN);
	}
	return ret;
}

/* RTC get current time and date*/
DevStatus_t BSP_RTC_GetDataDateTime(RTC_DateTypeDef* pdate, RTC_TimeTypeDef* ptime)
{
	DevStatus_t ret;
	if(local_prtc != NULL && pdate != NULL && ptime != NULL)
	{
		/* First always read time and then date */
		ret = (DevStatus_t)HAL_RTC_GetTime(local_prtc, ptime, RTC_FORMAT_BIN);

		if(ret != DRV_OK)
		{
			return ret;
		}
		ret = (DevStatus_t)HAL_RTC_GetDate(local_prtc, pdate, RTC_FORMAT_BIN);
	}

	return ret;
}
