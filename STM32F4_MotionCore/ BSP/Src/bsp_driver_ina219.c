#include <bsp_driver_ina219.h>

/* private variables for i2c driver */
typedef void (*I2C_TxRxCpltCallbak)(void);

static I2C_TxRxCpltCallbak I2C1RxCpltCallbak = NULL;
static I2C_HandleTypeDef* local_pi2c1 = NULL;

static uint8_t curr_reg[2];
static uint8_t pow_reg[2];
static uint8_t volt_reg[2];

/* Initialize I2C for driver and set 12-bit resolution for INA219 */
DevStatus_t BSP_INA219_Init(I2C_HandleTypeDef* hi2c1)
{
	if(hi2c1 == NULL)
	{
		return DRV_INIT_NEEDED;
	}
	local_pi2c1 = hi2c1;

	/* Set 0.1mA LSB in calibration register so cal = 4096 or 0x1000 */
	uint8_t cal_reg[2];
	cal_reg[0] = 0x10;
	cal_reg[1] = 0x00;

	DevStatus_t ret = (DevStatus_t)HAL_I2C_Mem_Write(local_pi2c1, (BSP_INA219_ADDR << 1), INA219_REG_CALIBRATION, 1, cal_reg, 2, 100);

	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	/* Set 32 samples for voltage and current in Configuration Register */
	uint8_t conf_reg[2];
	conf_reg[0] = 0x3E;
	conf_reg[1] = 0xEF;

	ret = (DevStatus_t)HAL_I2C_Mem_Write(local_pi2c1, (BSP_INA219_ADDR << 1), INA219_REG_CONFIG, 1, conf_reg, 2, 100);

	if(ret != DRV_OK)
	{
		return DRV_INIT_NEEDED;
	}

	return DRV_OK;
}

/* Read current from INA219 */
DevStatus_t BSP_INA219_ReadCurrent(void)
{
	/* Read current register */
	return (DevStatus_t)HAL_I2C_Mem_Read_DMA(local_pi2c1, (BSP_INA219_ADDR << 1), INA219_REG_CURRENT, 1, curr_reg, 2);
}

/* Get current */
float BSP_INA219_GetCurrent(void)
{
	int16_t raw_current = (int16_t)((curr_reg[0] << 8) | curr_reg[1]);

	float current_A;

	/* Multiply by LSB or step */
	current_A = (((float)raw_current) * 0.1f) / 1000.0f;

	return current_A;
}

/* Read power from INA219*/
DevStatus_t BSP_INA219_ReadPower(void)
{
	/* Read power register */
	return (DevStatus_t)HAL_I2C_Mem_Read_DMA(local_pi2c1, (BSP_INA219_ADDR << 1), INA219_REG_POWER, 1, pow_reg, 2);
}

/* Get power*/
float BSP_INA219_GetPower(void)
{
	int16_t raw_power = (int16_t)((pow_reg[0] << 8) | pow_reg[1]);

	/* Calculate power Power_LSB = 20 Current_LSB
	 * Power_LSB = 2
	 */

	float power_W = (((float)raw_power) * 2.0f) / 1000.0f;

	return power_W;
}

/* Read voltage from INA219 */
DevStatus_t BSP_INA219_ReadVoltage(void)
{
	/* Read voltage register */
	return (DevStatus_t)HAL_I2C_Mem_Read_DMA(local_pi2c1, (BSP_INA219_ADDR << 1), INA219_REG_BUSVOLT, 1, volt_reg, 2);
}

/* Get voltage */
float BSP_INA219_GetVoltage(void)
{
	int16_t raw_voltage = (int16_t)((volt_reg[0] << 8) | volt_reg[1]);

	/* Skip first 3 bits */
	raw_voltage >>= 3;

	/* LSB = 4mV*/

	float voltage_V = ((float)raw_voltage) * 0.004f;

	return voltage_V;
}

/* Register callback functions */
DevStatus_t BSP_INA219_RegisterRxCpltCallbak(void (*callback_fun)(void))
{
	if(callback_fun == NULL)
	{
		return DRV_INIT_NEEDED;
	}
	I2C1RxCpltCallbak = callback_fun;

	return DRV_OK;
}

/* Overwrite HAL Tx/RxCpltCallback */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance == I2C1)
	{
		if(I2C1RxCpltCallbak != NULL)
		{
			I2C1RxCpltCallbak();
		}
	}
}
