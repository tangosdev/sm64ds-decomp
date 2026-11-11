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

/* The SDK's bank bits (GXVRam) and the bank sets each user can be given
 * (GXVRamBG, GXVRamOBJ, ...), as macros: the setters below take int. A
 * set's name gives its size in KiB, then the slots it fills for the palette
 * users, then its banks. */
#define GX_VRAM_NONE 0x0000
#define GX_VRAM_A    0x0001
#define GX_VRAM_B    0x0002
#define GX_VRAM_C    0x0004
#define GX_VRAM_D    0x0008
#define GX_VRAM_E    0x0010
#define GX_VRAM_F    0x0020
#define GX_VRAM_G    0x0040
#define GX_VRAM_H    0x0080
#define GX_VRAM_I    0x0100

#define GX_VRAM_LCDC_NONE GX_VRAM_NONE
#define GX_VRAM_LCDC_A    GX_VRAM_A
#define GX_VRAM_LCDC_B    GX_VRAM_B
#define GX_VRAM_LCDC_C    GX_VRAM_C
#define GX_VRAM_LCDC_D    GX_VRAM_D
#define GX_VRAM_LCDC_E    GX_VRAM_E
#define GX_VRAM_LCDC_F    GX_VRAM_F
#define GX_VRAM_LCDC_G    GX_VRAM_G
#define GX_VRAM_LCDC_H    GX_VRAM_H
#define GX_VRAM_LCDC_I    GX_VRAM_I
#define GX_VRAM_LCDC_ALL  0x01ff

#define GX_VRAM_BG_NONE     GX_VRAM_NONE
#define GX_VRAM_BG_16_F     GX_VRAM_F
#define GX_VRAM_BG_16_G     GX_VRAM_G
#define GX_VRAM_BG_32_FG    (GX_VRAM_F | GX_VRAM_G)
#define GX_VRAM_BG_64_E     GX_VRAM_E
#define GX_VRAM_BG_80_EF    (GX_VRAM_E | GX_VRAM_F)
#define GX_VRAM_BG_80_EG    (GX_VRAM_E | GX_VRAM_G)
#define GX_VRAM_BG_96_EFG   (GX_VRAM_E | GX_VRAM_F | GX_VRAM_G)
#define GX_VRAM_BG_128_A    GX_VRAM_A
#define GX_VRAM_BG_128_B    GX_VRAM_B
#define GX_VRAM_BG_128_C    GX_VRAM_C
#define GX_VRAM_BG_128_D    GX_VRAM_D
#define GX_VRAM_BG_256_AB   (GX_VRAM_A | GX_VRAM_B)
#define GX_VRAM_BG_256_BC   (GX_VRAM_B | GX_VRAM_C)
#define GX_VRAM_BG_256_CD   (GX_VRAM_C | GX_VRAM_D)
#define GX_VRAM_BG_256_AC   (GX_VRAM_A | GX_VRAM_C)
#define GX_VRAM_BG_256_AD   (GX_VRAM_A | GX_VRAM_D)
#define GX_VRAM_BG_256_BD   (GX_VRAM_B | GX_VRAM_D)
#define GX_VRAM_BG_384_ABC  (GX_VRAM_A | GX_VRAM_B | GX_VRAM_C)
#define GX_VRAM_BG_384_BCD  (GX_VRAM_B | GX_VRAM_C | GX_VRAM_D)
#define GX_VRAM_BG_384_ABD  (GX_VRAM_A | GX_VRAM_B | GX_VRAM_D)
#define GX_VRAM_BG_384_ACD  (GX_VRAM_A | GX_VRAM_C | GX_VRAM_D)
#define GX_VRAM_BG_512_ABCD (GX_VRAM_A | GX_VRAM_B | GX_VRAM_C | GX_VRAM_D)

#define GX_VRAM_OBJ_NONE   GX_VRAM_NONE
#define GX_VRAM_OBJ_16_F   GX_VRAM_F
#define GX_VRAM_OBJ_16_G   GX_VRAM_G
#define GX_VRAM_OBJ_32_FG  (GX_VRAM_F | GX_VRAM_G)
#define GX_VRAM_OBJ_64_E   GX_VRAM_E
#define GX_VRAM_OBJ_80_EF  (GX_VRAM_E | GX_VRAM_F)
#define GX_VRAM_OBJ_80_EG  (GX_VRAM_E | GX_VRAM_G)
#define GX_VRAM_OBJ_96_EFG (GX_VRAM_E | GX_VRAM_F | GX_VRAM_G)
#define GX_VRAM_OBJ_128_A  GX_VRAM_A
#define GX_VRAM_OBJ_128_B  GX_VRAM_B
#define GX_VRAM_OBJ_256_AB (GX_VRAM_A | GX_VRAM_B)

#define GX_VRAM_BGEXTPLTT_NONE    GX_VRAM_NONE
#define GX_VRAM_BGEXTPLTT_0123_E  GX_VRAM_E
#define GX_VRAM_BGEXTPLTT_01_F    GX_VRAM_F
#define GX_VRAM_BGEXTPLTT_23_G    GX_VRAM_G
#define GX_VRAM_BGEXTPLTT_0123_FG (GX_VRAM_F | GX_VRAM_G)

#define GX_VRAM_TEX_NONE      GX_VRAM_NONE
#define GX_VRAM_TEX_0_A       GX_VRAM_A
#define GX_VRAM_TEX_0_B       GX_VRAM_B
#define GX_VRAM_TEX_0_C       GX_VRAM_C
#define GX_VRAM_TEX_0_D       GX_VRAM_D
#define GX_VRAM_TEX_01_AB     (GX_VRAM_A | GX_VRAM_B)
#define GX_VRAM_TEX_01_BC     (GX_VRAM_B | GX_VRAM_C)
#define GX_VRAM_TEX_01_CD     (GX_VRAM_C | GX_VRAM_D)
#define GX_VRAM_TEX_01_AC     (GX_VRAM_A | GX_VRAM_C)
#define GX_VRAM_TEX_01_AD     (GX_VRAM_A | GX_VRAM_D)
#define GX_VRAM_TEX_01_BD     (GX_VRAM_B | GX_VRAM_D)
#define GX_VRAM_TEX_012_ABC   (GX_VRAM_A | GX_VRAM_B | GX_VRAM_C)
#define GX_VRAM_TEX_012_BCD   (GX_VRAM_B | GX_VRAM_C | GX_VRAM_D)
#define GX_VRAM_TEX_012_ABD   (GX_VRAM_A | GX_VRAM_B | GX_VRAM_D)
#define GX_VRAM_TEX_012_ACD   (GX_VRAM_A | GX_VRAM_C | GX_VRAM_D)
#define GX_VRAM_TEX_0123_ABCD (GX_VRAM_A | GX_VRAM_B | GX_VRAM_C | GX_VRAM_D)

#define GX_VRAM_TEXPLTT_NONE       GX_VRAM_NONE
#define GX_VRAM_TEXPLTT_0_F        GX_VRAM_F
#define GX_VRAM_TEXPLTT_0_G        GX_VRAM_G
#define GX_VRAM_TEXPLTT_01_FG      (GX_VRAM_F | GX_VRAM_G)
#define GX_VRAM_TEXPLTT_0123_E     GX_VRAM_E
#define GX_VRAM_TEXPLTT_01234_EF   (GX_VRAM_E | GX_VRAM_F)
#define GX_VRAM_TEXPLTT_012345_EFG (GX_VRAM_E | GX_VRAM_F | GX_VRAM_G)

#define GX_VRAM_SUB_BG_NONE  GX_VRAM_NONE
#define GX_VRAM_SUB_BG_128_C GX_VRAM_C
#define GX_VRAM_SUB_BG_32_H  GX_VRAM_H
#define GX_VRAM_SUB_BG_48_HI (GX_VRAM_H | GX_VRAM_I)

#define GX_VRAM_SUB_OBJ_NONE  GX_VRAM_NONE
#define GX_VRAM_SUB_OBJ_128_D GX_VRAM_D
#define GX_VRAM_SUB_OBJ_16_I  GX_VRAM_I

#define GX_VRAM_SUB_BGEXTPLTT_NONE   GX_VRAM_NONE
#define GX_VRAM_SUB_BGEXTPLTT_0123_H GX_VRAM_H

#define GX_VRAM_SUB_OBJEXTPLTT_NONE GX_VRAM_NONE
#define GX_VRAM_SUB_OBJEXTPLTT_0_I  GX_VRAM_I

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
