/* RETIRED BY GATE SLOT5, all but one body of it.
 *
 * This file used to hold twenty-three hand copies of matched Render bodies --
 * Whomp, Butterfly, Fish, QuestionBlock, Scuttlebug, PowerStar, Bully,
 * BigBully, RotatingFirebar, UpDownLiftBbh, Seaweed, SeesawBob,
 * UnchainedChomp, BabyPenguin, HootTheOwl, SwitchPillar (the BasementWater
 * one), Spindrift, Moneybag, PushBlock, Boo, FlyGuy, YoshiEgg and
 * QuestionSwitch -- because the host could not dispatch the slot they mean.
 *
 * THE CAUSE, and it was never these classes. mwccarm spends TWO vtable
 * entries on `virtual ~X()`, the Itanium D1 complete-object and D0 deleting
 * pair, and MSVC folds them into ONE. Every virtual declared after the
 * destructor therefore sat one slot early on the host, all the way down
 * ModelBase -> Model -> ModelAnim. The ROM's _ZTV9ModelAnim spends slot 5 on
 * Render(Vector3 const *) and slot 6 on Virtual18(u32, Vector3 const *); the
 * folded host table put Virtual18 on slot 5, and Virtual18 reads one argument
 * more than a slot-5 caller passes, so it took its scale off the stack and
 * handed Model::Virtual10 whatever was there. That is the c0000005 in
 * Model::Virtual10 this file's old header measured on Butterfly.
 *
 * THE FIX. include/ModelBase.h and its six siblings now spell the destructor
 * pair as two plain virtuals under _MSC_VER, so the host numbers the table the
 * ROM's way and slot 5 is Render on both sides. Nothing in this tree defines
 * _MSC_VER for mwccarm, so no ROM byte moves. port/slice_slot5.txt puts the
 * twenty-three matched sources back in the link, and the thirty faces that let
 * their fill sites reach a __thiscall method are in
 * hal/actor_classes_bob_world.cpp.
 *
 * ONE BODY STAYS, and it is not a slot problem. src/func_ov012_02111324.cpp is
 * SwitchPillar's own Render (daObjC0_Switch_c, see port/ov012_syms.txt's
 * class-identity note) and it walks into the same collision, but its source
 * carries the "recovered from vtable slot identity" marker -- a body recovered
 * from the shape of the table rather than from the ROM's own bytes.
 * port/tools/inferred_stub_guard.py refuses new seats of those, so this copy
 * is what its slot dispatches until the body is adjudicated.
 */
#include "Model.h"

extern "C" {

/* ---- SWITCH_PILLAR (34, ov012, gate 199) ---------------------------------
   src/func_ov012_02111324.cpp dispatches through a LOCAL six-virtual ROM-order
   shadow (`struct Base{...virtual void m(int);}; struct Derived{char pad[0xd4];
   Base base;}; b->m(0);`) over a plain Model at +0xd4 -- the RotatingFirebar
   bare-call shape exactly (no scale argument, a Platform's own plain Model,
   not a ModelAnim). Excluded from slice_gate199.txt.
   PORT_HOST_ABI: ROM-order model slot-5 dispatch, the Whomp/Fish case. */
int func_ov012_02111324(void *selfv)
{ ((Model *)((char *)selfv + 0xd4))->Model::Render(0); return 1; }

}  /* extern "C" */
