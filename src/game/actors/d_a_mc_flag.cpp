//cpp
/* FLAG (342) -- ov009/daMcFlag_c, castle grounds flag.
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI ov009:0x02113b60 names daMcFlag_c; the debug table names MC_FLAG.
 * This is the flag, not metalnet and not castle water.
 *
 * Factory is `return new daMcFlag_c()`, through the inherited fBase_c::operator
 * new (`_ZN7fBase_cnwEm`). Definitions run in reverse address
 * order for mwccarm 2004/b56. The inline destructor in the header emits the
 * required D1/D0 pair; `new` odr-uses the class so that pair is emitted here.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_mc_flag_AssetId { ASSET_MC_FLAG_BMD = 0x068d, ASSET_MC_FLAG_WAIT_BCA = 0x068e };

#include "daMcFlag_c.h"
#include "SharedFilePtr.h"
#include "math/MtxStack.h"

/* The model handle constructs through func_02017acc and registers
   func_02017ab4; the animation handle constructs through
   SharedFilePtr::Construct and registers SharedFilePtr_Destruct_Anim.
   The manifest aliases these. */
struct McFlagModelFileHandle : SharedFilePtr {
    McFlagModelFileHandle(u32 fileID);
    ~McFlagModelFileHandle();
};

struct McFlagAnimResFileHandle : SharedFilePtr {
    McFlagAnimResFileHandle(u32 fileID);
    ~McFlagAnimResFileHandle();
};

extern McFlagModelFileHandle data_ov009_02113eb8;
extern McFlagAnimResFileHandle data_ov009_02113eb0;

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daMcFlag_c_classInit
extern "C" daMcFlag_c *daMcFlag_c_classInit() {
    return new daMcFlag_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c13InitResourcesEv
s32 daMcFlag_c::InitResources() {
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov009_02113eb8), 1, -1);
    mAnmModel.SetAnim((BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov009_02113eb0), 0, 0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c8BehaviorEv
s32 daMcFlag_c::Behavior() {
    mAnmModel.Advance();
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.t.x = mPosX >> 3;
    mAnmModel.mat4x3.t.y = mPosY >> 3;
    mAnmModel.mat4x3.t.z = mPosZ >> 3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c6RenderEv
s32 daMcFlag_c::Render() {
    mAnmModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c16CleanupResourcesEv
s32 daMcFlag_c::CleanupResources() {
    data_ov009_02113eb8.Release();
    data_ov009_02113eb0.Release();
    return 1;
}

// @symbol _ZN10daMcFlag_cD0Ev
// @symbol _ZN10daMcFlag_cD1Ev

McFlagModelFileHandle data_ov009_02113eb8(ASSET_MC_FLAG_BMD);
McFlagAnimResFileHandle data_ov009_02113eb0(ASSET_MC_FLAG_WAIT_BCA);
