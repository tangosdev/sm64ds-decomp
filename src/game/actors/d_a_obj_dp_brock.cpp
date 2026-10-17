//cpp
/* Production translation unit for ov025/daObjDpBrock_c.
 * 9 function(s), .text 0x02111d40..0x021120e4, the factory included.
 *
 * DP_BROCK, a pyramid step block (ov025). It sinks 5 units a frame for 100
 * frames, rises for 100, and repeats; param1 & 3 picks the phase it starts in.
 * The block carries a model and a collider matrix of its own besides
 * dBgActor_c's, and UpdateStepModelPosAndRotY and UpdateStepClsnPosAndRot
 * keep them on the actor's position and heading.
 *
 * NAME: daObjDpBrock_c is the cartridge's RTTI spelling. _ZTS at ov025
 * 0x021138dc is the string "14daObjDpBrock_c", and the _ZTI at 0x021138d0
 * names the vtable at 0x02113914 as this class's. The tree called the class
 * PyramidStep until then.
 *
 * The destructor is inline and empty in the class header, so Behavior is the
 * key function and this TU emits _ZTV14daObjDpBrock_c and the RTTI chain.
 * D1 and D0 come from the header, below every function written here.
 *
 * Function order is the reverse of the ROM's: mwccarm 2004/b56 emits one .text
 * section per function in the reverse of source order, so the highest-address
 * ROM function is written first. Do not reorder.
 */

// leftovers (each measured; the noted form size-DIFFs or breaks the build):
// SetFile stays the free call with int 0x1000 (method spellings grow
// InitResources; Fix12<int>{0x1000} does not compile -- this TUs InitResources).
// IsClsnInRange stays the free call with two ints (method spellings grow
// Behavior; Fix12<int>{0} does not compile).
// mMeshCollider.beforeClsnCallback goes through func_020393d4 (direct member
// store does not compile; the C-cast store shrinks InitResources).
// mClsnMat2 and mStepModel.mat4x3 stay flat s32 m[12] (.t does not compile;
// Vector3 overlays and address temps move bytes in both updates).
// InitResources, Behavior and the updates keep scalar pos (Vector3 refs grow them).
// mStateTimer++ and mStateTimer == kFramesPerState stay (the +1/+50 forms
// move bytes).

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjDpBrock_c_AssetId {
    ASSET_DP_BROCK_BMD = 0x05df,
    ASSET_DP_BROCK_KCL = 0x05e0
};

#include "daObjDpBrock_c.h"
#include "SharedFilePtr.h"

/* 8-byte file handles. The model uses func_02017acc / func_02017ab4 and the
 * collision file func_02017b4c / SharedFilePtr_Destruct_Clsn. The spellings
 * are local; the manifest aliases the generated names to those ROM symbols. */
struct BrockModelFilePtr : SharedFilePtr {
    u32 words[2];

    BrockModelFilePtr(u32 fileID);
    ~BrockModelFilePtr();
};

struct BrockCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    BrockCollisionFilePtr(u32 fileID);
    ~BrockCollisionFilePtr();
};

extern "C" {
/* The block's model file and collision file (ov025 .bss), and the CLPS
   block handed to dBgW_KcMbg::SetFile. */
extern BrockModelFilePtr data_ov025_02113ab8;
extern BrockCollisionFilePtr data_ov025_02113ab0;
extern CLPS_Block data_ov025_02112ce8;

void Matrix4x3_FromRotationY(void *m, int angleY);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angleY, CLPS_Block *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void func_020393d4(int *p, int v);
}

/* mState values, how long each lasts, and the 20.12 step.
 * kVertStep is 5.0 per frame. A full drop is that times
 * kFramesPerState; phase 1 starts at half of it. */
enum {
    kSinking = 0,
    kRising = 1,
    kFramesPerState = 100,
    kVertStep = 0x5000,
    kHalfDrop = 0xfa000,
    kFullDrop = 0x1f4000
};

// @symbol daObjDpBrock_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjDpBrock_c through RTTI,
 * allocation size, vtable identity and the DP_BROCK registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: PyramidStep_Spawn.
 *
 * Every instruction here falls out of the one `new`: operator new(932), the
 * out-of-line dBgActor_c base constructor, this class's vptr and the Model
 * constructor for mStepModel all come from the implicit constructor the
 * `new` inlines, and the null check is the one `new` itself emits. */
extern "C" daObjDpBrock_c *daObjDpBrock_c_classInit()
{
    return new daObjDpBrock_c();
}

// @symbol _ZN14daObjDpBrock_c13InitResourcesEv
int daObjDpBrock_c::InitResources()
{
    mStepModel.SetFile((BMD_File *)Model::LoadFile(data_ov025_02113ab8), 1, -1);
    UpdateStepModelPosAndRotY();
    UpdateStepClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov025_02113ab0),
        &mClsnMat2, 0x1000, mAngleY, &data_ov025_02112ce8);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);

    int phase = param1 & 3;
    mVertSpeed = -kVertStep;
    mState = kSinking;
    mStateTimer = 0;
    switch (phase) {
    case 0:
        break;
    case 1:
        /* Halfway down, halfway through the sink. */
        mPosY -= kHalfDrop;
        mStateTimer += kFramesPerState / 2;
        break;
    case 2:
        /* At the bottom, about to rise. */
        mPosY -= kFullDrop;
        mState = kRising;
        mVertSpeed = kVertStep;
        break;
    }
    return 1;
}

// @symbol _ZN14daObjDpBrock_c8BehaviorEv
int daObjDpBrock_c::Behavior()
{
    switch (mState) {
    case kSinking:
        if (mStateTimer == kFramesPerState) {
            mState = kRising;
            mVertSpeed = kVertStep;
            mStateTimer = 0;
        }
        break;
    case kRising:
        if (mStateTimer == kFramesPerState) {
            mState = kSinking;
            mVertSpeed = -kVertStep;
            mStateTimer = 0;
        }
        break;
    }
    mStateTimer++;
    mPosY += mVertSpeed;
    UpdateStepModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateStepClsnPosAndRot();
    return 1;
}

// @symbol _ZN14daObjDpBrock_c6RenderEv
int daObjDpBrock_c::Render()
{
    mStepModel.Render(0);
    return 1;
}

// @symbol _ZN14daObjDpBrock_c16CleanupResourcesEv
int daObjDpBrock_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov025_02113ab8.Release();
    data_ov025_02113ab0.Release();
    return 1;
}

// @symbol _ZN14daObjDpBrock_c25UpdateStepModelPosAndRotYEv
/* Puts the block's own model on the actor: heading from mAngleY, and the
 * position scaled down by 8 into model space. dBgActor_c's
 * UpdateModelPosAndRotY does the same for the inherited model. */
void daObjDpBrock_c::UpdateStepModelPosAndRotY()
{
    Matrix4x3_FromRotationY(&mStepModel.mat4x3, mAngleY);
    mStepModel.mat4x3.m[9]  = mPosX >> 3;
    mStepModel.mat4x3.m[10] = mPosY >> 3;
    mStepModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN14daObjDpBrock_c23UpdateStepClsnPosAndRotEv
/* Moves the collider with the block: rotation from mAngleY, translation
 * from the actor position, then dBgW_KcMbg::Transform. dBgActor_c's
 * UpdateClsnPosAndRot does the same for the inherited collider. */
void daObjDpBrock_c::UpdateStepClsnPosAndRot()
{
    Matrix4x3_FromRotationY(&mClsnMat2, mAngleY);
    mClsnMat2.m[9]  = mPosX;
    mClsnMat2.m[10] = mPosY;
    mClsnMat2.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat2, mAngleY);
}

/* D1 (0x02111d40) and D0 (0x02111d8c) have no text here. include/
 * daObjDpBrock_c.h defines ~daObjDpBrock_c() in the class body, and that
 * alone makes mwccarm emit the pair last, in the cartridge's D1-then-D0
 * order, with no D2. */
// @symbol _ZN14daObjDpBrock_cD1Ev
// @symbol _ZN14daObjDpBrock_cD0Ev

/* Source order is construction order: model file 1503, collision file 1504.
 * __sinit_d_a_obj_dp_brock.cpp emits both constructions and registers the
 * destructors; the registration nodes are compiler temporaries. */
BrockModelFilePtr data_ov025_02113ab8(ASSET_DP_BROCK_BMD);
BrockCollisionFilePtr data_ov025_02113ab0(ASSET_DP_BROCK_KCL);
