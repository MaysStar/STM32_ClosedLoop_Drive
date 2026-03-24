#include "app_sd_card.h"

/* Private value for SD card */
static FATFS SDFatFs;
static const TCHAR SD_MOUNT_PATH[] = "0:/";
static const BYTE  SD_MOUNT_OPT = 1;

static const TCHAR SD_WORK_DIR_PATH[] = "0:/motor_data";
static const TCHAR SD_MOTOR_LOGS_WORK_FILE_PATH[] = "logs.csv";
static FIL WORK_FILE;

static char logs_buffer[SD_CARD_BUFFER_MAX_LEN * 2];
static uint32_t logs_buffer_idx = 0;

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
static SDRESULT APP_SD_Unmount(void);

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

/* Unmount SD card finish write/read operation */
static SDRESULT APP_SD_Unmount(void)
{
	SDRESULT res = f_unmount(SD_MOUNT_PATH);
	if(res == FR_OK)
	{
		APP_LOG_INFO("sd unmount was successful, code: %d", res);
	}
	else APP_LOG_ERROR("sd unmount was unsuccessful, code: %d", res);

	return res;
}

/* Function for initialization and create work directory */
void APP_SD_Init(void)
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
}



/* Write data into SD card but previous data will be deleted */
void APP_SD_Write(const char* filename, const char* text, uint32_t len)
{
	SDRESULT res = APP_SD_Mount();

	if(res == FR_OK)
	{
		/* Open file in work directory */
		TCHAR SD_WORK_FILE_PATH[128];
		snprintf(SD_WORK_FILE_PATH, sizeof(SD_WORK_FILE_PATH), "%s/%s", SD_WORK_DIR_PATH, filename);

		/* if file doesn't exist it will be created and data just be overwritten because of the flag FA_OPEN_ALWAYS */
		res = f_open(&WORK_FILE, SD_WORK_FILE_PATH, FA_WRITE | FA_OPEN_ALWAYS);

		if(res == FR_OK)
		{
			APP_LOG_INFO("file opening was successful, code: %d", res);

			/* Write into file */
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
}

/* Write data from SD card */
void APP_SD_Read(const char* filename, uint32_t len)
{
	/* if you need this function you can write it yourself */
}

/* Append data to SD card and previous data won't be deleted */
void APP_SD_Append(const char* filename, const char* text, uint32_t len)
{
	SDRESULT res = APP_SD_Mount();

	if(res == FR_OK)
	{
		/* Open file in work directory */
		TCHAR SD_WORK_FILE_PATH[128];
		snprintf(SD_WORK_FILE_PATH, sizeof(SD_WORK_FILE_PATH), "%s/%s", SD_WORK_DIR_PATH, filename);

		/* if file doesn't exist it will be created and data will be appended to current data in that file because of the flag FA_OPEN_APPEND */
		res = f_open(&WORK_FILE, SD_WORK_FILE_PATH, FA_WRITE | FA_OPEN_APPEND);

		if(res == FR_OK)
		{
			APP_LOG_INFO("file opening was successful, code: %d", res);

			/* Append into file */
			UINT writen_bytes;
			res = f_write(&WORK_FILE, text, len, &writen_bytes);

			if(res == FR_OK)
			{
				APP_LOG_INFO("file appending was successful, code: %d", res);
				APP_LOG_INFO("number of bytes was written: %d", writen_bytes);
			}
			else APP_LOG_ERROR("file appending was unsuccessful, code: %d", res);

			f_close(&WORK_FILE);
		}
		else APP_LOG_ERROR("file opening was unsuccessful, code: %d", res);
	}
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
