#ifndef _GEARS_H_
#define _GEARS_H_

enum swich_state
{
  NO,
  OFF,
};

enum gears
{
  low_grade,//�͵�
  medium_grade,//�е�
   top_grade,//�ߵ�
};

extern enum gears gears_speed;
extern enum swich_state display_switch,send_switch;

extern void gears_set_init();
#endif