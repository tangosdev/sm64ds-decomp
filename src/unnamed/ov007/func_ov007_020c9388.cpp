//cpp
#include "Ov007MenuState.h"
#include "Ov007MenuMem.h"

extern "C" {

extern void func_ov007_020c934c(Ov007MenuState *state, int initialState);

void *func_ov007_020c9388(int initialState) {
    Ov007MenuState *state = (Ov007MenuState *)func_ov007_020c3df4(0, sizeof(Ov007MenuState));
    func_ov007_020c934c(state, initialState);
    return state;
}
}
