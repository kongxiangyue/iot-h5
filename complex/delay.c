#include "STC51.h"
#include "delay.h"
void delayus(unsigned int us)																		 //΢�뼶��ʱ
{
		while(us--);
}
void delayms(unsigned int ms)					//���뼶��ʱ
{
		unsigned char i,j;
		while(ms--)
		{
				for(i=0;i<16;i++)
					for(j=0;j<20;j++);			  
		}
}			