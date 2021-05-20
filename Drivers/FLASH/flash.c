#include <string.h>
#include "flash.h"
#include "stm32f4xx_hal.h"

#define FLASH_DATA_WRITE_SECTOR  FLASH_SECTOR_7
#define FLASHGETU8(addr)    *((uint8_t*)(addr))

static int FLASH_CheckAddress(uint32_t address, uint32_t size)
{
    if ((address > FLASH_DATA_WRITE_END_ADDRESS) || (address < FLASH_DATA_WRITE_START_ADDRESS)) {
        return -1;
    }

    if ((address + size) > FLASH_DATA_WRITE_END_ADDRESS) {
        return -1;
    }

    return 0;
}

int FLASH_EraseOneSector(uint32_t sectorNumber)
{
    FLASH_EraseInitTypeDef eraseType = {0};
    uint32_t error;
    int ret;

    eraseType.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseType.Banks = FLASH_BANK_1;
    eraseType.Sector = sectorNumber;
    eraseType.NbSectors = 1;
    eraseType.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    ret = HAL_FLASHEx_Erase(&eraseType, &error);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int FLASH_WriteData(uint32_t address, uint8_t *data, uint32_t size)
{
    int ret;
    int pos = 0;
    
    ret = FLASH_CheckAddress(address, size);
    if (ret != 0) {
        return -1;
    }

    ret = HAL_FLASH_Unlock();
    if (ret != 0) {
        return -1;
    }

    ret = FLASH_EraseOneSector(FLASH_DATA_WRITE_SECTOR);
    if (ret != 0) {
        return -1;
    }

    while (size > pos) {
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address + pos, data[pos]);
        if (ret != 0) {
            return -1;
        }
        pos++;
    }

    ret = HAL_FLASH_Lock();
    if (ret != 0) {
        return -1;
    }
    
    return 0;
}

int FLASH_ReadData(uint32_t address, uint8_t *data, uint32_t size)
{
    int ret;
    int pos = 0;

    ret = FLASH_CheckAddress(address, size);
    if (ret != 0) {
        return -1;
    }

    memcpy(data, (uint8_t *)address, size);
    return 0;
}