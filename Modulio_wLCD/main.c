//----------------------------------------
//--- LPS25H Test Program
//----------------------------------------
//
#include <stdio.h>
#define	EXTERN
#include "Modulio.h"
#include "ACM1602.h"
#include "ADXL345.h"
#include "LPS25H.h"
#include "PCA9632.h"
#undef EXTERN

ACM1602	lcd1,lcd2;
ADXL345	gsense;
LPS25H	psense;
PCA9632	led;
char	s[16];

void pr2dec(char *s, UINT8 dat)
{
	*s++ = (dat/10) + '0';
	*s++ = (dat%10) + '0';
	*s = 0x00;
}

void pr4dec(char *s, UINT16 dat)
{
	pr2dec(s, dat/100);
	pr2dec(s+2, dat%100);
	*(s+4) = 0x00;
}


UINT8 fx, fy, fz;
UINT16 x,y,z;
void exec_gsense(void)
{
	gsense.GetVal(&gsense);
	x = gsense.xdat;
	if (x & 0x200) {
		fx = '-';
		x = 0x400-x;
	} else	fx = '+';
	y = gsense.ydat;
	if (y & 0x200) {
		fy = '-';
		y = 0x400-y;
	} else	fy = '+';
	z = gsense.zdat;
	if (z & 0x200) {
		fz = '-';
		z = 0x400-z;
	} else	fz = '+';
	lcd1.Position(&lcd1, 0, 2);
	lcd1.WriteData(&lcd1, fx);
	lcd1.PrHexInt(&lcd1, x);
	lcd1.Position(&lcd1, 0, 10);
	lcd1.WriteData(&lcd1, fy);
	lcd1.PrHexInt(&lcd1, y);
	lcd1.Position(&lcd1, 1, 2);
	lcd1.WriteData(&lcd1, fz);
	lcd1.PrHexInt(&lcd1, z);
}

void exec_psense(void)
{
	psense.ReadPress(&psense);
	psense.wConvPress(&psense);
	lcd2.Position(&lcd2, 0, 0);

//	sprintf(s," %04d.%02dhPa", (psense.PRESS[0] << 8)|(psense.PRESS[1]), psense.PRESS[2]);
//	lcd2.PrString(&lcd2, s);
	pr4dec(s, (psense.PRESS[0] << 8)|(psense.PRESS[1]));
	lcd2.PrString(&lcd2, s);
	lcd2.WriteData(&lcd2, '.');
	pr2dec(s, psense.PRESS[2]);
	lcd2.PrString(&lcd2, s);
	lcd2.PrCString(&lcd2, "hPa");

	psense.ReadTemp(&psense);
	psense.wConvTemp(&psense);
	lcd2.Position(&lcd2, 1, 0);
//	sprintf(s," %02d.%02d%cC", psense.TEMP[0], psense.TEMP[1], 0xdf);
//	lcd2.PrString(&lcd2, s);
	pr2dec(s, psense.TEMP[0]);
	lcd2.PrString(&lcd2, s);
	lcd2.WriteData(&lcd2, '.');
	pr2dec(s, psense.TEMP[1]);
	lcd2.PrString(&lcd2, s);
	lcd2.WriteData(&lcd2, 0xdf);
	lcd2.WriteData(&lcd2, 'C');
}

void exec_led(UINT8 x, UINT8 y, UINT8 z)
{
	led.Bright(&led,0,x);
	led.Bright(&led,1,y);
	led.Bright(&led,2,z);
}

int main(void)
{
	UINT8 xd,yd,zd;
	MI2C_Start();
	ACM1602_Setup(&lcd1, BSW_ADRS_DEFAULT, 0, ACM1602_ADRS0, 0);
	ACM1602_Setup(&lcd2, BSW_ADRS_DEFAULT, 3, ACM1602_ADRS0, 0);
	ADXL345_Setup(&gsense, BSW_ADRS_DEFAULT, 1, ADXL345_ADRS0, 0);
	LPS25H_Setup(&psense, BSW_ADRS_DEFAULT, 1, LPS25H_ADRS0, 0);
	PCA9632_Setup(&led, BSW_ADRS_DEFAULT, 2, PCA9632_ADRS2, 0);
	lcd1.Start(&lcd1);
	lcd2.Start(&lcd2);

	gsense.Start(&gsense);
	psense.Start(&psense);
	led.Start(&led,0, 0xaa);

	lcd1.Position(&lcd1, 0, 0);
	lcd1.PrCString(&lcd1, "X:");
	lcd1.Position(&lcd1, 0, 8);
	lcd1.PrCString(&lcd1, "Y:");
	lcd1.Position(&lcd1, 1, 0);
	lcd1.PrCString(&lcd1, "Z:");
	lcd1.Position(&lcd1, 1, 8);
	lcd1.PrCString(&lcd1, "ADXL345");


	while(1) {
		exec_gsense();
		exec_psense();
		if (x >= 0x100)
			xd = 0xff;
		else	xd = x;
		if (y >= 0x100)
			yd = 0xff;
		else	yd = y;
		if (z >= 0x100)
			zd = 0xff;
		else	zd = z;
		exec_led(xd, yd, zd);
		MI2C_Waitms(100);
	}
}
