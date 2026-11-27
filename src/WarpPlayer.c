#include "LinkSettings.h"

/* If player is this console's, post its warp destination (daWarpkun_c);
   the other consoles see it through IsPlayerWarping. */
void WarpPlayer(int player, int destination) {
    int v;
    if (player != GetLocalPlayerId())
        return;
    v = GetLinkByte(player, LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER,
                (u8)((v & ~LINK_PLAYER_WARP_MASK) | (destination << LINK_PLAYER_WARP_SHIFT)));
}
