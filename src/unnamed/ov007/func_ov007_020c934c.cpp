//cpp
#include "Ov007MenuState.h"

extern "C" {

/* Seed a different last-tick state so the first update performs state entry.
 * State IDs are signed halfwords; initialState is narrowed on storage. */
void func_ov007_020c934c(char *record, int initialState) {
    Ov007MenuState *state = reinterpret_cast<Ov007MenuState *>(record);
    s16 beforeInitialState = (s16)(initialState - 1);
    state->currentState = (s16)initialState;
    state->lastTickState = beforeInitialState;
    state->stateWork = 0;
    state->previousState = beforeInitialState;
    state->stateTicks = 0;
    state->requestedState = -1;
}
}
