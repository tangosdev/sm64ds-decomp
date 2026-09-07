//cpp
#include "Bully.h"

extern "C" void func_0201267c(u32 soundID, const Vector3 *pos);

void Bully::PlayDeathSound()
{
    func_0201267c(0xc8, (const Vector3 *)&mCamSpacePosX);
}
