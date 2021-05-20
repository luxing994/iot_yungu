#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

#define FLASH_DATA_WRITE_START_ADDRESS  0x0807FC00
#define FLASH_DATA_WRITE_END_ADDRESS    0x0807FFFF

int FLASH_EraseOneSector(uint32_t sectorNumber);
int FLASH_WriteData(uint32_t address, uint8_t *data, uint32_t size);
int FLASH_ReadData(uint32_t address, uint8_t *data, uint32_t size);

#endif 