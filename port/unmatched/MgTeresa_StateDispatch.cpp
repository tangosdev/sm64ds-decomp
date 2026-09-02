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
   retired this file prints nothing at all and the seat owns the whole report. */

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

/* the four host copies this file DEFINES, so the seat can name them */
int  func_ov006_021203ac(char *c);
void func_ov006_0211dd0c(char *c);
void func_ov006_0211f6fc(char *c);
void func_ov006_0211d5a8(char *c);

void port_mg_teresa_counts(unsigned *hits, unsigned *unknown);
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

// ---- the four host copies --------------------------------------------------
//
// Each is its src TU with the member-pointer declaration replaced by MgPmf and
// the dispatch replaced by port_mg_teresa_call0/1. Everything else is
// verbatim, and every offset below was read off the ROM disassembly rather
// than off the src struct.

/* src/func_ov006_021203ac.cpp -- VTABLE SLOT 6, the Behavior. Table 02142eb0,
   arity 0. ROM 0x021203ac, size 0x50:
       add r0,r4,#0x4000 / ldr r0,[r0,#0xbe8]   the state index, a WORD at
                                                +0x4be8 (ldr, not ldrb)
       ldr r1,[pc,#0x34] -> pool 0x021203f8 = 02142eb0
       add r3,r1,r0,lsl#3                       an EIGHT-byte stride
       ldr r1,[r3,#4] / add r0,r4,r1,asr#1 / ands r1,r1,#1
       ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1] / ldreq r1,[r3] / blx r1
       mov r0,r4 / bl 0x0211e4e0
       mov r0,#1
   r1 holds the CODE WORD at the blx and no argument register is set, which is
   the arity-0 reading; stategen agrees. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgTeresa_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_021203ac(char *c)
{
    const unsigned j = (unsigned)*(int *)(c + 0x4be8);
    const MgPmf *e = &data_ov006_02142eb0[j];
    g_teresa_self = c;
    port_mg_teresa_call0(c, e->code, e->adj);
    func_ov006_0211e4e0(c);
    return 1;
}

/* src/func_ov006_0211dd0c.cpp -- table 02142e88, arity 1, argument 0.
   ROM 0x0211dd0c, size 0x60:
       add r1,r0,#0x4000 / ldrb r2,[r1,#0xbbc] / cmp r2,#0 -> return
       ldrb r1,[r1,#0xbba]                       the state index, a BYTE
       ldr r2,[pc,#0x30] -> pool 0x0211dd68 = 02142e88
       add r3,r2,r1,lsl#3 / the Itanium sequence
       mov r1,#0                                 the argument is a constant 0
       blx r2
   The gate byte is at +0x4bbc and the index byte at +0x4bba, both off
   this+0x4000, which is what the src's `char pad[0x4bba]; idx; gap; g;`
   spells and what this copy reads directly. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgTeresa_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_0211dd0c(char *c)
{
    if (*(unsigned char *)(c + 0x4bbc) == 0)
        return;
    {
        const MgPmf *e = &data_ov006_02142e88[*(unsigned char *)(c + 0x4bba)];
        port_mg_teresa_call1(c, e->code, e->adj, 0);
    }
}

/* src/func_ov006_0211d5a8.cpp -- table 02142f18, arity 1, argument 0.
   ROM 0x0211d5a8, size 0x60, the same shape as 0211dd0c with different
   offsets:
       add r1,r0,#0x4000 / ldrb r2,[r1,#0xbe0] / cmp r2,#0 -> return
       ldrb r1,[r1,#0xbe1]
       ldr r2,[pc,#0x30] -> pool 0x0211d604 = 02142f18
       add r3,r2,r1,lsl#3 / the Itanium sequence / mov r1,#0 / blx r2
   NOTE THE ORDER: the gate is at +0x4be0 and the index at +0x4be1, which is
   the opposite way round from 0211dd0c's pair. The src spells it that way too
   (`char pad[0x4be0]; g; idx;`) and both were re-read off the ROM. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgTeresa_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_0211d5a8(char *c)
{
    if (*(unsigned char *)(c + 0x4be0) == 0)
        return;
    {
        const MgPmf *e = &data_ov006_02142f18[*(unsigned char *)(c + 0x4be1)];
        port_mg_teresa_call1(c, e->code, e->adj, 0);
    }
}

/* src/func_ov006_0211f6fc.cpp -- table 02142ed8, arity 1, argument = the loop
   counter. ROM 0x0211f6fc, size 0x80:
       r7 = this; r5 = this; r6 = 0; r4 = pool 0x0211f778 = 02142ed8
     loop (0x0211f714):
       add r0,r5,#0x4000 / ldrb r1,[r0,#0x677] / cmp r1,#0 / beq skip
       ldrb r0,[r0,#0x678]
       add r3,r4,r0,lsl#3 / ldr r1,[r3,#4]
       add r0,r7,r1,asr#1                       THE RECEIVER IS r7, THE BASE
       ands r1,r1,#1 / ldrne... / mov r1,r6 / ldreq r2,[r3] / blx r2
       mov r0,r7 / mov r1,r6 / bl 0x0211f664
     skip:
       add r6,r6,#1 / cmp r6,#0x10 / add r5,r5,#0x24 / blt loop
   SIXTEEN RECORDS AT STRIDE 0x24, and the two bytes are read off the WALKING
   pointer r5 while the dispatch receiver stays the base object r7. Getting
   that backwards would hand every state a pointer 0x24 bytes into the record
   array. The increments are outside the gate's skip, so a gated-off record
   still advances the counter. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgTeresa_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_0211f6fc(char *c)
{
    int i;
    char *e = c;
    for (i = 0; i < 0x10; i++, e += 0x24) {
        if (*(unsigned char *)(e + 0x4677)) {
            const MgPmf *p =
                &data_ov006_02142ed8[*(unsigned char *)(e + 0x4678)];
            port_mg_teresa_call1(c, p->code, p->adj, i);
            func_ov006_0211f664(c, i);
        }
    }
}
