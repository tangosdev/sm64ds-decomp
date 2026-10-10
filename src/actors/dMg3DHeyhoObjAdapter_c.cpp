//cpp
/* dMg3DHeyhoObjAdapter_c translation unit, ov006 0x020c4048..0x020c6f8c, text only: the class's
   base constructor (its vtable and RTTI stay at their ROM homes in the module's unowned .data)
   plus 49 Heyho-minigame helper functions (see config/tu_manifest.d/ov006/dMg3DHeyhoObjAdapter_c.json).
   Function order is the reverse of the ROM: mwccarm 2004/b56 emits .text in reverse source order.
   Each former one-function C source sits in its own namespace so the per-file local types and
   extern declarations, which disagree with one another, stay as they were. */
#include "common.h"
#include "decl_common.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "dMgJump3DMario_c.h"

/* File 0x217 / 0x218 resource handles. The static initializer registers
 * their destructors; the spellings are this TU's, mapped onto
 * func_02017acc / func_02017ab4 and SharedFilePtr::Construct /
 * SharedFilePtr_Destruct_Anim. */
struct HeyhoAdapterModelFilePtr : SharedFilePtr {
    u32 words[2];

    HeyhoAdapterModelFilePtr(u32 fileID);
    ~HeyhoAdapterModelFilePtr();
};

struct HeyhoAdapterAnimationFileHandle : SharedFilePtr {
    u32 words[2];

    HeyhoAdapterAnimationFileHandle(u32 fileID);
    ~HeyhoAdapterAnimationFileHandle();
};

/* ---- func_ov006_020c6f70 ---- */
namespace n020c6f70 {
extern "C" {
extern void *_ZN9ModelAnimC1Ev(void *object);

int func_ov006_020c6f70(char *t)
{
    _ZN9ModelAnimC1Ev(t + 0x38);
    return (int)t;
}

}
}

/* ---- func_ov006_020c6f3c ---- */
namespace n020c6f3c {
extern "C" {
extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern HeyhoAdapterModelFilePtr data_ov006_02140330; extern HeyhoAdapterAnimationFileHandle data_ov006_02140338;
int func_ov006_020c6f3c(int* c){ _ZN13SharedFilePtr7ReleaseEv(&data_ov006_02140330); _ZN13SharedFilePtr7ReleaseEv(&data_ov006_02140338); _ZN9ModelAnimD1Ev((char*)c+0x38); return (int)c; }

}
}

/* ---- func_ov006_020c6e4c ---- */
struct BMD_File; struct BCA_File;
extern HeyhoAdapterModelFilePtr data_ov006_02140330;
extern HeyhoAdapterAnimationFileHandle data_ov006_02140338;
struct GObj { int w[8]; };
extern GObj *data_0209f5c0;
namespace n020c6e4c {
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *f, int a, int b, unsigned int u);


extern "C" void func_02016acc(char *m, int a);
extern "C" void func_02016b24(char *m, int a);
extern "C" void func_02016a14(char *m, int a);
extern "C" void func_02016a04(char *m, int a);
extern "C" void func_ov006_020c4c00(char *c);
extern "C" void func_ov006_020c4d20(char *p);



extern "C" int func_ov006_020c6e4c(char *c) {
    BMD_File *m = (BMD_File *)Model::LoadFile(data_ov006_02140330);
    BCA_File *a = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov006_02140338);
    if (((ModelBase *)(c + 0x38))->SetFile(m, 1, -1) == 0) {
        return 0;
    }
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)(c + 0x38), a, 0, 0x800, 0);
    func_02016acc(c + 0x38, 1);
    func_02016b24(c + 0x38, 2);
    int b = (*(unsigned short *)((char *)data_0209f5c0 + 0xc) == 0x175);
    if (b != 0) {
        func_02016a14(c + 0x38, 0x7fff);
        func_02016a04(c + 0x38, 0x7d40);
    }
    func_ov006_020c4c00(c);
    func_ov006_020c4d20(c);
    return 1;
}

}

/* ---- func_ov006_020c6ca4 ---- */
namespace n020c6ca4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[3]; } Blk3;
typedef struct { int w[4]; } Blk4;
typedef struct { int x, y, z, w; } Vec4;

extern void __register_global_object(void *object, void *destructor, void **node);
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void *_ZN7Vector3D1Ev(void *object);

extern int data_ov006_0214032c;
extern Vec3 data_ov006_02140364;
extern int data_ov006_02140358;
extern int data_ov006_02140318;
extern Vec3 data_ov006_0214037c;
extern int data_ov006_02140370;
typedef struct { int w[2]; } Pair;
extern Pair data_ov006_0213af38;

void func_ov006_020c6ca4(char* dst, char* src)
{
    if (!(data_ov006_0214032c & 1)) {
        data_ov006_02140364.x = -0x100000;
        data_ov006_02140364.y = 0;
        data_ov006_02140364.z = 0;
        __register_global_object(&data_ov006_02140364, (void *)&_ZN7Vector3D1Ev, (void**)&data_ov006_02140358);
        data_ov006_0214032c |= 1;
    }
    if (!(data_ov006_02140318 & 1)) {
        data_ov006_0214037c.x = 0x100000;
        data_ov006_0214037c.y = 0;
        data_ov006_0214037c.z = 0;
        __register_global_object(&data_ov006_0214037c, (void *)&_ZN7Vector3D1Ev, (void**)&data_ov006_02140370);
        data_ov006_02140318 |= 1;
    }

    *(int*)(dst) = *(int*)(src);
    *(Blk3*)(dst + 4) = *(Blk3*)(src + 4);
    *(Blk4*)(dst + 0x10) = *(Blk4*)(src + 0x10);
    *(Blk3*)(dst + 0x20) = *(Blk3*)(src + 0x20);
    *(int*)(dst + 0x2c) = *(int*)(src + 0x2c);

    if (*(volatile int*)(dst + 0x20) > 0) {
        Vec3 *p = (Vec3*)(dst + 0x9c);
        *(int*)(dst + 0x9c) = *(int*)(dst + 0x20);
        *(int*)(dst + 0xa0) = *(int*)(dst + 0x24);
        *(int*)(dst + 0xa4) = *(int*)(dst + 0x28);
        AddVec3(p, &data_ov006_0214037c, p);
    } else {
        Vec3 *q = (Vec3*)(dst + 0x9c);
        *(int*)(dst + 0x9c) = *(int*)(dst + 0x20);
        *(int*)(dst + 0xa0) = *(int*)(dst + 0x24);
        *(int*)(dst + 0xa4) = *(int*)(dst + 0x28);
        AddVec3(q, &data_ov006_02140364, q);
    }

    *(int*)(dst + 0xcc) = 0x1400;
    *(int*)(dst + 0xd0) = 0x1400;
    *(int*)(dst + 0xd4) = 0x1400;
    *(int*)(dst + 0xd8) = *(int*)dst;
    *(short*)(dst + 0xe6) = 0;
    *(short*)(dst + 0xec) = 8;
    *(int*)(dst + 0xe0) = 0;
    *(Pair*)(dst + 0x30) = data_ov006_0213af38;
}

}
}

/* ---- func_ov006_020c6a9c ---- */
namespace n020c6a9c {
extern "C" {
// @symbol func_ov006_020c6a9c
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int _Z15ApproachLinear2Rsss(short *v, short target, short step);


void func_ov006_020c6a9c(char *c)
{
    short *ang = (short *)(((int)c + 0xea));
    *ang = *ang + 0x200;

    *(int *)(c + 0xa8) = (int)(((*(int *)(c + 0x20) - *(int *)(c + 0x9c)) * 0x100LL + 0x800) >> 12);
    *(int *)(c + 0xac) = (int)(((*(int *)(c + 0x24) - *(int *)(c + 0xa0)) * 0x100LL + 0x800) >> 12);
    *(int *)(c + 0xb0) = (int)(((*(int *)(c + 0x28) - *(int *)(c + 0xa4)) * 0x100LL + 0x800) >> 12);
    AddVec3((struct Vector3 *)(c + 0x9c), (struct Vector3 *)(c + 0xa8), (struct Vector3 *)(c + 0x9c));

    if ((*(int *)(c + 0xa8) < 0 ? -*(int *)(c + 0xa8) : *(int *)(c + 0xa8)) < 0x10 &&
        (*(int *)(c + 0xac) < 0 ? -*(int *)(c + 0xac) : *(int *)(c + 0xac)) < 0x10) {
        *(int *)(c + 0x9c) = *(int *)(c + 0x20);
        *(int *)(c + 0xa0) = *(int *)(c + 0x24);
        *(int *)(c + 0xa4) = *(int *)(c + 0x28);

        switch (*(int *)(c + 0x2c)) {
        case 0:  func_ov006_020c63dc(c); break;
        case 1:  func_ov006_020c6348(c); break;
        case 2:  func_ov006_020c6248(c); break;
        case 3:  func_ov006_020c6188(c); break;
        case 4:  func_ov006_020c5ec8(c); break;
        case 5:  func_ov006_020c5cf4(c); break;
        case 6:  func_ov006_020c5aa4(c); break;
        case 7:  func_ov006_020c57d4(c); break;
        case 8:  func_ov006_020c561c(c); break;
        case 9:  func_ov006_020c54f4(c); break;
        case 10: func_ov006_020c5370(c); break;
        default: func_ov006_020c63dc(c); break;
        }
    }

    _Z15ApproachLinear2Rsss((short *)(c + 0xec), 0, 1);
    if (*(s16 *)(c + 0xec) != 1)
        return;
    func_ov006_020e6e3c(0x1c5, *(int *)(c + 0x9c));
}

}
}

/* ---- func_ov006_020c68f4 ---- */
namespace n020c68f4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[3]; } Blk3;
typedef struct { int w[4]; } Blk4;
typedef struct { int x, y, z, w; } Vec4;

extern void __register_global_object(void *object, void *destructor, void **node);
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void *_ZN7Vector3D1Ev(void *object);

extern int data_ov006_02140310;
extern Vec3 data_ov006_02140394;
extern int data_ov006_02140388;
extern int data_ov006_02140320;
extern Vec3 data_ov006_021403ac;
extern int data_ov006_021403a0;
typedef struct { int w[2]; } Pair;
extern Pair data_ov006_0213af60;

void func_ov006_020c68f4(char* dst, char* src)
{
    if (!(data_ov006_02140310 & 1)) {
        data_ov006_02140394.x = -0x100000;
        data_ov006_02140394.y = 0;
        data_ov006_02140394.z = 0;
        __register_global_object(&data_ov006_02140394, (void *)&_ZN7Vector3D1Ev, (void**)&data_ov006_02140388);
        data_ov006_02140310 |= 1;
    }
    if (!(data_ov006_02140320 & 1)) {
        data_ov006_021403ac.x = 0x100000;
        data_ov006_021403ac.y = 0;
        data_ov006_021403ac.z = 0;
        __register_global_object(&data_ov006_021403ac, (void *)&_ZN7Vector3D1Ev, (void**)&data_ov006_021403a0);
        data_ov006_02140320 |= 1;
    }

    *(int*)(dst) = *(int*)(src);
    *(Blk3*)(dst + 4) = *(Blk3*)(src + 4);
    *(Blk4*)(dst + 0x10) = *(Blk4*)(src + 0x10);
    *(Blk3*)(dst + 0x20) = *(Blk3*)(src + 0x20);
    *(int*)(dst + 0x2c) = *(int*)(src + 0x2c);

    if (*(volatile int*)(dst + 0x20) > 0) {
        Vec3 *p = (Vec3*)(dst + 0x9c);
        *(int*)(dst + 0x9c) = *(int*)(dst + 0x20);
        *(int*)(dst + 0xa0) = *(int*)(dst + 0x24);
        *(int*)(dst + 0xa4) = *(int*)(dst + 0x28);
        AddVec3(p, &data_ov006_021403ac, p);
    } else {
        Vec3 *q = (Vec3*)(dst + 0x9c);
        *(int*)(dst + 0x9c) = *(int*)(dst + 0x20);
        *(int*)(dst + 0xa0) = *(int*)(dst + 0x24);
        *(int*)(dst + 0xa4) = *(int*)(dst + 0x28);
        AddVec3(q, &data_ov006_02140394, q);
    }

    *(short*)(dst + 0xec) = 8;
    *(int*)(dst + 0xcc) = 0x1400;
    *(int*)(dst + 0xd0) = 0x1400;
    *(int*)(dst + 0xd4) = 0x1400;
    *(int*)(dst + 0xd8) = *(int*)dst;
    *(short*)(dst + 0xe6) = 0;
    *(int*)(dst + 0xe0) = 0;
    *(Pair*)(dst + 0x30) = data_ov006_0213af60;
}

}
}

/* ---- func_ov006_020c66bc ---- */
namespace n020c66bc {
extern "C" {
typedef struct { int x, y, z; } Vec3;

struct Obj;

extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern int LenVec3(Vec3 *v);
extern void _Z15ApproachLinear2Rsss(short *v, short t, short s);
extern int func_ov006_020e6e3c(int a, int b);
extern void func_ov006_020c63dc(char *p);
extern void func_ov006_020c6348(int *c);
extern void func_ov006_020c6248(int *c);
extern void func_ov006_020c6188(void *c);
extern void func_ov006_020c5ec8(char *self);
extern void func_ov006_020c5cf4(int *c);
extern void func_ov006_020c5aa4(struct Obj *o);
extern void func_ov006_020c57d4(struct Obj *o);
extern void func_ov006_020c561c(void *c);
extern void func_ov006_020c54f4(void *c);
extern void func_ov006_020c5370(char *c);
extern void func_ov006_020c49d8(void *c);
extern void func_ov006_020c47d4(void *c);

void func_ov006_020c66bc(char *c)
{
    int dx, dy, dz;
    int vx, vy;

    *(short *)(((int)c + 0xea)) += 0x200;

    dx = *(int *)(c + 0x20) - *(int *)(c + 0x9c);
    *(int *)(c + 0xa8) = (int)((((long long)dx << 8) + 0x800) >> 12);
    dy = *(int *)(c + 0x24) - *(int *)(c + 0xa0);
    *(int *)(c + 0xac) = (int)((((long long)dy << 8) + 0x800) >> 12);
    dz = *(int *)(c + 0x28) - *(int *)(c + 0xa4);
    *(int *)(c + 0xb0) = (int)((((long long)dz << 8) + 0x800) >> 12);

    AddVec3((Vec3 *)(c + 0x9c), (Vec3 *)(c + 0xa8), (Vec3 *)(c + 0x9c));

    vx = *(int *)(c + 0xa8);
    if (vx < 0) vx = -vx;
    if (vx < 0x18) {
        vy = *(int *)(c + 0xac);
        if (vy < 0) vy = -vy;
        if (vy < 0x18) {
            *(int *)(c + 0x9c) = *(int *)(c + 0x20);
            *(int *)(c + 0xa0) = *(int *)(c + 0x24);
            *(int *)(c + 0xa4) = *(int *)(c + 0x28);

            switch (*(int *)(c + 0x2c)) {
            case 0:  func_ov006_020c63dc(c); break;
            case 1:  func_ov006_020c6348((int *)c); break;
            case 2:  func_ov006_020c6248((int *)c); break;
            case 3:  func_ov006_020c6188(c); break;
            case 4:  func_ov006_020c5ec8(c); break;
            case 5:  func_ov006_020c5cf4((int *)c); break;
            case 6:  func_ov006_020c5aa4((struct Obj *)c); break;
            case 7:  func_ov006_020c57d4((struct Obj *)c); break;
            case 8:  func_ov006_020c561c(c); break;
            case 9:  func_ov006_020c54f4(c); break;
            case 10: func_ov006_020c5370(c); break;
            default: func_ov006_020c63dc(c); break;
            }

            func_ov006_020e6e3c(0x1c5, *(int *)(c + 0x9c));
        }
    }

    _Z15ApproachLinear2Rsss((short *)(c + 0xec), 0, 1);

    if (*(short *)(c + 0xec) == 1) {
        func_ov006_020e6e3c(0x1c5, *(int *)(c + 0x9c));
    }

    if (LenVec3((Vec3 *)(c + 0xa8)) < 0x1000) {
        func_ov006_020c49d8(c);
    } else {
        func_ov006_020c47d4(c);
    }
}

}
}

/* ---- func_ov006_020c64e4 ---- */
namespace n020c64e4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[2]; } W2;

extern void __register_global_object(void *object, void *destructor, void **node);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern void *_ZN7Vector3D1Ev(void *object);

extern int data_ov006_0213af70[2];
extern int data_ov006_0213afc0[2];
extern W2 data_ov006_0213afb8;
extern int data_ov006_02140300;
extern int data_ov006_0214030c;
extern Vec3 data_ov006_021403d0;
extern Vec3 data_ov006_021403e8;
extern void *data_ov006_021403c4;
extern void *data_ov006_021403dc;

void func_ov006_020c64e4(char *c)
{
    Vec3 locals[3];
    int *v;
    volatile int *q;
    unsigned base;

    v = (int *)(c + 0x30);
    q = (volatile int *)data_ov006_0213af70;
    if (v[0] == data_ov006_0213af70[0]
        && (v[1] == q[1] || *(int *)(c + 0x30) == 0))
        return;

    base = (unsigned)(int)c;
    v = (int *)(base + 0x30);
    q = (volatile int *)data_ov006_0213afc0;
    if (v[0] == data_ov006_0213afc0[0]
        && (v[1] == q[1] || *(int *)(c + 0x30) == 0))
        return;

    if ((data_ov006_02140300 & 1) == 0) {
        data_ov006_021403d0.x = -0x100000;
        data_ov006_021403d0.y = 0;
        data_ov006_021403d0.z = 0;
        __register_global_object(&data_ov006_021403d0, (void *)&_ZN7Vector3D1Ev, &data_ov006_021403c4);
        data_ov006_02140300 |= 1;
    }
    if ((data_ov006_0214030c & 1) == 0) {
        data_ov006_021403e8.x = 0x100000;
        data_ov006_021403e8.y = 0;
        data_ov006_021403e8.z = 0;
        __register_global_object(&data_ov006_021403e8, (void *)&_ZN7Vector3D1Ev, &data_ov006_021403dc);
        data_ov006_0214030c |= 1;
    }

    if (*(int *)(c + 0x9c) > 0) {
        Vec3_Add(&locals[1], (Vec3 *)(c + 0x9c), &data_ov006_021403e8);
        *(int *)(c + 0xb4) = locals[1].x;
        *(int *)(c + 0xb8) = locals[1].y;
        *(int *)(c + 0xbc) = locals[1].z;
    } else {
        Vec3_Add(&locals[2], (Vec3 *)(c + 0x9c), &data_ov006_021403d0);
        *(int *)(c + 0xb4) = locals[2].x;
        *(int *)(c + 0xb8) = locals[2].y;
        *(int *)(c + 0xbc) = locals[2].z;
    }

    *(int *)(c + 0xcc) = 0x1400;
    *(int *)(c + 0xd0) = 0x1400;
    *(int *)(c + 0xd4) = 0x1400;
    *(W2 *)(c + 0x30) = data_ov006_0213afb8;
}

}
}

/* ---- func_ov006_020c6400 ---- */
namespace n020c6400 {
extern "C" {
typedef struct { int x, y, z; } Vec3;

extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *out, const Vec3 *in, int scale);
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern void func_ov006_020c4d20(char *self);

void func_ov006_020c6400(char *self)
{
    Vec3 diff, scaled;
    int x, y;
    {
        short *pa = (short*)(self+0xea);
        *pa += 0x200;
    }
    Vec3_Sub(&diff, (Vec3*)(self+0xb4), (Vec3*)(self+0x9c));
    Vec3_MulScalar(&scaled, &diff, 0x100);
    *(int*)(self+0xa8) = scaled.x;
    *(int*)(self+0xac) = scaled.y;
    *(int*)(self+0xb0) = scaled.z;
    AddVec3((Vec3*)(self+0x9c), (Vec3*)(self+0xa8), (Vec3*)(self+0x9c));
    if (*(int*)(self+0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(self+0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(self+0xe6), -0x3000, 0x200);
    x = *(int*)(self+0xa8);
    if (x < 0) x = -x;
    if (x >= 0x20) return;
    y = *(int*)(self+0xac);
    if (y < 0) y = -y;
    if (y >= 0x20) return;
    func_ov006_020c4d20(self);
}

}
}

/* ---- func_ov006_020c63dc ---- */
namespace n020c63dc {
extern "C" {
struct S { int w[2]; };
extern struct S data_ov006_0213af88;
void func_ov006_020c63dc(char *p) { *(short *)(p + 0xe6) = 0; *(struct S *)(p + 0x30) = data_ov006_0213af88; }

}
}

/* ---- func_ov006_020c6378 ---- */
namespace n020c6378 {
extern "C" {
extern s16 data_02082214[];
extern void func_ov006_020c49d8(void* this_);

void func_ov006_020c6378(int this_)
{
    /* The two re-reads go through a const view of the object: without it
       mwcc 2004 CSEs the +0xea and +0xac field addresses into their own
       registers (ldrh ip,[ip] / add r1,r0,#0xac) and the function grows a
       word; the cartridge re-issues ldrh r2,[r0,#0xea] / ldr r1,[r0,#0xac]. */
    const char *ro = (const char*)this_;
    *(s16*)(this_ + 0xea) += 0x200;
    *(int*)(this_ + 0xa8) = 0;
    *(int*)(this_ + 0xac) = data_02082214[(*(const u16*)(ro + 0xea) >> 4) * 2] >> 3;
    *(int*)(this_ + 0xa0) += *(const int*)(ro + 0xac);
    func_ov006_020c49d8((void*)this_);
}

}
}

/* ---- func_ov006_020c6348 ---- */
namespace n020c6348 {
extern "C" {
struct S{int w[2];}; extern struct S data_ov006_0213af18;
void func_ov006_020c6348(int* c){ *(int*)((char*)c+0xa8)=0x800; *(int*)((char*)c+0xac)=0; *(int*)((char*)c+0xb0)=0; *(struct S*)((char*)c+0x30)=data_ov006_0213af18; }

}
}

/* ---- func_ov006_020c627c ---- */
namespace n020c627c {
extern "C" {
extern void AddVec3(void* a, void* b, void* c);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern void func_ov006_020c49d8(void* c);
extern short data_02082214[];

void func_ov006_020c627c(char* c)
{
    int v, w;
    *(short*)(((int)c + 0xea)) += 0x200;
    {
        int a = (unsigned short)*(unsigned short*)(c + 0xea) >> 4;
        *(int*)(((int)c + 0xac)) = data_02082214[a * 2] >> 3;
    }
    AddVec3(c + 0x9c, c + 0xa8, c + 0x9c);
    v = *(int*)(c + 0xa8);
    if (v < 0) {
        if (*(int*)(c + 0x9c) < -0x68000) {
            *(int*)(c + 0xa8) = -v;
            goto skip_else_if;
        }
    }
    if (v > 0) {
        if (*(int*)(c + 0x9c) > 0x68000)
            *(int*)(c + 0xa8) = -v;
    }
skip_else_if:
    w = *(int*)(c + 0xa8);
    if (w > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);
    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c6248 ---- */
namespace n020c6248 {
extern "C" {
struct S{int w[2];}; extern struct S data_ov006_0213af78;
void func_ov006_020c6248(int* c){ *(int*)((char*)c+0xa8)=0; *(int*)((char*)c+0xac)=0x800; *(int*)((char*)c+0xb0)=0; *(struct S*)((char*)c+0x30)=data_ov006_0213af78; *(short*)((char*)c+0xe6)=0; }

}
}

/* ---- func_ov006_020c61c4 ---- */
namespace n020c61c4 {
extern "C" {
// @symbol func_ov006_020c61c4
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *out);
void func_ov006_020c61c4(int this_) {
    int f24 = *(int*)(this_ + 0x24);
    int a0  = *(int*)(this_ + 0xa0);
    if (a0 < f24 + *(int*)(this_ + 0x18) && ((int*)this_)[43] < 0) {
        ((int*)this_)[43] = -((int*)this_)[43];
        ((short*)this_)[117] = 0;
    } else if (a0 > f24 + *(int*)(this_ + 0x1c) && ((int*)this_)[43] > 0) {
        ((int*)this_)[43] = -((int*)this_)[43];
        ((short*)this_)[117] = 0;
    }
    AddVec3((struct Vector3*)(this_ + 0x9c), (struct Vector3*)(this_ + 0xa8), (struct Vector3*)(this_ + 0x9c));
    func_ov006_020c49d8((void*)this_);
}

}
}

/* ---- func_ov006_020c6188 ---- */
namespace n020c6188 {
extern "C" {
struct S{int w[2];};
extern struct S data_ov006_0213af20;
void func_ov006_020c6188(void *c){
  *(short*)((char*)c+0xe6)=0;
  *(int*)((char*)c+0xa8)=*(int*)((char*)c+4);
  *(int*)((char*)c+0xac)=*(int*)((char*)c+8);
  *(int*)((char*)c+0xb0)=*(int*)((char*)c+0xc);
  *(struct S*)((char*)c+0x30)=data_ov006_0213af20;
}

}
}

/* ---- func_ov006_020c6088 ---- */
namespace n020c6088 {
extern "C" {
// @symbol func_ov006_020c6088
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);

void func_ov006_020c6088(char *c)
{
    int base_y = *(int*)(c + 0x24);
    int pos_y = *(int*)(c + 0xa0);
    if (pos_y < base_y) {
        int vel = *(int*)(c + 0xac);
        if (vel < 0) {
            *(int*)(c + 0xac) = -vel;
            *(short*)(c + 0xea) = 0;
            goto after_y;
        }
    }
    if (pos_y > base_y + 0x60000) {
        int vel = *(int*)(c + 0xac);
        if (vel > 0) {
            *(int*)(c + 0xac) = -vel;
            *(short*)(c + 0xea) = 0;
        }
    }
after_y:
    {
    int base_x = *(int*)(c + 0x20);
    int pos_x = *(int*)(c + 0x9c);
    if (pos_x < base_x - 0x60000) {
        int vel = *(int*)(c + 0xa8);
        if (vel < 0) {
            *(int*)(c + 0xa8) = -vel;
            *(short*)(c + 0xea) = 0;
            goto after_x;
        }
    }
    if (pos_x > base_x + 0x60000) {
        int vel = *(int*)(c + 0xa8);
        if (vel > 0) {
            *(int*)(c + 0xa8) = -vel;
            *(short*)(c + 0xea) = 0;
        }
    }
after_x:;
    }

    AddVec3((struct Vector3*)(c + 0x9c), (struct Vector3*)(c + 0xa8), (struct Vector3*)(c + 0x9c));
    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);
    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c5ec8 ---- */
namespace n020c5ec8 {
extern "C" {
typedef struct { int x, y, z; } Vec3;

extern int LenVec3(Vec3* v);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void Vec3_MulScalarInPlace(int* v, int s);
typedef struct { int w[2]; } S2;
extern S2 data_ov006_0213af68;

void func_ov006_020c5ec8(char* self)
{
    Vec3 v0, v1, v2, d0, d1, d2, diff;
    int len0, len1, len2;

    *(short*)(self + 0xe6) = 0;
    *(int*)(self + 0xa8) = *(int*)(self + 4);
    *(int*)(self + 0xac) = *(int*)(self + 8);
    *(int*)(self + 0xb0) = *(int*)(self + 0xc);
    *(int*)(self + 0xdc) = LenVec3((Vec3*)(self + 0xa8));

    {
        int y = *(int*)(self + 0x1c);
        int x = *(int*)(self + 0x10);
        v0.x = x;
        v0.y = y;
        v0.z = 0;
    }
    {
        int y = *(int*)(self + 0x1c);
        int x = *(int*)(self + 0x14);
        v1.x = x;
        v1.y = y;
        v1.z = 0;
    }
    {
        int a = *(int*)(self + 0x10);
        int b = *(int*)(self + 0x14);
        int c = *(int*)(self + 0x18);
        v2.x = (a + b) >> 1;
        v2.y = c;
        v2.z = 0;
    }

    Vec3_Sub(&d0, &v0, (Vec3*)(self + 0x9c));
    len0 = LenVec3(&d0);
    Vec3_Sub(&d1, &v1, (Vec3*)(self + 0x9c));
    len1 = LenVec3(&d1);
    Vec3_Sub(&d2, &v2, (Vec3*)(self + 0x9c));
    len2 = LenVec3(&d2);

    if (len2 < len1 && len2 < len0) {
        *(int*)(self + 0xb4) = v2.x;
        *(int*)(self + 0xb8) = v2.y;
        *(int*)(self + 0xbc) = v2.z;
    } else if (len1 < len0) {
        *(int*)(self + 0xb4) = v1.x;
        *(int*)(self + 0xb8) = v1.y;
        *(int*)(self + 0xbc) = v1.z;
    } else {
        *(int*)(self + 0xb4) = v0.x;
        *(int*)(self + 0xb8) = v0.y;
        *(int*)(self + 0xbc) = v0.z;
    }

    Vec3_Sub(&diff, (Vec3*)(self + 0xb4), (Vec3*)(self + 0x9c));
    *(int*)(self + 0xa8) = diff.x;
    *(int*)(self + 0xac) = diff.y;
    *(int*)(self + 0xb0) = diff.z;
    if (NormalizeVec3IfNonZero((Vec3*)(self + 0xa8)) == 0) {
        *(int*)(self + 0xa8) = *(int*)(self + 4);
        *(int*)(self + 0xac) = *(int*)(self + 8);
        *(int*)(self + 0xb0) = *(int*)(self + 0xc);
    } else {
        Vec3_MulScalarInPlace((int*)(self + 0xa8), *(int*)(self + 0xdc));
    }

    *(S2*)(self + 0x30) = data_ov006_0213af68;
}

}
}

/* ---- func_ov006_020c5d28 ---- */
namespace n020c5d28 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
extern int _Z14ApproachLinearRiii(int* v, int target, int step);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern void func_ov006_020c49d8(void* c);

void func_ov006_020c5d28(char* c)
{
    Vec3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = *(int*)(c + 0xa8);
    r5 = _Z14ApproachLinearRiii((int*)(c + 0x9c), *(int*)(c + 0xb4), t < 0 ? -t : t);
    t = *(int*)(c + 0xac);
    r0v = _Z14ApproachLinearRiii((int*)(c + 0xa0), *(int*)(c + 0xb8), t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x18)) {
        int c_a8 = *(int*)(c + 0xa8);
        if (c_a8 > 0) {
            *(int*)(c + 0xb4) = *(int*)(c + 0x14);
            *(int*)(c + 0xb8) = *(int*)(c + 0x1c);
        } else {
            *(int*)(c + 0xb4) = *(int*)(c + 0x10);
            *(int*)(c + 0xb8) = *(int*)(c + 0x1c);
        }
        Vec3_Sub(&sp0, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp0.x;
        *(int*)(c + 0xac) = sp0.y;
        *(int*)(c + 0xb0) = sp0.z;
        func_0203ce80(&spC, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    } else if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x1c)) {
        *(int*)(c + 0xb4) = (*(int*)(c + 0x10) + *(int*)(c + 0x14)) >> 1;
        *(int*)(c + 0xb8) = *(int*)(c + 0x18);
        Vec3_Sub(&sp18, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp18.x;
        *(int*)(c + 0xac) = sp18.y;
        *(int*)(c + 0xb0) = sp18.z;
        func_0203ce80(&sp24, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    }

    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);

    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c5cf4 ---- */
namespace n020c5cf4 {
extern "C" {
struct S{int w[2];}; extern struct S data_ov006_0213af58;
void func_ov006_020c5cf4(int* c){ *(int*)((char*)c+0xa8)=*(int*)((char*)c+4); *(int*)((char*)c+0xac)=*(int*)((char*)c+8); *(int*)((char*)c+0xb0)=*(int*)((char*)c+0xc); *(struct S*)((char*)c+0x30)=data_ov006_0213af58; }

}
}

/* ---- func_ov006_020c5bf8 ---- */
namespace n020c5bf8 {
extern "C" {
// @symbol func_ov006_020c5bf8
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);

void func_ov006_020c5bf8(char *c)
{
    int a8 = *(int*)(c + 0xa8);
    if (a8 < 0 && *(int*)(c + 0x9c) < *(int*)(c + 0x20) + *(int*)(c + 0x10)) {
        *(int*)(c + 0xa8) = -a8;
    } else if (a8 > 0 && *(int*)(c + 0x9c) > *(int*)(c + 0x20) + *(int*)(c + 0x14)) {
        *(int*)(c + 0xa8) = -a8;
    }
    int ac = *(int*)(c + 0xac);
    if (ac < 0 && *(int*)(c + 0xa0) < *(int*)(c + 0x24) + *(int*)(c + 0x18)) {
        *(int*)(c + 0xac) = -ac;
    } else if (ac > 0 && *(int*)(c + 0xa0) > *(int*)(c + 0x24) + *(int*)(c + 0x1c)) {
        *(int*)(c + 0xac) = -ac;
    }
    AddVec3((struct Vector3*)(c + 0x9c), (struct Vector3*)(c + 0xa8), (struct Vector3*)(c + 0x9c));
    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);
    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c5aa4 ---- */
namespace n020c5aa4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[2]; } W2;
extern int LenVec3(Vec3 *v);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void Vec3_MulScalarInPlace(int *v, int s);

struct Obj {
    int pad0;
    int f4, f8, fc;
    int f10, f14, f18, f1c;
    char gap20[0x9c - 0x20];
    Vec3 f9c;
    Vec3 fa8;
    Vec3 fb4;
    char gapc0[0xdc - 0xc0];
    int fdc;
    char gape0[0xe6 - 0xe0];
    short fe6;
};

extern W2 data_ov006_0213af40;

void func_ov006_020c5aa4(struct Obj *o)
{
    Vec3 a, b, da, db, t;
    int la;
    int ay, ax, by, bx;
    o->fa8.x = o->f4;
    o->fa8.y = o->f8;
    o->fa8.z = o->fc;
    o->fdc = LenVec3(&o->fa8);
    o->fe6 = 0;
    ay = o->f1c;
    ax = o->f10;
    a.x = ax;
    a.y = ay;
    a.z = 0;
    by = o->f18;
    bx = o->f14;
    b.x = bx;
    b.y = by;
    b.z = 0;
    Vec3_Sub(&da, &a, &o->f9c);
    la = LenVec3(&da);
    Vec3_Sub(&db, &b, &o->f9c);
    if (LenVec3(&db) < la) {
        o->fb4.x = a.x;
        o->fb4.y = a.y;
        o->fb4.z = a.z;
    } else {
        o->fb4.x = b.x;
        o->fb4.y = b.y;
        o->fb4.z = b.z;
    }
    Vec3_Sub(&t, &o->fb4, &o->f9c);
    o->fa8.x = t.x;
    o->fa8.y = t.y;
    o->fa8.z = t.z;
    if (NormalizeVec3IfNonZero(&o->fa8) == 0) {
        o->fa8.x = o->f4;
        o->fa8.y = o->f8;
        o->fa8.z = o->fc;
    } else {
        Vec3_MulScalarInPlace((int *)&o->fa8, o->fdc);
    }
    *(W2 *)((char *)o + 0x30) = data_ov006_0213af40;
}

}
}

/* ---- func_ov006_020c5928 ---- */
namespace n020c5928 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
extern int _Z14ApproachLinearRiii(int* v, int target, int step);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern void func_ov006_020c49d8(void* c);

void func_ov006_020c5928(char* c)
{
    Vec3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = *(int*)(c + 0xa8);
    r5 = _Z14ApproachLinearRiii((int*)(c + 0x9c), *(int*)(c + 0xb4), t < 0 ? -t : t);
    t = *(int*)(c + 0xac);
    r0v = _Z14ApproachLinearRiii((int*)(c + 0xa0), *(int*)(c + 0xb8), t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x18)) {
        *(int*)(c + 0xb4) = *(int*)(c + 0x10);
        *(int*)(c + 0xb8) = *(int*)(c + 0x1c);
        Vec3_Sub(&sp0, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp0.x;
        *(int*)(c + 0xac) = sp0.y;
        *(int*)(c + 0xb0) = sp0.z;
        func_0203ce80(&spC, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    } else if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x1c)) {
        *(int*)(c + 0xb4) = *(int*)(c + 0x14);
        *(int*)(c + 0xb8) = *(int*)(c + 0x18);
        Vec3_Sub(&sp18, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp18.x;
        *(int*)(c + 0xac) = sp18.y;
        *(int*)(c + 0xb0) = sp18.z;
        func_0203ce80(&sp24, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    }

    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);

    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c57d4 ---- */
namespace n020c57d4 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef struct { int w[2]; } W2;
extern int LenVec3(Vec3 *v);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void Vec3_MulScalarInPlace(int *v, int s);

struct Obj {
    int pad0;
    int f4, f8, fc;
    int f10, f14, f18, f1c;
    char gap20[0x9c - 0x20];
    Vec3 f9c;
    Vec3 fa8;
    Vec3 fb4;
    char gapc0[0xdc - 0xc0];
    int fdc;
    char gape0[0xe6 - 0xe0];
    short fe6;
};

extern W2 data_ov006_0213af50;

void func_ov006_020c57d4(struct Obj *o)
{
    Vec3 a, b, da, db, t;
    int la;
    int ay, ax, by, bx;
    o->fa8.x = o->f4;
    o->fa8.y = o->f8;
    o->fa8.z = o->fc;
    o->fdc = LenVec3(&o->fa8);
    o->fe6 = 0;
    ay = o->f1c;
    ax = o->f14;
    a.x = ax;
    a.y = ay;
    a.z = 0;
    by = o->f18;
    bx = o->f10;
    b.x = bx;
    b.y = by;
    b.z = 0;
    Vec3_Sub(&da, &a, &o->f9c);
    la = LenVec3(&da);
    Vec3_Sub(&db, &b, &o->f9c);
    if (LenVec3(&db) < la) {
        o->fb4.x = a.x;
        o->fb4.y = a.y;
        o->fb4.z = a.z;
    } else {
        o->fb4.x = b.x;
        o->fb4.y = b.y;
        o->fb4.z = b.z;
    }
    Vec3_Sub(&t, &o->fb4, &o->f9c);
    o->fa8.x = t.x;
    o->fa8.y = t.y;
    o->fa8.z = t.z;
    if (NormalizeVec3IfNonZero(&o->fa8) == 0) {
        o->fa8.x = o->f4;
        o->fa8.y = o->f8;
        o->fa8.z = o->fc;
    } else {
        Vec3_MulScalarInPlace((int *)&o->fa8, o->fdc);
    }
    *(W2 *)((char *)o + 0x30) = data_ov006_0213af50;
}

}
}

/* ---- func_ov006_020c5658 ---- */
namespace n020c5658 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
extern int _Z14ApproachLinearRiii(int* v, int target, int step);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void func_0203ce80(Vec3* dst, Vec3* src);
extern void Vec3_MulScalarInPlace(int* v, int s);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);
extern void func_ov006_020c49d8(void* c);

void func_ov006_020c5658(char* c)
{
    Vec3 sp0, spC, sp18, sp24;
    int r5, r0v;
    int t;

    t = *(int*)(c + 0xa8);
    r5 = _Z14ApproachLinearRiii((int*)(c + 0x9c), *(int*)(c + 0xb4), t < 0 ? -t : t);
    t = *(int*)(c + 0xac);
    r0v = _Z14ApproachLinearRiii((int*)(c + 0xa0), *(int*)(c + 0xb8), t < 0 ? -t : t);

    if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x18)) {
        *(int*)(c + 0xb4) = *(int*)(c + 0x14);
        *(int*)(c + 0xb8) = *(int*)(c + 0x1c);
        Vec3_Sub(&sp0, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp0.x;
        *(int*)(c + 0xac) = sp0.y;
        *(int*)(c + 0xb0) = sp0.z;
        func_0203ce80(&spC, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    } else if (r5 != 0 && r0v != 0 && *(int*)(c + 0xb8) == *(int*)(c + 0x1c)) {
        *(int*)(c + 0xb4) = *(int*)(c + 0x10);
        *(int*)(c + 0xb8) = *(int*)(c + 0x18);
        Vec3_Sub(&sp18, (Vec3*)(c + 0xb4), (Vec3*)(c + 0x9c));
        *(int*)(c + 0xa8) = sp18.x;
        *(int*)(c + 0xac) = sp18.y;
        *(int*)(c + 0xb0) = sp18.z;
        func_0203ce80(&sp24, (Vec3*)(c + 0xa8));
        Vec3_MulScalarInPlace((int*)(c + 0xa8), *(int*)(c + 0xdc));
        *(short*)(c + 0xea) = 0;
    }

    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);

    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c561c ---- */
namespace n020c561c {
extern "C" {
struct S{int w[2];};
extern struct S data_ov006_0213af30;
void func_ov006_020c561c(void *c){
  *(short*)((char*)c+0xe6)=0;
  *(int*)((char*)c+0xa8)=*(int*)((char*)c+4);
  *(int*)((char*)c+0xac)=*(int*)((char*)c+8);
  *(int*)((char*)c+0xb0)=*(int*)((char*)c+0xc);
  *(struct S*)((char*)c+0x30)=data_ov006_0213af30;
}

}
}

/* ---- func_ov006_020c5530 ---- */
namespace n020c5530 {
extern "C" {
// @symbol func_ov006_020c5530
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);

void func_ov006_020c5530(char *c)
{
    int a8 = *(int*)(c + 0xa8);
    if (a8 < 0 && *(int*)(c + 0x9c) < -0x68000) {
        *(int*)(c + 0xac) = -a8;
        *(int*)(c + 0xa8) = 0;
    } else if (a8 > 0 && *(int*)(c + 0x9c) > 0x68000) {
        *(int*)(c + 0xac) = -a8;
        *(int*)(c + 0xa8) = 0;
    }
    int ac = *(int*)(c + 0xac);
    if (ac < 0 && *(int*)(c + 0xa0) < 0x20000) {
        *(int*)(c + 0xa8) = ac;
        *(int*)(c + 0xac) = 0;
    } else if (ac > 0 && *(int*)(c + 0xa0) > 0xc0000) {
        *(int*)(c + 0xa8) = ac;
        *(int*)(c + 0xac) = 0;
    }
    AddVec3((struct Vector3*)(c + 0x9c), (struct Vector3*)(c + 0xa8), (struct Vector3*)(c + 0x9c));
    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);
    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c54f4 ---- */
namespace n020c54f4 {
extern "C" {
struct S{int w[2];};
extern struct S data_ov006_0213af48;
void func_ov006_020c54f4(void *c){
  *(short*)((char*)c+0xe6)=0;
  *(int*)((char*)c+0xa8)=*(int*)((char*)c+4);
  *(int*)((char*)c+0xac)=*(int*)((char*)c+8);
  *(int*)((char*)c+0xb0)=*(int*)((char*)c+0xc);
  *(struct S*)((char*)c+0x30)=data_ov006_0213af48;
}

}
}

/* ---- func_ov006_020c53f8 ---- */
namespace n020c53f8 {
extern "C" {
// @symbol func_ov006_020c53f8
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int _Z14ApproachLinearRsss(short *value, short target, short step);

void func_ov006_020c53f8(char *c)
{
    int a8 = *(int*)(c + 0xa8);
    if (a8 < 0 && *(int*)(c + 0x9c) < -0x68000) {
        *(int*)(c + 0xac) = a8;
        *(int*)(c + 0xa8) = 0;
    } else if (a8 > 0 && *(int*)(c + 0x9c) > 0x68000) {
        *(int*)(c + 0xac) = a8;
        *(int*)(c + 0xa8) = 0;
    }
    int ac = *(int*)(c + 0xac);
    if (ac < 0 && *(int*)(c + 0xa0) < -0xc8000) {
        *(int*)(c + 0xa8) = -ac;
        *(int*)(c + 0xac) = 0;
    } else if (ac > 0 && *(int*)(c + 0xa0) > -0x20000) {
        *(int*)(c + 0xa8) = -ac;
        *(int*)(c + 0xac) = 0;
    }
    AddVec3((struct Vector3*)(c + 0x9c), (struct Vector3*)(c + 0xa8), (struct Vector3*)(c + 0x9c));
    if (*(int*)(c + 0xa8) > 0)
        _Z14ApproachLinearRsss((short*)(c + 0xe6), 0x3000, 0x200);
    else
        _Z14ApproachLinearRsss((short*)(c + 0xe6), -0x3000, 0x200);
    func_ov006_020c49d8(c);
}

}
}

/* ---- func_ov006_020c5370 ---- */
namespace n020c5370 {
extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov006_0213af28[];

struct W2 { int w[2]; };

void func_ov006_020c5370(char *c)
{
    unsigned int r;
    *(short *)(c + 0xe6) = 0;
    r = (unsigned int)RandomIntInternal(&data_0209e650);
    r &= 0x7fffffff;
    *(short *)(c + 0xec) = (short)(((int)((r >> 0x13) * 0x2d) >> 0xc) + 0xf);
    *(int *)(c + 0xb4) = *(int *)(c + 0x9c);
    *(int *)(c + 0xb8) = *(int *)(c + 0xa0);
    *(int *)(c + 0xbc) = *(int *)(c + 0xa4);
    *(int *)(c + 0xa8) = *(int *)(c + 0x4);
    *(int *)(c + 0xac) = *(int *)(c + 0x8);
    *(int *)(c + 0xb0) = *(int *)(c + 0xc);
    *(struct W2 *)(c + 0x30) = *(struct W2 *)data_ov006_0213af28;
}

}
}

/* ---- func_ov006_020c4fa4 ---- */
bool ApproachLinear(short &value, short target, short step);
namespace n020c4fa4 {

typedef struct Vec3 { int x, y, z; } Vec3;

struct VB {
    virtual int *m0();
    virtual int *m1();
    virtual int m2();
};

extern "C" {
extern int _Z14ApproachLinearRiii(int *p, int t, int r);
extern int _Z15ApproachLinear2Rsss(short *p, short t, short r);
extern int RandomIntInternal(int *seed);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern int Vec3_Dist(Vec3 *a, Vec3 *b);
extern void Vec3_MulScalar(Vec3 *d, Vec3 *s, int m);
extern void Vec3_Sub(Vec3 *d, Vec3 *a, Vec3 *b);
extern void Vec3_Add(Vec3 *d, Vec3 *a, Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int DotVec3(Vec3 *a, Vec3 *b);
extern void func_ov006_020c49d8(char *c);
extern int data_0209e650;
extern VB *data_ov006_021403f4[];
}

extern "C" void func_ov006_020c4fa4(char *c)
{
    VB *best;
    Vec3 bp;
    Vec3 op;
    Vec3 delta;
    Vec3 nrm;
    Vec3 cand;
    Vec3 dir;
    Vec3 diff;
    Vec3 scaled;
    Vec3 proj;
    Vec3 sc2;
    Vec3 tgt;
    Vec3 vel;
    int bm, i, ra, rb;

    if (*(int *)(c + 0xe0) != 0) {
        _Z14ApproachLinearRiii((int *)(c + 0xb4), *(int *)(c + 0x9c), 0x1000);
        _Z14ApproachLinearRiii((int *)(c + 0xb8), *(int *)(c + 0xa0), 0x1000);
    }
    {
        int t = *(int *)(c + 0xa8);
        if (t < 0) t = -t;
        ra = _Z14ApproachLinearRiii((int *)(c + 0x9c), *(int *)(c + 0xb4), t);
    }
    {
        int t = *(int *)(c + 0xac);
        if (t < 0) t = -t;
        rb = _Z14ApproachLinearRiii((int *)(c + 0xa0), *(int *)(c + 0xb8), t);
    }
    if (ra != 0) *(int *)(c + 0xa8) = 0;
    if (rb != 0) *(int *)(c + 0xac) = 0;
    if (ra != 0 && rb != 0
        && ApproachLinear(*(short *)(c + 0xe6), 0, 0x200) != 0
        && _Z15ApproachLinear2Rsss((short *)(c + 0xec), 0, 1) != 0) {
        best = 0;
        bm = 0x20000;
        for (i = 0; i < 3; i++) {
            if (data_ov006_021403f4[i]->m2() != 0) {
                int *v;
                int dot;
                v = data_ov006_021403f4[i]->m0();
                op.x = v[0]; op.y = v[1]; op.z = v[2];
                Vec3_Sub(&delta, (Vec3 *)(c + 0x9c), &op);
                v = data_ov006_021403f4[i]->m1();
                nrm.x = v[0]; nrm.y = v[1]; nrm.z = v[2];
                if (NormalizeVec3IfNonZero(&nrm) != 0) {
                    dot = DotVec3(&nrm, &delta);
                    if (dot > 0) {
                        int d;
                        Vec3_MulScalar(&scaled, &nrm, dot);
                        Vec3_Add(&proj, &op, &scaled);
                        cand.x = proj.x; cand.y = proj.y; cand.z = proj.z;
                        d = Vec3_Dist((Vec3 *)(c + 0x9c), &cand);
                        if (d < bm) {
                            best = data_ov006_021403f4[i];
                            bp = cand;
                            bm = d;
                        }
                    }
                }
            }
        }
        if (best != 0) {
            Vec3_Sub(&dir, (Vec3 *)(c + 0x9c), &bp);
            if (NormalizeVec3IfNonZero(&dir) == 0) {
                dir.x = 0x1000; dir.y = 0; dir.z = 0;
            }
            Vec3_MulScalar(&sc2, &dir, 0x40000);
            Vec3_Add(&tgt, (Vec3 *)(c + 0x9c), &sc2);
            *(int *)(c + 0xb4) = tgt.x;
            *(int *)(c + 0xb8) = tgt.y;
            *(int *)(c + 0xbc) = tgt.z;
            {
                int v2 = *(int *)(c + 0xb4);
                if (v2 < -0x68000) v2 = -0x68000;
                else if (v2 > 0x68000) v2 = 0x68000;
                *(int *)(c + 0xb4) = v2;
            }
            {
                int v3 = *(int *)(c + 0xb8);
                if (v3 < -0xa8000) v3 = -0xa8000;
                else if (v3 > 0xa0000) v3 = 0xa0000;
                *(int *)(c + 0xb8) = v3;
            }
            {
                int len;
                Vec3_Sub(&diff, (Vec3 *)(c + 0xb4), (Vec3 *)(c + 0x9c));
                len = LenVec3((Vec3 *)(c + 4));
                if (NormalizeVec3IfNonZero(&diff) != 0) {
                    Vec3_MulScalar(&vel, &diff, len);
                    *(int *)(c + 0xa8) = vel.x;
                    *(int *)(c + 0xac) = vel.y;
                    *(int *)(c + 0xb0) = vel.z;
                } else {
                    int w = (int)(((long long)len * 0xb50 + 0x800) >> 12);
                    *(int *)(c + 0xa8) = w;
                    *(int *)(c + 0xac) = w;
                }
            }
            {
                int rr = RandomIntInternal(&data_0209e650) & 0x7fffffff;
                *(short *)(c + 0xec) = (short)(((int)((unsigned int)rr >> 0x13) * 0x2d >> 12) + 0xf);
            }
        }
    } else {
        int df = *(int *)(c + 0xb4) - *(int *)(c + 0x9c);
        if (df > 0) ApproachLinear(*(short *)(c + 0xe6), 0x3000, 0x200);
        else if (df < 0) ApproachLinear(*(short *)(c + 0xe6), -0x3000, 0x200);
    }
    func_ov006_020c49d8(c);
}

}

/* ---- func_ov006_020c4f68 ---- */
namespace n020c4f68 {
extern "C" {
struct S{int w[2];};
extern struct S data_ov006_0213af80;
extern void func_ov006_020e6e3c(int a, int b);
void func_ov006_020c4f68(int *c){
  *(short*)((char*)c+0xea)=0;
  func_ov006_020e6e3c(0x1c8, *(int*)((char*)c+0x9c));
  *(struct S*)((char*)c+0x30)=data_ov006_0213af80;
}

}
}

/* ---- func_ov006_020c4e8c ---- */
namespace n020c4e8c {
extern "C" {
#define AT(p, off) ((void*)(int)((char*)(p) + (off)))
int func_ov006_020e6e3c(int a, int b);
void func_ov006_020c4d3c(char* c);
extern short data_02082214[];

void func_ov006_020c4e8c(char* c) {
    if (*(short*)(c + 0xea) < 0x4000) {
        short* p = (short*)AT(c, 0xea);
        short* tbl = data_02082214;
        int ang, idx, idx1, r0;
        *p = *p + 0x400;
        ang = *(unsigned short*)(c + 0xea) >> 4;
        idx = ang * 2;
        idx1 = idx + 1;
        r0 = (int)(((long long)((tbl[idx] >> 1) + 0x1000) * (long long)0x1400 + 0x800) >> 0xc);
        *(int*)(c + 0xcc) = r0;
        *(int*)(c + 0xd0) = (int)(((long long)((tbl[idx1] >> 1) + 0x800) * (long long)0x1400 + 0x800) >> 0xc);
        *(int*)(c + 0xd4) = r0;
        return;
    }
    func_ov006_020e6e3c(0x1c7, *(int*)(c + 0x9c));
    func_ov006_020c4d3c(c);
}

}
}

/* ---- func_ov006_020c4d3c ---- */
namespace n020c4d3c {
extern "C" {
typedef int Fix12;
extern void* _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12 x, Fix12 y, Fix12 z);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern void func_ov006_020bfff8(void* a, void* b, int* c, int* d);
extern int func_ov004_020b04c0(void);
extern void func_ov006_020ef05c(int a, int b, int c);
extern void func_ov006_020c4d20(char* p);

extern int data_ov006_02140304[];
extern int data_ov006_02140308;
extern int data_ov006_02140314;
extern void* data_ov006_02141a50;
extern void* data_ov006_02141a40;

void func_ov006_020c4d3c(char* c)
{
    void* a;
    void* b;
    int v0, v1;
    int t;

    a = _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x122,
        *(int*)(c + 0x9c) << 3, *(int*)(c + 0xa0) << 3, *(int*)(c + 0xa4) << 3);
    b = _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x123,
        *(int*)(c + 0x9c) << 3, *(int*)(c + 0xa0) << 3, *(int*)(c + 0xa4) << 3);
    t = (int)(*(int*)(c + 0xd8) << 0x1c) >> 0x10;
    if (a) *(int*)((char*)a + 0x50) = t;
    if (b) *(int*)((char*)a + 0x50) = t;

    _Z14ApproachLinearRiii(data_ov006_02140304, 0, 1);
    _Z14ApproachLinearRiii(&data_ov006_02140308, 0x270f, 1);

    if (data_ov006_02140314 != 0) {
        if (*(int*)(c + 0xa0) > 0) {
            func_ov006_020bfff8(data_ov006_02141a50, (void*)(c + 0x9c), &v0, &v1);
            v1 = v1 - (func_ov004_020b04c0() + 0xc0);
        } else {
            func_ov006_020bfff8(data_ov006_02141a40, (void*)(c + 0x9c), &v0, &v1);
        }
        v1 = v1 - 0x20;
        func_ov006_020ef05c(v0 << 0xc, v1 << 0xc, (short)data_ov006_02140308);
    }

    func_ov006_020c4d20(c);
}

}
}

/* ---- func_ov006_020c4d20 ---- */
namespace n020c4d20 {
extern "C" {
struct S { int w[2]; };
extern struct S data_ov006_0213af90;
void func_ov006_020c4d20(char *p) { *(struct S *)(p + 0x30) = data_ov006_0213af90; }

}
}

/* ---- func_ov006_020c4d1c ---- */
namespace n020c4d1c {
extern "C" {
void func_ov006_020c4d1c(void)
{
}

}
}

/* ---- func_ov006_020c4cd8 ---- */
namespace n020c4cd8 {
extern "C" {
/* local extern: byte-proved alias spelling for a call by mangled name on a member subobject, as in the original one-function source */
void _ZN15dExtFrameCtrl_c7AdvanceEv(void *);
void func_ov006_020c4c00(void *c);
}
struct Foo {
  char pad[0x30];
  void (Foo::*pmf)();
};
extern "C" void func_ov006_020c4cd8(struct Foo *c){
  (c->*(c->pmf))();
  _ZN15dExtFrameCtrl_c7AdvanceEv((char*)c+0x88);
  func_ov006_020c4c00(c);
}

}

/* ---- func_ov006_020c4c54 ---- */
namespace n020c4c54 {
// @symbol func_ov006_020c4c54
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

extern "C" void Vec3_MulScalar(Vector3 *res, Vector3 *v, int scalar);
struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(Vector3 *p);
};
extern "C" void func_ov006_020c4c54(int this_) {
    Vector3 v;
    int *p = (int*)(((int)this_ + 0x30));
    Vector3 *d = &data_ov006_0213af98;
    if (p[0] == d->x) {
        if (p[1] == d->y)
            return;
        if (((volatile int*)this_)[0xc] == 0)
            return;
    }
    Vec3_MulScalar(&v, (Vector3*)(this_ + 0xcc), *(int*)(this_ + 0xd8));
    Sub *s = (Sub*)(this_ + 0x38);
    s->m5(&v);
}

}

/* ---- func_ov006_020c4c00 ---- */
namespace n020c4c00 {
extern "C" {
// @symbol func_ov006_020c4c00
/* recovered: shared common types */
extern struct Matrix4x3 data_020a0e68;
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3* m, short angY);
void func_ov006_020c4c00(char* c) {
    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x9c), *(int*)(c+0xa0), *(int*)(c+0xa4));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c+0xe6));
    *(struct Matrix4x3*)(c+0x54) = data_020a0e68;
}

}
}

/* ---- func_ov006_020c49d8 ---- */
namespace n020c49d8 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
typedef int (**VT)(void*);

struct Self {
    char pad0[0x9c];
    Vec3 pos;                /* 0x9c */
    Vec3 vel;                /* 0xa8 */
    char padb4[0xd8 - 0xb4];
    int d8;                  /* 0xd8 */
};

extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int DotVec3(Vec3* a, Vec3* b);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void func_ov006_020c4f68(struct Self* self);
extern char* data_ov006_021403f4[];

#define O  (data_ov006_021403f4[i])
#define OM (((char* volatile*)data_ov006_021403f4)[i])

void func_ov006_020c49d8(struct Self* self)
{
    int i;
    int lim = (int)(((long long)self->d8 * 0x12000 + 0x800) >> 12);
    int half = lim >> 1;

    for (i = 0; i < 3; i++) {
        Vec3 a;
        Vec3 b;
        Vec3 d;
        Vec3 c;

        if ((*(VT*)O)[2](O) == 0) continue;

        Vec3_Sub(&a, (Vec3*)(*(VT*)O)[0](O), &self->pos);
        a.y -= half;
        if (a.y < 0) {
            if (a.y > -0x18000) a.y = 0;
            else a.y += 0x18000;
        } else {
            if (a.x > 0x2000) a.x -= 0x2000;
            else if (a.x < -0x2000) a.x += 0x2000;
            else a.x = 0;
        }

        Vec3_Sub(&b, (Vec3*)(*(VT*)O)[1](O), &self->vel);
        Vec3_Add(&c, &a, &b);
        if (LenVec3(&c) > lim) continue;

        if (NormalizeVec3IfNonZero(&a) == 0) {
            *(short*)(OM + 0x10) = 1;
            *(int*)(O + 4) = self->pos.x;
            *(int*)(O + 8) = self->pos.y;
            *(int*)(O + 0xc) = self->pos.z;
            func_ov006_020c4f68(self);
        }

        d.x = 0;
        d.y = 0x1000;
        d.z = 0;
        if (DotVec3(&d, &a) > 0) {
            *(short*)(OM + 0x10) = 1;
            *(int*)(O + 4) = self->pos.x;
            *(int*)(O + 8) = self->pos.y;
            *(int*)(O + 0xc) = self->pos.z;
            func_ov006_020c4f68(self);
            return;
        } else {
            *(short*)(OM + 0x10) = 2;
            *(int*)(O + 4) = self->pos.x;
            *(int*)(O + 8) = self->pos.y;
            *(int*)(O + 0xc) = self->pos.z;
        }
    }
}

}
}

/* ---- func_ov006_020c47d4 ---- */
namespace n020c47d4 {
extern "C" {
typedef int Fix12;
typedef struct Vec3 { Fix12 x, y, z; } Vec3;

typedef struct Obj Obj;
typedef struct ObjVt {
    Vec3 *(*f0)(Obj *);
    Vec3 *(*f1)(Obj *);
    int   (*f2)(Obj *);
} ObjVt;
struct Obj {
    ObjVt *vt;      /* 0x0 */
    Vec3 vec;       /* 0x4 */
    short flag;     /* 0x10 */
};

typedef struct Self {
    char _pad0[0x9c];
    Vec3 pos;       /* 0x9c */
    Vec3 vel;       /* 0xa8 */
    char _padb4[0xd8 - 0xb4];
    int speed;      /* 0xd8 */
} Self;

extern Obj *data_ov006_021403f4[3];
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern Fix12 DotVec3(const Vec3 *a, const Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int NormalizeVec3IfNonZero(Vec3 *v);
extern void func_ov006_020c4f68(Self *self);

void func_ov006_020c47d4(Self *self)
{
    int i;
    int thresh = (int)(((long long)self->speed * 0x12000 + 0x800) >> 12);

    for (i = 0; i < 3; i++) {
        Vec3 disp, rel, up, sum;
        Vec3 *p, *q;
        Obj *o;

        if (data_ov006_021403f4[i]->vt->f2(data_ov006_021403f4[i]) == 0)
            continue;

        p = data_ov006_021403f4[i]->vt->f0(data_ov006_021403f4[i]);
        Vec3_Sub(&disp, p, &self->pos);

        disp.y -= thresh >> 1;
        if (disp.y < 0) {
            if (disp.y > -0x18000)
                disp.y = 0;
            else
                disp.y += 0x18000;
        } else {
            if (disp.x > 0x2000)
                disp.x -= 0x2000;
            else if (disp.x < -0x2000)
                disp.x += 0x2000;
            else
                disp.x = 0;
        }

        q = data_ov006_021403f4[i]->vt->f1(data_ov006_021403f4[i]);
        Vec3_Sub(&rel, q, &self->vel);
        Vec3_Add(&sum, &disp, &rel);

        if (LenVec3(&sum) > thresh)
            continue;

        if (NormalizeVec3IfNonZero(&disp) == 0) {
            data_ov006_021403f4[i]->flag = 1;
            o = data_ov006_021403f4[i];
            o->vec.x = self->pos.x;
            o->vec.y = self->pos.y;
            o->vec.z = self->pos.z;
            func_ov006_020c4f68(self);
        }

        up.x = 0;
        up.y = 0x1000;
        up.z = 0;
        if (DotVec3(&up, &disp) <= 0)
            continue;

        if (rel.y >= 0)
            continue;

        data_ov006_021403f4[i]->flag = 1;
        o = data_ov006_021403f4[i];
        o->vec.x = self->pos.x;
        o->vec.y = self->pos.y;
        o->vec.z = self->pos.z;
        func_ov006_020c4f68(self);
        return;
    }
}

}
}

/* ---- func_ov006_020c4710 ---- */
namespace n020c4710 {
extern "C" {
typedef struct UID { int low, high; } UID;

typedef struct dActor_c {
    int pad[0x30 / 4];
    UID id;                             /* 0x30 */
    unsigned char pad2[0xe6 - 0x38];
    short fE6;                          /* 0xe6 */
} dActor_c;

extern UID data_ov006_0213afa0;
extern UID data_ov006_0213afb0;

int func_ov006_020c4710(dActor_c *a)
{
    int result = 0;
    UID *p = (UID *)(int)(&a->id);
    UID *c = (UID *)(int)(&data_ov006_0213afa0);
    int neq = 1;
    if (p->low == c->low) {
        if (p->high == c->high || a->id.low == 0)
            neq = 0;
    }
    if (neq != 0) {
        int flag = 1;
        int neq2 = flag;
        UID *q = (UID *)(int)((long long)(unsigned int)&a->id);
        UID *c2 = (UID *)(int)(&data_ov006_0213afb0);
        if (q->low == c2->low) {
            if (q->high == c2->high || a->id.low == 0)
                neq2 = 0;
        }
        if (neq2 == 0) {
            if (a->fE6 != 0)
                flag = 0;
        }
        if (flag != 0)
            result = 1;
    }
    return result;
}

}
}

/* ---- func_ov006_020c4684 ---- */
namespace n020c4684 {
extern "C" {
extern int func_ov006_020c6e4c(char *p);
extern int data_ov006_02140328;
extern int data_ov006_02140304;
extern char *data_ov006_02140324;
extern int data_ov006_02140314;
int func_ov006_020c4684(char *ptr, int n);
int func_ov006_020c4684(char *ptr, int n){
    int i;
    int off;
    data_ov006_02140314 = 0;
    data_ov006_02140328 = n;
    data_ov006_02140304 = n;
    data_ov006_02140324 = ptr;
    if (n > 0) {
        i = 0;
        off = 0;
        do {
            if (func_ov006_020c6e4c(data_ov006_02140324 + off) == 0)
                return 0;
            i++;
            off += 0xf0;
        } while (i < data_ov006_02140328);
    }
    return 1;
}

}
}

/* ---- func_ov006_020c44b4 ---- */
namespace n020c44b4 {
extern "C" {
extern void *data_ov006_021402fc;
extern int data_ov006_02140304;
extern int data_ov006_02140308;
extern int data_ov006_0214031c;
extern char *data_ov006_02140324;
extern int data_ov006_02140328;
extern int data_ov006_021403b8[];
extern int data_0209e650;
extern char data_ov006_0212d560[];
extern char data_ov006_0212c9f0[];

extern void func_ov006_020c6ca4(char *dst, char *src);
extern void func_ov006_020c4c00(char *c);
extern int RandomIntInternal(int *seed);

void func_ov006_020c44b4(void *arg0, int arg1)
{
    int count;
    int sl;
    int sb;
    int r8;
    int r4;
    int *seed;
    int r6;
    int sp0;
    int sp4;
    int sp8;
    int sp10;
    int spc;
    int r1;
    char *base;
    int t;

    count = data_ov006_02140328;
    data_ov006_0214031c = arg1;
    data_ov006_021402fc = arg0;
    data_ov006_02140304 = count;
    data_ov006_02140308 = 0;
    if (count <= 0)
        return;

    seed = &data_0209e650;
    sl = 0;
    r8 = sl;
    sp4 = sl;
    r4 = sl;
    spc = sl;
    sp10 = sl;
    sp8 = sl;

    do {
        if (sl < 3) {
            base = data_ov006_02140324 + r8;
            *(short *)(base + 0xea) = (short)sp8;
            func_ov006_020c6ca4(
                data_ov006_02140324 + r8,
                data_ov006_0212d560 + data_ov006_0214031c * 0x90 + sp4);
            func_ov006_020c4c00(data_ov006_02140324 + r8);
        } else {
            t = RandomIntInternal(seed) & 0x7fffffff;
            t = (int)((unsigned)t >> 0x13);
            t = t << 2;
            sb = t >> 0xc;
            t = RandomIntInternal(seed) & 0x7fffffff;
            t = (int)((unsigned)t >> 0x13);
            t = t << 2;
            sp0 = t >> 0xc;
            r1 = spc;
            r6 = (sl - 3) | (r1 - r1);
            if (r6 > 0) {
                do {
                    if (sb == data_ov006_021403b8[r1]) {
                        t = RandomIntInternal(seed) & 0x7fffffff;
                        t = (int)((unsigned)t >> 0x13);
                        t = t << 2;
                        sb = t >> 0xc;
                        r1 = r4;
                    }
                    r1 = r1 + 1;
                } while (r1 < r6);
            }
            data_ov006_021403b8[sl - 3] = sb;
            base = data_ov006_02140324 + r8;
            *(short *)(base + 0xea) = (short)sp10;
            func_ov006_020c6ca4(
                data_ov006_02140324 + r8,
                data_ov006_0212c9f0 + sb * 0xc0 + sp0 * 0x30);
            func_ov006_020c4c00(data_ov006_02140324 + r8);
        }
        sp4 = sp4 + 0x30;
        r8 = r8 + 0xf0;
        sl = sl + 1;
    } while (sl < data_ov006_02140328);
}

}
}

/* ---- func_ov006_020c42bc ---- */
namespace n020c42bc {
extern "C" {
/* func_ov006_020c42bc at 0x020c42bc (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 */
typedef struct { int x, y, z; } Vec3;

struct Obj {
    char pad0[0x9c];
    Vec3 pos;             /* 0x9c */
    Vec3 vel;             /* 0xa8 */
    char padb4[0xe0 - 0xb4];
    int e0;               /* 0xe0 */
    char pade4[0xf0 - 0xe4];
};

extern struct Obj* data_ov006_02140324;
extern int data_ov006_02140328;

extern void func_ov006_020c4cd8(struct Obj* o);
extern int func_ov006_020c4710(struct Obj* o);
extern void Vec3_Sub(Vec3* out, Vec3* a, Vec3* b);
extern int LenVec3(Vec3* v);
extern int NormalizeVec3IfNonZero(Vec3* v);
extern void AddVec3(Vec3* a, Vec3* b, Vec3* c);
extern void SubVec3(Vec3* a, Vec3* b, Vec3* c);

void func_ov006_020c42bc(void)
{
    Vec3 v1;
    Vec3 v2;
    struct Obj* oi;
    struct Obj* oj;
    Vec3* pi;
    Vec3* pj;
    int i, j, k, t;
    Vec3* ai;

    for (i = 0; i < data_ov006_02140328; i++) {
        func_ov006_020c4cd8(&data_ov006_02140324[i]);
    }
    for (k = 0; k < data_ov006_02140328; k++) {
        data_ov006_02140324[k].e0 = 0;
    }
    for (t = 0; t < data_ov006_02140328; t++) {
        if (func_ov006_020c4710(&data_ov006_02140324[t]) != 0) {
            oi = &data_ov006_02140324[t];
            pi = &oi->pos;
            ai = &oi->vel;
            for (j = t + 1; j < data_ov006_02140328; j++) {
                if (func_ov006_020c4710(&data_ov006_02140324[j]) != 0) {
                    oj = (struct Obj*)((long long)(int)&data_ov006_02140324[j]);
                    pj = &oj->pos;
                    Vec3_Sub(&v1, pi, pj);
                    Vec3_Sub(&v2, ai, &oj->vel);
                    if (LenVec3(&v1) >= 0x12000) continue;
                    if (LenVec3(&v2) >= 0x100) continue;
                    if (NormalizeVec3IfNonZero(&v1) == 0) continue;
                    AddVec3(pi, &v1, pi);
                    SubVec3(pj, &v1, pj);
                    data_ov006_02140324[t].e0 = 1;
                    data_ov006_02140324[j].e0 = 1;
                }
            }
        }
    }
}

}
}

/* ---- func_ov006_020c425c ---- */
namespace n020c425c {
extern "C" {
extern void func_ov006_020c4c54(int);
extern int data_ov006_02140328;
extern int data_ov006_02140324;
void func_ov006_020c425c(void){
  int i = 0;
  if(data_ov006_02140328 > 0){
    int off = 0;
    do {
      func_ov006_020c4c54(data_ov006_02140324 + off);
      i++;
      off += 0xf0;
    } while(i < data_ov006_02140328);
  }
}

}
}

/* ---- func_ov006_020c4148 ---- */
namespace n020c4148 {
extern "C" {
typedef struct Ent {
    char _pad0[0x30];
    int x;
    int y;
    char _pad38[0xb8];
} Ent;

extern int data_ov006_02140328;
extern Ent *data_ov006_02140324;
extern int data_ov006_0213afc8[2];
extern char data_ov006_0212ccf0;
extern int data_ov006_02140304;
extern int data_0209e650;

extern int RandomIntInternal(int *seed);
extern void func_ov006_020c68f4(char *a, char *b);
extern void _Z14ApproachLinearRiii(int *x, int target, int step);

void func_ov006_020c4148(void)
{
    int i;
    for (i = 0; i < data_ov006_02140328; i++) {
        int *v = (int *)(&data_ov006_02140324[i].x);
        volatile int *q = (volatile int *)data_ov006_0213afc8;
        if (v[0] != data_ov006_0213afc8[0]
            || (v[1] != q[1] && data_ov006_02140324[i].x != 0))
            continue;
        {
            int r1 = RandomIntInternal(&data_0209e650);
            int r2 = RandomIntInternal(&data_0209e650);
            unsigned v0 = (unsigned)(r1 & 0x7fffffff) >> 0x13;
            unsigned u0 = (unsigned)(r2 & 0x7fffffff) >> 0x13;
            func_ov006_020c68f4(
                (char *)data_ov006_02140324 + i * 0xf0,
                &data_ov006_0212ccf0
                    + (((int)(v0 * 9)) >> 0xc) * 0xf0
                    + (((int)(u0 * 5)) >> 0xc) * 0x30);
            _Z14ApproachLinearRiii(&data_ov006_02140304, data_ov006_02140328, 1);
            return;
        }
    }
}

}
}

/* ---- func_ov006_020c40e8 ---- */
namespace n020c40e8 {
extern "C" {
extern void func_ov006_020c64e4(int);
extern int data_ov006_02140328;
extern int data_ov006_02140324;
void func_ov006_020c40e8(void){
  int i = 0;
  if(data_ov006_02140328 > 0){
    int off = 0;
    do {
      func_ov006_020c64e4(data_ov006_02140324 + off);
      i++;
      off += 0xf0;
    } while(i < data_ov006_02140328);
  }
}

}
}

/* ---- func_ov006_020c4060 ---- */
namespace n020c4060 {
extern "C" {
// Scans data_ov006_02140328 entries (stride 0xf0) from data_ov006_02140324:
// returns 0 if any entry's pair at +0x30 mismatches data_ov006_0213afa8
// (y-mismatch tolerated when x is 0), else 1. Leaf, no callees.
typedef struct Ent {
    char _pad0[0x30];  // 0x00
    int x;             // 0x30
    int y;             // 0x34
    char _pad38[0xb8]; // 0x38 (stride 0xf0)
} Ent;

extern int data_ov006_02140328;
extern Ent* data_ov006_02140324;
extern int data_ov006_0213afa8[2];

int func_ov006_020c4060(void) {
    int i;
    for (i = 0; i < data_ov006_02140328; i++) {
        int* v = (int*)(&data_ov006_02140324[i].x);
        volatile int* q = (volatile int*)data_ov006_0213afa8;
        if (v[0] != data_ov006_0213afa8[0]
            || (v[1] != q[1] && data_ov006_02140324[i].x != 0))
            return 0;
    }
    return 1;
}

}
}

// @symbol _ZN22dMg3DHeyhoObjAdapter_cC2Ev
dMg3DHeyhoObjAdapter_c::dMg3DHeyhoObjAdapter_c() : mCommand(0)
{
}

/* Definitions stay after the last .text function so they do not insert
 * a function into the reverse-emitted .text run. Construction order is
 * the model handle, then the animation handle. */
HeyhoAdapterModelFilePtr data_ov006_02140330(0x217);
HeyhoAdapterAnimationFileHandle data_ov006_02140338(0x218);
