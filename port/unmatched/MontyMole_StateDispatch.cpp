/* HOST COPY of MONTY_MOLE's (MontyMole / daChoropu_c, actor 310, ov080)
 * Behavior -- the only TU that reads its pointer-to-member state table -- and
 * the seat of the table it reads.
 *
 * MontyMole::Behavior drives a six-state machine through data_ov080_02128438,
 * six single {function, delta} records that __sinit_ov080_021278c0 copies out of
 * six source statics at ov080 0x02127f80..0x02127fa8. Unlike the Crate's
 * {init, main} pairs, each MontyMole state is one record and Behavior dispatches
 * it every frame by the object's own idx (at +0x17c):
 *
 *   MontyMole::Behavior(c)   calls state c->idx's function, every frame
 *
 * Behavior is the OneUpMushroom / WHOMP case and must be a host copy for the
 * same two reasons:
 *
 *   1. MSVC forms a pointer-to-member of the INCOMPLETE `struct C;` the matched
 *      Behavior declares as the four-word general representation, which strides
 *      the table at 0x10 instead of 0x8 and dispatches a neighbour's body.
 *      Reading the record as a plain { function, 0 } and calling the function
 *      with `this` is the fix.
 *
 *   2. The words the sinit copies are the overlay image's own -- DS CODE
 *      ADDRESSES, the ovdata contract, matched against ov080's relocs. The seat
 *      below rewrites each SOURCE static's first word with its host body BEFORE
 *      the sinit copies it into the runtime table (the WaterBomb / Crate
 *      reading: one less mapping to get wrong), each checked against the ROM
 *      address the body was compiled from, so a mount pointing at the wrong
 *      bytes says so instead of calling into the overlay image.
 *
 * The table is the mole's OWN -- only MontyMole::Behavior references
 * data_ov080_02128438 -- and all six targets are matched src (slice_gate174.txt),
 * so there are no state traps. All six .delta halves are 0 (non-virtual
 * complete-class form, per the relocated overlay bytes).
 *
 * The rest of the mole (Init, Cleanup, Render, both destructors, the mid-life
 * helpers func_ov080_02124088/02124208/02124360/021243d8 and the six state
 * bodies) is matched src in the slice; only Behavior lands here.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
/* the two per-frame helpers Behavior calls after the dispatch; matched src */
int func_ov080_02124208(void *c);
void func_ov080_021243d8(char *c);
/* engine faces Behavior calls; all in the map */
void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void *self, void *cyl);
void _ZN12CylinderClsn5ClearEv(void *c);
void _ZN12CylinderClsn6UpdateEv(void *c);

/* the six state functions, table order; all matched src */
void func_ov080_02123fcc(char *c);   /* state 0 */
void func_ov080_02123ecc(char *c);   /* state 1 */
void func_ov080_02123c24(char *c);   /* state 2 */
void func_ov080_02123a34(char *c);   /* state 3 */
void func_ov080_02123924(char *c);   /* state 4 */
void func_ov080_02123860(char *c);   /* state 5 */

struct PortPmf { unsigned fn; int delta; };
/* the RUNTIME table Behavior reads (bss, filled by the sinit) */
extern PortPmf data_ov080_02128438[6];
/* the six SOURCE statics __sinit_ov080_021278c0 copies from. The sinit's copy
   order maps table index -> source: 0<-fa0, 1<-f88, 2<-fa8, 3<-f80, 4<-f98,
   5<-f90. */
extern PortPmf data_ov080_02127fa0[], data_ov080_02127f88[],
    data_ov080_02127fa8[], data_ov080_02127f80[], data_ov080_02127f98[],
    data_ov080_02127f90[];
}  /* extern "C" */

/* HOST COPY RETIRED, run link100 lane FWD. MontyMole::Behavior
 * dispatches this table from src/_ZN9MontyMole8BehaviorEv.cpp now. The flat C name the port's
 * actor-class face calls is defined by the forwarder in
 * port/hal/fwd_forwarders.cpp, which receives `this` on the stack and calls
 * the member through the real class type; the member and the flat name are two
 * different symbols with two different conventions, so no /alternatename could
 * have bridged them. The cells this seat installs are __fastcall faces for the
 * same reason.
 */

/* RUN link100, LANE FWD: THIS TABLE'S CELLS HOLD __fastcall FACES NOW.
 * The host copy below is retired and the matched TU dispatches the table
 * itself. A matched TU dispatches a pointer to member as
 *     mov ecx, TAB[i*8+4] / mov eax, TAB[i*8] / add ecx, this / call eax
 * -- receiver in ecx, NOTHING pushed and no `add esp` after, read off the TU's
 * own /FAsc listing (runs/link100/out/FWD/emit_gate1_out.txt) -- where this
 * seat used to install a plain cdecl body that takes its self off the stack.
 * A zero-argument __fastcall face has exactly the convention the matched TU
 * calls with, and hands the receiver on as the cdecl argument the ROM's own
 * state bodies take. One face per CELL, not per body: two cells that carry the
 * same code word stay distinguishable (lane PMFB5's rule).
 */
#define MOLE_FACE(cell, sym)                                              \
    static void __fastcall mole_c##cell(void *self, void *dead_edx)       \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym((char *)self);                                                \
    }

MOLE_FACE(0, func_ov080_02123fcc)
MOLE_FACE(1, func_ov080_02123ecc)
MOLE_FACE(2, func_ov080_02123c24)
MOLE_FACE(3, func_ov080_02123a34)
MOLE_FACE(4, func_ov080_02123924)
MOLE_FACE(5, func_ov080_02123860)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_monty_mole_states[] = {
    {data_ov080_02127fa0, 0x02123fcc, (void *)mole_c0},  /* state 0 */
    {data_ov080_02127f88, 0x02123ecc, (void *)mole_c1},  /* state 1 */
    {data_ov080_02127fa8, 0x02123c24, (void *)mole_c2},  /* state 2 */
    {data_ov080_02127f80, 0x02123a34, (void *)mole_c3},  /* state 3 */
    {data_ov080_02127f98, 0x02123924, (void *)mole_c4},  /* state 4 */
    {data_ov080_02127f90, 0x02123860, (void *)mole_c5},  /* state 5 */
};

extern "C" void port_monty_mole_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_monty_mole_states /
                             sizeof g_monty_mole_states[0]; ++i) {
        PortPmf *p = g_monty_mole_states[i].slot;
        if (p->fn != g_monty_mole_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: MontyMole state %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_monty_mole_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_monty_mole_states[i].host;
    }
}
