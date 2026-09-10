// tsc_arm7.cpp -- THE HOST ARM7's TOUCH/SPI DRIVER: PXI CHANNEL 6.
//
// Run link100, lane R2D2, rung R2d of lane BOOTSCOUT's boot plan.
//
// WHAT THIS IS FOR. src/func_0203bbc0.c is the game's touch-panel bring-up and
// the fourth of the ROM's six PXI channels hangs off it. Nine calls, in the
// ROM's own order:
//
//     func_0205f270()                          TP bring-up, claims channel 6
//     func_0205f1e4(&data_020a0de0)            read the TSC calibration
//     func_0205f0e0(&data_020a0de0)            compute the calibration params
//     func_0205edd8(3, 0x1e)                   the sampling setup, two IPC words
//     func_0205ea10(8) / func_0205e9fc(8)      wait on bit 3, then Crash on error
//     func_0205eeac(0, 4, &data_020a0df8, 9)   start auto-sampling into the ring
//     func_0205ea10(2) / func_0205e9fc(2)      wait on bit 1, then Crash on error
//
// Two of those calls SEND on channel 6 and then wait for the other processor to
// answer. Nothing in this port was on the other end, so hal/boot2_ipc.cpp held
// the channel claimed and quiet and hal/boot_os.cpp refused the body. This file
// is the other end: the ARM7's half of the touch/SPI link, in the same shape
// hal/wm_arm7.cpp is the ARM7's half of the radio.
//
// EVERY FIELD BELOW IS READ OFF THE ROM'S OWN BODIES, and each one is named
// where it is used. Nothing here is inherited from a plan.
//
// ---------------------------------------------------------------------------
// 1. THE COMMANDS, AND THE THREE COMPLETIONS
// ---------------------------------------------------------------------------
//
// A command is a PAIR of words, both on channel 6, sent back to back inside one
// IRQ::Disable window:
//
//   src/func_0205edd8.cpp:13  IPCSend(6, a | 0x02000300)  a = 3    -> 0x02000303
//   src/func_0205edd8.cpp:15  IPCSend(6, b | 0x01010000)  b = 0x1e -> 0x0101001e
//   src/func_0205edd8.cpp:26  then sets +0x36 bit 3 and clears +0x34 bit 3.
//
//   src/func_0205eeac.cpp:36  IPCSend(6, (b & 0xff) | 0x02000100) b = 4 -> 0x02000104
//   src/func_0205eeac.cpp:38  IPCSend(6, a | 0x01010000)          a = 0 -> 0x01010000
//   src/func_0205eeac.cpp:49  then sets +0x36 bit 1 and clears +0x34 bit 1.
//
// The first word of a pair carries 0x02 in its top byte and THE BIT in bits
// 8..14; the second carries 0x0101 in its top halfword and the command's own
// parameter. The completion the ARM9 needs back is fixed by five conditions in
// src/func_0205f300.c, the callback src/func_0205f270.c registers:
//
//   a2 == 0                     :40-45 -- a nonzero flag is the no-handler arm,
//                               which SETS +0x34 and never clears +0x36
//   (a1 & 0x7f00) >> 8 == bit   :38    -- the bit the command set
//   a1 & 0x01000000 != 0        :67-68
//   a1 & 0xff == 0              :72-73 -- case 0, the success arm
//   (bit != 0x10)               :47    -- 0x10 is the auto-sample indication
//
// so the completion word is 0x01000000 | (bit << 8), which is 0x01000300 for
// func_0205edd8 and 0x01000100 for func_0205eeac. :93 is the clear of +0x36
// both waits turn on; :87 additionally sets +0x32 = 2 for bit 1. Low bytes 2, 3
// and 4 are ERROR completions (:97-108): they clear +0x36 as well but SET
// +0x34, and func_0203bbc0's own `if (func_0205e9fc(8)) Crash()` then fires.
// Any other low byte reaches :116 and Crash()es inside the callback. So the
// table below is the whole of what this ARM7 may say, and anything else
// arriving on the channel is a HARD FAULT that names the word rather than a
// swallow -- a swallowed channel-6 word is a boot that hangs somewhere else.
//
// ---------------------------------------------------------------------------
// 2. WHY THE REPLY CANNOT BE POSTED FROM arm7_recv, AND WHAT TAKES ITS PLACE
// ---------------------------------------------------------------------------
//
// hal/boot2_ipc.cpp's arm7_recv runs INSIDE the ARM9's store into IPCFIFOSEND
// (ntr/ipc.cpp's fifo_send hands the word straight over), and ntr's
// ipc_arm7_send dispatches the ROM's receive handler on the spot. So a
// completion posted from there would run func_0205f300 BEFORE func_0205edd8
// has reached its `+0x36 |= 8` two lines later: the clear would hit a bit that
// is not set yet, the ROM would then set it, and func_0205ea10(8) would spin on
// it forever. That is hal/boot2_ipc.cpp's "THE WAIT CLOSES EARLY" in its fatal
// form, and it is why hal/wm_arm7.cpp's law 1 exists.
//
// On the DS the completion is an INTERRUPT and it arrives from the other
// processor at the moment func_0205edd8's closing IRQ::Restore re-enables
// interrupts -- after the bit is set, before the wait. This host runs the ARM9
// on one thread, so there is no "meanwhile": the ARM7 has to be given its turn
// from INSIDE the wait. port_tsc_wait below is that turn, and it is the same
// move ntr/ipc.cpp already makes for src/func_0205bad8.c's IPCSYNC handshake,
// where the ARM9's own echoing store is what steps the ARM7's loop.
//
// port_tsc_wait is reached by a PER-SOURCE RENAME in port/CMakeLists.txt --
// src/func_0203bbc0.c is compiled with `func_0205ea10=port_tsc_wait` -- which
// is hal/boot_arms.cpp's own precedent for func_02060890 and lane NITROFS's for
// func_0205db2c. src/func_0205ea10.c is NOT displaced by that: it stays in the
// link under its own name for its other caller, src/func_0205efc0.c, and it is
// not a host body under a ROM name, so port/tools/linkage.py gains no SHADOW
// row. What the rename says, in one place, is that a two-processor spin is the
// one thing a single-threaded host cannot serve verbatim.
//
// ---------------------------------------------------------------------------
// 3. THE CALIBRATION, WHICH IS NOT OPTIONAL AND IS NOT THE GAME'S
// ---------------------------------------------------------------------------
//
// src/func_0205f1e4.c reads the touch calibration out of the FIRMWARE USER
// SETTINGS at 0x027ffc80 + 0x58..0x63 -- raw x1/y1, dot x1/y1, raw x2/y2, dot
// x2/y2 -- and returns 0 without calling func_0205eb58 when all four raw words
// are zero. So on a zeroed shared block the ROM declines to calibrate rather
// than failing, and the rung's LINKAGE does not need this.
//
// ITS BEHAVIOUR DOES, and this is a real trap rather than a nicety. Whatever
// func_0205f1e4 decides, func_0203bbc0's next call func_0205f0e0 runs, and its
// last line is `data_020a80cc.field_30 = 1` -- byte +0x30 of the work struct,
// which is exactly the halfword src/func_0205ea28.c reads as data_020a80cc[0x18]
// to decide whether the calibration transform is LIVE. func_0205ea28 is linked,
// and src/func_0203b9bc.c calls it on the per-frame stylus path. With a zero
// calibration func_0205f0e0's two divides are 0x10000000 / 0, ntr/io.cpp models
// the DS unit's answer (quotient -1 on a zero divisor, GBATEK), and
// func_0205ea28 then computes (-1 * (x << 2)) >> 22, which is negative for every
// x and clamps to 0. Seating this rung with an uncalibrated panel would put
// EVERY TAP AT (0,0). The stylus is not broken today only because +0x30 reads
// zero and func_0205ea28 takes its identity-copy arm.
//
// So the firmware user settings are published here, by the ARM7, at power-on --
// which is where they come from on hardware: the ARM7 copies the console's own
// user settings into the shared block before the ARM9's code runs. This is a
// CONSOLE MODEL, the same class as ntr/ipc.cpp standing in for the second
// processor; it is not ROM code, it raises no count, and no ROM body is changed
// or skipped by it.
//
// THE VALUES ARE DERIVED, NOT PICKED. This port's touch source publishes DS
// PIXELS (hal/sub_screen.cpp's poll_touch runs the present rectangle backwards
// to get them), so the panel's "raw" reading and its "dot" reading are the same
// number, and the transform the ROM computes from them has to come out as the
// identity. Running func_0205eb58's own arithmetic backwards:
//
//   xDotSize = ((raw_x1 - raw_x2) << 8) / (dx1 - dx2)      :65,:70,:78
//   x0       = (((raw_x1 + raw_x2) << 8) - (dx1 + dx2) * xDotSize) >> 7   :79
//   func_0205f0e0 then stores x0 at +0x18 and 0x10000000 / xDotSize at +0x20
//   func_0205ea28 computes  x' = (p[2] * ((x << 2) - p[0])) >> 22          :22
//
// x' == x for every x needs p[0] == 0 and p[2] == 0x10000000 / 256, i.e.
// xDotSize == 256 and x0 == 0, i.e. raw_x1 == dx1 and raw_x2 == dx2. The two
// calibration points below are the DS's own crosshair positions, (32,32) and
// (224,176), which also satisfy func_0205eb58's four refusals (:45 raw < 0x1000,
// :50 dx < 256 and dy < 192, :55 the four not-equal tests). The values are
// asserted after the ROM has computed from them rather than assumed: see
// port_tsc_report.
//
// SM64DS_TP_CALIB=0 publishes nothing, which is the pre-rung shared block, so a
// run can measure the uncalibrated path on purpose. It is not the default
// because the default would be a broken stylus.
//
// ---------------------------------------------------------------------------
// 4. THE AUTO-SAMPLE RING, THE ROM'S OWN CADENCE, AND WHY IT IS ON BY DEFAULT
// ---------------------------------------------------------------------------
//
// (Run link100, lane R2D3, continuing rung R2d. SM64DS_TP_RING was OFF by
// default when R2D2 left it -- the finding below is what this lane changed.)
//
// The third completion is the auto-sample indication: once per ARM7 sampling
// period, a word with (a1 & 0x7f00) >> 8 == 0x10 and a2 == 0. On it
// src/func_0205f300.c:47-64 advances the ring cursor at +0xc, wraps at the count
// at +0x14, reads the packed sample out of the shared words 0x027fffaa (low
// half) and 0x027fffac (high half) -- x 12 bits, y 12 bits, then valid 1 and
// range 2 -- and writes one eight-byte entry into the ring func_0203bbc0 handed
// it, &data_020a0df8 with a count of 9.
//
// THE CADENCE. src/func_0203bbc0.c's third call is
// src/func_0205edd8.cpp(3, 0x1e), which IPCSends 0x02000303 then 0x0101001e --
// "the sampling setup" by the ROM's own naming (section 1's table). That
// 0x1e is the ONE number the ROM hands the ARM7 for how often to sample, and
// arm7.bin is not decompiled anywhere in this project (section header above),
// so what the ARM7 does with it is not a fact this repo can read off a body.
// What IS measurable is the shape every reader downstream already assumes:
// src/func_0203b9bc.c's three-consecutive-samples debounce (section header,
// "left alone" note) is written against ONE ring entry per displayed frame --
// three entries is three frames of holding a tap, not three deliveries inside
// one -- and hal/sub_screen.cpp's poll_touch was already the port's stand-in
// at exactly that cadence, one store per call, and poll_touch is called
// exactly once per frame (hal/scene_boot.cpp's frame loop, both the level path
// and the scene path -- see its "THE BOTTOM SCREEN" note). So the cadence this
// lane implements and measures is the DS's own displayed-frame rate, ~59.8 Hz,
// sourced from where poll_touch already ran rather than from guessing at what
// 0x1e counts on the ARM7 side.
//
// PORT_TSC_ARM7_FRAME_TOUCH is the new entry point that cadence rides on.
// hal/sub_screen.cpp's poll_touch calls it once per frame, in the same place
// its own ring store used to be (see the note over that call there); it arms
// a one-shot latch and immediately gives the ARM7 the turn that finds it. A
// second turn landing in the SAME frame -- port_arm7_wireless_tick calls
// port_tsc_arm7_tick on every turn the wireless conductor takes, including
// several inside one multiplayer wait -- finds the latch already spent and
// posts nothing, so the multiplayer path cannot double the ROM's own cadence
// even though it asks for more turns than the touch panel needs. See
// port_tsc_arm7_tick and port_tsc_arm7_frame_touch below for the mechanism;
// hal/boot2_ipc.cpp's call site and comment are unchanged by this lane except
// for the census wording.
//
// THE HOST'S OWN INPUTS STILL REACH THE ROM'S WRITER, THROUGH THE SAMPLE, NOT
// THE RING. poll_touch resolves the mouse-to-stylus mapping, the drag latch,
// SM64DS_SKIP_MENU's forced tap and SM64DS_COMMS_INJECT's injected tap exactly
// as before, and stores the result into TouchInfo slot 0 (data_020a0de8) the
// way it always has (see the store two paragraphs above this ring note in the
// function body). port_tsc_arm7_frame_touch runs AFTER that store, and the
// ARM7's own sample -- the packed word this file writes to 0x027fffaa/ac --
// is read straight out of data_020a0de8[0]/[2]/[3]. So a mouse click, a
// SKIP_MENU tap or an injected comms tap all cross into the ROM exactly the
// way a real stylus press would: through the ARM7's shared sample words and
// src/func_0205f300.c's own read of them, never by this port reaching into
// the ring directly. hal/sub_screen.cpp's store is retired ONLY when
// port_tsc_ring_armed() is true; SM64DS_TP_RING=0 puts the pre-rung direct
// write back, unchanged, for measurement.
//
// ON BY DEFAULT as of this lane. SM64DS_TP_RING=0 is still read, and is what a
// run measuring the pre-rung shape (or isolating a regression to this rung)
// sets.
//
// Everything else on this channel is on by default, because everything else is
// the boot the port did not have.

#include "ntr/ipc.h"
#include "dsstate_seg.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {

// data_020a80c8 -- the "the touch panel is already up" halfword
// src/func_0205f270.c:12 guards on and :13 sets. Four bytes by ROM span
// (0x020a80c8 -> data_020a80cc), and nothing else in this port spells it, so
// the channel's own driver hosts it. Real DS state, so inside the .dsstate
// capture a save state rolls back; ".dsstate$tp0000" puts it immediately before
// hal/scene_boot.cpp's ".dsstate$tp0001" data_020a80cc, which is the ROM's own
// order. Nothing reads across the boundary -- func_0205f270 touches it as a
// halfword at +0 and no further -- so the adjacency is faithfulness, not a
// layout the code depends on.
#pragma section(".dsstate$tp0000", read, write)
__declspec(allocate(".dsstate$tp0000")) __declspec(align(4))
unsigned char data_020a80c8[4] = {0};

// The 0x38 work struct, hosted by hal/scene_boot.cpp as ONE ordered run.
// hal/boot2_ipc.cpp's port_tp_layout_check reads the 0x18 boundary back before
// it claims the channel; this file addresses the struct by byte offset off the
// first name, which is what every ROM body that reaches past +0x18 does.
extern unsigned char data_020a80cc[];
extern unsigned char data_020a80e4[];

}  // extern "C"

// ---------------------------------------------------------------------------
// ONE ADDRESS, TWO SPELLINGS -- the C++ TUs of this rung, and why the answer is
// a linker alias rather than an edit anywhere.
//
// src/func_0205edd8.cpp and src/func_0205eeac.cpp are the two //cpp TUs here,
// and each declares the work struct as `extern S data_020a80cc;` at C++ file
// scope with its own local `struct S`. MSVC therefore emits
// ?data_020a80cc@@3US@@A -- the same mangling from both, because the mangling
// carries the TAG and not the members -- while every other reader of the same
// object spells it with C linkage: src/func_0205f300.c and src/func_0205e9fc.c
// are .c, and hostgen wraps src/func_0205f0e0.c in extern "C".
//
// THE BYTE GATE CANNOT SEE THIS. dsd's symbol table carries an address, and
// mwccarm resolved both spellings to the same 0x020a80cc at link time, so the
// decomp is right and byte-matched either way; it is only the host link that
// has two names for one object. This is the tree's alias-width class and
// hal/cxx_aliases.cpp is 2471 directives of it; hal/wm_arm7.cpp carries its own
// two for the same reason a driver keeps its own. So does this one. No new
// code, no new definition, no forwarding body: the same storage under both
// names, and if hal/scene_boot.cpp ever stopped defining it the alias would
// resolve to nothing and the link would say so.
// ---------------------------------------------------------------------------
#pragma comment(linker, "/alternatename:?data_020a80cc@@3US@@A=_data_020a80cc")

extern "C" {

// TouchInfo[4], hal/auto_bss.cpp. Slot 0 is what hal/sub_screen.cpp's poll_touch
// publishes: +0 touched, +1 the edge, +2 x, +3 y. Read here ONLY by the
// auto-sample indication, to fill the shared words the ROM's own callback takes
// its sample from.
extern unsigned char data_020a0de8[];

}  // extern "C"

namespace {

// The ARM9's status halfwords, by the offsets the ROM's own bodies use.
//   +0x0c  the ring cursor        src/func_0205edc8.c returns it
//   +0x10  the ring pointer       src/func_0205eeac.cpp:28 sets it
//   +0x14  the ring count         src/func_0205eeac.cpp:31 sets it
//   +0x30  "the calibration is live"   src/func_0205f0e0.c:51 sets it
//   +0x32  the pen state          src/func_0205f300.c:87 sets it to 2
//   +0x34  the error bits         src/func_0205e9fc.c reads it
//   +0x36  the pending bits       src/func_0205ea10.c spins on it
volatile unsigned short *tp16(unsigned off) {
    return reinterpret_cast<volatile unsigned short *>(data_020a80cc + off);
}
const int *tp_calib_words() {
    return reinterpret_cast<const int *>(data_020a80e4);
}

bool env_on(const char *name, bool dflt) {
    const char *v = std::getenv(name);
    if (!v || !v[0]) return dflt;
    return v[0] != '0';
}

// THE TABLE. Two rows, and they are the only two commands the ROM sends on this
// channel; the derivation of every field is in section 1 of the header.
struct Command {
    unsigned int  first;    // the 0x02...... word
    unsigned int  second;   // the 0x0101.... word
    unsigned int  reply;    // 0x01000000 | (bit << 8), the success completion
    unsigned int  bit;      // the +0x36 / +0x34 bit the pair turns on
    const char   *what;
};
const Command kCommands[] = {
    { 0x02000303u, 0x0101001eu, 0x01000300u, 3u,
      "src/func_0205edd8.cpp(3, 0x1e), the sampling setup" },
    { 0x02000104u, 0x01010000u, 0x01000100u, 1u,
      "src/func_0205eeac.cpp(0, 4, &data_020a0df8, 9), auto-sampling start" },
};
const unsigned kCommandCount = sizeof kCommands / sizeof kCommands[0];

// The auto-sample indication. bit 0x10, low byte 0, and the 0x01 top byte the
// other two completions carry -- src/func_0205f300.c:47 returns before it ever
// looks at the low byte or the 0x01000000 marker for this bit, so the shape is
// taken from its siblings rather than invented.
const unsigned int kSampleWord = 0x01001000u;

enum : unsigned { kQueueMax = 8 };
unsigned int g_q[kQueueMax];
unsigned     g_qn;

unsigned int g_first;          // the latched first word of a pair
bool         g_have_first;

unsigned long g_words;         // channel-6 words from the ARM9
unsigned long g_commands;      // complete pairs
unsigned long g_replies;       // completions posted
unsigned long g_samples;       // auto-sample indications posted
unsigned long g_turns;         // ARM7 turns taken on this channel
unsigned long g_waits;         // port_tsc_wait calls
unsigned long g_wait_turns;    // turns spent inside a wait
bool          g_wait_gave_up;  // the bounded exit fired at least once

void fault(const char *why, unsigned int word)
{
    std::fprintf(stderr,
        "[tsc] HARD FAULT on PXI channel 6: %s (word 0x%08x, %lu word(s) seen, "
        "%lu command(s), %lu reply(ies)). The ROM's own senders are only "
        "src/func_0205edd8.cpp and src/func_0205eeac.cpp; a word this driver "
        "does not know is a body it has never met, and swallowing it would "
        "hang the boot somewhere else.\n",
        why, word, g_words, g_commands, g_replies);
    std::fflush(stderr);
    std::_Exit(24);
}

void queue(unsigned int reply)
{
    if (g_qn >= kQueueMax) fault("the completion queue is full", reply);
    g_q[g_qn++] = reply;
}

// THE DS'S OWN CALIBRATION CROSSHAIRS, published into the firmware user
// settings block. See section 3: raw == dot is what makes the ROM's transform
// the identity on a port whose touch source already speaks DS pixels.
enum : unsigned {
    kCalRawX1 = 32,  kCalRawY1 = 32,  kCalDotX1 = 32,  kCalDotY1 = 32,
    kCalRawX2 = 224, kCalRawY2 = 176, kCalDotX2 = 224, kCalDotY2 = 176,
};
unsigned char *user_settings() {
    return reinterpret_cast<unsigned char *>(static_cast<uintptr_t>(0x027ffc80u));
}
void put16(unsigned char *p, unsigned v) {
    const unsigned short h = static_cast<unsigned short>(v);
    std::memcpy(p, &h, 2);
}

bool g_calib_published;

// THE ONE-SHOT LATCH port_tsc_arm7_frame_touch arms and port_tsc_arm7_tick
// consumes. See section 4: it is what keeps the ring to one write per frame
// even though the wireless conductor can hand this ARM7 several turns inside
// one host frame.
bool g_sample_due;

}  // namespace

extern "C" {

// ---------------------------------------------------------------------------
// POWER-ON. hal/boot2_ipc.cpp's arm7_power_on calls this, at ntr::io_init time,
// before any ARM9 code runs -- which is when the ARM7 has finished its own boot
// on hardware and has already put the console's user settings in the shared
// block.
// ---------------------------------------------------------------------------
void port_tsc_arm7_power_on(void)
{
    if (!env_on("SM64DS_TP_CALIB", true)) {
        std::fprintf(stderr, "[tsc] SM64DS_TP_CALIB=0: the firmware touch "
                     "calibration is NOT published, so src/func_0205f1e4.c will "
                     "decline on the zeroed raw words and src/func_0205ea28.c "
                     "will run on a zero scale. Measurement mode.\n");
        return;
    }
    unsigned char *u = user_settings();
    put16(u + 0x58, kCalRawX1);
    put16(u + 0x5a, kCalRawY1);
    u[0x5c] = (unsigned char)kCalDotX1;
    u[0x5d] = (unsigned char)kCalDotY1;
    put16(u + 0x5e, kCalRawX2);
    put16(u + 0x60, kCalRawY2);
    u[0x62] = (unsigned char)kCalDotX2;
    u[0x63] = (unsigned char)kCalDotY2;
    g_calib_published = true;
    std::fprintf(stderr,
        "[tsc] firmware touch calibration published at 0x027ffc80+0x58: raw "
        "(%u,%u)/(%u,%u) dot (%u,%u)/(%u,%u). raw == dot, so the transform "
        "src/func_0205eb58.c computes is the identity on this port's DS-pixel "
        "samples.\n",
        kCalRawX1, kCalRawY1, kCalRawX2, kCalRawY2,
        kCalDotX1, kCalDotY1, kCalDotX2, kCalDotY2);
}

// ---------------------------------------------------------------------------
// A WORD ARRIVES. hal/boot2_ipc.cpp's arm7_recv hands tag 6 straight here,
// INSIDE the ARM9's store into IPCFIFOSEND. NOTHING IS POSTED FROM HERE: see
// section 2.
// ---------------------------------------------------------------------------
void port_tsc_arm7_command(unsigned int word)
{
    const unsigned int d = ntr::ipc_data(word);
    ++g_words;

    if (!g_have_first) {
        for (unsigned i = 0; i < kCommandCount; ++i)
            if (kCommands[i].first == d) {
                g_first = d;
                g_have_first = true;
                if (ntr::ipc_log_on())
                    std::fprintf(stderr, "[tsc] command word 1 of 2: 0x%08x "
                                 "(%s)\n", d, kCommands[i].what);
                return;
            }
        fault("this is not the first word of either command the ROM sends", d);
        return;
    }

    for (unsigned i = 0; i < kCommandCount; ++i) {
        if (kCommands[i].first != g_first || kCommands[i].second != d) continue;
        g_have_first = false;
        ++g_commands;
        queue(kCommands[i].reply);
        if (ntr::ipc_log_on())
            std::fprintf(stderr,
                "[tsc] command 0x%08x + 0x%08x -- %s. Completion 0x%08x (bit "
                "%u) QUEUED; it is posted from a turn, never from here.\n",
                kCommands[i].first, d, kCommands[i].what, kCommands[i].reply,
                kCommands[i].bit);
        return;
    }
    fault("this is not the second word of the command that opened", d);
}

// ---------------------------------------------------------------------------
// THE ARM7'S TURN. One completion, in arrival order. Returns nonzero when it
// posted something.
// ---------------------------------------------------------------------------
int port_tsc_arm7_turn(void)
{
    ++g_turns;
    if (!g_qn) return 0;
    const unsigned int reply = g_q[0];
    for (unsigned i = 1; i < g_qn; ++i) g_q[i - 1] = g_q[i];
    --g_qn;
    ++g_replies;
    if (ntr::ipc_log_on())
        std::fprintf(stderr, "[tsc] completion 0x%08x -> the ARM9\n", reply);
    ntr::ipc_arm7_send(ntr::ipc_word(6, reply, 0));
    return 1;
}

// ---------------------------------------------------------------------------
// THE WAIT. src/func_0203bbc0.c is compiled with func_0205ea10=port_tsc_wait;
// see section 2 for why, and for why src/func_0205ea10.c is not displaced.
//
// The body is src/func_0205ea10.c's own test -- `while (*(volatile u16 *)
// (data_020a80cc + 0x36) & mask)` -- with the other processor's turn inside the
// loop instead of outside it.
// ---------------------------------------------------------------------------
void port_tsc_wait(int mask)
{
    enum : unsigned long { kSpinCap = 64ul };
    ++g_waits;
    for (unsigned long i = 0; i < kSpinCap; ++i) {
        if ((*tp16(0x36) & (unsigned)mask) == 0) return;   // the ROM's own exit
        ++g_wait_turns;
        if (!port_tsc_arm7_turn()) break;                  // nothing left to say
    }
    if ((*tp16(0x36) & (unsigned)mask) == 0) return;
    g_wait_gave_up = true;
    std::fprintf(stderr,
        "[tsc] BOUNDED WAIT GAVE UP: src/func_0205ea10.c's spin on +0x36 bit "
        "mask 0x%x is still set after %lu turn(s) and the completion queue is "
        "empty. +0x34 reads 0x%04x, +0x36 reads 0x%04x. The ARM7 was asked for "
        "something this driver does not answer; the boot continues rather than "
        "hanging, and this line is the measurement.\n",
        (unsigned)mask, kSpinCap, (unsigned)*tp16(0x34), (unsigned)*tp16(0x36));
    std::fflush(stderr);
}

// ---------------------------------------------------------------------------
// THE AUTO-SAMPLE INDICATION, off by default. See section 4.
// ---------------------------------------------------------------------------
int port_tsc_ring_armed(void)
{
    static int armed = -1;
    if (armed < 0) armed = env_on("SM64DS_TP_RING", true) ? 1 : 0;
    return armed;
}

void port_tsc_arm7_tick(void)
{
    // Drain anything a turn owes even when the ring is not armed: a completion
    // that never leaves the queue is a swallowed word by another name.
    if (g_qn) { port_tsc_arm7_turn(); return; }
    if (!port_tsc_ring_armed()) return;
    if (!g_sample_due) return;         // this frame's one indication already
                                        // went out -- see section 4
    if (*tp16(0x14) == 0) return;      // func_0205eeac has not run yet
    g_sample_due = false;

    // The packed record src/func_0205f300.c:56-61 reads: x 12 bits, y 12 bits,
    // then valid 1 and range 2, low half at 0x027fffaa and high half at
    // 0x027fffac. The sample is this port's own stylus, off TouchInfo slot 0 --
    // which is DS pixels, which is what the published calibration makes the
    // ROM's transform an identity on.
    const unsigned down = data_020a0de8[0] ? 1u : 0u;
    const unsigned x = data_020a0de8[2];
    const unsigned y = data_020a0de8[3];
    const unsigned packed = (x & 0xfffu) | ((y & 0xfffu) << 12) | (down << 24);
    *reinterpret_cast<volatile unsigned short *>(
        static_cast<uintptr_t>(0x027fffaau)) = (unsigned short)(packed & 0xffffu);
    *reinterpret_cast<volatile unsigned short *>(
        static_cast<uintptr_t>(0x027fffacu)) = (unsigned short)(packed >> 16);
    ++g_samples;
    ntr::ipc_arm7_send(ntr::ipc_word(6, kSampleWord, 0));
}

// ---------------------------------------------------------------------------
// ONE PER FRAME. hal/sub_screen.cpp's poll_touch calls this once per frame,
// immediately after it has written TouchInfo (data_020a0de8) -- see section 4.
// Arming the latch and taking the turn in the same call means the common case
// (no other turn lands this frame) posts the indication right here, at
// exactly poll_touch's old cadence; if the wireless conductor also turns this
// ARM7 later in the same frame (comms_arm7_turn, which a multiplayer wait can
// call several times), port_tsc_arm7_tick above finds the latch already spent
// and does nothing, so the ring still gets one write.
// ---------------------------------------------------------------------------
void port_tsc_arm7_frame_touch(void)
{
    if (!port_tsc_ring_armed()) return;
    g_sample_due = true;
    port_tsc_arm7_tick();
}

// THE WATCHER'S ROM-SIDE COUNT. g_samples is incremented exactly where this
// file hands the ARM9 the auto-sample indication, and that send is synchronous
// (ntr::ipc_arm7_send -> raise_rx_irq -> the ARM9's registered rx handler ->
// src/func_0205f300.c, all inside the one call), so this count IS the number
// of times the ROM's own writer has stored into the ring -- not an estimate of
// it. hal/sub_screen.cpp's poll_touch reads it, under SM64DS_TP_RING_WATCH, to
// print the two writers' counts side by side (run link100, lane R2D3).
unsigned long port_tsc_arm7_sample_count(void) { return g_samples; }

// ---------------------------------------------------------------------------
// THE REPORT the R2d arm prints straight after func_0203bbc0 returns, and the
// census hal/boot2_ipc.cpp's exit report takes beside its per-tag tally.
// ---------------------------------------------------------------------------
void port_tsc_report(void)
{
    const int *p = tp_calib_words();
    std::fprintf(stderr,
        "  [rom-a054] R2d func_0203bbc0: the ROM's own touch-panel bring-up ran. "
        "channel 6 answered %lu command(s) with %lu completion(s) in %lu turn(s); "
        "+0x34 (errors) 0x%04x, +0x36 (pending) 0x%04x, +0x30 (calibration live) "
        "%u, +0x32 (pen) %u; ring at +0x10 with %u entries, cursor %u\n",
        g_commands, g_replies, g_wait_turns,
        (unsigned)*tp16(0x34), (unsigned)*tp16(0x36), (unsigned)*tp16(0x30),
        (unsigned)*tp16(0x32), (unsigned)*tp16(0x14), (unsigned)*tp16(0x0c));
    std::fprintf(stderr,
        "  [rom-a054] R2d calibration: %s. src/func_0205f0e0.c wrote "
        "data_020a80e4 = { x0 %d, xDotSize %d, 0x10000000/xDotSize %d, y0 %d, "
        "yDotSize %d, 0x10000000/yDotSize %d }, so the transform "
        "src/func_0205ea28.c runs is %s.\n",
        g_calib_published ? "published by the ARM7 and computed from the "
                            "firmware words by src/func_0205eb58.c"
                          : "NOT published (SM64DS_TP_CALIB=0)",
        p[0], p[1], p[2], p[3], p[4], p[5],
        (p[0] == 0 && p[3] == 0 && p[2] == 0x100000 && p[5] == 0x100000)
            ? "the IDENTITY -- a DS-pixel sample crosses it unchanged"
            : "NOT the identity: every tap is scaled by it");
    if (g_wait_gave_up)
        std::fprintf(stderr, "  [rom-a054] R2d NOTE: a bounded wait gave up; "
                             "see the [tsc] line above it.\n");
    std::fflush(stderr);
}

void port_tsc_arm7_census(void)
{
    std::fprintf(stderr,
        "[tsc:census] channel 6: %lu word(s) in, %lu command(s), %lu "
        "completion(s) out, %lu auto-sample indication(s), %lu turn(s), %lu "
        "wait(s)%s\n",
        g_words, g_commands, g_replies, g_samples, g_turns, g_waits,
        g_wait_gave_up ? ", A WAIT GAVE UP" : "");
    std::fprintf(stderr,
        "[tsc:census] ring %s; +0x30 %u, +0x32 %u, +0x34 %u, +0x36 %u, cursor "
        "%u of %u\n",
        port_tsc_ring_armed() ? "ARMED (SM64DS_TP_RING default-on): this ARM7 "
                                "is the ring's writer, once per frame off "
                                "hal/sub_screen.cpp's poll_touch" : "not armed "
                                "(SM64DS_TP_RING=0): hal/sub_screen.cpp's "
                                "poll_touch writes the ring directly, as it "
                                "did before this rung",
        (unsigned)*tp16(0x30), (unsigned)*tp16(0x32), (unsigned)*tp16(0x34),
        (unsigned)*tp16(0x36), (unsigned)*tp16(0x0c), (unsigned)*tp16(0x14));
    std::fflush(stderr);
}

}  // extern "C"
