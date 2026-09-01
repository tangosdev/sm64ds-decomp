//cpp
/* Genuine production translation unit for ov012/daObjC0_Switch_c (8 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daObjC0_Switch_c -> d_a_obj_c0_switch.
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `SwitchPillar`, a coined name. _ZTS16daObjC0_Switch_c at ov012:0x0211230c is
 * the byte string "16daObjC0_Switch_c" in extracted/overlays/overlay_0012.bin,
 * and _ZTI16daObjC0_Switch_c at 0x02112300 reads [0x0209a764, 0x0211230c,
 * 0x021089ec] -- _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt),
 * that same typeinfo name, and _ZTI10dBgActor_c, so the ROM itself states the
 * direct base too.
 * tools/class_rename.py performed the rename, and it is what makes this
 * promotion possible at all: a coined class reaches no symbol home, so its
 * _ZTI/_ZTS/_ZTV records can never be word-compared against the cartridge and
 * its key-function TU cannot declare a verifiable compiler_only_output
 * (tools/tubuild.py apply_compiler_only_policy). With the ROM spelling the
 * eleven emitted records below resolve as `deadstrip-data` and romdata_check
 * proves them.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM
 * address order):
 *   [0] 0x021111a0  src/_ZN16daObjC0_Switch_cD1Ev.cpp
 *   [1] 0x021111e4  src/_ZN16daObjC0_Switch_cD0Ev.cpp
 *   [2] 0x0211123c  src/_ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c.cpp
 *   [3] 0x021112ec  src/_ZN16daObjC0_Switch_c16CleanupResourcesEv.cpp
 *   [4] 0x02111324  src/_ZN16daObjC0_Switch_c6RenderEv.cpp
 *   [5] 0x0211134c  src/_ZN16daObjC0_Switch_c8BehaviorEv.cpp
 *   [6] 0x02111370  src/_ZN16daObjC0_Switch_c13InitResourcesEv.cpp
 *   [7] 0x02111420  src/daObjC0_Switch_c_Spawn.c
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. This order compiles and byte-matches as written. */
#include "daObjC0_Switch_c.h"
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"

/* Local declarations carried from the legacy files. These are the remaining
 * un-reconciled shapes in this TU and the only follow-up work it leaves:
 * `Base`/`Derived` are placeholder shadow structs, and the factory below still
 * stores the vptr by hand through the mangled _ZTV symbol rather than by
 * constructing the class. Both byte-match as written; replacing them is a
 * separate readability change that must be re-proved against the ROM. */
/* shadow struct 'Base' */
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void m(int); };

/* shadow struct 'Derived' */
struct Derived { char pad[0xd4]; Base base; };

extern "C" {
extern int _ZTV16daObjC0_Switch_c[];
extern int _ZTV10dBgActor_c[];
extern void *data_020a0eac;
extern int data_0209caa0[];
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov012_021124a8[];
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern int data_ov012_021124a0[];
extern int data_ov012_02111cd0[];
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daObjC0_Switch_c_Spawn, 0x02111420, size 0x30 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *daObjC0_Switch_c_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(800);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)(_ZTV16daObjC0_Switch_c + 2); }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN16daObjC0_Switch_c13InitResourcesEv, 0x02111370, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c13InitResourcesEv
/* daObjC0_Switch_c::InitResources -- vtable slot 0, ov012 0x02111370.
 *
 * Real member function on real fields, same idiom as
 * src/_ZN11VolcanoRing13InitResourcesEv.cpp (#1583): mModel/mMeshCollider/
 * mClsnMat/mAngleY are dBgActor_c's/dActor_c's own named fields
 * (include/dBgActor_c.h, include/dActor_c.h). ModelBase::SetFile,
 * UpdateModelPosAndRotY and UpdateClsnPosAndRot are real method calls.
 * Model::LoadFile, dBgW_Kc::LoadFile and dBgW_KcMbg::SetFile
 * stay extern "C" under their exact ROM symbols -- dBgW_KcMbg::SetFile
 * takes Fix12<int> BY VALUE, which cannot be declared callable here without
 * changing the argument-passing ABI (mwccarm-codegen.md 6az).
 *
 * data_0209caa0 is the same untyped shared-flags global other TUs reference
 * raw (e.g. src/func_ov002_020d4748.cpp); mPosY -= 0x64000 and mPressed = 1
 * only run when data_0209caa0[2] & 0x80000 is set -- some other pillar in the
 * group already reported pressed (see OnGroundPounded), so this one starts
 * sunk and pre-flagged. */
int daObjC0_Switch_c::InitResources()
{
    void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov012_021124a8);
    mModel.SetFile((BMD_File *)mdl, 1, -1);
    if (data_0209caa0[2] & 0x80000) {
        mPosY -= 0x64000;
        mPressed = 1;
    }
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov012_021124a0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY, data_ov012_02111cd0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN16daObjC0_Switch_c8BehaviorEv, 0x0211134c, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c8BehaviorEv
// recovered name: daObjC0_Switch_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjC0_Switch_c::Behavior - recovered from vtable slot identity */
s32 daObjC0_Switch_c::Behavior() {
    void * c = (void *)this;
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN16daObjC0_Switch_c6RenderEv, 0x02111324, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c6RenderEv
// recovered name: daObjC0_Switch_c_Render
/* recovered: renamed to Class_Method */
/* daObjC0_Switch_c::Render - recovered from vtable slot identity */
s32 daObjC0_Switch_c::Render() {
    Derived * d = (Derived *)this; Base *b = &d->base; b->m(0); return 1; }

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjC0_Switch_c16CleanupResourcesEv, 0x021112ec, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c16CleanupResourcesEv
// recovered name: daObjC0_Switch_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
/* recovered: renamed to Class_Method */
/* daObjC0_Switch_c::CleanupResources - recovered from vtable slot identity */
s32 daObjC0_Switch_c::CleanupResources() {
    void * t = (void *)this;
    _ZN4dBgW7DisableEv((char *)t + 0x124);
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124a8);
    _ZN13SharedFilePtr7ReleaseEv(data_ov012_021124a0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c, 0x0211123c, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c
/* daObjC0_Switch_c::OnGroundPounded -- vtable slot 21, ov012 0x0211123c.
 *
 * mPosY and mPressed are the same fields InitResources touches.
 * dActor_c::FindWithActorID walks every actor of a given actorID (0x22, this
 * class's own -- see include/fBase_c.h actorID). This one no-ops if already
 * pressed; otherwise it sinks, marks itself pressed, then walks every OTHER
 * actorID-0x22 pillar looking for one already pressed, and if it finds one,
 * sets the shared group flag in data_0209caa0[2] that InitResources reads. */
void daObjC0_Switch_c::OnGroundPounded(dActor_c &other)
{
    if (mPressed) return;
    mPosY -= 0x64000;
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mPressed = 1;
    dActor_c *p = dActor_c::FindWithActorID(0x22, 0);
    if (p == 0) return;
check:
    if (p != (dActor_c *)this) {
        if (((daObjC0_Switch_c *)p)->mPressed) {
            data_0209caa0[2] |= 0x80000;
        }
        return;
    }
    p = dActor_c::FindWithActorID(0x22, p);
    if (p != 0) goto check;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN16daObjC0_Switch_cD1Ev 0x021111a0 size 0x44 and   */
/* _ZN16daObjC0_Switch_cD0Ev 0x021111e4 size 0x58 -- are NOT written here.      */
/*                                                                             */
/* The destructor is defined INLINE in include/daObjC0_Switch_c.h. Written      */
/* out-of-line here instead, mwcc emits D0 BEFORE D1, which is the reverse of   */
/* the cartridge's own order and makes objisolate refuse the whole TU; it also  */
/* emits a third D2 base-object body with no ROM home. The inline definition    */
/* gives the retail D1/D0 pair in ROM order and no D2, while InitResources --   */
/* the first out-of-line virtual -- keeps this TU as the class's key-function   */
/* TU, so it still owns the complete _ZTV/_ZTI/_ZTS group declared in this      */
/* entry's compiler_only_output.                                                */
/* -------------------------------------------------------------------------- */
