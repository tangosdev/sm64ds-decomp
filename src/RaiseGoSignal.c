#include "LinkSettings.h"

/* The parent only: raise the go signal. The dScEntry_c sync steps raise it
   once ArePlayersReady (or ArePlayersReadyForRematch) sees every joined
   player's ready bit; each console then advances when IsGoSignalRaised
   sees it. */
void RaiseGoSignal(void) {
    int x;
    if (GetLocalPlayerId() != 0)
        return;
    x = GetLinkByte(0, LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(x | LINK_SESSION_GO));
}
