// THE PORT'S PLAYER WIDTH, written down in exactly one place. Run vs16.
//
// The cartridge did four. This port hosts sixteen, and that is a MOD -- the
// owner's standing ruling is that online multiplayer is a host-layer opt-in
// and is already a mod, while the DS-faithful local path stays the default.
//
// TWO NUMBERS, AND THEY ANSWER DIFFERENT QUESTIONS.
//
//   kPortNarrowPlayers  What the DS did. It is the width of wire version 2,
//                       the field count of SM64DS_VS_NAMES and
//                       SM64DS_VS_COLORS at contract v2, and the number every
//                       shipped build in the field understands. It is not a
//                       legacy value being tolerated; it is a live wire.
//
//   kPortMaxPlayers     What this port's own storage is sized for. Every
//                       per-slot array the port HOSTS is this wide. Hosting a
//                       ROM global wider than the cartridge's own run is the
//                       deviation, and it is the approved one: the port
//                       already hosts these globals, and hosting them wider
//                       costs nothing until something actually indexes past
//                       three.
//
// WHY WIDENING THE HOSTED COPIES IS SAFE AT FOUR PLAYERS. The port's hosted
// globals are plain definitions in the .dsstate section; they are not at the
// cartridge's addresses and nothing computes an address from a neighbour's.
// The two exceptions are the GROUPED-SECTION BANDS -- the VS star array
// (.dsstate$hvsstar*) and the per-slot ready bytes (.dsstate$hready*) -- where
// the ROM's own readers walk from one symbol into the next and port/tools/
// gxband_guard.py checks the contiguity off the linker's map. Those grow by
// growing the TAIL member, which leaves every offset the guard checks exactly
// where it was.
//
// AND WHAT DOES NOT CHANGE AT FOUR. Nothing on the wire, nothing in the
// game's behaviour, and no byte of src/. A wider host array that only ever
// sees indices 0..3 is more memory and nothing else. The visible mod arms only
// when the session says more than four -- see hal/comms_loopback.cpp's THE TWO
// WIRES.

#ifndef SM64DS_PORT_VS_WIDTH_H
#define SM64DS_PORT_VS_WIDTH_H

enum { kPortNarrowPlayers = 4 };
enum { kPortMaxPlayers = 16 };

// The two must not drift apart from hal/comms_seam.h's pair, which is the same
// fact stated for the transport. They are separate declarations because the
// BSS files must not drag the whole comms seam in, so this is the check that
// keeps one from being edited without the other.
#ifdef PORT_HAL_COMMS_SEAM_H
static_assert((int)port::kCommsMaxPlayers == (int)kPortMaxPlayers,
              "the transport's width and the port's hosted width disagree");
static_assert((int)port::kCommsNarrowPlayers == (int)kPortNarrowPlayers,
              "the transport's narrow width and the port's disagree");
#endif

#endif
