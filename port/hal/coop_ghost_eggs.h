// A COOP FRIEND'S YOSHI EGGS ON MY CONSOLE. Run after093, lane FRIENDVIS1.
//
// On the cartridge a Yoshi's eggs are local spawns (class 9, daYegg_c) that his own body lays
// (func_ov002_020d6368) and keeps in its five-egg queue (+0x588). A friend's console only gets his body
// (SYNG), so it drew him with no eggs and never showed one he threw. This module sends my egg count and
// my thrown eggs' path on one SYNW sub-type, and on the friend's console it lays the same number of eggs
// on my ghost body with the ROM's own lay call, lets their ROM Behavior trail the body where it is drawn,
// and flies a thrown one along my path (cosmetic only: the thrower's console owns the hit).
//
// Coop rooms: the SYNW kind below rides the coop world relay, driven from coop_actors.cpp's frame hook. Party rooms:
// party_hits.cpp's SYNH carries the egg count (kHeldState) and the thrown eggs (kEggFlight), driven from its frame
// hook (ghost_eggs_party_*). The per-egg half is the _MSC_VER arm of daYegg_c::Behavior (src/game/actors/d_a_yegg.cpp).
//
// WIRE (SYNW sub-type kWorldGhostEggs = 0x60; an old build drops it as an unknown sub-type):
//   GhostEggHead 8 B: key (level, act), ver 1, eggs 0..5 (my queue), n 0..2 (records), seq
//   then n x GhostEggRec 10 B: id (low byte of the egg's actor id), flags (1 in hand, 2 flying,
//   4 burst), x, y, z (world units, s16), yaw
//   Sent on a change, every 2 frames while an egg is out of the queue, a burst three times, and a
//   heartbeat every 30 frames while I have eggs (every 90 with none).
//
// Knobs: SM64DS_GHOST_EGGS=0 is the CONTROL (nothing sent, nothing drawn); SM64DS_GHOST_EGGS_LOG=1 logs.
#ifndef PORT_HAL_COOP_GHOST_EGGS_H
#define PORT_HAL_COOP_GHOST_EGGS_H

#include "coop_wire.h"

#include <cstdint>

namespace port {

enum : uint8_t { kWorldGhostEggs = 0x60 };

#pragma pack(push, 1)
struct GhostEggHead {
    CoopKeyWire key;
    uint8_t ver;
    uint8_t eggs;
    uint8_t n;
    uint8_t seq;
};
struct GhostEggRec {
    uint8_t id;
    uint8_t flags;
    int16_t x, y, z;
    int16_t yaw;
};
#pragma pack(pop)
static_assert(sizeof(GhostEggHead) == 8, "ghost egg head is 8 bytes");
static_assert(sizeof(GhostEggRec) == 10, "ghost egg record is 10 bytes");

bool ghost_eggs_validate(const void *payload, int len);

// Once a frame from coop_actors.cpp's on_frame (coop room, group established).
void ghost_eggs_frame(unsigned now_ms);
// A friend left my group: his eggs here go.
void ghost_eggs_member_gone(int seat);

// PARTY ROOMS (the coop world relay does not run there; party_hits.cpp carries the data on SYNH):
// once a frame from party_hits' frame hook, with my level; my egg count already rides its kHeldState (eggs << 4), and
// a thrown egg goes out through send as two kEggFlight messages: its start (position, speed per frame) and its burst.
void ghost_eggs_party_frame(char *me, int level, void (*send)(const GhostEggRec &rec, int vx, int vy, int vz));
// a friend's egg count (party_hits' kHeldState) and one record of his thrown egg (kEggFlight, speed 0 on a burst)
void ghost_eggs_party_in(int seat, int eggs);
void ghost_eggs_party_flight(int seat, const GhostEggRec &rec, int vx, int vy, int vz);

}  // namespace port

// The daYegg_c::Behavior arm (src/game/actors/d_a_yegg.cpp). begin returns 0 (not a friend's egg: the
// ROM as is), 1 (trails a ghost: the ghost was put at its drawn spot, end puts it back), 2 (a thrown
// friend egg: its position was set, it only animates and draws), 3 (that egg's flight is over: burst).
extern "C" int port_ghost_egg_begin(void *egg, int saved[4]);
extern "C" void port_ghost_egg_end(void *egg, const int saved[4]);

#endif  // PORT_HAL_COOP_GHOST_EGGS_H
