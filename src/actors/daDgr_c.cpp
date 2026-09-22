//cpp
/* daDgr_c -- Spindel (actor 163 DONGURU).
 * deslop
 *
 * Leftover:
 * - dBgW_KcMbg::SetFile 6az (Fix12<int> by value) -- InitResources
 * - dBgActor_c::IsClsnInRange 6az -- Behavior
 * - dActor_c::Earthquake 6az -- Behavior
 * - Particle::System::New 6az -- Behavior
 * - func_020393d4 stores dBgW::UpdatePosAndAngs (no setter)
 * - func_02012694 grind sound 0x65 at mCamSpacePosX
 * - data_ov025_02113a68 / 02113a60 SharedFilePtr handles (sinit BSS)
 * - data_ov025_02112c28 CLPS block
 * - common.h first (func_ov025_021112e0 twelve-word Matrix4x3 copy)
 * - func_ov025_* keep ROM labels (no identifiers)
 * - g_profile_DONGURU lives outside this TU (S14)
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder.
 * The factory leads; the inline destructor in include/daDgr_c.h emits the
 * retail D1/D0 pair (0x021111a0 then 0x021111e4) with no D2.
 */

/* common.h MUST COME FIRST. It and math/Matrix.h both define Matrix4x3;
 * whichever a TU sees first stands. daDgr_c.h reaches math/Matrix.h, whose
 * nested spelling scalarizes the twelve-word copy in func_ov025_021112e0. */
#include "common.h"
#include "daDgr_c.h"
#include "SharedFilePtr.h"
#include "decl_common.h"

/* Declarations for the ROM symbols this TU calls that no project header owns.
 * These have been checked against include/decl_common.h and the decl_*.h family
 * -- where a real header declares one of these, the header wins and the local
 * spelling is gone. What remains is what no header declares. */
/* shadow typedef 'Vec3' */
typedef struct { s32 x, y, z; } Vec3;

/* shadow typedef 'Fix12i' */
typedef int Fix12i;

/* shadow struct 'BMD_File' */
struct BMD_File;

/* shadow struct 'KCL_File' */
struct KCL_File;

/* shadow struct 'CLPS_Block' */
struct CLPS_Block;

extern "C" {
extern void Matrix4x3_FromRotationX(void *, int);
/* The two file handles carry ONE spelling here, not the two the merge produced.
 * The CleanupResources shard declared them `int[]` and cast the decayed pointer;
 * the InitResources shard declared them `SharedFilePtr` and passed a reference.
 * Both are the same object, so the merged file has to choose, and the object
 * spelling is the one that lets both call sites be ordinary C++ (`.Release()`
 * and a plain reference argument) with no cast at all. */
extern SharedFilePtr data_ov025_02113a68;
extern SharedFilePtr data_ov025_02113a60;
extern s16 data_02082214[];
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(daDgr_c *self, s32 a, s32 b);
void func_02012694(s32 a, void *b);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(daDgr_c *self, Vec3 *pos, s32 fix);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
u32 slot, u32 effect, s32 x, s32 y, s32 z, const void *rot, void *cb);
/* SetFile takes Fix12<int> by value (wall 6az); a real method call DIFFs. */
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block( dBgW_KcMbg*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
void func_020393d4(int* p, int v);
extern CLPS_Block data_ov025_02112c28;
extern int _ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol daDgr_c_classInit
extern "C" daDgr_c *daDgr_c_classInit()
{
    return new daDgr_c();
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c13InitResourcesEv
/* daDgr_c::InitResources -- vtable slot 0. Loads Spindel's model and KCL,
 * points the mesh collider's update hook at dBgW::UpdatePosAndAngs, zeroes
 * the facing angle the KCL was baked against, and parks the roll state.
 *
 * The func_ov025_* helpers still take a `char *`: decl_common.h spells them
 * that way and this file does not own that header. */
s32 daDgr_c::InitResources()
{
    func_ov025_02111344((char*)this);
    func_ov025_021112e0((char*)this);
    {
        BMD_File* bmd = (BMD_File*)Model::LoadFile(data_ov025_02113a68);
        mModel.SetFile(bmd, 1, -1);
    }
    {
        KCL_File* kcl = (KCL_File*)dBgW_Kc::LoadFile(data_ov025_02113a60);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, mClsnMat, 0x1000, mAngleY, data_ov025_02112c28);
    }
    func_020393d4((int*)&mMeshCollider,
                  (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    mAngleY = 0;
    mBasePosY = mPosY;
    mAngleXSpeed = 0;
    mPhaseTimer = 0;
    mRollStage = 0;
    mRollDir = 0;
    mDustParticle2 = 0;
    mDustParticle1 = mDustParticle2;
    return 1;
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c8BehaviorEv
/* daDgr_c::Behavior -- vtable slot 6, ov025 0x021113f0.
 *
 * Spindel. It rolls about X and translates along Z. mPhaseTimer is the phase
 * counter, mRollStage the stage counter (-1 is the "hold at the end" sentinel),
 * mRollDir the direction flag. The roll speed comes from `10 - mRollStage'
 * folded to a magnitude and bucketed to 1/2/4, so the roller eases in and out;
 * mBasePosY holds the resting height that the sine offset from data_02082214
 * is added to. Two dust particle systems (mDustParticle1, mDustParticle2)
 * trail either end, and the last frame of each roll shakes the ground.
 *
 * IsClsnInRange, Earthquake and Particle::System::New all carry Fix12<int> by
 * value in their mangled names (wall 6az), so all three stay extern-C free
 * functions.
 *
 * Swept greedily against build_pin.verify -- 33 substitutions, 33 kept once the
 * three real obstacles were fixed: `mAngleY' is read UNSIGNED at the table-index
 * sites (an s16 read costs four words, the same finding daObjC1_Trap_c::InitResources
 * produced), the three func_ov025_* helpers are declared `char *' in
 * decl_common.h so `this' needs a cast, and the two-step `b = self + 0x300;
 * b + 0x28' was just mRollStage reached the long way round.
 */
s32 daDgr_c::Behavior()
{
    s32 loc[6];
    s32 n;
    s32 m;

    if (mRollStage == -1) {
        if (mPhaseTimer == 0x20) {
            mRollStage = 0;
            mPhaseTimer = 0;
        } else {
            u16 *cnt = (u16 *)(&mPhaseTimer);
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

    n = 10 - mRollStage;
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
        p = (s8 *)(&mRollStage);
        *p = (s8)(*p + 1);
        if (mRollStage == 0x14) {
            u8 *pd = (u8 *)(&mRollDir);
            *pd = (u8)(*pd ^ 1);
            mRollStage = -1;
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
        if (mRollDir == 0) {
            unk_0ac = 0x14000 / n;
            mAngleXSpeed = (s16)(0x400 / n);
        } else {
            unk_0ac = (-0x14000) / n;
            mAngleXSpeed = (s16)((-0x400) / n);
        }

        {
            s32 *pz = (s32 *)(&mPosZ);
            s16 *pr = (s16 *)(&mAngleX);
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
        u16 *cnt = (u16 *)(&mPhaseTimer);
        (*cnt)++;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c6RenderEv
/* daDgr_c::Render -- vtable slot 9. Real C++ method over the shared header;
   the Model sub-object at +0xd4 is rendered through its own vtable (mwccarm
   does not devirtualise an embedded member's virtual call). */
s32 daDgr_c::Render() { mModel.Render(0); return 1; }

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_c16CleanupResourcesEv
/* daDgr_c::CleanupResources -- vtable slot 3. Real C++ method over the shared
   header. */
s32 daDgr_c::CleanupResources() {
  data_ov025_02113a68.Release();
  data_ov025_02113a60.Release();
  if (mMeshCollider.IsEnabled())
    mMeshCollider.Disable();
  return 1;
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol func_ov025_02111344
/* Added by hand: `tubuild create` writes @symbol markers only for mangled or
 * already-named members and skips auto-named func_ovNN_ADDR shards silently, so
 * without this line tiers scoring would miss this member. The other two
 * auto-named shards got theirs because they were carried in RAW. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov025_02111344(char *t)
{
    daDgr_c *self = (daDgr_c *)t;
    Matrix4x3_FromRotationX(&self->mModel.mat4x3, self->mAngleX);
    self->mModel.mat4x3.m[9] = self->mPosX >> 3;
    self->mModel.mat4x3.m[10] = self->mPosY >> 3;
    self->mModel.mat4x3.m[11] = self->mPosZ >> 3;
}
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol func_ov025_021112e0
extern "C" {

/* THE MATRIX SPELLING HERE IS LOAD-BEARING. common.h is included first in
 * this TU so Matrix4x3 is the flat `s32 m[12]`; the nested math/Matrix.h
 * spelling scalarizes this twelve-word copy. */
void func_ov025_021112e0(char* self){
    daDgr_c* o = (daDgr_c*)self;
    o->mClsnMat = o->mModel.mat4x3;
    o->mClsnMat.m[9] = o->mPosX;
    o->mClsnMat.m[10] = o->mPosY;
    o->mClsnMat.m[11] = o->mPosZ;
    o->mMeshCollider.Transform(o->mClsnMat, o->mAngleY);
}
}

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
// @symbol func_ov025_0211123c
extern "C" {
extern int Vec3_Dist(void*, void*);

int func_ov025_0211123c(char* c) {
    daDgr_c *self = (daDgr_c *)c;
    dActor_c *p = self->ClosestWithActorID(9);
    if (p != 0) {
        volatile struct Vector3 v;
        v.x = self->mPosX;
        v.y = self->mPosY;
        v.z = self->mPosZ;
        v.y = v.y + self->OnAimedAtWithEgg();
        if (Vec3_Dist(&self->mPosX, &p->mPosX) < (self->mClipRadius << 3)) {
            if (!self->mMeshCollider.IsEnabled()) {
                self->mMeshCollider.Enable(self);
                return 1;
            }
        }
    }
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/*
 *                         _ZN7daDgr_cD1Ev (0x021111a0, size 0x44)             */
/* -------------------------------------------------------------------------- */
// @symbol _ZN7daDgr_cD1Ev
// @symbol _ZN7daDgr_cD0Ev
/* NO DEFINITION APPEARS HERE, AND THAT IS THE POINT.
 *
 * Both variants come from `virtual ~daDgr_c() {}` in include/daDgr_c.h. Each of
 * the two one-function shards carried its own out-of-line `daDgr_c::~daDgr_c()
 * {}`, which is harmless in isolation -- objisolate keeps whichever variant
 * that file is bound to -- and wrong twice over once they are merged: it is a
 * duplicate definition, and mwccarm emits the out-of-line form as D2, D0, D1,
 * which is the wrong order for this cartridge and produces a D2 with no ROM
 * home.
 *
 * ov025 puts D1 at 0x021111a0 and D0 at 0x021111e4 -- D1 BELOW D0, the
 * reproducible direction -- and carries no D2 at all. The in-class body emits
 * exactly D1 then D0, so both land inside this TU's licensed .text run in ROM
 * address order and the class promotes whole.
 *
 * D0's deallocation is an inline `operator delete` found by ordinary lookup on
 * dActor_c (include/dActor_c.h), two levels up through dBgActor_c, which is why
 * nothing here mentions a heap. D1's body is the two vptr stores plus
 * dBgActor_c's Model and dBgW_KcMbg teardowns, every one of them a consequence
 * of `struct daDgr_c : dBgActor_c` alone; this class's own 0x14 tail bytes are
 * POD and destruct nothing.
 */
