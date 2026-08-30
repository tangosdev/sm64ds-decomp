//cpp
#include "daOts_c.h"

extern "C" void func_0201267c(u32 soundID, const Vector3 *pos);

void daOts_c::PlayHitSound()
{
    func_0201267c(0xcd, (const Vector3 *)&mCamSpacePosX);
}
