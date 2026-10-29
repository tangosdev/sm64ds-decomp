//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"
#include "cstd.h"

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

/* The 0x2c-byte vector tween func_ov007_020c8f64 allocates: Ov007Tween's
 * prefix, then Vector3 from/to (func_ov007_020c8de0 blends them through
 * `out`). Only the from/to pair is spelled here. */
typedef struct Ov007VecTween {
    char pad_00[0x14];
    Vector3 from; /* 0x14 */
    Vector3 to;   /* 0x20 */
} Ov007VecTween;

/* The 0xd8-byte camera func_ov007_020c023c allocates. func_ov007_020b2160
 * passes +0x08/+0x20/+0x14 to G3i::LookAt_ as eye/up/target, and
 * func_ov007_020bffb8 places the eye `distance` from the target. Only the
 * slots this file touches are spelled. */
typedef struct Ov007Camera {
    char pad_00[0x14];
    Vector3 target; /* 0x14 */
    char pad_20[0xc];
    int distance; /* 0x2c */
    char pad_30[0xc];
    Ov007VecTween *targetTween; /* 0x3c: func_ov007_020c023c binds it to target */
    Ov007Tween *distanceTween;  /* 0x40: bound to distance */
} Ov007Camera;

/* The 0x1c-byte animation player func_ov007_020c92ac allocates; names as in
 * func_ov007_020bc02c, only the slots this file touches. */
typedef struct Ov007AnimPlayer {
    s32 state; /* 0x00: 0 playing; func_ov007_020c9110 sets 2 when a one-shot ends */
    char pad_04[8];
    s16 anim; /* 0x0c: index into the owner's animation table; -1 none */
    char pad_0e[2];
    u16 frame;      /* 0x10: the whole frame func_ov007_020c9110 last reached */
    u16 frameCount; /* 0x12 */
} Ov007AnimPlayer;

/* The 0xa0-byte model record func_ov007_020bc6d4 allocates; only the slot
 * this file reads. */
typedef struct Ov007Model {
    char pad_00[0x18];
    Ov007AnimPlayer *boneAnim; /* 0x18: func_ov007_020b257c starts it */
} Ov007Model;

/* The 0x28-byte light func_ov007_020c241c allocates; func_ov007_020c232c
 * writes +0x22 to LIGHT_COLOR. Only that slot is spelled. */
typedef struct Ov007Light {
    char pad_00[0x22];
    u16 color; /* 0x22 */
} Ov007Light;

extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020b257c(void);
extern void func_ov007_020b2454(void* a, void* b);
extern void func_ov007_020bce70(int a, int b);
extern int func_ov007_020c3be0(int a, int b, int c);
extern void func_ov007_020b2764(int a);

extern char* data_ov007_0210342c;
/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define WORK ((Ov007Work *)data_ov007_0210342c)

void func_ov007_020b1718(void)
{
    Ov007Model *F = WORK->model;
    int r7val;
    int v;
    int flag = 0;
    int divisor;
    int mode;

    v = WORK->states[1]->stateTicks;

    if (v == 0)
        WORK->states[2]->requestedState = 2;

    if (v == 0 && *(int*)(data_ov007_0210342c + 0xec) == 0)
        func_ov007_020bdeb0(0x1c);
    else if (v == 0 && *(int*)(data_ov007_0210342c + 0xec) == 1)
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
            if (v <= 0) q = 0;
            else if (v >= 0xc) q = 0x1000;
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

        if (v <= 0) q = 0;
        else if (v >= 0x1e) q = 0x1000;
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
        if (v >= 0x28) flag = 1;
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
        if (w <= 0) qq = 0;
        else if (w >= 0xf) qq = 0x1000;
        else
            qq = cstd::div(w << 0xc, 0xf);
        WORK->lights[1]->color = func_ov007_020c3be0(qq, WORK->lights[1]->color, 0);
    }

    {
        int qq2;
        if (v <= 0) qq2 = 0;
        else if (v >= divisor) qq2 = 0x1000;
        else
            qq2 = cstd::div(v << 0xc, divisor);
        func_ov007_020b2764(0x1000 - qq2);
    }
}
}
