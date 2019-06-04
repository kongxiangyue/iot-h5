//********************************************************************
//*                                                                  *
//*   程序名：E2PROM.H                                               *
//*   功  能：头文件                                                 *
//*                                                                  *
//********************************************************************

#ifndef _E2PROM_H_
#define _E2PROM_H_

   void ISP_IAP_disable(void); //关闭ISP_IAP 
   void ISP_IAP_trigger();     //触发
   void ISP_IAP_readData(unsigned int beginAddr, unsigned char* pBuf, unsigned int dataSize); //读取数据 
   void ISP_IAP_writeData(unsigned int  beginAddr,unsigned char* pDat,unsigned int dataSize); //写数据 
   void ISP_IAP_sectorErase(unsigned int sectorAddr); //扇区擦除

#endif