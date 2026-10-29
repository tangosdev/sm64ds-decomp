#include "Ov007Tween.h"
extern void *func_ov007_020c3d40(int a, int sz);
extern void func_ov007_020c90f8(void *p);
void *func_ov007_020c908c(void *r5) {
    Ov007Tween *o = (Ov007Tween *)func_ov007_020c3d40(0, sizeof(Ov007Tween));
    o->out = (s32 *)r5;
    func_ov007_020c90f8(o);
    return o;
}
