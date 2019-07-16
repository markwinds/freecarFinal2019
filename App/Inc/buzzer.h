#ifndef _BUZZER_H_
#define _BUZZER_H_

#include "common.h"
#include "include.h"

typedef enum
{
    T2L33   = 0,
    T1L2    = 1,
    T0L0    = 2,
    T4L4242 = 3,
    T1L5    = 4
} SoundType;

extern int buzzer_timer_flag;
extern int sound_choose;
extern int sound_size;
extern int sound_num;

extern void initBuzzer();
extern void openBuzzer();
extern void closeBuzzer();
extern void turnBuzzer();
extern void tellMeRoadType(SoundType sound_type);
extern void checkBuzzerShouldSpeak();

#endif //_BUZZER_H_