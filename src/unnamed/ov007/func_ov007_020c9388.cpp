//cpp
#include "Ov007MenuState.h"

extern "C" {

/* The generic allocation bridge still has its legacy integer/pointer ABI
 * declaration. Its second word is a byte count; the returned word is a pointer. */
extern int func_ov007_020c3df4(int unused, void *byteCount);
extern void func_ov007_020c934c(char *record, int initialState);

void *func_ov007_020c9388(int initialState) {
    char *state = (char *)func_ov007_020c3df4(0, (void *)sizeof(Ov007MenuState));
    func_ov007_020c934c(state, initialState);
    return state;
}
}
