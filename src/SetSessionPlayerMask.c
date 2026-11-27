#include "LinkSettings.h"

/* The parent only: post the session's player mask, one bit per player.
   func_02030ca0 and func_02030b58 post the players func_0203d950 reports
   present; IsPlayerInSession reads a bit back, and func_02030b58 moves a
   joined player whose bit is clear into the dropped mask data_0209fc58. */
void SetSessionPlayerMask(u32 mask) {
    if (GetLocalPlayerId() != 0)
        return;
    int v = GetLinkByte(0, LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(mask | (v & ~LINK_SESSION_PLAYER_MASK)));
}
