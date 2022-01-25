//=======================================
//= Modulio Library on ATmega328	=
//=					=
//=======================================
//

#include <avr/io.h>
#include <util/delay.h>

#include "Modulio.h"

//#define F_CPU 8000000UL

//=======================================
//= Low Level I2C API			=
//= 低レベルAPI				=
//=					=
//=======================================

//---------------------------------------
//- Generate START condition on I2C	-
//- スタートコンディション生成		-
//-					-
//- Return Value（戻り値)		-
//- !=0: Slave acknowleged		-
//-	スレーブが応答した		-
//- ==0: Slave didn't acknowlege	-
//-	スレーブが応答しなかった	-
//---------------------------------------
//
#define	TWSR_START		0x08	// A START condition has been transmitted
#define	TWSR_SLAW_ACK		0x18	// SLA+W byte transmitted & ACK has been received.
#define	TWSR_SLAR_ACK		0x40	// SLA+R byte transmitted & ACK has been received.
UINT8 MI2C_fSendStart(UINT8 bSlaveAddr, UINT8 fRW)
{
	UINT8 stat;
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);	// Send Start Condition Clear TWINT
	while (!(TWCR & (1<<TWINT) ))	// Wait until TWINT == 1
		;
	stat = TWSR & 0xf8;
	if (stat != TWSR_START)
		return(MI2C_NAKslave);	// Error
	TWDR = ((bSlaveAddr << 1) | fRW);	// Load Slave Address & R/W flag into TWDR
	TWCR = (1<<TWINT) | (1<<TWEN);	// Start transmission
	while(!(TWCR & (1<<TWINT)))	// Wait until TWINT == 1
		;
	stat = TWSR & 0xf8;
	if (fRW == 0) {			// Write
		if (stat != TWSR_SLAW_ACK)
			return(MI2C_NAKslave);	// NAK has been received
	} else {			// Read
		if (stat != TWSR_SLAR_ACK)
			return(MI2C_NAKslave);	// NAK has been received
	}
	return(MI2C_ACKslave);		// ACK has been received
}

//---------------------------------------
//- Generate Rep.START condition on I2C	-
//- リピーティッドスタート生成		-
//-					-
//- Return Value（戻り値)		-
//- !=0: Slave acknowleged		-
//-	スレーブが応答した		-
//- ==0: Slave didn't acknowlege	-
//-	スレーブが応答しなかった	-
//---------------------------------------
//
#define	TWSR_REPSTART		0x10	// A REP-START condition has been transmitted
UINT8 MI2C_fSendRepStart(UINT8 bSlaveAddr, UINT8 fRW)
{
	UINT8 stat;
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);	// Send Start Condition Clear TWINT
	while(!(TWCR & (1<<TWINT)))	// Wait until TWINT == 1
		;
	stat = TWSR & 0xf8;
	if (stat != TWSR_REPSTART)
		return(MI2C_NAKslave);	// Error
	TWDR = ((bSlaveAddr << 1) | fRW);	// Load Slave Address & R/W flag into TWDR
	TWCR = (1<<TWINT) | (1<<TWEN);	// Start transmission
	while(!(TWCR & (1<<TWINT)))	// Wait until TWINT == 1
		;
	stat = TWSR & 0xf8;
	if (fRW == 0) {			// Write
		if (stat != TWSR_SLAW_ACK)
			return(MI2C_NAKslave);	// NAK has been received
	} else {			// Read
		if (stat != TWSR_SLAR_ACK)
			return(MI2C_NAKslave);	// NAK has been received
	}
	return(MI2C_ACKslave);		// ACK has been received
}

//---------------------------------------
//- Generate STOP condition on I2C	-
//- ストップコンディション生成		-
//-					-
//- Return Value（戻り値)		-
//-	None				-
//-	（なし）			-
//---------------------------------------
//
void MI2C_SendStop(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);	// Send Stop Condition Clear TWINT
	while(TWCR & (1<<TWSTO))	// Wait until generate STOP condition.
		;
}


//---------------------------------------
//- Send a single-byte on I2C bus 	-
//- １バイト送信			-
//-					-
//- Return Value（戻り値)		-
//- ==0xff: Slave faild to acknowlege	-
//-	スレーブにエラー発生		-
//---------------------------------------
//
#define	TWSR_MT_DATA_ACK	0x28	// Data byte transmitted & ACK has been received.
UINT8 MI2C_fWrite(UINT8 bData)
{
	UINT8 stat;
	TWDR = bData;			// Load Transmit data into TWDR
	TWCR = (1<<TWINT) | (1<<TWEN);	// Start transmission
	while(!(TWCR & (1<<TWINT)))	// Wait until TWINT == 1
		;
	stat = TWSR & 0xf8;
	if (stat != TWSR_MT_DATA_ACK)
		return(MI2C_NAKslave);	// NAK has been received
	return(MI2C_ACKslave);		// ACK has been received
}

//---------------------------------------
//- Read a single-byte from I2C bus 	-
//- １バイト受信			-
//-					-
//- fACK				-
//- ==MI2C_fNAK(==0x00)			-
//-   Generate NAK after recieving data	-
//-　 転送終了後にNAK生成（リード終了）	-
//- ==MI2C_fACK(=0x01)			-
//-   Generate ACK after recieving data	-
//-　 転送終了後にACK生成（リード継続）	-
//-					-
//- Return Value（戻り値)		-
//-	Data received from slave	-
//-	読み取ったデータ		-
//---------------------------------------
//

#define	TWSR_MR_DATA_ACK	0x50	// ACK has been transmitted
#define	TWSR_MR_DATA_NAK	0x58	// NAK has been transmitted
UINT8 MI2C_bRead(UINT8 fACK)
{
	UINT8	bData,stat;
	if (fACK == MI2C_fNAK) {
		TWCR = (1<<TWINT) | (1<<TWEN);			// Send NAK after data received
	} else {
		TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);	// Send ACK after datat received
	}
	while(!(TWCR & (1<<TWINT)))	// Wait until TWINT == 1
		;
	bData = TWDR;
	stat = TWSR & 0xf8;
	return(bData);
}


//---------------------------------------
//- MI2C Startup			-
//- MI2Cのスタートアップ		-
//- Return Value（戻り値)		-
//-	None				-
//-	（なし）			-
//---------------------------------------
//
// Setup TWI Prescaler & Baudrate Generator
//
// TWSR[1:0] =  00:/1  01:/4 10:/16 11:/32
// Bitrate = SYCLK/(16+2*TWPS*TW)
// ex.8MHz/(16+2*1*32) = 100kHz
//

static void setup_brg(UINT16 i2cclk)
{
	TWSR = TWSR & 0xfc;
	TWBR = F_CPU/2e3/i2cclk-8;
}

void MI2C_Startup(void)
{
	setup_brg(50);
}

//=======================================
//= Delay N(ms)				=
//= Nミリ秒ディレイ			=
//=======================================
void MI2C_WaitMills(UINT16 delay)
{
	while(delay--)
		_delay_ms(1);
}
