#ifndef __STEER_H__
#define __SYEER_H__

#define S3010_FTM FTM1   //FTM1模块
#define S3010_CH FTM_CH0 //通道号PTA8
#define S3010_HZ 50      //舵机的频率

extern int judge_road_black_num;
extern int judge_road_black_state;
extern int out_road;

#define SteerMax 965   //舵机最大值
#define SteerMin 765   //舵机最小值
#define SteerMidle 852 //舵机中值

extern float  LastError;
extern float  Error;
extern float  KD;     //舵机方向比例系数，影响舵机的打角范围
extern float  BasicP; //10//7.5//舵机方向微分系数,影响舵机的打角反应
extern int32  eleSpeed;
extern uint8  breakLoadFlag;
extern uint16 breakLoadCont;
extern uint32 LastSteerSwm;
extern int    ADC_updata;
extern int    actualSpeed;
extern int    eleP;
extern float  hli, hri, hlo, hro;
void          SteerInit(void); //舵机初始化
void          CalculateError(void);
extern void   SteerControl(void);

#endif
