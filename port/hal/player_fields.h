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

#include <cstddef>   // size_t, for the state-table address arithmetic

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

    // ---- THE POSE FIELDS, from runs/mg16/status/POSEFIELDS.md -------------
    // Every citation below is that derivation's, kept at the field.

    // The current animation, stored as id << 2 because the field doubles as
    // the base index into the stride-4 per-character file table
    // data_ov002_020ff480[]. src/_ZN6Player7SetAnimEji5Fix12IiEj.cpp writes
    // `a << 2` here, and src/func_ov002_020e3f90.c:40 reads it back as
    // `(*(int*)(p + 0x63c) >> 2)` -- the >> 2 is what proves the scaling.
    kAnimIdScaled = 0x63c,

    // The array of ModelAnim * for the player's body models. Indexed by
    // GetBodyModelID, NOT by the character number.
    kBodyModels = 0xdc,

    // Within a ModelAnim: Animation base at +0x50, Animation::currFrame at
    // +0x08. THE VALUE IS 20.12 FIXED POINT, not a frame number.
    kModelAnimCurrFrame = 0x58,

    // The CURRENT Player::State *. There is no integer state id on the object;
    // the id is an index into the table below.
    kStateCur = 0x370,

    // mIsAirborne. 0 == ON THE GROUND -- INVERTED. Not Actor::mFlags & 0x10,
    // which is a draw-distance cull bit and was in an earlier design sketch.
    kIsAirborne = 0x6de,
};

// The Player::State table. NOT an array symbol -- 78 individually named .bss
// globals in ov002 that happen to be perfectly contiguous at stride 0x18,
// which is the only reason an integer encoding is possible at all. Verified in
// config/arm9/overlays/ov002/symbols.txt (exactly 78 lines), bounded below and
// above by neighbours that break the stride.
enum : unsigned {
    kStateRecordSize = 0x18u,
    kStateCount      = 78u,
    kStateIdUnknown  = 0xffffu,
};

// #########################################################################
// #  THE INTEGER STATE ENCODING DOES NOT WORK IN THIS PORT. MEASURED.     #
// #########################################################################
//
// POSEFIELDS.md derives a 0..77 state id as (addr - base) / 0x18, and the
// derivation is right about the ROM: config/arm9/overlays/ov002/symbols.txt has
// exactly 78 State records perfectly contiguous at stride 0x18, and on the DS
// that arithmetic is exact. Its own honest-gaps section flagged the assumption
// that the port's hosted ov002 uses those same addresses.
//
// IT DOES NOT, AND THE FAILURE IS STRUCTURAL RATHER THAN AN OFFSET BEING WRONG.
// The port hosts those 78 records as 78 SEPARATE objects and the linker places
// them wherever it likes -- they are not contiguous, not ordered, and not at
// the DS addresses. Measured on a real boot:
//
//     [vs] state decode: base=00ceec24 ptr0=00ceb90c recsize=24
//
// The live state pointer is 0x13318 bytes BELOW the table base. Every decode
// returned kStateIdUnknown, every player looked like it was in one state
// forever, and rungP6 read it as buttons having stopped working.
//
// Making the encoding work would mean hosting all 78 records as one ROM-ordered
// band, the way hal/comms_conductor.cpp hosts the comms bands -- a real change
// with a real regression surface, and not one to make while chasing an
// animation artifact.
//
// SO THE STATE IS AN OBSERVATION ID, NOT AN APPLICABLE ONE. What follows is the
// address of the state's Main function -- the same word Player::Behavior's
// dispatcher switches on -- which is a stable identity WITHIN one build and is
// therefore fine for a rung to compare and fine to put on a wire between two
// copies of the same binary. It is NOT safe to apply: hal/comms_sync.cpp does
// not call ChangeState with it, because a received address that is not a real
// State * becomes a call through a garbage pointer-to-member, which is an
// arbitrary jump and not a visual glitch. The animation is applied; the state
// is carried for diagnosis only. See the sync layer's own note.

// Declared EXACTLY as src/_ZN6Player12FinishedAnimEv.cpp:5 declares it. DO NOT
// RESPELL: competing declarations of one extern "C" symbol is its own bug class.
extern "C" unsigned int _ZNK6Player14GetBodyModelIDEjb(char *, unsigned int, char);

// The animation id, unscaled.
inline unsigned short anim_id(const void *p) {
    return (unsigned short)(*(const unsigned *)((const char *)p + kAnimIdScaled) >> 2);
}

// The ModelAnim the player's body is currently animating. May be null.
inline void *body_model_anim(void *p) {
    unsigned id = _ZNK6Player14GetBodyModelIDEjb(
        (char *)p, (unsigned)(*(int *)((char *)p + 8) & 0xff), 0);
    return *(void **)((char *)p + kBodyModels + id * 4);
}

// The animation cursor in 20.12 FIXED POINT. Integer frame is `>> 12`.
inline int anim_frame(void *p) {
    void *m = body_model_anim(p);
    return m ? *(int *)((char *)m + kModelAnimCurrFrame) : 0;
}

// Writable cursor, for a same-animation re-seed ONLY.
// READ THE WARNING: Animation::WillHitFrame tests whether [currFrame,
// currFrame + speed) crosses a given frame, which is how the ROM fires
// footsteps, hitboxes and animation-timed sounds. Moving this can SKIP such an
// event or fire it twice. Prefer seeding via SetAnim's startFrame on an id
// change, which costs nothing.
inline int *anim_frame_ptr(void *p) {
    void *m = body_model_anim(p);
    return m ? (int *)((char *)m + kModelAnimCurrFrame) : 0;
}

inline void *state_ptr(const void *p) {
    return *(void *const *)((const char *)p + kStateCur);
}

// THE STATE'S IDENTITY WITHIN THIS BUILD: the address of its Main function,
// which is the word port/unmatched/Player_Behavior.cpp:193 dispatches through.
// Stable across the two consoles because they run the same image. See the
// banner above for why this is NOT the derivation's 0..77 index.
inline unsigned state_id(const void *p) {
    const void *st = state_ptr(p);
    if (!st) return 0u;
    return *(const unsigned *)((const char *)st + 8);
}

// DELIBERATELY REFUSES. A received state identity cannot be turned back into a
// State * safely in this port: the records are not a contiguous table here, so
// there is no range check that distinguishes a real record from a number that
// merely looks like one, and ChangeState on a wrong pointer is an arbitrary
// jump. Returns null always, and the caller must treat null as "drop this
// field". Kept as a named refusal rather than deleted so the next person sees
// the decision instead of wondering where state application went.
inline void *state_from_id(unsigned) { return 0; }

// 1 when the body is standing on something. kIsAirborne is INVERTED.
inline unsigned char on_ground(const void *p) {
    return (unsigned char)(*(const unsigned char *)((const char *)p + kIsAirborne) == 0);
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
// THE NAMED GAPS ARE CLOSED, by runs/mg16/status/POSEFIELDS.md. What that
// derivation refuted is worth keeping, because both wrong answers came from
// this file's own earlier guesses and both were plausible:
//
//   Player+0x8e >> 4 IS NOT AN ANIM SELECTOR. It is a sin/cos table index
//   derived from the facing angle. The +0x8e yaw itself is still the facing
//   field and v1's use of it was correct.
//   mFlags & 0x10 IS NOT GROUNDED. It is a draw-distance cull bit at
//   Actor+0xb0. Grounded is mIsAirborne at Player+0x6de, INVERTED.
//
// WHAT IS STILL OPEN, from that derivation's own honest-gaps section and
// carried here so it is not lost:
//   * no built-object disassembly was done -- offsets are from matched source
//     and headers, which is strong but is not the same as reading the image;
//   * the ov002 state table's DS addresses are assumed to be the addresses the
//     port's hosted ov002 actually uses;
//   * the cursor-correction threshold now HAS a measurement path: the
//     mp-sync-coopdx lane built the latency rig (SM64DS_SYNC_DELAY_MS + RTT
//     readout) and the reseed threshold is SM64DS_SYNC_PHASE, tuned against
//     the rig's phase_worst readout. The reseed goes through Player::SetAnim
//     with a startFrame -- the cursor is still never written directly.

}  // namespace player
}  // namespace port

#endif  // PORT_HAL_PLAYER_FIELDS_H
