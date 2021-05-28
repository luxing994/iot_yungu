#include <stdio.h>
#include <string.h>
#include "esp8266.h"
#include "uart.h"
#include "flash.h"
#include "main.h"

#define SENDCOMMONDTIMEOUT 30000

int commandSwitch = 0;
int tcpSendSwitch = 1;

char *RstCommond = "AT+RST\r\n";
char *TCPServerCommond[] = {
    "AT+CWMODE=3\r\n", "AT+CIPMUX=1\r\n","AT+CIPSERVER=1\r\n", "AT+CWSAP?\r\n", "AT+CIPAP?\r\n", "AT+CIPCLOSE=0\r\n"
};

char *TCPClientCommond[] = {
    "AT+CWMODE=3\r\n", "AT+CIFSR\r\n", "AT+CIPSTART=\"TCP\",\"121.37.178.224\",8090\r\n"    // My huwawei cloud ip:121.37.178.224:8090
};                                                                                          // Yungu ip:192.168.31.70:8090

typedef struct {
    uint8_t flag;
    char ssid[15];
    char password[16];
} WifiInfomation;
WifiInfomation wifiInfo = {0};

void ESP_SetFlag(int flag)
{   
    wifiInfo.flag = flag;
}

int ESP_SetSsid(uint8_t *data, uint32_t size)
{   
    if ((data == NULL) || (size > 15)) {
        return -1;
    }

    memset(wifiInfo.ssid, 0, sizeof(wifiInfo.ssid));
    memcpy(wifiInfo.ssid, data, size);
    return 0;
}

int ESP_SetPassword(uint8_t *data, uint32_t size)
{   
    if ((data == NULL) || (size > 16)) {
        return -1;
    }

    memset(wifiInfo.password, 0, sizeof(wifiInfo.password));
    memcpy(wifiInfo.password, data, size);
    return 0;
}

int ESP_SetInfo(char *ssid, char *password)
{   
    if ((ssid == NULL) && (password == NULL)) {
        return -1;
    }

    if ((strlen(ssid) > 15) || (strlen(password) > 16)) {
        return -1;
    }

    memcpy(wifiInfo.ssid, ssid, strlen(ssid));
    memcpy(wifiInfo.password, password, strlen(password));

    return 0;
}

int ESP_WriteInfo(void)
{   
    int ret;

    ret = FLASH_WriteData(FLASH_DATA_WRITE_START_ADDRESS, (uint8_t *)&wifiInfo, sizeof(wifiInfo));
    if (ret != 0) {
        return -1;
    }

    return 0;
}

static void ESP_ReadInfo(void)
{
    (void)FLASH_ReadData(FLASH_DATA_WRITE_START_ADDRESS, &(wifiInfo.flag), 1);
    (void)FLASH_ReadData(FLASH_DATA_WRITE_START_ADDRESS + 1, (uint8_t *)wifiInfo.ssid, 15);
    (void)FLASH_ReadData(FLASH_DATA_WRITE_START_ADDRESS + 16, (uint8_t *)wifiInfo.password, 16);
}


static int ESP_SendCommond(uint8_t *data, uint32_t size, uint32_t timeout)
{
    uint32_t tickstart = 0;

    tickstart = HAL_GetTick();
    while (commandSwitch == 0) {
        if ((timeout == 0) || (HAL_GetTick() - tickstart > timeout)) {
            return -1;
        }
        (void)UART_SendData(UART1, data, size);
        HAL_Delay(1000);
    } 

    commandSwitch = 0;
    return 0;
}

int ESP_SendDataTCP(uint8_t mode, uint8_t *data, uint32_t size, uint32_t timeout)
{
    char str[32] = {0};
    int ret;
    uint32_t tickstart = 0;

    tickstart = HAL_GetTick();
    while (tcpSendSwitch == 0) {
        if ((timeout == 0) || (HAL_GetTick() - tickstart > timeout)) {
            return -1;
        }
    }

    tcpSendSwitch = 0;
    if (mode == 0) {
        (void)sprintf(str, "AT+CIPSEND=%d\r\n", size);
    } else if (mode == 1) {
        (void)sprintf(str, "AT+CIPSEND=0, %d\r\n", size);
    }
    ret = UART_SendData(UART1, (uint8_t *)str, strlen(str));
    if (ret != 0) {
        return -1;
    }

    HAL_Delay(1);
    ret = UART_SendData(UART1, data, size);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

void ESP_SetCommondSwitch(void)
{
    commandSwitch = 1;
}

void ESP_SetTcpSendSwitch(void)
{
    tcpSendSwitch = 1;
}

int ESP_EnterTcpServerMode(void)
{
    int ret, i;

    for (i = 0; i < 5; i++) {
        ret = ESP_SendCommond((uint8_t *)TCPServerCommond[i], strlen(TCPServerCommond[i]), SENDCOMMONDTIMEOUT);
        if (ret != 0) {
            return -1;
        }
    }

    return 0;
}

int ESP_EnterTcpClientrMode(void)
{
    int ret = 0;
    char str[64] = {0};

    (void)sprintf(str, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifiInfo.ssid, wifiInfo.password);
    (void)ESP_SendCommond((uint8_t *)TCPClientCommond[0], strlen(TCPClientCommond[0]), SENDCOMMONDTIMEOUT);
    ret = ESP_SendCommond(str, strlen(str), SENDCOMMONDTIMEOUT);
    if (ret != 0) {
        ESP_SetFlag(0);
        ESP_WriteInfo();
        UART_Printf("wifi config error\n");
        HAL_Delay(1000);
        // HAL_NVIC_SystemReset();
    }
    (void)ESP_SendCommond((uint8_t *)TCPClientCommond[1], strlen(TCPClientCommond[1]), SENDCOMMONDTIMEOUT);
    ret = ESP_SendCommond((uint8_t *)TCPClientCommond[2], strlen(TCPClientCommond[2]), SENDCOMMONDTIMEOUT);
    if (ret != 0) {
        UART_Printf("tcp server config error\n");
        HAL_Delay(1000);
        // HAL_NVIC_SystemReset();
    }

    return 0;
}

int ESP_Init(void)
{
    int ret;

    ESP_SendCommond((uint8_t *)RstCommond, strlen(RstCommond), SENDCOMMONDTIMEOUT);
    HAL_Delay(2000);
    ESP_ReadInfo();

    if (wifiInfo.flag == 0) {
        ret = ESP_EnterTcpServerMode();
        if (ret != 0) {
            return -1;
        } else {
            return 1;
        }
    } else {
        ret = ESP_EnterTcpClientrMode();
        if (ret != 0) {
            return -1;
        }
    }

    UART_Printf("ESP init end!!!\n");
    HAL_Delay(1000);

    return 0;
}
