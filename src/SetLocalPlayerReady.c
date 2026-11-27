#include "LinkSettings.h"

/* Set this console's ready bit, the one dScEntry_c::ArePlayersReady polls
   on every joined player through IsPlayerReady. */
void SetLocalPlayerReady(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER, (u8)(v | LINK_PLAYER_READY));
}
