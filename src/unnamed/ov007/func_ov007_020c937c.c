#include "Ov007MenuMem.h"

/* The state object pointer is forwarded in r0 to the installed free callback.
 * Keep the long-call veneer: retail uses an absolute pooled tail branch. */
#pragma long_calls on

void func_ov007_020c937c(void *state) {
    func_ov007_020c3d1c(state);
}
