//---------------------------------------
//- HDC1000 Humidity & Temp. Sensor	-
//- 湿度・温度センサ			-
//---------------------------------------
//
#define	EXTERN
#include "Modulio.h"
#undef EXTERN
#include "HDC1000.h"

//---------------------------------------
//--- Init Device
//--- デバイス初期化
//---------------------------------------
static UINT8 HDC1000_Start(HDC1000 *p)
{
	UINT8 stat;
	UINT8	c[2];
				// Control Register (Reg.Adrs=0x02)
	c[0] = 0x00;		// 通常動作、ヒータOFF、温湿度個別 温度14bit、湿度14bit
	c[1] = 0x00;
	MI2C_Waitms(20);	// Device Startup time;
	stat = (p->madrs).WriteBytes(&(p->madrs), 0x02, c, 2);
	return(stat);
}

//---------------------------------------
//--- Heater ON/OFF Ctrl
//--- ヒータON/OFF
//---------------------------------------
static UINT8 HDC1000_Heater(HDC1000 *p, UINT8 hon)
{
	UINT8	stat, c[2];
					// Control Register (Reg.Adrs=0x02)
	c[0] = hon ? 0x20: 0x00;	// ヒーターON/OFF
	c[1] = 0;
	stat = (p->madrs).WriteBytes(&(p->madrs), 0x02, c, 2);
	return(stat);
}

//---------------------------------------
//--- Read Temp.
//--- 温度データ読み込み
//---------------------------------------
static UINT8 HDC1000_ReadTemp(HDC1000 *p)
{
	UINT8 stat;
	// 温度レジスタは0x00
	stat = (p->madrs).WriteBytes(&(p->madrs), 0x00, p->TEMP, 0);	// Temp. Reg# is 0x00 温度レジスタは0x00
	if (stat == MI2C_ACKslave) {
		MI2C_Waitms(20);	// Wait for conversion 変換時間待ち
		stat = (p->madrs).ReadBytes(&(p->madrs), REG_ADRS_NONE, p->TEMP, 2);
	}
	return(stat);
}

//---------------------------------------
//--- Read Humidity
//--- 湿度データ読み込み
//---------------------------------------
static UINT8 HDC1000_ReadHum(HDC1000 *p)
{
	UINT8 stat;
	stat = (p->madrs).WriteBytes(&(p->madrs), 0x01, p->HUM, 0);	// Humidity Reg# is 0x01 湿度レジスタは0x01
	if (stat == MI2C_ACKslave) {
		MI2C_Waitms(20);	// Wait for conversion  変換時間待ち
		stat = (p->madrs).ReadBytes(&(p->madrs), REG_ADRS_NONE, p->HUM, 2);
	}
	return(stat);
}

//---------------------------------------
//--- Convert Temp. Data to Int&Frac.
//--- 温度データを整数部と小数部に分離
//--- 整数部分のビット7='1'なら負
//---------------------------------------
static void HDC1000_wConvTemp(HDC1000 *p)
{
	UINT8 flag;
	UINT16	dat;
	dat = ((p->TEMP[0])*165)+(((p->TEMP[1])*165)>>8);
	if (dat >= 40*256) {
		flag = 0;
		dat -= 40*256;
	} else {
		flag = 0x80;
		dat = 0x40*256-dat;
	}
	p->TEMP[0] = (dat >> 8) | flag;		//*** Integer
	p->TEMP[1] = ((dat & 0xff)*100) >> 8;	//*** Fraction
}

//---------------------------------------
//--- Convert Hum.Data to Int&Frac.
//--- 温度データを整数部と小数部に分離
//---------------------------------------
static void HDC1000_wConvHum(HDC1000 *p)
{
	UINT16	dat;

	dat = ((p->HUM[0])*100)+(((p->HUM[1])*100) >> 8);
	p->HUM[0] = dat >> 8;
	p->HUM[1] = ((dat & 0xff)*100)>>8;
}

//****
#if 0
static float f,h;
float HDC1000_ConvTemp(UINT16 temp)
{
	p->ff = ((float)temp)*165/65536-40.0;
	return(f);
}

float HDC1000_ConvHum(UINT16 hum)
{
	h = ((float)hum)*100/65536;
	return(h);
}
#endif

//---------------------------------------
//--- Init Structure
//--- 構造体初期化
//---------------------------------------
void HDC1000_Setup(
	HDC1000 *p,
	UINT8 bswadrs,
	UINT8 bswch,
	UINT8 devadrs,
	UINT8 option)
{
	p->TEMP[0]=0; p->TEMP[1]=0;
	p->HUM[0]=0;  p->HUM[1]=0;

	p->Start = &HDC1000_Start;
	p->Heater = &HDC1000_Heater;
	p->ReadTemp = &HDC1000_ReadTemp;
	p->ReadHum = &HDC1000_ReadHum;
	p->wConvTemp = &HDC1000_wConvTemp;
	p->wConvHum = &HDC1000_wConvHum;

	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
