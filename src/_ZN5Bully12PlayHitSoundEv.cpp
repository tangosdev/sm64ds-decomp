//cpp
#include "Bully.h"

extern "C" void func_0201267c(u32 soundID, const Vector3 *pos);

void Bully::PlayHitSound()
{
    func_0201267c(0xcb, (const Vector3 *)&mCamSpacePosX);
}
