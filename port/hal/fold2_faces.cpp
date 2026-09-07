// LANE FOLD2, batch 2 (run link100): ten cdecl faces for the actor-class host
// copies whose only reason was the MSVC destructor fold.
//
// THE FOLD, AND WHY IT IS DEAD. mwccarm gives `virtual ~Class()` TWO vtable
// entries -- the Itanium D1 complete / D0 deleting pair -- and MSVC folds them
// into ONE, which pulls every virtual declared after the destructor up by one
// slot, all the way down ModelBase -> Model -> ModelAnim -> {ModelAnim2,
// BlendModelAnim}. Lane SLOT5F respelled that pair as two plain virtuals under
// _MSC_VER in include/ModelBase.h and its five siblings, so the host tables are
// ROM-numbered again. Read straight out of config/arm9/relocs.txt at each
// table's own address, with every destination resolved in
// config/arm9/symbols.txt:
//
//   _ZTV5Model           0x0208e90c  [3] 0x02016c98 Model::UpdateVerts
//                                    [4] 0x02016bb8 Model::Virtual10
//                                    [5] 0x02016b78 Model::Render
//   _ZTV9ModelAnim       0x0208e980  [3] 0x0201686c ModelAnim::UpdateVerts
//                                    [4] 0x0201682c ModelAnim::Virtual10
//                                    [5] 0x020167f8 ModelAnim::Render
//   _ZTV10ModelAnim2     0x0208e9b4  [3][4][5] the same three words
//   _ZTV14BlendModelAnim 0x0208e94c  [3] 0x02016578 BlendModelAnim::UpdateVerts
//                                    [4] 0x02016518 BlendModelAnim::Virtual10
//                                    [5] 0x020164e4 BlendModelAnim::Render
//
// Every matched source retired by this batch models a model-family member with
// a LOCAL shadow class counted in ROM numbering and dispatches index 5 (Render)
// or index 3 (UpdateVerts). The folded host table put Virtual18 on index 5 and
// Virtual10 on index 3 -- a different function taking a different argument, and
// on index 3 one that also returns `ret 4` and unbalances the caller's stack.
// It does not now, so each source reaches the body it means.
//
// WHY THE FACES. Each of the ten fill sites below DECLARES the Itanium name at
// C linkage and calls it, because the definition it used to reach was an
// extern "C" host copy in port/unmatched. The matched sources define __thiscall
// METHODS, whose MSVC mangling is a different symbol, and an /alternatename
// cannot bridge ecx against the stack. So each row here is a cdecl definition
// of the C name onto the method, and NOT ONE FILL SITE CHANGES. This is
// hal/actor_classes_bob_world.cpp's thirty-row block, one wave later.
//
// WHY A NEW FILE rather than that one: hal/actor_classes_bob_world.cpp names
// the BoB/enemy headers it needs and pulling ten more class headers into it
// would widen a file this lane does not own. This file is compiled into exactly
// the three targets that carried the retired host copies -- walk_window,
// walk_window_hires and smoke_player -- and no others, which is the constraint
// that block's own header states (hal/cxxname_bridge.cpp is compiled into five
// targets and only three link the slice, so a face there would be an unresolved
// external in smoke_actor, smoke_savestate and smoke_persist).
//
// VERIFIED BY ROM ADDRESS, every row: the word at the class's own vtable slot
// in config/arm9/overlays/<ov>/relocs.txt, the kind:function(arm,size=..)
// record at the destination in that overlay's symbols.txt, and a real ARM push
// as the body's first word. None of the sources carries the "recovered from
// vtable slot identity" guess marker.
//
//   BowserPuzzleManager::Render  ov064  _ZTV19BowserPuzzleManager 0x0211c25c
//        + 4*9 = 0x0211c280 -> 0x02119088  function(arm,size=0x28)  e92d4000
//   Cannon::Render               ov098  _ZTV7daCnn_c 0x0213c6a0
//        + 4*9 = 0x0213c6c4 -> 0x0213b214  function(arm,size=0x58)  e92d4010
//   Klepto::Render               ov062  _ZTV9daJango_c 0x0211dd5c
//        + 4*9 = 0x0211dd80 -> 0x0211c920  function(arm,size=0x2c)  e92d4000
//   Koopa::Render                ov062  _ZTV8daNknk_c 0x0211dab4
//        + 4*9 = 0x0211dad8 -> 0x02118f84  function(arm,size=0x168) e92d4010
//   Bowser::Render               ov060  _ZTV7daKpa_c 0x0211a6b8
//        + 4*9 = 0x0211a6dc -> 0x02115f0c  function(arm,size=0x50)  e92d4010
//   PrincessPeach::Behavior      ov085  _ZTV9daPeach_c 0x0212ffc0
//        + 4*6 = 0x0212ffd8 -> 0x0212a52c  function(arm,size=0x5c)  e92d4010
//   Key::Render                  ov089  _ZTV3Key 0x02132ba8
//        + 4*9 = 0x02132bcc -> 0x021320f0  function(arm,size=0xa4)  e92d4010
//   RacingPenguin::Render        ov019  _ZTV13RacingPenguin 0x02113310
//        + 4*9 = 0x02113334 -> 0x02112360  function(arm,size=0x34)  e92d4010
//   Submarine::Render            ov026  _ZTV9Submarine 0x02113d54
//        + 4*9 = 0x02113d78 -> 0x02111fd8  function(arm,size=0x34)  e92d4010
//   Wiggler::Render              ov034  _ZTV7Wiggler 0x021144bc
//        + 4*9 = 0x021144e0 -> 0x02112af4  function(arm,size=0x68)  e92d40f0
//
// Slot 9 is ActorBase::Render and slot 6 is ActorBase::Behavior throughout.
//
// SIX MORE ROWS IN THIS BATCH NEED NO FACE, and they are listed here so the
// count adds up rather than being silently short:
//   func_ov032_02112164        BUBBA's Render, an address-named C symbol its
//                              matched source defines at C linkage already.
//   func_ov027_02111e00        DA_PG_DFDR's Render, likewise. Its source
//                              carries the guess marker and is ADJUDICATED
//                              REAL_DECOMP at inferred_stub_adjudicated.txt
//                              line 1803, so inferred_stub_guard admits it.
//   func_ov036_02112378        FLYING_CARPET's Render, likewise.
//   func_ov002_020e4768        the Player head-model matrix seat, reached by an
//                              arm_call from src/func_ov002_020e4bb8.c.
//   _ZN16BowserShockwaves6RenderEv  its matched source already defines the
//                              Itanium name at C linkage.
//   _ZN6Eyerok6RenderEv        hal/actor_classes_ov066.cpp's ov66_render calls
//                              the C++ METHOD directly, so the matched TU
//                              simply becomes the definition of the same
//                              MSVC-mangled symbol the host copy provided.

#include "BowserPuzzleManager.h"
#include "Cannon.h"
#include "Klepto.h"
#include "Koopa.h"
#include "Bowser.h"
#include "PrincessPeach.h"
#include "Key.h"
#include "RacingPenguin.h"
#include "Submarine.h"
#include "Wiggler.h"

extern "C" {
int _ZN19BowserPuzzleManager6RenderEv(void *s) { return ((BowserPuzzleManager *)s)->BowserPuzzleManager::Render(); }
int _ZN6Cannon6RenderEv(void *s)               { return ((Cannon *)s)->Cannon::Render(); }
int _ZN6Klepto6RenderEv(void *s)               { return ((Klepto *)s)->Klepto::Render(); }
int _ZN5Koopa6RenderEv(void *s)                { return ((Koopa *)s)->Koopa::Render(); }
int _ZN6Bowser6RenderEv(void *s)               { return ((Bowser *)s)->Bowser::Render(); }
int _ZN13PrincessPeach8BehaviorEv(void *s)     { return ((PrincessPeach *)s)->PrincessPeach::Behavior(); }
int _ZN3Key6RenderEv(void *s)                  { return ((Key *)s)->Key::Render(); }
/* char*, not void*: hal/actor_classes_ov019_w3e.cpp declares this one
   `int _ZN13RacingPenguin6RenderEv(char *self)` and calls it with a cast. */
int _ZN13RacingPenguin6RenderEv(char *s)       { return ((RacingPenguin *)s)->RacingPenguin::Render(); }
int _ZN9Submarine6RenderEv(void *s)            { return ((Submarine *)s)->Submarine::Render(); }
int _ZN7Wiggler6RenderEv(void *s)              { return ((Wiggler *)s)->Wiggler::Render(); }
}
