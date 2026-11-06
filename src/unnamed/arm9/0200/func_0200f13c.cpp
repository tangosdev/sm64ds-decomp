//cpp
/* The localized 2D graphics loaders, .text 0x0200f13c..0x0200f2cc.
 * func_0200f220 fills the sub engine's BG2/BG3 characters and screens and
 * func_0200f13c the main engine's BG2 plus both engines' OBJ characters,
 * each picking its per-language files with GetLanguageIndex(). The entry
 * scene (dScEntry_c, ov075) and the download-play scene (dScMB_c) call
 * them after func_0200f2cc has set up the banks and BG controls.
 *
 * One file: the two functions' file-ID tables are interleaved in .data,
 * 0x0208a10c (f220), 0x0208a120 (f13c), 0x0208a134 (f220), 0x0208a148
 * (f13c), 0x0208a15c (f13c), five words each, one per language.
 *
 * mwccarm emits .text in reverse source order, so the file reads from the
 * top of the run down.
 *
 * leftovers:
 * - The five tables are this file's data, but the TU is text-only and
 *   they stay extern under their linker names.
 * - Neither neighbour is claimed. GetLanguageIndex (0x0200f0bc, just
 *   below) and func_0200f2cc (just above) touch none of the tables. The
 *   bytes do not say whether either one belongs to this file.
 */
#include "types.h"
#include "G2.h"
#include "GX.h"

extern "C" {

s32 GetLanguageIndex(void);
u32 LoadCompressedFileAt(int fileID, void *target);
void *LoadFile(int fileID);
void Deallocate(void *ptr);

/* Per-language file IDs, indexed by GetLanguageIndex(). */
extern u32 data_0208a10c[]; /* sub BG2 characters */
extern u32 data_0208a120[]; /* sub OBJ characters */
extern u32 data_0208a134[]; /* sub BG3 characters at +0x6000 */
extern u32 data_0208a148[]; /* main BG2 characters */
extern u32 data_0208a15c[]; /* main OBJ characters */

// @symbol func_0200f220
/* Loads the sub-engine BG graphics: per-language BG2 characters (and BG3
 * characters at +0x6000), file 0x8405 at BG3 characters +0x3000, files
 * 0x9802, 0x9800 and 0x8408 into BG2 screen memory at +0x800, +0x2000 and
 * +0x2800, and file 0x8407 as BG3's screen. */
void func_0200f220(void) {
    s32 idx;
    idx = GetLanguageIndex();
    LoadCompressedFileAt(data_0208a10c[idx], G2S::GetBG2CharPtr());
    idx = GetLanguageIndex();
    LoadCompressedFileAt(data_0208a134[idx], (char *)G2S::GetBG3CharPtr() + 0x6000);
    LoadCompressedFileAt(0x8405, (char *)G2S::GetBG3CharPtr() + 0x3000);
    LoadCompressedFileAt(0x9802, (char *)G2S::GetBG2ScrPtr() + 0x800);
    LoadCompressedFileAt(0x9800, (char *)G2S::GetBG2ScrPtr() + 0x2000);
    LoadCompressedFileAt(0x8408, (char *)G2S::GetBG2ScrPtr() + 0x2800);
    LoadCompressedFileAt(0x8407, G2S::GetBG3ScrPtr());
}

// @symbol func_0200f13c
/* Loads the main-engine BG2 graphics (language-specific characters and the
 * three screens 0x9808..0x980a), main OBJ characters at 0x2000 into OBJ VRAM
 * (0x06400000) with palette file 0x980f, and sub OBJ characters at 0x800
 * into sub OBJ VRAM (0x06600000) with palette file 0x8400. dScMB_c runs
 * func_0200f220 just before it. */
void func_0200f13c(void) {
    s32 i = GetLanguageIndex();
    LoadCompressedFileAt(data_0208a148[i], G2::GetBG2CharPtr());
    LoadCompressedFileAt(0x9808, G2::GetBG2ScrPtr());
    LoadCompressedFileAt(0x9809, (char *)G2::GetBG2ScrPtr() + 0x800);
    LoadCompressedFileAt(0x980a, (char *)G2::GetBG2ScrPtr() + 0x1000);
    char *a5 = (char *)0x6400000;
    a5 += 0x2000;
    LoadCompressedFileAt(data_0208a15c[GetLanguageIndex()], a5);
    void *p = LoadFile(0x980f);
    GX::LoadOBJPltt(p, 0x120, 0xe0);
    Deallocate(p);
    char *a6 = (char *)0x6600000;
    a6 += 0x800;
    LoadCompressedFileAt(data_0208a120[GetLanguageIndex()], a6);
    void *q = LoadFile(0x8400);
    GXS::LoadOBJPltt(q, 0, 0x200);
    Deallocate(q);
}
}
