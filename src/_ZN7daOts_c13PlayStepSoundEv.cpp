//cpp
#include "daOts_c.h"

extern "C" void func_0201267c(u32 soundID, const Vector3 *pos);

void daOts_c::PlayStepSound()
{
    if (mModelAnim.WillHitFrame(4) == 0) {
        if (mModelAnim.WillHitFrame(7) == 0)
            return;
    }
    func_0201267c(0xcc, (const Vector3 *)&mCamSpacePosX);
}
