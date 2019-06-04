#include "STC51.h"
#include <intrins.h>
#include "UART.h"
#include "delay.h"
#include "DS18B20.h"
#include "E2PROM.H"

sbit RUN  = P2^0;                            //运行指示灯
sbit OUT1 = P2^1;                            //灯控制
sbit OUT2 = P2^2;                            //电视机控制
sbit OUT3 = P2^3;                            //窗帘控制端
sbit OUT4 = P2^4;                            //窗帘控制端
sbit OUT5 = P2^5;                            //门禁控制端
sbit FM   = P2^6;


bit zhi=0,zhuan_t;
unsigned int temp,guan,Timer;
//----------------------------------------------------------------
void InitTimer0(void)
{
		TH0 = 0xFB;
    TL0 = 0xAE;             //100US一次
    EA   = 1;
    ET0  = 1;
    TR0  = 1;
}
//-----------------------------------------------------------------
void Timer0Interrupt(void) interrupt 1
{
		TH0 = 0xFB;
    TL0 = 0xAE;             //100US一次
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
void Boot_wait(unsigned char ci,unsigned int ss)                    //蜂鸣器
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
void LED_state(void)                                //灯的状态返回
{
		if(OUT1==0)   SendString(1,"Light=1\r\n");
		if(OUT1==1)   SendString(1,"Light=0\r\n");
}
//-----------------------------------------------------------------------
void television_state(void)                        //电视状态返回
{
		if(OUT2==0)   SendString(1,"TV=1\r\n");
		if(OUT2==1)   SendString(1,"TV=0\r\n");
}
//-----------------------------------------------------------------------
void curtains_state(void)                          //窗帘状态返回
{
		if(zhuan_t==1)  SendString(1,"curtains=1\r\n");
		if(zhuan_t==0)  SendString(1,"curtains=0\r\n");
}
//-----------------------------------------------------------------------
void control_LED(void)                            //开灯函数
{
		unsigned char ii;
	
		ii=Strzmp(RxBuf1,"Light=1");                  //开灯
		if(ii==1)
		{
				OUT1=0;
				ii=0;
				LED_state();
		}
		
		ii=Strzmp(RxBuf1,"Light=0");                  //关灯
		if(ii==1)   
		{
				OUT1=1;
				ii=0;
				LED_state();
		}
}
//-----------------------------------------------------------------------
void control_TV(void)                            //开电视函数
{
		unsigned char ii;
	
		ii=Strzmp(RxBuf1,"TV=1");                     //开电视
		if(ii==1)
		{
				OUT2=0;
				ii=0;
				television_state();
		}
		
		ii=Strzmp(RxBuf1,"TV=0");                     //关电视
		if(ii==1)    
		{
				OUT2=1;
				ii=0;
				television_state();
		}
}
//-----------------------------------------------------------------------
void control_curtains(void)                     //开窗帘函数
{
		unsigned char ii;
		
		ii=Strzmp(RxBuf1,"curtains=1");               //开窗帘
		if(ii==1)
		{
				OUT3=0;OUT4=1;
				zhuan_t=1;                               //记忆表示开状态
				zhi=1;
				ii=0;
				curtains_state();
		}
		
		ii=Strzmp(RxBuf1,"curtains=0");               //关窗帘
		if(ii==1)
		{
				OUT3=1;OUT4=0;
				zhuan_t=0;                                //记忆表示关状态
				zhi=1;
				ii=0;
				curtains_state();
		}
}
//-----------------------------------------------------------------------
void Read_Temperature(void)                       //读取温度函数
{
		unsigned char ii;
	
		ii=Strzmp(RxBuf1,"Take");                     //读取温度
		if(ii==1)
		{
				SendString(1,"T:");
				SendThe(1,temp);
				SendString(1,"\r\n");
				ii=0;
		}
}
//-----------------------------------------------------------------------
void Read_state(void)                             //上位机获取状态函数
{
		unsigned char ii;	
		
		ii=Strzmp(RxBuf1,"state=?\r\n");                  //读取状态
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
		unsigned char dat1[6];                       //原始密码/修改密码存储区
		unsigned char dat2[6];                       //读取密码存放区

		ii1=Strzmp(RxBuf1,":");                     
		if(ii1==1)
		{
				ii2=Strzmp(RxBuf1,"*1234#");              //恢复原始密码
				if(ii2!=0)                                  
				{
						SendString(1,"in recovery\r\n");      //返回恢复中   
						Boot_wait(5,3000);                
						Boot_wait(2,400);
						dat1[0]='1';dat1[1]='2';
						dat1[2]='3';dat1[3]='4';
						dat1[4]='5';dat1[5]='6';
						ISP_IAP_sectorErase(0x0000);           //擦除
						ISP_IAP_writeData(0x0000,dat1,6);      //写数据
						SendString(1,"Restore success\r\n");   //返回恢复成功
						delayms(8000);
						SendString(1,"empty\r\n");
						ClearBuf(1);
						ii2=0;
						return;
				}
				ISP_IAP_readData(0x0000,dat2,6);           //读数据
				if((RxBuf1[1]==dat2[0])&&(RxBuf1[2]==dat2[1])&&(RxBuf1[3]==dat2[2])&&(RxBuf1[4]==dat2[3])&&(RxBuf1[5]==dat2[4])&&(RxBuf1[6]==dat2[5]))   //密码正确
				{
						ii2=Strzmp(RxBuf1,"*");               //改密码
						if(ii2!=0)
						{
								if(Rxnum1==16)                            //密码输入格式正确
								{
										SendString(1,"being revised\r\n");   //返回修改密码中
										Boot_wait(1,8000);
										Boot_wait(2,400);
										dat1[0]=RxBuf1[7];
										dat1[1]=RxBuf1[8];
										dat1[2]=RxBuf1[9];
										dat1[3]=RxBuf1[10];
										dat1[4]=RxBuf1[11];
										dat1[5]=RxBuf1[12];
										ISP_IAP_sectorErase(0x0000);             //擦除
										ISP_IAP_writeData(0x0000,dat1,6);        //写数据
										SendString(1,"signal if\r\n");           //返回密码修改成功
										fang=1;
								}
								else                                          //密码输入格式错误
								{
										SendString(1,"Input error\r\n");         //返回修改密码输入格式错误
										Boot_wait(3,1000);
										fang=1;
								}
								ii2=0;
						}
						else                                              //开锁
						{
								SendString(1,"open the door\r\n");           //返回输入密码正确
								OUT5=0;
								fang=1;zhi=1;
						}
				}
				else   //密码不正确
				{
						SendString(1,"wrong password\r\n");      //返回密码输入不正常
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
						control_LED();                               //开灯
						control_TV();                                //开电视
						control_curtains();                          //开关窗帘
						Read_Temperature();                          //获取温度
						Read_state();                                //获取状态
						switch_lock();                               //开锁函数
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