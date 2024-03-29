/*
用于ui的一些函数、变量、结构体
*/
#ifndef _LCDUI_H_
#define _LCDUI_H_

/*------------------------------------------------include----------------------------------------------*/
#include "common.h"
#include "include.h"

/*------------------------------------------------define------------------------------------------------*/
#define IMG_MODE 1
#define UI_MODE 2
#define ITEM_NUM 3
#define PICTURE_MODE 4
#define RUN_MODE 5

#define MAX_OPTION 30                     //lcd能使用的最大的参数个数
#define SECTOR_NUM (FLASH_SECTOR_NUM - 1) //最后一个扇区，扇区编号从0开始，最后一个编号是SECTOR_NUM

/*---------------------------------------------状态模式的一些结构体---------------------------------------*/
typedef struct Screen_Data //传参结构体,ui显示数据的结构体
{
    char* data_name; //参数名称
    //int *data_value; //参数值
    union datauseofunion
    {
        int*   i;
        long*  l;
        float* f;
        uint8* c;
    } data_value;
    float icrement; //参数累加数 如果是99就是on\off
    int ip;       //是否存放入flash的标志位,为负数时表示该参数不写入flash，为正数时表示存储扇区的偏移地址
} Screen_Data;

//输入的是当前的状态，返回的是操作后的状态
typedef struct Lcd_State //状态结构体
{
    struct Lcd_State* (*press_M)(struct Lcd_State* pThis); //函数指针，press_M指向一个返回值是Lcd_State指针类型，参数是Lcd_State指针类型的函数
    struct Lcd_State* (*press_U)(struct Lcd_State* pThis); //指向上建按下时的处理函数
    struct Lcd_State* (*press_D)(struct Lcd_State* pThis);
    struct Lcd_State* (*press_L)(struct Lcd_State* pThis);
    struct Lcd_State* (*press_R)(struct Lcd_State* pThis);
} Lcd_State;

/*------------------------------------------------全局变量---------------------------------------------*/
extern Lcd_State  wait_middle, normal_page, imgbuff_show, read_picture, show_dealed_picture, set_value, various_adjust; //lcd等待按键时的4种状态
extern Lcd_State* p_current_state;                                                                                      //指向当前状态的指针

extern int          page;        //ui当前所在页
extern int          current_row; //ui当前所在行
extern Screen_Data* screen_data; //ui显示的参数
extern Screen_Data* dvarious;    //调试用的参数,ip变为是否已经选中

//extern float motor_go; //在显示状态下控制电机是否转动的变量
extern uint8  key_on;
extern int    colour[]; //存放各个参数背景色的数组
extern Site_t tem_site_str[];
extern Site_t tem_site_data[];

//control state open or close
typedef enum bitControl
{
    mainShowSW = 0,
    motorSW    = 1,
    steerSW    = 2,
    lcdSW      = 3,

    cameraSW = 4,
    adjustSW = 5,
    updateSW = 6,
}bitControl;

typedef enum modeControl
{
    iniMOD   = 0x21, // 0010 0001
    runMOD   = 0x16, // 0001 0110
    debugMOD = 0x39, // 0011 1001
    pageMOD  = 0x41, // 0100 0001
}modeControl;
/*--------------------------------------------------函数--------------------------------------------------*/

/*普通函数*/
void        UI_INIT();
void        updateUI();
void        updateadjustUI();
extern void flash_In();
extern void flash_Out();

void DiyDataPrintf(Site_t, Screen_Data* , int16 , int16 );

//inspect
extern uint8 getSwitch(enum bitControl);
extern void  setMode(enum modeControl);
extern void  setSwitch(enum bitControl);
extern void  clearSwitch(enum bitControl);
extern void  inverseSwitch(enum bitControl);

/*--------------------------------------------------变量--------------------------------------------------*/

extern int test_speed;
extern uint8 roost;

#endif