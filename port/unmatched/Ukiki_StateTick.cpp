/* HOST COPY of src/func_ov030_02114134.cpp -- the TICK half of the Ukiki's
 * eleven-cell pointer-to-member state machine. Run rel0215, lane cast-ov030.
 *
 * The matched src is the same shape as the enter half, one pair along:
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct C { char pad[0x3a4]; PMF *pp; };
 *     extern "C" void func_ov030_02114134(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
 *
 * `c->pp + 1` steps ONE 8-byte mwcc PMF, which is the cell's `.hi` half in
 * src/__sinit_ov030_02114924.c's own `S16 { S8 lo, hi; }` declaration. The full
 * reasoning is in port/unmatched/Ukiki_StateEnter.cpp's header and is not
 * repeated; only the field changes.
 *
 * ONE EXTRA NOTE THAT BELONGS HERE RATHER THAN THERE: this TU was ALREADY in
 * port/CMakeLists.txt before this lane, in the wave-18 `/vmg;/vmm` reclaimed-
 * dispatcher list at line 2725, under the comment "inert today: no host copy,
 * no reference, /OPT:REF strips them". Seating ov030 makes it REACHABLE --
 * _ZN13RollingLogTtm8BehaviorEv calls func_ov030_02114134 directly -- so the
 * "inert" reading expires with this lane and the host copy is what the call
 * must land on. The src TU is dropped from slice_ov030cast.txt; the /vmg;/vmm
 * row above stays untouched (this lane owns neither that list nor that TU's
 * compilation), and it now compiles an object nothing references, which is what
 * that comment already describes.
 */
extern "C" {
struct PortUkikiCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef void (*PortUkikiFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
   class; MSVC's PMF representation there does not reproduce the ROM's
   {function,delta} pair. */
void func_ov030_02114134(char *c)
{
    PortUkikiCell *p = *(PortUkikiCell **)(c + 0x3a4);
    ((PortUkikiFn)(size_t)p->tick_fn)(c);
}
}
