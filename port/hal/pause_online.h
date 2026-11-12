// PAUSE2 (run coopfix1, bugs 3, 4, 5): pause in a coop room.
//
// THE RULES (Tango 11-12). In a coop room the in-game pause menu is only a
// menu: it comes up, and the world and the player's own character keep
// running underneath it, like an online game. A friend sees a small "PAUSED"
// tag over that player while (and only while) the real pause menu is open.
// Unfocusing the window or dragging it is never a pause: nothing pause-like is
// sent and the game keeps running and sending at its full rate.
//
// Solo, VS and every other room keep the cartridge's pause exactly.
//
// THE WIRE. The menu bit rides the SYNI identity's flags byte (hal/comms_hub.h
// HubIdentHead::flags), which every build since GHOSTLOOK1 reads bit by bit and
// whose unknown bits it ignores, so a 1.0.3 friend parses these identities as
// before and simply never shows the tag. This build marks its identities with
// kIdentPauseAware; a friend whose identities lack it is a 1.0.3 console, whose
// world really does stop in the pause menu, and his tag follows his paused
// ghost frames the way 1.0.3 shows it.
//
// SM64DS_PAUSE_LEGACY=1 puts back 1.0.3's behaviour on this console (the world
// stops in the menu, the away rule, the old "ll" mark, the blocking window
// drag): the "before" half of a proof row.
#ifndef PORT_HAL_PAUSE_ONLINE_H
#define PORT_HAL_PAUSE_ONLINE_H

namespace port {
namespace pauseonline {

enum : unsigned char {
    kIdentPauseMenu  = 0x08,   // the sender's real pause menu is open
    kIdentPauseAware = 0x10,   // the sender sends kIdentPauseMenu (this build on)
};

// SM64DS_PAUSE_LEGACY=1: 1.0.3's pause, unchanged.
bool legacy();
// 1 while this console's pause menu is up in a coop room and the world keeps
// running under it (the actor think gate and the camera read this).
bool soft_world();
// The identity flag bits this console adds to every SYNI it sends.
unsigned char ident_flags();
// A friend's identity arrived (seat, its flags byte).
void ident_recv(int seat, unsigned char flags);
// Draw the pause tag over seat `seat`? legacy_paused is his newest ghost
// frames' paused bit (what 1.0.3 shows the mark from).
bool mark(int seat, bool legacy_paused);

}  // namespace pauseonline
}  // namespace port

extern "C" {
// The actor think gate's view of the pause byte (src/runtime/actor/d_actor.cpp,
// src/engine/view/d_camera.cpp, host builds only): data_0209f2c4, or 0 while the
// coop pause leaves the world running.
unsigned char port_pause_think_byte(void);
// Once per Stage::Behavior, right after it returns (tests/walk_window.cpp's
// port_frame_ctrl_publish): the open / close edges, and 1 when the local
// player's input must be emptied for this frame (the menu has it).
int port_pause_online_after_stage(void);
// The proof driver (SM64DS_PAUSE2_TEST): XInput buttons to OR into this
// frame's pad, at the seam SM64DS_HOST_PAD uses.
unsigned port_pause_online_test_pad(void);
// Stage::Behavior calls in the test level this visit (SM64DS_PAUSE2_LEVEL), 0 outside it.
unsigned port_pause_online_level_frames(void);
}

#endif
