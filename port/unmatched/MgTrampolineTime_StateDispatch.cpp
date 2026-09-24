// The mwcc POINTER-TO-MEMBER state pairs, per-class half:
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
//           -> dScMgTrampoline_c _ZTV17dScMgTrampoline_c 36 slots
//
// The code agrees twice over.  src/minigames/d_s_mg_trampoline.cpp writes
// _ZTV14dScMgD3DBase_c (which IS 0x0213c62c -- section 2) into the object and
// then _ZTV17dScMgTrampoline_c over it; slots 16 and 17
// (src/minigames/d_s_mg_trampoline.cpp, src/minigames/d_s_mg_trampoline.cpp) unwind them in the
// opposite order, and slot 17's literal pool holds both words in that order at
// 0x02120914 and 0x02120930.
//
// ---- 2. THE MIDDLE BASE IS MISNAMED IN config, AND FOUR CLASSES SHARE IT --
//
// config/arm9/overlays/ov006/symbols.txt calls 0x0213c62c
// `_ZTV14dScMgD3DBase_c` and names its thirteen bodies
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
//   0213fab0    02121d64    _ZN17dScMgTrampoline_c10BeginIntroEv     slot 18 (the reset)
//   0213fac0    021218fc    _ZN17dScMgTrampoline_c9BeginPlayEv     state 02121d64
//   0213fac8    02121848    _ZN17dScMgTrampoline_c12BeginResultsEv     state 021218fc
//   0213faa0    02121778    _ZN17dScMgTrampoline_c12StateResultsEv     state 02121848 itself
//   0213faa8    02121774    _ZN17dScMgTrampoline_c13StateWaitExitEv     state 02121778 itself
//
// so the machine is a five-link CHAIN, not an indexed table, and the port
// numbers the links 0..4 in that install order for the census.  Every
// adjustment word reads zero.
//
// THE PAIR RUN BRACKETS THE SpawnInfo AND SKIPS IT, which is
// port/mg_fanout_costs.txt section 4's curling trap and run mg9 lane LKY's
// again.  Two words inside 0x0213fa9c..0x0213fac8 are not pairs at all:
//
//     0x0213fa9c  an INT FLAG.  src/minigames/d_s_mg_trampoline.cpp (slot 23) reads and
//                 writes it as data_ov006_0213fa9c; it is never called.
//     0x0213fab8  g_profile_MG_TRAMPOLINE -- the factory word 0x02122490
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
//                                            0x0212151c, in _ZN17dScMgTrampoline_c8BehaviorEv
//   the 13 INHERITED dScMgD3DBase_c bodies                   0 sites
//   whole-overlay control                                  114 sites
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
// One site, and it is vtable SLOT 6 -- the slot every seat wires by name, which
// is section 14's practical rule firing again: read the slot-6 src before
// wiring it.  The `::*` sweep over this class's closure agrees and adds
// nothing.
//
// ---- 4. THE SLOT-6 DISASSEMBLY, THE ONLY THING WORTH COPYING --------------
//
//   _ZN17dScMgTrampoline_c8BehaviorEv  vtable slot 6, Behavior, 0x84
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
// config records.  It is the TU's own compiled body that runs now; section 6
// says what retired the host copy that used to stand in for it.
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
// ---- 6. THE HOST COPY IS RETIRED: THE ROM'S OWN BEHAVIOR RUNS THE FIELD ---
//
// Until run linkfull lane PMF2 this file carried a host copy of slot 6 that
// read the {code, adj} pair at +0x5004 and routed the code word through a DS
// address switch (port_mg_tti_call0, then unmatched/MgBase_StateDispatch.cpp's
// port_mg_call0).  That switch could not fire once the class's TU was
// promoted: src/minigames/d_s_mg_trampoline.cpp defines the five pairs itself,
//
//     P2 data_ov006_0213fab0 = { (int)_ZN17dScMgTrampoline_c10StateIntroEv, 0 };
//
// and on this port that initializer is a relocation against the HOST image:
// the word is the flat reverse face hal/faces_sync_gen.cpp defines for the
// state member, not 0x02121d64.  Every tick handed the switch a host word and
// the switch refused it (1153 refusals in 1200 frames of scene 384 on the
// wave-26 fold, the INT48 exe), so no state body ever ran and Trampoline Time
// sat on its intro.
//
// The TU's own dScMgTrampoline_c::Behavior IS slot 6 and it is already on the
// link line.  MSVC emits its dispatch as
//
//     mov ecx,[ebx+5008h] / mov eax,[ebx+5004h] / add ecx,ebx / call eax
//
// -- the receiver in ECX, NOTHING pushed: section 4's ARM sequence with the
// adjustment zero, which every one of the five pairs is.  A flat reverse face
// reads its receiver at [esp+4] (`mov ecx,[ebp+8]`), so the TU's words cannot
// be called that way as they stand.  Two changes retire the host copy:
//
//   1. the five pairs are SEATED with the __fastcall faces below, which take
//      the receiver in ECX and hand it to the flat face, the Trampoline Terror
//      shape (unmatched/MgTrampolineTerror_StateDispatch.cpp);
//   2. slot 6's flat name, which hal/scene_mg_trampoline.cpp's fill writes
//      into the table, is answered by a port/faces_sync.txt R row onto
//      ?Behavior@dScMgTrampoline_c@@UAEHXZ, the TU's own body.
//
// WHY THE SEAT RUNS AT C++ STATIC-INIT TIME.  The five pairs are not a mount.
// They are ordinary .data the TU defines, set by the loader from the
// relocations above before any code runs, and nothing writes them afterwards:
// the five installers only COPY a pair into the object.  So the earliest
// correct moment is also the only one needed -- before main, once, with no
// seam a scene boot has to remember to call.  hal/fdr_arm9_fader_seat.cpp and
// hal/fader_wipes.cpp seat at static-init time for the same reason.  The
// pairs sit inside the .dsstate span, and a snapshot is taken long after this
// has run, so a restore writes seated words back.
//
// THE CHECK IS AGAINST THIS BUILD'S OWN INITIALISER.  Each pair must read
// {the address of its flat face, 0}, which is what the TU's initializer
// produces here, or the seat refuses out loud: a DS word would mean the
// records came from a hosted blob, and a face word would mean a second seat
// owns them.  StateDone's flat face is folded by /OPT:ICF with other bodies of
// the same bytes; taking its address here yields the same folded address the
// TU's relocation does, so the comparison holds for it too.
//
// WHAT THE CENSUS STILL SEES.  hal/scene_mg_trampoline.cpp prints the link
// histogram and range this file exports, and the faces keep both: every face
// bumps its link before it forwards, so "routed" now counts the state bodies
// the ROM's own Behavior entered.  The three anomaly counters (a zero field, a
// nonzero adjustment, an unrouted word) belonged to the call-time switch and
// have no observer any more: the ROM's Behavior reads the field itself and
// tests none of the three, as the cartridge's does.  They report zero.

#include <cstdio>
#include <cstdlib>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* ---- the five state bodies' flat names, in install order ------------------
   hal/faces_sync_gen.cpp's reverse faces onto the TU's five members, each
   declared with the parameter list its own src member takes.  StateDone's
   member is a four-byte `bx lr` body that ignores its receiver; the face is
   still handed one, because a face reads its receiver whatever the member
   does with it. */
void _ZN17dScMgTrampoline_c10StateIntroEv(char *c);            /* link 0, the intro/countdown */
void _ZN17dScMgTrampoline_c9StatePlayEv(char *c);              /* link 1, the play state      */
void _ZN17dScMgTrampoline_c12StateResultsEv(unsigned char *c); /* link 2, the settle          */
void _ZN17dScMgTrampoline_c13StateWaitExitEv(char *c);         /* link 3, the result          */
void _ZN17dScMgTrampoline_c9StateDoneEv(char *c);              /* link 4, bx lr, terminal     */

/* The five pairs, defined by src/minigames/d_s_mg_trampoline.cpp with C
   linkage (hostgen), in the table order of section 3. */
extern MgPmf data_ov006_0213fab0, data_ov006_0213fac0, data_ov006_0213fac8,
    data_ov006_0213faa0, data_ov006_0213faa8;

}  /* extern "C" */

// ---- the census the faces keep ---------------------------------------------

static unsigned g_tti_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO -- section 5. */
static unsigned g_tti_floor_hits;
/* The lowest and highest chain link entered, so a run can say the machine
   MOVED rather than only that it fired.  This class has no state INDEX in the
   object -- the link number is this file's own numbering of the five pairs in
   install order (section 3) -- so the water marks and the per-link counts are
   the whole progression witness. */
static int g_tti_link_lo = -1, g_tti_link_hi = -1;
static unsigned g_tti_link_hits[5];

static void tti_note(int link)
{
    ++g_tti_state_hits;
    ++g_tti_link_hits[link];
    if (g_tti_link_lo < 0 || link < g_tti_link_lo) g_tti_link_lo = link;
    if (link > g_tti_link_hi)                      g_tti_link_hi = link;
}

// ---- the five faces ----------------------------------------------------------
//
// ONE FACE PER CODE WORD.  MSVC's member-pointer call puts `this + adj` in ECX
// and pushes nothing, so each face takes the receiver in ECX (the dead EDX
// absorbs __fastcall's second register), counts its link, and calls the flat
// face with the receiver pushed.  Arity zero: nothing on the stack to clean,
// the same `ret` a __thiscall member with no parameters makes.
#define TTI_FACE(tag, link, call)                                          \
    static void __fastcall tti_f_##tag(void *self, void *dead_edx)         \
    {                                                                      \
        (void)dead_edx;                                                    \
        tti_note(link);                                                    \
        call;                                                              \
    }

TTI_FACE(intro,   0, _ZN17dScMgTrampoline_c10StateIntroEv((char *)self))
TTI_FACE(play,    1, _ZN17dScMgTrampoline_c9StatePlayEv((char *)self))
TTI_FACE(results, 2, _ZN17dScMgTrampoline_c12StateResultsEv((unsigned char *)self))
TTI_FACE(wait,    3, _ZN17dScMgTrampoline_c13StateWaitExitEv((char *)self))
TTI_FACE(done,    4, _ZN17dScMgTrampoline_c9StateDoneEv((char *)self))

// ---- the seat -----------------------------------------------------------------
//
// {pair, face, the cartridge's code word, name}.  The table carries the faces
// and nothing else callable, so port/tools/pmf_guard.py's ledger row for it
// checks exactly the five words that land in the field.  The word each pair
// must hold BEFORE the seat is this build's own flat face, read in
// tti_initialiser_word rather than stored here.
namespace {
struct TtiSeat {
    MgPmf *rec;
    void (__fastcall *face)(void *, void *);
    unsigned rom;
    const char *what;
};
}  /* namespace */

static const TtiSeat g_tti_seats[5] = {
    {&data_ov006_0213fab0, tti_f_intro,   0x02121d64u, "0213fab0"},
    {&data_ov006_0213fac0, tti_f_play,    0x021218fcu, "0213fac0"},
    {&data_ov006_0213fac8, tti_f_results, 0x02121848u, "0213fac8"},
    {&data_ov006_0213faa0, tti_f_wait,    0x02121778u, "0213faa0"},
    {&data_ov006_0213faa8, tti_f_done,    0x02121774u, "0213faa8"},
};

static unsigned tti_initialiser_word(unsigned i)
{
    switch (i) {
    case 0: return (unsigned)(size_t)&_ZN17dScMgTrampoline_c10StateIntroEv;
    case 1: return (unsigned)(size_t)&_ZN17dScMgTrampoline_c9StatePlayEv;
    case 2: return (unsigned)(size_t)&_ZN17dScMgTrampoline_c12StateResultsEv;
    case 3: return (unsigned)(size_t)&_ZN17dScMgTrampoline_c13StateWaitExitEv;
    default: return (unsigned)(size_t)&_ZN17dScMgTrampoline_c9StateDoneEv;
    }
}

extern "C" void port_mg_tti_pairs_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_tti_seats / sizeof g_tti_seats[0]; ++i) {
        MgPmf *p = g_tti_seats[i].rec;
        const unsigned want = tti_initialiser_word(i);
        if (p->code != want || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgTrampoline_c pair %s holds "
                         "%08x/%d, this build's own initializer says %08x/0 "
                         "(the cartridge's is %08x/0) -- WRONG BYTES\n",
                         g_tti_seats[i].what, p->code, p->adj, want,
                         g_tti_seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)g_tti_seats[i].face;
    }
}

namespace {
struct TtiSeatAtStaticInit {
    TtiSeatAtStaticInit() { port_mg_tti_pairs_seat(); }
};
TtiSeatAtStaticInit g_tti_seat_at_static_init;
}  /* namespace */

// ---- the witnesses hal/scene_mg_trampoline.cpp prints ------------------------

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
/* Section 6: the call-time switch that counted these is retired and nothing
   observes the field between the installers and the ROM's Behavior, so all
   three read zero by construction. */
extern "C" void port_mg_tti_anomalies(unsigned *unset, unsigned *virt,
                                      unsigned *last_unrouted)
{
    *unset         = 0;
    *virt          = 0;
    *last_unrouted = 0;
}
