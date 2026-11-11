// GHOSTDOOR1 (run mp1, wave 56): what a hub ghost shows when its player is at a
// door, hidden by his own state, or in another room of the level.
//
// WIRE. This is the meaning build 6 gives to fields of the 44-byte SYNG frame
// (hal/comms_hub.h) that build 5 sent as zero. Build 6 is a new GAME_ID, so a
// build-5 console never receives these; nothing here is read from a frame that
// does not carry the frame-clock flag (0x20).
//
//   area byte (offset 40)
//       bits 0..3  the sender's area, plus one (data_02092120 is -1 for "none"
//                  and 0.. for a shown area): 0 = none or not known, 1..14 =
//                  area 0..13, 15 = area 14 or more (never compared)
//       bits 4..7  door kind, 0 none, kDoor*: the sender's body is in a door
//                  sequence (the no-control state's kind 7 / 0xa / 0xb / 0xe /
//                  0x10 / 0x11, or the big-door step)
//   flags (offset 6) bit 6 (0x40)  hidden: the sender's own state hides his
//                  body (cannon, pipe, a warp: opacity 0 or held in a mouth);
//                  bit 7 (0x80) reserved, sent as 0
//   vy (offset 26, s16) low byte: while a door kind is set, the door's own
//                  animation frame (0..254, 255 = no door near him); 0
//                  otherwise. GHOSTDOOR2: 0..127 a plain Door's model frame;
//                  128 + n (n = slide / 2 units, 0..69) a star gate (actor 354),
//                  which has no model animation, only sliding leaves. The follower never reads vy (Y is held, not
//                  reckoned), so the field is free.
//   vy while NO door kind is set (SWIM2): the swim tilt, (pitch_hi8 << 8) | roll_hi8,
//                  the high bytes of the body's nose angle (+0x8c) and turn lean
//                  (+0x90), 1.4 degree steps. A 1.0.2 receiver reads vy only when a
//                  door kind is set, so it ignores this; a 1.0.2 sender sends 0,
//                  which reads as level. Never use flags bit 7 for this.
//
// comms_hub.h stays WIRE1's file; its SYNG comment should point here.
#ifndef PORT_HAL_GHOST_DOOR_H
#define PORT_HAL_GHOST_DOOR_H

namespace port {
namespace ghostdoor {

enum : unsigned char {
    kAreaMask   = 0x0f,
    kDoorShift  = 4,
    kDoorNone   = 0,
    kDoorPlain  = 1,   // kind 7: the plain door touch (Player::CanEnterDoor)
    kDoorTalk   = 2,   // kinds 0xa / 0xb / 0xe: star, key and locked doors
    kDoorBig    = 3,   // kinds 0x10 / 0x11 and the big-door step 19
    kDoorKinds  = 4,   // a kind >= this is a bad field
    kFlagHidden = 0x40,
    kDoorFrameNone = 0xff,
    kGateBase   = 128,   // GHOSTDOOR2: frame bytes 128..197 are a star gate's slide (2-unit steps)
};

// ---- the sender: read the local body ---------------------------------------
// 0 or a kDoor* kind: is this body in a door sequence right now.
unsigned char sender_kind(const void *body);
// The area nibble (0..15) for the area byte.
unsigned char pack_area(int area);
// The nearest door's animation frame, 0..254, or kDoorFrameNone.
unsigned char sender_frame(const void *body);
// The sender's own state hides his body (cannon, pipe, warp, held in a mouth).
bool sender_hidden(const void *body);
// SM64DS_GHOST_DOOR_PROBE=1: one line per tick while a door kind is set or
// the door state changes, plus the level's door list once per level.
void sender_probe(const void *body, unsigned frame, unsigned char kind);

// ---- the watcher: the door a ghost is using ---------------------------------
// Plays the nearest Door actor's MODEL animation at the sender's frame and
// nothing else (never a state callback: those call OpenBigDoor / ChangeArea /
// SetRealCharacter on the body they are handed and would move the watcher).
// pos is the ghost's drawn position (Fix12). kind 0 hands the door back.
void watcher_apply(int seat, unsigned char kind, unsigned char frame,
                   const int pos[3]);
void watcher_release(int seat);
// DOORS9C: once per follow pass, after the last seat; gives back the door of any seat not driven lately.
void watcher_end_pass();
// PAINTTAG1: has this seat's ghost gone through a painting (or a floor hole,
// or any EXIT volume) on this screen. The watcher runs the ROM exit's own
// trigger (daChScene_c::Behavior, src/game/actors/d_a_ch_scene.cpp: inside
// the box in the exit's local x / y, local z changes sign since last time)
// on the ghost's drawn position against every EXIT actor (349) in this
// level, and from that frame on answers true until the ghost is seen on the
// side it came from again or the seat is released. pos is Fix12. Nothing on
// the wire: an older sender is read the same way.
// teleport: this sample is a snap (a level entry, a warp), not a step, so it
// is no crossing; it only becomes the next frame's "last".
bool paint_watch(int seat, const int pos[3], bool teleport);
// The seat was not drawn this pass (gone, parked, another level): forget it.
void paint_release(int seat);
// Counters for the [ghost:exit] line.
struct Stats {
    unsigned long long applied, released, no_door, yielded, restored, swept;
    unsigned long long paint_in, paint_out, paint_frames;   // PAINTTAG1
};
const Stats &stats();

}  // namespace ghostdoor
}  // namespace port

#endif  // PORT_HAL_GHOST_DOOR_H
