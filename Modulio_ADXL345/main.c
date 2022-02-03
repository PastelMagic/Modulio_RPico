//----------------------------------------
//--- ADXL345 Test Program
//----------------------------------------
//
#include <stdio.h>
#define	EXTERN
#include "Modulio.h"
#include "ACM1602.h"
#include "ADXL345.h"
#undef EXTERN

#if RaspiPico
#include "pico/stdlib.h"
#endif

ACM1602	lcd;
ADXL345	gsense;

UINT8 fx, fy, fz;
UINT16 x,y,z;
void exec_gsense(void)
{
	gsense.GetVal(&gsense);	// Gセンサの値取得
	x = gsense.xdat;	// X方向データ
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
	lcd.Position(&lcd, 0, 2);
	lcd.WriteData(&lcd, fx);
	lcd.PrHexInt(&lcd, x);
	lcd.Position(&lcd, 0, 10);
	lcd.WriteData(&lcd, fy);
	lcd.PrHexInt(&lcd, y);
	lcd.Position(&lcd, 1, 2);
	lcd.WriteData(&lcd, fz);
	lcd.PrHexInt(&lcd, z);
}

int main(void)
{
#if RaspiPico
	stdio_init_all();
#endif

	MI2C_Start();
	ACM1602_Setup(&lcd, BSW_ADRS_NONE, 0, ACM1602_ADRS0, 0);
	ADXL345_Setup(&gsense, BSW_ADRS_NONE, 0, ADXL345_ADRS0, 0);
	lcd.Start(&lcd);

	gsense.Start(&gsense);

	lcd.Position(&lcd, 0, 0);
	lcd.PrCString(&lcd, "X:");
	lcd.Position(&lcd, 0, 8);
	lcd.PrCString(&lcd, "Y:");
	lcd.Position(&lcd, 1, 0);
	lcd.PrCString(&lcd, "Z:");
	lcd.Position(&lcd, 1, 8);
	lcd.PrCString(&lcd, "ADXL345");

	while(1) {
		exec_gsense();
#if RaspiPico
		printf("X:%04XH Y:%04XH Z:%04XH\n",gsense.xdat, gsense.ydat, gsense.zdat);
#endif
		MI2C_Waitms(100);
	}
}
