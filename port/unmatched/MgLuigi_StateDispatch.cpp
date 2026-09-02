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

/* the two host-copied dispatchers that are also states; defined below */
extern "C" void func_ov006_020f0ba0(void *c, int i);
extern "C" void func_ov006_020f300c(char *o);

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

// ---- the six host copies ---------------------------------------------------
//
// Each is its src TU with the pointer-to-member declaration replaced by MgPmf
// and the dispatch replaced by port_mg_luigi_call0/1. Everything else is
// verbatim. EVERY FIELD OFFSET BELOW IS THE ROM'S, read off the disassembly of
// the body rather than off the src struct, because a src struct that contains
// a member pointer lays out differently on MSVC. None of these six holds one
// -- all five tables are external -- so the src offsets and the ROM's agree,
// and each is stated next to its body so the check is visible rather than
// assumed.

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

/* src/func_ov006_020f0d58.cpp, table data_ov006_02142204, one argument.
   ROM 0x020f0d58: two iterations, stride 0x18, guard at +0x47b4, index at
   +0x47b6, r1 = the loop counter, pool 0x020f0dd4 = 02142204; returns
   (count == 0). */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgLuigi_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_020f0d58(void *p)
{
    char *base = (char *)p;
    int count = 0;
    char *e = base;
    for (int i = 0; i < 2; ++i, e += 0x18) {
        if (*(unsigned char *)(e + 0x47b4) == 0)
            continue;
        unsigned k = *(unsigned char *)(e + 0x47b6);
        const MgPmf *q = &data_ov006_02142204[k];
        port_mg_luigi_call1(base, q->code, q->adj, i);
        ++count;
    }
    return count == 0;
}

/* src/func_ov006_020f1e90.cpp, table data_ov006_02142254, one argument.
   ROM 0x020f1e90: 0x78 iterations at stride 1, guard at +0x52ed + i, index at
   +0x51fd + i, r1 = i, pool 0x020f1ef4 = 02142254. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgLuigi_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020f1e90(void *p)
{
    char *base = (char *)p;
    for (int i = 0; i < 0x78; ++i) {
        char *b = base + i;
        if (*(unsigned char *)(b + 0x52ed) == 0)
            continue;
        unsigned k = *(unsigned char *)(b + 0x51fd);
        const MgPmf *q = &data_ov006_02142254[k];
        port_mg_luigi_call1(base, q->code, q->adj, i);
    }
}

/* src/func_ov006_020f300c.cpp. THE SILENT ONE: its src declares
   `extern "C" PMF data_ov006_02142254[];`, so the global mangles as the plain
   C name the mount already defines and no link this port has ever run named
   it, while the body strides the eight-byte table by four. Found by sweeping
   the slice for `::*`.
   ALSO state slot 3 of data_ov006_02142234, which is why luigi_try_0 calls it.
   ROM 0x020f300c, 0x190 bytes, verified statement for statement: the three
   leading calls, the 0x78 loop with the x == 9 guard, the second 0x78 loop
   clearing +0x53dd (pool 0x020f3194 = 000053DD), the +0x516a countdown (pool
   0x020f3198 = 0000516A), and the virtual call.
   THE VIRTUAL CALL IS SLOT 18. ROM: ldr r2,[r0]; ldr r2,[r2,#0x48]; mvn
   r1,#0; blx r2 -- 0x48/4 = 18, which is this class's own state-reset slot,
   func_ov006_020f3294, and the fill has already put a host thunk there. It is
   made here through a struct of nineteen virtuals so MSVC emits its
   __thiscall shape, which is what hal/scene_mg.cpp's __fastcall thunks
   answer; unmatched/MgBase_Slot1.cpp uses the same device for the same
   reason. */
namespace {
struct LuigiVt {
    virtual void v00(); virtual void v01(); virtual void v02();
    virtual void v03(); virtual void v04(); virtual void v05();
    virtual void v06(); virtual void v07(); virtual void v08();
    virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14();
    virtual void v15(); virtual void v16(); virtual void v17();
    virtual void v18(int);
};
}

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgLuigi_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020f300c(char *o)
{
    func_ov006_020f0044(o);
    func_ov006_020f10ec(o);
    func_ov006_020f0274(o);

    int matches = 0;
    for (int i = 0; i < 0x78; ++i) {
        if (*(unsigned char *)(o + i + 0x52ed) == 0)
            continue;
        int x = *(unsigned char *)(o + i + 0x51fd);
        if (x != 9)
            continue;
        ++matches;
        const MgPmf *q = &data_ov006_02142254[x];
        port_mg_luigi_call1(o, q->code, q->adj, i);
    }
    if (matches != 0)
        return;

    for (int i = 0; i < 0x78; ++i) {
        unsigned char *p = (unsigned char *)(o + i + 0x53dd);
        if (*p == 1)
            *p = 0;
    }

    if (*(unsigned short *)(o + 0x516a) == 0)
        return;
    *(unsigned short *)(o + 0x5164) = 1;
    *(unsigned short *)(o + 0x516a) = (unsigned short)(*(unsigned short *)(o + 0x516a) - 1);
    if (*(short *)(o + 0x516a) > 0)
        return;
    *(unsigned short *)(o + 0x516a) = 0;
    *(unsigned short *)(o + 0x5164) = 0;

    if (*(unsigned char *)(o + 0x5459) != 0) {
        int v = *(int *)(o + 0xbc);
        while (v >= 5)
            v -= 5;
        if (v != 4)
            ((LuigiVt *)o)->v18(-1);
        else
            func_ov004_020b0a54(1);
    } else {
        func_ov004_020b0a54(0x12);
    }
    *(unsigned char *)(o + 0xc3) = 0;
}

/* src/func_ov006_020f3414.cpp -- dScMgLuigi_c::Behavior, vtable slot 6, and
   the class's arity-0 dispatch. ROM 0x020f3414: add r1,r0,#0x4000; ldr
   r1,[r1,#0xf78] -- the state index at +0x4f78, which slot 18
   (func_ov006_020f3294) zeroes at the same offset, so the two cross-confirm
   each other -- then the five-instruction ARM Itanium sequence, pool
   0x020f345c = 02142234. Returns 1. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgLuigi_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_020f3414(void *p)
{
    char *c = (char *)p;
    int j = *(int *)(c + 0x4f78);
    const MgPmf *e = &data_ov006_02142234[j];
    port_mg_luigi_call0(c, e->code, e->adj);
    return 1;
}
