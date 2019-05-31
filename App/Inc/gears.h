#ifndef _GEARS_H_
#define _GEARS_H_

enum swich_state
{
  NO,
  OFF,
};

enum gears
{
  low_grade,//低档
  medium_grade,//中档
   top_grade,//高档
};

extern enum gears gears_speed;
extern enum swich_state display_switch,send_switch;

extern void gears_set_init();
#endif