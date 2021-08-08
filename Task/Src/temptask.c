#include <stdio.h>
#include <stdint.h>
#include "cmsis_os.h"
#include "temptask.h"
#include "recvtask.h"
#include "uart.h"
#include "time.h"
#include "dht22.h"

#define SENSORTYPE 1

extern QueueHandle_t xQueue1;
char tempstr[128] = {0};

void TempTask(void const * argument)
{
    float temperature = 0;  	    
	float humidity = 0;
    osEvent event;
    int32_t flag = 0x0001;
    uint32_t sendaddr = (uint32_t)&tempstr;
    
    while (1) {
        event = osSignalWait(flag, 1000);
        if ((event.status == osEventSignal) && (SYS_GetEspInitStatus() == 0)) {
            if (event.value.signals & 0x01) {
                DHT_ReadData(&temperature, &humidity);
                // UART_Printf("Temperature: %f, humidity: %f\n", temperature, humidity);
                (void)sprintf(tempstr, "{\"DeviceType\":%d,\"Item\":{\"device\":\"Temperature&Humidity\",\"time\":%lld,\"temperature\":%f,\"humidity\":%f}}", 
                    SENSORTYPE, TIME_GetTime(), temperature, humidity);
                if (xQueueSend(xQueue1, (void *)&sendaddr, (TickType_t)10) != pdPASS) {
                    //TO DO
                }
            }
        }
    }
}