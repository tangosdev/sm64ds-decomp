//cpp
#include "daOts_c.h"

extern "C" void func_0201267c(u32 soundID, const Vector3 *pos);

void daOts_c::PlayDeathSound()
{
    func_0201267c(0xce, (const Vector3 *)&mCamSpacePosX);
}
