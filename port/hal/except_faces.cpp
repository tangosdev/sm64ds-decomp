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
