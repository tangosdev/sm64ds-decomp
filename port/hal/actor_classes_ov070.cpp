// RUN LINKW WAVE 5 (lane w5-c): THE ov070 ENEMY PACK -- Amp, FlameChomp,
// FlameChompFire. Bowser in the Fire Sea's roaming cast (level 37 places
// Amp x2 and FlameChomp x1; the fire is spawned at run time by the chomp).
//
// ov070 carries FOUR classes; FlyGuy (232) is the one left unregistered: no
// bootable level spawns it (the ov019 rule), so it gets no row and no fill.
// Its sinit still runs and its state PMFs are still seated -- see the seat
// section -- so a future FlyGuy lane is a row plus a fill, nothing else.
//
// ---- THE MEASUREMENTS THIS FILE STANDS ON ---------------------------------
//
// SpawnInfo records, raw bytes out of extracted/overlays/overlay_0070.bin
// (base 0x0211f000 -- ov070 shares its load window with ov071/ov073/ov074,
// so dsd flags every record `ambiguous` and the raw read is the resolver):
//
//   id   class            SpawnInfo    word[0] = Spawn    vtable (+0x24)
//   232  FlyGuy           0x02123144   0x02120520         0x02123168
//   266  Amp              0x02123254   0x021210ac         0x02123278
//   270  FlameChomp       0x0212334c   0x02121af8         0x02123370
//   271  FlameChompFire   0x02123424   0x021221fc         0x02123448
//
// Every word[1] low half reads back the id it was reached from through the
// arm9 spawn table (0x02090864 + id*4). NO NAMING SHIFT in this overlay --
// checked, not assumed: each Spawn's literal pool stores the table adjacent
// to its own SpawnInfo, and all 33 own vtable slots stay inside their own
// class's code (28 in class-named TUs, 5 in address-named TUs of the same
// class, zero cross-class; delinks.txt, class by class). ov045's off-by-one
// is ov045's.
//
// ALL FOUR TABLES ARE 31 SLOTS: each span is pinned by the next dsd symbol
// AND by the reloc run ending at slot 30 (slots 31+ carry no relocation).
// The plain Actor shape -- these are enemies, not Platforms; no Kill tail.
//
// Own slots (relocs.txt, targets resolved through delinks.txt):
//   FlyGuy      0/3/6/9/12/16/17 + 18/19/29 (OnYoshiTryEat / OnTurnIntoEgg /
//               OnAimedAtWithEgg overrides)   -- measured, NOT filled (no row)
//   Amp         0/3/6/9/12/16/17
//   FlameChomp  0/3/6/9/12/16/17 + 18
//   FlameChompFire  same as FlameChomp
// Every other slot is the arm9 shared half and agrees address-for-address
// with the ov045 tables' shared half (1=0x02011268 BeforeInitResources ...
// 30=0x020100dc OnAimedAtWithEggReturnVec), so the fill below mirrors
// hal/actor_classes_ov045.cpp's reviewed shared fill slot for slot, traps
// included (13/14 the ActorBase Virtual34/38 traps, 30 declines: the ROM body
// returns a Vector3 by value and the sret contract is unproved).
//
// ---- TWO SLOT BODIES ARE INLINE HOST COPIES (the stub-guard rule) ---------
//
// src's func_ov070_021211bc / func_ov070_02121bdc (the chomps' slot 18,
// OnYoshiTryEat) and func_ov070_02121438 / func_ov070_02121fb0 (both packs'
// state-0 Kill handlers) carry the "recovered from vtable slot identity"
// marker: behavioral guesses, and port/tools/inferred_stub_guard.py rightly
// fails any build that seats a NEW one from port/hal. The four were
// disassembled here and the two that this file must reference are inlined
// from the ROM listing instead (ov70_chomp_yoshi below, and the two Kill
// transcriptions in the seat section); the slot-18 pair is two instructions:
//
//   021211bc  mov r0, #5          02121bdc  mov r0, #5
//   021211c0  bx  lr              02121be0  bx  lr
//
// The marked TUs stay OUT of every port/hal reference; the guard baseline
// (two symbols) is untouched.
//
// ---- THE D1 DESTRUCTORS ARE HOST CHAINS (the gate-199/200/202 treatment) --
//
// _ZN3AmpD1Ev.cpp / _ZN10FlameChompD1Ev.cpp / _ZN14FlameChompFireD1Ev.cpp are
// real MSVC destructors over structs local to their own TUs: they emit
// ??1Amp@@UAE@XZ against member teardowns (??1ModelAnim@...) the port does
// not define. Each chain below is the disassembly of the ROM's own D1 --
// store the OWN table, member D1s in reverse layout order, Actor::D2 --
// and the member set, order and offsets were read off the listing at
// 0x02120570 / 0x02121118 / 0x02121b48, not off the shadow structs. The D0
// siblings are plain .c TUs spelling the same chains with real C names and
// link from the slice untouched (they restore the table by its RTTI
// spelling, aliased below).
//
// ---- THE STATE PMFs (the Scuttlebug/MontyMole/Crate treatment) ------------
//
// The four sinits copy 8-byte {fn, 0} source records out of .data into the
// live bss state arrays the dispatchers read (FlameChomp's dispatcher is
// func_ov070_02121880 over data_ov070_021236ac[8]; FlameChompFire's is
// func_ov070_02122044 over data_ov070_021236ec[4]; Amp's is
// func_ov070_02120da8 over data_ov070_02123668; FlyGuy's six [2] arrays at
// 0x0212358c..0x021235dc). Every source fn word relocates to ov070 code, so
// the mount leaves DS addresses in them and a dispatch would land in dead
// memory. ov70_seat_state_pmfs rewrites all 30 source fn words with the host
// bodies BEFORE the sinits copy. FlyGuy's twelve are seated too: correct for
// the ROM (same bodies, host addresses) and it leaves nothing stale behind
// for the lane that eventually registers the class.
//
// ---- THE SINITS ALL LINK --------------------------------------------------
//
// All four __sinit_ov070_* are matched TUs on the slice and each name is
// referenced by nothing but its own definition, swept src-wide -- ov045's
// 0x02112280 window collision has no counterpart here. They build the packs'
// SharedFilePtrs (FlyGuy model 0x411 + anims 0x412-0x417; Amp models
// 0x2b1/0x2b3, anims 0x2b2/0x2b5, texseq 0x2b4; FlameChomp model 0x351),
// chain their destructor nodes, and run the PMF copies above.
//
// ---- LANE OWNERSHIP -------------------------------------------------------
//
// Same situation hal/actor_classes_ov045.cpp records: no wave-5 lane owns
// hal/actor_overlays.cpp, so the mount bring-up rides the first registry fill
// (ov70_bringup, one done-guard, all three fills call it). THE HANDOFF stands
// for both files now: whoever next owns actor_overlays.cpp should move
// ov45_bringup's and ov70_bringup's bodies into port_actor_overlays_sinits.
#include <cstdio>
#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half -- the ov045 exemplar's set, address-verified against
   ov070's own tables (see the header) */
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

/* the generated ov070 per-symbol mount (build/host-src/ov070_syms.c) */
void port_ov070_pack_check(void);
void port_ov070_syms_patch(void);
/* the four sinits, all linkable (see the header) */
void __sinit_ov070_02122afc(void);
void __sinit_ov070_02122d80(void);
void __sinit_ov070_02122f30(void);
void __sinit_ov070_02123030(void);

/* Amp's own slots (Cleanup/OnPendingDestroy/D0 are .c TUs with C names;
   Init/Behavior/Render are real methods, faced at the bottom of this file) */
int _ZN3Amp13InitResourcesEv(void *self);          /* slot 0  */
int _ZN3Amp16CleanupResourcesEv(void *self);       /* slot 3  */
int _ZN3Amp8BehaviorEv(void *self);                /* slot 6  */
int _ZN3Amp6RenderEv(void *self);                  /* slot 9  */
void _ZN3Amp16OnPendingDestroyEv(void);            /* slot 12 (ROM: bx lr) */
int *_ZN3AmpD0Ev(int *self);                       /* slot 17 */
/* FlameChomp's */
int _ZN10FlameChomp13InitResourcesEv(void *self);
int _ZN10FlameChomp16CleanupResourcesEv(void *self);
int _ZN10FlameChomp8BehaviorEv(void *self);
int _ZN10FlameChomp6RenderEv(void *self);
void _ZN10FlameChomp16OnPendingDestroyEv(void);
int *_ZN10FlameChompD0Ev(int *self);
/* FlameChompFire's */
int _ZN14FlameChompFire13InitResourcesEv(void *self);
int _ZN14FlameChompFire16CleanupResourcesEv(void *self);
int _ZN14FlameChompFire8BehaviorEv(void *self);
int _ZN14FlameChompFire6RenderEv(void *self);
void _ZN14FlameChompFire16OnPendingDestroyEv(void);
int *_ZN14FlameChompFireD0Ev(int *self);
/* the factories: each stores its OWN host table by real name (no VT1
   placeholder anywhere in this pack), so all three register directly */
void *Amp_Spawn(void);
void *FlameChomp_Spawn(void);
void *FlameChompFire_Spawn(void);

/* what the D1 chains spell out by hand (the ROM's own call set) */
void _ZN12WithMeshClsnD1Ev(void *self);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN18TextureTransformerD1Ev(void *self);
void _ZN15TextureSequenceD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);

/* what the two inline Kill transcriptions call */
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned n, const void *v);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *bca,
                                                 int mode, int speed,
                                                 unsigned start);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned id, int x,
                                                    int y, int z);

/* the host vtable arrays (excluded from port/ov070_syms.txt by the
   ov095/ov080 rule) */
DSSTATE_BEGIN
void *_ZTV3Amp[31];
void *_ZTV10FlameChomp[31];
void *_ZTV14FlameChompFire[31];
DSSTATE_END

/* the 30 state PMF source records and the one file table the Kill
   transcription reads, all in the ov070 mount */
extern unsigned char data_ov070_021230c0[], data_ov070_021230c8[],
    data_ov070_021230d0[], data_ov070_021230d8[], data_ov070_021230e0[],
    data_ov070_021230e8[], data_ov070_021230f0[], data_ov070_021230f8[],
    data_ov070_02123100[], data_ov070_02123108[], data_ov070_02123110[],
    data_ov070_02123118[];
extern unsigned char data_ov070_0212320c[], data_ov070_02123214[],
    data_ov070_0212321c[], data_ov070_02123224[], data_ov070_0212322c[],
    data_ov070_02123234[];
extern unsigned char data_ov070_021232f4[], data_ov070_021232fc[],
    data_ov070_02123304[], data_ov070_0212330c[], data_ov070_02123314[],
    data_ov070_0212331c[], data_ov070_02123324[], data_ov070_0212332c[];
extern unsigned char data_ov070_021233ec[], data_ov070_021233f4[],
    data_ov070_021233fc[], data_ov070_02123404[];
extern unsigned char data_ov070_021234c4[];

/* the 28 unmarked state handlers the seat installs (all matched src on the
   slice; the two marked Kill roots are the transcriptions below) */
int func_ov070_0211fa80(void *); int func_ov070_0211fd98(void *);
int func_ov070_0211f62c(void *); int func_ov070_0211fd60(void *);
int func_ov070_0211f48c(void *); int func_ov070_0211ffa8(void *);
int func_ov070_0211fae4(void *); int func_ov070_0211f450(void *);
int func_ov070_0211f368(void *); int func_ov070_0211f694(void *);
int func_ov070_0211f5f0(void *); int func_ov070_0211f6e0(void *);
int func_ov070_02120ce4(void *); int func_ov070_02120bf8(void *);
int func_ov070_02120910(void *); int func_ov070_02120cac(void *);
int func_ov070_021208a4(void *); int func_ov070_021209e4(void *);
int func_ov070_02121548(void *); int func_ov070_0212156c(void *);
int func_ov070_021214f8(void *); int func_ov070_021216b8(void *);
int func_ov070_02121710(void *); int func_ov070_021213cc(void *);
int func_ov070_021217ac(void *);
int func_ov070_02121ef8(void *); int func_ov070_02121f18(void *);
int func_ov070_02121eb0(void *);
}

/* The D0 .c TUs restore each table by its RTTI spelling; the recovered names
   Amp_Spawn/D1 use are the host arrays themselves. One storage per class. */
#pragma comment(linker, "/alternatename:__ZTV7daBrq_c=__ZTV3Amp")
#pragma comment(linker, "/alternatename:__ZTV8daKrpa_c=__ZTV10FlameChomp")
#pragma comment(linker, "/alternatename:__ZTV8daKpFr_c=__ZTV14FlameChompFire")

/* C++ TUs in the pack declare seven mounted objects without extern "C", so
   MSVC decorates the names (the bowserpuzzle/bbh precedent). Each decorated
   spelling is the same storage as the mounted C name. */
#pragma comment(linker, "/alternatename:?data_ov070_021231f4@@3UBTA_File@@A=_data_ov070_021231f4")
#pragma comment(linker, "/alternatename:?data_ov070_02123500@@3HA=_data_ov070_02123500")
#pragma comment(linker, "/alternatename:?data_ov070_02123528@@3PAIA=_data_ov070_02123528")
#pragma comment(linker, "/alternatename:?data_ov070_021235bc@@3DA=_data_ov070_021235bc")
#pragma comment(linker, "/alternatename:?data_ov070_021235cc@@3DA=_data_ov070_021235cc")
#pragma comment(linker, "/alternatename:?data_ov070_021235ec@@3UD2@@A=_data_ov070_021235ec")
#pragma comment(linker, "/alternatename:?data_ov070_021235f4@@3UD1@@A=_data_ov070_021235f4")
/* ...and the second wave of spellings the LINK named once the pack's method
   TUs compiled: Amp::InitResources declares its SharedFilePtrs by their real
   type (a different decoration than the probe's shadow-typed ones above),
   plus its pointer table, its rest vector, and the arm9 identity matrix
   data_02082128 under two more local-type spellings (romdata.c owns the one
   real C symbol; the bbh file already carries its @@3UM48@@A spelling). */
#pragma comment(linker, "/alternatename:?data_ov070_021235ec@@3USharedFilePtr@@A=_data_ov070_021235ec")
#pragma comment(linker, "/alternatename:?data_ov070_021235fc@@3USharedFilePtr@@A=_data_ov070_021235fc")
#pragma comment(linker, "/alternatename:?data_ov070_02123604@@3USharedFilePtr@@A=_data_ov070_02123604")
#pragma comment(linker, "/alternatename:?data_ov070_021222e0@@3PAPAUSharedFilePtr@@A=_data_ov070_021222e0")
#pragma comment(linker, "/alternatename:?data_ov070_0212365c@@3UVector3@@A=_data_ov070_0212365c")
#pragma comment(linker, "/alternatename:?data_02082128@@3DA=_data_02082128")
#pragma comment(linker, "/alternatename:?data_02082128@@3UBlk@@A=_data_02082128")
/* Amp::CleanupResources walks its two-pointer SharedFilePtr table under an
   ov074 FUNCTION spelling -- the shared-load-window naming race (ov070/ov074
   both cover 0x021222e0; dsd's ov074 export won inside that one TU). The use
   is address-only (indexed reads, never called), so the alias is storage
   identity, not a code seam.
   THE PRAGMA MOVED to hal/cxx_aliases.cpp (lane w8-shadows): src/ carries a
   matched TU named func_ov074_021222e0, so this name appears in linkage.py's
   replacement queue, and for a generated ovNNN_syms.c.obj that tool reads the
   PORT_HOST_ABI ruling out of cxx_aliases.cpp -- the alias registry -- and
   nowhere else. The pragma had to travel with its tag. Same three targets
   compile both files, so the alias reaches exactly the links it did before. */
/* FlameChomp/FlameChompFire InitResources declare the collider initialiser's
   Itanium name without extern "C", so MSVC decorates it as a C++ free
   function (@@YA, cdecl) -- alias-legal onto the matched TU's C symbol, the
   bowserpuzzle UpdatePosWithTransform precedent. */
#pragma comment(linker, "/alternatename:?_ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj@@YAXPAX0PBUVector3@@HHII@Z=__ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj")

/* Two WithMeshClsn method spellings the pack's C++ TUs want under their own
   declared return types. Same __thiscall bodies, already defined for the
   spellings on the right (the value comes back in EAX either way; the _N
   caller reads AL of the same 0/1). A GivePlayerCoins alias would be
   ABI-wrong (the defined body is a cdecl C function), so that one is a real
   face at the bottom of this file. */
#pragma comment(linker, "/alternatename:?GetFloorResult@WithMeshClsn@@QBEPAUSurfaceInfo@@XZ=?GetFloorResult@WithMeshClsn@@QBEHXZ")
#pragma comment(linker, "/alternatename:?IsOnWall@WithMeshClsn@@QBE_NXZ=?IsOnWall@WithMeshClsn@@QBEHXZ")

/* The probe's one "genuinely unmatched" external, resolved instead of
   copied: func_02015bcc IS _ZN9Animation8FinishedEv (arm9 symbols.txt names
   the address; the matched method TU and hal/player_bridges.cpp's C bridge
   are both in the link already). The ov070 TUs spell it by address. */
#pragma comment(linker, "/alternatename:_func_02015bcc=__ZN9Animation8FinishedEv")

// ---- the trap (the ov45 pattern, ov070's spelling) -------------------------
static void ov70_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov070 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov070 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV70_TRAP(n) \
    static int __fastcall ov70_trap##n(void *s, void *) \
    { ov70_trap_report(s, n); return 0; }
OV70_TRAP(13) OV70_TRAP(14) OV70_TRAP(30)
#undef OV70_TRAP

static int __fastcall ov70_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov70_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov70_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov70_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov70_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov70_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov70_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov70_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov70_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov70_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov70_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19: the three-parameter shape so it emits `ret 4` -- the dispatch
   site pushes the Player the callee pops (the wf_turn_egg contract). */
static int __fastcall ov70_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov70_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov70_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov70_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov70_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov70_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov70_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov70_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov70_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov70_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov70_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Slots 1..30 of the shared half; callers write their own 0/3/6/9/12/16/17
   (and the chomps their 18). Volatile for the gate-200 elided-stores bug --
   three callers, same as the ov45 file documents. */
static void ov70_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov70_binit;
    vt[2]  = (void *)ov70_ainit;
    vt[4]  = (void *)ov70_bclean;
    vt[5]  = (void *)ov70_aclean;
    vt[7]  = (void *)ov70_bbeh;
    vt[8]  = (void *)ov70_abeh;
    vt[10] = (void *)ov70_bren;
    vt[11] = (void *)ov70_aren;
    vt[12] = (void *)ov70_pdes;
    vt[13] = (void *)ov70_trap13;
    vt[14] = (void *)ov70_trap14;
    vt[15] = (void *)ov70_heap;
    vt[18] = (void *)ov70_yoshi;
    vt[19] = (void *)ov70_turn_egg;
    vt[20] = (void *)ov70_v50;
    vt[21] = (void *)ov70_pounded;
    vt[22] = (void *)ov70_atk1;
    vt[23] = (void *)ov70_atk2;
    vt[24] = (void *)ov70_kicked;
    vt[25] = (void *)ov70_pushed;
    vt[26] = (void *)ov70_cannon;
    vt[27] = (void *)ov70_mega;
    vt[28] = (void *)ov70_under;
    vt[29] = (void *)ov70_egg;
    vt[30] = (void *)ov70_trap30;
}

// ---- the two Kill roots, transcribed from the ROM (stub-guard rule) --------
//
// FlameChomp state 0, ROM 0x02121438 (0xc0 bytes), every line against the
// listing: Sound::PlayBank0(9, pos+0x74); clear bit 0 of [self+0xb0]; the
// velocity/accel block -0x2000 -> +0x9c, -0x3c000 -> +0xa0, 0xa000 -> +0x98,
// 0x28000 -> +0xa8; scale 0x1000 -> +0x80/84/88; byte 0x2d -> +0x3ac;
// ModelAnim::SetAnim(this+0xd4, data_ov070_021234c4, 0, 0x1000, 0) -- r3
// still holds the 0x1000 written to the scale, the listing's own trick;
// Particle::System::NewSimple(0x43 then 0x44, pos 0x5c/60/64); state word 3
// -> +0x3a0; return 1.
static int ov70_flamechomp_kill(void *self)
{
    char *c = (char *)self;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
    *(int *)(c + 0xb0) &= ~1;
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = 0xa000;
    *(int *)(c + 0xa8) = 0x28000;
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    *(unsigned char *)(c + 0x3ac) = 0x2d;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov070_021234c4,
                                                0, 0x1000, 0);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x43, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x44, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
    *(int *)(c + 0x3a0) = 3;
    return 1;
}
// FlameChompFire state 0, ROM 0x02121fb0 (0x20 bytes): 0xa000 -> +0x98,
// byte 0x69 -> +0x32c, 0 -> +0x320, return 1.
static int ov70_fcf_kill(void *self)
{
    char *c = (char *)self;
    *(int *)(c + 0x98) = 0xa000;
    *(unsigned char *)(c + 0x32c) = 0x69;
    *(int *)(c + 0x320) = 0;
    return 1;
}

// ---- the state PMF seat ----------------------------------------------------
static void ov70_seat_state_pmfs(void)
{
    struct Row { unsigned char *rec; void *fn; };
    static const Row rows[] = {
        /* FlyGuy's twelve, source order 0x021230c0.. */
        { data_ov070_021230c0, (void *)func_ov070_0211fa80 },
        { data_ov070_021230c8, (void *)func_ov070_0211fd98 },
        { data_ov070_021230d0, (void *)func_ov070_0211f62c },
        { data_ov070_021230d8, (void *)func_ov070_0211fd60 },
        { data_ov070_021230e0, (void *)func_ov070_0211f48c },
        { data_ov070_021230e8, (void *)func_ov070_0211ffa8 },
        { data_ov070_021230f0, (void *)func_ov070_0211fae4 },
        { data_ov070_021230f8, (void *)func_ov070_0211f450 },
        { data_ov070_02123100, (void *)func_ov070_0211f368 },
        { data_ov070_02123108, (void *)func_ov070_0211f694 },
        { data_ov070_02123110, (void *)func_ov070_0211f5f0 },
        { data_ov070_02123118, (void *)func_ov070_0211f6e0 },
        /* Amp's six */
        { data_ov070_0212320c, (void *)func_ov070_02120ce4 },
        { data_ov070_02123214, (void *)func_ov070_02120bf8 },
        { data_ov070_0212321c, (void *)func_ov070_02120910 },
        { data_ov070_02123224, (void *)func_ov070_02120cac },
        { data_ov070_0212322c, (void *)func_ov070_021208a4 },
        { data_ov070_02123234, (void *)func_ov070_021209e4 },
        /* FlameChomp's eight; state 0 is the transcription above */
        { data_ov070_021232f4, (void *)ov70_flamechomp_kill },
        { data_ov070_021232fc, (void *)func_ov070_02121548 },
        { data_ov070_02123304, (void *)func_ov070_0212156c },
        { data_ov070_0212330c, (void *)func_ov070_021214f8 },
        { data_ov070_02123314, (void *)func_ov070_021216b8 },
        { data_ov070_0212331c, (void *)func_ov070_02121710 },
        { data_ov070_02123324, (void *)func_ov070_021213cc },
        { data_ov070_0212332c, (void *)func_ov070_021217ac },
        /* FlameChompFire's four; state 0 likewise */
        { data_ov070_021233ec, (void *)ov70_fcf_kill },
        { data_ov070_021233f4, (void *)func_ov070_02121ef8 },
        { data_ov070_021233fc, (void *)func_ov070_02121f18 },
        { data_ov070_02123404, (void *)func_ov070_02121eb0 },
    };
    for (unsigned i = 0; i < sizeof rows / sizeof rows[0]; ++i)
        *(void **)rows[i].rec = rows[i].fn;
}

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov70_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov70_bringup_done;
DSSTATE_END

extern "C" void port_ov70_bringup(void)
{
    if (g_ov70_bringup_done)
        return;
    g_ov70_bringup_done = 1;
    port_ov070_pack_check();
    port_ov070_syms_patch();
    ov70_seat_state_pmfs();          /* BEFORE the sinits copy the records */
    __sinit_ov070_02122afc();
    __sinit_ov070_02122d80();
    __sinit_ov070_02122f30();
    __sinit_ov070_02123030();
}

// ============================================================================
// AMP (id 266) -- table 0x02123278, 1076-byte object
// ============================================================================
//
// ModelAnim +0xd4, Model +0x138, TextureSequence +0x188, TextureTransformer
// +0x19c, ShadowModel +0x1b0, MovingCylinderClsnWithPos +0x1d8, WithMeshClsn
// +0x218 -- the factory constructs all seven and stores _ZTV3Amp by name.
static int __fastcall amp_init(void *s, void *)
{ return _ZN3Amp13InitResourcesEv(s); }
static int __fastcall amp_clean(void *s, void *)
{ return _ZN3Amp16CleanupResourcesEv(s); }
static int __fastcall amp_behavior(void *s, void *)
{ return _ZN3Amp8BehaviorEv(s); }
/* run linkw wave 17 (lane w17): slot 9 is a HOST COPY, not the C face. Both
   this class's Render and FlameChomp's are ModelAnim SLOT-5 shadow dispatches
   -- the Butterfly/Whomp collision -- and both faulted c0000005 in
   Model::Virtual10 through ModelAnim::Virtual18 the first frame level 27 drew
   one. The two copies and the full reading are in
   port/unmatched/Ov070_PmfDispatch.cpp; the matched C++ methods stay in src/
   and on their slice, and their C faces below stay defined and unused. */
extern "C" int port_ov070_amp_render(void *self);
extern "C" int port_ov070_flamechomp_render(void *self);
static int __fastcall amp_render(void *s, void *)
{ port_actor_render_probe("AMP", (char *)s + 0xd4);
  return port_ov070_amp_render(s); }
static int __fastcall amp_pdes(void *s, void *)
{ (void)s; _ZN3Amp16OnPendingDestroyEv(); return 0; }
/* slot 16, HOST CHAIN -- the ROM listing at 0x02120570: store the own table,
   then WithMeshClsn +0x218, MovingCylinderClsnWithPos +0x1d8, ShadowModel
   +0x1b0, TextureTransformer +0x19c, TextureSequence +0x188, Model +0x138,
   ModelAnim +0xd4, Actor::D2. */
static int __fastcall amp_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV3Amp;
    _ZN12WithMeshClsnD1Ev(t + 0x218);
    _ZN25MovingCylinderClsnWithPosD1Ev(t + 0x1d8);
    _ZN11ShadowModelD1Ev(t + 0x1b0);
    _ZN18TextureTransformerD1Ev(t + 0x19c);
    _ZN15TextureSequenceD1Ev(t + 0x188);
    _ZN5ModelD1Ev(t + 0x138);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall amp_d0(void *s, void *)
{ return (int)(size_t)_ZN3AmpD0Ev((int *)s); }
extern "C" void hal_fill_amp_vtable(void)
{
    port_ov70_bringup();
    void *volatile *vt = (void *volatile *)_ZTV3Amp;
    ov70_fill_shared(vt);
    vt[0]  = (void *)amp_init;
    vt[3]  = (void *)amp_clean;
    vt[6]  = (void *)amp_behavior;
    vt[9]  = (void *)amp_render;
    vt[12] = (void *)amp_pdes;
    vt[16] = (void *)amp_d1;
    vt[17] = (void *)amp_d0;
}

// ============================================================================
// FLAME_CHOMP (id 270) -- table 0x02123370, 944-byte object
// ============================================================================
//
// ModelAnim +0xd4, ShadowModel +0x138, MovingCylinderClsnWithPos +0x160,
// WithMeshClsn +0x1a0. Slot 18 overrides OnYoshiTryEat.
static int __fastcall fc_init(void *s, void *)
{ return _ZN10FlameChomp13InitResourcesEv(s); }
static int __fastcall fc_clean(void *s, void *)
{ return _ZN10FlameChomp16CleanupResourcesEv(s); }
static int __fastcall fc_behavior(void *s, void *)
{ return _ZN10FlameChomp8BehaviorEv(s); }
static int __fastcall fc_render(void *s, void *)
{ port_actor_render_probe("FLAME_CHOMP", (char *)s + 0xd4);
  return port_ov070_flamechomp_render(s); }
static int __fastcall fc_pdes(void *s, void *)
{ (void)s; _ZN10FlameChomp16OnPendingDestroyEv(); return 0; }
/* slot 18: the ROM body is `mov r0, #5; bx lr` (0x021211bc, quoted in the
   header) -- "I am fire, spit me out". Inlined, stub-guard rule. */
static int __fastcall fc_yoshi(void *, void *)
{ return 5; }
/* slot 16, HOST CHAIN -- the listing at 0x02121118. */
static int __fastcall fc_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV10FlameChomp;
    _ZN12WithMeshClsnD1Ev(t + 0x1a0);
    _ZN25MovingCylinderClsnWithPosD1Ev(t + 0x160);
    _ZN11ShadowModelD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall fc_d0(void *s, void *)
{ return (int)(size_t)_ZN10FlameChompD0Ev((int *)s); }
extern "C" void hal_fill_flame_chomp_vtable(void)
{
    port_ov70_bringup();
    void *volatile *vt = (void *volatile *)_ZTV10FlameChomp;
    ov70_fill_shared(vt);
    vt[0]  = (void *)fc_init;
    vt[3]  = (void *)fc_clean;
    vt[6]  = (void *)fc_behavior;
    vt[9]  = (void *)fc_render;
    vt[12] = (void *)fc_pdes;
    vt[16] = (void *)fc_d1;
    vt[17] = (void *)fc_d0;
    vt[18] = (void *)fc_yoshi;
}

// ============================================================================
// FLAME_CHOMP_FIRE (id 271) -- table 0x02123448, 816-byte object
// ============================================================================
//
// ShadowModel +0xd4, MovingCylinderClsn +0xfc (NOT the WithPos variant -- the
// D1 listing at 0x02121b48 calls 0x020149a4), WithMeshClsn +0x130. No Model
// subobject, so no render probe. Slot 18 is the same two-instruction
// OnYoshiTryEat as the chomp's (0x02121bdc).
static int __fastcall fcf_init(void *s, void *)
{ return _ZN14FlameChompFire13InitResourcesEv(s); }
static int __fastcall fcf_clean(void *s, void *)
{ return _ZN14FlameChompFire16CleanupResourcesEv(s); }
static int __fastcall fcf_behavior(void *s, void *)
{ return _ZN14FlameChompFire8BehaviorEv(s); }
static int __fastcall fcf_render(void *s, void *)
{ return _ZN14FlameChompFire6RenderEv(s); }
static int __fastcall fcf_pdes(void *s, void *)
{ (void)s; _ZN14FlameChompFire16OnPendingDestroyEv(); return 0; }
static int __fastcall fcf_yoshi(void *, void *)
{ return 5; }
static int __fastcall fcf_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV14FlameChompFire;
    _ZN12WithMeshClsnD1Ev(t + 0x130);
    _ZN18MovingCylinderClsnD1Ev(t + 0xfc);
    _ZN11ShadowModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall fcf_d0(void *s, void *)
{ return (int)(size_t)_ZN14FlameChompFireD0Ev((int *)s); }
extern "C" void hal_fill_flame_chomp_fire_vtable(void)
{
    port_ov70_bringup();
    void *volatile *vt = (void *volatile *)_ZTV14FlameChompFire;
    ov70_fill_shared(vt);
    vt[0]  = (void *)fcf_init;
    vt[3]  = (void *)fcf_clean;
    vt[6]  = (void *)fcf_behavior;
    vt[9]  = (void *)fcf_render;
    vt[12] = (void *)fcf_pdes;
    vt[16] = (void *)fcf_d1;
    vt[17] = (void *)fcf_d0;
    vt[18] = (void *)fcf_yoshi;
}

// ============================================================================
// POPPING_LAVA_BUBBLES (id 196, ov002) -- table 0x021093e0, 216-byte object
// ============================================================================
//
// The census's second bonus find, level 37's one ov002-resident skip (also on
// level 15's list). NOT the class at _ZTV18PoppingLavaBubbles -- that table is
// WATERFALL_MIST's (197), hal/actor_classes.cpp's own header derives the ov002
// shift. Id 196's record at 0x021093bc (word[0] = 0x020b6dd8 =
// PoppingLavaBubbles_Spawn, word[1] low half = 196, raw bytes out of
// overlay_0002.bin) installs the UNNAMED table at 0x021093e0, whose RTTI
// chain names the class 11daObjLava_c: the lava-bubble particle tracker. An
// earlier level-boot audit filed its spawnFunc as ov000; the raw record
// settles it inside ov002, always loaded.
//
// 31 slots; shared half identical to the ov070 tables' (address-checked).
// Own slots 0/6/16/17; 3/9/12 are ActorBase's own bodies in the ROM table
// (0x02043bf0 CleanupResources / 0x02043af0 Render / 0x02043ac0
// OnPendingDestroy -- the WATERFALL_MIST reading, same addresses).
//
// THREE OWN BODIES ARE INLINE TRANSCRIPTIONS (the stub-guard rule again):
// src's func_ov002_020b6dd0 / _020b6d84 / _020b6d4c carry the inferred-stub
// marker. Listings, in full:
//   InitResources 0x020b6dd0 (0x8):  mov r0, #1; bx lr
//   Behavior 0x020b6d84 (0x48): r4=self; ClosestPlayer(); r3=r0+0x5c;
//     func_02022c3c([r4+0xd4], 0xb7, [r3], [r3+4], [r3+8], 0); result back
//     into [r4+0xd4]; return 1.   (effect 0xb7 tracked at the player's pos)
//   D0 0x020b6d4c (0x38): store 0x021093e0; Actor::D2; Memory::Deallocate
//     (self, *data_020a0eac); return self.
// The D1 (func_ov002_020b6d28) carries no marker and links from the slice.
extern "C" {
int func_ov002_020b6d28(int *self);            /* slot 16, matched */
unsigned func_02022c3c(unsigned tracker, unsigned effect, int x, int y,
                       int z, const void *dir);
void *_ZN5Actor13ClosestPlayerEv(void *self);
void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);
void *PoppingLavaBubbles_Spawn(void);
extern void *data_020a0eac;                    /* the game heap word */
DSSTATE_BEGIN
void *data_ov002_021093e0[31];
DSSTATE_END
}
static int __fastcall plb_init(void *, void *)
{ return 1; }
static int __fastcall plb_clean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall plb_behavior(void *s, void *)
{
    char *c = (char *)s;
    char *pl = (char *)_ZN5Actor13ClosestPlayerEv(s);
    int *pos = (int *)(pl + 0x5c);
    *(unsigned *)(c + 0xd4) = func_02022c3c(*(unsigned *)(c + 0xd4), 0xb7,
                                            pos[0], pos[1], pos[2], 0);
    return 1;
}
static int __fastcall plb_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall plb_d1(void *s, void *)
{ return (int)(size_t)func_ov002_020b6d28((int *)s); }
static int __fastcall plb_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)data_ov002_021093e0;
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}
extern "C" void hal_fill_popping_lava_bubbles_vtable(void)
{
    /* ov002 is always mounted; no bring-up needed here. */
    void *volatile *vt = (void *volatile *)data_ov002_021093e0;
    ov70_fill_shared(vt);
    vt[0]  = (void *)plb_init;
    vt[3]  = (void *)plb_clean;
    vt[6]  = (void *)plb_behavior;
    vt[9]  = (void *)plb_render;
    /* 12 stays ov70_pdes = ActorBase::OnPendingDestroy, the ROM's own word */
    vt[16] = (void *)plb_d1;
    vt[17] = (void *)plb_d0;
}

// ---- method faces ----------------------------------------------------------
// The nine bodies src defines as real C++ methods against their own headers
// (Init/Behavior/Render, three classes), the ov045/IceSheet recipe: the face
// is the C-name bridge INTO the method, not a host copy of it. Plus the two
// cross-class faces the pack's TUs want: Player::Shock spelled by address
// name from a .c TU, and Actor::GivePlayerCoins wanted under its MSVC
// decoration by the .cpp TUs (the defined body is the cdecl C function, so an
// alias cannot serve -- this defines the real method forwarding into it).
#include "Amp.h"
#include "FlameChomp.h"
#include "FlameChompFire.h"
#include "Player.h"
extern "C" {
int _ZN3Amp13InitResourcesEv(void *self)
{ return ((Amp *)self)->Amp::InitResources(); }
int _ZN3Amp8BehaviorEv(void *self)
{ return ((Amp *)self)->Amp::Behavior(); }
int _ZN3Amp6RenderEv(void *self)
{ return ((Amp *)self)->Amp::Render(); }
int _ZN10FlameChomp13InitResourcesEv(void *self)
{ return ((FlameChomp *)self)->FlameChomp::InitResources(); }
int _ZN10FlameChomp8BehaviorEv(void *self)
{ return ((FlameChomp *)self)->FlameChomp::Behavior(); }
int _ZN10FlameChomp6RenderEv(void *self)
{ return ((FlameChomp *)self)->FlameChomp::Render(); }
int _ZN14FlameChompFire13InitResourcesEv(void *self)
{ return ((FlameChompFire *)self)->FlameChompFire::InitResources(); }
int _ZN14FlameChompFire8BehaviorEv(void *self)
{ return ((FlameChompFire *)self)->FlameChompFire::Behavior(); }
int _ZN14FlameChompFire6RenderEv(void *self)
{ return ((FlameChompFire *)self)->FlameChompFire::Render(); }
int _ZN6Player5ShockEj(void *self, unsigned j)
{ return ((Player *)self)->Player::Shock(j); }
void _ZN5Actor15GivePlayerCoinsER6Playerhj(void *self, void *player,
                                           unsigned char count, unsigned kind);
}
void Actor::GivePlayerCoins(Player &player, u8 count, u32 coinKind)
{ _ZN5Actor15GivePlayerCoinsER6Playerhj(this, &player, count, coinKind); }
