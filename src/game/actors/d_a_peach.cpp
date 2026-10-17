//cpp
/* Princess Peach in the castle courtyard (PEACH_PRINCESS). Five state
 * pairs. SetState selects one, and each state writes 0..4 into mStateValue.
 * defer_codegen off: D1 then D0, no D2. Source follows the ROM.
 */
// leftovers:
// - func_ov085_02129f8c stays a free function taking char *. The cartridge
//   does not name it, and it is the one body here that is not a method.
// - ModelAnim::SetAnim stays mangled. The ModelAnim and dExtFrameCtrl_c
//   views are reached by cast because both take Fix12<int> by value.
// - Offsets 0xa4 and 0xac are dActor_c's unk_0a4 and unk_0ac. Offset 0xe8
//   sits inside the model's ModelComponents and is still read through words[].
#include "common.h"
#include "nitro/fx/fx.h"
#include "daPeach_c.h"
#include "dActor_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dExtFrameCtrl_c.h"
#include "dCc_c.h"
#include "types.h"
#include "dBgCh_Actr.h"

bool ApproachLinear(short &value, short target, short step);

struct BMD_File;
struct Vector3_16;

/* ModelAnim's fourth virtual slot, reached through a four-slot stand-in.
 * Behavior calls it on the ModelAnim at +0xd4; the real declaration lives
 * with that class, and this file only needs the dispatch. */
struct PeachAnimSlots {
    virtual int g0();
    virtual int g1();
    virtual int g2();
    virtual int g3();
};

/* The twelve-word body of a Matrix4x3, copied whole in and out of the
 * scratch matrix. */
typedef struct { int w[12]; } PeachM48;

extern "C" {

/* math / vector helpers */
extern int AngleDiff(int, int);
extern int Vec3_HorzDist(const void *a, const void *b);
extern short Vec3_HorzAngle(const void *a, const void *b);
extern short Vec3_VertAngle(const void *a, const void *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
extern int data_020a0e68[];

/* mesh collision */
extern void dBgCh_Actr_UpdateContinuous_Veneer(void *c);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *a, int b, int c, Vector3_16 *d, Vector3_16 *e);

/* actor plumbing */
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *mtx, int f, int g, unsigned int h);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *self, dActor_c *a, int b, int c, unsigned int d, unsigned int e);

/* model / animation */
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *self, void *bca, int frame, int speed, unsigned int flags);

/* ov085 statics. data_ov085_0212f280 and data_ov085_021304f4 keep
 * include/decl_common.h's spelling, so this file adds no new reading of
 * either symbol. */
extern int data_ov085_0212f280[];      /* the seven animation files */
extern char data_ov085_021304f4;       /* the model file */
extern void *data_ov085_021304bc[];
extern int data_ov085_021304c4[];
extern void *data_ov085_021304d4[];
extern int data_ov085_021304e4[];
extern void *data_ov085_021304ec[];
extern daPeach_c::StateEntry data_ov085_0213055c[];
}

#pragma defer_codegen off

// @symbol _ZN9daPeach_cD1Ev
// @symbol _ZN9daPeach_cD0Ev
/* ONE declaration, TWO ROM functions. mwccarm emits the complete variant D1
 * and the deleting variant D0 from the single destructor this class declares,
 * so D0 has no source of its own; with deferred code generation off they land
 * in the cartridge's order, D1 first at 0x02129d18 and D0 at 0x02129d60.
 *
 * There is nothing to write. The vptr store and the four member destructor
 * calls -- dBgCh_Actr at 0x194, dCcAc_c at 0x160, dExtShadowModel_c at 0x138,
 * ModelAnim at 0xd4 -- and then dActor_c's own step are every one a
 * consequence of `struct daPeach_c : dActor_c` and the members that
 * declaration types. D0's deallocation is the inline operator delete reached
 * through fBase_c, which is why nothing here mentions a heap.
 *
 * daPeach_c declares its destructor in the class body. The two functions
 * below demand D1 and D0, in that order: a destructor call, then a
 * delete-expression. Nothing calls either one, so the link drops them.
 * include/daPeach_c.h stays as it is. */
void PeachDemandCompleteDtor(daPeach_c *peach)
{
    peach->~daPeach_c();
}

void PeachDemandDeletingDtor(daPeach_c *peach)
{
    delete peach;
}

// @symbol _ZN9daPeach_c12UpdateLookAtEv
/* Peach turns her head toward the nearest player, but only when he is close
 * enough (0x15e000) and roughly in front of her (within 0x3000 of her
 * facing). Otherwise both target angles fall back to zero and she looks
 * straight ahead. The two ApproachLinear calls make the turn gradual. */
void daPeach_c::UpdateLookAt()
{
    Player *player = ClosestPlayer();
    if (player == 0)
        return;
    Vector3 target;
    Vector3 *playerPos = (Vector3 *)(((long long)(int)&player->mPosX));
    target = *playerPos;
    int dist = Vec3_HorzDist(&mPosX, &target);
    target.y = target.y - 0x1e000;
    short yaw = Vec3_HorzAngle(&mPosX, &target);
    short pitch = Vec3_VertAngle(&mPosX, &target);
    if (dist < 0x15e000 && AngleDiff(yaw, mAngleY) < 0x3000) {
        mTargetLookVertAngle = pitch;
        mTargetLookHorzAngle = mAngleY - yaw;
    } else {
        mTargetLookVertAngle = 0;
        mTargetLookHorzAngle = 0;
    }
    ApproachLinear(mLookHorzAngle, mTargetLookHorzAngle, 0x250);
    ApproachLinear(mLookVertAngle, mTargetLookVertAngle, 0x100);
}

// @symbol _ZN9daPeach_c21UpdateGroundCollisionEP10dBgCh_Actr
/* The floor normal becomes a pitch at +0xa8 so Peach stands square on a
 * slope. The wall branch reads its normal and drops it -- the ROM computes
 * the copy and uses nothing of it. */
void daPeach_c::UpdateGroundCollision(dBgCh_Actr *clsn)
{
    int *words = (int *)this;    /* 0xa4 and 0xac are dActor_c's unk_0a4 / unk_0ac */
    int floorN[3];
    int wallN[3];
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        ((SurfaceInfo *)((char *)clsn->GetFloorResult() + 4))->CopyNormalTo(*(Vector3 *)floorN);
        if (floorN[1] != 0) {
            int x = FX_MUL(floorN[0], words[0xa4 / 4]);
            int z = FX_MUL(floorN[2], words[0xac / 4]);
            mVertSpeed = -(_ZN4cstd4fdivEii(x + z, floorN[1]) + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        ((SurfaceInfo *)((char *)clsn->GetWallResult() + 4))->CopyNormalTo(*(Vector3 *)wallN);
    }
}

// @symbol func_ov085_02129f8c
/* Whether the actor whose id sits in mCylinder.otherOwner (+0x184) still
 * exists and is kind 0xbf. The one member of this TU the cartridge does not
 * name: a free function with C linkage, called by InitState0 and
 * InitState4. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov085_02129f8c(char *c) {
    unsigned int id = *(unsigned int *)(c + 0x184);
    void *actor;
    unsigned short kind;
    int alive;
    if (id == 0) return id;
    actor = dActor_c::FindWithID(id);
    if (actor == 0) return (int)actor;
    kind = ((dActor_c *)actor)->actorID;
    if (kind == 0xbf) alive = 1; else alive = 0;
    /* Both paths return alive. The cartridge still compares it with zero
     * first; a single return lets the compiler delete that compare. */
    if (alive == 0)
        goto done;
    return alive;
done:
    return alive;
}
}

// @symbol _ZN9daPeach_c11UpdateModelEv
/* The model matrix at +0xf0 is rebuilt from the body angle and the position,
 * then the head joint's own matrix (+0x360 inside the animated model) is
 * rotated by the two look angles through the scratch matrix at
 * data_020a0e68. */
void daPeach_c::UpdateModel()
{
    char *c = (char *)this;
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;
    *(PeachM48 *)data_020a0e68 = *(PeachM48 *)(*(char **)(c + 0xe8) + 0x360);
    Matrix4x3_ApplyInPlaceToRotationY(data_020a0e68, mLookHorzAngle);
    Matrix4x3_ApplyInPlaceToRotationZ(data_020a0e68, mLookVertAngle);
    *(PeachM48 *)(*(char **)(c + 0xe8) + 0x360) = *(PeachM48 *)data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mModelAnim.mat4x3, 0x8c000, 0x32000, 0xf);
}

// @symbol _ZN9daPeach_c10InitState0Ev
int daPeach_c::InitState0()
{
    UpdatePos((dCc_c *)&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    func_ov085_02129f8c((char *)this);
    return 1;
}

// @symbol _ZN9daPeach_c6State0Ev
int daPeach_c::State0()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim, data_ov085_021304ec[1], 0, 0x1000, 0);
    ((dExtFrameCtrl_c *)&mModelAnim)->SetFlags(0x40000000);
    mHorzSpeed = 0x4000;
    mVertSpeed = 0xa000;
    mStateValue = 4;
    return 1;
}

// @symbol _ZN9daPeach_c6State2Ev
int daPeach_c::State2()
{
    return 1;
}

// @symbol _ZN9daPeach_c10InitState2Ev
int daPeach_c::InitState2()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim, (void *)data_ov085_021304e4[1], 0, 0x1000, 0);
    mHorzSpeed = 0x4000;
    mStateValue = 3;
    return 1;
}

// @symbol _ZN9daPeach_c10InitState4Ev
int daPeach_c::InitState4()
{
    short angY = mAngleY;
    mPrevAngleY = angY;
    UpdatePos((dCc_c *)&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    func_ov085_02129f8c((char *)this);
    return 1;
}

// @symbol _ZN9daPeach_c6State1Ev
int daPeach_c::State1()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim, (void *)data_ov085_021304c4[1], 0, 0x1000, 0);
    mHorzSpeed = 0x1800;
    mStateValue = 2;
    return 1;
}

// @symbol _ZN9daPeach_c6State3Ev
/* The conversation, in three steps held in mTalkState: start the talk, turn
 * to face the player and put the message up, then wait for the player's talk
 * state to go back to -1. */
int daPeach_c::State3()
{
    Vector3 msgPos;
    switch (mTalkState) {
    case 0:
        if (mTalkPlayer->StartTalk(*this, 1) != 0)
            mTalkState += 1;
        break;
    case 1:
        if (ApproachLinear(mAngleY, Vec3_HorzAngle(&mPos, &mTalkPlayer->mPos), 0x514) != 0) {
            msgPos.x = mPosX;
            msgPos.y = mPosY;
            msgPos.z = mPosZ;
            msgPos.y = msgPos.y + 0xa0000;
            if (mTalkPlayer->ShowMessage(*this, 0xd0, &msgPos, 0, 0) != 0)
                mTalkState += 1;
        }
        break;
    case 2:
        if (mTalkPlayer->GetTalkState() == -1)
            SetState(0);
        break;
    }
    return 1;
}

// @symbol _ZN9daPeach_c10InitState1Ev
int daPeach_c::InitState1()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim, data_ov085_021304d4[1], 0, 0x1000, 0);
    ((dExtFrameCtrl_c *)&mModelAnim)->currFrame = 0;
    ((dExtFrameCtrl_c *)&mModelAnim)->Advance();
    mTalkState = 1;
    mStateValue = 1;
    return 1;
}

// @symbol _ZN9daPeach_c10InitState3Ev
/* The same liveness question func_ov085_02129f8c asks, written out here
 * instead of called -- the ROM inlines it at this one site. */
int daPeach_c::InitState3()
{
    if (mCylinder.hitFlags & 0x8000000) {
        char *a = (char *)dActor_c::FindWithID(mCylinder.otherOwner);
        if (a) {
            int match = (((dActor_c *)a)->actorID == 0xbf) ? 1 : 0;
            if (match != 0) {
                mTalkPlayer = (Player *)a;
                if (mTalkPlayer->StartTalk(*this, false)) {
                    SetState(1);
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN9daPeach_c6State4Ev
int daPeach_c::State4()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)&mModelAnim, (void *)data_ov085_021304bc[1], 0, 0x1000, 0);
    mStateValue = 0;
    return 1;
}

// @symbol _ZN9daPeach_c17CallStateBehaviorEv
/* The behavior half of the selected row. The table rows are pairs of
 * pointers-to-member copied out of the ten ROM constants at 0x0212ff34. */
void daPeach_c::CallStateBehavior()
{
    (this->*mState->behavior)();
}

// @symbol _ZN9daPeach_c13CallStateInitEv
void daPeach_c::CallStateInit()
{
    (this->*mState->init)();
}

// @symbol _ZN9daPeach_c8SetStateEi
void daPeach_c::SetState(int state)
{
    mState = &data_ov085_0213055c[state];
    CallStateInit();
}

// @symbol _ZN9daPeach_c16CleanupResourcesEv
/* One direct release, then a LOOP over a seven-entry table of pointers.
 * Unlike every sibling in this overlay, which writes its releases out one per
 * line, Peach's animation files are reached through data_ov085_0212f280 and
 * freed by index. Seven is the count the ROM's `blt` tests against. */
int daPeach_c::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_021304f4))->Release();
    s32 i = 0;
    do {
        ((SharedFilePtr *)data_ov085_0212f280[i])->Release();
        i++;
    } while (i < 7);
    return 1;
}

// @symbol _ZN9daPeach_c16OnPendingDestroyEv
/* Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself. */
void daPeach_c::OnPendingDestroy()
{
}

// @symbol _ZN9daPeach_c6RenderEv
/* Qualified Model::Render. ModelAnim overrides the slot. */
int daPeach_c::Render()
{
    mModelAnim.Model::Render(0);
    return 1;
}

// @symbol _ZN9daPeach_c8BehaviorEv
int daPeach_c::Behavior()
{
    CallStateBehavior();
    UpdateLookAt();
    if (mStateValue != 1)
        ((dExtFrameCtrl_c *)((dExtFrameCtrl_c *)&mModelAnim))->Advance();
    ((PeachAnimSlots *)((char *)&mModelAnim))->g3();
    ((dCc_c *)&mCylinder)->Clear();
    ((dCc_c *)&mCylinder)->Update();
    UpdateModel();
    return 1;
}

// @symbol _ZN9daPeach_c13InitResourcesEv
/* Declared by final name, not as members: both Init calls take Fix12<int>
 * where these calls pass int literals, and Fix12<int> is an aggregate with no
 * converting constructor from int. dBgCh_Actr::Init's last parameter is a
 * Vector3_16* as well -- the S5_ in ...P10Vector3_16S5_ back-references the
 * pointer type, it is not an int. */
int daPeach_c::InitResources()
{
    void *f = Model::LoadFile(*(SharedFilePtr *)&data_ov085_021304f4);
    ((ModelBase *)&mModelAnim)->SetFile((BMD_File *)f, 1, -1);
    for (int i = 0; i < 7; i++)
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov085_0212f280[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x90000, 0xc0000, 0x4800004, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x40000, 0x40000, (Vector3_16 *)0, (Vector3_16 *)0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    SetState(0);
    UpdateModel();
    return 1;
}

// @symbol daPeach_c_classInit
/* PEACH_PRINCESS factory. */
extern "C" daPeach_c *daPeach_c_classInit(void)
{
    return new daPeach_c();
}
