// THE ONE PLACE THE PORT NAMES A Player FIELD OFFSET. Run mg16 lane MP4.
//
// Ruled at the MP4 gate, and the reason is worth keeping next to the code: the
// state-sync layer reads player fields to BUILD a message and writes them to
// APPLY one, and those are two different files. Raw offsets in both means a
// future layout discovery breaks the builder and the applier independently, and
// the second break is found weeks later by someone reading a corrupted remote
// body. One accessor block, both paths through it.
//
// EVERY OFFSET HERE IS EVIDENCED, and the evidence is cited at the field. An
// offset nobody has confirmed does NOT get an accessor -- see THE NAMED GAPS at
// the bottom, which is deliberately a list rather than a set of guesses.

#ifndef PORT_HAL_PLAYER_FIELDS_H
#define PORT_HAL_PLAYER_FIELDS_H

namespace port {
namespace player {

// ---------------------------------------------------------------------------
// CONFIRMED OFFSETS. Each one is used by matched ROM code or by an existing
// port bridge that has been running against the real object.
// ---------------------------------------------------------------------------
enum : unsigned {
    // Actor::pos, a Vector3 of Fix12i. Read by Actor::ClosestPlayer,
    // CylinderClsn::Process's distance test, and Actor::
    // UpdatePosWithOnlySpeed's pushback add; hal/comms_conductor.cpp's VS probe
    // reads the same three words.
    kPosX = 0x5c,
    kPosY = 0x60,
    kPosZ = 0x64,

    // The facing angle. tests/walk_window.cpp's camera-follow arm computes
    // `(short)(*(short *)(c + 0x8e) + 0x8000)` as "behind the player", which is
    // only meaningful if +0x8e is the heading.
    kFacingYaw = 0x8e,

    // mPlayerNo, unpacked from the spawn flags by
    // src/_ZN6Player13InitResourcesEv.cpp:72 as `(a >> 6) & 3`.
    kPlayerNo = 0x6d8,

    // The character, written by the same function's :76 as `a & 7` -- THREE
    // bits, which is why hal/level_boot.cpp masks with 7 and not 3.
    kCharacter = 0x6d9,

    // mStateTimer. src/_ZN6Player12St_Wait_InitEv.cpp:38 sets it to 0x384 and
    // Player::Behavior:127 decrements it; it is the idle countdown that decides
    // when a character falls asleep.
    kStateTimer = 0x6a4,

    // mInvincibleTimer, read by src/_ZN6Player6RenderEv.cpp:50-54's blink gate.
    //
    // 0x6a0, NOT 0x6a6, AND THE WRONG ONE WAS LIVE. include/Player.h:170 puts
    // the named member at 0x6a0; 0x6a6 is mStateWaitTimer, a different field
    // entirely. This was not a dormant typo -- hal/player_bridges.cpp's body
    // draw gate reads it, so the port was culling a player's body on bit 0 of a
    // STATE TIMER, which ticks constantly. A body that blinks for no reason,
    // and the shadow path (which has its own correct gates) would have kept
    // drawing through it: a shadow with no body, from the very fix that was
    // meant to stop shadows without bodies.
    kInvincibleTimer = 0x6a0,

    // THE CURRENT STATE POINTER. Player::Behavior reaches the state machine as
    // `State *st = *(State **)&unk_370;` and dispatches on
    // `*(unsigned *)((char *)st + 8)` -- the DS address of the state's Main
    // function (port/unmatched/Player_Behavior.cpp:191-193). That address is a
    // stable, readable identity for "which state is this player in", which is
    // exactly what a button-action assertion needs: a punch or a crouch is a
    // STATE CHANGE, and position barely moves.
    kStatePtr = 0x370,
    kStateFnOff = 8,
};

// The DS address of the state this player is currently running, or 0 if it has
// none. NOT an invented enum -- it is the same word the port's own dispatcher
// switches on, so a rung asserting on it is asserting on the thing the game
// actually branches on.
inline unsigned state_id(const void *p) {
    const void *st = *(const void *const *)((const char *)p + kStatePtr);
    if (!st) return 0u;
    return *(const unsigned *)((const char *)st + kStateFnOff);
}

inline int   *pos_x(void *p)     { return (int *)((char *)p + kPosX); }
inline int   *pos_y(void *p)     { return (int *)((char *)p + kPosY); }
inline int   *pos_z(void *p)     { return (int *)((char *)p + kPosZ); }
inline short *facing(void *p)    { return (short *)((char *)p + kFacingYaw); }
inline unsigned char player_no(const void *p) {
    return *(const unsigned char *)((const char *)p + kPlayerNo);
}
inline unsigned char character(const void *p) {
    return (unsigned char)(*(const unsigned char *)((const char *)p + kCharacter) & 7);
}
inline unsigned short state_timer(const void *p) {
    return *(const unsigned short *)((const char *)p + kStateTimer);
}
inline unsigned short invincible_timer(const void *p) {
    return *(const unsigned short *)((const char *)p + kInvincibleTimer);
}

// ---------------------------------------------------------------------------
// THE NAMED GAPS. Fields MP4-DESIGN.md's SyncPlayer wants that nobody has
// confirmed an offset for yet. They are listed, not guessed, for the same
// reason hal/player_bridges.cpp declines to reproduce Player::Render's mOpacity
// gate: a guessed offset is worse than a named gap, because it produces plausible
// corruption instead of an obvious absence.
//
//   anim_id / anim_frame  hal/player_bridges.cpp derives a model index from
//                         +0x8e >> 4, but that is the FACING word doing double
//                         duty in one bridge and is not an animation cursor.
//                         The real animation state hangs off the ModelAnim at
//                         +0xdc and has not been mapped.
//   state_id              Player's state machine is pointer-to-member based
//                         (see port/tools/stategen.py); there is no small
//                         integer id to send without inventing an enum.
//   on-ground flag        mFlags is read as `mFlags & 0x10` by
//                         _ZN6Player6RenderEv.cpp:58, but which bit means
//                         grounded is unconfirmed.
//
// UNTIL THESE ARE MAPPED, v1 of the sync message carries position and facing
// only. That is a smaller message than MP4-DESIGN.md specced and the design
// note's own versioning rule covers it: adding a field later is a version bump.
// ---------------------------------------------------------------------------

}  // namespace player
}  // namespace port

#endif  // PORT_HAL_PLAYER_FIELDS_H
