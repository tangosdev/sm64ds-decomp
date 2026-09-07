//cpp
#include "SnowmanHead.h"

extern "C" {
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
    void *self, void *file, int flags, int speed, unsigned int startFrame);
extern int data_ov072_02122bd4[];
}

int SnowmanHead::InitState0()
{
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        &mTextureSequence, (void *)data_ov072_02122bd4[1], 0, 0x1000, 0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mStateValue = 0;
    return 1;
}
