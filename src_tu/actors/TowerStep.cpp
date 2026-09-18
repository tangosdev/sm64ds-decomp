//cpp
/**
 * Whomp's Fortress tower step (registry profile BK_LIFT01).
 *
 * A stepping platform on the tower. `param1 & 0xff` picks the behaviour:
 * kind 1 flips yaw by a half turn every 0x87 frames, kind 2 rides between
 * mMinPosY and mMaxPosY and keeps a looping sound alive while it moves,
 * and anything else only refreshes the collider. The class carries its
 * own cuboid ShadowModel, and func_ov015_021123c8 is the helper that
 * re-aims it every frame.
 *
 * This TU owns text only: ov015 delinks no .data here, so the _ZTV / _ZTI /
 * _ZTS group the class names is compiler-only output, and the factory keeps
 * taking the vptr symbol from decl_common.h.
 *
 * deslop
 * Leftover: the registry factory at 0x0211290c stays in
 *   src/d_a_obj_bk_lift.c together with the two small helpers at
 *   0x021128e8 / 0x021128f8. This entry licenses 0x02112290..0x021128e8
 *   only, so `new TowerStep()` is not written here.
 * Leftover: dBgW_KcMbg::SetFile, dBgActor_c::IsClsnInRange,
 *   dActor_c::UpdatePos, Sound::PlayLong, Particle::System::NewSimple and
 *   dActor_c::DropShadowScaleXYZ stay spelled as mangled extern-C free
 *   functions. Each takes Fix12<int> by value, and a real method call homes
 *   the argument and size-DIFFs the caller (notes/mwccarm-codegen.md 6az).
 * Leftover: func_02039394 / func_020393a4 / func_020393c4 / func_020393d4
 *   are small stores into dBgW (range and the two collision callbacks).
 *   This TU calls them; naming belongs with dBgW in arm9.
 * Leftover: func_ov015_021123c8 keeps its C-ABI cartridge name. It is this
 *   TU's own shadow-placement helper, not a vtable slot.
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

/* MEASURED -- MUST STAY AHEAD OF TowerStep.h, which reaches math/Matrix.h
   through dBgW_KcMbg.h. Matrix4x3 has two guarded spellings over the same 0x30
   bytes and whichever a translation unit sees first stands; func_ov015_021123c8
   copies one flat, and with math/Matrix.h's structured spelling that copy
   compiles to 0xb4 against the ROM's 0x9c. Same rule, and the same reason, as
   the common.h include in include/dBgActor_c.h. */
#include "common.h"

#include "TowerStep.h"
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
void func_ov015_021128f8();
void func_ov015_021123c8(TowerStep *step);

void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
int  _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
unsigned _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned handle, unsigned a,
                                           unsigned id, void *pos, s16 f);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y,
                                                    Fix12i z);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
void _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *shadow, void *mat, int a, int b, int c, unsigned flags);
}

// @symbol _ZN9TowerStep13InitResourcesEv
int TowerStep::InitResources()
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
        (void *)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
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

// @symbol _ZN9TowerStep8BehaviorEv
int TowerStep::Behavior()
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
        _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
        UpdateModelPosAndRotY();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(self, 0x150000, 0x1000) != 0)
            UpdateClsnPosAndRot();
    } else if (kind == 2) {
        if (DecIfAbove0_Byte(&mMoveTimer) == 0) {
            if (mMove != 0) {
                int flag;
                int bound;
                int y;

                _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
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

// @symbol _ZN9TowerStep6RenderEv
int TowerStep::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9TowerStep16CleanupResourcesEv
int TowerStep::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();

    data_ov015_02114a64.Release();
    data_ov015_02114a5c.Release();
    return 1;
}

/* Re-aims the cuboid shadow: the model's matrix, then the step's own position
   biased by mShadowOffsetX / mShadowOffsetY and flattened onto mFloorPosY. */
// @symbol func_ov015_021123c8
extern "C" void func_ov015_021123c8(TowerStep *step)
{
    char *c = (char *)step;
    *(Matrix4x3 *)(c + 0x348) = *(Matrix4x3 *)(c + 0xf0);
    *(int *)(c + 0x36c) = (*(int *)(c + 0x5c) - *(int *)(c + 0x384)) >> 3;
    *(int *)(c + 0x370) = *(int *)(c + 0x378) >> 3;
    *(int *)(c + 0x374) = (*(int *)(c + 0x64) - *(int *)(c + 0x388)) >> 3;
    _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, c + 0x320, c + 0x348, 0x190000, 0x28000, 0x258000, 0xf);
}

/* Slot 27. Player::IncMegaKillCount is a real method, and the trailing
   unqualified Kill() reaches this class's own slot-31 override. */
// @symbol _ZN9TowerStep15OnHitByMegaCharER6Player
void TowerStep::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

/* Slot 31, attributed by the vtable: _ZTV9TowerStep (ov015 0x02114650) carries
   0x0211233c at slot 31 -- config/arm9/overlays/ov015/relocs.txt has
   `from:0x021146cc kind:load to:0x0211233c` -- while _ZTV10dBgActor_c's word at
   the same slot (from:0x0210aeb4) relocates to 0x020ee55c,
   _ZN10dBgActor_c4KillEv.

   Particle 0x48 at the step's own position, no vertical offset, then the poof
   at that same position built from the fields a SECOND time: the ROM reloads
   mPos rather than reusing the words it just passed. */
// @symbol _ZN9TowerStep4KillEv
void TowerStep::Kill()
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

/* One definition, two ROM functions: mwcc emits the destructor variants as a
   group and this TU licenses D1 (0x02112290) then D0 (0x021122dc). The body is
   generated -- this class's vptr, then dBgActor_c's, inlined because
   dBgActor_c's destructor is defined in its class body, then mShadowModel,
   dBgActor_c's dBgW_KcMbg and its Model, then dActor_c. This class adds no
   member with a destructor of its own beyond mShadowModel. */
// @symbol _ZN9TowerStepD1Ev
// @symbol _ZN9TowerStepD0Ev
TowerStep::~TowerStep()
{
}
