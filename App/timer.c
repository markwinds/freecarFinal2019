int* p_timer_flag;
int  timer_busy     = 0;
int  pre_timer_flag = 0;

#include "common.h"
#include "include.h"

void startTimer(PITn_e pitn, uint32 cnt)
{
    //PIT 用的是 Bus Clock 总线频率
    //溢出计数 = 总线频率 * 时间

    ASSERT(cnt > 0); //用断言检测 时间必须不能为 0

    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; //使能PIT时钟

    PIT_MCR = (0
               //| PIT_MCR_MDIS_MASK       //禁用PIT定时器时钟选择（0表示使能PIT，1表示禁止PIT）
               //| PIT_MCR_FRZ_MASK        //调试模式下停止运行（0表示继续运行，1表示停止运行）
    );

    PIT_LDVAL(pitn) = cnt - 1; //设置溢出中断时间

    // PIT_Flag_Clear(pitn); //清中断标志位

    // PIT_TCTRL(pitn) &= ~PIT_TCTRL_TEN_MASK;   //禁止PITn定时器（用于清空计数值）
    // PIT_TCTRL(pitn) = (0 | PIT_TCTRL_TEN_MASK //使能 PITn定时器
    //                    | PIT_TCTRL_TIE_MASK   //开PITn中断
    // );

    /**********************zbt**********************/
    PIT_TCTRL(pitn) &= ~PIT_TCTRL_TEN_MASK; //禁止PITn定时器（用于清空计数值）
    PIT_TCTRL(pitn) |= PIT_TCTRL_TEN_MASK;  //使能 PITn定时器
    PIT_Flag_Clear(pitn);                   //清中断标志位
    PIT_TCTRL(pitn) |= PIT_TCTRL_TIE_MASK;  //开PITn中断
    /**********************zbt**********************/

    //enable_irq((int)pitn + PIT1_IRQn);            //开中断
}

void closeTimer(PITn_e pitn)
{
    PIT_TCTRL(pitn) &= ~PIT_TCTRL_TIE_MASK; //中断？
    //PIT_TCTRL(pitn) &= ~PIT_TCTRL_TEN_MASK; //禁止PITn定时器（用于清空计数值）
    PIT_Flag_Clear(pitn);                   //清中断标志位
    PIT_TCTRL(pitn) &= ~PIT_TCTRL_TEN_MASK; //禁止PITn定时器（用于清空计数值）
}

//定时的单位为ms
void setFlagInTimerLong(int* p_flag, int time_long, int vaule)
{
    p_timer_flag   = p_flag;        //得到要操作的标志位的地址
    pre_timer_flag = *p_timer_flag; //保存操作之前的值
    *p_timer_flag  = vaule;         //将标志位赋值
    while (timer_busy)              //等待定时器空闲
        ;
    timer_busy = 1;                            //占用定时器
    startTimer(PIT1, time_long * bus_clk_khz); //开始定时
}

void resetFlagTimer()
{
    closeTimer(PIT1);               //关闭定时器
    *p_timer_flag = pre_timer_flag; //恢复标志
    timer_busy    = 0;              //释放定时器资源
}

void initTimerForFlag()
{
    closeTimer(PIT1);
    enable_irq(PIT1_IRQn);
    set_vector_handler(PIT1_VECTORn, resetFlagTimer);
}