/* HOST COPY of src/func_ov085_0212d268.cpp -- the RABBIT_KEY's seat-and-enter,
 * with the mwcc pointer-to-member dispatch hand-rolled as a plain call.
 *
 * The gate-16 case again (OneUpMushroom_Behavior.cpp, SignPost_StateDispatch
 * .cpp, LakituBro_Behavior.cpp). The matched source spells
 *
 *     struct C; typedef int (C::*PMF)();
 *     c->pp = p; if (*q == 0) return 1; return (c->**q)();
 *
 * and MSVC forms a PMF over an INCOMPLETE class as the four-word general
 * representation: the call would read SIXTEEN bytes from the ROM's EIGHT-byte
 * {function, adj} pair -- its neighbour included -- and dispatch thiscall
 * where every seated host body is extern-C cdecl. Same failure, same fix: the
 * body below is the matched source's control flow with the PMF spelled out in
 * mwcc's own {value, adj} semantics, exactly as Rabbit::Behavior's matched .c
 * (src/_ZN6Rabbit8BehaviorEv.c) hand-rolls the identical dispatch:
 *
 *     this += adj >> 1;  adj & 1 ? call *(vtable + value) : call value
 *
 * EVIDENCE the pairs this dispatches are the plain nonvirtual form: the four
 * statics __sinit_ov085_0212f9bc copies into the two State objects were read
 * out of overlay_0085.bin (load base 0x02129020) and each is { function, 0 }:
 *
 *     data_ov085_02130174 = { 0x0212cd80, 0 }   caught Main
 *     data_ov085_0213017c = { 0x0212d038, 0 }   caught enter (the grant)
 *     data_ov085_02130184 = { 0x0212d24c, 0 }   initial enter
 *     data_ov085_0213018c = { 0x0212d108, 0 }   initial Main (approach)
 *
 * with the relocs to match (config/arm9/overlays/ov085/relocs.txt: from each
 * of the four addresses, kind:load, to the listed function, module
 * overlay(85)). port_rabbit_key_states_seat (hal/actor_overlays.cpp) verifies
 * those same words against the mount and repoints them at the host bodies
 * before the sinit copies them, so what arrives here is a host address with
 * adj still 0. The adj arithmetic is kept anyway -- it is what the ROM's own
 * call sequence does, and a future non-zero pair should take the documented
 * path rather than a silent wrong one.
 *
 * Callers (both matched, both cdecl): RabbitKey::InitResources seats the
 * initial State 0213071c; func_ov085_0212d108 seats the caught State 0213072c
 * the frame the key reaches the player. */
#include <cstddef>   /* size_t (pointer-width casts) */

// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC widens PMF over an incomplete class).
extern "C" int func_ov085_0212d268(char *c, int *p)
{
    int adj;
    char *self;
    int (*fn)(char *);

    *(int **)(c + 0x188) = p;           /* c->pp = p */
    if (p[0] == 0)
        return 1;
    adj = p[1];
    self = c + (adj >> 1);
    if (adj & 1)
        fn = *(int (**)(char *))(*(char **)self + p[0]);
    else
        fn = (int (*)(char *))(size_t)p[0];
    return fn(self);
}
