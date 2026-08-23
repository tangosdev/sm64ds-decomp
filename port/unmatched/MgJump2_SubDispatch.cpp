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
// 207-body closure rather than over its vtable:
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
// ---- 4. THE TEN STATES, AND WHICH FIELD EACH BELONGS TO ------------------
//
// Sixteen eight-byte {code, 0} records in this closure are installed by a TU
// inside it.  They split cleanly in two, and the split was MEASURED (each
// writer's own store offset) rather than inferred from the addresses:
//
//   FIELD +0x30, records 0x0213af38..0x0213afc8, six distinct code words
//     0x020c4d1c  func_ov006_020c4d1c(void)     idle-shaped, 0x18
//     0x020c4e8c  func_ov006_020c4e8c(char *)
//     0x020c4fa4  func_ov006_020c4fa4(char *)
//     0x020c6400  func_ov006_020c6400(char *)
//     0x020c66bc  func_ov006_020c66bc(char *)
//     0x020c6a9c  func_ov006_020c6a9c(char *)
//   FIELD +0x3c, records 0x0213b040..0x0213b090, four distinct code words
//     0x020c8048  func_ov006_020c8048(void *)
//     0x020c85bc  func_ov006_020c85bc(char *)
//     0x020c864c  func_ov006_020c864c(int *)
//     0x020c8680  func_ov006_020c8680(char *)
//
// Writers confirmed by their own str offsets: 0x020c6ca4, 0x020c68f4,
// 0x020c64e4, 0x020c4d20, 0x020c4c54, 0x020c4710, 0x020c4060 and 0x020c4148
// all write +0x30/+0x34; 0x020c7300, 0x020c8658, 0x020c862c and 0x020c8084 all
// write +0x3c/+0x40.  Every adjustment word is zero and every code word has a
// matched src TU, so neither machine has a floor.
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

/* field +0x30's six */
void func_ov006_020c4d1c(void);
void func_ov006_020c4e8c(char *c);
void func_ov006_020c4fa4(char *c);
void func_ov006_020c6400(char *self);
void func_ov006_020c66bc(char *c);
void func_ov006_020c6a9c(char *c);

/* field +0x3c's four */
void func_ov006_020c8048(void *c);
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
    case 0x020c6400u: func_ov006_020c6400(c); return 1;
    case 0x020c66bcu: func_ov006_020c66bc(c); return 1;
    case 0x020c6a9cu: func_ov006_020c6a9c(c); return 1;
    case 0x020c8048u: func_ov006_020c8048(c); return 1;
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
