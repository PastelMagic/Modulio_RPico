//---------------------------------------
//--- RX8564 Real-Time Clock
//--- リアルタイムクロック
//---------------------------------------
#define EXTERN
#include "Modulio.h"
#undef	EXTERN

#include "RX8564.h"

static UINT8 RX8564_Start(RX8564 *p)
{
	return((p->madrs).WriteByte(&(p->madrs), 0, 0));
}

static UINT8 RX8564_ReadSec(RX8564 *p)
{
	UINT8 sts;
	sts = (p->madrs).ReadByte(&(p->madrs), 0x02, &(p->Sec));
	p->Sec &= 0x7f;
	return(sts);
}

static UINT8 RX8564_ReadMin(RX8564 *p)
{
	UINT8 sts;
	sts = (p->madrs).ReadByte(&(p->madrs), 0x03, &(p->Min));
	p->Min &= 0x7f;
	return(sts);
}

static UINT8 RX8564_ReadHour(RX8564 *p)
{
	UINT8 sts;
	sts = (p->madrs).ReadByte(&(p->madrs), 0x04, &(p->Hour));
	p->Hour &= 0x1f;
	return(sts);
}


void RX8564_Setup(RX8564 *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option)
{
	p->Sec  = 0;
	p->Min  = 0;
	p->Hour = 0;
	p->Start = &RX8564_Start;
	p->ReadSec = &RX8564_ReadSec;
	p->ReadMin = &RX8564_ReadMin;
	p->ReadHour = &RX8564_ReadHour;
	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
