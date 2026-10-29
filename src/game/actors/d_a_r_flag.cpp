//cpp
/* The flag at the end of Koopa the Quick's race. While it is armed,
 * Mario touching the cylinder stops the timer and plays sound 0x1f/0x14.
 * From frame 90 it retries 0x1f/0x7f until PlaySub accepts it. The model
 * sits 60 units above the actor. defer_codegen off: D1 then D0, no D2.
 */
// What stays:
// - The player test stays `b = (actorID == 0xbf)` as an int. A bool does
//   not match Behavior.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daRFlag_c_AssetId { ASSET_OBJ_RACE_FLAG_BMD = 0x0488, ASSET_OBJ_RACE_FLAG_WAIT_BCA = 0x0489 };

#include "daRFlag_c.h"
#include "decl_common.h"
#include "Timer.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "math/MtxStack.h"

struct BMD_File;
struct BCA_File;

extern Timer data_0209d4c8;

/* The model handle constructs through func_02017acc and registers
   func_02017ab4; the animation handle constructs through
   SharedFilePtr::Construct and registers SharedFilePtr_Destruct_Anim.
   The manifest aliases these. */
struct RFlagModelFileHandle : SharedFilePtr {
    RFlagModelFileHandle(u32 fileID);
    ~RFlagModelFileHandle();
};

struct RFlagAnimResFileHandle : SharedFilePtr {
    RFlagAnimResFileHandle(u32 fileID);
    ~RFlagAnimResFileHandle();
};

extern RFlagModelFileHandle data_ov062_0211e0d4;
extern RFlagAnimResFileHandle data_ov062_0211e0dc;

#pragma defer_codegen off

// @symbol _ZN9daRFlag_cD1Ev
// @symbol _ZN9daRFlag_cD0Ev
/* Empty body: the vptr store and the member destructors (dCcAc_c at 0xd4,
 * dExtAnmModel_c at 0x108, then dActor_c) are consequences of the class. D0's
 * deallocation is the inline operator delete on dActor_c. */
daRFlag_c::~daRFlag_c() {}

// @symbol _ZN9daRFlag_c17UpdateModelMatrixEv
/* Builds the model matrix from the actor's facing and position.
 * The flag graphic sits 60.0 (0x3c000) above mPosY. dExtSimpleModel_c-matrix
 * translation is in 1/8 world units, so the position is shifted down by 3. */
void daRFlag_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.t.x = mPosX >> 3;
    mAnmModel.mat4x3.t.y = (mPosY + 0x3c000) >> 3;
    mAnmModel.mat4x3.t.z = mPosZ >> 3;
}

// @symbol _ZN9daRFlag_c16CleanupResourcesEv
int daRFlag_c::CleanupResources() {
    data_ov062_0211e0d4.Release();
    data_ov062_0211e0dc.Release();
    return 1;
}

// @symbol _ZN9daRFlag_c6RenderEv
int daRFlag_c::Render() {
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN9daRFlag_c8BehaviorEv
int daRFlag_c::Behavior() {
    unsigned int id;
    dActor_c *a;
    int b;

    if (mHasTouchedFlag == 0) {
        id = mdCcAc_c.otherOwner;
        if (id != 0) {
            a = dActor_c::FindWithID(id);
            if (a != 0) {
                b = a->actorID;
                b = b == 0xBF;
                if (b) {
                    mHasTouchedFlag = 1;
                    data_0209d4c8.StopTimer();
                    mVictoryTimer = 1;
                    Sound::PlaySub(0x1F, 0x14, 0x7F, 0x6B000, 0);
                }
            }
        }
    }

    if (mVictoryTimer != 0) {
        mVictoryTimer += 1;
        if (mVictoryTimer >= 0x5A) {
            if (Sound::PlaySub(0x1F, 0x7F, 0, 0x8777, 0) != 0) {
                mVictoryTimer = 0;
            }
        }
    }

    mAnmModel.Advance();
    UpdateModelMatrix();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN9daRFlag_c13InitResourcesEv
int daRFlag_c::InitResources() {
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov062_0211e0d4), 1, -1);
    mAnmModel.SetAnim((BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov062_0211e0dc), 0, 0x1000, 0);
    mdCcAc_c.Init(this, 0x35555, 0x294000, 0x280000c, 0);
    mHasTouchedFlag = 0xff;
    mVictoryTimer = 0;
    return 1;
}

// @symbol daRFlag_c_classInit
extern "C" daRFlag_c *daRFlag_c_classInit(void) {
    return new daRFlag_c();
}

RFlagModelFileHandle data_ov062_0211e0d4(ASSET_OBJ_RACE_FLAG_BMD);
RFlagAnimResFileHandle data_ov062_0211e0dc(ASSET_OBJ_RACE_FLAG_WAIT_BCA);
