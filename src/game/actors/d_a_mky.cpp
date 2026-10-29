//cpp
/* daMky_c -- Tall Tall Mountain Ukiki (UKIKI_THIEF 0x10b, UKIKI_STAR 0x10c).
 *
 * ov030 0x02115b78 is the bytes 7daMky_c. The star monkey's talk
 * (func_ov030_02112578) finds UKIKI_CAGE (0x67) and shoves it.
 * Key function ~daMky_c (out-of-line): D1 0x02111688, D0 0x021116d0;
 * D2 has no ROM home (deadstripped). defer_codegen off keeps ROM order.
 * Factories daMky_c_classInit_MONKEY_STAR/_THIEF last
 * (historical aliases UkikiStar_Spawn, UkikiThief_Spawn).
 * The func_ov030_* helpers are daMky_c methods (S33); symbols.txt
 * carries the mangled spellings.
 *
 * leftovers:
 * - The func_ov030_* methods keep their address names. Of the 22
 *   pointer-to-member records at 0x02115ac8, EnterState0..10 store
 *   mState. The other eleven are the ticks. Helpers those ticks call
 *   take the same Ukiki, so they are methods too.
 * - Shared animation/collision/scalar declarations retain their current
 *   reconstructed Fix12<int> spellings. Genuine scalar definitions and
 *   callers match in separate experiments; canonical shared migration
 *   remains pending, rather than an inherent compiler limitation.
 * - func_0201267c and PlayBank3 are the bank-3 wrappers.
 *   func_02012790 is Sound::Play2D(2, id). func_020383f0 and
 *   dBgCh_Actr_UpdateContinuous_Veneer are the tail-call veneers the
 *   bl targets. func_0203567c returns its argument plus 0x30;
 *   GetFloorResult is the previous symbol, and this TU still calls
 *   func_0203567c. DetectClsnSlot0 only queries collider slot zero.
 *   func_02037f44 returns word 8 of the record it is handed. Real
 *   dBgPi copy construction avoids the unwanted default-constructor call.
 * - Native Matrix4x3 copies use common.h's shared flat representation.
 *   common.h and math/Matrix.h still define incompatible C++ forms under
 *   one guard; this TU does not resolve that repository-wide type debt.
 * - The carrier/cap attachment uses dActor_c::mCarryMatrix at +0xc8.
 *   The cap's player pointer uses daObjMarioCap_c::mEatingPlayer.
 *   PathPtr and line-query construction/cleanup are ordinary C++ calls.
 * - data_ov030_* handles stay address labels. decl_common.h types
 *   four of them as void*[] where this file's blocks say int[].
 *   g_profile is not this TU's data.
 * - func_ov030_02112578 keeps `*(s32 *)((u8 *)v + N)` stores (not yet
 *   byte-verified as `v[N]`).
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
#include "Player.h"
#include "daObjMarioCap_c.h"
#include "dCamera_c.h"
#include "SurfaceInfo.h"
#include "dBgCh_Lin.h"
#include "dBgPi.h"
#include "dClipper.h"
#include "Sound.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "cstd.h"

/* The callees of this TU's members, spelled by their mangled or ROM names.
 * They cannot be declared inside a method body: a block-scope extern in a
 * daMky_c member gets C++ linkage, the reference mangles a second time, and
 * the module no longer links. Each is declared once, here, because C++
 * rejects two extern "C" declarations of one name that disagree, so one
 * spelling has to serve every call site. Data objects stay in the bodies:
 * a variable's name does not mangle. */
extern "C" {

void  Vec3_Asr(void *d, void *s, int sh);
}

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

static inline int IsPlayingAnim(daMky_c *mky, int *animFile) {
    return (int)mky->mAnmModel.file == animFile[1];
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
// @symbol _ZN7daMky_cD1Ev
// @symbol _ZN7daMky_cD0Ev
daMky_c::~daMky_c()
{
}

// @symbol _ZN7daMky_c13OnYoshiTryEatEv
/* Vtable slot 18.  Two instructions: mov r0,#7; bx lr. */
s32 daMky_c::OnYoshiTryEat()
{
    return 7;
}

// @symbol _ZN7daMky_c19func_ov030_02111734Ev
void daMky_c::func_ov030_02111734()
{

    extern dClipper data_0209f43c;
    extern Matrix4x3 data_0209b3ec;
    struct Vector3 a, b, out, asr;

    if (DecIfAbove0_Byte(&this->mResetTimer))
        return;

    dBgCh_Lin rc;

    a.x = this->mPosX;
    a.y = this->mPosY;
    a.z = this->mPosZ;
    a.y = a.y + 0x32000;
    b.x = this->mPosX;
    b.y = this->mPosY;
    b.z = this->mPosZ;
    b.y = b.y - 0x96000;
    rc.SetObjAndLine(a, b, this);

    if (this->mPerchPosY - this->mPosY > 0x96000 || rc.DetectClsn()) {
        Vec3_Asr(&asr, (Vector3 *)&this->mPerchPosX, 3);

        if (data_0209f43c.TransformAndClip(data_0209b3ec, asr, 0x1f400, out) > 0xc350000) {
            this->mPosX = this->mPerchPosX;
            this->mPosY = this->mPerchPosY;
            this->mPosZ = this->mPerchPosZ;
            this->mSpawnPosX = this->mPerchPosX;
            this->mSpawnPosY = this->mPerchPosY;
            this->mSpawnPosZ = this->mPerchPosZ;
            this->mPrevPosX = this->mPerchPosX;
            this->mPrevPosY = this->mPerchPosY;
            this->mPrevPosZ = this->mPerchPosZ;
            func_ov030_02112094();
            this->mResetTimer = 0x96;
        }
    }
}
// @symbol _ZN7daMky_c19func_ov030_02111890Ev
void daMky_c::func_ov030_02111890()
{
    extern int gMonkeyRunAnimationFile[];
    if (!IsPlayingAnim(this, gMonkeyRunAnimationFile))
        return;
    int v = (short)((unsigned int)this->mAnmModel.currFrame << 4 >> 16);
    if (v == 0xa || v == 0xc)
        func_0201267c(0xea, &this->mCamSpacePos);
}
// @symbol _ZN7daMky_c19func_ov030_02111908Ev
void daMky_c::func_ov030_02111908()
{
    extern int data_ov030_02115cf0[];
    extern int gMonkeyWait2AnimationFile[];
    extern int gMonkeyWait3AnimationFile[];

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
// @symbol _ZN7daMky_c19func_ov030_02111a00Ev
int daMky_c::func_ov030_02111a00()
{
    extern int data_ov030_02115cf0[];
    extern int gMonkeyWait2AnimationFile[];
    extern int gMonkeyWait3AnimationFile[];
    extern int gMonkeyWait4AnimationFile[];
    extern void** data_ov030_02115bc8[];
    func_ov030_02111908();
    if (static_cast<dExtFrameCtrl_c &>(this->mAnmModel).WillHitFrame(0) != 0 ||
        (!IsPlayingAnim(this, data_ov030_02115cf0) &&
         !IsPlayingAnim(this, gMonkeyWait2AnimationFile) &&
         !IsPlayingAnim(this, gMonkeyWait3AnimationFile) &&
         !IsPlayingAnim(this, gMonkeyWait4AnimationFile))) {
        this->mAnmModel.SetAnim((BCA_File *)(data_ov030_02115bc8[this->mAnimIdx][1]), 0, 0x1000, 0);
        {
            unsigned char *p = &this->mAnimIdx;
            this->mAnmModel.speed = 0x1000;
            (*p)++;
        }
        if (this->mAnimIdx >= 0xb)
            this->mAnimIdx = 0;
        return 1;
    }
    return 0;
}
// @symbol _ZN7daMky_c19func_ov030_02111b20Ev
int daMky_c::func_ov030_02111b20()
{
    typedef short s16;

  struct Vector3 v;
  int d;
  s16 ang;
  int *p;
  int n;
  mPathPtr.GetNode(v, mPathNode);
  d = Vec3_HorzDist(&this->mPos, &v);
  ang = Vec3_HorzAngle(&this->mPos, &v);
  UpdateAngle(this->mAngleY, ang, 2, 0x400);
  this->mPrevAngleY = this->mAngleY;
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
// @symbol _ZN7daMky_c19func_ov030_02111bc4Ev
int daMky_c::func_ov030_02111bc4()
{
    Player *player;

    if (IsBeingEaten(this) && this->mState != 2) {
        this->mPlayer = mEatingPlayer;
        if (IsUkikiThief(this))
            func_ov030_021141a8(5);
        else if (IsUkikiStar(this))
            func_ov030_021141a8(6);
        return 1;
    }

    if (this->mdCcAc_c.otherOwner == 0)
        return 0;

    if ((this->mdCcAc_c.hitFlags & 0x40000) && this->mState != 2) {
        this->mPlayer = this->ClosestPlayer();
        this->mPrevState = this->mState;
        func_ov030_021141a8(2);
        return 1;
    }

    player = (Player *)dActor_c::FindWithID(this->mdCcAc_c.otherOwner);
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
        if (player->TryGrab(*(dActor_c *)this)) {
            this->mPlayer = player;
            if (IsUkikiThief(this))
                func_ov030_021141a8(3);
            else if (IsUkikiStar(this))
                func_ov030_021141a8(4);
        }
    }
    return 1;
}
// @symbol _ZN7daMky_c19func_ov030_02111dd0Ev
int daMky_c::func_ov030_02111dd0()
{
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 v;
        int y, z, x, s;
        y = this->mPosY;
        z = this->mPosZ;
        x = this->mPosX;
        s = y + 0x1e000;
        v.x = x;
        v.y = s;
        v.z = z;
        rg.SetObjAndPos(v, this);
        if (rg.DetectClsnSlot0() == 0 || this->mPosY - rg.clsnY > 0x2000) {
            this->mPosX = this->mSpawnPosX;
            this->mPosY = this->mSpawnPosY;
            this->mPosZ = this->mSpawnPosZ;
            return 1;
        }
        this->mSpawnPosX = this->mPosX;
        this->mSpawnPosY = this->mPosY;
        this->mSpawnPosZ = this->mPosZ;
    }
    return 0;
}
// @symbol _ZN7daMky_c19func_ov030_02111ea4Ev
int daMky_c::func_ov030_02111ea4()
{


    #define ABS(x) ((x) < 0 ? -(x) : (x))
    if (this->mWithMeshClsn.IsOnGround() != 0) {
        dBgCh_Gnd rg;
        Vector3 pos;
        {
            int y = this->mPosY;
            int z = this->mPosZ;
            int x = this->mPosX;
            int y2 = y + 0x1e000;
            pos.x = x;
            pos.y = y2;
            pos.z = z;
        }
        rg.SetObjAndPos(pos, this);
        if (rg.DetectClsn() == 0 ||
            ABS(rg.clsnY - this->mPosY) > 0x1000) {
            this->mHorzSpeed = 0;
            this->mPosX = this->mPrevPosX;
            this->mPosY = this->mPrevPosY;
            this->mPosZ = this->mPrevPosZ;
            return 1;
        }
    }
    return 0;
}
// @symbol _ZN7daMky_c19func_ov030_02111f6cEP10dBgCh_Actr
void daMky_c::func_ov030_02111f6c(dBgCh_Actr *w) {
    if (IsFlagSet(this, 0x4000))
        return;
    if (IsUkikiThief(this) && this->mState != 9)
        func_020383f0(&this->mWithMeshClsn);
    else
        dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);
    if (w->IsOnGround() != 0) {
        Vector3 n;
        static_cast<const dBgPi *>(w->GetFloorResult())->surface.CopyNormalTo(n);
        if (n.y != 0) {
            int s = FX_MUL(n.x, this->mSpeedX) + FX_MUL(n.z, this->mSpeedZ);
            this->mVertSpeed = -(cstd::fdiv(s, n.y) + 0x8000);
        }
    }
    if (w->IsOnWall() != 0) {
        Vector3 wn;
        static_cast<const dBgPi *>(w->GetWallResult())->surface.CopyNormalTo(wn);
    }
}
// @symbol _ZN7daMky_c19func_ov030_02112094Ev
void daMky_c::func_ov030_02112094()
{
    /* from include/decl_common.h:255, which this TU does not include. */
    extern char data_ov030_02115ddc[];

    struct Bundle { Vector3_16 rot; short _p; Vector3 trans; int _tail[2]; };

    int idx;
    Matrix4x3 *res;
    dActor_c* obj;
    unsigned int id;
    Bundle bnd;

    if (IsFlagSet(this, 0x100) && this->mPlayer && this->mPlayer->mCarryMatrix) {
        idx = 0;
        if (this->mPlayer->IsFrontSliding() || this->mPlayer->LostGrabbedObject()) {
            idx = 1;
        }
        if (this->mPlayer->param1 == 2) {
            idx = (idx + 2) & 0xff;
        }
        res = UpdateCarry(*this->mPlayer, *(Vector3 *)(data_ov030_02115ddc + idx * 0xc));
        mAnmModel.mat4x3 = *res;
    } else {
        Matrix4x3_FromRotationY(&this->mAnmModel.mat4x3, this->mAngleY);
        this->mAnmModel.mat4x3.m[9] = this->mPosX >> 3;
        this->mAnmModel.mat4x3.m[10] = this->mPosY >> 3;
        this->mAnmModel.mat4x3.m[11] = this->mPosZ >> 3;
    }

    if (!IsInYoshiMouth(this)) {
        DropShadowRadHeight(this->mShadowModel, this->mAnmModel.mat4x3, 0x5a000, 0x190000, 0xf);
    }

    id = this->mCapUniqueID;
    if (id == 0)
        return;

    obj = dActor_c::FindWithID(id);

    bnd.trans.x = 0xa00;
    bnd.trans.y = 0;
    bnd.trans.z = -0x2f00;
    bnd.rot.x = -0x3f00;
    bnd.rot.y = 0;
    bnd.rot.z = -0x4000;

    MtxStack::copy(mAnmModel.mat4x3);
    MtxStack::concat(mAnmModel.data.transforms[5]);
    MtxStack::transM(bnd.trans);
    MtxStack::XYZrotM(bnd.rot);
    mCapMtx = MtxStack::get();

    obj->mCarryMatrix = &mCapMtx;
    obj->mPosX = this->mPosX;
    obj->mPosY = this->mPosY;
    obj->mPosZ = this->mPosZ;
}
// @symbol _ZN7daMky_c19func_ov030_021122b0Ev
int daMky_c::func_ov030_021122b0()
{
    short ang = this->HorzAngleToCPlayer() + 0x8000;
    ApproachLinear(this->mAngleY, ang, 0xa28);
    this->mPrevAngleY = this->mAngleY;
    if (DecIfAbove0_Byte(&this->mActionTimer) == 0)
        func_ov030_021141a8(0);
    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    if (IsUkikiThief(this)) {
        this->UpdatePos((dCc_c*)(&this->mdCcAc_c));
        func_ov030_02111dd0();
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
    } else {
        this->UpdatePos((dCc_c*)(&this->mdCcAc_c));
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
        func_ov030_02111ea4();
    }
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();
    func_ov030_02111890();
    return 1;
}
// @symbol _ZN7daMky_c12EnterState10Ev
int daMky_c::EnterState10(){
    struct S { int w[2]; };
    extern struct S gMonkeyRunAnimationFile;
    mAnmModel.SetAnim((BCA_File *)((void *)gMonkeyRunAnimationFile.w[1]), 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    mHorzSpeed = 0x13000;
    mActionTimer = 0x1e;
    mState = 0xa;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112400Ev
int daMky_c::func_ov030_02112400()
{
    func_ov030_02111a00();
    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    this->UpdatePos(&this->mdCcAc_c);
    func_ov030_02111f6c((dBgCh_Actr*)(&this->mWithMeshClsn));
    func_ov030_02111bc4();
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();

    if (IsUkikiStar(this)) {
        if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000) {
            if (this->mPosY > this->mPerchPosY - 0x12c000) {
                func_ov030_021141a8(1);
            }
        }
    } else {
        if (this->mWithMeshClsn.IsOnGround()) {
            dBgPi res = *func_0203567c(&mWithMeshClsn);
            if (res.GetClsnID() == -1 || func_02037f44(&res) == 0) {
                func_ov030_021141a8(0);
            }
        }
    }
    return 1;
}
// @symbol _ZN7daMky_c11EnterState9Ev
int daMky_c::EnterState9()
{
    mHorzSpeed = 0;
    mState = 9;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112578Ev
int daMky_c::func_ov030_02112578()
{

    extern void *gMonkeyWait2AnimationFile[];
    extern void *gMonkeyWait3AnimationFile[];
    extern void *gMonkeyJumpStAnimationFile[];
    extern void *gMonkeyJumpEdAnimationFile[];
    extern void *gMonkeyRunAnimationFile[];
    dActor_c *cage = dActor_c::FindWithActorID(0x67, 0);
    Player *player = this->ClosestPlayer();
    s32 v[3];
    *(s32 *)((u8 *)v + 0) = 0x981;
    *(s32 *)((u8 *)v + 4) = 0x77a;
    *(s32 *)((u8 *)v + 8) = 0x501;

    switch (this->mSubState) {
    case 0:
        if (func_ov030_02111b20() != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyWait2AnimationFile[1], 0, 0x1000, 0);
            this->mAnmModel.speed = 0x1000;
            this->mHorzSpeed = 0;
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
        func_ov030_02111890();
        break;
    case 1:
        ApproachLinear(this->mAngleY, Vec3_HorzAngle(&this->mPos, &player->mPos), 0x300);
        if (Vec3_Dist(&this->mPos, &player->mPos) < 0x96000) {
            if (player->StartTalk(*this, 1) != 0) {
                {
                    u8 *p = &this->mSubState;
                    *p = *p + 1;
                }
            }
        }
        func_ov030_02111908();
        break;
    case 2: {
        s32 sp[3];
        sp[0] = this->mPosX;
        sp[1] = this->mPosY;
        sp[2] = this->mPosZ;
        sp[1] += 0x50000;
        if (player->ShowMessage(*this, 0xbd, (const Vector3 *)sp, 1, 0) != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyWait3AnimationFile[1], 0, 0x1000, 0);
            static_cast<dExtFrameCtrl_c &>(this->mAnmModel).SetFlags(0);
            func_0201267c(0xd1, &this->mCamSpacePos);
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
        break;
    }
    case 3:
        if (player->GetTalkState() == 2) {
            player->HasFinishedTalking();
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile[1], 0, 0x1000, 0);
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
        break;
    case 4:
        if (ApproachLinear(this->mAngleY, (s16)0xffffe04e, 0x400) != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpStAnimationFile[1], 0x40000000, 0x1000,
                                    0);
            this->mPrevAngleY = this->mAngleY;
            this->mHorzSpeed = 0xf000;
            this->mVertSpeed = 0x2f000;
            func_0201267c(0xf1, &this->mCamSpacePos);
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
        break;
    case 5:
        if (this->mWithMeshClsn.JustHitGround() != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpEdAnimationFile[1], 0x40000000, 0x1000,
                                    0);
            this->mHorzSpeed = 0;
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
        break;
    case 6:
        if (static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Finished() != 0) {
            this->mAnmModel.SetAnim((BCA_File *)gMonkeyWait2AnimationFile[1], 0, 0x1000, 0);
            this->mAnmModel.speed = 0x1000;
            cage->mHorzSpeed = 0x400;
            Sound::PlaySub(0x20, 0x14, 0x7f, 0x15666, 0);
            this->mActionTimer = 0x78;
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
        /* fallthrough */
    case 7: {
        s32 *pp = &this->mCagePushProgress;
        *pp = *pp + 0x400;
        if (this->mCagePushProgress > 0x17ffd) {
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
    }
        /* fallthrough */
    case 8:
        if (DecIfAbove0_Byte(&this->mActionTimer) == 0) {
            Sound::PlaySub(0x20, 0x7f, 0, 0x15666, 0);
            {
                u8 *p = &this->mSubState;
                *p = *p + 1;
            }
        }
        break;
    case 9:
        this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpStAnimationFile[1], 0x40000000, 0x1000, 0);
        cage->mVertAccel = -0x2000;
        cage->mTerminalVelocity = -0x3c000;
        {
            u8 *p = &this->mSubState;
            *p = *p + 1;
        }
        break;
    case 10:
        if (dActor_c::FindWithActorID(0x67, 0) == 0) {
            MarkForDestruction();
        }
        break;
    default:
        break;
    }

    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    UpdatePos(&this->mdCcAc_c);
    func_ov030_02111f6c(&this->mWithMeshClsn);
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState8Ev
int daMky_c::EnterState8() {
    struct G { void *a; void *b; };
    extern struct G gMonkeyRunAnimationFile;
    this->mAnmModel.SetAnim((BCA_File *)gMonkeyRunAnimationFile.b, 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    this->mPathPtr.FromID(this->param1 & 0xff);
    mPathNode = 1;
    mSubState = 0;
    this->mHorzSpeed = 0x6000;
    mState = 8;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112a84Ev
int daMky_c::func_ov030_02112a84()
{
    dBgCh_Actr_UpdateContinuous_Veneer(&this->mWithMeshClsn);
    if (this->mWithMeshClsn.JustHitGround() || this->mWithMeshClsn.IsOnGround()) {
        this->mVertSpeed = 0;
        if (IsUkikiThief(this)) {
            dBgPi res = *func_0203567c(&mWithMeshClsn);
            if (res.GetClsnID() != 0xffffffff)
                func_ov030_021141a8(9);
            else
                func_ov030_021141a8(this->mPrevState);
        } else if (IsUkikiStar(this)) {
            if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000 &&
                this->mPosY > this->mPerchPosY - 0x12c000) {
                func_ov030_021141a8(this->mPrevState);
            } else {
                func_ov030_021141a8(9);
            }
        }
    }

    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    this->UpdatePos(&this->mdCcAc_c);
    func_ov030_02111bc4();
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState7Ev
int daMky_c::EnterState7()
{
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef short s16;
    extern short data_02082214[];
    extern void *gMonkeyJumpStAnimationFile[];
    u8 *new_var;
    int *pos;
    int *py;
    int *pz;
    struct Vector3 v;
    Player *other;
    u16 ang;
    s16 s;
    int mul = 0x4b000;
    mFlags &= ~0x80000;
    this->mAnmModel.SetAnim((BCA_File *)gMonkeyJumpStAnimationFile[1], 0x40000000, 0x1000, 0);
    this->mHorzSpeed = 0xa000;
    this->mVertSpeed = 0;
    other = this->mPlayer;
    pos = &mPosX;
    s = other->mAngleY;
    this->mAngleY = s;
    s = this->mAngleY;
    py = &mPosY;
    pz = &mPosZ;
    this->mPrevAngleY = s;
    other = this->mPlayer;
    {
        int *op = &other->mPosX;
        int t0 = op[0];
        mPosX = t0;
        int t1 = op[1];
        this->mPosY = t1;
        int t2 = op[2];
        this->mPosZ = t2;
    }
    ang = (u16)this->mAngleY;
    s = data_02082214[(ang >> 4) * 2];
    *pos = (*pos) + FX_MUL(s, mul);
    *py = (*py) + 0x50000;
    ang = (u16)this->mAngleY;
    s = data_02082214[((ang >> 4) * 2) + 1];
    *pz = (*pz) + FX_MUL(s, mul);
    other = this->mPlayer;
    {
        int oy = other->mPosY;
        int oz = *((int *)(new_var = (u8 *)&other->mPosZ));
        mul = oy + 0x50000;
        v.x = other->mPosX;
        v.y = mul;
        v.z = oz;
    }
    this->DetectRaycastClsn(v, *reinterpret_cast<Vector3 *>(pos), 1);
    mEatingPlayer = 0;
    this->mPlayer = 0;
    this->mState = 7;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02112da0Ev
int daMky_c::func_ov030_02112da0()
{
    extern u8 data_0209d684;
    if (IsInYoshiMouth(this)) {
        int *p = &this->mPlayer->mPosX;
        this->mPosX = p[0];
        this->mPosY = p[1];
        this->mPosZ = p[2];
    }

    if (IsFlagSet(this, 0x80000)) {
        this->mPrevState = 1;
        func_ov030_021141a8(7);
        return 1;
    }

    switch (this->mSubState) {
    case 0: {
        if (IsInYoshiMouth(this)) {
            Player *s = this->mPlayer;
            int *p = &s->mPosX;
            int x = *p;
            u8 *st = &this->mSubState;
            this->mPosX = x;
            this->mPosY = p[1];
            this->mPosZ = p[2];
            (*st)++;
        } else if (!IsBeingEaten(this) && !IsInYoshiMouth(this)) {
            /* Out of Yoshi altogether; the ROM re-tests the mouth bit. */
            mEatingPlayer = 0;
            func_ov030_021141a8(this->mPrevState);
        }
        break;
    }
    case 1:
        if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000 &&
            this->mPosY > this->mPerchPosY - 0x12c000) {
            if (this->mPlayer->ShowMessage(*this, 0xc1, 0, 0, 0) != 0) {
                func_0201267c(0xd1, &this->mCamSpacePos);
                this->mSubState++;
            }
        }
        {
            Player *s = this->mPlayer;
            u8 val = 0x3c;
            int *p = &s->mPosX;
            this->mPosX = *p;
            this->mPosY = p[1];
            this->mPosZ = p[2];
            this->mActionTimer = val;
        }
        break;
    case 2:
        if (this->mPlayer->GetTalkState() == -1) {
            u8 g = data_0209d684;
            if (g == 1) {
                this->mPlayer->DropActor();
                this->mPrevState = 8;
                func_ov030_021141a8(7);
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
// @symbol _ZN7daMky_c11EnterState6Ev
int daMky_c::EnterState6()
{
    this->mFlags &= ~0x80000;
    if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000 &&
        mPosY > mPerchPosY - 0x12c000) {
        mSubState = 0;
        this->SpawnSoundObj(1);
    } else {
        mSubState = 3;
    }
    mHorzSpeed = 0;
    mActionTimer = 0x3c;
    this->mdCcAc_c.Clear();
    mPrevState = mState;
    mState = 6;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113094Ev
int daMky_c::func_ov030_02113094()
{
    if (IsInYoshiMouth(this)) {
        int *p = &this->mPlayer->mPosX;
        this->mPosX = p[0];
        this->mPosY = p[1];
        this->mPosZ = p[2];
    }

    switch (this->mSubState) {
    case 0: {
        if (IsInYoshiMouth(this)) {
            if (this->mHasSpawnedCap != 0) {
                ::dActor_c *a = ::dActor_c::FindWithID(this->mCapUniqueID);
                static_cast<daObjMarioCap_c *>(a)->mEatingPlayer = mPlayer;
                a->mFlags |= 0x40000;
            }
            this->mSubState++;
        } else if (!IsBeingEaten(this) && !IsInYoshiMouth(this)) {
            /* Out of Yoshi altogether; the ROM re-tests the mouth bit. */
            mEatingPlayer = 0;
            func_ov030_021141a8(this->mPrevState);
        }
        break;
    }
    case 1: {
        int msg = (this->mHasSpawnedCap != 0) ? 0xc2 : 0xc3;
        if (this->mPlayer->ShowMessage(*this, (s16)msg, 0, 0, 0) != 0) {
            func_0201267c(0xd1, &this->mCamSpacePos);
            this->mSubState++;
        }
        if (IsFlagSet(this, 0x80000))
            func_ov030_021141a8(7);
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
                {
                    u32 z = 0;
                    this->mCapUniqueID = z;
                    this->mHasSpawnedCap = (u8)z;
                    this->mPrevState = 0xa;
                }
            }
            func_ov030_021141a8(7);
        }
        break;
    }
    }

    return 1;
}
// @symbol _ZN7daMky_c11EnterState5Ev
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

// @symbol _ZN7daMky_c19func_ov030_02113324Ev
int daMky_c::func_ov030_02113324()
{

    extern dCamera_c *data_0209f318;
    extern unsigned char data_0209d684;

    this->mAngleY = this->mPlayer->mAngleY;
    this->mPrevAngleY = this->mAngleY;

    if (!IsFlagSet(this, 0x100) || IsFlagSet(this, 0x2000)) {
        Player *other = this->mPlayer;
        Vector3 v;
        int oy = other->mPosY;
        int oz = other->mPosZ;
        int vy = oy + 0x32000;
        v.x = other->mPosX;
        v.y = vy;
        v.z = oz;
        this->DetectRaycastClsn(v, this->mPos, 1);

        if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000 &&
            this->mPosY > this->mPerchPosY - 0x12c000) {
            func_ov030_021141a8(1);
        } else {
            func_ov030_021141a8(9);
        }
        this->mPlayer = 0;
        return 1;
    }

    switch (this->mSubState) {
    case 0:
        if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000 &&
            this->mPosY > this->mPerchPosY - 0x12c000) {
            if (this->mPlayer->StartTalk(*this, 1) != 0) {
                Vector3 camPos;
                {
                    Vector3 *src = &data_0209f318->pos;
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
                msgPos.x = MtxStack::get().m[9];
                msgPos.y = MtxStack::get().m[10];
                msgPos.z = MtxStack::get().m[11];
                msgPos.y = this->mPosY + 0x64000;

                if (this->mPlayer->ShowMessage(*this, 0xc0, &msgPos, 0, 2) != 0) {
                    func_0201267c(0xd1, &this->mCamSpacePos);
                    {
                        unsigned char *p = &this->mSubState;
                        (*p)++;
                    }
                }
            }
        }
        this->mActionTimer = 0x3c;
        break;
    case 1:
        if (this->mPlayer->GetTalkState() == -1) {
            unsigned char g = data_0209d684;
            if (g == 1) {
                this->mPlayer->DropActor();
                func_ov030_021141a8(8);
            } else if (g == 2) {
                unsigned char *p = &this->mSubState;
                (*p)++;
            }
        }
        break;
    case 2:
        if (DecIfAbove0_Byte(&this->mActionTimer) == 0)
            this->mSubState = 0;
        break;
    }

    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState4Ev
int daMky_c::EnterState4(){
    extern int gMonkeyHoldAnimationFile[];
    this->mAnmModel.SetAnim((BCA_File *)((void *)gMonkeyHoldAnimationFile[1]), 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000 &&
        this->mPosY > mPerchPosY - 0x12c000) {
        mSubState = 0;
        this->SpawnSoundObj(1);
    } else {
        mSubState = 2;
    }
    mActionTimer = 0x3c;
    mState = 4;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_021136b0Ev
int daMky_c::func_ov030_021136b0()
{
    typedef short s16;
    typedef signed char s8;
    typedef unsigned char u8;
    typedef unsigned int u32;
    typedef int s32;


    /* The arm9 symbols.txt row spells this Vector3_16 (not Vector3s) — a wrong mangling here left the reloc BLIND and broke mwldarm. */

    extern dCamera_c *data_0209f318;
    int msg;
    s16 a = this->mPlayer->mAngleY;
    this->mAngleY = a;
    this->mPrevAngleY = this->mAngleY;

    switch (this->mSubState) {
    case 0:
        if (this->mHasSpawnedCap != 0) {
            {
                Player *p = this->mPlayer;
                int t = (p->mCharacter == p->param1);
                t = (t != 0);
                this->mPlayerWasOwnCharacter = t;
            }
            if (this->mPlayerWasOwnCharacter == 0) {
                Player *p = this->mPlayer;
                p->mHasNoCap = 1;
            } else {
                SaveData::PlayerLoseCap();
            }
            {
                Player *p = this->mPlayer;
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
        {
            u8 *st = &this->mSubState;
            (*st)++;
        }
        /* fall through */
    case 1: {
        s16 ang;
        Vector3 camPos;
        Vector3 msgPos;
        {
            u8 fl = this->mHasSpawnedCap;
            msg = fl ? 0xbe : 0xbf;
            dCamera_c *camBase = data_0209f318;
            Vector3 *src = &camBase->pos;
            camPos.x = src->x;
            camPos.y = src->y;
            camPos.z = src->z;
            ang = Vec3_HorzAngle(&camPos, &this->mPlayer->mPos);
        }
        {
            int *op = &this->mPlayer->mPosX;
            MtxStack::transS(op[0], op[1], op[2]);
        }
        MtxStack::YrotM(ang);
        MtxStack::transM(0, 0, -0x64000);
        {
            int msgArg = ((int)msg << 16) >> 16;
            int my = MtxStack::get().m[10];
            int mx = MtxStack::get().m[9];
            int mz = MtxStack::get().m[11];
            msgPos.x = mx;
            msgPos.y = my;
            msgPos.z = mz;
            msgPos.y = this->mPosY + 0x64000;
            if (this->mPlayer->ShowMessage(*this, msgArg, &msgPos, 0, 2) != 0) {
                func_0201267c(0xd1, &this->mCamSpacePos);
                {
                    u8 *st = &this->mSubState;
                    (*st)++;
                }
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
            {
                u8 *st = &this->mSubState;
                (*st)++;
            }
        }
        break;
    case 3: {
        if (!IsFlagSet(this, 0x100)) {
            if (this->mHasSpawnedCap != 0) {
                if (this->mPlayerWasOwnCharacter == 0) {
                    Player *p = this->mPlayer;
                    p->SetNewHatCharacter(p->mCharacter, 0, 0);
                }
                this->mPrevState = 1;
                func_ov030_021141a8(2);
            } else {
                dActor_c *act = dActor_c::FindWithID(this->mCapUniqueID);
                act->mCarryMatrix = 0;
                {
                    int *src = &this->mPlayer->mPosX;
                    act->mPosX = src[0];
                    act->mPosY = src[1];
                    act->mPosZ = src[2];
                }
                this->mCapUniqueID = 0;
                func_ov030_021141a8(0xa);
            }
            this->mPlayer = 0;
        }
        break;
    }
    case 4: {
        Player *p = this->mPlayer;
        if (p->mIsMetal == 0 &&
            p->mIsVanish == 0 &&
            p->mHasWings == 0) {
            this->mSubState = 0;
            {
                u8 *f = &this->mHasSpawnedCap;
                *f ^= 1;
            }
            break;
        }
        /* fall through */
    }
    case 5:
        if (!IsFlagSet(this, 0x100) || IsFlagSet(this, 0x2000)) {
            func_ov030_021141a8(0xa);
            this->mPlayer = 0;
        }
        break;
    default:
        break;
    }

    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    this->mdCcAc_c.Clear();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState3Ev
int daMky_c::EnterState3()
{
    /* The scalar ABI binding is retained: the
       ROM name carries by-value class parameters (e.g. Fix12<int>), which
       mwccarm passes differently at the call site, so declaring the true
       types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

    extern char gMonkeyHoldAnimationFile[];
    this->mAnmModel.SetAnim((BCA_File *)(*(void **)(gMonkeyHoldAnimationFile + 4)), 0, 0x1000, 0);
    this->mAnmModel.speed = 0x1000;
    if (mHasSpawnedCap == 0 && SaveData::HasPlayerLostCap()) {
        mSubState = 5;
    } else {
        Player *p = this->mPlayer;
        if (p->mIsMetal != 0 ||
            p->mIsVanish != 0 ||
            p->mHasWings != 0) {
            mSubState = 4;
        } else {
            mSubState = 0;
            unsigned char* f = &this->mHasSpawnedCap;
            *f ^= 1;
        }
    }
    mState = 3;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113b38Ev
int daMky_c::func_ov030_02113b38()
{
    typedef int Fix12i;
    extern int gMonkeyRunAnimationFile[];
    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    this->UpdatePos(&this->mdCcAc_c);
    func_ov030_02111f6c(&this->mWithMeshClsn);
    if (this->mWithMeshClsn.JustHitGround() != 0) {
        this->mWithMeshClsn.ClearLimMovFlag();
        this->mAnmModel.SetAnim((BCA_File *)((void *)gMonkeyRunAnimationFile[1]), 0, 0x1000, 0);
        this->mAnmModel.speed = 0x1000;
        this->mSpawnPosX = this->mPosX;
        this->mSpawnPosY = this->mPosY;
        this->mSpawnPosZ = this->mPosZ;
        func_ov030_021141a8(this->mPrevState);
    }
    this->mdCcAc_c.Clear();
    func_ov030_02111bc4();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState2Ev
int daMky_c::EnterState2() {
    typedef int Fix12i;
    typedef short s16;

    extern void *gMonkeyJumpStAnimationFile;
    Player *p;

    p = this->mPlayer;
    if (p == 0)
        p = this->ClosestPlayer();

    if (IsUkikiThief(this)) {
        this->mAngleY = Vec3_HorzAngle(&this->mPos, (Vector3 *)&this->mPerchPosX);
    } else if (Vec3_Dist((Vector3 *)&this->mPerchPosX, &this->mPos) < 0x514000 &&
               this->mPosY > this->mPerchPosY - 0x12c000) {
        this->mAngleY = Vec3_HorzAngle(&this->mPos, &p->mPos);
    } else {
        this->mAngleY = Vec3_HorzAngle(&this->mPos, (Vector3 *)&this->mPerchPosX);
    }

    this->mPrevAngleY = this->mAngleY;
    this->mHorzSpeed = 0xd000;
    this->mVertSpeed = IsUkikiThief(this) ? 0x23000 : 0x1e000;

    this->mWithMeshClsn.SetLimMovFlag();
    this->mAnmModel.SetAnim((BCA_File *)(((void **)&gMonkeyJumpStAnimationFile)[1]), 0x40000000,
                            0x1000, 0);
    func_0201267c(0xd1, &this->mCamSpacePos);
    func_0201267c(0xf1, &this->mCamSpacePos);
    this->mState = 2;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113d20Ev
int daMky_c::func_ov030_02113d20()
{
    typedef int Fix12i;
    typedef short s16;

    extern void *gMonkeyRunAnimationFile;
    Player *p;
    Fix12i dist;
    s16 angle;
    int onWall;

    p = this->mPlayer;
    if (p == 0)
        p = this->ClosestPlayer();

    dist = Vec3_Dist(&this->mPos, &p->mPos);

    if ((IsUkikiStar(this) && Vec3_Dist((Vector3 *)&this->mPerchPosX, &p->mPos) > 0x514000) ||
        p->mPosY < this->mPerchPosY - 0x12c000) {
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

    ApproachLinear(this->mAngleY, angle, 0xa28);
    this->mPrevAngleY = this->mAngleY;
    if (this->mHorzSpeed != 0)
        this->mAnmModel.SetAnim((BCA_File *)(((void **)&gMonkeyRunAnimationFile)[1]), 0, 0x1000, 0);
    else
        func_ov030_02111a00();

    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();

    if (IsUkikiThief(this)) {
        UpdatePos(&this->mdCcAc_c);
        onWall = (func_ov030_02111dd0() | this->mWithMeshClsn.IsOnWall()) != 0;
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
    } else {
        UpdatePos(&this->mdCcAc_c);
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
        onWall = (func_ov030_02111ea4() | this->mWithMeshClsn.IsOnWall()) != 0;
    }

    if (this->mState == 1 && onWall && dist < 0x12c000) {
        if (p->mHorzSpeed > 0x9000 || p->IsInAir() != 0) {
            this->mPrevState = 1;
            func_ov030_021141a8(2);
        }
    }

    this->mdCcAc_c.Clear();
    if (this->mState == 1)
        this->mdCcAc_c.Update();
    func_ov030_02111890();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState1Ev
int daMky_c::EnterState1(){
    mSubState = 0;
    mState = 1;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02113ff0Ev
int daMky_c::func_ov030_02113ff0()
{
    typedef int Fix12i;
    typedef short s16;
    extern int gMonkeyRunAnimationFile[];
    int dist = this->DistToCPlayer();
    if (dist > 0x15e000) {
        this->mHorzSpeed = 0xc000;
        this->mAnmModel.SetAnim((BCA_File *)((void *)gMonkeyRunAnimationFile[1]), 0, 0x1000, 0);
    } else if (dist <= 0xfa000) {
        this->mHorzSpeed = 0;
        func_ov030_02111a00();
    }
    ApproachLinear(this->mAngleY, this->HorzAngleToCPlayer(), 0x514);
    this->mPrevAngleY = this->mAngleY;
    this->mAnmModel.speed = 0x1000;
    static_cast<dExtFrameCtrl_c &>(this->mAnmModel).Advance();
    if (IsUkikiThief(this)) {
        this->UpdatePos(&this->mdCcAc_c);
        func_ov030_02111dd0();
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
    } else {
        this->UpdatePos(&this->mdCcAc_c);
        func_ov030_02111f6c(&this->mWithMeshClsn);
        func_ov030_02111bc4();
        func_ov030_02111ea4();
    }
    this->mdCcAc_c.Clear();
    this->mdCcAc_c.Update();
    func_ov030_02111890();
    return 1;
}
// @symbol _ZN7daMky_c11EnterState0Ev
int daMky_c::EnterState0()
{
    mState = 0;
    return 1;
}

// @symbol _ZN7daMky_c19func_ov030_02114134Ev
void daMky_c::func_ov030_02114134()
{
    (this->*mStateDesc->update)();
}
// @symbol _ZN7daMky_c19func_ov030_02114170Ev
void daMky_c::func_ov030_02114170()
{
    (this->*mStateDesc->enter)();
}
// @symbol _ZN7daMky_c19func_ov030_021141a8Ei
void daMky_c::func_ov030_021141a8(int idx)
{
    extern StateFunctions data_ov030_02115e0c[];

    this->mStateDesc = &data_ov030_02115e0c[idx];
    func_ov030_02114170();
}
/* File-scope extern "C" for the six virtuals below. A block-scope extern
 * inside a daMky_c method gets C++ linkage and the reference mangles a
 * second time, so these callees are declared here. The func_ov030_*
 * methods above are members of this class and are not redeclared. */
extern "C" {

extern SharedFilePtr  data_ov002_0210da40;
extern SharedFilePtr  data_ov002_0210d9a0;
extern SharedFilePtr  data_ov002_0210d9c0;
extern SharedFilePtr  data_ov030_02115d00;
extern SharedFilePtr *data_ov030_02114824[10];
extern void          *data_ov030_02115cf0[];

}

// @symbol _ZN7daMky_c16CleanupResourcesEv
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

// @symbol _ZN7daMky_c16OnPendingDestroyEv
/* Vtable slot 12.  The ROM body is one `bx lr`: the override exists only to
 * occupy the slot. */
void daMky_c::OnPendingDestroy()
{
}

// @symbol _ZN7daMky_c6RenderEv
/* Vtable slot 9. */
s32 daMky_c::Render()
{
    if (IsInYoshiMouth(this))
        return 1;
    mAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

// @symbol _ZN7daMky_c8BehaviorEv
/* Vtable slot 6. */
s32 daMky_c::Behavior()
{
    if (IsTooFarAwayFromPlayer(0x5dc000) != 0 && mState != 8) {
        if (IsUkikiThief(this) && mHasSpawnedCap == 0 && SaveData::HasPlayerLostCap() != 0) {
            Player *pl = ClosestPlayer();
            unsigned cp = pl->param1;
            if (cp < 3) {
                dActor_c *spawned;
                mCapPlayerNo = cp;
                spawned = Spawn(0x10d, (mCapPlayerNo << 8) | 2, mPos, 0, mAreaId, -1);
                mCapUniqueID = spawned->uniqueID;
                mHasSpawnedCap = 1;
                func_ov030_021141a8(1);
            }
        }
        func_ov030_02111734();
    } else {
        func_ov030_02114134();
        mAnmModel.UpdateVerts();
        func_ov030_02112094();
    }
    return 1;
}

// @symbol _ZN7daMky_c13InitResourcesEv
/* Vtable slot 0. */
s32 daMky_c::InitResources()
{
    int i;

    dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov030_02115d00), 1, 1);
    for (i = 0; i < 10; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov030_02114824[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mAnmModel.SetAnim((BCA_File *)data_ov030_02115cf0[1], 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mdCcAc_c.Init(this, 0x28000, 0x64000, 0x800004, 0x49000);
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mPerchPosX = mPosX;
    mPerchPosY = mPosY;
    mPerchPosZ = mPosZ;
    mPerchPosY += 0x64000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mPlayer = 0;
    mCapUniqueID = 0;
    mResetTimer = 0x96;
    if (IsUkikiThief(this)) {
        if (SaveData::HasPlayerLostCap() != 0) {
            Player *player = ClosestPlayer();
            if (player->param1 < 3) {
                dActor_c *spawned;
                mCapPlayerNo = player->param1;
                spawned = Spawn(0x10d, (mCapPlayerNo << 8) | 2, mPos, 0, mAreaId, -1);
                mCapUniqueID = spawned->uniqueID;
                mHasSpawnedCap = 1;
                func_ov030_021141a8(1);
            } else {
                func_ov030_021141a8(0);
            }
        } else {
            func_ov030_021141a8(0);
        }
    } else if (IsUkikiStar(this)) {
        func_ov030_021141a8(1);
    }
    func_ov030_02112094();
    return 1;
}

// @symbol _ZN7daMky_c13OnTurnIntoEggER6Player
/* Vtable slot 19.  The cartridge's twelve bytes at this address are a linker
 * long-branch veneer to _ZN7fBase_c18MarkForDestructionEv in arm9 -- r0 still
 * holds `this` at the jump and the Player& is dropped -- so the source form
 * that reproduces it is a one-line forwarding member with no return. */
void daMky_c::OnTurnIntoEgg(Player &player)
{
    MarkForDestruction();
}

/* Reconstructed source-style name: SM64DS proves daMky_c through RTTI,
 * allocation size, vtable identity, and the MONKEY_STAR registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UkikiStar_Spawn. */
// @symbol daMky_c_classInit_MONKEY_STAR
extern "C" daMky_c *daMky_c_classInit_MONKEY_STAR()
{
    return new daMky_c();
}

/* Reconstructed source-style name: SM64DS proves daMky_c through RTTI,
 * allocation size, vtable identity, and the MONKEY_THIEF registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: UkikiThief_Spawn. */
// @symbol daMky_c_classInit_MONKEY_THIEF
extern "C" daMky_c *daMky_c_classInit_MONKEY_THIEF()
{
    return new daMky_c();
}

