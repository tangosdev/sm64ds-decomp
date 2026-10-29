//cpp
/* daKinopio_c -- Toad (KINOPIO), ov085.
 *
 * He idles until a Mario (actor 0xbf) touches him, then turns to face the
 * player and talks. His head tracks the closest player and he fades out at a
 * distance. A Toad tied to a star (low byte of mAngleX) whose star is still
 * uncollected, while the player owns the star count data_ov085_0212f27c asks
 * for, says his base line, spawns POWER_STAR 0xb2 after the talk and only
 * then moves on to his next line. If the star is already collected or the
 * player has too few stars, InitResources starts him on the next line
 * instead. Variant 1 keeps a companion actor 0x10d keyed to the nearest
 * player's character.
 *
 * leftovers:
 * - common.h before math/Matrix.h is load-bearing: the flat m[12]
 *   Matrix4x3 keeps the destructor group at D1, D0, D2 where the
 *   cartridge keeps D1 below D0 and no D2 (measured note below).
 * - Source is ROM-ascending under defer_codegen off; the destructor is
 *   header-declared and defined out of line (D1 then D0, no D2).
 * - The opt_propagation off brackets on St_Talk_Main and InitResources
 *   and the qualified `mAnmModel.dExtSimpleModel_c::Render(0)` are the matched
 *   form.
 * - The companion Spawn passes mPos itself (a real Vector3 local changes
 *   codegen, unlike the star Spawn's).
 * - Vec3_* helpers, func_020138dc / func_02013a44,
 *   ObjectMessageIDToActualMessageID, IsStarCollectedInCurLevel,
 *   NumStars and data_0209f2f8 / data_0209caa0 keep linker names; the
 *   ov002 file handles stay address-named.
 * - MatrixWords keeps UpdateModelPose's aggregate 12-word copies in
 *   ldm/stm form; `char *c` offset spelling stays where the ROM reads
 *   untyped fields (mTalkPlayer, the model internals at +0x11c, the
 *   spawned actor's words).
 */
/* common.h FIRST, and the order is load-bearing. Both it and math/Matrix.h
 * define Matrix4x3 behind one shared guard -- flat `s32 m[12]` here,
 * `Matrix3x3 r; Vector3 t;` there -- and whichever the TU reaches first
 * stands. The structured spelling pulls in Vector3's destructor, and with
 * two Matrix4x3 members that makes mwccarm emit D2, D0, D1; the cartridge
 * has D1 below D0 and no D2. Measured: math/Matrix.h first emits D2, D0,
 * D1; common.h first emits D1, D0, D2, which links in cartridge order
 * once the homeless D2 is discarded. */
#include "common.h"
#include "daKinopio_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Message.h"
#include "dCc_c.h"
#include "dBgCh_Gnd.h"
#include "math/Approach.h"
#include "SpawnInfo.h"

struct BCA_File;
struct BMD_File;

/* The pointer-to-member state table, defined after the profile below.
   mwccarm leaves a file-scope variable's name unmangled in C++, so this
   names the ROM symbol as spelled. */
extern daKinopio_c::State data_ov085_0212fe88[2];

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daKinopio_c_AssetId {
    ASSET_KINOPIO_BMD = 0x035c,
    ASSET_KINOPIO_WAIT1_BCA = 0x035d,
    ASSET_KINOPIO_WAIT2_BCA = 0x035e
};

/* The handles reach their files through these wrappers' constructor and
   destructor veneers; the manifest maps each to the retail destination.
   Descriptive local types, not recovered spellings. */
struct KinopioModelFile : SharedFilePtr {
    KinopioModelFile(u32 fileID);
    ~KinopioModelFile();
};

struct KinopioAnimFile : SharedFilePtr {
    KinopioAnimFile(u32 fileID);
    ~KinopioAnimFile();
};

/* This TU's file handles, in the order the retail initializer (0x0212f2a8)
   builds them. */
KinopioModelFile gKinopioModelFile(ASSET_KINOPIO_BMD);
KinopioAnimFile gKinopioIdleAnimFile(ASSET_KINOPIO_WAIT1_BCA);
KinopioAnimFile gKinopioTalkAnimFile(ASSET_KINOPIO_WAIT2_BCA);

namespace Sound {
bool PlaySub(u32, u32, u32, int, bool);
}

extern "C" {

/* ov085 statics */
extern u8 data_ov085_0212f27c[];           /* star-count requirement table */

/* ov002 statics -- the three models the variant-1 Toad preloads */
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;

/* arm9 statics */
extern s8 data_0209f2f8;
extern int data_0209caa0[];
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 IDENTITY_MATRIX4X3;

/* math / vector helpers. (ApproachLinear, from math/Approach.h, returns whether
   it arrived, and that is load-bearing: St_Talk_Main tests the result to decide
   whether the turn has finished.) */
s16 Vec3_HorzAngle(const void *a, const void *b);
s16 Vec3_VertAngle(const void *a, const void *b);
s32 Vec3_HorzDist(const void *a, const void *b);
s32 AngleDiff(s32 a, s32 b);
void Matrix4x3_FromRotationY(void *m, int angY);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 ang);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(void *a, void *b, void *out);

/* game state */
int func_020138dc(void);
int func_02013a44(void);
u16 ObjectMessageIDToActualMessageID(s16 id);
int IsStarCollectedInCurLevel(int starID);
u8 NumStars(void);

/* dActor_c / Sound.
   The mangled spellings are kept wherever the real declaration takes a
   Fix12<int> BY VALUE, because mwccarm passes those differently at
   the call site than a loose spelling would. */
int d_a_kinopioPlaySubHold(u32 a, u32 b, u32 c, int d, int e);
}

/* St_Talk_Main's legacy source used this to keep an address computation from
   folding into the surrounding expression; it is carried verbatim. */
#define M(p) (p)

/* Actor ID 0xbf is PLAYER. This int-returning predicate is what makes the
 * ROM materialize 0/1 before testing it. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

#pragma defer_codegen off

// @symbol _ZN11daKinopio_cD1Ev
// @symbol _ZN11daKinopio_cD0Ev
/* ONE definition, TWO ROM functions (D1 at 0x02129020, D0 at 0x02129060;
 * the D2 the compiler also emits is homeless and discarded). The empty
 * body is one vptr store plus the member destructors in reverse
 * declaration order, then dActor_c's own; D0 adds the inline operator
 * delete through dActor_c. */
daKinopio_c::~daKinopio_c()
{
}

// @symbol _ZN11daKinopio_c12GetMessageIDEv
/* Which line of dialogue this Toad says. The player's param1 (+8) shifts
 * the id per character; the 0x32 mode overrides it entirely. */
u16 daKinopio_c::GetMessageID()
{
    int *player = (int *)mTalkPlayer;
    u16 messageID = mMessageID;

    if (data_0209f2f8 == 0x32) {
        int state = func_020138dc();
        switch (state) {
        case 0x1c:
            return 0x134;
        case 0: {
            int message = ObjectMessageIDToActualMessageID((s16)messageID);
            int sum = message + player[2];
            return (u16)sum;
        }
        default:
            return 0x133;
        }
    }

    if (mVariant == 1 && func_02013a44() != 0) {
        int playerParam = player[2];
        int message = 0xb0a;
        message += playerParam;
        return ObjectMessageIDToActualMessageID((s16)(u16)message);
    }

    int message = ObjectMessageIDToActualMessageID((s16)messageID);
    int sum = message + player[2];
    return (u16)sum;
}

// @symbol _ZN11daKinopio_c12St_Talk_MainEv
/* This member's legacy file carried `#pragma opt_propagation off`. That pragma
   is file-global last-wins in mwccarm, so it is bracketed here rather than left
   to leak into the members below; `defer_codegen off` is what makes the bracket
   bind to the definition it encloses. */
#pragma opt_propagation off
void daKinopio_c::St_Talk_Main()
{
    char *c = (char *)this;
    char *player;
    int messageID;
    s16 angleToPlayer;
    Vector3 playerPos;
    Vector3 messagePos;
    Vector3 starPos;
    int talkState;

    player = (char *)mTalkPlayer;
    messageID = GetMessageID();
    {
        int *pos = (int *)(int)M(player + 0x5c);
        playerPos.x = pos[0];
        playerPos.y = pos[1];
        playerPos.z = pos[2];
    }
    angleToPlayer = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &playerPos);
    talkState = ((Player *)player)->GetTalkState();
    switch (talkState) {
    case 0: {
        if (ApproachLinear(mAngleY, angleToPlayer, 0x800) == 0)
            return;
        if (data_0209f2f8 == 0x32) {
            if ((data_0209caa0[1] & 0x40000000) == 0)
                data_0209caa0[1] |= 0x40000000;
        }
        if (Sound::PlaySub(0x25, 0x14, 0x7f, 0x15666, 1) == 0)
            return;
        {
            int y = mPosY;
            int z = mPosZ;
            int x;
            y += 0x46000;
            messagePos.x = mPosX;
            messagePos.y = y;
            messagePos.z = z;
            (void)x;
        }
        ((Player *)player)->ShowMessage(*this, (u32)(s16)messageID, &messagePos, 0, 0);
        return;
    }
    case 1:
        return;
    default:
        break;
    }

    if (mTalkFinished == 0) {
        mTalkFinished = 1;
        if (mStarID != 0xff && IsStarCollectedInCurLevel(mStarID) == 0) {
            if (NumStars() >= data_ov085_0212f27c[mStarReqIndex]) {
                starPos.x = mPosX;
                starPos.y = mPosY;
                starPos.z = mPosZ;
                starPos.y += 0xc8000;
                dActor_c::Spawn(
                    0xb2, mStarID | 0x20, starPos, 0, mAreaId, -1);
                u16 *message = (u16 *)(int)M(c + 0x208);
                *message = (u16)(*message + 1);
            }
        }
    }

    if (mVariant == 1 && func_02013a44() != 0) {
        u32 id = mCapUniqueID;
        if (id != 0) {
            char *found = (char *)dActor_c::FindWithID(id);
            if (found != 0) {
                ((fBase_c *)found)->MarkForDestruction();
                mCapUniqueID = 0;
                SpawnSoundObj(1);
                {
                    u32 param = 0x13;
                    u8 character = ((Player *)player)->mCharacter;
                    dActor_c::Spawn(
                        0x10d, param | ((u32)character << 8),
                        playerPos, 0, mAreaId, -1);
                }
            }
        }
    }

    if (Sound::PlaySub(0x25, 0x7f, 0, 0x7222, 0) == 0)
        return;
    Message::EndTalk();
    SetState(0);
}
#pragma opt_propagation on

// @symbol _ZN11daKinopio_c12St_Talk_InitEv
void daKinopio_c::St_Talk_Init()
{
    u32 flags = 0;
    BCA_File *file = (BCA_File *)gKinopioTalkAnimFile.filePtr;
    mAnmModel.SetAnim((BCA_File *)file, 0, 0x1000, flags);
}

// @symbol _ZN11daKinopio_c12St_Idle_MainEv
void daKinopio_c::St_Idle_Main()
{
    char *self = (char *)this;
    if ((*(int *)(self + 0xf4) & 0x08000000) == 0)
        return;

    void *actor = dActor_c::FindWithID(*(u32 *)(self + 0xf8));
    if (actor == 0)
        return;
    if (!IsPlayerActor((dActor_c *)actor))
        return;

    mTalkPlayer = (Player *)actor;
    if (mTalkPlayer->StartTalk(*this, 0) == 0)
        return;

    Message::PrepareTalk();
    SetState(1);
}

// @symbol _ZN11daKinopio_c12St_Idle_InitEv
void daKinopio_c::St_Idle_Init()
{
    mAnmModel.SetAnim((BCA_File *)gKinopioIdleAnimFile.filePtr, 0, 0x1000, 0);
}

// @symbol _ZN11daKinopio_c8SetStateEi
void daKinopio_c::SetState(s32 state)
{
    mState = state;
    s32 index = mState;
    (this->*data_ov085_0212fe88[index].init)();
}

// @symbol _ZN11daKinopio_c8RunStateEv
void daKinopio_c::RunState()
{
    s32 index = mState;
    (this->*data_ov085_0212fe88[index].main)();
}

// @symbol _ZN11daKinopio_c15UpdateModelPoseEv
/* An array-only wrapper keeps the C++ aggregate copies in the same ldm/stm form
 * the original C-shaped match used. It stays local to the member that recovered
 * it rather than being promoted to a shared type. */
struct MatrixWords {
    s32 words[12];
};

void daKinopio_c::UpdateModelPose()
{
    char *c = (char *)this;
    Matrix4x3 *dst;
    dActor_c *actor;
    u32 id;
    int x, y, z;

    Matrix4x3_FromRotationY(c + 0x124, mAngleY);
    *(s32 *)(c + 0x148) = mPosX >> 3;
    *(s32 *)(c + 0x14c) = (mPosY + 0x4000) >> 3;
    *(s32 *)(c + 0x150) = mPosZ >> 3;

    *(MatrixWords *)&data_020a0e68 =
        *(MatrixWords *)(*(char **)(c + 0x11c) + 0xf0);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mHeadYaw);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mHeadPitch);
    *(MatrixWords *)(*(char **)(c + 0x11c) + 0xf0) =
        *(MatrixWords *)&data_020a0e68;

    id = *(u32 *)(c + 0x1f4);
    if (id != 0) {
        actor = dActor_c::FindWithID(id);
        if (actor != 0) {
            *(s32 *)&x = 0;
            *(s32 *)&y = 0;
            *(s32 *)&z = 0;
            dst = (Matrix4x3 *)(*(char **)(c + 0x11c) + 0xf0);
            MulMat4x3Mat4x3(dst, c + 0x124, c + 0x194);

            *(MatrixWords *)&data_020a0e68 = *(MatrixWords *)(c + 0x194);
            Matrix4x3_ApplyInPlaceToTranslation(
                &data_020a0e68, 0, 0x4000, -0xa00);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(
                &data_020a0e68, -0x1000, 0, 0);
            *(MatrixWords *)(c + 0x194) = *(MatrixWords *)&data_020a0e68;

            x = ((s32 *)&data_020a0e68)[9];
            y = ((s32 *)&data_020a0e68)[10];
            z = ((s32 *)&data_020a0e68)[11];
            y <<= 3;
            x <<= 3;
            z <<= 3;
            actor->mPosX = x;
            actor->mPosY = y;
            actor->mPosZ = z;
            actor->mCarryMatrix = &mCapTransform;
        }
    }

    *(MatrixWords *)(c + 0x1c4) = *(MatrixWords *)&IDENTITY_MATRIX4X3;
    *(s32 *)(c + 0x1e8) = mPosX >> 3;
    *(s32 *)(c + 0x1ec) = (mPosY - 0x8000) >> 3;
    *(s32 *)(c + 0x1f0) = mPosZ >> 3;
    ((dActor_c *)c)
        ->DropShadowRadHeight(*(dExtShadowModel_c *)(c + 0x16c), *(Matrix4x3 *)(c + 0x1c4), 0x50000,
                              0x64000, 0xf);
}

// @symbol _ZN11daKinopio_c16CleanupResourcesEv
/* The three ov002 handles are released only by the variant that took them. The
 * legacy file reached all six through an `int[]` spelling and a cast; they are
 * the real SharedFilePtr objects, which is how InitResources below already
 * spelled them, so the cast is gone and the call is the ordinary member call. */
int daKinopio_c::CleanupResources()
{
    if (*(unsigned char *)((char *)&mVariant) == 1) {
        data_ov002_0210da40.Release();
        data_ov002_0210d9a0.Release();
        data_ov002_0210d9c0.Release();
    }
    gKinopioModelFile.Release();
    gKinopioIdleAnimFile.Release();
    gKinopioTalkAnimFile.Release();
    return 1;
}

// @symbol _ZN11daKinopio_c6RenderEv
/* One call. THE CALL IS QUALIFIED, AND THAT IS LOAD-BEARING. dExtSimpleModel_c::Render is
 * virtual (slot 5 of _ZTV17dExtSimpleModel_c) and dExtAnmModel_c overrides it, so a plain
 * `mAnmModel.Render(0)` emits the vtable dispatch -- three words where the ROM
 * has one `bl`. Naming the base explicitly suppresses the dispatch and
 * reproduces the cartridge exactly. The null argument is the scale, and passing
 * 0 is what makes this Toad render at his own size rather than a
 * caller-supplied one. */
int daKinopio_c::Render()
{
    mAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

// @symbol _ZN11daKinopio_c8BehaviorEv
/* This Toad watches the player and turns his head; everything else in the
 * function is bookkeeping around that.
 *
 * The head-turn is two smoothed axes: mHeadYaw chases mHeadYawTarget at 0x250 a
 * frame and mHeadPitch chases mHeadPitchTarget at 0x100, and the targets are
 * recomputed each frame from the player's direction. He only tracks while the
 * player is inside the distance threshold AND within 0x3000 of his facing; out
 * of range the targets go to zero and the head returns to centre.
 *
 * The threshold widens from 0xfa000 to 0x1f4000 when data_0209f2f8 is 0x32 -- a
 * mode in which he also slowly rotates his whole body toward the player rather
 * than just his head, and in which he never fades out. mAngleY is his facing,
 * and the ApproachLinear on it in that one branch is the only place the body
 * turns.
 *
 * The opacity ramp is the other half: mTargetOpacity is the target and mOpacity
 * the current value, stepped by 6 a frame, and the model's opacity is that value
 * shifted right by 3. Anything that should keep him visible -- being close,
 * having a live tracked actor, or the 0x32 mode -- writes 0xff into the
 * target. */
int daKinopio_c::Behavior()
{
    RunState();
    mAnmModel.UpdateVerts();

    Player *player = ClosestPlayer();
    if (player != 0) {
        s32 threshold = 0xfa000;
        if (data_0209f2f8 == 0x32)
            threshold = 0x1f4000;

        Vector3 *psrc = &player->mPos;
        Vector3 playerPos;
        playerPos.x = psrc->x;
        playerPos.y = psrc->y;
        playerPos.z = psrc->z;

        s32 horzDist = Vec3_HorzDist(&mPosX, &playerPos);

        Vector3 lookTarget;
        lookTarget.y = playerPos.y;
        lookTarget.x = playerPos.x;
        lookTarget.z = playerPos.z;
        lookTarget.y = playerPos.y + 0x1e000;

        s16 yawToPlayer = Vec3_HorzAngle(&mPosX, &lookTarget);
        s16 pitchToPlayer = Vec3_VertAngle(&mPosX, &lookTarget);

        if (horzDist < 0x190000)
            mTargetOpacity = 0xff;
        else
            mTargetOpacity = 0x3c;

        if (horzDist < threshold && AngleDiff(yawToPlayer, mAngleY) < 0x3000) {
            mHeadYawTarget = (s16)(yawToPlayer - mAngleY);
            mHeadPitchTarget = pitchToPlayer;
        } else if (data_0209f2f8 != 0x32) {
            mHeadYawTarget = 0;
            mHeadPitchTarget = 0;
        } else {
            ApproachLinear(mAngleY, yawToPlayer, 0x100);
            mHeadYawTarget = 0;
            mHeadPitchTarget = 0;
        }
    } else {
        mTargetOpacity = 0x3c;
    }

    {
        u32 id = mCapUniqueID;
        if (id != 0) {
            if (dActor_c::FindWithID(id) != 0)
                mTargetOpacity = 0xff;
        }
    }

    if (data_0209f2f8 == 0x32)
        mTargetOpacity = 0xff;

    ApproachLinear(mHeadYaw, mHeadYawTarget, 0x250);
    ApproachLinear(mHeadPitch, mHeadPitchTarget, 0x100);

    ApproachLinear2(*(s32 *)&mOpacity, mTargetOpacity, 6);

    mAnmModel.Advance();

    UpdateModelPose();

    mAnmModel.speed = 0x1000;

    mAnmModel.ApplyOpacity((u32)(u8)(mOpacity >> 3), 1);

    mCollider.Clear();
    mCollider.Update();

    return 1;
}

// @symbol _ZN11daKinopio_c13InitResourcesEv
/* Spawn-time setup, and most of it is unpacking the spawn parameter.
 *
 * mParam carries three things at once. The low half becomes mMessageID, the
 * dialogue id, with 0xffff meaning "none" and normalising to 0. The TOP byte of
 * mAngleX becomes mVariant, which selects a variant -- value 1 is the one that
 * preloads the three ov002 models and, in multiplayer, spawns a companion actor
 * 0x10d keyed to the nearest player's character. The LOW byte of mAngleX becomes
 * mStarID, a star index; 0xff means "not tied to a star".
 *
 * When it IS tied to a star, the dialogue id shifts by one unless the star is
 * already collected or the player has fewer stars than the table at
 * data_ov085_0212f27c requires -- which is how one Toad says different things
 * before and after you have earned what he is guarding.
 *
 * Finally he is dropped onto the ground: a dBgCh_Gnd is aimed 0x14000 above his
 * spawn point and, if it hits, mPosY is snapped to the surface. The local
 * dBgCh_Gnd is declared at that exact mid-function construction point, so the
 * compiler emits both its constructor and destructor naturally.
 *
 * `#pragma opt_propagation off` IS LOAD-BEARING and stays, bracketed: without it
 * mwcc propagates the reloaded mMessageID through the 0xffff test and the
 * function changes shape. */
#pragma opt_propagation off
int daKinopio_c::InitResources()
{
    Vector3 objPos;
    dExtFrameCtrl_c::LoadFile(gKinopioIdleAnimFile);
    dExtFrameCtrl_c::LoadFile(gKinopioTalkAnimFile);
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gKinopioModelFile), 1, 0x16);
    mShadowModel.InitCylinder();
    mCollider.Init(this, 0x78000, 0x8c000, 0x4200004, 0);
    mOpacity = 0xff;
    mTargetOpacity = 0xff;
    mMessageID = param1;
    mStarID = (u8)mAngleX;
    mVariant = (u8)(mAngleX >> 8);
    if (mMessageID == 0xffff)
        mMessageID = 0;
    mCapUniqueID = 0;
    if (mVariant == 1) {
        dExtSimpleModel_c::LoadFile(data_ov002_0210da40);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9a0);
        dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
        if (func_02013a44() != 0) {
            Player *player = ClosestPlayer();
            if (player != 0) {
                u32 k = 0xd;
                u8 character = player->mCharacter;
                s8 area = mAreaId;
                s16 m1 = -1;
                void *spawned =
                    dActor_c::Spawn(0x10d, k | ((u32)character << 8), mPos, 0, area, m1);
                if (spawned != 0)
                    mCapUniqueID = *(s32 *)((char *)spawned + 4);
            }
        }
    }
    if (mStarID != 0xff) {
        u16 message = mMessageID;
        switch (message) {
        case 0xaf5: mStarReqIndex = 0; break;
        case 0xaf7: mStarReqIndex = 1; break;
        case 0xaf9: mStarReqIndex = 2; break;
        }
        if (IsStarCollectedInCurLevel(mStarID) != 0 ||
            NumStars() < data_ov085_0212f27c[mStarReqIndex]) {
            u16 *pm = (u16 *)((char *)&mMessageID);
            *pm = (u16)(*pm + 1);
        }
    }
    SetState(0);
    objPos.x = mPosX;
    objPos.y = mPosY;
    objPos.z = mPosZ;
    objPos.y = objPos.y + 0x14000;
    {
        dBgCh_Gnd ground;
        ground.SetObjAndPos(objPos, 0);
        if (ground.DetectClsn())
            mPosY = ground.clsnY;
    }
    return 1;
}
#pragma opt_propagation on

// @symbol daKinopio_c_classInit
/* The registry factory behind the KINOPIO profile: `return new` allocates
 * 0x210 (this class's sizeof) and installs this class's vtable.
 * Reconstructed source-style name: SM64DS proves daKinopio_c through RTTI,
 * allocation size, vtable identity and the KINOPIO registry profile; later
 * EAD lineage supplies classInit. The exact original spelling is not
 * preserved. Historical alias: Toad_Spawn. */
extern "C" daKinopio_c *daKinopio_c_classInit(void)
{
    return new daKinopio_c();
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daKinopio_c> g_profile_KINOPIO = {
    daKinopio_c_classInit, /* classInit */
    0x00b9,                /* behaviorPriority */
    0x00ab,                /* renderPriority */
    0x00000003,            /* actorFlags */
    0x40000,               /* clipOffsetY */
    0x80000,               /* clipRadius */
    0x01000000,            /* clipDistance */
    0x01000000             /* farDistance */
};

/* SetState and RunState index this by mState: 0 idles, 1 talks. */
daKinopio_c::State data_ov085_0212fe88[2] = {
    {&daKinopio_c::St_Idle_Init, &daKinopio_c::St_Idle_Main, "WAIT"},
    {&daKinopio_c::St_Talk_Init, &daKinopio_c::St_Talk_Main, "TALK"},
};
