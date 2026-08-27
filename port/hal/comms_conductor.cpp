// THE CONDUCTOR'S HOST SIDE. Run mg16, lane MP3.
//
// src/func_0203ea5c.c -- the ROM's own multiplayer lockstep -- is linked as of
// this lane, and src/func_0203df40.c with it. This file is everything the host
// owes them, and nothing else. What it is NOT is a driver: the ROM drives
// itself now. MP2's hal/comms_lockstep.cpp was a transcription written because
// this TU was in no slice, and it is retired by the same commit that adds this
// one.
//
// FOUR JOBS, in the order the frame reaches them:
//
//   1. THE DS KEY REGISTER. src/func_0203df40.c:31 reads KEYINPUT and the ARM7
//      shared pad word and computes the key halfword the wire carries. The port
//      never wrote either register, and zero through the ROM's own formula is
//      not "no buttons" -- it is EVERY button. See THE STUCK CONTROLLER below.
//   2. THE DS GLOBALS nothing had hosted. The conductor reaches four of them as
//      BANDS -- base-plus-offset walks over runs of separate DS symbols -- so
//      those go down in ROM order and contiguously, not as loose arrays.
//   3. ONE WM FACE. func_0203fd64 is game code and is linked; what it sends its
//      command through is the radio, and the radio stops at the seam.
//   4. THE PUMP. The ROM's wait sleeps through OS_SleepThread, and that is the
//      "pump turn" comms_seam.h has described since MP1 without one existing.
//      Installing it here is what makes the contract's `poll` entry true.
//
// ===========================================================================
// THE STUCK CONTROLLER, and why the key register is written rather than the
// record.
//
// src/func_0203df40.c:31, verbatim from the matched TU:
//
//     data_020a1040.unkE = ((*(volatile u16 *)0x4000130 |
//                            *(volatile u16 *)0x27fffa8) ^ 0x2fff) & 0x2fff;
//
// The DS's KEYINPUT is ACTIVE LOW: a 0 bit means PRESSED. The XOR is what turns
// it the right way up. port/ppu_gap_audit.txt measured both registers at 0 on
// every one of 300 and 900 samples, because nothing in the port had ever
// written them -- and 0 through that expression is 0x2fff, which is all
// fourteen buttons held down, on every frame, forever, on both instances.
//
// The port already met this once and dodged it: hal/scene_boot.cpp:3802-3811
// publishes the key word as a literal 0 and says in its own comment that "a
// literal transcription would hand the title a stuck controller". That dodge
// was available because that path HOSTS the record filler. It is not available
// here, because the whole point of this lane is that the ROM's own line runs.
//
// So the fix goes where the hardware is, not where the game is. The port has
// the right value already -- hal/input_probe.cpp:304 records that the pad
// mirror is in DS KEYINPUT bit order and already active-high -- so writing the
// REGISTER active-low makes the ROM's expression evaluate correctly with not
// one line of game code diverging:
//
//     KEYINPUT = (held ^ 0x2fff) & 0x2fff        shared word left at 0
//     =>  ROM reads ((K | 0) ^ 0x2fff) & 0x2fff  ==  held
//
// That is the port's north star applied literally: answer the question where
// the hardware asks it. It is also the same shape as comms_set_boot_indicator,
// which writes 0x027FFC40 rather than teaching the game to skip the read.
//
// ORDERING IS LOAD-BEARING and it is the trap MP2 wrote down. With the ROM's
// fan-out on, func_0203bc7c OVERWRITES the pad mirror from the four comms
// records later in the same frame. Publishing the register from the mirror
// AFTER that would feed the wire back into itself. port_comms_publish_pad is
// therefore called from where walk_window computes its raw pad bits, upstream
// of both the conductor and the fan-out, and takes the value as an ARGUMENT
// rather than reading the mirror itself -- so the ordering is enforced by the
// signature instead of by a comment.
// ===========================================================================

#include "comms_seam.h"
#include "comms_loopback.h"   // the carrier's role, for the retry
#include "os_thread.h"

#include <windows.h>   // ::Sleep, for the pump's one-millisecond yield

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ===========================================================================
// 2. THE DS GLOBALS.
//
// SECTION SUFFIX $ymp3, and it is a measurement rather than a preference --
// the same one MP1 made for $wcomms and MP2 for $xmp2. port/tools/battery.py's
// header: some hosted DS data reaches the geometry stream as a POINTER VALUE,
// so an insertion INTERIOR to .dsstate shifts every global past it and changes
// the rendered frame while leaving the section base where it was. 'y' sorts
// after 'x' and before the $zzz sentinel, so these land above MP1's three and
// MP2's two and NOT ONE existing hosted global moves. Appending to $xmp2
// instead would leave the order within that group to the linker, and if these
// landed ahead of MP2's two then MP2's two would move.
//
// SIZED BY ROM SPAN, every one, off config/arm9/symbols.txt -- never by the
// width of the first field that reads them. That is the undersized-host-global
// trap this tree has been bitten by before, and this lane is unusually exposed
// to it because the conductor reaches most of these as interior addresses.
//
// FOUR OF THEM ARE BANDS, not arrays. A band is a run of separate DS symbols
// the game walks with base-plus-offset arithmetic, so the run has to come out
// of the linker contiguous and in ROM order or the walk lands in the wrong
// place. Each band below is declared as consecutive definitions inside one
// section group, every member sized by its own ROM span, and no member carries
// an align specifier -- an alignment request on an interior member is exactly
// what would insert the padding that breaks the walk. Only band heads are
// aligned, and only where the DS requires it.
// ===========================================================================

// ONE SECTION PER SYMBOL, NUMBERED, and that is the part that actually makes a
// band a band. MSVC merges grouped sections in lexical order of the text after
// the '$', but WITHIN one section the order of objects is left to the linker.
// So a single .dsstate$ymp3 holding eleven definitions would compile, link, and
// put them in whatever order it liked -- and a base-plus-offset walk over a
// shuffled band reads the wrong bytes while every guard stays green.
// hal/camera_bridges.cpp already solved this for the two records it hosts by
// giving each symbol its own numbered section ($camcomm0000..0007), so the
// SORT ORDER pins the layout rather than a hope about link order. Same recipe
// here, and the numbers are assigned in ROM address order.
//
// align(1), NOT camera_bridges' align(2). Its runs are all even-sized so
// align(2) packs them exactly; this band has members of 1, 3 and 0x11 bytes,
// and an alignment request of 2 on an odd-sized member is precisely what would
// insert the interior padding that breaks the walk. The precedent for a
// byte-granular ROM-spaced run is hal/auto_bss.cpp:184-189's $touch0000..0003.
// The section name is passed WHOLE, not assembled from a prefix and a suffix:
// __pragma(section(...)) takes a literal and does not perform the adjacent
// string-literal concatenation an ordinary expression would, so ".dsstate$ymp3"
// sec is a syntax error rather than a name. camera_bridges.cpp's COMM passes
// the full name for the same reason. Found by the compiler, recorded here so
// the next band does not rediscover it.
#define MP3_BSS(sec, name, size)                                     \
    __pragma(section(sec, read, write))                              \
    extern "C" __declspec(allocate(sec))                             \
    __declspec(align(1)) unsigned char name[size] = {0}

// ---------------------------------------------------------------------------
// TWO GLOBALS INHERITED FROM MP2, AND THEY KEEP $xmp2 ON PURPOSE.
//
// hal/comms_lockstep.cpp hosted these and is retired by the same commit that
// adds this file. They are reproduced here VERBATIM -- same section, same
// order, same sizes, same initialisers -- rather than folded into $ymp3 below,
// because $xmp2 sorts before $ymp3 and moving them into it would shift them
// past MP1's three at $wcomms... no: it would shift them relative to nothing,
// but it WOULD change the address of every hosted global between $xmp2 and
// $ymp3, and "not one existing hosted global moves" is the property this whole
// suffix scheme exists to keep. Only one object contributes to $xmp2 either
// way, so the group's contents and position are byte-for-byte what MP2 left.
//
// data_020a0f08 is the player-count byte src/func_0203ea5c.c:288 writes from
// block+0x0D. data_02099e18 is the counter :279 bumps once per live peer record
// accepted. Both are now written by the LINKED ROM code rather than by a
// transcription, which is the difference this lane makes.
#pragma section(".dsstate$xmp2", read, write)
extern "C" {
__declspec(allocate(".dsstate$xmp2")) __declspec(align(4))
unsigned char data_020a0f08[4] = {0};   // 0x020a0f08 .. 0x020a0f0c
__declspec(allocate(".dsstate$xmp2")) __declspec(align(4))
int data_02099e18 = 0;                  // 0x02099e18 .. 0x02099e1c
}

// ---------------------------------------------------------------------------
// BAND A -- THE STAGED 0x20 BLOCK, 0x020a1020 .. 0x020a1040.
//
// This is the one the wire format is about. src/func_0203ea5c.c:169-186 stages
// into it with ten CpuCopy8 moves and then passes &data_020a1020 to
// func_020406b4, which hands the transport exactly kCommsBlockBytes from that
// address. Eleven DS symbols, spans 2+1+3+2+1+1+1+2+1+1+17, and they sum to
// exactly 0x20 -- which is the arithmetic that proves the band is the block
// rather than a guess about where it ends.
//
// THE BLOCK IS A UNION AND THE SPANS SAY SO. :171 copies 0x16 bytes into
// data_020a1023, whose own span is 3: on the info-mode path the block carries a
// 0x16-byte payload at +3 and the structured fields at +6..+0x10 are simply not
// there. That write RUNS THROUGH its neighbours, which is legal on the DS
// because they are contiguous, and is the reason this cannot be eleven separate
// arrays.
MP3_BSS(".dsstate$ymp3a0000", data_020a1020, 2);    // flag word
MP3_BSS(".dsstate$ymp3a0001", data_020a1022, 1);    // info selector
MP3_BSS(".dsstate$ymp3a0002", data_020a1023, 3);    // + the 0x16 info payload, see above
MP3_BSS(".dsstate$ymp3a0003", data_020a1026, 2);    // frame counter, low half
MP3_BSS(".dsstate$ymp3a0004", data_020a1028, 1);    // stylus x
MP3_BSS(".dsstate$ymp3a0005", data_020a1029, 1);    // stylus y
MP3_BSS(".dsstate$ymp3a0006", data_020a102a, 1);    // touch
MP3_BSS(".dsstate$ymp3a0007", data_020a102b, 2);    // heading
MP3_BSS(".dsstate$ymp3a0008", data_020a102d, 1);    // player count, slot 0 only
MP3_BSS(".dsstate$ymp3a0009", data_020a102e, 1);    // reserved, slot 0 only
MP3_BSS(".dsstate$ymp3a0010", data_020a102f, 17);   // game payload
// 0x020a1040 begins data_020a1040, the local record, hosted by
// hal/camera_bridges.cpp. Nothing may be added between here and there.

// ---------------------------------------------------------------------------
// BAND B -- THE PER-PLAYER INFO BUFFERS, 0x020a10a4 .. 0x020a1154.
//
// src/func_0203ea5c.c:171 indexes data_020a10a4 by (sel * 0x16) for sel in
// 0..3, and :319/:327 walk data_020a10ba and data_020a1112 in 0x16 strides.
// Eight DS symbols summing to 0xb0, landing exactly on data_020a1154 -- the
// four records camera_bridges already hosts. data_020a10a4's own span is 2 and
// its declared use is 0x16, which is the undersized trap in miniature: the
// other 20 bytes are data_020a10a6, and they are only there if the band is.
MP3_BSS(".dsstate$ymp3b0000", data_020a10a4, 2);
MP3_BSS(".dsstate$ymp3b0001", data_020a10a6, 20);
MP3_BSS(".dsstate$ymp3b0002", data_020a10ba, 22);
MP3_BSS(".dsstate$ymp3b0003", data_020a10d0, 22);
MP3_BSS(".dsstate$ymp3b0004", data_020a10e6, 22);
MP3_BSS(".dsstate$ymp3b0005", data_020a10fc, 2);
MP3_BSS(".dsstate$ymp3b0006", data_020a10fe, 20);
MP3_BSS(".dsstate$ymp3b0007", data_020a1112, 66);   // three more 0x16 slots

// ---------------------------------------------------------------------------
// BANDS C and D -- THE SIX-BYTE PLAYER IDS, 0x020a0fa6 and 0x020a0fbe.
//
// src/func_0203ea5c.c:321 asks func_0204271c whether an id is all zero and :326
// asks func_02042748 whether two are equal; both read THREE HALFWORDS, so the
// unit is six bytes, and the loops step by 6 four times and three times. Four
// ids of six bytes is 24, which is what the conductor's own extern declares and
// what span+neighbour gives: 18+6 and 18+8.
// Spans 18+6 and 18+28, contiguous, landing exactly on data_020a0fec.
MP3_BSS(".dsstate$ymp3c0000", data_020a0fa6, 18);
MP3_BSS(".dsstate$ymp3c0001", data_020a0fb8, 6);
MP3_BSS(".dsstate$ymp3c0002", data_020a0fbe, 18);
MP3_BSS(".dsstate$ymp3c0003", data_020a0fd0, 28);

// ---------------------------------------------------------------------------
// THE SCALARS. No interior addressing reaches any of these, so they are loose
// definitions and their order does not matter. Still ROM-spanned.
// ONLY WHAT THE LINKED TUs ACTUALLY REFERENCE. Hosting the rest of these two
// bands "while we are here" would add hundreds of bytes to the captured
// .dsstate span for no reader, and every one of them would be a symbol the
// port defines and nothing explains.
MP3_BSS(".dsstate$ymp3s0000", data_02099e1c, 4);   // "open the radio" one-shot, :137-140
MP3_BSS(".dsstate$ymp3s0001", data_0209d4fc, 4);   // the per-VBlank sleep queue
MP3_BSS(".dsstate$ymp3s0002", data_020a0ef0, 4);   // bound selector, :142-146
MP3_BSS(".dsstate$ymp3s0003", data_020a0ef8, 4);   // info-mode countdown
MP3_BSS(".dsstate$ymp3s0004", data_020a0efc, 4);   // the leave countdown, :449-457
MP3_BSS(".dsstate$ymp3s0005", data_020a0f28, 4);   // channel, folded into the flag at :149
MP3_BSS(".dsstate$ymp3s0006", data_020a0f2c, 4);   // the WM command argument, :361 and :381
MP3_BSS(".dsstate$ymp3s0007", data_020a0f94, 4);   // the ROM's own link-state word
MP3_BSS(".dsstate$ymp3s0008", data_020a0f98, 5);   // last role, src/func_0203df40.c:77

// data_020a1fc0 is the WM work buffer. func_02040a94 reads word 3 of it and
// nothing else in the linked tree touches it, but it is SPANNED at 0x440 and
// goes down at 0x440: a 16-byte host sized by its one reader is precisely the
// undersized-global bug this section's banner names.
MP3_BSS(".dsstate$ymp3s0009", data_020a1fc0, 0x440);

// ---------------------------------------------------------------------------
// THE TOUCH RING, 0x020a0dd8 and 0x020a0df8.
//
// src/func_0203b9bc.c walks data_020a0df8 as NINE eight-byte entries (its index
// fixup is `if (k < 0) k += 9`) and settles the answer into data_020a0dd8. The
// spans agree exactly: 0x020a0df8 runs 0x48 = 9 * 8 up to data_020a0e40, and
// data_020a0dd8 runs 8 up to data_020a0de0. Two independent facts meeting is
// what makes this a reading rather than a guess.
//
// THE PORT DOES NOT FILL THIS RING, and that is stated here rather than
// discovered later. hal/sub_screen.cpp's poll_touch writes TouchInfo at
// data_020a0de8 directly; nothing writes the ring. So func_0203b9bc takes its
// `flag == 0` branch every frame and publishes the ROM's own IDLE QUAD --
// a = b = 0xff, c = 0, d = 0 -- which is exactly what the ROM publishes when
// nobody is touching the screen. That is correct, not degraded: the stylus half
// of the local record honestly reads "no touch". Feeding the ring from
// poll_touch is what would make a real stylus cross the wire, and it is a
// separate change with its own regression surface, so it is named here and not
// done here.
MP3_BSS(".dsstate$ymp3t0000", data_020a0dd8, 8);
MP3_BSS(".dsstate$ymp3t0001", data_020a0de0, 8);
MP3_BSS(".dsstate$ymp3t0002", data_020a0df8, 0x48);   // nine eight-byte ring entries

// ---------------------------------------------------------------------------
// AND THE BANDS ARE CHECKED AT BRING-UP, not asserted in a comment.
//
// camera_bridges.cpp:225-228 does exactly this for the two records it hosts,
// and the reason is that everything above is a claim about what the LINKER did.
// The sort order is meant to pin it; this is what notices if it did not. A
// shuffled band is otherwise invisible -- it links, every guard stays green,
// and the game reads the wrong bytes.
//
// Band A is the one that would hurt most: &data_020a1020 is handed to the
// transport as kCommsBlockBytes, so if that run is not 0x20 contiguous bytes
// the wire carries whatever else the linker put there.
// The far ends of three of the bands are hosted elsewhere and are what makes
// the checks below meaningful: a band that is internally contiguous but lands
// in the wrong place would still pass a self-check. data_020a1040 and
// data_020a1154 are hal/camera_bridges.cpp's; data_020a0e40 is auto_bss's.
extern "C" {
extern unsigned char data_020a1040[];
extern unsigned char data_020a1154[];
extern unsigned char data_020a0e40[];
// The ROLE byte, hosted by hal/stage_slot0.cpp. src/func_0203df40.c switches
// on it and nothing in the port was seating it; see HOLE 3 below.
extern unsigned char data_020a0f04[];
// The touch-panel scratch block, hosted by hal/scene_boot.cpp as int[6]. Word
// 6 AS A HALFWORD (byte +12) is the ring write index; see section 6.
extern int data_020a80cc[];
// The per-slot Player pointers and the controller count, both hosted by the
// port already. Read by the VS probe in section 8.
extern void *data_0209f394[];
extern unsigned char data_0209f21c;
extern unsigned char data_0209f250[];
// The per-slot "this slot is live" flags. _Z19LoadEntranceObjects... DISCARDS
// the actor it just spawned when this is 0 (`data_0209f394[i] = 0`), so a slot
// with a spawned actor and a clear flag looks identical to a slot that never
// spawned. The probe prints it for exactly that reason.
extern int data_0209fc5c[];
// hal/level_boot.cpp's own accessor for the level's entrance-record count.
int port_entrance_count(void);
// The fanned-out pad mirror and Stage::CheckInput's Ctrl block, for the probe's
// input-chain columns.
extern int data_020a0e58[];
extern int data_0209f498[];
}

namespace port {
namespace {
inline int *data_020a0e58_arr() { return data_020a0e58; }
inline unsigned char *data_0209f498_bytes() {
    return reinterpret_cast<unsigned char *>(&data_0209f498[0]);
}
}  // namespace
}  // namespace port

namespace port {
namespace {
inline unsigned char data_0209f21c_byte() { return data_0209f21c; }
inline unsigned char data_0209f250_byte() { return data_0209f250[0]; }
}  // namespace
}  // namespace port

namespace port {
namespace {
inline unsigned char *data_020a80cc_bytes() {
    return reinterpret_cast<unsigned char *>(&data_020a80cc[0]);
}
}  // namespace
}  // namespace port

extern "C" int port_comms_conductor_check_layout(void) {
    struct { const char *what; long got, want; } rows[] = {
        {"A: 1020->1040 block",  (long)(data_020a1040 - data_020a1020), 0x20},
        {"A: 1020->1023",        (long)(data_020a1023 - data_020a1020), 3},
        {"A: 1020->102f",        (long)(data_020a102f - data_020a1020), 0x0f},
        {"B: 10a4->10ba",        (long)(data_020a10ba - data_020a10a4), 0x16},
        {"B: 10a4->1112",        (long)(data_020a1112 - data_020a10a4), 0x6e},
        {"B: 10a4->1154",        (long)(data_020a1154 - data_020a10a4), 0xb0},
        {"C: 0fa6->0fbe",        (long)(data_020a0fbe - data_020a0fa6), 0x18},
        {"T: 0df8 ring stride",  (long)(data_020a0e40 - data_020a0df8), 0x48},
    };
    int bad = 0;
    for (unsigned i = 0; i < sizeof rows / sizeof rows[0]; ++i) {
        if (rows[i].got == rows[i].want) continue;
        ++bad;
        std::fprintf(stderr,
                     "  [conductor] BAND BROKEN %s: +%ld, expected +%ld\n",
                     rows[i].what, rows[i].got, rows[i].want);
    }
    if (bad)
        std::fprintf(stderr,
                     "  [conductor] %d band row(s) wrong. The linker did not "
                     "lay the .dsstate$ymp3 sections out in sort order, so "
                     "every base-plus-offset walk in src/func_0203ea5c.c is "
                     "reading the wrong bytes.\n", bad);
    return bad == 0;
}

// ===========================================================================
// 3. THE ONE WM FACE.
//
// src/func_0203fd64.c is game code and is linked by slice_mp3.txt. What it
// calls is not:
//
//     int func_0206259c(int a, int b) {
//         WM_GetSystemWork();
//         r = WM_CheckStateEx(1, 9);          if (r) return r;
//         WM_SetCallbackTable(0x23, a);
//         r = WM_SendCommand(0x23, 1, b);     if (r == 0) r = 2;
//         return r;
//     }
//
// Four WM SDK entries and nothing else -- this IS the radio, and comms_seam.h
// draws the line exactly here. So func_0203fd64 is linked and func_0206259c is
// hosted, which keeps the cut where the doctrine puts it instead of moving it
// one function up to save writing this.
//
// IT MUST ANSWER 2, AND THE REASON HAS TEETH. src/func_0203ea5c.c:361-363 and
// :381-383 are
//
//     data_020a0f2c = N;  do { } while (func_0203fd64() != 1);
//
// an UNBOUNDED busy loop -- no turn count, no bound, no escape. func_0203fd64
// answers 1 only when this returns 2, which on the DS means WM_SendCommand
// accepted the command. Any other answer here hangs the game solid, and neither
// of the two bounds this lane relies on (the ROM's 0x4B0/0x12C wait bound, the
// pump limit) is anywhere near this loop. 2 is also the honest answer: a
// loopback carrier has no radio mode to change, so the command always succeeds.
//
// WHAT THE COMMAND MEANS, since the flag it guards was undocumented. Both call
// sites bracket a flip of bit 0x2000 in the local record's flag word -- :364
// sets it after sending argument 0, :384 clears it after sending argument 1 --
// and 0x2000 in slot 0's flag word is what :213 and :218 test to force the
// round complete. comms_seam.h's wire-format table had 0x2000 as "...". It is
// the radio's info/idle mode, and the conductor asks the radio to enter and
// leave it. That line of the table is filled in by this lane.
// ===========================================================================

extern "C" int func_0206259c(int callback, int arg) {
    (void)callback;   // the DS registers func_02040634 here; it is empty
    (void)arg;        // data_020a0f2c, the mode the ROM is asking for
    return 2;         // WM_SendCommand accepted it. See above: 2 or hang.
}

// ===========================================================================
// 1. THE DS KEY REGISTER, and the scripted-input knob that now rides on it.
// ===========================================================================

namespace port {
namespace {

// SM64DS_COMMS_INJECT="key=<hex>[,x=<n>][,y=<n>][,touch=<n>]"
//
// MP2 put this knob in the transcription, where it wrote the local comms record
// directly. It CANNOT stay there: the record is now filled by the ROM's own
// func_0203df40, which would overwrite anything written before it and be
// overwritten by anything written after. So the knob moved DOWN a level, to the
// inputs func_0203df40 reads. That is a better place for it on the merits --
// it now drives the game the way a player does, through the hardware, and the
// ROM's line is still the only writer of the record.
//
// It exists because the harness's own scripted input (SM64DS_PAD_TEST,
// SM64DS_CLICK_TEST) is read as UNSET under SM64DS_WINDOW_SELFTEST
// (walk_window.cpp:3388 and :3480 both gate on g_selftest), and the headless
// two-instance proofs are selftest runs.
//
// THE STYLUS FIELDS ARE ACCEPTED AND DO NOT REACH THE WIRE, and saying so is
// the point. They used to be written straight into the record. Now they would
// have to arrive through the touch ring, which nothing fills (see the ring's
// own note above), so func_0203b9bc publishes the idle quad regardless. The
// parser keeps them so the ladder's existing invocations stay valid and the
// change is visible as a verdict rather than as a silent zero.
bool     g_inject_on = false;
unsigned g_inj_key = 0, g_inj_x = 0, g_inj_y = 0, g_inj_touch = 0;

void inject_parse() {
    static bool done = false;
    if (done) return;
    done = true;
    const char *s = std::getenv("SM64DS_COMMS_INJECT");
    if (!s || !*s) return;
    g_inject_on = true;
    for (const char *p = s; *p; ) {
        while (*p == ',' || *p == ' ') ++p;
        if (!*p) break;
        const char *eq = std::strchr(p, '=');
        if (!eq) break;
        const unsigned v = (unsigned)std::strtoul(eq + 1, 0, 0);
        if      (std::strncmp(p, "key",   3) == 0) g_inj_key   = v;
        else if (std::strncmp(p, "touch", 5) == 0) g_inj_touch = v;
        else if (std::strncmp(p, "x",     1) == 0) g_inj_x     = v;
        else if (std::strncmp(p, "y",     1) == 0) g_inj_y     = v;
        const char *c = std::strchr(eq, ',');
        if (!c) break;
        p = c + 1;
    }
    std::fprintf(stderr,
                 "[comms:conductor] injecting key=0x%04x into the DS key "
                 "register; stylus={%u,%u} touch=%u ACCEPTED BUT NOT CARRIED "
                 "(the port does not fill the touch ring func_0203b9bc reads, "
                 "so the ROM publishes its idle quad)\n",
                 g_inj_key, g_inj_x, g_inj_y, g_inj_touch);
}

}  // namespace

// The two registers src/func_0203df40.c:31 reads. 0x04000130 is inside ntr's
// mapped IO window (io.cpp's kRegions, IO_BASE 0x04000000 size 0x2000) and
// 0x027FFFA8 is inside the shared region, so both are plain backing store here
// and a write is read back verbatim -- which is the whole mechanism.
enum : unsigned {
    kDsKeyInput   = 0x04000130u,
    kDsSharedPad  = 0x027FFFA8u,
    kDsKeyMask    = 0x2fffu,
};

// THE STYLUS HALF OF THE INJECTION, fed to the TOUCH PANEL rather than to the
// record. hal/sub_screen.cpp's poll_touch calls this exactly where it already
// honours SM64DS_TOUCH_PROBE's poke, so an injected stylus enters the game at
// the same place a finger would: poll_touch writes it into the ROM's four-deep
// ring, src/func_0203b9bc.c reads the ring, src/func_0203df40.c puts what it
// returns into the local record, and the conductor stages that onto the wire.
// Not one of those steps is the port's.
//
// AN EARLIER REVISION OF THIS FILE ACCEPTED THE STYLUS FIELDS AND DROPPED THEM,
// and said so loudly in its log line, because nothing filled the ring. Filling
// the ring is what made them carryable, so the log line changed with the code.
bool comms_inject_touch(int *down, int *x, int *y) {
    inject_parse();
    if (!g_inject_on || !g_inj_touch) return false;
    *down = 1;
    *x = (int)g_inj_x;
    *y = (int)g_inj_y;
    return true;
}

// ===========================================================================
// THE SESSION DROPPED, AND NOBODY WAS GOING TO ASK AGAIN -- HOLE 5, in the
// shape it actually bites.
//
// src/func_0203ea5c.c:487 drops the session back to solo when the wait bound
// runs out: `data_020a0f04 = 0`. That is correct ROM behaviour and it must
// stay. What was missing is what happens NEXT. On the DS the player is sitting
// in the multiplayer menu, which re-requests a session; in this port nothing
// re-seated the role byte, so the first bound expiry was permanent. Once
// data_020a0f04 reached 0, src/func_0203df40.c's switch took its solo arm on
// every frame forever and the transport was never asked for anything again.
//
// RUNG 7 IS WHAT THAT LOOKS LIKE FROM OUTSIDE, and it is the reason this exists
// rather than a theory about reconnects. That rung starts the CHILD two seconds
// before the parent. The child spends its whole 0x4B0 bound knocking at a port
// nobody has bound yet, gives up, drops to solo -- and then the parent comes
// up, binds, and finds a peer that has stopped talking. Neither side ever
// joined. The same rung passed for MP2, whose transcription re-decided the role
// itself on every tick and so retried by accident.
//
// So the seam notices the drop and does the two things the menu would: it tells
// the TRANSPORT (abandon(), so the carrier forgets the round the game walked
// away from) and it re-seats the request. This runs once per frame from
// comms_publish_pad, which is already the one call that lands upstream of the
// dispatcher.
//
// IT IS GATED ON A TRANSPORT BEING INSTALLED, so a single-player session never
// reaches it and the ROM's own solo behaviour is untouched. And it is gated on
// the transport still being CONNECTED: if the peer has genuinely gone, the link
// state is not 3 or 4, and re-requesting would spin the ROM's bound forever
// instead of letting the session end.
void comms_retry_dropped_session() {
    const CommsTransport *t = comms_transport();
    if (!t) return;                        // solo: the ROM's own behaviour
    if (data_020a0f04[0] != kCommsRoleSolo) return;   // still in session

    const int st = t->state();
    if (st != kCommsParentConnected && st != kCommsChildConnected) return;

    t->abandon();                          // forget the round the game left

    static unsigned long long n = 0;
    if (++n == 1)
        std::fprintf(stderr,
                     "[comms:conductor] the ROM dropped the session to solo "
                     "(src/func_0203ea5c.c:487) while the transport is still "
                     "connected (link=%d). Re-seating the request the way the "
                     "DS's multiplayer menu would; this message is printed "
                     "once.\n", st);

    const CommsLoopbackStats ls = comms_loopback_stats();
    comms_seat_session_request(ls.role == 1 ? kCommsRoleParent
                                            : kCommsRoleChild);
}

void comms_publish_pad(unsigned held) {
    inject_parse();
    comms_retry_dropped_session();
    if (g_inject_on) held = g_inj_key;

    // ACTIVE LOW, which is the entire point. See THE STUCK CONTROLLER at the
    // top of this file: the ROM XORs with 0x2fff on the way in, so the register
    // has to carry the complement for the game to read what the player pressed.
    *reinterpret_cast<volatile unsigned short *>(kDsKeyInput) =
        (unsigned short)((held ^ kDsKeyMask) & kDsKeyMask);

    // Left at 0. On the DS this is the ARM7's copy of the X/Y/hinge bits, which
    // are not visible at 0x04000130; the port has no ARM7 publishing it, and 0
    // is the correct "none of those pressed" value once the OR is taken.
    *reinterpret_cast<volatile unsigned short *>(kDsSharedPad) = 0;
}

// ===========================================================================
// 8. THE VS PROBE -- the instrument rungs 9 to 11 read their verdict off.
//
// SM64DS_VS_PROBE=1 prints one line per frame per live player slot, read
// STRAIGHT OUT OF THE GAME'S OWN ACTOR ARRAY, because that is the only kind of
// evidence the exit test accepts. Wire counters can say a session is healthy
// while nothing in the world has moved: rungs 2 to 8 all passed for weeks
// against a port that spawns exactly one player. What proves multiplayer is a
// SECOND PLAYER ACTOR EXISTING AND MOVING, so this reads:
//
//   data_0209f394[i]   the per-slot Player pointer, written by
//                      _Z19LoadEntranceObjects... as it spawns each one. NULL
//                      here means the slot never spawned, which is the first
//                      thing that would go wrong.
//   +0x6d8  mPlayerNo  the slot the actor believes it is, unpacked by
//                      Player::InitResources from the (i << 6) spawn flag. If
//                      two actors report the same number the spawn packing is
//                      wrong and everything downstream is reading one pad.
//   +0x5c   position   Vector3 of Fix12i, the same field Actor::ClosestPlayer
//                      and CylinderClsn::Process do their distance on.
//   +0x2f8  otherOwner the body cylinder's "who did I touch this frame", which
//                      CylinderClsn::Process writes on a pair hit and
//                      func_ov002_020d869c reads. NON-ZERO IS CONTACT, and it
//                      is what rung 11 asserts on rather than on positions
//                      alone -- two players can be near each other without the
//                      solver having paired them.
//
// Printed for every slot below data_0209f21c whether or not it spawned, so
// "slot 1 is missing" reads as loudly as "slot 1 is here", which is the same
// discipline the TITLE lane's scene-request witness needed.
// ===========================================================================

namespace {
int g_vs_probe = -1;
}

void vs_probe(int frame) {
    if (g_vs_probe < 0) g_vs_probe = std::getenv("SM64DS_VS_PROBE") ? 1 : 0;
    if (!g_vs_probe) return;

    // THE COUNT IS PART OF THE EVIDENCE, not the loop bound only. A probe that
    // silently iterates once because the count is 1 looks exactly like a probe
    // whose second actor failed to spawn, and those are different bugs. Print
    // it, and walk ALL FOUR slots regardless so a live actor in a slot past the
    // count is visible rather than skipped.
    const int n = (int)data_0209f21c_byte();
    std::fprintf(stderr, "[vs] f%d count=%d me=%d live=%d,%d,%d,%d\n", frame, n,
                 (int)data_0209f250_byte(),
                 data_0209fc5c[0], data_0209fc5c[1],
                 data_0209fc5c[2], data_0209fc5c[3]);
    // ONCE: how many entrance records the level actually has.
    // _Z19LoadEntranceObjects... spawns player i from entrance record p3 + i
    // (its `e++` at the bottom of the loop), so a level whose table has no
    // record at p3+1 cannot spawn a second player no matter what the count
    // says -- and the failure looks exactly like a spawn that returned null.
    // Printed once so the two causes are told apart from the log alone.
    static bool said_ent = false;
    if (!said_ent) {
        said_ent = true;
        std::fprintf(stderr,
                     "[vs] entrance records on this level: %d "
                     "(player i spawns from record p3+i)\n",
                     port_entrance_count());
    }
    for (int i = 0; i < kCommsMaxPlayers; ++i) {
        const unsigned char *a = (const unsigned char *)data_0209f394[i];
        if (!a) {
            std::fprintf(stderr, "[vs] f%d slot%d actor=NULL\n", frame, i);
            continue;
        }
        int px = 0, py = 0, pz = 0;
        unsigned other = 0;
        std::memcpy(&px, a + 0x5c, 4);
        std::memcpy(&py, a + 0x60, 4);
        std::memcpy(&pz, a + 0x64, 4);
        std::memcpy(&other, a + 0x2f8, 4);
        // THE INPUT CHAIN, per slot, so a body that will not move can be told
        // apart from a body that is not being TOLD to move. Four links:
        //   pad   data_020a0e58[i]      what func_0203bc7c fanned out
        //   ctrl  data_0209f498 + i*0x18 what Stage::CheckInput made of it
        //   ang   ctrl + 0x0e            the stick angle Player::Behavior reads
        const unsigned pad = (unsigned)(data_020a0e58_arr()[i] & 0xFFFF);
        const unsigned char *ctrl = data_0209f498_bytes() + i * 0x18;
        int cheld = 0, cang = 0;
        std::memcpy(&cheld, ctrl + 0x00, 4);
        std::memcpy(&cang, ctrl + 0x0e, 2);
        std::fprintf(stderr,
                     "[vs] f%d slot%d actor=%p no=%d pos=(%d,%d,%d) touched=%u "
                     "pad=%04x ctrl0=%08x ang=%04x\n",
                     frame, i, (const void *)a, (int)a[0x6d8], px, py, pz,
                     other, pad, (unsigned)cheld, (unsigned)(cang & 0xffff));
    }
}

// ===========================================================================
// 7. HOW MANY PLAYERS THIS LEVEL BOOTS WITH.
//
// hal/level_boot.cpp asks this where it used to write the constant 1, and the
// answer comes off the SEAM rather than off a knob, for a reason that decides
// whether this lane can ship: with no transport installed it MUST answer 1, so
// every single-player baseline in this tree stays valid and the multiplayer
// work costs the solo game nothing. That is the same discipline the seam's own
// solo answers keep, and rung 1 is what checks it.
//
// WITH A TRANSPORT it answers the live count the ROM's own conductor
// negotiated -- player_count() off the installed transport, which the carrier
// derives from its live mask, which src/func_0203ea5c.c's unpack loop is what
// populates. So the number of Player actors the level spawns is decided by how
// many consoles are actually in the session, which is what the DS does.
//
// CLAMPED TO kCommsMaxPlayers AND FLOORED AT 1. The floor matters: a transport
// that answers 0 (installed but not yet connected) would otherwise boot a level
// with no player at all, which faults rather than degrades.
//
// SM64DS_VS_PLAYERS overrides it, and is for proofs only. It exists because
// rungs 9 to 11 need a two-player arena on a single instance to separate "the
// spawn path works" from "the wire works", and debugging both at once is how a
// lane spends a day on the wrong half.
// ===========================================================================

int vs_player_count() {
    if (const char *s = std::getenv("SM64DS_VS_PLAYERS")) {
        const int v = std::atoi(s);
        if (v >= 1 && v <= kCommsMaxPlayers) return v;
        std::fprintf(stderr,
                     "[comms:conductor] SM64DS_VS_PLAYERS=%s out of range "
                     "1..%d; ignored\n", s, (int)kCommsMaxPlayers);
    }
    const CommsTransport *t = comms_transport();
    if (!t) return 1;                       // solo: unchanged, and that is load-bearing
    const int n = t->player_count();
    if (n < 1) return 1;
    return n > kCommsMaxPlayers ? (int)kCommsMaxPlayers : n;
}

// ===========================================================================
// 6. THE TOUCH RING'S WRITE INDEX.
//
// hal/sub_screen.cpp's poll_touch writes one ring entry per frame (see the note
// at its store); these two own the index it writes at. The index lives at
// data_020a80cc[6] as a HALFWORD, which is what src/func_0205edc8.c returns and
// what src/func_0203b9bc.c reads backwards from -- so it is kept in the DS's
// own word rather than in a static here, or the ROM's reader and the port's
// writer would be walking two different rings.
//
// data_020a80cc is hosted by hal/scene_boot.cpp as int[6] = 24 bytes, and index
// 6 as a HALFWORD is byte offset 12, comfortably inside it. Spelled as a
// halfword read at +12 rather than as data_020a80cc[6] on the hosted int array,
// which would be element 6 of 6 and out of bounds.
//
// NINE ENTRIES, not four. func_0203b9bc looks at four (idx-4 .. idx-1) but its
// index fixup is `if (k < 0) k += 9`, and the ROM span of data_020a0df8 is
// 0x48 = 9 * 8. Two independent facts agreeing is why the modulus is 9.
// ===========================================================================

namespace {
enum : int { kTouchRingEntries = 9 };
unsigned short *touch_ring_index_word() {
    return reinterpret_cast<unsigned short *>(data_020a80cc_bytes() + 12);
}
}  // namespace

int touch_ring_index() {
    const int i = (int)*touch_ring_index_word();
    return (i >= 0 && i < kTouchRingEntries) ? i : 0;
}

void touch_ring_advance() {
    unsigned short *w = touch_ring_index_word();
    *w = (unsigned short)((touch_ring_index() + 1) % kTouchRingEntries);
}

// ===========================================================================
// 5. SEATING THE SESSION REQUEST -- and the close of HOLE 3.
//
// MP2's transcription decided the role itself: it read the carrier's env role
// and called become_parent/become_child directly, then wrote data_020a0f04 so
// the rest of the game saw the truth. The real conductor does the opposite. It
// READS data_020a0f04 and dispatches on it (src/func_0203df40.c:42-57), and its
// case-0 arm (src/func_0203ea5c.c:192-198) is what calls become_parent or
// become_child, off that same byte. So with the role byte at 0 the ROM takes
// its solo arm forever and the transport is never asked for anything -- which
// is exactly what the first two-instance run after linking showed: two carriers
// installed, connected=no, role=0, exchanges=0, on both sides.
//
// THAT IS NOT A BUG IN THE ROM'S CODE, IT IS A MISSING MENU. On the DS the role
// byte is seated by the multiplayer menu before the conductor ever runs -- the
// same way src/func_0203db64.c:79 seats it to 2 for a download-played cartridge
// -- and data_02099e1c is the one-shot that menu sets to ask the radio to open
// (:137-140 tests it, calls func_020408b0(2), and clears it). The port has no
// menu on this path, so the launcher mod stands in for one. That is the honest
// division: the mod says "start a session as parent", and every line after that
// is the ROM's.
//
// THIS IS HOLE 3's ANSWER, and the real caller settled it in the direction MP2
// guessed at. The transport MAY be pre-configured with a role -- a loopback
// carrier must be, because the role decides which port it binds -- and the
// ROM's become_parent/become_child is then a REQUEST that agrees with it. What
// MP2 could not know is that the ROM does not ask at all until something seats
// the byte, so "who wins" was never the question. The question is who SEATS it,
// and the answer is: whatever stands in for the menu.
// ===========================================================================

void comms_seat_session_request(int role) {
    data_020a0f04[0] = (unsigned char)role;   // 1 = parent, 2 = child
    data_02099e1c[0] = 1;                     // ask the radio to open, :137-140
    std::fprintf(stderr,
                 "[comms:conductor] seated the session request the DS's "
                 "multiplayer menu would seat: role byte data_020a0f04 = %d "
                 "(%s), open one-shot data_02099e1c = 1. Every decision after "
                 "this one is src/func_0203df40.c's and src/func_0203ea5c.c's.\n",
                 role, role == kCommsRoleParent ? "parent" : "child");
}

// ===========================================================================
// 4. THE PUMP -- and the close of HOLE 1.
//
// comms_seam.h has said since MP1 that `poll` "is called once per pump turn --
// that is, once per DS frame the game is stalled", and MP2 measured that
// NOTHING EVER CALLED IT: `->poll` appeared once in the whole tree, in the null
// check that refuses a transport for omitting it. MP2 proposed two fixes and
// left the choice to this lane.
//
// FIX (a) IS TAKEN, and linking the real conductor is what makes it cheap. The
// ROM's wait loop does not spin: src/func_0203ea5c.c:417 calls func_02042778,
// which veneers to func_0201a4d0, which is OS_SleepThread on the per-VBlank
// queue data_0209d4fc. hal/os_thread.h was written for exactly that call chain
// and names it in its own header. So the "pump turn" the contract describes is
// a real, already-implemented thing, and the seam only has to hang poll() off
// it. MP2 could not take this option because its transcription never reached
// func_02042778 -- it called ::Sleep(1) instead, and said so.
//
// The pump CHAINS whatever was installed rather than replacing it, because
// os_thread's own pump_vblank is what advances the host frame and a transport
// that displaced it would stall the thing it is waiting for.
// ===========================================================================

namespace {

ThreadPump g_prev_pump = nullptr;

// ONE TURN PER SLEEP, AND THE FIRST VERSION OF THIS FUNCTION HUNG THE GAME.
//
// It returned true -- "keep pumping" -- which is the obvious thing for a pump
// to do and is wrong here for a reason worth writing down. OS_SleepThread's
// loop (hal/os_thread.cpp:88-101) ends when the queue word clears, when the
// pump says stop, or when the pump limit is reached. The queue word is
// data_0209d4fc and the only thing that clears it is OS_WakeupThread from
// IRQ::VBlankHandler, which is not running on this path -- so a pump that
// never says stop burns the WHOLE limit, 600 turns, on EVERY sleep. The ROM's
// wait loop takes up to 1200 turns and sleeps once per turn, so the honest
// worst case was 720,000 pump calls inside a single game frame. Measured, not
// reasoned about: the parent sent exactly ONE datagram and then stopped
// reporting, while the child knocked 160 times and resent 158 of them.
//
// os_thread.h's own header predicts the good case ("PUMP INSTALLED -> a real
// wait, one host frame per turn, bounded") and it is right for a pump that
// advances the thing being waited on. This one is not: the thing being waited
// on is ANOTHER PROCESS, and no amount of raising VBlank edges in this one
// makes the peer's datagram arrive sooner. What makes it arrive is wall-clock
// time and a recv, which is exactly the two lines below.
//
// So: service the transport, yield a millisecond, and give the turn back to
// the ROM's own loop -- whose bound (0x4B0 before the session is up, 0x12C
// after) is then the thing that governs the wait, which is what MP2's
// ::Sleep(1) achieved and what the ROM intends. The difference from MP2 is
// that poll() is now genuinely called, once per turn of the ROM's own wait,
// which is the sentence comms_seam.h has always made.
//
// A PREVIOUSLY INSTALLED PUMP KEEPS ITS VOTE. Nothing in the shipped binaries
// installs one today (only tests/mp_sleepwake.cpp does), but if something ever
// does it knows how many turns it needs and this must not overrule it.
bool conductor_pump(unsigned spin) {
    if (const CommsTransport *t = comms_transport())
        t->poll();                       // THE CONTRACT'S OWN SENTENCE, honoured
    if (g_prev_pump) return g_prev_pump(spin);
    ::Sleep(1);                          // wall time, so the peer can answer
    return false;                        // one turn; the ROM's bound governs
}

}  // namespace

void comms_install_pump() {
    static bool done = false;
    if (done) return;
    done = true;
    g_prev_pump = thread_pump();
    thread_set_pump(conductor_pump);
    std::fprintf(stderr,
                 "[comms:conductor] pump installed: the transport's poll() now "
                 "runs once per OS_SleepThread turn inside the ROM's own wait "
                 "(src/func_0203ea5c.c:417 -> func_02042778 -> func_0201a4d0). "
                 "HOLE 1 is closed%s\n",
                 g_prev_pump ? ", chaining the pump that was already there" : "");
}

}  // namespace port
