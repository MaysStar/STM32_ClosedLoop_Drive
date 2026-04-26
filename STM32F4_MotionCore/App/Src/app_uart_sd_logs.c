#include "app_uart_sd_logs.h"

/* Private value for SD card */
__attribute__ ((aligned(4))) static FATFS SDFatFs;
static const TCHAR SD_MOUNT_PATH[] = "0:/";
static const BYTE  SD_MOUNT_OPT = 1;

static const TCHAR SD_WORK_DIR_PATH[] = "0:/motor_data";
static const TCHAR SD_MOTOR_LOGS_WORK_FILE_PATH[] = "logs.csv";
static FIL WORK_FILE;

static char logs_buffer[SD_CARD_BUFFER_MAX_LEN * 2];
static uint32_t logs_buffer_idx = 0;

static uint8_t is_sd_card_present = SD_CARD_PRESENT;

/* private function for work with sd card */
static SDRESULT APP_SD_Open(void);
//static SDRESULT APP_SD_Write(const char* filename, const char* text, uint32_t len);
//static SDRESULT APP_SD_Read(const char* filename, uint32_t len);
static SDRESULT APP_SD_Append(const char* filename, const char* text, uint32_t len);
static SDRESULT APP_SD_Send_Logs(char* text, uint32_t len);

/* private task handle and task */
static TaskHandle_t uart_sd_logging_handle = NULL;
static void uart_sd_logging_task(void* pvParameters);

DWORD get_fattime(void) {
    return  ((DWORD)(2026 - 1980) << 25) |
            ((DWORD)1 << 21) |
            ((DWORD)1 << 16) |
            ((DWORD)0 << 11) |
            ((DWORD)0 << 5)  |
            ((DWORD)0 >> 1);
}

/* Private function */
static SDRESULT APP_SD_Mount(void);

/* Mount SD card prepare to work */
static SDRESULT APP_SD_Mount(void)
{
	SDRESULT res = f_mount(&SDFatFs, SD_MOUNT_PATH, SD_MOUNT_OPT);
	return res;
}

/* Function for initialization SD and prepare to logs UART */
void APP_LOGS_Init(SD_HandleTypeDef* phsd, UART_HandleTypeDef* phuart3)
{
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_SD_CARD_LOG, ERR_NOT_ACTIVE);
	APP_STATE_Update_Error_BeforeRTOSStart(ERR_UART_LOG, ERR_ACTIVE);
	/* Set all peripheral handles in BSP */
	BSP_SD_InitSetHandle(phsd);

	for(uint32_t i = 0; i < 3; ++i)
	{
		if(BSP_UART_LOG_Init(phuart3) == DRV_OK){
			APP_STATE_Update_Error_BeforeRTOSStart(ERR_UART_LOG, ERR_NOT_ACTIVE);
			break;
		}
	}



	/* Configure freeRTOS structures */
	xTaskCreate(uart_sd_logging_task, "uart_sd_logging_task", 2048, NULL, 1, &uart_sd_logging_handle);
	configASSERT(uart_sd_logging_handle != NULL);
}

/* Function to open file with name 0:/motor_data/logs.csv this function uses for append */
static SDRESULT APP_SD_Open(void)
{
	/* Open file in work directory */
	TCHAR SD_WORK_FILE_PATH[128];
	snprintf(SD_WORK_FILE_PATH, sizeof(SD_WORK_FILE_PATH), "%s/%s", SD_WORK_DIR_PATH, SD_MOTOR_LOGS_WORK_FILE_PATH);

	/* if file doesn't exist it will be created and data will be appended to current data in that file because of the flag FA_OPEN_APPEND */
	SDRESULT res = f_open(&WORK_FILE, SD_WORK_FILE_PATH, FA_WRITE | FA_OPEN_APPEND);
	return res;
}

/* Append data to SD card and previous data won't be deleted */
static SDRESULT APP_SD_Append(const char* filename, const char* text, uint32_t len)
{
	SDRESULT res;
	/* Append into file */
	UINT writen_bytes;
	res = f_write(&WORK_FILE, text, len, &writen_bytes);
	res = f_sync(&WORK_FILE);

	return res;
}

/* Function for logging data use big buffer to send data pieces of 512 bytes */
static SDRESULT APP_SD_Send_Logs(char* text, uint32_t len)
{
	SDRESULT ret;
	if(text == NULL)
	{
		return FR_NO_PATH;
	}
	for(uint32_t i = 0; i < len; ++i)
	{
		logs_buffer[logs_buffer_idx++] = text[i];

		/* Send data from 0 - 511 */
		if(logs_buffer_idx == SD_CARD_BUFFER_MAX_LEN)
		{
			ret = APP_SD_Append(SD_MOTOR_LOGS_WORK_FILE_PATH, logs_buffer, SD_CARD_BUFFER_MAX_LEN);
			if(ret != FR_OK)
			{
				return ret;
			}
		}

		/* Send data from 512 - 1023 and make buffer cycle */
		else if(logs_buffer_idx <= (SD_CARD_BUFFER_MAX_LEN * 2))
		{
			ret = APP_SD_Append(SD_MOTOR_LOGS_WORK_FILE_PATH, &logs_buffer[512], SD_CARD_BUFFER_MAX_LEN);
			if(ret != FR_OK)
			{
				return ret;
			}
			logs_buffer_idx = 0;
		}
	}

	return ret;
}

/* Main task for SD card and UART logging */
static void uart_sd_logging_task(void* pvParameters)
{

	SDRESULT sd_state;
	/* SD configuration before use */
	{
		sd_state = APP_SD_Mount();
		if(sd_state == FR_OK)
		{
			APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_NOT_ACTIVE);
			/* Create new work directory */
			sd_state = f_mkdir(SD_WORK_DIR_PATH);
			if(sd_state != FR_OK)
			{
				APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_ACTIVE);
			}
			else
			{
				APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_NOT_ACTIVE);
			}
		}
		else APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_ACTIVE);

		/* Open file */
		sd_state = APP_SD_Open();
		if(sd_state != FR_OK)
		{
			APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_ACTIVE);
		}
		else
		{
			APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_NOT_ACTIVE);
		}
	}

	static GlobalData_t log_msg;
	char sd_card_buffer[LOGS_DATA_LEN];
	static int temp_int, temp_frac, current_A_int, current_A_frac, power_W_int, power_W_frac, voltage_V_int, voltage_V_frac;

	static char date_time[128];
	static char temp_str[16];
	static char electricity_str[64];
	while(1)
	{
		log_msg = APP_STATE_Get_Data();
		/* Cast float into integer to print easier*/
		if(log_msg.dev_state & ERR_RTC)
		{
			snprintf(date_time, sizeof(date_time), "NaN");
		}
		else
		{
			snprintf(date_time, sizeof(date_time), "time: %02d.%02d.%04d %02d:%02d:%02d - %03d ms,"
					, log_msg.date.Date, log_msg.date.Month, log_msg.date.Year + 2000, log_msg.time.Hours, log_msg.time.Minutes, log_msg.time.Seconds,
					(int)( 1000 - ((((log_msg.time.SecondFraction - log_msg.time.SubSeconds) * 1000) / (log_msg.time.SecondFraction + 1)))));
		}

		if(log_msg.dev_state & ERR_TEMP_SENSOR)
		{
			snprintf(temp_str, sizeof(temp_str), "NaN");
		}
		else
		{
			char sign_str[2] = "";

			if(log_msg.temp < 0.0f)
			{
				sign_str[0] = '-';
				sign_str[1] = '\0';
			}
			temp_int = (int)log_msg.temp;
			temp_frac = abs((int)((log_msg.temp - (float)temp_int) * 100));

			snprintf(temp_str, sizeof(temp_str), "temp: %s%d.%02d,", sign_str, temp_int, temp_frac);
		}


		if(log_msg.dev_state & ERR_POWER_SENSOR)
		{
			snprintf(electricity_str, sizeof(electricity_str), "NaN");
		}
		else
		{
			char current_sign_str[2] = "";
			char power_sign_str[2] = "";
			char voltage_sign_str[2] = "";

			if(log_msg.current_A < 0.0f)
			{
				current_sign_str[0] = '-';
				current_sign_str[1] = '\0';
			}

			if(log_msg.power_W < 0.0f)
			{
				power_sign_str[0] = '-';
				power_sign_str[1] = '\0';
			}

			if(log_msg.voltage_V < 0.0f)
			{
				voltage_sign_str[0] = '-';
				voltage_sign_str[1] = '\0';
			}
			current_A_int = (int)log_msg.current_A;
			current_A_frac = abs((int)((log_msg.current_A - (float)current_A_int) * 100));

			power_W_int = (int)log_msg.power_W;
			power_W_frac = abs((int)((log_msg.power_W - (float)power_W_int) * 100));

			voltage_V_int = (int)log_msg.voltage_V;
			voltage_V_frac = abs((int)((log_msg.voltage_V - (float)voltage_V_int) * 100));

			snprintf(electricity_str, sizeof(electricity_str), "current A: %s%d.%02d, power W: %s%d.%02d, voltage V: %s%d.%02d,",
					current_sign_str, current_A_int, current_A_frac, power_sign_str, power_W_int, power_W_frac, voltage_sign_str, voltage_V_int, voltage_V_frac);
		}

		snprintf(sd_card_buffer, LOGS_DATA_LEN, "%s%s%s target_speed: %d, motor_speed: %d\n"
							, date_time, temp_str, electricity_str, (int)log_msg.target_motor_speed, (int)log_msg.real_motor_speed);
		/* UART management */
		{
			DevStatus_t uart_state = OSAL_UART_LOG_SendData(sd_card_buffer, strlen(sd_card_buffer));
			if(uart_state != DRV_OK)
			{
				APP_STATE_Update_Error(ERR_UART_LOG, ERR_ACTIVE);
			}
			else
			{
				APP_STATE_Update_Error(ERR_UART_LOG, ERR_NOT_ACTIVE);
			}
		}

		/* SDIO management */
		{
			if((log_msg.dev_state & ERR_SD_CARD_LOG) == 0)
			{
				if(BSP_SD_IsDetected() == SD_PRESENT)
				{
					if(is_sd_card_present == SD_CARD_NOT_PRESENT)
					{
						is_sd_card_present = SD_CARD_PRESENT;
						/* Open file */
						sd_state = APP_SD_Open();
						if(sd_state != FR_OK)
						{
							APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_ACTIVE);
						}
						else
						{
							APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_NOT_ACTIVE);
						}
					}
					sd_state = APP_SD_Send_Logs(sd_card_buffer, strlen(sd_card_buffer));
					if(sd_state != FR_OK)
					{
						APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_ACTIVE);
					}
					else
					{
						APP_STATE_Update_Error(ERR_SD_CARD_LOG, ERR_NOT_ACTIVE);
					}
				}
				else
				{
					is_sd_card_present = SD_CARD_NOT_PRESENT;
					//APP_LOG_ERROR("SD_CARD_NOT_PRESENT");
				}
			}
		}
	}
}
