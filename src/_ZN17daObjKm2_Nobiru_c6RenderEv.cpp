//cpp
// @symbol _ZN17daObjKm2_Nobiru_c6RenderEv
#include "daObjKm2_Nobiru_c.h"

extern "C" s32 func_0203aad0(dBgW_KcMbgSclY *);

int daObjKm2_Nobiru_c::Render()
{
    Vector3 scale;
    scale.x = 0x1000;
    scale.y = func_0203aad0(&mCollider);
    scale.z = 0x1000;
    mModel.Render(&scale);
    return 1;
}
