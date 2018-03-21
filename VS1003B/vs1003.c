/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               VS1003.c
** Descriptions:            The VS1003 application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-27
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

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "vs1003.h"
#include "spi.h"

/* Private variables ---------------------------------------------------------*/

/* VS1003���ò��� */ 
/* 0 , henh.1 , hfreq.2 , lenh.3 , lfreq 5 ,������ */
uint8_t vs1003ram[5] = { 0 , 0 , 0 , 0 , 100 };
SPI_HandleTypeDef 	SpiHandle;
DMA_HandleTypeDef 	VS1003_Tx_DmaHandle;
GPIO_InitTypeDef  		GPIO_InitStructure;
volatile uint8_t  *	TxBuffer;
volatile int32_t 	AudioSize = 0;
volatile uint32_t 	VS1003_Cplt = 0;
uint32_t * start_signal;
extern osThreadId FileTaskHandle;

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay Time
* Input          : - cnt: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 0xffff; j++ );
	}
} 

void SPI1_DMA_Init(void)
{
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
	HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
	/* Enable the DMA1 Clock */
	__HAL_RCC_DMA2_CLK_ENABLE();
	
	VS1003_Tx_DmaHandle.Instance 		= DMA2_Stream5;
	
	/* DMA2 Stream3  or Stream6 disable */
	__HAL_DMA_DISABLE(&VS1003_Tx_DmaHandle);
	HAL_DMA_DeInit(&VS1003_Tx_DmaHandle);

	VS1003_Tx_DmaHandle.Parent		= &SpiHandle;
	VS1003_Tx_DmaHandle.Init.Channel 	= DMA_CHANNEL_3;
	VS1003_Tx_DmaHandle.Init.Direction	= DMA_MEMORY_TO_PERIPH;
	VS1003_Tx_DmaHandle.Init.PeriphInc	= DMA_PINC_DISABLE;
	VS1003_Tx_DmaHandle.Init.MemInc	= DMA_MINC_ENABLE;
	VS1003_Tx_DmaHandle.Init.PeriphDataAlignment		= DMA_PDATAALIGN_BYTE;
	VS1003_Tx_DmaHandle.Init.MemDataAlignment		= DMA_MDATAALIGN_WORD;
	
	VS1003_Tx_DmaHandle.Init.Mode		= DMA_NORMAL;
	VS1003_Tx_DmaHandle.Init.Priority	= DMA_PRIORITY_MEDIUM;
	VS1003_Tx_DmaHandle.Init.FIFOMode	= DMA_FIFOMODE_ENABLE;
	VS1003_Tx_DmaHandle.Init.FIFOThreshold	= DMA_FIFO_THRESHOLD_FULL;
	VS1003_Tx_DmaHandle.Init.MemBurst		= DMA_MBURST_SINGLE;
	VS1003_Tx_DmaHandle.Init.PeriphBurst	= DMA_PBURST_SINGLE;
	
	/* DMA Config */
	HAL_DMA_Init(&VS1003_Tx_DmaHandle);
}


void VS1003_WriteData_DMA( uint32_t * start, uint8_t * buf, uint32_t size)
{		
//	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	
	while(  MP3_DREQ ==0 );  
	
	start_signal = start;
	AudioSize = size;
	TxBuffer = buf;
	VS1003_Cplt = 0;
	/*
	VS1003_WriteData(TxBuffer);
	AudioSize -= SPI_TRANSFER_SIZE;
	TxBuffer += SPI_TRANSFER_SIZE;
	*/

	MP3_DCS(0);   
	//SpiHandle.hdmatx = &VS1003_Tx_DmaHandle;
	//__HAL_DMA_ENABLE_IT(&VS1003_Tx_DmaHandle, DMA_IT_TC);
	HAL_SPI_Transmit_DMA(&hspi3, TxBuffer, 32);
	
} 
uint32_t GetVS1003State(void)
{
	return VS1003_Cplt;
}
uint32_t GetVS1003RemainAudio(void)
{
	return AudioSize;
}
/*
void DMA2_Stream5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&VS1003_Tx_DmaHandle);
}
*/
void EXTI9_5_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(DREQ_GPIO_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(MP3_DREQ == 1)
	{
		if(AudioSize > 0)
		{
			//while(  MP3_DREQ ==0 );  
			MP3_DCS(0);   
			hspi3.hdmatx = &VS1003_Tx_DmaHandle;
//			__HAL_DMA_ENABLE_IT(&VS1003_Tx_DmaHandle, DMA_IT_TC);
//			HAL_SPI_Transmit_DMA(&SpiHandle, TxBuffer, 32);
		}
		else
		{
//			AudioSize = 0;
//			TxBuffer = 0;
			VS1003_Cplt = 1;
		}
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi == &hspi3)
	{
		MP3_DCS(1);  
		MP3_CCS(1);  


		AudioSize -= 32;
		TxBuffer += 32;

		if(AudioSize > 0)
		{
			while(  MP3_DREQ ==0 );  
			MP3_DCS(0);   
	//		SpiHandle.hdmatx = &VS1003_Tx_DmaHandle;
	//		__HAL_DMA_ENABLE_IT(&VS1003_Tx_DmaHandle, DMA_IT_TC);
			HAL_SPI_Transmit_DMA(&hspi3, TxBuffer, 32);
		}
		else
		{
			AudioSize = 0;
			TxBuffer = 0;
	//		__HAL_DMA_DISABLE_IT(&VS1003_Tx_DmaHandle, DMA_IT_TC);
			VS1003_Cplt = 1;
			*start_signal = 0;
		}
	}
}

int VS1003_WriteData_DMA_ING(void)
{	
	if(  MP3_DREQ != 0 )
	{	
		if(AudioSize > 0)
		{
			VS1003_WriteData(TxBuffer);
			AudioSize -= SPI_TRANSFER_SIZE;
			TxBuffer += SPI_TRANSFER_SIZE;
//			MP3_DCS(0);   
//			SpiHandle.hdmatx = &VS1003_Tx_DmaHandle;
//			__HAL_DMA_ENABLE_IT(&VS1003_Tx_DmaHandle, DMA_IT_TC);
//			HAL_SPI_Transmit_DMA(&SpiHandle, TxBuffer, 32);
			return 1;
		}
		else
		{
			AudioSize = 0;
			TxBuffer = 0;
//			__HAL_DMA_DISABLE_IT(&VS1003_Tx_DmaHandle, DMA_IT_TC);
			VS1003_Cplt = 1;
			return 0;
		}
	}
	return 2;
} 
/*******************************************************************************
* Function Name  : VS1003_SPI_Init
* Description    : VS1003 SPI Init
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void VS1003_SPI_Init(void)
{
	__HAL_RCC_SPI1_CLK_ENABLE();
	
	/* SPI1 configuration */
	SpiHandle.Instance		= SPI1;
	SpiHandle.State			= HAL_SPI_STATE_RESET;
	SpiHandle.Init.Direction 	= SPI_DIRECTION_2LINES;
	SpiHandle.Init.Mode 		= SPI_MODE_MASTER;
	SpiHandle.Init.DataSize 	= SPI_DATASIZE_8BIT;
	SpiHandle.Init.CLKPolarity 	= SPI_POLARITY_HIGH;
	SpiHandle.Init.CLKPhase 	= SPI_PHASE_2EDGE;
	SpiHandle.Init.NSS 		= SPI_NSS_SOFT;
	SpiHandle.Init.BaudRatePrescaler 	= SPI_BAUDRATEPRESCALER_16;
	SpiHandle.Init.FirstBit 		= SPI_FIRSTBIT_MSB;
	SpiHandle.Init.CRCPolynomial 		= 7;
	SpiHandle.Init.CRCCalculation		= SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.TIMode				= SPI_TIMODE_DISABLE;
	
	HAL_SPI_Init(&SpiHandle);
	__HAL_SPI_ENABLE(&SpiHandle);
}

/*******************************************************************************
* Function Name  : VS1003_SPI_SetSpeed
* Description    : None
* Input          : - SpeedSet: SPI_SPEED_HIGH 1  SPI_SPEED_LOW 0
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void VS1003_SPI_SetSpeed( uint8_t SpeedSet)
{
	if( SpeedSet == SPI_SPEED_LOW )
	{
		hspi3.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_32;
	}
	else 
	{
		hspi3.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_8;
	}
	HAL_SPI_Init(&hspi3);
}

/*******************************************************************************
* Function Name  : VS1003_SPI_ReadWriteByte
* Description    : None
* Input          : - TxData: Write Byte
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static uint8_t VS1003_SPI_ReadWriteByte( uint8_t TxData )
{
	uint8_t tx_data = TxData, rx_data = 0;
	
	if( HAL_SPI_TransmitReceive(&hspi3, &tx_data, &rx_data, 1, 0xFFFFFF) == HAL_OK)
	{
		return rx_data;
	}
	else
	{
		return 0;
	}
	/*
	// Loop while DR register in not emplty 
	while (!__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_TXE));

	// Send byte through the SPI2 peripheral 
	HAL_SPI_Transmit(&SpiHandle, &tx_data, 1, 0xFFFFFF);

	// Wait to receive a byte 
	while (!__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_RXNE));

	// Return the byte read from the SPI bus 
	HAL_SPI_Receive(&SpiHandle, &rx_data, 1, 0xFFFFFF);
	
	return rx_data;
	*/
}

/*******************************************************************************
* Function Name  : VS1003_Init
* Description    : VS1003�˿ڳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void VS1003_Init(void)
{		 
	MP3_Reset(0);
	delay_ms(1);
	MP3_Reset(1);

	MP3_DCS(1); 
	MP3_CCS(1); 
}


/*******************************************************************************
* Function Name  : VS1003_WriteReg
* Description    : VS1003д�Ĵ���
* Input          : - reg: �����ַ
*				   - value: ��������
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_WriteReg( uint8_t reg,uint16_t value )
{  
   while(  MP3_DREQ ==0 );           /* �ȴ����� */
   
   VS1003_SPI_SetSpeed( SPI_SPEED_LOW );	 
   MP3_DCS(1); 
   MP3_CCS(0); 
   VS1003_SPI_ReadWriteByte(VS_WRITE_COMMAND); /* ����VS1003��д���� */
   VS1003_SPI_ReadWriteByte(reg);             
   VS1003_SPI_ReadWriteByte(value>>8);        
   VS1003_SPI_ReadWriteByte(value);	          
   MP3_CCS(1);         
   VS1003_SPI_SetSpeed( SPI_SPEED_HIGH );
} 

/*******************************************************************************
* Function Name  : VS1003_ReadReg
* Description    : VS1003���Ĵ���
* Input          : - reg: �����ַ
* Output         : None
* Return         : - value: ����
* Attention		 : None
*******************************************************************************/
uint16_t VS1003_ReadReg( uint8_t reg)
{ 
   uint16_t value;
    
   while(  MP3_DREQ ==0 );           /* �ȴ����� */
   VS1003_SPI_SetSpeed( SPI_SPEED_LOW );
   MP3_DCS(1);     
   MP3_CCS(0);     
   VS1003_SPI_ReadWriteByte(VS_READ_COMMAND);/* ����VS1003�Ķ����� */
   VS1003_SPI_ReadWriteByte( reg );   
   value = VS1003_SPI_ReadWriteByte(0xff);		
   value = value << 8;
   value |= VS1003_SPI_ReadWriteByte(0xff); 
   MP3_CCS(1);   
   VS1003_SPI_SetSpeed( SPI_SPEED_HIGH );
   return value; 
} 

/*******************************************************************************
* Function Name  : VS1003_ResetDecodeTime
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/                       
void VS1003_ResetDecodeTime(void)
{
   VS1003_WriteReg(SPI_DECODE_TIME,0x0000);
   VS1003_WriteReg(SPI_DECODE_TIME,0x0000); /* �������� */
}

/*******************************************************************************
* Function Name  : VS1003_GetDecodeTime
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/    
uint16_t VS1003_GetDecodeTime(void)
{ 
   return VS1003_ReadReg(SPI_DECODE_TIME);   
} 

void VS1003_StopPlay(void)
{
   uint8_t retry; 	
   			   
   retry = 0;
   VS1003_WriteReg(SPI_MODE,0x0808);  /* SM_OUTOFWAV = 1 */

   while( VS1003_ReadReg(0) != 0x0800 )   /* SM_OUTOFWAV�� clear�� ������ ��ٸ� */
   {
	   VS1003_WriteReg(SPI_MODE,0x0808);  
	   delay_ms(2);    
	   if( retry++ > 100 )
	   { 
	      break; 
	   }
   }	 	

}

/*******************************************************************************
* Function Name  : VS1003_SoftReset
* Description    : VS1003��λ
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_SoftReset(void)
{
   uint8_t retry; 	
   			   
   while(  MP3_DREQ ==0 );   /* �ȴ������λ���� */
   VS1003_SPI_ReadWriteByte(0xff);  /* �������� */
   retry = 0;
   while( VS1003_ReadReg(0) != 0x0804 )   /* �����λ,��ģʽ */  
   {
	   VS1003_WriteReg(SPI_MODE,0x0804);  /* �����λ,��ģʽ */
	   delay_ms(2);                        /* �ȴ�����1.35ms */
	   if( retry++ > 100 )
	   { 
	      break; 
	   }
   }	 	

   while(  MP3_DREQ ==0 );   /* �ȴ������λ���� */

   retry = 0;

   while( VS1003_ReadReg(SPI_CLOCKF) != 0X9800 ) /* ����vs1003��ʱ��,3��Ƶ ,1.5xADD */ 
   {
	   VS1003_WriteReg(SPI_CLOCKF,0X9800);       /* ����vs1003��ʱ��,3��Ƶ ,1.5xADD */
	   if( retry++ > 100 )
	   { 
	      break; 
	   }
   }		   
   retry = 0;

   while( VS1003_ReadReg(SPI_AUDATA) != 0XBB81 ) /* ����vs1003��ʱ��,3��Ƶ ,1.5xADD */
   {
	   VS1003_WriteReg(SPI_AUDATA,0XBB81);
	   if( retry++ > 100 )
	   { 
	      break; 
	   }   
   }

   VS1003_WriteReg(11,0x2020);	 /* ���� */			 
   VS1003_ResetDecodeTime();     /* ��λ����ʱ�� */	     
   /* ��vs1003����4���ֽ���Ч���ݣ���������SPI���� */	
   MP3_DCS(0);  /* ѡ�����ݴ��� */
   VS1003_SPI_ReadWriteByte(0XFF);
   VS1003_SPI_ReadWriteByte(0XFF);
   VS1003_SPI_ReadWriteByte(0XFF);
   VS1003_SPI_ReadWriteByte(0XFF);
   MP3_DCS(1);  /* ȡ�����ݴ��� */
   delay_ms(20);
} 

/*******************************************************************************
* Function Name  : VS1003_Reset
* Description    : VS1003��λ
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_Reset(void)
{
   delay_ms(20);
   VS1003_SPI_ReadWriteByte(0XFF);
   MP3_DCS(1);              /* ȡ�����ݴ��� */
   MP3_CCS(1);              /* ȡ�����ݴ��� */

   while(  MP3_DREQ ==0 );  /* �ȴ�DREQΪ�� */
   delay_ms(20);			 
}

/*******************************************************************************
* Function Name  : VS1003_SineTest
* Description    : VS1003���Ҳ���
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_SineTest(void)
{	
   VS1003_WriteReg(0x0b,0X2020);	  /* �������� */	 
   VS1003_WriteReg(SPI_MODE,0x0820); /* ����vs1003�Ĳ���ģʽ */	    
   while(  MP3_DREQ ==0 );  /* �ȴ�DREQΪ�� */
 	
   /* ��vs1003�������Ҳ������0x53 0xef 0x6e n 0x00 0x00 0x00 0x00 */
   MP3_DCS(0);/* ѡ�����ݴ��� */
   VS1003_SPI_ReadWriteByte(0x53);
   VS1003_SPI_ReadWriteByte(0xef);
   VS1003_SPI_ReadWriteByte(0x6e);
   VS1003_SPI_ReadWriteByte(0x24);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1); 
   /* �˳����Ҳ��� */
   MP3_DCS(0);/* ѡ�����ݴ��� */
   VS1003_SPI_ReadWriteByte(0x45);
   VS1003_SPI_ReadWriteByte(0x78);
   VS1003_SPI_ReadWriteByte(0x69);
   VS1003_SPI_ReadWriteByte(0x74);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1);		 

   /* �ٴν������Ҳ��Բ�����nֵΪ0x44���������Ҳ���Ƶ������Ϊ�����ֵ */
   MP3_DCS(0);/* ѡ�����ݴ��� */      
   VS1003_SPI_ReadWriteByte(0x53);
   VS1003_SPI_ReadWriteByte(0xef);
   VS1003_SPI_ReadWriteByte(0x6e);
   VS1003_SPI_ReadWriteByte(0x44);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1);
   /* �˳����Ҳ��� */
   MP3_DCS(0);/* ѡ�����ݴ��� */      
   VS1003_SPI_ReadWriteByte(0x45);
   VS1003_SPI_ReadWriteByte(0x78);
   VS1003_SPI_ReadWriteByte(0x69);
   VS1003_SPI_ReadWriteByte(0x74);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(100);
   MP3_DCS(1);	 
}	 

/*******************************************************************************
* Function Name  : VS1003_RamTest
* Description    : VS1003 RAM����
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/																			 
void VS1003_RamTest(void)
{
   volatile uint16_t value;
     
   VS1003_Reset();     
   VS1003_WriteReg(SPI_MODE,0x0820);/* ����vs1003�Ĳ���ģʽ */
   while(  MP3_DREQ ==0 );           /* �ȴ�DREQΪ�� */
   MP3_DCS(0);	       			     /* xDCS = 1��ѡ��vs1003�����ݽӿ� */
   VS1003_SPI_ReadWriteByte(0x4d);
   VS1003_SPI_ReadWriteByte(0xea);
   VS1003_SPI_ReadWriteByte(0x6d);
   VS1003_SPI_ReadWriteByte(0x54);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   delay_ms(50);  
   MP3_DCS(1);
   value = VS1003_ReadReg(SPI_HDAT0); /* ����õ���ֵΪ0x807F���������� */
}     
		 				
/*******************************************************************************
* Function Name  : VS1003_SetVol
* Description    : �趨vs1003���ŵ������͸ߵ���
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/   
void VS1003_SetVol(void)
{
   uint8_t i;
   uint16_t bass=0;  /* �ݴ������Ĵ���ֵ */
   uint16_t volt=0;  /* �ݴ�����ֵ */
   uint8_t  vset=0;  /* �ݴ�����ֵ */ 
		 
   vset = 255 - vs1003ram[4]; /* ȡ���õ����ֵ */ 
   volt =vset;
   volt <<= 8;
   volt += vset;/* �õ��������ú��С */
   /* 0,henh.1,hfreq.2,lenh.3,lfreq */      
   for( i = 0; i < 4; i++ )
   {
       bass <<= 4;
       bass += vs1003ram[i]; 
   }     
   VS1003_WriteReg(SPI_BASS,bass);/* BASS */   
   VS1003_WriteReg(SPI_VOL,volt); /* ������ */ 
}    

/*******************************************************************************
* Function Name  : VS1003_WriteData
* Description    : VS1003д����
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/  
void VS1003_WriteData( uint8_t * buf)
{		
   uint8_t  count = SPI_TRANSFER_SIZE;

   MP3_DCS(0);   
   while( count-- )
   {
	  VS1003_SPI_ReadWriteByte( *buf++ );
   }
   MP3_DCS(1);  
   MP3_CCS(1);  
} 

/*******************************************************************************
* Function Name  : VS1003_Record_Init
* Description    : VS1003¼����ʼ��
* Input          : None
* Output         : None
* Return         : None
* Attention		 : VS1003¼����ʼ���ɹ�֮��������������лط�
*                  ��ʱ�������������ײ����Լ�
*******************************************************************************/ 
void VS1003_Record_Init(void)
{
  uint8_t retry; 	

  /* ����ʱ�� */
  while( VS1003_ReadReg(SPI_CLOCKF) != 0x9800 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x9800);   
	  delay_ms(2);                        /* �ȴ�����1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  while( VS1003_ReadReg(SPI_BASS) != 0x0000 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x0000);   
	  delay_ms(2);                        /* �ȴ�����1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* Set sample rate divider=12 */ 
  while( VS1003_ReadReg(SPI_AICTRL0) != 0x0012 )   
  {
	  VS1003_WriteReg(SPI_AICTRL0,0x0012);   
	  delay_ms(2);                        /* �ȴ�����1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* AutoGain OFF, reclevel 0x1000 */ 
  while( VS1003_ReadReg(SPI_AICTRL1) != 0x1000 )   
  {
	  VS1003_WriteReg(SPI_AICTRL1,0x1000);   
	  delay_ms(2);                        /* �ȴ�����1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* RECORD,NEWMODE,RESET */ 
  while( VS1003_ReadReg(SPI_MODE) != 0x1804 )   
  {
	  VS1003_WriteReg(SPI_MODE,0x1804);   
	  delay_ms(2);                        /* �ȴ�����1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  while( VS1003_ReadReg(SPI_CLOCKF) != 0x9800 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x9800);   
	  delay_ms(2);                        /* �ȴ�����1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

