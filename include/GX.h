#ifndef SM64DS_GX_H
#define SM64DS_GX_H

#include "types.h"

/* GX / GXS -- the main and sub engines' display setup, VRAM bank mapping and
 * VRAM upload helpers (Nitro SDK namespaces, not classes). Defined in
 * src/engine/gx/GX_Init.cpp (DispOn, SetGraphicsMode),
 * src/engine/gx/GX_VramCnt.cpp (SetBankFor*), src/engine/gx/GX.cpp (texture
 * and BG palette loads), src/engine/gx/GXS.cpp (the OBJ, BG-character,
 * BG-screen and extended-palette loads of both engines) and
 * src/game/stages/d_scene.cpp (DisableAllBanks).
 *
 * The SetBankFor* banks are int, the Nitro SDK's GXVRamBG/GXVRamOBJ/... enum
 * parameters. Every definition compiles to the same bytes with int as with
 * u16, and the callers that pass a saved bank word (GX/GXS EndLoad*, the
 * d3d minigame base's restores) do so with no narrowing in the ROM; a u16
 * parameter inserted an lsl/lsr pair at each of them. Parameters stay
 * unnamed: a name here takes an @NNN in every includer. */
namespace GX {
void DispOn(void);
void SetGraphicsMode(int, int, int);
void DisableAllBanks(void);
void SetBankForBG(int);
void SetBankForOBJ(int);
void SetBankForTex(int);
void SetBankForTexPltt(int);
void SetBankForSubBG(int);
void SetBankForSubOBJ(int);
void SetBankForSubBGExtPltt(int);
void SetBankForSubOBJExtPltt(int);
/* Reset gives a user's banks back to the LCDC; Disable switches them off
   altogether. Both return the banks the user held, and both are one call on
   ResetVramBanks / DisableVramBanks against a single gGXState field, which is
   what names them -- see src/engine/gx/GX_VramCnt.cpp. LCDC, ARM7 and
   ClearImage have no SetBankFor* counterpart above, so those three names are
   coined from the field each one releases. */
u16 ResetBankForBG(void);
u16 ResetBankForOBJ(void);
u16 ResetBankForTex(void);
u16 ResetBankForTexPltt(void);
u16 ResetBankForSubBG(void);
u16 ResetBankForSubOBJ(void);
u16 ResetBankForSubBGExtPltt(void);
u16 ResetBankForSubOBJExtPltt(void);
u16 DisableBankForBG(void);
u16 DisableBankForOBJ(void);
u16 DisableBankForTex(void);
u16 DisableBankForTexPltt(void);
u16 DisableBankForBGExtPltt(void);
u16 DisableBankForOBJExtPltt(void);
u16 DisableBankForClearImage(void);
u16 DisableBankForARM7(void);
u16 DisableBankForLCDC(void);
u16 DisableBankForSubBG(void);
u16 DisableBankForSubOBJ(void);
u16 DisableBankForSubBGExtPltt(void);
u16 DisableBankForSubOBJExtPltt(void);
/* The banks a user currently holds. */
int GetBankForTex(void);
int GetBankForTexPltt(void);
int GetBankForSubBG(void);
int GetBankForSubOBJ(void);
void BeginLoadTex(void);
void LoadTex(const void *, unsigned int, unsigned int);
void EndLoadTex(void);
void BeginLoadTexPltt(void);
void LoadTexPltt(const void *, unsigned int, unsigned int);
void EndLoadTexPltt(void);
void LoadBGPltt(const void *, unsigned int, unsigned int);
void LoadOBJPltt(const void *, unsigned int, unsigned int);
void LoadOBJ(const void *, unsigned int, unsigned int);
void LoadBG0Char(const void *, unsigned int, unsigned int);
void LoadBG1Char(const void *, unsigned int, unsigned int);
void LoadBG2Char(const void *, u32, u32);
void LoadBG1Scr(const void *, int, int);
}

namespace GXS {
void SetGraphicsMode(int);
void LoadBGPltt(const void *, unsigned int, unsigned int);
void LoadOBJPltt(const void *, unsigned int, unsigned int);
void LoadBG0Char(const void *, unsigned int, unsigned int);
void LoadBG1Char(const void *, unsigned int, unsigned int);
void LoadBG2Char(const void *, unsigned int, unsigned int);
void LoadBG3Char(const void *, u32, u32);
void LoadOBJ(const void *, unsigned int, unsigned int);
void BeginLoadBGExtPltt(void);
void LoadBGExtPltt(const void *, unsigned int, unsigned int);
void EndLoadBGExtPltt(void);
void BeginLoadOBJExtPltt(void);
void LoadOBJExtPltt(const void *, unsigned int, unsigned int);
void EndLoadOBJExtPltt(void);
}

#endif
