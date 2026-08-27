//cpp
// @symbol _ZN14dBgW_KcMbgSclY10DetectClsnER9dBgCh_Lin
/* dBgW_KcMbgSclY::DetectClsn(dBgCh_Lin &) -- slot 7.
 *
 * A vertically-scaled mesh collider cannot be tested against a world-space
 * segment directly, so this transforms both endpoints into the collider's own
 * space, runs the unscaled dBgW_Kc test on a scratch dBgCh_Lin there, and
 * transforms the winning point back out. dBgW_Kc::DetectClsn is called
 * qualified on purpose: this IS an override of that slot, and the qualified
 * form emits the direct `bl' the ROM has instead of dispatching back into
 * ourselves.
 *
 * THE SCRATCH IS ONE OBJECT UNDER THREE NAMES. data_020a0d0c is the dBgCh_Lin,
 * and data_020a0d60 / data_020a0d1c are its own lineEnd (+0x54) and dBgPi
 * (+0x10) -- arm9 bss carries a separate symbol for each. They are declared
 * separately here rather than reached through the first: the ROM relocates
 * three distinct symbols, and folding them into member accesses would relocate
 * one symbol with addends instead. Typed, at least, so what they are is visible.
 */
#include "dBgW_KcMbgSclY.h"
#include "dBgCh_Lin.h"
#include "dBgPi.h"

extern "C" {
extern dBgCh_Lin data_020a0d0c;   /* the scratch line, in collider space */
extern Vector3   data_020a0d60;   /* == data_020a0d0c.lineEnd  (+0x54) */
extern dBgPi     data_020a0d1c;   /* == data_020a0d0c's dBgPi   (+0x10) */

void func_0203aa74(dBgW_KcMbgSclY *self, const Vector3 *v, Vector3 *res);  /* world -> collider */
void func_0203aa10(dBgW_KcMbgSclY *self, const Vector3 *v, Vector3 *res);  /* collider -> world */
void func_02035394(dBgCh_Lin *dst, dBgCh_Lin *src);
void func_020375ec(int *dst, const int *src);   /* dst[21..23] = src[0..2], i.e. dst->lineEnd */
void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(
    dBgCh_Lin *line, const Vector3 *start, const Vector3 *end, dActor_c *actor);
void _ZN5dBgPiaSERKS_(dBgPi *dst, const dBgPi *src);
}

int dBgW_KcMbgSclY::DetectClsn(dBgCh_Lin &ray)
{
    Vector3 start, end, worldPos;

    func_0203aa74(this, &ray.start, &start);
    func_0203aa74(this, &ray.lineEnd, &end);

    u8 hadClsn = ray.hasClsn;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&data_020a0d0c, &start, &end, 0);
    if (hadClsn != 0)
        data_020a0d0c.hasClsn = 1;
    func_02035394(&data_020a0d0c, &ray);

    int hit = dBgW_Kc::DetectClsn(data_020a0d0c);
    if (hit != 0) {
        Fix12i dist = data_020a0d0c.clsnDist;
        func_0203aa10(this, &data_020a0d60, &worldPos);
        func_020375ec((int *)&ray, (const int *)&worldPos);
        ray.clsnDist = dist;
        /* the dBgPi base sub-object, at +0x10 */
        _ZN5dBgPiaSERKS_((dBgPi *)((char *)&ray + 0x10), &data_020a0d1c);
        ray.hasClsn = 1;
    }
    return hit;
}
