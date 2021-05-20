#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "main.h"
#include "recvtask.h"
#include "esp8266.h"

#define TCPSENDDATATIMEOUT 10000

int sendflag = 0;

void RecvTask(void const * argument)
{
    char str[32] = {0};
    int ret;

    ret = ESP_Init();
    while(1) {
        if (sendflag == 1) {
            sendflag = 0;
            if (HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin)) {
                (void)sprintf(str, "LED is on\n");
            } else {
                (void)sprintf(str, "LED is off\n");
            }
            if (ret == 0) {
                ESP_SendDataTCP(0, (uint8_t *)str, strlen(str), TCPSENDDATATIMEOUT);
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