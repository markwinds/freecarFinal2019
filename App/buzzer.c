#include "common.h"
#include "include.h"

int buzzer_timer_flag = 0;
int sound_choose      = 0;
int sound_size        = 0;
int sound_num         = 0;

int buzzer_music[][10] = {
    { 300, 100, 300, 0 },
    { 200, 0 },
    { 0 },
    { 400, 100, 200, 100, 400, 100, 200, 0 },
    { 500, 0 }
};

void initBuzzer()
{
    gpio_init(PTD2, GPO, 0);
}

void openBuzzer()
{
    gpio_set(PTD2, 1);
}

void closeBuzzer()
{
    gpio_set(PTD2, 0);
}

void turnBuzzer()
{
    gpio_turn(PTD2);
}

void tellMeRoadType(SoundType sound_type)
{
    if (sound_type == T0L0 || timer_busy)
    {
        return;
    }

    int* p       = buzzer_music[sound_type];
    sound_choose = sound_type;
    sound_size   = 0;
    sound_num    = 0;
    while (*p++ != 0)
    {
        sound_size++;
    }
    sound_num = sound_size;
}

void checkBuzzerShouldSpeak()
{
    if (sound_num) //如果一段声音还没放完
    {
        if (setFlagInTimerLongCheck(&buzzer_timer_flag, buzzer_music[sound_choose][sound_size - sound_num], (sound_size - sound_num) % 2 == 0 ? 1 : 0))
        {
            sound_num--;
        }
    }
}