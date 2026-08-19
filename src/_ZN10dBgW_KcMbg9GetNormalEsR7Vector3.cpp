//cpp
// @symbol _ZN18MovingMeshCollider9GetNormalEsR7Vector3
#include "MovingMeshCollider.h"
extern "C" void func_02039db8(MovingMeshCollider *self, Vector3 *v, Vector3 *res);

void MovingMeshCollider::GetNormal(s16 triID, Vector3 &res)
{
    KCL_File *p = kclFile;
    u16 n = p->tris[triID].normalIdx;
    s16 *v = p->normals[n];
    Vector3 tmp;
    tmp.x = v[0] << 2;
    tmp.y = v[1] << 2;
    tmp.z = v[2] << 2;
    func_02039db8(this, &tmp, &res);
}
