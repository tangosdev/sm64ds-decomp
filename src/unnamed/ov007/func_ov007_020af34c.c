#include "Ov007MenuState.h"
#include "Ov007Work.h"

/* Queues background mode 6, clears the pending choice and sends states[0]
 * to state 5 and states[2] to state 8. */
void func_ov007_020af34c(void) {
    gMenuWork->pendingBgMode = 6;
    gMenuWork->pendingChoice = 0;
    gMenuWork->states[0]->requestedState = 5;
    gMenuWork->states[2]->requestedState = 8;
}
