//cpp
/* SHADOW translation unit -- not enrolled in the production ROM build.
 * ov006/dScMgCup_c: 32 functions, 0x020de988..0x020e0638.
 *
 * The boundary is reconstructed through the complete class run, the adjacent
 * MG_CUP factory, and its empty array-element constructor; dScMgCurling_c begins
 * exactly at the next address. The factory allocation, vptr transition, and
 * constructed member arrays independently identify the class.
 *
 * #pragma defer_codegen off is load-bearing: eager ROM-order emission lets the
 * five member-local optimizer settings remain scoped and emits D1/D0 in cartridge
 * order. The eight State* aliases are class-anchored inferences from the PMF table
 * built by __sinit_ov006_021303d0; their exact original spellings are not claimed.
 *
 * Current focused result: 32/32 byte+relocation matches. Compiler-only
 * D2/RTTI/vtable passengers are explicitly deadstripped in the manifest in favor
 * of canonical ROM copies.
 */
/* Shared declarations first; TU-private layouts follow. */
#include "dScMgCup_c.h"
#include "types.h"
#include "decl_common.h"

/* TU-private layouts retained only where no shared project type exists yet. */
typedef struct { u8 pad; u8 lo:4; u8 hi:4; } Entry094;

typedef struct { int a, b; } Pair6;

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

typedef struct Frame {
    int a, b;
} Frame;

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void d4();
    virtual void d5();
    virtual void d6();
    virtual void d7();
    virtual void d8();
    virtual void d9();
    virtual void d10();
    virtual void d11();
    virtual void d12();
    virtual void d13();
    virtual void d14();
    virtual void d15();
    virtual void d16();
    virtual void d17();
    virtual void m18(int);
};

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
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi( int show, struct OamAttr* attr, int a, int b, int c, int d, int e, int f);
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
extern "C" void func_ov004_020b0a54(int c);
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
struct P8 { int a; int b; };
void func_ov006_020debb4(char *a, int b);
void func_ov006_020deed8(int a0, void *a1, int a2, int a3, int a4, int a5);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b2574(int arg0, int arg1);
void func_ov004_020b1e34(void *a, int b, int c, int d);
extern char data_ov006_02139df4[];
extern "C" PMF data_ov006_02141870[];
extern "C" Frame *data_ov006_0213c0d8[];
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN10dScMgCup_cD1Ev, 0x020de988, size 0x94 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dScMgCup_cD1Ev
/* recovered: real C++ destructor. The three explicit calls reproduce the
   ROM's own recovered body (func_ov006_020de988, pre-migration) exactly:
   destroy mArray2 first, then mArray1 (reverse of the factory's
   construction order), then the 0x4f38 table -- addressed by raw offset
   from `this` because the header keeps it opaque. Everything after -- own
   vtable store, mSysTracker destruction, chain to ~dScMgBase_c() -- is the
   compiler's own inlining of dScMgSingle3DBase_c's now-inline destructor
   (see include/dScMgSingle3DBase_c.h's own note). */
dScMgCup_c::~dScMgCup_c()
{
    __destroy_arr(mArray2, 3, 8, (void *)NullDestructor_0203d47c);
    __destroy_arr(mArray1, 0x20, 0x18, (void *)func_ov006_020deac4);
    func_ov006_020c3288((char *)this + 0x4f38);
}

/* The single source-level destructor definition above also emits the D0
 * variant at ROM ordinal 1. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_020deac4, 0x020deac4, size 0x4 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020deac4(void)
{
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020deac8, 0x020deac8, size 0x28 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020deac8(char *p, int, int, int)
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020deaf0, 0x020deaf0, size 0x58 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020deaf0(char* p, int key, int a, int b){
  int i;
  for(i=0; i<0x20; i++){
    if(*(unsigned char*)(p+0x15) != 0){
      if(key == *(signed char*)(p+0x17)){
        func_ov006_020dec5c(p, a, b);
      }
    }
    p += 0x18;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020deb48, 0x020deb48, size 0x6c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_020deb48(char *c, int a, int b, int d, signed char e) {
    int r = func_ov006_020deac8(c, a, b, d);
    if (r >= 0 && r < 0x20) {
        func_ov006_020ded84(c + r * 0x18, a, b, d, e);
        return r;
    }
    return -1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020debb4, 0x020debb4, size 0x48 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020debb4(char* a, int b){
  int i = 0;
  do {
    if (*(unsigned char*)(a+0x15) != 0 && b == *(signed char*)(a+0x17))
      func_ov006_020dec88(a);
    i++;
    a += 0x18;
  } while (i < 0x20);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020debfc, 0x020debfc, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020debfc(char* c){
  int i;
  char* p = c;
  for (i = 0; i < 0x20; i++) {
    if (*(unsigned char*)(p+0x15)) func_ov006_020ded00((int)p);
    p += 0x18;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020dec3c, 0x020dec3c, size 0x20 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020dec3c(char *p) {
    int i;
    for (i = 0; i < 0x20; i++) {
        *(unsigned char *)(p + 0x15) = 0;
        p += 0x18;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020dec5c, 0x020dec5c, size 0x2c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020dec5c(char* self, int a, int b)
{
    if (*(unsigned char*)(self + 0x16) != 1)
        return;
    *(int*)self += a;
    *(int*)(((int)self + 4)) += b;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020dec88, 0x020dec88, size 0x78 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020ded00, 0x020ded00, size 0x84 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ded00(int self)
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020ded84, 0x020ded84, size 0x78 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020ded84(char* t, int a, int b, int c, signed char e)
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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020dedfc, 0x020dedfc, size 0xdc */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020dedfc(char *r7, int r6, int r5, int r4)
{
    if (r6 == 6) {
        if (r5 == 4) {
            func_02012718(0x1cf, (int)*(void **)(r7 + (r4 << 3) + 0x5000 + 0x3e8));
        } else if (r5 == 5 || r5 == 0xb) {
            func_02012718(0x1d0, (int)*(void **)(r7 + (r4 << 3) + 0x5000 + 0x3e8));
        }
    }
    if (r6 == 5 && r5 == 6 && r4 == *(unsigned char *)(r7 + 0x5000 + 0x46d)) {
        func_02012718(0x1ce, (int)*(void **)(r7 + (r4 << 3) + 0x5000 + 0x3e8));
    }
    if (r6 != 1 && r6 != 4) return;
    if (r5 != 4) return;
    _ZN5Sound12PlayBank2_2DEj(0x1cc);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020deed8, 0x020deed8, size 0xa8 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020deed8(int a0, void *a1, int a2, int a3, int s, int a5)
{
    unsigned char *sl;
    int a5_local;
    int e2;
    int e3;
    int neg1, zero, lim;

    sl = (unsigned char *)a1;
    e2 = a2 >> 12;
    e3 = a3 >> 12;
    a5_local = a5;
    s = func_02053200(s);
    neg1 = -1;
    zero = 0;
    lim = 0xffff;

    for (;;) {
        int r = func_ov004_020af770((int)sl, e2, e3, neg1, neg1, s, zero);
        if (r != 0) {
            if (a5_local == 2) {
                if ((unsigned)(*(int *)(sl + 4) << 0x10) >> 0x1c == 3) {
                    int *p = (int *)(((int)r + 4));
                    *p = (*p & ~0xf000) | 0x4000;
                }
            }
        }
        if (*(unsigned short *)(sl + 6) == (unsigned short)lim)
            break;
        sl += 8;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020def80, 0x020def80, size 0xa4 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020def80(char *c, int i)
{
  unsigned char t;
  char *b = c + i;
  int new_var;
  b += 0x5000;
  t = *((unsigned char *) (b + 0x465));
  if (t == 0)
  {
    t = *((unsigned char *) (b + 0x462));
    if (t != 0)
    {
      char *s = c + (i << 2);
      s += 0x5000;
      *((int *) (s + 0x434)) = 4;
    }
    else
    {
      char *s = c + (i << 2);
      s += 0x5000;
      *((int *) (s + 0x434)) = 1;
    }
    goto epilogue;
  }
  t = *((unsigned char *) (b + 0x462));
  if (t != 0)
  {
    char *s = c + (i << 2);
    s += 0x5000;
    *((int *) (s + 0x434)) = 5;
    {
      char *p;
      if ((*((unsigned char *) ((c + 0x5000) + 0x46d))) == 0xff)
      {
        if ((*((unsigned char *) ((c + 0x5000) + 0x469))) != 1)
        {
          *((unsigned char *) ((c + 0x5000) + 0x46d)) = (unsigned char) i;
        }
      }
    }
    goto epilogue;
  }
  {
    char *s = c + (i << 2);
    s += 0x5000;
    *((int *) (s + 0x434)) = 2;
  }
  new_var = i;
  epilogue:
  {
    char *s = c + (new_var << 2);
    s += 0x5000;
    *((int *) (s + 0x440)) = 0;
    *((int *) (s + 0x44c)) = 0;
  }

}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020df024, 0x020df024, size 0x198 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020df024(char *a)
{
    u32 rnd = (u32)RandomIntInternal(&data_0209e650);
    u32 r16 = rnd >> 0x10;
    int lr;
    int rem;

    if (r16 & 1) {
        lr = data_ov006_0213c094[*(u8*)(a + 0x5461)].hi;
    } else {
        lr = data_ov006_0213c094[*(u8*)(a + 0x5461)].lo;
    }

    lr = lr * 2;
    if (r16 & 2)
        lr += 1;

    *(s16*)(a + 0x545e) = data_ov006_0213c0f4[lr];

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
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- _ZN10dScMgCup_c9StateIdleEv, 0x020df1bc, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dScMgCup_c9StateIdleEv
void dScMgCup_c::StateIdle()
{
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- _ZN10dScMgCup_c11StateFinishEv, 0x020df1c0, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dScMgCup_c11StateFinishEv
void dScMgCup_c::StateFinish()
{
    char *c = (char *)this;
    *(int*)(((int)c + 0x541c)) -= 1;
    if (*(int*)(c + 0x5000 + 0x41c) > 0) return;
    if (*(unsigned char*)(c + 0x5000 + 0x469) != 0) {
        if (*(int*)(c + 0xb4) < 0x270f) *(int*)(((int)c + 0xb4)) += 1;
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- _ZN10dScMgCup_c11StateResultEv, 0x020df28c, size 0x130 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dScMgCup_c11StateResultEv
#pragma push
#pragma opt_strength_reduction off
void dScMgCup_c::StateResult()
{
    char *self = (char *)this;
        int r4;
        int i;
        int *pc = (int *)(((int)self + 0x541c));
        *pc = *pc - 1;
        if (*(int *)(self + 0x5000 + 0x41c) > 0) return;
        for (i = 0; i < 3; i++) {
            u8 *q = (u8 *)(((int)self + i + 0x5465));
            if (*q == 0) {
                *q = 1;
                func_ov006_020def80(self, i);
            }
        }
        r4 = *(int *)(self + 0xb4);
        if (*(u8 *)(self + 0x5000 + 0x469) != 0) {
            *(int *)(self + 0x5000 + 0x41c) = 0x3c;
            func_ov006_020c2594(self + 0x4f38);
            r4++;
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
        func_ov004_020adb1c(r4);
        *(int *)(self + 0x5000 + 0x418) = 6;
        *(u8 *)(self + 0xc3) = 0;
    }
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- _ZN10dScMgCup_c11StateSelectEv, 0x020df3bc, size 0x184 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- _ZN10dScMgCup_c17StateWaitForInputEv, 0x020df540, size 0x78 */
/* -------------------------------------------------------------------------- */
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
/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- _ZN10dScMgCup_c12StateShuffleEv, 0x020df5b8, size 0x720 */
/* -------------------------------------------------------------------------- */
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

    *(u16*)(((long long)(int)(c + 0x545c))) += *(s16*)(c + 0x545e);

    {
        s16 vv = *(s16*)(c + 0x545e);
        if ((vv >= 0 && *(u16*)(c + 0x545c) >= 0x8000u) ||
            (vv < 0 && *(u16*)(c + 0x545c) <= 0x8000u)) {
            *(u16*)(c + 0x545c) = 0x8000;
        }
    }

    {
        int slot = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
        int sv = data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2 + 1];
        ox = *(int*)(c + slot * 8 + 0x53e8);
        oy = *(int*)(c + slot * 8 + 0x53ec);
        *(int*)(c + slot * 8 + 0x53e8) = *(int*)(c + 0x5400)
            - (int)(((long long)sv * *(int*)(c + 0x5408) + 0x800) >> 12);
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
        int sv = data_02082214[(*(u16*)(c + 0x545c) >> 4) * 2 + 1];
        ox = *(int*)(c + slot * 8 + 0x53e8);
        oy = *(int*)(c + slot * 8 + 0x53ec);
        *(int*)(c + slot * 8 + 0x53e8) = *(int*)(c + 0x5400)
            + (int)(((long long)sv * *(int*)(c + 0x5408) + 0x800) >> 12);
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
        u16 uu = *(u16*)(state + 0x5c);
        if (uu == 0x8000) {
            int tb = *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420);
            int ta = *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420);
            *(int*)(c + *(int*)(c + 0x542c) * 4 + 0x5420) = tb;
            *(int*)(c + *(int*)(c + 0x5430) * 4 + 0x5420) = ta;
            *(u8*)(((long long)(int)(c + 0x5460))) -= 1;
            if (*(u8*)(c + 0x5460) == 0) {
                *(int*)(c + 0x541c) = 0x1e;
                *(int*)(c + 0x5418) = 3;
            } else {
                u32 r = RandomIntInternal(&data_0209e650);
                *(int*)(c + 0x541c) = (r >> 8) % 0x18 + 1;
                *(int*)(c + 0x5418) = 1;
            }
        } else if (*(u8*)(c + 0x546a) != 0) {
            s16 vv = *(s16*)(state + 0x5e);
            if ((vv >= 0 && uu >= 0x5555u) || (vv < 0 && uu <= 0xaaabu)) {
                /* This spelling is load-bearing; all three parts were measured.
                   The two loads above share one base register through `state`,
                   but the ROM re-materialises `this + 0x5400` here, so these
                   stores must respell the full offset rather than reuse it.
                   `vv = -vv;` as its own statement orders the rsb before that
                   base add; storing `-vv` directly reverses the pair. And the
                   increment must be a plain read-modify-write -- writing it as
                   `+= 0x8000` makes mwcc CSE the field address into a register
                   and drop the displacement, which `opt_common_subs off` (in
                   force over this function) does not undo. */
                vv = -vv;
                *(s16*)(c + 0x545e) = vv;
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

    *(int*)(((long long)(int)(c + 0x541c))) += 1;
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- _ZN10dScMgCup_c19StatePrepareShuffleEv, 0x020dfcd8, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dScMgCup_c19StatePrepareShuffleEv
void dScMgCup_c::StatePrepareShuffle()
{
    char *o = (char *)this;
    int *p = (int *)(((int)o + 0x541c));
    *p = *p - 1;
    if (*(int *)(o + 0x5000 + 0x41c) > 0) return;
    func_ov006_020df024(o);
    if (*(unsigned char *)(o + 0x5000 + 0x46b) != 0) {
        unsigned char *q = (unsigned char *)(((int)o + 0x546b));
        *q = *q - 1;
    }
    *(int *)(o + 0x5000 + 0x418) = 2;
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN10dScMgCup_c10StateSetupEv, 0x020dfd48, size 0x18c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dScMgCup_c10StateSetupEv
void dScMgCup_c::StateSetup()
{
    char *c = (char *)this;
    int i;
    int r4;
    unsigned int r0;
    unsigned int r3;
    unsigned int r1;
    unsigned int r2;

    (*(int *)(((int)c + 0x541c)))--;
    if (*(int *)(c + 0x541c) > 0) {
        return;
    }

    for (i = 0; i < 3; i++) {
        *(unsigned char *)(c + i + 0x5465) = 0;
        func_ov006_020def80(c, i);
    }

    r4 = *(int *)(c + 0xb4);
    if (r4 < 0xa) {
        *(unsigned char *)(c + 0x5461) = (unsigned char)r4;
    } else {
        r0 = (unsigned int)RandomIntInternal(&data_0209e650);
        r3 = r0 >> 16;
        r1 = (r3 % 5) + 5;
        *(unsigned char *)(c + 0x5461) = (unsigned char)r1;
    }

    *(unsigned char *)(c + 0x5460) =
        ((unsigned char *)data_ov006_0213c094)[*(unsigned char *)(c + 0x5461) * 2];

    if (r4 > 3) {
        r0 = (unsigned int)RandomIntInternal(&data_0209e650);
        if (r0 & 1) {
            *(unsigned char *)(c + 0x5468) = 1;
        } else {
            *(unsigned char *)(c + 0x5468) = 2;
        }
    } else if (r4 == 3) {
        *(unsigned char *)(c + 0x5468) = 2;
    }

    *(unsigned char *)(c + 0x546b) = 0;
    *(unsigned char *)(c + 0x5469) = 0;
    *(int *)(c + 0x5418) = 1;
    *(int *)(c + 0x541c) = 0x1e;
    *(unsigned char *)(c + 0x546d) = 0xff;
    *(int *)(c + 0x5458) = 0;
    FreeGfxSlotsById(0x1d);

    r0 = (unsigned int)RandomIntInternal(&data_0209e650);
    r2 = (r0 % 10) + 1;
    *(unsigned char *)(c + 0x546c) = (unsigned char)r2;

    if (*(unsigned char *)(c + 0xc4) == 0) {
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(short *)(c + 0xc0) = 0;
    }
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN10dScMgCup_c9Virtual50Ev, 0x020dfed4, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10dScMgCup_c9Virtual50Ev
// recovered name: dScMgCup_c_Virtual50
/* recovered: renamed to Class_Method */
/* dScMgCup_c::Virtual50 - recovered from vtable slot identity */
int dScMgCup_c::Virtual50()
{
    char *p = (char *)this;

    func_ov006_020c2594(p + 0x4f38);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- _ZN10dScMgCup_c13OnYoshiTryEatEi, 0x020dfeec, size 0x17c */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN10dScMgCup_c6RenderEv, 0x020e0068, size 0x19c */
/* -------------------------------------------------------------------------- */
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
    int n;
    int j;
    int x;
    int y;
    int k;
    int v;

    for (i = 0; i < 3; i++) {
        list[i] = i;
    }

    for (n = 3; n > 1; n--) {
        for (j = 0; j < n - 1; j++) {
            y = list[j + 1];
            x = list[j];
            if (*(int*)(c + x * 8 + 0x53ec) < *(int*)(c + y * 8 + 0x53ec)) {
                list[j] = y;
                list[j + 1] = x;
            }
        }
    }

    for (k = 0; k < 3; k++) {
        v = list[k];
        func_ov006_020debb4(c + 0x50e8, (char)v);
        func_ov006_020deed8((int)c,
            (void *)data_ov006_0213c0d8[*(int*)(c + v * 4 + 0x5434)][((int*)c + k)[0x1510]].a,
            *(int*)(c + v * 8 + 0x53e8),
            ((struct P8*)c + v)[0xa7d].b,
            *(int*)(c + v * 4 + 0x540c),
            *(unsigned char*)(c + v + 0x5462));
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- _ZN10dScMgCup_c8BehaviorEv, 0x020e0204, size 0x104 */
/* -------------------------------------------------------------------------- */
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
 * address arithmetic and are not spellings that can be tidied. */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- _ZN10dScMgCup_c13InitResourcesEv, 0x020e0308, size 0x26c */
/* -------------------------------------------------------------------------- */
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

    ((VtObj *)c)->m18(3);
    func_ov006_020dec3c(c + 0x50e8);
    return 1;
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- dScMgCup_c_classInit, 0x020e0574, size 0xc0 */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_020e0634, 0x020e0634, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_020e0634
extern "C" void func_ov006_020e0634()
{
}
