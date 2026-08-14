//cpp
/* SHADOW translation unit -- ov045 / PoleLift (RTTI name daObjKm2_Ami_Bou_c).
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
 * PoleLift_Spawn, InitResources, Behavior, Render, CleanupResources, ~PoleLift.
 * The rule holds between distinct definitions but NOT inside the destructor's
 * variant group -- see the report.
 *
 * THIS TU ALSO OWNS, and this round neither declares nor verifies any of it:
 *   _ZTV8PoleLift       .data  0x02112dbc (slot array; the C++ object starts
 *                              8 bytes earlier, at 0x02112db4)
 *   _ZTI8PoleLift       .data  0x02112d74
 *   PoleLift_SpawnInfo  .data  0x02112d98
 *   PoleLift_ClsnFile   .bss   0x021131d0
 *   PoleLift_ModelFile  .bss   0x021131d8
 *   a static initialiser in .init
 * Compiling the class's key function here does emit a vtable, RTTI records and
 * Platform's out-of-line destructors as a side effect. That is expected and is
 * inventoried in the report, not licensed.
 */
#include "decl_common.h"
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_Platform.h"

#include "PoleLift.h"
#include "SharedFilePtr.h"
#include "MeshColliderBase.h"

/* Reconciled from the six per-function copies. Every one of these is a ROM
 * symbol spelled by its final mangled name, so it needs C linkage: seen as C++
 * the compiler would mangle the already-mangled spelling a second time and emit
 * a reference nothing defines. The per-function files declared these across six
 * files; UpdateModelPosAndRotY and UpdateClsnPosAndRot appeared twice each with
 * identical signatures, so nothing was in conflict -- only repeated. */
extern "C" {
extern void  _ZN12CylinderClsn5ClearEv(void *self);
extern void  _ZN12CylinderClsn6UpdateEv(void *self);
extern void  _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern int   _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);
extern void  _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
extern void  _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        void *thiz, void *kcl, void *mtx, int fix, short s, void *clps);
extern void  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        void *thiz, void *act, int fix, int t, unsigned int a, unsigned int b);

/* Not in any decl_*.h; the Behavior file declared it locally. A plain global's
 * name is not mangled by this ABI either way, but the linkage is stated rather
 * than left to that accident. */
extern short data_02082214[];
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- PoleLift_Spawn, 0x02111808, size 0x38                     */
/* ------------------------------------------------------------------------- */
/* recovered: vtable identified, globals resolved, declarations from a shared header */
/* resolved: VT0 = _ZTV8PoleLift
 *
 * C LINKAGE IS LOAD-BEARING: the ROM symbol is the bare name. The legacy file
 * was a C translation unit and got that for free; in a merged C++ TU it has to
 * be asked for.
 *
 * THE `+ 2` IS ALSO LOAD-BEARING, and it is the one thing consolidation forced.
 * decl_common.h declares `_ZTV8PoleLift` by the ROM's convention, where the
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
extern "C" int *PoleLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(856);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)(_ZTV8PoleLift + 2);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x320);
    }
    return p;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- PoleLift::InitResources, 0x02111738, size 0xd0            */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method */
int PoleLift::InitResources()
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov045_021131b0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, -1);
    if ((*(s32 *)&param1) != 0xffff) {
        int* p = (int*)(((int)((char*)this) + 0x60));
        *p -= 0x12c000;
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
    void* mc = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov045_021131a8);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, mc, ((char*)this) + 0x2ec, 0x199, mAngleY, data_ov045_02112510);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        ((char*)this) + 0x320, ((char*)this), 0x35555, 0x258000, 0x280000c, 0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- PoleLift::Behavior, 0x02111618, size 0x120                */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method */
int PoleLift::Behavior()
{
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsn);
    if ((*(s32 *)&param1) != 0xffff) {
        int idx = unk_354 >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) + (int)(((long long)s * 0x7000 + 0x800) >> 12);
    } else {
        int idx = unk_354 >> 4;
        int s = *(short*)((char*)data_02082214 + (idx << 2));
        *(int*)(((int)((char *)this) + 0x60)) =
            *(int*)(((int)((char *)this) + 0x60)) - (int)(((long long)s * 0x3000 + 0x800) >> 12);
    }
    *(short*)(((int)((char *)this) + 0x354)) =
        *(short*)(((int)((char *)this) + 0x354)) + 0x100;
    _ZN8Platform21UpdateModelPosAndRotYEv(((char *)this));
    if (_ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(((char *)this), 0x400000, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
    }
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- PoleLift::Render, 0x021115f0, size 0x28                   */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method
 *
 * The per-function file spelled this with two local shadow types -- a `struct
 * Base` of six virtuals and a `struct Derived { char pad[0xd4]; Base base; }` --
 * which are Platform's Model at +0xd4 and its slot-5 Render(const Vector3 *).
 * Both names are far too generic to survive in a TU that includes the project
 * headers, and both describe something include/Model.h already declares
 * correctly. Reconciled onto the real member: byte-identical, measured.
 */
int PoleLift::Render()
{
    mModel.Render(0);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- PoleLift::CleanupResources, 0x021115b8, size 0x38         */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method
 *
 * The per-function file reached the collider through
 * `((MeshColliderBase *)((char *)&(*(u8 *)&mMeshCollider)))`, which laundered a
 * `u8` marker back into a class pointer. Platform's C++ half types that member
 * as a real MovingMeshCollider, and MovingMeshCollider : MeshCollider :
 * MeshColliderBase, so the cast chain was describing an inheritance the headers
 * already know about. Reconciled onto the member call: byte-identical, measured.
 */
int PoleLift::CleanupResources()
{
    mMeshCollider.Disable();
    ((SharedFilePtr *)(data_ov045_021131b0))->Release();
    ((SharedFilePtr *)(data_ov045_021131a8))->Release();
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one definition, two ROM-visible variants:          */
/*   _ZN8PoleLiftD1Ev  0x0211150c  size 0x4c   (complete-object destructor)   */
/*   _ZN8PoleLiftD0Ev  0x02111558  size 0x60   (deleting destructor)          */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ destructor -- the compiler emits the whole body.
 *
 * Two vtable stores and three destructor calls, every one a consequence of
 * `struct PoleLift : Platform`: its own vptr, then Platform's -- inlined,
 * because Platform's destructor is defined in its class body -- then Platform's
 * Model and MovingMeshCollider, then Actor. This class adds no member with a
 * destructor of its own. D0 additionally returns the object to its heap through
 * the inline operator delete it inherits, which is why nothing here mentions a
 * heap.
 *
 * In the one-function tree these were two files, each carrying this same empty
 * body and each having objisolate keep the variant its filename named. In TU
 * context one definition produces both -- and a third, _ZN8PoleLiftD2Ev, which
 * is byte-identical to D1 and which the ROM does not contain. See the report:
 * that third section is the one thing standing between this file and a
 * whole-range link.
 */
PoleLift::~PoleLift()
{
}
