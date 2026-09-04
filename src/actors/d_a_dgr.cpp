//cpp
/* ov025 / daDgr_c -- the swinging log platform of the pyramid set
 * (registry profile DONGURU, RTTI name at ov025:0x02113724).
 *
 * One translation unit for the whole ROM run 0x021111a0..0x021118c8: the four
 * class methods, the three file-local helpers they share, the destructor pair
 * and the factory, plus the .data run 0x02113724..0x021137e0 that holds the
 * class's RTTI chain, its profile descriptor and its vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daDgr_c.h, which is what makes mwccarm emit the cartridge's
 * D1/D0 pair, in the cartridge's order, and no D2 at all.
 */
#include "decl_common.h"

#include "daDgr_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

typedef int Fix12i;
struct BMD_File;
struct KCL_File;
struct CLPS_Block;
struct ModelBase;

/* Reconciled from the ten per-function copies. Every one of these is a ROM
 * symbol spelled by its final mangled name, so it needs C linkage: seen as C++
 * the compiler would mangle the already-mangled spelling a second time and emit
 * a reference nothing defines. */
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int size);
extern void  _ZN10dBgActor_cC2Ev(void *self);

extern BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase *thiz, BMD_File *f, int a, int b);
extern KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr &fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *thiz, KCL_File *kcl, const Matrix4x3 &mtx, Fix12i fix, short s,
    CLPS_Block &clps);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(daDgr_c *self, s32 a, s32 b);
extern void *_ZN8dActor_c18ClosestWithActorIDEj(void *self, unsigned int id);
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(daDgr_c *self, void *pos, s32 fix);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 slot, u32 effect, s32 x, s32 y, s32 z, const void *rot, void *cb);

/* dBgW's update-hook setter. It is in no decl_*.h; InitResources declared it
 * locally. */
extern void func_020393d4(int *thiz, int fn);
extern void Matrix4x3_FromRotationX(void *m, int angle);
extern int  Vec3_Dist(void *a, void *b);
extern void func_02012694(s32 a, void *b);

extern s16 data_02082214[];
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern SharedFilePtr data_ov025_02113a68;   /* the model's file handle */
extern SharedFilePtr data_ov025_02113a60;   /* the KCL's file handle */
extern CLPS_Block data_ov025_02112c28;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 9 -- daDgr_c_classInit, 0x02111898, size 0x30                  */
/* ------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daDgr_c through its ROM RTTI
 * string, allocation size, vtable identity, and the DONGURU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. */
// @symbol daDgr_c_classInit
extern "C" int *daDgr_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(820);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x02113758 and +8 is what reaches the
         * 0x02113760 address point the ROM stores. The addend-0 spelling is
         * right only while a class merely IMPORTS its vtable. */
        p[0] = (int)&_ZTV7daDgr_c[2];
    }
    return p;
}

/* ------------------------------------------------------------------------- */
/* g_profile_DONGURU, 0x0211373c, size 0x1c                                   */
/* ------------------------------------------------------------------------- */
/* .data objects emit in SOURCE order, so the descriptor is written directly
 * after the factory it names -- which is where the ROM's run puts it, between
 * _ZTI (0x02113730) and the vtable object (0x02113758). */
struct DonguruSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x00a3 == actor 163 */
    s16 drawOrder;                  /* 0x0049 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char DonguruSpawnInfo_size_must_be_0x1c[
    sizeof(DonguruSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_DONGURU
extern "C" DonguruSpawnInfo g_profile_DONGURU = {
    daDgr_c_classInit, 0x00a3, 0x0049, 0x12000002,
    0x00000000, 0x00320000, 0x02000000, 0x02000000
};

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- daDgr_c::InitResources, 0x021117dc, size 0xbc             */
/* ------------------------------------------------------------------------- */
/* Loads the platform's model and its KCL, points the mesh collider's update
 * hook at dBgW::UpdatePosAndAngs, zeroes the facing angle the KCL was baked
 * against, and parks the swing state -- resting height, angular speed, phase,
 * stage, direction and both particle handles. */
// @symbol _ZN7daDgr_c13InitResourcesEv
s32 daDgr_c::InitResources()
{
    func_ov025_02111344((char *)this);
    func_ov025_021112e0((char *)this);
    {
        BMD_File *bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov025_02113a68);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase *)&mModel, bmd, 1, -1);
    }
    {
        KCL_File *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov025_02113a60);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, mClsnMat, 0x1000, mAngleY, data_ov025_02112c28);
    }
    func_020393d4((int *)&mMeshCollider,
                  (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mAngleY = 0;
    mBasePosY = mPosY;
    mAngleXSpeed = 0;
    mPhaseTimer = 0;
    mSwingStage = 0;
    mSwingDir = 0;
    mDustParticle2 = 0;
    mDustParticle1 = mDustParticle2;
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daDgr_c::Behavior, 0x021113f0, size 0x3ec                 */
/* ------------------------------------------------------------------------- */
/* A swinging platform. mPhaseTimer is the phase counter, mSwingStage the stage
 * counter (-1 is the "hold at the end" sentinel), mSwingDir the direction flag.
 * The swing speed comes from `10 - mSwingStage' folded to a magnitude and
 * bucketed to 1/2/4, so the platform eases in and out; mBasePosY holds the
 * resting height that the sine offset from data_02082214 is added to. Two dust
 * particle systems trail either end, and the last frame of each swing shakes
 * the ground.
 *
 * LAUNDER() is a no-op macro that MARKS the read-modify-write sites. It emits
 * nothing and it is not the reason those sites take an address.
 *
 * mAngleY is read UNSIGNED at the table-index sites: an s16 read costs four
 * words and un-matches the function. */
#define LAUNDER(p) (p)

typedef struct { s32 x, y, z; } Vec3;

// @symbol _ZN7daDgr_c8BehaviorEv
s32 daDgr_c::Behavior()
{
    s32 loc[6];
    s32 n;
    s32 m;

    if (mSwingStage == -1) {
        if (mPhaseTimer == 0x20) {
            mSwingStage = 0;
            mPhaseTimer = 0;
        } else {
            u16 *cnt = (u16 *)LAUNDER(&mPhaseTimer);
            (*cnt)++;
            unk_0ac = 0;
            mAngleXSpeed = 0;
            func_ov025_02111344((char *)this);
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0)) {
                func_ov025_021112e0((char *)this);
            }
            return 1;
        }
    }

    n = 10 - mSwingStage;
    if (n < 0) {
        s32 neg = -1;
        n = n * neg;
    }
    n = n - 6;
    if (n < 0)
        n = 0;

    if (mPhaseTimer == n + 8) {
        s8 *p;
        mPhaseTimer = 0;
        p = (s8 *)LAUNDER(&mSwingStage);
        *p = (s8)(*p + 1);
        if (mSwingStage == 0x14) {
            u8 *pd = (u8 *)LAUNDER(&mSwingDir);
            *pd = (u8)(*pd ^ 1);
            mSwingStage = -1;
        }
    }

    if ((u32)(n - 3) <= 1u) {
        n = 4;
    } else if ((u32)(n - 1) <= 1u) {
        n = 2;
    } else if (n == 0) {
        n = 1;
    }

    m = n << 3;

    if (mPhaseTimer < m) {
        if (mSwingDir == 0) {
            unk_0ac = 0x14000 / n;
            mAngleXSpeed = (s16)(0x400 / n);
        } else {
            unk_0ac = (-0x14000) / n;
            mAngleXSpeed = (s16)((-0x400) / n);
        }

        {
            s32 *pz = (s32 *)LAUNDER(&mPosZ);
            s16 *pr = (s16 *)LAUNDER(&mAngleX);
            *pz = *pz + unk_0ac;
            *pr = (s16)(*pr + mAngleXSpeed);
            if ((mAngleX & 0x1fff) < 0x320) {
                if (mAngleXSpeed != 0)
                    func_02012694(0x65, (char *)&mCamSpacePosX);
            }
        }

        {
            s16 t = mAngleX;
            s32 idx = ((u16)(s16)(t << 2) >> 4) * 2;
            s32 prod = (s32)data_02082214[idx] * 23;
            if (prod < 0) {
                s32 neg = -1;
                prod = prod * neg;
            }
            mPosY = mBasePosY + prod;
        }

        if (mPhaseTimer == m - 1) {
            loc[3] = mPosX;
            loc[4] = mPosY;
            loc[5] = mPosZ;
            _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(this, (Vec3 *)&loc[3], 0x3e8000);
        }

        loc[0] = mPosX;
        loc[1] = mPosY;
        loc[2] = mPosZ;
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = c * scale + mPosX;
            loc[1] = mBasePosY - 0xb9000;
            c = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = c * scale + mPosZ;
            mDustParticle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mDustParticle1, 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
        {
            s32 scale = 100;
            s32 zero = 0;
            s32 c;
            c = data_02082214[((u16)mAngleY >> 4) * 2 + 1];
            loc[0] = mPosX - c * scale;
            c = data_02082214[((u16)mAngleY >> 4) * 2];
            loc[2] = mPosZ - c * scale;
            mDustParticle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mDustParticle2, 0x2d, loc[0], loc[1], loc[2], (void *)zero, (void *)zero);
        }
    }

    func_ov025_02111344((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) || func_ov025_0211123c((char *)this)) {
        func_ov025_021112e0((char *)this);
    }

    {
        u16 *cnt = (u16 *)LAUNDER(&mPhaseTimer);
        (*cnt)++;
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daDgr_c::Render, 0x021113c8, size 0x28                    */
/* ------------------------------------------------------------------------- */
/* The Model sub-object at +0xd4 is rendered through its own vtable; mwccarm
 * does not devirtualise an embedded member's virtual call. */
// @symbol _ZN7daDgr_c6RenderEv
s32 daDgr_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daDgr_c::CleanupResources, 0x02111384, size 0x44          */
/* ------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c16CleanupResourcesEv
s32 daDgr_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov025_02113a68))->Release();
    ((SharedFilePtr *)(&data_ov025_02113a60))->Release();
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov025_02111344, 0x02111344, size 0x40               */
/* ------------------------------------------------------------------------- */
/* Rebuilds the platform's collision matrix from its pitch and parks the
 * position, scaled down by 8, in the matrix's translation row. Both callers
 * are in this file. It keeps decl_common.h's `char *' signature. */
// @symbol func_ov025_02111344
extern "C" void func_ov025_02111344(char *t)
{
    Matrix4x3_FromRotationX(t + 0xf0, *(short *)(t + 0x8c));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov025_021112e0, 0x021112e0, size 0x64               */
/* ------------------------------------------------------------------------- */
/* Copies the matrix func_ov025_02111344 just built into mClsnMat, records the
 * position beside it, and pushes both into the dBgW_KcMbg member. */
/* The per-function file spelled the object with two local shadow types, and
 * that spelling is what the ROM's word order agrees with: the matrix is copied
 * THROUGH the member, not through a pointer cast to it. */
struct MMC { char p[0x124]; };
struct Obj { char p[0x2ec]; Matrix4x3 m; };
extern "C" int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(MMC *, Matrix4x3 &, short);

// @symbol func_ov025_021112e0
extern "C" void func_ov025_021112e0(char *self)
{
    Obj *o = (Obj *)self;
    o->m = *(Matrix4x3 *)(self + 0xf0);
    *(int *)(self + 0x310) = *(int *)(self + 0x5c);
    *(int *)(self + 0x314) = *(int *)(self + 0x60);
    *(int *)(self + 0x318) = *(int *)(self + 0x64);
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s((MMC *)(self + 0x124), o->m,
                                            *(short *)(self + 0x8e));
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov025_0211123c, 0x0211123c, size 0xa4               */
/* ------------------------------------------------------------------------- */
/* The proximity test Behavior ORs into its collision-range check: if the
 * nearest actor of type 9 is within eight clip radii, enable the mesh
 * collider. The volatile local is what the ROM's stack traffic shows -- the
 * vector is written, its Y raised by the slot-29 virtual, and then never read,
 * so nothing but volatile keeps the stores. */
// @symbol func_ov025_0211123c
extern "C" int func_ov025_0211123c(char *c)
{
    void *p = _ZN8dActor_c18ClosestWithActorIDEj(c, 9);
    if (p != 0) {
        volatile struct Vector3 v;
        v.x = *(int *)(c + 0x5c);
        v.y = *(int *)(c + 0x60);
        v.z = *(int *)(c + 0x64);
        v.y = v.y + ((dActor_c *)c)->OnAimedAtWithEgg();
        if (Vec3_Dist((char *)c + 0x5c, (char *)p + 0x5c) < (*(int *)(c + 0xb8) << 3)) {
            if (!((dBgW *)(c + 0x124))->IsEnabled()) {
                ((dBgW *)(c + 0x124))->Enable((dActor_c *)(c));
                return 1;
            }
        }
    }
    return 0;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN7daDgr_cD1Ev  0x021111a0  size 0x44  (complete-object destructor)     */
/*   _ZN7daDgr_cD0Ev  0x021111e4  size 0x58  (deleting destructor)            */
/* ------------------------------------------------------------------------- */
/* Defined in include/daDgr_c.h's class body, and deliberately NOT repeated out
 * of line here: out of line mwccarm emits D2, D0, D1, and the cartridge holds
 * D1, D0 with no D2 at all. The body is two vtable stores and the base
 * subobject teardown -- this class adds no member with a destructor of its
 * own. D0 additionally returns the object to its heap through the inline
 * operator delete it inherits from dActor_c, which is why nothing mentions a
 * heap. */

// @symbol _ZN7daDgr_cD0Ev
// @symbol _ZN7daDgr_cD1Ev
