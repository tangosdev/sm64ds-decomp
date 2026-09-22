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
 * below has ROM homes to be compared against at all. Profile SEAWEED(296) in
 * symbols/overlay_actors.md.
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
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim 6az: InitResources' by-value Fix12<int> speed makes
 *   the header method form size-DIFF.
 * - func_ov002_020bc488 keeps its unnamed ROM spelling; the image gives it
 *   neither a member name nor a `this`.
 * - data_ov002_0210e0d4 / _0210e0dc keep their ROM bss names (typed here as
 *   SharedFilePtr). This TU claims .text only.
 * - common.h is not first: this file wants math/Matrix.h's Matrix4x3 so
 *   mModelAnim.mat4x3.t is a Vector3.
 * - No Player.h / Camera.h.
 */

#include "daObjWakame_c.h"
#include "SharedFilePtr.h"

extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *m, s16 angleY);

/* ModelAnim::SetAnim -- wall 6az on InitResources. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self,
                                                BCA_File *animFile,
                                                int flags, int speed,
                                                u32 startFrame);

extern SharedFilePtr data_ov002_0210e0dc;   /* BMD model */
extern SharedFilePtr data_ov002_0210e0d4;   /* BCA animation */

void func_ov002_020bc488(daObjWakame_c *t);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjWakame_c through RTTI,
 * allocation size, vtable identity, and the WAKAME registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daObjWakame_c_Spawn.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x020bc5ac loads 312 = 0x138 -- the class's own size -- into the header's
 * inline operator new; 0x020bc5bc calls dActor_c's C2, the store at
 * 0x020bc5c8 lays down this class's vptr, and 0x020bc5d0 constructs
 * mModelAnim. The null check is the one `new` itself emits. */
// @symbol daObjWakame_c_classInit
extern "C" daObjWakame_c *daObjWakame_c_classInit(void)
{
    return new daObjWakame_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjWakame_c13InitResourcesEv
s32 daObjWakame_c::InitResources()
{
    void *m = Model::LoadFile(data_ov002_0210e0dc);
    mModelAnim.SetFile((BMD_File *)m, 1, -1);
    void *a = Animation::LoadFile(data_ov002_0210e0d4);
    /* MEASURED: by-value Fix12<int> speed is wall 6az. */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)a, 0, 0x1000, 0);
    func_ov002_020bc488(this);
    return 1;
}

/* -------------------------------------------------------------------------- */
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
/* -------------------------------------------------------------------------- */
// @symbol _ZN13daObjWakame_c16CleanupResourcesEv
s32 daObjWakame_c::CleanupResources()
{
    data_ov002_0210e0dc.Release();
    data_ov002_0210e0d4.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol func_ov002_020bc488
/* Posts the actor's placement into the model's transform. The >> 3 is the
   ROM's, and this file does not explain it. */
extern "C" void func_ov002_020bc488(daObjWakame_c *t)
{
    Matrix4x3_FromRotationY(&t->mModelAnim.mat4x3, t->mAngleY);
    t->mModelAnim.mat4x3.t.x = t->mPosX >> 3;
    t->mModelAnim.mat4x3.t.y = t->mPosY >> 3;
    t->mModelAnim.mat4x3.t.z = t->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* _ZN13daObjWakame_cD0Ev (vtable slot 17, the deleting destructor) is NOT
 * hand-written here. A hand-written mangled D0 next to a real out-of-line D1
 * ICEs mwccarm 2004/b56 (ELFgen.c:483); the compiler synthesizes D0 itself from
 * D1, and its tail -- `bl _ZN6Memory10DeallocateEPvP4Heap` at 0x020bc470 -- is
 * the inline operator delete, which is why nothing here mentions a heap. */

/* -------------------------------------------------------------------------- */
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
