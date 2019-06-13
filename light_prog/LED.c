#include <STC51.H>

#include "UART.h"

#define uchar unsigned char
#define uint unsigned int

sbit LED=P0^1;
sbit KEY=P1^0;

void delayedus(uchar us)  //΢����ʱ
{
		while(us--);
}

void manual() //�ֶ����ƺ���
{
		if(KEY==0)  //��ⰴ���Ƿ���
		{
			delayedus(20);  //��ʱ����
			if(KEY==0)
			{
					while(!KEY); //���ּ��
			}
			LED=~LED;   //ȡ��LED��״̬
			if(LED==0)
			{
					SendString(1,"Light=0\r\n");  //����LED״̬
			}
			if(LED==1)
			{
					SendString(1,"Light=1\r\n");  //����LED״̬
			}
		}
}

void ScanA()  //�������ݴ�����
{
		if(Uart1_OK==0)  //�Ƿ������ݽ���
		{
				return; //û�оͷ��ؿ�
		}
		else  //�оͽ������ݴ���
		{
			   // Light= ? �����ж��Ƿ��Ѿ����� 
				if(RxBuf1[0]='L'&&RxBuf1[1]=='i'&&RxBuf1[2]=='g'&&RxBuf1[3]=='h'&&RxBuf1[4]=='t'&&RxBuf1[5]=='='&&RxBuf1[6]=='?')  //�ж������Ƿ���Light=��
				{
						if(LED==0)   //����ǰ�ƵĿ���״������Ϊ����������λ�����Ϳ���ָ�����λ��ʵ�ֵƵ�ͬ����
						{
								SendString(1,"Light=0\r\n");
						}
						if(LED==1)   //����һ����
						{
								SendString(1,"Light=1\r\n");
						}
				}
				if(RxBuf1[0]='L'&&RxBuf1[1]=='i'&&RxBuf1[2]=='g'&&RxBuf1[3]=='h'&&RxBuf1[4]=='t'&&RxBuf1[5]=='='&&RxBuf1[6]=='0')  //�ж������Ƿ���Light=0
				{
						LED=0;
						SendString(1,"Light=0\r\n");
				}
				if(RxBuf1[0]='L'&&RxBuf1[1]=='i'&&RxBuf1[2]=='g'&&RxBuf1[3]=='h'&&RxBuf1[4]=='t'&&RxBuf1[5]=='='&&RxBuf1[6]=='1')  //�ж������Ƿ���Light=1
				{
						LED=1;
						SendString(1,"Light=1\r\n");
				}
			ClearBuf(1);  //������ڽ�������
		}
}

void main()
{                           
	AUXR = AUXR|0x40|0x14|0x80; // 1101 0100  // ����Ƶ
	TMOD = 0x20|0x01;       // 0010 0001          
	InitUART1();	                    //����1��ʼ������
	InitUART2(115200);	              //����2��ʼ������
	PCA_isr_init();                   //PCA��ʼ��               
	while(1)
	{
		ScanA();                       //�������ݴ�����
		manual();                      //�ֶ����ƺ���
	}
}