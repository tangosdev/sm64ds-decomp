//cpp
/* daLuigi_c -- the mirror-room Luigi reflection actor (ov055).
 *
 * One translation unit: the class's destructor, a fade-in helper, the
 * mirror state and SetState, and its five resource and update virtuals,
 * ov055 .text 0x021111a0..0x02111860. The factory daLuigi_c_classInit that
 * follows it stays in its own source (tu_map ends this TU at 0x02111860).
 *
 * mwccarm emits one .text section per function in the reverse of source
 * order, so the members are written highest ROM address first.
 */
#include "daLuigi_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

/* A Matrix4x3 copied as twelve plain words. Assigning Matrix4x3 itself
 * compiles to a different copy sequence (measured in InitResources and
 * Render). */
struct Mtx { int m[12]; };

/* The loaded file a SharedFilePtr holds, one word in. SharedFilePtr.h leaves
 * the handle's layout unrecovered, so this one access stays raw. */
#define FileOf(ptr, T) (((T **)&(ptr))[1])

int ApproachLinear(int &value, int target, int step);

/* Signatures that take Fix12<int> by value stay spelled as their mangled
 * names: declaring the real member does not reproduce the by-value argument
 * passing (wall 6az). */
extern "C" {
/* The reflection's shared state. b68 is a request word: bit 0 resets, bit 1
 * fades the reflection in. b6c is that fade, 0 (hidden) to 0x1ffff; bit
 * 0x20000 of b64 hides the models. b70 is the one state, filled by the
 * static initializer. */
extern int data_ov055_02111a90;
extern int data_ov055_02111b60;
extern int data_ov055_02111b64;
extern int data_ov055_02111b68;
extern int data_ov055_02111b6c;
extern daLuigiState data_ov055_02111b70;
unsigned int func_0201277c(unsigned int soundID);
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
int func_ov002_020e496c(char *player);
void func_0203c178(void *m, int a, int b, int c);
void MulMat3x3Mat3x3(void *d, void *a, void *b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void func_ov002_020e4374(char *c, int *p1, int *p2);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(dActor_c *self, ShadowModel *shadow, Matrix4x3 *mat, int radius, int depth, unsigned char opacity);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *, BCA_File *, int, int, unsigned short);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence *, BTP_File *, int, int, unsigned short);
void func_02016acc(void *model, unsigned int v);
void func_02016b24(void *model, unsigned int v);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angY);
}

// @symbol _ZN9daLuigi_c13InitResourcesEv
int daLuigi_c::InitResources()
{
    int t[3];

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210ebb8), 1, -1);
    func_02016acc(&mModelAnim, 0x80);
    func_02016b24(&mModelAnim, 0x40);

    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210eb20), 1, -1);
    func_02016acc(&mModel, 0x80);
    func_02016b24(&mModel, 0x40);

    TextureSequence::Prepare(*FileOf(data_ov002_0210ebb8, BMD_File),
                             *FileOf(data_ov002_0210e8d0, BTP_File));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequences[0], FileOf(data_ov002_0210e8d0, BTP_File), 0, 0x1000, 0);
    TextureSequence::Prepare(*FileOf(data_ov002_0210eb20, BMD_File),
                             *FileOf(data_ov002_0210ebd8, BTP_File));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequences[1], FileOf(data_ov002_0210ebd8, BTP_File), 0, 0x1000, 0);

    mShadowModel.InitCylinder();

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File *)Animation::LoadFile(data_ov002_0210eaa0), 0, 0x1000, 0);

    Vec3_Asr(t, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);

    *(Mtx *)&mModelAnim.mat4x3 = *(Mtx *)&data_020a0e68;
    *(Mtx *)&mModel.mat4x3 = *(Mtx *)&data_020a0e68;

    SetState(&data_ov055_02111b70, (Player *)data_0209f394[data_0209f250]);

    data_ov055_02111b68 = 0;
    data_ov055_02111a90 = 0x1ffff;
    data_ov055_02111b6c = 0;
    return 1;
}

// @symbol _ZN9daLuigi_c8BehaviorEv
int daLuigi_c::Behavior()
{
    int depth, radius;
    Player *player;

    if (data_ov055_02111b68 & 1) {
        data_ov055_02111b6c = 0;
        data_ov055_02111a90 = 0x1ffff;
        data_ov055_02111b60 = 0;
    } else if ((data_ov055_02111b68 & 2) && !(data_0209caa0[1] & 0x10)) {
        ApproachLinear(data_ov055_02111b6c, 0x1ffff, 0x400);
        data_ov055_02111b64 = (data_ov055_02111b64 & ~0x20000) + (0x1ffff - data_ov055_02111b6c);
    }
    player = (Player *)data_0209f394[data_0209f250];
    if (mState->execute != 0)
        (this->*mState->execute)(player);
    Matrix4x3_FromTranslation(&mShadowMatrix, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    func_ov002_020e4374((char *)player, &depth, &radius);
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMatrix, radius, depth, 0xf);
    *(char **)((char *)data_0209f318 + 0x114) = ((char *)this);
    return 1;
}

// @symbol _ZN9daLuigi_c6RenderEv
int daLuigi_c::Render()
{
    Player *player;
    Model *body;
    ModelComponents *mine;
    BMD_File *file;
    Matrix4x3 *mat;
    unsigned int i;
    Mtx *dst;
    Mtx *src;

    if (data_ov055_02111b6c == 0) return 1;

    player = (Player *)data_0209f394[data_0209f250];
    body = (Model *)func_ov002_020e496c((char *)player);

    mine = &mModelAnim.data;
    file = mine->modelFile;
    dst = (Mtx *)mine->transforms;
    src = (Mtx *)body->data.transforms;
    for (i = 0; i < file->numBones; i++) {
        /* The 64-bit round trip keeps the store pointer in the register
         * the ROM uses (lever 6m in notes/mwccarm-codegen.md). */
        *(Mtx *)(int)((long long)(int)dst) = *src;
        src++;
        dst++;
    }

    mat = &mModelAnim.mat4x3;
    *(Mtx *)mat = *(Mtx *)&body->mat4x3;
    mat->t.x = -mat->t.x;
    func_0203c178(&data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3(mat, &data_020a0e68, mat);
    *(Mtx *)&mModel.mat4x3 = *(Mtx *)mat;

    if (data_ov055_02111b64 & 0x20000) return 1;

    mModelAnim.Model::Render(0);
    *(Mtx *)&mModel.data.transforms[0] =
        *(Mtx *)&mModelAnim.data.transforms[15];
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
    /* One base pointer, read x, z, y: direct member reads schedule
     * differently. */
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
/* Requests the fade-in and plays sound 0x179. Its one candidate caller, at
 * ov063 0x02118004, relocates ambiguously to ov027 or ov055, so its owner
 * and name are unknown. */
extern "C" void func_ov055_02111264(void)
{
    data_ov055_02111b68 = 2;
    func_0201277c(0x179);
}
