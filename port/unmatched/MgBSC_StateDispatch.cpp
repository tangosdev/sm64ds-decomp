// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgBSC_c's ONE table dispatcher and its eleven state addresses.
// Run mg9, lane LKY.  Actor id 0x184, scene 388, "Lucky Stars".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgMemory2_StateDispatch.cpp.  The first carries the ROM
// disassembly of the mwcc dispatch sequence and the finding that the overlay
// constructors already copy the pairs at the right stride; the second is the
// template this file follows, and this class's slot 6 is that class's slot 6
// with two constants changed.  Neither is repeated here.
//
// ---- 1. THE CLASS IS dScMgBSC_c AND THE ROM SAYS SO -----------------------
//
// The word IMMEDIATELY BEFORE the vtable is the type_info pointer, and that
// record's second word is the name pointer.  Read out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//     0x0213fec4 -> 0x0213fd8c   the type_info
//     0x0213fd8c + 4 -> 0x0213fd98 -> "10dScMgBSC_c"
//     0x0213fd8c + 8 -> 0x0213bc64, which is the BASE's type_info and whose
//                       name pointer reads "19dScMgSingle3DBase_c"
//
// so the hierarchy is three deep and the ROM states both edges of it:
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgBSC_c          data_ov006_0213fec8  36 slots
//
// The RTTI is the strong witness and the code agrees with it twice:
// src/MgLuckyStars_Spawn.cpp writes 0x0213e448 into the object and then
// 0x0213fec8 over it, and slots 16 and 17 (src/func_ov006_02124908.c and
// src/func_ov006_0212497c.cpp) unwind them in the opposite order.
//
// ---- 2. ONE TABLE, FOURTEEN SLOTS, ELEVEN DISTINCT BODIES -----------------
//
// src/__sinit_ov006_0213326c.c, THIS class's own overlay constructor, read
// assignment by assignment -- NOT by sweeping the pair address range, which is
// the trap port/mg_fanout_costs.txt section 4 names and which this class walks
// straight into TWICE.  The fourteen source pairs run 0x0213fd0c..0x0213fd84,
// and inside that run sit two words the constructor never copies:
//
//     0x0213fd44  an INT ARRAY, not a pair.  src/func_ov006_02124ec4.c and
//                 src/func_ov006_02124fd8.c both index it as
//                 data_ov006_0213fd44[i] and neither ever calls it.
//     0x0213fd6c  MgLuckyStars_SpawnInfo -- the factory word 0x021255f8
//                 followed by the doubled id 0x01840184.
//
// A sweep of the span would have produced sixteen "pairs" and routed two
// addresses that are not code.  The constructor's own list skips both.
//
// Every pair was then read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0.  FOURTEEN pairs, ELEVEN distinct code words, and every
// adjustment word reads zero:
//
//   slot  source pair   code       slot  source pair   code
//   ----  -----------   --------   ----  -----------   --------
//     0   0213fd7c      021250e4     7   0213fd54      02124b58
//     1   0213fd5c      02124fd8     8   0213fd84      02124b58
//     2   0213fd4c      02124ec4     9   0213fd14      02124ae4
//     3   0213fd24      02124e1c    10   0213fd0c      02124ae4
//     4   0213fd3c      02124dc0    11   0213fd74      02124a08
//     5   0213fd34      02124cb4    12   0213fd1c      02124a04
//     6   0213fd64      02124bb4    13   0213fd2c      02124a04
//
// THE SOURCE PAIRS ARE NOT IN SLOT ORDER and that is the whole reason the
// constructor is the ground truth rather than the address range: slot 0 is
// copied from the SECOND-HIGHEST source address and slot 10 from the lowest.
// Three bodies are shared by two slots each (7/8, 9/10, 12/13), which is what
// makes fourteen slots eleven addresses.
//
//   table                n  arity  dispatched by
//   -------------------  -  -----  -------------------------------------------
//   data_ov006_02142f94  14   0    func_ov006_021254c0  (vtable slot 6)
//
// ONE TABLE AND ONE LEVEL, WHICH IS THE CHEAP END OF THE RANGE.
// port/mg_fanout_costs.txt section 14 tells a lane to ask "how deep is the
// machine" rather than "how many TUs name the table", and to check whether any
// state ADDRESS is also a dispatcher.  None of the eleven above is: the ROM
// scan in section 3 walks this class's bodies one at a time and finds exactly
// one Itanium decode site across all twenty of them, and it is slot 6's.
//
// ---- 3. THE DETECTOR THAT SETTLES IT IS THE ROM, NOT THE SOURCE -----------
//
// port/mg_fanout_costs.txt prescribes two detectors, a link and a `::*` source
// sweep, and its own FLW amendment says both return clean over a TU that
// open-codes the sequence in plain ints -- so twelve is "a floor rather than a
// census" and only a run convicts the thirteenth shape.
//
// THERE IS A THIRD DETECTOR AND IT IS A CENSUS RATHER THAN A FLOOR.  The five
// ARM instructions the wall is made of have a fixed encoding, so the OVERLAY
// IMAGE can be scanned for them directly:
//
//     add  Rd, Rn, Rm, asr #1        cond=E, opcode ADD, immediate shift,
//     ands Rx, Ry, #1                shift type ASR, shift amount 1, then an
//                                    AND-immediate with S set and imm 1
//
// That test is blind to how the source spells the pair, so it finds the
// extern "C" shape, the C++-linkage shape and the plain-int shape alike.  Run
// over the whole of ov006 it returns 114 sites.
//
// SCAN THE CLASS'S BODIES, NOT AN ADDRESS SPAN.  Each body is taken at its own
// start and size from config/arm9/overlays/ov006/symbols.txt:
//
//   this class's 20 OWN bodies       1 site   0x021254dc, in func_ov006_021254c0
//     (8 vtable overrides + 11 state bodies + the factory)
//   the 8 INHERITED middle-base       0 sites
//     bodies of dScMgSingle3DBase_c
//
// So the wall is ONE TU, at vtable slot 6.  The `::*` sweep over this class's
// closure agrees and adds nothing, and the claim rests on the image rather than
// on a source convention.
//
// AN EARLIER VERSION OF THIS PARAGRAPH SCANNED A SPAN AND THE SPAN WAS WRONG
// TWICE, which is worth keeping because the mistake is the one this class
// invites.  It read "over this class's code span (0x02124908..0x0212a3c0) it
// returns exactly ONE".  That span is NOT this class's -- its own bodies end at
// 0x0212568c, and 0x0212a3c0 is 0x4d34 bytes further on, deep inside the
// NEIGHBOUR's code -- and the scan over it returns FIVE, not one.  The extra
// four are genuine dispatch sequences and they belong to the class section 4
// correctly attributes them to.  A body-by-body scan cannot make that error;
// an address range picked by eye can, and did.
//
// THE OTHER FOUR SITES ARE MgSnowballSlalom's, AND THE NEXT LANE IS OWED THEM.
// Section 4 hands whoever takes 0x179 a resolved vtable (data_ov006_0214000c).
// Handing them a resolved vtable and a wall priced at zero would be worse than
// handing them nothing, so the four sites are named here:
//
//   0x02129dc0  func_ov006_02129d94   dispatches data_ov006_02143070
//   0x02129df8  func_ov006_02129d94   AND data_ov006_02143020 -- one TU, two
//                                     tables, two decode sites
//   0x0212a248  func_ov006_0212a224   dispatches data_ov006_02143050
//   0x0212a324  func_ov006_0212a2e0   dispatches data_ov006_02143038
//
// Four decode sites across THREE TUs, covering FOUR tables (14 slots in total,
// all arity 1, built by src/__sinit_ov006_021333e0.c).  Its vtable slot 6,
// func_ov006_021283a4, is NOT one of them: it reaches the machine by CALLING
// func_ov006_0212a2e0 rather than decoding a pair itself, so a lane that only
// read slot 6 would price that class's wall at zero as well.  0x179's state
// machine is also TWO LEVELS DEEP -- _0212a224 and _02129d94 are themselves
// slots 1 and 2 of data_ov006_02143038 -- which is the section 14 depth
// question already answered for them.
//
// ---- 4. THE SLOT-6 DISASSEMBLY, WHICH IS THE ONLY THING WORTH COPYING -----
//
// Read out of the shipped overlay image, because the state index offset and the
// `this` a callee is handed are the two things a wrong host copy gets silently
// wrong.
//
//   func_ov006_021254c0  vtable slot 6, Behavior, 0x5c
//     push {r4,lr} / mov r4,r0
//     add  r0,r4,#0x5000 / ldr r0,[r0,#0x1b8]     the index at +0x51b8
//     ldr  r1,[pc,#0x3c]                          = 0x02142f94
//     add  r3,r1,r0,lsl #3                        stride EIGHT
//     ldr  r1,[r3,#4] / add r0,r4,r1,asr #1 / ands r1,r1,#1
//     ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1] / ldreq r1,[r3]
//     blx  r1                                     this = the class base
//     bl   0x020b65e4                             func_ov004_020b65e4
//     ldr  r0,[pc,#0x14] (= 0x00004f38) / add r0,r4,r0 / bl 0x020c19d0
//     mov  r0,#1 / pop / bx lr
//
// 21 instructions plus a two-word pool = 23 words = 0x5c.  That tail --
// func_ov004_020b65e4() then func_ov006_020c19d0(c + 0x4f38) then return 1 --
// is dScMgMemory2_c's slot 6 verbatim; only the index offset and the table
// differ.  Both callees are already in the port, the second as a host copy in
// unmatched/MgMemory2_FieldPmf.cpp.
//
// ---- 5. NO STATE FLOOR, AND THE COUNTER STAYS ANYWAY ----------------------
//
// All eleven addresses reach a real src TU and none of the eleven carries a
// NONMATCHING banner, so this class's floor count is ZERO on the state axis.
// The bodiless counter below is kept rather than deleted for the reason
// MgMemory2_StateDispatch.cpp keeps its own: hal/scene_mg_luckystars.cpp prints
// it on every run, so a later lane that adds an unreachable state gets a
// nonzero reading rather than a silent gap.
//
// ---- 6. WHY THIS FILE HAS ITS OWN ENTRY POINT -----------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and chains it to
// exactly one per-class pair, which unmatched/MgCurling_StateDispatch.cpp
// defines.  A second class cannot define those names, so this file calls
// port_mg_bsc_call0, which tries THIS class's switch and hands everything else
// to the framework unchanged -- the shape MgCoin_, MgLuigi_, MgPachinko_ and
// MgMemory2_StateDispatch.cpp all use.  The framework therefore remains the
// single place that decides what a null code word means, what a nonzero
// adjustment means and how an unhandled address is reported.
//
// There is no port_mg_bsc_call1: this class has no arity-1 table.  The FOUR
// arity-1 tables that sit next to this class's in ov006's .data
// (data_ov006_02143020/38/50/70) belong to the NEIGHBOUR and are not routed
// here; port/slice_lky.txt section 4 is the derivation of who owns them.

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);

/* ---- the eleven routed state bodies, in address order --------------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   ride-through is called the way the ROM calls it rather than the way the
   slot's arity would suggest.  func_ov006_02124a04 is a four-byte `bx lr` body
   whose src takes (void) -- there is nothing for a missing argument to be
   wrong about, which is the MgCoin_StateDispatch.cpp and
   MgMemory2_StateDispatch.cpp ruling for the same shape, and the ROM body is
   one instruction long so it cannot read r0. */
void func_ov006_02124a04(void);               /* bx lr, slots 12 and 13 */
void func_ov006_02124a08(void *self);
void func_ov006_02124ae4(char *c);
void func_ov006_02124b58(char *c);
void func_ov006_02124bb4(char *c);
void func_ov006_02124cb4(char *o);
void func_ov006_02124dc0(void *arg0);
void func_ov006_02124e1c(char *c);
void func_ov006_02124ec4(char *sl);
void func_ov006_02124fd8(char *c);
void func_ov006_021250e4(char *base);

/* the mount table this file dispatches, re-typed to the ROM's eight-byte pair.
   The ov006 mount defines the storage; __sinit_ov006_0213326c fills it at
   minigame scene load. */
extern MgPmf data_ov006_02142f94[];

/* the ordinary callees the host copy below keeps, each spelled as its own src
   TU spells it.  func_ov006_020c19d0 is itself a host copy, in
   unmatched/MgMemory2_FieldPmf.cpp. */
void func_ov004_020b65e4(void);
void func_ov006_020c19d0(void *c);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_bsc_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO -- section 5. */
static unsigned g_bsc_floor_hits;
/* The state index the switch was last entered on, so a run can say the machine
   MOVED rather than only that it fired.  hal/scene_mg_luckystars.cpp prints the
   low and high water marks. */
static int g_bsc_idx_lo = -1, g_bsc_idx_hi = -1;

static int bsc_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142f94, dispatched by vtable slot 6 */
    case 0x021250e4u: func_ov006_021250e4(c); return 1;   /* slot 0  */
    case 0x02124fd8u: func_ov006_02124fd8(c); return 1;   /* slot 1  */
    case 0x02124ec4u: func_ov006_02124ec4(c); return 1;   /* slot 2  */
    case 0x02124e1cu: func_ov006_02124e1c(c); return 1;   /* slot 3  */
    case 0x02124dc0u: func_ov006_02124dc0(c); return 1;   /* slot 4  */
    case 0x02124cb4u: func_ov006_02124cb4(c); return 1;   /* slot 5  */
    case 0x02124bb4u: func_ov006_02124bb4(c); return 1;   /* slot 6  */
    case 0x02124b58u: func_ov006_02124b58(c); return 1;   /* slots 7, 8 */
    case 0x02124ae4u: func_ov006_02124ae4(c); return 1;   /* slots 9, 10 */
    case 0x02124a08u: func_ov006_02124a08(c); return 1;   /* slot 11 */
    case 0x02124a04u: func_ov006_02124a04();  return 1;   /* slots 12, 13,
                                                             bx lr body */
    default:                                  return 0;
    }
}

/* The entry point the host copy below uses.  Everything this switch does not
   own goes to the framework unchanged, so the null-code guard, the nonzero-
   adjustment refusal and the UNHANDLED report all still live in exactly one
   place. */
extern "C" void port_mg_bsc_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && bsc_try_0(self, code)) {
        ++g_bsc_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" unsigned port_mg_bsc_state_hits(void) { return g_bsc_state_hits; }
extern "C" unsigned port_mg_bsc_floor_hits(void) { return g_bsc_floor_hits; }
extern "C" void port_mg_bsc_index_range(int *lo, int *hi)
{
    *lo = g_bsc_idx_lo;
    *hi = g_bsc_idx_hi;
}

// ---- the one host copy -----------------------------------------------------
//
// src/func_ov006_021254c0.cpp verbatim except for the table declaration (MgPmf
// rather than a member-pointer type) and the dispatch site (port_mg_bsc_call0
// rather than `(c->*table[idx].pmf)()`).  Nothing else moved.
//
// NOTE THE `extern "C"` ON THE TABLE IN src: this dispatcher is SILENT to a
// link.  `extern "C" Entry data_ov006_02142f94[];` mangles as the plain C name
// the ov006 mount already defines, so the linker is satisfied while MSVC
// strides the eight-byte table by four and reads half of one record and half of
// the next from slot 1 on.  It is one of the two silent shapes
// port/mg_fanout_costs.txt section 4 names.  IT IS ALSO VTABLE SLOT 6 -- the
// slot every seat wires by name -- which is section 14's practical rule:
// before wiring slot 6 by name, read the src.

// PORT_HOST_ABI: dScMgBSC_c vtable slot 6 table dispatcher; the mwcc eight-byte member-pointer table MSVC's four-byte pmf cannot stride, so the host strides the {code, adj} pairs directly.
extern "C" int func_ov006_021254c0(void *self)
{
    char *c = (char *)self;
    const int idx = *(const int *)(c + 0x51b8);

    if (g_bsc_idx_lo < 0 || idx < g_bsc_idx_lo) g_bsc_idx_lo = idx;
    if (idx > g_bsc_idx_hi)                     g_bsc_idx_hi = idx;

    const MgPmf *e = &data_ov006_02142f94[idx];
    port_mg_bsc_call0(c, e->code, e->adj);
    func_ov004_020b65e4();
    func_ov006_020c19d0(c + 0x4f38);
    return 1;
}
