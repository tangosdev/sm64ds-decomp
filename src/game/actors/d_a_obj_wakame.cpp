//cpp
/* An animated seaweed prop -- ov002/daObjWakame_c.
 *
 * A GENUINE TRANSLATION UNIT, ENROLLED AND CANONICAL. It is the whole of the
 * cartridge's contiguous linker run .text 0x020bc414..0x020bc5e0, eight
 * functions and nothing else, and the production build links this object rather
 * than eight per-function ones. The filename is tools/tu_names.py's:
 * candidate_stem('daObjWakame_c') is d_a_obj_wakame.
 *
 * THE CLASS IS NAMED FROM THE CARTRIDGE, not coined. ov002 0x02109b78 is a
 * __si_class_type_info whose _ZTS at 0x02109b84 reads exactly `13daObjWakame_c`,
 * and whose one base, at subobject offset 0, is arm9 0x0208e390 -- _ZTI8dActor_c.
 * That is why the header says `struct daObjWakame_c : dActor_c` and why the RTTI
 * below has ROM homes to be compared against at all: a coined name is a
 * length-prefixed mangled string that matches nothing at any address, so it can
 * never be word-compared, and a class whose records cannot be compared cannot
 * have a key-function TU.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * NAMING THE CLASS PUTS ITS VAGUE-LINKAGE DATA IN THIS OBJECT, because this TU
 * defines the key function. romdata_check compares each record against the
 * cartridge with relocations applied before production isolation discards it.
 *
 * THE VTABLE CLAIM IS SCOPED TO THE SLOTS, deliberately. A symbols.txt _ZTV
 * address is the ADDRESS POINT, eight bytes past the table's real start; the
 * {offset-to-top, _ZTI pointer} header word pair is emitted here and
 * word-compared by nothing, so the 31 slots are proved and those two words are
 * not.
 */

#include "daObjWakame_c.h"
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"

extern "C" {
extern int _ZTV13daObjWakame_c[];
extern void Matrix4x3_FromRotationY(void *, int);
extern void _ZN9ModelAnimC1Ev(void *);

/* THE TWO FILE POINTERS KEEP THEIR MANGLED SPELLING, and that is a deliberate
 * limit rather than an oversight. data_ov002_0210e0d4 and _0210e0dc are two
 * SharedFilePtr objects -- Model::LoadFile, Animation::LoadFile and
 * SharedFilePtr::Release all take one -- but include/decl_common.h declares both
 * as `extern int []` and is included by a large part of the tree, so retyping
 * them is a tree-wide change with its own evidence and its own verify cycle, not
 * something to fold into a TU promotion. Until then these functions are reached
 * by their mangled names, which is what the legacy per-function sources did and
 * what byte-matches today. */
extern void  _ZN13SharedFilePtr7ReleaseEv(void *);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *);
extern int   _ZN9ModelBase7SetFileEP8BMD_Fileii(char *, void *, int, int);
extern int   _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char *, void *, int, int, unsigned int);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjWakame_c_Spawn, 0x020bc5a8, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* The +2 addend is the vtable ADDRESS POINT: _ZTV names the record's start and
   the object stores a pointer eight bytes in, past {offset-to-top, _ZTI}. */
int *daObjWakame_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(312);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)(_ZTV13daObjWakame_c + 2);
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN13daObjWakame_c13InitResourcesEv, 0x020bc540, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjWakame_c13InitResourcesEv
s32 daObjWakame_c::InitResources()
{
    char *c = (char *)this;
    void *m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210e0dc);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, -1);
    void *a = _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_0210e0d4);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, a, 0, 0x1000, 0);
    func_ov002_020bc488(c);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13daObjWakame_c8BehaviorEv, 0x020bc520, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjWakame_c8BehaviorEv
/* The ROM's `add r0, r0, #0x124` then `bl 0x02015c3c` is this call and the
   implicit conversion in front of it: Advance is Animation's, Animation sits at
   +0x50 inside a ModelAnim, and the member is at +0xd4. Nothing else happens --
   a seaweed's whole per-frame behaviour is to advance its animation. */
s32 daObjWakame_c::Behavior()
{
    mModelAnim.Advance();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13daObjWakame_c6RenderEv, 0x020bc4f8, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjWakame_c6RenderEv
/* A REAL VIRTUAL DISPATCH, and the ROM insists on it: `ldr r2,[r0,#0xd4]!` loads
   the member's own vptr, `ldr r2,[r2,#0x14]` picks slot 5 -- Model::Render -- and
   `blx r2` calls through it. mwccarm does not devirtualize this even though the
   member's dynamic type is known here, so the ordinary member call is the right
   spelling and the null argument is Render's `const Vector3 *scale`. */
s32 daObjWakame_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN13daObjWakame_c16CleanupResourcesEv, 0x020bc4c8, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjWakame_c16CleanupResourcesEv
s32 daObjWakame_c::CleanupResources()
{
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210e0dc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210e0d4);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov002_020bc488, 0x020bc488, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
/* Posts the actor's placement into the model's transform, and every offset here
   reads as a field of the member once 0xd4 is subtracted: 0xf0 is ModelAnim+0x1c,
   which include/Model.h names mat4x3, and 0x114/0x118/0x11c are ModelAnim+0x40/
   0x44/0x48 -- the translation column of that same Matrix4x3, which begins 0x24
   into it. The source is dActor_c's own mPosX/Y/Z at 0x5c/0x60/0x64, and the
   angle fed to the rotation is mAngleY at 0x8e. The >> 3 is the ROM's, and this
   file does not explain it.

   It keeps its func_ov002_ name and its char* parameter because the cartridge
   gives it neither a name nor a `this`: it is a file-scope helper the class
   calls, not a member, and renaming it would be a claim the image does not
   support. */
void func_ov002_020bc488(char *t)
{
    Matrix4x3_FromRotationY(t + 0xf0, *(short *)(t + 0x8e));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN13daObjWakame_cD0Ev, 0x020bc444, size 0x44 */
/* -------------------------------------------------------------------------- */
/* _ZN13daObjWakame_cD0Ev (vtable slot 17, the deleting destructor) is NOT
 * hand-written here. A hand-written mangled D0 next to a real out-of-line D1
 * ICEs mwccarm 2004/b56 (ELFgen.c:483); the compiler synthesizes D0 itself from
 * D1, and its tail -- `bl _ZN6Memory10DeallocateEPvP4Heap` at 0x020bc470 -- is
 * the inline operator delete, which is why nothing here mentions a heap. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN13daObjWakame_cD1Ev, 0x020bc414, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjWakame_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vptr store, the member destroyed, then the tail into
 * ~dActor_c.
 *
 * (no definition here: `virtual ~daObjWakame_c() {}` is in
 * include/daObjWakame_c.h, and that placement is load-bearing rather than
 * stylistic -- out of line, mwccarm emits D0 before D1 and adds a homeless D2,
 * and objisolate then refuses this whole TU. The header carries the reasoning and
 * the leaf measurement that makes it safe.) */
