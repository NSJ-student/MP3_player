/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               VS1003.h
** Descriptions:            The VS1003 application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-17
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

#ifndef _VS1003_H_
#define _VS1003_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx_hal.h>

#pragma pack(1)
typedef struct {
	uint8_t id[3];
	uint8_t version[2];
	uint8_t flags;
	uint8_t size[4];
} MP3_ID3v2;
#pragma pack(4)

#define ID3v2_SIZE_POS			6
#define ID3v2_FLAGS_POS			5
#define FOOTER_PRESEND_FLAG	0x10

#define AUDIO_MEM_NUM		2

#define FILE_SAVE_SIZE		512
#define SPI_TRANSFER_SIZE	32

#define PLAY_STATE_1	1
#define PLAY_STATE_2	2
#define PLAY_STATE_END	(AUDIO_MEM_NUM+1)

/* Private define ------------------------------------------------------------*/
#define SPI_SPEED_HIGH    1  
#define SPI_SPEED_LOW     0

#define VS1033B_GPIO		GPIOA
//#define XCS_GPIO_PIN		GPIO_PIN_4
#define SCLK_GPIO_PIN		GPIO_PIN_5
#define MISO_GPIO_PIN		GPIO_PIN_6
#define MOSI_GPIO_PIN		GPIO_PIN_7
#define XDCS_GPIO_PIN		GPIO_PIN_12
#define RESET_GPIO_PIN		GPIO_PIN_10
#define DREQ_GPIO_PIN		GPIO_PIN_8

#define VS1033B_GPIO_2		GPIOB
#define XCS_GPIO_PIN		GPIO_PIN_6

/* reset */
#define MP3_Reset(x)	  x ? 	HAL_GPIO_WritePin(VS1033B_GPIO, RESET_GPIO_PIN, GPIO_PIN_SET) : \
								HAL_GPIO_WritePin(VS1033B_GPIO, RESET_GPIO_PIN, GPIO_PIN_RESET)

/* 命令片选 */
#define MP3_CCS(x)	  x ? 	HAL_GPIO_WritePin(VS1033B_GPIO_2, XCS_GPIO_PIN, GPIO_PIN_SET) : \
								HAL_GPIO_WritePin(VS1033B_GPIO_2, XCS_GPIO_PIN, GPIO_PIN_RESET)

/* 数据片选 */ 
#define MP3_DCS(x)	  x ?	HAL_GPIO_WritePin(VS1033B_GPIO, XDCS_GPIO_PIN, GPIO_PIN_SET) : \
								HAL_GPIO_WritePin(VS1033B_GPIO, XDCS_GPIO_PIN, GPIO_PIN_RESET)

#define MP3_DREQ          HAL_GPIO_ReadPin(VS1033B_GPIO,DREQ_GPIO_PIN)


#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03

/* VS1003寄存器定义 */
#define SPI_MODE        	0x00   
#define SPI_STATUS      	0x01   
#define SPI_BASS        	0x02   
#define SPI_CLOCKF      	0x03   
#define SPI_DECODE_TIME 	0x04   
#define SPI_AUDATA      	0x05   
#define SPI_WRAM        	0x06   
#define SPI_WRAMADDR    	0x07   
#define SPI_HDAT0       	0x08   
#define SPI_HDAT1       	0x09 
#define SPI_AIADDR      	0x0a   
#define SPI_VOL         	0x0b   
#define SPI_AICTRL0     	0x0c   
#define SPI_AICTRL1     	0x0d   
#define SPI_AICTRL2     	0x0e   
#define SPI_AICTRL3     	0x0f   
#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 	


extern volatile uint32_t 	VS1003_Cplt;
extern volatile int32_t 	AudioSize;

/* Private function prototypes -----------------------------------------------*/
void VS1003_Init(void);
void VS1003_SetVol(void); 
void VS1003_Reset(void);
void VS1003_RamTest(void);
void VS1003_SineTest(void);
void VS1003_SoftReset(void); 
void VS1003_Record_Init(void);
void VS1003_ResetDecodeTime(void);
uint16_t VS1003_GetDecodeTime(void);
void VS1003_WriteData( uint8_t * buf);
uint16_t VS1003_ReadReg( uint8_t reg);
void VS1003_WriteData_DMA( uint32_t * start, uint8_t * buf, uint32_t size);
uint32_t GetVS1003State(void);
uint32_t GetVS1003RemainAudio(void);
int VS1003_WriteData_DMA_ING(void);
void SPI1_DMA_Init(void);
void VS1003_WriteReg( uint8_t reg,uint16_t value );
void VS1003_StopPlay(void);

	 
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

