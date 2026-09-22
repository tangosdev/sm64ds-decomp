/* HOST COPY of src/_ZN6Player16CleanupResourcesEv.cpp -- the Player's own
 * teardown, with the five declarations that collide with include/decl_common.h
 * taken out.
 *
 * WHY IT IS HERE AND NOT IN A SLICE. The matched TU opens an `extern "C"`
 * block that re-declares five functions decl_common.h has already declared:
 *
 *     TU              void func_ov002_020bdd2c(char *c);
 *     decl_common.h   extern void func_ov002_020bdd2c(void*);
 *
 * On mwcc that is one declaration seen twice with a compatible pointer type.
 * MSVC calls it an overload of an extern "C" function and refuses the TU
 * (C2733 x5, plus one C2664 where func_02073244's fourth parameter is
 * `void (*)(void*)` in the header and `void (*)(void)` in the TU). Neither
 * spelling is wrong about the ROM; they simply disagree, and src/ and
 * include/ are the byte-matched tree that the port does not edit.
 *
 * NOTHING ELSE CHANGES. The body below is the matched source's, statement for
 * statement: the four sub-object cleanups, the four-slot model/collider walk,
 * the ninety-odd SharedFilePtr::Release calls in their original order, and
 * the mLoadedResourceFlags tail. The only edits are the removed declarations
 * and the two casts that reconcile the header's prototypes.
 *
 *
 * SECOND EDIT, and it is the one that matters: the five `p->v1()` calls.
 * The matched TU deletes the body models, the head models and two more
 * model-family objects through a two-virtual SHADOW CLASS, so v1 is ROM slot
 * 1 -- the deleting destructor mwcc puts there. MSVC folds D1 and D0 into one
 * slot, so hal/cxxname_bridge.cpp's _ZTV5Model has DoSetFile at 1, and every
 * one of those five calls landed on Model::DoSetFile and faulted inside
 * Model::AddToCommonModelDataArr on the first level teardown. They are spelled
 * as port_model_family_delete now, which picks the destructor by the object's
 * own vtable pointer (same file) and then frees it, which is what the ROM's D0
 * is. Nothing dispatches through the ambiguous slot.
 *
 * WHAT REACHES IT (gate 31): a level change destroys every actor the level
 * spawned, and the Player is one of them. This is what gives his files back --
 * without it every transition would hold another copy of the character's
 * models and animations loaded.
 */
#include "types.h"
// @symbol _ZN6Player16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
#include "SharedFilePtr.h"
enum { false_, true_ };

extern "C" void port_model_family_delete(void *obj);

#pragma opt_common_subs off
#pragma opt_strength_reduction off

extern "C" {
void func_0203cbc0(int p);
void _ZN7Vector3D1Ev(void);
void func_ov002_020bebd4(char *c);
void UnloadSilverStarAndNumber(void);
void UnloadKeyModels(int n);
extern void *data_ov002_020ff480[];
extern u8 data_0209f2d8;
extern char data_ov002_0210d9a0[];
extern char data_ov002_0210d9a8[];
extern char data_ov002_0210d9c0[];
extern char data_ov002_0210da38[];
extern char data_ov002_0210da40[];
extern char data_ov002_0210e3d8[];
extern char data_ov002_0210e750[];
extern char data_ov002_0210e8d0[];
extern char data_ov002_0210eb20[];
extern char data_ov002_0210ebb8[];
extern char data_ov002_0210ebd8[];
extern char data_ov002_02110aa4[];
}

/* _ZN6Player16CleanupResourcesEv RETIRED (run link100, lane SEAT6, batch B6).
   Of its two stated blockers the first is DEAD: the `p->v1()` model
   deletes are ROM slot 1, and hal/model_dtor_seat.cpp seats
   _ZTV5Model[1], _ZTV9ModelAnim[1] and _ZTV10ModelAnim2[1] with the
   ROM's own deleting D0 since lane SLOT5F stopped MSVC folding the
   pair -- the body port_model_family_delete was hand-built to reach.
   The second is the five C2733s and the C2664 against decl_common.h.
   The matched TU src/_ZN6Player16CleanupResourcesEv.cpp is seated in its place: port/tools/hostgen.py's HEADER_SHADOW table hides the header's
   declaration of those five names for this TU alone. Neither src/ nor
   include/ moves.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
