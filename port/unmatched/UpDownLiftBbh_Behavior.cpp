/* THE UP_DOWN_LIFT's STATE TABLE, SEATED -- and the method that reads it is
 * the decomp's own TU now (UpDownLiftBbh / daUdlift_c, actor 33 and its
 * Bbh/Hmc/Rr siblings, ov095).
 *
 * UpDownLiftBbh::Behavior drives a five-state machine through
 * data_ov095_02137910, five {function, delta} records that
 * __sinit_ov095_0213722c copies out of five SOURCE statics at ov095
 * 0x0213757c..0x0213759c in the `TAB.<m> = SRC` spelling (members a..e = states
 * 0..4). The records do not pair up: the ROM indexes
 * data_ov095_02137910[mState] directly, one 8-byte record per state, and calls
 * that state's function every frame. All five .delta halves are 0 and all five
 * targets are matched src (slice_gate173.txt), so there are no state traps.
 *
 * ==== WHY IT IS NOT A HOST COPY ANY MORE ====================================
 *
 * The old note here said MSVC forms the pointer-to-member of the INCOMPLETE
 * `struct Plat;` as the four-word general representation, striding the table at
 * 0x10 instead of 0x08. That was measured before port/CMakeLists.txt block R8
 * turned /vmg /vmm on for every C++ source in the target. Re-measured for this
 * gate off the matched TU's own /FAsc listing under the port's own flags
 * (runs/link100/out/PMFB6/emit_gate2_out.txt):
 *
 *     mov ecx, _data_ov095_02137910[esi*8+4]
 *     mov eax, _data_ov095_02137910[esi*8]
 *     add ecx, edi
 *     call eax
 *
 * [esi*8] and [esi*8+4], the two words of ONE ROM record. ARITY ZERO -- nothing
 * pushed between the index load and the call and no `add esp` after it (the
 * `add esp, 4` earlier is the cleanup of the preceding cdecl
 * Actor::ClosestPlayer call). /Zp4 changes 0 listing lines outside the TITLE.
 * The TU declares its table INSIDE its own extern "C" block, so it comes in as
 * the plain _data_ name and needs no /alternatename.
 *
 * The dispatch is __thiscall and the state bodies are cdecl
 * func_ov095_xxxxxxxx(char *), so each cell holds a zero-argument __fastcall
 * FACE that forwards the receiver as the cdecl argument. ONE FACE PER CELL.
 * UpDownLiftBbh::Behavior is a real C++ MEMBER (?Behavior@UpDownLiftBbh@@QAEHXZ),
 * so the flat C name the port's vtable fill calls is defined by the forwarder
 * in port/hal/fwd_forwarders.cpp.
 *
 * ==== THE EXTENT, AND THE ONE CELL THIS LANE COULD NOT SEAT =================
 *
 * Lane FWD refused this row because data_ov095_02137910 is the LAST symbol in
 * ov095's symbols.txt. delinks.txt bounds it: .bss runs
 * 0x02137780..0x02137940, the filled run ends at 0x02137938, so the slack is
 * 0x8 = ONE more 8-byte cell and the maximum table length is SIX, not
 * unbounded (runs/link100/out/PMFB6/rom_gate2.txt).
 *
 * That sixth cell is NOT SEATED and the reason is worth writing down rather
 * than leaving quiet: port/ov095_syms.txt pins this mount to `:0x28`, forty
 * bytes, exactly the five filled cells, and it is the last object in ov095's
 * pack -- so there is no mounted storage at 0x02137938 to write an aborting
 * face into, and port/ov095_syms.txt is not this lane's file. A dispatch at
 * index 5 therefore reads eight bytes past the mount, which is exactly what it
 * did through the host copy this file retires: unchanged by this gate, not
 * introduced by it. The one-line fix is a written proposal in this lane's
 * report (`data_ov095_02137910:0x28` -> `:0x30`, plus the tail face here).
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
/* the five state bodies, table order = mState order; all matched src */
void func_ov095_02136368(char *c);   /* state 0 (source 0x02137594) */
void func_ov095_02136298(char *c);   /* state 1 (source 0x0213757c) */
void func_ov095_02136178(char *c);   /* state 2 (source 0x02137584) */
void func_ov095_02136090(char *c);   /* state 3 (source 0x0213758c) */
void func_ov095_02136104(char *c);   /* state 4 (source 0x0213759c) */

struct PortPmf { unsigned fn; int delta; };
/* the five SOURCE statics __sinit_ov095_0213722c copies from, in the sinit's
   copy order = state order 0..4 */
extern PortPmf data_ov095_02137594[], data_ov095_0213757c[],
    data_ov095_02137584[], data_ov095_0213758c[], data_ov095_0213759c[];
}  /* extern "C" */

/* HOST COPY RETIRED, run link100 lane PMFB6.
   src/_ZN13UpDownLiftBbh8BehaviorEv.cpp dispatches this table itself now. */

#define UDL_FACE(cell, sym)                                               \
    static void __fastcall udl_c##cell(void *self, void *dead_edx)        \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym((char *)self);                                                \
    }

UDL_FACE(0, func_ov095_02136368)
UDL_FACE(1, func_ov095_02136298)
UDL_FACE(2, func_ov095_02136178)
UDL_FACE(3, func_ov095_02136090)
UDL_FACE(4, func_ov095_02136104)

static const struct { PortPmf *slot; unsigned rom; void *host; }
g_updownlift_states[] = {
    {data_ov095_02137594, 0x02136368, (void *)udl_c0},   /* state 0 */
    {data_ov095_0213757c, 0x02136298, (void *)udl_c1},   /* state 1 */
    {data_ov095_02137584, 0x02136178, (void *)udl_c2},   /* state 2 */
    {data_ov095_0213758c, 0x02136090, (void *)udl_c3},   /* state 3 */
    {data_ov095_0213759c, 0x02136104, (void *)udl_c4},   /* state 4 */
};

extern "C" void port_updownlift_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_updownlift_states /
                             sizeof g_updownlift_states[0]; ++i) {
        PortPmf *p = g_updownlift_states[i].slot;
        if (p->fn != g_updownlift_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: UpDownLiftBbh state %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_updownlift_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_updownlift_states[i].host;
    }
}
