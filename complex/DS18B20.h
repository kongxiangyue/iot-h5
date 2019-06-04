#ifndef _DS18B20_H_
#define _DS18B20_H_

sbit ds	=	P1^0;												  //DS18B20 ÎÂ¶È²É¼¯

void DS_18B20_init();
unsigned char DS_18B20_du();
unsigned char DS_18B20_zijie();
void DS_18B20_ling(unsigned char ling);
unsigned int DS_18B20_quwen();

#endif

