// LANE w2-b SEAT (run linkw, wave 2) -- destructor heads on the ROM vtable
// storage the port hosts but never fills.
//
// This is wave 1's meshcollider_dtor_seat mechanism applied to what a full
// sweep of the port's hosted vtable storage turned up. The sweep read every
// two-word head in config/arm9 and every overlay config against the port's
// host definitions, and asked one question per table: do the ROM's own
// relocations at base+0 and base+4 name a matched TU that the link does not
// carry? Thirty tables answered yes. Twenty-nine of them are blocked for
// reasons recorded at the bottom of this file. What is left is seated here.
//
// WHY A SEAT AND NOT JUST A SLICE LINE, one more time. Release links with
// /OPT:REF, so a matched TU added to a slice with nothing referencing it is
// dropped before the map is written and the headline does not move. The
// reference edge IS the work; a ROM vtable slot the port never filled is the
// honest place to put one, because the ROM says what belongs there.
//
// ===========================================================================
// SEATED: _ZTV12CylinderClsn, arm9 0x0208e6ec, slots 0 and 1.
//
// The ROM's own two words, config/arm9/relocs.txt:
//
//     from:0x0208e6ec kind:load to:0x020150a8 module:main   D1 (complete)
//     from:0x0208e6f0 kind:load to:0x0201507c module:main   D0 (deleting)
//
// and config/arm9/symbols.txt puts a class destructor on both addresses:
//
//     _ZN5dCc_cD1Ev kind:func addr:0x020150a8
//     _ZN5dCc_cD0Ev kind:func addr:0x0201507c
//
// THE INDEX CONVENTION IS THE CLASS'S OWN, not an inference. src/
// _ZN5dCc_cD1Ev.c -- byte-matched, and the body this seat names at
// index 0 -- opens by writing the table base as the object's vptr:
//
//     *(int*)self = (int)data_0208e6ec; // set vptr
//
// so 0x0208e6ec is the vptr address, index 0 is the first virtual slot, and
// the Itanium pair sits at 0 and 1. src/_ZN5dCc_cD0Ev.cpp writes the
// same base and adds the Memory::operator_delete2 call, which is the D1/D0
// split every class in this band shows.
//
// TWO WORDS, NO MORE. dsd's next data symbol after 0x0208e6ec is 0x0208e704,
// so this head has room for six words, but there is relocation evidence for
// exactly two and this file does not own the storage. Index 2 and up stay
// zero.
//
// NOTHING FILLS IT AND NOTHING DISPATCHES IT. The storage is
// `int data_0208e6ec[4]` in hal/cxx_aliases.cpp, in the BSS ring the gate-10
// aliases land on; the only other mention of the name in the whole port is
// the /alternatename pragma four lines above it. No fill in the tree ever
// wrote a slot and no call site reads one, so this changes no behaviour: it
// makes the port's copy of the table carry the two words the ROM puts there,
// and that is the reference edge that pulls the two matched TUs into the link.
// The callee closure was already linked before this seat (_ZN5dCc_c6UnlinkEv and
// Memory::operator_delete2 are both in the map).
//
// ABI. The flat-C bodies are `void *f(void *this)` with `this` on the stack.
// A ROM-shadow dispatch through an MSVC vtable would arrive __thiscall with
// `this` in ecx, so each slot takes the ecx->arg adapter rather than the body
// directly -- the same convention hal/meshcollider_dtor_seat.cpp and
// hal/model_dtor_seat.cpp use for every slot they seat.
// ===========================================================================
//
// ===========================================================================
// SEATED (wave 3, lane w3-b): _ZTV8dScene_c, arm9 0x02092680, slot 1.
//
// This is the head group 4 at the bottom of this file found and could not
// take. Group 4's blocker was storage, not shape: the body's callee chain
// reaches Scene::ResetFadersAndSound, which writes data_0209f1e4, and the
// port hosted that DS BSS symbol nowhere. It is hosted now, in
// hal/auto_bss.cpp with its sizing evidence, so the head can be seated.
//
// THE ROM's OWN WORD, config/arm9/relocs.txt:
//
//     from:0x02092684 kind:load to:0x0202e638 module:main
//
// and config/arm9/symbols.txt puts a Scene virtual on both ends:
//
//     _ZTV8dScene_c                       kind:data(any)          addr:0x02092680
//     _ZN8dScene_c19BeforeInitResourcesEv kind:function(arm,...)  addr:0x0202e638
//
// THE INDEX CONVENTION IS THE TABLE'S OWN. 0x02092684 is base+4, so this is
// slot 1, and slot 1 of an ActorBase-derived table is BeforeInitResources by
// the slot order hal/stage_bridges.cpp reads out of the sibling table at
// 0x020921c0. The two tables agree face for face: 0x02092680's slot 0 is
// ActorBase::InitResources and 0x0209268c's is ActorBase::CleanupResources,
// which are slots 0 and 3 of that order.
//
// THE HEAD IS EIGHTEEN WORDS, and every one of them is a cited relocation
// (from:0x02092680 through from:0x020926c4; dsd's next data symbol is
// 0x020926c8, so 0x48/4 = 18 -- the same eighteen-entry shape the Stage's
// table has). EIGHT of the eighteen are seated: 1, 4, 7, 8, 10, 11, 16, 17.
//
//     slot  reloc        body                                 why
//      1    0x02092684   Scene::BeforeInitResources           seated
//      4    0x02092690   Scene::BeforeCleanupResources        seated (wave 4)
//      7    0x0209269c   Scene::BeforeBehavior                seated (wave 4)
//      8    0x020926a0   Scene::AfterBehavior                 seated (wave 4)
//     10    0x020926a8   Scene::BeforeRender                  seated
//     11    0x020926ac   Scene::AfterRender                   seated (wave 4)
//     16    0x020926c0   Scene::~Scene (D1)                   seated
//     17    0x020926c4   Scene::~Scene (D0)                   seated
//      0,3,6,9,12,13,14,15   ActorBase faces                  already linked
//      2,5                   see group 4 at the bottom        blocked
//
// Slots 0, 3, 6, 9, 12, 13, 14 and 15 name ActorBase faces whose TUs are
// already in the map, so a seat there buys no edge at all.
//
// THE FOUR SEATED SLOTS ARE THE ONES WHOSE ABI ACTUALLY LINES UP, which is
// the whole of the difference between this batch and the six blocked slots.
// Each of the four matched bodies is flat C or extern "C", takes its receiver
// as an ORDINARY FIRST ARGUMENT, and calls callees whose C-linkage cdecl names
// the map already defines:
//
//     Scene::BeforeRender  -> _ZN7fBase_c12BeforeRenderEv, and the C name is
//                             defined by hal/method_faces.cpp:305 as
//                             `int f(void *self)` -- receiver passed, not
//                             dropped
//     Scene::~Scene D1/D0  -> _ZN7fBase_cD2Ev (flat C, pointer argument) and
//                             _ZN6Memory10DeallocateEPvP4Heap, both already
//                             C-named in the map, plus data_0208e4b8 and
//                             data_020a0eac which are hosted
//
// so nothing here needs an adapter invented for it. The six blocked slots all
// fail that test in one of two documented ways and are written up below.
//
// RE-PROVING THE THREE THINGS THE MECHANISM DEPENDS ON.
//
// (a) REAL HOSTED STORAGE. `void *_ZTV8dScene_c[20]` in hal/stage_bridges.cpp,
//     line 83, in that file's extern "C" block. It is the port's own array,
//     not an alias of another table and not romdata.
//
// (b) NOTHING ELSE WRITES THE TABLE. The mechanism below is a pre-main
//     static initialiser, so it is only sound if no runtime fill writes the
//     same storage -- a fill would simply overwrite the seat and the port's
//     table would still not carry the ROM's word. Swept, and this is the
//     evidence, not a recollection:
//
//       - grep for `_ZTV8dScene_c` and `data_02092680` across the whole tree
//         (port/, src/, unmatched/, include/, config/) returns: the
//         definition in hal/stage_bridges.cpp:83, prose in that file's
//         comment and in this one, the config row, the decl_common.h
//         declaration, and the src/ TUs that name it.
//
//         THE src/ NUMBER IS 28 / 28 / 0, and it is a CORRECTION of the
//         "28 mention it; 25 store its address" this comment used to carry
//         from the wave-3 review. Re-measured for the slot-7 seat two ways
//         over the same file set (src/**/*.c and *.cpp): first by classifying
//         each matching line, then again by stripping /*..*/ and // comments
//         from the whole file and matching complete `... = ...;` statements,
//         which is the one that settles it. Both passes agree:
//
//             src TUs mentioning the table    28
//             TUs with a real code store      28
//             store statements                28
//             SLOT writes (_ZTV8dScene_c[i] = )   0
//             comment/declaration-only TUs     0
//
//         There is no set of "three that only name it in a comment": every
//         one of the 28 has exactly one real store, and three of them were
//         read end to end (dScEntry_c_classInit_RESULT.c, dScMB_c_classInit.c,
//         dScGameOver_c_classInit.cpp) to confirm the classifier rather than
//         trust it. The conclusion the old number supported is unchanged and
//         is in fact three TUs stronger.
//       - hal/stage_bridges.cpp's own fill, hal_fill_stage_vtable, writes
//         _ZTV5Stage[0..19] (a 20-iteration st_trap loop, stage_bridges.cpp
//         :105). It does NOT touch _ZTV8dScene_c; that array is labelled
//         "transient ctor install, storage only" on its own line.
//       - ALL 28 storing src/ mentions write the table's ADDRESS into an object's
//         vptr. Not one of them stores into _ZTV8dScene_c[i]. Read out:
//           dScStage_c_classInit.c      *(int*)p = (int)_ZTV8dScene_c;
//           _ZN5StageD0Ev.c      thiz->vtable = (void **)_ZTV8dScene_c;
//           _ZN5StageD1Ev.c      thiz->vtable = (void **)_ZTV8dScene_c;
//           _ZN8dScene_cD0Ev.c      self->vtable = (void **)_ZTV8dScene_c;
//           _ZN8dScene_cD1Ev.c      self->vtable = &_ZTV8dScene_c;
//           _ZN9BootSceneD1Ev.c      self->vtable = &_ZTV8dScene_c;
//           _ZN9BootSceneD0Ev.c  self->vtable = (void **)_ZTV8dScene_c;
//           dScStarSel_c_classInit.cpp *(void***)p = (void**)_ZTV8dScene_c;
//         and twenty more of exactly that shape (dScBoot_c_classInit,
//         _ZN7dScMB_cD1Ev, _ZN7dScMB_cD0Ev, dScMB_c_classInit, five ov003, four
//         ov004, two ov005, one ov007, four ov075). So the storage has
//         exactly one writer, and after this change that writer is this file.
//
// (c) NOTHING DISPATCHES IT, and the reason is structural rather than lucky.
//     _ZTV8dScene_c is an INTERMEDIATE-BASE table in the Itanium ctor/dtor
//     chain, and every one of the writers above overwrites the vptr
//     with another table before the object is used -- there is no call
//     between the two stores in any of them:
//
//       dScStage_c_classInit   data_0208e4b8, then _ZTV8dScene_c, then _ZTV5Stage (wins)
//       _ZN8dScene_cD0Ev   _ZTV8dScene_c, then data_0208e4b8, then ~ActorBase
//       _ZN5SceneD1/D2  same pair
//       _ZN5StageD0/D2  _ZTV5Stage, three sub-object dtors, then _ZTV8dScene_c,
//                       then data_0208e4b8 -- the Scene store is the second
//                       to last statement and nothing reads it
//       dScStarSel_c_classInit data_0208e4b8, then _ZTV8dScene_c, then
//                       data_ov003_020b1704 (wins)
//       _ZN9BootSceneD0Ev same shape as the Scene dtors
//
//     And exactly THREE of the 28 are in the link today. Re-measured for the
//     slot-7 seat by looking each writer's own .obj up in walk_window.map,
//     not by recollection:
//
//       dScStage_c_classInit.c  data_0208e4b8, _ZTV8dScene_c, two |= bit sets on +0x13
//                        (no calls), then _ZTV5Stage -- and only then the
//                        three sub-object constructors. Last store wins.
//       _ZN8dScene_cD1Ev.c  _ZTV8dScene_c then data_0208e4b8, adjacent statements,
//                        then ActorBase::~ActorBase.
//       _ZN8dScene_cD0Ev.c  the same pair, then ~ActorBase and Memory::Deallocate.
//
//     _ZN9BootSceneD1Ev.c is NOT in the map (the second wave-3 commit sliced D1
//     and D0, not D2). The other twenty-four are not in walk_window.map at
//     all. So no live object has _ZTV8dScene_c as its vptr at any point a
//     virtual could be called, which is the same condition the CylinderClsn
//     seat above rests on.
//
// WHAT THE BODY DOES IF IT EVER IS DISPATCHED. Scene::BeforeInitResources
// calls Scene::ResetFadersAndSound and then Scene::Initialise3dGraphics, and
// the second half of that chain (Initialise3dGraphics, func_0205583c,
// func_020554bc, func_020556d0) is nothing but stores to DS I/O registers at
// 0x0400xxxx. Those addresses ARE mapped in the host -- port/ntr/ppu.cpp puts
// engine A's register file at 0x04000000 and hal/message_pump.cpp already
// writes *(vu32 *)0x4000000 directly -- so this is a poke of the modelled
// register file, not a wild store. It still never runs today, per (c).
//
// ABI. Slot 1's matched body is flat C, `Bool f(struct Scene *self)` with
// `self` on the stack; an MSVC vtable dispatch arrives __thiscall with `this`
// in ecx, so the slot takes the same ecx->arg adapter every other seat in
// this family uses, and this one forwards the return value because the face
// returns one (0 aborts the level init).
// ===========================================================================
//
// ===========================================================================
// SEATED (wave 4, lane w4-d): _ZTV8dScene_c slot 7, Scene::BeforeBehavior.
//
// This is the slot group 4 below called "the biggest single remaining win on
// this table", and it is the last one of the six that needed no other lane's
// file.
//
// THE ROM's OWN WORD, config/arm9/relocs.txt:
//
//     from:0x0209269c kind:load to:0x0202e3d4 module:main
//
// and config/arm9/symbols.txt puts a Scene virtual on both ends:
//
//     _ZTV8dScene_c                   kind:data(any)               addr:0x02092680
//     _ZN8dScene_c14BeforeBehaviorEv  kind:function(arm,size=0x1fc) addr:0x0202e3d4
//
// (0x0209269c - 0x02092680) / 4 = 7, so the ROM's own base and its own word
// name the index; nothing here is inferred from the ActorBase slot order. The
// order agrees with it anyway: slot 6 is ActorBase::Behavior (0x02043b24) and
// slot 8 is Scene::AfterBehavior (0x0202e3c8), which is the before/base/after
// triple every one of these tables carries.
//
// (a) STORAGE, (b) NO OTHER WRITER, (c) NO LIVE DISPATCH: all three are the
// same three the slot-1 block above re-proves, and all three were re-measured
// on this tree for this seat rather than carried -- see the corrected 28/28/0
// sweep and the three-in-the-link reading in (b) and (c).
//
// THE ABI LINES UP the same way the other four do. The matched body is
// `int _ZN8dScene_c14BeforeBehaviorEv(char *self)` inside an extern "C" block --
// receiver as an ordinary first argument, return value meaningful (0 stops the
// actor's behaviour for the frame) -- so the slot takes the ecx->arg adapter
// and forwards the result, exactly like scene_before_init.
//
// WHAT THE BODY WOULD DO IF IT EVER IS DISPATCHED, which is the question this
// seat has to answer more carefully than the other four did, because
// BeforeBehavior is a FRAME-LOOP face rather than a level-init one. It reads
// in three parts:
//
//   1. `if (!ActorBase::BeforeBehavior(self)) return 0;` -- already linked.
//   2. the PAUSE branch, behind `data_0209f1e0 != 0`. This is the half with
//      the hazards in it: a virtual dispatched through data_0209f5e8 by raw
//      slot index (`data_0209f5e8->w4(&data_0209f5e8, 0, 0)`, which is byte
//      0x10 of the fader table = SetForwardTime, spelled as a cdecl call into
//      what is a __thiscall body on the host), and
//      FaderBrightness::SetForwardTime over &data_0209f5d0, whose host vptr is
//      null because the port does not link the ROM's static initialiser for it
//      (hal/auto_bss.cpp carries that note at the storage).
//
//      IT CANNOT RUN, and the reason is a linked-set fact rather than a hope:
//      src/func_02023498.c is the ONLY writer in the whole of src/ that ever
//      sets data_0209f1e0 non-zero, and that TU is not in walk_window.map.
//      Nothing else writes the byte at all except func_02023544, which writes
//      zero. So the branch is dead twice over -- once because slot 7 is not
//      dispatched (c), once because its own guard is a permanent zero.
//   3. the FADER branch, behind `data_02092664 != 0x187`, and this half is
//      well bridged rather than merely dead. It dispatches data_0209f5bc
//      (hal/fader_wipes.cpp: `&hal_wipes[0]`, a real HalFaderWipe with a real
//      vptr) through a local 7-virtual shadow class, and the shadow's slots
//      line up with HalFaderWipe's ROM byte order face for face: v3 -> 0x0c
//      SetBackwardTime(int,int), v4 -> 0x10 SetForwardTime(int,int), v5 ->
//      0x14 IsAtStart, v6 -> 0x18 IsAtEnd. That is not luck either;
//      fader_wipes.cpp says in its own header that it holds ROM byte order
//      precisely so raw-offset callers like this one work, and the dummy
//      second int on its two Set*Time slots is there for this arity.
//
// THE DOUBLE-WORK QUESTION, asked because it is the one a frame-loop seat can
// get wrong. hal/stage_bridges.cpp:204 carries a hand stand-in for exactly one
// statement of part 3's sibling branch (`*(u8*)(stage+0x13) &= ~(1|4)`), and
// its own comment names the retirement condition: link BeforeBehavior, give
// the Stage its processing-list seat, and let slot 7 do it. Only the FIRST of
// those three is done here. The Stage is still not on the behaviour processing
// list, nothing dispatches slot 7, and the stand-in therefore still has to
// stay -- removing it now would clear the bits NOWHERE rather than twice.
// Measured, not assumed: the control selftest, the 1000-frame soak and the
// BiFS boot census all print byte-identical numbers to the pre-seat build.
// ===========================================================================

// ---- one linkage alias the chain needs -------------------------------------
//
// The C-LINKAGE FLIP, the same one hal/cxx_aliases.cpp exists for, one class
// further down this chain. src/Initialise3dGraphics.cpp calls the clear-colour
// register write as `extern "C" void _ZN3G3X13SetClearColorEtiiib(...)`, but
// the matched definition, src/_ZN3G3X13SetClearColorEtiiib.cpp, writes it as a
// real C++ static member:
//
//     struct G3X { static void SetClearColor(unsigned short, int, int, int, bool); };
//     void G3X::SetClearColor(...) { ... }
//
// so MSVC emits it decorated and the Itanium name is undefined. The decorated
// name is lifted verbatim from the object rather than written out by hand:
//
//     dumpbin /symbols _ZN3G3X13SetClearColorEtiiib.cpp.obj
//     009 00000000 SECT3 notype () External | ?SetClearColor@G3X@@SAXGHHH_N@Z
//         (public: static void __cdecl G3X::SetClearColor(unsigned short,int,int,int,bool))
//
// This is the exact shape hal/cxx_aliases.cpp already closes for IRQ::Disable
// and IRQ::Restore, and it runs in the same direction those two do -- the
// alias names an undefined symbol and points it at a defined one, nothing is
// redefined. It lives here rather than in cxx_aliases.cpp only because this
// lane owns this file and not that one; it belongs in cxx_aliases.cpp with the
// rest of the family whenever a change that owns that file passes through.
#include "port_d16.h"

#pragma comment(linker, \
    "/alternatename:__ZN3G3X13SetClearColorEtiiib=?SetClearColor@G3X@@SAXGHHH_N@Z")

extern "C" {

/* _ZTV12CylinderClsn -- storage `int data_0208e6ec[4]` in hal/cxx_aliases.cpp */
extern int data_0208e6ec[];

void *_ZN5dCc_cD1Ev(void *self);   /* arm9 0x020150a8 */
void *_ZN5dCc_cD0Ev(void *self);   /* arm9 0x0201507c */

/* _ZTV8dScene_c -- storage `void *_ZTV8dScene_c[20]` in hal/stage_bridges.cpp */
extern void *_ZTV8dScene_c[];

int _ZN8dScene_c19BeforeInitResourcesEv(void *self);   /* arm9 0x0202e638 */
int _ZN8dScene_c22BeforeCleanupResourcesEv(void *self);/* arm9 0x0202e5f0 */
int _ZN8dScene_c14BeforeBehaviorEv(void *self);        /* arm9 0x0202e3d4 */
int _ZN8dScene_c12BeforeRenderEv(void *self);          /* arm9 0x0202e3a4 */
void *_ZN8dScene_cD1Ev(void *self);                    /* arm9 0x0202e140 */
void *_ZN8dScene_cD0Ev(void *self);                    /* arm9 0x0202e170 */

/* The two ROM tail veneers. Their matched TUs take no arguments at all -- the
   ROM rides `this` and the VirtualFuncSuccess code through r0/r1 -- so the
   adapters below take the dispatch's receiver and argument and then drop both,
   which is sound only because the bodies at the end of the chain are `ret 4`.
   The whole reading is in hal/method_faces.cpp beside the two tagged
   definitions those veneers call. */
void _ZN8dScene_c13AfterBehaviorEj(void);              /* arm9 0x0202e3c8 */
void _ZN8dScene_c11AfterRenderEj(void);                /* arm9 0x0202e398 */

}

static void __fastcall cyl_d1(void *self, void *) { _ZN5dCc_cD1Ev(self); }
static void __fastcall cyl_d0(void *self, void *) { _ZN5dCc_cD0Ev(self); }

static int __fastcall scene_before_init(void *self, void *)
{
    return _ZN8dScene_c19BeforeInitResourcesEv(self);
}

static int __fastcall scene_before_cleanup(void *self, void *)
{
    return _ZN8dScene_c22BeforeCleanupResourcesEv(self);
}

static int __fastcall scene_before_behavior(void *self, void *)
{
    return _ZN8dScene_c14BeforeBehaviorEv(self);
}

/* slots 8 and 11: the dispatch arrives __thiscall with `this` in ecx and the
   VirtualFuncSuccess code on the stack. Both are named and then deliberately
   unused -- the veneer they call takes neither, and its callee is `ret 4`. */
static void __fastcall scene_after_behavior(void *self, void *, unsigned)
{
    (void)self;
    _ZN8dScene_c13AfterBehaviorEj();
}

static void __fastcall scene_after_render(void *self, void *, unsigned)
{
    (void)self;
    _ZN8dScene_c11AfterRenderEj();
}

static int __fastcall scene_before_render(void *self, void *)
{
    return _ZN8dScene_c12BeforeRenderEv(self);
}

static void *__fastcall scene_d1(void *self, void *) { return _ZN8dScene_cD1Ev(self); }
static void *__fastcall scene_d0(void *self, void *) { return _ZN8dScene_cD0Ev(self); }

extern "C" void hal_seat_w2_dtor_heads(void)
{
    data_0208e6ec[0] = (int)(size_t)cyl_d1;
    data_0208e6ec[1] = (int)(size_t)cyl_d0;
    _ZTV8dScene_c[1] = (void *)scene_before_init;
    _ZTV8dScene_c[4] = (void *)scene_before_cleanup;
    _ZTV8dScene_c[7] = (void *)scene_before_behavior;
    _ZTV8dScene_c[8] = (void *)scene_after_behavior;
    _ZTV8dScene_c[10] = (void *)scene_before_render;
    _ZTV8dScene_c[11] = (void *)scene_after_render;
    _ZTV8dScene_c[16] = (void *)PORT_D16(scene_d1);
    _ZTV8dScene_c[17] = (void *)scene_d0;
}

// ---- how this fill gets called ---------------------------------------------
//
// Every seat wave 1 landed is called by name from hal/level_boot.cpp, at the
// tail of the boot that installs the collision and model tables. This lane
// does not own that file, and the campaign's rule is that a lane never edits
// another lane's file, so the call edge is a namespace-scope object whose
// constructor runs the fill -- ordinary C++ static initialisation, entirely
// inside the one file this lane owns.
//
// THAT IS ONLY SAFE BECAUSE OF WHAT IS SEATED. A pre-main write would be
// wrong for any table a runtime fill also writes (hal/stage_bridges.cpp's
// trap fill would simply overwrite it, and the seat would silently do
// nothing). The table above is written by nothing else in the port, so
// "before main" and "at level boot" put the same two words in the same two
// slots, and the storage is zero-initialised before any dynamic initialiser
// runs, so there is no ordering question with its own definition either.
//
// The fill keeps external C linkage so a later change that does own
// level_boot.cpp can call it explicitly and drop the object below.
namespace {
struct W2SeatDtorHeads {
    W2SeatDtorHeads() { hal_seat_w2_dtor_heads(); }
};
W2SeatDtorHeads g_w2_seat_dtor_heads;
}

// ===========================================================================
// THE REST OF THE SWEEP, and why nothing else is seated here.
//
// The sweep read config/arm9 plus all 103 overlay configs and asked, for every
// vtable-shaped symbol the port names anywhere under hal/, unmatched/ and
// tests/: do the ROM's relocations at base+0 or base+4 name a matched TU the
// link does not carry? Thirty tables answered yes. One is seated above. The
// other twenty-nine fall into five groups.
//
// 1. THE mwcc THUNK ARTEFACTS -- four tables, eight bodies. NOT LINKABLE.
//
//      VTable_dBgPi_dBgCh_GndThunk[0,1]  _ZThn16_N9dBgCh_GndD1Ev / _ZThn16_N9dBgCh_GndD0Ev
//      data_020992b4[0,1]  _ZThn16_N9dBgCh_LinD1Ev / _ZThn16_N9dBgCh_LinD0Ev
//      data_02099348[0,1]  _ZThn16_N12dBgCh_SphCrrD1Ev / _ZThn16_N12dBgCh_SphCrrD0Ev
//      data_02099358[0,1]  _ZThn56_N12dBgCh_SphCrrD1Ev / _ZThn56_N12dBgCh_SphCrrD0Ev
//
//    These four are the SECONDARY-BASE sub-vtables of the collision classes
//    wave 1 seated: 0x02099274 is 0x10 past RaycastGround's head, 0x020992b4
//    0x10 past RaycastLine's, and src/_ZN12dBgCh_SphCrrD0Ev.cpp -- SphereClsn's matched
//    deleting destructor, already linked -- names all three of the third
//    class's vptrs in its own comment: [this+0] = 0x02099338, [this+0x10] =
//    0x02099348, [this+0x38] = 0x02099358. So the addresses are right and the
//    index convention is right.
//
//    The BODIES are the problem. Every one of the eight is a compiler-emitted
//    this-adjusting destructor thunk, and their matched sources are mwcc
//    ARTEFACT reconstructions: a synthetic `struct Derived : Base1, Base2`
//    whose only job is to make the compiler emit the thunk. Under MSVC none
//    of them defines a symbol called func_0203xxxx at all. Measured, not
//    assumed:
//      - src/_ZThn16_N9dBgCh_GndD0Ev.cpp, src/_ZThn16_N9dBgCh_LinD0Ev.cpp and src/_ZThn16_N12dBgCh_SphCrrD0Ev.cpp
//        are BYTE-IDENTICAL to each other (md5 cf4b564a...). Each compiles to
//        ??1Derived@@UAE@XZ, so any two of them together are LNK2005
//        ("already defined in _ZThn16_N9dBgCh_GndD0Ev.obj"), and each leaves
//        ??1Base1@@UAE@XZ / ??1Base2@@UAE@XZ permanently undefined.
//      - the four .c halves are C++ inside a .c (`virtual` in a struct) and do
//        not compile as C: _ZThn16_N9dBgCh_GndD1Ev.c gives C2061 on line 6.
//    This is the same class wave-1 lane l3 hit on the six _ZThn80_ Animation
//    thunks, one family over, and it ends the same way. The four tables keep
//    their zeros.
//
// 2. MSVC DESTRUCTOR FOLDING -- three slots, already ruled by lane l3 and
//    re-confirmed here against the live fills.
//      _ZTV11ShadowModel[1] <- _ZN11ShadowModelD0Ev: the folded slot 0 holds
//        the D1 thunk (hal/cxxname_bridge.cpp:387) that PowerStar's embedded
//        shadow is destroyed through, and the ROM's second dtor word has no
//        MSVC index to live at.
//      _ZTV11CommonModel[1] <- _ZN11CommonModelD0Ev: same shape --
//        hal/actor_classes_bob_world.cpp writes [0] = cm_d1 and [1] =
//        cm_dosetfile, so the deleting body has nowhere to go.
//      _ZTV9ModelBase[0] <- _ZN9ModelBaseD1Ev: one slot up, and l3 already put
//        the deleting body (the useful half) in the single folded slot.
//
// 3. TABLES A RUNTIME FILL OWNS. _ZTV5Stage's head is Stage::InitResources and
//    Stage::BeforeInitResources (0x020921c0 -> 0x0202cc0c, +4 -> 0x0202ddc8),
//    and nothing dispatches the Stage today -- but hal_fill_stage_vtable
//    trap-fills all twenty slots at boot, so a seat from this file would be
//    overwritten and the port's table would still not carry the ROM's words.
//    Stage::InitResources is the level-load spine besides (its closure names
//    Sound, VRAM banks, skybox, fog, the 2D graphics load and the whole
//    LVL_Overlay path), which is a lane of its own, not a head seat.
//
// 4. _ZTV8dScene_c -- TAKEN IN WAVES 3 AND 4, and this entry is now the record of
//    what is LEFT of it. Wave 2 blocked the whole table on one missing global:
//    slot 1's callee chain reaches Scene::ResetFadersAndSound, which writes
//    data_0209f1e4, and the port hosted that dsd BSS symbol nowhere.
//    hal/auto_bss.cpp hosts it now, and slots 1, 10, 16 and 17 are seated at
//    the top of this file with the chain behind slot 1 (func_0205583c,
//    Initialise3dGraphics, func_020554bc, func_020556d0, G3X::SetClearColor
//    and the geometry-engine helpers) in slice_w1l2.txt. Wave 4 added slot 7
//    (Scene::BeforeBehavior) with eight more TUs and five more hosted globals,
//    then slots 4, 8 and 11 once this lane also owned hal/method_faces.cpp.
//    Slot 7's own block is at the top of this file.
//
//    TWO SLOTS OF THIS TABLE REMAIN, and neither is blocked on a file any lane
//    in this campaign can own.
//
//    (a) DROPPED RECEIVER -- slot 2, and ONLY slot 2. This entry used to read
//        "slots 2, 8 and 11", and the reason it does not any more is a
//        measurement rather than a change of mind. All three matched bodies
//        are ARM tail-call veneers that rely on the receiver riding through in
//        r0:
//          src/_ZN8dScene_c18AfterInitResourcesEj.cpp
//            extern "C" void _ZN7dBase_c18AfterInitResourcesEj(void);
//            void _ZN8dScene_c18AfterInitResourcesEj(void) { <callee>(); }
//        and the same shape for Scene::AfterBehavior and Scene::AfterRender.
//        The callees exist in the map ONLY under their MSVC decorations
//        (?AfterInitResources@dBase_c@@UAEXI@Z,
//        ?AfterBehavior@fBase_c@@UAEXI@Z, ?AfterRender@fBase_c@@UAEXI@Z --
//        all UAE, virtual __thiscall, taking an argument), so what matters is
//        what each CALLEE does with the two values the veneer cannot deliver.
//        Read out of their own objects:
//
//          ?AfterBehavior@fBase_c@@UAEXI@Z    ret 4          (three bytes)
//          ?AfterRender@fBase_c@@UAEXI@Z      ret 4          (three bytes)
//          ?AfterInitResources@dBase_c@@UAEXI@Z
//              cmp   dword ptr [ebp+8],1        reads the ARGUMENT
//              mov   esi,ecx
//              call  ?MarkForDestruction@fBase_c@@QAEXXZ
//                                               reads the RECEIVER
//              push  dword ptr [ebp+8] / mov ecx,esi
//              call  ?AfterInitResources@fBase_c@@UAEXI@Z
//
//        Slots 8 and 11 are therefore SEATED: their callees read neither
//        value, so the drop cannot be wrong the way method_faces.cpp's failure
//        mode 2 means -- that mode needs a `this+0x5c` load and these have no
//        loads at all. Both are tagged host-ABI definitions in that file with
//        the listings beside them.
//
//        Slot 2 stays blocked and now for a reason with bytes behind it: its
//        callee would run MarkForDestruction on whatever ecx happened to hold.
//        That is the CRASH half of mode 2, not the silent half, and an
//        /alternatename would be no better -- the G3X::SetClearColor alias
//        above works because that one is SAX (static __cdecl) with all five
//        arguments passed. Unblocking slot 2 means handing the callee the r0/r1
//        pair through a side channel the slot adapter parks, which is a
//        mechanism with a live constraint, not three lines.
//
//    (b) TAKEN IN WAVE 4 -- slot 4. Scene::BeforeCleanupResources is ordinary C
//        and DOES pass its receiver
//        (`_ZN7fBase_c22BeforeCleanupResourcesEv(thiz)`); only the C-linkage
//        cdecl name it calls was undefined, since only
//        ?BeforeCleanupResources@fBase_c@@UAEHXZ existed. That is one face
//        beside the BeforeRender one it copies, and this lane owned the file.
//
//    (c) STORAGE THE MATCHED TU DEFINES ITSELF -- slot 5. MEASURED, not
//        predicted: the slot was seated, the TU sliced in, and the build was
//        run. It fails at the guard.
//
//          dsstate_guard: 1 hosted DS symbol(s) are OUTSIDE .dsstate
//          [0x94c000, 0x99b1dc) and would NOT be captured by a save state:
//            _data_02092660  @ 0x00826ee1
//              (_ZN8dScene_c21AfterCleanupResourcesEj.cpp.obj)
//
//        src/_ZN8dScene_c21AfterCleanupResourcesEj.cpp does not merely reference
//        data_02092660, it DEFINES it (`extern "C" { unsigned char
//        data_02092660; }` at namespace scope in a .cpp is a definition, not a
//        tentative one), so the global lands in ordinary .bss and a restore
//        would not roll it back. The guard's own advice is to fix it "in the
//        file that DEFINES each symbol", and that file is src/, read-only for
//        this lane. Hosting it in hal/auto_bss.cpp instead is not the way out
//        either -- that is a second definition of the same symbol. The seat
//        and the slice line were both reverted.
//
// 5. LIVE ACTOR AND ENGINE TABLES. The remaining eighteen are overlay actor
//    vtables (OneUpLogo, SkiLift, BabyPenguin, MrBlizzard, LakituBro,
//    UnchainedChomp, MetalNet, HUD, InvisiblePole, InvisibleSecret, the
//    Rickshaw/Cloud/painting tables), the Player vtable at
//    data_ov002_0210a83c, the object-loader table data_ov002_0210cbb8, and the
//    four particle callback tables. Every one of them is filled by the port
//    today and DISPATCHED, and in each case the unlinked matched body is an
//    InitResources, a loader or a SpawnParticles that the port currently
//    services with a host stand-in. Swapping one in is a behaviour change in
//    live code and belongs to the lane that owns that actor or subsystem, not
//    to a destructor-head seat. One of them is worth flagging:
//    data_ov002_0210cbb8[1] is LoadEntranceObjects, served by
//    unmatched/LoadEntranceObjects.cpp.
//
//    RETRACTED (wave 3): this note used to also flag "the four particle tables
//    put Particle::SimpleCallback::SpawnParticles at index 0 in the ROM where
//    hal/particle_vtable.cpp seats the BASE Particle::Callback::SpawnParticles."
//    THAT WAS WRONG and there is no discrepancy to chase. Lane w3-e re-derived
//    all thirteen particle tables against config/arm9/relocs.txt and found zero
//    mismatches; this lane re-derived them independently before retracting,
//    rather than taking the ruling on report. All 26 words agree with the ROM,
//    and the four tables in question -- _ZTVN5dPa_c7level_c16bubbleCallback_cE (Bubble), _ZTVN5dPa_c7level_c16simpleCallback_cE
//    (SimpleCallback), _ZTVN5dPa_c7level_c16splashCallback_cE (Splash) and _ZTVN5dPa_c7level_c24fitWaterSimpleCallback_cE
//    (FitWaterSimple) -- already seat SimpleCallback::SpawnParticles, because
//    the ROM's own slot-0 relocation on each of them lands on 0x02022640,
//    which config/arm9/symbols.txt names
//    _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE. SIX seat the base (f3b4 f3f4 f424 f434 f454 f464) and three seat their
//    own class's override (f3d4 EndingStarGlitter, f404 CheckLava, f414
//    Scale) -- w3-b recount. The six that
//    seat the base take 0x020226d0, which is Callback::SpawnParticles. The
//    sharing is the ROM's, exactly as particle_vtable.cpp's own header says.
//    The original claim came from reading the seats without resolving the
//    to: addresses.
//
// ONE CAVEAT ON THE OVERLAY HALF OF THE SWEEP. Overlays share address space,
// so an address resolved across all 101 configs can come back with a class
// name from the wrong overlay (0x02112408 answers both _ZTV12SwitchPillar in
// ov047 and _ZTV14daObjC0Water_c in ov012). Nothing in group 5 was taken, so
// no seat rests on that, but a future sweep should resolve overlay addresses
// inside one module rather than globally.
// ===========================================================================
