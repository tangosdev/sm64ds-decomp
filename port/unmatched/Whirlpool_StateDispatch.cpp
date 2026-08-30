/* HOST COPY of src/func_ov026_02111ee0.cpp -- the WHIRLPOOL's state-enter
 * dispatcher (id 246, daWater_Tatumaki_c, ov026). The Cap/SoundObject/
 * MrBlizzard/BabyPenguin treatment applied to a two-cell state table.
 *
 * WHY A HOST COPY. The matched src dispatches through a REAL C++
 * pointer-to-member on a DELIBERATELY INCOMPLETE class:
 *
 *     struct C; typedef int (C::*PMF)();
 *     struct C { char pad[0x110]; PMF *pp; };
 *     int func_ov026_02111ee0(C *c, PMF *p)
 *     { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }
 *
 * mwcc's PMF for that shape is the ROM's own 8-byte {function, adjustment}
 * pair, which is the stride the two cells actually have. MSVC picks its
 * pointer-to-member representation where the type is first named, and `struct
 * C;` forward-declared gets the GENERALISED (multi-word) form -- so `*q` reads
 * the wrong number of bytes and the call does not reproduce the ROM's
 * semantics. The matched src stays in src/ as the byte proof; this file is the
 * port body, translated line for line with the PMF call replaced by a plain
 * function-pointer call through the same record layout
 * hal/actor_classes_ov026.cpp's port_ov026_states_seat() has already seated.
 *
 * THE CELL. Each state cell is TWO 8-byte {function, adjustment} records back
 * to back -- enter at +0, tick at +8 -- built into bss by
 * __sinit_ov026_02112c94 from the four source pairs at 0x02113cec..0x02113d04.
 * The whirlpool's two cells are data_ov026_02113f2c (state 0) and
 * data_ov026_02113f3c (state 1). The cell POINTER lives at +0x110 in the
 * object, which is where the matched src's `pad[0x110]` puts it and where
 * _ZN9Submarine8BehaviorEv reads it back from every frame.
 */
extern "C" {

struct PortOv026Cell { unsigned enter_fn, enter_adj, tick_fn, tick_adj; };
typedef int (*PortOv026Fn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately
   incomplete class (the SoundObject/Cap/MrBlizzard/BabyPenguin-state
   treatment); MSVC's PMF representation there does not reproduce the ROM's
   {function,adjustment} pair. */
int func_ov026_02111ee0(char *c, PortOv026Cell *p)
{
    *(PortOv026Cell **)(c + 0x110) = p;
    PortOv026Cell *q = *(PortOv026Cell **)(c + 0x110);
    if (q->enter_fn == 0)
        return 1;
    return ((PortOv026Fn)(size_t)q->enter_fn)(c);
}

}
