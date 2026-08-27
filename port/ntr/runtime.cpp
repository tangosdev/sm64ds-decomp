// Host implementations of the decomp's ARM assembly primitives.
//
// build/host-src/asm-shims.txt lists all 115 of them; this file covers the ones
// reached so far. They are matched, correct ARM -- and unusable off-hardware, so
// each needs a host equivalent with the *same name and linkage* as the decomp
// file it stands in for.
//
// Linkage matters and is not uniform in the decomp:
//   src/game/objects/VBlankIntrWait.c        `asm void VBlankIntrWait(void)`      -> C++ linkage
//   src/game/actors/IRQ/_ZN3IRQ6EnableEv.cpp    `extern "C" asm unsigned int ...`    -> C linkage
// The decomp's filenames are already Itanium mangled names, so a .c file built as
// C++ gets mangled a second time. Match whichever form the original used, or the
// link fails in a way that looks like a missing symbol rather than a mismatch.

#include "ntr/rt.h"

#include "ntr/gx.h"

#include <cstring>

// ---------------------------------------------------------------------------
// Interrupt control. The ARM originals set/clear the CPSR I bit and return the
// previous masked state (0x80 = was disabled).
// ---------------------------------------------------------------------------

extern "C" unsigned int _ZN3IRQ7DisableEv(void) { return ntr::rt_irq_disable(); }
extern "C" unsigned int _ZN3IRQ6EnableEv(void) { return ntr::rt_irq_enable(); }
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
extern "C" void Copy36Bytes(int *src, int *dst) {
    copy_words(reinterpret_cast<const uint32_t *>(src), reinterpret_cast<uint32_t *>(dst), 9);
}

// stmia WITHOUT writeback: twelve words through the same port address. Only
// ever used against the FIFO; for a memory destination the DS semantics would
// overwrite the same three words four times, so a memory dst is a bug here.
extern "C" void Copy48BytesFixed(int *src, int *dst) {
    if (reinterpret_cast<uintptr_t>(dst) == 0x04000400u) {
        for (int i = 0; i < 12; ++i) ntr::gx_write_fifo(static_cast<uint32_t>(src[i]));
    } else {
        for (int i = 0; i < 12; ++i) dst[i] = src[i];
    }
}

// The FIFO flush primitive: 32 no-writeback stmia of four zeroed registers,
// 128 NOP command words that push any partially-packed command through.
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
// storage for the game's DMA bookkeeping (BSS on the DS)
int data_020a6460[8];                                   /* GX-DMA state */
struct { unsigned handler, active, arg; } data_020a60c4[8]; /* per-channel cbs */
}

namespace {
unsigned g_ie;                      // IE word stand-in
void (*g_gxfifo_handler)(void);     // handler for mask 0x200000
}  // namespace

extern "C" void *_ZN3IRQ13GetIRQHandlerEj(unsigned mask) {
    return mask == 0x200000u ? reinterpret_cast<void *>(g_gxfifo_handler) : nullptr;
}
extern "C" void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned mask, void (*h)(void)) {
    if (mask == 0x200000u) g_gxfifo_handler = h;
}
extern "C" unsigned _ZN3IRQ10EnableIRQsEj(unsigned mask) {
    const unsigned prev = g_ie;
    g_ie |= mask;
    return prev;
}
extern "C" unsigned _ZN3IRQ11DisableIRQsEj(unsigned mask) {
    const unsigned prev = g_ie;
    g_ie &= ~mask;
    return prev;
}
extern "C" void _ZN3IRQ15ClearInterruptsEj(unsigned) {}

// DMA to the FIFO is the display-list path (func_0205a290). ctrl bit 30 is
// IRQ-on-complete (the final chunk); a GXFIFO-destined chunk without it relies
// on the half-empty IRQ to pump the next chunk.
extern "C" void DMAStartTransfer(int ch, int src, int dst, int ctrl) {
    const int words = ctrl & 0x1FFFFF;
    copy_words(reinterpret_cast<const uint32_t *>(static_cast<uintptr_t>(src)),
               reinterpret_cast<uint32_t *>(static_cast<uintptr_t>(dst)), words);
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
// ---------------------------------------------------------------------------

extern "C" void _ZN4CP1514FlushDataCacheEv(void) {}
extern "C" void _ZN4CP1514FlushDataCacheEjj(unsigned int, unsigned int) {}
extern "C" void _ZN4CP1516DrainWriteBufferEv(void) {}
extern "C" void _ZN4CP1519InvalidateDataCacheEjj(unsigned int, unsigned int) {}
extern "C" void _ZN4CP1526InvalidateInstructionCacheEjj(unsigned int, unsigned int) {}
extern "C" void _ZN4CP1527FlushAndInvalidateDataCacheEv(void) {}
extern "C" void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int, unsigned int) {}
