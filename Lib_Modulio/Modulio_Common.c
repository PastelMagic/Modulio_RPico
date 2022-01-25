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
//= 中間レベルAPI			=
//=					=
//=======================================
//

//---------------------------------------
//- Write multiple data on I2C		-
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
static UINT8 MI2C_bWriteBytes(UINT8 bSlaveAddr, UINT8 *pbXferData, UINT8 bCnt, UINT8 bMode)
{
	UINT8 stat;
	UINT8 counts;
	stat = MI2C_ACKslave;
	if ((bSlaveAddr & 0x80)==0x00) {	// Send SlaveAddr if it is valid(0x00-0x7f)
						// スレーブアドレスが0x00～0x7fならアドレス指定
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
//- 複数バイト読み込み			-
//-					-
//- Return Value（戻り値)		-
//- ==MI2C_NAKslave(0x00)		-
//-	Error occured on I2C bus	-
//-	読み込み中にエラー発生		-
//- == MI2C_ACKslave			-
//-	Xfr complete with no error	-
//-	正常終了			-
//---------------------------------------
//
static UINT8 MI2C_fReadBytes(UINT8 bSlaveAddr, UINT8 *pbXferData, UINT8 bCnt, UINT8 bMode)
{
	UINT8 stat;
	UINT8 counts;
	stat = MI2C_ACKslave;
	if ((bSlaveAddr & 0x80)==0x00) {	// Send SlaveAddr if it is valid(0x00-0x7f)
						// スレーブアドレスが0x00～0x7fならアドレス指定
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
UINT8 MI2C_WriteBytes(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length)
{
	UINT8 stat,reg;
	reg = Reg & 0xff;
	if (adrs->BSW_ADRS != 0) {	// Setup channel of bus switch
					// バススイッチのチャンネル設定
		stat = 1 << ((adrs->BSW_CH) & 0x3);
		stat = MI2C_bWriteBytes(adrs->BSW_ADRS, &stat, 1, MI2C_CompleteXfer);
		if (stat != MI2C_ACKslave)
			return(stat);
	}
	if (Reg != REG_ADRS_NONE) {	// Register number is valid
					// レジスタ番号が指定されているとき
		stat = MI2C_bWriteBytes(adrs->DEV_ADRS,	// Send register number
				 &reg,			// レジスタ番号書込み
				 1,
				 MI2C_NoStop);
		if (stat != MI2C_ACKslave)
			return(stat);
		stat = DEV_ADRS_NONE;
	} else {
		stat = adrs->DEV_ADRS;
	}
	stat = MI2C_bWriteBytes(	// Perform write operation
			stat,		// データ書込み実行
			Data,
			Length,
			MI2C_CompleteXfer);
	return(stat);
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
UINT8 MI2C_WriteByte(MI2CADRS *adrs, UINT16 Reg, UINT8 Data)
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
UINT8 MI2C_ReadBytes(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data, UINT8 Length)
{
	UINT8 Mode,reg,stat;
	reg = Reg & 0xff;
	if (adrs->BSW_ADRS != 0) {	// Setup channel number of bus switch
					// チャンネル番号設定
		stat = 1 << ((adrs->BSW_CH) & 0x3);
		stat = MI2C_bWriteBytes(adrs->BSW_ADRS, &stat, 1, MI2C_CompleteXfer);
		if (stat != MI2C_ACKslave)
			return(stat);
	}
	if (Reg != REG_ADRS_NONE) {	// Write register address if it is valid
					// レジスタアドレス指定があるなら書込む
		stat = MI2C_bWriteBytes(adrs->DEV_ADRS,	// Send register number
				 &reg,			// レジスタ番号書込み
				 1,
				 MI2C_NoStop);
		if (stat != MI2C_ACKslave)
			return(stat);
		Mode = MI2C_RepStart;	// REP.START should be used for following read operation
					// 続くリード動作はREPEATED STARTコンディション
	} else {
		Mode = MI2C_CompleteXfer;	// START should be used for following read operation
	}					// 続くリード動作はSTART
	stat = MI2C_fReadBytes(adrs->DEV_ADRS,	// Perform read operation
			Data,			// リード実行
			Length,
			Mode);
	return(stat);
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
UINT8 MI2C_ReadByte(MI2CADRS *adrs, UINT16 Reg, UINT8 *Data)
{
	UINT8 sts;
	sts = MI2C_ReadBytes(adrs, Reg, Data, 1);
	return(sts);
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
	MI2C_Startup();
}


//=======================================
//= Delay N(ms)				=
//= Nミリ秒ディレイ			=
//=======================================
void MI2C_Waitms(UINT16 delay)
{
	MI2C_WaitMills(delay);
}

