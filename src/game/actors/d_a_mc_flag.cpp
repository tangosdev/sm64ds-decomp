//cpp
/* FLAG (342) -- ov009/daMcFlag_c, castle grounds flag.
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI ov009:0x02113b60 names daMcFlag_c; the debug table names MC_FLAG.
 * This is the flag, not metalnet and not castle water.
 *
 * Factory is `return new daMcFlag_c()`. Leaf operator new(unsigned long)
 * forwards `_ZN7fBase_cnwEj` until #2570. Definitions run in reverse address
 * order for mwccarm 2004/b56. The inline destructor in the header emits the
 * required D1/D0 pair; `new` odr-uses the class so that pair is emitted here.
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim stays mangled (Fix12<int> by value, wall 6az)
 * - data_ov009_02113eb8 / 02113eb0 SharedFilePtr handles: this TU consumes
 *   them; overlay .bss owns them. symbols.txt has no recovered names, so
 *   they are not coined.
 */

#include "daMcFlag_c.h"
#include "SharedFilePtr.h"

extern "C" SharedFilePtr data_ov009_02113eb8;
extern "C" SharedFilePtr data_ov009_02113eb0;

extern "C" {
void Matrix4x3_FromRotationY(void *, s16);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self,
                                                BCA_File *animFile,
                                                int flags, int speed,
                                                u32 startFrame);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daMcFlag_c_classInit
extern "C" daMcFlag_c *daMcFlag_c_classInit()
{
    return new daMcFlag_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c13InitResourcesEv
s32 daMcFlag_c::InitResources()
{
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov009_02113eb8), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)Animation::LoadFile(data_ov009_02113eb0), 0,
        0x1000, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c8BehaviorEv
s32 daMcFlag_c::Behavior()
{
    mModelAnim.Advance();
    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.t.x = mPosX >> 3;
    mModelAnim.mat4x3.t.y = mPosY >> 3;
    mModelAnim.mat4x3.t.z = mPosZ >> 3;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c6RenderEv
s32 daMcFlag_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_c16CleanupResourcesEv
s32 daMcFlag_c::CleanupResources()
{
    data_ov009_02113eb8.Release();
    data_ov009_02113eb0.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/*   _ZN10daMcFlag_cD1Ev  0x02112078  size 0x30  (complete-object destructor) */
/*   _ZN10daMcFlag_cD0Ev  0x021120a8  size 0x44  (deleting destructor)        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daMcFlag_cD0Ev
// @symbol _ZN10daMcFlag_cD1Ev
