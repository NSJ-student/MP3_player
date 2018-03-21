#ifndef __FILE_HANDLE_H__
#define __FILE_HANDLE_H__


#define TYPE_DIRECTORY	1
#define TYPE_FILE	2
#define TYPE_UPPER	3
#define TYPE_NONE	4

typedef struct __filepath {
//	struct __filepath * prev;
	int type;
	GUI_HMEM hThis;
	GUI_HMEM hMem;
	char * ptrMem;
	struct __filepath * prev;
	struct __filepath * next;
} file_element_t;


int FileSystem_Init(void);
FRESULT ToUpperDir(void);
FRESULT ToDownDir(TCHAR * dir_name);
int ToSelecteDir(LISTBOX_Handle hList, int idx);
int GetMatchedType(TCHAR * filename);
void GetCurrentDir(void);
void UpdateList(LISTBOX_Handle hList);
uint32_t CheckExtension(TCHAR * FileName, const TCHAR * Ext);
uint32_t CheckSelectedFileExtension(LISTBOX_Handle hList, int idx, const TCHAR ** Ext);

FRESULT User_f_read (FIL* fp, void* buff, UINT btr, UINT* br);
FRESULT User_f_open (FIL* fp, const TCHAR* path, BYTE mode);
FRESULT User_f_lseek (FIL* fp, FSIZE_t ofs);
FRESULT User_f_close (FIL* fp);

FILINFO * PosToFInfo(uint16_t x_pos, uint16_t y_pos);
TCHAR * GetFilePath(TCHAR * name);
TCHAR * GetSelectedFilePath(LISTBOX_Handle hList, int idx);
TCHAR * GetCurrentFilePath(void);

//void OutPutFile(void);
/*
void DrawUpArrow(uint16_t x, uint16_t y);
void DrawDownArrow(uint16_t x, uint16_t y);
int PosToCtrl(uint16_t x_pos, uint16_t y_pos);
*/

#endif   /* __FILE_HANDLE_H__ */
