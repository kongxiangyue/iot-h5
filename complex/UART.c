//**************************************************************************
//*                        												   *
//*  程序名： UART.C                                                       *
//*  功  能： 串口驱动和串口处理函数                                       *
//*  说  明： 本程序部分摘取宏晶科技STC51系列单片机双串口驱动例程进行修改  *
//*                        												   *
//************************************************************************** 

#include "STC51.h"
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include "UART.h"

#define NONE_PARITY     0           //无校验
#define ODD_PARITY      1           //奇校验
#define EVEN_PARITY     2           //偶校验
#define MARK_PARITY     3           //标记校验
#define SPACE_PARITY    4           //空校验

#define FOSC   11059200L              //选择晶振频率
#define BAUD1  115200     		    //串口1波特率
#define PARITYBIT1 NONE_PARITY		//串口1校验位设定

									
#define BAUD2  115200 			    //串口2波特率
#define PARITYBIT2 NONE_PARITY		//串口2校验位设定

#define S2RI  0x01
#define S2TI  0x02

#define MaxBuf1 250					//串口缓冲最大值
#define MaxBuf2 250					//串口缓冲最大值
#define QHz    2000       //频率

#define THz  (FOSC/12/QHz)    //500微秒一次

bit busy1;
bit busy2;
unsigned int  Uart1_Time,Uart2_Time;
unsigned char Uart1_Start,Uart2_Start,Uart1_OK,Uart2_OK;
unsigned char Rxnum1;
unsigned char Rxnum2;		 
unsigned char xdata RxBuf1[MaxBuf1];	 //串口1接收缓冲
unsigned char xdata RxBuf2[MaxBuf2];	 //串口2接收缓冲
unsigned char xdata sum_rule[]="0123456789";

unsigned char cnt;
unsigned int value;

//------------------------------------------------------------------------------------
void InitUART1(void)	             //串口1初始化函数
{

    #if (PARITYBIT1==NONE_PARITY)
    SCON = 0x5a;                     //8位可变波特率 (无校验位)
    #elif (PARITYBIT1==ODD_PARITY)||(PARITYBIT==EVEN_PARITY)||(PARITYBIT==MARK_PARITY)
    SCON = 0xda;                     //9位可变波特率,校验位初始为1
    #elif (PARITYBIT1== SPACE_PARITY)
    SCON = 0xd5;                     //9位可变波特率,校验位初始为0
	  #endif

    TH1 = -(FOSC/32/BAUD1);          //设置波特率发生器的重载初值
    TL1 = TH1;
    ES = 1;
    EA = 1;
    PCON = 0x00;
    TR1 = 1;
	  Uart1_Time=0; 
    Uart1_Start=0;
	  Uart1_OK=0;
}
//-----------------------------------------------------------------------------------
void InitUART2(void)				//串口2初始化函数
{
    #if (PARITYBIT2==NONE_PARITY)
    S2CON = 0x5a;                   //8位可变波特率 (无校验位)
    #elif (PARITYBIT2==ODD_PARITY)||(PARITYBIT==EVEN_PARITY)||(PARITYBIT==MARK_PARITY)
    S2CON = 0xda;                   //9位可变波特率,校验位初始为1
    #elif (PARITYBIT2 == SPACE_PARITY)
    S2CON = 0xd5;                   //9位可变波特率,校验位初始为0
	#endif

    BRT = -(FOSC/32/BAUD2);         //设置独立波特率发生器的重载初值
    IE2 = 0x01;                     //使能串口2中断
    EA = 1;                         //开总中断
    Uart2_Time=0; 
    Uart2_Start=0;
	  Uart2_OK=0; 
}
//-----------------------------------------------------------------------------------
void SendByte1(unsigned char dat)  //串口1发送单字节函数
{
    while (busy1);                 //等待上个数据发送完成
    ACC = dat;                     //取得偶校验位P
    if (P)						   //根据P来设置串口数据的校验位
    {
        #if (PARITYBIT1==ODD_PARITY)
		  TB8=0;                   //置校验位为0
		#elif (PARITYBIT1==EVEN_PARITY)
		  TB8=1;                   //置校验位为1
        #endif
    }
    else
    {
        #if (PARITYBIT1==ODD_PARITY)
		  TB8=1;                  //置校验位为1
        #elif (PARITYBIT1==EVEN_PARITY)
		  TB8=0;                  //置校验位为0
        #endif
    }
    busy1 = 1;
    SBUF = ACC;                   //发送数据    
}
//------------------------------------------------------------------------------

void SendByte2(unsigned char dat) //串口2发送单字节函数
{
    while (busy2);                //等待上个数据发送完成
    ACC = dat;                    //取得偶校验位P
    if (P)						  //根据P来设置串口数据的校验位
    {
       #if (PARITYBIT2==ODD_PARITY)
	      S2CON&=~S2TB8;          //置校验位为0
       #elif (PARITYBIT2==EVEN_PARITY)
	      S2CON |= S2TB8;         //置校验位为1
       #endif
    }
    else
    {
       #if (PARITYBIT2==ODD_PARITY)
          S2CON |= S2TB8;         //置校验位为1
       #elif(PARITYBIT2==EVEN_PARITY)
          S2CON &= ~S2TB8;        //置校验位为0
       #endif
    }
    busy2 = 1;
    S2BUF = ACC;                  //发送数据 
}

//-------------------------------------------------------------------------------

void PCA_isr_init()                   //PCA 计数器初始化                 
{               
    value = THz;                   
    CCAP0L = value;                   //PCA 模块 0 的捕捉/比较寄存器低 8 位。      
    CCAP0H = value >> 8;              //PCA 模块 0 的捕捉/比较寄存器高 8 位。 
		value += THz;
    CCAPM0 = 0x49;                    //PCA 模块0 PWM 寄存器          
    CR = 1;                           //1:允许 PCA 计数器计数, 必须由软件清0。            
    EA = 1;
}

//-------------------------------------------------------------------------------
void UARTInterrupt1(void) interrupt 4	//串口1中断接收函数
{
    if(RI)
    {
      Uart1_Start=1;
 	    Uart1_Time=0;
		  RI = 0;
      RxBuf1[Rxnum1]=SBUF;
      Rxnum1++;
      if(Rxnum1>=MaxBuf1)
      {
        Rxnum1=0;
      }
    }
    else
	  {
      TI = 0;
		  busy1=0;
	  }
}
//-------------------------------------------------------------------------------
void UARTInterrupt2(void) interrupt 8  //串口2中断接收函数
{
    if (S2CON & S2RI)
    {
       Uart2_Start=1;
 	     Uart2_Time=0; 
			 S2CON &= ~S2RI;         //清除接收完成标志
		   RxBuf2[Rxnum2]=S2BUF;
		   Rxnum2++;
		   if(Rxnum2>=MaxBuf2)
		   {
		     Rxnum2=0;
		   }
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //清除发送完成标志
        busy2 = 0;
    }
}

//--------------------------------------------------------------------------------

void PCA_isr() interrupt 7 
{
    CCF0 = 0;                         //PCA 模块0 中断标志, 由硬件置位, 必须由软件清0。        
    CCAP0L = value;                   //PCA 模块 0 的捕捉/比较寄存器低 8 位。  
    CCAP0H = value >> 8;              //PCA 模块 0 的捕捉/比较寄存器高 8 位。 
    value += THz;
		if(Uart1_Start==1) //UART1有数据接收
		{
				Uart1_Time++;
				if(Uart1_Time>=20)  //计时10MS
				{	       
					Uart1_Time=0;
					Uart1_Start=0;
					Uart1_OK=1;
				}
		}
		if(Uart2_Start==1) //UART2有数据接收
		{
				Uart2_Time++;
				if(Uart2_Time>=20)  //计时10MS
				{	       
					Uart2_Time=0;
					Uart2_Start=0;
					Uart2_OK=1;
				}
		}
}

//--------------------------------------------------------------------------------
void ClearBuf(unsigned char No)	       //清除接收缓冲区数据函数
{
   unsigned char i;
   if(No==1)                          //串口1
   {
      for(i=0;i<MaxBuf1;i++)
      {
         RxBuf1[i]=0;
      }
      Rxnum1=0;
      Uart1_OK=0; 			
   }
   if(No==2)                         //串口2
   {
      for(i=0;i<MaxBuf2;i++)
      {
         RxBuf2[i]=0;
      }
      Rxnum2=0;
			Uart2_OK=0;
   }

}
//--------------------------------------------------------------------------------
unsigned char Strzmp(unsigned char *str1,unsigned char *str2)       //在一传字符传里面查询关键字
{
   unsigned char i=0;
	 unsigned char *Temppoint;
	 Temppoint=strstr(str1,str2);
	
   if(Temppoint==NULL)
	 {
	   i=0;
	 }
	 else
	 {
	   i=Temppoint-str1+1;
	 }
	 return i;
}
//--------------------------------------------------------------------------------
void SendString(unsigned char nCom,unsigned char *str)                   //发送一传字符串
{
	 if(nCom==1)                                                       //串口1发送
	 {
	    do
      {
         SendByte1(*str++);
      }
      while(*str!=0);
   }
	 if(nCom==2)                                                    //串口2发送
	 {
	    do
      {
         SendByte2(*str++);
      }
      while(*str!=0);
   }
}  

//--------------------------------------------------------------------------------
//void SendMem(unsigned char nCom,unsigned char *str,unsigned char nSize)      //发送字节和字符串   以长度发送
//{
//	 unsigned char i;
//		
//	 if(nCom==1)
//	 {
//	    for(i=0;i<nSize;i++)
//      {
//         SendByte1(*str++);
//      }
//   }
//	 if(nCom==2)
//	 {
//	    for(i=0;i<nSize;i++)
//      {
//         SendByte2(*str++);
//      }
//   }
//}
//--------------------------------------------------------------------------------
void SendThe(unsigned char nCom,unsigned int sum)              //发送求和数据
{
		if(nCom==1)                                               //串口1发送
		{
				if(sum>=100)   SendByte1(sum_rule[sum/100]);
				if(sum>=10)    SendByte1(sum_rule[sum%100/10]);
				SendByte1('.');
				if(sum>=0)     SendByte1(sum_rule[sum%100%10]);
		}
		if(nCom==2)                                                //串口2发送
		{
				if(sum>=100)   SendByte2(sum_rule[sum/100]);
				if(sum>=10)    SendByte2(sum_rule[sum%100/10]);
				SendByte2('.');
				if(sum>=0)     SendByte2(sum_rule[sum%100%10]);
		}
}
//--------------------------------------------------------------------------------
