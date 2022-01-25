//******************************
// Modulio Common Library Header
//
//******************************
//
#ifndef UINT8
#define	UINT8	unsigned char
#endif

#ifndef UINT16
#define	UINT16	unsigned short
#endif

#ifndef	UINT32
#define	UINT32	unsigned long
#endif

#ifndef INT8
#define	INT8	char
#endif

#ifndef INT16
#define	INT16	short
#endif

#ifndef	INT32
#define	INT32	long
#endif

#define	MI2C_WRITE	0x00
#define	MI2C_READ	0x01
#define	MI2C_fACK	0x01
#define	MI2C_fNAK	0x00
#define	MI2C_ACKslave	0x00
#define	MI2C_NAKslave	0x01

#define	MI2C_WRITE_ERR	0xff


#define	BSW_ADRS_NONE		0x00
#define	BSW_ADRS0		0x70
#define	BSW_ADRS1		0x71
#define	BSW_ADRS2		0x72
#define	BSW_ADRS3		0x73
#define	BSW_ADRS4		0x74
#define	BSW_ADRS5		0x75
#define	BSW_ADRS6		0x76
#define	BSW_ADRS7		0x77
#define	BSW_ADRS_DEFAULT	BSW_ADRS4

#define	REG_ADRS_NONE		0xffff
#define	DEV_ADRS_NONE		0xff

// bMode Value
#define MI2C_CompleteXfer   0x00 // Perform a complete I2C transfer
#define MI2C_RepStart       0x01 // Perform a complete transfer with a Repeat Start
#define MI2C_NoStop         0x02 // Perform a complete transfer, but leave off the stop
#define MI2C_NAKnextWr      0x04 // Flag to tell slave to NAK next byte from master


typedef struct MI2CADRS_STRUC {
	UINT8	BSW_ADRS;
	UINT8	BSW_CH;
	UINT8	DEV_ADRS;
	UINT8	OPTION_1;
	UINT8	(*WriteBytes)(struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length);
	UINT8	(*WriteByte)(struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 Dat);
	UINT8	(*ReadBytes) (struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length);
	UINT8	(*ReadByte) (struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 *Data);
} MI2CADRS;


#ifndef EXTERN
#define	extern
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern UINT8 MI2C_WriteBytes(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length);
extern UINT8 MI2C_ReadBytes(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length);
extern UINT8 MI2C_WriteByte(MI2CADRS *adrs, UINT16 Reg, UINT8 Data);
extern UINT8 MI2C_ReadByte(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data);
extern void  MI2C_Setup(MI2CADRS *adrs, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
extern void MI2C_Start(void);
extern void MI2C_Waitms(UINT16 delay);
#ifdef __cplusplus
}
#endif
#undef extern
