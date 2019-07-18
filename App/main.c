#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE]; //定义存储接收图像的数组
uint8 img[CAMERA_H][CAMERA_W];

uint32 temx, temy, tem1;
int32  zbttem;
float  temf         = 0.23;
int    is_roadblock = 0;
int    stopSpeed = 0, actualSpeed = 10;

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void HardWare_Init(void);
void PIT0_IRQHandler();
void PORTE_IRQHandler();
void set_vector_handler(VECTORn_t, void pfunc_handler(void)); //设置中断函数到中断向量表里

//可以调控的ui
Screen_Data mydata[] = { //
    { "speed", { .l = &(actualSpeed) }, 1.0, 1 },
    { "eleSpe", { .l = &(eleSpeed) }, 1.0, 1 },
    { "KP", { .f = &(BasicP) }, 0.1, 2 },
    { "KD", { .f = &(KD) }, 0.01, 2 },
    { "-reSpe", { .f = &(errorspeed) }, 0.1, 2 },
    { "adcp", { .l = &(ADC_pid.kp) }, 1.0, 1 },
    { "adcd", { .l = &(ADC_pid.kd) }, 1.0, 1 },
    { "tempi", { .l = &(temp_vaule) }, 50.0, 1 },
    { "tempf", { .f = &(temp_vaule_f) }, 0.1, 2 },
    { "KP+", { .f = &(BasicP) }, 1, 2 },
    { "KD+", { .f = &(KD) }, 0.1, 2 },
    { "moKP", { .f = &(SpeedP) }, 1, 2 },
    { "moKP+", { .f = &(SpeedP) }, 10, 2 },
    { "moKD", { .f = &(SpeedD) }, 1, 2 },
    { "moKI", { .f = &(SpeedI) }, 0.01, 2 },
    { "end", NULL, 0, 0 }
};

//用于查看的ui
int         block          = 0;
Screen_Data debug_window[] = {
    { "    ", NULL, 0, 0 },
    //{ "speed", { .l = &(SpeedSet) }, 0, 1 },ADC_normal_vaule
    { "error", { .f = &(Error) }, 0, 2 },
    { "adcl", { .i = &(disgy_AD_val[0]) }, 0, 3 },
    { "adcr", { .i = &(disgy_AD_val[1]) }, 0, 3 },
    { "adcm", { .i = &(disgy_AD_val[2]) }, 0, 3 },
    { "rslo", { .c = &(rSlope) }, 0, 4 },
    { "lslo", { .c = &(lSlope) }, 0, 4 },
    { "blm", { .i = &(BlackEndM) }, 0, 3 },
    { "bll", { .i = &(BlackEndML) }, 0, 3 },
    { "blr", { .i = &(BlackEndMR) }, 0, 3 },
    { "blll", { .i = &(BlackEndL) }, 0, 3 },
    { "blrr", { .i = &(BlackEndR) }, 0, 3 },
    { "blm", { .i = &(BlackEndM) }, 0, 3 },
    { "cir", { .c = &(circluFlag) }, 0, 4 },
    { "hamper", { .c = &(hamperFlag) }, 0, 4 },
    { "cirlose", { .c = &(circlulose) }, 0, 4 },
    { "end", NULL, 0, 0 }
};

/**每次循环都要进行的检测，主要是一些标志位的处理*/
void doThisEveryPeriod()
{
    checkBuzzerShouldSpeak();
    manageTimerFlag(); //处理定时器控制的标志位
}

//硬件初始化
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

    // NVIC_SetPriorityGrouping(4);     //设置优先级分组,4bit 抢占优先级,没有亚优先
    // NVIC_SetPriority(PORTA_IRQn, 0); //配置优先级  图像场中断  PORTA
    // NVIC_SetPriority(DMA0_IRQn, 1);  //配置优先级  图像  DMA0
    // NVIC_SetPriority(PIT0_IRQn, 2);  //配置优先级  编码器   PIT0

#if OpenDialSwitch

    DialSwitchInit(); //拨码开关初始化

#endif
    //gpio_init(PTD2, GPO, 0); //蜂鸣器初始化
    initBuzzer(); //蜂鸣器初始化

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
    //初始化
    /**---------------------zbt----------------------*/
    initNVIC();
    initMotorSteer();
    initADC();
    initUart();
    /**---------------------yl----------------------*/
    uint8 lcd_count = 0;
    HardWare_Init();
    InitEM();
    /**----------------------------------------------*/

    initTimerForFlag(); //这个必须放在最后初始化 zbt

    while (1)
    {
        if (getSwitch(cameraSW)) //控制图像处理
        {
            camera_get_img();          //（耗时13.4ms）图像采集
            img_extract(img, imgbuff); //解压 图像
            GetBlackEndParam();        //获取黑线截止行
            SearchCenterBlackline();   //搜边线
            if (!circluFlag)           //十字优化
            {
                NormalCrossConduct();
            }
        }

        CircluSearch(); //圆环识别

        if (LK_jishi_flag && BlackEndM > 10) //起跑线检测
        {
            star_line_judg();
        }

        /***********************************UI的控制***************************************/
        /*****************************舵机***************************/
        if (getSwitch(steerSW)) //控制舵机开关
        {
            if (is_roadblock)
            {
                setSteer(40);
                if (judgeRoadOk())
                {
                    is_roadblock = 0;
                }
            }
            else if (breakLoadFlag)
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

        /***************************电机*****************************/
        if (getSwitch(motorSW)) //控制电机开关 && !star_lineflag && go
        {
            if (stopSpeed)
            {
                MySpeedSet = actualSpeed;
                stopSpeed  = 0;
            }
            if (is_roadblock)
            {
                setSpeedLeft(1500);
                setSpeedRight(1500);
            }
            else
                MotorControl();
        }
        else
        {
            //ftm_pwm_duty(FTM3, FTM_CH0, 0);
            //ftm_pwm_duty(FTM3, FTM_CH1, 0);
            //ftm_pwm_duty(FTM3, FTM_CH2, 0); //PTC2,右电机
            //ftm_pwm_duty(FTM3, FTM_CH3, 0); //PTC2,右电机
            if (!stopSpeed)
            {
                stopSpeed  = 1;
                MySpeedSet = 0;
            }
            MotorControl();
        }

        /*************************显示*******************************/
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

        /********************************************************/
        if (getSwitch(ADCSW)) //电磁采集的显示
        {
            LCD_clear(WHITE);
            initADCUI();
            updateADCVaule();
            showADCvaule();
            showTrueError();
            //DELAY_MS(500);
        }
        /***********************************UI的控制的结尾***************************************/
        doThisEveryPeriod(); //不知道干嘛的
    }
}

//下面是山外摄像头中断配置
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
