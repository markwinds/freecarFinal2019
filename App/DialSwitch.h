#ifndef __DIALSWITCH_H__
#define __DIALSWITCH_H__

#include "common.h"
#include "include.h"

#define OpenDialSwitch 1  //启用拨码开关功能

#define  DialSwitch_1    gpio_get(PTA10)
#define  DialSwitch_2    gpio_get(PTA12)
#define  DialSwitch_3    gpio_get(PTA14)
#define  DialSwitch_4    gpio_get(PTA16)

extern void DialSwitchInit(void);


#endif