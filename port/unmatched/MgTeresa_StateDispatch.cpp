// PORT_HOST_ABI. dScMgTeresa_c's pointer-to-member state machine, host-copied.
// Run mg9, lane BOO. actor id 0x183 = scene 387, "Hide and Boo Seek".
//
// ---- FOUR DISPATCHING TUs, AND THE MACHINE IS TWO LEVELS DEEP -------------
//
// port/mg_fanout_costs.txt section 14 says the cost question a fan-out lane
// should ask is not "how many TUs name the table" but "how deep is the
// machine", and gives the cheap way to ask it: run stategen, then check
// whether any state ADDRESS it prints is also in the dispatcher list. This
// class answers in a variant of dScMgPanel_c's shape, and the variant matters
// for how the second level is reached.
//
//   L1  data_ov006_02142eb0  5 slots  arity 0  <- vtable slot 6, 021203ac
//   L2  data_ov006_02142e88  5 slots  arity 1  <- func_ov006_0211dd0c
//       data_ov006_02142ed8  8 slots  arity 1  <- func_ov006_0211f6fc
//       data_ov006_02142f18  9 slots  arity 1  <- func_ov006_0211d5a8
//
// On dScMgPanel_c five of the level-1 STATES are themselves dispatchers. Here
// they are not: four of the five level-1 states CALL the three level-2
// dispatchers by name, and those three are separate TUs.
//
//   021200a8 (L1 slot 1) -> 0211dd0c, 0211f6fc
//   02120008 (L1 slot 2) -> 0211d5a8, 0211f6fc
//   0211fe78 (L1 slot 3) -> 0211d5a8, 0211f6fc
//   0211fd44 (L1 slot 4) -> 0211d5a8, 0211f6fc
//   021200cc (L1 slot 0) -> neither
//
// So the four level-1 states stay ORDINARY SLICE LINES in port/slice_boo.txt
// and only the four dispatchers are copied here. A lane that host-copied slot
// 6 alone would link, boot, run the outer machine and then jump to a raw DS
// address the first frame an outer state advanced -- the same failure section
// 14 records, arrived at through a call rather than through a table.
//
// ---- ALL FOUR NAME A MEMBER-POINTER TYPE, SO ALL FOUR NEED A COPY ---------
//
// port/mg_fanout_costs.txt section 4's rulebook corollary keys on the
// SPELLING: "A PAIR WHOSE CONSUMER SPELLS IT AS TWO INTS IS SAFE AS AN ALIAS.
// A PAIR WHOSE CONSUMER NAMES A MEMBER-POINTER TYPE NEEDS A HOST COPY,
// WHETHER IT IS CALLED OR ONLY COPIED."
//
//   src/func_ov006_021203ac.cpp   struct Entry { PMF pmf; }   02142eb0  arity 0
//   src/func_ov006_0211dd0c.cpp   typedef void (C::*PMF)(int) 02142e88  arity 1
//   src/func_ov006_0211f6fc.cpp   typedef void (C::*PMF)(int) 02142ed8  arity 1
//   src/func_ov006_0211d5a8.cpp   typedef void (C::*PMF)(int) 02142f18  arity 1
//
// THE FIRST IS VTABLE SLOT 6 AND IT IS THE SPELLING facegen MISSES. Section 10
// tool finding 1: the WALL test is "P8" in the mangled name, and a struct that
// wraps a member pointer hides the P8, so `struct Entry { PMF pmf; } []`
// mangles ?data_ov006_02142eb0@@3PAUEntry@@A and slips both of facegen's
// guards. The struct-typed refusal does not catch it either -- it tests the
// by-value @@3U and an array of that struct is @@3PAU. Taken at face value it
// would have been generated as an ordinary alias, linked, and then strided an
// eight-byte table by four. The run law's "READ SLOT-6 SRC BEFORE WIRING BY
// NAME" is the rule that caught it.
//
// The other three declare the table as a bare `PMF data[]` outside extern "C",
// so they mangle with the P8 in view and the LINK named all three. Between the
// link and a `::*` sweep over all 80 src TUs in this class's closure, the four
// are found twice over; a scan of the same 80 for section 4's THIRD SHAPE (a
// TU carrying both a `>> 1` and an `& 1`, which is how func_ov006_020c3d18 and
// dScMgPanel_c's three open-code the decode in plain ints) returns EMPTY.
// Neither detector licenses the word "none" -- section 4 is explicit that the
// pair of them reaches twelve of thirteen and that only a run convicts the
// thirteenth -- so the evidence for "four and no more" is the FAULTS_FATAL=1
// boot with zero unknown code words, not this paragraph.
//
// ---- TWENTY-SEVEN PAIRS, READ OFF THE CONSTRUCTOR, NOT OFF A SWEEP -------
//
// Every pair below was read one assignment at a time out of
// src/__sinit_ov006_02132f68.c and then confirmed word for word in
// extracted/overlays/overlay_0006.bin at base 0x020bfec0. ALL TWENTY-SEVEN
// ADJUSTMENT WORDS READ ZERO and all twenty-seven code words are distinct.
//
// SWEEPING THE SPAN WOULD HAVE BEEN WRONG HERE IN THE EXACT WAY SECTION 4
// WARNS. The source pairs run 0x0213f8d4..0x0213f9ac, and 0x0213f974 inside
// that range is MgHideAndBooSeek_SpawnInfo -- this class's own SpawnInfo, the
// factory word followed by 0x01830183. A sweep would have manufactured it as
// a twenty-eighth "pair" whose adjustment is 0x01830183.
//
// ---- ROUTE AT THE DISPATCH SITE, NEVER BY REWRITING THE STORED PAIR ------
//
// The ov085 / ov100 shape -- write host addresses over the .data source pairs
// before the constructor copies them -- is wrong for this class for section
// 4's reason and for one of its own. The four tables live in ov006 .bss and
// their sources are interleaved with other classes' data in a span whose
// neighbours include a live SpawnInfo, and __sinit_ov006_02132f68 is not the
// only constructor reading that region. The stored pair keeps the ROM's own
// words and only the DECODE is replaced.
//
// THE STRIDE WAS NEVER THE PROBLEM. `struct MgPmf { unsigned code; int adj; }`
// is eight bytes on MSVC and eight bytes in the ROM, and the overlay
// constructor is a plain .c file that spells the pair as two ints, so the
// mount's tables have been correct all along -- section 4's "only the
// CONSUMERS were wrong".
//
// ---- THE ONE FLOOR IN A TABLE IS RETIRED (run mg10, lane F387) -----------
//
//   0x0211ebdc   slot 7 of data_ov006_02142ed8, size 0x258.
//
// Run mg9 emitted this address as a REPORTING case that returned -1 and never
// called anything, because no src file defined it. It is decompiled now:
// src/func_ov006_0211ebdc.c byte-matches the ROM under mwccarm 2004/b56 and
// passes tools/linkcheck.py VERIFIED with zero blind relocations. The case
// below is an ordinary call in the data_ov006_02142ed8 group beside its
// sibling 0x0211ee34, and THE FLOOR COUNTER IS GONE RATHER THAN LEFT READING
// ZERO -- a counter nothing can increment is not a measurement, and the number
// that still means something is the routed one, which absorbs this state's
// asks (539 over 1200 frames on the mg9 proof runs).
//
// THE OTHER TWO FLOORS. func_ov006_0211e72c was on the RENDER path (the sixth
// call vtable slot 9 makes) and is ALSO retired by this lane --
// src/func_ov006_0211e72c.c, same two gates -- so hal/scene_mg_booseek.cpp no
// longer carries a trap either. func_ov004_020ae5c4 is ov004's, is still a
// floor, and still gets its trap in hal/scene_mg_faces.cpp beside
// func_ov004_020ae858; seven ov006 TUs across the family reach it, so it is
// not this class's to retire.
//
// AMENDED, run mg12 lane PANEL: both have since been retired by the lanes that
// owned them -- func_ov004_020ae5c4 by run mg10 lane F371, func_ov004_020ae858 by
// this run. hal/scene_mg_faces.cpp carries no ov004 trap any more.

/* <cstdio> was included here for the floor's one-shot report; with the floor
   retired this file printed nothing at all for a while. Run link100 lane PMFB4
   brings both back for the boot installer's abort, which is the only thing in
   this file that reports anything now. */
#include <cstdio>
#include <cstdlib>

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the four state tables, re-typed -------------------------------------
   The ov006 mount defines the storage and __sinit_ov006_02132f68 fills it;
   that constructor is already in port/slice_mg1.txt with the other thirty, so
   nothing new is wired for the tables themselves. */
extern MgPmf data_ov006_02142e88[];
extern MgPmf data_ov006_02142eb0[];
extern MgPmf data_ov006_02142ed8[];
extern MgPmf data_ov006_02142f18[];

/* ---- the twenty-six reachable state bodies -------------------------------
   Reached ONLY through the switches below: the pair words are mounted DATA
   holding DS addresses, so nothing else in the build names them and /OPT:REF
   would drop every one of them without this file. All twenty-six are ordinary
   lines in port/slice_boo.txt.

   EVERY SIGNATURE HERE WAS CHECKED AGAINST THE ROM CALL AND AGAINST THE src
   TU'S OWN DEFINITION, because a dropped receiver is the bin-softlock disease.
   The three tables at arity 1 pass their argument in r1 and every one of their
   src definitions takes (pointer, int); the one exception is 0x0211ce90, whose
   whole body is a single `bx lr` at size 0x4 and whose src takes void. */

/* data_ov006_02142eb0, arity 0 -- the Behavior's own five */
void func_ov006_021200cc(char *c);
void func_ov006_021200a8(char *c);
void func_ov006_02120008(char *c);
void func_ov006_0211fe78(char *c);
void func_ov006_0211fd44(char *c);

/* data_ov006_02142e88, arity 1 */
void func_ov006_0211dce0(char *c, int i);
void func_ov006_0211db7c(char *c, int i);
void func_ov006_0211dad0(char *c, int i);
void func_ov006_0211d924(char *c, int i);
void func_ov006_0211d86c(char *c, int i);

/* data_ov006_02142ed8, arity 1 */
void func_ov006_0211f5d4(char *c, int i);
void func_ov006_0211f554(char *c, int i);
void func_ov006_0211f224(char *c, int i);
void func_ov006_0211f1a4(char *c, int i);
void func_ov006_0211f0d0(char *c, int i);
void func_ov006_0211f040(char *c, int i);
void func_ov006_0211ee34(char *c, int i);
void func_ov006_0211ebdc(char *c, int i);   /* mg9's floor, decompiled in mg10 */

/* data_ov006_02142f18, arity 1 */
void func_ov006_0211d4e8(char *c, int i);
void func_ov006_0211d368(char *c, int i);
void func_ov006_0211d224(char *c, int i);
void func_ov006_0211d0f8(char *c, int i);
void func_ov006_0211d018(char *c, int i);
void func_ov006_0211cef4(char *c, int i);
void func_ov006_0211ce94(char *c, int i);
void func_ov006_0211ce90(void);          /* `bx lr`, size 0x4, no argument */
void func_ov006_0211cd24(char *c, int i);

/* the one non-dispatching callee the four host copies below reach */
void func_ov006_0211e4e0(char *c);
void func_ov006_0211f664(char *c, int i);

/* Run link100 lane PMFB4 retired all four host copies. These four declarations
   named them; they name the matched TUs now, at the same C linkage and the same
   signatures, and nothing in this file calls any of them. They are kept because
   hal/scene_mg_booseek.cpp's vtable thunk for slot 6 declares
   func_ov006_021203ac the same way and the two must agree. */
int  func_ov006_021203ac(char *c);
void func_ov006_0211dd0c(char *c);
void func_ov006_0211f6fc(char *c);
void func_ov006_0211d5a8(char *c);

void port_mg_teresa_counts(unsigned *hits, unsigned *unknown);
/* the boot installer at the end of this file; hal/scene_mg.cpp calls it
   after the ov006 constructors have filled the tables. */
void port_mg_teresa_states_seat(void);
void port_mg_teresa_state_index(int *l1);

}  /* extern "C" */

/* THE WITNESS, AND THE TWO COUNTS ARE DISJOINT ON PURPOSE. A hit is a state
   this class ROUTED TO A REAL BODY; unknown is a code word neither switch
   knows, which is the number that convicts a missed dispatcher. dScMgPanel_c's
   first version counted a floor entry as a hit and read 2508 routed when 951
   had gone nowhere, which is why the two were ever separated.

   THERE WAS A THIRD COUNTER UNTIL RUN mg10 and it is gone rather than pinned
   at zero: it counted asks for 0x0211ebdc, the one state address with no
   decompiled body, and that address has one now. The rule the third counter
   enforced still holds -- a switch returns +1 only for a body it actually
   called -- and it is the rule, not the counter, that was load-bearing. */
static unsigned g_teresa_hits;
static unsigned g_teresa_unknown;
static char *g_teresa_self;

static int teresa_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142eb0, the Behavior's own five */
    case 0x021200ccu: func_ov006_021200cc(c); return 1;
    case 0x021200a8u: func_ov006_021200a8(c); return 1;
    case 0x02120008u: func_ov006_02120008(c); return 1;
    case 0x0211fe78u: func_ov006_0211fe78(c); return 1;
    case 0x0211fd44u: func_ov006_0211fd44(c); return 1;
    default:
        return 0;
    }
}

static int teresa_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142e88 */
    case 0x0211dce0u: func_ov006_0211dce0(c, a); return 1;
    case 0x0211db7cu: func_ov006_0211db7c(c, a); return 1;
    case 0x0211dad0u: func_ov006_0211dad0(c, a); return 1;
    case 0x0211d924u: func_ov006_0211d924(c, a); return 1;
    case 0x0211d86cu: func_ov006_0211d86c(c, a); return 1;
    /* data_ov006_02142ed8 */
    case 0x0211f5d4u: func_ov006_0211f5d4(c, a); return 1;
    case 0x0211f554u: func_ov006_0211f554(c, a); return 1;
    case 0x0211f224u: func_ov006_0211f224(c, a); return 1;
    case 0x0211f1a4u: func_ov006_0211f1a4(c, a); return 1;
    case 0x0211f0d0u: func_ov006_0211f0d0(c, a); return 1;
    case 0x0211f040u: func_ov006_0211f040(c, a); return 1;
    case 0x0211ee34u: func_ov006_0211ee34(c, a); return 1;
    /* slot 7, run mg9's reported floor, decompiled by run mg10 lane F387.
       0x0211ee34 above is slot 6 of the same table and the two read the same
       record fields, which is the cross-check on the layout. */
    case 0x0211ebdcu: func_ov006_0211ebdc(c, a); return 1;
    /* data_ov006_02142f18 */
    case 0x0211d4e8u: func_ov006_0211d4e8(c, a); return 1;
    case 0x0211d368u: func_ov006_0211d368(c, a); return 1;
    case 0x0211d224u: func_ov006_0211d224(c, a); return 1;
    case 0x0211d0f8u: func_ov006_0211d0f8(c, a); return 1;
    case 0x0211d018u: func_ov006_0211d018(c, a); return 1;
    case 0x0211cef4u: func_ov006_0211cef4(c, a); return 1;
    case 0x0211ce94u: func_ov006_0211ce94(c, a); return 1;
    case 0x0211ce90u: func_ov006_0211ce90();     return 1;
    case 0x0211cd24u: func_ov006_0211cd24(c, a); return 1;
    default:
        return 0;
    }
}

/* THE TWO ENTRY POINTS the host copies below call. A hit is this class's; a
   miss falls through to the framework, which owns the guards and the report,
   and is counted here as well because a nonzero unknown count is the number
   that says a dispatcher was missed. */
extern "C" void port_mg_teresa_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0) {
        const int r = teresa_try_0(self, code);
        if (r > 0) { ++g_teresa_hits; return; }
        /* -1 is the tri-state's "a named floor, already reported". NOTHING IN
           THIS CLASS RETURNS IT ANY MORE -- run mg10 decompiled the one address
           that did -- and the arm is kept rather than deleted so a future floor
           in this class lands on the shape the other seats use instead of being
           silently counted as unknown. */
        if (r < 0) { return; }
    }
    if (code != 0)
        ++g_teresa_unknown;
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_teresa_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0) {
        const int r = teresa_try_1(self, code, a);
        if (r > 0) { ++g_teresa_hits; return; }
        if (r < 0) { return; }
    }
    if (code != 0)
        ++g_teresa_unknown;
    port_mg_call1(self, code, adj, a);
}

extern "C" void port_mg_teresa_counts(unsigned *hits, unsigned *unknown)
{
    if (hits)    *hits    = g_teresa_hits;
    if (unknown) *unknown = g_teresa_unknown;
}

/* The level-1 state index, read back out of the live object for the run
   report. -1 until the Behavior has run once, so a scene that never reached
   slot 6 cannot report a state. */
extern "C" void port_mg_teresa_state_index(int *l1)
{
    if (l1)
        *l1 = g_teresa_self ? *(int *)(g_teresa_self + 0x4be8) : -1;
}
// ---- NO HOST COPY OF A dScMgTeresa_c DISPATCHER IS LEFT --------------------
//
// FOUR at run mg10, NONE after run link100 lane PMFB4. All four of this class's
// state tables now hold HOST addresses, written at boot by
// port_mg_teresa_states_seat below after every cell has been compared against
// the ROM's own code word and a zero adjustment word, so the four matched TUs
// dispatch through the tables directly:
//
//   func_ov006_021203ac  data_ov006_02142eb0   5 slots  arity 0  (vtable slot 6)
//   func_ov006_0211dd0c  data_ov006_02142e88   5 slots  arity 1
//   func_ov006_0211d5a8  data_ov006_02142f18   9 slots  arity 1
//   func_ov006_0211f6fc  data_ov006_02142ed8   8 slots  arity 1
//
// 5+5+9+8 = 27, which is the whole census this file's header counts.
//
// THE TWO SWITCHES ARE KEPT AND ARE NOW UNREACHABLE. Nothing calls
// port_mg_teresa_call0/1 any more: the four host copies were their only
// callers. They are left rather than deleted because they are the written
// record of which state address belongs to which table, and because
// g_teresa_unknown reading zero is now a STRUCTURAL fact rather than a
// measurement -- the defect that counter used to convict at frame time is
// convicted at boot instead, by the installer's abort.
//
// THE STRIDE, BOTH SIDES, PER ROW -- measured on this tree
// (runs/link100/out/PMFB4/rom_gate2.txt and emit_gate2.txt):
//
//   021203ac  ROM add r3,r1,r0,lsl #3  pool 021203f8 = 02142eb0  emitted [eax*8]
//   0211dd0c  ROM add r3,r4,r0,lsl #3  pool 0211dd68 = 02142e88  emitted [eax*8]
//   0211d5a8  ROM add r3,r4,r0,lsl #3  pool 0211d604 = 02142f18  emitted [eax*8]
//   0211f6fc  ROM add r3,r4,r0,lsl #3  pool 0211f778 = 02142ed8  emitted [eax*8]
//
// ROM 8 == emitted 8 on all four, with [eax*8+4] for the adjustment word.
// /Zp4 IS A MEASURED NO-OP on every one: each was compiled under the port's own
// flags both ways for this lane and the two /FAsc listings came back identical
// except for the TITLE line naming the .obj.
//
// THE TWENTY-SEVEN SOURCE PAIRS ALL READ {code, 0} in overlay_0006.bin at the
// addresses src/__sinit_ov006_02132f68.c copies each slot from, and every one is
// a WHOLE-PAIR copy with no field-form fill anywhere:
//
//   02142e88[0] <- 0213f99c 0211dce0/0   02142f18[0] <- 0213f90c 0211d4e8/0
//   02142e88[1] <- 0213f994 0211db7c/0   02142f18[1] <- 0213f95c 0211d368/0
//   02142e88[2] <- 0213f98c 0211dad0/0   02142f18[2] <- 0213f91c 0211d224/0
//   02142e88[3] <- 0213f984 0211d924/0   02142f18[3] <- 0213f8fc 0211d0f8/0
//   02142e88[4] <- 0213f97c 0211d86c/0   02142f18[4] <- 0213f944 0211d018/0
//   02142eb0[0] <- 0213f904 021200cc/0   02142f18[5] <- 0213f8f4 0211cef4/0
//   02142eb0[1] <- 0213f92c 021200a8/0   02142f18[6] <- 0213f9ac 0211ce94/0
//   02142eb0[2] <- 0213f954 02120008/0   02142f18[7] <- 0213f9a4 0211ce90/0
//   02142eb0[3] <- 0213f964 0211fe78/0   02142f18[8] <- 0213f924 0211cd24/0
//   02142eb0[4] <- 0213f914 0211fd44/0
//   02142ed8[0] <- 0213f934 0211f5d4/0   02142ed8[4] <- 0213f8e4 0211f0d0/0
//   02142ed8[1] <- 0213f8dc 0211f554/0   02142ed8[5] <- 0213f8d4 0211f040/0
//   02142ed8[2] <- 0213f94c 0211f224/0   02142ed8[6] <- 0213f93c 0211ee34/0
//   02142ed8[3] <- 0213f8ec 0211f1a4/0   02142ed8[7] <- 0213f96c 0211ebdc/0
//
// THE DISPATCH SHAPE, off each row's own listing: `mov ecx, tab[i*8+4] /
// mov eax, tab[i*8] / add ecx, <this> / call eax` with NO `add esp,N` after it,
// so receiver in ecx and callee cleanup. 0211d5a8 and 0211dd0c push a literal 0
// (their src's own `(0)`), 0211f6fc pushes esi (the loop counter), and 021203ac
// pushes nothing. So 02142e88, 02142f18 and 02142ed8 take one-argument
// __fastcall faces and 02142eb0 takes zero-argument ones.
//
// THREE /alternatename DIRECTIVES. 0211d5a8, 0211dd0c and 0211f6fc declare their
// tables at namespace scope as pointer-to-member arrays and come in as
// ?data_ov006_02142f18@@3PAP8C@@AEXH@ZA, ?data_ov006_02142e88@@3PAP8C@@AEXH@ZA
// and ?data_ov006_02142ed8@@3PAP8C@@AEXH@ZA; 021203ac's is the Entry-wrapper
// spelling ?data_ov006_02142eb0@@3PAUEntry@@A. All four read off the objects
// with dumpbin /symbols. Safe under port/tools/alternatename_guard.py for
// hal/pmfc_aliases.cpp's reason: each LHS is a C++ decoration only these matched
// TUs ever spell.
#pragma comment(linker, "/alternatename:?data_ov006_02142f18@@3PAP8C@@AEXH@ZA=_data_ov006_02142f18")
#pragma comment(linker, "/alternatename:?data_ov006_02142e88@@3PAP8C@@AEXH@ZA=_data_ov006_02142e88")
#pragma comment(linker, "/alternatename:?data_ov006_02142ed8@@3PAP8C@@AEXH@ZA=_data_ov006_02142ed8")
#pragma comment(linker, "/alternatename:?data_ov006_02142eb0@@3PAUEntry@@A=_data_ov006_02142eb0")

/* EVERY FACE COUNTS, AND THE BEHAVIOR'S FACES CARRY THE CENSUS RECEIVER.
   g_teresa_hits keeps counting exactly the dispatches that happen, as
   port_mg_teresa_callN counted them while the host copies routed through the
   switch. g_teresa_self is the harder one: hal/scene_mg_booseek.cpp reads the
   top-level state index through port_mg_teresa_state_index, and the only place
   that ever recorded the receiver was func_ov006_021203ac -- the Behavior --
   just before its dispatch. The Behavior is the matched TU now, so its table's
   five faces record it instead. Same object, same instant in the frame: the
   face runs from inside that same dispatch. Without this the census would read
   -1 forever and nothing would be wrong, which is the witness trap lane PMFB3
   recorded against the tapped-pad counter. */
#define TR_FACE1(sym)                                                         \
    static void __fastcall tr_##sym(void *self, void *dead_edx, int i)        \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_teresa_hits;                                                      \
        sym((char *)self, i);                                                 \
    }

/* slot 7 of data_ov006_02142f18: `bx lr`, size 0x4, and its src TU takes no
   argument, which is the shape port/tools/aritycheck.py checks. */
#define TR_FACE1_VOID(sym)                                                    \
    static void __fastcall tr_##sym(void *self, void *dead_edx, int i)        \
    {                                                                         \
        (void)self; (void)dead_edx; (void)i;                                  \
        ++g_teresa_hits;                                                      \
        sym();                                                                \
    }

/* data_ov006_02142eb0, the Behavior's own table: nothing pushed, and the
   receiver is recorded here for port_mg_teresa_state_index. */
#define TR_FACE0(sym)                                                         \
    static void __fastcall tr_##sym(void *self, void *dead_edx)               \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_teresa_hits;                                                      \
        g_teresa_self = (char *)self;                                         \
        sym((char *)self);                                                    \
    }

/* data_ov006_02142f18, arity 1 */
TR_FACE1(func_ov006_0211d4e8)
TR_FACE1(func_ov006_0211d368)
TR_FACE1(func_ov006_0211d224)
TR_FACE1(func_ov006_0211d0f8)
TR_FACE1(func_ov006_0211d018)
TR_FACE1(func_ov006_0211cef4)
TR_FACE1(func_ov006_0211ce94)
TR_FACE1_VOID(func_ov006_0211ce90)
TR_FACE1(func_ov006_0211cd24)
/* data_ov006_02142e88, arity 1 */
TR_FACE1(func_ov006_0211dce0)
TR_FACE1(func_ov006_0211db7c)
TR_FACE1(func_ov006_0211dad0)
TR_FACE1(func_ov006_0211d924)
TR_FACE1(func_ov006_0211d86c)
/* data_ov006_02142ed8, arity 1 */
TR_FACE1(func_ov006_0211f5d4)
TR_FACE1(func_ov006_0211f554)
TR_FACE1(func_ov006_0211f224)
TR_FACE1(func_ov006_0211f1a4)
TR_FACE1(func_ov006_0211f0d0)
TR_FACE1(func_ov006_0211f040)
TR_FACE1(func_ov006_0211ee34)
TR_FACE1(func_ov006_0211ebdc)
/* data_ov006_02142eb0, arity 0 */
TR_FACE0(func_ov006_021200cc)
TR_FACE0(func_ov006_021200a8)
TR_FACE0(func_ov006_02120008)
TR_FACE0(func_ov006_0211fe78)
TR_FACE0(func_ov006_0211fd44)

extern "C" void port_mg_teresa_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    static const struct {
        MgPmf *table;
        const char *name;
        unsigned slot;
        unsigned rom;
        void *face;
    } seats[] = {
        {data_ov006_02142f18, "02142f18", 0, 0x0211d4e8u, (void *)tr_func_ov006_0211d4e8},
        {data_ov006_02142f18, "02142f18", 1, 0x0211d368u, (void *)tr_func_ov006_0211d368},
        {data_ov006_02142f18, "02142f18", 2, 0x0211d224u, (void *)tr_func_ov006_0211d224},
        {data_ov006_02142f18, "02142f18", 3, 0x0211d0f8u, (void *)tr_func_ov006_0211d0f8},
        {data_ov006_02142f18, "02142f18", 4, 0x0211d018u, (void *)tr_func_ov006_0211d018},
        {data_ov006_02142f18, "02142f18", 5, 0x0211cef4u, (void *)tr_func_ov006_0211cef4},
        {data_ov006_02142f18, "02142f18", 6, 0x0211ce94u, (void *)tr_func_ov006_0211ce94},
        {data_ov006_02142f18, "02142f18", 7, 0x0211ce90u, (void *)tr_func_ov006_0211ce90},
        {data_ov006_02142f18, "02142f18", 8, 0x0211cd24u, (void *)tr_func_ov006_0211cd24},

        {data_ov006_02142e88, "02142e88", 0, 0x0211dce0u, (void *)tr_func_ov006_0211dce0},
        {data_ov006_02142e88, "02142e88", 1, 0x0211db7cu, (void *)tr_func_ov006_0211db7c},
        {data_ov006_02142e88, "02142e88", 2, 0x0211dad0u, (void *)tr_func_ov006_0211dad0},
        {data_ov006_02142e88, "02142e88", 3, 0x0211d924u, (void *)tr_func_ov006_0211d924},
        {data_ov006_02142e88, "02142e88", 4, 0x0211d86cu, (void *)tr_func_ov006_0211d86c},

        {data_ov006_02142ed8, "02142ed8", 0, 0x0211f5d4u, (void *)tr_func_ov006_0211f5d4},
        {data_ov006_02142ed8, "02142ed8", 1, 0x0211f554u, (void *)tr_func_ov006_0211f554},
        {data_ov006_02142ed8, "02142ed8", 2, 0x0211f224u, (void *)tr_func_ov006_0211f224},
        {data_ov006_02142ed8, "02142ed8", 3, 0x0211f1a4u, (void *)tr_func_ov006_0211f1a4},
        {data_ov006_02142ed8, "02142ed8", 4, 0x0211f0d0u, (void *)tr_func_ov006_0211f0d0},
        {data_ov006_02142ed8, "02142ed8", 5, 0x0211f040u, (void *)tr_func_ov006_0211f040},
        {data_ov006_02142ed8, "02142ed8", 6, 0x0211ee34u, (void *)tr_func_ov006_0211ee34},
        {data_ov006_02142ed8, "02142ed8", 7, 0x0211ebdcu, (void *)tr_func_ov006_0211ebdc},

        {data_ov006_02142eb0, "02142eb0", 0, 0x021200ccu, (void *)tr_func_ov006_021200cc},
        {data_ov006_02142eb0, "02142eb0", 1, 0x021200a8u, (void *)tr_func_ov006_021200a8},
        {data_ov006_02142eb0, "02142eb0", 2, 0x02120008u, (void *)tr_func_ov006_02120008},
        {data_ov006_02142eb0, "02142eb0", 3, 0x0211fe78u, (void *)tr_func_ov006_0211fe78},
        {data_ov006_02142eb0, "02142eb0", 4, 0x0211fd44u, (void *)tr_func_ov006_0211fd44},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        MgPmf *p = &seats[i].table[seats[i].slot];
        if (p->code != seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgTeresa_c state table %s slot %u: "
                         "the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].slot, p->code, p->adj, seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats[i].face;
    }
}
