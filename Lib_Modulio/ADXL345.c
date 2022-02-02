//---------------------------------------
//- ADXL345 3-Axis AccSensor		-
//- �R�������x�Z���T			-
//---------------------------------------
//
#define	EXTERN
#include "Modulio.h"
#undef EXTERN
#include "ADXL345.h"

//---------------------------------------
//--- Init Device
//--- �f�o�C�X������
//---------------------------------------
static UINT8 ADXL345_Start(ADXL345 *p)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), 0x2d, 0x08); // Start Measurement
	return(stat);
}

//---------------------------------------
//--- Read ID Reg. (0xE5�j
//--- ID���W�X�^�ǂݍ���
//---------------------------------------
//
static UINT8 ADXL345_GetID(ADXL345 *p)
{
	return((p->madrs).ReadByte(&(p->madrs), 0, &p->ID));
}

//---------------------------------------
//--- Read Accelero Data Reg.
//--- �����x�f�[�^���W�X�^�ǂݍ���
//---------------------------------------
static void ADXL345_GetVal(ADXL345 *p)
{
	UINT8 buf[2];
	(p->madrs).ReadBytes(&(p->madrs), 0x32, buf,2);
	p->xdat = ((buf[1] & 0x3) << 8) | buf[0];
	(p->madrs).ReadBytes(&(p->madrs), 0x34,buf,2);
	p->ydat = ((buf[1] & 0x3) << 8) | buf[0];
	(p->madrs).ReadBytes(&(p->madrs), 0x36,buf,2);
	p->zdat = ((buf[1] & 0x3) << 8) | buf[0];
}

//----------------------------------------
// Initialize ADXL345 Obuject
// ADXL345�I�u�W�F�N�g�̏�����
//----------------------------------------
//
void ADXL345_Setup(
		ADXL345 *p,
		UINT8 bswadrs,
		UINT8 bswch,
		UINT8 devadrs,
		UINT8 option)
{

	p->xdat = 0;
	p->ydat = 0;
	p->zdat = 0;
	p->Start  = &ADXL345_Start;
	p->GetID  = &ADXL345_GetID;
	p->GetVal = &ADXL345_GetVal;

	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
