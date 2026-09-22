// Lane INT4, run link100 wave 9c (the fold): the model family's twelve
// Destructor1 / Destructor0 rows.
//
// WHAT THESE NAMES ARE. include/ModelBase.h's own paragraph at line 110 says
// it: mwccarm puts a class's D1 and D0 in vtable slots 0 and 1, and MSVC puts
// one destructor entry there, so every slot from index 1 on would be one word
// early. The decomp headers therefore spell TWO ORDINARY VIRTUALS in those two
// places under the host-invented names Destructor1 and Destructor0, guarded by
// _MSC_VER so the ARM side never sees them and no ROM byte moves. The header
// also says "neither name is ever called; they hold the two slots the ROM's
// table holds."
//
// WHY THEY ARE ON THE WALL ANYWAY. Holding a slot still needs a definition. A
// constructor emits its class's MSVC vftable, and a vftable references every
// slot, so src/_ZN5ModelC1Ev.cpp and its siblings ask the linker for
// ?Destructor1@Model@@UAEXXZ and ?Destructor0@Model@@UAEXXZ. Six classes, two
// slots each, twelve rows of walk_window's unresolved wall, and the FACES4
// residue files put every one of them in the "no ROM name for that class and
// method" bucket, which is true of the NAME and not of the BODY.
//
// AN /alternatename IS NOT ADMISSIBLE HERE and that is why this is a face and
// not a row in cxx_aliases.cpp. The standing test (out/HALROWS/settled.txt
// section B) is that an alias is a name bridge and never an ABI bridge. These
// two sides do not agree about the call: ?Destructor1@Model@@UAEXXZ is
// __thiscall with the receiver in ecx, and _ZN5ModelD1Ev is the ROM body under
// C linkage taking the receiver as an ordinary first argument. A face moves it;
// an alias would leave it in the wrong place.
//
// WHAT EACH BODY IS. The ROM's own function for that slot, called with the
// receiver the host ABI put in ecx. Nothing invented, nothing stubbed: every
// one of the twelve is a real cartridge body with an address in
// config/arm9/symbols.txt, and every one is already DEFINED in this link, which
// is why none of the twelve flat names is on the wall.
//
//   slot 0, Destructor1              slot 1, Destructor0
//   _ZN9ModelBaseD1Ev      0x02017120   _ZN9ModelBaseD0Ev      0x020170e8
//   _ZN5ModelD1Ev          0x02016d20   _ZN5ModelD0Ev          0x02016ce0
//   _ZN11CommonModelD1Ev   0x020161e0   _ZN11CommonModelD0Ev   0x020161b4
//   _ZN10ModelAnim2D1Ev    0x02016364   _ZN10ModelAnim2D0Ev    0x02016320
//   _ZN14BlendModelAnimD1Ev 0x02016690  _ZN14BlendModelAnimD0Ev 0x02016644
//   _ZN11ShadowModelD1Ev   0x02015ff8   _ZN11ShadowModelD0Ev   0x02015f80
//
// THE TIDY VERSION, for whoever owns hal/model_host.cpp next: these twelve
// belong beside the rest of that family's host bodies. They are here because
// no lane owns that file this wave and the house law is that a lane stays
// inside its own files, so they go in additively through port/slice_int4.txt
// and the one CMake block that reads it.

#include "ModelBase.h"
#include "Model.h"
#include "CommonModel.h"
#include "ModelAnim2.h"
#include "BlendModelAnim.h"
#include "ShadowModel.h"

extern "C" {
void _ZN9ModelBaseD1Ev(void *self);
void _ZN9ModelBaseD0Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN5ModelD0Ev(void *self);
void _ZN11CommonModelD1Ev(void *self);
void _ZN11CommonModelD0Ev(void *self);
void _ZN10ModelAnim2D1Ev(void *self);
void _ZN10ModelAnim2D0Ev(void *self);
void _ZN14BlendModelAnimD1Ev(void *self);
void _ZN14BlendModelAnimD0Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN11ShadowModelD0Ev(void *self);
}

void ModelBase::Destructor1()      { _ZN9ModelBaseD1Ev(this); }
void ModelBase::Destructor0()      { _ZN9ModelBaseD0Ev(this); }
void Model::Destructor1()          { _ZN5ModelD1Ev(this); }
void Model::Destructor0()          { _ZN5ModelD0Ev(this); }
void CommonModel::Destructor1()    { _ZN11CommonModelD1Ev(this); }
void CommonModel::Destructor0()    { _ZN11CommonModelD0Ev(this); }
void ModelAnim2::Destructor1()     { _ZN10ModelAnim2D1Ev(this); }
void ModelAnim2::Destructor0()     { _ZN10ModelAnim2D0Ev(this); }
void BlendModelAnim::Destructor1() { _ZN14BlendModelAnimD1Ev(this); }
void BlendModelAnim::Destructor0() { _ZN14BlendModelAnimD0Ev(this); }
void ShadowModel::Destructor1()    { _ZN11ShadowModelD1Ev(this); }
void ShadowModel::Destructor0()    { _ZN11ShadowModelD0Ev(this); }

// =========================================================================
// FOUR MORE ROWS THE LEDGER COULD NOT SPELL, one hand face each
// =========================================================================
//
// Every one of these is a row facegen REFUSED for a reason about the TYPE
// SPELLING and not about the binding: a function-pointer parameter, a
// by-value template parameter, a class-spelled parameter, and a ROM free
// function whose body is a __thiscall member. In every case the member IS
// ALREADY DEFINED in this link and the flat ROM name is the one on the wall,
// so a hand face is the whole of the answer and nothing invents a body.
//
// THE ARITY OF EACH FACE WAS READ OFF ITS CALLER, never assumed. A face with
// the wrong arity is not a compile error, it smashes the stack on the first
// call, which is the standing hazard the port records against raw casts.
//
//   flat name                                        caller, and its own declaration
//   _ZN22ExpandingHeapAllocator13DeallocateAll...    port/hal/lk4_eh_dtor_seat.cpp:285
//       void *(void *thiz, void (*fn)(void*,void*,void*), void *ctx)   3 args
//   _ZN12dEnemyBase_c20KillByInvincibleChar...       include/decl_Enemy.h:24
//       void (void*, Vector3_16*, void*, int)                          4 args
//   _ZN8Particle10SysTracker8Contents6Create...      src/_ZN8Particle6System3New...c:8
//       void *(void*, unsigned, void*, const void*, void*)             5 args
//   func_ov006_020e39e0                              src/func_ov006_020e5450.c:57
//       void (char *c, int a, int b)                                   3 args
//
// THE SHADOW RULE APPLIES TO THE FIRST ONE and is why it binds the spelling it
// binds. port/hal/lk4_eh_dtor_seat.cpp declares its own private
// `struct ExpandingHeapAllocator` with `void DeallocateAll(Visitor *, u32)`,
// which decorates ?DeallocateAll@ExpandingHeapAllocator@@QAEXPAP6AXPAXPAV1@I@ZI@Z
// (V for class, and a pointer to the function pointer). That is a SHADOW. The
// owning translation unit emits
// ?DeallocateAll@ExpandingHeapAllocator@@QAEPAXP6AXPAXPAU1@I@ZI@Z, which is
// what out/HALROWS/settled.txt binds this row to and what this face calls.
// The shadow's own method keeps calling the flat name, so the chain is
// shadow method -> this face -> the real member, with no cycle: the two
// decorated names differ.

#include "ExpandingHeapAllocator.h"
#include "dEnemyBase_c.h"
#include "Particle__SysTracker.h"
#include "dScMgCurling2_c.h"

extern "C" void *_ZN22ExpandingHeapAllocator13DeallocateAllEPFvPvPS_jEj(
    void *thiz, void (*fn)(void *, void *, void *), void *ctx)
{
    return ((ExpandingHeapAllocator *)thiz)->ExpandingHeapAllocator::DeallocateAll(
        (ExpandingHeapAllocator::DeallocationFunction)fn, (u32)(size_t)ctx);
}

extern "C" void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(
    void *thiz, Vector3_16 *vel, void *player, int unused)
{
    /* Fix12<int> is an aggregate holding the raw 20.12 bits and has no
       converting constructor (include/math/Fix12.h). The parameter is the
       one the header names unused_, and the caller's declaration in
       include/decl_Enemy.h passes it as a plain int, so the raw bits go
       through unchanged. */
    Fix12<int> unused_bits;
    unused_bits.val = unused;
    ((dEnemyBase_c *)thiz)->dEnemyBase_c::KillByInvincibleChar(
        *vel, *(Player *)player, unused_bits);
}

extern "C" void *_ZN8Particle10SysTracker8Contents6CreateEjR7Vector3PK11Vector3_16fPN5dPa_c7level_c10callback_cE(
    void *contents, unsigned int definitionID, void *position,
    const void *direction, void *callback)
{
    return (void *)(size_t)((Particle::SysTracker::Contents *)contents)
        ->Particle::SysTracker::Contents::Create(
            definitionID, *(Vector3 *)position,
            (const Vector3_16f *)direction,
            (dPa_c::level_c::callback_c *)callback);
}

extern "C" void func_ov006_020e39e0(char *c, int a, int b)
{
    ((dScMgCurling2_c *)c)->dScMgCurling2_c::SpawnValue(a, b);
}

// =========================================================================
// THE TWO STRUCT-RETURN ROWS, which are mechanical once the order is read
// =========================================================================
//
// Both members return a Vector3 BY VALUE, and both ROM bodies are in this link
// already under their flat C names. facegen refused them because it will not
// emit an indirect return: the host compiler builds its own hidden return slot
// and the ROM body takes that slot as an explicit first argument, so the face
// has to move one and not the other. Nothing here is guessed -- the argument
// ORDER is read off each body's own definition, which is the only thing that
// could be got wrong:
//
//   src/_ZN8dActor_c25OnAimedAtWithEggReturnVecEv.cpp:45
//       extern "C" void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(Vector3 *ret,
//                                                                 dActor_c *self)
//   src/_ZN9dBgCh_Lin10GetClsnPosEv.cpp:15
//       extern "C" void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3 *res, dBgCh_Lin *self)
//
// Return slot first, receiver second, which is AAPCS indirect return with this
// displaced into r1, and which the first file's own header paragraph spells out
// from the ROM: r0 is written and never read, r1 supplies every field load.
// That file also explains why the ROM-side definition stays a free function
// rather than becoming a method -- mwcc does not apply the named return value
// optimisation there and the method spelling costs 0x10 bytes. That reasoning
// is about the ARM build and this face does not touch it: the definition below
// is host-only, the src file is unchanged, and no ROM byte moves.
//
// dActor_c::OnAimedAtWithEggReturnVec is slot 30 and 226 objects in this link
// reference it, every actor's vftable among them, so it is the single most
// referenced row left on the wall even though it is only one row.

#include "dActor_c.h"
#include "dBgCh_Lin.h"

extern "C" {
void _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(Vector3 *ret, void *self);
void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3 *ret, void *self);
}

Vector3 dActor_c::OnAimedAtWithEggReturnVec()
{
    Vector3 out;
    _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(&out, this);
    return out;
}

Vector3 dBgCh_Lin::GetClsnPos()
{
    Vector3 out;
    _ZN9dBgCh_Lin10GetClsnPosEv(&out, this);
    return out;
}

// =========================================================================
// TWO RENAMED DESTRUCTOR SPELLINGS, the same shape as FACES4's twenty-three
// =========================================================================
//
// port/hal/actor_classes_ov_link100.cpp reaches two matched C++ destructors
// through private shadow classes, because the real headers declare those two
// classes WITHOUT the virtual destructor and both spellings cannot coexist in
// one translation unit. The shadows are named for the bodies dsd named, and the
// main to port sync landed the decomp's own class names on the definitions, so
// each destructor is DEFINED under one name and ASKED FOR under another. Two
// rows, and both are a pure name bridge.
//
//   ??1CameraTag@@UAE@XZ  ->  ??1daChRoom_c@@UAE@XZ
//   ??1Cloud@@UAE@XZ      ->  ??1daObjKumo_c@@UAE@XZ
//
// AN /alternatename IS ADMISSIBLE, by the standing test: an alias is a name
// bridge and never an ABI bridge, so the two sides have to agree about the call
// already. They do exactly. Both spellings are ??1X@@UAE@XZ, public virtual
// __thiscall taking nothing and returning nothing, so there is no receiver to
// misplace and no stack to unbalance. This is the test FACES4 applied to its
// twenty-three renamed adapters.
//
// THE IDENTITY OF EACH PAIR IS MEASURED, not read off the name.
//   Cloud is daObjKumo_c BY ADDRESS. dsd gave one vtable address, ov039
//   0x02111858, two names, and this tree already carries the bridge in
//   port/hal/cxx_aliases.cpp:2451,
//   /alternatename:__ZTV11daObjKumo_c=__ZTV5Cloud. A shared vtable object is
//   one class.
//   CameraTag is daChRoom_c BY ITS OWN CALL SITES. The shadow is used in
//   actor_classes_ov_link100.cpp in exactly three places, chroom_init,
//   chroom_behavior and chroom_d1 at lines 340, 344 and 350, all three inside
//   the CLASS A block whose banner reads "ov002 0x021085f8, actor id 348, RTTI
//   10daChRoom_c", and whose other five slot fills call _ZN10daChRoom_c*
//   directly. That banner also says why: id 348's seven own bodies are the
//   files dsd named _ZN9CameraTag*, and they are that class's because the
//   table's own slots point at them.
//
// A CORRECTION FOR THE RECORD. out/DTORS2/residue.txt reads these two rows as
// "the ROM classes behind the two host names are daCamTag_c and daObjKumo_c".
// The second is right and the first is not: daCamTag_c is actor id 333 at RTTI
// 0x0210853c, a different class in the same family listing, and nothing in this
// file's CameraTag shadow reaches it. Binding the row that way would have put a
// room-change trigger's destructor under a camera tag's name.
//
// THE TIDY VERSION: rename the two shadows in that file to the classes they
// are, and delete this section.
// RETIRED at SMOKELINK (wave 10). BOTH ROWS ARE DEAD, and dead in the way
// alternatename_guard refuses: each LHS is a DEFINED symbol, so the alias is
// inert and references bind to the definition rather than to the RHS. Neither
// RHS is in the map at all. walk_window.map, this tip:
//   0001:000082b0  ??1CameraTag@@UAE@XZ  actorport_dtor_faces.cpp.obj
//   0001:000082c0  ??1Cloud@@UAE@XZ      actorport_dtor_faces.cpp.obj
//   ??1daChRoom_c@@UAE@XZ, ??1daObjKumo_c@@UAE@XZ  -- absent
// hal/actorport_dtor_faces.cpp took both host names over during the 09-14
// main-to-port sync, so the routing these two rows were written for already
// happens one file over and the pragmas only defeat themselves.
// #pragma comment(linker, "/alternatename:??1CameraTag@@UAE@XZ=??1daChRoom_c@@UAE@XZ")
// #pragma comment(linker, "/alternatename:??1Cloud@@UAE@XZ=??1daObjKumo_c@@UAE@XZ")

// =========================================================================
// FIVE ROWS A SECOND-SPELLING SWEEP FOUND, after the wall was down to 27
// =========================================================================
//
// These were found by asking one question of every row still on the wall: is
// this same function DEFINED IN THIS LINK UNDER ANOTHER SPELLING? The sweep ran
// each unresolved name's identifier tokens against a freshly built universe of
// walk_window's own link inputs (49917 defined externals over 8678 objects,
// rebuilt on this tip because the round-2 one predated ten commits). Four rows
// came back with an exact-signature twin and one with a static sibling.
//
// FOUR ARE A PURE NAME BRIDGE and take an /alternatename by the standing test:
// the two sides already agree about the call, same calling convention, same
// parameter list, same receiver.
//
//   ?_ZN8dActor_c15FindWithActorIDEjPS_@dActor_c@@SAPAU1@IPAU1@@Z
//       A caller declared the FLAT ROM name as a static member of dActor_c, so
//       MSVC mangled the Itanium string as a method name. The real member
//       ?FindWithActorID@dActor_c@@SAPAU1@IPAU1@@Z is defined in this link and
//       the two share a parameter list character for character: SAPAU1@IPAU1@@Z
//       on both sides, static and __cdecl on both sides.
//       Lane FACES4 refused this row as a seat, on the measurement that the
//       flat ROM body is not in the link. That is still true --
//       __ZN8dActor_c15FindWithActorIDEjPS_ is not among the 49917 -- and it is
//       not what the row needs: the DECORATED member is there.
//       (port/hal/actor_classes_ov060.cpp:266 carries a retired alias whose note
//       says that flat name is "a real definition in this link now". The
//       universe says otherwise, so that note is stale; it is retired either way
//       and nothing depends on it.)
//
//   ?SpawnMegaCharParticles@Enemy@@QAEXAAUActor@@PAD@Z
//       port/hal/bob_enemy_bridges.cpp:92 declares a private shadow
//       struct Enemy with this member and calls it. Enemy and Actor are the
//       pre-sync names of dEnemyBase_c and dActor_c, and
//       ?SpawnMegaCharParticles@dEnemyBase_c@@QAEXAAUdActor_c@@PAD@Z is defined
//       in this link with the same __thiscall shape, one reference parameter
//       and one char pointer.
//
//   ?DisappearPoofDustAt@ArrowSignRight@@QAEXABUVector3@@@Z
//   ?MarkForDestruction@ArrowSignRight@@QAEXXZ
//       include/ArrowSignRight.h:74 asks for these two by name: "INHERITED, AND
//       THEREFORE THE PORT'S TO BIND ... they resolve to
//       _ZN8dActor_c19DisappearPoofDustAtERK7Vector3 and
//       _ZN7fBase_c19MarkForDestructionEv, which is an /alternatename the port
//       can write". The right hand sides here are the DECORATED members and not
//       those flat names, on purpose: the flat bodies are __cdecl with the
//       receiver as a stack argument while these left hand sides are __thiscall
//       with the receiver in ecx, which is the ABI bridge an alias must never
//       be, and _ZN7fBase_c19MarkForDestructionEv is not in this link at all.
//       ?DisappearPoofDustAt@dActor_c@@QAEXABUVector3@@@Z and
//       ?MarkForDestruction@fBase_c@@QAEXXZ are both defined, both __thiscall,
//       both with the same parameter list as the left hand side.
//       THE RECEIVER IS THE SAME POINTER, which is what makes the bridge sound:
//       ArrowSignRight derives from dBgActor_c from dActor_c from fBase_c, all
//       single inheritance, and the header's own static_asserts pin its fields
//       at the base layout's offsets (actorID 0x00c, mPosX 0x05c, mAngleY
//       0x08e, mModel 0x0d4), so every base subobject is at offset 0.
#pragma comment(linker, "/alternatename:?_ZN8dActor_c15FindWithActorIDEjPS_@dActor_c@@SAPAU1@IPAU1@@Z=?FindWithActorID@dActor_c@@SAPAU1@IPAU1@@Z")
#pragma comment(linker, "/alternatename:?SpawnMegaCharParticles@Enemy@@QAEXAAUActor@@PAD@Z=?SpawnMegaCharParticles@dEnemyBase_c@@QAEXAAUdActor_c@@PAD@Z")
#pragma comment(linker, "/alternatename:?DisappearPoofDustAt@ArrowSignRight@@QAEXABUVector3@@@Z=?DisappearPoofDustAt@dActor_c@@QAEXABUVector3@@@Z")
#pragma comment(linker, "/alternatename:?MarkForDestruction@ArrowSignRight@@QAEXXZ=?MarkForDestruction@fBase_c@@QAEXXZ")

// THE FIFTH IS A FACE, because the two sides do NOT agree about the call.
// port/hal/cxx_aliases.cpp:689 declares a private shadow
// `struct Scene { void ResetHardwareRegisters(); };` and defines the flat ROM
// name against it, so the link asks for ?ResetHardwareRegisters@Scene@@QAEXXZ,
// a __thiscall member. What exists is ?ResetHardwareRegisters@dScene_c@@SAXXZ,
// which include/dScene_c.h:69 declares STATIC. An alias between a member and a
// static is an ABI bridge even when, as here, the zero-argument case happens to
// make the two epilogues identical, so the shadow member is defined here
// instead and forwards to the static. The shadow is repeated verbatim, because
// a mangle depends on the name, the class, the convention and the signature and
// on nothing else, and this one has no members that could disagree.

#include "dScene_c.h"

struct Scene { void ResetHardwareRegisters(); };
void Scene::ResetHardwareRegisters() { dScene_c::ResetHardwareRegisters(); }

// =========================================================================
// ONE PARTICLE CALLBACK ROW, and the two beside it that stay refused
// =========================================================================
//
// port/hal/particle_bridges.cpp wrote the top half of three face pairs: a
// vtable-slot thunk under the ROM's flat name that calls a private shadow
// class's member. The bottom half was never written and the three shadow
// spellings are on the wall. The sync had renamed those classes, so the owning
// translation units emit them under the decomp's own names.
//
// ONE OF THE THREE IS A PURE NAME BRIDGE.
//   ?OnUpdate@CleanParticleCallback@Particle@@QAEHAAUSystem@2@_N@Z
//       ->  ?OnUpdate@cleanParticleCallback_c@level_c@dPa_c@@UAEHAAUSystem@Particle@@_N@Z
//   particle_bridges.cpp:34 declares `int OnUpdate(System &, bool)` and the
//   owning TU emits `int` too. The only difference in the two mangles is Q
//   against U, which is non-virtual against virtual, and that letter is not part
//   of the call: both are __thiscall, both take a reference and a bool, both
//   return int. So the two sides already agree about the call and an alias is a
//   name bridge, which is the standing test.
//
// THE OTHER TWO STAY REFUSED, and this is the shadow rule doing its job.
//   ?OnUpdate@CheckWaterRippleCallback@Particle@@QAE_NAAUSystem@2@_N@Z
//       particle_bridges.cpp:38 declares this one returning BOOL, and the
//       owning TU emits it returning INT
//       (?OnUpdate@checkWaterRippleCallback_c@level_c@dPa_c@@UAEHAAUSystem@Particle@@_N@Z).
//       That is a return-type disagreement and not a spelling one: a
//       bool-returning callee writes only AL, and the bridge at line 74 reads
//       the result through `? 1 : 0`, so a ROM value whose low byte happens to
//       be zero would come back false. An alias here would be an ABI bridge.
//       The fix is one word in that file, bool to int, with the ternary dropped,
//       and that file is not this lane's.
//   ?SpawnParticles@CheckLavaCallback@Particle@@QAEXAAUSystem@2@@Z
//       No checkLavaCallback_c exists in this link under any spelling, so there
//       is nothing to bridge to. This one is a body and not a name.
#pragma comment(linker, "/alternatename:?OnUpdate@CleanParticleCallback@Particle@@QAEHAAUSystem@2@_N@Z=?OnUpdate@cleanParticleCallback_c@level_c@dPa_c@@UAEHAAUSystem@Particle@@_N@Z")
