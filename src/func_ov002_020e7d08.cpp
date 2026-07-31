//cpp
// @symbol func_ov002_020e7d08
/* recovered: shared common types */
#include "common.h"
typedef int Fix12;

struct Actor;

struct RaycastGround {
    char pad0[0x54];
    RaycastGround();
    ~RaycastGround();
    void SetObjAndPos(const Vector3& pos, Actor* actor);
    int DetectClsn();
};

struct Self {
    char pad[0x5c];
    int x, y, z; // 0x5c,0x60,0x64
};

extern "C" void func_ov002_020e7d08(Self* self)
{
    RaycastGround rc;
    Vector3 v;
    v.x = self->x;
    v.y = self->y;
    v.z = self->z;
    v.y += 0x32000;
    rc.SetObjAndPos(v, 0);
    *(int*)((char*)&rc + 0x4c) = 0x3e8000;
    if (rc.DetectClsn())
        *(int*)((char*)self + 0x42c) = *(int*)((char*)&rc + 0x44);
    else
        *(int*)((char*)self + 0x42c) = 0x7fffffff;
}
