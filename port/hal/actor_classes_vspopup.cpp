// Lane VSPOPUP: the two ov002 classes every live VS match asks for and the
// port declines -- NUMBER (330, the floating score popup) and WING_FEATHER
// (345, the wing-cap feather).
//
// Both come out of lane VSARENA2's live-match playlogs: every one of that
// night's seven matches logged `[spawn-declined] actor 0x14a not registered`,
// and maps 1 and 3 logged 0x159 as well. Neither is a host-layer gap -- both
// are ordinary class seats the port had simply never reached, because neither
// id is ever placed by a level's object table. They are spawned at RUN TIME,
// which is why a headless boot census never asked for them and no earlier
// gate noticed.
//
// Same law as hal/actor_classes.cpp and hal/actor_classes_ccm.cpp: MSVC slot
// order, __fastcall thunks, every call qualified, a named trap for anything
// the port cannot service, and the ten-odd shared Actor/ActorBase halves
// written by this file's own copy of the fill helper so it merges with the
// other actor streams without either touching the other's statics.
//
// ---- THE CONFIG'S CLASS NAMES ARE SHIFTED FOR NUMBER, and the port already
// ---- knew it ---------------------------------------------------------------
//
// hal/actor_classes_bob_world.cpp's INVISIBLE_SECRET section says it in as
// many words, and this seat is the other half of that finding. By address:
//
//   InvisibleSecret_SpawnInfo  ov002 0x0210b00c  actor 329
//   InvisibleSecret_Spawn      ov002 0x020f085c, installs data_ov002_0210b030
//                              (RTTI 9daSCoin_c) -- a 276-byte object with a
//                              MovingCylinderClsn, seated in bob_world
//   Number_SpawnInfo           ov002 0x0210b0c8  actor 330
//   Number_Spawn               ov002 0x020f0d90, installs 0x0210b0ec
//   _ZTV15InvisibleSecret      ov002 0x0210b0ec, RTTI 13daObjNumber_c
//
// So the six src files spelled _ZN15InvisibleSecret* are NUMBER'S methods and
// the table the config spells _ZTV15InvisibleSecret is NUMBER'S table. This
// file seats them under the id their own SpawnInfo carries (330), and touches
// nothing bob_world owns: different record, different factory, different
// table, and the registry's own +4 id cross-check reads 330 back out of the
// mounted record.
//
// ---- NUMBER (330) -- daObjNumber_c, a plain 31-slot Actor ------------------
//
// _ZTV15InvisibleSecret (ov002 0x0210b0ec) read with its relocations applied:
//   0  InitResources    0x020f0bd4  own (`InvisibleSecret::InitResources`)
//   3  CleanupResources 0x020f093c  own
//   6  Behavior         0x020f0a60  own
//   9  Render           0x020f0994  own -- dispatches a Model virtual through
//                                   a local shadow class; see the slot-5 note
//  12  OnPendingDestroy 0x02043ac0  ActorBase's base body (shared)
//  16  D1               0x020f0894  own -- a real MSVC-synthesised member dtor
//                                   chain over a LOCAL shadow hierarchy with
//                                   no bodies of its own, and NO vptr store
//                                   (checked: the only relocations in its span
//                                   are the three dtor calls). The OneUpLogo
//                                   D1 recipe: the chain is spelled directly
//                                   below and the shadow-class TU is not
//                                   compiled.
//  17  D0               0x020f08cc  own -- spells _ZTV13daObjNumber_c, the RTTI
//                                   name of its OWN table (exactly one speller
//                                   in all of src/, not a shared placeholder),
//                                   so it stays in the slice under an alias
//  18..30  every one the plain shared Actor default, confirmed by reloc target
//          address -- Number overrides none of the interaction tail.
// SpawnInfo 0x0210b0c8 (+0 factory 0x020f0d90, +4 id 330), 0x24 bytes, ending
// exactly where the table's typeinfo pointer starts. Object 336 bytes with a
// Model at +0xd4 and a TextureSequence at +0x124 -- the SAME shape as
// ONE_UP_LOGO, whose seat this one follows step for step.
//
// ---- WING_FEATHER (345) -- daFeather_c, a plain 31-slot Actor --------------
//
// _ZTV11WingFeather (ov002 0x021088a8), same read:
//   0  InitResources    0x020b311c  own
//   3  CleanupResources 0x020b2e30  own -- a C-linkage .c body taking NO
//                                   argument (it only Releases one file ptr)
//   6  Behavior         0x020b2e9c  own
//   9  Render           0x020b2e54  own -- the slot-5 note again
//  12  OnPendingDestroy 0x02043ac0  ActorBase's base body (shared)
//  16  D1               0x020b2ba0  own -- the same synthesised chain, this
//                                   one WITH a vptr store of its own table
//                                   (0x021088a8, in its literal pool). Writing
//                                   the class's own table back over itself is
//                                   a no-op -- nothing derives from this class
//                                   -- so the host thunk spells the four
//                                   member dtors and Actor::D2 and skips it,
//                                   the OneUpLogo treatment.
//  17  D0               0x020b2be8  own -- spells _ZTV11daFeather_c, again the
//                                   RTTI name of its OWN table with exactly
//                                   one speller; stays in the slice
//  18..30  the shared Actor defaults, byte-for-byte the same twelve addresses
//          Number's table carries.
// SpawnInfo 0x02108884 (+0 factory 0x020b3248, +4 id 345), 0x24 bytes. Object
// 904 bytes: Model +0xd4, MovingCylinderClsn +0x124, WithMeshClsn +0x158,
// ShadowModel +0x314.
//
// ---- THE SLOT-5 QUESTION, ANSWERED BEFORE IT COST ANYTHING -----------------
//
// Both Renders reach the object's Model through a LOCAL shadow class, and the
// two shadow classes are shaped DIFFERENTLY, which is exactly the shape of the
// MSVC destructor-slot shift:
//
//   src/_ZN15InvisibleSecret6RenderEv.cpp  struct Obj { v0..v4; m(int); }
//       no destructor -> MSVC counts m at 5, mwcc counts it at 5. Same slot.
//   src/_ZN11WingFeather6RenderEv.cpp      struct Sub { ~Sub(); a; b; c; f4; }
//       a VIRTUAL DESTRUCTOR -> MSVC folds D1/D0 into one and counts f4 at 4,
//       while mwcc counts it at 5. One slot low against the ROM.
//
// Neither is a hazard here, and the reason is a convention that already
// exists: hal/cxxname_bridge.cpp's hal_fill_model_vtable fills _ZTV5Model in
// MSVC numbering ([1] DoSetFile, [2] UpdateVerts, [3] Virtual10, [4] Render)
// and DUAL-FILLS [5] with Render as well, so shadow TUs that count in ROM
// numbering are served by the same array. Slot 4 and slot 5 both land on
// Model::Render. Written down because it is the first thing to re-check if
// either of these two ever renders wrong.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the shared Actor/ActorBase halves, all matched arm9 bodies already in the
   build. The addresses beside each are what BOTH of this file's two ROM
   tables actually hold in that slot -- read off config/arm9/overlays/ov002/
   relocs.txt, not assumed from the shape. */
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* 1  0x02011268 */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* 2  0x02011244 */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* 7  0x02010fd4 */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* 10 0x02010f78 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* 18 0x02010160 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* 19 0x02010154 */
int _ZN5Actor9Virtual50Ev(void *self);                         /* 20 0x0201014c */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* 21 0x02010148 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* 22 0x02010144 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* 23 0x02010140 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* 24 0x0201013c */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* 25 0x02010138 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26 0x02010134 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27 0x02010130 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28 0x0201012c */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                 /* 29 0x02010124 */
extern int data_02099f24[];               /* the frame phase */
extern unsigned char data_020a4b4c;       /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
}

static int __fastcall vsp_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall vsp_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall vsp_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
/* Slots 5, 8 and 11 are ARM tail-call veneers on the ROM (Actor's own forward
   into ActorBase's body), so the thunk calls the target directly rather than
   forwarding through the veneer's face and dropping the argument riding in
   r1 -- the bob_world/ccm convention, unchanged. */
static void __fastcall vsp_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall vsp_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall vsp_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall vsp_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall vsp_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall vsp_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static void __fastcall vsp_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); }

/* Slots 13 and 14 keep the gate-16 trap for the reason bob_world writes out at
   length: ActorBase::Virtual34/Virtual38 sit on top of the SolidHeap
   allocator, a subsystem rather than two slots, and nothing has reached them
   yet. The trap names the slot, the actor and the phase, which is the loud
   version of "no level has reached this". */
static void vsp_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n", slot, id,
                 port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall vsp_trap13(void *s, void *) { vsp_trap_report(s, 13); return 0; }
static int __fastcall vsp_trap14(void *s, void *) { vsp_trap_report(s, 14); return 0; }
/* Slot 30 returns a Vector3 by value; the sret contract is unproved and the
   body is in no slice, so it declines by name -- every other actor gate here
   does the same. */
static int __fastcall vsp_trap30(void *s, void *) { vsp_trap_report(s, 30); return 0; }

static int __fastcall vsp_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19, OnTurnIntoEgg(Player &): the caller PUSHES the player, so the
   three-parameter veneer pops it. */
static int __fastcall vsp_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall vsp_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall vsp_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall vsp_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall vsp_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall vsp_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall vsp_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall vsp_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall vsp_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall vsp_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall vsp_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* The shared half of both tables. Slot 12 is written here too: both ROM tables
   hold ActorBase::OnPendingDestroy (0x02043ac0) there, neither class overrides
   it, so it belongs with the rest of the shared set rather than being repeated
   per class. */
static void vsp_fill_shared(void **vt)
{
    vt[1]  = (void *)vsp_binit;
    vt[2]  = (void *)vsp_ainit;
    vt[4]  = (void *)vsp_bclean;
    vt[5]  = (void *)vsp_aclean;
    vt[7]  = (void *)vsp_bbeh;
    vt[8]  = (void *)vsp_abeh;
    vt[10] = (void *)vsp_bren;
    vt[11] = (void *)vsp_aren;
    vt[12] = (void *)vsp_pdes;
    vt[13] = (void *)vsp_trap13;
    vt[14] = (void *)vsp_trap14;
    vt[15] = (void *)vsp_heap;
    vt[18] = (void *)vsp_yoshi;
    vt[19] = (void *)vsp_turn_egg;
    vt[20] = (void *)vsp_v50;
    vt[21] = (void *)vsp_pounded;
    vt[22] = (void *)vsp_atk1;
    vt[23] = (void *)vsp_atk2;
    vt[24] = (void *)vsp_kicked;
    vt[25] = (void *)vsp_pushed;
    vt[26] = (void *)vsp_cannon;
    vt[27] = (void *)vsp_mega;
    vt[28] = (void *)vsp_under;
    vt[29] = (void *)vsp_aimed;
    vt[30] = (void *)vsp_trap30;
}

// ---- the two classes' own bodies -------------------------------------------
extern "C" {
/* the member destructors the two D1 chains call, in the order the ROM's own
   bodies call them (HIGH ADDRESS FIRST, which is reverse declaration order and
   what the matching D0s do too) */
void _ZN15TextureSequenceD1Ev(void *self);       /* Number      +0x124 */
void _ZN5ModelD1Ev(void *self);                  /* both        +0x0d4 */
void _ZN11ShadowModelD1Ev(void *self);           /* WingFeather +0x314 */
void _ZN12WithMeshClsnD1Ev(void *self);          /* WingFeather +0x158 */
void _ZN18MovingCylinderClsnD1Ev(void *self);    /* WingFeather +0x124 */
void _ZN5ActorD2Ev(void *self);

/* NUMBER's own bodies. The four real C++ methods are faced at the foot of this
   file; D0 and the factory are C-linkage already. */
int _ZN15InvisibleSecret13InitResourcesEv(void *self);
int _ZN15InvisibleSecret16CleanupResourcesEv(void *self);
int _ZN15InvisibleSecret8BehaviorEv(void *self);
int _ZN15InvisibleSecret6RenderEv(void *self);
int *_ZN15InvisibleSecretD0Ev(int *self);
void *Number_Spawn(void);                        /* installs _ZTV15InvisibleSecret */
extern unsigned char Number_SpawnInfo[];         /* ov002 0x0210b0c8 */
int _ZTV15InvisibleSecret[31];
int _ZTV13daObjNumber_c[];    /* RTTI alias, ONE speller (_ZN15InvisibleSecretD0Ev),
                                 not a shared placeholder -- aliased below. */

/* WING_FEATHER's own bodies. CleanupResources and D0 are C-linkage .c files;
   note CleanupResources takes NO argument -- it only releases one file
   pointer -- so its thunk drops the receiver rather than forwarding it. */
int _ZN11WingFeather13InitResourcesEv(void *self);
int _ZN11WingFeather16CleanupResourcesEv(void);
int _ZN11WingFeather8BehaviorEv(void *self);
int _ZN11WingFeather6RenderEv(void *self);
int *_ZN11WingFeatherD0Ev(int *self);
void *WingFeather_Spawn(void);                   /* installs _ZTV11WingFeather */
extern unsigned char WingFeather_SpawnInfo[];    /* ov002 0x02108884 */
int _ZTV11WingFeather[31];
int _ZTV11daFeather_c[];      /* RTTI alias, ONE speller (_ZN11WingFeatherD0Ev) */
}
/* Both D0 bodies spell their own table by its RTTI name. One object, two C
   names; alias rather than edit the matched src -- the OneUpLogo /
   daObj1UpLogo_c treatment. */
#pragma comment(linker, "/alternatename:__ZTV13daObjNumber_c=__ZTV15InvisibleSecret")
#pragma comment(linker, "/alternatename:__ZTV11daFeather_c=__ZTV11WingFeather")
/* WingFeather::InitResources declares its model's SharedFilePtr at FILE SCOPE
   rather than through decl_common.h's extern "C" umbrella, so MSVC C++-mangles
   what the mount emits under its C name -- the IceSheet_ModelFile case, and the
   only unresolved symbol either of these two classes produced. Confirmed
   against the exact LNK2019 text, not guessed:
     unresolved external symbol "struct SharedFilePtr data_ov002_0210da58"
     (?data_ov002_0210da58@@3USharedFilePtr@@A) referenced in
     "public: int __thiscall WingFeather::InitResources(void)"
   Number's four read the same way and are ALREADY aliased under this exact
   spelling by hal/actor_faces_bob.cpp:110-113 (0210d9a8 / d9e8 / da08 / da28),
   which is why only the feather's needed one. da58 already carries a
   ?...@@3PADA (char *) alias in hal/cxx_aliases.cpp for a different reader;
   a second spelling of the same object is the data_ov002_0210de00 precedent
   two lines below it. */
#pragma comment(linker, "/alternatename:?data_ov002_0210da58@@3USharedFilePtr@@A=_data_ov002_0210da58")

static int __fastcall num_init(void *s, void *)
{ return _ZN15InvisibleSecret13InitResourcesEv(s); }
static int __fastcall num_clean(void *s, void *)
{ return _ZN15InvisibleSecret16CleanupResourcesEv(s); }
static int __fastcall num_behavior(void *s, void *)
{ return _ZN15InvisibleSecret8BehaviorEv(s); }
static int __fastcall num_render(void *s, void *)
{ return _ZN15InvisibleSecret6RenderEv(s); }
/* slot 16: src/_ZN15InvisibleSecretD1Ev.cpp is a real MSVC-synthesised member
   destructor over a LOCAL shadow Actor/Model/TextureSequence hierarchy whose
   classes have no bodies of their own, so it is not compiled -- the gate-31
   recipe (hal/actor_classes.cpp's PeachPainting D1), the same one ONE_UP_LOGO
   takes for this identical object shape. The chain is spelled here instead:
   TextureSequence at +0x124, Model at +0xd4, then Actor's own D2. No vptr
   store, because the ROM body has none (its whole relocation span is the three
   destructor calls). */
static int __fastcall num_d1(void *s, void *)
{
    char *t = (char *)s;
    _ZN15TextureSequenceD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall num_d0(void *s, void *)
{ return (int)(size_t)_ZN15InvisibleSecretD0Ev((int *)s); }

static int __fastcall wf_init(void *s, void *)
{ return _ZN11WingFeather13InitResourcesEv(s); }
static int __fastcall wf_clean(void *, void *)
{ return _ZN11WingFeather16CleanupResourcesEv(); }
static int __fastcall wf_behavior(void *s, void *)
{ return _ZN11WingFeather8BehaviorEv(s); }
static int __fastcall wf_render(void *s, void *)
{ return _ZN11WingFeather6RenderEv(s); }
/* slot 16: the same shape, four members instead of two. The ROM body DOES load
   its own table into the vptr first; writing _ZTV11WingFeather back over a
   WingFeather's own vptr changes nothing (no class derives from it), so the
   thunk spells only the part that has an effect -- exactly what the class's
   own D0 does after its store. Order is the D0's order, high address first. */
static int __fastcall wf_d1(void *s, void *)
{
    char *t = (char *)s;
    _ZN11ShadowModelD1Ev(t + 0x314);
    _ZN12WithMeshClsnD1Ev(t + 0x158);
    _ZN18MovingCylinderClsnD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall wf_d0(void *s, void *)
{ return (int)(size_t)_ZN11WingFeatherD0Ev((int *)s); }

extern "C" void hal_fill_number_vtable(void)
{
    void **vt = (void **)_ZTV15InvisibleSecret;
    vsp_fill_shared(vt);
    vt[0]  = (void *)num_init;
    vt[3]  = (void *)num_clean;
    vt[6]  = (void *)num_behavior;
    vt[9]  = (void *)num_render;
    vt[16] = (void *)num_d1;
    vt[17] = (void *)num_d0;
    /* no slot 31: Actor-derived, not Platform-derived -- 31 slots total */
}

extern "C" void hal_fill_wing_feather_vtable(void)
{
    void **vt = (void **)_ZTV11WingFeather;
    vsp_fill_shared(vt);
    vt[0]  = (void *)wf_init;
    vt[3]  = (void *)wf_clean;
    vt[6]  = (void *)wf_behavior;
    vt[9]  = (void *)wf_render;
    vt[16] = (void *)wf_d1;
    vt[17] = (void *)wf_d0;
    /* no slot 31, same reason */
}

// ---- the method faces ------------------------------------------------------
//
// Seven of the twelve bodies are `Class::Method` .cpp definitions, so MSVC
// mangles them off the Itanium name rather than emitting it (the IceSheet /
// OneUpLogo case, hal/actor_classes_ccm.cpp's foot). They are faced here.
// _ZN15InvisibleSecretD0Ev, _ZN11WingFeather16CleanupResourcesEv,
// _ZN11WingFeatherD0Ev, both factories and func_ov002_020b2c44 are already
// C-linkage bodies -- no face.
#include "InvisibleSecret.h"
#include "WingFeather.h"
extern "C" {
int _ZN15InvisibleSecret13InitResourcesEv(void *self)
{ return ((InvisibleSecret *)self)->InvisibleSecret::InitResources(); }
int _ZN15InvisibleSecret16CleanupResourcesEv(void *self)
{ return ((InvisibleSecret *)self)->InvisibleSecret::CleanupResources(); }
int _ZN15InvisibleSecret8BehaviorEv(void *self)
{ return ((InvisibleSecret *)self)->InvisibleSecret::Behavior(); }
int _ZN15InvisibleSecret6RenderEv(void *self)
{ return ((InvisibleSecret *)self)->InvisibleSecret::Render(); }
int _ZN11WingFeather13InitResourcesEv(void *self)
{ return ((WingFeather *)self)->WingFeather::InitResources(); }
int _ZN11WingFeather8BehaviorEv(void *self)
{ return ((WingFeather *)self)->WingFeather::Behavior(); }
int _ZN11WingFeather6RenderEv(void *self)
{ return ((WingFeather *)self)->WingFeather::Render(); }
}
