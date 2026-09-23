//cpp
/* dScMgHanachan_c, the Hanachan (Wiggler) minigame scene: its 15 moving
 * objects, touch handling, drawing and setup.
 *
 * This TU covers 49 functions, 0x020eac38..0x020ede18. The rest of the
 * linker run, from 0x020ea280 (including the destructor pair), stays in
 * separate files, because func_ov006_020ea914 between them has no matching
 * source yet and a TU cannot claim .text with a hole in it. The destructors,
 * and so the vtable, are therefore not emitted here.
 *
 * `#pragma defer_codegen off` below does two things, both measured. mwccarm
 * then emits .text in source order, so functions run lowest ROM address
 * first; do not reorder. It also makes the optimisation pragmas bind to the
 * functions they bracket: four functions need opt_strength_reduction off and
 * Render needs opt_common_subs off, while the rest need the defaults. Without
 * it the TU drops to 44/49 and the order check fails.
 *
 * Still blocked:
 * - Most helpers are C functions on a char pointer, and their names and the
 *   data tables are unrecovered (func_ov006_*, data_ov006_*).
 * - The 15-entry object array at 0x4678 (0x98 bytes each) has no header
 *   type. The local views C, ObjB and Obj read it.
 * - OAM::Render and cstd::atan2 take Fix12 by value and stay mangled.
 */

#pragma defer_codegen off

#include "types.h"
#include "private/ov006_ec4dc_obj.h"
#include "dScMgHanachan_c.h"
#include "decl_common.h"

/* Local views of objects that have no header type yet. */

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
   spelled the same way for the same reason -- Pair, P, P2, Words2. */
struct Words2 { int v[2]; };

/* Two-named-int views that are never whole-struct-assigned, so the array
   spelling is NOT wanted here: func_ov006_020ec84c indexes +0x18 as a
   5-element array of {int,int} and func_ov006_020ed494 walks the 0x98
   stride with a real pointer (the only member that does). */
struct E { int a, b; };
typedef struct Thing
{
    int a;              /* 0x00 */
    int b;              /* 0x04 */
    char pad0[0x10];    /* 0x08 */
    V2 pos;             /* 0x18 */
    char pad1[0x78];    /* 0x20 */
} Thing;               /* 0x98 */

struct B72 {
    unsigned short f0 : 2;
    unsigned short f1 : 2;
    unsigned short f2 : 2;
    unsigned short f3 : 2;
};

/* The 0x98-byte element of the 15-entry array at +0x4678: five points at
   0x18, five words at 0x4c and five angles at 0x7a. No function steps a
   pointer of this type; the 0x98 stride is always a byte step. */
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

/* A second view of the same element, for func_ov006_020eb9dc; it names the
   fields at 0x70, 0x8c, 0x90 and 0x93. include/private/ov006_ec4dc_obj.h has a
   third view (Obj) for func_ov006_020ec4dc. */
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

/* ROM symbols this TU calls or reads. decl_common.h's declarations are not
   repeated. The C++ helpers must stay outside the extern "C" block so that
   they mangle. */
extern int ApproachLinear(int &, int, int);
extern void UpdateAngle(short &, short, int, short);
extern int ApproachLinear2(short &, short, short);
extern int ApproachLinear(short &, short, short);
namespace GX {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace GXS {
void LoadBGPltt(const void *src, u32 offset, u32 size);
void LoadOBJPltt(const void *src, u32 offset, u32 size);
}
namespace cstd { int fdiv(int numerator, int denominator); }

extern "C" {
/* This TU's own functions. */
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
extern void FreeGfxSlotsById(int x);
}

// @symbol func_ov006_020eac38
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void func_ov006_020eac38(void* arg)
{
    char* raw = (char*)arg;
    struct { int A, B, urot, va0x, res; } t;
    Vec2 vb0;
    struct { Vec2 vc0; } s3;
    struct { Vec2 pos, va, vb, vc, vout; } s;
    int px, py;
    int a2i;
    int i;
    short rot, ang;

    i = 0;
    vb0.y = -0x18000;
    s3.vc0.y = -0x14000;
    t.A = 0;
    t.B = 0;
    vb0.x = 0x2000;
    s3.vc0.x = 0;
    t.va0x = 0;
    for (; i < 5; i++) {
        rot = ((short*)(raw + 0x7a))[i];
        s.pos.x = *(int*)(raw + i*8 + 0x18);
        s.pos.y = *(int*)(raw + i*8 + 0x1c);
        s.va.x = t.va0x;
        s.va.y = 0x1000;
        ang = (short)(*(short*)(raw + 0x84) - t.A);
        func_0203d388(&s.va.x, rot);

        if (*(unsigned char*)(raw + 0x94) == 0) {
            if (i == 0) {
                rot = (short)(rot + (data_02082214[((unsigned short)ang >> 4) * 2] >> 1));
            } else {
                int sinv = data_02082214[((unsigned short)ang >> 4) * 2];
                rot = (short)(rot + (short)(((long long)sinv * (0x1800 - t.B) + 0x800) >> 12));
            }
        }

        if (data_ov006_0213c958 >= 6) {
            rot = (short)(rot & 0xfc00);
        }
        func_0203d680(&s.vout, &s.va, data_02082214[((unsigned short)(ang << 1) >> 4) * 2]);
        s.pos.x += s.vout.x;
        s.pos.y += s.vout.y;

        t.res = func_ov006_020ebb40(raw, i);
        px = s.pos.y >> 12;
        py = s.pos.x >> 12;
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
            1,
            data_ov006_02142018[(a2i = func_ov006_020ebc08(raw, i), (t.urot = (unsigned short)rot), a2i)],
            py, px,
            t.res, -1, 0x1000,
            t.urot);

        if (i == 0) {
            if (*(unsigned char*)(raw + 0x94) != 0 && *(int*)(raw + 0x48) > 0) {
                int fp2;
                int scale;
                fp2 = (0x3000 - *(int*)(raw + 0x48)) >> 12;
                s.vb.x = vb0.x;
                s.vb.y = vb0.y;
                func_0203d388(&s.vb.x, rot);
                s.vb.x += s.pos.x;
                s.vb.y += s.pos.y;
                if (fp2 != 0)
                    scale = 0x1000;
                else
                    scale = func_02053200((data_02082214[((unsigned short)ang >> 4) * 2] >> 2) + 0xc00);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                    1, data_ov006_021375c0[fp2],
                    s.vb.x >> 12, s.vb.y >> 12,
                    -1, -1, scale, t.urot);
            } else if (*(int*)raw == data_ov006_0213c97c[0]) {
                if ((*(int*)(raw + 4) == data_ov006_0213c97c[1] || *(int*)raw == 0) &&
                    *(short*)(raw + 0x90) > 0x1a && *(short*)(raw + 0x90) < 0x30) {
                    int idx = s3.vc0.x;
                    s.vc.x = s3.vc0.x;
                    s.vc.y = s3.vc0.y;
                    if (*(short*)(raw + 0x90) < 0x26) {
                        int d = (0x30 - *(short*)(raw + 0x90)) * 0x600;
                        int vy;
                        s.vc.y = (vy = *(volatile int*)((char*)&s - 4), (idx = 1) ? vy : vy) - d;
                    }
                    func_0203d388(&s.vc.x, rot);
                    s.vc.x += s.pos.x;
                    s.vc.y += s.pos.y;
                    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
                        1, data_ov006_021375a0[idx],
                        s.vc.x >> 12, s.vc.y >> 12,
                        -1, -1, 0x1000, (unsigned short)(short)(rot & 0xf800));
                }
            }
        }

        t.A += 0x1800;
        t.B += 0x200;
    }
}
#pragma pop
}


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
// @symbol func_ov006_020eb0c8
extern "C" void func_ov006_020eb0c8(char *self) {
    if (ApproachLinear2(*(s16 *)(self + 0x90), 0, 1)) {
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

// @symbol func_ov006_020eb1e0
extern "C" {
void func_ov006_020eb1e0(char *raw)
{
    int va[2];
    int vb[2];
    short tgt;

    ApproachLinear(*(int *)(raw + 0x48), 0, 0x200);
    if (ApproachLinear2(*(short *)(raw + 0x78), 0, 1) != 0) {
        int *p10 = (int *)(((int)raw + 0x10));
        int *pd = (int *)(((int)data_ov006_0213c984));
        if (p10[0] == pd[0] &&
            (p10[1] == pd[1] || *(int *)(raw + 0x10) == 0)) {
            *(unsigned char *)(raw + 0x94) = 1;
            *(int *)(raw + 0x48) = 0x3000;
            *(unsigned char *)(raw + 0x93) = 0;
            *(unsigned char *)(raw + 0x95) = 0;
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
    tgt = *(short *)(raw + 0x76) - 0x1000;
    ApproachLinear(*(short *)(raw + 0x7a), tgt, 0x400);
    func_0203d388(va, *(short *)(raw + 0x7a));
    func_0203d704((V2 *)vb, (V2 *)(raw + 0x20), (V2 *)va);
    *(int *)(raw + 0x18) = vb[0];
    *(int *)(raw + 0x1c) = vb[1];
    {
        short cnt = *(short *)(raw + 0x78);
        if ((cnt & 0x10) == 0 || cnt < 0x38)
            *(unsigned char *)(raw + 0x95) = 1;
        else
            *(unsigned char *)(raw + 0x95) = 0;
    }
}
}

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

// @symbol func_ov006_020eb3e4
extern "C" {
void func_ov006_020eb3e4(char *c)
{
    int va[2];
    int vb[2];
    short tgt;
    short cnt;

    ApproachLinear(*(int *)(c + 0x48), 0, 0x200);
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
    ApproachLinear(*(short *)(c + 0x7a), tgt, 0x400);
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

// @symbol func_ov006_020eb558
extern "C" /* char* parameter to agree with include/decl_common.h -- see func_ov006_020eac38. */
void func_ov006_020eb558(char *raw)
{
    int *thiz = (int *)raw;
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

// @symbol func_ov006_020eb768
extern "C" {
int func_ov006_020eb768(char* raw, const V2* b){
  int i;
  const V2* p = (const V2*)(raw + 0x18);
  for (i = 0; i < 5; i++){
    if (func_0203d5dc(p, b) < 0x14000) return 1;
    p = (const V2*)((char*)p + 8);
  }
  return 0;
}
}

// @symbol func_ov006_020eb7b0
extern "C" {
int func_ov006_020eb7b0(char* raw)
{
    if (*(unsigned char*)(raw + 0x93) == 0) {
        S* q = (S*)(long long)(raw + 8);
        int x = *(int*)raw;
        if (x == q->a && (*(int*)(raw + 4) == q->b || x == 0))
            return 1;
    }
    return 0;
}
}

// @symbol func_ov006_020eb7f8
extern "C" {
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

// @symbol func_ov006_020eb9b0
extern "C" {
void func_ov006_020eb9b0(char *raw)
{
    *(int *)(raw + 0x44) = 2560;
    *(short *)(raw + 0x86) = 1536;
    *(short *)(raw + 0x8e) = 0;
    *(char *)(raw + 0x93) = 0;
    *(char *)(raw + 0x94) = 0;
    *(char *)(raw + 0x95) = 2;
}
}

// @symbol func_ov006_020eb9dc
extern "C" {
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

// @symbol func_ov006_020ebb40
extern "C" {
/* Second parameter unused by the body -- func_ov006_020eac38 passes an index
   this callee ignores; declaring it keeps the caller's argument setup. */
int func_ov006_020ebb40(char *raw, int){
  int r2 = 0;
  if (*(unsigned char*)(raw + 0x93) != 0){
    int r1 = *(short*)(raw + 0x8e);
    if (r1 > 0x20){ r2 = 1; goto end; }
    if (r1 > 0x10){ *(unsigned char*)(raw + 0x95) = 3; r2 = 5; goto end; }
    if (r1 > 0) r2 = 4;
    goto end;
  }
  {
    int *ref = data_ov006_0213ca04;
    int ip = *(int*)raw;
    if (ip == ref[0]){
      if (*(int*)(raw + 4) == ref[1]) goto C;
      if (ip == 0) goto C;
    }
    ref = data_ov006_0213c9d4;
    if (ip != ref[0]) goto end0;
    if (*(int*)(raw + 4) == ref[1]) goto C;
    if (ip != 0) goto end0;
  }
C:
  {
    int v = *(short*)(raw + 0x90);
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

// @symbol func_ov006_020ebc08
extern "C" {
int func_ov006_020ebc08(char* raw, int n) {
    switch (n) {
    case 0:
        return *(unsigned char*)(raw + 0x95);
    case 1:
        return ((struct B72*)(raw + 0x74))->f0 + 7;
    case 2:
        return ((struct B72*)(raw + 0x74))->f1 + 7;
    case 3:
        return ((struct B72*)(raw + 0x74))->f2 + 7;
    default:
        return ((struct B72*)(raw + 0x74))->f3 + 7;
    }
}
}

// @symbol func_ov006_020ebc7c
extern "C" {
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

// @symbol func_ov006_020ebd7c
extern "C" {
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

// @symbol func_ov006_020ebe6c
extern "C" {
int func_ov006_020ebe6c(void){
  int v = 3;
  int r = (int)(((unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13);
  if (r < 0x400) v = 0;
  else if (r < 0x800) v = 1;
  else if (r < 0xc00) v = 2;
  return v;
}
}

// @symbol func_ov006_020ebeb4
extern "C" {
int func_ov006_020ebeb4(char *raw){
    int r=0;
    int v=*(int*)(raw+0x20);
    if(v>=0xe8000){ r|=1; v=0xe8000; *(int*)(raw+0x20)=v; r&=0xff; }
    else if(v<=0x18000){ r|=2; v=0x18000; *(int*)(raw+0x20)=v; r&=0xff; }
    v=*(int*)(raw+0x24);
    if(v>=0xa8000){ r|=8; v=0xa8000; *(int*)(raw+0x24)=v; r&=0xff; }
    else if(v<=0x18000){ r|=4; v=0x18000; *(int*)(raw+0x24)=v; r&=0xff; }
    return r;
}
}

// @symbol func_ov006_020ebf20
extern "C" {
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

    UpdateAngle(c->s7a[1], c->s76, 8, 0x100);

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
            UpdateAngle(c->s7a[i], na, step, amt);
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
        UpdateAngle(c->s7a[0], (s16)(c->s7a[1] + dd), 8, 0x100);
    }
    func_0203d388((int *)&g, c->s7a[0]);
    vb2 = c->v18;
    func_0203d704(&h, vb2 + 1, &g);
    c->v18[0].x = h.x;
    c->v18[0].y = h.y;
}
}

// @symbol func_ov006_020ec134
/* func_ov006_020ec134 at 0x020ec134 (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
extern "C" {
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

// @symbol func_ov006_020ec2bc
extern "C" {
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

// @symbol func_ov006_020ec458
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void func_ov006_020ec458(C* self) {
    int i;
    s16 v = self->s76;
    if (v < 0 && self->v18[0].x < -0x60000) {
        for (i = 0; i < 5; i++)
            (*(int*)((char*)self + (i << 3) + 0x18)) += 0x170000;
        return;
    }
    if (v > 0 && self->v18[0].x > 0x160000) {
        for (i = 0; i < 5; i++)
            (*(int*)((char*)self + (i << 3) + 0x18)) -= 0x170000;
    }
}
#pragma pop
}

// @symbol func_ov006_020ec4dc
extern "C" {
void func_ov006_020ec4dc(Obj *self, int n)
{
    int b = n % 3;
    int a = n / 3;
    int rA, rB, rC, rD;
    int g1;
    int cm, dm;

    self->f40 = 0xa00;
    self->f44 = 0xa00;
    g1 = data_ov006_0213c958 / 3 - 1;

    rA = cstd::fdiv(a << 12, g1 << 12);
    rB = cstd::fdiv((g1 - a) << 12, g1 << 12);
    rC = cstd::fdiv(b << 12, 0x2000);
    rD = cstd::fdiv((2 - b) << 12, 0x2000);

    if (a & 1) {
        self->s76 = -0x4000;
        self->s84 = -0x8000;
        cm = 0x8000;
        dm = 0xfe000;
    } else {
        self->s76 = 0x4000;
        self->s84 = 0;
        cm = 0x100000;
        dm = 0xa000;
    }

    {
        Vec2i base;
        int i = 0;
        int delta;

        base.x = 0;
        base.y = 0;
        delta = 0;
        base.x = (int)(((long long)cm * rC + 0x800) >> 12) + (int)(((long long)dm * rD + 0x800) >> 12);
        base.y = (int)((((long long)rA << 16) + 0x800) >> 12) + (int)(((long long)rB * 0xb0000 + 0x800) >> 12);

        for (; i < 5; i++) {
            int bx = *(volatile int *)&base.x;
            self->arr[i].x = bx;
            self->arr[i].y = *(volatile int *)&base.y;
            if (self->s76 < 0)
                self->arr[i].x += delta;
            else
                self->arr[i].x -= delta;
            self->s7a[i] = self->s76;
            delta += 0x20000;
        }
    }

    func_ov006_020ec2bc((struct C *)self);
    self->f68 = 0;
    self->f6c = 0x1ec;
    *(struct Words2 *)&self->v0 = *(struct Words2 *)&data_ov006_0213ca24;
    *(struct Words2 *)&self->v1 = *(struct Words2 *)&self->v0;
}
}


// @symbol func_ov006_020ec6e8
extern "C" {
void func_ov006_020ec6e8(char *raw)
{
    int res = func_ov006_020ebeb4(raw);
    if (res != 0) {
        int tx;
        int ty;
        int y;
        int x;
        s16 a;
        s16 d;
        x = *(int*)(raw + 0x20);
        y = *(int*)(raw + 0x24);
        tx = x;
        ty = y;
        if (res | 3) tx = 0x80000;
        if (res | 0xc) ty = 0x60000;
        a = _ZN4cstd5atan2E5Fix12IiES1_(tx - x, -(ty - y));
        d = (s16)(a - *(s16*)(raw + 0x7a));
        if (d > 0x2000) d = 0x2000;
        else if (d < -0x2000) d = -0x2000;
        *(s16*)(raw + 0x76) = (s16)(*(s16*)(raw + 0x7c) + d);
        return;
    }
    {
        int ra;
        int rb;
        s16 a;
        s16 d;
        *(s16*)(int)(raw + 0x78) -= 1;
        if (*(s16*)(raw + 0x78) >= 0) return;
        *(s16*)(raw + 0x78) = (s16)((((int)((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) * 0x1e) >> 12) + 0x3c);
        ra = RandomIntInternal(&data_0209e650);
        rb = RandomIntInternal(&data_0209e650);
        a = _ZN4cstd5atan2E5Fix12IiES1_(
            ((int)((unsigned int)(ra & 0x7fffffff) >> 19) * 0x100) - *(int*)(raw + 0x20),
            -(((int)((unsigned int)(rb & 0x7fffffff) >> 19) * 0xc0) - *(int*)(raw + 0x24)));
        d = (s16)(a - *(s16*)(raw + 0x7a));
        if (d > 0x2000) d = 0x2000;
        else if (d < -0x2000) d = -0x2000;
        *(s16*)(raw + 0x76) = (s16)(*(s16*)(raw + 0x7c) + d);
    }
}
}


// @symbol func_ov006_020ec84c
extern "C" {
void func_ov006_020ec84c(char *raw, int)
{
    int i;
    *(int *)(raw + 0x40) = 0xa00;
    *(int *)(raw + 0x44) = 0xa00;
    for (i = 0; i < 5; i++) {
        ((struct E *)(raw + 0x18))[i].a = (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) << 8);
        ((struct E *)(raw + 0x18))[i].b = (int)(((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 0x13) * 0xc0);
        ((short *)(raw + 0x7a))[i] = (short)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    }
    *(short *)(raw + 0x76) = (short)((*(short *)(raw + 0x7a) + *(short *)(raw + 0x7c)) >> 1);
    func_ov006_020ebeb4(raw);
    func_ov006_020ec2bc((struct C *)raw);
    *(short *)(raw + 0x78) = 0x1e;
    *(int *)(raw + 0x68) = 0;
    *(int *)(raw + 0x6c) = 0x1ec;
    *(struct Words2 *)(raw + 0) = data_ov006_0213ca14;
    *(struct Words2 *)(raw + 8) = *(struct Words2 *)(raw + 0);
}
}


// @symbol func_ov006_020ec93c
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void func_ov006_020ec93c(C* self) {
    int i;
    s16 v = self->s76;
    if (v < 0 && self->v18[0].x < -0x60000) {
        for (i = 0; i < 5; i++)
            (*(int*)((char*)self + (i << 3) + 0x18)) += 0x170000;
        return;
    }
    if (v > 0 && self->v18[0].x > 0x160000) {
        for (i = 0; i < 5; i++)
            (*(int*)((char*)self + (i << 3) + 0x18)) -= 0x170000;
    }
}
#pragma pop
}


// @symbol func_ov006_020ec9c0
extern "C" {
#pragma push
#pragma opt_strength_reduction off
void func_ov006_020ec9c0(char* raw, int idx)
{
    int n1;
    int A, B;
    int p;
    int valY;
    int pos[2];
    int i;
    int off;

    *(int*)(raw + 0x40) = 0xa00;
    *(int*)(raw + 0x44) = 0xa00;

    n1 = data_ov006_0213c958 - 1;
    A = cstd::fdiv(idx << 12, n1 << 12);
    B = cstd::fdiv((n1 - idx) << 12, n1 << 12);

    p = (n1 - 2) << 3;
    valY = (int)(((long long)((0x28 - p) << 12) * A + 0x800) >> 12)
         + (int)(((long long)((p + 0x98) << 12) * B + 0x800) >> 12);

    *(int*)((char*)pos + 4) = 0;
    *(int*)((char*)pos + 0) = 0;
    *(int*)((char*)pos + 4) = valY;

    if (RandomIntInternal(&data_0209e650) & 0x8000000) {
        *(short*)(raw + 0x76) = -0x4000;
        pos[0] = 0x80000;
        pos[0] += ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    } else {
        *(short*)(raw + 0x76) = 0x4000;
        pos[0] = 0x80000;
        pos[0] -= ((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) << 7;
    }

    for (i = 0, off = 0; i < 5; i++) {
        *(int*)(raw + i * 8 + 0x18) = pos[0];
        *(int*)(raw + i * 8 + 0x1c) = pos[1];
        if (*(short*)(raw + 0x76) < 0)
            *(int*)(raw + i * 8 + 0x18) += off;
        else
            *(int*)(raw + i * 8 + 0x18) -= off;
        *(short*)(raw + i * 2 + 0x7a) = *(short*)(raw + 0x76);
        off += 0x20000;
    }

    func_ov006_020ec2bc((struct C *)raw);
    *(int*)(raw + 0x68) = 0;
    *(int*)(raw + 0x6c) = 0x1ec;
    *(struct Words2 *)(raw + 0) = *(struct Words2 *)data_ov006_0213ca0c;
    *(struct Words2 *)(raw + 8) = *(struct Words2 *)(raw + 0);
}
#pragma pop
}


// @symbol func_ov006_020ecb80
extern "C" {
void func_ov006_020ecb80(void *arg)
{
  char *raw = (char *) arg;
  ApproachLinear(*(short *) (raw + 0x7a), (short) ((*((short *) (raw + 0x76))) - 0x2000), 0x100);
}
}


// @symbol func_ov006_020ecba4
extern "C" {
void func_ov006_020ecba4(struct C* c, int param)
{
    V2 vec0;
    V2 vec1;
    V2 vec3;
    V2 vec2;
    V2 vec4;
    int m, r1, r2, k, i, j;

    c->b94 = 1;
    c->w48 = 0x3000;
    c->b95 = 0;
    c->s86 = 0x200;
    c->w40 = 0;
    c->w44 = 0;

    m = data_ov006_0213c958 - 1;
    r1 = cstd::fdiv(param << 12, m << 12);
    r2 = cstd::fdiv((m - param) << 12, m << 12);
    k = (m - 2) * 8;

    vec0.y = 0;
    vec0.y = (int)(((long long)((0x40 - k) << 12) * r1 + 0x800) >> 12)
           + (int)(((long long)((k + 0x98) << 12) * r2 + 0x800) >> 12);
    vec0.x = 0;
    if (param & 1)
        vec0.x = 0xa8000;
    else
        vec0.x = 0x38000;

    c->s76 = -0x8000;
    c->v18[0].x = *(int*)&vec0;
    c->v18[0].y = *((int*)&vec0 + 1);
    c->s7a[0] = c->s76 - 0x2000;

    for (i = 1; i < 5; i++) {
        j = i - 1;
        vec1.x = 0;
        vec1.y = (i * 2 + 0x10) << 12;
        func_0203d388((int *)&vec1, (s16)(c->s7a[j] + 0x2000));
        c->s7a[i] = c->s7a[j] + 0x2000;
        func_0203d704(&vec2, &c->v18[j], &vec1);
        c->v18[i].x = vec2.x;
        c->v18[i].y = vec2.y;
    }

    func_ov006_020ec2bc(c);

    vec3.x = 0;
    vec3.y = -0x10000;
    c->s7a[0] = c->s76 - 0x2000;
    func_0203d388((int *)&vec3, c->s7a[0]);
    func_0203d704(&vec4, &c->v18[1], &vec3);
    c->v18[0].x = vec4.x;
    c->v18[0].y = vec4.y;

    *(struct Words2 *)&c->v0 = *(struct Words2 *)&data_ov006_0213c9dc;
    *(struct Words2 *)&c->v8 = *(struct Words2 *)&c->v0;
}
}


// @symbol func_ov006_020ecdb8
extern "C" {
void func_ov006_020ecdb8(char *raw, int arg1, int arg2)
{
    int i;
    struct C *o = (struct C *)raw;

    *(int *)(raw + 0x68) = 0;
    *(int *)(raw + 0x6c) = 0;
    *(int *)(raw + 0x70) = 1;
    *(int *)(raw + 0x64) = arg1;
    *(short *)(raw + 0x8c) = 0;
    *(short *)(raw + 0x84) = (short)(RandomIntInternal(&data_0209e650) & 0xfff0);
    *(short *)(raw + 0x86) = 0x600;
    *(unsigned char *)(raw + 0x93) = 0;
    *(unsigned char *)(raw + 0x94) = 0;
    *(unsigned char *)(raw + 0x95) = 2;
    *(int *)(raw + 0x88) = arg2;
    *(unsigned char *)(raw + 0x92) = 1;

    for (i = 0; i < 5; i++)
        o->w4c[i] = 0x10000;

    *(short *)(raw + 0x74) = (short)func_ov006_020ebc7c(arg1, 0xff);

    switch (data_ov006_02141fd8) {
    case 0:
        func_ov006_020ecba4((struct C *)raw, arg1);
        return;
    case 1:
        func_ov006_020ec9c0(raw, arg1);
        return;
    case 2:
        func_ov006_020ec84c(raw, arg1);
        return;
    case 3:
    default:
        func_ov006_020ec4dc((Obj *)raw, arg1);
        return;
    }
}
}


// @symbol _ZN15dScMgHanachan_c16CleanupResourcesEv
s32 dScMgHanachan_c::CleanupResources()
{
    func_ov004_020ad90c(this);
    return 1;
}


// @symbol _ZN15dScMgHanachan_c6RenderEv
#pragma push
#pragma opt_common_subs off
s32 dScMgHanachan_c::Render()
{
    char *raw = (char *)this;
    int i;
    char *arr;

    func_ov004_020b1e34(raw, 0xe0, 0x14, 1);
    func_ov006_020ea670();
    func_ov006_020ea350();

    {
        Pair *p = PAIR0(raw);
        Pair *g = &data_ov006_0213c9ac;
        if (p->v[0] == g->v[0]) {
            if (p->v[1] == g->v[1] || unk_4660 == 0) {
                i = 0;
                if (data_ov006_0213c958 > 0) {
                    arr = raw + 0x4678;
                    do {
                        func_ov006_020eac38(arr);
                        i++;
                        arr += 0x98;
                    } while (i < data_ov006_0213c958);
                }
                goto tail;
            }
        }
    }

    {
        Pair *p = PAIR0(raw);
        Pair *g = &data_ov006_0213c994;
        if (p->v[0] == g->v[0]) {
            if (p->v[1] == g->v[1] || unk_4660 == 0) {
                int flag = unk_4f64;
                if (flag != 0) {
                    if (unk_466c < 0x5a) goto do_eac;
                }
                if (flag != 0) goto count_loop;
                if (unk_466c < 0x8a) goto do_eac;
                goto count_loop;
            do_eac:
                func_ov006_020eac38(*(void **)(raw + 0x4f60));
                goto tail;
            count_loop:
                i = 0;
                if (data_ov006_0213c958 > 0) {
                    arr = raw + 0x4678;
                    do {
                        func_ov006_020eac38(arr);
                        i++;
                        arr += 0x98;
                    } while (i < data_ov006_0213c958);
                }
                goto tail;
            }
        }
    }

    {
        Pair *p = PAIR0(raw);
        Pair *g = &data_ov006_0213ca3c;
        if ((p->v[0] == g->v[0] && (p->v[1] == g->v[1] || unk_4660 == 0))
            || (p = PAIR0(raw), g = &data_ov006_0213ca34, (p->v[0] == g->v[0] && (p->v[1] == g->v[1] || unk_4660 == 0)))) {
            func_ov006_020eac38(*(void **)(raw + 0x4f60));
        }
    }

tail:
    func_ov006_020ea914(*(void **)(raw + 0x4f60));

    {
        Pair *p = PAIR0(raw);
        Pair *g = &data_ov006_0213ca2c;
        if (!(p->v[0] == g->v[0] && (p->v[1] == g->v[1] || unk_4660 == 0))) {
            int idx = GetGameLanguage();
            void *e = data_ov006_0213ca9c[idx];
            void *f = *(void **)((char *)e + 0xc);
            DrawOamSprite(f, 0x80, 0x10, 0);
            func_ov004_020b2220(0x80, 0x28, *(void **)(raw + 0x4670), 1, -1, 0x800, 0);
        }
    }

    return 1;
}
#pragma pop


// @symbol _ZN15dScMgHanachan_c8BehaviorEv
s32 dScMgHanachan_c::Behavior()
{
    char *raw = (char *)this;

    data_ov006_02141fcc = data_ov006_02141fcc + 0x800;
    (((CB *)raw)->*(*(PMF *)(raw + 0x4660)))();
    int i = 0;
    if (data_ov006_0213c958 > 0) {
        char *p = raw + 0x4678;
        do {
            func_ov006_020eb610(p);
            i++;
            p += 0x98;
        } while (i < data_ov006_0213c958);
    }
    int j = 0;
    if (data_ov006_0213c958 > 0) {
        char *q = raw + 0x4678;
        do {
            func_ov006_020eb558(q);
            j++;
            q += 0x98;
        } while (j < data_ov006_0213c958);
    }
    func_ov006_020ea71c();
    func_ov006_020ea3d0(raw + 0x4670);
    return 1;
}


// @symbol func_ov006_020ed270
extern "C" {
void func_ov006_020ed270(void)
{
}
}


// @symbol func_ov006_020ed274
extern "C" {
void func_ov006_020ed274(char *raw)
{
    int idx;
    int b;
    int w0, w1;
    *(int *)(((int)raw + 0x466c)) -= 1;
    if (*(int *)(raw + 0x4000 + 0x66c) != 0)
    {
        idx = data_020a0e40[0];
        b = 0;
        if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0)
            b = 1;
        if (b == 0)
            return;
    }
    w0 = data_ov006_0213ca54[0];
    w1 = data_ov006_0213ca54[1];
    w0 = w1 ? w0 : w0;
    *(int *)(raw + 0x4000 + 0x660) = w0;
    *(int *)(raw + 0x4000 + 0x664) = w1;
}
}


// @symbol func_ov006_020ed300
extern "C" {
void func_ov006_020ed300(char *raw)
{
    *(int *)(raw + 0x466c) = 0xb4;
    *(struct Pair *)(raw + 0x4660) = data_ov006_0213ca5c;
}
}


// @symbol func_ov006_020ed328
extern "C" {
void func_ov006_020ed328(void)
{
}
}


// @symbol func_ov006_020ed32c
extern "C" {
void func_ov006_020ed32c(char *raw) {
    *(struct Pair *)(raw + 0x4000 + 0x660) = data_ov006_0213ca64;
}
}


// @symbol func_ov006_020ed34c
extern "C" {
void func_ov006_020ed34c(char *raw)
{
    int v;
    *(int *)(((int)raw + 0x466c)) -= 1;
    v = *(int *)(raw + 0x466c);
    if (v == 0)
    {
        if (*(unsigned char *)(raw + 0x4f64) != 0)
        {
            func_ov004_020b0a54(1);
            func_ov006_020ed32c(raw);
        }
        else
        {
            *(int *)(*(int *)(raw + 0x4f60) + 0x70) = 1;
            func_ov004_020b0a54(0x12);
            func_ov006_020ed300(raw);
        }
        *(unsigned char *)(raw + 0xc3) = 0;
    }
    else if (*(unsigned char *)(raw + 0x4f64) != 0)
    {
        if (v == 0x5a)
        {
            func_ov006_020eb8f0(*(char **)(raw + 0x4f60));
        }
    }
    else
    {
        if (v == 0x8a)
        {
            func_ov006_020eb7f8(*(char **)(raw + 0x4f60));
        }
    }
}
}


// @symbol func_ov006_020ed40c
extern "C" {
void func_ov006_020ed40c(char *raw)
{
    int i = 0;
    if (i < data_ov006_0213c958) {
        char *p = raw;
        do {
            *(unsigned char *)(p + 0x4000 + 0x70a) = 0;
            i++;
            p += 0x98;
        } while (i < data_ov006_0213c958);
    }
    if (*(unsigned char *)(raw + 0x4000 + 0xf64) != 0)
        *(int *)(raw + 0x4000 + 0x66c) = 0xb4;
    else
        *(int *)(raw + 0x4000 + 0x66c) = 0xe4;
    *(struct Words2 *)(raw + 0x4000 + 0x660) = *(struct Words2 *)data_ov006_0213ca6c;
}
}


// @symbol func_ov006_020ed494
extern "C" {
void func_ov006_020ed494(char *raw)
{
    V2 a;
    int vec[4];
    int b = 0;
    int idx = data_020a0e40[0];

    if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0)
        b = 1;

    if (b != 0)
    {
        Thing *p;
        V2 *src;
        a.x = data_020a0dea[(unsigned int)idx * 4] << 12;
        a.y = data_020a0deb[(unsigned int)idx * 4] << 12;
        p = *(Thing **)(raw + 0x4f60);
        src = (V2 *)(int)((char *)p + 0x18);
        vec[0] = src->x;
        vec[1] = src->y;

        if (func_ov006_020eb7b0(*(char **)(raw + 0x4f60)) != 0 &&
            func_ov006_020eb768(*(char **)(raw + 0x4f60), &a) != 0)
        {
            *(u8 *)(raw + 0x4f64) = 1;
            func_02012790(0x26);
            p = *(Thing **)(raw + 0x4f60);
            {
            int *g = data_ov006_0213ca4c;
            if (p->a == g[0] &&
                (p->b == g[1] || p->a == 0))
                _ZN5Sound12PlayBank2_2DEj(0x1ef);
            else
                _ZN5Sound12PlayBank2_2DEj(0x1ee);
            if (data_ov006_02141fd8 == 0)
                vec[0] -= 0x28000;
            func_ov006_020ea5f0(vec[0], vec[1]);
            func_ov006_020ed40c(raw);
            return;
            }
        }

        {
            int found = -1;
            int i = 0;
            Thing *ptr;

            if (data_ov006_0213c958 > 0)
            {
                ptr = (Thing *)(raw + 0x4678);
                do
                {
                    if (func_ov006_020eb7b0((char *)ptr) != 0 &&
                        func_ov006_020eb768((char *)ptr, &a) != 0)
                    {
                        found = i;
                        break;
                    }
                    i++;
                    ptr++;
                }
                while (i < data_ov006_0213c958);
            }

            if (found != -1)
            {
                int j;
                Thing *ptr2;
                Thing *sel = (Thing *)(raw + 0x4678) + found;
                {
                int *g2 = data_ov006_0213ca74;
                if (sel->a == g2[0] &&
                    (sel->b == g2[1] || sel->a == 0))
                    _ZN5Sound12PlayBank2_2DEj(0x1eb);
                else
                    _ZN5Sound12PlayBank2_2DEj(0x1ed);
                }

                {
                    j = 0;
                    if (data_ov006_0213c958 > 0)
                    {
                        ptr2 = (Thing *)(raw + 0x4678);
                        do
                        {
                            V2 *s2 = (V2 *)(int)((char *)ptr2 + 0x18);
                            vec[0] = s2->x;
                            vec[1] = s2->y;
                            func_ov006_020eb9dc((struct ObjB *)ptr2, func_0203d5dc((const V2 *)vec, &a));
                            j++;
                            ptr2++;
                        }
                        while (j < data_ov006_0213c958);
                    }
                }

                func_02012790(0xe);
                {
                    V2 *s3 = (V2 *)(int)((char *)sel + 0x18);
                    vec[2] = s3->x;
                    vec[3] = s3->y;
                }
                if (data_ov006_02141fd8 == 0)
                    vec[2] -= 0x28000;
                func_ov006_020ea81c(vec[2], vec[3]);

                {
                    int t = *(int *)(raw + 0x4670) - 10;
                    if (t < 0)
                        t = 0;
                    *(int *)(raw + 0x4670) = t;
                }
            }
        }
    }

    if (ApproachLinear(*(int *)(raw + 0x4674), 0, 1) == 0)
        return;

    *(int *)(raw + 0x4674) = 0x3c;
    if (ApproachLinear(*(int *)(raw + 0x4670), 0, 1) != 0)
    {
        func_02012790(0xe);
        func_ov006_020ed40c(raw);
    }

    if (*(int *)(raw + 0x4670) > 2)
        _ZN5Sound12PlayBank2_2DEj(0xa7);
    else
        _ZN5Sound12PlayBank2_2DEj(0xa6);
}
}


// @symbol func_ov006_020ed81c
extern "C" {
void func_ov006_020ed81c(char *raw)
{
    *(int *)(raw + 0x466c) = 0x4b0;
    *(struct Pair *)(raw + 0x4660) = data_ov006_0213c9cc;
}
}


// @symbol func_ov006_020ed844
extern "C" {
void func_ov006_020ed844(char *raw)
{
    if (ApproachLinear(*(int *)(raw + 0x4668), 0, 1) == 0) return;
    FreeGfxSlotsById(0x1d);
    if (*(unsigned char *)(raw + 0xc4) == 0) {
        *(unsigned char *)(raw + 0xc3) = 1;
        *(unsigned char *)(raw + 0xc4) = 1;
        *(short *)(raw + 0xc0) = 0;
    }
    func_ov006_020ed81c(raw);
}
}


// @symbol func_ov006_020ed8a4
extern "C" {
void func_ov006_020ed8a4(void *arg) {
    char *raw = (char *)arg;
    if (*(s32 *)(raw + 0xbc) < 0xa) {
        int bc = *(s32 *)(raw + 0xbc);
        data_ov006_02141fd8 = data_ov006_0212e80c[bc];
        data_ov006_0213c958 = data_ov006_0212e820[bc];
    } else {
        int ret = func_ov006_020ebe6c();
        data_ov006_02141fd8 = ret;
        if (ret == 3) {
            data_ov006_0213c958 = 0xf;
        } else {
            unsigned int rnd = RandomIntInternal(&data_0209e650) & 0x7fffffff;
            data_ov006_0213c958 = ((int)(rnd >> 19) * 3 >> 12) + 3;
        }
    }

    if (*(unsigned int *)(raw + 0xbc) < 0xa)
        func_ov006_020ebd7c(2);
    else
        func_ov006_020ebd7c(3);

    {
        int i = 0;
        if (data_ov006_0213c958 > 0) {
            char *p = (char *)(raw + 0x4678);
            do {
                func_ov006_020ecdb8(p, i, *(s32 *)(raw + 0xbc));
                i++;
                p += 0x98;
            } while (i < data_ov006_0213c958);
        }
    }

    {
        unsigned int rnd = RandomIntInternal(&data_0209e650) & 0x7fffffff;
        int k = (int)(rnd >> 19) * data_ov006_0213c958 >> 12;
        *(char **)(raw + 0x4f60) = (char *)(raw + 0x4678) + k * 0x98;
        *(s32 *)(*(char **)(raw + 0x4f60) + 0x70) = 0;
        *(s32 *)(raw + 0x4668) = 0x3c;
        func_ov004_020b0cac(0xd, 0x80, 0x50, 1, -1, 0xd);
        *(s32 *)(raw + 0x4674) = 0x3c;
        *(u8 *)(raw + 0x4f64) = 0;
        *(Pair *)(raw + 0x4660) = data_ov006_0213c964;
    }
}
}


// @symbol _ZN15dScMgHanachan_c13OnYoshiTryEatEi
void dScMgHanachan_c::OnYoshiTryEat(int state)
{
    char *raw = (char *)this;

    if (state == 1) {
        /* `const` on the read is load-bearing: without it mwcc CSEs the +0xbc field
           address into its own register (add r2,r4,#0xbc / ldr [r2] / str [r2]) and the
           function grows a word; the cartridge re-issues ldr r1,[r4,#0xbc]. */
        unk_0bc = *(const unsigned int*)(raw + 0xbc) + 1;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
    } else if (state == 0x12) {
        unk_0bc = 0;
        if (unk_0bc > 0x270e) unk_0bc = 0x270e;
        if (data_ov004_020beb68 != 0) *(int*)((char *)data_ov004_020beb68 + 0xb4) = 0;
        func_ov004_020adb1c(mHudScore);
        unk_4670 = 0x14;
    } else {
        int v = unk_0bc;
        if (data_ov004_020beb68 != 0) *(int*)((char *)data_ov004_020beb68 + 0xb4) = v;
        unk_4670 = 0x14;
    }
    func_ov006_020ea8e0();
    func_ov006_020ea658();
    func_ov006_020ed8a4(raw);
}


// @symbol _ZN15dScMgHanachan_c13InitResourcesEv
s32 dScMgHanachan_c::InitResources()
{
    void *a;
    void *b;
    SelfV *obj = (SelfV *)this;

    func_ov006_020edcb0();
    a = LoadFile(0xcb);
    b = LoadFile(0xcc);
    DecompressLZ16(a, (void *)0x6400000);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0xc0);
    GX::LoadOBJPltt(b, 0, 0xc0);
    Deallocate(a);
    Deallocate(b);
    obj->unkA8 = data_ov006_0213c954;
    obj->unkAC = obj->unkA8;
    func_ov004_020b04d0(0x20);
    obj->v18(-1);
    *data_ov006_02137560[0] = (*data_ov006_02137560[0] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[1] = (*data_ov006_02137560[1] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[2] = (*data_ov006_02137560[2] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[3] = (*data_ov006_02137560[3] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[4] = (*data_ov006_02137560[4] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[5] = (*data_ov006_02137560[5] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[6] = (*data_ov006_02137560[6] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[7] = (*data_ov006_02137560[7] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[8] = (*data_ov006_02137560[8] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[9] = (*data_ov006_02137560[9] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[10] = (*data_ov006_02137560[10] & 0xc1fffcff) | 0x100;
    *data_ov006_02137560[11] = (*data_ov006_02137560[11] & 0xc1fffcff) | 0x100;
    return 1;
}


// @symbol func_ov006_020edcb0
extern "C" {
void func_ov006_020edcb0(void){
  void *r6 = LoadFile(0x3b);
  void *r5 = LoadFile(0x3d);
  void *r4 = LoadFile(0x3c);
  {
    volatile unsigned short *m = (volatile unsigned short *)0x400000e;
    *m = (*m & ~3) | 1;
    *m = (*m & 0x43) | 0x614;
  }
  DecompressLZ16(r6, (void *)func_02054d88());
  func_02056314(r5, 0, 0x800);
  GX::LoadBGPltt((const void *)r4, 0x60, 0x1a0);
  data_0209d45c |= 8;
  Deallocate(r6);
  Deallocate(r5);
  Deallocate(r4);

  r6 = LoadFile(0x3e);
  r5 = LoadFile(0x40);
  r4 = LoadFile(0x3f);
  {
    volatile unsigned short *m = (volatile unsigned short *)0x400100e;
    *m = (*m & ~3) | 2;
    *m = (*m & 0x43) | 0x614;
  }
  DecompressLZ16(r6, _ZN3G2S13GetBG3CharPtrEv());
  func_020562b4(r5, 0, 0x800);
  GXS::LoadBGPltt((const void *)r4, 0x60, 0x1a0);
  data_0209d454 |= 8;
  Deallocate(r6);
  Deallocate(r5);
  Deallocate(r4);
}
}

