//cpp
/* daObjCtMecha03_c -- the swinging pendulum of Tick Tock Clock (CT_MECHA03),
 * ov065.
 *
 * It swings under mSwingAccel; in clock mode 2 (data_0209f2c0) each stop
 * rolls a new push strength and an occasional pause. Its shadow follows the
 * bob, not the pivot (func_ov065_02119fe8).
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *
 *   mwccarm emits ordinary functions in reverse source order, so the nine
 *   definitions below run from the highest retail address back toward the
 *   compiler-owned destructor group. Keep the factory first. The factory is
 *   `return new` here; it emits a homeless _ZN10dBgActor_cD2Ev that the
 *   compiler-only policy deadstrips.
 *
 *   common.h first: the shadow and model matrices need the flat 12-word
 *   spelling.
 *
 *   Behavior's I16(0x322), the accelP mask and the 0x300+0x22 load. The named
 *   mSwingAngle / mSwingSpeed CSE to the r4+0x300 base and do not match.
 *
 *   dBgW_KcMbg::SetFile / DropShadowScaleXYZ / dBgActor_c::IsClsnInRange stay
 *   mangled: Fix12<int> by value (notes/mwccarm-codegen.md 6az).
 *
 * Known limits:
 *   func_ov065_02119fe8 and func_ov065_0211a114 keep their address-derived
 *   names: ov065's symbols.txt spells them that way, so they are C-linkage
 *   free functions here rather than members.
 *   func_020393a4 / func_02039394 / func_020393d4 store the dBgW range and
 *   callback; there is no setter.
 *
 * NOT OWNED BY THIS TU (it is text-only): the data_ov065_* handles; no
 * g_profile_CT_MECHA03; data_ov035_02112198, the CLPS_Block, whose
 * name is an overlay_residency settlement rather than a ROM name.
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

// @symbol daObjCtMecha03_c_classInit
extern "C" daObjCtMecha03_c *daObjCtMecha03_c_classInit()
{
    return new daObjCtMecha03_c();
}

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
                int roll = RandomIntInternal(&data_0209e650);
                if ((unsigned)roll % 3 != 0)
                    mSwingAccel = 0xd;
                else
                    mSwingAccel = 0x2a;
                if ((roll & 1) == 0) {
                    mPauseTimer = ((unsigned)roll >> 0x1b) + 3;
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

// @symbol _ZN16daObjCtMecha03_c6RenderEv
int daObjCtMecha03_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjCtMecha03_c16CleanupResourcesEv
int daObjCtMecha03_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov065_0211d88c.Release();
    data_ov065_0211d894.Release();
    return 1;
}

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

// @symbol func_ov065_02119fe8
/* Drops the pendulum's shadow: swings a fixed offset through the actor's
   orientation, raycasts the ground under the result, then hands the shadow
   matrix to dActor_c::DropShadowScaleXYZ. */
extern "C" void func_ov065_02119fe8(daObjCtMecha03_c *self)
{
    Vector3 armOffset;
    Vector3 bobPos;
    Vector3 pos;
    armOffset.y = 0;
    armOffset.y = -0x320000;
    bobPos.x = 0;
    bobPos.y = 0;
    bobPos.z = 0;
    armOffset.x = 0;
    armOffset.z = 0;
    Matrix4x3_FromRotationZXYExt(&data_020a0e68, self->mAngleX, self->mAngleY, self->mAngleZ);
    MulVec3Mat4x3(&armOffset, &data_020a0e68, &bobPos);
    AddVec3(&bobPos, (Vector3 *)&self->mPosX, &bobPos);
    {
        int bobY = bobPos.y;
        pos.x = bobPos.x;
        pos.z = bobPos.z;
        pos.y = bobY;
        pos.y = bobY - 0xc8000;
    }
    dBgCh_Gnd rc;
    rc.SetObjAndPos(pos, 0);
    self->mGroundY = pos.y;
    if (rc.DetectClsn())
        self->mGroundY = rc.clsnY;
    Matrix4x3_FromRotationY(&self->mShadowMat, self->mAngleY);
    self->mShadowMat.m[9] = bobPos.x >> 3;
    self->mShadowMat.m[10] = self->mGroundY >> 3;
    self->mShadowMat.m[11] = bobPos.z >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        self, &self->mShadowModel, &self->mShadowMat, 0x12c000, 0x12c000, 0x78000, 0xf);
}

/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 order without
 * the otherwise homeless D2 variant. */
