//cpp
// @symbol _ZN4Bird13InitResourcesEv
#include "Bird.h"
#include "SharedFilePtr.h"

extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *, BCA_File *, int, int, unsigned int);

extern SharedFilePtr data_ov009_02113c20;
extern SharedFilePtr data_ov009_02113c28;

int Bird::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov009_02113c20);
    mModelAnim.SetFile(modelFile, 1, 1);
    BCA_File *animFile = (BCA_File *)Animation::LoadFile(data_ov009_02113c28);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, animFile, 0, 0x1000, 0);
    mShadowModel.InitCylinder();
    {
        int *p60 = &mPosY;
        int y = *p60;
        int zero = 0;
        *p60 = y + 0xa000;
        mVertAccel = zero;
        mTerminalVelocity = -0x32000;
        mIsLeader = 1;
        mOwnerID = uniqueID;
        mHomePos.x = mPosX;
        mHomePos.y = mPosY;
        mHomePos.z = mPosZ;
        mState = zero;
    }
    return 1;
}
