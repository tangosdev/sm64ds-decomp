/* HOST COPY of src/_ZN14KnockDownPlank8BehaviorEv.cpp -- the mwcc
 * pointer-to-member dispatch read as a plain function pointer, and BOTH of the
 * class's {function, delta} tables seated with host addresses. This is the
 * MOVING_BAR_BIG (53) / MOVING_BAR_SMALL (54) and KNOCK_DOWN_PLANK (44) state
 * seat, the Fish/Door/WaterBomb treatment for exactly the same reason:
 *
 *   * The matched source forms `typedef void (C::*PMF)()` over a struct C it
 *     only FORWARD-DECLARES (`struct C;`) at the point of the typedef, so MSVC
 *     hands the pointer-to-member its GENERAL (worst-case-inheritance)
 *     representation -- a multi-word struct plus a this-adjust/vptr thunk at
 *     the call site -- not the single code pointer a complete single-
 *     inheritance class gets. Seating the bss table with a plain function
 *     address makes the general-PMF thunk read a bogus adjustment and mangle
 *     `this` into the state index; a spawned MOVING_BAR ran 133 behaviours then
 *     faulted in func_ov015_02111e60 with the arg register turned into idx.
 *     The host copy calls through the table's first word as a plain function
 *     pointer, so MSVC's PMF representation never enters. This is not a src
 *     edit and not undecompiled code.
 *
 *   * KnockDownPlank has TWO state tables, both seeded by
 *     __sinit_ov015_02113048 (linked in hal/actor_overlays.cpp), which copies
 *     fourteen SOURCE statics into the two runtime tables:
 *
 *         data_ov015_021149ec[0..6]  <- 02114500/f0/e0/d0/508/d8/e8
 *              (KnockDownPlank::Behavior's per-state dispatch, read here)
 *         data_ov015_02114a24[0..6]  <- 021144b0/f8/c8/c0/a8/b8/a0
 *              (func_ov015_02111fb8's table -- that driver reads the pair as
 *               two plain ints itself, so it needs no host copy, only the seat)
 *
 *     The .fn words the mount ships for those fourteen statics are the overlay
 *     image's own -- DS CODE ADDRESSES -- so the seat rewrites the SOURCE
 *     statics before the sinit copies them (the LakituBro/Cannon reading: one
 *     less mapping to get wrong than reseating the two runtime tables after),
 *     each checked against the ROM address its host body was compiled from. All
 *     fourteen .delta halves are 0 (the complete-class form, confirmed against
 *     the overlay bytes: no reloc at any +4 offset), so each pair is just the
 *     function pointer, and all fourteen target functions are matched src
 *     (func_ov015_02111c*..02111f*), so there is no hole and no trap.
 *
 * The Behavior body is the matched source's line for line; only the dispatch is
 * spelled as a plain call through the pair's first word, and idx is read at the
 * observed offset 0x330 (the matched source's `struct C { char pad[0x330]; int
 * idx; }`).
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* the seven statics __sinit_ov015_02113048 copies into data_ov015_021149ec
   (KnockDownPlank::Behavior's own table) */
extern PortPmf data_ov015_02114500[], data_ov015_021144f0[],
    data_ov015_021144e0[], data_ov015_021144d0[], data_ov015_02114508[],
    data_ov015_021144d8[], data_ov015_021144e8[];
/* the seven statics __sinit_ov015_02113048 copies into data_ov015_02114a24
   (func_ov015_02111fb8's table) */
extern PortPmf data_ov015_021144b0[], data_ov015_021144f8[],
    data_ov015_021144c8[], data_ov015_021144c0[], data_ov015_021144a8[],
    data_ov015_021144b8[], data_ov015_021144a0[];

extern PortPmf data_ov015_021149ec[];

/* the fourteen target bodies, all matched ov015 src */
void func_ov015_02111f6c(void *);  void func_ov015_02111eec(void *);
void func_ov015_02111e80(void *);  void func_ov015_02111df4(void *);
void func_ov015_02111d98(void *);  void func_ov015_02111d4c(void *);
void func_ov015_02111ce0(void *);
void func_ov015_02111fac(void *);  void func_ov015_02111f4c(void *);
void func_ov015_02111ee0(void *);  void func_ov015_02111e60(void *);
void func_ov015_02111dd4(void *);  void func_ov015_02111d8c(void *);
void func_ov015_02111d28(void *);

void _ZN8Platform21UpdateModelPosAndRotYEv(void *);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *);

/* the __cdecl extern-C bridge (hal/reverse_bridges.cpp) */
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *cl);

}  /* extern "C" */

/* func_ov015_02111d98.cpp calls ((Actor *)c)->UpdatePosWithOnlySpeed(0) through
 * its OWN local `struct Actor { void UpdatePosWithOnlySpeed(void *); }`, so MSVC
 * emits a __thiscall call to ?UpdatePosWithOnlySpeed@Actor@@QAEXPAX@Z (this in
 * ecx, one void* arg). The real body is the __cdecl(self, cl) bridge, so an
 * alias would pass this=0 and fault at &pos. This TU declares the identical
 * global `struct Actor`, so its definition mangles to that exact __thiscall
 * symbol and forwards this and a null CylinderClsn to the bridge. Not a src edit
 * and not undecompiled code: the same forwarding func_ov015_02111ce0 does when
 * it spells the call in the __cdecl two-arg form. */
struct Actor { void UpdatePosWithOnlySpeed(void *); };
void Actor::UpdatePosWithOnlySpeed(void *)
{ _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(this, 0); }

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
 /* ONLY THE FIRST SEVEN ROWS CHANGE. They feed data_ov015_021149ec, which
  * KnockDownPlank::Behavior dispatches. The last seven feed
  * data_ov015_02114a24, whose dispatcher is src/func_ov015_02111fb8.c -- a .c
  * file with no pointer to member in it, calling the cell as a plain cdecl
  * function pointer -- so those seven keep their plain cdecl bodies.
  */
#define KDP_FACE(cell, sym)                                               \
    static void __fastcall kdp_c##cell(void *self, void *dead_edx)        \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym(self);                                                        \
    }

KDP_FACE(0, func_ov015_02111f6c)
KDP_FACE(1, func_ov015_02111eec)
KDP_FACE(2, func_ov015_02111e80)
KDP_FACE(3, func_ov015_02111df4)
KDP_FACE(4, func_ov015_02111d98)
KDP_FACE(5, func_ov015_02111d4c)
KDP_FACE(6, func_ov015_02111ce0)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_knock_down_plank_states[] = {
    /* data_ov015_021149ec[0..6] -- the Behavior dispatch, __fastcall faces */
    {data_ov015_02114500, 0x02111f6c, (void *)kdp_c0},
    {data_ov015_021144f0, 0x02111eec, (void *)kdp_c1},
    {data_ov015_021144e0, 0x02111e80, (void *)kdp_c2},
    {data_ov015_021144d0, 0x02111df4, (void *)kdp_c3},
    {data_ov015_02114508, 0x02111d98, (void *)kdp_c4},
    {data_ov015_021144d8, 0x02111d4c, (void *)kdp_c5},
    {data_ov015_021144e8, 0x02111ce0, (void *)kdp_c6},
    /* data_ov015_02114a24[0..6] -- func_ov015_02111fb8's table, cdecl */
    {data_ov015_021144b0, 0x02111fac, (void *)func_ov015_02111fac},
    {data_ov015_021144f8, 0x02111f4c, (void *)func_ov015_02111f4c},
    {data_ov015_021144c8, 0x02111ee0, (void *)func_ov015_02111ee0},
    {data_ov015_021144c0, 0x02111e60, (void *)func_ov015_02111e60},
    {data_ov015_021144a8, 0x02111dd4, (void *)func_ov015_02111dd4},
    {data_ov015_021144b8, 0x02111d8c, (void *)func_ov015_02111d8c},
    {data_ov015_021144a0, 0x02111d28, (void *)func_ov015_02111d28},
};

extern "C" void port_knock_down_plank_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_knock_down_plank_states /
                             sizeof g_knock_down_plank_states[0]; ++i) {
        PortPmf *p = g_knock_down_plank_states[i].slot;
        if (p->fn != g_knock_down_plank_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: KnockDownPlank state %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, p->fn, p->delta,
                         g_knock_down_plank_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_knock_down_plank_states[i].host;
    }
}

/* HOST COPY RETIRED, run link100 lane FWD. KnockDownPlank::Behavior
 * dispatches this table from src/_ZN14KnockDownPlank8BehaviorEv.cpp now. The flat C name the port's
 * actor-class face calls is defined by the forwarder in
 * port/hal/fwd_forwarders.cpp, which receives `this` on the stack and calls
 * the member through the real class type; the member and the flat name are two
 * different symbols with two different conventions, so no /alternatename could
 * have bridged them. The cells this seat installs are __fastcall faces for the
 * same reason.
 */
