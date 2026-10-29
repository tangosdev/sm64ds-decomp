#include "Ov007MenuState.h"
#include "Ov007Tween.h"
#include "Ov007Work.h"

extern int func_ov007_020c8f98(void* p);
extern void func_ov007_020c1d78(int i);
extern void func_ov007_020b0244(void);
extern void func_ov007_020b7d94(int kind);
extern int func_ov007_020b79e4(void);
extern void func_ov007_020c1d8c(int i);
extern void RequestScreenFade(int a, int b, int c);
extern void func_ov007_020b8188(int a, int b, int c);
extern void func_ov007_020c0638(int a, int b, int c, int d);
extern void func_ov007_020b8070(int n);

/* The 0xc-byte record func_ov007_020adc08 returns; names as in
 * func_ov007_020adc08, only the layer read here. */
typedef struct Ov007LayerSlot {
    int state; /* 0x00: an Ov007MenuState (func_ov007_020c9388) */
    int layer; /* 0x04: the Ov007BgLayer func_ov007_020c0638 scrolls */
} Ov007LayerSlot;

extern char *data_ov007_0210342c;
/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define G data_ov007_0210342c
#define WORK ((Ov007Work *)data_ov007_0210342c)

void func_ov007_020afc44(void)
{
    Ov007MenuState *menu = WORK->states[1];
    unsigned char *r7 = (unsigned char *)(*(int *)WORK->settings) + 0xb;
    int r6 = menu->stateTicks;
    Ov007Tween *r5 = WORK->tweens[0];
    Ov007Tween *r4 = WORK->tweens[1];
    int v0 = func_ov007_020c8f98(r5);
    int v1 = func_ov007_020c8f98(r4);
    int sb = (v0 == 0);
    int fp = (v1 == 0);
    int st = menu->currentState;

    switch (st) {
    case 3:
        if (r6 == 0) {
            func_ov007_020c1d78(0);
            func_ov007_020b0244();
            WORK->states[2]->requestedState = 0xa;
        }
        switch (menu->stateWork) {
        case 0: {
            int idx = WORK->selectedSlot;
            if (idx != -1) {
                if (r7[idx] != 0) {
                    menu->requestedState = 4;
                } else {
                    WORK->selectedSlot = -1;
                    func_ov007_020b7d94(3);
                    menu->stateWork = 1;
                }
            }
            break;
        }
        case 1:
            if (func_ov007_020b79e4() == 0) {
                menu->stateWork = 0;
            }
            break;
        }
        break;
    case 4:
        if (r6 == 0) {
            func_ov007_020c1d8c(0);
            RequestScreenFade(8, 0xa, 0);
            WORK->states[2]->requestedState = 0xb;
            func_ov007_020b8188(WORK->selectedSlot, (int)WORK->settings, *(int *)(G + 0x60));
            r5->state = 1;
            r5->mode = 1;
            r5->t = 0;
            r5->step = 0;
            r5->from = 0;
            *r5->out = r5->from;
            r5->to = 0;
            r4->state = 1;
            r4->mode = 1;
            r4->t = 0;
            r4->step = 0x2000;
            r4->from = 0x30000;
            *r4->out = r4->from;
            r4->to = 0x1000;
        } else if (v1 != 0) {
            menu->requestedState = 2;
        }
        break;
    case 2:
        if (r6 == 0) {
            func_ov007_020c1d78(0);
        }
        {
            int target = WORK->targetSlot;
            if (target != -1) {
                if (target != WORK->selectedSlot) {
                    menu->requestedState = 5;
                }
            }
        }
        break;
    case 5: {
        int sl = 0x20000;
        int target = WORK->targetSlot;
        int sp4 = -0x20000;
        if (target < WORK->selectedSlot) {
            sp4 = 0x20000;
            sl = -0x20000;
        }
        if (r6 == 0) {
            func_ov007_020c1d8c(0);
            r5->state = 1;
            r5->mode = 1;
            r5->t = 0;
            r5->step = 0x3000;
            r5->from = 0;
            *r5->out = r5->from;
            r5->to = sp4;
        } else if (v0 != 0) {
            switch (menu->stateWork) {
            case 1:
                WORK->selectedSlot = target;
                WORK->targetSlot = -1;
                menu->requestedState = 2;
                break;
            case 0:
            case 2:
                if (r7[target] != 0) {
                    if (func_ov007_020b79e4() == 0) {
                        func_ov007_020c1d8c(0);
                        if (menu->stateWork == 2) {
                            RequestScreenFade(8, 0xa, 0);
                            WORK->states[2]->requestedState = 0xb;
                        }
                        func_ov007_020b8188(WORK->targetSlot, (int)WORK->settings,
                                            *(int *)(G + 0x60));
                        sb = 1;
                        r5->state = 1;
                        r5->mode = 1;
                        r5->t = 0;
                        r5->step = 0x3000;
                        r5->from = sl;
                        *r5->out = r5->from;
                        r5->to = 0;
                        r4->state = 1;
                        r4->mode = 1;
                        r4->t = 0;
                        r4->step = 0x1000;
                        r4->from = 0x1000;
                        *r4->out = r4->from;
                        r4->to = 0x1000;
                        menu->stateWork = 1;
                    }
                } else {
                    int t;
                    RequestScreenFade(8, 8, 0);
                    WORK->states[2]->requestedState = 0xa;
                    t = WORK->selectedSlot;
                    WORK->selectedSlot = WORK->targetSlot;
                    WORK->targetSlot = t;
                    func_ov007_020b7d94(3);
                    func_ov007_020c0638(WORK->layerSlots[6]->layer, 0, 0, 0);
                    menu->stateWork = 2;
                }
                break;
            }
        }
        break;
    }
    case 6:
        if (r6 == 0) {
            func_ov007_020c1d8c(0);
            if (menu->previousState == 3) {
                menu->requestedState = 0;
            } else {
                RequestScreenFade(8, 8, 0);
                WORK->states[2]->requestedState = 0xa;
                r4->state = 1;
                r4->mode = 1;
                r4->t = 0;
                r4->step = 0x2000;
                r4->from = 0x1000;
                *r4->out = r4->from;
                r4->to = 0x30000;
            }
        } else if (v1 != 0) {
            if (menu->previousState == 2) {
                menu->requestedState = 3;
            } else {
                menu->requestedState = 0;
            }
        }
        break;
    }

    if (menu->currentState == 3)
        return;
    if (func_ov007_020b79e4() != 0) return;
    if (sb != 0) {
        func_ov007_020c0638(WORK->layerSlots[6]->layer, (WORK->tweenOut[0] >> 0xc) * 8, 0, 0);
        func_ov007_020c0638(WORK->layerSlots[7]->layer, (WORK->tweenOut[0] >> 0xc) * 8, 0, 0);
    }
    if (fp == 0) return;
    func_ov007_020b8070(WORK->tweenOut[1] >> 0xc);
}
