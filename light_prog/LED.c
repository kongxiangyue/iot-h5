#include <STC51.H>

#include "UART.h"

#define uchar unsigned char
#define uint unsigned int

sbit LED=P0^1;
sbit KEY=P1^0;

void delayedus(uchar us)  //微秒延时
{
		while(us--);
}

void manual() //手动控制函数
{
		if(KEY==0)  //检测按键是否按下
		{
			delayedus(20);  //延时消抖
			if(KEY==0)
			{
					while(!KEY); //松手检测
			}
			LED=~LED;   //取反LED的状态
			if(LED==0)
			{
					SendString(1,"Light=0\r\n");  //发送LED状态
			}
			if(LED==1)
			{
					SendString(1,"Light=1\r\n");  //发送LED状态
			}
		}
}

void ScanA()  //串口数据处理函数
{
		if(Uart1_OK==0)  //是否有数据接收
		{
				return; //没有就返回空
		}
		else  //有就进行数据处理
		{
			   // Light= ? 用来判断是否已经联网 
				if(RxBuf1[0]='L'&&RxBuf1[1]=='i'&&RxBuf1[2]=='g'&&RxBuf1[3]=='h'&&RxBuf1[4]=='t'&&RxBuf1[5]=='='&&RxBuf1[6]=='?')  //判断数据是否是Light=？
				{
						if(LED==0)   //看当前灯的开关状况，若为开，即向上位机发送开的指令，与下位机实现灯的同步。
						{
								SendString(1,"Light=0\r\n");
						}
						if(LED==1)   //与上一样。
						{
								SendString(1,"Light=1\r\n");
						}
				}
				if(RxBuf1[0]='L'&&RxBuf1[1]=='i'&&RxBuf1[2]=='g'&&RxBuf1[3]=='h'&&RxBuf1[4]=='t'&&RxBuf1[5]=='='&&RxBuf1[6]=='0')  //判断数据是否是Light=0
				{
						LED=0;
						SendString(1,"Light=0\r\n");
				}
				if(RxBuf1[0]='L'&&RxBuf1[1]=='i'&&RxBuf1[2]=='g'&&RxBuf1[3]=='h'&&RxBuf1[4]=='t'&&RxBuf1[5]=='='&&RxBuf1[6]=='1')  //判断数据是否是Light=1
				{
						LED=1;
						SendString(1,"Light=1\r\n");
				}
			ClearBuf(1);  //清除串口接收数据
		}
}

void main()
{                           
	AUXR = AUXR|0x40|0x14|0x80; // 1101 0100  // 不分频
	TMOD = 0x20|0x01;       // 0010 0001          
	InitUART1();	                    //串口1初始化函数
	InitUART2(115200);	              //串口2初始化函数
	PCA_isr_init();                   //PCA初始化               
	while(1)
	{
		ScanA();                       //串口数据处理函数
		manual();                      //手动控制函数
	}
}