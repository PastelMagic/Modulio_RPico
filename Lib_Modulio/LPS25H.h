//---------------------------------------
//--- LPS25H Pressure Sensor		-
//--- ‹Cˆ³ƒZƒ“ƒT
//---------------------------------------
//
#define	LPS25H_ADRS0 0x5c	// 101 110x
#define	LPS25H_ADRS1 0x5d	// x: SDO/SA0 (Pin#4) VDD('1')/GND('0')

#define	LPS25H_ID	0xbd	// LPS25H ID (Identify LPS25H)

typedef struct LPS25H_STRUC {
	MI2CADRS	madrs;
	UINT8		ID;
	UINT8		STS;
	UINT8		PRESS[3];
	UINT8		TEMP[2];
	UINT8		STATUS;

	UINT8		(*Start)(struct LPS25H_STRUC *p);
	UINT8		(*ReadPress)(struct LPS25H_STRUC *p);
	void		(*wConvPress)(struct LPS25H_STRUC *p);
	UINT8		(*ReadTemp)(struct LPS25H_STRUC *p);
	void		(*wConvTemp)(struct LPS25H_STRUC *p);
	UINT8		(*ReadStatus)(struct LPS25H_STRUC *p);
} LPS25H;

#ifndef EXTERN
#define extern
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern void LPS25H_Setup(LPS25H *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
#ifdef __cplusplus
}
#endif
#undef extern

