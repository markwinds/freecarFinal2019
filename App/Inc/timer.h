#ifndef _TIMER_H
#define _TIMER_H

#include "common.h"
#include "include.h"

extern int timer_busy;

extern void initTimerForFlag();
extern void setFlagInTimerLongMust(int* p_flag, int time_long, int vaule);
extern void manageTimerFlag();
extern int  setFlagInTimerLongCheck(int* p_flag, int time_long, int vaule);

#endif //_TIMER_H