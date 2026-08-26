//cpp
// @symbol _ZN13TreasureChest13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * ModelAnim::SetAnim and dCcAc_c::Init keep their C ABI spellings here. Their
 * real declarations pass Fix12<int> by value, which mwccarm lowers differently
 * at a C++ call site (the documented Fix12 caller wall); the scalar ABI forms
 * are the measured ROM calls. Everything else uses the recovered class API.
 */
#include "TreasureChest.h"
#include "SharedFilePtr.h"

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed, u32 startFrame);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);
}
extern SharedFilePtr data_ov002_0210d9a8;
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov064_0211c96c;
extern SharedFilePtr data_ov064_0211c964;

int TreasureChest::InitResources()
{
    Model::LoadFile(data_ov002_0210d9a8);
    Model::LoadFile(data_ov002_0210da38);
    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(data_ov064_0211c96c), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        (BCA_File *)Animation::LoadFile(data_ov064_0211c964),
        0x40000000, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x96000, 0x96000, 0x200004, 0);
    UpdateModelTransform();
    mOrder = param1;
    mStarID = (u32)param1 >> 8;
    if (mStarID != 0xff) {
        mTrackedStarID = TrackStar(mStarID, 2);
    }
    return 1;
}
