#include "LinkSettings.h"

/* Post mode, the rest of player's byte kept, so the other consoles see it
   through GetControllerMode. dScEntry_c calls it with GetLocalPlayerId
   right after SetControllerMode, the local save; the store is always this
   console's own byte (SetLinkByte). */
void PostControllerMode(int player, u8 mode) {
    int x = GetLinkByte(player, LINK_BYTE_CONTROLLER);
    x = mode | (x & ~LINK_CONTROLLER_MODE_MASK);
    SetLinkByte(LINK_BYTE_CONTROLLER, x & 0xff);
}
