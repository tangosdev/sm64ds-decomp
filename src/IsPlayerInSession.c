#include "LinkSettings.h"

/* Player's bit of the session's player mask (GetSessionPlayerMask). */
u8 IsPlayerInSession(int player) {
    return (GetSessionPlayerMask() >> player) & 1;
}
