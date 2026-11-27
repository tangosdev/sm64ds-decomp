#include "LinkSettings.h"

/* The controller mode player posted (PostControllerMode). dScStage_c's
   ResetInput takes each player's mode from here in VS mode. */
u8 GetControllerMode(int player)
{
    return GetLinkByte(player, LINK_BYTE_CONTROLLER) & LINK_CONTROLLER_MODE_MASK;
}
