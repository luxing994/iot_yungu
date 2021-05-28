#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "cmsis_os.h"
#include "processtask.h"
#include "recvtask.h"
#include "uart.h"
#include "esp8266.h"
#include "time.h"

uint8_t recvDataBuffer[128] = {0};
uint64_t value = 0;

uint64_t strtoint(char *str, uint32_t size)
{
    int i = size;
    
    while (i != 0) {
        value += (str[size - i] - '0') * pow(10, i - 1);
        i--;
    }

    return value;
}

void ProcessTask(void const * argument)
{
    uint8_t rdata;
    uint8_t tempRecvBuffer[16] = {0};
    uint32_t count = 0;
    uint32_t sizeCount = 0;
    uint32_t dataCount = 0;
    uint32_t recDataSize = 0;
    int ret, size;
    int getSsidFlag = 0;
    int getPasswordFlag = 0;
    int startRecvFlag = 0;
    int startPraseRecvFlag = 0;
    int recvDataSwich = 0;
    char *delim = ",";
    char *token[4];
    
    (void)UART_RecvData(UART1);
    while (1) {
        ret = UART_ReadBufferByte(UART1, &rdata);
        if ((startRecvFlag == 1) && (ret == 0)) {
            UART_SendData(UART2, &rdata, sizeof(rdata));
            if (recvDataSwich == 0) {
                if (rdata != ':') {
                    tempRecvBuffer[count] = rdata;
                } else {
                    token[sizeCount] = strtok((char *)tempRecvBuffer, delim);
                    while (token[sizeCount] != NULL) {
                        sizeCount++;
                        if (sizeCount > 4) {
                            return;
                        }
                        token[sizeCount] = strtok(NULL, delim);
                    }
                    if (token[1] != NULL) {
                        recDataSize = atoi(token[1]);
                    } else {
                        recDataSize = atoi(token[0]);
                    }
                    recvDataSwich = 1;
                }
                count++;
                continue;
            }

            if (recvDataSwich == 1) {
                recvDataBuffer[dataCount] = rdata;
                dataCount++;
                if (dataCount == recDataSize) {
                    startPraseRecvFlag = 1;
                    recvDataSwich = 0;
                    startRecvFlag = 0;
                    count = 0;
                    sizeCount = 0;
                    dataCount = 0;
                }
            }
        } else if ((startRecvFlag == 0) && (ret == 0)) {
            UART_SendData(UART2, &rdata, sizeof(rdata));
            ret = UART_ParseRecvData(UART1, rdata);
            if (ret == 0) {
                startRecvFlag = 1;
                continue;
            }
            
            ret = UART_ParseOK(UART1, rdata);
            if (ret == 0) {
                ESP_SetCommondSwitch();
            }

            ret = UART_ParseSendEnd(UART1, rdata);
            if (ret == 0) {
                ESP_SetTcpSendSwitch();
            }
        } else if (startPraseRecvFlag == 1) {
            if (recDataSize < 2) {
                startPraseRecvFlag = 0;
                continue;
            }

            if ((recvDataBuffer[1] >= '0') && (recvDataBuffer[1] <= '9')) {
                size = recvDataBuffer[1] - '0';
            } else if ((recvDataBuffer[1] >= 'A') && (recvDataBuffer[1] <= 'F')) {
                size = recvDataBuffer[1] - 'A' + 10;
            }

            if (recvDataBuffer[0] == '0') {
                ESP_SetSsid(&recvDataBuffer[2], size);
                getSsidFlag = 1;
            }

            if (recvDataBuffer[0] == '1') {
                ESP_SetPassword(&recvDataBuffer[2], size);
                getPasswordFlag = 1;
            }

            if (recvDataBuffer[0] == '2') {
                TIME_SetTimebase(strtoint(&recvDataBuffer[2], size) - xTaskGetTickCount());
            }

            if ((getSsidFlag == 1) && (getPasswordFlag == 1)) {
                getSsidFlag = 0;
                getPasswordFlag = 0;
                ESP_SetFlag(1);
                ESP_WriteInfo();
                HAL_Delay(1000);
                // HAL_NVIC_SystemReset();
            }
            startPraseRecvFlag = 0;
        }
    }
}