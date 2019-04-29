//**************************************************************************
//*                        												   *
//*  �������� UART.C                                                       *
//*  ��  �ܣ� ���������ʹ��ڴ�����                                       *
//*  ˵  ���� �����򲿷�ժȡ�꾧�Ƽ�STC51ϵ�е�Ƭ��˫�����������̽����޸�  *
//*                        												   *
//************************************************************************** 

#include "STC51.h"
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include "UART.h"

#define NONE_PARITY     0           //��У��
#define ODD_PARITY      1           //��У��
#define EVEN_PARITY     2           //żУ��
#define MARK_PARITY     3           //���У��
#define SPACE_PARITY    4           //��У��

#define FOSC   11059200L              //ѡ����Ƶ��
#define BAUD1  115200    		    //����1������
#define PARITYBIT1 NONE_PARITY		//����1У��λ�趨

									
#define BAUD2  115200			    //����2������
#define PARITYBIT2 NONE_PARITY		//����2У��λ�趨

#define S2RI  0x01
#define S2TI  0x02

#define MaxBuf 64					//���ڻ������ֵ

#define T100Hz     (FOSC / 12 / 100)       //������Ƶ��  100hz=10ms

bit busy1;
bit busy2;

unsigned int  Uart1_Time,Uart2_Time;
unsigned char Uart1_Start,Uart2_Start,Uart1_OK,Uart2_OK;
unsigned char Rxnum1;
unsigned char Rxnum2;		 
unsigned char xdata RxBuf1[MaxBuf];	 //����1���ջ���
unsigned char xdata RxBuf2[MaxBuf];	 //����2���ջ���
unsigned char xdata sum_rule[]="0123456789";

unsigned char cnt;
unsigned int value;

//------------------------------------------------------------------------------------
void InitUART1(void)	             //����1��ʼ������
{

    #if (PARITYBIT1==NONE_PARITY)
    SCON = 0x5a;                     //8λ�ɱ䲨���� (��У��λ)
    #elif (PARITYBIT1==ODD_PARITY)||(PARITYBIT==EVEN_PARITY)||(PARITYBIT==MARK_PARITY)
    SCON = 0xda;                     //9λ�ɱ䲨����,У��λ��ʼΪ1
    #elif (PARITYBIT1== SPACE_PARITY)
    SCON = 0xd5;                     //9λ�ɱ䲨����,У��λ��ʼΪ0
	  #endif

    TH1 = -(FOSC/32/BAUD1);          //���ò����ʷ����������س�ֵ
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
void InitUART2(unsigned long nBaud)				//����2��ʼ������
{
    #if (PARITYBIT2==NONE_PARITY)
    S2CON = 0x5a;                   //8λ�ɱ䲨���� (��У��λ)
    #elif (PARITYBIT2==ODD_PARITY)||(PARITYBIT==EVEN_PARITY)||(PARITYBIT==MARK_PARITY)
    S2CON = 0xda;                   //9λ�ɱ䲨����,У��λ��ʼΪ1
    #elif (PARITYBIT2 == SPACE_PARITY)
    S2CON = 0xd5;                   //9λ�ɱ䲨����,У��λ��ʼΪ0
	#endif

    BRT = -(FOSC/32/nBaud);         //���ö��������ʷ����������س�ֵ
    IE2 = 0x01;                     //ʹ�ܴ���2�ж�
    EA = 1;                         //�����ж�
    Uart2_Time=0; 
    Uart2_Start=0;
	  Uart2_OK=0; 
}
//-----------------------------------------------------------------------------------
void SendByte1(unsigned char dat)  //����1���͵��ֽں���
{
    while (busy1);                 //�ȴ��ϸ����ݷ������
    ACC = dat;                     //ȡ��żУ��λP
    if (P)						   //����P�����ô������ݵ�У��λ
    {
        #if (PARITYBIT1==ODD_PARITY)
		  TB8=0;                   //��У��λΪ0
		#elif (PARITYBIT1==EVEN_PARITY)
		  TB8=1;                   //��У��λΪ1
        #endif
    }
    else
    {
        #if (PARITYBIT1==ODD_PARITY)
		  TB8=1;                  //��У��λΪ1
        #elif (PARITYBIT1==EVEN_PARITY)
		  TB8=0;                  //��У��λΪ0
        #endif
    }
    busy1 = 1;
    SBUF = ACC;                   //��������    
}
//------------------------------------------------------------------------------

void SendByte2(unsigned char dat) //����2���͵��ֽں���
{
    while (busy2);                //�ȴ��ϸ����ݷ������
    ACC = dat;                    //ȡ��żУ��λP
    if (P)						  //����P�����ô������ݵ�У��λ
    {
       #if (PARITYBIT2==ODD_PARITY)
	      S2CON&=~S2TB8;          //��У��λΪ0
       #elif (PARITYBIT2==EVEN_PARITY)
	      S2CON |= S2TB8;         //��У��λΪ1
       #endif
    }
    else
    {
       #if (PARITYBIT2==ODD_PARITY)
          S2CON |= S2TB8;         //��У��λΪ1
       #elif(PARITYBIT2==EVEN_PARITY)
          S2CON &= ~S2TB8;        //��У��λΪ0
       #endif
    }
    busy2 = 1;
    S2BUF = ACC;                  //�������� 
}

//-------------------------------------------------------------------------------
//STC12C5A60S2ϵ�е�Ƭ����������·�ɱ�̼��������У�PCA��ģ��
//�˴�����16λ������
void PCA_isr_init()                //PCA 16λ��������ʼ��                 
{                                  //  100hz=10ms  ���� /100=>*0.01=>*10ms,10ms����pca�ж�
    value = T100Hz;                //T100Hz 10ms����(FOSC / 12 / 100)  //������Ƶ�ʣ�FOSC / 12 / 100)=>FOSC/12*0.01=9216  
    CCAP0L = value;                //PCA ģ�� 0 �Ĳ�׽/�ȽϼĴ����� 8 λ��   ����ֵ   
    CCAP0H = value >> 8;           //PCA ģ�� 0 �Ĳ�׽/�ȽϼĴ����� 8 λ��   ����ֵ   ����8λֻʣ�¸�8λ
		value += T100Hz;
    CCAPM0 = 0x49;                 //PCA ģ��0 PWM �Ĵ���          
    CR = 1;                        //1:���� PCA ����������, �����������0��            
    EA = 1;
}

//-------------------------------------------------------------------------------
void PCA_isr() interrupt 7 //PCA �жϷ���  �������жϼ������˴���ͬʱCCF0=1����������Ҫ�����0��
{
    CCF0 = 0;                         //PCA ģ��0 �жϱ�־λ, ��Ӳ����λ, �����������0��        
    CCAP0L = value;                   //PCA ģ�� 0 �Ĳ�׽/�ȽϼĴ����� 8 λ��  
    CCAP0H = value >> 8;              //PCA ģ�� 0 �Ĳ�׽/�ȽϼĴ����� 8 λ�� 
    value += T100Hz;
		if(Uart1_Start==1) //UART1�����ݽ���
		{
				Uart1_Time++;
				if(Uart1_Time>10)  //��ʱ10MS
				{	       
					Uart1_Time=0;
					Uart1_Start=0;
					Uart1_OK=1;
				}
		}
		if(Uart2_Start==1) //UART2�����ݽ���
		{
				Uart2_Time++;
				if(Uart2_Time>10)  //��ʱ10MS
				{	       
					Uart2_Time=0;
					Uart2_Start=0;
					Uart2_OK=1;
				}
		}
}


//-------------------------------------------------------------------------------
void UARTInterrupt2(void) interrupt 8  //����2�жϽ��պ���
{
    if (S2CON & S2RI)
    {
       Uart2_Start=1;
 	     Uart2_Time=0; 
			 S2CON &= ~S2RI;         //���������ɱ�־
		   RxBuf2[Rxnum2]=S2BUF;
		   Rxnum2++;
		   if(Rxnum2>=MaxBuf)
		   {
		     Rxnum2=0;
		   }
    }
    if (S2CON & S2TI)
    {
        S2CON &= ~S2TI;         //���������ɱ�־
        busy2 = 0;
    }
}

//--------------------------------------------------------------------------------
void UARTInterrupt1(void) interrupt 4	//����1�жϽ��պ���
{
    if(RI)
    {
      Uart1_Start=1;
 	    Uart1_Time=0;
		  RI = 0;
      RxBuf1[Rxnum1]=SBUF;
      Rxnum1++;
      if(Rxnum1>=MaxBuf)
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

//--------------------------------------------------------------------------------
void ClearBuf(unsigned char No)	       //������ջ��������ݺ���
{
   unsigned char i;
   if(No==1)                          //����1
   {
      for(i=0;i<MaxBuf;i++)
      {
         RxBuf1[i]=0;
      }
      Rxnum1=0;
      Uart1_OK=0; 			
   }
   if(No==2)                         //����2
   {
      for(i=0;i<MaxBuf;i++)
      {
         RxBuf2[i]=0;
      }
      Rxnum2=0;
			Uart2_OK=0;
   }

}
//--------------------------------------------------------------------------------
unsigned char Strzmp(unsigned char *str1,unsigned char *str2)       //��һ���ַ��������ѯ�ؼ���
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
void SendString(unsigned char nCom,unsigned char *str)                   //����һ���ַ���
{
	 if(nCom==1)                                                       //����1����
	 {
	    do
      {
         SendByte1(*str++);
      }
      while(*str!=0);
   }
	 if(nCom==2)                                                    //����2����
	 {
	    do
      {
         SendByte2(*str++);
      }
      while(*str!=0);
   }
}  
//--------------------------------------------------------------------------------
void SendThe(unsigned char nCom,unsigned int sum)              //�����������
{
		if(nCom==1)                                               //����1����
		{
				if(sum>=100)   SendByte1(sum_rule[sum/100]);
				if(sum>=10)    SendByte1(sum_rule[sum%100/10]);
				if(sum>=0)     SendByte1(sum_rule[sum%100%10]);
		}
		if(nCom==2)                                                //����2����
		{
				if(sum>=100)   SendByte2(sum_rule[sum/100]);
				if(sum>=10)    SendByte2(sum_rule[sum%100/10]);
				if(sum>=0)     SendByte2(sum_rule[sum%100%10]);
		}
}
//--------------------------------------------------------------------------------
