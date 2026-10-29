#include "types.h"

/* The 0x34-byte screen transition record func_ov007_020c8b10 allocates
 * (data_ov007_02104bd8 slot 0); names as in func_ov007_020c8498. The
 * effect's arguments follow, read by the per-effect appliers. */
typedef struct Ov007Transition {
    s32 effect;          /* 0x00: 1 mosaic, 2/3 blend alpha, 4 brightness */
    u32 screens;         /* 0x04: bit 0 main engine, bit 1 sub */
    s32 state;           /* 0x08: 0 idle, 1 running, 3 held at the end */
    s32 endAction;       /* 0x0c */
    s32 curve;           /* 0x10 */
    s32 t;               /* 0x14: Q12 progress */
    s32 step;            /* 0x18 */
    s32 reversed;        /* 0x1c */
    s32 request;         /* 0x20 */
    void (*onEnd)(void); /* 0x24 */
    /* 0x28: the layers the effect applies to: func_ov007_020c86c4's mosaic
     * BG mask and SetBlendAlpha/SetBlendBrightness planes, and
     * func_ov007_020c8c88's mask */
    s32 planes;
    union {
        struct {
            s32 from; /* 0x2c: func_ov007_020c8b34 eases from this level */
            s32 to;   /* 0x30: to this one */
        } brightness;
        struct {
            u8 from; /* 0x2c: func_ov007_020c8c88's mosaic sizes, 4 bits per axis */
            u8 to;   /* 0x2d */
        } mosaic;
        struct {
            s32 planes2; /* 0x2c: SetBlendAlpha second planes (func_ov007_020c86c4) */
            u8 evaFrom;  /* 0x30: func_ov007_020c8bc8 eases EVA/EVB from/to */
            u8 evbFrom;  /* 0x31 */
            u8 evaTo;    /* 0x32 */
            u8 evbTo;    /* 0x33 */
        } alpha;
    } fx;
} Ov007Transition;

typedef char Ov007Transition_size_must_be_0x34[sizeof(Ov007Transition) == 0x34 ? 1 : -1];

/* o keeps the int * spelling of the definition; `tr` is the typed view. */
void func_ov007_020c8ac0(int* o){
    Ov007Transition *tr = (Ov007Transition *)o;
    tr->screens = 0;
    tr->effect = 0;
    tr->state = 0;
    tr->endAction = 0;
    tr->curve = 0;
    tr->t = 0;
    tr->step = 0;
    tr->reversed = 0;
    tr->onEnd = 0;
    tr->request = 0;
    tr->fx.brightness.to = 0;
    tr->fx.brightness.from = tr->fx.brightness.to;
    tr->planes = tr->fx.brightness.from;
}
