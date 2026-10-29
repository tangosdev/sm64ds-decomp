//cpp
/* daObjBk_Lift_c -- Whomp's Fortress tower lift (BK_LIFT01).
 *
 * param1 & 0xff picks the behaviour: kind 1 flips yaw every 0x87 frames,
 * kind 2 rides between mMinPosY/mMaxPosY with a looping sound, anything
 * else only refreshes the collider. UpdateShadow re-aims the cuboid shadow
 * every frame.
 *
 * The destructor pair comes off the in-class `~daObjBk_Lift_c() {}` in
 * include/daObjBk_Lift_c.h and lands ahead of everything, D1 then D0
 * (0x02112290, then 0x021122dc). Source order is reverse ROM order
 * (2004/b56 emits .text back to front); factory first, Kill() last.
 *
 * Notes:
 * - func_02039394 / func_020393a4 / func_020393c4 / func_020393d4 are
 *   small stores into dBgW (d_bg_w.cpp, no header).
 * - StoodOnCallback keeps #pragma long_calls -- the ROM veneer is the
 *   pooled ldr ip,[pc,#8]; bx ip absolute tail-call (size 0x14); a near b
 *   to OnStoodOn in this same TU is 0xc.
 * - OnStoodOn takes the rider it ignores: the veneer forwards two
 *   registers after dropping the collider, and a callee taking only
 *   `this` drops mov r1, r2.
 * - data_ov015_02114a5c / data_ov015_02114a64 are this class's KCL and
 *   BMD handles, defined at the file end so mwcc emits the .init.
 *   symbols.txt used to coin MovingBarSmall_ClsnFile /
 *   MovingBarSmall_ModelFile on those addresses -- a stale spelling of
 *   this class's old coined name -- so this TU keeps the address-true
 *   spelling.
 * - data_ov015_02113594 is the CLPS block this TU does not own;
 *   data_02082214 is arm9's sin/cos table; data_0209f220 / data_0209f2f8
 *   are arm9 scene state; g_profile_BK_LIFT01 lives outside this TU.
 * - Behavior kind 1 writes the half-turn through s16 *p = &mPrevAngleY
 *   and a u8 temporary for 0x87: a direct member add if-converts
 *   (ldrsheq / addeq / strheq) and Behavior shrinks from 0x1cc to 0x1c4.
 * - Behavior kind 2 keeps the 0/1 flag, the second compare, and the
 *   equal-arm ternary: direct mPosY < mMinPosY / mPosY > mMaxPosY clamps
 *   compile to 0x1a4 against the ROM's 0x1cc.
 * - Vec3_Dist, DecIfAbove0_Byte and IsStarCollectedInCurLevel stay local
 *   externs; Vec3_Dist's header copies take Vector3 * in unrelated leaf
 *   headers.
 */

/* MEASURED -- MUST STAY AHEAD OF daObjBk_Lift_c.h, which reaches math/Matrix.h
   through dBgW_KcMbg.h. Matrix4x3 has two guarded spellings over the same 0x30
   bytes and whichever a translation unit sees first stands; UpdateShadow
   copies one flat, and with math/Matrix.h's structured spelling that copy
   compiles to 0xb4 against the ROM's 0x9c. Same rule, and the same reason, as
   the common.h include in include/dBgActor_c.h. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjBk_Lift_c_AssetId { ASSET_BK_LIFT01_BMD = 0x0599, ASSET_BK_LIFT01_KCL = 0x059a };

#include "common.h"

#include "daObjBk_Lift_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "Particle__System.h"

/* The constructors and destructors are the ROM resource-family veneers,
 * aliased in the manifest. */
struct BkLiftModelFilePtr : SharedFilePtr {
    BkLiftModelFilePtr(u32 fileID);
    ~BkLiftModelFilePtr();
};

struct BkLiftCollisionFilePtr : SharedFilePtr {
    BkLiftCollisionFilePtr(u32 fileID);
    ~BkLiftCollisionFilePtr();
};

extern BkLiftCollisionFilePtr data_ov015_02114a5c; /* collision KCL */
extern BkLiftModelFilePtr data_ov015_02114a64;     /* step BMD */
extern CLPS_Block data_ov015_02113594;
extern s16 data_02082214[]; /* arm9 sin/cos table */
extern s8 data_0209f2f8;
extern u8 data_0209f220;

/* arm9 C helpers with no shared header. */
extern "C" {
int DecIfAbove0_Byte(u8 *p);
int IsStarCollectedInCurLevel(int starID);
int Vec3_Dist(void *a, void *b);
void func_02039394(void *collider, int v);
void func_020393a4(void *collider, int v);
void func_020393c4(dBgW *collider, void *callback);
void func_020393d4(dBgW *collider, void *callback);
}

// @symbol daObjBk_Lift_c_classInit
/* BK_LIFT01's registry factory. 0x394 is this class's size, and the calls the
   cartridge inlines here -- fBase_c::operator new, dBgActor_c's base
   constructor, the vptr store and dExtShadowModel_c's constructor on the member at
   0x320 -- are exactly what the implicit default constructor of
   `struct daObjBk_Lift_c : dBgActor_c` with a dExtShadowModel_c member generates.
   Same shape as daObjBk_Rotebar_c_classInit in this overlay. */
extern "C" daObjBk_Lift_c *daObjBk_Lift_c_classInit() {
    return new daObjBk_Lift_c();
}

// @symbol _ZN14daObjBk_Lift_c15StoodOnCallbackEP4dBgWP8dActor_cS3_
/* dBgW stood-on callback, armed by func_020393c4 in InitResources. Drops the
   collider and forwards (lift, rider) into OnStoodOn. long_calls is the
   ROM's pooled absolute tail-call. */
#pragma long_calls on
int daObjBk_Lift_c::StoodOnCallback(dBgW *collider, dActor_c *lift, dActor_c *rider) {
    return ((daObjBk_Lift_c *)lift)->OnStoodOn(rider);
}
#pragma long_calls off

// @symbol _ZN14daObjBk_Lift_c9OnStoodOnEP8dActor_c
/* Marks the lift as stood on, which is what restarts a kind-2 ride in
   Behavior. The rider is the veneer's extra forwarded register -- a
   callee taking only `this` drops `mov r1, r2`. */
int daObjBk_Lift_c::OnStoodOn(dActor_c *rider) {
    mJustSteppedOn = 1;
    mMove = 1;
}

// @symbol _ZN14daObjBk_Lift_c13InitResourcesEv
int daObjBk_Lift_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov015_02114a64), 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    ((dBgW_KcMbg *)(&mMeshCollider))
        ->dBgW_KcMbg::SetFile((KCL_File *)dBgW_Kc::LoadFile(data_ov015_02114a5c), *(&mClsnMat),
                              0x199, mAngleY, *(&data_ov015_02113594));
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);
    func_020393c4(&mMeshCollider, (void *)&StoodOnCallback);

    Vector3 probePos;
    probePos.x = mPosX;
    probePos.y = mPosY;
    probePos.z = mPosZ;
    probePos.y -= 0x14000;

    dBgCh_Gnd ground;
    ground.SetObjAndPos(probePos, 0);
    mFloorPosY = probePos.y;
    if (ground.DetectClsn())
        mFloorPosY = ground.clsnY;

    u16 angle = mAngleY;
    int sine = data_02082214[(angle >> 4) * 2];
    mShadowOffsetX = FX_MUL(sine, 0xc8000);
    angle = mAngleY;
    int cosine = data_02082214[(angle >> 4) * 2 + 1];
    mShadowOffsetY = FX_MUL(cosine, 0xc8000);

    UpdateShadow();

    mPrevAngleY = mAngleY + 0x8000;
    mMoveTimer = 0x87;

    int kind = param1 & 0xff;
    if (kind != 2) {
        mHorzSpeed = 0x3000;
    } else {
        mMinPosY = mPosY;
        mMaxPosY = mMinPosY + 0x1f4000;
        mTerminalVelocity = -0x3c000;
        mVertSpeed = 0xa000;
    }

    if (data_0209f2f8 == 7 && (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0) &&
        mPosY >= 0xdac000)
        return 0;

    return 1;
}

// @symbol _ZN14daObjBk_Lift_c8BehaviorEv
int daObjBk_Lift_c::Behavior() {
    int kind = param1 & 0xff;

    if (kind == 1) {
        if (DecIfAbove0_Byte(&mMoveTimer) == 0) {
            s16 *p = &mPrevAngleY;
            s16 v = *p;
            u8 b = 0x87;
            v = (s16)(v + 0x8000);
            *p = v;
            mMoveTimer = b;
        }
        UpdatePos(0);
        UpdateModelPosAndRotY();
        if (IsClsnInRange(0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
    } else if (kind == 2) {
        if (DecIfAbove0_Byte(&mMoveTimer) == 0) {
            if (mMove != 0) {
                int flag;
                int bound;
                int y;

                UpdatePos(0);
                bound = mMinPosY;
                y = mPosY;
                flag = (y >= bound);
                if (flag == 0) {
                    mPosY = (y < bound) ? bound : y;
                    mVertSpeed = 0xa000;
                    if (mJustSteppedOn == 0) {
                        flag = 0;
                        mMove = (u8)flag;
                    }
                } else {
                    bound = mMaxPosY;
                    flag = (y <= bound);
                    if (flag == 0) {
                        mPosY = (y > bound) ? bound : y;
                        mVertSpeed = -0xa000;
                        mMoveTimer = 0x5a;
                    }
                }
            }
        }
        UpdateModelPosAndRotY();
        if (IsClsnInRange(0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
        if (Vec3_Dist(&mPos, &mPrevPos) != 0) {
            unsigned z = 0;
            unsigned r = Sound::PlayLong((unsigned)mSoundID, 3, 0x82, mCamSpacePos, (s16)z);
            mSoundID = (s32)r;
        }
        mJustSteppedOn = 0;
    } else {
        IsClsnInRange(0x150000, 0x1000);
    }

    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);
    UpdateShadow();
    return 1;
}

// @symbol _ZN14daObjBk_Lift_c6RenderEv
int daObjBk_Lift_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjBk_Lift_c16CleanupResourcesEv
int daObjBk_Lift_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_02114a64.Release();
    data_ov015_02114a5c.Release();
    return 1;
}

// @symbol _ZN14daObjBk_Lift_c12UpdateShadowEv
/* Re-aims the cuboid shadow: the model's matrix, then the step's own position
   biased by mShadowOffsetX / mShadowOffsetY and flattened onto mFloorPosY. */
void daObjBk_Lift_c::UpdateShadow() {
    mShadowMat = mModel.mat4x3;
    mShadowMat.m[9] = (mPosX - mShadowOffsetX) >> 3;
    mShadowMat.m[10] = mFloorPosY >> 3;
    mShadowMat.m[11] = (mPosZ - mShadowOffsetY) >> 3;
    DropShadowScaleXYZ(mShadowModel, mShadowMat, 0x190000, 0x28000, 0x258000, 0xf);
}

// @symbol _ZN14daObjBk_Lift_c15OnHitByMegaCharER6Player
/* Slot 27. Player::IncMegaKillCount is a real method, and the trailing
   unqualified Kill() reaches this class's own slot-31 override. */
void daObjBk_Lift_c::OnHitByMegaChar(Player &player) {
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN14daObjBk_Lift_c4KillEv
/* Slot 31, attributed by the vtable: _ZTV14daObjBk_Lift_c (ov015 0x02114650)
   carries 0x0211233c at slot 31 -- config/arm9/overlays/ov015/relocs.txt has
   `from:0x021146cc kind:load to:0x0211233c` -- while _ZTV10dBgActor_c's word at
   the same slot (from:0x0210aeb4) relocates to 0x020ee55c,
   _ZN10dBgActor_c4KillEv.

   Particle 0x48 at the step's own position, no vertical offset, then the poof
   at that same position built from the fields a SECOND time: the ROM reloads
   mPos rather than reusing the words it just passed. */
void daObjBk_Lift_c::Kill() {
    Vector3 dustPos;
    Particle::System::NewSimple(0x48, mPosX, mPosY, mPosZ);
    dustPos.x = mPosX;
    dustPos.y = mPosY;
    dustPos.z = mPosZ;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, mCamSpacePos);
    MarkForDestruction();
}

/* The ROM's initializer constructs the model handle with file ID 1433 and
 * the collision handle with file ID 1434, registering a destructor node for
 * each. mwcc emits __sinit_d_a_obj_bk_lift.cpp from these definitions. */
BkLiftModelFilePtr data_ov015_02114a64(ASSET_BK_LIFT01_BMD);
BkLiftCollisionFilePtr data_ov015_02114a5c(ASSET_BK_LIFT01_KCL);
