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
// MPSOUND2 adds entry kinds above 3, in groups that never split across frames.
// An MPSOUND1 build skips each one alone (kind > 3, counted rx_bad) and keeps
// playing the body sounds of the same frame; 0.9.3.6 drops the whole frame as
// before. Positions are world units (Fix12 >> 12), s16 little endian.
//   0x10|kind id age      a one-shot at a position, then the two below:
//   0x48 x(s16) ylo       position, part 1
//   0x49 yhi z(s16)       position, part 2
//   0x20|kind id ctl      a loop is on (sent when it starts, again every 20
//                         frames): ctl bits 0-1 the ROM entry (0 Sound::PlayLong,
//                         1 func_0201226c, 2 func_02012194), bit 2 a position
//                         follows, bits 4-7 the sender's loop slot; then:
//   0x41 p0 (24 bits)     the entry's first extra argument
//   0x42 p1(s16) p2(s8)   the other two
//   (0x48, 0x49 if bit 2)
//   0x30 slot 0 0         that loop ended
// A loop is run on the listener every frame at the drawn body (or the sent
// position) until its stop entry, or until 60 frames pass with no word for it
// (a lost stop, a friend who left or crashed), or he leaves my level or area.
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
// MPSOUND2: also the sounds his egg makes for him (the flight loop, the pop on
// a wall; at the egg's own position) and his looping body sounds (sliding,
// skidding, spinning: Sound::PlayLong and its siblings).
//
// KNOBS (off by default): SM64DS_MPSOUND=0 turns the whole feature off (no send,
// no play); SM64DS_MPSOUND_LOG=1 a [msnd] line per sound captured, sent,
// played or skipped (id, friend seat, distance, gain, volume); =2 also a line
// for every sound request and loop this console makes, with its callers;
// SM64DS_MPSOUND_DROPSTOP=1 never sends a loop's stop (the timeout proof row);
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
// MPSOUND2, set by port_coop_sound_frame; called from the host arms of Sound::PlayLong,
// func_0201226c and func_02012194 (src/engine/sound/Sound.cpp) for every loop call
extern void (*port_coop_sound_loop_hook)(int fn, unsigned handle, unsigned kind, unsigned id, const void *v,
                                         int p0, int p1, int p2);
}

#endif
