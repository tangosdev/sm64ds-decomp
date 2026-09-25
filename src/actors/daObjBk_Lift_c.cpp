//cpp
/**
 * Whomp's Fortress tower lift (registry profile BK_LIFT01).
 *
 * A stepping platform on the tower. `param1 & 0xff` picks the behaviour:
 * kind 1 flips yaw by a half turn every 0x87 frames, kind 2 rides between
 * mMinPosY and mMaxPosY and keeps a looping sound alive while it moves,
 * and anything else only refreshes the collider. The class carries its
 * own cuboid ShadowModel, and func_ov015_021123c8 is the helper that
 * re-aims it every frame.
 *
 * This TU owns text only: ov015 delinks no .data here, so the _ZTV / _ZTI /
 * _ZTS group the class names is compiler-only output, compared against the
 * cartridge's own copies at ov015 0x02114650 / 0x0211460c / 0x02114618.
 *
 * SOURCE ORDER IS REVERSE ROM ORDER. mwccarm 2004/b56 emits .text back to
 * front under this tree's flags, so the factory is written first and Kill()
 * last; the destructor pair comes off the in-class `~daObjBk_Lift_c() {}` in
 * include/daObjBk_Lift_c.h and lands ahead of everything, D1 then D0, which
 * is the order the cartridge has (0x02112290, then 0x021122dc).
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange,
 *   dActor_c::UpdatePos, Sound::PlayLong, Particle::System::NewSimple and
 *   dActor_c::DropShadowScaleXYZ stay spelled as mangled extern-C free
 *   functions. Each takes Fix12<int> by value, and a real method call homes
 *   the argument and size-DIFFs the caller (notes/mwccarm-codegen.md 6az).
 * Leftover: func_02039394 / func_020393a4 / func_020393c4 / func_020393d4
 *   are small stores into dBgW (range and the two collision callbacks).
 *   This TU calls them; naming belongs with dBgW in arm9.
 * Leftover: func_ov015_021128f8 keeps #pragma long_calls: the ROM veneer is
 *   the pooled `ldr ip,[pc,#8]; bx ip` absolute tail-call (size 0x14); a near
 *   `b` to func_ov015_021128e8 in this same TU is 0xc. The pragma is
 *   positional in 2004/b56, the same bracket
 *   src/game/actors/d_a_obj_b_swdoor.cpp uses.
 * Leftover: func_ov015_021128e8 keeps a second unused parameter: the veneer
 *   forwards two registers after dropping the collider, and a 1-arg callee
 *   drops `mov r1, r2`. Same shape as daObjFallBlock_c_RequestShake.
 * Leftover: func_ov015_021123c8 / func_ov015_021128e8 / func_ov015_021128f8
 *   keep their C-ABI cartridge names. They are this TU's own helpers, not
 *   vtable slots.
 * Leftover: data_ov015_02114a5c / data_ov015_02114a64 are this overlay's
 *   KCL and BMD handles. symbols.txt also coins MovingBarSmall_ClsnFile /
 *   MovingBarSmall_ModelFile on the same two addresses -- this class's
 *   retired coined name -- so this TU uses the address-true spelling, the
 *   way src/game/actors/d_a_obj_bk_rotebar.cpp does.
 * Leftover: data_ov015_02113594 is the CLPS block in overlay .data that
 *   this TU does not own; data_02082214 is arm9's sin/cos table and
 *   data_0209f220 / data_0209f2f8 are arm9 scene state.
 * Leftover: g_profile_BK_LIFT01 lives outside this TU.
 */

/* MEASURED -- MUST STAY AHEAD OF daObjBk_Lift_c.h, which reaches math/Matrix.h
   through dBgW_KcMbg.h. Matrix4x3 has two guarded spellings over the same 0x30
   bytes and whichever a translation unit sees first stands; func_ov015_021123c8
   copies one flat, and with math/Matrix.h's structured spelling that copy
   compiles to 0xb4 against the ROM's 0x9c. Same rule, and the same reason, as
   the common.h include in include/dBgActor_c.h. */
#include "common.h"

#include "daObjBk_Lift_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgCh_Gnd.h"

extern "C" {
extern SharedFilePtr data_ov015_02114a5c;   /* collision KCL */
extern SharedFilePtr data_ov015_02114a64;   /* step BMD */
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
int  func_ov015_021128e8(daObjBk_Lift_c *lift, void *unused);
int  func_ov015_021128f8(void *collider, daObjBk_Lift_c *lift, void *unused);
void func_ov015_021123c8(daObjBk_Lift_c *lift);

int  _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned handle, unsigned a,
                                           unsigned id, void *pos, s16 f);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y,
                                                    Fix12i z);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mat, int a, int b, int c, unsigned flags);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjBk_Lift_c_classInit
/* BK_LIFT01's registry factory. 0x394 is this class's size, and the calls the
   cartridge inlines here -- fBase_c::operator new, dBgActor_c's base
   constructor, the vptr store and ShadowModel's constructor on the member at
   0x320 -- are exactly what the implicit default constructor of
   `struct daObjBk_Lift_c : dBgActor_c` with a ShadowModel member generates.
   Same shape as daObjBk_Rotebar_c_classInit in this overlay. */
extern "C" daObjBk_Lift_c *daObjBk_Lift_c_classInit()
{
    return new daObjBk_Lift_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_021128f8
/* dBgW callback veneer, armed by func_020393c4 in InitResources. Drops the
   collider and forwards the lift into func_ov015_021128e8. long_calls is the
   ROM's pooled absolute tail-call. */
extern "C" {
#pragma long_calls on
int func_ov015_021128f8(void *collider, daObjBk_Lift_c *lift, void *unused)
{
    return func_ov015_021128e8(lift, unused);
}
#pragma long_calls off
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_021128e8
/* Marks the lift as stood on, which is what restarts a kind-2 ride in
   Behavior. Second parameter is the veneer's extra forwarded register -- a
   1-arg callee drops `mov r1, r2`. */
extern "C" {
int func_ov015_021128e8(daObjBk_Lift_c *lift, void *unused)
{
    lift->mJustSteppedOn = 1;
    lift->mMove = 1;
}
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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
    mShadowOffsetX = (int)(((s64)sine * 0xc8000 + 0x800) >> 12);
    angle = mAngleY;
    int cosine = data_02082214[(angle >> 4) * 2 + 1];
    mShadowOffsetY = (int)(((s64)cosine * 0xc8000 + 0x800) >> 12);

    func_ov015_021123c8(this);

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

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBk_Lift_c8BehaviorEv
int daObjBk_Lift_c::Behavior()
{
    char *self = (char *)this;
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
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
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
                if (flag != 0)
                    goto c2_hi;
                mPosY = (y < bound) ? bound : y;
                mVertSpeed = 0xa000;
                if (mJustSteppedOn == 0) {
                    flag = 0;
                    mMove = (u8)flag;
                }
                goto c2_after;
            c2_hi:
                bound = mMaxPosY;
                flag = (y <= bound);
                if (flag != 0)
                    goto c2_after;
                mPosY = (y > bound) ? bound : y;
                mVertSpeed = -0xa000;
                mMoveTimer = 0x5a;
            }
        }
    c2_after:
        UpdateModelPosAndRotY();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
        if (Vec3_Dist(&mPosX, &mPrevPosX) != 0) {
            unsigned z = 0;
            unsigned r = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                (unsigned)mSoundID, 3, 0x82, &mCamSpacePosX, (s16)z);
            mSoundID = (s32)r;
        }
        mJustSteppedOn = 0;
    } else {
        _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000);
    }

    func_020393a4(&mMeshCollider, 0x150000);
    func_02039394(&mMeshCollider, 0x1000);
    func_ov015_021123c8(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBk_Lift_c6RenderEv
int daObjBk_Lift_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBk_Lift_c16CleanupResourcesEv
int daObjBk_Lift_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_02114a64.Release();
    data_ov015_02114a5c.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov015_021123c8
/* Re-aims the cuboid shadow: the model's matrix, then the step's own position
   biased by mShadowOffsetX / mShadowOffsetY and flattened onto mFloorPosY. */
extern "C" void func_ov015_021123c8(daObjBk_Lift_c *lift)
{
    char *c = (char *)lift;
    *(Matrix4x3 *)(c + 0x348) = *(Matrix4x3 *)(c + 0xf0);
    *(int *)(c + 0x36c) = (*(int *)(c + 0x5c) - *(int *)(c + 0x384)) >> 3;
    *(int *)(c + 0x370) = *(int *)(c + 0x378) >> 3;
    *(int *)(c + 0x374) = (*(int *)(c + 0x64) - *(int *)(c + 0x388)) >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x320, c + 0x348, 0x190000, 0x28000, 0x258000, 0xf);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjBk_Lift_c15OnHitByMegaCharER6Player
/* Slot 27. Player::IncMegaKillCount is a real method, and the trailing
   unqualified Kill() reaches this class's own slot-31 override. */
void daObjBk_Lift_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
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
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
