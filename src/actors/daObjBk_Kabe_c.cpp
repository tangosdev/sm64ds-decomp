//cpp
/* daKpa_c's castle walls (BK_KABE00 and BK_KABE01).
 *
 * The breakable wall, actor 0x30, does not die in Kill. Kill plays the
 * break sound at the camera-space position and a puff on the wall, then
 * sets mBroken. While that flag is set, Behavior turns the mesh off, waits
 * out the secret jingle, raises a star (actor 0xb2) 200 units, and only
 * then destroys the actor. The solid wall is destroyed by the same Kill
 * immediately. Render draws nothing once the breakable wall is broken.
 *
 * mVariant is 0 for the breakable wall and 1 for the solid one. It selects
 * a row of model, collision, and CLPS. The star index is the low byte of
 * param1, with 0xff read as star 0. The g_profile rows are the next ROM
 * run, not this file.
 *
 * daObjBk_Kabe_c_classInit_BK_KABE01 (0x0212757c, historical alias
 * FortressWall_Spawn) and daObjBk_Kabe_c_classInit_BK_KABE00 (0x021275ac,
 * historical alias FortressWallBreakable_Spawn) are reconstructed names
 * (RTTI daObjBk_Kabe_c, the two registry profiles); retail does not store
 * them. Each hand-called fBase_c::operator new(804) + the inherited
 * dBgActor_c ctor + this class's vtable store. daObjBk_Kabe_c has no
 * user-declared constructor, so both are now `new daObjBk_Kabe_c()`. This
 * TU keeps `#pragma defer_codegen off`, so emission follows source order
 * (ROM-ascending); the two factories append after InitResources, in ROM
 * order (BK_KABE01 then BK_KABE00).
 *
 * deslop leftovers:
 * - Kill: folding the test into `if (actorID == kBreakableActor)` is a
 *   size-DIFF, 0x60 to 0x54. The compare stays a 0/1 local.
 * - Kill: a Vector3 built from mCamSpacePosX/Y/Z for PlayBank3 is a
 *   size-DIFF, 0x60 to 0x84. The call keeps the address of mCamSpacePosX.
 * - Kill: Particle::System::NewSimple(kBreakParticle, mPosX, mPosY, mPosZ)
 *   does not compile (int does not convert to Fix12<int>). Fix12<int>
 *   locals are a size-DIFF, 0x60 to 0x90. The scalar extern stays.
 * - Behavior: `if (actorID == kBreakableActor && mBroken)` is a size-DIFF,
 *   0xf0 to 0xe4. The 0/1 local stays.
 * - Behavior: `pos.y = mPosY + kStarPopY` is a size-DIFF, 0xf0 to 0xec.
 *   Y is stored, then increased.
 * - Behavior: assigning mMeshCollider.unk_0c is a size-DIFF, 0xf0 to 0xe8.
 *   func_020393a4 stays; that is the call the ROM makes.
 * - Behavior: IsClsnInRange(kClsnRange, 0) does not compile (no conversion
 *   to Fix12<int>). Fix12<int> locals are a size-DIFF, 0xf0 to 0x108. The
 *   scalar extern stays.
 * - InitResources: indexing the row with mVariant, instead of a local, is
 *   a size-DIFF, 0xe8 to 0xf4. idx and idx2 stay.
 * - InitResources: mMeshCollider.SetFile(..., kMeshScale, ...) does not
 *   compile (kMeshScale does not convert to Fix12<int>). A Fix12<int>
 *   local is a size-DIFF, 0xe8 to 0xf4. The scalar extern stays.
 */

#include "daObjBk_Kabe_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"

/* Two walls, one row each: model, KCL, then the CLPS block. */
struct KabeFiles {
    SharedFilePtr *model;
    SharedFilePtr *kcl;
    CLPS_Block *clps;
};

/* File-scope objects at the end of this file construct the four resource
 * handles (model files 0x593/0x595, collision files 0x594/0x596). mwcc
 * emits __sinit_daObjBk_Kabe_c.cpp from those definitions. The wrapper
 * names are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. Nothing in this TU
 * names the handles directly; the KabeFiles rows above point at them. */
struct BkKabeModelFile : SharedFilePtr {
    u32 words[2];

    BkKabeModelFile(u32 fileID);
    ~BkKabeModelFile();
};

struct BkKabeCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    BkKabeCollisionFilePtr(u32 fileID);
    ~BkKabeCollisionFilePtr();
};

extern "C" {
extern KabeFiles data_ov079_02128058[];

/* Writes the mesh active range at dBgW+0x0c. The definition is C and
 * takes int *, so this spelling matches it and the call casts. */
void func_020393a4(int *mesh, int range);

/* Scalar ABI. These members take Fix12<int> by value, and that spelling
 * does not compile against the ints this file actually passes. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale, short angY,
    CLPS_Block *clps);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, s32 x, s32 y, s32 z);
}

enum {
    kBreakableActor = 0x30, /* BK_KABE00 */
    kStarActor = 0xb2,
    kStarSpawnFlags = 0x40, /* howToSpawnStar 4, above the star index */
    kStarPopY = 0xc8000,    /* 200.0 */
    kBreakParticle = 0x121,
    kBreakSound = 0xf,
    kClsnRange = 0x240000,
    kMeshScale = 0x199
};

/* Source order is the ROM order. */
#pragma defer_codegen off

// @symbol _ZN14daObjBk_Kabe_cD1Ev
// @symbol _ZN14daObjBk_Kabe_cD0Ev
daObjBk_Kabe_c::~daObjBk_Kabe_c()
{
}

// @symbol _ZN14daObjBk_Kabe_c4KillEv
void daObjBk_Kabe_c::Kill()
{
    Sound::PlayBank3(kBreakSound, *(Vector3 *)&mCamSpacePosX);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(kBreakParticle, mPosX, mPosY, mPosZ);
    int isBreakable = (actorID == kBreakableActor);
    if (isBreakable) {
        mBroken = 1;
        return;
    }
    MarkForDestruction();
}

// @symbol _ZN14daObjBk_Kabe_c24OnHitByCannonBlastedCharER8dActor_c
int daObjBk_Kabe_c::OnHitByCannonBlastedChar(dActor_c &other)
{
    Kill();
}

// @symbol _ZN14daObjBk_Kabe_c16CleanupResourcesEv
int daObjBk_Kabe_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov079_02128058[mVariant].model->Release();
    data_ov079_02128058[mVariant].kcl->Release();
    return 1;
}

// @symbol _ZN14daObjBk_Kabe_c6RenderEv
int daObjBk_Kabe_c::Render()
{
    if (mBroken)
        return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjBk_Kabe_c8BehaviorEv
int daObjBk_Kabe_c::Behavior()
{
    int isBreakable = (actorID == kBreakableActor);
    if (isBreakable && mBroken) {
        if (mMeshCollider.IsEnabled())
            mMeshCollider.Disable();
        if (Sound::PlaySecretSound(this, &mBreakSoundState)) {
            Vector3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y += kStarPopY;
            dActor_c::Spawn(kStarActor, mStarId | kStarSpawnFlags, pos,
                            (Vector3_16 *)0, mAreaId, -1);
            MarkForDestruction();
        }
        return 1;
    }
    func_020393a4((int *)&mMeshCollider, kClsnRange);
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, kClsnRange, 0);
    return 1;
}

// @symbol _ZN14daObjBk_Kabe_c13InitResourcesEv
int daObjBk_Kabe_c::InitResources()
{
    int isBreakable = actorID == kBreakableActor;
    if (isBreakable) {
        mVariant = 0;
        mStarId = (u8)param1;
        if (mStarId == 0xff)
            mStarId = 0;
    } else {
        mVariant = 1;
    }
    int idx = mVariant;
    void *mdl = Model::LoadFile(*data_ov079_02128058[idx].model);
    mModel.SetFile((BMD_File *)mdl, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    int idx2 = mVariant;
    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(*data_ov079_02128058[idx2].kcl);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, kMeshScale, mAngleY,
        data_ov079_02128058[idx2].clps);
    return 1;
}

// @symbol daObjBk_Kabe_c_classInit_BK_KABE01
extern "C" daObjBk_Kabe_c *daObjBk_Kabe_c_classInit_BK_KABE01()
{
    return new daObjBk_Kabe_c();
}

// @symbol daObjBk_Kabe_c_classInit_BK_KABE00
extern "C" daObjBk_Kabe_c *daObjBk_Kabe_c_classInit_BK_KABE00()
{
    return new daObjBk_Kabe_c();
}

/* Static-init globals (was the handwritten __sinit_ov079_02127acc shard).
 * Definition order is the retail initializer's construction order. */
BkKabeModelFile data_ov079_02128350(0x593);
BkKabeCollisionFilePtr data_ov079_02128348(0x594);
BkKabeModelFile data_ov079_02128340(0x595);
BkKabeCollisionFilePtr data_ov079_02128358(0x596);
