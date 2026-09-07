/* HOST COPIES of the CRATE's (Crate / daObjBlockS_c, actor 194, ov098) two
 * state dispatchers, and the seat of the table they read.
 *
 * Crate::Behavior and Crate::InitResources drive a seven-state machine through
 * data_ov098_0213c878, fourteen {function, delta} records that
 * __sinit_ov098_0213c058 copies out of fourteen source statics at ov098
 * 0x0213c458..0x0213c4c0. The records pair up: state j is {init = record[2j],
 * main = record[2j+1]}, and the ROM reaches them through two dispatchers:
 *
 *   Crate_SetState(c, i)        sets c->idx = i and calls state i's INIT
 *   func_ov098_02138b70(c)      calls state c->idx's MAIN, every frame
 *
 * Both TUs are the OneUpMushroom / ExclamationSwitch case and must be host
 * copies for the same two reasons:
 *
 *   1. MSVC forms a pointer-to-member of the INCOMPLETE `struct C;` they both
 *      declare as the four-word general representation, which strides the
 *      table at 0x20 instead of 0x10 and dispatches a neighbour's body.
 *      Reading the record as a plain { function, 0 } pair and calling the
 *      function with `this` is the fix.
 *
 *   2. The words the sinit copies are the overlay image's own -- DS CODE
 *      ADDRESSES, the ovdata contract, matched against ov098's relocs. The
 *      seat below rewrites each SOURCE static's first word with its host body
 *      BEFORE the sinit copies it into the runtime table (the WaterBomb /
 *      LakituBro reading: one less mapping to get wrong), each checked against
 *      the ROM address the body was compiled from, so a mount pointing at the
 *      wrong bytes says so instead of calling into the overlay image.
 *
 * The table is the crate's OWN -- only Crate's methods reference
 * Crate_SetState / func_ov098_02138b70 -- and all fourteen targets are matched
 * src (slice_gate172.txt), so there are no state traps. All fourteen .delta
 * halves are 0 (non-virtual complete-class form, per the overlay bytes).
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
/* the fourteen state halves, table order; all matched src */
void func_ov098_02138b18(char *c);   /* state 0 init */
void func_ov098_021389f8(char *c);   /* state 0 main */
void func_ov098_021389cc(char *c);   /* state 1 init */
void func_ov098_021388bc(char *c);   /* state 1 main */
void func_ov098_02138818(char *c);   /* state 2 init */
void func_ov098_02138734(char *c);   /* state 2 main */
void func_ov098_021385e0(char *c);   /* state 3 init */
void func_ov098_021384fc(char *c);   /* state 3 main */
void func_ov098_02138484(char *c);   /* state 4 init */
void func_ov098_02138344(char *c);   /* state 4 main */
void func_ov098_02138318(char *c);   /* state 5 init */
void func_ov098_02138238(char *c);   /* state 5 main */
void func_ov098_021381e8(char *c);   /* state 6 init */
void func_ov098_0213814c(char *c);   /* state 6 main */

struct PortPmf { unsigned fn; int delta; };
/* the RUNTIME table the dispatchers read (bss, filled by the sinit) */
extern PortPmf data_ov098_0213c878[14];
/* the fourteen SOURCE statics __sinit_ov098_0213c058 copies from, in the
   sinit's copy order = table order 0..13 */
extern PortPmf data_ov098_0213c4a8[], data_ov098_0213c488[],
    data_ov098_0213c490[], data_ov098_0213c468[], data_ov098_0213c498[],
    data_ov098_0213c470[], data_ov098_0213c4b0[], data_ov098_0213c480[],
    data_ov098_0213c4a0[], data_ov098_0213c478[], data_ov098_0213c458[],
    data_ov098_0213c460[], data_ov098_0213c4c0[], data_ov098_0213c4b8[];
}  /* extern "C" */

/* BOTH DISPATCHERS ARE BACK ON THE SLICE. src/Crate_SetState.cpp and
   src/func_ov098_02138b70.cpp are on port/slice_pmf3.txt (run link100 lane
   PMF3): /vmg /vmm makes the pointer-to-member the ROM's 8-byte record, the
   fourteen-record table strides 0x10, and both bodies TAIL JUMP. The mangled
   reference to the table is bridged in port/hal/pmf3_aliases.cpp. The seat
   below is unchanged and is the gate -- it aborts on a nonzero adjustment
   word and rewrites all fourteen function words with host bodies before a
   crate can spawn -- and the fourteen source statics at ov098
   0x0213c458..0x0213c4c0 were re-read out of overlay_0098.bin with their
   relocations, every adjustment word ROM zero. */

static const struct { PortPmf *slot; unsigned rom; void (*host)(char *); }
g_crate_states[] = {
    {data_ov098_0213c4a8, 0x02138b18, func_ov098_02138b18},
    {data_ov098_0213c488, 0x021389f8, func_ov098_021389f8},
    {data_ov098_0213c490, 0x021389cc, func_ov098_021389cc},
    {data_ov098_0213c468, 0x021388bc, func_ov098_021388bc},
    {data_ov098_0213c498, 0x02138818, func_ov098_02138818},
    {data_ov098_0213c470, 0x02138734, func_ov098_02138734},
    {data_ov098_0213c4b0, 0x021385e0, func_ov098_021385e0},
    {data_ov098_0213c480, 0x021384fc, func_ov098_021384fc},
    {data_ov098_0213c4a0, 0x02138484, func_ov098_02138484},
    {data_ov098_0213c478, 0x02138344, func_ov098_02138344},
    {data_ov098_0213c458, 0x02138318, func_ov098_02138318},
    {data_ov098_0213c460, 0x02138238, func_ov098_02138238},
    {data_ov098_0213c4c0, 0x021381e8, func_ov098_021381e8},
    {data_ov098_0213c4b8, 0x0213814c, func_ov098_0213814c},
};

extern "C" void port_crate_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_crate_states /
                             sizeof g_crate_states[0]; ++i) {
        PortPmf *p = g_crate_states[i].slot;
        if (p->fn != g_crate_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Crate state %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_crate_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_crate_states[i].host;
    }
}
