//cpp
/* A warp trigger volume -- ov002/daWarpkun_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x020ec388..0x020ec56c, eight
 * functions and nothing else, and the production build links this object rather
 * than eight per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daWarpkun_c') is d_a_warpkun.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov002 0x0210ac7c is a
 * __si_class_type_info whose _ZTS at 0x0210ac88 reads exactly `11daWarpkun_c`,
 * and whose one base, at subobject offset 0, is arm9 0x0208e390 --
 * _ZTI8dActor_c. That is why the header says `struct daWarpkun_c : dActor_c`
 * and why the RTTI below has ROM homes to be compared against at all: a coined
 * name is a length-prefixed mangled string that matches nothing at any address,
 * so it can never be word-compared, and a class whose records cannot be
 * compared cannot have a key-function TU.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * IT OWNS ONE MEMBER, AND THE ARITHMETIC CLOSES. The factory's literal is
 * `mov r0, #0x108`; dActor_c ends at 0xd0 and include/dCcAc_c.h asserts
 * sizeof(dCcAc_c) == 0x34, so 0xd4 + 0x34 = 0x108 exactly, with the four bytes
 * of pad_0d0 in between and nothing after. Both witnesses reach that member:
 * the factory calls _ZN7dCcAc_cC1Ev on this+0xd4 after the vptr store, and the
 * destructor calls _ZN7dCcAc_cD1Ev on it before tailing into ~dActor_c.
 *
 * NAMING THE CLASS PUTS ITS VAGUE-LINKAGE DATA IN THIS OBJECT, because this TU
 * defines the key function. Nine records come out; romdata_check compares each
 * against the cartridge with relocations applied before production isolation
 * discards it:
 *
 *   _ZTV11daWarpkun_c   ov002 0x0210acbc  VERIFIED, 124 bytes = 31 slots
 *   _ZTI11daWarpkun_c   ov002 0x0210ac7c  VERIFIED
 *   _ZTI8dActor_c / _ZTI7dBase_c / _ZTI7fBase_c   arm9   VERIFIED
 *   the four _ZTS records                                PARTIAL
 *   nothing                                              DIFFERS
 *
 * THE VTABLE CLAIM IS SCOPED TO THE SLOTS, deliberately. A symbols.txt _ZTV
 * address is the ADDRESS POINT, eight bytes past the table's real start; the
 * {offset-to-top, _ZTI pointer} header word pair is emitted here and
 * word-compared by nothing, so the 31 slots are proved and those two words are
 * not. Diffed against _ZTV8dActor_c (arm9 0x0208e3a4) the two tables are 31
 * words each and differ in exactly slots 0, 3, 6, 9, 12, 16 and 17 --
 * InitResources, CleanupResources, Behavior, Render, OnPendingDestroy, D1 and
 * D0, precisely the seven members the header declares. The four PARTIAL rows
 * are the known dsd extent shortfall on _ZTS records -- a range that stops
 * short, not a disagreement about bytes.
 *
 * NO PRAGMAS. None of the eight legacy sources carried one.
 */

/* Includes: the union of the eight legacy files', reconciled -- the class's own
 * header now carries the real base clause and the one member, so nothing here
 * shadows it. */
#include "daWarpkun_c.h"
#include "decl_Actor.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
#include "Player.h"
#include "decl_ActorBase.h"

extern "C" {
extern int _ZTV11daWarpkun_c[];
extern void *data_020a0eac;
extern "C" int IsPlayerWarping(int a0);
extern "C" void WarpPlayer(int i, int val);
typedef int Fix12i;
extern int _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *, Fix12i, Fix12i, Fix12i, Fix12i);
extern int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, void *, Fix12i, Fix12i, unsigned int, unsigned int);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daWarpkun_c_classInit, 0x020ec534, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol daWarpkun_c_classInit
/* recovered: vtable identified, declarations from a shared header */
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV11daWarpkun_c */
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* Reconstructed source-style name: SM64DS proves daWarpkun_c through RTTI,
 * allocation size, vtable identity, and the WARPKUN registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daWarpkun_c_Spawn. */
int *daWarpkun_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(264);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)(_ZTV11daWarpkun_c + 2);
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN11daWarpkun_c13InitResourcesEv, 0x020ec4c4, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWarpkun_c13InitResourcesEv
// recovered name: daWarpkun_c_InitResources
/* recovered: renamed to Class_Method */
/* daWarpkun_c::InitResources - recovered from vtable slot identity */
s32 daWarpkun_c::InitResources() {
    void * c = (void *)this;
  int v = *((int *) (((char *) c) + 8));
  int r = (((*((int *) (((char *) c) + 8))) & 0xf) + 1) << 0x12;
  _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0, r, r + 0x7d0000, 0);
  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *) c) + 0xd4, c, r, ((((v & 0xFFFFFFFFu) >> 4) & 0xf) + 1) << 0x12, 2, 0x400000);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN11daWarpkun_c8BehaviorEv, 0x020ec410, size 0xb4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWarpkun_c8BehaviorEv
// recovered name: daWarpkun_c::Behavior
/* daWarpkun_c::Behavior - vtable slot 6, overriding fBase_c::Behavior(). Migrated
 * to a real member: `c + 0xf8` is mdCcAc_c.otherOwner
 * (include/dCc_c.h, 0x24 into the embedded dCcAc_c at
 * 0xd4 -- 0xd4 + 0x24 = 0xf8) and `c + 0xd4` casts to that same member's
 * base for Clear()/Update(); `c + 0x8e` and `c + 8` are dActor_c's own
 * mAngleY and param1, already recovered in dActor_c.h (mAngleY is
 * repurposed here as a warp-in-progress flag, not an angle). The found
 * actor's own fields (actorID 0x00c, mPlayerNo 0x6d8) are real Player
 * members once cast. */
s32 daWarpkun_c::Behavior()
{
    unsigned int id = mdCcAc_c.otherOwner;
    if (id != 0) {
        if (mAngleY == 0) {
            dActor_c *a = dActor_c::FindWithID(id);
            if (a != 0) {
                int isPlayer = (a->actorID == 0xbf) ? 1 : 0;
                if (isPlayer != 0) {
                    Player *p = (Player *)a;
                    if (IsPlayerWarping(p->mPlayerNo) != 0) {
                        mAngleY = 1;
                    } else if (p->CanWarp()) {
                        unsigned int param = (param1 >> 0xc) + 1;
                        WarpPlayer(p->mPlayerNo, param & 0xff);
                    }
                }
            }
        }
    } else {
        mAngleY = 0;
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN11daWarpkun_c6RenderEv, 0x020ec408, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWarpkun_c6RenderEv
// recovered name: daWarpkun_c_Render
/* recovered: renamed to Class_Method */
/* daWarpkun_c::Render - recovered from vtable slot identity */
s32 daWarpkun_c::Render() {
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN11daWarpkun_c16OnPendingDestroyEv, 0x020ec404, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWarpkun_c16OnPendingDestroyEv
// recovered name: daWarpkun_c_OnPendingDestroy
/* recovered: renamed to Class_Method */
/* daWarpkun_c::OnPendingDestroy - recovered from vtable slot identity */
void daWarpkun_c::OnPendingDestroy() {
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN11daWarpkun_c16CleanupResourcesEv, 0x020ec3fc, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWarpkun_c16CleanupResourcesEv
// recovered name: daWarpkun_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daWarpkun_c::CleanupResources - recovered from vtable slot identity */
s32 daWarpkun_c::CleanupResources() {
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11daWarpkun_cD0Ev, 0x020ec3b8, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWarpkun_cD0Ev
/* The DELETING DESTRUCTOR, vtable slot 17. Deleted: keeping this hand-written
   mangled-name free-function body next to the real out-of-line daWarpkun_c::~daWarpkun_c()
   (D1, below) is the mwcc ICE this series has hit repeatedly (ELFgen.c:483).
   The real D1 makes the compiler synthesize D0 itself, byte-identical to the
   ROM (confirmed by verify, not assumed). */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN11daWarpkun_cD1Ev, 0x020ec388, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11daWarpkun_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the member subobject at 0xd4 destroyed,
 * then the tail into ~dActor_c.
 *
 * (no definition here: `virtual ~daWarpkun_c() {}` is in
 * include/daWarpkun_c.h, and that placement is load-bearing rather than
 * stylistic -- out of line, mwccarm emits D0 before D1 and adds a homeless D2,
 * and objisolate then refuses this whole TU. The header carries the reasoning,
 * the leaf measurement that makes it safe, and the disassembly of the 0x30
 * bytes the empty braces have to produce.) */
