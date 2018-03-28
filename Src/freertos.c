/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "includes.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId TouchTaskHandle;
osThreadId GuiTaskHandle;
osThreadId SoundTaskHandle;
osThreadId FileTaskHandle;

/* USER CODE BEGIN Variables */
static const TCHAR * AvailableExt[] = {
	"MP3",
	"WAV",
	0
};
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartTouchTask(void const * argument);
void StartGuiTask(void const * argument);
void StartSoundTask(void const * argument);
void StartFileTask(void const * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) 
{
  /* USER CODE BEGIN Init */
       
	LCD_Initializtion();
	HAL_Delay(500);
	GUI_Init();
	SD_Msg_Init();
	VS1003_Init();
	VS1003_Reset();
	VS1003_SoftReset();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of TouchTask */
  osThreadDef(TouchTask, StartTouchTask, osPriorityAboveNormal, 0, 256);
  TouchTaskHandle = osThreadCreate(osThread(TouchTask), NULL);

  /* definition and creation of GuiTask */
  osThreadDef(GuiTask, StartGuiTask, osPriorityNormal, 0, 512);
  GuiTaskHandle = osThreadCreate(osThread(GuiTask), NULL);

  /* definition and creation of SoundTask */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* StartTouchTask function */
void StartTouchTask(void const * argument)
{
  /* USER CODE BEGIN StartTouchTask */
  /* Infinite loop */
  for(;;)
  {
	GUI_TOUCH_Exec();
	osDelay(100);
  }
  /* USER CODE END StartTouchTask */
}


/* StartGuiTask function */
void StartGuiTask(void const * argument)
{
	int list_now=0, list_prev=0;
	int btn_now=0, btn_prev=0;
	int type;
	char * name;
	LISTBOX_Handle hListBox;
  /* USER CODE BEGIN StartGuiTask */
	
	osThreadDef(FileTask, StartFileTask, osPriorityAboveNormal, 0, 512);
	/* init code for FATFS */
	MX_FATFS_Init();
	// Mount FileSystem
	type = FileSystem_Init();

	// Touch Calibration
	_ExecCalibration();
	GUI_Clear();
	
	// Create WMGUI
	CreateUserGUI();
	
	// Get current directory contents
	GetCurrentDir();
	// Update File list to Listbox
	hListBox = GUI_GetListBoxHandle();
	UpdateList(hListBox);
	GUI_SetDirPath(GetCurrentFilePath());
	// set default volume
	Sound_ControlVolume(SOUND_VOLUME_DEF);
	GUI_SetVolume(Sound_GetVolume());
	//Sound_VolOff();
	// validate GUI
	WM_ExecIdle();
	MemAlloc();
	
	if(type == 0)
	{
		GUI_SetCurrentState("mount fail");
	}
	
  /* Infinite loop */
  for(;;)
  {
	list_now = LISTBOX_GetSel(hListBox);
	if(list_now != list_prev)
	{
		list_prev = list_now;
		type = ToSelecteDir(hListBox, list_now);
		if(type == TYPE_DIRECTORY || type == TYPE_UPPER)
		{
			GetCurrentDir();
			UpdateList(hListBox);
			GUI_SetDirPath(GetCurrentFilePath());
		}
		if(type == TYPE_FILE)
		{
			if(CheckSelectedFileExtension(hListBox, list_now, AvailableExt))
			{
				if(GetPlayState() != SOUND_PLAY_NONE)
				{
					GUI_SetCurrentState("Force Stop...");
					Sound_ReqEnd();
					osSignalWait(PLAY_SOUND_CPLT, osWaitForever);
					osDelay(30);
				} 
				name = GetSelectedFilePath(hListBox, list_now);
				FileTaskHandle = osThreadCreate(osThread(FileTask), name);
				if(FileTaskHandle == 0)
					GUI_SetCurrentState("File Task Failed");
				else
					GUI_SetCurrentState("Start");
			}
			else
			{
				GUI_SetCurrentState("Can't Play");
			}
		}
	}
	btn_now = GUI_GetKey();
	if((btn_now == 0) && (btn_now != btn_prev))
	{
		switch(btn_prev)
		{
			case BTN_BACK_ID:
				ToUpperDir();
				GetCurrentDir();
				UpdateList(hListBox);
				GUI_SetDirPath(GetCurrentFilePath());
				break;
			case BTN_SELECT_ID:
				VS1003_SineTest();
				break;
			case BTN_STOP_ID:
				if(GetPlayState() != SOUND_PLAY_NONE)
				{
					//osSignalSet(FileTaskHandle, PLAY_STOP);
					Sound_ReqEnd();
					osSignalWait(PLAY_SOUND_CPLT, osWaitForever);
					osDelay(30);
				}
				break;
			case BTN_VOL_UP_ID:
				Sound_ControlVolume(SOUND_VOLUME_UP);
				GUI_SetVolume(Sound_GetVolume());
				break;
			case BTN_VOL_DN_ID:
				Sound_ControlVolume(SOUND_VOLUME_DN);
				GUI_SetVolume(Sound_GetVolume());
				break;
			case BTN_PAUSE_ID:
				if(GetPlayState() != SOUND_PLAY_NONE)
				{
					if(GetPlayState() == SOUND_PLAY_PAUSE)
					{
						GUI_SetPause(GUI_BTN_PAUSE_OFF);
						SetPlayState(SOUND_PLAY_PLAYING);
						osThreadResume(FileTaskHandle);
						osThreadResume(SoundTaskHandle);
					}
					else
					{
						GUI_SetPause(GUI_BTN_PAUSE_ON);
						SetPlayState(SOUND_PLAY_PAUSE);
						osThreadSuspend(FileTaskHandle);
						osThreadSuspend(SoundTaskHandle);
					}
				}
				break;
		}
	}
	btn_prev = btn_now;
	
	Sound_SetVolume(GUI_GetVolume());
	//GUI_Text(90,0,(uint8_t*)"test",Red,Black);
	GUI_Delay(100);
  }
  /* USER CODE END StartGuiTask */
}

uint8_t temp[100];
/* StartSoundTask function */
void StartSoundTask(void const * argument)
{
  /* USER CODE BEGIN StartSoundTask */
  /* Infinite loop */
	for(;;)
	{
		if(SOUND_OK != Sound_Playing())
			break;
		osDelay(1);
	}
	
	osSignalSet(FileTaskHandle, PLAY_SOUND_END);
    osThreadTerminate(NULL);
  /* USER CODE END StartSoundTask */
}

void StartFileTask(void const * argument)
{
	extern uint32_t sd_error_test;
	TCHAR * path = (TCHAR *)argument;
	sound_result_t ret;
	osThreadDef(SoundTask, StartSoundTask, osPriorityHigh, 0, 512);
	
	GUI_SetDirPath(path);
		
	VS1003_SoftReset();
	VS1003_BufferZero();
	Sound_ControlVolume(SOUND_VOLUME_DEF);
	ret = Sound_Init(path);
	if(SOUND_OK != ret)
	{
		switch(ret)
		{
			case SOUND_END:
				GUI_SetCurrentState("Init: FileEnd");
				break;
			case SOUND_ERR:
				GUI_SetCurrentState("Init: Err");
				break;
			case SOUND_OPEN_FAIL:
				sprintf(temp,"Init error: %d  ", sd_error_test);
				GUI_Text(0, 140,(uint8_t*)temp,Red,Black);
				FileSystem_Init();
				GUI_SetCurrentState("Init: Open Fail");
				break;
			case SOUND_READ_FAIL:
				sprintf(temp,"Init error: %d  ", sd_error_test);
				GUI_Text(0, 140,(uint8_t*)temp,Red,Black);
				FileSystem_Init();
				GUI_SetCurrentState("Init: Read Fail");
				break;
			default: 
				GUI_SetCurrentState("Init: Undefined");
				break;
		}
		goto sound_end;
	}
	// 사운드 재생 스레드 실행
	SoundTaskHandle = osThreadCreate(osThread(SoundTask), NULL);
	if(SoundTaskHandle == 0)
	{
		GUI_SetCurrentState("Sound Task Failed");
		Sound_Deinit(SOUND_CLOSE_FILE_ON);		// memory free
		osThreadTerminate(NULL);
	}
	
	ret = Sound_PlayStart();
	if(SOUND_OK != ret)
	{
		switch(ret)
		{
			case SOUND_END:
				GUI_SetCurrentState("Start: FileEnd");
				break;
			case SOUND_ERR:
				GUI_SetCurrentState("Start: Err");
				break;
			case SOUND_OPEN_FAIL:
				sprintf(temp,"Init error: %d  ", sd_error_test);
				GUI_Text(0, 140,(uint8_t*)temp,Red,Black);
				FileSystem_Init();
				GUI_SetCurrentState("Start: Open Fail");
				break;
			case SOUND_READ_FAIL:
				sprintf(temp,"Init error: %d  ", sd_error_test);
				GUI_Text(0, 140,(uint8_t*)temp,Red,Black);
				FileSystem_Init();
				GUI_SetCurrentState("Start: Read Fail");
				break;
			default: 
				GUI_SetCurrentState("Start: Undefined");
				break;
		}
		goto sound_end;
	}
	
	GUI_SetCurrentState("Buffering...");
	for(;;)
	{
		ret = Sound_Buffering();
		if(SOUND_OK != ret)
			break;
		osDelay(10);
	}
	
	osSignalWait(PLAY_SOUND_END, osWaitForever);
	GUI_SetCurrentState("Stop Completely");
	
sound_end:
	Sound_Deinit(SOUND_CLOSE_FILE_OFF);		// memory free
	osSignalSet(GuiTaskHandle, PLAY_SOUND_CPLT);
    osThreadTerminate(NULL);
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
