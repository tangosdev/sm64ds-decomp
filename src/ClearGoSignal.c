#include "LinkSettings.h"

/* The parent only: drop the go signal (RaiseGoSignal, IsGoSignalRaised).
   Every console calls it once it has seen the signal. */
void ClearGoSignal(void) {
    int x;
    if (GetLocalPlayerId() != 0)
        return;
    x = GetLinkByte(0, LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(x & ~LINK_SESSION_GO));
}
