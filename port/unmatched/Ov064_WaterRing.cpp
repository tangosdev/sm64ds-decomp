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
 * 1b. func_ov064_02119afc is NO LONGER here (run linkfull wave 23, lane
 *    HGFRONT1). It stood here for a decl_common.h redeclaration, a C2733
 *    between the TU's `void func_ov064_02119ecc(void*, void*)` and
 *    include/decl_common.h:2762's `(char*, void*)`. The synced TU compiles
 *    clean under walk_window's own flags today and is MATCHING at 2004/b56
 *    with --strict-relocs against the cartridge (ov064 0x02119afc, 0x164
 *    bytes), so src/func_ov064_02119afc.cpp is on port/slice_w23_hostgen.txt
 *    and the host body, with the declarations only it used, is gone. It forms
 *    no pointer-to-member of its own; it only passes &data_ov064_0211c944 to
 *    the dispatcher above.
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
void _ZN8dActor_c9UpdatePosEP5dCc_c(char *self, char *cc);
void _ZN5dCc_c5ClearEv(char *c);
void _ZN5dCc_c6UpdateEv(char *c);
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

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN9WaterRing8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

/* func_ov064_02119ecc IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov064_02119ecc.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */

/* HOST COPY RETIRED, run linkfull wave 23 lane HGFRONT1: func_ov064_02119afc is
   src/func_ov064_02119afc.cpp again (port/slice_w23_hostgen.txt); note 1b in
   this file's header says why it could come back. */

/* ---- the seat ------------------------------------------------------------ */

/* run link100 lane PMFB7 gate 1: THE TWO TICK RECORDS ARE FACES.
   src/_ZN9WaterRing8BehaviorEv.cpp dispatches the cell's +8 half as a real
   pointer to member -- mov eax,[cell+8] / test / je / mov ecx,[cell+12] /
   add ecx,this / call eax, ARITY ZERO, /Zp4 diff 0 lines -- so the receiver
   arrives in ecx with nothing on the stack. The two ENTER records keep their
   plain cdecl bodies: func_ov064_02119ecc reaches them through the caller's
   own frame. */
#define WR_FACE(tag, sym)                                                 \
    static void __fastcall wr_##tag(void *self, void *dead_edx)           \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }
WR_FACE(02119d28, func_ov064_02119d28)
WR_FACE(02119c60, func_ov064_02119c60)

static const struct { PortPmf *slot; unsigned rom; int (*host)(void *); }
g_water_ring_states[] = {
    {data_ov064_0211c3b0, 0x02119ea0, func_ov064_02119ea0},  /* c954 enter */
    {data_ov064_0211c3c8, 0x02119d28,
     (int (*)(void *))(void *)wr_02119d28},                  /* c954 tick  */
    {data_ov064_0211c3c0, 0x02119ce4, func_ov064_02119ce4},  /* c944 enter */
    {data_ov064_0211c3b8, 0x02119c60,
     (int (*)(void *))(void *)wr_02119c60},                  /* c944 tick  */
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
