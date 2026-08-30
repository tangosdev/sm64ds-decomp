// RUN LINKW WAVE 5 (lane w5-B): HAZY MAZE CAVE'S CAST -- ov065's HMC subset
// (SNUFIT 236, SWOOP 237, DORRIE 168, DORRIE_CAP 169) and ov021's resident
// trio (WORK_ELEVATOR 68, ROLLING_ROCK 221, SHUTTER_HMC 67). Level 13's
// census skip list is exactly these seven classes plus three ids other
// mounts own (318 ov095 / 31 ov100 / 262 ov071; 262 is lane 5-A's, see the
// lane report). Same law as hal/actor_classes_ov045.cpp, the wave-4 exemplar
// this file is built on, and hal/actor_classes_scuttlebug.cpp for the
// Enemy-31-slot shape.
//
// ---- THE CAST MAP WAS RE-DERIVED, NOT CARRIED ------------------------------
// The five reads, per class (details in port/ov065_syms.txt and
// port/ov021_syms.txt): (1) arm9 spawn table 0x02090864 + id*4 out of
// extracted/arm9_dec.bin; (2) each SpawnInfo's raw word[1] id halfword out of
// extracted/overlays/overlay_0065.bin / overlay_0021.bin (both overlays are
// compressed:true, the dsd exports are the wrong images to read); (3) each
// record's word[0] Spawn address landing in its own class block; (4) each
// table's slot 0 landing in that class's own code; (5) each Spawn's own
// vtable-store relocation. UNLIKE ov045 there is NO naming shift in either
// overlay: every dsd name sits in its own class's block.
//
// ---- WIDTHS, each pinned by the reloc run AND the next dsd symbol ----------
//   _ZTV6Snufit        0x0211cba4  31 slots (Enemy shape; own 18/19/29)
//   _ZTV5Swoop         0x0211cca0  31 slots (same)
//   _ZTV9DorrieCap     0x0211cdc4  31 slots + 2 tail words: word31 = 0,
//                      word32 -> 0x0211cd40 (the daDossy_c typeinfo). No
//                      reloc reads either word, but a reloc-invisible
//                      vptr[32] read cannot be ruled out, so the host array
//                      is the FULL 33-word span -- the slot-31 lesson
//                      (hal/actor_classes_wf.cpp's header) run forward.
//   _ZTV6Dorrie        0x0211ce48  31 slots
//   _ZTV12WorkElevator 0x0211478c  32 slots, slot 31 Platform::Kill
//   _ZTV11RollingRock  0x02114848  31 slots (Actor shape, own 29)
//   _ZTV10ShutterHmc   0x02114914  32 slots, slot 31 Platform::Kill; the
//                      span crosses overlays.yaml's floor-rounded code_size
//                      (relocs carry slots 27..31 past 0x02114980)
//
// ---- THE STATE MACHINES ARE HOSTED, THE ROM RECORDS ARE SEATED -------------
// Snufit, Swoop and Dorrie drive per-state machines through {fn, 0} records
// their sinits copy from .data source statics into bss; the fn words are DS
// CODE addresses and the matched dispatchers form MSVC pointers-to-member
// over forward-declared classes (the 16-byte widening gate 173 measured).
// port_ov065_states_seat rewrites the source statics with host bodies before
// the sinits run, and the three Behaviors plus the two one-line dispatch
// helpers are host copies: port/unmatched/Ov065_StateDispatch.cpp,
// Snufit_Behavior.cpp, Swoop_Behavior.cpp, Dorrie_Behavior.cpp (the last
// also hosts func_ov065_02118838, the one HMC body src/ never banked).
//
// ---- FACTORY SHAPES --------------------------------------------------------
// Six of the seven Spawns store their OWN table by a real name as the final
// vptr write: registered directly. ShutterHmc_Spawn is the ov045 factory-F
// shape (base 0x021099e4 first, own 0x02114914 second, the own store spelled
// VT1 = auto_bss zeros); port_factory_shutter_hmc reseats p[0]. The
// derivation is in port/ov021_syms.txt's header, from the raw words.
//
// ---- NAME RACES RESOLVED BY ADDRESS (the shared-window rule) ---------------
//   data_ov075_0211d608 / data_ov075_0211d650  -> ov065's own bss (Snufit's
//       SharedFilePtrs; the ov075 spelling is another module's dsd export
//       winning the race at a shared-window address)
//   func_ov075_0211478c -> _ZTV12WorkElevator (same race, ov021's window)
//   _ZTV15daYurei_Mucho_c / _ZTV12daBasabasa_c / _ZTV12daDossyCap_c /
//   _ZTV9daGrock_c -> each class's own host array (the dtors restore the
//       table by the RTTI spelling)
//   _ZTV16daObjCvShutter_c -> _ZTV10ShutterHmc: in BOTH dtors the name is on
//       the OWN-table store (literal 0x02114914); only the recovered Spawn
//       binds it to the base literal, and the factory wrapper makes that
//       store transient either way.
//
// ---- LANE OWNERSHIP: the mount bring-up rides the first registry fill ------
// The w4-b pattern: no lane owns hal/actor_overlays.cpp this wave, so
// ov65_bringup (one done-guard, called by every fill) runs the two pack
// checks, the two patch passes, the states seat, and the SIX sinits --
// ov065's c110/c2a8/c440 (Snufit / Swoop / Dorrie+Cap) and ov021's
// 02113500/02113688/021136c8 (WorkElevator / RollingRock / ShutterHmc).
// RUN REL0215 WAVE 3 (lane w3-a2): the seven Ttc sinits no longer "stay out".
// The Ttc subset is seated at the bottom of this file, all eleven of its ids
// are registered, and ttc_bringup down there runs c660/c76c/c7d8/c890/c8fc/
// c9b8/ca74 -- the ov019 rule satisfied rather than dodged. They hang off the
// Ttc fills rather than off ov65_bringup so a target that links this file for
// Hazy Maze Cave alone still does not run them.
// THE HANDOFF: whoever next owns hal/actor_overlays.cpp should move the
// bring-up body beside the ov013 block and cut the guard here to a call.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half (every address from this lane's own reloc runs) */
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
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26  */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27  */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28  */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* 29  */
void _ZN8Platform4KillEv(void *self);                              /* 31  */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
/* the same decline NAMING its receiver, so the quarantine freezes the actor
   that reached the unhosted body rather than whichever actor's phase callback
   the walk happened to be inside; the wave-3 Ttc loud face is its customer */
void port_actor_slot_decline_for(void *actor, const char *what);
void port_actor_render_probe(const char *cls, void *model);

/* the generated per-symbol mounts (build/host-src/ov065_syms.c, ov021_syms.c) */
void port_ov065_pack_check(void);
void port_ov065_syms_patch(void);
void port_ov021_pack_check(void);
void port_ov021_syms_patch(void);
/* the state-record seat (port/unmatched/Ov065_StateDispatch.cpp) */
void port_ov065_states_seat(void);
/* the six sinits that run; the seven Ttc ones stay out (the ov019 rule) */
void __sinit_ov065_0211c110(void);   /* Snufit's SharedFilePtrs + states  */
void __sinit_ov065_0211c2a8(void);   /* Swoop's                           */
void __sinit_ov065_0211c440(void);   /* Dorrie's + DorrieCap's            */
void __sinit_ov021_02113500(void);   /* WorkElevator's                    */
void __sinit_ov021_02113688(void);   /* RollingRock's                     */
void __sinit_ov021_021136c8(void);   /* ShutterHmc's                      */
}

// ---- the trap --------------------------------------------------------------
static void ov65_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: hmc-cast vtable slot %d is not hosted (actor id "
                 "%u %s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted hmc-cast vtable slot %d on id "
                    "%u %s", slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV65_TRAP(n) \
    static int __fastcall ov65_trap##n(void *s, void *) \
    { ov65_trap_report(s, n); return 0; }
OV65_TRAP(13) OV65_TRAP(14) OV65_TRAP(17) OV65_TRAP(30)
#undef OV65_TRAP

// ---- name races and the decorated-data bridge ------------------------------
// Two kinds of alias, both the scuttlebug file's precedent:
//
// (1) BY-ADDRESS races. func_ov065_021162c0 (a Snufit state) spells its call
// to 0x02115f84 `_ZN6EyerokD0Ev` -- ov066 shares this load window and its
// dsd export won the naming race; the reloc at 0x02116314 lands on ov065's
// OWN func_ov065_02115f84, in this lane's slice. Slicing Eyerok's real D0
// (the probe's class-b suggestion) would have called another overlay's
// destructor on a live Snufit -- the window trap, caught by re-deriving.
//
// THE /alternatename THAT USED TO SIT HERE IS GONE. run rel0215 wave 2, lane
// cast-ov066 landed ov066 and with it src/_ZN6EyerokD0Ev.c, so
// __ZN6EyerokD0Ev became a DEFINED symbol and the alias was defeated silently
// -- which is exactly what port/build-port.cmd:92 and
// port/tools/alternatename_guard.py:20 both predicted would happen "if their
// overlays land". The binding is now a per-source -D on the one caller,
// -D_ZN6EyerokD0Ev=func_ov065_02115f84 in port/CMakeLists.txt, the
// func_ov065_02117994 / w5b_review.md R2 recipe. Eyerok's D0 is size 0xb8 and
// ov065's body at the same address is size 0x6c; the sizes alone settle which
// one this TU means.
//
// (2) DECORATED-DATA spellings. The cast's .cpp TUs declare mounted storage
// without extern "C", so MSVC mangles the type into the name; every one
// binds onto the one C-named mount array (the ?data_020a0e68@@3UMtx43@@A
// precedent). The three ov075 spellings are the shared-window race again --
// all three addresses are ov065's own bss, Snufit's SharedFilePtrs.
#pragma comment(linker, "/alternatename:?data_ov021_02113a60@@3UMatrix4x3@@A=_data_ov021_02113a60")
#pragma comment(linker, "/alternatename:?data_ov021_02113a80@@3UMatrix4x3@@A=_data_ov021_02113a80")
#pragma comment(linker, "/alternatename:?data_ov021_021149a0@@3USharedFilePtr@@A=_data_ov021_021149a0")
#pragma comment(linker, "/alternatename:?data_ov021_021149a8@@3USharedFilePtr@@A=_data_ov021_021149a8")
#pragma comment(linker, "/alternatename:?data_ov021_021149b0@@3USharedFilePtr@@A=_data_ov021_021149b0")
#pragma comment(linker, "/alternatename:?data_ov021_021149b8@@3USharedFilePtr@@A=_data_ov021_021149b8")
#pragma comment(linker, "/alternatename:?data_ov021_02114a50@@3USharedFilePtr@@A=_data_ov021_02114a50")
#pragma comment(linker, "/alternatename:?data_ov065_0211c080@@3PAPAUSharedFilePtr@@A=_data_ov065_0211c080")
#pragma comment(linker, "/alternatename:?data_ov065_0211c080@@3PAPAXA=_data_ov065_0211c080")
#pragma comment(linker, "/alternatename:?data_ov065_0211c08c@@3PAPAUSharedFilePtr@@A=_data_ov065_0211c08c")
#pragma comment(linker, "/alternatename:?data_ov065_0211c08c@@3PAPAXA=_data_ov065_0211c08c")
#pragma comment(linker, "/alternatename:?data_ov065_0211d600@@3USharedFilePtr@@A=_data_ov065_0211d600")
#pragma comment(linker, "/alternatename:?data_ov065_0211d618@@3USharedFilePtr@@A=_data_ov065_0211d618")
#pragma comment(linker, "/alternatename:?data_ov065_0211d670@@3UPMF@@A=_data_ov065_0211d670")
#pragma comment(linker, "/alternatename:?data_ov065_0211d690@@3USharedFilePtr@@A=_data_ov065_0211d690")
#pragma comment(linker, "/alternatename:?data_ov065_0211d698@@3USharedFilePtr@@A=_data_ov065_0211d698")
#pragma comment(linker, "/alternatename:?data_ov065_0211d6a0@@3USharedFilePtr@@A=_data_ov065_0211d6a0")
#pragma comment(linker, "/alternatename:?data_ov065_0211d6a8@@3USharedFilePtr@@A=_data_ov065_0211d6a8")
#pragma comment(linker, "/alternatename:?data_ov065_0211d700@@3UPMF@@A=_data_ov065_0211d700")
#pragma comment(linker, "/alternatename:?data_ov065_0211d720@@3HA=_data_ov065_0211d720")
#pragma comment(linker, "/alternatename:?data_ov065_0211d720@@3USharedFilePtr@@A=_data_ov065_0211d720")
#pragma comment(linker, "/alternatename:?data_ov075_0211d608@@3USharedFilePtr@@A=_data_ov065_0211d608")
#pragma comment(linker, "/alternatename:?data_ov075_0211d610@@3USharedFilePtr@@A=_data_ov065_0211d610")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9c0@@3HA=_data_ov002_0210d9c0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9c0@@3UG@@A=_data_ov002_0210d9c0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9c0@@3USharedFilePtr@@A=_data_ov002_0210d9c0")
//
// WorkElevator::InitResources declares its five callback bodies as plain
// int OBJECTS (it only takes their addresses); bind the decorated data
// spellings onto the function symbols.
#pragma comment(linker, "/alternatename:?func_ov021_02111edc@@3HA=_func_ov021_02111edc")
#pragma comment(linker, "/alternatename:?func_ov021_02111f34@@3HA=_func_ov021_02111f34")
#pragma comment(linker, "/alternatename:?func_ov021_02111f8c@@3HA=_func_ov021_02111f8c")
#pragma comment(linker, "/alternatename:?func_ov021_02111fe4@@3HA=_func_ov021_02111fe4")
#pragma comment(linker, "/alternatename:?func_ov021_02112128@@3HA=_func_ov021_02112128")
//
// Dorrie's Init spells MeshColliderBase::Enable with a void return over
// struct Actor; the linked body is the int-return spelling. Same __thiscall
// body, eax ignored -- the GetFloorResult precedent.
#pragma comment(linker, "/alternatename:?Enable@MeshColliderBase@@QAEXPAUActor@@@Z=?Enable@MeshColliderBase@@QAEHPAUActor@@@Z")

// ---- the shared half, slots 1..30 of the 31-slot Actor/Enemy shape ---------
// Every word read off this lane's seven reloc runs; they agree slot for slot
// outside each class's own overrides. Callers write their own 0/3/6/9/16/17
// and any own tail slots after this returns; the two Platforms write 31.
// Slots 13/14 trap (ActorBase Virtual34/38, the wf reading) and 30 traps
// (returns a Vector3 by value; the SRET contract is unproved -- the
// scuttlebug ruling). THE POINTER IS VOLATILE ON PURPOSE: the gate-200
// elided-stores bug (hal/actor_classes_ov002g200.cpp), seven callers here.
static int __fastcall ov65_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov65_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov65_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov65_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov65_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov65_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov65_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov65_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov65_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov65_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov65_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov65_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov65_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov65_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov65_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov65_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov65_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov65_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov65_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov65_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov65_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov65_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov65_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

static void ov65_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov65_binit;
    vt[2]  = (void *)ov65_ainit;
    vt[4]  = (void *)ov65_bclean;
    vt[5]  = (void *)ov65_aclean;
    vt[7]  = (void *)ov65_bbeh;
    vt[8]  = (void *)ov65_abeh;
    vt[10] = (void *)ov65_bren;
    vt[11] = (void *)ov65_aren;
    vt[12] = (void *)ov65_pdes;
    vt[13] = (void *)ov65_trap13;
    vt[14] = (void *)ov65_trap14;
    vt[15] = (void *)ov65_heap;
    vt[17] = (void *)ov65_trap17;   /* replaced by every caller (real D0s) */
    vt[18] = (void *)ov65_yoshi;
    vt[19] = (void *)ov65_turn_egg;
    vt[20] = (void *)ov65_v50;
    vt[21] = (void *)ov65_pounded;
    vt[22] = (void *)ov65_atk1;
    vt[23] = (void *)ov65_atk2;
    vt[24] = (void *)ov65_kicked;
    vt[25] = (void *)ov65_pushed;
    vt[26] = (void *)ov65_cannon;
    vt[27] = (void *)ov65_mega;
    vt[28] = (void *)ov65_under;
    vt[29] = (void *)ov65_egg;
    vt[30] = (void *)ov65_trap30;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov65_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov65_bringup_done;
DSSTATE_END

extern "C" void port_ov65_bringup(void)
{
    if (g_ov65_bringup_done)
        return;
    g_ov65_bringup_done = 1;
    port_ov065_pack_check();
    port_ov065_syms_patch();
    port_ov021_pack_check();
    port_ov021_syms_patch();
    /* the seat MUST run between the patch passes and the sinits: it verifies
       the mounted ROM words and rewrites them, and the sinits copy them */
    port_ov065_states_seat();
    __sinit_ov065_0211c110();
    __sinit_ov065_0211c2a8();
    __sinit_ov065_0211c440();
    __sinit_ov021_02113500();
    __sinit_ov021_02113688();
    __sinit_ov021_021136c8();
}

// ============================================================================
// SNUFIT (id 236) -- table 0x0211cba4, 31 slots, Enemy shape
// ============================================================================
//
// 996-byte object built over Enemy: MovingCylinderClsn +0x110, WithMeshClsn
// +0x144, ModelAnim +0x300, ShadowModel +0x364. The mask ghost that bobs and
// spits: its Behavior is the host copy (state machine, see the header).
extern "C" {
int _ZN6Snufit16CleanupResourcesEv(void);            /* slot 3, .c */
int _ZN6Snufit8BehaviorEv(void *self);               /* slot 6, HOST COPY */
void _ZN6Snufit16OnPendingDestroyEv(void);           /* slot 12, .c */
int *_ZN6SnufitD1Ev(int *t);                         /* slot 16 */
int *_ZN6SnufitD0Ev(int *t);                         /* slot 17 */
int func_ov065_02116f40(void);                       /* slot 18 */
int func_ov065_02116f14(char *c, void *player);      /* slot 19 */
int func_ov065_02116f0c(void);                       /* slot 29 */
void *Snufit_Spawn(void);
DSSTATE_BEGIN
void *_ZTV6Snufit[31];
DSSTATE_END
}
#include "Snufit.h"
/* yurei mucho = the ghost; SnufitD0 restores the table by this spelling */
#pragma comment(linker, "/alternatename:__ZTV15daYurei_Mucho_c=__ZTV6Snufit")
/* two Snufit state TUs spell their SharedFilePtrs with the ov075 prefix --
   the shared-window naming race; both addresses are ov065's own bss (config
   lines 267/274), built by Snufit's own sinit */
#pragma comment(linker, "/alternatename:_data_ov075_0211d608=_data_ov065_0211d608")
#pragma comment(linker, "/alternatename:_data_ov075_0211d650=_data_ov065_0211d650")
static int __fastcall sn_init(void *s, void *)
{ return ((Snufit *)s)->Snufit::InitResources(); }
static int __fastcall sn_clean(void *s, void *)
{ (void)s; return _ZN6Snufit16CleanupResourcesEv(); }
static int __fastcall sn_behavior(void *s, void *)
{ return _ZN6Snufit8BehaviorEv(s); }
/* RUN LINKW WAVE 19: was `((Snufit *)s)->Snufit::Render()`, the matched TU.
   That body dispatches slot 5 of a local six-virtual shadow over the ModelAnim
   at 0x300, and the host _ZTV9ModelAnim numbers slot 5 as Virtual18 -- a live
   c0000005 the moment a SNUFIT is drawn (measured with SM64DS_SPAWN_ACTOR=236
   on level 13; the banner in port/unmatched/W19_Slot5_Renders.cpp carries the
   fault). The host copy costs the matched TU and is the same trade
   Butterfly/Whomp/Amp already made. */
extern "C" int port_w19_snufit_render(void *self);
static int __fastcall sn_render(void *s, void *)
{ port_actor_render_probe("SNUFIT", (char *)s + 0x300);
  return port_w19_snufit_render(s); }
static int __fastcall sn_pdes(void *s, void *)
{ (void)s; _ZN6Snufit16OnPendingDestroyEv(); return 0; }
static int __fastcall sn_d1(void *s, void *)
{ return (int)(size_t)_ZN6SnufitD1Ev((int *)s); }
static int __fastcall sn_d0(void *s, void *)
{ return (int)(size_t)_ZN6SnufitD0Ev((int *)s); }
static int __fastcall sn_yoshi(void *s, void *)
{ (void)s; return func_ov065_02116f40(); }
static int __fastcall sn_egg(void *s, void *, void *p)
{ func_ov065_02116f14((char *)s, p); return 0; }
static int __fastcall sn_aimed(void *s, void *)
{ (void)s; return func_ov065_02116f0c(); }
extern "C" void hal_fill_snufit_vtable(void)
{
    port_ov65_bringup();
    void *volatile *vt = (void *volatile *)_ZTV6Snufit;
    ov65_fill_shared(vt);
    vt[0]  = (void *)sn_init;
    vt[3]  = (void *)sn_clean;
    vt[6]  = (void *)sn_behavior;
    vt[9]  = (void *)sn_render;
    vt[12] = (void *)sn_pdes;
    vt[16] = (void *)sn_d1;
    vt[17] = (void *)sn_d0;
    vt[18] = (void *)sn_yoshi;
    vt[19] = (void *)sn_egg;
    vt[29] = (void *)sn_aimed;
    /* 31 slots: Enemy shape, no Platform tail */
}

// ============================================================================
// SWOOP (id 237) -- table 0x0211cca0, 31 slots, Enemy shape
// ============================================================================
//
// The cave bat: MovingCylinderClsn +0x110, WithMeshClsn +0x144, ModelAnim
// +0x300 (hanging) and +0x364 (flying), ShadowModel +0x3c8; +0x43c selects
// which anim advances. Behavior is the host copy.
extern "C" {
int _ZN5Swoop16CleanupResourcesEv(void);             /* slot 3, .c */
int _ZN5Swoop8BehaviorEv(void *self);                /* slot 6, HOST COPY */
void _ZN5Swoop16OnPendingDestroyEv(void);            /* slot 12, .c */
int *_ZN5SwoopD1Ev(int *t);                          /* slot 16 */
int *_ZN5SwoopD0Ev(int *t);                          /* slot 17 */
int func_ov065_02117ee0(void);                       /* slot 18 */
int func_ov065_02117eb4(char *c, void *player);      /* slot 19 */
int func_ov065_02117eac(void);                       /* slot 29 */
void *Swoop_Spawn(void);
DSSTATE_BEGIN
void *_ZTV5Swoop[31];
DSSTATE_END
}
#include "Swoop.h"
/* basabasa = the flapping; SwoopD0 restores the table by this spelling */
#pragma comment(linker, "/alternatename:__ZTV12daBasabasa_c=__ZTV5Swoop")
static int __fastcall sw_init(void *s, void *)
{ return ((Swoop *)s)->Swoop::InitResources(); }
static int __fastcall sw_clean(void *s, void *)
{ (void)s; return _ZN5Swoop16CleanupResourcesEv(); }
static int __fastcall sw_behavior(void *s, void *)
{ return _ZN5Swoop8BehaviorEv(s); }
/* RUN LINKW WAVE 19: the same ModelAnim slot-5 collision, over BOTH of
   SWOOP's ModelAnims (0x300 and 0x364). Measured with SM64DS_SPAWN_ACTOR=237
   on level 13. See port/unmatched/W19_Slot5_Renders.cpp. */
extern "C" int port_w19_swoop_render(void *self);
static int __fastcall sw_render(void *s, void *)
{ port_actor_render_probe("SWOOP", (char *)s + 0x300);
  return port_w19_swoop_render(s); }
static int __fastcall sw_pdes(void *s, void *)
{ (void)s; _ZN5Swoop16OnPendingDestroyEv(); return 0; }
static int __fastcall sw_d1(void *s, void *)
{ return (int)(size_t)_ZN5SwoopD1Ev((int *)s); }
static int __fastcall sw_d0(void *s, void *)
{ return (int)(size_t)_ZN5SwoopD0Ev((int *)s); }
static int __fastcall sw_yoshi(void *s, void *)
{ (void)s; return func_ov065_02117ee0(); }
static int __fastcall sw_egg(void *s, void *, void *p)
{ func_ov065_02117eb4((char *)s, p); return 0; }
static int __fastcall sw_aimed(void *s, void *)
{ (void)s; return func_ov065_02117eac(); }
extern "C" void hal_fill_swoop_vtable(void)
{
    port_ov65_bringup();
    void *volatile *vt = (void *volatile *)_ZTV5Swoop;
    ov65_fill_shared(vt);
    vt[0]  = (void *)sw_init;
    vt[3]  = (void *)sw_clean;
    vt[6]  = (void *)sw_behavior;
    vt[9]  = (void *)sw_render;
    vt[12] = (void *)sw_pdes;
    vt[16] = (void *)sw_d1;
    vt[17] = (void *)sw_d0;
    vt[18] = (void *)sw_yoshi;
    vt[19] = (void *)sw_egg;
    vt[29] = (void *)sw_aimed;
}

// ============================================================================
// DORRIE (id 168) -- table 0x0211ce48, 31 slots
// ============================================================================
//
// The plesiosaur, 0x11b8 bytes -- the biggest object in the cast: ModelAnim
// +0xec, seven 0x200-stride segment rows from +0x150 (matrix + collider
// each), WithMeshClsn +0xf50, MovingCylinderClsn +0x110c,
// MovingCylinderClsnWithPos +0x1140. Behavior and the neck-chain helper
// func_ov065_02118838 are host copies (port/unmatched/Dorrie_Behavior.cpp).
extern "C" {
int _ZN6Dorrie8BehaviorEv(void *self);               /* slot 6, HOST COPY */
void *_ZN6DorrieD1Ev(void *t);                       /* slot 16, extern-C .cpp */
void *_ZN6DorrieD0Ev(void *t);                       /* slot 17, extern-C .cpp */
void *Dorrie_Spawn(void);
DSSTATE_BEGIN
void *_ZTV6Dorrie[31];
DSSTATE_END
}
#include "Dorrie.h"
static int __fastcall do_init(void *s, void *)
{ return ((Dorrie *)s)->Dorrie::InitResources(); }
static int __fastcall do_clean(void *s, void *)
{ return ((Dorrie *)s)->Dorrie::CleanupResources(); }
static int __fastcall do_behavior(void *s, void *)
{ return _ZN6Dorrie8BehaviorEv(s); }
/* RUN LINKW WAVE 19: the same ModelAnim slot-5 collision, over the ModelAnim
   at 0xec. Measured with SM64DS_SPAWN_ACTOR=168 on level 13. See
   port/unmatched/W19_Slot5_Renders.cpp. */
extern "C" int port_w19_dorrie_render(void *self);
static int __fastcall do_render(void *s, void *)
{ port_actor_render_probe("DORRIE", (char *)s + 0xec);
  return port_w19_dorrie_render(s); }
static int __fastcall do_d1(void *s, void *)
{ return (int)(size_t)_ZN6DorrieD1Ev(s); }
static int __fastcall do_d0(void *s, void *)
{ return (int)(size_t)_ZN6DorrieD0Ev(s); }
extern "C" void hal_fill_dorrie_vtable(void)
{
    port_ov65_bringup();
    void *volatile *vt = (void *volatile *)_ZTV6Dorrie;
    ov65_fill_shared(vt);
    vt[0]  = (void *)do_init;
    vt[3]  = (void *)do_clean;
    vt[6]  = (void *)do_behavior;
    vt[9]  = (void *)do_render;
    vt[16] = (void *)do_d1;
    vt[17] = (void *)do_d0;
}

// ============================================================================
// DORRIE_CAP (id 169) -- table 0x0211cdc4, 31 slots + the 2-word tail
// ============================================================================
//
// The rideable saddle Dorrie spawns (never in a level's own object list --
// registered so Dorrie's runtime ActorBase::Spawn of id 169 finds a class).
// Model +0xf0, MovingCylinderClsn +0x140, an ov001 sub-object at +0xd4.
// Slot 3 is ActorBase::CleanupResources (the reloc's own word, 0x02043bf0)
// and slot 12 the ActorBase default: the cap has no resources of its own.
extern "C" {
int *_ZN9DorrieCapD1Ev(int *t);                      /* slot 16 */
int *_ZN9DorrieCapD0Ev(int *t);                      /* slot 17 */
int func_ov065_021195e4(void);                       /* slot 18 */
void *DorrieCap_Spawn(void);
extern int data_ov065_0211cd40[];   /* daDossy_c typeinfo, mounted; the ROM's
                                       own word at table+0x80 points here */
DSSTATE_BEGIN
void *_ZTV9DorrieCap[33];
DSSTATE_END
}
#include "DorrieCap.h"
/* dossy cap; DorrieCapD0 restores the table by this spelling */
#pragma comment(linker, "/alternatename:__ZTV12daDossyCap_c=__ZTV9DorrieCap")
static int __fastcall dc_init(void *s, void *)
{ return ((DorrieCap *)s)->DorrieCap::InitResources(); }
static int __fastcall dc_clean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall dc_behavior(void *s, void *)
{ return ((DorrieCap *)s)->DorrieCap::Behavior(); }
static int __fastcall dc_render(void *s, void *)
{ port_actor_render_probe("DORRIE_CAP", (char *)s + 0xf0);
  return ((DorrieCap *)s)->DorrieCap::Render(); }
static int __fastcall dc_d1(void *s, void *)
{ return (int)(size_t)_ZN9DorrieCapD1Ev((int *)s); }
static int __fastcall dc_d0(void *s, void *)
{ return (int)(size_t)_ZN9DorrieCapD0Ev((int *)s); }
static int __fastcall dc_yoshi(void *s, void *)
{ (void)s; return func_ov065_021195e4(); }
extern "C" void hal_fill_dorrie_cap_vtable(void)
{
    port_ov65_bringup();
    void *volatile *vt = (void *volatile *)_ZTV9DorrieCap;
    ov65_fill_shared(vt);
    vt[0]  = (void *)dc_init;
    vt[3]  = (void *)dc_clean;
    vt[6]  = (void *)dc_behavior;
    vt[9]  = (void *)dc_render;
    vt[16] = (void *)dc_d1;
    vt[17] = (void *)dc_d0;
    vt[18] = (void *)dc_yoshi;
    /* the ROM's own tail: word 31 stays 0, word 32 is the daDossy_c typeinfo
       pointer -- replicated so a reloc-invisible vptr[32] read lands where
       the ROM's would */
    vt[31] = 0;
    vt[32] = (void *)data_ov065_0211cd40;
}

// ============================================================================
// WORK_ELEVATOR (id 68) -- table 0x0211478c, 32 slots, Platform shape
// ============================================================================
//
// The rideable cage lift, 0xc80 bytes: four Models from +0x320 (0x50
// stride), four MovingMeshColliders from +0x520 (0x1c8 stride), built by
// func_020733a8. Its InitResources hands five callback pairs out by name --
// host code storing host pointers, called back through the path layer, all
// self-consistent. Its rider-push callback (func_ov021_02112024) is what
// pulls the sqrt chain: the four drivers are hostgen sources (SQRTUNIT_SYMS
// in port/CMakeLists.txt; func_02053130 rides hostgen's MMIO_EXTERN table),
// closing the id-141 gap ov045's file names.
extern "C" {
int _ZN12WorkElevator13InitResourcesEv(char *c);     /* slot 0, extern-C .cpp */
int _ZN12WorkElevator8BehaviorEv(void *c);           /* slot 6, .c */
void *_ZN12WorkElevatorD1Ev(void *self);             /* slot 16, extern-C .cpp */
int _ZN12WorkElevatorD0Ev(void *self);               /* slot 17, .c */
void *WorkElevator_Spawn(void);
DSSTATE_BEGIN
void *_ZTV12WorkElevator[32];
DSSTATE_END
}
#include "WorkElevator.h"
/* the shared-window race: WorkElevator_Spawn spells its own-table store
   func_ov075_0211478c; ov021's config names that address _ZTV12WorkElevator.
   PORT_HOST_ABI: shared-window name race -- src/func_ov075_0211478c.c is
   ov075's OWN body at the same window address (a different overlay's image),
   and linking it here would hand WorkElevator VS-mode code for a vtable. */
#pragma comment(linker, "/alternatename:_func_ov075_0211478c=__ZTV12WorkElevator")
static int __fastcall we_init(void *s, void *)
{ return _ZN12WorkElevator13InitResourcesEv((char *)s); }
static int __fastcall we_clean(void *s, void *)
{ return ((WorkElevator *)s)->WorkElevator::CleanupResources(); }
static int __fastcall we_behavior(void *s, void *)
{ return _ZN12WorkElevator8BehaviorEv(s); }
static int __fastcall we_render(void *s, void *)
{ port_actor_render_probe("WORK_ELEVATOR", (char *)s + 0x320);
  return ((WorkElevator *)s)->WorkElevator::Render(); }
static int __fastcall we_d1(void *s, void *)
{ return (int)(size_t)_ZN12WorkElevatorD1Ev(s); }
static int __fastcall we_d0(void *s, void *)
{ return _ZN12WorkElevatorD0Ev(s); }
extern "C" void hal_fill_work_elevator_vtable(void)
{
    port_ov65_bringup();
    void *volatile *vt = (void *volatile *)_ZTV12WorkElevator;
    ov65_fill_shared(vt);
    vt[0]  = (void *)we_init;
    vt[3]  = (void *)we_clean;
    vt[6]  = (void *)we_behavior;
    vt[9]  = (void *)we_render;
    vt[16] = (void *)we_d1;
    vt[17] = (void *)we_d0;
    vt[31] = (void *)ov65_kill;
}

// ============================================================================
// ROLLING_ROCK (id 221) -- table 0x02114848, 31 slots, Actor shape
// ============================================================================
//
// The boulder: Model +0x110, ShadowModel +0x160, MovingCylinderClsnWithPos
// +0x1b8, WithMeshClsn +0x1f8. Own slot 29 (func_ov021_0211228c). The three
// relocated words after its table are dsd's own split symbol
// data_ov021_021148c4 and ride the ov021 join, not this array.
extern "C" {
int _ZN11RollingRock16CleanupResourcesEv(void);      /* slot 3, .c */
int _ZN11RollingRock8BehaviorEv(char *c);            /* slot 6, .c */
int *_ZN11RollingRockD1Ev(int *t);                   /* slot 16 */
int *_ZN11RollingRockD0Ev(int *t);                   /* slot 17 */
int func_ov021_0211228c(void);                       /* slot 29 */
void *RollingRock_Spawn(void);
DSSTATE_BEGIN
void *_ZTV11RollingRock[31];
DSSTATE_END
}
#include "RollingRock.h"
/* grock = the rock; RollingRockD0 restores the table by this spelling */
#pragma comment(linker, "/alternatename:__ZTV9daGrock_c=__ZTV11RollingRock")
static int __fastcall rr_init(void *s, void *)
{ return ((RollingRock *)s)->RollingRock::InitResources(); }
static int __fastcall rr_clean(void *s, void *)
{ (void)s; return _ZN11RollingRock16CleanupResourcesEv(); }
static int __fastcall rr_behavior(void *s, void *)
{ return _ZN11RollingRock8BehaviorEv((char *)s); }
static int __fastcall rr_render(void *s, void *)
{ port_actor_render_probe("ROLLING_ROCK", (char *)s + 0x110);
  return ((RollingRock *)s)->RollingRock::Render(); }
static int __fastcall rr_d1(void *s, void *)
{ return (int)(size_t)_ZN11RollingRockD1Ev((int *)s); }
static int __fastcall rr_d0(void *s, void *)
{ return (int)(size_t)_ZN11RollingRockD0Ev((int *)s); }
static int __fastcall rr_aimed(void *s, void *)
{ (void)s; return func_ov021_0211228c(); }
extern "C" void hal_fill_rolling_rock_vtable(void)
{
    port_ov65_bringup();
    void *volatile *vt = (void *volatile *)_ZTV11RollingRock;
    ov65_fill_shared(vt);
    vt[0]  = (void *)rr_init;
    vt[3]  = (void *)rr_clean;
    vt[6]  = (void *)rr_behavior;
    vt[9]  = (void *)rr_render;
    vt[16] = (void *)rr_d1;
    vt[17] = (void *)rr_d0;
    vt[29] = (void *)rr_aimed;
}

// ============================================================================
// SHUTTER_HMC (id 67) -- table 0x02114914, 32 slots, Platform shape
// ============================================================================
//
// The metal shutter door, 804 bytes, the smallest class here (Init and
// Cleanup are 0x14-byte bodies). Slot 9 Render is ov002's shared body
// func_ov002_020babf0 (already linked), the cross-overlay inherited-slot
// case ov045's (B) block describes. The factory is the wave's one VT1
// wrapper: see the header and port/ov021_syms.txt.
extern "C" {
int func_ov002_020babf0(void *self);                 /* slot 9, ov002 base */
int *_ZN10ShutterHmcD1Ev(int *t);                    /* slot 16 */
int *_ZN10ShutterHmcD0Ev(int *t);                    /* slot 17 */
int *ShutterHmc_Spawn(void);
DSSTATE_BEGIN
void *_ZTV10ShutterHmc[32];
DSSTATE_END
}
#include "ShutterHmc.h"
/* in BOTH dtors the daObjCvShutter_c spelling is on the OWN-table store
   (literal 0x02114914); the Spawn's binding of the same name to the base
   literal is transient behind the factory wrapper */
#pragma comment(linker, "/alternatename:__ZTV16daObjCvShutter_c=__ZTV10ShutterHmc")
extern "C" void *port_factory_shutter_hmc(void)
{
    void *p = ShutterHmc_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV10ShutterHmc;
    return p;
}
static int __fastcall sh_init(void *s, void *)
{ return ((ShutterHmc *)s)->ShutterHmc::InitResources(); }
static int __fastcall sh_clean(void *s, void *)
{ return ((ShutterHmc *)s)->ShutterHmc::CleanupResources(); }
static int __fastcall sh_behavior(void *s, void *)
{ return ((ShutterHmc *)s)->ShutterHmc::Behavior(); }
static int __fastcall sh_render(void *s, void *)
{ port_actor_render_probe("SHUTTER_HMC", (char *)s + 0xd4);
  return func_ov002_020babf0(s); }
static int __fastcall sh_d1(void *s, void *)
{ return (int)(size_t)_ZN10ShutterHmcD1Ev((int *)s); }
static int __fastcall sh_d0(void *s, void *)
{ return (int)(size_t)_ZN10ShutterHmcD0Ev((int *)s); }
extern "C" void hal_fill_shutter_hmc_vtable(void)
{
    port_ov65_bringup();
    void *volatile *vt = (void *volatile *)_ZTV10ShutterHmc;
    ov65_fill_shared(vt);
    vt[0]  = (void *)sh_init;
    vt[3]  = (void *)sh_clean;
    vt[6]  = (void *)sh_behavior;
    vt[9]  = (void *)sh_render;
    vt[16] = (void *)sh_d1;
    vt[17] = (void *)sh_d0;
    vt[31] = (void *)ov65_kill;
}

// ============================================================================
// RUN REL0215 WAVE 3 (lane w3-a2): ov065's TICK TOCK CLOCK CLUSTER
// ============================================================================
//
// ELEVEN ids (108..118), SEVEN classes, SEVEN 32-slot Platform tables, SEVEN
// static initialisers. The cast map is derived four independent ways in
// port/ov065_syms.txt's wave-3 section and the slice is port/slice_w3a.txt;
// neither is repeated here. What IS here is the four things a reader of this
// file has to know to trust the fills below.
//
// ---- 1. THE dsd BODY NAMES ARE SHIFTED AND THE SpawnInfo NAMES ARE NOT -----
// From 0x0211d3b4 on, every dsd _ZTV/body spelling carries the PREVIOUS
// class's name while the *_SpawnInfo records keep their own. So
// TtcRotatingGear_Spawn is id 114's FACTORY and _ZN15TtcRotatingGear* are ids
// 116/117's BODIES -- one prefix, two classes. Every array below is named for
// the RTTI string at its own table[-1], read out of ov065's shipped image,
// because that is the only spelling that is right on both halves. The Ttc
// spellings the FACTORIES store are aliased onto them.
//
// ---- 2. THE TWO TABLES dsd LABELLED PLAIN DATA ARE REAL CLASSES ------------
// 0x0211d0ec (id 110) and 0x0211d2b4 (id 113) carry 16daObjCtMecha03_c and
// 16daObjCtMecha05_c at table[-1]. They were in ov065's per-symbol mount as
// data_ov065_0211d0ec / data_ov065_0211d2b4 only because dsd called them data;
// this lane took them OUT, because a mounted vtable hands a factory DS code
// addresses and that is the one thing the rule forbids -- which is why the five
// tables dsd DID label _ZTV were left out from the start. They are host arrays
// here like the other five -- under the CONFIG's own names,
// data_ov065_0211d0ec and data_ov065_0211d2b4, because the RTTI spellings are
// host-only and vtspan.py cannot resolve a fill to a table that has no ROM
// symbol. Measured before removing them: of the 25 relocations in ov065 naming
// any of the seven tables, every one names the TABLE ADDRESS and not one names
// a prologue word, so nothing reads the offset-to-top or typeinfo the widened
// mount gap now covers.
//
// ---- 3. FOUR TUs REACH THEIR STORAGE THROUGH PLACEHOLDERS THAT ARE ALREADY
//         DEFINED, so a missing -D LINKS CLEAN AND IS WRONG AT RUN TIME ------
// VT0/VT1 are real definitions in hal/actor_vtables.cpp and bare G0/G1 are
// alternatename'd in hal/cxx_aliases.cpp to data_020a0eac (the game heap) and
// data_ov002_0210e05c (SignPost's collision file). A Ttc TU that spells VT0 and
// gets no -D therefore writes hal/actor_vtables.cpp's dummy array as its vptr
// and the link says nothing. The four are named in port/CMakeLists.txt with
// their per-source COMPILE_DEFINITIONS and every target address below was read
// out of the body's OWN literal pool in extracted/overlays/overlay_0065.bin:
//
//   func_ov065_02119f88 (id 110 D0)   VT0 0x0211d0ec  VT1 0x0210ae38
//   func_ov065_0211abac (id 113 D0)   VT0 0x0211d2b4  VT1 0x0210ae38
//   func_ov065_0211a15c (id 110 clean) G0 0x0211d88c  G1 0x0211d894
//   func_ov065_0211ad04 (id 113 clean) G0 0x0211d904  G1 0x0211d90c
//   TtcRotatingGear::CleanupResources  G0 0x0211d98c  G1 0x0211d97c
//   TtcMovingCubeA::CleanupResources   G0 0x0211d9d4  G1 0x0211d9cc
//
// The last four are the pairs the sinits BUILD, which is the same ownership
// read from the other side: c76c writes 0211d88c.., c890 writes d904.., c9b8
// writes d97c.., ca74 writes d9cc.. The five plain D0s spell G0 as the HEAP
// argument of Memory::Deallocate, and bare _G0 already binds to data_020a0eac,
// so those five correctly take no -D. Two TUs in the same shape with DIFFERENT
// answers (0211a15c and 0211ad04 are the same source text needing different
// pairs) is why this is per source and not one shared define.
//
// ---- 4. THE METHOD FACES DECLARE THEIR OWN CLASSES ------------------------
// Five of the seven classes have real C++ method bodies, and the obvious route
// -- include their generated headers -- does not work uniformly:
// include/TTC_MovingBar.h declares InitResources/CleanupResources/Render and
// NOT Behavior, while src/_ZN13TTC_MovingBar8BehaviorEv.cpp declares its own
// local class carrying only Behavior. MSVC decorates a member by name,
// signature and calling convention and not by layout, so a local declaration
// here produces the identical ?Method@Class@@QAEHXZ the definition emits --
// which is exactly what that body TU itself relies on. Declaring locally is
// uniform across all five and cannot go stale against a generated header that
// gains or loses a row.
/* The five classes' method faces. Layout is deliberately absent: nothing here
   does anything with `this` except forward it.

   GLOBAL SCOPE, NOT AN ANONYMOUS NAMESPACE, and that is load-bearing rather
   than style: MSVC decorates a class inside an anonymous namespace with the
   TU's own unnamed-namespace tag (?A0x...), so every call would emit a symbol
   no definition anywhere carries and the link would fail on five names that
   read correctly. */
class TtcRotatingCube      { public: int InitResources(); int CleanupResources();
                                     int Behavior(); int Render(); };
class TtcConveyorBeltLarge { public: int InitResources(); int CleanupResources();
                                     int Behavior(); int Render(); };
class TTC_MovingBar        { public: int InitResources(); int CleanupResources();
                                     int Behavior(); int Render(); };
class TtcRotatingGear      { public: int InitResources(); int CleanupResources();
                                     int Render(); };
class TtcMovingCubeA       { public: int CleanupResources(); int Render(); };

extern "C" {
/* The seven host tables, named for the RTTI string at each table[-1]. 32 slots
   each: slot 30 Actor::OnAimedAtWithEggReturnVec and slot 31 Platform::Kill --
   the wave-17 correction, checked again here against each table's own span. */
DSSTATE_BEGIN
void *_ZTV20daObjCtRotateBlock_c[32];   /* 0x0211d028, ids 108/109 */
/* NAMED FOR THE CONFIG, NOT FOR THE RTTI, and these two alone. dsd labelled
   both tables plain data, so _ZTV16daObjCtMecha03_c / _ZTV16daObjCtMecha05_c
   are HOST-ONLY names -- there is no such ROM symbol to resolve them to, and
   port/tools/vtspan.py's --fills pass says so out loud ("is a host-only name")
   and would put two new rows in a ratchet whose list may only shrink. Naming
   the arrays data_ov065_0211d0ec / _0211d2b4 gives vtspan the config row it
   needs: each one's next config symbol lands 0x80 later, which is 32 slots, so
   the tool now checks these fills against the ROM's own span instead of giving
   up on them. That span is also a THIRD independent confirmation of the slot
   count, after the reloc run and the Platform::Kill tail.
   The RTTI spellings every factory and destructor uses are aliased on below. */
void *data_ov065_0211d0ec[32];          /* 0x0211d0ec, id  110, Mecha03 */
void *_ZTV16daObjCtMecha04_c[32];       /* 0x0211d1ec, ids 111/112 */
void *data_ov065_0211d2b4[32];          /* 0x0211d2b4, id  113, Mecha05 */
void *_ZTV18daObjCtKaitendai_c[32];     /* 0x0211d3b4, ids 114/115 */
void *_ZTV16daObjCtMecha08_c[32];       /* 0x0211d494, ids 116/117 */
void *_ZTV16daObjCtMecha09_c[32];       /* 0x0211d568, id  118     */
DSSTATE_END

/* the two C-symbol bodies among the forty-two slots: id 118's InitResources
   and ids 116/117's Behavior are extern "C" free functions in src, not
   methods, so they are called by name rather than through a face */
int _ZN14TtcMovingCubeA13InitResourcesEv(char *c);   /* 0x0211bf04, slot 0  */
int _ZN15TtcRotatingGear8BehaviorEv(char *c);        /* 0x0211b8f8, slot 6  */

/* every destructor is a .c body taking and returning int* */
int *_ZN15TtcRotatingCubeD1Ev(int *t);          int *_ZN15TtcRotatingCubeD0Ev(int *t);
int *_ZN20TtcConveyorBeltLargeD1Ev(int *t);     int *_ZN20TtcConveyorBeltLargeD0Ev(int *t);
int *_ZN13TTC_MovingBarD1Ev(int *t);            int *_ZN13TTC_MovingBarD0Ev(int *t);
int *_ZN15TtcRotatingGearD1Ev(int *t);          int *_ZN15TtcRotatingGearD0Ev(int *t);
int *_ZN14TtcMovingCubeAD1Ev(int *t);           int *_ZN14TtcMovingCubeAD0Ev(int *t);

/* ids 110 and 113 have no class methods at all: every slot is a func_ov065_* */
int func_ov065_0211a358(char *c);   int func_ov065_0211a15c(void *t);
int func_ov065_0211a1c8(char *c);   int func_ov065_0211a1a0(void *t);
int *func_ov065_02119f3c(int *t);   int *func_ov065_02119f88(int *t);
int func_ov065_0211b1d4(char *c);   int func_ov065_0211ad04(void *t);
int func_ov065_0211ae08(char *c);   int func_ov065_0211ad48(void *t);
int *func_ov065_0211ab60(int *t);   int *func_ov065_0211abac(int *t);

/* the seven parked static initialisers, all now owned by a registered class */
void __sinit_ov065_0211c660(void);   /* daObjCtRotateBlock_c, 108/109 */
void __sinit_ov065_0211c76c(void);   /* daObjCtMecha03_c,     110     */
void __sinit_ov065_0211c7d8(void);   /* daObjCtMecha04_c,     111/112 */
void __sinit_ov065_0211c890(void);   /* daObjCtMecha05_c,     113     */
void __sinit_ov065_0211c8fc(void);   /* daObjCtKaitendai_c,   114/115 */
void __sinit_ov065_0211c9b8(void);   /* daObjCtMecha08_c,     116/117 */
void __sinit_ov065_0211ca74(void);   /* daObjCtMecha09_c,     118     */

/* ov035's per-symbol mount: the four .text-pool CLPS blocks the Ttc bodies
   name at LINK time (data_ov035_02112118 / _02112198 / _021121b8 / _02112258)
   are ITS storage.

   WHICH CONFIGURATION THIS MATTERS IN, stated precisely because the developer
   build makes it look unnecessary. In build/port the generated arrays are
   STATICALLY INITIALISED with the ROM's own bytes -- read
   build/port/host-src/ov035_syms.c and data_ov035_02112198 is literally
   { 67,76,80,83, 8,0, 2,0, ... }, 'CLPS' with entrySize 8 and count 2 -- so
   they are right from program start and nothing has to run first. The SHIPPING
   configuration is the other one: under PORT_ROM_CLEAN ovdata.py emits an
   apply pass that fills the image from romdata.bin at run time instead, and
   there the order does decide whether a Ttc InitResources reads the block or
   thirty-two zeros. Thirty-two zeros is not a crash: func_020381cc answers an
   entrySize that is not 8 with the SHARED DEFAULT ENTRY and no fault at all,
   which is the wave-17 silent shape exactly.

   So the two passes are called here rather than left to port_ov35_bringup(),
   whose own trigger is ov035's registry fills -- "the ov035 rows happen to be
   registered before the Ttc rows" is not a fact this file should rest on in
   the configuration that ships. Both are pure absolute assignment and
   idempotent, so running them from both places writes the same words twice.
   These are the GENERATED mount's entry points, not
   hal/actor_classes_ov035.cpp, so this pulls in no part of ov035's own seat. */
void port_ov035_pack_check(void);
void port_ov035_syms_patch(void);
/* Platform's base table, which every one of the ten Ttc destructors installs as
   the second vptr on its way out; plain zeroed storage until this fill runs. */
void hal_fill_platform_vtable(void);
}  /* extern "C" */

/* THE FACTORY SPELLINGS, ALIASED ONTO THE RTTI ARRAYS. Each Ttc factory stores
   the table under dsd's shifted name while each destructor restores it under
   the RTTI name, so both spellings have to reach one array. Every LHS is
   declared in include/decl_common.h (inside its extern "C" block, so all are C
   names) and DEFINED nowhere, which is what alternatename_guard checks. Ids 110
   and 113 need no alias: their factory and their destructors both spell the
   RTTI name, because dsd never produced a Ttc spelling for either table. */
#pragma comment(linker, "/alternatename:__ZTV15TtcRotatingCube=__ZTV20daObjCtRotateBlock_c")
#pragma comment(linker, "/alternatename:__ZTV20TtcConveyorBeltLarge=__ZTV16daObjCtMecha04_c")
#pragma comment(linker, "/alternatename:__ZTV13TTC_MovingBar=__ZTV18daObjCtKaitendai_c")
#pragma comment(linker, "/alternatename:__ZTV15TtcRotatingGear=__ZTV16daObjCtMecha08_c")
#pragma comment(linker, "/alternatename:__ZTV14TtcMovingCubeA=__ZTV16daObjCtMecha09_c")
/* Ids 110 and 113 the other way round: their arrays carry the CONFIG name (see
   the declaration above) and it is the RTTI spelling -- the only spelling their
   factories and destructors use -- that is aliased on. Both LHS are declared in
   decl_common.h and defined nowhere, so neither alias can be defeated. */
#pragma comment(linker, "/alternatename:__ZTV16daObjCtMecha03_c=_data_ov065_0211d0ec")
#pragma comment(linker, "/alternatename:__ZTV16daObjCtMecha05_c=_data_ov065_0211d2b4")

/* THE SIX LANE-OWNED PLACEHOLDER RENAMES. port/CMakeLists.txt gives each of the
   four TUs a per-source -D onto these names and they are bound here. The names
   are declared NOWHERE ELSE on purpose -- that is the ov035 remedy for the
   C2372 a direct -D would raise, because include/decl_common.h already declares
   data_ov065_0211d904 / _0211d90c as `extern char` scalars and _0211d97c /
   _0211d98c as `extern char []`, so rewriting G0 into one of those names
   redefines a line of that header at a different type.

   THE DECORATION DIFFERS BETWEEN THE TWO HALVES and it was read off the link's
   own unresolved list rather than predicted. decl_common.h wraps lines
   20..3189 in `extern "C"`, so a G1 rename is always a C name; a .cpp TU's own
   file-scope `extern int G0[];` sits outside that block and MSVC decorates it.
   Hence ?...@@3PAHA on the two .cpp G0 rows and a plain C name on everything
   else. The two VT rows are .c bodies and take the cdecl form only. */
#pragma comment(linker, "/alternatename:_port_ov065_m03_file0=_data_ov065_0211d88c")
#pragma comment(linker, "/alternatename:_port_ov065_m03_file1=_data_ov065_0211d894")
#pragma comment(linker, "/alternatename:_port_ov065_m05_file0=_data_ov065_0211d904")
#pragma comment(linker, "/alternatename:_port_ov065_m05_file1=_data_ov065_0211d90c")
#pragma comment(linker, "/alternatename:?port_ov065_m08_file0@@3PAHA=_data_ov065_0211d98c")
#pragma comment(linker, "/alternatename:_port_ov065_m08_file1=_data_ov065_0211d97c")
#pragma comment(linker, "/alternatename:?port_ov065_m09_file0@@3PAHA=_data_ov065_0211d9d4")
#pragma comment(linker, "/alternatename:_port_ov065_m09_file1=_data_ov065_0211d9cc")

/* ==== THE MEASURED EXTERNAL GAP, 29 rows =================================
 *
 * Read out of the linker's OWN LNK2019/LNK2001 list on the first link of this
 * slice, not predicted. Every LHS is a spelling some Ttc TU emits and no
 * object defines; every RHS was checked to exist before the row was written.
 * None of the LHS names is defined anywhere, so alternatename_guard stays
 * clean -- and its post-link map check is what keeps that true.
 *
 * ---- 1. THE FOUR .text LITERAL-POOL CLPS BLOCKS (4 rows) -----------------
 * These are the other half of ov065's twelve level-window reads and the half
 * hal/ttc_level_data_seat.cpp cannot touch. They are literal-pool words inside
 * ov065's .text; the port compiles that .text from source, so there is no word
 * to re-patch -- the compiled TU takes the block's ADDRESS and it is fixed at
 * LINK time. They bind to ov035's per-symbol mount, which run linkw wave 18
 * built for exactly this. That is correct for the only level these classes
 * spawn in: all fifty mounted levels were swept and only level 27's object
 * table names any of ids 108..118.
 *
 *   func_02112118  id 118's CLPS   func_02112198  id 110's CLPS
 *   func_021121b8  ids 116/117's   func_02112258  id 113's CLPS
 *
 * In build/port those arrays are statically initialised with the ROM's bytes;
 * under PORT_ROM_CLEAN they are filled at run time instead, which is why
 * ttc_bringup calls port_ov035_pack_check() and port_ov035_syms_patch() rather
 * than trusting registration order. An unfilled block reads entrySize 0, and
 * func_020381cc answers anything but 8 with the SHARED DEFAULT ENTRY and no
 * fault at all -- the wave-17 silent shape in a new place, closed here rather
 * than left to an ordering. The full argument is at the call site.
 *
 * dsd spells all four func_* because it saw the level overlay's bytes as code;
 * they are data, and the CLPS readback in ttc_level_data_seat.cpp is what says
 * so out loud.
 *
 * ---- 2. THE C++-MANGLED SPELLING CLASS (9 fn + 1 method + 15 data) -------
 * The ov025/ov035 class, and by far the biggest group. A .cpp TU that declares
 * a ROM symbol at FILE SCOPE, below its own extern "C" block instead of inside
 * it, gets MSVC's decoration on it while the definition and every mount carry
 * the one C name. The decoration encodes the DECLARED types, so two TUs that
 * declare the same function differently produce two different LHS for one
 * object -- which is why _ZN9ModelBase7SetFileEP8BMD_Fileii needs two rows
 * here (PAX0HH from one TU, PAXPAUBMD_File@@HH from another) and why
 * hal/cxx_aliases.cpp already carries three more decorations of it and of
 * Model::LoadFile from earlier slices. Same object, different spellings, one
 * row each.
 *
 * RaycastGround::SetObjAndPos is the one real C++ METHOD in the group and it
 * is NOT in the list below, because AN ALIAS IS THE WRONG TOOL FOR IT and this
 * lane found that out by crashing. See THE ONE ROW THAT HAD TO BE A BRIDGE,
 * under the list.
 *
 * func_ov065_0211aacc is a FUNCTION its caller declares as `extern int`, so it
 * decorates as DATA (@@3HA) -- the address of a function taken through an int
 * declaration. The alias is data-spelling to function, which is a name
 * question and not a type one at this layer.
 */
/* the four .text-pool CLPS blocks -> ov035's per-symbol mount */
#pragma comment(linker, "/alternatename:?func_02112118@@3UCLPS_Block@@A=_data_ov035_02112118")
#pragma comment(linker, "/alternatename:_func_02112198=_data_ov035_02112198")
#pragma comment(linker, "/alternatename:_func_021121b8=_data_ov035_021121b8")
#pragma comment(linker, "/alternatename:_func_02112258=_data_ov035_02112258")
/* decorated free-function spellings -> the one C name each body defines */
#pragma comment(linker, "/alternatename:?_ZN11ShadowModel10InitCuboidEv@@YAXPAX@Z=__ZN11ShadowModel10InitCuboidEv")
#pragma comment(linker, "/alternatename:?_ZN12MeshCollider8LoadFileER13SharedFilePtr@@YAPAUKCL_File@@AAUSharedFilePtr@@@Z=__ZN12MeshCollider8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block@@YAXPAXPAUKCL_File@@ABUMatrix4x3@@HFAAUCLPS_Block@@@Z=__ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block")
#pragma comment(linker, "/alternatename:?_ZN5Model8LoadFileER13SharedFilePtr@@YAPAUBMD_File@@AAUSharedFilePtr@@@Z=__ZN5Model8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN8Platform19UpdateClsnPosAndRotEv@@YAXPAX@Z=__ZN8Platform19UpdateClsnPosAndRotEv")
#pragma comment(linker, "/alternatename:?_ZN8Platform21UpdateModelPosAndRotYEv@@YAXPAX@Z=__ZN8Platform21UpdateModelPosAndRotYEv")
#pragma comment(linker, "/alternatename:?_ZN9ModelBase7SetFileEP8BMD_Fileii@@YAXPAX0HH@Z=__ZN9ModelBase7SetFileEP8BMD_Fileii")
#pragma comment(linker, "/alternatename:?_ZN9ModelBase7SetFileEP8BMD_Fileii@@YAXPAXPAUBMD_File@@HH@Z=__ZN9ModelBase7SetFileEP8BMD_Fileii")
/* decorated data spellings -> the mount's / romdata's one C name */
#pragma comment(linker, "/alternatename:?data_0209f2c0@@3EA=_data_0209f2c0")
#pragma comment(linker, "/alternatename:?data_020a0e68@@3PAXA=_data_020a0e68")
#pragma comment(linker, "/alternatename:?data_ov065_0211c0b8@@3PAHA=_data_ov065_0211c0b8")
#pragma comment(linker, "/alternatename:?data_ov065_0211c0d0@@3PADA=_data_ov065_0211c0d0")
#pragma comment(linker, "/alternatename:?data_ov065_0211c0d4@@3PADA=_data_ov065_0211c0d4")
#pragma comment(linker, "/alternatename:?data_ov065_0211cfd0@@3PAPAXA=_data_ov065_0211cfd0")
#pragma comment(linker, "/alternatename:?data_ov065_0211cfd0@@3PAUE@@A=_data_ov065_0211cfd0")
#pragma comment(linker, "/alternatename:?data_ov065_0211cfd4@@3PAPAXA=_data_ov065_0211cfd4")
#pragma comment(linker, "/alternatename:?data_ov065_0211cfd4@@3PAUE@@A=_data_ov065_0211cfd4")
#pragma comment(linker, "/alternatename:?data_ov065_0211d194@@3PADA=_data_ov065_0211d194")
#pragma comment(linker, "/alternatename:?data_ov065_0211d198@@3PADA=_data_ov065_0211d198")
#pragma comment(linker, "/alternatename:?data_ov065_0211d88c@@3USharedFilePtr@@A=_data_ov065_0211d88c")
#pragma comment(linker, "/alternatename:?data_ov065_0211d894@@3USharedFilePtr@@A=_data_ov065_0211d894")
#pragma comment(linker, "/alternatename:?data_ov065_0211d9cc@@3USharedFilePtr@@A=_data_ov065_0211d9cc")
#pragma comment(linker, "/alternatename:?data_ov065_0211d9d4@@3USharedFilePtr@@A=_data_ov065_0211d9d4")
#pragma comment(linker, "/alternatename:?func_ov065_0211aacc@@3HA=_func_ov065_0211aacc")

/* ==== THE ONE ROW THAT HAD TO BE A BRIDGE, NOT AN ALIAS ===================
 *
 * src/_ZN14TtcMovingCubeA13InitResourcesEv.cpp raycasts the ground under the
 * beam through a local RaycastGround shadow whose SetObjAndPos it declares
 * returning int, so it emits the __thiscall member
 * ?SetObjAndPos@RaycastGround@@QAEHABUVector3@@PAUActor@@@Z. The body is the
 * C-named _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor, a CDECL free
 * function taking the receiver as its FIRST STACK ARGUMENT.
 *
 * ALIASING ONE ONTO THE OTHER LINKS CLEAN AND IS AN ABI SWAP. The caller puts
 * the receiver in ECX and `&v` at [esp+4]; the cdecl body reads its `self`
 * from [esp+4] and its `v` from [esp+8]. So `self` becomes &v and `v` becomes
 * the Actor* the source passes as 0 -- and the first thing the body does is
 * read v->x. MEASURED, because that is how this was found rather than
 * predicted: FAULT c0000005 accessing 00000000, on the FIRST TTC_MOVING_BEAM
 * of level 27, in Stage::LoadClsnAndObjects -> LoadObjects ->
 * LoadStandardObjects -> Actor::Spawn -> m09_init ->
 * _ZN14TtcMovingCubeA13InitResourcesEv +0xf1 -> SetObjAndPos +0xe.
 *
 * hal/bob_enemy_bridges.cpp states the rule for its own three rows -- "shadow
 * definitions rather than aliases, because both sides are __thiscall" -- and
 * this row is the case that rule's converse covers: the two sides do NOT share
 * a calling convention, so only a real definition can adapt them.
 *
 * hal/reverse_bridges.cpp already carries a RaycastGround::SetObjAndPos, and
 * it cannot serve here: it is declared returning VOID (?...@@QAEXABU...), a
 * different decoration and so a different symbol, and C++ will not let one
 * struct declare both. A second shadow in a second TU is the same answer
 * bob_enemy_bridges.cpp reached for the same reason.
 *
 * The ROM's own SetObjAndPos returns nothing, which is why reverse_bridges
 * declares it void and why returning 0 here is not an invention -- the source
 * ignores the value. */
struct RaycastGroundIntShadow;   /* documentation only; the name below is what
                                    fixes the mangling and it must be the
                                    ROM's class name */
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(
    void *self, const void *v, void *a);
struct RaycastGround { int SetObjAndPos(const Vector3 &v, Actor *a); };
int RaycastGround::SetObjAndPos(const Vector3 &v, Actor *a)
{
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(this, &v, a);
    return 0;
}

// ---- THE ONE BODY THAT DOES NOT EXIST --------------------------------------
/* _ZN14TtcMovingCubeA8BehaviorEv, config addr 0x0211bd8c, size 0x178, is slot 6
 * of table 0x0211d568 -- id 118 TTC_MOVING_BEAM's Behavior. There is no src/ TU
 * for it, no delink block and no host copy: it appears NOWHERE in the tree. It
 * is therefore not a matched TU, which is why linkage.py never counts it and
 * why a census of ov065's unlinked rows does not show it.
 *
 * IT GETS A LOUD FACE, NOT A STUB, and the reason is wave 17's whole lesson: a
 * beam that returns 1 from a quiet stub is a platform that never moves and
 * never says so, which is the silent-failure shape this cluster came back out
 * over the first time. This is the hal/actor_classes_ov074.cpp shape verbatim
 * -- announce once by name, then decline through the receiver so the fault is
 * attributed to the beam and not to whichever actor's phase callback the walk
 * happened to be inside.
 *
 * THE DEBT IS IN THE DECOMP AND NO PORT LANE CAN CLOSE IT. It is filed as a
 * crack target and it is the sole reason port/tools/battery.py carries a
 * LEVEL_SKIPS row for level 27 naming TTC_MOVING_BEAM -- the GOOMBOSS row's
 * shape exactly, down to the bare re-probe that retires it automatically the
 * day 0x0211bd8c is matched. The class stays REGISTERED so that probe means
 * something. */
static int g_said_0211bd8c;
extern "C" int _ZN14TtcMovingCubeA8BehaviorEv(void *c)
{
    unsigned id = c ? *(unsigned short *)((char *)c + 0xc) : 0u;
    if (!g_said_0211bd8c) {
        g_said_0211bd8c = 1;
        std::fprintf(stderr,
                     "UNHOSTED: _ZN14TtcMovingCubeA8BehaviorEv (ov065 "
                     "0x0211bd8c, 0x178 bytes, slot 6 of table 0x0211d568) HAS "
                     "NO MATCHED BODY -- no src file, no delink block and no "
                     "host copy anywhere in the tree. Actor id %u %s reached "
                     "it. This is a DECOMP debt filed as a crack target; see "
                     "battery.py's LEVEL_SKIPS row for level 27.\n",
                     id, port_actor_class_name(id));
    }
    { static char _m[160];
      std::snprintf(_m, sizeof _m, "unhosted ov065 body "
                    "_ZN14TtcMovingCubeA8BehaviorEv on id %u %s", id,
                    port_actor_class_name(id));
      port_actor_slot_decline_for(c, _m); }
    return 0;
}

// ---- the seven fills -------------------------------------------------------
/* Every fill runs ttc_bringup, which runs the shared ov65_bringup and then the
   two halves only the Ttc subset needs. The pairs share one array and one
   idempotent fill, the ov035 ROTATING_COG shape. */
static void ttc_bringup(void)
{
    port_ov65_bringup();
    /* the four .text-pool CLPS blocks the Ttc InitResources bodies name at link
       time are ov035's storage; fill it from the ROM before any of them runs */
    port_ov035_pack_check();
    port_ov035_syms_patch();
    /* every Ttc destructor installs Platform's base table as its second vptr */
    hal_fill_platform_vtable();
    __sinit_ov065_0211c660();
    __sinit_ov065_0211c76c();
    __sinit_ov065_0211c7d8();
    __sinit_ov065_0211c890();
    __sinit_ov065_0211c8fc();
    __sinit_ov065_0211c9b8();
    __sinit_ov065_0211ca74();
}

/* ids 108 TTC_ROTATING_CUBE / 109 TTC_ROTATING_PRISM -- 0x0211d028 */
static int __fastcall ctrb_init(void *s, void *)
{ return ((TtcRotatingCube *)s)->TtcRotatingCube::InitResources(); }
static int __fastcall ctrb_clean(void *s, void *)
{ return ((TtcRotatingCube *)s)->TtcRotatingCube::CleanupResources(); }
static int __fastcall ctrb_behavior(void *s, void *)
{ return ((TtcRotatingCube *)s)->TtcRotatingCube::Behavior(); }
static int __fastcall ctrb_render(void *s, void *)
{ port_actor_render_probe("TTC_ROTATING_CUBE", (char *)s + 0xd4);
  return ((TtcRotatingCube *)s)->TtcRotatingCube::Render(); }
static int __fastcall ctrb_d1(void *s, void *)
{ return (int)(size_t)_ZN15TtcRotatingCubeD1Ev((int *)s); }
static int __fastcall ctrb_d0(void *s, void *)
{ return (int)(size_t)_ZN15TtcRotatingCubeD0Ev((int *)s); }
extern "C" void hal_fill_ct_rotate_block_vtable(void)
{
    ttc_bringup();
    void *volatile *vt = (void *volatile *)_ZTV20daObjCtRotateBlock_c;
    ov65_fill_shared(vt);
    vt[0]  = (void *)ctrb_init;
    vt[3]  = (void *)ctrb_clean;
    vt[6]  = (void *)ctrb_behavior;
    vt[9]  = (void *)ctrb_render;
    vt[16] = (void *)ctrb_d1;
    vt[17] = (void *)ctrb_d0;
    vt[31] = (void *)ov65_kill;
}

/* id 110 TTC_PENDULUM -- 0x0211d0ec, no class methods at all */
static int __fastcall m03_init(void *s, void *)
{ return func_ov065_0211a358((char *)s); }
static int __fastcall m03_clean(void *s, void *)
{ return func_ov065_0211a15c(s); }
static int __fastcall m03_behavior(void *s, void *)
{ return func_ov065_0211a1c8((char *)s); }
static int __fastcall m03_render(void *s, void *)
{ port_actor_render_probe("TTC_PENDULUM", (char *)s + 0xd4);
  return func_ov065_0211a1a0(s); }
static int __fastcall m03_d1(void *s, void *)
{ return (int)(size_t)func_ov065_02119f3c((int *)s); }
static int __fastcall m03_d0(void *s, void *)
{ return (int)(size_t)func_ov065_02119f88((int *)s); }
extern "C" void hal_fill_ct_mecha03_vtable(void)
{
    ttc_bringup();
    void *volatile *vt = (void *volatile *)data_ov065_0211d0ec;
    ov65_fill_shared(vt);
    vt[0]  = (void *)m03_init;
    vt[3]  = (void *)m03_clean;
    vt[6]  = (void *)m03_behavior;
    vt[9]  = (void *)m03_render;
    vt[16] = (void *)m03_d1;
    vt[17] = (void *)m03_d0;
    vt[31] = (void *)ov65_kill;
}

/* ids 111 TTC_CONVEYOR_BELT_LARGE / 112 ..SMALL -- 0x0211d1ec */
static int __fastcall m04_init(void *s, void *)
{ return ((TtcConveyorBeltLarge *)s)->TtcConveyorBeltLarge::InitResources(); }
static int __fastcall m04_clean(void *s, void *)
{ return ((TtcConveyorBeltLarge *)s)->TtcConveyorBeltLarge::CleanupResources(); }
static int __fastcall m04_behavior(void *s, void *)
{ return ((TtcConveyorBeltLarge *)s)->TtcConveyorBeltLarge::Behavior(); }
static int __fastcall m04_render(void *s, void *)
{ port_actor_render_probe("TTC_CONVEYOR_BELT", (char *)s + 0xd4);
  return ((TtcConveyorBeltLarge *)s)->TtcConveyorBeltLarge::Render(); }
static int __fastcall m04_d1(void *s, void *)
{ return (int)(size_t)_ZN20TtcConveyorBeltLargeD1Ev((int *)s); }
static int __fastcall m04_d0(void *s, void *)
{ return (int)(size_t)_ZN20TtcConveyorBeltLargeD0Ev((int *)s); }
extern "C" void hal_fill_ct_mecha04_vtable(void)
{
    ttc_bringup();
    void *volatile *vt = (void *volatile *)_ZTV16daObjCtMecha04_c;
    ov65_fill_shared(vt);
    vt[0]  = (void *)m04_init;
    vt[3]  = (void *)m04_clean;
    vt[6]  = (void *)m04_behavior;
    vt[9]  = (void *)m04_render;
    vt[16] = (void *)m04_d1;
    vt[17] = (void *)m04_d0;
    vt[31] = (void *)ov65_kill;
}

/* id 113 TTC_MOVING_BAR -- 0x0211d2b4, the other class with no method bodies */
static int __fastcall m05_init(void *s, void *)
{ return func_ov065_0211b1d4((char *)s); }
static int __fastcall m05_clean(void *s, void *)
{ return func_ov065_0211ad04(s); }
static int __fastcall m05_behavior(void *s, void *)
{ return func_ov065_0211ae08((char *)s); }
static int __fastcall m05_render(void *s, void *)
{ port_actor_render_probe("TTC_MOVING_BAR", (char *)s + 0xd4);
  return func_ov065_0211ad48(s); }
static int __fastcall m05_d1(void *s, void *)
{ return (int)(size_t)func_ov065_0211ab60((int *)s); }
static int __fastcall m05_d0(void *s, void *)
{ return (int)(size_t)func_ov065_0211abac((int *)s); }
extern "C" void hal_fill_ct_mecha05_vtable(void)
{
    ttc_bringup();
    void *volatile *vt = (void *volatile *)data_ov065_0211d2b4;
    ov65_fill_shared(vt);
    vt[0]  = (void *)m05_init;
    vt[3]  = (void *)m05_clean;
    vt[6]  = (void *)m05_behavior;
    vt[9]  = (void *)m05_render;
    vt[16] = (void *)m05_d1;
    vt[17] = (void *)m05_d0;
    vt[31] = (void *)ov65_kill;
}

/* ids 114 TTC_ROTATING_GEAR / 115 TTC_ROTATING_TRIANGLE -- 0x0211d3b4.
   dsd spells these bodies _ZN13TTC_MovingBar*, which is id 113's NAME. */
static int __fastcall kai_init(void *s, void *)
{ return ((TTC_MovingBar *)s)->TTC_MovingBar::InitResources(); }
static int __fastcall kai_clean(void *s, void *)
{ return ((TTC_MovingBar *)s)->TTC_MovingBar::CleanupResources(); }
static int __fastcall kai_behavior(void *s, void *)
{ return ((TTC_MovingBar *)s)->TTC_MovingBar::Behavior(); }
static int __fastcall kai_render(void *s, void *)
{ port_actor_render_probe("TTC_ROTATING_GEAR", (char *)s + 0xd4);
  return ((TTC_MovingBar *)s)->TTC_MovingBar::Render(); }
static int __fastcall kai_d1(void *s, void *)
{ return (int)(size_t)_ZN13TTC_MovingBarD1Ev((int *)s); }
static int __fastcall kai_d0(void *s, void *)
{ return (int)(size_t)_ZN13TTC_MovingBarD0Ev((int *)s); }
extern "C" void hal_fill_ct_kaitendai_vtable(void)
{
    ttc_bringup();
    void *volatile *vt = (void *volatile *)_ZTV18daObjCtKaitendai_c;
    ov65_fill_shared(vt);
    vt[0]  = (void *)kai_init;
    vt[3]  = (void *)kai_clean;
    vt[6]  = (void *)kai_behavior;
    vt[9]  = (void *)kai_render;
    vt[16] = (void *)kai_d1;
    vt[17] = (void *)kai_d0;
    vt[31] = (void *)ov65_kill;
}

/* ids 116 TTC_MOVING_CUBE_A / 117 ..B -- 0x0211d494.
   dsd spells these bodies _ZN15TtcRotatingGear*, which is id 114's name. Slot 6
   is a .c body and an extern "C" free function, so it is called by name. */
static int __fastcall m08_init(void *s, void *)
{ return ((TtcRotatingGear *)s)->TtcRotatingGear::InitResources(); }
static int __fastcall m08_clean(void *s, void *)
{ return ((TtcRotatingGear *)s)->TtcRotatingGear::CleanupResources(); }
static int __fastcall m08_behavior(void *s, void *)
{ return _ZN15TtcRotatingGear8BehaviorEv((char *)s); }
static int __fastcall m08_render(void *s, void *)
{ port_actor_render_probe("TTC_MOVING_CUBE", (char *)s + 0xd4);
  return ((TtcRotatingGear *)s)->TtcRotatingGear::Render(); }
static int __fastcall m08_d1(void *s, void *)
{ return (int)(size_t)_ZN15TtcRotatingGearD1Ev((int *)s); }
static int __fastcall m08_d0(void *s, void *)
{ return (int)(size_t)_ZN15TtcRotatingGearD0Ev((int *)s); }
extern "C" void hal_fill_ct_mecha08_vtable(void)
{
    ttc_bringup();
    void *volatile *vt = (void *volatile *)_ZTV16daObjCtMecha08_c;
    ov65_fill_shared(vt);
    vt[0]  = (void *)m08_init;
    vt[3]  = (void *)m08_clean;
    vt[6]  = (void *)m08_behavior;
    vt[9]  = (void *)m08_render;
    vt[16] = (void *)m08_d1;
    vt[17] = (void *)m08_d0;
    vt[31] = (void *)ov65_kill;
}

/* id 118 TTC_MOVING_BEAM -- 0x0211d568. dsd spells these bodies
   _ZN14TtcMovingCubeA*, which is id 116's name. Slot 0 is an extern "C" free
   function; slot 6 is THE MISSING BODY and takes the loud face above. */
static int __fastcall m09_init(void *s, void *)
{ return _ZN14TtcMovingCubeA13InitResourcesEv((char *)s); }
static int __fastcall m09_clean(void *s, void *)
{ return ((TtcMovingCubeA *)s)->TtcMovingCubeA::CleanupResources(); }
static int __fastcall m09_behavior(void *s, void *)
{ return _ZN14TtcMovingCubeA8BehaviorEv(s); }
static int __fastcall m09_render(void *s, void *)
{ port_actor_render_probe("TTC_MOVING_BEAM", (char *)s + 0xd4);
  return ((TtcMovingCubeA *)s)->TtcMovingCubeA::Render(); }
static int __fastcall m09_d1(void *s, void *)
{ return (int)(size_t)_ZN14TtcMovingCubeAD1Ev((int *)s); }
static int __fastcall m09_d0(void *s, void *)
{ return (int)(size_t)_ZN14TtcMovingCubeAD0Ev((int *)s); }
extern "C" void hal_fill_ct_mecha09_vtable(void)
{
    ttc_bringup();
    void *volatile *vt = (void *volatile *)_ZTV16daObjCtMecha09_c;
    ov65_fill_shared(vt);
    vt[0]  = (void *)m09_init;
    vt[3]  = (void *)m09_clean;
    vt[6]  = (void *)m09_behavior;
    vt[9]  = (void *)m09_render;
    vt[16] = (void *)m09_d1;
    vt[17] = (void *)m09_d0;
    vt[31] = (void *)ov65_kill;
}
