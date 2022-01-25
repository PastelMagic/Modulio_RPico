//---------------------------------------
//- PCA9632 4-Channel LED Driver	-
//---------------------------------------
//
#define	PCA9632_ADRS0	0x60
#define	PCA9632_ADRS1	0x61
#define	PCA9632_ADRS2	0x62
#define	PCA9632_ADRS3	0x63
typedef struct PCA9632_STRUC {
	MI2CADRS	madrs;
	UINT8	(*Start)(struct PCA9632_STRUC *p, UINT8 mode2, UINT8 ledmode);
	UINT8	(*DRVMode)(struct PCA9632_STRUC *p, UINT8 mode);
	UINT8	(*LEDMode)(struct PCA9632_STRUC *p, UINT8 mode);
	UINT8	(*Bright)(struct PCA9632_STRUC *p, UINT8 ch, UINT8 dat);
	UINT8	(*DimmBlink)(struct PCA9632_STRUC *p, UINT8 pwm, UINT8 freq);
} PCA9632;

#ifndef EXTERN
#define extern
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern void PCA9632_Setup(PCA9632 *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
#ifdef __cplusplus
}
#endif
#undef extern

