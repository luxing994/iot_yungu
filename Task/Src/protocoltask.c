#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"
#include "format.h"
#include "uart.h"
#include "time.h"
#include "protocoltask.h"

#define CONTROLERTYPE 2

extern QueueHandle_t xQueue1;
DataFrame dataFrame;
char controlerStr[128] = {0};

int GetDataFromControler(void)
{
	uint8_t curData = 0;
	uint8_t lastData = 0;
	uint8_t length = 0;
	uint8_t outData[256];
	int ret, i;

	while (!((curData == HEAD_SECOND_BYTE) && (lastData == HEAD_FIRST_BYTE))) {
		lastData = curData;
		ret = UART_ReadBufferByte(UART6, &curData);
		if (ret != 0) {
			return -1;
		}
	}

	ret = UART_ReadBufferByte(UART6, &length);
    if (ret != 0) {
        return -1;
    }

	for (i = 0; i < (length - 3); i++) {
		ret = UART_ReadBufferByte(UART6, &outData[i]);
		if (ret != 0) {
			return -1;
		}
	}
	dataFrame.head[0] = HEAD_FIRST_BYTE;
	dataFrame.head[1] = HEAD_SECOND_BYTE;
	dataFrame.size = length;
	memcpy(&dataFrame.type[0], outData, length - 4);
	dataFrame.data.crc8 = outData[length - 4];

	return 0;
}

void ProtocolTask(void const * argument)
{
    int ret;
	uint32_t sendaddr = (uint32_t)&controlerStr;
	uint16_t parameter = 0;

	TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 10;
	
	xLastWakeTime = xTaskGetTickCount();
	(void)UART_RecvDataDma(UART6);
	while (1) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		ret = GetDataFromControler();
		if (ret == 0) {
			parameter = (dataFrame.data.data[1] << 8) | dataFrame.data.data[2];
			(void)sprintf(controlerStr, "{\"DeviceType\":%d,\"Item\":{\"device\":%d,\"time\":%lld,\"value\":%d}}", 
                CONTROLERTYPE, dataFrame.data.data[0], TIME_GetTime(), parameter);
			if (xQueueSend(xQueue1, (void *)&sendaddr, (TickType_t)10) != pdPASS) {
                //TO DO
            }
		}
	}
}