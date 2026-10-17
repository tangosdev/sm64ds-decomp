//cpp
/* daObjMaruta_c -- the abstract base of the two rolling logs, and the shared
 * implementation both of them forward to.
 *
 * Licensed .text run: ov080 0x02126fbc .. 0x0212766c, the two destructor
 * variants (D0 0x02126fbc, D1 0x02127014) and then eight methods.
 *
 * The file is in ROM order under `#pragma defer_codegen off`, with the two
 * destructor forcers first. include/daObjMaruta_c.h keeps the destructor in
 * the class body so that both descendants inline it; deferred codegen emits
 * that inline body as D1 then D0, and the cartridge has D0 below D1. Do not
 * reorder.
 *
 * ABSTRACT: no classInit in this TU. The leaves own the factories
 * (ov022 daObjFlMaruta_c, ov030 daObjHmMaruta_c).
 *
 * Remaining interfaces:
 * Leftover: func_ov080_021274ac / 0212714c / 021270dc keep C linkage.
 *   daObjFlMaruta_c (ov022) and daObjHmMaruta_c (ov030) forward InitResources,
 *   Behavior and CleanupResources into these exact symbols. No ROM-proven
 *   English spelling; a member rename would fan those leaves. THIS-TU reason.
 * Leftover: func_ov080_0212758c / 02127658 stay placeholder labels
 *   (BeforeClsn body + arg-shifting veneer). 02127658 is stored as a
 *   dBgW callback address, so it cannot be a non-static member.
 * Leftover: dBgW_KcMbg::SetFile 6az: InitResources passes Fix12<int> by
 *   value; the header method form size-DIFFs.
 * Leftover: dBgActor_c::UpdateKillByMegaChar 6az: Behavior passes
 *   Fix12<int> by value; a method call homes the argument.
 * Leftover: dBgActor_c::IsClsnInRange / IsClsnInRangeOnScreen 6az
 *   (Behavior; header method form refused).
 * Leftover: func_020393d4 / func_020393c4 are 4-byte stores into dBgW+0x18
 *   / +0x1c. This TU stores dBgW::UpdatePosWithVelocity and
 *   &func_ov080_02127658; dBgW.h has no setter.
 * Leftover: func_02012694 (sound 0x1e at mCamSpacePosX).
 * Leftover: Matrix4x3_FromRotationXYZExt has no shared header.
 * Leftover: Vec3_Sub / LenVec3 / Vec3_HorzAngle / Vec3_Dist / AngleDiff:
 *   no shared header this TU can take without a campaign.
 * Leftover: (Vector3 *)&mRestPosX / &mHitPosX: the class stores those
 *   triples as scalars (issue #2566).
 * Leftover: sine table data_02082214 (arm9).
 * Leftover: no return new -- this class is abstract; the leaves own
 *   classInit.
 * Leftover: common.h first so func_ov080_02127094's Matrix4x3 translation
 *   stores use the flat s32 m[12] spelling (math/Matrix.h nested form
 *   scalarises).
 */

#pragma defer_codegen off

#include "common.h"
#include "daObjMaruta_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgW.h"
#include "nitro/fx/fx.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

extern "C" {
void func_02012694(int a, void *b);
void Matrix4x3_FromRotationXYZExt(void *mtx, int x, int y, int z);
int DecIfAbove0_Byte(u8 *p);
int Vec3_Dist(void *a, void *b);
short Vec3_HorzAngle(void *a, void *b);
int AngleDiff(int a, int b);
void Vec3_Sub(void *out, void *a, void *b);
int LenVec3(void *v);
u16 DecIfAbove0_Short(u16 *p);
int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(
    dBgActor_c *self, s16 a, s16 b, s16 c, int d);
int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
extern s16 data_02082214[];

int func_ov080_021270dc(daObjMaruta_c *self, ResourceDescriptor *arg);
int func_ov080_0212714c(daObjMaruta_c *self, int *maxDist);
int func_ov080_021274ac(daObjMaruta_c *self, ResourceDescriptor *arg);
void func_ov080_0212758c(daObjMaruta_c *self, dActor_c *other);
void func_ov080_02127658(void *a, void *b, void *c);
}

/* -------------------------------------------------------------------------- */
/* ROM 0x02126fbc (D0, size 0x58) and 0x02127014 (D1, size 0x44)              */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjMaruta_cD0Ev
// @symbol _ZN13daObjMaruta_cD1Ev
/* No source here: both destructor variants come from the ONE inline body in
 * include/daObjMaruta_c.h, which daObjFlMaruta_c and daObjHmMaruta_c need
 * visible to inline it.
 *
 * The two calls below are never executed. Under `#pragma defer_codegen off`
 * the compiler emits ordinary functions at parse time in source order, so the
 * delete-expression pulls the deleting variant out of line first and the
 * explicit destructor call pulls the complete-object variant out second --
 * the cartridge's D0-then-D1 order, with no D2. */

/* Not called. Forces the out-of-line copy of the deleting destructor. */
void daObjMaruta_c_EmitDeletingDestructor(daObjMaruta_c *p)
{
    delete p;
}

/* Not called. Forces the out-of-line copy of the inline destructor. */
void daObjMaruta_c_EmitDestructor(daObjMaruta_c *p)
{
    p->~daObjMaruta_c();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN13daObjMaruta_c15OnHitByMegaCharER6Player, 0x02127058 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjMaruta_c15OnHitByMegaCharER6Player
/* Vtable slot 27, and this class's KEY FUNCTION: the destructor is inline in
   the class body, so the first out-of-line virtual declared anchors
   _ZTV13daObjMaruta_c and the RTTI chain, and this TU is where it is defined.
   dBgActor_c::KillByMegaChar is non-virtual, so the unqualified call is the
   direct `bl` the ROM has. */
void daObjMaruta_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    func_02012694(0x1e, &mCamSpacePosX);
    KillByMegaChar(player);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- func_ov080_02127094, 0x02127094, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjMaruta_c19func_ov080_02127094Ev
void daObjMaruta_c::func_ov080_02127094()
{
    Matrix4x3_FromRotationXYZExt(&mModel.mat4x3,
        mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov080_021270dc, 0x021270dc, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_021270dc
extern "C" int func_ov080_021270dc(daObjMaruta_c *self, ResourceDescriptor *arg)
{
    if (self->mMeshCollider.IsEnabled())
        self->mMeshCollider.Disable();
    arg->model->Release();
    arg->collision->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13daObjMaruta_c6RenderEv, 0x02127124, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjMaruta_c6RenderEv
/* Dispatches through the Model at 0xd4 (dBgActor_c's mModel, inherited) --
   Model::Render(const Vector3 *) with a null scale, matching the ROM's
   literal-0 argument. */
s32 daObjMaruta_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov080_0212714c, 0x0212714c, size 0x360 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_0212714c
extern "C" int func_ov080_0212714c(daObjMaruta_c *self, int *maxDist)
{
    Vector3 tmp;
    int len;
    int ang;
    s16 v;
    int horz;

    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(
            self, (s16)-0x1500, 0, 0, 0) != 0)
        return 1;
    if ((int)((self->mFlags & 8) != 0) != 0)
        return 1;

    if (self->mHitByPlayer == 0) {
        if (DecIfAbove0_Byte(&self->mHitTimer)) {
            if (self->mRollAngVel > 0) {
                self->mRollAngVel -= 0x10;
            } else {
                self->mRollAngVel += 0x10;
            }
            v = self->mRollAngVel;
            if (v <= 0x10 && v >= (s16)-0x10)
                self->mRollAngVel = 0;
        } else {
            v = self->mRollAngVel;
            if (v != 0x100) {
                if (v > 0x100) {
                    self->mRollAngVel -= 0x10;
                } else {
                    self->mRollAngVel += 0x10;
                }
                v = self->mRollAngVel;
                if (v < 0x110 && v > 0xf0)
                    self->mRollAngVel = 0x100;
            }
        }
        if (Vec3_Dist(&self->mPos, (Vector3 *)&self->mRestPosX) < 0x2000)
            self->mRollAngVel = 0;
    } else {
        horz = Vec3_HorzAngle(&self->mPos, (Vector3 *)&self->mHitPosX);
        AngleDiff(horz, self->mAngleY);
        ang = AngleDiff(horz, self->mAngleY);
        if (ang > 0x4000) {
            self->mRollAngVel -= 0x10;
        } else {
            self->mRollAngVel += 0x10;
        }
        v = self->mRollAngVel;
        {
            int lim = 0x200;
            int nlim = -lim;
            if (v < nlim)
                v = (s16)nlim;
            else if (v > lim)
                v = (s16)lim;
        }
        self->mRollAngVel = v;
    }

    {
        int d = self->mRollAngVel;
        self->mHorzSpeed = (d / 64) << 12;
    }
    self->UpdatePos(0);
    Vec3_Sub(&tmp, &self->mPos, (Vector3 *)&self->mRestPosX);
    len = LenVec3(&tmp);
    ang = Vec3_HorzAngle(&self->mPos, (Vector3 *)&self->mRestPosX);
    ang = AngleDiff(ang, self->mAngleY);
    if (ang >= 0x7f00)
        ang = 0x8000;
    if (ang <= 0x100)
        ang = 0;
    if (len > *maxDist) {
        self->mPosX = self->mPrevPosX;
        self->mPosY = self->mPrevPosY;
        self->mPosZ = self->mPrevPosZ;
    }
    if (ang == self->mPathAngDiff || ang == 0x8000) {
        self->mPosX = self->mPrevPosX;
        self->mPosY = self->mPrevPosY;
        self->mPosZ = self->mPrevPosZ;
        self->mHitTimer = 0xa;
    }
    self->mAngleX += self->mRollAngVel;
    {
        int mask = 0x1fff;
        if ((self->mAngleX & mask) < 0x210 && self->mRollAngVel != 0) {
            if (self->mSoundTimer == 0) {
                Sound::PlayBank3(0x25, self->mCamSpacePos);
                self->mSoundTimer = 0xe;
            }
        } else {
            DecIfAbove0_Short(&self->mSoundTimer);
        }
    }
    self->func_ov080_02127094();
    {
        int is66 = (int)(self->actorID == 0x66);
        if (is66 != 0) {
            if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(self, 0, 0) != 0)
                self->UpdateClsnPosAndRot();
        } else {
            if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0, 0) != 0)
                self->UpdateClsnPosAndRot();
        }
    }
    self->mHitByPlayer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov080_021274ac, 0x021274ac, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_021274ac
extern "C" int func_ov080_021274ac(daObjMaruta_c *self, ResourceDescriptor *arg)
{
    struct Vector3 d;
    self->mModel.SetFile(
        (BMD_File *)Model::LoadFile(*arg->model), 1, -1);
    self->func_ov080_02127094();
    self->UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &self->mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*arg->collision),
        &self->mClsnMat, 0x1000, self->mAngleY, arg->clps);
    func_020393d4((int *)&self->mMeshCollider,
        (int)&dBgW::UpdatePosWithVelocity);
    func_020393c4((int *)&self->mMeshCollider, (int)&func_ov080_02127658);
    self->mRestPosX = self->mPosX;
    self->mRestPosY = self->mPosY;
    self->mRestPosZ = self->mPosZ;
    Vec3_Sub(&d, &self->mPos, (Vector3 *)&self->mRestPosX);
    LenVec3(&d);
    self->mPathAngDiff =
        AngleDiff(Vec3_HorzAngle(&self->mPos, (Vector3 *)&self->mRestPosX), self->mAngleY);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov080_0212758c, 0x0212758c, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_0212758c
/* BeforeClsn helper: if the other actor's actorID is PLAYER (0xbf), copy
   its mPos into mHitPos, arm mHitByPlayer / mHitTimer, and shove that
   actor along mAngleY by half of mHorzSpeed. Retain the volatile actor-ID
   read and the position view: separate scalar member loads change codegen. */
extern "C" void func_ov080_0212758c(daObjMaruta_c *self, dActor_c *other)
{
    int t;
    int half;

    t = (*(volatile u16 *)&other->actorID == 0xbf);
    if (t != 0) {
        Vector3 *position = &other->mPos;
        self->mHitPosX = position->x;
        self->mHitPosY = position->y;
        self->mHitPosZ = position->z;
        self->mHitByPlayer = 1;
        self->mHitTimer = 0x78;

        half = self->mHorzSpeed >> 1;

        {
            int idx = ((u16)self->mAngleY) >> 4;
            int sc = data_02082214[idx * 2];
            int v = position->x;
            position->x = v + FX_MUL(half, sc);
        }

        {
            int idx = ((u16)self->mAngleY) >> 4;
            int sc = data_02082214[idx * 2 + 1];
            int v = position->z;
            position->z = v + FX_MUL(half, sc);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov080_02127658, 0x02127658, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_02127658
/* Arg-shifting tail-call veneer, handed to the dBgW at mMeshCollider by
   func_ov080_021274ac below. Drops the first argument and forwards the next
   two; long_calls emits the pooled `ldr ip,[pc,#8]; bx ip` absolute
   tail-call. Bracketed closed immediately: long_calls is positional. */
#pragma long_calls on
extern "C" void func_ov080_02127658(void *a, void *b, void *c)
{
    func_ov080_0212758c((daObjMaruta_c *)b, (dActor_c *)c);
}
#pragma long_calls off
