#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_







#define Data_num_A  1   //���͵ı���������1������16λ,2���ֽڣ�
#define Data_num_B  2   //���͵ı���������1������16λ��2���ֽڣ�

/*���ܳ���ģʽö����������*/
enum mode_car
{
  car_A,
  car_B,
};


void LogicalFunction(void);
extern enum mode_car car;//������������
extern void car_communication();// ˫��ͨ�ź���
extern void PORTE_IRQHandler();//PORTE�жϷ�����

#endif