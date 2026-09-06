//cpp
/* dScMgCoin_c -- the coin minigame scene, ov006.
 *
 * Reconstructed translation unit: ROM ordinals 3..35 of the contiguous linker
 * run 0x020dbe40..0x020de940, i.e. 0x020dbf7c..0x020dd0e0, 33 functions.
 * Assembled from the 33 one-function legacy sources, then reconciled by hand
 * (declarations, struct spellings, one decl_common.h signature).
 *
 * ------------------------------------------------------------------------
 * WHY THIS IS A SUB-RANGE: TWO SOURCELESS HOLES, NOT ONE
 * ------------------------------------------------------------------------
 * tu_map calls 0x020dbe40..0x020de940 one 62-function linker run.  Two of
 * those 62 have no source anywhere in the tree and no entry in
 * config/arm9/overlays/ov006/delinks.txt at all -- the cartridge's own bytes
 * cover them:
 *
 *   ROM ordinal 2   func_ov006_020dbe9c  0x020dbe9c  size 0x0e0
 *   ROM ordinal 36  func_ov006_020dd0e0  0x020dd0e0  size 0x1ec
 *
 * Both are long-standing banked near-misses, not unattempted work.
 * func_ov006_020dbe9c has fourteen logged attempts in
 * config/match_attempts.jsonl across four models and three harnesses; the best
 * standing result is 7 divergent words (nearmiss/db.jsonl carries that draft),
 * and the last three sessions all recorded no_progress against a residual its
 * own notes call a welded shift ordering plus a register-colouring cascade.
 * func_ov006_020dd0e0 is banked in nearmiss/db.jsonl at 34 divergences.  A
 * near-miss never lands in src/, so neither can be brought in here.
 *
 * Nothing in this tree can express a .text claim with a hole in it: checked
 * tree-wide on this base, zero of 148 TU manifests and zero delink blocks in
 * any overlay delinks.txt carry two .text runs.  So the run has to be licensed
 * as one of its THREE contiguous pieces, and the arithmetic that picks this
 * one is:
 *
 *   segment 1  0x020dbe40..0x020dbe9c   2 sections  -- D1 and D0, alone
 *   segment 2  0x020dbf7c..0x020dd0e0  33 sections  -- THIS FILE
 *   segment 3  0x020dd2cc..0x020de988  26 sections  -- 25 shards + the factory
 *
 * Segment 2 is the largest side by shard count, which is the queue's own
 * ranking metric, and 33 beats segment 3's ceiling of 26 even after segment 3
 * absorbs dScMgCoin_c_classInit from src/d_s_mg_coin.cpp.  The cost of the
 * choice, stated plainly: segment 3 holds four of the class's six own vtable
 * overrides (slots 0, 6, 9, 18) and the factory, so those stay in their own
 * shards.  A TU of only helpers under a class-named file is precedented and
 * landed -- ov006/dScMgBomroom_c (41 functions) and ov074/Goomboss (36) both
 * promoted with no mangled class member at all.
 *
 * NOT A DESTRUCTOR-ORDER REFUSAL.  The cartridge orders this class D1
 * (0x020dbe40) BELOW D0 (0x020dbe64) with no D2 anywhere in the image, which is
 * a reproducible direction; the pair is out only because it lies on the far
 * side of the first hole.  Consequently this TU does not own the class's key
 * function -- the first declared virtual is the out-of-line destructor at
 * 0x020dbe40, in segment 1 -- so it emits no vtable and no RTTI, needs no
 * compiler_only_output block (verify reports zero unlicensed symbols), and
 * needed no edit to include/dScMgCoin_c.h.  That is also the only outcome
 * available: ov006's entire .data segment is one delink section owned by no
 * file, so no ov006 TU can own its own vtable.
 *
 * ------------------------------------------------------------------------
 * THE TWO PRAGMA SECTIONS ARE INERT -- MEASURED, NOT ASSUMED
 * ------------------------------------------------------------------------
 * The legacy shards of func_ov006_020dca04 and func_ov006_020dcb1c each carried
 * `#pragma opt_common_subs off` and `#pragma opt_strength_reduction off`, which
 * is the queue's `pragma:2` blocker.  Three configurations were compiled on this
 * identical member set:
 *
 *   defer_codegen off + both pragmas bracketed push/pop   33/33, ROM-ascending
 *   defer_codegen off + both pragmas deleted outright     33/33, ROM-ascending
 *   pragmas deleted, defer_codegen off deleted too        33/33, 32 pairs NOT
 *                                                          in ROM order
 *
 * The delete-outright control is the decisive one: it is byte-identical to the
 * bracketed build, so both pragmas buy zero members here and are gone from this
 * file.  This is the same verdict dScMgRoulette_c reached on its own `pragma:1`,
 * and the opposite of dScMgHanachan_c, where the same two switches were
 * load-bearing on the same overlay.  The count in the queue column is a count of
 * shards carrying a #pragma line and never was evidence.
 *
 * FUNCTION ORDER IS DELIBERATELY THE ROM'S OWN, LOWEST ADDRESS FIRST.  That is
 * what `#pragma defer_codegen off` buys: with codegen deferred mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, and a
 * merged TU has to be written backwards to compensate.  Generating at parse time
 * emits them in source order instead.  The third control above is the proof --
 * dropping that one line from this ascending file costs nothing in bytes and
 * inverts emission order to 32 out-of-order pairs, which linkcheck [4b/8]
 * refuses.  Do not reorder.
 *
 * ------------------------------------------------------------------------
 * WHAT WAS RECONCILED
 * ------------------------------------------------------------------------
 * Every member keeps its own declarations rather than sharing one canonical
 * spelling, because the shards genuinely disagree and forcing one rewrites call
 * sites.  Three names disagree between members and are therefore declared at
 * block scope inside the bodies that use them, where the enclosing extern "C"
 * still gives them C linkage: func_ov006_020ddeb0 (char* vs void*),
 * func_ov004_020b0380 (void*,int,int,int vs int,int,int,int) and
 * RenderOamMainScreen (int(...) vs void(...) -- a shard-vs-shard RETURN-type
 * disagreement, which tubuild's conflict detector did not report at all).
 *
 * Member-local type tags that collided across members are uniquified per member
 * rather than merged: merging a shared struct is a codegen hazard, not a naming
 * one.  Sub/Obj, C/PMF/Entry, Elem and E all carry a per-member tag suffix here.
 *
 * One real header disagreement: include/decl_common.h:1995 types func_02012718
 * as void(int, int) where the func_ov006_020dbf7c shard declared it
 * void(void*, int).  The header wins -- the shard's local declaration is gone
 * and the call site passes 0xef unqualified.  Byte-neutral: 33/33 either way.
 */

#include "types.h"
#include "decl_common.h"

#pragma defer_codegen off

/* ---------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020dbf7c, 0x020dbf7c, size 0x1d8 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dbf7c
extern "C" {
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
void func_ov006_020dc154(char *base, int idx)
{
    extern void func_ov006_020ddeb0(char *o);
  unsigned short *p = (unsigned short *) ((base + 0x51ba) + (idx * 0x1c));
  if ((*p) != 0)
  {
    *p = (*p) - 1;
    return;
  }
  {
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
void func_ov006_020dc1c4(void *thiz, int idx)
{
    extern void func_ov006_020ddeb0(void *thiz);
    unsigned char *c = (unsigned char *)thiz;
    int off = idx * 0x1c;

    *(int *)(c + 0x51ac + off) = *(int *)(c + 0x51ac + off) + *(int *)(c + 0x51b4 + off);
    *(int *)(c + 0x51b4 + off) = *(int *)(c + 0x51b4 + off) - 0x100;
    *(unsigned short *)(c + 0x51b8 + off) = *(unsigned short *)(c + 0x51b8 + off) + 0x400;

    if ((*(int *)(c + 0x51ac + off) >> 0xc) > -0xf0) return;

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
void func_ov006_020dc26c(char *c){
  *(char*)(c+0x51bf)=1;
  *(char*)(c+0x51be)=1;
  *(int*)(c+0x51b0)=0;
  *(int*)(c+0x51b4)=-0x4000;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020dc294, 0x020dc294, size 0x4 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc294
extern "C" {
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

void func_ov006_020dc298(C_c298 *c)
{
    if (c->guard == 0) return;
    int j = c->idx;
    (c->*data_ov006_021417c8[j].pmf[0])(0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020dc2f8, 0x020dc2f8, size 0x3c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc2f8
extern "C" {
void func_ov006_020dc2f8(char *c){
  *(char*)(c+0x51bc)=1;
  *(char*)(c+0x51bd)=1;
  *(char*)(c+0x51be)=0;
  *(char*)(c+0x51bf)=0x43000;
  *(short*)(c+0x51b8)=0;
  *(int*)(c+0x51a8)=0x8e000;
  *(int*)(c+0x51ac)=-0x43000;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020dc334, 0x020dc334, size 0x14 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc334
extern "C" {
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
void func_ov006_020dc348(char *c){
  *(char*)(c+0x51a1)=4;
  *(char*)(c+0x51a4)=0;
  *(short*)(c+0x519e)=0;
  *(char*)(c+0x51de)=1;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020dc370, 0x020dc370, size 0x4c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc370
extern "C" {
void func_ov006_020dc370(char *c){
  if(*(unsigned char*)(c+0x51de)!=0) return;
  if(*(unsigned char*)(c+0x51a1)==2){
    *(short*)(c+0x519c)=0x3c;
    return;
  }
  *(short*)(c+0x519c)=0x3c;
  *(char*)(c+0x51a1)=2;
  *(char*)(c+0x51a4)=0;
  *(short*)(c+0x519e)=0;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020dc3bc, 0x020dc3bc, size 0x58 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc3bc
extern "C" {
extern int data_ov006_0213a9fc[];

void func_ov006_020dc3bc(char*c){
    extern int RenderOamMainScreen(int a,int b,int c,int d,int e);
  if(*(unsigned char*)(c+0x5000+0x1a2)==0) return;
  int x=*(int*)(c+0x5000+0x194);
  int y=*(int*)(c+0x5000+0x198);
  RenderOamMainScreen(data_ov006_0213a9fc[*(unsigned char*)(c+0x5000+0x1a3)],
    x>>0xc, y>>0xc, -1, -1);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020dc414, 0x020dc414, size 0x9c */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc414
extern "C" {
extern unsigned char data_ov006_0212e324[];
extern unsigned char data_ov006_0212e31c[];

void func_ov006_020dc414(char* base, int idx){
  unsigned short* cnt = (unsigned short*)(base + 0x519e + idx*0x14);
  unsigned char* st = (unsigned char*)(base + 0x51a4 + idx*0x14);
  *cnt = *cnt + 1;
  if (*cnt < data_ov006_0212e324[*st]) return;
  *cnt = 0;
  *st = *st + 1;
  if (*st >= 8) {
    *st = 1;
  } else {
    *(unsigned char*)(base + idx*0x14 + 0x5000 + 0x1a3) = data_ov006_0212e31c[*st];
  }
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020dc4b0, 0x020dc4b0, size 0x18 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc4b0
extern "C" {
void func_ov006_020dc4b0(char* c, int i){
  *(char*)(c + i*0x14 + 0x5000 + 0x1a3) = 0;
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
  int new_var;
  *((unsigned short *) ((c + 0x519e) + n)) = cnt + 1;
  unsigned char idx = *((unsigned char *) ((c + 0x51a4) + n));
  if ((*((unsigned short *) ((c + 0x519e) + n))) >= data_ov006_0212e308[idx])
  {
    *((unsigned short *) ((c + 0x519e) + n)) = 0;
    *((unsigned char *) ((c + 0x51a4) + n)) = (*((unsigned char *) ((c + 0x51a4) + n))) + 1;
    *((unsigned char *) ((c + 0x51a4) + n)) = (*((unsigned char *) ((c + 0x51a4) + n))) & 1;
  }
  *((unsigned char *) ((c + n) + 0x51a3)) = data_ov006_0212e30c[*((unsigned char *) ((c + 0x51a4) + n))];
  if ((*((int *) (c + 0x51c8))) == 5)
  {
    return;
  }
  unsigned short w = *((unsigned short *) ((c + 0x519c) + n));
  if (w != 0)
  {
    *((unsigned short *) ((c + 0x519c) + n)) = (new_var = w - 1);
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
            int *p = (int *)(((int)c + 0x51c8));
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

void func_ov006_020dc6d0(int o, int idx)
{
  unsigned short *timer = (unsigned short *) ((((char *) o) + 0x519c) + (idx * 0x14));
  if ((*timer) != 0)
  {
    *timer = (*timer) - 1;
    if ((*timer) == 0x18)
    {
      _ZN5Sound12PlayBank2_2DEj(0xf1);
    }
    return;
  }
  *((unsigned char *) (((((char *) o) + (idx * 0x14)) + 0x5000) + 0x1a1)) = 1;
  *((unsigned short *) (((((char *) ((unsigned long long) o)) + (idx * 0x14)) + 0x5100) + 0x9e)) = 0;
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

void func_ov006_020dc754(C_c754 *c)
{
    if (c->guard == 0) return;
    int j = c->idx;
    (c->*data_ov006_021417e8[j].pmf[0])(0);
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020dc7b4, 0x020dc7b4, size 0x48 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc7b4
extern "C" {
void func_ov006_020dc7b4(char* c){
  *(char*)(c+0x5000+0x1a0)=1;
  *(char*)(c+0x5000+0x1a1)=0;
  *(char*)(c+0x5000+0x1a2)=1;
  *(short*)(c+0x5000+0x19c)=0x28;
  *(short*)(c+0x5000+0x19e)=0;
  *(char*)(c+0x5000+0x1a3)=3;
  *(char*)(c+0x5000+0x1a4)=0;
  *(int*)(c+0x5000+0x194)=0x70000;
  *(int*)(c+0x5000+0x198)=0x98000;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020dc7fc, 0x020dc7fc, size 0x18 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc7fc
extern "C" {
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

void func_ov006_020dc814(char *c){
  if(*(int*)(c+0x5000+0x1c8) != 5) return;
  int v = *(int*)(c+0x5000+0x1cc);
  if(v > 0x80) return;
  if(v == 0) return;
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
void func_ov006_020dc960(char *c, int i)
{
    unsigned char (*s)[0x18] = (unsigned char (*)[0x18])c;
    unsigned char (*d)[0x10] = (unsigned char (*)[0x10])c;
    *(int *)(&d[i][0x5014]) = *(int *)(&s[i][0x4ac0]);
    *(int *)(&d[i][0x5018]) = *(int *)(&s[i][0x4ac4]);
    *(short *)(&d[i][0x501e]) = *(unsigned char *)(&s[i][0x4ad3]);
    *(unsigned char *)(&d[i][0x5020]) = 1;
    *(short *)(&d[i][0x501c]) = 0x18;
}
}

/* ---------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020dc99c, 0x020dc99c, size 0x68 */
/* ---------------------------------------------------------------- */
// @symbol func_ov006_020dc99c
extern "C" {
extern void *data_ov006_02136e24[];
struct E_c99c { char pad[0x18]; };

void func_ov006_020dc99c(char *c) {
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
void func_ov006_020dca04(char *o)
{
    int i;
    char *q = o;
    for (i = 0; i < 0x20; i++, q += 0x18) {
        if (*(u8 *)(q + 0x4d28) != 0) {
            if (*(u16 *)(q + 0x4d24) != 0) {
                *(u16 *)(((int)q + 0x4d24)) -= 1;
            } else {
                *(u8 *)(o + i * 0x18 + 0x4d28) = 0;
                *(u8 *)(o + i * 0x18 + 0x4d29) = 0;
                return;
            }
            *(int *)(((int)q + 0x4d14)) += *(int *)(q + 0x4d1c);
            *(int *)(((int)q + 0x4d18)) += *(int *)(q + 0x4d20);
            *(int *)(((int)q + 0x4d20)) += 0x180;
            if (*(int *)(q + 0x4d1c) > 0x300)
                *(int *)(((int)q + 0x4d1c)) -= 0x180;
            else if (*(int *)(q + 0x4d1c) < -0x300)
                *(int *)(((int)q + 0x4d1c)) += 0x180;
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

void func_ov006_020dcb1c(char *o, int a1)
{
    int i, j, k;
    char *b = o + a1 * 0x18;
    int *px = (int *)(((int)b + 0x4ac0));
    int *pz = (int *)(((int)b + 0x4ac4));
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
/* func_ov006_020dcc48 at 0x020dcc48
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int data_ov006_021341ec;
extern int data_ov006_0212e364[];

void func_ov006_020dcc48(void) {
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

void func_ov006_020dccb8(char *thiz)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
    int idx;
    if (*(int*)(thiz + 0x5000 + 0x1c8) < 2)
        return;
    idx = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[idx][0x20/4], 0x80, 0x18, -1, -1);
    func_ov004_020b1de8(0x68, 0x28, 1, -1);
    idx = GetGameLanguage();
    DrawOamSprite((void*)data_ov006_0213bf34[idx][4/4], (void*)0x7a, 0x28, (void*)0);
    func_ov004_020b2444(0x8c, 0x28, *(int*)(thiz + 0x5000 + 0x1d4), 1, -1, 2, 0);
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

void func_ov006_020dcd74(char *thiz)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
    int idx;
    int r2;
    if (*(int*)(thiz + 0x5000 + 0x1c8) < 2)
        return;
    idx = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[idx][0x10/4], 0x80, 0x50, -1, -1);
    func_ov004_020b1de8(0x6c, 0x60, 1, -1);
    idx = GetGameLanguage();
    DrawOamSprite((void*)data_ov006_0213bf34[idx][4/4], (void*)0x7e, 0x60, (void*)0);
    if (data_ov004_020beb68 != 0)
        r2 = *(int*)((char*)data_ov004_020beb68 + 0xac);
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
void func_ov006_020dce3c(char* c)
{
    if (*(unsigned char*)(c + 0x4000 + 0xd13) == 0)
        return;
    if (*(unsigned short*)(c + 0x4d08) == *(unsigned short*)(c + 0x4d0a))
        return;
    {
        unsigned short* e = (unsigned short*)(c + 0x4d0c);
        *e = *e + 1;
    }
    if (*(unsigned short*)(c + 0x4d0c) < 8)
        return;
    *(unsigned short*)(c + 0x4d0c) = 0;
    {
        unsigned short* p = (unsigned short*)(c + 0x4d0a);
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

void func_ov006_020dcea8(struct Obj_cea8* o) {
    int i;
    int v1, v2;
    if (o->a == 5 && o->b == 0) return;
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
