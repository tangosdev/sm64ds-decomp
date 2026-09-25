//cpp
/* Production translation unit for ov079/daObjBk_Kabe_c.
 * 8 function(s), .text 0x021271e4..0x0212757c. Bowser's castle walls
 * (BK_KABE00 / BK_KABE01). The two classInit factories sit in the next
 * run and are not part of this TU.
 *
 * NAME: _ZTS14daObjBk_Kabe_c is "14daObjBk_Kabe_c" at ov079 0x02128044; _ZTI
 * at 0x02128038 reads [__si_class_type_info, that string, _ZTI10dBgActor_c].
 * The tree previously called the class FortressWall (coined; vtable address
 * only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x021271e4), D0
 * (0x02127228), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 */

#include "decl_common.h"
#include "daObjBk_Kabe_c.h"
#include "Sound.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "dBgW_Kc.h"

struct FileSlot {
    SharedFilePtr *file;
    int pad[2];
};

extern "C" {
extern FileSlot data_ov079_02128058[];
extern FileSlot data_ov079_0212805c[];
extern FileSlot data_ov079_02128060[];

void func_020393a4(int *collider, int range);
/* dBgActor_c::IsClsnInRange, dBgW_KcMbg::SetFile and Particle::System::NewSimple
   take Fix12<int> by value, which mwccarm passes differently at the call site,
   so they stay mangled. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const Matrix4x3 *mtx, int scale, short angY, void *clps);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, s32 x, s32 y, s32 z);
}

enum {
    kBreakableActor = 0x30, /* BK_KABE00 */
    kStarActor = 0xb2,
    kStarSpawnFlags = 0x40,
    kStarPopY = 0xc8000,    /* 200.0 */
    kBreakParticle = 0x121,
    kBreakSound = 0xf,
    kClsnRange = 0x240000,
    kMeshScale = 0x199
};

#pragma defer_codegen off

// @symbol _ZN14daObjBk_Kabe_cD1Ev
// @symbol _ZN14daObjBk_Kabe_cD0Ev
daObjBk_Kabe_c::~daObjBk_Kabe_c()
{
}

// @symbol _ZN14daObjBk_Kabe_c4KillEv
/* Kill: `if (actorID == kBreakableActor)` folds the compare into one
   branch and DIFFs. The ROM materialises it into a register first. */
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
    data_ov079_02128058[mVariant].file->Release();
    data_ov079_0212805c[mVariant].file->Release();
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
/* Behavior: `if (actorID == kBreakableActor && mBroken)` DIFFs the same
   way Kill's folded compare does. */
int daObjBk_Kabe_c::Behavior()
{
    int isBreakable = (actorID == kBreakableActor);
    if (isBreakable != 0 && mBroken != 0) {
        if (mMeshCollider.IsEnabled() != 0)
            mMeshCollider.Disable();
        if (Sound::PlaySecretSound(this, &mBreakSoundState) != 0) {
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
/* InitResources: indexing the file rows with mVariant directly DIFFs;
   the ROM copies it into a local first. dBgW_KcMbg::SetFile as a member
   call DIFFs too (Fix12<int> by value is homed, and a reloc lands on the
   wrong file-row symbol). */
int daObjBk_Kabe_c::InitResources()
{
    int isBreakable = (int)(actorID == kBreakableActor);
    if (isBreakable != 0) {
        mVariant = 0;
        mStarId = (u8)param1;
        if (mStarId == 0xff)
            mStarId = 0;
    } else {
        mVariant = 1;
    }
    {
        int idx = mVariant;
        void *mdl = Model::LoadFile(*data_ov079_02128058[idx].file);
        mModel.SetFile((BMD_File *)mdl, 1, -1);
        UpdateModelPosAndRotY();
        UpdateClsnPosAndRot();
        {
            int idx2 = mVariant;
            void *kcl = dBgW_Kc::LoadFile(*data_ov079_0212805c[idx2].file);
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider, kcl, &mClsnMat, kMeshScale, mAngleY,
                data_ov079_02128060[idx2].file);
        }
    }
    return 1;
}
