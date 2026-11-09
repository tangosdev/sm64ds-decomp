#ifndef OV007_TRANSITION_H
#define OV007_TRANSITION_H

#include "types.h"

/* Reconstructed layout and name, not recovered class identity. The 0x34-byte
 * screen transition Transition_Create allocates and func_ov007_020c8ac0
 * clears. Transition_Update runs it once a frame: it eases t along
 * `curve` and hands the eased value to the effect's applier, which writes
 * the 2D engines' registers for each screen in `screens`:
 *   1 mosaic          func_ov007_020c8c88
 *   2/3 blend alpha   func_ov007_020c8bc8
 *   4 brightness      func_ov007_020c8b34
 * func_ov007_020c86c4 starts it (request 1) and func_ov007_020c897c stops it
 * (request 2). Transition_StartBackward and Transition_StartForward restart it
 * backwards and forwards. */
typedef struct Ov007Transition {
    s32 effect;          /* 0x00: 1 mosaic, 2/3 blend alpha, 4 brightness */
    u32 screens;         /* 0x04: bit 0 main engine (0x0400xxxx), bit 1 sub (0x04001xxx) */
    s32 state;           /* 0x08: 0 idle (c897c), 1 running (c86c4), 3 held at the end */
    s32 endAction;       /* 0x0c: on reaching t = 0x1000: 0 stop, 1 hold,
                          * 2/3 restart forwards/backwards by `reversed` */
    s32 curve;           /* 0x10: 0 linear, 1 out-and-back, 2 squared, 3 none */
    s32 t;               /* 0x14: Q12 progress, clamped at 0x1000 */
    s32 step;            /* 0x18: added to t each frame */
    s32 reversed;        /* 0x1c: 1 runs the curve backwards (c8688 sets 1, c86a8 0) */
    s32 request;         /* 0x20: 1 start (c86c4), 2 stop (c897c); both clear it */
    void (*onEnd)(void); /* 0x24: called when t reaches the end */
    /* 0x28: the layers the effect applies to: c86c4's mosaic BG mask and
     * SetBlendAlpha/SetBlendBrightness planes, and c8c88's mask */
    s32 planes;
    union {
        struct {
            s32 from; /* 0x2c: c8b34 eases from this level */
            s32 to;   /* 0x30: to this one */
        } brightness;
        struct {
            u8 from; /* 0x2c: c8c88's mosaic sizes, 4 bits per axis */
            u8 to;   /* 0x2d */
        } mosaic;
        struct {
            s32 planes2; /* 0x2c: SetBlendAlpha's second-target planes (c86c4) */
            u8 evaFrom;  /* 0x30: c8bc8 eases EVA/EVB from/to */
            u8 evbFrom;  /* 0x31 */
            u8 evaTo;    /* 0x32 */
            u8 evbTo;    /* 0x33 */
        } alpha;
    } fx;
} Ov007Transition;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Transition_size_must_be_0x34[sizeof(Ov007Transition) == 0x34 ? 1 : -1];
#endif

#endif
