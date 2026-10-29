//cpp
/* daLuigi_c -- mirror-room Luigi (ov055, actor LUIGI).
 *
 * Stands where the current player stands, with X and facing negated, and
 * draws that pose on two models. The body copies the player's bone
 * transforms and flips X; the second model follows body bone 15. A request
 * word fades the reflection in. Twelve functions, .text
 * 0x021111a0..0x021118d4; the factory daLuigi_c_classInit (0x02111860) closes
 * the unit and ov055's .text.
 *
 * mwccarm emits one .text section per function in the reverse of source
 * order, so the members are written highest address first, starting with
 * the factory.
 *
 * What stays, and why (each alternative measured):
 * - InitResources: a local holding dExtSimpleModel_c::LoadFile's return, reused by
 *   Prepare, differs by 34 words (size stays 0x1ec).
 * - InitResources: dExtAnmModel_c::SetAnim with a Fix12<int> local differs
 *   (size stays 0x1ec, 10 relocation destinations wrong).
 * - InitResources: assigning Matrix4x3 onto both models differs (size stays
 *   0x1ec, 5 relocation destinations wrong). The twelve-word Mtx copy stays.
 * - Behavior: DropShadowRadHeight with Fix12<int> locals differs (size stays
 *   0x16c, 5 relocation destinations wrong).
 * - Behavior: or-ing the fade into data_ov055_02111b64 differs by 1 word.
 *   The ROM adds.
 * - Render: *dst = *src in the bone loop differs by 8 words (size stays
 *   0x1b0). The u64 destination round trip stays.
 * - Render: Matrix4x3 assignment of the body matrix, the flipped copy onto
 *   mModel, and transforms[15] together differs (size stays 0x1b0, 1
 *   relocation destination wrong).
 * - Render: inlining mBodyModels[GetBodyModelID(mBodyModelId, 1)] differs
 *   (size stays 0x1b0, 2 relocation destinations wrong). The ROM calls
 *   func_ov002_020e496c.
 * - ExecuteMirror: direct mPosX, mPosZ, mPosY reads differ (size stays
 *   0x034). One pointer, x then z then y, stays.
 * - func_ov055_02111264: Sound::Play2D(3, 0x179) differs (size stays 0x024,
 *   the relocation target is func_0201277c). That wrapper is Play2D(3, id).
 */
#include "daLuigi_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "math/Approach.h"
#include "math/MtxStack.h"

/* Twelve-word copy. Assigning Matrix4x3 itself is a different sequence. */
struct Mtx {
    int m[12];
};

/* Vec3_Asr's definition. Three ints, same layout as mPosX/Y/Z. */
struct Vec3 {
    int x, y, z;
};

#define CurrentPlayer() ((Player *)data_0209f394[data_0209f250])

/* Material bits func_02016acc clears and func_02016b24 sets. The reflected
 * player render applies the same pair around its draw. */
#define kMatClear 0x80
#define kMatSet 0x40
#define kFxOne 0x1000
#define kFadeFull 0x1ffff
#define kFadeStep 0x400
#define kHideBit 0x20000
#define kReqReset 1
#define kReqFade 2
/* data_0209caa0[1] is SaveData::flags1. Bit 0x10 is unnamed there. */
#define kSysHold 0x10
#define kShadowOpacity 0xf
#define kAppearSound 0x179
#define kMirrorSx (-0x1000)

/* Reflection state in this overlay. b68 is the request: bit 0 resets, bit 1
 * fades in. b6c is the fade, 0 (hidden) to kFadeFull. kHideBit of b64 skips
 * the draw. b70 is the one state record this file defines; the compiler
 * copies its two pointer-to-member descriptors into it at overlay load.
 * a90 is written full on reset and b60 is cleared; neither has a matched
 * reader. func_ov002_020e3e00 reads b6c and b64. */
extern int data_ov055_02111a90;
extern int data_ov055_02111b60;
extern int data_ov055_02111b64;
extern int data_ov055_02111b68;
extern int data_ov055_02111b6c;
extern daLuigiState data_ov055_02111b70;

/* Body model 0x8080, second model 0x807e, body anim 0x8029, texture
 * patterns 0x8022 (body) and 0x8025 (second model). */
extern SharedFilePtr data_ov002_0210ebb8;
extern SharedFilePtr data_ov002_0210eb20;
extern SharedFilePtr data_ov002_0210eaa0;
extern SharedFilePtr data_ov002_0210e8d0;
extern SharedFilePtr data_ov002_0210ebd8;

extern unsigned char data_0209f250;
extern void *data_0209f394[];
extern int data_0209caa0[];
extern void *data_0209f318;

/* local extern: arm9/ov002 C helpers with no shared header yet. */
extern "C" {
/* Sound::Play2D(3, id). The ROM calls this wrapper, not Play2D. */
unsigned int func_0201277c(unsigned int soundID);

/* Depth from mModelOffsetY, mPosY, mSinkDepth and mGroundY; radius from that,
 * mIsBalloon, mIsMega and mScaleY. The definition takes char *. */
void func_ov002_020e4374(char *player, int *depth, int *radius);
/* Returns mBodyModels[GetBodyModelID(mBodyModelId, 1)] as int. */
int func_ov002_020e496c(Player *player);

void func_0203c178(Matrix4x3 *m, int sx, int sy, int sz);
void MulMat3x3Mat3x3(const int *a, const int *b, int *dst);
void func_02016acc(dExtSimpleModel_c *model, unsigned int mask);
void func_02016b24(dExtSimpleModel_c *model, u32 mask);
void Vec3_Asr(Vec3 *dst, Vec3 *src, int shift);
}

/* Reconstructed source-style name: SM64DS proves daLuigi_c through RTTI,
 * allocation size, vtable identity, and the LUIGI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MirrorLuigi_Spawn. */
// @symbol daLuigi_c_classInit
extern "C" daLuigi_c *daLuigi_c_classInit() {
    return new daLuigi_c();
}

// @symbol _ZN9daLuigi_c13InitResourcesEv
int daLuigi_c::InitResources() {
    Vec3 t;

    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210ebb8), 1, -1);
    func_02016acc(&mAnmModel, kMatClear);
    func_02016b24(&mAnmModel, kMatSet);

    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210eb20), 1, -1);
    func_02016acc(&mModel, kMatClear);
    func_02016b24(&mModel, kMatSet);

    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210ebb8.filePtr,
                             *(BTP_File *)data_ov002_0210e8d0.filePtr);
    mTextureSequences[0].SetFile(*(BTP_File *)data_ov002_0210e8d0.filePtr, 0, kFxOne, 0);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210eb20.filePtr,
                             *(BTP_File *)data_ov002_0210ebd8.filePtr);
    mTextureSequences[1].SetFile(*(BTP_File *)data_ov002_0210ebd8.filePtr, 0, kFxOne, 0);

    mShadowModel.InitCylinder();

    mAnmModel.SetAnim((BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov002_0210eaa0), 0, kFxOne, 0);

    Vec3_Asr(&t, (Vec3 *)&mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);

    *(Mtx *)&mAnmModel.mat4x3 = *(Mtx *)&data_020a0e68;
    *(Mtx *)&mModel.mat4x3 = *(Mtx *)&data_020a0e68;

    SetState(&data_ov055_02111b70, CurrentPlayer());

    data_ov055_02111b68 = 0;
    data_ov055_02111a90 = kFadeFull;
    data_ov055_02111b6c = 0;
    return 1;
}

// @symbol _ZN9daLuigi_c8BehaviorEv
int daLuigi_c::Behavior() {
    int depth, radius;
    Player *player;

    if (data_ov055_02111b68 & kReqReset) {
        data_ov055_02111b6c = 0;
        data_ov055_02111a90 = kFadeFull;
        data_ov055_02111b60 = 0;
    } else if ((data_ov055_02111b68 & kReqFade) && !(data_0209caa0[1] & kSysHold)) {
        ApproachLinear(data_ov055_02111b6c, kFadeFull, kFadeStep);
        data_ov055_02111b64 = (data_ov055_02111b64 & ~kHideBit) + (kFadeFull - data_ov055_02111b6c);
    }
    player = CurrentPlayer();
    if (mState->execute)
        (this->*mState->execute)(player);
    Matrix4x3_FromTranslation(&mShadowMatrix, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    func_ov002_020e4374((char *)player, &depth, &radius);
    DropShadowRadHeight(mShadowModel, mShadowMatrix, radius, depth, kShadowOpacity);
    /* dCamera_c::mFramedActor; this file does not include dCamera_c.h. */
    *(daLuigi_c **)((char *)data_0209f318 + 0x114) = this;
    return 1;
}

// @symbol _ZN9daLuigi_c6RenderEv
int daLuigi_c::Render() {
    Player *player;
    dExtSimpleModel_c *body;
    ModelComponents *comps;
    BMD_File *file;
    Matrix4x3 *mat;
    unsigned int bone;
    Mtx *dst;
    Mtx *src;

    if (data_ov055_02111b6c == 0)
        return 1;

    player = CurrentPlayer();
    body = (dExtSimpleModel_c *)func_ov002_020e496c(player);

    comps = &mAnmModel.data;
    file = comps->modelFile;
    dst = (Mtx *)comps->transforms;
    src = (Mtx *)body->data.transforms;
    for (bone = 0; bone < file->numBones; bone++) {
        /* u64 round trip on the destination. A plain store swaps the copy
         * registers (8 words). */
        *(Mtx *)(int)((long long)(int)dst) = *src;
        src++;
        dst++;
    }

    mat = &mAnmModel.mat4x3;
    *(Mtx *)mat = *(Mtx *)&body->mat4x3;
    mat->t.x = -mat->t.x;
    func_0203c178(&data_020a0e68, kMirrorSx, kFxOne, kFxOne);
    MulMat3x3Mat3x3((const int *)mat, (const int *)&data_020a0e68, (int *)mat);
    *(Mtx *)&mModel.mat4x3 = *(Mtx *)mat;

    if (data_ov055_02111b64 & kHideBit)
        return 1;

    mAnmModel.dExtSimpleModel_c::Render(0);
    *(Mtx *)&mModel.data.transforms[0] = *(Mtx *)&mAnmModel.data.transforms[15];
    mTextureSequences[0].Update(mAnmModel.data);
    mTextureSequences[0].currFrame = player->mIsVanish << 12;
    mTextureSequences[1].Update(mModel.data);
    mTextureSequences[1].currFrame = player->mIsVanish << 12;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daLuigi_c16OnPendingDestroyEv
void daLuigi_c::OnPendingDestroy() {}

// @symbol _ZN9daLuigi_c16CleanupResourcesEv
int daLuigi_c::CleanupResources() {
    data_ov002_0210ebb8.Release();
    data_ov002_0210eb20.Release();
    data_ov002_0210eaa0.Release();
    return 1;
}

// @symbol _ZN9daLuigi_c8SetStateEP12daLuigiStateP6Player
int daLuigi_c::SetState(daLuigiState *state, Player *player) {
    mState = state;
    if (mState->enter == 0)
        return 1;
    return (this->*mState->enter)(player);
}

// @symbol _ZN9daLuigi_c11EnterMirrorEP6Player
int daLuigi_c::EnterMirror(Player *player) {
    return 1;
}

// @symbol _ZN9daLuigi_c13ExecuteMirrorEP6Player
int daLuigi_c::ExecuteMirror(Player *player) {
    /* One base, read x then z then y. Direct mPosX / mPosZ / mPosY
     * differ (same size). */
    int *pos = &player->mPosX;
    int x = pos[0];
    int z = pos[2];
    int y = pos[1];

    mPosX = -x;
    mPosY = y;
    mPosZ = z;
    mAngleY = -player->mAngleY;
    return 1;
}

// @symbol func_ov055_02111264
/* One candidate caller, ov063 0x02118004, relocates to ov027 or ov055, so
 * the owner is unknown. */
extern "C" void func_ov055_02111264(void) {
    data_ov055_02111b68 = kReqFade;
    func_0201277c(kAppearSound);
}

/* This file defines the record. The compiler copies the two
 * pointer-to-member descriptors into it at overlay load. */
daLuigiState data_ov055_02111b70 = {
    &daLuigi_c::EnterMirror,
    &daLuigi_c::ExecuteMirror,
};
