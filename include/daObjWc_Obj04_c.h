/* class daObjWc_Obj04_c -- the switch-triggered wooden plank of ov029.
 * Rebuilt from the flat auto-generated layout onto the real inheritance chain
 * once dBgActor_c was reconstructed (#1747): every field this header used to
 * spell between 0x000 and 0x320 is an inherited member, not our own.
 *
 *   0x08e mAngleY              -> dActor_c::mAngleY
 *   0x0d4 "mModel1"            -> dBgActor_c::mModel
 *   0x124 "mMovingMeshCollider"-> dBgActor_c::mMeshCollider
 *   0x2ec..0x320               -> dBgActor_c::mClsnMat + tail
 *
 * The class is polymorphic (the ROM emits D0/D1 for it) and its vtable is
 * _ZTV15daObjWc_Obj04_c at ov029 0x021141a0, whose RTTI at 0x0211415c carries
 * the same name -- which is why the class is spelled that way here. This
 * header used to record a second, coined spelling for that one address; that
 * alias is gone from symbols.txt, so there is nothing left to reconcile. The
 * cartridge's D1
 * stores that vtable, destroys its own Model at +0x320 first, then the inlined
 * dBgActor_c base step destroys mMeshCollider and mModel and runs dActor_c::D2
 * -- which is what an empty ~daObjWc_Obj04_c() with this inheritance
 * compiles to. */
#ifndef DAOBJWC_OBJ04_C_H
#define DAOBJWC_OBJ04_C_H
#include "types.h"
/* dBgActor_c.h must stand ahead of Model.h (Matrix4x3 guarded-spelling order,
 * see its header comment); it brings dActor_c.h, Model.h and dBgW_KcMbg.h. */
#include "dBgActor_c.h"

struct daObjWc_Obj04_c : dBgActor_c {
    /* Model member, named by _ZN5ModelD1Ev at +0x320 -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. First
       derived member: dBgActor_c ends at exactly 0x320. */
    Model mModel2;            /* 0x320 */
    u8  pad_370[0x30];
    s16 mStateTimer;            /* 0x3a0 */
    u8  mState;            /* 0x3a2 */
    u8  mVisible;            /* 0x3a3 */
    u8  mEventID;            /* 0x3a4 */

    /* methods */
    int CleanupResources();
    int InitResources();
    int Render();
    int Behavior();

    /* MEASURED. Declared here but never DEFINED as a C++ member: the
       cartridge's D1 and D0 are carried in src/actors/d_a_obj_wc_obj04.cpp as
       `// @symbol` marked bodies under their mangled names, because a real
       member definition emits the D2/D1/D0 triple in the order D0-then-D1
       while the ROM keeps D1 below D0. Declaring it virtual is still what
       fixes the vtable shape; see notes/dtor-migration.md and that file's
       header for the emission-order refusal. */
    virtual ~daObjWc_Obj04_c();
};

typedef char daObjWc_Obj04_c_size_must_be_0x3a8[
    sizeof(struct daObjWc_Obj04_c) == 0x3a8 ? 1 : -1];

#endif
