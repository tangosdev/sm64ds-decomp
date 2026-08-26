//cpp
// @symbol _ZN4Flag13InitResourcesEv
#include "Flag.h"
#include "SharedFilePtr.h"

extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *, BCA_File *, int, int, unsigned int);

extern SharedFilePtr data_ov009_02113eb8;
extern SharedFilePtr data_ov009_02113eb0;

int Flag::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov009_02113eb8);
    mModelAnim.SetFile(modelFile, 1, -1);
    BCA_File *animFile = (BCA_File *)Animation::LoadFile(data_ov009_02113eb0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, animFile, 0, 0x1000, 0);
    return 1;
}
