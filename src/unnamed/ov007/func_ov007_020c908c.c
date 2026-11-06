#include "Ov007Tween.h"

extern void *func_ov007_020c3d40(const char *name, int sz);
extern void func_ov007_020c90f8(Ov007Tween *tw);

/* Allocates a scalar tween that drives *out and resets it (func_ov007_020c90f8). */
Ov007Tween *func_ov007_020c908c(s32 *out) {
    Ov007Tween *o = func_ov007_020c3d40(0, sizeof(Ov007Tween));
    o->out = out;
    func_ov007_020c90f8(o);
    return o;
}
