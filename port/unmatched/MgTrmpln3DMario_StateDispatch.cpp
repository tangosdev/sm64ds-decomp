// PORT_HOST_ABI.  The mwcc POINTER-TO-MEMBER WALL, SECOND LEVEL:
// dMgTrmpln3DMario_c's dispatcher and its twenty-one state bodies.
// Run mg11, lane TTI.
//
// Read port/unmatched/MgTrampolineTime_StateDispatch.cpp first -- it is the
// OUTER class's half of the same wall, and the framework entry point both files
// chain to is documented in unmatched/MgBase_StateDispatch.cpp.
//
// ---- 1. WHY A SECOND DISPATCHER EXISTS AT ALL ----------------------------
//
// dScMgTrampoline_c's factory builds FOUR records of 0xd0 bytes at this+0x500c
// whose element vtable is data_ov006_0213b2c4 (port/slice_tti.txt section 8),
// and the ROM's type_info at 0x0213b2c0 names that class dMgTrmpln3DMario_c --
// the four bouncing characters.  THOSE RECORDS HAVE A STATE MACHINE OF THEIR
// OWN, of exactly the same shape as their owner's: a FIELD-HELD member pointer,
// here at element+0x64 (code) and element+0x68 (adjustment), dispatched from one
// place.
//
// So this class is TWO LEVELS DEEP, which is the question
// port/mg_fanout_costs.txt section 14 tells a lane to ask and the answer here
// is YES.  The outer machine is five links; this one is twenty-one bodies over
// THIRTY-EIGHT pair slots.  (0x0213b224 - 0x0213b0f4) / 8 + 1 = 39 slots in
// the run, less the one non-pair inside it (0x0213b19c) = 38.  Three committed
// places said thirty-seven; section 4 records why the lane's own verifier did
// not catch it.
//
// ---- 2. THE ONE DECODE SITE, FOUND BY THE IMAGE --------------------------
//
// The five-instruction ARM Itanium encoding scan (the detector lane LKY
// invented, blind to spelling) over the element machinery 0x020caf00..
// 0x020cd800, body by body at each body's own start and size:
//
//     func_ov006_020cb030   ONE site, at 0x020cb068
//
// THE ADDRESS A DECODE SITE IS REPORTED AT IS THE `add`, the FIRST instruction
// of the five-instruction sequence, in every place this lane names one.  The
// detector matches on the add/ands pair, so the add is what it has; the `blx`
// is three or five instructions later and is a different number for the same
// site.  Both sequences in full:
//
//     _ZN17dScMgTrampoline_c8BehaviorEv   add 0x0212151c   ands 0x02121520   blx 0x02121534
//     func_ov006_020cb030   add 0x020cb068   ands 0x020cb06c   blx 0x020cb080
//
// An earlier version of the element file reported ITS site at the blx while
// this class's was reported at the add, so two numbers that look like the same
// kind of thing were pointing at different instructions.
//
// and nothing else in that range.  Every other `blx` in it is an ordinary
// vtable dispatch (`ldr r1,[r0] / ldr r1,[r1,#imm] / blx r1`), which is a
// different shape and is not this wall.
//
// ---- 3. THE src IS ALREADY OPEN-CODED, AND THAT IS THE DANGEROUS PART ----
//
// src/func_ov006_020cb030.cpp does NOT use a member-pointer type.  It spells
// the pair by hand:
//
//     struct Closure { int off; int adj; };
//     Closure* cl = (Closure*)(o + 0x64);
//     void* tobj = o + (cl->adj >> 1);
//     if (cl->adj & 1) fn = *(void(**)())(*(char**)tobj + cl->off);
//     else             fn = (void(*)())cl->off;
//     ((void(*)(void*))fn)(tobj);
//
// which is a FAITHFUL reading of the ROM and the third of the three shapes
// port/mg_fanout_costs.txt section 4 names -- the one both prescribed detectors
// (a link, a `::*` sweep) are blind to, because there is no member pointer
// anywhere for either to find.  The stride is right, the virtual bit is right,
// the adjustment is right.  What is wrong on the host is the last line: `off`
// holds a DS CODE ADDRESS, so the call jumps into nothing.  It would link
// clean, compile clean, boot, and jump to 0x020cXXXX the first frame an element
// had a state.
//
// THE PORT CANNOT COMPILE THAT src ANYWAY, which is how this file came to be
// written before the run rather than after it:
//
//     src\func_ov006_020cb030.cpp(14): error C2761: 'System *System::New(...)':
//     redeclaration of member is not allowed
//     src\func_ov006_020cb030.cpp(15): error C2761: 'void Animation::Advance()'
//
// The TU declares two class members inside the class and then RE-DECLARES them
// at namespace scope.  mwcc accepted it; MSVC does not, and it is right.  So
// src/func_ov006_020cb030.cpp is out of port/slice_tti.txt and the body below
// takes its symbol -- the unmatched/MgBSC_StateDispatch.cpp convention for "the
// port cannot compile the src".
//
// ---- 4. THE TABLE, READ OFF THE READING POOLS AND NOT OFF A SWEEP --------
//
// There is no overlay constructor: ov006's .init (0x0212f4c4..0x0213356c) has
// NO relocation into this class's data, the same as the outer class.  The pairs
// are statically relocated .data at an eight-byte stride from 0x0213b0f4 to
// 0x0213b224, and each one is identified by the literal pool that LOADS it --
// THIRTY-EIGHT slots, TWENTY-ONE distinct code words, every adjustment zero.
//
// THE COUNT WAS WRONG IN THREE COMMITTED PLACES AND THE LANE'S OWN VERIFIER
// COULD NOT HAVE CAUGHT IT: tmp/tti/verify.py checked that the distinct code
// words number 21 and that every adjustment is zero, and it never counted the
// slots.  A self-audit catches only what it measures.  The slot-count check
// is in verify.py now.
//
// A SWEEP OF THE SPAN WOULD HAVE COLLECTED THREE WORDS THAT ARE NOT PAIRS,
// which is the curling trap for the third time in this lane:
//
//     0x0213b0ec  {3, 3}          two ints. Read by func_ov006_020cc2ac,
//                                 func_ov006_020cd424, and by the OUTER class's
//                                 slot 9 and chain link 1.
//     0x0213b19c  {0209a774, 0213b288}   the type_info of dMgTrmpObjAdapter_c,
//                                 this element class's own base.
//     0x0213b22c  {02140590, 02140560}   two .bss ADDRESSES, not code.
//
// ALL TWENTY-ONE CODE WORDS REACH A MATCHED src TU, so this machine has NO
// state floor, and the switch below is complete rather than partial.  The
// bodiless counter is kept for the reason every other one in the tree is kept.
//
// ---- 5. THE ARITY IS ZERO AND THE RECEIVER IS THE ADJUSTED POINTER -------
//
// The ROM's dispatch is `blx r1` with r0 = `o + (adj >> 1)` and nothing written
// to r1..r3 between the decode and the call, so every state is called with the
// element pointer alone.  Every stored adjustment is zero, so in practice the
// receiver IS the element, and the shift is implemented anyway rather than
// folded away: an adjustment this lane did not find would otherwise be silently
// ignored instead of reported.
//
// ---- 5b. THE src CALLS THE WRONG TWIN, AND THE HOST COPY DOES NOT -------
//
// src/func_ov006_020cb030.cpp's second-to-last statement is
//
//     func_ov006_020c9024(o);
//
// and the ROM's is not.  config/arm9/overlays/ov006/relocs.txt has exactly one
// arm_call out of this body into that neighbourhood --
// `from:0x020cb088 kind:arm_call to:0x020cb134` -- and 0x020c9024 appears
// nowhere in the body's relocations at all.
//
// THE TWO ARE NOT THE SAME FUNCTION, which is what makes it worth writing down
// rather than filing as a typo.  Both are 0x38 bytes and both are
// ApproachLinear-then-AddVec3, and their bytes DIFFER:
//
//     func_ov006_020c9024   ApproachLinear(o+0x40, data_ov006_021405b4[0],
//                           *(o+0x48));  AddVec3(o+0x24, o+0x3c, o+0x24)
//     func_ov006_020cb134   ApproachLinear(o+0x38, data_ov006_02140548[0],
//                           *(o+0x40));  AddVec3(o+0x1c, o+0x34, o+0x1c)
//
// -- different offsets, different global.  The element object this dispatcher
// runs on is the one whose +0x1c/+0x20/+0x24 the first three statements of this
// very body copy to +0x28/+0x2c/+0x30, and whose +0x38 it compares three
// statements later.  Those are 0x020cb134's offsets, not 0x020c9024's.  A port
// that compiled the src would have integrated the wrong vector every frame, on
// an object whose fields the wrong body reads eight bytes further along.
//
// This is a DECOMP-SIDE correction to route, in the same family as the
// receiver-drop findings run mg9 lane BOX banked: the body is otherwise
// faithful, and the byte gate cannot see a wrong callee because the two
// candidates are the same size and the relocation is what distinguishes them.
//
// ---- 6. IT IS SHARED WITH THE SIBLING, AND THE FILE SAYS SO -------------
//
// data_ov006_0213b0ec and data_ov006_0213b0f0 are read by dScMgTrampoline2_c's
// bodies too (0x02123330 and 0x02124014), so the element machinery is common to
// the two trampoline classes even though the ELEMENT ARRAY is not -- only this
// class's factory calls func_ov006_020cd12c, the element constructor that
// installs data_ov006_0213b2c4.  Lane TTE must CALL this file rather than write
// a second: a second definition of func_ov006_020cb030 is an LNK2005 and two
// switches over the same twenty-one addresses would split the witness.

#include <cstdio>
#include <cstdlib>   /* std::abort, for the field seat (run linkfull lane PMFMG1) */

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);

/* ---- the twenty-one routed state bodies, in address order ---------------
   Each is declared with the parameter list ITS OWN src TU defines. */
void func_ov006_020c905c(char *o);
void func_ov006_020c91ac(char *c);
void func_ov006_020c97bc(char *c);
void func_ov006_020c9d7c(char *c);
void func_ov006_020c9efc(char *c);
void func_ov006_020ca070(char *c);
void func_ov006_020ca310(char *c);
void func_ov006_020ca39c(int *p);
void func_ov006_020cb16c(int *c);
void func_ov006_020cb2b4(void *self);
void func_ov006_020cb5c4(char *c);
void func_ov006_020cb72c(char *c);
void func_ov006_020cb838(char *c);
void func_ov006_020cbd7c(char *c);
void func_ov006_020cc2ac(char *c);
void func_ov006_020cc408(char *c);
void func_ov006_020cc63c(int *c);
void func_ov006_020cc724(char *c);
void func_ov006_020cc9b8(int *c);
void func_ov006_020ccae0(char *c);
void func_ov006_020ccd64(char *p);

/* the ordinary callees the host copy keeps, spelled as their own src TUs do */
void func_ov006_020cb134(char *o);
void func_ov006_020cafdc(char *o);
extern int data_ov006_0214059c;
int  _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        int a, unsigned b, int x, int y, int z, const void *v, void *cb);
void _ZN9Animation7AdvanceEv(void *self);

}  /* extern "C" */

// ---- the element class's address switch ------------------------------------

static unsigned g_mario_state_hits;
static unsigned g_mario_floor_hits;   /* structurally zero -- section 4 */
static unsigned g_mario_unrouted;
static unsigned g_mario_virtual_arm;

static int mario_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x020c905cu: func_ov006_020c905c(c);            return 1;
    case 0x020c91acu: func_ov006_020c91ac(c);            return 1;
    case 0x020c97bcu: func_ov006_020c97bc(c);            return 1;
    case 0x020c9d7cu: func_ov006_020c9d7c(c);            return 1;
    case 0x020c9efcu: func_ov006_020c9efc(c);            return 1;
    case 0x020ca070u: func_ov006_020ca070(c);            return 1;
    case 0x020ca310u: func_ov006_020ca310(c);            return 1;
    case 0x020ca39cu: func_ov006_020ca39c((int *)c);     return 1;
    case 0x020cb16cu: func_ov006_020cb16c((int *)c);     return 1;
    case 0x020cb2b4u: func_ov006_020cb2b4(c);            return 1;
    case 0x020cb5c4u: func_ov006_020cb5c4(c);            return 1;
    case 0x020cb72cu: func_ov006_020cb72c(c);            return 1;
    case 0x020cb838u: func_ov006_020cb838(c);            return 1;
    case 0x020cbd7cu: func_ov006_020cbd7c(c);            return 1;
    case 0x020cc2acu: func_ov006_020cc2ac(c);            return 1;
    case 0x020cc408u: func_ov006_020cc408(c);            return 1;
    case 0x020cc63cu: func_ov006_020cc63c((int *)c);     return 1;
    case 0x020cc724u: func_ov006_020cc724(c);            return 1;
    case 0x020cc9b8u: func_ov006_020cc9b8((int *)c);     return 1;
    case 0x020ccae0u: func_ov006_020ccae0(c);            return 1;
    case 0x020ccd64u: func_ov006_020ccd64(c);            return 1;
    default:                                             return 0;
    }
}

extern "C" void port_mg_mario_call0(void *self, unsigned code, int adj)
{
    if (adj != 0) ++g_mario_virtual_arm;
    if (code != 0 && adj == 0 && mario_try_0(self, code)) {
        ++g_mario_state_hits;
        return;
    }
    if (code != 0) ++g_mario_unrouted;
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_mario_counts(unsigned *hits, unsigned *floor,
                                     unsigned *unrouted, unsigned *virt)
{
    *hits     = g_mario_state_hits;
    *floor    = g_mario_floor_hits;
    *unrouted = g_mario_unrouted;
    *virt     = g_mario_virtual_arm;
}

// ---- THE FIELD IS SEATED AND BOTH HOST COPIES ARE GONE, run linkfull lane PMFMG1
//
// src/func_ov006_020cb030.cpp (this class's element dispatcher) and
// src/func_ov006_020c8f20.cpp (dScMgTrmpln2Mario_c's, Trampoline Terror's
// element: unmatched/MgTrampolineTerror_SubDispatch.cpp) both run from src now.
// They read ONE pair universe, which is why the seat is here and serves both,
// as section 6 above asks.
//
// ---- 1. THE UNIVERSE, CLOSED BY THE ROM -----------------------------------
//
// runs/linkfull/out/PMFMG1/rom_records.txt reads all THIRTY-EIGHT slots of
// 0x0213b0f4..0x0213b224 (the run section 4 bounds, 0x0213b19c excluded) out
// of overlay_0006.bin with their relocations: every one {load-relocated code
// word, 0}, nothing relocated from +4, TWENTY-ONE distinct code words. The
// address-targeted sweep over the whole overlay for those twenty-one
// (rom_records_sweep.txt) finds exactly these thirty-eight pairs and no other;
// rom_trmpln_code_refs.txt lists every other relocation that reaches one of the
// twenty-one: eight arm_call rows (plain calls of a body) and three literal
// pool words, each a tail call at the end of a two-line writer
// (020ca2ec, 020cb814, 020cc618: `*field = pair; body(this);`). No code
// compares the field against a DS constant.
//
// ---- 2. ONE HOST WORD PER CODE WORD --------------------------------------
//
// The field is copied BY VALUE (thirty-odd writers) and compared by value
// (src/func_ov006_020c94e0.cpp compares a stored pair with a pair of the run),
// so each code word maps to ONE wrapper, written into every pair that holds
// it. Both sides of every comparison move together and a comparison answers
// what the cartridge answers: the MgMemory2_FieldPmf.cpp / FWD gate 3 rule.
//
// ---- 3. __cdecl, OFF BOTH LISTINGS ----------------------------------------
//
// Both TUs compile (under hostgen's MEMBER_REDECL rows for their two C2761s)
// to `push ecx / call edx / add esp, 4` (runs/linkfull/out/PMFMG1/
// func_ov006_020cb030.asm, func_ov006_020c8f20.asm): the receiver pushed and
// the caller cleaning it. So the cells hold __cdecl wrappers, one per code
// word, each counting what the two switches counted and calling the body with
// its own src parameter list. Both src TUs keep their own null guard on the
// code word, as the ROM does.
//
// ---- 4. THE WRONG TWIN IS FIXED IN src -----------------------------------
//
// Section 5b's finding (020c9024 called where the ROM calls 020cb134) is fixed
// in src/func_ov006_020cb030.cpp itself; its comment says so and its
// relocation row 0x020cb088 -> 0x020cb134 agrees.

static unsigned g_trmpln_field_hits;

#define TRM_W(code, cast)                                                  \
    static void tw_##code(void *o)                                        \
    {                                                                     \
        ++g_mario_state_hits;                                             \
        ++g_trmpln_field_hits;                                            \
        func_ov006_##code(cast o);                                        \
    }
TRM_W(020c905c, (char *))
TRM_W(020c91ac, (char *))
TRM_W(020c97bc, (char *))
TRM_W(020c9d7c, (char *))
TRM_W(020c9efc, (char *))
TRM_W(020ca070, (char *))
TRM_W(020ca310, (char *))
TRM_W(020ca39c, (int *))
TRM_W(020cb16c, (int *))
TRM_W(020cb2b4, )
TRM_W(020cb5c4, (char *))
TRM_W(020cb72c, (char *))
TRM_W(020cb838, (char *))
TRM_W(020cbd7c, (char *))
TRM_W(020cc2ac, (char *))
TRM_W(020cc408, (char *))
TRM_W(020cc63c, (int *))
TRM_W(020cc724, (char *))
TRM_W(020cc9b8, (int *))
TRM_W(020ccae0, (char *))
TRM_W(020ccd64, (char *))

extern "C" {
extern MgPmf data_ov006_0213b0f4;
extern MgPmf data_ov006_0213b0fc;
extern MgPmf data_ov006_0213b104;
extern MgPmf data_ov006_0213b10c;
extern MgPmf data_ov006_0213b114;
extern MgPmf data_ov006_0213b11c;
extern MgPmf data_ov006_0213b124;
extern MgPmf data_ov006_0213b12c;
extern MgPmf data_ov006_0213b134;
extern MgPmf data_ov006_0213b13c;
extern MgPmf data_ov006_0213b144;
extern MgPmf data_ov006_0213b14c;
extern MgPmf data_ov006_0213b154;
extern MgPmf data_ov006_0213b15c;
extern MgPmf data_ov006_0213b164;
extern MgPmf data_ov006_0213b16c;
extern MgPmf data_ov006_0213b174;
extern MgPmf data_ov006_0213b17c;
extern MgPmf data_ov006_0213b184;
extern MgPmf data_ov006_0213b18c;
extern MgPmf data_ov006_0213b194;
extern MgPmf data_ov006_0213b1a4;
extern MgPmf data_ov006_0213b1ac;
extern MgPmf data_ov006_0213b1b4;
extern MgPmf data_ov006_0213b1bc;
extern MgPmf data_ov006_0213b1c4;
extern MgPmf data_ov006_0213b1cc;
extern MgPmf data_ov006_0213b1d4;
extern MgPmf data_ov006_0213b1dc;
extern MgPmf data_ov006_0213b1e4;
extern MgPmf data_ov006_0213b1ec;
extern MgPmf data_ov006_0213b1f4;
extern MgPmf data_ov006_0213b1fc;
extern MgPmf data_ov006_0213b204;
extern MgPmf data_ov006_0213b20c;
extern MgPmf data_ov006_0213b214;
extern MgPmf data_ov006_0213b21c;
extern MgPmf data_ov006_0213b224;
}

static const struct { MgPmf *p; unsigned rom; void *host; } g_trmpln_seats[] = {
    {&data_ov006_0213b0f4, 0x020cc2acu, (void *)tw_020cc2ac},
    {&data_ov006_0213b0fc, 0x020c9d7cu, (void *)tw_020c9d7c},
    {&data_ov006_0213b104, 0x020ca39cu, (void *)tw_020ca39c},
    {&data_ov006_0213b10c, 0x020ca39cu, (void *)tw_020ca39c},
    {&data_ov006_0213b114, 0x020c905cu, (void *)tw_020c905c},
    {&data_ov006_0213b11c, 0x020ca39cu, (void *)tw_020ca39c},
    {&data_ov006_0213b124, 0x020ca310u, (void *)tw_020ca310},
    {&data_ov006_0213b12c, 0x020cc9b8u, (void *)tw_020cc9b8},
    {&data_ov006_0213b134, 0x020ca070u, (void *)tw_020ca070},
    {&data_ov006_0213b13c, 0x020c9efcu, (void *)tw_020c9efc},
    {&data_ov006_0213b144, 0x020ca39cu, (void *)tw_020ca39c},
    {&data_ov006_0213b14c, 0x020c97bcu, (void *)tw_020c97bc},
    {&data_ov006_0213b154, 0x020c9efcu, (void *)tw_020c9efc},
    {&data_ov006_0213b15c, 0x020cb2b4u, (void *)tw_020cb2b4},
    {&data_ov006_0213b164, 0x020ccd64u, (void *)tw_020ccd64},
    {&data_ov006_0213b16c, 0x020c9d7cu, (void *)tw_020c9d7c},
    {&data_ov006_0213b174, 0x020ccd64u, (void *)tw_020ccd64},
    {&data_ov006_0213b17c, 0x020ccd64u, (void *)tw_020ccd64},
    {&data_ov006_0213b184, 0x020ccae0u, (void *)tw_020ccae0},
    {&data_ov006_0213b18c, 0x020ccae0u, (void *)tw_020ccae0},
    {&data_ov006_0213b194, 0x020c91acu, (void *)tw_020c91ac},
    {&data_ov006_0213b1a4, 0x020ccd64u, (void *)tw_020ccd64},
    {&data_ov006_0213b1ac, 0x020cb838u, (void *)tw_020cb838},
    {&data_ov006_0213b1b4, 0x020c97bcu, (void *)tw_020c97bc},
    {&data_ov006_0213b1bc, 0x020cbd7cu, (void *)tw_020cbd7c},
    {&data_ov006_0213b1c4, 0x020cb72cu, (void *)tw_020cb72c},
    {&data_ov006_0213b1cc, 0x020cc2acu, (void *)tw_020cc2ac},
    {&data_ov006_0213b1d4, 0x020ca310u, (void *)tw_020ca310},
    {&data_ov006_0213b1dc, 0x020cbd7cu, (void *)tw_020cbd7c},
    {&data_ov006_0213b1e4, 0x020cc63cu, (void *)tw_020cc63c},
    {&data_ov006_0213b1ec, 0x020c91acu, (void *)tw_020c91ac},
    {&data_ov006_0213b1f4, 0x020cc408u, (void *)tw_020cc408},
    {&data_ov006_0213b1fc, 0x020cb2b4u, (void *)tw_020cb2b4},
    {&data_ov006_0213b204, 0x020cb72cu, (void *)tw_020cb72c},
    {&data_ov006_0213b20c, 0x020cb16cu, (void *)tw_020cb16c},
    {&data_ov006_0213b214, 0x020ca39cu, (void *)tw_020ca39c},
    {&data_ov006_0213b21c, 0x020cc724u, (void *)tw_020cc724},
    {&data_ov006_0213b224, 0x020cb5c4u, (void *)tw_020cb5c4},
};

/* Called from port_mg_framework_states_seat (unmatched/MgBase_StateDispatch
   .cpp), which hal/scene_mg.cpp runs once per process after the overlay
   constructors and before any trampoline element exists. */
extern "C" void port_mg_trmpln_field_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_trmpln_seats / sizeof g_trmpln_seats[0]; ++i) {
        if (g_trmpln_seats[i].p->code != g_trmpln_seats[i].rom ||
            g_trmpln_seats[i].p->adj != 0) {
            std::fprintf(stderr, "FATAL: trampoline Mario field pair %u: the "
                         "mount holds %08x/%d, the ROM's own word says %08x/0 "
                         "-- WRONG BYTES\n", i, g_trmpln_seats[i].p->code,
                         g_trmpln_seats[i].p->adj, g_trmpln_seats[i].rom);
            std::abort();
        }
        g_trmpln_seats[i].p->code = (unsigned)(size_t)g_trmpln_seats[i].host;
    }
}

/* The shared count, for unmatched/MgTrampolineTerror_SubDispatch.cpp's census
   entry point: one process runs one of the two scenes, so on scene 385 this is
   Trampoline Terror's element dispatch count and on scene 384 Trampoline
   Time's. */
extern "C" unsigned port_mg_trmpln_field_hits(void) { return g_trmpln_field_hits; }
