/* HOST COPIES for gate 178's two ov064 riders, METAL_NET_LIFT (69,
 * daObjFl_Amilift_c) and LAVA_BUBBLE (214, daBbl_c):
 *   - the Amilift Render func_ov064_02117cfc -- RETIRED, run link100 lane
 *     UNMATCH; the slot-5 shadow collision died with SLOT5F's unfold,
 *   - the two PMF-dispatching bodies of each class, and
 *   - the seat of the two source PMF tables their sinits copy into bss.
 *
 * WHY THESE ARE HOST COPIES, NOT SLICED
 * -------------------------------------
 * 1. func_ov064_02117cfc RETIRED, run link100 lane UNMATCH. It dispatched its
 *    model's slot 5 through a LOCAL six-virtual shadow struct whose sixth
 *    method takes an int (`Base *b = &d->base; b->m(0)`, Model at +0xd4), and
 *    the host _ZTV5Model / _ZTV9ModelAnim were MSVC-ordered (one dtor slot
 *    where Itanium spends two), so ROM slot 5 landed on Virtual18 and read a
 *    null matrix -> c0000005. Lane SLOT5F respelled the destructor pair under
 *    _MSC_VER; the tables are ROM-numbered and the matched source needs no
 *    help. See the block below where the copy used to be.
 *
 * 2. func_ov064_02117d24, func_ov064_021187ec and _ZN10LavaBubble8BehaviorEv all
 *    invoke an mwcc pointer-to-member: `(obj->*pmf)()`. On MSVC that is a
 *    __thiscall (this in ECX) against a plain extern-C state body that wants self
 *    on the stack -> calling-convention corruption. func_ov064_02117d24 also
 *    indexes `data_ov064_0211c750[idx]` with the mwcc 8-byte record stride that
 *    MSVC would make 4 (the Scuttlebug stride trap). Each is copied here reading
 *    the record as a plain { fn, delta } (delta 0, non-virtual complete-class)
 *    and calling fn with `this`.
 *
 * THE SEAT (the Scuttlebug/MontyMole "seat the source before the copy" order)
 * --------------------------------------------------------------------------
 * Both runtime tables are bss, filled at static-init:
 *   - __sinit_ov064_0211afc0 copies 3 SOURCE PMFs data_ov064_0211bc0c/14/1c into
 *     data_ov064_0211c750 (Amilift Behavior states, idx 0..2).
 *   - __sinit_ov064_0211b150 copies 4 SOURCE PMFs data_ov064_0211be90/98/a0/a8
 *     into data_ov064_0211c7b8[0..1] and data_ov064_0211c7c8[0..1] (LavaBubble).
 * The ovdata mount lays the source blocks down as raw overlay bytes; their fn
 * words point at CODE (func_ov064_*), which the mount's pointer pass does NOT
 * rebase (it only rebases pointers into other mounted DATA). So the raw words are
 * DS addresses. port_ov064_gate178_states_seat() rewrites each source fn word
 * with its host body BEFORE the two sinits run, validating each mounted word
 * against the ROM address first so a wrong mount says so instead of copying an
 * overlay-image address into the live table. All seven .delta halves are 0
 * (verified against the overlay image).
 *
 * The state bodies (func_ov064_02117c24/bdc/b8c, func_ov064_02118644/873c/8760/
 * 87d0) are plain matched src in slice_gate178.txt -- called here by explicit
 * host pointer with `this` as arg 0, which is what they expect.
 */
#include <cstdio>
#include <cstdlib>

#include "Model.h"

extern "C" {

/* ---- METAL_NET_LIFT (69) Render: RETIRED (run link100, lane UNMATCH) --------
   The copy that stood here dispatched Model::Render qualified because the
   folded host table put Virtual18 on the slot the matched source means. Lane
   SLOT5F's destructor unfold ROM-numbers _ZTV5Model, so the source's own
   six-virtual shadow reaches Render at index 5 again and there is nothing left
   to spell differently. src/func_ov064_02117cfc.cpp carries it now, in
   port/slice_unmatch.txt; the reloc is from:0x0211bc8c (data_ov064_0211bc68 +
   4*9) -> 0x02117cfc and the body is ADJUDICATED REAL_DECOMP at
   port/tools/inferred_stub_adjudicated.txt:2366. The fill at
   hal/actor_classes_ov064_gate178.cpp:306 calls the C symbol by name, so it is
   unchanged. Model.h stays included: the seat below reads it too. */

/* record layout of every ov064 gate-178 PMF table: the ROM's 8-byte { fn, delta }
   with delta 0. */
struct PortPmf { unsigned fn; int delta; };

/* ---- METAL_NET_LIFT (69) Behavior: the 3-state PMF machine ------------------
   func_ov064_02117d24 line for line, only the `(c->*table[idx].pmf)()` dispatch
   spelled as an explicit host call on the 8-byte record. */
extern PortPmf data_ov064_0211c750[3];      /* bss runtime table, sinit-filled */
extern short data_02082214[];                /* arm9 sin/cos-ish table (already linked) */
int _Z14ApproachLinearRiii(int *v, int target, int step);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);

/* HOST COPY RETIRED, run link100 lane FWD gate 2. src/func_ov064_02117d24.c
   compiles and dispatches data_ov064_0211c750 itself. TWO THINGS HAD TO BE
   TRUE AND BOTH WERE MEASURED: the file is a .c whose own text spells
   `typedef void (C::*PMF)();`, which is not C, so it carries LANGUAGE CXX in
   block R10e -- lane PMFB2's treatment for src/func_ov002_020aea30.c -- and
   its table comes in decorated as ?data_ov064_0211c750@@3PAUEntry@@A, which
   port/hal/fwd_forwarders.cpp aliases onto the mounted C name. Its src defines
   the FLAT C symbol, so unlike gate 1's nine rows it needs no forwarder; the
   "MSVC would make the stride 4" reading the retired banner carried expired
   when block R8's /vmg /vmm landed, and the emitted [esi*8] says so. */

/* ---- LAVA_BUBBLE (214) seeder-dispatcher func_ov064_021187ec ----------------
   src: `c->pp = p; if (*p == 0) return 1; return (c->**p)()` -- store the table
   base at +0x300, call record[0] (the ENTER state). The record is 8 bytes. */
/* func_ov064_021187ec IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov064_021187ec.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */
/* ---- LAVA_BUBBLE (214) Behavior _ZN10LavaBubble8BehaviorEv ------------------
   The matched .cpp line for line, with the two `(this->*(m->pmf))()` dispatches
   (m = the table base stored at +0x300, m->pmf = record[1].fn, the MAIN state)
   spelled as explicit host calls. */
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
unsigned short DecIfAbove0_Short(unsigned short *p);
void *_ZN5Actor10FindWithIDEj(unsigned int id);
void _ZN6Player4BurnEv(void *self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, unsigned int j);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch; MSVC's PMF over an
 * incomplete class is the wider general representation. See the header. */
int _ZN10LavaBubble8BehaviorEv(void *cv)
{
    char *c = (char *)cv;
    int flags = *(int *)(c + 0x0b0);
    int b20 = (flags & 0x20000) != 0;

    if (b20) {
        PortPmf *m = *(PortPmf **)(c + 0x300);
        if (m[1].fn != 0)
            /* PORT_HOST_ABI: mwcc `(this->*(m->pmf))()`, m->pmf = record[1].fn */
            ((void (*)(void *))(size_t)m[1].fn)(c);
        return 1;
    }

    if ((flags & 0x40000) != 0)
        return 1;

    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000)) {
        if (*(unsigned char *)(c + 0x310) != 0)
            _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));

    {
        unsigned int id = *(unsigned int *)(c + 0x134);
        if (id != 0) {
            if ((*(int *)(c + 0x130) & 0x8000) == 0) {
                char *a = (char *)_ZN5Actor10FindWithIDEj(id);
                if (a != 0 && *(unsigned short *)(a + 0xc) == 0xbf)
                    _ZN6Player4BurnEv(a);
            } else {
                *(int *)(c + 0x128) |= 1;
            }
        }
    }

    {
        PortPmf *m = *(PortPmf **)(c + 0x300);
        if (m[1].fn != 0)
            /* PORT_HOST_ABI: mwcc `(this->*(m->pmf))()`, m->pmf = record[1].fn */
            ((void (*)(void *))(size_t)m[1].fn)(c);
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    if (*(int *)(c + 0x09c) != 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x144, 0);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    return 1;
}

/* ---- THE SEAT ---------------------------------------------------------------
   The seven source PMF fn words, by ROM address, and the host bodies that
   replace them. Rewritten BEFORE __sinit_ov064_0211afc0 / __sinit_ov064_0211b150
   copy them into the bss runtime tables. */
int func_ov064_02117c24(void *c);   /* Amilift state 0 */
int func_ov064_02117bdc(void *c);   /* Amilift state 1 */
int func_ov064_02117b8c(void *c);   /* Amilift state 2 */
int func_ov064_02118760(void *c);   /* LavaBubble c7b8[1] / MAIN of unk_310==0 */
int func_ov064_02118644(void *c);   /* LavaBubble c7c8[1] / MAIN of unk_310!=0 */
int func_ov064_021187d0(void *c);   /* LavaBubble c7b8[0] / ENTER of unk_310==0 */
int func_ov064_0211873c(void *c);   /* LavaBubble c7c8[0] / ENTER of unk_310!=0 */

/* the source PMF blocks, as 8-byte { fn, delta } records, mounted from ov064 */
extern PortPmf data_ov064_0211bc0c, data_ov064_0211bc14, data_ov064_0211bc1c;
extern PortPmf data_ov064_0211be90, data_ov064_0211be98,
               data_ov064_0211bea0, data_ov064_0211bea8;
}  /* extern "C" */

/* RUN link100, LANE FWD gate 2: THE AMILIFT'S THREE CELLS HOLD __fastcall
   FACES NOW. src/func_ov064_02117d24.c dispatches data_ov064_0211c750 itself
   since the host copy above was retired, and a matched TU dispatches a pointer
   to member as `mov ecx, TAB[i*8+4] / mov eax, TAB[i*8] / add ecx, this /
   call eax` -- receiver in ecx, nothing pushed (read off its /FAsc listing,
   runs/link100/out/FWD/emit_gate2_out.txt). THE FOUR LAVABUBBLE ROWS DO NOT
   CHANGE: their dispatcher _ZN10LavaBubble8BehaviorEv is still a host copy in
   this file calling the cell cdecl with an explicit self. */
#define AMI_FACE(cell, sym)                                               \
    static void __fastcall ami_c##cell(void *self, void *dead_edx)        \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }

AMI_FACE(0, func_ov064_02117c24)
AMI_FACE(1, func_ov064_02117bdc)
AMI_FACE(2, func_ov064_02117b8c)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_ov064_gate178_sources[] = {
    /* Amilift Behavior states, source -> __sinit_ov064_0211afc0 -> c750[0..2] */
    {&data_ov064_0211bc0c, 0x02117c24, (void *)ami_c0},
    {&data_ov064_0211bc14, 0x02117bdc, (void *)ami_c1},
    {&data_ov064_0211bc1c, 0x02117b8c, (void *)ami_c2},
    /* LavaBubble states, source -> __sinit_ov064_0211b150 -> c7b8/c7c8 */
    {&data_ov064_0211be90, 0x02118760, (void *)func_ov064_02118760},
    {&data_ov064_0211be98, 0x02118644, (void *)func_ov064_02118644},
    {&data_ov064_0211bea0, 0x021187d0, (void *)func_ov064_021187d0},
    {&data_ov064_0211bea8, 0x0211873c, (void *)func_ov064_0211873c},
};

extern "C" void port_ov064_gate178_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_ov064_gate178_sources /
                             sizeof g_ov064_gate178_sources[0]; ++i) {
        PortPmf *p = g_ov064_gate178_sources[i].slot;
        if (p->fn != g_ov064_gate178_sources[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: ov064 gate-178 source %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_ov064_gate178_sources[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_ov064_gate178_sources[i].host;
    }
}
