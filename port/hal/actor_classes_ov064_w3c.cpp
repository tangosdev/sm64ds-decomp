// RUN REL0215 WAVE 3 (lane w3-c): ov064's LAST THREE SPAWN-TABLE CLASSES.
//
//   id   class           table                      width  placed on
//   245  JET_STREAM      0x0211c334 (already host)   31     levels 8, 18
//   244  WATER_RING      0x0211c420                  31     no level's list
//    13  TREASURE_CHEST  0x0211c50c                  31     levels 8, 9, 18
//
// port/ov064_syms.txt's gate-177 block names all three in its NOT-RUN paragraph
// and defers them; gates 178/179 and wave 12 took the other four, and this file
// is what is left. After it, ov064's spawn-table cast is closed.
//
// ---- IDENTITY, BOTH ROUTES, PER ID -----------------------------------------
//
// Spawn table: ACTOR_SPAWN_TABLE (arm9 0x02090864) slots 245/244/13 relocate to
// 0x0211c310 / 0x0211c3fc / 0x0211c4e8; each record's word[0] is a factory
// inside ov064's own image and each +4 halfword reads its own id back, read off
// the RAW image (extracted/overlays/overlay_0064.bin at base 0x02115ee0 -- T4,
// ov064 is compressed:true). Typeinfo: each factory's last vptr store names its
// table and the word at table-4 names the class --
// "18daWater_Hakidasi_c" / "14daWater_Ring_c" / "11daObjTbox_c".
//
// ---- JET_STREAM'S TABLE WAS ALREADY FILLED, ONE CLASS OFF ------------------
//
// dsd's ov064 BowserPuzzle labels are ROTATED BY ONE. id 78's factory
// (0x02119300, dsd BowserPuzzlePiece_Spawn) installs _ZTV19BowserPuzzleManager
// (0x0211c25c, RTTI "16daObjFl_Puzzle_c"); id 79's (0x021192d0) installs
// data_ov064_0211c1d8 ("14daObjFl_Coin_c"); and _ZTV17BowserPuzzlePiece
// (0x0211c334) is JET STREAM'S table -- which is why
// hal/actor_classes_bowserpuzzle.cpp's own note says the only things that spell
// it are "this D0 and JetStream_Spawn, unhosted". Gate 179 filled that array
// with the seven bodies dsd named _ZN17BowserPuzzlePiece* (JetStream's own
// InitResources / CleanupResources / Behavior / Render / OnPendingDestroy /
// D1 / D0), sliced every one of them in port/slice_gate179.txt, and seated its
// two-record state Obj. So JET_STREAM costs one SpawnInfo, one factory TU and
// one registry row; hal_fill_jet_stream_vtable below is a NAME, not a second
// fill, and calling gate 179's fill twice is idempotent (it writes constants).
//
// Nothing in gate 179 is corrected by this file. Its wiring was right; only its
// prose called JetStream "the Piece", and the correction is written down in
// port/ov064_syms.txt's wave-3 block rather than by renaming a symbol the ROM's
// own dsd export carries under two names already.
//
// ---- T1: BOTH NEW TABLES STAY ROM-SHAPED -----------------------------------
//
// Neither WaterRing nor TreasureChest derives from Model or ModelAnim -- each
// HOLDS one (WaterRing a Model at +0x30c and a TextureTransformer at +0x35c,
// TreasureChest a ModelAnim at +0xd4) -- so the default applies: [16] D1,
// [17] D0, ROM order. Both are 31 slots and neither is a Platform: slot 30 is
// Actor::OnAimedAtWithEggReturnVec (0x020100dc) and there is no Platform::Kill
// tail, the Clam/ov060 shape. The reloc runs read 32 and 34 respectively, and
// both overruns are the trap this overlay has already produced twice -- the
// words past slot 30 are the class's own pointer-to-member SOURCE pairs
// (WaterRing's four at 0x0211c3b0, TreasureChest's six at 0x0211c49c), every
// other one zero at 8-byte stride, which is the file-table/PMF tell. Clam's
// own header measured exactly this on _ZTV11daObjTbox_c and named it: "its last
// three slots are the next Itanium typeinfo object". Neither table is 34 and
// neither is 32.
//
// ---- WHAT IS HOST-COPIED, AND WHY ------------------------------------------
//
//   WaterRing::Behavior          mwcc PMF over a forward-declared struct
//   func_ov064_02119ecc          the same, the enter half
//   WaterRing::Render            local six-virtual Model slot-5 shadow
//   func_ov064_0211a6ec / _0211a734   TreasureChest's two PMF dispatchers
//   func_ov064_0211a4c4          THE ROM BODY THE DECOMP DOES NOT HAVE
//   TreasureChest::Render        local six-virtual ModelAnim slot-5 shadow
//   TreasureChest D1 (slot 16)   a real MSVC dtor with no C name -- host chain
//
// The first six live in port/unmatched/Ov064_WaterRing.cpp,
// port/unmatched/Ov064_TreasureChest.cpp and port/unmatched/Ov064_Clam.cpp
// (the last by that file's own instruction). The seventh is tc_d1 below.
// func_ov064_0211a4c4's derivation -- no delink block, no src file, referenced
// only from the data word at 0x0211c4bc, stored by __sinit_ov064_0211b59c at
// Entry[0].tick, and reached on the first Behavior frame of every chest -- is
// in Ov064_TreasureChest.cpp's header, along with why a loud face is not
// available for that one slot.
//
// ---- WaterRing's D1/D0 ARE BOTH C NAMES ------------------------------------
//
// src/_ZN9WaterRingD1Ev.c and src/_ZN9WaterRingD0Ev.c are both .c bodies with
// C linkage that restore _ZTV9WaterRing, so both stay in the slice and neither
// needs a host thunk -- the gate-179 Piece reading. TreasureChest's D0 is a .c
// too (it spells the table by its RTTI name _ZTV11daObjTbox_c, aliased below);
// only its D1 is a real MSVC destructor over structs declared inside its own
// .cpp, emitting ??1TreasureChest@@UAE@XZ and never the C name -- the
// gate-199/200/202, ov045 ExtendingPlatform and Clam case. tc_d1 is that
// chain, transcribed from the ROM at 0x0211a200 and cross-checked against its
// sibling D0, which spells the identical chain plus Memory::Deallocate.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half -- the same words every 31-slot Actor table carries,
   checked slot for slot against both reloc runs. */
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

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);
void port_actor_render_probe(const char *cls, void *model);

/* gate 179's fill, which already writes JetStream's table. */
void hal_fill_bowser_puzzle_piece_vtable(void);

/* ---- WATER_RING (244) bodies ---------------------------------------------
   Behavior and Render are HOST COPIES (unmatched/Ov064_WaterRing.cpp); the
   other five are matched src in port/slice_w3c.txt. */
int _ZN9WaterRing13InitResourcesEv(void *self);     /* slot 0, .cpp, faced */
int _ZN9WaterRing16CleanupResourcesEv(void *self);  /* slot 3, .c  */
int _ZN9WaterRing8BehaviorEv(void *self);           /* slot 6, HOST COPY */
int _ZN9WaterRing6RenderEv(void *self);             /* slot 9, HOST COPY */
void _ZN9WaterRing16OnPendingDestroyEv(void *self); /* slot 12, .c */
int *_ZN9WaterRingD1Ev(void *self);                 /* slot 16, .c */
int *_ZN9WaterRingD0Ev(void *self);                 /* slot 17, .c */
void *WaterRing_Spawn(void);
void port_water_ring_states_seat(void);
void __sinit_ov064_0211b518(void);

/* ---- TREASURE_CHEST (13) bodies ------------------------------------------- */
int _ZN13TreasureChest13InitResourcesEv(void *self);    /* slot 0, .cpp, faced */
int _ZN13TreasureChest16CleanupResourcesEv(void *self); /* slot 3, .c */
int _ZN13TreasureChest8BehaviorEv(void *self);          /* slot 6, .cpp, faced */
int _ZN13TreasureChest6RenderEv(void *self);            /* slot 9, HOST COPY */
int *_ZN13TreasureChestD0Ev(int *self);                 /* slot 17, .c */
void *TreasureChest_Spawn(void);
void port_treasure_chest_states_seat(void);
void __sinit_ov064_0211b59c(void);

/* what tc_d1 spells by hand */
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);

/* the two derived vtables this lane hosts. `int[]` with C linkage matches the
   `extern int _ZTV..[]` declarations in include/decl_common.h that the .c
   factories and D-tors read (the montymole reading). JetStream's is gate 179's
   _ZTV17BowserPuzzlePiece and is NOT redeclared here. */
DSSTATE_BEGIN
int _ZTV9WaterRing[31];
int _ZTV13TreasureChest[31];
DSSTATE_END
}

/* dsd names each address twice and both spellings are out of the mount; the
   RTTI spelling is the one each class's own D0 restores by
   (src/_ZN9WaterRingD0Ev.c and src/_ZN13TreasureChestD0Ev.c). Both LHS are
   declared and never defined, so tools/alternatename_guard.py stays clean. */
#pragma comment(linker, "/alternatename:__ZTV14daWater_Ring_c=__ZTV9WaterRing")
#pragma comment(linker, "/alternatename:__ZTV11daObjTbox_c=__ZTV13TreasureChest")

// ---- the trap --------------------------------------------------------------
static void ov64w3c_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov064 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov064 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV64W3C_TRAP(n) \
    static int __fastcall ov64w3c_trap##n(void *s, void *) \
    { ov64w3c_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
OV64W3C_TRAP(13) OV64W3C_TRAP(14) OV64W3C_TRAP(30)
#undef OV64W3C_TRAP

// ---- the shared 0..30 half -------------------------------------------------
static int __fastcall w3c_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall w3c_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall w3c_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall w3c_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall w3c_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall w3c_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall w3c_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall w3c_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall w3c_pdes_base(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall w3c_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall w3c_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall w3c_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall w3c_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall w3c_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall w3c_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall w3c_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall w3c_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall w3c_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall w3c_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall w3c_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall w3c_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall w3c_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fill slots 1..15 and 18..30 of a 31-slot Actor table with the shared bodies.
   The caller writes 0/3/6/9/16/17 and, where the class overrides it, 12. */
static void w3c_fill_shared(void **vt)
{
    vt[1]  = (void *)w3c_binit;
    vt[2]  = (void *)w3c_ainit;
    vt[4]  = (void *)w3c_bclean;
    vt[5]  = (void *)w3c_aclean;
    vt[7]  = (void *)w3c_bbeh;
    vt[8]  = (void *)w3c_abeh;
    vt[10] = (void *)w3c_bren;
    vt[11] = (void *)w3c_aren;
    vt[12] = (void *)w3c_pdes_base;
    vt[13] = (void *)ov64w3c_trap13;
    vt[14] = (void *)ov64w3c_trap14;
    vt[15] = (void *)w3c_heap;
    vt[18] = (void *)w3c_yoshi;
    vt[19] = (void *)w3c_turn_egg;
    vt[20] = (void *)w3c_v50;
    vt[21] = (void *)w3c_pounded;
    vt[22] = (void *)w3c_atk1;
    vt[23] = (void *)w3c_atk2;
    vt[24] = (void *)w3c_kicked;
    vt[25] = (void *)w3c_pushed;
    vt[26] = (void *)w3c_cannon;
    vt[27] = (void *)w3c_mega;
    vt[28] = (void *)w3c_under;
    vt[29] = (void *)w3c_egg;
    vt[30] = (void *)ov64w3c_trap30;
}

// ============================================================================
// JET_STREAM (245) -- gate 179's array, under its rotated name.
// ============================================================================
extern "C" void hal_fill_jet_stream_vtable(void)
{
    /* _ZTV17BowserPuzzlePiece IS JetStream's table (0x0211c334, RTTI
       "18daWater_Hakidasi_c") and gate 179 already fills it with JetStream's
       own seven bodies. This is the correctly-named entry point for the
       registry row, not a second fill; the underlying fill writes constants
       and is idempotent. */
    hal_fill_bowser_puzzle_piece_vtable();
}

// ============================================================================
// WATER_RING (244) -- table 0x0211c420, RTTI "14daWater_Ring_c".
// ============================================================================
//
// A 912-byte Enemy: MovingCylinderClsnWithPos at +0x110, WithMeshClsn at
// +0x150, Model at +0x30c, TextureTransformer at +0x35c, the state record
// pointer at +0x370. Overrides 0/3/6/9/12/16/17; slot 29 is Actor's own.
static int __fastcall wr_init(void *s, void *)
{ return _ZN9WaterRing13InitResourcesEv(s); }
static int __fastcall wr_clean(void *s, void *)
{ return _ZN9WaterRing16CleanupResourcesEv(s); }
static int __fastcall wr_behavior(void *s, void *)
{ return _ZN9WaterRing8BehaviorEv(s); }          /* HOST COPY */
static int __fastcall wr_render(void *s, void *)
{ port_actor_render_probe("WATER_RING", (char *)s + 0x30c);
  return _ZN9WaterRing6RenderEv(s); }            /* HOST COPY */
static int __fastcall wr_pdes(void *s, void *)
{ _ZN9WaterRing16OnPendingDestroyEv(s); return 0; }
static int __fastcall wr_d1(void *s, void *)
{ return (int)(size_t)_ZN9WaterRingD1Ev(s); }
static int __fastcall wr_d0(void *s, void *)
{ return (int)(size_t)_ZN9WaterRingD0Ev(s); }

// ============================================================================
// TREASURE_CHEST (13) -- table 0x0211c50c, RTTI "11daObjTbox_c".
// ============================================================================
//
// A 376-byte Actor: ModelAnim at +0xd4, MovingCylinderClsn at +0x138, the
// linked-actor id at +0x15c, the state index at +0x16c and the arming timer at
// +0x170. Overrides 0/3/6/9/16/17; slot 12 keeps ActorBase's OnPendingDestroy
// and slot 29 is Actor's own, both read off the reloc run.
static int __fastcall tc_init(void *s, void *)
{ return _ZN13TreasureChest13InitResourcesEv(s); }
static int __fastcall tc_clean(void *s, void *)
{ return _ZN13TreasureChest16CleanupResourcesEv(s); }
static int __fastcall tc_behavior(void *s, void *)
{ return _ZN13TreasureChest8BehaviorEv(s); }
static int __fastcall tc_render(void *s, void *)
{ port_actor_render_probe("TREASURE_CHEST", (char *)s + 0xd4);
  return _ZN13TreasureChest6RenderEv(s); }       /* HOST COPY */
/* slot 16, HOST CHAIN -- src/_ZN13TreasureChestD1Ev.cpp is a real MSVC
   destructor over structs declared inside itself, so it emits
   ??1TreasureChest@@UAE@XZ and never the C name. Transcribed from ROM
   0x0211a200 and cross-checked against src/_ZN13TreasureChestD0Ev.c, which
   spells the identical chain plus Memory::Deallocate. */
static int __fastcall tc_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV13TreasureChest;
    _ZN18MovingCylinderClsnD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall tc_d0(void *s, void *)
{ return (int)(size_t)_ZN13TreasureChestD0Ev((int *)s); }

/* CAPTURED, the hal/level_boot.cpp g_level_mounted argument: this flag says
   "the wave-3 bring-up has run", and everything those two sinits write -- the
   seated state records and the two SharedFilePtrs -- lives in .dsstate. A
   restore rolls that back; a guard that does not roll back with it leaves the
   pass skipped forever and the table holding raw DS pointers. */
DSSTATE_BEGIN
static int g_w3c_bringup_done;
DSSTATE_END

/* Seat both SOURCE tables over their host bodies BEFORE the sinits copy them --
   the MontyMole / Crate / gate-179 order. Each seat verifies the mounted words
   against the ROM addresses first and aborts on a mismatch, so a mount pointing
   at the wrong bytes says so instead of copying an overlay-image address into a
   live dispatch table. */
static void w3c_bringup(void)
{
    if (g_w3c_bringup_done)
        return;
    g_w3c_bringup_done = 1;
    port_water_ring_states_seat();
    __sinit_ov064_0211b518();      /* WaterRing: 4 sources -> c944 / c954 */
    port_treasure_chest_states_seat();
    __sinit_ov064_0211b59c();      /* TreasureChest: 2 SFPs + 6 sources -> c98c */
}

extern "C" void hal_fill_water_ring_vtable(void)
{
    w3c_bringup();
    void *volatile *vt = (void *volatile *)_ZTV9WaterRing;
    w3c_fill_shared((void **)vt);
    vt[0]  = (void *)wr_init;
    vt[3]  = (void *)wr_clean;
    vt[6]  = (void *)wr_behavior;
    vt[9]  = (void *)wr_render;
    vt[12] = (void *)wr_pdes;      /* WaterRing overrides OnPendingDestroy */
    vt[16] = (void *)wr_d1;
    vt[17] = (void *)wr_d0;
    /* 31 slots: an Enemy/Actor, not a Platform. No slot 31. */
}

extern "C" void hal_fill_treasure_chest_vtable(void)
{
    w3c_bringup();
    void *volatile *vt = (void *volatile *)_ZTV13TreasureChest;
    w3c_fill_shared((void **)vt);
    vt[0]  = (void *)tc_init;
    vt[3]  = (void *)tc_clean;
    vt[6]  = (void *)tc_behavior;
    vt[9]  = (void *)tc_render;
    vt[16] = (void *)tc_d1;
    vt[17] = (void *)tc_d0;
    /* slot 12 keeps w3c_pdes_base -- the chest defaults to ActorBase's own. */
    /* 31 slots: an Actor, not a Platform. No slot 31. */
}

// ---- method faces ----------------------------------------------------------
// The bodies src defines as real C++ methods. WaterRing's Cleanup /
// OnPendingDestroy / D1 / D0 and TreasureChest's Cleanup / D0 are extern-C
// already; both Renders and WaterRing's Behavior are host copies.
#include "WaterRing.h"
#include "TreasureChest.h"
extern "C" {
int _ZN9WaterRing13InitResourcesEv(void *self)
{ return ((WaterRing *)self)->WaterRing::InitResources(); }
int _ZN13TreasureChest13InitResourcesEv(void *self)
{ return ((TreasureChest *)self)->TreasureChest::InitResources(); }
int _ZN13TreasureChest8BehaviorEv(void *self)
{ return ((TreasureChest *)self)->TreasureChest::Behavior(); }
}
