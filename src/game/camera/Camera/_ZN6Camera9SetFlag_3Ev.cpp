//cpp
#include "types.h"
struct Camera {
    char pad[0x154];
    u32 flags;
    void SetFlag_3();
};

extern "C" void FUN_02029a68(u32 flags);

void Camera::SetFlag_3()
{
    u32 *p = (u32 *)(((int)this + 0x154));
    u32 v = *p | 8;
    *p = v;
    FUN_02029a68(v);
}
