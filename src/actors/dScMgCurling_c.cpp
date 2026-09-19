//cpp
/* dScMgCurling_c -- the one-player curling minigame scene, ov006.
 *
 * Reconstructed translation unit: ROM ordinals 0..26 of the linker run
 * 0x020e0638..0x020e3854, i.e. .text 0x020e0638..0x020e20bc, 27 functions.
 * Assembled from the 27 one-function legacy sources, then reconciled by hand.
 *
 * WHY THIS IS A SUB-RANGE: a non-matching member splits the run.  ROM ordinal
 * 27, func_ov006_020e20bc (0x020e20bc, size 0x5e0), has a source file but it
 * is an honest non-matching draft -- 299 divergent words of 376 -- and so it
 * carries NO entry in config/arm9/overlays/ov006/delinks.txt at all (the
 * blocks jump from 0x020e1dc8-0x020e20bc straight to 0x020e269c) and
 * tools/enroll.py leaves it out of the ROM build, which keeps the cartridge's
 * own bytes for that range.  It sits in the MIDDLE of the run, and nothing in
 * this tree can express a .text claim with a hole in it -- no file entry in
 * any delinks.txt anywhere carries two .text claims -- so the run has to be
 * licensed as one of its two contiguous sides.
 *
 * THIS FILE IS THE LOWER SIDE, ordinals 0..26 -- 27 functions against 19 on
 * the far side.  It is also the side that holds the class's key function: the
 * destructor, which include/dScMgCurling_c.h declares out of line and declares
 * FIRST.  The 19 members above the hole (func_ov006_020e269c through
 * dScMgCurling_c_classInit, including InitResources, Behavior, Render and
 * OnYoshiTryEat) keep their own shards and their own delinks.txt entries.
 *
 * THE RUN'S TWO BOUNDARIES ARE MEASURED, NOT ASSUMED.  A ROM-wide relocation
 * census -- every relocs.txt in the tree, filtered to targets whose module SET
 * contains 6, with each referring word resolved back to its containing
 * function or data symbol -- gives every member of 0x020e0638..0x020e3854 a
 * referrer set drawn from exactly four places: this class's own
 * _ZTV14dScMgCurling_c, its own factory record g_profile_MG_CURLING, a direct
 * call from another member of this same run, and a pointer-to-member table in
 * the narrow .data band 0x0213c1e4..0x0213c2bc that sits directly below this
 * class's own _ZTI.  Nothing outside ov006 refers into the run at all.  The
 * same census over two sibling ov006 scenes returns three DISJOINT .data bands
 * (dScMgCup_c 0x0213bfe8..0x0213c040, dScMgCurling2_c 0x0213c3d4..0x0213c4b4),
 * each below its own class's _ZTI, so the census discriminates rather than
 * returning the same answer for everything.
 *
 * BELOW the run, func_ov006_020e0634 (4 bytes, 0x020e0634) is NOT ours: its
 * only referrer is dScMgCup_c_classInit, and src/actors/dScMgCup_c.cpp already
 * licenses .text through 0x020e0638, which contains it.  ABOVE the run,
 * 0x020e3854 is dScMgCurling2_c's own D1 and the start of an already-promoted
 * TU.
 *
 * VTABLE, MEASURED OUT OF extracted/overlays/overlay_0006.bin.
 * _ZTV14dScMgCurling_c is 0x90 bytes, 36 slots, the symbol sitting at the
 * address point.  Six of them pin members of this run by name -- slot 0
 * InitResources, slot 6 Behavior, slot 9 Render, slot 16 this file's D1, slot
 * 17 its D0, slot 18 OnYoshiTryEat -- and that is the positive control for the
 * census above.  dScMgCurling2_c and dScMgD3DBase_c carry vtables of exactly
 * the same 36-slot length with the same inherited slot values, which is what
 * proves the base is dScMgBase_c; the 31-word/32-word length rule that
 * separates dActor_c from dBgActor_c is an actor rule and does not reach this
 * family.
 *
 * SLOTS 0 AND 3 ARE NOT ABSTRACT HERE, AND THAT HYPOTHESIS IS REFUTED, NOT
 * DROPPED.  Both are live relocated words -- slot 0 to this class's own
 * InitResources at 0x020e3578, slot 3 to 0x020b0840 in the ov000/ov004 pair --
 * so the "abstract slot 0/3 hides unlabelled helpers at the TU edges" reading
 * buys nothing on dScMgCurling_c.  The unlabelled helpers in this run are
 * unlabelled for the ordinary reason: they are non-virtual file-local members
 * reached through the pointer-to-member tables, and the census above is what
 * attaches them to the class.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, LOWEST ADDRESS FIRST, and that is one
 * decision with `#pragma defer_codegen off` below.  With codegen deferred
 * (the default) mwccarm 2004/b56 emits one .text section per function in the
 * REVERSE of source order; generating at parse time emits them in source
 * order.  Do not reorder.
 *
 * THE DESTRUCTOR IS OUT OF LINE AND THE CARTRIDGE ORDERS IT D1 (0x020e0638)
 * BELOW D0 (0x020e065c), adjacent, with no room between them for a D2 -- and
 * there is no _ZN14dScMgCurling_cD2Ev anywhere in the image.  One out-of-line
 * definition at the end of the source therefore has to produce D1 then D0 and
 * nothing in between.  See the manifest's dtor grid for the cells that were
 * compiled and what each one emitted.
 *
 * Because the destructor is out of line and declared first, this TU owns the
 * class's key function, so mwcc emits the whole inheritance chain's vtable and
 * typeinfo as vague-linkage passengers -- see the manifest's
 * compiler_only_output block.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020e0638  the legacy D1 shard
 *   [1] 0x020e065c  the legacy D0 shard
 *   [2] 0x020e0694  the legacy func_ov006_020e0694 shard
 *   [3] 0x020e071c  the legacy func_ov006_020e071c shard
 *   [4] 0x020e07b0  the legacy func_ov006_020e07b0 shard
 *   [5] 0x020e0884  the legacy func_ov006_020e0884 shard
 *   [6] 0x020e091c  the legacy func_ov006_020e091c shard
 *   [7] 0x020e0a24  the legacy func_ov006_020e0a24 shard
 *   [8] 0x020e0b64  the legacy func_ov006_020e0b64 shard
 *   [9] 0x020e0ca0  the legacy func_ov006_020e0ca0 shard
 *   [10] 0x020e0d84 the legacy func_ov006_020e0d84 shard
 *   [11] 0x020e0e18 the legacy func_ov006_020e0e18 shard
 *   [12] 0x020e0edc the legacy func_ov006_020e0edc shard
 *   [13] 0x020e0ff0 the legacy func_ov006_020e0ff0 shard
 *   [14] 0x020e1100 the legacy func_ov006_020e1100 shard
 *   [15] 0x020e1214 the legacy func_ov006_020e1214 shard
 *   [16] 0x020e1264 the legacy func_ov006_020e1264 shard
 *   [17] 0x020e12d0 the legacy func_ov006_020e12d0 shard
 *   [18] 0x020e13a4 the legacy func_ov006_020e13a4 shard
 *   [19] 0x020e1554 the legacy func_ov006_020e1554 shard
 *   [20] 0x020e1608 the legacy func_ov006_020e1608 shard
 *   [21] 0x020e1680 the legacy func_ov006_020e1680 shard
 *   [22] 0x020e17f8 the legacy func_ov006_020e17f8 shard
 *   [23] 0x020e1854 the legacy func_ov006_020e1854 shard
 *   [24] 0x020e1b54 the legacy func_ov006_020e1b54 shard
 *   [25] 0x020e1c68 the legacy func_ov006_020e1c68 shard
 *   [26] 0x020e1dc8 the legacy func_ov006_020e1dc8 shard
 */

#pragma defer_codegen off

#include "types.h"
#include "dScMgCurling_c.h"

/* ---------------------------------------------------------------------------
 * Shadow types.  Four members dispatch through the ov006 pointer-to-member
 * tables, and every legacy shard spelled the receiver the same way: an
 * INCOMPLETE `struct C`.  Incomplete is load-bearing -- mwccarm 2004/b56 picks
 * its pointer-to-member representation from the completeness of the class, so
 * completing it here would change the codegen of all four.  Kept as the shards
 * had it.
 * ------------------------------------------------------------------------- */
struct C;
typedef void (C::*PMF)(int);
struct Entry { PMF pmf; };

/* The 0x473c array as one shard spelled it: five 0x10-byte records.  Only
   ordinal 19 uses the typed view; every other member reaches the same bytes
   through raw offsets, and that difference is what the ROM has. */
typedef struct {
    int a;
    int b;
    unsigned short c;
    unsigned char p0[3];
    unsigned char flag;
    unsigned char p1[2];
} Elem;

typedef struct {
    unsigned char _pad[0x473c];
    Elem arr[5];
    unsigned char _pad2[0x4ee8 - 0x473c - 5*16];
    unsigned char go;
} Obj;

/* data_020a0dea/deb are four-byte records, the same B4 shape
   src/minigames/d_s_mg_trampoline.cpp already declares for these globals.
   Ordinal 23 needs the record type and a `u8 *` cast across it: see that
   member's own note -- a plain `u8 []` extern costs eighteen words there. */
struct B4 { unsigned char v; unsigned char pad[3]; };

#define F0(b,i)  (*(int*)  ((char*)(b) + 0x478c + (i)*0x24))
#define F4(b,i)  (*(int*)  ((char*)(b) + 0x4790 + (i)*0x24))
#define F8(b,i)  (*(int*)  ((char*)(b) + 0x4794 + (i)*0x24))
#define FC(b,i)  (*(int*)  ((char*)(b) + 0x4798 + (i)*0x24))
#define F14(b,i) (*(unsigned short*)((char*)(b) + 0x47a0 + (i)*0x24))
#define F14S(b,i) (*(short*)((char*)(b) + 0x47a0 + (i)*0x24))
#define F16(b,i) (*(unsigned short*)((char*)(b) + 0x47a2 + (i)*0x24))
#define F16S(b,i) (*(short*)((char*)(b) + 0x47a2 + (i)*0x24))
#define F1E(b,i) (*(unsigned char*)((char*)(b) + 0x47aa + (i)*0x24))
#define M(p) ((int *)(int)(p))

/* ---------------------------------------------------------------------------
 * ROM symbols this TU calls or reads, plus the forward declarations the
 * members need of each other.  ONE file-scope `extern "C"` region: a class
 * member function cannot sit inside an `extern "C" { }` block at all under
 * mwccarm 2004/b56, and a mangled ROM spelling outside one would be mangled a
 * second time -- a LINK failure that every byte gate passes.
 * ------------------------------------------------------------------------- */
extern "C" {

extern int  func_ov004_020adbc0(void);
extern void func_ov004_020af948(int a, int b, int c, int d);
extern int  func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern void func_ov004_020b2220(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_02012718(int a, int b);
extern void DrawOamSprite(int a, int b, int c, int d);
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern int  RandomIntInternal(int *seed);
extern int  _ZN4cstd4sqrtEy(u64);
extern int  _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

extern int  data_0209d4b8;
extern s16  data_02082214[];
extern u8   data_020a0de8[];
extern u8   data_020a0de9[];
extern struct B4 data_020a0dea[];
extern struct B4 data_020a0deb[];
extern u8   data_020a0e40[];
extern unsigned char data_ov006_0212e450[];
extern unsigned char data_ov006_0212e454[];
extern u8   data_ov006_0212e458[];
extern int  data_ov006_0212e460[];
extern int  data_ov006_0212e468[];
extern int  data_ov006_0213a5e0[];
extern int  data_ov006_0213c264;
extern int  data_ov006_0213c2ac;
extern int  data_ov006_0213c2e4[];

/* The four pointer-to-member tables this side of the hole dispatches through.
   __sinit_ov006_* fills them from the 8-byte {code pointer, zero adjustment}
   records in ov006 .data. */
extern PMF   data_ov006_021418c0[];
extern PMF   data_ov006_021418d8[];
extern Entry data_ov006_021418f0[];
extern PMF   data_ov006_02141930[];

/* This TU's own members, forward-declared for the callers below them. */
extern void func_ov006_020e0694(char *c);
extern void func_ov006_020e071c(char *c, int i);
extern void func_ov006_020e07b0(char *o, int i);
extern void func_ov006_020e0884(char *c, int i);
extern void func_ov006_020e091c(char *base, int i);
extern void func_ov006_020e0a24(char *base, int idx);
extern void func_ov006_020e0b64(char *base, int index);
extern void func_ov006_020e0ca0(char *o, int i);
extern void func_ov006_020e0d84(char *c, int i);
extern void func_ov006_020e0e18(char *base, int idx);
extern void func_ov006_020e0edc(char *c, int idx);
extern void func_ov006_020e0ff0(void *base, int idx);
extern void func_ov006_020e1100(char *c, int idx);
extern void func_ov006_020e1214(char *base, int idx);
extern void func_ov006_020e1264(char *c, int idx);
extern void func_ov006_020e12d0(char *o);
extern void func_ov006_020e13a4(char *c);
extern void func_ov006_020e1554(Obj *o);
extern void func_ov006_020e1608(char *self);
extern void func_ov006_020e1680(char *o);
extern void func_ov006_020e17f8(char *self);
extern void func_ov006_020e1854(void *arg);
extern void func_ov006_020e1b54(char *c);
extern void func_ov006_020e1c68(char *a0);
extern void func_ov006_020e1dc8(dScMgCurling_c *self, int idx);

}  /* extern "C" */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN14dScMgCurling_cD1Ev, 0x020e0638, size 0x24 */
/* ROM ordinal 1 -- _ZN14dScMgCurling_cD0Ev, 0x020e065c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgCurling_cD1Ev
// @symbol _ZN14dScMgCurling_cD0Ev
dScMgCurling_c::~dScMgCurling_c()
{
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_020e0694, 0x020e0694, size 0x88 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0694
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0694(char *c)
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(c + 0x4000 + 0x7ac)) {
            int x = *(int *)(c + 0x4000 + 0x78c) >> 0xc;
            int y = *(int *)(c + 0x4000 + 0x790) >> 0xc;
            func_ov004_020af948(data_ov006_0213a5e0[*(unsigned char *)(c + 0x4000 + 0x7ad)], x, y, 0);
            DrawOamSprite(data_ov006_0213a5e0[*(unsigned char *)(c + 0x4000 + 0x7ae)], x, y, 0);
        }
        c += 0x24;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020e071c, 0x020e071c, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e071c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e071c(char *c, int i)
{
    int idx = i * 0x24;
    if (*(unsigned short *)(c + 0x47a4 + idx) != 0) {
        short *p = (short *)(c + 0x47a4 + idx);
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (*(int *)(c + 0x4798 + idx) > 0x100) {
        int *q = (int *)(c + 0x4798 + idx);
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        *(unsigned char *)(c + 0x4000 + idx + 0x7ab) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020e07b0, 0x020e07b0, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e07b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e07b0(char *o, int i)
{
    int n = i * 0x24;
    if (*(int *)(o + 0x479c + n) > *(int *)(o + 0x4798 + n)) {
        *(int *)(o + 0x4798 + n) += 0x10;
        if (*(int *)(o + 0x479c + n) > *(int *)(o + 0x4798 + n))
            *(int *)(o + 0x4798 + n) = *(int *)(o + 0x479c + n);
    }
    if (*(unsigned short *)(o + 0x47a4 + n) != 0) {
        *(unsigned short *)(o + 0x47a4 + n) = *(unsigned short *)(o + 0x47a4 + n) - 1;
        if (*(short *)(o + 0x47a4 + n) < 0) *(short *)(o + 0x47a4 + n) = 0;
    } else {
        *(unsigned char *)(o + n + 0x47ab) = 2;
        *(short *)(o + 0x47a4 + n) = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020e0884, 0x020e0884, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0884
extern "C" {
extern int RandomIntInternal(int* seed);
extern int data_0209d4b8;

void func_ov006_020e0884(char* c, int i) {
  int idx = i * 0x24;
  unsigned int r;
  *(int*)(c + 0x4798 + idx) = 0;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  *(int*)(c + 0x479c + idx) = (((r << 4) >> 15) << 4) + 0x300;
  *(unsigned char*)(c + 0x47ab + idx) = 1;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  r = ((r << 5) >> 15) + 0x20;
  *(short*)(c + 0x47a4 + idx) = (unsigned char)r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020e091c, 0x020e091c, size 0x108 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e091c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e091c(char *base, int i)
{
    int n = i * 0x24;
    char *a4794 = base + 0x4794;
    char *a478c = base + 0x478c;
    char *a4790 = base + 0x4790;
    *(int*)(a478c + n) = *(int*)(a478c + n) + *(int*)(a4794 + n);
    *(int*)(a4790 + n) = *(int*)(a4790 + n) + *(int*)(base + n + 0x4798);
    if (*(u16*)(base + n + 0x47a0) != 0) {
        char *a47a2 = base + 0x47a2;
        *(u16*)(a47a2 + n) = *(u16*)(a47a2 + n) - 1;
        if (*(s16*)(a47a2 + n) < 0) *(s16*)(a47a2 + n) = 0;
        return;
    }
    if (*(int*)(a4794 + n) > 0) {
        *(int*)(a4794 + n) = *(int*)(a4794 + n) - 8;
        if ((s16)*(int*)(a4794 + n) < 0) *(int*)(a4794 + n) = 0;
        return;
    }
    if (*(int*)(a4794 + n) < 0) {
        *(int*)(a4794 + n) = *(int*)(a4794 + n) + 8;
        if (*(int*)(a4794 + n) > 0) *(int*)(a4794 + n) = 0;
        return;
    }
    *(u8*)(base + n + 0x47aa) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020e0a24, 0x020e0a24, size 0x140 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0a24
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0a24(char *base, int idx)
{
    unsigned short v;

    F0(base, idx) = F0(base, idx) + F8(base, idx);
    F4(base, idx) = F4(base, idx) + FC(base, idx);

    if (F14(base, idx) != 0) {
        F14(base, idx) = F14(base, idx) - 1;
        if (F14S(base, idx) < 0)
            F14(base, idx) = 0;
        return;
    }

    if (F8(base, idx) > -0x300) {
        F8(base, idx) -= 8;
        if (F8(base, idx) <= -0x300)
            F8(base, idx) = 0x300;
    }

    v = F16(base, idx);
    if (v != 0) {
        F16(base, idx) = v - 1;
        if (F16S(base, idx) < 0)
            F16(base, idx) = 0;
        return;
    }

    F1E(base, idx) = 3;
    F16(base, idx) = (unsigned char)(((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf) + 0x20);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020e0b64, 0x020e0b64, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0b64
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0b64(char *base, int index)
{
    int i = index * 0x24;
    unsigned short v;

    *(int *)(base + 0x478c + i) += *(int *)(base + 0x4794 + i);
    *(int *)(base + 0x4790 + i) += *(int *)(base + 0x4798 + i);

    v = *(unsigned short *)(base + 0x47a0 + i);
    if (v != 0) {
        *(short *)(base + 0x47a0 + i) = v - 1;
        if (*(short *)(base + 0x47a0 + i) < 0)
            *(short *)(base + 0x47a0 + i) = 0;
        return;
    }

    if (*(int *)(base + 0x4794 + i) < 0x300) {
        *(int *)(base + 0x4794 + i) += 8;
        if (*(int *)(base + 0x4794 + i) >= 0x300)
            *(int *)(base + 0x4794 + i) = 0x300;
    }

    v = *(unsigned short *)(base + 0x47a2 + i);
    if (v != 0) {
        *(short *)(base + 0x47a2 + i) = v - 1;
        if (*(short *)(base + 0x47a2 + i) < 0)
            *(short *)(base + 0x47a2 + i) = 0;
        return;
    }

    *(char *)(base + 0x47aa + i) = 3;
    *(short *)(base + 0x47a2 + i) = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020e0ca0, 0x020e0ca0, size 0xe4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0ca0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0ca0(char *o, int i)
{
    int n = i * 0x24;
    if (*(unsigned short *)(o + 0x47a0 + n) != 0) {
        *(unsigned short *)(o + 0x47a0 + n) = *(unsigned short *)(o + 0x47a0 + n) - 1;
        if (*(short *)(o + 0x47a0 + n) < 0) *(short *)(o + 0x47a0 + n) = 0;
        return;
    }
    *(int *)(o + 0x4794 + n) = 0;
    *(unsigned char *)(o + 0x47aa + n) = data_ov006_0212e450[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(o + 0x47a0 + n) = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    *(unsigned short *)(o + 0x47a2 + n) = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020e0d84, 0x020e0d84, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0d84
extern "C" void func_ov006_020e0d84(char *c, int i)
{
    C *self = (C *)c;
    int idx = i * 0x24;
    unsigned char k0 = *(unsigned char *)(c + idx + 0x4000 + 0x7aa);
    (self->*data_ov006_02141930[k0])(i);
    unsigned char k1 = *(unsigned char *)((char *)self + idx + 0x4000 + 0x7ab);
    (self->*data_ov006_021418d8[k1])(i);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020e0e18, 0x020e0e18, size 0xc4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0e18
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0e18(char *base, int idx) {
    int off = idx * 0x24;
    int *a = (int *)(base + 0x478c + off);
    int *c = (int *)(base + 0x4794 + off);
    int *b = (int *)(base + 0x4790 + off);
    *a += *c;
    *b += *(int *)(base + off + 0x4798);
    if (*c > 0) {
        *c -= 0x20;
        if ((int)(short)*c < 0) *c = 0;
    } else if (*c < 0) {
        *c += 0x20;
        if (*c > 0) *c = 0;
    } else {
        *(unsigned char *)(base + off + 0x47aa) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020e0edc, 0x020e0edc, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0edc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0edc(char *c, int idx)
{
    char *b = c;
    int off = idx * 0x24;

    *(int *)(b + 0x478c + off) += *(int *)(b + 0x4794 + off);
    *(int *)(b + 0x4790 + off) +=
        *(int *)((char *)b + off + 0x4000 + 0x798);

    u16 *p1 = (u16 *)(b + 0x47a0 + off);
    if (*p1 != 0) {
        *p1 = *p1 - 1;
        s16 v = *(s16 *)p1;
        if (v < 0) {
            *p1 = 0;
        }
        return;
    }

    int *p2 = (int *)(b + 0x4794 + off);
    if (*p2 > -0x400) {
        *p2 -= 0x20;
        if (*p2 <= -0x400) {
            *p2 = 0x400;
        }
    }

    u16 *p3 = (u16 *)(b + 0x47a2 + off);
    if (*p3 != 0) {
        *p3 = *p3 - 1;
        s16 v = *(s16 *)p3;
        if (v < 0) {
            *p3 = 0;
        }
        return;
    }

    *(unsigned char *)(b + off + 0x4000 + 0x7aa) = 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020e0ff0, 0x020e0ff0, size 0x110 */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma inline_depth(0)
// @symbol func_ov006_020e0ff0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e0ff0(void* base, int idx) {
    char* b = (char*)base;
    int off = idx * 0x24;

    *(int*)(b + 0x478c + off) += *(int*)(b + 0x4794 + off);
    *(int*)(b + 0x4790 + off) += *(int*)((char*)base + off + 0x4000 + 0x798);

    unsigned short* p1 = (unsigned short*)(b + 0x47a0 + off);
    if (*p1 != 0) {
        *p1 = *p1 - 1;
        short v = *(short*)p1;
        if (v < 0) {
            *p1 = 0;
        }
        return;
    }

    int* p2 = (int*)(b + 0x4794 + off);
    if (*p2 < 0x400) {
        *p2 = *p2 + 0x20;
        if (*p2 >= 0x400) {
            *p2 = 0x400;
        }
    }

    unsigned short* p3 = (unsigned short*)(b + 0x47a2 + off);
    if (*p3 != 0) {
        *p3 = *p3 - 1;
        short v = *(short*)p3;
        if (v < 0) {
            *p3 = 0;
        }
        return;
    }

    *(char*)(b + off + 0x4000 + 0x7aa) = 3;
}
#pragma pop
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020e1100, 0x020e1100, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e1100
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1100(char *c, int idx)
{
    int n = idx * 0x24;
    unsigned short d;
    short r;
    unsigned int v;
    d = *(unsigned short *)(c + 0x47a0 + n);
    if (d != 0) {
        r = (short)(d - 1);
        *(short *)(c + 0x47a0 + n) = r;
        if (*(short *)(c + 0x47a0 + n) < 0) *(short *)(c + 0x47a0 + n) = 0;
        return;
    }
    *(int *)(c + 0x4794 + n) = 0;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(int *)(c + 0x4798 + n) = (int)(((v << 5) >> 15) << 4) + 0x600;
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(unsigned char *)(c + 0x47aa + n) = data_ov006_0212e454[(v << 1) >> 15];
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(c + 0x47a0 + n) = (unsigned char)((v << 4) >> 15);
    v = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    *(short *)(c + 0x47a2 + n) = (unsigned char)(((v * 0x30) >> 15) + 0x30);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020e1214, 0x020e1214, size 0x50 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e1214
extern "C" void func_ov006_020e1214(char *base, int idx)
{
    unsigned char state = F1E(base, idx);
    (((C*)base)->*data_ov006_021418f0[state].pmf)(idx);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020e1264, 0x020e1264, size 0x6c */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_propagation off
// @symbol func_ov006_020e1264
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1264(char *c, int idx) {
    unsigned r = (unsigned)RandomIntInternal(&data_0209d4b8);
    int k = 0;
    unsigned m = ((r >> 16) & 0x7fff) << 3 >> 0xf;
    if (m == 5) k = 1;
    char *p = c + idx * 0x24;
    *(unsigned char *)(p + 0x47a9) = data_ov006_0212e460[k];
    *(unsigned char *)(p + 0x47aa) = 0;
}
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020e12d0, 0x020e12d0, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e12d0
extern "C" void func_ov006_020e12d0(char *o)
{
    int i;
    char *e = o;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(e + 0x47a8) != 0) {
            unsigned char idx = *(unsigned char *)(e + 0x47a9);
            (((C *)o)->*data_ov006_021418c0[idx])(i);
            if ((*(int *)(e + 0x4790) >> 0xc) >= 0xc8) {
                *(int *)(e + 0x478c) = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(e + 0x4790) = -0x8000;
                *(unsigned char *)(e + 0x47aa) = 0;
                *(unsigned char *)(e + 0x47a9) = 0;
                *(unsigned char *)(e + 0x47ab) = 0;
            }
        }
        e += 0x24;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020e13a4, 0x020e13a4, size 0x1b0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e13a4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e13a4(char *c)
{
    int i;
    char *p;
    unsigned int r;
    unsigned int v;
    int q;
    unsigned int m;

    i = 0;
    p = c;
    for (; i < 0x32; i++)
    {
        *(int *)(p + 0x4000 + 0x78c) = 0;
        *(int *)(p + 0x4000 + 0x790) = 0;
        *(int *)(p + 0x4000 + 0x794) = 0;
        *(int *)(p + 0x4000 + 0x798) = 0;
        *(short *)(p + 0x4700 + 0xa0) = 0;
        *(short *)(p + 0x4700 + 0xa2) = 0;
        *(short *)(p + 0x4700 + 0xa4) = 0;
        *(char *)(p + 0x4000 + 0x7a8) = 0;
        *(char *)(p + 0x4000 + 0x7a9) = 0;
        *(char *)(p + 0x4000 + 0x7aa) = 0;
        *(char *)(p + 0x4000 + 0x7ab) = 0;
        *(char *)(p + 0x4000 + 0x7ac) = 0;
        *(char *)(p + 0x4000 + 0x7ad) = 0;
        *(char *)(p + 0x4000 + 0x7ae) = 1;
        p += 0x24;
    }

    i = 0;
    p = c;
    for (; i < 0x32; i++)
    {
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x78c) = (int)((m >> 0xf)) << 0xf;
        *(int *)(p + 0x4000 + 0x790) = -0x8000;
        *(char *)(p + 0x4000 + 0x7a8) = 1;
        *(char *)(p + 0x4000 + 0x7ac) = 1;
        *(char *)(p + 0x4000 + 0x7a9) = 0;
        *(char *)(p + 0x4000 + 0x7aa) = 0;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(p + 0x4000 + 0x7ad) = (char)(((r >> 16) & 0x7fff) * 5 >> 0xf);

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(p + 0x4000 + 0x7ad) + ((((r >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(p + 0x4000 + 0x7ae) = (char)v;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(short *)(p + 0x4700 + 0xa0) = (short)(((i & 7) << 6) + (((r >> 16) & 0x7fff) * 0x30 >> 0xf));

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x78c) = (int)((m >> 0xf)) << 0xf;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((r >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(p + 0x4000 + 0x790) = (((q << 3) - 8)) << 0xc;
        *(short *)(p + 0x4700 + 0xa0) = 0;
        p += 0x24;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020e1554, 0x020e1554, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e1554
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1554(Obj *o)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (o->arr[i].flag != 0) {
            func_ov004_020b2444(o->arr[i].a >> 12, o->arr[i].b >> 12, o->arr[i].c, -1, -1, 0, 0);
        }
    }
    if (o->go != 0) {
        int r = func_ov004_020adbc0();
        func_ov004_020b2220(0x80, 0x60, r, 1, 0, 0x800, 0);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020e1608, 0x020e1608, size 0x78 */
/* -------------------------------------------------------------------------- */
#pragma push
// @symbol func_ov006_020e1608
extern "C" {  /* .c-derived member: C linkage for the whole block */
void _ZN5Sound12PlayBank2_2DEj(unsigned int);

#pragma opt_strength_reduction off
void func_ov006_020e1608(char *self) {
    int i;
    for (i = 0; i < 5; i++) {
        char *base = self + (i << 4);
        if (*(unsigned char*)(base + 0x4748) == 0) continue;
        if (*(unsigned short*)(base + 0x4746) == 0) continue;
        {
            volatile unsigned short *t = (volatile unsigned short*)((unsigned int)base + 0x4746);
            *t = *t - 1;
            if (*t != 0) continue;
        }
        *(unsigned char*)(base + 0x4749) = 1;
        _ZN5Sound12PlayBank2_2DEj(0x1bc);
    }
}
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020e1680, 0x020e1680, size 0x178 */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_strength_reduction off
// @symbol func_ov006_020e1680
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1680(char *o)
{
    int slot = 0;
    int i, j;
    char *e;
    u8 *p;

    for (j = 0; j < 5; j++) {
        if (*(u8 *)(o + j * 16 + 0x4748) == 0) {
            slot = j;
            break;
        }
    }

    p = &data_ov006_0212e458[slot];
    e = o;
    for (i = 0; i < 5; i++, e += 0x2c) {
        int dx, dz, d, t;

        if (*(u8 *)(e + 0x4689) == 0)
            continue;
        dx = *(int *)(e + 0x4660) - *(int *)(o + 0x4e94);
        dz = *(int *)(e + 0x4664) - *(int *)(o + 0x4e98);
        {
            int ax = dx >> 12;
            int az = dz >> 12;
            d = _ZN4cstd4sqrtEy((u64)(s64)(ax * ax + az * az));
        }
        *(u8 *)(o + slot * 16 + 0x4748) = 1;
        *(int *)(o + slot * 16 + 0x473c) = *(int *)(e + 0x4660);
        {
        int *py = M(o + slot * 16 + 0x4740);
        *py = *(int *)(e + 0x4664) + 0x1000;
        *(short *)(o + slot * 16 + 0x4746) = *p;
        if (d <= 8)
            *(short *)(o + slot * 16 + 0x4744) = 1000;
        else if (d <= 0x18)
            *(short *)(o + slot * 16 + 0x4744) = 500;
        else if (d <= 0x28)
            *(short *)(o + slot * 16 + 0x4744) = 300;
        else if (d <= 0x38)
            *(short *)(o + slot * 16 + 0x4744) = 100;
        else
            *(short *)(o + slot * 16 + 0x4744) = 0;
        t = *py >> 12;
        if (t >= -32 && t <= 8)
            *py = -0x28000;
        }
        p++;
        slot++;
    }
}
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020e17f8, 0x020e17f8, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e17f8
/* ov006, 0x020e17f8, size 0x5c. RESOLVED: this held the name
 * _ZN6Player16St_WallJump_InitEv, and that symbol has now been moved to the
 * ov002 function at the same shared address -- the one that adds half a turn
 * to the facing angle and launches, reached from ov002's Player::State
 * pointer-to-member table (two `kind:load ... module:overlay(2)` sites).
 *
 * This function is reached by an arm_call from ov006, module:overlay(6), and
 * reads this+0x4eb0, this+0x4eb4 and this+0x4ee5 -- roughly 0x4700 bytes past
 * the end of a Player, which is 0x768. It was never Player's. Back to a
 * placeholder name until its own class is identified; the offsets stay
 * untyped, because typing them was always the wrong repair.
 */
extern "C" void func_ov006_020e17f8(char *self)
{
  if(*(unsigned char*)(self+0x4ee5)==0) return;
  int x=*(int*)(self+0x4eb0);
  int y=*(int*)(self+0x4eb4);
  func_ov004_020afdd0((int)data_ov006_0213c2e4,(x>>12)-0x20,(y>>12)-8,-1,0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020e1854, 0x020e1854, size 0x300 */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_common_subs off
// @symbol func_ov006_020e1854
/* func_ov006_020e1854 -- the pen handler for the ov006 slot-machine style scene,
 * 0x020e1854, 0x300 bytes.
 *
 * While the stylus is down it drags the object: the touch point plus the stored
 * grab offset becomes the new position, clamped into the 0x20000..0xe0000 x
 * 0x94000..0xb8000 box.  A drag shorter than two units is undone outright.  What
 * survives updates the swing bookkeeping (0x4eea's three-state direction
 * machine), turns the drag vector into an angle at 0x4ede, clamps that angle
 * into the lower half turn, averages it with the previous one, and folds the
 * drag length into the 0x4ec8 speed with a decay when it falls.  Finally the
 * grab offset is re-derived from the new position so the next tick drags from
 * the same spot on the object.  With the stylus up, two flags are reset.
 *
 * THREE THINGS ARE LOAD BEARING AND ALL THREE WERE MEASURED.
 *
 * 1. THE TOUCH TABLE IS REACHED THROUGH A BYTE VIEW OF A RECORD ARRAY.
 *    data_020a0dea and data_020a0deb are four-byte records (the B4 shape that
 *    src/minigames/d_s_mg_trampoline.cpp already declares for the same
 *    globals), and this file takes a `u8 *` of each before indexing.  A plain
 *    `u8 data_020a0dea[]` extern with no cast, `&arr[0]`, `arr + 0`, or a u8*
 *    alias of a u8 array all cost eighteen more words: the cast has to cross a
 *    type boundary for 2004/b56 to keep the ROM's addressing.
 *
 * 2. dx IS COMPUTED BEFORE dy, AND NEITHER SUBTRACTION GETS A NAMED TEMPORARY.
 *    Writing dy first, or routing either difference through an `int t`, leaves
 *    the ROM's shape four words off: the cartridge loads the new x into a fresh
 *    register and puts both subtractions in the register the base pointer just
 *    vacated, while the other orders reuse the dying base for the load and
 *    subtract in place.
 *
 * 3. THE TAIL BLOCK'S SLOT INDEX IS WIDER THAN A BYTE.  `unsigned short j`
 *    (int, short and unsigned int are byte-identical to it); `u8 j` costs six
 *    words in the two table reads under it.
 *
 * `#pragma opt_common_subs off` is real: the two 0x4eb0/0x4eb4 re-reads after
 * the clamps have to stay re-reads.
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1854(void *arg)
{
    u8 *c = (u8 *)arg;
    u8 idx;
    int dx, oldx, oldy;
    int diff;
    u16 ang;

    idx = data_020a0e40[0];
    if (data_020a0de8[idx * 4] != 0) {
        int dy2, dy;

        int i4 = idx * 4;
        u8 *pa = (u8 *)data_020a0dea;
        u8 *pb = (u8 *)data_020a0deb;
        u8 bx = pa[i4];
        u8 by = pb[i4];

        oldx = *(int *)(c + 0x4eb0);
        oldy = *(int *)(c + 0x4eb4);
        *(int *)(c + 0x4eb0) = (bx << 12) + *(int *)(c + 0x4ec0);
        *(int *)(c + 0x4eb4) = *(int *)(c + 0x4ec4) + (by << 12);

        if (*(int *)(c + 0x4eb4) <= 0x94000)
            *(int *)(c + 0x4eb4) = 0x94000;
        if (*(int *)(c + 0x4eb0) <= 0x20000)
            *(int *)(c + 0x4eb0) = 0x20000;
        if (*(int *)(c + 0x4eb0) >= 0xe0000)
            *(int *)(c + 0x4eb0) = 0xe0000;
        if (*(int *)(c + 0x4eb4) >= 0xb8000)
            *(int *)(c + 0x4eb4) = 0xb8000;


        dx = (*(int *)(c + 0x4eb0) - oldx) >> 12;
        dy = (*(int *)(c + 0x4eb4) - oldy) >> 12;
        dy2 = dy * dy;

        if (_ZN4cstd4sqrtEy((s64)(dx * dx + dy2)) <= 1) {
            *(int *)(c + 0x4eb0) = oldx;
            *(int *)(c + 0x4eb4) = oldy;
            return;
        }

        diff = (*(int *)(c + 0x4eb4) - *(int *)(c + 0x4ebc)) >> 12;
        if (*(u8 *)(c + 0x4eea) == 0) {
            func_02012718(0x1d6, *(int *)(c + 0x4eb0));
            *(u8 *)(c + 0x4eea) = 2;
            *(int *)(c + 0x4ed4) = (*(int *)(c + 0x4eb4) - *(int *)(c + 0x4ebc)) >> 12;
            *(int *)(c + 0x4ebc) = *(int *)(c + 0x4eb4);
        } else if (*(u8 *)(c + 0x4eea) == 1) {
            if (*(int *)(c + 0x4ed4) * diff > 0) {
                if (diff < 0)
                    diff = -diff;
                if (diff >= 0xa)
                    *(u8 *)(c + 0x4eea) = 0;
            } else {
                *(int *)(c + 0x4ed4) = diff;
                *(int *)(c + 0x4ebc) = *(int *)(c + 0x4eb4);
            }
        } else {
            if (*(int *)(c + 0x4ed4) * diff < 0)
                *(u8 *)(c + 0x4eea) = 1;
            *(int *)(c + 0x4ed4) = (*(int *)(c + 0x4eb4) - *(int *)(c + 0x4ebc)) >> 12;
            *(int *)(c + 0x4ebc) = *(int *)(c + 0x4eb4);
        }

        ang = *(u16 *)(c + 0x4ede);
        *(u16 *)(c + 0x4ede) = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
        {
            u16 a = *(u16 *)(c + 0x4ede);
            if (a <= 0x8000) {
                if (a >= 0x4000) {
                    *(u16 *)(c + 0x4ede) = 0x8000;
                    goto ang_done;
                }
            }
            if (a <= 0x4000)
                *(u16 *)(c + 0x4ede) = 0;
        }
    ang_done:;

        {
            int mag;
            *(u16 *)(c + 0x4ede) = (u16)((*(u16 *)(c + 0x4ede) + ang) >> 1);
            mag = _ZN4cstd4sqrtEy((s64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
            mag = (mag << 12) >> 4;
            if (mag >= 0xc000)
                mag = 0xc000;
            if (mag > *(int *)(c + 0x4ec8))
                *(int *)(c + 0x4ec8) = mag;
            {
                int cur = *(int *)(c + 0x4ec8);
                if (cur > mag) {
                    *(int *)(c + 0x4ec8) = *(int *)(c + 0x4ec8) - ((cur - mag) >> 1);
                }
            }
        }


        {
            int px = *(int *)(c + 0x4eb0);
            int py = *(int *)(c + 0x4eb4);
            int j = data_020a0e40[0];
            u8 jx = ((u8 *)data_020a0dea)[j * 4];
            int ax = (px >> 12) - jx;
            u8 jy = ((u8 *)data_020a0deb)[j * 4];
            int ay = (py >> 12) - jy;
            *(int *)(c + 0x4ec0) = ax << 12;
            *(int *)(c + 0x4ec4) = ay << 12;
        }

        return;
    }

    *(u8 *)(c + 0x4ee4) = 0;
    *(u8 *)(c + 0x4ee5) = 1;
}
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020e1b54, 0x020e1b54, size 0x114 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e1b54
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1b54(char *c)
{
  int idx;
  int flag = 0;
  int x;
  int y;
  idx = data_020a0e40[0];
  if (data_020a0de8[idx * (4 & 0xFFFFFFFF)] != 0)
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0)
  {
    return;
  }
  x = ((*((int *) (c + 0x4eb0))) >> 0xc) - ((u8 *)data_020a0dea)[idx * 4];
  y = ((*((int *) (c + 0x4eb4))) >> 0xc) - ((u8 *)data_020a0deb)[idx * 4];
  *((int *) (c + 0x4ec0)) = x << 0xc;
  *((int *) (c + 0x4ec4)) = y << 0xc;
  *((u8 *) (c + 0x4ee4)) = 1;
  *((u16 *) (c + 0x4ede)) = 0xc000;
  if ((*((u8 *) (c + 0x4ee9))) == 0)
  {
    func_02012718(0x1d2, *((int *) (c + 0x4eb0)));
    *((u8 *) (c + 0x4ee9)) = 6;
  }
  *((int *) (c + 0x4ecc)) = 0;
  *((int *) (c + 0x4ed0)) = 0;
  *((int *) (c + 0x4eb8)) = (*((int *) (c + 0x4eb0))) + (*((int *) (c + 0x4ec0)));
  *((int *) (c + 0x4ebc)) = (*((int *) (c + 0x4eb4))) + (*((int *) (c + 0x4ec4)));
  *((int *) (c + 0x4ed4)) = 0xff;
  *((u8 *) (c + 0x4eea)) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020e1c68, 0x020e1c68, size 0x160 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e1c68
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020e1c68(char* a0) {
    int x, y;
    int i;
    char* p;
    int n;
    int j;
    p = a0;
    for (i = 0; i < 5; i++) {
        if (*(unsigned char*)(p + 0x4689) == 0) goto next;
        if (*(unsigned char*)(p + 0x468a) == 0) goto next;
        x = *(int*)(p + 0x4660) >> 12;
        y = *(int*)(p + 0x4664) >> 12;
        RenderOamBothScreens(&data_ov006_0213c264, x, y, -1, 1, 0);
        RenderOamBothScreens(&data_ov006_0213c2ac, x, y + 8, -1, 2, 0);
    next:
        p += 0x2c;
    }
    n = 5 - *(unsigned char*)(a0 + 0x4ee6);
    if (n < 0) n = 0;
    j = 0;
    if (n > 0) {
        for (; j < n; j++) {
            int v = data_ov006_0212e468[j];
            RenderOamBothScreens(&data_ov006_0213c264, v, 0xb0, -1, 1, 0);
            RenderOamBothScreens(&data_ov006_0213c2ac, v, 0xb8, -1, 2, 0);
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020e1dc8, 0x020e1dc8, size 0x2f4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e1dc8
/* recovered: dScMgCurling_c stone separation, ov006 0x020e1dc8 (756 bytes).
 * Stone idx has just moved; any other active stone within 24 units of it is
 * pushed to 26 units away along the line between them (FX_Mul of the angle's
 * cos/sin by 0x1a, then back to integer units), and that pushed stone is in
 * turn checked once against the rest: the first stone it overlaps is pushed
 * the same way, the bump sound plays, and the routine returns. Only one push
 * per call in each direction; the outer scan stops after its first hit.
 *
 * What the match hinged on: the two FX_Mul results must be named (vx, vy) --
 * inlined into the stores they cost a reloaded 0x4660 pool constant and four
 * bytes -- and the inner scan must have its OWN dx/dy/dist/ang, declared in
 * the loop body. Sharing the outer loop's four variables recolours the outer
 * head (dy/dx/i-pointer rotate through r7/r8/sb). No pragma. */
extern "C" void func_ov006_020e1dc8(dScMgCurling_c *self, int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 5; i++) {
        if (self->mStone[i].active == 0) continue;
        if (idx == i) continue;
        dx = (self->mStone[i].x - self->mStone[idx].x) >> 12;
        dy = (self->mStone[i].y - self->mStone[idx].y) >> 12;
        dist = _ZN4cstd4sqrtEy((u64)(dx * dx + dy * dy));
        ang = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        if (dist > 0x18) continue;
        {
            int cs;
            int sn;
            int vx;
            int vy;

            k = (ang >> 4) * 2;
            cs = data_02082214[k + 1];
            vx = (int)(((long long)cs * 0x1a + 0x800) >> 12);
            self->mStone[i].x = self->mStone[idx].x + (vx << 12);
            sn = data_02082214[k];
            vy = (int)(((long long)sn * 0x1a + 0x800) >> 12);
            self->mStone[i].y = self->mStone[idx].y + (vy << 12);
            for (j = 0; j < 5; j++) {
                int dx2;
                int dy2;
                int dist2;
                u16 ang2;

                if (self->mStone[j].active == 0) continue;
                if (i == j) continue;
                dx2 = (self->mStone[j].x - self->mStone[i].x) >> 12;
                dy2 = (self->mStone[j].y - self->mStone[i].y) >> 12;
                dist2 = _ZN4cstd4sqrtEy((u64)(dx2 * dx2 + dy2 * dy2));
                ang2 = _ZN4cstd5atan2E5Fix12IiES1_(dy2, dx2);
                if (dist2 > 0x18) continue;
                {
                    int cs2;
                    int sn2;
                    int vx2;
                    int vy2;

                    k = (ang2 >> 4) * 2;
                    cs2 = data_02082214[k + 1];
                    vx2 = (int)(((long long)cs2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].x = self->mStone[i].x + (vx2 << 12);
                    sn2 = data_02082214[k];
                    vy2 = (int)(((long long)sn2 * 0x1a + 0x800) >> 12);
                    self->mStone[j].y = self->mStone[i].y + (vy2 << 12);
                    func_02012718(0xe8, self->mStone[idx].x);
                    return;
                }
            }
            return;
        }
    }
}

