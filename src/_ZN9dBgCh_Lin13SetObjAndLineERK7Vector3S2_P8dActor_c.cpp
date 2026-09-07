//cpp
#include "dBgCh_Lin.h"

extern "C" {
void func_020353b0(dBgCh *query, dActor_c *actor);
void func_02037608(dBgCh_Lin *query);
void func_0203abd4(dM3dGSph *sphere, Vector3 *centre, Fix12i radius);
}

void dBgCh_Lin::SetObjAndLine(const Vector3 &start, const Vector3 &end,
                              dActor_c *actor)
{
    Vector3 mid;
    dM3dGLin::Set(start, end);
    func_020353b0(this, actor);
    func_02037608(this);
    mid.x = start.x + end.x;
    mid.y = start.y + end.y;
    mid.z = start.z + end.z;
    mid.x >>= 1;
    mid.y >>= 1;
    mid.z >>= 1;
    func_0203abd4(&mBoundSphere, &mid, (clsnDist >> 1) + 0x1000);
}
