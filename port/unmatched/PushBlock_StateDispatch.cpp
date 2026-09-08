/* HOST COPIES of src/func_ov002_020b9704.cpp (the state-cell installer) and
 * src/func_ov002_020b9750.cpp (the per-frame tick dispatcher) --
 * PUSH_BLOCK's (306, the "PowerFlower" id-decoy -- see the registry row for
 * the derivation) own 3-state machine. The Unagi/MrBlizzard/BabyPenguin/
 * HootTheOwl shape: a REAL C++ pointer-to-member dispatched through a
 * DELIBERATELY INCOMPLETE class.
 *
 * func_ov002_020b9704(c, i) forms:
 *     struct C; typedef void (C::*PMF)();
 *     struct C { char pad[0x3c0]; int idx; };
 *     c->idx = i;
 *     (c->*data_ov002_021097bc[i].pmf)();
 * -- store the state index and dispatch entry i's FIRST word (the "enter"
 * half) immediately. func_ov002_020b9750(c) reads the CURRENT idx back and
 * dispatches entry idx's SECOND word (the "tick" half) every frame.
 *
 * THE TABLE SHAPE, resolved from __sinit_ov002_021014e4's OWN source (that
 * sinit's comments already carry the byte offsets, byte-for-byte matched
 * against mwccarm -- not inferred): data_ov002_021097bc is three 20-byte
 * entries, each { Pair enter; Pair tick; int gap; } where Pair{int fn,delta}
 * is the standard 8-byte mwcc PMF pair -- so enter/tick live at +0/+8, +20/
 * +28, +40/+48. The gap words (+16/+36/+56/+64) belong to a DIFFERENT
 * symbol dsd merged under the same name (unrelated string-table pointers,
 * host-seated by the ov002 mount's own syms patch) and are not touched
 * here. NOTE: both dsd auto-recovered local struct declarations
 * ("Entry{pmf[2];extra}" and "Entry{pmf;extra[3]}") are imprecise; the
 * 20-byte stride above is the one the sinit's byte offsets actually walk.
 *
 * The sinit copies six SOURCE pairs (data_ov002_02109758/68/50/48/40/60,
 * each a real {fn,0} pair, delta always 0 per the ROM's own bytes) into the
 * three destination entries; every source fn word reloc-confirmed against a
 * real func_ov002_020b9* body (config/arm9/overlays/ov002/relocs.txt):
 *   entry 0 (idx 0): enter=020b96c0, tick=020b94c4
 *   entry 1 (idx 1): enter=020b9450, tick=020b92c4
 *   entry 2 (idx 2): enter=020b92b8, tick=020b91fc
 * All six are plain matched src (slice_gate200.txt), none PMF, none
 * previously linked before this gate.
 *
 * __sinit_ov002_021014e4 and __sinit_ov002_021071f4 (a SECOND, unrelated
 * sinit that ALSO targets a symbol dsd calls data_ov002_021097bc, from
 * entirely different source addresses) are BOTH ALREADY in slice_gate10.txt,
 * already linked, already stable -- but the ov002 mount emits the six SOURCE
 * pairs as RAW BYTES (u8 arrays holding DS fn words, verified in the
 * generated host-src/ov002_data.c), so after the sinit copy the dest entries
 * hold DS CODE ADDRESSES. THE SEAT below rewrites the six DEST fn words with
 * the host bodies, the ov002 house order (sinit first, seat the dest at
 * vtable-fill time -- the gate-51 WaterfallMist/SignPost treatment), with
 * the Unagi WRONG-BYTES guard on every word it overwrites.
 *
 * MSVC's PMF representation for an INCOMPLETE class does not reproduce the
 * ROM's own {function,delta} dispatch -- both sites are host copies,
 * translated field for field with the PMF call replaced by a plain
 * function-pointer call through the same struct layout.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
struct PortPushBlockPair { unsigned fn; int delta; };
struct PortPushBlockEntry { PortPushBlockPair enter, tick; int gap; };
extern PortPushBlockEntry data_ov002_021097bc[3];

/* the six matched state bodies (slice_gate200.txt) */
void func_ov002_020b96c0(void *c);   /* entry 0 enter */
void func_ov002_020b94c4(void *c);   /* entry 0 tick  */
void func_ov002_020b9450(void *c);   /* entry 1 enter */
void func_ov002_020b92c4(void *c);   /* entry 1 tick  */
void func_ov002_020b92b8(void *c);   /* entry 2 enter */
void func_ov002_020b91fc(void *c);   /* entry 2 tick  */

/* Seat the six dest fn words the sinit filled with DS addresses. Called from
   hal_fill_pushblock_vtable (registration runs after the ov002 sinit chain).
   Any word that does not hold exactly the ROM address the reloc table
   promises is a loud abort, not a silent wrong dispatch. */
void port_pushblock_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    static const struct { unsigned entry, half, rom; void (*host)(void *); }
    seats[] = {
        {0, 0, 0x020b96c0, func_ov002_020b96c0},
        {0, 1, 0x020b94c4, func_ov002_020b94c4},
        {1, 0, 0x020b9450, func_ov002_020b9450},
        {1, 1, 0x020b92c4, func_ov002_020b92c4},
        {2, 0, 0x020b92b8, func_ov002_020b92b8},
        {2, 1, 0x020b91fc, func_ov002_020b91fc},
    };
    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        PortPushBlockPair *p = seats[i].half
            ? &data_ov002_021097bc[seats[i].entry].tick
            : &data_ov002_021097bc[seats[i].entry].enter;
        if (p->fn != seats[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: PushBlock state %u/%u: the sinit "
                         "left %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", seats[i].entry, seats[i].half,
                         p->fn, p->delta, seats[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)seats[i].host;
    }
}

/* func_ov002_020b9704 and func_ov002_020b9750 RETIRED (run link100, lane
   PMFB1). src/func_ov002_020b9704.cpp and src/func_ov002_020b9750.cpp carry
   both on port/slice_pmfc.txt. The header's note that "MSVC's PMF
   representation for an INCOMPLETE class does not reproduce the ROM's own
   {function,delta} dispatch" is dead: /vmg /vmm target-wide (block R8) makes
   it exactly that pair. What was still wrong was the twenty-byte RECORD the
   header derives above -- MSVC gives a struct containing a pointer-to-member
   eight-byte alignment, so PortPushBlockEntry's C++ twin came out twenty-four
   and the matched TUs strode 24 where the ROM strides 0x14. A per-TU /Zp4
   (block R9d in port/CMakeLists.txt) makes it twenty. port_pushblock_states_seat
   above STAYS, still asserting {the ROM's own address, 0} on all six dest
   words before it writes a host one. */
}  /* extern "C" */
