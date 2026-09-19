//cpp
#include "daDonketu_c.h"

extern "C" void func_0201267c(u32 soundID, const Vector3 *pos);

void daDonketu_c::PlayShellHitSound()
{
    func_0201267c(0xc9, (const Vector3 *)&mCamSpacePosX);
}
