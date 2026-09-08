// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL in its WORSE form: three
// dispatchers that read the member pointer out of an OBJECT FIELD rather than
// out of a table.  Run mg6, lane MEM (dScMgMemory2_c, actor id 0x16b,
// scene 363).
//
// Read unmatched/MgBase_StateDispatch.cpp's header first for the ROM's dispatch
// sequence and the framework's entry points; read
// unmatched/MgMemory2_StateDispatch.cpp for this class's TABLE half.  This file
// is only the field half.
//
// ---- 1. WHY A FIELD IS WORSE THAN A TABLE ---------------------------------
//
// port/mg_fanout_costs.txt section 4 states it and this lane's three cases are
// three more instances: a table dispatched at the wrong stride is a wrong
// stride inside a correctly laid-out object, while a member pointer stored IN
// the object is four bytes on MSVC where the ROM has eight, so every field
// after it moves and the body reads the wrong field BEFORE any dispatch
// happens.  One of the three below has exactly that defect and it is spelled
// out in section 3.
//
// A LINK NEVER NAMES ANY OF THE THREE.  There is no global to be unresolved --
// the pointer is a field -- so an LNK1120 is silent on all of them however the
// tables are declared.  Only the `::*` sweep finds them, which is why
// port/mg_fanout_costs.txt makes the sweep a rule rather than a courtesy.  This
// lane swept its whole 869-file closure (runs/mg6/out/MEM/pmf_sweep_16b.txt)
// and found fourteen `::*` TUs: the six ov006 ones the handover named, seven
// ov004 ones MG1/MG2/BASESET already host-copied, and ONE THAT NO LANE HAD
// SEEN -- src/func_ov004_020b52fc.cpp, section 4 below.
//
// ---- 2. func_ov006_020c19d0, THE MODEL SUB-OBJECT'S TICK -------------------
//
// dScMgMemory2_c::Behavior calls it on `this + 0x4f38` every frame (the last
// statement of func_ov006_020f7458, disassembled in the state-dispatch file).
// It is SHARED ov006 glue, not this class's: eight src TUs name it, among them
// bodies belonging to four other minigames.  This is the first lane to reach
// it, so this file is where it lands.
//
// The ROM at 0x020c19d0, out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0:
//
//     stmdb sp!,{r4,lr}
//     mov   r4,r0
//     ldr   r2,[r4]           the CODE word
//     cmp   r2,#0
//     beq   +6                the ROM'S OWN null-pmf guard
//     ldr   r1,[r4,#4]        the adjustment
//     add   r0,r4,r1,asr #1
//     ands  r1,r1,#1
//     ldrne r1,[r0] / ldrne r1,[r1,r2]
//     ldreq r1,[r4]
//     blx   r1
//
// which is src/func_ov006_020c19d0.cpp's `if (*(int*)c != 0) (o->*(o->pmf))();`
// instruction for instruction.  The pair is at offset 0 of the sub-object, so
// nothing after it moves and this one is a wrong-decode only.
//
// ---- 3. func_ov006_020c07e8, AND IT IS THE LAYOUT CASE ---------------------
//
// Reached from 020c19d0's last statement, on `c + 0xdc`.  Its src struct is
//
//     char pad0[0xb4];  PMF pmf;  char pad1[0xc8 - 0xb4 - 8];  Vector3 v0c8;
//
// -- the pad after the member pointer is written 0xc8 - 0xb4 - 8, which is only
// correct at EIGHT BYTES.  Compiled as spelled, MSVC's four-byte member pointer
// puts v0c8 at 0xc4 and every field after it four bytes early, so the body
// would read the wrong words before the dispatch is even reached.  Spelling the
// pair as MgPmf (two ints, eight bytes on both machines) restores every offset
// the src file itself asserts, and nothing else in the struct moves.
//
// ITS IDLE TEST READS THE STORED PAIR BY VALUE, which is the reason this seat
// routes at the DISPATCH SITE and does not rewrite the mounted pair.  The body
// ends with
//
//     int* p = (int*)((int)c + 0xb4);  int* g = data_ov006_0213ac78;
//     if (p[0] == g[0]) { if (p[1] == g[1]) return; ... }
//
// and data_ov006_0213ac78 is a real mwcc pair, {0x020c0364, 0}.  A seat of the
// ov085/ov100 kind -- write HOST addresses over the .data source pairs before
// the constructor copies them -- would leave that comparison permanently false
// while the dispatch itself looked fine.  port/mg_fanout_costs.txt section 4
// derives the same rule from src/func_ov006_020c3b80.c on the flower's
// sub-object.  The stored pair keeps the ROM's own words here.
//
// ---- 4. func_ov004_020b52fc IS A FIND, NOT AN INHERITANCE ------------------
//
// The handover for this class named six ov006 TUs to host-copy and said the
// framework's own (func_ov004_020b87e0) was already trapped.  The sweep found a
// SEVENTH ov004 dispatcher that no lane has ever handled, and it is on this
// class's live path:
//
//     func_ov006_020f7458  (vtable slot 6, every frame)
//       -> func_ov004_020b65e4
//            -> for i in 0..0x13: func_ov004_020b52fc(&data_ov004_020bfa34[i*0x24])
//
// src/func_ov004_020b52fc.cpp is six lines, `struct C { PMF pmf; };
// (c->*(c->pmf))();`, and it is NOT in unmatched/MgBase_StateSetter.cpp's
// eighty-address universe: that file bounds ov004's TABLE member pointers, and
// this one is a field of a .bss record array.  Neither func_ov004_020b65e4 nor
// func_ov004_020b52fc appears in the baseline walk_window.map, so nothing in
// the port reached it before this lane and no neighbour changes because of it.
//
// THE ROM HAS NO NULL GUARD HERE, and that is worth stating because this file
// adds one.  0x020b52fc is fourteen instructions with no `cmp r2,#0` anywhere:
// it decodes and `blx r1` unconditionally, so on the DS a record whose pair is
// still {0,0} would branch to zero.  The port routes through port_mg_call0,
// whose first statement is the framework's `if (code == 0) return;`.  That is a
// divergence in the safe direction and it is visible rather than hidden: the
// dispatch call count in hal/scene_mg_memory2.cpp's census counts every one of
// the twenty per frame, so a run in which the records are never populated reads
// as a large call count and a small routed count.
//
// THE TWENTY RECORDS' PMF UNIVERSE IS BOUNDED FROM THE SOURCE, the way lane
// BASESET bounded ov004's.  data_ov004_020bfa34 is twenty records of 0x24
// bytes, constructed by __sinit_ov004_020b9ad0 through func_020733a8 with
// func_ov004_020b67e4 as the element constructor -- and that constructor is
// EMPTY, so the pairs start as the .bss zeros the null guard above is about.
// The writers are func_ov004_020b506c (`*(struct W2*)c = data_ov004_020bc7d8`)
// and func_ov004_020b5108 (data_ov004_020bc800 and _020bc820), and the pair
// symbols they draw from are a contiguous run in the ov004 image:
//
//     data_ov004_020bc7d8  {0x020b4ff0, 0}      data_ov004_020bc800  {0x020b51f0, 0}
//     data_ov004_020bc7e0  {0x020b4d50, 0}      data_ov004_020bc808  {0x020b4aa4, 0}
//     data_ov004_020bc7e8  {0x020b5288, 0}      data_ov004_020bc810  {0x020b4dfc, 0}
//     data_ov004_020bc7f0  {0x020b5288, 0}      data_ov004_020bc818  {0x020b4c30, 0}
//     data_ov004_020bc7f8  {0x020b4f44, 0}      data_ov004_020bc820  {0x020b5288, 0}
//
// Ten pairs, EIGHT distinct code words, every adjustment zero, read out of
// extracted/overlays/overlay_0004.bin at base 0x020ad660.  All eight are routed
// below and all eight have a matched src TU.
//
// ---- 5. THE MODEL SUB-OBJECT'S PMF UNIVERSE, BOUNDED THE SAME WAY ---------
//
// The pairs 020c19d0 and 020c07e8 dispatch come from one contiguous run of
// ov006 .data, 0x0213ac48..0x0213acb8, every one {code, 0}:
//
//     0213ac48 {020c11c0,0}   0213ac60 {020c14bc,0}   0213ac78 {020c0364,0}
//     0213ac50 {020c1760,0}   0213ac68 {020c0364,0}   0213ac80 {020c0b74,0}
//     0213ac58 {020c1760,0}   0213ac70 {020c0264,0}   0213ac88 {020c0364,0}
//     0213ac90 {020c1760,0}   0213ac98 {020c0ce8,0}   0213aca0 {020c0df0,0}
//     0213aca8 {020c0f9c,0}   0213acb0 {020c11c0,0}   0213acb8 {020c0f9c,0}
//
// Fifteen pairs, NINE distinct code words, all with a matched src TU.  The
// writers are the ordinary ones -- func_ov006_020c0304 and _020c0264 write
// +0xb4, func_ov006_020c0e8c and _020c1164 write offset 0 -- and 0213ac78 is
// the by-value sentinel section 3 names.  0x0213acc0 is where the run stops
// being pairs: it reads {4, 0x0213ad58}, which is a length and a pointer, and
// is the reason this list is taken from the CONSUMERS rather than from an
// address range.
//
// ---- 6. WHAT A RUN IS STILL THE ONLY DETECTOR FOR --------------------------
//
// Seventeen addresses are routed below because the source names them.  Nothing
// here claims that is all of them: a field pmf can be written by any code that
// can reach the object, and port/mg_fanout_costs.txt section 4's third shape
// (a TU that open-codes the Itanium sequence in plain ints) is invisible to
// both the link and the sweep.  Anything this file does not own falls through
// to the framework, which prints the address on an UNHANDLED line, and
// hal/scene_mg_memory2.cpp's census reports the count.  A run with zero
// UNHANDLED is the evidence; this header is not.

#include <cstdio>
#include <cstdlib>   /* std::abort, for the model seat below (lane MGWRITER) */

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines.  The same struct MgMemory2_StateDispatch.cpp uses; each TU declares
   its own because neither is a header. */
struct MgPmf { unsigned code; int adj; };

/* src/func_ov006_020c07e8.cpp's own Vector3, kept so the two Vec3_HorzAngle
   arguments are the shapes that file passes. */
struct MemVector3 { int x, y, z; };

extern "C" {

/* this class's table-half chain, which ends at the framework's port_mg_call0 */
void port_mg_memory2_call0(void *self, unsigned code, int adj);

/* ---- the eight ov004 record bodies, section 4 -------------------------- */
void func_ov004_020b4aa4(char *c);
void func_ov004_020b4c30(char *r4);
void func_ov004_020b4d50(char *c);
void func_ov004_020b4dfc(char *c);
void func_ov004_020b4f44(char *c);
void func_ov004_020b4ff0(char *c);
void func_ov004_020b51f0(char *c);
void func_ov004_020b5288(char *c);

/* ---- the nine ov006 model bodies, section 5 ---------------------------- */
void func_ov006_020c0264(char *c);
void func_ov006_020c0364(char *c);
void func_ov006_020c0b74(char *p);
void func_ov006_020c0ce8(char *c);
void func_ov006_020c0df0(char *c);
void func_ov006_020c0f9c(void *cc);
void func_ov006_020c11c0(char *c);
void func_ov006_020c14bc(char *c);
void func_ov006_020c1760(void);      /* 4 bytes in the ROM, `bx lr` */

/* the ordinary callees the two ov006 host copies keep, spelled as their own
   src TUs spell them */
void _ZN14BlendModelAnim7AdvanceEv(void *x);
int  _Z15ApproachLinear2Rsss(short &v, short a, short b);
short Vec3_HorzAngle(const void *a, const void *b);
void _Z11UpdateAngleRssis(short *p, short d, int n, short lim);
extern short data_ov006_0212b89c[];
extern int   data_ov006_0213ac78[2];
/* src/func_ov006_020c07e8.cpp declares this one OUTSIDE its extern "C" block,
   so it mangles as a C++ global and the ov006 mount's plain C definition does
   not satisfy it -- the ordinary name-spelling defect this port carries
   /alternatename rows for. The host copy declares it at C linkage instead,
   which is the same fix without an alias. */
extern struct MemVector3 data_ov006_0212b890;

/* host-copied below and called from above its own definition */
void func_ov006_020c07e8(void *x);

}  /* extern "C" */

// ---- THE MODEL SUB-OBJECT'S +0x00 SEAT, run link100 lane MGWRITER ----------
//
// WHAT THIS REPLACES. func_ov006_020c19d0's host copy stood below and handed
// the field's DS code word to mem2_field_try. It is src/func_ov006_020c19d0.cpp
// again, on port/slice_mgwriter.txt, because the ELEVEN .data pairs that can
// ever reach the +0x00 field now hold host addresses installed at boot.
//
// ---- WHY SECTION 3'S RULE DOES NOT FORBID THIS ----------------------------
//
// Section 3 says a seat that writes host addresses over the mounted pairs
// "would leave that comparison permanently false while the dispatch itself
// looked fine", and that is true of a seat that writes ONE SIDE. Four of the
// eleven pairs below are not installs at all; they are the by-value sentinels
// the class asks "is this slot idle" against:
//
//   data_ov006_0213ac58  src/func_ov006_020c1718.c   returns "idle" when the
//                        stored pair equals it (states 2, 5 and 7 of
//                        dScMgCard_c gate on this, and hal/scene_mg_card.cpp
//                        prints the same test as its census line)
//   data_ov006_0213ac90  src/func_ov006_020c16b4.c   the same test one caller over
//   data_ov006_0213acb0  src/func_ov006_020c0f0c.c   the first arm of its answer
//   data_ov006_0213aca8  src/func_ov006_020c0f0c.c   the second arm
//
// Every one of the four holds the SAME code word as an install in the same
// group (0x020c1760 twice, 0x020c11c0, 0x020c0f9c), so seating BOTH SIDES with
// the same host body leaves every comparison answering exactly what the
// cartridge answers, and `p[0] == 0` stays zero on an unwritten slot. That is
// the rule this lane derived at data_ov004_020bc254 in
// port/unmatched/MgBase_StateSetter.cpp and it is the same shape here.
//
// ---- THE UNIVERSE IS CLOSED BY RANGE, NOT BY A CONSUMER LIST -------------
//
// Section 6 says a run is still the only detector, because a field pmf can be
// written by any code that can reach the object. That is answered for this
// field. A relocation sweep over ov006 for every `load` row whose destination
// lands anywhere in the model sub-object's own code block 0x020c0000 ..
// 0x020c2000, keeping the rows whose FOLLOWING word reads zero, returns
// EXACTLY FIFTEEN pairs -- the fifteen section 5 lists, and nothing else. The
// four rows in that range that are not pairs are literal-pool words inside
// code, each followed by an ARM instruction (0x020c0af4, _020c1ef4, _020c228c
// and _020c3b7c, all holding 0x020c0134). So no storage anywhere in this
// overlay can hand this field a model-block address that is not one of the
// nine, and a code word from outside the block cannot be one of this class's
// own states. runs/link100/out/MGWRITER/sweep_ov006.txt is the listing.
//
// ---- THE SPLIT IS BY FIELD, AND THE TWO HALVES ARE DISJOINT --------------
//
// The fifteen serve TWO fields, and only the +0x00 one is seated here:
//
//   +0x00  func_ov006_020c19d0's field, ELEVEN pairs, SEVEN code words
//          (0x020c0b74, _0ce8, _0df0, _0f9c, _11c0, _14bc, _1760)
//          installs  ac48 ac50 ac60 ac80 ac98 aca0 acb8
//          sentinels ac58 ac90 aca8 acb0
//   +0xb4  func_ov006_020c07e8's field, FOUR pairs, TWO code words
//          (0x020c0264, _0364): ac68 (src/func_ov006_020c057c.cpp), ac70
//          (src/func_ov006_020c0304.c), ac88 (src/func_ov006_020c0264.cpp) and
//          the sentinel ac78 that body reads itself
//
// The two code-word sets are DISJOINT, so seating one half cannot hand the
// other half's dispatcher a host word: mem2_field_try keeps its 0x020c0264 and
// 0x020c0364 arms for func_ov006_020c07e8, and its eight ov004 arms for
// func_ov004_020b52fc, and loses only the seven this seat made unreachable.
//
// ---- WHY func_ov006_020c07e8 IS NOT TAKEN WITH IT ------------------------
//
// Two reasons, both measured. First, its src TU NEEDS /Zp4 and the tree does
// not carry it: the ROM reads the pair at `add r3, r6, #0xb4` (0x020c07f4) and
// MSVC without the option emits `mov ecx,[esi+188] / mov eax,[esi+184]`, which
// is +0xb8 -- section 3's own alignment case, four bytes late, a silent wrong
// read rather than a link error. With /Zp4 it emits +0xb4 and agrees with the
// ROM. Second, and this is the one that stops it rather than costs it an
// option: NEITHER the ROM nor the src has a null guard at 0x020c07e8 (fourteen
// instructions, no `cmp` before the `blx`), so retiring the host copy would
// take away the `if (code == 0) return` this port adds through
// port_mg_memory2_field_call, and an unwritten +0xb4 field would branch to
// zero. func_ov006_020c19d0 has no such exposure: `ldr r2,[r4]; cmp r2,#0;
// beq` is the ROM's OWN guard at 0x020c19d8 and its src spells it, so the
// retired TU refuses a zero exactly where the cartridge refuses it.
//
// ---- THE DISPATCH SHAPE, OFF THE TU'S OWN /FAsc LISTING ------------------
//
//   ROM       ldr r2,[r4] / cmp r2,#0 / ldr r1,[r4,#4] / add r0,r4,r1,asr #1 /
//             ands r1,r1,#1 / ldreq r1,[r4] / blx r1 -- zero argument, this in r0
//   EMITTED   mov eax,[esi] / test eax,eax / mov ecx,[esi+4] / add ecx,esi /
//             CALL eax, with no push before it and no add esp after it
//
// so the receiver is in ecx with ZERO stack arguments and the callee cleans
// nothing: the seven cells hold __fastcall FACES, not plain bodies. /Zp4 is a
// measured no-op on this TU (compiled both ways, the only differing listing
// line is the TITLE naming the .obj).
//
// ONE /alternatename. The matched TU declares `extern short
// data_ov006_0212b89c[]` outside its extern "C" block, so MSVC spells the
// reference ?data_ov006_0212b89c@@3PAFA -- read off the TU's own listing --
// while the ov006 mount defines the plain C name. Nothing in the tree DEFINES
// that LHS, so it cannot be silently defeated.

#pragma comment(linker, "/alternatename:?data_ov006_0212b89c@@3PAFA=_data_ov006_0212b89c")

extern "C" {
extern MgPmf data_ov006_0213ac48;
extern MgPmf data_ov006_0213ac50;
extern MgPmf data_ov006_0213ac58;
extern MgPmf data_ov006_0213ac60;
extern MgPmf data_ov006_0213ac80;
extern MgPmf data_ov006_0213ac90;
extern MgPmf data_ov006_0213ac98;
extern MgPmf data_ov006_0213aca0;
extern MgPmf data_ov006_0213aca8;
extern MgPmf data_ov006_0213acb0;
extern MgPmf data_ov006_0213acb8;
}

static unsigned g_m4f38_hits;
static unsigned g_m4f38_seated;

static void __fastcall m4f38_020c0b74(void *self, void *) { ++g_m4f38_hits; func_ov006_020c0b74((char *)self); }
static void __fastcall m4f38_020c0ce8(void *self, void *) { ++g_m4f38_hits; func_ov006_020c0ce8((char *)self); }
static void __fastcall m4f38_020c0df0(void *self, void *) { ++g_m4f38_hits; func_ov006_020c0df0((char *)self); }
static void __fastcall m4f38_020c0f9c(void *self, void *) { ++g_m4f38_hits; func_ov006_020c0f9c(self); }
static void __fastcall m4f38_020c11c0(void *self, void *) { ++g_m4f38_hits; func_ov006_020c11c0((char *)self); }
static void __fastcall m4f38_020c14bc(void *self, void *) { ++g_m4f38_hits; func_ov006_020c14bc((char *)self); }
static void __fastcall m4f38_020c1760(void *self, void *) { ++g_m4f38_hits; func_ov006_020c1760(); }

typedef void (*SeatFn)(void *);

/* Compare against the cartridge's own code word and a zero adjustment first,
   abort loudly on either mismatch, then write the host body. */
extern "C" void port_mg_memory2_model_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    static const struct {
        MgPmf *cell;
        const char *name;
        unsigned rom;
        SeatFn host;
    } seats[] = {
    { &data_ov006_0213ac48, "0213ac48", 0x020c11c0u, (SeatFn)m4f38_020c11c0 },
    { &data_ov006_0213ac50, "0213ac50", 0x020c1760u, (SeatFn)m4f38_020c1760 },
    { &data_ov006_0213ac58, "0213ac58", 0x020c1760u, (SeatFn)m4f38_020c1760 },
    { &data_ov006_0213ac60, "0213ac60", 0x020c14bcu, (SeatFn)m4f38_020c14bc },
    { &data_ov006_0213ac80, "0213ac80", 0x020c0b74u, (SeatFn)m4f38_020c0b74 },
    { &data_ov006_0213ac90, "0213ac90", 0x020c1760u, (SeatFn)m4f38_020c1760 },
    { &data_ov006_0213ac98, "0213ac98", 0x020c0ce8u, (SeatFn)m4f38_020c0ce8 },
    { &data_ov006_0213aca0, "0213aca0", 0x020c0df0u, (SeatFn)m4f38_020c0df0 },
    { &data_ov006_0213aca8, "0213aca8", 0x020c0f9cu, (SeatFn)m4f38_020c0f9c },
    { &data_ov006_0213acb0, "0213acb0", 0x020c11c0u, (SeatFn)m4f38_020c11c0 },
    { &data_ov006_0213acb8, "0213acb8", 0x020c0f9cu, (SeatFn)m4f38_020c0f9c },
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        if (seats[i].cell->code != seats[i].rom || seats[i].cell->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgCard_c model sub-object pair "
                         "data_ov006_%s: the mount holds %08x/%d, the ROM's own "
                         "bytes say %08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].cell->code, seats[i].cell->adj, seats[i].rom);
            std::abort();
        }
        seats[i].cell->code = (unsigned)(size_t)seats[i].host;
        ++g_m4f38_seated;
    }
}

extern "C" void port_mg_memory2_model_counts(unsigned *seated, unsigned *hits)
{
    if (seated) *seated = g_m4f38_seated;
    if (hits)   *hits   = g_m4f38_hits;
}

// ---- the field switch -------------------------------------------------------

static unsigned g_mem2_field_calls;
static unsigned g_mem2_field_hits;

static int mem2_field_try(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* ov004, data_ov004_020bfa34's ten pairs, eight distinct */
    case 0x020b4aa4u: func_ov004_020b4aa4(c); return 1;
    case 0x020b4c30u: func_ov004_020b4c30(c); return 1;
    case 0x020b4d50u: func_ov004_020b4d50(c); return 1;
    case 0x020b4dfcu: func_ov004_020b4dfc(c); return 1;
    case 0x020b4f44u: func_ov004_020b4f44(c); return 1;
    case 0x020b4ff0u: func_ov004_020b4ff0(c); return 1;
    case 0x020b51f0u: func_ov004_020b51f0(c); return 1;
    case 0x020b5288u: func_ov004_020b5288(c); return 1;
    /* ov006, the model sub-object's +0xb4 field, the only half left. Run
       link100 lane MGWRITER took the other seven arms out: the +0x00 field's
       eleven pairs hold host bodies now and func_ov006_020c19d0 dispatches
       them itself, so no path can present 0x020c0b74, _0ce8, _0df0, _0f9c,
       _11c0, _14bc or _1760 here again. These two stay because
       func_ov006_020c07e8 is still a host copy: the seat block above says why
       (its src TU needs /Zp4 and neither the ROM nor the src guards a null
       pair at 0x020c07e8, so the guard this file adds is load-bearing). */
    case 0x020c0264u: func_ov006_020c0264(c); return 1;
    case 0x020c0364u: func_ov006_020c0364(c); return 1;
    default:                                  return 0;
    }
}

/* The entry point the three host copies below use.  It tries this file's
   addresses and hands everything else to the class's table chain, which ends at
   the framework -- so the null-code guard, the nonzero-adjustment refusal and
   the UNHANDLED report still live in exactly one place. */
extern "C" void port_mg_memory2_field_call(void *self, unsigned code, int adj)
{
    ++g_mem2_field_calls;
    if (code != 0 && adj == 0 && mem2_field_try(self, code)) {
        ++g_mem2_field_hits;
        return;
    }
    port_mg_memory2_call0(self, code, adj);
}

extern "C" void port_mg_memory2_field_counts(unsigned *calls, unsigned *hits)
{
    if (calls) *calls = g_mem2_field_calls;
    if (hits)  *hits  = g_mem2_field_hits;
}

// ---- the three host copies -------------------------------------------------
//
// Each is its src TU verbatim except for the member-pointer declaration (MgPmf
// rather than a PMF field) and the dispatch site.  Where anything else moved it
// is stated on the line.

/* src/func_ov004_020b52fc.cpp, section 4.  Its whole body is the dispatch, so
   the host copy is the dispatch and nothing else.  The record pointer IS the
   object the pair belongs to, so `this` is the argument unchanged -- the ROM's
   `mov r0,r2` with a zero adjustment is exactly r0. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch through an object-field 8-byte member pointer MSVC's 4-byte pmf cannot reproduce
extern "C" void func_ov004_020b52fc(void *c)
{
    const MgPmf *p = (const MgPmf *)c;
    port_mg_memory2_field_call(c, p->code, p->adj);
}

/* func_ov006_020c19d0 IS GONE, run link100 lane MGWRITER: its host copy is
   src/func_ov006_020c19d0.cpp again, on port/slice_mgwriter.txt. The seat block
   above is the derivation. Its callee below is unchanged and still hosted, and
   the matched TU calls it by the same C name on c+0xdc, exactly as the src
   spells it. */

/* src/func_ov006_020c07e8.cpp, section 3.  The struct below is that file's own,
   with `PMF pmf` replaced by `MgPmf pmf` -- which is what makes the pads it
   spells (`0xc8 - 0xb4 - 8`) true again, and therefore what puts v0c8, a0ea,
   a0f0 and a0f4 back at the offsets the file asserts.  Nothing else in the
   layout or the body moves, including the by-value idle test at the end. */
struct MemModelC {
    char  pad0[0xb4];
    MgPmf pmf;                        /* 0xb4, two words -- DELTA: was PMF */
    char  pad1[0xc8 - 0xb4 - 8];
    MemVector3 v0c8;                  /* 0xc8 */
    char  pad2[0xea - 0xc8 - 12];
    short a0ea;                       /* 0xea */
    char  pad3[0xf0 - 0xea - 2];
    short a0f0;                       /* 0xf0 */
    char  pad4[0xf4 - 0xf0 - 2];
    int   a0f4;                       /* 0xf4 */
};

/* PORT_HOST_ABI: mwcc pointer-to-member wall, the field-embedded form (an
   8-byte {code,adj} pair stored INSIDE the object, MSVC's PMF is 4 bytes and
   strides the layout wrong); section 3 above. Host-copied with MgPmf in the
   pmf field's place so the rest of the struct's offsets stay true. */
extern "C" void func_ov006_020c07e8(void *x)
{
    MemModelC *c = (MemModelC *)x;

    port_mg_memory2_field_call(c, c->pmf.code, c->pmf.adj);

    _ZN14BlendModelAnim7AdvanceEv((char *)c + 0x18);

    if (c->a0f4 != 0) {
        MemVector3 t;
        t.x = data_ov006_0212b890.x;
        t.y = data_ov006_0212b890.y;
        t.z = data_ov006_0212b890.z;
        short d;
        int ang;
        ang = Vec3_HorzAngle(&c->v0c8, &t);
        d = (short)(ang - c->a0ea);
        if (d < -0x3000) d = -0x3000;
        else if (d > 0x3000) d = 0x3000;
        _Z11UpdateAngleRssis(&c->a0f0, d, 8, 0x200);

        {
            int *p = (int *)(((char *)c) + 0xb4);
            int *g = data_ov006_0213ac78;
            if (p[0] == g[0]) {
                if (p[1] == g[1])
                    return;
                if (*(int *)((char *)c + 0xb4) == 0)
                    return;
            }
        }
        d = (short)(ang - d);
        _Z11UpdateAngleRssis(&c->a0ea, d, 8, 0x200);
    } else {
        _Z11UpdateAngleRssis(&c->a0f0, 0, 8, 0x200);
    }
}
