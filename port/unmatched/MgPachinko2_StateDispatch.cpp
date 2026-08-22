// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgPachinko2_c's three dispatching TUs and its twenty-eight state
// addresses. Actor id 0x171, scene 369. Run mg6, lane LKT.
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgPachinko_StateDispatch.cpp. The first carries the ROM
// disassembly of the dispatch sequence and the framework's own states; the
// second is the sibling class's version of this file. Neither is repeated
// here.
//
// ---- THE TWENTY-EIGHT ADDRESSES, AND WHERE THEY COME FROM -----------------
//
// Not guessed and not read out of a running table. src/__sinit_ov006_02131cd0.c
// names, in source order, which .data pair fills which .bss slot, and
//
//     python port/tools/stategen.py --sinit src/__sinit_ov006_02131cd0.c \
//                                   --overlay 6
//
// reprints the join. That constructor is attributed to THIS class by address
// containment: its copied pairs run 0x0213da5c..0x0213db4c and that range
// brackets MgLakituLaunch_SpawnInfo at 0x0213da64.
//
//   table                n  arity  state addresses (in slot order)
//   -------------------  -  -----  ----------------------------------------
//   data_ov006_021426cc  5    1    020fff54 020ffde4 020ffb54 020ff8c8
//                                  020ff690
//   data_ov006_021426f4  8    1    02102f3c 02102fe8 02103360 02103608
//                                  0210371c 02103870 0210397c 02103994
//   data_ov006_02142734  15   1    02102274 021020c4 02101e88 02101af0
//                                  021019e0 021016ec 021012cc 02101224
//                                  02101088 02100f7c 02100e3c 02100d90
//                                  02100bac 02100b08 021009b8
//
// 5+8+15 = 28. EVERY TABLE IS ARITY 1 and there is no arity-0 table in this
// class, which is why this file has one switch. Every adjustment word in the
// twenty-eight pairs is ZERO, re-measured out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 by reading each pair
// the constructor NAMES rather than by sweeping the address range those pairs
// span -- the sweep is how port/mg_fanout_costs.txt section 4 says a lane gets
// phantom states with nonzero adjustments, and this class's range holds its
// own SpawnInfo at 0x0213da64 in the middle of it.
//
// THE STATE 4 THAT PROVES THE SIBLING'S WIDTH. data_ov006_02142734 slot 4 is
// 0x021019e0, copied from the pair at data_ov006_0213da5c -- which is the word
// hal/scene_mg.cpp section 8 identifies as index 36 of dScMgPachinko_c's
// vtable, the phantom slot a 37-slot fill of THAT class would have written
// over. From this commit the corruption would no longer be latent: it would
// take out this class's state 4 on the same boot. The sibling's fill is
// called with 36.
//
// STATEGEN REFUSED FOUR OF THE TWENTY-EIGHT AND THE REFUSALS ARE CARRIED, NOT
// GUESSED. All four are genuine decomp holes -- a config symbol, no delink
// block in config/arm9/overlays/ov006/delinks.txt, and no src file in either
// extension:
//
//   0x020ffb54  slot 2 of data_ov006_021426cc   0x290 bytes
//   0x020ff8c8  slot 3 of data_ov006_021426cc   0x28c bytes
//   0x02102274  slot 0 of data_ov006_02142734   0x1f8 bytes
//   0x02102fe8  slot 1 of data_ov006_021426f4   0x378 bytes
//
// Each is the func_ov006_020e1854 shape as it stood BEFORE run link60 lane CT1
// hosted it: the case below reports which state was wanted, by address, and
// calls nothing. No symbol is invented for any of them -- inventing a
// definition for a body with no source is the guess
// port/tools/inferred_stub_guard exists to refuse -- and no port_ body is
// transcribed here either, because transcribing four ROM bodies is a second
// lane's work and not this seat's.
//
// ---- WHY ALL THREE DISPATCHERS ARE HOSTED --------------------------------
//
// port/mg_fanout_costs.txt section 4's rulebook corollary keys on the
// SPELLING, and a `::*` sweep of this class's whole closure returns exactly
// ten TUs: these three, and the seven ov004 framework ones already host-copied
// by run link60 lane MG2 and run mg5 lane BASESET. Unlike the sibling class,
// where three of five dispatchers were on the corollary's SAFE side and were
// hosted only because the pair word is a DS address, all three of this
// class's name a member-pointer type and are the wall itself:
//
//   func_ov006_020fff84  `struct Entry { PMF pmf; };` and
//        `extern Entry data_ov006_021426cc[];` at file scope OUTSIDE
//        extern "C". MSVC mangles the member-pointer type into the symbol, so
//        a link would name it -- and sizeof(Entry) is 4 where the ROM strides
//        8, so index 1 would read half of pair 0 and half of pair 1.
//   func_ov006_02102e8c  the same shape, same spelling, same visibility, over
//        data_ov006_021426f4.
//   func_ov006_0210246c  `extern "C" PMF data_ov006_02142734[];` INSIDE
//        extern "C", so the global mangles to the plain C name the mount
//        already defines and THE LINK IS SATISFIED. Section 4's silent
//        extern-"C" class, the one no link can ever surface, and the reason
//        this file was written from a source sweep rather than from an
//        LNK1120.
//
// ---- THE THREE ROM SEQUENCES, READ RATHER THAN ASSUMED -------------------
//
// All three were disassembled out of extracted/overlays/overlay_0006.bin at
// base 0x020bfec0, because the guard offset and the loop bound are what decide
// whether a clean run is a real one, and a wrong guard offset produces a run
// that dispatches nothing and reads green. Each is the same shape:
//
//   func_ov006_020fff84   i = 0..1     add r0, r6, r5, lsl #5   (stride 0x20)
//                                      add r0, r0, #0x5000
//                                      ldrb r1, [r0, #0x634]    guard  +0x5634
//                                      ldrb r0, [r0, #0x635]    index  +0x5635
//   func_ov006_0210246c   i = 0..2     add r0, r6, r5, lsl #6   (stride 0x40)
//                                      add r0, r0, #0x5000
//                                      ldrb r1, [r0, #0x294]    guard  +0x5294
//                                      ldrb r0, [r0, #0x296]    index  +0x5296
//                                      and after the dispatch,
//                                      bl 0x210076c with (r6, r5)
//   func_ov006_02102e8c   i = 0..0x2f  add r0, r6, r5, lsl #6   (stride 0x40)
//                                      add r0, r0, #0x4000
//                                      ldrb r1, [r0, #0x698]    guard  +0x4698
//                                      ldrb r0, [r0, #0x699]    index  +0x4699
//
// and all three then run the five-instruction ARM Itanium sequence verbatim:
//
//     add r3, r4, r0, lsl #3     ; the EIGHT-byte stride, which is the whole
//                                ; reason the src spelling is unusable
//     ldr r1, [r3, #4]           ; the adjustment
//     add r0, r6, r1, asr #1     ; this, advanced, arithmetic shift
//     ands r1, r1, #1            ; the virtual bit, in the LSB
//     ldrne r2,[r0] / ldrne r1,[r3] / ldrne r2,[r2,r1] / ldreq r2,[r3]
//     mov r1, r5                 ; the loop index, the ONE argument
//     blx r2
//
// THE RECEIVER IS THE OBJECT, UNADVANCED, on every dispatch this class makes:
// r6 holds the original r0 throughout and every adjustment word reads zero, so
// `add r0, r6, r1, asr #1` is r6. The host copies below pass `self` and not
// the per-entry base, which is what the src spelling `(c->*...)(i)` says too.
//
// ---- THE CHAIN, AND WHY IT RUNS THIS WAY ---------------------------------
//
// unmatched/MgBase_StateDispatch.cpp's port_mg_call1 chains
// mg_try_ov004 -> port_mg_try_ov006_1 -> report, and port_mg_try_ov006_1 is
// dScMgCurling_c's. A second class cannot define that symbol without a
// duplicate, and this lane does not edit either of those two files.
//
// So the host copies below call lkt_call1, which tries THIS class's switch
// first and falls through to port_mg_call1 for anything it does not know. That
// ordering is correct rather than merely convenient: the framework's state
// tables hold ov004 addresses and none of this class's twenty-eight, so the
// two sets are disjoint by construction and neither switch can shadow the
// other. The fall-through also keeps ONE reporting site for a genuinely
// unknown address, which is MgBase_StateDispatch's mg_unhandled.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the twenty-four state bodies that have one, in address order --------
   All twenty-four take (self, index) in their own src -- checked one by one,
   not assumed from the table's arity -- so none of them is the ARM
   ride-through curling met at func_ov006_020e2eb8. They are declared with
   void* here rather than with each src's own struct type: these are C-linkage
   symbols, the declaration cannot change the mangling, and a host copy that
   repeated two dozen private struct definitions would be two dozen more places
   for a layout to drift. */
void func_ov006_020ff690(void *c, int i);
void func_ov006_020ffde4(void *c, int i);
void func_ov006_020fff54(void *c, int i);
void func_ov006_021009b8(void *c, int i);
void func_ov006_02100b08(void *c, int i);
void func_ov006_02100bac(void *c, int i);
void func_ov006_02100d90(void *c, int i);
void func_ov006_02100e3c(void *c, int i);
void func_ov006_02100f7c(void *c, int i);
void func_ov006_02101088(void *c, int i);
void func_ov006_02101224(void *c, int i);
void func_ov006_021012cc(void *c, int i);
void func_ov006_021016ec(void *c, int i);
void func_ov006_021019e0(void *c, int i);
void func_ov006_02101af0(void *c, int i);
void func_ov006_02101e88(void *c, int i);
void func_ov006_021020c4(void *c, int i);
void func_ov006_02102f3c(void *c, int i);
void func_ov006_02103360(void *c, int i);
void func_ov006_02103608(void *c, int i);
void func_ov006_0210371c(void *c, int i);
void func_ov006_02103870(void *c, int i);
void func_ov006_0210397c(void *c, int i);
void func_ov006_02103994(void *c, int i);

/* the three mount tables, re-typed to the ROM's eight-byte pair. The mount
   defines the storage; __sinit_ov006_02131cd0 fills it at scene load. */
extern MgPmf data_ov006_021426cc[];
extern MgPmf data_ov006_021426f4[];
extern MgPmf data_ov006_02142734[];

/* the one ordinary callee a host copy below keeps, from its own src. It has NO
   BODY -- no delink block and no src file -- and is the named self-reporting
   trap in unmatched/MgPachinko2_Traps.cpp. */
void func_ov006_0210076c(void *c, int i);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_pachinko2_state_hits;
static unsigned g_pachinko2_state_missing;

/* One line per DISTINCT missing address, so a state that is wanted every frame
   does not bury the report. Four addresses can reach this, and which four is
   named in the header. */
static void lkt_no_body(unsigned code)
{
    static unsigned seen[8];
    static unsigned n;
    ++g_pachinko2_state_missing;
    for (unsigned i = 0; i < n; ++i)
        if (seen[i] == code)
            return;
    if (n < sizeof seen / sizeof seen[0])
        seen[n++] = code;
    std::fprintf(stderr, "  [mg] dScMgPachinko2_c state 0x%08x has NO "
                 "DECOMPILED BODY (no delink block, no src TU). Reported, "
                 "never called.\n", code);
    std::fflush(stderr);
}

extern "C" int port_mg_try_pachinko2_1(void *self, unsigned code, int a)
{
    ++g_pachinko2_state_hits;
    switch (code) {
    /* data_ov006_021426cc, the 2-entry loop at +0x5634 */
    case 0x020fff54u: func_ov006_020fff54(self, a); return 1;
    case 0x020ffde4u: func_ov006_020ffde4(self, a); return 1;
    /* slots 2 and 3 have no source at all; see the header */
    case 0x020ffb54u: lkt_no_body(0x020ffb54u); return 1;
    case 0x020ff8c8u: lkt_no_body(0x020ff8c8u); return 1;
    case 0x020ff690u: func_ov006_020ff690(self, a); return 1;

    /* data_ov006_021426f4, the 0x30-entry loop at +0x4698 */
    case 0x02102f3cu: func_ov006_02102f3c(self, a); return 1;
    /* slot 1 has no source at all */
    case 0x02102fe8u: lkt_no_body(0x02102fe8u); return 1;
    case 0x02103360u: func_ov006_02103360(self, a); return 1;
    case 0x02103608u: func_ov006_02103608(self, a); return 1;
    case 0x0210371cu: func_ov006_0210371c(self, a); return 1;
    case 0x02103870u: func_ov006_02103870(self, a); return 1;
    case 0x0210397cu: func_ov006_0210397c(self, a); return 1;
    case 0x02103994u: func_ov006_02103994(self, a); return 1;

    /* data_ov006_02142734, the 3-entry loop at +0x5294 */
    /* slot 0 has no source at all */
    case 0x02102274u: lkt_no_body(0x02102274u); return 1;
    case 0x021020c4u: func_ov006_021020c4(self, a); return 1;
    case 0x02101e88u: func_ov006_02101e88(self, a); return 1;
    case 0x02101af0u: func_ov006_02101af0(self, a); return 1;
    /* slot 4, the word the sibling class's phantom slot 36 would have
       clobbered. See the header. */
    case 0x021019e0u: func_ov006_021019e0(self, a); return 1;
    case 0x021016ecu: func_ov006_021016ec(self, a); return 1;
    case 0x021012ccu: func_ov006_021012cc(self, a); return 1;
    case 0x02101224u: func_ov006_02101224(self, a); return 1;
    case 0x02101088u: func_ov006_02101088(self, a); return 1;
    case 0x02100f7cu: func_ov006_02100f7c(self, a); return 1;
    case 0x02100e3cu: func_ov006_02100e3c(self, a); return 1;
    case 0x02100d90u: func_ov006_02100d90(self, a); return 1;
    case 0x02100bacu: func_ov006_02100bac(self, a); return 1;
    case 0x02100b08u: func_ov006_02100b08(self, a); return 1;
    case 0x021009b8u: func_ov006_021009b8(self, a); return 1;

    default: --g_pachinko2_state_hits;          return 0;
    }
}

/* THIS CLASS FIRST, THEN THE FRAMEWORK. See the header for why the order is
   safe in both directions. */
static void lkt_call1(void *self, unsigned code, int adj, int a)
{
    if (code == 0)
        return;                       /* the ROM's own null-pmf guard */
    if (adj == 0 && port_mg_try_pachinko2_1(self, code, a))
        return;
    port_mg_call1(self, code, adj, a);
}

extern "C" void port_mg_pachinko2_state_counts(unsigned *hits, unsigned *missing)
{
    if (hits)    *hits    = g_pachinko2_state_hits;
    if (missing) *missing = g_pachinko2_state_missing;
}

// ---- the three host copies -------------------------------------------------
//
// Each is its src TU verbatim with the member-pointer call site replaced by
// lkt_call1, and with each src's private struct spelled as the byte offsets it
// resolves to. The offsets are the src's own AND the ROM's -- both were read,
// and they agree.

/* src/func_ov006_020fff84.cpp, table data_ov006_021426cc.
   Its src reads
       for (i = 0; i < 2; i++) {
           char *b = (char *)c + (i << 5);
           if (*(unsigned char *)(b + 0x5634) != 0)
               (c->*data_ov006_021426cc[*(unsigned char *)(b + 0x5635)].pmf)(i);
       }
   Note the receiver: the guard and the index are read off the PER-ENTRY base
   b, and the call is made on c. The ROM agrees -- r6 is never reassigned. */
extern "C" void func_ov006_020fff84(void *self)
{
    char *c = (char *)self;
    for (int i = 0; i < 2; i++) {
        char *b = c + (i << 5);
        if (*(unsigned char *)(b + 0x5634) != 0) {
            const MgPmf *p = &data_ov006_021426cc[*(unsigned char *)(b + 0x5635)];
            lkt_call1(c, p->code, p->adj, i);
        }
    }
}

/* src/func_ov006_0210246c.cpp, table data_ov006_02142734.
   Its src reads
       Row *rows = (Row *)self;               // Row is u8 d[0x40]
       for (int i = 0; i < 3; i++)
           if (rows[i].d[0x5294]) {
               (self->*data_ov006_02142734[rows[i].d[0x5296]])(i);
               func_ov006_0210076c(self, i);
           }
   THE ONE OF THE THREE A LINK COULD NOT HAVE NAMED: its table is declared
   inside extern "C", so the global mangles to the plain C name the mount
   already defines. The trailing call is kept because the ROM makes it
   unconditionally after every dispatch (bl 0x210076c at 0x021024c4, inside the
   guarded arm), and its callee has no body -- see the trap file. */
extern "C" void func_ov006_0210246c(void *self)
{
    char *c = (char *)self;
    for (int i = 0; i < 3; i++) {
        char *b = c + i * 0x40;
        if (*(unsigned char *)(b + 0x5294)) {
            const MgPmf *p = &data_ov006_02142734[*(unsigned char *)(b + 0x5296)];
            lkt_call1(c, p->code, p->adj, i);
            func_ov006_0210076c(c, i);
        }
    }
}

/* src/func_ov006_02102e8c.cpp, table data_ov006_021426f4.
   Its src reads
       for (i = 0; i < 0x30; i++)
           if (*(unsigned char *)((char *)c + i * 0x40 + 0x4698) != 0) {
               unsigned char k = *(unsigned char *)((char *)c + i*0x40 + 0x4699);
               (c->*data_ov006_021426f4[k].pmf)(i);
           }
   Forty-eight entries at a 0x40 stride, which is the widest of the three and
   the one that carries this class's per-object cast. */
extern "C" void func_ov006_02102e8c(void *self)
{
    char *c = (char *)self;
    for (int i = 0; i < 0x30; i++) {
        char *b = c + i * 0x40;
        if (*(unsigned char *)(b + 0x4698) != 0) {
            const MgPmf *p = &data_ov006_021426f4[*(unsigned char *)(b + 0x4699)];
            lkt_call1(c, p->code, p->adj, i);
        }
    }
}
