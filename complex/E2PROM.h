//********************************************************************
//*                                                                  *
//*   ��������E2PROM.H                                               *
//*   ��  �ܣ�ͷ�ļ�                                                 *
//*                                                                  *
//********************************************************************

#ifndef _E2PROM_H_
#define _E2PROM_H_

   void ISP_IAP_disable(void); //�ر�ISP_IAP 
   void ISP_IAP_trigger();     //����
   void ISP_IAP_readData(unsigned int beginAddr, unsigned char* pBuf, unsigned int dataSize); //��ȡ���� 
   void ISP_IAP_writeData(unsigned int  beginAddr,unsigned char* pDat,unsigned int dataSize); //д���� 
   void ISP_IAP_sectorErase(unsigned int sectorAddr); //��������

#endif