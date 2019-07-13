#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE]; //定义存储接收图像的数组
uint8 img[CAMERA_H][CAMERA_W];

uint32 temx, temy, tem1;
int32  zbttem;
float  temf = 0.23;

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void HardWare_Init(void);
void PIT0_IRQHandler();
void PORTE_IRQHandler();
void set_vector_handler(VECTORn_t, void pfunc_handler(void)); //设置中断函数到中断向量表里

Screen_Data mydata[] = { //
    { "speed", { .l = &(MySpeedSet) }, 1.0, 1 },
    { "eleSpe", { .l = &(eleSpeed) }, 1.0, 1 },
    { "KP", { .f = &(BasicP) }, 0.1, 2 },
    { "KD", { .f = &(KD) }, 0.01, 2 },
    { "-reSpe", { .f = &(errorspeed) }, 0.1, 2 },
    { "adcp", { .l = &(ADC_pid.kp) }, 1.0, 1 },
    { "adcd", { .l = &(ADC_pid.kd) }, 1.0, 1 },
    { "end", NULL, 0, 0 }
};

int         block          = 0;
Screen_Data debug_window[] = {
    { "    ", NULL, 0, 0 },
    //{ "speed", { .l = &(SpeedSet) }, 0, 1 },ADC_normal_vaule
    { "error", { .f = &(Error) }, 0, 2 },
    { "adcl", { .i = &(disgy_AD_val[0]) }, 0, 3 },
    { "adcr", { .i = &(disgy_AD_val[1]) }, 0, 3 },
    { "adcm", { .i = &(disgy_AD_val[2]) }, 0, 3 },
    { "zbtadc", { .l = &(ADC_normal_vaule[0]) }, 0, 1 },
    { "blm", { .i = &(BlackEndM) }, 0, 3 },
    { "bll", { .i = &(BlackEndML) }, 0, 3 },
    { "blr", { .i = &(BlackEndMR) }, 0, 3 },
    { "blll", { .i = &(BlackEndL) }, 0, 3 },
    { "blrr", { .i = &(BlackEndR) }, 0, 3 },
    { "zbter", { .l = &(ADC_max_vaule[0]) }, 0, 1 },
    { "blm", { .i = &(BlackEndM) }, 0, 3 },
    { "cir", { .c = &(circluFlag) }, 0, 4 },
    { "cirlose", { .c = &(circlulose) }, 0, 4 },
    { "end", NULL, 0, 0 }
};

void HardWare_Init(void)
{
    DisableInterrupts;

    camera_init(imgbuff);                                //摄像头初始化
    set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler); //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
    set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);   //设置 DMA0 的中断服务函数为 PORTA_IRQHandler

#if 1
    ftm_quad_init(FTM1);  //FTM1  PTA8 （ PTA9 ）正交解码初始化
    ftm_quad_clean(FTM1); //计数寄存器清零
    ftm_quad_init(FTM2);  //FTM2  PTA10 （ PTA11 ）正交解码初始化
    ftm_quad_clean(FTM2); //计数寄存器清零
    //lptmr_pulse_init(LPT0_ALT2, 0xffff, LPT_Rising); //PTA19
    // lptmr_pulse_clean();
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
    gpio_init(PTD2, GPO, 0); //蜂鸣器初始化

    SteerInit(); //舵机初始化

    MotorInit(); //电机初始化

    LCD_Init(); // LCD_ST7735R 液晶初始化,不能初始化在摄像头前面

    screen_data = mydata;
    dvarious    = debug_window;
    UI_INIT();

    adc_init(ADC0_DP0); //ADC初始化1
    adc_init(ADC0_DP1); //ADC初始化2
    adc_init(ADC0_DP3); //ADC初始化3
    adc_init(ADC0_DM1); //ADC初始化4
    adc_init(ADC1_DM1); //ADC初始化5
    adc_init(ADC1_DP1); //ADC初始化6

    EnableInterrupts; //使能总中断
}

void main(void)
{
    initMotorSteer();
    initADC();

    uint8 lcd_count = 0;
    HardWare_Init();
    InitEM();
    while (1)
    {
        // if (out_road)
        // {
        //     setSteer(-50);
        //     DELAY_MS(300);
        //     setSteer(50);
        //     DELAY_MS(300);
        //     out_road = 0;
        // }

        if (getSwitch(cameraSW))
        {                     //控制图像处理
            camera_get_img(); //（耗时13.4ms）图像采集
            img_extract(img, imgbuff);
            GetBlackEndParam(); //获取黑线截止行
            SearchCenterBlackline();
#if LoopOpen
            LoopFlag = 0; //环路清标志

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
            if (LoopRightControlFlag == 0 && LoopLeftControlFlag == 0 && MotivateLoopDlayFlagL == 0 && MotivateLoopDlayFlagR == 0 && LoopFlag == 0)
            {
                NormalCrossConduct();
            }
#endif

#if ObstacleOpen //如果不需要避障碍，将这个宏定义置0即可

            RecognitionObstacle();
#endif
        }
        /*
        if (BlackEndM < 35 && BlackEndM > 28 && BlackEndML - BlackEndM < 3 && BlackEndM - BlackEndMR < 3 && BlackEndM >= BlackEndMR && BlackEndML >= BlackEndM && !DisconnectFlag)
        {
            DisconnectFlag = 1; //断路识别
        }
        else if (DisconnectFlag == 1 && BlackEndM < 10)
        {
            DisconnectFlag = 2;
        }
        else if (DisconnectFlag == 2 && BlackEndM > 40)
        {
            DisconnectFlag = 0;
        }*/

        /* if ((disgy_AD_val[0] > 95 || disgy_AD_val[1] > 95) && disgy_AD_val[2] > 95)
        {
            uint8 a, b;
            a = adc_once(ADC0_DP0, ADC_10bit);
            b = adc_once(ADC0_DM1, ADC_10bit);
            if (a > 100 && a > b)
            {
                //左环
                temx = 1;
            }
            else if (b > 100 && b > a)
            {
                //右环
                temy = 1;
            }
        }*/
        //temx = adc_once(ADC0_DP0, ADC_10bit);
        //temy = adc_once(ADC0_DM1, ADC_10bit);
        CircluSearch();
        if (LK_jishi_flag && BlackEndM > 10)
        {
            star_line_judg();
        }

        if (getSwitch(steerSW)) //控制舵机开关
        {
            if (breakLoadFlag)
            {
                Error       = 0;
                int32 error = getSteerPwmFromADCError();
                setSteer(error);
                if (BlackEndM > 30) //如果摄像头得到有用的图像
                {
                    breakLoadFlag = 0;
                    eleSpeed += MySpeedSet;
                    MySpeedSet = eleSpeed - MySpeedSet;
                    eleSpeed -= MySpeedSet;
                }
            }
            else
            {
                SteerControl();
            }
        }
        else
        {
            ftm_pwm_duty(FTM0, FTM_CH6, SteerMidle); //舵机pwm更新
        }
        if (getSwitch(motorSW)) //控制电机开关 && !star_lineflag && go
        {
            MotorControl();
        }

        if (getSwitch(mainShowSW)) //控制DeBug显示
        {
            //ftm_pwm_duty(FTM0, FTM_CH6, tem1);
            //temx = ReadValue(ADC_LEFT);
            //temy = ReadValue(ADC_RIGHT);
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
        if (getSwitch(ADCSW))
        {
            LCD_clear(WHITE);
            initADCUI();
            updateADCVaule();
            showADCvaule();
            showTrueError();
            DELAY_MS(500);
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
