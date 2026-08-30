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
// WHAT CONFIG SAYS INSTEAD: _ZTV9UkikiCage is on 0x02115a48 (RollingLogTtm's
// table), _ZTV13RollingLogTtm and _ZTV7daMky_c are BOTH on 0x02115bfc (only the
// second is right), and UkikiCage's real table 0x02115974 has no _ZTV name at
// all. Every _ZN9UkikiCage*Ev body below is really a RollingLogTtm method and
// every _ZN13RollingLogTtm*Ev body is really a Ukiki method. The full evidence
// is in port/ov030_syms.txt's header. NOTHING in config/ or src/ is renamed by
// this lane -- a rename moves byte-gate credit and ripples into the decomp,
// which is not a port lane's call.
//
// src/ IS INTERNALLY CORRECT ON THE ADDRESSES: the recovered sources took the
// identity from the type_info route, so UkikiCage_Spawn.c writes
// _ZTV13daObjHmBskt_c and RollingLogTtm_Spawn.c writes _ZTV15daObjHmMaruta_c
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
// the ov080 spelling is the live one. src/func_ov080_02127124.cpp and
// src/func_ov080_02127058.c are appended to slice_ov030cast.txt for it.
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
//   src/func_ov030_021141a8.c installs `&data_ov030_02115e0c[idx]` at +0x3a4;
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
#include <cstdio>
#include <cstdlib>

#include "types.h"
#include "Actor.h"
#include "ActorBase.h"
#include "RollingLogTtm.h"

extern "C" {
/* ---- the shared arm9 defaults, slots 1..30 ---- */
int _ZN5Actor19BeforeInitResourcesEv(void *self);
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);
int _ZN5Actor14BeforeBehaviorEv(void *self);
int _ZN5Actor12BeforeRenderEv(void *self);
int _ZN5Actor13OnYoshiTryEatEv(void *self);
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);
int _ZN5Actor9Virtual50Ev(void *self);
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);
void _ZN5Actor8OnKickedERS_(void *self, void *o);
void _ZN5Actor8OnPushedERS_(void *self, void *o);
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);
int _ZN5Actor16OnAimedAtWithEggEv(void *self);
void _ZN8Platform4KillEv(void *self);                    /* slot 31, 0x020ee55c */
void _ZN5ActorD2Ev(void *self);
extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);  /* hal/actor_registry */
void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model);

/* ---- the sub-object destructors the inline D1 thunk spells ---- */
void _ZN9ModelAnimD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN12WithMeshClsnD1Ev(void *self);

/* ---- UkikiCage (103, 13daObjHmBskt_c), own bodies ---- */
int func_ov030_02111410(void *self);      /* slot 0  InitResources */
int func_ov030_0211130c(void *self);      /* slot 3  CleanupResources */
int func_ov030_02111384(void *self);      /* slot 6  Behavior */
int func_ov030_02111350(char *self);      /* slot 9  Render */
int *func_ov030_021111a0(int *self);      /* slot 16 D1 */
int *func_ov030_021111ec(int *self);      /* slot 17 D0 */
void *UkikiCage_Spawn(void);
extern unsigned char UkikiCage_SpawnInfo[];

/* ---- RollingLogTtm (102, 15daObjHmMaruta_c), own bodies.
        Every name here is the config's UkikiCage spelling and every one of them
        is really a RollingLogTtm method -- see this file's header. ---- */
int _ZN9UkikiCage13InitResourcesEv(void *self);      /* slot 0 */
int _ZN9UkikiCage16CleanupResourcesEv(void *self);   /* slot 3 */
int _ZN9UkikiCage8BehaviorEv(void *self);            /* slot 6 */
int func_ov080_02127124(void *self);                 /* slot 9, ov080 */
void func_ov080_02127058(void *self, void *player);  /* slot 27, ov080 */
int *_ZN9UkikiCageD1Ev(int *self);                   /* slot 16 */
int *_ZN9UkikiCageD0Ev(int *self);                   /* slot 17 */
void *RollingLogTtm_Spawn(void);
extern unsigned char RollingLogTtm_SpawnInfo[];

/* ---- the Ukiki (267 + 268, 7daMky_c), own bodies.
        Same shift the other way: every _ZN13RollingLogTtm*Ev is a Ukiki
        method. ---- */
int _ZN13RollingLogTtm13InitResourcesEv(void *self);  /* slot 0, FACED at the bottom of this file */
int _ZN13RollingLogTtm6RenderEv(void *self);          /* slot 9, FACED at the bottom of this file */
int _ZN13RollingLogTtm16CleanupResourcesEv(void);    /* slot 3, .c body takes void */
/* Slot 6 is the HOST COPY in port/unmatched/Ukiki_Behavior.cpp and it is
   DELIBERATELY not exported under the Itanium name: src/__sinit_ov029_
   02112c10.c declares _ZN13RollingLogTtm8BehaviorEv with no parameters and
   hands its ADDRESS to func_020731dc as an ov029 SharedFilePtr destructor
   callback -- a shared-load-window mis-attribution. Defining that name here
   would trip aritycheck's receiver ratchet today and silently bind ov029's
   callback to this class's Behavior the day lane W1-C slices that sinit.
   See that file's own header for the full argument. */
int port_ov030_ukiki_behavior(void *self);           /* slot 6, HOST COPY */
void _ZN13RollingLogTtm16OnPendingDestroyEv(void);   /* slot 12, .c body takes void */
int *_ZN13RollingLogTtmD0Ev(int *self);              /* slot 17 */
int func_ov030_0211172c(void);                       /* slot 18, own OnYoshiTryEat, takes void */
/* Slot 19's body is a tail-call VENEER (ldr ip,[pc]; bx ip; .word 0x02043824
   onto _ZN9ActorBase18MarkForDestructionEv) and src declares it (void). It is
   declared WITH a self parameter here, exactly as gate 193 declares the
   byte-identical func_ov072_02121fa0: a veneer is a tail jump on the host too,
   so it forwards whatever this thunk pushed, and pushing nothing would leave
   MarkForDestruction reading the thunk's own return address as its self. */
void func_ov030_021145d4(void *self);                /* slot 19, own OnTurnIntoEgg */
void *UkikiStar_Spawn(void);
void *UkikiThief_Spawn(void);
extern unsigned char UkikiStar_SpawnInfo[];
extern unsigned char UkikiThief_SpawnInfo[];

/* ---- the three host vtable arrays. A mounted vtable would hand a factory DS
        code addresses, so all three spans are excluded from port/ov030_syms.txt
        and live here (the ov015/ov016/ov022/ov045/ov080/ov072 rule). ---- */
int _ZTV13daObjHmBskt_c[32];    /* vtspan: data_ov030_02115974 */
int _ZTV15daObjHmMaruta_c[32];  /* vtspan: data_ov030_02115a48 */
int _ZTV13RollingLogTtm[31];    /* vtspan: data_ov030_02115bfc, really _ZTV7daMky_c */
}

/* ONE STORAGE, TWO NAMES, for 0x02115a48. src/RollingLogTtm_Spawn.c stores that
   table SECOND (its pool is 0x02111680 -> 0x02128338 then 0x02111684 ->
   0x02115a48) and spells the second store `VT1`, a placeholder, while spelling
   the FIRST store `_ZTV15daObjHmMaruta_c` -- the right name on the wrong store.
   port/CMakeLists.txt renames that TU's `_ZTV15daObjHmMaruta_c` to
   data_ov080_02128338 and its VT1 to _ZTV9UkikiCage, and this alias points
   _ZTV9UkikiCage at the one host array. A macro whose body is another macro is
   rescanned and expanded again, so renaming VT1 straight to
   _ZTV15daObjHmMaruta_c would have collapsed BOTH stores onto the same array
   and lost the ov080 intermediate the ROM writes first.
   _ZTV9UkikiCage is config's own name for 0x02115a48 -- mis-attributed (that
   table is RollingLogTtm's, see this file's header) but the right ADDRESS, so
   the alias documents the config defect rather than inventing a name.
   The LHS is deliberately UNDEFINED anywhere in the link, which is what
   /alternatename needs and what port/tools/alternatename_guard.py checks: it is
   excluded from port/ov030_syms.txt with the rest of that vtable span, and no
   host TU defines it. This is the _ZTV10dBgActor_c / _ZTV8Platform shape at
   hal/actor_classes.cpp:591, one storage reached under both spellings. */
#pragma comment(linker, "/alternatename:__ZTV9UkikiCage=__ZTV15daObjHmMaruta_c")

/* ONE STORAGE, TWO NAMES, for 0x02115bfc as well. config gives that address
   BOTH _ZTV13RollingLogTtm and _ZTV7daMky_c (symbols.txt lines 143 and 144),
   and src uses whichever its own TU happened to be recovered under:
   UkikiStar_Spawn.c and UkikiThief_Spawn.c spell _ZTV13RollingLogTtm, while
   src/_ZN13RollingLogTtmD0Ev.c spells _ZTV7daMky_c. The host array carries the
   first name (decl_common.h:524 declares it), so this alias points the second
   at the same storage. Measured, not predicted: the FIRST link of this seat
   failed with exactly this unresolved external and no other spelling.
   _ZTV7daMky_c is the name that is actually RIGHT about the class -- see this
   file's header -- and it is the one this lane could not use as the array's own
   name, because src/ reaches the table under the other one from two TUs. */
#pragma comment(linker, "/alternatename:__ZTV7daMky_c=__ZTV13RollingLogTtm")

/* FIVE C-LINKAGE FLIPS. Five mounted data symbols are declared WITHOUT
   extern "C" by a //cpp TU in this slice, so MSVC mangles the reference while
   the mount defines the plain C name. The hal/cxx_aliases.cpp recipe applied
   verbatim; each was measured off the first link's own unresolved list, with
   the mangling MSVC actually produced, never predicted from the declaration:
     src/func_ov030_0211360c.cpp     extern int data_ov030_02115ce0[];
     src/func_ov030_02113b38.cpp     extern int data_ov030_02115d18[];
     src/func_ov030_02113ff0.cpp     the same symbol again (LNK2001)
     src/_ZN13RollingLogTtm13InitResourcesEv.cpp  extern char data_ov002_*;
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

int func_ov030_02114124(void *c);
int func_ov030_02113ff0(void *c);
int func_ov030_02113fd8(void *c);
int func_ov030_02113d20(void *c);
int func_ov030_02113be8(void *c);
int func_ov030_02113b38(void *c);
int func_ov030_02113a80(void *c);
/* state 3's tick half, func_ov030_021136b0, HAS NO BODY -- faced below */
int func_ov030_0211360c(void *c);
int func_ov030_02113324(void *c);
int func_ov030_021132d4(void *c);
int func_ov030_02113094(void *c);
int func_ov030_02112ff8(void *c);
int func_ov030_02112da0(void *c);
int func_ov030_02112c14(void *c);
int func_ov030_02112a84(void *c);
int func_ov030_02112a14(void *c);
int func_ov030_02112578(void *c);
int func_ov030_02112560(void *c);
int func_ov030_02112400(void *c);
int func_ov030_021123a4(void *c);
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
static int __fastcall ov30_trap13(void *s, void *) { ov30_trap_report(s, 13, "vtable slot 13 ActorBase::Virtual34(u32,u32)"); return 0; }
static int __fastcall ov30_trap14(void *s, void *) { ov30_trap_report(s, 14, "vtable slot 14 ActorBase::Virtual38(u32,u32)"); return 0; }
static int __fastcall ov30_trap30(void *s, void *) { ov30_trap_report(s, 30, "vtable slot 30 Actor::OnAimedAtWithEggReturnVec (SRET)"); return 0; }

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
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov30_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov30_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov30_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov30_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov30_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov30_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov30_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov30_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov30_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov30_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov30_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov30_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov30_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov30_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov30_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov30_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov30_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov30_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov30_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov30_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* Slot 12's ROM word on both Platform-derived tables is 0x02043ac0,
   ActorBase::OnPendingDestroy -- an empty 4-byte body. The port links it as an
   MSVC METHOD (?OnPendingDestroy@ActorBase@@UAEXXZ), never under the Itanium
   C name, so it is reached through the class exactly as hal/actor_classes.cpp's
   own ac_pdes_base does, and it returns void. */
static void __fastcall ov30_pdes_default(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); }
static int __fastcall ov30_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

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
    vt[30] = (void *)ov30_trap30;   /* Actor::OnAimedAtWithEggReturnVec, SRET */
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

static const struct { unsigned enter_rom, tick_rom; PortUkikiFn enter_host, tick_host; }
g_ukiki_cells[11] = {
    { 0x02114124, 0x02113ff0, (PortUkikiFn)func_ov030_02114124, (PortUkikiFn)func_ov030_02113ff0 },
    { 0x02113fd8, 0x02113d20, (PortUkikiFn)func_ov030_02113fd8, (PortUkikiFn)func_ov030_02113d20 },
    { 0x02113be8, 0x02113b38, (PortUkikiFn)func_ov030_02113be8, (PortUkikiFn)func_ov030_02113b38 },
    { 0x02113a80, 0x021136b0, (PortUkikiFn)func_ov030_02113a80, ukiki_state3_tick },
    { 0x0211360c, 0x02113324, (PortUkikiFn)func_ov030_0211360c, (PortUkikiFn)func_ov030_02113324 },
    { 0x021132d4, 0x02113094, (PortUkikiFn)func_ov030_021132d4, (PortUkikiFn)func_ov030_02113094 },
    { 0x02112ff8, 0x02112da0, (PortUkikiFn)func_ov030_02112ff8, (PortUkikiFn)func_ov030_02112da0 },
    { 0x02112c14, 0x02112a84, (PortUkikiFn)func_ov030_02112c14, (PortUkikiFn)func_ov030_02112a84 },
    { 0x02112a14, 0x02112578, (PortUkikiFn)func_ov030_02112a14, (PortUkikiFn)func_ov030_02112578 },
    { 0x02112560, 0x02112400, (PortUkikiFn)func_ov030_02112560, (PortUkikiFn)func_ov030_02112400 },
    { 0x021123a4, 0x021122b0, (PortUkikiFn)func_ov030_021123a4, (PortUkikiFn)func_ov030_021122b0 },
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
{ return func_ov030_02111410(s); }
static int __fastcall cage_clean(void *s, void *)
{ return func_ov030_0211130c(s); }
static int __fastcall cage_behavior(void *s, void *)
{ return func_ov030_02111384(s); }
static int __fastcall cage_render(void *s, void *)
{ port_actor_render_probe("UKIKI_CAGE", (char *)s + 0xd4);
  return func_ov030_02111350((char *)s); }
static int __fastcall cage_d1(void *s, void *)
{ return (int)(size_t)func_ov030_021111a0((int *)s); }
static int __fastcall cage_d0(void *s, void *)
{ return (int)(size_t)func_ov030_021111ec((int *)s); }

extern "C" void hal_fill_ukikicage_vtable(void)
{
    void **vt = (void **)_ZTV13daObjHmBskt_c;
    ov30_fill_shared(vt);
    vt[0]  = (void *)cage_init;
    vt[3]  = (void *)cage_clean;
    vt[6]  = (void *)cage_behavior;
    vt[9]  = (void *)cage_render;
    vt[12] = (void *)ov30_pdes_default;  /* ROM takes ActorBase's 0x02043ac0 */
    vt[16] = (void *)cage_d1;
    vt[17] = (void *)cage_d0;
    vt[31] = (void *)ov30_kill;          /* Platform::Kill, the 32nd slot */
}

// ============================================================================
// RollingLogTtm (102, 15daObjHmMaruta_c) -- Platform-derived, 32 slots.
// Every _ZN9UkikiCage*Ev below is one of THIS class's methods.
// ============================================================================
static int __fastcall log_init(void *s, void *)
{ return _ZN9UkikiCage13InitResourcesEv(s); }
static int __fastcall log_clean(void *s, void *)
{ return _ZN9UkikiCage16CleanupResourcesEv(s); }
static int __fastcall log_behavior(void *s, void *)
{ return _ZN9UkikiCage8BehaviorEv(s); }
/* Slot 9 comes from ov080, not from ov030 and not from arm9. It is the same
   six-virtual plain-Model shadow src/func_ov030_02111350.cpp uses, calling
   index 5, which hal/cxxname_bridge.cpp:511 dual-fills as Render. */
static int __fastcall log_render(void *s, void *)
{ port_actor_render_probe("ROLLING_LOG_TTM", (char *)s + 0xd4);
  return func_ov080_02127124(s); }
/* Slot 27 also comes from ov080: OnHitByMegaChar(Player&), one stack argument. */
static int __fastcall log_mega(void *s, void *, void *p)
{ func_ov080_02127058(s, p); return 0; }
static int __fastcall log_d1(void *s, void *)
{ return (int)(size_t)_ZN9UkikiCageD1Ev((int *)s); }
static int __fastcall log_d0(void *s, void *)
{ return (int)(size_t)_ZN9UkikiCageD0Ev((int *)s); }

extern "C" void hal_fill_rollinglogttm_vtable(void)
{
    void **vt = (void **)_ZTV15daObjHmMaruta_c;
    ov30_fill_shared(vt);
    vt[0]  = (void *)log_init;
    vt[3]  = (void *)log_clean;
    vt[6]  = (void *)log_behavior;
    vt[9]  = (void *)log_render;         /* ov080 override */
    vt[12] = (void *)ov30_pdes_default;  /* ROM takes ActorBase's 0x02043ac0 */
    vt[16] = (void *)log_d1;
    vt[17] = (void *)log_d0;
    vt[27] = (void *)log_mega;           /* ov080 override, NOT Actor's default */
    vt[31] = (void *)ov30_kill;          /* Platform::Kill, the 32nd slot */
}

// ============================================================================
// The Ukiki (267 UkikiThief + 268 UkikiStar, 7daMky_c) -- plain Actor, 31
// slots. Every _ZN13RollingLogTtm*Ev below is one of THIS class's methods.
// ============================================================================
static int __fastcall mky_init(void *s, void *)
{ return _ZN13RollingLogTtm13InitResourcesEv(s); }
static int __fastcall mky_clean(void *s, void *)
{ (void)s; return _ZN13RollingLogTtm16CleanupResourcesEv(); }
static int __fastcall mky_behavior(void *s, void *)
{ return port_ov030_ukiki_behavior(s); }
static int __fastcall mky_render(void *s, void *)
{ port_actor_render_probe("UKIKI", (char *)s + 0xd4);
  return _ZN13RollingLogTtm6RenderEv(s); }
static int __fastcall mky_pdes(void *s, void *)
{ (void)s; _ZN13RollingLogTtm16OnPendingDestroyEv(); return 0; }
/* Slot 16, D1: src/_ZN13RollingLogTtmD1Ev.cpp is a real MSVC-synthesised
   destructor over a local shadow class with NO vtable store of its own, so it
   is kept OUT of the slice (MSVC would mangle it ?1RollingLogTtm@@UAE@XZ and
   the Itanium name the slot needs would never exist). The MotherPenguin /
   OneUpLogo / BabyPenguin shape: chain spelled directly, HIGH ADDRESS FIRST,
   from that file's own member list -- WithMeshClsn +0x194, MovingCylinderClsn
   +0x160, ShadowModel +0x138, ModelAnim +0xd4, then Actor's own D2. */
static int __fastcall mky_d1(void *s, void *)
{
    char *t = (char *)s;
    _ZN12WithMeshClsnD1Ev(t + 0x194);
    _ZN18MovingCylinderClsnD1Ev(t + 0x160);
    _ZN11ShadowModelD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall mky_d0(void *s, void *)
{ return (int)(size_t)_ZN13RollingLogTtmD0Ev((int *)s); }
static int __fastcall mky_yoshi(void *s, void *)
{ (void)s; return func_ov030_0211172c(); }
/* Slot 19 is OnTurnIntoEgg(Player &player): the caller pushes the player, so
   the thunk needs the third parameter to pop it even though the ov030 body
   takes nothing of its own. The BabyPenguin slot-19 reasoning applies
   unchanged. */
static int __fastcall mky_egg(void *s, void *, void *)
{ func_ov030_021145d4(s); return 0; }

extern "C" void hal_fill_ukiki_vtable(void)
{
    /* Seat and verify the eleven PMF cells BEFORE anything can dispatch through
       them: func_ov030_021141a8 installs a cell and TAIL-JUMPS straight into
       the enter half, and InitResources reaches it on the first frame. */
    port_ukiki_states_seat();
    void **vt = (void **)_ZTV13RollingLogTtm;
    ov30_fill_shared(vt);
    vt[0]  = (void *)mky_init;
    vt[3]  = (void *)mky_clean;
    vt[6]  = (void *)mky_behavior;
    vt[9]  = (void *)mky_render;
    vt[12] = (void *)mky_pdes;   /* own body, overrides ActorBase's default */
    vt[16] = (void *)mky_d1;
    vt[17] = (void *)mky_d0;
    vt[18] = (void *)mky_yoshi;  /* own OnYoshiTryEat, overrides the shared default */
    vt[19] = (void *)mky_egg;    /* own OnTurnIntoEgg, overrides the shared default */
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}

// ---- method faces ----------------------------------------------------------
// src/_ZN13RollingLogTtm13InitResourcesEv.cpp and _ZN13RollingLogTtm6RenderEv
// .cpp are real MSVC methods against include/RollingLogTtm.h
// (?InitResources@RollingLogTtm@@..., not __ZN13RollingLogTtm...), so the
// Itanium names the vtable slots need are faced here -- the IceSheet /
// OneUpLogo / BabyPenguin recipe.
// The Ukiki's Behavior is NOT faced: it is the HOST COPY in
// port/unmatched/Ukiki_Behavior.cpp, which defines the Itanium name directly
// (the ModelAnim slot-3 shadow, see that file's header).
// _ZN13RollingLogTtm16CleanupResourcesEv and _ZN13RollingLogTtm16OnPendingDestroyEv
// are plain C-linkage .c bodies taking (void) -- no face needed, declared
// extern "C" above and called directly.
extern "C" {
int _ZN13RollingLogTtm13InitResourcesEv(void *self)
{ return ((RollingLogTtm *)self)->RollingLogTtm::InitResources(); }
int _ZN13RollingLogTtm6RenderEv(void *self)
{ return ((RollingLogTtm *)self)->RollingLogTtm::Render(); }
}
