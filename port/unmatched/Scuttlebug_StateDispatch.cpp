/* HOST COPIES of the SCUTTLEBUG's (Scuttlebug / daSpd_c, actor 255, ov071) two
 * state dispatchers, and the seat of the table they read.
 *
 * Scuttlebug drives a nine-state machine through data_ov071_02122fa8, nine
 * 16-byte records each holding an {ENTER, MAIN} pair of mwcc {fn, delta} PMFs.
 * Scuttlebug_SetState(c, idx) stores &data_ov071_02122fa8[idx] (16-byte stride,
 * the ROM's ADD r1,r2,r1,LSL#4) into c+0x380, and two dispatchers read that
 * stored pointer:
 *
 *   func_ov071_021202b4(c)   calls state idx's ENTER (record+0), on SetState
 *   func_ov071_02120278(c)   calls state idx's MAIN  (record+8), every frame
 *                            (Scuttlebug::Behavior calls it)
 *
 * The nine records are not copied from separate {init, main} statics the way
 * the Crate's are: __sinit_ov071_021226ac copies eighteen 8-byte SOURCE PMFs at
 * data_ov071_02122b60..02122be8 into the bss array, two per record, so the
 * array's ENTER halves come from the even-copied sources and the MAIN halves
 * from the odd-copied ones. The mapping was read out of the sinit's own
 * store offsets (below), not guessed.
 *
 * NEITHER DISPATCHER IS A HOST COPY ANY MORE, and the two paragraphs below are
 * kept because they are the derivation, not because they still rule. Wave 18
 * put the MAIN half back on the slice and run link100 lane PMF put the ENTER
 * half back; both notes sit at the bodies' old positions further down. What
 * changed under them is reason 1: /vmg /vmm is global now, so MSVC's member
 * pointer IS the ROM's 8-byte {fn, delta}. Reason 2 and the seat below are
 * untouched and still needed.
 *
 * Both dispatcher TUs were the OneUpMushroom / MontyMole case and were host
 * copies for the same two reasons:
 *
 *   1. MSVC forms a pointer-to-member of the `struct C` the matched TUs
 *      declare. func_ov071_021202b4 reads `*p` -- a 4-byte MSVC PMF over the
 *      complete local C vs the 8-byte {fn, delta} the ROM record is -- which
 *      calls the fn by accident because delta is 0. But func_ov071_02120278
 *      forms `c->pp + 1`, and mwcc's PMF is 8 bytes while MSVC's is 4, so the
 *      host `+ 1` lands on the record's +4 (the delta=0 word) instead of the
 *      MAIN PMF at +8 and dispatches a zero. Reading each record as plain
 *      { fn, 0 } halves and calling the fn with `this` is the fix.
 *
 *   2. The words the sinit copies are the overlay image's own -- DS CODE
 *      ADDRESSES, the ovdata contract, matched against ov071's relocs. The
 *      seat below rewrites each SOURCE PMF's first word with its host body
 *      BEFORE __sinit_ov071_021226ac copies it into the runtime array (the
 *      MontyMole / Crate reading: seat the source, before the copy, one less
 *      mapping to get wrong), each checked against the ROM address the body
 *      was compiled from, so a mount pointing at the wrong bytes says so
 *      instead of copying an overlay-image address into the live table.
 *
 * NOTE ON THE GROUND TRUTH: the scoping note that "there is NO sinit copy, the
 * mounted array IS the live table" is HALF right -- the mounted array is bss
 * and the live table, but __sinit_ov071_021226ac (site 0x02122890 loads
 * 0x02122fa8, the block copy at 0x02122700..0x02122850 fills +0x00..+0x8c)
 * DOES fill it from the eighteen sources. Seating the sources before the copy
 * (this file) is therefore the correct order and matches the MontyMole seat;
 * seating the bss array in place would have to run AFTER the sinit and is not
 * done here. See the invocation order in hal/actor_overlays.cpp.
 *
 * The table is the bug's OWN -- only Scuttlebug's own TUs reference
 * Scuttlebug_SetState / the two dispatchers -- and all eighteen targets are
 * matched src (slice_gate176.txt), so there are no state traps. All eighteen
 * .delta halves are 0 (non-virtual complete-class form, per the overlay bytes).
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
/* the eighteen state halves, by ROM address; all matched src. ENTER halves are
   dispatched by func_ov071_021202b4 on SetState, MAIN halves by
   func_ov071_02120278 from Behavior. */
void func_ov071_02120200(void *c);   /* state 0 enter */
void func_ov071_021201b4(void *c);   /* state 0 main  */
void func_ov071_02120130(void *c);   /* state 1 enter */
void func_ov071_02120028(void *c);   /* state 1 main  */
void func_ov071_0211ff84(void *c);   /* state 2 enter */
void func_ov071_0211fee4(void *c);   /* state 2 main  */
void func_ov071_0211fe38(void *c);   /* state 3 enter */
void func_ov071_0211fd58(void *c);   /* state 3 main  */
void func_ov071_0211fcd4(void *c);   /* state 4 enter */
void func_ov071_0211fc60(void *c);   /* state 4 main  */
void func_ov071_0211fbf4(void *c);   /* state 5 enter */
void func_ov071_0211fb24(void *c);   /* state 5 main  */
void func_ov071_0211fb0c(void *c);   /* state 6 enter */
void func_ov071_0211fa54(void *c);   /* state 6 main  */
void func_ov071_0211f8d0(void *c);   /* state 7 enter */
void func_ov071_0211f7d4(void *c);   /* state 7 main  */
void func_ov071_0211f6f8(void *c);   /* state 8 enter */
void func_ov071_0211f694(void *c);   /* state 8 main  */

struct PortPmf { unsigned fn; int delta; };
/* the RUNTIME array the dispatchers read (bss, filled by the sinit); nine
   16-byte records, each two PortPmf {enter, main} */
extern PortPmf data_ov071_02122fa8[18];
/* the eighteen SOURCE PMFs __sinit_ov071_021226ac copies from, in ADDRESS
   order (the mount's order). The sinit's copy order maps each to a runtime
   half -- see g_scuttlebug_sources below, which lists source-by-source. */
extern PortPmf data_ov071_02122b60[], data_ov071_02122b68[],
    data_ov071_02122b70[], data_ov071_02122b78[], data_ov071_02122b80[],
    data_ov071_02122b88[], data_ov071_02122b90[], data_ov071_02122b98[],
    data_ov071_02122ba0[], data_ov071_02122ba8[], data_ov071_02122bb0[],
    data_ov071_02122bb8[], data_ov071_02122bc0[], data_ov071_02122bc8[],
    data_ov071_02122bd0[], data_ov071_02122bd8[], data_ov071_02122be0[],
    data_ov071_02122be8[];
}  /* extern "C" */

/* func_ov071_021202b4 -- the ENTER half, `c->pp[0]` -- WAS host-copied here,
   and is now back on the slice: run link100 lane PMF makes /vmg /vmm GLOBAL
   (port/CMakeLists.txt, above R8), so the matched src/func_ov071_021202b4.cpp
   forms the ROM's own 8-byte {fn, delta} record instead of MSVC's four-byte
   default. Everything wave 18 checked for the MAIN half below covers this half
   unchanged -- same table, same seat, same eighteen ROM-zero deltas, and the
   same tail jump, which is what carries `this` on the caller's own frame onto
   the plain cdecl bodies g_scuttlebug_sources installs. Its caller
   src/Scuttlebug_SetState.c is a plain .c cdecl TU that was already linked, so
   there is no new convention at the call site either. See port/slice_w18a.txt's
   own link100 section. */

/* func_ov071_02120278 -- the MAIN half, `c->pp + 1` -- WAS host-copied here
   for the width reason this file's header states, and is now back on the
   slice: run linkw wave 18 compiles src/func_ov071_02120278.cpp with
   /vmg /vmm, which gives MSVC the 8-byte {fn, delta} representation the ROM's
   record already is, so the matched TU strides eight and reads the record's
   own function word. All eighteen of this table's source deltas are ROM zeros,
   so the matched dispatch (which adds delta to `this`) and the host body it
   replaces (which did not) agree word for word. See port/slice_w18a.txt and
   the R9 block in port/CMakeLists.txt. The ENTER half above stays a host copy:
   it reads record 0, where the wrong stride never bit, so nothing forced it
   off the slice and nothing about this change puts it back on. */

/* Each SOURCE PMF, in the mount's address order, with the ROM fn it holds and
   the host body that replaces it. Read out of ov071's relocs (the word at each
   source address relocates to the fn). */
static const struct { PortPmf *slot; unsigned rom; void (*host)(void *); }
g_scuttlebug_sources[] = {
    {data_ov071_02122b60, 0x0211fee4, func_ov071_0211fee4},  /* state 2 main  */
    {data_ov071_02122b68, 0x02120130, func_ov071_02120130},  /* state 1 enter */
    {data_ov071_02122b70, 0x02120028, func_ov071_02120028},  /* state 1 main  */
    {data_ov071_02122b78, 0x0211ff84, func_ov071_0211ff84},  /* state 2 enter */
    {data_ov071_02122b80, 0x021201b4, func_ov071_021201b4},  /* state 0 main  */
    {data_ov071_02122b88, 0x02120200, func_ov071_02120200},  /* state 0 enter */
    {data_ov071_02122b90, 0x0211fa54, func_ov071_0211fa54},  /* state 6 main  */
    {data_ov071_02122b98, 0x0211f694, func_ov071_0211f694},  /* state 8 main  */
    {data_ov071_02122ba0, 0x0211f6f8, func_ov071_0211f6f8},  /* state 8 enter */
    {data_ov071_02122ba8, 0x0211f7d4, func_ov071_0211f7d4},  /* state 7 main  */
    {data_ov071_02122bb0, 0x0211f8d0, func_ov071_0211f8d0},  /* state 7 enter */
    {data_ov071_02122bb8, 0x0211fb0c, func_ov071_0211fb0c},  /* state 6 enter */
    {data_ov071_02122bc0, 0x0211fb24, func_ov071_0211fb24},  /* state 5 main  */
    {data_ov071_02122bc8, 0x0211fbf4, func_ov071_0211fbf4},  /* state 5 enter */
    {data_ov071_02122bd0, 0x0211fc60, func_ov071_0211fc60},  /* state 4 main  */
    {data_ov071_02122bd8, 0x0211fcd4, func_ov071_0211fcd4},  /* state 4 enter */
    {data_ov071_02122be0, 0x0211fd58, func_ov071_0211fd58},  /* state 3 main  */
    {data_ov071_02122be8, 0x0211fe38, func_ov071_0211fe38},  /* state 3 enter */
};

extern "C" void port_scuttlebug_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_scuttlebug_sources /
                             sizeof g_scuttlebug_sources[0]; ++i) {
        PortPmf *p = g_scuttlebug_sources[i].slot;
        if (p->fn != g_scuttlebug_sources[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Scuttlebug source %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_scuttlebug_sources[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_scuttlebug_sources[i].host;
    }
}
