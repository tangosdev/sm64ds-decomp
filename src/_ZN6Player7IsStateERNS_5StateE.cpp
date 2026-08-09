//cpp
// @symbol _ZN6Player7IsStateERNS_5StateE
/* recovered: real C++ method, named members + shared header */
#include "Player.h"

/* Five instructions, and every one of them is in the source now:
     ldr r0, [r0, #0x370]   -> mState
     cmp r0, r1             -> == &state
     moveq r0, #1 / movne r0, #0
     bx lr
   The moveq/movne pair is the compiler normalising to a truth value, which is
   what makes bool the honest return type rather than int -- contrast the
   WithMeshClsn accessors, which return a raw mask straight out of an `and`.

   The parameter is a reference, so the caller passes an address and this
   compares pointer identity: two distinct State objects with identical
   contents are NOT the same state. That is the whole semantic. */
bool Player::IsState(State &state)
{
    return mState == &state;
}
