/* HOST COPIES for WATER_RING (244, ov064, "14daWater_Ring_c"), run rel0215
 * wave 3, lane w3-c -- the two pointer-to-member dispatchers and the Model
 * slot-5 Render, plus the seat of the four SOURCE records the sinit copies.
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
 * 2. WaterRing::Render dispatches its plain Model at +0x30c through a LOCAL
 *    six-virtual shadow (`struct Sub { ... virtual int g5(void*); }`), which
 *    counts in ROM/Itanium numbering, so its slot 5 is Render; the host
 *    _ZTV5Model is MSVC-numbered and its slot 5 is Virtual18, which takes two
 *    arguments where the shadow passes one. The measured Butterfly / Fish /
 *    QuestionBlock / Whomp / BabyPenguin fault. Spelled here as the qualified
 *    Model::Render, the BowserPuzzle_Render.cpp / Ov064_Clam.cpp treatment.
 *    WaterRing_Spawn constructs a Model (not a ModelAnim) at +0x30c, which is
 *    why the qualified call is Model's.
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

#include "Model.h"

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

/* PORT_HOST_ABI: ROM-order Model slot-5 dispatch, the Whomp/Fish case. The
   matched TU's shadow passes literal 0 as the scale, so the qualified call
   passes a null Vector3 * -- the ROM's own argument. */
extern "C" {
int _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);

int _ZN9WaterRing6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x35c, c + 0x314);
    ((Model *)(c + 0x30c))->Model::Render((const Vector3 *)0);
    return 1;
}
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
