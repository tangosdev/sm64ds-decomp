#include "LinkSettings.h"

/* Clear this console's ready bit, the one SetLocalPlayerReady sets and
   IsPlayerReady tests. */
void ClearLocalPlayerReady(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER, (u8)(v & ~LINK_PLAYER_READY));
}
