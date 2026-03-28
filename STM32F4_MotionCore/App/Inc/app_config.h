#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stdio.h"

#define APP_LOG_INFO(fmt, ...) printf("[INFO]: " fmt "\r\n ", ##__VA_ARGS__)
#define APP_LOG_ERROR(fmt, ...) printf("[ERROR]: " fmt "\r\n ", ##__VA_ARGS__)

#endif // APP_CONFIG_H
