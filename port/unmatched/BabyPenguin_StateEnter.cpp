/* HOST COPY of src/func_ov072_02121d18.cpp -- the SoundObject/Cap/
 * MrBlizzard-state treatment applied to BabyPenguin's six-cell state
 * table.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member on a DELIBERATELY INCOMPLETE class (`struct C { char
 * pad[0x35c]; PMF *pp; };`, no base class, no virtuals declared) --
 * mwcc's PMF there is the ROM's own 8-byte {function, delta} pair (the
 * table's ROM stride, matching data_ov072_02122d6c's own S16 = {S8 enter,
 * S8 tick} sinit-copy unit -- see __sinit_ov072_02122414.c and
 * port/ov072_syms.txt's own header for the full cell derivation, ROM-byte
 * verified against extracted/overlays/overlay_0072.bin). MSVC's PMF
 * representation for an INCOMPLETE class is the "unknown inheritance"
 * form, which is not a plain function pointer -- calling through it does
 * not reproduce the ROM's own {function, delta} semantics on the host.
 * The matched src stays in src/ as the byte proof; this file is the port
 * body, translated field for field with the PMF call replaced by a plain
 * function-pointer call through the same struct layout the seat
 * establishes (hal/actor_classes_ov072.cpp's PortBabyPenguinPair).
 *
 * func_ov072_02121d50.c (the cell-pointer installer, unchanged, stays in
 * the slice) writes `*(char**)(c + 0x35c) = data_ov072_02122d6c + (i<<4)`
 * before calling this -- so `c->pp` (at +0x35c) already points at the
 * seated cell by the time this runs. This is the "enter" half (pp+0); the
 * "tick" half (pp+1) is port/unmatched/BabyPenguin_StateTick.cpp.
 */
#include <cstddef>   /* size_t (pointer-width casts) */
extern "C" {
struct PortBabyPenguinPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef void (*PortBpFn)(void *);

/* No null guard, matching the matched src exactly (C::pp is always seated
 * by func_ov072_02121d50 before this can be reached, and the ROM body
 * itself is unconditional -- see the header for the full derivation). */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately
   incomplete class (the SoundObject/Cap/MrBlizzard-state treatment);
   MSVC's PMF representation there does not reproduce the ROM's
   {function,delta} pair. */
void func_ov072_02121d18(char *c)
{
    PortBabyPenguinPair *p = *(PortBabyPenguinPair **)(c + 0x35c);
    ((PortBpFn)(size_t)p->enter_fn)(c);
}
}
