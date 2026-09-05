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
//   Four are held and OBSERVED ONLY, each for a reason:
//     7    hal/sdat/consumer.cpp is already the port's ARM7 sound driver and
//          it is driven from func_0205b5d4. Consuming the batch here as well
//          would run it twice.
//     0xa  nothing in this build runs WM_Init (src/func_020616e8.c), so a
//          fabricated wireless answer would be answering a question no one
//          asked.
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
    { 0x6, "touch/SPI      (src/func_0205f270.c -> func_0205f300)",  false },
    { 0x7, "sound command  (src/func_0205ae64.c; driven by "
           "hal/sdat/consumer.cpp)",                                 false },
    { 0x8, "channel 8      (src/func_0205fde8.c -> func_0205fcfc)",  true  },
    { 0xa, "wireless       (src/func_020616e8.c, WM_SendCommand)",   false },
    { 0xc, "channel 0xc    (src/func_02059e48.c -> func_02059e04)",  true  },
    { 0xd, "GBA slot       (src/func_0206a88c.c -> func_0206a694)",  false },
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
// POWER-ON
// ---------------------------------------------------------------------------

void arm7_power_on() {
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
//   channel 6, src/func_0205f270.c (func_0203bbc0's first call). This one is
//   blocked on a HOST BUG, not on scope: hal/scene_boot.cpp hosts
//   data_020a80cc as `int[6]`, 24 bytes, and func_0205f270's own stores reach
//   +0x36 -- so enrolling it today would write 22 bytes past the end of a host
//   object. port/touch_map.txt section 269 already records the undersize. The
//   one-line fix is in the lane report; hal/scene_boot.cpp is not this lane's
//   file, and running the body against the short object would be worse than
//   leaving the channel claimed and quiet.
void arm9_bring_up() {
    if (!env_on("SM64DS_IPC_ROM_BOOT", true)) {
        std::fprintf(stderr, "[ipc] ROM bring-up skipped "
                     "(SM64DS_IPC_ROM_BOOT=0)\n");
        return;
    }
    func_0205b858();     // func_02058c84 call 2:  PXI init (func_0205bad8)
    func_02059e48();     // func_02058c84 call 10: channel 0xc
    func_0205fde8();     // func_02058c84 call 13: channel 8
    ntr::ipc_report("rom-boot");
    if (env_on("SM64DS_IPC_SELFTEST", false)) selftest();
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
