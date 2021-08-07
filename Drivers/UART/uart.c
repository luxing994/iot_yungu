#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "uart.h"
#include "stm32f4xx_hal.h"

#define RECV_DATA_SIZE 1

typedef struct {
    uint32_t front;
    uint32_t rear;
    uint8_t rdata;
    uint8_t recBuffer[UART_RECEIVE_BUFFER_SIZE];
} UartBuffer;
UartBuffer uartBuffer[UARTMAX];
UART_HandleTypeDef huart[UARTMAX];
USART_TypeDef *uartpoint[UARTMAX] = {
    USART1,
    USART2,
    USART6,
};

static void UART_InitBuffer(uint32_t channel)
{
    uartBuffer[channel].front = uartBuffer[channel].rear = 0;
    memset(uartBuffer[channel].recBuffer, 0, UART_RECEIVE_BUFFER_SIZE);
}

static int UART_WriteBufferByte(uint32_t channel, uint8_t data)
{
    if ((uartBuffer[channel].rear + 1) % (UART_RECEIVE_BUFFER_SIZE) == uartBuffer[channel].front) {
		return -1;
	}

    uartBuffer[channel].recBuffer[uartBuffer[channel].rear] = data;
	uartBuffer[channel].rear = (uartBuffer[channel].rear + 1) % UART_RECEIVE_BUFFER_SIZE;
	return 0;
}

int UART_ReadBufferByte(uint32_t channel, uint8_t *data)
{
    if (uartBuffer[channel].front == uartBuffer[channel].rear) {
        return -1;
    }

    *data = uartBuffer[channel].recBuffer[uartBuffer[channel].front];
    uartBuffer[channel].front = (uartBuffer[channel].front + 1) % UART_RECEIVE_BUFFER_SIZE;
    return 0;
}

int UART_Init(uint32_t channel, uint32_t baudrate)
{
    if (channel > UARTMAX) {
        return -1;
    }
    
    huart[channel].Instance = uartpoint[channel];
    huart[channel].Init.BaudRate = baudrate;
    huart[channel].Init.WordLength = UART_WORDLENGTH_8B;
    huart[channel].Init.StopBits = UART_STOPBITS_1;
    huart[channel].Init.Parity = UART_PARITY_NONE;
    huart[channel].Init.Mode = UART_MODE_TX_RX;
    huart[channel].Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart[channel].Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart[channel]) != HAL_OK)
    {
        return -1;
    }

    UART_InitBuffer(channel);
    return 0;
}

int UART_SendData(uint32_t channel, uint8_t *data, uint32_t size)
{
    int ret;

    ret = HAL_UART_Transmit(&huart[channel], data, size, UART_SEND_TIMEOUT);
    if (ret != HAL_OK) {
        return -1;
    }

    return 0;
}

int UART_RecvData(uint32_t channel)
{
    int ret;

    ret = HAL_UART_Receive_IT(&huart[channel], &(uartBuffer[channel].rdata), RECV_DATA_SIZE);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int UART_RecvDataDma(uint32_t channel)
{
    int ret;

    ret = HAL_UART_Receive_DMA(&huart[channel], &(uartBuffer[channel].rdata), RECV_DATA_SIZE);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int UART_ParseOK(uint32_t channel, uint8_t curData)
{
    uint32_t curfront = uartBuffer[channel].front;
    
    if ((curData != 'K') || (uartBuffer[channel].recBuffer[(curfront - 2 + UART_RECEIVE_BUFFER_SIZE) % 
        UART_RECEIVE_BUFFER_SIZE] != 'O')) {
        return -1;
    }

    return 0;
}

int UART_ParseSendClosed(uint32_t channel, uint8_t curData)
{
    uint32_t curfront = uartBuffer[channel].front;

    if ((curData != 'D') || (uartBuffer[channel].recBuffer[(curfront - 
        2 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'E') ||
        (uartBuffer[channel].recBuffer[(curfront - 
        3 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'S') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        4 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'O') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        5 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'L') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        6 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'C')) {
        return -1;
    }

    return 0;
}

int UART_ParseSendEnd(uint32_t channel, uint8_t curData)
{
    uint32_t curfront = uartBuffer[channel].front;

    if ((curData != 'K') || (uartBuffer[channel].recBuffer[(curfront - 
        2 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'O') ||
        (uartBuffer[channel].recBuffer[(curfront - 
        3 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != ' ') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        4 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'D') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        5 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'N') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        6 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'E') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        7 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'S')) {
        return -1;
    }

    return 0;
}

int UART_ParseRecvData(uint32_t channel, uint8_t curData)
{
    uint32_t curfront = uartBuffer[channel].front;

    if ((curData != 'D') || (uartBuffer[channel].recBuffer[(curfront - 
        2 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'P') ||
        (uartBuffer[channel].recBuffer[(curfront - 
        3 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != 'I') || 
        (uartBuffer[channel].recBuffer[(curfront - 
        4 + UART_RECEIVE_BUFFER_SIZE) % UART_RECEIVE_BUFFER_SIZE] != '+')) {
        return -1;
    }

    return 0;
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart[UART1]);
}

int UART_Printf(const char *format, ...)
{
    uint8_t str[128] = {0};
    int ret, i;
    va_list args;

    va_start(args, format);
    ret = vsprintf((char *)str, format, args);
    va_end(args); 
    if (ret < 0) {
        return -1;
    }

    if (ret > 128) {
        ret = 128;
    }

    for (i = 0; i < ret; i++) {
        (void)HAL_UART_Transmit(&huart[UART2], &str[i], 1, 1000);
    }

    return 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) {
        (void)UART_WriteBufferByte(UART1, uartBuffer[UART1].rdata);
        HAL_UART_Receive_IT(&huart[UART1], &(uartBuffer[UART1].rdata), RECV_DATA_SIZE);
    } else if (huart->Instance == USART6) {
        (void)UART_WriteBufferByte(UART6, uartBuffer[UART6].rdata);
    }
}
