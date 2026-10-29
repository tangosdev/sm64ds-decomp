//cpp
/* Production translation unit for ov002/daObjHeart_c.
 * 8 function(s), .text 0x020bc5e0..0x020bc8f4. The HEART healing pickup.
 *
 * NAME: _ZTS12daObjHeart_c is "12daObjHeart_c" at ov002 0x02109c40; _ZTI at
 * 0x02109c34 reads [__si_class_type_info, that string, _ZTI8dActor_c]. The tree
 * previously called the class HealingHeart (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x020bc5e0), D0
 * (0x020bc618), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 *
 * UpdateModelMatrix is a coined name; the cartridge records no name for it.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjHeart_c_AssetId { ASSET_OBJ_HEART_BMD = 0x0470, ASSET_OBJ_HEART_WAIT_BCA = 0x0471 };

#pragma defer_codegen off

#include "common.h"
#include "daObjHeart_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4; the animation handle constructs through
 * SharedFilePtr::Construct and destroys through SharedFilePtr_Destruct_Anim.
 * The manifest aliases those undefined members onto the ROM symbols. */
struct HeartModelFilePtr : SharedFilePtr {
    HeartModelFilePtr(u32 fileID);
    ~HeartModelFilePtr();
};

struct HeartAnimationFilePtr : SharedFilePtr {
    HeartAnimationFilePtr(u32 fileID);
    ~HeartAnimationFilePtr();
};

extern HeartModelFilePtr data_ov002_0210e104;
extern HeartAnimationFilePtr data_ov002_0210e0fc;

/* local extern: no shared header declares it. */
extern "C" int Math_Function_0203b14c(int *ptr, int target, int rate, int limit, int step);

// @symbol _ZN12daObjHeart_cD1Ev
// @symbol _ZN12daObjHeart_cD0Ev
daObjHeart_c::~daObjHeart_c() {}

// @symbol _ZN12daObjHeart_c17UpdateModelMatrixEv
void daObjHeart_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN12daObjHeart_c16CleanupResourcesEv
s32 daObjHeart_c::CleanupResources() {
    data_ov002_0210e104.Release();
    data_ov002_0210e0fc.Release();
    return 1;
}

// @symbol _ZN12daObjHeart_c6RenderEv
s32 daObjHeart_c::Render() {
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN12daObjHeart_c8BehaviorEv
s32 daObjHeart_c::Behavior() {
    s32 spin = 0x1000;
    DecIfAbove0_Byte(&mHealTimer);

    if (mdCcAc_c.otherOwner != 0) {
        if (mWasTouched == 0) {
            mWasTouched = 1;
            mHealTimer = 0;
        }
        if (mHealTimer == 0) {
            dActor_c *other = dActor_c::FindWithID(mdCcAc_c.otherOwner);
            if (other != 0) {
                int isPlayer = (int)(other->actorID == 0xbf);
                if (isPlayer != 0) {
                    s32 scaled = other->mHorzSpeed / 0x1000;
                    s32 amount = 0x100;
                    if (scaled >= 8) {
                        scaled -= 8;
                        amount = (scaled / 4 + 1) << 8;
                    }
                    ((Player *)other)->Heal(amount);
                    Sound::PlayBank3(0x61, mCamSpacePos);
                }
            }
            mHealTimer = 0x5a;
        }
    } else {
        mWasTouched = 0;
    }

    if (mHealTimer > 0x2d)
        spin = 0x8000;
    Math_Function_0203b14c(&mAnimSpeed, spin, 0x400, 0x2000, 0x80);
    mAnmModel.speed = mAnimSpeed;
    mAnmModel.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN12daObjHeart_c13InitResourcesEv
s32 daObjHeart_c::InitResources() {
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210e104), 1, -1);
    mAnmModel.SetAnim((BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov002_0210e0fc), 0, 0x1000, 0);
    mdCcAc_c.Init(this, 0x3c000, 0x78000, 0x100002, 0x8000);
    mAnimSpeed = 0x1000;
    UpdateModelMatrix();
    return 1;
}

// @symbol daObjHeart_c_classInit
/* Reconstructed source-style name; historical alias HealingHeart_Spawn. */
extern "C" daObjHeart_c *daObjHeart_c_classInit(void) {
    return new daObjHeart_c();
}

/* Retail initializer order: model 0x470, then animation 0x471. mwcc emits
 * __sinit_d_a_obj_heart.cpp from these two definitions. */
HeartModelFilePtr data_ov002_0210e104(ASSET_OBJ_HEART_BMD);
HeartAnimationFilePtr data_ov002_0210e0fc(ASSET_OBJ_HEART_WAIT_BCA);
