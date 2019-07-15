/********
* 处理电磁  
********/
#ifndef _ELECTROMAGNETIC_H_
#define _ELECTROMAGNETIC_H_

/*------------------------------------------------include----------------------------------------------*/
#include "common.h"
#include "include.h"

/*------------------------------------------------define------------------------------------------------*/
#define ADC_LEFT ADC0_DP1 //需要根据实际情况更改
#define ADC_RIGHT ADC1_DM1
#define ADC_MIDDLE ADC0_DM1
/* 
    horizontal_middle = ADC0_DP3,
    horizontal_left = ADC0_DP1,
    horizontal_right = ADC1_DM1,
    vertical_left = ADC0_DP0,
    vertical_right = ADC0_DM1
*/

/*------------------------------------------------Perdefine Type---------------------------------------------*/
typedef struct MedianTree
{
    uint8 history[64];
    uint8 tree[256];
    uint8 hvHead;
} MedianTree;
/*------------------------------------------------外部变量---------------------------------------------*/
extern int AD_val_min[3];
extern int AD_val_max[3];

extern int AD_val[3]; //left 0, right 1, middle 2
extern int dis_AD_val[3];
extern int disgy_AD_val[3];
/*--------------------------------------------------函数--------------------------------------------------*/
/*-------------------------domestic函数-------------------------*/
void TreeInsert(uint8*, int, int, int, int);
void TreeDelete(uint8*, int, int, int, int);
int  GetMedian(uint8*, int, int, int, int);

void InitTree(struct MedianTree*, ADCn_Ch_e);

uint8 Normalized(int);
/*-------------------------abroad函数-------------------------*/
extern int  ReadValue(ADCn_Ch_e);
extern void InitEM();
#endif