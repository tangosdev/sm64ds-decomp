/* HOST COPY of src/func_ov034_02112604.cpp -- the TICK half of the Wiggler's
 * eleven-cell pointer-to-member state machine (id 248, ov034, level 26).
 * Run rel0215, lane cast-sweep2.
 *
 * The matched src is the same shape as the enter half, eight bytes along:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct Entry { char pad[8]; PMF pmf; char tail[20 - 8 - sizeof(PMF)]; };
 *     extern "C" void func_ov034_02112604(C *c)
 *     { int j = c->idx; (c->*data_ov034_02114538[j].pmf)(); }
 *
 * `char pad[8]` steps over the entry's LO (enter) half to reach the HI (tick)
 * half. The full reasoning is in port/unmatched/Wiggler_StateEnter.cpp's header
 * and is not repeated; only the field changes, and this half does not write the
 * index.
 *
 * ROM 0x02112604 differs from the enter half only in the `add r3, r1, #8` that
 * picks the second pmf, and in not storing the index first.
 *
 * WHO CALLS IT: _ZN7Wiggler8BehaviorEv at 0x02112b98 -- which is the ONE body
 * in this overlay with no matched source, faced loudly in
 * hal/actor_classes_ov034.cpp. So on the host this tick half is reached only if
 * something else calls it; nothing in the seated set does today, and the face
 * says so once. It is hosted anyway because the enter half's table is shared
 * and a half-seated table is worse than a fully seated one.
 *
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov034.txt.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function, delta} pair.
 */
extern "C" {

struct PortWigglerEntry { unsigned enter_fn, enter_delta, tick_fn, tick_delta, name; };
typedef void (*PortWigglerFn)(void *);
extern PortWigglerEntry data_ov034_02114538[];

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class. */
void func_ov034_02112604(void *selfv)
{
    char *c = (char *)selfv;
    PortWigglerEntry *e = &data_ov034_02114538[*(int *)(c + 0x8c4)];
    char *recv = c + ((int)e->tick_delta >> 1);
    PortWigglerFn fn;
    if (e->tick_delta & 1)
        fn = (PortWigglerFn)(size_t)(*(unsigned **)recv)[e->tick_fn / 4];
    else
        fn = (PortWigglerFn)(size_t)e->tick_fn;
    fn(recv);
}
}
