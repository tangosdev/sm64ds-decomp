// GATE 206 (run link60, lane A2): DIAMOND_LIFT (135, ov043), the first class
// seated out of Bowser in the Dark World's own overlay. daObjKm1_Ukishima_c,
// six instances on level 35.
//
// ov043 is already whole-mounted in PORT_LEVEL_OVERLAYS; this adds the SECOND,
// per-symbol mount of the same overlay, the ov012/ov013/ov021/ov044/ov045/ov046
// dual-mount shape. The full derivation -- the cast map, the four widths, the
// naming shift and the two classes this slice deliberately leaves skipped -- is
// port/ov043_syms.txt, and this gate's own reading of it is
// port/slice_gate206.txt.
//
// Same law as hal/actor_classes_ov045.cpp beside it: ROM slot order, __fastcall
// thunks that call the class's own C body, unhosted slots trap by name. The
// table is the 32-slot Platform shape and every vtspan route agrees on 32.
//
// SLOT 9 IS THE WAVE-19 SLOT-5 SHADOW AND IT IS SAFE, adjudicated from how the
// class constructs the member it dispatches rather than from any header name:
// InitResources feeds Model::LoadFile into ModelBase::SetFile at +0xd4 and both
// destructors call _ZN5ModelD1Ev on +0xd4, so it is a plain Model and
// hal/cxxname_bridge.cpp's dual-filled _ZTV5Model[5] is Model::Render. The
// matched TU is linked.
//
// THE VTABLE NAME IS THE ROM'S OWN, taken from the table's own RTTI record and
// not from a dsd label: 0x021122b8's vtable[-1] is 0x02112270 and that record's
// word[1] names "19daObjKm1_Ukishima_c". dsd calls the table
// data_ov043_021122b8, which is what the matched factory spells, so that
// spelling is an /alternatename onto this one array. 0x021122b8 is excluded
// from the mount (the ov015/ov016/ov022/ov045/ov080 rule) -- mounting it would
// hand the factory DS code addresses.
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
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "dsstate_seg.h"

extern "C" {
/* the shared lifecycle halves, the same functions every sibling fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p); /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */

/* the mount's generated bring-up halves */
void port_ov043_pack_check(void);
void port_ov043_syms_patch(void);
void __sinit_ov043_021117fc(void);   /* id 135's own two SharedFilePtrs */
/* lane SEAT-BDW: the file constructors of the two SEATED classes (136, 134),
   run in the same bring-up (declared here so port_ov43_bringup can call them).
   id 137's sinit __sinit_ov043_02111868 stays OFF -- 137 is not seated (its
   InitResources is an inferred wall that hard-crashes level 35 at spawn; the
   ov081 rule, a sinit for an unhosted class stays off). */
void __sinit_ov043_021118d4(void);   /* id 136 */
void __sinit_ov043_02111940(void);   /* id 134 */

/* id 135's own bodies, all matched src on slice_gate206.txt */
int func_ov043_02111320(void *self);     /* slot 0,  InitResources */
int func_ov043_0211123c(void *self);     /* slot 3,  CleanupResources */
int func_ov043_021112a8(void *self);     /* slot 6,  Behavior */
int func_ov043_02111280(void *self);     /* slot 9,  Render */
int *func_ov043_021111a0(int *self);     /* slot 16, D1 */
int *func_ov043_021111e4(int *self);     /* slot 17, D0 */
void *DiamondLift_Spawn(void);

/* The array the ROM factory installs. Thirty-two slots, defined here and not
   just declared: the `int` type and C linkage match the
   `extern int _ZTV19daObjKm1_Ukishima_c[]` in include/decl_common.h:680 that
   the matched D1 reads. Inside DSSTATE because the factory's spelling of it,
   data_ov043_021122b8, is a hosted DS name and dsstate_guard requires every
   one of those to be inside the captured segment -- the ov045 treatment of
   _ZTV15FireSeaElevator. */
DSSTATE_BEGIN
int _ZTV19daObjKm1_Ukishima_c[32];
DSSTATE_END
}

/* The matched factory and the D0's VT0 spell the same address the way dsd
   named it. Both references are C linkage, so point that spelling at the one
   host array -- the daChoropu_c / ami_bou precedent. The LHS is undefined
   everywhere (0x021122b8 is excluded from the mount by name), so the alias
   cannot be defeated. */
#pragma comment(linker, "/alternatename:_data_ov043_021122b8=__ZTV19daObjKm1_Ukishima_c")

/* The G0/G1 landing pads. src/func_ov043_0211123c.c releases ov043's own two
   bss SharedFilePtrs through the shared role-name placeholders G0 and G1, which
   hal/cxx_aliases.cpp has already bound to OTHER objects -- _G0 to the game
   heap pointer, _G1 to SignPost's KCL -- so the body would have released the
   game heap pointer without failing at link. port/CMakeLists.txt renames that
   one file's G0/G1 to the two private names below and they bind here, the
   PATH_LIFT remedy. The addresses come from the body's own literal pool:
   0x02111278 -> 0x021125e8 and 0x0211127c -> 0x021125e0, both
   module:overlay(43). The destructor's G0 is a different object -- the game
   heap at 0x020a0eac -- and is deliberately left on the cxx_aliases binding. */
extern "C" {
int port_ov043_ukishima_file0[];   /* -> data_ov043_021125e8, the Model file */
int port_ov043_ukishima_file1[];   /* -> data_ov043_021125e0, the collision file */
}
#pragma comment(linker, "/alternatename:_port_ov043_ukishima_file0=_data_ov043_021125e8")
#pragma comment(linker, "/alternatename:_port_ov043_ukishima_file1=_data_ov043_021125e0")

// ---- the trap --------------------------------------------------------------
static void o43_trap_report(void *self, int slot)
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
#define O43_TRAP(n) \
    static int __fastcall o43_trap##n(void *s, void *) \
    { o43_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38, the pair every sibling fill traps. 30 is
   Actor::OnAimedAtWithEggReturnVec, the SRET body no fill's thunk models. */
O43_TRAP(13) O43_TRAP(14)
#undef O43_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall o43_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall o43_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall o43_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall o43_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall o43_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall o43_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall o43_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall o43_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall o43_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall o43_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall o43_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall o43_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall o43_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall o43_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall o43_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall o43_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall o43_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall o43_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall o43_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall o43_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall o43_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall o43_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall o43_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

// ---- the mount bring-up ----------------------------------------------------
//
// LANE OWNERSHIP, the ov045 note applied again. The right home for this is
// port_actor_overlays_sinits() in hal/actor_overlays.cpp, beside the ov013
// block. No lane owns that file in this wave, so the bring-up rides the
// registry fill behind a done-guard the way ov045's does. The ordering ov045
// measured holds here unchanged: port_actor_overlays_sinits() completes before
// port_actor_registry_install(), the generated port_ov043_syms_patch() writes
// SpawnInfo + 32 and nothing else, and the SharedFilePtrs the sinit builds are
// read by InitResources at spawn, after registration.
// THE HANDOFF: whoever next owns hal/actor_overlays.cpp should move this body
// beside the ov013 block and cut this to a call.
//
// ONE SINIT, NOT FOUR. 021117fc is id 135's own -- Model file 1625 into
// data_ov043_021125e8, collision file 1626 into data_ov043_021125e0, the exact
// two cells this class's CleanupResources releases. The other three build other
// classes' pairs and stay OFF (the ov081 rule), and 02111940 could not link if
// it were wanted: it spells twenty-odd cells with ov047 and ov052 names, none
// of which resolve in port/.
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov43_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov43_bringup_done;
DSSTATE_END

extern "C" void port_ov43_bringup(void)
{
    if (g_ov43_bringup_done)
        return;
    g_ov43_bringup_done = 1;
    port_ov043_pack_check();
    port_ov043_syms_patch();
    /* the Platform base table the D1/D0 install between member teardowns */
    hal_fill_platform_vtable();
    __sinit_ov043_021117fc();
    /* lane SEAT-BDW: the file constructors of the two SEATED classes. 136's
       (021118d4) and 134's (02111940, the five-stair array) feed the
       InitResources those classes run. DIAMOND_LIFT's "one sinit, not four"
       note partly discharged: 136 and 134 are hosted so their sinits come on;
       137's (02111868) stays off because 137 is not seatable (its InitResources
       is an inferred wall). */
    __sinit_ov043_021118d4();
    __sinit_ov043_02111940();
}

// ---- DIAMOND_LIFT (id 135) -- table 0x021122b8 -----------------------------
static int __fastcall dl_init(void *s, void *)
{ return func_ov043_02111320(s); }
static int __fastcall dl_clean(void *s, void *)
{ return func_ov043_0211123c(s); }
static int __fastcall dl_behavior(void *s, void *)
{ return func_ov043_021112a8(s); }
static int __fastcall dl_render(void *s, void *)
{ port_actor_render_probe("DIAMOND_LIFT", (char *)s + 0xd4);
  return func_ov043_02111280(s); }
static int __fastcall dl_d1(void *s, void *)
{ return (int)(size_t)func_ov043_021111a0((int *)s); }
static int __fastcall dl_d0(void *s, void *)
{ return (int)(size_t)func_ov043_021111e4((int *)s); }

extern "C" void hal_fill_diamond_lift_vtable(void)
{
    port_ov43_bringup();
    void **vt = (void **)_ZTV19daObjKm1_Ukishima_c;
    vt[0]  = (void *)dl_init;
    vt[1]  = (void *)o43_binit;
    vt[2]  = (void *)o43_ainit;
    vt[3]  = (void *)dl_clean;
    vt[4]  = (void *)o43_bclean;
    vt[5]  = (void *)o43_aclean;
    vt[6]  = (void *)dl_behavior;
    vt[7]  = (void *)o43_bbeh;
    vt[8]  = (void *)o43_abeh;
    vt[9]  = (void *)dl_render;
    vt[10] = (void *)o43_bren;
    vt[11] = (void *)o43_aren;
    vt[12] = (void *)o43_pdes;
    vt[13] = (void *)o43_trap13;
    vt[14] = (void *)o43_trap14;
    vt[15] = (void *)o43_heap;
    vt[16] = (void *)dl_d1;
    vt[17] = (void *)dl_d0;
    vt[18] = (void *)o43_yoshi;
    vt[19] = (void *)o43_turn_egg;
    vt[20] = (void *)o43_v50;
    vt[21] = (void *)o43_pounded;
    vt[22] = (void *)o43_atk1;
    vt[23] = (void *)o43_atk2;
    vt[24] = (void *)o43_kicked;
    vt[25] = (void *)o43_pushed;
    vt[26] = (void *)o43_cannon;
    vt[27] = (void *)o43_mega;
    vt[28] = (void *)o43_under;
    vt[29] = (void *)o43_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)o43_kill;
}

// ============================================================================
// lane SEAT-BDW: the rest of ov043's cast -- RICKSHAW_BDW (137),
// RICKSHAW_PLATFORM_BDW (136), STAIRS_BDW (134). Bowser in the Dark World,
// level 35. Seated on top of DIAMOND_LIFT (135) above, the ov047 152/151/153
// shape applied to the Bowser-in-the-Dark-World siblings.
//
// THE ID<->BODY MAP IS THE NAMING SHIFT (port/ov043_syms.txt): each id is
// pinned by its OWN Spawn's final vtable store, never by a name. dsd's
// _ZTV11RickshawBdw / _ZN11RickshawBdw* are id 136's; _ZTV19RickshawPlatformBdw
// / _ZN19RickshawPlatformBdw* are id 134's; and RickshawBdw_Spawn is id 137's.
//
// ONLY 136 AND 134 ARE SEATED. id 137 (RICKSHAW_BDW) is NOT: its four own
// bodies are GUESS func_ov043 TUs (inferred stubs), so its InitResources has no
// matched body. 137 is placed on level 35 and spawns at boot, and a half-built
// 137 (InitResources declined) NULL-derefs during its own spawn continuation --
// not a survivable quarantine, a hard c0000005. Registering it faults BitDW, so
// it stays skipped. SHIP-GATE: 137 needs its InitResources matched before it can
// be seated. See lane SEAT-BDW's report and port/slice_bdw.txt.
//
// SLOTS 6/9 are inherited ov002 bodies already in the link (ov036 + ov047
// slices); declared and faced here, never re-enrolled. SLOTS 0/3/16/17 are each
// seated class's own matched bodies.
extern "C" {
/* the two seated factories are referenced only by the registry rows in
   hal/actor_classes.inc (declared there as void*(void) to match the registry
   field); nothing in this file calls them. */

/* id 136's four matched own bodies (slots 0/3/16/17) */
int _ZN11RickshawBdw13InitResourcesEv(void *self);      /* slot 0  */
int _ZN11RickshawBdw16CleanupResourcesEv(void *self);   /* slot 3  */
int *_ZN11RickshawBdwD1Ev(int *self);                   /* slot 16 */
int *_ZN11RickshawBdwD0Ev(int *self);                   /* slot 17 */

/* id 134's four matched own bodies (slots 0/3/16/17) */
int _ZN19RickshawPlatformBdw13InitResourcesEv(void *self);    /* slot 0  */
int _ZN19RickshawPlatformBdw16CleanupResourcesEv(void *self); /* slot 3  */
int *_ZN19RickshawPlatformBdwD1Ev(void *self);                /* slot 16 */
int _ZN19RickshawPlatformBdwD0Ev(void *self);                 /* slot 17 */

/* the four inherited slot 6/9 bodies (for 136 and 134), already linked by the
   ov036 and ov047 slices -- declared and faced, NEVER enrolled here (a second
   definition would be a duplicate symbol). */
int func_ov002_020b6920(void *self);   /* 136 Behavior, ov047 slice */
int func_ov002_020b68f8(void *self);   /* 136 Render,   ov047 slice */
int func_ov002_020b4bfc(void *self);   /* 134 Behavior, ov036 slice */
int func_ov002_020b4bc4(void *self);   /* 134 Render,   ov036 slice */

/* the two seated classes' file-constructor sinits (135's runs already; 137's
   __sinit_ov043_02111868 stays off, 137 is not seated). */
void __sinit_ov043_021118d4(void);   /* id 136's Model 1619 + clsn 1620 */
void __sinit_ov043_02111940(void);   /* id 134's five-stair array (files 1609-1618) */

/* the two host vtables for the seated classes, both excluded from the mount
   (the four-spans rule in port/CMakeLists.txt). The names are what each class's
   OWN bodies spell after the per-source -D binds resolve -- dsd's for the
   ADDRESS, not the class. 137's table 0x0211238c is deliberately NOT hosted:
   nothing links to it because 137 is not registered. */
DSSTATE_BEGIN
int _ZTV11RickshawBdw[32];          /* 0x0211245c, id 136 RICKSHAW_PLATFORM_BDW */
int _ZTV19RickshawPlatformBdw[32];  /* 0x0211255c, id 134 STAIRS_BDW */
DSSTATE_END
}

/* id 134's InitResources/CleanupResources are real C++ methods (spelled
   RickshawPlatformBdw::), so the Itanium _ZN19RickshawPlatformBdw* symbols the
   fill wants do not exist -- faced here, the ov047 STAIRS_BS recipe. */
#include "RickshawPlatformBdw.h"
extern "C" {
int _ZN19RickshawPlatformBdw13InitResourcesEv(void *self)
{ return ((RickshawPlatformBdw *)self)->RickshawPlatformBdw::InitResources(); }
int _ZN19RickshawPlatformBdw16CleanupResourcesEv(void *self)
{ return ((RickshawPlatformBdw *)self)->RickshawPlatformBdw::CleanupResources(); }
}

/* id 134's two //cpp method TUs declare `extern struct Arg data_ov043_02112518;`
   at file scope OUTSIDE decl_common.h's extern "C" block, so MSVC mangles it
   with the local struct type. The mount emits one C-named array; bind the
   mangled spelling onto it -- the ov047 line, read off the linker's own LNK2019
   hint. The LHS is defined nowhere, so alternatename_guard stays clean. */
#pragma comment(linker, "/alternatename:?data_ov043_02112518@@3UArg@@A=_data_ov043_02112518")

/* the shared half of a Platform table, exactly DIAMOND_LIFT's 25 non-own slots.
   Each fill sets its own 0/3/6/9/16/17/31 afterward. */
static void o43_fill_shared(void **vt)
{
    vt[1]  = (void *)o43_binit;
    vt[2]  = (void *)o43_ainit;
    vt[4]  = (void *)o43_bclean;
    vt[5]  = (void *)o43_aclean;
    vt[7]  = (void *)o43_bbeh;
    vt[8]  = (void *)o43_abeh;
    vt[10] = (void *)o43_bren;
    vt[11] = (void *)o43_aren;
    vt[12] = (void *)o43_pdes;
    vt[13] = (void *)o43_trap13;
    vt[14] = (void *)o43_trap14;
    vt[15] = (void *)o43_heap;
    vt[18] = (void *)o43_yoshi;
    vt[19] = (void *)o43_turn_egg;
    vt[20] = (void *)o43_v50;
    vt[21] = (void *)o43_pounded;
    vt[22] = (void *)o43_atk1;
    vt[23] = (void *)o43_atk2;
    vt[24] = (void *)o43_kicked;
    vt[25] = (void *)o43_pushed;
    vt[26] = (void *)o43_cannon;
    vt[27] = (void *)o43_mega;
    vt[28] = (void *)o43_under;
    vt[29] = (void *)o43_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// id 137 RICKSHAW_BDW is intentionally NOT filled or registered -- see the file
// header. Its four own bodies are inferred and a live level-35 spawn crashes.

// ---- RICKSHAW_PLATFORM_BDW (id 136) -- table 0x0211245c, matched -----------
static int __fastcall rpbdw_init(void *s, void *)
{ return _ZN11RickshawBdw13InitResourcesEv(s); }
static int __fastcall rpbdw_clean(void *s, void *)
{ return _ZN11RickshawBdw16CleanupResourcesEv(s); }
static int __fastcall rpbdw_behavior(void *s, void *)
{ return func_ov002_020b6920(s); }
static int __fastcall rpbdw_render(void *s, void *)
{ port_actor_render_probe("RICKSHAW_PLATFORM_BDW", (char *)s + 0xd4);
  return func_ov002_020b68f8(s); }
static int __fastcall rpbdw_d1(void *s, void *)
{ return (int)(size_t)_ZN11RickshawBdwD1Ev((int *)s); }
static int __fastcall rpbdw_d0(void *s, void *)
{ return (int)(size_t)_ZN11RickshawBdwD0Ev((int *)s); }

extern "C" void hal_fill_rickshaw_platform_bdw_vtable(void)
{
    port_ov43_bringup();
    void **vt = (void **)_ZTV11RickshawBdw;
    o43_fill_shared(vt);
    vt[0]  = (void *)rpbdw_init;
    vt[3]  = (void *)rpbdw_clean;
    vt[6]  = (void *)rpbdw_behavior;
    vt[9]  = (void *)rpbdw_render;
    vt[16] = (void *)rpbdw_d1;
    vt[17] = (void *)rpbdw_d0;
    vt[31] = (void *)o43_kill;
}

// ---- STAIRS_BDW (id 134) -- table 0x0211255c, matched ----------------------
static int __fastcall sbdw_init(void *s, void *)
{ return _ZN19RickshawPlatformBdw13InitResourcesEv(s); }
static int __fastcall sbdw_clean(void *s, void *)
{ return _ZN19RickshawPlatformBdw16CleanupResourcesEv(s); }
static int __fastcall sbdw_behavior(void *s, void *)
{ return func_ov002_020b4bfc(s); }
static int __fastcall sbdw_render(void *s, void *)
{ port_actor_render_probe("STAIRS_BDW", (char *)s + 0xd4);
  return func_ov002_020b4bc4(s); }
static int __fastcall sbdw_d1(void *s, void *)
{ return (int)(size_t)_ZN19RickshawPlatformBdwD1Ev(s); }
static int __fastcall sbdw_d0(void *s, void *)
{ return (int)(size_t)_ZN19RickshawPlatformBdwD0Ev(s); }

extern "C" void hal_fill_stairs_bdw_vtable(void)
{
    port_ov43_bringup();
    void **vt = (void **)_ZTV19RickshawPlatformBdw;
    o43_fill_shared(vt);
    vt[0]  = (void *)sbdw_init;
    vt[3]  = (void *)sbdw_clean;
    vt[6]  = (void *)sbdw_behavior;
    vt[9]  = (void *)sbdw_render;
    vt[16] = (void *)sbdw_d1;
    vt[17] = (void *)sbdw_d0;
    vt[31] = (void *)o43_kill;
}
