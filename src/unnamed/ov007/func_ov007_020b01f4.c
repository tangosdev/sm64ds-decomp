#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020b0244(void);

/* On the first tick of states[1]'s state: func_ov007_020b0244, then
 * states[2] to state 8. func_ov007_020b01a4 is the same with state 9. */
void func_ov007_020b01f4(void) {
    if (gMenuWork->states[1]->stateTicks != 0)
        return;
    func_ov007_020b0244();
    gMenuWork->states[2]->requestedState = 8;
}
