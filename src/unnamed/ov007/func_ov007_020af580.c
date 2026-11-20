#include "Ov007MenuState.h"
#include "Ov007Work.h"

/* Sends states[1] to state 0xc. */
void func_ov007_020af580(void) {
    gMenuWork->states[1]->requestedState = 0xc;
}
