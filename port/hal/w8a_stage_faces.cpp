// THE STAGE CLASS'S C-NAME FACES, and the LINK ROOTS that reach its bodies.
// (run linkw wave 8, lane w8-a.)
//
// ===========================================================================
// PART 1 -- THE LINK ROOTS. READ THIS BEFORE READING THE DIRECTIVES.
// ===========================================================================
//
// This is a /OPT:REF link. A translation unit whose symbols nothing names is
// compiled and then DISCARDED, so listing a matched TU in a slice does not put
// it in the binary. Measured on this very tree, not assumed: the wave-8 base
// build compiles 4975 objects into walk_window and 61 of them are absent from
// walk_window.map -- src/_ZN6Player8CanPauseEv.cpp and
// src/_ZN8Particle10SysTrackerD1Ev.cpp among them -- purely because no
// surviving reference named them.
//
// On the ROM every Stage body below is reached the same way: through
// _ZTV5Stage, the eighteen-entry vtable Stage::Stage installs last. The port
// does not have that seat yet. hal/stage_bridges.cpp TRAP-fills all twenty
// slots at boot (`hal_fill_stage_vtable`), nothing dispatches through the
// Stage -- it is in the scene tree but on neither processing list -- and
// seating the real words in that table is a lane of its own that owns
// stage_bridges.cpp. This lane does not.
//
// So the reach here is EXPLICIT AND LINK-ONLY: one /include: per body. Each
// directive is an artificial root that keeps the section alive; it is NOT a
// call site and it adds NO behaviour, which is the whole point -- this piece
// is meant to move linkage and leave every gate byte-identical. Say it plainly
// rather than let a reader infer from a rising count that the ROM's Stage is
// now running: it is not. What is true after this piece is that the bodies and
// the storage they need are IN the binary under the ROM's own names, so the
// vtable seat that comes next is a table fill and not a link excavation.
//
// A .drectve directive is processed for every object on the linker's command
// line before any elimination runs, so these roots cannot themselves be
// stripped -- which is exactly why the reach has to be stated as a directive
// and not as a dead function nothing calls.
//
// The two names that are NOT rooted here, because they do not need to be:
// src/_ZN8Particle10SysTrackerD1Ev.cpp and src/func_02021b98.cpp are already
// compiled into walk_window by earlier slices and were being discarded; the
// Stage destructors below name them, so they come back on their own. They are
// the two TUs of this piece that are reached by real ROM code rather than by a
// directive.

/* Stage::GraphCallback1 -- the Stage's SceneRelated block slot 1, whose body
   is Particle::RenderAll. */
#pragma comment(linker, "/include:__ZN5Stage14GraphCallback1Ev")
/* Stage::IsPauseDisabled -- Stage::Behavior's pause gate. Pulls
   src/func_ov002_020bd8ac.c, its only callee. */
#pragma comment(linker, "/include:__ZN5Stage15IsPauseDisabledEv")
/* Stage::CanPause -- the sibling gate. */
#pragma comment(linker, "/include:__ZN5Stage8CanPauseEv")
/* Stage::BeforeInitResources -- _ZTV5Stage slot 1. */
#pragma comment(linker, "/include:__ZN5Stage19BeforeInitResourcesEv")
/* Stage::UpdateMenuButtons -- the pause/VS/level-clear button rows. Reads
   data_0209f244, data_0209f2b4, data_0209f2e0 and data_0209f360[0..3], all
   four hosted by hal/w8a_stage_storage.cpp in this same piece. */
#pragma comment(linker, "/include:__ZN5Stage17UpdateMenuButtonsEb")
/* Stage::OnPendingDestroy -- _ZTV5Stage slot 12. This one is a real C++
   method in its own TU, so it is rooted by its MSVC decoration, not by the
   Itanium name; the string is the one its object file publishes. */
#pragma comment(linker, "/include:?OnPendingDestroy@Stage@@QAEXXZ")
/* ~Stage, both ROM destructors -- _ZTV5Stage slots 16 (D1/D2) and 17 (D0).
   Between them they name _ZTV5Stage, _ZTV5Scene, MeshCollider::~MeshCollider,
   Model::~Model, Particle::SysTracker::~SysTracker, ActorBase::~ActorBase and
   Memory::Deallocate -- every one already in the link except the SysTracker
   destructor, which these two revive. */
#pragma comment(linker, "/include:__ZN5StageD2Ev")
#pragma comment(linker, "/include:__ZN5StageD0Ev")

// ===========================================================================
// PART 2 -- THE C-NAME FACES.
// ===========================================================================
//
// The ROM's callers spell these methods with the Itanium C name; the matched
// TU that defines each one either IS that C name (a .c file, or a .cpp that
// wraps the definition in extern "C") or is the MSVC method. Where the two
// spellings are the same function with the same calling convention, a linker
// alias is the whole face and no forwarder is needed.
//
// Each LHS below is transcribed verbatim from the probe link's own error text
// -- the throwaway build that wired all 29 unwired src/_ZN5Stage* TUs into
// walk_window and printed LNK1120: 171 unresolved externals. A hand-built
// mangling that is one letter wrong is a directive that never fires and never
// says so.
//
// PER-FACE ABI CHECK, the hal/method_faces.cpp checklist applied to each:
//
//   ?CanPause@Stage@@SAHXZ = __ZN5Stage8CanPauseEv
//   ?IsPauseDisabled@Stage@@SAHXZ = __ZN5Stage15IsPauseDisabledEv
//   ?RenderBouncingArrows@Stage@@SAXXZ = __ZN5Stage20RenderBouncingArrowsEv
//       STATIC members (SA = static, cdecl) aliased onto cdecl C definitions
//       of the same arity (zero) and the same return width. Exact: same
//       convention, same stack discipline, no receiver in play.
//
//   ?CheckInput@Stage@@QAEXXZ = __ZN5Stage10CheckInputEv
//       DROPPED RECEIVER, and safe because the receiver is not used. QAE is
//       __thiscall: the caller (Stage::Behavior) leaves `this` in ECX and
//       pushes nothing. src/_ZN5Stage10CheckInputEv.cpp defines the body as
//       `void _ZN5Stage10CheckInputEv(void)` -- no parameters, and every value
//       it touches is a global (the Ctrl block at data_0209f498, the touch
//       block, data_0209f2f8), never a field off `this`. Both conventions
//       return with a bare `ret` when there are no stack arguments, so the
//       frame balances. This is the shape method_faces.cpp warns about; it is
//       written down here because it was checked, not because it is obvious.
//
// All four are INERT in this piece. Their LHS is referenced only by
// Stage::Behavior and Stage::Render, neither of which is in this lane's slice,
// so each LHS is absent from walk_window.map -- the "OK (unused)" row of
// alternatename_guard.py. They land now with their provenance so the piece
// that adds Stage::Behavior adds a translation unit and not plumbing.
#pragma comment(linker, "/alternatename:?CanPause@Stage@@SAHXZ=__ZN5Stage8CanPauseEv")
#pragma comment(linker, "/alternatename:?IsPauseDisabled@Stage@@SAHXZ=__ZN5Stage15IsPauseDisabledEv")
#pragma comment(linker, "/alternatename:?RenderBouncingArrows@Stage@@SAXXZ=__ZN5Stage20RenderBouncingArrowsEv")
#pragma comment(linker, "/alternatename:?CheckInput@Stage@@QAEXXZ=__ZN5Stage10CheckInputEv")

/* An object with no code and no data would still be handed to the linker for
   its .drectve, but an empty translation unit is a warning in some MSVC
   configurations and reads as a mistake in review. One byte of file-static
   storage, deliberately outside the .dsstate bracket because it is host
   bookkeeping and not DS state, keeps the object honest. */
static const char w8a_stage_faces_marker = 0;
extern "C" const char *port_w8a_stage_faces_marker(void)
{
    return &w8a_stage_faces_marker;
}
