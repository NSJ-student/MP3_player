#include "includes.h"

// Static Memory
static play_state_t play_state = SOUND_PLAY_NONE;
static buffering_state_t buffer_state = PLAY_NONE;
static buffering_state_t sound_state = PLAY_NONE;
static uint32_t audio_total_size;
static uint32_t audio_play_size;
static uint32_t volume_cur = 100, volume_pre = 100;

extern uint8_t temp[50];

#if (SOUND_USE_ALLOC != 0)

// Dynamic Memory
GUI_HMEM hFile;
FIL * ptrFile;
// 		Audio Buffer 
GUI_HMEM hMem1, hMem2;
char * ptrAudio1, *ptrAudio2;
// 		Sound Message 
GUI_HMEM hMsgMem1, hMsgMem2, hMsgMemEnd;
audio_msg_t * ptrMsg1, *ptrMsg2, *ptrMsgEnd;

#else

// Dynamic Memory
FIL orgFile;
FIL * ptrFile = &orgFile;
// 		Audio Buffer 
char ptrAudio1[512];
char ptrAudio2[512];
// 		Sound Message 
audio_msg_t orgMsg1, orgMsg2, orgMsgEnd;
audio_msg_t * ptrMsg1 = &orgMsg1;
audio_msg_t * ptrMsg2 = &orgMsg2;
audio_msg_t * ptrMsgEnd = &orgMsgEnd;

#endif

osMessageQId AudioQueue;
osMessageQDef(AudioQueue, 3, audio_msg_t);

static void MemFree(void);

void Sound_VolOff(void)
{
	while(  MP3_DREQ ==0 );
	VS1003_WriteReg(SPI_VOL, 0xFEFE);
}
void Sound_ControlVolume(uint8_t updn)
{
	extern osThreadId SoundTaskHandle;
	
	if((updn == SOUND_VOLUME_UP) && (volume_cur >= 10))
		volume_cur -= SOUND_VOLUME_STEP;
	if((updn == SOUND_VOLUME_DN) && (volume_cur <= 240))
		volume_cur += SOUND_VOLUME_STEP;
	
	sprintf(temp, "%3d", volume_cur);
	GUI_Text(SLD_VOLUME_X+SLD_VOLUME_W, SLD_VOLUME_Y, 
				(uint8_t *)temp, Black, Red);
	
	if(play_state != SOUND_PLAY_PLAYING)
	{
		volume_pre = volume_cur;
		while(  MP3_DREQ ==0 );
		VS1003_WriteReg(SPI_VOL,  (volume_cur | (volume_cur<<8))  );
	}
}
void Sound_SetVolume(uint8_t vol)
{
	extern osThreadId SoundTaskHandle;
	
	if(vol == volume_cur)
		return;
	
	volume_cur = vol;
	
	sprintf(temp, "%3d", volume_cur);
	GUI_Text(SLD_VOLUME_X+SLD_VOLUME_W, SLD_VOLUME_Y, 
				(uint8_t *)temp, Black, Red);
	
	if(play_state != SOUND_PLAY_PLAYING)
	{
		volume_pre = volume_cur;
		while(  MP3_DREQ ==0 );
		VS1003_WriteReg(SPI_VOL,  (volume_cur | (volume_cur<<8))  );
	}
}
uint8_t Sound_GetVolume(void)
{
	return volume_cur;
}
play_state_t GetPlayState(void)
{
	return play_state;
}
void SetPlayState(play_state_t state)
{
	play_state = state;
}

int Sound_Init(char * f_path)
{
	FRESULT fres;
	uint32_t audio_start_pos = 0;
	uint32_t readByte;
	MP3_ID3v2 * mp3_hdr;
	
	if(play_state != SOUND_PLAY_NONE)
	{
		Sound_Deinit(SOUND_CLOSE_FILE_OFF);
		return SOUND_ERR;
	}
	
	
	fres = User_f_open(ptrFile, f_path, FA_OPEN_EXISTING | FA_READ);
	if(fres != FR_OK)
	{
		sprintf(temp,"Init: %d  ", fres);
		GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
		MemFree();
		return SOUND_OPEN_FAIL;
	}

	if(CheckExtension(f_path, "MP3"))
	{
		// MP3 헤더 읽기 및 분석
		audio_start_pos = MP3_FindStartPos(ptrFile);
		if(audio_start_pos == AUDIO_FILE_FAIL)
		{
			User_f_close(ptrFile);
			MemFree();
			return SOUND_READ_FAIL;
		}
		//MP3_FindFileLen(ptrFile, audio_start_pos);
		User_f_lseek(ptrFile, audio_start_pos);
		GUI_Text(0, 220, "MP3",Red,Black);
	}
	if(CheckExtension(f_path, "WAV"))
	{
		audio_start_pos = sizeof(WAV_Header);
		sprintf(temp,"Time: %.3f sec ", WVA_CalPlayTime(ptrFile));
		GUI_SetFileLen(temp);
		GUI_Text(0, 220, "WAV",Red,Black);
		User_f_lseek(ptrFile, 0);
	}
	//VS1003_PreparePlay();
	audio_total_size = ptrFile->obj.objsize - audio_start_pos;
	audio_play_size = 0;
	
	AudioQueue = osMessageCreate(osMessageQ(AudioQueue), NULL);
	
	play_state = SOUND_PLAY_INIT;
	buffer_state = PLAY_NONE;
	
	return SOUND_OK;
}

sound_result_t Sound_PlayStart(void)
{
	uint32_t readByte;
	FRESULT fres;
	
	if(play_state == SOUND_PLAY_INIT)
	{
		fres = User_f_read (ptrFile, ptrAudio1, SOUND_BUFFER_SIZE, &readByte);
		audio_play_size += readByte;

		if(readByte == 0)	buffer_state = PLAY_BUFFER_END;
		else				buffer_state = PLAY_BUFFER_1;
		ptrMsg1->play_state = buffer_state;
		ptrMsg1->buffer = ptrAudio1;
		ptrMsg1->data_size = readByte;
		osMessagePut(AudioQueue, (uint32_t)ptrMsg1, osWaitForever);
		
		if(fres != FR_OK)
		{
			sprintf(temp,"File1: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_READ_FAIL;
		}
		if(buffer_state == PLAY_BUFFER_END)
		{
			sprintf(temp,"File1: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_END;
		}
		
		fres = User_f_read (ptrFile, ptrAudio2, SOUND_BUFFER_SIZE, &readByte);
		audio_play_size += readByte;
		
		if(readByte == 0)	buffer_state = PLAY_BUFFER_END;
		else				buffer_state = PLAY_BUFFER_2;
		ptrMsg2->play_state = buffer_state;
		ptrMsg2->buffer = ptrAudio2;
		ptrMsg2->data_size = readByte;
		osMessagePut(AudioQueue, (uint32_t)ptrMsg2, osWaitForever);
		
		play_state = SOUND_PLAY_PLAYING;
		buffer_state = PLAY_BUFFER_1;
		
		if(fres != FR_OK)
		{
			sprintf(temp,"File2: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_READ_FAIL;
		}
		if(buffer_state == PLAY_BUFFER_END)
		{
			sprintf(temp,"File2: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_END;
		}
		return SOUND_OK;
	}
	return SOUND_ERR;
}

sound_result_t Sound_Buffering(void)
{
	osEvent evt2;
	int currentSignal;
	sound_result_t ret = SOUND_OK;
	uint32_t readByte;
	FRESULT fres;
	
	if(play_state != SOUND_PLAY_PLAYING)
		return SOUND_ERR;
	
	do
	{
		evt2 = osSignalWait(0, osWaitForever);
	} while(evt2.status != osEventSignal);
	currentSignal = evt2.value.signals;
	
	//if(buffer_state == PLAY_BUFFER_1)
	if(currentSignal & PLAY_BUFFER_1)
	{
		//osSignalWait(PLAY_BUFFER_1, osWaitForever);
		fres = User_f_read (ptrFile, ptrAudio1, SOUND_BUFFER_SIZE, &readByte);
		audio_play_size += readByte;
		
		if(readByte == 0)	buffer_state = PLAY_BUFFER_END;
		else				buffer_state = PLAY_BUFFER_1;
		
		ptrMsg1->play_state = buffer_state;
		ptrMsg1->buffer = ptrAudio1;
		ptrMsg1->data_size = readByte;
		osMessagePut(AudioQueue, (uint32_t)ptrMsg1, osWaitForever);

		if(fres != FR_OK)
		{
			sprintf(temp,"File1: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_ERR;
		}
		if(buffer_state == PLAY_BUFFER_END)
		{
			sprintf(temp,"File1: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_END;
		}
		buffer_state = PLAY_BUFFER_2;
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		
		ret = SOUND_OK;
	}
	//if(buffer_state == PLAY_BUFFER_2)
	if(currentSignal & PLAY_BUFFER_2)
	{
		//sSignalWait(PLAY_BUFFER_2, osWaitForever);
		fres = User_f_read (ptrFile, ptrAudio2, SOUND_BUFFER_SIZE, &readByte);
		audio_play_size += readByte;
		
		if(readByte == 0)	buffer_state = PLAY_BUFFER_END;
		else				buffer_state = PLAY_BUFFER_2;
		
		ptrMsg2->play_state = buffer_state;
		ptrMsg2->buffer = ptrAudio2;
		ptrMsg2->data_size = readByte;
		osMessagePut(AudioQueue, (uint32_t)ptrMsg2, osWaitForever);

		if(fres != FR_OK)
		{
			sprintf(temp,"File2: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_ERR;
		}
		if(buffer_state == PLAY_BUFFER_END)
		{
			sprintf(temp,"File2: size(%d) / %d  ", readByte, fres);
			GUI_Text(0, 200,(uint8_t*)temp,Red,Black);
			User_f_close(ptrFile);
			return SOUND_END;
		}
		buffer_state = PLAY_BUFFER_1;
		
		ret = SOUND_OK;
	}
	
	if(currentSignal & PLAY_STOP)
	{
		buffer_state = PLAY_SOUND_END;
		ptrMsgEnd->play_state = PLAY_SOUND_END;
		ptrMsgEnd->buffer = 0;
		ptrMsgEnd->data_size = 0;
		osMessagePut(AudioQueue, (uint32_t)ptrMsgEnd, osWaitForever);
		User_f_close(ptrFile);
		
		ret = SOUND_END;
	}
	
	return ret;
}

sound_result_t Sound_Playing(void)
{
	// from freertos.c
	extern osThreadId GuiTaskHandle;
	extern osThreadId FileTaskHandle;
	osEvent evt;
	audio_msg_t * ptrMsg;
	
	evt = osMessageGet(AudioQueue, osWaitForever);
	if (evt.status == osEventMessage) {
		ptrMsg = (audio_msg_t *)evt.value.p;
		sound_state = ptrMsg->play_state;
		
		if(sound_state == PLAY_SOUND_END)
			return SOUND_END;
		if(ptrMsg->data_size == 0)
			return SOUND_END;
		
		// Write MP3 buffer
		VS1003_WriteData_DMA((uint8_t *)ptrMsg->buffer, ptrMsg->data_size);
		// Wait for SPI DMA tx cplt
		evt = osSignalWait(SOUND_DMA_CPLT, osWaitForever);
		
		if(sound_state == PLAY_BUFFER_END)
			return SOUND_END;
		
		// Notify available buffer to FileTask 
		osSignalSet(FileTaskHandle, sound_state);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		
		// Set Volume
		if(volume_pre != volume_cur)
		{
			volume_pre = volume_cur;
			VS1003_WriteReg(SPI_VOL,  (volume_cur | (volume_cur<<8)));
		}
		
		return SOUND_OK;
	}
	
	return SOUND_ERR;
}

void Sound_Deinit(int close_file)
{
	if(close_file)
	{
		User_f_close(ptrFile);
	}
	MemFree();
	audio_play_size = 0;
	audio_total_size = 0;
	play_state = SOUND_PLAY_NONE;
}



int MemAlloc(void)
{
#if (SOUND_USE_ALLOC != 0)
// FIL Alloc
	hFile = GUI_ALLOC_AllocZero(sizeof(FIL));
	if(!hFile)
	{
		MemFree();
		return SOUND_ALLOC_ERR;
	}
	ptrFile = GUI_ALLOC_h2p(hFile);
// Audio Buffer Alloc
	hMem1 = GUI_ALLOC_AllocZero(SOUND_BUFFER_SIZE);
	if(!hMem1)
	{
		MemFree();
		return SOUND_ALLOC_ERR;
	}
	ptrAudio1 = GUI_ALLOC_h2p(hMem1);
	
	hMem2 = GUI_ALLOC_AllocZero(SOUND_BUFFER_SIZE);
	if(!hMem2)
	{
		MemFree();
		return SOUND_ALLOC_ERR;
	}
	ptrAudio2 = GUI_ALLOC_h2p(hMem2);
// Message Alloc
	hMsgMem1 = GUI_ALLOC_AllocZero(sizeof(audio_msg_t));
	if(!hMsgMem1)
	{
		MemFree();
		return SOUND_ALLOC_ERR;
	}
	ptrMsg1 = (audio_msg_t *)GUI_ALLOC_h2p(hMsgMem1);
	hMsgMem2 = GUI_ALLOC_AllocZero(sizeof(audio_msg_t));
	if(!hMsgMem2)
	{
		MemFree();
		return SOUND_ALLOC_ERR;
	}
	ptrMsg2 = (audio_msg_t *)GUI_ALLOC_h2p(hMsgMem2);
	hMsgMemEnd = GUI_ALLOC_AllocZero(sizeof(audio_msg_t));
	if(!hMsgMemEnd)
	{
		MemFree();
		return SOUND_ALLOC_ERR;
	}
	ptrMsgEnd = (audio_msg_t *)GUI_ALLOC_h2p(hMsgMemEnd);
	
	return SOUND_OK;
#else
	return SOUND_OK;
#endif
}
static void MemFree(void)
{
#if (SOUND_USE_ALLOC != 0)
	if(hMsgMem1)	GUI_ALLOC_Free(hMsgMem1);
	if(hMsgMem2)	GUI_ALLOC_Free(hMsgMem2);
	if(hMsgMemEnd)	GUI_ALLOC_Free(hMsgMemEnd);
	if(hMem1)	GUI_ALLOC_Free(hMem1);
	if(hMem2)	GUI_ALLOC_Free(hMem2);
	if(hFile)	GUI_ALLOC_Free(hFile);
	hMsgMem1 = 0;
	hMsgMem2 = 0;
	hMsgMemEnd = 0;
	hMem1 = 0;
	hMem2 = 0;
	hFile = 0;
#else
	return;
#endif
}

/*
pretime = GUI_X_GetTime();
curtime = GUI_X_GetTime();
sprintf(temp,"AUDIO : %d  ", curtime-pretime);
GUI_Text(0, 130,(uint8_t*)temp,Red,Black);
*/
