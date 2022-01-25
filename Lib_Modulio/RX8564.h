//---------------------------------------
//- EPSON RX-8564 Real-Time-Clock	-
//---------------------------------------
//
typedef struct RX8564_STRUC {
	MI2CADRS	madrs;

	UINT8	Sec, Min, Hour;
	UINT8	(*Start)(struct RX8564_STRUC *p);
	UINT8	(*ReadSec)(struct RX8564_STRUC *p);
	UINT8	(*ReadMin)(struct RX8564_STRUC *p);
	UINT8	(*ReadHour)(struct RX8564_STRUC *p);
} RX8564;


#ifndef EXTERN
#define extern
#endif
extern void RX8564_Setup(RX8564 *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
#undef extern
