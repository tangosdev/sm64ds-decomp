#include "types.h"
#include "Ov007Tween.h"

extern void func_ov007_020c90c0(void *self);
/* LenVec3 still defines its vector as its own struct Vec3; declared as
   defined, its call casts. SubVec3 takes Vector3 now. */
struct Vec3;
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(struct Vec3 *v);
extern void NormalizeVec3(int *v, int *out); /* as defined */
extern void func_02053320(int s, int *a, int *b, int *out);

/* One frame of a vector tween: mode 0 blends *out from `from` to `to` by t
 * (then steps t, func_ov007_020c90c0), mode 1 moves *out toward `to` by
 * at most step. Returns 1 once it has arrived (state 3). */
int func_ov007_020c8de0(Ov007VecTween *o) {
    Vector3 tmp;
    if (o->state == 2) {
        o->state = 3;
    }
    if (o->state == 1) {
        switch (o->mode) {
        case 0: {
            {
                int t0 = o->t;
                int a0 = o->to.x;
                int b0 = o->from.x;
                o->out->x =
                    (t0 >= 0x1000)
                        ? (a0)
                        : ((t0 <= 0)
                               ? (b0)
                               : (((((s64)(0x1000 - t0)) * b0) + (((s64)t0) * o->to.x)) >> 0xc));
            }
            {
                int t1 = o->t;
                int a1 = o->to.y;
                int b1 = o->from.y;
                o->out->y =
                    (t1 >= 0x1000)
                        ? (a1)
                        : ((t1 <= 0)
                               ? (b1)
                               : (((((s64)(0x1000 - t1)) * b1) + (((s64)t1) * o->to.y)) >> 0xc));
            }
            {
                int t2 = o->t;
                int a2 = o->to.z;
                int b2 = o->from.z;
                o->out->z =
                    (t2 >= 0x1000)
                        ? (a2)
                        : ((t2 <= 0)
                               ? (b2)
                               : (((((s64)(0x1000 - t2)) * b2) + (((s64)t2) * o->to.z)) >> 0xc));
            }
            func_ov007_020c90c0(o);
            break;
        }

        case 1: {
            Vector3 *dst = o->out;
            int step = o->step;
            int ok; /* stays a flag: setting state inside the arm shrinks it (0x170) */
            SubVec3((Vector3 *)&o->to, (Vector3 *)dst, (Vector3 *)&tmp);
            if (LenVec3((struct Vec3 *)&tmp) <= step) {
                *dst = o->to;
                ok = 1;
            } else {
                NormalizeVec3((int *)&tmp, (int *)&tmp);
                func_02053320(step, (int *)&tmp, (int *)dst, (int *)dst);
                ok = 0;
            }
            if (ok) {
                o->state = 2;
            }
            break;
        }
        }
    }

    return (o->state == 3) ? (1) : (0);
}
