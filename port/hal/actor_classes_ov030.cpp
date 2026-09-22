// ============================================================================
// ov030 -- TALL TALL MOUNTAIN's own cast. Run rel0215 wave 1, lane cast-ov030.
//
// FOUR SpawnInfo RECORDS, THREE CLASSES, THREE VTABLES:
//
//   id   class          ROM type_info name    vtable      slots  size  base
//   102  RollingLogTtm  15daObjHmMaruta_c     0x02115a48   32     836  Platform
//   103  UkikiCage      13daObjHmBskt_c       0x02115974   32    1248  Platform
//   267  UkikiThief     7daMky_c              0x02115bfc   31     972  Actor
//   268  UkikiStar      7daMky_c              0x02115bfc   31     972  Actor
//
// 267 and 268 are ONE class under two spawn entry points: both factories store
// the same table (relocs from:0x02114634 and from:0x0211468c both to:0x02115bfc),
// both allocate 972, both run the same five sub-object constructors. So this
// file fills THREE tables and registers FOUR ids.
//
// ============================================================================
// THE CONFIG'S CLASS LABELS ARE SHIFTED ONE CLASS -- EVERYTHING HERE IS SEATED
// BY ADDRESS
// ============================================================================
// This is the third instance of the "dsd class-identity swap/gap" the tree
// already names twice (port/CMakeLists.txt:1589-1600 for ov072's gap and
// :1618-1627 for ov012's swap), and it is the most dangerous thing in this
// overlay because filling a table by its config NAME fills a DIFFERENT class's
// table and fails silently at run time.
//
// The ROM's answer is each table's vtable[-1] -> __si_class_type_info record ->
// word[1] name pointer -> the NUL-terminated string, the route
// port/ov026_syms.txt's wave-C section settled ov026's identity with:
//
//   table       vtable[-1]  name string          really is
//   0x02115974  0x02115934  "13daObjHmBskt_c"    UkikiCage     (basket/cage)
//   0x02115a48  0x021159f8  "15daObjHmMaruta_c"  RollingLogTtm (maruta = log)
//   0x02115bfc  0x02115b84  "7daMky_c"           the Ukiki     (mky = monkey)
//
// and it agrees with each factory's own last literal-pool store, and with the
// base each factory constructs (Platform for the first two, plain Actor for the
// third -- which is exactly why the first two carry Platform::Kill at slot 31
// and the third stops at 30).
//
// WHAT CONFIG SAYS INSTEAD: _ZTV13RollingLogTtm is on 0x02115a48 (RollingLogTtm's
// table), _ZTV13RollingLogTtm and _ZTV7daMky_c are BOTH on 0x02115bfc (only the
// second is right), and UkikiCage's real table 0x02115974 has no _ZTV name at
// all. Every _ZN9UkikiCage*Ev body below is really a RollingLogTtm method and
// every _ZN13RollingLogTtm*Ev body is really a Ukiki method. The full evidence
// is in port/ov030_syms.txt's header. NOTHING in config/ or src/ is renamed by
// this lane -- a rename moves byte-gate credit and ripples into the decomp,
// which is not a port lane's call.
//
// src/ IS INTERNALLY CORRECT ON THE ADDRESSES: the recovered sources took the
// identity from the type_info route, so daObjHmBskt_c_classInit.c writes
// _ZTV13daObjHmBskt_c and daObjHmMaruta_c_classInit.c writes _ZTV15daObjHmMaruta_c
// (include/decl_common.h:535 and :595). Only the two Ukiki factories use the
// config spelling _ZTV13RollingLogTtm (decl_common.h:524) for daMky_c's table.
// This file defines all three host arrays under the names src actually uses.
//
// ============================================================================
// THE SLOT DERIVATION, ROM WORD BY ROM WORD
// ============================================================================
// Out of config/arm9/overlays/ov030/relocs.txt with targets resolved through
// config/arm9/symbols.txt, the port/hal/actor_classes_ov072.cpp route, and the
// words themselves read from extracted/overlays/overlay_0030.bin (trap T4 --
// never extracted/dsd/arm9_overlays/).
//
//  slot  UkikiCage 0x02115974  RollingLogTtm 0x02115a48  Ukiki 0x02115bfc   what
//   0    02111410 own          02111638 own              02114378 own      InitResources
//   1    02011268               02011268                  02011268          Actor::BeforeInitResources
//   2    02011244               02011244                  02011244          Actor::AfterInitResources(u32)
//   3    0211130c own          02111610 own              021141c4 own      CleanupResources
//   4    02011220               02011220                  02011220          Actor::BeforeCleanupResources
//   5    02011214               02011214                  02011214          Actor::AfterCleanupResources(u32)
//   6    02111384 own          02111624 own              02114278 own      Behavior
//   7    02010fd4               02010fd4                  02010fd4          Actor::BeforeBehavior
//   8    02010fc8               02010fc8                  02010fc8          Actor::AfterBehavior(u32)
//   9    02111350 own          02127124 ov080            02114230 own      Render
//  10    02010f78               02010f78                  02010f78          Actor::BeforeRender
//  11    02010f6c               02010f6c                  02010f6c          Actor::AfterRender(u32)
//  12    02043ac0               02043ac0                  0211422c own     OnPendingDestroy
//  13    0204357c               0204357c                  0204357c          ActorBase::Virtual34(u32,u32)
//  14    0204349c               0204349c                  0204349c          ActorBase::Virtual38(u32,u32)
//  15    02043494               02043494                  02043494          ActorBase::OnHeapCreated
//  16    021111a0 own          0211155c own              02111688 own      D1
//  17    021111ec own          021115ac own              021116d0 own      D0
//  18    02010160               02010160                  0211172c own     OnYoshiTryEat
//  19    02010154               02010154                  021145d4 own     OnTurnIntoEgg(Player&)
//  20    0201014c               0201014c                  0201014c          Actor::Virtual50
//  21    02010148               02010148                  02010148          Actor::OnGroundPounded(Actor&)
//  22    02010144               02010144                  02010144          Actor::OnAttacked1(Actor&)
//  23    02010140               02010140                  02010140          Actor::OnAttacked2(Actor&)
//  24    0201013c               0201013c                  0201013c          Actor::OnKicked(Actor&)
//  25    02010138               02010138                  02010138          Actor::OnPushed(Actor&)
//  26    02010134               02010134                  02010134          Actor::OnHitByCannonBlastedChar(Actor&)
//  27    02010130               02127058 ov080            02010130          OnHitByMegaChar(Player&)
//  28    0201012c               0201012c                  0201012c          Actor::OnHitFromUnderneath(Actor&)
//  29    02010124               02010124                  02010124          Actor::OnAimedAtWithEgg
//  30    020100dc               020100dc                  020100dc          Actor::OnAimedAtWithEggReturnVec
//  31    020ee55c               020ee55c                  --                Platform::Kill
//
// THE TWO ov080 SLOTS ARE THE CLOSURE GAP, and both are measured rather than
// guessed: relocs.txt resolves 0x02127124 and 0x02127058 as
// `module:overlays(78,80)`, and level 22's roster loads ov080 and not ov078, so
// the ov080 spelling is the live one. src/_ZN13daObjMaruta_c6RenderEv.cpp and
// src/_ZN13daObjMaruta_c15OnHitByMegaCharER6Player.cpp are appended to slice_ov030cast.txt for it.
//
// THE WIDTHS (32 / 32 / 31) ARE SETTLED BY THE SEMANTIC TAIL and by nothing
// else, because no mechanical route is right on all three. The reloc run
// over-reads 0x02115a48 by one, dsd's next symbol over-reads 0x02115bfc by two,
// and the typeinfo terminator reads 0x02115a48 as SEVENTY-NINE slots because a
// 22-pair pointer-to-member source table sits between that vtable and daMky_c's
// type_info record. All four width traps and the evidence are in
// port/ov030_syms.txt's header.
//
// ============================================================================
// THE ELEVEN-CELL STATE MACHINE
// ============================================================================
// The Ukiki dispatches an mwcc pointer-to-member state machine, the BabyPenguin
// six-cell shape (hal/actor_classes_ov072.cpp) with ELEVEN cells:
//
//   data_ov030_02115e0c[11], .bss, 16 bytes a cell, declared by
//   src/__sinit_ov030_02114924.c's own `S16 { S8 lo, hi; }`;
//   src/actors/daMky_c.cpp installs `&data_ov030_02115e0c[idx]` at +0x3a4;
//   func_ov030_02114170 dispatches pp+0 (the enter half, the sinit's `.lo`) and
//   func_ov030_02114134 dispatches pp+1 (the tick half, the `.hi`).
//
// THE CELL STRIDE IS ROM-VERIFIED, not read off the struct: func_ov030_021141a8
// is `add r1, r2, r1, lsl #4` -- 16 bytes -- and then `bx ip` straight into
// func_ov030_02114170, so installing a cell IS entering its state.
//
// STATE 3's TICK HALF HAS NO BODY. func_ov030_021136b0 (0x3d0 bytes) is the ONE
// missing body in the whole overlay: no delink block and no src file. It gets a
// LOUD face below, never a silent stub.
//
// ============================================================================
// WHAT THIS FILE DOES NOT CLAIM
// ============================================================================
//  - Whether the cast LOOKS right. Nobody has held this next to a DS. The lane
//    reports spawn counts and behaviour-list membership, not a verdict.
//  - What the eleven states MEAN. They are seated by address, in the sinit's
//    own field order, and the run reports which ones were entered.
// ============================================================================
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
#include <cstdlib>

#include "types.h"
#include "dtor_faces_cpp.h"
#include "dActor_c.h"
#include "fBase_c.h"
#include "daMky_c.h"
#include "RollingLogTtm.h"

extern "C" {
/* ---- the shared arm9 defaults, slots 1..30 ---- */
int _ZN8dActor_c19BeforeInitResourcesEv(void *self);
void _ZN8dActor_c18AfterInitResourcesEj(void *self, unsigned a);
int _ZN8dActor_c14BeforeBehaviorEv(void *self);
int _ZN8dActor_c12BeforeRenderEv(void *self);
int _ZN8dActor_c13OnYoshiTryEatEv(void *self);
void _ZN8dActor_c13OnTurnIntoEggER6Player(void *self, void *p);
int _ZN8dActor_c9Virtual50Ev(void *self);
void _ZN8dActor_c15OnGroundPoundedERS_(void *self, void *o);
void _ZN8dActor_c11OnAttacked1ERS_(void *self, void *o);
void _ZN8dActor_c11OnAttacked2ERS_(void *self, void *o);
void _ZN8dActor_c8OnKickedERS_(void *self, void *o);
void _ZN8dActor_c8OnPushedERS_(void *self, void *o);
void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN8dActor_c15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN8dActor_c19OnHitFromUnderneathERS_(void *self, void *o);
int _ZN8dActor_c16OnAimedAtWithEggEv(void *self);
void _ZN10dBgActor_c4KillEv(void *self);                    /* slot 31, 0x020ee55c */
void _ZN8dActor_cD2Ev(void *self);
extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);  /* hal/actor_registry */
void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model);

/* ---- the sub-object destructors the inline D1 thunk spells ---- */
void _ZN9ModelAnimD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN7dCcAc_cD1Ev(void *self);
void _ZN10dBgCh_ActrD1Ev(void *self);

/* ---- UkikiCage (103, 13daObjHmBskt_c), own bodies ---- */
int _ZN13daObjHmBskt_c13InitResourcesEv(void *self);      /* slot 0  InitResources */
int _ZN13daObjHmBskt_c16CleanupResourcesEv(void *self);      /* slot 3  CleanupResources */
int _ZN13daObjHmBskt_c8BehaviorEv(void *self);      /* slot 6  Behavior */
int _ZN13daObjHmBskt_c6RenderEv(char *self);      /* slot 9  Render */
int *_ZN13daObjHmBskt_cD1Ev(int *self);      /* slot 16 D1 */
int *_ZN13daObjHmBskt_cD0Ev(int *self);      /* slot 17 D0 */
void *daObjHmBskt_c_classInit(void);
extern unsigned char g_profile_HM_BASKET[];

/* ---- RollingLogTtm (102, 15daObjHmMaruta_c), own bodies.
        Every name here is the config's UkikiCage spelling and every one of them
        is really a RollingLogTtm method -- see this file's header. ---- */
int _ZN13RollingLogTtm13InitResourcesEv(void *self);      /* slot 0 */
int _ZN13RollingLogTtm16CleanupResourcesEv(void *self);   /* slot 3 */
int _ZN13RollingLogTtm8BehaviorEv(void *self);            /* slot 6 */
int _ZN13daObjMaruta_c6RenderEv(void *self);                 /* slot 9, ov080 */
void _ZN13daObjMaruta_c15OnHitByMegaCharER6Player(void *self, void *player);  /* slot 27, ov080 */
int *_ZN13RollingLogTtmD1Ev(int *self);                   /* slot 16 */
int *_ZN13RollingLogTtmD0Ev(int *self);                   /* slot 17 */
void *daObjHmMaruta_c_classInit(void);
extern unsigned char g_profile_HM_MARUTA[];

/* ---- the Ukiki (267 + 268, 7daMky_c), own bodies.
        Same shift the other way: every _ZN13RollingLogTtm*Ev is a Ukiki
        method. ---- */
int _ZN7daMky_c13InitResourcesEv(void *self);  /* slot 0, FACED at the bottom of this file */
int _ZN7daMky_c6RenderEv(void *self);          /* slot 9, FACED at the bottom of this file */
int _ZN7daMky_c16CleanupResourcesEv(void);    /* slot 3, .c body takes void */
/* Slot 6 is the HOST COPY in port/unmatched/Ukiki_Behavior.cpp and it is
   DELIBERATELY not exported under the Itanium name: src/__sinit_ov029_
   02112c10.c declares _ZN7daMky_c8BehaviorEv with no parameters and
   hands its ADDRESS to func_020731dc as an ov029 SharedFilePtr destructor
   callback -- a shared-load-window mis-attribution. Defining that name here
   would trip aritycheck's receiver ratchet today and silently bind ov029's
   callback to this class's Behavior the day lane W1-C slices that sinit.
   See that file's own header for the full argument. */
int port_ov030_ukiki_behavior(void *self);           /* slot 6, HOST COPY */
void _ZN7daMky_c16OnPendingDestroyEv(void);   /* slot 12, .c body takes void */
int *_ZN7daMky_cD0Ev(int *self);              /* slot 17 */
int _ZN7daMky_c13OnYoshiTryEatEv(void);                       /* slot 18, own OnYoshiTryEat, takes void */
/* Slot 19's body is a tail-call VENEER (ldr ip,[pc]; bx ip; .word 0x02043824
   onto _ZN7fBase_c18MarkForDestructionEv) and src declares it (void). It is
   declared WITH a self parameter here, exactly as gate 193 declares the
   byte-identical _ZN11BabyPenguin13OnTurnIntoEggER6Player: a veneer is a tail jump on the host too,
   so it forwards whatever this thunk pushed, and pushing nothing would leave
   MarkForDestruction reading the thunk's own return address as its self. */
void _ZN7daMky_c13OnTurnIntoEggER6Player(void *self);                /* slot 19, own OnTurnIntoEgg */
void *daMky_c_classInit_MONKEY_STAR(void);
void *daMky_c_classInit_MONKEY_THIEF(void);
extern unsigned char g_profile_MONKEY_STAR[];
extern unsigned char g_profile_MONKEY_THIEF[];

/* ---- the three host vtable arrays. A mounted vtable would hand a factory DS
        code addresses, so all three spans are excluded from port/ov030_syms.txt
        and live here (the ov015/ov016/ov022/ov045/ov080/ov072 rule). ---- */
int _ZTV13daObjHmBskt_c[32];    /* vtspan: _ZTV13daObjHmBskt_c */
int _ZTV13RollingLogTtm[32];    /* vtspan: data_ov030_02115a48, config name for it */
int _ZTV7daMky_c[31];           /* vtspan: data_ov030_02115bfc, config name for it */
}

/* ONE STORAGE, TWO NAMES, for 0x02115a48. src/d_a_obj_hm_maruta.c stores that
   table SECOND (its pool is 0x02111680 -> 0x02128338 then 0x02111684 ->
   0x02115a48) and spells the second store `VT1`, a placeholder, while spelling
   the FIRST store `_ZTV15daObjHmMaruta_c` -- the right name on the wrong store.
   port/CMakeLists.txt renames that TU's `_ZTV15daObjHmMaruta_c` to
   _ZTV13daObjMaruta_c and its VT1 to _ZTV13RollingLogTtm, and this alias points
   _ZTV13RollingLogTtm at the one host array. A macro whose body is another macro is
   rescanned and expanded again, so renaming VT1 straight to
   _ZTV15daObjHmMaruta_c would have collapsed BOTH stores onto the same array
   and lost the ov080 intermediate the ROM writes first.
   _ZTV13RollingLogTtm is config's own name for 0x02115a48 and it is CORRECT,
   not mis-attributed: the typeinfo at 0x02115a48-4 leads to the string
   "15daObjHmMaruta_c", and maruta is the log, so RollingLogTtm and
   daObjHmMaruta_c are one class under two names. The array now carries that
   config name outright, so VT1 resolves to it with no join at all.
   The LHS is deliberately UNDEFINED anywhere in the link, which is what
   /alternatename needs and what port/tools/alternatename_guard.py checks: it is
   excluded from port/ov030_syms.txt with the rest of that vtable span, and no
   host TU defines it. This is the _ZTV10dBgActor_c / _ZTV10dBgActor_c shape at
   hal/actor_classes.cpp:591, one storage reached under both spellings. */
/* RETIRED at ALIAS2 (wave 8, the main -> port sync). DEFEATED: the left hand side is a real definition in this link now (actor_classes_ov030.cpp.obj), so the directive is inert and alternatename_guard fails on it.
   STILL RETIRED after ALIAS5, and now also backwards: the live row is the other
   way round, at the end of this section. */
// #pragma comment(linker, "/alternatename:__ZTV13RollingLogTtm=__ZTV15daObjHmMaruta_c")

/* The block that used to stand here read "ONE STORAGE, TWO NAMES, for
   0x02115bfc as well" and rested on two claims that do not hold. Both were
   re-checked against the tree rather than taken from the note:
     - "config gives that address BOTH _ZTV13RollingLogTtm and _ZTV7daMky_c
       (symbols.txt lines 143 and 144)". Line 143 is _ZTV7daMky_c at 0x02115bfc;
       line 144 is data_ov030_02115c80, a bss row. _ZTV13RollingLogTtm is on
       line 115 at a DIFFERENT address, 0x02115a48.
     - "daMky_c_classInit_MONKEY_STAR.c and ..._MONKEY_THIEF.c spell
       _ZTV13RollingLogTtm". They do not: src/d_a_mky_monkey_star.c:22 and
       src/d_a_mky_monkey_thief.c:22 both store _ZTV7daMky_c, agreeing with the
       cartridge, and the only file in src/ that spells _ZTV13RollingLogTtm is
       src/d_a_obj_hm_maruta.c, the rolling log. */
/* THE TWO TABLES, read out of config rather than derived one name at a time.
   config/arm9/overlays/ov030/symbols.txt:

       0x02115a48   _ZTV13RollingLogTtm  (line 115)
       0x02115bfc   _ZTV7daMky_c         (line 143)

   ONE name each, at TWO addresses. The note that used to stand here said both
   names were on 0x02115bfc at lines 143 and 144; line 144 is
   data_ov030_02115c80, a bss row, and there is no second _ZTV at that address.
   The cartridge agrees three more ways. The typeinfo word at 0x02115a48-4 leads
   to the string "15daObjHmMaruta_c" (maruta is the log) and the one at
   0x02115bfc-4 to "7daMky_c" (the monkey). The factories disagree:
   daObjHmMaruta_c_classInit (0x0211164c) allocates 0x344 and stamps 0x02115a48,
   while daMky_c_classInit_MONKEY_STAR (0x021145e0) and _MONKEY_THIEF
   (0x02114638) each allocate 0x3cc and stamp 0x02115bfc, then build a ModelAnim
   at +0xd4, a ShadowModel at +0x138, a dCcAc_c at +0x160, a dBgCh_Actr at +0x194
   and a PathPtr at +0x398 that the 0x344 class never builds. And the two tables
   differ in ten of their thirty-one slots.

   So the join __ZTV7daMky_c=__ZTV13RollingLogTtm was false, and its effect was
   that src/d_a_obj_hm_maruta.c, which stamps _ZTV13RollingLogTtm, spawned every
   rolling log holding the MONKEY's methods -- including a Render that reads a
   ModelAnim at +0xd4 the log's own constructor never builds -- while the array
   this file fills with the log's methods was installed on nothing at all. Both
   arrays now carry config's own name for the address they hold, so no join is
   needed and none is written.

   _ZTV15daObjHmMaruta_c is the port's other spelling for the SAME class as
   _ZTV13RollingLogTtm (the RTTI string at 0x02115a48 is "15daObjHmMaruta_c");
   config carries no _ZTV under that spelling, only the _ZTS name string, so the
   row below is a true statement of identity and not an address join. Its LHS is
   undefined in the link, which is what /alternatename needs and what
   port/tools/alternatename_guard.py checks. */
#pragma comment(linker, "/alternatename:__ZTV15daObjHmMaruta_c=__ZTV13RollingLogTtm")

/* FIVE C-LINKAGE FLIPS. Five mounted data symbols are declared WITHOUT
   extern "C" by a //cpp TU in this slice, so MSVC mangles the reference while
   the mount defines the plain C name. The hal/cxx_aliases.cpp recipe applied
   verbatim; each was measured off the first link's own unresolved list, with
   the mangling MSVC actually produced, never predicted from the declaration:
     src/actors/daMky_c.cpp     extern int data_ov030_02115ce0[];
     src/actors/daMky_c.cpp     extern int data_ov030_02115d18[];
     src/actors/daMky_c.cpp     the same symbol again (LNK2001)
     src/actors/daMky_c.cpp  extern char data_ov002_*;
   All five are already mounted and already in the map as plain C symbols --
   the first two by port/ov030_syms.txt, the last three by port/ov002_syms.txt
   -- so this is a spelling bridge and not a new definition. Every LHS is
   deliberately undefined, which is what alternatename_guard.py checks. */
#pragma comment(linker, "/alternatename:?data_ov030_02115ce0@@3PAHA=_data_ov030_02115ce0")
#pragma comment(linker, "/alternatename:?data_ov030_02115d18@@3PAHA=_data_ov030_02115d18")
#pragma comment(linker, "/alternatename:?data_ov002_0210da40@@3DA=_data_ov002_0210da40")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9a0@@3DA=_data_ov002_0210d9a0")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9c0@@3DA=_data_ov002_0210d9c0")

extern "C" {

/* ---- the eleven-cell state storage and the twenty-two handler bodies ---- */
struct PortUkikiCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortUkikiCell data_ov030_02115e0c[11];

int _ZN7daMky_c11EnterState0Ev(void *c);
int func_ov030_02113ff0(void *c);
int _ZN7daMky_c11EnterState1Ev(void *c);
int func_ov030_02113d20(void *c);
int _ZN7daMky_c11EnterState2Ev(void *c);
int func_ov030_02113b38(void *c);
int _ZN7daMky_c11EnterState3Ev(void *c);
/* state 3's tick half, func_ov030_021136b0, HAS NO BODY -- faced below */
int _ZN7daMky_c11EnterState4Ev(void *c);
int func_ov030_02113324(void *c);
int _ZN7daMky_c11EnterState5Ev(void *c);
int func_ov030_02113094(void *c);
int _ZN7daMky_c11EnterState6Ev(void *c);
int func_ov030_02112da0(void *c);
int _ZN7daMky_c11EnterState7Ev(void *c);
int func_ov030_02112a84(void *c);
int _ZN7daMky_c11EnterState8Ev(void *c);
int func_ov030_02112578(void *c);
int _ZN7daMky_c11EnterState9Ev(void *c);
int func_ov030_02112400(void *c);
int _ZN7daMky_c12EnterState10Ev(void *c);
int func_ov030_021122b0(void *c);
}

// ---- the traps ------------------------------------------------------------
// Slots 13/14 are ActorBase::Virtual34/Virtual38 and slot 30 is
// Actor::OnAimedAtWithEggReturnVec. None of the three can be serviced by a
// __fastcall(void*, void*) thunk: 13/14 take two u32 arguments each and their
// bodies (solid-heap allocator work) are not linked into the port at all, and
// 30 is an SRET method whose hidden return pointer no thunk shape here models.
// The ccm/ov064/jrb/bbh/ov072 convention: trap them by name rather than seat a
// body with the wrong contract. Do not read "its body is not linked" as "the
// slot is not reachable" -- dispatch is indirect through the table.
static void ov30_trap_report(void *self, int slot, const char *what)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: %s is not hosted (actor id %u %s, phase %d, "
                 "spawn step %d)\n",
                 what, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[160];
      std::snprintf(_m, sizeof _m, "unhosted %s on id %u %s",
                    what, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
    (void)slot;
}
static int __fastcall ov30_trap13(void *s, void *) { ov30_trap_report(s, 13, "vtable slot 13 fBase_c::Virtual34(u32,u32)"); return 0; }
static int __fastcall ov30_trap14(void *s, void *) { ov30_trap_report(s, 14, "vtable slot 14 fBase_c::Virtual38(u32,u32)"); return 0; }

// ---- THE ONE MISSING BODY -------------------------------------------------
// func_ov030_021136b0 (0x3d0 bytes) is the Ukiki's state 3 TICK half. It has
// neither a delink block in config/arm9/overlays/ov030/delinks.txt nor a src
// file anywhere in the tree -- the only such symbol in the overlay (63 function
// symbols, 62 with a body). It is faced LOUDLY rather than stubbed, the
// hal/scene_boot.cpp l2_trap model: a silent stub here would make the Ukiki
// sit in state 3 doing nothing and read as "the monkey is idle" rather than as
// a hole.
static int ov30_missing_021136b0_said;
static void ov30_missing_021136b0(void *c)
{
    unsigned id = c ? *(unsigned short *)((char *)c + 0xc) : 0u;
    if (!ov30_missing_021136b0_said) {
        ov30_missing_021136b0_said = 1;
        std::fprintf(stderr,
                     "UNHOSTED: func_ov030_021136b0 (ov030 state 3 tick, 0x3d0 "
                     "bytes) HAS NO MATCHED BODY -- no delink block and no src "
                     "file. The Ukiki (actor id %u %s) entered state 3 and its "
                     "tick does nothing. This is the ONE missing body in "
                     "ov030; see port/slice_ov030cast.txt section 2.\n",
                     id, port_actor_class_name(id));
    }
    { static char _m[128];
      std::snprintf(_m, sizeof _m,
                    "unhosted ov030 state 3 tick (func_ov030_021136b0) on id %u", id);
      port_actor_slot_decline(_m); }
}

// ---- the shared 1..30 half; all three tables share it ---------------------
static int __fastcall ov30_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall ov30_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall ov30_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall ov30_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall ov30_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall ov30_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall ov30_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall ov30_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall ov30_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall ov30_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
static int __fastcall ov30_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov30_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall ov30_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov30_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov30_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov30_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall ov30_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall ov30_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov30_mega(void *s, void *, void *p)
{ _ZN8dActor_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov30_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov30_aimed(void *s, void *)
{ return _ZN8dActor_c16OnAimedAtWithEggEv(s); }
/* Slot 12's ROM word on both Platform-derived tables is 0x02043ac0,
   ActorBase::OnPendingDestroy -- an empty 4-byte body. The port links it as an
   MSVC METHOD (?OnPendingDestroy@fBase_c@@UAEXXZ), never under the Itanium
   C name, so it is reached through the class exactly as hal/actor_classes.cpp's
   own ac_pdes_base does, and it returns void. */
static void __fastcall ov30_pdes_default(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); }
static int __fastcall ov30_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }

/* Fills slots 1,2,4,5,7,8,10,11,13,14,15,18..30 -- every slot all three tables
   share. The caller writes its own 0/3/6/9/12/16/17 afterward, plus slot 12 for
   the two Platform classes (they take the arm9 default there and it is written
   here rather than left NULL), plus daMky_c's own 18/19 and the two
   Platform-derived tables' slot 31.

   SLOT NUMBERING IS THE ROM'S, taken from the three reloc spans tabulated in
   this file's header, not from the order the bodies are declared in. */
static void ov30_fill_shared(void **vt)
{
    vt[1]  = (void *)ov30_binit;    /* Actor::BeforeInitResources */
    vt[2]  = (void *)ov30_ainit;    /* Actor::AfterInitResources(u32) */
    vt[4]  = (void *)ov30_bclean;   /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)ov30_aclean;   /* Actor::AfterCleanupResources(u32) */
    vt[7]  = (void *)ov30_bbeh;     /* Actor::BeforeBehavior */
    vt[8]  = (void *)ov30_abeh;     /* Actor::AfterBehavior(u32) */
    vt[10] = (void *)ov30_bren;     /* Actor::BeforeRender */
    vt[11] = (void *)ov30_aren;     /* Actor::AfterRender(u32) */
    vt[13] = (void *)ov30_trap13;   /* ActorBase::Virtual34(u32,u32), not linked */
    vt[14] = (void *)ov30_trap14;   /* ActorBase::Virtual38(u32,u32), not linked */
    vt[15] = (void *)ov30_heap;     /* ActorBase::OnHeapCreated */
    vt[18] = (void *)ov30_yoshi;    /* Actor::OnYoshiTryEat */
    vt[19] = (void *)ov30_egg;      /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)ov30_v50;      /* Actor::Virtual50 */
    vt[21] = (void *)ov30_pounded;  /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)ov30_atk1;     /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)ov30_atk2;     /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)ov30_kicked;   /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)ov30_pushed;   /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)ov30_cannon;   /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)ov30_mega;     /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)ov30_under;    /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)ov30_aimed;    /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)port_actor_s30_base;   /* Actor::OnAimedAtWithEggReturnVec, SRET */
}

// ============================================================================
// THE ELEVEN-CELL STATE SEAT
// ============================================================================
/* {the ROM address the sinit's own source pair carries, the host body}. Order
   is __sinit_ov030_02114924.c's OWN cell order, not sorted by address, and the
   ROM column is verified against the mounted bytes before the rewrite -- the
   SoundObject/Cap/MrBlizzard/BabyPenguin shape: a mount pointing at the wrong
   bytes ABORTS instead of silently calling into garbage. */
typedef void (*PortUkikiFn)(void *);
static void ukiki_state3_tick(void *c) { ov30_missing_021136b0(c); }

/* ---- RUN link100 LANE PMFSWEEP3'S HANDOFF: THE ELEVEN TICK CELLS TAKE THEIR
   RECEIVER IN ECX. ?Behavior@daMky_c@@UAEHXZ +0x1f5..+0x203 dispatches the
   TICK half inline, with nothing pushed:

       mov eax,[edi+0x3a4]   the cell
       mov ecx,[eax+0xc]     the TICK delta
       mov eax,[eax+8]       the TICK word
       add ecx,edi           this + delta
       call eax              a REAL call

   while the eleven tick_rom bodies below (func_ov030_* and ukiki_state3_tick)
   are matched flat cdecl bodies that read their receiver off the stack at
   [ebp+8]. This is 5ae983797's family at another class (daMip_c and
   Scuttlebug already fixed on port/l7-pmfsweep2). The ENTER half is NOT
   reached only by the flat tail jumps _func_ov030_021141a8 and _02114134,
   _02113324, _02113d20, _02113ff0: see the UKIKI2 block below, which names
   two more inlined readers, so the enter_host column is thunked there too.
   Each thunk names its matched body, so trap T2's rule still holds. */
static void __fastcall uk_02113ff0(void *self, void *)
{ func_ov030_02113ff0(self); }
static void __fastcall uk_02113d20(void *self, void *)
{ func_ov030_02113d20(self); }
static void __fastcall uk_02113b38(void *self, void *)
{ func_ov030_02113b38(self); }
static void __fastcall uk_021136b0(void *self, void *)
{ ukiki_state3_tick(self); }
static void __fastcall uk_02113324(void *self, void *)
{ func_ov030_02113324(self); }
static void __fastcall uk_02113094(void *self, void *)
{ func_ov030_02113094(self); }
static void __fastcall uk_02112da0(void *self, void *)
{ func_ov030_02112da0(self); }
static void __fastcall uk_02112a84(void *self, void *)
{ func_ov030_02112a84(self); }
static void __fastcall uk_02112578(void *self, void *)
{ func_ov030_02112578(self); }
static void __fastcall uk_02112400(void *self, void *)
{ func_ov030_02112400(self); }
static void __fastcall uk_021122b0(void *self, void *)
{ func_ov030_021122b0(self); }

/* ---- RUN link100 WAVE 15 LANE UKIKI2: THE ELEVEN *ENTER* CELLS TAKE THEIR
   RECEIVER IN ECX TOO, and the block above is wrong about the readers. The
   flat tail jumps are not the only callers into data_ov030_02115e0c:
   ?InitResources@daMky_c@@UAEHXZ inlines two more dispatches into the same
   runtime array, read back out of this lane's own build with
   tmp/scan_inline_pmf.py (the HMC1 census tool):

       ?InitResources@daMky_c@@UAEHXZ+0x1d0  call ECX -> record 0 enter
       ?InitResources@daMky_c@@UAEHXZ+0x205  call ECX -> record 1 enter

   both `lea ecx,[ecx+this]; call dword ptr [cell]` with NOTHING pushed --
   the identical shape 65deff04d (fixer SBENTER, the same night) fixed at
   Scuttlebug's nine ENTER cells. A thunk is correct at every other reader
   too: the flat dispatcher _func_ov030_021141a8 (va 006d1320 on this
   lane's build; the ledger's old 006cd080 was already stale before this
   change) sets ecx as well as riding the receiver through:

       +0x18  8b4a04  mov ecx, dword ptr [edx+4]     the delta
       +0x1b  03c8    add ecx, eax                   ecx = this + delta
       +0x1d  8b02    mov eax, dword ptr [edx]
       +0x20  ffe0    jmp eax

   so no cell regresses. Each thunk names its matched body, so trap T2's
   rule still holds. */
static void __fastcall uk_02114124(void *self, void *)
{ _ZN7daMky_c11EnterState0Ev(self); }
static void __fastcall uk_02113fd8(void *self, void *)
{ _ZN7daMky_c11EnterState1Ev(self); }
static void __fastcall uk_02113be8(void *self, void *)
{ _ZN7daMky_c11EnterState2Ev(self); }
static void __fastcall uk_02113a80(void *self, void *)
{ _ZN7daMky_c11EnterState3Ev(self); }
static void __fastcall uk_0211360c(void *self, void *)
{ _ZN7daMky_c11EnterState4Ev(self); }
static void __fastcall uk_021132d4(void *self, void *)
{ _ZN7daMky_c11EnterState5Ev(self); }
static void __fastcall uk_02112ff8(void *self, void *)
{ _ZN7daMky_c11EnterState6Ev(self); }
static void __fastcall uk_02112c14(void *self, void *)
{ _ZN7daMky_c11EnterState7Ev(self); }
static void __fastcall uk_02112a14(void *self, void *)
{ _ZN7daMky_c11EnterState8Ev(self); }
static void __fastcall uk_02112560(void *self, void *)
{ _ZN7daMky_c11EnterState9Ev(self); }
static void __fastcall uk_021123a4(void *self, void *)
{ _ZN7daMky_c12EnterState10Ev(self); }

static const struct { unsigned enter_rom, tick_rom; PortUkikiFn enter_host, tick_host; }
g_ukiki_cells[11] = {
    { 0x02114124, 0x02113ff0, (PortUkikiFn)(void *)uk_02114124, (PortUkikiFn)(void *)uk_02113ff0 },
    { 0x02113fd8, 0x02113d20, (PortUkikiFn)(void *)uk_02113fd8, (PortUkikiFn)(void *)uk_02113d20 },
    { 0x02113be8, 0x02113b38, (PortUkikiFn)(void *)uk_02113be8, (PortUkikiFn)(void *)uk_02113b38 },
    { 0x02113a80, 0x021136b0, (PortUkikiFn)(void *)uk_02113a80, (PortUkikiFn)(void *)uk_021136b0 },
    { 0x0211360c, 0x02113324, (PortUkikiFn)(void *)uk_0211360c, (PortUkikiFn)(void *)uk_02113324 },
    { 0x021132d4, 0x02113094, (PortUkikiFn)(void *)uk_021132d4, (PortUkikiFn)(void *)uk_02113094 },
    { 0x02112ff8, 0x02112da0, (PortUkikiFn)(void *)uk_02112ff8, (PortUkikiFn)(void *)uk_02112da0 },
    { 0x02112c14, 0x02112a84, (PortUkikiFn)(void *)uk_02112c14, (PortUkikiFn)(void *)uk_02112a84 },
    { 0x02112a14, 0x02112578, (PortUkikiFn)(void *)uk_02112a14, (PortUkikiFn)(void *)uk_02112578 },
    { 0x02112560, 0x02112400, (PortUkikiFn)(void *)uk_02112560, (PortUkikiFn)(void *)uk_02112400 },
    { 0x021123a4, 0x021122b0, (PortUkikiFn)(void *)uk_021123a4, (PortUkikiFn)(void *)uk_021122b0 },
};

extern "C" void port_ukiki_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 11; ++i) {
        PortUkikiCell &cell = data_ov030_02115e0c[i];
        if (cell.enter_fn != g_ukiki_cells[i].enter_rom || cell.enter_delta != 0 ||
            cell.tick_fn != g_ukiki_cells[i].tick_rom || cell.tick_delta != 0) {
            std::fprintf(stderr, "FATAL: Ukiki state cell %d: the sinit left "
                         "%08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         cell.enter_fn, cell.enter_delta, cell.tick_fn, cell.tick_delta,
                         g_ukiki_cells[i].enter_rom, g_ukiki_cells[i].tick_rom);
            std::abort();
        }
        cell.enter_fn = (unsigned)(size_t)g_ukiki_cells[i].enter_host;
        cell.tick_fn = (unsigned)(size_t)g_ukiki_cells[i].tick_host;
    }
}

// ============================================================================
// UkikiCage (103, 13daObjHmBskt_c) -- Platform-derived, 32 slots
// ============================================================================
static int __fastcall cage_init(void *s, void *)
{ return _ZN13daObjHmBskt_c13InitResourcesEv(s); }
static int __fastcall cage_clean(void *s, void *)
{ return _ZN13daObjHmBskt_c16CleanupResourcesEv(s); }
static int __fastcall cage_behavior(void *s, void *)
{ return _ZN13daObjHmBskt_c8BehaviorEv(s); }
static int __fastcall cage_render(void *s, void *)
{ port_actor_render_probe("UKIKI_CAGE", (char *)s + 0xd4);
  return _ZN13daObjHmBskt_c6RenderEv((char *)s); }
static int __fastcall cage_d1(void *s, void *)
{ return (int)(size_t)_ZN13daObjHmBskt_cD1Ev((int *)s); }
static int __fastcall cage_d0(void *s, void *)
{ return (int)(size_t)_ZN13daObjHmBskt_cD0Ev((int *)s); }

extern "C" void hal_fill_ukikicage_vtable(void)
{
    void **vt = (void **)_ZTV13daObjHmBskt_c;
    ov30_fill_shared(vt);
    vt[0]  = (void *)cage_init;
    vt[3]  = (void *)cage_clean;
    vt[6]  = (void *)cage_behavior;
    vt[9]  = (void *)cage_render;
    vt[12] = (void *)ov30_pdes_default;  /* ROM takes ActorBase's 0x02043ac0 */
    vt[16] = (void *)PORT_D16(cage_d1);
    vt[17] = (void *)cage_d0;
    vt[31] = (void *)ov30_kill;          /* Platform::Kill, the 32nd slot */
}

// ============================================================================
// RollingLogTtm (102, 15daObjHmMaruta_c) -- Platform-derived, 32 slots.
// Every _ZN9UkikiCage*Ev below is one of THIS class's methods.
// ============================================================================
static int __fastcall log_init(void *s, void *)
{ return _ZN13RollingLogTtm13InitResourcesEv(s); }
static int __fastcall log_clean(void *s, void *)
{ return _ZN13RollingLogTtm16CleanupResourcesEv(s); }
static int __fastcall log_behavior(void *s, void *)
{ return _ZN13RollingLogTtm8BehaviorEv(s); }
/* Slot 9 comes from ov080, not from ov030 and not from arm9. It is the same
   six-virtual plain-Model shadow src/_ZN13daObjHmBskt_c6RenderEv.cpp uses, calling
   index 5, which hal/cxxname_bridge.cpp:511 dual-fills as Render. */
static int __fastcall log_render(void *s, void *)
{ port_actor_render_probe("ROLLING_LOG_TTM", (char *)s + 0xd4);
  return _ZN13daObjMaruta_c6RenderEv(s); }
/* Slot 27 also comes from ov080: OnHitByMegaChar(Player&), one stack argument. */
static int __fastcall log_mega(void *s, void *, void *p)
{ _ZN13daObjMaruta_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall log_d1(void *s, void *)
{ return (int)(size_t)_ZN13RollingLogTtmD1Ev((int *)s); }
static int __fastcall log_d0(void *s, void *)
{ return (int)(size_t)_ZN13RollingLogTtmD0Ev((int *)s); }

extern "C" void hal_fill_rollinglogttm_vtable(void)
{
    void **vt = (void **)_ZTV13RollingLogTtm;
    ov30_fill_shared(vt);
    vt[0]  = (void *)log_init;
    vt[3]  = (void *)log_clean;
    vt[6]  = (void *)log_behavior;
    vt[9]  = (void *)log_render;         /* ov080 override */
    vt[12] = (void *)ov30_pdes_default;  /* ROM takes ActorBase's 0x02043ac0 */
    vt[16] = (void *)PORT_D16(log_d1);
    vt[17] = (void *)log_d0;
    vt[27] = (void *)log_mega;           /* ov080 override, NOT Actor's default */
    vt[31] = (void *)ov30_kill;          /* Platform::Kill, the 32nd slot */
}

// ============================================================================
// The Ukiki (267 UkikiThief + 268 UkikiStar, 7daMky_c) -- plain Actor, 31
// slots. Every _ZN13RollingLogTtm*Ev below is one of THIS class's methods.
// ============================================================================
static int __fastcall mky_init(void *s, void *)
{ return _ZN7daMky_c13InitResourcesEv(s); }
static int __fastcall mky_clean(void *s, void *)
{ (void)s; return _ZN7daMky_c16CleanupResourcesEv(); }
static int __fastcall mky_behavior(void *s, void *)
{ return port_ov030_ukiki_behavior(s); }
static int __fastcall mky_render(void *s, void *)
{ port_actor_render_probe("UKIKI", (char *)s + 0xd4);
  return _ZN7daMky_c6RenderEv(s); }
static int __fastcall mky_pdes(void *s, void *)
{ (void)s; _ZN7daMky_c16OnPendingDestroyEv(); return 0; }
/* Slot 16, D1. src/actors/daMky_c.cpp is a real MSVC-synthesised
   destructor over a local shadow class, so it is kept OUT of the slice: MSVC
   would mangle it ?1RollingLogTtm@@UAE@XZ and the Itanium name the slot needs
   would never exist. Chain spelled directly, HIGH ADDRESS FIRST, from that
   file's own member list -- WithMeshClsn +0x194, MovingCylinderClsn +0x160,
   ShadowModel +0x138, ModelAnim +0xd4, then Actor's own D2.

   THE ROM D1 DOES STORE A VTABLE AND THIS THUNK DOES NOT, so read the omission
   narrowly. An earlier revision of this comment claimed the body had "no vtable
   store of its own", copying gate 193's finding about ITS class; that is false
   here. The ROM at 0x02111688 is:

       02111690  ldr r1, [pc, #0x34]    ; pool 0x021116cc = 0x02115bfc
       02111694  add r0, r4, #0x194
       02111698  str r1, [r4]           ; installs its OWN table at +0
       0211169c  bl  _ZN10dBgCh_ActrD1Ev ...

   WHY DROPPING IT IS HARMLESS HERE, AND ONLY HERE. The word it installs is the
   class's OWN table, 0x02115bfc -- not a base table -- and on the host that is
   _ZTV13RollingLogTtm, the very array the object's vptr already holds: the
   factory wrote it, nothing between has swapped it, and this slot is only ever
   reached AS the most-derived class through that same array. Every call in the
   chain below is a direct call, not a virtual dispatch, so nothing reads the
   vptr while the thunk runs. Re-storing the pointer would be a no-op.

   DO NOT REUSE THIS AS A GENERAL RULE. A destructor that installs a DIFFERENT
   table is doing real work and its stores must be reproduced -- this overlay
   has three such bodies two slots away: src/actors/daMky_c.cpp and
   src/actors/daMky_c.cpp (which are RollingLogTtm's, not UkikiCage's) walk
   0x02115a48 -> _ZTV13daObjMaruta_c -> _ZTV10dBgActor_c, three tables deep, and
   src/_ZN13daObjHmBskt_cD1Ev.cpp walks two. All three stay in the slice and run
   their own stores. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (mky_d1) spelled the same chain by hand. */
static int __fastcall mky_d0(void *s, void *)
{ return (int)(size_t)_ZN7daMky_cD0Ev((int *)s); }
static int __fastcall mky_yoshi(void *s, void *)
{ (void)s; return _ZN7daMky_c13OnYoshiTryEatEv(); }
/* Slot 19 is OnTurnIntoEgg(Player &player): the caller pushes the player, so
   the thunk needs the third parameter to pop it even though the ov030 body
   takes nothing of its own. The BabyPenguin slot-19 reasoning applies
   unchanged. */
static int __fastcall mky_egg(void *s, void *, void *)
{ _ZN7daMky_c13OnTurnIntoEggER6Player(s); return 0; }

extern "C" void hal_fill_ukiki_vtable(void)
{
    /* Seat and verify the eleven PMF cells BEFORE anything can dispatch through
       them: func_ov030_021141a8 installs a cell and TAIL-JUMPS straight into
       the enter half, and InitResources reaches it on the first frame. */
    port_ukiki_states_seat();
    void **vt = (void **)_ZTV7daMky_c;
    ov30_fill_shared(vt);
    vt[0]  = (void *)mky_init;
    vt[3]  = (void *)mky_clean;
    vt[6]  = (void *)mky_behavior;
    vt[9]  = (void *)mky_render;
    vt[12] = (void *)mky_pdes;   /* own body, overrides ActorBase's default */
    vt[16] = (void *)PORT_D16(hal_cppd1_RollingLogTtm);
    vt[17] = (void *)mky_d0;
    vt[18] = (void *)mky_yoshi;  /* own OnYoshiTryEat, overrides the shared default */
    vt[19] = (void *)mky_egg;    /* own OnTurnIntoEgg, overrides the shared default */
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}

// ---- method faces ----------------------------------------------------------
// src/actors/daMky_c.cpp and _ZN7daMky_c6RenderEv
// .cpp are real MSVC methods against include/RollingLogTtm.h
// (?InitResources@RollingLogTtm@@..., not __ZN13RollingLogTtm...), so the
// Itanium names the vtable slots need are faced here -- the IceSheet /
// OneUpLogo / BabyPenguin recipe.
// The Ukiki's Behavior is NOT faced and is NOT exported under its Itanium name
// either: it is the HOST COPY in port/unmatched/Ukiki_Behavior.cpp, which
// deliberately exports port_ov030_ukiki_behavior instead, and the slot-6 thunk
// above calls that name. Two reasons, both in that file's header -- the
// aritycheck receiver ratchet, and the ov029 sinit that would otherwise bind
// its own SharedFilePtr destructor callback to this body. The reason the TU is
// a host copy at all is the ModelAnim slot-3 shadow, also in that header.
// _ZN7daMky_c16CleanupResourcesEv and _ZN7daMky_c16OnPendingDestroyEv
// are plain C-linkage .c bodies taking (void) -- no face needed, declared
// extern "C" above and called directly.
extern "C" {
int _ZN7daMky_c13InitResourcesEv(void *self)
{ return ((daMky_c *)self)->daMky_c::InitResources(); }
int _ZN7daMky_c6RenderEv(void *self)
{ return ((daMky_c *)self)->daMky_c::Render(); }
}
