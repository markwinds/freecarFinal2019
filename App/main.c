#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE]; //定义存储接收图像的数组
uint8 img[CAMERA_H][CAMERA_W];

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void HardWare_Init(void);
void PIT0_IRQHandler();
void PORTE_IRQHandler();
void set_vector_handler(VECTORn_t, void pfunc_handler(void)); //设置中断函数到中断向量表里

Screen_Data mydata[] = {
    { "speed", { .l = &(SpeedSet) }, 1, 45 },
    { "end", NULL, 1202, 0 }
};

int         block          = 0;
Screen_Data debug_window[] = {
    { "    ", { .i = NULL }, 1, 0 },
    { "speed", { .l = &(SpeedSet) }, 1, 0 },
    { "speeds", { .l = &(GetLeftMotorPules) }, 1, 0 },
    { "speedls", { .l = &(GetRightMotorPules) }, 1, 0 },
    { "end", NULL, 1202, 0 }
};

void HardWare_Init(void)
{
    DisableInterrupts;

    camera_init(imgbuff);                                //摄像头初始化
    set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
    set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);   //设置 DMA0 的中断服务函数为 PORTA_IRQHandler

#if 1
    ftm_quad_init(FTM1);                             //FTM1  PTA8 （ PTA9 ）正交解码初始化
    ftm_quad_clean(FTM1);                            //计数寄存器清零
    ftm_quad_init(FTM2);                             //FTM2  PTA10 （ PTA11 ）正交解码初始化
    ftm_quad_clean(FTM2);                            //计数寄存器清零
    lptmr_pulse_init(LPT0_ALT2, 0xffff, LPT_Rising); //PTA19
    lptmr_pulse_clean();
#endif

#if 1
    pit_init_ms(PIT0, 1);                              //PIT初始化  1ms
    set_vector_handler(PIT0_VECTORn, PIT0_IRQHandler); // 设置中断复位函数到中断向量表
    enable_irq(PIT0_IRQn);                             // 使能PIT0中断

#endif

    NVIC_SetPriorityGrouping(4);     //设置优先级分组,4bit 抢占优先级,没有亚优先
    NVIC_SetPriority(PORTA_IRQn, 0); //配置优先级  图像场中断  PORTA
    NVIC_SetPriority(DMA0_IRQn, 1);  //配置优先级  图像  DMA0
    NVIC_SetPriority(PIT0_IRQn, 2);  //配置优先级  编码器   PIT0

#if OpenDialSwitch

    DialSwitchInit(); //拨码开关初始化

#endif
    gpio_init(PTD6, GPO, 0); //蜂鸣器初始化

    SteerInit(); //舵机初始化

    MotorInit(); //电机初始化

    LCD_Init(); // LCD_ST7735R 液晶初始化,不能初始化在摄像头前面

    screen_data = mydata;
    dvarious    = debug_window;
    UI_INIT();

    EnableInterrupts; //使能总中断
}

void main(void)
{
    uint8 lcd_count = 0;
    HardWare_Init();

    while (1)
    {
        camera_get_img(); //（耗时13.4ms）图像采集
        img_extract(img, imgbuff);
        GetBlackEndParam(); //获取黑线截止行
        SearchCenterBlackline();
        LoopFlag = 0; //环路清标志
#if LoopOpen
        if (MotivateLoopDlayFlagL == 0 && MotivateLoopDlayFlagR == 0 && CloseLoopFlag == 0) //进了环道或者十字，关掉圆环处理
        {
            FindInflectionPoint(); //寻找拐点
            FindLoopExit();
            LoopControl();
            LoopRepair();
        }

        if (MotivateLoopDlayFlagL || MotivateLoopDlayFlagR)
        {
            LoopExitRepair(); //出口处理
        }
#endif
        if (LoopRightControlFlag == 0 && LoopLeftControlFlag == 0 && MotivateLoopDlayFlagL == 0 && MotivateLoopDlayFlagR == 0 && LoopFlag == 0)
        {
            NormalCrossConduct();
        }

#if ObstacleOpen //如果不需要避障碍，将这个宏定义置0即可

        RecognitionObstacle();
#endif

        if (getSwitch(motorSW))
        {
            MotorControl();
        }
        else
        {
            SpeedSet = 0;
            ftm_pwm_duty(FTM3, FTM_CH0, 0);
            ftm_pwm_duty(FTM3, FTM_CH2, 0); //PTC2,左电机
        }

        if (getSwitch(steerSW))
        {
            SteerControl();
        }
        else
        {
            ftm_pwm_duty(FTM0, FTM_CH6, SteerMidle); //舵机pwm更新
        }

        if (getSwitch(mainShowSW))
        {
            if (lcd_count > 32)
            {
                if (getSwitch(lcdSW))
                {
                    LCDDisplay(); //液晶显示
                }
                if (getSwitch(adjustSW))
                {
                    updateadjustUI();
                }
            }
            if (lcd_count > 32)
            {
                lcd_count = 0;
            }
            else
            {
                lcd_count++;
            }
        }
    }

}

void PORTA_IRQHandler()
{
    uint8  n; //引脚号
    uint32 flag;

    while (!PORTA_ISFR)
        ;
    flag       = PORTA_ISFR;
    PORTA_ISFR = ~0; //清中断标志位

    n = 29;              //场中断
    if (flag & (1 << n)) //PTA29触发中断
    {
        camera_vsync();
    }
#if (CAMERA_USE_HREF == 1) //使用行中断
    n = 28;
    if (flag & (1 << n)) //PTA28触发中断
    {
        camera_href();
    }
#endif
}

void DMA0_IRQHandler()
{
    camera_dma();
}
