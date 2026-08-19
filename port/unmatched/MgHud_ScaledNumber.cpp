// HOST TRANSCRIPTION of func_ov004_020b2220 -- the minigame HUD's SCALED,
// ROTATABLE NUMBER DRAWER. Run mg5, lane WTIMER.
//
// WHAT IT IS, PLAINLY. It draws a number on the top screen as up to four
// separate digit sprites, centred on an (x, y) you give it, at a size and a
// tilt you give it. It is the big-digit drawer: its small sibling
// func_ov004_020b1ea4 draws a six-digit number at fixed size, and this one
// draws a four-digit number through an OAM affine matrix so the digits can be
// magnified and spun.
//
// WHY IT EXISTS. It is the "Wanted!" countdown timer. The owner reported the
// scene on 2026-08-19 as
//     "Also time is not showing in wanted"
// and a 300-frame headless boot of scene 366 on cons 2fbc2c0a1 measured
//     [scene] unmatched ov004/ov006 traps entered: 127
// where every sibling minigame scene (368, 374, 376, 378, 390) reports
// exactly 1. A per-site census of that counter split the 127 as
//     func_0202e78c              1     <- the long-standing pre-existing one
//     func_ov004_020b2220      126     <- this body, ~ every other frame
// and an argument probe on the trap showed the value arriving correctly and
// counting down:
//     call 0..7  x=128 y=40 val=10 d=1 e=-1 f=0x800 g=0
//     call 64    x=128 y=40 val=9  d=1 e=-1 f=0x800 g=0
// So the timer was always computed and always handed over. The stub that
// stood in for the drawer returned 0 and drew nothing. That is the whole
// defect: a draw gap, not a logic gap.
//
// ITS CALLER ON THIS PATH IS MATCHED AND SETTLES THE ARGUMENTS.
// src/func_ov006_020f0e28.c is dScMgLuigi_c's timer HUD:
//     if (*(int*)(this + 0x4f78) < 2) return;               // not counting yet
//     DrawOamSprite(lang_label[GetGameLanguage()], 0x80, 0x10, 0);
//     func_ov004_020b2220(0x80, 0x28, *(u16*)(this + 0x5172), 1, -1, 0x800, 0);
// The label sprite above the number always drew; only the number was missing.
// Five more matched TUs call it with the same shape (curling's countdown at
// src/func_ov006_020e1554.c, and 020ecee4 / 02109834 / 0211e29c / 02127d10),
// so the arity and the argument order are settled six times over and not by
// this file.
//
// PROVENANCE. Not a banked near-miss; nearmiss/db.jsonl has no draft for this
// symbol, and no branch in this repository -- origin/main included -- has ever
// carried a src TU for it. Read straight off a capstone listing of
//   extracted/overlays/overlay_0004.bin   at base 0x020AD660
// (ov004 base_address 34264672 from extracted/dsd/arm9_overlays/overlays.yaml
// id:4). The dsd export extracted/dsd/arm9_overlays/ov004.bin is the
// COMPRESSED payload and was not used: a prologue scan over all 282 of ov004's
// function symbols matches 207 at offset 0 in extracted/overlays/, and 62 at
// the best offset in the dsd export.
//
// EXTENT AND DELINK STATUS. config/arm9/overlays/ov004/symbols.txt:109
//     func_ov004_020b2220 kind:function(arm,size=0x224) addr:0x020b2220
// and config/arm9/overlays/ov004/delinks.txt COVERS NO PART OF IT: the blocks
// run  src/func_ov004_020b1ea4.c  .text 0x020b1ea4..0x020b2220  and then jump
// straight to  src/func_ov004_020b2444.c  .text 0x020b2444..0x020b2574. A
// name-shaped check would call this decompiled because symbols.txt names it;
// the delink join is what says otherwise. THIS FILE IS NOT A DECOMPILATION
// AND DOES NOT CLAIM TO BE ONE -- it carries a port_ name, the rule
// MgCurling_Collide_020e1dc8.cpp set, so nothing in this tree reads as a
// decomp contribution that does not exist. 0x020b2220 remains open decomp
// work.
//   0x020b2220..0x020b2434   code, 134 ARM instructions
//   0x020b2438..0x020b2443   literal pool, 3 words:
//       0x020b2438 = 0x0000270f   plain constant, 9999, no relocation
//       0x020b243c = 0x02082214   relocs.txt:463  kind:load module:main
//       0x020b2440 = 0x02137cd8   relocs.txt:464  kind:load module:overlay(6)
// Both relocated words already have names the build defines and other TUs
// already use: data_02082214 is the arm9 sin/cos table (src/
// Matrix4x3_FromRotationY.c and eleven others read it as `s16[]` with [i*2]
// sin and [i*2+1] cos), and data_ov006_02137cd8 is the digit glyph table that
// src/func_ov004_020b1ea4.c and src/func_ov004_020b2444.c index the same way.
// No new symbol is introduced here.
//
// EVERY CALLEE IS ALREADY IN THE BUILD, which is what makes this seat
// self-contained. The only function it calls is func_ov004_020b1c68, and that
// one is matched (src/func_ov004_020b1c68.c, a complete delink block at
// 0x020b1c68..0x020b1cf0) and already on port/slice_mg1.txt:283, as are the
// three drawers it forwards to (RenderOamBothScreens, func_ov004_020af868 at
// slice_mg1.txt:256 and func_ov004_020afc18 at :258). Nothing new was pulled
// into the slice for this.
//
// ---- HOW THE ROM BODY READS -----------------------------------------------
//
// Register map at entry, after `push {r4-r8,sb,sl,fp,lr}` (9 words) and
// `sub sp, sp, #0x1c`, so the three stacked arguments land at sp+0x40, sp+0x44
// and sp+0x48:
//     r0 -> sl   x          r1 -> sb   y          r2 -> r8   num
//     r3 -> r7   a3         sp+0x40 -> r6  a4     sp+0x44    scale
//     sp+0x48    angle
//
// THE CLAMP IS 9999, NOT 999999. 0x020b2228 loads 0x270f and 0x020b2234/38 do
// `cmp r8,r4 / movge r8,r4`. That is a different ceiling from the 0xf423f its
// small sibling func_ov004_020b1ea4 clamps at, and it is the right one for a
// drawer that lays out at most four digits.
//
// THE DIGIT SPLIT IS REPEATED SUBTRACTION, not division, and the three loops
// run thousands (0x3e8), hundreds (0x64), tens (0xa) in that order, leaving
// the units in r8. Transcribed as the same three loops rather than as / and %
// so that a reader diffing this against the listing sees the listing.
//
// THE MATRIX IS BUILT AT sp+8..sp+0x14 and passed by pointer as the sixth
// argument of every draw:
//     idx = (u16)angle >> 4
//     A   = (data_02082214[idx*2 + 1] * scale + 0x800) >> 12      (cos)
//     B   = (data_02082214[idx*2    ] * scale + 0x800) >> 12      (sin)
//     [sp+0x08] = A   [sp+0x0c] = B   [sp+0x10] = -B   [sp+0x14] = A
// which is { _00, _01, _10, _11 } = { cos, sin, -sin, cos } -- the exact
// struct and the exact rounding idiom src/func_ov004_020b369c.c already uses
// for the same callee, so the shape is settled by a matched TU and not by this
// file. The 64-bit intermediate is not cosmetic: the ROM does `smull` and
// carries the high word into the shift (`adds lo,#0x800 / adc hi,#0 /
// lo>>12 | hi<<20`), so the multiply must be done at 64 bits and narrowed
// after, which is what the s64 cast below is for.
//
// WITH THE CALLER'S ARGUMENTS scale=0x800 and angle=0 this comes out
// { 0x800, 0, 0, 0x800 }: an OAM affine matrix of 0.5 in 1.12, which is
// inverse-mapped and therefore draws at DOUBLE size. That is why the digit
// pitch below is 0x20 and not 0x10 -- a 16-pixel glyph at 2x -- and it is the
// consistency check that says the matrix halves are not swapped.
//
// THE LAYOUT IS CENTRED ON x, and the four cases are written out separately in
// the ROM rather than looped:
//     4 digits   x-0x30  x-0x10  x+0x10  x+0x30
//     3 digits   x-0x20  x       x+0x20
//     2 digits   x-0x10  x+0x10
//     1 digit    x
// Each branch advances x by its own amount and falls into the shared units
// draw at 0x020b2408, which is why the units digit is drawn once at the bottom
// instead of four times.
//
// LEADING ZEROES ARE SUPPRESSED BY THE if/else-if CHAIN, so a value of 9 draws
// one digit at x and a value of 10 draws two at x-0x10 and x+0x10. That is the
// behaviour the "Wanted!" timer wants and it is the ROM's, not a choice made
// here.
//
// A NEGATIVE num IS NOT DEFENDED AGAINST, and that is deliberate fidelity: the
// clamp at 0x020b2234 is a signed `movge`, the three loops are `blt`-guarded
// and do not run, and the final draw indexes data_ov006_02137cd8 with the
// negative value. The ROM reads out of bounds there and so does this. Every
// one of the six matched callers passes an unsigned short or a clamped
// counter, so no caller in the tree can reach it.
//
// ---- WIRING ---------------------------------------------------------------
//
// hal/scene_mg_faces.cpp's trap for this address is replaced by a forwarder to
// the port_ name below, the same two-part shape CUR2 used for the two curling
// collision bodies. The forwarder keeps the ROM's seven-argument arity, which
// the six matched callers already declare and pass; the trap it replaces took
// (void *) and ignored all seven.
//
// port/CMakeLists.txt appends this file to SCENE_MG_SOURCES, next to the two
// curling collision copies and for the identical reason: hal/scene_mg_faces.cpp
// now calls port_mg_hud_scaled_number_020b2220 by name, so a target that
// compiles the faces and not this file fails the link on that symbol.

#include <cstdio>
#include <cstdlib>

typedef short          s16;
typedef unsigned short u16;
typedef long long      s64;

/* The 2x2 OAM affine matrix, spelled exactly as the matched callers of the
   same callee spell it (src/func_ov004_020b369c.c, src/func_ov004_020b38ac.c). */
struct MgHudMtx { int _00, _01, _10, _11; };

extern "C" {
extern s16   data_02082214[];        /* arm9 sin/cos table, [i*2]=sin [i*2+1]=cos */
extern void *data_ov006_02137cd8[];  /* the digit glyph table                     */
void func_ov004_020b1c68(void *glyph, int x, int y, int a3, int a4,
                         struct MgHudMtx *m);
}

/* A witness, the shape every host copy in this directory carries: a run that
   draws the timer and a run that never reaches it are different facts, and the
   aggregate trap counter can no longer tell them apart now the trap is gone. */
static unsigned g_hud_scaled_number_entries;

extern "C" unsigned port_mg_hud_scaled_number_entries(void)
{
    return g_hud_scaled_number_entries;
}

extern "C" void port_mg_hud_scaled_number_020b2220(int x, int y, int num,
                                                   int a3, int a4,
                                                   int scale, int angle)
{
    int thousands = 0;
    int hundreds  = 0;
    int tens      = 0;
    int idx;
    struct MgHudMtx m;

    ++g_hud_scaled_number_entries;

    /* 0x020b2228 / 0x020b2234 / 0x020b2238: ldr 0x270f, cmp, movge */
    if (num >= 0x270f)
        num = 0x270f;

    /* 0x020b2254..0x020b2298: three repeated-subtraction loops */
    while (num >= 0x3e8) { num -= 0x3e8; ++thousands; }
    while (num >= 0x64)  { num -= 0x64;  ++hundreds;  }
    while (num >= 0xa)   { num -= 0xa;   ++tens;      }

    /* 0x020b229c..0x020b2300: the rotate-and-scale matrix at sp+8 */
    idx    = (int)((u16)angle) >> 4;
    m._00  = (int)(((s64)data_02082214[idx * 2 + 1] * scale + 0x800) >> 12);
    m._01  = (int)(((s64)data_02082214[idx * 2    ] * scale + 0x800) >> 12);
    m._10  = -m._01;
    m._11  = m._00;

    /* 0x020b2304..0x020b2404: the centred layout, leading zeroes suppressed */
    if (thousands != 0) {
        func_ov004_020b1c68(data_ov006_02137cd8[thousands], x - 0x30, y, a3, a4, &m);
        func_ov004_020b1c68(data_ov006_02137cd8[hundreds],  x - 0x10, y, a3, a4, &m);
        func_ov004_020b1c68(data_ov006_02137cd8[tens],      x + 0x10, y, a3, a4, &m);
        x += 0x30;
    } else if (hundreds != 0) {
        func_ov004_020b1c68(data_ov006_02137cd8[hundreds], x - 0x20, y, a3, a4, &m);
        func_ov004_020b1c68(data_ov006_02137cd8[tens],     x,        y, a3, a4, &m);
        x += 0x20;
    } else if (tens != 0) {
        func_ov004_020b1c68(data_ov006_02137cd8[tens], x - 0x10, y, a3, a4, &m);
        x += 0x10;
    }

    /* 0x020b2408..0x020b2428: the shared units draw every path falls into */
    func_ov004_020b1c68(data_ov006_02137cd8[num], x, y, a3, a4, &m);
}
