// port/hal/smoke_dtor_slots_modelanim.cpp -- ModelBase's, Model's and
// BlendModelAnim's two vtable-slot virtuals, for smoke_modelanim, which hosts
// those three classes and no other member of the model family.
//
// Lane SMOKELINK3, run link100 wave 10 round 2.
//
// WHAT THESE NAMES ARE.  port/hal/int4_rows.cpp's header says it in full and
// include/ModelBase.h:110 is the ruling: mwccarm puts a class's D1 and D0 in
// vtable slots 0 and 1 and MSVC folds them into one entry, so every slot below
// the destructor would sit a word early.  The decomp headers therefore spell
// TWO ORDINARY VIRTUALS in those two places under the host-invented names
// Destructor1 and Destructor0, guarded by _MSC_VER so the ARM side never sees
// them and no ROM byte moves.  Neither name is ever called; they hold the two
// slots the ROM's table holds.  Holding a slot still needs a definition,
// because a constructor emits its class's MSVC vftable and a vftable
// references every slot -- which is why src/_ZN5ModelC1Ev.cpp asks the linker
// for ?Destructor1@Model@@UAEXXZ.
//
// THREE CLASSES RATHER THAN port/hal/smoke_dtor_slots_model.cpp's TWO.
// smoke_modelanim is the BlendModelAnim gate and src/_ZN14BlendModelAnimC1Ev's
// vftable asks for that class's two slots as well.  It is a separate file and
// not an extension of the two-class one because the six targets that use that
// one host no BlendModelAnim and would gain two unresolved flat names.
//
// WHY THIS FILE RATHER THAN hal/int4_rows.cpp ITSELF.  That file is where
// these four bodies live for the window targets, and taking it whole was tried
// first: it covers SIX classes (ModelBase, Model, CommonModel, ModelAnim2,
// BlendModelAnim, ShadowModel) and also carries five /alternatename rows and a
// flat ExpandingHeapAllocator face, so on smoke_model it traded four
// unresolved names for eighteen -- the other four classes' flat destructors
// plus dEnemyBase_c::KillByInvincibleChar, Particle::SysTracker::Contents::
// Create, dScene_c::ResetHardwareRegisters, dScMgCurling2_c::SpawnValue,
// dActor_c::OnAimedAtWithEggReturnVec and dBgCh_Lin::GetClsnPos, none of which
// a model harness hosts.  This is the same subset argument
// port/faces_sync_smoke_heap.txt makes for the ledger and
// port/hal/smoke_aliases_heap.cpp makes for the alias rows: take the rows the
// target needs, verbatim, and leave the rest of the window build's scaffolding
// where it is.
//
// THE FOUR BODIES BELOW ARE COPIED CHARACTER FOR CHARACTER from
// hal/int4_rows.cpp lines 58-59, 71-74, together with the two extern "C"
// declarations they call.  Nothing is re-derived: each forwards to the ROM's
// own function for that slot, with the receiver the host ABI put in ecx.  The
// flat names come from port/unmatched/ModelFamily_Dtors_HostCopy.c, which is
// where walk_window gets them too (walk_window.map lines 15562-15570).  No
// target carries both this file and hal/int4_rows.cpp, so the two definitions
// cannot collide.
//
//   slot 0, Destructor1              slot 1, Destructor0
//   _ZN9ModelBaseD1Ev      0x02017120   _ZN9ModelBaseD0Ev      0x020170e8
//   _ZN5ModelD1Ev          0x02016d20   _ZN5ModelD0Ev          0x02016ce0
//   _ZN14BlendModelAnimD1Ev 0x02016690  _ZN14BlendModelAnimD0Ev 0x02016644

#include "ModelBase.h"
#include "Model.h"
#include "BlendModelAnim.h"

extern "C" {
void _ZN9ModelBaseD1Ev(void *self);
void _ZN9ModelBaseD0Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN5ModelD0Ev(void *self);
void _ZN14BlendModelAnimD1Ev(void *self);
void _ZN14BlendModelAnimD0Ev(void *self);
}

void ModelBase::Destructor1()      { _ZN9ModelBaseD1Ev(this); }
void ModelBase::Destructor0()      { _ZN9ModelBaseD0Ev(this); }
void Model::Destructor1()          { _ZN5ModelD1Ev(this); }
void Model::Destructor0()          { _ZN5ModelD0Ev(this); }
void BlendModelAnim::Destructor1() { _ZN14BlendModelAnimD1Ev(this); }
void BlendModelAnim::Destructor0() { _ZN14BlendModelAnimD0Ev(this); }
