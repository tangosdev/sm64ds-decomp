//cpp
/* daObjEwbIce_c, ov073. Sixteen functions, .text 0x02121f90..0x02122730.
 *
 * NAME: _ZTS13daObjEwbIce_c is the cartridge string "13daObjEwbIce_c" at
 * 0x02123158. _ZTI13daObjEwbIce_c at 0x0212314c reads [__si_class_type_info,
 * 0x02123158, _ZTI10dBgActor_c], so the single base is dBgActor_c, and the
 * word before _ZTV13daObjEwbIce_c (0x021231e8), at 0x021231e4, is that _ZTI.
 * The class was coined CccArena before the ROM name was read (factory
 * aliases CccArena_Spawn, CccBigIce_Spawn and CccSmallIce_Spawn for the
 * EWB_ICE_A/B/C profiles, all one class).
 *
 * `#pragma defer_codegen off` is load-bearing. The out-of-line destructor is
 * the key function, so this TU emits _ZTV/_ZTI/_ZTS. It comes out D1
 * (0x02121f90), D0 (0x02121fd4), then a D2 the cartridge does not keep
 * (manifest: deadstrip), and every other function follows source order.
 * This file is ROM-ascending. One destructor.
 *
 * func_ov073_02122730 and func_ov073_021227d0 sit between InitResources and
 * the three factories and stay one-function sources.
 *
 * The class header comes first so mModel.mat4x3.t sees the structured
 * Matrix4x3. Including dBgActor_c.h ahead of it would lock in common.h's
 * flat spelling.
 */

#pragma defer_codegen off

#include "daObjEwbIce_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x320]; PMF *pp; };

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};
struct Derived { char pad[0xd4]; Base base; };

extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
int func_02012694(int, const Vector3 &);
unsigned short DecIfAbove0_Short(unsigned short *p);
void _Z14ApproachLinearRiii(int *p, int a, int b);
extern short data_02082214[];
extern int data_02092138;
extern int data_ov073_021234a0;
int _Z14ApproachLinearRsss(short &v, short a, short b);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 &pos);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *, int, void *, int, short, int);
void func_020393d4(void *, void *);
void func_020393c4(void *, void *);
int func_ov073_021223a4(C *c, PMF *p);
extern unsigned char data_ov073_02123420[];
extern unsigned char data_ov073_02123424[];
extern char data_ov073_021231bc[];
extern char data_ov073_021231c0[];
extern char data_ov073_021231c4[];
extern void *data_ov073_021234b0;
int func_ov073_021227d0(void *, void *, void *);
void Matrix4x3_FromRotationXYZExt(void *, int, int, int);
}


// @symbol _ZN13daObjEwbIce_cD1Ev
// @symbol _ZN13daObjEwbIce_cD0Ev
/* D1: own vptr, then dBgActor_c's -- inlined, because dBgActor_c's
 * destructor is defined in its class body -- then dBgActor_c's own dBgW_Kc
 * and Model, then dActor_c. This class adds no member with a destructor of
 * its own (see include/daObjEwbIce_c.h). D0 adds the inline operator delete
 * inherited from dActor_c. */
daObjEwbIce_c::~daObjEwbIce_c()
{
}

// @symbol func_ov073_0212202c
extern "C" int func_ov073_0212202c(void)
{
    return 1;
}

// @symbol func_ov073_02122034
extern "C" int func_ov073_02122034(char *c)
{
    Vector3 pos;
    int t;
    *(short *)(c + 0x332) = 0;
    ((int *)&pos)[0] = *(int *)(c + 0x5c);
    ((int *)&pos)[1] = *(int *)(c + 0x60);
    ((int *)&pos)[2] = *(int *)(c + 0x64);
    ((int *)&pos)[0] = *(int *)(c + 0x5c);
    t = *(int *)(c + 0x60);
    ((int *)&pos)[1] = t;
    ((int *)&pos)[2] = *(int *)(c + 0x64);
    ((int *)&pos)[1] = t - 0x12c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8b, ((int *)&pos)[0], ((int *)&pos)[1], ((int *)&pos)[2]);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8c, ((int *)&pos)[0], ((int *)&pos)[1], ((int *)&pos)[2]);
    func_02012694(0x172, *(Vector3 *)(c + 0x74));
    return 1;
}

// @symbol func_ov073_021220c0
extern "C" int func_ov073_021220c0(char *c)
{
    *(int *)(c + 0x328) = 0x400;
    if (DecIfAbove0_Short((unsigned short *)(c + 0x330)) != 0) {
        unsigned short *q = (unsigned short *)(((int)c + 0x332));
        int t;
        *q += 0x800;
        t = *(int *)(c + 0x328);
        {
            int v = *(short *)((char *)(c + 0x300) + 0x32);
            *(short *)(c + 0x8c) = t + (int)(((long long)t * data_02082214[(((int)((unsigned int)(v << 16) >> 16)) >> 4) * 2] + 0x800) >> 12);
        }
        _Z14ApproachLinearRiii((int *)(c + 0x328), 0, 0x40000);
        return 1;
    }
    *(short *)(c + 0x8c) = 0;
    *(int *)(c + 0x9c) = -0xa000;
    ((dActor_c *)c)->UpdatePos(0);
    if (*(unsigned char *)(c + 0x32c) != 0) goto end;
    {
        int v = data_02092138 + 0x96000;
        if (v <= *(int *)(c + 0x60)) goto end;
        *(int *)(c + 0x60) = v;
        *(short *)(c + 0x330) = 0;
        *(int *)(c + 0x9c) = 0;
        *(int *)(c + 0xa8) = 0;
        func_ov073_021223a4((C *)c, (PMF *)&data_ov073_021234a0);
    }
end:
    return 1;
}

// @symbol func_ov073_021221e0
extern "C" int func_ov073_021221e0(char *c)
{
    *(short *)(c + 0x330) = 0x64;
    *(int *)(c + 0xa0) = -0xc8000;
    return 1;
}

// @symbol func_ov073_02122200
extern "C" int func_ov073_02122200(char *thiz)
{
    char *c = thiz;
    ((dActor_c *)c)->UpdatePos(0);
    if (*(unsigned short *)(c + 0x330) < 0x18d) {
        *(int *)(c + 0x9c) = 0;
        *(int *)(c + 0xa8) = 0;
        if (*(unsigned short *)(c + 0x330) == 0x183)
            func_02012694(0x171, *(Vector3 *)(c + 0x74));
        if (*(unsigned short *)(c + 0x330) < 0x183) {
            *(int *)(c + 0x9c) = -0xa000;
            _Z14ApproachLinearRsss(*(short *)(c + 0x8c), 0x2000, 0x80);
        }
    }
    if (DecIfAbove0_Short((unsigned short *)(c + 0x330)) == 0 ||
        data_02092138 - 0xc8000 > *(int *)(c + 0x60)) {
        ((fBase_c *)c)->MarkForDestruction();
    }
    return 1;
}

// @symbol func_ov073_021222c8
extern "C" int func_ov073_021222c8(void *c)
{
    *(unsigned short *)((char *)c + 0x330) = 0x190;
    int neg = -0x14000;
    *(int *)((char *)c + 0x9c) = neg;
    *(int *)((char *)c + 0xa0) = neg;
    return 1;
}

// @symbol func_ov073_021222ec
extern "C" int func_ov073_021222ec(char *c)
{
    if (DecIfAbove0_Short((unsigned short *)(c + 0x330)) == 1) {
        char *a = (char *)dActor_c::FindWithActorID(0xda, 0);
        if (a != 0) {
            switch (*(u16 *)(c + 0xc)) {
            case 0xaa:
                *(int *)(c + 0x334) = *(int *)(a + 4);
                break;
            case 0xab: {
                char *p = a + (*(unsigned char *)(c + 0x32d)) * 0xc;
                *(int *)(p + 0x3e8) = *(int *)(c + 0x5c);
                *(int *)(p + 0x3ec) = *(int *)(c + 0x60);
                *(int *)(p + 0x3f0) = *(int *)(c + 0x64);
                break;
            }
            case 0xac: {
                char *p = a + (*(unsigned char *)(c + 0x32d)) * 0xc;
                *(int *)(p + 0x448) = *(int *)(c + 0x5c);
                *(int *)(p + 0x44c) = *(int *)(c + 0x60);
                *(int *)(p + 0x450) = *(int *)(c + 0x64);
                break;
            }
            }
        }
    }
    return 1;
}

// @symbol func_ov073_0212239c
extern "C" int func_ov073_0212239c(void)
{
    return 1;
}

// @symbol func_ov073_021223a4
extern "C" int func_ov073_021223a4(C *c, PMF *p)
{
    c->pp = p;
    PMF *q = c->pp;
    if (*q == 0) return 1;
    return (c->**q)();
}

// @symbol _ZN13daObjEwbIce_c4KillEv
/* Slot 31, attributed by the vtable: _ZTV13daObjEwbIce_c + 4*31 =
 * 0x021231e8 + 0x7c = 0x02123264; ov073 relocs.txt: 0x02123264 -> 0x021223f4.
 * Not the key function. */
void daObjEwbIce_c::Kill()
{
    char *c = (char *)this;
    Vector3 vec;
    Vector3 vec2;
    vec.x = *(int *)(c + 0x5c);
    vec.y = *(int *)(c + 0x60);
    vec.z = *(int *)(c + 0x64);
    vec.y += 0x32000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, vec.x, vec.y, vec.z);
    ((int *)&vec2)[0] = ((int *)&vec)[0];
    ((int *)&vec2)[1] = ((int *)&vec)[1];
    ((int *)&vec2)[2] = ((int *)&vec)[2];
    ((dActor_c *)c)->PoofDustAt(vec2);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3 *)(c + 0x74));
}

// @symbol _ZN13daObjEwbIce_c16CleanupResourcesEv
int daObjEwbIce_c::CleanupResources()
{
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled())
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    ((SharedFilePtr *)(*(void **)(data_ov073_021231bc + mVariant * 0xc)))->Release();
    ((SharedFilePtr *)(*(void **)(data_ov073_021231c0 + mVariant * 0xc)))->Release();
    return 1;
}

// @symbol _ZN13daObjEwbIce_c6RenderEv
int daObjEwbIce_c::Render()
{
    Base *b = &((Derived *)this)->base;
    b->m(0);
    return 1;
}

// @symbol _ZN13daObjEwbIce_c8BehaviorEv
int daObjEwbIce_c::Behavior()
{
    void *o = *(void **)&mState;
    if (*(int *)((char *)o + 8)) {
        char *base = (char *)o + 8;
        int adj = *(int *)(base + 4);
        char *self = ((char *)this) + (adj >> 1);
        void *fn;
        if (adj & 1) {
            void *vt = *(void **)self;
            fn = *(void **)((char *)vt + *(int *)base);
        } else
            fn = *(void **)base;
        ((void (*)(char *))fn)(self);
    }
    Matrix4x3_FromRotationXYZExt(((char *)this) + 0xf0, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN13daObjEwbIce_c13InitResourcesEv
int daObjEwbIce_c::InitResources()
{
    unsigned char idx;
    int f;

    switch (actorID) {
        case 0xaa:
            unk_330 = 3;
            mVariant = 0;
            break;
        case 0xab:
            unk_330 = 3;
            mSpawnIndex = data_ov073_02123424[0];
            data_ov073_02123424[0]++;
            mVariant = 1;
            break;
        case 0xac:
            unk_330 = 3;
            mSpawnIndex = data_ov073_02123420[0];
            data_ov073_02123420[0]++;
            mVariant = 2;
            break;
    }

    idx = mVariant;
    f = (int)Model::LoadFile(**(SharedFilePtr **)(data_ov073_021231bc + idx * 0xc));
    mModel.SetFile((BMD_File *)f, 1, -1);
    Matrix4x3_FromRotationXYZExt(((char *)this) + 0xf0, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
    UpdateClsnPosAndRot();

    {
        unsigned char i = mVariant;
        f = (int)dBgW_Kc::LoadFile(**(SharedFilePtr **)(data_ov073_021231c0 + i * 0xc));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            ((char *)this) + 0x124, f, ((char *)this) + 0x2ec, 0x1000, mAngleY, *(int *)(data_ov073_021231c4 + i * 0xc));
    }

    func_020393d4(((char *)this) + 0x124, (void *)&dBgW::UpdatePosWithTransform);
    func_020393c4(((char *)this) + 0x124, func_ov073_021227d0);
    ((dBgW *)(((char *)this) + 0x124))->Enable((dActor_c *)(((char *)this)));

    unk_338 = 0;
    unk_334 = 0;
    func_ov073_021223a4((C *)((char *)this), (PMF *)&data_ov073_021234b0);

    return 1;
}
