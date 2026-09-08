/* THE BOSS REWARD'S STATE MACHINE -- ov089's one pointer-to-member dispatch
 * table, seated over the MAXIMUM SPAN its .bss section allows, and the method
 * that reads it is the decomp's own TU now.
 *
 * ==== THE TABLE =============================================================
 *
 * data_ov089_02132cec, bss, EIGHT filled 8-byte {code, adj} records, filled by
 * __sinit_ov089_021328d4 out of eight .data source statics at 0x02132b00..
 * 0x02132b38, and read by Key::Behavior with `PMFTABLE[mState].pmf`. Read from
 * extracted/overlays/overlay_0089.bin (base 0x02130f00) and cross-checked
 * against config/arm9/overlays/ov089/relocs.txt: all eight adj halves are 0,
 * and the eight records name only THREE distinct bodies --
 *
 *   state 0,1,2,4,5,6  func_ov089_02131b18   (the idle/hover body)
 *   state 3            func_ov089_0213162c
 *   state 7            func_ov089_021311c0
 *
 * -- all three matched src, all three in port/slice_w6a.txt.
 *
 * ==== THE EXTENT IS BOUNDED, WHICH IS WHY THIS ROW MOVED --------------------
 *
 * Lane FWD refused this row because data_ov089_02132cec is the LAST symbol in
 * config/arm9/overlays/ov089/symbols.txt, so the next-symbol rule cannot say
 * where the table ends, and "a seat over an unclosed extent can leave a cell
 * the matched TU would CALL". delinks.txt bounds it instead: ov089's .bss runs
 * 0x02132c40..0x02132d40, the filled run ends at 0x02132d2c, and the slack is
 * 0x14 bytes = TWO more 8-byte cells. The maximum table length is TEN cells,
 * not unbounded (runs/link100/out/PMFB6/rom_gate2.txt).
 *
 * So this seat covers ten cells. Cells 0..7 are seated through the source
 * statics before the sinit copies them, each checked against the ROM address
 * its host body was compiled from. Cells 8 and 9 have no source static -- the
 * sinit never writes them and the mount holds zero there -- so they are
 * written DIRECTLY with an aborting face, after asserting that the mounted
 * bytes really are zero. A zero cell would have been CALLED as address 0; an
 * aborting face names itself instead. Nothing else can reach those two cells:
 * they are inside the mounted 84-byte object (0x54 = the whole span to the
 * .bss end, pinned in port/ov089_syms.txt).
 *
 * ==== WHY THE CELLS ARE __fastcall FACES NOW ================================
 *
 * The old note here said MSVC forms this pointer-to-member as FOUR bytes
 * because src/_ZN3Key8BehaviorEv.cpp declares `struct C { virtual void
 * dummy(); };` -- a complete single-inheritance class. That was measured
 * before port/CMakeLists.txt block R8 turned /vmg /vmm on for every C++ source
 * in the target. Re-measured for this gate off the matched TU's own /FAsc
 * listing under the port's own flags (runs/link100/out/PMFB6/emit_gate2_out.txt):
 *
 *     mov ecx, ?data_ov089_02132cec@@3PAUPmfEntry@@A[eax*8+4]
 *     mov eax, ?data_ov089_02132cec@@3PAUPmfEntry@@A[eax*8]
 *     add ecx, edi
 *     call eax
 *
 * [eax*8] and [eax*8+4] -- the two words of ONE ROM record, receiver in ecx,
 * NOTHING pushed and no `add esp` after (the `add esp, 8` three instructions
 * earlier is the cleanup of the preceding cdecl Actor::UpdatePos call, read in
 * the listing rather than counted as an argument). ARITY ZERO. /Zp4 changes 0
 * listing lines outside the TITLE, so it is not claimed on the gate block.
 *
 * That is __thiscall, and the state bodies are cdecl func_ov089_xxxxxxxx(char
 * *), so every cell holds a zero-argument __fastcall FACE that forwards the
 * receiver as the cdecl argument. ONE FACE PER CELL, not per body: six of the
 * eight carry the same code word and stay distinguishable that way.
 *
 * Key::Behavior is a real C++ MEMBER (?Behavior@Key@@QAEHXZ, read off the
 * matched TU's own object), so the flat C name the port's actor-class face
 * calls is defined by the forwarder in port/hal/fwd_forwarders.cpp.
 *
 * BOTH KEY (282) and LAST_STAR (283) run it -- they share _ZTV3Key, the only
 * vtable ov089 defines, and the mActorID == 0x11a test in the body itself is
 * how it tells the two apart.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int adj; };

/* the eight source statics, in the sinit's copy order = table order 0..7 */
extern PortPmf data_ov089_02132b28[], data_ov089_02132b38[],
    data_ov089_02132b00[], data_ov089_02132b30[], data_ov089_02132b20[],
    data_ov089_02132b10[], data_ov089_02132b08[], data_ov089_02132b18[];
/* the runtime table (bss, filled by the sinit); the mount is the whole
   0x54-byte span to the end of ov089's .bss, so cells 8 and 9 are real
   storage this file can write */
extern PortPmf data_ov089_02132cec[];

/* the three state bodies, all matched src */
void func_ov089_02131b18(char *c);
void func_ov089_0213162c(char *c);
void func_ov089_021311c0(char *c);

}  /* extern "C" */

/* HOST COPY RETIRED, run link100 lane PMFB6. src/_ZN3Key8BehaviorEv.cpp
   dispatches this table itself now; the flat C name the port's actor-class
   face calls is defined by the forwarder in port/hal/fwd_forwarders.cpp. */

static void ov089_unhosted(unsigned idx)
{
    std::fprintf(stderr, "UNHOSTED: data_ov089_02132cec[%u] is past the eight "
                 "cells __sinit_ov089_021328d4 fills. ov089's .bss allows ten, "
                 "so this cell exists and the ROM never writes it -- see the "
                 "header of port/unmatched/Ov089_StateDispatch.cpp\n", idx);
    std::abort();
}

#define OV089_FACE(cell, sym)                                             \
    static void __fastcall ov089_c##cell(void *self, void *dead_edx)      \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym((char *)self);                                                \
    }
#define OV089_ABORT(cell)                                                 \
    static void __fastcall ov089_c##cell(void *self, void *dead_edx)      \
    {                                                                     \
        (void)self;                                                       \
        (void)dead_edx;                                                   \
        ov089_unhosted(cell);                                             \
    }

OV089_FACE(0, func_ov089_02131b18)
OV089_FACE(1, func_ov089_02131b18)
OV089_FACE(2, func_ov089_02131b18)
OV089_FACE(3, func_ov089_0213162c)
OV089_FACE(4, func_ov089_02131b18)
OV089_FACE(5, func_ov089_02131b18)
OV089_FACE(6, func_ov089_02131b18)
OV089_FACE(7, func_ov089_021311c0)
OV089_ABORT(8)
OV089_ABORT(9)

namespace {
struct Seat { PortPmf *slot; unsigned rom; void *host; const char *tab; };
const Seat g_ov089_states[] = {
    {data_ov089_02132b28, 0x02131b18, (void *)ov089_c0, "cec[0]"},
    {data_ov089_02132b38, 0x02131b18, (void *)ov089_c1, "cec[1]"},
    {data_ov089_02132b00, 0x02131b18, (void *)ov089_c2, "cec[2]"},
    {data_ov089_02132b30, 0x0213162c, (void *)ov089_c3, "cec[3]"},
    {data_ov089_02132b20, 0x02131b18, (void *)ov089_c4, "cec[4]"},
    {data_ov089_02132b10, 0x02131b18, (void *)ov089_c5, "cec[5]"},
    {data_ov089_02132b08, 0x02131b18, (void *)ov089_c6, "cec[6]"},
    {data_ov089_02132b18, 0x021311c0, (void *)ov089_c7, "cec[7]"},
};
/* the slack the .bss end allows past the filled run */
void *const g_ov089_tail[] = { (void *)ov089_c8, (void *)ov089_c9 };
}  /* namespace */

/* Seats the SOURCE statics before __sinit_ov089_021328d4 copies them, and the
   two TAIL cells directly (the sinit never touches them). That sinit runs from
   hal/actor_overlays.cpp's ov089 block, which is where the call is. */
extern "C" void port_ov089_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_ov089_states / sizeof g_ov089_states[0];
         ++i) {
        PortPmf *p = g_ov089_states[i].slot;
        if (p->fn != g_ov089_states[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: ov089 state %s: the mount holds "
                         "%08x/%d, the ROM's own record says %08x/0 -- WRONG "
                         "BYTES\n", g_ov089_states[i].tab, p->fn, p->adj,
                         g_ov089_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_ov089_states[i].host;
    }
    /* THE TAIL. The extent bound says ten cells; the sinit fills eight. The
       two past the filled run must read {0,0} in the mount -- a non-zero word
       there would mean the table is longer than delinks.txt says and this seat
       has the extent wrong, which is worth stopping for. */
    for (unsigned k = 0; k < sizeof g_ov089_tail / sizeof g_ov089_tail[0]; ++k) {
        PortPmf *p = &data_ov089_02132cec[8 + k];
        if (p->fn != 0 || p->adj != 0) {
            std::fprintf(stderr, "FATAL: ov089 tail cell cec[%u] reads %08x/%d "
                         "and the ROM never writes it -- the table is longer "
                         "than ov089's .bss extent says\n", 8 + k, p->fn,
                         p->adj);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_ov089_tail[k];
        p->adj = 0;
    }
}
