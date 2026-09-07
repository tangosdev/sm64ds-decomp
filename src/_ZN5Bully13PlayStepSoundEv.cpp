//cpp
#include "Bully.h"

extern "C" void func_0201267c(u32 soundID, const Vector3 *pos);

void Bully::PlayStepSound()
{
    if (mModelAnim.WillHitFrame(4) == 0) {
        if (mModelAnim.WillHitFrame(7) == 0)
            return;
    }
    func_0201267c(0xca, (const Vector3 *)&mCamSpacePosX);
}
