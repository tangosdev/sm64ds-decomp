#include "LinkSettings.h"

/* Clear this console's rematch ready bit, the one
   SetLocalPlayerReadyForRematch sets and IsPlayerReadyForRematch tests. */
void ClearLocalPlayerReadyForRematch(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(v & ~LINK_SESSION_REMATCH_READY));
}
