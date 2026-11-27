#include "LinkSettings.h"

/* The session's player mask the parent posted (SetSessionPlayerMask). */
u8 GetSessionPlayerMask(void) {
    return GetLinkByte(0, LINK_BYTE_SESSION) & LINK_SESSION_PLAYER_MASK;
}
