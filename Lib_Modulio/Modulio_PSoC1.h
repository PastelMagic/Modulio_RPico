//=======================================
//= Modulio Device depend Library Header=
//=					=
//=======================================
//
#include "m8c.h"
#include "PSoCAPI.h"

#define EXTERN
#include "Modulio.h"
#undef EXTERN
#include "Modulio_HAL.h"

//=======================================
//= Low Level I2C API			=
//= �჌�x��API				=
//=					=
//=======================================

//---------------------------------------
//- Generate START condition on I2C	-
//- �X�^�[�g�R���f�B�V��������		-
//-					-
//- Return Value�i�߂�l)		-
//- ==0: Slave acknowleged		-
//-	�X���[�u����������		-
//- !=0: Slave didn't acknowlege	-
//-	�X���[�u���������Ȃ�����	-
//---------------------------------------
//
UINT8 MI2C_fSendStart(UINT8 bSlaveAddr, UINT8 fRW)
{
	UINT8 stat;
	stat = I2CHW_1_fSendStart(bSlaveAddr, fRW);
	if (stat == 0)
		stat = MI2C_NAKslave;
	else	stat = MI2C_ACKslave;
	return(stat);
}


//---------------------------------------
//- Generate Rep.START condition on I2C	-
//- ���s�[�e�B�b�h�X�^�[�g����		-
//-					-
//- Return Value�i�߂�l)		-
//- ==0: Slave acknowleged		-
//-	�X���[�u����������		-
//- !=0: Slave didn't acknowlege	-
//-	�X���[�u���������Ȃ�����	-
//---------------------------------------
//
UINT8 MI2C_fSendRepStart(UINT8 bSlaveAddr, UINT8 fRW)
{
	UINT8 stat;
	stat = I2CHW_1_fSendRepeatStart(bSlaveAddr, fRW);
	if (stat == 0)
		stat = MI2C_NAKslave;
	else	stat = MI2C_ACKslave;
	return(stat);
}

//---------------------------------------
//- Generate STOP condition on I2C	-
//- �X�g�b�v�R���f�B�V��������		-
//-					-
//- Return Value�i�߂�l)		-
//-	None				-
//-	�i�Ȃ��j			-
//---------------------------------------
//
void MI2C_SendStop(void)
{
	I2CHW_1_SendStop();
}

//---------------------------------------
//- Send a single-byte on I2C bus 	-
//- �P�o�C�g���M			-
//-					-
//- Return Value�i�߂�l)		-
//- !=0x00: Slave faild to acknowlege	-
//-	�X���[�u�ɃG���[����		-
//---------------------------------------
//
UINT8 MI2C_fWrite(UINT8 bData)
{
	UINT8 stat;
	stat = I2CHW_1_fWrite(bData);
	if (stat == 0)
		stat = MI2C_NAKslave;
	else	stat = MI2C_ACKslave;
	return(stat);
}


//---------------------------------------
//- Read a single-byte from I2C bus 	-
//- �P�o�C�g��M			-
//-					-
//- fACK				-
//- ==MI2C_NAKslave(==0x00)		-
//-   Generate NAK after recieving data	-
//-�@ �]���I�����NAK�����i���[�h�I���j	-
//- ==MI2C_ACKslave(!=0x00)		-
//-   Generate ACK after recieving data	-
//-�@ �]���I�����ACK�����i���[�h�p���j	-
//-					-
//- Return Value�i�߂�l)		-
//-	Data received from slave	-
//-	�ǂݎ�����f�[�^		-
//---------------------------------------
//
UINT8 MI2C_bRead(UINT8 fACK)
{
	UINT8 data;
	data = I2CHW_1_bRead(fACK);
	return(data);
}


// Create pragmas to support proper argument and return value passing
#pragma fastcall16  Delay50uTimes
#pragma fastcall16  Delay50u
#pragma fastcall16  Delay10msTimes
//-------------------------------------------------
// Prototypes of the delay API.
//-------------------------------------------------
extern void Delay50u(void);
extern void Delay50uTimes(BYTE Multiple);
extern void Delay10msTimes(BYTE TenMstimes);
//=======================================
//= Delay N(ms)				=
//= N�~���b�f�B���C			=
//=======================================
void MI2C_WaitMills(UINT16 delay)
{
	if (delay) {
		while(--delay) {
			Delay50uTimes(20);
		}
	}
}


//---------------------------------------
//- MI2C Startup			-
//- MI2C�̃X�^�[�g�A�b�v		-
//- Return Value�i�߂�l)		-
//-	None				-
//-	�i�Ȃ��j			-
//---------------------------------------
//
void MI2C_Startup(void)
{
	M8C_EnableGInt;
	I2CHW_1_Start();
	I2CHW_1_EnableMstr();
	I2CHW_1_EnableInt();
}

