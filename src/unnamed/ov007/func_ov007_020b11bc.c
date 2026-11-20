#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020b2614(int anim, int startFrame, int fadeFrames);
extern void func_ov007_020b131c(void);
extern void func_ov007_020b12e8(void);

/* On states[1]'s first tick: states[2] to state 0xe, func_ov007_020b2614(2,
 * 0, 8), item 9 as the chosen item and func_ov007_020b131c; on its second
 * tick, func_ov007_020b12e8. */
void func_ov007_020b11bc(void) {
    Ov007Work *w = gMenuWork;
    int ticks = w->states[1]->stateTicks;
    if (ticks == 0) {
        w->states[2]->requestedState = 0xe;
        func_ov007_020b2614(2, 0, 8);
        gMenuWork->chosenItemId = 9;
        func_ov007_020b131c();
    }
    if (ticks == 1) {
        func_ov007_020b12e8();
    }
}
