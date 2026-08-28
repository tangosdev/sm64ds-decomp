//cpp
// @symbol _ZN4Fish13InitResourcesEv
/* recovered: typed ModelAnim and shared-file setup */
#include "Fish.h"
#include "SharedFilePtr.h"

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *self, BCA_File *file, int flags, int speed, unsigned int startFrame);
}
extern SharedFilePtr data_ov100_021489cc;
extern SharedFilePtr *data_ov100_021473a4[];
extern SharedFilePtr *data_ov100_021473b0[];

int Fish::InitResources()
{
    u8 modelIndex;
    Animation::LoadFile(data_ov100_021489cc);
    mModelIndex = (param1 >> 4) & 7;
    modelIndex = mModelIndex;
    if (modelIndex > 2) {
        if (modelIndex < 6)
            mVariant = modelIndex - 2;
        mModelIndex = 0;
    }
    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(*data_ov100_021473a4[mModelIndex]), 1, -1);
    /* Calling the true Fix12<int>-taking method homes the class value to the
       stack under this compiler (+8 bytes), so retain its exact ROM ABI here. */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim,
        (BCA_File *)Animation::LoadFile(*data_ov100_021473b0[mModelIndex]),
        0, 0x1000, 0);
    mHidden = 1;
    mUniqueID_13c = uniqueID;
    mState = 0;
    mTopY = mPosY + 0xc8000;
    unk_150 = 0;
    return 1;
}
