#include "common.h"
#include "include.h"

/*********define for CrossConduct**********/
unsigned char  CrossFlag  = 0;//ʮ�ֱ�־
unsigned char  LastCrossFlag  = 0;
unsigned char  LastLastCrossFlag  = 0;
unsigned char  CrossNumberFalg=0;
unsigned char  CloseLoopFlag=0;

/*********define for RecognitionObstacle**********/
unsigned char  ObstacleLfetRowStart=0;
unsigned char  ObstacleLfetColStart=0;
unsigned char  ObstacleLfetOkFlag=0;
unsigned char  ObstacleRightRowStart=0;
unsigned char  ObstacleRightColStart=0;
unsigned char  ObstacleRightOkFlag=0;


//��ʮ�ֽ���ʶ��ͽ������⴦��      
void NormalCrossConduct(void)
{       
     unsigned char i;
 
     LastLastCrossFlag=LastCrossFlag;//��¼���ϴ��Ƿ���ʮ��
     LastCrossFlag=CrossFlag;//��¼��һ���Ƿ���ʮ��
     CrossFlag=0;//����
     
     
   if((AllLose>=15))//����ͬʱ���� 
      {
             CrossFlag=1;//���ʮ��    
      }
      if(CrossFlag)     
       { 
         
         
       if((BlackEndM>=BlackEndL)&&(BlackEndM>=BlackEndR))//�м������� 
       {
         for(i=58;i>20;i--)     
         {
          MiddleLine[i]=40;//ֱ��    
         }   
       }            
       else if((BlackEndL>BlackEndM)&&(BlackEndL>BlackEndR))//������߰׵���࣬����ƫ���ˣ�������  
       {
          for(i=58;i>20;i--)    
          {        
                  MiddleLine[i]=30; 
          }  
       }          
       else if((BlackEndR>BlackEndM)&&(BlackEndR>BlackEndL))//�����ұ߰׵���࣬����ƫ���ˣ�������
       {
         for(i=58;i>20;i--)     
         { 
             
              MiddleLine[i]=50;    
          }  
         }
       }
     
   
     IsOrNotCrossLoop();

}

//�ж��ǲ��ǽ�ȥ��ʮ�ֻػ����������Ҫ�رջ���ʶ�𣬷���б��ʮ���ǣ������гɻ���

void IsOrNotCrossLoop()
{
  
  static char CrossLoopErrorNum=0;
  static unsigned char CloseLoopNum=0;
  static unsigned char CrossErrorClose=0;//��¼���û������ƫ�����10
  unsigned char  ErrorCloseFlag=0;
  
  ErrorCloseFlag=0;
 
   if(LastLastCrossFlag&&LastCrossFlag&&CrossFlag==0)//ǰ������ʮ�֣����β���ʮ��
    {
      
      CrossNumberFalg++;//��¼�������Ѿ�������ʮ��
      
    
      
    }
   
   if(CrossNumberFalg==1)
   {
       CrossErrorClose++;
       
       if(CrossErrorClose>8)
       {
           ErrorCloseFlag=1;
           CrossErrorClose=0;
       
       }
     
          if(ABS(Error)>=10&&!ErrorCloseFlag&&LastLine>=20)//����ʮ�ֽ�ֱ�����ر��˻���
          {
              CrossLoopErrorNum++;//������¼ƫ����10
          
          }
          else
          {
            CrossLoopErrorNum=0;
          
          }
          
          
           if(CrossLoopErrorNum>=4&&CloseLoopFlag==0)//����4��ƫ�����10
         {
           CloseLoopFlag=1;//�ص�Բ������
           CrossLoopErrorNum=0;
        }
   }
   
  
   
   if(CloseLoopFlag)//��ʼ����ر��˻��������ʱ��
   {
       
      CloseLoopNum++;
      gpio_set   (PTB19, 1);

      if(CloseLoopNum>=100)//���ʱ��̫����û����ʮ�ֳ��ڣ�ǿ�ƴ򿪻���
      {
         CloseLoopFlag=0;
         CloseLoopNum=0;
         LastLastCrossFlag=0;
         LastCrossFlag=0;
         CrossNumberFalg=0;//��Բ������
         gpio_set   (PTB19, 0);
      }
      
   }
   
   if(CrossNumberFalg>=2&&CloseLoopFlag)
   {
     CloseLoopFlag=0;//��Բ������
     LastLastCrossFlag=0;
     LastCrossFlag=0;
     CrossNumberFalg=0;
     CloseLoopNum=0;
     gpio_set   (PTB19, 0);
   }
}


#if ObstacleOpen  //����������ϰ�ʶ��

//�ϰ�ʶ��

void RecognitionObstacle(void)
{
  int i,j;
  static unsigned char ObstacleLfetRowNum=0;
  static unsigned char ObstacleLfetNum=0;
  unsigned char  ObstacleLeftFlag=0;
  unsigned char ObstacleLeftLFlag=0;
  static unsigned char ScanColStartLeft=0;//ɨ�����ʼ��
  static unsigned char ScanColEndLeft=0;//ɨ�����ֹ��
  
  static unsigned char ObstacleRightRowNum=0;
  static unsigned char ObstacleRightNum=0;
  unsigned char   ObstacleRightFlag=0;
  unsigned char  ObstacleRightRFlag=0;
  static unsigned char ScanColStartRight=0;//ɨ�����ʼ��
  static unsigned char ScanColEndRight=0;//ɨ�����ֹ��
  
  
  ScanColStartLeft=0;
  ScanColEndLeft=0;
  ObstacleLeftFlag=0;
  ObstacleLeftLFlag=0;
  ObstacleLfetRowStart=0;
  ObstacleLfetColStart=0;
  ObstacleLfetRowNum=0;
  
  ScanColStartRight=0;
  ScanColEndRight=0;
  ObstacleRightFlag=0;
  ObstacleRightRFlag=0;
  ObstacleRightRowStart=0;
  ObstacleRightColStart=0;
  ObstacleRightRowNum=0;
     //ObstacleLfetOkFlag=0;
  
  if(ABS(Error)<=6&&AllLose<=1&&LastLine<=12&&!CrossFlag&&LoopRightControlFlag==0&&LoopLeftControlFlag==0&&LeftLose<4&&RightLose<=4)
  
  {
    for(i=58;i>20;i--)
    {
      if(LeftEdge[i]!=0)//������е�����ߴ���
      {
        if(ABS(LeftEdge[i+1]-LeftEdge[i])>=3)//��߽�����
        {
          
          if(LeftEdge[i+1]!=0)
          {
            ScanColStartLeft=LeftEdge[i+1];//��ǰһ����Ϊɨ�����ʼ��
          
          }
          
          else
          {
          
            ScanColStartLeft=LeftEdge[56];
          
          }
          
        }
        
        else
        {
          
          ScanColStartLeft=LeftEdge[i];
        
        }
      
      }
      
      else//�����������߲�����
      {
         ScanColStartLeft=LeftEdge[i+1];//��ǰһ����Ϊɨ�����ʼ��
      }
      
      
        ScanColEndLeft=MiddleLine[i];
      
       for(j=ScanColStartLeft;j<ScanColEndLeft;j++)//�����Ѱ���ϰ���
       {
            if(img[i][j]==White_Point&&img[i][j+1]==Black_Point)//�ҵ�������
            {
               ObstacleLeftFlag=1;
              
               break;

            }
 
       }
       
       unsigned char jj=j+1;
       
       if(ObstacleLeftFlag)
       {
       
         while(jj<ScanColEndLeft)
         {
            if(img[i][jj]==Black_Point&&img[i][jj+1]==White_Point)//�ҵ���һ������
            {
                ObstacleLeftLFlag=1;
                
                break;//�˳�while
             }
            
            jj++;
          
          }
       
         
       }
     
     if(ObstacleLeftLFlag&&ObstacleLeftFlag)
     {
      
        ObstacleLfetRowNum++;
        
        if(ObstacleLfetRowNum==1)
        {
           ObstacleLfetRowStart=i;
           ObstacleLfetColStart=j+1;
           //ObstacleLfetRowNum=0;
        }
     }
     
     
    }
 
    if(ObstacleLfetRowNum>=4&&ObstacleLeftLFlag&&ObstacleLeftFlag)//����4�������������϶�����������ϰ�
    {
      
        ObstacleLfetNum++;
       
       if(ObstacleLfetNum>=3)
       {
         
        ObstacleLfetOkFlag=1;
        ObstacleLfetNum=0;
        ObstacleLfetRowNum=0;
        gpio_set (PTB19,1);
       
       }
    }
    else
    {
      // ObstacleLfetOkFlag=0;
       ObstacleLfetNum=0;
       gpio_set (PTB19, 0);
       
    }
    
  
    //�ұ��ϰ� 
    
  if(!ObstacleLfetOkFlag)  //���û���ҵ�����ϰ����Ѱ���ұ���û���ϰ���
  {
  
    
    for(i=58;i>20;i--)
    {
      if(RightEdge[i]!=80)//������е��ұ��ߴ���
      {
        if(ABS(RightEdge[i+1]-RightEdge[i])>=3)//�ұ߽�����
        {
          
          if(RightEdge[i+1]!=80)
          {
            ScanColStartRight=RightEdge[i+1];//��ǰһ����Ϊɨ�����ʼ��
          
          }
          
          else
          {
          
            ScanColStartRight=RightEdge[56];
          
          }
          
        }
        
        else
        {
          
          ScanColStartRight=RightEdge[i];
        
        }
      
      }
      
      else//�����������߲�����
      {
         ScanColStartRight=RightEdge[i+1];//��ǰһ����Ϊɨ�����ʼ��
      }
      
      
        ScanColEndRight=MiddleLine[i];
      
       for(j=ScanColStartRight;j>ScanColEndRight;j--)//�����Ѱ���ϰ���
       {
            if(img[i][j]==White_Point&&img[i][j-1]==Black_Point)//�ҵ�������
            {
               ObstacleRightFlag=1;
              
               break;

            }
 
       }
       
       unsigned char jj=j-1;
       
       if(ObstacleRightFlag)
       {
       
         while(jj>ScanColEndRight)
         {
            if(img[i][jj]==Black_Point&&img[i][jj-1]==White_Point)//�ҵ���һ������
            {
                ObstacleRightRFlag=1;
                
                break;//�˳�while
             }
            
            jj--;
          
          }
       
         
       }
     
     if(ObstacleRightFlag&&ObstacleRightRFlag)
     {
      
        ObstacleRightRowNum++;
        
        if(ObstacleRightRowNum==1)
        {
           ObstacleLfetRowStart=i;
           ObstacleLfetColStart=j+1;
           //ObstacleLfetRowNum=0;
        }
     }
     
     
    }
 
    if(ObstacleRightRowNum>=4&&ObstacleRightFlag&&ObstacleRightRFlag)//����4�������������϶�����������ϰ�
    {
      
        ObstacleRightNum++;
       
       if(ObstacleRightNum>=3)
       {
         
        ObstacleRightOkFlag=1;
        ObstacleRightNum=0;
        ObstacleRightRowNum=0;
        gpio_set (PTB19,1);
       
       }
    }
    else
    {
      // ObstacleLfetOkFlag=0;
       ObstacleRightNum=0;
       gpio_set (PTB19, 0);
       
    }
   
      
    }


  }
    
      


  ObstacleRepair();
  
}



//�ϰ�����ߴ���

void ObstacleRepair()
{
    int i;
    static unsigned char ObstacleLfetControlDelay=0;

   if(ObstacleLfetOkFlag&&!ObstacleRightOkFlag)
   {
      for(i=59;i>=LastLine;i--)
      {
      
        MiddleLine[i]=(int)(RightEdge[i]-Width[i]*0.2);

      }
      
      ObstacleLfetControlDelay++;
      
      if(ObstacleLfetControlDelay>=20)
      {
         ObstacleLfetControlDelay=0;
         ObstacleLfetOkFlag=0;
      }
      
   
   }
   
   if(ObstacleRightOkFlag&&!ObstacleLfetOkFlag)
   {
      for(i=59;i>=LastLine;i--)
      {
      
        MiddleLine[i]=(int)(LeftEdge[i]+Width[i]*0.2);

      }
      
      ObstacleLfetControlDelay++;
      
      if(ObstacleLfetControlDelay>=20)
      {
         ObstacleLfetControlDelay=0;
         ObstacleRightOkFlag=0;
      }
      
   
   }

}

#endif












