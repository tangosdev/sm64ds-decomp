#include "Ov007MenuState.h"
#include "Ov007Work.h"

/* A screen-fade callback (func_ov007_020af4dc, func_ov007_020aef84): queues
 * background mode 1 and sends states[0] to state 0. */
void func_ov007_020b0520(void) {
    gMenuWork->pendingBgMode = 1;
    gMenuWork->states[0]->requestedState = 0;
}
