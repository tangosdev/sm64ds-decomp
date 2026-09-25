//cpp
/**
 * Bowser in the Fire Sea rising platform.
 *
 * Waits at its spawn height until the player stands on it, pauses 20
 * frames, rises 10.0 a frame to 1500.0 above that height, pauses 20
 * frames again, then sinks 10.0 a frame back down and waits for the
 * next rider. The mesh collider carries whoever stands on it through
 * dBgW::UpdatePosWithTransform; the second callback word is the
 * func_ov045_021114c8 / func_ov045_021114a8 pair below, which is how
 * Behavior learns the player is aboard.
 *
 * daObjKm2_Agaru_c_classInit is reconstructed (RTTI daObjKm2_Agaru_c,
 * KM2_AGARU registry). Retail does not store that spelling.
 * Historical alias: FireSeaElevator_Spawn.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm emits one .text
 * section per function in the reverse of source order.
 *
 * Known limits:
 * - dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled:
 *   both take Fix12<int> by value (notes/mwccarm-codegen.md 6az).
 * - func_020393d4 / func_020393c4 install dBgW's two callback words;
 *   dBgW carries no setter for them.
 * - data_ov045_02113180 / 02113188 are this overlay's KCL / BMD file
 *   handles and data_ov045_021125d0 the CLPS block, all in overlay data
 *   this TU does not own.
 */

#include "daObjKm2_Agaru_c.h"
#include "SharedFilePtr.h"

struct KCL_File;
struct CLPS_Block;

extern "C" {
extern SharedFilePtr data_ov045_02113180; /* the KCL */
extern SharedFilePtr data_ov045_02113188; /* the BMD */
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
