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

extern "C" {

void _ZN7Message6UpdateEv(void *self);     /* Message::Update, faced in reverse_bridges */

extern unsigned char data_0209d45c;   /* engine-A BG-enable layer mask */
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
struct Stage {
    static void UpdateMessage_rom();   /* src/_ZN5Stage13UpdateMessageEv.cpp, renamed */
};

/* ?Update@Message@@SAXXZ: the ROM body's dialogue-arm call. The matched
   Message::Update, then the engine-A sync the box needs on the port. */
void Message::Update()
{
    _ZN7Message6UpdateEv(0);
    port_message_flush_engine_a_regs();
}

/* hal/stage_frame.cpp's Stage::UpdateMessage face lands here. */
extern "C" void port_message_pump(void)
{
    Stage::UpdateMessage_rom();
}
