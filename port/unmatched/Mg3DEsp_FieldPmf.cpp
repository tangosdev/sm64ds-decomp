// PORT_HOST_ABI.  The mwcc POINTER-TO-MEMBER WALL, FIELD half:
// func_ov006_020e7be8, the per-frame tick of the sub-object dScMg3DEsp_c holds
// at this+0x4fd8.  Run mg9, lane PSY.
//
// THIS IS THE EIGHTH DISPATCHER IN A CLASS THE COST FILE PREDICTS FIVE FOR,
// and it is the one neither the link nor the class's own derivation would have
// produced.  The seven in unmatched/Mg3DEsp_StateDispatch.cpp all come out of
// the class's overlay constructor: read the pair assignments, find the tables,
// find the TUs whose literal pools load them.  This one dispatches a member
// pointer held in an OBJECT FIELD, so there is no table, no global, and nothing
// in __sinit_ov006_02130a08 names it.  It was found by the `::*` SWEEP OVER THE
// CLOSURE -- port/mg_fanout_costs.txt section 4's second prescribed detector,
// run over the wave-2 slice additions before they were wired rather than after.
//
// ---- 1. THE SHAPE, OUT OF THE ROM ----------------------------------------
//
// extracted/overlays/overlay_0006.bin at base 0x020bfec0, 0xd8:
//
//     020e7bf4  ldr r2,[pc,#0xbc]      = 0x0213c704, the IDLE SENTINEL
//     020e7bf8  add r3,r4,#0x210       the member-pointer FIELD
//     020e7bfc  ldr r1,[r3]  / ldr r0,[r2]  / cmp r1,r0 / bne
//     020e7c0c  ldr r1,[r3,#4] / ldr r0,[r2,#4] / cmp r1,r0 / RETURN if equal
//     020e7c24  ldr r0,[r4,#0x210] / cmp r0,#0 / RETURN if zero
//     020e7c38  add r3,r4,#0x210
//     020e7c3c  ldr r1,[r3,#4]         the adjustment
//     020e7c40  add r0,r4,r1,asr #1    this, advanced
//     020e7c44  ands r1,r1,#1          the virtual bit
//     020e7c48  ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1]
//     020e7c54  ldreq r1,[r3]
//     020e7c58  blx r1                 NO r1 argument -> ARITY 0
//
// then Animation::Advance(this+0x5c), func_ov006_020e7818(this+0x84), and a
// Particle::System::New guarded on this+0x208.  src/func_ov006_020e7be8.cpp
// reproduces every line of that and spells the dispatch
//
//     (((C*)c)->**(PMF*)(c + 0x210))();
//
// which is FOUR bytes on MSVC where the ROM's field is eight.  It compiles, it
// links -- the type is local to the TU, so no global is emitted and no symbol
// can fail -- and the call goes to whatever the low word holds, which is a raw
// DS code address.
//
// ---- 2. THE STORED PAIR MUST NOT BE REWRITTEN, AND THIS CLASS PROVES IT ---
//
// run mg5 lane FLW ruled that a field pair is routed AT THE DISPATCH SITE and
// never by writing host addresses over the stored words, because a second
// reader compares the pair against an idle sentinel BY VALUE and a host address
// never equals a DS one.  On that class the second reader was one TU.  Here
// there are SIX, and five of them are writers:
//
//   func_ov006_020e7d7c   *(struct G2*)(c+0x210) = data_ov006_0213c754
//   func_ov006_020e7e74   *(struct G2*)(c+0x210) = data_ov006_0213c764
//   func_ov006_020e7fb0   *(struct M2*)(c+0x210) = data_ov006_0213c774
//   func_ov006_020e7f5c   *(double*)(c+0x210)    = data_ov006_0213c76c
//   func_ov006_020e7b44   compares c+0x210 against data_ov006_0213c744 and
//                         returns early on a match -- the RENDER half of the
//                         same idle test, and it does not dispatch
//   func_ov006_020e7be8   this file: compares against data_ov006_0213c704,
//                         then dispatches
//
// EVERY ONE OF THE FOUR WRITERS MOVES EIGHT BYTES.  Three spell the pair as a
// two-int struct and the fourth spells it as a `double`, and eight bytes is
// eight bytes on both machines, so the stores are already correct on the host
// and nothing about them needs repairing.  Only the DECODE is wrong.  That is
// the same finding section 4 records for the tables -- "the mount's tables have
// been correct at the right stride all along and only the CONSUMERS were
// wrong" -- reaching a field.
//
// ---- 3. THE FIVE SOURCE PAIRS ARE THE SIX FALSE PAIRS FROM THE SWEEP TRAP -
//
// port/unmatched/Mg3DEsp_StateDispatch.cpp section 1 lists six eight-byte
// records inside the class's pair-symbol span that read {code, 0} and that
// __sinit_ov006_02130a08 does not copy, and warns that a SWEEP of the range
// would take them for state pairs.  They are these.  The correction that
// belongs beside that warning is that they are not junk: they are THIS field's
// states, written one at a time by the four writers above and compared by the
// two readers, and the reason the constructor does not copy them is that they
// go into an object rather than into a .bss table.
//
//     0x0213c704  {0x020e7fac, 0}   the idle sentinel this file reads
//     0x0213c744  {0x020e7fac, 0}   the idle sentinel func_ov006_020e7b44 reads
//     0x0213c754  {0x020e7cc0, 0}   written by func_ov006_020e7d7c
//     0x0213c764  {0x020e7de8, 0}   written by func_ov006_020e7e74
//     0x0213c76c  {0x020e7f04, 0}   written by func_ov006_020e7f5c
//     0x0213c774  {0x020e7fac, 0}   written by func_ov006_020e7fb0
//
// FOUR DISTINCT CODE WORDS, every adjustment zero, and all four have a src TU.
// The seventh record in that span, 0x0213c75c, is the ASCII string "ki_mizu"
// and is not a pair at all.
//
// ---- 4. WHY THE IDLE TEST STILL WORKS -----------------------------------
//
// Nothing here writes the field, so the sentinel comparison keeps comparing DS
// words with DS words and answers what the ROM answers.  The one behaviour that
// differs is the null guard: the ROM's `ldr r0,[r4,#0x210] / cmp r0,#0` on a
// still-zero field returns, and so does this copy, because the guard is kept
// above the routed call rather than delegated.  port_mg_call0's own
// zero-code refusal is a second net under it, not a replacement for it.

#include <cstdio>
#include <cstdlib>

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);

/* the four routed field states, each with the parameter list its own src TU
   defines.  func_ov006_020e7fac takes (void) and is the idle body -- there is
   nothing for an ignored receiver to be wrong about, which is the
   MgCoin_StateDispatch.cpp ruling for the same shape. */
void func_ov006_020e7cc0(char *thiz);
void func_ov006_020e7de8(char *c);
void func_ov006_020e7f04(char *c);
void func_ov006_020e7fac(void);

/* the ordinary callees the host copy keeps, spelled as their own src TUs
   spell them */
int   _ZN9Animation7AdvanceEv(void *anim);
void  func_ov006_020e7818(void *p);
void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        unsigned int a, unsigned int b, int c, int d, int e,
        const void *f, void *g);

/* the idle sentinel, as two ints -- the spelling that is true on both
   machines, and the one src already uses for the comparison */
extern int data_ov006_0213c704[2];

}  /* extern "C" */

static unsigned g_esp_field_calls, g_esp_field_routed;


/* ---- RUN link100 LANE PMFB7 GATE 3: THE SIX RECORDS ARE SEATED ------------
 *
 * SECTION 4 ABOVE IS SUPERSEDED, and the measurement that supersedes it is the
 * point. It kept the stored pairs because "nothing here writes the field, so
 * the sentinel comparison keeps comparing DS words with DS words" -- true of a
 * face per RECORD and false of a face per CODE WORD, which is lane FWD's
 * gate-3 rule. 0x0213c704 (this file's sentinel), 0x0213c744
 * (func_ov006_020e7b44's) and 0x0213c774 (the installed state) all hold the
 * SAME code word 0x020e7fac. Seat all three with the same face and each
 * predicate compares one host address against itself: it answers what it
 * answered before, on every path. src/func_ov006_020e7b44.cpp is in the link
 * (port/slice_fold2.txt) and reads data_ov006_0213c744 by value, so this is not
 * hypothetical.
 *
 * THE UNIVERSE, over a block derived from the class. dScMg3DEsp_c's own vtable
 * (0x0213c8c4) names its bodies; the contiguous cluster of them runs
 * 0x020e7660..0x020ea1f0 (its D2 at 0x020e7660 through InitResources
 * 0x020e9e70 + 0x380), which is the span one translation unit emits. The sweep
 * over that block returns 28 {code,0} pairs -- MORE than this field's universe,
 * because the block also holds the source statics of the seven .bss STATE
 * TABLES the class's own __sinit copies. Every reference into the run says
 * which is which, and the separation is total: SIX are loaded by the class's
 * OWN CODE and they are exactly this field's records, while the other 22 are
 * loaded by __sinit_ov006_02130a08 alone.
 *
 *   0213c704 {020e7fac,0}  read by func_ov006_020e7be8    BY-VALUE SENTINEL
 *   0213c744 {020e7fac,0}  read by func_ov006_020e7b44    BY-VALUE SENTINEL
 *   0213c754 {020e7cc0,0}  written by func_ov006_020e7d7c
 *   0213c764 {020e7de8,0}  written by func_ov006_020e7e74
 *   0213c76c {020e7f04,0}  written by func_ov006_020e7f5c
 *   0213c774 {020e7fac,0}  written by func_ov006_020e7fb0
 *
 * FOUR distinct code words, four faces, six records.
 * (runs/link100/out/PMFB7/sweep_3desp.txt)
 *
 * THE EMITTED SIDE: mov edx,[esi+528] / test edx,edx / je / mov ecx,[esi+532] /
 * add ecx,esi / call edx -- 0x210 and 0x214, the ROM's own offsets, the ROM's
 * own null guard, ARITY ZERO. /Zp4 changes 0 listing lines outside the TITLE
 * and is NOT claimed: 0x210 mod 8 is 0, which is PMFB6's rule.
 *
 * THE FOUR WRITERS DO NOT CHANGE. Section 2 measured that every one of them
 * moves eight bytes -- three as a two-int struct and one as a `double` -- so
 * all four are already correct on the host. Only the decode was wrong.
 *
 * WHAT THE CENSUS CAN STILL SEE: calls and routed are bumped by every face and
 * stay exact. The framework-routed arm is STRUCTURALLY ZERO after the seat,
 * because the field can only hold one of the four faces.
 */
#define ESP_FACE(tag, call)                                               \
    static void __fastcall esp_f##tag(void *self, void *dead_edx)         \
    {                                                                     \
        (void)dead_edx;                                                   \
        ++g_esp_field_calls;                                              \
        ++g_esp_field_routed;                                             \
        call;                                                             \
    }

ESP_FACE(020e7cc0, func_ov006_020e7cc0((char *)self))
ESP_FACE(020e7de8, func_ov006_020e7de8((char *)self))
ESP_FACE(020e7f04, func_ov006_020e7f04((char *)self))
ESP_FACE(020e7fac, func_ov006_020e7fac())

extern "C" {
extern MgPmf data_ov006_0213c744, data_ov006_0213c754, data_ov006_0213c764,
    data_ov006_0213c76c, data_ov006_0213c774;
}

namespace {
struct EspSeat { MgPmf *rec; unsigned rom; void *face; const char *what; };
const EspSeat g_esp_seats[] = {
    {(MgPmf *)data_ov006_0213c704, 0x020e7facu, (void *)esp_f020e7fac,
     "0213c704 the sentinel this file reads"},
    {&data_ov006_0213c744, 0x020e7facu, (void *)esp_f020e7fac,
     "0213c744 the sentinel func_ov006_020e7b44 reads"},
    {&data_ov006_0213c754, 0x020e7cc0u, (void *)esp_f020e7cc0, "0213c754"},
    {&data_ov006_0213c764, 0x020e7de8u, (void *)esp_f020e7de8, "0213c764"},
    {&data_ov006_0213c76c, 0x020e7f04u, (void *)esp_f020e7f04, "0213c76c"},
    {&data_ov006_0213c774, 0x020e7facu, (void *)esp_f020e7fac, "0213c774"},
};
}  /* namespace */

extern "C" void port_mg_esp3d_field_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_esp_seats / sizeof g_esp_seats[0]; ++i) {
        MgPmf *p = g_esp_seats[i].rec;
        if (p->code != g_esp_seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMg3DEsp_c pair %s: the mount holds "
                         "%08x/%d, the cartridge's own record says %08x/0 -- "
                         "WRONG BYTES\n", g_esp_seats[i].what, p->code, p->adj,
                         g_esp_seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)g_esp_seats[i].face;
        p->adj = 0;
    }
}

/* THE CALL-TIME SWITCH IS DEAD AND SAYS SO -- the abort below is what a route
   by DS code word would hit, and reaching it means the seat did not run. */
static int esp_field_try(void *self, unsigned code)
{
    std::fprintf(stderr, "FATAL: dScMg3DEsp_c field dispatch reached the call-time "
                 "switch with code %08x -- the seat did not run, or the pair "
                 "universe is not the one the sweep closed\n", code);
    (void)self;
    std::abort();
    return 0;
}
/* Everything this switch does not own goes to the framework unchanged, so the
   null-code guard, the nonzero-adjustment refusal and the UNHANDLED report all
   still live in exactly one place. */
static void esp_field_call(void *self, unsigned code, int adj)
{
    ++g_esp_field_calls;
    if (code != 0 && adj == 0 && esp_field_try(self, code)) {
        ++g_esp_field_routed;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_esp3d_field_counts(unsigned *calls, unsigned *routed)
{
    if (calls)  *calls  = g_esp_field_calls;
    if (routed) *routed = g_esp_field_routed;
}

/* src/func_ov006_020e7be8.cpp verbatim except that the member-pointer type is
   gone and the dispatch site is a routed call.  The two early returns, the
   Animation::Advance, the func_ov006_020e7818 and the guarded
   Particle::System::New below it are src's, unchanged, and every one is
   confirmed against the disassembly in section 1. */
/* HOST COPY RETIRED, run link100 lane PMFB7 gate 3.
   src/func_ov006_020e7be8.cpp dispatches its own field now: with /vmg /vmm
   (block R8) MSVC's pointer to member IS the ROM's eight-byte {code, adjust}
   pair, so the widening the banner above was written for does not happen, and
   the six records hold four faces -- one per code word, which is what keeps the
   two by-value sentinels exact. */
