//cpp
/* daLuigi_c -- the mirror-room Luigi reflection actor (ov055).
 *
 * One translation unit: the class's destructor, its three state-table helpers,
 * the reflection-state setter, and its five resource and update virtuals,
 * ov055 .text 0x021111a0..0x02111860. The factory daLuigi_c_classInit that
 * follows it stays in its own source (tu_map ends this TU at 0x02111860).
 *
 * mwccarm emits one .text section per function in the reverse of source
 * order, so the members are written highest ROM address first.
 */
#include "daLuigi_c.h"
#include "SharedFilePtr.h"

/* A Matrix4x3 copied as twelve plain words. Assigning Matrix4x3 itself
 * compiles to a different copy sequence (measured in InitResources). */
struct Mtx { int m[12]; };

/* The loaded file a SharedFilePtr holds, one word in. SharedFilePtr.h leaves
 * the handle's layout unrecovered, so this one access stays raw. */
#define FileOf(ptr, T) (((T **)&(ptr))[1])

int ApproachLinear(int &value, int target, int step);

/* Signatures that take Fix12<int> by value stay spelled as their mangled
 * names: declaring the real member does not reproduce the by-value argument
 * passing (wall 6az). */
extern "C" {
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
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(dActor_c *self, ShadowModel *shadow, Matrix4x3 *mat, int height, int radius, unsigned char opacity);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *, BCA_File *, int, int, unsigned short);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence *, BTP_File *, int, int, unsigned short);
void func_02016acc(void *model, unsigned int v);
void func_02016b24(void *model, unsigned int v);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angY);
int func_ov055_021112c4(daLuigi_c *self, daLuigiState *state, char *player);
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

    func_ov055_021112c4(this, &data_ov055_02111b70, (char *)data_0209f394[data_0209f250]);

    data_ov055_02111b68 = 0;
    data_ov055_02111a90 = 0x1ffff;
    data_ov055_02111b6c = 0;
    return 1;
}

// @symbol _ZN9daLuigi_c8BehaviorEv
int daLuigi_c::Behavior()
{
    int a, b;
    int f;
    char *val;
    daLuigiState *state;

    f = data_ov055_02111b68;
    if (f & 1) {
        data_ov055_02111b6c = 0;
        data_ov055_02111a90 = 0x1ffff;
        data_ov055_02111b60 = 0;
    } else if ((f & 2) && !(data_0209caa0[1] & 0x10)) {
        ApproachLinear(data_ov055_02111b6c, 0x1ffff, 0x400);
        data_ov055_02111b64 = (data_ov055_02111b64 & ~0x20000) + (0x1ffff - data_ov055_02111b6c);
    }
    val = (char *)data_0209f394[data_0209f250];
    state = mState;
    if (state->execute != 0)
        (((daLuigiStateHost *)this)->*state->execute)(val);
    Matrix4x3_FromTranslation(&mShadowMatrix, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    func_ov002_020e4374(val, &a, &b);
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMatrix, b, a, 0xf);
    *(char **)((char *)data_0209f318 + 0x114) = ((char *)this);
    return 1;
}

// @symbol _ZN9daLuigi_c6RenderEv
int daLuigi_c::Render()
{
    char *player;
    char *r8res;
    char *comp;
    char *q;
    char *p_f0;
    unsigned int i;
    Mtx *dst;
    Mtx *src;

    if (data_ov055_02111b6c == 0) return 1;

    /* Still raw: the player's model record returned by func_ov002_020e496c
     * and the player's +0x6fb frame byte have no typed layout yet. */
    player = (char *)data_0209f394[data_0209f250];
    r8res = (char *)func_ov002_020e496c(player);

    q = (char *)(int)((char *)&mModelAnim.data);
    comp = *(char **)q;
    dst = *(Mtx **)(q + 0xc);
    src = *(Mtx **)(r8res + 0x14);
    for (i = 0; i < *(unsigned int *)(comp + 4); i++) {
        /* The 64-bit round trip keeps the store pointer in the register
         * pair the ROM uses (lever 6m in notes/mwccarm-codegen.md). */
        *(Mtx *)(int)((long long)(int)dst) = *src;
        src++;
        dst++;
    }

    p_f0 = (char *)&mModelAnim.mat4x3;
    *(Mtx *)p_f0 = *(Mtx *)(r8res + 0x1c);
    *(int *)(p_f0 + 0x24) = -*(int *)(p_f0 + 0x24);
    func_0203c178(&data_020a0e68, -0x1000, 0x1000, 0x1000);
    MulMat3x3Mat3x3(p_f0, &data_020a0e68, p_f0);
    *(Mtx *)((char *)&mModel.mat4x3) = *(Mtx *)p_f0;

    if (data_ov055_02111b64 & 0x20000) return 1;

    mModelAnim.Model::Render(0);
    *(Mtx *)(*(char **)((char *)&mModel.data.transforms)) =
        *(Mtx *)(*(char **)((char *)&mModelAnim.data.transforms) + 0x2d0);
    mTextureSequences[0].Update(mModelAnim.data);
    mTextureSequences[0].currFrame =
        (int)(*(unsigned char *)(player + 0x6fb)) << 12;
    mTextureSequences[1].Update(mModel.data);
    mTextureSequences[1].currFrame =
        (int)(*(unsigned char *)(player + 0x6fb)) << 12;
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

// @symbol func_ov055_021112c4
extern "C" int func_ov055_021112c4(daLuigi_c *self, daLuigiState *state, char *player)
{
    self->mState = state;
    daLuigiState *s = self->mState;
    if (s->enter == 0) return 1;
    return (((daLuigiStateHost *)self)->*s->enter)(player);
}

// @symbol func_ov055_021112bc
extern "C" int func_ov055_021112bc(void)
{
    return 1;
}

// @symbol func_ov055_02111288
extern "C" int func_ov055_02111288(daLuigi_c *self, dActor_c *player)
{
    int *pos = &player->mPosX;
    int x = pos[0];
    int z = pos[2];
    int y = pos[1];

    self->mPosX = -x;
    self->mPosY = y;
    self->mPosZ = z;
    self->mAngleY = -player->mAngleY;
    return 1;
}

// @symbol func_ov055_02111264
extern "C" void func_ov055_02111264(void)
{
    data_ov055_02111b68 = 2;
    func_0201277c(0x179);
}
