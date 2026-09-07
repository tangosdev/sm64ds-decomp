/* HOST COPIES of src/func_ov102_02149da8.cpp and src/func_ov102_02149df0.cpp
 * -- the question block's state machine -- plus the seat for the six
 * {function, delta} statics they dispatch. The gate-16 pointer-to-member case
 * for the seventh and last time on this level.
 *
 * The shape is the door's, one level simpler: three entries of TWO
 * pointers-to-member each at ov102 0x0214e890, "enter" at +0 and "main" at
 * +8. func_ov102_02149da8(c, i) sets the state and runs its enter half;
 * func_ov102_02149df0(c) runs the current state's main half, and
 * QuestionBlock::Behavior calls that on its own first line.
 *
 * Both source TUs write `struct C;` and form the pointer-to-member while C is
 * INCOMPLETE, so MSVC answers with the four-word general representation and
 * the table's stride doubles. Spelled here as the two plain words the ROM
 * stores; the encoding is the one read off the ROM in Door_Behavior.cpp, and
 * all six deltas are zero here too.
 *
 * ov102 0x021498e0 -- state 1's main half, the block bouncing after it is hit
 * -- IS matched now (src/func_ov102_021498e0.cpp) and hosted, with its two
 * pointer-to-member content tables, in
 * port/unmatched/QuestionBlock_BounceDispatch.cpp (gate 180). The state row for
 * it below points at that host frame; the old abort stub is gone. Hitting a
 * ? block no longer aborts.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };
/* the six statics __sinit_ov102_0214d908 copies into data_ov102_0214e890 */
extern PortPmf data_ov102_0214e278[], data_ov102_0214e260[],
    data_ov102_0214e268[], data_ov102_0214e258[], data_ov102_0214e3a0[],
    data_ov102_0214e398[];
extern PortPmf data_ov102_0214e890[];      /* 3 entries x 2 pmfs */

void func_ov102_02149d80(void *);   /* state 0 enter */
void func_ov102_02149ccc(void *);   /* state 0 main  */
void func_ov102_02149c78(void *);   /* state 1 enter */
void func_ov102_021498e0(void *);   /* state 1 main  (host copy, gate 180) */
void func_ov102_021498c4(void *);   /* state 2 enter */
void func_ov102_02149878(void *);   /* state 2 main  */

}  /* extern "C" */

static const struct { PortPmf *slot; unsigned rom; void (*host)(void *); }
g_qblock_states[] = {
    {data_ov102_0214e278, 0x02149d80, func_ov102_02149d80},
    {data_ov102_0214e260, 0x02149ccc, func_ov102_02149ccc},
    {data_ov102_0214e268, 0x02149c78, func_ov102_02149c78},
    {data_ov102_0214e258, 0x021498e0, func_ov102_021498e0},
    {data_ov102_0214e3a0, 0x021498c4, func_ov102_021498c4},
    {data_ov102_0214e398, 0x02149878, func_ov102_02149878},
};

extern "C" void port_question_block_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0;
         i < sizeof g_qblock_states / sizeof g_qblock_states[0]; ++i) {
        PortPmf *p = g_qblock_states[i].slot;
        if (p->fn != g_qblock_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: QuestionBlock state %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, p->fn, p->delta,
                         g_qblock_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_qblock_states[i].host;
    }
}

/* BOTH DISPATCHERS ARE BACK ON THE SLICE. src/func_ov102_02149da8.cpp and
   src/func_ov102_02149df0.cpp are on port/slice_pmf3.txt (run link100 lane
   PMF3): /vmg /vmm makes MSVC's pointer-to-member the ROM's 8-byte record, so
   the three-entry table strides 0x10 and both bodies TAIL JUMP;
   port/hal/pmf3_aliases.cpp bridges the mangled table name. The seat above is
   unchanged and is the gate -- it aborts on a nonzero adjustment word and
   rewrites all six function words with host bodies -- and the six source
   statics at ov102 0x0214e258..0x0214e3a0 were re-read out of overlay_0102.bin
   with their relocations, every adjustment word ROM zero.

   port_qblock_pmf goes with them. It bounds-checked the state index and
   ABORTED, which its own comment says the ROM does not do; because it aborted
   rather than skipped, every green battery is proof it never fired. */
