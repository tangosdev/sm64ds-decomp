// PORT_HOST_ABI.  The mwcc POINTER-TO-MEMBER WALL for dScMgJump2_c (actor id
// 0x175, scene 373, "Bounce and Trounce"), CLASS half: vtable slot 6,
// func_ov006_020ef3e0.  Run mg11, lane BNT.
//
// ---- 1. THIS CLASS HAS NO STATE TABLE AT ALL ------------------------------
//
// Every seated minigame before this one dispatches through a TABLE that its
// overlay constructor builds -- read the __sinit's pair assignments, find the
// tables, find the TUs whose literal pools load them.  dScMgJump2_c has none.
// Its member pointer lives in an OBJECT FIELD at this+0x5004, so there is no
// table, no .bss array, and NO __sinit BLOCK NAMES ANY OF IT.  That is
// dScMg3DEsp_c's field shape (port/unmatched/Mg3DEsp_FieldPmf.cpp, run mg9
// lane PSY) as the class's WHOLE state machine rather than as an eighth
// dispatcher beside seven table ones.
//
// The consequence for the sibling trap this lane was briefed on is in
// port/slice_bnt.txt section 3, and it is the reason the attribution here is
// safe: the five pair records are told apart from 0x174's seven by their
// RELOCATIONS, not by their addresses.
//
// ---- 2. THE SHAPE, OUT OF THE ROM ----------------------------------------
//
// extracted/overlays/overlay_0006.bin at base 0x020bfec0, func_ov006_020ef3e0,
// size 0x9c -- 38 instructions plus a one-word pool = 39 = 0x9c/4:
//
//     020ef440  ldr r0,[pc,#0x30]      = 0x00005004, the FIELD offset
//     020ef444  add r3, r4, r0         &self->pmf
//     020ef448  ldr r1,[r3,#4]         the adjustment
//     020ef44c  add r0, r4, r1, asr#1  this, advanced
//     020ef450  ands r1, r1, #1        the virtual bit
//     020ef454  ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1]
//     020ef460  ldreq r1,[r3]
//     020ef464  blx r1                 NO r1 argument -> ARITY 0
//
// THERE IS NO NULL GUARD AND NO IDLE-SENTINEL COMPARE.  dScMg3DEsp_c's field
// dispatcher tests the pair against a sentinel and returns early; this one
// dispatches unconditionally, so the field MUST be seeded before the first
// behavior tick.  It is: vtable slot 18 (func_ov006_020efaa8) ends with
// func_ov006_020ef7f8(this), which is the writer of data_ov006_0213cc94, and
// slot 0 (func_ov006_020ef834) dispatches slot 18 through the object's own
// vtable with r1 = -1 at 0x020efa2c..0x020efa38.  So the seed arrives on the
// InitResources path, before Behavior ever runs.  port_mg_call0's own
// zero-code refusal is a second net under that, not a replacement for it.
//
// src/func_ov006_020ef3e0.cpp spells the dispatch
//
//     (((C*)c)->**(PMF*)(c + 0x5004))();
//
// which is FOUR bytes on MSVC where the ROM's field is eight.  It compiles and
// it links -- the type is local to the TU, so no global is emitted and no
// symbol can fail -- and the call goes to whatever the low word holds, which
// is a raw DS code address.  That is the SILENT shape, so the src TU is out of
// port/slice_bnt.txt and this file is linked in its place.
//
// ---- 3. THE FIVE STATES, AND WHY THEY ARE EXACTLY FIVE -------------------
//
// Five eight-byte {code, adjustment} records sit in this class's data run, and
// every one of them has EXACTLY ONE relocation in the whole overlay, from a
// literal pool inside this class's own code:
//
//   record      pair              loaded by            reloc (relocs.txt)
//   ----------  ----------------  -------------------  --------------------
//   0213cc74    {0x020ef47c, 0}   func_ov006_020ef480  from:0x020ef4e8
//   0213cc84    {0x020ef5ac, 0}   func_ov006_020ef768  from:0x020ef790
//   0213cc8c    {0x020ef480, 0}   func_ov006_020ef4ec  from:0x020ef57c
//   0213cc94    {0x020ef794, 0}   func_ov006_020ef7f8  from:0x020ef830
//   0213cc9c    {0x020ef4ec, 0}   func_ov006_020ef580  from:0x020ef5a8
//
// FOUR DISTINCT WRITER TUs PLUS ONE, five distinct code words, EVERY
// ADJUSTMENT ZERO, and no .init relocation anywhere among them.  All five code
// words have a matched src TU and none carries a NONMATCHING banner, so this
// class has NO state floor.
//
// EVERY WRITER MOVES EIGHT BYTES.  The pairs are copied whole (ldr/ldr then
// str/str at +0x5004 and +0x5008), and eight bytes is eight bytes on both
// machines, so the STORES are already right on the host and nothing about them
// needs repairing.  Only the DECODE is wrong.  Run mg5 lane FLW's ruling holds
// and is why this file routes AT THE DISPATCH SITE and never rewrites a stored
// pair with a host address.
//
// ---- 4. THE ADJACENCY TRAP, NAMED --------------------------------------
//
// 0x0213cc74 is the word IMMEDIATELY PAST the end of 0x174's vtable
// (0x0213cbe4 + 36*4 = 0x0213cc74) and the word IMMEDIATELY BEFORE this
// class's own SpawnInfo.  An attribution by address adjacency gives it to
// MgBounceAndPounce.  The relocation gives it to func_ov006_020ef4ec, which is
// one of THIS class's states, and the relocation is the one that cannot be
// wrong.  0x174's own seven records (0x0213cb4c, 0x0213cb54, 0x0213cb5c,
// 0x0213cb6c, 0x0213cb74, 0x0213cb7c, 0x0213cb84) are loaded only from
// 0x020ee3b8..0x020ee68c, which is inside 0x174's code, and this lane touches
// none of them.

#include <cstdio>

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry point; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);

/* the five routed states, each with the parameter list its own src TU defines.
   func_ov006_020ef47c is FOUR BYTES -- a bare `bx lr` -- and is this class's
   idle body; there is nothing for an ignored receiver to be wrong about, which
   is the MgCoin_StateDispatch.cpp ruling for the same shape. */
void func_ov006_020ef47c(void);
void func_ov006_020ef480(char *c);
void func_ov006_020ef4ec(char *c);
void func_ov006_020ef5ac(char *self);
void func_ov006_020ef794(char *self);

/* the ordinary callees slot 6 keeps, spelled as src/func_ov006_020ef3e0.cpp
   spells them */
unsigned int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        unsigned int a, unsigned int b, int c, int d, int e, const void *f);
void *_ZN8Particle6System12FromUniqueIDEj(unsigned int id);
void  func_ov006_020eef90(void);

}  /* extern "C" */

static unsigned g_jump2_field_calls, g_jump2_field_routed;
/* The state PROGRESSION witness.  This class has no integer state index, so
   the analogue of one is the sequence of code words the field has held: a run
   that ticks the object without ever changing state reads as a success on a
   call count alone, and that is exactly the vacuous-clean shape the run law
   refuses. */
/* Sized against the switch, not guessed: this class has FIVE states, and 32
   is the same headroom unmatched/MgD3DBase_ObjStateDispatch.cpp uses for the
   same reason.  The overflow counter is the disclosure: a table that silently
   truncates prints a number indistinguishable from saturation. */
enum { FIELD_SEEN_MAX = 32 };

static unsigned g_jump2_last_code;
static unsigned g_jump2_distinct, g_jump2_dropped;
static unsigned g_jump2_seen[FIELD_SEEN_MAX];
static unsigned g_jump2_hits[FIELD_SEEN_MAX];

static void jump2_note(unsigned code)
{
    for (unsigned i = 0; i < g_jump2_distinct; ++i)
        if (g_jump2_seen[i] == code) { ++g_jump2_hits[i]; g_jump2_last_code = code; return; }
    if (g_jump2_distinct < FIELD_SEEN_MAX) {
        g_jump2_seen[g_jump2_distinct] = code;
        g_jump2_hits[g_jump2_distinct] = 1;
        ++g_jump2_distinct;
    } else {
        ++g_jump2_dropped;
    }
    g_jump2_last_code = code;
}

extern "C" unsigned port_mg_jump2_field_dropped(void) { return g_jump2_dropped; }
extern "C" unsigned port_mg_jump2_field_capacity(void) { return FIELD_SEEN_MAX; }

static int jump2_field_try(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x020ef47cu: func_ov006_020ef47c();  return 1;  /* the idle body */
    case 0x020ef480u: func_ov006_020ef480(c); return 1;
    case 0x020ef4ecu: func_ov006_020ef4ec(c); return 1;
    case 0x020ef5acu: func_ov006_020ef5ac(c); return 1;
    case 0x020ef794u: func_ov006_020ef794(c); return 1;
    default:                                  return 0;
    }
}

/* Everything this switch does not own goes to the framework unchanged, so the
   null-code guard, the nonzero-adjustment refusal and the UNHANDLED report all
   still live in exactly one place. */
static void jump2_field_call(void *self, unsigned code, int adj)
{
    ++g_jump2_field_calls;
    if (code != 0 && adj == 0 && jump2_field_try(self, code)) {
        ++g_jump2_field_routed;
        jump2_note(code);
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_jump2_field_counts(unsigned *calls, unsigned *routed,
                                           unsigned *distinct, unsigned *last)
{
    if (calls)    *calls    = g_jump2_field_calls;
    if (routed)   *routed   = g_jump2_field_routed;
    if (distinct) *distinct = g_jump2_distinct;
    if (last)     *last     = g_jump2_last_code;
}

extern "C" unsigned port_mg_jump2_field_row(unsigned i, unsigned *code)
{
    if (i >= g_jump2_distinct)
        return 0;
    if (code) *code = g_jump2_seen[i];
    return g_jump2_hits[i];
}

/* src/func_ov006_020ef3e0.cpp verbatim except that the member-pointer type is
   gone and the dispatch site is a routed call.  The Particle::System calls
   above it and the func_ov006_020eef90 below it are src's, unchanged, and
   every one is confirmed against the disassembly in section 2.  The field the
   ROM reads is at +0x5004 and the unique id it feeds Particle::System is at
   +0x5a6c -- `add r0,r4,#0x5000 / ldr r0,[r0,#0xa6c]` at 0x020ef400. */
// PORT_HOST_ABI: dScMgJump2_c vtable slot 6 Behavior; src dispatches a member pointer held in the object field at c+0x5004 that is four bytes on MSVC where the ROM's field is eight, so the host reads the {code, adj} pair and routes it.
extern "C" int func_ov006_020ef3e0(char *c)
{
    *(unsigned *)(c + 0x5a6c) =
        _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(unsigned *)(c + 0x5a6c), 0xf0, 0x400000, 0x800000, -0x480000, 0);

    {
        void *o = _ZN8Particle6System12FromUniqueIDEj(*(unsigned *)(c + 0x5a6c));
        if (o != 0) {
            *(int *)((char *)o + 0x50) = 0x4000;
            *(unsigned char *)((char *)o + 0x58) = 0x2c;
        }
    }

    func_ov006_020eef90();

    {
        const MgPmf *p = (const MgPmf *)(c + 0x5004);
        jump2_field_call(c + (p->adj >> 1), p->code, p->adj);
    }
    return 1;
}
