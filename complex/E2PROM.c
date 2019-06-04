#include "STC51.H"
#include <intrins.h> 
#include "E2PROM.H"

#define CMD_IDIE    0      //stand-by  ��IAP�ȴ���
#define CMD_READ    1      //BYTE - READ  ����flash��
#define CMD_PROGRAM 2      // BYTE- PROGRAM  ��д�룩
#define CMD_ERASE   3      //SECTOR --ERASE   ��������


/*����Flash �����ȴ�ʱ�估����IAP/ISP/EEPROM �����ĳ���******************/  
//#define enable_waitTime 0x80  //ϵͳ����ʱ��<30MHz ʱ����IAP_CONTR �Ĵ������ô�ֵ  
//#define enable_waitTime 0x81  //ϵͳ����ʱ��<24MHz ʱ����IAP_CONTR �Ĵ������ô�ֵ  
//#define enable_waitTime 0x82  //ϵͳ����ʱ��<20MHz ʱ����IAP_CONTR �Ĵ������ô�ֵ  
#define enable_waitTime 0x83    //ϵͳ����ʱ��<12MHz ʱ����IAP_CONTR �Ĵ������ô�ֵ  
//#define enable_waitTime 0x84  //ϵͳ����ʱ��<6MHz  ʱ����IAP_CONTR �Ĵ������ô�ֵ

void ISP_IAP_disable(void) //�ر�ISP_IAP  
{     
    EA=1;                  //�ָ��ж�  
    IAP_CONTR =CMD_IDIE;   //IAP_CONTR=0   
	  IAP_CMD = 0x00;        //IAP_CMD�е� MS1 MS0=00 ����Standby����ģʽ����ISP����
    IAP_TRIG = 0x00;       //������Ĵ�����0                                        
}  
  
void ISP_IAP_trigger()    //ISP_IAP����   
{     
    EA=0;                 //�����2��ָ���������ִ��,�ʹ��ж�             
    IAP_TRIG = 0x5A;      //�ʹ���������0x5A       
    IAP_TRIG = 0xA5;      //�ʹ���������0xA5       
}  
         
            
void ISP_IAP_sectorErase(unsigned int sectorAddr)     //��������  
{             
    IAP_CONTR = enable_waitTime;                      //����ISP_IAP;���͵ȴ�ʱ��         
    IAP_CMD = CMD_ERASE;                              //����������������           
    IAP_ADDRH = (unsigned char)(sectorAddr>> 8);      //�͵�ַ���ֽ�     ��IAP_ADDRH �Ǹ�8λ�����԰�uintת��Ϊuchar��
    IAP_ADDRL = (unsigned char)(sectorAddr&0X00FF);   //�͵�ַ���ֽ�             
    ISP_IAP_trigger();                                //����  
    ISP_IAP_disable();                                //�ر�ISP_IAP����           
} 
void ISP_IAP_writeData(unsigned int beginAddr,unsigned char* pDat,unsigned int dataSize) //д����      
{             
    IAP_CONTR = enable_waitTime;                  //����ISP_IAP�����͵ȴ�ʱ��         
    IAP_CMD = CMD_PROGRAM;                        //���ֽڱ��������  
    while(dataSize--)                             //dataSizeΪ0����ѭ����˵������ȫ��д��
    {                                             
        IAP_ADDRH = (unsigned char)(beginAddr>> 8);    //�͵�ַ���ֽ�   ��ǿ��ת���ӵ�λ��ʼȡ������ȡ���ǵ�8λ��   
        IAP_ADDRL = (unsigned char)(beginAddr&0x00ff); //�͵�ַ���ֽ�   ��&0x00ff����Ϊȡ8λ��0&f=0��f&f=f �����忴beginAddr��         
        IAP_DATA = *pDat++;                  //������ 
	      _nop_();        
        beginAddr++;   
        ISP_IAP_trigger();                   //����  
    }            
        ISP_IAP_disable();   //�ر�                                        
}           
void ISP_IAP_readData(unsigned int beginAddr, unsigned char* pBuf, unsigned int dataSize) //��ȡ����  
{  
    IAP_DATA=0;                               //����,����Ҳ����  
    IAP_CMD = CMD_READ;                       //ָ��:��ȡ  
    IAP_CONTR = enable_waitTime;              //����ISP_IAP�����͵ȴ�ʱ��   
    while(dataSize--)                         //ѭ����ȡ  
    {  
        IAP_ADDRH = (unsigned char)(beginAddr >> 8);     //�͵�ַ���ֽ�  (ucharһ���ֽ�8λ��uint�����ֽ�16λ��Ϊ�˽�ʡռ���ڴ�)    
        IAP_ADDRL = (unsigned char)(beginAddr & 0x00ff); //�͵�ַ���ֽ�  
        ISP_IAP_trigger();                  //����  
        beginAddr++;                        //��ַ++  
        *pBuf++ = IAP_DATA;                 //�����ݱ��浽���ջ�����  
		    _nop_();
    }  
    ISP_IAP_disable();                      //�ر�ISP_IAP����  
}            
