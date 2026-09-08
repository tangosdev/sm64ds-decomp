/* HOST COPY of src/_ZN5Whomp8BehaviorEv.cpp -- the mwcc pointer-to-member
 * dispatch read as a plain function pointer, and the twelve-entry state table
 * seated with host addresses. The Fish/Door case for gates 64-69's first
 * class, and the reasons are the ones every prior seat gives:
 *
 *   * MSVC forms a pointer-to-member of an INCOMPLETE class as the four-word
 *     general representation. The matched source writes `struct Actor;` and
 *     `typedef int (Actor::*PMF)()` before Actor is defined, then indexes
 *     `data_ov079_02128280[idx]` (an mwcc {function, delta} pair, 8 bytes) and
 *     calls `(this->**pmf)()`. Compiled by MSVC that reads a 16-byte stride and
 *     dispatches a neighbour's body. So the table is read here as what it is:
 *     the pair's first word, called as a plain function pointer.
 *   * __sinit_ov079_02127618 copies twelve {function, delta} statics from ov079
 *     0x02127bc0..0x02127c20 into the bss table data_ov079_02128280, and those
 *     statics are the overlay image's own words -- DS CODE ADDRESSES. The seat
 *     rewrites the twelve STATICS before the sinit runs (the LakituBro reading:
 *     one less mapping to get wrong), each checked against the ROM address its
 *     host body was compiled from. All twelve deltas are 0 in the ROM.
 *
 * ALL TWELVE STATES ARE MATCHED SRC -- there is no hole and no trap. WHOMP and
 * WHOMP_KING share this class and this table.
 *
 * The body is the matched source line for line; only the dispatch is spelled
 * as a plain call through the pair's first word.
 */
#include <cstdio>
#include <cstdlib>

struct PortWithMeshClsn;
struct PortCylinderClsn;

extern "C" {
int _ZN5Actor13DistToCPlayerEv(void *self);
void func_ov079_02123f34(void *self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(
    void *self, void *w, int a, short b, int c, int d, void *e);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *w, unsigned n);
void func_ov079_02124188(void *self);
int func_ov079_021243e0(char *c, int r4);
int func_ov079_02123a8c(void *self);
void func_ov079_02124008(void *self);

extern int data_0209f318;

struct PortPmf { unsigned fn; int delta; };
/* the bss table the sinit fills, dispatched every frame */
extern PortPmf data_ov079_02128280[];

/* the twelve statics __sinit_ov079_02127618 copies into data_ov079_02128280 */
extern PortPmf data_ov079_02127bc0[], data_ov079_02127bc8[],
    data_ov079_02127bd0[], data_ov079_02127bd8[], data_ov079_02127be0[],
    data_ov079_02127be8[], data_ov079_02127bf8[], data_ov079_02127c00[],
    data_ov079_02127c08[], data_ov079_02127c10[], data_ov079_02127c18[],
    data_ov079_02127c20[];

void func_ov079_02124530(void *); void func_ov079_02124638(void *);
void func_ov079_021246d8(void *); void func_ov079_021246dc(void *);
void func_ov079_021249f0(void *); void func_ov079_02124b08(void *);
void func_ov079_02125240(void *); void func_ov079_0212538c(void *);
void func_ov079_021254b4(void *); void func_ov079_021256d4(void *);
void func_ov079_021258fc(void *); void func_ov079_02125b44(void *);
}

/* Seated over the SOURCE side, one row per static, checked against the ROM
   address the host body was compiled from. All twelve deltas are 0. */
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
#define WHOMP_FACE(cell, sym)                                             \
    static void __fastcall whomp_c##cell(void *self, void *dead_edx)      \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }

WHOMP_FACE(0, func_ov079_0212538c)
WHOMP_FACE(1, func_ov079_02125240)
WHOMP_FACE(2, func_ov079_021258fc)
WHOMP_FACE(3, func_ov079_021254b4)
WHOMP_FACE(4, func_ov079_02124638)
WHOMP_FACE(5, func_ov079_021246dc)
WHOMP_FACE(6, func_ov079_021246d8)
WHOMP_FACE(7, func_ov079_02125b44)
WHOMP_FACE(8, func_ov079_02124530)
WHOMP_FACE(9, func_ov079_021256d4)
WHOMP_FACE(10, func_ov079_02124b08)
WHOMP_FACE(11, func_ov079_021249f0)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_whomp_states[] = {
    {data_ov079_02127bc0, 0x0212538c, (void *)whomp_c0},
    {data_ov079_02127bc8, 0x02125240, (void *)whomp_c1},
    {data_ov079_02127bd0, 0x021258fc, (void *)whomp_c2},
    {data_ov079_02127bd8, 0x021254b4, (void *)whomp_c3},
    {data_ov079_02127be0, 0x02124638, (void *)whomp_c4},
    {data_ov079_02127be8, 0x021246dc, (void *)whomp_c5},
    {data_ov079_02127bf8, 0x021246d8, (void *)whomp_c6},
    {data_ov079_02127c00, 0x02125b44, (void *)whomp_c7},
    {data_ov079_02127c08, 0x02124530, (void *)whomp_c8},
    {data_ov079_02127c10, 0x021256d4, (void *)whomp_c9},
    {data_ov079_02127c18, 0x02124b08, (void *)whomp_c10},
    {data_ov079_02127c20, 0x021249f0, (void *)whomp_c11},
};

extern "C" void port_whomp_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_whomp_states / sizeof g_whomp_states[0];
         ++i) {
        PortPmf *p = g_whomp_states[i].slot;
        if (p->fn != g_whomp_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Whomp state %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_whomp_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_whomp_states[i].host;
    }
}

/* HOST COPY RETIRED, run link100 lane FWD. Whomp::Behavior
 * dispatches this table from src/_ZN5Whomp8BehaviorEv.cpp now. The flat C name the port's
 * actor-class face calls is defined by the forwarder in
 * port/hal/fwd_forwarders.cpp, which receives `this` on the stack and calls
 * the member through the real class type; the member and the flat name are two
 * different symbols with two different conventions, so no /alternatename could
 * have bridged them. The cells this seat installs are __fastcall faces for the
 * same reason.
 */
