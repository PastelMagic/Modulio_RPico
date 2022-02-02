//---------------------------------------
//- LPS25H Pressure Sensor
//- ���x�E�C���Z���T
//---------------------------------------
#define	EXTERN
#include "Modulio.h"
#undef EXTERN
#include "LPS25H.h"

//-------------------------------------------
//--- Initialize Device
//--- �f�o�C�X������
//-------------------------------------------
static UINT8	LPS25H_Start(LPS25H *p)
{
	UINT8	sts;
	sts = (p->madrs).ReadByte(&(p->madrs), 0x0f, &(p->ID));
	if (sts != MI2C_ACKslave)
		return(sts);
	if (p->ID != LPS25H_ID)
		return(sts);
	sts = (p->madrs).WriteByte(&(p->madrs), 0x20, 0x90);
	return(sts);
}

//-------------------------------------------
//--- Read PRESSOUT(Pressure) Registers
//--- 0x28(PRESS[0]:LSB) - 0x2A(PRESS[2]:MSB)
//--- 0x28����0x2a��3�o�C�g���C���i0x28��LSB)
//-------------------------------------------
static UINT8	LPS25H_ReadPress(LPS25H *p)
{
	UINT8 c,sts;
	for (c=0; c<3; c++) {
		sts = (p->madrs).ReadByte(&(p->madrs), 0x28+c, &(p->PRESS[c]));
		if (sts != MI2C_ACKslave)
			break;
	}
	return(sts);
}

//---------------------------------------------------
//--- Convert PRESS Reg.Value
//--- PRESS[0](MSB)-PRESS[1][LSB):Integer Part
//--- PRESS[2]:Fractional Part
//--- PRESS[0]-[2]�̒l�𐮐������Ə����_�ȉ��ɕ���
//---------------------------------------------------
static void	LPS25H_wConvPress(LPS25H *p)
{
	UINT16	frac;
	frac = ((p->PRESS[1] & 0xf) << 8) | p->PRESS[0];	//*** PRESS[0]:LSB PRESS[2]:MSB
	frac = (frac * 100) >> 12;
	p->PRESS[1] =((p->PRESS[2] & 0xf)<<4) | (p->PRESS[1] >>4);
	p->PRESS[0] = p->PRESS[2] >> 4;
	p->PRESS[2] = frac & 0xff;
}

//-------------------------------------------
//--- Read Temp(TEMP_OUT_L/H) Register
//--- TEMP_OUT���W�X�^�ŉ��x�ǂݍ���
//-------------------------------------------
static UINT8	LPS25H_ReadTemp(LPS25H *p)
{
	UINT8 c,sts;
	for (c=0; c<2; c++) {
		sts = (p->madrs).ReadByte(&(p->madrs), 0x2b+c, &(p->TEMP[c]));
		if (sts != MI2C_ACKslave)
			break;
	}
	return(sts);
}

//-------------------------------------------
//--- Convert Temp. Data
//--- TEMP[0]:Integer Part
//--- TMEP[1]:Fractional Part
//--- 42.5+(TEMP_OUT/480)
//--- ���W�X�^�l���珬���_���x�f�[�^�ɕϊ�
//--- TEMP[0]�F��������
//--- TEMP[1]�F�����_�ȉ�
//-------------------------------------------
static void	LPS25H_wConvTemp(LPS25H *p)
{
	UINT16 idat,fdat;
	UINT16 dat;
	dat = (p->TEMP[1] << 8) | p->TEMP[0];
	if ((dat & 0x8000) && (dat < 0xb050)) {
		dat = (((20400 + dat) & 0xffff)^0xffff)+1;
		idat = 0x80;
	} else {
		dat = (20400 + dat) & 0xffff;
		idat = 0;
	}
	idat += (dat / 480) & 0x7f;
	dat -= idat*480;
	fdat = dat*5/24;
	p->TEMP[0] = (UINT8)idat;
	p->TEMP[1] = (UINT8)fdat;
}

//-------------------------------------------
//--- Read Status Register
//--- �X�e�[�^�X���W�X�^�ǂݍ���
//-------------------------------------------
static UINT8	LPS25H_ReadStatus(LPS25H *p)
{
	UINT8 sts;
	sts = (p->madrs).ReadByte(&(p->madrs), 0x27, &(p->STS));		//*** Adrs for StatusReg is 0x28
	return(sts);
}

//-------------------------------------------
//--- Initialize Structure
//--- �\���̂̏�����
//-------------------------------------------
void LPS25H_Setup(
	LPS25H *p,
	UINT8 bswadrs,
	UINT8 bswch,
	UINT8 devadrs,
	UINT8 option)
{
	p->Start = &LPS25H_Start;
	p->ReadPress = &LPS25H_ReadPress;
	p->wConvPress = &LPS25H_wConvPress;
	p->ReadTemp = &LPS25H_ReadTemp;
	p->wConvTemp = &LPS25H_wConvTemp;
	p->ReadStatus = &LPS25H_ReadStatus;

	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
