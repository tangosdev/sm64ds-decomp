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
// The ARM7/ARM9 shared block at the top of main RAM. It sits past MAIN_SIZE,
// so it needs its own mapping: the DS puts main RAM at 0x02000000 with this
// system area mirrored up at 0x027ff000, and the game reads it through raw
// literals rather than through a symbol, so nothing else brings it in.
constexpr uintptr_t SHARED_BASE = 0x027ff000u, SHARED_SIZE = 0x00001000u;

// Bring every region up. Idempotent; false if any REQUIRED fixed mapping failed
// (main RAM is the one that is not required -- see io.cpp).
bool io_init();

// Reserve the fixed ranges above. Idempotent, and callable before the CRT has
// initialised, which is the whole point: it runs from a TLS callback at process
// start so the ranges are claimed before the CRT heap, the static initialisers
// and anything this program loads for itself have had a chance to take them.
// io_init() calls it too, so a range lost early still gets a later attempt.
//   stage 1 = the TLS callback, stage 2 = io_init.
void io_reserve(int stage);

// Failure-path reporting. Empty or zero on a healthy start.
//   io_reserve_detail()      one technical block for the log: which ranges were
//                            lost and, where it can be named, the module or
//                            allocation that was sitting in them.
//   io_reserve_player_text() the same failure in plain language, for a player.
//   io_reserve_lost_mask()   bit per lost range, zero when all came up.
//   io_reserve_attempts()    reservation passes run (1 on a clean start).
//   io_reserve_stage()       where the first pass ran, 1 or 2 as above.
//   io_reserve_stage_won()   where the last missing range finally came up.
const char *io_reserve_detail();
const char *io_reserve_player_text();
unsigned io_reserve_lost_mask();
unsigned io_reserve_attempts();
int io_reserve_stage();
int io_reserve_stage_won();

// Host pointer to a DS address. Valid only inside a mapped region.
inline void *host_ptr(uint32_t addr) { return reinterpret_cast<void *>(addr); }

uint64_t io_read(uint32_t addr, unsigned width);
void     io_write(uint32_t addr, uint64_t value, unsigned width);

// How many times the square-root unit has actually been RUN this process.
//
// A REACH COUNTER, and it exists because "no pixel changed" is two different
// findings. Run mg15 lane SQRT routed func_02053274 (the 3D distance function)
// through hostgen and every regression capture came back byte-identical. That
// is only reassuring once you know whether the body RAN at all in those frames:
// a fix that changes nothing because nothing called it, and a fix that changes
// nothing because its answer never flipped a branch, are not the same claim.
// Reading this on both arms of the A/B gives the call count as a DELTA, since a
// plain build never enters run_sqrt and a routed one does. One increment on a
// path that already does a 64-bit integer square root.
unsigned long sqrt_runs();

// Write the geometry-engine-owned half of GXSTAT back into the mapped window:
// the FIFO status and the two MATRIX STACK LEVELS, from the levels ntr/gx.cpp
// is holding. On hardware those bits are driven continuously by the geometry
// engine and no store can change them, so the truthful moment to call this is
// the moment the engine changes what they report -- MTX_PUSH, MTX_POP and the
// per-frame reset, which is where ntr/gx.cpp calls it from.
//
// It exists because the same fixup inside io_read/io_write is not enough. That
// one runs only on accesses routed through this proxy, and the game's two
// stack-level readers (func_02055464 and func_02055490) are built plain -- they
// read the mapped window directly and get whatever the last proxied touch left
// there. Safe before io_init: it brings the mapping up itself and does nothing
// if that fails.
void io_gxstat_publish();

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
