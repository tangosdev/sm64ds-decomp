/* class SwitchActivatedPlank -- the switch-triggered wooden plank of ov029.
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
 * _ZTV15daObjWc_Obj04_c == _ZTV20SwitchActivatedPlank: two names, one address
 * (ov029 0x021141a0; RTTI at 0x0211415c carries the Obj04 name). Same
 * EAD-name/English-name alias as HootTheOwl/_ZTV7daOwl_c. The cartridge's D1
 * stores that vtable, destroys its own Model at +0x320 first, then the inlined
 * dBgActor_c base step destroys mMeshCollider and mModel and runs dActor_c::D2
 * -- which is what an empty ~SwitchActivatedPlank() with this inheritance
 * compiles to. */
#ifndef SWITCHACTIVATEDPLANK_H
#define SWITCHACTIVATEDPLANK_H
#include "types.h"
/* dBgActor_c.h must stand ahead of Model.h (Matrix4x3 guarded-spelling order,
 * see its header comment); it brings dActor_c.h, Model.h and dBgW_KcMbg.h. */
#include "dBgActor_c.h"

struct SwitchActivatedPlank : dBgActor_c {
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

    /* MEASURED -- defined out-of-line in src/_ZN20SwitchActivatedPlankD1Ev.cpp.
       Declaring it virtual is what makes mwccarm emit the ROM's D1/D0 pair;
       see notes/dtor-migration.md. */
    virtual ~SwitchActivatedPlank();
};

typedef char SwitchActivatedPlank_size_must_be_0x3a8[
    sizeof(struct SwitchActivatedPlank) == 0x3a8 ? 1 : -1];

#endif
