#ifndef __MOTOR_STEER_H__
#define __MOTOR_STEER_H__

#include "common.h"
#include "include.h"

#define STEER_MID 868
#define STEER_MAX_LEFT 965
#define STEER_MAX_RIGHT 765
// #define STEER_MID 652
// #define STEER_MAX_LEFT 730
// #define STEER_MAX_RIGHT 570
// #define SteerMax 965   //舵机最大值
// #define SteerMin 765   //舵机最小值
// #define SteerMidle 868 //舵机中值

extern int steerTable[101]; //将方向100等分
extern int speed_different;

extern void initMotorSteer();
extern void setSteer(int dir);
extern void setSpeedLeft(int speed);
extern void setSpeedRight(int speed);

#endif //__MOTOR_STEER_H__