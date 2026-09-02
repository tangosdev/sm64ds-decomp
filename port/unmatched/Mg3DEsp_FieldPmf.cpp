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

static int esp_field_try(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x020e7cc0u: func_ov006_020e7cc0(c); return 1;
    case 0x020e7de8u: func_ov006_020e7de8(c); return 1;
    case 0x020e7f04u: func_ov006_020e7f04(c); return 1;
    case 0x020e7facu: func_ov006_020e7fac();  return 1;  /* the idle body */
    default:                                  return 0;
    }
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
// PORT_HOST_ABI: mwcc field pointer-to-member dispatch on the 8-byte pair at +0x210 MSVC's 4-byte pmf cannot reproduce
extern "C" void func_ov006_020e7be8(char *c)
{
    const MgPmf *p = (const MgPmf *)(c + 0x210);
    const int   *d = data_ov006_0213c704;

    if ((int)p->code == d[0]) {
        if (p->adj == d[1]) return;
        if (*(int *)(c + 0x210) == 0) return;
    }

    esp_field_call(c + (p->adj >> 1), p->code, p->adj);

    _ZN9Animation7AdvanceEv(c + 0x5c);
    func_ov006_020e7818(c + 0x84);

    if (*(int *)(c + 0x208) == 0) return;

    *(void **)(c + 0x204) =
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned int *)(c + 0x204), 0xec, 0x48c000, 0x140000, 0x200000,
            0, 0);
}
