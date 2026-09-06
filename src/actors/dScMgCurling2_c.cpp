//cpp
/* dScMgCurling2_c -- the two-player curling minigame scene, ov006.
 *
 * Reconstructed translation unit: ROM ordinals 0..30 of the linker run
 * 0x020e3854..0x020e6c28, i.e. .text 0x020e3854..0x020e5450, 31 functions.
 * Assembled from the 31 one-function legacy sources, then reconciled by hand.
 *
 * WHY THIS IS A SUB-RANGE: a sourceless hole splits the run.  ROM ordinal 31,
 * func_ov006_020e5450 (0x020e5450, size 0x560), has no source anywhere in the
 * tree -- it carries a symbols.txt row but NO src/ file and NO entry in
 * config/arm9/overlays/ov006/delinks.txt at all (the blocks jump from
 * 0x020e513c-0x020e5450 straight to 0x020e59b0), so the cartridge's own bytes
 * cover that range.  It is a banked near-miss whose best recorded attempt sits
 * at 29 divergences (config/match_attempts.jsonl).  It sits in the MIDDLE of
 * the run, and nothing in this tree can express a .text claim with a hole in
 * it, so the run has to be licensed as one of its two contiguous sides.
 *
 * THIS FILE IS THE LOWER SIDE, ordinals 0..30 -- 31 functions against 21 on
 * the far side.  It is also the side that holds the class's key function: the
 * destructor, which include/dScMgCurling2_c.h declares out of line and
 * declares FIRST.  The 21 members above the hole (func_ov006_020e59b0 through
 * dScMgCurling2_c_classInit, including InitResources, Behavior, Render and
 * OnYoshiTryEat) keep their own shards and their own delinks.txt entries.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, LOWEST ADDRESS FIRST, and that is one
 * decision with `#pragma defer_codegen off` below.  With codegen deferred
 * (the default) mwccarm 2004/b56 emits one .text section per function in the
 * REVERSE of source order; generating at parse time emits them in source
 * order.  Flipping either alone fails linkcheck [4b/8]'s ROM-ascending
 * emission-order audit.  Do not reorder.
 *
 * THE DESTRUCTOR IS OUT OF LINE, AND THE CARTRIDGE ORDERS IT D1 (0x020e3854)
 * BELOW D0 (0x020e3878).  Out-of-line + deferred codegen emits D2, D0, D1 --
 * the wrong order; out-of-line + `defer_codegen off` emits D1, D0, D2, which
 * is the cartridge's order with the homeless D2 trailing where it costs
 * nothing.  There is no D2 in the ROM at all: dScMgCurling2_c is a confirmed
 * leaf -- no RTTI edge names it as a base -- so its own D2 is never odr-used
 * and the compiler's copy is deadstripped.  include/dScMgCurling2_c.h
 * therefore needs NO edit; its existing out-of-line declaration is what this
 * TU wants and what the still-enrolled shards above the hole want too.
 *
 * Because the destructor is out of line, this TU owns the class's key
 * function and mwcc emits the whole inheritance chain's vtable and typeinfo
 * as vague-linkage passengers -- see the manifest's compiler_only_output
 * block: eleven RTTI/vtable data symbols (2 x 5 levels of
 * dScMgCurling2_c -> dScMgBase_c -> dScene_c -> dBase_c -> fBase_c, plus the
 * one vtable) and the homeless _ZN15dScMgCurling2_cD2Ev.
 *
 * TWO MEMBERS CARRY BRACKETED OPTIMISATION PRAGMAS, AND THEY ARE REAL.
 * func_ov006_020e4b00 wants `opt_strength_reduction off` and
 * func_ov006_020e4bd4 wants both `opt_common_subs off` and
 * `opt_propagation off`; each carries its own `#pragma push` / `#pragma pop`
 * bracket.  Measured, not assumed, and the delete-outright control was run
 * first: delete every bracketed pragma from this file and nothing else, and
 * it drops from 31/31 to 29/31 with the DIFFs falling on exactly those two
 * members.  Keep the brackets but delete only `defer_codegen off` and it is
 * 29/31 again AND the emission order inverts to "30 ordinal pair(s) NOT in
 * ROM order" -- the brackets bind only while codegen is generated at parse
 * time.  One pragma buys the bytes and the order.
 *
 * A THIRD SHARD'S PRAGMA WAS MEASURED INERT AND IS NOT CARRIED.
 * src/func_ov006_020e4794.c opens with `#pragma opt_propagation off`;
 * bracketing it here and deleting it outright both give 31/31, so it buys
 * nothing and would only mislead the next reader.
 *
 * decl_common.h is NOT included.  It declares nine of these 31 members and
 * eight of the nine contradict the byte-matched shard -- seven on the
 * parameter type (`void*` against the shard's `char*`) and two of those also
 * on the return type (func_ov006_020e3948 and func_ov006_020e4800 as `int`
 * against the shards' `void`).  Including it makes each an `illegal function
 * overloading` error against code that already matches, so this TU follows
 * the ov006/dScMgPanel_c precedent and declares every symbol it needs itself.
 * No header under include/ pulls decl_common.h in, so excluding it is enough.
 *
 * Promoted members in this TU (ROM address order):
 *   [0]  0x020e3854  _ZN15dScMgCurling2_cD1Ev
 *   [1]  0x020e3878  _ZN15dScMgCurling2_cD0Ev
 *   [2]  0x020e38b0  func_ov006_020e38b0
 *   [3]  0x020e3948  func_ov006_020e3948
 *   [4]  0x020e39e0  func_ov006_020e39e0
 *   [5]  0x020e3b9c  func_ov006_020e3b9c
 *   [6]  0x020e3bc4  func_ov006_020e3bc4
 *   [7]  0x020e3c4c  func_ov006_020e3c4c
 *   [8]  0x020e3ce0  func_ov006_020e3ce0
 *   [9]  0x020e3db4  func_ov006_020e3db4
 *   [10] 0x020e3e4c  func_ov006_020e3e4c
 *   [11] 0x020e3f54  func_ov006_020e3f54
 *   [12] 0x020e4094  func_ov006_020e4094
 *   [13] 0x020e41d0  func_ov006_020e41d0
 *   [14] 0x020e42b4  func_ov006_020e42b4
 *   [15] 0x020e4348  func_ov006_020e4348
 *   [16] 0x020e440c  func_ov006_020e440c
 *   [17] 0x020e4520  func_ov006_020e4520
 *   [18] 0x020e4630  func_ov006_020e4630
 *   [19] 0x020e4744  func_ov006_020e4744
 *   [20] 0x020e4794  func_ov006_020e4794
 *   [21] 0x020e4800  func_ov006_020e4800
 *   [22] 0x020e48d4  func_ov006_020e48d4
 *   [23] 0x020e4a84  func_ov006_020e4a84
 *   [24] 0x020e4b00  func_ov006_020e4b00
 *   [25] 0x020e4b78  func_ov006_020e4b78
 *   [26] 0x020e4bd4  func_ov006_020e4bd4
 *   [27] 0x020e4ed4  func_ov006_020e4ed4
 *   [28] 0x020e4fe8  func_ov006_020e4fe8
 *   [29] 0x020e507c  func_ov006_020e507c
 *   [30] 0x020e513c  func_ov006_020e513c
 */

#pragma defer_codegen off

#include "types.h"
#include "dScMgCurling2_c.h"

/* ---------------------------------------------------------------------------
 * Shadow types.  Three members dispatch through the ov006 pointer-to-member
 * tables that __sinit_ov006_02130758 fills in, and all three legacy files
 * spelled the receiver the same way: an INCOMPLETE `struct C`.  Incomplete is
 * load-bearing -- mwccarm 2004/b56 picks its pointer-to-member representation
 * from the completeness of the class, so completing it here would change the
 * codegen of every one of them.  Kept exactly as the shards had it.
 * ------------------------------------------------------------------------- */
struct C;
typedef void (C::*PMF)(int);

/* func_ov006_020e4a84's view of the five-element 0x10-byte array at 0x4870. */
struct E22 {
    unsigned char b[0x10];
};

/* ---------------------------------------------------------------------------
 * ROM symbols this TU calls or reads.  One file-scope `extern "C"` region:
 * every member below is a C-linkage function and the one C++-named member
 * (the destructor) neither calls nor reads any of these.  Mangled ROM
 * spellings belong INSIDE the region -- outside it the reference would be
 * mangled a second time and the link would fail on a name nothing defines.
 * ------------------------------------------------------------------------- */
extern "C" {

extern int  func_ov004_020adbc0(void);
extern void func_ov004_020adb1c(int self);
extern void func_ov004_020af948(int a, int b, int c, int d);
extern void func_ov004_020b1a5c(int a, int b);
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern int  func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern void func_02012718(int id, int v);
extern void DrawOamSprite(int a, int b, int c, int d);
extern void RenderOamBothScreens(void *a0, int a1, int a2, int a3, int a4, void *a5);
extern int  RandomIntInternal(int *seed);
extern int  GetGameLanguage(void);
extern int  _ZN4cstd4sqrtEy(u64 v);
extern int  _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);

extern int            data_0209d4b8;
extern u8             data_020a0e40;
extern u8             data_020a0de8[];
extern u8             data_020a0de9[];
extern u8             data_020a0dea[];
extern u8             data_020a0deb[];
extern s16            data_02082214[];
extern unsigned char  data_ov006_0212e4f4[];
extern unsigned char  data_ov006_0212e4f8[];
extern int            data_ov006_0212e4fc[];
extern int            data_ov006_0213a5e0[];
extern int            data_ov006_0213c3fc;
extern int            data_ov006_0213c44c;
extern int            data_ov006_0213c454;
extern int           *data_ov006_0213c4dc[];
extern int            data_ov006_0213c4f0[];

/* The four pointer-to-member tables this side of the hole dispatches through.
   __sinit_ov006_02130758 fills them from the 8-byte {code pointer, zero
   adjustment} records in ov006 .data. */
extern PMF data_ov006_02141988[];
extern PMF data_ov006_021419a0[];
extern PMF data_ov006_021419b8[];
extern PMF data_ov006_021419f8[];

}  /* extern "C" */


/* ===========================================================================
 * [0] 0x020e3854  _ZN15dScMgCurling2_cD1Ev
 * [1] 0x020e3878  _ZN15dScMgCurling2_cD0Ev
 *
 * One out-of-line definition, emitting D1, D0 and a homeless D2 in that order
 * under `defer_codegen off`.  dScMgCurling2_c is a leaf with no member that
 * needs destroying, so the whole body is the compiler-generated own-vtable
 * write plus the base's D2 call; D0 adds operator delete.
 * ========================================================================= */
// @symbol _ZN15dScMgCurling2_cD1Ev
// @symbol _ZN15dScMgCurling2_cD0Ev
dScMgCurling2_c::~dScMgCurling2_c()
{
}


/* [2] 0x020e38b0 -- draw the 0x3c-entry spark array at 0x4fe0, stride 0x18. */
// @symbol func_ov006_020e38b0
extern "C" void func_ov006_020e38b0(char *r0)
{
    int i;
    char *p = r0;
    for (i = 0; i < 0x3c; i++) {
        unsigned char mode = *(unsigned char*)(p + 0x4ff5);
        if (mode != 0) {
            int a = *(int*)(p + 0x4fe0) >> 0xc;
            int b = *(int*)(p + 0x4fe4) >> 0xc;
            int c = *(unsigned short*)(p + 0x4ff2);
            if (mode == 1) {
                func_ov004_020b1ea4(a, b, c, -1, -1, 0, 0);
            } else {
                func_ov004_020b1ea4(a, b, c, -1, -1, 0, 0x32);
            }
        }
        p += 0x18;
    }
}


/* [3] 0x020e3948 -- age the same spark array one frame. */
// @symbol func_ov006_020e3948
extern "C" void func_ov006_020e3948(char *p)
{
    int i;
    for (i = 0; i < 60; i++, p += 0x18)
    {
        if (*(unsigned char *)(p + 0x4ff4) != 0)
        {
            if (*(unsigned short *)(p + 0x4ff0) != 0)
            {
                *(unsigned short *)(int)(p + 0x4ff0) -= 1;
                *(int *)(int)(p + 0x4fe4) += *(int *)(p + 0x4fec);
                *(int *)(int)(p + 0x4fec) -= 0x40;
            }
            else
            {
                *(unsigned char *)(p + 0x4ff4) = 0;
                *(unsigned char *)(p + 0x4ff5) = 0;
            }
        }
    }
}


/* [4] 0x020e39e0 -- spawn one score spark between stone `a` and stone `b`. */
// @symbol func_ov006_020e39e0
extern "C" void func_ov006_020e39e0(char *c, int a, int b)
{
    int i;
    char *p;
    int sx;
    int sy;
    unsigned char *q;

    p = c;
    for (i = 0; i < 0x3c; i++, p += 0x18) {
        if (*(unsigned char *)(p + 0x4ff4) != 0) continue;

        sx = *(int *)(c + a * 0x30 + 0x4660) + *(int *)(c + 0x4000 + b * 0x30 + 0x660);
        sy = *(int *)(c + a * 0x30 + 0x4664) + *(int *)(c + 0x4000 + b * 0x30 + 0x664);

        *(unsigned char *)(c + i * 0x18 + 0x4ff4) = 1;
        *(unsigned char *)(c + 0x4ff5 + i * 0x18) = 1;
        *(int *)(c + i * 0x18 + 0x4fe0) = sx >> 1;
        *(int *)(c + i * 0x18 + 0x4fe4) = sy >> 1;
        *(unsigned short *)(c + i * 0x18 + 0x4ff0) = 0x40;
        *(int *)(c + i * 0x18 + 0x4fe8) = 0;
        *(int *)(c + i * 0x18 + 0x4fec) = 0;

        if (*(unsigned char *)(c + a * 0x30 + 0x468d) != 0 && *(unsigned char *)(c + 0x4000 + b * 0x30 + 0x68d) != 0) {
            *(unsigned short *)(c + 0x4ff2 + i * 0x18) = (*(unsigned char *)(c + 0x55bf) + 1) * 10;
        } else if (*(unsigned char *)(c + a * 0x30 + 0x468d) != 0 || *(unsigned char *)(c + 0x4000 + b * 0x30 + 0x68d) != 0) {
            *(unsigned short *)(c + 0x4ff2 + i * 0x18) = (*(unsigned char *)(c + 0x55bf) + 1) * 10;
        } else {
            *(unsigned short *)(c + 0x4ff2 + i * 0x18) = (*(unsigned char *)(c + 0x55bf) + 1) * 100;
            *(unsigned char *)(c + 0x4ff5 + i * 0x18) = 2;
        }

        q = (unsigned char *)(c + 0x55bf);
        *q = *q + 1;
        if (*(unsigned char *)(c + 0x55bf) >= 0x17) *(unsigned char *)(c + 0x55bf) = 0x17;
        func_ov004_020adb1c(*(unsigned short *)(c + 0x4ff2 + i * 0x18) + func_ov004_020adbc0());
        return;
    }
}


/* [5] 0x020e3b9c -- clear the spark array. */
// @symbol func_ov006_020e3b9c
extern "C" void func_ov006_020e3b9c(char *p)
{
    int i;
    for (i = 0; i < 0x3c; i++) {
        *(unsigned char *)(p + 0x4ff4) = 0;
        *(unsigned char *)(p + 0x4ff5) = 0;
        p += 0x18;
    }
}


/* [6] 0x020e3bc4 -- draw the 0x32-entry snowflake array at 0x48c0, stride 0x24. */
// @symbol func_ov006_020e3bc4
extern "C" void func_ov006_020e3bc4(char *c)
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(c + 0x4000 + 0x8e0)) {
            int x = *(int *)(c + 0x4000 + 0x8c0) >> 0xc;
            int y = *(int *)(c + 0x4000 + 0x8c4) >> 0xc;
            func_ov004_020af948(data_ov006_0213a5e0[*(unsigned char *)(c + 0x4000 + 0x8e1)], x, y, 0);
            DrawOamSprite(data_ov006_0213a5e0[*(unsigned char *)(c + 0x4000 + 0x8e2)], x, y, 0);
        }
        c += 0x24;
    }
}


/* [7] 0x020e3c4c -- snowflake state callback. */
// @symbol func_ov006_020e3c4c
extern "C" void func_ov006_020e3c4c(char *c, int i)
{
    int idx = i * 0x24;
    if (*(unsigned short *)(c + 0x48d8 + idx) != 0) {
        short *p = (short *)(c + 0x48d8 + idx);
        *p = (short)(*(unsigned short *)p - 1);
        if (*p < 0)
            *p = 0;
    } else if (*(int *)(c + 0x48cc + idx) > 0x100) {
        int *q = (int *)(c + 0x48cc + idx);
        *q = *q - 0x10;
        if ((short)*q < 0x100)
            *q = 0x100;
    } else {
        *(unsigned char *)(c + 0x4000 + idx + 0x8df) = 0;
    }
}


/* [8] 0x020e3ce0 -- snowflake state callback. */
// @symbol func_ov006_020e3ce0
extern "C" void func_ov006_020e3ce0(char *o, int i)
{
    int n = i * 0x24;
    if (*(int *)(o + 0x48d0 + n) > *(int *)(o + 0x48cc + n)) {
        *(int *)(o + 0x48cc + n) += 0x10;
        if (*(int *)(o + 0x48d0 + n) > *(int *)(o + 0x48cc + n))
            *(int *)(o + 0x48cc + n) = *(int *)(o + 0x48d0 + n);
    }
    if (*(unsigned short *)(o + 0x48d8 + n) != 0) {
        *(unsigned short *)(o + 0x48d8 + n) = *(unsigned short *)(o + 0x48d8 + n) - 1;
        if (*(short *)(o + 0x48d8 + n) < 0) *(short *)(o + 0x48d8 + n) = 0;
    } else {
        *(unsigned char *)(o + n + 0x48df) = 2;
        *(short *)(o + 0x48d8 + n) = (short)(unsigned char)((((0x20 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf)) + 0x20);
    }
}


/* [9] 0x020e3db4 -- snowflake state callback. */
// @symbol func_ov006_020e3db4
extern "C" void func_ov006_020e3db4(char* c, int i) {
  int idx = i * 0x24;
  unsigned int r;
  *(int*)(c + 0x48cc + idx) = 0;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  *(int*)(c + 0x48d0 + idx) = (((r << 4) >> 15) << 4) + 0x300;
  *(unsigned char*)(c + 0x48df + idx) = 1;
  r = ((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
  r = ((r << 5) >> 15) + 0x20;
  *(short*)(c + 0x48d8 + idx) = (unsigned char)r;
}


/* [10] 0x020e3e4c -- snowflake drift callback. */
// @symbol func_ov006_020e3e4c
extern "C" void func_ov006_020e3e4c(char *base, int i)
{
    int n = i * 0x24;
    char *a48c8 = base + 0x48c8;
    char *a48c0 = base + 0x48c0;
    char *a48c4 = base + 0x48c4;
    *(int*)(a48c0 + n) = *(int*)(a48c0 + n) + *(int*)(a48c8 + n);
    *(int*)(a48c4 + n) = *(int*)(a48c4 + n) + *(int*)(base + n + 0x48cc);
    if (*(u16*)(base + n + 0x48d4) != 0) {
        char *a48d6 = base + 0x48d6;
        *(u16*)(a48d6 + n) = *(u16*)(a48d6 + n) - 1;
        if (*(s16*)(a48d6 + n) < 0) *(s16*)(a48d6 + n) = 0;
        return;
    }
    if (*(int*)(a48c8 + n) > 0) {
        *(int*)(a48c8 + n) = *(int*)(a48c8 + n) - 8;
        if ((s16)*(int*)(a48c8 + n) < 0) *(int*)(a48c8 + n) = 0;
        return;
    }
    if (*(int*)(a48c8 + n) < 0) {
        *(int*)(a48c8 + n) = *(int*)(a48c8 + n) + 8;
        if (*(int*)(a48c8 + n) > 0) *(int*)(a48c8 + n) = 0;
        return;
    }
    *(u8*)(base + n + 0x48de) = 0;
}


/* [11] 0x020e3f54 -- snowflake drift callback.
 * The F* macros are this shard's own spelling of the 0x24-stride record; they
 * are what byte-matched, so they are carried verbatim and undefined again
 * straight after the body. */
#define F0(b,i)  (*(int*)  ((char*)(b) + 0x48c0 + (i)*0x24))
#define F4(b,i)  (*(int*)  ((char*)(b) + 0x48c4 + (i)*0x24))
#define F8(b,i)  (*(int*)  ((char*)(b) + 0x48c8 + (i)*0x24))
#define FC(b,i)  (*(int*)  ((char*)(b) + 0x48cc + (i)*0x24))
#define F14(b,i) (*(unsigned short*)((char*)(b) + 0x48d4 + (i)*0x24))
#define F14S(b,i) (*(short*)((char*)(b) + 0x48d4 + (i)*0x24))
#define F16(b,i) (*(unsigned short*)((char*)(b) + 0x48d6 + (i)*0x24))
#define F16S(b,i) (*(short*)((char*)(b) + 0x48d6 + (i)*0x24))
#define F1E(b,i) (*(unsigned char*)((char*)(b) + 0x48de + (i)*0x24))
// @symbol func_ov006_020e3f54
extern "C" void func_ov006_020e3f54(char *base, int idx)
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
#undef F0
#undef F4
#undef F8
#undef FC
#undef F14
#undef F14S
#undef F16
#undef F16S
#undef F1E


/* [12] 0x020e4094 -- snowflake drift callback. */
// @symbol func_ov006_020e4094
extern "C" void func_ov006_020e4094(char *base, int index)
{
    int i = index * 0x24;
    unsigned short v;

    *(int *)(base + 0x48c0 + i) += *(int *)(base + 0x48c8 + i);
    *(int *)(base + 0x48c4 + i) += *(int *)(base + 0x48cc + i);

    v = *(unsigned short *)(base + 0x48d4 + i);
    if (v != 0) {
        *(short *)(base + 0x48d4 + i) = v - 1;
        if (*(short *)(base + 0x48d4 + i) < 0)
            *(short *)(base + 0x48d4 + i) = 0;
        return;
    }

    if (*(int *)(base + 0x48c8 + i) < 0x300) {
        *(int *)(base + 0x48c8 + i) += 8;
        if (*(int *)(base + 0x48c8 + i) >= 0x300)
            *(int *)(base + 0x48c8 + i) = 0x300;
    }

    v = *(unsigned short *)(base + 0x48d6 + i);
    if (v != 0) {
        *(short *)(base + 0x48d6 + i) = v - 1;
        if (*(short *)(base + 0x48d6 + i) < 0)
            *(short *)(base + 0x48d6 + i) = 0;
        return;
    }

    *(char *)(base + 0x48de + i) = 3;
    *(short *)(base + 0x48d6 + i) = (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) + 0x20 & 0xff;
}


/* [13] 0x020e41d0 -- snowflake drift callback. */
// @symbol func_ov006_020e41d0
extern "C" void func_ov006_020e41d0(char *o, int i)
{
    int n = i * 0x24;
    if (*(unsigned short *)(o + 0x48d4 + n) != 0) {
        *(unsigned short *)(o + 0x48d4 + n) = *(unsigned short *)(o + 0x48d4 + n) - 1;
        if (*(short *)(o + 0x48d4 + n) < 0) *(short *)(o + 0x48d4 + n) = 0;
        return;
    }
    *(int *)(o + 0x48c8 + n) = 0;
    *(unsigned char *)(o + 0x48de + n) = data_ov006_0212e4f4[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(o + 0x48d4 + n) = (short)(unsigned char)((0x10 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf);
    *(unsigned short *)(o + 0x48d6 + n) = (short)(unsigned char)(((0x40 * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 0xf) + 0x60);
}


/* [14] 0x020e42b4 -- run one snowflake's two pointer-to-member callbacks. */
// @symbol func_ov006_020e42b4
extern "C" void func_ov006_020e42b4(char *c, int i)
{
    C *self = (C *)c;
    int idx = i * 0x24;
    unsigned char k0 = *(unsigned char *)(c + idx + 0x4000 + 0x8de);
    (self->*data_ov006_021419f8[k0])(i);
    unsigned char k1 = *(unsigned char *)((char *)self + idx + 0x4000 + 0x8df);
    (self->*data_ov006_021419a0[k1])(i);
}


/* [15] 0x020e4348 -- snowflake drift callback. */
// @symbol func_ov006_020e4348
extern "C" void func_ov006_020e4348(char *base, int idx) {
    int off = idx * 0x24;
    int *a = (int *)(base + 0x48c0 + off);
    int *c = (int *)(base + 0x48c8 + off);
    int *b = (int *)(base + 0x48c4 + off);
    *a += *c;
    *b += *(int *)(base + off + 0x48cc);
    if (*c > 0) {
        *c -= 0x20;
        if ((int)(short)*c < 0) *c = 0;
    } else if (*c < 0) {
        *c += 0x20;
        if (*c > 0) *c = 0;
    } else {
        *(unsigned char *)(base + off + 0x48de) = 0;
    }
}


/* [16] 0x020e440c -- snowflake drift callback. */
// @symbol func_ov006_020e440c
extern "C" void func_ov006_020e440c(char *c, int idx)
{
    int n = idx * 0x24;

    *(int *)(c + 0x48c0 + n) =
        *(int *)(c + 0x48c0 + n) + *(int *)(c + 0x48c8 + n);
    *(int *)(c + 0x48c4 + n) =
        *(int *)(c + 0x48c4 + n) + *(int *)(c + 0x48cc + n);

    if (*(u16 *)(c + 0x48d4 + n) != 0) {
        *(s16 *)(c + 0x48d4 + n) =
            (s16)(*(u16 *)(c + 0x48d4 + n) - 1);
        if (*(s16 *)(c + 0x48d4 + n) < 0)
            *(s16 *)(c + 0x48d4 + n) = 0;
        return;
    }

    if (*(int *)(c + 0x48c8 + n) > -0x400) {
        *(int *)(c + 0x48c8 + n) -= 0x20;
        if (*(int *)(c + 0x48c8 + n) <= -0x400)
            *(int *)(c + 0x48c8 + n) = 0x400;
    }

    if (*(u16 *)(c + 0x48d6 + n) != 0) {
        *(s16 *)(c + 0x48d6 + n) =
            (s16)(*(u16 *)(c + 0x48d6 + n) - 1);
        if (*(s16 *)(c + 0x48d6 + n) < 0)
            *(s16 *)(c + 0x48d6 + n) = 0;
        return;
    }

    *(unsigned char *)(c + 0x48de + n) = 3;
}


/* [17] 0x020e4520 -- snowflake drift callback. */
// @symbol func_ov006_020e4520
extern "C" void func_ov006_020e4520(char *c, int idx)
{
    int off = idx * 0x24;

    *(int *)(c + 0x48c0 + off) += *(int *)(c + 0x48c8 + off);
    *(int *)(c + 0x48c4 + off) += *(int *)(c + 0x48cc + off);

    {
        unsigned short cv = *(unsigned short *)(c + 0x48d4 + off);
        if (cv != 0) {
            *(short *)(c + 0x48d4 + off) = (short)(cv - 1);
            if (*(short *)(c + 0x48d4 + off) < 0) {
                *(short *)(c + 0x48d4 + off) = 0;
            }
            return;
        }
    }

    if (*(int *)(c + 0x48c8 + off) < 0x400) {
        *(int *)(c + 0x48c8 + off) += 0x20;
        if (*(int *)(c + 0x48c8 + off) >= 0x400) {
            *(int *)(c + 0x48c8 + off) = 0x400;
        }
    }

    {
        unsigned short ev = *(unsigned short *)(c + 0x48d6 + off);
        if (ev != 0) {
            *(short *)(c + 0x48d6 + off) = (short)(ev - 1);
            if (*(short *)(c + 0x48d6 + off) < 0) {
                *(short *)(c + 0x48d6 + off) = 0;
            }
            return;
        }
    }

    *(unsigned char *)(c + 0x48de + off) = 3;
}


/* [18] 0x020e4630 -- snowflake drift callback. */
// @symbol func_ov006_020e4630
extern "C" void func_ov006_020e4630(char *o, int i)
{
    int n = i * 0x24;
    if (*(unsigned short *)(o + 0x48d4 + n) != 0) {
        *(unsigned short *)(o + 0x48d4 + n) = *(unsigned short *)(o + 0x48d4 + n) - 1;
        if (*(short *)(o + 0x48d4 + n) < 0) *(short *)(o + 0x48d4 + n) = 0;
        return;
    }
    *(int *)(o + 0x48c8 + n) = 0;
    *(int *)(o + 0x48cc + n) = ((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5) >> 0xf) << 4) + 0x600;
    *(unsigned char *)(o + 0x48de + n) = data_ov006_0212e4f8[(((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 1 >> 15];
    *(unsigned short *)(o + 0x48d4 + n) = (unsigned short)((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 4) >> 0xf) & 0xff);
    *(unsigned short *)(o + 0x48d6 + n) = (unsigned short)(((((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 0x30) >> 0xf) + 0x30) & 0xff);
}


/* [19] 0x020e4744 -- run one snowflake's data_ov006_021419b8 callback. */
// @symbol func_ov006_020e4744
extern "C" void func_ov006_020e4744(char *o, int i)
{
    unsigned char idx = *(unsigned char *)(o + i * 0x24 + 0x48de);
    (((C *)o)->*data_ov006_021419b8[idx])(i);
}


/* [20] 0x020e4794 -- snowflake respawn callback.
 * The shard declared data_0209d4b8 as `int[]` and passed it bare where every
 * other member declares the scalar and passes its address; both spell the
 * same word address, and one file-scope declaration cannot be both, so the
 * scalar wins and the call takes `&`.
 *
 * The shard's own `#pragma opt_propagation off` is NOT carried: bracketed
 * here it gave 31/31 and deleted outright it also gave 31/31, so it buys
 * nothing on this compiler at these flags. */
// @symbol func_ov006_020e4794
extern "C" void func_ov006_020e4794(char *c, int idx) {
    unsigned r = (unsigned)RandomIntInternal(&data_0209d4b8);
    int k = 0;
    unsigned m = ((r >> 16) & 0x7fff) << 3 >> 0xf;
    if (m == 5) k = 1;
    char *p = c + idx * 0x24;
    *(unsigned char *)(p + 0x48dd) = data_ov006_0212e4fc[k];
    *(unsigned char *)(p + 0x48de) = 0;
}


/* [21] 0x020e4800 -- step every snowflake through data_ov006_02141988 and
 * recycle the ones that have fallen off the bottom. */
// @symbol func_ov006_020e4800
extern "C" void func_ov006_020e4800(char *o)
{
    int i;
    char *e = o;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(e + 0x48dc) != 0) {
            unsigned char idx = *(unsigned char *)(e + 0x48dd);
            (((C *)o)->*data_ov006_02141988[idx])(i);
            if ((*(int *)(e + 0x48c4) >> 0xc) >= 0xc8) {
                *(int *)(e + 0x48c0) = (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(e + 0x48c4) = -0x8000;
                *(unsigned char *)(e + 0x48de) = 0;
                *(unsigned char *)(e + 0x48dd) = 0;
                *(unsigned char *)(e + 0x48df) = 0;
            }
        }
        e += 0x24;
    }
}


/* [22] 0x020e48d4 -- seed all 0x32 snowflakes. */
// @symbol func_ov006_020e48d4
extern "C" void func_ov006_020e48d4(char *c)
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
        *(int *)(p + 0x4000 + 0x8c0) = 0;
        *(int *)(p + 0x4000 + 0x8c4) = 0;
        *(int *)(p + 0x4000 + 0x8c8) = 0;
        *(int *)(p + 0x4000 + 0x8cc) = 0;
        *(short *)(p + 0x4800 + 0xd4) = 0;
        *(short *)(p + 0x4800 + 0xd6) = 0;
        *(short *)(p + 0x4800 + 0xd8) = 0;
        *(char *)(p + 0x4000 + 0x8dc) = 0;
        *(char *)(p + 0x4000 + 0x8dd) = 0;
        *(char *)(p + 0x4000 + 0x8de) = 0;
        *(char *)(p + 0x4000 + 0x8df) = 0;
        *(char *)(p + 0x4000 + 0x8e0) = 0;
        *(char *)(p + 0x4000 + 0x8e1) = 0;
        *(char *)(p + 0x4000 + 0x8e2) = 1;
        p += 0x24;
    }

    i = 0;
    p = c;
    for (; i < 0x32; i++)
    {
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x8c0) = (int)((m >> 0xf)) << 0xf;
        *(int *)(p + 0x4000 + 0x8c4) = -0x8000;
        *(char *)(p + 0x4000 + 0x8dc) = 1;
        *(char *)(p + 0x4000 + 0x8e0) = 1;
        *(char *)(p + 0x4000 + 0x8dd) = 0;
        *(char *)(p + 0x4000 + 0x8de) = 0;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(char *)(p + 0x4000 + 0x8e1) = (char)(((r >> 16) & 0x7fff) * 5 >> 0xf);

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        v = *(unsigned char *)(p + 0x4000 + 0x8e1) + ((((r >> 16) & 0x7fff) << 2) >> 0xf) + 1;
        v = v & 0xff;
        if (v >= 5)
            v = (v - 5) & 0xff;
        *(char *)(p + 0x4000 + 0x8e2) = (char)v;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(short *)(p + 0x4800 + 0xd4) = (short)(((i & 7) << 6) + (((r >> 16) & 0x7fff) * 0x30 >> 0xf));

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        m = ((r >> 16) & 0x7fff) << 5;
        *(int *)(p + 0x4000 + 0x8c0) = (int)((m >> 0xf)) << 0xf;

        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        q = (((r >> 16) & 0x7fff) * 0x1a) >> 0xf;
        *(int *)(p + 0x4000 + 0x8c4) = (((q << 3) - 8)) << 0xc;
        *(short *)(p + 0x4800 + 0xd4) = 0;
        p += 0x24;
    }
}


/* [23] 0x020e4a84 -- draw the five score markers at 0x4870, stride 0x10. */
// @symbol func_ov006_020e4a84
extern "C" void func_ov006_020e4a84(unsigned char *c)
{
  struct E22 *a = (struct E22 *) c;
  int new_var;
  int i;
  func_ov004_020b1a5c(func_ov004_020adbc0(), 6);
  for (i = 0; i < 5; i++)
  {
    unsigned char *p = (unsigned char *) (&a[(((((i & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF) & 0xFFFFFFFF]);
    if (*((unsigned char *) (p + 0x487d)))
    {
      new_var = (*((int *) (p + 0x4870))) >> 12;
      func_ov004_020b2444(new_var, (*((int *) (p + 0x4874))) >> 12, *((unsigned short *) (p + 0x4878)), -1, -1, 0, 0);
    }
  }

}


/* [24] 0x020e4b00 -- age the five score markers, playing the pop sound as each
 * lands.  The legacy shard spelled the receiver `this`, which a C++ TU cannot
 * accept as a parameter name; renamed to `self` and otherwise untouched. */
#pragma push
#pragma opt_strength_reduction off
// @symbol func_ov006_020e4b00
extern "C" void func_ov006_020e4b00(char *self) {
    int i;
    for (i = 0; i < 5; i++) {
        char *base = self + (i << 4);
        volatile unsigned short *t;
        if (*(unsigned char*)(base + 0x487c) == 0) continue;
        if (*(unsigned short*)(base + 0x487a) == 0) continue;
        t = (volatile unsigned short*)(((int)base + 0x487a));
        *t = *t - 1;
        if (*t != 0) continue;
        *(unsigned char*)(base + 0x487d) = 1;
        _ZN5Sound12PlayBank2_2DEj(0x1bc);
    }
}
#pragma pop


/* [25] 0x020e4b78 -- draw the drag cursor. */
// @symbol func_ov006_020e4b78
extern "C" void func_ov006_020e4b78(char*c){
  if(*(unsigned char*)(c+0x55b9)==0) return;
  int x=*(int*)(c+0x5584);
  int y=*(int*)(c+0x5588);
  func_ov004_020afdd0((int)data_ov006_0213c4f0,(x>>12)-0x20,(y>>12)-8,-1,0);
}


/* [26] 0x020e4bd4 -- stylus drag update, 768 bytes.
 * Runs while the player drags a stone before the throw. With no touch in
 * the current input record it just drops the drag flags. Otherwise it moves
 * the drag point to the stylus plus the grab offset, clamps it to the rink,
 * and bails out (restoring the old point) when the stylus moved a unit or
 * less. The vertical direction-change tracker (unk_55be, unk_55a8,
 * unk_5590) plays the scrape sound on the first move and flips phases on
 * reversals; the throw angle (unk_55b2) is atan2 of the move with the
 * horizontal halved, snapped to straight up inside [0x4000,0x8000] and to
 * zero below, then averaged with the previous frame; the throw power
 * (unk_559c) follows the move length times 9 with a 0xc000 cap, rising
 * instantly and decaying by half. Finally the grab offset is recomputed
 * from the new point and a fresh read of the input index.
 *
 * Both pragmas are load-bearing. The ROM shares nothing across an extended
 * basic block that 2004/b56 would share by default: dx >> 1 is recomputed
 * after the atan2 call and the input index is re-read at the end, so
 * opt_common_subs is off, and every value the ROM does share is a named
 * local here (i4, bx/by, dy2, j-free ax/ay). opt_propagation off keeps the
 * stylus byte loads above the first store (the ROM loads both before it)
 * and puts i4 in r4. Loading by before bx, and ay before ax, colours the
 * two scratch pairs the ROM's way; the twin func_ov006_020e1854 in
 * dScMgCurling_c reads the same. */
#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
// @symbol func_ov006_020e4bd4
extern "C" void func_ov006_020e4bd4(dScMgCurling2_c *self)
{
    u8 idx;
    int dx;
    int oldx;
    int oldy;
    int diff;
    u16 oldAng;
    int mag;
    int i4;

    idx = data_020a0e40;
    i4 = idx * 4;
    if (data_020a0de8[idx * 4] != 0) {
        int dy2;
        int dy;
        u8 bx;
        u8 by;

        by = data_020a0deb[i4];
        bx = data_020a0dea[i4];
        oldx = self->unk_5584;
        oldy = self->unk_5588;
        self->unk_5584 = self->unk_5594 + (bx << 12);
        self->unk_5588 = self->unk_5598 + (by << 12);
        if (self->unk_5588 <= 0x94000) self->unk_5588 = 0x94000;
        if (self->unk_5584 <= 0x20000) self->unk_5584 = 0x20000;
        if (self->unk_5584 >= 0xe0000) self->unk_5584 = 0xe0000;
        if (self->unk_5588 >= 0xb8000) self->unk_5588 = 0xb8000;
        dx = (self->unk_5584 - oldx) >> 12;
        dy = (self->unk_5588 - oldy) >> 12;
        dy2 = dy * dy;
        if (_ZN4cstd4sqrtEy((u64)(dx * dx + dy2)) <= 1) {
            self->unk_5584 = oldx;
            self->unk_5588 = oldy;
            return;
        }
        diff = (self->unk_5588 - self->unk_5590) >> 12;
        if (self->unk_55be == 0) {
            func_02012718(0x1d6, self->unk_5584);
            self->unk_55be = 2;
            self->unk_55a8 = (self->unk_5588 - self->unk_5590) >> 12;
            self->unk_5590 = self->unk_5588;
        } else if (self->unk_55be == 1) {
            if (self->unk_55a8 * diff > 0) {
                if (diff < 0) diff = -diff;
                if (diff >= 0xa) self->unk_55be = 0;
            } else {
                self->unk_55a8 = diff;
                self->unk_5590 = self->unk_5588;
            }
        } else {
            if (self->unk_55a8 * diff < 0) self->unk_55be = 1;
            self->unk_55a8 = (self->unk_5588 - self->unk_5590) >> 12;
            self->unk_5590 = self->unk_5588;
        }
        oldAng = self->unk_55b2;
        self->unk_55b2 = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
        {
            u16 a = self->unk_55b2;

            if (a <= 0x8000 && a >= 0x4000) {
                self->unk_55b2 = 0x8000;
            } else if (a <= 0x4000) {
                self->unk_55b2 = 0;
            }
        }
        self->unk_55b2 = (self->unk_55b2 + oldAng) >> 1;
        mag = _ZN4cstd4sqrtEy((u64)((dx >> 1) * (dx >> 1) + dy2)) * 9;
        mag = (mag << 12) >> 4;
        if (mag >= 0xc000) mag = 0xc000;
        if (mag > self->unk_559c) self->unk_559c = mag;
        if (self->unk_559c > mag) self->unk_559c -= (self->unk_559c - mag) >> 1;
        {
            int j = data_020a0e40;
            int ay = (self->unk_5588 >> 12) - data_020a0deb[j * 4];
            int ax = (self->unk_5584 >> 12) - data_020a0dea[j * 4];

            self->unk_5594 = ax << 12;
            self->unk_5598 = ay << 12;
        }
        return;
    }
    self->unk_55b8 = 0;
    self->unk_55b9 = 1;
}
#pragma pop


/* [27] 0x020e4ed4 -- begin a drag: latch the grab offset and the first angle.
 * The legacy shard spelled the receiver `this`; renamed to `self`.  Its own
 * `void func_02012718(void *a, int b)` spelling disagreed with the one
 * func_ov006_020e4bd4 and func_ov006_020e513c recovered, so the shared
 * declaration is `void func_02012718(int, int)` and the literal below is
 * spelled as the int it always was. */
// @symbol func_ov006_020e4ed4
extern "C" void func_ov006_020e4ed4(char *self)
{
    int idx;
    int flag = 0;
    int x;
    int y;

    idx = data_020a0e40;
    if (data_020a0de8[idx * (4 & 0xFFFFFFFF)] != 0)
    {
        if (data_020a0de9[idx * 4] != 0)
            flag = 1;
    }
    if (flag == 0)
        return;

    x = ((*((int *)(self + 0x5584))) >> 0xc) -
        data_020a0dea[idx * 4];
    y = ((*((int *)(self + 0x5588))) >> 0xc) -
        data_020a0deb[idx * 4];

    *((int *)(self + 0x5594)) = x << 0xc;
    *((int *)(self + 0x5598)) = y << 0xc;
    *((u8 *)(self + 0x55b8)) = 1;
    *((u16 *)(self + 0x55b2)) = 0xc000;

    if (*((u8 *)(self + 0x55bd)) == 0)
    {
        func_02012718(0x1d2, *((int *)(self + 0x5584)));
        *((u8 *)(self + 0x55bd)) = 6;
    }

    *((int *)(self + 0x55a0)) = 0;
    *((int *)(self + 0x55a4)) = 0;
    *((int *)(self + 0x558c)) =
        (*((int *)(self + 0x5584))) + (*((int *)(self + 0x5594)));
    *((int *)(self + 0x5590)) =
        (*((int *)(self + 0x5588))) + (*((int *)(self + 0x5598)));
    *((int *)(self + 0x55a8)) = 0xff;
    *((u8 *)(self + 0x55be)) = 0;
}


/* [28] 0x020e4fe8 -- draw the "shots left" counter. */
// @symbol func_ov006_020e4fe8
extern "C" void func_ov006_020e4fe8(char *c)
{
    int v;
    if (*(unsigned char *)(c + 0x5000 + 0x5c3) == 0)
        return;
    v = 5 - *(unsigned char *)(c + 0x5000 + 0x5ba);
    if (v < 0)
        v = 0;
    func_ov004_020af948((int)&data_ov006_0213c3fc, 0xd0, 0xb4, 0);
    func_ov004_020af948(data_ov006_0213c4dc[GetGameLanguage()][1], 0xe8, 0xb4, 0);
    func_ov004_020b2444(0xf8, 0xb4, v, 0, 0, 0, 0);
}


/* [29] 0x020e507c -- draw the eleven stones on both screens. */
// @symbol func_ov006_020e507c
extern "C" void func_ov006_020e507c(char* a0) {
    int x, y;
    int i;
    for (i = 0; i < 0xb; i++) {
        void* d;
        if (*(unsigned char*)(a0 + 0x4689) == 0) goto next;
        if (*(unsigned char*)(a0 + 0x468a) == 0) goto next;
        x = *(int*)(a0 + 0x4660) >> 12;
        y = *(int*)(a0 + 0x4664) >> 12;
        if (*(unsigned char*)(a0 + 0x468d) != 0) d = &data_ov006_0213c44c;
        else d = &data_ov006_0213c3fc;
        RenderOamBothScreens(d, x, y, -1, 1, 0);
        RenderOamBothScreens(&data_ov006_0213c454, x, y + 8, -1, 2, 0);
    next:
        a0 += 0x30;
    }
}


/* [30] 0x020e513c -- stone separation, 788 bytes.
 * The two-player rink's twin of func_ov006_020e1dc8: eleven stones at a
 * 0x30 stride instead of five at 0x2c, and after the bump sound the pushed
 * pair is handed to func_ov006_020e39e0. Everything else, including the
 * two levers that closed the Curling version (named vx/vy, the inner scan's
 * own dx2/dy2/dist2/ang2 declared in the loop body), carries over verbatim.
 * The extra live idx/i for the trailing call is what moves k out of fp and
 * spills the 0x800 constant; the compiler does that on its own.
 *
 * The shard declared func_ov006_020e39e0 as taking a dScMgCurling2_c*; the
 * definition above is that shard's own `char*` spelling, so the call casts. */
// @symbol func_ov006_020e513c
extern "C" void func_ov006_020e513c(dScMgCurling2_c *self, int idx)
{
    int i;
    int j;
    int dx;
    int dy;
    int dist;
    u16 ang;
    int k;

    for (i = 0; i < 11; i++) {
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
            for (j = 0; j < 11; j++) {
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
                    func_ov006_020e39e0((char *)self, idx, i);
                    return;
                }
            }
            return;
        }
    }
}
