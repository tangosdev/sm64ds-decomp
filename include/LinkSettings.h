#ifndef LINK_SETTINGS_H
#define LINK_SETTINGS_H

#include "types.h"

/* The link settings bytes: the lobby and session state each console posts
 * in its input record (PlayerInputRecord.settings, private/
 * player_input_record.h) and the link copies to every console. A player
 * here is one console in the session, 0-3, 0 the parent; GetLocalPlayerId
 * says which one this console is. GetLinkByte reads any player's copy and
 * SetLinkByte writes this console's, so every writer below posts only this
 * console's byte; the parent's bytes (player 0) carry the session-wide
 * fields, and their writers return on every other console.
 *
 * All the names are coined: the ROM keeps none. The byte map is the
 * accessors' own, one field per getter/setter pair. */

/* PlayerInputRecord.settings byte indices */
#define LINK_BYTE_PLAYER        0x00 /* each player's own state */
#define LINK_BYTE_COURSE        0x01 /* the parent's course pick */
#define LINK_BYTE_SESSION       0x02 /* session mask and handshake bits */
#define LINK_BYTE_COURSE_ACTION 0x03 /* the parent's course-select action */
#define LINK_BYTE_CONTROLLER    0x04 /* each player's controller mode */

/* LINK_BYTE_PLAYER */
#define LINK_PLAYER_READY      0x08 /* SetLocalPlayerReady, IsPlayerReady */
#define LINK_PLAYER_WARP_MASK  0x70 /* WarpPlayer, IsPlayerWarping */
#define LINK_PLAYER_WARP_SHIFT 4

/* LINK_BYTE_COURSE: the course index, SetParentCourseIndex */
#define LINK_COURSE_INDEX_MASK 0x07

/* LINK_BYTE_SESSION */
#define LINK_SESSION_PLAYER_MASK   0x0f /* one bit per player, SetSessionPlayerMask */
#define LINK_SESSION_REMATCH_READY 0x10 /* SetLocalPlayerReadyForRematch */
#define LINK_SESSION_GO            0x20 /* RaiseGoSignal */

/* LINK_BYTE_COURSE_ACTION: SetCourseSelectAction (0 a course was touched,
 * 1 SELECT, 2 START, as dScEntry_c::UpdateCourseSelect posts them) */
#define LINK_COURSE_ACTION_MASK 0x03

/* LINK_BYTE_CONTROLLER: the mode SetControllerMode saved, PostControllerMode */
#define LINK_CONTROLLER_MODE_MASK 0x03

/* The settings bytes the link checks for desync. dScStage_c::Render stores
 * two values through SetLinkByte every frame, an XOR of the players'
 * positions and a bit per player with wings, and func_0203ea5c is meant to
 * compare each across the four records, so that a position mismatch would
 * raise link error bit 0x08 and a wing mismatch 0x10. ClearLinkErrors and
 * func_0203ea5c's error countdown clear both.
 *
 * On the cartridge both indices are 0x10: Render's second store overwrites
 * its first, the two compares test the same byte, and the two clears clear
 * it twice. One wrong constant explains all three doublings, so this is most
 * likely an EAD bug, the second index presumably meant to name another byte.
 * What the cartridge does is a wing check only: the position check never
 * runs, and since a wing mismatch already fails the first compare, error bit
 * 0x10 can never be raised (callers test it through GetLinkStatus(0x9d)).
 *
 * Which byte was meant is not recoverable. It was not 0x11: the link
 * exchanges only the first 0x11 settings bytes (0x12..0x22), which fill the
 * last 0x11 bytes of the 0x20-byte packet data_020a1020, and 0x20 bytes per
 * console is what func_0203fa50 shares, so record byte 0x23 had no room to
 * travel. Settings bytes 5..0x0f are never used and would have fit. */
#define LINK_BYTE_SYNC_POSITION 0x10
#define LINK_BYTE_SYNC_WINGS    0x10 /* presumably meant to differ (see above) */

/* Parameters are unnamed on purpose: a named parameter in a shared
 * prototype takes a number from mwccarm's @NNN counter in every C++
 * includer. The names are the definitions'. */
#ifdef __cplusplus
extern "C" {
#endif

u8 GetLinkByte(int, int);   /* (player, byte) */
void SetLinkByte(int, int); /* (byte, value) */
int GetLocalPlayerId(void);

/* LINK_BYTE_PLAYER */
u8 IsPlayerWarping(int);   /* (player) the destination, 0 for none */
void WarpPlayer(int, int); /* (player, destination) */
void ClearPlayerWarp(int); /* (player) */
u8 IsPlayerReady(int);     /* (player) */
void SetLocalPlayerReady(void);
void ClearLocalPlayerReady(void);

/* LINK_BYTE_COURSE */
u8 GetParentCourseIndex(void);
void SetParentCourseIndex(u32); /* (course) */

/* LINK_BYTE_SESSION */
u8 IsPlayerInSession(int); /* (player) */
u8 GetSessionPlayerMask(void);
void SetSessionPlayerMask(u32);  /* (mask) */
u8 IsPlayerReadyForRematch(int); /* (player) */
void SetLocalPlayerReadyForRematch(void);
void ClearLocalPlayerReadyForRematch(void);
u8 IsGoSignalRaised(void);
void RaiseGoSignal(void);
void ClearGoSignal(void);

/* LINK_BYTE_COURSE_ACTION */
u8 GetCourseSelectAction(void);
void SetCourseSelectAction(u32); /* (action) */

/* LINK_BYTE_CONTROLLER */
u8 GetControllerMode(int);        /* (player) */
void PostControllerMode(int, u8); /* (player, mode) */

#ifdef __cplusplus
}
#endif

#endif
