//cpp
/* The localized 2D graphics loaders and their shared language/player-count
 * state, .text 0x0200f0bc..0x0200f470. GetLanguageIndex indexes every
 * per-language table here. func_0200f220 fills the sub engine's BG2/BG3
 * characters and screens and func_0200f13c the main engine's BG2 plus both
 * engines' OBJ characters after func_0200f2cc has set up the banks and BG
 * controls. SetNumPlayers keeps the count the tables' first byte holds.
 * The two trailing empty functions are this build's unused frame hooks.
 *
 * The two loader functions' file-ID tables are interleaved in .data,
 * 0x0208a10c (f220), 0x0208a120 (f13c), 0x0208a134 (f220), 0x0208a148
 * (f13c), 0x0208a15c (f13c), five words each, one per language. The
 * player count and the two per-language pointer tables sit in front of
 * them. The eight bytes after the last file-ID word are not part of
 * them.
 *
 * mwccarm emits .text in reverse source order, so the file reads from the
 * top of the run down.
 */
#include "types.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"
#include "nitro/gx/gx.h"
#include "nitro/gx/gx_vramcnt.h"
#include "GX.h"
#include "nitro/hw/registers.h"

extern "C" {

s32 GetOwnerLanguage(void);
s32 GetLanguageIndex(void);
u32 LoadCompressedFileAt(int fileID, void *target);
void *LoadFile(int fileID);
void Deallocate(void *ptr);
void func_020308d0(int n);

/* The per-language compressed OBJ graphics and OAM template tables point
   at file data other translation units own. */
extern u8 data_02096f08[], data_02097230[], data_020975d4[], data_02097950[], data_02097ca4[];
struct OamAttr;
extern struct OamAttr data_02098b58[], data_02098d20[], data_02098e60[], data_02098fa0[],
    data_02099068[];

/* Player count. Callers store one byte; the next object is word-aligned,
   so the three bytes after it stay 0. */
unsigned char data_0208a0e0 = 1;

/* Per-language file pointers, then OAM templates, indexed by GetLanguageIndex(). */
void *data_0208a0e4[5] = {
    data_02096f08, data_02097230, data_020975d4, data_02097950, data_02097ca4,
};
struct OamAttr *data_0208a0f8[5] = {
    data_02098b58, data_02098d20, data_02098e60, data_02098fa0, data_02099068,
};

/* Per-language file IDs, indexed by GetLanguageIndex(). */
u32 data_0208a10c[5] = {0xa00d, 0xa40d, 0xa80d, 0xac0d, 0xb00d}; /* sub BG2 characters */
u32 data_0208a120[5] = {0xa000, 0xa400, 0xa800, 0xac00, 0xb000}; /* sub OBJ characters */
u32 data_0208a134[5] = {0xa004, 0xa404, 0xa804, 0xac04, 0xb004}; /* sub BG3 characters at +0x6000 */
u32 data_0208a148[5] = {0xa00f, 0xa40f, 0xa80f, 0xac0f, 0xb00f}; /* main BG2 characters */
u32 data_0208a15c[5] = {0xa00e, 0xa40e, 0xa80e, 0xac0e, 0xb00e}; /* main OBJ characters */

/* Both empty in this build. func_02019390 calls func_0200f468 every frame,
   after OAM::Reset; func_02019440 calls func_0200f46c last. */
void func_0200f46c(void) {}

void func_0200f468(void) {}

void SetNumPlayers(int n) {
    data_0208a0e0 = n;
    func_020308d0(n);
}

/* Display setup shared by the scenes in src/game/stages/d_s_entry_methods.cpp
 * and src/game/stages/d_s_mb.cpp: VRAM banks, both engines' BG modes with
 * 2D-mapped OBJ characters, and the BG controls the loaders func_0200f220
 * and func_0200f13c fill. Each BG control store keeps the priority and
 * mosaic bits (& 0x43), then clears mosaic. */
void func_0200f2cc(void) {
    GX::DisableAllBanks();
    GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
    GX_SetBankForBG(GX_VRAM_BG_128_C);
    GX_SetBankForOBJ(GX_VRAM_OBJ_32_FG);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_48_HI);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);

    GX_SetBGCharOffset(0);
    GX_SetBGScrOffset(0);
    /* Main engine on the top screen. */
    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);

    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                         GX_BG_CHARBASE_0x10000);
    G2_BG3Mosaic(0);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_512x512, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800,
                         GX_BG_CHARBASE_0x08000);
    G2_BG2Mosaic(0);
    GXS_SetGraphicsMode(GX_BGMODE_0);

    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_2D);
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0000,
                      GX_BG_CHARBASE_0x04000, GX_BG_EXTPLTT_01);
    G2S_BG0Mosaic(0);
    G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x0800,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG2Mosaic(0);
    G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3800,
                          GX_BG_CHARBASE_0x04000);
    G2S_BG3Mosaic(0);
}

/* Loads the sub-engine BG graphics: per-language BG2 characters (and BG3
 * characters at +0x6000), file 0x8405 at BG3 characters +0x3000, files
 * 0x9802, 0x9800 and 0x8408 into BG2 screen memory at +0x800, +0x2000 and
 * +0x2800, and file 0x8407 as BG3's screen. */
void func_0200f220(void) {
    s32 idx;
    idx = GetLanguageIndex();
    LoadCompressedFileAt(data_0208a10c[idx], G2S_GetBG2CharPtr());
    idx = GetLanguageIndex();
    LoadCompressedFileAt(data_0208a134[idx], (char *)G2S_GetBG3CharPtr() + 0x6000);
    LoadCompressedFileAt(0x8405, (char *)G2S_GetBG3CharPtr() + 0x3000);
    LoadCompressedFileAt(0x9802, (char *)G2S_GetBG2ScrPtr() + 0x800);
    LoadCompressedFileAt(0x9800, (char *)G2S_GetBG2ScrPtr() + 0x2000);
    LoadCompressedFileAt(0x8408, (char *)G2S_GetBG2ScrPtr() + 0x2800);
    LoadCompressedFileAt(0x8407, G2S_GetBG3ScrPtr());
}

/* Loads the main-engine BG2 graphics (language-specific characters and the
 * three screens 0x9808..0x980a), main OBJ characters at 0x2000 into OBJ VRAM
 * (0x06400000) with palette file 0x980f, and sub OBJ characters at 0x800
 * into sub OBJ VRAM (0x06600000) with palette file 0x8400. dScMB_c runs
 * func_0200f220 just before it. */
void func_0200f13c(void) {
    s32 i = GetLanguageIndex();
    LoadCompressedFileAt(data_0208a148[i], G2_GetBG2CharPtr());
    LoadCompressedFileAt(0x9808, G2_GetBG2ScrPtr());
    LoadCompressedFileAt(0x9809, (char *)G2_GetBG2ScrPtr() + 0x800);
    LoadCompressedFileAt(0x980a, (char *)G2_GetBG2ScrPtr() + 0x1000);
    LoadCompressedFileAt(data_0208a15c[GetLanguageIndex()], (u8 *)G2_GetOBJCharPtr() + 0x2000);
    void *p = LoadFile(0x980f);
    GX_LoadOBJPltt(p, 0x120, 0xe0);
    Deallocate(p);
    LoadCompressedFileAt(data_0208a120[GetLanguageIndex()], (u8 *)G2S_GetOBJCharPtr() + 0x800);
    void *q = LoadFile(0x8400);
    GXS_LoadOBJPltt(q, 0, 0x200);
    Deallocate(q);
}

/* The console language as an index into the per-language asset tables:
   English 0, French 1, German 2, Italian 3, Spanish 4; anything else 0. */
s32 GetLanguageIndex(void) {
    s32 lang = GetOwnerLanguage();
    switch (lang) {
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 4;
    default:
        return 0;
    }
}
}
