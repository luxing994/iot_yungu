#include "dht22.h"
#include "stm32f4xx_hal.h"

#define TICKTOUS 100

// void DHT_DelayUs(unsigned int us)
// {
//     uint32_t temp;
//     SysTick->LOAD=us*(SystemCoreClock/8000000);             //加载时间
//     SysTick->VAL=0x00;                   //清空计时器
//     SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
//     do
//     {
//         temp=SysTick->CTRL;
//     }
//     while(temp&0x01&&!(temp&(1<<16)));         //等待时间到达
//     SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;   //关闭计时器
//     SysTick->VAL=0x00;                         //清空计时器
// }

static void DHT_DelayUs(uint32_t us)
{
    uint32_t delay = TICKTOUS * us;

    while (delay != 0) {
        delay--;
    }
}

static void DHT_DelayMs(uint32_t ms)
{
    HAL_Delay(ms);
}

void DHT_Reset(void)
{
    DHT22_IO_OUT(); 	//SET OUTPUT
    DHT22_DQ_OUT = 0; 	//GPIOA.0=0
    DHT_DelayMs(30);    //Pull down Least 800us
    DHT22_DQ_OUT = 1; 	//GPIOA.0=1 
	DHT_DelayUs(30);    //Pull up 20~40us
}

uint8_t DHT_Check(void) 	   
{   
	uint8_t retry = 0;

	DHT22_IO_IN();      //SET INPUT
    while ((DHT22_DQ_IN) && (retry < 100)) {    //DHT22 Pull down 40~80us
		retry++;
		DHT_DelayUs(1);
	};

	if (retry >= 100) {
		return 1;	
	} else {
        retry=0;
    }

    while ((!DHT22_DQ_IN) && (retry<100)) {     //DHT22 Pull up 40~80us
		retry++;
		DHT_DelayUs(1);
	};

	if (retry >= 100) {
		return 1;      //chack error				
    }
	return 0;
}

static uint8_t DHT_ReadBit(void) 			 
{
 	uint8_t retry = 0;

	while ((DHT22_DQ_IN) && (retry < 100)) {    //wait become Low level
		retry++;
		DHT_DelayUs(1);
	}

	retry=0;
	while ((!DHT22_DQ_IN) && (retry < 100)) {  //wait become High level
		retry++;
		DHT_DelayUs(1);
	}

	DHT_DelayUs(40);  //wait 40us
	if(DHT22_DQ_IN) {
        return 1;
    } else {
        return 0;
    }		   
}

static uint8_t DHT_ReadByte(void)    
{        
    uint8_t i, data;
    data = 0;
	for (i = 0; i < 8; i++) {
   		data <<= 1; 
	    data |= DHT_ReadBit();
    }						    
    return data;
}

uint8_t DHT_ReadData(float *temperature, float *humidity)
{        
 	uint8_t buf[5];
	uint8_t i;
	uint8_t sum;
	*humidity = 0;
	*temperature = 0;

	DHT_Reset();
	if (DHT_Check() == 0) {
		for (i = 0; i < 5; i++) {
			buf[i] = DHT_ReadByte();
		}

		sum = buf[0] + buf[1] + buf[2] + buf[3];
		if(sum == buf[4]) {
			*humidity = (float)((buf[0] << 8) + buf[1]) / 10;
			*temperature = (float)((buf[2] << 8) + buf[3]) / 10;
		} else {
			*humidity = (float)((buf[0] << 8) + buf[1]) / 10;
			*temperature = (float)((buf[2] << 8) + buf[3]) / 10;
		}
	} else {
		return 1;
	}

	return 0;	    
}

uint8_t DHT_Init(void)
{	 
    DHT_DelayMs(2000);

	DHT_Reset();  
	return DHT_Check();
}
