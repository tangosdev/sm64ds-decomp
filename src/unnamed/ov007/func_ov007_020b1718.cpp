//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"
#include "cstd.h"
#include "Ov007Camera.h"
#include "Ov007Model.h"
#include "Ov007Light.h"

extern "C" {

/* Vector3 declares a constructor and destructor under C++, which makes its
 * assignment copy word by word. The ROM copies these with C's ldm/stm block
 * move, which C++ keeps for a struct with an array member. */
typedef struct Vec3Words {
    Fix12i w[3];
} Vec3Words;
// @symbol func_ov007_020b1718
/* recovered: per-frame update of a two-mode screen transition driven by the
 * manager at data_ov007_0210342c. Mode 1 eases a scale toward 0x1000 over
 * 12 frames and flags completion when the slot reaches its end; mode 0 copies
 * the start positions on frame 0, then lerps the camera's target x/y and
 * distance along a smoothstep-squared curve and flags completion at frame 0x28.
 * Both modes leave a (remaining, total) pair for the shared tail, which
 * advances the counter and fades the target through func_ov007_020b2764.
 *
 * Left raw: the manager's +0xec (0 or 1; picks the sound and, in
 * func_ov007_020b1fa4, the camera preset), with no role to name.
 *
 * Codegen note: `r7val = v - 2; divisor = 0x28;` must be assigned BEFORE the
 * Fix12 store block (the compiler sinks both defs to the block exit, ahead of
 * the predicated flag write); assigning them after the block instead lands the
 * movge first and rotates the block's coloring.
 */

extern int func_ov007_020bdeb0(int a);
extern void func_ov007_020b257c(void);
extern void func_ov007_020b2454(void *a, void *b);
extern void func_ov007_020bce70(short n, int frames);
extern unsigned short func_ov007_020c3be0(int t, int a, int b);
extern void func_ov007_020b2764(int a);

void func_ov007_020b1718(void) {
    Ov007Model *F = WORK->model;
    int r7val;
    int v;
    int flag = 0;
    int divisor;
    int mode;

    v = WORK->states[1]->stateTicks;

    if (v == 0)
        WORK->states[2]->requestedState = 2;

    if (v == 0 && WORK->figureIndex == 0)
        func_ov007_020bdeb0(0x1c);
    else if (v == 0 && WORK->figureIndex == 1)
        func_ov007_020bdeb0(0x1d);

    mode = WORK->transitionMode;
    switch (mode) {
    case 1: {
        Ov007AnimPlayer *F18 = F->boneAnim;
        int q, r3, s;
        Ov007AnimPlayer *F18b;

        divisor = 0x1e;
        r7val = (int)F18->frame - ((int)F18->frameCount - 0x12);

        if (v < 0xc) {
            if (v <= 0)
                q = 0;
            else if (v >= 0xc)
                q = 0x1000;
            else
                q = cstd::div(v << 0xc, 0xc);
            r3 = q << 2;

            if (r3 <= 0x1000)
                s = FX_MUL(r3, 0x1000);
            else if (r3 <= 0x3000)
                s = FX_MUL((r3 - 0x1000), (-0x400)) + 0x1000;
            else
                s = (int)((((s64)1 - (r3 - 0x3000)) * 0x800) >> 12) + 0x800;

            WORK->modelGrowth = 0x1000 - (s >> 2);
        } else {
            WORK->modelGrowth = 0x1000;
        }

        F18b = F->boneAnim;
        if (F18b->anim != -1) {
            if ((int)F18b->frame == (int)F18b->frameCount - 2) {
                if (F18b->state == 0)
                    F18b->state = 1;
                flag = 1;
            }
        }
        break;
    }
    case 0: {
        Ov007Camera *m = WORK->cameras[0];
        int q, t, w, r3, u;

        if (v == 0) {
            Ov007VecTween *d;
            Ov007Tween *dd;
            func_ov007_020b257c();
            d = m->targetTween;
            *(Vec3Words *)&d->to = *(Vec3Words *)&m->target;
            d = m->targetTween;
            *(Vec3Words *)&d->from = *(Vec3Words *)&d->to;
            m->distanceTween->to = m->distance;
            dd = m->distanceTween;
            dd->from = dd->to;
            func_ov007_020b2454(&m->targetTween->to, &m->distanceTween->to);
        }

        if (v <= 0)
            q = 0;
        else if (v >= 0x1e)
            q = 0x1000;
        else
            q = cstd::div(v << 0xc, 0x1e);

        t = (int)(((s64)q * (0x2000 - q)) >> 12);
        u = (int)(((s64)t * (0x2000 - t)) >> 12);
        w = (int)(((s64)u * u) >> 12);
        if (w >= 0x800)
            r3 = 0x1000 - ((w - 0x800) << 1);
        else
            r3 = w << 1;

        r7val = v - 2;
        divisor = 0x28;
        {
            int uu = (int)(unsigned int)u;
            m->target.x = (int)(((s64)(0x1000 - u) * m->targetTween->from.x +
                                 (s64)uu * m->targetTween->to.x) >>
                                12);
            {
                Ov007VecTween *d2 = m->targetTween;
                r3 = u - (int)(((s64)r3 * (0x2000 - r3)) >> 12);
                m->target.y = (int)(((s64)(0x1000 - r3) * d2->from.y + (s64)r3 * d2->to.y) >> 12);
            }
            q = (int)(((s64)u * (0x2000 - u)) >> 12);
            m->distance = (int)(((s64)(0x1000 - q) * m->distanceTween->from +
                                 (s64)q * m->distanceTween->to) >>
                                12);
        }
        if (v >= 0x28)
            flag = 1;
        break;
    }
    }

    if (flag != 0) {
        WORK->pendingBgMode = 2;
        WORK->states[0]->requestedState = 1;
    }

    if (v >= 2 && r7val >= 0) {
        int w;
        int qq;
        func_ov007_020bce70(2, 0xf);
        w = WORK->paletteState->stateTicks;
        if (w <= 0)
            qq = 0;
        else if (w >= 0xf)
            qq = 0x1000;
        else
            qq = cstd::div(w << 0xc, 0xf);
        WORK->lights[1]->color = func_ov007_020c3be0(qq, WORK->lights[1]->color, 0);
    }

    {
        int qq2;
        if (v <= 0)
            qq2 = 0;
        else if (v >= divisor)
            qq2 = 0x1000;
        else
            qq2 = cstd::div(v << 0xc, divisor);
        func_ov007_020b2764(0x1000 - qq2);
    }
}
}
