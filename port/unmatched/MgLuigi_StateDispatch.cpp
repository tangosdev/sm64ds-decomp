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
// ---- func_ov006_020f15ac IS A HARD FLOOR AND IS NOT TRANSCRIBED -----------
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
// ---- func_ov006_020f2790 IS A SECOND FLOOR, AND A NAME TRAP ---------------
//
// 0x528 bytes, called eight times in a loop by src/func_ov006_020f31dc.c,
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
void func_ov006_020eff20(char *c, int i);
void func_ov006_020effb8(char *c, int i);
void func_ov006_020f002c(char *c, int i);
void func_ov006_020f05d8(char *c, int i);
void func_ov006_020f06fc(char *c, int i);
void func_ov006_020f088c(char *c, int i);
void func_ov006_020f0a6c(char *c, int i);
void func_ov006_020f0bf0(char *c, int i);
void func_ov006_020f1318(char *c, int i);
void func_ov006_020f13cc(char *c, int i);
/* Run mg5, lane INTEG: slot 7 of data_ov006_02142254, recovered on branch
   decomp/wanted-layout; was a floor, now a real dispatch below. */
void func_ov006_020f15ac(char *c, int i);
void func_ov006_020f17fc(char *c, int i);
void func_ov006_020f192c(char *c, int i);
void func_ov006_020f1a70(char *c, int i);
void func_ov006_020f1b98(char *c, int i);
void func_ov006_020f1cb4(char *c, int i);
void func_ov006_020f1e40(char *c, int i);
void func_ov006_020f1e58(char *c, int i);
void func_ov006_020f319c(char *c);
void func_ov006_020f31dc(int self);   /* src takes `int this`, not a pointer */
void func_ov006_020f3260(char *o);

/* the ordinary callees the host copies below keep, spelled as their own src
   TUs spell them so the faces they already have keep their references */
void func_ov006_020f10ec(void *q);
void func_ov006_020f0274(void *s);
void func_ov004_020b0a54(int c);

}  /* extern "C" */

// ---- the two floors --------------------------------------------------------

/* Run mg5, lane INTEG: func_ov006_020f2790 (the "Wanted!" round-setup driver)
   was recovered on branch decomp/mg-luigi-state as src/func_ov006_020f2790.c
   (void(char*)) together with the scripted-layout routine func_ov006_020f2224 it
   calls (branch decomp/wanted-layout), both now in port/slice_lui.txt, so the
   trap that src/func_ov006_020f31dc.c linked against is gone and the real body
   runs. src/func_ov002_020f2790.c is a different function in a non-co-resident
   overlay and is not in this build. The counter is kept because
   port_mg_luigi_counts reports it; it now stays 0. */
static unsigned g_luigi_floor_hits;

// ---- this class's address switch -------------------------------------------

static unsigned g_luigi_state_hits;
static unsigned g_luigi_state_floor;

/* The two dispatchers that are also states. func_ov006_020f0ba0 is still the
   host copy defined below; run link100 lane PMFB4 retired func_ov006_020f300c,
   so this declaration now names src/func_ov006_020f300c.cpp, at the same C
   linkage and the same one-pointer signature. */
extern "C" void func_ov006_020f0ba0(void *c, int i);
extern "C" void func_ov006_020f300c(char *o);

/* the boot installer at the end of this file; hal/scene_mg.cpp calls it after
   the ov006 constructors have filled the tables. */
extern "C" void port_mg_luigi_states_seat(void);

/* Run mg5, lane INTEG: slot 7 of data_ov006_02142254 (func_ov006_020f15ac) was
   recovered on branch decomp/wanted-layout and is now sliced and dispatched
   below, so the floor reporter is gone. g_luigi_state_floor is kept because
   port_mg_luigi_counts reports it; it now stays 0. */

static int luigi_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142234, the arity-0 table slot 6 dispatches */
    case 0x020f3260u: func_ov006_020f3260(c);              return 1;
    case 0x020f31dcu: func_ov006_020f31dc((int)(size_t)c); return 1;
    case 0x020f319cu: func_ov006_020f319c(c);              return 1;
    case 0x020f300cu: func_ov006_020f300c(c);              return 1;
    default:                                               return 0;
    }
}

static int luigi_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_021421ec */
    case 0x020f002cu: func_ov006_020f002c(c, a); return 1;
    case 0x020effb8u: func_ov006_020effb8(c, a); return 1;
    case 0x020eff20u: func_ov006_020eff20(c, a); return 1;
    /* data_ov006_02142204 */
    case 0x020f0bf0u: func_ov006_020f0bf0(c, a); return 1;
    case 0x020f0ba0u: func_ov006_020f0ba0(c, a); return 1;
    case 0x020f05d8u: func_ov006_020f05d8(c, a); return 1;
    /* data_ov006_0214221c */
    case 0x020f0a6cu: func_ov006_020f0a6c(c, a); return 1;
    case 0x020f088cu: func_ov006_020f088c(c, a); return 1;
    case 0x020f06fcu: func_ov006_020f06fc(c, a); return 1;
    /* data_ov006_02142254 */
    case 0x020f1e58u: func_ov006_020f1e58(c, a); return 1;
    case 0x020f1e40u: func_ov006_020f1e40(c, a); return 1;
    case 0x020f1cb4u: func_ov006_020f1cb4(c, a); return 1;
    case 0x020f1b98u: func_ov006_020f1b98(c, a); return 1;
    case 0x020f1a70u: func_ov006_020f1a70(c, a); return 1;
    case 0x020f192cu: func_ov006_020f192c(c, a); return 1;
    case 0x020f17fcu: func_ov006_020f17fc(c, a); return 1;
    case 0x020f13ccu: func_ov006_020f13cc(c, a); return 1;
    case 0x020f1318u: func_ov006_020f1318(c, a); return 1;
    /* run mg5 lane INTEG seated this, so it is a real dispatch now */
    case 0x020f15acu: func_ov006_020f15ac(c, a); return 1;
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

/* src/func_ov006_020f0044.cpp, table data_ov006_021421ec, one argument.
   ROM 0x020f0044: add r1,r0,#0x4000; ldrb r2,[r1,#0x7f4] guard; ldrb
   r1,[r1,#0x7f5] index; pool 0x020f00a0 = 021421EC; the dispatch passes
   r1 = 0, which is the src's literal (0). */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgLuigi_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020f0044(void *p)
{
    char *c = (char *)p;
    if (*(unsigned char *)(c + 0x47f4) == 0)
        return;
    unsigned j = *(unsigned char *)(c + 0x47f5);
    const MgPmf *e = &data_ov006_021421ec[j];
    port_mg_luigi_call1(c, e->code, e->adj, 0);
}

/* src/func_ov006_020f0ba0.cpp, table data_ov006_0214221c, one argument. ALSO
   state slot 1 of data_ov006_02142204, which is why luigi_try_1 calls it.
   ROM 0x020f0ba0: mov r2,#0x18; mla r2,r1,r2,r0; add r2,#0x4000; ldrb
   r2,[r2,#0x7b7]; pool 0x020f0bec = 0214221C. The ROM never writes r1 before
   the blx, so the incoming index rides through; this copy passes it. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgLuigi_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020f0ba0(void *p, int i)
{
    char *c = (char *)p;
    unsigned k = *(unsigned char *)(c + i * 0x18 + 0x47b7);
    const MgPmf *e = &data_ov006_0214221c[k];
    port_mg_luigi_call1(c, e->code, e->adj, i);
}

// ---- FOUR HOST COPIES ARE RETIRED, TWO ARE LEFT ----------------------------
//
// Run link100 lane PMFB4 seated THREE of this class's five tables, so the four
// dispatchers that read them compile from their own matched TUs
// (port/slice_pmfb4.txt) and the host copies of them are gone:
//
//   func_ov006_020f0d58  data_ov006_02142204   src/func_ov006_020f0d58.cpp
//   func_ov006_020f3414  data_ov006_02142234   src/func_ov006_020f3414.cpp
//   func_ov006_020f1e90  data_ov006_02142254   src/func_ov006_020f1e90.cpp
//   func_ov006_020f300c  data_ov006_02142254   src/func_ov006_020f300c.cpp
//
// TWO TABLES ARE NOT SEATED, so their two dispatchers keep their host copies
// above: data_ov006_021421ec (func_ov006_020f0044) and data_ov006_0214221c
// (func_ov006_020f0ba0). luigi_try_1 therefore stays live for those two tables'
// six addresses. luigi_try_0 is now unreachable -- data_ov006_02142234 was the
// only arity-0 table and it is seated -- and is kept rather than deleted for
// the same reason MgBomroom keeps its dead cases: the switch is the written
// record of which address belongs to which table.
//
// TWO OF THE FOUR ARE THEMSELVES STATE BODIES, and both keep working:
//   func_ov006_020f300c is slot 3 of data_ov006_02142234, so the seat installs
//     a face that calls it -- and it is the matched TU now, which dispatches
//     data_ov006_02142254 straight through the seated cells.
//   func_ov006_020f0ba0 is slot 1 of data_ov006_02142204 and is still a host
//     copy, so the seat installs a face that calls THAT copy.

// ---- THE SEVENTEEN FACES AND THE BOOT INSTALLER ----------------------------
//
// THE STRIDE, BOTH SIDES, PER ROW -- measured on this tree
// (runs/link100/out/PMFB4/rom_gate1.txt and emit_gate1.txt):
//
//   dispatcher            table                ROM                     emitted
//   --------------------  -------------------  ----------------------  --------
//   func_ov006_020f0d58   data_ov006_02142204  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020f0dd4 = 02142204                   [eax*8+4]
//   func_ov006_020f3414   data_ov006_02142234  add r3,r2,r1,lsl #3     [edx*8]
//                           pool 020f345c = 02142234                   [edx*8+4]
//   func_ov006_020f1e90   data_ov006_02142254  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020f1ef4 = 02142254                   [eax*8+4]
//   func_ov006_020f300c   data_ov006_02142254  add r3,r4,r0,lsl #3     [eax*8]
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
// THREE /alternatename DIRECTIVES. src/func_ov006_020f0d58.cpp,
// src/func_ov006_020f3414.cpp and src/func_ov006_020f1e90.cpp declare their
// tables at namespace scope, so MSVC spells the references
// ?data_ov006_02142204@@3PAUEntry@@A, ?data_ov006_02142234@@3PAUEntry@@A and
// ?data_ov006_02142254@@3PAUEntry@@A -- all three read off the objects with
// dumpbin /symbols. src/func_ov006_020f300c.cpp declares the SAME table
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
LG_FACE1(func_ov006_020f0bf0, (char *))
LG_FACE1(func_ov006_020f0ba0, (char *))
LG_FACE1(func_ov006_020f05d8, (char *))
/* data_ov006_02142234, arity 0. Slot 1's src takes `int this`, not a pointer;
   slot 3 is func_ov006_020f300c, which is the matched TU now. */
LG_FACE0(func_ov006_020f3260, (char *))
LG_FACE0(func_ov006_020f31dc, (int)(size_t))
LG_FACE0(func_ov006_020f319c, (char *))
LG_FACE0(func_ov006_020f300c, (char *))
/* data_ov006_02142254, arity 1 */
LG_FACE1(func_ov006_020f1e58, (char *))
LG_FACE1(func_ov006_020f1e40, (char *))
LG_FACE1(func_ov006_020f1cb4, (char *))
LG_FACE1(func_ov006_020f1b98, (char *))
LG_FACE1(func_ov006_020f1a70, (char *))
LG_FACE1(func_ov006_020f192c, (char *))
LG_FACE1(func_ov006_020f17fc, (char *))
LG_FACE1(func_ov006_020f15ac, (char *))
LG_FACE1(func_ov006_020f13cc, (char *))
LG_FACE1(func_ov006_020f1318, (char *))

extern "C" void port_mg_luigi_states_seat(void)
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
        {data_ov006_02142204, "02142204", 0, 0x020f0bf0u, (void *)lg_func_ov006_020f0bf0},
        {data_ov006_02142204, "02142204", 1, 0x020f0ba0u, (void *)lg_func_ov006_020f0ba0},
        {data_ov006_02142204, "02142204", 2, 0x020f05d8u, (void *)lg_func_ov006_020f05d8},

        {data_ov006_02142234, "02142234", 0, 0x020f3260u, (void *)lg_func_ov006_020f3260},
        {data_ov006_02142234, "02142234", 1, 0x020f31dcu, (void *)lg_func_ov006_020f31dc},
        {data_ov006_02142234, "02142234", 2, 0x020f319cu, (void *)lg_func_ov006_020f319c},
        {data_ov006_02142234, "02142234", 3, 0x020f300cu, (void *)lg_func_ov006_020f300c},

        {data_ov006_02142254, "02142254", 0, 0x020f1e58u, (void *)lg_func_ov006_020f1e58},
        {data_ov006_02142254, "02142254", 1, 0x020f1e40u, (void *)lg_func_ov006_020f1e40},
        {data_ov006_02142254, "02142254", 2, 0x020f1cb4u, (void *)lg_func_ov006_020f1cb4},
        {data_ov006_02142254, "02142254", 3, 0x020f1b98u, (void *)lg_func_ov006_020f1b98},
        {data_ov006_02142254, "02142254", 4, 0x020f1a70u, (void *)lg_func_ov006_020f1a70},
        {data_ov006_02142254, "02142254", 5, 0x020f192cu, (void *)lg_func_ov006_020f192c},
        {data_ov006_02142254, "02142254", 6, 0x020f17fcu, (void *)lg_func_ov006_020f17fc},
        {data_ov006_02142254, "02142254", 7, 0x020f15acu, (void *)lg_func_ov006_020f15ac},
        {data_ov006_02142254, "02142254", 8, 0x020f13ccu, (void *)lg_func_ov006_020f13cc},
        {data_ov006_02142254, "02142254", 9, 0x020f1318u, (void *)lg_func_ov006_020f1318},
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
