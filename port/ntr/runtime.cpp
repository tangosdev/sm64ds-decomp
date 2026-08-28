// Host implementations of the decomp's ARM assembly primitives.
//
// build/host-src/asm-shims.txt lists all 115 of them; this file covers the ones
// reached so far. They are matched, correct ARM -- and unusable off-hardware, so
// each needs a host equivalent with the *same name and linkage* as the decomp
// file it stands in for.
//
// Linkage matters and is not uniform in the decomp:
//   src/VBlankIntrWait.c        `asm void VBlankIntrWait(void)`      -> C++ linkage
//   src/_ZN3IRQ6EnableEv.cpp    `extern "C" asm unsigned int ...`    -> C linkage
// The decomp's filenames are already Itanium mangled names, so a .c file built as
// C++ gets mangled a second time. Match whichever form the original used, or the
// link fails in a way that looks like a missing symbol rather than a mismatch.

#include "ntr/rt.h"

#include "ntr/gx.h"

#include <cstring>
#include "hal/dsstate_seg.h"

// ---------------------------------------------------------------------------
// Interrupt control. The ARM originals set/clear the CPSR I bit and return the
// previous masked state (0x80 = was disabled).
// ---------------------------------------------------------------------------

// PORT_HOST_ABI: hand-asm primitive (src/ carries the banner): mrs/msr on
//   the CPSR I bit. See the Interrupt control block above.
extern "C" unsigned int _ZN3IRQ7DisableEv(void) { return ntr::rt_irq_disable(); }
extern "C" unsigned int _ZN3IRQ6EnableEv(void) { return ntr::rt_irq_enable(); }
// PORT_HOST_ABI: hand-asm primitive (src/ carries the banner): mrs/msr on
//   the CPSR I bit. See the Interrupt control block above.
extern "C" unsigned int _ZN3IRQ7RestoreEj(unsigned int prev) {
    return ntr::rt_irq_restore(prev);
}

// ---------------------------------------------------------------------------
// BIOS SWI wrappers. These are .c files in the decomp, so C++ linkage.
// ---------------------------------------------------------------------------

void VBlankIntrWait(void) { ntr::rt_vblank_wait(); }

// swi 0x04. The game only ever waits on VBlank in practice.
void IntrWait(void) { ntr::rt_vblank_wait(); }

// swi 0x06 -- halt until an interrupt. Same thing here.
void Halt(void) { ntr::rt_vblank_wait(); }

// swi 0x03 -- burn `count` loop iterations. Nothing to wait for on a host.
void WaitByLoop(void) {}

// swi 0x0B/0x0C. The DS BIOS copies in 4- or 32-byte units, with an optional
// fixed-source fill mode; bit 24 selects fill, bit 26 selects 32-bit units.
extern "C" void CpuSet(const void *src, void *dst, unsigned int ctrl) {
    const unsigned int count = ctrl & 0x1FFFFF;
    const bool fill = (ctrl >> 24) & 1;
    const bool word = (ctrl >> 26) & 1;
    if (word) {
        const uint32_t *s = static_cast<const uint32_t *>(src);
        uint32_t *d = static_cast<uint32_t *>(dst);
        for (unsigned int i = 0; i < count; ++i) d[i] = fill ? *s : s[i];
    } else {
        const uint16_t *s = static_cast<const uint16_t *>(src);
        uint16_t *d = static_cast<uint16_t *>(dst);
        for (unsigned int i = 0; i < count; ++i) d[i] = fill ? *s : s[i];
    }
}

extern "C" void CpuFastSet(const void *src, void *dst, unsigned int ctrl) {
    const unsigned int count = ctrl & 0x1FFFFF;
    const bool fill = (ctrl >> 24) & 1;
    const uint32_t *s = static_cast<const uint32_t *>(src);
    uint32_t *d = static_cast<uint32_t *>(dst);
    for (unsigned int i = 0; i < count; ++i) d[i] = fill ? *s : s[i];
}

// ---------------------------------------------------------------------------
// Block copies. These matter more than they look: the geometry FIFO is fed by
// handing 0x4000400 to a copy routine --
//
//     *(volatile u32 *)0x4000400 = 0x1a;   Copy36Bytes(m, (int *)0x4000400);
//
// -- so the FIFO address never appears as a dereference the transform can see.
// docs/mmio-inventory.md calls that the `address-passed` class, and this is where
// it resolves: the copy routines are themselves ARM asm primitives, so they need
// host shims anyway, and the shim is the natural place to notice the destination.
// ---------------------------------------------------------------------------

namespace {

constexpr uint32_t GXFIFO = 0x04000400u;

void copy_words(const uint32_t *src, uint32_t *dst, int n) {
    if (reinterpret_cast<uintptr_t>(dst) == GXFIFO) {
        for (int i = 0; i < n; ++i) ntr::gx_write_fifo(src[i]);
    } else {
        for (int i = 0; i < n; ++i) dst[i] = src[i];
    }
}

}  // namespace

// C linkage: in this port the decomp's .c files compile as C TUs (hostgen
// wraps its transformed copies in extern "C"), so the copy primitives must
// carry C names.
// PORT_HOST_ABI: ARM asm primitive (ldmia/stmia) AND the GXFIFO-address seam
//                (0x04000400 routes to gx_write_fifo, not a memory store).
extern "C" void Copy36Bytes(int *src, int *dst) {
    copy_words(reinterpret_cast<const uint32_t *>(src), reinterpret_cast<uint32_t *>(dst), 9);
}

// stmia WITHOUT writeback: twelve words through the same port address. Only
// ever used against the FIFO; for a memory destination the DS semantics would
// overwrite the same three words four times, so a memory dst is a bug here.
// PORT_HOST_ABI: ARM asm primitive AND the GXFIFO-address seam (0x04000400).
extern "C" void Copy48BytesFixed(int *src, int *dst) {
    if (reinterpret_cast<uintptr_t>(dst) == 0x04000400u) {
        for (int i = 0; i < 12; ++i) ntr::gx_write_fifo(static_cast<uint32_t>(src[i]));
    } else {
        for (int i = 0; i < 12; ++i) dst[i] = src[i];
    }
}

// The FIFO flush primitive: 32 no-writeback stmia of four zeroed registers,
// 128 NOP command words that push any partially-packed command through.
// PORT_HOST_ABI: hand-asm primitive (banner-marked in src/), a raw stmia
// loop into the GXFIFO port; the ntr layer models the flush, not the
// instruction stream. Tag added at the wave-3 close after a Scene seat gave
// it its first caller and the untagged body surfaced as the queue's only
// regression.
extern "C" void func_020553c0(unsigned addr) {
    if (addr == 0x04000400u)
        for (int i = 0; i < 128; ++i) ntr::gx_write_fifo(0);
}

// ---------------------------------------------------------------------------
// IRQ handler registry + synthetic delivery. The display-list pump
// (func_0205a358/func_0205a290) is interrupt-driven on hardware: GXFIFO-timed
// DMA chunks re-arm on the FIFO-half-empty IRQ (IE bit 21, mask 0x200000) and
// the final chunk raises the DMA-channel IRQ, whose OS dispatcher calls the
// callback registered through func_02056e98. On host the copies are
// synchronous and the FIFO is always empty, so DMAStartTransfer delivers
// those interrupts inline right after the copy; the recursion terminates
// because the pump's re-entry finds no bytes remaining.
// ---------------------------------------------------------------------------

extern "C" {
// storage for the game's DMA bookkeeping (BSS on the DS). These two are the
// only DS globals this host library owns, and they are real save state rather
// than host bookkeeping: the callback table holds the handlers the game itself
// registered through func_02056e98. Everything else in this file (the window,
// frame pacing, the IE stand-in below) deliberately stays out of the capture.
// See hal/dsstate_seg.h.
DSSTATE_BEGIN
int data_020a6460[8];                                   /* GX-DMA state */
struct { unsigned handler, active, arg; } data_020a60c4[8]; /* per-channel cbs */
DSSTATE_END
}

namespace {
unsigned g_ie;                      // IE word stand-in
void (*g_gxfifo_handler)(void);     // handler for mask 0x200000
void (*g_hblank_handler)(void);     // handler for mask 0x2, the HBlank edge

// The two DS registers the HBlank gate reads. Both are ordinary latches in the
// mapped I/O window (ntr/mmio.h mechanism 1), written by the ROM's own arming
// code: IME by func_0202f708's save/restore bracket, DISPSTAT bit 4 by
// func_02053c10.
constexpr uintptr_t REG_IME = 0x04000208u;
constexpr uintptr_t REG_DISPSTAT = 0x04000004u;
constexpr unsigned DISPSTAT_HBLANK_IRQ_ENABLE = 0x10u;
}  // namespace

// PORT_HOST_ABI: src walks the DS IRQ vector tables (data_02099fe4,
//   data_020a60c4); the host models the handlers it dispatches.
extern "C" void *_ZN3IRQ13GetIRQHandlerEj(unsigned mask) {
    if (mask == 0x200000u) return reinterpret_cast<void *>(g_gxfifo_handler);
    if (mask == ntr::IRQ_HBLANK) return reinterpret_cast<void *>(g_hblank_handler);
    return nullptr;
}
// PORT_HOST_ABI: src walks the DS IRQ vector tables (data_02099fe4,
//   data_020a60c4); the host models the handlers it dispatches.
//
// TWO MASKS ARE MODELLED, and the second one is why the fade can move. Mask
// 0x200000 is the geometry FIFO, delivered synthetically from DMAStartTransfer
// below. Mask 2 is HBlank: the dWipe_c setters install func_0202f2c4 on it and
// the scanline sweep in rt.cpp delivers it. Every other mask is still dropped
// on the floor, deliberately -- a handler this layer never raises is better
// stored nowhere than stored and silently never run.
extern "C" void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned mask, void (*h)(void)) {
    if (mask == 0x200000u) g_gxfifo_handler = h;
    else if (mask == ntr::IRQ_HBLANK) g_hblank_handler = h;
}

namespace ntr {

// The five gates the DS applies before an HBlank IRQ reaches the handler,
// reported one bit each so a closed gate can be NAMED rather than guessed at.
// All five have a ROM writer on this path, which is what makes the disarm
// work: func_0202fb30 clears IE bit 1, clears DISPSTAT bit 4 and nulls the
// handler, and any one of the three closes this.
unsigned rt_hblank_gates() {
    unsigned g = 0;
    if (g_hblank_handler) g |= HBLANK_GATE_HANDLER;
    if (g_ie & IRQ_HBLANK) g |= HBLANK_GATE_IE;
    if (!rt_irq_masked()) g |= HBLANK_GATE_CPSR;
    if (*reinterpret_cast<volatile uint16_t *>(REG_IME) & 1u) g |= HBLANK_GATE_IME;
    if (*reinterpret_cast<volatile uint16_t *>(REG_DISPSTAT) &
        DISPSTAT_HBLANK_IRQ_ENABLE)
        g |= HBLANK_GATE_DISPSTAT;
    return g;
}

bool rt_hblank_armed() { return rt_hblank_gates() == HBLANK_GATE_ALL; }

void rt_hblank_dispatch() { g_hblank_handler(); }

}  // namespace ntr
// PORT_HOST_ABI: src pokes the DS interrupt registers (IME 0x4000208, IE
//   0x4000210); the host keeps the IE word stand-in above.
extern "C" unsigned _ZN3IRQ10EnableIRQsEj(unsigned mask) {
    const unsigned prev = g_ie;
    g_ie |= mask;
    return prev;
}
// PORT_HOST_ABI: src pokes the DS interrupt registers (IME 0x4000208, IE
//   0x4000210); the host keeps the IE word stand-in above.
extern "C" unsigned _ZN3IRQ11DisableIRQsEj(unsigned mask) {
    const unsigned prev = g_ie;
    g_ie &= ~mask;
    return prev;
}
// PORT_HOST_ABI: src pokes the DS interrupt registers (IME 0x4000208, IF
//   0x4000214), which the ntr layer does not model.
extern "C" void _ZN3IRQ15ClearInterruptsEj(unsigned) {}

// DMA to the FIFO is the display-list path (func_0205a290). ctrl bit 30 is
// IRQ-on-complete (the final chunk); a GXFIFO-destined chunk without it relies
// on the half-empty IRQ to pump the next chunk.
// PORT_HOST_ABI: src pokes DS DMA registers (REG_DMA_BASE); the host models the
//                FIFO seam and synthesises the completion IRQ instead.
extern "C" void DMAStartTransfer(int ch, int src, int dst, int ctrl) {
    const int words = ctrl & 0x1FFFFF;
    /* CONTROL BIT 24 IS DMA_CONTROL_SRC_FIXED: the source address does NOT
       advance, so the transfer REPEATS one word. That is what a FILL is, and
       it is what DMASyncFillTransfer asks for.
       Ignoring it made every fill a memcpy FROM THE SOURCE ADDRESS ONWARDS,
       and on this hardware model the source it is handed is the mapped MMIO
       window at 0x040000EC -- so a fill walked the live I/O registers into
       its destination instead of clearing it.
       On the title that destination is the two OAM shadow buffers, every
       frame, and the 3D geometry-port latches landed in entries 64..127. All
       three of the owner's visual complaints are that one memcpy:
         * the bottom-screen star correct on half the frames and MASSIVE and
           rotated 90 degrees on the others -- garbage attr0 rotation/scale
           and double-size bits, alternating with the buffer swap;
         * identical corruption top-left of BOTH screens -- same source
           address feeding both engines;
         * random glitched LETTERS there on the attract's Yoshi switch --
           garbage attr2 tile index pointing into the swapped character sheet.
       Four other live callers (func_02053c40, func_020554bc, func_020616e8
       and the title's own path) were doing the same MMIO memcpy; honouring
       the bit heals all of them, because it is the primitive that was wrong
       rather than any one caller. */
    if (ctrl & 0x01000000) {
        const uint32_t v =
            *reinterpret_cast<const uint32_t *>(static_cast<uintptr_t>(src));
        uint32_t *d = reinterpret_cast<uint32_t *>(static_cast<uintptr_t>(dst));
        if (static_cast<uintptr_t>(dst) == GXFIFO) {
            for (int i = 0; i < words; ++i) ntr::gx_write_fifo(v);
        } else {
            for (int i = 0; i < words; ++i) d[i] = v;
        }
    } else {
    copy_words(reinterpret_cast<const uint32_t *>(static_cast<uintptr_t>(src)),
               reinterpret_cast<uint32_t *>(static_cast<uintptr_t>(dst)), words);
    }
    if (static_cast<uintptr_t>(dst) == 0x04000400u) {
        if (ctrl & 0x40000000) {
            const unsigned h = data_020a60c4[ch & 7].handler;
            if (h) reinterpret_cast<void (*)(unsigned)>(h)(data_020a60c4[ch & 7].arg);
        } else if (g_ie & 0x200000u) {
            if (g_gxfifo_handler) g_gxfifo_handler();
        }
    }
}

// ---------------------------------------------------------------------------
// CP15 cache maintenance. The host has coherent memory, so these are no-ops --
// but they must still exist, because the decomp calls them around every DMA.
//
// EVERY ONE OF THESE HAS A MATCHED src TU AND ONLY THE LINKED ONES ARE QUEUE
// ROWS, which is why the tags below look scattered rather than uniform.
// port/tools/linkage.py only sees a definition once /OPT:REF keeps it, so an
// untagged sibling here is dormant and not forgiven: the day a slice reaches
// it, it surfaces as an UNDOCUMENTED SHADOW and wants the same one-line ruling
// the tagged ones carry. Run link60 lane PC2 hit exactly that. Its new arm9
// chain (func_ov007_020b2bd4 -> func_02044efc / func_02045ef8) linked
// _ZN4CP1514FlushDataCacheEv for the first time, the queue went 557 to 558 and
// SHADOWS 43 to 44, and the answer was the tag and not a code change: the
// matched TU is an `asm` block of mcr p15 instructions, which is DS hardware
// the ntr layer does not model, so the host definition IS the faithful
// stand-in.
// ---------------------------------------------------------------------------

// PORT_HOST_ABI: hand-asm primitive (src/ carries the banner): mcr p15
//   cache maintenance, a cache-flush primitive the host runtime necessarily
//   owns. See the CP15 block above -- host memory is coherent.
extern "C" void _ZN4CP1514FlushDataCacheEv(void) {}
// PORT_HOST_ABI: hand-asm primitive (src/ carries the banner): mcr p15
//   cache maintenance. See the CP15 block above -- host memory is coherent.
extern "C" void _ZN4CP1514FlushDataCacheEjj(unsigned int, unsigned int) {}
extern "C" void _ZN4CP1516DrainWriteBufferEv(void) {}
// PORT_HOST_ABI: hand-asm primitive (src/ carries the banner): mcr p15
//   cache maintenance. See the CP15 block above -- host memory is coherent.
extern "C" void _ZN4CP1519InvalidateDataCacheEjj(unsigned int, unsigned int) {}
extern "C" void _ZN4CP1526InvalidateInstructionCacheEjj(unsigned int, unsigned int) {}
extern "C" void _ZN4CP1527FlushAndInvalidateDataCacheEv(void) {}
// PORT_HOST_ABI: hand-asm primitive (src/ carries the banner): mcr p15
//   cache maintenance. See the CP15 block above -- host memory is coherent.
extern "C" void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int, unsigned int) {}
