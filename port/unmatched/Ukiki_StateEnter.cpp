/* HOST COPY of src/func_ov030_02114170.cpp -- the ENTER half of the Ukiki's
 * eleven-cell pointer-to-member state machine. Run rel0215, lane cast-ov030.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member on a DELIBERATELY INCOMPLETE class:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct C { char pad[0x3a4]; PMF *pp; };
 *     extern "C" void func_ov030_02114170(C *c) { PMF *p = c->pp; (c->**p)(); }
 *
 * no base class, no virtuals declared. mwcc's PMF there is the ROM's own
 * 8-byte {function, delta} pair -- which is the table's ROM stride, and matches
 * data_ov030_02115e0c's own `S16 { S8 lo, hi; }` sinit-copy unit exactly (see
 * src/__sinit_ov030_02114924.c and port/ov030_syms.txt's header for the full
 * cell derivation, ROM-byte verified against extracted/overlays/overlay_0030
 * .bin). MSVC's PMF representation for an INCOMPLETE class is the "unknown
 * inheritance" form, which is not a plain function pointer -- calling through
 * it does not reproduce the ROM's {function, delta} semantics on the host.
 *
 * This is the BabyPenguin six-cell shape (port/unmatched/BabyPenguin_StateEnter
 * .cpp) with ELEVEN cells and the cell pointer at +0x3a4 instead of +0x35c.
 *
 * The matched src stays in src/ as the byte proof; this file is the port body,
 * translated statement for statement with the PMF call replaced by a plain
 * function-pointer call through the same struct layout the seat establishes
 * (hal/actor_classes_ov030.cpp's PortUkikiCell).
 *
 * src/func_ov030_021141a8.c (the cell-pointer installer, UNCHANGED, stays in
 * the slice) writes `*(Item16 **)(self + 0x3a4) = &data_ov030_02115e0c[idx]`
 * before anything can reach here -- so `c->pp` already points at the seated
 * cell. This is the ENTER half (pp+0, the sinit's `.lo`); the TICK half (pp+1,
 * the sinit's `.hi`) is port/unmatched/Ukiki_StateTick.cpp.
 */
extern "C" {
struct PortUkikiCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef void (*PortUkikiFn)(void *);

/* No null guard, matching the matched src exactly: +0x3a4 is always seated by
 * func_ov030_021141a8 before this can be reached, and the ROM body itself is
 * unconditional. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
   class; MSVC's PMF representation there does not reproduce the ROM's
   {function,delta} pair. */
void func_ov030_02114170(char *c)
{
    PortUkikiCell *p = *(PortUkikiCell **)(c + 0x3a4);
    ((PortUkikiFn)(size_t)p->enter_fn)(c);
}
}
