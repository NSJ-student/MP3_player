#ifndef __USER_GUI_H__
#define __USER_GUI_H__

// Listbox Macro
#define FILE_LIST_X		0
#define FILE_LIST_Y		20
#define FILE_LIST_W		150
#define FILE_LIST_H		100

// Edit Macro
#define EDIT_FILE_ID	0x01
#define EDIT_FILE_X		0
#define EDIT_FILE_Y		0
#define EDIT_FILE_W		150
#define EDIT_FILE_H		20

#define EDIT_STATE_ID	0x02
#define EDIT_STATE_X	150
#define EDIT_STATE_Y	0
#define EDIT_STATE_W	100
#define EDIT_STATE_H	20

#define EDIT_LEN_ID		0x03
#define EDIT_LEN_X		0
#define EDIT_LEN_Y		120
#define EDIT_LEN_W		150
#define EDIT_LEL_H		20

// Slider Macro
#define SLD_VOLUME_ID	0x10
#define SLD_VOLUME_X	150
#define SLD_VOLUME_Y	20
#define SLD_VOLUME_W	100
#define SLD_VOLUME_H	20

#define SLD_SEEK_ID		0x11
#define SLD_SEEK_X		25
#define SLD_SEEK_Y		160
#define SLD_SEEK_W		270
#define SLD_SEEK_H		25

// Buton Macro
#define IDX_DIR_BACK	0
#define IDX_FILE_SELECT	1
#define IDX_PLAY_STOP	2
#define IDX_VOL_UP		3
#define IDX_VOL_DOWN	4
#define IDX_PAUSE		5

#define BTN_BACK_ID		0x20
#define BTN_BACK_X		150
#define BTN_BACK_Y		60
#define BTN_BACK_W		50
#define BTN_BACK_H		20

#define BTN_SELECT_ID	0x21
#define BTN_SELECT_X	200
#define BTN_SELECT_Y	60
#define BTN_SELECT_W	50
#define BTN_SELECT_H	20

#define BTN_STOP_ID		0x22
#define BTN_STOP_X		150
#define BTN_STOP_Y		80
#define BTN_STOP_W		50
#define BTN_STOP_H		20

#define BTN_VOL_UP_ID		0x23
#define BTN_VOL_UP_X		150
#define BTN_VOL_UP_Y		40
#define BTN_VOL_UP_W		50
#define BTN_VOL_UP_H		20

#define BTN_VOL_DN_ID		0x24
#define BTN_VOL_DN_X		200
#define BTN_VOL_DN_Y		40
#define BTN_VOL_DN_W		50
#define BTN_VOL_DN_H		20

#define BTN_PAUSE_ID		0x25
#define BTN_PAUSE_X			200
#define BTN_PAUSE_Y			80
#define BTN_PAUSE_W			50
#define BTN_PAUSE_H			20

#define GUI_BTN_PAUSE_ON	1
#define GUI_BTN_PAUSE_OFF	0

int _OwnerDraw(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

void CreateUserGUI(void);
LISTBOX_Handle GUI_GetListBoxHandle(void);
void GUI_SetCurrentState(char * state);
void GUI_SetDirPath(char * path);
void GUI_SetFileLen(char * filelen);
void GUI_SetVolume(uint8_t vol);
uint8_t GUI_GetVolume(void);
void GUI_SetPause(int pause_on);

#endif   /* __USER_GUI_H__ */
