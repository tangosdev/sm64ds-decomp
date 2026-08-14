// THE STAGE CLASS'S OWN BSS, hosted. (run linkw wave 8, lane w8-a.)
//
// WHERE THIS LIST COMES FROM, and why it is not a guess. The lane probe-wired
// every src/_ZN5Stage* translation unit no slice already names -- 29 files --
// into walk_window in a throwaway build and read the linker's answer:
//
//     walk_window.exe : fatal error LNK1120: 171 unresolved externals
//
// 34 of those 171 are arm9 BSS symbols the port hosts nowhere. 33 of them are
// below; the 34th is data_020a0dea and it is deliberately NOT here (see the
// blocked note at the end of this header). Nothing in this file is behaviour:
// every symbol is zero storage that no code in today's link writes except the
// Stage TUs that land alongside it, and those are not dispatched yet.
//
// ---- SIZES ARE MEASURED, NOT CHOSEN ---------------------------------------
//
// Every extent below is the NEXT-SYMBOL DELTA in config/arm9/symbols.txt, the
// same rule the w6-c retirement used for data_0209f250/data_0209f2f8 after a
// one-byte declaration nearly shipped. Read out of the config, sorted by
// address:
//
//     data_0209d4a8  -> data_0209d4ac      4
//     data_0209f1ec  -> data_0209f1f0      4      (and so on: every symbol in
//     ...                                          this file is a 4-byte cell)
//     data_0209f360  -> data_0209f368      8      THE ONE EXCEPTION
//     data_0209fcc8  -> data_0209fccc      4
//
// data_0209f360 is the only one that is not four bytes and the only one any
// reader INDEXES: Stage::VE_Init writes [0..1], Stage::LC_Update [0..2] and
// Stage::PS_Update [0..3] -- four u16 slots, which is exactly the eight bytes
// the delta pins. Hosting it as a bare scalar (or as one of the generous
// `int x[8]` cells hal/auto_bss.cpp uses) would have been wrong in opposite
// directions: too small silently strays into the next host object, too large
// is dead .dsstate. `unsigned short data_0209f360[4]` is the measurement.
//
// The remaining 32 are read as SCALARS by every declaration in src (u8, u16,
// s16, int, void *) -- checked one by one against the declarations in the 29
// probed TUs, none of them subscripts anything but [0]. They are hosted four
// bytes wide regardless of the declared width, because four bytes is what the
// DS layout gives them and a byte-wide host object would let a `u16` or `int`
// writer (data_0209f300 and data_0209f304 are both spelled `u16` in one TU and
// `s16` in another; data_0209d4a8 is `int` in one and `void *` in another)
// walk into the next symbol.
//
// ---- WHY .dsstate -------------------------------------------------------
//
// These are hosted DS globals: mutable game state the in-memory save state has
// to roll back. DSSTATE_BEGIN/END route every plain global between them into
// the captured section (hal/dsstate_seg.h); dsstate_guard.py fails the build if
// one lands outside. There is no later `extern` re-declaration of any symbol
// below anywhere in this file -- that is the silent-defeat shape the header of
// dsstate_seg.h documents and it is why this file declares nothing twice.
//
// ---- NOT HERE, AND WHY ----------------------------------------------------
//
// data_020a0dea. It is byte +2 of the sixteen-byte TouchInfo block the port
// already hosts as `int data_020a0de8[8]` in hal/auto_bss.cpp, and the host
// stylus writer (hal/sub_screen.cpp) publishes the touch X through
// data_020a0de8[2] -- the same byte. Its readers walk it as
// data_020a0dea[slot * 4] for slot 0..3, i.e. straight across that block. A
// separate object here would read zeros forever while the writer filled the
// other one, which is the adjacent-symbol failure the campaign has already
// paid for once. MSVC's /alternatename can only alias symbol to symbol, never
// symbol to symbol+2, so hosting it correctly means re-shaping the existing
// auto_bss definition -- a modification of a wave-1 symbol, not an append.
// Recorded as a blocked item; the three Stage TUs that need it
// (Stage::LC_Update, Stage::PS_Update, Stage::VE_Update) stay out of this
// lane's slice because of it.
#include "dsstate_seg.h"

DSSTATE_BEGIN
extern "C" {

/* ---- the pause / VS / level-clear state block, 0x0209f1ec..0x0209f304 -----
   Stage::PS_Init, Stage::PS_Update, Stage::PS_Render, Stage::VE_Init,
   Stage::VE_Update, Stage::LC_Update, Stage::LC_Render, Stage::Behavior,
   Stage::Render, Stage::UpdateMenuButtons and Stage::PS_UpdateSaveMenu read
   and write these. Only Stage::UpdateMenuButtons is in this lane's slice; the
   rest are landed here so the next piece does not re-litigate the storage. */
unsigned char data_0209f1ec[4];   /* pause-screen mode */
unsigned char data_0209f1fc[4];
unsigned char data_0209f210[4];
unsigned char data_0209f218[4];
unsigned char data_0209f22c[4];
unsigned char data_0209f230[4];
unsigned char data_0209f234[4];
unsigned char data_0209f238[4];
unsigned char data_0209f23c[4];
unsigned char data_0209f240[4];
unsigned char data_0209f244[4];   /* menu-button state (UpdateMenuButtons) */
unsigned char data_0209f260[4];
unsigned char data_0209f280[4];
unsigned char data_0209f290[4];
unsigned char data_0209f29c[4];
unsigned char data_0209f2a0[4];
unsigned char data_0209f2a4[4];
unsigned char data_0209f2a8[4];
unsigned char data_0209f2b0[4];
unsigned char data_0209f2b4[4];   /* menu-button state (UpdateMenuButtons) */
unsigned char data_0209f2b8[4];
unsigned char data_0209f2c8[4];
unsigned char data_0209f2cc[4];
unsigned char data_0209f2dc[4];
unsigned char data_0209f2e0[4];   /* menu-button state (UpdateMenuButtons) */
unsigned char data_0209f2e4[4];
unsigned char data_0209f2ec[4];
unsigned char data_0209f2f0[4];
unsigned char data_0209f300[4];   /* u16 in one TU, s16 in another */
unsigned char data_0209f304[4];

/* The four on-screen menu-button X positions, u16 each. The ONLY indexed
   symbol in this file: [0..3], which is the eight bytes 0x0209f360..0x0209f368
   the next-symbol delta pins. Stage::UpdateMenuButtons -- in this lane's slice
   -- reads them. */
unsigned short data_0209f360[4];

/* Stage::Behavior's frame counter. */
unsigned char data_0209fcc8[4];

/* The CURRENT SCENE'S GRAPHICS BLOCK pointer. Stage::InitResources stores
   &data_0209f3c4 here and func_02019144 dispatches its vtable slot 2 (the beat
   hal/sub_actors.cpp stands in for with port_minimap_affine_update). Declared
   `int` in one TU and `void *` in another; four bytes either way. Nothing in
   today's link seats it, so it stays the null the ROM boots with until the
   Stage's InitResources runs for real. */
unsigned char data_0209d4a8[4];

}  /* extern "C" */
DSSTATE_END

/* ---- THE C-NAME FACES FOR THE STORAGE ABOVE -------------------------------
 *
 * Same flip hal/cxx_aliases.cpp exists for, one class further on. A Stage TU
 * compiled as C++ that declares `extern unsigned char data_0209f2e0;` without
 * extern "C" emits the MSVC mangling `?data_0209f2e0@@3EA`, while the storage
 * above (and every .c reader) is the plain C name `_data_0209f2e0`. The alias
 * closes the gap without touching src/.
 *
 * EVERY LINE BELOW IS TRANSCRIBED FROM A LINKER ERROR, not constructed from
 * the type by hand: each decorated LHS is the exact string the probe link
 * printed, and the comment names the TU whose reference produced it. A hand-
 * built mangling that is one letter off is a directive that never fires and
 * never says so.
 *
 * They are INERT in this piece: the referencing TUs (Stage::PS_Init.cpp,
 * Stage::PS_UpdateOptionsMenu, Stage::PS_UpdateOkAndBackButtons,
 * Stage::Behavior) are not in this lane's slice, so each LHS is absent from
 * the map -- which alternatename_guard.py reads as "OK (unused)". They land
 * now so the next piece adds translation units, not plumbing.
 */
/* referenced by src/_ZN5Stage7PS_InitEv.cpp */
#pragma comment(linker, "/alternatename:?data_0209f1ec@@3EA=_data_0209f1ec")
#pragma comment(linker, "/alternatename:?data_0209f210@@3EA=_data_0209f210")
#pragma comment(linker, "/alternatename:?data_0209f218@@3EA=_data_0209f218")
#pragma comment(linker, "/alternatename:?data_0209f22c@@3EA=_data_0209f22c")
#pragma comment(linker, "/alternatename:?data_0209f230@@3EA=_data_0209f230")
#pragma comment(linker, "/alternatename:?data_0209f238@@3EA=_data_0209f238")
#pragma comment(linker, "/alternatename:?data_0209f23c@@3EA=_data_0209f23c")
#pragma comment(linker, "/alternatename:?data_0209f240@@3EA=_data_0209f240")
#pragma comment(linker, "/alternatename:?data_0209f280@@3EA=_data_0209f280")
#pragma comment(linker, "/alternatename:?data_0209f2c8@@3EA=_data_0209f2c8")
#pragma comment(linker, "/alternatename:?data_0209f2f0@@3EA=_data_0209f2f0")
#pragma comment(linker, "/alternatename:?data_0209f300@@3GA=_data_0209f300")
/* referenced by src/_ZN5Stage7PS_InitEv.cpp and
   src/_ZN5Stage25PS_UpdateOkAndBackButtonsEb.cpp */
#pragma comment(linker, "/alternatename:?data_0209f244@@3EA=_data_0209f244")
#pragma comment(linker, "/alternatename:?data_0209f29c@@3EA=_data_0209f29c")
/* referenced by src/_ZN5Stage7PS_InitEv.cpp,
   src/_ZN5Stage20PS_UpdateOptionsMenuEv.cpp and
   src/_ZN5Stage25PS_UpdateOkAndBackButtonsEb.cpp */
#pragma comment(linker, "/alternatename:?data_0209f2e0@@3EA=_data_0209f2e0")
#pragma comment(linker, "/alternatename:?data_0209f2cc@@3EA=_data_0209f2cc")
/* referenced by src/_ZN5Stage20PS_UpdateOptionsMenuEv.cpp */
#pragma comment(linker, "/alternatename:?data_0209f2e4@@3EA=_data_0209f2e4")
#pragma comment(linker, "/alternatename:?data_0209f2ec@@3EA=_data_0209f2ec")
/* referenced by src/_ZN5Stage25PS_UpdateOkAndBackButtonsEb.cpp */
#pragma comment(linker, "/alternatename:?data_0209f2b4@@3EA=_data_0209f2b4")
