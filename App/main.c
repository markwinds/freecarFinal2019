#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE];            //����洢����ͼ�������
uint8 img[CAMERA_H][CAMERA_W];   

//��������
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void HardWare_Init(void);
void PIT0_IRQHandler();
void PORTE_IRQHandler();
void set_vector_handler(VECTORn_t , void pfunc_handler(void));      //�����жϺ������ж���������



void HardWare_Init(void) 
{
        DisableInterrupts;
        
        camera_init(imgbuff);//����ͷ��ʼ��
        set_vector_handler(PORTA_VECTORn , PORTA_IRQHandler);   //���� PORTA ���жϷ�����Ϊ PORTA_IRQHandler
        set_vector_handler(DMA0_VECTORn , DMA0_IRQHandler);     //���� DMA0 ���жϷ�����Ϊ PORTA_IRQHandler

#if 1										      
	ftm_quad_init(FTM2);  //FTM2  PTA10 �� PTA11 �����������ʼ��
        ftm_quad_clean(FTM2);//�����Ĵ�������
        lptmr_pulse_init(LPT0_ALT2,0xffff,LPT_Rising); //PTA19
        lptmr_pulse_clean();
#endif

#if 1										
	pit_init_ms(PIT0,1);//PIT��ʼ��  1ms                  
	set_vector_handler(PIT0_VECTORn,PIT0_IRQHandler);   // �����жϸ�λ�������ж�������
	enable_irq(PIT0_IRQn);                  // ʹ��PIT0�ж�
  
#endif    
        
        
       NVIC_SetPriorityGrouping(4);            //�������ȼ�����,4bit ��ռ���ȼ�,û��������
       NVIC_SetPriority(PORTA_IRQn , 0);          //�������ȼ�  ͼ���ж�  PORTA
       NVIC_SetPriority(DMA0_IRQn  , 1);          //�������ȼ�  ͼ��  DMA0
       NVIC_SetPriority(PIT0_IRQn  , 2);          //�������ȼ�  ������   PIT0 
    
#if    OpenDialSwitch
    
        DialSwitchInit();//���뿪�س�ʼ��
        
#endif         
        gpio_init (PTB19, GPO, 0);//��������ʼ��
         
        SteerInit();//�����ʼ��
     
        MotorInit();//�����ʼ��
        
        if(DialSwitch_1)
        {
          LCD_Init();// LCD_ST7735R Һ����ʼ��,���ܳ�ʼ��������ͷǰ��
          
        }
        EnableInterrupts;//ʹ�����ж�
    
}
    
    
  void  main(void)
{  
  
    HardWare_Init();   
   
    while(1)
      
     { 
              camera_get_img();//����ʱ13.4ms��ͼ��ɼ�
              img_extract(img,imgbuff);
              GetBlackEndParam();//��ȡ���߽�ֹ�� 
              SearchCenterBlackline();
              LoopFlag=0;//��·���־
               
             if(MotivateLoopDlayFlagL==0&&MotivateLoopDlayFlagR==0&&CloseLoopFlag==0)//���˻�������ʮ�֣��ص�Բ������
             {
               
          
              FindInflectionPoint();//Ѱ�ҹյ�
              FindLoopExit();
              LoopControl();
              LoopRepair();

             }       

            if(MotivateLoopDlayFlagL||MotivateLoopDlayFlagR)
             {
                LoopExitRepair();//���ڴ���
             }
              if(LoopRightControlFlag==0&&LoopLeftControlFlag==0&&MotivateLoopDlayFlagL==0&&MotivateLoopDlayFlagR==0&&LoopFlag==0)
              {
                NormalCrossConduct();
               
              }
             
# if ObstacleOpen  //�������Ҫ���ϰ���������궨����0����
             
               RecognitionObstacle();
#endif             
             
               SteerControl();
               MotorControl();
               if(DialSwitch_1)
               {
             
                 LCDDisplay();//Һ����ʾ
                 
               }
         }
    
}

void PORTA_IRQHandler()
{
    uint8  n;    //���ź�
    uint32 flag;

    while(!PORTA_ISFR);
    flag = PORTA_ISFR;
    PORTA_ISFR  = ~0;                                   //���жϱ�־λ

    n = 29;                                             //���ж�
    if(flag & (1 << n))                                 //PTA29�����ж�
    {
        camera_vsync();
    }
#if ( CAMERA_USE_HREF == 1 )                            //ʹ�����ж�
    n = 28;
    if(flag & (1 << n))                                 //PTA28�����ж�
    {
        camera_href();
    }
#endif


}


void DMA0_IRQHandler()
{
    camera_dma();
}





