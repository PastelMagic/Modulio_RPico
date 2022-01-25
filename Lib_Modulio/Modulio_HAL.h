//=======================================
//= Hardware depended Library API	=
//= ハードウェア依存ライブラリAPI	=
//=======================================
//

#ifndef EXTERN
#define extern
#endif
#ifdef __cplusplus
extern "C" {
#endif
extern UINT8 MI2C_fSendStart(UINT8 bSlaveAddr, UINT8 fRW);
extern UINT8 MI2C_fSendRepStart(UINT8 bSlaveAddr, UINT8 fRW);
extern void MI2C_SendStop(void);
extern UINT8 MI2C_fWrite(UINT8 bData);
extern UINT8 MI2C_bRead(UINT8 fACK);
extern void MI2C_WaitMills(UINT16 delay);
extern void MI2C_Startup(void);
#ifdef __cplusplus
}
#endif
