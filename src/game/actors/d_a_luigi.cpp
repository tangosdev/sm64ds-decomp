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
 *   0x1ec, 5 relocation destinations wrong). The twelve-word Mtx43 copy stays.
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
#include "daPly_c.h"
#include "dCamera_c.h"
#include "math/Approach.h"
#include "math/MtxStack.h"
#include "private/mtx43.h"
#include "nitro/fx/fx.h"
#include "SaveBlock.h"

/* The matrix copies go through the flat Mtx43. Assigning Matrix4x3 itself
   is a different sequence (.text grows by 0xa0 bytes). */
/* InitResources' local is a VecFx32. Vector3's declared destructor (see
   include/types.h) costs this TU a numbered .data local if an object of it
   is declared here, so the local stays POD and casts at the call. */
#define CurrentPlayer() gPlayers[gLocalPlayerId]

/* Material bits ClearMaterialFlags clears and SetMaterialFlags sets. The reflected
 * player render applies the same pair around its draw. */
#define kMatClear 0x80
#define kMatSet 0x40
#define kFxOne 0x1000
#define kFadeFull 0x1ffff
#define kFadeStep 0x400
#define kHideBit 0x20000
#define kReqReset 1
#define kReqFade 2
/* Bit 0x10 of FileSaveData::flags1 is unnamed. */
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

extern unsigned char gLocalPlayerId;
extern daPly_c *gPlayers[];
extern dCamera_c *gCamera;

/* local extern: arm9/ov002 C helpers with no shared header yet. */

/* Sound::Play2D(3, id). The ROM calls this wrapper, not Play2D. */
unsigned int func_0201277c(unsigned int soundID);

/* Depth from mModelOffsetY, mPosY, mSinkDepth and mGroundY; radius from that,
 * mIsBalloon, mIsMega and mScaleY. The definition takes char *. */
/* Returns mBodyModels[GetBodyModelID(mBodyModelId, 1)] as int. */
extern "C" int func_ov002_020e496c(daPly_c *player);

extern "C" void func_0203c178(Matrix4x3 *m, int sx, int sy, int sz);
extern "C" void MTX_Concat33(const int *a, const int *b, int *dst);
/* local extern: math/CalcLib.h's spelling is the one below, but including
   that header renumbers this TU's @NNN .data locals. */
extern "C" void Vec3_Asr(Vector3 *dst, Vector3 *src, int shift);


/* Reconstructed source-style name: SM64DS proves daLuigi_c through RTTI,
 * allocation size, vtable identity, and the LUIGI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MirrorLuigi_Spawn. */
daLuigi_c *daLuigi_c_classInit() {
    return new daLuigi_c();
}

int daLuigi_c::InitResources() {
    VecFx32 t;

    mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0210ebb8), 1, -1);
    mAnmModel.ClearMaterialFlags(kMatClear);
    mAnmModel.SetMaterialFlags(kMatSet);

    mModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov002_0210eb20), 1, -1);
    mModel.ClearMaterialFlags(kMatClear);
    mModel.SetMaterialFlags(kMatSet);

    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210ebb8.filePtr,
                             *(BTP_File *)data_ov002_0210e8d0.filePtr);
    mTextureSequences[0].SetFile(*(BTP_File *)data_ov002_0210e8d0.filePtr, 0, kFxOne, 0);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov002_0210eb20.filePtr,
                             *(BTP_File *)data_ov002_0210ebd8.filePtr);
    mTextureSequences[1].SetFile(*(BTP_File *)data_ov002_0210ebd8.filePtr, 0, kFxOne, 0);

    mShadowModel.InitCylinder();

    mAnmModel.SetAnim(dExtFrameCtrl_c::LoadFile(data_ov002_0210eaa0), 0, kFxOne, 0);

    Vec3_Asr((Vector3 *)&t, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);

    *(Mtx43 *)&mAnmModel.mat4x3 = *(Mtx43 *)&data_020a0e68;
    *(Mtx43 *)&mModel.mat4x3 = *(Mtx43 *)&data_020a0e68;

    SetState(&data_ov055_02111b70, CurrentPlayer());

    data_ov055_02111b68 = 0;
    data_ov055_02111a90 = kFadeFull;
    data_ov055_02111b6c = 0;
    return 1;
}

int daLuigi_c::Behavior() {
    int depth, radius;
    daPly_c *player;

    if (data_ov055_02111b68 & kReqReset) {
        data_ov055_02111b6c = 0;
        data_ov055_02111a90 = kFadeFull;
        data_ov055_02111b60 = 0;
    } else if ((data_ov055_02111b68 & kReqFade) && !(data_0209caa0.file.flags1 & kSysHold)) {
        ApproachLinear(data_ov055_02111b6c, kFadeFull, kFadeStep);
        data_ov055_02111b64 = (data_ov055_02111b64 & ~kHideBit) + (kFadeFull - data_ov055_02111b6c);
    }
    player = CurrentPlayer();
    if (mState->execute)
        (this->*mState->execute)(player);
    Matrix4x3_FromTranslation(&mShadowMatrix, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    player->func_ov002_020e4374(&depth, &radius);
    DropShadowRadHeight(mShadowModel, mShadowMatrix, radius, depth, kShadowOpacity);
    /* dCamera_c::mFramedActor; this file does not include dCamera_c.h. */
    gCamera->mFramedActor = this;
    return 1;
}

int daLuigi_c::Render() {
    daPly_c *player;
    dExtSimpleModel_c *body;
    ModelComponents *comps;
    BMD_File *file;
    Matrix4x3 *mat;
    unsigned int bone;
    Mtx43 *dst;
    Mtx43 *src;

    if (data_ov055_02111b6c == 0)
        return 1;

    player = CurrentPlayer();
    body = (dExtSimpleModel_c *)func_ov002_020e496c(player);

    comps = &mAnmModel.data;
    file = comps->modelFile;
    dst = (Mtx43 *)comps->transforms;
    src = (Mtx43 *)body->data.transforms;
    for (bone = 0; bone < file->numBones; bone++) {
        /* u64 round trip on the destination. A plain store swaps the copy
         * registers (8 words). */
        *(Mtx43 *)((long long)(int)dst) = *src;
        src++;
        dst++;
    }

    mat = &mAnmModel.mat4x3;
    *(Mtx43 *)mat = *(Mtx43 *)&body->mat4x3;
    mat->t.x = -mat->t.x;
    func_0203c178(&data_020a0e68, kMirrorSx, kFxOne, kFxOne);
    MTX_Concat33((const int *)mat, (const int *)&data_020a0e68, (int *)mat);
    *(Mtx43 *)&mModel.mat4x3 = *(Mtx43 *)mat;

    if (data_ov055_02111b64 & kHideBit)
        return 1;

    mAnmModel.dExtSimpleModel_c::Render(0);
    *(Mtx43 *)&mModel.data.transforms[0] = *(Mtx43 *)&mAnmModel.data.transforms[15];
    mTextureSequences[0].Update(mAnmModel.data);
    mTextureSequences[0].currFrame = player->mIsVanish << 12;
    mTextureSequences[1].Update(mModel.data);
    mTextureSequences[1].currFrame = player->mIsVanish << 12;
    mModel.Render(0);
    return 1;
}

void daLuigi_c::OnPendingDestroy() {}

int daLuigi_c::CleanupResources() {
    data_ov002_0210ebb8.Release();
    data_ov002_0210eb20.Release();
    data_ov002_0210eaa0.Release();
    return 1;
}

int daLuigi_c::SetState(daLuigiState *state, daPly_c *player) {
    mState = state;
    if (mState->enter == 0)
        return 1;
    return (this->*mState->enter)(player);
}

int daLuigi_c::EnterMirror(daPly_c *player) {
    return 1;
}

int daLuigi_c::ExecuteMirror(daPly_c *player) {
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

/* One candidate caller, ov063 0x02118004, relocates to ov027 or ov055, so
 * the owner is unknown. */
void func_ov055_02111264(void) {
    data_ov055_02111b68 = kReqFade;
    func_0201277c(kAppearSound);
}

/* This file defines the record. The compiler copies the two
 * pointer-to-member descriptors into it at overlay load. */
daLuigiState data_ov055_02111b70 = {
    &daLuigi_c::EnterMirror,
    &daLuigi_c::ExecuteMirror,
};
