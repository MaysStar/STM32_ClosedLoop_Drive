#ifndef	APP_SD_CARD_H
#define APP_SD_CARD_H

#include "app_logs.h"
#include "ff.h"

#define SD_CARD_BUFFER_MAX_LEN 512

typedef FRESULT SDRESULT;

void APP_SD_Init(void);
void APP_SD_Write(const char* filename, const char* text, uint32_t len);
void APP_SD_Read(const char* filename, uint32_t len);
void APP_SD_Append(const char* filename, const char* text, uint32_t len);

void APP_SD_Send_Logs(char* text, uint32_t len);

#endif // APP_SD_CARD_H
