//cpp
/* Production translation unit for ov065/daObjCtMecha03_c.
 *
 * deslop
 *
 * Leftover:
 * - dBgW_KcMbg::SetFile / DropShadowScaleXYZ / dBgActor_c::IsClsnInRange stay
 *   mangled (Fix12-by-value, 6az)
 * - func_020393a4 / func_02039394 / func_020393d4 store dBgW range/callback
 *   (no setter)
 * - Behavior I16(0x322) / accelP mask / 0x300+0x22 load (named mSwingAngle /
 *   mSwingSpeed CSE to the r4+0x300 base; DIFF)
 * - func_ov065_02119fe8 / func_ov065_0211a114 keep ROM address names
 * - data_ov065_* handles; this TU is text-only (S14 no g_profile_CT_MECHA03)
 * - data_ov035_02112198 CLPS_Block (overlay_residency settlement, not a ROM name)
 * - common.h first (shadow/model matrices need the flat 12-word spelling)
 * - return new emits homeless _ZN10dBgActor_cD2Ev; compiler-only policy deadstrips it
 *
 * mwccarm emits ordinary functions in reverse source order, so the nine
 * definitions below intentionally run from the highest retail address back
 * toward the compiler-owned destructor group. Keep the factory first.
 *
 * func_ov065_02119fe8 and func_ov065_0211a114 keep their address-derived
 * names: ov065's symbols.txt spells them that way, so they are C-linkage
 * free functions here rather than members.
 *
 * The factory is `return new` in this file.
 */

#include "common.h"
#include "daObjCtMecha03_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "Sound.h"

struct CLPS_Block;

/* Behavior reads and writes 0x322 through a RAW combined offset -- not the
   (c+0x300)+0x22 decomposition used everywhere else in that function. The mask
   is a no-op on a 32-bit int; its only purpose is to stop the compiler from
   recognizing 0x322 as 0x300+0x22 and reusing the r4+0x300 base already live in
   a register nearby. The ROM computes this one address via a literal-pool add
   instead. */
#define I16(off) (*(short *)(((int)this + (off)) & 0xFFFFFFFF))

/* Fix12-by-value calls retain their measured raw ABI declarations. Natural
 * class-typed declarations make mwccarm home arguments absent from retail. */
extern "C" {
extern void Matrix4x3_FromRotationZXYExt(Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angle);
extern void MulVec3Mat4x3(Vector3 *in, Matrix4x3 *m, Vector3 *out);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);
extern void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    dActor_c *actor, ShadowModel *shadow, Matrix4x3 *matrix,
    int scaleX, int scaleY, int scaleZ, u32 opacity);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *thisp, KCL_File *kcl, const Matrix4x3 &mtx, int fix, short s,
    CLPS_Block &clps);
extern void func_02039394(void *p, int v);
extern void func_020393a4(void *p, int v);
extern void func_020393d4(void *p, void *v);
extern int func_ov065_0211a114(daObjCtMecha03_c *self);
extern void func_ov065_02119fe8(daObjCtMecha03_c *self);
extern Matrix4x3 data_020a0e68;
extern u8 data_0209f2c0;
extern int data_0209e650;
extern s16 data_ov065_0211c0b0[];
extern SharedFilePtr data_ov065_0211d88c;
extern SharedFilePtr data_ov065_0211d894;
extern int data_ov035_02112198;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjCtMecha03_c_classInit
extern "C" daObjCtMecha03_c *daObjCtMecha03_c_classInit()
{
    return new daObjCtMecha03_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c13InitResourcesEv
int daObjCtMecha03_c::InitResources()
{
    void *bmd;
    void *kcl;

    bmd = Model::LoadFile(data_ov065_0211d88c);
    mModel.SetFile((BMD_File *)bmd, 1, -1);
    mShadowModel.InitCuboid();

    mSwingDir = 1;
    mSwingAccel = data_ov065_0211c0b0[data_0209f2c0];
    mSwingAngle = 0x1964;
    mAngleZ = mSwingAngle;

    func_ov065_0211a114(this);
    UpdateClsnPosAndRot();

    kcl = dBgW_Kc::LoadFile(data_ov065_0211d894);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)kcl, mClsnMat, 0x1000,
        mAngleY, *(CLPS_Block *)&data_ov035_02112198);

    func_020393a4(&mMeshCollider, 0x300000);
    func_02039394(&mMeshCollider, -0x200000);

    if (data_0209f2c0 != 3) {
        func_020393d4(&mMeshCollider,
            (void *)&dBgW::UpdatePosWithTransform);
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c8BehaviorEv
int daObjCtMecha03_c::Behavior()
{
    if (data_0209f2c0 != 3) {
        if (*(unsigned short *)&mSoundTimer != 0) {
            if (DecIfAbove0_Short((u16 *)&mSoundTimer) == 0) {
                Sound::PlayBank3(0x38, *(Vector3 *)&mCamSpacePosX);
            }
        }
        if (DecIfAbove0_Short((u16 *)&mPauseTimer) == 0) {
            {
                short vx = mSwingDir;
                short vy = *(short *)((char *)this + 0x300 + 0x22);
                short *accelP = (short *)(((int)this + 0x324) & 0xFFFFFFFF);
                if (vy * vx > 0) {
                    vx = -vx;
                    mSwingDir = vx;
                }
                {
                    short spd = mSwingAccel;
                    short pos = mSwingDir;
                    short accel = *accelP;
                    *accelP = (short)(spd * pos + accel);
                }
            }
            if (data_0209f2c0 == 2 && mSwingSpeed == 0) {
                int r0 = RandomIntInternal(&data_0209e650);
                if ((unsigned)r0 % 3 != 0)
                    mSwingAccel = 0xd;
                else
                    mSwingAccel = 0x2a;
                if ((r0 & 1) == 0) {
                    mPauseTimer = ((unsigned)r0 >> 0x1b) + 3;
                }
            }
            if (mSwingSpeed == 0) {
                mSoundTimer = *(unsigned short *)&mPauseTimer + 0xf;
            }
            I16(0x322) = I16(0x322) + mSwingSpeed;
        }
        mAngleZ = mSwingAngle;
    }

    func_ov065_0211a114(this);
    func_ov065_02119fe8(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x300000, -0x200000) != 0)
        UpdateClsnPosAndRot();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c6RenderEv
int daObjCtMecha03_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha03_c16CleanupResourcesEv
int daObjCtMecha03_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d88c.Release();
    data_ov065_0211d894.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_0211a114
/* Writes mModel.mat4x3 from the actor's Y/Z angles and copies the
   actor position into its translation row. */
extern "C" int func_ov065_0211a114(daObjCtMecha03_c *c)
{
    Matrix4x3_FromRotationZXYExt(&c->mModel.mat4x3, 0, c->mAngleY, c->mAngleZ);
    c->mModel.mat4x3.m[9] = c->mPosX >> 3;
    c->mModel.mat4x3.m[10] = c->mPosY >> 3;
    int z = c->mPosZ >> 3;
    c->mModel.mat4x3.m[11] = z;
    return z;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov065_02119fe8
/* Drops the pendulum's shadow: swings a fixed offset through the actor's
   orientation, raycasts the ground under the result, then hands the shadow
   matrix to dActor_c::DropShadowScaleXYZ. */
extern "C" void func_ov065_02119fe8(daObjCtMecha03_c *self)
{
    Vector3 v1;
    Vector3 v2;
    Vector3 pos;
    v1.y = 0;
    v1.y = -0x320000;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0;
    v1.x = 0;
    v1.z = 0;
    Matrix4x3_FromRotationZXYExt(&data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    MulVec3Mat4x3(&v1, &data_020a0e68, &v2);
    AddVec3(&v2, (Vector3 *)&self->mPosX, &v2);
    {
        int vy = v2.y;
        pos.x = v2.x;
        pos.z = v2.z;
        pos.y = vy;
        pos.y = vy - 0xc8000;
    }
    dBgCh_Gnd rc;
    rc.SetObjAndPos(pos, 0);
    self->mGroundY = pos.y;
    if (rc.DetectClsn())
        self->mGroundY = rc.clsnY;
    Matrix4x3_FromRotationY(&self->mShadowMat, self->mAngleY);
    self->mShadowMat.m[9] = v2.x >> 3;
    self->mShadowMat.m[10] = self->mGroundY >> 3;
    self->mShadowMat.m[11] = v2.z >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, &self->mShadowModel, &self->mShadowMat, 0x12c000, 0x12c000, 0x78000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 order without
 * the otherwise homeless D2 variant. */
