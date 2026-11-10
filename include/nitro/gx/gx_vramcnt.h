#ifndef NITRO_GX_GX_VRAMCNT_H
#define NITRO_GX_GX_VRAMCNT_H

/* NitroSDK's VRAM bank control (gx_vramcnt.h), defined in
 * src/engine/gx/GX_VramCnt.cpp (pret pokediamond
 * arm9/lib/NitroSDK/src/GX_vramcnt.c). Only the functions this game links
 * are declared.
 *
 * The SetBankFor* banks are int, the SDK's GXVRam* enum parameters. Every
 * definition compiles to the same bytes with int as with u16, and the callers
 * that pass a saved bank word (the EndLoad* functions, the d3d minigame
 * base's restores) do so with no narrowing in the ROM; a u16 parameter
 * inserted an lsl/lsr pair at each of them. Reset gives a user's banks back
 * to the LCDC, Disable switches them off altogether; both return the banks
 * the user held. Parameters stay unnamed: a name in a shared header takes an
 * @NNN in every includer. */

#ifdef __cplusplus
extern "C" {
#endif

void GX_SetBankForBG(int);
void GX_SetBankForOBJ(int);
void GX_SetBankForBGExtPltt(int);
void GX_SetBankForTex(int);
void GX_SetBankForTexPltt(int);
void GX_SetBankForLCDC(int);
void GX_SetBankForSubBG(int);
void GX_SetBankForSubOBJ(int);
void GX_SetBankForSubBGExtPltt(int);
void GX_SetBankForSubOBJExtPltt(int);

unsigned short GX_ResetBankForBG(void);
unsigned short GX_ResetBankForOBJ(void);
unsigned short GX_ResetBankForTex(void);
unsigned short GX_ResetBankForTexPltt(void);
unsigned short GX_ResetBankForSubBG(void);
unsigned short GX_ResetBankForSubOBJ(void);
unsigned short GX_ResetBankForSubBGExtPltt(void);
unsigned short GX_ResetBankForSubOBJExtPltt(void);

unsigned short GX_DisableBankForBG(void);
unsigned short GX_DisableBankForOBJ(void);
unsigned short GX_DisableBankForBGExtPltt(void);
unsigned short GX_DisableBankForOBJExtPltt(void);
unsigned short GX_DisableBankForTex(void);
unsigned short GX_DisableBankForTexPltt(void);
unsigned short GX_DisableBankForClearImage(void);
unsigned short GX_DisableBankForARM7(void);
unsigned short GX_DisableBankForLCDC(void);
unsigned short GX_DisableBankForSubBG(void);
unsigned short GX_DisableBankForSubOBJ(void);
unsigned short GX_DisableBankForSubBGExtPltt(void);
unsigned short GX_DisableBankForSubOBJExtPltt(void);

/* The banks a user currently holds. */
int GX_GetBankForTex(void);
int GX_GetBankForTexPltt(void);
int GX_GetBankForSubBG(void);
int GX_GetBankForSubOBJ(void);

/* The bytes of VRAM a user currently holds (the SDK returns u32). */
int GX_GetSizeOfTex(void);
int GX_GetSizeOfTexPltt(void);

#ifdef __cplusplus
}
#endif

#endif
