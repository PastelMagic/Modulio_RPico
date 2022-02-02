//----------------------------------------
//--- PCA9632 Test Program
//----------------------------------------
//
#include <stdio.h>
#define	EXTERN
#include "Modulio.h"
#include "ACM1602.h"
#include "PCA9632.h"
#undef EXTERN

ACM1602	lcd;
PCA9632	led;

int main(void)
{
	UINT16 x,y,z;
	UINT8 bx,by,bz;
	MI2C_Start();
	ACM1602_Setup(&lcd, BSW_ADRS_DEFAULT, 0, ACM1602_ADRS0, 0);
	PCA9632_Setup(&led, BSW_ADRS_DEFAULT, 2, PCA9632_ADRS2, 0);
	lcd.Start(&lcd);
	led.Start(&led,0, 0xaa);
	x = 0;
	y = 0;
	z = 0;

	lcd.Position(&lcd, 0, 0);
	lcd.PrCString(&lcd,"00 00 00");
	while(1) {

		bx = (x & 0x100) ? (x ^ 0xff) & 0xff : x & 0xff;
		by = (y & 0x100) ? (y ^ 0xff) & 0xff : y & 0xff;
		bz = (z & 0x100) ? (z ^ 0xff) & 0xff : z & 0xff;

		lcd.Position(&lcd, 0, 0);
		lcd.PrHexByte(&lcd, bx);
		lcd.Position(&lcd, 0, 3);
		lcd.PrHexByte(&lcd, by);
		lcd.Position(&lcd, 0, 6);
		lcd.PrHexByte(&lcd, bz);

		led.Bright(&led,0,bx);
		led.Bright(&led,1,by);
		led.Bright(&led,2,bz);

		x++;
		y+=2;
		z+=4;
//		MI2C_Waitms(200);
	}
}
