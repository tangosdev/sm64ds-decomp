//cpp
/* Production translation unit for ov079/daObjBkKillerdai_c.
 * 8 function(s), .text 0x02126dbc..0x021271b4. The Bullet Bill launcher
 * (BK_KILLER_DAI).
 *
 * Behavior fires actor 0xde when none is already out and the player
 * is in front of the barrel. InitResources loads the launcher model
 * and mesh, and leaves the actor dormant on two courses.
 *
 * NAME: _ZTS18daObjBkKillerdai_c is "18daObjBkKillerdai_c" at ov079
 * 0x02127f7c; _ZTI at 0x02127f70 reads [__si_class_type_info, that string,
 * _ZTI10dBgActor_c]. The tree previously called the class BillBlaster
 * (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02126dbc), D0
 * (0x02126e00), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * The factory daObjBkKillerdai_c_classInit (0x021271b4) sits just past this
 * run's right edge and stays a one-function source.
 */

#include "decl_common.h"
#include "daObjBkKillerdai_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "Player.h"

/* Launcher model, mesh, and the two shared files the bill itself uses.
 * The table is three pointers: model, collision, CLPS block. */
extern SharedFilePtr data_ov079_02128300;
extern SharedFilePtr data_ov079_021282f0;
extern void *data_ov079_02127f64[];

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern int _ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(void *self, short a, short b, short c, int d);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, const void *mtx, int scale, short angY, void *clps);
extern int IsStarCollectedInCurLevel(int starID);
extern s8 data_0209f2f8;
extern int data_0209caa0[];
extern u8 data_0209f220;
extern int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
}

#pragma defer_codegen off

// @symbol _ZN18daObjBkKillerdai_cD1Ev
// @symbol _ZN18daObjBkKillerdai_cD0Ev
daObjBkKillerdai_c::~daObjBkKillerdai_c()
{
}

// @symbol _ZN18daObjBkKillerdai_c4KillEv
/* Leftover: Particle::System::NewSimple takes Fix12<int> by value, so it
 * stays mangled. dustPos is stored memberwise; a Vector3 assignment does not
 * match. */
void daObjBkKillerdai_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x96000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8d, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0xf, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}

// @symbol _ZN18daObjBkKillerdai_c15OnHitByMegaCharER6Player
void daObjBkKillerdai_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    KillByMegaChar(player);
    mAngleY = mPrevAngleY;
}

// @symbol _ZN18daObjBkKillerdai_c16CleanupResourcesEv
s32 daObjBkKillerdai_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov079_02127f64[0])->Release();
    ((SharedFilePtr *)data_ov079_02127f64[1])->Release();
    data_ov079_02128300.Release();
    data_ov079_021282f0.Release();
    return 1;
}

// @symbol _ZN18daObjBkKillerdai_c6RenderEv
s32 daObjBkKillerdai_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjBkKillerdai_c8BehaviorEv
/* Leftover: dBgActor_c::UpdateKillByMegaChar and dBgActor_c::IsClsnInRange
 * take Fix12<int> by value, so they stay mangled.
 * mPosY is widened through a long long before the spawn-height add;
 * a plain add misses. */
s32 daObjBkKillerdai_c::Behavior()
{
    if (_ZN10dBgActor_c20UpdateKillByMegaCharEsss5Fix12IiE(this, 0x2000, 0, 0, 0xc8000))
        return 1;
    if (!dActor_c::FindWithID(mSpawnedUniqueID)) {
        Player *player = ClosestPlayer();
        int dist = Vec3_HorzDist((Vector3 *)&mPosX, (Vector3 *)&player->mPosX);
        s16 ang = Vec3_HorzAngle((Vector3 *)&mPosX, (Vector3 *)&player->mPosX);
        if (AngleDiff(ang, mAngleY) < 0x2000 && dist > 0x320000 && dist < 0x5dc000) {
            long long yll = mPosY;
            int z = mPosZ;
            int x = mPosX;
            int y = (int)yll + 0x96000;
            Vector3 pos;
            pos.x = x;
            pos.z = z;
            pos.y = y;
            dActor_c *spawned = dActor_c::Spawn(0xde, 0, pos, (Vector3_16 *)&mAngleX,
                                                 mAreaId, -1);
            mSpawnedUniqueID = spawned->uniqueID;
            *(u32 *)((char *)spawned + 0x3dc) = (u32)this;
        }
    }
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0);
    return 1;
}

// @symbol _ZN18daObjBkKillerdai_c13InitResourcesEv
/* Leftover: dBgW_KcMbg::SetFile takes Fix12<int> by value, so it stays
 * mangled. */
s32 daObjBkKillerdai_c::InitResources()
{
    void *file;
    s8 course;

    Model::LoadFile(data_ov079_02128300);
    Model::LoadFile(data_ov079_021282f0);
    file = Model::LoadFile(*(SharedFilePtr *)data_ov079_02127f64[0]);
    mModel.SetFile((BMD_File *)file, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    file = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov079_02127f64[1]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, file, &mClsnMat, 0x199, mAngleY, data_ov079_02127f64[2]);

    mSpawnedUniqueID = 0;

    course = data_0209f2f8;
    if (course == 0x12) {
        if (data_0209caa0[1] & 0x204)
            return 0;
    }
    if (course == 7) {
        if (data_0209f220 != 1) {
            if (IsStarCollectedInCurLevel(1) != 0)
                goto alive;
        }
        if (mPosY >= 0xdac000)
            return 0;
    }
alive:
    return 1;
}
