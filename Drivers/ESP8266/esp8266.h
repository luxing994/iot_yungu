#ifndef ESP8266_H
#define ESP8266_H

#include <stdint.h>

int ESP_Init(void);
int ESP_SendDataTCP(uint8_t mode, uint8_t *data, uint32_t size, uint32_t timeout);
int ESP_SetInfo(char *ssid, char *password);
int ESP_SetSsid(uint8_t *data, uint32_t size);
int ESP_SetPassword(uint8_t *data, uint32_t size);
int ESP_WriteInfo(void);
void ESP_SetFlag(int flag);
void ESP_SetCommondSwitch(void);
void ESP_SetTcpSendSwitch(void);

#endif