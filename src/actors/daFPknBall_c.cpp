//cpp
/* daFPknBall_c (FPAKUN_BALL), ov002 0x020f8858..0x020f9304.
 *
 * RTTI names daFPknBall_c. daFPknBall_c_classInit at 0x020f9304 follows this
 * run and is not in it. One out-of-line destructor under
 * #pragma defer_codegen off emits D1, D0, then a homeless D2, and .text
 * follows source order. common.h is first so the flat Matrix4x3 is what
 * the particle helper copies.
 */

#pragma defer_codegen off

#include "common.h"
#include "daFPknBall_c.h"

typedef struct { s32 x, y, z; } Vec3;

struct Vector3_16f;

struct Vec3F { int x, y, z; };
struct Vec3P { int x, y, z; Vec3P() {} };
enum Bool { FALSE, TRUE };

struct Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m10(); virtual int m11();
    virtual int m12(); virtual int m13(); virtual int m14(); virtual int m15();
    virtual int m16(); virtual int m17(); virtual int m18(); virtual int m19();
    virtual int m20(); virtual int m21(); virtual int m22(); virtual int m23();
    virtual int m24(); virtual int m25(); virtual int m26(); virtual int m27();
    virtual int m28(); virtual int m29();
};

extern "C" {
extern int RandomIntInternal(int* seed);
extern char* _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void* v, void* w, int e, int f);
extern int data_0209e650;
extern void* _ZN8dActor_c18ClosestWithActorIDEj(void* self, u32 id);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(u32 id, void* prev);
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 c);
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const struct Vector3_16f* f);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const void* f, void* g);
extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3* m, s32 x, s32 y, s32 z);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, Fix12i f, Fix12i a, u32 b);
extern s32 data_ov002_02100320[];
extern s32 data_ov002_02100334[];
extern s32 data_ov002_02100348[];
extern Matrix4x3 data_020a0e68;
void func_ov002_020f8b24(void* self);
int func_ov002_020ad660(void* cc, void* pp, void* r5p, int flags);
void _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(void* self, const struct Vec3F* v);
s16 Vec3_HorzAngle(const void* a, const void* b);
void* _ZN8dActor_c10FindWithIDEj(u32 id);
void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const void* pos);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, s32 x, s32 y, s32 z);
void _ZN5dCc_c5ClearEv(void* self);
void _ZN5dCc_c6UpdateEv(void* self);
void _ZN6Player4BurnEv(void* self);
void func_02012694(u32 id, const void* v);
void _ZN7fBase_c18MarkForDestructionEv(void* self);
void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
int _Z14ApproachLinearRiii(void* p, s32 target, s32 step);
int _Z14ApproachLinearRsss(void* p, s16 target, s16 step);
int _ZNK10dBgCh_Actr8IsOnWallEv(void* self);
int _ZNK10dBgCh_Actr12TouchesWaterEv(void* self);
void func_ov002_020f897c(void* self);
void func_ov002_020f88ec(char* self);
extern s16 data_02082214[];
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(void* thiz);
}

// @symbol _ZN12daFPknBall_cD1Ev
// @symbol _ZN12daFPknBall_cD0Ev
/* D1: own vptr, then the members in reverse declaration order (ShadowModel,
 * dBgCh_Actr, dCcAc_c), then dEnemyBase_c's destructor. D0 is the same body
 * plus the inherited operator delete; it has no source of its own. */
daFPknBall_c::~daFPknBall_c()
{
}

// @symbol func_ov002_020f88ec
extern "C" {
void func_ov002_020f88ec(char* c)
{
    char* a;
    if (((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 10 >= 4) return;
    a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x120, 0, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
    if (a != 0) {
        *(int*)(a + 0xa4) = 0;
        *(int*)(a + 0xa8) = 0;
        *(int*)(a + 0xac) = 0;
    }
}
}

// @symbol func_ov002_020f897c
extern "C" {
void func_ov002_020f897c(void* self)
{
    char* sl = (char*)self;
    u8 n = (u8)(((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 10);
    if (n >= 4) {
        if (_ZN8dActor_c18ClosestWithActorIDEj(sl, 0xfe) != 0) return;
    }
    {
        void* p = _ZN8dActor_c13ClosestPlayerEv(sl);
        char* sb;
        if (p != 0 && *(int*)((char*)p + 8) == 3 &&
            (sb = (char*)_ZN8dActor_c15FindWithActorIDEjPS_(0x117, 0)) != 0 &&
            *(u8*)(sb + 0x42b) == 0) {
            int c;
            int idx;
            int mask = 0;
            for (c = 0; c < 3; c++) {
                if (_ZN8SaveData19IsCharacterUnlockedEj(c) != 0) {
                    mask = (mask | (1 << c)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 3;
            } while ((mask & (1 << idx)) == 0);
            if (_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, (idx << 8) | 0xb,
                    (struct Vector3*)(sl + 0x5c), (struct Vector3_16*)(sl + 0x8c),
                    *(s8*)(sl + 0xcc), -1) != 0) {
                *(u8*)(sb + 0x42b) = 1;
                return;
            }
        }
    }
    if (n >= 4) return;
    {
        void* r = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x120, 0,
            (struct Vector3*)(sl + 0x5c), 0, *(s8*)(sl + 0xcc), -1);
        if (r != 0) {
            *(int*)((char*)r + 0xa4) = 0;
            *(int*)((char*)r + 0xa8) = 0;
            *(int*)((char*)r + 0xac) = 0;
        }
    }
}
}

// @symbol func_ov002_020f8b24
extern "C" {
void func_ov002_020f8b24(void* arg0)
{
    char* c = (char*)arg0;
    Vec3 v;
    Vec3 asr;

    {
        s32 x = *(s32*)(c + 0x5c);
        v.x = x;
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if (*(u8*)(c + 0x36e) != 0)
            v.x = x * (u32)-1;
    }
    v.y = v.y + data_ov002_02100320[*(u8*)(c + 0x36d)];

    *(u32*)(c + 0x370) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        *(u32*)(c + 0x370), data_ov002_02100334[*(u8*)(c + 0x36d)], v.x, v.y, v.z, 0);

    *(void**)(c + 0x374) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x374), data_ov002_02100348[*(u8*)(c + 0x36d)], v.x, v.y, v.z, 0, 0);

    {
        s32 x = *(s32*)(c + 0x5c);
        v.x = x;
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if (*(u8*)(c + 0x36e) != 0)
            v.x = x * (u32)-1;
    }
    Vec3_Asr(&asr, &v, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    *(Matrix4x3*)(c + 0x328) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x300, c + 0x328, 0x28000, 0x64000, 0xf);
}
}

// @symbol _ZN12daFPknBall_c6RenderEv
int daFPknBall_c::Render()
{
    return 1;
}

// @symbol _ZN12daFPknBall_c8BehaviorEv
/* _ZN12daFPknBall_c8BehaviorEv at 0x020f8c94 -- a real daFPknBall_c:: method now.
 *
 * The file-local POD `struct Vector3` is renamed `Vec3F` rather than dropped.
 * types.h's Vector3 is NOT a POD, and the two cannot both be named Vector3 in one
 * TU; keeping the POD spelling is deliberate (a C++ struct copy scalarises to the
 * members' own types, which is what a non-POD substitution would change here).
 * Renaming a file-local type cannot move a byte. The choice is per-function and
 * has to be measured: src/actors/dEnemyBase_c.cpp carried the same POD shadow and
 * dropping it for types.h's real Vector3 cost nothing there, so the shadow is gone
 * in that TU. Re-measure before assuming either answer applies here.
 */
int daFPknBall_c::Behavior() {
    char* c = (char*)this;
    struct Vec3F v1;
    volatile struct Vec3F pt;
    struct Vec3F v2, v3, w1, w2, w3;
    int f;
    int res;
    void* found;
    int lr;
    int id134;

    f = *(int*)(c + 0xb0);
    {
        enum Bool b = (enum Bool)((f & 0x20000) != 0);
        if (b != FALSE) {
            func_ov002_020f8b24(c);
            return 1;
        }
    }
    {
        enum Bool b = (enum Bool)((f & 0x40000) != 0);
        if (b != FALSE) {
            return 1;
        }
    }

    res = func_ov002_020ad660(c, c + 0x144, 0, 2);
    if (res != 0) {
        if (res == 2) {
            int x = *(int*)(c + 0x5c);
            v1.x = x;
            v1.y = *(int*)(c + 0x60);
            v1.z = *(int*)(c + 0x64);
            if (*(u8*)(c + 0x36e) != 0)
                v1.x = x * (u32)-1;
            v1.y += 0x50000;
            ((int*)&w1)[0] = ((int*)&v1)[0];
            ((int*)&w1)[1] = ((int*)&v1)[1];
            ((int*)&w1)[2] = ((int*)&v1)[2];
            _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(c, &w1);
        } else {
            func_ov002_020f8b24(c);
        }
        return 1;
    }

    *(void**)(c + 0x358) = _ZN8dActor_c13ClosestPlayerEv(c);
    if (*(void**)(c + 0x358) != 0 && *(u8*)(c + 0x36d) != 3) {
        *(s16*)(c + 0x368) = Vec3_HorzAngle(c + 0x5c, (char*)(*(void**)(c + 0x358)) + 0x5c);
    } else {
        *(s16*)(c + 0x368) = *(s16*)(c + 0x8e);
    }

    id134 = *(int*)(c + 0x134);
    if (id134 != 0) {
        if ((*(int*)(c + 0x130) & 0x8000) == 0) {
            found = _ZN8dActor_c10FindWithIDEj((u32)id134);
            if (found != 0) {
                if (*(int*)(c + 0x130) & 0x10) {
                    *(u32*)(((int)c + 0xb0)) &= ~0x10000001;
                    *(s16*)(c + 0x94) = Vec3_HorzAngle((char*)found + 0x5c, c + 0x5c);
                    *(int*)(c + 0x98) = 0xa000;
                    *(int*)(c + 0xa8) = 0x28000;
                    *(s16*)(c + 0x102) = 0x1e;
                    *(s16*)(c + 0xec) = 0;
                    *(s16*)(c + 0xee) = 0;
                    *(s16*)(c + 0xf0) = 0;
                    *(int*)(c + 0x10c) = 8;
                    *(int*)(c + 0x9c) = -0x2000;
                    *(int*)(c + 0xa0) = -0x32000;
                    _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
                    pt.x = *(int*)(c + 0x5c);
                    pt.y = *(int*)(c + 0x60);
                    pt.z = *(int*)(c + 0x64);
                    {
                        int ret = ((Obj*)c)->m29();
                        int px, py, pz;
                        py = pt.y;
                        px = pt.x;
                        py = py + ret;
                        pz = pt.z;
                        pt.y = py;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, px, py, pz);
                    }
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, pt.x, pt.y, pt.z);
                    func_ov002_020f8b24(c);
                    _ZN5dCc_c5ClearEv(c + 0x110);
                    return 1;
                }
                {
                    enum Bool isbf = (enum Bool)(*(u16*)((char*)found + 0xc) == 0xbf);
                    if (isbf != FALSE && *(u8*)((char*)found + 0x6f9) == 0 && *(u8*)((char*)found + 0x6fb) == 0) {
                        _ZN6Player4BurnEv(found);
                        {
                            int x = *(int*)(c + 0x5c);
                            v2.x = x;
                            v2.y = *(int*)(c + 0x60);
                            v2.z = *(int*)(c + 0x64);
                            if (*(u8*)(c + 0x36e) != 0)
                                v2.x = x * (u32)-1;
                            v2.y += 0x50000;
                            ((int*)&w2)[0] = ((int*)&v2)[0];
                            ((int*)&w2)[1] = ((int*)&v2)[1];
                            ((int*)&w2)[2] = ((int*)&v2)[2];
                            _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(c, &w2);
                        }
                        if (*(u8*)(c + 0x36d) == 4)
                            func_02012694(0x157, c + 0x74);
                        _ZN7fBase_c18MarkForDestructionEv(c);
                    }
                }
            }
        } else {
            *(int*)(((int)c + 0x128)) |= 1;
        }
    }

    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x144);
    if (_Z14ApproachLinearRiii(c + 0x98, *(int*)(c + 0x35c), 0x999) != 0) {
        _Z14ApproachLinearRsss(c + 0x94, *(s16*)(c + 0x368), 0x200);
    }

    lr = (*(int*)(c + 0x98) * data_02082214[(*(u16*)(c + 0x92) >> 4) * 2 + 1]) / 4096;
    *(int*)(c + 0xa4) = (lr * data_02082214[(*(u16*)(c + 0x94) >> 4) * 2]) / 4096;
    *(int*)(c + 0xa8) = (-*(int*)(c + 0x98) * data_02082214[(*(u16*)(c + 0x92) >> 4) * 2]) / 4096;
    *(int*)(c + 0xac) = (lr * data_02082214[(*(u16*)(c + 0x94) >> 4) * 2 + 1]) / 4096;
    *(int*)(((int)c + 0x5c)) += *(int*)(c + 0xa4);
    *(int*)(((int)c + 0x60)) += *(int*)(c + 0xa8);
    *(int*)(((int)c + 0x64)) += *(int*)(c + 0xac);

    _ZN5dCc_c5ClearEv(c + 0x110);
    _ZN5dCc_c6UpdateEv(c + 0x110);
    *(int*)(((int)c + 0x360)) += *(int*)(c + 0x98);

    if (*(int*)(c + 0x360) > *(int*)(c + 0x364)
        || _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144) != 0
        || _ZNK10dBgCh_Actr12TouchesWaterEv(c + 0x144) != 0) {
        u8 st = *(u8*)(c + 0x36d);
        if (st == 0) {
            func_ov002_020f897c(c);
        } else if (st == 4 && _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144) == 0) {
            func_ov002_020f88ec(c);
        }
        {
            int x = *(int*)(c + 0x5c);
            v3.x = x;
            v3.y = *(int*)(c + 0x60);
            v3.z = *(int*)(c + 0x64);
            if (*(u8*)(c + 0x36e) != 0)
                v3.x = x * (u32)-1;
            v3.y += 0x50000;
            ((int*)&w3)[0] = ((int*)&v3)[0];
            ((int*)&w3)[1] = ((int*)&v3)[1];
            ((int*)&w3)[2] = ((int*)&v3)[2];
            _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(c, &w3);
        }
        if (*(u8*)(c + 0x36d) == 4)
            func_02012694(0x157, c + 0x74);
        _ZN7fBase_c18MarkForDestructionEv(c);
    }

    *(u16*)(((int)c + 0x100)) += 1;
    func_ov002_020f8b24(c);
    return 1;
}

// @symbol _ZN12daFPknBall_c13InitResourcesEv
int daFPknBall_c::InitResources()
{
    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x50000, 0x200002, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv((char*)&mWithMeshClsn);
    mStateTimer = 0;
    unk_36a = 0;
    unk_360 = 0;
    unk_364 = 0x5dc000;
    unk_36d = param1 & 7;
    unk_370 = 0;
    unk_374 = 0;
    {
        unsigned char v = unk_36d;
        if (v != 0 && v != 4) {
            *(unsigned int*)(((int)((char*)&mdCcAc_c.vulnFlags))) |= 0x8000;
        }
    }
    return 1;
}

// @symbol _ZN12daFPknBall_c13OnYoshiTryEatEv
s32 daFPknBall_c::OnYoshiTryEat() {
    unsigned char b = unk_36d;
    if (b != 0 && b != 4)
        return 5;
    return 0;
}
