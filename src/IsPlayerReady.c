#include "LinkSettings.h"

/* Player's ready bit, the one SetLocalPlayerReady sets. */
u8 IsPlayerReady(int player) {
    return GetLinkByte(player, LINK_BYTE_PLAYER) & LINK_PLAYER_READY;
}
