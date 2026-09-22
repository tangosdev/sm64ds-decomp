//cpp
// @symbol _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Gnd
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 6. A downward ground probe answered as a LINE query: the probe
 * point and a second point b4c below it are pulled into the collider's local
 * frame, and the scratch dBgCh_Lin at 0x020a0d0c is aimed between them, so
 * the base dBgW_Kc::DetectClsn(dBgCh_Lin&) does the work.
 *
 * How far down to look is the interesting part. It starts at mProbeHeight, the
 * caller's search depth; but if the caller already HAS a hit (hasClsn), the
 * drop to that existing hit bounds the search instead, whenever that is
 * shorter. So a collider can only improve on the caller's best floor, never
 * report one below it -- which is what makes several moving colliders
 * queryable in sequence without ordering them.
 *
 * The volatile on the local is load-bearing and original: it forces the
 * probe's y to be re-read from the stack for the subtraction.
 */
#include "dBgW_KcMbg.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"

extern "C" {
extern void func_020374b8(int *ground, int *position);
extern void func_02039e48(dBgW_KcMbg *self, const Vector3 *v, Vector3 *res);
extern void func_02035394(dBgCh_Lin *dst, dBgCh_Gnd *src);
extern void func_02039e30(dBgW_KcMbg *self, const Vector3 *v, Vector3 *res);

extern dBgCh_Lin data_020a0d0c;
extern Vector3   data_020a0d60;
extern dBgPi     data_020a0d1c;
}

int dBgW_KcMbg::DetectClsn(dBgCh_Gnd &ray)
{
    Vector3 localStart;
    Vector3 probePos;
    Vector3 localEnd;
    Vector3 lineEnd;
    Vector3 worldPos;

    func_020374b8((int *)&ray, (int *)&probePos);
    lineEnd = probePos;

    int probeHeight = ray.mProbeHeight;
    if (ray.hasClsn != 0) {
        int distanceToHit = *(volatile Fix12i *)&probePos.y - ray.clsnY;
        if (distanceToHit < probeHeight)
            probeHeight = distanceToHit;
    }
    lineEnd.y -= probeHeight;

    func_02039e48(this, &probePos, &localStart);
    func_02039e48(this, &lineEnd, &localEnd);
    data_020a0d0c.SetObjAndLine(localStart, localEnd, 0);
    func_02035394(&data_020a0d0c, &ray);

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        func_02039e30(this, &data_020a0d60, &worldPos);
        (dBgPi &)ray = data_020a0d1c;
        ray.clsnY = worldPos.y;
        ray.hasClsn = 1;
    }
    return hit;
}
