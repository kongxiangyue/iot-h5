/********************************************************************************************************
*********************************************************************************************************

���Բ������£�
						1.��������˵��
							
							�¶�ģ��									  ��Ƭ��		   
							DQ1 <--------------�����ӡ�--------------->  P2.0
							
						2.Э��˵��
						
							�ѳ������ؽ�ȥ��  �ڴ��ڵ����������ò�����λ9600 Ȼ��򿪴���  ��Ƭ������5�����񴮿ڷ���
							��in service�� ��ʾ�����ܵ���˼��
							�ڴ��ڷ�����  ���� TEMP=?  ��Ƭ���᷵�ص�ǰ�¶�ֵ������������,��Ƭ���᷵�� 
							Command error  ��ʾ��ʽ�������˼��
				
**********************************************************************************************************
*********************************************************************************************************/

#include "STC51.h"
#include "intrins.h" 
#include "delay.h"
#include "DS18B20.h"
#include "main.h"
#include "UART.h"

unsigned int New_Temp;

void ScanA();

void InitTimer0(void)    //��ʱ��0,100us��ʱ
{
    TH0  = 0xFB;        
    TL0  = 0xAE;
    EA   = 1;
    ET0  = 1;
    TR0  = 1;
}
void Timer0Interrupt(void) interrupt 1
{
    TH0  = 0xFB;        
    TL0  = 0xAE;
		if(Uart1_Start==1) //UART1�����ݽ���
		{
				Uart1_Time++;
				if(Uart1_Time>100)  //��ʱ10MS
				{	       
					Uart1_Time=0;
					Uart1_Start=0;
					Uart1_OK=1;       //10MS�Ƚ�һ�ν��յ�����
				}
		}
		if(Uart2_Start==1) //UART2�����ݽ���
		{
				Uart2_Time++;
				if(Uart2_Time>100)  //��ʱ10MS
				{	       
					Uart2_Time=0;
					Uart2_Start=0;
					Uart2_OK=1;
				}
		}
}
void ScanA()  //�ȴ�����,���Ƚ��ַ���,�ٴ���
{
		unsigned char search_key;
		
		if(Uart1_OK==0)
		{
				return;
		}
		else
		{
				search_key=Strzmp(RxBuf1,"TEMP=?"); //���ҹؼ���

				if(search_key==1)
				{
						SendString(1,"TEMP=");
						SendThe(1,New_Temp);
						SendString(1,"��\r\n");
				}
				else
				{
						SendString(1,"Command error\r\n");
				}
				    ClearBuf(1);
		}
}
void main()
{
	  //��ʱ�ȴ�ϵͳ�ȶ�
	  delayms(20000); 
	  //T1x12=1,��ʱ��1����Ƶ; BRTR=1��BRTx12=1,ʹ�ö��������ʷ�����,����Ƶ;T0x12=1,��ʱ��0����Ƶ;
	  AUXR = AUXR|0x40|0x14|0x80;
	  //��ʱ��1��M1=1,M0=0,ʹ�÷�ʽ2,8λ�Զ���װ;��ʱ��0��M1=0,M0=1,ʹ�÷�ʽ1,16λ��ʱ��
	  TMOD = 0x20|0x01; 
		InitTimer0();
		InitUART1();
		InitUART2(115200); 
		SendString(1,"in service\r\n");  //����λ������"in service\r\n"
		while(1)
		{	
			ScanA();                           //�ȴ�����,���Ƚ��ַ���,�ٴ���
			New_Temp=DS_18B20_Get_Temp();      //��ȡ�¶�
		}
}



