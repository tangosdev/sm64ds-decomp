#include "Ov007MenuState.h"
#include "Ov007Work.h"

/* Sends states[0] to state 0. */
void func_ov007_020b0324(void) {
    gMenuWork->states[0]->requestedState = 0;
}
