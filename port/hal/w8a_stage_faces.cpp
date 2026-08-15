// THE STAGE CLASS'S C-NAME FACES. (run linkw wave 8, lane w8-a.)
//
// ===========================================================================
// PART 1 -- THERE IS NO REACH FOR THE STAGE BODIES YET, AND THIS FILE DOES NOT
//           MANUFACTURE ONE. Read this before reading a linkage number.
// ===========================================================================
//
// slice_w8a.txt lists nine matched translation units. They COMPILE into
// walk_window and /OPT:REF then DISCARDS every one of them, because nothing in
// the link names them. That is the correct outcome, not a defect to be worked
// around, and linkage.py counts them out on purpose: the campaign's number is
// how much of the decomp RUNS, and a body nothing can call does not run.
//
// The first draft of this file rooted the eight Stage bodies with eight
// `#pragma comment(linker, "/include:...")` directives and the linked count
// went 4623 -> 4634. Review removed the eight directives and measured the
// remainder: 4623 again, exactly. All eleven TUs of that delta were
// directive-manufactured. The directives are gone and the number is 4623.
//
// THE PRECEDENT, corrected. /INCLUDE: is not banned here -- the base build
// already carries eight of them, four in port/ntr/io.cpp and four in
// port/tests/io_pressure.cpp:
//
//     #pragma comment(linker, "/INCLUDE:_tls_used")
//     #pragma comment(linker, "/INCLUDE:_ntr_io_tls_entry")
//
// and every one of those roots a TLS callback THE WINDOWS LOADER ACTUALLY
// INVOKES. The directive is there because the caller is the loader and the
// loader is not visible to the linker, not because the symbol has no caller.
// That is the standard: something real calls it. None of the nine bodies in
// slice_w8a.txt has a caller yet.
//
// WHAT THE HONEST SEAT IS, and how much of this it actually covers.
//
// On the ROM the Stage is dispatched through _ZTV5Stage, the eighteen-entry
// vtable Stage::Stage installs last. hal/stage_bridges.cpp trap-fills all
// twenty slots at boot (hal_fill_stage_vtable) and nothing dispatches through
// the Stage at all -- it is in the scene tree but on neither processing list.
// Seating real words in that table is the next piece and it owns
// stage_bridges.cpp, which this lane does not.
//
// And that seat reaches AT MOST FOUR of the eight Stage bodies here, because
// only four of them are vtable entries:
//
//     slot 1   Stage::BeforeInitResources     0x0202ddc8
//     slot 12  Stage::OnPendingDestroy        0x0202b8a0
//     slot 16  ~Stage (D1/D2)                 0x02023688
//     slot 17  ~Stage (D0)                    0x020236f0
//
// The other four are reached from somewhere else entirely, and every one of
// those callers is itself still blocked:
//
//     Stage::GraphCallback1     the Stage's SceneRelated graphics block,
//                               dispatched by func_02019144 -- a beat the port
//                               does not run and nothing seats data_0209d4a8
//     Stage::CanPause           called from Stage::Behavior
//     Stage::IsPauseDisabled    called from Stage::Behavior
//     Stage::UpdateMenuButtons  called from Stage::PS_Update, Stage::VE_Init,
//                               Stage::VE_Update and Stage::LC_Update
//
// Stage::Behavior needs nine faces and Scene::SetSceneToSpawn; PS_Update and
// LC_Update need data_020a0dea, which cannot be hosted without re-shaping the
// TouchInfo block hal/auto_bss.cpp owns. So even after the vtable seat lands,
// four of these eight stay discarded until their own callers land. Written
// down here so the next lane sizes its piece from this rather than from the
// hope that one table fill reaches everything.
//
// AND ONE MORE THING THE SEAT WILL HAVE TO SOLVE. Stage::OnPendingDestroy's
// object publishes ONLY the MSVC mangling ?OnPendingDestroy@Stage@@QAEXXZ --
// never the Itanium C name _ZN5Stage16OnPendingDestroyEv, because
// src/_ZN5Stage16OnPendingDestroyEv.cpp compiles a real C++ method and does
// not wrap it in extern "C" (verified with dumpbin /symbols on its object:
// one External, that mangling, nothing else). QAE is __thiscall. So slot 12
// cannot be filled with a plain function pointer to a C name that does not
// exist; it needs a face or a __fastcall thunk, the same shape every other
// slot in hal/stage_bridges.cpp already uses.

// ===========================================================================
// PART 2 -- THE C-NAME FACES.
// ===========================================================================
//
// The ROM's callers spell these methods with the Itanium C name; the matched
// TU that defines each one is that C name (a .c file, or a .cpp that wraps the
// definition in extern "C"). Where the two spellings are the same function
// with the same calling convention, a linker alias is the whole face.
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
//   ?func_020aba70@@3PAPAUOamAttr@@A = _func_020aba70
//       A DATA face, not a code one, and NOT a chained alias: _func_020aba70
//       is a real definition in the base map (ov001_syms.c.obj, the ov001
//       per-symbol mount) at the OAM::NUMBERS pointer table, so this aliases
//       onto a defined symbol rather than onto another alias.
//       src/_ZN5Stage12RenderNumberEhiibi.cpp declares it as
//       `extern OamAttr *func_020aba70[]` without extern "C", which is where
//       the mangling comes from.
//
// ALL FIVE ARE INERT TODAY. Their LHS is referenced only by Stage::Behavior,
// Stage::Render and Stage::RenderNumber, none of which is in this lane's
// slice, so each LHS is absent from walk_window.map -- the "OK (unused)" row
// of alternatename_guard.py, which the guard confirms (910 fired, unchanged
// from base; 0 new defeats). They land now with their provenance so the piece
// that adds those bodies adds translation units and not plumbing.
#pragma comment(linker, "/alternatename:?CanPause@Stage@@SAHXZ=__ZN5Stage8CanPauseEv")
#pragma comment(linker, "/alternatename:?IsPauseDisabled@Stage@@SAHXZ=__ZN5Stage15IsPauseDisabledEv")
#pragma comment(linker, "/alternatename:?RenderBouncingArrows@Stage@@SAXXZ=__ZN5Stage20RenderBouncingArrowsEv")
#pragma comment(linker, "/alternatename:?CheckInput@Stage@@QAEXXZ=__ZN5Stage10CheckInputEv")
#pragma comment(linker, "/alternatename:?func_020aba70@@3PAPAUOamAttr@@A=_func_020aba70")

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
