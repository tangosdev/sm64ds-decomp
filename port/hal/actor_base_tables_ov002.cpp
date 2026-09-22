/* SIX ov002 ActorBase-shaped vtables the port hosts NOWHERE -- run link100,
 * lane OV6, gate 224.
 *
 * WHAT THIS IS. port/CMakeLists.txt has carried the sentence "that intermediate
 * base table is hosted nowhere in this port -- a table to host, not a rename"
 * about _ZTV17daObjFloatBoard_c since the gate-216 rename block was written, and
 * the ov029 binding block next to it adds "the ROM's own ov002 base tables
 * (_ZTV17daObjFloatBoard_c/021091d4) are NOT host arrays, so a dead store is never
 * bound to them". One of the two named there (0x021091d4, daObjKaitendai_c)
 * was hosted by hal/actor_classes_ov036.cpp; the other five plus 0x02108284
 * were not. This file hosts all six and seats the ROM's own words in them.
 *
 * THE MECHANISM IS hal/w2_dtor_heads.cpp's, not a new one: release links with
 * /OPT:REF, so a matched TU on a slice line with nothing referencing it is
 * dropped before the map is written. A ROM vtable slot the port never filled
 * is the honest place to put a reference edge, because the ROM says what
 * belongs there. The ov036 block this file copies its shape from spells the
 * behavioural half: an abstract intermediate installed only between two member
 * teardowns is never dispatched, so seating it changes no behaviour and makes
 * the port's copy of each table carry the words the ROM puts there.
 *
 * ---- THE SIX TABLES, AND HOW EACH ONE WAS SIZED ---------------------------
 *
 * Width is the NEXT-SYMBOL DELTA out of config/arm9/overlays/ov002/symbols.txt,
 * divided by four -- never a guess and never the reloc run's length:
 *
 *   0x02108284 -> next data_ov002_02108300   0x7c   31 slots
 *   0x02108fdc -> next _ZTI15daObjGuragura_c   0x80   32 slots
 *   0x02109084 -> next _ZTI14daObjUkiyuka_c   0x80   32 slots
 *   0x0210912c -> next _ZTI16daObjKaitendai_c   0x80   32 slots
 *   0x021096b0 -> next data_ov002_02109730   0x80   32 slots
 *   0x021099e4 -> next data_ov002_02109a64   0x80   32 slots
 *
 * All six carry the fBase_c SIGNATURE at slots 13/14/15 -- 0x0204357c,
 * 0x0204349c, 0x02043494 -- which is what says they are vtables rather than
 * pointer-to-member runs or file tables. Read out of
 * config/arm9/overlays/ov002/relocs.txt, the same three words
 * port/mg_fanout_costs.txt section 3 uses to find a Scene table in ov006.
 *
 * FIVE OF THE SIX ARE Platform-DERIVED: slot 31 is 0x020ee55c,
 * _ZN10dBgActor_c4KillEv, on 02108fdc / 02109084 / 0210912c / 021099e4, and
 * 021096b0 overrides 31 with its own body. 02108284 is 31 slots and has no
 * slot 31 at all -- it is CapEnemy's, an dActor_c and not a Platform.
 *
 * ---- WHAT EACH TABLE'S OWN SLOTS HOLD, BY ROM ADDRESS ---------------------
 *
 * Every row below is `config/arm9/overlays/ov002/relocs.txt from:base+4*slot`
 * resolved through that overlay's symbols.txt. Nothing here is read off a
 * config class NAME: the tree's class names are shifted and the addresses are
 * not.
 *
 *   0x02108284  (CapEnemy, 31)
 *     0  0x02043c80  fBase_c::InitResources        already linked
 *     3  0x02043bf0  fBase_c::CleanupResources     already linked
 *     6  0x02043b24  fBase_c::Behavior             already linked
 *     9  0x02043af0  fBase_c::Render               already linked
 *    16  0x0200651c  _ZN11dCapEnemy_cD1Ev    arm9        SEATED, was unlinked
 *    17  0x020aedf4  _ZN11dCapEnemy_cD0Ev    ov002       SEATED, was unlinked
 *
 *   0x02108fdc  (BlueFlame/RedFlame's Platform intermediate, 32)
 *     0  no relocation -- the ROM parks zero              trap
 *     3  0x020b5be0  _ZN17daObjFloatBoard_c16CleanupResourcesEv             already linked
 *     6  0x020b5c4c  _ZN17daObjFloatBoard_c8BehaviorEv             already linked
 *     9  0x020b5c24  _ZN17daObjFloatBoard_c6RenderEv             HOST body, see below
 *    16  0x020b5a70  _ZN17daObjFloatBoard_cD1Ev             SEATED, was unlinked
 *    17  0x020b5a18  _ZN17daObjFloatBoard_cD0Ev             SEATED, was unlinked
 *    31  0x020ee55c  Platform::Kill                  already linked
 *
 *   0x02109084  (32)
 *     0, 3  no relocation                                 trap
 *     6  0x020b616c  _ZN15daObjGuragura_c8BehaviorEv             already linked
 *     9  0x020b6144  _ZN15daObjGuragura_c6RenderEv             already linked
 *    16  0x020b6030  _ZN15daObjGuragura_cD1Ev             SEATED, was unlinked
 *    17  0x020b5fd8  _ZN15daObjGuragura_cD0Ev             SEATED, was unlinked
 *    31  0x020ee55c  Platform::Kill                  already linked
 *
 *   0x0210912c  (32)
 *     0, 3  no relocation                                 trap
 *     6  0x020b6494  _ZN14daObjUkiyuka_c8BehaviorEv             already linked
 *     9  0x020b646c  _ZN14daObjUkiyuka_c6RenderEv             already linked
 *    16  0x020b63e0  _ZN14daObjUkiyuka_cD1Ev             SEATED, was unlinked
 *    17  0x020b6388  _ZN14daObjUkiyuka_cD0Ev             SEATED, was unlinked
 *    31  0x020ee55c  Platform::Kill                  already linked
 *
 *   0x021096b0  (_ZTV16daObjPushblock_c, 32)
 *     0  0x020b8fe0   3  0x020b8d68   6  0x020b8dd4   9  0x020b8dac
 *    17  0x020b8c3c  25  0x020b8d3c  27  0x020b8d14
 *        SEVEN MARKER-CARRYING BODIES. They trapped when this file was written,
 *        because the marker "recovered from vtable slot identity" was the only
 *        thing known about them and port/tools/inferred_stub_guard.py refuses
 *        to seat a guess. Gate 226 (run link100 lane STUBADJ) ruled all seven
 *        against the ROM by address and reproduced them byte for byte with
 *        tools/match.py at 2004/b56 --strict-relocs, so they are SEATED now and
 *        the traps are gone. The marker recorded how each NAME was recovered,
 *        not where its BODY came from -- slot 17 is the clearest case: src
 *        calls it OnYoshiTryEat and config calls the same address
 *        _ZN16daObjPushblock_cD0Ev, and the body is the D0.
 *    16  0x020b8bf0  _ZN16daObjPushblock_cD1Ev             SEATED, was unlinked
 *    31  0x020b8c9c  _ZN16daObjPushblock_c4KillEv             SEATED, was unlinked
 *
 *   0x021099e4  (ShutterBob's Platform intermediate, 32)
 *     0, 3, 6  no relocation                              trap
 *     9  0x020babf0  _ZN13daObjSwdoor_c6RenderEv             already linked
 *    16  0x020bab64  _ZN13daObjSwdoor_cD1Ev             SEATED, was unlinked
 *    17  0x020bab0c  _ZN13daObjSwdoor_cD0Ev             SEATED, was unlinked
 *    31  0x020ee55c  Platform::Kill                  already linked
 *
 * ---- ONE SLOT IS A HOST BODY AND IS REFERENCED, NOT REDEFINED -------------
 *
 * 0x02108fdc slot 9 is _ZN17daObjFloatBoard_c6RenderEv, and port/unmatched/Jrb_Renders.cpp
 * already defines that symbol under a
 * `PORT_HOST_ABI: ROM-order model slot-5 dispatch, the RotatingFirebar case`
 * tag. That is the collision class every fold of this campaign has hit: the
 * slice line for src/actors/daObjFloatBoard_c.cpp was in this gate's first draft,
 * port/tools/closure.py named it as a FOREIGN-object LNK2005 candidate against
 * Jrb_Renders.cpp.obj, and it came back out. The slot below calls the host
 * body by name, so the ROM's word is honoured and the tagged stand-in is what
 * runs -- which is what the tag rules. The seat therefore buys no linkage
 * count on that slot and this file does not claim one.
 *
 * ---- THE DESTRUCTORS' OWN vptr STORES, AND WHY EACH NEEDS A -D ------------
 *
 * All nine ov002 destructor bodies in gate 224 were recovered through a SHARED
 * HEADER, so they spell their two vptr stores with the placeholder names
 * _ZTV10dBgActor_c (and _ZTV16daObjPushblock_c on 020b8bf0) and VT1. Those
 * names mean a different table in every TU -- the ov065 disease the ov029
 * binding block in port/CMakeLists.txt documents -- so each source gets its own
 * COMPILE_DEFINITIONS row read out of ITS OWN literal pool. Measured, with
 * port/tools relocation data, not taken from the recovery's names:
 *
 *   _ZN17daObjFloatBoard_cD0Ev  0x020b5a64 -> 0x02108fdc   0x020b5a68 -> 0x0210ae38
 *   _ZN17daObjFloatBoard_cD1Ev  0x020b5aac -> 0x02108fdc   0x020b5ab0 -> 0x0210ae38
 *   _ZN15daObjGuragura_cD0Ev  0x020b6024 -> 0x02109084   0x020b6028 -> 0x0210ae38
 *   _ZN15daObjGuragura_cD1Ev  0x020b606c -> 0x02109084   0x020b6070 -> 0x0210ae38
 *   _ZN14daObjUkiyuka_cD0Ev  0x020b63d4 -> 0x0210912c   0x020b63d8 -> 0x0210ae38
 *   _ZN14daObjUkiyuka_cD1Ev  0x020b641c -> 0x0210912c   0x020b6420 -> 0x0210ae38
 *   _ZN16daObjPushblock_cD1Ev  0x020b8c34 -> 0x021096b0   0x020b8c38 -> 0x0210ae38
 *   _ZN13daObjSwdoor_cD0Ev  0x020bab58 -> 0x021099e4   0x020bab5c -> 0x0210ae38
 *   _ZN13daObjSwdoor_cD1Ev  0x020baba0 -> 0x021099e4   0x020baba4 -> 0x0210ae38
 *
 * 0x0210ae38 is _ZTV10dBgActor_c on every one of the nine. The rule the ov029
 * block states is applied unchanged: the LIVE (last) store gets its true
 * target, and the DEAD earlier store is bound to the class's own array -- which
 * this file is what makes definable at all.
 *
 * ---- HOW THE FILL GETS CALLED --------------------------------------------
 *
 * The same namespace-scope object hal/w2_dtor_heads.cpp uses, and for the same
 * reason: the seats every wave-1 lane landed are called by name from
 * hal/level_boot.cpp, this lane does not own that file, and the campaign's rule
 * is that a lane never edits another lane's file. A pre-main write is only safe
 * for a table no runtime fill also writes, and these six are written by nothing
 * else in the port -- that is the measurement that made them work in the first
 * place. The fill keeps external C linkage so a later change that does own
 * level_boot.cpp can call it explicitly and drop the object.
 */
#include "port_d16.h"

#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"
#include "dActor_c.h"
#include "fBase_c.h"

extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);

/* GATE 226: two C++ SPELLINGS of ov002 mount symbols.
 *
 * src/game/actors/daObjPushblock_c.cpp is a C++ TU and declares the two SharedFilePtr
 * its slot-0 body loads as
 *
 *     extern SharedFilePtr data_ov002_0210df9c;
 *     extern SharedFilePtr data_ov002_0210df94;
 *
 * with C++ linkage, so MSVC emits references to ?data_ov002_0210df9c@@
 * 3USharedFilePtr@@A. The ovdata mount from port/ov002_syms.txt publishes the
 * C name. Both are the same DS address (0x0210df9c / 0x0210df94, the first two
 * pool words of 0x020b8fe0 in config/arm9/overlays/ov002/relocs.txt) and the
 * mount is the only definition, so an /alternatename routes the C++ spelling
 * onto it -- the same shape hal/actor_classes_bob_enemy.cpp uses for ov102's
 * SharedFilePtr cells and hal/actor_classes.cpp for data_ov002_0210d9a8.
 *
 * These two directives are LIVE, not decorative: their LHS is referenced by
 * the object above and nothing else defines it, which is the condition
 * tools/alternatename_guard.py checks (LHS and RHS must land at ONE address in
 * the map). The C-named slot-3 body reaches the same two cells through gate
 * 226's -D rows and does not need an alias. */
#pragma comment(linker, "/alternatename:?data_ov002_0210df9c@@3USharedFilePtr@@A=_data_ov002_0210df9c")
#pragma comment(linker, "/alternatename:?data_ov002_0210df94@@3USharedFilePtr@@A=_data_ov002_0210df94")

extern "C" {
/* the arm9 shared half; every address read off ov002's own reloc runs */
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
void _ZN8dActor_c15OnHitByMegaCharER6Player(void *self, void *p); /* slot 27 */
void _ZN8dActor_c19OnHitFromUnderneathERS_(void *self, void *o);  /* slot 28 */
int _ZN8dActor_c16OnAimedAtWithEggEv(void *self);                 /* slot 29 */
void _ZN10dBgActor_c4KillEv(void *self);                          /* slot 31 */

/* ActorBase's own InitResources, 0x02108284 slot 0. The port already carries
   the Itanium C name for this one (hal/scene_boot.cpp's sc_base_init reaches
   it); slots 3, 6 and 9 have no C-named face anywhere in the link, so those
   three go through the MSVC method below instead of an extern that would be
   an LNK2019. Measured: the first link of this gate named exactly those
   three and not this one. */
int _ZN7fBase_c13InitResourcesEv(void *self);

/* the per-table own bodies, flat C, receiver as an ordinary first argument */
void *_ZN11dCapEnemy_cD1Ev(void *self);          /* 02108284 slot 16 */
int *_ZN11dCapEnemy_cD0Ev(int *self);            /* 02108284 slot 17 */
int _ZN17daObjFloatBoard_c16CleanupResourcesEv(char *self);         /* 02108fdc slot 3  */
int _ZN17daObjFloatBoard_c8BehaviorEv(char *self);         /* 02108fdc slot 6  */
int _ZN17daObjFloatBoard_c6RenderEv(void *self);         /* 02108fdc slot 9, HOST body */
int *_ZN17daObjFloatBoard_cD1Ev(int *self);         /* 02108fdc slot 16 */
int *_ZN17daObjFloatBoard_cD0Ev(int *self);         /* 02108fdc slot 17 */
int _ZN15daObjGuragura_c8BehaviorEv(char *self);         /* 02109084 slot 6  */
int _ZN15daObjGuragura_c6RenderEv(void *self);         /* 02109084 slot 9  */
int *_ZN15daObjGuragura_cD1Ev(int *self);         /* 02109084 slot 16 */
int *_ZN15daObjGuragura_cD0Ev(int *self);         /* 02109084 slot 17 */
int _ZN14daObjUkiyuka_c8BehaviorEv(char *self);         /* 0210912c slot 6  */
int _ZN14daObjUkiyuka_c6RenderEv(void *self);         /* 0210912c slot 9  */
int *_ZN14daObjUkiyuka_cD1Ev(int *self);         /* 0210912c slot 16 */
int *_ZN14daObjUkiyuka_cD0Ev(int *self);         /* 0210912c slot 17 */
int *_ZN16daObjPushblock_cD1Ev(int *self);         /* 021096b0 slot 16 */
void _ZN16daObjPushblock_c4KillEv(char *self);        /* 021096b0 slot 31 */
/* gate 226: the seven ruled bodies of 0x021096b0 */
int _ZN16daObjPushblock_c13InitResourcesEv(char *self);         /* 021096b0 slot 0  */
int _ZN16daObjPushblock_c16CleanupResourcesEv(void *self);         /* 021096b0 slot 3  */
int _ZN16daObjPushblock_c8BehaviorEv(char *self);         /* 021096b0 slot 6  */
int _ZN16daObjPushblock_c6RenderEv(void *self);         /* 021096b0 slot 9  */
int *_ZN16daObjPushblock_cD0Ev(int *self);         /* 021096b0 slot 17, the D0 */
void _ZN16daObjPushblock_c8OnPushedER8dActor_c(char *self, char *other);   /* slot 25 OnPushed  */
void _ZN16daObjPushblock_c15OnHitByMegaCharER6Player(void *self, void *player);  /* slot 27 OnHitByMegaChar.
     src spells the second parameter `int`; the ROM passes a Player* in r1
     (mov r0,r1 then bl Player::IncMegaKillCount at 0x020b8d20), so the
     declaration here carries the pointer. Same width, same ABI slot. */
int _ZN13daObjSwdoor_c6RenderEv(void *self);         /* 021099e4 slot 9  */
int *_ZN13daObjSwdoor_cD1Ev(int *self);         /* 021099e4 slot 16 */
int *_ZN13daObjSwdoor_cD0Ev(int *self);         /* 021099e4 slot 17 */
}

/* THE SIX HOST ARRAYS. Their names are DS-shaped (data_ov002_*), so they have
   to sit inside the .dsstate span a save state captures or dsstate_guard fails
   the build -- the same reading hal/actor_classes_ov036.cpp records for the
   two intermediates it hosts. `void *` and not `int` so the fill can bind them
   through a `void **tabs[]` without a cast (vtspan --fills resolves that
   spelling and reports "table binding not modelled" for a cast). */
/* 0x02108284 IS ALREADY MOUNTED and must not be defined here. gate 32 put it
   in port/ov002_syms.txt ("the cap tier's own storage"), so the generated
   ov002 mount emits `u8 data_ov002_02108284[124]` carrying the ROM's own
   thirty-one raw words -- and defining a second array LNK2005'd against
   ov002_data.c.obj on this gate's first link, which is how the mount was
   found. Its fill therefore OVERWRITES the mounted DS addresses in place,
   the way every fill over a mounted scene table does, and its width comes
   from the mount's own 124 bytes as well as the symbols.txt delta. */
extern "C" unsigned char data_ov002_02108284[];

extern "C" {
DSSTATE_BEGIN
void *_ZTV17daObjFloatBoard_c[32];  /* vtspan: flame Platform,    ov002 0x02108fdc */
void *_ZTV15daObjGuragura_c[32];  /* vtspan: ov002 0x02109084 */
void *_ZTV14daObjUkiyuka_c[32];  /* vtspan: ov002 0x0210912c */
void *_ZTV16daObjPushblock_c[32];  /* vtspan: daObjPushblock_c,  ov002 0x021096b0 */
/* SYNC4: src/game/actors/daObjPushblock_c.cpp reaches this table through the
   VT0 placeholder, which include/decl_common.h declares `int VT0[]`. Binding
   VT0 to the array's own name now collides with main's own `void *[]`
   declaration of it, so the TU binds VT0 to the spelling below and the alias
   lands it on this object. */
#pragma comment(linker, "/alternatename:_port_vt_daObjPushblock_c=__ZTV16daObjPushblock_c")
void *_ZTV13daObjSwdoor_c[32];  /* vtspan: ShutterBob base,   ov002 0x021099e4 */
DSSTATE_END
}
/* src/game/actors/daObjPushblock_c.cpp spells its own table by the config's RTTI name
   _ZTV16daObjPushblock_c. That is a PLACEHOLDER, not a second object: the
   -D row for that source in port/CMakeLists.txt binds the name to the array
   above, so no alias is needed and none is added -- an /alternatename whose
   LHS is never referenced is exactly the shape tools/alternatename_guard.py
   is there to keep out of the tree. */

// ---- traps -----------------------------------------------------------------
//
// A slot the ROM leaves at zero, and every slot whose ROM word names a GUESSED
// body. Both report which slot and stop, rather than calling address zero or
// running a body no ROM ever built.
static void ob2_trap_report(void *self, int slot, const char *what)
{
    std::fprintf(stderr,
                 "  UNHOSTED: ov002 base-table slot %d is not hosted (%s, "
                 "object %p)\n", slot, what, self);
    std::fflush(stderr);
    if (std::getenv("SM64DS_FAULTS_FATAL"))
        std::exit(3);
}
static int __fastcall ob2_trap0(void *s, void *)
{ ob2_trap_report(s, 0, "abstract base InitResources, ROM word is zero"); return 0; }
static int __fastcall ob2_trap3(void *s, void *)
{ ob2_trap_report(s, 3, "abstract base CleanupResources, ROM word is zero"); return 0; }
static int __fastcall ob2_trap6(void *s, void *)
{ ob2_trap_report(s, 6, "abstract base Behavior, ROM word is zero"); return 0; }
static int __fastcall ob2_trap13(void *s, void *)
{ ob2_trap_report(s, 13, "fBase_c::Virtual34, not linked"); return 0; }
static int __fastcall ob2_trap14(void *s, void *)
{ ob2_trap_report(s, 14, "fBase_c::Virtual38, not linked"); return 0; }
/* The seven slots that used to trap here. Gate 226 (run link100 lane STUBADJ)
   ruled every one of them against the ROM -- read at its ADDRESS out of
   config/arm9/overlays/ov002/relocs.txt and symbols.txt, confirmed a real
   function entry and not an interior address, then reproduced byte for byte by
   tools/match.py at 2004/b56 with --strict-relocs. They are decompiled bodies
   whose NAME was recovered from the vtable slot, not guessed bodies, so the
   traps are gone and the faces below take their place. The rulings are in
   port/tools/inferred_stub_adjudicated.txt and inferred_stub_guard.py accepts
   them from there; nothing was added to the baseline or the debt queue. */

// ---- the shared 1..30 half -------------------------------------------------
//
// Slots 1, 2, 4, 5, 7, 8, 10, 11, 12, 15 and 18..30 hold the SAME arm9 words on
// all six tables; verified per table against ov002's own relocation run, which
// is what makes one shared fill legitimate here. 13 and 14 trap for the reason
// hal/actor_classes_ov036.cpp gives: their matched TUs are not in the link.
static int __fastcall ob2_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall ob2_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall ob2_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall ob2_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall ob2_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall ob2_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall ob2_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall ob2_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall ob2_pdes(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); return 0; }
static int __fastcall ob2_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall ob2_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops. */
static int __fastcall ob2_turn_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ob2_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall ob2_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ob2_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ob2_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ob2_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall ob2_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall ob2_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ob2_mega(void *s, void *, void *p)
{ _ZN8dActor_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ob2_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ob2_aimed(void *s, void *)
{ return _ZN8dActor_c16OnAimedAtWithEggEv(s); }
static int __fastcall ob2_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }

static void ob2_fill_shared(void **vt)
{
    vt[1]  = (void *)ob2_binit;     /* Actor::BeforeInitResources        */
    vt[2]  = (void *)ob2_ainit;     /* Actor::AfterInitResources(u32)    */
    vt[4]  = (void *)ob2_bclean;    /* Actor::BeforeCleanupResources     */
    vt[5]  = (void *)ob2_aclean;    /* Actor::AfterCleanupResources(u32) */
    vt[7]  = (void *)ob2_bbeh;      /* Actor::BeforeBehavior             */
    vt[8]  = (void *)ob2_abeh;      /* Actor::AfterBehavior(u32)         */
    vt[10] = (void *)ob2_bren;      /* Actor::BeforeRender               */
    vt[11] = (void *)ob2_aren;      /* Actor::AfterRender(u32)           */
    vt[12] = (void *)ob2_pdes;      /* ActorBase::OnPendingDestroy       */
    vt[13] = (void *)ob2_trap13;    /* ActorBase::Virtual34(u32,u32)     */
    vt[14] = (void *)ob2_trap14;    /* ActorBase::Virtual38(u32,u32)     */
    vt[15] = (void *)ob2_heap;      /* ActorBase::OnHeapCreated          */
    vt[18] = (void *)ob2_yoshi;     /* Actor::OnYoshiTryEat              */
    vt[19] = (void *)ob2_turn_egg;  /* Actor::OnTurnIntoEgg(Player&)     */
    vt[20] = (void *)ob2_v50;       /* Actor::Virtual50                  */
    vt[21] = (void *)ob2_pounded;   /* Actor::OnGroundPounded(Actor&)    */
    vt[22] = (void *)ob2_atk1;      /* Actor::OnAttacked1(Actor&)        */
    vt[23] = (void *)ob2_atk2;      /* Actor::OnAttacked2(Actor&)        */
    vt[24] = (void *)ob2_kicked;    /* Actor::OnKicked(Actor&)           */
    vt[25] = (void *)ob2_pushed;    /* Actor::OnPushed(Actor&)           */
    vt[26] = (void *)ob2_cannon;    /* Actor::OnHitByCannonBlastedChar   */
    vt[27] = (void *)ob2_mega;      /* Actor::OnHitByMegaChar(Player&)   */
    vt[28] = (void *)ob2_under;     /* Actor::OnHitFromUnderneath        */
    vt[29] = (void *)ob2_aimed;     /* Actor::OnAimedAtWithEgg           */
    vt[30] = (void *)port_actor_s30_base;  /* ...ReturnVec, SRET         */
}

// ---- the six tables' own slots ---------------------------------------------
static int __fastcall ob2_ab_init(void *s, void *)
{ return _ZN7fBase_c13InitResourcesEv(s); }
static int __fastcall ob2_ab_clean(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::CleanupResources(); }
static int __fastcall ob2_ab_beh(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::Behavior(); }
static int __fastcall ob2_ab_render(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::Render(); }
static void *__fastcall ob2_cap_d2(void *s, void *)
{ return _ZN11dCapEnemy_cD1Ev(s); }
static void *__fastcall ob2_cap_d0(void *s, void *)
{ return (void *)_ZN11dCapEnemy_cD0Ev((int *)s); }

static int __fastcall ob2_fdc_s3(void *s, void *)
{ return _ZN17daObjFloatBoard_c16CleanupResourcesEv((char *)s); }
static int __fastcall ob2_fdc_s6(void *s, void *)
{ return _ZN17daObjFloatBoard_c8BehaviorEv((char *)s); }
static int __fastcall ob2_fdc_s9(void *s, void *)
{ return _ZN17daObjFloatBoard_c6RenderEv(s); }
static void *__fastcall ob2_fdc_d1(void *s, void *)
{ return (void *)_ZN17daObjFloatBoard_cD1Ev((int *)s); }
static void *__fastcall ob2_fdc_d0(void *s, void *)
{ return (void *)_ZN17daObjFloatBoard_cD0Ev((int *)s); }

static int __fastcall ob2_084_s6(void *s, void *)
{ return _ZN15daObjGuragura_c8BehaviorEv((char *)s); }
static int __fastcall ob2_084_s9(void *s, void *)
{ return _ZN15daObjGuragura_c6RenderEv(s); }
static void *__fastcall ob2_084_d1(void *s, void *)
{ return (void *)_ZN15daObjGuragura_cD1Ev((int *)s); }
static void *__fastcall ob2_084_d0(void *s, void *)
{ return (void *)_ZN15daObjGuragura_cD0Ev((int *)s); }

static int __fastcall ob2_12c_s6(void *s, void *)
{ return _ZN14daObjUkiyuka_c8BehaviorEv((char *)s); }
static int __fastcall ob2_12c_s9(void *s, void *)
{ return _ZN14daObjUkiyuka_c6RenderEv(s); }
static void *__fastcall ob2_12c_d1(void *s, void *)
{ return (void *)_ZN14daObjUkiyuka_cD1Ev((int *)s); }
static void *__fastcall ob2_12c_d0(void *s, void *)
{ return (void *)_ZN14daObjUkiyuka_cD0Ev((int *)s); }

static void *__fastcall ob2_6b0_d1(void *s, void *)
{ return (void *)_ZN16daObjPushblock_cD1Ev((int *)s); }
static int __fastcall ob2_6b0_s31(void *s, void *)
{ _ZN16daObjPushblock_c4KillEv((char *)s); return 0; }
/* gate 226. Arity is taken from the ROM slot, not from the C spelling: slots
   0/3/6/9/17 are nullary methods and slots 25/27 take one reference, so the
   faces carry exactly one trailing argument on the last two and none on the
   rest. A face with the wrong arity would smash the stack on its first call. */
static int __fastcall ob2_6b0_s0(void *s, void *)
{ return _ZN16daObjPushblock_c13InitResourcesEv((char *)s); }
static int __fastcall ob2_6b0_s3(void *s, void *)
{ return _ZN16daObjPushblock_c16CleanupResourcesEv(s); }
static int __fastcall ob2_6b0_s6(void *s, void *)
{ return _ZN16daObjPushblock_c8BehaviorEv((char *)s); }
static int __fastcall ob2_6b0_s9(void *s, void *)
{ return _ZN16daObjPushblock_c6RenderEv(s); }
static void *__fastcall ob2_6b0_d0(void *s, void *)
{ return (void *)_ZN16daObjPushblock_cD0Ev((int *)s); }
static int __fastcall ob2_6b0_s25(void *s, void *, void *o)
{ _ZN16daObjPushblock_c8OnPushedER8dActor_c((char *)s, (char *)o); return 0; }
static int __fastcall ob2_6b0_s27(void *s, void *, void *p)
{ _ZN16daObjPushblock_c15OnHitByMegaCharER6Player(s, p); return 0; }

static int __fastcall ob2_9e4_s9(void *s, void *)
{ return _ZN13daObjSwdoor_c6RenderEv(s); }
static void *__fastcall ob2_9e4_d1(void *s, void *)
{ return (void *)_ZN13daObjSwdoor_cD1Ev((int *)s); }
static void *__fastcall ob2_9e4_d0(void *s, void *)
{ return (void *)_ZN13daObjSwdoor_cD0Ev((int *)s); }

extern "C" void hal_seat_ov002_base_tables(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    /* 0x02108284 -- 31 slots, Actor-derived, no slot 31 */
    {
        void **vt = (void **)data_ov002_02108284;
        ob2_fill_shared(vt);
        vt[0]  = (void *)ob2_ab_init;
        vt[3]  = (void *)ob2_ab_clean;
        vt[6]  = (void *)ob2_ab_beh;
        vt[9]  = (void *)ob2_ab_render;
        vt[16] = (void *)PORT_D16(ob2_cap_d2);
        vt[17] = (void *)ob2_cap_d0;
    }

    /* 0x02108fdc */
    {
        void **vt = _ZTV17daObjFloatBoard_c;
        ob2_fill_shared(vt);
        vt[0]  = (void *)ob2_trap0;
        vt[3]  = (void *)ob2_fdc_s3;
        vt[6]  = (void *)ob2_fdc_s6;
        vt[9]  = (void *)ob2_fdc_s9;
        vt[16] = (void *)PORT_D16(ob2_fdc_d1);
        vt[17] = (void *)ob2_fdc_d0;
        vt[31] = (void *)ob2_kill;
    }

    /* 0x02109084 */
    {
        void **vt = _ZTV15daObjGuragura_c;
        ob2_fill_shared(vt);
        vt[0]  = (void *)ob2_trap0;
        vt[3]  = (void *)ob2_trap3;
        vt[6]  = (void *)ob2_084_s6;
        vt[9]  = (void *)ob2_084_s9;
        vt[16] = (void *)PORT_D16(ob2_084_d1);
        vt[17] = (void *)ob2_084_d0;
        vt[31] = (void *)ob2_kill;
    }

    /* 0x0210912c */
    {
        void **vt = _ZTV14daObjUkiyuka_c;
        ob2_fill_shared(vt);
        vt[0]  = (void *)ob2_trap0;
        vt[3]  = (void *)ob2_trap3;
        vt[6]  = (void *)ob2_12c_s6;
        vt[9]  = (void *)ob2_12c_s9;
        vt[16] = (void *)PORT_D16(ob2_12c_d1);
        vt[17] = (void *)ob2_12c_d0;
        vt[31] = (void *)ob2_kill;
    }

    /* 0x021096b0 -- gate 226 seats the seven that used to trap */
    {
        void **vt = _ZTV16daObjPushblock_c;
        ob2_fill_shared(vt);
        vt[0]  = (void *)ob2_6b0_s0;
        vt[3]  = (void *)ob2_6b0_s3;
        vt[6]  = (void *)ob2_6b0_s6;
        vt[9]  = (void *)ob2_6b0_s9;
        vt[16] = (void *)PORT_D16(ob2_6b0_d1);
        vt[17] = (void *)ob2_6b0_d0;
        vt[25] = (void *)ob2_6b0_s25;
        vt[27] = (void *)ob2_6b0_s27;
        vt[31] = (void *)ob2_6b0_s31;
    }

    /* 0x021099e4 */
    {
        void **vt = _ZTV13daObjSwdoor_c;
        ob2_fill_shared(vt);
        vt[0]  = (void *)ob2_trap0;
        vt[3]  = (void *)ob2_trap3;
        vt[6]  = (void *)ob2_trap6;
        vt[9]  = (void *)ob2_9e4_s9;
        vt[16] = (void *)PORT_D16(ob2_9e4_d1);
        vt[17] = (void *)ob2_9e4_d0;
        vt[31] = (void *)ob2_kill;
    }
}

namespace {
struct Ov002BaseTableSeat {
    Ov002BaseTableSeat() { hal_seat_ov002_base_tables(); }
};
Ov002BaseTableSeat g_ov002_base_table_seat;
}

/* ---- GATE 230: the registry's fill hook for actor id 192 -----------------
   _ZTV16daObjPushblock_c is filled above, by hal_seat_ov002_base_tables through
   the static constructor, and gate 226 seated all nine of its own slots with
   no traps left. So id 192 needs no new fill -- it needs the EXISTING one
   reachable from the registry, because the tables live in .dsstate and a
   savestate restore wipes them; every other registered class re-fills from
   its hal_fill_* hook and this one now does too. hal_seat_ov002_base_tables
   writes constants only, so calling it twice is the same as calling it once. */
extern "C" void hal_fill_pushblock192_vtable(void)
{
    hal_seat_ov002_base_tables();
}
