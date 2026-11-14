#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void RequestScreenFade(int kind, int flags, void (*onEnd)(void));
extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b1604(void);

/* On the first tick of states[1]'s state, fades out to func_ov007_020b1604:
 * fade 2 for pending choices 0xa and 0xb, fade 1 otherwise. Unless the
 * choice is 8 it also passes 0x13 to the work block's callback. */
void func_ov007_020b155c(void) {
    Ov007Work *w = WORK;
    if (w->states[1]->stateTicks != 0)
        return;
    {
        int choice = w->pendingChoice;
        if (choice == 8) {
            RequestScreenFade(1, 0, func_ov007_020b1604);
        } else {
            if (choice == 0xa || choice == 0xb) {
                RequestScreenFade(2, 0, func_ov007_020b1604);
            } else {
                RequestScreenFade(1, 0, func_ov007_020b1604);
            }
            WORK->callback(0x13);
        }
    }
    func_ov007_020c1d8c(0);
}
