#ifndef __TRACKTYPE_H__
#define __TRACKTYPE_H__

#include "common.h"
#include "include.h"

#define ObstacleOpen  1//���ϰ�

extern unsigned char  CrossFlag ;//ʮ�ֱ�־
extern unsigned char CloseLoopFlag;



void IsOrNotCrossLoop();
extern void RecognitionObstacle(void);
extern void ObstacleRepair();
extern void NormalCrossConduct(void);

#endif