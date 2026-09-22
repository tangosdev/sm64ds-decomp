//cpp
#include "dActor_c.h"
#include "dBgCh_Lin.h"

extern "C" Vector3 *func_02037dc4(SurfaceInfo *surface);

bool dActor_c::DetectRaycastClsn(Vector3 &start, Vector3 &end, bool storeHitPos)
{
    dBgCh_Lin ray;
    ray.SetObjAndLine(start, end, 0);
    if (ray.DetectClsn()) {
        if (storeHitPos) {
            Vector3 *hitPos = &ray.lineEnd;
            Vector3 *normal = func_02037dc4(&ray.surface);
            int x = hitPos->x + normal->x;
            int z = hitPos->z + normal->z;
            int y = hitPos->y + normal->y;
            end.x = x;
            end.y = y;
            end.z = z;
        }
        return true;
    }
    return false;
}
