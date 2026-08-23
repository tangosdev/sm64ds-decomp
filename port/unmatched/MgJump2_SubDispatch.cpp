// PORT_HOST_ABI.  The mwcc POINTER-TO-MEMBER WALL for dScMgJump2_c, SUB-OBJECT
// half: the two per-frame ticks of the two sub-object families this class
// builds, func_ov006_020c4cd8 (field +0x30) and func_ov006_020c7860
// (field +0x3c).  Run mg11, lane BNT.
//
// ---- 1. WHY A SECOND HOST-COPY FILE ---------------------------------------
//
// unmatched/MgJump2_FieldPmf.cpp is the CLASS's own dispatcher, vtable slot 6.
// These two are not on any vtable and are not reached from any table: they are
// the tick bodies of the objects the factory constructs inside the scene, and
// each one dispatches a member pointer held in ITS OWN object.  Neither the
// class's vtable derivation nor a __sinit reading would have produced them.
// They were found by the two prescribed detectors run over this lane's whole
// closure rather than over its vtable:
//
//   * the ENCODING SCAN (run mg9 lane LKY's detector, blind to spelling):
//     THREE ARM Itanium five-instruction dispatch sites in the closure --
//     0x020ef450 (slot 6, the class's own), 0x020c4cec and 0x020c789c.
//   * the `::*` SOURCE SWEEP over the closure's new TUs: four hits --
//     src/func_ov004_020b87e0.cpp (the framework state setter, already
//     host-copied in unmatched/MgBase_StateSetter.cpp and excluded from every
//     minigame slice), src/func_ov006_020c4cd8.cpp, src/func_ov006_020c7860.cpp
//     and src/func_ov006_020ef3e0.cpp.
//
// The two detectors agree exactly, which is the property that makes the
// "THREE and only three" claim worth making.
//
// ---- 2. THEY ARE THIS CLASS'S ALONE, WHICH WAS CHECKED NOT ASSUMED --------
//
// dScMgJump2_c shares its intermediate base dScMgD3DBase_c with 0x174, 0x180
// and 0x181, and this run seats all four concurrently, so anything shared is a
// merge collision.  These two are NOT shared.  Each has exactly ONE caller in
// the whole overlay -- func_ov006_020c42bc calls 0x020c4cd8 at 0x020c42e8 and
// func_ov006_020c712c calls 0x020c7860 at 0x020c7168 -- and neither caller nor
// callee appears in 0x174's closure (71 bodies), 0x180's (92) or 0x181's
// (109).  All three were walked from their own vtable overrides and factories
// for this check.  So this file defines nothing another lane in this wave also
// defines, and the merge owes it no dedup ruling.
//
// ---- 3. THE SHAPE, OUT OF THE ROM -----------------------------------------
//
// extracted/overlays/overlay_0006.bin at base 0x020bfec0.
//
// func_ov006_020c4cd8, 0x44 -- 17 instructions and NO pool = 17 = 0x44/4:
//
//     020c4ce0  add r3, r4, #0x30      &obj->pmf
//     020c4ce4  ldr r1,[r3,#4]         the adjustment
//     020c4ce8  add r0, r4, r1, asr#1  this, advanced
//     020c4cec  ands r1, r1, #1        the virtual bit
//     020c4cf0  ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1]
//     020c4cfc  ldreq r1,[r3]
//     020c4d00  blx r1                 NO r1 argument -> ARITY 0
//     020c4d04  add r0,r4,#0x88 / bl 0x02015c3c    Animation::Advance
//     020c4d0c  mov r0,r4 / bl 0x020c4c00
//
// func_ov006_020c7860, 0x8c -- 32 instructions plus a three-word pool
// (0x0213b010, 0x0213b018, 0x02141a40) = 35 = 0x8c/4: ApproachLinear on
// +0x24, AddVec3(+0x14, +0x20, +0x14), the same five-instruction dispatch at
// &obj->pmf = +0x3c, then func_ov006_020bfec0(data_ov006_02141a40, +0x14,
// +0x36), func_ov006_020c76e0(this) and Animation::Advance(+0x9c).
//
// NEITHER HAS A NULL GUARD.  port_mg_call0 refuses a zero code and reports it,
// which is the only net under a field that is ticked before it is seeded.
//
// ---- 4. THE TWENTY-TWO STATES, AND WHICH FIELD EACH BELONGS TO -----------
//
// Thirty-six eight-byte {code, 0} records sit in two runs of ov006 .data, and
// the split between the two fields was MEASURED (each writer's own store
// offset) rather than inferred from the addresses:
//
//   FIELD +0x30, twenty-four records 0x0213af10..0x0213afd0, SIXTEEN distinct
//     code words: 0x020c4d1c (a 0x18-byte idle), 0x020c4e8c, 0x020c4fa4,
//     0x020c53f8, 0x020c5530, 0x020c5658, 0x020c5928, 0x020c5bf8, 0x020c5d28,
//     0x020c6088, 0x020c61c4, 0x020c627c, 0x020c6378, 0x020c6400, 0x020c66bc
//     and 0x020c6a9c.
//   FIELD +0x3c, fifteen records 0x0213b020..0x0213b090, NINE distinct code
//     words: 0x020c78ec, 0x020c7a30, 0x020c7c68, 0x020c8048, 0x020c814c,
//     0x020c833c, 0x020c85bc, 0x020c864c and 0x020c8680.
//
// THE FIRST VERSION OF THIS FILE CARRIED TEN OF THE TWENTY-TWO, and the run is
// what caught it. A scan that took only pair records whose single relocation
// came from inside the closure-as-then-known found six and four; the writers
// of the other twelve (func_ov006_020c4f68, _020c762c, _020c8270, _020c81e0,
// _020c85a0, _020c8768 and the eleven-arm switch inside func_ov006_020c6a9c)
// were not in that closure BECAUSE they are only reachable through the
// dispatch this file implements. The closure and the switch are mutually
// recursive, and the fix is to walk it again with every code word as a root
// until it stabilises -- 172 bodies, then 207, then 223.
//
// THE 600-FRAME RUN NAMED THE TWO THAT MATTERED, which is why the run is the
// acceptance and not the scan: "MINIGAME STATE DISPATCH UNHANDLED: DS address
// 0x020c833c" and "... 0x020c6378", 1266 + 914 ticks that reached the
// framework and did nothing. A second 1800-frame run named one more,
// 0x020c7c68, once the element vtable below stopped faulting the run early.
// ALL TWENTY-FIVE are cases below, and the acceptance is a run whose
// framework-routed count is ZERO on both machines rather than a scan that
// says it should be.
//
// THE SPLIT WAS MEASURED ON THE WRITERS THEMSELVES, not read off the record
// addresses: every function that installs one of these records was
// disassembled and its own store offset taken.  Eight write +0x30/+0x34
// (0x020c4060, 0x020c4148, 0x020c4710, 0x020c4c54, 0x020c4d20, 0x020c64e4,
// 0x020c68f4, 0x020c6ca4) plus the eleven-arm switch inside
// func_ov006_020c6a9c and func_ov006_020c4f68; the rest write +0x3c/+0x40
// (0x020c7300, 0x020c762c, 0x020c79a8, 0x020c7ba4, 0x020c802c, 0x020c8084,
// 0x020c8270, 0x020c81e0, 0x020c85a0, 0x020c862c, 0x020c8658, 0x020c8768).
// EVERY adjustment word is zero and EVERY one of the twenty-five code words
// resolves to a matched src TU with no NONMATCHING banner, so neither machine
// has a floor -- checked per body against src/ rather than inferred from the
// costs table's nosrc column, which is computed over the VTABLE only and
// cannot see a state-table floor at all.
//
// BOTH SWITCHES CARRY BOTH SETS.  A field is one address and a code word is
// one address, so the two key sets are disjoint and a shared switch cannot
// mis-route; keeping them separate would only mean a state that migrated
// between the two object families reported UNHANDLED instead of running.  The
// per-site counters below say which site actually routed what, so the claim
// that the two sets are disjoint in practice is measured on every run.

#include <cstdio>

struct MgPmf { unsigned code; int adj; };

extern "C" {

void port_mg_call0(void *self, unsigned code, int adj);

/* field +0x30's SIXTEEN */
void func_ov006_020c4d1c(void);
void func_ov006_020c4e8c(char *c);
void func_ov006_020c4fa4(char *c);
void func_ov006_020c53f8(char *c);
void func_ov006_020c5530(char *c);
void func_ov006_020c5658(char *c);
void func_ov006_020c5928(char *c);
void func_ov006_020c5bf8(char *c);
void func_ov006_020c5d28(char *c);
void func_ov006_020c6088(char *c);
void func_ov006_020c61c4(int c);
void func_ov006_020c627c(char *c);
void func_ov006_020c6378(int c);
void func_ov006_020c6400(char *self);
void func_ov006_020c66bc(char *c);
void func_ov006_020c6a9c(char *c);

/* field +0x3c's NINE */
void func_ov006_020c78ec(char *thiz);
void func_ov006_020c7a30(char *c);
void func_ov006_020c7c68(char *c);
void func_ov006_020c8048(void *c);
void func_ov006_020c814c(char *c);
void func_ov006_020c833c(char *c);
void func_ov006_020c85bc(char *o);
void func_ov006_020c864c(int *p);
void func_ov006_020c8680(char *self);

/* the ordinary callees the two host copies keep, spelled as their own src TUs
   spell them */
void _ZN9Animation7AdvanceEv(void *anim);
void func_ov006_020c4c00(void *c);
void _Z14ApproachLinearRiii(int &v, int target, int step);
void AddVec3(void *a, void *b, void *c);
void func_ov006_020bfec0(void *a, char *b, short *d);
void func_ov006_020c76e0(char *c);

extern int   data_ov006_0213b010;
extern int   data_ov006_0213b018;
extern void *data_ov006_02141a40;

}  /* extern "C" */

static unsigned g_sub_calls[2], g_sub_routed[2], g_sub_unknown[2];
static unsigned g_sub_distinct[2];
static unsigned g_sub_seen[2][8], g_sub_hits[2][8];

static void sub_note(int which, unsigned code)
{
    for (unsigned i = 0; i < g_sub_distinct[which]; ++i)
        if (g_sub_seen[which][i] == code) { ++g_sub_hits[which][i]; return; }
    if (g_sub_distinct[which] < 8) {
        g_sub_seen[which][g_sub_distinct[which]] = code;
        g_sub_hits[which][g_sub_distinct[which]] = 1;
        ++g_sub_distinct[which];
    }
}

static int sub_try(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    case 0x020c4d1cu: func_ov006_020c4d1c();  return 1;
    case 0x020c4e8cu: func_ov006_020c4e8c(c); return 1;
    case 0x020c4fa4u: func_ov006_020c4fa4(c); return 1;
    case 0x020c53f8u: func_ov006_020c53f8(c); return 1;
    case 0x020c5530u: func_ov006_020c5530(c); return 1;
    case 0x020c5658u: func_ov006_020c5658(c); return 1;
    case 0x020c5928u: func_ov006_020c5928(c); return 1;
    case 0x020c5bf8u: func_ov006_020c5bf8(c); return 1;
    case 0x020c5d28u: func_ov006_020c5d28(c); return 1;
    case 0x020c6088u: func_ov006_020c6088(c); return 1;
    case 0x020c61c4u: func_ov006_020c61c4((int)c); return 1;
    case 0x020c627cu: func_ov006_020c627c(c); return 1;
    case 0x020c6378u: func_ov006_020c6378((int)c); return 1;
    case 0x020c6400u: func_ov006_020c6400(c); return 1;
    case 0x020c66bcu: func_ov006_020c66bc(c); return 1;
    case 0x020c6a9cu: func_ov006_020c6a9c(c); return 1;
    case 0x020c78ecu: func_ov006_020c78ec(c); return 1;
    case 0x020c7a30u: func_ov006_020c7a30(c); return 1;
    case 0x020c7c68u: func_ov006_020c7c68(c); return 1;
    case 0x020c8048u: func_ov006_020c8048(c); return 1;
    case 0x020c814cu: func_ov006_020c814c(c); return 1;
    case 0x020c833cu: func_ov006_020c833c(c); return 1;
    case 0x020c85bcu: func_ov006_020c85bc(c); return 1;
    case 0x020c864cu: func_ov006_020c864c((int *)c); return 1;
    case 0x020c8680u: func_ov006_020c8680(c); return 1;
    default:                                  return 0;
    }
}

static void sub_call(int which, void *self, unsigned code, int adj)
{
    ++g_sub_calls[which];
    if (code != 0 && adj == 0 && sub_try(self, code)) {
        ++g_sub_routed[which];
        sub_note(which, code);
        return;
    }
    ++g_sub_unknown[which];
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_jump2_sub_counts(int which, unsigned *calls,
                                         unsigned *routed, unsigned *unknown,
                                         unsigned *distinct)
{
    if (which < 0 || which > 1)
        return;
    if (calls)    *calls    = g_sub_calls[which];
    if (routed)   *routed   = g_sub_routed[which];
    if (unknown)  *unknown  = g_sub_unknown[which];
    if (distinct) *distinct = g_sub_distinct[which];
}

extern "C" unsigned port_mg_jump2_sub_row(int which, unsigned i, unsigned *code)
{
    if (which < 0 || which > 1 || i >= g_sub_distinct[which])
        return 0;
    if (code) *code = g_sub_seen[which][i];
    return g_sub_hits[which][i];
}

/* src/func_ov006_020c4cd8.cpp verbatim except for the routed dispatch. */
extern "C" void func_ov006_020c4cd8(char *c)
{
    const MgPmf *p = (const MgPmf *)(c + 0x30);
    sub_call(0, c + (p->adj >> 1), p->code, p->adj);
    _ZN9Animation7AdvanceEv(c + 0x88);
    func_ov006_020c4c00(c);
}

/* src/func_ov006_020c7860.cpp verbatim except for the routed dispatch. */
extern "C" void func_ov006_020c7860(char *c)
{
    _Z14ApproachLinearRiii(*(int *)(c + 0x24), data_ov006_0213b010,
                           data_ov006_0213b018);
    AddVec3(c + 0x14, c + 0x20, c + 0x14);

    {
        const MgPmf *p = (const MgPmf *)(c + 0x3c);
        sub_call(1, c + (p->adj >> 1), p->code, p->adj);
    }

    func_ov006_020bfec0(*(void **)&data_ov006_02141a40, c + 0x14,
                        (short *)(c + 0x36));
    func_ov006_020c76e0(c);
    _ZN9Animation7AdvanceEv(c + 0x9c);
}
