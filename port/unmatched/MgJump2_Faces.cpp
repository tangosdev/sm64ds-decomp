// PORT_HOST_ABI.  dScMgJump2_c's link faces -- actor id 0x175, scene 373,
// "Bounce and Trounce".  Run mg11, lane BNT.
//
// EVERYTHING HERE WAS NAMED BY A LINK OF THE SEAT, NOT PREDICTED.  The first
// link produced exactly 35 unresolved externals across walk_window, and they
// fall into four kinds.  Only three of the four are faces; the fourth is a
// slice line, which is the category error port/mg_fanout_costs.txt section 10
// records facegen making over _ZN6Memory8AllocateEji and which
// unmatched/MgCoin_Faces.cpp records making again.  A name being unresolved
// does not make it a face.
//
//   27  C++-LINKAGE SPELLINGS of C-named data the mounts already define
//    1  a BARE NAME that exists in no config (func_020beb74)
//    3  C++ METHOD spellings of Itanium-named bodies already in the build
//    3  ordinary slice lines (src/func_02053ea0.c, _02053eb0.c, _02054748.c)
//    1  an arm9 .data table nothing had referenced before (data_02082614) --
//       NOT a face either: it is a romdata.py NAMED entry, see section 4
//
// ---- 1. THE C++-LINKAGE SPELLINGS ----------------------------------------
//
// The ordinary case port/mg_fanout_costs.txt section 4 counts twenty-two of.
// A //cpp TU declares a C-named DS symbol ABOVE its extern "C" block (or with
// no extern "C" at all), so MSVC mangles the reference with the declared type
// while the mount defines the plain C name.  Both sides are the same object at
// the same address and nothing about the type or the calling convention
// differs, so an alias is the whole fix.  Every right-hand side below was
// checked to be a real definition: build/port/host-src/ov006_syms.c defines
// all twenty-five ov006 names, hal/fdr_arm9_fader_seat.cpp defines
// data_0209f5f8 and hal/actor_vtables.cpp defines data_0209f5c0, both inside
// extern "C" and therefore as the plain C name.
//
// port/tools/alternatename_guard.py checks the other half POST-LINK on every
// build: it fails if an /alternatename left-hand side is ALSO a defined symbol,
// which is the failure mode that would make one of these lines silently
// useless.
//
// ONE SYMBOL HAS TWO C++ SPELLINGS AND BOTH ARE REAL.  data_ov006_0214042c is
// declared `void *` by src/func_ov006_020c8084.cpp and `int` by
// src/func_ov006_020c893c.cpp, so the link asks for ?...@@3PAXA AND ?...@@3HA.
// Two rows, one right-hand side; that is the two TUs disagreeing about the
// type of one DS word, not two objects.

/* the nine SharedFilePtr records the two sub-object families construct */
#pragma comment(linker, "/alternatename:?data_ov006_02140330@@3USharedFilePtr@@A=_data_ov006_02140330")
#pragma comment(linker, "/alternatename:?data_ov006_02140338@@3USharedFilePtr@@A=_data_ov006_02140338")
#pragma comment(linker, "/alternatename:?data_ov006_02140438@@3USharedFilePtr@@A=_data_ov006_02140438")
#pragma comment(linker, "/alternatename:?data_ov006_02140440@@3USharedFilePtr@@A=_data_ov006_02140440")
#pragma comment(linker, "/alternatename:?data_ov006_02140448@@3USharedFilePtr@@A=_data_ov006_02140448")
#pragma comment(linker, "/alternatename:?data_ov006_02140450@@3USharedFilePtr@@A=_data_ov006_02140450")
#pragma comment(linker, "/alternatename:?data_ov006_02140458@@3USharedFilePtr@@A=_data_ov006_02140458")
#pragma comment(linker, "/alternatename:?data_ov006_02140460@@3USharedFilePtr@@A=_data_ov006_02140460")
#pragma comment(linker, "/alternatename:?data_ov006_02140468@@3USharedFilePtr@@A=_data_ov006_02140468")

/* the plain-integer and pointer spellings */
#pragma comment(linker, "/alternatename:?data_ov006_0213afd8@@3HA=_data_ov006_0213afd8")
#pragma comment(linker, "/alternatename:?data_ov006_0213b0cc@@3HA=_data_ov006_0213b0cc")
#pragma comment(linker, "/alternatename:?data_ov006_02140408@@3HA=_data_ov006_02140408")
#pragma comment(linker, "/alternatename:?data_ov006_0214040c@@3HA=_data_ov006_0214040c")
#pragma comment(linker, "/alternatename:?data_ov006_02140424@@3HA=_data_ov006_02140424")
#pragma comment(linker, "/alternatename:?data_ov006_02140430@@3HA=_data_ov006_02140430")
#pragma comment(linker, "/alternatename:?data_ov006_0213b088@@3PAHA=_data_ov006_0213b088")
#pragma comment(linker, "/alternatename:?data_ov006_0213b090@@3PAHA=_data_ov006_0213b090")
#pragma comment(linker, "/alternatename:?data_ov006_02141a44@@3PAXA=_data_ov006_02141a44")
#pragma comment(linker, "/alternatename:?data_ov006_02141a48@@3PAXA=_data_ov006_02141a48")
#pragma comment(linker, "/alternatename:?data_ov006_0212ddd0@@3DA=_data_ov006_0212ddd0")
#pragma comment(linker, "/alternatename:?data_ov006_02140400@@3EA=_data_ov006_02140400")
#pragma comment(linker, "/alternatename:?data_ov006_02140404@@3GA=_data_ov006_02140404")
#pragma comment(linker, "/alternatename:?data_ov006_02134d1c@@3PAUOamAttr@@A=_data_ov006_02134d1c")
/* the one word two TUs type differently */
#pragma comment(linker, "/alternatename:?data_ov006_0214042c@@3PAXA=_data_ov006_0214042c")
#pragma comment(linker, "/alternatename:?data_ov006_0214042c@@3HA=_data_ov006_0214042c")
/* two arm9 globals the HAL hosts, spelled at C++ linkage by their readers */
#pragma comment(linker, "/alternatename:?data_0209f5f8@@3EA=_data_0209f5f8")
#pragma comment(linker, "/alternatename:?data_0209f5c0@@3PAUGObj@@A=_data_0209f5c0")

// ---- 2. THE ONE BARE NAME ------------------------------------------------
//
// src/func_ov006_020e7124.c declares `extern int func_020beb74[];` and reads
// it.  There is no func_020beb74 in any config: the address 0x020beb74 is
// ov004's data_ov004_020beb74, a bss symbol.  This is the same defect class as
// src/func_ov006_020e3578.c's bare `func_020adc74` (port/mg_fanout_costs.txt
// section 6) and unmatched/MgCoin_Faces.cpp's `func_020beb68` -- a DATA symbol
// spelled with the func_ prefix, which no linker can be expected to guess.
//
// THE RESOLUTION IS NOT A GUESS.  data_ov004_020beb68 is dScMgBase_c's "current
// minigame scene" pointer, and 0x020beb74 is three words past it in the same
// ov004 bss run; the ROM instruction that reads it here is a plain `ldr` of a
// word out of the ov004 range, and ov004 is the co-resident overlay for every
// ov006 minigame.  The alias resolves the SPELLING, not the address.
#pragma comment(linker, "/alternatename:_func_020beb74=_data_ov004_020beb74")

// ---- 2b. A SECOND BARE NAME, AND IT IS A FUNCTION ------------------------
//
// src/func_ov006_020c7c68.c declares and calls `func_ov006_020e6df0`, which is
// the ADDRESS-SHAPED spelling of an ov006 body the config gives a real name:
// config/arm9/overlays/ov006/symbols.txt:  Sound_PlayBank1Panned
// kind:function(arm,size=0x4c) addr:0x020e6df0.  src/Sound_PlayBank1Panned.cpp
// is in port/slice_bnt.txt and defines it under the NAME, so this is a
// spelling and not a slice line -- the same shape scene_mg_flower.cpp's
// `_func_02012754 = __ZN5Sound12PlayBank2_2DEj` row records for the arm9 half
// of the same family.  Both sides are cdecl and the three arguments agree.
#pragma comment(linker, "/alternatename:_func_ov006_020e6df0=_Sound_PlayBank1Panned")

// ---- 3. THE THREE C++ METHOD SPELLINGS -----------------------------------
//
// Three bodies are already in the build under their Itanium C names and are
// referenced by their MSVC method mangling.  The bridge is the same
// /alternatename shape hal/bob_enemy_bridges.cpp uses for two other spellings
// of Animation::LoadFile and hal/cxx_aliases.cpp uses for a third; a leading
// underscore is MSVC's cdecl decoration, so the right-hand sides are the
// Itanium names with one more underscore.
//
//   src/func_ov006_020c6e4c.cpp calls Animation::LoadFile and ModelAnim::
//   SetAnim as METHODS.  src/_ZN9Animation8LoadFileER13SharedFilePtr.cpp and
//   src/_ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj.c define both, and the
//   second is already in port/slice_gate7.txt, so nothing new is linked.
//
//   src/_ZN17MgBounceAndPounce11AfterRenderEj.cpp calls the bare name
//   `Scene_AfterRender`, which include/decl_common.h declares as
//   `void(void*, unsigned int)` and which exists in no config.  The address is
//   0x0202e398 -- dScMgBase_c's own slot 11 in the base table -- and
//   src/_ZN5Scene11AfterRenderEj.cpp is that body, already in
//   port/slice_w1l2.txt.  IT IS AN ARM TAIL VENEER, spelled in src as
//   `void(void)` calling _ZN9ActorBase11AfterRenderEj, which is why the arity
//   difference is safe rather than the dropped-receiver defect
//   hal/method_faces.cpp's checklist warns about: both sides are CDECL, the
//   CALLER cleans, and the callee reads neither argument.  The ROM does the
//   same thing -- r0 and r1 ride through the veneer untouched into an empty
//   body.
#pragma comment(linker, "/alternatename:?LoadFile@Animation@@SAPAUBCA_File@@AAUSharedFilePtr@@@Z=__ZN9Animation8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:_Scene_AfterRender=__ZN5Scene11AfterRenderEj")

// ---- 3b. THE THIRD ONE IS NOT AN ALIAS, AND THE FIRST RUN PROVED IT ------
//
// ModelAnim::SetAnim IS AN INSTANCE METHOD, so `?SetAnim@ModelAnim@@QAEHPAU
// BCA_File@@HHI@Z` is __THISCALL: the caller leaves `this` in ECX and pushes
// only the four real arguments.  An /alternatename straight onto the CDECL
// body __ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj is therefore the
// MIS-BRIDGED RECEIVER defect hal/method_faces.cpp's checklist lists as
// failure mode 3, and this lane shipped it into a build before catching it.
//
// THE FAULT, so the shape is on the record rather than the conclusion:
//
//     FAULT code c0000005 at +0x000c1c1a accessing 00000002
//     +0x000c1c1a -> __ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj +0x2a
//
// +0x2a is `ldr r0,[r5,#0x60]`'s host equivalent -- the body reading `thiz`,
// which had arrived holding the BCA_File* the caller pushed first, because the
// real receiver never left ECX.  0x00000002 is what that read produced.
//
// THE FIX IS THE SHADOW-PLUS-ALIAS FORM hal/actor_faces_bob.cpp uses for
// ModelBase::SetFile, and for the same second reason: the definition cannot be
// spelled twice.  hal/bob_enemy_shadow_faces.cpp already carries a VOID-
// returning ModelAnim::SetAnim face for ov084's caller, and this caller's
// shadow declares it INT-returning, so the two manglings differ (QAEX vs QAEH)
// and both are needed.  A face struct under a different name gets the
// __thiscall convention from the language, forwards `this` as the first
// argument, and an alias points the real mangling at it.
//
// THE INT IS THE TRUTHFUL SHAPE, checked in the ROM rather than assumed.
// ModelAnim::SetAnim at arm9 0x02016748 ends BOTH of its arms with a `bl`
// (0x02015bf8 and 0x02015c20) and then pops and returns, so r0 rides the
// callee's value out -- the same reading actor_faces_bob.cpp makes for
// ModelBase::SetFile.  src spells the definition `void` because nothing in the
// matched tree read it; func_ov006_020c6e4c discards the result too, so the
// declared int changes no behaviour and only keeps eax from being invented.

struct BCA_File;
extern "C" int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        void *thiz, BCA_File *animFile, int flags, int speed, unsigned start);

struct ModelAnimFace {
    int SetAnim(BCA_File *animFile, int flags, int speed, unsigned start);
};
int ModelAnimFace::SetAnim(BCA_File *animFile, int flags, int speed,
                           unsigned start)
{
    return _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(this, animFile, flags,
                                                       speed, start);
}
#pragma comment(linker, "/alternatename:?SetAnim@ModelAnim@@QAEHPAUBCA_File@@HHI@Z=?SetAnim@ModelAnimFace@@QAEHPAUBCA_File@@HHI@Z")

// ---- 4. WHAT IS NOT HERE, AND WHY ----------------------------------------
//
// FOUR of the 35 are not faces and are not in this file.
//
//   src/func_02053ea0.c, src/func_02053eb0.c and src/func_02054748.c are
//   ordinary matched arm9 TUs that simply had no caller in the build before
//   this seat.  They are lines in port/slice_bnt.txt.
//
//   data_02082614 is a 0x100-byte arm9 .data table -- the delta to
//   data_02082714, which port/tools/romdata.py already emits -- that
//   func_ov006_020ef834 loads and reads a SIGNED HALFWORD out of at
//   0x020ef8c8.  It is added to romdata.py's NAMED list rather than hosted
//   here, because it has REAL ROM BYTES behind it (it is below bss_start and
//   carries no relocations in its span, both checked) and a zeroed host array
//   would be a silent wrong answer rather than a link error.  That is the
//   run mg9 lane MMD ruling about hosted globals: size them by the ROM span
//   and give them the ROM's bytes, or do not host them at all.
//
// ---- 5. NO TRAP, AND THAT IS A MEASUREMENT -------------------------------
//
// unmatched/MgCoin_Faces.cpp carries a named trap because one of its Render
// callees has no src in either extension.  This class has none: the closure
// walk found exactly ONE body with no src at all, func_ov004_020ae858, and it
// is ALREADY TRAPPED in hal/scene_mg_faces.cpp where seven ov006 TUs across
// this family already reach it.  port/slice_bnt.txt section 11 is the
// accounting.
//
// AMENDED, run mg12 lane PANEL: that body now HAS a src file and is NOT trapped.
// src/func_ov004_020ae858.cpp is the results panel's label renderer, seated off
// port/slice_mg1.txt; the closure statement above is otherwise unchanged.

/* MSVC emits nothing for a TU that is only pragmas, and an empty object still
   carries the linker directives. This symbol exists so a reader grepping for
   who owns the aliases finds a definition rather than only a comment. */
extern "C" const char port_mg_jump2_faces_marker[] =
    "dScMgJump2_c faces, run mg11 lane BNT";
