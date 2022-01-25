//---------------------------------------
//- PCA9632 4-Channel LED Driver	-
//-					-
//---------------------------------------
//
#define	EXTERN
#include "Modulio.h"
#undef EXTERN
#include "PCA9632.h"



static UINT8 PCA9632_DRVMode(PCA9632 *p, UINT8 mode)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), 0x01, mode); // MODE2
	return(stat);
}

//
// LED Mode control
//
static UINT8 PCA9632_LEDMode(PCA9632 *p, UINT8 mode)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), 0x08, mode); // LED output mode
	return(stat);
}


static UINT8 PCA9632_Start(PCA9632 *p, UINT8 drvmode, UINT8 ledmode)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), 0x00, 0); // MODE1
	if (stat == MI2C_WRITE_ERR)
		return(stat);
	stat = PCA9632_DRVMode(p, drvmode);	// MODE2
	if (stat == MI2C_WRITE_ERR)
		return(stat);
	stat = PCA9632_LEDMode(p, ledmode);	// LED OUTPUT MODE
	return(stat);
}


//
// LED Brightness control
//
static UINT8 PCA9632_Bright(PCA9632 *p, UINT8 ch, UINT8 dat)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), (ch & 3)+2, dat);
	return(stat);
}

//
// LED Global Dimmer/Blink control
//
static UINT8 PCA9632_DimmBlink(PCA9632 *p, UINT8 pwm, UINT8 freq)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), 0x06, pwm);
	if (stat == MI2C_WRITE_ERR)
		return(stat);
	stat = (p->madrs).WriteByte(&(p->madrs), 0x07, freq);
	return(stat);
}

void PCA9632_Setup(
		PCA9632 *p,
		UINT8 bswadrs,
		UINT8 bswch,
		UINT8 devadrs,
		UINT8 option)
{

	p->Start = &PCA9632_Start;
	p->DRVMode = &PCA9632_DRVMode;
	p->LEDMode = &PCA9632_LEDMode;
	p->Bright = &PCA9632_Bright;
	p->DimmBlink = &PCA9632_DimmBlink;
	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
