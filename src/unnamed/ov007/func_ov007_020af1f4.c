#include "Ov007MenuState.h"
#include "Ov007Work.h"

extern void func_ov007_020c1d8c(int i);

/* Sends states[2] to state 4 unless it is in state 7 with no transition
 * mode, then sets states[1]'s current state to 1 when states[0] came from
 * state 1, to 0 otherwise. */
void func_ov007_020af1f4(void) {
    Ov007Work *w = WORK;
    Ov007MenuState *s2 = w->states[2];

    if (s2->currentState != 7 || w->transitionMode != 0)
        s2->requestedState = 4;

    w = WORK;
    if (w->states[0]->previousState != 1)
        w->states[1]->currentState = 0;
    else
        w->states[1]->currentState = 1;

    func_ov007_020c1d8c(0);
}
