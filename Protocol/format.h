#ifndef FORMAT_H
#define FORMAT_H

#include <stdint.h>

#define MAX_DATA_LOAD_SIZE        128
#define HEAD_FIRST_BYTE           0xAF
#define HEAD_SECOND_BYTE          0xCD

#define CONTROL_FRAME_TYPE        0x01
#define CONTROL_START_FRAME_TYPE  0x01
#define CONTROL_END_FRAME_TYPE    0x02
#define CONTROL_BACK_FRAME_TYPE   0x03

#define DATA_FRAME_TYPE           0x02
#define DATA_COMMOND_FRAME_TYPE   0x01
#define DATA_LOAR_FRAME_TYPE      0x02

#pragma pack(1)
// 控制帧
typedef struct {
    uint8_t head[2];
    uint8_t size;
    uint8_t type[2];
    uint8_t data;
} ControlFrame;

// 数据域
typedef struct {
    uint8_t size;
    uint8_t data[MAX_DATA_LOAD_SIZE];
    uint8_t crc8;
} DataField;

// 数据帧
typedef struct {
    uint8_t head[2];
    uint8_t size;
    uint8_t type[2];
    DataField data;
} DataFrame;
#pragma pack()

#endif