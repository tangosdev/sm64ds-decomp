// Host-side DS memory-mapped I/O.
//
// Two mechanisms, deliberately:
//
//   1. Real memory is mapped at the DS I/O addresses (see port/io.cpp). Any access
//      pattern the transform does not rewrite -- a pointer bound to a register and
//      dereferenced later, a struct overlay, DMA -- still lands somewhere valid and
//      latches correctly. Most DS registers really are just latches.
//
//   2. Registers whose *write* triggers hardware behaviour (the divider starting a
//      division, the geometry FIFO consuming a command) are routed through this
//      proxy by tools/hostgen.py, so the side effect actually happens.
//
// Mechanism 1 keeps the port from crashing on the long tail; mechanism 2 is what
// makes it correct. See notes/assessment.md section 2a.

#ifndef NTR_MMIO_H
#define NTR_MMIO_H

#include <stdint.h>

namespace ntr {

// DS regions, mapped at their real addresses on the host. All below 2GB so that
// truncating 32-bit round-trips -- `int base = (int)&file`, which the BMD/BCA/KCL
// loaders do in place -- stay exact.
constexpr uintptr_t MAIN_BASE = 0x02000000u, MAIN_SIZE = 0x00400000u;  // main RAM
constexpr uintptr_t IO_BASE   = 0x04000000u, IO_SIZE   = 0x00002000u;  // 2D/3D/system
constexpr uintptr_t PLTT_BASE = 0x05000000u, PLTT_SIZE = 0x00000800u;  // palettes
constexpr uintptr_t VRAM_BASE = 0x06000000u, VRAM_SIZE = 0x00900000u;  // all banks
constexpr uintptr_t OAM_BASE  = 0x07000000u, OAM_SIZE  = 0x00000800u;  // sprites

// Bring every region up. Idempotent; false if any fixed mapping failed.
bool io_init();

// Host pointer to a DS address. Valid only inside a mapped region.
inline void *host_ptr(uint32_t addr) { return reinterpret_cast<void *>(addr); }

uint64_t io_read(uint32_t addr, unsigned width);
void     io_write(uint32_t addr, uint64_t value, unsigned width);

// Proxy standing in for `*(volatile T *)addr`. Reads and writes are dispatched so
// that write-triggered registers run their side effect.
template <class T>
struct Reg {
    uint32_t addr;
    explicit Reg(uint32_t a) : addr(a) {}

    operator T() const { return static_cast<T>(io_read(addr, sizeof(T))); }

    // Non-const on purpose: these are called on temporaries produced by the macro.
    Reg &operator=(T v) { io_write(addr, static_cast<uint64_t>(v), sizeof(T)); return *this; }
    Reg &operator|=(T v) { return *this = static_cast<T>(static_cast<T>(*this) | v); }
    Reg &operator&=(T v) { return *this = static_cast<T>(static_cast<T>(*this) & v); }
    Reg &operator^=(T v) { return *this = static_cast<T>(static_cast<T>(*this) ^ v); }
};

}  // namespace ntr

// What tools/hostgen.py rewrites `*(volatile T *)0xADDR` into.
#define NTR_MMIO(T, A) (::ntr::Reg<T>((A)))

#endif  // NTR_MMIO_H
