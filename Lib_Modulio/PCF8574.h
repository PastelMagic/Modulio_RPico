//---------------------------------------
//- PCF8574 Digital Input/Output	-
//---------------------------------------
//
#define	PCF8574_ADRS0	0x20
#define	PCF8574_ADRS1	0x21
#define	PCF8574_ADRS2	0x22
#define	PCF8574_ADRS3	0x23
#define	PCF8574_ADRS4	0x24
#define	PCF8574_ADRS5	0x25
#define	PCF8574_ADRS6	0x26
#define	PCF8574_ADRS7	0x27

typedef struct PCF8574_STRUC {
	MI2CADRS	madrs;
	UINT8	INPUT_Port;	// 入力ポートは'1'
	UINT8	INDATA;		// リードしたデータ
	UINT8	OUTDATA;	// 最後にライトしたデータ

	UINT8 (*Start)(struct PCF8574_STRUC *p);
	UINT8 (*Write)(struct PCF8574_STRUC *p, UINT8 dat);
	UINT8 (*Read)(struct PCF8574_STRUC *p);
} PCF8574;

#ifdef	EXTRN
#define	extern
#endif
extern void PCF8574_Setup(PCF8574 *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
#undef extern


