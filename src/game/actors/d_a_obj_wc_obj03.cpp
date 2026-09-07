//cpp
/* PROMOTED translation unit -- ov029/daObjWc_Obj03_c (10 function(s)).
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object, the way the
 * cartridge's own build did. It licenses the contiguous .text run
 * 0x02111728..0x02111ac4 in ov029 (config/tu_manifest.d/ov029/daObjWc_Obj03_c.json,
 * config/arm9/overlays/ov029/delinks.txt).
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm emits one .text section per function
 * in the reverse of source order, so the highest-address ROM function is
 * written FIRST and the lowest last. Do not reorder these blocks.
 *
 * WHY D1 AND D0 ARE MANGLED BODIES RATHER THAN A REAL DESTRUCTOR.
 * The cartridge keeps D1 at 0x02111728, BELOW D0 at 0x02111760. Defining
 * `daObjWc_Obj03_c::~daObjWc_Obj03_c()` as a real C++ member makes mwccarm emit
 * the D2/D1/D0 triple as ONE group in the order D0-then-D1, which objisolate
 * refuses with `requested functions are not emitted in ROM order`; the group
 * also carries an unhomed `_ZN15daObjWc_Obj03_cD2Ev`. Writing the two variants
 * as separate `// @symbol` marked bodies under their mangled names is the only
 * placement that reaches both ROM addresses. It costs the CONVERTED tier's
 * no-raw-offset and no-mangled-refs criteria for those two functions, banked in
 * config/converted-backslide-exceptions.jsonl. Byte-match over readability.
 * The manifest flagged this before the promotion was attempted, as
 * `functions_occur_in_expected_order: PARTIAL -- [(0, 1)]`.
 *
 * THE VPTR STORES CARRY NO BIAS. symbols.txt records _ZTV15daObjWc_Obj03_c at
 * 0x02113db4, which IS the vtable's address point -- the cartridge's literal
 * pool word at 0x0211175c holds exactly that value. So the stores below name
 * the symbol with addend 0; `&_ZTV15daObjWc_Obj03_c[2]` would ask production
 * isolation to rewrite an undefined RTTI reference by 8 and is refused.
 *
 * THIS TU CARRIES THE CLASS'S VTABLE AND RTTI AS VAGUE LINKAGE. With no key
 * function defined anywhere, mwccarm emits _ZTV/_ZTI/_ZTS15daObjWc_Obj03_c plus
 * the base chain here -- but only because include/daObjWc_Obj03_c.h declares
 * ~daObjWc_Obj03_c() AFTER the other members. Declared first, this file emits no
 * data at all and those three ROM records go unverified by any source. The
 * emitted symbols are licensed as `deadstrip-data` in the manifest; production
 * isolation discards them after tools/romdata_check.py compares them to the
 * cartridge.
 *
 * Absorbed these legacy one-function sources (ROM address order), all deleted
 * by tools/tu_promote.py:
 *   [0] 0x02111728  src/_ZN15daObjWc_Obj03_cD1Ev.cpp
 *   [1] 0x02111760  src/_ZN15daObjWc_Obj03_cD0Ev.cpp
 *   [2] 0x021117ac  src/_ZN15daObjWc_Obj03_c19CheckClsnWithPlayerEv.cpp
 *   [3] 0x02111850  src/_ZN15daObjWc_Obj03_c10SetWaterIDEv.cpp
 *   [4] 0x021118c8  src/_ZN15daObjWc_Obj03_c20UpdateModelTransformEv.cpp
 *   [5] 0x02111908  src/_ZN15daObjWc_Obj03_c16CleanupResourcesEv.cpp
 *   [6] 0x0211192c  src/_ZN15daObjWc_Obj03_c6RenderEv.cpp
 *   [7] 0x02111954  src/_ZN15daObjWc_Obj03_c8BehaviorEv.cpp
 *   [8] 0x02111a04  src/_ZN15daObjWc_Obj03_c13InitResourcesEv.cpp
 *   [9] 0x02111a84  src/daObjWc_Obj03_c_classInit.c
 */

/* Matrix4x3 has two 0x30-byte spellings and whichever a TU sees FIRST stands
 * (include/common.h). decl_common.h below reaches the flat `s32 m[12]` one,
 * while UpdateModelTransform writes the translation by name as `.t`, so the
 * structured spelling is pulled in here, ahead of everything else. */
#include "math/Matrix.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- daObjWc_Obj03_c_classInit, 0x02111a84, size 0x40 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* .c-derived member: C linkage for the whole block */
// @symbol daObjWc_Obj03_c_classInit
/* The pinned compiler cannot spell the ROM's folded factory in C++: `new
 * daObjWc_Obj03_c` calls the unavailable global _Znwm, while the cartridge calls
 * fBase_c::operator new. Placement new and class-specific operator new are
 * rejected by mwccarm 2004/b56 (notes/ctor-migration.md section 5d), so this
 * remains the truthful C transcription of the compiler-generated sequence. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV15daObjWc_Obj03_c */
/* Historical alias: WaterDiamond_Spawn -- this entry point carried the
 * project's coined class name until the ROM's RTTI name was adopted.
 * The ROM proves the class through RTTI (_ZTS15daObjWc_Obj03_c at
 * 0x02113d7c), the 0x160 allocation below, the vtable at 0x02113db4 and
 * the WC_OBJ03 registry profile at 0x02113d90. */
int *daObjWc_Obj03_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(352);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV15daObjWc_Obj03_c; /* the address point; the cartridge stores 0x02113db4 */
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN7dCcAc_cC1Ev((char *)p + 0x124);
    }
    return p;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN15daObjWc_Obj03_c13InitResourcesEv, 0x02111a04, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Obj03_c.h"
#include "SharedFilePtr.h"

/* This one call deliberately retains the ABI-spelled entry point. Its ROM
 * signature takes Fix12<int> by value; asking this compiler to pass the real
 * class type homes the register arguments to the stack and grows the caller.
 * The scalar view below is the measured 2004/b56 codegen wall, not a guessed
 * replacement API. */
extern "C" {
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *, Fix12i, Fix12i, u32, u32);
}

/* decl_common.h, pulled in by the classInit block above, already declares this
 * one as `int[]`; spelling it `SharedFilePtr` a second time is a redeclaration
 * under 2004/b56, so the two uses below cast at the point of use instead. */
int daObjWc_Obj03_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov029_02114270), 1, -1);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x32000, 0x64000, 0x800002, 0);
    mWaterID = 0;
    mWaterParam = param1 & 1;
    mActive = 0;
    mAngleY = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN15daObjWc_Obj03_c8BehaviorEv, 0x02111954, size 0xb0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Obj03_c.h"
#include "daObjWc_Mizu_c.h"

int daObjWc_Obj03_c::Behavior()
{
    SetWaterID();
    if (mWaterID == 0) return 1;
    CheckClsnWithPlayer();
    if (mActive != 0) {
        /* The address-valued temporary is load-bearing under 2004/b56: the
         * cartridge keeps r1 as &mAngleY for both the halfword load and store. */
        s16 *angleY = (s16 *)((int)(char *)this + 0x8e);
        *angleY = *angleY + 0x400;
        daObjWc_Mizu_c *water = (daObjWc_Mizu_c *)dActor_c::FindWithID(mWaterID);
        if (water != 0) {
            if (mPosY == water->mPosY) {
                if (mAngleY == 0) mActive = 0;
            }
            if (mPosY != water->mTargetPosY) {
                mActive = 0;
                mAngleY = 0;
            }
        }
    }
    UpdateModelTransform();
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN15daObjWc_Obj03_c6RenderEv, 0x0211192c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Obj03_c.h"

int daObjWc_Obj03_c::Render()
{
    Model *model = &mModel;
    model->Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN15daObjWc_Obj03_c16CleanupResourcesEv, 0x02111908, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c16CleanupResourcesEv
/* daObjWc_Obj03_c::CleanupResources -- vtable slot 3. Releases the one shared
 * file the class holds; it never touches `this`, which is why the legacy C form
 * could declare itself nullary and still reproduce. */
#include "daObjWc_Obj03_c.h"
#include "SharedFilePtr.h"

int daObjWc_Obj03_c::CleanupResources()
{
    ((SharedFilePtr *)data_ov029_02114270)->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN15daObjWc_Obj03_c20UpdateModelTransformEv, 0x021118c8, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c20UpdateModelTransformEv
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Obj03_c.h"

extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *, int);

void daObjWc_Obj03_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN15daObjWc_Obj03_c10SetWaterIDEv, 0x02111850, size 0x78 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c10SetWaterIDEv
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Obj03_c.h"
#include "daObjWc_Mizu_c.h"

void daObjWc_Obj03_c::SetWaterID()
{
  dActor_c *water;
  if (mWaterID != 0) return;
  water = 0;
  for (;;) {
    water = dActor_c::FindWithActorID(0x65, water);
    if (water == 0) goto done;
    if (mWaterParam == ((daObjWc_Mizu_c *)water)->mUseSpawnPosY) break;
  }
  mWaterID = water->uniqueID;
done:
  if (mWaterID != 0) return;
  MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN15daObjWc_Obj03_c19CheckClsnWithPlayerEv, 0x021117ac, size 0xa4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c19CheckClsnWithPlayerEv
/* recovered: named members + shared header, real C++ method */
#include "daObjWc_Obj03_c.h"
#include "daObjWc_Mizu_c.h"

struct Sound {
    static void PlayBank3(u32 id, const Vector3 &pos);
};

void daObjWc_Obj03_c::CheckClsnWithPlayer()
{
    if (mActive != 0)
        return;
    u32 id = mCylinder.otherOwner;
    if (id == 0)
        return;
    dActor_c *actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return;
    int isPlayer = (int)(actor->actorID == 0xbf);
    if (isPlayer == 0)
        return;
    daObjWc_Mizu_c *water = (daObjWc_Mizu_c *)dActor_c::FindWithID(mWaterID);
    int targetPosY = water->mTargetPosY;
    if (targetPosY != water->mPosY)
        return;
    if (targetPosY == mPosY)
        return;
    mActive = 1;
    water->mTargetPosY = mPosY;
    Sound::PlayBank3(0x63, *(Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN15daObjWc_Obj03_cD0Ev, 0x02111760, size 0x4c */
/* -------------------------------------------------------------------------- */
extern "C" {  /* mangled body: C linkage so the name is emitted verbatim */
// @symbol _ZN15daObjWc_Obj03_cD0Ev
/* D0 is the DELETING destructor: destroy through this class and its bases, then
 * return the object to its heap. The single vptr store is the class's own --
 * dActor_c's destructor is out of line, so the base chain is reached by the
 * _ZN8dActor_cD2Ev call rather than by a second store here.
 *
 * Written as a mangled body rather than as a real `~daObjWc_Obj03_c()` member,
 * for the emission-order reason this file's banner gives. */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* vtable identified: VT0 = _ZTV15daObjWc_Obj03_c */
extern void *data_020a0eac;
int *_ZN15daObjWc_Obj03_cD0Ev(int *t)
{
    t[0] = (int)_ZTV15daObjWc_Obj03_c;
    _ZN7dCcAc_cD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN15daObjWc_Obj03_cD1Ev, 0x02111728, size 0x38 */
/* -------------------------------------------------------------------------- */
extern "C" {  /* mangled body: C linkage so the name is emitted verbatim */
// @symbol _ZN15daObjWc_Obj03_cD1Ev
/* One vptr store and three destructor calls, every one a consequence of
 * `struct daObjWc_Obj03_c : dActor_c`: its own vptr, then its own dCcAc_c member
 * at +0x124 and Model at +0xd4 in reverse construction order, then dActor_c.
 *
 * A mangled body for the same emission-order reason given above D0. */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
/* vtable identified: VT0 = _ZTV15daObjWc_Obj03_c */
int *_ZN15daObjWc_Obj03_cD1Ev(int *t)
{
    t[0] = (int)_ZTV15daObjWc_Obj03_c;
    _ZN7dCcAc_cD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
}
