#ifndef TIME_H
#define TIME_H

#include <stdint.h>

void TIME_SetTimebase(uint64_t timenow);
uint64_t TIME_GetTime(void);

#endif