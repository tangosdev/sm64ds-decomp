//cpp
/* daMky_c -- Tall Tall Mountain Ukiki (UKIKI_THIEF 0x10b, UKIKI_STAR 0x10c).
 *
 * ov030 0x02115b78 is the bytes 7daMky_c. The star monkey's talk
 * (St_PushCage_Main) finds UKIKI_CAGE (0x67) and shoves it.
 * Key function ~daMky_c (out-of-line): D1 0x02111688, D0 0x021116d0;
 * D2 has no ROM home (deadstripped). defer_codegen off keeps ROM order.
 * Factories daMky_c_classInit_MONKEY_STAR/_THIEF last
 * (historical aliases UkikiStar_Spawn, UkikiThief_Spawn).
 * The helpers and the eleven St_*_Main ticks are daMky_c methods (S33);
 * symbols.txt carries the mangled spellings.
 *
 * leftovers:
 * - Of the 22 pointer-to-member records at 0x02115ac8, EnterState0..10
 *   store mState; the other eleven are the ticks, named St_<state>_Main
 *   after what each body does. The state numbers are the cartridge's and
 *   the words are coined; the enter halves keep the numbers in their own
 *   names. Helpers those ticks call take the same Ukiki, so they are
 *   methods too.
 * - func_0201267c and PlayBank3 are the bank-3 wrappers.
 *   func_02012790 is Sound::Play2D(2, id). func_020383f0 and
 *   dBgCh_Actr_UpdateContinuous_Veneer are the tail-call veneers the
 *   bl targets. dBgCh_Actr::GetSphereResult returns its argument plus 0x30;
 *   GetFloorResult is the previous symbol, and this TU still calls
 *   dBgCh_Actr::GetSphereResult. DetectClsnSlot0 only queries collider slot zero.
 *   dBgPi::GetOwner returns the result's owner (+0x20). Real
 *   dBgPi copy construction avoids the unwanted default-constructor call.
 * - The carrier/cap attachment uses dActor_c::mCarryMatrix at +0xc8.
 *   The cap's player pointer is the inherited dActor_c::mEatingPlayer.
 *   PathPtr and line-query construction/cleanup are ordinary C++ calls.
 * - data_ov030_* handles stay address labels. decl_common.h types
 *   four of them as void*[] where this file's blocks say int[].
 *   g_profile is not this TU's data.
 * - St_PushCage_Main fills a Vector3 v it never reads. As a Vector3
 *   the three stores survive; as `s32 v[3]`, or with v removed, mwcc
 *   drops them and the function comes out 0x24 short.
 */
#pragma defer_codegen off

#include "types.h"
#include "common.h"
#include "daMky_c.h"
#include "nitro/fx/fx.h"
#include "SharedFilePtr.h"
#include "decl_SaveData.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Actr.h"
#include "SaveData.h"
#include "daPly_c.h"
#include "daObjMarioCap_c.h"
#include "private/mky_dcamera.h"
#include "SurfaceInfo.h"
#include "dBgCh_Lin.h"
#include "dBgPi.h"
#include "dClipper.h"
#include "Sound.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "Vector3s.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"

/* The model handle constructs through ModelFile::ModelFile and the animation
 * handles through AnimFile::AnimFile. Declared here, ahead of the
 * members that read them; defined at the bottom in retail __sinit order. */
struct MkyModelFilePtr : SharedFilePtr {
    MkyModelFilePtr(unsigned int fileID);
    ~MkyModelFilePtr();
};
struct MkyAnimFilePtr : SharedFilePtr {
    MkyAnimFilePtr(unsigned int fileID);
    ~MkyAnimFilePtr();
};

extern MkyAnimFilePtr gMonkeyHoldAnimationFile;
extern MkyAnimFilePtr gMonkeyJumpStAnimationFile;
extern MkyAnimFilePtr gMonkeyJumpEdAnimationFile;
extern MkyAnimFilePtr gMonkeyRunAnimationFile;
extern MkyAnimFilePtr data_ov030_02115cf0; /* wait 1 */
extern MkyAnimFilePtr gMonkeyWait2AnimationFile;
extern MkyAnimFilePtr gMonkeyWait3AnimationFile;
extern MkyAnimFilePtr gMonkeyWait4AnimationFile;

/* The carry offsets, one per hold pose: UpdateModelAndCap picks one by
 * index. The table is defined at the bottom, in retail __sinit order. */
struct MkyOff {
    Vector3 offset;
    MkyOff(int, int, int, int);
    MkyOff(int, int, int);
    ~MkyOff();
};
typedef char MkyOff_size_must_be_0xc[sizeof(MkyOff) == 0xc ? 1 : -1];

extern MkyOff data_ov030_02115ddc[4];

/* Actor IDs 0xbf, 0x10b and 0x10c are PLAYER, UKIKI_THIEF and UKIKI_STAR.
 * dActor_c::mFlags bits 0x20000 and 0x40000 are the two yoshi-mouth states.
 * These int-returning predicates are what make the ROM materialize 0/1
 * before testing it. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsUkikiThief(dActor_c *actor) {
    return actor->actorID == 0x10b;
}

static inline int IsUkikiStar(dActor_c *actor) {
    return actor->actorID == 0x10c;
}

static inline int IsBeingEaten(dActor_c *actor) {
    return (actor->mFlags & 0x20000) != 0;
}

static inline int IsInYoshiMouth(dActor_c *actor) {
    return (actor->mFlags & 0x40000) != 0;
}

static inline int IsFlagSet(dActor_c *actor, u32 flag) {
    return (actor->mFlags & flag) != 0;
}

static inline int IsPlayingAnim(daMky_c *mky, SharedFilePtr &animFile) {
    return mky->mAnmModel.file == animFile.filePtr;
}

/* ~daMky_c is the first virtual DECLARED in include/daMky_c.h and it is
 * defined here, out of line, so it is this class's key function: this TU owns
 * _ZTV7daMky_c, _ZTI7daMky_c and _ZTS7daMky_c.
 *
 * Under `#pragma defer_codegen off` an out-of-line destructor emits D1, then
 * D0, then D2, in that order and at this position.  The cartridge puts D1 at
 * 0x02111688 and D0 at 0x021116d0, the two lowest addresses in the run, which
 * is exactly what this produces.  D2 has no ROM home anywhere and is
 * deadstripped.
 *
 * The body is empty on purpose.  The compiler writes the whole of both
 * variants from the class definition: the vptr store, then dExtAnmModel_c,
 * dExtShadowModel_c, dCcAc_c and dBgCh_Actr destroyed in reverse declaration order
 * (PathPtr is trivial and skipped), then ~dActor_c; D0 additionally inlines
 * all of that and tail-calls Memory::Deallocate. */
daMky_c::~daMky_c()
{
}

/* Vtable slot 18.  Two instructions: mov r0,#7; bx lr. */
s32 daMky_c::OnYoshiTryEat()
{
    return 7;
}

void daMky_c::ReturnToPerch() {

    extern dClipper gClipper;
    extern Matrix4x3 gViewMtx;
    struct Vector3 a, b, out, asr;

    if (DecIfAbove0_Byte(&this->mResetTimer))
        return;

    dBgCh_Lin rc;

    a.x = this->mPos.x;
    a.y = this->mPos.y;
    a.z = this->mPos.z;
    a.y = a.y + 0x32000;
    b.x = this->mPos.x;
    b.y = this->mPos.y;
    b.z = this->mPos.z;
    b.y = b.y - 0x96000;
    rc.SetObjAndLine(a, b, this);

    if (this->mPerchPos.y - this->mPos.y > 0x96000 || rc.DetectClsn()) {
        Vec3_Asr(&asr, &this->mPerchPos, 3);

        if (gClipper.TransformAndClip(gViewMtx, asr, 0x1f400, out) > 0xc350000) {
            this->mPos = this->mPerchPos;
            this->mSpawnPos = this->mPerchPos;
            this->mPrevPos = this->mPerchPos;
            UpdateModelAndCap();
            this->mResetTimer = 150;
        }
    }
}
void daMky_c::PlayRunFootstepSound() {
    if (!IsPlayingAnim(this, gMonkeyRunAnimationFile))
        return;
    int v = (short)((unsigned int)this->mAnmModel.currFrame << 4 >> 16);
    if (v == 0xa || v == 0xc)
        func_0201267c(0xea, &this->mCamSpacePos);
}
void daMky_c::PlayIdleSounds() {
    int frame = (short)(((unsigned)(this->mAnmModel.currFrame << 4)) >> 16);

    if (IsPlayingAnim(this, data_ov030_02115cf0)) {
        if (frame == 7 || frame == 0x28)
            func_0201267c(0xeb, &this->mCamSpacePos);
    } else if (IsPlayingAnim(this, gMonkeyWait2AnimationFile)) {
        if (frame == 1) {
            func_0201267c(0xf1, &this->mCamSpacePos);
            func_0201267c(0xe8, &this->mCamSpacePos);
        }
    } else if (IsPlayingAnim(this, gMonkeyWait3AnimationFile)) {
        if (frame == 8)
            func_0201267c(0xe9, &this->mCamSpacePos);
    }
}
int daMky_c::CycleIdleAnimation() {
    extern void** data_ov030_02115bc8[];
    PlayIdleSounds();
    if (this->mAnmModel.WillHitFrame(0) != 0 || (!IsPlayingAnim(this, data_ov030_02115cf0) &&
                                                 !IsPlayingAnim(this, gMonkeyWait2AnimationFile) &&
                                                 !IsPlayingAnim(this, gMonkeyWait3AnimationFile) &&
                                                 !IsPlayingAnim(this, gMonkeyWait4AnimationFile))) {
        this->mAnmModel.SetAnim((BCA_File *)(data_ov030_02115bc8[this->mAnimIdx][1]), 0, 0x1000, 0);
        {
            this->mAnmModel.speed = 0x1000;
            this->mAnimIdx++;
        }
        if (this->mAnimIdx >= 0xb)
            this->mAnimIdx = 0;
        return 1;
    }
    return 0;
}
int daMky_c::FollowPath() {
    typedef short s16;

  struct Vector3 v;
  int d;
  s16 ang;
  int *p;
  int n;
  mPathPtr.GetNode(v, mPathNode);
  d = Vec3_HorzDist(&this->mPos, &v);
  ang = Vec3_HorzAngle(&this->mPos, &v);
  UpdateAngle(this->mAngle.y, ang, 2, 0x400);
  this->mPrevAngle.y = this->mAngle.y;
  if (d < this->mHorzSpeed) {
      n = mPathPtr.NumNodes();
      p = &mPathNode;
      n = n - 1;
      *p = *p + 1;
      if (mPathNode >= n)
          return 1;
  }
  return 0;
}
int daMky_c::HandleContact() {
    daPly_c *player;

    if (IsBeingEaten(this) && this->mState != 2) {
        this->mPlayer = mEatingPlayer;
        if (IsUkikiThief(this))
            ChangeState(5);
        else if (IsUkikiStar(this))
            ChangeState(6);
        return 1;
    }

    if (this->mdCcAc_c.otherOwner == 0)
        return 0;

    if ((this->mdCcAc_c.hitFlags & 0x40000) && this->mState != 2) {
        this->mPlayer = this->ClosestPlayer();
        this->mPrevState = this->mState;
        ChangeState(2);
        return 1;
    }

    player = (daPly_c *)dActor_c::FindWithID(this->mdCcAc_c.otherOwner);
    if (player == 0 || !IsPlayerActor(player))
        return 0;

    if (player->mIsUnderwater != 0)
        return 0;
    if (player->mIsMetal != 0)
        return 0;
    if (player->mIsVanish != 0)
        return 0;
    if (player->mHasWings != 0)
        return 0;

    if (this->mdCcAc_c.hitFlags & 0x1000) {
        if (player->TryGrab(*this)) {
            this->mPlayer = player;
            if (IsUkikiThief(this))
                ChangeState(3);
            else if (IsUkikiStar(this))
                ChangeState(4);
        }
    }
    return 1;
}
int daMky_c::UpdateSafePos() {
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 v;
        int y, z, x, s;
        y = this->mPos.y;
        z = this->mPos.z;
        x = this->mPos.x;
        s = y + 0x1e000;
        v.x = x;
        v.y = s;
        v.z = z;
        rg.SetObjAndPos(v, this);
        if (rg.DetectClsnSlot0() == 0 || this->mPos.y - rg.clsnY > 0x2000) {
            this->mPos = this->mSpawnPos;
            return 1;
        }
        this->mSpawnPos = this->mPos;
    }
    return 0;
}
int daMky_c::RevertOffLedge() {

#define ABS(x) ((x) < 0 ? -(x) : (x))
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = this->mPos.y;
            int z = this->mPos.z;
            int x = this->mPos.x;
            int y2 = y + 0x1e000;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, this);
        if (rg.DetectClsn() == 0 || ABS(rg.clsnY - this->mPos.y) > 0x1000) {
            this->mHorzSpeed = 0;
            this->mPos.x = this->mPrevPos.x;
            this->mPos.y = this->mPrevPos.y;
            this->mPos.z = this->mPrevPos.z;
            return 1;
        }
    }
    return 0;
}
void daMky_c::UpdateFloorAndWalls(dBgCh_Actr *w) {
    if (IsFlagSet(this, 0x4000))
        return;
    if (IsUkikiThief(this) && this->mState != 9)
        func_020383f0(&this->mWithMeshClsn);
    else
        dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);
    if (w->IsOnGround() != 0) {
        Vector3 n;
        w->GetFloorResult()->surface.CopyNormalTo(n);
        if (n.y != 0) {
            int s = FX_MUL(n.x, this->mSpeed.x) + FX_MUL(n.z, this->mSpeed.z);
            this->mSpeed.y = -(FX_Div(s, n.y) + 0x8000);
        }
    }
    if (w->IsOnWall() != 0) {
        Vector3 wn;
        w->GetWallResult()->surface.CopyNormalTo(wn);
    }
}
void daMky_c::UpdateModelAndCap() {
    int idx;
    Matrix4x3 *res;
    dActor_c* obj;
    unsigned int id;
    Vector3s rot;
    Vector3 trans;

    if (IsFlagSet(this, 0x100) && this->mPlayer && this->mPlayer->mCarryMatrix) {
        idx = 0;
        if (this->mPlayer->IsFrontSliding() || this->mPlayer->LostGrabbedObject()) {
            idx = 1;
        }
        if (this->mPlayer->param1 == 2) {
            idx = (idx + 2) & 0xff;
        }
        res = UpdateCarry(*this->mPlayer, data_ov030_02115ddc[idx].offset);
        mAnmModel.mat4x3 = *res;
    } else {
        Matrix4x3_FromRotationY(&this->mAnmModel.mat4x3, this->mAngle.y);
        this->mAnmModel.mat4x3.t.x = this->mPos.x >> 3;
        this->mAnmModel.mat4x3.t.y = this->mPos.y >> 3;
        this->mAnmModel.mat4x3.t.z = this->mPos.z >> 3;
    }

    if (!IsInYoshiMouth(this)) {
        DropShadowRadHeight(this->mShadowModel, this->mAnmModel.mat4x3, 0x5a000, 0x190000, 0xf);
    }

    id = this->mCapUniqueID;
    if (id == 0)
        return;

    obj = dActor_c::FindWithID(id);

    trans = Vector3(0xa00, 0, -0x2f00);
    rot.x = -0x3f00;
    rot.y = 0;
    rot.z = -0x4000;

    MtxStack::copy(mAnmModel.mat4x3);
    MtxStack::concat(mAnmModel.data.transforms[5]);
    MtxStack::transM(trans);
    MtxStack::XYZrotM(rot.x, rot.y, rot.z);
    mCapMtx = MtxStack::get();

    obj->mCarryMatrix = &mCapMtx;
    obj->mPos.x = this->mPos.x;
    obj->mPos.y = this->mPos.y;
    obj->mPos.z = this->mPos.z;
}
/* State 10's tick. Steers mAngle.y toward HorzAngleToCPlayer() + 0x8000 -- the
 * heading away from the player -- at the 0x13000 EnterState10 armed, and hands
 * over to state 0 once mActionTimer runs out. */
int daMky_c::St_Flee_Main() {
    short ang = this->HorzAngleToCPlayer() + 0x8000;
    ApproachLinear(this->mAngle.y, ang, 0xa28);
    this->mPrevAngle.y = this->mAngle.y;
    if (DecIfAbove0_Byte(&this->mActionTimer) == 0)
        ChangeState(0);
    this->mAnmModel.Advance();
    if (IsUkikiThief(this)) {
        this->UpdatePos(&this->mdCcAc_c);
        UpdateSafePos();
        UpdateFloorAndWalls(&this->mWithMeshClsn);
        HandleContact();
    } else {
        this->UpdatePos(&this->mdCcAc_c);
        UpdateFloorAndWalls(&this->mWithMeshClsn);
        HandleContact();
        RevertOffLedge();
    }
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();
    PlayRunFootstepSound();
    return 1;
}
int daMky_c::EnterState10(){
    mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile.filePtr, 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    mHorzSpeed = 0x13000;
    mActionTimer = 30;
    mState = 0xa;
    return 1;
}

/* State 9's tick. It never moves: EnterState9 zeroed mHorzSpeed and nothing here
 * restores it. It cycles the idle animations and waits for the one condition that
 * lets this Ukiki resume -- the star back within 0x514000 of mPerchPos (state 1),
 * the thief standing on plain collision again (state 0). */
int daMky_c::St_Wait_Main() {
    CycleIdleAnimation();
    this->mAnmModel.Advance();
    this->UpdatePos(&this->mdCcAc_c);
    UpdateFloorAndWalls(&this->mWithMeshClsn);
    HandleContact();
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();

    if (IsUkikiStar(this)) {
        if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000) {
            if (this->mPos.y > this->mPerchPos.y - 0x12c000) {
                ChangeState(1);
            }
        }
    } else {
        if (this->mWithMeshClsn.IsOnGround()) {
            dBgPi res = *mWithMeshClsn.GetSphereResult();
            if (res.GetClsnID() == -1 || res.GetOwner() == 0) {
                ChangeState(0);
            }
        }
    }
    return 1;
}
int daMky_c::EnterState9()
{
    mHorzSpeed = 0;
    mState = 9;
    return 1;
}

/* State 8's tick. Walks mPathPtr to UKIKI_CAGE (actor 0x67) and talks (message
 * 0xbd), then shoves the cage: mCagePushProgress climbs 0x400 a frame while the
 * cage carries mHorzSpeed 0x400 and then falls, and once the cage is gone the
 * Ukiki marks itself for destruction. */
int daMky_c::St_PushCage_Main() {

    dActor_c *cage = dActor_c::FindWithActorID(0x67, 0);
    daPly_c *player = this->ClosestPlayer();
    Vector3 v;
    v.x = 0x981;
    v.y = 0x77a;
    v.z = 0x501;

    switch (this->mSubState) {
    case 0:
        if (FollowPath() != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyWait2AnimationFile.filePtr, 0, 0x1000, 0);
            this->mAnmModel.speed = 0x1000;
            this->mHorzSpeed = 0;
            this->mSubState++;
        }
        PlayRunFootstepSound();
        break;
    case 1:
        ApproachLinear(this->mAngle.y, Vec3_HorzAngle(&this->mPos, &player->mPos), 0x300);
        if (Vec3_Dist(&this->mPos, &player->mPos) < 0x96000) {
            if (player->StartTalk(*this, 1) != 0) {
                {
                    this->mSubState++;
                }
            }
        }
        PlayIdleSounds();
        break;
    case 2: {
        Vector3 sp = this->mPos;
        sp.y += 0x50000;
        if (player->ShowMessage(*this, 0xbd, &sp, 1, 0) != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyWait3AnimationFile.filePtr, 0, 0x1000, 0);
            this->mAnmModel.SetFlags(0);
            func_0201267c(0xd1, &this->mCamSpacePos);
            this->mSubState++;
        }
        break;
    }
    case 3:
        if (player->GetTalkState() == 2) {
            player->HasFinishedTalking();
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile.filePtr, 0, 0x1000, 0);
            this->mSubState++;
        }
        break;
    case 4:
        if (ApproachLinear(this->mAngle.y, -0x1fb2, 0x400) != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpStAnimationFile.filePtr, 0x40000000,
                                    0x1000, 0);
            this->mPrevAngle.y = this->mAngle.y;
            this->mHorzSpeed = 0xf000;
            this->mSpeed.y = 0x2f000;
            func_0201267c(0xf1, &this->mCamSpacePos);
            this->mSubState++;
        }
        break;
    case 5:
        if (this->mWithMeshClsn.JustHitGround() != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpEdAnimationFile.filePtr, 0x40000000,
                                    0x1000, 0);
            this->mHorzSpeed = 0;
            this->mSubState++;
        }
        break;
    case 6:
        if (this->mAnmModel.Finished() != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyWait2AnimationFile.filePtr, 0, 0x1000, 0);
            this->mAnmModel.speed = 0x1000;
            cage->mHorzSpeed = 0x400;
            Sound::PlaySub(0x20, 0x14, 0x7f, 0x15666, 0);
            this->mActionTimer = 120;
            this->mSubState++;
        }
        /* fallthrough */
    case 7: {
        this->mCagePushProgress += 0x400;
        if (this->mCagePushProgress > 0x17ffd) {
            {
                this->mSubState++;
            }
        }
    }
        /* fallthrough */
    case 8:
        if (DecIfAbove0_Byte(&this->mActionTimer) == 0) {
            Sound::PlaySub(0x20, 0x7f, 0, 0x15666, 0);
            this->mSubState++;
        }
        break;
    case 9:
        this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpStAnimationFile.filePtr, 0x40000000, 0x1000,
                                0);
        cage->mVertAccel = -0x2000;
        cage->mTerminalVelocity = -0x3c000;
        this->mSubState++;
        break;
    case 10:
        if (dActor_c::FindWithActorID(0x67, 0) == 0) {
            MarkForDestruction();
        }
        break;
    default:
        break;
    }

    this->mAnmModel.Advance();
    UpdatePos(&this->mdCcAc_c);
    UpdateFloorAndWalls(&this->mWithMeshClsn);
    this->mdCcAc_c.Clear();
    return 1;
}
int daMky_c::EnterState8() {
    this->mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile.filePtr, 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    this->mPathPtr.FromID(this->param1 & 0xff);
    mPathNode = 1;
    mSubState = 0;
    this->mHorzSpeed = 0x6000;
    mState = 8;
    return 1;
}

/* State 7's tick, reached only from the two in-mouth states. EnterState7 cleared
 * the 0x80000 flag, dropped mEatingPlayer and placed the Ukiki 0x4b000 in front of
 * the player, so this half only waits for the ground and then resumes. */
int daMky_c::St_LeaveMouth_Main() {
    dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);
    if (this->mWithMeshClsn.JustHitGround() || this->mWithMeshClsn.IsOnGround()) {
        this->mSpeed.y = 0;
        if (IsUkikiThief(this)) {
            dBgPi res = *mWithMeshClsn.GetSphereResult();
            if (res.GetClsnID() != 0xffffffff)
                ChangeState(9);
            else
                ChangeState(this->mPrevState);
        } else if (IsUkikiStar(this)) {
            if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000 &&
                this->mPos.y > this->mPerchPos.y - 0x12c000) {
                ChangeState(this->mPrevState);
            } else {
                ChangeState(9);
            }
        }
    }

    this->mAnmModel.Advance();
    this->UpdatePos(&this->mdCcAc_c);
    HandleContact();
    this->mdCcAc_c.Clear();
    return 1;
}
int daMky_c::EnterState7()
{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef short s16;
    int *pos;
    int *py;
    int *pz;
    struct Vector3 v;
    daPly_c *other;
    u16 ang;
    s16 s;
    int mul = 0x4b000;
    mFlags &= ~0x80000;
    this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpStAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    this->mHorzSpeed = 0xa000;
    this->mSpeed.y = 0;
    other = this->mPlayer;
    pos = &mPos.x;
    s = other->mAngle.y;
    this->mAngle.y = s;
    s = this->mAngle.y;
    py = &mPos.y;
    pz = &mPos.z;
    this->mPrevAngle.y = s;
    other = this->mPlayer;
    {
        int *op = &other->mPos.x;
        int t0 = op[0];
        mPos.x = t0;
        int t1 = op[1];
        this->mPos.y = t1;
        int t2 = op[2];
        this->mPos.z = t2;
    }
    ang = this->mAngle.y;
    s = FX_SinIdx(ang >> 4);
    *pos = (*pos) + FX_MUL(s, mul);
    *py = (*py) + 0x50000;
    ang = this->mAngle.y;
    s = FX_CosIdx(ang >> 4);
    *pz = (*pz) + FX_MUL(s, mul);
    other = this->mPlayer;
    {
        int oy = other->mPos.y;
        int oz = other->mPos.z;
        mul = oy + 0x50000;
        v.x = other->mPos.x;
        v.y = mul;
        v.z = oz;
    }
    this->DetectRaycastClsn(v, *reinterpret_cast<Vector3 *>(pos), 1);
    mEatingPlayer = 0;
    this->mPlayer = 0;
    this->mState = 7;
    return 1;
}

/* State 6's tick -- the star Ukiki in Yoshi's mouth; HandleContact sends the star
 * here while IsBeingEaten. Message 0xc1 is a question: yes drops the Ukiki and
 * queues state 8, the cage. */
int daMky_c::St_StarEaten_Main() {
    extern u8 data_0209d684;
    if (IsInYoshiMouth(this))
        mPos = mPlayer->mPos;

    if (IsFlagSet(this, 0x80000)) {
        this->mPrevState = 1;
        ChangeState(7);
        return 1;
    }

    switch (this->mSubState) {
    case 0: {
        if (IsInYoshiMouth(this)) {
            mPos = mPlayer->mPos;
            mSubState++;
        } else if (!IsBeingEaten(this) && !IsInYoshiMouth(this)) {
            /* Out of Yoshi altogether; the ROM re-tests the mouth bit. */
            mEatingPlayer = 0;
            ChangeState(this->mPrevState);
        }
        break;
    }
    case 1:
        if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000 &&
            this->mPos.y > this->mPerchPos.y - 0x12c000) {
            if (this->mPlayer->ShowMessage(*this, 0xc1, 0, 0, 0) != 0) {
                func_0201267c(0xd1, &this->mCamSpacePos);
                this->mSubState++;
            }
        }
        mPos = mPlayer->mPos;
        mActionTimer = 60;
        break;
    case 2:
        if (this->mPlayer->GetTalkState() == -1) {
            u8 g = data_0209d684;
            if (g == 1) {
                this->mPlayer->DropActor();
                this->mPrevState = 8;
                ChangeState(7);
            } else if (g == 2) {
                this->mSubState++;
            }
        }
        break;
    case 3:
        if (DecIfAbove0_Byte(&this->mActionTimer) == 0) {
            this->mSubState = 1;
        }
        break;
    }
    return 1;
}
int daMky_c::EnterState6()
{
    this->mFlags &= ~0x80000;
    if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000 && mPos.y > mPerchPos.y - 0x12c000) {
        mSubState = 0;
        this->SpawnSoundObj(1);
    } else {
        mSubState = 3;
    }
    mHorzSpeed = 0;
    mActionTimer = 60;
    this->mdCcAc_c.Clear();
    mPrevState = mState;
    mState = 6;
    return 1;
}

/* State 5's tick -- the thief in Yoshi's mouth; HandleContact sends the thief here
 * while IsBeingEaten. Message 0xc2 or 0xc3 depending on mHasSpawnedCap; on the way
 * out it respawns the cap with parameter 5 and queues state 10. */
int daMky_c::St_ThiefEaten_Main() {
    if (IsInYoshiMouth(this)) {
        int *p = &this->mPlayer->mPos.x;
        this->mPos.x = p[0];
        this->mPos.y = p[1];
        this->mPos.z = p[2];
    }

    switch (this->mSubState) {
    case 0: {
        if (IsInYoshiMouth(this)) {
            if (this->mHasSpawnedCap != 0) {
                ::dActor_c *a = ::dActor_c::FindWithID(this->mCapUniqueID);
                a->mEatingPlayer = mPlayer;
                a->mFlags |= 0x40000;
            }
            this->mSubState++;
        } else if (!IsBeingEaten(this) && !IsInYoshiMouth(this)) {
            /* Out of Yoshi altogether; the ROM re-tests the mouth bit. */
            mEatingPlayer = 0;
            ChangeState(this->mPrevState);
        }
        break;
    }
    case 1: {
        s16 msg = (this->mHasSpawnedCap != 0) ? 0xc2 : 0xc3;
        if (this->mPlayer->ShowMessage(*this, msg, 0, 0, 0) != 0) {
            func_0201267c(0xd1, &this->mCamSpacePos);
            this->mSubState++;
        }
        if (IsFlagSet(this, 0x80000))
            ChangeState(7);
        break;
    }
    case 2:
        if (this->mPlayer->GetTalkState() == -1) {
            this->mPlayer->DropActor();
            this->mSubState++;
        }
        break;
    case 3: {
        if (IsFlagSet(this, 0x80000)) {
            if (this->mHasSpawnedCap != 0) {
                Spawn(0x10d, (this->mCapPlayerNo << 8) | 5, this->mPlayer->mPos, 0, this->mAreaId,
                      -1);
                ((fBase_c *)::dActor_c::FindWithID(this->mCapUniqueID))->MarkForDestruction();
                this->mCapUniqueID = 0;
                this->mHasSpawnedCap = 0;
                this->mPrevState = 0xa;
            }
            ChangeState(7);
        }
        break;
    }
    }

    return 1;
}
int daMky_c::EnterState5() {
    this->mFlags &= ~0x80000;
    mSubState = 0;
    this->mHorzSpeed = 0;
    this->mdCcAc_c.Clear();
    this->mWithMeshClsn.ClearGroundFlag();
    mPrevState = mState;
    mState = 5;
    return 1;
}

/* State 4's tick -- the star Ukiki held by the player; HandleContact sends the star
 * here when TryGrab succeeds. Message 0xc0 is a question: yes starts state 8, no
 * re-asks after mActionTimer. Being let go sends it to state 1 or state 9. */
int daMky_c::St_StarGrabbed_Main() {

    extern dCamera_c *gCamera;
    extern unsigned char data_0209d684;

    this->mAngle.y = this->mPlayer->mAngle.y;
    this->mPrevAngle.y = this->mAngle.y;

    if (!IsFlagSet(this, 0x100) || IsFlagSet(this, 0x2000)) {
        daPly_c *other = this->mPlayer;
        Vector3 v;
        int oy = other->mPos.y;
        int oz = other->mPos.z;
        int vy = oy + 0x32000;
        v.x = other->mPos.x;
        v.y = vy;
        v.z = oz;
        this->DetectRaycastClsn(v, this->mPos, 1);

        if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000 &&
            this->mPos.y > this->mPerchPos.y - 0x12c000) {
            ChangeState(1);
        } else {
            ChangeState(9);
        }
        this->mPlayer = 0;
        return 1;
    }

    switch (this->mSubState) {
    case 0:
        if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000 &&
            this->mPos.y > this->mPerchPos.y - 0x12c000) {
            if (this->mPlayer->StartTalk(*this, 1) != 0) {
                Vector3 camPos;
                {
                    Vector3 *src = &gCamera->pos;
                    camPos.x = src->x;
                    camPos.y = src->y;
                    camPos.z = src->z;
                }
                short ang = Vec3_HorzAngle(&camPos, &this->mPlayer->mPos);
                {
                    Vector3 *op = &this->mPlayer->mPos;
                    MtxStack::transS(op->x, op->y, op->z);
                }
                MtxStack::YrotM(ang);
                MtxStack::transM(0, 0, -0x64000);

                Vector3 msgPos;
                msgPos.x = MtxStack::get().t.x;
                msgPos.y = MtxStack::get().t.y;
                msgPos.z = MtxStack::get().t.z;
                msgPos.y = this->mPos.y + 0x64000;

                if (this->mPlayer->ShowMessage(*this, 0xc0, &msgPos, 0, 2) != 0) {
                    func_0201267c(0xd1, &this->mCamSpacePos);
                    this->mSubState++;
                }
            }
        }
        this->mActionTimer = 60;
        break;
    case 1:
        if (this->mPlayer->GetTalkState() == -1) {
            unsigned char g = data_0209d684;
            if (g == 1) {
                this->mPlayer->DropActor();
                ChangeState(8);
            } else if (g == 2) {
                this->mSubState++;
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte(&this->mActionTimer) == 0)
            this->mSubState = 0;
        break;
    }

    this->mAnmModel.Advance();
    this->mdCcAc_c.Clear();
    return 1;
}
int daMky_c::EnterState4(){
    this->mAnmModel.SetAnim((BCA_File *)gMonkeyHoldAnimationFile.filePtr, 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000 &&
        this->mPos.y > mPerchPos.y - 0x12c000) {
        mSubState = 0;
        this->SpawnSoundObj(1);
    } else {
        mSubState = 2;
    }
    mActionTimer = 60;
    mState = 4;
    return 1;
}

/* State 3's tick -- the thief held by the player; HandleContact sends the thief here
 * when TryGrab succeeds. With mHasSpawnedCap set it takes the cap (SaveData::
 * PlayerLoseCap or the player's mHasNoCap, plus the 0x10d cap actor); with it clear
 * it hands the cap back and flees into state 10. */
int daMky_c::St_ThiefGrabbed_Main() {
    typedef short s16;
    typedef signed char s8;
    typedef unsigned char u8;
    typedef unsigned int u32;
    typedef int s32;


    /* The arm9 symbols.txt row spells this Vector3_16 (not Vector3s) — a wrong mangling here left the reloc BLIND and broke mwldarm. */

    extern dCamera_c *gCamera;
    int msg;
    s16 a = this->mPlayer->mAngle.y;
    this->mAngle.y = a;
    this->mPrevAngle.y = this->mAngle.y;

    switch (this->mSubState) {
    case 0:
        if (this->mHasSpawnedCap != 0) {
            {
                daPly_c *p = this->mPlayer;
                int t = (p->mCharacter == p->param1);
                t = (t != 0);
                this->mPlayerWasOwnCharacter = t;
            }
            if (this->mPlayerWasOwnCharacter == 0) {
                daPly_c *p = this->mPlayer;
                p->mHasNoCap = 1;
            } else {
                SaveData::PlayerLoseCap();
            }
            {
                daPly_c *p = this->mPlayer;
                void *spawned;
                this->mCapPlayerNo = p->param1;
                msg = this->mAreaId;
                spawned = Spawn(0x10d, (this->mCapPlayerNo << 8) | 2, this->mPos, 0, msg, -1);
                this->mCapUniqueID = ((dActor_c *)spawned)->uniqueID;
            }
        } else {
            if (this->mPlayerWasOwnCharacter != 0)
                func_02012790(0xa);
        }
        this->mSubState++;
        /* fall through */
    case 1: {
        s16 ang;
        Vector3 camPos;
        Vector3 msgPos;
        {
            u8 fl = this->mHasSpawnedCap;
            msg = fl ? 0xbe : 0xbf;
            dCamera_c *camBase = gCamera;
            Vector3 *src = &camBase->pos;
            camPos.x = src->x;
            camPos.y = src->y;
            camPos.z = src->z;
            ang = Vec3_HorzAngle(&camPos, &this->mPlayer->mPos);
        }
        {
            int *op = &this->mPlayer->mPos.x;
            MtxStack::transS(op[0], op[1], op[2]);
        }
        MtxStack::YrotM(ang);
        MtxStack::transM(0, 0, -0x64000);
        {
            int msgArg = (msg << 16) >> 16;
            int my = MtxStack::get().t.y;
            int mx = MtxStack::get().t.x;
            int mz = MtxStack::get().t.z;
            msgPos.x = mx;
            msgPos.y = my;
            msgPos.z = mz;
            msgPos.y = this->mPos.y + 0x64000;
            if (this->mPlayer->ShowMessage(*this, msgArg, &msgPos, 0, 2) != 0) {
                func_0201267c(0xd1, &this->mCamSpacePos);
                this->mSubState++;
            }
        }
        break;
    }
    case 2:
        if (this->mPlayer->GetTalkState() == -1) {
            if (this->mHasSpawnedCap != 0) {
                this->mFlags &= ~0x200u;
                this->mPlayer->DropActor();
                this->mFlags |= 0x200u;
            } else {
                this->mPlayer->DropActor();
            }
            this->mSubState++;
        }
        break;
    case 3: {
        if (!IsFlagSet(this, 0x100)) {
            if (this->mHasSpawnedCap != 0) {
                if (this->mPlayerWasOwnCharacter == 0) {
                    daPly_c *p = this->mPlayer;
                    p->SetNewHatCharacter(p->mCharacter, 0, 0);
                }
                this->mPrevState = 1;
                ChangeState(2);
            } else {
                dActor_c *act = dActor_c::FindWithID(this->mCapUniqueID);
                act->mCarryMatrix = 0;
                {
                    int *src = &this->mPlayer->mPos.x;
                    act->mPos.x = src[0];
                    act->mPos.y = src[1];
                    act->mPos.z = src[2];
                }
                this->mCapUniqueID = 0;
                ChangeState(0xa);
            }
            this->mPlayer = 0;
        }
        break;
    }
    case 4: {
        daPly_c *p = this->mPlayer;
        if (p->mIsMetal == 0 &&
            p->mIsVanish == 0 &&
            p->mHasWings == 0) {
            this->mSubState = 0;
            this->mHasSpawnedCap ^= 1;
            break;
        }
        /* fall through */
    }
    case 5:
        if (!IsFlagSet(this, 0x100) || IsFlagSet(this, 0x2000)) {
            ChangeState(0xa);
            this->mPlayer = 0;
        }
        break;
    default:
        break;
    }

    this->mAnmModel.Advance();
    this->mdCcAc_c.Clear();
    return 1;
}
int daMky_c::EnterState3()
{
    this->mAnmModel.SetAnim((BCA_File *)gMonkeyHoldAnimationFile.filePtr, 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    if (mHasSpawnedCap == 0 && SaveData::HasPlayerLostCap()) {
        mSubState = 5;
    } else {
        daPly_c *p = this->mPlayer;
        if (p->mIsMetal != 0 ||
            p->mIsVanish != 0 ||
            p->mHasWings != 0) {
            mSubState = 4;
        } else {
            mSubState = 0;
            this->mHasSpawnedCap ^= 1;
        }
    }
    mState = 3;
    return 1;
}

/* State 2's tick. EnterState2 set the jump-start animation, the launch speeds and
 * the limited-movement flag; this waits for JustHitGround, then clears that flag,
 * returns to the run animation and resumes mPrevState. */
int daMky_c::St_Jump_Main() {
    this->mAnmModel.Advance();
    this->UpdatePos(&this->mdCcAc_c);
    UpdateFloorAndWalls(&this->mWithMeshClsn);
    if (this->mWithMeshClsn.JustHitGround() != 0) {
        this->mWithMeshClsn.ClearLimMovFlag();
        this->mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile.filePtr, 0, 0x1000, 0);
        this->mAnmModel.speed = 0x1000;
        this->mSpawnPos = this->mPos;
        ChangeState(this->mPrevState);
    }
    this->mdCcAc_c.Clear();
    HandleContact();
    return 1;
}
int daMky_c::EnterState2() {
    typedef short s16;

    daPly_c *p;

    p = this->mPlayer;
    if (p == 0)
        p = this->ClosestPlayer();

    if (IsUkikiThief(this)) {
        this->mAngle.y = Vec3_HorzAngle(&this->mPos, &this->mPerchPos);
    } else if (Vec3_Dist(&this->mPerchPos, &this->mPos) < 0x514000 &&
               this->mPos.y > this->mPerchPos.y - 0x12c000) {
        this->mAngle.y = Vec3_HorzAngle(&this->mPos, &p->mPos);
    } else {
        this->mAngle.y = Vec3_HorzAngle(&this->mPos, &this->mPerchPos);
    }

    this->mPrevAngle.y = this->mAngle.y;
    this->mHorzSpeed = 0xd000;
    this->mSpeed.y = IsUkikiThief(this) ? 0x23000 : 0x1e000;

    this->mWithMeshClsn.SetLimMovFlag();
    this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpStAnimationFile.filePtr, 0x40000000, 0x1000, 0);
    func_0201267c(0xd1, &this->mCamSpacePos);
    func_0201267c(0xf1, &this->mCamSpacePos);
    this->mState = 2;
    return 1;
}

/* State 1's tick. Holds the player at arm's length through three substates: 0 runs
 * away (the player's heading + 0x8000) at 0x13000 inside 0x190000, 1 closes again at
 * 0xc000 beyond 0x258000, 2 stands still in between. Cornered on a wall within
 * 0x12c000 of a fast or airborne player, it jumps (state 2). */
int daMky_c::St_KeepAway_Main() {
    typedef short s16;

    daPly_c *p;
    fx32 dist;
    s16 angle;
    int onWall;

    p = this->mPlayer;
    if (p == 0)
        p = this->ClosestPlayer();

    dist = Vec3_Dist(&this->mPos, &p->mPos);

    if ((IsUkikiStar(this) && Vec3_Dist(&this->mPerchPos, &p->mPos) > 0x514000) ||
        p->mPos.y < this->mPerchPos.y - 0x12c000) {
        this->mSubState = 2;
    }

    switch (this->mSubState) {
    case 0:
        angle = Vec3_HorzAngle(&this->mPos, &p->mPos) + 0x8000;
        this->mHorzSpeed = 0x13000;
        if (dist >= 0x1f4000)
            this->mSubState = 2;
        break;
    case 1:
        angle = Vec3_HorzAngle(&this->mPos, &p->mPos);
        this->mHorzSpeed = 0xc000;
        if (dist < 0x190000)
            this->mSubState = 0;
        else if (dist < 0x1f4000)
            this->mSubState = 2;
        break;
    case 2:
        angle = Vec3_HorzAngle(&this->mPos, &p->mPos);
        this->mHorzSpeed = 0;
        if (dist < 0x190000)
            this->mSubState = 0;
        else if (dist >= 0x258000)
            this->mSubState = 1;
        break;
    }

    ApproachLinear(this->mAngle.y, angle, 0xa28);
    this->mPrevAngle.y = this->mAngle.y;
    if (this->mHorzSpeed != 0)
        this->mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile.filePtr, 0, 0x1000, 0);
    else
        CycleIdleAnimation();

    this->mAnmModel.Advance();

    if (IsUkikiThief(this)) {
        UpdatePos(&this->mdCcAc_c);
        onWall = (UpdateSafePos() | this->mWithMeshClsn.IsOnWall()) != 0;
        UpdateFloorAndWalls(&this->mWithMeshClsn);
        HandleContact();
    } else {
        UpdatePos(&this->mdCcAc_c);
        UpdateFloorAndWalls(&this->mWithMeshClsn);
        HandleContact();
        onWall = (RevertOffLedge() | this->mWithMeshClsn.IsOnWall()) != 0;
    }

    if (this->mState == 1 && onWall && dist < 0x12c000) {
        if (p->mHorzSpeed > 0x9000 || p->IsInAir() != 0) {
            this->mPrevState = 1;
            ChangeState(2);
        }
    }

    this->mdCcAc_c.Clear();
    if (this->mState == 1)
        this->mdCcAc_c.Update();
    PlayRunFootstepSound();
    return 1;
}
int daMky_c::EnterState1(){
    mSubState = 0;
    mState = 1;
    return 1;
}

/* State 0's tick -- the thief's resting behaviour. It always turns toward the
 * closest player, walks at 0xc000 past 0x15e000 and stops to idle inside 0xfa000. */
int daMky_c::St_Follow_Main() {
    typedef short s16;
    int dist = this->DistToCPlayer();
    if (dist > 0x15e000) {
        this->mHorzSpeed = 0xc000;
        this->mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile.filePtr, 0, 0x1000, 0);
    } else if (dist <= 0xfa000) {
        this->mHorzSpeed = 0;
        CycleIdleAnimation();
    }
    ApproachLinear(this->mAngle.y, this->HorzAngleToCPlayer(), 0x514);
    this->mPrevAngle.y = this->mAngle.y;
    this->mAnmModel.speed = 0x1000;
    this->mAnmModel.Advance();
    if (IsUkikiThief(this)) {
        this->UpdatePos(&this->mdCcAc_c);
        UpdateSafePos();
        UpdateFloorAndWalls(&this->mWithMeshClsn);
        HandleContact();
    } else {
        this->UpdatePos(&this->mdCcAc_c);
        UpdateFloorAndWalls(&this->mWithMeshClsn);
        HandleContact();
        RevertOffLedge();
    }
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();
    PlayRunFootstepSound();
    return 1;
}
int daMky_c::EnterState0()
{
    mState = 0;
    return 1;
}

void daMky_c::CallStateUpdate() {
    (this->*mStateDesc->update)();
}
void daMky_c::CallStateEnter() {
    (this->*mStateDesc->enter)();
}
void daMky_c::ChangeState(int idx) {
    extern StateFunctions data_ov030_02115e0c[];

    this->mStateDesc = &data_ov030_02115e0c[idx];
    CallStateEnter();
}
extern MkyModelFilePtr data_ov030_02115d00;

/* File-scope extern "C" for the six virtuals below. A block-scope extern
 * inside a daMky_c method gets C++ linkage and the reference mangles a
 * second time, so these callees are declared here. The daMky_c methods
 * above are members of this class and are not redeclared. */
extern "C" {

extern SharedFilePtr  data_ov002_0210da40;
extern SharedFilePtr  data_ov002_0210d9a0;
extern SharedFilePtr  data_ov002_0210d9c0;
extern SharedFilePtr *data_ov030_02114824[10];
}

/* Vtable slot 3.  Releases the four shared files the class holds plus its
 * ten-entry table; it never touches `this`, which is why the legacy C form
 * could declare itself nullary and still reproduce. */
s32 daMky_c::CleanupResources()
{
    int i;
    data_ov002_0210da40.Release();
    data_ov002_0210d9a0.Release();
    data_ov002_0210d9c0.Release();
    data_ov030_02115d00.Release();
    for (i = 0; i < 10; i++)
        data_ov030_02114824[i]->Release();
    return 1;
}

/* Vtable slot 12.  The ROM body is one `bx lr`: the override exists only to
 * occupy the slot. */
void daMky_c::OnPendingDestroy()
{
}

/* Vtable slot 9. */
s32 daMky_c::Render()
{
    if (IsInYoshiMouth(this))
        return 1;
    mAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

/* Vtable slot 6. */
s32 daMky_c::Behavior()
{
    if (IsTooFarAwayFromPlayer(0x5dc000) != 0 && mState != 8) {
        if (IsUkikiThief(this) && mHasSpawnedCap == 0 && SaveData::HasPlayerLostCap() != 0) {
            daPly_c *pl = ClosestPlayer();
            unsigned cp = pl->param1;
            if (cp < 3) {
                dActor_c *spawned;
                mCapPlayerNo = cp;
                spawned = Spawn(0x10d, (mCapPlayerNo << 8) | 2, mPos, 0, mAreaId, -1);
                mCapUniqueID = spawned->uniqueID;
                mHasSpawnedCap = 1;
                ChangeState(1);
            }
        }
        ReturnToPerch();
    } else {
        CallStateUpdate();
        mAnmModel.UpdateVerts();
        UpdateModelAndCap();
    }
    return 1;
}

/* Vtable slot 0. */
s32 daMky_c::InitResources()
{
    int i;

    dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
    mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov030_02115d00), 1, 1);
    for (i = 0; i < 10; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov030_02114824[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mAnmModel.SetAnim((BCA_File *)data_ov030_02115cf0.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mdCcAc_c.Init(this, 0x28000, 0x64000, 0x800004, 0x49000);
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mPerchPos = mPos;
    mPerchPos.y += 0x64000;
    mSpawnPos = mPos;
    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    mPlayer = 0;
    mCapUniqueID = 0;
    mResetTimer = 150;
    if (IsUkikiThief(this)) {
        if (SaveData::HasPlayerLostCap() != 0) {
            daPly_c *player = ClosestPlayer();
            if (player->param1 < 3) {
                dActor_c *spawned;
                mCapPlayerNo = player->param1;
                spawned = Spawn(0x10d, (mCapPlayerNo << 8) | 2, mPos, 0, mAreaId, -1);
                mCapUniqueID = spawned->uniqueID;
                mHasSpawnedCap = 1;
                ChangeState(1);
            } else {
                ChangeState(0);
            }
        } else {
            ChangeState(0);
        }
    } else if (IsUkikiStar(this)) {
        ChangeState(1);
    }
    UpdateModelAndCap();
    return 1;
}

/* Vtable slot 19.  The cartridge's twelve bytes at this address are a linker
 * long-branch veneer to _ZN7fBase_c18MarkForDestructionEv in arm9 -- r0 still
 * holds `this` at the jump and the daPly_c& is dropped -- so the source form
 * that reproduces it is a one-line forwarding member with no return. */
void daMky_c::OnTurnIntoEgg(daPly_c &player) {
    MarkForDestruction();
}

/* Reconstructed source-style name: SM64DS proves daMky_c through RTTI,
 * allocation size, vtable identity, and the MONKEY_STAR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UkikiStar_Spawn. */
daMky_c *daMky_c_classInit_MONKEY_STAR() {
    return new daMky_c();
}

/* Reconstructed source-style name: SM64DS proves daMky_c through RTTI,
 * allocation size, vtable identity, and the MONKEY_THIEF registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UkikiThief_Spawn. */
daMky_c *daMky_c_classInit_MONKEY_THIEF() {
    return new daMky_c();
}

/* File handles, the eleven state pairs, and the four carry offsets.
 * Definition order is the retail __sinit order: model, ten animations,
 * the state table, then the four Vector3s. mwcc emits
 * __sinit_d_a_mky.cpp from these, including one ~Vector3 registration
 * per offset against the destructor this TU already emits.
 *
 * The model handle's ctor/dtor are ModelFile::ModelFile / ModelFile::~ModelFile.
 * Each animation handle's are AnimFile::AnimFile /
 * AnimFile::~AnimFile. Those four are not defined here. */
typedef char MkyModelFilePtr_size_must_be_8[sizeof(MkyModelFilePtr) == 8 ? 1 : -1];
typedef char MkyAnimFilePtr_size_must_be_8[sizeof(MkyAnimFilePtr) == 8 ? 1 : -1];

enum {
    ASSET_MONKEY_BMD = 0x03aa,
    ASSET_MONKEY_HOLD_BCA = 0x03ab,
    ASSET_MONKEY_JUMP_ED_BCA = 0x03ac,
    ASSET_MONKEY_JUMP_ST_BCA = 0x03ad,
    ASSET_MONKEY_RUN_BCA = 0x03ae,
    ASSET_MONKEY_RUN_ED_BCA = 0x03af,
    ASSET_MONKEY_RUN_ST_BCA = 0x03b0,
    ASSET_MONKEY_WAIT1_BCA = 0x03b1,
    ASSET_MONKEY_WAIT2_BCA = 0x03b2,
    ASSET_MONKEY_WAIT3_BCA = 0x03b3,
    ASSET_MONKEY_WAIT4_BCA = 0x03b4
};

MkyModelFilePtr data_ov030_02115d00(ASSET_MONKEY_BMD);
MkyAnimFilePtr gMonkeyHoldAnimationFile(ASSET_MONKEY_HOLD_BCA);
MkyAnimFilePtr gMonkeyJumpStAnimationFile(ASSET_MONKEY_JUMP_ST_BCA);
MkyAnimFilePtr gMonkeyJumpEdAnimationFile(ASSET_MONKEY_JUMP_ED_BCA);
MkyAnimFilePtr gMonkeyRunStAnimationFile(ASSET_MONKEY_RUN_ST_BCA);
MkyAnimFilePtr gMonkeyRunAnimationFile(ASSET_MONKEY_RUN_BCA);
MkyAnimFilePtr gMonkeyRunEdAnimationFile(ASSET_MONKEY_RUN_ED_BCA);
MkyAnimFilePtr data_ov030_02115cf0(ASSET_MONKEY_WAIT1_BCA);
MkyAnimFilePtr gMonkeyWait2AnimationFile(ASSET_MONKEY_WAIT2_BCA);
MkyAnimFilePtr gMonkeyWait3AnimationFile(ASSET_MONKEY_WAIT3_BCA);
MkyAnimFilePtr gMonkeyWait4AnimationFile(ASSET_MONKEY_WAIT4_BCA);

/* Eleven {enter, update} pairs. Brace-init is what makes __sinit copy the
 * 22 pointer-to-member constants; a constructor body here scalarizes. */
extern "C" daMky_c::StateFunctions data_ov030_02115e0c[11] = {
    {&daMky_c::EnterState0, &daMky_c::St_Follow_Main},
    {&daMky_c::EnterState1, &daMky_c::St_KeepAway_Main},
    {&daMky_c::EnterState2, &daMky_c::St_Jump_Main},
    {&daMky_c::EnterState3, &daMky_c::St_ThiefGrabbed_Main},
    {&daMky_c::EnterState4, &daMky_c::St_StarGrabbed_Main},
    {&daMky_c::EnterState5, &daMky_c::St_ThiefEaten_Main},
    {&daMky_c::EnterState6, &daMky_c::St_StarEaten_Main},
    {&daMky_c::EnterState7, &daMky_c::St_LeaveMouth_Main},
    {&daMky_c::EnterState8, &daMky_c::St_PushCage_Main},
    {&daMky_c::EnterState9, &daMky_c::St_Wait_Main},
    {&daMky_c::EnterState10, &daMky_c::St_Flee_Main},
};

/* Vector3(x, y, z) matches only the first offset. The other three store
 * y before z, which is what an unsigned-int view does, and the first is
 * three field stores. ~MkyOff is not defined here: after this TU's vague
 * ~Vector3 is stripped it is aliased onto _ZN7Vector3D1Ev, the one
 * destructor the cartridge registers for all four. */
inline MkyOff::MkyOff(int px, int py, int pz, int) {
    offset.x = px;
    offset.y = py;
    offset.z = pz;
}
inline MkyOff::MkyOff(int px, int py, int pz) {
    unsigned int *p = (unsigned int *)this;
    p[0] = px;
    p[1] = py;
    p[2] = pz;
}

MkyOff data_ov030_02115ddc[4] = {
    MkyOff(0x1c000, -0x42000, 0x18000, 0),
    MkyOff(0x1c000, -0x28000, 0x28000),
    MkyOff(0x2b000, -0x42000, 0x18000),
    MkyOff(0x30000, -0x1e000, 0x1e000),
};
