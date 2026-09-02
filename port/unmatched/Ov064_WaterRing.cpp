/* HOST COPIES for WATER_RING (244, ov064, "14daWater_Ring_c"), run rel0215
 * wave 3, lane w3-c -- the two pointer-to-member dispatchers, plus the seat
 * of the four SOURCE records the sinit copies.
 *
 * WHY EACH ONE IS HERE
 *
 * 1. WaterRing::Behavior and func_ov064_02119ecc both form an mwcc
 *    pointer-to-member over a FORWARD-DECLARED struct (`struct C;` then
 *    `typedef void (C::*PMF)();`), so MSVC hands them the general
 *    worst-case-inheritance representation -- a multi-word value plus a
 *    this-adjust thunk -- where the ROM has a plain { function, 0 } pair.
 *    Dispatching that mangles `this`. The KnockDownPlank / MontyMole /
 *    Scuttlebug case, and the same shape gate 179's own two Piece dispatchers
 *    take in unmatched/BowserPuzzle_StateDispatch.cpp. Each body below is the
 *    matched source's control flow line for line; only the dispatch is read as
 *    a plain { fn, 0 } and the fn called with `this`.
 *
 * 1b. func_ov064_02119afc is here for a DIFFERENT reason -- a decl_common.h
 *    redeclaration, MEASURED off the first link of this slice rather than
 *    predicted. The matched TU declares `void func_ov064_02119ecc(void*, void*)`
 *    inside its own extern "C" block; include/decl_common.h:2762 already
 *    declares `extern void func_ov064_02119ecc(char*, void*)`. Both visible in
 *    one TU is `error C2733: you cannot overload a function with 'extern "C"'
 *    linkage`, and src/ is not edited to fix it. The ov096 lane hit this exact
 *    class on src/func_ov096_02137088.cpp and port/CMakeLists.txt records the
 *    treatment in one line: "the decl_common.h redeclaration that forced that
 *    one out of the slice". The body below is that TU verbatim, in a file that
 *    includes no decl_common.h, so the two declarations never meet. It forms no
 *    pointer-to-member of its own; it only passes &data_ov064_0211c944 to the
 *    dispatcher above.
 *
 * 2. WaterRing::Render is NO LONGER here -- it retired to its matched TU
 *    src/_ZN9WaterRing6RenderEv.cpp (slice_w3c.txt). That TU dispatches its
 *    plain Model at +0x30c through a LOCAL six-virtual shadow
 *    (`struct Sub { ... virtual int g5(void*); }`), which counts in ROM/Itanium
 *    numbering, so its slot 5 is Render; hal/cxxname_bridge.cpp dual-fills
 *    _ZTV5Model[5] with mv_render, so the shadow's one-argument slot-5 call
 *    lands on Model::Render. This host copy passed a NULL Vector3 * as the scale
 *    where the ROM (overlay_0064.bin 0x02119fc8: `add r1, r4, #0x80` before the
 *    `blx` through slot 5) and the matched TU both pass this+0x80, so retiring
 *    the host body also fixes that null. The stale reasoning this replaced --
 *    that slot 5 was Virtual18's MSVC slot -- predates the dual fill.
 *
 * THE STATE MACHINE IS TWO RECORDS, NOT FOUR. __sinit_ov064_0211b518 reads four
 * eight-byte SOURCE pairs and writes two SIXTEEN-byte bss records, each
 * { enter, tick }. The store order is off the sinit's own disassembly out of
 * extracted/overlays/overlay_0064.bin (T4), not off its literal pool order,
 * which is permuted:
 *     0x0211c954   enter 0x02119ea0   tick 0x02119d28    <- InitResources' record
 *     0x0211c944   enter 0x02119ce4   tick 0x02119c60    <- func_ov064_02119afc's
 * All four .delta halves are 0 (no reloc at any +4 in the overlay bytes), and
 * all four target bodies are matched src -- 0x02119ea0 was already sliced by
 * gate 179 as a "Piece" helper, the other three come in with this lane. There
 * are no state traps: nothing in this table is unhosted.
 *
 * The seat rewrites the SOURCE records' first words with the host bodies BEFORE
 * the sinit copies them (the MontyMole / Crate reading: seat the source, before
 * the copy, one less mapping to get wrong), each checked against the ROM address
 * the body was compiled from, so a mount pointing at the wrong bytes aborts
 * instead of copying an overlay-image address into the live table.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* WaterRing::Behavior's own C-linkage helpers, all matched src (slice_w3c) */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(char *self, char *cc);
void _ZN12CylinderClsn5ClearEv(char *c);
void _ZN12CylinderClsn6UpdateEv(char *c);
void _ZN9Animation7AdvanceEv(char *c);
void func_ov064_02119f1c(char *c);

/* the four state bodies, all matched src */
int func_ov064_02119ea0(void *c);   /* c954 enter */
int func_ov064_02119d28(void *c);   /* c954 tick  */
int func_ov064_02119ce4(void *c);   /* c944 enter */
int func_ov064_02119c60(void *c);   /* c944 tick  */

/* the four SOURCE records the sinit copies from (data, { fn, 0 }) */
extern PortPmf data_ov064_0211c3b0[];   /* {02119ea0, 0} */
extern PortPmf data_ov064_0211c3b8[];   /* {02119c60, 0} */
extern PortPmf data_ov064_0211c3c0[];   /* {02119ce4, 0} */
extern PortPmf data_ov064_0211c3c8[];   /* {02119d28, 0} */

/* what func_ov064_02119afc reaches, on top of the ring above */
struct PortVec3 { int x, y, z; };
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
        void *t, const PortVec3 &v);
void *_ZN5Actor10FindWithIDEj(unsigned id);
short Vec3_VertAngle(const PortVec3 *v1, const PortVec3 *v0);
int AngleDiff(int a, int b);
short _ZN5Actor18HorzAngleToCPlayerEv(void *t);
void _ZN6Player4HealEi(void *p, int amt);
extern PortVec3 data_ov064_0211c3d0;
extern PortPmf data_ov064_0211c944[];   /* the record it switches to */

}  /* extern "C" */

/* PORT_HOST_ABI: mwcc pointer-to-member on a forward-declared struct. The
   matched WaterRing::Behavior reads the record at this+0x370 and calls its
   SECOND half (the tick, byte +8) when non-null. Read here as a plain
   { fn, 0 }. The DecIfAbove0_Short, the three angle copies, the UpdatePos and
   the collider/animation tail are the matched Behavior's own, in its order. */
extern "C" int _ZN9WaterRing8BehaviorEv(void *self)
{
    char *c = (char *)self;
    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        PortPmf *obj = *(PortPmf **)(c + 0x370);
        if (obj && obj[1].fn)
            ((int (*)(void *))(size_t)obj[1].fn)(c);
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    *(short *)(c + 0x08c) = *(short *)(c + 0x092);
    *(short *)(c + 0x08e) = *(short *)(c + 0x094);
    *(short *)(c + 0x090) = *(short *)(c + 0x096);
    func_ov064_02119f1c(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    _ZN9Animation7AdvanceEv(c + 0x35c);
    return 1;
}

/* PORT_HOST_ABI: mwcc pointer-to-member on a forward-declared struct. The
   matched func_ov064_02119ecc stores the record at this+0x370, then reads its
   FIRST half (the enter, byte +0) and, if non-null, calls it with `this`.
   Read as a plain { fn, 0 }. */
extern "C" int func_ov064_02119ecc(void *self, void *p)
{
    char *c = (char *)self;
    *(void **)(c + 0x370) = p;
    PortPmf *rec = (PortPmf *)p;
    if (rec[0].fn == 0)
        return 1;
    return ((int (*)(void *))(size_t)rec[0].fn)(c);
}

/* PORT_HOST_ABI: displaced by a decl_common.h redeclaration (see 1b in this
   file's header), not by an ABI fault of its own. src/func_ov064_02119afc.cpp
   verbatim, including the two gotos and the sign-bit test the matched TU
   spells as `(x >> 16) & 1`; only the declarations moved. */
extern "C" void func_ov064_02119afc(char *c)
{
    PortVec3 hv;
    PortVec3 v;
    char *a;
    int b;
    unsigned id;

    *(int *)(c + 0x368) = 0x1000;
    v = data_ov064_0211c3d0;
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x110, v);
    id = *(unsigned *)(c + 0x134);
    if (id == 0) return;
    a = (char *)_ZN5Actor10FindWithIDEj(id);
    if (a == 0) return;
    b = (*(unsigned short *)(a + 0xc) == 0xbf);
    if (b == 0) return;
    {
        hv.x = *(int *)(a + 0x5c);
        hv.y = *(int *)(a + 0x60);
        hv.z = *(int *)(a + 0x64);
    }
    if (AngleDiff(*(short *)(c + 0x8c),
                  Vec3_VertAngle((PortVec3 *)(c + 0x5c), &hv)) >= 0x3000)
        return;
    if (*(int *)(c + 0x37c) != 1) goto Lcheck;
    if (((*(short *)(c + 0x388) >> 16) & 1)
        != ((_ZN5Actor18HorzAngleToCPlayerEv(c) >> 16) & 1))
        goto Lpassed;
Lcheck:
    if (*(int *)(c + 0x37c) == 1) goto Lkeep;
Lpassed:
    if (*(int *)(c + 0x37c) == 1)
        _ZN6Player4HealEi(a, 0x100);
    *(int *)(c + 0x368) = 0x4000;
    func_ov064_02119ecc(c, &data_ov064_0211c944[0]);
    return;
Lkeep:
    *(short *)(c + 0x388) = _ZN5Actor18HorzAngleToCPlayerEv(c);
    return;
}

/* ---- the seat ------------------------------------------------------------ */

static const struct { PortPmf *slot; unsigned rom; int (*host)(void *); }
g_water_ring_states[] = {
    {data_ov064_0211c3b0, 0x02119ea0, func_ov064_02119ea0},  /* c954 enter */
    {data_ov064_0211c3c8, 0x02119d28, func_ov064_02119d28},  /* c954 tick  */
    {data_ov064_0211c3c0, 0x02119ce4, func_ov064_02119ce4},  /* c944 enter */
    {data_ov064_0211c3b8, 0x02119c60, func_ov064_02119c60},  /* c944 tick  */
};

extern "C" void port_water_ring_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_water_ring_states /
                             sizeof g_water_ring_states[0]; ++i) {
        PortPmf *p = g_water_ring_states[i].slot;
        if (p->fn != g_water_ring_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: WaterRing source %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_water_ring_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_water_ring_states[i].host;
    }
}
