// THE BOWSER FIGHT -- ov060, level 38 (koopa2_boss). Wave 5 (lane w5-e)
// mounted the overlay and hosted the arena FLOOR; wave 6 (lane w6-A) hosts the
// other five classes and level 38 reaches ZERO SKIPS.
//
// ov060 is the pack all three koopaN_boss arenas load (LoadOrUnloadObject-
// Overlays short-circuits idx 0x24/0x26/0x28 to ov060 with an early return --
// but the SELECTOR LOOP runs first, so those levels also get ov089, the
// boss-reward pack: KEY 282 / LAST_STAR 283. An arm9 read, corrected in
// wave 6 from the "ov060 alone" the first draft of this file said).
// This lane mounts it per symbol (port/ov060_syms.txt, where the full cast
// map, the one dsd label shift and every width pin are written down), wires
// its 106 func_ov060_* TUs and six sinits (port/slice_w5e.txt), and hosts
// BOWSER_FIRE_SEA_ARENA (166, daKpa2Bg_c) -- the tilting platform that IS
// the arena's walkable floor. With it skipped, the level-38 census boots
// honest but Mario has nothing to stand on except the lava. Wave 6 adds
// SPIKE_BOMB (284, x8 placed), BOWSER (279), BOWSER_TAIL (278), BOWSER_FIRE
// (280) and BOWSER_SKY_PLATFORM (167) in port/slice_w6a.txt, and ov089's
// KEY/LAST_STAR in hal/actor_classes_ov089.cpp.
//
// WHAT WAVE 6 ADDED, and what the wave-5 bank got wrong:
//   - The bank recorded "Bowser's two state-pair tables, 0x0211a4e0.. and
//     0x0211a734..".  Re-derived from the READERS, ov060 has SIX source runs
//     filling SEVEN runtime tables (BOWSER FIRE's one run of sixteen records
//     fills two of them, its init half and its behaviour half), and neither of
//     those attributions holds: 0x0211a734 is BOWSER FIRE's (both
//     of its halves), 0x0211a4e0 is the four-halfword message-id table
//     func_ov060_02115518 indexes and Bowser's own records start at
//     0x0211a4e8, and SPIKE BOMB has a table of its own at 0x0211b1d8 that was
//     not in the bank at all.  The full map, all 50 seated records and the
//     measured pointer-to-member sizes are in
//     port/unmatched/Ov060_StateDispatch.cpp.
//   - Bowser's init spawns 278 AND 280 and writes through both results with
//     no null check, and his defeat path spawns 282 (ov089's KEY) and writes
//     through that one too -- so 279/278/280/282 register in ONE landing.
//     Bowser Shockwaves (281) is the one class still out: its two Actor::Spawn
//     sites ignore their results (checked), and its Render is the ModelAnim
//     slot-5 collision on TWO ModelAnims.
//   - The arena class's closure was reloc-walked body by body: two
//     func_ov060 helpers (02117a64/02117ae0, in the slice), the already-
//     seated BeforeClsn installer func_020393d4 (the c9a1731da family), its
//     two SharedFilePtrs (0211affc model / 0211aff4 clsn, built by
//     __sinit_ov060_02119f94), and ONE cross-window CLPS read -- nothing
//     touches the state tables. It is hostable without a single new seam.
//
// ---- THE CLEANUP THUNK (the one body held out of the slice) ---------------
// src/_ZN18BowserFireSeaArena16CleanupResourcesEv.cpp spells its two
// SharedFilePtrs G0/G1, and hal/cxx_aliases.cpp binds those single global
// names to SignPost's LIVE ov002 pointers -- the identical PoleLift trap
// hal/actor_classes_ov045.cpp documents. Linking it would Release SignPost's
// files on every arena teardown. relocs.txt (0x02117b94/98) says the ROM
// body releases 0x0211affc then 0x0211aff4 behind an IsEnabled-guarded
// Disable of the collider at +0x374 (calls 0x020393dc IsEnabled then
// 0x02039140 Disable); arena_clean below is that transcription, the ep_clean
// recipe verbatim.
//
// ---- THE CLPS ALIAS -------------------------------------------------------
// InitResources passes the collider CLPS as `&func_021115bc` -- dsd's
// code-flavored name for 0x021115bc in the twenty-way-shared level-overlay
// window. For this class's only level the owner is ov046 (level 38's own
// image), so the name is aliased onto the one-symbol per-symbol ov046 mount
// (port/ov046_syms.txt). CLPS records carry no pointers; the copy cannot
// diverge from the whole-image mount the loaders walk.
#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"
#include "dtor_faces_cpp.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half -- the same family every hosted 31/32-slot table
   carries; the slot addresses were read off _ZTV18BowserFireSeaArena's own
   reloc run and match the ov045/wf tables slot for slot */
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* the generated per-symbol mounts (build/host-src/ov060_syms.c, ov046_syms.c) */
void port_ov060_pack_check(void);
void port_ov060_syms_patch(void);
void port_ov046_pack_check(void);
void port_ov046_syms_patch(void);
/* run linkw wave 6 (lane w6-A): the six state tables' seat. Rewrites the 50
   SOURCE statics with host bodies BEFORE the sinits copy them into bss (the
   Crate/WaterBomb ordering); see port/unmatched/Ov060_StateDispatch.cpp for
   the table map, the measured pointer-to-member sizes and the two named
   holes. */
void port_ov060_states_seat(void);
/* ov060's six sinits, all linkable (no ov045-style shared-window sinit) */
void __sinit_ov060_021195dc(void);
void __sinit_ov060_02119df0(void);
void __sinit_ov060_02119f94(void);
void __sinit_ov060_0211a000(void);
void __sinit_ov060_0211a388(void);
void __sinit_ov060_0211a428(void);

/* the arena's own bodies (the .cpp methods are faced at file bottom) */
int _ZN18BowserFireSeaArena13InitResourcesEv(void *self);     /* slot 0  */
int _ZN18BowserFireSeaArena8BehaviorEv(void *self);           /* slot 6  */
int _ZN18BowserFireSeaArena6RenderEv(void *self);             /* slot 9  */
int *_ZN18BowserFireSeaArenaD1Ev(int *self);                  /* slot 16 */
int *_ZN18BowserFireSeaArenaD0Ev(int *self);                  /* slot 17 */
void *BowserFireSeaArena_Spawn(void);
/* what arena_clean spells by hand (the ep_clean recipe) */
int _ZN16MeshColliderBase9IsEnabledEv(void *self);
void _ZN16MeshColliderBase7DisableEv(void *self);
void _ZN13SharedFilePtr7ReleaseEv(void *sfp);
extern int data_ov060_0211affc[], data_ov060_0211aff4[];
/* the Enable face's C-linkage dispatcher (hal/cxxname_bridge.cpp routing) */
int _ZN16MeshColliderBase6EnableEP5Actor(void *self, void *actor);

/* ---- run linkw wave 6 (lane w6-A): the fight's own five classes ---------- */
/* member teardowns the three .cpp-destructor host thunks spell by hand */
void *_ZN5ActorD2Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);
void _ZN15TextureSequenceD1Ev(void *self);
void _ZN12WithMeshClsnD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *self);

/* SPIKE BOMB (284) -- the SHIFTED spellings, see the class block below */
int _ZN17BowserSkyPlatform13InitResourcesEv(void *self);   /* slot 0  */
int _ZN17BowserSkyPlatform16CleanupResourcesEv(void);      /* slot 3  */
int _ZN17BowserSkyPlatform8BehaviorEv(void *self);         /* slot 6  */
int _ZN17BowserSkyPlatform6RenderEv(void *self);           /* slot 9  */
int *_ZN17BowserSkyPlatformD0Ev(int *self);                /* slot 17 */
void *SpikeBomb_Spawn(void);

/* BOWSER (279) */
int _ZN6Bowser13InitResourcesEv(void *self);               /* slot 0  */
int _ZN6Bowser16CleanupResourcesEv(void);                  /* slot 3  */
int _ZN6Bowser8BehaviorEv(void *self);   /* slot 6, HOST COPY (StateDispatch) */
int _ZN6Bowser6RenderEv(void *self);   /* slot 9, HOST COPY (Ov060_Renders) */
void _ZN6Bowser16OnPendingDestroyEv(void);                 /* slot 12 */
int *_ZN6BowserD0Ev(int *self);                            /* slot 17 */
void *Bowser_Spawn(void);

/* BOWSER TAIL (278) */
int _ZN10BowserTail13InitResourcesEv(void *self);          /* slot 0  */
int _ZN10BowserTail16CleanupResourcesEv(void);             /* slot 3  */
int _ZN10BowserTail8BehaviorEv(void *self);                /* slot 6  */
int _ZN10BowserTail6RenderEv(void);                        /* slot 9  */
int *_ZN10BowserTailD0Ev(int *self);                       /* slot 17 */
void *BowserTail_Spawn(void);

/* BOWSER FIRE (280) -- an Enemy build; Init/Behavior are HOST COPIES */
int _ZN10BowserFire13InitResourcesEv(void *self);          /* slot 0  */
int _ZN10BowserFire16CleanupResourcesEv(void);             /* slot 3  */
int _ZN10BowserFire8BehaviorEv(void *self);                /* slot 6  */
int _ZN10BowserFire6RenderEv(void);                        /* slot 9  */
int *_ZN10BowserFireD1Ev(int *self);                       /* slot 16 */
int *_ZN10BowserFireD0Ev(int *self);                       /* slot 17 */
void *BowserFire_Spawn(void);

/* BOWSER SKY PLATFORM (167) -- every own body is a func_ov060_* already in
   slice_w5e.txt; only slot 6 is a host copy (Ov060_StateDispatch). */
int func_ov060_021182b0(void *self);                       /* slot 0  */
int func_ov060_021181e8(void *self);                       /* slot 3  */
int func_ov060_02118254(void *self);                       /* slot 6  */
int func_ov060_0211822c(void *self);                       /* slot 9  */
int *func_ov060_02117d1c(int *self);                       /* slot 16 */
int *func_ov060_02117d60(int *self);                       /* slot 17 */
void *BowserSkyPlatform_Spawn(void);

DSSTATE_BEGIN
void *_ZTV18BowserFireSeaArena[32];
void *_ZTV17BowserSkyPlatform[31];   /* SPIKE BOMB's table, 0x0211aa8c */
void *_ZTV6Bowser[31];               /* 0x0211a6b8 */
void *_ZTV10BowserTail[31];          /* 0x0211a634 */
void *_ZTV10BowserFire[31];          /* 0x0211a7f4 */
void *data_ov060_0211a9b0[32];       /* BOWSER SKY PLATFORM's, Kill @31 */
DSSTATE_END
}

/* 0x0211a8b0 answers to both names; the D1/D0 (.c, real extern spellings, not
   VT placeholders) restore it by the RTTI one. daKpa2Bg = the koopa2 arena
   background. The base-table store in the same bodies is _ZTV10dBgActor_c,
   already defined and host-filled by hal/actor_classes.cpp. */
#pragma comment(linker, "/alternatename:__ZTV10daKpa2Bg_c=__ZTV18BowserFireSeaArena")

/* The arena CLPS spelling onto the one-symbol ov046 mount (header note). */
#pragma comment(linker, "/alternatename:_func_021115bc=_data_ov046_021115bc")

/* run linkw wave 6 (lane w6-A): the RTTI spellings the four .c destructors
   restore their tables by. Same table, sibling name -- the daKpa2Bg_c line
   above, four more times. kirai = the naval mine = SPIKE BOMB (the label
   shift; the daKirai_c name belongs to the table SpikeBomb_Spawn stores). */
#pragma comment(linker, "/alternatename:__ZTV9daKirai_c=__ZTV17BowserSkyPlatform")
#pragma comment(linker, "/alternatename:__ZTV7daKpa_c=__ZTV6Bowser")
#pragma comment(linker, "/alternatename:__ZTV11daKpaTail_c=__ZTV10BowserTail")
#pragma comment(linker, "/alternatename:__ZTV11daKpaFire_c=__ZTV10BowserFire")

/* The ov066-window spellings of ov060's own bss cells (ov060_syms.txt's
   window note): same bytes, one storage, sibling names aliased on.
   THE THREE /alternatename ROWS THAT USED TO SIT HERE ARE GONE. run rel0215
   wave 2, lane cast-ov066 mounted ov066 per symbol, and all three addresses
   are named there too -- 0x0211ac20, 0x0211ac68 and 0x0211acd0 are three of
   Eyerok's THIRTY-EIGHT pointer-to-member source pairs. The mount DEFINES
   data_ov066_0211ac20 / _0211ac68 / _0211acd0, so every one of the three
   aliases was defeated silently and ov060's two caller TUs would have read
   Eyerok's state table instead of their own bss. The bindings are now
   per-source -D renames in port/CMakeLists.txt, on the two TUs that actually
   spell them:
     src/func_ov060_02111c68.c   data_ov066_0211ac20, data_ov066_0211ac68
     src/func_ov060_02112350.c   data_ov066_0211acd0
   the w5b_review.md R2 recipe. The ov060 storage below is unchanged; only the
   spelling bridge moved. */

/* The MSVC-decorated spellings C++ TUs in the pack use for the same mounted
   storage (the actor_faces_bob / bowserpuzzle @@3PA precedent; data aliases
   are exact, there is no `this` to lose). The P8C one is a pointer-to-member
   array of a TU-local class -- the cells are BUILT and DISPATCHED by host
   code on both sides, so the stride is self-consistent (the Painting trap
   needs ROM-record reads, which this is not). */
#pragma comment(linker, "/alternatename:?data_ov060_0211ac20@@3PAHA=_data_ov060_0211ac20")
#pragma comment(linker, "/alternatename:?data_ov060_0211ac68@@3PAHA=_data_ov060_0211ac68")
#pragma comment(linker, "/alternatename:?data_ov060_0211ac70@@3PAHA=_data_ov060_0211ac70")
#pragma comment(linker, "/alternatename:?data_ov060_0211aed4@@3PAUTabEnt@@A=_data_ov060_0211aed4")
/* The @@3PAP8C@@AEXXZA (0x0211ae9c) and @@3PAUEntry@@A (0x0211b1ac) siblings
   of the line above are GONE in wave 6: the two TUs that spelled those cells
   at C++ linkage -- func_ov060_02115b84 and func_ov060_02118254 -- are host
   copies now and use the C name. The guard reads an unreferenced LHS as
   "unused, OK", so leaving them would have been inert rather than wrong; they
   are removed because they would read as live documentation of a spelling
   nothing emits any more. The TabEnt one stays: func_ov060_021128c0 is still
   matched src and still spells it that way. */

/* Two statics wanted by pack TUs under their STRUCT-tagged MSVC decorations
   (the class-tagged PAV variants already exist in hal/actor_faces_bob.cpp;
   statics are cdecl on both sides, so the aliases are exact), and the
   mangled-in-mangled free-function spelling of UpdatePosWithTransform (@@YA,
   the one alias-eligible decoration class), aliased onto the same defined
   MSVC static the @@3PAHA data spelling in hal/actor_classes_bob_world.cpp
   already targets. */
#pragma comment(linker, "/alternatename:?FindWithActorID@Actor@@SAPAU1@IPAU1@@Z=__ZN5Actor15FindWithActorIDEjPS_")
#pragma comment(linker, "/alternatename:?LoadFile@MeshCollider@@SAPAUKCL_File@@AAUSharedFilePtr@@@Z=__ZN12MeshCollider8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@YAXXZ=?UpdatePosWithTransform@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")

/* Link residue of the wired pack, each with its named precedent:
   - the arena Init TU spells three C-linkage bodies at C++ linkage (no
     extern "C" on its decls); all three decorations are @@YA cdecl free
     functions, the one alias-eligible class, onto their defined C names --
     the wf_enemy_bridges UpdatePosAndAngs recipe (its @@YAXXZ sibling is
     already there; the arena's decls return int, hence @@YAH here);
   - Bowser_IsAnimAtLastFrame declares Animation::GetFrameCount returning
     int; the matched method (slice_gate21) emits the u32 form. Identical
     receiver, identical args, return in eax either way -- the
     StarMarkerFace member-onto-member alias precedent;
   - func_ov060_02112ee0 wants Sound::StopLoadedMusic_Layer1 by its GNU
     spelling; the matched TU (slice_gate10) defines the MSVC STATIC --
     no `this`, cdecl both sides, exact. */
#pragma comment(linker, "/alternatename:?_ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block@@YAHPAX00HF0@Z=__ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block")
#pragma comment(linker, "/alternatename:?func_020393d4@@YAHPAX0@Z=_func_020393d4")
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@YAHXZ=__ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_")
#pragma comment(linker, "/alternatename:?GetFrameCount@Animation@@QBEHXZ=?GetFrameCount@Animation@@QBEIXZ")
#pragma comment(linker, "/alternatename:__ZN5Sound22StopLoadedMusic_Layer1Ej=?StopLoadedMusic_Layer1@Sound@@SAXI@Z")

/* ?Enable@MeshColliderBase@@QAEXPAUActor@@@Z -- the STRUCT-Actor variant of
   the face hal/shutter_bob_face.cpp defines for `class Actor` spellers. A
   thiscall member can never be an /alternatename; this TU includes Actor.h,
   so Actor is a struct here and the symbol this definition emits is
   byte-for-byte the one the pack TUs reference. Forwards to the C-linkage
   dispatcher like every other Enable. */
struct MeshColliderBase {
    void Enable(Actor *a);
};
void MeshColliderBase::Enable(Actor *a)
{ _ZN16MeshColliderBase6EnableEP5Actor(this, a); }

// ---- the trap --------------------------------------------------------------
static void ov60_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov060 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov060 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV60_TRAP(n) \
    static int __fastcall ov60_trap##n(void *s, void *) \
    { ov60_trap_report(s, n); return 0; }
OV60_TRAP(13) OV60_TRAP(14) OV60_TRAP(17)
#undef OV60_TRAP

static int __fastcall ov60_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov60_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov60_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov60_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov60_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov60_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov60_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov60_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov60_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov60_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov60_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov60_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov60_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov60_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov60_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov60_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov60_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov60_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov60_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov60_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov60_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov60_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail; the arena's table carries it (id 166 is a
   PlatformC2 build and its reloc run has 0x020ee55c at slot 31). */
static int __fastcall ov60_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half, slots 1..30, read off _ZTV18BowserFireSeaArena's own
   reloc run (slot 12 is ActorBase::OnPendingDestroy 0x02043ac0 there, so it
   is written here; a wave-6 class that overrides 12 -- Bowser and
   ExtendingPlatform-shaped ids -- writes its own after this returns).
   Slots 13/14 are the ActorBase Virtual34/38 traps and 30 declines, the
   wf/ov45 reading.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug
   (hal/actor_classes_ov002g200.cpp): MSVC 19.44 x86 /O2 can delete a static
   filler's stores when it is called with several distinct extern-array
   arguments. One caller today, seven tomorrow. */
static void ov60_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov60_binit;
    vt[2]  = (void *)ov60_ainit;
    vt[4]  = (void *)ov60_bclean;
    vt[5]  = (void *)ov60_aclean;
    vt[7]  = (void *)ov60_bbeh;
    vt[8]  = (void *)ov60_abeh;
    vt[10] = (void *)ov60_bren;
    vt[11] = (void *)ov60_aren;
    vt[12] = (void *)ov60_pdes;
    vt[13] = (void *)ov60_trap13;
    vt[14] = (void *)ov60_trap14;
    vt[15] = (void *)ov60_heap;
    vt[17] = (void *)ov60_trap17;
    vt[18] = (void *)ov60_yoshi;
    vt[19] = (void *)ov60_turn_egg;
    vt[20] = (void *)ov60_v50;
    vt[21] = (void *)ov60_pounded;
    vt[22] = (void *)ov60_atk1;
    vt[23] = (void *)ov60_atk2;
    vt[24] = (void *)ov60_kicked;
    vt[25] = (void *)ov60_pushed;
    vt[26] = (void *)ov60_cannon;
    vt[27] = (void *)ov60_mega;
    vt[28] = (void *)ov60_under;
    vt[29] = (void *)ov60_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the mount bring-up ----------------------------------------------------
// The ov45_bringup shape: no lane owns hal/actor_overlays.cpp this wave
// either, so the pack bring-up rides the first registry fill. Whoever next
// owns actor_overlays.cpp should move this body beside the ov013/ov045
// blocks and cut the guard to a call. The ov046 one-symbol mount rides the
// same guard: its only consumer is this cast.
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov60_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov60_bringup_done;
DSSTATE_END

extern "C" void port_ov60_bringup(void)
{
    if (g_ov60_bringup_done)
        return;
    g_ov60_bringup_done = 1;
    port_ov060_pack_check();
    port_ov060_syms_patch();
    port_ov046_pack_check();
    port_ov046_syms_patch();
    /* BEFORE the sinits: all six state tables are bss the sinits fill from
       .data source records holding DS code addresses. */
    port_ov060_states_seat();
    __sinit_ov060_021195dc();
    __sinit_ov060_02119df0();
    __sinit_ov060_02119f94();
    __sinit_ov060_0211a000();
    __sinit_ov060_0211a388();
    __sinit_ov060_0211a428();
}

// ============================================================================
// BOWSER_FIRE_SEA_ARENA (id 166) -- table 0x0211a8b0, 32 slots
// ============================================================================
//
// PlatformC2 build; Model at +0x324, MovingMeshCollider at +0x374. The
// arena's tilting disc over the fire sea: its InitResources loads its model
// and collision through the two __sinit_ov060_02119f94 SharedFilePtrs,
// installs MeshColliderBase::UpdatePosWithTransform as the BeforeClsn
// callback through func_020393d4 (the contested-slot family c9a1731da seats
// -- the third mover proof after ov015's bridge and JRB's SHIP_UP), and its
// Behavior tilts the disc under the player through the two func_ov060
// helpers. Own slots 0/3/6/9/16/17 + Platform::Kill at 31.
static int __fastcall arena_init(void *s, void *)
{ return _ZN18BowserFireSeaArena13InitResourcesEv(s); }
/* slot 3, HOST THUNK, not the matched TU -- the G0/G1 trap (file header). */
static int __fastcall arena_clean(void *s, void *)
{
    char *t = (char *)s;
    if (_ZN16MeshColliderBase9IsEnabledEv(t + 0x374))
        _ZN16MeshColliderBase7DisableEv(t + 0x374);
    _ZN13SharedFilePtr7ReleaseEv(data_ov060_0211affc);
    _ZN13SharedFilePtr7ReleaseEv(data_ov060_0211aff4);
    return 1;
}
static int __fastcall arena_behavior(void *s, void *)
{ return _ZN18BowserFireSeaArena8BehaviorEv(s); }
static int __fastcall arena_render(void *s, void *)
{ port_actor_render_probe("BOWSER_FIRE_SEA_ARENA", (char *)s + 0x324);
  return _ZN18BowserFireSeaArena6RenderEv(s); }
static int __fastcall arena_d1(void *s, void *)
{ return (int)(size_t)_ZN18BowserFireSeaArenaD1Ev((int *)s); }
static int __fastcall arena_d0(void *s, void *)
{ return (int)(size_t)_ZN18BowserFireSeaArenaD0Ev((int *)s); }
extern "C" void hal_fill_bowser_fire_sea_arena_vtable(void)
{
    port_ov60_bringup();
    void *volatile *vt = (void *volatile *)_ZTV18BowserFireSeaArena;
    ov60_fill_shared(vt);
    vt[0]  = (void *)arena_init;
    vt[3]  = (void *)arena_clean;
    vt[6]  = (void *)arena_behavior;
    vt[9]  = (void *)arena_render;
    vt[16] = (void *)arena_d1;
    vt[17] = (void *)arena_d0;
    vt[31] = (void *)ov60_kill;
}

// ============================================================================
// SPIKE BOMB (id 284) -- table 0x0211aa8c, 31 slots        run linkw wave 6
// ============================================================================
//
// THE LABEL SHIFT, re-confirmed on this tree rather than inherited: the table
// dsd names _ZTV17BowserSkyPlatform / _ZTV9daKirai_c is the one SpikeBomb_Spawn
// stores (src/SpikeBomb_Spawn.c, `p[0] = (int)_ZTV17BowserSkyPlatform`), and
// its slot bodies carry the shifted _ZN17BowserSkyPlatform* spellings.
// BowserSkyPlatform_Spawn stores the UNNAMED data_ov060_0211a9b0 instead.
// The semantics settle it a fourth way: this class's InitResources ends in
// AddSpikeBomb(this), which claims a slot in the eight-entry global
// data_0209f3a4 -- and level 38 places exactly EIGHT id-284 objects.
//
// Own slots 0/3/6/9/16/17, no Platform tail (31 slots, the plain Actor shape;
// this is an Actor build with a Model at +0xd4 and a
// MovingCylinderClsnWithPos at +0x124, 432 bytes).
//
// EVERY BODY IS MATCHED SRC. Two things that would normally hold bodies out
// do NOT apply here, both checked:
//   - Behavior decodes its state record BY HAND (`struct {int a,b;}`, an
//     8-byte read, then a cdecl call through the first word) instead of
//     forming an MSVC pointer-to-member, so its stride is the ROM's. It only
//     needs the four source records at 0x0211aa30/38/40/48 seated, which
//     port_ov060_states_seat does.
//   - Render's `((Sub *)&mModel)->m(0)` six-virtual local shadow lands on
//     _ZTV5Model slot 5, and hal/cxxname_bridge.cpp DUAL-FILLS that table
//     (slot 4 and slot 5 both Render) precisely for shadow-TU dispatch. This
//     is not the ModelAnim collision -- ModelAnim cannot be dual-filled
//     because Virtual18 already occupies its slot 5, and this class has no
//     ModelAnim.
static int __fastcall spikebomb_init(void *s, void *)
{ return _ZN17BowserSkyPlatform13InitResourcesEv(s); }
static int __fastcall spikebomb_clean(void *s, void *)
{ return _ZN17BowserSkyPlatform16CleanupResourcesEv(); }
static int __fastcall spikebomb_behavior(void *s, void *)
{ return _ZN17BowserSkyPlatform8BehaviorEv(s); }
static int __fastcall spikebomb_render(void *s, void *)
{ port_actor_render_probe("SPIKE_BOMB", (char *)s + 0xd4);
  return _ZN17BowserSkyPlatform6RenderEv(s); }
/* slot 16, HOST THUNK: src/_ZN17BowserSkyPlatformD1Ev.cpp is a real MSVC
   destructor over a shadow class, so MSVC emits ??1BowserSkyPlatform@@UAE@XZ
   and auto-calls ??1Model@@QAE@XZ and two more that exist nowhere in this
   build -- the BigBooIcon/HauntedChair case. The chain below is what its
   matched D0 (.c, linked) spells minus the Deallocate. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (spikebomb_d1) spelled the same chain by hand. */
static int __fastcall spikebomb_d0(void *s, void *)
{ return (int)(size_t)_ZN17BowserSkyPlatformD0Ev((int *)s); }
extern "C" void hal_fill_spike_bomb_vtable(void)
{
    port_ov60_bringup();
    void *volatile *vt = (void *volatile *)_ZTV17BowserSkyPlatform;
    ov60_fill_shared(vt);
    vt[0]  = (void *)spikebomb_init;
    vt[3]  = (void *)spikebomb_clean;
    vt[6]  = (void *)spikebomb_behavior;
    vt[9]  = (void *)spikebomb_render;
    vt[16] = (void *)hal_cppd1_BowserSkyPlatform;
    vt[17] = (void *)spikebomb_d0;
}

// ============================================================================
// BOWSER (279) + BOWSER TAIL (278) + BOWSER FIRE (280)     run linkw wave 6
// ============================================================================
//
// THESE THREE LAND TOGETHER, and that is not a preference. Bowser's
// InitResources ends with two Actor::Spawn calls and writes through BOTH
// results without a null check (src/_ZN6Bowser13InitResourcesEv.c, verified on
// this tree, not inherited from the wave-5 bank):
//
//     a1 = Actor::Spawn(0x118 = 280 BOWSER FIRE, ...);
//     *(int *)(a1 + 0x2cc) = *(int *)(c + 4);            <- unchecked
//     a2 = Actor::Spawn(0x116 = 278 BOWSER TAIL, ...);
//     *(int *)(c + 0x3a8) = *(int *)(a2 + 4);            <- unchecked read
//     *(int *)(a2 + 0x108) = *(int *)(c + 4);            <- unchecked
//     func_02011d50(a2);
//
// The port's Actor::Spawn returns null for an unregistered class (that is what
// the census counts as a skip), so a Bowser registered alone faults on his
// first init frame at null+0x2cc. 279 without 280 and 278 is not a smaller
// step, it is a crash.
//
// A THIRD CLASS IS REACHED THE SAME WAY, one level deeper, and this one was
// NOT in the wave-5 bank: func_ov060_021135fc -- Bowser's defeat path, reached
// from func_ov060_02112ee0 and func_ov060_021130c0 -- spawns 0x11a (282 KEY)
// and writes `*(short *)(spawned + 0x440)` unchecked. KEY lives in ov089,
// which the arena already loads through the selector row, and its registry row
// is in hal/actor_classes_ov089.cpp: it has to be in the same landing as
// Bowser or his defeat sequence faults the same way his init would.
// (Its sibling spawn, 0x11b = 283 LAST_STAR, IS ignored.
//
// TWO MORE WRITERS, named here because the lane's prose left them anonymous:
// func_ov060_021167ec spawns 0x118 = 280 once and writes f92/f94/f96 and
// +0x360 through the result, and func_ov060_021169f8 spawns 0x118 at THREE
// sites and writes +0x360 through each. Neither null-checks. Both are already
// safe in this landing because 280 registers with Bowser -- but they are safe
// as a by-product of his init needing it, not because a registration decision
// was ever made for them, which is exactly why they are written down here.
//
// The remaining ov060-internal sites -- func_ov060_02114858, 02115b0c,
// 02116b68, 0211747c -- ignore their results, and func_ov060_021172e0
// null-checks both of its. That is the whole sweep: NINE functions in the pack
// call Actor::Spawn, sixteen call sites between them, FOUR of those functions
// write through a result without a check (Bowser's InitResources, his defeat
// path func_ov060_021135fc, func_ov060_021167ec and func_ov060_021169f8), and
// every id those four reach -- 278, 280, 282 -- registers in this landing.)
//
// BOWSER 279, table 0x0211a6b8, 31 slots. Own 0/3/6/9/12/16/17 -- he is the
// one class in this pack that overrides OnPendingDestroy (slot 12), so his
// fill writes it AFTER ov60_fill_shared. His Behavior dispatches two of the
// three tables the seat covers (0x0211aeb4 through func_ov060_02112434, then
// 0x0211aed4 through func_ov060_021128c0, which aeb4[0] points at).
// Slot 16 is a HOST THUNK -- src/_ZN6BowserD1Ev.cpp is a real MSVC destructor
// over a shadow class (the BigBooIcon case); the chain is his matched D0's,
// minus the Deallocate. Slot 9 is a HOST COPY, the ModelAnim slot-5 collision
// (port/unmatched/Ov060_Renders.cpp).
//
// BOWSER TAIL 278, table 0x0211a634, 31 slots -- width 31, not the 33 the dsd
// span reads: the two words past it are _ZTV6Bowser's {offset-to-top,
// typeinfo} RTTI header (re-read from the image this wave, slot 31 raw zero,
// slot 32 a pointer to data_ov060_0211a5cc which is _ZTV6Bowser's typeinfo).
// Own 0/3/6/9/16/17; slot 16 is the same .cpp-destructor host thunk. Its
// Behavior runs func_ov060_02115b84 (host copy) over the third seated table.
//
// BOWSER FIRE 280, table 0x0211a7f4, 31 slots, an EnemyC2 build. Own
// 0/3/6/9/16/17, and BOTH of its own state-driven bodies are host copies:
// InitResources forms its pointer-to-member over a complete empty `struct
// Actor { }` (stride 4) and Behavior over a never-defined one (stride 16),
// against the ROM's 8. Its D1/D0 are .c and link directly.
static int __fastcall bowser_init(void *s, void *)
{ return _ZN6Bowser13InitResourcesEv(s); }
static int __fastcall bowser_clean(void *s, void *)
{ return _ZN6Bowser16CleanupResourcesEv(); }
static int __fastcall bowser_behavior(void *s, void *)
{ return _ZN6Bowser8BehaviorEv(s); }
static int __fastcall bowser_render(void *s, void *)
{ port_actor_render_probe("BOWSER", (char *)s + 0xd4);
  return _ZN6Bowser6RenderEv(s); }
static int __fastcall bowser_pdes(void *s, void *)
{ (void)s; _ZN6Bowser16OnPendingDestroyEv(); return 0; }
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (bowser_d1) spelled the same chain by hand. */
static int __fastcall bowser_d0(void *s, void *)
{ return (int)(size_t)_ZN6BowserD0Ev((int *)s); }
extern "C" void hal_fill_bowser_vtable(void)
{
    port_ov60_bringup();
    void *volatile *vt = (void *volatile *)_ZTV6Bowser;
    ov60_fill_shared(vt);
    vt[0]  = (void *)bowser_init;
    vt[3]  = (void *)bowser_clean;
    vt[6]  = (void *)bowser_behavior;
    vt[9]  = (void *)bowser_render;
    vt[12] = (void *)bowser_pdes;   /* after the shared fill, which writes 12 */
    vt[16] = (void *)hal_cppd1_Bowser;
    vt[17] = (void *)bowser_d0;
}

static int __fastcall btail_init(void *s, void *)
{ return _ZN10BowserTail13InitResourcesEv(s); }
static int __fastcall btail_clean(void *s, void *)
{ return _ZN10BowserTail16CleanupResourcesEv(); }
static int __fastcall btail_behavior(void *s, void *)
{ return _ZN10BowserTail8BehaviorEv(s); }
static int __fastcall btail_render(void *s, void *)
{ (void)s; return _ZN10BowserTail6RenderEv(); }
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (btail_d1) spelled the same chain by hand. */
static int __fastcall btail_d0(void *s, void *)
{ return (int)(size_t)_ZN10BowserTailD0Ev((int *)s); }
extern "C" void hal_fill_bowser_tail_vtable(void)
{
    port_ov60_bringup();
    void *volatile *vt = (void *volatile *)_ZTV10BowserTail;
    ov60_fill_shared(vt);
    vt[0]  = (void *)btail_init;
    vt[3]  = (void *)btail_clean;
    vt[6]  = (void *)btail_behavior;
    vt[9]  = (void *)btail_render;
    vt[16] = (void *)hal_cppd1_BowserTail;
    vt[17] = (void *)btail_d0;
}

static int __fastcall bfire_init(void *s, void *)
{ return _ZN10BowserFire13InitResourcesEv(s); }
static int __fastcall bfire_clean(void *s, void *)
{ return _ZN10BowserFire16CleanupResourcesEv(); }
static int __fastcall bfire_behavior(void *s, void *)
{ return _ZN10BowserFire8BehaviorEv(s); }
static int __fastcall bfire_render(void *s, void *)
{ (void)s; return _ZN10BowserFire6RenderEv(); }
static int __fastcall bfire_d1(void *s, void *)
{ return (int)(size_t)_ZN10BowserFireD1Ev((int *)s); }
static int __fastcall bfire_d0(void *s, void *)
{ return (int)(size_t)_ZN10BowserFireD0Ev((int *)s); }
extern "C" void hal_fill_bowser_fire_vtable(void)
{
    port_ov60_bringup();
    void *volatile *vt = (void *volatile *)_ZTV10BowserFire;
    ov60_fill_shared(vt);
    vt[0]  = (void *)bfire_init;
    vt[3]  = (void *)bfire_clean;
    vt[6]  = (void *)bfire_behavior;
    vt[9]  = (void *)bfire_render;
    vt[16] = (void *)bfire_d1;
    vt[17] = (void *)bfire_d0;
}

// ============================================================================
// BOWSER SKY PLATFORM (167) -- table data_ov060_0211a9b0, 32 slots
// ============================================================================
//
// The other half of the label shift: the koopa3 arena's floor (daKpa3Bg_c),
// whose table carries NO _ZTV name because dsd handed that name to the table
// SpikeBomb actually uses. A Platform build (812 bytes, PlatformC2, Model at
// +0xd4 like every Platform), so slot 31 is Platform::Kill and the width is
// 32 -- both read off the reloc run.
//
// Every own body is already a linked func_ov060_* (slice_w5e.txt); only its
// Spawn joins this lane's slice. Slot 6 is the host copy: it dispatches the
// three-record bss table at 0x0211b1ac through a pointer-to-member whose
// stride is 4, not 8. Slot 9's `((Base *)(self + 0xd4))->m(0)` six-virtual
// shadow lands on _ZTV5Model slot 5, which cxxname_bridge dual-fills -- the
// SpikeBomb reading, and Platform::Platform is what puts a plain Model there
// (src/_ZN8PlatformC2Ev.c, `Model::Model(this + 0xd4)`).
//
// NOT PLACED IN LEVEL 38 -- this is level 40's floor (koopa3_boss). It
// registers here because the seat, the mount and the cast map are all in hand
// and the row costs one line; level 40's mount is a separate lane.
static int __fastcall skyplat_init(void *s, void *)
{ return func_ov060_021182b0(s); }
static int __fastcall skyplat_clean(void *s, void *)
{ return func_ov060_021181e8(s); }
static int __fastcall skyplat_behavior(void *s, void *)
{ return func_ov060_02118254(s); }
static int __fastcall skyplat_render(void *s, void *)
{ port_actor_render_probe("BOWSER_SKY_PLATFORM", (char *)s + 0xd4);
  return func_ov060_0211822c(s); }
static int __fastcall skyplat_d1(void *s, void *)
{ return (int)(size_t)func_ov060_02117d1c((int *)s); }
static int __fastcall skyplat_d0(void *s, void *)
{ return (int)(size_t)func_ov060_02117d60((int *)s); }
extern "C" void hal_fill_bowser_sky_platform_vtable(void)
{
    port_ov60_bringup();
    void *volatile *vt = (void *volatile *)data_ov060_0211a9b0;
    ov60_fill_shared(vt);
    vt[0]  = (void *)skyplat_init;
    vt[3]  = (void *)skyplat_clean;
    vt[6]  = (void *)skyplat_behavior;
    vt[9]  = (void *)skyplat_render;
    vt[16] = (void *)skyplat_d1;
    vt[17] = (void *)skyplat_d0;
    vt[31] = (void *)ov60_kill;
}

// ---- method faces ----------------------------------------------------------
// The three bodies src defines as real C++ methods against
// BowserFireSeaArena.h (Init/Behavior/Render; the D1/D0 are .c and callable
// directly). The IceSheet/ov045 recipe: the face is the C-name bridge INTO
// the matched method, not a host copy of it.
#include "BowserFireSeaArena.h"
extern "C" {
int _ZN18BowserFireSeaArena13InitResourcesEv(void *self)
{ return ((BowserFireSeaArena *)self)->BowserFireSeaArena::InitResources(); }
int _ZN18BowserFireSeaArena8BehaviorEv(void *self)
{ return ((BowserFireSeaArena *)self)->BowserFireSeaArena::Behavior(); }
int _ZN18BowserFireSeaArena6RenderEv(void *self)
{ return ((BowserFireSeaArena *)self)->BowserFireSeaArena::Render(); }
}

// run linkw wave 6 (lane w6-A): the same recipe for the five wave-6 bodies src
// defines as real C++ methods. SPIKE BOMB's three are under the shifted
// BowserSkyPlatform.h spellings -- the header is generated from the matched
// functions' own evidence, so it is SPIKE BOMB's layout under that name.
#include "BowserSkyPlatform.h"
#include "BowserTail.h"
extern "C" {
int _ZN17BowserSkyPlatform13InitResourcesEv(void *self)
{ return ((BowserSkyPlatform *)self)->BowserSkyPlatform::InitResources(); }
int _ZN17BowserSkyPlatform8BehaviorEv(void *self)
{ return ((BowserSkyPlatform *)self)->BowserSkyPlatform::Behavior(); }
int _ZN17BowserSkyPlatform6RenderEv(void *self)
{ return ((BowserSkyPlatform *)self)->BowserSkyPlatform::Render(); }
int _ZN10BowserTail13InitResourcesEv(void *self)
{ return ((BowserTail *)self)->BowserTail::InitResources(); }
int _ZN10BowserTail8BehaviorEv(void *self)
{ return ((BowserTail *)self)->BowserTail::Behavior(); }
}
