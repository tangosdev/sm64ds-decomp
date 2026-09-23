//cpp
/* The Toad NPC (KINOPIO registry): idle and talk states, head tracking,
 * star-guard dialogue, and a variant-1 companion spawn. 15 functions.
 *
 * Load-bearing orders, all measured: common.h before math/Matrix.h (the
 * flat Matrix4x3 spelling keeps the destructor group D1,D0); source
 * ROM-ascending under defer_codegen off; the destructor is declared in
 * the header and defined out of line below (D1 then D0, no D2).
 *
 * Leftover: math/vector helpers and game-state free functions keep
 *   linker names (no header homes).
 * Leftover: dBgW_KcMbg::SetFile, dCcAc_c::Init, ModelAnim::SetAnim,
 *   DropShadowRadHeight and Particle::System::NewSimple keep mangled
 *   spellings (Fix12<int> by value, wall 6az).
 * Leftover: the companion Spawn keeps its mangled spelling: forming
 *   a Vector3 reference over int storage is not free, unlike the
 *   star Spawn's real local.
 * Leftover: data_0209f2f8 / data_0209caa0 / data_0209f318 and the
 *   ov085/ov002 file homes keep linker names.
 */
/* common.h FIRST, and the order is load-bearing. Both it and math/Matrix.h
 * define Matrix4x3 behind one shared guard -- flat `s32 m[12]` here, and
 * `Matrix3x3 r; Vector3 t;` there -- and whichever a translation unit reaches
 * first stands. The structured spelling holds a Vector3, which declares its own
 * destructor, so Matrix4x3 acquires a compiler-declared one; this class has two
 * Matrix4x3 members, and synthesizing that member destructor while generating
 * this class's own makes mwccarm emit the variant group D2, D0, D1 instead of
 * D1, D0, D2. The cartridge has D1 at 0x02129020 below D0 at 0x02129060 and no
 * D2 at all, so the group must come out D1 first for the licensed pair to link
 * in cartridge order once the homeless D2 is discarded. Measured on this class:
 * with math/Matrix.h first the object emits D2, D0, D1; with common.h first it
 * emits D1, D0, D2. Neither spelling is named anywhere below -- the two members
 * are layout only -- and all fifteen functions reproduce their cartridge bytes
 * either way. */
#include "common.h"
#include "daKinopio_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Message.h"
#include "dCc_c.h"
#include "dBgCh_Gnd.h"

struct BCA_File;
struct BMD_File;

/* The pointer-to-member state table at 0x0212fe88. mwccarm leaves a file-scope
   variable's name unmangled in C++, so this names the ROM symbol as spelled. */
extern daKinopio_c::State data_ov085_0212fe88[];

extern "C" {

/* ov085 statics */
extern u8 data_ov085_0212f27c[];           /* star-count requirement table */
extern SharedFilePtr data_ov085_02130480;  /* body model      */
extern SharedFilePtr data_ov085_02130488;  /* idle animation  */
extern SharedFilePtr data_ov085_02130490;  /* talk animation  */

/* ov002 statics -- the three models the variant-1 Toad preloads */
extern SharedFilePtr data_ov002_0210da40;
extern SharedFilePtr data_ov002_0210d9a0;
extern SharedFilePtr data_ov002_0210d9c0;

/* arm9 statics */
extern s8 data_0209f2f8;
extern int data_0209caa0[];
extern Matrix4x3 data_020a0e68;
extern Matrix4x3 IDENTITY_MATRIX4X3;

/* math / vector helpers.
   `_Z14ApproachLinearRsss` returns int and that is load-bearing: St_Talk_Main
   tests the result to decide whether the turn has finished. */
s16 Vec3_HorzAngle(const void *a, const void *b);
s16 Vec3_VertAngle(const void *a, const void *b);
s32 Vec3_HorzDist(const void *a, const void *b);
s32 AngleDiff(s32 a, s32 b);
int _Z14ApproachLinearRsss(s16 *cur, s16 tgt, s16 step);
void _Z15ApproachLinear2Riii(s32 *cur, s32 tgt, s32 step);
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

/* Player / Message, reached by their ROM names */
int _ZN6Player12GetTalkStateEv(char *player);
int _ZN6Player9StartTalkER7fBase_cb(void *player, void *actor, char flag);
void _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(
    char *player, char *actor, u32 messageID, const Vector3 *pos, u32 a, u32 b);
int _ZN7Message11PrepareTalkEv(void);
void _ZN7Message7EndTalkEv(void);

/* dActor_c / fBase_c / Sound.
   The mangled spellings are kept wherever the real declaration takes a
   Fix12<int> or an s8/s16 BY VALUE, because mwccarm passes those differently at
   the call site than a loose spelling would. */
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(u32 a, u32 b, u32 c, int d, int e);
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 profile, u32 param, const void *pos, const void *rot, int area, int id);
void *_ZN8dActor_c10FindWithIDEj(u32 id);
void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
void _ZN8dActor_c13SpawnSoundObjEj(char *thiz, u32 id);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *thiz, void *sm, void *mtx, int rad, int height, u32 flags);
void _ZN7fBase_c18MarkForDestructionEv(char *thiz);

/* model / collision / shadow */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *thiz, BCA_File *file, int a, int speed, u32 flags);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, BMD_File *f, int a, int b);
BMD_File *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr *p);
BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr *p);
void _ZN11ShadowModel12InitCylinderEv(void *thiz);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *thiz, void *actor, Fix12i a, Fix12i b, u32 c, u32 d);

}

/* St_Talk_Main's legacy source used this to keep an address computation from
   folding into the surrounding expression; it is carried verbatim. */
#define M(p) (p)

#pragma defer_codegen off

/* ROM ordinals 0 and 1 -- _ZN11daKinopio_cD1Ev 0x02129020 size 0x40,
                          _ZN11daKinopio_cD0Ev 0x02129060 size 0x54 */
// @symbol _ZN11daKinopio_cD1Ev
// @symbol _ZN11daKinopio_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * ONE definition, TWO ROM functions. mwccarm emits the deleting variant D0 and
 * the complete variant D1 from this single declaration, so D0 has no source of
 * its own; with deferred code generation off they land in the cartridge's order,
 * D1 first at 0x02129020 and D0 at 0x02129060, and the base-object variant D2
 * the compiler also emits is homeless and discarded.
 *
 * The body is empty because there is nothing to write: one vptr store and three
 * member destructor calls, every one a consequence of
 * `struct daKinopio_c : dActor_c` and the members that declaration types --
 * ShadowModel (0x16c), ModelAnim (0x108) and dCcAc_c (0x0d4) in reverse
 * declaration order, then dActor_c's own. D0's deallocation is an inline
 * operator delete reached through dActor_c, this class's immediate base, which
 * is why nothing here mentions a heap. */
daKinopio_c::~daKinopio_c()
{
}

// @symbol _ZN11daKinopio_c12GetMessageIDEv
/* Which line of dialogue this Toad says. The player's own offset at +8 shifts
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
        int n = player[2];
        int message = 0xb0a;
        message += n;
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
    s16 angle;
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
    angle = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &playerPos);
    talkState = ((Player *)player)->GetTalkState();
    switch (talkState) {
    case 0: {
        if (_Z14ApproachLinearRsss(&mAngleY, angle, 0x800) == 0)
            return;
        if (data_0209f2f8 == 0x32) {
            if ((data_0209caa0[1] & 0x40000000) == 0)
                data_0209caa0[1] |= 0x40000000;
        }
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(
                0x25, 0x14, 0x7f, 0x15666, 1) == 0)
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
                _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0xb2, mStarID | 0x20, &starPos, 0, mAreaId, -1);
                u16 *message = (u16 *)(int)M(c + 0x208);
                *message = (u16)(*message + 1);
            }
        }
    }

    if (mVariant == 1 && func_02013a44() != 0) {
        u32 id = mCapUniqueID;
        if (id != 0) {
            char *found = (char *)_ZN8dActor_c10FindWithIDEj(id);
            if (found != 0) {
                _ZN7fBase_c18MarkForDestructionEv(found);
                mCapUniqueID = 0;
                SpawnSoundObj(1);
                {
                    u32 param = 0x13;
                    u8 character = ((Player *)player)->mCharacter;
                    _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                        0x10d, param | ((u32)character << 8),
                        &playerPos, 0, mAreaId, -1);
                }
            }
        }
    }

    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x25, 0x7f, 0, 0x7222, 0) == 0)
        return;
    Message::EndTalk();
    SetState(0);
}
#pragma opt_propagation on

// @symbol _ZN11daKinopio_c12St_Talk_InitEv
void daKinopio_c::St_Talk_Init()
{
    u32 flags = 0;
    BCA_File *file = (BCA_File *)(((int *)&data_ov085_02130490)[1]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, file, 0, 0x1000, flags);
}

// @symbol _ZN11daKinopio_c12St_Idle_MainEv
void daKinopio_c::St_Idle_Main()
{
    char *self = (char *)this;
    if ((*(int *)(self + 0xf4) & 0x08000000) == 0)
        return;

    void *actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(self + 0xf8));
    if (actor == 0)
        return;
    int isPlayer = (((dActor_c *)actor)->actorID == 0xbf) ? 1 : 0;
    if (!isPlayer)
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
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, ((BCA_File **)&data_ov085_02130488)[1], 0, 0x1000, 0);
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

/* ROM ordinal 9 -- _ZN11daKinopio_c15UpdateModelPoseEv, 0x021295bc,
                    size 0x22c */
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
    void *actor;
    u32 id;
    int x, y, z;

    Matrix4x3_FromRotationY(c + 0x124, *(s16 *)(c + 0x8e));
    *(s32 *)(c + 0x148) = *(s32 *)(c + 0x5c) >> 3;
    *(s32 *)(c + 0x14c) = (*(s32 *)(c + 0x60) + 0x4000) >> 3;
    *(s32 *)(c + 0x150) = *(s32 *)(c + 0x64) >> 3;

    *(MatrixWords *)&data_020a0e68 =
        *(MatrixWords *)(*(char **)(c + 0x11c) + 0xf0);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mHeadYaw);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mHeadPitch);
    *(MatrixWords *)(*(char **)(c + 0x11c) + 0xf0) =
        *(MatrixWords *)&data_020a0e68;

    id = *(u32 *)(c + 0x1f4);
    if (id != 0) {
        actor = _ZN8dActor_c10FindWithIDEj(id);
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
            *(s32 *)((char *)actor + 0x5c) = x;
            *(s32 *)((char *)actor + 0x60) = y;
            *(s32 *)((char *)actor + 0x64) = z;
            *(void **)((char *)actor + 0xc8) = c + 0x194;
        }
    }

    *(MatrixWords *)(c + 0x1c4) = *(MatrixWords *)&IDENTITY_MATRIX4X3;
    *(s32 *)(c + 0x1e8) = *(s32 *)(c + 0x5c) >> 3;
    *(s32 *)(c + 0x1ec) = (*(s32 *)(c + 0x60) - 0x8000) >> 3;
    *(s32 *)(c + 0x1f0) = *(s32 *)(c + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x16c, c + 0x1c4, 0x50000, 0x64000, 0xf);
}

/* ROM ordinal 10 -- _ZN11daKinopio_c16CleanupResourcesEv, 0x021297e8,
                     size 0x6c */
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
    data_ov085_02130480.Release();
    data_ov085_02130488.Release();
    data_ov085_02130490.Release();
    return 1;
}

// @symbol _ZN11daKinopio_c6RenderEv
/* One call. THE CALL IS QUALIFIED, AND THAT IS LOAD-BEARING. Model::Render is
 * virtual (slot 5 of _ZTV5Model) and ModelAnim overrides it, so a plain
 * `mModelAnim.Render(0)` emits the vtable dispatch -- three words where the ROM
 * has one `bl`. Naming the base explicitly suppresses the dispatch and
 * reproduces the cartridge exactly. The null argument is the scale, and passing
 * 0 is what makes this Toad render at his own size rather than a
 * caller-supplied one. */
int daKinopio_c::Render()
{
    mModelAnim.Model::Render(0);
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
    mModelAnim.UpdateVerts();

    Player *p = ClosestPlayer();
    if (p != 0) {
        s32 threshold = 0xfa000;
        if (data_0209f2f8 == 0x32)
            threshold = 0x1f4000;

        Vector3 *psrc = (Vector3 *)&p->mPosX;
        Vector3 v;
        v.x = psrc->x;
        v.y = psrc->y;
        v.z = psrc->z;

        s32 hd = Vec3_HorzDist(&mPosX, &v);

        Vector3 v2;
        v2.y = v.y;
        v2.x = v.x;
        v2.z = v.z;
        v2.y = v.y + 0x1e000;

        s16 ha = Vec3_HorzAngle(&mPosX, &v2);
        s16 va = Vec3_VertAngle(&mPosX, &v2);

        if (hd < 0x190000)
            mTargetOpacity = 0xff;
        else
            mTargetOpacity = 0x3c;

        if (hd < threshold && AngleDiff(ha, mAngleY) < 0x3000) {
            mHeadYawTarget = (s16)(ha - mAngleY);
            mHeadPitchTarget = va;
        } else if (data_0209f2f8 != 0x32) {
            mHeadYawTarget = 0;
            mHeadPitchTarget = 0;
        } else {
            _Z14ApproachLinearRsss(&mAngleY, ha, 0x100);
            mHeadYawTarget = 0;
            mHeadPitchTarget = 0;
        }
    } else {
        mTargetOpacity = 0x3c;
    }

    {
        u32 id = mCapUniqueID;
        if (id != 0) {
            if (_ZN8dActor_c10FindWithIDEj(id) != 0)
                mTargetOpacity = 0xff;
        }
    }

    if (data_0209f2f8 == 0x32)
        mTargetOpacity = 0xff;

    _Z14ApproachLinearRsss(&mHeadYaw, mHeadYawTarget, 0x250);
    _Z14ApproachLinearRsss(&mHeadPitch, mHeadPitchTarget, 0x100);

    _Z15ApproachLinear2Riii((s32 *)&mOpacity, mTargetOpacity, 6);

    mModelAnim.Advance();

    UpdateModelPose();

    mModelAnim.speed = 0x1000;

    mModelAnim.ApplyOpacity((u32)(u8)(mOpacity >> 3), 1);

    mCollider.Clear();
    mCollider.Update();

    return 1;
}

/* ROM ordinal 13 -- _ZN11daKinopio_c13InitResourcesEv, 0x02129a7c,
                     size 0x254 */
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
    Animation::LoadFile(data_ov085_02130488);
    Animation::LoadFile(data_ov085_02130490);
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov085_02130480), 1, 0x16);
    mShadowModel.InitCylinder();
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mCollider, this,
        0x78000, 0x8c000, 0x4200004, 0);
    mOpacity = 0xff;
    mTargetOpacity = 0xff;
    mMessageID = param1;
    mStarID = (u8)mAngleX;
    mVariant = (u8)(mAngleX >> 8);
    if (mMessageID == 0xffff)
        mMessageID = 0;
    mCapUniqueID = 0;
    if (mVariant == 1) {
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
        if (func_02013a44() != 0) {
            Player *p = ClosestPlayer();
            if (p == 0)
                goto after_spawn;
            {
                u32 k = 0xd;
                u8 pl = *(u8 *)((char *)p + 0x6d9);
                s8 cc = mAreaId;
                int m1 = -1;
                void *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, k | ((u32)pl << 8), (Vector3 *)&mPosX, 0, cc, m1);
                if (spawned != 0)
                    mCapUniqueID = *(s32 *)((char *)spawned + 4);
            }
        after_spawn: ;
        }
    }
    if (mStarID != 0xff) {
        u16 v = mMessageID;
        switch (v) {
        case 0xaf5: mStarReqIndex = 0; break;
        case 0xaf7: mStarReqIndex = 1; break;
        case 0xaf9: mStarReqIndex = 2; break;
        }
        if (IsStarCollectedInCurLevel(mStarID) == 0) {
            if (NumStars() >= data_ov085_0212f27c[mStarReqIndex])
                goto after_inc;
        }
        {
            u16 *pm = (u16 *)((char *)&mMessageID);
            *pm = (u16)(*pm + 1);
        }
    after_inc: ;
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
/* The registry factory behind the KINOPIO profile. It allocates 0x210 -- this
 * class's own sizeof -- and installs this class's vtable, which is the second of
 * the two independent witnesses the header's layout is read from.
 *
 * Reconstructed source-style name: SM64DS proves daKinopio_c through RTTI,
 * allocation size, vtable identity and the KINOPIO registry profile; later EAD
 * lineage supplies classInit. The exact original spelling is not preserved.
 * Historical alias: Toad_Spawn.
 *
 * `return new daKinopio_c()` is the whole body: the synthesized constructor is
 * what stores the vptr and runs the dCcAc_c, ModelAnim and ShadowModel
 * constructors the ROM calls, in that order, and `operator new` is fBase_c's.
 * Spelling the vptr store by hand instead would be wrong here as well as longer
 * -- this TU emits the vtable itself, so the store has to be the compiler's
 * own. */
extern "C" daKinopio_c *daKinopio_c_classInit(void)
{
    return new daKinopio_c();
}
