//cpp
// @symbol _ZN10dBgW_KcMbg10DetectClsnER9dBgCh_Lin
/* recovered: named members + shared header, real C++ method
 *
 * Vtable slot 7. A moving collider is tested by moving the QUERY instead of the
 * mesh: both ends of the caller's line are pulled into the collider's local
 * frame by func_02039e48, the scratch dBgCh_Lin at 0x020a0d0c is aimed along
 * the transformed segment, and the base dBgW_Kc::DetectClsn does the real
 * work against the static mesh. A hit is then pushed back out to world space.
 *
 * clsnDist is carried across by hand rather than through the dBgPi copy:
 * it is read off the scratch line before func_020375ec overwrites the caller's
 * leading words, and restored after.
 */
#include "dBgW_KcMbg.h"
#include "dBgCh_Lin.h"

extern "C" {
extern void func_02039e48(dBgW_KcMbg *self, const Vector3 *v, Vector3 *res);
extern void func_02035394(dBgCh_Lin *dst, dBgCh_Lin *src);
extern void func_02039e30(dBgW_KcMbg *self, const Vector3 *v, Vector3 *res);
extern void func_020375ec(int *line, const int *position);

extern dBgCh_Lin data_020a0d0c;
extern Vector3   data_020a0d60;
extern dBgPi     data_020a0d1c;
}

int dBgW_KcMbg::DetectClsn(dBgCh_Lin &ray)
{
    Vector3 start;
    Vector3 end;
    Vector3 worldPos;

    func_02039e48(this, &ray.start, &start);
    func_02039e48(this, &ray.lineEnd, &end);

    u8 hadClsn = ray.hasClsn;
    data_020a0d0c.SetObjAndLine(start, end, 0);
    if (hadClsn != 0)
        data_020a0d0c.hasClsn = 1;
    func_02035394(&data_020a0d0c, &ray);

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        Fix12i distance = data_020a0d0c.clsnDist;
        func_02039e30(this, &data_020a0d60, &worldPos);
        func_020375ec((int *)&ray, (const int *)&worldPos);
        ray.clsnDist = distance;
        (dBgPi &)ray = data_020a0d1c;
        ray.hasClsn = 1;
    }
    return hit;
}
