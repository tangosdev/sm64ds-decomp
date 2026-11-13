//cpp
/* The localized 2D graphics loaders, .text 0x0200f13c..0x0200f2cc.
 * func_0200f220 fills the sub engine's BG2/BG3 characters and screens and
 * func_0200f13c the main engine's BG2 plus both engines' OBJ characters,
 * each picking its per-language files with GetLanguageIndex(). The entry
 * scene (dScEntry_c, ov075) and the download-play scene (dScMB_c) call
 * them after func_0200f2cc has set up the banks and BG controls.
 *
 * The two functions' file-ID tables are interleaved in .data,
 * 0x0208a10c (f220), 0x0208a120 (f13c), 0x0208a134 (f220), 0x0208a148
 * (f13c), 0x0208a15c (f13c), five words each, one per language. Those
 * tables, the player count and the two pointer tables in front of them
 * live in src/data/arm9, one object per file: this translation unit is
 * text-only, so it cannot carry them. The eight bytes after the last
 * file-ID word are not part of them.
 *
 * mwccarm emits .text in reverse source order, so the file reads from the
 * top of the run down.
 */
#include "types.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx_load.h"

extern "C" {


s32 GetLanguageIndex(void);
u32 LoadCompressedFileAt(int fileID, void *target);
void *LoadFile(int fileID);
}

void Deallocate(void *ptr);

/* Player count. Callers store one byte; the next object is word-aligned,
   so the three bytes after it stay 0. */
extern unsigned char data_0208a0e0;

/* Per-language file pointers, then OAM templates, indexed by GetLanguageIndex(). */
extern void *data_0208a0e4[5];

struct OamAttr;
extern struct OamAttr *data_0208a0f8[5];

/* Per-language file IDs, indexed by GetLanguageIndex(). */
extern u32 data_0208a10c[5];
 /* sub BG2 characters */
extern u32 data_0208a120[5];
 /* sub OBJ characters */
extern u32 data_0208a134[5];
 /* sub BG3 characters at +0x6000 */
extern u32 data_0208a148[5];
 /* main BG2 characters */
extern u32 data_0208a15c[5];
 /* main OBJ characters */

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
