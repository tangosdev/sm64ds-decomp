//cpp
/* leftovers:
 * - dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled.
 *   Both take Fix12<int> by value.
 * - func_020393d4 and func_020393c4 install dBgW's two callback words.
 *   dBgW has no setter for them.
 * - data_ov045_02113180 and 02113188 are this TU's KCL and BMD handles.
 *   data_ov045_021125d0, the CLPS block, stays overlay data.
 *
 * daObjKm2_Agaru_c, the Fire Sea rising platform. It waits, pauses 20
 * frames, rises 10.0 a frame to 1500.0, pauses, then sinks back.
 * The mesh carries the rider through dBgW::UpdatePosWithTransform.
 * The aboard test is func_ov045_021114c8 / func_ov045_021114a8.
 * classInit is reconstructed. Retail does not store that spelling.
 */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_obj_km2_agaru_AssetId {
    ASSET_KM2_AGARU_BMD = 0x065b,
    ASSET_KM2_AGARU_KCL = 0x065c
};

#include "daObjKm2_Agaru_c.h"
#include "SharedFilePtr.h"

struct KCL_File;
struct CLPS_Block;

/* Retail constructs these 8-byte handles in this order and registers
 * their destructors. The wrapper names are local; the ctor/dtor
 * addresses are the ROM resource-family functions. */
struct AgaruModelFilePtr : SharedFilePtr {
    u32 words[2];

    AgaruModelFilePtr(u32 fileID);
    ~AgaruModelFilePtr();
};

struct AgaruCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    AgaruCollisionFilePtr(u32 fileID);
    ~AgaruCollisionFilePtr();
};

extern "C" {
extern AgaruCollisionFilePtr data_ov045_02113180; /* the KCL */
extern AgaruModelFilePtr data_ov045_02113188;     /* the BMD */
extern CLPS_Block data_ov045_021125d0;

void func_020393d4(int *collider, int callback);
void func_020393c4(int *collider, int callback);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(
    dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *kcl, const Matrix4x3 *mtx, int scale,
    s16 angleY, CLPS_Block *clps);

void func_ov045_021114a8(daObjKm2_Agaru_c *self, dActor_c *other);
void func_ov045_021114c8(void *collider, daObjKm2_Agaru_c *self,
                         dActor_c *other);
}

enum {
    kPlayerActorID = 0xbf,
    kWaitFrames = 20,
    kRideSpeed = 0xa000,     /* 10.0 */
    kRideHeight = 0x5dc000,  /* 1500.0 */
};

// @symbol daObjKm2_Agaru_c_classInit
extern "C" daObjKm2_Agaru_c *daObjKm2_Agaru_c_classInit()
{
    return new daObjKm2_Agaru_c();
}

// @symbol func_ov045_021114c8
/* The collider's second callback word: drop the collider, forward the pair. */
extern "C" void func_ov045_021114c8(void *collider, daObjKm2_Agaru_c *self,
                                    dActor_c *other)
{
    func_ov045_021114a8(self, other);
}

// @symbol func_ov045_021114a8
extern "C" void func_ov045_021114a8(daObjKm2_Agaru_c *self, dActor_c *other)
{
    int isPlayer = other->actorID == kPlayerActorID;
    if (isPlayer)
        self->mStoodOn = true;
}

// @symbol _ZN16daObjKm2_Agaru_c13InitResourcesEv
s32 daObjKm2_Agaru_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov045_02113188), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    KCL_File *kcl = (KCL_File *)dBgW_Kc::LoadFile(data_ov045_02113180);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
        &data_ov045_021125d0);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    func_020393c4((int *)&mMeshCollider, (int)&func_ov045_021114c8);

    mWaitTimer = 0;
    mState = 0;
    mRestY = mPosY;
    return 1;
}

// @symbol _ZN16daObjKm2_Agaru_c8BehaviorEv
s32 daObjKm2_Agaru_c::Behavior()
{
    switch (mState) {
    case 0:
        if (mStoodOn)
            mState++;
        break;
    case 1:
        if (mWaitTimer >= kWaitFrames) {
            mPosY += kRideSpeed;
            int top = mRestY + kRideHeight;
            if (mPosY >= top) {
                mPosY = top;
                mState++;
                mWaitTimer = 0;
            }
        } else {
            mWaitTimer++;
        }
        break;
    case 2:
        if (mWaitTimer >= kWaitFrames) {
            mPosY -= kRideSpeed;
            int bottom = mRestY;
            if (mPosY <= bottom) {
                mPosY = bottom;
                mState = 0;
                mWaitTimer = 0;
            }
        } else {
            mWaitTimer++;
        }
        break;
    }
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x1f4000, 0))
        UpdateClsnPosAndRot();
    mStoodOn = false;
    return 1;
}

// @symbol _ZN16daObjKm2_Agaru_c6RenderEv
s32 daObjKm2_Agaru_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjKm2_Agaru_c16CleanupResourcesEv
s32 daObjKm2_Agaru_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov045_02113188.Release();
    data_ov045_02113180.Release();
    return 1;
}

// @symbol _ZN16daObjKm2_Agaru_cD1Ev
// @symbol _ZN16daObjKm2_Agaru_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline destructor in the header
   emits D1 then D0 -- the cartridge's order -- and no D2. */

/* The static-init globals -- mwcc emits __sinit_d_a_obj_km2_agaru.cpp
   from these: one ctor veneer plus destructor registration per handle. */
AgaruModelFilePtr data_ov045_02113188(ASSET_KM2_AGARU_BMD);
AgaruCollisionFilePtr data_ov045_02113180(ASSET_KM2_AGARU_KCL);
