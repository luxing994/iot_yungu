#include <string.h>
#include <stdint.h>
#include "main.h"
#include "inittask.h"
#include "processtask.h"
#include "recvtask.h"
#include "temptask.h"
#include "protocoltask.h"
#include "cmsis_os.h"
#include "uart.h"
#include "dht22.h"
#include "esp8266.h"

extern TIM_HandleTypeDef htim4;
osThreadId procTaskHandle;
osThreadId recvTaskHandle;
osThreadId tempTaskHandle;
osThreadId protocolTaskHandle;

void InitTask(void const * argument)
{
    float temperature = 0;  	    
	float humidity = 0;
    
    UART_Printf("Iot terminal start!!!\n");
    DHT_Init();
    DHT_ReadData(&temperature,&humidity);

    osThreadDef(protocolTask, ProtocolTask, osPriorityNormal, 0, 256);
    protocolTaskHandle = osThreadCreate(osThread(protocolTask), NULL);
    if (protocolTaskHandle == NULL) {
        // TO DO
    }

    osThreadDef(procTask, ProcessTask, osPriorityNormal, 0, 256);
    procTaskHandle = osThreadCreate(osThread(procTask), NULL);
    if (procTaskHandle == NULL) {
        // TO DO
    }

    osThreadDef(recvTask, RecvTask, osPriorityNormal, 0, 256);
    recvTaskHandle = osThreadCreate(osThread(recvTask), NULL);
    if (recvTaskHandle == NULL) {
        // TO DO
    }

    osThreadDef(tempTask, TempTask, osPriorityAboveNormal, 0, 1024);
    tempTaskHandle = osThreadCreate(osThread(tempTask), NULL);
    if (tempTaskHandle == NULL) {
        // TO DO
    }

    HAL_TIM_Base_Start_IT(&htim4);

    vTaskDelete(NULL);
}