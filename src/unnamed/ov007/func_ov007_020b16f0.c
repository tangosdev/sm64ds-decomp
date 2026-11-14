#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020c1d78(int i);

/* The screen-fade callback func_ov007_020b10dc installs: sends states[1] to
 * state 1. */
void func_ov007_020b16f0(void) {
    WORK->states[1]->requestedState = 1;
    func_ov007_020c1d78(0);
}
