//----------------------------------------
//--- ADXL345 Test Program
//----------------------------------------
//
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "hardware/i2c.h"
const uint LED_PIN = 25;

#define	EXTERN
#include "Modulio.h"
#include "ACM1602.h"
#include "ADXL345.h"
#undef EXTERN

ACM1602	lcd;
ADXL345	gsense;
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
	UINT16 x,y,z;
	UINT8 fx, fy, fz;

	stdio_init_all();
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);

	sleep_ms(15000);
	gpio_put(LED_PIN, 0);
	sleep_ms(1000);

	puts("MI2C Start\n");
	sleep_ms(250);
	MI2C_Start();
	puts("LCD Setup\n");
	sleep_ms(250);
	ACM1602_Setup(&lcd, BSW_ADRS_DEFAULT, 0, ACM1602_ADRS0, 0);
	puts("ADXL Setup\n");
	sleep_ms(250);
	ADXL345_Setup(&gsense, BSW_ADRS_DEFAULT, 1, ADXL345_ADRS0, 0);
	puts("LCD Star\n");
	sleep_ms(250);
	lcd.Start(&lcd);
	lcd.Position(&lcd, 0, 0);
	lcd.PrCString(&lcd, "X:");
	lcd.Position(&lcd, 0, 8);
	lcd.PrCString(&lcd, "Y:");
	lcd.Position(&lcd, 1, 0);
	lcd.PrCString(&lcd, "Z:");
	lcd.Position(&lcd, 1, 8);
	lcd.PrCString(&lcd, "ADXL345");
	gsense.Start(&gsense);
	while(1) {
		gsense.GetVal(&gsense);
		x = gsense.xdat;
		printf("GsenseData = %d\n", x);
		sleep_ms(250);
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
		MI2C_Waitms(200);
	}
}
