#include "types.h"
#include "Ov007Tween.h"
#include "math/CalcLib.h"

extern void func_ov007_020c90c0(void *self);
extern void VEC_Normalize(const Vector3 *v, Vector3 *out);
extern void VEC_MultAdd(int s, int *a, int *b, int *out);

/* from at t <= 0, to at t >= 0x1000, between them the Q12 blend */
static inline int TweenLerp(int t, int from, int to) {
    if (t < 0x1000) {
        if (t <= 0)
            to = from;
        else
            to = (int)(((s64)(0x1000 - t) * from + (s64)t * to) >> 12);
    }
    return to;
}

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
        case 0:
            o->out->x = TweenLerp(o->t, o->from.x, o->to.x);
            o->out->y = TweenLerp(o->t, o->from.y, o->to.y);
            o->out->z = TweenLerp(o->t, o->from.z, o->to.z);
            func_ov007_020c90c0(o);
            break;

        case 1: {
            Vector3 *dst = o->out;
            int step = o->step;
            int ok; /* stays a flag: setting state inside the arm shrinks it (0x170) */
            VEC_Subtract(&o->to, dst, &tmp);
            if (VEC_Mag(&tmp) <= step) {
                *dst = o->to;
                ok = 1;
            } else {
                VEC_Normalize(&tmp, &tmp);
                VEC_MultAdd(step, (int *)&tmp, (int *)dst, (int *)dst);
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
