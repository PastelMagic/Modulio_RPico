//---------------------------------------
//- HDC1000 Humidity & Temp. Sensor	-
//---------------------------------------
//
#define	HDC1000_ADRS0	0x40
#define	HDC1000_ADRS1	0x41
#define	HDC1000_ADRS2	0x42
#define	HDC1000_ADRS3	0x43
typedef struct HDC1000_STRUC {
	MI2CADRS	madrs;
	UINT8	TEMP[2];
	UINT8	HUM[2];

	UINT8	(*Start)(struct HDC1000_STRUC *p);
	UINT8	(*Heater)(struct HDC1000_STRUC *p, UINT8 hon);
	UINT8	(*ReadTemp)(struct HDC1000_STRUC *p);
	UINT8	(*ReadHum)(struct HDC1000_STRUC *p);
	void	(*wConvTemp)(struct HDC1000_STRUC *p);
	void	(*wConvHum)(struct HDC1000_STRUC *p);
} HDC1000;

#ifdef	EXTRN
#define	extern
#endif
extern void HDC1000_Setup(HDC1000 *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
#undef extern
