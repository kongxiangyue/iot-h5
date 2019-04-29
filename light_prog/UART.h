//********************************************************************
//*   程序名：UART.H                                                 *
//*   说  明：头文件                                                 *
//********************************************************************
#ifndef _UART_H_
#define _UART_H_

	extern unsigned int  Uart1_Time,Uart2_Time;
	extern unsigned char Uart1_Start,Uart2_Start,Uart1_OK,Uart2_OK;
	extern unsigned char Rxnum1;
	extern unsigned char Rxnum2;		 
	extern unsigned char xdata RxBuf1[];	 //串口1接收缓冲
	extern unsigned char xdata RxBuf2[];	 //串口2接收缓冲
	extern unsigned char xdata sum_rule[];
//   #define WatchDog_Start  WDT_CONTR=0x22  //启动看门狗(复位时间157.3mS)
//   #define WatchDog_Stop   WDT_CONTR=0x00	//停止看门狗
//   #define WatchDog_Clear  WDT_CONTR|=0x10	//喂狗指令
	void InitUART1(void);
	void InitUART2(unsigned long nBaud);
	void SendByte1(unsigned char dat);
	void SendByte2(unsigned char dat);
	void PCA_isr_init();
	void ClearBuf(unsigned char No);
	unsigned char Strzmp(unsigned char *str1,unsigned char *str2);
	void SendString(unsigned char nCom,unsigned char *str);
	void SendThe(unsigned char nCom,unsigned int sum);

#endif
