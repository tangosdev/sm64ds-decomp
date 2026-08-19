//cpp
// @symbol _ZN18MovingMeshCollider17GetTriangleOriginEsR7Vector3
#include "MovingMeshCollider.h"
extern "C" void func_02039e30(MovingMeshCollider *self, Vector3 *v, Vector3 *res);

void MovingMeshCollider::GetTriangleOrigin(s16 triID, Vector3 &res)
{
    KCL_File *p = kclFile;
    u16 n = p->tris[triID].posIdx;
    s32 *v = p->positions[n];
    Vector3 tmp;
    tmp.x = v[0] << 6;
    tmp.y = v[1] << 6;
    tmp.z = v[2] << 6;
    func_02039e30(this, &tmp, &res);
}
