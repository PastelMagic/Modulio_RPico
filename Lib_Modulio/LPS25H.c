//---------------------------------------
//- LPS25H Pressure Sensor
//- 温度・気圧センサ
//---------------------------------------
#define	EXTERN
#include "Modulio.h"
#undef EXTERN
#include "LPS25H.h"

//-------------------------------------------
//--- Initialize Device
//--- デバイス初期化
//-------------------------------------------
static UINT8	LPS25H_Start(LPS25H *p)
{
	UINT8	sts;
	sts = (p->madrs).ReadByte(&(p->madrs), 0x0f, &(p->ID));
	if (sts != MI2C_ACKslave)
		return(sts);
	if (p->ID != LPS25H_ID)
		return(sts);
	sts = (p->madrs).WriteByte(&(p->madrs), 0x20, 0x90);
	return(sts);
}

//-------------------------------------------
//--- Read PRESSOUT(Pressure) Registers
//--- 0x28(PRESS[0]:LSB) - 0x2A(PRESS[2]:MSB)
//--- 0x28から0x2aの3バイトが気圧（0x28がLSB)
//-------------------------------------------
static UINT8	LPS25H_ReadPress(LPS25H *p)
{
	UINT8 c,sts;
	for (c=0; c<3; c++) {
		sts = (p->madrs).ReadByte(&(p->madrs), 0x28+c, &(p->PRESS[c]));
		if (sts != MI2C_ACKslave)
			break;
	}
	return(sts);
}

//---------------------------------------------------
//--- Convert PRESS Reg.Value
//--- PRESS[0](MSB)-PRESS[1][LSB):Integer Part
//--- PRESS[2]:Fractional Part
//--- PRESS[0]-[2]の値を整数部分と小数点以下に分割
//---------------------------------------------------
static void	LPS25H_wConvPress(LPS25H *p)
{
	UINT16	frac;
	frac = ((p->PRESS[1] & 0xf) << 8) | p->PRESS[0];	//*** PRESS[0]:LSB PRESS[2]:MSB
	frac = (frac * 100) >> 12;
	p->PRESS[1] =((p->PRESS[2] & 0xf)<<4) | (p->PRESS[1] >>4);
	p->PRESS[0] = p->PRESS[2] >> 4;
	p->PRESS[2] = frac & 0xff;
}

//-------------------------------------------
//--- Read Temp(TEMP_OUT_L/H) Register
//--- TEMP_OUTレジスタで温度読み込み
//-------------------------------------------
static UINT8	LPS25H_ReadTemp(LPS25H *p)
{
	UINT8 c,sts;
	for (c=0; c<2; c++) {
		sts = (p->madrs).ReadByte(&(p->madrs), 0x2b+c, &(p->TEMP[c]));
		if (sts != MI2C_ACKslave)
			break;
	}
	return(sts);
}

//-------------------------------------------
//--- Convert Temp. Data
//--- TEMP[0]:Integer Part
//--- TMEP[1]:Fractional Part
//--- 42.5+(TEMP_OUT/480)
//--- レジスタ値から小数点温度データに変換
//--- TEMP[0]：整数部分
//--- TEMP[1]：小数点以下
//-------------------------------------------
static void	LPS25H_wConvTemp(LPS25H *p)
{
	UINT16 idat,fdat;
	UINT16 dat;
	dat = (p->TEMP[1] << 8) | p->TEMP[0];
	if ((dat & 0x8000) && (dat < 0xb050)) {
		dat = (((20400 + dat) & 0xffff)^0xffff)+1;
		idat = 0x80;
	} else {
		dat = (20400 + dat) & 0xffff;
		idat = 0;
	}
	idat += (dat / 480) & 0x7f;
	dat -= idat*480;
	fdat = dat*5/24;
	p->TEMP[0] = (UINT8)idat;
	p->TEMP[1] = (UINT8)fdat;
}

//-------------------------------------------
//--- Read Status Register
//--- ステータスレジスタ読み込み
//-------------------------------------------
static UINT8	LPS25H_ReadStatus(LPS25H *p)
{
	UINT8 sts;
	sts = (p->madrs).ReadByte(&(p->madrs), 0x27, &(p->STS));		//*** Adrs for StatusReg is 0x28
	return(sts);
}

//-------------------------------------------
//--- Initialize Structure
//--- 構造体の初期化
//-------------------------------------------
void LPS25H_Setup(
	LPS25H *p,
	UINT8 bswadrs,
	UINT8 bswch,
	UINT8 devadrs,
	UINT8 option)
{
	p->Start = &LPS25H_Start;
	p->ReadPress = &LPS25H_ReadPress;
	p->wConvPress = &LPS25H_wConvPress;
	p->ReadTemp = &LPS25H_ReadTemp;
	p->wConvTemp = &LPS25H_wConvTemp;
	p->ReadStatus = &LPS25H_ReadStatus;

	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
