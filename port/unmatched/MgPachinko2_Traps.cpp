// PORT_HOST_ABI. dScMgPachinko2_c's NAMED TRAP: the one ov006 address in this
// class's closure that has no delink block and no src TU and that a REFERENCED
// body names by symbol. Actor id 0x171, scene 369. Run mg6, lane LKT.
//
// ---- WHY A TRAP AND NOT A BODY -------------------------------------------
//
// port/hal/scene_mg_faces.cpp section 3 is the precedent and its argument is
// not repeated here beyond the one sentence that decides it: a plausible body
// is worse than an absent one, because a plausible body is indistinguishable
// from a correct one in every check this tree runs. The trap below is named,
// counted and reports itself once, so a run that enters it says so instead of
// producing a subtly wrong frame.
//
// THIS FILE DEFINES ONLY WHAT SOMETHING REFERENCES. Five ov006 addresses in
// this class's closure have no source at all; ONE is here and four are not,
// and the split is measured rather than chosen:
//
//   0x0210076c  size 0x24c. Called by func_ov006_0210246c, the fifteen-state
//               dispatcher, immediately after every dispatch it makes -- so
//               its reference is a real symbol in a file this port compiles
//               (unmatched/MgPachinko2_StateDispatch.cpp) and the link fails
//               without a definition. It is also the ONLY symbol the whole
//               slice references with nothing defining it, which is measured:
//               a sweep of every func_ov004_*/func_ov006_* identifier in
//               port/slice_lkt.txt against src/ returns this name and no
//               other.
//
//   NOT HERE, because nothing names them: 0x020ffb54 and 0x020ff8c8 (states 2
//   and 3 of data_ov006_021426cc), 0x02102274 (state 0 of
//   data_ov006_02142734) and 0x02102fe8 (state 1 of data_ov006_021426f4). All
//   four are reached only through a mounted pair word, so there is no symbol
//   to resolve and a definition here would be an unreferenced invention. They
//   are handled at the dispatch site in
//   unmatched/MgPachinko2_StateDispatch.cpp, which reports the address and
//   calls nothing -- the treatment unmatched/MgCurling_StateDispatch.cpp gave
//   func_ov006_020e1854 before run link60 lane CT1 transcribed it.
//
// ---- THE SIGNATURE IS THE ROM'S, AND THAT IS NOT COSMETIC ----------------
//
// Run link60 lane CUR2 recorded the cost of getting this wrong: curling's two
// collision traps took (void *) where the ROM takes (self, idx), so when the
// bodies were finally transcribed the veneers dropped the index and collided
// shell 0 every time. This arity is checked TWICE.
//
// AT THE CALL SITE, extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//     021024BC  mov  r0, r6      ; self, unadvanced
//     021024C0  mov  r1, r5      ; the loop index, 0..2
//     021024C4  bl   #0x210076c
//
// AND IN THE CALLEE'S OWN PROLOGUE:
//
//     0210076C  push  {r4, r5, r6, r7, r8, sb, sl, fp, lr}
//     02100770  sub   sp, sp, #0x1c
//     02100774  mov   sb, r0                 ; self
//     02100778  add   r4, sb, r1, lsl #6     ; index, stride 0x40
//     0210077C  add   r0, r4, #0x5000
//     02100780  ldrb  r0, [r0, #0x296]       ; the SAME +0x5296 the dispatcher
//                                            ; reads to pick the state
//     02100788  cmp   r0, #0     ...  early-out
//     02100798  cmp   r0, #0xc   ...  early-out on >= 12
//
// so it takes (self, index) over the same 0x40-stride array its caller walks
// and re-reads the same state index. Two arguments, and the second one is
// load-bearing.
//
// WHAT THE PORT LOSES BY TRAPPING IT. The body is 0x24c bytes and switches on
// the state index, so whatever per-entry follow-up work the ROM does after
// each of the fifteen states does not happen. The dispatch itself is
// unaffected: the trap is called AFTER the state body returns. That is worth
// saying plainly rather than leaving to be discovered -- this class boots and
// ticks with a hole in it, and the hole has a name and a counter.

#include <cstdio>

static unsigned g_lkt_trap_hits;

extern "C" {

void func_ov006_0210076c(void *c, int i)
{
    static int said;
    (void)c;
    ++g_lkt_trap_hits;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [mg] TRAP func_ov006_0210076c(self, %d): "
                     "dScMgPachinko2_c's per-entry state follow-up has NO "
                     "DECOMPILED BODY (config symbol, no delink block, no src "
                     "TU, 0x24c bytes). Returning without doing it.\n", i);
        std::fflush(stderr);
    }
}

unsigned port_mg_pachinko2_trap_hits(void)
{
    return g_lkt_trap_hits;
}

}  /* extern "C" */
