#include "private/arm9_action_walk.h"

/* EH runtime: the current action's opcode, the low 5 bits of the byte at
   the iterator's action pointer, or 0 when there is none. The handler
   search and the rethrow switch on it. */
unsigned char ActionWalk::func_02072d90() {
    unsigned char *p = iterator.mAction;
    int r;
    if (p)
        r = p[0] & 0x1f;
    else
        r = 0;
    return (unsigned char)r;
}
