#ifndef _DS18B20_H_
#define _DS18B20_H_

sbit ds	=	P2^0;												  //DS18B20 ÎÂ¶È²É¼¯

void DS_18B20_init();
unsigned char DS_18B20_read();
unsigned char DS_18B20_byte();
void DS_18B20_command(unsigned char command);
unsigned int DS_18B20_Get_Temp();

#endif

