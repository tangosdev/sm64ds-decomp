// THE HOST ARM7, and the bss the ROM's PXI layer keeps.
//
// NONE OF THIS IS DECOMPILED CODE. The DS has two processors; the port runs
// one. Everything below stands in for the other one, the same way ntr/gx.cpp
// stands in for the geometry engine and hal/sdat/consumer.cpp stands in for
// the ARM7's sound driver. Nothing here is tagged PORT_HOST_ABI: that tag is a
// ruling about a matched body, and there is no matched body in this file.
//
// WHAT IT ANSWERS, and the ROM bodies that say so:
//
//   THE HANDSHAKE.  src/func_0205bad8.c runs a nine-round exchange over
//   IPCSYNC. ntr/ipc.cpp owns that, off the ARM7's own loop read out of
//   extracted/arm7.bin (see ntr/ipc.h).
//
//   THE READINESS WORD.  src/func_0205ba3c.c reads word 1 of the array at
//   0x027FFC00+0x388 and four matched initialisers spin on it:
//       src/func_0205f270.c   channel 6    registers func_0205f300
//       src/func_0205fde8.c   channel 8    registers func_0205fcfc
//       src/func_02059e48.c   channel 0xc  registers func_02059e04
//       src/func_0206a88c.c   channel 0xd  registers func_0206a694/func_0206a634
//   plus src/func_0205ae64.c on channel 7 (the sound command channel, whose
//   ARM9 half is already linked out of port/slice_gate10.txt) and
//   src/func_020616e8.c on channel 0xa (wireless). The ARM7 claims all six at
//   power-on, which is what the DS's ARM7 has done before the ARM9's code
//   runs at all.
//
//   THE COMMAND CHANNELS.  Every word the ARM9 pushes is decoded and logged,
//   and the table below says, per channel, whether this ARM7 answers it.
//   CHANNEL 0xA IS ANSWERED as of run link100 lane WM3 (rung W4): the ROM's own
//   become-parent and become-child bodies are linked and they send WM commands,
//   and hal/wm_arm7.cpp is the driver behind the channel. It was held
//   observed-only for as long as nothing asked it anything, which was the whole
//   life of this port until that rung.
//   Three are held and OBSERVED ONLY, each for a reason:
//     7    hal/sdat/consumer.cpp is already the port's ARM7 sound driver and
//          it is driven from func_0205b5d4. Consuming the batch here as well
//          would run it twice.
//     6    the ARM9 half is not enrolled (see arm9_bring_up below).
//     0xd  likewise.
//   The two it does answer are the two whose ARM9 halves this lane enrolled.
//   A channel it holds with no driver behind gets the SDK's own reply: the
//   word back with the no-handler flag set, which is the exact shape
//   src/_ZN3IRQ24IPCRxFifoNotEmptyHandlerEv.c uses in the other direction. A
//   channel NOBODY holds is reported loudly and refused the same way.
//
// THE ONE SEAM. link_up() below runs the ROM's own PXI bring-up. On the DS
// that is src/func_02058c84.c -- its 2nd, 10th and 13th calls -- and on a
// build with a pre-main spine it should come from there and nowhere else.
// This build has no such spine (lane BOOT of run link100 is bringing one in),
// so the calls are made from the moment the hardware model comes up, in the
// ROM's own order, and the block is marked so it can be deleted in one piece.

#include "ntr/ipc.h"
#include "comms_seam.h"
#include "dsstate_seg.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------------------
// THE ROM's PXI BSS
//
// Sized by ROM SPAN out of config/arm9/symbols.txt -- the distance to the next
// bss symbol -- not by the widest field any one body happens to touch, because
// several of these blocks are addressed past their own next symbol.
//
// ALL OF IT IS INSIDE THE SAVE-STATE BRACKET, and tools/dsstate_guard.py said
// so before this file ever ran: every one of these is REAL DS STATE, not host
// bookkeeping. The receive-callback table alone decides where an incoming FIFO
// word goes, and a restore that left it holding the pre-save pointers while the
// rest of the world rolled back is the class of bug hal/dsstate_seg.h exists
// for. The host ARM7's own counters below are NOT in here: they are the model's
// bookkeeping, and a save state has no business rolling them back.
// ---------------------------------------------------------------------------

extern "C" {
DSSTATE_BEGIN

// 0x020a6458 .. 0x020a6460. func_02059e48's "already initialised" halfword and
// the soft-reset acknowledge flag src/func_02059e04.c sets.
unsigned char data_020a6458[4];
unsigned char data_020a645c[4];

// 0x020a7fc4 .. 0x020a8048. The PXI layer's own state: the init guard and the
// 32-entry receive-callback table src/func_0205ba64.c writes and
// src/_ZN3IRQ24IPCRxFifoNotEmptyHandlerEv.c indexes. 0x80 bytes is exactly the
// 0x20 entries func_0205bad8 zeroes.
unsigned short data_020a7fc4;
void          *data_020a7fc8[0x20];

// 0x020a8104 .. 0x020a8160. The channel-8 driver's state. data_020a8114 is
// NOT here: hal/player_bridges.cpp already hosts it as int[4], which is its
// 0x10 ROM span exactly, and every field src/func_0205fcfc.c and
// src/func_0205feac.c reach (+0, +4, +8, +0xc) is inside it.
unsigned short data_020a8104;
int            data_020a8108;
int            data_020a810c;
int            data_020a8110;
unsigned char  data_020a8124[0x14];

// 0x020a8138 and 0x020a813c ARE ONE TABLE ON THE DS: four 8-byte entries from
// 0x020a8138, with 0x020a8138 naming each entry's flag halfword and 0x020a813c
// naming its pointer, four bytes on. src/func_0205fde8.c strides the first by
// 8 (`(char *)&data_020a8138 + (i << 3)`) and src/func_0205fcfc.c strides the
// second by 8, and NEITHER takes the distance between the two names -- so two
// parallel 0x20-byte arrays are indistinguishable here from the interleaved
// table, and are what this hosts. If a future body ever computes one name from
// the other, this is the line that has to become a single backing block.
unsigned char data_020a8138[0x20];
unsigned char data_020a813c[0x20];

DSSTATE_END

// Rung W1's mode word, hosted at the foot of hal/comms_seam.cpp. Declared as
// its ROM span; src/func_020408b0.c writes its low halfword.
extern unsigned char data_020a0f14[];
// THE TOUCH-PANEL WORK STRUCT, hosted by hal/scene_boot.cpp. ONE 0x38 ROM
// object under two dsd names, laid out there as an ordered .dsstate$tp run;
// the read-back in port_tp_layout_check below is what makes that a
// measurement rather than a hope, and channel 6 is not claimed without it.
extern unsigned char data_020a80cc[];
extern unsigned char data_020a80e4[];
// THE HOSTED TOUCH/SPI DRIVER, hal/tsc_arm7.cpp. Run link100, lane R2D2, rung
// R2d: src/func_0203bbc0.c and the eight TUs behind it are linked now, and two
// of them SEND on channel 6, so the channel stops being observed-only. The
// command is handed over from arm7_recv and the completion is posted from a
// TURN, never from there -- the same law hal/wm_arm7.cpp opens with, and on
// this channel it is fatal rather than survivable: src/func_0205edd8.cpp sets
// its +0x36 bit AFTER its two sends, so a completion delivered inline would
// clear a bit that is not set yet and src/func_0205ea10.c would spin on it
// forever. hal/tsc_arm7.cpp's section 2 is that derivation in full.
void port_tsc_arm7_power_on(void);
void port_tsc_arm7_command(unsigned int word);
int  port_tsc_arm7_turn(void);
void port_tsc_arm7_tick(void);
void port_tsc_arm7_census(void);
int  port_tsc_ring_armed(void);
// hal/comms_seam.cpp's read-back of the data_020a89ec/data_020a8a00 pair.
int port_wm_message_layout_check(void);
// THE HOSTED WM DRIVER, hal/wm_arm7.cpp. Run link100, lane WM3, rung W4: the
// ROM's own become-parent and become-child bodies are linked now and they SEND,
// so channel 0xa stops being observed-only. The command is handed over from
// arm7_recv; the reply is posted from a TURN and never from there. That is the
// same reentrancy this file's channel-0xb note calls "THE WAIT CLOSES EARLY",
// and for the radio it is fatal rather than survivable.
void port_wm_arm7_command(unsigned int word);
void port_wm_arm7_turn(void);
void port_wm_arm7_census(void);
int  port_wm_arm7_saw_traffic(void);

// THE WIRELESS WORKER THREAD, hal/wm_thread.cpp. Run link100, lane WM5, rung
// W6: src/func_02040c34.c is linked now, so a session creates the cartridge's
// SECOND OS thread through the ROM's own func_02058200 and parks it in the
// ROM's own sleep. The census is a one-shot -- it returns immediately until
// src/func_02042200.c has written the entry pc into the record -- and the host
// ARM7's turn is where it is driven from, because that is the first owned code
// that runs after the ROM's reschedule has entered the thread.
void port_wm5_worker_census(void);
void port_wm5_report(void);

// The matched bodies this file brings up. Declared, never defined here.
void func_0205b858(void);            // tail veneer to func_0205bad8, PXI init
void func_02059e48(void);            // channel 0xc bring-up
void func_0205fde8(void);            // channel 8 bring-up
void _ZN3IRQ24IPCRxFifoNotEmptyHandlerEv(void);   // the receive interrupt

}  // extern "C"

namespace {

// ---------------------------------------------------------------------------
// THE CHANNELS THE ARM7 HOLDS
// ---------------------------------------------------------------------------

struct Channel {
    unsigned    id;
    const char *what;      // named from the ROM body that registers it
    bool        answer;    // false: held, but somebody else drives it
};

const Channel kChannels[] = {
    // Rung R2d (run link100, lane R2D2) linked src/func_0203bbc0.c and the
    // eight TUs behind it, and two of them SEND: src/func_0205edd8.cpp's pair
    // 0x02000303 / 0x0101001e and src/func_0205eeac.cpp's 0x02000104 /
    // 0x01010000. So this row is answer=true now and hal/tsc_arm7.cpp is what
    // makes it true. It answers on EVERY boot, solo or not, because the touch
    // panel is not a multiplayer device: the per-tag census must read four
    // words on tag 6 in any run that reaches the a054 seam.
    { 0x6, "touch/SPI      (src/func_0205f270.c -> func_0205f300; answered "
           "by hal/tsc_arm7.cpp)",                                   true  },
    // Run link100, lane SND1, rung R1: src/func_0205ae64.c is LINKED now
    // (port/slice_snd1.txt), so the ROM's own body registers this channel --
    // and it registers it by TESTING THIS CLAIM. func_0205ae64 does
    // `if (func_0205ba3c(7, 1)) return;` and the claim below is what makes that
    // true on its first turn, so the retry loop under it is never entered. The
    // row stays answer=false, and its reason is unchanged: hal/sdat/consumer.cpp
    // is the driver behind channel 7 and is driven from func_0205b5d4, so
    // consuming the batch here as well would run it twice.
    { 0x7, "sound command  (src/func_0205ae64.c; driven by "
           "hal/sdat/consumer.cpp)",                                 false },
    { 0x8, "channel 8      (src/func_0205fde8.c -> func_0205fcfc)",  true  },
    { 0xa, "wireless       (src/func_020616e8.c, WM_SendCommand; "
           "answered by hal/wm_arm7.cpp)",                           true  },
    // Rung W1 (run link100, lane WM2) linked src/func_020616e8.c, so WM_Init
    // really runs -- but it SENDS NOTHING, and this row was answer=false and
    // honestly so. Rung W4 (lane WM3) linked src/func_02040820.c and
    // src/func_02040790.c, the ROM's own become-parent and become-child state
    // machines, and THEY send: apiids 01 03 04 05 06 07 08 0c 0d 0e. So the row
    // is answer=true now and hal/wm_arm7.cpp is what makes it true. A solo boot
    // still sends nothing at all -- the role byte never leaves 0, so
    // src/func_0203df40.c takes its solo arm and neither body is ever called --
    // and the per-tag census must still read ZERO on tag 10 there.
    { 0xc, "channel 0xc    (src/func_02059e48.c -> func_02059e04)",  true  },
    { 0xd, "GBA slot       (src/func_0206a88c.c -> func_0206a694)",  false },
    { 0xb, "cart backup    (src/func_02060f60.cpp; ntr/backup.cpp faces the "
           "ROM body today and answers without the FIFO. See THE 0xB RECIPE "
           "below for what enrolling it now needs)",                  false },
};
const unsigned kChannelCount = sizeof kChannels / sizeof kChannels[0];

const Channel *find_channel(unsigned id) {
    for (unsigned i = 0; i < kChannelCount; ++i)
        if (kChannels[i].id == id) return &kChannels[i];
    return nullptr;
}

bool env_on(const char *name, bool dflt) {
    const char *v = std::getenv(name);
    if (!v || !v[0]) return dflt;
    return v[0] != '0';
}

// ---------------------------------------------------------------------------
// THE TOUCH-PANEL OBJECT'S LAYOUT, READ BACK BEFORE CHANNEL 6 IS CLAIMED
// ---------------------------------------------------------------------------
//
// data_020a80cc and data_020a80e4 are ONE 0x38 object with two dsd names, and
// hal/scene_boot.cpp lays them out as an ordered .dsstate$tp0001/$tp0002 run
// so that +0x18 of the first IS the second. A grouped-section run is a LAYOUT
// the linker could reorder, and every access this file's channel-6 driver
// serves reaches +0x30 .. +0x36, i.e. THROUGH the boundary -- so a reordered
// run is silent corruption of the ARM9's own status halfwords, not a fault.
//
// So it is measured, on every boot, before the channel is claimed, the way
// hal/comms_seam.cpp's port_wm_message_layout_check and
// hal/camera_bridges.cpp's hal_camera_check_layout measure theirs. There is no
// degraded mode: an ARM7 that answers channel 6 into a broken object would
// write the ROM's completion bits into whatever the linker put there instead,
// which is worse than not booting.
int port_tp_layout_check() {
    return (int)(data_020a80e4 - data_020a80cc) == 0x18;
}

// ---------------------------------------------------------------------------
// POWER-ON
// ---------------------------------------------------------------------------

void arm7_power_on() {
    if (!port_tp_layout_check()) {
        std::fprintf(stderr,
            "[arm7] HARD FAULT: the touch-panel work struct is not the ROM's "
            "0x38 run. data_020a80e4 came out at data_020a80cc + %d and the "
            "ROM says +0x18, so src/func_0205f270.c's stores at +0x30..+0x36 "
            "and src/func_0205ea28.c's read at +0x30 would land outside the "
            "object. hal/scene_boot.cpp's .dsstate$tp0001/$tp0002 run is what "
            "holds this.\n", (int)(data_020a80e4 - data_020a80cc));
        std::fflush(stderr);
        std::_Exit(24);
    }
    if (ntr::ipc_log_on())
        std::fprintf(stderr, "[arm7] touch-panel work struct: data_020a80e4 at "
                     "data_020a80cc + 0x%x, one 0x38 run as the ROM has it\n",
                     (unsigned)(data_020a80e4 - data_020a80cc));
    // THE ARM7'S OWN BOOT, which on hardware has already happened before the
    // ARM9's code runs: the console's user settings, including the touch
    // calibration src/func_0205f1e4.c reads at 0x027ffc80+0x58, are in the
    // shared block by now. hal/tsc_arm7.cpp section 3 is why that matters and
    // why a zeroed block is not a neutral default here.
    port_tsc_arm7_power_on();
    for (unsigned i = 0; i < kChannelCount; ++i)
        ntr::ipc_arm7_claim(kChannels[i].id);
    // The ROM registers this same function with
    // IRQ::SetIRQHandler(0x40000, IRQ::IPCRxFifoNotEmptyHandler) inside
    // src/func_0205bad8.c. ntr/runtime.cpp's host SetIRQHandler models the
    // geometry-FIFO and HBlank masks and silently drops every other one, so
    // 0x40000 never reaches it and the model is handed the handler here
    // instead. The proposed runtime.cpp change that would make this line
    // unnecessary is in the lane report; it is not made here because
    // ntr/runtime.cpp belongs to another lane tonight.
    ntr::ipc_set_rx_handler(&_ZN3IRQ24IPCRxFifoNotEmptyHandlerEv);
    if (ntr::ipc_log_on()) {
        std::fprintf(stderr, "[arm7] claimed %u channels, flags now 0x%08x\n",
                     kChannelCount, (unsigned)ntr::ipc_arm7_flags());
        for (unsigned i = 0; i < kChannelCount; ++i)
            std::fprintf(stderr, "[arm7]   %2u  %s%s\n", kChannels[i].id,
                         kChannels[i].what,
                         kChannels[i].answer ? "" : "   [observed only]");
    }
}

// ---------------------------------------------------------------------------
// A WORD FROM THE ARM9
// ---------------------------------------------------------------------------

unsigned long g_seen[32];
unsigned long g_answered;
unsigned long g_observed;
unsigned long g_refused;

void arm7_recv(uint32_t word) {
    const unsigned tag = ntr::ipc_tag(word);
    const unsigned flag = ntr::ipc_flag(word);
    ++g_seen[tag & 31u];

    if (flag) {
        // The ARM9's own receive handler sets this bit and posts the word back
        // when it holds no callback for the channel. Arriving here it means the
        // ARM9 refused something the ARM7 sent; there is nothing to answer.
        ++g_refused;
        if (ntr::ipc_log_on())
            std::fprintf(stderr, "[arm7] tag %u: the ARM9 has no handler "
                         "(word 0x%08x)\n", tag, word);
        return;
    }

    const Channel *c = find_channel(tag);

    // THE RADIO. Run link100, lane WM3, rung W4. src/WM_SendCommand.c's
    // IPCSend(0xa, buf, 0) lands here with the 0x100 command buffer's address
    // in the payload, and hal/wm_arm7.cpp is the driver behind the channel.
    // NOTHING IS POSTED FROM HERE: this call runs INSIDE the ARM9's store into
    // IPCFIFOSEND (ntr/ipc.cpp's fifo_send hands the word over inline), and the
    // WM callback path sends its next command from inside the callback -- so a
    // reply posted here would dispatch nested and the next one would hit
    // raise_rx_irq's g_in_rx guard and be dropped without a word. The command
    // is queued and answered from port_wm_arm7_turn(), at the points the ARM9
    // yields.
    if (tag == 0xa) {
        ++g_answered;
        port_wm_arm7_command(word);
        return;
    }

    // THE TOUCH PANEL. Run link100, lane R2D2, rung R2d. Same law, and here it
    // is load-bearing rather than merely correct: each command is a PAIR of
    // words and the ARM9 sets the bit it then waits on AFTER the second one, so
    // a completion posted from this call would clear a bit that is not set yet.
    // hal/tsc_arm7.cpp queues it and posts it from the turn taken inside the
    // ROM's own wait.
    if (tag == 0x6) {
        ++g_answered;
        port_tsc_arm7_command(word);
        return;
    }

    if (c && !c->answer) {
        ++g_observed;
        if (ntr::ipc_log_on())
            std::fprintf(stderr, "[arm7] tag %u observed, not consumed: %s\n",
                         tag, c->what);
        return;
    }
    if (c) {
        // A channel this ARM7 holds and has no driver behind. The honest reply
        // is the SDK's own: hand the word back with the no-handler flag set,
        // which is what the ARM9's handler does in the other direction.
        ++g_answered;
        if (ntr::ipc_log_on())
            std::fprintf(stderr, "[arm7] tag %u: held, no driver -- returning "
                         "the word with the no-handler flag\n", tag);
        ntr::ipc_arm7_send(word | 0x20u);
        return;
    }

    // A channel nobody claimed. Loud, because it means a ROM body reached a
    // part of the ARM7 this model does not know about, and a silent drop there
    // is a bug that shows up somewhere else entirely.
    ++g_refused;
    std::fprintf(stderr, "[arm7] UNCLAIMED CHANNEL %u (word 0x%08x, data "
                 "0x%07x). No host ARM7 driver holds it.\n",
                 tag, word, ntr::ipc_data(word));
    ntr::ipc_arm7_send(word | 0x20u);
}

// ---------------------------------------------------------------------------
// THE WIRELESS ARM: where the transport is opened.  Run link100, lane WM2,
// rung W1.
//
// WHAT CHANGED ON THE ARM9 SIDE. src/func_020408b0.c and eleven TUs behind it
// are LINKED (port/slice_wm2.txt), so the ROM's own WM bring-up runs: it takes
// eight buffers off the game heap and calls src/func_020616e8.c (WM_Init),
// which checks that the ARM7 holds channel 0xa, DMA-clears the 0x1300 work
// buffer, lays out the command area, the 0xa00 status block and the reply
// message, and finishes by registering the ARM9's own channel-0xa callback --
// `func_0205ba64(0xa, func_02061188)`.
//
// WHAT DID NOT CHANGE. THIS ARM7 STILL ANSWERS NOTHING. WM_Init sends no
// command: nothing in its call tree reaches WM_SendCommand, so no word ever
// arrives on tag 10 on this rung and the per-tag census must read zero for it
// in a solo run. That is a measurement this lane owes, not a hope.
//
// SO WHY OPEN ANYTHING AT ALL. Because the port's CommsTransport is not the
// ARM7's WM driver; it is the stand-in for the whole radio, including the
// bring-up command sequence that rungs W3 and W4 have not brought back yet
// (WM_Initialize, SET_P_PARAM, START_PARENT, START_CONNECT...). The earliest
// moment at which the ROM has COMMITTED to the radio, and the last thing its
// own bring-up does, is the channel-0xa registration above. That is the event
// this arm opens on, and it is the honest one: before it there is no radio
// request in flight, and after it the ROM proceeds straight to asking for a
// role.
//
// THE MODE WORD IS THE ROM'S OWN. src/func_020408b0.c:25 is
// `data_020a0f14 = arg` -- the FIRST statement of the body, before a single
// allocation and long before WM_Init -- so by the time the registration is
// visible the word already holds exactly what the ROM was opened with. The
// statement order was read off the source and is quoted in the lane report, so
// the brief's "if the mode word is written after the claim, pick a later
// trigger" branch does not arise. The seam used to pass its face's argument
// through; this reads the same value out of the ROM's own storage instead.
//
// HOW THE CLAIM IS SEEN. src/func_0205ba64.c sets bit N of the word at
// 0x027FFC00+0x388 when the ARM9 installs a receive callback for channel N --
// ntr/ipc.h calls that word PXI_FLAGS_ARM9 and ntr::ipc_arm9_flags() reads it.
// It is a plain store into the shared block, not an I/O access, so nothing
// traps it and this arm cannot be event-driven. It is POLLED, once per turn the
// ARM9 gives the ARM7, and hal/comms_seam.cpp's comms_arm7_turn is where those
// turns are handed over -- at the top of each remaining seam face, so the
// ARM7's look always precedes the ARM9's next radio request. The ordering
// matters: the frozen contract refuses a become_parent()/become_child() that
// arrives before open() and leaves the state idle permanently.
//
// NO TRANSPORT INSTALLED = NO OPEN = SOLO, exactly as before this rung. The
// arm still notes the claim (once, under SM64DS_IPC_LOG) so a solo log says the
// ROM's WM_Init ran and the ARM7 had nothing to open.
// ---------------------------------------------------------------------------

bool g_wm_claim_seen = false;
bool g_wm_opened = false;

void wireless_tick()
{
    if (g_wm_claim_seen) return;
    if ((ntr::ipc_arm9_flags() & (1u << 0xa)) == 0) return;
    g_wm_claim_seen = true;

    // The pair layout src/func_02061188.c's synthesised message depends on.
    // Checked here because this is the first host code that runs after WM_Init
    // laid the buffer out, and because a layout trick that stops working has to
    // fail loudly rather than quietly write past an object.
    if (!port_wm_message_layout_check())
        std::fprintf(stderr, "[arm7] WM message layout BROKEN: data_020a8a00 is "
                     "not at data_020a89ec + 0x14. src/func_02061188.c's "
                     "synthesised port-recv would write past its object.\n");

    const unsigned mode = *(const unsigned short *)data_020a0f14;
    const port::CommsTransport *t = port::comms_transport();
    if (!t) {
        if (ntr::ipc_log_on())
            std::fprintf(stderr, "[arm7] the ROM registered channel 0xa "
                         "(WM_Init ran, mode %u). No transport installed, so "
                         "there is nothing to open: solo.\n", mode);
        return;
    }
    g_wm_opened = true;
    std::fprintf(stderr, "[arm7] the ROM claimed channel 0xa (WM_Init ran); "
                 "opening the installed transport with the ROM's own mode word "
                 "data_020a0f14 = %u.\n", mode);
    t->open(mode);
    port::comms_publish_link_words();
}

// ---------------------------------------------------------------------------
// THE 0xB RECIPE: what enrolling src/func_02060f60.cpp actually needs
//
// Run link100 lane IPCSEND was asked whether the ROM's own card-backup body
// could run over the FIFO now that the ARM9 can send. It CAN in principle and
// it is NOT taken here, because the save path has to stay byte-identical and
// this is more than a route. What it wants, measured rather than guessed:
//
//   1. CHANNEL 0xB IS THE ONE TAG WHOSE PROTOCOL USES BIT 5 AS PART OF THE
//      REQUEST, and arm7_recv above would swallow every word of it. The ROM
//      sends `IPCSend(0xb, v, 1)` -- the third argument is the flag, and it is
//      SET -- and its own receive callback src/func_02060310.c only acts on
//      `if (a == 0xb && c != 0)`, c being that same bit off the reply. So both
//      directions carry it, and the general reading up there ("the flag means
//      the far side had no handler") is wrong for this tag alone. A 0xb route
//      has to sit BEFORE the `if (flag)` arm, not after it.
//
//   2. IT IS A TWO-WORD INIT FOLLOWED BY ONE-WORD COMMANDS. func_02060f60's
//      first call is `func_02060f60(thiz, 0, 1)`, and for v == 0 it sends the
//      command word AND THEN the command-block pointer (`q = thiz->f0`, which
//      is data_020a8160 -- a main-RAM address, so it fits the 26-bit payload).
//      Every later command (2 identify, 6 read, 8 program, 9 verify) is ONE
//      word carrying just the number, because the ARM7 already has the block.
//      So the host side is a two-state machine, not a switch: word 0 arms
//      "the next 0xb word is the block pointer", and the reply is owed after
//      the pointer rather than after the 0.
//
//   3. THE REPLY WAKES A THREAD, AND THE THREAD IS REAL IN THIS BUILD.
//      func_02060310 clears data_020a8180 + 0x34 bit 0x20 and then calls
//      func_02058048(*(void **)(data_020a8180 + 0xd0)). That word is not
//      written by func_0206002c; it is parked by the CARD THREAD ITSELF --
//      src/func_020602bc.cpp does `*(void **)(base + 0xd0) = base + 0x3c;`
//      immediately before each OS_SleepThread(0). Gate 223 creates and enters
//      that thread and port/tools/thread_create_proof.py measures it going
//      back to sleep, so +0xd0 holds &data_020a81bc rather than a null. THE
//      OFFSETS WERE READ AT THE ADDRESS: func_02060310 at 0x02060338 is
//      `ldr r2,[pc,#0x20] / add r1,r2,#0x34 / ldr r0,[r1] / bic r0,r0,#0x20 /
//      str r0,[r1] / ldr r0,[r2,#0xd0]`.
//
//   4. THE WAIT CLOSES EARLY, WHICH IS FINE AND WORTH KNOWING. ipc_arm7_send
//      dispatches inline, so func_02060310 runs INSIDE IPCSend -- before
//      func_02060f60 reaches `IRQ::Disable(); while (f34 & 0x20)
//      OS_SleepThread(0);`. The bit is already clear when the loop is entered,
//      so the loop is not executed at all. The func_02058048 in step 3 still
//      reschedules into the card thread from inside that send, which finds no
//      bit 8 in the state word, re-parks +0xd0 and sleeps again.
//
//   WHAT IS LEFT is the work this lane did not do: ntr/backup.cpp's
//   func_02060f60 face becomes the FIFO-facing half (the same switch, driven
//   by the stored block instead of by the caller's `self`), the face's C name
//   is freed so src/func_02060f60.cpp can take it, a slice carries that TU,
//   and port/tools/save_proof.py has to come back 27/27 with the 8192-byte
//   image byte-identical -- which is the real cost, because it is a second
//   full build and a save run, not a route.

// ---------------------------------------------------------------------------
// THE RECEIVE-PATH SELF-TEST  (SM64DS_IPC_SELFTEST=1)
//
// Two words, chosen so they change no game state at all, that walk the whole
// loop: model -> the ROM's own receive handler -> the ROM's own callback table
// -> back through the model to this ARM7.
//
//   1. tag 31, an UNCLAIMED channel. data_020a7fc8[31] is null and the flag is
//      clear, so src/_ZN3IRQ24IPCRxFifoNotEmptyHandlerEv.c takes its
//      no-handler arm: it ors 0x20 into the word and posts it to IPCFIFOSEND.
//      Seeing that word arrive back at arm7_recv proves the receive read, the
//      table lookup and the send path.
//   2. tag 8, data 0x100. The handler finds src/func_0205fcfc.c in the table
//      and calls it with (8, 0x100, 0). Inside func_0205fcfc that is
//      cmd = (0x100 & 0x7f00) >> 8 = 1 -- none of the commands it handles
//      (0x60, 0x62, 0x67, 0x70..0x73) -- and val = 0, so it falls straight
//      through to func_0205feac(0), which finds a zero flag and a null
//      callback and returns. Dispatch proved, nothing perturbed.
// ---------------------------------------------------------------------------

void selftest() {
    const ntr::IpcCounters before = ntr::ipc_counters();
    std::fprintf(stderr, "[ipc:selftest] pushing tag 31 (unclaimed) and "
                 "tag 8 (registered)\n");
    ntr::ipc_arm7_send(ntr::ipc_word(31, 0x12345u, 0));
    ntr::ipc_arm7_send(ntr::ipc_word(8, 0x100u, 0));
    const ntr::IpcCounters after = ntr::ipc_counters();
    std::fprintf(stderr,
        "[ipc:selftest] rx dispatches %lu -> %lu, ARM9 receives %lu -> %lu, "
        "ARM9 sends %lu -> %lu, refused-by-ARM9 %lu\n",
        before.rx_dispatches, after.rx_dispatches, before.recvs, after.recvs,
        before.sends, after.sends, g_refused);
    std::fprintf(stderr, "[ipc:selftest] %s\n",
                 (after.rx_dispatches > before.rx_dispatches &&
                  after.recvs - before.recvs >= 2 &&
                  after.sends > before.sends && g_refused > 0)
                 ? "PASS: the ROM handler read both words, dispatched the "
                   "registered one and returned the unclaimed one"
                 : "FAIL");
    std::fflush(stderr);
}

// ---------------------------------------------------------------------------
// THE EXIT REPORT AND THE PER-CHANNEL TALLY  (run link100, lane IPCSEND)
//
// port_ipc_rom_boot_done below takes ipc_report("rom-boot") at the end of the
// ROM's pre-main PXI span, and at THAT point send= can only ever be zero:
// nothing in the bring-up sends. Every ARM9-initiated command in this build
// comes later -- channel 7 once a frame out of src/func_0205b070.c, channel 8
// at game init out of func_020196cc -- so the rom-boot line is a report on a
// FIFO nobody has used yet, and reading its send=0 as "the ARM9 never sends"
// is half of what cost lane BOOT2 a seat. (Only half: src/IPCSend.c really was
// compiled plain and really did latch every store in the mapped window. It is
// hostgen'd now, port/CMakeLists.txt's GATE2IPC_SYMS.)
//
// So the same report is taken again at the OTHER end of the run, with the
// per-tag tally arm7_recv has been keeping all along beside it. That tally is
// the only place the ARM9->ARM7 direction is visible PER CHANNEL, and that is
// what a sound-path regression would show up in: channel 7 is held
// observed-only, so its count going UP is traffic arriving and being declined
// -- which changes nothing -- while its count going to zero would mean
// func_0205b070's per-frame poke had stopped happening.
//
// Log-gated on SM64DS_IPC_LOG, which port/tools/ipc_proof.py's R1 already
// sets: this is evidence, not a line every launch needs.
void exit_report()
{
    ntr::ipc_report("exit");
    for (unsigned t = 0; t < 32; ++t) {
        if (!g_seen[t]) continue;
        const Channel *c = find_channel(t);
        std::fprintf(stderr, "[arm7:census] tag %2u  %6lu word(s) from the "
                     "ARM9  %s%s\n", t, g_seen[t],
                     c ? c->what : "UNCLAIMED CHANNEL",
                     (c && !c->answer) ? "   [observed only]" : "");
    }
    std::fprintf(stderr, "[arm7:census] answered %lu, observed %lu, "
                 "refused %lu\n", g_answered, g_observed, g_refused);
    // AND THE RADIO'S OWN THREE NUMBERS beside them (run link100, lane WM3).
    // Printed on every logged run, including a solo one where all three are
    // zero -- "the ARM7 was never asked" is the measurement a solo run owes,
    // not an absence of output.
    port_wm_arm7_census();
    // AND THE WORKER THREAD'S OWN LINE (run link100, lane WM5). Same reason:
    // a solo run owes "no wireless thread was created", stated, not absent.
    port_wm5_report();
    // AND THE TOUCH PANEL'S, on the same terms (run link100, lane R2D2). A run
    // that never reached the a054 seam reports zeros, which is the measurement
    // that run owes rather than an absence of output.
    port_tsc_arm7_census();
    std::fflush(stderr);
}

// ---------------------------------------------------------------------------
// THE SEAM: the ROM's own PXI bring-up
// ---------------------------------------------------------------------------
//
// DELETE THIS WHOLE FUNCTION when a pre-main spine lands. src/func_02058c84.c
// makes exactly these calls, in this order, as its 2nd, 10th and 13th steps;
// src/func_0203bbc0.c makes the fourth as its first. Every one of them is the
// ROM's own body doing its own work -- the only thing this function supplies
// is the moment.
//
// TWO OF THE SIX CHANNELS ARE CLAIMED BUT NOT BROUGHT UP, and both for a
// reason outside this file:
//
//   channel 0xd, src/func_0206a88c.c (func_02058c84's 11th call). The model
//   unblocks its spin the same way it unblocks the other two, but linking it
//   pulls src/func_0206a6d0.c and with it the GBA-slot chain and the OS lock
//   family under func_020570b8/func_02057158. A different lane's work.
//
//   channel 6, src/func_0205f270.c (func_0203bbc0's first call). THE HOST BUG
//   THIS PARAGRAPH USED TO NAME IS FIXED (run link100, lane R2D2, gate 1):
//   hal/scene_boot.cpp hosted data_020a80cc as `int[6]`, 24 bytes, while
//   func_0205f270's own stores reach +0x36, and it now hosts the ROM's whole
//   0x38 run -- data_020a80cc and data_020a80e4 as one ordered .dsstate$tp
//   pair -- with port_tp_layout_check above reading the 0x18 boundary back on
//   every boot. port/touch_map.txt section 269 recorded the undersize.
//   Whether the channel is ANSWERED is the row in kChannels, not this note.
void arm9_bring_up() {
    // Nothing to bring up here any more: the three ROM arms (func_0205b858 at
    // func_02058c84's call 2, func_02059e48 at call 10, func_0205fde8 at call
    // 13) run from hal/boot_os.cpp's pre-main span at the ROM's own points in
    // the boot order (run link100 integration), and this hook runs at
    // ntr::io_init() time, before that span. The report and the selftest moved
    // to port_ipc_rom_boot_done(), which the span calls when the ROM's own
    // handshake has happened. SM64DS_IPC_ROM_BOOT is retired: the spine runs
    // ROM code.
}

const ntr::Arm7Model kArm7 = {
    "host ARM7 (hal/boot2_ipc.cpp)",
    arm7_power_on,
    arm7_recv,
    arm9_bring_up,
};

// Registered at static-init, before main() and so before ntr::io_init() runs
// ipc_init(). A plain object with no dynamic initialisation of its own on
// either side, so there is no static-order question to get wrong.
struct Attach {
    Attach() { ntr::ipc_attach_arm7(&kArm7); }
};
Attach g_attach;

}  // namespace

// hal/comms_seam.cpp's comms_arm7_turn calls this. See THE WIRELESS ARM above.
//
// TWO THINGS HAPPEN IN A TURN, and the order is the DS's. First the ARM7 looks
// for the ROM's channel-0xa claim and opens the transport on it (rung W1's
// arm); then it answers ONE queued WM command (rung W4's). The open has to come
// first because the frozen contract refuses a become_parent()/become_child()
// that arrives before open(), and on the very first turn of a session both
// happen -- the ROM registers the channel inside func_020408b0 and asks for a
// role in the same breath (hal/comms_conductor.cpp:1137-1139).
extern "C" void port_arm7_wireless_tick(void)
{
    wireless_tick();
    port_wm_arm7_turn();
    port_wm5_worker_census();
    // AND THE TOUCH PANEL'S TURN (run link100, lane R2D2). Two jobs: drain a
    // completion no wait collected -- a queued word that never leaves is a
    // swallowed one by another name -- and, only under SM64DS_TP_RING, post the
    // auto-sample indication that makes the ROM's own func_0205f300 the ring's
    // writer. Off by default because hal/sub_screen.cpp still writes that ring;
    // hal/tsc_arm7.cpp section 4 is the whole of it.
    port_tsc_arm7_tick();
}

// Called by hal/boot_os.cpp at the end of port_boot_rom_pre_main(), i.e. after
// the ROM's own PXI arms have run against the model: the report and the
// selftest see the handshake the ROM performed, not an empty model.
extern "C" void port_ipc_rom_boot_done(void)
{
    ntr::ipc_report("rom-boot");
    /* and the same report at the far end of the run, where the ARM9's own
       sends have actually happened. See THE EXIT REPORT above. */
    if (ntr::ipc_log_on()) std::atexit(exit_report);
    if (env_on("SM64DS_IPC_SELFTEST", false)) selftest();
}
