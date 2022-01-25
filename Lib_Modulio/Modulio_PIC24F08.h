//=======================================
//= Modulio Common Library		=
//= For PIC24F08KL200			=
//=======================================
//
#include <xc.h>
#define FCY 2000000UL
#include <libpic30.h>
//#include "PIC24F08.h"

#include "Modulio.h"
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
//- !=0: Slave acknowleged		-
//-	�X���[�u����������		-
//- ==0: Slave didn't acknowlege	-
//-	�X���[�u���������Ȃ�����	-
//---------------------------------------
//
UINT8 MI2C_fSendStart(UINT8 bSlaveAddr, UINT8 fRW)
{
	UINT8 stat;
	SSP1CON2bits.SEN = 1;		// Initiate START Condition
	while(SSP1CON2bits.SEN)	// Wait until clear
		;
	SSP1BUF = (bSlaveAddr << 1) | fRW;
	while(SSP1STATbits.R_W)	// Wait until Transmition complete
		;
	stat = (SSP1CON2bits.ACKSTAT) ? MI2C_NAKslave: MI2C_ACKslave;
	return(stat);
}


//---------------------------------------
//- Generate Rep.START condition on I2C	-
//- ���s�[�e�B�b�h�X�^�[�g����		-
//-					-
//- Return Value�i�߂�l)		-
//- !=0: Slave acknowleged		-
//-	�X���[�u����������		-
//- ==0: Slave didn't acknowlege	-
//-	�X���[�u���������Ȃ�����	-
//---------------------------------------
//
UINT8 MI2C_fSendRepStart(UINT8 bSlaveAddr, UINT8 fRW)
{
	UINT8 stat;
	SSP1CON2bits.RSEN = 1;		// Initiate REP.START Condition
	while(SSP1CON2bits.RSEN)	// Wait until clear
		;
	SSP1BUF = (bSlaveAddr << 1) | fRW;
	while(SSP1STATbits.R_W)		// Wait until Transmition complete
		;
	stat = (SSP1CON2bits.ACKSTAT) ? MI2C_NAKslave: MI2C_ACKslave;
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
	SSP1CON2bits.PEN = 1;		// Initiate STOP Condition
	while(SSP1CON2bits.PEN)		// Wait until clear
		;
}

//---------------------------------------
//- Send a single-byte on I2C bus 	-
//- �P�o�C�g���M			-
//-					-
//- Return Value�i�߂�l)		-
//- ==0xff: Slave faild to acknowlege	-
//-	�X���[�u�ɃG���[����		-
//---------------------------------------
//
UINT8 MI2C_fWrite(UINT8 bData)
{
	UINT8 stat;
	SSP1BUF = bData;	// Write Data
	while(SSP1STATbits.R_W)	// Wait until Transmition complete
		;
	stat = (SSP1CON2bits.ACKSTAT) ? MI2C_NAKslave: MI2C_ACKslave;
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
	SSP1CON2bits.RCEN = 1;		// Receive Function Enable
	while(SSP1CON2bits.RCEN)
		;
	data = SSP1BUF;			// Read Data;
	if (fACK == MI2C_fNAK) {
		SSP1CON2bits.ACKDT = 1;
		SSP1CON2bits.ACKEN = 1;	// Send NAK (Stop reading)
	} else {
		SSP1CON2bits.ACKEN = 1;			// Send ACK (Continue reading)
	}
	while(SSP1CON2bits.ACKEN)
		;
	return(data);
}


//=======================================
//= Middle Level I2C API		=
//= ���ԃ��x��API			=
//=					=
//=======================================
//

//---------------------------------------
//- Write multiple data on I2C		-
//- �����o�C�g������			-
//-					-
//- Return Value�i�߂�l)		-
//- ==MI2C_NAKslave(0x00)		-
//-	Error occured on I2C bus	-
//-	�������ݒ��ɃG���[����		-
//- == MI2C_ACKslave			-
//-	Xfr complete with no error	-
//-	����I��			-
//---------------------------------------
//
UINT8 MI2C_bWriteBytes(UINT8 bSlaveAddr, UINT8 *pbXferData, UINT8 bCnt, UINT8 bMode)
{
	UINT8 stat;
	UINT8 counts;
	stat = MI2C_ACKslave;
	if ((bSlaveAddr & 0x80)==0x00) {	// Send SlaveAddr if it is valid(0x00-0x7f)
						// �X���[�u�A�h���X��0x00�`0x7f�Ȃ�A�h���X�w��
		if (bMode & MI2C_RepStart)
			stat = MI2C_fSendRepStart(bSlaveAddr, MI2C_WRITE);
		else	stat = MI2C_fSendStart(bSlaveAddr, MI2C_WRITE);
		if (stat != MI2C_ACKslave) {
			MI2C_SendStop();
			return(stat);
		}
	}
	for (counts = 0; counts < bCnt; counts++) {
		stat = MI2C_fWrite(*pbXferData);
		if (stat != MI2C_ACKslave)
			break;
	}

	if (bMode & MI2C_NoStop)
		;
	else	MI2C_SendStop();
	return(stat);
}


//---------------------------------------
//- Read multiple data on I2C		-
//- �����o�C�g�ǂݍ���			-
//-					-
//- Return Value�i�߂�l)		-
//-	None				-
//-	�i�Ȃ��j			-
//---------------------------------------
//
void MI2C_fReadBytes(UINT8 bSlaveAddr, UINT8 *pbXferData, UINT8 bCnt, UINT8 bMode)
{
	UINT8 stat;
	UINT8 counts;
	if ((bSlaveAddr & 0x80)==0x00) {	// Send SlaveAddr if it is valid(0x00-0x7f)
						// �X���[�u�A�h���X��0x00�`0x7f�Ȃ�A�h���X�w��
		if (bMode & MI2C_RepStart)
			stat = MI2C_fSendRepStart(bSlaveAddr, MI2C_READ);
		else	stat = MI2C_fSendStart(bSlaveAddr, MI2C_READ);

		if (stat != MI2C_ACKslave) {
			MI2C_SendStop();
			return;
		}
	}
	for (counts = 1; counts < bCnt; counts++) {
		*pbXferData++ = MI2C_bRead(MI2C_fACK);
	}
	*pbXferData = MI2C_bRead(MI2C_fNAK);
	MI2C_SendStop();
	return;
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
	SSP1CON1 = 0x0028;		// 0b00101000;
	SSP1CON2 = 0x0000;
	SSP1CON3 = 0x0000;		// CON3_SDAH;		// Long Hold time
	SSP1ADD  = 39;			// FCY/(2*I2CCLK)-1;	// 8000/(2*100)-1 = 39

	//SSP1MSK  = 0x0000;
	SSP1STAT = 0x0000;		// Slew Rate Control Disable
	//PADCFG1  = 0x0000;		// Implemented only on PIC24FXXKL40X/30X device
}

//=======================================
//= Delay N(ms)				=
//= N�~���b�f�B���C			=
//=======================================
void MI2C_WaitMills(UINT16 delay)
{
	__delay_ms((unsigned long)delay);
}
