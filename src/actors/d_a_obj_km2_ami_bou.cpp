//cpp
/* SHADOW translation unit -- ov045 / daObjKm2_Ami_Bou_c (RTTI name daObjKm2_Ami_Bou_c).
 *
 * NOT ENROLLED, AND NOT CANONICAL. This file contributes nothing to the ROM
 * build. The seven one-function sources under src/ remain the enrolled owners of
 * 0x0211150c..0x02111840. See notes/translation-unit-reconstruction-plan.md
 * (the design) and notes/tu-reconstruction-pilot-report.md (what this measured).
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order. Confirmed by probe against this
 * exact compiler rather than assumed; the reading order below is therefore
 * daObjKm2_Ami_Bou_c_classInit, InitResources, Behavior, Render,
 * CleanupResources, ~daObjKm2_Ami_Bou_c.
 * The rule holds between distinct definitions but NOT inside the destructor's
 * variant group -- see the report.
 *
 * THIS TU ALSO OWNS, and this round neither declares nor verifies any of it:
 *   _ZTV18daObjKm2_Ami_Bou_c       .data  0x02112dbc (slot array; the C++ object starts
 *                              8 bytes earlier, at 0x02112db4)
 *   _ZTI18daObjKm2_Ami_Bou_c       .data  0x02112d74
 *   g_profile_KM2_AMI_BOU .data 0x02112d98
 *                          (historical alias daObjKm2_Ami_Bou_c_SpawnInfo)
 *   daObjKm2_Ami_Bou_c_ClsnFile   .bss   0x021131d0
 *   daObjKm2_Ami_Bou_c_ModelFile  .bss   0x021131d8
 *   a static initialiser in .init
 * Compiling the class's key function here does emit a vtable, RTTI records and
 * dBgActor_c's out-of-line destructors as a side effect. That is expected and is
 * inventoried in the report, not licensed.
 */
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_Platform.h"

#include "daObjKm2_Ami_Bou_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

/* Reconciled from the six per-function copies. Every one of these is a ROM
 * symbol spelled by its final mangled name, so it needs C linkage: seen as C++
 * the compiler would mangle the already-mangled spelling a second time and emit
 * a reference nothing defines. The per-function files declared these across six
 * files; UpdateModelPosAndRotY and UpdateClsnPosAndRot appeared twice each with
 * identical signatures, so nothing was in conflict -- only repeated. */
extern "C" {
extern void  _ZN5dCc_c5ClearEv(void *self);
extern void  _ZN5dCc_c6UpdateEv(void *self);
extern void  _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern int   _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern void  _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
extern void *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *thiz, void *kcl, void *mtx, int fix, short s, void *clps);
extern void  _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        void *thiz, void *act, int fix, int t, unsigned int a, unsigned int b);

/* Not in any decl_*.h; the Behavior file declared it locally. A plain global's
 * name is not mangled by this ABI either way, but the linkage is stated rather
 * than left to that accident. */
extern short data_02082214[];
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- daObjKm2_Ami_Bou_c_classInit, 0x02111808, size 0x38      */
/* ------------------------------------------------------------------------- */
/* recovered: vtable identified, globals resolved, declarations from a shared header */
/* resolved: VT0 = _ZTV18daObjKm2_Ami_Bou_c
 *
 * C LINKAGE IS LOAD-BEARING: the ROM symbol is the bare name. The legacy file
 * was a C translation unit and got that for free; in a merged C++ TU it has to
 * be asked for.
 *
 * THE `+ 2` IS ALSO LOAD-BEARING, and it is the one thing consolidation forced.
 * decl_common.h declares `_ZTV18daObjKm2_Ami_Bou_c` by the ROM's convention, where the
 * symbol IS the slot array. mwcc's own convention -- and this TU now defines
 * the vtable, because it defines the class's key function -- puts the symbol at
 * the start of the vtable OBJECT, two words of Itanium preamble earlier. The
 * legacy C file could write the bare symbol because it only ever referenced an
 * external one; here the same spelling would bind to the local definition and
 * store a vptr eight bytes low. The bytes are identical either way, because a
 * relocated word is a wildcard to every byte gate -- objisolate's addend check
 * is what catches it, and it refuses the addend-0 form outright. Writing `+ 2`
 * (int-indexed, so eight bytes) makes this reference agree with the addend-8
 * stores the compiler itself emits in the destructors below.
 */
/* Reconstructed source-style name: SM64DS proves daObjKm2_Ami_Bou_c through
 * RTTI, allocation size, vtable identity, and the KM2_AMI_BOU registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: daObjKm2_Ami_Bou_c_Spawn. */
// @symbol daObjKm2_Ami_Bou_c_classInit
extern "C" int *daObjKm2_Ami_Bou_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(856);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x02112db4 and +8 is what reaches the
         * 0x02112dbc address point the ROM stores. The addend-0 spelling was
         * right only while the class merely IMPORTED its vtable. */
        p[0] = (int)&_ZTV18daObjKm2_Ami_Bou_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0x320);
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* g_profile_KM2_AMI_BOU, 0x02112d98, size 0x1c                               */
/* -------------------------------------------------------------------------- */
/* .data objects emit in SOURCE order, so the descriptor is written directly
 * after the factory it names -- which is where the ROM's run puts it, between
 * _ZTS (0x02112d80) and the vtable object (0x02112db4). */
struct Km2AmiBouSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x008e */
    s16 drawOrder;                  /* 0x00d5 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;                 /* 0x1000 == 1.0 */
    s32 clipDistance;
    s32 farDistance;
};
typedef char Km2AmiBouSpawnInfo_size_must_be_0x1c[
    sizeof(Km2AmiBouSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_KM2_AMI_BOU
extern "C" Km2AmiBouSpawnInfo g_profile_KM2_AMI_BOU = {
    daObjKm2_Ami_Bou_c_classInit, 0x008e, 0x00d5, 0x00000002,
    0x00100000, 0x00300000, 0x01000000, 0x00000000
};

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- daObjKm2_Ami_Bou_c::InitResources, 0x02111738, size 0xd0            */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method */
// @symbol _ZN18daObjKm2_Ami_Bou_c13InitResourcesEv
int daObjKm2_Ami_Bou_c::InitResources()
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov045_021131b0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, -1);
    if ((*(s32 *)&param1) != 0xffff) {
        int* p = (int*)(((int)((char*)this) + 0x60));
        *p -= 0x12c000;
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    void* mc = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov045_021131a8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, mc, ((char*)this) + 0x2ec, 0x199, mAngleY, data_ov045_02112510);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        ((char*)this) + 0x320, ((char*)this), 0x35555, 0x258000, 0x280000c, 0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjKm2_Ami_Bou_c::Behavior, 0x02111618, size 0x120                */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method */
// @symbol _ZN18daObjKm2_Ami_Bou_c8BehaviorEv
int daObjKm2_Ami_Bou_c::Behavior()
{
    _ZN5dCc_c5ClearEv((char *)&mdCcAc_c);
    _ZN5dCc_c6UpdateEv((char *)&mdCcAc_c);
    if ((*(s32 *)&param1) != 0xffff) {
        int idx = mHeightAng >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) + (int)(((long long)s * 0x7000 + 0x800) >> 12);
    } else {
        int idx = mHeightAng >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) - (int)(((long long)s * 0x3000 + 0x800) >> 12);
    }
    *(short*)(((int)((char *)this) + 0x354)) =
        *(short*)(((int)((char *)this) + 0x354)) + 0x100;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char *)this));
    if (_ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(((char *)this), 0x400000, 0)) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char *)this));
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- daObjKm2_Ami_Bou_c::Render, 0x021115f0, size 0x28                   */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method
 *
 * The per-function file spelled this with two local shadow types -- a `struct
 * Base` of six virtuals and a `struct Derived { char pad[0xd4]; Base base; }` --
 * which are dBgActor_c's Model at +0xd4 and its slot-5 Render(const Vector3 *).
 * Both names are far too generic to survive in a TU that includes the project
 * headers, and both describe something include/Model.h already declares
 * correctly. Reconciled onto the real member: byte-identical, measured.
 */
// @symbol _ZN18daObjKm2_Ami_Bou_c6RenderEv
int daObjKm2_Ami_Bou_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- daObjKm2_Ami_Bou_c::CleanupResources, 0x021115b8, size 0x38         */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method
 *
 * The per-function file reached the collider through
 * `((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))`, which laundered a
 * `u8` marker back into a class pointer. dBgActor_c's C++ half types that member
 * as a real dBgW_KcMbg, and dBgW_KcMbg : dBgW_Kc :
 * dBgW, so the cast chain was describing an inheritance the headers
 * already know about. Reconciled onto the member call: byte-identical, measured.
 */
// @symbol _ZN18daObjKm2_Ami_Bou_c16CleanupResourcesEv
int daObjKm2_Ami_Bou_c::CleanupResources()
{
    mMeshCollider.Disable();
    ((SharedFilePtr *)(data_ov045_021131b0))->Release();
    ((SharedFilePtr *)(data_ov045_021131a8))->Release();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN18daObjKm2_Ami_Bou_cD1Ev  0x0211150c  size 0x4c   (complete-object destructor)   */
/*   _ZN18daObjKm2_Ami_Bou_cD0Ev  0x02111558  size 0x60   (deleting destructor)          */
/* ------------------------------------------------------------------------- */
/* The destructor is defined in the class body in include/daObjKm2_Ami_Bou_c.h, and is
 * deliberately NOT repeated out of line here. An out-of-line definition makes
 * mwccarm emit D2, D0, D1 -- and the cartridge holds D1, D0 with no D2 at all.
 * Defined inline, the compiler emits exactly the ROM's two variants, in the
 * ROM's order, from this translation unit.
 *
 * The body itself is two vtable stores and three destructor calls, every one a
 * consequence of `struct daObjKm2_Ami_Bou_c : dBgActor_c`: its own vptr, then
 * dBgActor_c's -- inlined, because dBgActor_c's destructor is defined in its
 * class body -- then dBgActor_c's Model and dBgW_KcMbg, then dActor_c. This
 * class adds no member with a destructor of its own. D0 additionally returns
 * the object to its heap through the inline operator delete it inherits, which
 * is why nothing mentions a heap.
 */

// @symbol _ZN18daObjKm2_Ami_Bou_cD0Ev
// @symbol _ZN18daObjKm2_Ami_Bou_cD1Ev
