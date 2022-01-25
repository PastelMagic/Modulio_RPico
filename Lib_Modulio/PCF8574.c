//---------------------------------------
//- HDC1000 Humidity & Temp. Sensor	-
//-					-
//---------------------------------------
//
#define	EXTRN
#include "Modulio.h"
#undef EXTERN
#include "PCF8574.h"

static UINT8 PCF8574_Start(PCF8574 *p)
{
	UINT8	c,stat;
	c = 0xff;
	stat = MI2C_WriteBytes(&(p->madrs), REG_ADRS_NONE, &c, 1);	// ‚Æ‚è‚ ‚¦‚¸‚·‚×‚Ä'1'‚É‚·‚é
	return(stat);
}

static UINT8 PCF8574_Write(PCF8574 *p, UINT8 dat)
{
	UINT8	c, stat;
	c = dat | p->INPUT_Port;	// “ü—Íƒ|[ƒg‚Í'1'‚É‚·‚é
	stat = MI2C_WriteBytes(&(p->madrs), REG_ADRS_NONE, &c, 1);
	if (stat == MI2C_ACKslave)
		p->OUTDATA = c;
	return(stat);
}

static UINT8 PCF8574_Read(PCF8574 *p)
{
	UINT8	c, stat;
	stat = MI2C_ReadBytes(&(p->madrs), REG_ADRS_NONE, &(p->INDATA), 1);
	return(stat);
}

void PCF8574_Setup(
	PCF8574 *p,
	UINT8 bswadrs,
	UINT8 bswch,
	UINT8 devadrs,
	UINT8 option)
{
	p->INPUT_Port=0;
	p->INDATA = 0;
	p->OUTDATA = 0;
	p->Start = &PCF8574_Start;
	p->Write = &PCF8574_Write;
	p->Read = &PCF8574_Read;
	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
