//cpp
/* HAND-ASSEMBLED translation unit -- ov014/ChainChomp (29 function(s)).
 * tubuild create refused this TU (legacy bodies wrapped in extern "C" { }),
 * so this is a raw concatenation of the complete legacy files in REVERSE
 * ROM order (mwccarm emits one .text section per function in the reverse
 * of source order). Conflicting declarations were reconciled by hand; see
 * the manifest notes.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111308  src/_ZN10ChainChompD1Ev.cpp
 *   [1] 0x021113bc  src/_ZN10ChainChompD0Ev.cpp
 *   [2] 0x02111484  src/func_ov014_02111484.c
 *   [3] 0x021114d8  src/func_ov014_021114d8.c
 *   [4] 0x0211150c  src/func_ov014_0211150c.c
 *   [5] 0x021115c0  src/func_ov014_021115c0.c
 *   [6] 0x021115ec  src/func_ov014_021115ec.cpp
 *   [7] 0x02111a6c  src/func_ov014_02111a6c.cpp
 *   [8] 0x02111af0  src/func_ov014_02111af0.c
 *   [9] 0x02111b70  src/func_ov014_02111b70.c
 *   [10] 0x02111ca8  src/func_ov014_02111ca8.cpp
 *   [11] 0x02111dc4  src/func_ov014_02111dc4.cpp
 *   [12] 0x02111e14  src/func_ov014_02111e14.c
 *   [13] 0x02111e74  src/func_ov014_02111e74.cpp
 *   [14] 0x02111ebc  src/func_ov014_02111ebc.cpp
 *   [15] 0x02111f08  src/func_ov014_02111f08.cpp
 *   [16] 0x02111f54  src/func_ov014_02111f54.cpp
 *   [17] 0x02111fb8  src/func_ov014_02111fb8.c
 *   [18] 0x02111fe0  src/func_ov014_02111fe0.c
 *   [19] 0x02112114  src/func_ov014_02112114.c
 *   [20] 0x021122dc  src/func_ov014_021122dc.c
 *   [21] 0x0211236c  src/func_ov014_0211236c.c
 *   [22] 0x0211250c  src/func_ov014_0211250c.c
 *   [23] 0x02112788  src/func_ov014_02112788.c
 *   [24] 0x0211294c  src/_ZN10ChainChomp16CleanupResourcesEv.cpp
 *   [25] 0x02112994  src/_ZN10ChainChomp6RenderEv.cpp
 *   [26] 0x021129ec  src/_ZN10ChainChomp8BehaviorEv.cpp
 *   [27] 0x02112b14  src/_ZN10ChainChomp13InitResourcesEv.cpp
 *   [28] 0x02112d1c  src/ChainChomp_Spawn.cpp
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- ChainChomp_Spawn, 0x02112d1c, size 0xf0 */
/* -------------------------------------------------------------------------- */
extern "C" {
void* _ZN7fBase_cnwEj(unsigned int);
void _ZN12dEnemyBase_cC2Ev(void*);
int _ZN10dCcAcPos_cC1Ev(void*);
int _ZN9ModelAnimC1Ev(void*);
int _ZN11ShadowModelC1Ev(void*);
int func_020733a8(void*, int, int, void*, void*);
extern int _ZTV10ChainChomp[];
extern void _ZN5ModelD1Ev();
extern void _ZN5ModelC1Ev();
extern void _ZN11ShadowModelD1Ev();
extern void _ZN7Vector3D1Ev();
extern void func_0203d384();
void* ChainChomp_Spawn(void){
  char* c = (char*)_ZN7fBase_cnwEj(0x620);
  if(c){
    _ZN12dEnemyBase_cC2Ev(c);
    *(int**)c = &_ZTV10ChainChomp[2]; /* +8: this TU defines the vtable */
    _ZN10dCcAcPos_cC1Ev(c+0x110);
    _ZN9ModelAnimC1Ev(c+0x150);
    _ZN11ShadowModelC1Ev(c+0x1b4);
    func_020733a8(c+0x1dc, 7, 0x50, (void*)_ZN5ModelC1Ev, (void*)_ZN5ModelD1Ev);
    func_020733a8(c+0x40c, 7, 0x28, (void*)_ZN11ShadowModelC1Ev, (void*)_ZN11ShadowModelD1Ev);
    func_020733a8(c+0x524, 7, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
    func_020733a8(c+0x578, 7, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
  }
  return c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- _ZN10ChainChomp13InitResourcesEv, 0x02112b14, size 0x208 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ChainChomp13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChainChomp.h"
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);
extern "C" void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern "C" void _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *pos, int fix, int t, unsigned int a, unsigned int b);
extern "C" void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);

extern char data_ov014_02114980;
extern char data_ov014_02114970;

int ChainChomp::InitResources()
{
    unsigned char *c = (unsigned char *)((void *)this);
    void *f;
        int i;
    unsigned char *p;
    void *spawned;
    int *px;
    int *py;
    int *pz;
    int one;


    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov014_02114968);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x150, f, 1, 1);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov014_02114978);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov014_02114980);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov014_02114970);

    p = c + 0x1dc;
    i = 0;
    do {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(p, *(void **)((char *)&data_ov014_02114978 + 4), 1, 1);
        i = i + 1;
        p = p + 0x50;
    } while (i < 7);

    _ZN11ShadowModel12InitCylinderEv(c + 0x1b4);
    {
    int si; unsigned char *sp;
    si = 0; sp = c + 0x40c;
    do {
        _ZN11ShadowModel12InitCylinderEv(sp);
        si = si + 1;
        sp = sp + 0x28;
    } while (si < 7);
    }

    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;

    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
            c + 0x110, c, v, 0x96000, 0x12c000, 0x200004, 0x26ff0);
    }

    func_ov014_02111ebc(c, 1);

        {
        int cnt = 0;
        unsigned char *dst = c;
        do {
            *(int *)(dst + 0x524) = *(int *)(c + 0x5c);
            cnt = cnt + 1;
            *(int *)(dst + 0x528) = *(int *)(c + 0x60);
            *(int *)(dst + 0x52c) = *(int *)(c + 0x64);
            dst = dst + 0xc;
        } while (cnt < 7);
    }


    *(int *)(c + 0x5ec) = *(int *)(c + 0x5c);
    *(int *)(c + 0x5f0) = *(int *)(c + 0x60);
    *(int *)(c + 0x5f4) = *(int *)(c + 0x64);

    spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        0x1b, 0x11, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
    *(int *)(c + 0x608) = *(int *)((unsigned char *)spawned + 4);
    one = 1;
    *(unsigned char *)((unsigned char *)spawned + 0x320) = (unsigned char)one;
    *(int *)(c + 0x60c) = 0;

    px = (int *)(c + 0x5c);
    *px = *px + 0xc8000;
    py = (int *)(c + 0x60);
    *py = *py + 0xc8000;
    pz = (int *)(c + 0x64);
    *pz = *pz + 0xc8000;

    *(int *)(c + 0x5f8) = 0x50000;
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    return one;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- _ZN10ChainChomp8BehaviorEv, 0x021129ec, size 0x128 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ChainChomp8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ChainChomp.h"
extern "C" {
int func_ov014_02111fb8(char* c);
char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int a, void* b);
void func_ov014_02111f08(void* c);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* cc);
void func_ov014_02112114(void* c);
void func_ov014_02111fe0(char* c);
void func_ov014_0211250c(char* c);
void func_ov014_0211236c(char* c);
void func_ov014_021122dc(char* c);
void func_ov014_02112788(char* c);
void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* self, void* v);
void _ZN5dCc_c5ClearEv(void* self);
char* _ZN8dActor_c13ClosestPlayerEv(char* self);
void _ZN5dCc_c6UpdateEv(void* self);
}

int ChainChomp::Behavior()
{
    unk_61c = 0;
    {
        int v = unk_5f0 + 0xc8000;
        if (mPosY <= v) {
            mPosY = v;
            if (unk_61d == 0) {
                func_ov014_02111fb8(((char*)this));
            }
            unk_61c = 1;
        }
    }
    unk_61d = unk_61c;
    if (unk_60c == 0) {
        char* r = _ZN8dActor_c15FindWithActorIDEjPS_(0x29, 0);
        unk_60c = *(int*)(r + 4);
    }
    func_ov014_02111f08(((char*)this));
    _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), ((char*)this) + 0x110);
    func_ov014_02112114(((char*)this));
    if (unk_605 == 0) {
        func_ov014_02111fe0(((char*)this));
    }
    func_ov014_0211250c(((char*)this));
    if (unk_605 == 0) {
        func_ov014_0211236c(((char*)this));
        func_ov014_021122dc(((char*)this));
    }
    func_ov014_02112788(((char*)this));
    {
        int v[3];
        v[0] = data_ov014_02114700[0];
        v[1] = data_ov014_02114700[1];
        v[2] = data_ov014_02114700[2];
        _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(((char*)this) + 0x110, v);
    }
    _ZN5dCc_c5ClearEv((char*)&mdCcAcPos_c);
    if (*(unsigned char*)(_ZN8dActor_c13ClosestPlayerEv(((char*)this)) + 0x6fb) == 0) {
        _ZN5dCc_c6UpdateEv((char*)&mdCcAcPos_c);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- _ZN10ChainChomp6RenderEv, 0x02112994, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ChainChomp6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "ChainChomp.h"
struct A;
struct B {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(A* arg);
};

int ChainChomp::Render()
{
    B *b = (B*)((char *)&mModelAnim);
    b->m5((A*)((char *)&mScaleX));
    
    int j = 0;
    char *p2 = ((char *)this) + 0x1dc;
    for (;;) {
        B *b2 = (B*)p2;
        b2->m5((A*)0);
        j++;
        p2 += 0x50;
        if (j >= 7) break;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- _ZN10ChainChomp16CleanupResourcesEv, 0x0211294c, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ChainChomp16CleanupResourcesEv

#include "ChainChomp.h"
#include "SharedFilePtr.h"

extern "C" {
}

int ChainChomp::CleanupResources()
{
    ((SharedFilePtr *)&data_ov014_02114968)->Release();  /* decl_common's char view; same object */
    ((SharedFilePtr *)&data_ov014_02114978)->Release();
    ((SharedFilePtr *)&data_ov014_02114980)->Release();
    ((SharedFilePtr *)&data_ov014_02114970)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov014_02112788, 0x02112788, size 0x1c4 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov014_02112788
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* self, void* sm, void* mtx, int a, int b, unsigned int g);


extern const struct Matrix4x3 IDENTITY_MATRIX4X3;

void func_ov014_02112788(char* c) {
    struct Matrix4x3 tmp;
    int i;
    int t;
    char* m;
    char* e;
    char* o;
    char* sm;
    Matrix4x3_FromRotationXYZExt(c+0x16c, *(s16*)(c+0x8c), *(s16*)(c+0x8e), *(s16*)(c+0x90));
    *(int*)(c+0x190) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x194) = *(int*)(c+0x60) >> 3;
    *(int*)(c+0x198) = *(int*)(c+0x64) >> 3;
    t = *(int*)(c+0x60) - *(int*)(c+0x5f0);
    if (t <= 0x1000) t = 0x1000;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c+0x1b4, c+0x16c,
        0x15e000 - (int)(((long long)t * 0x180 + 0x800) >> 12),
        t + 0x28000,
        0xf);
    tmp = IDENTITY_MATRIX4X3;
    sm = c + 0x40c;
    i = 0;
    m = c + 0x1dc;
    e = c;
    o = c;
    for (; i < 7; i++) {
        *(struct Matrix4x3*)(m+0x1c) = tmp;
        *(int*)(o+0x21c) = *(int*)(e+0x524) >> 3;
        *(int*)(o+0x220) = *(int*)(e+0x528) >> 3;
        *(int*)(o+0x224) = *(int*)(e+0x52c) >> 3;
        t = *(int*)(e+0x528) - *(int*)(c+0x5f0);
        if (t <= 0x1000) t = 0x1000;
        _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, sm, m+0x1c,
            0x78000 - (int)(((long long)t * 0x180 + 0x800) >> 12),
            t + 0x28000,
            0xf);
        m += 0x50;
        e += 0xc;
        o += 0x50;
        sm += 0x28;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov014_0211250c, 0x0211250c, size 0x27c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
typedef struct { int x, y, z; } Vec3;

extern void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(void* v, void* m, void* dst);
extern void Vec3_Add(void* out, void* a, void* b);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int Vec3_HorzLen(void* v);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void Vec3_Sub(void* out, void* a, void* b);
extern void Vec3_MulScalar(void* out, void* v, int s);

extern int data_020a0e68[];

void func_ov014_0211250c(char *c)
{
    int yoff;
    Vec3 dvec;
    Vec3 head;
    Vec3 rotated;
    Vec3 saved;
    Vec3 tmp;
    Vec3 sum;
    Vec3 diff;
    Vec3 mul;
    int scale;
    Vec3 *seg;
    Vec3 *dst;
    int *bound;
    int state;
    short angY;
    short angX;
    Vec3 *prev;
    int i;

    head.x = 0;
    head.y = 0;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;
    head.z = -0xc8000;

    /* setup order is load-bearing: seg before state so add/cmp schedule matches */
    seg = (Vec3 *)(c + 0x530);
    dst = (Vec3 *)(c + 0x584);
    state = *(int *)(c + 0x610);
    bound = (int *)(c + 0x5d0);
    scale = 0xb68;
    yoff = -0x5000;

    if (state == 2 || state == 4) {
        if (*(unsigned char *)(c + 0x605) == 0) {
            scale += 0x7800;
            yoff = 0;
        } else {
            scale += 0x190;
        }
    }

    Matrix4x3_FromRotationXYZExt(
        data_020a0e68,
        *(short *)(c + 0x8c),
        *(short *)(c + 0x8e),
        *(short *)(c + 0x90));
    MulVec3Mat4x3(&head, data_020a0e68, &rotated);
    Vec3_Add(&tmp, (Vec3 *)(c + 0x5c), &rotated);
    saved.x = tmp.x;
    saved.y = tmp.y;
    saved.z = tmp.z;
    *(int *)(c + 0x524) = tmp.x;
    *(int *)(c + 0x528) = saved.y;
    *(int *)(c + 0x52c) = saved.z;

    head.z = *(int *)(c + 0x5f8);
    head.x = 0;
    head.y = 0;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;
    i = 1;

    for (; i < 7; i++, bound++) {
        if (i == 0)
            prev = &saved;
        else
            prev = (Vec3 *)((char *)seg - 0xc);

        dvec.x = (seg->x - prev->x) + dst->x;
        dvec.z = (seg->z - prev->z) + dst->z;
        {
            int t = (seg->y + dst->y) + yoff;
            if (t <= *bound)
                t = *bound;
            dvec.y = t - prev->y;
        }

        angY = _ZN4cstd5atan2E5Fix12IiES1_(dvec.x, dvec.z);
        angX = (short)(-_ZN4cstd5atan2E5Fix12IiES1_(dvec.y, Vec3_HorzLen(&dvec)));
        Matrix4x3_FromRotationY(data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, angX);
        MulVec3Mat4x3(&head, data_020a0e68, &rotated);

        dst->x = seg->x;
        dst->y = seg->y;
        dst->z = seg->z;
        Vec3_Add(&sum, prev, &rotated);
        seg->x = sum.x;
        seg->y = sum.y;
        seg->z = sum.z;
        Vec3_Sub(&diff, seg, dst);
        Vec3_MulScalar(&mul, &diff, scale);
        dst->x = mul.x;
        dst->y = mul.y;
        dst->z = mul.z;

        if (*bound <= *(int *)(c + 0x5f0))
            *bound = *(int *)(c + 0x5f0) + 0x28000;

        seg = (Vec3 *)((char *)seg + 0xc);
        dst = (Vec3 *)((char *)dst + 0xc);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov014_0211236c, 0x0211236c, size 0x1a0 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void Vec3_Sub(void* out, void* a, void* b);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int Vec3_HorzLen(void* v);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern void MulVec3Mat4x3(void* v, void* m, void* dst);
extern void Vec3_Add(void* out, void* a, void* b);
extern void Vec3_MulScalar(void* out, void* v, int s);
extern int data_020a0e68[];

void func_ov014_0211236c(char* c)
{
    Vec3 tmp;
    Vec3 head;
    Vec3 rotated;
    Vec3 sum;
    Vec3 prevpos;
    Vec3 diff;
    Vec3 delta;
    Vec3 scaled;
    Vec3 mul;
    short angY;
    Vec3* target;
    Vec3* cur;
    short angX;
    int i;
    int z;

    z = *(int*)(c + 0x5f8);
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    prevpos.x = *(int*)(c + 0x5ec);
    {
        int py = *(int*)(c + 0x5f0);
        prevpos.y = py;
        prevpos.z = *(int*)(c + 0x5f4);
        prevpos.y = py + 0x1e000;
    }

    z = *(int*)(c + 0x5f8);
    head.x = 0;
    head.y = 0;
    head.z = z;
    rotated.x = 0;
    rotated.y = 0;
    rotated.z = 0;

    cur = (Vec3*)(c + 0x56c);
    i = 6;
    do {
        target = (i == 6) ? &prevpos : (Vec3*)((char*)cur + 0xc);
        Vec3_Sub(&diff, cur, target);
        tmp.x = diff.x;
        tmp.y = diff.y;
        tmp.z = diff.z;
        angY = _ZN4cstd5atan2E5Fix12IiES1_(tmp.x, tmp.z);
        angX = (short)(-_ZN4cstd5atan2E5Fix12IiES1_(tmp.y, Vec3_HorzLen(&tmp)));
        Matrix4x3_FromRotationY(data_020a0e68, angY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, angX);
        MulVec3Mat4x3(&head, data_020a0e68, &rotated);
        sum.x = cur->x;
        sum.y = cur->y;
        sum.z = cur->z;
        Vec3_Add(&delta, target, &rotated);
        cur->x = delta.x;
        cur->y = delta.y;
        cur->z = delta.z;
        Vec3_Sub(&scaled, cur, &sum);
        Vec3_MulScalar(&mul, &scaled, 0xb68);
        sum.x = mul.x;
        sum.y = mul.y;
        sum.z = mul.z;
        i = i - 1;
        cur = (Vec3*)((char*)cur - 0xc);
    } while (i >= 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov014_021122dc, 0x021122dc, size 0x90 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void Vec3_Sub(void *out, void *a, void *b);
extern void Vec3_MulScalar(void *out, void *v, int s);
extern void AddVec3(void *a, void *b, void *c);

void func_ov014_021122dc(char *c)
{
    char *p;
    int i;
    int diff[3];
    int scaled[3];

    Vec3_Sub(diff, c + 0x5c, c + 0x524);
    p = c + 0x524;
    i = 0;
    do {
        Vec3_MulScalar(scaled, diff, (7 - i) / 7);
        AddVec3(p, scaled, p);
        i++;
        p += 0xc;
    } while (i < 7);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov014_02112114, 0x02112114, size 0x1c8 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern void *_ZN8dActor_c10FindWithIDEj(unsigned id);
extern void func_ov014_02111ebc(void *c, int i);
extern int Vec3_HorzAngle(void *a, void *b);
extern int AngleDiff(int a, int b);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *o, void *v, unsigned a, int fx, unsigned b, unsigned d, unsigned e);
extern void func_ov102_0214ae1c(void *o);

void func_ov014_02112114(void *cc)
{
    char *c = (char*)cc;
    char *e;
    int id;
    unsigned h;
    int ang;

    if ((unsigned)(*(int*)(c + 0x610) - 3) <= 1)
        return;
    id = *(int*)(c + 0x134);
    if (id == 0)
        return;
    e = (char*)_ZN8dActor_c10FindWithIDEj(id);
    if (e == 0)
        return;

    h = *(unsigned short*)(e + 0xc);
    if ((int)(h == 9) != 0) {
        if (*(int*)(c + 0x130) & 0x2000) {
            *(int*)(c + 0x80) = 0x2000;
            *(int*)(c + 0x84) = *(int*)(c + 0x80);
            *(int*)(c + 0x88) = *(int*)(c + 0x84);
            func_ov014_02111ebc(c, 0);
            return;
        }
    }

    if (*(int*)(c + 0x130) & 0x4000) {
        *(int*)(c + 0x80) = 0x2000;
        *(int*)(c + 0x84) = *(int*)(c + 0x80);
        *(int*)(c + 0x88) = *(int*)(c + 0x84);
        func_ov014_02111ebc(c, 4);
        return;
    }

    if ((int)(h == 0xbf) != 0) {
        ang = Vec3_HorzAngle(c + 0x5c, e + 0x5c);
        if (*(int*)(c + 0x130) & 0x10) {
            func_ov014_02111ebc(c, 5);
            *(short*)(c + 0x94) = *(short*)(e + 0x8e);
        }
        if (AngleDiff(ang, *(short*)(c + 0x8e)) < 0x4000) {
            int v[3];
            v[0] = *(int*)(c + 0x5c);
            v[1] = *(int*)(c + 0x60);
            v[2] = *(int*)(c + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(e, v, 3, 0xc000, 1, 0, 1);
        }
    }

    if (*(int*)(c + 0x610) == 4)
        return;
    if ((int)(*(unsigned short*)(e + 0xc) == 0xce) != 0) {
        func_ov102_0214ae1c(e);
        func_ov014_02111ebc(c, 4);
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov014_02111fe0, 0x02111fe0, size 0x134 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol func_ov014_02111fe0
/* recovered: shared common types */
#include "common.h"
typedef int Fix12i;
typedef long long s64;


extern void Vec3_Sub(void* out, void* a, void* b);
extern int LenVec3(struct Vector3* v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_Add(void* out, void* a, void* b);

void func_ov014_02111fe0(char* c){
    struct Vector3 v;
    struct Vector3 out;
    int len, lim;
    Vec3_Sub(&v, (struct Vector3*)(c + 0x5c), (struct Vector3*)(c + 0x5ec));
    len = LenVec3(&v);
    lim = *(int*)(c + 0x5f8) * 7 + 0xc8000;
    if (len <= lim) return;
    v.x = (int)(((s64)v.x * _ZN4cstd4fdivEii(lim, len) + 0x800) >> 12);
    v.y = (int)(((s64)v.y * _ZN4cstd4fdivEii(lim, len) + 0x800) >> 12);
    v.z = (int)(((s64)v.z * _ZN4cstd4fdivEii(lim, len) + 0x800) >> 12);
    Vec3_Add(&out, (struct Vector3*)(c + 0x5ec), &v);
    *(int*)(c + 0x5c) = out.x;
    *(int*)(c + 0x60) = out.y;
    *(int*)(c + 0x64) = out.z;
    if (*(int*)(c + 0x610) != 2) return;
    if (*(int*)(c + 0x5f8) == 0x64000) {
        *(int*)(c + 0x98) = 0;
        *(int*)(c + 0xa8) = 0;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov014_02111fb8, 0x02111fb8, size 0x28 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int func_0201267c(int,void*);
extern void _ZN8dActor_c15HugeLandingDustEb(void*,int);
int func_ov014_02111fb8(char* c){
  func_0201267c(0x39, (char*)c+0x74);
  _ZN8dActor_c15HugeLandingDustEb(c,1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov014_02111f54, 0x02111f54, size 0x64 */
/* -------------------------------------------------------------------------- */
extern "C" {
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
char* _ZN8dActor_c13ClosestPlayerEv(char* a);
int _ZN6Player17SetNoControlStateEhih(void* p, unsigned char a, int b, unsigned char c);
void func_ov014_02111ebc(void* c, int i);
int func_ov014_02111f54(void* c){
  char* r4=(char*)c;
  void* a=_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(r4+0x608));
  if(*(unsigned char*)((char*)a+0x31e)!=0) goto fail;
  {
    void* p=_ZN8dActor_c13ClosestPlayerEv(r4);
    if(_ZN6Player17SetNoControlStateEhih(p,4,-1,0)==0) goto fail;
    func_ov014_02111ebc(r4,3);
    *(unsigned char*)(r4+0x605)=1;
    return 1;
  }
fail:
  return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov014_02111f08, 0x02111f08, size 0x4c */
/* -------------------------------------------------------------------------- */
struct C; typedef void (C::*PMF)();
struct Entry { char pad[8]; PMF pmf; char tail[20 - 8 - sizeof(PMF)]; };
extern Entry data_ov014_0211476c[];
struct C { char pad[0x610]; int idx; };
extern "C" void func_ov014_02111f08(void *vc) {
  C *c = (C *)vc;
  int j = c->idx;
  (c->*data_ov014_0211476c[j].pmf)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov014_02111ebc, 0x02111ebc, size 0x4c */
/* -------------------------------------------------------------------------- */
namespace ent0 {  /* this member reads the state table's +0 field; ordinal 15's view
                     reads +8 -- two honest views of one table, isolated by namespace */
struct Entry { void (C::*pmf)(); char rest[12]; };
extern "C" Entry data_ov014_0211476c[];
}
extern "C" void func_ov014_02111ebc(void *vc, int i) {
  C *c = (C *)vc;
  c->idx = i;
  int j = c->idx;
  (c->*ent0::data_ov014_0211476c[j].pmf)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov014_02111e74, 0x02111e74, size 0x48 */
/* -------------------------------------------------------------------------- */
struct BCA_File;
/* (ModelAnim: real header type in scope; call stays on the mangled spelling with
   int in place of the by-value Fix12<int> -- notes/mwccarm-codegen.md 6az) */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void*, int, int, unsigned int);
extern "C" void func_ov014_02111e74(char* c){
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x98)=0;
  *(short*)(c+0x500+0xfc)=0x78;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim*)(c+0x150), *(BCA_File**)((char*)&data_ov014_02114980 + 4), 0, 0x1000, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov014_02111e14, 0x02111e14, size 0x60 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern unsigned short DecIfAbove0_Short(unsigned short*);
extern void func_ov014_02111ebc(void*, int);
extern void _Z14ApproachLinearRiii(int*, int, int);
void func_ov014_02111e14(char* c){
  *(int*)(c+0xa8)=0;
  _Z14ApproachLinearRiii((int*)(c+0x80), 0x1000, 0x500);
  *(int*)(c+0x88)=*(int*)(c+0x80);
  *(int*)(c+0x84)=*(int*)(c+0x88);
  if(DecIfAbove0_Short((unsigned short*)(c+0x5fc))!=0) return;
  func_ov014_02111ebc(c, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov014_02111dc4, 0x02111dc4, size 0x50 */
/* -------------------------------------------------------------------------- */
extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
void func_ov014_02111dc4(char *c){
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x9c)=-0x2000;
  *(short*)(c+0x5fc)=0x78;
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x150,(void*)((int*)&data_ov014_02114980)[1],0,0x1000,0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov014_02111ca8, 0x02111ca8, size 0x11c */
/* -------------------------------------------------------------------------- */
typedef short s16;
extern "C" {
int func_ov014_02111f54(void* c);
void ApproachAngle(void* p, int a, int b, int c, int d);
unsigned short DecIfAbove0_Short(unsigned short* p);
void _Z14ApproachLinearRiii(int* p, int to, int step);
void _Z14ApproachLinearRsss(short* p, short to, short step);
int _ZN8dActor_c13DistToCPlayerEv(void* self);
short _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
int AngleDiff(int a, int b);
void func_ov014_02111ebc(void* c, int i);
int _ZN9Animation7AdvanceEv(void* self);

void func_ov014_02111ca8(char* c){
    if (func_ov014_02111f54(c)) return;
    ApproachAngle(c + 0x8c, 0, 4, 0x200, 0x80);
    DecIfAbove0_Short((unsigned short*)(c + 0x5fc));
    _Z14ApproachLinearRiii((int*)(c + 0x5f8), 0x50000, 0x1000);
    _Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x602), 0x190);
    _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x400);
    if (*(unsigned char*)(c + 0x61c)) {
        int d = _ZN8dActor_c13DistToCPlayerEv(c);
        *(short*)(c + 0x602) = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
        *(short*)(c + 0x94) = *(short*)(c + 0x8e);
        *(int*)(c + 0x98) = 0xa000;
        *(int*)(c + 0xa8) = 0x14000;
        *(short*)(c + 0x600) = 0;
        if (d < 0x500000 &&
            AngleDiff(*(short*)(c + 0x602), *(short*)(c + 0x8e)) < 0x800 &&
            *(unsigned short*)(c + 0x5fc) == 0) {
            func_ov014_02111ebc(c, 2);
        }
    }
    _ZN9Animation7AdvanceEv(c + 0x1a0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov014_02111b70, 0x02111b70, size 0x138 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
namespace call3_267c { extern "C" int func_0201267c(int, void *, int); } /* this member byte-requires the three-argument call (r2 set); the TU's file-scope view is (int, void*) */
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern char *_ZN8dActor_c13ClosestPlayerEv(char *);
extern short Vec3_VertAngle(const void *, const void *);
extern short data_02082214[];

void func_ov014_02111b70(char *c)
{
    int tgt[3];
    char *p;
    int a;
    int i;
    int v;

    *((int *)(c + 0x9c)) = 0;
    call3_267c::func_0201267c(0x3a, c + 0x74, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x150, (void *)((int *)&data_ov014_02114970)[1], 0, 0x1000, 0);
    p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);

    /* ROM load order: y, z, x — then y+0x50000, store x, setup call, store y/z */
    {
        int y = *((int *)(p + 0x60));
        int z = *((int *)(p + 0x64));
        int x = *((int *)(p + 0x5c));
        tgt[0] = x;
        tgt[1] = y + 0x50000;
        tgt[2] = z;
    }

    a = (unsigned short)Vec3_VertAngle(c + 0x5c, tgt);
    i = (a >> 4) << 1;
    *((int *)(c + 0xa8)) = -((int)(((((long long)data_02082214[i]) * 0x8c000) + 0x800) >> 12));
    v = *((int *)(c + 0xa8));
    if (v < 0x5000) {
        v = 0x5000;
    } else if (v > 0x2d000) {
        v = 0x2d000;
    }
    *((int *)(c + 0xa8)) = v;
    *((int *)(c + 0x98)) = (int)(((((long long)data_02082214[i + 1]) * 0x8c000) + 0x800) >> 12);
    *((short *)(c + 0x5fc)) = 0x3c;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov014_02111af0, 0x02111af0, size 0x80 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int func_ov014_02111f54(void*);
extern int Math_Function_0203b14c(void*,int,int,int,int);
extern unsigned short DecIfAbove0_Short(unsigned short*);
extern void func_ov014_02111ebc(void*,int);
extern int _ZN9Animation7AdvanceEv(void*);
int func_ov014_02111af0(char* c){
  int r = func_ov014_02111f54(c);
  if(r) return r;
  if(Math_Function_0203b14c((char*)c+0x5f8, 0x64000, 0x800, 0x10000, 0x800)) goto adv;
  if(DecIfAbove0_Short((unsigned short*)(c+0x5fc))) goto adv;
  func_ov014_02111ebc(c, 1);
adv:
  return _ZN9Animation7AdvanceEv((char*)c+0x1a0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov014_02111a6c, 0x02111a6c, size 0x84 */
/* -------------------------------------------------------------------------- */
struct BCA_File;
/* (ModelAnim: real header type in scope; call stays on the mangled spelling with
   int in place of the by-value Fix12<int> -- notes/mwccarm-codegen.md 6az) */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void*, int, int, unsigned int);
extern "C" void func_ov014_02111a6c(char* c){
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim*)(c+0x150), *(BCA_File**)((char*)&data_ov014_02114980 + 4), 0, 0x1000, 0);
  *(int*)(c+0xa8)=0;
  *(int*)(c+0x98)=0;
  *(int*)(c+0x9c)=-0x2000;
  *(char*)(c+0x604)=0;
  *(short*)(c+0x600)=0;
  *(short*)(c+0x500+0xfc)=0x3c;
  *(int*)(((int)c + 0xb0)) &= ~3;
  *(int*)(c+0x60)=*(int*)(c+0x5f0)+0xc8000;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov014_021115ec, 0x021115ec, size 0x480 */
/* -------------------------------------------------------------------------- */
#include "types.h"
// @symbol func_ov014_021115ec
/* recovered: shared common types */
#include "common.h"
static inline void inc604(u8 *self) {
    u8 *p = (u8 *)(self + 0x604);
    *p = (u8)(*p + 1);
}
extern "C" void func_ov014_021115ec(u8 *self)
{
    /* This member's declaration views (parameter widths and pointer spellings)
     * are byte-load-bearing and diverge from the TU's canonical ones; block
     * scope keeps them its own (C linkage inherited, file-scope views hidden). */
    void _ZN5Sound15PlaySecretSoundEP8dActor_cPt(void *actor, u16 *snd);
    void *_ZN8dActor_c10FindWithIDEj(unsigned id);
    void ApproachAngle(void *self_, s32 a, s32 b, s32 c, s32 d);
    s16 _ZN8dActor_c18HorzAngleToCPlayerEv(void *self_);
    s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
    s32 _Z14ApproachLinearRsss(void *dst, s32 target, s32 step);
    u16 DecIfAbove0_Short(void *p);
    s32 _Z14ApproachLinearRiii(void *dst, s32 target, s32 step);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self_, void *bca, s32 a, s32 fix, unsigned b);
    s32 Vec3_ApproachHorz(Vector3 *out, Vector3 *target, s32 maxStep);
    void func_ov014_02112ea8(void *actor);
    void _ZN6Camera9SetFlag_3Ev(void *cam);
    void *_ZN8dActor_c13ClosestPlayerEv(void *self_);
    s32 _ZN6Player12Unk_020ca150Eh(void *player, u8 a);
    void _ZN6Camera9SetLookAtERK7Vector3(void *cam, const Vector3 *v);
    void _ZN7fBase_c18MarkForDestructionEv(void *self_);
    int _ZN9Animation7AdvanceEv(void *self_);
    extern s16 data_02082214[];
    extern void *data_0209f318;
    Vector3 sp4;
    u8 *r4; s16 r7; s16 r6; u8 *r8;
    _ZN5Sound15PlaySecretSoundEP8dActor_cPt(self, (u16 *)(self + 0x5fe));
    r4 = (u8 *)_ZN8dActor_c10FindWithIDEj(*(unsigned *)(self + 0x60c));
    {
        s32 *src = (s32 *)(r4 + 0x5c);
        s32 fifth = 0x80;
        sp4.x = src[0];
        void *ap = self + 0x8c;
        sp4.y = src[1];
        s32 z = 0;
        sp4.z = src[2];
        ApproachAngle(ap, z, 4, 0x200, fifth);
    }
    r8 = *(u8 **)&data_0209f318;
    r7 = _ZN8dActor_c18HorzAngleToCPlayerEv(self);
    r6 = Vec3_HorzAngle((Vector3 *)(self + 0x5c), (Vector3 *)(self + 0x5ec));
    switch (*(u8 *)(self + 0x604)) {
    case 0:
        *(u8 **)(r8 + 0x118) = self;
        if (_Z14ApproachLinearRsss(self + 0x8e, r7, 0x320) != 0 && DecIfAbove0_Short(self + 0x5fc) == 0)
            inc604(self);
        *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);
        break;
    case 1: case 2: case 3: case 4:
        *(u8 **)(r8 + 0x118) = self;
        _Z14ApproachLinearRiii(self + 0x98, 0, 0x400);
        if (*(u8 *)(self + 0x61c) != 0) {
            *(s16 *)(self + 0x94) = (s16)(r6 + 0x2000);
            *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
            *(s32 *)(self + 0xa8) = 0x32000;
            *(s32 *)(self + 0x98) = 0x1e000;
            inc604(self);
        }
        break;
    case 5:
        *(u8 **)(r8 + 0x118) = self;
        _Z14ApproachLinearRiii(self + 0x98, 0, 0x400);
        if (*(u8 *)(self + 0x61c) != 0) {
            *(s16 *)(self + 0x94) = r6;
            *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
            *(s32 *)(self + 0xa8) = 0x32000;
            *(s32 *)(self + 0x98) = 0x1e000;
            inc604(self);
            if (*(u8 *)(r4 + 0x31e) != 0) *(u8 *)(self + 0x604) = 7;
        }
        break;
    case 6: {
        *(u8 **)(r8 + 0x118) = self;
        _Z14ApproachLinearRiii(self + 0x618, 0x1000, 0x400);
        if (*(u8 *)(self + 0x61c) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x150, ((void**)&data_ov014_02114970)[1], 0, 0x1000, 0);
            *(s16 *)(self + 0x8e) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), &sp4);
            *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);
            *(s32 *)(self + 0xa8) = 0x14000;
            *(s32 *)(self + 0x618) = 0x64000;
        }
        {
            Vector3 target;
            s32 x = sp4.x;
            target.x = x;
            s32 z = sp4.z;
            target.z = z;
            s32 y = sp4.y;
            target.y = y;
            {
                s16 *tbl = data_02082214;
                u16 ang = *(u16 *)(r4 + 0x8e);
                s32 s = tbl[(ang >> 4) << 1];
                s32 add = (s32)(((((long long)s) * 0x96000) + 0x800) >> 12);
                target.x = x + add;
            }
            {
                s16 *tbl = data_02082214;
                u16 ang = *(u16 *)(r4 + 0x8e);
                s32 s = tbl[(((ang >> 4) << 1) + 1)];
                s32 add = (s32)(((((long long)s) * 0x96000) + 0x800) >> 12);
                target.z = z + add;
            }
            if (Vec3_ApproachHorz((Vector3 *)(self + 0x5c), &target, *(s32 *)(self + 0x618)) != 0) {
                inc604(self);
                *(s16 *)(self + 0x94) = (s16)(Vec3_HorzAngle((Vector3 *)(self + 0x5c), &sp4) + 0x8000);
                *(s32 *)(self + 0x98) = 0x28000;
                *(s32 *)(self + 0xa8) = 0xa000;
                func_ov014_02112ea8(r4);
            }
        }
        break;
    }
    case 7:
        if (*(u8 *)(self + 0x61c) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x150, ((void**)&data_ov014_02114980)[1], 0, 0x1000, 0);
            *(s16 *)(self + 0x94) = Vec3_HorzAngle((Vector3 *)(self + 0x5c), &sp4);
            *(u16 *)(self + 0x5fc) = 0x3c;
            *(s16 *)(self + 0x8e) = *(s16 *)(self + 0x94);
            if (*(u8 *)(r4 + 0x31e) != 0) {
                *(s32 *)(self + 0x98) = 0x28000; *(s32 *)(self + 0xa8) = 0x5a000;
            } else {
                *(s32 *)(self + 0x98) = 0x1e000; *(s32 *)(self + 0xa8) = 0x50000;
            }
            _ZN6Camera9SetFlag_3Ev(r8);
            inc604(self);
        }
        break;
    case 8:
        if (DecIfAbove0_Short(self + 0x5fc) == 0) {
            {
                unsigned *flag = (unsigned *)(r8 + 0x154);
                *flag &= ~8u;
            }
            if (_ZN6Player12Unk_020ca150Eh(_ZN8dActor_c13ClosestPlayerEv(self), 4) != 0) {
                inc604(self);
                *(u16 *)(self + 0x5fc) = 0x3c;
            }
        } else {
            _ZN6Camera9SetLookAtERK7Vector3(r8, (Vector3 *)(self + 0x5c));
        }
        break;
    case 9:
        if (DecIfAbove0_Short(self + 0x5fc) == 0)
            _ZN7fBase_c18MarkForDestructionEv(self);
        break;
    }
    _ZN9Animation7AdvanceEv(self + 0x1a0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov014_021115c0, 0x021115c0, size 0x2c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern int func_0201267c(int,void*);

void func_ov014_021115c0(char *r4) {
    func_0201267c(0x3a, r4 + 0x74);
    *(int *)(r4 + 0xa8) = 0x12c000;
    *(int *)(r4 + 0x9c) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov014_0211150c, 0x0211150c, size 0xb4 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov014_0211150c(char *c) {
    /* views moved to block scope: this file's return/parameter spellings differ
     * from the TU's canonical declarations (C linkage inherited). */
    extern int _Z14ApproachLinearRiii(int *p, int a, int b);
    extern int ApproachAngle(void *p, int a, int b, int c, int d);
    extern int Math_Function_0203b14c(void *p, int a, int b, int c, int d);
    extern int DecIfAbove0_Short(void *p);
    extern int func_ov014_02111ebc(void *c, int i);
    _Z14ApproachLinearRiii((int*)(c + 0x80), 0x1000, 0x500);
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(int*)(c + 0x84) = *(int*)(c + 0x88);
    ApproachAngle(c + 0x8c, -0x4000, 4, 0x1000, 0x400);
    if (Math_Function_0203b14c(c + 0x5f8, 0x64000, 0x800, 0x10000, 0x800) != 0) return;
    if (DecIfAbove0_Short(c + 0x5fc) != 0) return;
    func_ov014_02111ebc(c, 1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov014_021114d8, 0x021114d8, size 0x34 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov014_021114d8(char *c) {
    short v = *(short *)(c + 0x94);
    *(short *)(c + 0x8e) = v;
    *(short *)(c + 0x5fc) = 0x3c;
    *(int *)(c + 0x98) = 0x64000;
    *(int *)(c + 0xa8) = 0xc8000;
    *(int *)(c + 0x9c) = -0xa000;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov014_02111484, 0x02111484, size 0x54 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
extern unsigned short DecIfAbove0_Short(unsigned short*);
extern void func_ov014_02111ebc(void*, int);
extern void _Z14ApproachLinearRiii(int*, int, int);
void func_ov014_02111484(char* c){
  if(DecIfAbove0_Short((unsigned short*)(c+0x5fc))==0)
    func_ov014_02111ebc(c, 1);
  if(*(unsigned char*)(c+0x61c)!=0)
    *(int*)(c+0xa8)=*(int*)(c+0x98);
  _Z14ApproachLinearRiii((int*)(c+0x98), 0, 0x2000);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN10ChainChompD0Ev, 0x021113bc, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ChainChompD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * Destroy through ChainChomp and dEnemyBase_c, then hand the object back through
 * dActor_c's inline operator delete. The hand-written version declared its own
 * `data_020a0eac` for the actor heap, which collides with the `void *` dActor_c.h
 * supplies for that same symbol once the real header is in scope.
 */
#include "ChainChomp.h"

/* (no separate definition: the single ~ChainChomp() below emits the D0 and
 * D1 variants together.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN10ChainChompD1Ev, 0x02111308, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10ChainChompD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Four array cleanups and three member destructors, reverse declaration order
 * out of ChainChomp.h. Seven links means seven of nearly everything: Model[7]
 * at 0x1dc, ShadowModel[7] at 0x40c, and two Vector3[7] at 0x524 and 0x578 for
 * the per-link positions. Every element type was already named in the tree.
 */
#include "ChainChomp.h"

ChainChomp::~ChainChomp()
{
}

