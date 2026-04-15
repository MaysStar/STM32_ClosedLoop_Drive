#ifndef OSAL_FREERTOS_H
#define OSAL_FREERTOS_H

/* BSP party includes */


/* third party includes */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

/* else includes*/
#include <stdio.h>
#include <string.h>

void OSAL_Init(void);

#endif /* OSAL_FREERTOS_H */
