#include "STC51.H"
#include <intrins.h> 
#include "E2PROM.H"

#define CMD_IDIE    0      //stand-by  （IAP等待）
#define CMD_READ    1      //BYTE - READ  （读flash）
#define CMD_PROGRAM 2      // BYTE- PROGRAM  （写入）
#define CMD_ERASE   3      //SECTOR --ERASE   （擦除）


/*定义Flash 操作等待时间及允许IAP/ISP/EEPROM 操作的常数******************/  
//#define enable_waitTime 0x80  //系统工作时钟<30MHz 时，对IAP_CONTR 寄存器设置此值  
//#define enable_waitTime 0x81  //系统工作时钟<24MHz 时，对IAP_CONTR 寄存器设置此值  
//#define enable_waitTime 0x82  //系统工作时钟<20MHz 时，对IAP_CONTR 寄存器设置此值  
#define enable_waitTime 0x83    //系统工作时钟<12MHz 时，对IAP_CONTR 寄存器设置此值  
//#define enable_waitTime 0x84  //系统工作时钟<6MHz  时，对IAP_CONTR 寄存器设置此值

void ISP_IAP_disable(void) //关闭ISP_IAP  
{     
    EA=1;                  //恢复中断  
    IAP_CONTR =CMD_IDIE;   //IAP_CONTR=0   
	  IAP_CMD = 0x00;        //IAP_CMD中的 MS1 MS0=00 进入Standby待机模式，无ISP操作
    IAP_TRIG = 0x00;       //命令触发寄存器清0                                        
}  
  
void ISP_IAP_trigger()    //ISP_IAP触发   
{     
    EA=0;                 //下面的2条指令必须连续执行,故关中断             
    IAP_TRIG = 0x5A;      //送触发命令字0x5A       
    IAP_TRIG = 0xA5;      //送触发命令字0xA5       
}  
         
            
void ISP_IAP_sectorErase(unsigned int sectorAddr)     //扇区擦除  
{             
    IAP_CONTR = enable_waitTime;                      //开启ISP_IAP;并送等待时间         
    IAP_CMD = CMD_ERASE;                              //送扇区擦除命令字           
    IAP_ADDRH = (unsigned char)(sectorAddr>> 8);      //送地址高字节     （IAP_ADDRH 是高8位，所以把uint转换为uchar）
    IAP_ADDRL = (unsigned char)(sectorAddr&0X00FF);   //送地址低字节             
    ISP_IAP_trigger();                                //触发  
    ISP_IAP_disable();                                //关闭ISP_IAP功能           
} 
void ISP_IAP_writeData(unsigned int beginAddr,unsigned char* pDat,unsigned int dataSize) //写数据      
{             
    IAP_CONTR = enable_waitTime;                  //开启ISP_IAP，并送等待时间         
    IAP_CMD = CMD_PROGRAM;                        //送字节编程命令字  
    while(dataSize--)                             //dataSize为0跳出循环，说明数据全部写入
    {                                             
        IAP_ADDRH = (unsigned char)(beginAddr>> 8);    //送地址高字节   （强制转换从低位开始取，这里取的是低8位）   
        IAP_ADDRL = (unsigned char)(beginAddr&0x00ff); //送地址低字节   （&0x00ff是因为取8位，0&f=0，f&f=f ，具体看beginAddr）         
        IAP_DATA = *pDat++;                  //送数据 
	      _nop_();        
        beginAddr++;   
        ISP_IAP_trigger();                   //触发  
    }            
        ISP_IAP_disable();   //关闭                                        
}           
void ISP_IAP_readData(unsigned int beginAddr, unsigned char* pBuf, unsigned int dataSize) //读取数据  
{  
    IAP_DATA=0;                               //清零,不清也可以  
    IAP_CMD = CMD_READ;                       //指令:读取  
    IAP_CONTR = enable_waitTime;              //开启ISP_IAP，并送等待时间   
    while(dataSize--)                         //循环读取  
    {  
        IAP_ADDRH = (unsigned char)(beginAddr >> 8);     //送地址高字节  (uchar一个字节8位、uint两个字节16位，为了节省占用内存)    
        IAP_ADDRL = (unsigned char)(beginAddr & 0x00ff); //送地址低字节  
        ISP_IAP_trigger();                  //触发  
        beginAddr++;                        //地址++  
        *pBuf++ = IAP_DATA;                 //将数据保存到接收缓冲区  
		    _nop_();
    }  
    ISP_IAP_disable();                      //关闭ISP_IAP功能  
}            
