#include "LinkSettings.h"

/* Set this console's rematch ready bit (dScEntry_c::EnterRematchSync,
   unless a player has dropped). The parent polls every joined player's
   copy through IsPlayerReadyForRematch (dScEntry_c::
   ArePlayersReadyForRematch). */
void SetLocalPlayerReadyForRematch(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(v | LINK_SESSION_REMATCH_READY));
}
