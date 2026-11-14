//cpp
/* Bob-omb Buddy (RED_BOMBHEI), ov084 .text 0x0212c10c..0x0212d248,
 * twenty-three functions.
 *
 * Stands in the level turning to face the nearest player. When a player walks
 * into its talk cylinder it starts a conversation. The red-coin variant picks
 * its message from the red coins collected; the cannon variants, while the
 * level's cannon is still closed, run a short cutscene that swings the camera
 * over to the cannon shutter, opens it, waits for it to finish and swings the
 * camera back.
 *
 * NAME: daRedBombhei_c is the cartridge's RTTI spelling (_ZTI14daRedBombhei_c
 * at 0x021309f4, _ZTS at 0x02130a00, vtable address point 0x02130a38). The
 * tree called the class BobOmbBuddy until then. daRedBombhei_c_classInit is
 * reconstructed (RTTI daRedBombhei_c, RED_BOMBHEI registry); retail does not
 * store that spelling. Historical alias: BobOmbBuddy_Spawn.
 *
 * Behavior runs one of three states out of a pointer-to-member table in .bss
 * at 0x02130dc4 that this module's static initializer (__sinit_ov084_02130558)
 * fills from the .data records at 0x021309c4. Each state is an {enter, update}
 * pair:
 *
 *   0 EnterIdle / UpdateIdle  idle: wait for a talk
 *   1 EnterTalk / UpdateTalk  talk
 *   2 EnterCannonOpening / UpdateCannonOpening  open the cannon
 *
 * Source order is the reverse of the ROM. The factory is written first.
 */
/* leftovers: data_ov084_02130dc4 keeps its address name.
 */

#include "daRedBombhei_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "dBgCh_Gnd.h"
#include "SaveData.h"
#include "daObjCannonShutter_c.h"
#include "Sound.h"
#include "math/Approach.h"
#include "SpawnInfo.h"
#include "private/mtx43.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"

struct BCA_File;

/* The one whole-matrix copy below goes through Mtx43. The translation-vector
   spelling in math/Matrix.h lowers the same assignment to two ldm/stm pairs
   plus three word copies; the cartridge has three pairs. */

enum {
    kPlayerActorID        = 0xbf,
    kCannonShutterActorID = 0x0e,
};

/* The low byte of the spawn X angle picks what this buddy does. */
enum {
    kVariantCannonPan = 1,  /* opens the cannon; the camera pans to the shutter */
    kVariantCannonCut = 2,  /* opens the cannon; the camera cuts to the shutter */
    kVariantRedCoins  = 3,  /* explains red coins */
};

enum {
    kStateIdle   = 0,
    kStateTalk   = 1,
    kStateCannon = 2,
};

/* The three character-specific spawn parameters: a buddy placed with one of
   these only appears for Mario, Luigi or Wario respectively. */
enum {
    kParamMario = 0xb26,
    kParamLuigi = 0xb27,
    kParamWario = 0xb28,
};

/* Behavior calls the current state through this member-pointer table. */
typedef void (daRedBombhei_c::*RedBombheiStatePMF)();
struct RedBombheiState {
    RedBombheiStatePMF enter;
    RedBombheiStatePMF update;
};

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daRedBombhei_c_AssetId { ASSET_RED_BOMBHEI_BMD = 0x02b9, ASSET_RED_WAIT_BCA = 0x02ba };

/* The handles reach their files through these wrappers' constructor and
   destructor veneers; the manifest maps each to the retail destination.
   Descriptive local types, not recovered spellings. */
struct RedBombheiModelFile : SharedFilePtr {
    RedBombheiModelFile(u32 fileID);
    ~RedBombheiModelFile();
};

struct RedBombheiAnimFile : SharedFilePtr {
    RedBombheiAnimFile(u32 fileID);
    ~RedBombheiAnimFile();
};

/* This TU's file handles and state table, in the order the retail
   initializer (0x02130558) builds them. */
RedBombheiModelFile gRedBombheiModelFile(ASSET_RED_BOMBHEI_BMD);
RedBombheiAnimFile gRedBombheiWaitAnimFile(ASSET_RED_WAIT_BCA);

RedBombheiState data_ov084_02130dc4[3] = {
    {&daRedBombhei_c::EnterIdle, &daRedBombhei_c::UpdateIdle},
    {&daRedBombhei_c::EnterTalk, &daRedBombhei_c::UpdateTalk},
    {&daRedBombhei_c::EnterCannonOpening, &daRedBombhei_c::UpdateCannonOpening},
};

extern "C" {

/* -- globals -- */
extern dCamera_c *gCamera;                 /* the camera */
extern s8 gCurrentSublevel;                /* current level */
extern u8 gCurrentStarId;
extern u8 gShowTouchScreenArrows;
extern u8 gShowRedCoinMarkers;
extern u8  data_0209d660;               /* nonzero while a message box is open */
extern u8  data_0209d6bc;               /* message box page state */
extern u16 data_0209d6d4;               /* message on screen */
extern Matrix4x3 data_020a0e68;         /* the shared scratch matrix */
extern Matrix4x3 IDENTITY_MATRIX4X3;

/* -- other modules -- */
int  IsCannonOpenInCurLevel(void);
void OpenCannonInCurLevel(void);
int ObjectMessageIDToActualMessageID(s16);
int  SublevelToLevel(int sublevel);

void  Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);
void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
void  Vec3_MulScalarInPlace(Vector3 *v, int s);
void MTX_MultVec43(Vector3 *v, Matrix4x3 *m, Vector3 *out);
void  Matrix4x3_FromRotationY(void *m, int angle);
int   Math_Function_0203b14c(int *value, int target, int a, int b, int c);


}
int  IsStarCollected(int level, int star);
s8   NumRedCoins(void);
void func_02012694(unsigned int soundID, const Vector3 *camSpacePos);

/* This int-returning predicate is what makes the ROM materialize 0/1
 * before testing it. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == kPlayerActorID;
}

daRedBombhei_c *daRedBombhei_c_classInit()
{
    return new daRedBombhei_c();
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daRedBombhei_c> g_profile_RED_BOMBHEI = {
    daRedBombhei_c_classInit, /* classInit */
    0x00b5,                   /* behaviorPriority */
    0x002e,                   /* renderPriority */
    0x00000003,               /* actorFlags */
    0x40000,                  /* clipOffsetY */
    0x80000,                  /* clipRadius */
    0x01000000,               /* clipDistance */
    0x01000000                /* farDistance */
};

s32 daRedBombhei_c::InitResources()
{
    Vector3 pos;

    BMD_File *modelFile = dExtSimpleModel_c::LoadFile(gRedBombheiModelFile);
    mAnmModel.SetFile(modelFile, 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(gRedBombheiWaitAnimFile);
    mdCcAc_c.Init(this, 0x8c000, 0x8c000, 0x4200004, 0);
    ChangeState(0);
    mShutterID = 0;

    {
        int z = mPosZ;
        int y = mPosY + 0x64000;
        pos.x = mPosX;
        pos.y = y;
        pos.z = z;
    }

    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn() != 0)
        mPosY = ground.clsnY;

    if (IsCharacterSpecific() != 0) {
        u8 character = ClosestPlayer()->mCharacter;
        if ((character == 0 && param1 == kParamMario) ||
            (character == 1 && param1 == kParamLuigi) ||
            (character == 2 && param1 == kParamWario) || SaveData::HasPlayerLostCap())
            return 0;
    }

    if (IsCannonRole() != 0 && gCurrentSublevel == 8 &&
        (gCurrentStarId == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0))
        return 0;
    return 1;
}

/* Runs the state, keeps turning toward a player within 300.0, and plays
   sound 0xd7 at the start of each animation loop. */
s32 daRedBombhei_c::Behavior()
{
    Vector3 playerPos;
    daPly_c *player;
    UpdateState();
    player = ClosestPlayer();
    if (player != 0) {
        Vector3 *src = &player->mPos;
        playerPos = *src;
        if (Vec3_HorzDist(&mPos, &playerPos) < 0x12c000) {
            short angle = Vec3_HorzAngle(&mPos, &playerPos);
            ApproachLinear(mAngleY, angle, 0x100);
        }
    }
    mAnmModel.Advance();
    if ((unsigned short)(mAnmModel.currFrame >> 12) == 0)
        func_02012694(0xd7, &mCamSpacePos);
    UpdateModelTransform();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

s32 daRedBombhei_c::Render()
{
    mAnmModel.Render(0);
    return 1;
}

s32 daRedBombhei_c::CleanupResources()
{
    gRedBombheiModelFile.Release();
    gRedBombheiWaitAnimFile.Release();
    return 1;
}

/* Places the model and drops the shadow under the body. */
void daRedBombhei_c::UpdateModelTransform() {
    Matrix4x3_FromRotationY(&this->mAnmModel.mat4x3, this->mAngleY);
    this->mAnmModel.mat4x3.t.x = this->mPosX >> 3;
    this->mAnmModel.mat4x3.t.y = (this->mPosY + 0x4000) >> 3;
    this->mAnmModel.mat4x3.t.z = this->mPosZ >> 3;
    *(Mtx43 *)&this->mShadowMat = *(Mtx43 *)&IDENTITY_MATRIX4X3;
    this->mShadowMat.t.x = this->mPosX >> 3;
    this->mShadowMat.t.y = (this->mPosY - 0x8000) >> 3;
    this->mShadowMat.t.z = this->mPosZ >> 3;
    DropShadowRadHeight(this->mShadowModel, this->mShadowMat, 0x64000, 0x32000, 0xf);
}

/* Moves the camera point *cur one step toward target, easing the distance
   between them down toward zero. Returns 1 once it has arrived. */
int daRedBombhei_c::StepCameraPoint(Vector3 *cur, Vector3 *target) {
    Vector3 delta;
    int len;
    int remaining;
    int newLen;
    Vec3_Sub(&delta, cur, target);
    len = VEC_Mag(&delta);
    if (len == 0) return 1;
    newLen = len;
    remaining = Math_Function_0203b14c(&newLen, 0, 0x400, 0x100000, 0x1000);
    Vec3_MulScalarInPlace(&delta, FX_Div(newLen, len));
    {
        Vector3 next;
        Vec3_Add(&next, target, &delta);
        cur->x = next.x;
        cur->y = next.y;
        cur->z = next.z;
    }
    return remaining == 0;
}

/* Cutscene step 3: pan the camera back to where it was before the cutscene.
   Returns 1 once both points have arrived. */
int daRedBombhei_c::PanCameraBack() {
    dCamera_c *cam = gCamera;
    int posDone;
    Vector3 lookAt;
    Vector3 pos;
    {
        const Vector3 &l = cam->lookAt;
        lookAt = l;
    }
    {
        const Vector3 &p = cam->pos;
        pos = p;
    }
    int lookAtDone = StepCameraPoint(&lookAt, &this->mSavedCamLookAt);
    posDone = StepCameraPoint(&pos, &this->mSavedCamPos);
    if ((this->mAngleX & 0xff) == kVariantCannonCut) {
        lookAtDone = 1;
        posDone = 1;
        cam->SetLookAt(this->mSavedCamLookAt);
        cam->SetPos(this->mSavedCamPos);
    } else {
        cam->SetLookAt(lookAt);
        cam->SetPos(pos);
    }
    if (lookAtDone != 0 && posDone != 0) return 1;
    return 0;
}

/* Cutscene step 1: pan the camera over to the cannon shutter -- looking at a
   point 128.0 up and 10.0 behind it from 512.0 up and 200.0 in front. Returns 1
   once both points have arrived. */
int daRedBombhei_c::PanCameraToShutter() {
    int posDone, lookAtDone;
    dCamera_c *cam;
    dActor_c *shutter;
    Vector3 lookAt, pos, targetLookAt, targetPos, offset, sum, sum2;
    unsigned id = this->mShutterID;
    cam = gCamera;
    if (id != 0) {
        shutter = dActor_c::FindWithID(id);
        if (shutter != 0) {
            targetLookAt.x = 0;
            targetLookAt.y = 0x80000;
            targetLookAt.z = -0xa000;
            targetPos.x = 0;
            targetPos.y = 0x200000;
            targetPos.z = 0xc8000;
            offset.x = 0;
            offset.y = 0;
            offset.z = 0;
            Matrix4x3_FromRotationY(&data_020a0e68, shutter->mAngleY);
            MTX_MultVec43(&targetLookAt, &data_020a0e68, &offset);
            Vec3_Add(&sum, &shutter->mPos, &offset);
            targetLookAt = sum;
            offset.x = 0;
            offset.y = 0;
            offset.z = 0;
            MTX_MultVec43(&targetPos, &data_020a0e68, &offset);
            Vec3_Add(&sum2, &shutter->mPos, &offset);
            targetPos = sum2;
            {
                const Vector3 &l = cam->lookAt;
                lookAt = l;
            }
            {
                const Vector3 &p = cam->pos;
                pos = p;
            }
            lookAtDone = StepCameraPoint(&lookAt, &targetLookAt);
            posDone = StepCameraPoint(&pos, &targetPos);
            if ((this->mAngleX & 0xff) == kVariantCannonCut) {
                lookAtDone = 1;
                posDone = 1;
                cam->SetLookAt(targetLookAt);
                cam->SetPos(targetPos);
            } else {
                cam->SetLookAt(lookAt);
                cam->SetPos(pos);
            }
        }
    }
    /* With no shutter, both flags are read uninitialized; the ROM does the same. */
    return lookAtDone != 0 && posDone != 0;
}

/* Is this the buddy that opens the cannon? */
int daRedBombhei_c::IsCannonRole() {
    int variant = this->mAngleX;
    int result = 1;
    variant = variant & 0xff;
    if (variant == kVariantCannonPan) return result;
    if (variant != kVariantCannonCut) result = 0;
    return result;
}

/* Is this the red-coin tutor? */
int daRedBombhei_c::IsRedCoinRole() {
    return (this->mAngleX & 0xff) == kVariantRedCoins;
}

/* Was this buddy placed for one character only? */
int daRedBombhei_c::IsCharacterSpecific() {
    int result = 1;
    int isMarioOrLuigi = result;
    int param = this->param1;
    if (param != kParamMario) {
        if (param != kParamLuigi)
            isMarioOrLuigi = 0;
    }
    if (!isMarioOrLuigi) {
        if (param != kParamWario)
            result = 0;
    }
    return result;
}

/* Opens message msgID on the talking player's screen, with the talk sound. */
void daRedBombhei_c::ShowTalkMessage(int msgID, u8 msgFlag) {
    daPly_c *player = this->mTalkPlayer;
    Vector3 pos;
    int x = this->mPosX;
    int z = this->mPosZ;
    int y = this->mPosY + 0x32000;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    func_02012694(0x108, &this->mCamSpacePos);
    player->ShowMessage(*this, (s16)msgID, &pos, msgFlag, 0);
}

void daRedBombhei_c::UpdateState() {
    int j = this->mState;
    (this->*data_ov084_02130dc4[j].update)();
}

void daRedBombhei_c::ChangeState(int state) {
    this->mState = state;
    int j = this->mState;
    (this->*data_ov084_02130dc4[j].enter)();
}

void daRedBombhei_c::EnterIdle() {
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)gRedBombheiWaitAnimFile.filePtr;
    this->mAnmModel.SetAnim(file, 0, 0x1000, flags);
}

/* State 0 update: start talking when a player walks into the talk cylinder. */
void daRedBombhei_c::UpdateIdle() {
    dActor_c *other;
    if ((this->mdCcAc_c.hitFlags & 0x8000000) == 0)
        return;
    other = dActor_c::FindWithID(this->mdCcAc_c.otherOwner);
    if (other == 0)
        return;
    if (!IsPlayerActor(other))
        return;
    this->mTalkPlayer = (daPly_c *)other;
    if (this->mTalkPlayer->StartTalk(*this, false) == 0)
        return;
    ChangeState(kStateTalk);
}

/* State 1 enter. */
void daRedBombhei_c::EnterTalk() {
    this->mMsgPage = 0;
    this->mPrevMsgPageState = 0;
}

/* State 1 update: turn to the player, open the right message, and track the
   message box to light the hint arrow on the pages that point somewhere. */
void daRedBombhei_c::UpdateTalk() {
    daPly_c *player = this->mTalkPlayer;
    int msgID = 0;
    int param = this->param1;
    s16 angle;
    Vector3 playerPos;
    int msg;
    u16 shownMsg;
    if (param != 0xffff)
        msgID = (u16)param;
    {
        int *p = (int *)(unsigned)&player->mPosX;
        int a = p[0];
        playerPos.x = a;
        int b = p[1];
        playerPos.y = b;
        int c = p[2];
        playerPos.z = c;
    }
    angle = Vec3_HorzAngle(&this->mPos, &playerPos);

    if (IsCannonRole() != 0) {
        if (IsCannonOpenInCurLevel() == 0) {
            ChangeState(kStateCannon);
            return;
        }
    }

    switch (player->GetTalkState()) {
    case 0:
        if (ApproachLinear(this->mAngleY, angle, 0x800) != 0) {
            if (IsRedCoinRole() != 0) {
                if ((data_0209caa0.file.flags2 & 0x8000) == 0) {
                    data_0209caa0.file.flags2 |= 0x8000;
                    msg = 0x15b;
                } else if (NumRedCoins() <= 3) {
                    msg = 0x15c;
                } else if (NumRedCoins() <= 6) {
                    msg = 0x15d;
                } else if (NumRedCoins() == 7) {
                    msg = 0x15e;
                } else {
                    msg = 0x15f;
                }
                ShowTalkMessage(msg, 0);
                gShowRedCoinMarkers = 1;
            } else if (IsCannonRole() != 0 && IsCannonOpenInCurLevel() != 0) {
                if (this->mCannonOpened == 0) {
                    if (gCurrentSublevel == 6)
                        msg = 0x8f;
                    else
                        msg = 0x14a;
                } else {
                    if (gCurrentSublevel == 6)
                        msg = 0x90;
                    else
                        msg = 0x14b;
                }
                this->mCannonOpened = 1;
                ShowTalkMessage(msg, 0);
            } else {
                msg = ObjectMessageIDToActualMessageID(msgID);
                msg = msg + player->param1;
                ShowTalkMessage((u16)msg, 0);
            }
        }
        break;
    case 1:
        break;
    default:
        ChangeState(kStateIdle);
        break;
    }

    if (data_0209d660 == 0)
        return;

    if (data_0209d6bc == 9) {
        if (IsRedCoinRole() != 0)
            gShowRedCoinMarkers = 0;
    }

    {
        u8 pageState = data_0209d6bc;
        if (this->mPrevMsgPageState != pageState) {
            if (pageState == 3 || pageState == 9) {
                this->mMsgPage++;
            }
        }
    }

    shownMsg = data_0209d6d4;
    if (shownMsg == 0x15c || shownMsg == 0x15e || IsCharacterSpecific() != 0) {
        if (this->mMsgPage == 0)
            gShowTouchScreenArrows = 1;
        else
            gShowTouchScreenArrows = 0;
    }

    if (shownMsg == 0x15b || shownMsg == 0x15d) {
        if (this->mMsgPage == 1)
            gShowTouchScreenArrows = 1;
        else
            gShowTouchScreenArrows = 0;
    }

    if (shownMsg == 0x8a) {
        if (this->mMsgPage == 1)
            gShowTouchScreenArrows = 1;
        if (data_0209d6bc == 9)
            gShowTouchScreenArrows = 0;
    }

    if (this->mMsgHint != gShowTouchScreenArrows && gShowTouchScreenArrows != 0)
        func_02012790(0x24);

    this->mMsgHint = gShowTouchScreenArrows;
    this->mPrevMsgPageState = data_0209d6bc;
}

/* State 2 enter: find this level's closed cannon shutter. */
void daRedBombhei_c::EnterCannonOpening() {
    dActor_c *actor;
    actor = dActor_c::FindWithActorID(kCannonShutterActorID, 0);
    while (actor) {
        if ((actor->param1 & 0xff) == 1) {
            if (((daObjCannonShutter_c *)actor)->mCannonOpen == 0) {
                this->mShutterID = actor->uniqueID;
            }
        }
        actor = dActor_c::FindWithActorID(kCannonShutterActorID, actor);
    }
    this->mCutsceneStep = 0;
}

/* State 2 update: the cannon-opening cutscene, one step at a time. */
void daRedBombhei_c::UpdateCannonOpening() {
    daPly_c *player = this->mTalkPlayer;
    dCamera_c *cam = gCamera;
    Vector3 playerPos;
    s16 angle;
    int *src = &player->mPosX;

    playerPos.x = src[0];
    playerPos.y = src[1];
    playerPos.z = src[2];
    angle = Vec3_HorzAngle(&this->mPos, &playerPos);

    switch (this->mCutsceneStep) {
    case 0:
        /* Show the cannon message; on talk state 2 save the camera and take it. */
        switch (player->GetTalkState()) {
        case 0: {
            int msg;
            if (ApproachLinear(this->mAngleY, angle, 0x800) == 0)
                return;
            msg = gCurrentSublevel == 6 ? 0x8f : 0x14a;
            if (this->mShutterID != 0)
                ShowTalkMessage(msg, 1);
            else
                ShowTalkMessage(msg, 0);
            return;
        }
        case 2: {
            int *lookAt = (int *)&cam->lookAt;
            int *pos = (int *)&cam->pos;
            this->mSavedCamLookAt.x = lookAt[0];
            this->mSavedCamLookAt.y = lookAt[1];
            this->mSavedCamLookAt.z = lookAt[2];
            this->mSavedCamPos.x = pos[0];
            this->mSavedCamPos.y = pos[1];
            this->mSavedCamPos.z = pos[2];
            cam->SetFlag_3();
            this->mFlags &= ~1;
            this->mCutsceneStep += 1;
            return;
        }
        case -1:
            OpenCannonInCurLevel();
            this->mCannonOpened = 1;
            ChangeState(kStateIdle);
            return;
        }
        return;
    case 1: {
        /* dCamera_c over to the shutter, then open it. */
        dActor_c *shutter;
        if (this->mShutterID == 0)
            return;
        if (PanCameraToShutter() == 0)
            return;
        shutter = dActor_c::FindWithID(this->mShutterID);
        if (shutter == 0)
            return;
        ((daObjCannonShutter_c *)shutter)->StartOpening();
        this->mCutsceneStep += 1;
        return;
    }
    case 2: {
        /* Wait for the shutter to finish opening. */
        unsigned int id = this->mShutterID;
        if (id == 0)
            return;
        if (((daObjCannonShutter_c *)dActor_c::FindWithID(id))->mCannonOpen == 1)
            this->mCutsceneStep += 1;
        return;
    }
    case 3:
        /* dCamera_c back. */
        if (PanCameraBack() != 0)
            this->mCutsceneStep += 1;
        return;
    case 4: {
        /* Finish the talk and hand the camera back. */
        int talkState = player->GetTalkState();
        if (talkState == 1)
            return;
        if (talkState == 2) {
            int msg = gCurrentSublevel == 6 ? 0x90 : 0x14b;
            ShowTalkMessage(msg, 0);
            return;
        }
        this->mFlags |= 1;
        OpenCannonInCurLevel();
        this->mCannonOpened = 1;
        cam->mFlags &= ~8;
        ChangeState(kStateIdle);
        return;
    }
    }
}

/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */
