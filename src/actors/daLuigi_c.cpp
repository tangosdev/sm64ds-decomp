//cpp
/* daLuigi_c -- mirror-room Luigi (ov055, actor LUIGI).
 *
 * Stands where the current player stands, with X and facing negated, and
 * draws that pose on two models. The body copies the player's bone
 * transforms and flips X; the second model follows body bone 15. A request
 * word fades the reflection in. The factory daLuigi_c_classInit and
 * g_profile_LUIGI stay in src/d_a_luigi.cpp; this TU ends at 0x02111860.
 *
 * mwccarm emits one .text section per function in the reverse of source
 * order, so the members are written highest address first.
 *
 * deslop leftovers:
 * - InitResources: a local holding Model::LoadFile's return, reused by
 *   Prepare, differs by 34 words (size stays 0x1ec).
 * - InitResources: ModelAnim::SetAnim with a Fix12<int> local differs
 *   (size stays 0x1ec, 10 relocation destinations wrong).
 * - InitResources: TextureSequence::SetFile with a Fix12<int> local differs
 *   (size stays 0x1ec, 11 relocation destinations wrong; the call is not
 *   the int-parameter symbol the ROM uses).
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

/* Twelve-word copy. Assigning Matrix4x3 itself is a different sequence. */
struct Mtx { int m[12]; };

/* filePtr is the word Model::LoadFile and Animation::LoadFile read.
 * include/SharedFilePtr.h has no fields. */
struct LoadedSharedFile {
    u16 fileID;    /* 0x00 */
    u8 numRefs;    /* 0x02 */
    u8 pad;        /* 0x03 */
    void *filePtr; /* 0x04 */
};

/* Vec3_Asr's definition. Three ints, same layout as mPosX/Y/Z. */
struct Vec3 { int x, y, z; };
struct Obj;
#define FileOf(handle, T) ((T *)((LoadedSharedFile *)&(handle))->filePtr)

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

int ApproachLinear(int &value, int target, int step);

extern "C" {
/* Reflection state in this overlay. b68 is the request: bit 0 resets, bit 1
 * fades in. b6c is the fade, 0 (hidden) to kFadeFull. kHideBit of b64 skips
 * the draw. b70 is the one state, filled by __sinit_ov055_021118d4 from the
 * static table. a90 is written full on reset and b60 is cleared; neither has
 * a matched reader. func_ov002_020e3e00 reads b6c and b64. */
extern int data_ov055_02111a90;
extern int data_ov055_02111b60;
extern int data_ov055_02111b64;
extern int data_ov055_02111b68;
extern int data_ov055_02111b6c;
extern daLuigiState data_ov055_02111b70;

/* Sound::Play2D(3, id). The ROM calls this wrapper, not Play2D. */
unsigned int func_0201277c(unsigned int soundID);

/* Body model 0x8080, second model 0x807e, body anim 0x8029, texture
 * patterns 0x8022 (body) and 0x8025 (second model). */
extern SharedFilePtr data_ov002_0210ebb8;
extern SharedFilePtr data_ov002_0210eb20;
extern SharedFilePtr data_ov002_0210eaa0;
extern SharedFilePtr data_ov002_0210e8d0;
extern SharedFilePtr data_ov002_0210ebd8;

extern unsigned char data_0209f250;
extern void *data_0209f394[];
extern Matrix4x3 data_020a0e68;
extern int data_0209caa0[];
extern void *data_0209f318;

/* Depth from unk_690, mPosY, mSinkDepth and mGroundY; radius from that,
 * mIsBalloon, mIsMega and mScaleY. The definition takes char *. */
void func_ov002_020e4374(char *player, int *depth, int *radius);
/* Returns mBodyModels[GetBodyModelID(mBodyModelId, 1)] as int. */
int func_ov002_020e496c(char *player);

void func_0203c178(Matrix4x3 *m, int sx, int sy, int sz);
void MulMat3x3Mat3x3(const int *a, const int *b, int *dst);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void func_02016acc(char *model, unsigned int mask);
void func_02016b24(Obj *model, unsigned int mask);
void Vec3_Asr(Vec3 *dst, Vec3 *src, int shift);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angY);

/* Fix12<int> by value. The method form homes that argument on the stack. */
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(dActor_c *self, ShadowModel *shadow, Matrix4x3 *mat, int radius, int depth, unsigned char opacity);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *model, BCA_File *file, int flags, int speed, unsigned short start);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence *seq, BTP_File *file, int flags, int speed, unsigned short start);
}

// @symbol _ZN9daLuigi_c13InitResourcesEv
int daLuigi_c::InitResources()
{
    Vec3 t;

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210ebb8), 1, -1);
    func_02016acc((char *)&mModelAnim, kMatClear);
    func_02016b24((Obj *)&mModelAnim, kMatSet);

    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210eb20), 1, -1);
    func_02016acc((char *)&mModel, kMatClear);
    func_02016b24((Obj *)&mModel, kMatSet);

    TextureSequence::Prepare(*FileOf(data_ov002_0210ebb8, BMD_File),
                             *FileOf(data_ov002_0210e8d0, BTP_File));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequences[0], FileOf(data_ov002_0210e8d0, BTP_File), 0, kFxOne, 0);
    TextureSequence::Prepare(*FileOf(data_ov002_0210eb20, BMD_File),
                             *FileOf(data_ov002_0210ebd8, BTP_File));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequences[1], FileOf(data_ov002_0210ebd8, BTP_File), 0, kFxOne, 0);

    mShadowModel.InitCylinder();

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)Animation::LoadFile(data_ov002_0210eaa0), 0, kFxOne, 0);

    Vec3_Asr(&t, (Vec3 *)&mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t.x, t.y, t.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);

    *(Mtx *)&mModelAnim.mat4x3 = *(Mtx *)&data_020a0e68;
    *(Mtx *)&mModel.mat4x3 = *(Mtx *)&data_020a0e68;

    SetState(&data_ov055_02111b70, CurrentPlayer());

    data_ov055_02111b68 = 0;
    data_ov055_02111a90 = kFadeFull;
    data_ov055_02111b6c = 0;
    return 1;
}

// @symbol _ZN9daLuigi_c8BehaviorEv
int daLuigi_c::Behavior()
{
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
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMatrix, radius, depth, kShadowOpacity);
    /* Camera::pad_114. No named field; the store is the actor pointer. */
    *(daLuigi_c **)((char *)data_0209f318 + 0x114) = this;
    return 1;
}

// @symbol _ZN9daLuigi_c6RenderEv
int daLuigi_c::Render()
{
    Player *player;
    Model *body;
    ModelComponents *comps;
    BMD_File *file;
    Matrix4x3 *mat;
    unsigned int bone;
    Mtx *dst;
    Mtx *src;

    if (data_ov055_02111b6c == 0) return 1;

    player = CurrentPlayer();
    body = (Model *)func_ov002_020e496c((char *)player);

    comps = &mModelAnim.data;
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

    mat = &mModelAnim.mat4x3;
    *(Mtx *)mat = *(Mtx *)&body->mat4x3;
    mat->t.x = -mat->t.x;
    func_0203c178(&data_020a0e68, kMirrorSx, kFxOne, kFxOne);
    MulMat3x3Mat3x3((const int *)mat, (const int *)&data_020a0e68, (int *)mat);
    *(Mtx *)&mModel.mat4x3 = *(Mtx *)mat;

    if (data_ov055_02111b64 & kHideBit) return 1;

    mModelAnim.Model::Render(0);
    *(Mtx *)&mModel.data.transforms[0] = *(Mtx *)&mModelAnim.data.transforms[15];
    mTextureSequences[0].Update(mModelAnim.data);
    mTextureSequences[0].currFrame = player->mIsVanish << 12;
    mTextureSequences[1].Update(mModel.data);
    mTextureSequences[1].currFrame = player->mIsVanish << 12;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daLuigi_c16OnPendingDestroyEv
void daLuigi_c::OnPendingDestroy()
{
}

// @symbol _ZN9daLuigi_c16CleanupResourcesEv
int daLuigi_c::CleanupResources()
{
    data_ov002_0210ebb8.Release();
    data_ov002_0210eb20.Release();
    data_ov002_0210eaa0.Release();
    return 1;
}

// @symbol _ZN9daLuigi_c8SetStateEP12daLuigiStateP6Player
int daLuigi_c::SetState(daLuigiState *state, Player *player)
{
    mState = state;
    if (mState->enter == 0) return 1;
    return (this->*mState->enter)(player);
}

// @symbol _ZN9daLuigi_c11EnterMirrorEP6Player
int daLuigi_c::EnterMirror(Player *player)
{
    return 1;
}

// @symbol _ZN9daLuigi_c13ExecuteMirrorEP6Player
int daLuigi_c::ExecuteMirror(Player *player)
{
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
extern "C" void func_ov055_02111264(void)
{
    data_ov055_02111b68 = kReqFade;
    func_0201277c(kAppearSound);
}
