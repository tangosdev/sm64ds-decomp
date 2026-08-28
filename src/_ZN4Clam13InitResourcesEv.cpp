//cpp
// @symbol _ZN4Clam13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Clam.h"
#include "SharedFilePtr.h"

/* Both calls below carry Fix12<T> by value, a proven CodeWarrior source wall.
 * Keep those ABI seams explicit while typing their receivers and file data. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *model, BCA_File *file, int flags, int speed, u32 startFrame);
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    u32 flags, u32 vulnFlags);
extern "C" void func_ov064_0211a9b4(Clam *clam);

extern SharedFilePtr data_ov064_0211c9c4;
extern SharedFilePtr data_ov064_0211c9cc;
extern SharedFilePtr data_ov064_0211c9bc;

int Clam::InitResources()
{
    Animation::LoadFile(data_ov064_0211c9cc);
    Animation::LoadFile(data_ov064_0211c9bc);
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov064_0211c9c4);
    mModelAnim.SetFile(modelFile, 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        *(BCA_File **)((u8 *)&data_ov064_0211c9bc + 4),
        0x40000000, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x64000, 0x64000, 0x200004, 0);
    func_ov064_0211a9b4(this);
    mState = 0;
    return 1;
}
