/* HOST COPY of src/func_ov081_02125488.cpp -- the Cap_StateDispatch/
 * SoundObject treatment applied to MrBlizzard's ten-cell state table.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member on a DELIBERATELY INCOMPLETE class (`struct C { char
 * pad[0x3f8]; PMF *pp; };`, no base class, no virtuals declared) -- mwcc's
 * PMF there is the ROM's own 8-byte {function, delta} pair (the table's
 * ROM stride, matching this file's own sinit-copy unit). MSVC's PMF
 * representation for an INCOMPLETE class is the "unknown inheritance"
 * form, which is not a plain function pointer and not necessarily even
 * the same size -- calling through it does not reproduce the ROM's own
 * {function, delta} semantics at all, and the seated raw host function
 * pointer (hal/actor_classes_ov081.cpp's port_mr_blizzard_states_seat)
 * crashes when dispatched this way. The matched src stays in src/ as the
 * byte proof; this file is the port body, translated field for field with
 * the PMF call replaced by a plain function-pointer call through the same
 * struct layout the seat already establishes
 * (hal/actor_classes_ov081.cpp's PortMrBlizzardPair).
 */
#include <cstddef>   /* size_t (pointer-width casts) */
extern "C" {
struct PortMrBlizzardPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef int (*PortMbFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately
   incomplete class (the SoundObject/Cap/BabyPenguin-state treatment);
   MSVC's PMF representation there does not reproduce the ROM's
   {function,delta} pair. */
int func_ov081_02125488(char *c, PortMrBlizzardPair *p)
{
    *(PortMrBlizzardPair **)(c + 0x3f8) = p;
    PortMrBlizzardPair *q = *(PortMrBlizzardPair **)(c + 0x3f8);
    if (q->enter_fn == 0)
        return 1;
    return ((PortMbFn)(size_t)q->enter_fn)(c);
}
}
