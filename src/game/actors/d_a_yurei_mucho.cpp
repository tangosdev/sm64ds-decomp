//cpp
/* daYurei_Mucho_c -- Snufit, the masked Snifit that drifts around Hazy Maze
 * Cave and shoots at you (registry name YUREI_MUCHO, ov065).
 *
 * Class identity, layout evidence and the matching experiments behind the
 * spellings below are recorded in
 * notes/agents/handoffs/daYurei_Mucho_c-ov065.md.
 *
 * func_ov065_02115f84..0211696c are this TU's own state and helper methods.
 * The address is kept as the method name. symbols.txt records the mangled
 * spelling, which is what the State records at the end of this file name.
 *
 * Remaining reconstruction under issue #2478:
 * - mStateTimer comparisons read it through a (u16) cast, because the ROM
 *   loads it with ldrh.
 * - The Mtx43 / V3A overlays on mAnmModel.mat4x3, mShadowMat and the
 *   player position copies. Plain Matrix4x3 / Vector3 assignment
 *   scalarizes.
 * - dExtAnmModel_c::SetAnim, dCcAc_c::Init, dBgCh_Actr::Init,
 *   DropShadowRadHeight, KillByInvincibleChar, SpawnCoins, Player::Hurt
 *   and Player::Bounce retain scalar ABI declarations. Native fixed-point
 *   call reconstruction remains separate work; the handoff records the
 *   measured Init alternatives. Hurt follows its current scalar definition.
 * - pad_3e2 is unobserved.
 */

#include "daYurei_Mucho_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "SpawnInfo.h"

/* POD views preserve the retail aggregate copies. Matrix4x3 and Vector3 are
   available, but their non-POD copies change these two helpers' codegen; see
   the measured alternatives in the handoff. */
typedef struct Mtx43 { int w[12]; } Mtx43;
struct V3A { int w[3]; };

/* func_ov065_02116364 builds a five-Vector3 aggregate on the stack. Leaving it
   an unnamed local class works, but mwccarm then mangles its implicit
   destructor with a file-and-counter tag (_ZN29@class$NNNd_a_yurei_mucho_cppD1Ev)
   that moves whenever this file does, so the manifest row licensing it would
   not survive the move out of src_tu/. Naming it at file scope pins the
   symbol; the emitted bytes are unchanged. */
struct V3Quint {
    Vector3 pp;
    Vector3 spv;
    Vector3 sout;
    Vector3 d;
    Vector3 tgt;
};

/* Scalar ABI declarations retained by this TU. */
extern "C" {

/* arm9 scalar ABI and unnamed helpers; see the remaining work above. */
extern unsigned int RandomIntInternal(void *seed);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void ApproachAngle(void *p, short target, int a, int b, int limit);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern short Vec3_VertAngle(const void *a, const void *b);
extern int Vec3_Dist(const void *a, const void *b);
extern void Vec3_Asr(void *d, const void *s, int sh);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ax);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(const void *in, const void *m, void *out);
extern void MulMat4x3Mat4x3(const int *a, const int *b, int *dst);
extern int func_02012694(int id, void *v);
extern int data_020a0e68[];
extern int data_0209e650[];
extern short data_02082214[];

/* ov002 -- unnamed particle helper. */
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
}

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daYurei_Mucho_c_AssetId {
    ASSET_YUREI_MUCHO_BMD = 0x042d,
    ASSET_YUREI_MUCHO_ATTACK_BCA = 0x042e,
    ASSET_YUREI_MUCHO_BALL_BMD = 0x042f,
    ASSET_YUREI_MUCHO_WAIT_BCA = 0x0430
};

/* The handles reach their files through these wrappers' constructor and
 * destructor veneers; the manifest maps each to the retail destination.
 * Descriptive local types, not recovered spellings. */
struct YureiMuchoModelFile : SharedFilePtr {
    YureiMuchoModelFile(u32 fileID);
    ~YureiMuchoModelFile();
};

struct YureiMuchoAnimFile : SharedFilePtr {
    YureiMuchoAnimFile(u32 fileID);
    ~YureiMuchoAnimFile();
};

/* This TU's four file handles, in the order the retail initializer
 * (0x0211c110) builds them. d_a_propeller_heyho_fire.cpp also loads the
 * shot model through an extern, so that handle keeps its address name. */
YureiMuchoModelFile data_ov065_0211d610(ASSET_YUREI_MUCHO_BALL_BMD);
YureiMuchoModelFile gYureiMuchoModelFile(ASSET_YUREI_MUCHO_BMD);
YureiMuchoAnimFile gYureiMuchoWaitAnimFile(ASSET_YUREI_MUCHO_WAIT_BCA);
YureiMuchoAnimFile gYureiMuchoAttackAnimFile(ASSET_YUREI_MUCHO_ATTACK_BCA);

/* The four State records, defined at the end of this file. */
extern daYurei_Mucho_c::State data_ov065_0211d670;
extern daYurei_Mucho_c::State data_ov065_0211d680;
extern daYurei_Mucho_c::State data_ov065_0211d650;
extern daYurei_Mucho_c::State data_ov065_0211d660;

/* -------------------------------------------------------------------------- */
// @symbol daYurei_Mucho_c_classInit
/* The registry factory behind the YUREI_MUCHO / SNUFIT profile. `return new
   daYurei_Mucho_c()` MATCHES (size 0x50); the synthesized ctor stores
   `_ZTV15daYurei_Mucho_c + 2`. classInit is a reconstructed source-style
   name, not a preserved identifier. */
extern "C" daYurei_Mucho_c *daYurei_Mucho_c_classInit(void)
{
    return new daYurei_Mucho_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13OnYoshiTryEatEv
/* dActor_c vtable slot 18. */
s32 daYurei_Mucho_c::OnYoshiTryEat()
{
    return 4;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13OnTurnIntoEggER6Player
/* dActor_c vtable slot 19, confirmed by address: _ZTV15daYurei_Mucho_c
 * (0x0211cba4) + 0x4c -> 0x02116f14.
 *
 * The `R6Player` reference spelling in the mangled name is a coined guess: a
 * reference and a pointer generate identical ARM for this body, so the bytes
 * cannot distinguish them. */
void daYurei_Mucho_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, (unsigned char)(mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16OnAimedAtWithEggEv
/* dActor_c vtable slot 29. */
s32 daYurei_Mucho_c::OnAimedAtWithEgg()
{
    return 0;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c13InitResourcesEv
/* Both Init methods are declared. The current dCcAc_c fixed-point aggregate
   arguments add 16 bytes here; dBgCh_Actr's scalar declaration emits a name
   absent from symbols.txt. Retain these ABI calls pending interface repair. */
int daYurei_Mucho_c::InitResources()
{
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gYureiMuchoModelFile), 1, -1);
    dExtSimpleModel_c::LoadFile(data_ov065_0211d610);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(gYureiMuchoWaitAnimFile);
    dExtFrameCtrl_c::LoadFile(gYureiMuchoAttackAnimFile);
    mTerminalVelocity = -0x1e000;
    mdCcAc_c.Init(this, 0x38000, 0x7e000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    mWithMeshClsn.Init(this, 0x46000, 0, 0, 0);
    mCoinType = 1;
    mExtraCoinCount = 1;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mAnmModel.speed = 0x1000;
    func_ov065_0211691c(&data_ov065_0211d670);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c8BehaviorEv
/* dActor_c vtable slot 6. */
int daYurei_Mucho_c::Behavior()
{
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (mSpitOutTimer == 0) {
                mdCcAc_c.Update();
            }
        }
        func_ov065_0211696c();
        mHomePosX = mPosX;
        mHomePosY = mPosY;
        mHomePosZ = mPosZ;
        func_ov065_0211691c(&data_ov065_0211d670);
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        ApproachAngle(&mAngleX, -0x4000, 0xa, 0x200, 0x100);
        UpdateDeath(mWithMeshClsn);
        func_ov065_0211696c();
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        State *q = mCurrentState;
        /* Reads the handler's pointer word directly rather than as `&q->mMain`:
           taking the ADDRESS of a pointer-to-member makes mwcc materialise the
           whole 8-byte pmf. Reading one to CALL it is free. */
        if (*(int *)((char *)q + 8) != 0) {
            (this->*(q->mMain))();
        }
    }
    {
        /* Gravity, clamped at terminal velocity. mSpeedZ is read and written
           back unchanged -- the ROM really does reload and restore it here. */
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped) {
            clamped = fallSpeed;
        }
        int keep = mSpeedZ;
        mVertSpeed = clamped;
        mSpeedZ = keep;
    }
    if (mCurrentState != &data_ov065_0211d650) {
        int ang;
        int idx;
        short tbl;
        int result;
        mBobAngle += 0x200;
        ang = mBobAngle;
        /* The shift must be LOGICAL so the angle wraps -- writing it on the
           signed s16 would read the wrong table entry for negative angles. */
        idx = ((unsigned short)(short)ang >> 4) * 2;
        tbl = data_02082214[idx];
        result = FX_MUL(tbl, 0x46000);
        ApproachLinear(mPosY, mHomePosY + (result + 0xb4000), 0x3000);
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    func_ov065_0211696c();
    if (mCurrentState != &data_ov065_0211d660) {
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        func_ov065_02115ff0();
    }
    mdCcAc_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }
    mAnmModel.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c6RenderEv
/* dActor_c vtable slot 9. */
int daYurei_Mucho_c::Render()
{
    int b = ((mFlags & 0x40000) != 0);
    if (b) return 1;
    mAnmModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daYurei_Mucho_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c16CleanupResourcesEv
/* dActor_c vtable slot 3. Releases the four files InitResources claimed.
 *
 * The body does not read the incoming object pointer or access instance
 * fields. Spelling it as the class method preserves the ROM bytes. */
int daYurei_Mucho_c::CleanupResources()
{
    data_ov065_0211d610.Release();
    gYureiMuchoModelFile.Release();
    gYureiMuchoWaitAnimFile.Release();
    gYureiMuchoAttackAnimFile.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_0211696cEv
/* Rebuilds the model matrix from the actor's position and Z/X/Y angles, then
   drops the shadow. Called three times from Behavior. */
void daYurei_Mucho_c::func_ov065_0211696c()
{
    Vector3 v;

    Vec3_Asr(&v, &mPosX, 3);
    Matrix4x3_FromTranslation(data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(data_020a0e68,
        mAngleX, mAngleY, mAngleZ);

    *(Mtx43 *)&mAnmModel.mat4x3 = *(Mtx43 *)data_020a0e68;
    mShotPosX = 0;
    mShotPosY = 0;
    mShotPosZ = 0;
    *(Mtx43 *)data_020a0e68 = *(Mtx43 *)&mAnmModel.mat4x3;

    MulMat4x3Mat4x3(reinterpret_cast<const int *>(&mAnmModel.data.transforms[4]), data_020a0e68,
                    data_020a0e68);

    mShotPosX = data_020a0e68[9];
    mShotPosY = data_020a0e68[10];
    mShotPosZ = data_020a0e68[11];
    mShotPosX <<= 3;
    mShotPosY <<= 3;
    mShotPosY -= 0xa000;
    mShotPosZ <<= 3;

    Matrix4x3_FromTranslation(data_020a0e68,
        mPosX >> 3,
        (mPosY - 0x18000) >> 3,
        mPosZ >> 3);

    *(Mtx43 *)mShadowMat = *(Mtx43 *)data_020a0e68;

    DropShadowRadHeight(mShadowModel, *(Matrix4x3 *)mShadowMat, 0x64000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_0211691cEPNS_5StateE
/* The state setter. Stores the State into mCurrentState, RE-READS it -- the ROM
   really does reload the field it has just written -- null-tests the entry hook
   at +0x00 and calls it through the object. Called eight times inside this run.

   The pointer-to-member call is the ROM's own `ldr r2,[r3]; cmp r2,#0;
   ldr r1,[r3,#4]; add r0,r0,r1,asr #1; ands r1,r1,#1; ldrne r1,[r0];
   ldrne r1,[r1,r2]; ldreq r1,[r3]; blx r1` sequence, i.e. the Itanium
   {ptr_or_vtable_offset, adj*2|isVirtual} encoding. All eight records in
   ov065's .data carry adj word 0: non-virtual, no this-adjustment. */
int daYurei_Mucho_c::func_ov065_0211691c(State *s)
{
    mCurrentState = s;
    State *q = mCurrentState;
    if (q->mEnter == 0) {
        return 1;
    }
    return (this->*(q->mEnter))();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_021168a8Ev
/* State entry hook at 0x0211d670 +0x00: randomise the facing angle and the
   timer, then start the wait animation. The BCA file is read straight out of
   the SharedFilePtr's filePtr -- the ROM does `ldr r1,[r0,#4]` off the
   literal at 0x02116918, not a call, so the field read is what reproduces. */
int daYurei_Mucho_c::func_ov065_021168a8()
{
    mTargetAngle = (short)((RandomIntInternal(data_0209e650) >> 8) << 0xc);
    mStateTimer = (short)(((RandomIntInternal(data_0209e650) >> 8) & 0x1f) + 0x32);
    mAnmModel.SetAnim((BCA_File *)gYureiMuchoWaitAnimFile.filePtr, 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_02116744Ev
/* State main hook at 0x0211d670 +0x08: drift, and switch to the attack state
   when a player comes close enough. */
int daYurei_Mucho_c::func_ov065_02116744()
{
    Vector3 in;
    Vector3 dp;
    Player *pl;
    int *pos;

    in.x = 0;
    in.y = 0;
    in.z = 0;

    if (Vec3_Dist(&mPosX, &mHomePosX) > 0x1f4000 ||
        mWithMeshClsn.IsOnWall() != 0) {
        mTargetAngle = Vec3_HorzAngle(&mPosX, &mHomePosX);
        if ((u16)mStateTimer < 0x14)
            mStateTimer = 0x14;
    }

    ApproachAngle(&mPrevAngleY, mTargetAngle, 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngleX, 0, 1, 0x500, 0x500);

    in.z = 0xa000;
    Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
    MulVec3Mat4x3(&in, data_020a0e68, &mSpeedX);

    if ((u16)mStateTimer == 0) {
        func_ov065_0211691c(&data_ov065_0211d670);
        return 1;
    }

    pl = ClosestNonVanishPlayer();
    if (pl != 0) {
        pos = (int *)&pl->mPosX;
        dp.x = pos[0];
        dp.y = pos[1];
        dp.z = pos[2];

        if (Vec3_Dist(&mPosX, &dp) < 0x3e8000) {
            mStateTimer = 0x14;
            func_ov065_0211691c(&data_ov065_0211d680);
        }
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_0211672cEv
/* State entry hook at 0x0211d680 +0x00: stop the actor dead. */
int daYurei_Mucho_c::func_ov065_0211672c()
{
    mSpeedX = 0;
    mVertSpeed = 0;
    mSpeedZ = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_021165d8Ev
/* State main hook at 0x0211d680 +0x08: steer toward the nearest non-vanishing
   player.

   dActor_c::ClosestNonVanishPlayer takes `this` in r0; the ROM emits no `mov`
   before the `bl` at 0x021165e8 because r0 still holds the incoming object. */
int daYurei_Mucho_c::func_ov065_021165d8()
{
    short pitch = 0;
    Player *p = ClosestNonVanishPlayer();
    if (p != 0) {
        Vector3 tmp;
        *(V3A *)&tmp = *(V3A *)&p->mPosX;
        Vector3 v;
        Vector3 a;
        a.x = tmp.x;
        a.y = tmp.y;
        a.z = tmp.z;
        mTargetAngle = Vec3_HorzAngle(&mPosX, &a);
        Vector3 b;
        b.x = tmp.x;
        b.y = tmp.y;
        b.z = tmp.z;
        pitch = Vec3_VertAngle(&mPosX, &b);
        if (Vec3_Dist(&mPosX, &tmp) >= 0x1f4000) {
            mSpeedX = 0;
            mVertSpeed = 0;
            mSpeedZ = 0;
        } else {
            v.z = 0;
            v.z = -0x1000;
            v.x = 0;
            v.y = 0;
            Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
            MulVec3Mat4x3(&v, data_020a0e68, &mSpeedX);
        }
    } else {
        mStateTimer = pitch;
    }
    ApproachAngle(&mPrevAngleY, mTargetAngle, 1, 0x500, 0x500);
    ApproachAngle(&mPrevAngleX, pitch, 1, 0x500, 0x500);
    if ((u16)mStateTimer == 0)
        func_ov065_0211691c(&data_ov065_0211d650);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_02116588Ev
/* State entry hook at 0x0211d650 +0x00: start the attack animation. As at
   ordinal 11, the BCA file is the SharedFilePtr's filePtr, read directly
   (`ldr r1,[r0,#4]` at 0x021165a4 off the literal at 0x021165d4). */
short daYurei_Mucho_c::func_ov065_02116588()
{
    mShotCount = 0;
    mAnmModel.SetAnim((BCA_File *)gYureiMuchoAttackAnimFile.filePtr, 0x40000000, 0x1000, 0);
    mStateTimer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_02116364Ev
/* State main hook at 0x0211d650 +0x08: aim, and spawn the projectile (profile
   0xe9) up to three times before returning to a wait state. */
int daYurei_Mucho_c::func_ov065_02116364()
{
    V3Quint L;
    Player *pl = ClosestNonVanishPlayer();
    if (pl != 0) {
        *(V3A *)(int)(&L.pp) = *(V3A *)(int)(&pl->mPosX);
        L.tgt = L.pp;
        mTargetAngle = Vec3_HorzAngle(&mPosX, &L.tgt);
        ApproachAngle(&mPrevAngleY, mTargetAngle, 1, 0x500, 0x500);

        if ((((u32)mAnmModel.currFrame << 4) >> 16) >= 0xf && (u16)mStateTimer == 0 &&
            mShotCount < 3) {
            void *spawned = dActor_c::Spawn(0xe9, 1, *(Vector3 *)&mShotPosX, 0, mAreaId, -1);
            if (spawned != 0) {
                dActor_c *shot = (dActor_c *)spawned;
                func_02012694(0xfb, &mCamSpacePosX);
                L.spv.x = 0;
                L.spv.y = 0;
                L.spv.z = 0x1e000;
                L.sout.x = 0;
                L.sout.y = 0;
                L.sout.z = 0;
                Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mAngleX);
                MulVec3Mat4x3(&L.spv, &data_020a0e68, &L.sout);
                shot->mSpeedX = L.sout.x;
                shot->mVertSpeed = L.sout.y;
                shot->mSpeedZ = L.sout.z;
                mShotCount += 1;
                mStateTimer = 4;
            }
        }
    }

    if (mAnmModel.Finished() != 0) {
        if (pl != 0) {
            s32 *dsrc = (s32 *)(int)(&pl->mPosX);
            L.d.x = dsrc[0];
            L.d.y = dsrc[1];
            L.d.z = dsrc[2];
            if (Vec3_Dist(&mPosX, &L.d) > 0x3e8000) {
                mStateTimer = 0;
                func_ov065_0211691c(&data_ov065_0211d670);
            } else {
                mStateTimer = 0x32;
                mAnmModel.SetAnim((BCA_File *)gYureiMuchoWaitAnimFile.filePtr, 0, 0x1000, 0);
                func_ov065_0211691c(&data_ov065_0211d680);
            }
        } else {
            mStateTimer = 0;
            func_ov065_0211691c(&data_ov065_0211d670);
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_02116328Ev
/* State entry hook at 0x0211d660 +0x00: the bumped-from-below pop. */
int daYurei_Mucho_c::func_ov065_02116328()
{
    mSpeedX = 0;
    mVertSpeed = 0;
    mSpeedZ = 0;
    mVertSpeed = 0x32000;
    mVertAccel = -0x5000;
    mStateTimer = 0xa;
    mFlags = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_021162c0Ev
/* State main hook at 0x0211d660 +0x08.
 *
 * CORRECTED HERE. The retired shard func_ov065_021162c0.c called `_ZN6EyerokD0Ev`, which is
 * ov066's name for 0x02115f84. This module's own name for that address is
 * func_ov065_02115f84, the death helper two ordinals below, and ov065 is the
 * module this branch links in. match.py wildcards every relocated word, so the
 * wrong callee still reproduced the bytes; the ROM's own `bl 0x02115f84` at
 * 0x02116314 decides it. */
int daYurei_Mucho_c::func_ov065_021162c0()
{
    ApproachAngle(&mAngleX, -0x4000, 0xa, 0x200, 0x100);
    ApproachLinear(mAngleX, -0x4000, 0x200);
    if ((u16)mStateTimer == 0)
        func_ov065_02115f84();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_02115ff0Ev
/* The collision response, called once from Behavior. */
void daYurei_Mucho_c::func_ov065_02115ff0()
{
    Vector3_16 v;
    Vector3 hv;
    Player *p;
    int flags;
    unsigned int id;

    id = mdCcAc_c.otherOwner;
    if (id == 0) return;
    p = (Player *)dActor_c::FindWithID(id);
    if (p == 0) return;
    flags = (int)mdCcAc_c.hitFlags;

    if (flags & 0x40000) {
        mDeathState = 4;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x11e, &mCamSpacePosX);
        return;
    }
    if (flags & 0x2400) {
        mDeathState = 2;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x11e, &mCamSpacePosX);
        return;
    }
    if (flags & 0x4380) {
        mDeathState = 3;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x11e, &mCamSpacePosX);
        return;
    }
    if (BumpedUnderneathByPlayer(*p) == 1) {
        func_02012694(0x11e, &mCamSpacePosX);
        func_ov065_0211691c(&data_ov065_0211d660);
        return;
    }
    if (flags & 0x10) {
        v.x = -0x2000;
        v.y = 0;
        v.z = 0;
        KillByInvincibleChar(v, *p, 0);
        func_02012694(0x11e, &mCamSpacePosX);
        return;
    }
    if (flags & 0x40) {
        mDeathState = 2;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x11e, &mCamSpacePosX);
        return;
    }
    if (flags & 0x20) {
        mDeathState = 1;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x11e, &mCamSpacePosX);
        return;
    }
    {
        int b = (int)(((long long)(p->actorID == 0xbf)));
        if (b == 0) return;
    }
    if (p->mIsMetal == 1 || p->IsOnShell() == 1) {
        func_ov065_02115f84();
        return;
    }
    if (JumpedOnByPlayer(mdCcAc_c, *p)) {
        p->Bounce(0x28000);
        mDeathState = 1;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x11e, &mCamSpacePosX);
        return;
    }
    hv.x = mPosX;
    hv.y = mPosY;
    hv.z = mPosZ;
    p->Hurt(hv, 2, 0xc000, 1, 0, 1);
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_c19func_ov065_02115f84Ev
/* The death helper: poof, drop the coins, unregister. */
int daYurei_Mucho_c::func_ov065_02115f84()
{
    Vector3 v;
    SmallPoofDust();
    v.x = mPosX;
    v.y = mPosY;
    v.z = mPosZ;
    SpawnCoins(v, mExtraCoinCount + 1, 0xa000, 0);
    KillAndTrackInDeathTable();
    return func_02012694(0x11e, &mCamSpacePosX);
}


/* -------------------------------------------------------------------------- */
// @symbol _ZN15daYurei_Mucho_cD1Ev
// @symbol _ZN15daYurei_Mucho_cD0Ev
/* No separate body lives here. The inline virtual destructor in the directly
 * included class header makes mwccarm emit retail's D1 then D0 pair without
 * the otherwise homeless D2 variant an out-of-line definition produces.
 *
 * D1 stores the vptr, then destroys the dExtShadowModel_c at 0x364, the dExtAnmModel_c at
 * 0x300, the dBgCh_Actr at 0x144 and the dCcAc_c at 0x110 in reverse
 * declaration order, and tails into ov002 _ZN12dEnemyBase_cD2Ev. D0 repeats
 * that body verbatim -- it does NOT call D1 -- and then hands the object back
 * to the game heap. All of that is a consequence of the class declaration;
 * none of it is written out. */

/* The four {enter, main} State records, in the order the retail initializer
 * fills them. func_ov065_02116588 returns short, so its pointer is cast to
 * the record's int-returning type. */
typedef int (daYurei_Mucho_c::*YureiMuchoStateFn)();

daYurei_Mucho_c::State data_ov065_0211d670 = {&daYurei_Mucho_c::func_ov065_021168a8,
                                              &daYurei_Mucho_c::func_ov065_02116744};
daYurei_Mucho_c::State data_ov065_0211d680 = {&daYurei_Mucho_c::func_ov065_0211672c,
                                              &daYurei_Mucho_c::func_ov065_021165d8};
daYurei_Mucho_c::State data_ov065_0211d650 = {
    (YureiMuchoStateFn)&daYurei_Mucho_c::func_ov065_02116588,
    &daYurei_Mucho_c::func_ov065_02116364};
daYurei_Mucho_c::State data_ov065_0211d660 = {&daYurei_Mucho_c::func_ov065_02116328,
                                              &daYurei_Mucho_c::func_ov065_021162c0};

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daYurei_Mucho_c> g_profile_YUREI_MUCHO = {
    daYurei_Mucho_c_classInit, /* classInit */
    0x00ec,                    /* behaviorPriority */
    0x005b,                    /* renderPriority */
    0x10000003,                /* actorFlags */
    0x64000,                   /* clipOffsetY */
    0xc8000,                   /* clipRadius */
    0x01000000,                /* clipDistance */
    0x01000000                 /* farDistance */
};
