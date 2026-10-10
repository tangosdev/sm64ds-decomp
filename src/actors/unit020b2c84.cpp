//cpp
/* unit020b2c84 -- the 44 functions of the ov004 linker unit 0x020b2c84..0x020b4aa4.
 *
 * This is the unlabelled run between the end of dScMgBase_c's unit and the unit
 * that starts at 0x020b4aa4. No class label, RTTI record or vtable falls in it,
 * so the file keeps the unit<addr> name. What the functions do, from their
 * bodies: the first group manages a table of 29 loaded files (func_ov004_020b2c84
 * frees them, func_ov004_020b2cb8 loads them by language, func_ov004_020b3278
 * decompresses one into a graphics slot), and the rest are the per-state
 * handlers of a small effect object whose layout the cartridge code assumes
 * (position words at +0x00..+0x0c, state at +0x20, timer at +0x24).
 *
 * Edges. 0x020b2c84 is the first function after func_ov004_020b2c80, the last
 * of the three destructor helpers (func_ov004_020b2c58, func_ov004_020b2c7c,
 * func_ov004_020b2c80) that __sinit_ov004_020b948c registers for the preceding
 * unit; those stay with dScMgBase_c. 0x020b4aa4 is the next function after
 * _ZN4ElemD1Ev, the last of the three destructor helpers
 * (_ZN4ElemC1Ev, __arraydtor$954, _ZN4ElemD1Ev) that
 * __sinit_ov004_020b955c registers for this unit's static array at
 * data_ov004_020bf648; __arraydtor$954 runs __cxa_vec_cleanup over it.
 * The unit emits one data object of its own, the anonymous initializer template
 * of func_ov004_020b2cb8's local array at 0x020bc6e8 (0xe8 bytes), which this
 * file's .data claim covers. Everything else it touches is reached by extern.
 *
 * Emission order. `#pragma defer_codegen off` is load-bearing here: with codegen
 * deferred the opt_* pragmas around func_ov004_020b2cb8, func_ov004_020b3b38,
 * func_ov004_020b4360 and func_ov004_020b45c0 do not bind to the one function
 * they bracket. Source is therefore ROM-ascending.
 *
 * The bodies were matched as one-function C or C++ sources and keep that
 * spelling under extern "C" and their ROM names. Two-word struct copies go
 * through W2 (an int[2] wrapper) so C++ keeps the ROM's block move.
 *
 * Retired one-function sources (ROM address order):
 *   0x020b2c84  func_ov004_020b2c84
 *   0x020b2cb8  func_ov004_020b2cb8
 *   0x020b3194  func_ov004_020b3194
 *   0x020b31b4  func_ov004_020b31b4
 *   0x020b321c  func_ov004_020b321c
 *   0x020b3278  func_ov004_020b3278
 *   0x020b35d8  func_ov004_020b35d8
 *   0x020b3698  func_ov004_020b3698
 *   0x020b369c  func_ov004_020b369c
 *   0x020b37c4  func_ov004_020b37c4
 *   0x020b37f0  func_ov004_020b37f0
 *   0x020b3834  func_ov004_020b3834
 *   0x020b3888  func_ov004_020b3888
 *   0x020b38ac  func_ov004_020b38ac
 *   0x020b3978  func_ov004_020b3978
 *   0x020b39a4  func_ov004_020b39a4
 *   0x020b3b38  func_ov004_020b3b38
 *   0x020b3c58  func_ov004_020b3c58
 *   0x020b3c9c  func_ov004_020b3c9c
 *   0x020b3cb8  func_ov004_020b3cb8
 *   0x020b3e9c  func_ov004_020b3e9c
 *   0x020b4080  func_ov004_020b4080
 *   0x020b40ac  func_ov004_020b40ac
 *   0x020b40c0  func_ov004_020b40c0
 *   0x020b410c  func_ov004_020b410c
 *   0x020b4214  func_ov004_020b4214
 *   0x020b422c  func_ov004_020b422c
 *   0x020b42c0  func_ov004_020b42c0
 *   0x020b433c  func_ov004_020b433c
 *   0x020b4360  func_ov004_020b4360
 *   0x020b45c0  func_ov004_020b45c0
 *   0x020b4820  func_ov004_020b4820
 *   0x020b484c  func_ov004_020b484c
 *   0x020b49b8  func_ov004_020b49b8
 *   0x020b49e4  func_ov004_020b49e4
 *   0x020b49f0  func_ov004_020b49f0
 *   0x020b4a1c  func_ov004_020b4a1c
 *   0x020b4a28  func_ov004_020b4a28
 *   0x020b4a40  func_ov004_020b4a40
 *   0x020b4a4c  func_ov004_020b4a4c
 *   0x020b4a64  func_ov004_020b4a64
 *   0x020b4a70  _ZN4ElemC1Ev
 *   0x020b4a7c  __arraydtor$954
 *   0x020b4aa0  _ZN4ElemD1Ev
 */

#pragma defer_codegen off

#include "types.h"

struct M { int _00, _01, _10, _11; };
struct Pair { int a, b; };
struct W2 { int w[2]; };
struct PairW { int w0, w1; };
struct S3 { int v[3]; };
struct Entry { int id; int ptr; };
struct EntryTable { Entry e[29]; };

/* Local views of the object each handler is handed, one per layout the
   cartridge code assumes. */
struct S433c { char pad24[0x24]; int f24; int f28; short f2c; };
struct S49b8 { char pad10[0x10]; short f10; short f12; short f14; char pad16[0x32 - 0x16]; short f32; };
struct Obj35d8 {
    char pad0[0x10];
    s16 f10;
    s16 f12;
    char pad1[0x18 - 0x14];
    int f18;
    int f1c;
    char pad2[0x34 - 0x20];
    int f34;
};

struct C31b4;
typedef void (C31b4::*PMF31b4)();
struct C31b4 {
    char pad[8];
    PMF31b4 pmf;
    char pad2[0x10];
    int state;
};
struct C321c;
typedef void (C321c::*PMF321c)();
struct C321c {
    PMF321c pmf;
    char pad[0x18];
    int state;
};

struct Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m10(); virtual int m11();
    virtual int m12(); virtual int m13(); virtual int m14(); virtual int m15();
    virtual int m16(); virtual int m17(); virtual int m18(); virtual int m19();
    virtual int m20(); virtual int m21(); virtual int m22(); virtual int m23();
    virtual int m24(); virtual int m25(); virtual int m26();
};
struct Base { virtual void dummy(); };
typedef void (Base::*PMF)();

/* The 3 records at data_ov004_020bf648 are 0x134 bytes each; the field at
   0x20 (29 while live) is all this unit writes in the constructor. The real
   class is not recovered. */
struct Elem {
    char pad[0x134];
    Elem();
    ~Elem() {}
};

extern int ApproachLinear(int &, int, int);

extern "C" {
void Deallocate(void *ptr);
int GetGameLanguage(void);
int LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
int func_ov004_020af5e0(int a, void *b, int c, int d);
int _ZN4cstd4fdivEii(int a, int b);
int func_02053200(int x);
unsigned int func_02012790(unsigned int x);
void __register_global_object(void *object, void *destructor, void **node);
void func_0203d704(int *o, int *a, int *b);
void func_0203d388(int *p, int angle);
int RandomIntInternal(int *seed);
void func_ov004_020adfc4(int c, short a1, int *a2, int *r3, int *sp0);
void NullDestructor_0203d47c(void);
void __cxa_vec_cleanup(void *, unsigned int, unsigned int, void (*)(void *));
void _Z14ApproachLinearRiii(int *r, int a, int b);
int _Z15ApproachLinear2Rsss(short *r, short a, short b);
void func_ov004_020b1c68(void *a0, int a1, int a2, int a3, int a4, M *a5);
void func_ov004_020b1cf0(int a, int b, int c, int d, int e);
int func_ov004_020b1aec(void);
void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
void func_ov004_020b42c0(char *c);
void func_ov004_020b37f0(int *obj);
void func_ov004_020b39a4(char *c);
}

extern s16 data_02082214[];
extern int data_0209e650;
extern Obj *data_ov004_020beb68;
extern S3 data_ov004_020bc27c;
extern int data_ov004_020b9f54;
static EntryTable data_ov004_020bc6e8 = { { { 0, 0 }, { 0x140, (int)&data_ov004_020b9f54 } } };
extern int data_ov006_021346bc;
extern int data_ov004_020beb6c;
extern int data_ov004_020bc150;
extern Pair data_ov004_020bc17c;
extern Pair data_ov004_020bc1ec;
extern Pair data_ov004_020bc224;
extern Pair data_ov004_020bc274;
extern int data_ov004_020bc1b4[];
extern int data_ov004_020bc20c[];
extern int data_ov004_020bc254[];
extern unsigned char data_ov004_020bf3e8[];
extern int data_ov004_020bf3ec;
extern int data_ov004_020bf3f0;
extern int data_ov004_020bf3f4[2];
extern int data_ov004_020bf3fc[2];
extern void *data_ov004_020bf404[3];
extern void *data_ov004_020bf410[3];
extern PairW data_ov004_020bf428[];
extern PairW data_ov004_020bf490[];
extern PairW data_ov004_020bf4f8[];
extern int data_ov004_020bf560[];
extern int data_ov004_020bf5d4[];

extern int data_ov004_020bc42c[];
extern int data_ov004_020bc65c[];
extern int data_ov004_020bc418[];
extern int data_ov004_020bc648[];
extern int data_ov004_020bc3c8[];
extern int data_ov004_020bc5e4[];
extern int data_ov004_020bc4f4[];
extern int data_ov004_020bc300[];
extern int data_ov004_020bc3dc[];
extern int data_ov004_020bc5f8[];
extern int data_ov004_020bc3f0[];
extern int data_ov004_020bc60c[];
extern int data_ov004_020bc404[];
extern int data_ov004_020bc634[];
extern int data_ov004_020bc3b4[];
extern int data_ov004_020bc5bc[];
extern int data_ov004_020bc468[];
extern int data_ov004_020bc6ac[];
extern int data_ov004_020bc454[];
extern int data_ov004_020bc698[];
extern int data_ov004_020bc378[];
extern int data_ov004_020bc594[];
extern int data_ov004_020bc620[];
extern int data_ov004_020bc670[];
extern int data_ov004_020bc440[];
extern int data_ov004_020bc684[];
extern int data_ov004_020bc5d0[];
extern int data_ov004_020bc3a0[];
extern int data_ov004_020bc47c[];
extern int data_ov004_020bc288[];
extern int data_ov004_020bc490[];
extern int data_ov004_020bc29c[];
extern int data_ov004_020bc580[];
extern int data_ov004_020bc350[];
extern int data_ov004_020bc4a4[];
extern int data_ov004_020bc2b0[];
extern int data_ov004_020bc4b8[];
extern int data_ov004_020bc2c4[];
extern int data_ov004_020bc4cc[];
extern int data_ov004_020bc2d8[];
extern int data_ov004_020bc4e0[];
extern int data_ov004_020bc2ec[];
extern int data_ov004_020bc38c[];
extern int data_ov004_020bc508[];
extern int data_ov004_020bc314[];
extern int data_ov004_020bc51c[];
extern int data_ov004_020bc328[];
extern int data_ov004_020bc530[];
extern int data_ov004_020bc6d4[];
extern int data_ov004_020bc544[];
extern int data_ov004_020bc6c0[];
extern int data_ov004_020bc5a8[];
extern int data_ov004_020bc33c[];
extern int data_ov004_020bc558[];
extern int data_ov004_020bc364[];
extern int data_ov004_020bc56c[];


// @symbol func_ov004_020b2c84
extern "C" void func_ov004_020b2c84(void) {
    int i;
    for (i = 0; i < 0x1d; i++) {
        Deallocate((void *)data_ov004_020bf560[i]);
        data_ov004_020bf560[i] = 0;
    }
}

// @symbol func_ov004_020b2cb8
#pragma opt_strength_reduction off
extern "C" void func_ov004_020b2cb8(void)
{
    int i;
    /* The ROM's template for this array is at 0x020bc6e8 and is all zero except
       entry 1. What stood here was 29 invented pairs {1,100}, {2,101} ... {29,128};
       none of them were ever compared to anything, because mwccarm puts a local
       initializer in an anonymous .rodata object (`@7`) that objisolate drops and
       match.py wildcards the relocation to.

       It matters for exactly one entry. Every index except 1 is overwritten below
       before the loop reads it -- entry 1 is not, so its initial value is live, and
       it was wrong. */
    EntryTable entries = data_ov004_020bc6e8;

    entries.e[0].id = data_ov004_020bc42c[GetGameLanguage()];
    entries.e[0].ptr = data_ov004_020bc65c[GetGameLanguage()];
    entries.e[2].id = data_ov004_020bc418[GetGameLanguage()];
    entries.e[2].ptr = data_ov004_020bc648[GetGameLanguage()];
    entries.e[3].id = data_ov004_020bc3c8[GetGameLanguage()];
    entries.e[3].ptr = data_ov004_020bc5e4[GetGameLanguage()];
    entries.e[4].id = data_ov004_020bc4f4[GetGameLanguage()];
    entries.e[4].ptr = data_ov004_020bc300[GetGameLanguage()];
    entries.e[5].id = data_ov004_020bc3dc[GetGameLanguage()];
    entries.e[5].ptr = data_ov004_020bc5f8[GetGameLanguage()];
    entries.e[6].id = data_ov004_020bc3f0[GetGameLanguage()];
    entries.e[6].ptr = data_ov004_020bc60c[GetGameLanguage()];
    entries.e[7].id = data_ov004_020bc404[GetGameLanguage()];
    entries.e[7].ptr = data_ov004_020bc634[GetGameLanguage()];
    entries.e[8].id = data_ov004_020bc3b4[GetGameLanguage()];
    entries.e[8].ptr = data_ov004_020bc5bc[GetGameLanguage()];
    entries.e[9].id = data_ov004_020bc468[GetGameLanguage()];
    entries.e[9].ptr = data_ov004_020bc6ac[GetGameLanguage()];
    entries.e[10].id = data_ov004_020bc454[GetGameLanguage()];
    entries.e[10].ptr = data_ov004_020bc698[GetGameLanguage()];
    entries.e[11].id = data_ov004_020bc378[GetGameLanguage()];
    entries.e[11].ptr = data_ov004_020bc594[GetGameLanguage()];
    entries.e[12].id = data_ov004_020bc620[GetGameLanguage()];
    entries.e[12].ptr = data_ov004_020bc670[GetGameLanguage()];
    entries.e[13].id = data_ov004_020bc440[GetGameLanguage()];
    entries.e[13].ptr = data_ov004_020bc684[GetGameLanguage()];
    entries.e[14].id = data_ov004_020bc5d0[GetGameLanguage()];
    entries.e[14].ptr = data_ov004_020bc3a0[GetGameLanguage()];
    entries.e[15].id = data_ov004_020bc47c[GetGameLanguage()];
    entries.e[15].ptr = data_ov004_020bc288[GetGameLanguage()];
    entries.e[16].id = data_ov004_020bc490[GetGameLanguage()];
    entries.e[16].ptr = data_ov004_020bc29c[GetGameLanguage()];
    entries.e[17].id = data_ov004_020bc580[GetGameLanguage()];
    entries.e[17].ptr = data_ov004_020bc350[GetGameLanguage()];
    entries.e[18].id = data_ov004_020bc4a4[GetGameLanguage()];
    entries.e[18].ptr = data_ov004_020bc2b0[GetGameLanguage()];
    entries.e[19].id = data_ov004_020bc4b8[GetGameLanguage()];
    entries.e[19].ptr = data_ov004_020bc2c4[GetGameLanguage()];
    entries.e[20].id = data_ov004_020bc4cc[GetGameLanguage()];
    entries.e[20].ptr = data_ov004_020bc2d8[GetGameLanguage()];
    entries.e[21].id = data_ov004_020bc4e0[GetGameLanguage()];
    entries.e[21].ptr = data_ov004_020bc2ec[GetGameLanguage()];
    entries.e[22].id = data_ov004_020bc38c[GetGameLanguage()];
    entries.e[22].ptr = data_ov004_020bc508[GetGameLanguage()];
    entries.e[23].id = data_ov004_020bc314[GetGameLanguage()];
    entries.e[23].ptr = data_ov004_020bc51c[GetGameLanguage()];
    entries.e[24].id = data_ov004_020bc328[GetGameLanguage()];
    entries.e[24].ptr = data_ov004_020bc530[GetGameLanguage()];
    entries.e[25].id = data_ov004_020bc6d4[GetGameLanguage()];
    entries.e[25].ptr = data_ov004_020bc544[GetGameLanguage()];
    entries.e[26].id = data_ov004_020bc6c0[GetGameLanguage()];
    entries.e[26].ptr = data_ov004_020bc5a8[GetGameLanguage()];
    entries.e[27].id = data_ov004_020bc33c[GetGameLanguage()];
    entries.e[27].ptr = data_ov004_020bc558[GetGameLanguage()];
    entries.e[28].id = data_ov004_020bc364[GetGameLanguage()];
    entries.e[28].ptr = data_ov004_020bc56c[GetGameLanguage()];

    for (i = 0; i < 29; i++) {
        int t;
        t = LoadFile(entries.e[i].id);
        ((volatile int *)data_ov004_020bf560)[i] = t;
        t = (int)&entries.e[i];
        t = *(volatile int *)(t + 4);
        t = *(int *)t;
        data_ov004_020bf5d4[i] = t;
    }
}
#pragma opt_strength_reduction on

// @symbol func_ov004_020b3194
extern "C" void func_ov004_020b3194(void *c)
{
    *(int*)((char*)c+0x20) = 0x1d;
    s16 idx = *(s16*)((char*)c+0x30);
    data_ov004_020bf3e8[idx] = 0;
}

// @symbol func_ov004_020b31b4
extern "C" void func_ov004_020b31b4(C31b4* c) {
    if (c->state == 0x1d) return;
    if (c->pmf != 0) {
        (c->*(c->pmf))();
        return;
    }
    func_ov004_020b42c0((char*)c);
}

// @symbol func_ov004_020b321c
extern "C" void func_ov004_020b321c(C321c* c) {
    if (c->state == 0x1d) return;
    if (c->pmf == 0) return;
    (c->*(c->pmf))();
}

// @symbol func_ov004_020b3278
extern "C" void func_ov004_020b3278(char *self, int arg1, short arg2, short arg3, int arg4, int arg5, short arg6)
{
    int a, b;

    if (data_ov004_020beb68->m26() == 2) {
        a = 0x6400000;
        b = 0;
    } else {
        a = 0x6600000;
        b = 0x6400000;
    }

    switch (arg1) {
    case 3: case 4: case 5: case 6:
    case 8: case 9: case 10: case 11: case 12:
    case 14: case 16: case 17: case 18: case 19: case 20: case 21:
        if (data_ov004_020bf3e8[0] != 0)
            return;
        DecompressLZ16((void *)data_ov004_020bf560[arg1], (void *)(a + 0x7000));
        if (b != 0)
            DecompressLZ16((void *)data_ov004_020bf560[arg1], (void *)(b + 0x7000));
        *(short *)(self + 0x30) = 0;
        data_ov004_020bf3e8[0] = 1;
        break;
    default:
        if (data_ov004_020bf3e8[1] == 0) {
            DecompressLZ16((void *)data_ov004_020bf560[arg1], (void *)(a + 0x6000));
            if (b != 0)
                DecompressLZ16((void *)data_ov004_020bf560[arg1], (void *)(b + 0x6000));
            *(short *)(self + 0x30) = 1;
            data_ov004_020bf3e8[1] = 1;
            break;
        }
        if (data_ov004_020bf3e8[2] == 0) {
            DecompressLZ16((void *)data_ov004_020bf560[arg1], (void *)(a + 0x6800));
            if (b != 0)
                DecompressLZ16((void *)data_ov004_020bf560[arg1], (void *)(b + 0x6800));
            *(short *)(self + 0x30) = 2;
            data_ov004_020bf3e8[2] = 1;
            break;
        }
        return;
    }

    *(int *)(self + 0x20) = arg1;
    *(short *)(self + 0x10) = arg2;
    *(short *)(self + 0x12) = arg3;
    *(short *)(self + 0x14) = *(short *)(self + 0x10);
    *(short *)(self + 0x16) = *(short *)(self + 0x12);
    *(int *)(self + 0x1c) = arg4;
    *(int *)(self + 0x18) = arg5;
    *(short *)(self + 0x32) = 0;

    if (arg6 != 0xd) {
        *(short *)(self + 0x2e) = arg6;
    } else {
        switch (arg1) {
        case 11:
            *(short *)(self + 0x2e) = 7;
            break;
        case 3: case 4: case 5: case 6: case 20: case 21:
            *(short *)(self + 0x2e) = 8;
            break;
        case 8: case 14:
            *(short *)(self + 0x2e) = 9;
            break;
        case 0:
            *(short *)(self + 0x2e) = 3;
            break;
        case 1: case 2:
            *(short *)(self + 0x2e) = 4;
            break;
        case 13:
            *(short *)(self + 0x2e) = 0xc;
            break;
        default:
            *(short *)(self + 0x2e) = 0;
            break;
        }
    }

    {
        S3 tmp = data_ov004_020bc27c;
        *(short *)(self + 0x2c) = (short)func_ov004_020af5e0(
            data_ov004_020bf5d4[*(int *)(self + 0x20)],
            self + 0x34,
            tmp.v[*(short *)(self + 0x30)],
            *(int *)(self + 0x20));
    }

    {
        int st = *(short *)(self + 0x2e);
        if (((PMF *)data_ov004_020bf490)[st])
            (((Base *)self)->*((PMF *)data_ov004_020bf490)[st])();
    }

    {
        short st;
        PairW *e;
        st = *(short *)(self + 0x2e);
        e = &data_ov004_020bf428[st];
        a = data_ov004_020bf428[st].w0;
        b = e->w1;
        *(int *)(self + 0) = b ? a : a;
        *(int *)(self + 4) = b;
        st = *(short *)(self + 0x2e);
        e = &data_ov004_020bf4f8[st];
        a = data_ov004_020bf4f8[st].w0;
        b = e->w1;
        *(int *)(self + 8) = b ? a : a;
        *(int *)(self + 0xc) = b;
    }
}

// @symbol func_ov004_020b35d8
extern "C" void func_ov004_020b35d8(Obj35d8 *self)
{
  int x;
  int y = self->f10;
  int n;
  s16 new_var2;
  int v = (int) (((unsigned int) (self->f34 << 7)) >> 0x17);
  int w;
  unsigned long new_var;
  n = func_ov004_020b1aec() + 1;
  if (v > 0x100)
  {
    v -= 0x200;
  }
  if (v < 0)
  {
    v = -v;
  }
  x = (x = (self->f10 + v) + 0x10);
  w = 0x10;
  {
    int p = 1;
    while (p < n)
    {
      p *= 10;
      w += 0x10;
    }

  }
  x = x - (w >> 1);
  new_var = w >> 1;
  new_var = y - new_var;
  func_ov004_020b1cf0((int) (&self->f34), new_var, self->f12, self->f1c, self->f18);
  new_var2 = self->f12;
  func_ov004_020b2444(x, new_var2, n, self->f1c, self->f18, 2, 0);
}

// @symbol func_ov004_020b3698
extern "C" void func_ov004_020b3698(void)
{
}

// @symbol func_ov004_020b369c
extern "C" void func_ov004_020b369c(char* self) {
    char* el = self + 0x34;
    int angle = (u16)*(int*)(self + 0x24);
    M m;
    u16 flag;
    do {
        int s0 = data_02082214[(angle >> 4) * 2];
        int mm = (int)(((s64)s0 * 0x800 + 0x800) >> 12);
        int idx = (u16)mm >> 4;
        int cos1 = data_02082214[idx * 2 + 1];
        int sin1 = data_02082214[idx * 2];
        int fc = (int)(((s64)cos1 * 0x1000 + 0x800) >> 12);
        int fs = (int)(((s64)sin1 * 0x1000 + 0x800) >> 12);
        m._00 = fc;
        m._11 = fc;
        m._01 = fs;
        m._10 = -fs;
        func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                            *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
        angle = (u16)(angle + 0x4000);
        flag = *(u16*)(el + 6);
        el += 8;
    } while (flag != 0xffff);
}

// @symbol func_ov004_020b37c4
extern "C" void func_ov004_020b37c4(char *c) {
    if (ApproachLinear(*(int*)(c + 0x24), *(int*)(c + 0x28), 0x1000)) {
        *(int*)(c + 0x24) = 0;
    }
}

// @symbol func_ov004_020b37f0
extern "C" void func_ov004_020b37f0(int* r0){
  char* c=(char*)r0;
  *(int*)(c+0x24)=0;
  *(int*)(c+0x28)=0x27000;
  *(W2*)(c+0)=*(W2*)data_ov004_020bc20c;
  *(W2*)(c+8)=*(W2*)data_ov004_020bc1b4;
}

// @symbol func_ov004_020b3834
extern "C" void func_ov004_020b3834(int* obj){
    char* c = (char*)obj;
    if (*(int*)(c + 0x24) < 0x30){
        *(unsigned*)(c + 0x24) += 1;
        return;
    }
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x12), *(short*)(c + 0x16), 0x10)){
        func_ov004_020b37f0(obj);
    }
}

// @symbol func_ov004_020b3888
extern "C" void func_ov004_020b3888(char* r0) {
    *(int*)(r0+0x24) = 0;
    *(short*)(r0+0x12) = -(data_ov004_020beb6c + 0x60);
}

// @symbol func_ov004_020b38ac
extern "C" void func_ov004_020b38ac(char* self) {
    char* el = self + 0x34;
    int x = *(int*)(self + 0x24) >> 2;
    int t = 0x1000;
    int base = t;

    u16 flag;
    do {
        int v = base;
        int d = x - t;
        if (d < 0) d = -d;
        if (d < 0x1000) v += (int)(((s64)(0x1000 - d) * 0x400 + 0x800) >> 12);
        v = func_02053200(v);
        {
            M m = {0};
            m._00 = v;
            m._11 = v;
            func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
        }
        t += 0x1000;
        flag = *(u16*)(el + 6);
        el += 8;
    } while (flag != 0xffff);
}

// @symbol func_ov004_020b3978
extern "C" void func_ov004_020b3978(char *c) {
    if (ApproachLinear(*(int*)(c + 0x24), *(int*)(c + 0x28), 0x1000)) {
        *(int*)(c + 0x24) = 0;
    }
}

// @symbol func_ov004_020b39a4
extern "C" void func_ov004_020b39a4(char* c) {
    int i;
    int angle;
    int boxA[2], boxB[2], boxC[2];
    int posA[2];
    int outA[2], outB[2];
    int flags;

    int shifted;

    *(int*)(c + 0x24) = 0;
    angle = *(short*)(c + 0x2c);
    angle = angle + 1;
    shifted = angle << 14;
    *(int*)(c + 0x28) = shifted;

    *(W2*)(c + 0)   = *(W2*)&data_ov004_020bc17c;
    *(W2*)(c + 8)   = *(W2*)&data_ov004_020bc1ec;

    boxA[0] = 0; boxA[1] = -0x2000;
    boxB[0] = 0; boxB[1] = -0x1800;
    boxC[0] = 0; boxC[1] = -0x4000;

    flags = data_ov004_020bf3ec;

    {
        int y12 = *(short*)(c + 0x12);
        int y10 = *(short*)(c + 0x10);
        int sY12, sY10;
        flags = flags & 1;
        sY12 = y12 << 12;
        sY10 = y10 << 12;
        posA[0] = sY10;
        posA[1] = sY12;
    }

    if (flags == 0) {
        data_ov004_020bf3f4[0] = 0;
        data_ov004_020bf3f4[1] = 0xc0;
        __register_global_object(data_ov004_020bf3f4, (void *)NullDestructor_0203d47c, data_ov004_020bf410);
        data_ov004_020bf3ec = data_ov004_020bf3ec | 1;
    }

    for (i = 0; i < 0x20; i++) {
        int r;
        int ang;
        s16 spawnAngle;

        func_0203d704(outA, posA, boxC);
        func_0203d388(boxA, 0x800);
        func_0203d388(boxC, 0x800);
        func_0203d704(outB, boxB, boxA);

        r = RandomIntInternal(&data_0209e650);
        r &= 0x7fffffff;
        ang = (int)((u32)r >> 0x13) * 0x3c >> 0xc;
        ang = ang + 0x5a;
        spawnAngle = (s16)ang;

        func_ov004_020adfc4(0, spawnAngle, outA, outB, data_ov004_020bf3f4);
    }
}

// @symbol func_ov004_020b3b38
#pragma opt_propagation off
extern "C" void func_ov004_020b3b38(char* c) {
    int m, fd, t, v;
    unsigned short term;
    if (*(int*)(c + 0x24) == 0) return;
    m = (int)(((s64)*(int*)(c + 0x24) * 0xcc + 0x800) >> 12);
    {
        char* e = c + 0x34;
        fd = _ZN4cstd4fdivEii(m, 0x1000);
        t = (int)(((s64)fd * 0x1000 + 0x800) >> 12);
        v = func_02053200(m);
        {
            M buf = {0};
            buf._00 = v;
            buf._11 = v;
            t = 0x1000 - t;
            do {
                s16 f = (s16)(((unsigned int)(*(int*)e << 7)) >> 23);
                int off;
                if (f > 0x100) f -= 0x200;
                off = f * t;
                func_ov004_020b1c68(e,
                    *(s16*)(c + 0x10) - ((off << 4) >> 16),
                    *(s16*)(c + 0x12), *(int*)(c + 0x1c),
                    *(int*)(c + 0x18), &buf);
                term = *(unsigned short*)(e + 6);
                e += 8;
            } while (term != 0xffff);
        }
    }
}
#pragma opt_propagation on

// @symbol func_ov004_020b3c58
extern "C" void func_ov004_020b3c58(char *c)
{
    _Z14ApproachLinearRiii((int*)(c+0x24), *(int*)(c+0x28), 0x1000);
    if (_Z15ApproachLinear2Rsss((short*)(c+0x12), *(short*)(c+0x16), 0x10) == 0)
        return;
    func_ov004_020b39a4(c);
}

// @symbol func_ov004_020b3c9c
extern "C" void func_ov004_020b3c9c(char *p)
{
    *(int *)(p + 0x24) = 0;
    *(int *)(p + 0x28) = 81920;
    *(short *)(p + 0x12) = 96;
}

// @symbol func_ov004_020b3cb8
extern "C" void func_ov004_020b3cb8(char* self) {
    u16 flag;
    if (*(int*)(self + 0x24) == 0) return;
    {
        char* el = self + 0x34;
        if (*(int*)(self + 0x24) < 0x18000) {
            int v = 0x2000;
            if (*(int*)(self + 0x24) >= 0x14000) {
                v = 0x1000;
            } else {
                int t = _ZN4cstd4fdivEii(0x1000, 0x18000);
                v -= (int)(((s64)*(int*)(self + 0x24) * t + 0x800) >> 12);
                v = func_02053200(v);
            }
            {
                M m = {0};
                m._00 = v;
                m._11 = v;
                do {
                    func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                        *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
                    flag = *(u16*)(el + 6);
                    el += 8;
                } while (flag != 0xffff);
            }
        } else {
            int angle = (u16)(*(int*)(self + 0x24) - 0x18000);
            M m2;
            do {
                int s0 = data_02082214[(angle >> 4) * 2];
                int mm = (int)(((s64)s0 * 0x800 + 0x800) >> 12);
                int idx = (u16)mm >> 4;
                int cos1 = data_02082214[idx * 2 + 1];
                int sin1 = data_02082214[idx * 2];
                int fc = (int)(((s64)cos1 * 0x1000 + 0x800) >> 12);
                int fs = (int)(((s64)sin1 * 0x1000 + 0x800) >> 12);
                m2._00 = fc;
                m2._11 = fc;
                m2._01 = fs;
                m2._10 = -fs;
                func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                    *(int*)(self + 0x1c), *(int*)(self + 0x18), &m2);
                angle = (u16)(angle + 0x4000);
                flag = *(u16*)(el + 6);
                el += 8;
            } while (flag != 0xffff);
        }
    }
}

// @symbol func_ov004_020b3e9c
extern "C" void func_ov004_020b3e9c(char* self) {
    u16 flag;
    if (*(int*)(self + 0x24) == 0) return;
    {
        char* el = self + 0x34;
        if (*(int*)(self + 0x24) < 0x18000) {
            int v = 0x2000;
            if (*(int*)(self + 0x24) >= 0x14000) {
                v = 0x1000;
            } else {
                int t = _ZN4cstd4fdivEii(0x1000, 0x18000);
                v -= (int)(((s64)*(int*)(self + 0x24) * t + 0x800) >> 12);
                v = func_02053200(v);
            }
            {
                M m = {0};
                m._00 = v;
                m._11 = v;
                do {
                    func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                        *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
                    flag = *(u16*)(el + 6);
                    el += 8;
                } while (flag != 0xffff);
            }
        } else {
            int angle = (u16)(*(int*)(self + 0x24) - 0x18000);
            M m2;
            do {
                int s0 = data_02082214[(angle >> 4) * 2];
                int mm = (int)(((s64)s0 * 0x800 + 0x800) >> 12);
                int idx = (u16)mm >> 4;
                int cos1 = data_02082214[idx * 2 + 1];
                int sin1 = data_02082214[idx * 2];
                int fc = (int)(((s64)cos1 * 0x1000 + 0x800) >> 12);
                int fs = (int)(((s64)sin1 * 0x1000 + 0x800) >> 12);
                m2._00 = fc;
                m2._11 = fc;
                m2._01 = fs;
                m2._10 = -fs;
                func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                    *(int*)(self + 0x1c), *(int*)(self + 0x18), &m2);
                angle = (u16)(angle + 0x4000);
                flag = *(u16*)(el + 6);
                el += 8;
            } while (flag != 0xffff);
        }
    }
}

// @symbol func_ov004_020b4080
extern "C" void func_ov004_020b4080(char *c) {
    if (ApproachLinear(*(int*)(c + 0x24), *(int*)(c + 0x28), 0x1000)) {
        *(int*)(c + 0x24) = 98304;
    }
}

// @symbol func_ov004_020b40ac
extern "C" void func_ov004_020b40ac(char *p)
{
    *(int *)(p + 0x24) = 0;
    *(int *)(p + 0x28) = 159744;
}

// @symbol func_ov004_020b40c0
extern "C" int func_ov004_020b40c0(int* o){
  int* d = data_ov004_020bc254;
  if(o[0] == d[0] && (o[1] == d[1] || o[0] == 0) && o[9] == o[10])
    return 1;
  return 0;
}

// @symbol func_ov004_020b410c
extern "C" void func_ov004_020b410c(char* self)
{
    char* el = self + 0x34;
    int x = *(int*)(self + 0x24);
    int idx = (int)(((unsigned int)x << 15) >> 16) >> 4;
    s16 s = *(s16*)((int)data_02082214 + (idx << 2));
    int v = func_02053200((int)((((s64)s << 11) + 0x800) >> 12) + 0x1000);
    M m = {0};
    u16 lim = 0xffff;
    u16 flag;
    m._00 = v;
    m._11 = v;
    do {
        func_ov004_020b1c68(el,
            *(s16*)(self + 0x10), *(s16*)(self + 0x12),
            *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
        flag = *(u16*)(el + 6);
        el += 8;
    } while (flag != lim);

    x = *(int*)(self + 0x20);
    if (x != 0 && x != 1 && x != 2)
        return;
    func_ov004_020b1cf0(data_ov006_021346bc,
        *(s16*)(self + 0x10), *(s16*)(self + 0x12),
        *(int*)(self + 0x1c), *(int*)(self + 0x18));
}

// @symbol func_ov004_020b4214
extern "C" void func_ov004_020b4214(char* p) {
    _Z14ApproachLinearRiii((int*)(p + 0x24), *(int*)(p + 0x28), 0x1000);
}

// @symbol func_ov004_020b422c
extern "C" void func_ov004_020b422c(char* r4){
  data_ov004_020bc150 = 0;
  switch (*(int*)(r4 + 0x20)) {
  case 0:
    func_02012790(0x62);
    break;
  case 1:
  case 2:
    func_02012790(0x63);
    break;
  }
  *(int*)(r4 + 0x24) = 0;
  *(int*)(r4 + 0x28) = 0x10000;
  *(W2*)r4 = *(W2*)&data_ov004_020bc224;
  *(W2*)(r4 + 8) = *(W2*)&data_ov004_020bc274;
}

// @symbol func_ov004_020b42c0
extern "C" void func_ov004_020b42c0(char *c) {
    func_ov004_020b1cf0((int)(c + 0x34), *(short*)(c + 0x10), *(short*)(c + 0x12), *(int*)(c + 0x1c), *(int*)(c + 0x18));
    if (*(int*)(c + 0x20) != 0 && *(int*)(c + 0x20) != 1 && *(int*)(c + 0x20) != 2)
        return;
    func_ov004_020b1cf0(data_ov006_021346bc, *(short*)(c + 0x10), *(short*)(c + 0x12), *(int*)(c + 0x1c), *(int*)(c + 0x18));
}

// @symbol func_ov004_020b433c
extern "C" void func_ov004_020b433c(S433c* s) {
    s->f24 = 0;
    s->f28 = ((s->f2c + 1) << 2) + 0x1e << 0xc;
}

// @symbol func_ov004_020b4360
#pragma opt_propagation off
extern "C" void func_ov004_020b4360(char* self) {
    u16 flag;
    int val = *(int*)(self + 0x24);
    if (val == 0) return;
    {
        char* el = self + 0x34;
        if (val < 0x1e000) {
            int d;
            int scale;
            if (val < 0x14000) {
                int t;
                d = (int)(((s64)val * 0x100 + 0x800) >> 12);
                t = _ZN4cstd4fdivEii(d, 0x1400);
                scale = (int)(((s64)t * 0x1400 + 0x800) >> 12);
            } else {
                int a = (int)(((s64)(val - 0x14000) * 0x66 + 0x800) >> 12);
                int b = 0x1400 - a;
                int t = _ZN4cstd4fdivEii(b - 0x1000, 0x400);
                scale = (0x1000 - t) + (int)(((s64)t * 0x1400 + 0x800) >> 12);
                d = b;
            }
            d = func_02053200(d);
            {
                M m = {0};
                m._00 = d;
                m._11 = d;
                scale = 0x1000 - scale;
                do {
                    s16 f = (s16)(((unsigned int)(*(int*)el << 7)) >> 23);
                    int off;
                    if (f > 0x100) f -= 0x200;
                    off = f * scale;
                    func_ov004_020b1c68(el,
                                        *(s16*)(self + 0x10) - ((off << 4) >> 16),
                                        *(s16*)(self + 0x12), *(int*)(self + 0x1c),
                                        *(int*)(self + 0x18), &m);
                    flag = *(u16*)(el + 6);
                    el += 8;
                } while (flag != 0xffff);
            }
        } else {
            int idx = (val - 0x1e000) >> 2;
            int i = 0x1000;
            do {
                int diff = idx - i;
                int r = 0x1000;
                if (diff < 0) diff = -diff;
                if (diff < 0x1000) {
                    r += (int)(((s64)(0x1000 - diff) * 0x400 + 0x800) >> 12);
                }
                r = func_02053200(r);
                {
                    M m2 = {0};
                    m2._00 = r;
                    m2._11 = r;
                    func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                        *(int*)(self + 0x1c), *(int*)(self + 0x18), &m2);
                }
                i += 0x1000;
                flag = *(u16*)(el + 6);
                el += 8;
            } while (flag != 0xffff);
        }
    }
}
#pragma opt_propagation on

// @symbol func_ov004_020b45c0
#pragma opt_propagation off
extern "C" void func_ov004_020b45c0(char* self) {
    u16 flag;
    int val = *(int*)(self + 0x24);
    if (val == 0) return;
    {
        char* el = self + 0x34;
        if (val < 0x1e000) {
            int d;
            int scale;
            if (val < 0x14000) {
                int t;
                d = (int)(((s64)val * 0x100 + 0x800) >> 12);
                t = _ZN4cstd4fdivEii(d, 0x1400);
                scale = (int)(((s64)t * 0x1400 + 0x800) >> 12);
            } else {
                int a = (int)(((s64)(val - 0x14000) * 0x66 + 0x800) >> 12);
                int b = 0x1400 - a;
                int t = _ZN4cstd4fdivEii(b - 0x1000, 0x400);
                scale = (0x1000 - t) + (int)(((s64)t * 0x1400 + 0x800) >> 12);
                d = b;
            }
            d = func_02053200(d);
            {
                M m = {0};
                m._00 = d;
                m._11 = d;
                scale = 0x1000 - scale;
                do {
                    s16 f = (s16)(((unsigned int)(*(int*)el << 7)) >> 23);
                    int off;
                    if (f > 0x100) f -= 0x200;
                    off = f * scale;
                    func_ov004_020b1c68(el,
                                        *(s16*)(self + 0x10) - ((off << 4) >> 16),
                                        *(s16*)(self + 0x12), *(int*)(self + 0x1c),
                                        *(int*)(self + 0x18), &m);
                    flag = *(u16*)(el + 6);
                    el += 8;
                } while (flag != 0xffff);
            }
        } else {
            int idx = (val - 0x1e000) >> 2;
            int i = 0x1000;
            do {
                int diff = idx - i;
                int r = 0x1000;
                if (diff < 0) diff = -diff;
                if (diff < 0x1000) {
                    r += (int)(((s64)(0x1000 - diff) * 0x400 + 0x800) >> 12);
                }
                r = func_02053200(r);
                {
                    M m2 = {0};
                    m2._00 = r;
                    m2._11 = r;
                    func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                        *(int*)(self + 0x1c), *(int*)(self + 0x18), &m2);
                }
                i += 0x1000;
                flag = *(u16*)(el + 6);
                el += 8;
            } while (flag != 0xffff);
        }
    }
}
#pragma opt_propagation on

// @symbol func_ov004_020b4820
extern "C" void func_ov004_020b4820(char *c) {
    if (ApproachLinear(*(int*)(c + 0x24), *(int*)(c + 0x28), 0x1000)) {
        *(int*)(c + 0x24) = 122880;
    }
}

// @symbol func_ov004_020b484c
extern "C" void func_ov004_020b484c(char* c)
{
    int i;
    int boxA[2], boxB[2], boxC[2];
    int posA[2];
    int outA[2], outB[2];
    int flags;
    int r;
    int ang;
    s16 spawnAngle;

    *(int*)(c + 0x24) = 0;
    *(int*)(c + 0x28) = (((*(s16*)(c + 0x2c) + 1) << 2) + 0x1e) << 0xc;

    boxA[0] = 0;
    boxA[1] = -0x2000;
    boxB[0] = 0;
    boxB[1] = -0x1800;
    boxC[0] = 0;
    boxC[1] = -0x4000;

    flags = data_ov004_020bf3f0;

    {
        int y12 = *(s16*)(c + 0x12);
        int y10 = *(s16*)(c + 0x10);
        int sY12, sY10;
        flags = flags & 1;
        sY12 = y12 << 12;
        sY10 = y10 << 12;
        posA[0] = sY10;
        posA[1] = sY12;
    }

    if (flags == 0) {
        data_ov004_020bf3fc[0] = 0;
        data_ov004_020bf3fc[1] = 0xc0;
        __register_global_object(data_ov004_020bf3fc, (void*)NullDestructor_0203d47c, data_ov004_020bf404);
        data_ov004_020bf3f0 = data_ov004_020bf3f0 | 1;
    }

    for (i = 0; i < 0x20; i++) {
        func_0203d704(outA, posA, boxC);
        func_0203d388(boxA, 0x800);
        func_0203d388(boxC, 0x800);
        func_0203d704(outB, boxB, boxA);

        r = RandomIntInternal(&data_0209e650);
        r &= 0x7fffffff;
        ang = (int)((u32)r >> 0x13) * 0x3c >> 0xc;
        ang = ang + 0x5a;
        spawnAngle = (s16)ang;

        func_ov004_020adfc4(0, spawnAngle, outA, outB, data_ov004_020bf3fc);
    }
}

// @symbol func_ov004_020b49b8
extern "C" void func_ov004_020b49b8(S49b8* s) {
    if (_Z15ApproachLinear2Rsss((short*)((char*)s + 0x10), s->f14, 0x10))
        s->f32 = 1;
}

// @symbol func_ov004_020b49e4
extern "C" void func_ov004_020b49e4(short *p)
{
    p[8] = 320;
}

// @symbol func_ov004_020b49f0
extern "C" void func_ov004_020b49f0(S49b8* s) {
    if (_Z15ApproachLinear2Rsss((short*)((char*)s + 0x10), s->f14, 0x10))
        s->f32 = 1;
}

// @symbol func_ov004_020b4a1c
extern "C" void func_ov004_020b4a1c(short *p)
{
    *(short *)((char *)p + 0x10) = -0x40;
}

// @symbol func_ov004_020b4a28
extern "C" void func_ov004_020b4a28(char* p) {
    _Z15ApproachLinear2Rsss((short*)(p + 0x12), *(short*)(p + 0x16), 4);
}

// @symbol func_ov004_020b4a40
extern "C" void func_ov004_020b4a40(short *p)
{
    p[9] = 200;
}

// @symbol func_ov004_020b4a4c
extern "C" void func_ov004_020b4a4c(char* p) {
    _Z15ApproachLinear2Rsss((short*)(p + 0x12), *(short*)(p + 0x16), 4);
}

// @symbol func_ov004_020b4a64
extern "C" void func_ov004_020b4a64(short *p)
{
    *(short *)((char *)p + 0x12) = -8;
}

Elem::Elem()
{
    *(int *)&pad[0x20] = 29;
}

/* __sinit_ov004_020b955c sources: the PMF records the dispatch table at
   data_ov004_020bf490 copies, the two-word records the two coordinate
   tables copy, and the words the 29-entry file-id table copies. */
extern "C" {
extern PairW data_02086b58;
extern PairW data_ov004_020bc234, data_ov004_020bc23c, data_ov004_020bc244, data_ov004_020bc16c;
extern PairW data_ov004_020bc1bc, data_ov004_020bc1ac, data_ov004_020bc25c, data_ov004_020bc264;
extern PairW data_ov004_020bc1c4, data_ov004_020bc164, data_ov004_020bc15c, data_ov004_020bc1d4;
extern PairW data_ov004_020bc22c, data_ov004_020bc19c, data_ov004_020bc184, data_ov004_020bc204;
extern PairW data_ov004_020bc18c, data_ov004_020bc1fc, data_ov004_020bc1f4, data_ov004_020bc154;
extern PairW data_ov004_020bc1dc, data_ov004_020bc1e4, data_ov004_020bc214;
extern PairW data_ov004_020bc1a4, data_ov004_020bc24c, data_ov004_020bc21c, data_ov004_020bc26c;
extern PairW data_ov004_020bc1cc, data_ov004_020bc174, data_ov004_020bc194;
extern int data_ov004_020b9e98, data_ov004_020b9e8c, data_ov004_020b9f2c;
extern int data_ov004_020b9ea4, data_ov004_020b9fd8, data_ov004_020b9eb8, data_ov004_020b9f5c;
extern int data_ov004_020ba010, data_ov004_020b9e90, data_ov004_020b9ed4, data_ov004_020b9ffc;
extern int data_ov004_020b9f78, data_ov004_020b9fa0, data_ov004_020b9e7c, data_ov004_020b9ebc;
extern int data_ov004_020b9ee4, data_ov004_020b9ea8, data_ov004_020b9ef0, data_ov004_020b9f7c;
extern int data_ov004_020b9f0c, data_ov004_020b9e50, data_ov004_020b9e70, data_ov004_020b9f60;
extern int data_ov004_020b9efc, data_ov004_020b9f88, data_ov004_020b9e78, data_ov004_020b9fc8;
extern int data_ov004_020b9ee8;
}

namespace s20b955c {
extern "C" {
Elem data_ov004_020bf648[3];
}
}

extern "C" {
int data_ov004_020bf5d4[29] = {
    data_ov004_020b9e98, data_ov004_020b9f54, data_ov004_020b9e8c, data_ov004_020b9f2c,
    data_ov004_020b9ea4, data_ov004_020b9fd8, data_ov004_020b9eb8, data_ov004_020b9f5c,
    data_ov004_020ba010, data_ov004_020b9e90, data_ov004_020b9ed4, data_ov004_020b9ffc,
    data_ov004_020b9f78, data_ov004_020b9fa0, data_ov004_020b9e7c, data_ov004_020b9ebc,
    data_ov004_020b9ee4, data_ov004_020b9ea8, data_ov004_020b9ef0, data_ov004_020b9f7c,
    data_ov004_020b9f0c, data_ov004_020b9e50, data_ov004_020b9e70, data_ov004_020b9f60,
    data_ov004_020b9efc, data_ov004_020b9f88, data_ov004_020b9e78, data_ov004_020b9fc8,
    data_ov004_020b9ee8
};
PairW data_ov004_020bf490[13] = {
    data_02086b58, data_ov004_020bc234, data_ov004_020bc23c, data_ov004_020bc244,
    data_ov004_020bc16c, data_ov004_020bc1bc, data_ov004_020bc1ac, data_ov004_020bc25c,
    data_ov004_020bc264, data_ov004_020bc1c4, data_ov004_020bc164, data_ov004_020bc15c,
    data_ov004_020bc1d4
};
PairW data_ov004_020bf428[13] = {
    data_02086b58, data_ov004_020bc22c, data_ov004_020bc19c, data_ov004_020bc184,
    data_ov004_020bc204, data_ov004_020bc18c, data_ov004_020bc1fc, data_ov004_020bc1f4,
    data_ov004_020bc154, data_ov004_020bc1dc, data_ov004_020bc1e4, data_ov004_020bc214,
    data_02086b58
};
PairW data_ov004_020bf4f8[13] = {
    data_02086b58, data_02086b58, data_02086b58, data_02086b58, data_02086b58,
    data_ov004_020bc1a4, data_ov004_020bc24c, data_ov004_020bc21c, data_ov004_020bc26c,
    data_ov004_020bc1cc, data_ov004_020bc174, data_02086b58, data_ov004_020bc194
};
int data_ov004_020bf560[29];
}
