#include "common.h"
#include "include.h"
#include "steer.h"

/*********define for SteerControl**********/

float  KP          = 0.0; //舵机方向比例系数，影响舵机的打角范围
float  KD          = 5.5; //10//7.5//16.4//舵机方向微分系数,影响舵机的打角反应
float  SteerPwmAdd = 0.0; //舵机pwm增量
float  Error;             //偏差值
float  LastError;         //上次的偏差
float  WeightSum       = 0;
float  CenterMeanValue = 0;
float  CenterSum       = 0;
float  J               = 0.05;         //调节p和偏差的关系，越大，作用越强
float  BasicP          = 2.4;          //基本的P值
uint32 SteerPwm = 0, LastSteerSwm = 0; //舵机的pwm值和上次舵机的pwm值

int judge_road_black_num   = 2;
int judge_road_black_state = 1;
int out_road = 0, eleP = 85;

uint16 breakLoadCont;

//加权平均，权值的选取
#if 1
float Weight[60] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, //0-9行，基本用不到
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    2, //0-19行，基本用不到

    1.8,
    1.8,
    1.8,
    1.8,
    1.8,
    1.3,
    1.3,
    1.3,
    1.3,
    1.3, //20-29行

    //1.05,1.05,1.05,1.05,1.05,2,2,2,2,2,

    2,
    2,
    2,
    2,
    2,
    2.0,
    2.0,
    2.5,
    2.5,
    2.5, //30-39行

    2.1,
    2.1,
    2.1,
    2.1,
    2.1,
    2,
    1,
    2,
    1,
    2, //40-49行
    1,
    0,
    0,
    0,
    0,
}; //最近十行*///不压线，用于弯道
#endif

#if 0

float Weight[60]={ 
                     0,0,0,0,0,0,0,0,0,0,              //0-9行，基本用不到
                    
                     0,0,0,0,0,1.5,1,1.5,1,             //0-19行，基本用不到
                  
                     2.5,1,2.2,1,2.5,1,2,2,1,2,            //20-29行
                    
                     1,3,2,2,1,2,1,2,2,1,              //30-39行

                     3,1,2,3,3,2,3,1,1,             //40-49行
                   
                     1,1,1,1,1,2,1,1,0,0,};          //最近十行

#endif

/****************************************************** 
 *函数名：SteerInit
 *
 *功能：舵机初始化
 * 
 * 入口参数：无
 * 
 *返回参数：无
 *
 * 作者：XGL
 * 
 * 日期：2016-3-01(已测试)
 *******************************************************/

void SteerInit(void) //舵机初始化

{

    ftm_pwm_init(FTM0, FTM_CH6, 50, SteerMidle);
}

#if 1

/****************************************************** 
 *函数名：CalculateError
 *
 *功能：计算误差
 * 
 * 入口参数：无
 * 
 *返回参数：无
 *
 * 作者：XGL
 * 
 * 日期：2016-3-01(已测试)
 *******************************************************/
float eError;
uint8 espped        = 0;
uint8 breakLoadFlag = 0, breakcout = 0;
int32 eleSpeed = 17;
void  CalculateError(void)
{
    //右是负的，左是正的
    int i;

    CenterSum = 0;

    CenterMeanValue = 0;

    WeightSum = 0;
    LastError = Error;
    for (i = 57; i > LastLine; i--)
    {
        CenterSum += MiddleLine[i] * Weight[i];

        WeightSum += Weight[i];
    }

    if (WeightSum > 1)

    {
        CenterMeanValue = (CenterSum / WeightSum); //算出加权平均后中线的值
    }

    //eError = ((60 - disgy_AD_val[2]) / 8.0) * (dis_AD_val[0] - dis_AD_val[1]);
    //eError = getSteerPwmFromADCError();
    Error = (40 - CenterMeanValue); // 一场图像偏差值
    if (hhhar[hhhead] == 1 && (LK_jishi_flag || getSwitch(mainShowSW)) && !breakLoadFlag && !circluFlag && (!hamperFlag || hamperFlag == 5) && (dis_AD_val[0] + dis_AD_val[1] + dis_AD_val[2]) > 40)
    {
        if ((BlackEndL < 20 && BlackEndR < 20) || BlackEndM < 10 || (lSlope > 4 && rSlope > 4 && lSlope < 20 && rSlope < 20))
        {
            breakcout++;
            if (breakcout > 3)
            {
                breakLoadFlag = 1;
                breakLoadCont = 0;
                breakcout     = 0;
                //tellMeRoadType(T2L11);
            }
            else if (lSlope > 4 && rSlope > 4 && lSlope < 20 && rSlope < 20)
            {
                breakcout += 3;
            }
        }
        else
            breakcout--;
    }
    else if (breakLoadFlag) //如果是电磁跑的断路
    {
        Error = 1000.0 * (disgy_AD_val[0] - disgy_AD_val[1]) / (disgy_AD_val[0] + disgy_AD_val[1] + 1) / (disgy_AD_val[2] + 20);
        if (BlackEndM > 30 || (BlackEndM && (dis_AD_val[0] + dis_AD_val[1] + dis_AD_val[2]) < 10)) //如果摄像头得到有用的图像
        {
            breakLoadFlag = 0;
            if (breakLoadCont > 188)
            {
                hhhead++;
            }
            MySpeedSet = actualSpeed;
        }
    }

    switch (circluFlag)
    {
        int16 ttem;
        case 2:
            // if (Error < aMark)
            // {
            //     Error = aMark;
            // }
            // else if (Error > 8)
            // {
            //     circluFlag = 4;
            // }
            break;
        case 3:
            // if (Error > aMark)
            // {
            //     Error = aMark;
            // }
            // else if (Error < -8)
            // {
            //     circluFlag = 5;
            // }
            break;
        case 4:
            if (Error < 1)
            {
                Error = 11;
            }
            break;
        case 5:
            if (Error > -1)
            {
                Error = -11;
            }
            break;
        case 6:
            // if (Error < -1)
            // {

            //     circluFlag = 8;
            // }

            Error -= Error / 3;
            break;
        case 7:
            // if (Error > 1)
            // {

            //     circluFlag = 9;
            // }
            Error -= Error / 3;
            break;
        case 8:
            // if (Error < 46 - BlackEndM)
            // {
            //     Error = (46 - BlackEndM);
            // }
            ttem = (43 - BlackEndM) * (43 - BlackEndM) / 7;
            if (Error < ttem)
            {
                Error = ttem;
            }
            break;
        case 9:
            // if (Error > BlackEndM - 46)
            // {
            //     Error = BlackEndM - 46;
            // }
            ttem = (43 - BlackEndM) * (43 - BlackEndM) / 7;
            if (Error > -ttem)
            {
                Error = -ttem;
            }
            break;
        case 10:
            Error /= 2;
            break;
        case 11:
            Error /= 2;
            break;
    }
    if (hamperFlag)
    {
        switch (hamperFlag)
        {
            case 1:

                Error = 11;

                break;
            case 2:
                Error = -10;
                break;
            case 3:
                break;
            case 4:
                Error = -7;
                break;
        }
        if (hhhar[hhhead] == 5 && hamperFlag < 5)
        {
            Error = -Error;
        }
    }

    /* 
    if (circluFlag == 2)
    {
        //Error += ((dis_AD_val[1] + dis_AD_val[0]) / 12);
        if (Error < 6)
        {
            Error = 11;
        }
        // else if (Error < 5.5)
        // {
        //     Error = 8;
        // }
        else
        {
            circluFlag = 4;
        }
    }
    else if (circluFlag == 3)
    {
        // Error -= ((dis_AD_val[1] + dis_AD_val[0]) / 13);
        if (Error > -6)
        {
            Error = -11;
        }
        // else if (Error > -5.5)
        // {
        //     Error = -8;
        // }
        else
        {
            circluFlag = 5;
        }
    }
    else if (circluFlag == 6)
    {

        if (Error < 4 && BlackEndM < 40)
        {
            Error = 15;
        }
    }
    else if (circluFlag == 7)
    {
        if (Error > -4 && BlackEndM < 40)
        {
            Error = -16;
        }
    }
    else if (circluFlag == 8)
    {
        Error /= 2;
    }
    else if (circluFlag == 9)
    {
        Error /= 2;
    }
    */
    if (!LK_jishi_flag && !getSwitch(mainShowSW))
    {
        if (Error >= 6.0) //偏差限幅

            Error = 6.0;

        if (Error <= -6.0)

            Error = -6.0;
    }
    if (Error >= 30.0) //偏差限幅

        Error = 30.0;

    if (Error <= -30.0)

        Error = -30.0;
    if (breakLoadFlag)
    {
        KP = Error * Error / eleP + 1;
    }
    else
    {
        KP = BasicP + (Error * Error) * J; //动态二次p模型
    }

    if (KP >= 12)
        KP = 12; //p值限幅
}

#endif

/****************************************************** 
 *函数名：SteerControl
 *
 *功能：舵机控制
 * 
 * 入口参数：无
 * 
 *返回参数：无
 *
 * 作者：XGL
 * 
 * 日期：2016-3-01(已测试)
 *******************************************************/

void SteerControl(void)
{

    CalculateError();

    SteerPwmAdd = (KP * Error) + KP * KD * (Error - LastError); //舵机的pd控制器

    if (SteerPwmAdd >= 120)

        SteerPwmAdd = 120;

    if (SteerPwmAdd <= -120)

        SteerPwmAdd = -120;

    SteerPwm = (uint32)(SteerPwmAdd * 5 / 6 + SteerMidle);

    if (SteerPwm >= SteerMax) //限幅

        SteerPwm = SteerMax;

    if (SteerPwm <= SteerMin)

        SteerPwm = SteerMin;

    ftm_pwm_duty(FTM0, FTM_CH6, SteerPwm); //舵机pwm更新

    LastSteerSwm = SteerPwm; //记录pwm值
}
