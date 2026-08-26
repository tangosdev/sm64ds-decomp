// RUN LINKW WAVE 12 (lane w12): WARIO'S KEY ARENA -- ov073's WHOLE CAST.
//
// Level 49 (ex_wario, "Wario's key arena", course 20) is mounted and boots, and
// until this file it placed FOUR actors and skipped EIGHTEEN: the entire cast of
// the level is ov073's and ov073 had no mount at all. hal/level_boot.cpp's own
// row for 49 says so out loud -- "[unmounted ov073]".
//
//   id   class          instances on L49   table                     width
//   170  CccArena       x1                 0x021231e8 _ZTV8CccArena    32
//   171  CccBigIce      x8                 0x021231e8 (the same one)   32
//   172  CccSmallIce    x8                 0x021231e8 (the same one)   32
//   218  ChiefChilly    x1                 0x02123090 _ZTV11ChiefChilly 31
//
// Seventeen of the eighteen sit on ONE table. All three Ccc Spawns are the same
// 0x30-byte body -- ActorBase::operator new(828), Platform::C2,
// `p[0] = _ZTV8CccArena` -- and each one's own vtable-store reloc names that
// same table (0x02122870 / 0x02122840 / 0x02122810). It is one C++ class under
// three ids, and CccArena::InitResources switches on the id it reads back out
// of the object at +0xc (0xaa / 0xab / 0xac) to pick row 0/1/2 of the
// three-column file table at 0x021231bc. So the seventeen cost ONE fill and
// three registry rows, and 171/172 take the `0` fill column: row 170 is
// installed on every level boot (port_actor_registry_install walks the whole
// table), so the one fill always runs.
//
// The full cast map with both attribution routes, the RTTI identities and the
// two width derivations are in port/ov073_syms.txt. The short version of the
// width block, because it is the one measurement that could have taken a level:
// ChiefChilly's table is THIRTY-ONE and its reloc run reads THIRTY-TWO. The
// extra word, 0x0212310c, is the first record of CccArena's OWN state table --
// __sinit_ov073_02122d48 copies it into 0x021234c0, and the function it holds
// (func_ov073_02122200) lives inside CccArena's code block, not ChiefChilly's.
// The next-symbol landing wins, exactly the way the ov052 id-174 warning says
// it does.
//
// ---- THE VTABLES ARE HOST STORAGE ------------------------------------------
//
// Both are excluded from the ov073 per-symbol mount (port/ov073_syms.txt) and
// declared here as host arrays -- the ov015/ov016/ov045/ov060 rule: a mounted
// vtable hands a factory DS code addresses. CccArena's dsd span carries one
// interior split (data_ov073_02123254, its slot 27), which is excluded with it.
// The one RTTI name a destructor restores by, _ZTV13daObjEwbIce_c, is aliased
// onto the host array below; ChiefChilly's D1/D0 spell their own table by its
// _ZTV name and need nothing.
//
// ---- FOUR PLACES THE RECOVERED SOURCE CANNOT BE CALLED AS WRITTEN ----------
//
// Three of them are the pointer-to-member and ModelAnim-slot-5 landings and
// they live in port/unmatched/Ov073_State.cpp with their own derivations:
// ChiefChilly_ChangeState, func_ov073_021223a4 (CccArena's ChangeState),
// _ZN11ChiefChilly8BehaviorEv and _ZN11ChiefChilly6RenderEv.
//
// The fourth is here. ChiefChilly_Spawn is an ARM r0 RIDE-THROUGH: its matched
// src calls `func_020aed98();` with NO ARGUMENT, because in the ROM the object
// the allocator just returned is still in r0 when the Enemy constructor is
// entered --
//
//     02121ed4  bl  0x02043444   ActorBase::operator new(0x504)
//     02121ed8  movs r4, r0                 <- r0 UNTOUCHED
//     02121edc  beq  0x02121f68
//     02121ee0  bl  0x020aed98   Enemy::C2  <- called with r0 = the object
//
// -- and mwcc reproduced that, so the TU is byte-faithful and unusable under
// cdecl, where nothing pushes the receiver at all. It is the _Znwj shape the
// port has ruled on since gate 24. port_factory_chief_chilly below is the ROM's
// own sequence with the receiver spelled: every call target, every member
// offset (0x110 / 0x150 / 0x30c / 0x380 / 0x3e8 / 0x448 / 0x4d4) and every
// literal (0x504 the size, 8/8/2 the array counts, 0xc the stride,
// func_0203d384 the element constructor in r3, func_020072c0 the destructor on
// the stack) is read from the disassembly of 0x02121ec8 and its literal pool.
// src/ChiefChilly_Spawn.cpp stays OUT of this lane's slice.
// The three Ccc Spawns do NOT have this problem -- their
// `_ZN8PlatformC2Ev(p)` names the receiver -- so they are registered directly.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE --------------------------------
//
// The ov045/ov060/ov031 lane-ownership pattern: no lane owns
// hal/actor_overlays.cpp in this wave, so the bring-up rides the first registry
// fill (ov73_bringup, one done-guard, both fills call it). Its ORDER is the
// load-bearing part and it is the ov060/Crate ordering, not a preference:
//
//     port_ov073_pack_check()      the emitted --pack adjacency assert
//     port_ov073_syms_patch()      the mount's own pointer pass
//     port_ov073_states_seat()     the FORTY {code, adj} source records
//     __sinit_ov073_02122874()     copies 32 of them into 16 bss tables
//     __sinit_ov073_02122d48()     copies the other 8 into 4 bss tables
//
// The seat must precede both sinits because the sinits COPY the records: a seat
// that ran after them would fix the sources and leave every live table holding
// DS code addresses. Both sinits LINK (unlike ov045's 0x02112280, neither name
// is contested at a shared window), so nothing here is hand-hosted.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half -- slots 1/2/4/5/7/8/10/11/13/14/15/18..30 of BOTH
   tables are the SAME arm9 words, and they are the same words ov045's six
   platform tables carry (checked slot for slot against _ZTV15FireSeaElevator
   before this fill was written), so the shared half below is that file's. */
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

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* the generated ov073 per-symbol mount (build/port/host-src/ov073_syms.c) */
void port_ov073_pack_check(void);
void port_ov073_syms_patch(void);
/* the forty state records' seat (port/unmatched/Ov073_State.cpp) */
void port_ov073_states_seat(void);
/* both ov073 sinits, both linkable */
void __sinit_ov073_02122874(void);
void __sinit_ov073_02122d48(void);

/* what port_factory_chief_chilly spells by hand */
void *_ZN9ActorBasenwEj(unsigned size);
void _ZN5EnemyC2Ev(void *self);                  /* ov002 0x020aed98 */
void _ZN25MovingCylinderClsnWithPosC1Ev(void *self);
void _ZN12WithMeshClsnC1Ev(void *self);
void _ZN14BlendModelAnimC1Ev(void *self);
void _ZN11ShadowModelC1Ev(void *self);
void func_020733a8(void *base, int count, int stride, void *ctor, void *dtor);
void func_020072c0(void);
void func_0203d384(void);
}

/* ---- C++-MANGLED DATA SPELLINGS, the data_02082128 / data_020a0e68 precedent
   (hal/actor_classes_ov071.cpp, hal/actor_classes_ov100pl.cpp,
   hal/actor_classes_ov065.cpp all carry the same block).

   Six of ov073's mounted globals are spelled from .cpp TUs at file scope rather
   than inside an `extern "C"` block, so those TUs emit MSVC-decorated data
   names. The mount emits ONE C-named array per symbol (`u8 data_ov073_XXXX[N]`
   in a generated .c), and these aliases bind every decorated spelling onto that
   one storage. Where the same address is spelled with two or three different
   types -- 0x02123360 is `char`, `int *` AND `struct PMF` across three TUs --
   all of them land on the same array, which is the point: the ROM has one
   object there and the type disagreement is a decompiler artefact, not a
   layout one. The list is exactly the link's unresolved set, not a guess.

   Every LHS below is DECLARED and never DEFINED anywhere in the port, so
   tools/alternatename_guard.py's "a defined LHS defeats the alias silently"
   check stays clean; it runs post-link in build-port.cmd and covers these. */
/* the eight ChiefChilly SharedFilePtrs, spelled `struct SharedFilePtr` by
   _ZN11ChiefChilly13InitResourcesEv.cpp */
#pragma comment(linker, "/alternatename:?data_ov073_02123280@@3USharedFilePtr@@A=_data_ov073_02123280")
#pragma comment(linker, "/alternatename:?data_ov073_02123288@@3USharedFilePtr@@A=_data_ov073_02123288")
#pragma comment(linker, "/alternatename:?data_ov073_02123290@@3USharedFilePtr@@A=_data_ov073_02123290")
#pragma comment(linker, "/alternatename:?data_ov073_02123298@@3USharedFilePtr@@A=_data_ov073_02123298")
#pragma comment(linker, "/alternatename:?data_ov073_021232a0@@3USharedFilePtr@@A=_data_ov073_021232a0")
#pragma comment(linker, "/alternatename:?data_ov073_021232a8@@3USharedFilePtr@@A=_data_ov073_021232a8")
#pragma comment(linker, "/alternatename:?data_ov073_021232b0@@3USharedFilePtr@@A=_data_ov073_021232b0")
#pragma comment(linker, "/alternatename:?data_ov073_021232b8@@3USharedFilePtr@@A=_data_ov073_021232b8")
/* the same 0x021232a0 again, as `void **`, from func_ov073_0211f61c */
#pragma comment(linker, "/alternatename:?data_ov073_021232a0@@3PAPAXA=_data_ov073_021232a0")
/* four of the sixteen bss state tables, spelled `struct PMF` / `char` /
   `int *` by the state bodies and by ChiefChilly::InitResources */
#pragma comment(linker, "/alternatename:?data_ov073_02123330@@3UPMF@@A=_data_ov073_02123330")
#pragma comment(linker, "/alternatename:?data_ov073_02123360@@3UPMF@@A=_data_ov073_02123360")
#pragma comment(linker, "/alternatename:?data_ov073_021233c0@@3UPMF@@A=_data_ov073_021233c0")
#pragma comment(linker, "/alternatename:?data_ov073_021233d0@@3UPMF@@A=_data_ov073_021233d0")
#pragma comment(linker, "/alternatename:?data_ov073_02123360@@3DA=_data_ov073_02123360")
#pragma comment(linker, "/alternatename:?data_ov073_021233a0@@3DA=_data_ov073_021233a0")
#pragma comment(linker, "/alternatename:?data_ov073_021233d0@@3DA=_data_ov073_021233d0")
#pragma comment(linker, "/alternatename:?data_ov073_02123360@@3PAHA=_data_ov073_02123360")
/* the arm9 RNG seed, hosted as `int data_0209e650[8]` in hal/auto_bss.cpp.
   hal/cxx_aliases.cpp already binds the `int` spelling (?..@@3HA); ov073's
   func_ov073_0211f61c spells it `unsigned short`. */
#pragma comment(linker, "/alternatename:?data_0209e650@@3GA=_data_0209e650")
/* Player::GetHurtState WAS AN /alternatename ON THIS LINE AND IT WAS WRONG.
   The matched TU is a real C++ method (carried by slice_w1l5.txt) and its
   callers spell the flat C name -- but an /alternatename is a NAME bridge and
   never an ABI bridge, so pointing the flat name at ?GetHurtState@Player@@
   QAEHXZ delivered the receiver nowhere. The face is further down this file
   and the evidence is in its header. */

/* ONE CROSS-LANGUAGE SPELLING in the closure. func_ov073_0212128c reaches the
   arm9 veneer func_02011d08, whose matched TU calls _ZN5Sound13Func_02048ec4Ev
   as a C name, while src/_ZN5Sound13Func_02048ec4Ev.cpp defines it as a CLASS
   STATIC. Both TUs are in port/slice_w12.txt and this is the bridge -- the
   Sound::LoadInitialGroup precedent in hal/cxx_aliases.cpp, alias rather than
   face so no second declaration of Sound has to agree with the first. */
#pragma comment(linker, "/alternatename:__ZN5Sound13Func_02048ec4Ev=?Func_02048ec4@Sound@@SAXXZ")

/* THE RECEIVER-BRIDGING FACE FOR Player::GetHurtState, replacing the
   /alternatename this file used to carry beside the data bridges above.
   port/abi_checks.txt section 6 defect 3.

   The deleted directive was

     __ZN6Player12GetHurtStateEv = ?GetHurtState@Player@@QAEHXZ

   flat C name on the left, __thiscall method on the right, which is the
   dropped-receiver direction: the caller pushes the player, the body reads
   ECX. THREE COMPILED CALL SITES pass it the ARM way and always did --

     src/func_ov073_02120ed0.c:20   extern int _ZN6Player12GetHurtStateEv(void *self);
     src/func_ov073_02120ed0.c:112  if (_ZN6Player12GetHurtStateEv(p) == 4) goto hz;
     src/func_ov073_02120ed0.c:113  if (_ZN6Player12GetHurtStateEv(p) == 5) goto hz;
     src/func_ov077_02126640.cpp:37 if (_ZN6Player12GetHurtStateEv(a) < 0) {

   -- and the body src/_ZN6Player12GetHurtStateEv.cpp:10 reads `this` twice:

     if (_ZN6Player7IsStateERNS_5StateE((char *)this, data_ov002_02110094))
         return mStateStep & 7;
     return -1;

   so through the directive the state test ran against whatever ECX held and
   the returned hurt step was read out of that same object. It never faults on
   a plausible pointer; it answers the wrong question. All three sites are
   damage tests (a Chief Chilly slam and an ov077 reader), so the failure is a
   character who does or does not react to being hit, not a crash.

   The face is a plain forward: the value the caller pushed goes into ECX where
   the method reads it. */
struct Player
{
    /* include/Player.h:333 declares `int GetHurtState();` on
       `struct Player : Actor`, which decorates ?GetHurtState@Player@@QAEHXZ.
       Declared locally rather than by including Player.h, the reason
       hal/bob_enemy_bridges.cpp gives beside its own one-method Player shadow:
       a wrong signature here is an LNK2019 on a name that does not exist,
       never a quiet call to a sibling. Actor.h forward-declares Player, so
       this completes that declaration and does not conflict with it. */
    int GetHurtState();
};

extern "C" int _ZN6Player12GetHurtStateEv(void *thiz)
{ return ((Player *)thiz)->Player::GetHurtState(); }

// ---- the trap --------------------------------------------------------------
static void ov73_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov073 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov073 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV73_TRAP(n) \
    static int __fastcall ov73_trap##n(void *s, void *) \
    { ov73_trap_report(s, n); return 0; }
OV73_TRAP(13) OV73_TRAP(14) OV73_TRAP(17) OV73_TRAP(30)
#undef OV73_TRAP

static int __fastcall ov73_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov73_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov73_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov73_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov73_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov73_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov73_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov73_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov73_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov73_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov73_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov73_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov73_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov73_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov73_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov73_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov73_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov73_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov73_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov73_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov73_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov73_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* The shared half, every word of it read off the two tables' relocations (they
   agree slot for slot outside each class's own overrides, and agree with
   ov045's six). The caller writes its own 0/3/6/9/16/17 and, for CccArena,
   its 31; ChiefChilly also replaces 12 and 29.

   Slots 13/14 are the ActorBase Virtual34/Virtual38 traps and 30 declines,
   both the wf/ov045 reading (30's ROM body returns a Vector3 by value and the
   sret contract is unproved). Slot 17 traps here and both callers replace it.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   and dumpbin proof in hal/actor_classes_ov002g200.cpp. */
static void ov73_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov73_binit;
    vt[2]  = (void *)ov73_ainit;
    vt[4]  = (void *)ov73_bclean;
    vt[5]  = (void *)ov73_aclean;
    vt[7]  = (void *)ov73_bbeh;
    vt[8]  = (void *)ov73_abeh;
    vt[10] = (void *)ov73_bren;
    vt[11] = (void *)ov73_aren;
    vt[12] = (void *)ov73_pdes;
    vt[13] = (void *)ov73_trap13;
    vt[14] = (void *)ov73_trap14;
    vt[15] = (void *)ov73_heap;
    vt[17] = (void *)ov73_trap17;
    vt[18] = (void *)ov73_yoshi;
    vt[19] = (void *)ov73_turn_egg;
    vt[20] = (void *)ov73_v50;
    vt[21] = (void *)ov73_pounded;
    vt[22] = (void *)ov73_atk1;
    vt[23] = (void *)ov73_atk2;
    vt[24] = (void *)ov73_kicked;
    vt[25] = (void *)ov73_pushed;
    vt[26] = (void *)ov73_cannon;
    vt[27] = (void *)ov73_mega;
    vt[28] = (void *)ov73_under;
    vt[29] = (void *)ov73_egg;
    vt[30] = (void *)ov73_trap30;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov73_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov73_bringup_done;
DSSTATE_END

extern "C" void port_ov73_bringup(void)
{
    if (g_ov73_bringup_done)
        return;
    g_ov73_bringup_done = 1;
    port_ov073_pack_check();
    port_ov073_syms_patch();
    port_ov073_states_seat();       /* BEFORE the sinits -- they copy records */
    __sinit_ov073_02122874();
    __sinit_ov073_02122d48();
}

// ============================================================================
// CCC_ARENA (170) / CCC_BIG_ICE (171) / CCC_SMALL_ICE (172) -- 0x021231e8
// ============================================================================
//
// 828-byte object built by Platform's constructor; Model at +0xd4,
// MovingMeshCollider at +0x124, the state-table pointer at +0x320 and the
// row index the id selected at +0x32c. Its Behavior decodes the state record
// BY HAND (`o+8`, then adj>>1 / adj&1) in matched src, which is why that TU is
// right as written and only the two ChangeState veneers are host copies.
extern "C" {
int _ZN8CccArena13InitResourcesEv(void *self);     /* slot 0  */
int _ZN8CccArena16CleanupResourcesEv(void *self);  /* slot 3  */
int _ZN8CccArena8BehaviorEv(void *self);           /* slot 6  */
int _ZN8CccArena6RenderEv(void *self);             /* slot 9  */
int *_ZN8CccArenaD1Ev(int *self);                  /* slot 16 */
int *_ZN8CccArenaD0Ev(int *self);                  /* slot 17 */
int func_ov073_021223f4(char *self);               /* slot 31 */
void *CccArena_Spawn(void);
void *CccBigIce_Spawn(void);
void *CccSmallIce_Spawn(void);
DSSTATE_BEGIN
void *_ZTV8CccArena[32];
DSSTATE_END
}
/* ewb = the ice block. The D1/D0 open their teardown by restoring the OWN table
   under its RTTI spelling (0x021231e4 -> 0x0212314c -> "13daObjEwbIce_c") and
   then store the zeroed _ZTV10dBgActor_c placeholder, which hal/actor_classes.cpp
   already defines port-wide -- a dying-object write, the ov045 reading: the two
   stores sit between direct calls with no dispatch in between. */
#pragma comment(linker, "/alternatename:__ZTV13daObjEwbIce_c=__ZTV8CccArena")
static int __fastcall ccc_init(void *s, void *)
{ return _ZN8CccArena13InitResourcesEv(s); }
static int __fastcall ccc_clean(void *s, void *)
{ return _ZN8CccArena16CleanupResourcesEv(s); }
static int __fastcall ccc_behavior(void *s, void *)
{ return _ZN8CccArena8BehaviorEv(s); }
static int __fastcall ccc_render(void *s, void *)
{ port_actor_render_probe("CCC_ARENA", (char *)s + 0xd4);
  return _ZN8CccArena6RenderEv(s); }
static int __fastcall ccc_d1(void *s, void *)
{ return (int)(size_t)_ZN8CccArenaD1Ev((int *)s); }
static int __fastcall ccc_d0(void *s, void *)
{ return (int)(size_t)_ZN8CccArenaD0Ev((int *)s); }
/* slot 31 is CccArena's own Kill (poof dust + bank-3 sound), not
   Platform::Kill -- the one place this cast's tail differs from ov045's. */
static int __fastcall ccc_kill(void *s, void *)
{ return func_ov073_021223f4((char *)s); }
extern "C" void hal_fill_ccc_arena_vtable(void)
{
    port_ov73_bringup();
    void *volatile *vt = (void *volatile *)_ZTV8CccArena;
    ov73_fill_shared(vt);
    vt[0]  = (void *)ccc_init;
    vt[3]  = (void *)ccc_clean;
    vt[6]  = (void *)ccc_behavior;
    vt[9]  = (void *)ccc_render;
    vt[16] = (void *)ccc_d1;
    vt[17] = (void *)ccc_d0;
    vt[31] = (void *)ccc_kill;
}

// ============================================================================
// CHIEF_CHILLY (218) -- table 0x02123090, THIRTY-ONE slots
// ============================================================================
//
// 0x504-byte Enemy; MovingCylinderClsnWithPos at +0x110, WithMeshClsn at
// +0x150, BlendModelAnim at +0x30c, ShadowModel at +0x380, the state pointer at
// +0x37c and three constructed arrays at +0x3e8 (8 x 0xc), +0x448 (8 x 0xc) and
// +0x4d4 (2 x 0xc). Thirty-one slots: it is an Enemy, not a Platform, and its
// table ends on Actor::OnAimedAtWithEggReturnVec at slot 30 -- the ov060
// Bowser/BowserTail/BowserFire/SpikeBomb shape.
extern "C" {
int _ZN11ChiefChilly13InitResourcesEv(void *self);    /* slot 0  (faced below) */
int _ZN11ChiefChilly16CleanupResourcesEv(void);       /* slot 3  */
int _ZN11ChiefChilly8BehaviorEv(void *self);          /* slot 6, HOST COPY */
int _ZN11ChiefChilly6RenderEv(void *self);            /* slot 9, HOST COPY */
void _ZN11ChiefChilly16OnPendingDestroyEv(void);      /* slot 12 */
int *_ZN11ChiefChillyD1Ev(int *self);                 /* slot 16 */
int *_ZN11ChiefChillyD0Ev(int *self);                 /* slot 17 */
int func_ov073_02121ec0(void);                        /* slot 29 */
DSSTATE_BEGIN
void *_ZTV11ChiefChilly[31];
DSSTATE_END
}
/* THE FACTORY, the r0 ride-through this file's header derives. */
extern "C" void *port_factory_chief_chilly(void)
{
    char *p = (char *)_ZN9ActorBasenwEj(0x504);
    if (p) {
        _ZN5EnemyC2Ev(p);                     /* ROM: entered with r0 = p */
        *(void **)p = (void *)_ZTV11ChiefChilly;
        _ZN25MovingCylinderClsnWithPosC1Ev(p + 0x110);
        _ZN12WithMeshClsnC1Ev(p + 0x150);
        _ZN14BlendModelAnimC1Ev(p + 0x30c);
        _ZN11ShadowModelC1Ev(p + 0x380);
        func_020733a8(p + 0x3e8, 8, 0xc, (void *)&func_0203d384,
                      (void *)&func_020072c0);
        func_020733a8(p + 0x448, 8, 0xc, (void *)&func_0203d384,
                      (void *)&func_020072c0);
        func_020733a8(p + 0x4d4, 2, 0xc, (void *)&func_0203d384,
                      (void *)&func_020072c0);
    }
    return p;
}
static int __fastcall cc_init(void *s, void *)
{ return _ZN11ChiefChilly13InitResourcesEv(s); }
static int __fastcall cc_clean(void *s, void *)
{ return _ZN11ChiefChilly16CleanupResourcesEv(); }
static int __fastcall cc_behavior(void *s, void *)
{ return _ZN11ChiefChilly8BehaviorEv(s); }
static int __fastcall cc_render(void *s, void *)
{ port_actor_render_probe("CHIEF_CHILLY", (char *)s + 0x30c);
  return _ZN11ChiefChilly6RenderEv(s); }
/* slot 12 is his own OnPendingDestroy, a bare `bx lr` in the ROM. */
static int __fastcall cc_pdes(void *s, void *)
{ (void)s; _ZN11ChiefChilly16OnPendingDestroyEv(); return 0; }
static int __fastcall cc_d1(void *s, void *)
{ return (int)(size_t)_ZN11ChiefChillyD1Ev((int *)s); }
static int __fastcall cc_d0(void *s, void *)
{ return (int)(size_t)_ZN11ChiefChillyD0Ev((int *)s); }
/* slot 29 overrides Actor::OnAimedAtWithEgg. The ROM body is two
   instructions, `mov r0, #0x64000; bx lr`. */
static int __fastcall cc_egg(void *s, void *)
{ (void)s; return func_ov073_02121ec0(); }
extern "C" void hal_fill_chief_chilly_vtable(void)
{
    port_ov73_bringup();
    void *volatile *vt = (void *volatile *)_ZTV11ChiefChilly;
    ov73_fill_shared(vt);
    vt[0]  = (void *)cc_init;
    vt[3]  = (void *)cc_clean;
    vt[6]  = (void *)cc_behavior;
    vt[9]  = (void *)cc_render;
    vt[12] = (void *)cc_pdes;
    vt[16] = (void *)cc_d1;
    vt[17] = (void *)cc_d0;
    vt[29] = (void *)cc_egg;
    /* 31 slots: no slot 31. See this file's header and port/ov073_syms.txt. */
}

// ---- method faces ----------------------------------------------------------
// The five bodies src defines as real C++ methods rather than extern-C free
// functions. The IceSheet/SwitchPillar/ov045 recipe -- these compile from src
// as real methods, so the face is the C-name bridge INTO them, not a host copy.
// ChiefChilly's Behavior and Render are NOT faced here: both are host copies in
// port/unmatched/Ov073_State.cpp and their matched TUs are out of the slice.
#include "CccArena.h"
#include "ChiefChilly.h"
extern "C" {
int _ZN8CccArena13InitResourcesEv(void *self)
{ return ((CccArena *)self)->CccArena::InitResources(); }
int _ZN8CccArena16CleanupResourcesEv(void *self)
{ return ((CccArena *)self)->CccArena::CleanupResources(); }
int _ZN8CccArena8BehaviorEv(void *self)
{ return ((CccArena *)self)->CccArena::Behavior(); }
int _ZN8CccArena6RenderEv(void *self)
{ return ((CccArena *)self)->CccArena::Render(); }
int _ZN11ChiefChilly13InitResourcesEv(void *self)
{ return ((ChiefChilly *)self)->ChiefChilly::InitResources(); }
}
