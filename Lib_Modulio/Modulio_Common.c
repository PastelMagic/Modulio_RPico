//=======================================
//= Modulio Common Library		=
//=					=
//=======================================
//
#include "Modulio.h"
#define EXTERN
#include "Modulio_HAL.h"
#undef EXTERN

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
static UINT8 MI2C_bWriteBytes(UINT8 bSlaveAddr, UINT8 *pbXferData, UINT8 bCnt, UINT8 bMode)
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
//- ==MI2C_NAKslave(0x00)		-
//-	Error occured on I2C bus	-
//-	�ǂݍ��ݒ��ɃG���[����		-
//- == MI2C_ACKslave			-
//-	Xfr complete with no error	-
//-	����I��			-
//---------------------------------------
//
static UINT8 MI2C_fReadBytes(UINT8 bSlaveAddr, UINT8 *pbXferData, UINT8 bCnt, UINT8 bMode)
{
	UINT8 stat;
	UINT8 counts;
	stat = MI2C_ACKslave;
	if ((bSlaveAddr & 0x80)==0x00) {	// Send SlaveAddr if it is valid(0x00-0x7f)
						// �X���[�u�A�h���X��0x00�`0x7f�Ȃ�A�h���X�w��
		if (bMode & MI2C_RepStart)
			stat = MI2C_fSendRepStart(bSlaveAddr, MI2C_READ);
		else	stat = MI2C_fSendStart(bSlaveAddr, MI2C_READ);
		if (stat != MI2C_ACKslave) {
			MI2C_SendStop();
			return(stat);
		}
	}
	for (counts = 1; counts < bCnt; counts++) {
		*pbXferData++ = MI2C_bRead(MI2C_fACK);
	}
	*pbXferData = MI2C_bRead(MI2C_fNAK);
	MI2C_SendStop();
	return(stat);
}

//=======================================
//= Public (High Level) I2C API		=
//= ���J�i�����x���jAPI			=
//=					=
//=======================================
//


//---------------------------------------
//- Write multi-byte data on I2C	-
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
UINT8 MI2C_WriteBytes(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length)
{
	UINT8 stat,reg;
	reg = Reg & 0xff;
	if (adrs->BSW_ADRS != 0) {	// Setup channel of bus switch
					// �o�X�X�C�b�`�̃`�����l���ݒ�
		stat = 1 << ((adrs->BSW_CH) & 0x3);
		stat = MI2C_bWriteBytes(adrs->BSW_ADRS, &stat, 1, MI2C_CompleteXfer);
		if (stat != MI2C_ACKslave)
			return(stat);
	}
	if (Reg != REG_ADRS_NONE) {	// Register number is valid
					// ���W�X�^�ԍ����w�肳��Ă���Ƃ�
		stat = MI2C_bWriteBytes(adrs->DEV_ADRS,	// Send register number
				 &reg,			// ���W�X�^�ԍ�������
				 1,
				 MI2C_NoStop);
		if (stat != MI2C_ACKslave)
			return(stat);
		stat = DEV_ADRS_NONE;
	} else {
		stat = adrs->DEV_ADRS;
	}
	stat = MI2C_bWriteBytes(	// Perform write operation
			stat,		// �f�[�^�����ݎ��s
			Data,
			Length,
			MI2C_CompleteXfer);
	return(stat);
}


//---------------------------------------
//- Write a single-byte data on I2C	-
//- 1�o�C�g������			-
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
UINT8 MI2C_WriteByte(MI2CADRS *adrs, UINT16 Reg, UINT8 Data)
{
	return(MI2C_WriteBytes(adrs, Reg, &Data, 1));
}


//---------------------------------------
//- Read a multi-byte data from I2C	-
//- �����o�C�g�ǂݍ���			-
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
UINT8 MI2C_ReadBytes(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length)
{
	UINT8 Mode,reg,stat;
	reg = Reg & 0xff;
	if (adrs->BSW_ADRS != 0) {	// Setup channel number of bus switch
					// �`�����l���ԍ��ݒ�
		stat = 1 << ((adrs->BSW_CH) & 0x3);
		stat = MI2C_bWriteBytes(adrs->BSW_ADRS, &stat, 1, MI2C_CompleteXfer);
		if (stat != MI2C_ACKslave)
			return(stat);
	}
	if (Reg != REG_ADRS_NONE) {	// Write register address if it is valid
					// ���W�X�^�A�h���X�w�肪����Ȃ珑����
		stat = MI2C_bWriteBytes(adrs->DEV_ADRS,	// Send register number
				 &reg,			// ���W�X�^�ԍ�������
				 1,
				 MI2C_NoStop);
		if (stat != MI2C_ACKslave)
			return(stat);
		Mode = MI2C_RepStart;	// REP.START should be used for following read operation
					// �������[�h�����REPEATED START�R���f�B�V����
	} else {
		Mode = MI2C_CompleteXfer;	// START should be used for following read operation
	}					// �������[�h�����START
	stat = MI2C_fReadBytes(adrs->DEV_ADRS,	// Perform read operation
			Data,			// ���[�h���s
			Length,
			Mode);
	return(stat);
}


//---------------------------------------
//- Read a single-byte data from I2C	-
//- 1�o�C�g�ǂݍ���			-
//-					-
//- Return Value�i�߂�l)		-
//-	None				-
//-	�i�Ȃ��j			-
//---------------------------------------
//
UINT8 MI2C_ReadByte(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data)
{
	UINT8 sts;
	sts = MI2C_ReadBytes(adrs, Reg, Data, 1);
	return(sts);
}


//---------------------------------------
//- Setup MI2CADRS database		-
//- MI2CADRS�̏�����			-
//- Return Value�i�߂�l)		-
//-	None				-
//-	�i�Ȃ��j			-
//---------------------------------------
//
void MI2C_Setup(MI2CADRS *adrs,
		UINT8 bswadrs,
		UINT8 bswch,
		UINT8 devadrs,
		UINT8 option)
{
	adrs->BSW_ADRS = bswadrs;
	adrs->BSW_CH = bswch;
	adrs->DEV_ADRS = devadrs;
	adrs->OPTION_1 = option;
	adrs->WriteBytes = &MI2C_WriteBytes;
	adrs->WriteByte = &MI2C_WriteByte;
	adrs->ReadBytes = &MI2C_ReadBytes;
	adrs->ReadByte = &MI2C_ReadByte;
	return;
}

//---------------------------------------
//- Initialize I2C hardware		-
//- I2C�n�[�h�E�F�A�̏�����		-
//- Return Value�i�߂�l)		-
//-	None				-
//-	�i�Ȃ��j			-
//---------------------------------------
//
void MI2C_Start(void)
{
	MI2C_Startup();
}


//=======================================
//= Delay N(ms)				=
//= N�~���b�f�B���C			=
//=======================================
void MI2C_Waitms(UINT16 delay)
{
	MI2C_WaitMills(delay);
}

