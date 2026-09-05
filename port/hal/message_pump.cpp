// The message-box PUMP, host side.
//
// WHAT WAS MISSING. The dialogue box's whole state machine is matched src and
// already links: Message::UpdateWindow (src/_ZN7Message12UpdateWindowEv.cpp,
// the box open/close animation and its WIN0 register writes) and Message::Update
// (src/_ZN7Message6UpdateEv.cpp, the per-frame box logic, cursor OAM and the
// AddChar glyph stream). Nothing TICKED them, so the box never advanced.
//
// On the ROM the tick is Stage::Behavior's, in exactly one place. When
// data_0209f2d8 != 1 (i.e. not the VS split-screen mode) Stage::Behavior calls
// Stage::UpdateMessage (src/_ZN5Stage13UpdateMessageEv.cpp), whose body is:
//
//     void Stage::UpdateMessage() {
//         if (data_0209d660 == 0) return;              // no message active
//         if (Message::UpdateWindow()) {               // box fully open?
//             if (data_0209d654 == 0) { Message::Update(); return; }  // normal
//             ... save-screen countdown ...            // data_0209d654 != 0
//         }
//         ... save-screen close ...
//     }
//
// The Stage runs on the DS's behaviour PROCESSING LIST at priority 3, and
// slot 7 (Scene::BeforeBehavior) clears its pause bits so slot 6
// (Stage::Behavior) runs every frame. The port does not run the Stage as a
// list actor yet -- hal/stage_bridges.cpp drives its Render and texture-anim
// halves by hand and traps every vtable slot -- so Stage::Behavior, and with it
// UpdateMessage, never ran.
//
// WHY A HOST COPY AND NOT THE MATCHED FILE. src/_ZN5Stage13UpdateMessageEv.cpp
// is a STUB: it defines its own empty Message::UpdateWindow / Message::Update /
// Message::DisplaySaving / SaveData::SaveCurrentFile locally and calls those,
// so linking it would TICK NOTHING -- the message box would be driven by four
// empty bodies. That reason alone is decisive.
//
// (This comment used to give a second reason, that linking it would "collide
// with the real Message symbols already in the slice". That one does not
// survive checking and has been dropped: MSVC encodes the return type and the
// static/instance distinction into a member function's decorated name, so the
// stub's `static bool Message::UpdateWindow()` decorates to
// ?UpdateWindow@Message@@SA_NXZ while the real one in walk_window.map is
// ?UpdateWindow@Message@@SAHXZ, and the stub's `static void Message::Update()`
// is ?Update@Message@@SAXXZ against the real ?Update@Message@@QAEXXZ. Different
// symbols, no collision. The other two resolve against C names entirely.)
//
// And Stage::Behavior itself pulls in the entire
// pause/VS/level-change machinery (PS_Update, VE_Init, Scene::SetSceneToSpawn,
// data_0209f5bc->v5 ...), none of which the port hosts. So this file is the ONE
// statement of Stage::Behavior that owns the message box -- UpdateMessage's own
// body, calling the REAL matched Message methods -- and nothing else. It is the
// same treatment the +0x13 pause-bit clear and the LC save-prompt clear already
// get in stage_bridges.cpp / level_boot.cpp: a stand-in for one line of the
// Stage's own Behavior, to be retired the day the Stage runs as a real actor.
//
// THE SAVE-SCREEN ARM (data_0209d654 != 0) IS NOW SERVICED. It is the exact
// tail of Stage::UpdateMessage (src/_ZN5Stage13UpdateMessageEv.cpp), the same
// one line of the Stage's own Behavior this file already owns for the dialogue
// arm. It is reached in ordinary play: PowerStar state 11 case 2
// (func_ov002_020e9af4) calls Message::DisplaySaving(0x295) when the player picks
// the save choice (t==1, the low two bits of the star's +0x4a2, seeded from the
// message box's last choice data_0209d684). DisplaySaving arms
// data_0209d654=1 / data_0209d660=1 / data_0209d6d4=0x295 and seats a countdown
// data_0209d67c=0x3c. The DS ticks it every frame from Stage::Behavior; the port
// skipped it, so the save box never closed, data_0209d660 never cleared, and the
// star's case 3 waited on data_0209d660==0 forever -- the whole gameplay handoff
// (func_ov002_020e8618: untrack, MarkForDestruction, Event bits) never ran, and
// EVERY DisplaySaving caller in the game stalled the same way.
//
// The arm's body, byte for byte the matched Stage::UpdateMessage:
//   UpdateWindow()==1 (box open), data_0209d654!=0, id==0x295:
//     at data_0209d67c==0x3c run SaveData::SaveCurrentFile() ONCE; then decrement
//     data_0209d67c by data_0208ee44 and, at 0, set data_0209d670=1 (request the
//     window close). Message::UpdateWindow then animates the box shut and clears
//     data_0209d660 itself.
//   UpdateWindow()==0 (box shut), data_0209d654!=0, data_0209d660==0:
//     id!=0x295 -> clear data_0209d654 (arm done); id==0x295 -> DisplaySaving(0x296),
//     the "Saved!" confirmation, which loops the arm once more and on its own close
//     (id 0x296 != 0x295) clears data_0209d654.
//
// THE FILE WRITE IS REAL NOW (run link100, lane SAVE). This block used to say
// the write "bottoms out on the DS backup-cart hardware layer
// (func_0203da3c/func_0206045c/func_02057020/func_02060484), none of which the
// port hosts, and none of which is in any build slice", and stubbed the leaf
// with a one-shot note to stderr. All four of those are in the binary now:
// port/slice_gate215.txt links the card driver and port/ntr/backup.cpp hosts the
// medium under it, so SaveData::SaveCurrentFile -> SaveFile -> SaveDataToCart
// reaches a real 8192-byte cartridge image on disk. port_save_current_file below
// is a two-line wrapper over the ROM's own function and nothing else.
//
// The box CLOSE still does not depend on the write completing -- it is driven
// purely by the data_0209d67c countdown -- which is the ROM's own arrangement
// and is left exactly as it was.
#include <cstdio>
#include <cstdlib>

/* Message::UpdateWindow is a real C++ static method (int Message::UpdateWindow()
   in src/_ZN7Message12UpdateWindowEv.cpp), so it is reached through the class,
   not an itanium C name -- MSVC mangles the static to ?UpdateWindow@Message@@SAHXZ
   and only the qualified call spells it. Message::Update is a non-static method
   faced in hal/reverse_bridges.cpp as the C entry _ZN7Message6UpdateEv(void*);
   it reads only globals, so the self pointer is unused. */
struct Message {
    static int UpdateWindow();
};

extern "C" {

void _ZN7Message6UpdateEv(void *self);     /* Message::Update, faced in reverse_bridges */
int  _ZN8SaveData15SaveCurrentFileEv(void);  /* the ROM save, gate 215 */

static void port_save_current_file(void);  /* the save-write leaf (below) */

extern unsigned char data_0209d660;   /* message-active flag */
extern unsigned char data_0209d654;   /* save-screen flag (0 for a dialogue) */
extern unsigned char data_0209d698;   /* box target screen: 0 engine A, 1/2 sub */
extern unsigned char data_0209d6bc;   /* Message::Update state var */
extern short         data_0209d6d4;   /* current message id (-1 = none) */

/* the save-screen arm's own globals: the countdown data_0209d67c (seated 0x3c by
   DisplaySaving), its per-frame step data_0208ee44 (the game's frame-count/rate,
   1 at 60fps), and the window-close request data_0209d670 that Message::UpdateWindow
   reads to animate the box shut. All matched-src symbols, all in the link. */
extern unsigned char data_0209d67c;   /* save-screen countdown */
extern int           data_0208ee44;   /* per-frame decrement step (frame rate) */
extern unsigned char data_0209d670;   /* window close request */

/* Message::DisplaySaving(u16) -- the matched save prompt (src/_ZN7Message13
   DisplaySavingEt.cpp, in the link via slice_gate18). The arm re-fires it with
   0x296 for the "Saved!" confirmation box. */
void _ZN7Message13DisplaySavingEt(unsigned short id);

/* THE ENGINE-A DISPLAY SYNC, the piece the port skips. On the DS func_02019144
   runs once per frame and, among other things, copies the software BG-offset
   shadows and the BG-enable layer mask into the engine-A hardware registers.
   SetBg3Offset (src/SetBg3Offset.c) writes data_0209d48c/d490 -- shadows, NOT
   hardware -- and func_0201f32c scrolls BG3 through it so the box content (which
   sits at the TOP of the BG3 tilemap) lines up under the WIN0 window lower down
   the screen. Without the flush the hardware BG3 offset stays 0, the content
   never scrolls under the window, and the compositor reads only the empty fill
   tile inside the window (the 2026-08-08 "box active but 0 px composited"
   finding).

   hal/sub_screen.cpp already reproduces func_02019144's OTHER beats -- the sub
   DISPCNT mask publish, the minimap affine callback, OAM Load. This is the
   engine-A remainder those did not cover: the engine-A DISPCNT layer-mask
   publish (data_0209d45c) and the four engine-A BG offset registers, byte for
   byte as func_02019144 lines 46,49,53,54,55 write them. Reached only while a
   message is up, so it touches nothing when no box is open. */
extern unsigned char data_0209d45c;   /* engine-A BG-enable layer mask */
extern short data_0209d48c, data_0209d490;   /* BG3 hofs/vofs shadows */

/* THE MESSAGE-BOX INPUT PUBLISH, the piece the port skips. On the DS the pad
   record data_020a0e58 (PadData[4], stride 4: {u16 held @ +0, u16 pressed @ +2})
   is filled by the pad read every frame, and data_020a0e5a is the SAME memory --
   pad 0's pressed halfword -- read by IsButtonInputValid (src/IsButtonInputValid.c)
   and Message::Update (src/_ZN7Message6UpdateEv.cpp) to advance and DISMISS the
   box. The port splits those two into separate auto_bss symbols, and its input
   layer publishes the face buttons into the game's remapped Ctrl block
   (data_0209f49c held / data_0209f49e pressed) directly rather than through the
   pad record, so nothing ever reaches data_020a0e5a. IsButtonInputValid then
   reads 0 every frame, Message::Update never leaves its wait state, the box
   cannot be dismissed and the player is softlocked (the castle-grounds door
   bug -- the box opens and shows text, but there is no way out).

   This restores the DS's pad aliasing for the one consumer that needs it: while a
   message is active it republishes the local player's pressed face buttons into
   the pad record (data_020a0e58's pressed halfword) and its overlay symbol
   (data_020a0e5a), taking the press edge from the Ctrl block the input layer has
   already filled this frame. The Ctrl-block pressed word carries A in bit 0 and B
   in bit 1 (StartTalk's own gate reads data_0209f49e & 3), which is the same low
   two bits IsButtonInputValid's mode-0 test (data_020a0e5a & 0xf) reads for A/B,
   so the two low bits carry straight across with no remap. Reached only while a
   message is up, so it touches the pad record for nothing else. */
extern unsigned char data_020a0e40;          /* the local player index */
extern unsigned short data_020a0e58[];       /* PadData[4]: [i*2] held, [i*2+1] pressed */
extern unsigned short data_020a0e5a[];        /* pad0.pressed overlay the box reads */
extern unsigned short data_0209f49e[];        /* Ctrl block: pressed-this-frame */

static void port_message_publish_input(void)
{
    int idx = (int)data_020a0e40;
    if (idx < 0 || idx > 3)
        idx = 0;
    /* A (bit0) and B (bit1) pressed this frame, from the remapped Ctrl block
       (data_0209f49e is already a one-frame edge: the input layer writes it as
       btn & ~btn_was), folded onto whatever the pad record's own pressed edge
       carries (the D-pad, written fresh each frame by the input layer). */
    unsigned short pressed = (unsigned short)(data_020a0e58[idx * 2 + 1] |
                                              (data_0209f49e[0] & 0x3));
    data_020a0e58[idx * 2 + 1] = pressed;
    /* data_020a0e5a is the DS overlay of the pressed halfword -- the same memory
       on hardware, split into its own symbol on the port and never otherwise
       written. ASSIGN (not OR) so it tracks the pad record's per-frame edge
       exactly, instead of latching a stale press that would dismiss every future
       box on its first frame. */
    data_020a0e5a[idx * 2] = pressed;
}

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

/* Reproduces Stage::UpdateMessage's dialogue arm, called once per frame. This
   is the exact tick Stage::Behavior gives the box when data_0209f2d8 != 1. */
void port_message_pump(void)
{
    /* diagnostic: report the box state each frame it is active for the first
       ~30 active frames, so a headless run can watch UpdateWindow animate the
       box open (d658/d64c grow to d6d0/d6c8) and Message::Update advance d6bc. */
    if (std::getenv("SM64DS_MSG_PUMP_DEBUG") && data_0209d660) {
        extern unsigned char data_0209d658, data_0209d64c, data_0209d6d0,
            data_0209d6c8, data_0209d650, data_0209d6cc;
        static int n;
        if (n < 1000) {
            ++n;
            std::fprintf(stderr, "[msgpump] active: d698=%u d6bc=%u id=%d "
                         "cur(w=%u h=%u) max(w=%u h=%u) min(w=%u h=%u) close=%u | "
                         "save(d654=%u d67c=%u)\n",
                         (unsigned)data_0209d698, (unsigned)data_0209d6bc,
                         (int)data_0209d6d4, (unsigned)data_0209d658,
                         (unsigned)data_0209d64c, (unsigned)data_0209d6d0,
                         (unsigned)data_0209d6c8, (unsigned)data_0209d650,
                         (unsigned)data_0209d6cc, (unsigned)data_0209d670,
                         (unsigned)data_0209d654, (unsigned)data_0209d67c);
        }
    }

    if (data_0209d660 == 0)
        return;                        /* no message active: same first line */

    /* restore the DS pad aliasing the box's input readers depend on, so a real
       A/B press can advance and dismiss the box (without it the box softlocks --
       the castle-grounds door bug). Reached only while a message is active. */
    port_message_publish_input();

    if (Message::UpdateWindow()) {     /* box fully open */
        if (data_0209d654 == 0) {
            _ZN7Message6UpdateEv(0);   /* the dialogue tick */
            port_message_flush_engine_a_regs();
            return;
        }
        /* SAVE-SCREEN ARM (data_0209d654 != 0), the box is fully open.
           Stage::UpdateMessage's save arm, verbatim. Only id 0x295 (the initial
           "Now saving..." box) runs the write; every arm variant runs the same
           countdown-to-close. */
        if (data_0209d6d4 == 0x295) {
            if (data_0209d67c == 0x3c)
                port_save_current_file();   /* the write, at the countdown's top */
        }
        if (data_0209d67c != 0) {
            data_0209d67c -= (unsigned char)data_0208ee44;
            if (data_0209d67c == 0)
                data_0209d670 = 1;          /* request the window close */
        }
        return;
    }

    /* UpdateWindow()==0 with the save arm set: either the box is still animating
       shut (data_0209d660 still 1 -- nothing to do, wait a frame) or it has fully
       closed (UpdateWindow cleared data_0209d660). On close, id 0x295 hands off to
       the "Saved!" confirmation 0x296; any other id ends the arm. */
    if (data_0209d654 != 0) {
        if (data_0209d660 != 0)
            return;                         /* still closing */
        if (data_0209d6d4 != 0x295) {
            data_0209d654 = 0;              /* arm complete */
            if (std::getenv("SM64DS_MSG_PUMP_DEBUG"))
                std::fprintf(stderr, "[msgpump] SAVE ARM COMPLETE: box closed, "
                             "data_0209d654 cleared, data_0209d660=%u (id was "
                             "0x%x)\n", (unsigned)data_0209d660,
                             (unsigned)(unsigned short)data_0209d6d4);
            return;
        }
        _ZN7Message13DisplaySavingEt(0x296);  /* "Saved!" confirmation box */
    }
}

/* The save-write leaf. The ROM's own SaveData::SaveCurrentFile, linked from
   src/_ZN8SaveData15SaveCurrentFileEv.c through port/slice_gate215.txt: it
   writes the current file (slot data_0209caa0[0x328]) and then the minigame
   record, and the value it returns is 1 when both reached the medium. Nothing
   in the ROM reads that value here either -- Message's countdown owns the box
   -- so it is traced under the pump's own debug flag and dropped. */
static void port_save_current_file(void)
{
    int ok = _ZN8SaveData15SaveCurrentFileEv();
    if (std::getenv("SM64DS_MSG_PUMP_DEBUG"))
        std::fprintf(stderr, "[msgpump] SaveData::SaveCurrentFile -> %d" "\n", ok);
}

} /* extern "C" */
