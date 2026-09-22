// port/hal/smoke_dtor_slots_actor.cpp -- ModelBase's, Model's and
// ShadowModel's two vtable-slot virtuals, plus dActor_c's struct-return face,
// for smoke_actor, smoke_savestate and smoke_persist, which host those three
// model classes and dActor_c and nothing else in the two int4_rows.cpp
// derivations this file carries a subset of.
//
// Lane SMOKELINK5, run link100 wave 10 round 5, applying out/SMOKELINK4B/
// remaining.md section A verbatim.
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
// smoke_actor is the ShadowModel gate (an actor's shadow) and
// src/_ZN11ShadowModelC1Ev's vftable asks for that class's two slots as well.
// It is a separate file and not an extension of the two-class one because the
// six targets that use that one host no ShadowModel and would gain two
// unresolved flat names.
//
// WHY THIS FILE RATHER THAN hal/int4_rows.cpp ITSELF.  That file covers SIX
// classes and also carries five /alternatename rows and a flat
// ExpandingHeapAllocator face; taking it whole on this batch traded seven
// names for seventeen (out/SMOKELINK4B/remaining.md section A), the other
// three classes' flat destructors (CommonModel, ModelAnim2, BlendModelAnim),
// dBgCh_Lin::GetClsnPos, and four unrelated faces at the foot of that file,
// none of which an actor harness hosts.
//
// THE SIX DESTRUCTOR BODIES BELOW ARE COPIED CHARACTER FOR CHARACTER from
// hal/int4_rows.cpp lines 71-74 and 81-82, together with the six extern "C"
// declarations they call.  Nothing is re-derived: each forwards to the ROM's
// own function for that slot, with the receiver the host ABI put in ecx.  The
// flat names come from port/unmatched/ModelBaseModelShadow_Dtors_HostCopy.c
// (this batch's own three-class host copy), which is where walk_window gets
// the same six bodies from port/unmatched/ModelFamily_Dtors_HostCopy.c.  No
// target carries both this file and hal/int4_rows.cpp, so the two definitions
// cannot collide.
//
//   slot 0, Destructor1              slot 1, Destructor0
//   _ZN9ModelBaseD1Ev      0x02017120   _ZN9ModelBaseD0Ev      0x020170e8
//   _ZN5ModelD1Ev          0x02016d20   _ZN5ModelD0Ev          0x02016ce0
//   _ZN11ShadowModelD1Ev   0x02015ff8   _ZN11ShadowModelD0Ev   0x02015f80
//
// THE SEVENTH ROW, dActor_c::OnAimedAtWithEggReturnVec, IS COPIED CHARACTER
// FOR CHARACTER from hal/int4_rows.cpp lines 198 and 202-206 (the
// dActor_c half of "THE TWO STRUCT-RETURN ROWS"; dBgCh_Lin::GetClsnPos, the
// other half, is not on this batch).  facegen refused it because it will not
// emit an indirect return: the host compiler builds its own hidden return
// slot and the ROM body takes that slot as an explicit first argument.  Return
// slot first, receiver second, which is AAPCS indirect return with `this`
// displaced into r1 -- src/_ZN8dActor_c25OnAimedAtWithEggReturnVecEv.cpp's own
// header paragraph spells this out from the ROM.  dActor_c::
// OnAimedAtWithEggReturnVec is slot 30 and every actor's vftable references
// it, so it is the single most referenced row on this batch's wall even
// though it is only one row.

#include "ModelBase.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dActor_c.h"

extern "C" {
void _ZN9ModelBaseD1Ev(void *self);
void _ZN9ModelBaseD0Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN5ModelD0Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN11ShadowModelD0Ev(void *self);
void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(Vector3 *ret, void *self);
}

void ModelBase::Destructor1()      { _ZN9ModelBaseD1Ev(this); }
void ModelBase::Destructor0()      { _ZN9ModelBaseD0Ev(this); }
void Model::Destructor1()          { _ZN5ModelD1Ev(this); }
void Model::Destructor0()          { _ZN5ModelD0Ev(this); }
void ShadowModel::Destructor1()    { _ZN11ShadowModelD1Ev(this); }
void ShadowModel::Destructor0()    { _ZN11ShadowModelD0Ev(this); }

Vector3 dActor_c::OnAimedAtWithEggReturnVec()
{
    Vector3 out;
    _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(&out, this);
    return out;
}

// data_0208e87c, the port's old name for ModelBase's vtable at arm9
// 0x0208e87c.  port/unmatched/ModelBaseModelShadow_Dtors_HostCopy.c's
// ModelBase bodies store it into word 0 under that spelling, the way the
// parent file they are copied from does, and config names the same table
// _ZTV9ModelBase.  Plain data on both sides: one table, two names.  Copied
// verbatim from hal/smoke_aliases_model.cpp:113 (itself hal/cxx_aliases.cpp:3684),
// which this batch does not otherwise carry.
#pragma comment(linker, "/alternatename:_data_0208e87c=__ZTV9ModelBase")
