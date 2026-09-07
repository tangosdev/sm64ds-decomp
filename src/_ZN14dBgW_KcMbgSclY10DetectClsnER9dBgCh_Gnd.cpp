//cpp
/* Slot 6: turn a ground probe into a collider-space line query. The scratch
 * dBgCh_Lin owns its lifetime normally; the qualified base call prevents this
 * override from dispatching back into itself. */
#include "dBgW_KcMbgSclY.h"
#include "dBgCh_Gnd.h"
#include "dBgCh_Lin.h"

extern "C" {
void func_020374b8(int *ground, int *position);
void func_0203aa74(dBgW_KcMbgSclY *self, Vector3 *v, Vector3 *res);
void func_02035394(dBgCh_Lin *dst, dBgCh_Gnd *src);
void func_0203aa10(dBgW_KcMbgSclY *self, const Vector3 *v, Vector3 *res);
}

int dBgW_KcMbgSclY::DetectClsn(dBgCh_Gnd &ground)
{
    Vector3 localStart;
    Vector3 probePos;
    Vector3 localEnd;
    Vector3 lineEnd;

    func_020374b8((int *)&ground, (int *)&probePos);
    lineEnd = probePos;

    int probeHeight = ground.mProbeHeight;
    if (ground.hasClsn != 0) {
        int distanceToHit = probePos.y - ground.clsnY;
        if (distanceToHit < probeHeight)
            probeHeight = distanceToHit;
    }
    lineEnd.y -= probeHeight;

    func_0203aa74(this, &probePos, &localStart);
    func_0203aa74(this, &lineEnd, &localEnd);

    dBgCh_Lin ray;
    ray.SetObjAndLine(localStart, localEnd, 0);
    func_02035394(&ray, &ground);

    int hit = dBgW_Kc::DetectClsn(ray);
    if (hit != 0) {
        Vector3 clsnPos = ray.GetClsnPos();
        Vector3 worldPos;
        func_0203aa10(this, &clsnPos, &worldPos);
        (dBgPi &)ground = (dBgPi &)ray;
        ground.clsnY = worldPos.y;
        ground.hasClsn = 1;
    }
    return hit;
}
