// Seat the model family's own deleting dtors (D0) into slot 0 of the five
// hosted primary tables, the meshcollider_dtor_seat treatment one family over:
// naming the matched bodies here is the reference edge that pulls the five
// TUs into the link, and the slots stop holding a no-op (ModelAnim/ModelAnim2/
// BlendModelAnim) or nothing at all (Model/Animation, whose slot 0 no fill
// ever wrote).
//
//   _ZTV5Model[0]           <- _ZN5ModelD0Ev            (arm9 0x02016ce0)
//   _ZTV9Animation[0]       <- _ZN9AnimationD0Ev        (arm9 0x02015cc4)
//   _ZTV9ModelAnim[0]       <- _ZN9ModelAnimD0Ev
//   _ZTV10ModelAnim2[0]     <- _ZN10ModelAnim2D0Ev
//   _ZTV14BlendModelAnim[0] <- _ZN14BlendModelAnimD0Ev  (arm9 0x02016644)
//
// All five are matched .c with `this` on the stack, so each slot takes the
// ecx->arg adapter (the hal/mmc_vtable.cpp mechanism) rather than the body
// directly. Their whole callee closure is already linked: the D2/D1 chain
// (ModelBase D2, Model D2, Animation D1/D2, ModelAnim D2) came in with the
// class slices, and func_0203cbc0 / Memory::operator_delete2 with the
// gate-16 MeshCollider dtor pair.
//
// ORDER. The no-op fills run first: hal_fill_modelanim2_vtable at process
// start (walk_window.cpp / smoke_player.cpp), hal_fill_blendmodelanim_vtable
// from King Bob-omb's registry fill inside port_actor_registry_install. This
// seat is called at the TAIL of port_stage_a2_seat, after the registry
// install, so the real bodies win every level boot.
//
// The SECONDARY Animation-subobject thunk tables (VTable_Animation_*Thunk)
// keep their no-ops: their slot 0/1 belong to this-adjusting thunk bodies the
// ROM emits per class, not to these primary-this D0s, and seating a primary
// body behind an unadjusted secondary `this` would be the wrong-body seat the
// vtspan --seats check exists to catch.
//
// The TUs ride slice_gate16 (walk_window family); the gate-8/9 smoke targets
// do not link this file and keep their current slots, which they never
// dispatch (they never delete a model through a base pointer).
//
// ===========================================================================
// LANE l3 SEAT (run linkw, wave 1) -- five more slots the port left empty.
//
// WHY A SEAT AND NOT JUST A SLICE LINE. Release links with /OPT:REF, so a
// matched TU added to a slice and referenced by nothing is discarded before
// it reaches the map: adding the thirteen model-family TUs to slice_w1l3.txt
// on their own moved port/tools/linkage.py's headline by exactly zero. The
// reference edge IS the work, and a ROM vtable slot the port never filled is
// the honest place to put one.
//
// THE SLOT NUMBERS ARE MSVC'S, NOT THE ROM'S, and that is the whole hazard in
// this family. Every ROM vtable here is Itanium-shaped and carries the dtor
// TWICE (slot 0 the complete D1, slot 1 the deleting D0); MSVC folds both into
// a single slot 0, so everything below the dtor shifts down one. The port's
// arrays are the MSVC shape -- hal/bob_enemy_bridges.cpp's ModelBase::SetFile
// dispatches DoSetFile as vt[1], the ROM's slot 2 -- and the fills already in
// the tree (hal/cxxname_bridge.cpp, hal/blend_vtable.cpp) agree. So a seat here
// names the ROM relocation that proves WHICH BODY belongs to the slot, and
// then writes it at the MSVC index. No existing fill is renumbered or moved.
//
//   _ZTV11ShadowModel[1]       <- ShadowModel::DoSetFile
//        ROM from:0x0208e870 -> 0x02015ef4 (_ZTV11ShadowModel+8, ROM slot 2)
//        Slot 0 already holds hal/cxxname_bridge.cpp's D1 thunk and stays put.
//        This slot is the one ModelBase::SetFile reaches, and it has been null
//        since gate 9 -- ShadowModel::SetFile currently aborts with "vtable has
//        no DoSetFile", which is why hal/bob_enemy_bridges.cpp stubs
//        ShadowModel::InitCylinder to `return 1` rather than let it dispatch.
//   _ZTV15TextureSequence[0]   <- _ZN15TextureSequenceD0Ev
//        ROM from:0x0208e7d8 -> 0x02015a00 (_ZTV15TextureSequence+4, slot 1)
//   _ZTV15MaterialChanger[0]   <- _ZN15MaterialChangerD0Ev
//        ROM from:0x0208e7f8 -> 0x02015800 (_ZTV15MaterialChanger+4, slot 1)
//   _ZTV18TextureTransformer[0]<- _ZN18TextureTransformerD0Ev
//        ROM from:0x0208e7c8 -> 0x02015900 (_ZTV18TextureTransformer+4, slot 1)
//   data_0208e87c[0]           <- _ZN9ModelBaseD0Ev
//        data_0208e87c IS _ZTV9ModelBase's function-slot start (dsd named the
//        address, not the class): ROM from:0x0208e87c -> 0x02017120 is
//        ModelBase's D1 and from:0x0208e880 -> 0x020170e8 its D0. Storage is
//        hal/model_host.cpp, which already calls it the ModelBase vtable.
//
// The three Animation-derived tables and the ModelBase table were ALL ZEROS
// before this seat -- no fill in the tree ever wrote a slot in them, so there
// is no behaviour to regress and nothing dispatches them today. Seating the
// DELETING body in the folded slot is the convention the five seats above set,
// and it is the right one for MSVC: `delete p` through a virtual destructor
// calls slot 0 and expects it to free. The one class where that would be wrong
// is ShadowModel, whose slot 0 the port deliberately filled with the
// NON-deleting D1 because PowerStar destroys an embedded shadow through it;
// that fill is left exactly as it is.
//
// LEFT UNSEATED, on purpose (reported as blocked, not forgotten):
//   _ZN11ShadowModelD0Ev, _ZN11CommonModelD0Ev -- their folded slot 0 already
//     holds the D1 their embedders need, and the ROM's second dtor slot has no
//     MSVC index to live at. A deleting body cannot be added without evicting
//     a destructor that is dispatched.
//   _ZN9ModelBaseD1Ev -- same folding, one slot up: data_0208e87c[0] can hold
//     one of the two ModelBase dtors and the deleting one is the useful half.
//   _ZThn80_N{9ModelAnim,14BlendModelAnim,10ModelAnim2}D{0,1}Ev -- the six
//     Animation-subobject thunks. Their matched TUs are mwcc ARTEFACT sources:
//     a synthetic two-base hierarchy whose only job is to make the compiler
//     emit a this-adjusting thunk, and the D0 and D1 file of each pair are
//     byte-identical. Under MSVC each pair therefore defines the SAME symbol
//     (??1ModelAnim@@UAE@XZ and its two siblings) twice, and that symbol is
//     also the one a real ModelAnim destructor would take. They cannot be
//     linked. The VTable_Animation_*Thunk tables keep their no-ops.
// ===========================================================================

#include "ShadowModel.h"

extern "C" {

extern void *_ZTV5Model[];            /* storage in hal/model_host.cpp */
extern void *_ZTV9Animation[];        /* storage in hal/model_host.cpp */
extern void *_ZTV9ModelAnim[];        /* storage in hal/model_host.cpp */
extern void *_ZTV10ModelAnim2[];      /* storage in hal/actor_vtables.cpp */
extern void *_ZTV14BlendModelAnim[];  /* storage in hal/blend_vtable.cpp */
extern void *_ZTV11ShadowModel[];        /* storage in hal/actor_vtables.cpp */
extern void *_ZTV15TextureSequence[];    /* storage in hal/actor_vtables.cpp */
extern void *_ZTV15MaterialChanger[];    /* storage in hal/actor_vtables.cpp */
extern void *_ZTV18TextureTransformer[]; /* storage in hal/method_faces.cpp */
extern int data_0208e87c[];              /* _ZTV9ModelBase, hal/model_host.cpp */

void *_ZN5ModelD0Ev(void *self);
void *_ZN9AnimationD0Ev(void *self);
void *_ZN9ModelAnimD0Ev(void *self);
void *_ZN10ModelAnim2D0Ev(void *self);
void *_ZN14BlendModelAnimD0Ev(void *self);
void *_ZN15TextureSequenceD0Ev(void *self);
void *_ZN15MaterialChangerD0Ev(void *self);
void *_ZN18TextureTransformerD0Ev(void *self);
void *_ZN9ModelBaseD0Ev(void *self);

}

static void __fastcall model_d0(void *s, void *)     { _ZN5ModelD0Ev(s); }
static void __fastcall anim_d0(void *s, void *)      { _ZN9AnimationD0Ev(s); }
static void __fastcall modelanim_d0(void *s, void *) { _ZN9ModelAnimD0Ev(s); }
static void __fastcall modelanim2_d0(void *s, void *){ _ZN10ModelAnim2D0Ev(s); }
static void __fastcall blend_d0(void *s, void *)     { _ZN14BlendModelAnimD0Ev(s); }
static void __fastcall texseq_d0(void *s, void *)    { _ZN15TextureSequenceD0Ev(s); }
static void __fastcall matchg_d0(void *s, void *)    { _ZN15MaterialChangerD0Ev(s); }
static void __fastcall texxfm_d0(void *s, void *)    { _ZN18TextureTransformerD0Ev(s); }
static void __fastcall modelbase_d0(void *s, void *) { _ZN9ModelBaseD0Ev(s); }

/* ShadowModel::DoSetFile is a matched METHOD, so the adapter dispatches it
   qualified, in the (self, edx, args...) shape hal/cxxname_bridge.cpp's
   mv_dosetfile uses for the Model slot next door. */
static int __fastcall shadow_dosetfile(void *self, void *, char *f, int a, int b)
{
    return ((ShadowModel *)self)->ShadowModel::DoSetFile(f, a, b);
}

extern "C" void hal_seat_model_family_dtors(void)
{
    _ZTV5Model[0]           = (void *)model_d0;
    _ZTV9Animation[0]       = (void *)anim_d0;
    _ZTV9ModelAnim[0]       = (void *)modelanim_d0;
    _ZTV10ModelAnim2[0]     = (void *)modelanim2_d0;
    _ZTV14BlendModelAnim[0] = (void *)blend_d0;

    /* lane l3, wave 1: see the SEAT block at the top of this file. */
    _ZTV11ShadowModel[1]        = (void *)shadow_dosetfile;
    _ZTV15TextureSequence[0]    = (void *)texseq_d0;
    _ZTV15MaterialChanger[0]    = (void *)matchg_d0;
    _ZTV18TextureTransformer[0] = (void *)texxfm_d0;
    data_0208e87c[0]            = (int)(size_t)modelbase_d0;

    /* run link100, lane STAGEFIX: _ZTV18TextureTransformer's ROM slot 1 (the
       Itanium D0, folded away by MSVC the same way slot 0 above already
       explains) duplicated rather than left at its zero default.
       src/_ZN5Stage16CleanupResourcesEv.cpp's area-table loop walks every
       area entry's TextureTransformer* (Stage+0x8bc, stride 0xc) with the
       SAME faithful two-word vtable read Stage's own skybox hits --
       hal/stage_bridges.cpp's st_skybox_vtable_fixup has the full account of
       why that read targets the ROM's slot 1 -- and this table's slot 1 is
       otherwise unused: nothing else in the tree reads or writes
       _ZTV18TextureTransformer[1..3], unlike _ZTV5Model[1], which
       hal/method_faces.cpp:199 already ruled load-bearing (Model::DoSetFile)
       and therefore off limits. A castle-grounds boot allocates exactly one
       TextureTransformer ("[stage] texture transformers: 1 of 1 areas
       animate"), and with SM64DS_STAGE_SLOT3_ROM open its area-table entry is
       destroyed the same frame the skybox is -- so this slot needed the same
       fix the skybox's own vtable got, just without the "shared table"
       complication that forced a private copy there. */
    _ZTV18TextureTransformer[1] = (void *)texxfm_d0;
}
