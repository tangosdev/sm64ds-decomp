#include "types.h"
extern int ACTOR_DEBUG_NAMES[];
extern char data_020914a0[8];
int GetActorDebugName(unsigned int i) {
    return (i < 0x188) ? ACTOR_DEBUG_NAMES[i] : (intptr_t)(&data_020914a0);
}
