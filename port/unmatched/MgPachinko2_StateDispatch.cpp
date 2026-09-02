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
// STATEGEN REFUSED FOUR OF THE TWENTY-EIGHT, AND RUN mg7 LANE L369 CLOSED ALL
// FOUR. They were genuine decomp holes -- a config symbol, no delink block in
// config/arm9/overlays/ov006/delinks.txt, and no src file in either extension:
//
//   0x020ffb54  slot 2 of data_ov006_021426cc   0x290   MATCHED  1.2/sp2p3
//   0x020ff8c8  slot 3 of data_ov006_021426cc   0x28c   MATCHED  1.2/sp2p3
//   0x02102274  slot 0 of data_ov006_02142734   0x1f8   MATCHED  1.2/sp2p3
//   0x02102fe8  slot 1 of data_ov006_021426f4   0x378   NONMATCHING 2004/b56
//
// All four are src TUs on port/slice_lkt.txt now and all four are CALLED by
// the switch below, so this file no longer reports a single address. The three
// matched ones carry delink blocks; 0x02102fe8 ships with the honest banner
// and its per-instruction diff pasted in the source, which is what the tree
// already does for src/func_ov006_02103ac0.c in this same class.
//
// WHAT DID NOT CHANGE: no symbol is invented for anything here. The four
// definitions are decompilations of the ROM's own bodies under their real
// func_ov006_ names, not port_ host copies, which is why they are slice lines
// and not another file in this directory.
//
// THE MISSING COUNTER STAYS, AND READS ZERO ON PURPOSE. lkt_no_body is gone,
// but g_pachinko2_state_missing is kept and still printed by
// hal/scene_mg.cpp: a run that states the zero is evidence, and a report that
// stopped counting is not.
//
// THE NUMBERS THIS WAVE MOVED, scene 369 under SM64DS_FAULTS_FATAL=1:
//   3000 frames  BEFORE  12367 routed, 5714 of them bodiless
//                AFTER   12321 routed, 0 bodiless, 0 UNHANDLED
//    300 frames  BEFORE   1480 routed,  598 of them bodiless
//                AFTER    1480 routed, 0 bodiless, 0 UNHANDLED
// The routed total moves because the real bodies advance the machine
// differently from a reporting case that returned without doing anything --
// which is the point of the wave, not noise in it.
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

/* ---- ALL TWENTY-EIGHT state bodies, in address order ---------------------
   Twenty-four of these were here from the seat; the four run mg7 lane L369
   decompiled (0x020ff8c8, 0x020ffb54, 0x02102274, 0x02102fe8) are marked
   below. All twenty-eight take (self, index) in their own src -- checked one
   by one,
   not assumed from the table's arity -- so none of them is the ARM
   ride-through curling met at func_ov006_020e2eb8. They are declared with
   void* here rather than with each src's own struct type: these are C-linkage
   symbols, the declaration cannot change the mangling, and a host copy that
   repeated two dozen private struct definitions would be two dozen more places
   for a layout to drift. */
void func_ov006_020ff8c8(void *c, int i);   /* mg7 lane L369 */
void func_ov006_020ff690(void *c, int i);
void func_ov006_020ffb54(void *c, int i);   /* mg7 lane L369 */
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
void func_ov006_02102274(void *c, int i);   /* mg7 lane L369 */
void func_ov006_02102f3c(void *c, int i);
void func_ov006_02102fe8(void *c, int i);   /* mg7 lane L369 */
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

/* the one ordinary callee a host copy below keeps, from its own src. Run mg7
   lane L369 decompiled it (0x24c, NONMATCHING at 2004/b56 with the diff pasted
   in src/func_ov006_0210076c.c), so unmatched/MgPachinko2_Traps.cpp -- which
   existed for this one symbol and nothing else -- is DELETED. A trap standing
   beside a real definition is a duplicate symbol, not a safety net. */
void func_ov006_0210076c(void *c, int i);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_pachinko2_state_hits;
static unsigned g_pachinko2_state_missing;

/* g_pachinko2_state_missing is write-never as of run mg7 lane L369: every one
   of the twenty-eight addresses below calls a real body, so lkt_no_body is
   gone. The counter is kept, and kept printed by hal/scene_mg.cpp, because a
   run that states the zero is evidence and a report that stopped counting is
   not.

   THE PER-ADDRESS CENSUS is the other half of that evidence and it is new with
   the same wave. A total says a machine dispatched; only a per-slot breakdown
   says a machine MOVED. Before this wave data_ov006_02142734 spent every tick
   of every live entry in slot 0 -- the reporting case that returned without
   doing anything -- and a run that printed 1480 routed looked exactly like a
   run that worked. The three rows below are printed in slot order, so a table
   sitting on one slot is visible at a glance. */
static unsigned g_lkt_cc[5];      /* data_ov006_021426cc, 2-entry loop  */
static unsigned g_lkt_f4[8];      /* data_ov006_021426f4, 0x30-entry    */
static unsigned g_lkt_34[15];     /* data_ov006_02142734, 3-entry loop  */

static void lkt_census(unsigned *row, unsigned n, unsigned i)
{
    if (i < n) ++row[i];
}

extern "C" int port_mg_try_pachinko2_1(void *self, unsigned code, int a)
{
    ++g_pachinko2_state_hits;
    switch (code) {
    /* data_ov006_021426cc, the 2-entry loop at +0x5634 */
    case 0x020fff54u: func_ov006_020fff54(self, a); lkt_census(g_lkt_cc, 5, 0); return 1;
    case 0x020ffde4u: func_ov006_020ffde4(self, a); lkt_census(g_lkt_cc, 5, 1); return 1;
    /* slots 2 and 3, the two Lakitu carriers. Run mg7 lane L369, both
       byte-matched at 1.2/sp2p3. */
    case 0x020ffb54u: func_ov006_020ffb54(self, a); lkt_census(g_lkt_cc, 5, 2); return 1;
    case 0x020ff8c8u: func_ov006_020ff8c8(self, a); lkt_census(g_lkt_cc, 5, 3); return 1;
    case 0x020ff690u: func_ov006_020ff690(self, a); lkt_census(g_lkt_cc, 5, 4); return 1;

    /* data_ov006_021426f4, the 0x30-entry loop at +0x4698 */
    case 0x02102f3cu: func_ov006_02102f3c(self, a); lkt_census(g_lkt_f4, 8, 0); return 1;
    /* slot 1, the stylus driver. Run mg7 lane L369, NONMATCHING at 2004/b56
       (register naming only; the diff is pasted in the source). */
    case 0x02102fe8u: func_ov006_02102fe8(self, a); lkt_census(g_lkt_f4, 8, 1); return 1;
    case 0x02103360u: func_ov006_02103360(self, a); lkt_census(g_lkt_f4, 8, 2); return 1;
    case 0x02103608u: func_ov006_02103608(self, a); lkt_census(g_lkt_f4, 8, 3); return 1;
    case 0x0210371cu: func_ov006_0210371c(self, a); lkt_census(g_lkt_f4, 8, 4); return 1;
    case 0x02103870u: func_ov006_02103870(self, a); lkt_census(g_lkt_f4, 8, 5); return 1;
    case 0x0210397cu: func_ov006_0210397c(self, a); lkt_census(g_lkt_f4, 8, 6); return 1;
    case 0x02103994u: func_ov006_02103994(self, a); lkt_census(g_lkt_f4, 8, 7); return 1;

    /* data_ov006_02142734, the 3-entry loop at +0x5294 */
    /* slot 0, selected on EVERY tick of every live entry: the respawn timer
       and the launch setup. Run mg7 lane L369, byte-matched at 1.2/sp2p3. */
    case 0x02102274u: func_ov006_02102274(self, a); lkt_census(g_lkt_34, 15, 0); return 1;
    case 0x021020c4u: func_ov006_021020c4(self, a); lkt_census(g_lkt_34, 15, 1); return 1;
    case 0x02101e88u: func_ov006_02101e88(self, a); lkt_census(g_lkt_34, 15, 2); return 1;
    case 0x02101af0u: func_ov006_02101af0(self, a); lkt_census(g_lkt_34, 15, 3); return 1;
    /* slot 4, the word the sibling class's phantom slot 36 would have
       clobbered. See the header. */
    case 0x021019e0u: func_ov006_021019e0(self, a); lkt_census(g_lkt_34, 15, 4); return 1;
    case 0x021016ecu: func_ov006_021016ec(self, a); lkt_census(g_lkt_34, 15, 5); return 1;
    case 0x021012ccu: func_ov006_021012cc(self, a); lkt_census(g_lkt_34, 15, 6); return 1;
    case 0x02101224u: func_ov006_02101224(self, a); lkt_census(g_lkt_34, 15, 7); return 1;
    case 0x02101088u: func_ov006_02101088(self, a); lkt_census(g_lkt_34, 15, 8); return 1;
    case 0x02100f7cu: func_ov006_02100f7c(self, a); lkt_census(g_lkt_34, 15, 9); return 1;
    case 0x02100e3cu: func_ov006_02100e3c(self, a); lkt_census(g_lkt_34, 15, 10); return 1;
    case 0x02100d90u: func_ov006_02100d90(self, a); lkt_census(g_lkt_34, 15, 11); return 1;
    case 0x02100bacu: func_ov006_02100bac(self, a); lkt_census(g_lkt_34, 15, 12); return 1;
    case 0x02100b08u: func_ov006_02100b08(self, a); lkt_census(g_lkt_34, 15, 13); return 1;
    case 0x021009b8u: func_ov006_021009b8(self, a); lkt_census(g_lkt_34, 15, 14); return 1;

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

/* One line per table, in slot order. Run mg7 lane L369. */
extern "C" void port_mg_pachinko2_state_census(void)
{
    static const struct { const char *name; const unsigned *row; unsigned n; } rows[] = {
        { "data_ov006_021426cc", g_lkt_cc, 5  },
        { "data_ov006_021426f4", g_lkt_f4, 8  },
        { "data_ov006_02142734", g_lkt_34, 15 },
    };
    for (unsigned r = 0; r < 3; ++r) {
        unsigned total = 0, live = 0;
        for (unsigned i = 0; i < rows[r].n; ++i) {
            total += rows[r].row[i];
            live  += (rows[r].row[i] != 0);
        }
        std::printf("[scene] dScMgPachinko2_c %s: %u dispatch(es) over %u of "
                    "%u slots  [", rows[r].name, total, live, rows[r].n);
        for (unsigned i = 0; i < rows[r].n; ++i)
            std::printf("%s%u", i ? " " : "", rows[r].row[i]);
        std::printf("]\n");
    }
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
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPachinko2_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
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
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPachinko2_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
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
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPachinko2_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
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
