/*
 * The link settings accessors: for each field of the settings bytes that
 * LinkSettings.h maps, its getter and then its writers. The fields run in
 * byte order: the player byte's warp destination and ready bit, the
 * parent's course, the session byte's player mask, rematch ready bit and go
 * signal, the parent's course-select action, and the controller mode.
 *
 * mwcc emits C bodies in reverse source order, so the cartridge holds this
 * file backwards, PostControllerMode at 0x02020028 up to IsPlayerWarping,
 * which ends at 0x0202043c.
 */
#include "LinkSettings.h"

/* LINK_BYTE_PLAYER */

/* Player's warp destination (WarpPlayer), 0 for none. daWarpkun_c tests
   it; daPly_c copies it into mTeleportId. */
u8 IsPlayerWarping(int player) {
    return (GetLinkByte(player, LINK_BYTE_PLAYER) & LINK_PLAYER_WARP_MASK) >>
           LINK_PLAYER_WARP_SHIFT;
}

/* If player is this console's, post its warp destination (daWarpkun_c). */
void WarpPlayer(int player, int destination) {
    int v;
    if (player != GetLocalPlayerId())
        return;
    v = GetLinkByte(player, LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER,
                (u8)((v & ~LINK_PLAYER_WARP_MASK) | (destination << LINK_PLAYER_WARP_SHIFT)));
}

/* If player is this console's, clear its warp destination.
   daPly_c::func_ov002_020c2e78 calls it when the destination is set but
   the player is not in state daPly_c::ST_WAIT (otherwise it copies the
   destination into mTeleportId), and daPly_c::St_Teleport_Main calls it
   every frame once the player has arrived. */
void ClearPlayerWarp(int player) {
    int v;
    if (player != GetLocalPlayerId())
        return;
    v = GetLinkByte(player, LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER, (u8)(v & ~LINK_PLAYER_WARP_MASK));
}

u8 IsPlayerReady(int player) {
    return GetLinkByte(player, LINK_BYTE_PLAYER) & LINK_PLAYER_READY;
}

/* Set this console's ready bit, the one dScEntry_c::ArePlayersReady polls
   on every joined player through IsPlayerReady. */
void SetLocalPlayerReady(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER, (u8)(v | LINK_PLAYER_READY));
}

void ClearLocalPlayerReady(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_PLAYER);
    SetLinkByte(LINK_BYTE_PLAYER, (u8)(v & ~LINK_PLAYER_READY));
}

/* LINK_BYTE_COURSE */

u8 GetParentCourseIndex(void) {
    return GetLinkByte(0, LINK_BYTE_COURSE) & LINK_COURSE_INDEX_MASK;
}

/* The parent only: post the course index. dScEntry_c::UpdateCourseSelect
   posts the chosen course (data_0209b2e4); the other consoles copy
   GetParentCourseIndex into it. */
void SetParentCourseIndex(u32 course) {
    int v;
    if (GetLocalPlayerId() != 0)
        return;
    v = GetLinkByte(0, LINK_BYTE_COURSE);
    SetLinkByte(LINK_BYTE_COURSE, (u8)(course | (v & ~LINK_COURSE_INDEX_MASK)));
}

/* LINK_BYTE_SESSION */

u8 IsPlayerInSession(int player) {
    return (GetSessionPlayerMask() >> player) & 1;
}

u8 GetSessionPlayerMask(void) {
    return GetLinkByte(0, LINK_BYTE_SESSION) & LINK_SESSION_PLAYER_MASK;
}

/* The parent only: post the session's player mask, one bit per player.
   func_02030ca0 and func_02030b58 post the players func_0203d950 reports
   present, and func_02030b58 moves a joined player whose bit is clear into
   the dropped mask data_0209fc58. */
void SetSessionPlayerMask(u32 mask) {
    int v;
    if (GetLocalPlayerId() != 0)
        return;
    v = GetLinkByte(0, LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(mask | (v & ~LINK_SESSION_PLAYER_MASK)));
}

u8 IsPlayerReadyForRematch(int player) {
    return GetLinkByte(player, LINK_BYTE_SESSION) & LINK_SESSION_REMATCH_READY;
}

/* Set this console's rematch ready bit (dScEntry_c::EnterRematchSync,
   unless a player has dropped). The parent polls every joined player's
   copy through IsPlayerReadyForRematch (dScEntry_c::
   ArePlayersReadyForRematch). */
void SetLocalPlayerReadyForRematch(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(v | LINK_SESSION_REMATCH_READY));
}

void ClearLocalPlayerReadyForRematch(void) {
    int v = GetLinkByte(GetLocalPlayerId(), LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(v & ~LINK_SESSION_REMATCH_READY));
}

/* The parent's go signal. On seeing it the dScEntry_c sync steps clear
   this console's ready bit (ClearLocalPlayerReady) and the parent drops the
   signal (ClearGoSignal). */
u8 IsGoSignalRaised(void) {
    return GetLinkByte(0, LINK_BYTE_SESSION) & LINK_SESSION_GO;
}

/* The parent only: raise the go signal. The dScEntry_c sync steps raise it
   once ArePlayersReady (or ArePlayersReadyForRematch) sees every joined
   player's ready bit; each console then advances when IsGoSignalRaised
   sees it. */
void RaiseGoSignal(void) {
    int v;
    if (GetLocalPlayerId() != 0)
        return;
    v = GetLinkByte(0, LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(v | LINK_SESSION_GO));
}

/* The parent only: drop the go signal. Every console calls it once it has
   seen the signal. */
void ClearGoSignal(void) {
    int v;
    if (GetLocalPlayerId() != 0)
        return;
    v = GetLinkByte(0, LINK_BYTE_SESSION);
    SetLinkByte(LINK_BYTE_SESSION, (u8)(v & ~LINK_SESSION_GO));
}

/* LINK_BYTE_COURSE_ACTION */

u8 GetCourseSelectAction(void) {
    return GetLinkByte(0, LINK_BYTE_COURSE_ACTION) & LINK_COURSE_ACTION_MASK;
}

/* The parent only: post the course-select action. dScEntry_c::
   UpdateCourseSelect posts 0 for a touched course, 1 or 2 for SELECT or
   START, then raises the go signal (RaiseGoSignal). */
void SetCourseSelectAction(u32 action) {
    int v;
    if (GetLocalPlayerId() != 0)
        return;
    v = GetLinkByte(0, LINK_BYTE_COURSE_ACTION);
    SetLinkByte(LINK_BYTE_COURSE_ACTION, (u8)(action | (v & ~LINK_COURSE_ACTION_MASK)));
}

/* LINK_BYTE_CONTROLLER */

/* dScStage_c's ResetInput takes each player's mode from here in VS mode. */
u8 GetControllerMode(int player) {
    return GetLinkByte(player, LINK_BYTE_CONTROLLER) & LINK_CONTROLLER_MODE_MASK;
}

/* Post mode, so the other consoles see it through GetControllerMode.
   dScEntry_c calls it with GetLocalPlayerId right after SetControllerMode,
   the local save; the store is always this console's own byte. */
void PostControllerMode(int player, u8 mode) {
    int v = GetLinkByte(player, LINK_BYTE_CONTROLLER);
    SetLinkByte(LINK_BYTE_CONTROLLER, (u8)(mode | (v & ~LINK_CONTROLLER_MODE_MASK)));
}
