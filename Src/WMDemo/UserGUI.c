#include "includes.h"


EDIT_Handle hDirPath;
EDIT_Handle hState;
EDIT_Handle hFileLen;
LISTBOX_Handle hListBox;
SLIDER_Handle hVolume;
SLIDER_Handle hSeek;
BUTTON_Handle hButton[6];
uint8_t vol_text[4];

void CreateUserGUI(void)
{
	// create dir edit control
	hDirPath = EDIT_Create(EDIT_FILE_X,EDIT_FILE_Y,
						EDIT_FILE_W, EDIT_FILE_H,
						EDIT_FILE_ID, 255, WM_CF_SHOW|EDIT_CF_VCENTER|EDIT_CF_LEFT);
	EDIT_SetFont(hDirPath, &GUI_Font13_ASCII);
	// create state edit control
	hState = EDIT_Create(EDIT_STATE_X,EDIT_STATE_Y,
						EDIT_STATE_W, EDIT_STATE_H,
						EDIT_STATE_ID, 255, 
						WM_CF_SHOW|EDIT_CF_VCENTER|EDIT_CF_LEFT);
	EDIT_SetFont(hState, &GUI_Font13_ASCII);
	// create file len edit control
	hFileLen = EDIT_Create(EDIT_LEN_X,EDIT_LEN_Y,
						EDIT_LEN_W, EDIT_LEL_H,
						EDIT_LEN_ID, 255, 
						WM_CF_SHOW|EDIT_CF_VCENTER|EDIT_CF_LEFT);
	EDIT_SetFont(hFileLen, &GUI_Font13_ASCII);
	// create slider control
	hVolume = SLIDER_Create(SLD_VOLUME_X, SLD_VOLUME_Y,
							SLD_VOLUME_W, SLD_VOLUME_H,
							WM_HBKWIN, SLD_VOLUME_ID, 
							WM_CF_SHOW, 0);
	SLIDER_SetRange(hVolume, 0, 255);
	SLIDER_SetNumTicks(hVolume, 10);
	SLIDER_SetBkColor(hVolume, GUI_LIGHTGRAY);
	
	hSeek = SLIDER_Create(SLD_SEEK_X, SLD_SEEK_Y,
							SLD_SEEK_W, SLD_SEEK_H,
							WM_HBKWIN, SLD_SEEK_ID, 
							WM_CF_SHOW, 0);
	SLIDER_SetRange(hSeek, 0, 255);
//	SLIDER_SetNumTicks(hSeek, 10);
	SLIDER_SetBkColor(hSeek, GUI_LIGHTGRAY);
	
	// create button
	hButton[IDX_DIR_BACK] = BUTTON_Create(BTN_BACK_X, BTN_BACK_Y, 
								BTN_BACK_W, BTN_BACK_H, 
								BTN_BACK_ID,BUTTON_CF_SHOW );
	BUTTON_SetText (hButton[IDX_DIR_BACK], "Back");
	
	hButton[IDX_FILE_SELECT] = BUTTON_Create(BTN_SELECT_X, BTN_SELECT_Y, 
								BTN_SELECT_W, BTN_SELECT_H, 
								BTN_SELECT_ID,BUTTON_CF_SHOW );
	BUTTON_SetText (hButton[IDX_FILE_SELECT], "Select");
	
	hButton[IDX_PLAY_STOP] = BUTTON_Create(BTN_STOP_X, BTN_STOP_Y, 
								BTN_STOP_W, BTN_STOP_H, 
								BTN_STOP_ID,BUTTON_CF_SHOW );
	BUTTON_SetText (hButton[IDX_PLAY_STOP], "Stop");
	
	hButton[IDX_VOL_UP] = BUTTON_Create(BTN_VOL_UP_X, BTN_VOL_UP_Y, 
								BTN_VOL_UP_W, BTN_VOL_UP_H, 
								BTN_VOL_UP_ID,BUTTON_CF_SHOW );
	BUTTON_SetText (hButton[IDX_VOL_UP], "Vol Up");
	
	hButton[IDX_VOL_DOWN] = BUTTON_Create(BTN_VOL_DN_X, BTN_VOL_DN_Y, 
								BTN_VOL_DN_W, BTN_VOL_DN_H, 
								BTN_VOL_DN_ID,BUTTON_CF_SHOW );
	BUTTON_SetText (hButton[IDX_VOL_DOWN], "Vol Dn");
	
	hButton[IDX_PAUSE] = BUTTON_Create(BTN_PAUSE_X, BTN_PAUSE_Y, 
								BTN_PAUSE_W, BTN_PAUSE_H, 
								BTN_PAUSE_ID,BUTTON_CF_SHOW );
	BUTTON_SetText (hButton[IDX_PAUSE], "Pause");
	
	// create listbox
   	hListBox = LISTBOX_Create(0, 
							FILE_LIST_X, FILE_LIST_Y, 
							FILE_LIST_W, FILE_LIST_H, WM_CF_SHOW);
	LISTBOX_SetFont(hListBox, &GUI_Font13_ASCII);
	SCROLLBAR_CreateAttached(hListBox, SCROLLBAR_CF_VERTICAL);
}

LISTBOX_Handle GUI_GetListBoxHandle(void)
{
	return hListBox;
}

void GUI_SetDirPath(char * path)
{
	EDIT_SetText(hDirPath, path);
}
void GUI_SetFileLen(char * filelen)
{
	EDIT_SetText(hFileLen, filelen);
}
void GUI_SetCurrentState(char * state)
{
	EDIT_SetText(hState, state);
}

void GUI_SetPause(int pause_on)
{
	if(pause_on == GUI_BTN_PAUSE_ON)
	{
		BUTTON_SetBkColor(hButton[IDX_PAUSE], 0, GUI_LIGHTRED);
		BUTTON_SetText (hButton[IDX_PAUSE], "Resume");
	}
	else
	{
		BUTTON_SetBkColor(hButton[IDX_PAUSE], 0, GUI_LIGHTGRAY);
		BUTTON_SetText (hButton[IDX_PAUSE], "Pause");
	}
}
void GUI_SetVolume(uint8_t vol)
{
	SLIDER_SetValue(hVolume, 255-vol);
}

uint8_t GUI_GetVolume(void)
{
	int vol =  SLIDER_GetValue(hVolume);
	
	return 255 - vol;
}


/*********************************************************************
*
*       Bitmap data for user drawn list box
*/
const GUI_COLOR ColorsSmilie0[] = {
     0xFFFFFF,0x000000,0xFF00FF
};

const GUI_COLOR ColorsSmilie1[] = {
     0xFFFFFF,0x000000,0x00FFFF
};

const GUI_LOGPALETTE PalSmilie0 = {
  3,	// number of entries
  1, 	// Has transparency
  &ColorsSmilie0[0]
};

const GUI_LOGPALETTE PalSmilie1 = {
  3,	// number of entries
  1, 	// Has transparency
  &ColorsSmilie1[0]
};

const unsigned char acSmilie0[] = {		// File
	/*
  0x00, 0x55, 0x40, 0x00,
  0x01, 0xAA, 0x90, 0x00,
  0x06, 0xAA, 0xA4, 0x00,
  0x19, 0x6A, 0x59, 0x00,
  0x69, 0x6A, 0x5A, 0x40,
  0x6A, 0xA6, 0xAA, 0x40,
  0x6A, 0xA6, 0xAA, 0x40,
  0x6A, 0xA6, 0xAA, 0x40,
  0x6A, 0xAA, 0xAA, 0x40,
  0x1A, 0x95, 0xA9, 0x00,
  0x06, 0x6A, 0x64, 0x00,
  0x01, 0xAA, 0x90, 0x00,
  0x00, 0x55, 0x40, 0x00
	*/
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0A, 0x00, 0x00, 
	0x00, 0x0A, 0x80, 0x00, 
	0x00, 0x08, 0x80, 0x00, 
	0x00, 0x08, 0x80, 0x00, 
	0x00, 0x08, 0xA0, 0x00, 
	0x00, 0x08, 0x20, 0x00, 
	0x00, 0x28, 0x28, 0x00, 
	0x02, 0xA8, 0x00, 0x00, 
	0x0A, 0xA8, 0x00, 0x00, 
	0x0A, 0xA8, 0x00, 0x00, 
	0x02, 0xA0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
};

const unsigned char acSmilie1[] = {		// Directory
	/*
  0x00, 0x55, 0x40, 0x00,
  0x01, 0xAA, 0x90, 0x00,
  0x06, 0xAA, 0xA4, 0x00,
  0x19, 0x6A, 0x59, 0x00,
  0x69, 0x6A, 0x5A, 0x40,
  0x6A, 0xA6, 0xAA, 0x40,
  0x6A, 0xA6, 0xAA, 0x40,
  0x6A, 0xA6, 0xAA, 0x40,
  0x6A, 0xAA, 0xAA, 0x40,
  0x1A, 0x6A, 0x69, 0x00,
  0x06, 0x95, 0xA4, 0x00,
  0x01, 0xAA, 0x90, 0x00,
  0x00, 0x55, 0x40, 0x00
	*/
	0x00, 0x00, 0x00, 0x00, 
	0x15, 0x50, 0x00, 0x00, 
	0x1A, 0x90, 0x00, 0x00, 
	0x1A, 0x95, 0x55, 0x00, 
	0x1A, 0xAA, 0xA9, 0x00, 
	0x1A, 0xAA, 0xA9, 0x00, 
	0x19, 0x55, 0x59, 0x00, 
	0x1A, 0xAA, 0xA9, 0x00, 
	0x1A, 0xAA, 0xA9, 0x00, 
	0x1A, 0xAA, 0xA9, 0x00, 
	0x1A, 0xAA, 0xA9, 0x00, 
	0x15, 0x55, 0x55, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
};

const GUI_BITMAP bmSmilie0 = {
 13,          // XSize
 13,          // YSize
 4,           // BytesPerLine
 2,           // BitsPerPixel
 acSmilie0,   // Pointer to picture data (indices)
 &PalSmilie0  // Pointer to palette
};

const GUI_BITMAP bmSmilie1 = {
 13,          // XSize
 13,          // YSize
 4,           // BytesPerLine
 2,           // BitsPerPixel
 acSmilie1,   // Pointer to picture data (indices)
 &PalSmilie1  // Pointer to palette
};


/*********************************************************************
*
*       _GetItemSizeX
*/
static int _GetItemSizeX(WM_HWIN hWin, int ItemIndex) {
  char acBuffer[100];
  int  DistX;

  LISTBOX_GetItemText(hWin, ItemIndex, acBuffer, sizeof(acBuffer));
  DistX = GUI_GetStringDistX(acBuffer);
  return DistX + bmSmilie0.XSize + 16;
}

/*********************************************************************
*
*       _GetItemSizeY
*/
static int _GetItemSizeY(WM_HWIN hWin, int ItemIndex) {
  int DistY;

  DistY = GUI_GetFontDistY() + 1;
  if (LISTBOX_GetMulti(hWin)) {
    if (LISTBOX_GetItemSel(hWin, ItemIndex)) {
      //DistY += 8;
    }
  } else if (LISTBOX_GetSel(hWin) == ItemIndex) {
    //DistY += 8;
  }
  return DistY;
}
/*********************************************************************
*
*       _OwnerDraw
*
* Function description
*   This is the owner draw function.
*   It allows complete customization of how the items in the listbox are
*   drawn. A command specifies what the function should do;
*   The minimum is to react to the draw command (WIDGET_ITEM_DRAW);
*   If the item x-size differs from the default, then this information
*   needs to be returned in reaction to WIDGET_ITEM_GET_XSIZE.
*   To insure compatibility with future version, all unhandled commands
*   must call the default routine LISTBOX_OwnerDraw.
*/
int _OwnerDraw(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
  WM_HWIN hWin;
  int     Index;

  hWin     = pDrawItemInfo->hWin;
  Index    = pDrawItemInfo->ItemIndex;
  switch (pDrawItemInfo->Cmd) {
  case WIDGET_ITEM_GET_XSIZE:
    return _GetItemSizeX(hWin, Index);
  case WIDGET_ITEM_GET_YSIZE:
    return _GetItemSizeY(hWin, Index);
  case WIDGET_ITEM_DRAW:
    {
      int   MultiSel;
      int   Sel;
      int   YSize;
      int   FontDistY;
      int   IsDisabled;
      int   IsSelected;
      int   ColorIndex;
      char  acBuffer[100];
      const GUI_BITMAP * pBm;
      const GUI_FONT * pOldFont = 0;

      GUI_COLOR aColor[4] = {GUI_BLACK, GUI_WHITE, GUI_WHITE, GUI_GRAY};
      GUI_COLOR aBkColor[4] = {GUI_WHITE, GUI_GRAY, GUI_BLUE, 0xC0C0C0};
      IsDisabled = LISTBOX_GetItemDisabled(pDrawItemInfo->hWin, pDrawItemInfo->ItemIndex);
      IsSelected = LISTBOX_GetItemSel(hWin, Index);
      MultiSel   = LISTBOX_GetMulti(hWin);
      Sel        = LISTBOX_GetSel(hWin);
      YSize      = _GetItemSizeY(hWin, Index);
      //
      // Calculate color index
      //
      if (MultiSel) {
        if (IsDisabled) {
          ColorIndex = 3;
        } else {
          ColorIndex = (IsSelected) ? 2 : 0;
        }
      } else {
        if (IsDisabled) {
          ColorIndex = 3;
        } else {
          if (pDrawItemInfo->ItemIndex == Sel) {
            ColorIndex = WM_HasFocus(pDrawItemInfo->hWin) ? 2 : 1;
          } else {
            ColorIndex = 0;
          }
        }
      }
      //
      // Draw item
      //
      GUI_SetBkColor(aBkColor[ColorIndex]);
      GUI_SetColor  (aColor[ColorIndex]);
      LISTBOX_GetItemText(pDrawItemInfo->hWin, pDrawItemInfo->ItemIndex, acBuffer, sizeof(acBuffer));
      GUI_Clear();
      if ((ColorIndex == 1) || (ColorIndex == 2)) {
        pOldFont = GUI_SetFont(&GUI_Font13_ASCII);
      }
      FontDistY  = GUI_GetFontDistY();
      GUI_DispStringAt(acBuffer, pDrawItemInfo->x0 + bmSmilie0.XSize + 16, pDrawItemInfo->y0 + (YSize - FontDistY) / 2);
      if (pOldFont) {
        GUI_SetFont(pOldFont);
      }
      GUI_DispCEOL();
      //
      // Draw bitmap
      //
	  if(GetMatchedType(acBuffer) == TYPE_FILE)
		  pBm = &bmSmilie0;
	  else
		  pBm = &bmSmilie1;
      //pBm = MultiSel ? IsSelected ? &bmSmilie1 : &bmSmilie0 : (pDrawItemInfo->ItemIndex == Sel) ? &bmSmilie1 : &bmSmilie0;
      GUI_DrawBitmap(pBm, pDrawItemInfo->x0 + 7, pDrawItemInfo->y0 + (YSize - pBm->YSize) / 2);
      //
      // Draw focus rectangle
      //
      if (MultiSel && (pDrawItemInfo->ItemIndex == Sel)) {
        GUI_RECT rFocus;
        GUI_RECT rInside;

        WM_GetInsideRectEx(pDrawItemInfo->hWin, &rInside);
        rFocus.x0 = pDrawItemInfo->x0;
        rFocus.y0 = pDrawItemInfo->y0;
        rFocus.x1 = rInside.x1;
        rFocus.y1 = pDrawItemInfo->y0 + YSize - 1;
        GUI_SetColor(GUI_WHITE - aBkColor[ColorIndex]);
        GUI_DrawFocusRect(&rFocus, 0);
      }
    }
    break;
  default:
    return LISTBOX_OwnerDraw(pDrawItemInfo);
  }
  return 0;
}
