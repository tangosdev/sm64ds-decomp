// PORT_HOST_ABI. dScMgSound_c's pointer-to-member state machine, host-copied.
// Run mg9, lane BOX. actor id 0x16f = scene 367, the "Boom Box" minigame.
//
// Everything below was read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0, never out of a dsd export. The alignment was checked first on a
// body this lane did not write and could not have tuned: func_ov006_0210a600
// disassembles to `mov r0,#1 / bx lr`, which is exactly what
// port/tools/inferred_stub_adjudicated.txt already records for it.
//
// ---- WHY THIS FILE HAS SIXTEEN HOST COPIES --------------------------------
//
// port/mg_fanout_costs.txt section 4 costs the wall at "five dispatching TUs
// to host-copy" per minigame. Section 14 raised that to eleven for
// dScMgPanel_c and said the question to ask is not "how many TUs name the
// table" but "how deep is the machine". This class answers SIXTEEN, and the
// depth is the whole of the reason:
//
//   level 1  data_ov006_02142df8   5 slots  arity 1  <- func_ov006_0211b954
//   level 1  data_ov006_02142e20  13 slots  arity 1  <- func_ov006_0211b5e0
//   level 2  each of 02142e20's THIRTEEN states is itself a 3-slot dispatcher
//            02142d38 02142d50 02142d80 02142dc8 02142de0 02142cd8 02142cf0
//            02142d20 02142d68 02142d98 02142cc0 02142d08 02142db0
//   plus     func_ov006_020c2b8c, the +0x4f38 sub-object's tick, which holds
//            its member pointer as an object FIELD rather than in a table
//
// 2 + 13 + 1 = 16, over FIFTEEN tables and FIFTY-SEVEN pairs.
//
// Both level-1 dispatchers are called DIRECTLY by vtable slot 6, which is a
// PLAIN SWITCH and not a dispatcher itself -- so the "read the slot-6 src
// before wiring it by name" rule (section 14) was obeyed and came back clean
// here, and the machine still had to be found one level down.
//
// ---- THE FIFTY-SEVEN PAIRS, AND HOW THE SLOT NUMBERS WERE GOT -------------
//
// From src/__sinit_ov006_02132970.c ONE ASSIGNMENT AT A TIME, which is the
// only ground truth for a slot number: an overlay constructor does NOT copy
// its pairs in address order, and this one interleaves fifteen destinations.
// A sweep of the source span would also have manufactured phantoms --
// MgBoomBox_SpawnInfo itself (0x0213f69c, two words) sits INSIDE the pair span
// 0x0213f4fc..0x0213f6dc, bracketed by pair 0x0213f694 and pair 0x0213f6a4.
//
// ALL FIFTY-SEVEN ADJUSTMENT WORDS READ ZERO and all fifty-seven code words
// are DISTINCT, and every one of them lands in 0x02119d50..0x0211bf44, inside
// this class's own code block (slot 16 at 0x02119904 to the factory at
// 0x0211cb70). That range check is what says these tables are this class's.
//
//   THE FIRST CONSTRUCTOR THIS LANE LANDED ON WAS THE WRONG CLASS'S, and it
//   is worth recording because the next lane will meet the same shape.
//   __sinit_ov006_02132f68 -- the block immediately after this one, and the
//   one a relocation scan over the data region ABOVE the vtable finds first --
//   builds four tables of 27 pairs whose code words all lie in
//   0x0211cd24..0x021200cc. That is dScMgTeresa_c's code (id 0x183, scene
//   387), not this class's. The pairs are physically parked between this
//   class's vtable and Teresa's type_info, so proximity says nothing; only the
//   code-word range does.
//
// ---- THE THREE PMF SHAPES ARE ALL PRESENT IN THIS ONE CLASS ---------------
//
// Section 4 names three ways a pointer-to-member dispatch can hide, and says
// the link finds one of them and a `::*` source sweep finds another and the
// third needs a run. This class has all three, which makes it the first seat
// that can state the trio from one closure:
//
//   1. OPEN-CODED IN PLAIN INTS -- invisible to BOTH prescribed detectors.
//      func_ov006_0211b954  table 02142df8
//      func_ov006_0211b5e0  table 02142e20
//      Both declare `struct Ent { int a; int b; }` inside extern "C" and do
//      their own `>> 1` / `& 1` decode. The ROM at 0x0211b984, one for one
//      with what the src spells:
//          ldr   r1,[r3,#4]        the adjustment
//          add   r0,r7,r1,asr #1   this, advanced, arithmetic shift
//          ands  r1,r1,#1          the virtual bit, in the LSB
//          ldrne r2,[r0] / ldrne r1,[r3] / ldrne r2,[r2,r1]
//          ldreq r2,[r3]
//          blx   r2
//      and `mov r1,r6` immediately before the blx is the loop counter, which
//      is what makes both tables arity ONE.
//
//   2. PMF-TYPED BEHIND extern "C" -- silent on the link, found by the sweep.
//      The twelve sub-dispatchers whose table extern sits inside the extern
//      "C" block, so the global mangles as the plain C name the ov006 mount
//      already defines and the linker is satisfied while MSVC strides an
//      eight-byte table by four.
//
//   3. PMF-TYPED AT C++ LINKAGE -- the only one a link can name.
//      src/func_ov006_0211a2c4.cpp declares `extern PMF data_ov006_02142cc0[];`
//      OUTSIDE its extern "C" block, so it is the class's single LNK2019.
//
// THE THIRTEEN SUB-DISPATCHERS ARE THE SAME NINETEEN INSTRUCTIONS THIRTEEN
// TIMES. Read word for word out of the image: 0x0211b590, 0x0211b398,
// 0x0211b17c, 0x0211af60, 0x0211ad44, 0x0211abdc, 0x0211aa44, 0x0211a7ac,
// 0x0211a648, 0x0211a4b0, 0x0211a2c4, 0x0211a0d8 and 0x02119eec are BYTE
// IDENTICAL over their first nineteen words and differ only in the twentieth,
// which is the literal-pool word holding their table address. So one shape is
// transcribed once below and applied thirteen times, and the check that this
// is legitimate is the byte comparison rather than a reading of the sources.
//
//     mla   r2,r1,#0x24,r0     the entity, stride 0x24
//     add   r2,r2,#0x5000
//     ldrb  r2,[r2,#0x1d1]     the SUB-state index, at entity +5
//     add   ip,r3,r2,lsl #3    an EIGHT-byte stride
//     ... the five-instruction Itanium sequence ...
//     blx   r2                 with r1 still holding the incoming index
//
// ---- ROUTE AT THE DISPATCH SITE, NEVER BY REWRITING THE STORED PAIR -------
//
// Section 4's rulebook, applied rather than restated. The tables' STRIDE is
// already right on both machines -- `struct MgPmf { unsigned code; int adj; }`
// is eight bytes here and eight bytes in the ROM -- so what is wrong is only
// that the code word is a DS address. The pairs keep the ROM's own words and
// the DECODE is what this file replaces. The ov085 / ov100 shape (write host
// addresses over the .data pairs before the constructor copies them) is wrong
// for the same reason section 4 gives for func_ov006_020c3d18, and for one
// more that is this class's own: the source pairs share their span with
// MgBoomBox_SpawnInfo, whose factory word the registry writes.
//
// ---- THE ONE HARD FLOOR IS RETIRED, run mg10 lane F367 --------------------
//
//   func_ov006_0211bc8c   slot 1 of data_ov006_02142df8, 0x2b8 bytes
//
// Run mg9 shipped this address as a REPORTING case with no symbol invented for
// it: config symbol, no delink block, no src file in either extension. It is
// now src/func_ov006_0211bc8c.cpp, a BYTE MATCH on mwccarm 2004/b56 with all
// fourteen relocation destinations checked against ov006's relocs.txt, and it
// has a delink block covering exactly 0x0211bc8c..0x0211bf44. The switch below
// routes it like the other fifty-five and the counter that used to say "asks
// for the floor" now says "entries into state 1".
//
// WHY IT MATTERED, kept because it is the reason the run law calls a
// trap-shaped floor worse than a missing one. An unattended 1200-frame boot
// asked for it ZERO times, which said only that it was not in the way of a
// BOOT. Driven -- SM64DS_TOUCH_PROBE="100-104:64:32,160-164:128:32" pokes two
// of the six pads, both records move from state 0 to state 1, the per-round tap
// counter at +0x5624 reaches its own early-out of 2 -- the SAME 300-frame run
// reported 338 asks where the untouched one reported none. Every one of those
// 338 was a pad tick that did nothing, so a tap changed the state byte and then
// the pad froze: no press animation, no note, no sound. That is the whole of
// what stood between this seat and a playable Boom Box.
//
// WHAT THE BODY DOES, so the next reader need not re-derive it. It runs the
// press animation on the per-step timer at +0x50f0 against the eight-entry
// frame table data_ov006_0212ef3c, stepping the sub-index at +0x50f7 from 0 to
// 6 and returning each time. On the step that would make it 7 it clamps back to
// 6 and COMMITS: the record moves to state 2 (func_ov006_0211bc68), the note
// index at +0x50f8 plus one is appended to the 16-bit play log at +0x5610 with
// the pad index packed into the high byte, the write cursor at +0x5625 advances,
// +0x5614 is seeded 0x20, func_ov006_0211b654 fills up to three of the THIRTY
// records with note entities -- which is what the second level-1 dispatcher then
// ticks and draws -- and one of three arm9 sound entry points fires on vtable
// slot 35 and the mode byte at +0x5627. The slot-35 call goes through a 36-slot
// shadow class, so the receiver arrives in r0; this class has already produced
// three receiver-drop defects and the byte match is what proves this is not a
// fourth. Finally, on the SECOND tap of a round (+0x5624 == 1) it calls
// func_ov006_020c2300 on the +0x4f38 sub-object.
//
// TWO SLICE LINES STOP BEING FREE. src/func_ov006_0211b654.c and
// src/func_ov006_020c2300.c were in port/slice_box.txt reachable only from
// 0x0211bd88 and 0x0211bf10, both inside this body, so /OPT:REF dropped them
// and run mg9 budgeted them as +0 on the headline. They now have a caller.
//
// ---- ONE STATE IS A JOIN MISS AND IS NOT A FLOOR --------------------------
//
//   func_ov006_0211ba88   slot 4 of data_ov006_02142df8, 0x158 bytes
//
// Also outside the delinks join, but src/func_ov006_0211ba88.c EXISTS,
// bannered NONMATCHING (register allocation, div=35). That is section 13's
// 0x020f6904 shape and the cost is a slice line plus a switch arm -- except
// that this one ALSO drops its receiver, so it is host-copied in
// port/unmatched/MgSound_ShadowSlot35.cpp rather than sliced. That file
// carries the evidence.

/* <cstdio> came BACK with run link100 lane PMFB2's boot installer, and for the
   reason the reporting case did not deserve: port_mg_sound_states_seat refuses
   to install over bytes the ROM's own pairs do not describe, and a refusal has
   to say which slot and what it found. Nothing on the per-frame path prints. */
#include <cstdio>
#include <cstdlib>


/* ---- THE ONE ALIAS THE THIRTEEN MATCHED TUs ASK FOR (lane PMFB3) --------
   Twelve of the thirteen sub-dispatchers declare their table INSIDE their own
   extern "C" block, so MSVC spells the reference _data_ov006_XXXXXXXX and the
   ov006 mount's own definition satisfies it. src/func_ov006_0211a2c4.cpp is the
   thirteenth and declares `extern PMF data_ov006_02142cc0[];` at namespace
   scope, which mangles as the name below -- read off that TU's own /FAsc
   listing (runs/link100/out/PMFB3/listings/func_ov006_0211a2c4_zp4.asm), not
   guessed. Same address, same bytes, only the decoration differs, and src/ is
   byte-locked so the source cannot be given the extern "C" instead.

   SAFE UNDER port/tools/alternatename_guard.py for hal/pmfc_aliases.cpp's
   reason: the LHS is a C++ mangling that only this one matched TU ever spells
   and nothing in the tree defines, so it cannot acquire a real definition and
   be silently defeated; the RHS is the mount's C-linkage symbol, and both land
   in the map at one address. */
#pragma comment(linker, "/alternatename:?data_ov006_02142cc0@@3PAP8C@@AEXH@ZA=_data_ov006_02142cc0")

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the fifteen state tables, re-typed ---------------------------------
   The ov006 mount defines the storage and __sinit_ov006_02132970 fills it.
   Each is spelled here as an array of two plain ints, which is eight bytes on
   MSVC and eight bytes in the ROM. */
extern MgPmf data_ov006_02142df8[];
extern MgPmf data_ov006_02142e20[];
extern MgPmf data_ov006_02142d38[];
extern MgPmf data_ov006_02142d50[];
extern MgPmf data_ov006_02142d80[];
extern MgPmf data_ov006_02142dc8[];
extern MgPmf data_ov006_02142de0[];
extern MgPmf data_ov006_02142cd8[];
extern MgPmf data_ov006_02142cf0[];
extern MgPmf data_ov006_02142d20[];
extern MgPmf data_ov006_02142d68[];
extern MgPmf data_ov006_02142d98[];
extern MgPmf data_ov006_02142cc0[];
extern MgPmf data_ov006_02142d08[];
extern MgPmf data_ov006_02142db0[];

/* ---- the fifty-seven reachable state bodies ------------------------------
   Reached ONLY through the switch below: the pair words are mounted DATA
   holding DS addresses, so nothing else in the build names them and /OPT:REF
   would drop every one of them without this file. The forty-one that are not
   defined here or in MgSound_ShadowSlot35.cpp are in port/slice_box.txt.

   FIFTY-SIX UNTIL RUN mg10 LANE F367. The fifty-seventh is 0x0211bc8c, which
   run mg9 could only report; it now has a matched body and a slice line and is
   declared and routed exactly like the rest.

   EVERY DECLARATION'S PARAMETER COUNT MATCHES ITS DEFINITION'S. The three
   spelled `(void)` below are empty bodies in src and are declared and called
   with no argument, which is the shape port/tools/aritycheck.py checks and the
   shape MgPanel_StateDispatch.cpp already uses for func_ov006_02104ec8. */
void func_ov006_0211bf44(char *c, int i);   /* 02142df8 p0 */
void func_ov006_0211bc8c(char *c, int i);   /* 02142df8 p1, was THE FLOOR */
void func_ov006_0211bc68(char *c, int i);   /* 02142df8 p2 */
void func_ov006_0211bbe0(char *c, int i);   /* 02142df8 p3 */
int  func_ov006_0211ba88(char *c, int i);   /* 02142df8 p4, host copy */
void func_ov006_0211b590(char *c, int i);   /* 02142e20 p0,  sub-dispatcher */
void func_ov006_0211b398(char *c, int i);   /* 02142e20 p1,  sub-dispatcher */
void func_ov006_0211b17c(char *c, int i);   /* 02142e20 p2,  sub-dispatcher */
void func_ov006_0211af60(char *c, int i);   /* 02142e20 p3,  sub-dispatcher */
void func_ov006_0211ad44(char *c, int i);   /* 02142e20 p4,  sub-dispatcher */
void func_ov006_0211abdc(char *c, int i);   /* 02142e20 p5,  sub-dispatcher */
void func_ov006_0211aa44(char *c, int i);   /* 02142e20 p6,  sub-dispatcher */
void func_ov006_0211a7ac(char *c, int i);   /* 02142e20 p7,  sub-dispatcher */
void func_ov006_0211a648(char *c, int i);   /* 02142e20 p8,  sub-dispatcher */
void func_ov006_0211a4b0(char *c, int i);   /* 02142e20 p9,  sub-dispatcher */
void func_ov006_0211a2c4(char *c, int i);   /* 02142e20 p10, sub-dispatcher */
void func_ov006_0211a0d8(char *c, int i);   /* 02142e20 p11, sub-dispatcher */
void func_ov006_02119eec(char *c, int i);   /* 02142e20 p12, sub-dispatcher */
void func_ov006_0211b4fc(char *c, int i);   /* 02142d38 p0 */
void func_ov006_0211b3ec(char *c, int i);   /* 02142d38 p1 */
void func_ov006_0211b3e8(void);             /* 02142d38 p2, empty body */
void func_ov006_0211b308(char *c, int i);   /* 02142d50 p0 */
void func_ov006_0211b278(char *c, int i);   /* 02142d50 p1 */
void func_ov006_0211b1cc(char *c, int i);   /* 02142d50 p2 */
void func_ov006_0211b0ec(char *c, int i);   /* 02142d80 p0 */
void func_ov006_0211b05c(char *c, int i);   /* 02142d80 p1 */
void func_ov006_0211afb0(char *c, int i);   /* 02142d80 p2 */
void func_ov006_0211aed0(char *c, int i);   /* 02142dc8 p0 */
void func_ov006_0211ae40(char *c, int i);   /* 02142dc8 p1 */
void func_ov006_0211ad94(char *c, int i);   /* 02142dc8 p2 */
void func_ov006_0211ad00(char *c, int i);   /* 02142de0 p0 */
void func_ov006_0211ac30(char *c, int i);   /* 02142de0 p1 */
void func_ov006_0211ac2c(void);             /* 02142de0 p2, empty body */
void func_ov006_0211ab80(char *c, int i);   /* 02142cd8 p0 */
void func_ov006_0211ab0c(char *c, int i);   /* 02142cd8 p1 */
void func_ov006_0211aa94(char *c, int i);   /* 02142cd8 p2 */
void func_ov006_0211a9fc(char *c, int i);   /* 02142cf0 p0 */
void func_ov006_0211a910(char *c, int i);   /* 02142cf0 p1 */
void func_ov006_0211a7fc(char *c, int i);   /* 02142cf0 p2 */
void func_ov006_0211a714(char *c, int i);   /* 02142d20 p0 */
void func_ov006_0211a69c(char *c, int i);   /* 02142d20 p1 */
void func_ov006_0211a698(void);             /* 02142d20 p2, empty body */
void func_ov006_0211a5ec(char *c, int i);   /* 02142d68 p0 */
void func_ov006_0211a578(char *c, int i);   /* 02142d68 p1 */
void func_ov006_0211a500(char *c, int i);   /* 02142d68 p2 */
void func_ov006_0211a420(char *c, int i);   /* 02142d98 p0 */
void func_ov006_0211a388(char *c, int i);   /* 02142d98 p1 */
void func_ov006_0211a314(char *c, int i);   /* 02142d98 p2 */
void func_ov006_0211a234(char *c, int i);   /* 02142cc0 p0 */
void func_ov006_0211a19c(char *c, int i);   /* 02142cc0 p1 */
void func_ov006_0211a128(char *c, int i);   /* 02142cc0 p2 */
void func_ov006_0211a048(char *c, int i);   /* 02142d08 p0 */
void func_ov006_02119fb0(char *c, int i);   /* 02142d08 p1 */
void func_ov006_02119f3c(char *c, int i);   /* 02142d08 p2 */
void func_ov006_02119e5c(char *c, int i);   /* 02142db0 p0 */
void func_ov006_02119dc4(char *c, int i);   /* 02142db0 p1 */
void func_ov006_02119d50(char *c, int i);   /* 02142db0 p2 */

/* THE SUB-OBJECT'S FIELD-HELD PMF DISPATCH MOVED OUT OF THIS FILE.
   func_ov006_020c2b8c belongs to the +0x4f38 sub-object, which dScMgCup_c
   mounts as well -- ov006 has exactly two arm_calls into it, 0x0211c960 (this
   class's slot 6) and 0x020e02dc (0x169's). Run mg9 landed both classes in one
   merge and the body is now defined once, in
   port/unmatched/MgShared4f38_SubDispatch.cpp, which carries the seven-pair
   derivation this file used to hold and routes SIX states rather than this
   lane's six or lane CUP's four -- the union, which is this lane's set. Its
   counters are port_mg_sub4f38_calls / _routed / _idle / _unknown and this
   file's port_mg_sound_counts reports them unchanged. */
unsigned port_mg_sub4f38_calls(void);
unsigned port_mg_sub4f38_routed(void);
unsigned port_mg_sub4f38_unknown(void);

/* what this file DEFINES, so the seat can name it. func_ov006_0211b954 and
   func_ov006_0211b5e0 ARE NOT HERE ANY MORE: run link100 lane PMFB2 retired
   both host copies onto their own matched TUs (port/slice_pmfb2.txt) and
   replaced the call-time address switch they needed with the boot installer
   port_mg_sound_states_seat below. */
void port_mg_sound_states_seat(void);
void port_mg_sound_counts(unsigned *hits, unsigned *floor, unsigned *unknown,
                          unsigned *field_calls, unsigned *field_routed,
                          unsigned *field_unknown);

}  /* extern "C" */

/* THE WITNESS. A hit is a state this class ROUTED TO A REAL BODY, counted in
   the level-2 faces below.

   WHAT RUN link100 LANE PMFB3 CHANGED IS THE MECHANISM, NOT THE MEANING. Until
   this lane the count came from a call-time switch over the code word
   (sound_try_1), which the thirteen host-copied sub-dispatchers called through
   port_mg_sound_call1. Both are gone: all FIFTEEN tables now hold host
   addresses that port_mg_sound_states_seat installed at boot, and the thirteen
   sub-dispatchers are the ROM's own matched TUs. The number this counter
   carries is exactly the number it has carried since lane PMFB2 -- the LEVEL-2
   dispatches, thirty-nine of the fifty-seven reachable states -- because the two
   level-1 tables have gone direct since that lane and never reached the switch
   either.

   THE SECOND COUNTER SAYS WHAT IT CLAIMS AGAIN. PMFB2's installer put
   func_ov006_0211bc8c straight into level-1 slot 1, so from that lane until this
   one the tapped-pad number could only read zero however hard the pads were
   driven, while hal/scene_mg_boombox.cpp went on printing it as the witness that
   a tap produced an execution. snd1_state1_0211bc8c below is a plain cdecl
   counting wrapper -- same convention, same two arguments, the same body called
   -- installed in that one slot instead of the bare body.

   THE THIRD COUNTER IS NOW STRUCTURALLY ZERO, said out loud rather than quietly
   enjoyed. An unknown code word was the number that convicted a missed
   dispatcher while a run-time switch decided what a word meant. Nothing decides
   at run time any more: port_mg_sound_states_seat compares all fifty-seven words
   against the ROM's own before it writes one of them and ABORTS on a mismatch,
   so that defect is now caught at boot, loudly, instead of being counted at
   frame time. */
static unsigned g_snd_hits;
static unsigned g_snd_state1_0211bc8c;
static unsigned g_snd_unknown;

/* ---- THE THIRTY-NINE LEVEL-2 FACES, run link100 lane PMFB3 ---------------
   WHY A FACE HERE AND NONE ON LEVEL 1. The two level-1 TUs open-code the mwcc
   member pointer as two plain ints and call it through
   `((void(*)(void*,int))fn)(obj,i)`, which MSVC emits as a cdecl call with the
   receiver PUSHED -- the shape the host bodies already have, so lane PMFB2
   installed them raw. The thirteen level-2 TUs spell a REAL pointer to member,
   `(((C *)o)->*data_ov006_XXXXXXXX[idx])(i)`, and MSVC emits that as

       push  ecx                                  the one argument, i
       mov   ecx, _data_ov006_XXXXXXXX[eax*8+4]   the adjustment word
       mov   eax, _data_ov006_XXXXXXXX[eax*8]     the code word
       add   ecx, edx                             this
       call  eax
       pop   ebp / ret 0                      NO add esp,4: the CALLEE cleans

   -- a __thiscall indirect: receiver in ecx, one argument on the stack, callee
   cleanup. A plain cdecl body reads its receiver off the stack and cleans
   nothing, so the code word could not hold one. __fastcall takes its first
   argument in ecx, ignores edx (which at the call still holds the same object,
   because the adjustment word is zero), and cleans the same four bytes.

   READ OFF EACH TU's OWN LISTING, not off one sample: all thirteen were
   compiled under the port's own flags with and without /Zp4
   (runs/link100/out/PMFB3/emit_subdisp.txt) and all thirteen emit an operand
   scale of EIGHT against the ROM's `add ip, r3, r2, lsl #3`, with the listings
   byte-identical either way -- these records are bare pointer-to-member arrays,
   not the struct CONTAINING a pointer to member that forced lane PMFB1's /Zp4,
   so the option is not claimed on any of them.

   EVERY FACE COUNTS. A face is reachable only from a table word this file's own
   seat wrote, so the count is exactly the dispatches that happened. */
#define SND2_FACE(sym)                                                        \
    static void __fastcall snd2_##sym(void *self, void *dead_edx, int i)      \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_snd_hits;                                                         \
        sym((char *)self, i);                                                 \
    }

/* the three states whose ROM body takes nothing and whose src file is an empty
   body; declared and called with no argument, which is the shape
   port/tools/aritycheck.py checks. */
#define SND2_FACE0(sym)                                                       \
    static void __fastcall snd2_##sym(void *self, void *dead_edx, int i)      \
    {                                                                         \
        (void)self; (void)dead_edx; (void)i;                                  \
        ++g_snd_hits;                                                         \
        sym();                                                                \
    }

/* data_ov006_02142d38 */
SND2_FACE(func_ov006_0211b4fc)
SND2_FACE(func_ov006_0211b3ec)
SND2_FACE0(func_ov006_0211b3e8)
/* data_ov006_02142d50 */
SND2_FACE(func_ov006_0211b308)
SND2_FACE(func_ov006_0211b278)
SND2_FACE(func_ov006_0211b1cc)
/* data_ov006_02142d80 */
SND2_FACE(func_ov006_0211b0ec)
SND2_FACE(func_ov006_0211b05c)
SND2_FACE(func_ov006_0211afb0)
/* data_ov006_02142dc8 */
SND2_FACE(func_ov006_0211aed0)
SND2_FACE(func_ov006_0211ae40)
SND2_FACE(func_ov006_0211ad94)
/* data_ov006_02142de0 */
SND2_FACE(func_ov006_0211ad00)
SND2_FACE(func_ov006_0211ac30)
SND2_FACE0(func_ov006_0211ac2c)
/* data_ov006_02142cd8 */
SND2_FACE(func_ov006_0211ab80)
SND2_FACE(func_ov006_0211ab0c)
SND2_FACE(func_ov006_0211aa94)
/* data_ov006_02142cf0 */
SND2_FACE(func_ov006_0211a9fc)
SND2_FACE(func_ov006_0211a910)
SND2_FACE(func_ov006_0211a7fc)
/* data_ov006_02142d20 */
SND2_FACE(func_ov006_0211a714)
SND2_FACE(func_ov006_0211a69c)
SND2_FACE0(func_ov006_0211a698)
/* data_ov006_02142d68 */
SND2_FACE(func_ov006_0211a5ec)
SND2_FACE(func_ov006_0211a578)
SND2_FACE(func_ov006_0211a500)
/* data_ov006_02142d98 */
SND2_FACE(func_ov006_0211a420)
SND2_FACE(func_ov006_0211a388)
SND2_FACE(func_ov006_0211a314)
/* data_ov006_02142cc0 */
SND2_FACE(func_ov006_0211a234)
SND2_FACE(func_ov006_0211a19c)
SND2_FACE(func_ov006_0211a128)
/* data_ov006_02142d08 */
SND2_FACE(func_ov006_0211a048)
SND2_FACE(func_ov006_02119fb0)
SND2_FACE(func_ov006_02119f3c)
/* data_ov006_02142db0 */
SND2_FACE(func_ov006_02119e5c)
SND2_FACE(func_ov006_02119dc4)
SND2_FACE(func_ov006_02119d50)

#undef SND2_FACE
#undef SND2_FACE0

/* The one level-1 wrapper, and the only reason it exists is the counter above.
   cdecl in and cdecl out, both arguments forwarded, the same body called. */
static void snd1_state1_0211bc8c(char *c, int i)
{
    ++g_snd_state1_0211bc8c;
    func_ov006_0211bc8c(c, i);
}

/* ---- THE BOOT INSTALLER, lanes PMFB2 (level 1) and PMFB3 (level 2) -------
   WHAT THIS REPLACES. Until lane PMFB2 the two LEVEL-1 tables were reached only
   through a call-time address switch: the host copies of func_ov006_0211b954
   and func_ov006_0211b5e0 read the DS code word the sinit left in place and a
   switch turned it into a host body by name. That shape cannot retire, because
   retiring the host copy takes the switch with it and the matched TU would call
   a DS address. Turning the switch inside out fixes it once: install HOST
   addresses into the eighteen level-1 slots at boot, and the ROM's own two
   dispatchers -- which read the pair, decode it and call it with a plain cdecl
   `call eax` -- reach the same host bodies with no switch at all.

   LANE PMFB3 DID THE SAME ONE LEVEL DOWN, and the switch is now gone entirely.
   The THIRTEEN level-2 tables each sub-dispatcher reads held DS words and still
   went through it; their thirty-nine slots are installed below, their thirteen
   host copies are retired onto the ROM's own matched TUs, and nothing in this
   class decides at run time what a code word means any more.

   WHY THIS IS THE ov085 SHAPE AND NOT THE ONE THE HEADER REFUSES. The header
   above rules out writing host addresses over the .data SOURCE pairs, because
   this class's source span is shared with MgBoomBox_SpawnInfo. This installer
   does not touch the source pairs. It writes the DESTINATION storage, after
   __sinit_ov006_02132970 has copied into it, which is exactly what
   port_pushblock_states_seat does to data_ov002_021097bc, and it is called from
   the one place in the tree that runs those constructors (hal/scene_mg.cpp).

   THE EIGHTEEN PAIRS, RE-READ FOR THIS LANE out of
   extracted/overlays/overlay_0006.bin at the addresses
   src/__sinit_ov006_02132970.c assigns from, not out of prose
   (runs/link100/out/PMFB2/rom_pairs.txt):
     02142df8 <- 0213f594 0213f694 0213f6a4 0213f634 0213f64c
     02142e20 <- 0213f6c4 0213f5b4 0213f63c 0213f5c4 0213f5a4 0213f5ac
                 0213f62c 0213f554 0213f5e4 0213f55c 0213f53c 0213f564
                 0213f604
   All eighteen adjustment words read ZERO, so `this` is unadjusted and the ROM
   takes the `ldreq` arm: the code word is called directly. Both dispatchers
   stride the table by EIGHT (`add r3, r4, r0, lsl #3` at 0x0211b96c and
   0x0211b60c), and the matched TUs emit `[ebx*8]` with and without /Zp4 -- the
   record here is two plain ints, not the twenty-byte struct-containing-a-
   pointer-to-member that lane PMFB1 had to pack.

   NO FACE IS NEEDED ON THIS TABLE, unlike the family-C call-shape rows. Both
   matched TUs open-code the pair as two ints and dispatch
   `((void(*)(void*,int))fn)(obj,i)`, which MSVC emits as `push esi / push edx /
   call eax / add esp,8`: a plain cdecl call with the receiver PUSHED. That is
   the signature the eighteen host bodies already have.

   THE CHECK IS THE POINT. Every slot is compared against the ROM's own code
   word and against a zero adjust word before anything is written, and either
   mismatch is a loud abort rather than a silent wrong dispatch -- the
   port_pushblock_states_seat rule. */
extern "C" void port_mg_sound_states_seat(void)
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
        void (*host)(char *, int);
    } seats[] = {
        /* data_ov006_02142df8, 5 slots, read by func_ov006_0211b954 */
        {data_ov006_02142df8, "02142df8",  0, 0x0211bf44u, func_ov006_0211bf44},
        {data_ov006_02142df8, "02142df8",  1, 0x0211bc8cu, snd1_state1_0211bc8c},
        {data_ov006_02142df8, "02142df8",  2, 0x0211bc68u, func_ov006_0211bc68},
        {data_ov006_02142df8, "02142df8",  3, 0x0211bbe0u, func_ov006_0211bbe0},
        /* the one state that returns a value; cdecl leaves eax to the caller,
           and the ROM's own dispatch ignores it exactly as this cast does. */
        {data_ov006_02142df8, "02142df8",  4, 0x0211ba88u,
                                    (void (*)(char *, int))func_ov006_0211ba88},
        /* data_ov006_02142e20, 13 slots, read by func_ov006_0211b5e0. Every
           one of the thirteen is itself a sub-dispatcher, still host-copied
           below, still reading a level-2 table full of DS words. */
        {data_ov006_02142e20, "02142e20",  0, 0x0211b590u, func_ov006_0211b590},
        {data_ov006_02142e20, "02142e20",  1, 0x0211b398u, func_ov006_0211b398},
        {data_ov006_02142e20, "02142e20",  2, 0x0211b17cu, func_ov006_0211b17c},
        {data_ov006_02142e20, "02142e20",  3, 0x0211af60u, func_ov006_0211af60},
        {data_ov006_02142e20, "02142e20",  4, 0x0211ad44u, func_ov006_0211ad44},
        {data_ov006_02142e20, "02142e20",  5, 0x0211abdcu, func_ov006_0211abdc},
        {data_ov006_02142e20, "02142e20",  6, 0x0211aa44u, func_ov006_0211aa44},
        {data_ov006_02142e20, "02142e20",  7, 0x0211a7acu, func_ov006_0211a7ac},
        {data_ov006_02142e20, "02142e20",  8, 0x0211a648u, func_ov006_0211a648},
        {data_ov006_02142e20, "02142e20",  9, 0x0211a4b0u, func_ov006_0211a4b0},
        {data_ov006_02142e20, "02142e20", 10, 0x0211a2c4u, func_ov006_0211a2c4},
        {data_ov006_02142e20, "02142e20", 11, 0x0211a0d8u, func_ov006_0211a0d8},
        {data_ov006_02142e20, "02142e20", 12, 0x02119eecu, func_ov006_02119eec},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        MgPmf *p = &seats[i].table[seats[i].slot];
        if (p->code != seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgSound_c state table %s slot %u: "
                         "the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].slot, p->code, p->adj, seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats[i].host;
    }

    /* ---- THE THIRTEEN LEVEL-2 TABLES, run link100 lane PMFB3 -------------
       The thirty-nine slots the thirteen sub-dispatchers read. Every row below
       carries the code word this lane read out of
       extracted/overlays/overlay_0006.bin at the address
       src/__sinit_ov006_02132970.c assigns that slot from -- not out of prose,
       and not out of the call-time switch this block replaced. All thirty-nine
       adjustment words read ZERO, all thirty-nine code words are DISTINCT, and
       every one lands inside this class's own code block
       0x02119904..0x0211cb70, which is the check that says these tables are
       this class's (runs/link100/out/PMFB3/romstride_pmfb3.txt).

       THE SLOT NUMBERS COME FROM THE SINIT'S OWN ASSIGNMENTS, one at a time,
       for the reason the head of this file gives: an overlay constructor does
       not copy its pairs in address order and this one interleaves fifteen
       destinations. All thirty-nine are WHOLE-PAIR copies
       (`data_ov006_TAB[i] = data_ov006_SRC;`); there is not one field-form fill
       among them, which is what lets this installer prove what it writes.

       THE VALUE INSTALLED IS A FACE, not the body: the thirteen matched TUs
       dispatch with the receiver in ecx and the argument on the stack under
       callee cleanup. */
    static const struct {
        MgPmf *table;
        const char *name;
        unsigned slot;
        unsigned rom;
        void (__fastcall *face)(void *, void *, int);
    } seats2[] = {
        /* data_ov006_02142d38 */
        {data_ov006_02142d38, "02142d38", 0, 0x0211b4fcu, snd2_func_ov006_0211b4fc},
        {data_ov006_02142d38, "02142d38", 1, 0x0211b3ecu, snd2_func_ov006_0211b3ec},
        {data_ov006_02142d38, "02142d38", 2, 0x0211b3e8u, snd2_func_ov006_0211b3e8},
        /* data_ov006_02142d50 */
        {data_ov006_02142d50, "02142d50", 0, 0x0211b308u, snd2_func_ov006_0211b308},
        {data_ov006_02142d50, "02142d50", 1, 0x0211b278u, snd2_func_ov006_0211b278},
        {data_ov006_02142d50, "02142d50", 2, 0x0211b1ccu, snd2_func_ov006_0211b1cc},
        /* data_ov006_02142d80 */
        {data_ov006_02142d80, "02142d80", 0, 0x0211b0ecu, snd2_func_ov006_0211b0ec},
        {data_ov006_02142d80, "02142d80", 1, 0x0211b05cu, snd2_func_ov006_0211b05c},
        {data_ov006_02142d80, "02142d80", 2, 0x0211afb0u, snd2_func_ov006_0211afb0},
        /* data_ov006_02142dc8 */
        {data_ov006_02142dc8, "02142dc8", 0, 0x0211aed0u, snd2_func_ov006_0211aed0},
        {data_ov006_02142dc8, "02142dc8", 1, 0x0211ae40u, snd2_func_ov006_0211ae40},
        {data_ov006_02142dc8, "02142dc8", 2, 0x0211ad94u, snd2_func_ov006_0211ad94},
        /* data_ov006_02142de0 */
        {data_ov006_02142de0, "02142de0", 0, 0x0211ad00u, snd2_func_ov006_0211ad00},
        {data_ov006_02142de0, "02142de0", 1, 0x0211ac30u, snd2_func_ov006_0211ac30},
        {data_ov006_02142de0, "02142de0", 2, 0x0211ac2cu, snd2_func_ov006_0211ac2c},
        /* data_ov006_02142cd8 */
        {data_ov006_02142cd8, "02142cd8", 0, 0x0211ab80u, snd2_func_ov006_0211ab80},
        {data_ov006_02142cd8, "02142cd8", 1, 0x0211ab0cu, snd2_func_ov006_0211ab0c},
        {data_ov006_02142cd8, "02142cd8", 2, 0x0211aa94u, snd2_func_ov006_0211aa94},
        /* data_ov006_02142cf0 */
        {data_ov006_02142cf0, "02142cf0", 0, 0x0211a9fcu, snd2_func_ov006_0211a9fc},
        {data_ov006_02142cf0, "02142cf0", 1, 0x0211a910u, snd2_func_ov006_0211a910},
        {data_ov006_02142cf0, "02142cf0", 2, 0x0211a7fcu, snd2_func_ov006_0211a7fc},
        /* data_ov006_02142d20 */
        {data_ov006_02142d20, "02142d20", 0, 0x0211a714u, snd2_func_ov006_0211a714},
        {data_ov006_02142d20, "02142d20", 1, 0x0211a69cu, snd2_func_ov006_0211a69c},
        {data_ov006_02142d20, "02142d20", 2, 0x0211a698u, snd2_func_ov006_0211a698},
        /* data_ov006_02142d68 */
        {data_ov006_02142d68, "02142d68", 0, 0x0211a5ecu, snd2_func_ov006_0211a5ec},
        {data_ov006_02142d68, "02142d68", 1, 0x0211a578u, snd2_func_ov006_0211a578},
        {data_ov006_02142d68, "02142d68", 2, 0x0211a500u, snd2_func_ov006_0211a500},
        /* data_ov006_02142d98 */
        {data_ov006_02142d98, "02142d98", 0, 0x0211a420u, snd2_func_ov006_0211a420},
        {data_ov006_02142d98, "02142d98", 1, 0x0211a388u, snd2_func_ov006_0211a388},
        {data_ov006_02142d98, "02142d98", 2, 0x0211a314u, snd2_func_ov006_0211a314},
        /* data_ov006_02142cc0 */
        {data_ov006_02142cc0, "02142cc0", 0, 0x0211a234u, snd2_func_ov006_0211a234},
        {data_ov006_02142cc0, "02142cc0", 1, 0x0211a19cu, snd2_func_ov006_0211a19c},
        {data_ov006_02142cc0, "02142cc0", 2, 0x0211a128u, snd2_func_ov006_0211a128},
        /* data_ov006_02142d08 */
        {data_ov006_02142d08, "02142d08", 0, 0x0211a048u, snd2_func_ov006_0211a048},
        {data_ov006_02142d08, "02142d08", 1, 0x02119fb0u, snd2_func_ov006_02119fb0},
        {data_ov006_02142d08, "02142d08", 2, 0x02119f3cu, snd2_func_ov006_02119f3c},
        /* data_ov006_02142db0 */
        {data_ov006_02142db0, "02142db0", 0, 0x02119e5cu, snd2_func_ov006_02119e5c},
        {data_ov006_02142db0, "02142db0", 1, 0x02119dc4u, snd2_func_ov006_02119dc4},
        {data_ov006_02142db0, "02142db0", 2, 0x02119d50u, snd2_func_ov006_02119d50},
    };

    for (unsigned i = 0; i < sizeof seats2 / sizeof seats2[0]; ++i) {
        MgPmf *p = &seats2[i].table[seats2[i].slot];
        if (p->code != seats2[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgSound_c state table %s slot %u: "
                         "the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats2[i].name,
                         seats2[i].slot, p->code, p->adj, seats2[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats2[i].face;
    }
}


/* THE SECOND PARAMETER IS STILL SPELLED `floor` SO THE SEAT'S CALL AND EVERY
   OTHER CALLER'S KEEP THE SAME ABI ACROSS THIS CHANGE -- a six-argument
   signature that changed arity here would be exactly the cross-TU drift
   port/tools/aritycheck.py exists to catch. What it CARRIES changed: it is now
   the number of entries into state 1, which run mg10 lane F367 decompiled. */
extern "C" void port_mg_sound_counts(unsigned *hits, unsigned *floor,
                                     unsigned *unknown, unsigned *field_calls,
                                     unsigned *field_routed,
                                     unsigned *field_unknown)
{
    if (hits)          *hits          = g_snd_hits;
    if (floor)         *floor         = g_snd_state1_0211bc8c;
    if (unknown)       *unknown       = g_snd_unknown;
    /* The FIELD counts come from the shared host copy now. The mapping is
       exact: its `calls` counts every entry as this file's did, its `routed`
       counts the same six states, and its `unknown` counts the same misses.
       Its fourth counter, `idle`, has no field in this census and reads zero on
       every measured run of this scene. */
    if (field_calls)   *field_calls   = port_mg_sub4f38_calls();
    if (field_routed)  *field_routed  = port_mg_sub4f38_routed();
    if (field_unknown) *field_unknown = port_mg_sub4f38_unknown();
}

// ---- NO HOST COPY OF A dScMgSound_c DISPATCHER IS LEFT --------------------
//
// SIXTEEN at run mg9, THIRTEEN after run link100 lane PMFB2 retired the two
// level-1 dispatchers, and NONE after lane PMFB3. The thirteen level-2
// sub-dispatchers are now src/func_ov006_0211b590.cpp and its twelve siblings,
// on port/slice_pmfb3.txt, and the boot installer above puts a host face in
// each of their thirty-nine table words, so the ROM's own code runs the ROM's
// own dispatch.
//
// THE THIRTEEN ROM BODIES ARE ONE SHAPE, and that is a measurement rather than
// a convenience: read word for word out of extracted/overlays/overlay_0006.bin
// for this lane, 0x0211b590, 0x0211b398, 0x0211b17c, 0x0211af60, 0x0211ad44,
// 0x0211abdc, 0x0211aa44, 0x0211a7ac, 0x0211a648, 0x0211a4b0, 0x0211a2c4,
// 0x0211a0d8 and 0x02119eec are BYTE IDENTICAL over their first nineteen words
// and differ only in the twentieth, the literal-pool word that names their
// table -- and all thirteen of those pool words agree with the destination
// __sinit_ov006_02132970 fills (runs/link100/out/PMFB3/pool_words.txt).
//
//     mov   r2,#0x24 / mla r2,r1,r2,r0   the entity, stride 0x24
//     add   r2,r2,#0x5000
//     ldrb  r2,[r2,#0x1d1]               the SUB-state index, at entity +5
//     add   ip,r3,r2,lsl #3              an EIGHT-byte record stride
//     ldr   r2,[ip,#4] / add r0,r0,r2,asr #1 / ands r2,r2,#1
//     ldrne r3,[r0] / ldrne r2,[ip] / ldrne r2,[r3,r2] / ldreq r2,[ip]
//     blx   r2                           with r1 still holding the index
//
// The two level-1 readings are kept because hal/scene_mg_boombox.cpp's entity
// printouts are derived from them:
//   0x0211b954, 0x74: TEN records at stride 0x14 off `this`, the in-play gate
//   at +0x50f5 and the state index at +0x50f4, `add r3,r4,r0,lsl #3` on the
//   pool word 0x0211b9c4 = 02142df8, `mov r1,r6` (the loop counter) into the
//   argument register, and `this` taken from r7 -- the ORIGINAL object, not the
//   walking entity.
//   0x0211b5e0, 0x74: THIRTY records at stride 0x24, gate at +0x51cc, index at
//   +0x51d0, pool word 0x0211b650 = 02142e20, same `mov r1,r6`, same r7.

/* func_ov006_020c2b8c is NOT defined here any more -- see the note above the
   declarations at the head of this file. */
