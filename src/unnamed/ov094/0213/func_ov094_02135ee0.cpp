//cpp
// @symbol func_ov094_02135ee0
/* recovered: shared common types */
#include "common.h"

struct Player { int GetTalkState(); };
extern "C" void func_ov094_02136188(void *c, void *p);
extern "C" short Vec3_HorzAngle(const void *a, const void *b);
extern "C" short Vec3_VertAngle(const void *a, const void *b);
extern "C" void ApproachAngle(void *dst, int target, int c, int d, int e);
extern "C" char data_ov094_02136b60;
extern "C" int func_ov094_02135ee0(void *self)
{
    char *s = (char*)self;
    if (*(Player**)(s + 0x3d0) == 0) {
        func_ov094_02136188(self, &data_ov094_02136b60);
        return 1;
    }
    Vector3 v = *(Vector3*)((char*)(*(Player**)(s + 0x3d0)) + 0x5c);
    Vector3 w;
    w.x = v.x;
    w.y = v.y;
    w.z = v.z;
    ApproachAngle(s + 0x94, Vec3_HorzAngle(s + 0x5c, &w), 0xa, 0x200, 0x100);
    Vector3 u;
    v.y += 0xc8000;
    u.x = v.x - 0;
    u.y = v.y;
    u.z = v.z;
    ApproachAngle(s + 0x92, Vec3_VertAngle(s + 0x5c, &u), 0xa, 0x200, 0x100);
    if ((*(Player**)(s + 0x3d0))->GetTalkState() < 0) {
        *(char*)(s + 0x3d4) = 2;
        func_ov094_02136188(self, &data_ov094_02136b60);
    }
    return 1;
}
