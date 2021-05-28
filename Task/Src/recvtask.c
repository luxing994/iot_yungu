#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "cmsis_os.h"
#include "recvtask.h"
#include "esp8266.h"
#include "time.h"
#include "uart.h"

#define TCPSENDDATATIMEOUT 10000

extern QueueHandle_t xQueue1;
int sendflag = 0;
int EspInitStatus = 1;
char ledstr[128] = {0};

int SYS_GetEspInitStatus(void)
{
    return EspInitStatus;
}

void RecvTask(void const * argument)
{
    uint32_t recvp;
    uint32_t sendaddr = (uint32_t)&ledstr;

    EspInitStatus = ESP_Init();
    while(1) {
        if (sendflag == 1) {
            sendflag = 0;

            (void)sprintf(ledstr, "{\"device\":\"LED\",\"time\":%lld,\"value\":%d}", TIME_GetTime(), HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin));
            if(xQueueSend(xQueue1, (void *)&sendaddr, (TickType_t)10) != pdPASS) {
                //TO DO
            }
        }

        if(xQueueReceive(xQueue1, &recvp, (TickType_t)10) == pdPASS) {
            // UART_Printf("%s", recvp);
            if (EspInitStatus == 0) {
                ESP_SendDataTCP(0, (uint8_t *)recvp, strlen((char *)recvp), TCPSENDDATATIMEOUT);
            }
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    sendflag = 1;
}

void TCP_SetSendFlag(void)
{
    sendflag = 1;
}