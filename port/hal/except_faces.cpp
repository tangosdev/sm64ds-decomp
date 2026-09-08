// LANE EXCEPT, batch 2 (run link100): sixteen cdecl faces for the actor-class
// Render host copies whose stated reason was a vtable numbering that lane
// SLOT5F retired.
//
// THE READING, and it is the same one lane FOLD2's hal/fold2_faces.cpp opens
// with. mwccarm gives `virtual ~Class()` TWO vtable entries -- the Itanium
// complete/deleting pair -- and MSVC folds them into ONE, which pulled every
// virtual declared after the destructor up a slot all the way down
// ModelBase -> Model -> ModelAnim -> {ModelAnim2, BlendModelAnim}. SLOT5F
// respelled that pair as two plain virtuals under _MSC_VER in
// include/ModelBase.h and its five siblings, so the host tables are
// ROM-numbered again and index 5 is Render on every one of them:
//
//     hal/cxxname_bridge.cpp:522   _ZTV5Model[5]      = mv_render
//                          :578    _ZTV9ModelAnim[5]  = ma2_render
//                          :566    _ZTV10ModelAnim2[5]= ma2_render
//
// Every matched source these faces serve draws its model through a LOCAL
// SIX-VIRTUAL shadow class counted in ROM numbering and dispatches index 5.
// The folded host table put Virtual18 there -- a different method taking a
// different argument, which is the measured c0000005 the Butterfly, the Fish,
// the QuestionBlock and the Whomp copies all recorded. It does not now.
//
// THE CONVENTION AGREES, and that is the half worth stating because the index
// is not the whole story. MSVC compiles the shadow's `o->m5(x)` as a
// __thiscall virtual call: `this` in ecx, the one argument pushed, callee
// cleans four. What is seated is
//
//     void __fastcall mv_render(void *self, void *unused, const void *scale)
//
// -- ecx = self, edx unread, scale pushed, callee cleans four. Same receiver
// register, same stack argument, same cleanup. The only difference is the
// dummy edx, which mv_render never reads. Four of the sixteen shadows declare
// the slot returning int where Render returns void; that leaves garbage in
// eax and all four discard it.
//
// WHY THE FACES. Each fill site in hal/actor_classes_ov0NN.cpp DECLARES the
// Itanium name at C linkage and calls it, because the definition it used to
// reach was an extern "C" host copy in port/unmatched. The matched sources
// define __thiscall METHODS, whose MSVC mangling is a different symbol, and an
// /alternatename cannot bridge ecx against the stack. So each row below is a
// cdecl definition of the C name onto the method, and NOT ONE FILL SITE
// CHANGES.
//
// src/_ZN7Tornado6RenderEv.cpp is NOT here: it defines the C name itself.
//
// The per-row ROM vtable word (each class's own slot 9, read out of the raw
// overlay image at that overlay's .text base) and the kind:function record at
// the destination are in port/slice_except2.txt.

#include "CheepCheep.h"
#include "ChiefChilly.h"
#include "Clam.h"
#include "FirePiranhaPlantBig.h"
#include "HeaveHo.h"
#include "Lakitu.h"
#include "LakituBro.h"
#include "MantaRay.h"
#include "MrI.h"
#include "PiranhaPlant.h"
#include "Rabbit.h"
#include "Shark.h"
#include "Skeeter.h"
#include "Spiny.h"
#include "Stump.h"
#include "TreasureChest.h"

/* TWO NAME BRIDGES, and they are the whole reason this file has a pragma in it.
   Two of the sixteen matched sources declare a mount symbol OUTSIDE their own
   extern "C" block, so MSVC gives the declaration C++ linkage and asks the
   linker for a mangled name that nothing defines:

     src/_ZN7HeaveHo6RenderEv.cpp        extern int data_0209f32c;
     src/_ZN12PiranhaPlant6RenderEv.cpp  extern G2  data_ov084_02130df4;

   The mount defines both at C linkage (_data_0209f32c out of
   hal/cxx_aliases.cpp, _data_ov084_02130df4 out of ov084_syms.c.obj), so each
   is one /alternatename away, the mechanism hal/actor_classes.cpp:863 and
   hal/actor_classes_bob_enemy.cpp:333-336 already use for this exact shape.
   BOTH MANGLINGS WERE READ OFF THE OBJECT, not guessed: each source was
   compiled standalone under the port's own flags and dumpbin printed

     ?data_0209f32c@@3HA (int data_0209f32c)
     ?data_ov084_02130df4@@3UG2@@A (struct G2 data_ov084_02130df4)

   as its only C++-mangled UNDEF. The third such UNDEF in the batch,
   Lakitu's ?Update@TextureSequence@@QAEXAAUModelComponents@@@Z, needs nothing:
   src/_ZN15TextureSequence6UpdateER15ModelComponents.cpp already defines that
   exact thiscall symbol and the map has it. */
#pragma comment(linker, "/alternatename:?data_0209f32c@@3HA=_data_0209f32c")
#pragma comment(linker, "/alternatename:?data_ov084_02130df4@@3UG2@@A=_data_ov084_02130df4")

extern "C" {

int _ZN3MrI6RenderEv(void *s)                 { return ((MrI *)s)->MrI::Render(); }
int _ZN11ChiefChilly6RenderEv(void *s)        { return ((ChiefChilly *)s)->ChiefChilly::Render(); }
int _ZN4Clam6RenderEv(void *s)                { return ((Clam *)s)->Clam::Render(); }
int _ZN13TreasureChest6RenderEv(void *s)      { return ((TreasureChest *)s)->TreasureChest::Render(); }
int _ZN5Spiny6RenderEv(void *s)               { return ((Spiny *)s)->Spiny::Render(); }
int _ZN5Shark6RenderEv(void *s)               { return ((Shark *)s)->Shark::Render(); }
int _ZN7Skeeter6RenderEv(void *s)             { return ((Skeeter *)s)->Skeeter::Render(); }
int _ZN8MantaRay6RenderEv(void *s)            { return ((MantaRay *)s)->MantaRay::Render(); }
int _ZN6Lakitu6RenderEv(void *s)              { return ((Lakitu *)s)->Lakitu::Render(); }
int _ZN7HeaveHo6RenderEv(void *s)             { return ((HeaveHo *)s)->HeaveHo::Render(); }
int _ZN10CheepCheep6RenderEv(void *s)         { return ((CheepCheep *)s)->CheepCheep::Render(); }
int _ZN5Stump6RenderEv(void *s)               { return ((Stump *)s)->Stump::Render(); }
int _ZN12PiranhaPlant6RenderEv(void *s)       { return ((PiranhaPlant *)s)->PiranhaPlant::Render(); }
int _ZN19FirePiranhaPlantBig6RenderEv(void *s){ return ((FirePiranhaPlantBig *)s)->FirePiranhaPlantBig::Render(); }
int _ZN6Rabbit6RenderEv(void *s)              { return ((Rabbit *)s)->Rabbit::Render(); }
int _ZN9LakituBro6RenderEv(void *s)           { return ((LakituBro *)s)->LakituBro::Render(); }

}  /* extern "C" */
/* ==========================================================================
   LANE FACEF (run link100, wave 7), gate 1: the calling-convention-face rows.
   ==========================================================================

   SAME MECHANISM, SAME SHAPE, DIFFERENT QUEUE FAMILY. The sixteen faces above
   are lane EXCEPT's family E (vtable slot / virtual-shadow dispatch). The rows
   below are the ones out/EXCEPT/census.md ranked as family F -- "calling-
   convention face" -- plus the family E leftovers whose SECOND stated reason
   turned out to be this same repair rather than a separate wall. Each was
   re-derived from the ROM one row at a time; the per-row referrer (the class's
   own vtable slot 9 word, or the arm_call in relocs.txt for a body the ROM
   calls directly), the RTTI name string at vtable-4 and the kind:function
   record at the destination are in port/slice_facef.txt.

   WHY A FACE AND NOT A FILL EDIT, restated for this section because it is the
   whole reason the file exists: each fill site declares the ITANIUM C NAME at
   C linkage and calls it, because what it used to reach was an extern "C"
   host copy in port/unmatched. A matched source that recovered as a real C++
   method defines ?Render@Class@@QAEHXZ instead, which is a different symbol,
   and an /alternatename cannot bridge ECX against the stack. So each row below
   is a cdecl definition of the C name onto the method, and NOT ONE FILL SITE
   CHANGES. A matched source that defines the plain C name itself is not here
   at all -- it needs no face, and four of this lane's rows are that shape.

   THE CONVENTION AGREES, measured the same way lane EXCEPT measured it. MSVC
   compiles the matched TU's `shadow->m(x)` as a __thiscall virtual call --
   `this` in ecx, the one argument pushed, callee cleans four -- and the host
   slot holds mv_render / ma2_render / mv_virtual10, all declared
   `__fastcall(void *self, void *unused, ...)`: ecx = self, edx unread, the
   argument pushed, callee cleans four. Same receiver register, same stack
   argument, same cleanup.

   AND THE RETURN-TYPE OBJECTION IS NOT ONE. Three of this lane's rows carried
   a second stated reason -- "the shadow's methods return int where
   Model::Render returns void, so MSVC decorates that differently". An
   INDIRECT call through the object's own vptr decorates nothing: the declared
   return type only decides whether EAX is read after the call, and every one
   of these bodies discards it. Lane EXCEPT shipped four seats of exactly that
   shape and the battery was green. */
#include "FloatOnWaterPlatformJrb.h"
#include "Goomboss.h"
#include "Player.h"
#include "ShipUp.h"

extern "C" {

/* Goomboss::Render, ROM 0x02121b70, _ZTV8Goomboss[9] @ 0x02122edc ("12daKuriKing_c"). */
int _ZN8Goomboss6RenderEv(void *s)            { return ((Goomboss *)s)->Goomboss::Render(); }
/* ShipUp::Render, ROM 0x02112744, _ZTV6ShipUp[9] @ 0x02114a60 ("14daObjKi_Fune_c"). */
int _ZN6ShipUp6RenderEv(void *s)              { return ((ShipUp *)s)->ShipUp::Render(); }
/* FloatOnWaterPlatformJrb::Render, ROM 0x02113130. The class name on the table
   is a decoy the host file recorded and this lane re-read from the ROM: the
   RTTI name string behind _ZTV23FloatOnWaterPlatformJrb[-1] @ 0x02114c88 says
   "13daSlide_Box_c", so the table SLIDING_BOX (313) installs is the one that
   holds this body at slot 9 @ 0x02114cb0. */
int _ZN23FloatOnWaterPlatformJrb6RenderEv(void *s)
{ return ((FloatOnWaterPlatformJrb *)s)->FloatOnWaterPlatformJrb::Render(); }
/* Player::BlowAway, ROM 0x020d4fe4. Not a vtable row: the ROM reaches this body
   by two arm_calls (ov027 from:0x0211221c, ov091 from:0x02133b84), and every
   caller in the tree spells the FLAT name include/decl_Player.h:38 declares at
   C linkage. The matched TU recovered as a real __thiscall method, so the flat
   name is this one cdecl line and no call site changes. */
void _ZN6Player8BlowAwayEs(void *s, short v) { ((Player *)s)->Player::BlowAway(v); }

}  /* extern "C" */

/* ---- RUN link100 LANE PMFB2 ---------------------------------------------
   The two rows lane FACEF proposed and could not take, because their stated
   precondition was a ROM-ORDERED ModelAnim table and the tree did not have one
   at the time. It does now: hal/cxxname_bridge.cpp fills _ZTV9ModelAnim[5] =
   ma2_render and [6] = ma2_virtual18, the numbering include/ModelAnim.h
   annotates on the ROM table. Both matched TUs therefore dispatch the slot the
   ROM dispatches. The full derivation, including the ten ROM instructions each
   body is, is in the retirement block of hal/method_faces.cpp; the ROM read is
   runs/link100/out/PMFB2/rom_renders.txt.

   Same shape as the rows above: the matched source recovered as a real C++
   method (`int Bird::Render()`, `int Flag::Render()`), the fill sites in
   hal/actor_classes.cpp:1334 and :1506 call the Itanium C name at C linkage,
   so each row is one cdecl definition of that name onto the method and no fill
   site changes. */
#include "Bird.h"
#include "Flag.h"

extern "C" {

/* Bird::Render, ROM 0x02111870 (ov009, kind:function(arm,size=0x28)); the fill
   site is hal/actor_classes.cpp:1334. */
int _ZN4Bird6RenderEv(void *s)                { return ((Bird *)s)->Bird::Render(); }
/* Flag::Render, ROM 0x0211211c (ov009, kind:function(arm,size=0x28)); the fill
   site is hal/actor_classes.cpp:1506. */
int _ZN4Flag6RenderEv(void *s)                { return ((Flag *)s)->Flag::Render(); }

}  /* extern "C" */

/* ---- RUN link100 LANE PMFB2, THE FAMILY-C CALL-SHAPE ROWS ---------------
   Four of the five pointer-to-member rows this lane took recovered as real
   C++ methods rather than as the Itanium C name, so each needs the same one
   cdecl line the rows above need, and for the same reason: the fill site
   declares the C name at C linkage because what it used to reach was an
   extern "C" host copy in port/unmatched. The fifth,
   _ZN8PathLift12BaseBehaviorEv, defines its C name itself and is not here, and
   neither are batch 1's two ov006 sound dispatchers, for the same reason.

   THESE ROWS' OWN REPAIR IS NOT THIS FILE. What made them takeable is a
   __fastcall face on each STATE BODY, written into the code word by the seat
   that owns the table, because MSVC dispatches a pointer-to-member as
   `call <reg>` with the receiver in ecx and nothing pushed while the state
   bodies are plain cdecl. Those faces live in the seat files
   (port/unmatched/Coin_Behavior.cpp, PowerStar_States.cpp,
   PiranhaPlant_States.cpp) with the per-table ROM evidence. */
#include "Coin.h"
#include "OneUpMushroom.h"
#include "PowerStar.h"
#include "PiranhaPlant.h"

extern "C" {

/* Coin::Behavior, ROM 0x020b2324 (ov002); the fill site is
   hal/actor_classes.cpp. */
int _ZN4Coin8BehaviorEv(void *s)              { return ((Coin *)s)->Coin::Behavior(); }
/* OneUpMushroom::Behavior, ROM 0x020b00e8 (ov002). */
int _ZN13OneUpMushroom8BehaviorEv(void *s)    { return ((OneUpMushroom *)s)->OneUpMushroom::Behavior(); }
/* PowerStar::Behavior, ROM 0x020eb05c (ov002). */
int _ZN9PowerStar8BehaviorEv(void *s)         { return ((PowerStar *)s)->PowerStar::Behavior(); }
/* PiranhaPlant::Behavior, ROM 0x0212fd4c (ov084). */
int _ZN12PiranhaPlant8BehaviorEv(void *s)     { return ((PiranhaPlant *)s)->PiranhaPlant::Behavior(); }

}  /* extern "C" */
