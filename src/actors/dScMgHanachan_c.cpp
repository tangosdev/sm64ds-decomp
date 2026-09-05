//cpp
/* dScMgHanachan_c -- the Hanachan (Wiggler) minigame scene, ov006.
 *
 * Reconstructed translation unit: ROM ordinals 13..34 of the contiguous linker
 * run 0x020ea280..0x020ede18, i.e. 0x020eb018..0x020ec458, 22 functions.
 * Assembled by tools/tubuild.py create from the 22 one-function legacy sources,
 * then reconciled by hand (declarations, includes, struct spellings).
 *
 * ------------------------------------------------------------------------
 * WHY THIS IS A SUB-RANGE, AND EXACTLY WHERE THE TWO EDGES COME FROM
 * ------------------------------------------------------------------------
 * tu_map calls 0x020ea280..0x020ede18 one 61-function linker run.  Two separate
 * things stop that whole run from being one licensed TU, and both were measured,
 * not assumed.
 *
 * (1) A SOURCELESS HOLE.  ROM ordinal 11, func_ov006_020ea914 (0x020ea914, size
 *     0x324), has no source anywhere in the tree: it is a banked near-miss
 *     (config/match_attempts.jsonl, best 113 divergences) and carries no entry in
 *     config/arm9/overlays/ov006/delinks.txt at all -- the cartridge's own bytes
 *     cover that range.  It sits in the MIDDLE of the run, and nothing in this
 *     tree can express a .text claim with a hole in it: no delink block and no TU
 *     manifest anywhere in the repository carries two .text runs (both checked
 *     tree-wide).  So the run has to be licensed as one of its two contiguous
 *     sides, and ordinals 0..10 -- which is where the destructor pair lives --
 *     are the smaller one.
 *
 * (2) TWO FILE-GLOBAL PRAGMAS THAT CONTRADICT EACH OTHER.  Of the 49 functions on
 *     the larger side, four only reproduce with `#pragma opt_strength_reduction
 *     off` (func_ov006_020eac38, func_ov006_020ec458, func_ov006_020ec93c,
 *     func_ov006_020ec9c0 -- each carried it in its own legacy file) and three
 *     only reproduce with it ON (func_ov006_020ebf20, func_ov006_020ec2bc,
 *     func_ov006_020ecba4).  A fifth, dScMgHanachan_c::Render, carried
 *     `#pragma opt_common_subs off`, which likewise costs five other members when
 *     it is on.  Both pragmas are FILE-GLOBAL last-wins in mwccarm 2004/b56 --
 *     re-measured here: bracketing opt_strength_reduction `off ... on` around the
 *     four members that want it gives byte-for-byte the same 41/49 as leaving it
 *     out entirely, so there is no positional escape.  One TU therefore cannot
 *     satisfy both groups.
 *
 *     Measured whole-side scores, with every other reconciliation identical:
 *       opt_strength_reduction off, opt_common_subs on   44/49
 *       opt_strength_reduction on,  opt_common_subs on   42/49
 *       opt_strength_reduction off, opt_common_subs off  40/49
 *     None of those is promotable, because promotion needs a CONTIGUOUS run in
 *     which every function matches.  The longest such run in the best of them is
 *     ordinals 12..31 (20 functions); the longest across all three is ordinals
 *     13..34 under strength reduction ON -- 22 functions -- and that is this file.
 *     The remaining 27 functions keep their own shards and their own delinks.txt
 *     entries, so nothing is lost; a later pass that finds the source shape which
 *     reproduces ebf20/ec2bc/ecba4 under `off` extends this run to ordinal 44.
 *
 * NOT A DESTRUCTOR-ORDER REFUSAL.  The cartridge orders this class D1
 * (0x020ea280) BELOW D0 (0x020ea2c8), which is the reproducible direction
 * (inline-in-class => D1 then D0), so the pair would have promoted whole; it is
 * out only because it is on the far side of the sourceless hole.  Because the
 * pair stays out, this TU does not own the class's key function -- the first
 * declared virtual is the out-of-line ~dScMgHanachan_c() -- so it emits no
 * _ZTV/_ZTI/_ZTS, needs no compiler_only_output block, and needed no header
 * edit: include/dScMgHanachan_c.h keeps the out-of-line destructor declaration
 * that the two still-enrolled destructor shards require.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order, so the
 * highest-address ROM function is written FIRST.  Do not reorder.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [13] 0x020eb018  src/func_ov006_020eb018.cpp
 *   [14] 0x020eb0c8  src/func_ov006_020eb0c8.cpp
 *   [15] 0x020eb1e0  src/func_ov006_020eb1e0.c
 *   [16] 0x020eb31c  src/func_ov006_020eb31c.cpp
 *   [17] 0x020eb3e4  src/func_ov006_020eb3e4.c
 *   [18] 0x020eb558  src/func_ov006_020eb558.cpp
 *   [19] 0x020eb610  src/func_ov006_020eb610.cpp
 *   [20] 0x020eb768  src/func_ov006_020eb768.c
 *   [21] 0x020eb7b0  src/func_ov006_020eb7b0.c
 *   [22] 0x020eb7f8  src/func_ov006_020eb7f8.c
 *   [23] 0x020eb8f0  src/func_ov006_020eb8f0.cpp
 *   [24] 0x020eb9b0  src/func_ov006_020eb9b0.c
 *   [25] 0x020eb9dc  src/func_ov006_020eb9dc.c
 *   [26] 0x020ebb40  src/func_ov006_020ebb40.c
 *   [27] 0x020ebc08  src/func_ov006_020ebc08.c
 *   [28] 0x020ebc7c  src/func_ov006_020ebc7c.c
 *   [29] 0x020ebd7c  src/func_ov006_020ebd7c.c
 *   [30] 0x020ebe6c  src/func_ov006_020ebe6c.c
 *   [31] 0x020ebeb4  src/func_ov006_020ebeb4.c
 *   [32] 0x020ebf20  src/func_ov006_020ebf20.c
 *   [33] 0x020ec134  src/func_ov006_020ec134.c
 *   [34] 0x020ec2bc  src/func_ov006_020ec2bc.c
 */

/* #pragma push / O3 / pop bracket func_ov006_020eb018 below, where its own
   legacy file had them; unlike opt_strength_reduction, push/pop IS positional.
   No file-global pragma is set here -- see the boundary note above for why. */

#include "types.h"
#include "private/ov006_ec4dc_obj.h"
#include "dScMgHanachan_c.h"
#include "decl_common.h"

/* ---------------------------------------------------------------------------
 * Shadow types.  These are the legacy files' own local views of objects whose
 * real types have not been recovered; they are reconciled here into one set,
 * and every merge below is offset-proved, not guessed.
 * ------------------------------------------------------------------------- */

typedef struct V2 { int x, y; } V2;
typedef struct { int x, y; } Vec2;
typedef struct P2 { int v[2]; } P2;   /* array spelling: see struct Pair */
typedef struct S { int a; int b; } S;
typedef struct { int v[2]; } P;   /* array spelling: see struct Pair */
/* Spelled as a 2-int ARRAY, not two named ints: the whole-struct copies below
   (func_ov006_020ed81c and friends) come out ldr/ldr/str/str with the array and
   ldr/str/ldr/str with the named fields, which is a byte difference. */
struct Pair { int v[2]; };
/* A two-word blob copy.  The member is an ARRAY on purpose: C++ scalarizes an
   assignment between two structs of named ints word-by-word (ldr/str/ldr/str)
   where C block-moves it (ldr/ldr/str/str), and these functions were C.  Every
   2-int shadow type in this file that is whole-struct-assigned anywhere is
   spelled the same way for the same reason -- Pair, P, P2, W2, Words2. */
struct Words2 { int v[2]; };
struct E { int a, b; };
struct W2 { int v[2]; };   /* array spelling: see struct Pair */

struct B72 {
    unsigned short f0 : 2;
    unsigned short f1 : 2;
    unsigned short f2 : 2;
    unsigned short f3 : 2;
};

/* The 0x98-byte element of the class's own 15-entry array at +0x4678.  Six
   legacy files each described a different window onto it; this is their union,
   and every one of them agrees offset-for-offset:
     func_ov006_020ecba4  the widest view, used verbatim below
     func_ov006_020ebf20  v18[5]@0x18, w4c[5]@0x4c, s76@0x76, s7a[5]@0x7a, b94@0x94
     func_ov006_020ec134  v18[5]@0x18, w4c[5]@0x4c, s7a[5]@0x7a
     func_ov006_020ec2bc  same as 020ec134
     func_ov006_020ec458  spelled entries[5]@0x18 (int val/unk4 == V2 x/y) and
                          unk76@0x76 -- identical layout, renamed here
     func_ov006_020ec93c  same as 020ec458
     func_ov006_020ecdb8  spelled arr[5]@0x4c == w4c[5]
   None of them does pointer arithmetic on the type (the 0x98 stride is always
   spelled as a raw byte step), so the differing trailing sizes cost nothing. */
struct C {
    V2 v0;              /* 0x00 */
    V2 v8;              /* 0x08 */
    char pad10[8];      /* 0x10 */
    V2 v18[5];          /* 0x18 */
    int w40;            /* 0x40 */
    int w44;            /* 0x44 */
    int w48;            /* 0x48 */
    int w4c[5];         /* 0x4c */
    char pad60[0x16];   /* 0x60 */
    s16 s76;            /* 0x76 */
    char pad78[2];      /* 0x78 */
    s16 s7a[5];         /* 0x7a */
    char pad84[2];      /* 0x84 */
    s16 s86;            /* 0x86 */
    char pad88[0xc];    /* 0x88 */
    unsigned char b94;  /* 0x94 */
    unsigned char b95;  /* 0x95 */
};

/* A second view of the same 0x98 element, from func_ov006_020eb9dc.  It is kept
   apart from struct C because it names 0x70/0x8c/0x90/0x93 that C spells as
   padding, and it is NOT called Obj because include/private/ov006_ec4dc_obj.h
   already defines a real Obj for this same object (a third window onto it,
   the one func_ov006_020ec4dc uses). */
struct ObjB {
    P2 m0;                          /* 0x00 */
    P2 m8;                          /* 0x08 */
    P2 m10;                         /* 0x10 */
    char pad18[0x40 - 0x18];
    int w40;                        /* 0x40 */
    char pad44[0x70 - 0x44];
    int w70;                        /* 0x70 */
    char pad74[0x78 - 0x74];
    short h78;                      /* 0x78 */
    char pad7a[0x8c - 0x7a];
    short h8c;                      /* 0x8c */
    short pad8e;
    short h90;                      /* 0x90 */
    unsigned char pad92;
    unsigned char b93;              /* 0x93 */
};

/* A third view, from func_ov006_020ed494, which needs the 0x98 stride as a type
   because it walks the array with a real pointer. */
typedef struct Thing
{
    int a;              /* 0x00 */
    int b;              /* 0x04 */
    char pad0[0x10];    /* 0x08 */
    V2 pos;             /* 0x18 */
    char pad1[0x78];    /* 0x20 */
} Thing;                /* 0x98 */

/* The pointer-to-member-function view of the element used by
   func_ov006_020eb018, func_ov006_020eb31c and dScMgHanachan_c::Behavior: the
   word at 0x10 (Behavior reaches the one at +0x4660 instead) is called through
   as a PMF, which is the only reason these need a class type at all. */
struct CB {
    char pad10[0x10];
    void (CB::*pmf)();  /* 0x10 */
};
typedef void (CB::*PMF)();

/* func_ov006_020eb0c8's own spelling of the same idiom, kept separate because
   it names the member `cb`. */
struct Self;
typedef void (Self::*Pmf)();
struct Self {
    char pad10[0x10];
    Pmf cb;
};

/* dScMgHanachan_c::InitResources reaches slot 18 polymorphically through this
   shim.  Slot 18 is a declared method on the class now, but the legacy source
   dispatched through a 19-slot shim and that is the dispatch the ROM matches.
   Nothing else in the TU uses it. */
struct SelfV {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(int a);
    char pad[0xa4];
    int unkA8;
    int unkAC;
};

#define L(a) (*(int*)(a))
#define M(a) (*(int*)(a))
#define PAIR0(s) ((Pair *)(int)((char *)(s) + 0x4660))
#define PAIR1(s) ((Pair *)(int)((char *)(s) + 0x4660))

/* ---------------------------------------------------------------------------
 * ROM symbols this TU calls or reads.  Declarations that decl_common.h already
 * carries are NOT repeated here -- a real header always wins, and repeating a
 * disagreeing spelling is `illegal function overloading` pointing at the
 * definition rather than at the header.  Where the legacy files disagreed among
 * themselves, one spelling is chosen here and the odd call site is cast; every
 * such difference was pointer-vs-pointer or a return type nobody used, so none
 * of them changes an instruction.
 * ------------------------------------------------------------------------- */
/* ApproachLinear is spelled unmangled in the ROM's symbol table as
   _Z14ApproachLinearRiii, so it must be declared OUTSIDE the extern "C" block
   below: inside it, C linkage strips the mangling and the module link fails with
   `Undefined : "ApproachLinear"`.  func_ov006_020eb610's legacy file had it
   outside for the same reason. */
extern int ApproachLinear(int &, int, int);

extern "C" {
/* Forward declarations of this TU's own members.  Source order is the
   REVERSE of ROM order, so almost every call here is to a function defined
   further down the file. */
extern void func_ov006_020eac38(void* o_);
extern void func_ov006_020eb018(char *c);
extern void func_ov006_020eb0c8(char *self);
extern void func_ov006_020eb1e0(char *c);
extern void func_ov006_020eb31c(char* c);
extern void func_ov006_020eb3e4(char *c);
extern void func_ov006_020eb558(char *thiz_);
extern void func_ov006_020eb610(char *c);
extern int func_ov006_020eb768(char* a, const V2* b);
extern int func_ov006_020eb7b0(char* p);
extern void func_ov006_020eb7f8(char* c);
extern void func_ov006_020eb8f0(char *c);
extern void func_ov006_020eb9b0(char *p);
extern void func_ov006_020eb9dc(struct ObjB* c, int v);
extern int func_ov006_020ebb40(char *o, int);
extern int func_ov006_020ebc08(char* c, int n);
extern int func_ov006_020ebc7c(int n, int mask);
extern void func_ov006_020ebd7c(int count);
extern int func_ov006_020ebe6c(void);
extern int func_ov006_020ebeb4(char *c);
extern void func_ov006_020ebf20(struct C* c);
extern void func_ov006_020ec134(struct C* c);
extern void func_ov006_020ec2bc(struct C* c);
extern void func_ov006_020ec458(C* self);
extern void func_ov006_020ec4dc(Obj *self, int n);
extern void func_ov006_020ec6e8(char *c);
extern void func_ov006_020ec84c(char *c, int);
extern void func_ov006_020ec93c(C* self);
extern void func_ov006_020ec9c0(char* self, int idx);
extern void func_ov006_020ecb80(void *a);
extern void func_ov006_020ecba4(struct C* c, int param);
extern void func_ov006_020ecdb8(char *self, int arg1, int arg2);
extern void func_ov006_020ed270(void);
extern void func_ov006_020ed274(char *c);
extern void func_ov006_020ed300(char *p);
extern void func_ov006_020ed328(void);
extern void func_ov006_020ed32c(char *p);
extern void func_ov006_020ed34c(char *p);
extern void func_ov006_020ed40c(char *c);
extern void func_ov006_020ed494(char *c);
extern void func_ov006_020ed81c(char *p);
extern void func_ov006_020ed844(char *c);
extern void func_ov006_020ed8a4(void *r4_);
extern void func_ov006_020edcb0(void);

/* ROM symbols this TU calls or reads.  Anything decl_common.h already
   declares is deliberately NOT repeated -- a real header always wins, and a
   disagreeing local spelling is `illegal function overloading` pointed at the
   definition rather than at the header. */
extern short data_02082214[];
extern void* data_ov006_02142018[];
extern void* data_ov006_021375c0[];
extern int data_ov006_0213c97c[];
extern void* data_ov006_021375a0[];
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int b, void* attr, int x, int y, int a, int c, int f, int g);
extern int data_ov006_0213c984[];
extern int data_ov006_0213ca44[];
extern int data_ov006_0213ca1c;
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
extern P data_ov006_0213c9b4;
extern P data_ov006_0213c9a4;
extern P data_ov006_0213c99c;
extern P data_ov006_0213c9fc;
extern Pair data_ov006_0213c96c;
extern Pair data_ov006_0213c9c4;
extern Pair data_ov006_0213c9bc;
extern struct P2 data_ov006_0213c9f4;
extern struct P2 data_ov006_0213c9ec;
extern struct P2 data_ov006_0213c9e4;
extern struct P2 data_ov006_0213c974;
extern struct P2 data_ov006_0213c95c;
extern int data_ov006_0213ca04[];
extern int data_ov006_0213c9d4[];
extern short data_ov006_02141fd0;
extern unsigned short data_ov006_02141fdc[];
extern unsigned short data_ov006_02141ff8[];
extern void _Z11UpdateAngleRssis(s16* p, s16 tgt, int step, s16 amt);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern Vec2i data_ov006_0213ca24;
extern struct Words2 data_ov006_0213ca14;
extern int data_ov006_0213ca0c[];
extern V2 data_ov006_0213c9dc;
extern void func_ov004_020ad90c(void *);
extern void func_ov004_020b1e34(void *a, int b, int c, int d);
extern int GetGameLanguage(void);
extern void DrawOamSprite(void *a, int b, int c, int d);
extern void func_ov004_020b2220(int a, int b, void *c, int d, int e, int f, int g);
extern Pair data_ov006_0213c9ac;
extern Pair data_ov006_0213c994;
extern Pair data_ov006_0213ca3c;
extern Pair data_ov006_0213ca34;
extern Pair data_ov006_0213ca2c;
extern int data_ov006_0213ca54[];
extern struct Pair data_ov006_0213ca5c;
extern struct Pair data_ov006_0213ca64;
extern int data_ov006_0213ca6c[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int data_ov006_0213ca4c[];
extern int data_ov006_0213ca74[];
extern void func_02012790(int a);
extern void func_ov006_020ea5f0(int x, int y);
extern void func_ov006_020ea81c(int x, int y);
extern struct Pair data_ov006_0213c9cc;
extern s16 data_ov006_0212e80c[];
extern volatile int data_ov006_0212e820[];
extern Pair data_ov006_0213c964;
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern u8 data_0209d45c;
extern u8 data_0209d454;

/* func_0203d5dc: func_ov006_020eb768 typed both arguments `const Vector3*` and
   then stepped one of them by 8 bytes, so they are V2s; func_ov006_020ed494
   typed the first `int*`.  Pointer either way -- one spelling, a cast at the
   odd call site. */
extern int func_0203d5dc(const V2 *a, const V2 *b);
/* Four files spelled func_0203d704's arguments V2* and two spelled them int*;
   decl_common.h does not declare it, so V2* wins and the int* callers cast. */
extern void func_0203d704(V2 *o, V2 *a, V2 *b);
extern void Vec2_Sub(V2 *o, V2 *a, V2 *b);
extern void func_0203d680(Vec2 *out, const Vec2 *in, int scale);
/* _Z15ApproachLinear2Rsss took `s16&` in one file and `short*` in another, and
   _Z14ApproachLinearRiii took `int&` in one and `int*` elsewhere.  A reference
   IS a pointer here, so the pointer spelling wins and the reference call sites
   take an `&`. */
extern int _Z15ApproachLinear2Rsss(short *v, short target, short step);
/* _Z14ApproachLinearRiii: one file typed the result int and used it, the rest
   typed it void and ignored it.  int wins; ignoring a result costs nothing. */
extern int _Z14ApproachLinearRiii(int *v, int target, int step);
extern void _Z14ApproachLinearRsss(short *v, short target, short step);
extern int _ZN4cstd4fdivEii(int numerator, int denominator);
extern int RandomIntInternal(int *seed);
/* data_0209e650 is the RNG state: one file spelled it int[] and passed it bare,
   the rest spelled it int and passed &.  Scalar wins; the address is the same. */
extern int data_0209e650;
/* data_ov006_02141fd4 is only read where it is unsigned and only written as a
   halfword, so the unsigned spelling costs the writer nothing. */
extern unsigned short data_ov006_02141fd4;
/* data_ov006_02141fd8: func_ov006_020ed8a4 spelled it volatile and only ever
   stores to it (never re-reads), so the plain spelling emits the same strh. */
extern s16 data_ov006_02141fd8;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0e40[];
/* LoadFile returns the buffer; one file typed it int and casts at each use. */
extern void *LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov006_020ec2bc, 0x020ec2bc, size 0x19c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ec2bc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ec2bc(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i = 2;
    V2* p = &c->v18[i];
    int j;

    for (; i < 5; i++) {
        j = i - 1;
        Vec2_Sub(&d, p, &c->v18[j]);
        if (func_0203d434((int *)&d) != 0) {
            s16 a, prev, na, d2, diff, cur;
            func_0203d630((int *)&d, (i * 2 + 0x10) << 12);
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->s7a[j];
            d2 = 0;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            c->s7a[i] = na;
            cur = c->s7a[i];
            func_0203d388((int *)&d, (s16)(d2 - (na - cur)));
        }
        func_0203d704(&e, &c->v18[j], &d);
        c->v18[i].x = e.x;
        c->v18[i].y = e.y;
        p++;
    }
    g.x = 0;
    g.y = -0x10000;
    c->s7a[0] = (s16)(c->s7a[1] + (((c->s7a[1] - c->s7a[2]) << 14) >> 16));
    func_0203d388((int *)&g, c->s7a[0]);
    func_0203d704(&h, (V2*)(c->v18) + 1, &g);
    c->v18[0].x = h.x;
    c->v18[0].y = h.y;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov006_020ec134, 0x020ec134, size 0x188 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ec134
/* func_ov006_020ec134 at 0x020ec134 (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ec134(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i = 4;
    V2* p = &c->v18[i];
    int m = 0x8000;
    s16 zero = 0;
    int j;

    for (; i >= 1; i--) {
        j = i - 1;
        Vec2_Sub(&d, p, &c->v18[j]);
        if (func_0203d434((int *)&d) != 0) {
            s16 a, prev, na, d2, diff, cur;
            { int w = c->w4c[i]; func_0203d630((int *)&d, w + m); }
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->s7a[j];
            d2 = zero;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            cur = c->s7a[i];
            func_0203d388((int *)&d, (s16)(d2 - (na - cur)));
        }
        Vec2_Sub(&e, p, &d);
        c->v18[j].x = e.x;
        c->v18[j].y = e.y;
        p--;
        m -= 0x2000;
    }
    g.x = 0;
    g.y = -0x10000;
    func_0203d388((int *)&g, c->s7a[0]);
    func_0203d704(&h, (V2*)(c->v18) + 1, &g);
    c->v18[0].x = h.x;
    c->v18[0].y = h.y;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_020ebf20, 0x020ebf20, size 0x214 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ebf20
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ebf20(struct C* c)
{
    V2 d;
    V2 g;
    V2 e;
    V2 h;
    int i;
    s16 na;
    int j;
    s16 d2;
    int cur;
    V2* pj;
    int zero;
    int step;
    int amt;
    V2* vb2;

    if (c->b94 != 0)
        return;

    _Z11UpdateAngleRssis(&c->s7a[1], c->s76, 8, 0x100);

    i = 2;
    step = 2;
    zero = 0;
    amt = 0x100;

    for (; i < 5; i++) {
        j = i - 1;
        pj = &c->v18[j];
        Vec2_Sub(&d, &c->v18[i], pj);
        if (func_0203d434((int *)&d) != 0) {
            s16 a, prev, diff;
            func_0203d630((int *)&d, c->w4c[i] + i * 0x2000);
            a = _ZN4cstd5atan2E5Fix12IiES1_(d.x, d.y);
            prev = c->s7a[j];
            d2 = zero;
            na = (s16)(-a);
            diff = (s16)(prev - (s16)(-a));
            if (diff > 0x2000) {
                d2 = (s16)(diff - 0x2000);
                na = (s16)(prev - 0x2000);
            } else if (diff < -0x2000) {
                d2 = (s16)(diff + 0x2000);
                na = (s16)(prev + 0x2000);
            }
            _Z11UpdateAngleRssis(&c->s7a[i], na, step, amt);
            cur = c->s7a[i];
            cur = na - cur;
            func_0203d388((int *)&d, (s16)(d2 - cur));
        }
        func_0203d704(&e, pj, &d);
        c->v18[i].x = e.x;
        c->v18[i].y = e.y;
    }

    {
        s16 dd = (s16)(c->s76 - c->s7a[1]);
        g.x = 0;
        g.y = -0x10000;
        _Z11UpdateAngleRssis(&c->s7a[0], (s16)(c->s7a[1] + dd), 8, 0x100);
    }
    func_0203d388((int *)&g, c->s7a[0]);
    vb2 = c->v18;
    func_0203d704(&h, vb2 + 1, &g);
    c->v18[0].x = h.x;
    c->v18[0].y = h.y;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_020ebeb4, 0x020ebeb4, size 0x6c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ebeb4
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020ebeb4(char *c){
    int r=0;
    int v=*(int*)(c+0x20);
    if(v>=0xe8000){ r|=1; v=0xe8000; *(int*)(c+0x20)=v; r&=0xff; }
    else if(v<=0x18000){ r|=2; v=0x18000; *(int*)(c+0x20)=v; r&=0xff; }
    v=*(int*)(c+0x24);
    if(v>=0xa8000){ r|=8; v=0xa8000; *(int*)(c+0x24)=v; r&=0xff; }
    else if(v<=0x18000){ r|=4; v=0x18000; *(int*)(c+0x24)=v; r&=0xff; }
    return r;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_020ebe6c, 0x020ebe6c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ebe6c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020ebe6c(void){
  int v = 3;
  int r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
  if (r < 0x400) v = 0;
  else if (r < 0x800) v = 1;
  else if (r < 0xc00) v = 2;
  return v;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_020ebd7c, 0x020ebd7c, size 0xf0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ebd7c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ebd7c(int count)
{
    int i, j;
    data_ov006_02141fd4 = (short)(RandomIntInternal(&data_0209e650) & 0xf);
    data_ov006_02141fd0 = (short)count;
    for (i = 0; i < count; i++) {
        int r = RandomIntInternal(&data_0209e650);
        data_ov006_02141fdc[i] = (unsigned short)((int)((int)((unsigned int)(r & 0x7fffffff) >> 0x13) * data_ov006_0213c958) >> 0xc);
        j = 0;
        while (j < i) {
            if (data_ov006_02141fdc[j] == data_ov006_02141fdc[i]) {
                r = RandomIntInternal(&data_0209e650);
                data_ov006_02141fdc[i] = (unsigned short)((int)((int)((unsigned int)(r & 0x7fffffff) >> 0x13) * data_ov006_0213c958) >> 0xc);
                j = 0;
            } else {
                j++;
            }
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_020ebc7c, 0x020ebc7c, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ebc7c
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020ebc7c(int n, int mask)
{
    int v;
    int i;
    int found = 0;

    for (i = 0; i < data_ov006_02141fd0; i++) {
        if (n == data_ov006_02141fdc[i]) {
            found = 1;
            break;
        }
    }

    v = (unsigned short)(mask & RandomIntInternal(&data_0209e650));
    if (found) v = data_ov006_02141fd4 | (unsigned short)(v & 0xf0);

    for (i = 0; i < n; i++) {
        if (v == data_ov006_02141ff8[i]) {
            v = (unsigned short)(mask & RandomIntInternal(&data_0209e650));
            if (found) v = data_ov006_02141fd4 | (unsigned short)(v & 0xf0);
            i = -1;
        }
    }

    data_ov006_02141ff8[n] = v;
    return v;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_020ebc08, 0x020ebc08, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ebc08
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020ebc08(char* c, int n) {
    switch (n) {
    case 0:
        return *(unsigned char*)(c + 0x95);
    case 1:
        return ((struct B72*)(c + 0x74))->f0 + 7;
    case 2:
        return ((struct B72*)(c + 0x74))->f1 + 7;
    case 3:
        return ((struct B72*)(c + 0x74))->f2 + 7;
    default:
        return ((struct B72*)(c + 0x74))->f3 + 7;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020ebb40, 0x020ebb40, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020ebb40
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* Second parameter unused by the body -- func_ov006_020eac38 passes an index
   this callee ignores; declaring it keeps the caller's argument setup. */
int func_ov006_020ebb40(char *o, int){
  int r2 = 0;
  if (*(unsigned char*)(o + 0x93) != 0){
    int r1 = *(short*)(o + 0x8e);
    if (r1 > 0x20){ r2 = 1; goto end; }
    if (r1 > 0x10){ *(unsigned char*)(o + 0x95) = 3; r2 = 5; goto end; }
    if (r1 > 0) r2 = 4;
    goto end;
  }
  {
    int *ref = data_ov006_0213ca04;
    int ip = *(int*)o;
    if (ip == ref[0]){
      if (*(int*)(o + 4) == ref[1]) goto C;
      if (ip == 0) goto C;
    }
    ref = data_ov006_0213c9d4;
    if (ip != ref[0]) goto end0;
    if (*(int*)(o + 4) == ref[1]) goto C;
    if (ip != 0) goto end0;
  }
C:
  {
    int v = *(short*)(o + 0x90);
    if (v < 0x10){ r2 = 5; goto end; }
    if (v < 0x20) r2 = 4;
    else r2 = 0;
    goto end;
  }
end0:
  r2 = 0;
end:
  return r2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020eb9dc, 0x020eb9dc, size 0x164 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb9dc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eb9dc(struct ObjB* c, int v)
{
    short* h = (short*)(int)(&c->h8c);
    struct P2* p = (struct P2*)(int)(&c->m8);
    struct P2* g = (struct P2*)(int)(&data_ov006_0213c9f4);

    (*h)++;
    if (p->v[0] == g->v[0] &&
        (p->v[1] == g->v[1] || c->m8.v[0] == 0) &&
        c->h8c < 3) {
        c->b93 = 0;
        c->h78 = (RandomIntInternal(&data_0209e650) & 0x20) + 0x60;
        if (c->h8c == 1) {
            c->m10 = data_ov006_0213c9ec;
            c->m8 = c->m10;
        } else {
            c->m10 = data_ov006_0213c9e4;
            c->m8 = c->m10;
        }
        if (c->w70 == 0) {
            _ZN5Sound12PlayBank2_2DEj(0x1e9);
        }
        c->m0 = data_ov006_0213c974;
    } else {
        c->w40 = 0;
        c->h90 = (v >> 12) / 4 + 0x31;
        c->m10 = c->m8;
        c->m0 = data_ov006_0213c95c;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020eb9b0, 0x020eb9b0, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb9b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eb9b0(char *p)
{
    *(int *)(p + 0x44) = 2560;
    *(short *)(p + 0x86) = 1536;
    *(short *)(p + 0x8e) = 0;
    *(char *)(p + 0x93) = 0;
    *(char *)(p + 0x94) = 0;
    *(char *)(p + 0x95) = 2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020eb8f0, 0x020eb8f0, size 0xc0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb8f0
extern "C" void func_ov006_020eb8f0(char *c)
{
    Pair *g = &data_ov006_0213c96c;
    Pair *p = (Pair *)c;
    if (p->v[0] == g->v[0] && (p->v[1] == g->v[1] || p->v[0] == 0)) {
        *(unsigned char *)(c + 0x93) = 0;
        *(unsigned short *)(c + 0x78) = 0x60;
        if (*(int *)(c + 0x70) == 0)
            _ZN5Sound12PlayBank2_2DEj(0x1e9);
        *p = data_ov006_0213c9c4;
    } else {
        *(unsigned char *)(c + 0x95) = 5;
        *(int *)(c + 0x60) = 0;
        *(Pair *)(c + 0x10) = *(Pair *)(c + 8);
        *p = data_ov006_0213c9bc;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020eb7f8, 0x020eb7f8, size 0xf8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb7f8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eb7f8(char* c){
  P* d = &data_ov006_0213c9b4;
  int c0 = *(int*)c;
  if(c0 == d->v[0] && (*(int*)(c+4) == d->v[1] || c0 == 0)){
    *(unsigned char*)(c+0x93) = 0;
    *(short*)(c+0x78) = 0x60;
    if(*(int*)(c+0x70) == 0){
      _ZN5Sound12PlayBank2_2DEj(0x1e9);
    }
    *(P*)c = data_ov006_0213c99c;
  }else{
    P* d2 = &data_ov006_0213c9a4;
    if(c0 != d2->v[0] || (*(int*)(c+4) != d2->v[1] && c0 != 0)){
      *(short*)(c+0x90) = 0x31;
      *(P*)(c+0x10) = *(P*)(c+8);
      _ZN5Sound12PlayBank2_2DEj(0x1ed);
    }
    *(int*)(c+0x60) = 0;
    *(P*)c = data_ov006_0213c9fc;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020eb7b0, 0x020eb7b0, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb7b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020eb7b0(char* p)
{
    if (*(unsigned char*)(p + 0x93) == 0) {
        S* q = (S*)(long long)(p + 8);
        int x = *(int*)p;
        if (x == q->a && (*(int*)(p + 4) == q->b || x == 0))
            return 1;
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020eb768, 0x020eb768, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb768
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020eb768(char* a, const V2* b){
  int i;
  const V2* p = (const V2*)(a + 0x18);
  for (i = 0; i < 5; i++){
    if (func_0203d5dc(p, b) < 0x14000) return 1;
    p = (const V2*)((char*)p + 8);
  }
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020eb610, 0x020eb610, size 0x158 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb610
extern "C" void func_ov006_020eb610(char *c)
{
    int v[2];
    int step;
    int v93;

    if (*(unsigned char *)(c + 0x92) != 0) {
        step = 0x1000;
        if (*(int *)(c + 0x40) < 0xa00) step = step >> 2;
        ApproachLinear(*(int *)(c + 0x40), *(int *)(c + 0x44), step);
        v[1] = -*(int *)(c + 0x40);
        v[0] = 0;
        func_0203d388(v, *(short *)(c + 0x7c));
        L(c + 0x20) += v[0];
        M(c + 0x24) += v[1];
        if (*(unsigned char *)(c + 0x93) != 0) {
            *(short *)(long long)(int)(c + 0x8e) = *(short *)(long long)(int)(c + 0x8e) - 1;
            if (*(short *)(c + 0x8e) == 0) {
                func_ov006_020eb9b0(c);
            }
        }
    }
    if (*(int *)(c + 0x6c) != 0 && *(int *)(c + 0x70) == 0) {
        v93 = 0;
        if (*(unsigned char *)(c + 0x93) != 0) v93 = 0x1f4;
        *(int *)(c + 0x68) = func_02012468(*(int *)(c + 0x68), 2,
            *(int *)(c + 0x6c), 2, 0, v93, 0, 0);
    } else {
        *(int *)(c + 0x68) = 0;
    }
    if (*(int *)c != 0) {
        int off = *(int *)(c + 4);
        char *base = c + (off >> 1);
        void (*fn)(char *);
        if (off & 1)
            fn = *(void (**)(char *))(*(char **)base + *(int *)c);
        else
            fn = *(void (**)(char *))c;
        fn(base);
    }
    if (*(unsigned char *)(c + 0x92) == 0) return;
    func_ov006_020ebf20((C *)c);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020eb558, 0x020eb558, size 0xb8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb558
extern "C" /* char* parameter to agree with include/decl_common.h -- see func_ov006_020eac38. */
void func_ov006_020eb558(char *thiz_)
{
    int *thiz = (int *)thiz_;
    int *g = &data_ov006_0213ca1c;
    int v0 = thiz[0];
    if (v0 == g[0] && (thiz[1] == g[1] || v0 == 0) && thiz[0x70/4] == 0) {
        short h = *(short*)((char*)thiz + 0x84);
        if (h < 0 && (short)(h + *(short*)((char*)thiz + 0x86)) > 0) {
            _ZN5Sound12PlayBank2_2DEj(0x1e8);
        } else if (h > 0 && (short)(h + *(short*)((char*)thiz + 0x86)) < 0) {
            _ZN5Sound12PlayBank2_2DEj(0x1e7);
        }
    }
    {
        short *p = (short*)((int)thiz + 0x84);
        *p = *p + *(short*)((char*)thiz + 0x86);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020eb3e4, 0x020eb3e4, size 0x174 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb3e4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eb3e4(char *c)
{
    int va[2];
    int vb[2];
    short tgt;
    short cnt;

    _Z14ApproachLinearRiii((int *)(c + 0x48), 0, 0x200);
    *(short *)(((int)c + 0x78)) -= 1;
    if (*(short *)(c + 0x78) == 0) {
        int *p10 = (int *)(((int)c + 0x10));
        int *pd = (int *)(((int)data_ov006_0213ca44));
        if (p10[0] == pd[0] &&
            (p10[1] == pd[1] || *(int *)(c + 0x10) == 0)) {
            *(unsigned char *)(c + 0x94) = 1;
            *(int *)(c + 0x48) = 0x3000;
            *(unsigned char *)(c + 0x93) = 0;
            *(unsigned char *)(c + 0x95) = 0;
        } else {
            *(unsigned char *)(c + 0x94) = 0;
            *(unsigned char *)(c + 0x93) = 0;
            *(int *)(c + 0x44) = 0xa00;
            *(unsigned short *)(c + 0x86) = 0x600;
            *(unsigned char *)(c + 0x95) = 2;
            *(int *)(c + 0x68) = 0;
            *(int *)(c + 0x6c) = 0x1ec;
        }
        *(P2 *)c = *(P2 *)(c + 0x10);
        return;
    }
    {
        int z = 0;
        int n = 0x10000;
        n = -n;
        va[0] = z;
        va[1] = n;
    }
    tgt = *(short *)(c + 0x76) - 0x1000;
    _Z14ApproachLinearRsss((short *)(c + 0x7a), tgt, 0x400);
    func_0203d388(va, *(short *)(c + 0x7a));
    func_0203d704((V2 *)vb, (V2 *)(c + 0x20), (V2 *)va);
    *(int *)(c + 0x18) = vb[0];
    *(int *)(c + 0x1c) = vb[1];
    cnt = *(short *)(c + 0x78);
    if ((cnt & 0x10) == 0 || cnt < 0x38)
        *(unsigned char *)(c + 0x95) = 1;
    else
        *(unsigned char *)(c + 0x95) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020eb31c, 0x020eb31c, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb31c
extern "C" void func_ov006_020eb31c(char* c)
{
    short* t = (short*)(((int)c + 0x90));
    short v = (short)(*t - 1);
    *t = v;
    if (*(short*)(c+0x90) == 0) {
        *(unsigned char*)(c+0x94) = 0;
        *(unsigned char*)(c+0x93) = 1;
        *(int*)(c+0x44) = 0x1000;
        *(short*)(c+0x86) = 0x900;
        *(unsigned char*)(c+0x95) = 4;
        *(short*)(c+0x8e) = 0x78;
        {
            int a = *(int*)(c+0x10);
            int b = *(int*)(c+0x14);
            *(int*)c = b ? a : a;
            *(int*)(c+4) = b;
        }
        return;
    }
    CB* o = (CB*)c;
    (o->*(o->pmf))();
    if (*(short*)(c+0x90) != 0x30) return;
    if (*(int*)(c+0x70) == 0)
        _ZN5Sound12PlayBank2_2DEj(0x1ea);
    *(unsigned char*)(c+0x95) = 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020eb1e0, 0x020eb1e0, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb1e0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020eb1e0(char *c)
{
    int va[2];
    int vb[2];
    short tgt;

    _Z14ApproachLinearRiii((int *)(c + 0x48), 0, 0x200);
    if (_Z15ApproachLinear2Rsss((short *)(c + 0x78), 0, 1) != 0) {
        int *p10 = (int *)(((int)c + 0x10));
        int *pd = (int *)(((int)data_ov006_0213c984));
        if (p10[0] == pd[0] &&
            (p10[1] == pd[1] || *(int *)(c + 0x10) == 0)) {
            *(unsigned char *)(c + 0x94) = 1;
            *(int *)(c + 0x48) = 0x3000;
            *(unsigned char *)(c + 0x93) = 0;
            *(unsigned char *)(c + 0x95) = 0;
        }
        return;
    }
    {
        int z = 0;
        int n = 0x10000;
        n = -n;
        va[0] = z;
        va[1] = n;
    }
    tgt = *(short *)(c + 0x76) - 0x1000;
    _Z14ApproachLinearRsss((short *)(c + 0x7a), tgt, 0x400);
    func_0203d388(va, *(short *)(c + 0x7a));
    func_0203d704((V2 *)vb, (V2 *)(c + 0x20), (V2 *)va);
    *(int *)(c + 0x18) = vb[0];
    *(int *)(c + 0x1c) = vb[1];
    {
        short cnt = *(short *)(c + 0x78);
        if ((cnt & 0x10) == 0 || cnt < 0x38)
            *(unsigned char *)(c + 0x95) = 1;
        else
            *(unsigned char *)(c + 0x95) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020eb0c8, 0x020eb0c8, size 0x118 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb0c8
extern "C" void func_ov006_020eb0c8(char *self) {
    if (_Z15ApproachLinear2Rsss((s16 *)(self + 0x90), 0, 1)) {
        *(u8 *)(self + 0x94) = 0;
        *(u8 *)(self + 0x93) = 1;
        *(s16 *)(self + 0x8e) = 0x21;
        *(s32 *)(self + 0x44) = 0x1000;
        *(s16 *)(self + 0x86) = 0x900;
        *(u8 *)(self + 0x95) = 6;
    } else {
        (((Self *)self)->*(((Self *)self)->cb))();
        if (*(s16 *)(self + 0x90) == 0x30) {
            if (*(u8 *)(self + 0x95) != 4) *(u8 *)(self + 0x95) = 3;
        }
    }

    if (*(u8 *)(self + 0x95) != 6) return;

    int *cnt = (int *)(self + 0x60);
    *cnt = *cnt + 1;

    int i = 0;
    int scale = 0x200;
    int *q = (int *)(self + 0x4c);
    do {
        if (*(s32 *)(self + 0x60) > (5 - i) * 3) {
            Math_Function_0203b0fc(q, 0x9800, 0x400, scale);
        }
        scale += 0x140;
        q += 1;
        i++;
    } while (i < 5);

    func_ov006_020ec134((C *)self);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020eb018, 0x020eb018, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020eb018
/* #pragma push / O3 / pop bracketed this member in its legacy file; unlike
   opt_strength_reduction, push/pop really is positional, so it is carried
   here in place. */
#pragma push
#pragma O3
extern "C" void func_ov006_020eb018(char *c)
{
    CB *o = (CB*)c;
    (o->*(o->pmf))();
    if (*(unsigned char*)(c + 0x95) != 5) return;
    *(int*)(((int)c + 0x60)) += 1;
    {
        int i = 0;
        int max = 0x200;
        int *p = (int*)(c + 0x4c);
        do {
            if (*(int*)(c + 0x60) > (5 - i) * 3)
                Math_Function_0203b0fc(p, 0x9800, 0x400, max);
            i++;
            max += 0x140;
            p++;
        } while (i < 5);
    }
    func_ov006_020ec134((C *)c);
}
#pragma pop

