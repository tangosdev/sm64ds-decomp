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
// _ZTV19BowserPuzzleManager (ov064 0x0211c25c) and _ZTV17BowserPuzzlePiece
// (0x0211c334) are each the standard 31-slot Actor table read straight off their
// reloc spans: slot 1 = Actor::BeforeInitResources (arm9 0x02011268), slots 18..29
// the Actor interaction tail, slot 30 = OnAimedAtWithEggReturnVec (0x020100dc).
// Both carry the vtable signature (+4 relocates to 0x02011268) and are HOST arrays
// this file fills -- they stay OUT of the mount, the ov080/ov095/ov071/ov064 rule.
// The Manager overrides 0/3/6/9/16/17 and defaults slot 12 to ActorBase::OnPending-
// Destroy; the Piece overrides 0/3/6/9/12/16/17 (its own OnPendingDestroy body).
//
// ---- THE CONFIG NAMES CROSS THE TWO CLASSES; THE FACTORIES ARE HONEST -----
//
// Read off the spawn chain, not the names. BowserPuzzlePiece_Spawn (id 78, x14)
// allocates 828 bytes, runs Platform's ctor and installs _ZTV19BowserPuzzleManager
// (0x0211c25c) -- so the class the config calls "Manager" is the PIECE's real
// class: its InitResources reads its model out of the 14-entry SFP-pointer table
// data_ov064_0211adc8[param & 0xf], one model per puzzle piece, and its Behavior
// runs the six-state PMF machine. BowserPuzzleManager_Spawn (id 79, x1) allocates
// a 216-byte COORDINATOR SHELL and installs data_ov064_0211c1d8, a real 31-slot
// table of its own: slot 0 func_ov064_02119284 (three byte-flags), slot 6
// func_ov064_0211915c (waits for flag 3 + player range), slots 16/17 its D1/D0,
// slots 3/9 the ActorBase defaults. Each factory installs its class's REAL table
// as the last vptr write -- NO reseat wrappers. (An earlier reading called c1d8 a
// placeholder and reseated both vptrs onto the big class; the 216-byte shell then
// ran the 828-byte InitResources, indexed adc8[0xffff & 0xf] into the pack gap
// and faulted on the null SFP.)
//
// _ZTV17BowserPuzzlePiece (0x0211c334) is installed only mid-teardown by the
// D-tors that bear its name; it is filled too, belt and braces.
//
// ---- THE D-TORS STAY IN THE SLICE -----------------------------------------
//
// The Manager D1/D0 (.c) spell VT0 = _ZTV16daObjFl_Puzzle_c then VT1 =
// _ZTV10dBgActor_c (Platform's base table, already host-filled by
// hal_fill_platform_vtable), both harmless host writes to a dying object -- the
// FallBlockWf reading. _ZTV16daObjFl_Puzzle_c is aliased below to the real
// Manager host array. The Piece D1/D0 (.c) spell _ZTV17BowserPuzzlePiece /
// _ZTV18daWater_Hakidasi_c, which are the SAME address (0x0211c334) and both the
// real Piece table -- no launder, no placeholder, so they need no host thunk. All
// four run their member teardowns (Manager: MovingMeshCollider +0x124, Model +0xd4,
// Actor::D2; Piece: WithMeshClsn +0x144, MovingCylinderClsn +0x110, the Enemy base
// D2 func_ov002_020aed18) with faces already in the build.
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
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the shared lifecycle halves, the same functions every 31-slot fill writes */
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
int _ZN5Actor16OnAimedAtWithEggEv(void *self);               /* slot 29 (Actor's) */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc.cpp: per-actor decline */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* ---- Manager's own C-linkage bodies (matched src, slice_gate179.txt) ----
   Behavior is NOT here -- it dispatches the six-state PMF table and is a host
   copy in unmatched/BowserPuzzle_StateDispatch.cpp. Render is NOT here -- it is
   the slot-5 shadow collision, host-copied in unmatched/BowserPuzzle_Render.cpp. */
int _ZN19BowserPuzzleManager13InitResourcesEv(void *self);   /* slot 0, .cpp method, face below */
int _ZN19BowserPuzzleManager16CleanupResourcesEv(void *self);/* slot 3, .cpp method, face below */
int _ZN19BowserPuzzleManager6RenderEv(void *self);           /* slot 9, host copy */
int _ZN19BowserPuzzleManager8BehaviorEv(void *self);         /* slot 6, host copy */
int *_ZN19BowserPuzzleManagerD1Ev(void *self);              /* slot 16, .c */
int *_ZN19BowserPuzzleManagerD0Ev(void *self);              /* slot 17, .c */
int *BowserPuzzleManager_Spawn(void);                        /* .c factory */

/* ---- Piece's own C-linkage bodies (matched src, slice_gate179.txt) ----
   Behavior is NOT here -- forward-declared-struct PMF, host copy in unmatched/. */
int _ZN17BowserPuzzlePiece13InitResourcesEv(void *self);     /* slot 0, .cpp method, face below */
int _ZN17BowserPuzzlePiece16CleanupResourcesEv(void *self);  /* slot 3, .c C linkage */
void _ZN17BowserPuzzlePiece16OnPendingDestroyEv(void *self); /* slot 12, .c C linkage */
int _ZN17BowserPuzzlePiece6RenderEv(void *self);             /* slot 9, .c C linkage, faced below */
int _ZN17BowserPuzzlePiece8BehaviorEv(void *self);           /* slot 6, host copy */
int *_ZN17BowserPuzzlePieceD1Ev(void *self);                /* slot 16, .c */
int *_ZN17BowserPuzzlePieceD0Ev(void *self);                /* slot 17, .c */
int *BowserPuzzlePiece_Spawn(void);                          /* .c factory */

/* ---- the id-79 coordinator SHELL (table data_ov064_0211c1d8, mounted) ---- */
int func_ov064_02119284(void *self);   /* slot 0, three byte-flags */
int func_ov064_0211915c(void *self);   /* slot 6, the range gate */
/* D1 (func_ov064_02118bec) stores the MOUNTED c1d8 as the mid-teardown vptr and
   D0 (func_ov064_02118c10) spells the shared VT/HEAP placeholders -- both are
   host thunks below storing the HOST shell table instead (the gate-178 Amilift
   treatment; the mounted words are DS addresses and must never be dispatched). */
void _ZN5ActorD2Ev(void *);
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;            /* Memory::gameHeapPtr */
extern unsigned char data_ov064_0211c1d8[];  /* the mounted shell table (0x84) */
int *BowserPuzzleManager_Spawn(void);  /* .c factory, installs c1d8 itself */

/* the two derived vtables, HOST arrays this file fills; 31 slots each. `int[]`
   with C linkage matches the `extern int _ZTV..[]` decls in include/decl_common.h
   that the .c factories and D-tors read (the montymole reading). */
/* THIRTY-TWO, not 31: the Manager is a Platform subclass (ov064 0x0211c25c)
   and its slot 31 is Platform::Kill. Thirty-one is the plain Actor width --
   which is what the Piece below really is. The "31 because Platform" premise
   that put both at 31 came from hal/actor_classes_wf.cpp's header and is
   corrected there. */
int _ZTV19BowserPuzzleManager[32];
/* 31 and correct: the Piece's table ends at slot 30, and the code word one
   past it (func_ov064_02119ea0) is a pointer-to-member pair with its own dsd
   symbol, not a slot. */
int _ZTV17BowserPuzzlePiece[31];
}

/* The Manager D-tors spell VT0 as _ZTV16daObjFl_Puzzle_c (the class's RTTI name);
   point it at the host Manager array -- the daChoropu_c / daDonketu_c precedent.
   _ZTV10dBgActor_c (VT1) and _ZTV18daWater_Hakidasi_c (the Piece's D0 alias, same
   address as _ZTV17BowserPuzzlePiece) are already defined elsewhere in the build. */
#pragma comment(linker, "/alternatename:__ZTV16daObjFl_Puzzle_c=__ZTV19BowserPuzzleManager")

/* The Piece D0 (src, .c) spells the Piece table by its OTHER RTTI name,
   _ZTV18daWater_Hakidasi_c -- the same ROM address (0x0211c334) as
   _ZTV17BowserPuzzlePiece, an aliased symbol pair the dsd export carries twice.
   Nothing else defines it (only JetStream_Spawn, unhosted, and this D0 spell it),
   so alias it onto the host Piece array -- the daObjFl_Puzzle_c reading. */
#pragma comment(linker, "/alternatename:__ZTV18daWater_Hakidasi_c=__ZTV17BowserPuzzlePiece")

/* BowserPuzzleManager::CleanupResources spells the collider SharedFilePtr
   data_ov064_0211c800 as data_ov075_0211c800 -- a dsd overlay mislabel: the
   address 0x0211c800 is genuinely in ov064 (ov075 has no such symbol). The .cpp
   reads it at C++ linkage through decl_common.h (?...@@3PAXA); alias it onto the
   ov064 mount's C-named symbol -- the gate-177 data_ov064_0211adbc reading. */
#pragma comment(linker, "/alternatename:?data_ov075_0211c800@@3PAXA=_data_ov064_0211c800")

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
BP_TRAP(13) BP_TRAP(14) BP_TRAP(30)
#undef BP_TRAP

// ---- the shared 0..30 half -------------------------------------------------
static int __fastcall bp_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall bp_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall bp_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall bp_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall bp_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall bp_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall bp_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall bp_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall bp_pdes_base(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall bp_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall bp_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall bp_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall bp_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall bp_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall bp_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall bp_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall bp_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall bp_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall bp_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall bp_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall bp_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall bp_aimed_actor(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own default */

/* Fill slots 1..30 of a Manager/Piece table with the shared bodies. The caller
   writes 0/3/6/9/16/17/29 and (for the Piece) slot 12. Slot 12 defaults to
   ActorBase::OnPendingDestroy here; the Piece caller overrides it. */
/* slot 31 of a Platform table, ov002 0x020ee55c, already in the build */
extern "C" void _ZN8Platform4KillEv(void *self);
static int __fastcall bp_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

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
    vt[30] = (void *)bp_trap30;
}

// ============================================================================
// BOWSER_PUZZLE_MANAGER (79)
// ============================================================================
static int __fastcall mgr_init(void *s, void *)
{ return _ZN19BowserPuzzleManager13InitResourcesEv(s); }
static int __fastcall mgr_clean(void *s, void *)
{ return _ZN19BowserPuzzleManager16CleanupResourcesEv(s); }
static int __fastcall mgr_behavior(void *s, void *)
{ return _ZN19BowserPuzzleManager8BehaviorEv(s); }     /* host copy */
static int __fastcall mgr_render(void *s, void *)
{ port_actor_render_probe("BOWSER_PUZZLE_MANAGER", (char *)s + 0xd4);
  return _ZN19BowserPuzzleManager6RenderEv(s); }        /* host copy */
static int __fastcall mgr_d1(void *s, void *)
{ return (int)(size_t)_ZN19BowserPuzzleManagerD1Ev(s); }
static int __fastcall mgr_d0(void *s, void *)
{ return (int)(size_t)_ZN19BowserPuzzleManagerD0Ev(s); }

extern "C" void hal_fill_bowser_puzzle_manager_vtable(void)
{
    void **vt = (void **)_ZTV19BowserPuzzleManager;
    bp_fill_shared_0_30(vt);
    vt[0]  = (void *)mgr_init;
    vt[3]  = (void *)mgr_clean;
    vt[6]  = (void *)mgr_behavior;
    vt[9]  = (void *)mgr_render;
    vt[16] = (void *)mgr_d1;
    vt[17] = (void *)mgr_d0;
    /* slot 12 keeps bp_pdes_base -- the Manager defaults to ActorBase's own. */
    /* slot 31, the Platform tail; the Manager does not override it */
    vt[31] = (void *)bp_kill;
}

/* ---- the id-79 SHELL table is a HOST ARRAY (the ov080/ov095 rule: derived
   vtables are host arrays, never mounted storage; in-place fills of pack
   storage measurably lose writes). The factory installs the MOUNTED
   data_ov064_0211c1d8 (raw DS words, pack-checked, never dispatchable); the
   registry wrapper below reseats the vptr onto this array -- a real
   placeholder-to-host reseat, the Bully treatment. Slots 3/9 are the ActorBase
   defaults per relocs. */
static int port_bp_shell_vtable[31];   /* vtspan: data_ov064_0211c1d8 */
static int __fastcall shl_init(void *s, void *)
{ return func_ov064_02119284(s); }
static int __fastcall shl_clean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall shl_behavior(void *s, void *)
{ return func_ov064_0211915c(s); }
static int __fastcall shl_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall shl_d1(void *s, void *)
{
    char *st = (char *)s;
    *(void **)st = (void *)port_bp_shell_vtable;
    _ZN5ActorD2Ev(st);
    return (int)(size_t)s;
}
static int __fastcall shl_d0(void *s, void *)
{
    char *st = (char *)s;
    *(void **)st = (void *)port_bp_shell_vtable;
    _ZN5ActorD2Ev(st);
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
    vt[16] = (void *)shl_d1;
    vt[17] = (void *)shl_d0;
}

/* the id-79 factory installs the mounted c1d8; reseat onto the host array. */
extern "C" void *port_factory_bowser_puzzle_shell(void)
{
    void *p = (void *)BowserPuzzleManager_Spawn();
    if (p)
        *(void **)p = (void *)port_bp_shell_vtable;
    return p;
}

// ============================================================================
// BOWSER_PUZZLE_PIECE (78)
// ============================================================================
static int __fastcall pce_init(void *s, void *)
{ return _ZN17BowserPuzzlePiece13InitResourcesEv(s); }
static int __fastcall pce_clean(void *s, void *)
{ return _ZN17BowserPuzzlePiece16CleanupResourcesEv(s); }
static int __fastcall pce_behavior(void *s, void *)
{ return _ZN17BowserPuzzlePiece8BehaviorEv(s); }        /* host copy */
static int __fastcall pce_render(void *s, void *)
{ return _ZN17BowserPuzzlePiece6RenderEv(s); }           /* .c stub, faced below */
static int __fastcall pce_pdes(void *s, void *)
{ _ZN17BowserPuzzlePiece16OnPendingDestroyEv(s); return 0; }  /* slot 12, its own */
static int __fastcall pce_d1(void *s, void *)
{ return (int)(size_t)_ZN17BowserPuzzlePieceD1Ev(s); }
static int __fastcall pce_d0(void *s, void *)
{ return (int)(size_t)_ZN17BowserPuzzlePieceD0Ev(s); }

extern "C" void hal_fill_bowser_puzzle_piece_vtable(void)
{
    void **vt = (void **)_ZTV17BowserPuzzlePiece;
    bp_fill_shared_0_30(vt);
    vt[0]  = (void *)pce_init;
    vt[3]  = (void *)pce_clean;
    vt[6]  = (void *)pce_behavior;
    vt[9]  = (void *)pce_render;
    vt[12] = (void *)pce_pdes;   /* the Piece overrides OnPendingDestroy */
    vt[16] = (void *)pce_d1;
    vt[17] = (void *)pce_d0;
}

/* id 78's registry fill: the factory installs _ZTV19BowserPuzzleManager (the
   828-byte class's real table), so fill that; also fill _ZTV17BowserPuzzlePiece,
   which the teardown path writes mid-destruction. */
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
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN19BowserPuzzleManager13InitResourcesEv(void *self)
{ ((BowserPuzzleManager *)self)->BowserPuzzleManager::InitResources(); return 1; }
#else
int _ZN19BowserPuzzleManager13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN19BowserPuzzleManager13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN19BowserPuzzleManager16CleanupResourcesEv(void *self)
{ return ((BowserPuzzleManager *)self)->BowserPuzzleManager::CleanupResources(); }
#else
int _ZN19BowserPuzzleManager16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN19BowserPuzzleManager16CleanupResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN17BowserPuzzlePiece13InitResourcesEv(void *self)
{ return ((BowserPuzzlePiece *)self)->BowserPuzzlePiece::InitResources(); }
#else
int _ZN17BowserPuzzlePiece13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN17BowserPuzzlePiece13InitResourcesEv */
#endif /* _WIN32 */
}
