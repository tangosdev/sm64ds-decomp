#include "LinkSettings.h"

/* Player's warp destination (WarpPlayer), 0 for none. daWarpkun_c tests
   it; daPly_c copies it into mTeleportId. */
u8 IsPlayerWarping(int player) {
    return (GetLinkByte(player, LINK_BYTE_PLAYER) & LINK_PLAYER_WARP_MASK) >>
           LINK_PLAYER_WARP_SHIFT;
}
