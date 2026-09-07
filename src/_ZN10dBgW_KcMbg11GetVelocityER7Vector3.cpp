//cpp
// @symbol _ZN10dBgW_KcMbg11GetVelocityER7Vector3
#include "dBgW_KcMbg.h"

void dBgW_KcMbg::GetVelocity(Vector3 &res)
{
    res.x = velocity.x;
    res.y = velocity.y;
    res.z = velocity.z;
}
