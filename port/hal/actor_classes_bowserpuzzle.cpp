// GATE 179: BOWSER_PUZZLE_MANAGER (79, daObjFl_Puzzle_c) and BOWSER_PUZZLE_PIECE
// (78, daObjFl_Coin_c) -- the two seat classes ov064 carries that gate 177 named
// and deferred ("the BowserPuzzle pair ... follow in their own gates"). ov064 is
// already mounted per symbol (gate 177, port/ov064_syms.txt); this gate adds the
// two classes' own SpawnInfo records, file tables, PMF sources and bss to that
// mount and runs their two sinits. The Bowser puzzle: fourteen PIECES (id 78,
// x14) the MANAGER (id 79, x1) drives.
//
// Same law as hal/actor_classes_ov064.cpp -- ROM slot order, __fastcall thunks
// that call the class's C body or a QUALIFIED method, the shared 0..30 half from
// this file's own helper, slots 13/14 (ActorBase::Virtual34/38, not linked) and
// slot 30 (the SRET OnAimedAtWithEggReturnVec) trapped by name.
//
// ---- BOTH TABLES ARE THE PLAIN 31-SLOT ACTOR SHAPE ------------------------
//
// _ZTV16daObjFl_Puzzle_c (ov064 0x0211c25c, also named _ZTV17BowserPuzzlePiece)
// and _ZTV9JetStream (0x0211c334, also named _ZTV18daWater_Hakidasi_c) are each
// the standard 31-slot Actor table read straight off their
// reloc spans: slot 1 = Actor::BeforeInitResources (arm9 0x02011268), slots 18..29
// the Actor interaction tail, slot 30 = OnAimedAtWithEggReturnVec (0x020100dc).
// Both carry the vtable signature (+4 relocates to 0x02011268) and are HOST arrays
// this file fills -- they stay OUT of the mount, the ov080/ov095/ov071/ov064 rule.
// The Manager overrides 0/3/6/9/16/17 and defaults slot 12 to ActorBase::OnPending-
// Destroy; the Piece overrides 0/3/6/9/12/16/17 (its own OnPendingDestroy body).
//
// ---- THE CONFIG NAMES CROSS THE TWO CLASSES; THE FACTORIES ARE HONEST -----
//
// Read off the spawn chain, not the names. daObjFl_Puzzle_c_classInit (id 78, x14)
// allocates 828 bytes, runs Platform's ctor and installs _ZTV16daObjFl_Puzzle_c
// (0x0211c25c) -- so the class the config calls "Manager" is the PIECE's real
// class: its InitResources reads its model out of the 14-entry SFP-pointer table
// data_ov064_0211adc8[param & 0xf], one model per puzzle piece, and its Behavior
// runs the six-state PMF machine. daObjFl_Coin_c_classInit (id 79, x1) allocates
// a 216-byte COORDINATOR SHELL and installs _ZTV19BowserPuzzleManager, a real 31-slot
// table of its own: slot 0 _ZN19BowserPuzzleManager13InitResourcesEv (three byte-flags), slot 6
// _ZN19BowserPuzzleManager8BehaviorEv (waits for flag 3 + player range), slots 16/17 its D1/D0,
// slots 3/9 the ActorBase defaults. Each factory installs its class's REAL table
// as the last vptr write -- NO reseat wrappers. (An earlier reading called c1d8 a
// placeholder and reseated both vptrs onto the big class; the 216-byte shell then
// ran the 828-byte InitResources, indexed adc8[0xffff & 0xf] into the pack gap
// and faulted on the null SFP.)
//
// _ZTV9JetStream (0x0211c334) is installed at spawn by daWater_Hakidasi_c's own
// factory and mid-teardown by the D-tors that bear its name; it is filled here.
//
// ---- THE D-TORS STAY IN THE SLICE -----------------------------------------
//
// The Manager D1/D0 (.c) spell VT0 = _ZTV16daObjFl_Puzzle_c then VT1 =
// _ZTV10dBgActor_c (Platform's base table, already host-filled by
// hal_fill_platform_vtable), both harmless host writes to a dying object -- the
// FallBlockWf reading. _ZTV16daObjFl_Puzzle_c is aliased below to the real
// Manager host array. The other class's D1/D0 (.c) spell _ZTV9JetStream /
// _ZTV18daWater_Hakidasi_c, which ARE the same address (0x0211c334) and both the
// real JetStream table -- no launder, no placeholder, so they need no host thunk.
// _ZTV17BowserPuzzlePiece is NOT that address: it is 0x0211c25c, the other table
// in this file, and the alias block below is where that is settled. All
// four run their member teardowns (Manager: MovingMeshCollider +0x124, Model +0xd4,
// Actor::D2; Piece: WithMeshClsn +0x144, MovingCylinderClsn +0x110, the Enemy base
// D2 _ZN12dEnemyBase_cD2Ev) with faces already in the build.
//
// ---- THREE PMF DISPATCHERS ARE HOST COPIES, WITH TWO SEATS ----------------
//
// Manager::Behavior, Piece::Behavior and func_ov064_0211982c each form a
// pointer-to-member over a LOCAL or FORWARD-DECLARED struct, so MSVC hands them
// the general (worst-case-inheritance) PMF representation and the dispatch mangles
// this -- the KnockDownPlank/MontyMole/Scuttlebug case. All three are host copies
// in port/unmatched/BowserPuzzle_StateDispatch.cpp, which also seats the six
// Manager state sources (data_ov064_0211bf68..bf90, copied by __sinit_ov064_
// 0211b1d4 into the bss table data_ov064_0211c904) and the two Piece sources
// (data_ov064_0211c2dc/2e4, copied by __sinit_ov064_0211b4dc into data_ov064_
// 0211c934) BEFORE their sinits run. Both seats are CALLED from actor_overlays,
// not from the fill.
//
// ---- RENDER: THE MANAGER IS THE SLOT-5 COLLISION, THE PIECE IS TRIVIAL -----
//
// BowserPuzzleManager::Render (.cpp) dispatches its plain Model at +0xd4 through a
// six-virtual ROM-order local shadow (`Base` with m(int) as slot 5) -- the
// Whomp/Scuttlebug/Firebar collision -- so it is a host copy in
// port/unmatched/BowserPuzzle_Render.cpp (spelled Model::Render, the dual-filled
// slot). BowserPuzzlePiece::Render (.c) is `return 1;` -- no draw, safe in the
// slice.
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

#include "dActor_c.h"
#include "fBase_c.h"

extern "C" {
/* the shared lifecycle halves, the same functions every 31-slot fill writes */
int _ZN8dActor_c19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN8dActor_c18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN8dActor_c14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN8dActor_c12BeforeRenderEv(void *self);                   /* slot 10 */
int _ZN8dActor_c13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN8dActor_c13OnTurnIntoEggER6Player(void *self, void *p); /* slot 19 */
int _ZN8dActor_c9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN8dActor_c15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN8dActor_c11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN8dActor_c11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN8dActor_c8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN8dActor_c8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN8dActor_c15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN8dActor_c19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN8dActor_c16OnAimedAtWithEggEv(void *self);               /* slot 29 (Actor's) */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* ---- Manager's own C-linkage bodies (matched src, slice_gate179.txt) ----
   Behavior is NOT here -- it dispatches the six-state PMF table and is a host
   copy in unmatched/BowserPuzzle_StateDispatch.cpp. Render is NOT here -- it is
   the slot-5 shadow collision, host-copied in unmatched/BowserPuzzle_Render.cpp. */
int _ZN17BowserPuzzlePiece13InitResourcesEv(void *self);   /* slot 0, .cpp method, face below */
int _ZN17BowserPuzzlePiece16CleanupResourcesEv(void *self);/* slot 3, .cpp method, face below */
int _ZN17BowserPuzzlePiece6RenderEv(void *self);           /* slot 9, host copy */
int _ZN17BowserPuzzlePiece8BehaviorEv(void *self);         /* slot 6, host copy */
int *_ZN17BowserPuzzlePieceD1Ev(void *self);              /* slot 16, .c */
int *_ZN17BowserPuzzlePieceD0Ev(void *self);              /* slot 17, .c */
int *daObjFl_Coin_c_classInit(void);                        /* .c factory */

/* ---- Piece's own C-linkage bodies (matched src, slice_gate179.txt) ----
   Behavior is NOT here -- forward-declared-struct PMF, host copy in unmatched/. */
int _ZN9JetStream13InitResourcesEv(void *self);     /* slot 0, .cpp method, face below */
int _ZN9JetStream16CleanupResourcesEv(void *self);  /* slot 3, .c C linkage */
void _ZN9JetStream16OnPendingDestroyEv(void *self); /* slot 12, .c C linkage */
int _ZN9JetStream6RenderEv(void *self);             /* slot 9, .c C linkage, faced below */
int _ZN9JetStream8BehaviorEv(void *self);           /* slot 6, host copy */
int *_ZN9JetStreamD1Ev(void *self);                /* slot 16, .c */
int *_ZN9JetStreamD0Ev(void *self);                /* slot 17, .c */
int *daObjFl_Puzzle_c_classInit(void);                          /* .c factory */

/* ---- the id-79 coordinator SHELL (table _ZTV19BowserPuzzleManager, mounted) ---- */
int _ZN19BowserPuzzleManager13InitResourcesEv(void *self);   /* slot 0, three byte-flags */
int _ZN19BowserPuzzleManager8BehaviorEv(void *self);   /* slot 6, the range gate */
/* D1 (_ZN19BowserPuzzleManagerD1Ev) stores the MOUNTED c1d8 as the mid-teardown vptr and
   D0 (_ZN19BowserPuzzleManagerD0Ev) spells the shared VT/HEAP placeholders -- both are
   host thunks below storing the HOST shell table instead (the gate-178 Amilift
   treatment; the mounted words are DS addresses and must never be dispatched). */
void _ZN8dActor_cD2Ev(void *);
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;            /* Memory::gameHeapPtr */
extern unsigned char _ZTV19BowserPuzzleManager[];  /* the mounted shell table (0x84) */
int *daObjFl_Coin_c_classInit(void);  /* .c factory, installs c1d8 itself */
/* The slot-16 face onto the matched src/_ZN19BowserPuzzleManagerD1Ev.cpp; it is
   defined at the bottom of this file, where BowserPuzzleManager.h is included. */
void port_bpm_d1(void *self);

/* the two derived vtables, HOST arrays this file fills; 31 slots each. `int[]`
   with C linkage matches the `extern int _ZTV..[]` decls in include/decl_common.h
   that the .c factories and D-tors read (the montymole reading). */
/* THIRTY-TWO, not 31: the Manager is a Platform subclass (ov064 0x0211c25c)
   and its slot 31 is Platform::Kill. Thirty-one is the plain Actor width --
   which is what the Piece below really is. The "31 because Platform" premise
   that put both at 31 came from hal/actor_classes_wf.cpp's header and is
   corrected there. */
int _ZTV16daObjFl_Puzzle_c[32];
/* ov064 0x0211c334, JETSTREAM's table, 31 words: it ends at slot 30, and the
   code word one past it (func_ov064_02119ea0) is a pointer-to-member pair with
   its own dsd symbol, not a slot. THE NAME ON THIS ARRAY USED TO BE
   _ZTV17BowserPuzzlePiece, which config puts on 0x0211c25c, the OTHER table in
   this file. The contents were always JetStream's; only the label was wrong. */
int _ZTV9JetStream[31];
}

/* The Manager D-tors spell VT0 as _ZTV16daObjFl_Puzzle_c (the class's RTTI name);
   point it at the host Manager array -- the daChoropu_c / daDonketu_c precedent.
   _ZTV10dBgActor_c (VT1) and _ZTV18daWater_Hakidasi_c (JetStream's other ROM
   name, 0x0211c334) are already defined elsewhere in the build. */

/* THE TWO REAL ALIAS PAIRS IN ov064, read out of config rather than derived
   one name at a time. config/arm9/overlays/ov064/symbols.txt:

       0x0211c25c   _ZTV17BowserPuzzlePiece (266)   _ZTV16daObjFl_Puzzle_c (267)
       0x0211c334   _ZTV9JetStream          (273)   _ZTV18daWater_Hakidasi_c (274)

   Two addresses, two names each: two classes, not one. The cartridge agrees
   three more ways. The typeinfo word at 0x0211c25c-4 leads to the string
   "16daObjFl_Puzzle_c" and the one at 0x0211c334-4 to "18daWater_Hakidasi_c".
   The factories disagree: daObjFl_Puzzle_c_classInit (0x02119300) allocates
   0x33c and stamps 0x0211c25c, while daWater_Hakidasi_c_classInit (0x02119a18)
   allocates 0x378 and stamps 0x0211c334, then builds a dCcAc_c at +0x110 and a
   dBgCh_Actr at +0x144 that the 0x33c class never builds. And the two tables
   differ in exactly the seven slots a subclass overrides (0, 3, 6, 9, 12, 16, 17).

   What used to stand here was the reverse join, __ZTV18daWater_Hakidasi_c=
   __ZTV17BowserPuzzlePiece, written on the premise that those two names are one
   address. They are not. That premise proved an address for ONE of the two names
   and never asked what address the other had, which is the twelfth defect's root
   cause exactly. Its effect: src/d_a_obj_fl_puzzle.c stamps
   _ZTV17BowserPuzzlePiece, so all fourteen puzzle pieces spawned holding
   JETSTREAM's methods, while the array this file fills with the pieces' own
   methods was installed on nothing at all. */
#pragma comment(linker, "/alternatename:__ZTV18daWater_Hakidasi_c=__ZTV9JetStream")
#pragma comment(linker, "/alternatename:__ZTV17BowserPuzzlePiece=__ZTV16daObjFl_Puzzle_c")

/* BowserPuzzleManager::CleanupResources spells the collider SharedFilePtr
   data_ov064_0211c800 as data_ov075_0211c800 -- a dsd overlay mislabel: the
   address 0x0211c800 is genuinely in ov064 (ov075 has no such symbol). The .cpp
   reads it at C++ linkage through decl_common.h (?...@@3PAXA); alias it onto the
   ov064 mount's C-named symbol -- the gate-177 data_ov064_0211adbc reading. */
#pragma comment(linker, "/alternatename:?data_ov075_0211c800@@3PAXA=_data_ov064_0211c800")

/* The Piece's own CleanupResources, which the face above now reaches, declares
   the same mount as a plain `void *` at file scope, so it emits a third
   spelling of one address. Same reading, same shape: the LHS is declared and
   never defined, so tools/alternatename_guard.py stays clean. */
#pragma comment(linker, "/alternatename:?data_ov064_0211c800@@3PAXA=_data_ov064_0211c800")

/* The C++/C data-linkage bridges, the montymole/painting reading: the Manager's
   .cpp InitResources/CleanupResources spell four ov064 data symbols at C++
   linkage with specific struct types, but the ov064 mount (ovdata.py) emits only
   the C name (_data_...). Alias each C++-mangled spelling onto the C symbol.
   data_ov064_0211adc8 is spelled two ways -- SharedFilePtr*[] in InitResources,
   void*[] in CleanupResources -- so it needs both aliases. data_020a0e68
   (Matrix4x3, func_ov064_02118fa4) is already aliased in hal/actor_vtables.cpp. */
#pragma comment(linker, "/alternatename:?data_ov064_0211adc8@@3PAPAUSharedFilePtr@@A=_data_ov064_0211adc8")
#pragma comment(linker, "/alternatename:?data_ov064_0211adc8@@3PAPAXA=_data_ov064_0211adc8")
#pragma comment(linker, "/alternatename:?data_ov064_0211c800@@3USharedFilePtr@@A=_data_ov064_0211c800")
#pragma comment(linker, "/alternatename:?data_ov064_0211baac@@3UCLPS_Block@@A=_data_ov064_0211baac")
#pragma comment(linker, "/alternatename:?data_ov064_0211c198@@3PAHA=_data_ov064_0211c198")
/* Manager::CleanupResources also spells the mislabeled symbol at C linkage. */
#pragma comment(linker, "/alternatename:_data_ov075_0211c800=_data_ov064_0211c800")
/* Piece::InitResources spells the (mounted) ov002 collider SFP at C++ linkage,
   and is the first .cpp to call static Model::LoadFile by its MSVC member
   mangle; the body is the C-named _ZN5Model8LoadFileER13SharedFilePtr. */
#pragma comment(linker, "/alternatename:?data_ov002_0210da10@@3USharedFilePtr@@A=_data_ov002_0210da10")
#pragma comment(linker, "/alternatename:?LoadFile@Model@@YAXAAUSharedFilePtr@@@Z=__ZN5Model8LoadFileER13SharedFilePtr")

// ---- the trap --------------------------------------------------------------
static void bp_trap_report(void *self, int slot)
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
#define BP_TRAP(n) \
    static int __fastcall bp_trap##n(void *s, void *) \
    { bp_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
BP_TRAP(13) BP_TRAP(14)
#undef BP_TRAP

// ---- the shared 0..30 half -------------------------------------------------
static int __fastcall bp_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall bp_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall bp_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall bp_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall bp_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall bp_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall bp_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall bp_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall bp_pdes_base(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); return 0; }
static int __fastcall bp_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall bp_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
static int __fastcall bp_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall bp_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall bp_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall bp_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall bp_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall bp_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall bp_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall bp_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall bp_mega(void *s, void *, void *p)
{ _ZN8dActor_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall bp_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall bp_aimed_actor(void *s, void *)
{ return _ZN8dActor_c16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own default */

/* Fill slots 1..30 of a Manager/Piece table with the shared bodies. The caller
   writes 0/3/6/9/16/17/29 and (for the Piece) slot 12. Slot 12 defaults to
   ActorBase::OnPendingDestroy here; the Piece caller overrides it. */
/* slot 31 of a Platform table, ov002 0x020ee55c, already in the build */
extern "C" void _ZN10dBgActor_c4KillEv(void *self);
static int __fastcall bp_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }

static void bp_fill_shared_0_30(void **vt)
{
    vt[1]  = (void *)bp_binit;
    vt[2]  = (void *)bp_ainit;
    vt[4]  = (void *)bp_bclean;
    vt[5]  = (void *)bp_aclean;
    vt[7]  = (void *)bp_bbeh;
    vt[8]  = (void *)bp_abeh;
    vt[10] = (void *)bp_bren;
    vt[11] = (void *)bp_aren;
    vt[12] = (void *)bp_pdes_base;
    vt[13] = (void *)bp_trap13;
    vt[14] = (void *)bp_trap14;
    vt[15] = (void *)bp_heap;
    vt[18] = (void *)bp_yoshi;
    vt[19] = (void *)bp_egg;
    vt[20] = (void *)bp_v50;
    vt[21] = (void *)bp_pounded;
    vt[22] = (void *)bp_atk1;
    vt[23] = (void *)bp_atk2;
    vt[24] = (void *)bp_kicked;
    vt[25] = (void *)bp_pushed;
    vt[26] = (void *)bp_cannon;
    vt[27] = (void *)bp_mega;
    vt[28] = (void *)bp_under;
    vt[29] = (void *)bp_aimed_actor;
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// BOWSER_PUZZLE_MANAGER (79)
// ============================================================================
static int __fastcall mgr_init(void *s, void *)
{ return _ZN17BowserPuzzlePiece13InitResourcesEv(s); }
static int __fastcall mgr_clean(void *s, void *)
{ return _ZN17BowserPuzzlePiece16CleanupResourcesEv(s); }
static int __fastcall mgr_behavior(void *s, void *)
{ return _ZN17BowserPuzzlePiece8BehaviorEv(s); }     /* host copy */
static int __fastcall mgr_render(void *s, void *)
{ port_actor_render_probe("BOWSER_PUZZLE_MANAGER", (char *)s + 0xd4);
  return _ZN17BowserPuzzlePiece6RenderEv(s); }        /* host copy */
static int __fastcall mgr_d1(void *s, void *)
{ return (int)(size_t)_ZN17BowserPuzzlePieceD1Ev(s); }
static int __fastcall mgr_d0(void *s, void *)
{ return (int)(size_t)_ZN17BowserPuzzlePieceD0Ev(s); }

extern "C" void hal_fill_bowser_puzzle_manager_vtable(void)
{
    void **vt = (void **)_ZTV16daObjFl_Puzzle_c;
    bp_fill_shared_0_30(vt);
    vt[0]  = (void *)mgr_init;
    vt[3]  = (void *)mgr_clean;
    vt[6]  = (void *)mgr_behavior;
    vt[9]  = (void *)mgr_render;
    vt[16] = (void *)PORT_D16(mgr_d1);
    vt[17] = (void *)mgr_d0;
    /* slot 12 keeps bp_pdes_base -- the Manager defaults to ActorBase's own. */
    /* slot 31, the Platform tail; the Manager does not override it */
    vt[31] = (void *)bp_kill;
}

/* ---- the id-79 SHELL table is a HOST ARRAY (the ov080/ov095 rule: derived
   vtables are host arrays, never mounted storage; in-place fills of pack
   storage measurably lose writes). The factory installs the MOUNTED
   _ZTV19BowserPuzzleManager (raw DS words, pack-checked, never dispatchable); the
   registry wrapper below reseats the vptr onto this array -- a real
   placeholder-to-host reseat, the Bully treatment. Slots 3/9 are the ActorBase
   defaults per relocs. */
static int port_bp_shell_vtable[31];   /* vtspan: _ZTV19BowserPuzzleManager */
static int __fastcall shl_init(void *s, void *)
{ return _ZN19BowserPuzzleManager13InitResourcesEv(s); }
static int __fastcall shl_clean(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::CleanupResources(); }
static int __fastcall shl_behavior(void *s, void *)
{ return _ZN19BowserPuzzleManager8BehaviorEv(s); }
static int __fastcall shl_render(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::Render(); }
/* SLOT 16 IS THE MATCHED TU NOW (run link100 wave 14, lane SEAT14C, BATCH 2).
   What stood here was a hand-written teardown -- store the host shell table as
   the mid-teardown vptr, then _ZN8dActor_cD2Ev -- written while
   src/_ZN19BowserPuzzleManagerD1Ev.cpp (ov064 0x02118bec) was unlinked. That
   TU is on port/slice_vtd1.txt now and this thunk forwards to it through the
   face at the bottom of this file, which is where include/BowserPuzzleManager.h
   is in scope. The base step is the same body: faces_sync.txt:2861 faces
   __ZN8dActor_cD2Ev onto ??1dActor_c@@UAE@XZ and the ACTORPORT bridge aliases
   that onto ??1dActor_c@@QAE@XZ = src/_ZN8dActor_cD1Ev.cpp, which is exactly
   what the matched destructor's own ~dActor_c() call reaches. The vptr the
   matched destructor stores is MSVC's own ??_7BowserPuzzleManager@@6B@ rather
   than this array; ~dActor_c overwrites it one call later and nothing
   dispatches in between, which is the transient store hal/dtor_store_guard.py
   rules inert. It is NOT the mounted _ZTV19BowserPuzzleManager -- those words
   are DS addresses and still must never be dispatched. */
static int __fastcall shl_d1(void *s, void *)
{
    port_bpm_d1(s);
    return (int)(size_t)s;
}
static int __fastcall shl_d0(void *s, void *)
{
    char *st = (char *)s;
    *(void **)st = (void *)port_bp_shell_vtable;
    _ZN8dActor_cD2Ev(st);
    _ZN6Memory10DeallocateEPvP4Heap(st, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_bowser_puzzle_shell_vtable(void)
{
    void **vt = (void **)port_bp_shell_vtable;
    bp_fill_shared_0_30(vt);
    vt[0]  = (void *)shl_init;
    vt[3]  = (void *)shl_clean;
    vt[6]  = (void *)shl_behavior;
    vt[9]  = (void *)shl_render;
    vt[16] = (void *)PORT_D16(shl_d1);
    vt[17] = (void *)shl_d0;
}

/* the id-79 factory installs the mounted c1d8; reseat onto the host array. */
extern "C" void *port_factory_bowser_puzzle_shell(void)
{
    void *p = (void *)daObjFl_Coin_c_classInit();
    if (p)
        *(void **)p = (void *)port_bp_shell_vtable;
    return p;
}

// ============================================================================
// BOWSER_PUZZLE_PIECE (78)
// ============================================================================
static int __fastcall pce_init(void *s, void *)
{ return _ZN9JetStream13InitResourcesEv(s); }
static int __fastcall pce_clean(void *s, void *)
{ return _ZN9JetStream16CleanupResourcesEv(s); }
static int __fastcall pce_behavior(void *s, void *)
{ return _ZN9JetStream8BehaviorEv(s); }        /* host copy */
static int __fastcall pce_render(void *s, void *)
{ return _ZN9JetStream6RenderEv(s); }           /* .c stub, faced below */
static int __fastcall pce_pdes(void *s, void *)
{ _ZN9JetStream16OnPendingDestroyEv(s); return 0; }  /* slot 12, its own */
static int __fastcall pce_d1(void *s, void *)
{ return (int)(size_t)_ZN9JetStreamD1Ev(s); }
static int __fastcall pce_d0(void *s, void *)
{ return (int)(size_t)_ZN9JetStreamD0Ev(s); }

extern "C" void hal_fill_bowser_puzzle_piece_vtable(void)
{
    void **vt = (void **)_ZTV9JetStream;
    bp_fill_shared_0_30(vt);
    vt[0]  = (void *)pce_init;
    vt[3]  = (void *)pce_clean;
    vt[6]  = (void *)pce_behavior;
    vt[9]  = (void *)pce_render;
    vt[12] = (void *)pce_pdes;   /* the Piece overrides OnPendingDestroy */
    vt[16] = (void *)PORT_D16(pce_d1);
    vt[17] = (void *)pce_d0;
}

/* id 78's registry fill: the factory installs _ZTV16daObjFl_Puzzle_c (the
   828-byte class's real table), so fill that; also fill _ZTV9JetStream
   (0x0211c334), which this overlay's other class spawns and tears down with. */
extern "C" void hal_fill_bowser_puzzle_piece_vtable(void);
extern "C" void hal_fill_bowser_puzzle_pair_vtables(void)
{
    hal_fill_bowser_puzzle_manager_vtable();
    hal_fill_bowser_puzzle_piece_vtable();
}

// ---- method faces ----------------------------------------------------------
// The C-named references the vtable thunks take onto definitions that are real
// MSVC methods against include/. The two Behaviors and the Manager's Render are
// NOT faced here -- they are host copies (unmatched/). The Piece's Render and its
// CleanupResources/OnPendingDestroy are C-linkage .c bodies in the slice; they
// need no face. The three .cpp methods (both InitResources, Manager's Cleanup) are
// faced onto their qualified method bodies.
#include "BowserPuzzleManager.h"
#include "BowserPuzzlePiece.h"
#include "JetStream.h"
extern "C" {
/* Each C name is bound to ITS OWN class. config/arm9/overlays/ov064 puts
   _ZN17BowserPuzzlePiece13InitResourcesEv at 0x021191a8 and
   _ZN17BowserPuzzlePiece16CleanupResourcesEv at 0x0211904c, which are slots 0
   and 3 of table 0x0211c25c (the Piece's), and
   _ZN9JetStream13InitResourcesEv at 0x0211992c, which is slot 0 of table
   0x0211c334 (JetStream's). src/ agrees: each of those three TUs defines the
   method of the class its own name spells. */
int _ZN17BowserPuzzlePiece13InitResourcesEv(void *self)
{ return ((BowserPuzzlePiece *)self)->BowserPuzzlePiece::InitResources(); }
int _ZN17BowserPuzzlePiece16CleanupResourcesEv(void *self)
{ return ((BowserPuzzlePiece *)self)->BowserPuzzlePiece::CleanupResources(); }
int _ZN9JetStream13InitResourcesEv(void *self)
{ return ((JetStream *)self)->JetStream::InitResources(); }

/* THE MANAGER'S SLOT-16 FACE (lane SEAT14C, BATCH 2). shl_d1 above calls this;
   it calls the matched ov064 0x02118bec destructor, qualified so it is the
   complete-object form and not a virtual re-dispatch. */
void port_bpm_d1(void *self)
{ ((BowserPuzzleManager *)self)->BowserPuzzleManager::~BowserPuzzleManager(); }
}

/* THE TWO COMPILER-VTABLE SLOTS THE MATCHED TU NEEDS, under the ACTORPORT
   ruling (hal/actorport_dtor_bridge.cpp, section 2 of its header). Compiling
   src/_ZN19BowserPuzzleManagerD1Ev.cpp makes MSVC emit ??_7BowserPuzzleManager@@6B@,
   and include/BowserPuzzleManager.h spells the destructor pair as two plain
   virtuals under _MSC_VER so that MSVC numbers the table the way mwccarm does.
   NOTHING DISPATCHES THAT COMPILER TABLE: this overlay's objects come out of the
   .c factories and are reseated onto the host arrays this file fills, so these
   two only have to LINK. Destructor1 is the class's own complete-object
   destructor, which is what ROM slot 16 holds. Destructor0 is the shared trap
   the bridge uses, because the deleting half's free is per class and inventing
   one generic body would be a guess; if it ever fires, something really did
   dispatch the compiler table and that is a finding, not a crash to paper over.
   The bridge's generator did not emit a row for this class because nothing in
   the image referenced the pair until this batch put the TU on a slice. */
extern "C" void __fastcall actorport_dtor_slot_trap(void *self, void *unused);

void BowserPuzzleManager::Destructor1()
{ this->BowserPuzzleManager::~BowserPuzzleManager(); }

void BowserPuzzleManager::Destructor0()
{ actorport_dtor_slot_trap(this, 0); }
