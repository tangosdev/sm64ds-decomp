// RUN REL0215 WAVE 1 (lane cast-sweep1): OV035'S FOUR CLASSES (level 27,
// Tick Tock Clock). Overlay 1 of 6 in this lane.
//
// Level 27 spawns 70 actors and skips 57 across fourteen ids on tip
// f77f01169. NINETEEN of those instances across FOUR ids are this overlay's
// own cast, and they are the largest single block on the level:
//
//   id   name                 x on L27  factory                  table
//   119  ROTATING_CLOCK_HAND   2        RotatingClockHand_Spawn  0x02112b00
//   120  SPINNING_PLATFORM    13        SpinningPlatform_Spawn   0x02112bcc
//   121  ROTATING_COG_BIG      2        func_ov035_0211168c      0x02112b00
//   122  ROTATING_COG_SMALL    2        RotatingCogSmall_Spawn   0x02112b00
//
// ov035 is a LEVEL overlay, whole-mounted since wave 17 and per-symbol mounted
// since wave 18. This lane adds its OWN cast to that per-symbol mount, the
// ov012/ov013/ov025/ov045/ov052/ov056 dual-mount shape applied again. The
// widths, the four T3 pins in .data, the four in .bss, both attribution routes
// and the two alias races are derived in port/ov035_syms.txt's second header;
// the slice and its refusal audit are in port/slice_sweep1_ov035.txt.
//
// ---- TWO TABLES, FOUR IDS, AND THE DSD LABELS ARE SHIFTED ------------------
//
// Disassembled from extracted/overlays/overlay_0035.bin (T4), all four
// factories:
//     0x0211165c RotatingCogSmall_Spawn   alloc 0x330  installs 0x02112b00
//     0x0211168c func_ov035_0211168c      alloc 0x330  installs 0x02112b00
//     0x021116bc RotatingClockHand_Spawn  alloc 0x330  installs 0x02112b00
//     0x02111b98 SpinningPlatform_Spawn   alloc 0x380  installs 0x02112bcc
// THREE ids share the table dsd named _ZTV16RotatingCogSmall; the table dsd
// named _ZTV17RotatingClockHand is SPINNING_PLATFORM's alone. The method
// bodies follow the tables, not the ids -- the _ZN16RotatingCogSmall* bodies
// serve 119/121/122, the _ZN17RotatingClockHand* bodies serve 120 -- so the
// dsd spellings are kept as the HOST ARRAY NAMES (which is what lets the
// matched TUs link unchanged) while every registry row is wired by address.
// The independent confirmation is the shared class's own InitResources:
// it branches CMP r0,#0x77 / CMP r0,#0x79 (119 / 121) at 0x021114e0 to pick
// one of three model files, which is only meaningful if one class serves
// three ids.
//
// ---- BOTH TABLES ARE 32-SLOT PLATFORM, ROM-SHAPED (T1) --------------------
//
// Neither class derives from Model or ModelAnim -- both hold a plain Model at
// +0xd4 as a MEMBER (their destructors call _ZN5ModelD1Ev on this+0xd4), so
// the default applies and both tables stay ROM-shaped: [16] D1, [17] D0, and
// the 32-slot Platform close
//     29 _ZN5Actor16OnAimedAtWithEggEv        0x02010124
//     30 _ZN5Actor25OnAimedAtWithEggReturnVecEv 0x020100dc  (trapped here)
//     31 _ZN8Platform4KillEv                  0x020ee55c
// verified slot by slot against config/arm9/overlays/ov035/relocs.txt before
// a single word was replaced. Both dsd landings under-read badly (2 slots and
// 4 slots), so both tables are EXCLUDED from the mount and hosted here.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov045/ov052/ov056/ov073 lane-ownership pattern: no lane owns
// hal/actor_overlays.cpp this wave, so the bring-up rides the first registry
// fill behind one DSSTATE-bracketed done-guard. It runs the pack check, the
// syms patch, hal_fill_platform_vtable (all four destructors install ov002
// 0x0210ae38 as the base vptr on the way out, and that array is plain zeroed
// storage until the fill runs) and then BOTH ov035 sinits, which is all of
// them -- unlike ov019/ov025 there is no class here without reach.
//
// AND IT IS THE FIRST TIME THE OV035 MOUNT HAS EVER BEEN PATCHED. Nothing on
// tip f77f01169 calls port_ov035_pack_check() or port_ov035_syms_patch();
// wave 18 landed the list and the CMake row and no caller. The twelve
// cross-overlay blocks that mount exists for were still holding whatever the
// emitter left them.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half -- the same words ov025's two platform tables, ov045's
   six, ov052's two and ov056's one carry, checked slot for slot against BOTH
   ROM tables before this fill was written. */
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
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */

/* the generated ov035 per-symbol mount (build/port/host-src/ov035_syms.c) */
void port_ov035_pack_check(void);
void port_ov035_syms_patch(void);
/* BOTH of ov035's sinits -- every class in the overlay is registered here, so
   unlike ov019/ov025 none is left unrun. 02111f04 builds the three model
   SharedFilePtrs and the one collision pointer of the 119/121/122 class;
   02111fc0 builds id 120's pair. */
void __sinit_ov035_02111f04(void);
void __sinit_ov035_02111fc0(void);

/* the bodies src spells with C names (port/slice_sweep1_ov035.txt) */
int *_ZN16RotatingCogSmallD1Ev(int *self);        /* 0x02112b00 slot 16 */
int *_ZN16RotatingCogSmallD0Ev(int *self);        /* 0x02112b00 slot 17 */
int *_ZN17RotatingClockHandD1Ev(int *self);       /* 0x02112bcc slot 16 */
int *_ZN17RotatingClockHandD0Ev(int *self);       /* 0x02112bcc slot 17 */
void *RotatingCogSmall_Spawn(void);               /* id 122 */
void *func_ov035_0211168c(void);                  /* id 121 */
void *RotatingClockHand_Spawn(void);              /* id 119 */
void *SpinningPlatform_Spawn(void);               /* id 120 */

/* the two host vtables, both 32-slot Platform, both excluded from the mount */
DSSTATE_BEGIN
void *_ZTV16RotatingCogSmall[32];    /* 0x02112b00, ids 119/121/122 */
void *_ZTV17RotatingClockHand[32];   /* 0x02112bcc, id 120 */
DSSTATE_END
}

/* The RTTI spellings the four destructors restore their tables by. dsd names
   0x02112bcc twice (_ZTV17RotatingClockHand and _ZTV16daObjCtMecha11_c) and
   0x02112b00 once, and the matched destructors reach both through the RTTI
   name; 16daObjCtMecha10_c / 16daObjCtMecha11_c are the strings at
   0x02112a90 / 0x02112b94, read out of the overlay image. All three LHS are
   excluded from the mount and defined nowhere else, so the aliases cannot be
   defeated. The second store both destructors make, _ZTV10dBgActor_c, is
   Platform's base table and is already a host array in
   hal/actor_classes.cpp. */
#pragma comment(linker, "/alternatename:__ZTV16daObjCtMecha10_c=__ZTV16RotatingCogSmall")
#pragma comment(linker, "/alternatename:__ZTV16daObjCtMecha11_c=__ZTV17RotatingClockHand")

/* id 120's CleanupResources spells its two SharedFilePtrs G0 and G1 -- the
   resolver placeholders -- and G0 means something ELSE in the two deleting
   destructors (the game heap). Both are routed by a per-source -D onto the two
   private names below rather than straight onto data_ov035_02112cb0/b8,
   because that TU also includes decl_common.h, which already declares both of
   those as `extern char` scalars: a direct -D is a C2040 redeclaration, the
   ov100 path-lift trap. G0 is declared in the TU itself OUTSIDE its extern "C"
   block so MSVC mangles it; G1 comes from decl_common.h, which is inside one,
   so it stays a C name. Hence the two different LHS shapes -- both measured
   off the link's own unresolved list, not guessed. The pairing is the ROM's:
   0x0211192c holds 0x02112cb0 (model, file 1488) and 0x02111930 holds
   0x02112cb8 (collision, file 1489), released in that order. */
#pragma comment(linker, "/alternatename:?port_ov035_sp_file0@@3PAHA=_data_ov035_02112cb0")
#pragma comment(linker, "/alternatename:_port_ov035_sp_file1=_data_ov035_02112cb8")
/* The same treatment for the OTHER alias race, the ov056 spelling of ov035's
   own collision SharedFilePtr at 0x02112c68 in the 119/121/122 class's
   CleanupResources. It could not take a direct -D either: decl_common.h
   declares BOTH spellings, `extern char data_ov056_02112c68[]` at 262 and
   `extern int data_ov035_02112c68[]` at 1149, so renaming one onto the other
   is a C2371 different-basic-types redefinition -- measured, not predicted.
   The name is declared inside decl_common.h's extern "C" block, so this LHS
   is a C name; it is defined nowhere, so the alias cannot be defeated. */
#pragma comment(linker, "/alternatename:_port_ov035_rcs_clsn=_data_ov035_02112c68")

/* ---- THE MEASURED EXTERNAL GAP -------------------------------------------
   Eight unresolved externals off the FIRST link of this slice, read out of
   the linker's own LNK2019 list rather than predicted. Six are the ov025
   C++-MANGLED SPELLING class: three of the slice's TUs declare globals at
   FILE SCOPE, below their own extern "C" block instead of inside it, so MSVC
   decorates each with its declared type while the mount and romdata emit the
   one C name. Every LHS below is declared and never defined, so
   alternatename_guard stays clean. */
#pragma comment(linker, "/alternatename:?data_ov035_02112c78@@3PAHA=_data_ov035_02112c78")
#pragma comment(linker, "/alternatename:?data_ov035_02112c70@@3PAHA=_data_ov035_02112c70")
#pragma comment(linker, "/alternatename:?data_ov035_02112c60@@3PAHA=_data_ov035_02112c60")
#pragma comment(linker, "/alternatename:?data_ov035_02111ef4@@3PAY03FA=_data_ov035_02111ef4")
#pragma comment(linker, "/alternatename:?data_ov035_02111ef0@@3PAFA=_data_ov035_02111ef0")
/* the RNG seed, arm9. hal/cxx_aliases.cpp:1476 already binds the SCALAR
   spelling ?data_0209e650@@3HA; id 120's Behavior declares it as an array, so
   it needs the pointer spelling too. Same object, different decoration. */
#pragma comment(linker, "/alternatename:?data_0209e650@@3PAHA=_data_0209e650")
/* MovingMeshCollider::SetFile, declared as a mangled-in-mangled FREE function
   by the 119/121/122 InitResources (the ov060 precedent at line 262 of
   hal/actor_classes_ov060.cpp, which is the same alias with a different return
   type in the decoration: that TU declares it int, this one void). */
#pragma comment(linker, "/alternatename:?_ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block@@YAXPAX00HF0@Z=__ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block")
/* id 120's InitResources installs MeshColliderBase::UpdatePosWithTransform as
   the collider's BeforeClsn callback -- the contested-slot family c9a1731da
   seats -- but takes its address through the resolver's own invented name
   UpdatePosWithTransformSym (decl_common.h:1966, declared and defined
   nowhere). Bound to the same static method the bob_world / updownlift /
   ov060 aliases bind their three spellings to. */
#pragma comment(linker, "/alternatename:_UpdatePosWithTransformSym=?UpdatePosWithTransform@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")

/* The four class bodies src defines as real C++ methods rather than extern-"C"
   free functions, faced here -- the ov013/ov025 recipe. Render IS faced: both
   Renders are plain-Model slot-5 bare-call shadows over +0xd4, and
   _ZTV5Model[5] is dual-filled in hal/cxxname_bridge.cpp (the Tree/ov013
   precedent, NOT the ModelAnim slot-5 collision -- neither class holds a
   ModelAnim). */
#include "RotatingCogSmall.h"
#include "RotatingClockHand.h"
extern "C" {
int _ZN16RotatingCogSmall13InitResourcesEv(void *self)
{ return ((RotatingCogSmall *)self)->RotatingCogSmall::InitResources(); }
int _ZN16RotatingCogSmall16CleanupResourcesEv(void *self)
{ return ((RotatingCogSmall *)self)->RotatingCogSmall::CleanupResources(); }
int _ZN16RotatingCogSmall8BehaviorEv(void *self)
{ return ((RotatingCogSmall *)self)->RotatingCogSmall::Behavior(); }
int _ZN16RotatingCogSmall6RenderEv(void *self)
{ return ((RotatingCogSmall *)self)->RotatingCogSmall::Render(); }
int _ZN17RotatingClockHand13InitResourcesEv(void *self)
{ return ((RotatingClockHand *)self)->RotatingClockHand::InitResources(); }
int _ZN17RotatingClockHand16CleanupResourcesEv(void *self)
{ return ((RotatingClockHand *)self)->RotatingClockHand::CleanupResources(); }
int _ZN17RotatingClockHand8BehaviorEv(void *self)
{ return ((RotatingClockHand *)self)->RotatingClockHand::Behavior(); }
int _ZN17RotatingClockHand6RenderEv(void *self)
{ return ((RotatingClockHand *)self)->RotatingClockHand::Render(); }
}

// ---- the trap --------------------------------------------------------------
static void ov35_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov035 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov035 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV35_TRAP(n) \
    static int __fastcall ov35_trap##n(void *s, void *) \
    { ov35_trap_report(s, n); return 0; }
OV35_TRAP(13) OV35_TRAP(14) OV35_TRAP(30)
#undef OV35_TRAP

static int __fastcall ov35_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov35_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov35_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov35_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov35_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov35_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov35_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov35_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov35_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov35_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov35_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov35_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov35_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov35_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov35_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov35_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov35_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov35_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov35_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov35_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov35_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov35_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 31, the Platform tail, the word both widths are pinned by. */
static int __fastcall ov35_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half of both tables. The caller writes its own 0/3/6/9/16/17 and
   31.

   THE POINTER IS VOLATILE ON PURPOSE: MSVC 19.44 x86 /O2 deletes a static
   filler's calls from BOTH call sites when it is called with two distinct
   extern-array arguments, dropping every shared-half store from the object
   file -- the gate-200 elided-stores bug, repro and dumpbin proof in
   hal/actor_classes_ov002g200.cpp. This helper has exactly two callers. */
static void ov35_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov35_binit;
    vt[2]  = (void *)ov35_ainit;
    vt[4]  = (void *)ov35_bclean;
    vt[5]  = (void *)ov35_aclean;
    vt[7]  = (void *)ov35_bbeh;
    vt[8]  = (void *)ov35_abeh;
    vt[10] = (void *)ov35_bren;
    vt[11] = (void *)ov35_aren;
    vt[12] = (void *)ov35_pdes;
    vt[13] = (void *)ov35_trap13;
    vt[14] = (void *)ov35_trap14;
    vt[15] = (void *)ov35_heap;
    vt[18] = (void *)ov35_yoshi;
    vt[19] = (void *)ov35_turn_egg;
    vt[20] = (void *)ov35_v50;
    vt[21] = (void *)ov35_pounded;
    vt[22] = (void *)ov35_atk1;
    vt[23] = (void *)ov35_atk2;
    vt[24] = (void *)ov35_kicked;
    vt[25] = (void *)ov35_pushed;
    vt[26] = (void *)ov35_cannon;
    vt[27] = (void *)ov35_mega;
    vt[28] = (void *)ov35_under;
    vt[29] = (void *)ov35_egg;
    vt[30] = (void *)ov35_trap30;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov35_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers. Bracketed, the pass re-runs exactly when its
   results were rolled away. */
DSSTATE_BEGIN
static int g_ov35_bringup_done;
DSSTATE_END

extern "C" void port_ov35_bringup(void)
{
    if (g_ov35_bringup_done)
        return;
    g_ov35_bringup_done = 1;
    port_ov035_pack_check();
    port_ov035_syms_patch();
    /* before the sinits and before any teardown: all four destructors install
       ov002 0x0210ae38 as the base vptr on the way out, and that array is
       plain zeroed storage until this fill runs. */
    hal_fill_platform_vtable();
    __sinit_ov035_02111f04();
    __sinit_ov035_02111fc0();
}

// ============================================================================
// ROTATING_CLOCK_HAND (119) + ROTATING_COG_BIG (121) + ROTATING_COG_SMALL
// (122) -- ONE class, ONE table 0x02112b00, RTTI 16daObjCtMecha10_c.
// ============================================================================
//
// An 816-byte object: Model at +0xd4, MovingMeshCollider at +0x124, the
// collider's transform source at +0x2ec and a rotation block at +0x300.
// InitResources reads the actor's own id out of +0xc and branches three ways
// -- 0x77 (119) takes the collision path (model file 1486, KCL file 1487, CLPS
// 0x021121d8, UpdatePosAndAngs installed as the BeforeClsn callback and
// mRotationState 0), 0x79 (121) takes model file 1490 and 122 takes model file
// 1491, both with mRotationState 1 and no collider. Behavior is a two-mode
// spin: with the language/mode selector data_0209f2c0[0] == 3 it only tracks
// the model and rebuilds the collider transform when Platform::IsClsnInRange
// says the player is near; otherwise it approaches its target angle, and on
// selector 2 it re-randomises direction and dwell out of RandomIntInternal.
// One fill serves all three registry rows -- the table is the same array and
// the fill is idempotent, the ov013 CLOCK_HAND_SHORT/LONG shape.
static int __fastcall rcs_init(void *s, void *)
{ return _ZN16RotatingCogSmall13InitResourcesEv(s); }
static int __fastcall rcs_clean(void *s, void *)
{ return _ZN16RotatingCogSmall16CleanupResourcesEv(s); }
static int __fastcall rcs_behavior(void *s, void *)
{ return _ZN16RotatingCogSmall8BehaviorEv(s); }
static int __fastcall rcs_render(void *s, void *)
{ port_actor_render_probe("ROTATING_COG", (char *)s + 0xd4);
  return _ZN16RotatingCogSmall6RenderEv(s); }
static int __fastcall rcs_d1(void *s, void *)
{ return (int)(size_t)_ZN16RotatingCogSmallD1Ev((int *)s); }
static int __fastcall rcs_d0(void *s, void *)
{ return (int)(size_t)_ZN16RotatingCogSmallD0Ev((int *)s); }

extern "C" void hal_fill_rotating_cog_vtable(void)
{
    port_ov35_bringup();
    void *volatile *vt = (void *volatile *)_ZTV16RotatingCogSmall;
    ov35_fill_shared(vt);
    vt[0]  = (void *)rcs_init;
    vt[3]  = (void *)rcs_clean;
    vt[6]  = (void *)rcs_behavior;
    vt[9]  = (void *)rcs_render;
    vt[16] = (void *)rcs_d1;
    vt[17] = (void *)rcs_d0;
    vt[31] = (void *)ov35_kill;
}

// ============================================================================
// SPINNING_PLATFORM (120) -- table 0x02112bcc, RTTI 16daObjCtMecha11_c.
// Thirteen instances on level 27, the largest population in this lane.
// ============================================================================
//
// An 896-byte object: Model at +0xd4, MovingMeshCollider at +0x124, a second
// transform at +0x350 and a ShadowModel at +0x328 that the factory constructs
// and both destructors tear down first. InitResources loads model file 1488
// and KCL file 1489, hands the collider CLPS 0x02112238, installs
// MeshColliderBase::UpdatePosWithTransform as the BeforeClsn callback (the
// contested-slot family c9a1731da seats), then raycasts straight down from
// 0xa000 above its own origin and parks the ground height in +0x324 -- which
// is what func_ov035_02111798 later reads to scale the drop shadow by the
// platform's height above the floor. Behavior indexes the four-short table
// data_ov035_02112b80 with the selector data_0209f2c0[0] for its per-frame
// spin, and on selector 2 re-randomises sign and dwell.
static int __fastcall sp_init(void *s, void *)
{ return _ZN17RotatingClockHand13InitResourcesEv(s); }
static int __fastcall sp_clean(void *s, void *)
{ return _ZN17RotatingClockHand16CleanupResourcesEv(s); }
static int __fastcall sp_behavior(void *s, void *)
{ return _ZN17RotatingClockHand8BehaviorEv(s); }
static int __fastcall sp_render(void *s, void *)
{ port_actor_render_probe("SPINNING_PLATFORM", (char *)s + 0xd4);
  return _ZN17RotatingClockHand6RenderEv(s); }
static int __fastcall sp_d1(void *s, void *)
{ return (int)(size_t)_ZN17RotatingClockHandD1Ev((int *)s); }
static int __fastcall sp_d0(void *s, void *)
{ return (int)(size_t)_ZN17RotatingClockHandD0Ev((int *)s); }

extern "C" void hal_fill_spinning_platform_vtable(void)
{
    port_ov35_bringup();
    void *volatile *vt = (void *volatile *)_ZTV17RotatingClockHand;
    ov35_fill_shared(vt);
    vt[0]  = (void *)sp_init;
    vt[3]  = (void *)sp_clean;
    vt[6]  = (void *)sp_behavior;
    vt[9]  = (void *)sp_render;
    vt[16] = (void *)sp_d1;
    vt[17] = (void *)sp_d0;
    vt[31] = (void *)ov35_kill;
}
