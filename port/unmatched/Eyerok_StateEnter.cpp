/* HOST COPY of src/func_ov066_02119454.cpp -- the ENTER half of EYEROK's
 * (176, ov066) nineteen-cell pointer-to-member state machine, and its state
 * INSTALLER in the same three statements. Run rel0215, lane cast-ov066.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member on a DELIBERATELY INCOMPLETE class:
 *
 *     struct C; typedef int (C::*PMF)();
 *     struct C { char pad[0x48c]; PMF *pp; };
 *     extern "C" int func_ov066_02119454(C *c, PMF *p)
 *     { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
 *
 * no base class, no virtuals declared. mwcc's PMF there is the ROM's own
 * 8-byte {function, delta} pair -- which is the table's ROM stride, and matches
 * data_ov066_0211afcc..0211b0ec's own `S16 { S8 x, y; }` sinit-copy unit
 * exactly (see src/__sinit_ov066_0211a418.c and port/ov066_syms.txt's header
 * for the full nineteen-cell derivation, ROM-byte verified against
 * extracted/overlays/overlay_0066.bin). MSVC's PMF representation for an
 * INCOMPLETE class is the "unknown inheritance" form, which is not a plain
 * function pointer -- calling through it does not reproduce the ROM's
 * {function, delta} semantics on the host.
 *
 * This is the Ukiki eleven-cell shape (port/unmatched/Ukiki_StateEnter.cpp)
 * with NINETEEN cells, the cell pointer at +0x48c, and one difference worth
 * naming: ov030 split install (func_ov030_021141a8) and enter
 * (func_ov030_02114170) into two bodies, while Eyerok FUSES them. The write to
 * +0x48c happens FIRST, so installing a state IS entering it, and there is no
 * window in which +0x48c is stale.
 *
 * THE NULL GUARD IS THE ROM'S, not an addition: `if (*q == 0) return 1`. mwcc
 * compares a PMF against 0 by testing the FUNCTION word, which is the pair's
 * first word, so the host test is `q->enter_fn == 0`. Nineteen of the ROM's
 * thirty-eight source pairs are non-null and the guard never fires today; it
 * is kept because the ROM keeps it.
 *
 * All nineteen cells are seated with host function pointers by
 * port_eyerok_states_seat() (hal/actor_classes_ov066.cpp) before anything can
 * dispatch through them, and that seat ABORTS if the sinit left bytes other
 * than the ROM's own -- so a wrong mount is loud rather than a jump into
 * unmapped memory.
 *
 * The matched src stays in src/ as the byte proof; this file is the port body,
 * translated statement for statement with the PMF call replaced by a plain
 * function-pointer call through the same struct layout the seat establishes.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function,delta} pair.
 */
extern "C" {

struct PortEyerokCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef int (*PortEyerokFn)(void *);

int func_ov066_02119454(void *cv, void *pv)
{
    char *c = (char *)cv;
    PortEyerokCell *p = (PortEyerokCell *)pv;

    *(PortEyerokCell **)(c + 0x48c) = p;
    {
        PortEyerokCell *q = *(PortEyerokCell **)(c + 0x48c);
        if (q->enter_fn == 0)
            return 1;
        return ((PortEyerokFn)(size_t)q->enter_fn)(c);
    }
}

}  /* extern "C" */
