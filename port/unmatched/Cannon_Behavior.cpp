/* HOST COPY of src/_ZN6Cannon8BehaviorEv.cpp -- the mwcc pointer-to-member
 * dispatch read as a plain function pointer, and the table seated with host
 * addresses. The gate-16 case for the fourth time
 * (OneUpMushroom_Behavior.cpp, SignPost_StateDispatch.cpp,
 * LakituBro_Behavior.cpp), and the reasons are unchanged:
 *
 *   * MSVC forms a pointer-to-member of an INCOMPLETE class as the four-word
 *     general representation, which doubles this table's stride and
 *     dispatches a neighbour's body. The source TU declares `struct C;`
 *     before the typedef, so it hits that.
 *   * __sinit_ov098_0213c214 copies four {function, delta} statics into
 *     data_ov098_0213c8fc, and those are the overlay image's own words --
 *     DS CODE ADDRESSES. The seat rewrites the four STATICS before the sinit
 *     runs (the LakituBro reading: same guarantee, one less mapping to get
 *     wrong), each checked against the ROM address its host body was compiled
 *     from.
 *
 * ov098 0x0213ade8 (state 1, the lid opening) is MATCHED SRC now:
 * src/func_ov098_0213ade8.c went byte-perfect at 2004/b56 (PR #1227,
 * linkcheck VERIFIED) and the gate-19 slice compiles it. This file no
 * longer carries any state body -- only the PMF-dispatch frame and the
 * seat, which remain PORT_HOST_ABI for the incomplete-class PMF reason
 * above. The seat routes all four states at their ROM addresses to the
 * linked matched bodies.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

int func_ov098_0213a984(void *self);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);

struct PortPmf { unsigned fn; int delta; };
/* the four statics __sinit_ov098_0213c214 copies into data_ov098_0213c8fc */
extern PortPmf data_ov098_0213c644[], data_ov098_0213c64c[],
    data_ov098_0213c654[], data_ov098_0213c65c[];
extern PortPmf data_ov098_0213c8fc[];

void func_ov098_0213aa28(void *);   /* state 3 */
void func_ov098_0213ad08(void *);   /* state 2, the closed lid */
void func_ov098_0213b0a4(void *);   /* state 0, the aim */
void func_ov098_0213ade8(void *);   /* state 1, the lid opening (matched src) */

}  /* extern "C" */

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
#define CANNON_FACE(cell, sym)                                            \
    static void __fastcall cannon_c##cell(void *self, void *dead_edx)     \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }

CANNON_FACE(0, func_ov098_0213aa28)
CANNON_FACE(1, func_ov098_0213b0a4)
CANNON_FACE(2, func_ov098_0213ad08)
CANNON_FACE(3, func_ov098_0213ade8)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_cannon_states[] = {
    {data_ov098_0213c644, 0x0213aa28, (void *)cannon_c0},
    {data_ov098_0213c64c, 0x0213b0a4, (void *)cannon_c1},
    {data_ov098_0213c654, 0x0213ad08, (void *)cannon_c2},
    {data_ov098_0213c65c, 0x0213ade8, (void *)cannon_c3},
};

extern "C" void port_cannon_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_cannon_states / sizeof g_cannon_states[0];
         ++i) {
        PortPmf *p = g_cannon_states[i].slot;
        if (p->fn != g_cannon_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Cannon state %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_cannon_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_cannon_states[i].host;
    }
}

/* HOST COPY RETIRED, run link100 lane FWD. Cannon::Behavior
 * dispatches this table from src/_ZN6Cannon8BehaviorEv.cpp now. The flat C name the port's
 * actor-class face calls is defined by the forwarder in
 * port/hal/fwd_forwarders.cpp, which receives `this` on the stack and calls
 * the member through the real class type; the member and the flat name are two
 * different symbols with two different conventions, so no /alternatename could
 * have bridged them. The cells this seat installs are __fastcall faces for the
 * same reason.
 */
