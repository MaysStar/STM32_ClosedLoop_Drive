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
static void APP_SD_Send_Logs(char* text, uint32_t len);

/* private task handle, task and queue */
static QueueHandle_t q_uart_sd_logging = NULL;
static TaskHandle_t uart_sd_logging_handle = NULL;
static void uart_sd_logging_task(void* pvParameters);



DWORD get_fattime(void) {
    return  ((DWORD)(2022 - 1980) << 25) |
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
	if(res == FR_OK)
	{
		APP_LOG_INFO("sd mount was successful, code: %d", res);
	}
	else APP_LOG_ERROR("sd mount was unsuccessful, code: %d", res);

	return res;
}

/* Function for initialization SD and prepare to logs UART */
void APP_LOGS_Init(SD_HandleTypeDef* phsd, UART_HandleTypeDef* phuart1, UART_HandleTypeDef* phuart2, UART_HandleTypeDef* phuart3)
{
	/* Set all peripheral handles in BSP */
	BSP_SD_InitSetHandle(phsd);
	BSP_UART_Init(phuart1, phuart2, phuart3);

	/* Configure freeRTOS structures */
	q_uart_sd_logging = xQueueCreate(SD_CARD_QUEUE_SIZE, sizeof(char) * SD_CARD_QUEUE_DATA_LEN);
	configASSERT(q_uart_sd_logging != NULL);

	xTaskCreate(uart_sd_logging_task, "uart_sd_logging_task", 2048, NULL, 4, &uart_sd_logging_handle);
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

	if(res == FR_OK)
	{
		APP_LOG_INFO("file opening was successful, code: %d", res);
	}
	else APP_LOG_ERROR("file opening was unsuccessful, code: %d", res);

	return res;
}

/* Write data into SD card but previous data will be deleted
//static SDRESULT APP_SD_Write(const char* filename, const char* text, uint32_t len)
{
	SDRESULT res = APP_SD_Mount();

	if(res == FR_OK)
	{
		// Open file in work directory
		TCHAR SD_WORK_FILE_PATH[128];
		snprintf(SD_WORK_FILE_PATH, sizeof(SD_WORK_FILE_PATH), "%s/%s", SD_WORK_DIR_PATH, filename);

		// if file doesn't exist it will be created and data just be overwritten because of the flag FA_OPEN_ALWAYS
		res = f_open(&WORK_FILE, SD_WORK_FILE_PATH, FA_WRITE | FA_OPEN_ALWAYS );

		if(res == FR_OK)
		{
			APP_LOG_INFO("file opening was successful, code: %d", res);

			// Write into file
			UINT writen_bytes;
			res = f_write(&WORK_FILE, text, len, &writen_bytes);

			if(res == FR_OK)
			{
				APP_LOG_INFO("file writing was successful, code: %d", res);
				APP_LOG_INFO("number of bytes was written: %d", writen_bytes);
			}
			else APP_LOG_ERROR("file writing was unsuccessful, code: %d", res);

			f_close(&WORK_FILE);
		}
		else APP_LOG_ERROR("file opening was unsuccessful, code: %d", res);
	}
	return res;
}

// Write data from SD card
static SDRESULT APP_SD_Read(const char* filename, uint32_t len)
{
	//if you need this function you can write it yourself
	return 0;
}

*/

/* Append data to SD card and previous data won't be deleted */
static SDRESULT APP_SD_Append(const char* filename, const char* text, uint32_t len)
{
	SDRESULT res;
	/* Append into file */
	UINT writen_bytes;
	res = f_write(&WORK_FILE, text, len, &writen_bytes);

	if(res == FR_OK)
	{
		APP_LOG_INFO("file appending was successful, code: %d", res);
		APP_LOG_INFO("number of bytes was written: %d", writen_bytes);
	}
	else APP_LOG_ERROR("file appending was unsuccessful, code: %d", res);

	f_sync(&WORK_FILE);

	return res;
}

/* Function for logging data use big buffer to send data pieces of 512 bytes */
void APP_SD_Send_Logs(char* text, uint32_t len)
{
	for(uint32_t i = 0; i < len; ++i)
	{
		logs_buffer[logs_buffer_idx++] = text[i];

		/* Send data from 0 - 511 */
		if(logs_buffer_idx == SD_CARD_BUFFER_MAX_LEN)
		{
			APP_SD_Append(SD_MOTOR_LOGS_WORK_FILE_PATH, logs_buffer, SD_CARD_BUFFER_MAX_LEN);
		}

		/* Send data from 512 - 1023 and make buffer cycle */
		else if(logs_buffer_idx == (SD_CARD_BUFFER_MAX_LEN * 2))
		{
			APP_SD_Append(SD_MOTOR_LOGS_WORK_FILE_PATH, &logs_buffer[512], SD_CARD_BUFFER_MAX_LEN);
			logs_buffer_idx = 0;
		}
	}
}

/* Function to set data from other tasks to local Queue and process it */
void APP_LOGS_SetData(char* data, uint32_t len)
{
	char temp_buffer[SD_CARD_QUEUE_DATA_LEN];
	while(len >= SD_CARD_QUEUE_DATA_LEN)
	{
		/* in the end will be "\0" so SD_CARD_QUEUE_DATA_LEN - 1 length */
		snprintf(temp_buffer, SD_CARD_QUEUE_DATA_LEN, "%.*s", (int)(SD_CARD_QUEUE_DATA_LEN - 1), data);

		/* Send cope of the temp buffer */
		xQueueSend(q_uart_sd_logging, temp_buffer, pdMS_TO_TICKS(50));

		/* increase and decrease buffer and length */
		data += (SD_CARD_QUEUE_DATA_LEN - 1);
		len -= (SD_CARD_QUEUE_DATA_LEN - 1);
	}
	if(len > 0)
	{
		/* Send last buffer of data which less than or equal SD_CARD_QUEUE_DATA_LEN */
		snprintf(temp_buffer, SD_CARD_QUEUE_DATA_LEN, "%.*s", (int)len , data);
		xQueueSend(q_uart_sd_logging, temp_buffer, pdMS_TO_TICKS(50));
	}
}

/* Main task for SD card and UART logging */
static void uart_sd_logging_task(void* pvParameters)
{

	/* SD configuration before use */
	{
		SDRESULT res = APP_SD_Mount();
		if(res == FR_OK)
		{
			/* Create new work directory */
			res = f_mkdir(SD_WORK_DIR_PATH);
			if(res == FR_OK)
			{
				APP_LOG_INFO("make dir was successful, code: %d", res);
			}
			else APP_LOG_ERROR("make dir was unsuccessful, code: %d", res);
		}

		/* Open file */
		APP_SD_Open();
	}

	char sd_card_buffer[SD_CARD_QUEUE_DATA_LEN];
	while(1)
	{
		xQueueReceive(q_uart_sd_logging, sd_card_buffer, portMAX_DELAY);

		/* UART management */
		{
			OSAL_UART3_SendData(sd_card_buffer, strlen(sd_card_buffer));
			APP_LOG_INFO("UART transmit data");
		}

		/* SDIO management */
		{
			if(BSP_SD_IsDetected() == SD_PRESENT)
			{
				if(is_sd_card_present == SD_CARD_NOT_PRESENT)
				{
					is_sd_card_present = SD_CARD_PRESENT;
					/* Open file */
					APP_SD_Open();
				}
				APP_SD_Send_Logs(sd_card_buffer, strlen(sd_card_buffer));
			}
			else
			{
				is_sd_card_present = SD_CARD_NOT_PRESENT;
				//APP_LOG_ERROR("SD_CARD_NOT_PRESENT");
			}
		}
	}
}
