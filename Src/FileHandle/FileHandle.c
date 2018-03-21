#include "includes.h"

/*********************************************************************
*
*       Variable for API
*/

struct path_tag {
	uint32_t size;
	TCHAR path[256];
} dir_path = {3, {'0', ':', '\\', 0, }};


osMutexId FatKey = 0;
osMutexDef(FatKey);

FATFS 		FileObject;		// Work area (file system object) for logical driv
DIR 		curr_dir;
file_element_t * curr_files;
TCHAR temp_str[50];

/*********************************************************************
*
*       functions
*/
void Add_File(file_element_t * node, GUI_HMEM hNode, TCHAR * fname, int type)
{
	int flen = strlen(fname);
	
	node->type = type;
	node->hThis = hNode;
	node->hMem = GUI_ALLOC_AllocZero(flen + 1);
	node->ptrMem = GUI_ALLOC_h2p(node->hMem);
	memcpy((void*)node->ptrMem, fname, flen);
	(node->ptrMem)[flen] = 0;
	
	if(curr_files == 0)
	{
		node->prev = 0;
		node->next = 0;
		curr_files = node;
	}
	else
	{
		curr_files->prev = node;
		node->prev = 0;
		node->next = curr_files;
		curr_files = node;
	}
}

void Insert_File(file_element_t * node, file_element_t * pos)
{
	if(node->prev != 0)
		node->prev->next = node->next;
	if(node->next != 0)
		node->next->prev = node->prev;
	
	if(pos == 0)
	{
		if(curr_files == 0)
		{
			curr_files = node;
		}
		else
		{
			curr_files->prev = node;
			node->prev = 0;
			node->next = curr_files;
			curr_files = node;
		}
	}
	else
	{
		node->prev = pos;
		node->next = pos->next;
		if(pos->next != 0)
		{
			pos->next->prev = node;
		}
		pos->next = node;
	}
}

void Arrange_File(void)
{
	file_element_t * files = curr_files;
	file_element_t * temp;
	
	while(files != 0)
	{
		temp = files;
		files = files->next;
		if(temp->type == TYPE_DIRECTORY)
		{
			Insert_File(temp, (file_element_t *)0);
		}
	}
}

void Clear_File(void)
{
	GUI_HMEM hTemp;
	
	if(curr_files == 0)	return;
	
	while(curr_files != 0)
	{
		GUI_ALLOC_Free(curr_files->hMem);
		hTemp = curr_files->hThis;
		curr_files = curr_files->next;
		GUI_ALLOC_Free(hTemp);
	}
}


int FileSystem_Init(void)		// 파일 시스템 초기화
{
	extern void User_LinkDmaHandle(SD_HandleTypeDef* sdHandle);
	extern SD_HandleTypeDef hsd;
	if(FatKey == 0)
		FatKey = osMutexCreate(osMutex(FatKey));
	
	User_LinkDmaHandle(&hsd);
	if(FR_OK == f_mount(&FileObject, "0", 1))
	{
		if(FR_OK == f_opendir(&curr_dir, dir_path.path))
		{
			f_closedir(&curr_dir);
			return 1;
		}
	}
	
	return 0;
}

FRESULT ToUpperDir(void)			// 이전 디렉토리로 이동
{
	FRESULT res;
	int idx;
	int org_size = dir_path.size;
	
	for(idx=dir_path.size-1; idx > 0; idx--)
	{
		if(dir_path.path[idx] == '\\')
		{
			if(dir_path.path[idx-1] != ':')
			{
				dir_path.path[idx] = 0;
				dir_path.size = idx;
				break;
			}
			else
			{
				idx++;
				dir_path.path[idx] = 0;
				dir_path.size = idx;
				break;
			}
		}
	}
    osMutexWait(FatKey, osWaitForever);
	res = f_opendir(&curr_dir, dir_path.path);
	if(res != FR_OK)
	{
		dir_path.path[org_size-1] = 0;
		dir_path.size = org_size;
	}
	else
		f_closedir(&curr_dir);
	osMutexRelease(FatKey);
	return res;
}

FRESULT ToDownDir(TCHAR * dir_name)	// 내부 디렉토리로 이동
{
	FRESULT res;
	int cnt = 0;
	int org_size = dir_path.size;
	
	if(dir_path.path[dir_path.size-1] != '\\')
	{
		dir_path.path[dir_path.size] = '\\';
		dir_path.size++;
	}
	
	for(cnt=0; *((TCHAR *)dir_name+cnt) != 0; cnt++)
	{
		dir_path.path[dir_path.size] = *(dir_name+cnt);
		dir_path.size++;
	}
	dir_path.path[dir_path.size] = 0;
	
    osMutexWait(FatKey, osWaitForever);
	res = f_opendir(&curr_dir, dir_path.path);
	if(res != FR_OK)
	{
		dir_path.path[org_size-1] = 0;
		dir_path.size = org_size;
	}
	else
		f_closedir(&curr_dir);
	osMutexRelease(FatKey);
	return res;
}

int ToSelecteDir(LISTBOX_Handle hList, int idx)
{
	int type;
	
	LISTBOX_GetItemText(hList, idx, temp_str, 50);
	type = GetMatchedType(temp_str);
	if(type == TYPE_DIRECTORY)
	{
		if(FR_OK != ToDownDir(temp_str))
			return TYPE_NONE;
	}
	else if(type == TYPE_UPPER)
	{
		if(FR_OK != ToUpperDir())
			return TYPE_NONE;
	}
	
	return type;
}

int GetMatchedType(TCHAR * filename)
{
	file_element_t * start = curr_files;
	
	if(!memcmp("..", filename, 2))
	{
		return TYPE_UPPER;
	}
	
	while(start)
	{
		if(!memcmp(filename, start->ptrMem, strlen(filename)))
		{
			return start->type;
		}
		start = start->next;
	}
	
	return 0;
}

TCHAR * GetFilePath(TCHAR * name)
{
	static TCHAR FilePath[100] = {0,};
	uint32_t cnt = 0, offset = 0;
	
	memcpy(FilePath, dir_path.path, dir_path.size);
	if(dir_path.path[dir_path.size-1] != '\\')
	{
		FilePath[dir_path.size] = '\\';
		offset = 1;
	}
	
	for(cnt=0; *((TCHAR *)name+cnt) != 0; cnt++)
	{
		FilePath[dir_path.size+offset+cnt] = *(name+cnt);
	}
	FilePath[dir_path.size+offset+cnt] = 0;
	
	return FilePath;
}

TCHAR * GetSelectedFilePath(LISTBOX_Handle hList, int idx)
{
	static TCHAR FilePath[100] = {0,};
	uint32_t cnt = 0, offset = 0;
	
	LISTBOX_GetItemText(hList, idx, temp_str, 50);
	memcpy(FilePath, dir_path.path, dir_path.size);
	if(dir_path.path[dir_path.size-1] != '\\')
	{
		FilePath[dir_path.size] = '\\';
		offset = 1;
	}
	
	for(cnt=0; *((TCHAR *)temp_str+cnt) != 0; cnt++)
	{
		FilePath[dir_path.size+offset+cnt] = *(temp_str+cnt);
	}
	FilePath[dir_path.size+offset+cnt] = 0;
	
	return FilePath;
}

TCHAR * GetCurrentFilePath(void)
{
	return dir_path.path;
}

uint32_t CheckExtension(TCHAR * FileName, const TCHAR * Ext)
{
	uint32_t cnt = 0;
	uint32_t total_len = strlen(FileName);
	uint8_t idx=0;
	
	for(cnt=0; *(FileName+cnt) != '.'; cnt++)
	{
		if(cnt > total_len-1)
		{
			return 0;
		}
	}
	cnt++;
	for(idx=0; idx<3; idx++)
	{
		if( *(FileName+cnt+idx) != *(Ext+idx) )
		{
			return 0;
		}
	}
	
	return 1;
}
uint32_t CheckSelectedFileExtension(LISTBOX_Handle hList, int idx, const TCHAR ** Ext)
{
	LISTBOX_GetItemText(hList, idx, temp_str, 50);
	
	while(Ext != 0)
	{
		if(CheckExtension(temp_str, *Ext))
			return 1;
		Ext++;
	}
	
	return 0;
}

void GetCurrentDir(void)
{
	FILINFO	finfo;
	GUI_HMEM hTemp = 0;
	file_element_t * ptrTemp;
	
	Clear_File();
    osMutexWait(FatKey, osWaitForever);
	if(f_opendir(&curr_dir, dir_path.path) == FR_OK)
	{
		while (f_readdir(&curr_dir, &finfo) == FR_OK)  
		{
			if(!finfo.fname[0]) break;   
			if(finfo.fattrib & AM_HID)	continue;
			if(finfo.fattrib & AM_SYS)	continue;
			
			if(finfo.fattrib & AM_DIR)
			{
				hTemp = GUI_ALLOC_AllocZero(sizeof(file_element_t));
				if(hTemp)
				{
					ptrTemp = (file_element_t *)GUI_ALLOC_h2p(hTemp);
					Add_File(ptrTemp, hTemp, finfo.fname, TYPE_DIRECTORY); 
				}
			}
			if (finfo.fattrib & AM_ARC) 
			{ 
				hTemp = GUI_ALLOC_AllocZero(sizeof(file_element_t));
				if(hTemp)
				{
					ptrTemp = GUI_ALLOC_h2p(hTemp);
					Add_File(ptrTemp, hTemp, finfo.fname, TYPE_FILE); 
				}
			}
		}
	}
	f_closedir(&curr_dir);
	Arrange_File();
	osMutexRelease(FatKey);
}




void UpdateList(LISTBOX_Handle hList)
{
	int itemnum = LISTBOX_GetNumItems(hList);
	file_element_t * start = curr_files;
	
	LISTBOX_SetOwnerDraw(hList, _OwnerDraw);
	while(itemnum)
	{
		LISTBOX_DeleteItem(hList, 0);
		itemnum--;
	}
	
	if(strcmp(dir_path.path, "0:\\"))
	{
		LISTBOX_AddString(hList, "..");
	}
	while(start)
	{
		LISTBOX_AddString(hList, start->ptrMem);
		start = start->next;
	}
}
FRESULT User_f_read (FIL* fp, void* buff, UINT btr, UINT* br)
{
	FRESULT res;
    osMutexWait(FatKey, osWaitForever);
	res = f_read(fp, buff, btr, br);
	osMutexRelease(FatKey);
	return res;
}
FRESULT User_f_open (FIL* fp, const TCHAR* path, BYTE mode)
{
	FRESULT res;
    osMutexWait(FatKey, osWaitForever);
	res = f_open(fp, path, mode);
	osMutexRelease(FatKey);
	return res;
}
FRESULT User_f_lseek (FIL* fp, FSIZE_t ofs)
{
	FRESULT res;
    osMutexWait(FatKey, osWaitForever);
	res = f_lseek(fp, ofs);
	osMutexRelease(FatKey);
	return res;
}
FRESULT User_f_close (FIL* fp)
{
	FRESULT res;
    osMutexWait(FatKey, osWaitForever);
	res = f_close(fp);
	osMutexRelease(FatKey);
	return res;
}
#if 0
void OutPutFile(void)
{ 
	unsigned int a;
	FRESULT res;
	FILINFO finfo;
	DIR dirs;
	char path[16]={""};  

	if (f_opendir(&dirs, path) == FR_OK) 
	{
		a=0;
		while (f_readdir(&dirs, &finfo) == FR_OK)  
		{
			if(!finfo.fname[0]) break;   
			if(finfo.fattrib == AM_DIR)
			{
				a++;
				GUI_Text(50,a*20,"Dir",White,Red);
				GUI_Text(100, a*20, (uint8_t *)finfo.fname, White,Red);
			}
			if (finfo.fattrib == AM_ARC) 
			{ 
				a++;
				GUI_Text(50,a*20,"FILE",White,Red);
				GUI_Text(100, a*20, (uint8_t *)finfo.fname, White,Red);
/*				
				GUI_Text(200,100,"Open",White,Red);
				res = f_open(&fsrc, finfo.fname, FA_OPEN_EXISTING | FA_READ);
				br=1;
				a=0;
				{					
					for(a=0; a<512; a++) 
						buffer[a]=0; 
					res = f_read(&fsrc, buffer, sizeof(buffer), &br);
					if (res || br == 0) break;   // error or eof
				}
				f_close(&fsrc);            
*/	
			}
		} 
	}
	if (f_opendir(&dirs, "dir1") == FR_OK) 
	{
		a++;
		GUI_Text(50,a*20,"Open [ 0:\\dir1 ]",White,Blue);
		while (f_readdir(&dirs, &finfo) == FR_OK)  
		{
			if(!finfo.fname[0]) break;   
			if(finfo.fattrib == AM_DIR)
			{
				a++;
				GUI_Text(50,a*20,"Dir",White,Red);
				GUI_Text(100, a*20, (uint8_t *)finfo.fname, White,Red);
			}
			if (finfo.fattrib == AM_ARC) 
			{ 
				a++;
				GUI_Text(50,a*20,"File",White,Red);
				GUI_Text(100, a*20, (uint8_t *)finfo.fname, White,Red);
/*				
				res = f_open(&fsrc, finfo.fname, FA_OPEN_EXISTING | FA_READ);
				br=1;
				a=0;
				{					
					for(a=0; a<512; a++) 
						buffer[a]=0; 
					res = f_read(&fsrc, buffer, sizeof(buffer), &br);
					if (res || br == 0) break;   // error or eof
				}
				f_close(&fsrc);            
*/				
			}
		} 
	}
	if (f_opendir(&dirs, "dir1\\subdir1") == FR_OK) 
	{
		a++;
		GUI_Text(50,a*20,"Open [ 0:\\subdir1 ]",White,Blue);
		while (f_readdir(&dirs, &finfo) == FR_OK)  
		{
			if(!finfo.fname[0]) break;   
			if(finfo.fattrib == AM_DIR)
			{
				a++;
				GUI_Text(50,a*20,"Dir",White,Red);
				GUI_Text(100, a*20, (uint8_t *)finfo.fname, White,Red);
			}
			if (finfo.fattrib == AM_ARC) 
			{ 
				a++;
				GUI_Text(50,a*20,"FILE",White,Red);
				GUI_Text(100, a*20, (uint8_t *)finfo.fname, White,Red);
/*
				res = f_open(&fsrc, finfo.fname, FA_OPEN_EXISTING | FA_READ);
				br=1;
				a=0;
				{					
					for(a=0; a<512; a++) 
						buffer[a]=0; 
					res = f_read(&fsrc, buffer, sizeof(buffer), &br);
					if (res || br == 0) break;   // error or eof
				}
				f_close(&fsrc);            
*/
			}
		} 
	}
  	res = f_unlink("0:/test.txt");
	res = f_open(&fsrc,"0:/test.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if(res==FR_OK)
	{
		res = f_write(&fsrc,textFileBuffer,sizeof(textFileBuffer),(void *)&bw);
	}	
  	f_close(&fsrc);

	res = f_open(&fsrc,"0:/test.txt", FA_OPEN_EXISTING | FA_READ);
	if(res==FR_OK)
	{
		res = f_read(&fsrc, buffer, sizeof(buffer), &br);
	}
  	f_close(&fsrc);

	while(1);
}

void DrawUpArrow(uint16_t x, uint16_t y)
{
	LCD_DrawLine(x-ARROW_HAlF_WIDTH, y+ARROW_HALF_HEIGHT, x+ARROW_HAlF_WIDTH, y+ARROW_HALF_HEIGHT, Red);
	LCD_DrawLine(x-ARROW_HAlF_WIDTH, y+ARROW_HALF_HEIGHT, x, y-ARROW_HALF_HEIGHT, Red);
	LCD_DrawLine(x+ARROW_HAlF_WIDTH, y+ARROW_HALF_HEIGHT, x, y-ARROW_HALF_HEIGHT, Red);
}

void DrawDownArrow(uint16_t x, uint16_t y)
{
	LCD_DrawLine(x-ARROW_HAlF_WIDTH, y-ARROW_HALF_HEIGHT, x+ARROW_HAlF_WIDTH, y-ARROW_HALF_HEIGHT, Red);
	LCD_DrawLine(x-ARROW_HAlF_WIDTH, y-ARROW_HALF_HEIGHT, x, y+ARROW_HALF_HEIGHT, Red);
	LCD_DrawLine(x+ARROW_HAlF_WIDTH, y-ARROW_HALF_HEIGHT, x, y+ARROW_HALF_HEIGHT, Red);
}

int PosToCtrl(uint16_t x_pos, uint16_t y_pos)
{
	uint32_t cnt;
	if((y_pos <= VOLUME_UP_POS_Y+ARROW_HEIGHT) && (y_pos >= VOLUME_UP_POS_Y))
	{
		if((x_pos >= VOLUME_UP_POS_X) && (x_pos <= VOLUME_UP_POS_X+ARROW_WIDTH))
		{
			return 1;
		}
		if((x_pos >= VOLUME_DOWN_POS_X) && (x_pos <= VOLUME_DOWN_POS_X+ARROW_WIDTH))
		{
			return 2;
		}
		if((x_pos >= PLAY_STOP_POS_X) && (x_pos <= PLAY_STOP_POS_X+40))
		{
			return 3;
		}
	}
	
	return -1;
}
#endif
