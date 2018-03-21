
#ifndef __UCGUI_USER_GRAPH_H__
#define __UCGUI_USER_GRAPH_H__

#include "GUIDEMO.h"

typedef struct _graph{
	int x_min;
	int x_max;
	int y_min;
	int y_max;
} graph_axis_t;

typedef struct {
  signed short * aY;
} PARAM;

typedef struct _param {
	graph_axis_t * axis;
	PARAM p; 
	PARAM p2;
	int p_num;
} draw_param_t;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#if GUIDEMO_LARGE
  #define YSIZE   (LCD_YSIZE - 100)
#else
  #define YSIZE   (LCD_YSIZE - 30)
#endif
/*
#if LCD_YSIZE > 120
  #define YSIZE   (LCD_YSIZE - 100)
#else
  #define YSIZE   20
#endif
*/

#define DEG2RAD (3.1415926f / 180)

#if LCD_BITSPERPIXEL == 1
  #define COLOR_GRAPH0 GUI_WHITE
  #define COLOR_GRAPH1 GUI_WHITE
#else
  #define COLOR_GRAPH0 GUI_GREEN
  #define COLOR_GRAPH1 GUI_YELLOW
#endif

#define GRAPH_SIZE	(LCD_XSIZE - 20)
#define GRAPH_STEP	(sizeof(I16))
	
#define GRAPH_X_LEFT	18
#define GRAPH_X_RIGHT	(LCD_XSIZE - 1)
#define GRAPH_X_SIZE	(LCD_XSIZE - 20)
#define GRAPH_Y_TOP		(LCD_YSIZE - 21 - YSIZE)
#define GRAPH_Y_BOTTOM	(LCD_YSIZE - 20)

#define GRAPH_X_MIN		0
#define GRAPH_X_MAX		6
#define GRAPH_X_STEP	(GRAPH_X_MAX - GRAPH_X_MIN)

#define GRAPH_Y_MIN		0
#define GRAPH_Y_MAX		5
#define GRAPH_Y_STEP	(GRAPH_Y_MAX - GRAPH_Y_MIN)

#define GRAPH_BASE_STEP	7

#define AdcToVolt(adc)  	(adc*YSIZE)
#define VoltToGraph(volt)	((volt)*YSIZE/GRAPH_Y_STEP)


#define GRAPH_MEASURE_TOP		(LCD_YSIZE-10)
#define GRAPH_MEASURE_WIDTH		80

#define GRAPH_NUMBER_ENABLE		0
#define DEBUG_ENABLE			0
#define USE_SENSOR_TIMER		0

#define BUTTON_SPEED_ID			1
#define BUTTON_MEASURE_ID		2
#define BUTTON_CHANNEL_ID		3

#define CHECKBOX_PERIOD_ID		4
#define CHECKBOX_FREQUENCY_ID	5
#define CHECKBOX_MEAN_ID		6
#define CHECKBOX_CHANNEL_ID		7
#define RADIO_CH1_ID			8
#define RADIO_CH2_ID			9

#define CHANNEL_CH1_CHECK_ID	10
#define CHANNEL_CH2_CHECKC_ID	11
#define CHANNEL_UP_BTN_ID		12
#define CHANNEL_DN_BTN_ID		13

#define ADC_CHANNEL_NUM			2

#define MENU_NUM				3
#define CHECKBOX_NUMBER			4
#define CHANNEL_BTN_NUM			4


#define MENU_BUTTON_LEFT	150
#define MENU_BUTTON_TOP		0
#define MENU_BUTTON_W		50
#define MENU_BUTTON_H		20

#define CHILD_LEFT			200
#define CHILD_TOP			0
#define CHIlD_W				120
#define CHILD_H				60

#define SPD_LISTBOX_LEFT	(200)
#define SPD_LISTBOX_TOP		(0)
#define SPD_LISTBOX_W		(120)
#define SPD_LISTBOX_H		(60)

#define CH_RADIO_LEFT		(200)
#define CH_RADIO_TOP		(0)
#define CH_RADIO_W			40
#define CH_RADIO_H			40

#define MEASURE_CHECK_LEFT	(200+50)
#define MEASURE_CHECK_TOP	(0)
#define MEASURE_CHECK_W		80
#define MEASURE_CHECK_H		15

#define CHANNEL_GAP			5

#define CHANNEL_CHECK_LEFT	(205)
#define CHANNEL_CHECK_TOP	(0)
#define CHANNEL_CHECK_W		40
#define CHANNEL_CHECK_H		20

#define CHANNEL_BTN_LEFT	(255)
#define CHANNEL_BTN_TOP		(0)
#define CHANNEL_BTN_W		40
#define CHANNEL_BTN_H		20

void Graph_Init(char * title, graph_axis_t * axis);
int Draw_Graph(void * pPARAM);
void Draw_CH_Base(int ch, int ch1_base, int ch2_base);


#endif
