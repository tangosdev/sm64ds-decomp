#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern int func_ov007_020bdeb0(int x);
extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void func_ov007_020b1fa4(int self);
extern void func_ov007_020b2764(int x);
extern void func_ov007_020b16f0(void);

/* Timed by states[1]'s ticks: at 0 it passes 0x11 to the work block's
 * callback; per figureIndex it calls func_ov007_020bdeb0 at two ticks
 * (0x2b / 0x2d for figure 0, 0x2c / 0x2e for figure 1); at 0x3c it fades
 * to func_ov007_020b16f0 and applies camera preset 1, and at 0x3d it sets
 * the screen quads' corner colours to their end values
 * (func_ov007_020b2764(0x1000)). */
void func_ov007_020b10dc(void) {
    Ov007Work *w = WORK;
    int ticks = w->states[1]->stateTicks;
    if (ticks == 0) {
        w->callback(0x11);
    }
    switch (WORK->figureIndex) {
    case 0:
        if (ticks == 0x1e)
            func_ov007_020bdeb0(0x2b);
        else if (ticks == 0x80)
            func_ov007_020bdeb0(0x2d);
        break;
    case 1:
        if (ticks == 0x1e)
            func_ov007_020bdeb0(0x2c);
        else if (ticks == 0x7a)
            func_ov007_020bdeb0(0x2e);
        break;
    }
    if (ticks == 0x3c) {
        RequestScreenFade(5, 3, func_ov007_020b16f0);
        func_ov007_020b1fa4(1);
        return;
    }
    if (ticks == 0x3d) {
        func_ov007_020b2764(0x1000);
    }
}
