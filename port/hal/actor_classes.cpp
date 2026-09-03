// The vtables of the actor classes the registry carries, filled at runtime.
//
// THE VTABLE LAW, as gate 9 wrote it (hal/actor_vtables.cpp) and gate 13
// repeated for the Camera (hal/camera_bridges.cpp):
//
//   * MSVC slot order, which for these classes is the ROM's -- include/
//     ActorBase.h declares the destructor LAST on purpose, so slots 0..15
//     agree and only the tail can diverge. Every slot below was read out of
//     the class's own _ZTV in the overlay image with its relocations applied,
//     not inferred from the header.
//   * Every entry is a __fastcall thunk: ecx carries `this` exactly as
//     __thiscall does and the dummy edx absorbs fastcall's second register.
//     That is the convention the four Process wrappers in port/unmatched/
//     call through.
//   * Every thunk calls QUALIFIED, never virtual.
//   * Slots the port cannot service TRAP BY NAME rather than being left null
//     or pointing into the overlay image.
//
// Filled at runtime rather than written as a static array because the ROM
// factories install the vtable themselves (`p[0] = (int)_ZTV4Tree`), so the
// symbol has to be an array of the right size in the first place -- and
// because InitResources can dispatch through the vptr the constructor just
// installed, which means the table must be up before the first spawn.
//
// ---- THE TABLES ARE NOT TWENTY SLOTS LONG ----------------------------------
//
// Every array in this file was written [20], the plain ActorBase width, and
// every ROM table it stands in for is 31 or 32 words. Slot 21 is
// OnGroundPounded, at byte offset 0x54; a [20] array is 0x50 bytes; so a
// player ground-pounding an ordinary sign dispatched whatever the linker put
// next in memory. That is a live crash on castle-grounds furniture, not a
// latent hazard, and it fires on the objects a player stands on most.
//
// The width is not a style choice, it is the class's own base chain, and
// port/tools/vtspan.py reads it out of the ROM's relocations:
//
//   31 slots  a plain Actor subclass. Slot 30 is Actor::
//             OnAimedAtWithEggReturnVec (arm9 0x020100dc) and the table ends.
//   32 slots  a Platform subclass. Slot 31 is Platform::Kill (ov002
//             0x020ee55c) or the class's own override of it.
//
// The next-symbol delta is NOT the width: dsd emitted ambiguous data symbols
// inside several of these tables, so the symbol bound truncates (_ZTV4Bird
// reads as 10 words and is 31, _ZTV8MetalNet reads as 12 and is 32). Every
// size below is the reloc run intersected with that bound, per vtspan.
//
// ---- the shared half -------------------------------------------------------
//
// Ten of the first twenty slots are the same functions in every one of these
// classes: Actor's four Before/After pairs, ActorBase::OnHeapCreated, and
// Actor::OnYoshiTryEat. So is the whole 20..30 tail, which is Actor's own
// interaction list -- Virtual50 plus the nine combat hooks plus the two egg
// virtuals -- and every one of those bodies is already in the build. They are
// written once here.
//
// SLOT 30 (OnAimedAtWithEggReturnVec) DECLINES rather than forwarding. Its ROM
// body returns a Vector3 BY VALUE, so the host call needs an sret contract
// this file has never proved, and the body is not in a slice; a named decline
// is the honest seat. Same reading as hal/actor_classes_wf.cpp.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "dtor_faces_cpp.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);      /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2 */
int _ZN5Actor14BeforeBehaviorEv(void *self);           /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);             /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);            /* slot 18 */
/* slot 19. The ROM body is a tail-call veneer to KillAndTrackInDeathTable and
   MSVC emits it as a bare `jmp` too, so the caller's pushed `self` is what the
   veneer's target reads -- the two-argument call here is what makes that word
   the right one. Same shape hal/actor_classes_painting.cpp uses. */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);  /* slot 19 */
/* Actor's own interaction list, slots 20..29, all matched arm9 bodies already
   linked (slice_gate32 and slice_gate50). Read out of the ROM tables, not the
   header: every one of them appears at these indices in _ZTV4Tree and its
   twenty-four neighbours. */
int  _ZN5Actor9Virtual50Ev(void *self);                      /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);              /* slot 29 */
/* slot 31 of a Platform table, ov002 0x020ee55c, already in the build */
void _ZN8Platform4KillEv(void *self);
}

static int __fastcall ac_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ac_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ac_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
/* Slots 5, 8 and 11 are ARM tail-call veneers on the ROM -- two instructions
   that drop into ActorBase's implementation with the argument still riding in
   r1. A host forward through the veneer's own C face would lose it, so the
   thunk calls the target directly, the same reading the Player's AfterBehavior
   slot already takes. */
static void __fastcall ac_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ac_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ac_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
/* SM64DS_ACTOR_PROBE=2: why an actor did or did not reach its own Render.
   Actor::BeforeRender is the gate -- it refuses on WRONG_AREA (0x20) and on
   OFF_SCREEN (0x8) when the actor asked to be culled off screen (0x2) -- and
   both bits are set by Actor::BeforeBehavior out of the SpawnInfo's own
   radius and cull distance. Printed once per actor id. */
static int __fastcall ac_bren(void *s, void *)
{
    int r = _ZN5Actor12BeforeRenderEv(s);
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_ACTOR_PROBE");
        on = e && e[0] == '2';
    }
    if (on) {
        static unsigned said[64];
        unsigned id = *(unsigned short *)((char *)s + 0xc);
        int seen = 0;
        for (int i = 0; i < 64 && said[i]; ++i)
            if (said[i] == id) seen = 1;
        if (!seen) {
            for (int i = 0; i < 64; ++i)
                if (!said[i]) { said[i] = id; break; }
            const char *c = (const char *)s;
            std::printf("[bren] id %3u -> %d  flags %08x area %d radius %d "
                        "cull %d view (%d,%d,%d)\n", id, r,
                        *(const unsigned *)(c + 0xb0),
                        *(const signed char *)(c + 0xcc),
                        *(const int *)(c + 0xb8) >> 12,
                        *(const int *)(c + 0xbc) >> 12,
                        *(const int *)(c + 0x74) >> 12,
                        *(const int *)(c + 0x78) >> 12,
                        *(const int *)(c + 0x7c) >> 12);
        }
    }
    return r;
}
static void __fastcall ac_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ac_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ac_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static void __fastcall ac_pdes_base(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); }

/* Actor's own interaction tail, slots 20..29. Every one of these takes its
   argument PUSHED by the __thiscall caller, so the thunk needs the dummy edx
   AND the named parameter or it pops nothing and the caller's frame runs
   short -- the same three-parameter shape hal/actor_classes_wf.cpp uses. */
static int __fastcall ac_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ac_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ac_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ac_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ac_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ac_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ac_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ac_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ac_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ac_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 19, OnTurnIntoEgg(Player &player). Three parameters, like the rest of
   this group: the caller PUSHES the player, so the thunk has to pop it. */
static int __fastcall ac_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
/* slot 31, the Platform tail. Only the Platform subclasses below write it. */
static int __fastcall ac_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The trap. Slots 13/14 are the actor's own solid-heap creation and 16/17 are
   the destructors where a class leaves them unhosted; a named abort is the
   honest placeholder for a slot the port has not proved. One report serves
   every trapped slot, and it says WHICH slot fired on WHICH actor in WHICH
   phase -- the old per-class form printed "slot 13" from whatever slot it sat
   in, and a woken Bird's destructor spent a session disguised as a slot-13
   dispatch.

   SLOT 19 IS NO LONGER ONE OF THEM. It used to trap here on the reading that
   OnTurnIntoEgg was unproved, and that reading is out of date: gate 50 linked
   Actor::OnTurnIntoEgg (arm9 0x02010154) when the PAINTING became the first
   hosted class to inherit the Actor default, and every class in this file
   inherits the same word. The ROM's own tables say so -- for all
   twenty-three plain-Actor classes here the reloc at vtable+0x4c lands on
   0x02010154, e.g. _ZTV6Rabbit (ov085 0x021300f8) slot 19 -> 0x02010154 -- and
   the one class that overrides it (ONEUP_MUSHROOM, func_ov002_020af2b0)
   already writes its own body over the shared fill.

   The cost of the trap was not a diagnostic: a trapped slot RAISES, the
   quarantine net catches it, and the actor is frozen for the rest of the level
   (unmatched/func_02043fdc_hostcopy.cpp). Actor::OnTurnIntoEgg is a tail-call veneer to
   KillAndTrackInDeathTable, so what the ROM does when Yoshi swallows one of
   these is kill it AND track it so it comes back. Trapping the slot turned a
   respawnable death into a permanent freeze -- the rabbit that never returns. */
extern "C" {
extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
/* per-actor decline: quarantine this one actor (freeze) instead of aborting the
   whole process, unless SM64DS_FAULTS_FATAL. port/unmatched/func_02043fdc_hostcopy.cpp
   The _for form NAMES the actor, which is what makes the freeze land on the
   receiver rather than on whoever was being walked when the slot dispatched. */
void port_actor_slot_decline(const char *what);
void port_actor_slot_decline_for(void *actor, const char *what);
}

static void ac_trap_report(void *self, int slot)
{
    /* An unhosted vtable slot is a PER-ACTOR decline: this one actor reached a
       method the port has not proved, but the rest of the level is fine. It
       dispatches inside fn(actor), so port_actor_slot_decline raises a fault the
       quarantine net catches -- the actor freezes, the frame goes on. Was a hard
       std::abort() (the whole process for one actor); SM64DS_FAULTS_FATAL keeps
       that behaviour for proof runs.

       "THE ACTOR" HAS TO BE `self`, AND FOR THE INTERACTION TAIL IT WAS NOT.
       Slots 20..31 are dispatched by the ATTACKER, inside the attacker's own
       phase callback, so the walk is standing on the Player when a brick's
       slot 21/23/28 declines here. The net freezes whatever it is walking, so
       it froze the Player: he stops moving, and because a frozen actor is never
       dispatched again he is never destroyed either, so every later level
       change fails to converge and is declined for the rest of the session.
       Naming `self` here is what puts the freeze on the brick. */
    static char msg[128];
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    if (slot >= 0)
        std::fprintf(stderr,
                     "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                     "phase %d, spawn step %d)\n",
                     slot, id, port_actor_class_name(id), data_02099f24[0],
                     (int)data_020a4b4c);
    else
        std::fprintf(stderr,
                     "UNHOSTED: a dBgActor_c base-table slot dispatched "
                     "(actor id %u %s, phase %d, spawn step %d)\n",
                     id, port_actor_class_name(id), data_02099f24[0],
                     (int)data_020a4b4c);
    std::snprintf(msg, sizeof msg, "unhosted vtable slot %d on id %u %s",
                  slot, id, port_actor_class_name(id));
    port_actor_slot_decline_for(self, msg);
}
static int __fastcall ac_trap13(void *s, void *) { ac_trap_report(s, 13); return 0; }
static int __fastcall ac_trap14(void *s, void *) { ac_trap_report(s, 14); return 0; }
static int __fastcall ac_trap16(void *s, void *) { ac_trap_report(s, 16); return 0; }
static int __fastcall ac_trap17(void *s, void *) { ac_trap_report(s, 17); return 0; }
/* The interaction-tail traps. A class that OVERRIDES one of Actor's 20..31
   bodies has a body of its own that is not in any slice, so forwarding to
   Actor's would run the wrong code; declining names the slot and the class
   instead. Slot 30 declines for every class in this file (sret contract
   unproved). Their own `ret` is unreachable -- port_actor_slot_decline either
   aborts under SM64DS_FAULTS_FATAL or raises for the quarantine handler -- so
   the two-parameter shape here is dead code, not a pop-contract break. */
#define AC_TRAP(n) \
    static int __fastcall ac_trap##n(void *s, void *) \
    { ac_trap_report(s, n); return 0; }
AC_TRAP(21) AC_TRAP(22)
AC_TRAP(27) AC_TRAP(28) AC_TRAP(29) AC_TRAP(30) AC_TRAP(31)
#undef AC_TRAP
/* Slots 23 and 24 take the THREE-parameter shape so they emit `ret 4`, not a
   bare `ret`. Their one dispatch site each is now thiscall (the receiver in
   ECX and the single pushed argument popped by the callee -- see
   unmatched/Actor_OnAttacked2Dispatch.cpp and Actor_OnKickedDispatch.cpp), and
   a two-parameter trap on those slots would emit a bare `ret` that pops nothing
   against a caller that pushes one, leaving a stale word. The body is identical
   to the AC_TRAP shape above -- decline still names the slot and the class --
   only the pop contract widens to match the seated ac_atk2/ac_kicked bodies. */
static int __fastcall ac_trap23(void *s, void *, void *)
{ ac_trap_report(s, 23); return 0; }
static int __fastcall ac_trap24(void *s, void *, void *)
{ ac_trap_report(s, 24); return 0; }
static int __fastcall plat_trap(void *s, void *) { ac_trap_report(s, -1); return 0; }

/* ---- the pieces the hosted D1 destructors tear members down with -----------
   The ROM's destroy path is ActorBase::AfterCleanupResources: unlink both
   nodes, drop the dedicated heap, then `this->Destructor()` -- a VIRTUAL
   dispatch of slot 16 (D1, the complete-object form) -- and only after that
   returns does it Memory::Deallocate the object itself. So a hosted slot 16
   is the class's own D0 body minus the final Deallocate: install the class's
   own vtable, member D1s in reverse declaration order, Actor's D2. The walk
   proved the path live: a woken Bird's flock climbs past y 3000 and
   func_ov009_02111234 marks every one of them. */
extern "C" {
void _ZN11ShadowModelD1Ev(void *);
void _ZN9ModelAnimD1Ev(void *);
void _ZN5ModelD1Ev(void *);
void _ZN12WithMeshClsnD1Ev(void *);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
void *_ZN18MovingCylinderClsnD1Ev(void *);
void _ZN9ModelBaseD2Ev(void *);
void *_ZN5ActorD2Ev(void *);
}

/* ---- gate 31: the destructors that were still trapping --------------------
   NINE of this file's classes left slot 16 on ac_trap16, and the reading was
   that nothing on the castle grounds ever destroys one of them: the trees and
   the birds die, the nets and the doors live from the level boot to the level
   teardown. That held exactly as long as there WAS no level teardown. A level
   change (gate 31) destroys every actor the level spawned, through the ROM's
   own path, and the first one aborted on a QUESTION_BLOCK slot 16 in phase 4.

   Four of the nine have a plain-C D1 in src and those are in slice_gate31.txt
   (CASTLE_WATER, METAL_NET, WATERFALL_MIST, QUESTION_BLOCK). The other five
   are written in the matched tree as real C++ destructors over SHADOW CLASSES
   -- the layout spelled as one pad member plus the sub-objects -- so the body
   is entirely compiler-generated member destruction, and MSVC would generate
   it against a base destructor no TU in this build defines. Writing them out
   is the recipe this file already documents for a hosted slot 16 and nothing
   more: the class's own vtable, member D1s in REVERSE declaration order, then
   Actor's D2. The member offsets are the shadows' own.

   The vtable store the ROM's D1 opens with is left out on purpose. It exists
   so a base destructor running mid-chain dispatches the base's virtuals; here
   the object is one instruction from Memory::Deallocate and nothing dispatches
   through it again, and writing it would mean naming a host vtable array per
   class for no reader.

   Slot 17 stays trapped in all nine. The ROM's teardown dispatches 16 and does
   the Memory::Deallocate itself (port/unmatched/
   ActorBase_AfterCleanupResources.cpp); a call landing on 17 means something
   reached the actor through operator delete, which is worth an abort. */
/* Flag's and Cannon's slot 16 (once ac_d1_flag / ac_d1_cannon, transcribed
   chains) are the matched src D1s through hal/dtor_faces_cpp.cpp. */
/* src/_ZN4DoorD0Ev.c is the same chain with the deallocate on the end, which
   is the cross-check that the member is a CommonModel at 0xd4.
   CommonModel::~CommonModel is itself a vtable store plus ModelBase's D2, and
   the store is the one this file leaves out, so the chain is spelled at its
   bottom rather than through a symbol whose only other use would be a host
   vtable array nobody dispatches through. */
static int __fastcall ac_d1_door(void *s, void *)
{
    _ZN9ModelBaseD2Ev((char *)s + 0xd4);
    return (int)(size_t)_ZN5ActorD2Ev(s);
}

/* The four whose D1 is plain C in src (slice_gate31.txt). Each already does
   its own vtable stores and its own member chain; the thunk is only the
   __fastcall face the slot needs. */
extern "C" {
int *_ZN11CastleWaterD1Ev(int *self);
int *_ZN11CastleWaterD0Ev(int *self);  /* slot 17, DTOR-PAIRS seat (ov009 0x02111abc) */
int *_ZN8MetalNetD1Ev(int *self);
int *_ZN13QuestionBlockD1Ev(int *self);
}
static int __fastcall cw_d1(void *s, void *)
{ return (int)(size_t)_ZN11CastleWaterD1Ev((int *)s); }
/* DTOR-PAIRS seat: the water's own D0 spells VT0/VT1; port/CMakeLists.txt
   compiles that one TU with the ROM's two relocation targets
   (_ZTV14daObjMcWater_c and the Platform base table as _ZTV10dBgActor_c). */
static int __fastcall cw_d0(void *s, void *)
{ return (int)(size_t)_ZN11CastleWaterD0Ev((int *)s); }
static int __fastcall mn_d1(void *s, void *)
{ return (int)(size_t)_ZN8MetalNetD1Ev((int *)s); }
static int __fastcall qb_d1(void *s, void *)
{ return (int)(size_t)_ZN13QuestionBlockD1Ev((int *)s); }

/* Fill the ten shared slots plus the three traps; the caller writes its own
   six. Keeps each class's fill down to the lines that are actually its own. */
static void ac_fill_shared(void **vt)
{
    vt[1] = (void *)ac_binit;
    vt[2] = (void *)ac_ainit;
    vt[4] = (void *)ac_bclean;
    vt[5] = (void *)ac_aclean;
    vt[7] = (void *)ac_bbeh;
    vt[8] = (void *)ac_abeh;
    vt[10] = (void *)ac_bren;
    vt[11] = (void *)ac_aren;
    vt[13] = (void *)ac_trap13;
    vt[14] = (void *)ac_trap14;
    vt[15] = (void *)ac_heap;
    vt[18] = (void *)ac_yoshi;
    vt[19] = (void *)ac_turn_egg;
    /* Actor's interaction tail. Every table in this file is at least 31 words
       and every one of them carries these arm9 bodies at these indices unless
       the class overrides -- the overriders rewrite their own slots below. */
    vt[20] = (void *)ac_v50;
    vt[21] = (void *)ac_pounded;
    vt[22] = (void *)ac_atk1;
    vt[23] = (void *)ac_atk2;
    vt[24] = (void *)ac_kicked;
    vt[25] = (void *)ac_pushed;
    vt[26] = (void *)ac_cannon;
    vt[27] = (void *)ac_mega;
    vt[28] = (void *)ac_under;
    vt[29] = (void *)ac_egg;
    vt[30] = (void *)ac_trap30;
}

// ---- TREE (actor 286, ov002) -----------------------------------------------
//
// _ZTV4Tree, ov002 0x0210ac00. Five of the six own slots are plain C-named
// free functions in their TUs (the mwcc form) and two are real MSVC methods
// against include/Tree.h; the thunks bridge both faces.
//
// Behavior takes no argument at all. That is the ROM's shape, not a
// transcription slip: Tree::Behavior ticks the five GLOBAL CylinderClsn lists
// at data_ov002_02110a48 rather than anything on `this`, so mwcc's r0 went
// unread. Twenty-one Tree actors therefore all run the same body and the
// cylinders are ticked twenty-one times over -- also the ROM's behaviour.
#include "Tree.h"
extern "C" {
int _ZN4Tree13InitResourcesEv(char *self);
int _ZN4Tree8BehaviorEv(void);
void _ZN4Tree16OnPendingDestroyEv(void);
int _ZN4TreeD1Ev(char *self);
void *_ZN4TreeD0Ev(char *self);
void *_ZTV4Tree[31];
extern int data_ov002_02110a48[5];   /* the five variant cylinder lists */
}

static int __fastcall tree_init(void *s, void *)
{ return _ZN4Tree13InitResourcesEv((char *)s); }
static int __fastcall tree_clean(void *s, void *)
{ return ((Tree *)s)->Tree::CleanupResources(); }
static int __fastcall tree_behavior(void *, void *)
{ return _ZN4Tree8BehaviorEv(); }
/* SM64DS_TREE_PROBE=1: the five variant lists as InitResources built them --
   one CylinderClsnWithPos per Tree actor, its position stored in SCENE units
   (Vec3_AsrInPlace by 3) with the trunk's 30-unit lift already added, which is
   what Render clips and draws at. Printed back in world units so it can be
   read against the level's own object table. */
static void tree_probe(void)
{
    static int done;
    if (done || !std::getenv("SM64DS_TREE_PROBE"))
        return;
    done = 1;
    for (int i = 0; i < 5; ++i) {
        int n = 0;
        std::printf("[tree] variant %d:", i);
        for (int *p = (int *)(size_t)data_ov002_02110a48[i]; p && n < 64;
             p = (int *)(size_t)p[0x12], ++n)
            std::printf(" (%d,%d,%d)", p[0] * 8 / 4096,
                        (p[1] - 0x1e000) * 8 / 4096, p[2] * 8 / 4096);
        std::printf("  [%d]\n", n);
    }
}

/* SM64DS_TREE_PROBE=2 additionally draws variant 4's Model through the
   HARNESS path (hal_render_model, the one the castle uses) at the origin, so
   the collapsed-geometry question is answered on one BMD by two renderers
   rather than by argument. */
extern "C" void hal_render_model(void *model, int scaleShift);
static void tree_probe_harness(void *self)
{
    const char *e = std::getenv("SM64DS_TREE_PROBE");
    if (!e || e[0] != '2')
        return;
    char *mdl = (char *)self + 0xd4 + 4 * 0x50;
    const unsigned char *bf = *(const unsigned char *const *)(mdl + 0x0c);
    hal_render_model(mdl, bf ? (int)bf[0] : 0);
}

static int __fastcall tree_render(void *s, void *)
{ tree_probe(); tree_probe_harness(s); return ((Tree *)s)->Tree::Render(); }
static int __fastcall tree_pdes(void *, void *)
{ _ZN4Tree16OnPendingDestroyEv(); return 0; }
static int __fastcall tree_d1(void *s, void *)
{ return _ZN4TreeD1Ev((char *)s); }
static int __fastcall tree_d0(void *s, void *)
{ return (int)(size_t)_ZN4TreeD0Ev((char *)s); }

extern "C" void hal_fill_cylinder_withpos_vtable(void);
extern "C" void hal_fill_moving_cylinder_vtables(void);

extern "C" void hal_fill_tree_vtable(void)
{
    void **vt = _ZTV4Tree;
    hal_fill_cylinder_withpos_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)tree_init;
    vt[3] = (void *)tree_clean;
    vt[6] = (void *)tree_behavior;
    vt[9] = (void *)tree_render;
    vt[12] = (void *)tree_pdes;
    vt[16] = (void *)tree_d1;
    vt[17] = (void *)tree_d0;
}

// ---- AMBIENT_SOUND_EFFECTS (actor 350, ov002) x5 ---------------------------
//
// The spawn table's entry for 350 is AmbientSoundEffects_SpawnInfo, whose
// factory word is 0x020f1b94 -- AmbientSoundEffects_Spawn -- and that
// function's literal pool installs the vtable at 0x0210b4c8. Its methods are
// the block at 0x020f198c..0x020f1ac4.
//
// The reading is confirmed from the other side: the Behavior in that block
// calls Sound::PlayLong with data_ov002_0210b498[param] -- an ambient loop
// table -- and skips it in the three underwater camera modes. That is
// AMBIENT SOUND EFFECTS.
//
// Config used to carry these names one class late (mwcc emits a class as
// [methods..., Spawn], so the block BEFORE a factory belongs to it), which is
// why this row was first written against _ZN14EnemySwitchTag*. #1048 shifted
// the ov002 sound cluster onto its real blocks and the names now agree.
//
// It renders nothing (Render is `return 1`) and its five instances are silent
// while the Sound:: layer is stubbed, so what this row proves is the registry
// generalising to a second class: five actors spawn, initialise, land on both
// processing lists and tick every frame.
#include "AmbientSoundEffects.h"
extern "C" {
int _ZN19AmbientSoundEffects6RenderEv(void);
int _ZN19AmbientSoundEffects16CleanupResourcesEv(void);
void _ZN19AmbientSoundEffects16OnPendingDestroyEv(void);
int *_ZN19AmbientSoundEffectsD0Ev(int *self);   /* slot 17, gate 207 */
void *_ZTV19AmbientSoundEffects[31];
}

static int __fastcall amb_init(void *s, void *)
{ return ((AmbientSoundEffects *)s)->AmbientSoundEffects::InitResources(); }
static int __fastcall amb_clean(void *, void *)
{ return _ZN19AmbientSoundEffects16CleanupResourcesEv(); }
static int __fastcall amb_behavior(void *s, void *)
{ return ((AmbientSoundEffects *)s)->AmbientSoundEffects::Behavior(); }
static int __fastcall amb_render(void *, void *)
{ return _ZN19AmbientSoundEffects6RenderEv(); }
static int __fastcall amb_pdes(void *, void *)
{ _ZN19AmbientSoundEffects16OnPendingDestroyEv(); return 0; }
static int __fastcall amb_d0(void *s, void *)
{ return (int)(size_t)_ZN19AmbientSoundEffectsD0Ev((int *)s); }

extern "C" void hal_fill_ambient_sound_vtable(void)
{
    void **vt = _ZTV19AmbientSoundEffects;
    ac_fill_shared(vt);
    vt[0] = (void *)amb_init;
    vt[3] = (void *)amb_clean;
    vt[6] = (void *)amb_behavior;
    vt[9] = (void *)amb_render;
    vt[12] = (void *)amb_pdes;
    /* SLOT 17 IS THE ROM'S OWN D0, gate 207. It used to keep the trap on the
       reading that these two TUs need a shape the port has no reason to build;
       that is half right. The D1 does -- it is a .cpp real C++ destructor MSVC
       would emit under ??1AmbientSoundEffects@@UAE@XZ against a layout that is
       not the ROM's -- but the D0 is flat C whose only placeholders are VT and
       HEAP, and HEAP is already bound correctly. Gate 204's three conditions
       all hold and were re-measured on this build; see slice_gate207.txt.
       Slot 16 is unchanged. */
    vt[16] = (void *)hal_cppd1_AmbientSoundEffects;   /* lane DTOR-FACES-CPP */
    vt[17] = (void *)amb_d0;
}

/* Silence the unused-static warning for the two shared helpers a class with
   its own OnPendingDestroy does not take. */
extern "C" void *hal_actor_shared_pdes(void) { return (void *)ac_pdes_base; }

// ---- Platform, the base three of these classes destruct through ------------
//
// Every Platform-derived destructor is the mwcc two-step: install the class's
// own vtable, run the members that belong to the derived part, install the
// BASE vtable, run the rest. The base one is ov002 0x0210ae38, which the
// destructors spell _ZTV10dBgActor_c (Platform is dBgActor_c in the ROM's own
// RTTI) and which config now calls _ZTV8Platform -- it read
// _ZTV17ExclamationSwitch until #1042, a name attached one class late, the
// same skew AMBIENT_SOUND_EFFECTS ran into. The ROM settles it: slots 16/17 of that
// table are _ZN8PlatformD2Ev and _ZN8PlatformD0Ev, and slots 0/3/6/9 are
// ActorBase's own do-nothing bodies, which is exactly a base-class table.
//
// It is only ever installed BETWEEN two member teardowns and nothing dispatches
// through it while it is there, so the port gives it the shared half and traps
// the rest: if a future class does dispatch, it says which slot.
//
// THIRTY-TWO WORDS, not twenty. This is the ROM's Platform base table: 20..30
// are Actor's own interaction list and 31 is Platform::Kill. The prefill loop
// stopped at 20 with the array, so the width had to move with it or 20..31
// would have stayed null.
extern "C" { void *_ZTV10dBgActor_c[32];   /* vtspan: _ZTV8Platform */
              extern void *_ZTV8Platform[32]; }
extern "C" void hal_fill_platform_vtable(void)
{
    static int done;
    if (done) return;
    done = 1;
    /* _ZTV8Platform (hal/actor_vtables.cpp) is the SAME ROM table under the
       other name a destructor TU may spell; it was plain zeroed storage, so a
       dispatch through it while it was the installed vptr called address 0.
       Both get the same contents. */
    void **tabs[2] = { _ZTV10dBgActor_c, _ZTV8Platform };
    for (int k = 0; k < 2; ++k) {
        void **vt = tabs[k];
        for (int i = 0; i < 32; ++i)
            vt[i] = (void *)plat_trap;
        ac_fill_shared(vt);
        vt[12] = (void *)ac_pdes_base;
        vt[31] = (void *)ac_kill;
    }
}

// ---- Enemy, the other ov002 base table the ctor chain installs -------------
//
// data_ov002_021081e4 is the Enemy base vtable (ov002, 31 slots, plain Actor
// shape). hal/actor_vtables.cpp holds the storage: the Enemy ctor
// (func_ov002_020aed98, spelled _ZN5EnemyC2Ev) installs it and the derived
// factory overwrites it two lines later, and the already-linked
// _ZN5EnemyD2Ev restores it mid-teardown -- installed only between two member
// teardowns, never dispatched through, the same reading as the Platform base
// table above.
//
// LINKAGE SEAT (lk2). The ROM's table carries exactly two of the class's own
// bodies: slot 16 the complete-object destructor (_ZN5EnemyD2Ev, linked) and
// slot 17 the deleting one (_ZN5EnemyD0Ev, clean flat-C matched src that was
// in no slice). Seating the pair gives the hosted table the ROM's contents
// and is the reference edge that pulls D0 into the link; every other slot is
// an Actor/ActorBase main body the shared half already writes. D0's VT and
// HEAP placeholders are settled in hal/cxx_aliases.cpp next to the Platform
// pair's G0 row.
extern "C" {
int _ZN5EnemyD2Ev(int *self);    /* slot 16 body (ov002 0x020aed74) */
int *_ZN5EnemyD0Ev(int *self);   /* slot 17 body (ov002 0x020aed3c) */
extern void *data_ov002_021081e4[31];   /* storage: hal/actor_vtables.cpp */
}
static int __fastcall en_d1(void *s, void *)
{ return _ZN5EnemyD2Ev((int *)s); }
static int __fastcall en_d0(void *s, void *)
{ return (int)(size_t)_ZN5EnemyD0Ev((int *)s); }

extern "C" void hal_fill_enemy_base_vtable(void)
{
    void **vt = data_ov002_021081e4;
    for (int i = 0; i < 31; ++i)
        vt[i] = (void *)plat_trap;
    ac_fill_shared(vt);
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)en_d1;
    vt[17] = (void *)en_d0;
    /* 31 words, no slot 31: the Enemy is a plain Actor, not a Platform. */
}

// ---- BLACK_BRICK_BLOCK (actor 17, ov002) x1 --------------------------------
//
// _ZTV13BigBrickBlock, ov002 0x02108adc. One class body serves six actor ids
// and switches on its own; 17 is 0x11, the variant that tracks a star and
// carries the 1.0-scaled KCL. The castle grounds' single instance is at
// (-3300, -650, 20), under the bridge.
//
// Four of the six own slots are real MSVC methods against include/
// BigBrickBlock.h; InitResources and the two destructors are C-form TUs.
#include "BigBrickBlock.h"
extern "C" {
int _ZN13BigBrickBlock13InitResourcesEv(void *self);
int *_ZN13BigBrickBlockD1Ev(int *self);
int *_ZN13BigBrickBlockD0Ev(int *self);
void *_ZTV13BigBrickBlock[32];
}
/* The destructors spell the class's own table by its RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV13daObjBlockL_c=__ZTV13BigBrickBlock")

static int __fastcall bbb_init(void *s, void *)
{ return _ZN13BigBrickBlock13InitResourcesEv(s); }
static int __fastcall bbb_clean(void *s, void *)
{ return ((BigBrickBlock *)s)->BigBrickBlock::CleanupResources(); }
static int __fastcall bbb_behavior(void *s, void *)
{ return ((BigBrickBlock *)s)->BigBrickBlock::Behavior(); }
extern "C" void port_actor_render_probe(const char *cls, void *model);
static int __fastcall bbb_render(void *s, void *)
{
    port_actor_render_probe("BLACK_BRICK_BLOCK", (char *)s + 0xd4);
    return ((BigBrickBlock *)s)->BigBrickBlock::Render();
}
static int __fastcall bbb_d1(void *s, void *)
{ return (int)(size_t)_ZN13BigBrickBlockD1Ev((int *)s); }
static int __fastcall bbb_d0(void *s, void *)
{ return (int)(size_t)_ZN13BigBrickBlockD0Ev((int *)s); }

/* Slot 21, OnGroundPounded(Actor &other). The declared third parameter is the
   pounder: the caller pushes it and a __fastcall thunk only pops what it
   declares, so two parameters would leave it on the stack. The body reads it
   (its `a->f8` is the pounder's +8) and dispatches slot 31 as a C++ virtual,
   which is why 31 is seated below rather than left trapped. */
extern "C" void func_ov002_020b382c(void *self, void *other);
static int __fastcall bbb_pounded(void *s, void *, void *other)
{ func_ov002_020b382c(s, other); return 0; }
/* Slot 31, Kill(). No arguments, so nothing to pop. */
extern "C" void func_ov002_020b38a0(char *self);
static int __fastcall bbb_kill(void *s, void *)
{ func_ov002_020b38a0((char *)s); return 0; }

extern "C" void hal_fill_black_brick_block_vtable(void)
{
    void **vt = _ZTV13BigBrickBlock;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)bbb_init;
    vt[3] = (void *)bbb_clean;
    vt[6] = (void *)bbb_behavior;
    vt[9] = (void *)bbb_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)bbb_d1;
    vt[17] = (void *)bbb_d0;
    /* A 32-slot Platform table, and BLACK_BRICK_BLOCK overrides six of the
       tail: 21 OnGroundPounded (ov002 0x020b382c), 22 OnAttacked1 (0x020b37ec),
       23 OnAttacked2 (0x020b3788), 24 OnKicked (0x020b36dc), 27 OnHitByMegaChar
       (0x020b36b4) and 31 Kill (0x020b38a0). Every address here is the ov002
       relocation for base 0x02108adc + 4N, not a recovered name: actor id 15
       is spelled BrickBlock in the registry but installs THIS table (RTTI
       13daObjBlockL_c, the Large one), so a name-led read seats the wrong
       class's bodies. Slots 28/29/30 relocate to main (0x0201012c, 0x02010124,
       0x020100dc), the shared arm9 Actor bodies, and keep the shared thunks
       ac_fill_shared just wrote.

       21 and 31 are seated. 21 is the ground pound three players reported, and
       it DELEGATES to 31: seating 21 alone would have moved the decline one
       level down into Kill and still frozen the Player, which looks fixed and
       is not. Both bodies are sliced with it (slice_gate16.txt) and 31's whole
       closure was already linked.

       22 AND 23 STAY TRAPPED here, but the reason narrowed. Slot 23's only
       dispatcher (was src/func_ov002_020ef070.cpp:73) modelled the vtable as
       plain cdecl function pointers while every seated thunk here is thiscall;
       one word could not satisfy both. That dispatcher is now the thiscall
       host copy unmatched/Actor_OnAttacked2Dispatch.cpp (slot 24's is
       unmatched/Actor_OnKickedDispatch.cpp), and ac_trap23/ac_trap24 are the
       three-parameter shape that emits `ret 4` to match it, so the convention
       conflict is gone. 23 stays on ac_trap23 only because its real body is
       not sliced yet -- seating it is a follow-up, not this change. Slot 22 is
       still genuinely blocked: its body (0x020b37ec) reaches slot 31 through
       `fn(c)`, a cdecl call, so seating 22 would put a cdecl caller back onto
       the slot 31 this file seated thiscall; it needs its own dispatcher
       converted first, the transform in unmatched/Player_HeadBonk.cpp. */
    vt[21] = (void *)bbb_pounded;
    vt[22] = (void *)ac_trap22;
    vt[23] = (void *)ac_trap23;
    vt[24] = (void *)ac_trap24;
    vt[27] = (void *)ac_trap27;
    vt[31] = (void *)bbb_kill;
}

// ---- STAR_MARKER (actor 180, ov002) ----------------------------------------
//
// _ZTV10StarMarker, ov002 0x0210aab8 (RTTI daStarBase_c). The invisible marker
// that holds a course star's spot: it drops a ground ray, plants a
// MovingCylinderClsnWithPos trigger, and for the red-coin variant (kind 6)
// spawns the POWER_STAR (id 0xb2) as a child -- guarded by `if (sp != 0)`, so an
// unregistered POWER_STAR just leaves the child unspawned, no hard dependency.
// A plain Actor subclass (the Rabbit shape), 20-slot vtable, own slots
// 0/3/6/9/12/16/17. Init/Clean/Behavior/Render/OnPendingDestroy are real C++
// methods (call qualified); D0 is extern-C .c; D1's matched .cpp is a real C++
// destructor; since lane DTOR-FACES-CPP it links from src through the faces
// in hal/dtor_faces_cpp.cpp (the host copy port/unmatched/StarMarker_D1.cpp
// that stood in for it is deleted). The
// Behavior is a plain method with no pointer-to-member.
//
// InitResources/CleanupResources declare data_ov002_0210d9a8 as a C++ `extern
// char`, which MSVC mangles to ?data_ov002_0210d9a8@@3DA; the mounted symbol is
// the C-linkage _data_ov002_0210d9a8. Alias it, byte-faithful.
#include "StarMarker.h"
extern "C" {
int *_ZN10StarMarkerD0Ev(int *self);                    /* .c, C linkage */
int *_ZN10StarMarkerD1Ev(int *self);                    /* host copy, extern C */
void *_ZTV10StarMarker[31];
}
#pragma comment(linker, "/alternatename:?data_ov002_0210d9a8@@3DA=_data_ov002_0210d9a8")
/* the destructors spell the class's own table by its RTTI name */
#pragma comment(linker, "/alternatename:__ZTV12daStarBase_c=__ZTV10StarMarker")
static int __fastcall sm_init(void *s, void *)
{ return ((StarMarker *)s)->StarMarker::InitResources(); }
static int __fastcall sm_clean(void *s, void *)
{ return ((StarMarker *)s)->StarMarker::CleanupResources(); }
static int __fastcall sm_behavior(void *s, void *)
{ return ((StarMarker *)s)->StarMarker::Behavior(); }
static int __fastcall sm_render(void *s, void *)
{
    port_actor_render_probe("STAR_MARKER", (char *)s + 0x114);
    return ((StarMarker *)s)->StarMarker::Render();
}
static int __fastcall sm_pdes(void *s, void *)
{ ((StarMarker *)s)->StarMarker::OnPendingDestroy(); return 0; }
/* slot 16: the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP). */
static int __fastcall sm_d0(void *s, void *)
{ return (int)(size_t)_ZN10StarMarkerD0Ev((int *)s); }
extern "C" void hal_fill_star_marker_vtable(void)
{
    void **vt = _ZTV10StarMarker;
    ac_fill_shared(vt);
    vt[0] = (void *)sm_init;
    vt[3] = (void *)sm_clean;
    vt[6] = (void *)sm_behavior;
    vt[9] = (void *)sm_render;
    vt[12] = (void *)sm_pdes;
    vt[16] = (void *)hal_cppd1_StarMarker;
    vt[17] = (void *)sm_d0;
}

// ---- BLUE_COIN_SWITCH (actor 10, ov002) ------------------------------------
//
// _ZTV14BlueCoinSwitch, ov002 0x0210b26c (RTTI daObjBC_Switch_c). The switch
// that, when ground-pounded, turns the level's blue-coin markers into collecta-
// ble blue coins for a few seconds. A Platform subclass whose six own slots are
// all extern-C C-linkage TUs (the .cpp Render is extern "C" too), so every thunk
// calls the mangled name directly. Its Behavior is a plain state machine with no
// pointer-to-member and no message path. The same 32-slot Platform table
// BigBrickBlock has, hosted as a [20] the fill covers -- nothing dispatches a
// slot past 19 (slot 31 is Platform::Kill, reached only by KillAndTrackIn-
// DeathTable, which the ROM never triggers here).
extern "C" {
int _ZN14BlueCoinSwitch13InitResourcesEv(void *self);
int _ZN14BlueCoinSwitch16CleanupResourcesEv(void *self);
int _ZN14BlueCoinSwitch8BehaviorEv(void *self);
int _ZN14BlueCoinSwitch6RenderEv(char *self);
int *_ZN14BlueCoinSwitchD1Ev(int *self);
int *_ZN14BlueCoinSwitchD0Ev(int *self);
void *_ZTV14BlueCoinSwitch[32];
}
/* the destructors spell the class's own table by its RTTI name */
#pragma comment(linker, "/alternatename:__ZTV16daObjBC_Switch_c=__ZTV14BlueCoinSwitch")
static int __fastcall bcs_init(void *s, void *)
{ return _ZN14BlueCoinSwitch13InitResourcesEv(s); }
static int __fastcall bcs_clean(void *s, void *)
{ return _ZN14BlueCoinSwitch16CleanupResourcesEv(s); }
static int __fastcall bcs_behavior(void *s, void *)
{ return _ZN14BlueCoinSwitch8BehaviorEv(s); }
static int __fastcall bcs_render(void *s, void *)
{
    port_actor_render_probe("BLUE_COIN_SWITCH", (char *)s + 0xd4);
    return _ZN14BlueCoinSwitch6RenderEv((char *)s);
}
static int __fastcall bcs_d1(void *s, void *)
{ return (int)(size_t)_ZN14BlueCoinSwitchD1Ev((int *)s); }
static int __fastcall bcs_d0(void *s, void *)
{ return (int)(size_t)_ZN14BlueCoinSwitchD0Ev((int *)s); }
extern "C" void hal_fill_blue_coin_switch_vtable(void)
{
    void **vt = _ZTV14BlueCoinSwitch;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)bcs_init;
    vt[3] = (void *)bcs_clean;
    vt[6] = (void *)bcs_behavior;
    vt[9] = (void *)bcs_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)bcs_d1;
    vt[17] = (void *)bcs_d0;
    /* 32 slots. Slot 31 is Platform::Kill (ov002 0x020ee55c), not overridden
       here, and 20..30 are Actor's own list ac_fill_shared already wrote. */
    vt[31] = (void *)ac_kill;
}

// ---- SIGN_POST (actor 184, ov002) x5 ---------------------------------------
//
// _ZTV8SignPost, ov002 0x02109af8 (RTTI: daObjTatefuda_c -- tatefuda is a
// signboard). Five of them around the grounds. This is the first class whose
// InitResources RAYCASTS THE LEVEL: it drops a ground ray from 100 units above
// the object record's Y and plants the post on whatever it finds, so the five
// signs read back the same floor the Player stands on.
//
// It also carries three collision objects at once -- a MovingMeshCollider for
// the solid post, a MovingCylinderClsn for the read trigger, and a WithMeshClsn
// of its own -- which is the shape every Platform actor after it has.
//
// THE TALK PATH IS GUARDED, and this is the one place a slot is not the ROM's.
// Behavior's read branch runs when the Player is in range with its 0xc8 word
// set, and func_ov002_020bb060 ends in the Message system (Message::Show and
// the dialogue box's OAM/font machinery), none of which the port hosts. The
// guard is in hal/actor_vtables.cpp at that function, not here: the sign
// still turns to face the player and still blocks him, it just does not open
// a box that has nothing to draw with.
#include "SignPost.h"
extern "C" {
int _ZN8SignPost8BehaviorEv(char *self);
int *_ZN8SignPostD1Ev(int *self);
int *_ZN8SignPostD0Ev(int *self);
void *_ZTV8SignPost[32];
}
#pragma comment(linker, "/alternatename:__ZTV15daObjTatefuda_c=__ZTV8SignPost")

static int __fastcall sp_init(void *s, void *)
{ return ((SignPost *)s)->SignPost::InitResources(); }
static int __fastcall sp_clean(void *s, void *)
{ return ((SignPost *)s)->SignPost::CleanupResources(); }
static int __fastcall sp_behavior(void *s, void *)
{ return _ZN8SignPost8BehaviorEv((char *)s); }
/* SM64DS_ACTOR_PROBE=1: one line per class the first time its Render runs,
   with what the Model has to draw with -- the BMD the file pointer resolved
   to and the model matrix's own translation row, which Platform writes in
   SCENE units. A class that never prints is being culled before Render;
   one that prints with a null file has a load problem instead. */
extern "C" void port_actor_render_probe(const char *cls, void *model)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_ACTOR_PROBE") != 0;
    if (!on) return;
    /* 32, not 8: the registry carried five classes when this was written and
       carries twenty now, so the old cap stopped reporting before it reached
       the ones landed last -- silently, which is the worst way for a probe to
       go quiet. The transforms pointer is new for the same reason: it is what
       ModelBase::SetFile allocates and what a Model's own Virtual10 copies
       out of, so a null there is a load that did not finish. */
    static const char *said[32];
    static int n;
    for (int i = 0; i < n; ++i)
        if (said[i] == cls) return;
    if (n < 32) said[n++] = cls;
    const int *m = (const int *)((const char *)model + 0x1c);
    std::printf("[actor] %-17s model %p file %p transforms %p "
                "mat.t (%d,%d,%d) scene\n",
                cls, model, *(void *const *)((const char *)model + 0x0c),
                *(void *const *)((const char *)model + 0x14),
                m[9] >> 12, m[10] >> 12, m[11] >> 12);
}

static int __fastcall sp_render(void *s, void *)
{
    port_actor_render_probe("SIGN_POST", (char *)s + 0xd4);
    return ((SignPost *)s)->SignPost::Render();
}
static int __fastcall sp_d1(void *s, void *)
{ return (int)(size_t)_ZN8SignPostD1Ev((int *)s); }
static int __fastcall sp_d0(void *s, void *)
{ return (int)(size_t)_ZN8SignPostD0Ev((int *)s); }

extern "C" void port_sign_post_states_seat(void);   /* port/unmatched */

extern "C" void hal_fill_sign_post_vtable(void)
{
    void **vt = _ZTV8SignPost;
    /* the sinit copied the ROM's own five {Init, Main} pairs into
       data_ov002_0210e084, which means DS code addresses; seat the host
       bodies over them (the ovdata contract, see the file header there) */
    port_sign_post_states_seat();
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)sp_init;
    vt[3] = (void *)sp_clean;
    vt[6] = (void *)sp_behavior;
    vt[9] = (void *)sp_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)sp_d1;
    vt[17] = (void *)sp_d0;
    /* SIGN_POST is a 32-slot Platform table and the worst instance of the [20]
       bug in this file: the array ended at 0x50, _ZTV13OneUpMushroom starts
       0x50 later with no padding, and slot 22 therefore landed on the 1-up's
       slot 2 -- Actor::AfterInitResources, called with a sign as `this`. Slot
       21 crashed outright (the confirmed level-3 repro).
       Four of the tail are the sign's OWN bodies, matched in src and in no
       slice: 21 OnGroundPounded (ov002 0x020bb27c, the post sinking a notch
       and playing bank-3 sound 0x62), 22 OnAttacked1 (0x020bb23c), 27
       OnHitByMegaChar (0x020bb374) and 31 Kill (0x020bb3b8). They decline by
       name rather than run Actor's shared body, which is not what a sign
       does. */
    vt[21] = (void *)ac_trap21;
    vt[22] = (void *)ac_trap22;
    vt[27] = (void *)ac_trap27;
    vt[31] = (void *)ac_trap31;
}

// ---- ONE_UP_MUSHROOM (actor 276, ov002) x3 ---------------------------------
//
// _ZTV13OneUpMushroom, ov002 0x021083c8 (RTTI: da1up_c). Three of them on the
// castle roof at param 0x00f3, which is mushroom type 3.
//
// Two slots are this class's own rather than Actor's, and they are the reason
// the class exists: slot 18 (OnYoshiTryEat) returns 4 -- "swallow me" -- and
// slot 19 (OnTurnIntoEgg) is the collect path, GiveLives plus the 1UP logo
// actor. Both are hosted; nothing on this level reaches them while the
// character is Mario.
//
// Behavior is the HOST COPY in port/unmatched/OneUpMushroom_Behavior.cpp: the
// ROM's dispatches the mushroom type through an mwcc pointer-to-member table
// and MSVC has no representation for one. See that file's header.
#include "OneUpMushroom.h"
extern "C" {
int _ZN13OneUpMushroom8BehaviorEv(void *self);       /* port/unmatched */
void _ZN13OneUpMushroom16OnPendingDestroyEv(void);
int *_ZN13OneUpMushroomD1Ev(int *self);
int *_ZN13OneUpMushroomD0Ev(int *self);
int func_ov002_020af3a0(void);                       /* OnYoshiTryEat */
void func_ov002_020af2b0(char *self, int arg);       /* OnTurnIntoEgg */
void *_ZTV13OneUpMushroom[31];
}
#pragma comment(linker, "/alternatename:__ZTV7da1up_c=__ZTV13OneUpMushroom")

static int __fastcall oum_init(void *s, void *)
{ return ((OneUpMushroom *)s)->OneUpMushroom::InitResources(); }
static int __fastcall oum_clean(void *s, void *)
{ return ((OneUpMushroom *)s)->OneUpMushroom::CleanupResources(); }
static int __fastcall oum_behavior(void *s, void *)
{ return _ZN13OneUpMushroom8BehaviorEv(s); }
static int __fastcall oum_render(void *s, void *)
{
    port_actor_render_probe("ONE_UP_MUSHROOM", (char *)s + 0x300);
    return (int)((OneUpMushroom *)s)->OneUpMushroom::Render();
}
static int __fastcall oum_pdes(void *, void *)
{ _ZN13OneUpMushroom16OnPendingDestroyEv(); return 0; }
static int __fastcall oum_d1(void *s, void *)
{ return (int)(size_t)_ZN13OneUpMushroomD1Ev((int *)s); }
static int __fastcall oum_d0(void *s, void *)
{ return (int)(size_t)_ZN13OneUpMushroomD0Ev((int *)s); }
static int __fastcall oum_yoshi(void *, void *)
{ return func_ov002_020af3a0(); }
static int __fastcall oum_egg(void *s, void *, int a)
{ func_ov002_020af2b0((char *)s, a); return 0; }

extern "C" void port_one_up_mushroom_types_seat(void);   /* port/unmatched */

extern "C" void hal_fill_one_up_mushroom_vtable(void)
{
    void **vt = _ZTV13OneUpMushroom;
    /* same as the sign: __sinit_ov002_02100adc left fourteen DS code
       addresses in data_ov002_0210dc00 */
    port_one_up_mushroom_types_seat();
    ac_fill_shared(vt);
    vt[0] = (void *)oum_init;
    vt[3] = (void *)oum_clean;
    vt[6] = (void *)oum_behavior;
    vt[9] = (void *)oum_render;
    vt[12] = (void *)oum_pdes;
    vt[16] = (void *)oum_d1;
    vt[17] = (void *)oum_d0;
    vt[18] = (void *)oum_yoshi;
    vt[19] = (void *)oum_egg;
}

// ---- CylinderClsnWithPos ---------------------------------------------------
//
// Not an actor, but the Tree's InitResources constructs one per instance and
// the constructor installs _ZTV19CylinderClsnWithPos by name. Four slots
// (arm9 0x0208e6bc: D1, D0, GetPos, GetOwnerID) and the last two really do
// dispatch -- every cylinder the Tree threads onto data_0209cee8 shares that
// list with the Player's own, and the cylinder pass asks each node where it
// is and who owns it. A zeroed table would be a call through null the first
// time Mario walked near a trunk.
extern "C" {
void *_ZTV19CylinderClsnWithPos[4];
void *_ZN19CylinderClsnWithPosD1Ev(void *self);
void *_ZN19CylinderClsnWithPosD0Ev(void *self);
void *_ZN19CylinderClsnWithPos6GetPosEv(void *self);
}
#include "CylinderClsnWithPos.h"

static void *__fastcall ccp_d1(void *s, void *)
{ return _ZN19CylinderClsnWithPosD1Ev(s); }
static void *__fastcall ccp_d0(void *s, void *)
{ return _ZN19CylinderClsnWithPosD0Ev(s); }
static void *__fastcall ccp_getpos(void *s, void *)
{ return _ZN19CylinderClsnWithPos6GetPosEv(s); }
static unsigned __fastcall ccp_ownerid(void *s, void *)
{ return ((CylinderClsnWithPos *)s)->CylinderClsnWithPos::GetOwnerID(); }

extern "C" void hal_fill_cylinder_withpos_vtable(void)
{
    _ZTV19CylinderClsnWithPos[0] = (void *)ccp_d1;
    _ZTV19CylinderClsnWithPos[1] = (void *)ccp_d0;
    _ZTV19CylinderClsnWithPos[2] = (void *)ccp_getpos;
    _ZTV19CylinderClsnWithPos[3] = (void *)ccp_ownerid;
}

// ---- the two ACTOR-ATTACHED cylinder tables --------------------------------
//
// Same treatment, same reason, and the reason is what killed the tree-grab
// chain on frame 0. _ZTV18MovingCylinderClsn (arm9 0x0208e6d4) and
// _ZTV25MovingCylinderClsnWithPos (0x0208e704) are declared in
// hal/actor_vtables.cpp as bare zeroed arrays and nothing filled them,
// because until now nothing in the port dispatched a cylinder virtual --
// every consumer read the objects' fields directly.
//
// CylinderClsn::Process is the consumer that does. It asks EVERY node on
// data_0209cee8 for GetPos and GetOwnerID, and the castle grounds put seven
// actor-attached cylinders on that list from the first frame: six
// MovingCylinderClsn (the SIGN_POSTs' read triggers and the RABBITs) and one
// MovingCylinderClsnWithPos, behind the 21 CylinderClsnWithPos the trees
// thread. The tree table was filled and the actor ones were not, so the pass
// walked 21 good nodes and then called address zero. That is the frame-0
// crash the chain was backed out for; PORT_TRACE_CYL=1 in
// port/unmatched/CylinderClsn_Process.cpp prints the list with both slots.
//
// Slot order is the ROM's, per include/MovingCylinderClsn.h: [0] D1, [1] D0,
// [2] GetPos, [3] GetOwnerID. WithPos overrides GetPos only -- its ROM slot 3
// is a 12-byte interworking veneer onto the parent's GetOwnerID, so the port
// installs the parent's shim there rather than inventing an override.
extern "C" {
void *_ZTV18MovingCylinderClsn[];
void *_ZTV25MovingCylinderClsnWithPos[];
void *_ZN18MovingCylinderClsnD1Ev(void *self);
void *_ZN18MovingCylinderClsnD0Ev(void *self);
void *_ZN25MovingCylinderClsnWithPosD0Ev(void *self);
/* WithPos::GetPos is a C file (it just returns this + 0x34), so it carries
   the plain C name rather than the method mangling the parent's .cpp emits */
void *_ZN25MovingCylinderClsnWithPos6GetPosEv(void *self);
/* the WithPos D1 is already declared above, returning void */
}
#include "MovingCylinderClsnWithPos.h"

static void *__fastcall mcc_d1(void *s, void *)
{ return _ZN18MovingCylinderClsnD1Ev(s); }
static void *__fastcall mcc_d0(void *s, void *)
{ return _ZN18MovingCylinderClsnD0Ev(s); }
static void *__fastcall mcc_getpos(void *s, void *)
{ return &((MovingCylinderClsn *)s)->MovingCylinderClsn::GetPos(); }
static unsigned __fastcall mcc_ownerid(void *s, void *)
{ return ((MovingCylinderClsn *)s)->MovingCylinderClsn::GetOwnerID(); }

static void *__fastcall mccp_d1(void *s, void *)
{ _ZN25MovingCylinderClsnWithPosD1Ev(s); return s; }
static void *__fastcall mccp_d0(void *s, void *)
{ return _ZN25MovingCylinderClsnWithPosD0Ev(s); }
static void *__fastcall mccp_getpos(void *s, void *)
{ return _ZN25MovingCylinderClsnWithPos6GetPosEv(s); }

extern "C" void hal_fill_moving_cylinder_vtables(void)
{
    _ZTV18MovingCylinderClsn[0] = (void *)mcc_d1;
    _ZTV18MovingCylinderClsn[1] = (void *)mcc_d0;
    _ZTV18MovingCylinderClsn[2] = (void *)mcc_getpos;
    _ZTV18MovingCylinderClsn[3] = (void *)mcc_ownerid;

    _ZTV25MovingCylinderClsnWithPos[0] = (void *)mccp_d1;
    _ZTV25MovingCylinderClsnWithPos[1] = (void *)mccp_d0;
    _ZTV25MovingCylinderClsnWithPos[2] = (void *)mccp_getpos;
    _ZTV25MovingCylinderClsnWithPos[3] = (void *)mcc_ownerid;  /* inherited */
}

// ============================================================================
// GATE 17: the LEVEL OVERLAY'S OWN CLASSES
// ============================================================================
//
// Four classes that exist only on the castle grounds. mwcc emits a class as
// [methods..., Spawn], so the block BEFORE a factory belongs to it, and each
// factory's own literal pool names the vtable it installs:
//
//   Bird_Spawn        -> _ZTV4Bird             = ov009 0x02113958
//   CastleWater_Spawn -> _ZTV14daObjMcWater_c  = ov009 0x02113a18
//   MetalNet_Spawn    -> _ZTV8MetalNet         = ov009 0x02113ae0
//   Flag_Spawn        -> _ZTV4Flag             = ov009 0x02113ba0
//
// The ROM's own RTTI agrees from the other side: 0x02113ae0 is
// daObjMc_Metalnet_c and 0x02113ba0 is daMcFlag_c, and config now carries both
// spellings for each table. It used to attach these names one class late from
// Bird onward, which is why this gate was first written against func_ov009_*
// for the water and _ZN11CastleWater*/_ZN8DockPole* for the net and the flag;
// #1041 shifted them onto the blocks they belong to.
//
// SLOT 17 USED TO TRAP FOR ALL FOUR. Nothing on the castle grounds destroys
// one -- every InitResources here returns 1 on this level -- and the water's
// own D0 (src/_ZN11CastleWaterD0Ev.c) spells its two vtables VT0 and VT1, the
// same placeholder names src/_ZN10SphereClsnD1Ev.c uses for three completely
// different tables, so one shared host definition would have satisfied both
// with the wrong bytes. Since lane DTOR-PAIRS-C all four D0s ride from src:
// the water's relocs (from:0x02111ab4 -> 0x02113a18, from:0x02111ab8 ->
// 0x0210ae38) name its two tables, and port/CMakeLists.txt compiles that one
// TU with VT0=_ZTV14daObjMcWater_c;VT1=_ZTV10dBgActor_c, per source.

// ---- BIRD (actor 343, ov009) x2 --------------------------------------------
extern "C" {
int _ZN4Bird13InitResourcesEv(void *self);
int _ZN4Bird8BehaviorEv(void *self);            /* host copy: hal/ov009_boot */
int _ZN4Bird6RenderEv(void *self);
int _ZN4Bird16CleanupResourcesEv(void);
void _ZN4Bird16OnPendingDestroyEv(void);
int *_ZN4BirdD0Ev(int *self);                  /* slot 17, DTOR-PAIRS seat (ov009 0x021111d8) */
void *_ZTV4Bird[31];
}
/* The Bird's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV9daSBird_c=__ZTV4Bird")

static int __fastcall bird_init(void *s, void *)
{ return _ZN4Bird13InitResourcesEv(s); }
static int __fastcall bird_clean(void *, void *)
{ return _ZN4Bird16CleanupResourcesEv(); }
static int __fastcall bird_behavior(void *s, void *)
{ return _ZN4Bird8BehaviorEv(s); }
static int __fastcall bird_render(void *s, void *)
{ port_actor_render_probe("BIRD", (char *)s + 0xd4);
  return _ZN4Bird6RenderEv(s); }
static int __fastcall bird_pdes(void *, void *)
{ _ZN4Bird16OnPendingDestroyEv(); return 0; }
static int __fastcall bird_d0(void *s, void *)
{ return (int)(size_t)_ZN4BirdD0Ev((int *)s); }
/* SLOT 16 IS LIVE: a woken bird's state 0 spawns its flock, state 3 climbs,
   and past y 3000 func_ov009_02111234 marks every member -- the turn walk
   died right here while this slot was a trap. The body is
   src/_ZN4BirdD0Ev.c minus its final Memory::Deallocate, which
   AfterCleanupResources performs itself after the dispatch returns. */
/* slot 16: the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP). */

extern "C" void hal_fill_bird_vtable(void)
{
    void **vt = _ZTV4Bird;
    ac_fill_shared(vt);
    vt[0] = (void *)bird_init;
    vt[3] = (void *)bird_clean;
    vt[6] = (void *)bird_behavior;
    vt[9] = (void *)bird_render;
    vt[12] = (void *)bird_pdes;
    vt[16] = (void *)hal_cppd1_Bird;
    /* Slot 17, the ROM's own deleting destructor (DTOR-PAIRS seat): the
       matched flat-C body behind the ecx->arg adapter, the
       lk2_platform_dtor_seat.cpp shape. Nothing on a mounted level
       dispatches 17 (AfterCleanupResources dispatches 16 and frees
       itself), so this is the ROM's word where the trap stood, and the
       reference edge that links the TU. */
    vt[17] = (void *)bird_d0;
}

// ---- CASTLE_WATER (actor 338, ov009) x1 ------------------------------------
//
// The moat, one instance at the origin. Its InitResources is the reason this
// class matters to anything but the picture: it loads a KCL of its own, hands
// it to MovingMeshCollider with the actor's matrix and a 1.0 scale, Enables it
// onto the level's collider list, and then CLAMPS THE GLOBAL WATER LEVEL --
// data_0209f32c = min(data_0209f32c, self.y - 100.0) -- which is the word the
// Player's own state machine reads to decide it is swimming.
extern "C" {
int _ZN11CastleWater13InitResourcesEv(void *self);
int _ZN11CastleWater16CleanupResourcesEv(void *self);
int _ZN11CastleWater8BehaviorEv(void *self);
int _ZN11CastleWater6RenderEv(void *self);
void *_ZTV14daObjMcWater_c[32];        /* ov009 0x02113a18 */
}
/* The water's own D0 spells this table by the class name. */
#pragma comment(linker, "/alternatename:__ZTV11CastleWater=__ZTV14daObjMcWater_c")

static int __fastcall cw_init(void *s, void *)
{ return _ZN11CastleWater13InitResourcesEv(s); }
static int __fastcall cw_clean(void *s, void *)
{ return _ZN11CastleWater16CleanupResourcesEv(s); }
static int __fastcall cw_behavior(void *s, void *)
{ return _ZN11CastleWater8BehaviorEv(s); }
static int __fastcall cw_render(void *s, void *)
{ port_actor_render_probe("CASTLE_WATER", (char *)s + 0xd4);
  return _ZN11CastleWater6RenderEv(s); }

extern "C" void hal_fill_castle_water_vtable(void)
{
    void **vt = _ZTV14daObjMcWater_c;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)cw_init;
    vt[3] = (void *)cw_clean;
    vt[6] = (void *)cw_behavior;
    vt[9] = (void *)cw_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)cw_d1;
    vt[17] = (void *)cw_d0;
    /* 32 slots; slot 31 is Platform::Kill unchanged in the ROM table. dsd left
       an ambiguous symbol at word 14, so the symbol bound reads 14 here -- the
       reloc run is what says 32. */
    vt[31] = (void *)ac_kill;
}

// ---- METAL_NET (actor 339, ov009) x3 ---------------------------------------
//
// The three climbable nets under the bridge. Same shape as the water -- a
// Platform with a MovingMeshCollider -- which is why the two were easy to
// mistake for one class twice over while the names sat one block off.
extern "C" {
int _ZN8MetalNet13InitResourcesEv(void *self);
int _ZN8MetalNet8BehaviorEv(void *self);
int _ZN8MetalNet6RenderEv(void *self);
int _ZN8MetalNet16CleanupResourcesEv(void *self);
void _ZN8MetalNet16OnPendingDestroyEv(void);
int *_ZN8MetalNetD0Ev(int *self);              /* slot 17, DTOR-PAIRS seat (ov009 0x02111e08) */
void *_ZTV8MetalNet[32];
}
#pragma comment(linker, "/alternatename:__ZTV18daObjMc_Metalnet_c=__ZTV8MetalNet")

static int __fastcall mn_init(void *s, void *)
{ return _ZN8MetalNet13InitResourcesEv(s); }
static int __fastcall mn_clean(void *s, void *)
{ return _ZN8MetalNet16CleanupResourcesEv(s); }
static int __fastcall mn_behavior(void *s, void *)
{ return _ZN8MetalNet8BehaviorEv(s); }
static int __fastcall mn_render(void *s, void *)
{ port_actor_render_probe("METAL_NET", (char *)s + 0xd4);
  return _ZN8MetalNet6RenderEv(s); }
static int __fastcall mn_pdes(void *, void *)
{ _ZN8MetalNet16OnPendingDestroyEv(); return 0; }
static int __fastcall mn_d0(void *s, void *)
{ return (int)(size_t)_ZN8MetalNetD0Ev((int *)s); }

extern "C" void hal_fill_metal_net_vtable(void)
{
    void **vt = _ZTV8MetalNet;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)mn_init;
    vt[3] = (void *)mn_clean;
    vt[6] = (void *)mn_behavior;
    vt[9] = (void *)mn_render;
    vt[12] = (void *)mn_pdes;
    vt[16] = (void *)mn_d1;
    /* Slot 17, the ROM's own deleting destructor (DTOR-PAIRS seat): the
       matched flat-C body behind the ecx->arg adapter, the
       lk2_platform_dtor_seat.cpp shape. Nothing on a mounted level
       dispatches 17 (AfterCleanupResources dispatches 16 and frees
       itself), so this is the ROM's word where the trap stood, and the
       reference edge that links the TU. */
    vt[17] = (void *)mn_d0;
    /* 32 slots; slot 31 is Platform::Kill unchanged in the ROM table. */
    vt[31] = (void *)ac_kill;
}

// ---- FLAG (actor 342, ov009) x4 --------------------------------------------
//
// The four castle-tower flags, at y 3095 and 3662. Behavior is one
// Animation::Advance and a rotation matrix: the flag waves, and it is the
// first actor on the grounds whose model is ANIMATED by the game's own
// ModelAnim rather than posed once.
extern "C" {
int _ZN4Flag13InitResourcesEv(void *self);
int _ZN4Flag8BehaviorEv(void *self);
int _ZN4Flag6RenderEv(void *self);
int _ZN4Flag16CleanupResourcesEv(void);
int *_ZN4FlagD0Ev(int *self);                  /* slot 17, DTOR-PAIRS seat (ov009 0x021120a8) */
void *_ZTV4Flag[31];
}
#pragma comment(linker, "/alternatename:__ZTV10daMcFlag_c=__ZTV4Flag")

static int __fastcall flag_init(void *s, void *)
{ return _ZN4Flag13InitResourcesEv(s); }
static int __fastcall flag_clean(void *, void *)
{ return _ZN4Flag16CleanupResourcesEv(); }
static int __fastcall flag_behavior(void *s, void *)
{ return _ZN4Flag8BehaviorEv(s); }
static int __fastcall flag_render(void *s, void *)
{ port_actor_render_probe("FLAG", (char *)s + 0xd4);
  return _ZN4Flag6RenderEv(s); }
static int __fastcall flag_d0(void *s, void *)
{ return (int)(size_t)_ZN4FlagD0Ev((int *)s); }

extern "C" void hal_fill_flag_vtable(void)
{
    void **vt = _ZTV4Flag;
    ac_fill_shared(vt);
    vt[0] = (void *)flag_init;
    vt[3] = (void *)flag_clean;
    vt[6] = (void *)flag_behavior;
    vt[9] = (void *)flag_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)hal_cppd1_Flag;
    /* Slot 17, the ROM's own deleting destructor (DTOR-PAIRS seat): the
       matched flat-C body behind the ecx->arg adapter, the
       lk2_platform_dtor_seat.cpp shape. Nothing on a mounted level
       dispatches 17 (AfterCleanupResources dispatches 16 and frees
       itself), so this is the ROM's word where the trap stood, and the
       reference edge that links the TU. */
    vt[17] = (void *)flag_d0;
}

// ============================================================================
// GATE 18: ov085, THE FIRST ACTOR OVERLAY
// ============================================================================
//
// Not the engine overlay and not the level's: ov085 exists for six classes
// that several levels share, and the castle grounds names two of them.
//
// THE CONFIG'S NAMES ARE NOT SHIFTED HERE. That is checked, not assumed --
// ov009 is the reason to check. Every one of ov085's six factories installs a
// vtable whose slot 0 is its own class's InitResources and whose slot 16 is
// its own D1, and the ROM's RTTI names agree from the other side (daMip_c is
// the rabbit, daC_Jugem_c the Lakitu). Each class's own D0 spells the table
// by the RTTI name and its D1 by the config name, which is why both spellings
// have to resolve to ONE array -- the alternatename pairs below.

// ---- RABBIT (actor 187, ov085) x12 -----------------------------------------
//
// _ZTV6Rabbit / _ZTV7daMip_c, ov085 0x021300f8. The twelve glowing rabbits.
// This is the port's first AI actor: InitResources seeds it from the star
// count and binds it to one of the level's own paths, and Behavior takes the
// closest Player every frame and runs the flee/idle machine against it
// through a MovingCylinderClsn and a WithMeshClsn.
//
// THE CATCH PATH IS GUARDED, and the guard is not here. Behavior's caught
// branch calls Message::PrepareTalk and Player::ShowMessage, which end in the
// dialogue box's OAM/font machinery -- the same system the SIGN_POST's read
// branch stops at (hal/actor_vtables.cpp). The rabbit still flees, still
// gets touched and still plays its caught animation; it just does not open a
// box that has nothing to draw with.
extern "C" {
int _ZN6Rabbit13InitResourcesEv(void *self);      /* face: method_faces */
int _ZN6Rabbit8BehaviorEv(void *self);
int _ZN6Rabbit6RenderEv(void *self);              /* face: method_faces */
int _ZN6Rabbit16CleanupResourcesEv(void);
void _ZN6Rabbit16OnPendingDestroyEv(void);
int *_ZN6RabbitD1Ev(int *self);
int *_ZN6RabbitD0Ev(int *self);
int func_ov085_0212cc18(unsigned char *self);     /* slot 18, its own */
void *_ZTV6Rabbit[31];
}
#pragma comment(linker, "/alternatename:__ZTV7daMip_c=__ZTV6Rabbit")

static int __fastcall rb_init(void *s, void *)
{ return _ZN6Rabbit13InitResourcesEv(s); }
static int __fastcall rb_clean(void *, void *)
{ return _ZN6Rabbit16CleanupResourcesEv(); }
static int __fastcall rb_behavior(void *s, void *)
{ return _ZN6Rabbit8BehaviorEv(s); }
static int __fastcall rb_render(void *s, void *)
{ port_actor_render_probe("RABBIT", (char *)s + 0x300);
  return _ZN6Rabbit6RenderEv(s); }
static int __fastcall rb_pdes(void *, void *)
{ _ZN6Rabbit16OnPendingDestroyEv(); return 0; }
static int __fastcall rb_d1(void *s, void *)
{ return (int)(size_t)_ZN6RabbitD1Ev((int *)s); }
static int __fastcall rb_d0(void *s, void *)
{ return (int)(size_t)_ZN6RabbitD0Ev((int *)s); }
/* The rabbit is the first class on this level to OVERRIDE OnYoshiTryEat
   rather than inherit Actor's -- Yoshi can eat one. */
static int __fastcall rb_yoshi(void *s, void *)
{ return func_ov085_0212cc18((unsigned char *)s); }

extern "C" void hal_fill_rabbit_vtable(void)
{
    void **vt = _ZTV6Rabbit;
    ac_fill_shared(vt);
    vt[0] = (void *)rb_init;
    vt[3] = (void *)rb_clean;
    vt[6] = (void *)rb_behavior;
    vt[9] = (void *)rb_render;
    vt[12] = (void *)rb_pdes;
    vt[16] = (void *)rb_d1;
    vt[17] = (void *)rb_d0;
    vt[18] = (void *)rb_yoshi;
}

// ---- RABBIT_KEY (actor 229, ov085) -----------------------------------------
//
// _ZTV9RabbitKey / _ZTV15daObj_Mip_Key_c, ov085 0x021301d8 (RTTI daObj_Mip_Key_c).
// The KEY the caught rabbit turns into. THIS is the actor that grants progress:
// the caught-dialog state (func_ov085_0212ae08, already in gate 18) ends the talk
// by calling Actor::Spawn(0xe5, ...) to make a RabbitKey at the rabbit's spot, and
// the RabbitKey's own state machine writes the save "caught" bit (func_02013868 ->
// data_0209caa0[3]), plays the caught cutscene (Player::SetNoControlState), and for
// the eighth catch opens the minigame menu (StartMinigameMenu). Without this class
// registered the id-229 spawn was declined by the prespawn gate and the whole grant
// was skipped silently -- no key, and because the save bit never set, a re-talk
// re-entered the caught dialog and stuck. Same 20-slot Enemy shape as RABBIT, no
// own OnYoshiTryEat override (ROM slots 18+ are the base's). The state machine is a
// PMF table (data_ov085_0213071c/072c) dispatched out of this+0x188; the four
// {function, delta} statics are seated host-side by port_rabbit_key_states_seat
// (hal/actor_overlays.cpp) before the sinit copies them. TWO HOST COPIES ride
// along: the seat-and-enter func_ov085_0212d268 (a real MSVC PMF over an
// incomplete class, the gate-16 case -- port/unmatched/RabbitKey_StateSeat.cpp)
// and Render (shadow-struct Model slot-5 dispatch, cdecl into __fastcall --
// port/unmatched/Ov085_Renders.cpp, the third ov085 render). Behavior's own
// per-frame dispatch is hand-rolled in the matched source and safe as-is.
extern "C" {
int _ZN9RabbitKey13InitResourcesEv(void *self);   /* face: method_faces */
int _ZN9RabbitKey8BehaviorEv(void *self);          /* face: method_faces */
int _ZN9RabbitKey6RenderEv(void *self);            /* host copy: Ov085_Renders */
int _ZN9RabbitKey16CleanupResourcesEv(void);
void _ZN9RabbitKey16OnPendingDestroyEv(void);
int *_ZN9RabbitKeyD1Ev(int *self);
int *_ZN9RabbitKeyD0Ev(int *self);
void *_ZTV9RabbitKey[31];
}
#pragma comment(linker, "/alternatename:__ZTV15daObj_Mip_Key_c=__ZTV9RabbitKey")

static int __fastcall rk_init(void *s, void *)
{ return _ZN9RabbitKey13InitResourcesEv(s); }
static int __fastcall rk_clean(void *, void *)
{ return _ZN9RabbitKey16CleanupResourcesEv(); }
static int __fastcall rk_behavior(void *s, void *)
{ return _ZN9RabbitKey8BehaviorEv(s); }
static int __fastcall rk_render(void *s, void *)
{ port_actor_render_probe("RABBIT_KEY", (char *)s + 0x110);
  return _ZN9RabbitKey6RenderEv(s); }
static int __fastcall rk_pdes(void *, void *)
{ _ZN9RabbitKey16OnPendingDestroyEv(); return 0; }
static int __fastcall rk_d1(void *s, void *)
{ return (int)(size_t)_ZN9RabbitKeyD1Ev((int *)s); }
static int __fastcall rk_d0(void *s, void *)
{ return (int)(size_t)_ZN9RabbitKeyD0Ev((int *)s); }

extern "C" void hal_fill_rabbit_key_vtable(void)
{
    void **vt = _ZTV9RabbitKey;
    ac_fill_shared(vt);
    vt[0] = (void *)rk_init;
    vt[3] = (void *)rk_clean;
    vt[6] = (void *)rk_behavior;
    vt[9] = (void *)rk_render;
    vt[12] = (void *)rk_pdes;
    vt[16] = (void *)rk_d1;
    vt[17] = (void *)rk_d0;
}

// ---- LAKITU_BRO (actor 235, ov085) x1 --------------------------------------
//
// _ZTV9LakituBro / _ZTV11daC_Jugem_c, ov085 0x02130344. The cameraman over
// the moat. He is here rather than deferred with the rest of the cutscene
// cast because what he reads is hosted: his own state machine places him
// against the LIVE Camera, the one gate 13 brought up.
//
// His Behavior is a HOST COPY (port/unmatched/LakituBro_Behavior.cpp) for the
// gate-16 reason -- it dispatches an mwcc pointer-to-member out of an eleven
// entry state table, and MSVC forms a PMF over an incomplete class as the
// four-word general representation.
extern "C" {
int _ZN9LakituBro13InitResourcesEv(void *self);   /* face: method_faces */
int _ZN9LakituBro8BehaviorEv(void *self);         /* host copy */
int _ZN9LakituBro6RenderEv(void *self);           /* face: method_faces */
int _ZN9LakituBro16CleanupResourcesEv(void);
void _ZN9LakituBro16OnPendingDestroyEv(void);
int *_ZN9LakituBroD1Ev(int *self);
int *_ZN9LakituBroD0Ev(int *self);
void *_ZTV9LakituBro[31];
}
#pragma comment(linker, "/alternatename:__ZTV11daC_Jugem_c=__ZTV9LakituBro")

static int __fastcall lb_init(void *s, void *)
{ return _ZN9LakituBro13InitResourcesEv(s); }
static int __fastcall lb_clean(void *, void *)
{ return _ZN9LakituBro16CleanupResourcesEv(); }
static int __fastcall lb_behavior(void *s, void *)
{ return _ZN9LakituBro8BehaviorEv(s); }
static int __fastcall lb_render(void *s, void *)
{ port_actor_render_probe("LAKITU_BRO", (char *)s + 0x110);
  return _ZN9LakituBro6RenderEv(s); }
static int __fastcall lb_pdes(void *, void *)
{ _ZN9LakituBro16OnPendingDestroyEv(); return 0; }
static int __fastcall lb_d1(void *s, void *)
{ return (int)(size_t)_ZN9LakituBroD1Ev((int *)s); }
static int __fastcall lb_d0(void *s, void *)
{ return (int)(size_t)_ZN9LakituBroD0Ev((int *)s); }

extern "C" void hal_fill_lakitu_bro_vtable(void)
{
    void **vt = _ZTV9LakituBro;
    ac_fill_shared(vt);
    vt[0] = (void *)lb_init;
    vt[3] = (void *)lb_clean;
    vt[6] = (void *)lb_behavior;
    vt[9] = (void *)lb_render;
    vt[12] = (void *)lb_pdes;
    vt[16] = (void *)lb_d1;
    vt[17] = (void *)lb_d0;
}

// ============================================================================
// GATE 19: ov098's CANNON
// ============================================================================
//
// _ZTV7daCnn_c, ov098 0x0213c6a0 (daCnn_c -- "cannon"). One instance, behind
// the grate in the west moat wall. The config's names are not shifted here
// either: slot 0 is Cannon::InitResources and slot 16 is Cannon's own D1.
//
// IT IS DORMANT ON PURPOSE. The cannon's own Behavior reads the save before
// it will open, so on a fresh save the lid stays shut -- which is what the
// level looks like before Bob-omb Buddy opens it, and the reason nothing here
// goes near the cannon-entry path.
//
// SLOTS 16/17 TRAP, the gate-17 reading twice over: nothing on this level
// destroys a cannon, and src/_ZN6CannonD1Ev.cpp is a real C++ destructor over
// its own shadow hierarchy rather than a C-named body, so hosting it would
// mean MSVC synthesising member destructor calls against a layout that is not
// the ROM's.
extern "C" {
int _ZN6Cannon13InitResourcesEv(void *self);      /* face: method_faces */
int _ZN6Cannon8BehaviorEv(void *self);            /* face: method_faces */
int _ZN6Cannon6RenderEv(void *self);              /* host copy */
int _ZN6Cannon16CleanupResourcesEv(void);
int *_ZN6CannonD0Ev(int *self);                /* slot 17, DTOR-PAIRS seat (ov098 0x0213a938) */
void *_ZTV6Cannon[31];
}
#pragma comment(linker, "/alternatename:__ZTV7daCnn_c=__ZTV6Cannon")

static int __fastcall cn_init(void *s, void *)
{ return _ZN6Cannon13InitResourcesEv(s); }
static int __fastcall cn_clean(void *, void *)
{ return _ZN6Cannon16CleanupResourcesEv(); }
static int __fastcall cn_behavior(void *s, void *)
{ return _ZN6Cannon8BehaviorEv(s); }
static int __fastcall cn_render(void *s, void *)
{ port_actor_render_probe("CANNON", (char *)s + 0xd4);
  return _ZN6Cannon6RenderEv(s); }
static int __fastcall cn_d0(void *s, void *)
{ return (int)(size_t)_ZN6CannonD0Ev((int *)s); }

extern "C" void hal_fill_cannon_vtable(void)
{
    void **vt = _ZTV6Cannon;
    ac_fill_shared(vt);
    vt[0] = (void *)cn_init;
    vt[3] = (void *)cn_clean;
    vt[6] = (void *)cn_behavior;
    vt[9] = (void *)cn_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)hal_cppd1_Cannon;
    /* Slot 17, the ROM's own deleting destructor (DTOR-PAIRS seat): the
       matched flat-C body behind the ecx->arg adapter, the
       lk2_platform_dtor_seat.cpp shape. Nothing on a mounted level
       dispatches 17 (AfterCleanupResources dispatches 16 and frees
       itself), so this is the ROM's word where the trap stood, and the
       reference edge that links the TU. */
    vt[17] = (void *)cn_d0;
}

// ============================================================================
// GATE 20: ov002's LAST TWO
// ============================================================================
//
// EXIT (349) and WATERFALL_MIST (197), the two classes the castle grounds
// names that live in the overlay already mounted. Neither has a model, so
// neither takes a render probe, and both are shorter than any class above:
// the Exit is a trigger box and the mist is three slots.
//
// THE CONFIG'S NAMES ARE SHIFTED HERE TOO, the other way round from ov009's.
// In ov009 the factories were named right and the methods were the
// neighbour's; in ov002 the *_SpawnInfo names are right and the METHODS carry
// the neighbour's class name. Each factory's own literal pool names the
// vtable it installs and the ROM's RTTI names that vtable back:
//
//   Exit_Spawn          -> _ZTV11VirtualDoor         RTTI 11daChScene_c
//   WaterfallMist_Spawn -> _ZTV18PoppingLavaBubbles  RTTI 16daObjWaterfall_c
//
// daChScene_c is "change scene", which is what an exit does, and
// daObjWaterfall_c is the waterfall. So the bodies below are spelled
// VirtualDoor and PoppingLavaBubbles and are the exit's and the mist's. The
// port uses the files the vtables point at and renames nothing.

// ---- EXIT (actor 349, ov002) x4 --------------------------------------------
//
// _ZTV11VirtualDoor, ov002 0x021086b4. The four level-transition volumes.
// Three of its six own slots are do-nothing returns in the ROM and the other
// three are the box: InitResources builds it and inverts the actor's matrix,
// Behavior tests the Player in that local frame, and the whole of Behavior's
// body sits behind `data_02092110 < 0` -- the pending-entrance slot the level
// boot leaves non-negative -- so on the grounds the four exits measure and
// never fire. That is the ROM's gate, not a port guard.
extern "C" {
int _ZN11VirtualDoor13InitResourcesEv(char *self);
int _ZN11VirtualDoor8BehaviorEv(char *self);
int _ZN11VirtualDoor6RenderEv(void);
int _ZN11VirtualDoor16CleanupResourcesEv(void);
void _ZN11VirtualDoor16OnPendingDestroyEv(void);
int *_ZN11VirtualDoorD0Ev(int *self);            /* slot 17, gate 207 */
void *_ZTV11VirtualDoor[31];
}

static int __fastcall ex_init(void *s, void *)
{ return _ZN11VirtualDoor13InitResourcesEv((char *)s); }
static int __fastcall ex_clean(void *, void *)
{ return _ZN11VirtualDoor16CleanupResourcesEv(); }
static int __fastcall ex_behavior(void *s, void *)
{ return _ZN11VirtualDoor8BehaviorEv((char *)s); }
static int __fastcall ex_render(void *, void *)
{ return _ZN11VirtualDoor6RenderEv(); }
static int __fastcall ex_pdes(void *, void *)
{ _ZN11VirtualDoor16OnPendingDestroyEv(); return 0; }
static int __fastcall ex_d0(void *s, void *)
{ return (int)(size_t)_ZN11VirtualDoorD0Ev((int *)s); }

extern "C" void hal_fill_exit_vtable(void)
{
    void **vt = _ZTV11VirtualDoor;
    ac_fill_shared(vt);
    vt[0] = (void *)ex_init;
    vt[3] = (void *)ex_clean;
    vt[6] = (void *)ex_behavior;
    vt[9] = (void *)ex_render;
    vt[12] = (void *)ex_pdes;
    /* SLOT 16 STILL STANDS IN, SLOT 17 IS NOW THE ROM'S OWN D0 (gate 207).
       The gate-17 reading above holds for the D1 and only for the D1:
       src/_ZN11VirtualDoorD1Ev.cpp is a real C++ destructor over its own
       shadow hierarchy, which MSVC would emit under ??1VirtualDoor@@UAE@XZ
       against a layout that is not the ROM's. The D0 is a flat .c whose only
       placeholders are VT and HEAP. Gate 204's three conditions all hold and
       were re-measured on this build; see port/slice_gate207.txt. */
    vt[16] = (void *)hal_cppd1_VirtualDoor;   /* lane DTOR-FACES-CPP: the matched D1 */
    vt[17] = (void *)ex_d0;
}

// ---- WATERFALL_MIST (actor 197, ov002) x7 ----------------------------------
//
// _ZTV18PoppingLavaBubbles, ov002 0x021094a0. The moat's waterfall spray, and
// the first class the port carries that the LEVEL'S OBJECT TABLES DO NOT
// NAME: CASTLE_WATER's own InitResources ends by calling func_ov009_02111b1c,
// which spawns seven of these along one of three seven-element Vector3 arrays
// in ov009. The seven skips gate 19's census reported were that call.
//
// Three own slots; 3, 9 and 12 are ActorBase's, because the class has no
// model and nothing to clean up. InitResources is a real MSVC method (face in
// hal/method_faces.cpp) and picks a particle id from the level -- 0x24 here.
//
// ITS BEHAVIOR RUNS INTO A STUB and that is honest rather than hidden:
// Particle::System::New is the no-op in hal/reverse_bridges.cpp until the
// particle subsystem is seated, so the seven actors spawn at the ROM's own
// positions, sit in both lists and tick. Every one of their eighteen slots is
// matched src; nothing here changes when the particles arrive.
#include "PoppingLavaBubbles.h"
extern "C" {
int _ZN18PoppingLavaBubbles13InitResourcesEv(void *self);   /* face */
int _ZN18PoppingLavaBubbles8BehaviorEv(char *self);
int *_ZN18PoppingLavaBubblesD0Ev(int *self);     /* slot 17, gate 207 */
void *_ZTV18PoppingLavaBubbles[31];
}

static int __fastcall wm_init(void *s, void *)
{ return _ZN18PoppingLavaBubbles13InitResourcesEv(s); }
static int __fastcall wm_clean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall wm_behavior(void *s, void *)
{ return _ZN18PoppingLavaBubbles8BehaviorEv((char *)s); }
static int __fastcall wm_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall wm_d0(void *s, void *)
{ return (int)(size_t)_ZN18PoppingLavaBubblesD0Ev((int *)s); }

extern "C" void hal_fill_waterfall_mist_vtable(void)
{
    void **vt = _ZTV18PoppingLavaBubbles;
    ac_fill_shared(vt);
    vt[0] = (void *)wm_init;
    vt[3] = (void *)wm_clean;
    vt[6] = (void *)wm_behavior;
    vt[9] = (void *)wm_render;
    vt[12] = (void *)ac_pdes_base;
    /* The class here is PoppingLavaBubbles, not WaterfallMist: the vtable
       WaterfallMist_Spawn installs is _ZTV18PoppingLavaBubbles, and that
       class's D1 is an empty body over an Actor base -- which is what this
       section already says about slots 3, 9 and 12. Slot 17 is that class's
       own matched D0 as of gate 207, on gate 204's three conditions
       re-measured against this build; see port/slice_gate207.txt. The name
       shift holds there too: the ROM word at 0x021094a0+0x44 is
       _ZN18PoppingLavaBubblesD0Ev, not WaterfallMist's. */
    vt[16] = (void *)hal_cppd1_PoppingLavaBubbles;   /* lane DTOR-FACES-CPP */
    vt[17] = (void *)wm_d0;
}

// ============================================================================
// GATE 21: ov100's BUTTERFLY and FISH
// ============================================================================
//
// The third ACTOR overlay. ov100 holds seven classes and the castle grounds
// names three; these are the two whose own methods need nothing the port
// cannot already spell.
//
// THE CONFIG'S NAMES CHECK OUT FOR BOTH, and that is checked rather than
// assumed -- ov100 does carry one shifted pair. Butterfly_Spawn installs
// _ZTV9Butterfly (RTTI 9daBtfly_c) and Fish_Spawn installs _ZTV4Fish (RTTI
// 8daFish_c); the shift is Door/StarDoor, which is gate 22's.

// ---- BUTTERFLY (actor 336, ov100) x2 ---------------------------------------
//
// _ZTV9Butterfly / _ZTV9daBtfly_c, ov100 0x02147e9c. The two flutterers over
// the east lawn. Eight states, all matched, seated in hal/actor_overlays.cpp;
// its Behavior is a plain C-named body that reads the state pair as two ints
// itself, so no host copy is needed (the rabbit's case, not the fish's).
//
// Render dispatches slot 5 through a LOCAL SHADOW CLASS on both the ModelAnim
// at +0xd4 and the Model at +0x138 -- ROM numbering, where two slots go to the
// destructor -- which is exactly what the dual fill in hal/cxxname_bridge.cpp
// already covers on both classes. Slot 3 never comes into it, so unlike the
// cannon this one needs no host copy of Render either.
extern "C" {
int _ZN9Butterfly13InitResourcesEv(void *self);      /* face: method_faces */
int _ZN9Butterfly8BehaviorEv(char *self);
int _ZN9Butterfly6RenderEv(void *self);              /* face: method_faces */
int _ZN9Butterfly16CleanupResourcesEv(void);
void _ZN9Butterfly16OnPendingDestroyEv(void);
int *_ZN9ButterflyD0Ev(int *self);             /* slot 17, DTOR-PAIRS seat (ov100 0x02140dd8) */
void *_ZTV9Butterfly[31];
}
#pragma comment(linker, "/alternatename:__ZTV9daBtfly_c=__ZTV9Butterfly")

static int __fastcall bf_init(void *s, void *)
{ return _ZN9Butterfly13InitResourcesEv(s); }
static int __fastcall bf_clean(void *, void *)
{ return _ZN9Butterfly16CleanupResourcesEv(); }
static int __fastcall bf_behavior(void *s, void *)
{ return _ZN9Butterfly8BehaviorEv((char *)s); }
static int __fastcall bf_render(void *s, void *)
{ port_actor_render_probe("BUTTERFLY", (char *)s + 0xd4);
  return _ZN9Butterfly6RenderEv(s); }
static int __fastcall bf_pdes(void *, void *)
{ _ZN9Butterfly16OnPendingDestroyEv(); return 0; }
/* SLOT 16 IS LIVE, the bird's discovery applied here before it fires: three
   of the eight states (0, 3 and 7) call MarkForDestruction, so a woken
   swarm despawns through AfterCleanupResources -> D1. The body is
   src/_ZN9ButterflyD0Ev.c minus its final Memory::Deallocate; the six
   members in the ROM's own reverse order, not MSVC's. */
/* slot 16: the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP). */

static int __fastcall bf_d0(void *s, void *)
{ return (int)(size_t)_ZN9ButterflyD0Ev((int *)s); }

extern "C" void hal_fill_butterfly_vtable(void)
{
    void **vt = _ZTV9Butterfly;
    ac_fill_shared(vt);
    vt[0] = (void *)bf_init;
    vt[3] = (void *)bf_clean;
    vt[6] = (void *)bf_behavior;
    vt[9] = (void *)bf_render;
    vt[12] = (void *)bf_pdes;
    vt[16] = (void *)hal_cppd1_Butterfly;
    /* Slot 17, the ROM's own deleting destructor (DTOR-PAIRS seat): the
       matched flat-C body behind the ecx->arg adapter, the
       lk2_platform_dtor_seat.cpp shape. Nothing on a mounted level
       dispatches 17 (AfterCleanupResources dispatches 16 and frees
       itself), so this is the ROM's word where the trap stood, and the
       reference edge that links the TU. */
    vt[17] = (void *)bf_d0;
}

// ---- FISH (actor 344, ov100) x2 --------------------------------------------
//
// _ZTV4Fish / _ZTV8daFish_c, ov100 0x021484bc. The two shoals in the moat.
//
// THE FISH IS THE FIRST CLASS THE PORT CARRIES THAT CAN KILL ITSELF. Its
// Behavior looks its owner up by actor id every frame and calls
// ActorBase::MarkForDestruction when the owner is gone or refuses it -- which
// is what binds a shoal to the water it was spawned against rather than to a
// position. That makes slots 16/17 reachable in principle, so the trap there
// is a real statement and not a formality: on the castle grounds the owner is
// CASTLE_WATER, which lives as long as the level does, and the census is the
// measurement (two fish in, two fish still on the lists at frame 3000).
//
// Its Behavior is a host copy for the pointer-to-member reason
// (port/unmatched/Fish_Behavior.cpp), which also carries the seven-static
// seat. Render is the same slot-5 shadow dispatch the butterfly has.
extern "C" {
int _ZN4Fish13InitResourcesEv(void *self);           /* face: method_faces */
int _ZN4Fish8BehaviorEv(void *self);                 /* host copy */
int _ZN4Fish6RenderEv(void *self);                   /* face: method_faces */
int _ZN4Fish16CleanupResourcesEv(void *self);        /* face: method_faces */
void _ZN4Fish16OnPendingDestroyEv(void);
int *_ZN4FishD0Ev(int *self);                  /* slot 17, DTOR-PAIRS seat (ov100 0x0214623c) */
void *_ZTV4Fish[31];
}
#pragma comment(linker, "/alternatename:__ZTV8daFish_c=__ZTV4Fish")

static int __fastcall fs_init(void *s, void *)
{ return _ZN4Fish13InitResourcesEv(s); }
static int __fastcall fs_clean(void *s, void *)
{ return _ZN4Fish16CleanupResourcesEv(s); }
static int __fastcall fs_behavior(void *s, void *)
{ return _ZN4Fish8BehaviorEv(s); }
static int __fastcall fs_render(void *s, void *)
{ port_actor_render_probe("FISH", (char *)s + 0xd4);
  return _ZN4Fish6RenderEv(s); }
static int __fastcall fs_pdes(void *, void *)
{ _ZN4Fish16OnPendingDestroyEv(); return 0; }
/* SLOT 16 IS LIVE, the class's own comment above already said so: the owner
   test can MarkForDestruction any frame. src/_ZN4FishD0Ev.c minus the
   Deallocate -- one ModelAnim, then Actor's D2. */
/* slot 16: the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP). */

static int __fastcall fs_d0(void *s, void *)
{ return (int)(size_t)_ZN4FishD0Ev((int *)s); }

extern "C" void hal_fill_fish_vtable(void)
{
    void **vt = _ZTV4Fish;
    ac_fill_shared(vt);
    vt[0] = (void *)fs_init;
    vt[3] = (void *)fs_clean;
    vt[6] = (void *)fs_behavior;
    vt[9] = (void *)fs_render;
    vt[12] = (void *)fs_pdes;
    vt[16] = (void *)hal_cppd1_Fish;
    /* Slot 17, the ROM's own deleting destructor (DTOR-PAIRS seat): the
       matched flat-C body behind the ecx->arg adapter, the
       lk2_platform_dtor_seat.cpp shape. Nothing on a mounted level
       dispatches 17 (AfterCleanupResources dispatches 16 and frees
       itself), so this is the ROM's word where the trap stood, and the
       reference edge that links the TU. */
    vt[17] = (void *)fs_d0;
}

// ---- UNCHAINED_CHOMP (actor 337, ov100) ------------------------------------
//
// _ZTV14UnchainedChomp, ov100 (SpawnInfo at 0x02148020). The obstacle BoB star 3
// (5 Silver Stars) needs; census-skipped today with no row here, not a crash.
//
// Behavior is a host copy for the same pointer-to-member reason Fish's is
// (port/unmatched/UnchainedChomp_Behavior.cpp), the simplest ov100 PMF table: a
// single Holder* at +0x668, not an N-state array. Render is the same ModelAnim
// slot-5 shadow dispatch, off mModelAnim at +0x30c (ModelAnim_Renders.cpp).
extern "C" {
int _ZN14UnchainedChomp13InitResourcesEv(unsigned char *self);  /* host copy (PathPtr thiscall) */
int _ZN14UnchainedChomp8BehaviorEv(void *self);                 /* host copy */
int _ZN14UnchainedChomp6RenderEv(void *self);                   /* host copy */
int _ZN14UnchainedChomp16CleanupResourcesEv(void);
void _ZN14UnchainedChomp16OnPendingDestroyEv(void);
void *_ZTV14UnchainedChomp[31];
/* D1 teardown helpers (the other classes' D1 blocks use these too) */
void __destroy_arr(void *p, int n, int sz, void *dtor);
void func_02011508(void);
void func_020072c0(void);
void func_ov002_020aed18(void *);   /* the Enemy base D2 (ov002), already hosted */
}

static int __fastcall uc_init(void *s, void *)
{ return _ZN14UnchainedChomp13InitResourcesEv((unsigned char *)s); }
static int __fastcall uc_clean(void *, void *)
{ return _ZN14UnchainedChomp16CleanupResourcesEv(); }
static int __fastcall uc_behavior(void *s, void *)
{ return _ZN14UnchainedChomp8BehaviorEv(s); }
static int __fastcall uc_render(void *s, void *)
{ port_actor_render_probe("UNCHAINED_CHOMP", (char *)s + 0x30c);
  return _ZN14UnchainedChomp6RenderEv(s); }
static int __fastcall uc_pdes(void *, void *)
{ _ZN14UnchainedChomp16OnPendingDestroyEv(); return 0; }
/* SLOT 16: the butterfly reading -- UnchainedChomp's matched Behavior never
   calls MarkForDestruction itself, but D1 is still reachable through ordinary
   AfterCleanupResources / level-unload teardown, so it is filled rather than
   trapped. Body is src/_ZN14UnchainedChompD1Ev.cpp line for line (D1, not D0:
   no final Deallocate). func_ov002_020aed18 is the shared Enemy base D2. */
static void *__fastcall uc_d1(void *s, void *)
{
    char *c = (char *)s;
    *(void **)c = (void *)_ZTV14UnchainedChomp;
    __destroy_arr(c + 0x768, 6, 6, (void *)func_02011508);
    __destroy_arr(c + 0x720, 6, 0xc, (void *)func_020072c0);
    __destroy_arr(c + 0x6d8, 6, 0xc, (void *)func_020072c0);
    _ZN11ShadowModelD1Ev(c + 0x640);
    __destroy_arr(c + 0x550, 6, 0x28, (void *)_ZN11ShadowModelD1Ev);
    __destroy_arr(c + 0x370, 6, 0x50, (void *)_ZN5ModelD1Ev);
    _ZN9ModelAnimD1Ev(c + 0x30c);
    _ZN12WithMeshClsnD1Ev(c + 0x150);
    _ZN25MovingCylinderClsnWithPosD1Ev(c + 0x110);
    func_ov002_020aed18(c);
    return c;
}

extern "C" void hal_fill_unchained_chomp_vtable(void)
{
    void **vt = _ZTV14UnchainedChomp;
    ac_fill_shared(vt);
    vt[0] = (void *)uc_init;
    vt[3] = (void *)uc_clean;
    vt[6] = (void *)uc_behavior;
    vt[9] = (void *)uc_render;
    vt[12] = (void *)uc_pdes;
    vt[16] = (void *)uc_d1;
    /* 17 keeps the trap, the Butterfly/Fish reading: destroy is D1 + an explicit
       Deallocate; nothing on this level calls the deleting form. */
    vt[17] = (void *)ac_trap17;
    /* 31 slots, a plain Actor table. Slot 29 (OnAimedAtWithEgg) is the chomp's
       own body (ov100 0x021442d4), matched in src and in no slice, so it
       declines rather than answering Actor's default 20.0 lock-on radius. */
    vt[29] = (void *)ac_trap29;
}

// ============================================================================
// GATE 22: ov100's DOOR
// ============================================================================
//
// _ZTV8daDoor_c, ov100 0x02148188 -- the table config left as
// data_ov100_02148188 while handing `_ZTV4Door` to the STAR door, whose RTTI
// reads daStarGate_c. So the whole _ZN4Door* method family in src/ implements
// the star door and the real door's are the unnamed func_ov100_* block; the
// port compiles the files the vtable points at and renames nothing.
//
// The three castle doors come out of LoadDoorObjects rather than the standard
// or simple loader, which is why they carry no record in the object tables
// the other classes were read against -- the level names them as its kind
// 0x09, count 3.
//
// Both its Behavior and its Render are host copies, for the two reasons this
// gate has met all the way through: a pointer-to-member table (this time a
// POINTER THE ACTOR CARRIES at +0x140, not a fixed array) and a shadow-class
// dispatch that needs Model SLOT 4, the one the dual fill cannot serve. See
// port/unmatched/Door_Behavior.cpp and Door_Render.cpp.
extern "C" {
int func_ov100_021455a0(void *self);      /* InitResources */
int func_ov100_0214542c(void *self);      /* CleanupResources */
int func_ov100_02145550(void *self);      /* Behavior -- host copy */
int func_ov100_021454c8(void *self);      /* Render   -- host copy */
void func_ov100_021454c4(void);           /* OnPendingDestroy */
void *_ZTV8daDoor_c[31];   /* vtspan: data_ov100_02148188 */
}

static int __fastcall dr_init(void *s, void *)
{ return func_ov100_021455a0(s); }
static int __fastcall dr_clean(void *s, void *)
{ return func_ov100_0214542c(s); }
static int __fastcall dr_behavior(void *s, void *)
{ return func_ov100_02145550(s); }
static int __fastcall dr_render(void *s, void *)
{ port_actor_render_probe("DOOR", (char *)s + 0xd4);
  return func_ov100_021454c8(s); }
static int __fastcall dr_pdes(void *, void *)
{ func_ov100_021454c4(); return 0; }

extern "C" void hal_fill_door_vtable(void)
{
    void **vt = _ZTV8daDoor_c;
    ac_fill_shared(vt);
    vt[0] = (void *)dr_init;
    vt[3] = (void *)dr_clean;
    vt[6] = (void *)dr_behavior;
    vt[9] = (void *)dr_render;
    vt[12] = (void *)dr_pdes;
    /* SLOTS 16/17 TRAP, the gate-17 reading: nothing on the castle grounds
       destroys a door -- the three live from the level boot to the teardown. */
    vt[16] = (void *)ac_d1_door;
    vt[17] = (void *)ac_trap17;
}

// ============================================================================
// GATE 40: ov100's STAR_DOOR (actor 354) -- the config's _ZN4Door* family
// ============================================================================
//
// _ZTV4Door / _ZTV12daStarGate_c, ov100 0x021483cc. Gate 22 found that config
// swapped the two doors' names: the vtable config left as data_ov100_02148188
// (RTTI 8daDoor_c) is the REAL door gate 22 hosts, and config's `_ZTV4Door`
// is a SECOND table (RTTI 12daStarGate_c) with the whole _ZN4Door* method
// family. So this fills the star door, whose Spawn installs _ZTV4Door and
// whose own D0 spells the same table _ZTV12daStarGate_c -- the RABBIT's
// dual-name case, so both spellings resolve to one host array.
//
// Its member is a CommonModel at 0xd4, the real door's layout, so slot 16
// reuses ac_d1_door (member D2 then Actor's D2). D0 is plain C in src.
// Nothing here is a host copy: both InitResources' and Behavior's
// pointer-to-member sites are formed over COMPLETE local classes and both are
// guarded off on a fresh boot (the callback table data_ov100_02148974 is
// ov100 .bss, zeroed) -- see the head of slice_gate40.txt.
extern "C" {
int _ZN4Door13InitResourcesEv(void *self);      /* face: method_faces */
int _ZN4Door8BehaviorEv(void *self);            /* face: method_faces */
int _ZN4Door6RenderEv(int self);                /* C in src */
int _ZN4Door16CleanupResourcesEv(void);         /* C in src */
void _ZN4Door16OnPendingDestroyEv(void);        /* C in src */
int *_ZN4DoorD0Ev(int *self);                   /* C in src */
void *_ZTV4Door[31];
}
#pragma comment(linker, "/alternatename:__ZTV12daStarGate_c=__ZTV4Door")

static int __fastcall sd_init(void *s, void *)
{ return _ZN4Door13InitResourcesEv(s); }
static int __fastcall sd_clean(void *, void *)
{ return _ZN4Door16CleanupResourcesEv(); }
static int __fastcall sd_behavior(void *s, void *)
{ return _ZN4Door8BehaviorEv(s); }
static int __fastcall sd_render(void *s, void *)
{ port_actor_render_probe("STAR_DOOR", (char *)s + 0xd4);
  return _ZN4Door6RenderEv((int)(size_t)s); }
static int __fastcall sd_pdes(void *, void *)
{ _ZN4Door16OnPendingDestroyEv(); return 0; }
static int __fastcall sd_d0(void *s, void *)
{ return (int)(size_t)_ZN4DoorD0Ev((int *)s); }

extern "C" void hal_fill_star_door_vtable(void)
{
    void **vt = _ZTV4Door;
    ac_fill_shared(vt);
    vt[0] = (void *)sd_init;
    vt[3] = (void *)sd_clean;
    vt[6] = (void *)sd_behavior;
    vt[9] = (void *)sd_render;
    vt[12] = (void *)sd_pdes;
    /* slot 16: the member chain is the real door's (CommonModel at 0xd4), so
       ac_d1_door serves it. slot 17 is the class's own C D0. */
    vt[16] = (void *)hal_cppd1_Door;   /* lane DTOR-FACES-CPP: the ROM word 16 of _ZTV4Door */
    vt[17] = (void *)sd_d0;
}

// ============================================================================
// GATE 41: ov010's TRAP (actor 36) and LIGHT_BEAM (actor 37, shares this table)
// ============================================================================
//
// _ZTV4Trap, ov010 0x02112ba8. The castle interior's traps and the light beam
// through the entrance. LIGHT_BEAM is a variant of TRAP: its own Spawn
// installs THIS SAME vtable (_ZTV4Trap) and it has no methods of its own, so
// one fill serves both classes. The table wears three names -- config's
// _ZTV4Trap, LIGHT_BEAM's RTTI _ZTV15daObjC1Hikari_c (hikari = light) at the
// same address, and TRAP's own RTTI _ZTV14daObjC1_Trap_c that Trap_Spawn and
// Trap's D0 spell -- so all three resolve to one host array.
//
// TRAP is Platform-derived (Trap_Spawn runs Platform's constructor and a Model
// at 0x320), so the fill runs hal_fill_platform_vtable first, the SIGN_POST
// shape. Nothing here dispatches a pointer-to-member: both Behavior and Render
// are straight-line C++ methods.
//
// CLEANUP AND DESTRUCTOR (gate 31 reading applied). The old note here said
// slots 16/17 trap because nothing destroys a trap or the light beam on the
// castle-interior boot. That held only while there was no level teardown; a
// level change destroys every actor the level spawned through the ROM's own
// ActorBase::AfterCleanupResources, and both of these classes reach it. Two
// bugs sat in the path:
//
//   1. tr_clean swallowed CleanupResources' return. src returns 1
//      (SharedFilePtr released, done); AfterCleanupResources only unlinks and
//      destroys the node when cleanup reports done (func_0204322c maps 1 ->
//      code 2), so returning 0 left TRAP and LIGHT_BEAM nodes dangling at
//      teardown -- the stale-cleanup-node symptom of playlog 001951 (garden
//      door) and the render-walk use-after-free of playlog 002712 (BoB). Fixed
//      by forwarding the return.
//
//   2. Once cleanup reports done, AfterCleanupResources dispatches slot 16
//      (D1). The ROM vtable at 0x02112ba8 puts _ZN4TrapD1Ev in slot 16 and
//      _ZN4TrapD0Ev in slot 17, and BOTH classes share this one table -- but
//      the two have different layouts and the shared dtor is written for only
//      one of them. LIGHT_BEAM (LightBeam_Spawn, 364 bytes, Actor-derived) puts
//      its Model at +0xd4 and a MovingCylinderClsnWithPos at +0x124, which is
//      exactly the member chain _ZN4TrapD0Ev tears down. TRAP (Trap_Spawn, 944
//      bytes, Platform-derived) puts its Model at +0x320 and has no
//      MovingCylinderClsnWithPos at all, so running the 0xd4/0x124 chain on a
//      TRAP would smash Platform's fields. So slot 16 (tr_d1) discriminates on
//      the actor id at +0xc: LIGHT_BEAM (0x25) runs the real member chain,
//      TRAP (0x24) does the Actor-level teardown only. Slot 17 keeps the trap,
//      the sibling convention: AfterCleanupResources dispatches 16 and does the
//      Deallocate itself, so a landing on the deleting form (17) means someone
//      reached the actor through operator delete and deserves the abort.
extern "C" {
int _ZN4Trap13InitResourcesEv(void *self);        /* face: method_faces */
int _ZN4Trap8BehaviorEv(void *self);              /* face: method_faces */
int _ZN4Trap6RenderEv(void *self);                /* face: method_faces */
int _ZN4Trap16CleanupResourcesEv(void);           /* C in src, returns 1 */
int *_ZN4TrapD0Ev(int *self);                  /* slot 17, DTOR-PAIRS seat (ov010 0x02111a08) */
void *_ZTV4Trap[31];
void hal_fill_platform_vtable(void);
}
#pragma comment(linker, "/alternatename:__ZTV15daObjC1Hikari_c=__ZTV4Trap")
#pragma comment(linker, "/alternatename:__ZTV14daObjC1_Trap_c=__ZTV4Trap")

static int __fastcall tr_init(void *s, void *)
{ return _ZN4Trap13InitResourcesEv(s); }
static int __fastcall tr_clean(void *, void *)
{ return _ZN4Trap16CleanupResourcesEv(); }
static int __fastcall tr_behavior(void *s, void *)
{ return _ZN4Trap8BehaviorEv(s); }
static int __fastcall tr_render(void *s, void *)
{ port_actor_render_probe("TRAP", (char *)s + 0x320);
  return _ZN4Trap6RenderEv(s); }

/* Slot 16, the D1 (complete-object) destructor both classes share through the
   one _ZTV4Trap table. The hosted slot 16 is the ROM D0's body minus its final
   Memory::Deallocate, which AfterCleanupResources performs itself after the
   dispatch returns (the file's gate-31 recipe). The ROM's _ZN4TrapD0Ev is
   written for LIGHT_BEAM's layout only -- Model at +0xd4, MovingCylinderClsnWithPos
   at +0x124, then Actor's D2 -- so we can only run that chain when the object
   really is a LIGHT_BEAM. Discriminate on the actor id at +0xc. */
static int __fastcall tr_d1(void *s, void *)
{
    unsigned id = *(unsigned short *)((char *)s + 0xc);
    if (id == 0x25) {                 /* LIGHT_BEAM: the layout the ROM dtor fits */
        _ZN25MovingCylinderClsnWithPosD1Ev((char *)s + 0x124);
        _ZN5ModelD1Ev((char *)s + 0xd4);
    }
    /* TRAP (0x24): Actor-level teardown only. The ROM's 0xd4/0x124 chain is
       LIGHT_BEAM's and would smash Platform's fields on a 944-byte TRAP, whose
       Model lives at +0x320. LOUD LEAK: TRAP's Model at +0x320 is NOT torn down
       here -- Trap_Spawn constructs it (Model::Model at +0x320) and nothing
       else knows to free it, so a level change leaks one Model per castle-interior
       trap. That is a bounded, non-corrupting leak; running the wrong-offset
       chain to avoid it would corrupt. Left for whoever recovers TRAP's own D0
       (the ROM shares _ZN4TrapD0Ev across both layouts, so there is no separate
       matched TRAP dtor to call). */
    return (int)(size_t)_ZN5ActorD2Ev(s);
}

static int __fastcall tr_d0(void *s, void *)
{ return (int)(size_t)_ZN4TrapD0Ev((int *)s); }

extern "C" void hal_fill_trap_vtable(void)
{
    void **vt = _ZTV4Trap;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)tr_init;
    vt[3] = (void *)tr_clean;
    vt[6] = (void *)tr_behavior;
    vt[9] = (void *)tr_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)tr_d1;
    /* Slot 17, the ROM's own deleting destructor (DTOR-PAIRS seat). The ROM
       parks ONE body, _ZN4TrapD0Ev (ov010 0x02111a08), at word 17 of this
       table for both ids that install it, and that body runs the LIGHT_BEAM
       chain (MovingCylinderClsnWithPos +0x124, Model +0xd4) -- the same fact
       tr_d1 above discriminates on for slot 16. Seating it is the ROM's word
       where the trap stood; nothing dispatches 17 on a mounted level
       (AfterCleanupResources dispatches 16 and frees itself), so the
       TRAP-layout caveat in tr_d1 stays tr_d1's. */
    vt[17] = (void *)tr_d0;
}

// ============================================================================
// GATE 42: ov010's PEACH_PAINTING (actor 38)
// ============================================================================
//
// _ZTV13PeachPainting / _ZTV14daObjC1Peach_c, ov010 0x02112c68. Peach's
// portrait over the castle-interior stairs. A prior sweep marked it BLOCKED on
// ov052 and a RollingRock spawn; the ROM disagrees, and it was the shared-base
// illusion the gate-40/41 readings already turn on. ov010 and ov052 (and ov021)
// are all linked at 0x021111a0 because the DS only ever has one overlay of a
// base loaded, so an address in one names a symbol in every other. dsd
// attributed two of PeachPainting's OWN internal references to the ov052/ov021
// names at the shared address:
//
//   * InitResources' `data_ov052_02111e84(this)` is an arm_call
//     to:0x02111e84 module:overlay(10) -- func_ov010_02111e84, a three-line
//     matrix/scale helper compiled in slice_gate42.txt. ov052's 0x02111e84 is a
//     CLPS collision table (first word "CLPS"); calling it would crash.
//   * The `RollingRock_Spawn` handed to Model::LoadFile is a load reloc
//     to:0x02112d64 module:overlay(10) -- data_ov010_02112d64, an 8-byte BSS
//     SharedFilePtr the ov010 sinits build, already mounted in ov010_syms.txt.
//     CleanupResources names data_ov010_02112d64 directly, the cross-check.
//
// So there is no cross-overlay spawn and no RollingRock; ov052 is NOT mounted.
// The two mislabeled names are closed by /alternatename below (data_ov052 ->
// func_ov010, RollingRock_Spawn -> data_ov010's BSS SharedFilePtr).
//
// The vtable is the standard 31-slot Actor table (0x7c bytes): slot 12 is
// ActorBase::OnPendingDestroy (ac_pdes_base, same word 0x02043ac0 as TRAP),
// slots 0/3/6/9 the class's own four, slot 16 the D1 (Model at 0xd4 then
// Actor::~Actor) and slot 17 the class's own C D0. Nothing dispatches a
// pointer-to-member. PeachPainting_Spawn installs _ZTV13PeachPainting and D0
// spells the dual name _ZTV14daObjC1Peach_c, so both resolve to one host array.
extern "C" {
int _ZN13PeachPainting13InitResourcesEv(void *self);   /* face: method_faces */
int _ZN13PeachPainting8BehaviorEv(void *self);         /* face: method_faces */
int _ZN13PeachPainting6RenderEv(void *self);           /* face: method_faces */
int _ZN13PeachPainting16CleanupResourcesEv(void);      /* C in src */
int *_ZN13PeachPaintingD0Ev(int *self);                /* C in src */
void *_ZTV13PeachPainting[31];
/* the D1 member chain: Model at 0xd4, then Actor::~Actor. */
void _ZN5ModelD1Ev(void *);
void *_ZN5ActorD2Ev(void *);
}
#pragma comment(linker, "/alternatename:__ZTV14daObjC1Peach_c=__ZTV13PeachPainting")

static int __fastcall pt_init(void *s, void *)
{ return _ZN13PeachPainting13InitResourcesEv(s); }
static int __fastcall pt_clean(void *, void *)
{ return _ZN13PeachPainting16CleanupResourcesEv(); }
static int __fastcall pt_behavior(void *s, void *)
{ return _ZN13PeachPainting8BehaviorEv(s); }
static int __fastcall pt_render(void *s, void *)
{ port_actor_render_probe("PEACH_PAINTING", (char *)s + 0xd4);
  return _ZN13PeachPainting6RenderEv(s); }
/* slot 16: the class's D1 is a shadow-class C++ destructor in src, so it is not
   compiled; the chain is spelled here (Model at 0xd4 then Actor::~Actor), the
   gate-31 recipe for a shadow-class slot 16. */
/* slot 16: the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP). */
static int __fastcall pt_d0(void *s, void *)
{ return (int)(size_t)_ZN13PeachPaintingD0Ev((int *)s); }

extern "C" void hal_fill_peach_painting_vtable(void)
{
    void **vt = _ZTV13PeachPainting;
    ac_fill_shared(vt);
    vt[0] = (void *)pt_init;
    vt[3] = (void *)pt_clean;
    vt[6] = (void *)pt_behavior;
    vt[9] = (void *)pt_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)hal_cppd1_PeachPainting;
    vt[17] = (void *)pt_d0;
}

// ============================================================================
// GATE 23: ov102's QUESTION_BLOCK
// ============================================================================
//
// _ZTV13QuestionBlock / _ZTV18daObjHatenaBlock_c, ov102 0x0214e47c -- hatena
// is the question mark, which is the ROM's own RTTI agreeing with config's
// name for once. One instance, at (0,3498,-3500): the block on the castle
// roof, over the tower the cannon shoots you to.
//
// NOTHING HERE NEEDS A HOST COPY, which makes it the first class since gate 17
// that is only its own six slots. Its Behavior dispatches no pointer-to-member
// at all -- a straight-line body over a MovingMeshCollider and an Animation --
// and its Render is the slot-5 shadow dispatch on both ModelAnim and Model,
// which the dual fill in hal/cxxname_bridge.cpp already serves on both. The
// door's slot-4 problem does not arise.
//
// Slot 12 is ActorBase's own OnPendingDestroy, which is what the ROM's table
// holds -- the class does not override it.
extern "C" {
int _ZN13QuestionBlock13InitResourcesEv(char *self);
int _ZN13QuestionBlock8BehaviorEv(void *self);          /* face: method_faces */
int _ZN13QuestionBlock6RenderEv(void *self);            /* face: method_faces */
int _ZN13QuestionBlock16CleanupResourcesEv(void *self); /* face: method_faces */
int *_ZN13QuestionBlockD0Ev(int *self);        /* slot 17, DTOR-PAIRS seat (ov102 0x02149010) */
void *_ZTV13QuestionBlock[32];
}
#pragma comment(linker, "/alternatename:__ZTV18daObjHatenaBlock_c=__ZTV13QuestionBlock")

static int __fastcall qb_init(void *s, void *)
{ return _ZN13QuestionBlock13InitResourcesEv((char *)s); }
static int __fastcall qb_clean(void *s, void *)
{ return _ZN13QuestionBlock16CleanupResourcesEv(s); }
static int __fastcall qb_behavior(void *s, void *)
{ return _ZN13QuestionBlock8BehaviorEv(s); }
/* THE PROBE HAS TO PICK THE MEMBER THE ROM'S OWN Render PICKS, and it used to
   name +0x320 for all six classes that share this vtable. That is the
   ModelAnim, and src/_ZN13QuestionBlock6RenderEv.cpp:23-30 only draws it when
   `mActorId == 0x14` -- the plain QUESTION_BLOCK. Every other id (21
   EXCLAMATION_BLOCK, 22 EXCLAMATION_BLOCK_VS, 23/24/25 the three CAP_BLOCKs)
   draws mModel at +0xd4, and InitResources never touches +0x320 for them
   (src/_ZN13QuestionBlock13InitResourcesEv.c:26-46: only case 0 does).

   So the probe was reading a member the class legitimately leaves zeroed and
   reporting `file 00000000 transforms 00000000` on a block that was drawing
   perfectly well. Run vsmap, chasing "the VS ! blocks do not render": the null
   was the instrument's, not the game's. Reading +0xd4 for those ids is what the
   probe was always supposed to say. */
static int __fastcall qb_render(void *s, void *)
{ port_actor_render_probe("QUESTION_BLOCK",
                          (char *)s + (*(unsigned short *)((char *)s + 0xc) ==
                                       0x14 ? 0x320 : 0xd4));
  return _ZN13QuestionBlock6RenderEv(s); }

/* GATE 203: the block's own five HIT slots, ov102, all matched src and now
   sliced (port/slice_gate203.txt). Every one of them is `(self, other)` on the
   ROM -- the caller PUSHES the other actor -- so every veneer here takes the
   THREE-parameter __fastcall shape (self in ecx, the dummy in edx, the
   argument pushed) and MSVC emits `ret 4` for it. A two-parameter veneer would
   emit a bare `ret`, pop nothing, and desync the caller's frame: that exact
   mistake on this exact slot 28 is what hal/actor_classes_ov072.cpp records as
   a live stack smash. The emitted `C2 04 00` is checked against the LINKED
   binary, not inferred from these signatures. */
extern "C" {
void func_ov102_021496a4(void *self, void *other);  /* 28 OnHitFromUnderneath */
void func_ov102_02149710(void *self, void *player); /* 27 OnHitByMegaChar     */
void func_ov102_02149770(void *self, void *other);  /* 24 OnKicked            */
void func_ov102_021497c8(void *self, void *other);  /* 22 OnAttacked1         */
void func_ov102_02149820(void *self, void *other);  /* 21 OnGroundPounded     */
}
static int __fastcall qb_under(void *s, void *, void *o)
{ func_ov102_021496a4(s, o); return 0; }
static int __fastcall qb_mega(void *s, void *, void *p)
{ func_ov102_02149710(s, p); return 0; }
static int __fastcall qb_kicked(void *s, void *, void *o)
{ func_ov102_02149770(s, o); return 0; }
static int __fastcall qb_atk1(void *s, void *, void *o)
{ func_ov102_021497c8(s, o); return 0; }
static int __fastcall qb_pounded(void *s, void *, void *o)
{ func_ov102_02149820(s, o); return 0; }

static int __fastcall qb_d0(void *s, void *)
{ return (int)(size_t)_ZN13QuestionBlockD0Ev((int *)s); }

extern "C" void hal_fill_question_block_vtable(void)
{
    void **vt = _ZTV13QuestionBlock;
    ac_fill_shared(vt);
    vt[0] = (void *)qb_init;
    vt[3] = (void *)qb_clean;
    vt[6] = (void *)qb_behavior;
    vt[9] = (void *)qb_render;
    vt[12] = (void *)ac_pdes_base;
    /* The gate-17 reading: the castle grounds' one block is never destroyed
       -- its own Behavior parks it in state 2 when it is used rather than
       marking it. Slot 16 is the matched D1; slot 17 is the ROM's own deleting
       destructor (DTOR-PAIRS seat), the matched flat-C body behind the ecx->arg
       adapter where the trap stood. */
    vt[16] = (void *)qb_d1;
    vt[17] = (void *)qb_d0;
    /* 32 slots. Five of the tail are the block's own ov102 bodies: 21
       OnGroundPounded (0x02149820), 22 OnAttacked1 (0x021497c8), 24 OnKicked
       (0x02149770), 27 OnHitByMegaChar (0x02149710) and 28
       OnHitFromUnderneath (0x021496a4) -- the last is how a question block
       answers being punched from below, so running Actor's do-nothing there
       would be wrong rather than merely incomplete.

       GATE 203 SEATS ALL FIVE. They were on ac_trapNN because they were in no
       slice; slice_gate203.txt links them and their one shared helper
       (func_ov102_02149078). The trap was not a harmless placeholder here: it
       calls port_actor_slot_decline, the quarantine net catches the fault, and
       the actor FREEZES for the rest of the level. Every one of these five
       bodies ends in func_ov102_02149da8(c, 1) -- enter state 1, the bounce,
       which gate 180 already hosts -- so trapping them is precisely what kept
       the box from opening while the state table underneath was mounted and
       correct the whole time. The reported symptom, "softlock when trying to
       open exclamation box in jrb ship from below", is slot 28's.

       Each is the THREE-parameter veneer: the ROM body is (self, other) and
       both dispatch sites (src/func_ov002_020eeca8.cpp and the head-bonk host
       copy unmatched/Player_HeadBonk.cpp) push the argument and expect the
       CALLEE to pop it. Slot 31 is Platform::Kill, unchanged. */
    vt[21] = (void *)qb_pounded;
    vt[22] = (void *)qb_atk1;
    vt[27] = (void *)qb_mega;
    vt[28] = (void *)qb_under;
    vt[31] = (void *)ac_kill;

    /* SLOT 24 IS SEATED with qb_kicked, the block's own OnKicked (ov102
       0x02149770). The convention conflict that used to block it is closed.
       Slots 21/22/27/28 above are safe because every linked dispatch site for
       them is thiscall: the receiver arrives in ECX and the callee pops the one
       pushed argument, which is what the three-parameter veneer's `ret 4` does.
       Slot 24's only dispatcher used to be the exception -- func_ov002_020eeeb8
       was CDECL, both arguments pushed and the CALLER cleaning up via a batched
       `add esp,10h`, which needed a callee that popped NOTHING. That dispatcher
       is now the thiscall host copy unmatched/Actor_OnKickedDispatch.cpp, so
       the site pushes one argument and expects the callee to pop it, exactly
       like the seated slot-24 bodies (ac_kicked, and every override, already
       emit `ret 4`). qb_kicked is the same three-parameter __fastcall veneer
       that emits `ret 4`, so it balances the now-thiscall caller.

       The body (func_ov102_02149770) was already sliced with its four siblings
       in slice_gate203.txt, and its whole closure is in the link: it consults
       func_ov102_02149078 (the refusal test, whose zero-argument ClosestPlayer
       read is already host copied in Actor_ClosestPlayer_OverlayReaders.cpp)
       and ends in func_ov102_02149da8(c, 1), the state-1 bounce already hosted.
       It dispatches no vtable slot of its own, so there is no tail chain to
       seat alongside it. This is the follow-up seat the dispatcher conversion
       unblocked. */
    vt[24] = (void *)qb_kicked;
}
