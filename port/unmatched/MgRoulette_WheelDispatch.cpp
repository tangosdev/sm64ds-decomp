// PORT_HOST_ABI. HOST COPY of src/func_ov006_02107db8.cpp, the roulette
// WHEEL's per-frame tick -- and it carries TWO of this port's known defect
// classes in eleven lines of source. Run mg11, lane RLT (dScMgRoulette_c,
// actor id 0x17f, scene 383).
//
// ---- 1. WHAT THE OBJECT IS -------------------------------------------------
//
// The wheel lives at `this + 0x530c` inside dScMgRoulette_c and it is built by
// the class's own factory, not by any overlay constructor:
// MgMushroomRoulette_Spawn runs `_ZN5ModelC1Ev(m + 0x10)` and
// `_ZN5ModelC1Ev(m + 0x60)` on it, and vtable slot 0's
// func_ov006_021085c0 gives those two Models NitroFS files 0x21d and 0x21e
// through ModelBase::SetFile. So +0x10 and +0x60 are HOST C++ Model objects
// with MSVC vtables, and offset 0 is an mwcc member-pointer pair. Both facts
// are what this file is about.
//
// Its callers are vtable slot 6 (the Behavior, func_ov006_02109aac, at
// 0x0210a120 -- the ONLY arm_call relocation in the whole overlay that reaches
// 0x02107db8) and nothing else.
//
// ---- 2. THE MEMBER POINTER, AND ITS UNIVERSE IS TWO -----------------------
//
// src/func_ov006_02107db8.cpp is the ONE `::*` TU in this class's whole code
// block (0x0210788c..0x0210a4ac, 41 symbols, all with a matched src file). It
// spells
//
//     struct C { PMF pmf; char pad8[0x10 - 8]; Obj obj; };
//     (c->*(c->pmf))();
//
// and both halves are wrong on MSVC. The dispatch is the ARM Itanium sequence
// mwcc emits and MSVC does not, and the `pad8[0x10 - 8]` is only correct at
// EIGHT bytes -- at MSVC's four-byte member pointer `obj` lands at 0xc instead
// of 0x10, so even the Model dispatch in section 3 would come off the wrong
// address before anything else went wrong. The ROM, out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0:
//
//     02107e28  ldr   r1,[r4,#4]          the adjustment
//     02107e2c  ands  r0,r1,#1            the virtual bit, in the LSB
//     02107e30  add   r0,r4,r1,asr #1     this, advanced, arithmetic shift
//     02107e34  ldrne r2,[r0] / ldrne r1,[r4] / ldrne r1,[r2,r1]
//     02107e40  ldreq r1,[r4]
//     02107e44  blx   r1
//
// THE PAIR UNIVERSE IS EXACTLY TWO CODE WORDS, and it is bounded from the
// RELOCATION SET rather than from a reading. In this class's whole .data span
// (0x0213e2dc..0x0213e42c) exactly THREE data words carry a relocation into the
// class's own code, on top of the 36 vtable slots: the SpawnInfo's factory word
// at 0x0213e2f0, and
//
//     data_ov006_0213e2e8  { 0x021082fc, 0 }   installed by
//                                              func_ov006_02108508, which slot
//                                              18's func_ov006_02108524 calls
//     data_ov006_0213e2f8  { 0x02107ea8, 0 }   installed by
//                                              func_ov006_021082c4, which state
//                                              0x021082fc itself calls
//
// Both adjustments are zero. The two writers are the ONLY relocations anywhere
// in ov006 that name either pair (from:0x02108520, inside
// func_ov006_02108508's literal pool, and from:0x021082f8, inside
// func_ov006_021082c4's), so the wheel's state machine is a two-state machine:
// slot 18 arms it at 0x021082fc (spin up and decelerate) and that state hands
// over to 0x02107ea8 (settle onto a pocket) exactly once per round.
//
// NEITHER WRITER NEEDS A HOST COPY, and that is the mg_fanout_costs section 4
// rulebook corollary applied rather than a shortcut. Both spell the pair as two
// plain ints -- `struct S { int w[2]; }` in func_ov006_02108508 and
// `struct Pair { int a, b; }` in func_ov006_021082c4 -- which is eight bytes on
// both machines, so the COPY is already right and only the CONSUMER was wrong.
// Both stay in port/slice_rlt.txt.
//
// THE STORED PAIR IS NOT REWRITTEN. This file routes at the DISPATCH SITE, the
// way section 4 prescribes: the mounted .data keeps the ROM's own words, so
// anything that ever compares the pair by value still sees what the ROM sees.
//
// AND THE ROM HAS NO NULL GUARD HERE. Unlike func_ov006_020c19d0, which opens
// with its own `cmp r2,#0`, 0x02107db8 decodes and `blx`es unconditionally. The
// port routes through port_mg_call0, whose first statement is the framework's
// `if (code == 0) return;`, which is a divergence in the safe direction and a
// visible one: the call and hit counts below are printed side by side, so a run
// in which the pair was never armed reads as calls with no hits rather than as
// silence.
//
// ---- 3. THE SHADOW CLASS, AND IT IS THE DANGEROUS KIND --------------------
//
// src also spells
//
//     struct Obj { virtual void v0(); ... virtual void v3(); };
//     c->obj.v3();
//
// which is a LOCAL SHADOW over the Model at +0x10. port/mg_fanout_costs.txt
// section 13 states the test: a shadow over an object whose vptr is the MOUNTED
// ROM TABLE is correct, because the port's fills key on a ROM word and leave the
// table in ROM word order; a shadow over a HOST C++ vtable -- Model, ModelAnim,
// BlendModelAnim, built by hal/cxxname_bridge.cpp in MSVC order -- is wrong.
// This is the second kind. The ROM:
//
//     02107dcc  add   r0,r4,#0x10          the Model
//     02107de8  ldr   r1,[r0]              its vptr
//     02107dec  ldr   r1,[r1,#0xc]         ROM slot 3
//     02107df0  blx   r1
//
// Byte +0xc is the FOURTH word of an Itanium vtable, and include/Model.h reads
// _ZTV5Model out of the ROM as (0 D1, 1 D0, 2 DoSetFile, 3 UpdateVerts,
// 4 Virtual10, 5 Render), so ROM slot 3 is Model::UpdateVerts(). On the host
// MSVC folds D1 and D0 into one slot, so the fourth MSVC word is Virtual10 --
// one slot along, taking a `Matrix4x3 &` nobody passed and returning `ret 4`
// off a stack the caller never pushed. That is the exact pair of failures
// port/unmatched/MgMemory2_ShadowSlot_020c06dc.cpp measured on scene 363, and
// its header has the fault dump.
//
// THE DISPATCH STAYS VIRTUAL rather than qualified, for
// MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
// (ldr vptr / ldr [vptr,#0xc] / blx), and a qualified call would silently
// devirtualise a subclass override. Here the object IS exactly a Model by
// construction (the factory calls _ZN5ModelC1Ev on it), so the virtual call
// resolves to Model::UpdateVerts either way; keeping it virtual costs nothing
// and matches the ROM's shape.
//
// A SECOND READING SAYS SLOT 3 IS UpdateVerts, independent of the vtable
// listing: the two statements around it write the wheel's spin angle into
// +0xc2 and push its negation into the object at +0x20's +0x82, and then the
// next three statements build a fresh translation matrix into the model's own
// Matrix4x3 at +0x2c (+0x1c into the Model at +0x10). Refreshing the vertex
// transform between an angle update and a matrix write is coherent; handing
// Virtual10 a matrix reference at whatever the shadow left in the register is
// not.
//
// ---- 4. WHAT CHANGED FROM src, LINE BY LINE -------------------------------
//
// The body below is src/func_ov006_02107db8.cpp verbatim except:
//
//   1. `struct C`, its `PMF` typedef and its `struct Obj` shadow are gone. The
//      layout they described is spelled with byte offsets, which is what the
//      rest of the body already used.
//   2. `c->obj.v3()` became `((Model *)(b + 0x10))->UpdateVerts()`.
//   3. `(c->*(c->pmf))()` became a read of the eight-byte pair at offset 0 and
//      a call into this class's address switch.
//   4. `extern int data_020a0e68;` was at NAMESPACE scope in the src file, so
//      MSVC mangles it and the ov006 mount's plain C definition cannot satisfy
//      it. It is declared at C linkage here instead, which is the same fix
//      without an /alternatename row -- the MgMemory2_FieldPmf.cpp precedent.
//
// Everything else -- the +0xc2 spin accumulate by the +0xc8 velocity, the
// negated angle stored through the pointer at +0x20 to its +0x82, the
// Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0), the twelve-word matrix
// copy into +0x2c, and the +0xcc sound gate with its 0x158 / 0x157 bank ids --
// is the matched source's, transcribed unchanged and checked against the
// disassembly instruction for instruction (58 instructions plus a two-word
// literal pool = 60 words = 0xf0, the size config gives).
//
// IT DEFINES func_ov006_02107db8, so src/func_ov006_02107db8.cpp is OUT of
// port/slice_rlt.txt -- listing both would be an LNK2005.
//
// WHAT THE DISPLACEMENT COSTS IS COUNTERFACTUAL AND IS MEASURED AS SUCH. This
// TU was in no slice on the base, so nothing was LOST: port/tools/linkage.py
// reads 7121/11293 before the seat and 7162/11293 after, +41 against 41 slice
// lines, and no row goes backwards. The price is the line the slice could
// otherwise have carried -- 44 rather than 41 across this file, the factory
// and the model-render sibling -- and it is paid three times in this lane.
//
// NOTHING IS OWED ON THE DECOMP SIDE. Both constructs produce the ROM's own
// instruction sequence under mwccarm; the defects are entirely the host's
// member-pointer width and vtable shape.

#include "common.h"
#include "Model.h"

extern "C" {

/* the ROM's own callees, spelled as their own src TUs spell them */
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int  Sound_PlayIfNotActive(int a, int b, int c, int d);

/* DELTA 4: at C linkage here, namespace scope in src. */
extern int data_020a0e68;

/* the framework's chain, for the null-code guard, the nonzero-adjustment
   refusal and the UNHANDLED report */
void port_mg_call0(void *self, unsigned code, int adj);

/* the wheel's two states, section 2 */
void func_ov006_021082fc(char *self);
void func_ov006_02107ea8(char *self);

void func_ov006_02107db8(void *self);

}  /* extern "C" */

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines. Each host copy declares its own; this is not a header. */
struct MgPmf { unsigned code; int adj; };

static unsigned g_rlt_wheel_calls;
static unsigned g_rlt_wheel_hits;
static unsigned g_rlt_wheel_state_hits[2];

static const unsigned kRltWheelStates[2] = { 0x021082fcu, 0x02107ea8u };

static int rlt_wheel_try(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x021082fcu: ++g_rlt_wheel_state_hits[0]; func_ov006_021082fc(c); return 1;
    case 0x02107ea8u: ++g_rlt_wheel_state_hits[1]; func_ov006_02107ea8(c); return 1;
    default:                                                               return 0;
    }
}

/* The one entry point for this class's member-pointer dispatch. Everything it
   does not own goes to the framework's chain, so the guard and the UNHANDLED
   report stay in exactly one place. */
extern "C" void port_mg_roulette_call0(void *self, unsigned code, int adj)
{
    ++g_rlt_wheel_calls;
    if (code != 0 && adj == 0 && rlt_wheel_try(self, code)) {
        ++g_rlt_wheel_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_roulette_wheel_counts(unsigned *calls, unsigned *hits,
                                              unsigned *spin, unsigned *settle)
{
    if (calls)  *calls  = g_rlt_wheel_calls;
    if (hits)   *hits   = g_rlt_wheel_hits;
    if (spin)   *spin   = g_rlt_wheel_state_hits[0];
    if (settle) *settle = g_rlt_wheel_state_hits[1];
}

extern "C" unsigned port_mg_roulette_wheel_state_addr(unsigned i)
{ return i < 2u ? kRltWheelStates[i] : 0u; }

// PORT_HOST_ABI: mwcc member-pointer dispatch and a virtual-shadow dispatch
// (ROM vtable numbering vs MSVC's), in one body.
extern "C" void func_ov006_02107db8(void *self)
{
    char *b = (char *)self;

    short *p = (short *)(b + 0xc2);
    *p = (short)(*p + *(short *)(b + 0xc8));
    *(short *)(*(int *)(b + 0x20) + 0x82) = (short)(-*(short *)(b + 0xc2));

    /* DELTA 2: src's `c->obj.v3()`, ROM slot 3 of the Model at +0x10. */
    ((Model *)(b + 0x10))->UpdateVerts();

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    *(Matrix4x3 *)(b + 0x2c) = *(Matrix4x3 *)&data_020a0e68;

    /* DELTA 3: src's `(c->*(c->pmf))()`. The pair is at offset 0 and the ROM
       has no null guard; port_mg_call0 supplies one. */
    {
        const MgPmf *pm = (const MgPmf *)b;
        port_mg_roulette_call0(b, pm->code, pm->adj);
    }

    {
        short s = *(short *)(b + 0xcc);
        if (s == 0)
            return;
        if (s == 2)
            *(int *)(b + 0xbc) =
                Sound_PlayIfNotActive(*(int *)(b + 0xbc), 2, 0x158, 0);
        else
            *(int *)(b + 0xbc) =
                Sound_PlayIfNotActive(*(int *)(b + 0xbc), 2, 0x157, 0);
    }
}
