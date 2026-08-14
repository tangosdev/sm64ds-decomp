/* HOST COPIES of MR. I's (MrI / daEykn_c, actors 262 and 263, ov071) two state
 * dispatchers, and the seat of the table they read.
 *
 * This is the Scuttlebug treatment applied to the second class in the same
 * overlay -- see port/unmatched/Scuttlebug_StateDispatch.cpp, which this file
 * is built on. The shapes are identical; the addresses, the record count and
 * the source list are MrI's own and were re-derived from the ROM.
 *
 * ---- THE MACHINE, READ OUT OF THE ROM --------------------------------------
 * MrI drives a THREE-state machine through data_ov071_02123088 (bss), three
 * 16-byte records each holding an {ENTER, MAIN} pair of mwcc {fn, delta} PMFs.
 *
 *   func_ov071_02121634(c, idx)  -- MrI_SetState. `add r1, r2, r1, lsl #4`:
 *       stores &data_ov071_02123088[idx] at 16-byte stride into c+0x1e4, then
 *       tail-calls the ENTER dispatcher. MATCHED SRC and correct as it stands
 *       (src/func_ov071_02121634.c strides a 16-byte `Item16`), so it rides
 *       the slice rather than being copied here.
 *   func_ov071_021215fc(c)  -- ENTER: reads record+0, called on SetState
 *   func_ov071_021215c0(c)  -- MAIN:  reads record+8, called by MrI::Behavior
 *                              every frame
 *
 * The record array is filled by __sinit_ov071_021228c8, which copies SIX
 * 8-byte SOURCE PMFs into it. The mapping below is the sinit's OWN store
 * offsets (0x02122940..0x021229b8), not a guess:
 *
 *   record 0 (+0x00) <- data_ov071_02122cb8 (enter) , data_ov071_02122cc0 (main)
 *   record 1 (+0x10) <- data_ov071_02122ca8 (enter) , data_ov071_02122cd0 (main)
 *   record 2 (+0x20) <- data_ov071_02122cb0 (enter) , data_ov071_02122cc8 (main)
 *
 * and the six source words, read raw out of extracted/overlays/overlay_0071.bin
 * at base 0x0211f000 (= overlays.yaml's base_address 34729984 -- see the
 * base derivation in unmatched/MrI_StateMains.cpp), are:
 *
 *   02122ca8 -> 021214f4 / 0    state 1 enter   matched src
 *   02122cb0 -> 0212110c / 0    state 2 enter   matched src
 *   02122cb8 -> 02121570 / 0    state 0 enter   matched src
 *   02122cc0 -> 0212152c / 0    state 0 main    matched src
 *   02122cc8 -> 02120d30 / 0    state 2 main    HOST COPY (MrI_StateMains.cpp)
 *   02122cd0 -> 021211e0 / 0    state 1 main    HOST COPY (MrI_StateMains.cpp)
 *
 * All six .delta halves are 0 (non-virtual complete-class form, per the bytes).
 * data_ov071_02123088's mounted span is 0x30 -- three records exactly, closed
 * by the next symbol data_ov071_021230b8 -- so the array cannot silently
 * over-run. All six sources were ALREADY in port/ov071_syms.txt from the
 * gate-176 mount; this lane adds no new source symbols, only the seat.
 *
 * ---- WHY BOTH DISPATCHERS MUST BE HOST COPIES ------------------------------
 * The same two reasons as Scuttlebug's pair, verified against the matched TUs:
 *
 *   1. src/func_ov071_021215c0.cpp is
 *          struct C; typedef void (C::*PMF)();
 *          struct C { char pad[0x1e4]; PMF *pp; };
 *          void func_ov071_021215c0(C *c) { PMF *p = c->pp + 1; (c->**p)(); }
 *      `c->pp + 1` is an mwcc 8-byte PMF stride. MSVC forms a 4-byte PMF over
 *      the complete single-inheritance C, so the host `+ 1` lands on the
 *      record's +4 -- the delta=0 word -- instead of the MAIN PMF at +8, and
 *      dispatches a zero. That is a null call on the FIRST frame MrI ticks.
 *   2. src/func_ov071_021215fc.cpp reads `*c->pp`, which happens to work
 *      because delta is 0, but models the same wrong stride. Both are read
 *      here as plain { fn, 0 } halves and the fn is called with `this`.
 *
 * ---- WHY THE SEAT ----------------------------------------------------------
 * The words the sinit copies are the overlay image's own -- DS CODE ADDRESSES.
 * The seat rewrites each SOURCE PMF's fn word with its host body BEFORE
 * __sinit_ov071_021228c8 copies it into the runtime array (the MontyMole /
 * Crate / Scuttlebug order: seat the source, before the copy). Each rewrite is
 * gated on the mount holding the ROM's own word, so a mount pointing at the
 * wrong bytes aborts loudly instead of copying a DS address into a live table.
 * Invocation order is in hal/actor_overlays.cpp: this seat runs after
 * port_scuttlebug_states_seat() and before the four ov071 sinits.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

/* the six state halves. Four are matched src (this lane's slice); the two
   MAIN halves of states 1 and 2 are the host copies in MrI_StateMains.cpp. */
int func_ov071_02121570(void *c);   /* state 0 enter, matched src */
int func_ov071_0212152c(void *c);   /* state 0 main , matched src */
int func_ov071_021214f4(void *c);   /* state 1 enter, matched src */
int func_ov071_021211e0(char *c);   /* state 1 main , HOST COPY */
int func_ov071_0212110c(char *c);   /* state 2 enter, matched src */
int func_ov071_02120d30(char *c);   /* state 2 main , HOST COPY */

struct PortPmf { unsigned fn; int delta; };

/* the six SOURCE PMFs __sinit_ov071_021228c8 copies from, address order */
extern PortPmf data_ov071_02122ca8[], data_ov071_02122cb0[],
    data_ov071_02122cb8[], data_ov071_02122cc0[], data_ov071_02122cc8[],
    data_ov071_02122cd0[];

void func_ov071_021215c0(void *c);
void func_ov071_021215fc(void *c);
void port_mri_states_seat(void);

}  /* extern "C" */

/* PORT_HOST_ABI: the matched TU forms `c->pp + 1` -- an mwcc 8-byte PMF stride
   MSVC makes 4. Read the MAIN PMF at the record's +8 half directly.
   ROM 0x021215c0, 0x3c bytes. */
extern "C" void func_ov071_021215c0(void *c)
{
    PortPmf *rec = *(PortPmf **)((char *)c + 0x1e4);
    ((void (*)(void *))(size_t)rec[1].fn)(c);
}

/* PORT_HOST_ABI: the matched TU forms `(c->**c->pp)()` over the ENTER PMF at
   the record's +0; here the record is read as a plain { fn, 0 } and the fn
   called with `this`. ROM 0x021215fc, 0x38 bytes. */
extern "C" void func_ov071_021215fc(void *c)
{
    PortPmf *rec = *(PortPmf **)((char *)c + 0x1e4);
    ((void (*)(void *))(size_t)rec[0].fn)(c);
}

/* Each SOURCE PMF, in the mount's address order, with the ROM fn it holds and
   the host body that replaces it. */
static const struct { PortPmf *slot; unsigned rom; int (*host)(void *); }
g_mri_sources[] = {
    {data_ov071_02122ca8, 0x021214f4, (int (*)(void *))func_ov071_021214f4}, /* s1 enter */
    {data_ov071_02122cb0, 0x0212110c, (int (*)(void *))func_ov071_0212110c}, /* s2 enter */
    {data_ov071_02122cb8, 0x02121570, (int (*)(void *))func_ov071_02121570}, /* s0 enter */
    {data_ov071_02122cc0, 0x0212152c, (int (*)(void *))func_ov071_0212152c}, /* s0 main  */
    {data_ov071_02122cc8, 0x02120d30, (int (*)(void *))func_ov071_02120d30}, /* s2 main  */
    {data_ov071_02122cd0, 0x021211e0, (int (*)(void *))func_ov071_021211e0}, /* s1 main  */
};

extern "C" void port_mri_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_mri_sources / sizeof g_mri_sources[0];
         ++i) {
        PortPmf *p = g_mri_sources[i].slot;
        if (p->fn != g_mri_sources[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: MrI source %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_mri_sources[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_mri_sources[i].host;
    }
}
