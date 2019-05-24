#ifndef __MOTOR_H__
#define __MOTOR_H__

#define  MotorControlPeriod   8     //�������ֵ��������
#define  SteetControlPeriod   20    //�����������

extern  int32 GetRightMotorPules;
extern  int32 GetLeftMotorPules;
extern  float OutData[4];
extern  float Error;
extern  float AvaliableLines;
extern  int   BlackEndM ; 
extern  int   MiddleLine[RowMax+1];//����
extern  int   BlackEndMR;
extern  int   BlackEndML;
extern float  SteerPwmAdd;
extern int ChangeFlag;
extern int SpeedDropRow;
extern  int32 SpeedSet;
extern void MotorInit(void);

void CalculateMotorSpeedError(float LeftMotorTarget,float RightMotorTarget);

extern void MotorControl(void);

void GetTargetSpeed(void);


#endif