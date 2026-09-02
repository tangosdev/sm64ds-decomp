// Bridges for TUs that declare C-NAMED symbols at C++ linkage (an extern
// declaration outside the file's extern "C" block): the reference mangles
// as ?_ZN...@@YA..., so a same-shape C++ definition here forwards to the
// real implementation. The forward hop goes through a differently-named
// extern "C" helper because one TU cannot name both linkages of the same
// identifier.
#include <cstdio>
#include "vs_width.h"   /* run vs16: the port's player width */
#include <cstdlib>
#include <cstring>
#include "MeshColliderBase.h"
#include "dsstate_seg.h"

extern "C" {
void hal_fileptr_release(void *self);
}

void _ZN13SharedFilePtr7ReleaseEv(void *self)
{
    hal_fileptr_release(self);
}
int _ZN16MeshColliderBase9IsEnabledEv(void *self)
{
    return ((MeshColliderBase *)self)->MeshColliderBase::IsEnabled();
}
void _ZN16MeshColliderBase7DisableEv(void *self)
{
    ((MeshColliderBase *)self)->MeshColliderBase::Disable();
}

// The ov098 file table's second column is addressed through its own symbol
// (base+4 on the DS -- an offset alias). The harness used to fake it with a
// separate 0xc-stride array here; gate 19 mounts ov098 per symbol with
// --pack, which puts data_ov098_0213c384 exactly four bytes after
// data_ov098_0213c380 the way the ROM does, so the alias is real. The
// gate-9 smoke keeps the old two-view fiction in port/tests/smoke_actor.cpp.

// ---- gate-9 method bridges (C name -> MSVC method), the gx_upload pattern -
#include "Platform.h"
#include "ShadowModel.h"
#include "Model.h"
/* Model.h only forward-declares SharedFilePtr; the Model::LoadFile face below
   binds a reference to one, so it needs the complete (fieldless) declaration. */
#include "SharedFilePtr.h"
extern "C" {
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self)
{ ((Platform *)self)->Platform::UpdateClsnPosAndRot(); }
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self)
{ ((Platform *)self)->Platform::UpdateModelPosAndRotY(); }
/* SHADOW SYSTEM DEFERRED (cosmetic). The cause recorded here was WRONG and is
   corrected: the cuboid template BMD at data_020ad524 is NOT built at runtime.
   It is static .data in OVERLAY 1 (ov001 .data spans 0x020ab800..0x020ad620),
   a complete BMD_File with numBones 1, bones 0x020ad59c, numMaterials 1,
   materials 0x020ad4f4, and its pointers already absolute in the ROM image.
   Nothing writes it: the only two references to the address in all of config/
   are the literal-pool loads inside InitCuboid and InitCylinder themselves.
   The ov000 path strings this comment saw are the OTHER overlay sharing the
   0x020aa420 window, which is why the address reads as garbage today.

   The real blockers are three, all mechanical: (1) the port mounts ov002,
   ov009, ov085, ov098, ov100, ov089 and ov102 but never ov001, so the bytes
   are absent and these stubs stand in for them; (2) _ZTV11ShadowModel
   (actor_vtables.cpp) is eight nulls and hal_fill_shadow_vtable below is
   empty, so ModelBase::SetFile's vtable+0x8 dispatch would fault even once the
   data lands; (3) no per-frame ShadowModel::RenderAll caller is matched in
   src/ outside UnknownVsEntry::Render, so the host render path would have to
   call it.

   RUN LINKW WAVE 3 (lane w3-a): (1) AND (2) ARE NOW CLEARED FOR THE CUBOID,
   so this is no longer a stub -- it is the ordinary C-name-to-method bridge,
   and the body it reaches is the matched src/_ZN11ShadowModel10InitCuboidEv.cpp.

   (1) was half wrong even as written. ov001 HAS been mounted since gate 27;
   what was missing is that the mount named only the HUD's sprite templates, so
   --pack emitted 0x020ad478..0x020ad628 as a ZEROED gap and the template BMD
   read as 432 bytes of nothing. The bytes were present and zero, not absent.
   port/ov001_syms.txt now names the cuboid's nine symbols, reloc-followed out
   of the template rather than taken as a span.
   (2) was cleared by lane l3 in wave 1: hal/model_dtor_seat.cpp:157 puts
   ShadowModel::DoSetFile in _ZTV11ShadowModel[1], and level_boot.cpp:2119
   calls that seat inside port_stage_a2_seat, so it is live in walk_window and
   not only in the smokes (hal_fill_shadow_vtable below is still empty and is
   still only called from the four smoke mains -- it is NOT what unblocked
   this).
   (3) is still open and is the reason ShadowModel::RenderAll is not seated.
   It is ALSO no longer the whole story: the node that faults belongs to a
   BUTTERFLY, and a butterfly's two shadows are cylinders, not cuboids. See
   port/slice_w1l1.txt under LANE w3-a for the measured chain. */
void _ZN11ShadowModel10InitCuboidEv(void *self)
{ ((ShadowModel *)self)->ShadowModel::InitCuboid(); }
}   /* extern "C" -- reopened below, after the fallback's C++ linkage */

/* THE NARROW HARNESSES NEED A FALLBACK, same shape as hal_shadow_d1_fallback
   further down this file. Three targets compile this TU but do NOT carry
   SLICE_W1L1_SOURCES -- smoke_actor, smoke_savestate and smoke_persist -- so
   in those the matched InitCuboid does not exist and the bridge above is an
   unresolved external. walk_window, walk_window_hires and smoke_player carry
   the slice and link the real body, which WINS over an /alternatename.

   The fallback is a method on a differently-named class rather than a plain
   function, because the symbol being defaulted is __thiscall: a real C++
   method has that ABI exactly, and no hand-decorated @name@4 has to be
   guessed at. hal/actor_faces_bob.cpp uses the same construction for
   ShadowModel::InitCylinder.

   IT IS A NO-OP, AND THE FIRST VERSION OF IT WAS NOT. This was written as an
   abort() first, on the same reasoning hal_shadow_d1_fallback states -- that a
   narrow harness has no business installing a shadow, so reaching the fallback
   would be a bug worth stopping on. Running it disproved that: smoke_savestate
   and smoke_persist BOTH reach InitCuboid, and both aborted with exit 127 on
   the very first run. They do install a cuboid shadow.

   So the abort was wrong and the no-op is right, for a reason that is worth
   more than the guess it replaced. Those two targets have been running an
   InitCuboid that did nothing since gate 27 -- that is what the old
   `void _ZN11ShadowModel10InitCuboidEv(void *) {}` gave every target -- and
   they are green against exactly that behaviour. Returning quietly here
   reproduces their baseline bit for bit; aborting would have made this lane's
   change break two harnesses that never had a shadow system to begin with.
   The real fix is to give them SLICE_W1L1_SOURCES in port/CMakeLists.txt,
   which is not this lane's file. Until then this is a scoped no-op, not a
   claim that nothing reaches it. */
struct ShadowModelCuboidFallback { void InitCuboid(); };
void ShadowModelCuboidFallback::InitCuboid() {}
#pragma comment(linker, "/alternatename:?InitCuboid@ShadowModel@@QAEXXZ=?InitCuboid@ShadowModelCuboidFallback@@QAEXXZ")

extern "C" {
/* ACTOR::DROPSHADOWSCALEXYZ IS GONE FROM THIS FILE, and what stood here was
   `... (void *, void *, void *, int, int, int, unsigned) {}` -- an empty body,
   not a bridge. Nine linked callers reached it and got nothing: SignPost and
   ArrowSignRight's Behavior methods and seven ov functions, all of them in
   walk_window.map, all of them asking for a shadow every frame they ran.

   HOW OFTEN, counted rather than assumed, because a swap nothing calls is not
   a swap. A scratch build put a call counter in the fallback below, kept the
   slice line out so walk_window resolved onto it, and ran the 300-frame
   control: 1800 calls, six per frame, on the castle-grounds boot alone (the
   level spawns five SIGN_POST). Reverted; the counter is not in the tree.

   The matched TU takes over: src/_ZN5Actor18DropShadowScaleXYZER11ShadowModel
   R9Matrix4x35Fix12IiES5_S5_j.c, now listed in port/slice_w1l3.txt. It is
   plain C, it spells all seven arguments, and it is the structural twin of
   Actor::DropShadowRadHeight -- same `flags & 0x10` test at Actor+0xb0, same
   ShadowModel::InitModel call -- which has been linked and running every
   frame since before this wave (walk_window.map lists both the matched
   DropShadowRadHeight TU and its InitModel callee). There is no host body
   left to bridge into, so nothing here forwards; the C name IS the matched
   definition now.

   THE THREE NARROW HARNESSES STILL NEED A DEFAULT, and they said so rather
   than being guessed at. smoke_actor, smoke_savestate and smoke_persist
   compile this TU but do NOT carry SLICE_W1L3_SOURCES, and deleting the stub
   broke all three the same way:

     _ZN14ArrowSignRight8BehaviorEv.cpp.obj : error LNK2019: unresolved
     external symbol __ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x3
     5Fix12IiES5_S5_j referenced in function "public: int __thiscall
     ArrowSignRight::Behavior(void)"

   walk_window, walk_window_hires and smoke_player carry the slice and link
   the real body, which wins over an /alternatename. This is the same shape as
   hal_shadow_d1_fallback below and the InitCuboid fallback further down, with
   one difference: no shadow class is needed, because the symbol being
   defaulted is a plain cdecl C name and not a __thiscall method.

   AND IT IS A NO-OP, on w3-a's evidence rather than on principle. That lane
   wrote its InitCuboid fallback as an abort() first, reasoning that a narrow
   harness has no business installing a shadow, and running it disproved that
   outright. Here the argument is even shorter: an empty body is what EVERY
   target had before this change, so returning quietly is precisely the three
   harnesses' existing baseline, and an abort would break them on a body they
   have been calling all along. If they ever need the real shadow, the fix is
   SLICE_W1L3_SOURCES in port/CMakeLists.txt, which is not this lane's file. */
extern "C" void hal_dropshadow_scalexyz_fallback(void *, void *, void *,
                                                 int, int, int, unsigned) {}
#pragma comment(linker, "/alternatename:__ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j=_hal_dropshadow_scalexyz_fallback")
/* MODEL::LOADFILE IS A FACE AGAIN, and it was not one before. This definition
   used to be a hand-expanded copy of the matched Model::LoadFile -- LoadFile,
   read filePtr, check numRefs, UpdateFileOffsets, AddToCommonModelDataArr --
   written out so each load stage could carry its own PORT_TRACE_SETFILE line.

   port/tools/linkage.py counted it in the FACES bucket, and the docstring on
   that bucket says exactly why that reading could not be trusted: a map proves
   both definitions are linked, not that the host one forwards. This was the
   case the caveat is about. The matched TU is in the binary because other
   callers reach it by its MSVC name, while everything arriving on the ROM's C
   name got this copy instead.

   Worse, the copy was not equal. The matched body ends the numRefs == 1 branch
   with ptr.ReallocateModelFile(); this one dropped it, on the true observation
   that reallocation is a DS heap shrink with nothing to do on host. True but
   unnecessary: hal/gx_upload_bridge.cpp already hosts
   SharedFilePtr::ReallocateModelFile as an empty body, so the matched body's
   call lands on a real no-op rather than on nothing. Forwarding costs one call
   into a function that returns immediately, and buys back a body the port was
   restating.

   THE SPLIT THIS CLOSES. Model::LoadFile is spelled three ways in the tree and
   the map had them landing in two places: the ROM's C name and the two
   /alternatename decorations in hal/actor_faces_bob.cpp,
   hal/bob_enemy_bridges.cpp and hal/actor_classes_bowserpuzzle.cpp all reached
   THIS body, while any TU compiled against include/Model.h's `static void
   *LoadFile(SharedFilePtr &)` reached the matched one. Same ROM function, two
   host bodies, picked by how a caller spelled the return type. One hop here
   puts all three spellings on the matched body.

   The trace stays, at the granularity a forwarder can honestly offer: the
   handle going in and the buffer coming out. */
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp)
{
    int trace = getenv("PORT_TRACE_SETFILE") != 0;
    if (trace)
        fprintf(stderr, "  model_loadfile fp=%p id=%u refs=%u\n", fp,
                *(unsigned short *)fp, ((unsigned char *)fp)[2]);
    void *filePtr = Model::LoadFile(*(SharedFilePtr *)fp);
    if (trace)
        fprintf(stderr, "    model_loadfile -> buf=%p refs=%u\n", filePtr,
                ((unsigned char *)fp)[2]);
    return filePtr;
}

// BSS the shadow/collider systems use
DSSTATE_BEGIN
/* data_020ad524 USED TO BE A ZEROED char[0x40] HERE (run linkw wave 3, w3-a).
   It is the cuboid shadow's template BMD, and it is not bss and not the
   port's to invent: it is 0x3c bytes of ov001 .data, now mounted by name in
   port/ov001_syms.txt with its bone, its material and the four words they
   reach. The host array is gone so the reference below resolves onto the ROM
   bytes instead of shadowing them with zeros. Note the old array was also
   OVERSIZED (0x40 against the ROM's 0x3c), which would have put the bone's
   first word inside the template had anyone walked it. */
void *data_020a0c80[24];        /* the collision actor registry (gate 8) */
DSSTATE_END
}
/* The mount emits ov001's own spelling; the matched TU and every other caller
   spell the arm9 name. Both are data at the same address, so the alias is
   exact -- the same pattern hal/sub_actors.cpp uses for OAM::LIFE_ICONS and
   its four siblings. /alternatename only fires for an UNDEFINED symbol, which
   is why the host array above had to go rather than merely be renamed. */
#pragma comment(linker, "/alternatename:_data_020ad524=_data_ov001_020ad524")

#include "MeshCollider.h"
#include "ModelBase.h"
extern "C" {
void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp)
{ return MeshCollider::LoadFile(*(SharedFilePtr *)fp); }
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b)
{
    if (getenv("PORT_TRACE_SETFILE")) {
        extern void *_ZTV5Model[8];
        extern void *_ZTV10ModelAnim2[12];
        fprintf(stderr, "  setfile self=%p vt=%p slot1=%p bmd=%p b=%d"
                " (model_vt=%p ma2_vt=%p)\n",
                self, *(void **)self,
                self ? ((void ***)self)[0][1] : 0, bmd, b,
                (void *)_ZTV5Model, (void *)_ZTV10ModelAnim2);
    }
    ((ModelBase *)self)->ModelBase::SetFile((BMD_File *)bmd, a, b);
}
}
#pragma comment(linker, "/alternatename:?data_ov098_0213c380@@3PADA=_data_ov098_0213c380")
#pragma comment(linker, "/alternatename:?data_ov098_0213c384@@3PADA=_data_ov098_0213c384")

extern "C" void _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(
    void *self, void *kcl, void *clps)
{
    ((MeshCollider *)self)->MeshCollider::SetFile((KCL_File *)kcl,
                                                  *(CLPS_Block *)clps);
}

// operator new support: the game heap pointer for actors (the smoke seeds
// it with the root heap), and the zero-fill veneer -- its DS chain rides
// arguments through registers, so the host supplies the semantics direct.
//
// DELIBERATELY OUTSIDE .dsstate: this is the real storage behind the game
// heap word data_020a0eac (the two /alternatename lines below), so it IS a
// hosted DS global -- but a BOOT-CONSTANT one. The ROM writes it exactly once,
// in Heap::InitializeGameHeap off the main.c boot spine (func_0201a054), and
// the port seeds it once before the frame loop (walk_window.cpp / the smoke
// preambles). Nothing writes it during play, so it cannot diverge between a
// save and a load, and a snapshot of it would restore the value it already
// holds. Excluded so the capture stays exactly "state that can change";
// tools/dsstate_guard.py records the same exclusion by name. If a future level
// path ever re-initializes the game heap mid-session, move this inside a
// DSSTATE_BEGIN/END bracket and drop the guard exemption.
extern "C" {
void *data_020a0eac_c;
}
#pragma comment(linker, "/alternatename:?data_020a0eac@@3PAUHeap@@A=_data_020a0eac_c")
#pragma comment(linker, "/alternatename:_data_020a0eac=_data_020a0eac_c")
/* C linkage since main's mangled-declaration sweep: ActorBase::operator new
   now spells this plain, so the definition has to be the plain name. The
   alias below still catches any TU that kept the old C++ mangling.
   PORT_HOST_ABI: ARM r0/r1/r2 ride-through -- src veneer (int x) forwards p/v/n
   through registers to the actual fill; host spells the args out. */
extern "C" void func_0206e2f8(void *p, int v, unsigned n)
{
    unsigned char *b = (unsigned char *)p;
    for (unsigned i = 0; i < n; ++i) b[i] = (unsigned char)v;
}
#pragma comment(linker, "/alternatename:?func_0206e2f8@@YAXPAXHI@Z=_func_0206e2f8")
extern "C" void hal_m43_roty(void *m, int a);
void Matrix4x3_FromRotationY(void *m, int a) { hal_m43_roty(m, a); }

#include "MeshColliderBase.h"
extern "C" int _ZN16MeshColliderBase6EnableEP5Actor(void *self, void *actor)
{
    return ((MeshColliderBase *)self)->MeshColliderBase::Enable((Actor *)actor);
}
extern "C" {
DSSTATE_BEGIN
/* player-list globals ClosestPlayer scans: empty world -> null result */
int data_0208e37c[2];
int data_0208e380[2];
int data_0209b450[2];
int data_0209b458[2];
int data_0209f21c[8];
/* THE PER-SLOT Player* TABLE, and the one array in this list every VS path
   walks: the entrance loader writes it, the sync layer reads it by slot, the
   conductor censuses it. run vs16: sixteen seats. */
int data_0209f394[kPortMaxPlayers];
DSSTATE_END
}

/* Memory_Deallocate IS ARM9 0x0203c1e8, THE TWO-ARGUMENT OVERLOAD, and the
   one-argument binding this line used to carry was the level-change heap fault.

   THE CALLER THAT ACTUALLY LINKS, named precisely. `Memory_Deallocate` is the
   placeholder src/_ZN9ActorBase21AfterCleanupResourcesEj.c spells for the last
   statement of the actor teardown, but that src TU is not what the binary
   compiles. What links is hostgen's --extern-data rewrite of it, generated
   from GATE9_SYMS in port/CMakeLists.txt into
   build/host-src/src/_ZN9ActorBase21AfterCleanupResourcesEj.cpp, which keeps
   the call below verbatim and only rehomes the three role-named engine globals
   onto the HAL. (While lane w6-B ran, the caller was a third file --
   port/unmatched/ActorBase_AfterCleanupResources.cpp, the hand-written host
   copy -- and the lane's write-up named the src TU as though it were the one
   linking. That host copy is RETIRED as of lane w6-C item 3 and bannered as
   such. The src TU is ROM truth and the reference for the lines below; the
   hostgen output is the code that runs.) The src TU says which ROM function
   this is in its own comment:

       void Memory_Deallocate(void*, struct Heap*);   // 0x0203c1e8
       ...
       Memory_Deallocate(this, Memory_gameHeapPtr);

   0x0203c1e8 is Memory::Deallocate(void*, Heap*). 0x0203c1b4 is the OTHER
   overload, Memory::Deallocate(void*), which forwards with a NULL heap and so
   frees against Memory::defaultHeapPtr. This bridge was bound to that one, and
   because both sides are C linkage and __cdecl the teardown's second argument
   was accepted and discarded in silence -- no warning, no link error, no gate.
   Every actor in the game was freed against the wrong heap.

   WHAT THAT DOES, measured on the 38 -> 1 death exit (game heap 0x30000060,
   default 0x30000000):
     - ExpandingHeapAllocator::Deallocate splices the node out through its OWN
       prev/next, so the GAME heap's used list loses it correctly...
     - ...but FreeNode links the freed region into the DEFAULT heap's free list,
       so the game heap's free region never grows back. Six actors torn down at
       the arena exit and the free node stayed at [300000b0..30039414], byte for
       byte, all six times.
     - and when the freed node is the game list's head or tail, UnlinkNode
       writes the neighbour into the DEFAULT allocator's head/tail word instead,
       leaving the game heap's own tail pointing at a node that no longer
       exists. The next AllocateNode appends through that dangling tail, so the
       next level's Player never reaches the used list at all -- and the storage
       it occupies is, as far as both allocators are concerned, nobody's.
       SharedFilePtr::Load then hands a 328-byte file buffer out at the live
       player's address and the memcpy walks the inline ModelAnim's vptr at
       player+0xF0 to zero: FAULT in ModelBase::SetFile+0x6, which is the
       crash w5-E pinned register-for-register.
   The DS never sees any of this because on the DS the teardown always passed
   the game heap; the port lost the argument at this line.

   THE CALL IS BY C++ NAME, not by the Itanium spelling: the C-linkage
   __ZN6Memory10DeallocateEPvP4Heap only exists where hal/cxx_aliases.cpp is
   linked, and this TU rides three smokes that do not carry it. The matched TU
   is C++ (it defines ?Deallocate@Memory@@YAXPAXPAVHeap@@@Z), and slice_gate3a
   is in every target that compiles this file, so naming the method resolves
   everywhere with no new alias. */
class Heap;
namespace Memory { void Deallocate(void *ptr, Heap *heap); }
extern "C" void Memory_Deallocate(void *p, void *heap)
{ Memory::Deallocate(p, (Heap *)heap); }
extern "C" int hal_f0204424c(char *c);
int func_0204424c(int c) { return hal_f0204424c((char *)c); }

// gate 9: the Model vtable gains real slots at RUNTIME (targets without the
// Model methods keep the zero storage; nothing dispatches there). MSVC
// order per Model.h: dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
// Render 4. ModelBase::SetFile dispatches DoSetFile -- the first virtual
// the actor lifecycle exercises.
static int __fastcall mv_dosetfile(void *self, void *, char *f, int a, int b)
{
    if (getenv("PORT_TRACE_SETFILE"))
        fprintf(stderr, "  dosetfile self=%p f=%p a=%d b=%d\n", self, f, a, b);
    return ((Model *)self)->Model::DoSetFile(f, a, b);
}
/* ---- the actor bucket draws in the frame's own units --------------------
   THE SEAM IS GONE. The whole frame is SCENE units now (world >> 3): an
   actor's Render fills its model matrix with rotation rows at 1.0 and a
   scene-unit translation, Camera::Render's LookAt_ leaves the view matrix's
   translation row in scene units to match, and the port's own two draws --
   the stage model and the Player -- were moved onto the same convention.
   So processing list 5 needs no conversion at all: Model::Render gets the
   matrix the actor wrote and the scale the actor asked for.

   What used to be here: the model matrix's translation row and the view
   matrix's translation row were both scaled by 8 for the duration of one
   draw, and the scene->world factor was spent through the Vector3 scale
   argument. That bought a world-unit frame at the price of two hacks --
   this one, and putting the view matrix back before Actor::BeforeBehavior
   read it for the Clipper. Both retired together.

   SM64DS_TRACE_ACTOR_MAT=1 stays: it is how the collapsed-geometry
   questions get answered on one model rather than by argument. */
extern "C" int data_0209b3ec[12];       /* the view matrix Model::Render composes with */

/* SM64DS_ACTOR_BOX=1: the SCREEN box each Model::Render through this seam
   draws into, one line the first time a given BMD appears. This is where
   actor size is measured rather than argued about: a class's box against
   Mario's ("[w] mario screen box") at comparable depth is the proportion
   check, and a class that renders nothing at all prints no line. */
namespace ntr { struct GxTriangle; const GxTriangle *gx_polygons(size_t &n); }
#include "ntr/gx.h"
static void mv_box(const Model *m, size_t before)
{
    size_t after = 0;
    const ntr::GxTriangle *t = ntr::gx_polygons(after);
    static const void *said[32];
    static int n;
    const void *key = m->data.modelFile;
    for (int i = 0; i < n; ++i)
        if (said[i] == key) return;
    if (n < 32) said[n++] = key;
    if (after <= before) {
        printf("[abox] file %p shift %u: NO TRIANGLES\n", key,
               m->data.modelFile ? m->data.modelFile->scaleShift : 0u);
        return;
    }
    float mnx = 1e30f, mxx = -1e30f, mny = 1e30f, mxy = -1e30f;
    for (size_t i = before; i < after; ++i)
        for (int v = 0; v < 3; ++v) {
            if (t[i].v[v].x < mnx) mnx = t[i].v[v].x;
            if (t[i].v[v].x > mxx) mxx = t[i].v[v].x;
            if (t[i].v[v].y < mny) mny = t[i].v[v].y;
            if (t[i].v[v].y > mxy) mxy = t[i].v[v].y;
        }
    const int *q = (const int *)&m->mat4x3;
    printf("[abox] file %p shift %u scene(%d,%d,%d) %zu tris screen "
           "x[%.0f..%.0f] y[%.0f..%.0f] h=%.0f\n", key,
           m->data.modelFile ? m->data.modelFile->scaleShift : 0u,
           q[9] >> 12, q[10] >> 12, q[11] >> 12, after - before, mnx, mxx,
           mny, mxy, mxy - mny);
}

static void __fastcall mv_updateverts(void *self, void *)
{ ((Model *)self)->Model::UpdateVerts(); }
static void __fastcall mv_virtual10(void *self, void *, void *m)
{ ((Model *)self)->Model::Virtual10(*(Matrix4x3 *)m); }
/* Both of these sit in Model::Render's dispatch, which runs once per actor
   per frame -- eighty-odd times a frame at the castle. MSVC's getenv walks
   the whole environment block on every call, so they are latched once. */
static int trace_actor_mat()
{
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_TRACE_ACTOR_MAT") ? 1 : 0;
    return on;
}
static int actor_box()
{
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_ACTOR_BOX") ? 1 : 0;
    return on;
}

static void __fastcall mv_render(void *self, void *, const void *s)
{
    Model *m = (Model *)self;
    if (trace_actor_mat()) {
        const unsigned char *bf = (const unsigned char *)m->data.modelFile;
        const int *q = (const int *)&m->mat4x3;
        const int *bt = (const int *)m->data.transforms;
        fprintf(stderr, "  [amat] file %p shift %u bones %u scale %d | "
                "%d %d %d | %d %d %d | %d %d %d | %d %d %d\n",
                (const void *)bf, bf ? bf[0] : 0u,
                bf ? *(const unsigned *)(bf + 4) : 0u,
                s ? *(const int *)s : 0, q[0], q[1], q[2], q[3], q[4], q[5],
                q[6], q[7], q[8], q[9], q[10], q[11]);
        if (bt)
            fprintf(stderr, "  [abone] %d %d %d | %d %d %d | %d %d %d | "
                    "%d %d %d\n", bt[0], bt[1], bt[2], bt[3], bt[4], bt[5],
                    bt[6], bt[7], bt[8], bt[9], bt[10], bt[11]);
    }
    if (actor_box()) {
        size_t before = 0;
        ntr::gx_polygons(before);
        m->Model::Render((const Vector3 *)s);
        mv_box(m, before);
        return;
    }
    m->Model::Render((const Vector3 *)s);
}
extern "C" {
extern void *_ZTV5Model[8];
void hal_fill_model_vtable(void)
{
    _ZTV5Model[1] = (void *)mv_dosetfile;
    _ZTV5Model[2] = (void *)mv_updateverts;
    _ZTV5Model[3] = (void *)mv_virtual10;
    _ZTV5Model[4] = (void *)mv_render;
    /* Slot 5 too: TUs that dispatch through LOCAL shadow classes count in
       ROM/Itanium numbering (two dtor slots), which lands Render at 5.
       Model.h-compiled TUs land it at 4. The object serves both. */
    _ZTV5Model[5] = (void *)mv_render;
}
}

// gate 10: ModelAnim2's primary table (the Player's two body ModelAnims
// dispatch DoSetFile through it via ModelBase::SetFile). MSVC order: dtor 0,
// DoSetFile 1, UpdateVerts 2, Virtual10 3, Render 4, Virtual18 5. ROM slots
// carry the ModelAnim overrides for everything past DoSetFile. No dual-fill
// here: Render's ROM slot (5) is Virtual18's MSVC slot, so shadow-TU Render
// dispatch cannot be served by the same array -- trap-by-Virtual18 will name
// it if such a TU ever appears.
#include "ModelAnim.h"
static void __fastcall ma2_dtor(void *, void *) {}
static void __fastcall ma2_updateverts(void *self, void *)
{ ((ModelAnim *)self)->ModelAnim::UpdateVerts(); }
static void __fastcall ma2_virtual10(void *self, void *, void *m)
{ ((ModelAnim *)self)->ModelAnim::Virtual10(*(Matrix4x3 *)m); }
static void __fastcall ma2_render(void *self, void *, const void *s)
{ ((ModelAnim *)self)->ModelAnim::Render((const Vector3 *)s); }
static void __fastcall ma2_virtual18(void *self, void *, unsigned m, const void *s)
{ ((ModelAnim *)self)->ModelAnim::Virtual18(m, (const Vector3 *)s); }
extern "C" {
extern void *_ZTV10ModelAnim2[12];
extern void *VTable_Animation_ModelAnim2Thunk[12];
extern void *_ZTV9ModelAnim[10];
extern void *VTable_Animation_ModelAnimThunk[8];
void hal_fill_modelanim2_vtable(void)
{
    _ZTV10ModelAnim2[0] = (void *)ma2_dtor;
    _ZTV10ModelAnim2[1] = (void *)mv_dosetfile;
    _ZTV10ModelAnim2[2] = (void *)ma2_updateverts;
    _ZTV10ModelAnim2[3] = (void *)ma2_virtual10;
    _ZTV10ModelAnim2[4] = (void *)ma2_render;
    _ZTV10ModelAnim2[5] = (void *)ma2_virtual18;
    /* the Animation-base secondary table only ever destructs */
    VTable_Animation_ModelAnim2Thunk[0] = (void *)ma2_dtor;
    VTable_Animation_ModelAnim2Thunk[1] = (void *)ma2_dtor;
    /* plain ModelAnim (the Player's head models) shares every slot */
    _ZTV9ModelAnim[0] = (void *)ma2_dtor;
    _ZTV9ModelAnim[1] = (void *)mv_dosetfile;
    _ZTV9ModelAnim[2] = (void *)ma2_updateverts;
    _ZTV9ModelAnim[3] = (void *)ma2_virtual10;
    _ZTV9ModelAnim[4] = (void *)ma2_render;
    _ZTV9ModelAnim[5] = (void *)ma2_virtual18;
    VTable_Animation_ModelAnimThunk[0] = (void *)ma2_dtor;
    VTable_Animation_ModelAnimThunk[1] = (void *)ma2_dtor;
}
}

#include "ShadowModel.h"
extern "C" void _ZN11ShadowModelD1Ev(void *self);
/* Fallback so the narrow gate-8/9 harness (smoke_actor) still links: it slices
   ShadowModel's constructor but not its destructor (slice_gate9.txt carries
   _ZN11ShadowModelC1Ev only), yet it compiles this TU. The full targets
   (walk_window, walk_window_hires, smoke_player) link the real matched D1 from
   gate 10/16, which wins over this alternatename. The stub is never CALLED in
   smoke_actor -- that harness spawns gate-8/9 collision actors, none of which
   dispatch the shadow slot -- so it traps loudly if a target ever does reach it
   without the real dtor. THE SENTENCE THAT USED TO END THIS COMMENT IS DEAD:
   it called InitCuboid and DropShadowScaleXYZ "the same reasoning for the same
   reason", and neither is a stub any more. w3-a gave InitCuboid the matched
   body plus a no-op fallback, this wave gave DropShadowScaleXYZ the matched
   body outright, and this one -- an abort -- is now the only trap of its kind
   in the file. That difference is the point: an abort is right where reaching
   the fallback would be a bug, and w3-a measured that it is NOT right for a
   body the narrow harnesses have been calling all along. */
extern "C" void hal_shadow_d1_fallback(void *)
{ std::fprintf(stderr, "hal_shadow_d1_fallback: real _ZN11ShadowModelD1Ev not "
                       "linked in this target but the shadow slot was reached\n");
  std::abort(); }
#pragma comment(linker, "/alternatename:__ZN11ShadowModelD1Ev=_hal_shadow_d1_fallback")
/* combined dtor (Itanium D1) thunk, __fastcall to match the MSVC virtual ABI
   the rest of this file's fills use (self in ecx, ignored second arg) */
static void __fastcall shadow_dtor(void *self, void *)
{ _ZN11ShadowModelD1Ev(self); }
extern "C" {
extern void *_ZTV11ShadowModel[8];
void hal_fill_shadow_vtable(void)
{
    /* PowerStar state-5 lift-off destroys its drop shadow through slot 0
       (ROM 0x020EA284-0x020EA2A4: r0=actor+0x3d4, ldr vptr, ldr vtable[0], blx).
       Seat that single reachable slot; the double-unlink (this virtual call
       plus the later non-virtual _ZN11ShadowModelD1Ev in PowerStarD1Ev) is
       self-guarding in the matched source (ShadowModelD1Ev.c:33-51 checks
       prev/head and zeroes both). */
    _ZTV11ShadowModel[0] = (void *)shadow_dtor;
    /* vt[1] is DoSetFile under this tree's MSVC numbering (ROM slots 0+1 fold
       into MSVC 0, so ROM slot 2 = MSVC 1, the same fold _ZTV5Model[1] gets
       elsewhere in this file). hal/model_dtor_seat.cpp seats the matched
       ShadowModel::DoSetFile there. An earlier version of this comment called
       vt[1] "the D0 slot, null deliberately", which was ROM numbering leaking
       into an MSVC-numbered fill; the folded table gives the D0 no slot of
       its own. */
}
}

extern "C" {
void *_ZN5Model23AddToCommonModelDataArrER8BMD_File(void *file)
{ return Model::AddToCommonModelDataArr(*(BMD_File *)file); }
void *func_0203cc0c(unsigned size);
void _ZN6Memory10DeallocateEPv(void *p);

/* THE DS GLOBAL operator new. src/_Znwj.cpp is byte-matched and it is a
   THREE-INSTRUCTION ARM VENEER -- `ldr ip, [pc]; bx ip; .word 0x203cc0c` --
   transcribed exactly as `void _Znwj(void) { func_0203cc0c(); }`. It names no
   parameter because it MOVES none: the allocation size the caller left in r0
   is still in r0 when func_0203cc0c reads it. Compile that under 32-bit cdecl
   and the size is on the caller's stack, nothing pushes it a second time, and
   func_0203cc0c reads _Znwj's own return address as the byte count. No slice
   position fixes that, and no target could carry both definitions anyway --
   the matched TU and this one spell the same C symbol.

   HEAP IDENTITY IS NOT AT STAKE, which is the thing worth checking before
   touching operator new at all. The callee below is the ROM's own next hop,
   matched and linked: build/port/walk_window.map lists _func_0203cc0c against
   func_0203cc0c.c.obj, and that TU is `Heap::Allocate(data_020a0ea0, size)`.
   So this definition reaches the ROM's game-heap word through the ROM's own
   body. The argument is the only thing it adds.
   PORT_HOST_ABI: ARM r0 ride-through; src/_Znwj.cpp names no size. */
void *_Znwj(unsigned size) { return func_0203cc0c(size); }

/* THE DS GLOBAL operator delete2, the same shape one address down.
   src/_ZN6Memory16operator_delete2EPv.cpp is the veneer at 0x203cbcc
   (`ldr ip, [pc]; bx ip; .word 0x203cbf0`), transcribed `void
   _ZN6Memory16operator_delete2EPv(void) { _ZdlPv(); }` -- it hands _ZdlPv the
   pointer that is already in r0, and under cdecl it hands it nothing. The
   port has been ruling on this exact veneer shape since gate 24: the same
   finding is written out in slice_gate24.txt and slice_gate31.txt for
   func_0203cbc0, the OTHER ROM veneer onto _ZdlPv, which is hosted in
   unmatched/func_02073244_hostcopy.c for the same reason.

   THE ROUTE ENDS IN THE ROM'S HEAP, checked rather than assumed, and it is
   NOT the ROM's hop sequence. The ROM goes _ZdlPv -> defaultHeapPtr
   ->_Deallocate -> (a third veneer) -> Heap::Deallocate. This goes through
   the matched Memory::Deallocate(void*) -> Memory::Deallocate(void*, 0),
   which falls back to the same defaultHeapPtr and calls the same
   Heap::Deallocate, whose body is a single dispatch of vtable slot 4. Same
   heap pointer, same virtual, more hops.

   The extra hops are deliberate. _ZdlPv reaches walk_window and smoke_player
   only -- it is absent from the smoke_actor, smoke_savestate and smoke_persist
   maps -- while this definition has to resolve in all five targets that
   compile this file, and Memory::Deallocate's C name does resolve in all five.
   Pointing this at _ZdlPv would buy one hop of ROM shape at the price of an
   /alternatename fallback in three harnesses.
   PORT_HOST_ABI: ARM r0 ride-through; the src veneer names no pointer. */
void _ZN6Memory16operator_delete2EPv(void *p) { _ZN6Memory10DeallocateEPv(p); }
}
