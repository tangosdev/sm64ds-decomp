// The IPC/PXI hardware model. See ntr/include/ntr/ipc.h for what it is and
// where the ARM7's half of the handshake was read out of.

#include "ntr/ipc.h"
#include "ntr/mmio.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ntr {
namespace {

// ---------------------------------------------------------------------------
// STATE
// ---------------------------------------------------------------------------

const Arm7Model *g_arm7;
void (*g_rx_handler)();

bool g_init;
int  g_enabled = -1;    // -1 unread, 0 off, 1 on
int  g_log = -1;

// IPCSYNC. Bits 0-3 are what the OTHER cpu published; bits 8-11 are ours.
// Bits 13 (send IRQ to the other cpu) and 14 (accept IRQ from it) are ours to
// latch. The DS wires each side's bits 8-11 to the other side's 0-3.
unsigned g_sync_arm9_out;    // the ARM9's nibble, bits 8-11
unsigned g_sync_arm7_out = 8;  // the ARM7's nibble, as the ARM9 reads it
unsigned g_sync_ctrl;        // bits 13-14, latched
int      g_arm7_r4 = 8;      // the ARM7 loop's counter
bool     g_sync_done;

// IPCFIFOCNT: only the three writable enables live here; every status bit is
// derived from the queues below so it can never go stale.
uint16_t g_ctrl;
bool     g_error;

uint32_t g_send[FIFO_DEPTH];   // ARM9 -> ARM7
unsigned g_send_n;
uint32_t g_recv[FIFO_DEPTH];   // ARM7 -> ARM9
unsigned g_recv_n;
uint32_t g_recv_last;          // GBATEK: an empty read returns the last word

IpcCounters g_c;

// THE BOUNDED-SPIN GUARD. src/func_0205bad8.c's handshake has no timeout: with
// no ARM7 answering, its inner 1000-read spin times out, resets its counter and
// starts over, forever. That is the failure this whole file removes, and it is
// also the negative control port/tools/ipc_proof.py runs -- so it must end in a
// diagnostic and an exit code, never in a hang. The cap is generous: a healthy
// handshake takes about twenty reads.
unsigned long g_spin_cap = 100000ul;   // safe before ipc_init reads the env
unsigned long g_since_progress;

void log_line(const char *fmt, ...);

void spin_guard_trip() {
    std::fprintf(stderr,
        "[ipc] BOUNDED-SPIN GUARD: %lu reads of IPCSYNC with no handshake "
        "progress. The ARM7 model is %s. src/func_0205bad8.c cannot converge "
        "without it and would spin here forever.\n",
        g_since_progress, ipc_enabled() ? "attached but silent" : "DISABLED "
        "(SM64DS_IPC_MODEL=0)");
    ipc_report("spin-guard");
    std::fflush(stderr);
    std::_Exit(23);
}

// ---------------------------------------------------------------------------
// THE MAPPED WINDOW
//
// Reads and writes that reach io_read/io_write only come from TUs hostgen
// routed. Everything else latches in the window io.cpp maps, so the model
// publishes its own status there after every change. src/IPCSend.c is the case
// that matters: it is built plain out of port/slice_gate10.txt and it reads
// IPCFIFOCNT's error and send-full bits before every store. If those read
// stale-set, its callers (src/func_0205ae30.c's `while (IPCSend(7,0,0) < 0)`,
// src/func_0205f040.c) spin forever -- so publish() is not cosmetic.
// ---------------------------------------------------------------------------

volatile uint16_t *win16(uint32_t addr) {
    return reinterpret_cast<volatile uint16_t *>(static_cast<uintptr_t>(addr));
}
volatile uint32_t *win32(uint32_t addr) {
    return reinterpret_cast<volatile uint32_t *>(static_cast<uintptr_t>(addr));
}

uint16_t sync_value() {
    return static_cast<uint16_t>((g_sync_arm7_out & 0xfu) |
                                 ((g_sync_arm9_out & 0xfu) << 8) |
                                 (g_sync_ctrl & 0x6000u));
}

uint16_t fifocnt_value() {
    uint16_t v = static_cast<uint16_t>(g_ctrl & (FIFOCNT_ENABLE |
                                                 FIFOCNT_RECV_IRQ |
                                                 FIFOCNT_SEND_EMPTY_IRQ));
    if (g_send_n == 0)          v |= FIFOCNT_SEND_EMPTY;
    if (g_send_n >= FIFO_DEPTH) v |= FIFOCNT_SEND_FULL;
    if (g_recv_n == 0)          v |= FIFOCNT_RECV_EMPTY;
    if (g_recv_n >= FIFO_DEPTH) v |= FIFOCNT_RECV_FULL;
    if (g_error)                v |= FIFOCNT_ERROR;
    return v;
}

void log_line(const char *fmt, ...) {
    if (!ipc_log_on()) return;
    va_list ap;
    va_start(ap, fmt);
    std::fputs("[ipc] ", stderr);
    std::vfprintf(stderr, fmt, ap);
    std::fputc('\n', stderr);
    va_end(ap);
}

// ---------------------------------------------------------------------------
// THE ARM7'S HANDSHAKE LOOP, from extracted/arm7.bin 0x31a4..0x31e0
// ---------------------------------------------------------------------------
//
// One iteration per ARM9 write to IPCSYNC. On hardware the ARM7 sits in
// WaitByLoop(0x3e8) between publishing its nibble and reading the echo, and
// the ARM9 completes a whole round inside that wait; here the ARM9's echoing
// store IS the event, so it drives the step. The trace, which is the one the
// hardware runs:
//
//   ARM7 boots first and has already published 8.
//   round 1  ARM9 reads 8, echoes 8   -> ARM7 sees its value, r4=7, publishes 7
//   round 2  ARM9 reads 7, echoes 7   -> r4=6 ...
//   ...
//   round 8  ARM9 reads 1, echoes 1   -> r4=0, publishes 0
//   round 9  ARM9 reads 0, echoes 0   -> r4 goes below zero, the ARM7 exits;
//            the ARM9's own counter is 8 > 4, so it breaks too.
//
// Nine rounds, both sides finishing on the same store, no spinning on either.
void arm7_sync_step() {
    if (g_sync_done) return;
    ++g_c.sync_steps;
    if (static_cast<int>(g_sync_arm9_out) != g_arm7_r4)
        g_arm7_r4 = 8;                 // `movne r4, r6`: not echoed, start over
    --g_arm7_r4;                       // `sub r4, r4, #1`
    if (g_arm7_r4 < 0) {               // `cmp r4,#0 / bge` fell through
        g_sync_done = true;
        g_c.sync_done = true;
        log_line("sync complete after %lu steps, %lu reads",
                 g_c.sync_steps, g_c.sync_reads);
        return;
    }
    g_sync_arm7_out = static_cast<unsigned>(g_arm7_r4);   // `strh r0,[r8]`
    g_c.sync_arm7_count = static_cast<unsigned>(g_arm7_r4);
}

// ---------------------------------------------------------------------------
// THE FIFOs
// ---------------------------------------------------------------------------

bool g_in_rx;   // re-entrancy guard: the handler's own NAK re-enters fifo_send

void raise_rx_irq() {
    if (g_in_rx) return;
    // THE GATE, and what it deliberately leaves out. On hardware four things
    // have to be true: the FIFO enabled, the recv-not-empty enable set, IE bit
    // 18, and IME. src/func_0205bad8.c arms the first two itself by storing
    // 0xc408 into IPCFIFOCNT, and those two are what this checks -- they are
    // the ROM's own arming, observed through this model, so a build where the
    // game never ran the bring-up never gets a dispatch.
    //
    // IE and IME are NOT checked, and that is a stated gap rather than an
    // oversight. ntr/runtime.cpp holds the IE word in a host variable with no
    // reader outside that file, and IME (0x04000208) is written by the port's
    // frame loop LATER than the seam that runs the bring-up, so gating on
    // either would mean never delivering a word at all. When lane BOOT's
    // pre-main spine lands, both become observable and belong in this test.
    if (!g_rx_handler) return;
    if (!(g_ctrl & FIFOCNT_ENABLE)) return;
    if (!(g_ctrl & FIFOCNT_RECV_IRQ)) return;
    if (g_recv_n == 0) return;
    ++g_c.rx_dispatches;
    g_in_rx = true;
    g_rx_handler();
    g_in_rx = false;
}

void fifo_send(uint32_t word) {
    ++g_c.sends;
    if (!(g_ctrl & FIFOCNT_ENABLE)) {
        log_line("send 0x%08x DROPPED: the FIFO is not enabled", word);
        return;
    }
    if (g_send_n >= FIFO_DEPTH) {
        g_error = true;
        ++g_c.errors;
        log_line("send 0x%08x REFUSED: send FIFO full", word);
        return;
    }
    log_line("ARM9 -> ARM7  tag %2u flag %u data 0x%07x  (word 0x%08x)",
             ipc_tag(word), ipc_flag(word), ipc_data(word), word);
    // A one-word queue in practice: the host ARM7 answers on the store, which
    // is what a second processor with an idle FIFO IRQ does. The depth is
    // still modelled so a refusing ARM7 can back the queue up honestly.
    g_send[g_send_n++] = word;
    if (g_arm7 && g_arm7->recv) {
        const uint32_t w = g_send[0];
        std::memmove(g_send, g_send + 1, (g_send_n - 1) * sizeof g_send[0]);
        --g_send_n;
        g_arm7->recv(w);
    }
}

uint32_t fifo_recv() {
    if (g_recv_n == 0) {
        // GBATEK: reading an empty receive FIFO latches the error bit and
        // returns the last word read. The ROM's handler tests RECV_EMPTY
        // before every read, so this is the path it never takes.
        g_error = true;
        ++g_c.errors;
        return g_recv_last;
    }
    const uint32_t w = g_recv[0];
    std::memmove(g_recv, g_recv + 1, (g_recv_n - 1) * sizeof g_recv[0]);
    --g_recv_n;
    g_recv_last = w;
    ++g_c.recvs;
    log_line("ARM9 <- ARM7  tag %2u flag %u data 0x%07x  (word 0x%08x)",
             ipc_tag(w), ipc_flag(w), ipc_data(w), w);
    return w;
}

}  // namespace

// ---------------------------------------------------------------------------
// PUBLIC
// ---------------------------------------------------------------------------

bool ipc_enabled() {
    if (g_enabled < 0) {
        const char *v = std::getenv("SM64DS_IPC_MODEL");
        g_enabled = (v && v[0] == '0') ? 0 : 1;
    }
    return g_enabled != 0;
}

bool ipc_log_on() {
    if (g_log < 0) {
        const char *v = std::getenv("SM64DS_IPC_LOG");
        g_log = (v && v[0] && v[0] != '0') ? 1 : 0;
    }
    return g_log != 0;
}

bool ipc_attach_arm7(const Arm7Model *m) {
    if (!m || !m->power_on || !m->recv) return false;
    g_arm7 = m;
    return true;
}

void ipc_set_rx_handler(void (*h)()) { g_rx_handler = h; }

void ipc_reset() {
    g_sync_arm9_out = 0;
    g_sync_arm7_out = 8;
    g_sync_ctrl = 0;
    g_arm7_r4 = 8;
    g_sync_done = false;
    g_ctrl = 0;
    g_error = false;
    g_send_n = g_recv_n = 0;
    g_recv_last = 0;
    g_since_progress = 0;
    g_in_rx = false;
    std::memset(&g_c, 0, sizeof g_c);
    g_c.sync_arm7_count = 8;
    ipc_publish();
}

void ipc_publish() {
    *win16(REG_IPCSYNC) = sync_value();
    *win16(REG_IPCFIFOCNT) = fifocnt_value();
    g_c.fifocnt = fifocnt_value();
}

uint32_t ipc_arm7_flags() { return *win32(PXI_FLAGS_ARM7); }
uint32_t ipc_arm9_flags() { return *win32(PXI_FLAGS_ARM9); }

void ipc_arm7_claim(unsigned channel) {
    *win32(PXI_FLAGS_ARM7) |= (1u << (channel & 31u));
}

void ipc_arm7_send(uint32_t word) {
    if (g_recv_n >= FIFO_DEPTH) {
        g_error = true;
        ++g_c.errors;
        log_line("ARM7 push 0x%08x REFUSED: receive FIFO full", word);
        ipc_publish();
        return;
    }
    g_recv[g_recv_n++] = word;
    ++g_c.arm7_pushes;
    if (ipc_flag(word)) ++g_c.naks;
    ipc_publish();
    raise_rx_irq();
    ipc_publish();
}

void ipc_init() {
    if (g_init) return;
    g_init = true;

    const char *cap = std::getenv("SM64DS_IPC_SPIN_CAP");
    g_spin_cap = cap ? std::strtoul(cap, nullptr, 0) : 100000ul;
    if (g_spin_cap == 0) g_spin_cap = 100000ul;

    if (!ipc_enabled()) {
        std::fprintf(stderr, "[ipc] model DISABLED (SM64DS_IPC_MODEL=0): "
                     "IPCSYNC and the FIFOs fall through to the mapped window "
                     "and no ARM7 answers. The bounded-spin guard is armed at "
                     "%lu reads.\n", g_spin_cap);
        // THE CONTROL STILL RUNS THE ROM CODE. Turning the model off has to
        // leave the ROM's own bring-up being ATTEMPTED, or port/tools/
        // ipc_proof.py's rung R2 proves nothing at all -- it would just be a
        // build where src/func_0205bad8.c is never called. The host ARM7 is
        // NOT powered on here and claims no channel, so the handshake cannot
        // converge and the bounded-spin guard is what ends the run.
        if (g_arm7 && g_arm7->link_up) g_arm7->link_up();
        return;
    }
    ipc_reset();
    if (g_arm7) {
        log_line("ARM7 model '%s' powering on", g_arm7->name);
        g_arm7->power_on();
        ipc_publish();
        if (g_arm7->link_up) g_arm7->link_up();
    } else {
        // Every smoke target links ntr and therefore this model, and most of
        // them have no reason to link a host ARM7. Log-gated, so a build that
        // never touches an IPC register is not told about it on every run.
        log_line("no ARM7 model attached: the registers are modelled but "
                 "nothing answers them");
    }
}

uint64_t ipc_reg_read(uint32_t addr, unsigned width, bool *handled) {
    *handled = false;

    // IPCFIFORECV is outside every mapped region, so it is answered even with
    // the model off -- a fall-through would be an access violation, not a
    // control. With the model off it reports an empty FIFO, which is what the
    // hardware says when no one is talking.
    if (addr == REG_IPCFIFORECV) {
        *handled = true;
        if (!ipc_enabled()) return 0;
        const uint32_t w = fifo_recv();
        ipc_publish();
        return w;
    }
    if (!ipc_enabled()) {
        // The negative control still counts IPCSYNC reads, so the guard can
        // end the run instead of letting func_0205bad8 spin.
        if (addr == REG_IPCSYNC) {
            ++g_c.sync_reads;
            if (++g_since_progress > g_spin_cap) spin_guard_trip();
        }
        return 0;
    }
    if (addr == REG_IPCSYNC && width == 2) {
        *handled = true;
        ++g_c.sync_reads;
        if (++g_since_progress > g_spin_cap) spin_guard_trip();
        return sync_value();
    }
    if (addr == REG_IPCFIFOCNT && width == 2) {
        *handled = true;
        return fifocnt_value();
    }
    return 0;
}

bool ipc_reg_write(uint32_t addr, uint64_t value, unsigned width) {
    if (addr == REG_IPCFIFORECV) return true;      // read-only; never mapped
    if (!ipc_enabled()) return false;

    if (addr == REG_IPCSYNC && width == 2) {
        const uint16_t v = static_cast<uint16_t>(value);
        g_sync_arm9_out = (v >> 8) & 0xfu;
        g_sync_ctrl = v & 0x6000u;
        g_since_progress = 0;                      // the handshake moved
        arm7_sync_step();
        ipc_publish();
        return true;
    }
    if (addr == REG_IPCFIFOCNT && width == 2) {
        const uint16_t v = static_cast<uint16_t>(value);
        g_ctrl = static_cast<uint16_t>(v & (FIFOCNT_ENABLE | FIFOCNT_RECV_IRQ |
                                            FIFOCNT_SEND_EMPTY_IRQ));
        if (v & FIFOCNT_SEND_CLEAR) g_send_n = 0;  // bit 3, write-only
        if (v & FIFOCNT_ERROR)      g_error = false;  // bit 14 acks
        log_line("IPCFIFOCNT <- 0x%04x  (enable %u, recv-irq %u, send-irq %u%s%s)",
                 v, (g_ctrl & FIFOCNT_ENABLE) ? 1u : 0u,
                 (g_ctrl & FIFOCNT_RECV_IRQ) ? 1u : 0u,
                 (g_ctrl & FIFOCNT_SEND_EMPTY_IRQ) ? 1u : 0u,
                 (v & FIFOCNT_SEND_CLEAR) ? ", send-clear" : "",
                 (v & FIFOCNT_ERROR) ? ", error-ack" : "");
        ipc_publish();
        // Enabling the receive interrupt with words already queued delivers
        // them, the way the DS raises the IRQ the moment the enable goes up on
        // a non-empty FIFO.
        raise_rx_irq();
        ipc_publish();
        return true;
    }
    if (addr == REG_IPCFIFOSEND && width == 4) {
        fifo_send(static_cast<uint32_t>(value));
        ipc_publish();
        return true;
    }
    return false;
}

IpcCounters ipc_counters() {
    g_c.fifocnt = fifocnt_value();
    return g_c;
}

void ipc_report(const char *tag) {
    const IpcCounters c = ipc_counters();
    std::fprintf(stderr,
        "[ipc:%s] sync=%s steps=%lu reads=%lu send=%lu recv=%lu push=%lu "
        "rx=%lu nak=%lu err=%lu fifocnt=0x%04x arm7flags=0x%08x "
        "arm9flags=0x%08x\n",
        tag ? tag : "-", c.sync_done ? "done" : "PENDING", c.sync_steps,
        c.sync_reads, c.sends, c.recvs, c.arm7_pushes, c.rx_dispatches,
        c.naks, c.errors, c.fifocnt,
        (unsigned)ipc_arm7_flags(), (unsigned)ipc_arm9_flags());
    std::fflush(stderr);
}

}  // namespace ntr
