//cpp
/**
 * Whomp's Fortress tower lift (registry profile BK_LIFT01).
 *
 * A stepping platform on the tower. `param1 & 0xff` picks the behaviour:
 * kind 1 flips yaw by a half turn every 0x87 frames, kind 2 rides between
 * mMinPosY and mMaxPosY and keeps a looping sound alive while it moves,
 * and anything else only refreshes the collider. The class carries its
 * own cuboid dExtShadowModel_c, and func_ov015_021123c8 is the helper that
 * re-aims it every frame.
 *
 * The class's .text, its .init/.ctor pair and the two resource handles'
 * .bss are owned here. ov015 delinks no .data for this class, so the
 * _ZTV / _ZTI / _ZTS group it names is compiler-only output, compared
 * against the cartridge's own copies at ov015 0x02114650 / 0x0211460c /
 * 0x02114618.
 *
 * SOURCE ORDER IS REVERSE ROM ORDER. mwccarm 2004/b56 emits .text back to
 * front under this tree's flags, so the factory is written first and Kill()
 * last; the destructor pair comes off the in-class `~daObjBk_Lift_c() {}` in
 * include/daObjBk_Lift_c.h and lands ahead of everything, D1 then D0, which
 * is the order the cartridge has (0x02112290, then 0x021122dc).
 *
 * leftovers:
 * - dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange, dActor_c::UpdatePos,
 *   Sound::PlayLong, Particle::System::NewSimple and
 *   dActor_c::DropShadowScaleXYZ stay mangled extern-C free functions:
 *   each takes Fix12<int> by value, and a real method call homes the
 *   argument and size-DIFFs the caller (notes/mwccarm-codegen.md 6az).
 * - func_02039394 / func_020393a4 / func_020393c4 / func_020393d4 are
 *   small stores into dBgW. This TU calls them; naming belongs to dBgW.
 * - func_ov015_021128f8 stays a free function: its first argument is the
 *   collider, not the lift. It keeps #pragma long_calls -- the ROM veneer
 *   is the pooled ldr ip,[pc,#8]; bx ip absolute tail-call (size 0x14);
 *   a near b to func_ov015_021128e8 in this same TU is 0xc.
 * - func_ov015_021128e8 keeps a second unused parameter: the veneer
 *   forwards two registers after dropping the collider, and a 1-arg
 *   callee drops mov r1, r2.
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
 *   externs; Vec3_Dist is called on raw s32 triplets and its header
 *   copies take Vector3 * in unrelated leaf headers.
 */

/* MEASURED -- MUST STAY AHEAD OF daObjBk_Lift_c.h, which reaches math/Matrix.h
   through dBgW_KcMbg.h. Matrix4x3 has two guarded spellings over the same 0x30
   bytes and whichever a translation unit sees first stands; func_ov015_021123c8
   copies one flat, and with math/Matrix.h's structured spelling that copy
   compiles to 0xb4 against the ROM's 0x9c. Same rule, and the same reason, as
   the common.h include in include/dBgActor_c.h. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjBk_Lift_c_AssetId {
    ASSET_BK_LIFT01_BMD = 0x0599,
    ASSET_BK_LIFT01_KCL = 0x059a
};

#include "common.h"

#include "daObjBk_Lift_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"

/* SharedFilePtr has no fields; the two words are the handle's 8 bytes. The
 * constructors and destructors are the ROM resource-family veneers, aliased
 * in the manifest. */
struct BkLiftModelFilePtr : SharedFilePtr {
    u32 words[2];

    BkLiftModelFilePtr(u32 fileID);
    ~BkLiftModelFilePtr();
};

struct BkLiftCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    BkLiftCollisionFilePtr(u32 fileID);
    ~BkLiftCollisionFilePtr();
};

extern "C" {
extern BkLiftCollisionFilePtr data_ov015_02114a5c;   /* collision KCL */
extern BkLiftModelFilePtr data_ov015_02114a64;       /* step BMD */
extern CLPS_Block    data_ov015_02113594;
extern s16 data_02082214[];                 /* arm9 sin/cos table */
extern s8  data_0209f2f8;
extern u8  data_0209f220;

int  DecIfAbove0_Byte(u8 *p);
int  IsStarCollectedInCurLevel(int starID);
int  Vec3_Dist(void *a, void *b);
void func_02039394(void *collider, int v);
void func_020393a4(void *collider, int v);
void func_020393c4(dBgW *collider, void *callback);
void func_020393d4(dBgW *collider, void *callback);
int  func_ov015_021128f8(void *collider, daObjBk_Lift_c *lift, void *unused);

int  _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned handle, unsigned a,
                                           unsigned id, void *pos, s16 f);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y,
                                                    Fix12i z);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mat, int a, int b, int c, unsigned flags);
}

// @symbol daObjBk_Lift_c_classInit
/* BK_LIFT01's registry factory. 0x394 is this class's size, and the calls the
   cartridge inlines here -- fBase_c::operator new, dBgActor_c's base
   constructor, the vptr store and dExtShadowModel_c's constructor on the member at
   0x320 -- are exactly what the implicit default constructor of
   `struct daObjBk_Lift_c : dBgActor_c` with a dExtShadowModel_c member generates.
   Same shape as daObjBk_Rotebar_c_classInit in this overlay. */
extern "C" daObjBk_Lift_c *daObjBk_Lift_c_classInit()
{
    return new daObjBk_Lift_c();
}

// @symbol func_ov015_021128f8
/* dBgW callback veneer, armed by func_020393c4 in InitResources. Drops the
   collider and forwards the lift into func_ov015_021128e8. long_calls is the
   ROM's pooled absolute tail-call. */
extern "C" {
#pragma long_calls on
int func_ov015_021128f8(void *collider, daObjBk_Lift_c *lift, void *unused)
{
    return lift->func_ov015_021128e8(unused);
}
#pragma long_calls off
}

// @symbol _ZN14daObjBk_Lift_c19func_ov015_021128e8EPv
/* Marks the lift as stood on, which is what restarts a kind-2 ride in
   Behavior. Second parameter is the veneer's extra forwarded register -- a
   1-arg callee drops `mov r1, r2`. */
int daObjBk_Lift_c::func_ov015_021128e8(void *unused)
{
    mJustSteppedOn = 1;
    mMove = 1;
}

// @symbol _ZN14daObjBk_Lift_c13InitResourcesEv
int daObjBk_Lift_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov015_02114a64), 1, -1);
    mShadowModel.InitCuboid();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(data_ov015_02114a5c),
        &mClsnMat, 0x199, mAngleY, &data_ov015_02113594);
    func_020393d4(&mMeshCollider,
        (void *)&dBgW::UpdatePosWithVelocity);
    func_020393c4(&mMeshCollider, (void *)&func_ov015_021128f8);

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

    func_ov015_021123c8();

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

    if (data_0209f2f8 == 7
        && (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0)
        && mPosY >= 0xdac000)
        return 0;

    return 1;
}

// @symbol _ZN14daObjBk_Lift_c8BehaviorEv
int daObjBk_Lift_c::Behavior()
{
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
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x150000, 0x1000) != 0)
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
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
        if (Vec3_Dist(&mPosX, &mPrevPosX) != 0) {
            unsigned z = 0;
            unsigned r = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                (unsigned)mSoundID, 3, 0x82, &mCamSpacePosX, (s16)z);
            mSoundID = (s32)r;
        }
        mJustSteppedOn = 0;
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x150000, 0x1000);
    }

    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);
    func_ov015_021123c8();
    return 1;
}

// @symbol _ZN14daObjBk_Lift_c6RenderEv
int daObjBk_Lift_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjBk_Lift_c16CleanupResourcesEv
int daObjBk_Lift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_02114a64.Release();
    data_ov015_02114a5c.Release();
    return 1;
}

// @symbol _ZN14daObjBk_Lift_c19func_ov015_021123c8Ev
/* Re-aims the cuboid shadow: the model's matrix, then the step's own position
   biased by mShadowOffsetX / mShadowOffsetY and flattened onto mFloorPosY. */
void daObjBk_Lift_c::func_ov015_021123c8()
{
    mShadowMat = mModel.mat4x3;
    mShadowMat.m[9] = (mPosX - mShadowOffsetX) >> 3;
    mShadowMat.m[10] = mFloorPosY >> 3;
    mShadowMat.m[11] = (mPosZ - mShadowOffsetY) >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_S5_j(
        this, &mShadowModel, &mShadowMat, 0x190000, 0x28000, 0x258000, 0xf);
}

// @symbol _ZN14daObjBk_Lift_c15OnHitByMegaCharER6Player
/* Slot 27. Player::IncMegaKillCount is a real method, and the trailing
   unqualified Kill() reaches this class's own slot-31 override. */
void daObjBk_Lift_c::OnHitByMegaChar(Player &player)
{
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
void daObjBk_Lift_c::Kill()
{
    Vector3 dustPos;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, mPosX, mPosY, mPosZ);
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
