// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgCoin_c's seven dispatching TUs and its twenty-four state addresses.
// Run mg5, lane CCN.
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgCurling_StateDispatch.cpp. The first carries the ROM disassembly
// of the mwcc dispatch sequence and the finding that the overlay constructors
// already copy the pairs at the right stride; the second is the template this
// file follows. Neither is repeated here.
//
// ---- THE TWENTY-FOUR ADDRESSES, AND WHERE THEY COME FROM -------------------
//
// port/tools/stategen.py --sinit src/__sinit_ov006_0213014c.c --overlay 6.
// That constructor is dScMgCoin_c's own: it is the one that writes
// data_ov006_02141810, which is the table vtable slot 6 dispatches. Five
// tables, twenty-four slots, twenty-four DISTINCT addresses, no slot repeated:
//
//   table                n  arity  dispatched by
//   -------------------  -  -----  ----------------------------------------
//   data_ov006_021417b0  3    1    func_ov006_020dd2cc
//   data_ov006_021417c8  4    1    func_ov006_020dc298
//   data_ov006_021417e8  5    1    func_ov006_020dc754
//   data_ov006_02141810  6    0    func_ov006_020de69c  (vtable slot 6)
//   data_ov006_02141840  6    1    func_ov006_020ddd6c, _020de26c, _020de440
//
// THE REFUSAL AT 0x020dd0e0 IS GONE, AND A src TU LANDED FOR IT.
// The paragraph this replaces said "if the refusal ever disappears without a
// src TU landing, something lied", so here is what landed. 0x020dd0e0 is slot 0
// of data_ov006_021417b0 and it is THE TOUCH STATE: with it missing the port
// delivered every stylus tap to the DS bottom screen correctly and the minigame
// did nothing, which is exactly what a live session measured. Lane TOUCH of run
// mg5 matched it; src/func_ov006_020dd0e0.c and its delink block
// (.text start:0x020dd0e0 end:0x020dd2cc) came across in the commit before this
// one, and tools/match.py re-verified MATCH on 2004/b56 in THIS tree, at this
// size (0x1ec), before the switch case below was changed. The slot is wired to
// the real symbol now, not to an invented one, so inferred_stub_guard's rule is
// still kept: nothing here fabricates a definition.
//
// THE CALL SHAPE WAS RE-DERIVED FROM THE ROM AND NOT TAKEN ON THE MATCHING
// LANE'S WORD, because it could not see this file. func_ov006_020dd2cc's own
// bytes, at file offset 0x1d40c of extracted/overlays/overlay_0006.bin:
//
//     mov  r7, r0            r7 = the CLASS BASE, and it never changes
//     mov  r5, r7            r5 = the per-coin cursor
//     mov  r6, #0            r6 = i
//     ldr  r4, [pc, #0x48]   r4 = data_ov006_021417b0   (literal 0x021417b0)
//   loop:
//     add  r0, r5, #0x4000
//     ldrb r0, [r0, #0xad0]  state = *(u8 *)(base + i * 0x18 + 0x4ad0)
//     add  r3, r4, r0, lsl #3            stride 8, the mwcc pair
//     ldr  r1, [r3, #4] / ands r1, r1, #1 / ...  the ordinary PMF sequence
//     add  r0, r7, r1, asr #1            THIS = the class base, adjusted
//     mov  r1, r6                        ARG  = i, the loop counter
//     blx  r2
//     add  r6, r6, #1 / cmp r6, #0x18 / add r5, r5, #0x18 / blt loop
//
// So the callee is passed (class base, i) and NOT (per-coin base, i): r5 is
// only ever used to fetch the state byte. func_ov006_020dd0e0's own prologue
// agrees, at offset 0x1d220 of the same file -- it reads [r0 + 0x51c8] off the
// argument, which is a class-level field 0x51c8 past the base and far outside
// any one 0x18-byte coin record, and then does `mov r0, #0x18 / mul r4, r1, r0`
// to recover i * 0x18 for itself. Its src signature, void func_ov006_020dd0e0-
// (char *c, int i), is that pair in the same order. The host dispatch loop
// further down this file already passes port_mg_coin_call1(c, code, adj, i)
// with c the class base and i the loop index, so the case below hands those two
// straight through and no argument is invented, dropped or reordered.
//
// THE CLASS'S REMAINING FLOOR IS NOT A STATE. func_ov006_020dbe9c (0xe0) is
// the sixth callee of vtable slot 9, Render, and has no delink block and no
// src. It is not this file's business -- nothing here calls it -- and it is
// recorded in port/slice_ccn.txt. It is now the only one of the two left.
//
// ---- WHY SEVEN HOST COPIES AND NOT FOUR ------------------------------------
//
// A LINK IS NOT A COMPLETE DETECTOR FOR THIS WALL, which is the rule
// port/mg_fanout_costs.txt section 4 states and which this class demonstrates
// again at a 3-to-4 split. This lane swept the whole closure for `::*` rather
// than waiting for an LNK1120, and found fifteen TUs: the eight ov004 ones
// MG1/MG2 already host-copied, and these seven. Only four of the seven would
// ever be named by a link:
//
//   LINK-VISIBLE
//     func_ov006_020de69c   ?data_ov006_02141810@@3PAUEntry@@A
//     func_ov006_020dd2cc   ?data_ov006_021417b0@@3PAUEntry@@A
//     func_ov006_020de26c   ?data_ov006_02141840@@3PAP8Obj@@AEXH@ZA
//     func_ov006_020de440   ?data_ov006_02141840@@3PAP8C@@AEXH@ZA
//   SILENT -- the table is declared at C linkage, so it mangles as the plain
//   C name the ov006 mount already defines and the linker is satisfied while
//   the stride is wrong
//     func_ov006_020dc298   Entry table declared inside extern "C"
//     func_ov006_020dc754   Entry table declared inside extern "C"
//     func_ov006_020ddd6c   extern "C" Entry data_ov006_02141840[]
//
// Note the two PAU spellings above: an ARRAY of a struct that wraps a member
// pointer, which is the form port/tools/facegen.py's WALL test misses (its
// test is "P8" in the symbol, and the struct hides the P8; its struct-typed
// refusal tests the by-value @@3U and this is @@3PAU). Refused by hand.
//
// Note also that data_ov006_02141840 is declared with THREE different types
// across its three consumers -- Entry-wrapped at C linkage, PMF-to-Obj, and
// PMF-to-C -- so two of them emit two DIFFERENT unresolved symbols for one
// table and the third emits none. That is the clearest single statement of why
// the sweep is the detector and the link is not.
//
// NONE OF THE SEVEN PUTS A MEMBER POINTER IN AN OBJECT FIELD. That is the
// worse ov004 defect class (four bytes where the ROM has eight moves every
// field after it, so the body reads the wrong field before any dispatch
// happens). All seven of these dispatch through a table, so their struct
// layouts are correct and only the stride and the call sequence are wrong.
//
// ---- FOUR RIDE-THROUGHS, AND ALL FOUR ARE EMPTY ----------------------------
//
// func_ov006_020de5ac sits in the arity-0 table and takes (void).
// func_ov006_020dcffc, _020dc294 and _020dd7bc sit in ONE-argument slots and
// also take (void). Every one of the four is FOUR BYTES in the ROM -- the
// whole function is `bx lr` -- so there is nothing for an ignored argument to
// be wrong about, and calling them with no arguments from here is the faithful
// host form. This is deliberately not the defect class of
// hal/scene_actor_faces.cpp's veneers, where a (void) callee stood between a
// caller and a body that DID want arguments; these four have no body at all.
//
// ---- WHY THIS FILE HAS ITS OWN ENTRY POINTS --------------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and port_mg_call1 and
// chains them to exactly one per-class pair, port_mg_try_ov006_0 and _1, which
// unmatched/MgCurling_StateDispatch.cpp defines. A second class cannot define
// those names, and widening the chain means editing MG2's file, which is not
// this lane's to edit.
//
// So the host copies below call port_mg_coin_call0 / _call1, which try THIS
// class's switch and then hand everything else to the framework's entry point
// unchanged. The framework therefore remains the single place that decides
// what a null code word means, what a nonzero adjustment means, and how an
// unhandled address is reported -- which is the property its header asks for.
// Nothing about the chain's semantics moves; only one more link is added in
// front of it. Whoever generalises the framework chain can delete these two
// wrappers and add one more `try` line.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the twenty-three matched state bodies, in address order -------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   ride-through is called the way the ROM calls it rather than the way the
   slot's arity would suggest. The four (void) ones are the empty bodies the
   header names. */
void func_ov006_020dbf7c(void *sb, int i);
void func_ov006_020dc154(char *base, int idx);
void func_ov006_020dc1c4(void *thiz, int idx);
void func_ov006_020dc294(void);              /* one-argument slot, bx lr body */
void func_ov006_020dc414(char *base, int idx);
void func_ov006_020dc4b0(char *c, int i);
void func_ov006_020dc4c8(char *c, int i);
void func_ov006_020dc5c4(char *c, int i);
void func_ov006_020dc6d0(int o, int idx);
void func_ov006_020dcffc(void);              /* one-argument slot, bx lr body */
void func_ov006_020dd000(char *c, int i);
void func_ov006_020dd0e0(char *c, int i);    /* THE TOUCH STATE, see header */
void func_ov006_020dd658(char *self, int i);
void func_ov006_020dd7bc(void);              /* one-argument slot, bx lr body */
void func_ov006_020dd7c0(char *thiz, int index);
void func_ov006_020dd880(char *c, int i);
void func_ov006_020dda94(char *self, int i);
void func_ov006_020ddca0(char *c, int i);
void func_ov006_020de0e0(char *self);
void func_ov006_020de1d4(char *c);
void func_ov006_020de584(void *a);
void func_ov006_020de5ac(void);              /* zero-argument slot, bx lr body */

/* the five mount tables, re-typed to the ROM's eight-byte pair. The mount
   defines the storage; __sinit_ov006_0213014c fills it at scene load. */
extern MgPmf data_ov006_021417b0[];
extern MgPmf data_ov006_021417c8[];
extern MgPmf data_ov006_021417e8[];
extern MgPmf data_ov006_02141810[];
extern MgPmf data_ov006_02141840[];

/* the ordinary callees the host copies below keep, each spelled as its own src
   TU spells it */
void _ZN5Sound12PlayBank2_2DEj(unsigned int);
void func_ov006_020dde28(void *c, int index);
void func_ov006_020dca04(void *c);
void func_ov006_020dc348(void *c);
void func_ov006_020dc900(void *c);
void func_ov006_020dce3c(void *c);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
void func_ov004_020ae274(void *c);
void func_ov004_020adb1c(int arg);

/* THE NAME-SPELLING VARIANT src/func_ov006_020de1d4.c and
   src/func_ov006_020de26c.cpp use for ov004's data_ov004_020beb68 -- a name
   that exists in no config, the same shape src/func_ov006_020e3578.c's bare
   `func_020adc74` has and which port/mg_fanout_costs.txt section 6 records as
   the ordinary kind of spelling defect. Declared here exactly as the two src
   TUs declare it so the host copy below keeps their reference. */
extern char *func_020beb68;

/* host-copied further down this file, and called from above their own
   definitions -- by the address switch (020de26c and 020de440 are STATE
   BODIES as well as dispatchers) and by the Behavior host copy. */
void func_ov006_020dd2cc(void *c);
void func_ov006_020ddd6c(char *thiz);
void func_ov006_020dc298(void *self);
void func_ov006_020dc754(void *self);
void func_ov006_020de26c(char *self);
void func_ov006_020de440(char *c);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_coin_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND NOW STRUCTURALLY ZERO. Every state
   address in the two switches below reaches a real symbol, so nothing
   increments this any more and hal/scene_mg.cpp's census reports 0 for it. It
   is kept rather than deleted so that census field keeps its meaning: if a
   later lane ever adds a state this class cannot reach, this is where it is
   counted, and a nonzero reading is a regression rather than a new field. */
static unsigned g_coin_floor_hits;
/* How many times the touch state actually RAN. The count the field above used
   to carry, now that the call goes somewhere. */
static unsigned g_coin_touch_calls;

static int coin_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x020de0e0u: func_ov006_020de0e0(c); return 1;
    case 0x020de1d4u: func_ov006_020de1d4(c); return 1;
    case 0x020de26cu: func_ov006_020de26c(c); return 1;   /* host copy below */
    case 0x020de440u: func_ov006_020de440(c); return 1;   /* host copy below */
    case 0x020de584u: func_ov006_020de584(c); return 1;
    case 0x020de5acu: func_ov006_020de5ac();  return 1;   /* (void) body */
    default:                                  return 0;
    }
}

static int coin_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_021417b0 */
    /* THE TOUCH STATE. (class base, coin index), the order func_ov006_020dd2cc
       passes them in the ROM; see the header for the disassembly. */
    case 0x020dd0e0u: ++g_coin_touch_calls;
                      func_ov006_020dd0e0(c, a);  return 1;
    case 0x020dd000u: func_ov006_020dd000(c, a);  return 1;
    case 0x020dcffcu: func_ov006_020dcffc();      return 1;  /* (void) body */
    /* data_ov006_021417c8 */
    case 0x020dc294u: func_ov006_020dc294();      return 1;  /* (void) body */
    case 0x020dc1c4u: func_ov006_020dc1c4(c, a);  return 1;
    case 0x020dc154u: func_ov006_020dc154(c, a);  return 1;
    case 0x020dbf7cu: func_ov006_020dbf7c(c, a);  return 1;
    /* data_ov006_021417e8 */
    case 0x020dc6d0u: func_ov006_020dc6d0((int)(size_t)c, a); return 1;
    case 0x020dc5c4u: func_ov006_020dc5c4(c, a);  return 1;
    case 0x020dc4c8u: func_ov006_020dc4c8(c, a);  return 1;
    case 0x020dc4b0u: func_ov006_020dc4b0(c, a);  return 1;
    case 0x020dc414u: func_ov006_020dc414(c, a);  return 1;
    /* data_ov006_02141840 */
    case 0x020ddca0u: func_ov006_020ddca0(c, a);  return 1;
    case 0x020dda94u: func_ov006_020dda94(c, a);  return 1;
    case 0x020dd880u: func_ov006_020dd880(c, a);  return 1;
    case 0x020dd7c0u: func_ov006_020dd7c0(c, a);  return 1;
    case 0x020dd7bcu: func_ov006_020dd7bc();      return 1;  /* (void) body */
    case 0x020dd658u: func_ov006_020dd658(c, a);  return 1;
    default:                                      return 0;
    }
}

/* The two entry points the host copies below use. See the header for why this
   class needs its own rather than defining port_mg_try_ov006_0/_1: those two
   names are curling's, and the framework chain that reaches them is MG2's file
   to widen, not this lane's. Everything this switch does not own goes to the
   framework unchanged, so the null-code guard, the nonzero-adjustment refusal
   and the UNHANDLED report all still live in exactly one place. */
extern "C" void port_mg_coin_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && coin_try_0(self, code)) {
        ++g_coin_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_coin_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0 && coin_try_1(self, code, a)) {
        ++g_coin_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" unsigned port_mg_coin_state_hits(void)
{
    return g_coin_state_hits;
}

extern "C" unsigned port_mg_coin_floor_hits(void)
{
    return g_coin_floor_hits;
}

extern "C" unsigned port_mg_coin_touch_calls(void)
{
    return g_coin_touch_calls;
}

// ---- the seven host copies -------------------------------------------------
//
// Each is its src TU verbatim except for the table declaration (MgPmf rather
// than a member-pointer type) and the dispatch site (port_mg_coin_callN rather
// than `(c->*table[i].pmf)()`). Nothing else moves.

/* src/func_ov006_020de69c.cpp -- dScMgCoin_c::Behavior, vtable SLOT 6, and the
   one this class's half of the wall is about. Its src reads
       struct C; typedef void (C::*PMF)();
       struct Entry { PMF pmf; };  extern Entry data_ov006_02141810[];
       int idx = self->unk_51c8;
       (c->*data_ov006_02141810[idx].pmf)();
   The state index is at +0x51c8, which the ROM reads as
   `add r0,r4,#0x5000 / ldr r0,[r0,#0x1c8]` and which slot 18
   (func_ov006_020de5b0) zeroes at the same offset. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCoin_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_020de69c(void *self)
{
    char *c = (char *)self;
    int idx = *(int *)(c + 0x51c8);
    const MgPmf *e = &data_ov006_02141810[idx];
    port_mg_coin_call0(c, e->code, e->adj);
    func_ov006_020dc754(c);
    func_ov006_020dc298(c);
    func_ov006_020dc900(c);
    func_ov006_020dce3c(c);
    return 1;
}

/* src/func_ov006_020dc298.cpp. SILENT: its Entry table was declared inside
   extern "C", so the link never named it. The guard byte is +0x51bc and the
   index +0x51bf, which is what its src struct's pad/guard/pad2/idx layout
   spells. The slot is a ONE-argument one and the ROM passes a constant zero. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCoin_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020dc298(void *self)
{
    char *c = (char *)self;
    if (*(unsigned char *)(c + 0x51bc) == 0)
        return;
    int j = *(unsigned char *)(c + 0x51bf);
    const MgPmf *p = &data_ov006_021417c8[j];
    port_mg_coin_call1(c, p->code, p->adj, 0);
}

/* src/func_ov006_020dc754.cpp. SILENT for the same reason. Guard +0x51a0,
   index +0x51a1. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCoin_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020dc754(void *self)
{
    char *c = (char *)self;
    if (*(unsigned char *)(c + 0x51a0) == 0)
        return;
    int j = *(unsigned char *)(c + 0x51a1);
    const MgPmf *p = &data_ov006_021417e8[j];
    port_mg_coin_call1(c, p->code, p->adj, 0);
}

/* src/func_ov006_020dd2cc.cpp. Twenty-four elements of stride 0x18, the state
   byte at +0x4ad0 of each. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCoin_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020dd2cc(void *self)
{
    char *c = (char *)self;
    for (int i = 0; i < 0x18; i++) {
        char *base = c + i * 0x18;
        unsigned char k = *(unsigned char *)(base + 0x4ad0);
        const MgPmf *p = &data_ov006_021417b0[k];
        port_mg_coin_call1(c, p->code, p->adj, i);
    }
}

/* src/func_ov006_020ddd6c.cpp. SILENT: `extern "C" Entry data_ov006_02141840[]`.
   Forty elements of stride 0x1c; the live flag at +0x4677, the state byte at
   +0x4675. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCoin_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020ddd6c(char *thiz)
{
    int n = 0;
    char *p = thiz;
    for (int i = 0; i < 0x28; i++) {
        if (*(unsigned char *)(p + 0x4000 + 0x677) != 0) {
            const MgPmf *e =
                &data_ov006_02141840[*(unsigned char *)(p + 0x4000 + 0x675)];
            port_mg_coin_call1(thiz, e->code, e->adj, i);
            if (*(unsigned char *)(p + 0x4000 + 0x675) != 4)
                n++;
            func_ov006_020dde28(thiz, i);
        }
        p += 0x1c;
    }
    if (n != 0)
        return;
    *(int *)(thiz + 0x5000 + 0x1c8) = 2;
    *(unsigned char *)(thiz + 0x4000 + 0xd13) = 1;
    _ZN5Sound12PlayBank2_2DEj(0x151);
}

/* THE NAME-SPELLING VARIANT src/func_ov006_020de584.c uses for the TU above:
   a bare `func_020ddd6c`, a name that exists in no config. Defined here rather
   than aliased because the host copy is what has to be reached, and an
   /alternatename whose left-hand side is also a defined symbol is defeated
   silently -- the class port/tools/alternatename_guard.py exists to catch. It
   forwards its one argument explicitly, so it is correct whether MSVC compiles
   it as a call or as a jmp. */
extern "C" void func_020ddd6c(void *c)
{
    func_ov006_020ddd6c((char *)c);
}

/* src/func_ov006_020de26c.cpp. ALSO a state body: slot 3 of
   data_ov006_02141810. Its src declares `extern PMF data_ov006_02141840[];`
   with PMF a pointer-to-member-of-Obj, which is one of the two link-visible
   spellings of that one table. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCoin_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020de26c(char *self)
{
    int count = 0;
    char *r5 = self;
    for (int i = 0; i < 0x28; i++) {
        if (*(unsigned char *)(r5 + 0x4677)) {
            int idx = *(unsigned char *)(r5 + 0x4675);
            const MgPmf *p = &data_ov006_02141840[idx];
            port_mg_coin_call1(self, p->code, p->adj, i);
            if (*(unsigned char *)(r5 + 0x4675) != 4) count++;
            if (*(unsigned char *)(r5 + 0x4676) != 0) func_ov006_020dde28(self, i);
        }
        r5 += 0x1c;
    }
    char *r2 = self;
    for (int j = 0; j < 0x18; j++) {
        if (*(unsigned char *)(r2 + 0x4ad0) == 1 && *(unsigned char *)(r2 + 0x4ad1) != 0)
            count++;
        r2 += 0x18;
    }
    func_ov006_020dd2cc(self);
    func_ov006_020dca04(self);
    if (count != 0) return;

    if (((int *)(self + 0x5000))[0x73] != 0) {
        *(int *)(self + 0x51cc) -= 1;
        if (((int *)(self + 0x5000))[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] == 0)
            func_ov006_020dc348(self);
        if (((int *)(self + 0x5000))[0x73] == 0x20 && ((unsigned char *)(self + 0x5000))[0x1df] != 0) {
            func_ov004_020b0cac(6, 0x80, -0x80, -1, -1, 0xd);
            func_ov004_020ae274(0);
        }
        if (((int *)(self + 0x5000))[0x73] <= 0) ((int *)(self + 0x5000))[0x73] = 0;
        return;
    }
    {
        char *g = func_020beb68;
        func_ov004_020adb1c(g != 0 ? *(int *)(g + 0xa8) : 0);
    }
    ((int *)(self + 0x5000))[0x73] = 0x70;
    ((int *)(self + 0x5000))[0x72] = 4;
}

/* src/func_ov006_020de440.cpp. ALSO a state body: slot 2 of
   data_ov006_02141810. Its src declares the SAME table as
   `extern PMF data_ov006_02141840[];` with PMF a pointer-to-member-of-C, so it
   emits a DIFFERENT unresolved symbol for the same storage than the TU above
   does -- which is the clearest evidence in this class that the link names
   spellings and not tables. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgCoin_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020de440(char *c)
{
    char *p = c;
    for (int i = 0; i < 0x28; i++, p += 0x1c) {
        if (*(unsigned char *)(p + 0x4677) != 0) {
            const MgPmf *e =
                &data_ov006_02141840[*(unsigned char *)(p + 0x4675)];
            port_mg_coin_call1(c, e->code, e->adj, i);
            func_ov006_020dde28(c, i);
        }
    }
    func_ov006_020dd2cc(c);
    func_ov006_020dca04(c);
    if (*(int *)(c + 0x51c8) == 3)
        return;
    {
        int found = 0;
        char *q = c;
        for (int j = 0; j < 0x18; j++, q += 0x18) {
            if (*(unsigned char *)(q + 0x4ad3) != 0) {
                if (*(unsigned char *)(q + 0x4ad5) == 0) {
                    found++;
                    break;
                }
            }
        }
        if (found != 0)
            return;
    }
    *(int *)(c + 0x51c8) = 3;
    *(int *)(c + 0x51cc) = 0x40;
    *(unsigned char *)(c + 0x51df) = 1;
    {
        int t;
        char *g = func_020beb68;
        if (g != 0)
            t = *(int *)(g + 0xa8);
        else
            t = 0;
        if (t > *(int *)(c + 0x51d4))
            *(unsigned char *)(c + 0x51db) = 1;
        else
            *(unsigned char *)(c + 0x51db) = 0;
    }
}
