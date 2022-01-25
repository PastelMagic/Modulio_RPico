//----------------------------------------
//--- LPS25H Test Program
//----------------------------------------
//
#include <stdio.h>
#define	EXTERN
#include "Modulio.h"
#include "ACM1602.h"
#include "LPS25H.h"
#undef EXTERN

ACM1602	lcd;
LPS25H	psense;
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

int main(void)
{
	MI2C_Start();
	ACM1602_Setup(&lcd, BSW_ADRS_DEFAULT, 0, ACM1602_ADRS0, 0);
	LPS25H_Setup(&psense, BSW_ADRS_DEFAULT, 1, LPS25H_ADRS0, 0);
	lcd.Start(&lcd);
	lcd.Position(&lcd, 0, 0);
	psense.Start(&psense);
	while(1) {
		psense.ReadPress(&psense);
		psense.wConvPress(&psense);
		lcd.Position(&lcd, 0, 0);
//		sprintf(s," %04d.%02dhPa", (psense.PRESS[0] << 8)|(psense.PRESS[1]), psense.PRESS[2]);
//		lcd.PrString(&lcd, s);
		pr4dec(s, (psense.PRESS[0] << 8)|(psense.PRESS[1]));
		lcd.PrString(&lcd, s);
		lcd.WriteData(&lcd, '.');
		pr2dec(s, psense.PRESS[2]);
		lcd.PrString(&lcd, s);
		lcd.PrCString(&lcd, "hPa");

		psense.ReadTemp(&psense);
		psense.wConvTemp(&psense);
		lcd.Position(&lcd, 1, 0);
//		sprintf(s," %02d.%02d%cC", psense.TEMP[0], psense.TEMP[1], 0xdf);
//		lcd.PrString(&lcd, s);
		pr2dec(s, psense.TEMP[0]);
		lcd.PrString(&lcd, s);
		lcd.WriteData(&lcd, '.');
		pr2dec(s, psense.TEMP[1]);
		lcd.PrString(&lcd, s);
		lcd.WriteData(&lcd, 0xdf);
		lcd.WriteData(&lcd, 'C');
	}
}
