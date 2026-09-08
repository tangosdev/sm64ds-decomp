/* HOST COPY of src/_ZN4Fish8BehaviorEv.cpp -- the mwcc pointer-to-member
 * dispatch read as a plain function pointer, and the table seated with host
 * addresses. The gate-16 case for the fifth time (OneUpMushroom_Behavior.cpp,
 * SignPost_StateDispatch.cpp, LakituBro_Behavior.cpp, Cannon_Behavior.cpp),
 * and the reasons are unchanged:
 *
 *   * MSVC forms a pointer-to-member of an INCOMPLETE class as the four-word
 *     general representation, which quadruples this table's stride and
 *     dispatches a neighbour's body. The source TU writes `struct C;` before
 *     the typedef and only defines C further down, so it hits that squarely.
 *   * __sinit_ov100_02147bc0 copies seven {function, delta} statics from
 *     ov100 0x02148448..0x02148478 into data_ov100_02148a1c, and those are
 *     the overlay image's own words -- DS CODE ADDRESSES. The seat rewrites
 *     the seven STATICS before the sinit runs (the LakituBro reading: same
 *     guarantee, one less mapping to get wrong), each checked against the ROM
 *     address its host body was compiled from.
 *
 * ALL SEVEN STATES ARE MATCHED SRC -- there is no hole here and no trap. The
 * fish has nowhere to fall through to.
 *
 * The body is the matched source's line for line; only the dispatch is
 * spelled as a plain call through the pair's first word.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortVec3 { int x, y, z; };
struct PortM48 { int w[12]; };
extern PortM48 data_020a0e68;

int *_ZN5Actor10FindWithIDEj(unsigned id);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
void Vec3_Asr(PortVec3 *d, PortVec3 *s, int sh);
void Matrix4x3_FromTranslation(PortM48 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(PortM48 *m, short angY);
void _ZN9Animation7AdvanceEv(void *anim);
int func_ov100_0214639c(void *other);

struct PortPmf { unsigned fn; int delta; };
/* the seven statics __sinit_ov100_02147bc0 copies into data_ov100_02148a1c */
extern PortPmf data_ov100_02148448[], data_ov100_02148450[],
    data_ov100_02148458[], data_ov100_02148460[], data_ov100_02148468[],
    data_ov100_02148470[], data_ov100_02148478[];
extern PortPmf data_ov100_02148a1c[];

void func_ov100_021463b0(void *);
void func_ov100_02146468(void *);
void func_ov100_021464f4(void *);
void func_ov100_02146640(void *);
void func_ov100_021467d4(void *);
void func_ov100_021467e8(void *);
void func_ov100_02146828(void *);

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
 /* Fish::Behavior has TWO dispatch sites on this one table (the guarded arm
  * and the else arm of the same `if`), both the same shape, both arity zero:
  * one set of faces serves both.
  */
#define FISH_FACE(cell, sym)                                              \
    static void __fastcall fish_c##cell(void *self, void *dead_edx)       \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }

FISH_FACE(0, func_ov100_021467d4)
FISH_FACE(1, func_ov100_02146828)
FISH_FACE(2, func_ov100_021464f4)
FISH_FACE(3, func_ov100_021467e8)
FISH_FACE(4, func_ov100_02146640)
FISH_FACE(5, func_ov100_02146468)
FISH_FACE(6, func_ov100_021463b0)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_fish_states[] = {
    {data_ov100_02148448, 0x021467d4, (void *)fish_c0},
    {data_ov100_02148450, 0x02146828, (void *)fish_c1},
    {data_ov100_02148458, 0x021464f4, (void *)fish_c2},
    {data_ov100_02148460, 0x021467e8, (void *)fish_c3},
    {data_ov100_02148468, 0x02146640, (void *)fish_c4},
    {data_ov100_02148470, 0x02146468, (void *)fish_c5},
    {data_ov100_02148478, 0x021463b0, (void *)fish_c6},
};

extern "C" void port_fish_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_fish_states / sizeof g_fish_states[0];
         ++i) {
        PortPmf *p = g_fish_states[i].slot;
        if (p->fn != g_fish_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Fish state %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_fish_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_fish_states[i].host;
    }
}

/* HOST COPY RETIRED, run link100 lane FWD. Fish::Behavior
 * dispatches this table from src/_ZN4Fish8BehaviorEv.cpp now. The flat C name the port's
 * actor-class face calls is defined by the forwarder in
 * port/hal/fwd_forwarders.cpp, which receives `this` on the stack and calls
 * the member through the real class type; the member and the flat name are two
 * different symbols with two different conventions, so no /alternatename could
 * have bridged them. The cells this seat installs are __fastcall faces for the
 * same reason.
 */
