#ifndef OV007_TWEEN_H
#define OV007_TWEEN_H

#include "types.h"

/* Reconstructed layout and name, not recovered class identity. A scalar tween
 * that drives one s32 through `out`: func_ov007_020c908c allocates the 0x1c
 * bytes and binds `out`, func_ov007_020c90f8 clears the first four fields, and
 * func_ov007_020c8f98 updates it once per frame. The 0x2c-byte vector tween
 * that func_ov007_020c8f64 allocates (updated by func_ov007_020c8de0) shares
 * this prefix through func_ov007_020c90f8 and func_ov007_020c90c0. */
typedef struct Ov007Tween {
    s32 state; /* 0x00 -- 0 idle (c90f8); callers set 1 to start; c90c0 and c8f98's
                *         mode 1 set 2 on arrival; c8f98 turns 2 into 3 and returns 1 */
    s32 mode;  /* 0x04 -- c8f98: 0 blends from->to by t, 1 steps *out toward `to` */
    s32 step;  /* 0x08 -- mode 0: c90c0 adds it to t; mode 1: c8f98's largest move */
    s16 t;     /* 0x0c -- mode 0 blend fraction, 0..0x1000; c90c0 clamps at 0x1000 */
    s32 *out;  /* 0x10 -- c908c binds it; c8f98 writes the tweened value through it */
    s32 from;  /* 0x14 -- mode 0 start value; callers also seed *out with it */
    s32 to;    /* 0x18 -- target: mode 0 end value, mode 1 destination */
} Ov007Tween;

typedef char Ov007Tween_size_must_be_0x1c[sizeof(Ov007Tween) == 0x1c ? 1 : -1];

#endif
