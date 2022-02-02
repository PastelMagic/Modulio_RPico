//---------------------------------------
//--- ADXL345 3-Axis AccSensor
//--- 三軸加速度センサ
//---------------------------------------
//
#define	ADXL345_ADRS0	0x1d
#define	ADXL345_ADRS1	0x53

#define	ADXL345_ID	0xe5	// ADXL345 ID (Identify ADXL345)
typedef struct ADXL345_STRUC {
	MI2CADRS	madrs;
	UINT8		ID;
	UINT16		xdat;	// X-axis Data
	UINT16		ydat;	// Y-axis Data
	UINT16		zdat;	// Z-axis Data

	UINT8	(*Start)(struct ADXL345_STRUC *p);
	UINT8	(*GetID)(struct ADXL345_STRUC *p);
	void	(*GetVal)(struct ADXL345_STRUC *p);	// Get Acc. Data
} ADXL345;

#ifndef EXTERN
#define extern
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern void ADXL345_Setup(ADXL345 *p, UINT8 bswadrs, UINT8 bswch, UINT8 devadrs, UINT8 option);
#ifdef __cplusplus
}
#endif
#undef extern

