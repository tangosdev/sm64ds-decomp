//cpp
// @symbol _ZN17ExtendingPlatform6RenderEv
#include "ExtendingPlatform.h"

extern "C" s32 func_0203aad0(dBgW_KcMbgSclY *);

int ExtendingPlatform::Render()
{
    Vector3 scale;
    scale.x = 0x1000;
    scale.y = func_0203aad0(&mCollider);
    scale.z = 0x1000;
    mModel.Render(&scale);
    return 1;
}
