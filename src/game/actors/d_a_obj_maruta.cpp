//cpp
/* daObjMaruta_c -- the abstract base of the two rolling logs, and the shared
 * implementation both of them forward to.
 *
 * Licensed .text run: ov080 0x02127058 .. 0x0212766c, eight functions.
 *
 * The class's two destructor variants sit just below this run, at 0x02126fbc
 * (D0) and 0x02127014 (D1), and stay in their own enrolled sources. The
 * cartridge orders D0 BELOW D1; with the destructor defined in the class body
 * -- which include/daObjMaruta_c.h requires, so that both descendants inline
 * the body instead of calling it -- mwccarm 2004/b56 emits D1 then D0, and no
 * admissible source form reverses that. This TU emits its own copies of both
 * because it owns the key function, and they are licensed as deadstripped
 * duplicates.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the reverse of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * ABSTRACT: no classInit in this TU. The leaves own the factories
 * (ov022 RollingLogLll, ov030 RollingLogTtm).
 *
 * deslop
 * Leftover: func_ov080_021274ac / 0212714c / 021270dc keep C linkage.
 *   RollingLogLll (ov022) and RollingLogTtm (ov030) forward InitResources,
 *   Behavior and CleanupResources into these exact symbols. No ROM-proven
 *   English spelling; a member rename would fan those leaves. THIS-TU reason.
 * Leftover: func_ov080_02127094 keeps C linkage (private matrix helper;
 *   same shape as func_ov036_0211123c). Member form unmeasured. Parameter
 *   stays char * because include/decl_common.h declares it that way.
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
 * Leftover: (Vector3 *)&mPosX / &mRestPosX / &mHitPosX / &mCamSpacePosX:
 *   dActor_c stores the triples as scalars (issue #2566).
 * Leftover: Behavior's mRollAngVel / mSoundTimer walks keep
 *   (s16*)((c+0x300)+0x3c) / (c+0x33c) -- named mRollAngVel CSE's the
 *   field address (same class as daObjBC_Switch_c 0x32a).
 * Leftover: sine table data_02082214 (arm9).
 * Leftover: no return new -- this class is abstract; the leaves own
 *   classInit.
 * Leftover: inline destructor (out-of-line emits D0 before D1 plus a
 *   homeless D2). D0/D1 stay in their enrolled shards.
 * Leftover: common.h first so func_ov080_02127094's Matrix4x3 translation
 *   stores use the flat s32 m[12] spelling (math/Matrix.h nested form
 *   scalarises).
 */

#include "common.h"
#include "daObjMaruta_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "dBgW.h"

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

void func_ov080_02127094(char *self);
int func_ov080_021270dc(daObjMaruta_c *self, ResourceDescriptor *arg);
int func_ov080_0212714c(daObjMaruta_c *self, int *maxDist);
int func_ov080_021274ac(daObjMaruta_c *self, ResourceDescriptor *arg);
void func_ov080_0212758c(daObjMaruta_c *self, char *other);
void func_ov080_02127658(void *a, void *b, void *c);
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
    func_ov080_0212758c((daObjMaruta_c *)b, (char *)c);
}
#pragma long_calls off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov080_0212758c, 0x0212758c, size 0xcc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_0212758c
/* BeforeClsn helper: if the other actor's actorID is PLAYER (0xbf), copy
   its mPos into mHitPos, arm mHitByPlayer / mHitTimer, and shove that
   actor along mAngleY by half of mHorzSpeed. The volatile load and the
   a1 rebase onto +0x5c are load-bearing. */
extern "C" void func_ov080_0212758c(daObjMaruta_c *self, char *a1)
{
    int t;
    int half;

    t = (*(volatile u16 *)(a1 + 0xc) == 0xbf);
    if (t != 0) {
        a1 = (char *)(int)(a1 + 0x5c);
        self->mHitPosX = *(int *)a1;
        self->mHitPosY = *(int *)(a1 + 4);
        self->mHitPosZ = *(int *)(a1 + 8);
        self->mHitByPlayer = 1;
        self->mHitTimer = 0x78;

        half = self->mHorzSpeed >> 1;

        {
            int idx = ((u16)self->mAngleY) >> 4;
            int sc = data_02082214[idx * 2];
            int v = *(int *)a1;
            *(int *)a1 = v + (int)(((long long)half * sc + 0x800) >> 12);
        }

        a1 = (char *)(int)(a1 + 8);
        {
            int idx = ((u16)self->mAngleY) >> 4;
            int sc = data_02082214[idx * 2 + 1];
            int v = *(int *)a1;
            *(int *)a1 = v + (int)(((long long)half * sc + 0x800) >> 12);
        }
    }
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
    func_ov080_02127094((char *)self);
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
    Vec3_Sub(&d, (Vector3 *)&self->mPosX, (Vector3 *)&self->mRestPosX);
    LenVec3(&d);
    self->mPathAngDiff = AngleDiff(
        Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mRestPosX),
        self->mAngleY);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov080_0212714c, 0x0212714c, size 0x360 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_0212714c
extern "C" int func_ov080_0212714c(daObjMaruta_c *self, int *maxDist)
{
    char *c = (char *)self;
    int tmp[3];
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
        if (DecIfAbove0_Byte((u8 *)((int)c + 0x33e))) {
            if (*(s16 *)((c + 0x300) + 0x3c) > 0) {
                s16 *p = (s16 *)(int)(c + 0x33c);
                *p = (s16)(*p - 0x10);
            } else {
                s16 *p = (s16 *)(int)(c + 0x33c);
                *p = (s16)(*p + 0x10);
            }
            v = *(s16 *)((c + 0x300) + 0x3c);
            if (v <= 0x10 && v >= (s16)-0x10)
                *(s16 *)((c + 0x300) + 0x3c) = 0;
        } else {
            v = *(s16 *)((c + 0x300) + 0x3c);
            if (v != 0x100) {
                if (v > 0x100) {
                    s16 *p = (s16 *)(int)(c + 0x33c);
                    *p = (s16)(*p - 0x10);
                } else {
                    s16 *p = (s16 *)(int)(c + 0x33c);
                    *p = (s16)(*p + 0x10);
                }
                v = *(s16 *)((c + 0x300) + 0x3c);
                if (v < 0x110 && v > 0xf0)
                    *(s16 *)((c + 0x300) + 0x3c) = 0x100;
            }
        }
        if (Vec3_Dist((Vector3 *)&self->mPosX, (Vector3 *)&self->mRestPosX) < 0x2000)
            *(s16 *)((c + 0x300) + 0x3c) = 0;
    } else {
        horz = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mHitPosX);
        AngleDiff(horz, self->mAngleY);
        ang = AngleDiff(horz, self->mAngleY);
        if (ang > 0x4000) {
            s16 *p = (s16 *)(int)(c + 0x33c);
            *p = (s16)(*p - 0x10);
        } else {
            s16 *p = (s16 *)(int)(c + 0x33c);
            *p = (s16)(*p + 0x10);
        }
        v = *(s16 *)((c + 0x300) + 0x3c);
        {
            int lim = 0x200;
            int nlim = -lim;
            if (v < nlim)
                v = (s16)nlim;
            else if (v > lim)
                v = (s16)lim;
        }
        *(s16 *)((c + 0x300) + 0x3c) = v;
    }

    {
        int d = *(s16 *)((c + 0x300) + 0x3c);
        int t = d + ((unsigned)(d >> 5) >> 26);
        self->mHorzSpeed = (t >> 6) << 12;
    }
    self->UpdatePos(0);
    Vec3_Sub(tmp, (Vector3 *)&self->mPosX, (Vector3 *)&self->mRestPosX);
    len = LenVec3(tmp);
    ang = Vec3_HorzAngle((Vector3 *)&self->mPosX, (Vector3 *)&self->mRestPosX);
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
    {
        s16 *px = (s16 *)(int)(c + 0x8c);
        *px = (s16)(*px + *(s16 *)((c + 0x300) + 0x3c));
    }
    {
        int mask = 0x1fff;
        if ((self->mAngleX & mask) < 0x210 && *(s16 *)((c + 0x300) + 0x3c) != 0) {
            if (*(u16 *)((c + 0x300) + 0x40) == 0) {
                Sound::PlayBank3(0x25, *(const Vector3 *)&self->mCamSpacePosX);
                *(u16 *)((c + 0x300) + 0x40) = 0xe;
            }
        } else {
            DecIfAbove0_Short((u16 *)(c + 0x340));
        }
    }
    func_ov080_02127094((char *)self);
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
/* ROM ordinal 1 -- func_ov080_02127094, 0x02127094, size 0x48 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov080_02127094
extern "C" void func_ov080_02127094(char *self_)
{
    daObjMaruta_c *self = (daObjMaruta_c *)self_;
    Matrix4x3_FromRotationXYZExt(&self->mModel.mat4x3,
        self->mAngleX, self->mAngleY, self->mAngleZ);
    self->mModel.mat4x3.m[9]  = self->mPosX >> 3;
    self->mModel.mat4x3.m[10] = self->mPosY >> 3;
    self->mModel.mat4x3.m[11] = self->mPosZ >> 3;
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

/* Below this point the compiler emits, unbidden, _ZN13daObjMaruta_cD1Ev and
   _ZN13daObjMaruta_cD0Ev in that order, plus the vtable and the RTTI chain --
   the price of owning the key function. Only the vtable and RTTI are wanted;
   the two destructor bodies are the cartridge's, in the cartridge's bytes, but
   in the opposite order, so they are deliberately outside the licensed run
   above and stay in the two enrolled sources that already hold them. */
