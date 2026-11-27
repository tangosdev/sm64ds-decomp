#include "LinkSettings.h"

/* Player's rematch ready bit, the one SetLocalPlayerReadyForRematch sets. */
u8 IsPlayerReadyForRematch(int player) {
    return GetLinkByte(player, LINK_BYTE_SESSION) & LINK_SESSION_REMATCH_READY;
}
