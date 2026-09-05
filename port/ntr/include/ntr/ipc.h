// Host model of the DS's inter-processor link: IPCSYNC, the two 16-entry PXI
// FIFOs, and the two channel-registration words in the shared system block.
//
// WHY THIS FILE EXISTS. port/docs/mmio-inventory.md counted eight IPC
// registers and zero modelled, and that zero is what kept the ROM's own boot
// chain out of the link. Five matched bodies stop dead without a second CPU
// on the other end of these registers:
//
//   src/func_0205bad8.c   PXI bring-up. Writes IPCFIFOCNT 0xc408 (enable |
//                         error-ack | recv-not-empty IRQ | send-fifo clear),
//                         then runs a nine-round handshake against the ARM7
//                         over IPCSYNC 0x04000180. Reached through the tail
//                         veneer src/func_0205b858.c.
//   src/func_02059e48.c   spins `while (func_0205ba3c(0xc, 1) == 0)`
//   src/func_0205fde8.c   the same spin on channel 8
//   src/func_0206a88c.c   the same spin on channel 0xd
//   src/func_0205f270.c   the same spin on channel 6
//
// func_0205ba3c(bit, word) reads word `word` of the array at 0x027FFC00+0x388.
// Word 0 is the ARM9's own registered-channel mask -- src/func_0205ba64.c is
// its only writer, setting bit N when the ARM9 installs a receive callback for
// channel N. Word 1 is the ARM7's, and the four spins above are each waiting
// for the ARM7 to claim their channel. Nothing in the port ever wrote it, so
// every one of those loops is infinite on the host.
//
// THE HANDSHAKE IS NOT GUESSED. The ARM7's half of it is in the ROM:
// extracted/arm7.bin at file offset 0x31a4..0x31e0 (ARM7 RAM 0x023831a4),
// which is the mirror of src/func_0205bad8.c's loop:
//
//      r4 = 8; r6 = 8; r8 = 0x04000180; r7 = 0x3e8
//   L: *(u16 *)r8 = r4 << 8          // publish our nibble
//      WaitByLoop(0x3e8)             // let the ARM9 see it and echo
//      r0 = *(u16 *)r8 & 0xf         // read the ARM9's echo
//      if (r0 != r4) r4 = r6         // not echoed yet -- restart at 8
//      r4 = r4 - 1
//      if (r4 >= 0) goto L
//
// so the ARM7 counts 8,7,6,...,0 and each step needs the ARM9 to have echoed
// the previous value. The ARM9's loop breaks when it reads a zero nibble on a
// round where its own counter has passed 4, which is round nine. ipc.cpp runs
// exactly that ARM7 loop, one iteration per ARM9 write to IPCSYNC -- the write
// IS the echo the ARM7's WaitByLoop is there to wait for, so stepping on it is
// the single-threaded host's version of "the other core got there".
//
// This is a HARDWARE MODEL. Nothing in ipc.cpp or hal/boot2_ipc.cpp is
// decompiled code and none of it is tagged PORT_HOST_ABI: it stands in for a
// second processor, the way ntr/gx.cpp stands in for the geometry engine.
#ifndef NTR_IPC_H
#define NTR_IPC_H

#include <stdint.h>

namespace ntr {

// GBATEK names. IPCFIFORECV is the odd one: it lives at 0x04100000, outside
// the 0x04000000+0x2000 window io.cpp maps, so a PLAIN read of it faults on
// the host. Every TU that reads it has to go through hostgen (see
// port/slice_gate2ipc.txt) so the access arrives at ipc_reg_read below.
constexpr uint32_t REG_IPCSYNC     = 0x04000180u;
constexpr uint32_t REG_IPCFIFOCNT  = 0x04000184u;
constexpr uint32_t REG_IPCFIFOSEND = 0x04000188u;
constexpr uint32_t REG_IPCFIFORECV = 0x04100000u;

// The two registration words, as src/func_0205ba3c.c and src/func_0205ba64.c
// address them: a 0x388-byte pad from 0x027FFC00, then word 0 (ARM9) and
// word 1 (ARM7). Inside SHARED_BASE, so both are backed memory already.
constexpr uint32_t PXI_FLAGS_ARM9 = 0x027FFC00u + 0x388u;
constexpr uint32_t PXI_FLAGS_ARM7 = 0x027FFC00u + 0x38Cu;

// IPCFIFOCNT bits (GBATEK).
enum : uint16_t {
    FIFOCNT_SEND_EMPTY     = 0x0001,
    FIFOCNT_SEND_FULL      = 0x0002,
    FIFOCNT_SEND_EMPTY_IRQ = 0x0004,
    FIFOCNT_SEND_CLEAR     = 0x0008,
    FIFOCNT_RECV_EMPTY     = 0x0100,
    FIFOCNT_RECV_FULL      = 0x0200,
    FIFOCNT_RECV_IRQ       = 0x0400,
    FIFOCNT_ERROR          = 0x4000,
    FIFOCNT_ENABLE         = 0x8000,
};
constexpr unsigned FIFO_DEPTH = 16;   // 16 words each way, both DS FIFOs

// A word on the wire, as src/IPCSend.c and
// src/_ZN3IRQ24IPCRxFifoNotEmptyHandlerEv.c lay it out: five tag bits, one
// flag bit, twenty-six payload bits. The flag is the SDK's "no handler on the
// far side" marker -- the ROM's receive handler sets it (`cmd.raw |= 0x20`)
// and posts the word back when a channel arrives with no callback installed,
// and the host ARM7 answers an unclaimed channel the same way.
inline unsigned ipc_tag(uint32_t w)  { return w & 0x1fu; }
inline unsigned ipc_flag(uint32_t w) { return (w >> 5) & 1u; }
inline uint32_t ipc_data(uint32_t w) { return w >> 6; }
inline uint32_t ipc_word(unsigned tag, uint32_t data, unsigned flag) {
    return (tag & 0x1fu) | ((flag & 1u) << 5) | (data << 6);
}

// The host's stand-in for the second processor. hal/boot2_ipc.cpp registers
// one; ntr never depends on hal.
struct Arm7Model {
    const char *name;
    // The ARM7 has finished its own boot before the ARM9's code runs, which is
    // true on hardware too. This is where it claims its channels.
    void (*power_on)();
    // A word the ARM9 pushed into IPCFIFOSEND. On hardware the ARM7's FIFO IRQ
    // picks it up; here it is handed over on the store.
    void (*recv)(uint32_t word);
    // The link is live. Optional, and it is a SEAM, not hardware: it is where
    // the owner of the model runs whatever ARM9-side bring-up the port does not
    // have a spine for yet. See hal/boot2_ipc.cpp.
    void (*link_up)();
};

bool ipc_attach_arm7(const Arm7Model *m);

// Idempotent. io_init() calls it once the regions are up.
void ipc_init();
void ipc_reset();

bool ipc_enabled();     // SM64DS_IPC_MODEL=0 turns the model off (the control)
bool ipc_log_on();      // SM64DS_IPC_LOG=1

// io.cpp's dispatch. ipc_reg_read sets *handled when it owns the address; when
// it does not, io.cpp falls through to the mapped window as before.
uint64_t ipc_reg_read(uint32_t addr, unsigned width, bool *handled);
bool     ipc_reg_write(uint32_t addr, uint64_t value, unsigned width);

// The ARM7 side.
void     ipc_arm7_send(uint32_t word);       // ARM7 -> ARM9, fills the recv FIFO
void     ipc_arm7_claim(unsigned channel);   // set bit in PXI_FLAGS_ARM7
uint32_t ipc_arm7_flags();
uint32_t ipc_arm9_flags();

// The ROM's receive interrupt. func_0205bad8 arms it with
// IRQ::SetIRQHandler(0x40000, IRQ::IPCRxFifoNotEmptyHandler), but
// ntr/runtime.cpp's host SetIRQHandler models two masks and drops that one, so
// the model is told the handler directly. See hal/boot2_ipc.cpp.
void ipc_set_rx_handler(void (*h)());

// Put the model's authoritative IPCSYNC / IPCFIFOCNT values back into the
// mapped I/O window, so a TU built PLAIN reads the truth. Same reason
// io_gxstat_publish() exists: a trigger in io.cpp only reaches TUs that are
// routed, and src/IPCSend.c is built plain out of port/slice_gate10.txt.
void ipc_publish();

struct IpcCounters {
    unsigned long sync_reads;      // ARM9 reads of IPCSYNC
    unsigned long sync_steps;      // host-ARM7 handshake iterations run
    unsigned      sync_arm7_count; // the ARM7's counter, -1 once it is done
    bool          sync_done;       // the ARM7's loop exited (r4 went below 0)
    unsigned long sends;           // words the ARM9 put in IPCFIFOSEND
    unsigned long recvs;           // words the ARM9 took from IPCFIFORECV
    unsigned long arm7_pushes;     // words the host ARM7 delivered
    unsigned long rx_dispatches;   // times the ROM's receive handler was run
    unsigned long errors;          // FIFO error latches (empty read / full send)
    unsigned long naks;            // words returned with the no-handler flag
    unsigned      fifocnt;         // the current IPCFIFOCNT value
};
IpcCounters ipc_counters();

// One line for a log, in the shape port/tools/ipc_proof.py parses.
void ipc_report(const char *tag);

}  // namespace ntr

#endif  // NTR_IPC_H
