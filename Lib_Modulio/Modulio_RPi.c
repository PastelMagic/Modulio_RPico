//=======================================
//= Modulio Common Library		=
//=					=
//=======================================
//
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "Modulio.h"
#define EXTERN
#include "Modulio_HAL.h"
#undef EXTERN


//=======================================
//= Public (High Level) I2C API		=
//= 公開（高レベル）API			=
//=					=
//=======================================
//

//---------------------------------------
//- Write multi-byte data on I2C	-
//- 複数バイト書込み			-
//-					-
//- Return Value（戻り値)		-
//- ==MI2C_NAKslave(0x00)		-
//-	Error occured on I2C bus	-
//-	書き込み中にエラー発生		-
//- == MI2C_ACKslave			-
//-	Xfr complete with no error	-
//-	正常終了			-
//---------------------------------------
//

UINT8 MI2C_WriteBytes(struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length)
{
	UINT8 tmp,reg;
	UINT8 tmpbuf[257];
	reg = Reg & 0xff;
	if (adrs->BSW_ADRS != 0) {	// Setup channel of bus switch
					// バススイッチのチャンネル設定
		tmp = 1 << ((adrs->BSW_CH) & 0x3);
		if (write(adrs->BSW_FD, &tmp, 1) < 0)
			return(MI2C_NAKslave);
	}
	if (Reg != REG_ADRS_NONE) {	// Register number is valid
					// レジスタ番号が指定されているとき
		tmpbuf[0] = reg;
		for (tmp=0; tmp<Length; tmp++)
			tmpbuf[tmp+1] = *Data++;
		if (write(adrs->DEV_FD, tmpbuf, Length+1) < 0)
			return(MI2C_NAKslave);
	} else {
		if (Length > 0) {
			if (write(adrs->DEV_FD, Data, Length) < 0)
				return(MI2C_NAKslave);
		}
	}
	return(MI2C_ACKslave);
}

//---------------------------------------
//- Write a single-byte data on I2C	-
//- 1バイト書込み			-
//-					-
//- Return Value（戻り値)		-
//- ==MI2C_NAKslave(0x00)		-
//-	Error occured on I2C bus	-
//-	書き込み中にエラー発生		-
//- == MI2C_ACKslave			-
//-	Xfr complete with no error	-
//-	正常終了			-
//---------------------------------------
//
UINT8 MI2C_WriteByte(struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 Data)
{
	return(MI2C_WriteBytes(adrs, Reg, &Data, 1));
}


//---------------------------------------
//- Read a multi-byte data from I2C	-
//- 複数バイト読み込み			-
//-					-
//- Return Value（戻り値)		-
//- ==MI2C_NAKslave(0x00)		-
//-	Error occured on I2C bus	-
//-	書き込み中にエラー発生		-
//- == MI2C_ACKslave			-
//-	Xfr complete with no error	-
//-	正常終了			-
//---------------------------------------
//
UINT8 MI2C_ReadBytes(struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length)
{
	UINT8 Mode,reg,tmp;
	reg = Reg & 0xff;
	if (adrs->BSW_ADRS != 0) {	// Setup channel number of bus switch
					// チャンネル番号設定
		tmp = 1 << ((adrs->BSW_CH) & 0x3);
		write(adrs->BSW_FD, &tmp, 1);
	}
	if (Reg != REG_ADRS_NONE) {	// Write register address if it is valid
					// レジスタアドレス指定があるなら書込む
		if (write(adrs->DEV_FD, &reg, 1) < 0)
			return(MI2C_NAKslave);
		if (Length > 0) {
			if (read(adrs->DEV_FD, Data, Length) < 0)
				return(MI2C_NAKslave);
		}
					// 続くリード動作はREPEATED STARTコンディション
	} else {
		if (Length > 0) {
			if (read(adrs->DEV_FD, Data, Length) < 0)
				return(MI2C_NAKslave);
		}
	}
	return(MI2C_ACKslave);
}


//---------------------------------------
//- Read a single-byte data from I2C	-
//- 1バイト読み込み			-
//-					-
//- Return Value（戻り値)		-
//-	None				-
//-	（なし）			-
//---------------------------------------
//
UINT8 MI2C_ReadByte(struct MI2CADRS_STRUC *adrs, UINT16 Reg, UINT8 *Data)
{
	UINT8 stat;
	stat = MI2C_ReadBytes(adrs, Reg, Data, 1);
	return(stat);
}


//---------------------------------------
//- Setup MI2CADRS database		-
//- MI2CADRSの初期化			-
//- Return Value（戻り値)		-
//-	None				-
//-	（なし）			-
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

	if (bswadrs != 0) {
		adrs->BSW_FD = wiringPiI2CSetup(bswadrs);
	}
	adrs->DEV_FD = wiringPiI2CSetup(devadrs);
	return;
}

//---------------------------------------
//- Initialize I2C hardware		-
//- I2Cハードウェアの初期化		-
//- Return Value（戻り値)		-
//-	None				-
//-	（なし）			-
//---------------------------------------
//
void MI2C_Start(void)
{
	system("sudo modprobe i2c_dev");
	system("sudo modprobe -r i2c_bcm2708");
	system("sudo modprobe i2c_bcm2708 baudrate=50000");
}


//=======================================
//= Delay N(ms)				=
//= Nミリ秒ディレイ			=
//=======================================
void MI2C_Waitms(UINT16 d)
{
	delay(d);
}

