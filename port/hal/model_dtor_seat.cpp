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

extern "C" {

extern void *_ZTV5Model[];            /* storage in hal/model_host.cpp */
extern void *_ZTV9Animation[];        /* storage in hal/model_host.cpp */
extern void *_ZTV9ModelAnim[];        /* storage in hal/model_host.cpp */
extern void *_ZTV10ModelAnim2[];      /* storage in hal/actor_vtables.cpp */
extern void *_ZTV14BlendModelAnim[];  /* storage in hal/blend_vtable.cpp */

void *_ZN5ModelD0Ev(void *self);
void *_ZN9AnimationD0Ev(void *self);
void *_ZN9ModelAnimD0Ev(void *self);
void *_ZN10ModelAnim2D0Ev(void *self);
void *_ZN14BlendModelAnimD0Ev(void *self);

}

static void __fastcall model_d0(void *s, void *)     { _ZN5ModelD0Ev(s); }
static void __fastcall anim_d0(void *s, void *)      { _ZN9AnimationD0Ev(s); }
static void __fastcall modelanim_d0(void *s, void *) { _ZN9ModelAnimD0Ev(s); }
static void __fastcall modelanim2_d0(void *s, void *){ _ZN10ModelAnim2D0Ev(s); }
static void __fastcall blend_d0(void *s, void *)     { _ZN14BlendModelAnimD0Ev(s); }

extern "C" void hal_seat_model_family_dtors(void)
{
    _ZTV5Model[0]           = (void *)model_d0;
    _ZTV9Animation[0]       = (void *)anim_d0;
    _ZTV9ModelAnim[0]       = (void *)modelanim_d0;
    _ZTV10ModelAnim2[0]     = (void *)modelanim2_d0;
    _ZTV14BlendModelAnim[0] = (void *)blend_d0;
}
