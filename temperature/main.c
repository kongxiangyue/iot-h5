/********************************************************************************************************
*********************************************************************************************************

测试步骤如下：
						1.引脚连接说明
							
							温度模块									  单片机		   
							DQ1 <--------------【连接】--------------->  P2.0
							
						2.协议说明
						
							把程序下载进去后  在串口调试哪里设置波特率位9600 然后打开串口  单片机运行5秒后会像串口发送
							“in service“ 表示运行总的意思。
							在串口发送区  输入 TEMP=?  单片机会返回当前温度值。如果输入错误,单片机会返回 
							Command error  表示格式错误的意思。
				
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

void InitTimer0(void)    //定时器0,100us计时
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
		if(Uart1_Start==1) //UART1有数据接收
		{
				Uart1_Time++;
				if(Uart1_Time>100)  //计时10MS
				{	       
					Uart1_Time=0;
					Uart1_Start=0;
					Uart1_OK=1;       //10MS比较一次接收的数据
				}
		}
		if(Uart2_Start==1) //UART2有数据接收
		{
				Uart2_Time++;
				if(Uart2_Time>100)  //计时10MS
				{	       
					Uart2_Time=0;
					Uart2_Start=0;
					Uart2_OK=1;
				}
		}
}
void ScanA()  //等待接收,并比较字符串,再处理。
{
		unsigned char search_key;
		
		if(Uart1_OK==0)
		{
				return;
		}
		else
		{
				search_key=Strzmp(RxBuf1,"TEMP=?"); //查找关键字

				if(search_key==1)
				{
						SendString(1,"TEMP=");
						SendThe(1,New_Temp);
						SendString(1,"℃\r\n");
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
	  //延时等待系统稳定
	  delayms(20000); 
	  //T1x12=1,定时器1不分频; BRTR=1、BRTx12=1,使用独立波特率发生器,不分频;T0x12=1,定时器0不分频;
	  AUXR = AUXR|0x40|0x14|0x80;
	  //定时器1：M1=1,M0=0,使用方式2,8位自动重装;定时器0：M1=0,M0=1,使用方式1,16位定时器
	  TMOD = 0x20|0x01; 
		InitTimer0();
		InitUART1();
		InitUART2(115200); 
		SendString(1,"in service\r\n");  //向上位机发送"in service\r\n"
		while(1)
		{	
			ScanA();                           //等待接收,并比较字符串,再处理。
			New_Temp=DS_18B20_Get_Temp();      //获取温度
		}
}



