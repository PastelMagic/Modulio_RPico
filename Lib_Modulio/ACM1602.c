//---------------------------------------
//- ACM1602 16x2 Character Display	-
//-					-
//---------------------------------------
//
#define	EXTERN
#include "Modulio.h"
#undef EXTERN
#include "ACM1602.h"


static UINT8 ACM1602_Start(ACM1602 *p)
{
	UINT8 stat;
	MI2C_Waitms(15);
	do {
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x38);
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x0c);
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x01);
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x02);
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = p->Clear(p);
	} while(0);
	return(stat);
}


static void ACM1602_Position(ACM1602 *p, UINT8 y, UINT8 x)
{
	p->Xpos = x & 0xf;
	p->Ypos = y & 1;
	if (p->Ypos == 0)
		p->DDRAM_Adrs = 0x00;
	else	p->DDRAM_Adrs = 0x40;
	p->DDRAM_Adrs |= p->Xpos;
}

static UINT8 ACM1602_Clear(ACM1602 *p)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x01);
	if (stat == MI2C_WRITE_ERR)
		return(stat);
	MI2C_Waitms(5);
	(p->Position)(p, 0,0);
	return(stat);

}

static UINT8 ACM1602_WriteData(ACM1602 *p, UINT8 dat)
{
	UINT8 stat;
	stat = MI2C_ACKslave;
	switch(dat) {
		case	'\r':
			ACM1602_Position(p, p->Ypos, 0);
			break;
		case	'\n':
			ACM1602_Position(p, (p->Ypos)+1, 0);
			break;
		default:
			stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x80 | p->DDRAM_Adrs);
			if (stat == MI2C_WRITE_ERR)
				break;
			stat = (p->madrs).WriteByte(&(p->madrs), 0x80, dat);
			(p->Position)(p, p->Ypos, p->Xpos+1);
			break;
	}
	return(stat);
}

static const UINT8 HexDat[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static UINT8 ACM1602_PrHexByte(ACM1602 *p, UINT8 dat)
{
	UINT8 stat, c;
	c = (dat >> 4) & 0xf;
	stat = (p->WriteData)(p, HexDat[c]);
	if (stat != MI2C_WRITE_ERR) {
		c = dat & 0xf;
		stat = (p->WriteData)(p, HexDat[c]);
	}
	return(stat);
}

static UINT8 ACM1602_PrHexInt(ACM1602 *p, UINT16 dat)
{
	UINT8 stat;
	stat = (p->PrHexByte)(p, (dat >> 8) & 0xff);
	if (stat != MI2C_WRITE_ERR) {
		stat = (p->PrHexByte)(p, dat& 0xff);
	}
	return(stat);
}

static UINT8 ACM1602_PrString(ACM1602 *p, char *sRamString)
{
	UINT8 stat,c;
	stat = !MI2C_WRITE_ERR;
	while ((c = *sRamString++) != '\0') {
		stat = (p->WriteData)(p,c);
		if (stat == MI2C_WRITE_ERR)
			break;
	}
	return(stat);
}

static UINT8 ACM1602_PrCString(ACM1602 *p, const char *sRomString)
{
	UINT8 stat,c;
	stat = !MI2C_WRITE_ERR;
	while ((c = *sRomString++) != '\0') {
		stat = (p->WriteData)(p,c);
		if (stat == MI2C_WRITE_ERR)
			break;
	}
	return(stat);
}



void ACM1602_Setup(
		ACM1602 *p,
		UINT8 bswadrs,
		UINT8 bswch,
		UINT8 devadrs,
		UINT8 option)
{
	p->Xpos = 0;
	p->Ypos = 0;
	p->Start = &ACM1602_Start;
	p->Position = &ACM1602_Position;
	p->Clear = &ACM1602_Clear;
	p->WriteData = &ACM1602_WriteData;
	p->PrHexByte = &ACM1602_PrHexByte;
	p->PrHexInt = &ACM1602_PrHexInt;
	p->PrString = &ACM1602_PrString;
	p->PrCString = &ACM1602_PrCString;

	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
