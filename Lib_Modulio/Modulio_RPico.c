//=======================================
//= Modulio Common Library		=
//=					=
//=======================================
//
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"


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
	if (adrs->BSW_ADRS != 0) {	// Setup channel number of bus switch
					// チャンネル番号設定
		tmp = 1 << ((adrs->BSW_CH) & 0x3);
		if (i2c_write_blocking(i2c_default, adrs->BSW_ADRS, &tmp, 1, false) != 1)
			return(MI2C_NAKslave);
	}
	if (Reg != REG_ADRS_NONE) {	// Register number is valid
					// レジスタ番号が指定されているとき
		tmpbuf[0] = reg;
		for (tmp=0; tmp<Length; tmp++)
			tmpbuf[tmp+1] = *Data++;
		
		if (i2c_write_blocking(i2c_default, adrs->DEV_ADRS, tmpbuf, Length+1, false) != Length+1)
			return(MI2C_NAKslave);
	} else {
		if (Length > 0) {
			if (i2c_write_blocking(i2c_default, adrs->DEV_ADRS, tmpbuf, Length, false) != Length)
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
		if (i2c_write_blocking(i2c_default, adrs->BSW_ADRS, &tmp, 1, false) != 1)
			return(MI2C_NAKslave);
	}
	if (Reg != REG_ADRS_NONE) {	// Write register address if it is valid
					// レジスタアドレス指定があるなら書込む
					// Attempt to execute REPEATED START
					// 続くリード動作はREPEATED STARTコンディション
		if (Length == 0) {
			if (i2c_write_blocking(i2c_default, adrs->DEV_ADRS, &reg, 1, false) != 1)
				return(MI2C_NAKslave);
		} else {
			if (i2c_write_blocking(i2c_default, adrs->DEV_ADRS, &reg, 1, false) != 1)
				return(MI2C_NAKslave);
			if (i2c_read_blocking(i2c_default, adrs->DEV_ADRS, Data, Length, false) != Length)
				return(MI2C_NAKslave);
		}
	} else {
		if (Length > 0) {
			if (i2c_read_blocking(i2c_default, adrs->DEV_ADRS, Data, Length, false) != Length)
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
///- ==MI2C_NAKslave(0x00)		-
//-	Error occured on I2C bus	-
//-	書き込み中にエラー発生		-
//- == MI2C_ACKslave			-
//-	Xfr complete with no error	-
//-	正常終了			-
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
    // Enable UART so we can print status output
    stdio_init_all();
//#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
//#warning Modulio requires a board with I2C pins
//    puts("Default I2C pins were not defined");
//#else

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    //i2c_init(i2c_default, 100 * 1000);		// 100kHz動作にする
    i2c_init(i2c_default, 50 * 1000);		// 50kHz動作にする
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}


//=======================================
//= Delay N(ms)				=
//= Nミリ秒ディレイ			=
//=======================================
void MI2C_Waitms(UINT16 d)
{
	sleep_ms(d);
}

