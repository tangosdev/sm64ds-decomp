#include "LinkSettings.h"

/* The parent's go signal (RaiseGoSignal). On seeing it the dScEntry_c sync
   steps clear this console's ready bit (ClearLocalPlayerReady) and the
   parent drops the signal (ClearGoSignal). */
u8 IsGoSignalRaised(void) {
    return GetLinkByte(0, LINK_BYTE_SESSION) & LINK_SESSION_GO;
}
