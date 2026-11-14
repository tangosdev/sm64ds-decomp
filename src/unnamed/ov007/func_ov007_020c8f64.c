#include "Ov007Tween.h"
#include "Ov007MenuMem.h"

extern void func_ov007_020c90f8(void *p);
/* Allocates a vector tween bound to out, as func_ov007_020c908c does a scalar one. */
Ov007VecTween *func_ov007_020c8f64(Vector3 *out) {
    Ov007VecTween *o = MenuMem_AllocClear(0, sizeof(Ov007VecTween));
    o->out = out;
    func_ov007_020c90f8(o);
    return o;
}
