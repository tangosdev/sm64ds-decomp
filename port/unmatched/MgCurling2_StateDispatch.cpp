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
 *   func_ov006_020e42b4   021419f8, 021419a0        same, two globals
 *   func_ov006_020e6354   02141978, 021419d8        PMF typedefs INSIDE
 *                                                   extern "C" -- silent
 *   func_ov006_020e4744   021419b8                  same -- silent
 *   func_ov006_020e683c   02141a18                  NO member-pointer type at
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
 * The arities above are read off the ROM. func_ov006_020e42b4 sets `mov r1,r5`
 * -- the loop index -- immediately before both of its `blx r2`;
 * func_ov006_020e4744 never touches r1 after entry, so the incoming index rides
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
 * (src/func_ov006_020e4bd4.c, NONMATCHING-bannered, size exact, register
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
void func_ov006_020e4ed4(char *c);
void func_ov006_020e4bd4(char *c);

/* the 1-arity state bodies */
void func_ov006_020e5e3c(char *c, int i);
void func_ov006_020e5b7c(char *c, int i);
void func_ov006_020e5b70(char *c, int i);
void func_ov006_020e5a0c(char *c, int i);
void func_ov006_020e4794(char *c, int i);
void func_ov006_020e4630(char *c, int i);
void func_ov006_020e4520(char *c, int i);
void func_ov006_020e440c(char *c, int i);
void func_ov006_020e4348(char *c, int i);
void func_ov006_020e41d0(char *c, int i);
void func_ov006_020e4094(char *c, int i);
void func_ov006_020e3f54(char *c, int i);
void func_ov006_020e3e4c(char *c, int i);
void func_ov006_020e3db4(char *c, int i);
void func_ov006_020e3ce0(char *c, int i);
void func_ov006_020e3c4c(char *c, int i);

/* the rest of what the five dispatchers call, verbatim from src */
int  RandomIntInternal(int *seed);
extern int data_0209d4b8;                /* src spells it at C++ linkage; a
                                            host copy has no reason to, and
                                            spelling it here saves an alias */
void func_ov006_020e3948(char *c);

/* the five this file defines */
void func_ov006_020e6354(char *c);
void func_ov006_020e4800(char *o);
void func_ov006_020e42b4(char *c, int i);
void func_ov006_020e4744(char *o, int i);
int  func_ov006_020e683c(char *c);

unsigned port_mg_curling2_state_calls(void);
unsigned port_mg_curling2_state_unknown(void);
unsigned port_mg_curling2_state_floor(void);
unsigned port_mg_curling2_state_4bd4(void);
unsigned port_mg_curling2_d8_hits(int slot);

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
    case 0x020e4ed4u: func_ov006_020e4ed4((char *)p);       return;
    case 0x020e4bd4u: ++g_4bd4; func_ov006_020e4bd4((char *)p); return;
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
    case 0x020e4794u: func_ov006_020e4794((char *)p, i);    return;
    case 0x020e4744u: func_ov006_020e4744((char *)p, i);    return;
    case 0x020e42b4u: func_ov006_020e42b4((char *)p, i);    return;
    case 0x020e4630u: func_ov006_020e4630((char *)p, i);    return;
    case 0x020e4520u: func_ov006_020e4520((char *)p, i);    return;
    case 0x020e440cu: func_ov006_020e440c((char *)p, i);    return;
    case 0x020e4348u: func_ov006_020e4348((char *)p, i);    return;
    case 0x020e41d0u: func_ov006_020e41d0((char *)p, i);    return;
    case 0x020e4094u: func_ov006_020e4094((char *)p, i);    return;
    case 0x020e3f54u: func_ov006_020e3f54((char *)p, i);    return;
    case 0x020e3e4cu: func_ov006_020e3e4c((char *)p, i);    return;
    case 0x020e3db4u: func_ov006_020e3db4((char *)p, i);    return;
    case 0x020e3ce0u: func_ov006_020e3ce0((char *)p, i);    return;
    case 0x020e3c4cu: func_ov006_020e3c4c((char *)p, i);    return;
    default: break;
    }
    c2_unhandled("UNHANDLED at a one-argument site", (unsigned)e.code, e.adj);
}

/* ---- src/func_ov006_020e683c.cpp, vtable slot 6, the Behavior -------------
 *
 * Verbatim except that the hand-written Itanium decode becomes c2_call0. The
 * state index at +0x5580 is the same word slot 0 sets to 1 and slot 18 zeroes.
 * The decode itself is kept rather than replaced by `tbl[idx]` indexing,
 * because the virtual bit and the adjustment shift are the ROM's own and this
 * file reports them rather than assuming them away. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch open-coded as two ints, calling the raw DS code word MSVC's pmf ABI cannot reproduce
extern "C" int func_ov006_020e683c(char *c)
{
    const int idx = *(int *)(c + 0x5580);
    const MgC2Pair &e = data_ov006_02141a18[idx];

    if (e.adj & 1) {
        c2_unhandled("through the VIRTUAL arm of the member pointer, which no "
                     "measured pair in this class uses",
                     (unsigned)e.code, e.adj);
    } else {
        c2_call0(c + (e.adj >> 1), e);
    }

    func_ov006_020e4800(c);
    func_ov006_020e3948(c);
    return 1;
}

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

/* ---- src/func_ov006_020e4800.cpp, table 02141988 (arity 1) ---------------- */
// PORT_HOST_ABI: mwcc arity-1 table pointer-to-member dispatch, the 8-byte pair MSVC's 4-byte pmf strides wrong
extern "C" void func_ov006_020e4800(char *o)
{
    int i;
    char *e = o;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(e + 0x48dc) != 0) {
            const unsigned char idx = *(unsigned char *)(e + 0x48dd);
            c2_call1(o, data_ov006_02141988[idx], i);
            if ((*(int *)(e + 0x48c4) >> 0xc) >= 0xc8) {
                *(int *)(e + 0x48c0) =
                    (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16)
                     & 0x7fff) << 5 >> 0xf << 0xf;
                *(int *)(e + 0x48c4) = -0x8000;
                *(unsigned char *)(e + 0x48de) = 0;
                *(unsigned char *)(e + 0x48dd) = 0;
                *(unsigned char *)(e + 0x48df) = 0;
            }
        }
        e += 0x24;
    }
}

/* ---- src/func_ov006_020e42b4.cpp, tables 021419f8 and 021419a0 ------------
 *
 * Two dispatches off two adjacent index bytes, +0x48de and +0x48df, both at
 * arity 1 with the loop index the caller handed in. The ROM sets `mov r1,r5`
 * before each `blx r2`, which is where the arity is read from.
 * PORT_HOST_ABI: mwcc pointer-to-member wall, decoded through this class's
 * tables and host-copied as the class's address-switch dispatch, c2_call1. */
extern "C" void func_ov006_020e42b4(char *c, int i)
{
    const int idx = i * 0x24;
    const unsigned char k0 = *(unsigned char *)(c + idx + 0x48de);
    c2_call1(c, data_ov006_021419f8[k0], i);
    const unsigned char k1 = *(unsigned char *)(c + idx + 0x48df);
    c2_call1(c, data_ov006_021419a0[k1], i);
}

/* ---- src/func_ov006_020e4744.cpp, table 021419b8 (arity 1) ----------------
 * PORT_HOST_ABI: mwcc pointer-to-member wall, decoded through this class's
 * table and host-copied as the class's address-switch dispatch, c2_call1. */
extern "C" void func_ov006_020e4744(char *o, int i)
{
    const unsigned char idx = *(unsigned char *)(o + i * 0x24 + 0x48de);
    c2_call1(o, data_ov006_021419b8[idx], i);
}
