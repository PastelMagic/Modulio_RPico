//---------------------------------------
//- ACM1602 16x2 Character Display	-
//- キャラクタディスプレイライブラリ	-
//---------------------------------------
//
#define	EXTERN
#include "Modulio.h"
#undef EXTERN
#include "ACM1602.h"

//----------------------------------------
//- Initialize ACM1602
//- I2C Clock should be set to 50KHz
//- ACM1602の初期化
//- I2Cバスクロックは50kHにすること
//----------------------------------------
//
static UINT8 ACM1602_Start(ACM1602 *p)
{
	UINT8 stat;
	MI2C_Waitms(15);
	do {
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x38);	// Data 8bit 2Lines 5x10dot font
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x0c);	// Display, No cursor
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x01);	// Clear Display
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x02);	// Set DDRAM Address to 00H
		if (stat == MI2C_WRITE_ERR)
			break;
		MI2C_Waitms(5);
		stat = p->Clear(p);
		break;
	} while(0);
	return(stat);
}


//----------------------------------------
// Set Display Position
// 文字表示位置設定
//----------------------------------------
//
static void ACM1602_Position(ACM1602 *p, UINT8 y, UINT8 x)
{
	p->Xpos = x & 0xf;		// X-Position is 0-15
	p->Ypos = y & 1;		// Y-Position is 0 or 1
	if (p->Ypos == 0)		// if Y-Position is 0
		p->DDRAM_Adrs = 0x00;	// Base-address is 0
	else	p->DDRAM_Adrs = 0x40;	// Base-address is 0x40
	p->DDRAM_Adrs |= p->Xpos;	// Setup DDRAM Address
}

//----------------------------------------
// Clear Display
// 表示クリア
//----------------------------------------
//
static UINT8 ACM1602_Clear(ACM1602 *p)
{
	UINT8 stat;
	stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x01);	// Clear Command
	if (stat == MI2C_WRITE_ERR)				// 表示クリアコマンド発行
		return(stat);
	MI2C_Waitms(5);
	(p->Position)(p, 0,0);
	return(stat);

}

//----------------------------------------
// Write Character
// 文字書き込み
//----------------------------------------
//
static UINT8 ACM1602_WriteData(ACM1602 *p, UINT8 dat)
{
	UINT8 stat;
	stat = MI2C_ACKslave;
	switch(dat) {
		case	'\r':	// CR 
			ACM1602_Position(p, p->Ypos, 0);	// Go to Lef-end Position
			break;					// 左端に移動
		case	'\n':	// CR+LF
			ACM1602_Position(p, (p->Ypos)+1, 0);	// Go to Left-end of next-line
			break;					// 次の行の左端に移動
		default:
			// Set RAM Address
			// RAMアドレスを指定
			stat = (p->madrs).WriteByte(&(p->madrs), 0, 0x80 | p->DDRAM_Adrs);
			if (stat == MI2C_WRITE_ERR)
				break;
			// Write Data
			// データを書き込み
			stat = (p->madrs).WriteByte(&(p->madrs), 0x80, dat);
			(p->Position)(p, p->Ypos, p->Xpos+1);
			break;
	}
	return(stat);
}


//----------------------------------------
// Print a byte in Hex String
// 1byte(8bit)を16進数表示
//----------------------------------------
//
static const UINT8 HexDat[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static UINT8 ACM1602_PrHexByte(ACM1602 *p, UINT8 dat)
{
	UINT8 stat, c;
	c = (dat >> 4) & 0xf;				// diaplay upper 4 bit in Hex
	stat = (p->WriteData)(p, HexDat[c]);		// 上位4bit を16進数表示
	if (stat != MI2C_WRITE_ERR) {
		c = dat & 0xf;				// display lower 4 bit in HEX
		stat = (p->WriteData)(p, HexDat[c]);	// 下位4bit を16進数表示
	}
	return(stat);
}

//----------------------------------------
// Print a Word(16bit) in Hex String
// 1 word(16bit)を16進数表示
//----------------------------------------
//
static UINT8 ACM1602_PrHexInt(ACM1602 *p, UINT16 dat)
{
	UINT8 stat;
	stat = (p->PrHexByte)(p, (dat >> 8) & 0xff);	// Display Upper 8bit in HEX
	if (stat != MI2C_WRITE_ERR) {			// 上位8ビットを16進数で表示
		stat = (p->PrHexByte)(p, dat& 0xff);	// Display lower 8bit in HEX 
	}						// 下位8ビットを16進数で表示
	return(stat);
}

//----------------------------------------
// Print String in RAM
// RAM上の文字列表示
//----------------------------------------
//
static UINT8 ACM1602_PrString(ACM1602 *p, char *sRamString)
{
	UINT8 stat,c;
	stat = !MI2C_WRITE_ERR;
	while ((c = *sRamString++) != '\0') {		// Repeat until EOS
		stat = (p->WriteData)(p,c);		// EOSがくるまで繰り返す
		if (stat == MI2C_WRITE_ERR)
			break;
	}
	return(stat);
}

//----------------------------------------
// Print Const String
// ROM上の文字列表示
//----------------------------------------
//
static UINT8 ACM1602_PrCString(ACM1602 *p, const char *sRomString)
{
	UINT8 stat,c;
	stat = !MI2C_WRITE_ERR;
	while ((c = *sRomString++) != '\0') {		// Repeat unitl EOS
		stat = (p->WriteData)(p,c);		// EOSがくるまで繰り返す
		if (stat == MI2C_WRITE_ERR)
			break;
	}
	return(stat);
}

//----------------------------------------
// Initialize ACM1602 Obuject
// ACM1602オブジェクトの初期化
//----------------------------------------
//
void ACM1602_Setup(
		ACM1602 *p,
		UINT8 bswadrs,
		UINT8 bswch,
		UINT8 devadrs,
		UINT8 option)
{
	p->Xpos = 0;				// X-Position
	p->Ypos = 0;				// Y-Position
	// Setup Library funtion Pointers
	// ライブラリ関数ポインタセットアップ
	p->Start = &ACM1602_Start;
	p->Position = &ACM1602_Position;
	p->Clear = &ACM1602_Clear;
	p->WriteData = &ACM1602_WriteData;
	p->PrHexByte = &ACM1602_PrHexByte;
	p->PrHexInt = &ACM1602_PrHexInt;
	p->PrString = &ACM1602_PrString;
	p->PrCString = &ACM1602_PrCString;

	// Initialize Common Library
	// 共通ライブラリの初期化
	MI2C_Setup(&(p->madrs), bswadrs, bswch, devadrs, option);
}
