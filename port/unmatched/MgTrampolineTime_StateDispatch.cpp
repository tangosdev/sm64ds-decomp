// PORT_HOST_ABI.  The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgTrampoline_c's ONE dispatcher and its five state addresses.
// Run mg11, lane TTI.  Actor id 0x180, scene 384, "Trampoline Time".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first for the ROM
// disassembly of the mwcc dispatch sequence and for what the framework entry
// point decides (null code, nonzero adjustment, unhandled address).  None of
// that is repeated here.
//
// ---- 1. THE CLASS IS dScMgTrampoline_c AND THE ROM SAYS SO ----------------
//
// The word IMMEDIATELY BEFORE the vtable is the type_info pointer, that
// record's second word is the name pointer and its third is the BASE's
// type_info.  Read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0, which is the shipped image and deliberately not a dsd export:
//
//     0x0213fb30 -> 0x0213fad0                the type_info
//     0x0213fad0 + 4 -> 0x0213faf0 -> "17dScMgTrampoline_c"
//     0x0213fad0 + 8 -> 0x0213c5c8 + 4 -> 0x0213c5d4 -> "14dScMgD3DBase_c"
//
// so the hierarchy is THREE DEEP and the ROM states both edges:
//
//     Scene -> dScMgBase_c      data_ov004_020bc0c0  36 slots
//           -> dScMgD3DBase_c   data_ov006_0213c62c  36 slots
//           -> dScMgTrampoline_c data_ov006_0213fb34 36 slots
//
// The code agrees twice over.  src/MgTrampolineTime_Spawn.c writes
// _ZTV17MgBounceAndPounce (which IS 0x0213c62c -- section 2) into the object and
// then data_ov006_0213fb34 over it; slots 16 and 17
// (src/func_ov006_021207dc.c, src/func_ov006_02120880.cpp) unwind them in the
// opposite order, and slot 17's literal pool holds both words in that order at
// 0x02120914 and 0x02120930.
//
// ---- 2. THE MIDDLE BASE IS MISNAMED IN config, AND FOUR CLASSES SHARE IT --
//
// config/arm9/overlays/ov006/symbols.txt calls 0x0213c62c
// `_ZTV17MgBounceAndPounce` and names its thirteen bodies
// `_ZN17MgBounceAndPounce*`.  The ROM's own RTTI says the table belongs to
// dScMgD3DBase_c, and FIVE type_info records in ov006 carry 0x0213c5c8 as their
// base pointer:
//
//     0x0213c620 (+8)  dScMgD3DBase_c itself, vtable 0x0213c62c
//     0x0213cb98       dScMgJump_c,        vtable 0x0213cbe4   id 0x174
//     0x0213cca4       dScMgJump2_c,       vtable 0x0213ccfc   id 0x175
//     0x0213fad0       dScMgTrampoline_c,  vtable 0x0213fb34   id 0x180  THIS
//     0x0213fc04       dScMgTrampoline2_c, vtable 0x0213fc7c   id 0x181
//
// symbols/actor_renames_report.txt already records the consequence as NINE
// "SKIP MgTrampolineTime: ... already claimed by MgBounceAndPounce" lines, and
// nine more for MgTrampolineTerror: the namer walked the derived classes in
// address order and gave the FIRST one the shared base's bodies.  FOUR
// committed places in this lane said eight; the dropped line is the first,
// `ov006:0x213c62c ... (vtable alloc=?)`, the TABLE rather than a body in it.
// The first correction said three and fixed three -- the fourth was
// port/tools/inferred_stub_adjudicated.txt.
//
// This lane does not rename anything -- config is not this lane's file and
// three sibling lanes are live on the same base in the same run -- it records
// the correction so the decomp side can route it.
//
// ---- 3. THE WALL IS ONE TU AND IT IS NOT A TABLE ---------------------------
//
// This class has NO state table.  Its member pointer is a FIELD, at this+0x5004
// (code) and this+0x5008 (adjustment) -- the eighth-PMF-instance shape run mg9
// lane PSY found, and the reason a sweep for pair tables belonging to this
// class finds nothing.  Five .data pairs are its states and each is read by
// exactly ONE of its own bodies' literal pools:
//
//   pair addr   code word   installed by            reached from
//   ---------   ---------   --------------------    ------------------------
//   0213fab0    02121d64    func_ov006_02121f04     slot 18 (the reset)
//   0213fac0    021218fc    func_ov006_02121cf4     state 02121d64
//   0213fac8    02121848    func_ov006_021218c4     state 021218fc
//   0213faa0    02121778    func_ov006_02121848     state 02121848 itself
//   0213faa8    02121774    func_ov006_02121778     state 02121778 itself
//
// so the machine is a five-link CHAIN, not an indexed table, and the port
// numbers the links 0..4 in that install order for the census.  Every
// adjustment word reads zero.
//
// THE PAIR RUN BRACKETS THE SpawnInfo AND SKIPS IT, which is
// port/mg_fanout_costs.txt section 4's curling trap and run mg9 lane LKY's
// again.  Two words inside 0x0213fa9c..0x0213fac8 are not pairs at all:
//
//     0x0213fa9c  an INT FLAG.  src/func_ov006_0212101c.c (slot 23) reads and
//                 writes it as data_ov006_0213fa9c; it is never called.
//     0x0213fab8  MgTrampolineTime_SpawnInfo -- the factory word 0x02122490
//                 followed by the doubled id 0x01800180.
//
// A sweep of the span would have produced seven "pairs" and routed two words
// that are not code.  The five literal pools that actually name a pair do not.
//
// THE ONE-TU CLAIM RESTS ON THE IMAGE, NOT ON A SOURCE CONVENTION.  The
// five-instruction ARM Itanium encoding
//
//     add Rd,Rn,Rm,asr #1   then   ands Rx,Ry,#1
//
// was scanned for BODY BY BODY, each body at its own start and size from
// config/arm9/overlays/ov006/symbols.txt -- never over an address span, which
// is the mistake lane LKY made twice and the reason its correction exists:
//
//   this class's 43 own bodies (0x021207dc..0x021225a4)      1 site
//                                            0x0212151c, in func_ov006_021214f8
//   the 13 INHERITED dScMgD3DBase_c bodies                   0 sites
//   whole-overlay control                                  114 sites
//
// THE ADDRESS A DECODE SITE IS REPORTED AT IS THE `add`, the FIRST instruction
// of the five-instruction sequence, in every place this lane names one.  The
// detector matches on the add/ands pair, so the add is what it has; the `blx`
// is three or five instructions later and is a different number for the same
// site.  Both sequences in full:
//
//     func_ov006_021214f8   add 0x0212151c   ands 0x02121520   blx 0x02121534
//     func_ov006_020cb030   add 0x020cb068   ands 0x020cb06c   blx 0x020cb080
//
// An earlier version of the element file reported ITS site at the blx while
// this class's was reported at the add, so two numbers that look like the same
// kind of thing were pointing at different instructions.
//
// One site, and it is vtable SLOT 6 -- the slot every seat wires by name, which
// is section 14's practical rule firing again: read the slot-6 src before
// wiring it.  The `::*` sweep over this class's closure agrees and adds
// nothing.
//
// ---- 4. THE SLOT-6 DISASSEMBLY, THE ONLY THING WORTH COPYING --------------
//
//   func_ov006_021214f8  vtable slot 6, Behavior, 0x84
//     push {r4,r5,lr} / sub sp,sp,#4
//     ldr  r1,[pc,#0x68]  (= 0x02140588) / mov r5,r0 / ldr r4,[r1]
//     bl   0x02120c40
//     ldr  r0,[pc,#0x5c]  (= 0x00005004) / add r3,r5,r0
//     ldr  r1,[r3,#4]                          the ADJUSTMENT, at +0x5008
//     add  r0,r5,r1,asr #1 / ands r1,r1,#1
//     ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1] / ldreq r1,[r3]
//     blx  r1                                  this = the class base, adj 0
//     mov  r0,r5 / bl 0x0212157c
//     ldr  r0,[pc,#0x30] (= 0x00005d84) / add r0,r5,r0 / bl 0x021209ac
//     ldr  r0,[pc,#0x1c] (= 0x02140588) / ldr r0,[r0] / cmp r4,r0
//     beq  0x02121560 / bl 0x020adb1c
//     mov  r0,#1 / add sp,sp,#4 / pop {r4,r5,lr} / bx lr
//
// 30 instructions plus a three-word pool = 33 words = 0x84, which is the size
// config records.  The host copy below is that body with the dispatch site
// replaced and nothing else moved.
//
// data_ov006_02140588 is ov006 .bss (past the image's 0x021402e0 end), so its
// value is a run-time reading and the saved/compare pair is kept verbatim
// rather than folded.
//
// ---- 5. NO STATE FLOOR, AND THE COUNTER STAYS ANYWAY ----------------------
//
// All five state addresses reach a real src TU and none of the five carries a
// NONMATCHING banner, so this class's floor count is ZERO on the state axis.
// The bodiless counter is kept for the reason MgBSC_StateDispatch.cpp keeps
// its own: hal/scene_mg_trampoline.cpp prints it on every run, so a later lane
// that adds an unreachable state gets a nonzero reading instead of silence.
//
// ---- 6. WHY THIS FILE HAS ITS OWN ENTRY POINT -----------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and chains it to
// exactly one per-class pair.  A second class cannot define those names, so
// this file calls port_mg_tti_call0, which tries THIS class's switch and hands
// everything else to the framework unchanged -- the shape MgCoin_, MgLuigi_,
// MgPachinko_, MgMemory2_ and MgBSC_StateDispatch.cpp all use.  The framework
// therefore remains the single place that decides what a null code word means,
// what a nonzero adjustment means and how an unhandled address is reported.
//
// There is no port_mg_tti_call1: this class has no arity-1 anything.  The run
// that sits IMMEDIATELY AFTER this class's vtable is dScMgTrampoline2_c's, and
// it is FIVE PAIRS: 0x0213fbd0, 0x0213fbd8, 0x0213fbe0, 0x0213fbe8 and
// 0x0213fbf0.  0x0213fbc8 is NOT one of them -- config names it
// MgTrampolineTerror_SpawnInfo and the word after it is the doubled id
// 0x01810181, which is the SpawnInfo-in-the-pair-run trap section 3 of this
// file documents for THIS class's own run.  An earlier version of this comment
// counted six and took the range from the SpawnInfo's factory word.  The five
// real code words land in 0x02123b20..0x02124088, past this class's last body
// (0x021225a8), and every literal pool that names one is inside that class's
// own code.  Deliberately NOT routed here; lane TTE owns it.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);

/* ---- the five routed state bodies, in install order ----------------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   ride-through is called the way the ROM calls it rather than the way a slot
   arity would suggest.  func_ov006_02121774 is a four-byte `bx lr` body whose
   src takes (void): the ROM body is one instruction long, so there is nothing
   for a missing argument to be wrong about.  That is the MgCoin_, MgMemory2_
   and MgBSC_StateDispatch.cpp ruling for the same shape. */
void func_ov006_02121d64(char *c);            /* link 0, the intro/countdown */
void func_ov006_021218fc(char *c);            /* link 1, the play state      */
void func_ov006_02121848(unsigned char *c);   /* link 2, the settle          */
void func_ov006_02121778(char *c);            /* link 3, the result          */
void func_ov006_02121774(void);               /* link 4, bx lr, terminal     */

/* the ordinary callees the host copy keeps, each spelled as its own src TU
   spells it */
void func_ov006_02120c40(void);
void func_ov006_0212157c(char *c);
void func_ov006_021209ac(short *o);
void func_ov004_020adb1c(int a);

/* ov006 .bss, written by the state bodies and read by slot 6 */
extern int data_ov006_02140588;

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_tti_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO -- section 5. */
static unsigned g_tti_floor_hits;
/* The chain link the switch was last entered on, so a run can say the machine
   MOVED rather than only that it fired.  This class has no state INDEX in the
   object -- the link number is this file's own numbering of the five code
   addresses in install order (section 3) -- so the low and high water marks
   below are the only progression witness that exists for it. */
static int g_tti_link_lo = -1, g_tti_link_hi = -1;
static unsigned g_tti_link_hits[5];
/* The last code word the field held, printed when it was routed nowhere. */
static unsigned g_tti_last_unrouted;
/* Behaviors entered with the field still zero -- the object ticked before its
   reset installed a state.  Printed rather than assumed away. */
static unsigned g_tti_unset;
/* Nonzero adjustments seen.  Every stored pair in the ROM reads {code, 0}, so
   this counter is the virtual-member-pointer arm of the ROM's own sequence and
   a nonzero reading would mean the field was written by something this lane
   did not find. */
static unsigned g_tti_virtual_arm;

static int tti_try_0(void *self, unsigned code, int *link)
{
    char *c = (char *)self;
    switch (code) {
    case 0x02121d64u: *link = 0; func_ov006_02121d64(c); return 1;
    case 0x021218fcu: *link = 1; func_ov006_021218fc(c); return 1;
    case 0x02121848u: *link = 2; func_ov006_02121848((unsigned char *)c); return 1;
    case 0x02121778u: *link = 3; func_ov006_02121778(c); return 1;
    case 0x02121774u: *link = 4; func_ov006_02121774();  return 1;
    default:                                             return 0;
    }
}

/* The entry point the host copy below uses.  Everything this switch does not
   own goes to the framework unchanged, so the null-code guard, the nonzero-
   adjustment refusal and the UNHANDLED report all still live in exactly one
   place. */
extern "C" void port_mg_tti_call0(void *self, unsigned code, int adj)
{
    int link = -1;
    if (adj != 0) ++g_tti_virtual_arm;
    if (code == 0) ++g_tti_unset;
    if (code != 0 && adj == 0 && tti_try_0(self, code, &link)) {
        ++g_tti_state_hits;
        ++g_tti_link_hits[link];
        if (g_tti_link_lo < 0 || link < g_tti_link_lo) g_tti_link_lo = link;
        if (link > g_tti_link_hi)                      g_tti_link_hi = link;
        return;
    }
    if (code != 0) g_tti_last_unrouted = code;
    port_mg_call0(self, code, adj);
}

extern "C" unsigned port_mg_tti_state_hits(void) { return g_tti_state_hits; }
extern "C" unsigned port_mg_tti_floor_hits(void) { return g_tti_floor_hits; }
extern "C" void port_mg_tti_link_range(int *lo, int *hi)
{
    *lo = g_tti_link_lo;
    *hi = g_tti_link_hi;
}
extern "C" void port_mg_tti_link_hits(unsigned *out5)
{
    for (int i = 0; i < 5; ++i) out5[i] = g_tti_link_hits[i];
}
extern "C" void port_mg_tti_anomalies(unsigned *unset, unsigned *virt,
                                      unsigned *last_unrouted)
{
    *unset         = g_tti_unset;
    *virt          = g_tti_virtual_arm;
    *last_unrouted = g_tti_last_unrouted;
}

// ---- the one host copy -----------------------------------------------------
//
// src/func_ov006_021214f8.cpp verbatim except for the field read (two ints
// rather than a member-pointer type) and the dispatch site (port_mg_tti_call0
// rather than `(c->*c->pmf)()`).  Nothing else moved.
//
// THE src IS SILENT TO A LINK AND TO A COMPILE.  It spells the field as
// `PMF pmf;` inside a local `struct C`, so there is no external member-pointer
// symbol for a `::*` sweep to find and nothing for the linker to complain
// about, while MSVC's single-inheritance member pointer is FOUR bytes where the
// ROM's field is eight -- so the compiled read takes the code word alone, calls
// it through MSVC's own thunk shape, and the adjustment word is never seen.
// It is the third of the three shapes port/mg_fanout_costs.txt section 4 names
// and the second time a class has hidden it at vtable slot 6.

extern "C" int func_ov006_021214f8(void *self)
{
    char *c = (char *)self;
    const int saved = data_ov006_02140588;

    func_ov006_02120c40();

    {
        const MgPmf *e = (const MgPmf *)(c + 0x5004);
        port_mg_tti_call0(c, e->code, e->adj);
    }

    func_ov006_0212157c(c);
    func_ov006_021209ac((short *)(c + 0x5d84));

    if (saved != data_ov006_02140588)
        func_ov004_020adb1c(data_ov006_02140588);

    return 1;
}
