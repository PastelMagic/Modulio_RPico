//=======================================
//= Modulio Device depend Library Header=
//=					=
//=======================================
//

#define EXTERN
#include "Modulio.h"
#undef EXTERN
#include "Modulio_HAL.h"

#include "Mbed_Hookup.h"

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
	Mbed_i2c_start();
	return(Mbed_i2c_write((bSlaveAddr<<1) | fRW));
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
	Mbed_i2c_stop();
	Mbed_i2c_start();
	return(Mbed_i2c_write((bSlaveAddr<<1) | fRW));
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
	Mbed_i2c_stop();
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
	int sts;
	sts = Mbed_i2c_write(bData);
	if (sts == MI2C_ACKslave)	
		return(MI2C_ACKslave);
	else	return(MI2C_NAKslave);
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
	data = (UINT8)Mbed_i2c_read(fACK);
	return(data);
}



//=======================================
//= Delay N(ms)				=
//= N�~���b�f�B���C			=
//=======================================
void MI2C_WaitMills(UINT16 delay)
{
	Mbed_Waitms(delay);
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
	Mbed_Startup();
}

