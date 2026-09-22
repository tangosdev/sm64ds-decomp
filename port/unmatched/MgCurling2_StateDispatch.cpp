/* PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, dScMgCurling2_c's half.
 * Actor id 0x177, scene 375. Run mg6, lane S75.
 *
 * Read unmatched/MgBase_StateDispatch.cpp's header first for the ROM
 * disassembly of the dispatch sequence, the finding that the overlay
 * constructors copy every pair correctly at the right stride, and the rulebook
 * corollary about spellings. Read unmatched/MgFlower_SubDispatch.cpp for the
 * open-coded-in-ints shape. Neither is repeated here.
 *
 * ---- THIS CLASS CARRIES BOTH KINDS OF THE WALL AT ONCE --------------------
 *
 * port/mg_fanout_costs.txt section 4 enumerates three ways a dispatcher hides:
 * it names a member-pointer type and the link fails on the global; it names one
 * inside extern "C" so the global mangles as the plain C name the mount already
 * defines and the link is satisfied; or it names no member-pointer type at all
 * and open-codes the Itanium sequence in plain ints, which neither the link nor
 * a `::*` sweep can see. dScMgCurling2_c has FIVE dispatching TUs and they
 * cover all three:
 *
 *   TU                    tables                    how it hides
 *   -------------------   -----------------------   ------------------------
 *   func_ov006_020e4800   02141988                  nothing: PMF typedef at
 *                                                   C++ linkage, the link
 *                                                   names the global
 *   _ZN15dScMgCurling2_c9StepXAndYEi   021419f8, 021419a0        same, two globals
 *   func_ov006_020e6354   02141978, 021419d8        PMF typedefs INSIDE
 *                                                   extern "C" -- silent
 *   _ZN15dScMgCurling2_c9StepXOnlyEi   021419b8                  same -- silent
 *   _ZN15dScMgCurling2_c8BehaviorEv   02141a18                  NO member-pointer type at
 *                                                   all, the pair read as
 *                                                   `struct Ent{int a,b;}` and
 *                                                   the five-instruction
 *                                                   sequence written out by
 *                                                   hand -- silent to BOTH
 *                                                   detectors
 *
 * ALL FIVE ARE HOST-COPIED, and the boundary is the spelling and not the
 * symptom. Section 4's corollary is "a pair whose consumer NAMES a
 * member-pointer type needs a host copy, whether it is called or only copied",
 * and being invisible to a link does not make a wrong stride right: MSVC's
 * single-inheritance member pointer is four bytes where the ROM's is eight, so
 * `data[idx]` walks half a record per index in all four of the typed TUs
 * regardless of whether the linker had anything to say. 020e683c is the one
 * whose STRIDE is already correct -- two ints is eight bytes on both machines
 * -- and it is still copied, because it CALLS the raw DS code word.
 *
 * ---- THE TWENTY-FIVE ADDRESSES, READ OUT OF THE ROM -----------------------
 *
 * Not guessed and not read from a running table. src/__sinit_ov006_02130758.c
 * is this class's overlay constructor and it NAMES all twenty-five source
 * pairs, one assignment at a time; each was then read out of
 * extracted/overlays/overlay_0006.bin at base 0x020bfec0. The range those pairs
 * occupy is 0x0213c3d4..0x0213c4b4 and it BRACKETS this class's SpawnInfo at
 * 0x0213c434 -- which is exactly the trap section 4 point 2 records for
 * curling, so the pairs are taken from the constructor and never from a sweep
 * of the range.
 *
 *   table                n  arity  slot addresses in slot order
 *   -------------------  -  -----  -------------------------------------------
 *   data_ov006_02141978  2    0    020e4ed4  020e4bd4(FLOOR)
 *   data_ov006_02141988  3    1    020e4794  020e4744  020e42b4
 *   data_ov006_021419a0  3    1    020e3db4  020e3ce0  020e3c4c
 *   data_ov006_021419b8  4    1    020e4630  020e4520  020e440c  020e4348
 *   data_ov006_021419d8  4    1    020e5e3c  020e5b7c  020e5b70  020e5a0c
 *   data_ov006_021419f8  4    1    020e41d0  020e4094  020e3f54  020e3e4c
 *   data_ov006_02141a18  5    0    020e64e4  020e6354  020e628c  020e61c4
 *                                  020e61c0
 *
 * EVERY ONE OF THE TWENTY-FIVE ADJUSTMENT WORDS IS ZERO, re-measured here
 * rather than inherited, so `v >> 1` is 0, the advanced `this` is the object
 * itself, and the virtual branch is never taken. The virtual arm is REPORTED
 * rather than implemented, for MgBase_StateDispatch's reason: a dispatch shape
 * nobody has measured is exactly the plausible body
 * port/tools/inferred_stub_guard exists to refuse.
 *
 * ---- THE ARITY COLUMN IS THE PART A LATER LANE MUST NOT COPY FROM stategen -
 *
 * port/tools/stategen.py REFUSES this constructor, and the refusal is correct:
 * "no src TU declares data_ov006_02141a18 as a typed member-pointer array",
 * because 020e683c spells the pair as two ints. Passing --arity 0 to get past
 * it yields the right twenty-five ADDRESSES and the WRONG arity for five of the
 * seven tables. Nineteen of the twenty-five sites take an index argument and a
 * switch generated from that output would have dropped it at every one.
 *
 * The arities above are read off the ROM. _ZN15dScMgCurling2_c9StepXAndYEi sets `mov r1,r5`
 * -- the loop index -- immediately before both of its `blx r2`;
 * _ZN15dScMgCurling2_c9StepXOnlyEi never touches r1 after entry, so the incoming index rides
 * through to the callee. They then agree with the matched src signature of all
 * twenty-four bodies that have one: every body in a 0-arity table takes the
 * receiver alone (or nothing), and every body in a 1-arity table takes
 * (receiver, index). Two independent derivations, no disagreement.
 *
 * ---- THE FLOOR IS GONE ----------------------------------------------------
 *
 * 0x020e4bd4, slot 1 of data_ov006_02141978, was this class's analogue of
 * curling's func_ov006_020e1854: a config symbol with no delink block and no
 * src, reported here rather than called. Run mg7 lane L375 decompiled it
 * (src/actors/dScMgCurling2_c.cpp, NONMATCHING-bannered, size exact, register
 * colouring only) together with func_ov006_020e513c and func_ov006_020e5450, so
 * all twenty-five of this class's states now have a body and the address
 * switches below have no reported case left.
 *
 * ---- THE STORED WORDS ARE DELIBERATELY NOT REWRITTEN ----------------------
 *
 * The alternative is the ov085/ov100 port_*_states_seat shape: write host
 * addresses over the pairs before the constructor copies them. It is not taken,
 * for MgFlower_SubDispatch's reason and one of this class's own: the pairs live
 * in the mounted ov006 .data at 0x0213c3d4..0x0213c4b4, three of whose
 * neighbours in that range are NOT pairs at all -- 0x0213c434 is the SpawnInfo
 * this class is registered by -- so a writer over that range is a writer over
 * live records the registry reads. Routing at the dispatch site leaves every
 * reader of every word correct and touches nothing but the five calls.
 *
 * ---- WHY IT DOES NOT ROUTE THROUGH port_mg_call0 --------------------------
 *
 * MgBase_StateDispatch.cpp's port_mg_call0 is the tidier shape and its ov006
 * arm, port_mg_try_ov006_0, is DEFINED in unmatched/MgCurling_StateDispatch.cpp
 * -- another class's file -- so a second class cannot add cases to it without
 * editing it. port/slice_mga361.txt flagged that singleton, lane FLW hit it
 * second, and this is the third. The refactor is not this lane's to take across
 * two other lanes' files, so this file carries its own switches and its own
 * adjustment ruling, spelled the same way, and reports its own counts.
 *
 * IT DEFINES ALL FIVE DISPATCHERS, so all five src TUs are OUT of
 * port/slice_s75.txt. That is five linked functions this seat trades for a
 * working state machine, which is the same trade MG2 made for curling.
 */

#include <cstdio>
#include <cstdlib>   /* std::abort, for the boot installer below (lane PMFB5) */

extern "C" {

/* The seven state tables, in the mounted ov006 .bss the constructor fills.
   Spelled as a pair of ints rather than as a member pointer, which is what
   src/__sinit_ov006_02130758.c itself spells and is why the mount's copy has
   been at the right stride all along: eight bytes on MSVC is eight bytes in
   the ROM. */
struct MgC2Pair { int code; int adj; };

extern MgC2Pair data_ov006_02141978[];   /* 2, arity 0 */
extern MgC2Pair data_ov006_02141988[];   /* 3, arity 1 */
extern MgC2Pair data_ov006_021419a0[];   /* 3, arity 1 */
extern MgC2Pair data_ov006_021419b8[];   /* 4, arity 1 */
extern MgC2Pair data_ov006_021419d8[];   /* 4, arity 1 */
extern MgC2Pair data_ov006_021419f8[];   /* 4, arity 1 */
extern MgC2Pair data_ov006_02141a18[];   /* 5, arity 0 */

/* the 0-arity state bodies */
void func_ov006_020e64e4(int c);
void func_ov006_020e628c(char *c);
void func_ov006_020e61c4(char *c);
void func_ov006_020e61c0(void);
void _ZN15dScMgCurling2_c9DragBeginEv(char *c);
void _ZN15dScMgCurling2_c10DragUpdateEv(char *c);

/* the 1-arity state bodies */
void func_ov006_020e5e3c(char *c, int i);
void func_ov006_020e5b7c(char *c, int i);
void func_ov006_020e5b70(char *c, int i);
void func_ov006_020e5a0c(char *c, int i);
void _ZN15dScMgCurling2_c12PickStepModeEi(char *c, int i);
void _ZN15dScMgCurling2_c13StepXPickFastEi(char *c, int i);
void _ZN15dScMgCurling2_c16StepXPushPosFastEi(char *c, int i);
void _ZN15dScMgCurling2_c16StepXPushNegFastEi(char *c, int i);
void _ZN15dScMgCurling2_c15StepXSettleFastEi(char *c, int i);
void _ZN15dScMgCurling2_c9StepXPickEi(char *c, int i);
void _ZN15dScMgCurling2_c12StepXPushPosEi(char *c, int i);
void _ZN15dScMgCurling2_c12StepXPushNegEi(char *c, int i);
void _ZN15dScMgCurling2_c11StepXSettleEi(char *c, int i);
void _ZN15dScMgCurling2_c12StepYRestartEi(char *c, int i);
void _ZN15dScMgCurling2_c11StepYRampUpEi(char *c, int i);
void _ZN15dScMgCurling2_c13StepYRampDownEi(char *c, int i);

/* the rest of what the five dispatchers call, verbatim from src */
int  RandomIntInternal(int *seed);
extern int data_0209d4b8;                /* src spells it at C++ linkage; a
                                            host copy has no reason to, and
                                            spelling it here saves an alias */
void func_ov006_020e3948(char *c);

/* the five this file defines */
void func_ov006_020e6354(char *c);
void func_ov006_020e4800(char *o);
void _ZN15dScMgCurling2_c9StepXAndYEi(char *c, int i);
void _ZN15dScMgCurling2_c9StepXOnlyEi(char *o, int i);
int  _ZN15dScMgCurling2_c8BehaviorEv(char *c);

unsigned port_mg_curling2_state_calls(void);
unsigned port_mg_curling2_state_unknown(void);
unsigned port_mg_curling2_state_floor(void);
unsigned port_mg_curling2_state_4bd4(void);
unsigned port_mg_curling2_d8_hits(int slot);
/* the boot installer in this file's 02141988 section; hal/scene_mg.cpp calls it
   after the ov006 constructors have filled the tables (lane PMFB5). */
void port_mg_curling2_states_seat(void);

}  /* extern "C" */

static unsigned g_calls, g_unknown, g_floor;

/* THE ROUND-END MACHINE'S OWN CENSUS. Table data_ov006_021419d8's four states
   are the ones run mg7 lane L375 unblocked, so they are counted per SLOT rather
   than folded into g_calls: the four addresses in slot order are 020e5e3c,
   020e5b7c, 020e5b70 (the veneer onto 020e5450) and 020e5a0c, and a run where
   the machine advances shows all four moving. They are counted HERE and not in
   the bodies on purpose -- the bodies are decomp transcriptions, and a port
   counter inside one would be a port edit to ROM code. */
static unsigned g_d8[4];
static unsigned g_4bd4;

extern "C" unsigned port_mg_curling2_state_calls(void)   { return g_calls; }
extern "C" unsigned port_mg_curling2_state_unknown(void) { return g_unknown; }
extern "C" unsigned port_mg_curling2_state_floor(void)   { return g_floor; }
extern "C" unsigned port_mg_curling2_state_4bd4(void)    { return g_4bd4; }
extern "C" unsigned port_mg_curling2_d8_hits(int slot)
{
    return (slot >= 0 && slot < 4) ? g_d8[slot] : 0u;
}

static void c2_unhandled(const char *why, unsigned code, int adj)
{
    static int said;
    ++g_unknown;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [scene] dScMgCurling2_c STATE %s: code %08x "
                     "adj %d. No state was run. "
                     "port/unmatched/MgCurling2_StateDispatch.cpp\n",
                     why, code, adj);
        std::fflush(stderr);
    }
}

/* THE FLOOR IS RETIRED AND ITS COUNTER IS KEPT AT ZERO ON PURPOSE. Run mg6 read
   0x020e4bd4 as "config symbol, no delink block, no src" and reported it here
   instead of calling it; run mg7 lane L375 decompiled it, so the case below
   calls the real body and g_floor can no longer move. The counter and the line
   the scene prints from it stay, because "0 to the floor" is the measurement
   that says the retirement held -- deleting the counter would turn a proof into
   an assumption. Any address this file has no body for still goes through
   c2_unhandled, which is what an underived code word is. */

/* ---- the two address switches -------------------------------------------
 *
 * Split by ARITY and not by table, because the arity is what decides the call
 * shape and because five of the twenty-five addresses appear in a table of one
 * arity and nowhere else. A code word this does not know is REPORTED, never
 * guessed at, which is the whole difference between these and the `blx r1`
 * they replace. */
static void c2_call0(void *p, const MgC2Pair &e)
{
    ++g_calls;
    if (e.code == 0)
        return;                                  /* the ROM's null-pmf guard */
    if (e.adj != 0) {
        c2_unhandled("with a NONZERO ADJUSTMENT, which no measured pair in "
                     "this class has", (unsigned)e.code, e.adj);
        return;
    }
    switch ((unsigned)e.code) {
    case 0x020e64e4u: func_ov006_020e64e4((int)(size_t)p);  return;
    case 0x020e6354u: func_ov006_020e6354((char *)p);       return;
    case 0x020e628cu: func_ov006_020e628c((char *)p);       return;
    case 0x020e61c4u: func_ov006_020e61c4((char *)p);       return;
    case 0x020e61c0u: func_ov006_020e61c0();                return;
    case 0x020e4ed4u: _ZN15dScMgCurling2_c9DragBeginEv((char *)p);       return;
    case 0x020e4bd4u: ++g_4bd4; _ZN15dScMgCurling2_c10DragUpdateEv((char *)p); return;
    default: break;
    }
    c2_unhandled("UNHANDLED at a zero-argument site", (unsigned)e.code, e.adj);
}

static void c2_call1(void *p, const MgC2Pair &e, int i)
{
    ++g_calls;
    switch ((unsigned)e.code) {          /* the 021419d8 four, per slot */
    case 0x020e5e3cu: ++g_d8[0]; break;
    case 0x020e5b7cu: ++g_d8[1]; break;
    case 0x020e5b70u: ++g_d8[2]; break;
    case 0x020e5a0cu: ++g_d8[3]; break;
    default: break;
    }
    if (e.code == 0)
        return;
    if (e.adj != 0) {
        c2_unhandled("with a NONZERO ADJUSTMENT, which no measured pair in "
                     "this class has", (unsigned)e.code, e.adj);
        return;
    }
    switch ((unsigned)e.code) {
    case 0x020e5e3cu: func_ov006_020e5e3c((char *)p, i);    return;
    case 0x020e5b7cu: func_ov006_020e5b7c((char *)p, i);    return;
    /* THE VENEER TAKES ITS ARGUMENTS. func_ov006_020e5b70 is `ldr ip,[pc];
       bx ip; .word 0x020e5450` in the image -- a tail jump, so r0 and r1 ride
       through untouched -- and both this site and src/func_ov006_020e5b70.c
       used to drop them. That was harmless only while 0x020e5450 was a trap
       that ignored its parameters; it now strides an eleven-entry 0x30-byte
       record array by that index. Both halves are fixed. */
    case 0x020e5b70u: func_ov006_020e5b70((char *)p, i);   return;
    case 0x020e5a0cu: func_ov006_020e5a0c((char *)p, i);    return;
    case 0x020e4794u: _ZN15dScMgCurling2_c12PickStepModeEi((char *)p, i);    return;
    case 0x020e4744u: _ZN15dScMgCurling2_c9StepXOnlyEi((char *)p, i);    return;
    case 0x020e42b4u: _ZN15dScMgCurling2_c9StepXAndYEi((char *)p, i);    return;
    case 0x020e4630u: _ZN15dScMgCurling2_c13StepXPickFastEi((char *)p, i);    return;
    case 0x020e4520u: _ZN15dScMgCurling2_c16StepXPushPosFastEi((char *)p, i);    return;
    case 0x020e440cu: _ZN15dScMgCurling2_c16StepXPushNegFastEi((char *)p, i);    return;
    case 0x020e4348u: _ZN15dScMgCurling2_c15StepXSettleFastEi((char *)p, i);    return;
    case 0x020e41d0u: _ZN15dScMgCurling2_c9StepXPickEi((char *)p, i);    return;
    case 0x020e4094u: _ZN15dScMgCurling2_c12StepXPushPosEi((char *)p, i);    return;
    case 0x020e3f54u: _ZN15dScMgCurling2_c12StepXPushNegEi((char *)p, i);    return;
    case 0x020e3e4cu: _ZN15dScMgCurling2_c11StepXSettleEi((char *)p, i);    return;
    case 0x020e3db4u: _ZN15dScMgCurling2_c12StepYRestartEi((char *)p, i);    return;
    case 0x020e3ce0u: _ZN15dScMgCurling2_c11StepYRampUpEi((char *)p, i);    return;
    case 0x020e3c4cu: _ZN15dScMgCurling2_c13StepYRampDownEi((char *)p, i);    return;
    default: break;
    }
    c2_unhandled("UNHANDLED at a one-argument site", (unsigned)e.code, e.adj);
}

/* src/_ZN15dScMgCurling2_c8BehaviorEv -- RETIRED, run link100 lane SEAT4. Its table is
   seated in port/hal/pmf_seat4.cpp and the matched TU is on
   port/slice_seat4.txt, so the host copy that stood in for it is gone and
   the declaration above is what the faces in this file reach. */

/* ---- src/func_ov006_020e6354.cpp, table 02141978 (0) and 021419d8 (1) -----
 *
 * Verbatim except the two dispatch sites. src launders both decrements through
 * `(u16*)((long long)(int)(p + off))`, an mwcc rematerialisation lever with no
 * host meaning; the plain pointer arithmetic below is the same store.
 * PORT_HOST_ABI: mwcc pointer-to-member wall, decoded through this class's
 * tables and host-copied as the class's address-switch dispatch (c2_call0/
 * c2_call1). */
extern "C" void func_ov006_020e6354(char *c)
{
    if (*(unsigned short *)(c + 0x55b6) != 0) {
        --*(unsigned short *)(c + 0x55b6);
        return;
    }
    if (*(unsigned char *)(c + 0xc4) == 0) {
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(unsigned short *)(c + 0xc0) = 0;
    }
    if (*(unsigned char *)(c + 0x55bd) != 0)
        --*(unsigned char *)(c + 0x55bd);

    c2_call0(c, data_ov006_02141978[*(unsigned char *)(c + 0x55b8)]);

    {
        int count = 0;
        int i = 0;
        char *p = c;
        for (; i < 0xb; i++, p += 0x30) {
            if (*(unsigned char *)(p + 0x4689) != 0) {
                *(int *)(p + 0x466c) = *(int *)(p + 0x4660);
                *(int *)(p + 0x4670) = *(int *)(p + 0x4664);
                if (*(unsigned short *)(p + 0x4680) != 0)
                    --*(unsigned short *)(p + 0x4680);
                c2_call1(c, data_ov006_021419d8[*(unsigned char *)(p + 0x4688)],
                         i);
                if (*(unsigned char *)(p + 0x4688) != 2)
                    count++;
            }
        }
        if (count != 0)
            return;
    }
    *(int *)(c + 0x5580) = 2;
    *(unsigned short *)(c + 0x55b6) = 0x40;
}

/* ---- 02141988 IS SEATED AND func_ov006_020e4800 IS GONE -------------------
 *
 * Run link100 lane PMFB5. data_ov006_02141988's three cells hold HOST addresses
 * after boot, written by port_mg_curling2_states_seat below once every cell has
 * been compared against the ROM's own code word and a zero adjustment word, so
 * src/actors/dScMgCurling2_c.cpp compiles from src and this file no longer defines
 * it.
 *
 *   func_ov006_020e4800   data_ov006_02141988   3 slots   arity 1
 *
 * THE STRIDE, ROM SIDE, read at the body's OWN address out of
 * extracted/overlays/overlay_0006.bin at ov006 base 0x020bfec0
 * (runs/link100/out/PMFB5/rom_gate1.txt):
 *   020e4800  add r3, r4, r0, lsl #3 at 020e4860, pool 020e48c8 = 02141988
 * -> EIGHT. The `add r6, r6, #0x24` in the same body walks the FIFTY-ELEMENT
 * array off the cursor, not the table; the record stride is the add whose base
 * register is the one the literal pool loaded with 02141988.
 *
 * THE STRIDE, EMITTED SIDE, off the matched TU's own /FAsc listing under the
 * port's own flags: [eax*8] and [eax*8+4]. ROM 8 == emitted 8. /Zp4 is a
 * MEASURED NO-OP -- the TU was compiled both ways and the listings differ only
 * in the TITLE line naming the .obj.
 *
 * THE THREE SOURCE PAIRS ALL READ {code, 0} in overlay_0006.bin at the
 * addresses src/__sinit_ov006_02130758.c copies each slot from, all whole-pair
 * copies, no field-form fill:
 *   [0] <- 0213c42c  020e4794/0
 *   [1] <- 0213c494  020e4744/0
 *   [2] <- 0213c48c  020e42b4/0
 *
 * THE ARITY IS ONE AND IT IS READ OFF THIS ROW'S OWN LISTING: `push edi /
 * mov ecx, tab[eax*8+4] / mov eax, tab[eax*8] / add ecx, ebx / call eax` with
 * no caller cleanup, the pushed word being src's own loop counter `i`. So the
 * three faces are one-argument __fastcall. PMFC2's note column says "0 stack
 * argument(s)" for this row and the listing says otherwise, which is the census
 * error lane PMFB4 recorded.
 *
 * ONE /alternatename. src/actors/dScMgCurling2_c.cpp names the member-pointer type
 * at C++ linkage, so MSVC spells the reference
 * ?data_ov006_02141988@@3PAP8C@@AEXH@ZA -- read off the object with
 * dumpbin /symbols, not guessed -- while the ov006 mount defines the plain C
 * name this file declares. The same object's UNDEF sweep also names
 * ?data_0209d4b8@@3HA, which hal/scene_mg_faces_gen.cpp:11 already aliases,
 * which is why this row adds one directive and not two.
 *
 * TWO OF THE THREE CELLS ARE THIS FILE'S OWN HOST COPIES, which is why the seat
 * does not orphan them: slots 1 and 2 are _ZN15dScMgCurling2_c9StepXOnlyEi and
 * _ZN15dScMgCurling2_c9StepXAndYEi, both defined below, and both keep dispatching their own
 * unseated tables through c2_call1. Slot 0 is the matched src body.
 *
 * THE WITNESS MOVES INTO THE FACES. c2_call1 increments g_calls on every routed
 * dispatch and hal/scene_mg_curling2.cpp prints it through
 * port_mg_curling2_state_calls; the three cells no longer reach c2_call1, so
 * each face takes the same increment. */

#pragma comment(linker, "/alternatename:?data_ov006_02141988@@3PAP8C@@AEXH@ZA=_data_ov006_02141988")

#define C2_FACE(slot, sym)                                                \
    static void __fastcall c2_s##slot##_##sym(void *self, void *dead_edx, \
                                              int i)                      \
    {                                                                     \
        (void)dead_edx;                                                   \
        ++g_calls;                                                        \
        sym((char *)self, i);                                             \
    }

C2_FACE(0, _ZN15dScMgCurling2_c12PickStepModeEi)
C2_FACE(1, _ZN15dScMgCurling2_c9StepXOnlyEi)
C2_FACE(2, _ZN15dScMgCurling2_c9StepXAndYEi)

/* run link100 lane SEAT4: this class's remaining state tables are
   seated in port/hal/pmf_seat4.cpp, from inside this installer, so the
   seat order hal/scene_mg.cpp already establishes is the one they get
   and no new call site is added anywhere. */
extern "C" void port_pmf_seat4_curling2(void);

extern "C" void port_mg_curling2_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    port_pmf_seat4_curling2();

    static const struct {
        unsigned slot;
        unsigned rom;
        void *face;
    } seats[] = {
        {0, 0x020e4794u, (void *)c2_s0__ZN15dScMgCurling2_c12PickStepModeEi},
        {1, 0x020e4744u, (void *)c2_s1__ZN15dScMgCurling2_c9StepXOnlyEi},
        {2, 0x020e42b4u, (void *)c2_s2__ZN15dScMgCurling2_c9StepXAndYEi},
    };

    for (unsigned k = 0; k < sizeof seats / sizeof seats[0]; ++k) {
        MgC2Pair *q = &data_ov006_02141988[seats[k].slot];
        if ((unsigned)q->code != seats[k].rom || q->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgCurling2_c state table 02141988 "
                         "slot %u: the sinit left %08x/%d, the ROM's own pairs "
                         "say %08x/0 -- WRONG BYTES\n", seats[k].slot,
                         (unsigned)q->code, q->adj, seats[k].rom);
            std::abort();
        }
        q->code = (int)(size_t)seats[k].face;
    }
}

/* src/_ZN15dScMgCurling2_c9StepXAndYEi -- RETIRED by lane SEAT4, then RESTORED at
   the main->port sync (lane SYNC5).  SEAT4 retired this host copy in favour of
   the matched per-function TU on port/slice_seat4.txt, and main has since folded
   that body into src/actors/dScMgCurling2_c.cpp (ov006 delinks, .text
   0x020e3854..0x020e5450).  The port cannot take the consolidated TU:
   port/slice_s75.txt already compiles the per-function leftovers whose
   addresses that range covers, so taking both is a duplicate symbol.  The
   slice_seat4 row is quarantined and this copy stands in again; the pair
   retires together when s75 moves to the consolidated TU. */
/* RETIRED, run link100 lane HOSTGEN2. The reason this copy was RESTORED at the
   sync is spelled out above: main folded the body into
   src/actors/dScMgCurling2_c.cpp and the port could not take the consolidated
   TU. It can now. Every one of this class's four dispatch tables --
   data_ov006_021419f8, 021419a0 and 021419b8 (hal/pmf_seat4.cpp) and
   data_ov006_02141988 (lane PMFB5) -- is SEATED with host addresses at boot,
   and the faces seated there are `__fastcall (void *self, void *dead_edx, int
   arg)`, which is exactly how MSVC enters a pointer-to-member call under the
   /vmg /vmm pair port/CMakeLists.txt compiles with: receiver in ecx, the one
   argument pushed. So the matched TU's own `(self->*table[k])(i)` is already
   the right call, and the stride the restore note worries about is the ROM's
   eight bytes on both machines.

   Text kept, not deleted. */
#if 0  /* HOSTGEN2: body seated from src, see above */
extern "C" void _ZN15dScMgCurling2_c9StepXAndYEi(char *c, int i)
{
    const int idx = i * 0x24;
    const unsigned char k0 = *(unsigned char *)(c + idx + 0x48de);
    c2_call1(c, data_ov006_021419f8[k0], i);
    const unsigned char k1 = *(unsigned char *)(c + idx + 0x48df);
    c2_call1(c, data_ov006_021419a0[k1], i);
}
#endif  /* HOSTGEN2: _ZN15dScMgCurling2_c9StepXAndYEi retired to src */

/* src/_ZN15dScMgCurling2_c9StepXOnlyEi -- RETIRED by lane SEAT4, then RESTORED at
   the main->port sync (lane SYNC5), for the reason spelled out on StepXAndY
   directly above: main folded the matched TU into src/actors/dScMgCurling2_c.cpp
   and slice_s75's per-function leftovers cover the same range. */
/* RETIRED, run link100 lane HOSTGEN2. The reason this copy was RESTORED at the
   sync is spelled out above: main folded the body into
   src/actors/dScMgCurling2_c.cpp and the port could not take the consolidated
   TU. It can now. Every one of this class's four dispatch tables --
   data_ov006_021419f8, 021419a0 and 021419b8 (hal/pmf_seat4.cpp) and
   data_ov006_02141988 (lane PMFB5) -- is SEATED with host addresses at boot,
   and the faces seated there are `__fastcall (void *self, void *dead_edx, int
   arg)`, which is exactly how MSVC enters a pointer-to-member call under the
   /vmg /vmm pair port/CMakeLists.txt compiles with: receiver in ecx, the one
   argument pushed. So the matched TU's own `(self->*table[k])(i)` is already
   the right call, and the stride the restore note worries about is the ROM's
   eight bytes on both machines.

   Text kept, not deleted. */
#if 0  /* HOSTGEN2: body seated from src, see above */
extern "C" void _ZN15dScMgCurling2_c9StepXOnlyEi(char *o, int i)
{
    const unsigned char idx = *(unsigned char *)(o + i * 0x24 + 0x48de);
    c2_call1(o, data_ov006_021419b8[idx], i);
}
#endif  /* HOSTGEN2: _ZN15dScMgCurling2_c9StepXOnlyEi retired to src */
