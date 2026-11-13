// Host runtime: the frame clock, the interrupt state, and the thing that makes
// `VBlankIntrWait()` mean something off-hardware.
//
// A DS game is not a tick function. It runs a loop that blocks in the middle on
// VBlank, and 11,000 decompiled functions assume exactly that shape. Restructuring
// them into something a host frame loop can call is not on the table.
//
// So the game runs on its own fiber. VBlankIntrWait switches back to the host
// fiber, which ends the frame -- scan out, present, advance the clock -- and
// switches back. Game control flow is untouched, and nothing races, because only
// one fiber is ever running. The same mechanism generalises to OS_SleepThread /
// OS_WakeupThread when those are needed.

#ifndef NTR_RT_H
#define NTR_RT_H

#include <stdint.h>

namespace ntr {

// DS interrupt bits, as they appear in IE/IF (0x4000210 / 0x4000214).
enum : uint32_t {
    IRQ_VBLANK = 1u << 0,
    IRQ_HBLANK = 1u << 1,
    IRQ_VCOUNT = 1u << 2,
};

// Called once per frame, after the game blocks in VBlankIntrWait and before it
// resumes. Return false to stop the loop.
using FrameHook = bool (*)(uint64_t frame);

// Run `game` on its own fiber until it returns or `hook` asks to stop.
// Returns the number of completed frames.
uint64_t rt_run(void (*game)(), FrameHook hook, uint64_t max_frames = 0);

// Block until the next VBlank. Called from game code (via the VBlankIntrWait
// shim). Yields to the host fiber; returns when the frame has been presented.
void rt_vblank_wait();

uint64_t rt_frame();

// CPSR I-bit equivalent. Mirrors IRQ::Disable / Enable / Restore, which return
// the *previous* masked state (0x80 when interrupts were disabled).
uint32_t rt_irq_disable();
uint32_t rt_irq_enable();
uint32_t rt_irq_restore(uint32_t prev);
bool rt_irq_masked();

// ---- THE HBLANK EDGE -------------------------------------------------------
//
// IRQ 2 is a MASK and not an ordinal: IE/IF bit 1, which is HBlank. The ROM's
// dWipe_c motion path arms it with SetIRQHandler(2, func_0202f2c4),
// EnableIRQs(2) and func_02053c10(1) (DISPSTAT bit 4, HBLANK IRQ ENABLE), and
// the handler it installs programs the next scanline's window bounds out of a
// 192-line table. Nothing on the host used to raise it, so the table was built
// every fade and read never. See port/irq2_map.txt for the derivation.
//
// The delivery lives in the ntr layer beside the one interrupt path it already
// had (runtime.cpp's GXFIFO handler for mask 0x200000) rather than in a fader
// seam, because an interrupt is not a fader: any handler registered for mask 2
// gets it.

// The five gates the DS applies to a mask-2 edge, one bit each. Reported
// separately from the verdict so a lane that finds the fade still frozen can
// NAME the gate that is shut instead of re-deriving the chain.
enum : unsigned {
    HBLANK_GATE_HANDLER  = 1u << 0,  // SetIRQHandler(2, ...) ran
    HBLANK_GATE_IE       = 1u << 1,  // EnableIRQs(2)
    HBLANK_GATE_CPSR     = 1u << 2,  // the CPSR I bit is clear
    HBLANK_GATE_IME      = 1u << 3,  // IME 0x4000208 bit 0
    HBLANK_GATE_DISPSTAT = 1u << 4,  // DISPSTAT 0x4000004 bit 4, func_02053c10
    HBLANK_GATE_ALL      = 0x1fu,
};
unsigned rt_hblank_gates();

// Is the mask-2 edge armed right now? Re-asked per scanline, because a handler
// may disarm itself mid-frame (func_0202fb30 does).
bool rt_hblank_armed();

// Call the registered mask-2 handler. Undefined unless rt_hblank_armed().
void rt_hblank_dispatch();

// DS TIMER 0 (run hunt2, lane HUD2). The ROM programs it once at boot
// (src/unnamed/arm9/0205/func_02059788.c: TM0CNT_L = 0, TM0CNT_H = 0xc1 = run, IRQ on overflow,
// prescaler F/64) and reads it as the OS tick (src/unnamed/arm9/0205/func_02059650.c: the
// counter at 0x04000100 plus the overflow count its mask-8 handler keeps).
// rt_timer0_advance moves it across `vblanks` DS frames of 560190 bus cycles
// (263 lines x 2130) and raises the overflow interrupt the way the DS does.
// The frame loops call it once per game tick with the ROM's own vblanks-per-
// tick word (data_0208ee44), so a course timer counts DS time.
// SM64DS_TIMER0_OFF=1 leaves the timer frozen (the pre-fix behaviour).
void rt_timer0_advance(unsigned vblanks);
bool rt_timer0_irq_gates_open();   // IE bit 3, IME, CPSR I (runtime.cpp)
void rt_timer0_dispatch();         // the vector at bit 3 (runtime.cpp)

// One frame of display scan-out: walk VCOUNT across all 263 lines and raise
// the HBlank edge on each. rt_run does this itself between the game fiber
// yielding and the frame hook; the walk_window level loop and the scene loop
// run their own frame clocks and call this directly, at the same point in the
// frame -- after the game's behaviour work, before the host rasterises.
void rt_scanout_frame();

// THE WINDOW REGISTERS, LINE BY LINE (run hunt1, lane HUNTHW). A mask-2 handler
// (the ROM's dWipe_c, func_0202f2c4) rewrites WIN0H / WIN1H on every HBlank, so
// the 192 visible lines each have their own horizontal window edges. The last
// rt_scanout_frame recorded them: rows[y] is the WIN1H:WIN0H word the 2D unit
// had while drawing line y (line 0 = the value when the scan began, line y + 1 =
// the value after the HBlank at VCOUNT y). Returns false, leaving *rows alone,
// on a frame on which no handler ran on lines 0..190: the registers were then
// constant for the whole frame and the one-rectangle read is exact. engine 0 is
// the main engine (0x04000040), 1 the sub engine (0x04001040).
bool rt_window_rows(int engine, const uint32_t **rows);
// MGEXIT1: while a one-screen menu recipe is live the per-line window rows are not applied to the two source screens (the recipe moves their pieces;
// hal/menu_compose.cpp cuts the finished picture instead, ntr::wipe_cut_picture).
void set_wipe_suppress(bool on);
bool wipe_suppressed();
bool wipe_cut_picture(uint32_t *dst, int w, int h, int stride);

// The DS's power-on interrupt state (IME set, IE carrying VBlank). It stands
// in for src/unnamed/arm9/0201/func_0201a054.c, the game's own IRQ init, which is in no slice.
// Idempotent. rt_run calls it; a frame loop that does not run on the fiber
// must call it before the first tick, because the ROM's arming code SAVES AND
// RESTORES IME and therefore cannot set it.
void rt_irq_boot_state();

// What the last scan-out did, cumulative over the process.
//   deliveries    handler entries
//   window_writes deliveries after which WIN0H/WIN1H changed value, i.e. the
//                 ones that read the motion table and programmed a scanline
void rt_hblank_counters(unsigned long long *deliveries,
                        unsigned long long *window_writes);

}  // namespace ntr

#endif  // NTR_RT_H
