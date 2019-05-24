#ifndef __FINDTRACK_H__
#define __FINDTRACK_H__

/*** �궨�� ****/ 

#define Black_Point 0
#define White_Point 255
#define RowMax	    60	  //����
#define ColumnMax	80	  //����


/*** �ⲿ������������ ****/  


extern uint8  img[CAMERA_H][CAMERA_W];//����洢����ͼ�������,CAMERA_H(59-0),CAMERA_W(0-79)

extern uint8 imgbuff[CAMERA_SIZE];
extern int   AllLose;
extern int BlackEndL;
extern int BlackEndM;
extern int BlackEndR;
extern  int   RightEdge[RowMax+1];
extern  int   LeftEdge[RowMax+1];
extern  int   MiddleLine[RowMax+1];
extern int   LastLine;
extern int   Width[RowMax+1];
extern int   LeftLose;
extern int   RightLose ;
extern unsigned char LoopFlag;

extern  unsigned char LoopRightControlFlag;
extern  unsigned char LoopLeftControlFlag;
extern  int MotivateLoopDlayFlagR;
extern  int MotivateLoopDlayFlagL;
extern  unsigned char MotivateLoopDlayFlagLOK;
extern  unsigned char MotivateLoopDlayFlagROK;
extern  unsigned char OpenLoopExitRepairFlagL;
extern  unsigned char OpenLoopExitRepairFlagR;
extern  unsigned char RightInflectionPointFlag;
extern  unsigned char LeftInflectionPointFlag;

/*** �������� ****/    

void FindInflectionPoint();
extern void SearchCenterBlackline(void);
extern void GetBlackEndParam();
extern void DrawBlackline();
void MidValeFilte(void);
void SendImage();
int SortRank(int x,int y,int z);
extern void FindLoopExit();
extern void SetInitVal();
extern  void LoopControl();
extern  void LoopRepair();

extern  void LoopExitRepair();
#endif