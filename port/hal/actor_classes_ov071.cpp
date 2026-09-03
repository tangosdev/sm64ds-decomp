// RUN LINKW WAVE 7 (lane w7b): THE MR. I FAMILY -- MR_I (262) and BIG_MR_I
// (263), the floating eyeball daEykn_c, plus MR_I_PROJECTILE (264), the shot it
// fires. Second, third and fourth classes hosted out of ov071; the overlay was
// already mounted per-symbol by gate 176 (SCUTTLEBUG 255) and by wave 5 lane
// w5-A (COFFIN 64), so this lane adds NO new overlay -- only rows, fills, a
// state seat and two host-copied cores.
//
// Same law as hal/actor_classes_scuttlebug.cpp (the sibling in this very
// overlay, and the Enemy-31-slot exemplar this file is built on) and
// hal/actor_classes_ov065.cpp: ROM slot order, __fastcall thunks that call
// QUALIFIED or by C name, unhosted slots trap by name.
//
// ---- THE CAST MAP WAS RE-DERIVED, NOT CARRIED ------------------------------
// The brief named 262/263 as "the MR. I family"; port/ov071_syms.txt's own
// header comment says something DIFFERENT -- "MrI/BigMrI (198/199, the
// eyeball), MrI_Projectile (262?)". That comment is WRONG and the ids below are
// re-derived from the ROM, by the double test the shared ov070/071/073/074
// address window makes mandatory (spawnFunc must land inside ov071 AND the
// record's +4 id halfword must match):
//
//   id  ACTOR_SPAWN_TABLE slot   SpawnInfo   word[0] spawnFunc      +4 halfword
//   255 0x02090c60 -> 0x02122c08 Scuttlebug  0x02120618 Scuttlebug_Spawn   255
//   262 0x02090c7c -> 0x02122cf0 MrI         0x02121a1c MrI_Spawn          262
//   263 0x02090c80 -> 0x02122d0c BigMrI      0x021219cc BigMrI_Spawn       263
//   264 0x02090c84 -> 0x02122dc4 MrI_Proj    0x02121f9c MrI_Projectile_S.  264
//
// read from extracted/arm9_dec.bin (base 0x02004000) for the table and from
// extracted/overlays/overlay_0071.bin for the records. Ids 198 and 199 DO
// resolve to records inside ov071's address window but their word[0] spawnFuncs
// land OUTSIDE it -- they belong to another module sharing the window, which is
// exactly the trap the double test exists for. So the syms header's "198/199"
// is a mis-attribution and "262?" was one class short.
//
// THE OVERLAY BASE the byte reads use is 0x0211f000 -- which IS the yaml's
// base_address 34729984 (34729984 == 0x0211f000; the yaml is correct, a
// wrong hex gloss in the lane brief said 0x0211f600).
// Pinned three ways that agree, all in
// config/arm9/overlays/ov071/delinks.txt and the yaml's own size fields:
// .text start:0x0211f000, .bss end:0x02123100; image 0x02122f80-0x0211f000 =
// 0x3f80 = 16256 = the byte length of overlay_0071.bin (overlays.yaml has no
// ram_size key -- its size field is code_size: 16224 = 0x3f60); bss
// 0x02123100-0x02122f80 = 0x180 = 384 = bss_size. At the glossed 0x0211f600
// the symbol func_ov071_0211f0a4 would sit below the overlay.
//
// ---- 263 IS A FREE SHARE OF 262'S FILL -------------------------------------
// MrI_Spawn (0x02121a1c) and BigMrI_Spawn (0x021219cc) are the SAME function
// twice: both `new(0x218)`, both store _ZTV3MrI (0x02122d30) as the vptr, both
// construct ModelAnim +0xd4, TextureSequence +0x138, ShadowModel +0x14c,
// MovingCylinderClsnWithPos +0x174. One class, one vtable, two ids. The class
// separates the two at runtime by its own actor id: MrI::InitResources gives
// 0x106 (262) scale 0x1000 with a 0x55000/0x96000 collider and 0x107 (263)
// scale 0x2000 with 0xaa000/0x12c000, and func_ov071_02120d30's step 3 pays out
// a plain drop for 262 but the tracked star for 263. So BIG_MR_I costs one
// registry row and no second fill.
//
// ---- WIDTHS, each pinned by the reloc run AND the next dsd symbol ----------
//   _ZTV3MrI            0x02122d30  31 slots (Enemy shape). Declared span to
//                       the next symbol (data_ov071_02122dac) is 0x7c = 31
//                       words, and config/arm9/overlays/ov071/relocs.txt has
//                       exactly 31 relocations in [+0x00, +0x78] and NONE at or
//                       after 0x02122dac. Both tests agree.
//   _ZTV14MrI_Projectile 0x02122de8 31 slots, the identical double check
//                       (31 relocs +0x00..+0x78, next symbol 0x02122e64).
// Cross-checked against _ZTV10Scuttlebug in the same overlay: the shared
// Actor/ActorBase halves (1/2/4/5/7/8/10/11/13/14/15/20..28/30) hold the
// identical arm9 addresses in all three tables.
//
// IDENTITY BY RTTI, corroborated by code landing. This overlay's dsd data
// labels are the `ambiguous` kind the brief warns can shift one class over, so
// they are not the evidence. An earlier revision of this comment claimed mwcc
// emitted no Itanium RTTI here and that vtable[-1] held nothing to read; a
// review refuted that from the ROM, and the typeinfo is the strongest evidence
// available, so it leads:
//
//   _ZTV3MrI            0x02122d30  vtable[-1] 0x02122ce4  name "8daEykn_c"
//   _ZTV14MrI_Projectile 0x02122de8  vtable[-1] 0x02122db8  name "8daEyBm_c"
//   _ZTV10Scuttlebug    0x02122c2c  vtable[-1] 0x02122bfc  name "7daSpd_c"
//
// Each is a proper __si_class_type_info record (vtable 0x0209a764, name ptr,
// base ptr), relocs.txt carries a relocation at each vtable[-1] and none at
// vtable[-2], and all three names match the config's dsd aliases exactly.
// The ov065 header's "slot 0 lands in that class's own code" test agrees and
// is kept as corroboration -- slot 0 of _ZTV3MrI is 0x02121734 =
// _ZN3MrI13InitResourcesEv, slot 0 of _ZTV14MrI_Projectile is 0x02121eb4 =
// _ZN14MrI_Projectile13InitResourcesEv, and (the control) slot 0 of
// _ZTV10Scuttlebug is 0x021203f8 = Scuttlebug's own. No label is shifted.
//
// ---- SLOTS EACH CLASS OWNS -------------------------------------------------
// MrI owns 0 (Init), 3 (Cleanup), 6 (Behavior), 9 (Render), 12
// (OnPendingDestroy), 16 (D1), 17 (D0). UNLIKE Scuttlebug it has NO 18/19/29
// overrides -- those bind Actor's own defaults. MrI_Projectile owns the same
// seven plus 18 (OnYoshiTryEat func_ov071_02121b00, returns 4).
// Slots 13/14 (ActorBase::Virtual34/38) trap by name the way every sibling fill
// traps that pair, and slot 30 (OnAimedAtWithEggReturnVec) traps because its
// matched body is SRET -- a hidden return-slot pointer before self -- which no
// fill's thunk shape models. The Scuttlebug binding set exactly.
//
// ---- WHAT IS A HOST COPY AND WHY -------------------------------------------
//   ONE per-frame state MAIN -- func_ov071_021211e0 (0x314) -- is the only
//     ov071 name with no src/ TU at all, on either this branch or main.
//     port/unmatched/MrI_StateMains.cpp, with the derivation. It used to be
//     two: func_ov071_02120d30 (0x3dc) was hosted alongside it until run
//     linkw wave 9 (lane w9-harvest) found src/func_ov071_02120d30.c on
//     main (PR #1474, 8ec808874) -- a TU this branch, 626 commits behind,
//     could not see. It is a slice line now (port/slice_w9harvest.txt).
//   the two state dispatchers -- func_ov071_021215c0 (MAIN, record+8) and
//     func_ov071_021215fc (ENTER, record+0) -- are the Scuttlebug PMF-stride
//     case: the matched TUs form `c->pp + 1` over an mwcc 8-byte PMF that MSVC
//     makes 4, so the host `+1` lands on the delta word and dispatches a zero.
//     port/unmatched/MrI_StateDispatch.cpp, with the source-side seat.
//   MrI::Render -- the ModelAnim slot-5 collision (the Whomp/Fish/Scuttlebug
//     case): the matched TU dispatches slot 5 of a local six-virtual shadow,
//     which is ModelAnim::Render in ROM order but Virtual18 in the host's
//     MSVC-ordered _ZTV9ModelAnim. port/unmatched/MrI_Render.cpp.
//   both D1s (slot 16) are HOST THUNKS, the flame/HauntedChair/Scuttlebug
//     treatment: the matched D1s are the auto-emitted-member-dtor form whose
//     compiler-emitted member and base dtor calls resolve to MSVC-mangled names
//     that do not exist in this build, so they are NOT compiled. Each thunk
//     runs its own D0 chain minus the final Memory::Deallocate, because the
//     slot-16 caller (ActorBase::AfterCleanupResources) deallocates itself.
//     Member sub-objects are destroyed high address first, the D0 order:
//       MrI            +0x174 MovingCylinderClsnWithPos, +0x14c ShadowModel,
//                      +0x138 TextureSequence, +0xd4 ModelAnim, ~Actor
//       MrI_Projectile +0x13c WithMeshClsn, +0xfc MovingCylinderClsnWithPos,
//                      +0xd4 ShadowModel, ~Actor
//     Both orders are read from the matched D0 TUs, which ARE compiled.
//   D1/D0 NUMBERING: the seats above are ROM vtable slots 16 and 17 -- the
//     Itanium pair. MSVC folds its own complete/deleting destructors into one
//     slot, which is why slot 16 cannot be an MSVC `~MrI()` and is a thunk.
//
// MrI::InitResources is NONMATCHING-but-decompiled src
// (src/_ZN3MrI13InitResourcesEv.c, "constant / value (div=6)", logic verified
// correct vs ROM by whoever banked it). It is real behaviour, not a stub, so it
// rides the slice; it is the one body in the closure that is not byte-matched.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "dtor_faces_cpp.h"
#include "ActorBase.h"
#include "MrI.h"
#include "MrI_Projectile.h"

extern "C" {
/* the shared lifecycle halves, the same functions every fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);             /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                  /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                    /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                   /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);  /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                        /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);     /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);         /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);         /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);             /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);             /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);            /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* ---- MR. I's own bodies ----------------------------------------------------
   Behavior is a REAL C++ METHOD (src/_ZN3MrI8BehaviorEv.cpp defines
   `int MrI::Behavior()` against include/MrI.h), so it is reached qualified
   below, not by the _ZN name -- the Scuttlebug Init/Behavior/Render treatment.
   InitResources IS spelled extern "C" inside its own TU, so it keeps the
   _ZN name; Render is this lane's host copy. */
int _ZN3MrI13InitResourcesEv(char *self);      /* slot 0,  nonmatching src, extern "C" */
int _ZN3MrI16CleanupResourcesEv(void);         /* slot 3,  C linkage, no self */
int _ZN3MrI6RenderEv(void *self);              /* slot 9,  HOST COPY */
void _ZN3MrI16OnPendingDestroyEv(void);        /* slot 12, empty, no self */
int *_ZN3MrID0Ev(int *self);                   /* slot 17, C linkage */
void *MrI_Spawn(void);
void *BigMrI_Spawn(void);

/* ---- MR_I_PROJECTILE's own bodies -----------------------------------------
   Init/Behavior/Render are all real C++ methods against
   include/MrI_Projectile.h -- reached qualified below. */
int _ZN14MrI_Projectile16CleanupResourcesEv(void);      /* slot 3  */
void _ZN14MrI_Projectile16OnPendingDestroyEv(void);     /* slot 12 */
int *_ZN14MrI_ProjectileD0Ev(int *self);                /* slot 17 */
int func_ov071_02121b00(void);                          /* slot 18, returns 4 */
void *MrI_Projectile_Spawn(void);

/* the D1 chains' sub-object destructors, all C-linkage in the build (the same
   node dtors the two D0 TUs call) */
void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
void _ZN11ShadowModelD1Ev(void *);
void _ZN15TextureSequenceD1Ev(void *);
void _ZN9ModelAnimD1Ev(void *);
void _ZN12WithMeshClsnD1Ev(void *);
void *_ZN5ActorD2Ev(void *);

/* The arrays the ROM factories install (MrI_Spawn and BigMrI_Spawn both do
   `p[0] = (int)_ZTV3MrI`; MrI_Projectile_Spawn stores its own); thirty-one
   slots each. Defined here, not just declared: the `int` type and C linkage
   match the `extern int _ZTV3MrI[]` in include/decl_common.h that the .c
   factories and D0s read. */
int _ZTV3MrI[31];
int _ZTV14MrI_Projectile[31];
}

/* The two D0 TUs store the RTTI base spellings (the dsd names at the same
   addresses) into slot 0; both references are C linkage, so point the
   underscore spellings at the one host array each -- the daSpd_c / daChoropu_c
   precedent in the sibling fill. */
#pragma comment(linker, "/alternatename:__ZTV8daEykn_c=__ZTV3MrI")
#pragma comment(linker, "/alternatename:__ZTV8daEyBm_c=__ZTV14MrI_Projectile")

/* NAME RACE RESOLVED BY ADDRESS (the shared-window rule, and the same disease
   the sibling fill records for data_ov073_02122f88). src/func_ov071_021209c8.cpp
   spells its texture-sequence SharedFilePtr `data_ov074_02123038`, but the load
   site's own relocation settles it: config/arm9/overlays/ov071/relocs.txt has
       from:0x02120a1c kind:load to:0x02123038 module:overlay(71)
   -- ov071's OWN bss pointer, mounted in port/ov071_syms.txt. The ov074
   spelling is another module's dsd export winning the race at a shared-window
   address. Alias by address; the config rename goes upstream as its own PR.

   RETIRED, run rel0215 wave 2 (lane w2-ov074), AND THE RULING ABOVE STILL
   APPLIES UNCHANGED. The directive here used to be
     /alternatename:_data_ov074_02123038=_data_ov071_02123038
   which worked only while nothing DEFINED the LHS. ov074 now has its own
   per-symbol mount (port/ov074_syms.txt) and data_ov074_02123038 is one of
   Goomboss's fourteen SharedFilePtrs -- its own relocations cite the address
   three times as module:overlay(74) and src/__sinit_ov074_02122978.c
   Constructs it with file id 0x38f -- so the alias became inert and
   func_ov071_021209c8 would have set its TextureSequence's file out of
   GOOMBOSS's cell: silently, with no link error and no byte-gate signal, the
   Coffin/Spindrift shape. port/tools/alternatename_guard.py catches it at the
   link. The routing moved to the guard's own remedy, a per-source -D on the
   ONE reader (src/func_ov071_021209c8.cpp) in port/CMakeLists.txt beside the
   ov074 slice block. Nothing about ov071 changes; what changes is that the
   binding no longer depends on a name staying undefined somewhere else. */

/* C++-MANGLED DATA SPELLINGS, the data_02082128 / data_020a0e68 precedent.
   src/_ZN3MrI13InitResourcesEv.c and the two MrI_Projectile method TUs declare
   their mount data OUTSIDE any extern "C" block, so MSVC mangles the
   references while the mount defines the plain C names. Point each mangled
   spelling at the one host object. Every LHS below is the exact decorated name
   the linker asked for, and each is undefined everywhere else, so no alias can
   be defeated by a real definition (the wave-5 R1/R2 lesson). */
#pragma comment(linker, "/alternatename:?data_ov071_02123038@@3USharedFilePtr@@A=_data_ov071_02123038")
#pragma comment(linker, "/alternatename:?data_ov071_02123048@@3USharedFilePtr@@A=_data_ov071_02123048")
#pragma comment(linker, "/alternatename:?data_ov071_02123050@@3USharedFilePtr@@A=_data_ov071_02123050")
#pragma comment(linker, "/alternatename:?data_ov071_021226a0@@3USharedFilePtr@@A=_data_ov071_021226a0")
#pragma comment(linker, "/alternatename:?data_ov071_021226a4@@3PAPAUSharedFilePtr@@A=_data_ov071_021226a4")
/* data_ov071_021230b8 is the gravity Vector3 __sinit_ov071_02122a1c seats
   {0, -0x19000, 0}. MrI_Projectile::Behavior spells it `Vector3` and
   MrI_Projectile::InitResources spells the SAME address `void *` -- one
   storage, two spellings, the data_ov071_02122ecc_d case in the sibling fill. */
#pragma comment(linker, "/alternatename:?data_ov071_021230b8@@3UVector3@@A=_data_ov071_021230b8")
#pragma comment(linker, "/alternatename:?data_ov071_021230b8@@3PAXA=_data_ov071_021230b8")

// ---- the trap --------------------------------------------------------------
static void mri_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define MRI_TRAP(n) \
    static int __fastcall mri_trap##n(void *s, void *) \
    { mri_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
MRI_TRAP(13) MRI_TRAP(14) MRI_TRAP(30)
#undef MRI_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall mri_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall mri_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall mri_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall mri_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall mri_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall mri_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall mri_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall mri_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall mri_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall mri_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall mri_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall mri_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall mri_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall mri_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall mri_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall mri_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall mri_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall mri_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall mri_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall mri_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall mri_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ---- MR. I's own slots ------------------------------------------------------
static int __fastcall mri_init(void *s, void *)
{ return _ZN3MrI13InitResourcesEv((char *)s); }
static int __fastcall mri_clean(void *s, void *)
{ (void)s; return _ZN3MrI16CleanupResourcesEv(); }
static int __fastcall mri_behavior(void *s, void *)
{ return ((MrI *)s)->MrI::Behavior(); }
/* Render is the MrI_Render host copy (ROM-order ModelAnim slot-5 dispatch, the
   Whomp/Fish case), not the matched TU. */
static int __fastcall mri_render(void *s, void *)
{ port_actor_render_probe("MR_I", (char *)s + 0xd4);
  return _ZN3MrI6RenderEv(s); }
static int __fastcall mri_pdes(void *s, void *)
{ (void)s; _ZN3MrI16OnPendingDestroyEv(); return 0; }
static int __fastcall mri_d0(void *s, void *)
{ return (int)(size_t)_ZN3MrID0Ev((int *)s); }
/* D1, the complete-object destructor ROM slot 16 holds: the D0 chain without
   the Memory::Deallocate at its tail. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (mri_d1) spelled the same chain by hand. */

// ---- MR_I_PROJECTILE's own slots -------------------------------------------
static int __fastcall mrp_init(void *s, void *)
{ return ((MrI_Projectile *)s)->MrI_Projectile::InitResources(); }
static int __fastcall mrp_clean(void *s, void *)
{ (void)s; return _ZN14MrI_Projectile16CleanupResourcesEv(); }
static int __fastcall mrp_behavior(void *s, void *)
{ return ((MrI_Projectile *)s)->MrI_Projectile::Behavior(); }
/* The projectile's Render is a real method with NO vtable dispatch in it (two
   Particle::System::NewUnkCallback818 calls), so the matched TU is used --
   unlike MrI::Render, which is this lane's ModelAnim slot-5 host copy. */
static int __fastcall mrp_render(void *s, void *)
{ return ((MrI_Projectile *)s)->MrI_Projectile::Render(); }
static int __fastcall mrp_pdes(void *s, void *)
{ (void)s; _ZN14MrI_Projectile16OnPendingDestroyEv(); return 0; }
static int __fastcall mrp_d0(void *s, void *)
{ return (int)(size_t)_ZN14MrI_ProjectileD0Ev((int *)s); }
static int __fastcall mrp_yoshi(void *s, void *)
{ (void)s; return func_ov071_02121b00(); }
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (mrp_d1) spelled the same chain by hand. */

/* the twenty-three shared-half seats both tables get, written once
   (slots 1,2,4,5,7,8,10,11,13,14,15,19-30 -- counted from this macro, after a
   review caught the earlier "eighteen") */
#define MRI_SHARED_TAIL(vt)                       \
    vt[1]  = (void *)mri_binit;                   \
    vt[2]  = (void *)mri_ainit;                   \
    vt[4]  = (void *)mri_bclean;                  \
    vt[5]  = (void *)mri_aclean;                  \
    vt[7]  = (void *)mri_bbeh;                    \
    vt[8]  = (void *)mri_abeh;                    \
    vt[10] = (void *)mri_bren;                    \
    vt[11] = (void *)mri_aren;                    \
    vt[13] = (void *)mri_trap13;                  \
    vt[14] = (void *)mri_trap14;                  \
    vt[15] = (void *)mri_heap;                    \
    vt[19] = (void *)mri_egg;                     \
    vt[20] = (void *)mri_v50;                     \
    vt[21] = (void *)mri_pounded;                 \
    vt[22] = (void *)mri_atk1;                    \
    vt[23] = (void *)mri_atk2;                    \
    vt[24] = (void *)mri_kicked;                  \
    vt[25] = (void *)mri_pushed;                  \
    vt[26] = (void *)mri_cannon;                  \
    vt[27] = (void *)mri_mega;                    \
    vt[28] = (void *)mri_under;                   \
    vt[29] = (void *)mri_aimed;                   \
    vt[30] = (void *)mri_trap30;

/* MR_I (262) and BIG_MR_I (263) share this one table -- both factories store
   _ZTV3MrI, and the class tells the two apart by its own actor id. */
extern "C" void hal_fill_mri_vtable(void)
{
    void **vt = (void **)_ZTV3MrI;
    vt[0]  = (void *)mri_init;
    vt[3]  = (void *)mri_clean;
    vt[6]  = (void *)mri_behavior;
    vt[9]  = (void *)mri_render;
    vt[12] = (void *)mri_pdes;
    vt[16] = (void *)hal_cppd1_MrI;
    vt[17] = (void *)mri_d0;
    vt[18] = (void *)mri_yoshi;   /* Actor's default; MrI has no override */
    MRI_SHARED_TAIL(vt)
}

extern "C" void hal_fill_mri_projectile_vtable(void)
{
    void **vt = (void **)_ZTV14MrI_Projectile;
    vt[0]  = (void *)mrp_init;
    vt[3]  = (void *)mrp_clean;
    vt[6]  = (void *)mrp_behavior;
    vt[9]  = (void *)mrp_render;
    vt[12] = (void *)mrp_pdes;
    vt[16] = (void *)hal_cppd1_MrI_Projectile;
    vt[17] = (void *)mrp_d0;
    vt[18] = (void *)mrp_yoshi;   /* its OWN, func_ov071_02121b00, returns 4 */
    MRI_SHARED_TAIL(vt)
}
