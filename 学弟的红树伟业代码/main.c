/****************************************************************************************************
【dev.env.】IAR8.3
【Target  】K66FX1M0VLQ18
【Crystal 】 50.000Mhz
【busclock】 40.000MHz
【pllclock】200.000MHz
越野车专用程序
******************************************************************************************************/

#include "include.h"

#define servMotorCenture 210 //570// // //1080 //1080   //1095     //舵机中心位置
#define servMotorLeft 170	//890  // ////955  //950    //970     //舵机左极限，很关键，限幅防止舵机打死
#define servMotorRight 245   //1210 // //  //1220     //舵机右极限，很关键，限s幅防止舵机打死

s32 TurnPosition;	//当前实际位置
s32 TurnPositionOld; //上一次位置
////////////////////////////////

u8 ensend;	//允许发送
u8 speedmodi; //速度修改标志

u8 workmode = 0; //工作状态 0：正常   1：舵机调试
u8 keymode = 0;  //0: 正常  1：调节高速PD  2：调节低速PD   3：调节高低速度值
u8 lock_flag = 0;
//
unsigned short DuoCenter;   //舵机中间值 100HZ
unsigned short servPWMDuty; //当前舵机值
unsigned short dianjispeed;
s32 TimeCount = 0; //1ms中断标志
u8 adjspdcnt;	  //速度调节时间
u8 roadmode;	   //赛道状态  1： 正常   2: 小弯   3：大弯   4：前端丢线，弯道向右  5：前端丢线，弯道向左
u8 lockduoji;	  //舵机锁死状态  0： 不锁死   1：锁死（适用在丢线状态）
u8 enDuoji;		   //舵机控制允许
u8 workcnt, discnt;

// 舵机控制参数
int AD_val_1 = 0; //左电感
int AD_val_2 = 0; //右电感
int AD_val_3 = 0; //中间电感

int adtmp1, adtmp2, adtmp3;
int dis_AD_val_1, dis_AD_val_2, dis_AD_val_3;
int disgy_AD_val_1, disgy_AD_val_2, disgy_AD_val_3;

int AD_val_1_min = 0;
int AD_val_2_min = 0;
int AD_val_3_min = 0;

int AD_val_1_max = 2400;
int AD_val_2_max = 2400;
int AD_val_3_max = 2400;

int AD1DD, AD2DD, AD3DD; //最大和最小的差值

int AD_val_1old;
int AD_val_2old;
int AD_val_3old;
u8 adflag;

unsigned char Dir_last = 0;
int dir_error_pre = 0;
int dir_error = 0;
int dis_error, dis_error1, dis_error_pre;
unsigned char zz;

u32 sum = 0;

//速度控制参数
u16 setspeed;			//设定速度
u16 nowspeed;			//当前速度
u16 rspeed;				//速度计数
u16 setspeedhigh = 330; //直道时速度  250
u16 setspeedmid = 280;
u16 setspeedlow = 240; //450
u16 speedkp = 20;	  //10;
u16 speedkd = 20;	  //速度调节PD值
s32 newspeedpwm;	   //速度PWM
s32 oldspeedpwm;	   //上一次速度PWM

////////////////控制参数 ////////////////
s16 rkp3 = 10; //   5  转向PD参数 直道 放大10倍
s16 rkd3 = 60; // 160

s16 rkp2 = 13; //9  转向PD参数 小弯道
s16 rkd2 = 30; //100; //130

s16 rkp1 = 21; //6  转向PD参数 大弯道
s16 rkd1 = 10; //45

int DuoP; //
int DuoD;

//-----------------函数声明------------------
void PORT_INI();
void PWM_Init();
//void UART_Init();
void Image_Init();
void CountThreshole(void);
void LCD_Init();
//void FTM_IN_Init();
void AD_Init();   //AD初始化
void disnowval(); //显示参数
void oled_init();
void LCD_P6x8Str(u8 x, u8 y, u8 ch[]);
void Dis_num(u8 x, u8 y, int asc);
void LCD_CLS();
void adjspeed();
void getadval();
void adjduoji();

//主函数
void main(void)
{ //unsigned int i;
	unsigned short duotmp;
	unsigned short speedtmp;
	DisableInterrupts;			  //关闭总中断
	PLL_Init(PLL200);			  //初始化PLL为200M，总线为40MHZ
								  //////////////////////////////////////////////////////////////////////
	DuoCenter = servMotorCenture; //  163x0;  //   1210  ~   1610
	dianjispeed = 13;
	enDuoji = 1;
	DuoP = rkp3;
	DuoD = rkd3;
	setspeed = setspeedhigh;
	roadmode = 1;  //直道模式
	lockduoji = 0; //舵机不锁
	PORT_INI();	//------GPIO初始化
	PWM_Init();
	//PIT_Init(PIT0, 20);      //中断定时20ms，
	LED_Init();			  //LED初始化
	LED_Ctrl(LEDALL, ON); //LED全亮
	// 一路测速
	FTM_Input_init(FTM1, FTM_CH0, FTM_Rising);
	NVIC_EnableIRQ(43);
	//UART_Init(UART_0,115200);
	LCD_Init();
	AD_Init();
	EnableInterrupts;

	AD1DD = AD_val_1_max - AD_val_1_min;
	AD2DD = AD_val_2_max - AD_val_2_min;
	AD3DD = AD_val_3_max - AD_val_3_min;
	//
	LCD_CLS(); //清屏
	LCD_P6x8Str(0, 0, "LMax:      LMin:");
	LCD_P6x8Str(0, 1, "RMax:      RMin:");
	LCD_P6x8Str(0, 2, "CMax:      CMin:");
	Dis_num(32, 0, AD_val_1_max);
	Dis_num(96, 0, AD_val_1_min);
	Dis_num(32, 1, AD_val_3_max);
	Dis_num(96, 1, AD_val_3_min);
	Dis_num(32, 2, AD_val_2_max);
	Dis_num(96, 2, AD_val_2_min);
	time_delay_ms(10);
	while (1) //显示当前电感归一化值，查看归一化值是否正确，等待按鍵PA9按下后小车出发
	{
		adtmp1 = 0;
		adtmp2 = 0;
		adtmp3 = 0;

		AD_val_1 = ADC_Ave(ADC_1, ADC1_SE13, ADC_12bit, 50); //左  B7
		AD_val_2 = ADC_Ave(ADC_0, ADC0_SE8, ADC_12bit, 50);  //右  B0
		AD_val_3 = ADC_Ave(ADC_1, ADC1_SE11, ADC_12bit, 50); //中间 B5

		if (AD_val_1 > AD_val_1_max)
			AD_val_1 = AD_val_1_max;
		if (AD_val_2 > AD_val_2_max)
			AD_val_2 = AD_val_2_max;
		if (AD_val_3 > AD_val_3_max)
			AD_val_3 = AD_val_3_max;
		if (AD_val_1 < AD_val_1_min)
			AD_val_1 = AD_val_1_min;
		if (AD_val_2 < AD_val_2_min)
			AD_val_2 = AD_val_2_min;
		if (AD_val_3 < AD_val_3_min)
			AD_val_3 = AD_val_3_min;

		//归一化
		dis_AD_val_1 = 100 * (AD_val_1 - AD_val_1_min) / AD1DD;
		dis_AD_val_2 = 100 * (AD_val_2 - AD_val_2_min) / AD2DD;
		dis_AD_val_3 = 100 * (AD_val_3 - AD_val_3_min) / AD3DD;

		LCD_P6x8Str(0, 3, "LEFT  CEN   RIGHT");
		Dis_num(0, 4, AD_val_1);
		Dis_num(40, 4, AD_val_3);
		Dis_num(80, 4, AD_val_2);
		LCD_P6x8Str(0, 5, "GuiYi:");
		Dis_num(0, 6, dis_AD_val_1);
		Dis_num(40, 6, dis_AD_val_3);
		Dis_num(80, 6, dis_AD_val_2);

		if (GPIO_Get(PTA9) == 0)
		{
			time_delay_ms(5);
			if (GPIO_Get(PTA9) == 0)
			{
				while (GPIO_Get(PTA9) == 0)
					;
				break;
			}
		}
	}
#if 1 // 舵机中值调试
	duotmp = DuoCenter;
	FTM_PWM_Duty(FTM2, FTM_CH0, duotmp);
	LCD_CLS(); //清屏
	LCD_P6x8Str(20, 1, "DuoCenADJ");
	Dis_num(46, 2, duotmp);
	while (1)
	{
		if (GPIO_Get(PTA9) == 0) //退出
		{
			time_delay_ms(4);
			if (GPIO_Get(PTA9) == 0)
			{
				while (GPIO_Get(PTA9) == 0)
					;
				break;
			}
		}
		if (GPIO_Get(PTA27) == 0) // 中值加
		{
			time_delay_ms(4);
			if (GPIO_Get(PTA27) == 0)
			{
				duotmp = duotmp + 5;
				FTM_PWM_Duty(FTM2, FTM_CH0, duotmp * 10);
				Dis_num(46, 2, duotmp);
				while (GPIO_Get(PTA27) == 0)
					;
			}
		}
		if (GPIO_Get(PTA24) == 0) //中值减
		{
			time_delay_ms(4);
			if (GPIO_Get(PTA24) == 0)
			{
				duotmp = duotmp - 5;
				FTM_PWM_Duty(FTM2, FTM_CH0, duotmp * 10);
				Dis_num(46, 2, duotmp);
				while (GPIO_Get(PTA24) == 0)
					;
			}
		}
	}
#endif
#if 0 // 舵机中值调试
   speedtmp = 1000;
   FTM_PWM_Duty(FTM0, FTM_CH0, speedtmp);
   LCD_CLS();  //清屏
   LCD_P6x8Str(20,1,"speedADJ");
   Dis_num(45,2,speedtmp);
   while(1)
   { if(GPIO_Get(PTA9)==0)  //退出
     { time_delay_ms(4);
       if(GPIO_Get(PTA9)==0)
       { while(GPIO_Get(PTA9)==0);
         break;
       }
     }
     if(GPIO_Get(PTA27)==0)  // 中值加
     { time_delay_ms(4);
       if(GPIO_Get(PTA27)==0)
       { speedtmp = speedtmp + 100;
         FTM_PWM_Duty(FTM2, FTM_CH0, speedtmp);
         Dis_num(45,2,speedtmp);
         while(GPIO_Get(PTA27)==0);
       }
     }
     if(GPIO_Get(PTA24)==0)  //中值减
     { time_delay_ms(4);
       if(GPIO_Get(PTA24)==0)
       { speedtmp = speedtmp - 100;
         FTM_PWM_Duty(FTM0, FTM_CH0, speedtmp);
         Dis_num(45,2,speedtmp);
         while(GPIO_Get(PTA24)==0);
       }
     }
   }
#endif
	///////////////////// 准备前进  ///////////
	workmode = 0;
	LCD_CLS(); //清屏
	LCD_P6x8Str(0, 0, "DuoPWM:");
	Dis_num(46, 0, DuoCenter);
	LCD_P6x8Str(0, 1, "CarSpd:");
	LCD_P6x8Str(0, 2, "D_err:");
	LCD_P6x8Str(0, 3, "ADVAL:");		 //AD值
	LCD_P6x8Str(0, 5, "GYVAL:");		 //归一
	LCD_P6x8Str(0, 7, "Dp:        Dd:"); //舵机转向PD
	TimeCount = 0;
	enDuoji = 0;
	keymode = 0;
	while (1)
	{
		FTM_PWM_Duty(FTM0, FTM_CH0, 0);
		FTM_PWM_Duty(FTM0, FTM_CH1, 2500);
		getadval(); //读AD值
		adjduoji(); //控制舵机
					/* if(keymode==0)  //正常状态
        {
        workcnt++;
        discnt++;
        if(workcnt>=4)  //读速度
        { nowspeed = rspeed;
          rspeed=0;
          adjspeed();
          workcnt=0;
        }
        getadval();   //读AD值
        adjduoji();   //控制舵机     
        if(discnt>100)  //刷新显示
        {
           disnowval();
           discnt=0;
           LED_Ctrl(LED0, RVS);
           LED_Ctrl(LED1, RVS);
           LED_Ctrl(LED2, RVS);
           LED_Ctrl(LED3, RVS); 
        }
        time_delay_ms(1);
      }*/
	}
}

void PORT_INI()
{
	GPIO_Init(PTA, 17, GPO, 1u); //初始化PTA17为高电平输出---LED0
	GPIO_Init(PTC, 5, GPO, 1u);  //初始化PTC5为高电平输出---LED0
	GPIO_Init(PTD, 15, GPO, 1u); //初始化PTD15为高电平输出---LED0
	GPIO_Init(PTE, 24, GPO, 1u); //初始化PTE24为高电平输出---LED0

	GPIO_Init(PTA, 24, GPI, 1); //摇杆输入
	GPIO_Init(PTA, 25, GPI, 1); //摇杆输入
	GPIO_Init(PTA, 26, GPI, 1); //摇杆输入
	GPIO_Init(PTA, 27, GPI, 1); //摇杆输入
	GPIO_Init(PTA, 28, GPI, 1); //摇杆输入

	GPIO_Init(PTA, 9, GPI, 1); //按键输入
}

void PWM_Init() //FTM_PWM_Init(FTM0,FTM_CH6,499,250);  50%占空比，频率5M / 500 = 10KHz
{
	FTM_PWM_Init(FTM0, FTM_CH0, 5000, 0); //5M/5000 = 100Hz    PTC1
	FTM_PWM_Init(FTM0, FTM_CH1, 5000, 0); //5M/5000 = 100Hz    PTC2
	//FTM_PWM_Init(FTM0, FTM_CH2, 5000,0); //5M/5000 = 100Hz    PTC3
	//FTM_PWM_Init(FTM0, FTM_CH3, 50000,0); //5M/5000 = 1000Hz    PTC4

	FTM_PWM_Init(FTM2, FTM_CH0, 25000, DuoCenter); // 1.25M/25000 = 50 Hz
}

void AD_Init()
{
	ADC_Init(ADC_0);
	ADC_Init(ADC_1);
	//  adc_init(ADC0, AD8) ;     //PTB0    最右电磁
	//  adc_init(ADC1, AD10) ;     //PTB4   中间
	//  adc_init(ADC1, AD13) ;     //PTB7   最左电磁
}

void disnowval() //   左AD1    中AD3    右AD2
{
	Dis_num(46, 0, DuoCenter);
	Dis_num(80, 0, servPWMDuty);
	Dis_num(48, 1, nowspeed);
	Dis_num(96, 1, setspeed);
	Dis_num(40, 2, dis_error);
	Dis_num(90, 2, dis_error1);
	Dis_num(65, 3, dis_error_pre);
	Dis_num(10, 4, dis_AD_val_1);
	Dis_num(50, 4, dis_AD_val_3);
	Dis_num(90, 4, dis_AD_val_2);
	Dis_num(10, 6, disgy_AD_val_1);
	Dis_num(50, 6, disgy_AD_val_3);
	Dis_num(90, 6, disgy_AD_val_2);
	Dis_num(32, 7, rkp3);
	Dis_num(96, 7, rkd3);
}

void adjspeed() //速度控制
{
	if (enDuoji == 0) //每100ms调节一次速度，可以修改
	{
		if (nowspeed < setspeed - 4) //速度过慢时加速,在小车急弯时推不动，需要加PWM
		{
			newspeedpwm = dianjispeed + speedkp * (setspeed - nowspeed); // + speedkd*(dspeed - dspeedold);
			//if(newspeedpwm>(oldspeedpwm+500)) newspeedpwm = oldspeedpwm + 500;  //加速时PWM变化不宜太快，最大加幅为500
			if (newspeedpwm > 45)						  //如果有坡道爬不上，要增大该值。最大9900
				newspeedpwm = 45;						  //1300
			FTM_PWM_Duty(FTM0, FTM_CH0, newspeedpwm / 2); //PTC1 小车前进
		}
		else if (nowspeed > setspeed + 3) //速度过快，需要减速
		{
			newspeedpwm = dianjispeed + speedkp * (setspeed - nowspeed); //100
			if (newspeedpwm < 100)
				newspeedpwm = 100;						  //需要保证pwm值不能小于0
			FTM_PWM_Duty(FTM0, FTM_CH0, newspeedpwm / 2); //PTC1 小车前进
		}
		else //正常速度
		{
			newspeedpwm = dianjispeed;
			FTM_PWM_Duty(FTM0, FTM_CH0, newspeedpwm / 2); //PTC1 小车前进
		}
		adjspdcnt = 0;
		oldspeedpwm = newspeedpwm; //记录上次的PWM输出
	}
}

void getadval() //ADC_Ave(ADC_1,ADC1_SE8,ADC_12bit,100)
{
	AD_val_1 = ADC_Ave(ADC_1, ADC1_SE13, ADC_12bit, 50); //左  B7
	AD_val_2 = ADC_Ave(ADC_0, ADC0_SE8, ADC_12bit, 50);  //右  B0
	AD_val_3 = ADC_Ave(ADC_1, ADC1_SE11, ADC_12bit, 50); //中间 B5

	dis_AD_val_1 = AD_val_1;
	dis_AD_val_2 = AD_val_2;
	dis_AD_val_3 = AD_val_3;

	//限幅
	if (AD_val_1 > AD_val_1_max)
		AD_val_1 = AD_val_1_max;
	if (AD_val_2 > AD_val_2_max)
		AD_val_2 = AD_val_2_max;
	if (AD_val_3 > AD_val_3_max)
		AD_val_3 = AD_val_3_max;

	if (AD_val_1 < AD_val_1_min)
		AD_val_1 = AD_val_1_min;
	if (AD_val_2 < AD_val_2_min)
		AD_val_2 = AD_val_2_min;
	if (AD_val_3 < AD_val_3_min)
		AD_val_3 = AD_val_3_min;

	AD_val_1 = 100 * (AD_val_1 - AD_val_1_min) / AD1DD;
	AD_val_2 = 100 * (AD_val_2 - AD_val_2_min) / AD2DD;
	AD_val_3 = 100 * (AD_val_3 - AD_val_3_min) / AD3DD;

	disgy_AD_val_1 = AD_val_1;
	disgy_AD_val_2 = AD_val_2;
	disgy_AD_val_3 = AD_val_3;
}

void adjduoji()
{
	//if(AD_val_3==30 && abs(AD_val_2-AD_val_1)<1&&15<=AD_val_1<=20&&15<=AD_val_2<=20) Dir_last=0;
	//  Dir_last=0;
	if ((AD_val_1 - AD_val_2 >= 13) && lock_flag == 0)
	{
		Dir_last = 1; //上一状态为左转
		lock_flag = 1;
		time_delay_ms(25);
	}
	if ((AD_val_2 - AD_val_1 >= 13) && lock_flag == 0)
	{
		Dir_last = 0; //上一状态为右转
		lock_flag = 1;
	}
	if (Dir_last == 1) //大弯或丢线
	{
		servPWMDuty = servMotorLeft; //保持左转
		DuoP = rkp3;
		DuoD = rkd3;
	}
	else
	{
		servPWMDuty = servMotorRight; //保持左转
		DuoP = rkp3;
		DuoD = rkd3;
	}
	//正常处理
	if (AD_val_1 >= 40 || AD_val_3 > 24 || AD_val_2 >= 40)
	{
		lock_flag = 0;
		dir_error_pre = dir_error; //上个偏差
		dis_error_pre = dir_error_pre;
		dir_error = 100.0 * (AD_val_2 - AD_val_1) / (AD_val_2 + AD_val_1); //偏差量
		dis_error1 = dir_error;
		dir_error = dis_error1 * (dis_error1 * dis_error1 / 1250.0 + 2) / 10.0;
		dis_error = dir_error;
		DuoP = rkp3;
		DuoD = rkd3;
		servPWMDuty = DuoCenter + DuoP * dir_error / 10 + DuoD * (dir_error - dir_error_pre) / 10;
		if (AD_val_3 >= 70)
			setspeed = setspeedhigh;
		else
			setspeed = setspeedlow;
	}
	if (servPWMDuty <= servMotorLeft)
		servPWMDuty = servMotorLeft;
	if (servPWMDuty >= servMotorRight)
		servPWMDuty = servMotorRight;
	if ((servPWMDuty > (servMotorCenture + 25)) || (servPWMDuty < (servMotorCenture - 25))) //大弯道
	{
		setspeed = setspeedlow;
	}
	else if ((servPWMDuty > (servMotorCenture + 20)) || (servPWMDuty < (servMotorCenture - 20))) //小弯道
	{
		setspeed = setspeedmid;
	}
	else
	{
		setspeed = setspeedhigh;
	}
	if (enDuoji == 0)
	{
		if (workmode == 0) // 正常状态
			FTM_PWM_Duty(FTM2, FTM_CH0, servPWMDuty * 10);
		else //调试状态
			FTM_PWM_Duty(FTM2, FTM_CH0, DuoCenter * 10);
	}
}
