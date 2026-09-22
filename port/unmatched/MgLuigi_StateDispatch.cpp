// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgLuigi_c's six dispatching TUs and its twenty-three state addresses.
// Run mg5, lane LUI. Actor id 0x16e = scene 366, the "Wanted!" minigame.
//
// Read unmatched/MgBase_StateDispatch.cpp's header first and then
// unmatched/MgCurling_StateDispatch.cpp, which is the template this file is
// cut from. The ROM disassembly of the dispatch sequence, the measurement that
// every adjustment word in this family is zero, and the finding that the
// overlay constructors already copy the pairs at the right stride are all
// there and are not repeated here.
//
// ---- WHY THIS FILE DOES NOT DEFINE port_mg_try_ov006_0/1 ------------------
//
// MgCurling_StateDispatch.cpp defines those two names and
// MgBase_StateDispatch.cpp's port_mg_call0/1 call them directly. A second
// class defining them is a duplicate symbol, so the fan-out cannot simply copy
// the template's entry points -- and widening the framework file to know about
// every class would put five concurrent lanes in one function.
//
// So this class owns its own entry, port_mg_luigi_call0/1, which its six host
// copies call. It tries this class's switch and DELEGATES A MISS to the
// framework's port_mg_call0/1, which still applies the null-code guard, the
// nonzero-adjustment refusal, dScMgBase_c's own four addresses and the
// UNHANDLED report. Nothing in MgBase_StateDispatch.cpp or in curling's file
// is edited. The one visible consequence is in the counters: a state this
// class handles increments THIS file's counter and not the framework's
// g_mg_dispatch_calls, and a miss increments the framework's. Read
// port_mg_luigi_state_hits() and port_mg_dispatch_counts() together.
//
// ---- THE TWENTY-THREE ADDRESSES, AND WHERE THEY COME FROM -----------------
//
// port/tools/stategen.py --sinit src/__sinit_ov006_02130f64.c --overlay 6,
// which reads the constructor's own assignments in source order and resolves
// each .data pair out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0. Five tables, twenty-three slots, twenty-three DISTINCT
// addresses, every adjustment word zero. REFUSALS: one, below.
//
//   table               n  arity  state addresses (in slot order)
//   ------------------- -  -----  ------------------------------------------
//   data_ov006_021421ec 3    1    020f002c  020effb8  020eff20
//   data_ov006_02142204 3    1    020f0bf0  020f0ba0  020f05d8
//   data_ov006_0214221c 3    1    020f0a6c  020f088c  020f06fc
//   data_ov006_02142234 4    0    020f3260  020f31dc  020f319c  020f300c
//   data_ov006_02142254 10   1    020f1e58  020f1e40  020f1cb4  020f1b98
//                                 020f1a70  020f192c  020f17fc  020f15ac
//                                 020f13cc  020f1318
//
// 3+3+3+4+10 = 23. Twenty-two have a matched src TU in port/slice_lui.txt.
// The twenty-third is the hole below.
//
// TWO OF THE TWENTY-THREE ARE THEMSELVES DISPATCHERS and are host-copied in
// this file rather than sliced: 020f0ba0 (slot 1 of data_ov006_02142204) and
// 020f300c (slot 3 of data_ov006_02142234). The switch calls the host copies,
// which is the only definition of those symbols in the build.
//
// ---- _ZN12dScMgLuigi_c15MovePictureSwayEi IS A HARD FLOOR AND IS NOT TRANSCRIBED -----------
//
// Slot 7 of data_ov006_02142254. No delink block in
// config/arm9/overlays/ov006/delinks.txt and no src file defines it, and the
// ONLY relocation in the whole overlay that names the address is the mwcc pair
// load at 0x0213cd8c -- so it is reached as a state and by nothing else.
// stategen.py reports it under REFUSALS.
//
// Lane CT1 transcribed curling's equivalent (func_ov006_020e1854) from the ROM
// and this lane deliberately does NOT do the same. Transcribing is a real
// piece of work with its own verification, and a lane that does it silently
// while seating a class turns "the class boots" into a claim about a body
// nobody reviewed. The case below reports which state was wanted and calls
// nothing, which is what MgCurling_StateDispatch.cpp did before CT1.
//
// ---- _ZN12dScMgLuigi_c16PlaceNextPictureEv IS A SECOND FLOOR, AND A NAME TRAP ---------------
//
// 0x528 bytes, called eight times in a loop by src/actors/dScMgLuigi_c.cpp,
// which is state slot 1 of data_ov006_02142234 and IS sliced. No delink block
// and no src file. config/arm9/overlays/ov006/symbols.txt DOES name it, which
// is exactly the shape port/mg_fanout_costs.txt section 3 records as having
// produced a wrong nosrc column once already: a name-shaped lookup calls this
// decompiled.
//
// AND src/func_ov002_020f2790.c EXISTS. ov002 and ov006 are not co-resident
// and both own bytes at 0x020f2790; that file is a different function in a
// different overlay. Seating it here would be the ov043/ov047 mistake.
//
// So it is a named trap, taking the one int argument its matched caller
// declares and passes.

#include <cstdio>
#include <cstdlib>   /* std::abort, for the boot installer below (lane PMFB4) */

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the five state tables, re-typed ------------------------------------
   The ov006 mount defines the storage and __sinit_ov006_02130f64 fills it.
   Every one of the five is spelled here as an array of two plain ints, which
   is eight bytes on MSVC and eight bytes in the ROM -- section 4's rulebook
   corollary, and the reason the mount's tables were correct all along. */
extern MgPmf data_ov006_021421ec[];
extern MgPmf data_ov006_02142204[];
extern MgPmf data_ov006_0214221c[];
extern MgPmf data_ov006_02142234[];
extern MgPmf data_ov006_02142254[];

/* ---- the twenty-two matched state bodies, in address order --------------
   Reached ONLY through the switch below: the pair words are mounted DATA
   holding DS addresses, so nothing else in the build names them and /OPT:REF
   would drop every one of them without this file. They join
   port/slice_lui.txt in the same commit. */
void _ZN12dScMgLuigi_c8IrisStopEi(char *c, int i);
void _ZN12dScMgLuigi_c8IrisGrowEi(char *c, int i);
void _ZN12dScMgLuigi_c8IrisHoldEi(char *c, int i);
void _ZN12dScMgLuigi_c16CurtainStateSlowEi(char *c, int i);
void _ZN12dScMgLuigi_c16CurtainStateFastEi(char *c, int i);
void _ZN12dScMgLuigi_c16CurtainStateHoldEi(char *c, int i);
void _ZN12dScMgLuigi_c18CurtainStateBounceEi(char *c, int i);
void _ZN12dScMgLuigi_c12CurtainStartEi(char *c, int i);
void _ZN12dScMgLuigi_c16TickPictureFlashEi(char *c, int i);
void _ZN12dScMgLuigi_c17MovePictureBounceEi(char *c, int i);
/* Run mg5, lane INTEG: slot 7 of data_ov006_02142254, recovered on branch
   decomp/wanted-layout; was a floor, now a real dispatch below. */
void _ZN12dScMgLuigi_c15MovePictureSwayEi(char *c, int i);
void _ZN12dScMgLuigi_c22MovePictureDriftRandomEi(char *c, int i);
void _ZN12dScMgLuigi_c21MovePictureDriftByRowEi(char *c, int i);
void _ZN12dScMgLuigi_c24MovePictureDriftByColumnEi(char *c, int i);
void _ZN12dScMgLuigi_c23MovePictureDriftByLevelEi(char *c, int i);
void _ZN12dScMgLuigi_c21MovePictureDriftFixedEi(char *c, int i);
void _ZN12dScMgLuigi_c11StopPictureEi(char *c, int i);
void _ZN12dScMgLuigi_c14RestartPictureEi(char *c, int i);
void _ZN12dScMgLuigi_c9StatePlayEv(char *c);
void _ZN12dScMgLuigi_c10StatePlaceEv(int self);   /* src takes `int this`, not a pointer */
void _ZN12dScMgLuigi_c10StateSetupEv(char *o);

/* the ordinary callees the host copies below keep, spelled as their own src
   TUs spell them so the faces they already have keep their references */
void _ZN12dScMgLuigi_c15UpdatePenaltiesEv(void *q);
void _ZN12dScMgLuigi_c12UpdateRewardEv(void *s);
void func_ov004_020b0a54(int c);

}  /* extern "C" */

// ---- the two floors --------------------------------------------------------

/* Run mg5, lane INTEG: _ZN12dScMgLuigi_c16PlaceNextPictureEv (the "Wanted!" round-setup driver)
   was recovered on branch decomp/mg-luigi-state as src/actors/dScMgLuigi_c.cpp
   (void(char*)) together with the scripted-layout routine _ZN12dScMgLuigi_c16PlaceBoardLayoutEv it
   calls (branch decomp/wanted-layout), both now in port/slice_lui.txt, so the
   trap that src/actors/dScMgLuigi_c.cpp linked against is gone and the real body
   runs. src/func_ov002_020f2790.c is a different function in a non-co-resident
   overlay and is not in this build. The counter is kept because
   port_mg_luigi_counts reports it; it now stays 0. */
static unsigned g_luigi_floor_hits;

// ---- this class's address switch -------------------------------------------

static unsigned g_luigi_state_hits;
static unsigned g_luigi_state_floor;

/* The two dispatchers that are also states. _ZN12dScMgLuigi_c13CurtainUpdateEi is still the
   host copy defined below; run link100 lane PMFB4 retired _ZN12dScMgLuigi_c10StateCatchEv,
   so this declaration now names src/actors/dScMgLuigi_c.cpp, at the same C
   linkage and the same one-pointer signature. */
extern "C" void _ZN12dScMgLuigi_c13CurtainUpdateEi(void *c, int i);
extern "C" void _ZN12dScMgLuigi_c10StateCatchEv(char *o);

/* the boot installer at the end of this file; hal/scene_mg.cpp calls it after
   the ov006 constructors have filled the tables. */
/* run link100 lane SEAT4: this class's remaining state tables are
   seated in port/hal/pmf_seat4.cpp, from inside this installer, so the
   seat order hal/scene_mg.cpp already establishes is the one they get
   and no new call site is added anywhere. */
extern "C" void port_pmf_seat4_luigi(void);

extern "C" void port_mg_luigi_states_seat(void);

/* Run mg5, lane INTEG: slot 7 of data_ov006_02142254 (_ZN12dScMgLuigi_c15MovePictureSwayEi) was
   recovered on branch decomp/wanted-layout and is now sliced and dispatched
   below, so the floor reporter is gone. g_luigi_state_floor is kept because
   port_mg_luigi_counts reports it; it now stays 0. */

static int luigi_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142234, the arity-0 table slot 6 dispatches */
    case 0x020f3260u: _ZN12dScMgLuigi_c10StateSetupEv(c);              return 1;
    case 0x020f31dcu: _ZN12dScMgLuigi_c10StatePlaceEv((int)(size_t)c); return 1;
    case 0x020f319cu: _ZN12dScMgLuigi_c9StatePlayEv(c);              return 1;
    case 0x020f300cu: _ZN12dScMgLuigi_c10StateCatchEv(c);              return 1;
    default:                                               return 0;
    }
}

static int luigi_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_021421ec */
    case 0x020f002cu: _ZN12dScMgLuigi_c8IrisHoldEi(c, a); return 1;
    case 0x020effb8u: _ZN12dScMgLuigi_c8IrisGrowEi(c, a); return 1;
    case 0x020eff20u: _ZN12dScMgLuigi_c8IrisStopEi(c, a); return 1;
    /* data_ov006_02142204 */
    case 0x020f0bf0u: _ZN12dScMgLuigi_c12CurtainStartEi(c, a); return 1;
    case 0x020f0ba0u: _ZN12dScMgLuigi_c13CurtainUpdateEi(c, a); return 1;
    case 0x020f05d8u: _ZN12dScMgLuigi_c16CurtainStateSlowEi(c, a); return 1;
    /* data_ov006_0214221c */
    case 0x020f0a6cu: _ZN12dScMgLuigi_c18CurtainStateBounceEi(c, a); return 1;
    case 0x020f088cu: _ZN12dScMgLuigi_c16CurtainStateHoldEi(c, a); return 1;
    case 0x020f06fcu: _ZN12dScMgLuigi_c16CurtainStateFastEi(c, a); return 1;
    /* data_ov006_02142254 */
    case 0x020f1e58u: _ZN12dScMgLuigi_c14RestartPictureEi(c, a); return 1;
    case 0x020f1e40u: _ZN12dScMgLuigi_c11StopPictureEi(c, a); return 1;
    case 0x020f1cb4u: _ZN12dScMgLuigi_c21MovePictureDriftFixedEi(c, a); return 1;
    case 0x020f1b98u: _ZN12dScMgLuigi_c23MovePictureDriftByLevelEi(c, a); return 1;
    case 0x020f1a70u: _ZN12dScMgLuigi_c24MovePictureDriftByColumnEi(c, a); return 1;
    case 0x020f192cu: _ZN12dScMgLuigi_c21MovePictureDriftByRowEi(c, a); return 1;
    case 0x020f17fcu: _ZN12dScMgLuigi_c22MovePictureDriftRandomEi(c, a); return 1;
    case 0x020f13ccu: _ZN12dScMgLuigi_c17MovePictureBounceEi(c, a); return 1;
    case 0x020f1318u: _ZN12dScMgLuigi_c16TickPictureFlashEi(c, a); return 1;
    /* run mg5 lane INTEG seated this, so it is a real dispatch now */
    case 0x020f15acu: _ZN12dScMgLuigi_c15MovePictureSwayEi(c, a); return 1;
    default:                                     return 0;
    }
}

/* THE TWO ENTRY POINTS this class's host copies call. A hit is this class's; a
   miss falls through to the framework, which owns the guards and the report. */
extern "C" void port_mg_luigi_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && luigi_try_0(self, code)) {
        ++g_luigi_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_luigi_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0 && luigi_try_1(self, code, a)) {
        ++g_luigi_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" void port_mg_luigi_counts(unsigned *hits, unsigned *floor,
                                     unsigned *nosrc)
{
    if (hits)  *hits  = g_luigi_state_hits;
    if (floor) *floor = g_luigi_state_floor;
    if (nosrc) *nosrc = g_luigi_floor_hits;
}

// ---- the two host copies that are left --------------------------------------
//
// SIX at run mg5, TWO after run link100 lane PMFB4 seated three of the five
// tables. Each is its src TU with the pointer-to-member declaration replaced by
// MgPmf and the dispatch replaced by port_mg_luigi_call0/1. Everything else is
// verbatim. EVERY FIELD OFFSET BELOW IS THE ROM'S, read off the disassembly of
// the body rather than off the src struct, because a src struct that contains
// a member pointer lays out differently on MSVC. Neither of these two holds one
// -- all five tables are external -- so the src offsets and the ROM's agree,
// and each is stated next to its body so the check is visible rather than
// assumed. The four that went are named after them.

/* src/_ZN12dScMgLuigi_c10UpdateIrisEv -- RETIRED, run link100 lane SEAT4. Its table is
   seated in port/hal/pmf_seat4.cpp and the matched TU is on
   port/slice_seat4.txt, so the host copy that stood in for it is gone and
   the declaration above is what the faces in this file reach. */

/* src/_ZN12dScMgLuigi_c13CurtainUpdateEi -- RETIRED, run link100 lane SEAT4. Its table is
   seated in port/hal/pmf_seat4.cpp and the matched TU is on
   port/slice_seat4.txt, so the host copy that stood in for it is gone and
   the declaration above is what the faces in this file reach. */

// ---- FOUR HOST COPIES ARE RETIRED, TWO ARE LEFT ----------------------------
//
// Run link100 lane PMFB4 seated THREE of this class's five tables, so the four
// dispatchers that read them compile from their own matched TUs
// (port/slice_pmfb4.txt) and the host copies of them are gone:
//
//   _ZN12dScMgLuigi_c12CurtainsDoneEv  data_ov006_02142204   src/actors/dScMgLuigi_c.cpp
//   _ZN12dScMgLuigi_c8BehaviorEv  data_ov006_02142234   src/actors/dScMgLuigi_c.cpp
//   _ZN12dScMgLuigi_c14UpdatePicturesEv  data_ov006_02142254   src/actors/dScMgLuigi_c.cpp
//   _ZN12dScMgLuigi_c10StateCatchEv  data_ov006_02142254   src/actors/dScMgLuigi_c.cpp
//
// TWO TABLES ARE NOT SEATED, so their two dispatchers keep their host copies
// above: data_ov006_021421ec (_ZN12dScMgLuigi_c10UpdateIrisEv) and data_ov006_0214221c
// (_ZN12dScMgLuigi_c13CurtainUpdateEi). luigi_try_1 therefore stays live for those two tables'
// six addresses. luigi_try_0 is now unreachable -- data_ov006_02142234 was the
// only arity-0 table and it is seated -- and is kept rather than deleted for
// the same reason MgBomroom keeps its dead cases: the switch is the written
// record of which address belongs to which table.
//
// TWO OF THE FOUR ARE THEMSELVES STATE BODIES, and both keep working:
//   _ZN12dScMgLuigi_c10StateCatchEv is slot 3 of data_ov006_02142234, so the seat installs
//     a face that calls it -- and it is the matched TU now, which dispatches
//     data_ov006_02142254 straight through the seated cells.
//   _ZN12dScMgLuigi_c13CurtainUpdateEi is slot 1 of data_ov006_02142204 and is still a host
//     copy, so the seat installs a face that calls THAT copy.

// ---- THE SEVENTEEN FACES AND THE BOOT INSTALLER ----------------------------
//
// THE STRIDE, BOTH SIDES, PER ROW -- measured on this tree
// (runs/link100/out/PMFB4/rom_gate1.txt and emit_gate1.txt):
//
//   dispatcher            table                ROM                     emitted
//   --------------------  -------------------  ----------------------  --------
//   _ZN12dScMgLuigi_c12CurtainsDoneEv   data_ov006_02142204  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020f0dd4 = 02142204                   [eax*8+4]
//   _ZN12dScMgLuigi_c8BehaviorEv   data_ov006_02142234  add r3,r2,r1,lsl #3     [edx*8]
//                           pool 020f345c = 02142234                   [edx*8+4]
//   _ZN12dScMgLuigi_c14UpdatePicturesEv   data_ov006_02142254  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020f1ef4 = 02142254                   [eax*8+4]
//   _ZN12dScMgLuigi_c10StateCatchEv   data_ov006_02142254  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020f3190 = 02142254                   [eax*8+4]
//
// so ROM 8 == emitted 8 on all four. /Zp4 IS A MEASURED NO-OP on every one:
// each was compiled under the port's own flags both ways for this lane and the
// two /FAsc listings came back identical except for the TITLE line naming the
// .obj, so the option is not claimed.
//
// THE SEVENTEEN SOURCE PAIRS ALL READ {code, 0} in overlay_0006.bin at the
// addresses src/__sinit_ov006_02130f64.c copies each slot from, every one a
// WHOLE-PAIR copy (`data_ov006_TAB.pN = data_ov006_SRC;`) with no field-form
// fill anywhere:
//
//   02142204[0] <- 0213ce3c 020f0bf0/0    02142254[0] <- 0213cdcc 020f1e58/0
//   02142204[1] <- 0213cdbc 020f0ba0/0    02142254[1] <- 0213cddc 020f1e40/0
//   02142204[2] <- 0213ce14 020f05d8/0    02142254[2] <- 0213cd94 020f1cb4/0
//   02142234[0] <- 0213cdd4 020f3260/0    02142254[3] <- 0213cde4 020f1b98/0
//   02142234[1] <- 0213ce24 020f31dc/0    02142254[4] <- 0213cda4 020f1a70/0
//   02142234[2] <- 0213ce44 020f319c/0    02142254[5] <- 0213cd9c 020f192c/0
//   02142234[3] <- 0213ce4c 020f300c/0    02142254[6] <- 0213cdb4 020f17fc/0
//                                         02142254[7] <- 0213cd8c 020f15ac/0
//                                         02142254[8] <- 0213ce1c 020f13cc/0
//                                         02142254[9] <- 0213cdc4 020f1318/0
//
// THE DISPATCH SHAPE, read off each row's own listing and not off the census:
// all four emit `mov ecx, tab[i*8+4] / mov eax, tab[i*8] / add ecx, <this> /
// call eax` with NO `add esp,N` afterwards -- receiver in ecx, callee cleanup.
// 020f0d58, 020f1e90 and 020f300c push ONE argument before the call
// (`push edi`, `push esi`, `push esi`); 020f3414 pushes NOTHING. So:
//
//   arity 1: data_ov006_02142204, data_ov006_02142254
//   arity 0: data_ov006_02142234
//
// THREE /alternatename DIRECTIVES. src/actors/dScMgLuigi_c.cpp,
// src/actors/dScMgLuigi_c.cpp and src/actors/dScMgLuigi_c.cpp declare their
// tables at namespace scope, so MSVC spells the references
// ?data_ov006_02142204@@3PAUEntry@@A, ?data_ov006_02142234@@3PAUEntry@@A and
// ?data_ov006_02142254@@3PAUEntry@@A -- all three read off the objects with
// dumpbin /symbols. src/actors/dScMgLuigi_c.cpp declares the SAME table
// data_ov006_02142254 inside its own extern "C" block and comes in as the plain
// _data_ov006_02142254, which is the pair its own header calls "the silent one":
// the two spellings of one table, one aliased and one not, both resolving to the
// mount's storage.
//
// SAFE UNDER port/tools/alternatename_guard.py for hal/pmfc_aliases.cpp's
// reason: each LHS is a C++ decoration only these matched TUs ever spell.
#pragma comment(linker, "/alternatename:?data_ov006_02142204@@3PAUEntry@@A=_data_ov006_02142204")
#pragma comment(linker, "/alternatename:?data_ov006_02142234@@3PAUEntry@@A=_data_ov006_02142234")
#pragma comment(linker, "/alternatename:?data_ov006_02142254@@3PAUEntry@@A=_data_ov006_02142254")

/* EVERY FACE COUNTS. A face is reachable only from a table word this file's own
   seat wrote, so g_luigi_state_hits keeps counting exactly the dispatches that
   happened -- the number port_mg_luigi_callN counted while the host copies
   routed through the switch, and the number port_mg_luigi_counts reports. */
#define LG_FACE1(sym, cast)                                                   \
    static void __fastcall lg_##sym(void *self, void *dead_edx, int i)        \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_luigi_state_hits;                                                 \
        sym(cast self, i);                                                    \
    }

/* data_ov006_02142234 pushes nothing: receiver in ecx, callee cleans zero. */
#define LG_FACE0(sym, cast)                                                   \
    static void __fastcall lg_##sym(void *self, void *dead_edx)               \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_luigi_state_hits;                                                 \
        sym(cast self);                                                       \
    }

/* data_ov006_02142204, arity 1. Slot 1 is the host copy above. */
LG_FACE1(_ZN12dScMgLuigi_c12CurtainStartEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c13CurtainUpdateEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c16CurtainStateSlowEi, (char *))
/* data_ov006_02142234, arity 0. Slot 1's src takes `int this`, not a pointer;
   slot 3 is _ZN12dScMgLuigi_c10StateCatchEv, which is the matched TU now. */
LG_FACE0(_ZN12dScMgLuigi_c10StateSetupEv, (char *))
LG_FACE0(_ZN12dScMgLuigi_c10StatePlaceEv, (int)(size_t))
LG_FACE0(_ZN12dScMgLuigi_c9StatePlayEv, (char *))
LG_FACE0(_ZN12dScMgLuigi_c10StateCatchEv, (char *))
/* data_ov006_02142254, arity 1 */
LG_FACE1(_ZN12dScMgLuigi_c14RestartPictureEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c11StopPictureEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c21MovePictureDriftFixedEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c23MovePictureDriftByLevelEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c24MovePictureDriftByColumnEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c21MovePictureDriftByRowEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c22MovePictureDriftRandomEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c15MovePictureSwayEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c17MovePictureBounceEi, (char *))
LG_FACE1(_ZN12dScMgLuigi_c16TickPictureFlashEi, (char *))

extern "C" void port_mg_luigi_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    port_pmf_seat4_luigi();

    static const struct {
        MgPmf *table;
        const char *name;
        unsigned slot;
        unsigned rom;
        void *face;
    } seats[] = {
        {data_ov006_02142204, "02142204", 0, 0x020f0bf0u, (void *)lg__ZN12dScMgLuigi_c12CurtainStartEi},
        {data_ov006_02142204, "02142204", 1, 0x020f0ba0u, (void *)lg__ZN12dScMgLuigi_c13CurtainUpdateEi},
        {data_ov006_02142204, "02142204", 2, 0x020f05d8u, (void *)lg__ZN12dScMgLuigi_c16CurtainStateSlowEi},

        {data_ov006_02142234, "02142234", 0, 0x020f3260u, (void *)lg__ZN12dScMgLuigi_c10StateSetupEv},
        {data_ov006_02142234, "02142234", 1, 0x020f31dcu, (void *)lg__ZN12dScMgLuigi_c10StatePlaceEv},
        {data_ov006_02142234, "02142234", 2, 0x020f319cu, (void *)lg__ZN12dScMgLuigi_c9StatePlayEv},
        {data_ov006_02142234, "02142234", 3, 0x020f300cu, (void *)lg__ZN12dScMgLuigi_c10StateCatchEv},

        {data_ov006_02142254, "02142254", 0, 0x020f1e58u, (void *)lg__ZN12dScMgLuigi_c14RestartPictureEi},
        {data_ov006_02142254, "02142254", 1, 0x020f1e40u, (void *)lg__ZN12dScMgLuigi_c11StopPictureEi},
        {data_ov006_02142254, "02142254", 2, 0x020f1cb4u, (void *)lg__ZN12dScMgLuigi_c21MovePictureDriftFixedEi},
        {data_ov006_02142254, "02142254", 3, 0x020f1b98u, (void *)lg__ZN12dScMgLuigi_c23MovePictureDriftByLevelEi},
        {data_ov006_02142254, "02142254", 4, 0x020f1a70u, (void *)lg__ZN12dScMgLuigi_c24MovePictureDriftByColumnEi},
        {data_ov006_02142254, "02142254", 5, 0x020f192cu, (void *)lg__ZN12dScMgLuigi_c21MovePictureDriftByRowEi},
        {data_ov006_02142254, "02142254", 6, 0x020f17fcu, (void *)lg__ZN12dScMgLuigi_c22MovePictureDriftRandomEi},
        {data_ov006_02142254, "02142254", 7, 0x020f15acu, (void *)lg__ZN12dScMgLuigi_c15MovePictureSwayEi},
        {data_ov006_02142254, "02142254", 8, 0x020f13ccu, (void *)lg__ZN12dScMgLuigi_c17MovePictureBounceEi},
        {data_ov006_02142254, "02142254", 9, 0x020f1318u, (void *)lg__ZN12dScMgLuigi_c16TickPictureFlashEi},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        MgPmf *p = &seats[i].table[seats[i].slot];
        if (p->code != seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgLuigi_c state table %s slot %u: "
                         "the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].slot, p->code, p->adj, seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats[i].face;
    }
}
