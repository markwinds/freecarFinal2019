#include "common.h"
#include "include.h"

void initNVIC()
{
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4); //设置抢占优先级

    //NVIC_SetPriority(PIT2_IRQn, 7);        //道路状态清空和
    //NVIC_SetPriority(PIT1_IRQn, 10);       //摄像头的采集图片中断

    NVIC_SetPriority(UART4_RX_TX_IRQn, 8); //命令行中断
    NVIC_SetPriority(PORTD_IRQn, 3);       //LCD的按键中断
    NVIC_SetPriority(PIT1_IRQn, 4);        //flag保持时间定时器
    NVIC_SetPriority(DMA0_IRQn, 5);        //摄像头的DMA中断
    NVIC_SetPriority(PORTA_IRQn, 6);       //摄像头的场中断
    NVIC_SetPriority(PIT0_IRQn, 7);        //配置优先级  编码器   PIT0

    // NVIC_SetPriority(PORTA_IRQn, 0); //配置优先级  图像场中断  PORTA
    // NVIC_SetPriority(DMA0_IRQn, 1);  //配置优先级  图像  DMA0
}