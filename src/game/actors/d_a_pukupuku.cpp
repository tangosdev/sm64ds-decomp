//cpp
/* daPukupuku_c -- Cheep Cheep (PUKUPUKU).
 *
 * Swims out from its spawn point, pauses, then swims again. InitResources
 * records the home position and installs the swim state. Behavior handles
 * Yoshi, steps the state timer, runs the current state, and hurts Mario on
 * contact. The swim state picks a random heading and speed; if it gets too
 * far from home it turns back. When the timer expires it pauses (speed 0),
 * then starts swimming again.
 *
 * common.h is first on purpose. Matrix4x3 is either common.h's flat s32 m[12]
 * or math/Matrix.h's nested {r, t}, and the first include wins. The nested
 * form splits the model-matrix copy in UpdateModelMatrix. Nothing here
 * names .r or .t.
 *
 * Source order is the reverse of the ROM. mwccarm emits one .text section per
 * function in reverse source order. Do not reorder. The inline destructor in
 * daPukupuku_c.h emits D1 then D0 from `return new`; g_profile_PUKUPUKU stays
 * outside this TU.
 *
 * The state record is 16 bytes, filled by __sinit_ov090_02133f4c from the
 * four 8-byte pointer-to-member words at 0x021342b8. Enter is at +0 and is
 * called by SetState; main is at +8 and is called by Behavior.
 * data_ov090_02134594 is swim (InitStateSwim, StateSwim).
 * data_ov090_02134584 is pause (InitStatePause, StatePause).
 * PukuStatePMF is declared while PukuStateC is still incomplete: that is the
 * 8-byte member pointer the ROM table stores (function, this-delta 0).
 *
 * What stays:
 * - Render: `if ((mFlags & 0x40000) != 0) return 1` shrinks the function
 *   0x50 -> 0x44. The compare has to land in an int (`int b = ((mFlags &
 *   0x40000) != 0); if (b)`). mAnmModel.Render(0) itself matches.
 * - StatePause and StateSwim: `mStateTimer == 0` is ldrsh. The ROM is ldrh,
 *   so the test is `*(unsigned short *)&mStateTimer`. One word in
 *   StatePause.
 * - SetState calls through PukuStateC, the incomplete class the state PMF
 *   type is declared against.
 */

#include "common.h"
#include "daPukupuku_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dExtFrameCtrl_c.h"
#include "decl_common.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* Behavior calls the main PMF, which sits 8 bytes into the state record. */
typedef void (dEnemyBase_c::*PukuMainPMF)();
struct PukuMainView {
    char enter[8];
    PukuMainPMF main;
};

/* File-scope objects at the end of this file construct the two resource
 * handles and fill the two state rows. mwcc emits
 * __sinit_d_a_pukupuku.cpp from those definitions. The wrapper names are
 * local; the handle constructors and destructors are the ROM resource-family
 * functions, aliased in the manifest. */
struct PukupukuModelFile : SharedFilePtr {
    PukupukuModelFile(u32 fileID);
    ~PukupukuModelFile();
};

struct PukupukuAnimationFilePtr : SharedFilePtr {
    PukupukuAnimationFilePtr(u32 fileID);
    ~PukupukuAnimationFilePtr();
};

/* One 8-byte state word (a pointer-to-member: function address, this-delta).
 * The four words live in .data; each state row copies two of them. */
struct PukuDesc {
    int w[2];
};

struct PukuStateRow {
    PukuDesc x, y;
};

extern PukupukuModelFile data_ov090_02134564;
extern PukupukuAnimationFilePtr data_ov090_0213455c;
extern PukuDesc data_ov090_021342c0;
extern PukuDesc data_ov090_021342b8;
extern PukuDesc data_ov090_021342d0;
extern PukuDesc data_ov090_021342c8;
extern PukuStateRow data_ov090_02134594;
extern PukuStateRow data_ov090_02134584;

extern Vector3 data_ov090_021342d8;
extern int data_0209e650[];

/* local extern: math/CalcLib.h leaves Vec3_Asr out. */
extern "C" void Vec3_Asr(void *dst, void *src, int n);

// @symbol daPukupuku_c_classInit
extern "C" {
int *daPukupuku_c_classInit(void) {
    return (int *)new daPukupuku_c;
}
}

// @symbol _ZN12daPukupuku_c13InitResourcesEv
int daPukupuku_c::InitResources() {
    struct BMD_File *bmd;
    struct Vector3 v;

    bmd = (struct BMD_File *)dExtSimpleModel_c::LoadFile(data_ov090_02134564);
    mAnmModel.SetFile(bmd, 1, -1);

    dExtFrameCtrl_c::LoadFile(data_ov090_0213455c);

    v = data_ov090_021342d8;
    mdCcAcPos_c.Init(this, v, 0x32000, 0x3c000, 0x200004, 0x8000);

    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mAngleY = mPrevAngleY;
    mWithMeshClsn.Init(this, 0x1e000, 0x1e000, 0, 0);

    mAnmModel.SetAnim((BCA_File *)data_ov090_0213455c.filePtr, 0, 0x1000, 0);

    SetState((PukuStatePMF *)&data_ov090_02134594);
    return 1;
}

// @symbol _ZN12daPukupuku_c8BehaviorEv
int daPukupuku_c::Behavior() {
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAcPos_c.Clear();
        if (mEatenByYoshi != 0) {
            if (mSpitOutTimer == 0) {
                mdCcAcPos_c.Update();
            }
        }
        UpdateModelMatrix();
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    UpdatePos(&mdCcAcPos_c);
    {
        PukuMainView *q = (PukuMainView *)mState;
        if (q->main != 0)
            (((dEnemyBase_c *)this)->*(q->main))();
    }
    mAngleY = mPrevAngleY;
    mAnmModel.speed = 0x1000;
    static_cast<dExtFrameCtrl_c &>(mAnmModel).Advance();
    UpdateModelMatrix();
    CheckPlayerHit();
    mdCcAcPos_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAcPos_c.Update();
        }
    }
    return 1;
}

// @symbol _ZN12daPukupuku_c6RenderEv
int daPukupuku_c::Render() {
    int b = ((mFlags & 0x40000) != 0);
    if (b)
        return 1;
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN12daPukupuku_c16OnPendingDestroyEv
void daPukupuku_c::OnPendingDestroy() {}

// @symbol _ZN12daPukupuku_c16CleanupResourcesEv
int daPukupuku_c::CleanupResources() {
    data_ov090_02134564.Release();
    data_ov090_0213455c.Release();
    return 1;
}

// @symbol _ZN12daPukupuku_c17UpdateModelMatrixEv
/* Model matrix: position (>> 3, Vec3_Asr) and the X/Y/Z angles, built in the
 * scratch matrix and copied out. */
void daPukupuku_c::UpdateModelMatrix() {
    int v[3];
    Vec3_Asr(v, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mAnmModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN12daPukupuku_c8SetStateEPM10PukuStateCFivE
/* Install a state record and run its enter routine, if it has one. */
struct PukuStateC {
    char pad[0x370];
    PukuStatePMF *pp;
};
int daPukupuku_c::SetState(PukuStatePMF *p) {
    mState = p;
    PukuStatePMF *q = mState;
    if (*q == 0)
        return 1;
    return (((PukuStateC *)this)->**q)();
}

// @symbol _ZN12daPukupuku_c13InitStateSwimEv
/* Swim enter: a random heading (one of sixteen), 50..113 frames, speed 5.0. */
int daPukupuku_c::InitStateSwim() {
    unsigned int r = RandomIntInternal(data_0209e650);
    mTargetAngleY = ((r >> 8) & 0xf) << 0xc;
    r = RandomIntInternal(data_0209e650);
    mStateTimer = (s16)(((r >> 8) & 0x3f) + 0x32);
    mHorzSpeed = 0x5000;
    return 1;
}

// @symbol _ZN12daPukupuku_c9StateSwimEv
/* Swim main: past 1000.0 from home, head back home for 50 frames; turn toward
 * mTargetAngleY; pause when the timer runs out. */
int daPukupuku_c::StateSwim() {
    if (Vec3_Dist(&mPos, &mHomePos) > 0x3e8000) {
        *(unsigned short *)&mStateTimer = 0x32;
        mTargetAngleY = Vec3_HorzAngle(&mPos, &mHomePos);
    }
    ApproachAngle(&mPrevAngleY, mTargetAngleY, 1, 0x100, 0x200);
    if (*(unsigned short *)&mStateTimer == 0)
        SetState((PukuStatePMF *)&data_ov090_02134584);
    return 1;
}

// @symbol _ZN12daPukupuku_c14InitStatePauseEv
/* Pause enter: stop for 50..113 frames. */
int daPukupuku_c::InitStatePause() {
    unsigned int r = RandomIntInternal(data_0209e650);
    mStateTimer = (s16)(((r >> 8) & 0x3f) + 0x32);
    mHorzSpeed = 0;
    return 1;
}

// @symbol _ZN12daPukupuku_c10StatePauseEv
/* Pause main: swim again when the timer runs out. */
int daPukupuku_c::StatePause() {
    if (*(unsigned short *)&mStateTimer == 0) {
        SetState((PukuStatePMF *)&data_ov090_02134594);
    }
    return 1;
}

// @symbol _ZN12daPukupuku_c14CheckPlayerHitEv
/* Move the hit cylinder to its body offset, then hurt the player touching
 * it unless the player is vanished. */
void daPukupuku_c::CheckPlayerHit() {
    Vector3 v;
    v.x = data_ov090_021342d8.x;
    v.y = data_ov090_021342d8.y;
    v.z = data_ov090_021342d8.z;
    mdCcAcPos_c.SetPosRelativeToActor(v);
    {
        unsigned int id = mdCcAcPos_c.otherOwner;
        if (id == 0)
            return;
        {
            Player *a = (Player *)dActor_c::FindWithID(id);
            int b = (int)(a->actorID == 0xbf);
            if (b == 0)
                return;
            if (a->mIsVanish != 0)
                return;
            {
                Vector3 hv;
                hv.x = mPosX;
                hv.y = mPosY;
                hv.z = mPosZ;
                a->Hurt(hv, 1, 0xc000, 1, 0, 1);
            }
        }
    }
}

// @symbol _ZN12daPukupuku_cD0Ev
/* D0 is emitted from ~daPukupuku_c() in the header. */

// @symbol _ZN12daPukupuku_cD1Ev
/* D1 is emitted from the same inline destructor. */

/* Static-init globals (was the handwritten __sinit_ov090_02133f4c shard).
 * Definition order is the retail initializer's construction order: the model
 * handle (file 0x418), then the animation handle (file 0x419), then the swim
 * row (InitStateSwim, StateSwim) and the pause row (InitStatePause,
 * StatePause). */
PukupukuModelFile data_ov090_02134564(0x418);
PukupukuAnimationFilePtr data_ov090_0213455c(0x419);
PukuStateRow data_ov090_02134594 = {data_ov090_021342c0, data_ov090_021342b8};
PukuStateRow data_ov090_02134584 = {data_ov090_021342d0, data_ov090_021342c8};
