//cpp
/* dScMgCup_c -- the cup-shuffle minigame scene, ov006.
 *
 * One translation unit, ROM ordinals 0..31 of the contiguous linker run
 * 0x020de988..0x020e0638: the destructor pair, fourteen file-local helpers,
 * two empty array-element callbacks, the eight State* members the scene
 * dispatches through, the five non-destructor vtable overrides, and the MG_CUP
 * factory.  dScMgCurling_c begins exactly at the next address.  The factory's
 * allocation size, its vptr transition and its constructed member arrays
 * identify the class independently of the boundary.
 *
 * FUNCTION ORDER IS THE ROM'S OWN, LOWEST ADDRESS FIRST, and that is one
 * decision with `#pragma defer_codegen off` below: generating at parse time
 * emits .text in source order where deferred codegen reverses it, it keeps the
 * five member-local optimiser brackets scoped, and it puts D1 ahead of D0 the
 * way the cartridge has them.  Do not reorder and do not drop that pragma.
 *
 * NOT ONE FIELD IN THIS CLASS'S OWN TAIL HAS A PROVEN NAME.  Eleven offsets in
 * 0x5400..0x5470 are proven -- each from a load/store displacement or from a
 * constructor call site -- and zero names are.  That is why the bodies below
 * address state as `*(T *)(c + 0x54xx)` rather than through members: the raw
 * offset is the honest spelling here, not a leftover the next pass should
 * tidy away.  include/dScMgCup_c.h does spell a few of them (mState,
 * mShuffleAngle, mShuffleSpeed, mOnes, mIds, mFlags); those are that header's
 * inferences, not ROM facts, and only mState is used from here.
 *
 * THE EIGHT State* SPELLINGS ARE INFERENCES TOO.  __sinit_ov006_021303d0 fixes
 * the pointer-to-member table's ORDER and each body's behaviour fixes its role,
 * but the original names are not in the cartridge.  The same holds for
 * Virtual50 and for dScMgCup_c_classInit.
 *
 * FIVE SPELLINGS IN THIS FILE WERE MEASURED AND MUST NOT BE TIDIED.  Each
 * carries its own comment where it sits: func_ov006_020ded00's two spellings of
 * +0x12; func_ov006_020def80's late `cup = i;`, which the two goto paths skip;
 * StateShuffle's read-modify-write on +0x545c/+0x545e; Behavior's `(int)`
 * launders over the three parallel per-cup arrays; and Render's two
 * pointer-arithmetic towers.  Every other cast of that shape in this file was
 * measured one at a time and deleted as byte-neutral -- a cast tower here is
 * either load-bearing and commented, or gone.
 *
 * Result: 32/32 byte+relocation matches.  Compiler-only D2/RTTI/vtable
 * passengers are externalized in the manifest to the canonical cartridge
 * copies; this TU claims no data and no bss.
 */
/* Shared declarations first; TU-private layouts follow. */
#include "dScMgCup_c.h"
#include "types.h"
#include "decl_common.h"

/* TU-private layouts, kept only where no shared project type exists yet.
   Three of them are an 8-byte pair of s32 under three names, because they are
   three unrelated tables: Pair6 is the static cup-position table in .data,
   Frame is an animation {id,length} row, and P8 is the per-cup live position
   pair in the object's own tail. */

/* One row of data_ov006_0213c094: a packed left/right nibble pair per round. */
typedef struct { u8 pad; u8 lo:4; u8 hi:4; } Entry094;

typedef struct { int a, b; } Pair6;

/* Stand-in for an unrelated polymorphic object reached by slot, not by name.
   Only slot 18 is ever called through it; the other nineteen exist to place
   that slot at the right index. */
struct Obj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18(int a);
    virtual void v19();
};

/* The initializer at 0x021303d0 proves an eight-entry member-function table. */
typedef void (dScMgCup_c::*PMF)();

/* One row of an animation table in data_ov006_0213c0d8: {sprite id, length}. */
typedef struct Frame {
    int a, b;
} Frame;

/* InitResources' own window onto the object tail -- the one place in this file
   where the three parallel per-cup arrays are written as arrays. */
typedef struct Obj6e {
    char _p0[0x53e8];
    Pair6 pairs[3];  /* 0x53e8 */
    char _p1[0x540c - 0x5400];
    int ones[3];     /* 0x540c */
    char _p2[0x5420 - 0x5418];
    int ids[3];      /* 0x5420 */
    char _p3[0x5465 - 0x542c];
    u8 flags[3];     /* 0x5465 */
} Obj6e;

extern "C" {
extern void func_ov006_020dec5c(char *p, int a, int b);
extern int func_ov006_020deac8(char *p, int a, int b, int c);
extern void func_ov006_020ded84(char *p, int a, int b, int c, signed char e);
extern void func_ov006_020dec88(char *a);
extern void func_ov006_020ded00(int p);
struct OamAttr;
extern int data_ov006_0213c114[];
extern unsigned char data_ov006_0213c064[];
extern int data_ov006_0213c074[];
extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int show, struct OamAttr* attr, int a, int b, int c, int d, int e, int f);
extern unsigned char DecIfAbove0_Byte(unsigned char *counter);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int x);
extern int func_02053200(int x);
extern int func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern int data_0209e650;
extern Entry094 data_ov006_0213c094[];
extern s16 data_ov006_0213c0f4[];
extern Pair6 data_ov006_0213c0c0[];
extern Pair6 data_ov006_0213c0c4[];
extern Pair6 data_ov006_0213c0a8[];
extern Pair6 data_ov006_0213c0ac[];
extern int RandomIntInternal(int *seed);
extern void func_ov004_020b0a54(int c);
extern void func_ov006_020def80(char *self, int i);
extern void func_ov006_020c2594(void *c);
extern void func_ov004_020b1b78(void *c, int val);
extern void func_ov006_020c2664(char *c);
extern void func_ov006_020c2440(char *c);
extern void func_02012790(int x);
extern void func_ov004_020adb1c(int self);
extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0deb[];
extern unsigned char data_020a0dea[];
extern void FreeGfxSlotsById(int arg);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void func_ov006_020deaf0(char* p, int key, int a, int b);
extern int func_ov006_020deb48(char *c, int a, int b, int d, signed char e);
extern short data_02082214[];
extern signed char data_ov006_0213c084[];
extern signed char data_ov006_0213c085[];
extern void func_ov006_020df024(char *o);
extern void func_ov006_020c2924(char *c);
/* The live per-cup {x, y} pair at +0x53e8, seen from Render. */
struct P8 { int a; int b; };
void func_ov006_020debb4(char *a, int b);
void func_ov006_020deed8(int a0, void *a1, int a2, int a3, int a4, int a5);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b2574(int arg0, int arg1);
void func_ov004_020b1e34(void *a, int b, int c, int d);
extern char data_ov006_02139df4[];
extern PMF data_ov006_02141870[];
extern Frame *data_ov006_0213c0d8[];
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0208ee44;
extern void func_ov006_020c225c(char *);
extern int func_ov006_020c3050(char *);
extern int _ZN3G2S13GetBG2CharPtrEv(void);
extern void LoadCompressedFileAt(int, int);
extern void *LoadFile(int);
extern void _ZN3GXS10LoadBGPlttEPKvjj(void *, unsigned int, unsigned int);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern int _ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(void *, unsigned int, unsigned int);
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN8Particle10SysTrackerC1Ev(void *tracker);
void func_ov006_020c33dc(void *table);
void func_020733a8(void *base, int count, int stride, void *ctor, void *dtor);
void func_ov006_020e0634(void);
void func_0203d738(void);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *_ZTV10dScMgCup_c[];
}

#pragma defer_codegen off

/* [0] 0x020de988  _ZN10dScMgCup_cD1Ev  size 0x94 */
// @symbol _ZN10dScMgCup_cD1Ev
/* The three explicit calls are the factory's construction order run backwards:
   mArray2, then mArray1, then the 0x4f38 table -- that one addressed by raw
   offset because the header keeps it opaque.  Everything the ROM does after
   them (own vtable store, the tracker's destruction, the chain up to
   ~dScMgBase_c) is the compiler inlining dScMgSingle3DBase_c's destructor, so
   none of it is written here.  This one definition emits ordinal 1, D0, too. */
dScMgCup_c::~dScMgCup_c()
{
    __destroy_arr(mArray2, 3, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray1, 0x20, 0x18, (void *)func_ov006_020deac4);
    func_ov006_020c3288((char *)this + 0x4f38);
}

/* [2] 0x020deac4  func_ov006_020deac4  size 0x4 */
extern "C" void func_ov006_020deac4(void)
{
}

/* [3] 0x020deac8  func_ov006_020deac8  size 0x28 */
extern "C" int func_ov006_020deac8(char *p, int, int, int)
{
    int n = 0;
    do {
        if (*(unsigned char *)(p + 0x15) == 0)
            break;
        n++;
        p += 0x18;
    } while (n < 0x20);
    return n;
}

/* [4] 0x020deaf0  func_ov006_020deaf0  size 0x58 */
extern "C" void func_ov006_020deaf0(char *p, int key, int a, int b)
{
    int i;
    for (i = 0; i < 0x20; i++) {
        if (*(unsigned char *)(p + 0x15) != 0) {
            if (key == *(signed char *)(p + 0x17)) {
                func_ov006_020dec5c(p, a, b);
            }
        }
        p += 0x18;
    }
}

/* [5] 0x020deb48  func_ov006_020deb48  size 0x6c */
extern "C" int func_ov006_020deb48(char *c, int a, int b, int d, signed char e) {
    int slot = func_ov006_020deac8(c, a, b, d);
    if (slot >= 0 && slot < 0x20) {
        func_ov006_020ded84(c + slot * 0x18, a, b, d, e);
        return slot;
    }
    return -1;
}

/* [6] 0x020debb4  func_ov006_020debb4  size 0x48 */
extern "C" void func_ov006_020debb4(char *a, int b)
{
    int i = 0;
    do {
        if (*(unsigned char *)(a + 0x15) != 0 && b == *(signed char *)(a + 0x17))
            func_ov006_020dec88(a);
        i++;
        a += 0x18;
    } while (i < 0x20);
}

/* [7] 0x020debfc  func_ov006_020debfc  size 0x40 */
extern "C" void func_ov006_020debfc(char *c)
{
    int i;
    char *p = c;
    for (i = 0; i < 0x20; i++) {
        if (*(unsigned char *)(p + 0x15)) func_ov006_020ded00((int)p);
        p += 0x18;
    }
}

/* [8] 0x020dec3c  func_ov006_020dec3c  size 0x20 */
extern "C" void func_ov006_020dec3c(char *p) {
    int i;
    for (i = 0; i < 0x20; i++) {
        *(unsigned char *)(p + 0x15) = 0;
        p += 0x18;
    }
}

/* [9] 0x020dec5c  func_ov006_020dec5c  size 0x2c */
extern "C" void func_ov006_020dec5c(char* self, int a, int b)
{
    if (*(unsigned char*)(self + 0x16) != 1)
        return;
    *(int*)self += a;
    *(int*)(self + 4) += b;
}

/* [10] 0x020dec88  func_ov006_020dec88  size 0x78 */
extern "C" void func_ov006_020dec88(char* t)
{
    int idx = *(signed char*)(t + 0x12) + *(unsigned char*)(t + 0x13) * 6;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
        1,
        (struct OamAttr*)data_ov006_0213c074[data_ov006_0213c064[idx]],
        *(int*)t >> 12,
        *(int*)(t + 4) >> 12,
        -1,
        -1,
        data_ov006_0213c114[idx],
        0);
}

/* [11] 0x020ded00  func_ov006_020ded00  size 0x84 */
extern "C" void func_ov006_020ded00(int self)
{
  *((int *) (self + 0)) += *((int *) (self + 8));
  *((int *) (self + 4)) += *((int *) (self + 0xc));
  if (*((unsigned char *) (self + 0x13)))
  {
    *((int *) (self + 4)) -= 6;
  }
  if (DecIfAbove0_Byte((unsigned char *) (self + 0x14)))
  {
    return;
  }
  *((unsigned char *) (self + 0x14)) = 3;
  /* The two spellings of +0x12 are load-bearing under 2004/b56 and must not be
     unified: the ROM keeps the decrement's address in a register (`add r1,r4,#0x12`)
     and then RE-READS the byte as `ldrsb r0,[r4,#0x12]`. Written with one expression
     shape, b56 recognises the second read as the first lvalue and reuses r1. Every
     way of writing the decrement produces the ROM's form; only the re-read decides. */
  *((signed char *) (self + 0x12)) -= 1;
  if (*((signed char *) self + 0x12) < 0)
  {
    *((signed char *) (self + 0x15)) = 0;
  }
}

/* [12] 0x020ded84  func_ov006_020ded84  size 0x78 */
extern "C" void func_ov006_020ded84(char* t, int a, int b, int c, signed char e)
{
    if (a == 2) {
        *(unsigned char*)(t + 0x16) = 0;
        *(unsigned char*)(t + 0x13) = 1;
    } else {
        *(unsigned char*)(t + 0x16) = (unsigned char)a;
        *(unsigned char*)(t + 0x13) = 0;
    }
    *(signed char*)(t + 0x17) = e;
    *(unsigned char*)(t + 0x15) = 1;
    *(int*)t = b;
    *(int*)(t + 4) = c;
    *(int*)(t + 8) = 0;
    *(int*)(t + 0xc) = 0;
    if (*(unsigned char*)(t + 0x13) != 0)
        *(unsigned char*)(t + 0x12) = 7;
    else
        *(unsigned char*)(t + 0x12) = 5;
    *(unsigned char*)(t + 0x14) = 3;
}

/* [13] 0x020dedfc  func_ov006_020dedfc  size 0xdc */
extern "C" void func_ov006_020dedfc(char *self, int anim, int frame, int cup)
{
    if (anim == 6) {
        if (frame == 4) {
            func_02012718(0x1cf, (int)*(void **)(self + (cup << 3) + 0x5000 + 0x3e8));
        } else if (frame == 5 || frame == 0xb) {
            func_02012718(0x1d0, (int)*(void **)(self + (cup << 3) + 0x5000 + 0x3e8));
        }
    }
    if (anim == 5 && frame == 6 && cup == *(unsigned char *)(self + 0x5000 + 0x46d)) {
        func_02012718(0x1ce, (int)*(void **)(self + (cup << 3) + 0x5000 + 0x3e8));
    }
    if (anim != 1 && anim != 4) return;
    if (frame != 4) return;
    _ZN5Sound12PlayBank2_2DEj(0x1cc);
}

/* [14] 0x020deed8  func_ov006_020deed8  size 0xa8 */
extern "C" void func_ov006_020deed8(int self, void *list, int x, int y, int scale, int modeArg)
{
    unsigned char *e;
    int mode;
    int sx;
    int sy;

    e = (unsigned char *)list;
    sx = x >> 12;
    sy = y >> 12;
    mode = modeArg;
    scale = func_02053200(scale);

    for (;;) {
        int oam = func_ov004_020af770((int)e, sx, sy, -1, -1, scale, 0);
        if (oam != 0) {
            if (mode == 2) {
                if ((unsigned)(*(int *)(e + 4) << 0x10) >> 0x1c == 3) {
                    int *p = (int *)(oam + 4);
                    *p = (*p & ~0xf000) | 0x4000;
                }
            }
        }
        if (*(unsigned short *)(e + 6) == 0xffff)
            break;
        e += 8;
    }
}

/* [15] 0x020def80  func_ov006_020def80  size 0xa4 */
extern "C" void func_ov006_020def80(char *c, int i)
{
    unsigned char flag;
    char *row = c + i;
    int cup;
    row += 0x5000;
    flag = *((unsigned char *) (row + 0x465));
    if (flag == 0) {
        flag = *((unsigned char *) (row + 0x462));
        if (flag != 0) {
            char *out = c + (i << 2);
            out += 0x5000;
            *((int *) (out + 0x434)) = 4;
        } else {
            char *out = c + (i << 2);
            out += 0x5000;
            *((int *) (out + 0x434)) = 1;
        }
        goto epilogue;
    }
    flag = *((unsigned char *) (row + 0x462));
    if (flag != 0) {
        char *out = c + (i << 2);
        out += 0x5000;
        *((int *) (out + 0x434)) = 5;
        if ((*((unsigned char *) ((c + 0x5000) + 0x46d))) == 0xff) {
            if ((*((unsigned char *) ((c + 0x5000) + 0x469))) != 1) {
                *((unsigned char *) ((c + 0x5000) + 0x46d)) = (unsigned char) i;
            }
        }
        goto epilogue;
    }
    {
        char *out = c + (i << 2);
        out += 0x5000;
        *((int *) (out + 0x434)) = 2;
    }
    cup = i;

    /* All three arms clear the same two per-cup counters, but only this last
       one assigns `cup` -- the two `goto epilogue` paths reach the label with
       it still unset, and the epilogue reads `i` out of whatever register the
       compiler left it in.  That is what the cartridge does, and it is the ONLY
       spelling that reproduces it: hoisting the assignment to the declaration
       (`int cup = i;`) and deleting this line was measured and takes this
       function from MATCH to DIFF.  Do not tidy it. */
epilogue:
    {
        char *out = c + (cup << 2);
        out += 0x5000;
        *((int *) (out + 0x440)) = 0;
        *((int *) (out + 0x44c)) = 0;
    }
}

/* [16] 0x020df024  func_ov006_020df024  size 0x198 */
extern "C" void func_ov006_020df024(char *a)
{
    u32 rnd = (u32)RandomIntInternal(&data_0209e650);
    u32 bits = rnd >> 0x10;
    int idx;
    int rem;

    if (bits & 1) {
        idx = data_ov006_0213c094[*(u8*)(a + 0x5461)].hi;
    } else {
        idx = data_ov006_0213c094[*(u8*)(a + 0x5461)].lo;
    }

    idx = idx * 2;
    if (bits & 2)
        idx += 1;

    *(s16*)(a + 0x545e) = data_ov006_0213c0f4[idx];

    rem = (rnd >> 0x18) % 3;
    *(int*)(a + 0x542c) = data_ov006_0213c0c0[rem].a;
    *(int*)(a + 0x5430) = data_ov006_0213c0c4[rem].a;

    *(int*)(a + 0x5400) = (data_ov006_0213c0a8[*(int*)(a + 0x5430)].a +
                            data_ov006_0213c0a8[*(int*)(a + 0x542c)].a) / 2;
    *(int*)(a + 0x5404) = (data_ov006_0213c0ac[*(int*)(a + 0x5430)].a +
                            data_ov006_0213c0ac[*(int*)(a + 0x542c)].a) / 2;
    *(int*)(a + 0x5408) = *(int*)(a + 0x5400) -
                           data_ov006_0213c0a8[*(int*)(a + 0x542c)].a;
    *(s16*)(a + 0x545c) = 0;

    {
        u8 kind = *(u8*)(a + 0x5461);
        if (kind == 7) {
            if (*(u8*)(a + 0x5460) == *(u8*)(a + 0x546c)) {
                goto set1;
            }
        }
        if (kind < 7) {
            goto set0;
        }
        {
            u32 r2 = (u32)RandomIntInternal(&data_0209e650);
            if (((r2 >> 0x10) & 7) != 0) {
                goto set0;
            }
        }
    set1:
        *(u8*)(a + 0x546a) = 1;
        return;
    set0:
        *(u8*)(a + 0x546a) = 0;
        return;
    }
}

/* [17] 0x020df1bc  _ZN10dScMgCup_c9StateIdleEv  size 0x4 */
// @symbol _ZN10dScMgCup_c9StateIdleEv
void dScMgCup_c::StateIdle()
{
}

/* [18] 0x020df1c0  _ZN10dScMgCup_c11StateFinishEv  size 0xcc */
// @symbol _ZN10dScMgCup_c11StateFinishEv
void dScMgCup_c::StateFinish()
{
    char *c = (char *)this;
    *(int*)(c + 0x541c) -= 1;
    if (*(int*)(c + 0x5000 + 0x41c) > 0) return;
    if (*(unsigned char*)(c + 0x5000 + 0x469) != 0) {
        if (*(int*)(c + 0xb4) < 0x270f) *(int*)(c + 0xb4) += 1;
        if (*(int*)(c + 0xb4) > *(int*)(c + 0xb8)) *(int*)(c + 0xb8) = *(int*)(c + 0xb4);
        func_ov004_020b0a54(0);
        *(int*)(c + 0x5000 + 0x418) = 7;
    } else {
        if (*(int*)(c + 0xa8) > 0) {
            ((Obj*)c)->v18(-1);
        } else {
            func_ov004_020b0a54(0x12);
            *(int*)(c + 0x5000 + 0x418) = 7;
        }
    }
}

/* [19] 0x020df28c  _ZN10dScMgCup_c11StateResultEv  size 0x130 */
// @symbol _ZN10dScMgCup_c11StateResultEv
#pragma push
#pragma opt_strength_reduction off
void dScMgCup_c::StateResult()
{
    char *self = (char *)this;
    int score;
    int i;
    int *pc = (int *)(self + 0x541c);
    *pc = *pc - 1;
    if (*(int *)(self + 0x5000 + 0x41c) > 0) return;
    for (i = 0; i < 3; i++) {
        u8 *q = (u8 *)(self + i + 0x5465);
        if (*q == 0) {
            *q = 1;
            func_ov006_020def80(self, i);
        }
    }
    score = *(int *)(self + 0xb4);
    if (*(u8 *)(self + 0x5000 + 0x469) != 0) {
        *(int *)(self + 0x5000 + 0x41c) = 0x3c;
        func_ov006_020c2594(self + 0x4f38);
        score++;
    } else {
        func_ov004_020b1b78(self, 1);
        if (*(int *)(self + 0xa8) > 0) {
            *(int *)(self + 0x5000 + 0x41c) = 0x96;
            func_ov006_020c2664(self + 0x4f38);
        } else {
            *(int *)(self + 0x5000 + 0x41c) = 0x1e;
            func_ov006_020c2440(self + 0x4f38);
        }
        func_02012790(0x12f);
    }
    func_ov004_020adb1c(score);
    *(int *)(self + 0x5000 + 0x418) = 6;
    *(u8 *)(self + 0xc3) = 0;
}
#pragma pop

/* [20] 0x020df3bc  _ZN10dScMgCup_c11StateSelectEv  size 0x184 */
// @symbol _ZN10dScMgCup_c11StateSelectEv
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
void dScMgCup_c::StateSelect()
{
    char *c = (char *)this;
    int flag = 0;
    unsigned int idx = data_020a0e40[0];
    int j;
    unsigned char thA;
    unsigned char thB;

    if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0) flag = 1;
    if (flag == 0) return;

    {
        unsigned char* thP = &data_020a0de8[idx * 4];
        thA = thP[2];
        thB = thP[3];
    }

    for (j = 0; j < 3; j++) {
        int dx = thA - (*(int*)(c + j * 8 + 0x5000 + 0x3e8) >> 12);
        int dy = thB - (*(int*)(c + j * 8 + 0x5000 + 0x3ec) >> 12);
        if (dx > 0x10 || dx < -0x28 || dy > 0x18 || dy < -0x20) continue;

        *(unsigned char*)(c + j + 0x5000 + 0x465) = 1;
        func_ov006_020def80(c, j);

        if (*(unsigned char*)(c + 0x5000 + 0x468) == *(unsigned char*)(c + j + 0x5000 + 0x462)) {
            *(unsigned char*)(c + 0x5000 + 0x469) = 1;
            *(int*)(c + j * 4 + 0x5000 + 0x434) = 6;
        } else {
            *(unsigned char*)(c + 0x5000 + 0x469) = 0;
        }

        *(int*)(c + 0x5000 + 0x418) = 5;
        *(int*)(c + 0x5000 + 0x41c) = 0x3c;
        FreeGfxSlotsById(0x1d);

        {
            unsigned int idx2 = data_020a0e40[0];
            unsigned char a1 = *(volatile unsigned char*)&data_020a0deb[idx2 * 4];
            unsigned char a2 = *(volatile unsigned char*)&data_020a0dea[idx2 * 4];
            *(int*)(c + 0x5000 + 0xdc) = 1;
            *(int*)(c + 0x5000 + 0xd4) = a2;
            *(int*)(c + 0x5000 + 0xd8) = a1;
        }
        _ZN5Sound12PlayBank2_2DEj(0x1cd);
        return;
    }
}
#pragma pop

/* [21] 0x020df540  _ZN10dScMgCup_c17StateWaitForInputEv  size 0x78 */
// @symbol _ZN10dScMgCup_c17StateWaitForInputEv
void dScMgCup_c::StateWaitForInput()
{
    char *c = (char *)this;
    *(int *)(c + 0x541c) -= 1;
    if (*(int *)(c + 0x541c) > 0)
        return;
    func_ov004_020b0cac(0xf, 0x80, 0x38, 0, -1, 0xd);
    *(int *)(c + 0x5418) = 4;
}

/* [22] 0x020df5b8  _ZN10dScMgCup_c12StateShuffleEv  size 0x720 */
// @symbol _ZN10dScMgCup_c12StateShuffleEv
#pragma push
#pragma opt_common_subs off
void dScMgCup_c::StateShuffle()
{
    char *c = (char *)this;
    int ox, oy;

    {
        int v = *(s16*)(c + 0x545e);
        int amp;
        if (v < 0) v = -v;
        amp = (v * 0x1f4) / 0x1000;
        if (amp >= 0x1f4) amp = 0x1f4;
        *(int*)(c + 0x5458) = func_02012468(*(int*)(c + 0x5458), 2, 0x1cb, 2, 0, amp, 0, 0);
    }

    *(u16*)(c + 0x545c) += *(s16*)(c + 0x545e);

    {
        s16 speed = *(s16*)(c + 0x545e);
        if ((speed >= 0 && *(u16*)(c + 0x545c) >= 0x8000u) ||
            (speed < 0 && *(u16*)(c + 0x545c) <= 0x8000u)) {
            *(u16*)(c + 0x545c) = 0x8000;
        }
    }

    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        int sinv = data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2 + 1];
        ox = *(int*)(c + slot * 8 + 0x53e8);
        oy = *(int*)(c + slot * 8 + 0x53ec);
        *(int*)(c + slot * 8 + 0x53e8) = *(int*)(c + 0x5400)
            - (int)(((long long)sinv * *(int*)(c + 0x5408) + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        *(int*)(c + slot * 8 + 0x53ec) = *(int*)(c + 0x5404)
            - (int)((data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        *(int*)(c + slot * 4 + 0x540c) = ((*(int*)(c + slot * 8 + 0x53ec) - *(int*)(c + 0x5404)) >> 7) + 0x1000;
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        func_ov006_020deaf0(c + 0x50e8, (u8)(s8)slot,
            *(int*)(c + slot * 8 + 0x53e8) - ox,
            *(int*)(c + slot * 8 + 0x53ec) - oy);
    }

    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        int sinv = data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2 + 1];
        ox = *(int*)(c + slot * 8 + 0x53e8);
        oy = *(int*)(c + slot * 8 + 0x53ec);
        *(int*)(c + slot * 8 + 0x53e8) = *(int*)(c + 0x5400)
            + (int)(((long long)sinv * *(int*)(c + 0x5408) + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        *(int*)(c + slot * 8 + 0x53ec) = *(int*)(c + 0x5404)
            + (int)((data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        *(int*)(c + slot * 4 + 0x540c) = ((*(int*)(c + slot * 8 + 0x53ec) - *(int*)(c + 0x5404)) >> 7) + 0x1000;
    }
    {
        int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
        func_ov006_020deaf0(c + 0x50e8, (u8)(s8)slot,
            *(int*)(c + slot * 8 + 0x53e8) - ox,
            *(int*)(c + slot * 8 + 0x53ec) - oy);
    }

    {
        char *state = c + 0x5400;
        u16 angle = *(u16*)(state + 0x5c);
        if (angle == 0x8000) {
            int tb = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
            int ta = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
            *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420) = tb;
            *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420) = ta;
            *(u8*)(c + 0x5460) -= 1;
            if (*(u8*)(c + 0x5460) == 0) {
                *(int*)(c + 0x541c) = 0x1e;
                *(int*)(c + 0x5418) = 3;
            } else {
                u32 r = RandomIntInternal(&data_0209e650);
                *(int*)(c + 0x541c) = (r >> 8) % 0x18 + 1;
                *(int*)(c + 0x5418) = 1;
            }
        } else if (*(u8*)(c + 0x546a) != 0) {
            s16 speed = *(s16*)(state + 0x5e);
            if ((speed >= 0 && angle >= 0x5555u) || (speed < 0 && angle <= 0xaaabu)) {
                /* This spelling is load-bearing; all three parts were measured.
                   The two loads above share one base register through `state`,
                   but the ROM re-materialises `this + 0x5400` here, so these
                   stores must respell the full offset rather than reuse it.
                   `speed = -speed;` as its own statement orders the rsb before that
                   base add; storing `-speed` directly reverses the pair. And the
                   increment must be a plain read-modify-write -- writing it as
                   `+= 0x8000` makes mwcc CSE the field address into a register
                   and drop the displacement, which `opt_common_subs off` (in
                   force over this function) does not undo. */
                speed = -speed;
                *(s16*)(c + 0x545e) = speed;
                *(u16*)(c + 0x545c) = *(u16*)(c + 0x545c) + 0x8000;
                {
                    int tb = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                    int ta = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                    *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420) = tb;
                    *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420) = ta;
                }
                *(u8*)(c + 0x546a) = 0;
            }
        }
    }

    if (*(u8*)(c + 0x546b) != 0) {
        int cnt = *(int*)(c + 0x541c);
        if ((cnt & 3) == 0) {
            int i;
            {
                int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                i = (cnt >> 2) & 7;
                func_ov006_020deb48(c + 0x50e8, 2,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
            {
                int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 2,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
        }
    } else {
        int cnt = *(int*)(c + 0x541c);
        if (cnt & 1) {
            int i;
            {
                int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                i = (cnt >> 1) & 7;
                func_ov006_020deb48(c + 0x50e8, 1,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
            {
                int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 1,
                    *(int*)(c + slot * 8 + 0x53e8) + (data_ov006_0213c084[i * 2] << 12),
                    *(int*)(c + slot * 8 + 0x53ec) + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
        }
        if ((*(int*)(c + 0x541c) & 3) == 0) {
            {
                int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 0,
                    *(int*)(c + slot * 8 + 0x53e8) - 0xe000,
                    *(int*)(c + slot * 8 + 0x53ec) + 0x1c000,
                    slot);
            }
            {
                int slot = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
                func_ov006_020deb48(c + 0x50e8, 0,
                    *(int*)(c + slot * 8 + 0x53e8) - 0xe000,
                    *(int*)(c + slot * 8 + 0x53ec) + 0x1c000,
                    slot);
            }
        }
    }

    *(int*)(c + 0x541c) += 1;
}
#pragma pop

/* [23] 0x020dfcd8  _ZN10dScMgCup_c19StatePrepareShuffleEv  size 0x70 */
// @symbol _ZN10dScMgCup_c19StatePrepareShuffleEv
void dScMgCup_c::StatePrepareShuffle()
{
    char *o = (char *)this;
    int *p = (int *)(o + 0x541c);
    *p = *p - 1;
    if (*(int *)(o + 0x5000 + 0x41c) > 0) return;
    func_ov006_020df024(o);
    if (*(unsigned char *)(o + 0x5000 + 0x46b) != 0) {
        unsigned char *q = (unsigned char *)(o + 0x546b);
        *q = *q - 1;
    }
    *(int *)(o + 0x5000 + 0x418) = 2;
}

/* [24] 0x020dfd48  _ZN10dScMgCup_c10StateSetupEv  size 0x18c */
// @symbol _ZN10dScMgCup_c10StateSetupEv
void dScMgCup_c::StateSetup()
{
    char *c = (char *)this;
    int i;
    int score;
    unsigned int rnd;
    unsigned int hi;
    unsigned int kind;
    unsigned int picked;

    (*(int *)(c + 0x541c))--;
    if (*(int *)(c + 0x541c) > 0) {
        return;
    }

    for (i = 0; i < 3; i++) {
        *(unsigned char *)(c + i + 0x5465) = 0;
        func_ov006_020def80(c, i);
    }

    score = *(int *)(c + 0xb4);
    if (score < 0xa) {
        *(unsigned char *)(c + 0x5461) = (unsigned char)score;
    } else {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650);
        hi = rnd >> 16;
        kind = (hi % 5) + 5;
        *(unsigned char *)(c + 0x5461) = (unsigned char)kind;
    }

    *(unsigned char *)(c + 0x5460) =
        ((unsigned char *)data_ov006_0213c094)[*(unsigned char *)(c + 0x5461) * 2];

    if (score > 3) {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650);
        if (rnd & 1) {
            *(unsigned char *)(c + 0x5468) = 1;
        } else {
            *(unsigned char *)(c + 0x5468) = 2;
        }
    } else if (score == 3) {
        *(unsigned char *)(c + 0x5468) = 2;
    }

    *(unsigned char *)(c + 0x546b) = 0;
    *(unsigned char *)(c + 0x5469) = 0;
    *(int *)(c + 0x5418) = 1;
    *(int *)(c + 0x541c) = 0x1e;
    *(unsigned char *)(c + 0x546d) = 0xff;
    *(int *)(c + 0x5458) = 0;
    FreeGfxSlotsById(0x1d);

    rnd = (unsigned int)RandomIntInternal(&data_0209e650);
    picked = (rnd % 10) + 1;
    *(unsigned char *)(c + 0x546c) = (unsigned char)picked;

    if (*(unsigned char *)(c + 0xc4) == 0) {
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(short *)(c + 0xc0) = 0;
    }
}

/* [25] 0x020dfed4  _ZN10dScMgCup_c9Virtual50Ev  size 0x18 */
/* Vtable slot 20. The original name is not in the ROM; `Virtual50` is the
   tree's placeholder spelling for this slot. Body is a single forward to the
   0x4f38 table's own entry point, and it falls off the end -- the ROM does
   the same, so the declared `int` return is never actually produced. */
// @symbol _ZN10dScMgCup_c9Virtual50Ev
int dScMgCup_c::Virtual50()
{
    char *p = (char *)this;

    func_ov006_020c2594(p + 0x4f38);
}

/* [26] 0x020dfeec  _ZN10dScMgCup_c13OnYoshiTryEatEi  size 0x17c */
#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgCup_c::OnYoshiTryEat(int msg)
{
    char *c = (char *)this;

    int i;

    if (msg == 3 || msg == 0x12) {
        if (msg == 3) {
            *(int *)(c + 0xb4) = 0;
        } else {
            *(int *)(c + 0xb4) = 0;
            func_ov004_020adb1c(*(int *)(c + 0xb4));
        }
        *(int *)(c + 0xa8) = 2;
        *(int *)(c + 0xac) = *(int *)(c + 0xa8);
        *(unsigned char *)(c + 0x5000 + 0x462) = 1;
        *(unsigned char *)(c + 0x5000 + 0x463) = 0;
        if (*(int *)(c + 0xb4) >= 3) {
            *(unsigned char *)(c + 0x5000 + 0x464) = 2;
        } else {
            *(unsigned char *)(c + 0x5000 + 0x464) = 0;
        }
        *(unsigned char *)(c + 0x5000 + 0x468) = 1;
    } else if (msg == 0) {
        *(int *)(c + 0xa8) = 2;
        *(int *)(c + 0xac) = *(int *)(c + 0xa8);
        if (*(int *)(c + 0xb4) == 3) {
            *(unsigned char *)(c + 0x5000 + 0x462) = 1;
            *(unsigned char *)(c + 0x5000 + 0x463) = 0;
            *(unsigned char *)(c + 0x5000 + 0x464) = 2;
        }
    }

    for (i = 0; i < 3; i++) {
        if (*(unsigned char *)(c + i + 0x5000 + 0x462) != 0) {
            *(int *)(c + i * 4 + 0x5000 + 0x434) = 3;
        } else {
            *(int *)(c + i * 4 + 0x5000 + 0x434) = 0;
        }
        *(int *)(c + i * 4 + 0x5000 + 0x440) = 0;
        *(int *)(c + i * 4 + 0x5000 + 0x44c) = 0;
    }

    *(int *)(c + 0x5000 + 0x41c) = 0x3c;
    *(int *)(c + 0x5000 + 0x418) = 0;
    *(int *)(c + 0x5000 + 0xdc) = 0;
    func_ov006_020c2924(c + 0x4f38);

    *(unsigned int *)(c + 0xbc) = *(int *)(c + 0xb4);
    if (*(unsigned int *)(c + 0xbc) > 0x270e) {
        *(unsigned int *)(c + 0xbc) = 0x270e;
    }
    func_ov004_020b0cac(0xd, 0x80, 0xa0, 1, -1, 0xd);
}
#pragma pop

/* [27] 0x020e0068  _ZN10dScMgCup_c6RenderEv  size 0x19c */
// @symbol _ZN10dScMgCup_c6RenderEv
/* dScMgCup_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable at ov006 0x0213c154, the second of the two slots
 * where the table differs from dScMgSingle3DBase_c's. The old file carried no
 * `recovered name:` comment at all, only the func_ov006_ address.
 *
 * Draws the three cups back to front: the bubble sort orders the indices by the
 * per-cup depth at 0x53ec, and the render loop then walks them in that order.
 *
 * WAS A C99 FILE, and every declaration below therefore has to move inside
 * `extern "C"` -- in C++ these names would otherwise mangle and the link would
 * come up short. The `extern` on the two arrays is load-bearing for the same
 * reason the tree's notes give: without it, a variable declaration inside
 * `extern "C" {}` is a DEFINITION and collides with the delinked gap object. */
s32 dScMgCup_c::Render()
{
    char *c = (char *)this;
    int list[3];
    int i;
    int limit;
    int j;
    int cur;
    int next;
    int k;
    int cup;

    for (i = 0; i < 3; i++) {
        list[i] = i;
    }

    for (limit = 3; limit > 1; limit--) {
        for (j = 0; j < limit - 1; j++) {
            next = list[j + 1];
            cur = list[j];
            if (*(int*)(c + cur * 8 + 0x53ec) < *(int*)(c + next * 8 + 0x53ec)) {
                list[j] = next;
                list[j + 1] = cur;
            }
        }
    }

    /* The two pointer-arithmetic spellings in the call below are load-bearing.
       Rewriting `((int*)c + k)[0x1510]` as `*(int*)(c + k * 4 + 0x5440)`, or
       `((struct P8*)c + cup)[0xa7d].b` as `*(int*)(c + cup * 8 + 0x53ec)`, was
       measured here: either one alone takes Render from MATCH to DIFF. b56
       scales the index into the base register from these forms and re-computes
       the address from `c` in the flat form. */
    for (k = 0; k < 3; k++) {
        cup = list[k];
        func_ov006_020debb4(c + 0x50e8, (char)cup);
        func_ov006_020deed8((int)c,
            (void *)data_ov006_0213c0d8[*(int*)(c + cup * 4 + 0x5434)][((int*)c + k)[0x1510]].a,
            *(int*)(c + cup * 8 + 0x53e8),
            ((struct P8*)c + cup)[0xa7d].b,
            *(int*)(c + cup * 4 + 0x540c),
            *(unsigned char*)(c + cup + 0x5462));
    }

    if (*(int*)(c + 0x5418) == 4 || *(int*)(c + 0x5418) == 5) {
        Hud_RenderSprite(data_ov006_02139df4, 0x92, 0x20,
            (*(unsigned char*)(c + 0x5468) == 2) ? 4 : -1, -1);
    }

    func_ov004_020b2574(*(int*)(c + 0xa8), 1);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);
    func_ov006_020c29dc(c + 0x4f38);
    return 1;
}

/* [28] 0x020e0204  _ZN10dScMgCup_c8BehaviorEv  size 0x104 */
// @symbol _ZN10dScMgCup_c8BehaviorEv
/* dScMgCup_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable at ov006 0x0213c154, which holds 0x020e0204 in
 * the one slot where the table differs from dScMgSingle3DBase_c's. The old file's
 * `recovered name: dScMgCup_c_Behavior` agrees for once -- unlike the four the
 * reference commit had to correct, this address really is Cup's own.
 *
 * The dispatch through data_ov006_02141870 is a pointer-to-member table indexed by
 * mState. `C` stays a bare non-polymorphic stand-in rather than dScMgCup_c: a PMF
 * on a polymorphic class is a different representation, so the shape here is
 * codegen and not decoration -- the same reason
 * src/_ZN15daObjMarioCap_c8BehaviorEv.cpp keeps its own stand-in.
 *
 * The three parallel per-cup arrays keep their `(int)` launders; those steer the
 * address arithmetic and are not spellings that can be tidied.  Re-measured:
 * strip the launders and nothing else and Behavior goes MATCH -> DIFF, nine
 * words. */
#pragma push
#pragma opt_strength_reduction off
s32 dScMgCup_c::Behavior()
{
    char *o = (char *)this;
    int i;
    (this->*data_ov006_02141870[mState])();
    for (i = 0; i < 3; i++) {
        Frame *f = &data_ov006_0213c0d8[*(int *)(((int)o + i * 4 + 0x5434))][*(int *)(o + i * 4 + 0x5440)];
        int n = f->b;
        if (n != 0) {
            *(int *)(((int)o + i * 4 + 0x544c)) += 1;
            if (*(int *)(((int)o + i * 4 + 0x544c)) >= n) {
                *(int *)(((int)o + i * 4 + 0x544c)) = 0;
                *(int *)(((int)o + i * 4 + 0x5440)) += 1;
                func_ov006_020dedfc(o, *(int *)(((int)o + i * 4 + 0x5434)),
                                    *(int *)(((int)o + i * 4 + 0x5440)), i);
            }
        }
    }
    func_ov006_020debfc(o + 0x50e8);
    func_ov006_020c2b8c(o + 0x4f38);
    return 1;
}
#pragma pop

/* [29] 0x020e0308  _ZN10dScMgCup_c13InitResourcesEv  size 0x26c */
// @symbol _ZN10dScMgCup_c13InitResourcesEv
/* dScMgCup_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgCup_c, this class's own slot 0
 * (fBase_c::InitResources). The old file's `recovered name:
 * dScMgCup_c_InitResources` agreed.
 *
 * Minigame sub-screen setup: configures sub BG1/BG2 control, loads the
 * board tiles/map/palette files, sets the touch UI margins, initializes
 * the three sliders from the table at data_ov006_0213c0a8, then calls
 * virtual +0x48 (this class's own slot 18) with mode 3. */
s32 dScMgCup_c::InitResources()
{
    char *c = (char *)this;
    void *f;

    data_0209d45c = 0x11;
    func_ov006_020c225c(c + 0x4660);
    if (func_ov006_020c3050(c + 0x4f38) == 0)
        return 0;

    {
        volatile u16 *bg = (volatile u16 *)0x04001008;
        *bg = (*bg & 0x43) | 0x2214;
        *bg = *bg & ~0x40;
        *bg = *bg & ~3;
        SetSubBg0Offset(0, 0);
    }

    data_0209d454 |= 1;
    {
        volatile u16 *bg = (volatile u16 *)0x0400100c;
        *bg = (*bg & 0x43) | 0x414;
        *bg = *bg & ~0x40;
        *bg = (*bg & ~3) | 3;
        SetSubBg2Offset(0, 0);
    }

    data_0209d454 |= 4;

    LoadCompressedFileAt(0x2b, _ZN3G2S13GetBG2CharPtrEv() + 0x4000);
    f = LoadFile(0x2c);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);
    LoadCompressedFileAt(0x2a, _ZN3G2S12GetBG0ScrPtrEv());
    LoadCompressedFileAt(0x29, _ZN3G2S12GetBG2ScrPtrEv());
    LoadCompressedFileAt(0xc5, 0x6600000);
    f = LoadFile(0xc6);
    _ZN3GXS11LoadOBJPlttEPKvjj(f, 0, 0xa0);
    Deallocate(f);

    data_0208ee44 = 1;
    func_ov004_020b6808();
    data_ov004_020bc880 = 0x80;
    data_ov004_020bc884 = 0x88;
    data_ov004_020bc8a8 = 0x40;
    data_ov004_020bc898 = 0xa0;
    data_ov004_020bc86c = 0xc0;
    data_ov004_020bc8a4 = 0xa0;

    {
        int i;
        for (i = 0; i < 3; i++) {
            ((Obj6e *)c)->pairs[i].a = data_ov006_0213c0a8[i].a;
            ((Obj6e *)c)->pairs[i].b = data_ov006_0213c0a8[i].b;
            ((Obj6e *)c)->ones[i] = 0x1000;
            ((Obj6e *)c)->ids[i] = i;
            *(u8 *)(c + i + 0x5465) = 1;
        }
    }

    ((Obj *)c)->v18(3);
    func_ov006_020dec3c(c + 0x50e8);
    return 1;
}

/* [30] 0x020e0574  dScMgCup_c_classInit  size 0xc0 */
// @symbol dScMgCup_c_classInit
/* Actor-table factory. The MG_CUP profile, allocation size, RTTI and vptr
 * transition identify dScMgCup_c. The literal construction spelling preserves
 * the measured CodeWarrior array-construction boundary. */
extern "C" void *dScMgCup_c_classInit()
{
    char *scene = (char *)_ZN7fBase_cnwEj(sizeof(dScMgCup_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(scene + 0x471c);
        *(void **)scene = _ZTV10dScMgCup_c + 2;
        func_ov006_020c33dc(scene + 0x4f38);
        func_020733a8(scene + 0x50e8, 0x20, 0x18,
                      (void *)func_ov006_020e0634,
                      (void *)func_ov006_020deac4);
        func_020733a8(scene + 0x53e8, 3, 8,
                      (void *)func_0203d738,
                      (void *)NullDestructor_0203d47c);
    }
    return scene;
}

/* [31] 0x020e0634  func_ov006_020e0634  size 0x4 */
// @symbol func_ov006_020e0634
extern "C" void func_ov006_020e0634()
{
}
