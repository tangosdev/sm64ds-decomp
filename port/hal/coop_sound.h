// FRIENDS' SOUNDS IN PARTY, COOP AND WORLD PARTIES (run after093, lane MPSOUND1).
//
// What a player hears: a friend in the same level and area as me is no longer
// silent. Every sound his own body asks the ROM for (Sound::Play with the
// vector at his Player+0x74: footsteps, jumps, landings, voices, hurt, the
// tongue, stomps, ground pounds) plays on my console at his drawn body, with
// the VS arena's distance rule (full within 90 units of my body, silent from
// 450, linear between; hal/vs_arena.h vs_puppet_sfx_*). Music, menu sounds,
// star jingles and sounds the level plays for everyone (coins, enemies) are not
// his body's and are never sent.
//
// HOW IT RIDES THE WIRE. A sound frame is a chat datagram ('SYNT', the only
// kind the live relay routes in party, coop and world rooms with a free field
// to carry it) with flags 0x80. Every build since the typing frame
// (hal/adv_chat.cpp pump_recv) drops a chat datagram whose flags are not 0 or
// 1 and counts it as rx_bad_flags, so an older console ignores it. Layout:
//
//    0  head (HubChatHead): tag 'SYNT', version 1, seat, flags 0x80,
//       text_len = payload bytes, msg_id = my sound frame sequence, reserved 0
//   12  payload: version 1 | level (u8) | area (u8) | count n (1..16)
//   16  n entries of 4 bytes: kind (u8) | id (u16 LE) | age (u8, frames
//       between the request and the send)
//
// The relay holds a seat to 8 chat datagrams a second, so this console sends
// at most 4 sound frames a second (burst 2) and packs what is waiting into
// one; chat and typing frames keep at least 2 a second.
//
// WHO HEARS. Party and coop rooms: every seat in my level and area. World hub:
// only the members of my world party (hal/world_party.h), and a world console
// sends only while it is in a party. Many friends: at most 8 remote sounds
// start in any 10 frames, at most 4 from one friend.
//
// VOLUME. The sound plays through the ROM's own Sound::Play, so the game's
// own volume setting (SM64DS_VOLUME / settings Volume) applies; a muted game
// stays silent.
//
// KNOBS (off by default): SM64DS_MPSOUND=0 turns the whole feature off (no send,
// no play); SM64DS_MPSOUND_LOG=1 a [msnd] line per sound captured, sent,
// played or skipped (id, friend seat, distance, gain, volume);
// SM64DS_MPSOUND_RIG=<frame> makes MY body ask for one jump voice and one
// effect every 45 frames from that frame (the proof rows' sound source).
#ifndef PORT_HAL_COOP_SOUND_H
#define PORT_HAL_COOP_SOUND_H

extern "C" {
// once a frame from tests/walk_window.cpp (the NETHOOK block): installs the
// two hooks below, sends what is waiting, plays what arrived
void port_coop_sound_frame(void);

// set by port_coop_sound_frame; called from the host arm at the top of
// Sound::Play (src/engine/sound/Sound.cpp) for every sound request (nullable:
// other targets do not link me)
extern void (*port_coop_sound_hook)(unsigned kind, unsigned id, const void *v);
// set by port_coop_sound_frame; called by hal/adv_chat.cpp for a chat datagram
// with flags 0x80 (nullable for the same reason)
extern void (*port_chat_sound_rx)(int seat, const unsigned char *buf, int n);
}

#endif
