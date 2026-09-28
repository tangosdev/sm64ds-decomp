//cpp
/* daBtfly_c -- butterfly (actor 0x150) on ov100. Kind 0 is the spawner:
 * State4 releases three more of actor 0x150, and kind 1 turns into a 1-Up
 * (actor 0x114) in State6. Behavior dispatches State0..State7 through
 * data_ov100_02148628, a pointer-to-member table.
 *
 * `#pragma defer_codegen off` is load-bearing. It makes the
 * `#pragma opt_common_subs` bracket around State2 bind (without the pair
 * State2 is 8 bytes short of 0x244) and it emits .text in source order, so
 * this file is ROM-ascending: D1, D0, State7..State0, CleanupResources,
 * OnPendingDestroy, Render, Behavior, InitResources
 * (0x02140d80..0x02141ea4). Do not reorder the functions without the pragma.
 *
 * One ~daBtfly_c() emits D1 and D0 together. A second hand-mangled D0 beside
 * it is the mwccarm ICE (ELFgen.c:483).
 *
 * deslop leftovers:
 * - State7 mFlutterPhase / mScale: named += is 0x234 vs 0x258. The phase
 *   load is ldrsh plus a zero-extend shift (ldrh of the field is shorter),
 *   and the adds take the int-cast address, not add #0x300.
 * - State7 Player::Hurt: method form is 0x264 vs 0x258 (+0xc). Fix12<int>
 *   by value. Player.h does not declare Hurt.
 * - State7 UpdateContinuous(): WRONG-DEST. ROM calls
 *   dBgCh_Actr_UpdateContinuous_Veneer (0x020383fc).
 * - State7 func_02035638 is mClsnFlags & 0x10. No method; IsOnGround is
 *   mFlags & 0x10.
 * - Behavior DropShadowRadHeight: method form is 0x24c vs 0x22c (+0x20,
 *   both calls). Fix12<int> by value.
 * - Behavior scratch matrix: Matrix4x3 assignment is 0x264 vs 0x22c.
 *   Flat Mtx copy is the ROM ldm/stm; the real type's translation is a
 *   Vector3 and runs ~Vector3.
 * - InitResources ModelAnim::SetAnim: method form is 0x244 vs 0x238
 *   (+0xc). Fix12<int> speed.
 * - InitResources dBgCh_Actr::Init: header Fix12i mangles as cii. ROM
 *   symbol is 5Fix12IiES3_.
 * - InitResources dCcAcPos_c::Init: method form is 0x248 vs 0x238
 *   (+0x10). Fix12<int> by value, and a stack Vector3 emits ~Vector3, so
 *   the offset stays a destructor-free Vec3.
 * - State0: building the angle triple before reading param1 differs by
 *   10 words.
 * - State6 SetRanges stays the scalar symbol. dActor_c does not declare it.
 * - daBtfly_c_classInit and g_profile_BUTTERFLY sit past this text run.
 */

#include "daBtfly_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "decl_Actor.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* Makes the per-member `#pragma opt_common_subs` bracket around State2 bind.
 * Without it mwccarm defers codegen and the last file-global state wins. */
#pragma defer_codegen off

/* Flat 12-word copy. Assigning Matrix4x3 would run ~Vector3 on its translation. */
struct Mtx { int w[12]; };

/* No destructor. dCcAcPos_c::Init's offset argument is passed as a pointer;
 * a stack Vector3 here would emit ~Vector3. */
struct Vec3 { s32 x, y, z; };

typedef void (daBtfly_c::*ButterflyState)();

/* SharedFilePtr.h has no fields. LoadFile stashes the file in the second word. */
#define LOADED(h) (((void **)&(h))[1])

/* State2's player-position reload. opt_common_subs is off in that function;
 * folding the cast lets the two reloads CSE. */
#define M(p) (p)
/* State7's flutter-phase add. A plain mFlutterPhase += folds to add #0x300;
 * the int cast keeps the pool-offset add the ROM has. */
#define L(p) ((int)(p))

extern "C" {
void _Z14ApproachLinearRiii(int *p, int target, int step);
void _Z14ApproachLinearRsss(s16 *p, s16 target, s16 step);
s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
s16 Vec3_HorzAngle(const void *a, const void *b);
void Vec3_Sub(Vector3 *out, void *a, void *b);
int Vec3_HorzLen(Vector3 *v);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 ang);
void dBgCh_Actr_UpdateContinuous_Veneer(void *c);
int RandomIntInternal(int *seed);
extern int data_0209e650;
extern s16 data_02082214[]; /* sin/cos; index (u16 angle >> 4) * 2 */
extern Matrix4x3 data_020a0e68; /* scratch matrix */
/* Init LoadFile: ov002 BMD shared with the other small fauna, this overlay's
 * BCA, the still-model BMD, and the animated-model BMD. Cleanup Release. */
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov100_02148600;
extern SharedFilePtr data_ov100_02148668;
extern SharedFilePtr data_ov100_02148608;
extern ButterflyState data_ov100_02148628[];

/* Fix12<int> by value. The header method form homes those arguments on the
 * stack; these scalar entries are the ROM calls. */
void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *c, int oy, int rad, int clip, int far);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *actor, void *shadow, void *mtx, int radius, int height, unsigned int opacity);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *ma, void *bca, int flags, int speed, u32 start);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *w, void *actor, int radius, int height, void *a, void *b);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *c, void *actor, void *pos, int radius, int height, u32 flags, u32 vuln);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *player, Vector3 *pos, unsigned int a, int damage, unsigned int c, unsigned int d, unsigned int e);
}

// @symbol _ZN9daBtfly_cD1Ev
// @symbol _ZN9daBtfly_cD0Ev
/* One ~daBtfly_c() emits both D1 and D0. */
daBtfly_c::~daBtfly_c()
{
}

// @symbol _ZN9daBtfly_c6State7Ev
void daBtfly_c::State7()
{
    char *c = (char *)this;
    int hasContact;
    int v;

    /* 0x3ee does not fit a halfword immediate, and the sign-extend then
     * zero-extend is not the same instruction as ldrh of mFlutterPhase. */
    if (mStateTimer > 0x78) {
        int ang = (short)*(s16 *)(c + 0x3ee);
        ang = (unsigned short)(short)ang;
        int idx = ang >> 4;
        v = (int)(((s64)(int)data_02082214[(idx << 1) + 1] * 0xa3 + 0x800) >> 12);
        if (v > 0) {
            v = (int)(((s64)v * 0x4800 + 0x800) >> 12);
            *(u16 *)L(c + 0x3ee) += 0x2710;
        } else {
            *(u16 *)L(c + 0x3ee) += 0xfa0;
        }
        *(int *)L(c + 0x3e0) += v;
    }

    _Z14ApproachLinearRiii(&mHorzSpeed, 0x14000, 0x1000);

    Player *player = ClosestPlayer();
    if (player != 0) {
        _Z14ApproachLinearRsss(&mPrevAngleY, HorzAngleToCPlayer(), 0x320);
        _Z14ApproachLinearRsss(&mPrevAngleX,
            Vec3_VertAngle((Vector3 *)&mPosX, (Vector3 *)&player->mPosX), 0x320);
    }

    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    {
        int noId = (mdCcAcPos_c.otherOwner == 0);
        hasContact = (noId == 0);
    }
    if (hasContact == 0) {
        if (mWithMeshClsn.IsOnWall() == 0) {
            if (mWithMeshClsn.IsOnGround() == 0) {
                /* mClsnFlags & 0x10. No accessor; IsOnGround is mFlags & 0x10. */
                if (func_02035638((u8 *)&mWithMeshClsn) == 0) {
                    if (mStateTimer <= 0x9d)
                        goto cylinder_only;
                }
            }
        }
    }

    if (hasContact != 0) {
        dActor_c *hit = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
        if (hit != 0) {
            int isPlayer = (hit->actorID == 0xbf);
            if (isPlayer != 0) {
                Vector3 pos;
                pos.x = mPosX;
                pos.y = mPosY;
                pos.z = mPosZ;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(hit, &pos, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    TriplePoofDust();
    MarkForDestruction();

cylinder_only:
    mdCcAcPos_c.Clear();
    {
        Vector3 off;
        off.x = 0;
        off.y = -0x32000;
        off.z = 0;
        mdCcAcPos_c.SetPosRelativeToActor(off);
    }
    mdCcAcPos_c.Update();
}

// @symbol _ZN9daBtfly_c6State6Ev
void daBtfly_c::State6()
{
    if (mStateTimer == 0x14) {
        if (mKind != 1) {
            mScale = 0;
            mUseAnimModel = 0;
            return;
        }
        dActor_c::Spawn(0x114, 0, *(Vector3 *)&mPosX, 0, mAreaId, -1);
        MarkForDestruction();
        return;
    }
    if (mStateTimer <= 0x14)
        return;
    mScale += 0x40;
    if (mScale < 0x800)
        return;
    _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(this, 0x32000, 0x32000, 0x1000000, 0x320000);
    mFlutterPhase = 0;
    mStateTimer = 0;
    mState = 7;
}

// @symbol _ZN9daBtfly_c6State5Ev
void daBtfly_c::State5()
{
    s16 hAngle;

    if (!IsPlayerInRange(0x5dc)) {
        MarkForDestruction();
        return;
    }

    if (mStateTimer > 0x6e && DistToCPlayer() < 0xc8000 &&
        (unsigned char)(mKind + 0xff) <= 1) {
        mHorzSpeed = 0;
        mStateTimer = 0;
        mState = 6;
        mFlags &= ~0x10000;
    } else {
        _Z14ApproachLinearRiii(&mHorzSpeed, 0x8000, 0x800);
    }

    if (mStateTimer >= 0x3c)
        hAngle = mWanderAngle;
    else
        hAngle = Vec3_HorzAngle(&mPosX, &mHomePosX);

    {
        s16 target;
        if (mPosY < mHomePosY +
                (int)(((unsigned)RandomIntInternal(&data_0209e650) >> 16 & 0xfff) * 0x32 + 0x32000))
            target = -0x2000;
        else
            target = 0x2000;
        _Z14ApproachLinearRsss(&mPrevAngleX, target, 0x190);
    }

    _Z14ApproachLinearRsss(&mPrevAngleY, hAngle,
        (s16)((((unsigned)RandomIntInternal(&data_0209e650) >> 16) % 800) + 0x190));
}

// @symbol _ZN9daBtfly_c6State4Ev
void daBtfly_c::State4()
{
    int typ = param1 & 0xff;
    if ((typ & 0xc0) != 0) {
        mKind = (u8)(typ >> 6);
    } else {
        if (DistToCPlayer() >= 0xc8000)
            return;
        {
            int sb = (int)((unsigned int)RandomIntInternal(&data_0209e650) % 3);
            int sel;
            int i;
            int mask = typ & 0x30;
            struct AngleTriple { u16 w[3]; };
            AngleTriple rot;
            int two = 2;
            int three = 3;
            i = 1;
            rot = *(AngleTriple *)&mPrevAngleX;
            for (; i < 3; i++) {
                if (i == sb)
                    sel = 1;
                else if (mask == 0x20)
                    sel = two;
                else
                    sel = three;
                rot.w[1] = (s16)((s16)rot.w[1] + (s16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10));
                dActor_c::Spawn(0x150, mask | (sel << 6), *(Vector3 *)&mPosX,
                    (Vector3_16 *)&rot, mAreaId, -1);
            }
            if (sb == 0)
                mKind = 1;
            else if (mask == 0x20)
                mKind = 2;
            else
                mKind = 3;
        }
    }

    mWanderAngle = (s16)((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10);
    mPrevAngleY = (s16)(mWanderAngle + (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) & 0x3fff));
    mHorzSpeed = (int)(((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) & 0xfff) * 0xf + 0xf000;
    mStateTimer = 0;
    mState = 5;
}

// @symbol _ZN9daBtfly_c6State3Ev
void daBtfly_c::State3()
{
    _Z14ApproachLinearRsss(&mPrevAngleY, Vec3_HorzAngle(&mPosX, &mHomePosX), 0x800);
    _Z14ApproachLinearRsss(&mPrevAngleX, Vec3_VertAngle((Vector3 *)&mPosX, (Vector3 *)&mHomePosX), 0x50);
    UpdatePos(0);

    {
        int *p = &mPosY;
        *p = *p - ((int)(((long long)mHorzSpeed
            * data_02082214[((unsigned short)mPrevAngleX >> 4) * 2] + 0x800) >> 12)
            + (short)data_02082214[
            ((unsigned short)(short)((mStateTimer << 16) / 100) >> 4) * 2 + 1]
            * (short)20 / 4);
    }

    mStateTimer = mStateTimer + 1;
    if (mStateTimer > 100)
        mStateTimer = 0;

    if (IsPlayerInRange(0xbb8))
        return;

    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;
    mState = 1;
}

#pragma opt_common_subs off
// @symbol _ZN9daBtfly_c6State2Ev
void daBtfly_c::State2()
{
    char *player;
    Vector3 v;
    Vector3 d;

    mPosX = mHomePosX;
    mPosY = mHomePosY;
    mPosZ = mHomePosZ;

    player = (char *)ClosestPlayer();
    if (player != 0) {
        int *pp;
        int k = 0x5000;
        int py;
        int t;
        int five = 5;
        Vec3_Sub(&d, &mPosX, player + 0x5c);
        v.x = d.x;
        v.y = d.y;
        v.z = d.z;
        if (Vec3_HorzLen(&v) > 0x4b0000)
            mState = 3;

        pp = (int *)(int)M(player + 0x5c);
        mPosX = mPrevPosX;
        mPosY = mPrevPosY;
        mPosZ = mPrevPosZ;

        v.x = pp[0];
        v.y = pp[1];
        v.z = pp[2];
        t = mStateTimer * k;
        v.x = v.x + t / 4;
        t = mStateTimer * k;
        v.z = v.z + t / 4;
        _Z14ApproachLinearRsss(&mPrevAngleY, Vec3_HorzAngle(&mPosX, &v), 0x300);

        pp = (int *)(int)M(player + 0x5c);
        v.x = pp[0];
        py = pp[1];
        v.y = py;
        v.z = pp[2];
        v.y = py + ((mStateTimer * five + 0x100) << 12) / 4;
        _Z14ApproachLinearRsss(&mPrevAngleX, Vec3_VertAngle((Vector3 *)&mPosX, &v), 0x500);

        UpdatePos(0);

        {
            int *p = &mPosY;
            *p = *p - ((int)(((long long)mHorzSpeed
                * data_02082214[((unsigned short)mPrevAngleX >> 4) * 2] + 0x800) >> 12)
                + (short)data_02082214[
                ((unsigned short)(short)((mStateTimer << 16) / 100) >> 4) * 2 + 1]
                * (short)20 / 4);
            int *cnt = &mStateTimer;
            *cnt = *cnt + 1;
            if (mStateTimer > 100)
                mStateTimer = 0;
        }
        return;
    }
    mPosX = mPrevPosX;
    mPosY = mPrevPosY;
    mPosZ = mPrevPosZ;
}

#pragma opt_common_subs on

// @symbol _ZN9daBtfly_c6State1Ev
void daBtfly_c::State1()
{
    if (IsPlayerInRange(0x3e8) == 0)
        return;
    mState = 2;
    Player *p = ClosestPlayer();
    mPrevAngleY = Vec3_HorzAngle(&mPosX, &p->mPosX);
}

// @symbol _ZN9daBtfly_c6State0Ev
void daBtfly_c::State0()
{
    struct AngleTriple { u16 w[3]; };
    AngleTriple rot;
    int n;
    int i;
    n = (param1 & 0xf) - 1;
    rot = *(AngleTriple *)&mPrevAngleX;
    i = 0;
    if (n > 0) {
        do {
            Vector3 pos;
            int r;
            r = RandomIntInternal(&data_0209e650);
            pos.x = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + mPosX;
            r = RandomIntInternal(&data_0209e650);
            pos.y = mPosY + ((int)((unsigned int)r % 40) << 13);
            r = RandomIntInternal(&data_0209e650);
            pos.z = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + mPosZ;
            r = RandomIntInternal(&data_0209e650);
            rot.w[1] = (s16)((s16)rot.w[1] + ((r << 1) >> 16));
            dActor_c::Spawn(0x150, 0, pos, (Vector3_16 *)&rot, mAreaId, -1);
            i++;
        } while (i < n);
    }
    mState = 1;
}

// @symbol _ZN9daBtfly_c16CleanupResourcesEv
int daBtfly_c::CleanupResources()
{
    data_ov100_02148608.Release();
    data_ov100_02148600.Release();
    data_ov002_0210d9d8.Release();
    data_ov100_02148668.Release();
    return 1;
}

// @symbol _ZN9daBtfly_c16OnPendingDestroyEv
void daBtfly_c::OnPendingDestroy()
{
}

// @symbol _ZN9daBtfly_c6RenderEv
int daBtfly_c::Render()
{
    if (mState == 4)
        return 1;
    if (mUseAnimModel != 0)
        mModelAnim.Render(0);
    else
        mModel.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN9daBtfly_c8BehaviorEv
int daBtfly_c::Behavior()
{
    (this->*data_ov100_02148628[mState])();

    if (mKind != 0) {
        int spd = mScale;
        mScaleX = spd;
        mScaleY = spd;
        mScaleZ = spd;

        {
            int s = mHorzSpeed;
            int idx = ((unsigned short)mPrevAngleX >> 4) << 1;
            long long p;
            p = (long long)(-(int)data_02082214[idx]) * s;
            mVertSpeed = (int)((p + 0x800) >> 0xc);
            p = (long long)(int)data_02082214[idx + 1] * s;
            int horiz = (int)((p + 0x800) >> 0xc);
            {
                int idx2 = ((unsigned short)mPrevAngleY >> 4) << 1;
                p = (long long)horiz * (int)data_02082214[idx2];
                unk_0a4 = (int)((p + 0x800) >> 0xc);
            }
            {
                int idx3 = (((unsigned short)mPrevAngleY >> 4) << 1) + 1;
                p = (long long)horiz * (int)data_02082214[idx3];
                unk_0ac = (int)((p + 0x800) >> 0xc);
            }
        }
        UpdatePosWithOnlySpeed(0);
        mStateTimer++;
    }

    if (mState != 4) {
        int t[3];
        Vec3_Asr(t, &mPosX, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
        mAngleY = mPrevAngleY;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
        if (mUseAnimModel != 0) {
            *(Mtx *)&mModelAnim.mat4x3 = *(Mtx *)&data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                this, &mShadowModel1, &mModelAnim.mat4x3, 0x14000, 0x12c000, 0xf);
            mModelAnim.Advance();
        } else {
            *(Mtx *)&mModel.mat4x3 = *(Mtx *)&data_020a0e68;
            _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                this, &mShadowModel2, &mModel.mat4x3, 0x64000, 0x12c000, 0xf);
        }
    }
    return 1;
}

// @symbol _ZN9daBtfly_c13InitResourcesEv
int daBtfly_c::InitResources()
{
    Model::LoadFile(data_ov002_0210d9d8);
    Animation::LoadFile(data_ov100_02148600);
    Model::LoadFile(data_ov100_02148668);
    void *bmd = Model::LoadFile(data_ov100_02148608);

    if (mModelAnim.SetFile((BMD_File *)bmd, 1, 1) == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, LOADED(data_ov100_02148600), 0, 0x1000, 0);
    if (mShadowModel1.InitCylinder() == 0)
        return 0;
    if (mModel.SetFile((BMD_File *)LOADED(data_ov100_02148668), 1, 1) == 0)
        return 0;
    if (mShadowModel2.InitCylinder() == 0)
        return 0;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);

    Vec3 v;
    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &v, 0x32000, 0x64000, 0x200000, 0);

    int sub = (int)(u8)(param1 & 0x30);
    if (sub != 0x10 && sub != 0x20) {
        mKind = 0;
        mHorzSpeed = 0x7800;
        mStateTimer = ((u32)RandomIntInternal(&data_0209e650) >> 16) % 100;
        if ((u32)(u8)(param1 & 0xf) > 1)
            mState = 0;
        else
            mState = 1;
    } else {
        mState = 4;
    }

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;

    int r = RandomIntInternal(&data_0209e650);
    int fc = mModelAnim.GetFrameCount();
    u32 rem = (u32)r % (u32)fc;
    mModelAnim.currFrame = (rem << 16) >> 4;

    mScale = 0x1000;
    s32 spd = mScale;
    mScaleX = spd;
    mScaleY = spd;
    mScaleZ = spd;
    mUseAnimModel = 1;
    return 1;
}
