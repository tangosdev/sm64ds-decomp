#include "types.h"
extern void func_ov007_020c86c4(void* p);
extern void func_ov007_020c897c(void* p);
extern void func_ov007_020c8970(void* p);
extern void func_ov007_020c8688(void* p, int a);
extern void func_ov007_020c86a8(void* p, int a);
extern void func_ov007_020c8c88(void* p, int a);
extern void func_ov007_020c8bc8(void* p, int a);
extern void func_ov007_020c8b34(void* p, int a);

typedef void (*VFN)(void);

/* Reconstructed layout and name, not recovered class identity: the screen
 * transition record in data_ov007_02104bd8 slot 0 (func_ov007_020bf690 drives
 * it, func_ov007_020bf790 configures it). Only the slots this file reads; the
 * effect parameters from +0x28 on are read by the per-effect appliers. */
typedef struct Ov007Transition {
    s32 effect;    /* 0x00: picks the applier below: 1 func_ov007_020c8c88 (mosaic),
                    * 2/3 func_ov007_020c8bc8 (blend alpha), 4 func_ov007_020c8b34
                    * (brightness); func_ov007_020c86c4/020c897c switch on it too */
    u32 screens;   /* 0x04: bit 0 main engine (0x0400xxxx), bit 1 sub (0x04001xxx) */
    s32 state;     /* 0x08: 0 idle (func_ov007_020c897c), 1 running
                    * (func_ov007_020c86c4), 3 held at the end (endAction 1) */
    s32 endAction; /* 0x0c: on reaching t = 0x1000: 0 stop, 1 hold,
                    * 2/3 restart forwards/backwards by `reversed` */
    s32 curve;     /* 0x10: 0 linear, 1 out-and-back, 2 squared, 3 none */
    s32 t;         /* 0x14: Q12 progress, clamped at 0x1000 */
    s32 step;      /* 0x18: added to t each frame (func_ov007_020bf790 seeds it) */
    s32 reversed;  /* 0x1c: 1 runs the curve backwards (func_ov007_020c8688 sets 1,
                    * func_ov007_020c86a8 sets 0) */
    s32 request;   /* 0x20: 1 start (func_ov007_020c86c4), 2 stop (func_ov007_020c897c);
                    * both clear it */
    VFN onEnd;     /* 0x24: called when t reaches the end; func_ov007_020bf790 seeds it */
} Ov007Transition;

/* r5 keeps the char * spelling of the extern; `tr` is the typed view. */
void func_ov007_020c8498(char* r5)
{
    Ov007Transition *tr = (Ov007Transition *)r5;
    int r4 = 0;
    VFN r6;

    if (tr->request == 1) {
        func_ov007_020c86c4(r5);
    } else if (tr->request == 2) {
        func_ov007_020c897c(r5);
    }

    if (tr->state == 0)
        return;

    if (tr->state == 1) {
        int *v14 = &tr->t;
        *v14 += tr->step;
        if (tr->t > 0x1000) {
            tr->t = 0x1000;
            switch (tr->endAction) {
            case 0:
                func_ov007_020c8970(r5);
                break;
            case 1:
                tr->state = 3;
                break;
            case 2:
                if (tr->reversed == 1)
                    func_ov007_020c8688(r5, tr->screens);
                else
                    func_ov007_020c86a8(r5, tr->screens);
                break;
            case 3:
                if (tr->reversed == 1)
                    func_ov007_020c86a8(r5, tr->screens);
                else
                    func_ov007_020c8688(r5, tr->screens);
                break;
            }

            r6 = tr->onEnd;
            if(r6 != 0){
                r6();
            }
            if (r6 == tr->onEnd && (u32)tr->endAction <= 1)
                tr->onEnd = 0;
        }
    }

    if (tr->state == 0)
        return;

    switch (tr->curve) {
    case 0:
        r4 = tr->t;
        break;
    case 1:
        r4 = tr->t << 1;
        if(r4 > 0x1000) r4 = -(r4 - 0x2000);
        break;
    case 2:
        r4 = (int)(((long long)tr->t * tr->t) >> 12);
        break;
    case 3:
        break;
    }

    if (tr->reversed == 1)
        r4 = 0x1000 - r4;

    switch (tr->effect) {
    case 1:
        func_ov007_020c8c88(r5, r4);
        break;
    case 2:
    case 3:
        func_ov007_020c8bc8(r5, r4);
        break;
    case 4:
        func_ov007_020c8b34(r5, r4);
        break;
    default:
        break;
    }
}
