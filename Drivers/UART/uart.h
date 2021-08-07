#ifndef UART_H
#define UART_H

typedef enum {
    UART1,
    UART2,
    UART6,
    UARTMAX,
} UART_NUMBER;

#define UART1_BAUDRATE  115200
#define UART2_BAUDRATE  115200
#define UART6_BAUDRATE  115200
#define UART_RECEIVE_BUFFER_SIZE 2048
#define UART_SEND_TIMEOUT 100

int UART_Init(uint32_t uart, uint32_t baudrate);
int UART_ReadBufferByte(uint32_t channel, uint8_t *data);
int UART_SendData(uint32_t channel, uint8_t *data, uint32_t size);
int UART_RecvData(uint32_t channel);
int UART_RecvDataDma(uint32_t channel);
int UART_ParseOK(uint32_t channel, uint8_t curData);
int UART_ParseSendClosed(uint32_t channel, uint8_t curData);
int UART_ParseSendEnd(uint32_t channel, uint8_t curData);
int UART_ParseRecvData(uint32_t channel, uint8_t curData);
int UART_Printf(const char *format, ...);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART6_IRQHandler(void);

#endif