#ifndef DAOBJRCCARPET_C_H
#define DAOBJRCCARPET_C_H

#include "types.h"

/* The flying carpet. Its factory is FlyingCarpet_Spawn, and its own C++ base is
 * PathLift (include/PathLift.h) -- RTTI's own class for that level is
 * `dPathLiftActor_c` (ov002:0x0210af0c), but every mangled symbol in the ROM
 * spells the base `PathLift` (_ZTV8PathLift, _ZN8PathLiftD1Ev, ...), so this
 * header follows PathLift.h and daObjPathLift_c.h's precedent (#1589) and uses
 * that name. daObjPathLift_c (ov100) is a direct sibling of this class under
 * the same intermediate.
 *
 *   _ZTI15daObjRcCarpet_c  ov036 0x02113f4c
 *   _ZTS15daObjRcCarpet_c  ov036 0x02113f64
 *   _ZTV15daObjRcCarpet_c  ov036 0x02113f9c  (its record sits at V-4)
 *   kind  __si_class_type_info, ONE base, subobject offset 0
 *   base  dPathLiftActor_c (= PathLift), ov002 0x0210af0c
 *
 * SIZE 0x4c0, the literal FlyingCarpet_Spawn (src/game/actors/FlyingCarpet/FlyingCarpet_Spawn.cpp) passes
 * to fBase_c::operator new. PathLift ends 0x450; the D1 destructor
 * (func_ov036_02112158, this class's own) destroys only a ModelAnim at 0x450
 * before storing PathLift's own vtable and running PathLift's inlined cleanup
 * (Model[3] array), so ModelAnim is this class's only member with a
 * constructor/destructor of its own -- matching the ONE call
 * (_ZN9ModelAnimD1Ev) the D1 relocations show, beyond the calls
 * include/daObjPathLift_c.h's own D1 already documents for PathLift's own
 * teardown (__destroy_arr, MovingMeshColliderD1Ev, ModelD1Ev, dActor_cD2Ev).
 *
 * A PathPtr is also constructed, at +0x430 -- inside PathLift's own generic
 * tail padding (PathLift.h's pad_42c[0x20]), not this class's own storage.
 * daObjPathLift_c_Spawn (src/game/actors/PathLift/PathLift_Spawn.cpp) builds one at the identical
 * offset for its sibling, so this is PathLift's own construction sequence,
 * replayed identically by every PathLift descendant's still-hand-rolled Spawn
 * function; it is trivially destructible (include/PathPtr.h declares no
 * destructor), which is why no D1 call destroys it. Not declared as a member
 * here, for the same reason include/daObjPathLift_c.h does not declare
 * PathLift's own unk_428/unk_42c/unk_43c/unk_440: a derived struct cannot place
 * a member inside its base's own byte range.
 *
 * unk_4bc, read/written only by this class's own Behavior/func_ov036_021122c0/
 * func_ov036_0211224c as a plain accumulator for _Z14ApproachLinearRiii, closes
 * the object exactly: 0x4bc + 4 = 0x4c0. The 8 bytes at 0x4b4..0x4bc are
 * unwitnessed by any reachable function and stay explicit padding.
 */

#ifdef __cplusplus

#include "PathLift.h"
#include "ModelAnim.h"

struct daObjRcCarpet_c : PathLift {
    ModelAnim mModelAnim;      /* 0x450 */
    u8  pad_4b4[0x8];
    s32 unk_4bc;                /* 0x4bc */

    /* THE DESTRUCTOR STAYS A DECLARATION ONLY (never defined as a real method),
       the same reason include/daObjPathLift_c.h gives for its own: PathLift's
       own destructor is genuinely out-of-line, not inlined like dBgActor_c's,
       so a real `daObjRcCarpet_c::~daObjRcCarpet_c() {}` would call a
       `_ZN8PathLiftD2Ev` that does not exist in the ROM. D1/D0 stay
       hand-written free functions under the correct mangled names in
       src/game/actors/daObjRcCarpet_c/_ZN15daObjRcCarpet_cD1Ev.cpp and src/_ZN15daObjRcCarpet_cD0Ev.c. */
    virtual ~daObjRcCarpet_c();

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

typedef char daObjRcCarpet_c_size_must_be_0x4c0[sizeof(daObjRcCarpet_c) == 0x4c0 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJRCCARPET_C_H */
