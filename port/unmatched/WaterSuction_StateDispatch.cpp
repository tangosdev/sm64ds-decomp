/* HOST COPY of src/func_ov026_021122d4.cpp -- WATERSUCTION's state-enter
 * dispatcher (id 247, daWater_Suikomi_c, ov026). Identical shape to
 * Whirlpool_StateDispatch.cpp one class over, and host-copied for the same
 * reason; the only differences are the member offset (0x30c, not 0x110) and
 * which cell the class points at.
 *
 * WHY A HOST COPY. The matched src is
 *
 *     struct C; typedef int (C::*PMF)();
 *     struct C { char pad[0x30c]; PMF *pp; };
 *     int func_ov026_021122d4(C *c, PMF *p)
 *     { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
 *
 * a real C++ pointer-to-member call on a forward-declared class, which MSVC
 * gives the generalised multi-word representation. The ROM's record is the
 * 8-byte Itanium {function, adjustment} pair. The matched src stays in src/ as
 * the byte proof.
 *
 * THE CELL. One state cell, data_ov026_02113f58 in bss, built by
 * __sinit_ov026_02112d68 from the two source pairs at 0x02113dd0/0x02113dd8:
 * enter 0x021122cc at +0, tick 0x021122b0 at +8. Both are seated with host
 * addresses by hal/actor_classes_ov026.cpp's port_ov026_states_seat() before
 * that sinit runs. The cell POINTER lives at +0x30c, where
 * _ZN12WaterSuction8BehaviorEv reads it back from.
 */
extern "C" {

struct PortOv026Cell { unsigned enter_fn, enter_adj, tick_fn, tick_adj; };
typedef int (*PortOv026Fn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately
   incomplete class (the SoundObject/Cap/MrBlizzard/BabyPenguin-state
   treatment); MSVC's PMF representation there does not reproduce the ROM's
   {function,adjustment} pair. */
int func_ov026_021122d4(char *c, PortOv026Cell *p)
{
    *(PortOv026Cell **)(c + 0x30c) = p;
    PortOv026Cell *q = *(PortOv026Cell **)(c + 0x30c);
    if (q->enter_fn == 0)
        return 1;
    return ((PortOv026Fn)(size_t)q->enter_fn)(c);
}

}
