#include "STC51.h"
#include <intrins.h>
#include "UART.h"
#include "delay.h"
#include "DS18B20.h"
#include "E2PROM.H"

sbit RUN  = P2^0;                            //����ָʾ��
sbit OUT1 = P2^1;                            //�ƿ���
sbit OUT2 = P2^2;                            //���ӻ�����
sbit OUT3 = P2^3;                            //�������ƶ�
sbit OUT4 = P2^4;                            //�������ƶ�
sbit OUT5 = P2^5;                            //�Ž����ƶ�
sbit FM   = P2^6;


bit zhi=0,zhuan_t;
unsigned int temp,guan,Timer;
//----------------------------------------------------------------
void InitTimer0(void)
{
		TH0 = 0xFB;
    TL0 = 0xAE;             //100USһ��
    EA   = 1;
    ET0  = 1;
    TR0  = 1;
}
//-----------------------------------------------------------------
void Timer0Interrupt(void) interrupt 1
{
		TH0 = 0xFB;
    TL0 = 0xAE;             //100USһ��
		Timer++;
		if(Timer==5000)
		{
				Timer=0;
				RUN=~RUN;
		}
		if(zhi==1)
		{
				guan++;
				if(guan==20000)
				{
						guan=0;
						zhi=0;	
						OUT3=OUT4=OUT5=1;
				}
		}
}
//-------------------------------------------------------------------
void Boot_wait(unsigned char ci,unsigned int ss)                    //������
{
		unsigned char ii;
		
		for(ii=0;ii<ci;ii++)
		{
				FM=0;
				delayms(ss);
				FM=1;
				delayms(ss);
		}
		FM=1;
}
//----------------------------------------------------------------------
void LED_state(void)                                //�Ƶ�״̬����
{
		if(OUT1==0)   SendString(1,"Light=1\r\n");
		if(OUT1==1)   SendString(1,"Light=0\r\n");
}
//-----------------------------------------------------------------------
void television_state(void)                        //����״̬����
{
		if(OUT2==0)   SendString(1,"TV=1\r\n");
		if(OUT2==1)   SendString(1,"TV=0\r\n");
}
//-----------------------------------------------------------------------
void curtains_state(void)                          //����״̬����
{
		if(zhuan_t==1)  SendString(1,"curtains=1\r\n");
		if(zhuan_t==0)  SendString(1,"curtains=0\r\n");
}
//-----------------------------------------------------------------------
void control_LED(void)                            //���ƺ���
{
		unsigned char ii;
	
		ii=Strzmp(RxBuf1,"Light=1");                  //����
		if(ii==1)
		{
				OUT1=0;
				ii=0;
				LED_state();
		}
		
		ii=Strzmp(RxBuf1,"Light=0");                  //�ص�
		if(ii==1)   
		{
				OUT1=1;
				ii=0;
				LED_state();
		}
}
//-----------------------------------------------------------------------
void control_TV(void)                            //�����Ӻ���
{
		unsigned char ii;
	
		ii=Strzmp(RxBuf1,"TV=1");                     //������
		if(ii==1)
		{
				OUT2=0;
				ii=0;
				television_state();
		}
		
		ii=Strzmp(RxBuf1,"TV=0");                     //�ص���
		if(ii==1)    
		{
				OUT2=1;
				ii=0;
				television_state();
		}
}
//-----------------------------------------------------------------------
void control_curtains(void)                     //����������
{
		unsigned char ii;
		
		ii=Strzmp(RxBuf1,"curtains=1");               //������
		if(ii==1)
		{
				OUT3=0;OUT4=1;
				zhuan_t=1;                               //�����ʾ��״̬
				zhi=1;
				ii=0;
				curtains_state();
		}
		
		ii=Strzmp(RxBuf1,"curtains=0");               //�ش���
		if(ii==1)
		{
				OUT3=1;OUT4=0;
				zhuan_t=0;                                //�����ʾ��״̬
				zhi=1;
				ii=0;
				curtains_state();
		}
}
//-----------------------------------------------------------------------
void Read_Temperature(void)                       //��ȡ�¶Ⱥ���
{
		unsigned char ii;
	
		ii=Strzmp(RxBuf1,"Take");                     //��ȡ�¶�
		if(ii==1)
		{
				SendString(1,"T:");
				SendThe(1,temp);
				SendString(1,"\r\n");
				ii=0;
		}
}
//-----------------------------------------------------------------------
void Read_state(void)                             //��λ����ȡ״̬����
{
		unsigned char ii;	
		
		ii=Strzmp(RxBuf1,"state=?\r\n");                  //��ȡ״̬
		if(ii==1)
		{
				LED_state();
				delayms(500);
				television_state();
				delayms(500);
				curtains_state();
				ii=0;
		}
}
//-----------------------------------------------------------------------
void switch_lock(void)
{
		bit fang=0;
		unsigned char ii1,ii2;
		unsigned char dat1[6];                       //ԭʼ����/�޸�����洢��
		unsigned char dat2[6];                       //��ȡ��������

		ii1=Strzmp(RxBuf1,":");                     
		if(ii1==1)
		{
				ii2=Strzmp(RxBuf1,"*1234#");              //�ָ�ԭʼ����
				if(ii2!=0)                                  
				{
						SendString(1,"in recovery\r\n");      //���ػָ���   
						Boot_wait(5,3000);                
						Boot_wait(2,400);
						dat1[0]='1';dat1[1]='2';
						dat1[2]='3';dat1[3]='4';
						dat1[4]='5';dat1[5]='6';
						ISP_IAP_sectorErase(0x0000);           //����
						ISP_IAP_writeData(0x0000,dat1,6);      //д����
						SendString(1,"Restore success\r\n");   //���ػָ��ɹ�
						delayms(8000);
						SendString(1,"empty\r\n");
						ClearBuf(1);
						ii2=0;
						return;
				}
				ISP_IAP_readData(0x0000,dat2,6);           //������
				if((RxBuf1[1]==dat2[0])&&(RxBuf1[2]==dat2[1])&&(RxBuf1[3]==dat2[2])&&(RxBuf1[4]==dat2[3])&&(RxBuf1[5]==dat2[4])&&(RxBuf1[6]==dat2[5]))   //������ȷ
				{
						ii2=Strzmp(RxBuf1,"*");               //������
						if(ii2!=0)
						{
								if(Rxnum1==16)                            //���������ʽ��ȷ
								{
										SendString(1,"being revised\r\n");   //�����޸�������
										Boot_wait(1,8000);
										Boot_wait(2,400);
										dat1[0]=RxBuf1[7];
										dat1[1]=RxBuf1[8];
										dat1[2]=RxBuf1[9];
										dat1[3]=RxBuf1[10];
										dat1[4]=RxBuf1[11];
										dat1[5]=RxBuf1[12];
										ISP_IAP_sectorErase(0x0000);             //����
										ISP_IAP_writeData(0x0000,dat1,6);        //д����
										SendString(1,"signal if\r\n");           //���������޸ĳɹ�
										fang=1;
								}
								else                                          //���������ʽ����
								{
										SendString(1,"Input error\r\n");         //�����޸����������ʽ����
										Boot_wait(3,1000);
										fang=1;
								}
								ii2=0;
						}
						else                                              //����
						{
								SendString(1,"open the door\r\n");           //��������������ȷ
								OUT5=0;
								fang=1;zhi=1;
						}
				}
				else   //���벻��ȷ
				{
						SendString(1,"wrong password\r\n");      //�����������벻����
						Boot_wait(2,400);
						fang=1;
				}
				ii1=0;
		}
		if(fang==1)
		{
				delayms(8000);
				SendString(1,"empty\r\n");
				fang=0;
		}
}
//-----------------------------------------------------------------------
void Scan1(void)
{
		unsigned char ii;
		if(Uart1_OK==0)
		{
				return;
		}
		else
		{
				ii=Strzmp(RxBuf1,"\r\n");
				if(ii!=0)
				{
						control_LED();                               //����
						control_TV();                                //������
						control_curtains();                          //���ش���
						Read_Temperature();                          //��ȡ�¶�
						Read_state();                                //��ȡ״̬
						switch_lock();                               //��������
				}
				ClearBuf(1);
		}
}
//-----------------------------------------------------------------------
void main()
{
		delayms(10000);
		AUXR = AUXR|0x40|0x14|0x80; 
		TMOD = 0x20|0x01;
		InitTimer0();
		InitUART1();
		InitUART2();
		PCA_isr_init();
		RUN=0;
		while(1)
		{
				temp=DS_18B20_quwen();
				Scan1();
		}
}