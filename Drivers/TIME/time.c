#include "time.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"

uint64_t systimebase = 0;

void TIME_SetTimebase(uint64_t timenow)
{
    systimebase = timenow;
}

uint64_t TIME_GetTime(void)
{
    return (systimebase + xTaskGetTickCount());
}