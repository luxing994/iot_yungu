#include <string.h>
#include <stdint.h>
#include "inittask.h"
#include "uart.h"
#include "processtask.h"
#include "recvtask.h"
#include "cmsis_os.h"

osThreadId procTaskHandle;
osThreadId recvTaskHandle;

void InitTask(void const * argument)
{
    UART_Printf("Iot terminal restart!!!\n");
    
    osThreadDef(procTask, ProcessTask, osPriorityNormal, 0, 128);
    procTaskHandle = osThreadCreate(osThread(procTask), NULL);
    if (procTaskHandle == NULL) {
        // TO DO
    }

    osThreadDef(recvTask, RecvTask, osPriorityNormal, 0, 128);
    recvTaskHandle = osThreadCreate(osThread(recvTask), NULL);
    if (recvTaskHandle == NULL) {
        // TO DO
    }

    vTaskDelete(NULL);
}