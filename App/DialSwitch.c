#include "common.h"
#include "include.h"

#if OpenDialSwitch

void DialSwitchInit(void)
{
    //往上波了是0，不波的时候是1
    // gpio_init(PTA10, GPI, 0); //1
    gpio_init(PTA12, GPI, 0); //2
    gpio_init(PTA14, GPI, 0); //3
    gpio_init(PTA16, GPI, 0); //4

    gpio_init(PTD10, GPI, 0); //5上
    gpio_init(PTD11, GPI, 0); //6左
    gpio_init(PTD12, GPI, 0); //7中
    gpio_init(PTD13, GPI, 0); //8右
    gpio_init(PTD14, GPI, 0); //9下
}

#endif