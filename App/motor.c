
#include "common.h"
#include "include.h"
#include "motor.h"

/****电机初始化******/

int16 NumPulseFTM, NumPulselptmr; //脉冲计数寄存器的值
int32 RightMotorPulseAccumulate;
int32 LeftMotorPulseAccumulate;
int32 GetRightMotorPules, GetLeftMotorPules; //获得的一次脉冲总数（用于PID电机的数据处理）

int32 LSpeedSet  = 0; //60;//2.5M/S
int32 RSpeedSet  = 0;
int32 SpeedSet   = 0;
int32 MySpeedSet = 10;

float SpeedErrorL = 0;
float SpeedErrorR = 0;

float SpeedLastErrorL = 0;
float SpeedLastErrorR = 0;

float SpeedPerErrorL = 0;
float SpeedPerErrorR = 0;

int Error1 = 0, Error1Last = 0;
int SpeedDropRow = 0;
int LastSpeedDropRow;

#if 1

float SpeedP = 77.0;  //50.0;40
float SpeedI = 0.001; //16.0;50,0.0006
float SpeedD = 11.0;  //1.3,10.0

#endif

float Differential_P = 0.0; //0.0250,0.1050

int MotorPwmL = 0;
int MotorPwmR = 0;

int MotorPwmRight = 0;
int MotorPwmLeft  = 0;

float LeftMotorPwmAdd  = 0;
float RightMotorPwmAdd = 0;
float errorspeed       = 0.5;

void MotorInit(void)
{
    //gpio_init(PTB18, GPO, 0);             //0正转，1反转
    ftm_pwm_init(FTM3, FTM_CH2, 10000, 0); //PTC1，右电机
    ftm_pwm_init(FTM3, FTM_CH3, 10000, 0); //PTC1，右电机

    //gpio_init(PTB9, GPO, 0);
    ftm_pwm_init(FTM3, FTM_CH0, 10000, 0); //PTC2,左电机
    ftm_pwm_init(FTM3, FTM_CH1, 10000, 0); //PTC2,左电机
}

#if 1

void SpeedGet(void)

{
    NumPulselptmr = ftm_quad_get(FTM1);
    ftm_quad_clean(FTM1);                      //计数寄存器清零 测出来1100转10cm...v/88 = (m/s);(v/0.8)*100/1100/10
    LeftMotorPulseAccumulate += NumPulselptmr; //把每一次PIT定时时间的脉冲数累加到脉冲总数变量pulse_period_L

    NumPulseFTM = ftm_quad_get(FTM2);         //取寄存器的值
    ftm_quad_clean(FTM2);                     //计数寄存器清零
    RightMotorPulseAccumulate += NumPulseFTM; //单相脉冲    （正交解码测脉冲）右轮
}

int   LK_jishi_flag, LK_jishi;
uint8 runrunrun = 0, go;
int16 ste = 868, dir = 1;
void  PIT0_IRQHandler()
{

    static uint16 TimerCnt8ms = 0;

    disable_irq(PIT0_IRQn); //关PIT中断

    PIT_Flag_Clear(PIT0); //清中断标志位

    TimerCnt8ms++;

    SpeedGet(); //每次进来累加一次速度

    if (TimerCnt8ms >= MotorControlPeriod) //一个8ms周期取一次速度，当作车的速度

    {
        /* ftm_pwm_duty(FTM0, FTM_CH6, ste);
        if (ste > SteerMax)
            dir = -5;
        else if (ste < SteerMin)
            dir = 5;
        ste += dir;*/
        GetRightMotorPules = RightMotorPulseAccumulate; //保持好这次测得的总数，10ms一共测得的脉冲
        GetLeftMotorPules  = -LeftMotorPulseAccumulate;

        TimerCnt8ms = 0; //清零

        RightMotorPulseAccumulate = 0; //累加值清零，
        LeftMotorPulseAccumulate  = 0;
        if (LK_jishi != 2000 && getSwitch(motorSW))
        {
            if (LK_jishi_flag == 0) //准备延迟检测起跑线
            {

                LK_jishi++;
            }
            if (LK_jishi == 300) //起步延时1.5秒
            {
                go = 1; //小车前进
            }
            if (LK_jishi >= 800) //延迟2000*5ms后（10s）检测起跑线
            {
                LK_jishi_flag = 1;
                LK_jishi      = 2000;
            }
        }
        else
        {
            if (!dis_AD_val[0] && !dis_AD_val[1] && !dis_AD_val[2])
            {

                if (runrunrun > 120)
                {
                    go = 0;
                }
                else
                {
                    runrunrun++;
                }
            }
            else
            {
                if (runrunrun > 0)
                    runrunrun--;
                if (!runrunrun)
                    go = 1;
            }
        }
    }

    enable_irq(PIT0_IRQn); //使能中断
}

#endif

#if 1
void GetTargetSpeed(void)
{

    if (1) //二号拨码开关不拨上去,动态速度
    {

        LastSpeedDropRow = SpeedDropRow;

        SpeedDropRow = (int)(BlackEndM);

        if (SpeedDropRow >= 55)
            SpeedDropRow = 55;

        else if (SpeedDropRow < 35)
            SpeedDropRow = 35; //急弯减速很好

        //SpeedSet=(int)(0.05*(SpeedDropRow*SpeedDropRow)-(ABS(Error*Error)*0.02));//0.045,0.020//0.048,0.022
        SpeedSet = (int)(1.2 * SpeedDropRow + 10 * (SpeedDropRow - LastSpeedDropRow) - (0.3 * ABS(Error) + 1.0 * (Error - LastError)));

        if (SpeedSet <= 35)
            SpeedSet = 35;
        if (SpeedSet >= 55)
            SpeedSet = 55;

        if (ABS(Error) >= 10) //偏差大于某个值才进行差速
        {
            Differential_P = (float)(30.0 / ((60 - (LastLine + 4)) * (60 - (LastLine + 4))));      //30
            LSpeedSet      = (int32)(SpeedSet - (Differential_P * Error * SpeedSet / errorspeed)); //左轮差速
            if (LSpeedSet <= 5)
                LSpeedSet = 5;
            if (LSpeedSet >= 110)
                LSpeedSet = 110;
            RSpeedSet = (int32)(SpeedSet + (Differential_P * Error * SpeedSet / errorspeed)); //右轮差速
            if (RSpeedSet <= 5)
                RSpeedSet = 5;
            if (RSpeedSet >= 110)
                RSpeedSet = 110;
        }
        else
        {
            LSpeedSet = SpeedSet;
            RSpeedSet = SpeedSet;
        }
        LSpeedSet = (LSpeedSet >> 1) * MySpeedSet;
        RSpeedSet = (RSpeedSet >> 1) * MySpeedSet;
        if (!go || star_lineflag)
        {
            LSpeedSet = 0;
            RSpeedSet = 0;
        }
    }

    else if (0) //二号拨码开关往上波
    {
        SpeedSet = 210;
        SpeedP   = 40.0;   //50.0;40
        SpeedI   = 0.0009; //16.0;50,0.0006
        SpeedD   = 10.0;   //1.3,10.0

        if (ABS(Error) >= 10)
        {
            //Differential_P=(float)(30.0/((60-(AvaliableLines+10))*(60-(AvaliableLines+10))));
            SpeedSet = 40;

            Differential_P = 0.0180;                                                  //调差速，调太大会跳轮
            LSpeedSet      = (int32)(SpeedSet - (Differential_P * Error * SpeedSet)); //左轮差速
            if (LSpeedSet <= 100)
                LSpeedSet = 100;
            if (LSpeedSet >= 300)
                LSpeedSet = 300;
            RSpeedSet = (int32)(SpeedSet + (Differential_P * Error * SpeedSet)); //右轮差速
            if (RSpeedSet <= 100)
                RSpeedSet = 100;
            if (RSpeedSet >= 300)
                RSpeedSet = 300;
        }
        else
        {
            LSpeedSet = SpeedSet;
            RSpeedSet = SpeedSet;
        }
    }
}

#endif

//计算速度偏差

void CalculateMotorSpeedError(float LeftMotorTarget, float RightMotorTarget)
{
    SpeedPerErrorL  = SpeedLastErrorL;                     //上上次
    SpeedLastErrorL = SpeedErrorL;                         //上次
    SpeedErrorL     = LeftMotorTarget - GetLeftMotorPules; //这次

    SpeedPerErrorR  = SpeedLastErrorR;
    SpeedLastErrorR = SpeedErrorR;
    SpeedErrorR     = RightMotorTarget - GetRightMotorPules;
}

//增量式PID控制算法

void MotorControl(void)
{
    GetTargetSpeed();
    CalculateMotorSpeedError(LSpeedSet, RSpeedSet); //设定目标速度计算偏差
    MotorPwmR += (SpeedP + SpeedI + SpeedD) * SpeedErrorR - (SpeedP + 2 * SpeedD) * SpeedLastErrorR + SpeedD * SpeedPerErrorR;
    MotorPwmRight = (int)(MotorPwmR);
    MotorPwmL += (SpeedP + SpeedI + SpeedD) * SpeedErrorL - (SpeedP + 2 * SpeedD) * SpeedLastErrorL + SpeedD * SpeedPerErrorL;
    MotorPwmLeft = (int)(MotorPwmL);
    if (MotorPwmLeft <= -1390)
        MotorPwmLeft = -1390;
    else if (MotorPwmLeft >= 2990)
        MotorPwmLeft = 2990;

    if (MotorPwmRight <= -1390)
        MotorPwmRight = -1390;
    else if (MotorPwmRight >= 2990)
        MotorPwmRight = 2990;

    if (MotorPwmLeft > 0)
    {
        ftm_pwm_duty(FTM3, FTM_CH0, MotorPwmLeft * 2);
        ftm_pwm_duty(FTM3, FTM_CH1, 0);
    }
    else
    {
        ftm_pwm_duty(FTM3, FTM_CH0, 0);
        ftm_pwm_duty(FTM3, FTM_CH1, -MotorPwmLeft * 2);
    }
    if (MotorPwmRight > 0)
    {
        ftm_pwm_duty(FTM3, FTM_CH2, MotorPwmRight * 2); //PTC2,右电机
        ftm_pwm_duty(FTM3, FTM_CH3, 0);                 //PTC2,右电机
    }
    else
    {
        ftm_pwm_duty(FTM3, FTM_CH2, 0);                  //PTC2,右电机
        ftm_pwm_duty(FTM3, FTM_CH3, -MotorPwmRight * 2); //PTC2,右电机
    }
}
