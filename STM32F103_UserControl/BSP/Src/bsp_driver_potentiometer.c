#include "bsp_driver_potentiometer.h"

/* private variables */
static ADC_HandleTypeDef* local_padc1 = NULL;
volatile static uint16_t adc_measurements_buf[BSP_POTENTIOMETER_BUFFER_RESOLUTION];

/* Initialize ADC1 and start measurements with DMA */
DevStatus_t BSP_POTENTIOMETER_Init(ADC_HandleTypeDef* padc1)
{
	if(padc1 != NULL)
	{
		local_padc1 = padc1;
		memset((uint32_t*)adc_measurements_buf, 0, sizeof(adc_measurements_buf));
		DevStatus_t ret = (DevStatus_t)HAL_ADC_Start_DMA(local_padc1, (uint32_t*)adc_measurements_buf, (uint32_t)BSP_POTENTIOMETER_BUFFER_RESOLUTION);

		return ret;
	}
	return DRV_INIT_NEEDED;
}

/* Calculate average and turn into percent */
DevStatus_t BSP_POTENTIOMETER_GetMotorTargetSpeed(uint32_t* motor_target_speed_percent)
{
	uint32_t average_motor_target_speed = 0;
	uint32_t count_of_valid_numbers = 0;

	/* Calculate average value of resistor */
	for(uint32_t i = 0; i < BSP_POTENTIOMETER_BUFFER_RESOLUTION; ++i)
	{
		if(adc_measurements_buf[i] != 0)
		{
			average_motor_target_speed += adc_measurements_buf[i];
			count_of_valid_numbers++;
		}
	}

	if(count_of_valid_numbers == 0U)
	{
		return DRV_ERROR;
	}
	average_motor_target_speed /= count_of_valid_numbers;

	/* Turn value into percent from 0 - 4095 to 0 - 100% */
	if(BSP_POTENTIOMETER_MEAS_RESOLUTION == 0U)
	{
		return DRV_ERROR;
	}
	average_motor_target_speed = (uint32_t)(( average_motor_target_speed * 100U ) / BSP_POTENTIOMETER_MEAS_RESOLUTION);

	*motor_target_speed_percent = average_motor_target_speed;

	return DRV_OK;
}
