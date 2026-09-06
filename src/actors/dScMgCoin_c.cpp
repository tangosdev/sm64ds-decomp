//cpp
/* dScMgCoin_c -- the coin minigame scene, ov006.
 *
 * Reconstructed translation unit, PARTIAL: ROM ordinals 3..35 of the contiguous
 * linker run 0x020dbe40..0x020de940, i.e. 0x020dbf7c..0x020dd0e0, 33 functions.
 *
 * WHY A SUB-RANGE.  Two of the run's 62 functions have no source in the tree and
 * no entry in config/arm9/overlays/ov006/delinks.txt: func_ov006_020dbe9c
 * (ordinal 2) and func_ov006_020dd0e0 (ordinal 36).  Both are long-standing
 * banked near-misses, and a near-miss never lands in src/.  Nothing in this tree
 * can express a .text claim with a hole in it, so the run has to be licensed as
 * one of its three contiguous pieces:
 *
 *     segment 1  0x020dbe40..0x020dbe9c   2 sections  D1 and D0, alone
 *     segment 2  0x020dbf7c..0x020dd0e0  33 sections  THIS FILE
 *     segment 3  0x020dd2cc..0x020de988  26 sections  25 shards + the factory
 *
 * Segment 2 is the largest.  The cost is that segment 3 keeps four of the six own
 * vtable overrides (slots 0, 6, 9, 18) and the factory.  A class-named TU holding
 * only helpers is precedented and landed: ov006/dScMgBomroom_c, ov074/Goomboss.
 *
 * So this TU does not own the class's key function -- the first declared virtual
 * is the out-of-line destructor at 0x020dbe40, over in segment 1 -- and emits no
 * vtable and no RTTI, needs no compiler_only_output block, and left
 * include/dScMgCoin_c.h untouched.  That is the only outcome available anyway:
 * ov006's whole .data segment is one delink section owned by no file, so no ov006
 * TU can own its own vtable.
 *
 * DO NOT REORDER.  Function order is the ROM's own, lowest address first, which
 * is what `#pragma defer_codegen off` buys: with codegen deferred, mwccarm
 * 2004/b56 emits one .text section per function in the REVERSE of source order.
 * Dropping that one line costs no bytes but inverts emission order to 32
 * out-of-order pairs, and linkcheck [4b/8] refuses that.
 *
 * The two shards that carried `#pragma opt_common_subs off` and
 * `#pragma opt_strength_reduction off` were measured against a delete-outright
 * control: byte-identical, 33/33 either way, so both pragmas are gone.  Do not
 * generalise from that -- dScMgHanachan_c, same overlay, needs them.
 *
 * Every member keeps its own declarations rather than sharing one canonical
 * spelling: the shards genuinely disagree, and forcing one spelling rewrites call
 * sites.  Three names disagree between members and so are declared at block scope
 * in the bodies that use them, where the enclosing extern "C" still gives them C
 * linkage -- func_ov006_020ddeb0 (char* vs void*), func_ov004_020b0380
 * (void*,int,int,int vs int,int,int,int) and RenderOamMainScreen (a shard-vs-shard
 * RETURN-type disagreement).  Member-local type tags carry a per-member suffix for
 * the same reason: merging a shared struct across members is a codegen hazard, not
 * a naming one.
 *
 * THE SCENE OBJECT.  No layout for dScMgCoin_c is recovered, so every member here
 * reaches its fields through raw offsets off the scene pointer.  These shapes are
 * read off the arithmetic in this file and nothing more -- they are descriptions,
 * not recovered names.  Nothing else in the tree names any of these offsets, and
 * the same addresses mean unrelated things in dScMgSound_c and dScMgMemory2_c.
 *
 *     +0x4ac0  0x18-stride records: position, fall speed, a state byte at +0x10
 *     +0x4d14  0x18-stride records, 0x20 of them: position, velocity, +0x180 of
 *              gravity a frame, an 0x18-frame life, drawn from data_ov006_02136e24
 *     +0x5014  0x10-stride records, 24 of them: seeded from a +0x4ac0 record,
 *              0x18-frame life, drawn as a number at (x >> 12, y >> 12)
 *     +0x519c  0x14-stride records: a delay, a frame counter, a state byte and
 *              the sprite index those two drive
 *     +0x51a8  0x1c-stride records: position, velocity, a countdown and two
 *              little counters
 *     +0x51c8  a scene phase word, tested against 5 all over this file, and
 *              incremented in exactly one place (func_ov006_020dc5c4)
 */

#include "types.h"
#include "decl_common.h"

#pragma defer_codegen off

/* ---------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020dbf7c, 0x020dbf7c, size 0x1d8 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dbf7c
extern "C" {
/* Steps one +0x51a8 record: integrate, spin it a little faster in whichever
 * direction it is travelling, and once it has fallen past the height for its
 * stage, bounce it back up with a random horizontal speed.  field_18 is how many
 * bounces are left in the stage, field_19 the stage; after stage 6 the record
 * just stops being drawn once it is off the bottom.
 *
 * The call to func_02012718 passes 0xef unqualified because decl_common.h types
 * it void(int, int), not the void(void*, int) this member's shard declared.  The
 * header wins; it is byte-neutral either way. */
extern int RandomIntInternal(int *seed);

extern int data_0209d4b8;
extern int data_ov006_0212e370[];
extern int data_ov006_0212e388[];
extern int data_ov006_0212e344[];

typedef struct {
    int field_00;
    int field_04;
    int field_08;
    int field_0c;
    unsigned short field_10;
    unsigned short field_12;
    unsigned char field_14;
    unsigned char field_15;
    unsigned char field_16;
    unsigned char field_17;
    unsigned char field_18;
    unsigned char field_19;
    unsigned char field_1a;
    unsigned char field_1b;
} Sub_bf7c;

struct Obj_bf7c {
    unsigned char pad[0x51a8];
    Sub_bf7c arr[1];
};

void func_ov006_020dbf7c(struct Obj_bf7c *sb, int i)
{
    int v;
    int k;

    sb->arr[i].field_00 += sb->arr[i].field_08;
    sb->arr[i].field_04 += sb->arr[i].field_0c;
    sb->arr[i].field_0c += 0x400;
    if (sb->arr[i].field_08 > 0) {
        sb->arr[i].field_10 += 0x400;
    } else {
        sb->arr[i].field_10 -= 0x400;
    }

    v = sb->arr[i].field_04 >> 12;
    k = sb->arr[i].field_19;

    if (k >= 6) {
        if (v >= 0xd0) {
            sb->arr[i].field_15 = 0;
            sb->arr[i].field_14 = 0;
        }
        return;
    }

    if (sb->arr[i].field_18 != 0) {
        unsigned int r;
        if (v < (data_ov006_0212e370[k] - data_ov006_0212e388[k])) {
            return;
        }
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        sb->arr[i].field_08 = data_ov006_0212e344[((r >> 16 & 0x7fff) << 1) >> 15];
        sb->arr[i].field_0c = -0x3000;
        sb->arr[i].field_18--;
        func_02012718(0xef, sb->arr[i].field_00);
        if (sb->arr[i].field_18 == 0) {
            sb->arr[i].field_19++;
        }
    } else {
        sb->arr[i].field_18 = 1;
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020dc154, 0x020dc154, size 0x70 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc154
extern "C" {
/* Runs the countdown at +0x51ba on one +0x51a8 record; when it expires the
 * record goes to state 3 and the scene is told to re-scan. */
void func_ov006_020dc154(char *base, int idx)
{
    extern void func_ov006_020ddeb0(char *o);

    unsigned short *p = (unsigned short *) ((base + 0x51ba) + (idx * 0x1c));
    if ((*p) != 0) {
        *p = (*p) - 1;
        return;
    }
    {
        /* Only the first store goes through o; spelling the other three out from
         * base again is what the original does, and folding them into o costs
         * eight words.  Measured, this file, 2026-09-06. */
        char *o = (base + (idx * 0x1c)) + 0x5000;
        *((unsigned char *) (o + 0x1bf)) = 3;
        *((int *) (((base + (idx * 0x1c)) + 0x5000) + 0x1b4)) = 0;
        *((unsigned char *) (((base + (idx * 0x1c)) + 0x5000) + 0x1c0)) = 1;
        *((unsigned char *) (((base + (idx * 0x1c)) + 0x5000) + 0x1c1)) = 0;
    }
    func_ov006_020ddeb0(base);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020dc1c4, 0x020dc1c4, size 0xa8 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc1c4
extern "C" {
/* The falling half of the same record: apply gravity and spin until it reaches
 * the floor at -0xf0, then park it there, put it in state 2 and start the 0x30
 * frame countdown func_ov006_020dc154 runs down. */
void func_ov006_020dc1c4(void *thiz, int idx)
{
    extern void func_ov006_020ddeb0(void *thiz);

    unsigned char *c = (unsigned char *)thiz;
    int off = idx * 0x1c;

    *(int *)(c + 0x51ac + off) = *(int *)(c + 0x51ac + off) + *(int *)(c + 0x51b4 + off);
    *(int *)(c + 0x51b4 + off) = *(int *)(c + 0x51b4 + off) - 0x100;
    *(unsigned short *)(c + 0x51b8 + off) = *(unsigned short *)(c + 0x51b8 + off) + 0x400;

    if ((*(int *)(c + 0x51ac + off) >> 0xc) > -0xf0)
        return;

    *(int *)(c + 0x51ac + off) = -0xf0000;
    *(unsigned char *)(c + 0x51bf + off) = 2;
    *(unsigned short *)(c + 0x51ba + off) = 0x30;
    func_ov006_020ddeb0(c);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020dc26c, 0x020dc26c, size 0x28 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc26c
extern "C" {
/* Kicks record 0 into state 1 and gives it a little upward hop. */
void func_ov006_020dc26c(char *c)
{
    *(char *)(c + 0x51bf) = 1;
    *(char *)(c + 0x51be) = 1;
    *(int *)(c + 0x51b0) = 0;
    *(int *)(c + 0x51b4) = -0x4000;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020dc294, 0x020dc294, size 0x4 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc294
extern "C" {
/* Deliberately empty: the first entry of the scene's callback table at
 * 0x0213be44 points here, so state 0 does nothing. */
void func_ov006_020dc294(void)
{
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020dc298, 0x020dc298, size 0x60 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc298
extern "C" {
struct C_c298;
typedef void (C_c298::*PMF_c298)(int);
struct Entry_c298 { PMF_c298 pmf[1]; };
extern Entry_c298 data_ov006_021417c8[];
struct C_c298 {
    char pad[0x51bc];
    unsigned char guard;
    char pad2[2];
    unsigned char idx;
};

/* Per-frame step for the +0x51a8 records: dispatch on the state byte at +0x51bf
 * through the pointer-to-member table at 0x021417c8, but only while the enable
 * flag at +0x51bc is set. */
void func_ov006_020dc298(C_c298 *c)
{
    if (c->guard == 0)
        return;

    int j = c->idx;
    (c->*data_ov006_021417c8[j].pmf[0])(0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020dc2f8, 0x020dc2f8, size 0x3c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc2f8
extern "C" {
/* Resets the +0x51a8 records to their start-of-round position and enables them. */
void func_ov006_020dc2f8(char *c)
{
    *(char *)(c + 0x51bc) = 1;
    *(char *)(c + 0x51bd) = 1;
    *(char *)(c + 0x51be) = 0;
    *(char *)(c + 0x51bf) = 0;
    *(short *)(c + 0x51b8) = 0;
    *(int *)(c + 0x51a8) = 0x8e000;
    *(int *)(c + 0x51ac) = -0x43000;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020dc334, 0x020dc334, size 0x14 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc334
extern "C" {
/* The other half of func_ov006_020dc2f8: clears both enable flags. */
void func_ov006_020dc334(char *p)
{
    *(char *)(p + 0x51bc) = 0;
    *(char *)(p + 0x51bd) = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020dc348, 0x020dc348, size 0x28 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc348
extern "C" {
/* Puts the +0x519c animation into state 4 from frame 0 and latches the flag at
 * +0x51de, which func_ov006_020dc370 then reads as "leave me alone". */
void func_ov006_020dc348(char *c)
{
    *(char *)(c + 0x51a1) = 4;
    *(char *)(c + 0x51a4) = 0;
    *(short *)(c + 0x519e) = 0;
    *(char *)(c + 0x51de) = 1;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020dc370, 0x020dc370, size 0x4c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc370
extern "C" {
/* Starts, or retriggers, the 0x3c frame state-2 animation.  Retriggering only
 * refreshes the delay; the frame counter is left alone. */
void func_ov006_020dc370(char *c)
{
    if (*(unsigned char *)(c + 0x51de) != 0)
        return;

    if (*(unsigned char *)(c + 0x51a1) == 2) {
        *(short *)(c + 0x519c) = 0x3c;
        return;
    }

    *(short *)(c + 0x519c) = 0x3c;
    *(char *)(c + 0x51a1) = 2;
    *(char *)(c + 0x51a4) = 0;
    *(short *)(c + 0x519e) = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020dc3bc, 0x020dc3bc, size 0x58 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc3bc
extern "C" {
extern int data_ov006_0213a9fc[];

/* Draws the +0x5194 sprite, if it is visible, at its own fixed-point position.
 * The sprite index at +0x51a3 selects a cell out of data_ov006_0213a9fc. */
void func_ov006_020dc3bc(char *c)
{
    extern int RenderOamMainScreen(int a, int b, int c, int d, int e);

    if (*(unsigned char *)(c + 0x5000 + 0x1a2) == 0)
        return;

    int x = *(int *)(c + 0x5000 + 0x194);
    int y = *(int *)(c + 0x5000 + 0x198);
    RenderOamMainScreen(data_ov006_0213a9fc[*(unsigned char *)(c + 0x5000 + 0x1a3)],
                        x >> 0xc, y >> 0xc, -1, -1);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020dc414, 0x020dc414, size 0x9c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc414
extern "C" {
extern unsigned char data_ov006_0212e324[];
extern unsigned char data_ov006_0212e31c[];

/* Eight-frame looping animation: hold each frame for its own duration out of
 * data_ov006_0212e324, then step the sprite index out of data_ov006_0212e31c.
 * Frame 0 is the intro, so the loop comes back round to 1 rather than 0. */
void func_ov006_020dc414(char *base, int idx)
{
    unsigned short *cnt = (unsigned short *)(base + 0x519e + idx * 0x14);
    unsigned char *st = (unsigned char *)(base + 0x51a4 + idx * 0x14);

    *cnt = *cnt + 1;
    if (*cnt < data_ov006_0212e324[*st])
        return;

    *cnt = 0;
    *st = *st + 1;
    if (*st >= 8) {
        *st = 1;
    } else {
        *(unsigned char *)(base + idx * 0x14 + 0x5000 + 0x1a3) = data_ov006_0212e31c[*st];
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020dc4b0, 0x020dc4b0, size 0x18 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc4b0
extern "C" {
/* Back to sprite cell 0. */
void func_ov006_020dc4b0(char *c, int i)
{
    *(char *)(c + i * 0x14 + 0x5000 + 0x1a3) = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020dc4c8, 0x020dc4c8, size 0xfc */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc4c8
extern "C" {
extern unsigned char data_ov006_0212e308[];
extern unsigned char data_ov006_0212e30c[];

void func_ov006_020dc4c8(char *c, int i)
{
    int n = i * 0x14;
    unsigned short cnt = *((unsigned short *) ((c + 0x519e) + n));
  *((unsigned short *) ((c + 0x519e) + n)) = cnt + 1;
    unsigned char idx = *((unsigned char *) ((c + 0x51a4) + n));
    if ((*((unsigned short *) ((c + 0x519e) + n))) >= data_ov006_0212e308[idx]) {
    *((unsigned short *) ((c + 0x519e) + n)) = 0;
    *((unsigned char *) ((c + 0x51a4) + n)) = (*((unsigned char *) ((c + 0x51a4) + n))) + 1;
    *((unsigned char *) ((c + 0x51a4) + n)) = (*((unsigned char *) ((c + 0x51a4) + n))) & 1;
    }
  *((unsigned char *) ((c + n) + 0x51a3)) = data_ov006_0212e30c[*((unsigned char *) ((c + 0x51a4) + n))];
    if ((*((int *) (c + 0x51c8))) == 5) {
        return;
    }
    unsigned short w = *((unsigned short *) ((c + 0x519c) + n));
    if (w != 0) {
    *((unsigned short *) ((c + 0x519c) + n)) = w - 1;
        return;
    }
  *((c + n) + 0x51a1) = 3;
  *((unsigned char *) ((c + 0x51a4) + n)) = 0;
  *((unsigned short *) ((c + 0x519e) + n)) = 0;
  *((unsigned short *) ((c + 0x519c) + n)) = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020dc5c4, 0x020dc5c4, size 0x10c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc5c4
extern "C" {
extern void func_ov006_020dc26c(char *c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern u16 data_ov006_0212e33c[];
extern u8 data_ov006_0212e310[];

/* The count-in animation.  Steps through data_ov006_0212e33c's per-frame
 * durations; frame 2 hops record 0 and plays the "ready" sound, and after frame 3
 * the slot drops into state 2 with a 0xb4 frame delay and the scene phase at
 * +0x51c8 advances.  This is the only place that phase word is incremented. */
void func_ov006_020dc5c4(char *c, int i)
{
    int off = i * 0x14;
    u16 *counter = (u16 *)(c + 0x519e + off);
    u8  *st = (u8 *)(c + 0x51a4 + off);
    *counter = (u16)(*counter + 1);
    if (*counter < data_ov006_0212e33c[*st])
        return;
    *counter = 0;
    *st = (u8)(*st + 1);
    if (*st == 2) {
        func_ov006_020dc26c(c);
        _ZN5Sound12PlayBank2_2DEj(0xf0);
    }
    if (*st >= 4) {
        *(u16 *)(c + 0x519c + off) = 0xb4;
        *(u8 *)(c + 0x51a1 + off) = 2;
        {
            int *p = (int *)(c + 0x51c8);
            *st = 0;
            *counter = 0;
            *p = *p + 1;
        }
    } else {
        *(u8 *)(c + 0x51a3 + off) = data_ov006_0212e310[*st];
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020dc6d0, 0x020dc6d0, size 0x84 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc6d0
extern "C" {
extern void _ZN5Sound12PlayBank2_2DEj(unsigned);

/* Runs the state-2 delay down, chiming 0x18 frames before it expires, and then
 * hands the slot back to state 1 at frame 0.
 *
 * The (unsigned long long) on the last line is load-bearing, not litter: taking
 * it out costs five words.  Measured, this file, 2026-09-06. */
void func_ov006_020dc6d0(int o, int idx)
{
    unsigned short *timer = (unsigned short *) ((((char *) o) + 0x519c) + (idx * 0x14));
    if ((*timer) != 0) {
        *timer = (*timer) - 1;
        if ((*timer) == 0x18) {
            _ZN5Sound12PlayBank2_2DEj(0xf1);
        }
        return;
    }
    *((unsigned char *) (((((char *) o) + (idx * 0x14)) + 0x5000) + 0x1a1)) = 1;
    *((unsigned short *) (((((char *) ((unsigned long long) o)) + (idx * 0x14)) + 0x5000) + 0x19e)) = 0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020dc754, 0x020dc754, size 0x60 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc754
extern "C" {
struct C_c754;
typedef void (C_c754::*PMF_c754)(int);
struct Entry_c754 { PMF_c754 pmf[1]; };
extern Entry_c754 data_ov006_021417e8[];
struct C_c754 {
    char pad[0x51a0];
    unsigned char guard;
    unsigned char idx;
};

/* Same shape as func_ov006_020dc298, one record set over: dispatch the +0x519c
 * slot's state byte through the table at 0x021417e8 while its flag is set. */
void func_ov006_020dc754(C_c754 *c)
{
    if (c->guard == 0)
        return;

    int j = c->idx;
    (c->*data_ov006_021417e8[j].pmf[0])(0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020dc7b4, 0x020dc7b4, size 0x48 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc7b4
extern "C" {
/* Start of round: park the +0x519c slot at the middle of the screen, visible, in
 * state 0 on sprite cell 3, with 0x28 frames on the clock. */
void func_ov006_020dc7b4(char *c)
{
    *(char *)(c + 0x5000 + 0x1a0) = 1;
    *(char *)(c + 0x5000 + 0x1a1) = 0;
    *(char *)(c + 0x5000 + 0x1a2) = 1;
    *(short *)(c + 0x5000 + 0x19c) = 0x28;
    *(short *)(c + 0x5000 + 0x19e) = 0;
    *(char *)(c + 0x5000 + 0x1a3) = 3;
    *(char *)(c + 0x5000 + 0x1a4) = 0;
    *(int *)(c + 0x5000 + 0x194) = 0x70000;
    *(int *)(c + 0x5000 + 0x198) = 0x98000;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020dc7fc, 0x020dc7fc, size 0x18 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc7fc
extern "C" {
/* End of round: stop stepping the slot but leave it on screen. */
void func_ov006_020dc7fc(char *p)
{
    *(char *)(p + 0x51a0) = 0;
    *(char *)(p + 0x51a2) = 1;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020dc814, 0x020dc814, size 0x5c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc814
extern "C" {
extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);

/* In the last phase only, and only over the final 0x80 frames, draw the
 * countdown overlay.  Nothing at all once it reaches zero. */
void func_ov006_020dc814(char *c)
{
    if (*(int *)(c + 0x5000 + 0x1c8) != 5)
        return;

    int v = *(int *)(c + 0x5000 + 0x1cc);
    if (v > 0x80)
        return;
    if (v == 0)
        return;

    func_ov004_020b0d8c(c, 0xe0, 0xa0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020dc870, 0x020dc870, size 0x90 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc870
extern "C" {
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);

typedef struct {
    int x;
    int y;
    unsigned char p0[2];
    unsigned short num;
    unsigned char p1;
    unsigned char flag;
    unsigned char p2[2];
} Elem_c870;

typedef struct {
    unsigned char _pad[0x5014];
    Elem_c870 arr[24];
} Obj_c870;

/* Draws all 24 live +0x5014 popups.  Once the scene is in its last phase they
 * are suppressed as soon as the countdown at +0x51cc hits zero. */
void func_ov006_020dc870(Obj_c870 *o)
{
    int i;
    if (*(int *)((char *)o + 0x51c8) == 5) {
        if (*(int *)((char *)o + 0x51cc) == 0)
            return;
    }
    for (i = 0; i < 0x18; i++) {
        if (o->arr[i].flag != 0) {
            func_ov004_020b2444(o->arr[i].x >> 12, o->arr[i].y >> 12, o->arr[i].num, 0, -1, 0, 0);
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020dc900, 0x020dc900, size 0x60 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc900
extern "C" {
/* Ages the 24 +0x5014 popups a frame each and marks the expired ones done.
 *
 * Both cast towers here are load-bearing.  The (long long) on the index and the
 * (int) round-trip on the two +0x501c accesses each cost a word if removed --
 * the (int) is what stops mwccarm reusing the address it just computed for the
 * read-modify-write.  Measured, this file, 2026-09-06. */
void func_ov006_020dc900(char *c)
{
    int i;
    for (i = 0; i < 0x18; i++) {
        char *p = c + (int)((long long)i) * 0x10;
        if (*(u8 *)(p + 0x5020) != 0 && *(u16 *)(p + 0x501c) != 0) {
            *(u16 *)((int)(p + 0x501c)) -= 1;
            if (*(s16 *)((int)(p + 0x501c)) <= 0)
                *(u8 *)(p + 0x5021) = 1;
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020dc960, 0x020dc960, size 0x3c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc960
extern "C" {
/* Spawns popup i over coin i: same position, the coin's value, 0x18 frames.
 * The two strides differ -- 0x18 for the coin, 0x10 for the popup. */
void func_ov006_020dc960(char *c, int i)
{
    char *s = c + i * 0x18;
    char *d = c + i * 0x10;
    *(int *)(d + 0x5014) = *(int *)(s + 0x4ac0);
    *(int *)(d + 0x5018) = *(int *)(s + 0x4ac4);
    *(short *)(d + 0x501e) = *(unsigned char *)(s + 0x4ad3);
    *(unsigned char *)(d + 0x5020) = 1;
    *(short *)(d + 0x501c) = 0x18;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020dc99c, 0x020dc99c, size 0x68 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc99c
extern "C" {
extern void *data_ov006_02136e24[];
struct E_c99c { char pad[0x18]; };

/* Draws whichever of the 0x20 +0x4d14 sparkles are visible, each with its own
 * sprite out of data_ov006_02136e24. */
void func_ov006_020dc99c(char *c)
{
    extern void func_ov004_020b0380(void *fn, int a, int b, int d);

    int i;
    struct E_c99c *arr = (struct E_c99c *)c;
    for (i = 0; i < 0x20; i++) {
        char *base = (char *)&arr[i];
        if (*(unsigned char *)(base + 0x4d29) != 0) {
            int k = *(unsigned char *)(base + 0x4d2a);
            int a = *(int *)(base + 0x4d14);
            int b = *(int *)(base + 0x4d18);
            func_ov004_020b0380(data_ov006_02136e24[k], a >> 0xc, b >> 0xc, 0);
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_020dca04, 0x020dca04, size 0x118 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dca04
extern "C" {
/* Steps the 0x20 +0x4d14 sparkles: gravity, and horizontal drag that pulls any
 * speed above 0x300 back towards it.  The early return when one expires is the
 * original's, not a transcription slip -- only one sparkle is retired a frame. */
void func_ov006_020dca04(char *o)
{
    int i;
    char *q = o;
    for (i = 0; i < 0x20; i++, q += 0x18) {
        if (*(u8 *)(q + 0x4d28) != 0) {
            if (*(u16 *)(q + 0x4d24) != 0) {
                *(u16 *)(q + 0x4d24) -= 1;
            } else {
                *(u8 *)(o + i * 0x18 + 0x4d28) = 0;
                *(u8 *)(o + i * 0x18 + 0x4d29) = 0;
                return;
            }
            *(int *)(q + 0x4d14) += *(int *)(q + 0x4d1c);
            *(int *)(q + 0x4d18) += *(int *)(q + 0x4d20);
            *(int *)(q + 0x4d20) += 0x180;
            if (*(int *)(q + 0x4d1c) > 0x300)
                *(int *)(q + 0x4d1c) -= 0x180;
            else if (*(int *)(q + 0x4d1c) < -0x300)
                *(int *)(q + 0x4d1c) += 0x180;
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_020dcb1c, 0x020dcb1c, size 0x12c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dcb1c
extern "C" {
extern int data_ov006_0212e430[];
extern u16 data_ov006_0212e334[];
extern s16 data_02082214[];

/* Bursts four sparkles out of coin a1, one per fixed direction in
 * data_ov006_0212e334.  The offsets come in x/z pairs out of
 * data_ov006_0212e430, the speeds off the shared sine table at 0x02082214. */
void func_ov006_020dcb1c(char *o, int a1)
{
    int i, j, k;
    char *b = o + a1 * 0x18;
    int *px = (int *)(b + 0x4ac0);
    int *pz = (int *)(b + 0x4ac4);
    for (i = 0, j = 0, k = 1; i < 4; i++, o += 0x18, j += 2, k += 2) {
        if (*(u8 *)(o + 0x4d28) == 0) {
            *(u8 *)(o + 0x4d28) = 1;
            *(u8 *)(o + 0x4d29) = 1;
            *(int *)(o + 0x4d14) = *px + (data_ov006_0212e430[j] << 12);
            *(int *)(o + 0x4d18) = *pz + (data_ov006_0212e430[k] << 12);
            {
                u16 ang = data_ov006_0212e334[i];
                int idx = (ang >> 4) * 2;
                *(int *)(o + 0x4d1c) = (int)(((s64)data_02082214[idx + 1] * 0x2000 + 0x800) >> 12);
                *(int *)(o + 0x4d20) = (int)(((s64)data_02082214[idx] * 0x2000 + 0x800) >> 12);
            }
            *(u16 *)(o + 0x4d24) = 0x18;
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_020dcc48, 0x020dcc48, size 0x70 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dcc48
extern "C" {
extern int data_ov006_021341ec;
extern int data_ov006_0212e364[];

/* Tiles the backdrop: three rows of sixteen, 0x20 apart, all the same sprite. */
void func_ov006_020dcc48(void)
{
    extern void func_ov004_020b0380(int a, int b, int c, int d);

    int i;
    int s, v, j;
    for (i = 0; i < 3; i++) {
        v = data_ov006_0212e364[i];
        s = 0x10;
        for (j = 0; j < 0x10; j++) {
            func_ov004_020b0380(data_ov006_021341ec, s, v, 0);
            s += 0x20;
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_020dccb8, 0x020dccb8, size 0xbc */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dccb8
extern "C" {
extern int *data_ov006_0213bf34[];
int GetGameLanguage(void);
void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);

/* The top line of the HUD, once the scene is past its opening phase: a caption
 * from the current language's table, a label, and the count at +0x51d4.
 * GetGameLanguage is asked twice on purpose: hoisting the second call out costs
 * eighteen words here.  Measured, this file, 2026-09-06. */
void func_ov006_020dccb8(char *thiz)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

    int idx;
    if (*(int *)(thiz + 0x5000 + 0x1c8) < 2)
        return;

    idx = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[idx][8], 0x80, 0x18, -1, -1);
    func_ov004_020b1de8(0x68, 0x28, 1, -1);
    idx = GetGameLanguage();
    DrawOamSprite((void *)data_ov006_0213bf34[idx][1], (void *)0x7a, 0x28, (void *)0);
    func_ov004_020b2444(0x8c, 0x28, *(int *)(thiz + 0x5000 + 0x1d4), 1, -1, 2, 0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_020dcd74, 0x020dcd74, size 0xc8 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dcd74
extern "C" {
extern int *data_ov006_0213bf34[];
extern int *data_ov004_020beb68;
int GetGameLanguage(void);
void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);

/* The second line of the HUD, laid out exactly like the first, except that the
 * number is the other player's and is read out of ov004's record -- which may
 * not exist yet, in which case it shows zero. */
void func_ov006_020dcd74(char *thiz)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

    int idx;
    int r2;
    if (*(int *)(thiz + 0x5000 + 0x1c8) < 2)
        return;

    idx = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[idx][4], 0x80, 0x50, -1, -1);
    func_ov004_020b1de8(0x6c, 0x60, 1, -1);
    idx = GetGameLanguage();
    DrawOamSprite((void *)data_ov006_0213bf34[idx][1], (void *)0x7e, 0x60, (void *)0);
    if (data_ov004_020beb68 != 0)
        r2 = *(int *)((char *)data_ov004_020beb68 + 0xac);
    else
        r2 = 0;
    func_ov004_020b2444(0x90, 0x60, r2, 1, -1, 2, 0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_020dce3c, 0x020dce3c, size 0x6c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dce3c
extern "C" {
/* Walks the displayed total at +0x4d0a up towards the real one at +0x4d08, one
 * step every eight frames, so the score counts up rather than jumping. */
void func_ov006_020dce3c(char *c)
{
    if (*(unsigned char *)(c + 0x4000 + 0xd13) == 0)
        return;
    if (*(unsigned short *)(c + 0x4d08) == *(unsigned short *)(c + 0x4d0a))
        return;
    {
        unsigned short *e = (unsigned short *)(c + 0x4d0c);
        *e = *e + 1;
    }
    if (*(unsigned short *)(c + 0x4d0c) < 8)
        return;
    *(unsigned short *)(c + 0x4d0c) = 0;
    {
        unsigned short *p = (unsigned short *)(c + 0x4d0a);
        *p = *p + 1;
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov006_020dcea8, 0x020dcea8, size 0x154 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dcea8
extern "C" {
struct E_cea8 {
    int x;
    int y;
    char pad8[8];
    unsigned char b10;
    unsigned char b11;
    unsigned char b12;
    unsigned char b13;
    unsigned char b14;
    unsigned char b15;
    unsigned char b16;
    unsigned char b17;
};

struct Obj_cea8 {
    char pad0[0x4ac0];
    struct E_cea8 arr[24];
    char pad1[0x4c8];
    int a;
    int b;
};

extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void* data_ov006_02133f10[];

/* Draws the 24 coins.  Before the last phase a coin is drawn whenever b12 is
 * set; in the last phase only the ones that were collected are, with their value
 * printed over them.  b10 == 2 picks the second sprite and the highlight. */
void func_ov006_020dcea8(struct Obj_cea8 *o)
{
    int i;
    int v1, v2;
    if (o->a == 5 && o->b == 0)
        return;

    v1 = 0;
    v2 = 0;
    for (i = 0; i < 24; i++) {
        int t = o->arr[i].b10;
        int idx = (t == 2) ? 1 : v1;
        int x = o->arr[i].x >> 12;
        int y = o->arr[i].y >> 12;
        int val = (o->a == 5) ? 1 : v2;
        if (t == 2) val = 1;
        if (o->a != 5) {
            if (o->arr[i].b12 != 0) {
                func_ov004_020afdd0(data_ov006_02133f10[idx], x, y, -1, val);
            }
        } else {
            if (idx != 0 && o->arr[i].b13 != 0) {
                func_ov004_020afdd0(data_ov006_02133f10[idx], x, y, -1, val);
            }
            if (o->arr[i].b15 == 0 && o->arr[i].b13 != 0) {
                func_ov004_020b2444(x, y, o->arr[i].b13, 0, -1, 0, 0);
            }
        }
    }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov006_020dcffc, 0x020dcffc, size 0x4 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dcffc
extern "C" {
/* Another table no-op, like func_ov006_020dc294. */
void func_ov006_020dcffc(void)
{
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_020dd000, 0x020dd000, size 0xe0 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dd000
extern "C" {
extern void func_ov006_020dc370(char *c);
extern void func_ov006_020dc960(char *c, int i);
extern void func_ov006_020dd4b0(char *c, int i);

/* One hop of a bouncing coin: fall, land on the floor recorded at +0x4ac8, count
 * the bounce and start the caption animation.  After the last bounce the coin
 * settles into state 2 and leaves a popup behind; otherwise it goes back up. */
void func_ov006_020dd000(char *c, int i)
{
    int off = i * 0x18;
    *(s32 *)(c + 0x4ac4 + off) = *(s32 *)(c + 0x4ac4 + off) + *(s32 *)(c + 0x4acc + off);
    *(s32 *)(c + 0x4acc + off) = *(s32 *)(c + 0x4acc + off) + 0x400;
    if (*(s32 *)(c + 0x4ac4 + off) < *(s32 *)(c + 0x4ac8 + off)) {
        return;
    }
    *(s32 *)(c + 0x4ac4 + off) = *(s32 *)(c + 0x4ac8 + off);
    *(u8 *)(c + 0x4ad6 + off) = *(u8 *)(c + 0x4ad6 + off) + 1;
    func_ov006_020dc370(c);
    if (*(u8 *)(c + 0x4ad3 + off) == *(u8 *)(c + 0x4ad6 + off)) {
        *(u8 *)(c + 0x4ad0 + off) = 2;
        func_ov006_020dc960(c, i);
        return;
    }
    *(s32 *)(c + 0x4acc + off) = -0x3000;
    func_ov006_020dd4b0(c, i);
}
}
