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
// ---- FOUR SLOT BODIES LINK FROM THE MATCHED TUs (lane mount-ov070) --------
//
// src's _ZN8daKrpa_c13OnYoshiTryEatEv / _ZN8daKpFr_c13OnYoshiTryEatEv (the chomps' slot 18,
// OnYoshiTryEat) and func_ov070_02121438 / func_ov070_02121fb0 (both packs'
// state-0 Kill handlers) carry the "recovered from vtable slot identity"
// marker, so wave 5 kept them out of every port/hal reference and inlined
// the ROM listings here instead. Lane mount-ov070 byte-matched all four
// against extracted/overlays/overlay_0070.bin (match.py, 2004/b56,
// --strict-relocs, --module ov070: MATCH on 0x8, 0x8, 0xc0 and 0x20 bytes)
// and adjudicated them REAL_DECOMP in port/tools/inferred_stub_adjudicated.txt
// -- the w3-d treatment FlyGuy's three own slots got. The four matched TUs
// are on port/slice_w5c.txt and the thunks below call them by name; the
// inline transcriptions are gone. The slot-18 pair is two instructions:
//
//   021211bc  mov r0, #5          02121bdc  mov r0, #5
//   021211c0  bx  lr              02121be0  bx  lr
//
// ---- THE D1 DESTRUCTORS ARE HOST CHAINS (the gate-199/200/202 treatment) --
//
// _ZN7daBrq_cD1Ev.cpp / _ZN8daKrpa_cD1Ev.cpp / _ZN8daKpFr_cD1Ev.cpp are
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
// _ZN7daBrq_c8SetStateEi over data_ov070_02123668; FlyGuy's six [2] arrays at
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
#include "port_d16.h"

#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN8dActor_c25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"
#include "dtor_faces_cpp.h"

#include "dActor_c.h"
#include "fBase_c.h"

extern "C" {
/* the arm9 shared half -- the ov045 exemplar's set, address-verified against
   ov070's own tables (see the header) */
int _ZN8dActor_c19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN8dActor_c18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN8dActor_c14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN8dActor_c12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN8dActor_c13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN8dActor_c13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int _ZN8dActor_c9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN8dActor_c15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN8dActor_c11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN8dActor_c11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN8dActor_c8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN8dActor_c8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN8dActor_c15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN8dActor_c19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN8dActor_c16OnAimedAtWithEggEv(void *self);                     /* slot 29 */

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
int _ZN7daBrq_c13InitResourcesEv(void *self);          /* slot 0  */
int _ZN7daBrq_c16CleanupResourcesEv(void *self);       /* slot 3  */
int _ZN7daBrq_c8BehaviorEv(void *self);                /* slot 6  */
int _ZN7daBrq_c6RenderEv(void *self);                  /* slot 9  */
void _ZN7daBrq_c16OnPendingDestroyEv(void);            /* slot 12 (ROM: bx lr) */
int *_ZN7daBrq_cD0Ev(int *self);                       /* slot 17 */
/* FlameChomp's */
int _ZN8daKrpa_c13InitResourcesEv(void *self);
int _ZN8daKrpa_c16CleanupResourcesEv(void *self);
int _ZN8daKrpa_c8BehaviorEv(void *self);
int _ZN8daKrpa_c6RenderEv(void *self);
void _ZN8daKrpa_c16OnPendingDestroyEv(void);
int *_ZN8daKrpa_cD0Ev(int *self);
/* FlameChompFire's */
int _ZN8daKpFr_c13InitResourcesEv(void *self);
int _ZN8daKpFr_c16CleanupResourcesEv(void *self);
int _ZN8daKpFr_c8BehaviorEv(void *self);
int _ZN8daKpFr_c6RenderEv(void *self);
void _ZN8daKpFr_c16OnPendingDestroyEv(void);
int *_ZN8daKpFr_cD0Ev(int *self);
/* the factories: each stores its OWN host table by real name (no VT1
   placeholder anywhere in this pack), so all three register directly */
void *daBrq_c_classInit(void);
void *daKrpa_c_classInit(void);
void *daKpFr_c_classInit(void);

/* what the D1 chains spell out by hand (the ROM's own call set) */
void _ZN10dBgCh_ActrD1Ev(void *self);
void _ZN10dCcAcPos_cD1Ev(void *self);
void _ZN7dCcAc_cD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN18TextureTransformerD1Ev(void *self);
void _ZN15TextureSequenceD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);
void *_ZN8dActor_cD2Ev(void *self);

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
int _ZN7daBrq_c18EnterCooldownStateEv(void *); int _ZN7daBrq_c16EnterActiveStateEv(void *);
int _ZN7daBrq_c18EnterDefeatedStateEv(void *); int _ZN7daBrq_c19UpdateCooldownStateEv(void *);
int _ZN7daBrq_c19UpdateDefeatedStateEv(void *); int _ZN7daBrq_c17UpdateActiveStateEv(void *);
int func_ov070_02121548(void *); int func_ov070_0212156c(void *);
int func_ov070_021214f8(void *); int func_ov070_021216b8(void *);
int func_ov070_02121710(void *); int func_ov070_021213cc(void *);
int func_ov070_021217ac(void *);
int func_ov070_02121ef8(void *); int func_ov070_02121f18(void *);
int func_ov070_02121eb0(void *);
/* the four bodies lane mount-ov070 adjudicated REAL_DECOMP (byte-matched):
   the two state-0 Kill roots and the two slot-18 OnYoshiTryEat returns */
int func_ov070_02121438(void *);                  /* FlameChomp state 0 */
int func_ov070_02121fb0(void *);                  /* FlameChompFire state 0 */
int _ZN8daKrpa_c13OnYoshiTryEatEv(void);                    /* FlameChomp slot 18 */
int _ZN8daKpFr_c13OnYoshiTryEatEv(void);                    /* FlameChompFire slot 18 */
}

/* The D0 .c TUs restore each table by its RTTI spelling; the recovered names
   daBrq_c_classInit/D1 use are the host arrays themselves. One storage per class. */
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
   IDENTITY_MATRIX4X3 under two more local-type spellings (romdata.c owns the one
   real C symbol; the bbh file already carries its @@3UM48@@A spelling). */
#pragma comment(linker, "/alternatename:?data_ov070_021235ec@@3USharedFilePtr@@A=_data_ov070_021235ec")
#pragma comment(linker, "/alternatename:?data_ov070_021235fc@@3USharedFilePtr@@A=_data_ov070_021235fc")
#pragma comment(linker, "/alternatename:?data_ov070_02123604@@3USharedFilePtr@@A=_data_ov070_02123604")
#pragma comment(linker, "/alternatename:?data_ov070_021222e0@@3PAPAUSharedFilePtr@@A=_data_ov070_021222e0")
#pragma comment(linker, "/alternatename:?data_ov070_0212365c@@3UVector3@@A=_data_ov070_0212365c")
#pragma comment(linker, "/alternatename:?IDENTITY_MATRIX4X3@@3DA=_data_02082128")
#pragma comment(linker, "/alternatename:?IDENTITY_MATRIX4X3@@3UBlk@@A=_data_02082128")
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
#pragma comment(linker, "/alternatename:?_ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj@@YAXPAX0PBUVector3@@HHII@Z=__ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj")

/* Two WithMeshClsn method spellings the pack's C++ TUs want under their own
   declared return types. Same __thiscall bodies, already defined for the
   spellings on the right (the value comes back in EAX either way; the _N
   caller reads AL of the same 0/1). A GivePlayerCoins alias would be
   ABI-wrong (the defined body is a cdecl C function), so that one is a real
   face at the bottom of this file. */
#pragma comment(linker, "/alternatename:?GetFloorResult@dBgCh_Actr@@QBEPAUSurfaceInfo@@XZ=?GetFloorResult@dBgCh_Actr@@QBEHXZ")
#pragma comment(linker, "/alternatename:?IsOnWall@dBgCh_Actr@@QBE_NXZ=?IsOnWall@dBgCh_Actr@@QBEHXZ")

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
OV70_TRAP(13) OV70_TRAP(14)
#undef OV70_TRAP

static int __fastcall ov70_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall ov70_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall ov70_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall ov70_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall ov70_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall ov70_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall ov70_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall ov70_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall ov70_pdes(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); return 0; }
static int __fastcall ov70_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall ov70_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
/* slot 19: the three-parameter shape so it emits `ret 4` -- the dispatch
   site pushes the Player the callee pops (the wf_turn_egg contract). */
static int __fastcall ov70_turn_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov70_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall ov70_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov70_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov70_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov70_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall ov70_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall ov70_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov70_mega(void *s, void *, void *p)
{ _ZN8dActor_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov70_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov70_egg(void *s, void *)
{ return _ZN8dActor_c16OnAimedAtWithEggEv(s); }

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
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the two Kill roots are the matched TUs -------------------------------
// FlameChomp state 0 is func_ov070_02121438 (ROM 0x02121438, 0xc0 bytes) and
// FlameChompFire state 0 is func_ov070_02121fb0 (ROM 0x02121fb0, 0x20 bytes);
// both byte-match the ROM under the pin and are seated directly below. The
// per-instruction transcriptions that stood here while the marker kept the
// TUs out of port/hal were retired by lane mount-ov070.

// ---- the state PMF seat ----------------------------------------------------
// ---- FLY_GUY'S SIX TICK HALVES NEED A __fastcall THUNK (lane w3-d) ---------
//
// THE SPLIT IS PER SITE, NOT PER CLASS, and FlyGuy is the first class in this
// overlay where the two halves of a cell are reached by DIFFERENT calling
// conventions. Measured with dumpbin over this build's own objects, not
// reasoned about:
//
//   FlyGuy_ChangeState.cpp.obj -- the ENTER half, pp[0]
//     mov eax,[ebp+0Ch] / mov ecx,[ebp+8] / mov [ecx+3BCh],eax
//     mov eax,[eax] / test eax,eax / ... / pop ebp / JMP EAX
//   It is a TAIL JUMP. The caller's own cdecl frame survives, so `c` is still
//   at [esp+4] when the raw body reads it, and ecx happens to hold `c` too.
//   A raw cdecl body is correct here and stays.
//
//   _ZN19daPropeller_Heyho_c8BehaviorEv.cpp.obj -- the TICK half, pp[1]
//     mov ecx,[esi+3BCh] / mov eax,[ecx+8] / mov ecx,[ecx+0Ch]
//     add ecx,esi / CALL EAX
//   It is a REAL CALL inside Behavior's own frame, because Behavior does more
//   work afterwards and cannot be a forwarder. The receiver goes in ECX and
//   NOTHING IS PUSHED. A raw cdecl body reads its `c` from [esp+4], which here
//   is Behavior's own spilled data -- a wrong pointer it then writes through
//   (func_ov070_0211fa80's first statement is `*(int *)(c + 0x3dc) = 0`).
//   Silent memory corruption, not a fault, which is why 300 frames of level 19
//   came back rc 0 with it live.
//
// This is NOT the Amp/FlameChomp case and their rows are deliberately
// untouched: _ZN7daBrq_c11UpdateStateEv and func_ov070_0212180c are one-call
// forwarders that MSVC compiles as `jmp eax` (verified in the same dumpbin
// pass), so their tick bodies get the caller's frame and must stay raw cdecl.
// The R9 sweep in port/unmatched/Ov070_PmfDispatch.cpp enumerated the
// `c->pp + 1` shape and FlyGuy::Behavior is not in it -- its shape is a
// `Holder { char pad[8]; PMF fn; }` read at +8, which that grep could not see.
//
// The enter and tick sets are DISJOINT (checked body by body against
// __sinit_ov070_02122afc's own cell assignments), so no body needs both
// conventions and a thunk on the tick side cannot disturb the enter side:
//   cell 0x0212358c  enter 0211f5f0   tick 0211f48c
//   cell 0x0212359c  enter 0211ffa8   tick 0211fd98
//   cell 0x021235ac  enter 0211fd60   tick 0211fae4
//   cell 0x021235bc  enter 0211f450   tick 0211f368
//   cell 0x021235cc  enter 0211fa80   tick 0211f6e0
//   cell 0x021235dc  enter 0211f694   tick 0211f62c
//
// One register argument, so MSVC reads the receiver from ecx and returns with
// a bare `ret` -- which is what the call site wants, having pushed nothing.
static int __fastcall fg_tick_f48c(void *s) { return func_ov070_0211f48c(s); }
static int __fastcall fg_tick_fd98(void *s) { return func_ov070_0211fd98(s); }
static int __fastcall fg_tick_fae4(void *s) { return func_ov070_0211fae4(s); }
static int __fastcall fg_tick_f368(void *s) { return func_ov070_0211f368(s); }
static int __fastcall fg_tick_f6e0(void *s) { return func_ov070_0211f6e0(s); }
static int __fastcall fg_tick_f62c(void *s) { return func_ov070_0211f62c(s); }

// ---- AND FLYGUY'S SIX ENTER BODIES NEED IT TOO -----------------------------
//
// The note above rules the ENTER half correct because FlyGuy_ChangeState is a
// one-call forwarder MSVC compiles as a tail jump, so the caller's own cdecl
// argument is still at [esp+4] when a raw body reads it. That is true of
// FlyGuy_ChangeState AS A SEPARATE FRAME and it is not true of the frame the
// spawn path actually runs in. FlyGuy_ChangeState and
// daPropeller_Heyho_c::InitResources are the same translation unit
// (src/game/actors/d_a_propeller_heyho.cpp), so /O2 inlines the forwarder into
// InitResources' last statement and the pointer-to-member call comes out in
// InitResources' own frame. Read off this build's own walk_window.exe rather
// than reasoned about:
//
//   ?InitResources@daPropeller_Heyho_c@@UAEHXZ +0x105
//     mov  dword ptr [esi+3BCh],offset _data_ov070_0212359c
//     mov  edx,dword ptr [_data_ov070_0212359c]        ; the cell's enter fn
//     test edx,edx
//     je   +0x11A
//     mov  ecx,dword ptr [_data_ov070_0212359c+4]      ; the cell's delta
//     lea  ecx,[ecx+esi]                               ; this + delta
//     call edx                                         ; a REAL CALL
//
// Nothing is pushed. func_ov070_0211ffa8 is a raw cdecl body declared
// `(daPropeller_Heyho_c *c)`, so it read InitResources' own spilled stack as
// its `c' -- the constant 1 on this build -- and its first statement writes
// through it at +0x3e6. Run link100's boot sweep saw that as levels 16, 22 and
// 24 faulting at func_ov070_0211ffa8+0x1c on the address 0x3e7.
//
// The thunk is right on BOTH paths, which is why it goes on all six enter rows
// and not on the one that was caught: FlyGuy_ChangeState's own out-of-line body
// ends `mov ecx,[ecx+4] / add ecx,eax / pop ebp / jmp edx`, so the receiver is
// in ECX on the tail-jump path too, and reading it from the stack is right only
// while that path is the one taken. Cells and their enter records are read out
// of __sinit_ov070_02122afc, not from a comment:
//   cell 0212358c[0] <- 02123110   cell 0212359c[0] <- 021230e8
//   cell 021235ac[0] <- 021230d8   cell 021235bc[0] <- 021230f8
//   cell 021235cc[0] <- 021230c0   cell 021235dc[0] <- 02123108
static int __fastcall fg_enter_fa80(void *s) { return func_ov070_0211fa80(s); }
static int __fastcall fg_enter_fd60(void *s) { return func_ov070_0211fd60(s); }
static int __fastcall fg_enter_ffa8(void *s) { return func_ov070_0211ffa8(s); }
static int __fastcall fg_enter_f450(void *s) { return func_ov070_0211f450(s); }
static int __fastcall fg_enter_f694(void *s) { return func_ov070_0211f694(s); }
static int __fastcall fg_enter_f5f0(void *s) { return func_ov070_0211f5f0(s); }

// ---- FLAMECHOMP'S AND FLAMECHOMPFIRE'S STATE BODIES NEED THE SAME THUNK ----
//
// The note above rules FlameChomp's rows correct because its dispatchers are
// one-call forwarders MSVC compiles as `jmp eax`, which leaves the caller's own
// cdecl frame in place so a raw body still finds its receiver at [esp+4]. That
// is true of the dispatchers AS SEPARATE FRAMES and it is not true of the frame
// the enter path actually runs in. func_ov070_02121880 and func_ov070_02121848
// live in the same translation unit as daKrpa_c::InitResources, so /O2 inlines
// both of them into it, and the PMF call comes out inside InitResources' own
// frame. Read off this build's own object with dumpbin, not reasoned about:
//
//   ?InitResources@daKrpa_c@@UAEHXZ +0xD1
//     mov ecx,dword ptr [_data_ov070_021236ac+4]   ; the record's delta
//     lea ecx,[ecx+esi]                            ; this + delta
//     call dword ptr [_data_ov070_021236ac]        ; a REAL CALL
//
// Nothing is pushed. func_ov070_021217ac is a raw cdecl body declared
// `(daKrpa_c *self)`, so it reads InitResources' own spilled stack as its
// receiver and hands that to ModelAnim::SetAnim: run link100's boot sweep saw
// it as four levels faulting a few bytes into SetAnim on a garbage pointer
// (21, 25, 26 and 27, each with a different junk address, which is what an
// uninitialised stack slot looks like).
//
// A __fastcall thunk is correct on BOTH paths and that is why it goes on every
// row rather than on the one site that was caught. MSVC's PMF call sequence
// always puts `this + delta` in ECX before it transfers control, whether it
// transfers with a call or with a tail jump, so reading the receiver from ECX
// is right either way; reading it from the stack is right only when the
// transfer happened to be a tail jump. One register argument, so the thunk
// returns with a bare `ret`, which is what a call site that pushed nothing
// wants.
static int __fastcall fc_st_1438(void *s) { return func_ov070_02121438(s); }
static int __fastcall fc_st_14f8(void *s) { return func_ov070_021214f8(s); }
static int __fastcall fc_st_1548(void *s) { return func_ov070_02121548(s); }
static int __fastcall fc_st_156c(void *s) { return func_ov070_0212156c(s); }
static int __fastcall fc_st_16b8(void *s) { return func_ov070_021216b8(s); }
static int __fastcall fc_st_1710(void *s) { return func_ov070_02121710(s); }
static int __fastcall fc_st_13cc(void *s) { return func_ov070_021213cc(s); }
static int __fastcall fc_st_17ac(void *s) { return func_ov070_021217ac(s); }
static int __fastcall ff_st_1fb0(void *s) { return func_ov070_02121fb0(s); }
static int __fastcall ff_st_1ef8(void *s) { return func_ov070_02121ef8(s); }
static int __fastcall ff_st_1f18(void *s) { return func_ov070_02121f18(s); }
static int __fastcall ff_st_1eb0(void *s) { return func_ov070_02121eb0(s); }

// ---- THE AMP'S SIX STATE BODIES NEED THE SAME THUNK ------------------------
//
// Same defect as FlameChomp's above, one class over, and the note there is the
// whole argument: MSVC's pointer-to-member call sequence always puts
// `this + delta` in ECX and pushes NOTHING, so a raw cdecl body seated in a
// cell reads the caller's own stack as its receiver.
//
// daBrq_c's dispatchers are compiled /vmg /vmm (port/CMakeLists.txt R8), and
// _ZN7daBrq_c8SetStateEi / _ZN7daBrq_c10EnterStateEv are small enough that /O2
// inlines them into their callers, so the PMF call comes out inside the
// caller's frame exactly as daKrpa_c::InitResources did. Read off this build's
// own image, not reasoned about:
//
//   ?UpdateCooldownState@daBrq_c@@AAEHXZ +0x21   (the inlined SetState(1))
//     mov  dword ptr [esi+41Ch], offset <table>+10h   ; mStateHandlers
//     mov  ecx,dword ptr [<table>+14h]                ; the record's delta = 0
//     lea  ecx,[ecx+esi]                              ; this + delta
//     call dword ptr [<table>+10h]                    ; a REAL CALL
//
// while every one of the six cells held a face whose whole body is
// `push ebp / mov ebp,esp / mov ecx,[ebp+8] / pop ebp / jmp <member>`: the
// receiver taken from the first STACK argument the call site never pushed.
// Measured as the junk word left in that slot, which on a snow course is the
// small integer 1, so EnterActiveState formed &mModelAnim as 1 + 0xd4 and
// ModelAnim::SetAnim faulted 0xd bytes in reading the BCA slot at 0x135 --
// the "a few bytes into SetAnim on a garbage pointer" shape the note above
// names, this time with an address low enough to fault every run.
//
// On all six rows and not just the one that was caught, for the reason given
// there: the thunk is right whether the transfer is a call or a tail jump.
// Cells and their records are __sinit_ov070_02122d80's, not a comment's:
//   record 0212320c -> cell[0].enter   record 02123224 -> cell[0].update
//   record 02123214 -> cell[1].enter   record 02123234 -> cell[1].update
//   record 0212321c -> cell[2].enter   record 0212322c -> cell[2].update
static int __fastcall brq_st_enter_cool(void *s)
{ return _ZN7daBrq_c18EnterCooldownStateEv(s); }
static int __fastcall brq_st_enter_active(void *s)
{ return _ZN7daBrq_c16EnterActiveStateEv(s); }
static int __fastcall brq_st_enter_dead(void *s)
{ return _ZN7daBrq_c18EnterDefeatedStateEv(s); }
static int __fastcall brq_st_tick_cool(void *s)
{ return _ZN7daBrq_c19UpdateCooldownStateEv(s); }
static int __fastcall brq_st_tick_dead(void *s)
{ return _ZN7daBrq_c19UpdateDefeatedStateEv(s); }
static int __fastcall brq_st_tick_active(void *s)
{ return _ZN7daBrq_c17UpdateActiveStateEv(s); }

static void ov70_seat_state_pmfs(void)
{
    struct Row { unsigned char *rec; void *fn; };
    static const Row rows[] = {
        /* FlyGuy's twelve, source order 0x021230c0.. */
        { data_ov070_021230c0, (void *)fg_enter_fa80 }  /* ENTER: cell 021235cc[0] */,
        { data_ov070_021230c8, (void *)fg_tick_fd98 }   /* TICK: cell 0212359c[1] */,
        { data_ov070_021230d0, (void *)fg_tick_f62c }   /* TICK: cell 021235dc[1] */,
        { data_ov070_021230d8, (void *)fg_enter_fd60 }  /* ENTER: cell 021235ac[0] */,
        { data_ov070_021230e0, (void *)fg_tick_f48c }   /* TICK: cell 0212358c[1] */,
        { data_ov070_021230e8, (void *)fg_enter_ffa8 }  /* ENTER: cell 0212359c[0] */,
        { data_ov070_021230f0, (void *)fg_tick_fae4 }   /* TICK: cell 021235ac[1] */,
        { data_ov070_021230f8, (void *)fg_enter_f450 }  /* ENTER: cell 021235bc[0] */,
        { data_ov070_02123100, (void *)fg_tick_f368 }   /* TICK: cell 021235bc[1] */,
        { data_ov070_02123108, (void *)fg_enter_f694 }  /* ENTER: cell 021235dc[0] */,
        { data_ov070_02123110, (void *)fg_enter_f5f0 }  /* ENTER: cell 0212358c[0] */,
        { data_ov070_02123118, (void *)fg_tick_f6e0 }   /* TICK: cell 021235cc[1] */,
        /* Amp's six */
        { data_ov070_0212320c, (void *)brq_st_enter_cool }   /* ENTER: cell[0] */,
        { data_ov070_02123214, (void *)brq_st_enter_active } /* ENTER: cell[1] */,
        { data_ov070_0212321c, (void *)brq_st_enter_dead }   /* ENTER: cell[2] */,
        { data_ov070_02123224, (void *)brq_st_tick_cool }    /* TICK:  cell[0] */,
        { data_ov070_0212322c, (void *)brq_st_tick_dead }    /* TICK:  cell[2] */,
        { data_ov070_02123234, (void *)brq_st_tick_active }  /* TICK:  cell[1] */,
        /* FlameChomp's eight; state 0 is the matched Kill root */
        { data_ov070_021232f4, (void *)fc_st_1438 },
        { data_ov070_021232fc, (void *)fc_st_1548 },
        { data_ov070_02123304, (void *)fc_st_156c },
        { data_ov070_0212330c, (void *)fc_st_14f8 },
        { data_ov070_02123314, (void *)fc_st_16b8 },
        { data_ov070_0212331c, (void *)fc_st_1710 },
        { data_ov070_02123324, (void *)fc_st_13cc },
        { data_ov070_0212332c, (void *)fc_st_17ac },
        /* FlameChompFire's four; state 0 likewise */
        { data_ov070_021233ec, (void *)ff_st_1fb0 },
        { data_ov070_021233f4, (void *)ff_st_1ef8 },
        { data_ov070_021233fc, (void *)ff_st_1f18 },
        { data_ov070_02123404, (void *)ff_st_1eb0 },
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
{ return _ZN7daBrq_c13InitResourcesEv(s); }
static int __fastcall amp_clean(void *s, void *)
{ return _ZN7daBrq_c16CleanupResourcesEv(s); }
static int __fastcall amp_behavior(void *s, void *)
{ return _ZN7daBrq_c8BehaviorEv(s); }
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
{ (void)s; _ZN7daBrq_c16OnPendingDestroyEv(); return 0; }
/* slot 16, HOST CHAIN -- the ROM listing at 0x02120570: store the own table,
   then WithMeshClsn +0x218, MovingCylinderClsnWithPos +0x1d8, ShadowModel
   +0x1b0, TextureTransformer +0x19c, TextureSequence +0x188, Model +0x138,
   ModelAnim +0xd4, Actor::D2. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (amp_d1) spelled the same chain by hand. */
static int __fastcall amp_d0(void *s, void *)
{ return (int)(size_t)_ZN7daBrq_cD0Ev((int *)s); }
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
    vt[16] = (void *)PORT_D16(hal_cppd1_Amp);
    vt[17] = (void *)amp_d0;
}

// ============================================================================
// FLAME_CHOMP (id 270) -- table 0x02123370, 944-byte object
// ============================================================================
//
// ModelAnim +0xd4, ShadowModel +0x138, MovingCylinderClsnWithPos +0x160,
// WithMeshClsn +0x1a0. Slot 18 overrides OnYoshiTryEat.
static int __fastcall fc_init(void *s, void *)
{ return _ZN8daKrpa_c13InitResourcesEv(s); }
static int __fastcall fc_clean(void *s, void *)
{ return _ZN8daKrpa_c16CleanupResourcesEv(s); }
static int __fastcall fc_behavior(void *s, void *)
{ return _ZN8daKrpa_c8BehaviorEv(s); }
static int __fastcall fc_render(void *s, void *)
{ port_actor_render_probe("FLAME_CHOMP", (char *)s + 0xd4);
  return port_ov070_flamechomp_render(s); }
static int __fastcall fc_pdes(void *s, void *)
{ (void)s; _ZN8daKrpa_c16OnPendingDestroyEv(); return 0; }
/* slot 18: the ROM body is `mov r0, #5; bx lr` (0x021211bc, quoted in the
   header) -- "I am fire, spit me out". The matched TU, adjudicated. */
static int __fastcall fc_yoshi(void *, void *)
{ return _ZN8daKrpa_c13OnYoshiTryEatEv(); }
/* slot 16, HOST CHAIN -- the listing at 0x02121118. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (fc_d1) spelled the same chain by hand. */
static int __fastcall fc_d0(void *s, void *)
{ return (int)(size_t)_ZN8daKrpa_cD0Ev((int *)s); }
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
    vt[16] = (void *)PORT_D16(hal_cppd1_FlameChomp);
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
// OnYoshiTryEat as the chomp's (0x02121bdc), the matched TU.
static int __fastcall fcf_init(void *s, void *)
{ return _ZN8daKpFr_c13InitResourcesEv(s); }
static int __fastcall fcf_clean(void *s, void *)
{ return _ZN8daKpFr_c16CleanupResourcesEv(s); }
static int __fastcall fcf_behavior(void *s, void *)
{ return _ZN8daKpFr_c8BehaviorEv(s); }
static int __fastcall fcf_render(void *s, void *)
{ return _ZN8daKpFr_c6RenderEv(s); }
static int __fastcall fcf_pdes(void *s, void *)
{ (void)s; _ZN8daKpFr_c16OnPendingDestroyEv(); return 0; }
static int __fastcall fcf_yoshi(void *, void *)
{ return _ZN8daKpFr_c13OnYoshiTryEatEv(); }
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (fcf_d1) spelled the same chain by hand. */
static int __fastcall fcf_d0(void *s, void *)
{ return (int)(size_t)_ZN8daKpFr_cD0Ev((int *)s); }
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
    vt[16] = (void *)PORT_D16(hal_cppd1_FlameChompFire);
    vt[17] = (void *)fcf_d0;
    vt[18] = (void *)fcf_yoshi;
}

// ============================================================================
// POPPING_LAVA_BUBBLES (id 196, ov002) -- table 0x021093e0, 216-byte object
// ============================================================================
//
// The census's second bonus find, level 37's one ov002-resident skip (also on
// level 15's list). NOT the class at _ZTV16daObjWaterfall_c -- that table is
// WATERFALL_MIST's (197), hal/actor_classes.cpp's own header derives the ov002
// shift. Id 196's record at 0x021093bc (word[0] = 0x020b6dd8 =
// daObjLava_c_classInit, word[1] low half = 196, raw bytes out of
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
// THREE OWN BODIES WERE INLINE TRANSCRIPTIONS, and GATE 227 REPLACES ALL THREE
// WITH THE ROM'S OWN. src's _ZN11daObjLava_c13InitResourcesEv / _020b6d84 / _020b6d4c carry
// the inferred-stub marker, so port/tools/inferred_stub_guard.py refused them
// and this file re-typed the listings by hand instead. Lane STUBADJ ran the
// whole unlinked marker set through tools/match.py at each symbol's config
// address and size under the pinned mwccarm 2004/b56 with --strict-relocs and
// ruled these three REAL DECOMP, so the hand copies are gone and the faces
// below call the real bodies. Listings, kept because they are what the seat is
// checked against:
//   InitResources 0x020b6dd0 (0x8):  mov r0, #1; bx lr
//   Behavior 0x020b6d84 (0x48): r4=self; ClosestPlayer(); r3=r0+0x5c;
//     func_02022c3c([r4+0xd4], 0xb7, [r3], [r3+4], [r3+8], 0); result back
//     into [r4+0xd4]; return 1.   (effect 0xb7 tracked at the player's pos)
//   D0 0x020b6d4c (0x38): store 0x021093e0; Actor::D2; Memory::Deallocate
//     (self, *data_020a0eac); return self.
// VERIFIED BY ADDRESS before the seat: the table words at 0x021093e0 + 4*0,
// + 4*6 and + 4*17 relocate to 0x020b6dd0, 0x020b6d84 and 0x020b6d4c in
// config/arm9/overlays/ov002/relocs.txt, and each of those addresses has its
// own kind:function(arm,size=..) record in that overlay's symbols.txt, so none
// is an interior address of a tail-shared body. The D0's two placeholder names
// are bound per-TU in port/CMakeLists.txt from its own literal pool
// (0x020b6d7c -> 0x021093e0, 0x020b6d80 -> 0x020a0eac). All three take nothing
// past the receiver, so all three faces keep the two-parameter shape.
// The D1 (_ZN11daObjLava_cD1Ev) carries no marker and links from the slice.
extern "C" {
int _ZN11daObjLava_cD1Ev(int *self);            /* slot 16, matched */
unsigned func_02022c3c(unsigned tracker, unsigned effect, int x, int y,
                       int z, const void *dir);
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
void _ZN6Memory10DeallocateEPvP4Heap(void *p, void *heap);
void *daObjLava_c_classInit(void);
extern void *data_020a0eac;                    /* the game heap word */
DSSTATE_BEGIN
void *_ZTV11daObjLava_c[31];
DSSTATE_END
}
/* gate 227: the three ROM bodies that used to be hand copies here. */
extern "C" {
int  _ZN11daObjLava_c13InitResourcesEv(void);       /* slot 0  InitResources */
int  _ZN11daObjLava_c8BehaviorEv(void *self); /* slot 6  Behavior      */
int *_ZN11daObjLava_cD0Ev(int *t);     /* slot 17 the D0        */
}
static int __fastcall plb_init(void *, void *)
{ return _ZN11daObjLava_c13InitResourcesEv(); }
static int __fastcall plb_clean(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::CleanupResources(); }
static int __fastcall plb_behavior(void *s, void *)
{ return _ZN11daObjLava_c8BehaviorEv(s); }
static int __fastcall plb_render(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::Render(); }
static int __fastcall plb_d1(void *s, void *)
{ return (int)(size_t)_ZN11daObjLava_cD1Ev((int *)s); }
static int __fastcall plb_d0(void *s, void *)
{ return (int)(size_t)_ZN11daObjLava_cD0Ev((int *)s); }
extern "C" void hal_fill_popping_lava_bubbles_vtable(void)
{
    /* ov002 is always mounted; no bring-up needed here. */
    void *volatile *vt = (void *volatile *)_ZTV11daObjLava_c;
    ov70_fill_shared(vt);
    vt[0]  = (void *)plb_init;
    vt[3]  = (void *)plb_clean;
    vt[6]  = (void *)plb_behavior;
    vt[9]  = (void *)plb_render;
    /* 12 stays ov70_pdes = ActorBase::OnPendingDestroy, the ROM's own word */
    vt[16] = (void *)PORT_D16(plb_d1);
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
#include "daBrq_c.h"
#include "daPropeller_Heyho_c.h"
#include "daKrpa_c.h"
#include "daKpFr_c.h"
#include "Player.h"
extern "C" {
int _ZN7daBrq_c13InitResourcesEv(void *self)
{ return ((daBrq_c *)self)->daBrq_c::InitResources(); }
int _ZN7daBrq_c8BehaviorEv(void *self)
{ return ((daBrq_c *)self)->daBrq_c::Behavior(); }
int _ZN7daBrq_c6RenderEv(void *self)
{ return ((daBrq_c *)self)->daBrq_c::Render(); }
int _ZN8daKrpa_c13InitResourcesEv(void *self)
{ return ((daKrpa_c *)self)->daKrpa_c::InitResources(); }
int _ZN8daKrpa_c8BehaviorEv(void *self)
{ return ((daKrpa_c *)self)->daKrpa_c::Behavior(); }
int _ZN8daKrpa_c6RenderEv(void *self)
{ return ((daKrpa_c *)self)->daKrpa_c::Render(); }
int _ZN8daKpFr_c13InitResourcesEv(void *self)
{ return ((daKpFr_c *)self)->daKpFr_c::InitResources(); }
int _ZN8daKpFr_c8BehaviorEv(void *self)
{ return ((daKpFr_c *)self)->daKpFr_c::Behavior(); }
int _ZN8daKpFr_c6RenderEv(void *self)
{ return ((daKpFr_c *)self)->daKpFr_c::Render(); }
int _ZN6Player5ShockEj(void *self, unsigned j)
{ return ((Player *)self)->Player::Shock(j); }
void _ZN8dActor_c15GivePlayerCoinsER6Playerhj(void *self, void *player,
                                           unsigned char count, unsigned kind);
}
/* RETIRED at ALIAS2 (wave 8, the main -> port sync): src/_ZN8dActor_c15GivePlayerCoinsER6Playerhj.cpp is a real dActor_c member since main langmode migration and emits ?GivePlayerCoins@dActor_c@@QAEXAAUPlayer@@EI@Z itself, so this face was the second definition (LNK2005).
   The body is kept below under #if 0 rather than deleted, so the
   evidence in it stays readable. */
#if 0
void dActor_c::GivePlayerCoins(Player &player, u8 count, u32 coinKind)
{ _ZN8dActor_c15GivePlayerCoinsER6Playerhj(this, &player, count, coinKind); }
#endif

// ============================================================================
// FLY_GUY (id 232) -- table 0x02123168, ov070's fourth class (lane w3-d)
// ============================================================================
//
// This file's own header says FlyGuy "is the one left unregistered: no
// bootable level spawns it ... so a future FlyGuy lane is a row plus a fill,
// nothing else." The first half of that has stopped being true and the second
// half was right. Level 19 (Snowman's Land) places FlyGuy and level 19 is
// mounted: the baseline census on cons cc10acd4e reads "[spawn] actor 0xe8
// not registered, skipped", and 0xe8 is 232.
//
// The prediction held. _ZTV6FlyGuy was already excluded from
// port/ov070_syms.txt as host storage, and ov70_seat_state_pmfs already
// rewrites all TWELVE of FlyGuy's PMF source records with host addresses
// alongside the other eighteen -- this file's header says so explicitly ("All
// 30 are rewritten, FlyGuy's included"). One thing the note did not cover:
// g_profile_PROPELLER_HEYHO was not mounted either, and the registry row needs it. It
// is mounted now, in the three siblings' own shape.
//
// MORE OWN SLOTS THAN ITS SIBLINGS. Read out of
// extracted/overlays/overlay_0070.bin at 0x02123168..0x021231e4, 31 words:
//
//   slot  FlyGuy 0x02123168      what the ROM names
//    0    0x021203b4 (own)        InitResources
//    3    0x02120150 (own)        CleanupResources
//    6    0x02120210 (own)        Behavior
//    9    0x021201c0 (own)        Render          <- HOST COPY, see below
//   12    0x021201bc (own)        OnPendingDestroy
//   16    0x0211f000 (own)        D1
//   17    0x0211f048 (own)        D0
//   18    0x02120518 (own)        OnYoshiTryEat
//   19    0x021204ec (own)        OnTurnIntoEgg(Player&)
//   29    0x021204e4 (own)        OnAimedAtWithEgg
//
// Every other slot is byte-identical to the three tables ov70_fill_shared
// already serves, so it is reused and the ten above are written over it.
// 18/19/29 matter: ov70_fill_shared seats the SHARED Actor defaults there and
// FlyGuy overrides all three, which Amp, FlameChomp and FlameChompFire do not.
//
// SLOT 9 IS A HOST COPY, and unlike the ov072 pair this one really is the
// collision rather than a false alarm on a plain Model.
// src/game/actors/d_a_propeller_heyho.cpp takes a LOCAL six-virtual ROM-order shadow off
// &mModelAnim and calls its slot 5, and the receiver is genuinely a ModelAnim:
// both destructors call _ZN9ModelAnimD1Ev on this+0x300 and FlyGuy.h puts
// mModelAnim there. _ZTV9ModelAnim cannot be dual-filled (Virtual18 owns
// MSVC's slot 5), so this is the Butterfly/Whomp/BabyPenguin case exactly --
// the same one Amp's and FlameChomp's Renders above already took. The body is
// at the end of port/unmatched/ModelAnim_Renders.cpp and defines the C name
// directly, so slot 9 calls it without a face.
//
// SLOT 19 TAKES THE PLAYER. _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player is declared
// (char *c, void *player) and the caller pushes the player, so the thunk needs
// the third parameter to pop it -- the bp_egg lesson from
// hal/actor_classes_ov072.cpp, where a thunk that pushed nothing left the
// callee reading the thunk's own return address as its argument. Slots 18 and
// 29 take nothing of their own: both .c bodies are declared (void).
//
// FLYGUY DERIVES FROM ENEMY, NOT STRAIGHT FROM ACTOR. Both destructors end in
// _ZN12dEnemyBase_cD2Ev rather than _ZN8dActor_cD2Ev. That is ov002 code the port
// already links, both destructors are plain .c bodies that name it themselves,
// and nothing here has to bridge it -- which is also why neither destructor
// needs the host chain SnowmanBody's and SnowmanHead's got: there are no local
// shadow classes in either file for MSVC to emit undefined member destructors
// against.
// ============================================================================
extern "C" {
int _ZN19daPropeller_Heyho_c13InitResourcesEv(void *self);      /* slot 0, faced below */
int _ZN19daPropeller_Heyho_c16CleanupResourcesEv(void);         /* slot 3, .c body takes void */
int _ZN19daPropeller_Heyho_c8BehaviorEv(void *self);            /* slot 6, faced below */
int _ZN19daPropeller_Heyho_c6RenderEv(void *self);              /* slot 9, HOST COPY (ModelAnim_Renders.cpp) */
void _ZN19daPropeller_Heyho_c16OnPendingDestroyEv(void);        /* slot 12, own empty body */
int *_ZN19daPropeller_Heyho_cD1Ev(int *t);                      /* slot 16, plain .c, names its own table */
int *_ZN19daPropeller_Heyho_cD0Ev(int *t);                      /* slot 17, plain .c */
int _ZN19daPropeller_Heyho_c13OnYoshiTryEatEv(void);                    /* slot 18, own OnYoshiTryEat */
void _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player(char *c, void *player);  /* slot 19, own OnTurnIntoEgg(Player&) */
int _ZN19daPropeller_Heyho_c16OnAimedAtWithEggEv(void);                    /* slot 29, own OnAimedAtWithEgg */
void *daPropeller_Heyho_c_classInit(void);                         /* installs _ZTV6FlyGuy */
extern unsigned char g_profile_PROPELLER_HEYHO[];          /* ov070 0x02123144 */
int _ZTV6FlyGuy[31];   /* vtspan: 0x02123168, 31 words to 0x021231e4 */
}

static int __fastcall fg_init(void *s, void *)
{ return _ZN19daPropeller_Heyho_c13InitResourcesEv(s); }
static int __fastcall fg_clean(void *s, void *)
{ (void)s; return _ZN19daPropeller_Heyho_c16CleanupResourcesEv(); }
static int __fastcall fg_behavior(void *s, void *)
{ return _ZN19daPropeller_Heyho_c8BehaviorEv(s); }
static int __fastcall fg_render(void *s, void *)
{ port_actor_render_probe("FLY_GUY", (char *)s + 0x300);
  return _ZN19daPropeller_Heyho_c6RenderEv(s); }
static int __fastcall fg_pdes(void *s, void *)
{ (void)s; _ZN19daPropeller_Heyho_c16OnPendingDestroyEv(); return 0; }
static int __fastcall fg_d1(void *s, void *)
{ return (int)(size_t)_ZN19daPropeller_Heyho_cD1Ev((int *)s); }
static int __fastcall fg_d0(void *s, void *)
{ return (int)(size_t)_ZN19daPropeller_Heyho_cD0Ev((int *)s); }
static int __fastcall fg_yoshi(void *s, void *)
{ (void)s; return _ZN19daPropeller_Heyho_c13OnYoshiTryEatEv(); }
/* Third parameter on purpose: the caller of slot 19 pushes the Player and this
   thunk must pop it. The ov072 bp_egg note has the failure this prevents. */
static int __fastcall fg_turn_egg(void *s, void *, void *player)
{ _ZN19daPropeller_Heyho_c13OnTurnIntoEggER6Player((char *)s, player); return 0; }
static int __fastcall fg_egg(void *s, void *)
{ (void)s; return _ZN19daPropeller_Heyho_c16OnAimedAtWithEggEv(); }

extern "C" void hal_fill_fly_guy_vtable(void)
{
    port_ov70_bringup();
    void *volatile *vt = (void *volatile *)_ZTV6FlyGuy;
    ov70_fill_shared(vt);
    vt[0]  = (void *)fg_init;
    vt[3]  = (void *)fg_clean;
    vt[6]  = (void *)fg_behavior;
    vt[9]  = (void *)fg_render;
    vt[12] = (void *)fg_pdes;
    vt[16] = (void *)PORT_D16(fg_d1);
    vt[17] = (void *)fg_d0;
    /* the three ov70_fill_shared seats a default into and FlyGuy overrides */
    vt[18] = (void *)fg_yoshi;
    vt[19] = (void *)fg_turn_egg;
    vt[29] = (void *)fg_egg;
}

// ---- method faces ----------------------------------------------------------
// InitResources and Behavior are real MSVC methods against FlyGuy.h, so each
// needs the C-named face the ROM's own table uses. CleanupResources and
// OnPendingDestroy are plain C-linkage .c bodies taking (void), declared
// extern "C" above and called directly. Render is NOT faced here: its body is
// the host copy in port/unmatched/ModelAnim_Renders.cpp, which defines the C
// name itself.
extern "C" {
int _ZN19daPropeller_Heyho_c13InitResourcesEv(void *self)
{ return ((daPropeller_Heyho_c *)self)->daPropeller_Heyho_c::InitResources(); }
int _ZN19daPropeller_Heyho_c8BehaviorEv(void *self)
{ return ((daPropeller_Heyho_c *)self)->daPropeller_Heyho_c::Behavior(); }
}
