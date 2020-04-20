#include <reg52.h>
#include "INTRINS.h"

//¾§Õñ:24Mhz
sbit DQ = P3^7;

void delay960us()
{
	unsigned char us;
	for(us=230;us>0;us--)
		;
}
void delay1s()
{
	int i;
	for(i=1000;i>0;i--)
		delay960us();
}

void delay6us()
{
	char us;
	for(us=0;us>0;us--);
}
void delay15us()
{
	char us;
	for(us=2;us>0;us--);
}
void delay45us()
{
	char us;
	for(us=8;us>0;us--);
}
void writeByte(char dat)
{
	char i;
	for(i=8;i>0;i--)
	{
		DQ = 0;
		delay6us();
		DQ = dat & 0x01;
		delay6us();
		dat >>= 1;
		delay45us();
		DQ = 1;
		delay15us();
	}
}
char readByte()
{
	char i,dat=0;
	int j=0;
	for(i=8;i>0;i--)
	{
		dat >>= 1;
		DQ = 0;
		DQ = 1;
		delay6us();
		if(DQ)
			dat |= 0x80;
		else
			dat &= 0x7F;
		delay45us();
		delay15us();
	}
	return dat;
}
char resetDev()
{
	char i;
	DQ = 0;
	delay960us();
	DQ = 1;
	for(i=100;i>0;i--)
	{
		if(DQ == 0)
			return 1;
	}
	return -1;
}
char readBit()
{
	char dat;

		DQ = 0;
		DQ = 1;
		delay6us();
		if(DQ)
			dat = 1;
		else
			dat = 0;
		delay45us();
		delay15us();
	
	return dat;
}
void writeBit(char dat)
{
		DQ = 0;
		delay6us();
		DQ = dat & 0x01;
		delay6us();
		delay45us();
		DQ = 1;
		delay15us();
}
unsigned char searchRom()
{
	char R1,R2,i=0,j;
	char id[8];
	if(resetDev() == 1)
	{
		delay960us();
		writeByte(0xF0);//search rom
		for(j=0;j<8;j++)
		{
			for(i=0;i<8;i++)
			{
				id[j] >>=1;
				id[j] &= 0x7F;
				R1 = readBit();
				R2 = readBit();
				if(R1 == 0 && R2 == 1){	
					id[j] |= 0x0;
					writeBit(0);
					continue;
				}else if(R1 == 1 && R2 == 0){
					id[j] |= 0x80;
					writeBit(1);
					continue;
				}else if(R1 == 0 && R2 == 0){//³åÍ»
					id[j] |= 0x0; //ÏÈËÑË÷0
					writeBit(0);
					continue;
				}else if(R1 == 1 && R2 == 1){
					break; //Ã»ÓÐÆ÷¼þ
				}
			}
		}
	}
	return id[0];
}
	
unsigned char readRom()
{
	char id[6],i;
	if(resetDev() == 1)
	{
		delay960us();
		writeByte(0x33);//read rom
		for(i=0;i<6;i++)
			id[i] = readByte();
	}
	return id[0];
}

short tempRead()
{
	char ack = 0;
	char tl,th;
	short t;
	searchRom();
	ack = resetDev();
	if(ack == 1)
	{
		delay960us();
		writeByte(0xCC);
		writeByte(0x44);
		delay1s();
		
		resetDev();
		delay960us();
		writeByte(0xCC);
		writeByte(0xBE);		
		tl = readByte();
		th = readByte();
		t = th * 256 + tl;
    t = (float)t * 0.0625*10;
		return t;
	}
	return -1;
}

void main()
{
	while(1)
	{
		tempRead();
	}
}