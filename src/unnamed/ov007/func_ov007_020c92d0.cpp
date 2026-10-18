//cpp
#include "Ov007MenuState.h"

extern "C" {

/* Consume a pending request, detect entry (including direct state changes),
 * then advance the saturating state timer. Requesting the current state does
 * not restart it. The previous entered state survives subsequent ticks. */
void func_ov007_020c92d0(char *record) {
    Ov007MenuState *state = reinterpret_cast<Ov007MenuState *>(record);
    s16 pending = state->requestedState;

    if (pending != -1) {
        s16 current = state->currentState;

        if (pending != current) {
            state->lastTickState = current;
            state->currentState = state->requestedState;
        }
        state->requestedState = -1;
    }

    if (state->currentState != state->lastTickState) {
        state->stateTicks = 0;
        state->previousState = state->lastTickState;
        state->stateWork = 0;
    } else if (state->stateTicks < 0xffffffffU) {
        ++state->stateTicks;
    }

    state->lastTickState = state->currentState;
}
}
