//cpp
/* Production translation unit for ov015/daObjBkBillboard_c, hand-curated.
 * 7 function(s), .text 0x021111a0..0x02111314.
 *
 * The billboard on a pole in Big Boo's Haunt (`bk` is the ROM's tag for that
 * stage). It derives DIRECTLY from dActor_c -- the cartridge's own RTTI says
 * so -- and adds one member, a Model at 0xd4, which is what closes the class
 * on the factory's literal 0x124. It fills three vtable slots: InitResources
 * (0), CleanupResources (3) and Render (9).
 *
 * ONE OF THE SIX IS NOT A METHOD. func_ov015_02111214 sits between D0 and
 * CleanupResources in the ROM's own .text order, so it is part of this TU and
 * cannot be split out; it is a file-local helper that InitResources calls to
 * seed the model's rotation matrix and scale from the actor's fields. It keeps
 * C linkage and its unmangled ROM name, which is why it is wrapped in its own
 * `extern "C"` block below rather than being made a member.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder:
 * tools/rombuild.py refuses the object outright when the emitted order and the
 * ROM's disagree.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x021111a0  src/_ZN18daObjBkBillboard_cD1Ev.cpp
 *   [1] 0x021111d0  src/_ZN18daObjBkBillboard_cD0Ev.cpp
 *   [2] 0x02111214  src/func_ov015_02111214.c
 *   [3] 0x02111254  src/_ZN18daObjBkBillboard_c16CleanupResourcesEv.cpp
 *   [4] 0x02111278  src/_ZN18daObjBkBillboard_c6RenderEv.cpp
 *   [5] 0x021112a0  src/_ZN18daObjBkBillboard_c13InitResourcesEv.cpp
 *   [6] 0x021112dc  src/daObjBkBillboard_c_classInit.c
 *
 * THE SEVENTH IS THE FACTORY. daObjBkBillboard_c_classInit (historical alias
 * PoleBillboard_Spawn) is the BK_BILLBOARD registry profile's spawn function
 * and sits immediately after InitResources in the ROM's own .text order, so it
 * is part of this TU. It keeps C linkage and is written first here, being the
 * highest-address member.
 */

#include "daObjBkBillboard_c.h"
#include "decl_common.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'Base' */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

/* shadow struct 'Derived' */
struct Derived { char pad[0xd4]; Base base; };

extern "C" {
extern void Matrix4x3_FromRotationY(void *, int);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
/* CONFLICT RESOLVED. The two legacy files spelled this ROM object differently:
   CleanupResources declared it locally as `int data_ov015_02114960[]`, while
   InitResources took include/decl_common.h's `extern int data_ov015_02114960`.
   Both cannot stand in one TU. decl_common.h's spelling wins -- it is the
   project-wide declaration -- and CleanupResources takes the address explicitly
   below, which is the same address the array spelling decayed to. */
/* Defined further down this same file -- InitResources is written above it
   because the emitted order is the reverse of the source order. */
void func_ov015_02111214(char *t);

/* The factory's own dependencies, restated here rather than pulled in through
   decl_Actor.h / decl_ActorBase.h / decl_Model.h as the legacy file did --
   this TU declares in place, and pulling new decl_*.h headers in changes what
   the TU sees and can perturb members that already match. */
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN8dActor_cC2Ev(void *self);
extern void _ZN5ModelC1Ev(void *self);
extern int _ZTV18daObjBkBillboard_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjBkBillboard_c_classInit, 0x021112dc, size 0x38       */
/* -------------------------------------------------------------------------- */
// @symbol daObjBkBillboard_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjBkBillboard_c through
   RTTI, allocation size, vtable identity, and the BK_BILLBOARD registry
   profile; later EAD lineage supplies classInit. Exact original spelling is
   not preserved. Historical alias: PoleBillboard_Spawn.

   292 = 0x124 = the whole object: dActor_c's 0xd4 plus this class's one Model
   member, whose constructor runs on `p + 0xd4` right after the vptr store. */
extern "C" int *daObjBkBillboard_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(292);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV18daObjBkBillboard_c;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN18daObjBkBillboard_c13InitResourcesEv, 0x021112a0, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjBkBillboard_c13InitResourcesEv
// recovered name: daObjBkBillboard_c::InitResources
/* daObjBkBillboard_c::InitResources - vtable slot 0, overriding
 * fBase_c::InitResources(). Migrated to a real member: mModel is a real
 * field (include/daObjBkBillboard_c.h, 0xd4) so the raw `c + 0xd4` cast becomes
 * `mModel`, and Model::LoadFile / Model::SetFile are real (static/member)
 * declarations in include/Model.h and include/ModelBase.h. */
s32 daObjBkBillboard_c::InitResources()
{
    void *file = Model::LoadFile(*(SharedFilePtr *)&data_ov015_02114960);
    mModel.SetFile((BMD_File *)file, 1, -1);
    func_ov015_02111214((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN18daObjBkBillboard_c6RenderEv, 0x02111278, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjBkBillboard_c6RenderEv
// recovered name: daObjBkBillboard_c_Render
/* recovered: renamed to Class_Method */
/* daObjBkBillboard_c::Render - recovered from vtable slot identity */
s32 daObjBkBillboard_c::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN18daObjBkBillboard_c16CleanupResourcesEv, 0x02111254, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjBkBillboard_c16CleanupResourcesEv
// recovered name: daObjBkBillboard_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daObjBkBillboard_c::CleanupResources - recovered from vtable slot identity */
s32 daObjBkBillboard_c::CleanupResources() {
    _ZN13SharedFilePtr7ReleaseEv(&data_ov015_02114960);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov015_02111214, 0x02111214, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov015_02111214(char *t)
{
    Matrix4x3_FromRotationY(t + 0xf0, *(short *)(t + 0x8e));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN18daObjBkBillboard_cD0Ev, 0x021111d0, size 0x44        */
/* ROM ordinal 0 -- _ZN18daObjBkBillboard_cD1Ev, 0x021111a0, size 0x30        */
/* -------------------------------------------------------------------------- */
// @symbol _ZN18daObjBkBillboard_cD1Ev
// @symbol _ZN18daObjBkBillboard_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The inline `~daObjBkBillboard_c() {}` in the
   header is the whole source of both variants: from an inline body mwcc emits
   D1 and then D0 -- the cartridge's own order -- and no D2. Writing the body
   out of line here instead flips them to D0-before-D1 and the isolation step
   rejects the object.

   Their bodies are one vptr store, then mModel destroyed, then ~dActor_c --
   the direct-base chain the RTTI states. D0's trailing deallocation is the
   inline `operator delete` it inherits, which is why nothing here names a
   heap. */
