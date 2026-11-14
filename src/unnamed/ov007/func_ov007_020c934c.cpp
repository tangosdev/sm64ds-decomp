//cpp
#include "Ov007MenuState.h"


/* Seed a different last-tick state so the first update performs state entry.
 * State IDs are signed halfwords; initialState is narrowed on storage. */
void func_ov007_020c934c(Ov007MenuState *state, int initialState) {
    s16 beforeInitialState = (s16)(initialState - 1);
    state->currentState = (s16)initialState;
    state->lastTickState = beforeInitialState;
    state->stateWork = 0;
    state->previousState = beforeInitialState;
    state->stateTicks = 0;
    state->requestedState = -1;
}
