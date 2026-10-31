// The message box's HOST SIDE, around the ROM's own Stage::UpdateMessage.
//
// WHAT RUNS NOW (run linkfull, lane RS5B). Stage::UpdateMessage is the ROM's
// own body: src/_ZN5Stage13UpdateMessageEv.cpp, main's blob (c45176b308 via
// #3107), which declares the four callees it used to stub -- Message::
// UpdateWindow, Message::Update, Message::DisplaySaving, SaveData::
// SaveCurrentFile -- and calls the real ones. Stage::Behavior (_ZTV5Stage slot
// 6) calls it once a frame when data_0209f2d8 != 1, exactly as on the DS:
//
//     void Stage::UpdateMessage() {
//         if (data_0209d660 == 0) return;              // no message active
//         if (Message::UpdateWindow()) {               // box fully open?
//             if (data_0209d654 == 0) { Message::Update(); return; }  // dialogue
//             ... the save-screen countdown ...        // data_0209d654 != 0
//         }
//         ... the save-screen close ...
//     }
//
// This file used to BE that body, statement for statement (port_message_pump),
// because the matched file defined empty stubs of its four callees. Main fixed
// the file; the host transcription of the save arm, the SaveCurrentFile leaf
// and the pump's own per-frame duties retire with it. What is left here is
// what the ROM body needs from the host, and nothing else:
//
// 1. THE FORWARD. hal/stage_frame.cpp's face `Stage::UpdateMessage()
//    { port_message_pump(); }` still defines ?UpdateMessage@Stage@@SAXXZ, the
//    name Stage::Behavior spells (that file belongs to another lane this wave).
//    So the ROM body is compiled under a per-source rename, UpdateMessage ->
//    UpdateMessage_rom (port/CMakeLists.txt, the RS5B block), and
//    port_message_pump is one call into it. The day stage_frame.cpp's face is
//    deleted, the define goes, the ROM body takes its own name back, and this
//    function goes with them.
//
// 2. ?Update@Message@@SAXXZ. The ROM TU declares Message::Update STATIC (it
//    reads only globals, and Stage::UpdateMessage calls it with nothing in r0);
//    the matched body, src/_ZN7Message6UpdateEv.cpp, is a member
//    (?Update@Message@@QAEXXZ), faced for C callers in hal/reverse_bridges.cpp
//    as _ZN7Message6UpdateEv(void *). The static spelling is defined below and
//    forwards to that body with the same null receiver the pump always passed.
//
// 3. THE ENGINE-A DISPLAY SYNC, the one host duty the box needs, riding that
//    face. On the DS func_02019144 copies the software BG-offset shadows and
//    the BG-enable layer mask into the engine-A registers once a frame.
//    SetBg3Offset (src/SetBg3Offset.c) writes shadows, not hardware, and
//    func_0201f32c scrolls BG3 through them so the box content (at the TOP of
//    the BG3 tilemap) lines up under the WIN0 window lower down the screen.
//    Without the flush the hardware BG3 offset stays 0 and the compositor reads
//    only the empty fill tile inside the window (the 2026-08-08 "box active but
//    0 px composited" finding). hal/sub_screen.cpp reproduces func_02019144's
//    other beats; this is the engine-A remainder, byte for byte as func_02019144
//    lines 46 and 55 write it, at the point the pump always ran it: right after
//    Message::Update in the dialogue arm, so it touches nothing when no box is
//    open.
//
// 4. TWO DATA SPELLINGS. The ROM TU declares its six globals at file scope in
//    C++, so it asks for their decorated names. Four were already bridged
//    (hal/cxx_aliases.cpp, romdata.c); data_0209d654 and data_0209d67c are
//    bridged below, onto the same hal/auto_bss.cpp storage every other reader
//    uses.
//
// WHAT RETIRED, and why each is safe:
//  - The pad publish (A/B from the Ctrl block OR-ed into PadData[i].pressed and
//    data_020a0e5a while a box was up). It existed because nothing wrote
//    data_020a0e5a, so IsButtonInputValid and Message::Update never saw a press
//    and the box could not be dismissed. The game loop's own input steps write
//    it now (run linkfull, lane B5INPUT: phase 0x17, func_0203bc7c builds
//    PadData and data_020a0e5a is mirrored from PadData[i].pressed right after
//    it, tests/walk_window.cpp), and the two Ctrl bits the publish carried
//    (0x1 punch, 0x2 jump) are raw A and raw B by construction
//    (host_btn_to_raw_keys), so the OR added nothing: measured, the word at
//    every Message::UpdateWindow call is identical with and without it.
//  - SM64DS_SAVE_POINT (lane SAVE1's instrument that fired DisplaySaving(0x295)
//    on a chosen pump frame) and SM64DS_MSG_PUMP_DEBUG (the per-frame box
//    trace). Nothing under port/tools reads either; both were properties of
//    the host pump, not of the game.

#include <cstdio>
#include "sdat/sdat.h"   /* sd_seq_player_mask / sd_seq_active: is the tune still sounding */

extern "C" {

void _ZN7Message6UpdateEv(void *self);     /* Message::Update, faced in reverse_bridges */

extern unsigned char data_0209d45c;   /* engine-A BG-enable layer mask */
extern unsigned char data_0209d6bc;   /* the box's state (3 = the lines scrolling) */
extern short data_0209d48c, data_0209d490;   /* BG3 hofs/vofs shadows */

}

/* The two decorated data names the ROM TU asks for that nothing bridged yet.
   One storage, both names: the C definitions are hal/auto_bss.cpp's. */
#pragma comment(linker, "/alternatename:?data_0209d654@@3EA=_data_0209d654")
#pragma comment(linker, "/alternatename:?data_0209d67c@@3EA=_data_0209d67c")

static void port_message_flush_engine_a_regs(void)
{
    typedef volatile unsigned int vu32;
    /* engine-A DISPCNT layer-mask publish (func_02019144 line 46): puts BG3 on
       (data_0209d45c bit 3, which func_0201f32c set), so the compositor's
       BG-enable read sees the box layer. */
    *(vu32 *)0x4000000 = (*(vu32 *)0x4000000 & ~0x1f00u) | (data_0209d45c << 8);
    /* BG3 offset (func_02019144 line 55): the box's own scroll, so the content
       at the top of the BG3 tilemap lines up under the WIN0 window. This is the
       one offset the box actually uses; BG0/1/2's offset shadows are not seated
       on the port and the box does not scroll them, so only BG3 is flushed. */
    *(vu32 *)0x400001c = (data_0209d48c & 0x1ff) | (0x1ff0000u & (data_0209d490 << 16));
}

/* The ROM TU's spellings. Message is declared here the way the ROM TU declares
   it, not from include/Message.h, because the matched Update is a member there
   and the static face is exactly what this file must not confuse with it. */
struct Message {
    static void Update();
};
struct dScStage_c {
    static void UpdateMessage_rom();   /* src/_ZN5Stage13UpdateMessageEv.cpp, renamed */
};

/* ?Update@Message@@SAXXZ: the ROM body's dialogue-arm call. The matched
   Message::Update, then the engine-A sync the box needs on the port. */
/* TEXT2D1: 1 when this tick's Message::Update ran the box's line scroll (state 3: func_0201adfc moves the text tile rows up, 4 DS pixels
   a tick). The compositor takes it for this tick's picture (port_text_scroll_take); a picture between two ticks then slides the text
   back down by what is left of the step. */
static int g_text_scroll_tick;
extern "C" int port_text_scroll_take(void) { int v = g_text_scroll_tick; g_text_scroll_tick = 0; return v; }
void Message::Update()
{
    const unsigned char st = data_0209d6bc;
    _ZN7Message6UpdateEv(0);
    g_text_scroll_tick = (st == 3) ? 1 : 0;
    port_message_flush_engine_a_regs();
}

/* PEACHJINGLE1: the cartridge's NCS_BGM_PEACH (SDAT sequence 70, 7 s) when a star door's UNLOCK message opens.
   The DS plays nothing there. The unlock state (Player func_ov002_020c8b78) ends in ShowMessage(0x188) for a star door (daDoor_c, TryTalkToDoor(0)) or
   ShowMessage(0x29) for a big star gate (daStarGate_c, TryTalkToDoor(1)); the refusals ("It takes the power of N Stars", 0x19, 0x1a..0x1f) are other ids
   and play nothing, and the key doors have no unlock text. The box opening (func_0201f32c) sets data_0209d660 and the id data_0209d6d4, so this watches
   that edge once a frame.
   HOW IT PLAYS. The cartridge files sequence 70 under the MAIN music player (like the two Lakitu tunes), so it takes the level music's place rather than
   mixing over it. The game's own way to do that and give the level music back is the temporary-music pair the cap power-ups use (Player.cpp, the
   invincibility / metal / wing tunes): Sound::SetMusic(player, id) swaps the id in, Sound::EndMusic(player, ..) puts the level's own id back and restarts
   it. Both ignore any player but the local one (data_0209f250), so nothing here reaches another player. It is not started while a boss layer (e444) or
   another temporary tune is up, and it is only ended if sequence 70 is still the current music (a level change or the game's own end of a tune that
   replaced it leaves nothing for us to undo). The unlock state ducks the music to 20 of 127 every frame it runs, which would make the tune a whisper,
   so the volume target is held at the level it had before while the tune plays and handed back when it ends. Audio only. Setting PeachJingle (default
   on); off never calls into the sound code, so the audio is the DS's. */
extern "C" {
extern unsigned char data_0209d660;      /* the box is open */
extern short data_0209d6d4;              /* the open box's message id */
extern int data_0209b4ac;                /* the current music id (layer 1 / temporary tune) */
extern int data_0208e43c;                /* the level's own music id */
extern int data_0208e444;                /* the boss layer's id, -1 = none */
extern signed char data_0208e42c;        /* the music volume it is being faded to */
extern unsigned char data_0209f250;      /* the local player's number */
void _ZN5Sound8SetMusicEjj(unsigned int player, unsigned int id);
void _ZN5Sound8EndMusicEjj(unsigned int player, unsigned int unused);
int _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int vol, int fade);
int host_setting_peach_jingle(void);
}
static void port_peach_jingle_tick(void)
{
    enum { PEACH_SEQ = 70, PEACH_CAP = 330 /* fallback when the sequence player cannot be found: the tune is audible about 5.5 s at 60 per second */, PEACH_MAX = 720 /* a stuck player never holds the music longer than this */, PEACH_FADE = 0x15666 };
    static int s_prev_open, s_prev_id = -1, s_age = -1, s_vol = 0x7f, s_player = -1;
    static unsigned s_mask0;
    const int open = data_0209d660 ? 1 : 0;
    const int id = open ? (int)data_0209d6d4 : -1;
    const bool edge = open && !(s_prev_open && s_prev_id == id);
    s_prev_open = open;
    s_prev_id = id;
    if (s_age >= 0 && data_0209b4ac != PEACH_SEQ) {   /* the level changed, or something else took the music: nothing of ours is left to undo */
        fprintf(stderr, "[peach] music changed under the tune (now %d) after %d frames; dropped\n", data_0209b4ac, s_age);
        s_age = -1;
    }
    if (s_age < 0) {
        if (edge && (id == 0x188 || id == 0x29) && host_setting_peach_jingle()
            && data_0208e444 < 0 && data_0208e43c >= 0 && data_0209b4ac == data_0208e43c) {
            s_vol = data_0208e42c;
            s_mask0 = sd_seq_player_mask();
            s_player = -1;
            _ZN5Sound8SetMusicEjj((unsigned)data_0209f250, PEACH_SEQ);
            if (data_0209b4ac == PEACH_SEQ) {
                s_age = 0;
                fprintf(stderr, "[peach] unlock message 0x%x opened: NCS_BGM_PEACH (sequence %d) in place of music %d, volume target was %d\n", id, PEACH_SEQ, data_0208e43c, s_vol);
            }
        }
    }
    if (s_age >= 0) {
        /* what the game wants the music at: its duck while the unlock box is up (read before this tick overrides it), the full level once the box is closed */
        if (!open) s_vol = 0x7f;
        else if (data_0208e42c != 0x7f) s_vol = data_0208e42c;
        if (s_player < 0 && s_age >= 1 && s_age <= 20) {   /* the sound queue starts the sequence a tick after the call: the player that is newly sounding is the tune's */
            const unsigned fresh = sd_seq_player_mask() & ~s_mask0;
            for (int p = 0; p < SD_SEQ_PLAYERS && s_player < 0; ++p)
                if ((fresh >> p) & 1u && sd_seq_active(p)) s_player = p;
            if (s_player >= 0) fprintf(stderr, "[peach] the tune is on sequence player %d\n", s_player);
        }
        const bool over = s_player >= 0 ? (s_age > 20 && !sd_seq_active(s_player)) || s_age >= PEACH_MAX : s_age >= PEACH_CAP;
        if (!over) {
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, PEACH_FADE);   /* the unlock state's own duck runs earlier in the frame; this is what the sound tick sees */
            ++s_age;
        } else {
            _ZN5Sound8EndMusicEjj((unsigned)data_0209f250, 0);
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE((unsigned)s_vol, PEACH_FADE);
            fprintf(stderr, "[peach] NCS_BGM_PEACH ended after %d frames; level music %d back, volume target %d\n", s_age, data_0209b4ac, s_vol);
            s_age = -1;
        }
    }
}

/* hal/stage_frame.cpp's Stage::UpdateMessage face lands here. */
extern "C" void port_message_pump(void)
{
    dScStage_c::UpdateMessage_rom();
    port_peach_jingle_tick();
}
