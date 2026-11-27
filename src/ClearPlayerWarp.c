#include "LinkSettings.h"

/* If player is this console's, clear its warp destination (WarpPlayer,
   IsPlayerWarping). daPly_c::func_ov002_020c2e78 calls it when the
   destination is set but the player is not in state daPly_c::ST_WAIT;
   otherwise it copies the destination into mTeleportId. */
void ClearPlayerWarp(int player) {
    if (player != GetLocalPlayerId())
        return;
    int v = GetLinkByte(player, LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER, (u8)(v & ~LINK_PLAYER_WARP_MASK));
}
