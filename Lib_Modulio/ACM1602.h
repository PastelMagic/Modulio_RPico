//---------------------------------------
//--- ACM1602 16x2 LCD module
//--- ƒLƒƒƒ‰ƒNƒ^LCD
//---------------------------------------
//
#define ACM1602_ADRS0	0x50
typedef struct ACM1602_STRUC {
	MI2CADRS	madrs;

	UINT8	Xpos, Ypos, DDRAM_Adrs;

	UINT8	(*Start)(struct ACM1602_STRUC *p);
	void	(*Position)(struct ACM1602_STRUC *p, UINT8 y, UINT8 x);
	UINT8	(*Clear)(struct ACM1602_STRUC *p);
	UINT8	(*WriteData)(struct ACM1602_STRUC *p, UINT8 dat);
	UINT8	(*PrHexByte)(struct ACM1602_STRUC *p, UINT8 dat);
	UINT8	(*PrHexInt)(struct ACM1602_STRUC *p, UINT16 dat);
	UINT8	(*PrString)(struct ACM1602_STRUC *p, char *sRamString);
	UINT8	(*PrCString)(struct ACM1602_STRUC *p, const char *sRomString);
} ACM1602;

#ifndef EXTERN
#define extern
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern void ACM1602_Setup(ACM1602 *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
#ifdef __cplusplus
}
#endif
#undef extern
