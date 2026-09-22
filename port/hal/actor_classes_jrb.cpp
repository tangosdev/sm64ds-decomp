// GATE 188: the four host vtables for Jolly Roger Bay's own overlay (ov016,
// data/stage/kaizoku_irie, base 0x021111a0) mover classes -- UNAGI (242, an
// Enemy), SHIP_UP (57), ROCK_PILLAR (58) and FLOAT_ON_WATER_PLATFORM_JRB (60,
// all Platforms). SHIP_DOWN (56) shares _ZTV6ShipUp; SLIDING_BOX (313) shares
// _ZTV23FloatOnWaterPlatformJrb.
//
// Same law as hal/actor_classes_ov064.cpp (gate 177) and its gate-178 sibling:
// ROM slot order (fill index vt[i] == ROM vtable word i), __fastcall thunks that
// call the class's C body or a QUALIFIED base method, the SRET slot 30 trapped,
// the interaction tail bound to the Actor base bodies. The colliders these
// movers ride on (MovingMeshCollider) are seated by hal/mmc_vtable.cpp's
// hal_fill_moving_mesh_collider_vtable, a SEPARATE sub-object table -- these are
// the ACTOR vtables and do NOT re-seat it.
//
// ---- THE VTABLES STAY OUT OF THE MOUNT -------------------------------------
//
// _ZTV5Unagi (0x02114958, 31 words), _ZTV6ShipUp (0x02114a3c, 32), the RockPillar
// table _ZTV10RockPillar (0x02114b00, 32) and _ZTV23FloatOnWaterPlatformJrb ==
// _ZTV13daSlide_Box_c (0x02114c8c, 37; an RTTI alias, one table two names) each
// carry the vtable signature (+4 relocates to Actor::BeforeInitResources, arm9
// 0x02011268) and are LEFT OUT of port/ov016_syms.txt (the ov080/ov095/ov064
// rule). So all four are HOST arrays this file fills. RockPillar's table has no
// _ZTV name in config -- it is the plain data symbol _ZTV10RockPillar, and
// daObjKi_Hasira_c_classInit writes `p[0] = (int)_ZTV10RockPillar`, so it too is a host
// array here, defined int[] and NOT in the mount (confirmed: ov016_syms.txt
// leaves it out).
//
// ---- SLOT MAPS, read off ov016's relocs (word = (addr - base)/4) -----------
//
// _ZTV6ShipUp (Platform, 32): 0 Init 0x0211283c, 3 Cleanup 0x021126f0, 6 Behavior
//   0x0211276c, 9 Render 0x02112744 (slot-5 collision -> host copy), 14 D1
//   0x0211260c, 15 D0 0x02112650, 31 Kill 0x020ee55c. D1/D0 ARE at 14/15, NOT
//   16/17 -- ShipUp is the daObjKi_Fune_c base shape, a different layout from the
//   Enemy-derived RotatingFirebar (16/17). The ROM word indices are authoritative.
//
// _ZTV10RockPillar (RockPillar, Platform, 32): 0 Init _ZN10RockPillar13InitResourcesEv, 3
//   Cleanup _ZN10RockPillar16CleanupResourcesEv, 6 Behavior _ZN10RockPillar8BehaviorEv, 9 Render
//   _ZN10RockPillar6RenderEv (slot-5 collision -> host copy), 16 D1 _ZN10RockPillarD1Ev,
//   17 D0 _ZN10RockPillarD0Ev, 31 Kill. D1/D0 ARE at 16/17 per the ROM relocs
//   (0x02114b40/0x02114b44) -- the daObjKi_Hasira_c shape.
//
// _ZTV23FloatOnWaterPlatformJrb (Platform, 37): 0 Init 0x02113434, 3 Cleanup
//   0x021130ec, 6 Behavior 0x02113158, 9 Render 0x02113130 (slot-5 collision ->
//   host copy), 16 D1 0x02112ff8, 17 D0 0x02113044, 31 Kill. Words 32..36 carry
//   no relocs (base-default padding of the 37-word span); nothing dispatches
//   through them, the array is zeroed there.
//
// _ZTV5Unagi (Enemy, 31): 0 Init 0x021121c0, 3 Cleanup 0x02111f38, 6 Behavior
//   0x02112010 (HOST COPY, PMF dispatch, in port/unmatched/Unagi_StateDispatch.cpp),
//   9 Render 0x02111f84 (NOT a collision -- it calls _ZN5Model6RenderEPK7Vector3
//   by C name, so it stays in the slice and the fill thunks to it directly), 12
//   OnPendingDestroy 0x02111f80, 16 D1 0x021111a0, 17 D0 0x02111208. No Kill (an
//   Enemy is 31 slots).
//
// ---- D1/D0: WHO STAYS IN THE SLICE, WHO BECOMES A HOST THUNK ---------------
//
// The teardowns that store a SHARED PLACEHOLDER as their last-visible vptr write
// (_ZTV10dBgActor_c -- 196+ TUs spell that one name for as many different tables,
// so one host definition would satisfy them all with the wrong bytes and nothing
// would say so; the CastleWater/SphereClsn reading, gate 177's bully treatment)
// are NOT in the slice; the host thunks below run the matched chain with the
// derived table stored once and the placeholder elided:
//
//   ShipUp D1 (0x0211260c) / D0 (0x02112650): store _ZTV14daObjKi_Fune_c then
//     OVERWRITE with _ZTV10dBgActor_c (placeholder) -> HOST THUNKS. Chain:
//     MovingMeshCollider +0x124, Model +0xd4, Actor D2; D0 also Deallocate.
//   RockPillar D1 (_ZN10RockPillarD1Ev) / D0 (_ZN10RockPillarD0Ev): store
//     _ZTV16daObjKi_Hasira_c / VT0 then _ZTV10dBgActor_c / VT1 (placeholders) ->
//     HOST THUNKS. Chain: MovingMeshCollider +0x124, Model +0xd4, Actor D2; D0
//     also Deallocate.
//   FloatOnWater D1 (0x02112ff8) / D0 (0x02113044): store _ZTV13daSlide_Box_c,
//     run WithMeshClsn +0x324, THEN store _ZTV10dBgActor_c (placeholder) -> HOST
//     THUNKS. Chain: WithMeshClsn +0x324, MovingMeshCollider +0x124, Model +0xd4,
//     Actor D2; D0 also Deallocate.
//
// Unagi D1 (_ZN5UnagiD1Ev.c) / D0 (_ZN5UnagiD0Ev.cpp) store _ZTV5Unagi itself --
// the class's OWN host table, already hosted -- as their vptr write (no
// placeholder), and spell _ZN12dEnemyBase_cD2Ev (Enemy base D2) and data_020a0eac
// (the game heap), both hosted. So BOTH STAY IN THE SLICE and the fill just calls
// them (the RotatingFirebar/LavaBubble treatment).
//
// ---- FACTORIES -------------------------------------------------------------
//
// daObjKi_Fune_c_classInit_KI_FUNE_UP / daObjKi_Fune_c_classInit_KI_FUNE install _ZTV6ShipUp directly (last vptr write),
// daObjKi_Hasira_c_classInit installs _ZTV10RockPillar, daSlide_Box_c_classInit installs
// _ZTV23FloatOnWaterPlatformJrb, daMoray_c_classInit installs _ZTV5Unagi -- so those need
// NO reseat wrapper. daObjKi_Ita_c_classInit's LAST vptr write is VT1 (the
// shared placeholder) after _ZTV13daObjKi_Ita_c, so a raw spawn leaves the object
// on the placeholder; port_factory_float_on_water_jrb reseats slot 0 onto the
// host table (the Thwomp/bully treatment).
//
// ---- RENDER COLLISION ------------------------------------------------------
//
// All three Platform Renders (ShipUp/RockPillar/FloatOnWater) dispatch the Model
// at +0xd4 through a ROM-order six-virtual local shadow (slot 5) -- the
// Whomp/RotatingFirebar collision -- so they are host copies in
// port/unmatched/Jrb_Renders.cpp and out of slice_gate188.txt. Unagi's Render is
// NOT a collision (Model::Render by C name) and stays in the slice.
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
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* ---- UNAGI (242) own bodies. Render + D1/D0 stay in the slice. -------------
   Behavior is the host copy (PMF dispatch, extern-C in Unagi_StateDispatch.cpp);
   the fill binds it at slot 6. */
int _ZN5Unagi13InitResourcesEv(void *self);          /* slot 0, faced below */
int _ZN5Unagi8BehaviorEv(void *self);                /* slot 6, HOST COPY */
int _ZN5Unagi6RenderEv(void *self);                  /* slot 9, .c, Model by C name */
int _ZN5Unagi16CleanupResourcesEv(void);             /* slot 3, .c C linkage */
void _ZN5Unagi16OnPendingDestroyEv(void);            /* slot 12, .c C linkage, empty */
int *_ZN5UnagiD1Ev(void *self);                      /* slot 16, .c, spells _ZTV5Unagi */
void *_ZN5UnagiD0Ev(void *self);                     /* slot 17, .cpp, spells _ZTV5Unagi */
void *daMoray_c_classInit(void);                             /* installs _ZTV5Unagi itself */

/* ---- SHIP_UP (57) own bodies. Init/Cleanup/Behavior are real methods, faced
   below; Render is the host copy; D1/D0 are host thunks. ------------------- */
int _ZN6ShipUp13InitResourcesEv(void *self);         /* slot 0, faced */
int *_ZN6ShipUpD1Ev(int *self);                      /* slot 16, .c, DTOR-PAIRS seat (0x0211260c) */
int *_ZN6ShipUpD0Ev(int *self);                      /* slot 17, .c, DTOR-PAIRS seat (0x02112650) */
int _ZN6ShipUp16CleanupResourcesEv(void *self);      /* slot 3, faced */
int _ZN6ShipUp8BehaviorEv(void *self);               /* slot 6, faced */
int _ZN6ShipUp6RenderEv(void *self);                 /* slot 9, HOST COPY */
void *daObjKi_Fune_c_classInit_KI_FUNE_UP(void);                            /* installs _ZTV6ShipUp itself */

/* ---- ROCK_PILLAR (58) own bodies (all func_ov016_* C linkage) ------------- */
int _ZN10RockPillar13InitResourcesEv(void *self);   /* slot 0, Init */
int _ZN10RockPillar16CleanupResourcesEv(void *self);   /* slot 3, Cleanup */
int _ZN10RockPillar8BehaviorEv(void *self);   /* slot 6, Behavior */
int _ZN10RockPillar6RenderEv(void *self);   /* slot 9, Render -- HOST COPY */
void *daObjKi_Hasira_c_classInit(void);          /* installs _ZTV10RockPillar itself */

/* ---- id 313 SLIDING_BOX == the DERIVED _ZN23FloatOnWaterPlatformJrb* class
   (installs _ZTV23FloatOnWaterPlatformJrb). Init/Cleanup faced; Behavior is a
   plain C-linkage body (the COUPLED FindWithActorID(0x39) one); Render host copy;
   D1/D0 host thunks. -- */
int _ZN10SlidingBox13InitResourcesEv(void *self);    /* slot 0, faced */
int *_ZN10SlidingBoxD1Ev(int *self);    /* slot 16, .c, DTOR-PAIRS seat (0x02112ff8) */
int *_ZN10SlidingBoxD0Ev(int *self);    /* slot 17, .c, DTOR-PAIRS seat (0x02113044) */
int _ZN10SlidingBox16CleanupResourcesEv(void *self); /* slot 3, faced */
int _ZN10SlidingBox8BehaviorEv(void *self);          /* slot 6, .c C linkage */
int _ZN10SlidingBox6RenderEv(void *self);            /* slot 9, HOST COPY */
void *daSlide_Box_c_classInit(void);              /* id 313: installs _ZTV23FloatOnWaterPlatformJrb directly */

/* ---- id 60 FLOAT_ON_WATER_PLATFORM_JRB == the daObjKi_Ita_c BASE (installs
   _ZTV13daObjKi_Ita_c). Its only own slot is Init (_ZN13daObjKi_Ita_c13InitResourcesEv);
   Behavior/Cleanup/Render are arm9 Platform defaults; D1/D0 (_ZN13daObjKi_Ita_cD1Ev/
   02112f44) write the placeholder -> host thunks. Factory writes VT1 -> reseat. */
int _ZN13daObjKi_Ita_c13InitResourcesEv(void *self);       /* slot 0, id 60 Init (daObjKi_Ita_c) */
int _ZN17daObjFloatBoard_c16CleanupResourcesEv(void *self);       /* slot 3, Platform base Cleanup */
int _ZN17daObjFloatBoard_c8BehaviorEv(void *self);       /* slot 6, Platform base Behavior */
int _ZN17daObjFloatBoard_c6RenderEv(void *self);       /* slot 9, Platform base Render -- HOST COPY */
void *daObjKi_Ita_c_classInit(void); /* last vptr write is VT1 -> reseat wrapper */

/* the D-tor chain's sub-object destructors, base D2s and heap, all C-linkage */
void _ZN10dBgW_KcMbgD1Ev(void *);   /* MovingMeshCollider at +0x124 */
void _ZN5ModelD1Ev(void *);                 /* Model at +0xd4 */
void _ZN10dBgCh_ActrD1Ev(void *);         /* WithMeshClsn at +0x324 (FloatOnWater) */
void _ZN8dActor_cD2Ev(void *);                 /* the Actor base D2 */
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;                 /* Memory::gameHeapPtr (== G0) */
void _ZN10dBgActor_c4KillEv(void *self);       /* slot 31, Platform's own */

/* the four derived vtables, HOST arrays this file fills; 31/32/32/37 slots.
   Defined int[] with C linkage to match the extern int _ZTV*[] spellings in
   include/decl_common.h that the factories and matched teardowns read (the
   montymole reading). _ZTV10RockPillar is RockPillar's UNNAMED table -- not
   in decl_common.h, so it is declared here too, an int[] host array. */
int _ZTV5Unagi[31];
int _ZTV6ShipUp[32];
int _ZTV23FloatOnWaterPlatformJrb[37];   /* id 313 SlidingBox (derived) */
DSSTATE_BEGIN
int _ZTV10RockPillar[32];             /* id 58 RockPillar (daObjKi_Hasira_c) */
DSSTATE_END
DSSTATE_BEGIN
int _ZTV13daObjKi_Ita_c[32];             /* id 60 FloatOnWaterPlatformJrb (daObjKi_Ita_c base) */
DSSTATE_END
}

/* Two DISTINCT tables, two RTTI aliases (the decoy inversion, from relocs):
   - id 313 SlidingBox's derived table _ZTV23FloatOnWaterPlatformJrb is also
     spelled _ZTV13daSlide_Box_c (by daSlide_Box_c_classInit and the derived D1/D0).
   - id 60's base table _ZTV13daObjKi_Ita_c is spelled _ZTV13daObjKi_Ita_c (by
     daObjKi_Ita_c_classInit's transient write and _ZN13daObjKi_Ita_cD1Ev/f44).
   Each RTTI name aliases to its OWN host array, not to the other. */
#pragma comment(linker, "/alternatename:__ZTV13daSlide_Box_c=__ZTV23FloatOnWaterPlatformJrb")
/* ShipUp's D1/D0 spell their table by its RTTI name; the ROM store relocates
   to ov016 0x02114a3c, _ZTV6ShipUp (the host array above). */
#pragma comment(linker, "/alternatename:__ZTV14daObjKi_Fune_c=__ZTV6ShipUp")

// ---- the trap --------------------------------------------------------------
static void jrb_trap_report(void *self, int slot)
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
#define JRB_TRAP(n) \
    static int __fastcall jrb_trap##n(void *s, void *) \
    { jrb_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
JRB_TRAP(13) JRB_TRAP(14)
#undef JRB_TRAP

// ---- the shared 0..30 half -------------------------------------------------
static int __fastcall jrb_binit(void *s, void *)
{ return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall jrb_ainit(void *s, void *, unsigned a)
{ _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int __fastcall jrb_bclean(void *s, void *)
{ return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall jrb_aclean(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int __fastcall jrb_bbeh(void *s, void *)
{ return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall jrb_abeh(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int __fastcall jrb_bren(void *s, void *)
{ return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall jrb_aren(void *s, void *, unsigned a)
{ ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int __fastcall jrb_pdes(void *s, void *)
{ ((fBase_c *)s)->fBase_c::OnPendingDestroy(); return 0; }
static int __fastcall jrb_heap(void *s, void *)
{ return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int __fastcall jrb_yoshi(void *s, void *)
{ return _ZN8dActor_c13OnYoshiTryEatEv(s); }
static int __fastcall jrb_egg(void *s, void *, void *p)
{ _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall jrb_v50(void *s, void *)
{ return _ZN8dActor_c9Virtual50Ev(s); }
static int __fastcall jrb_pounded(void *s, void *, void *o)
{ _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall jrb_atk1(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall jrb_atk2(void *s, void *, void *o)
{ _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall jrb_kicked(void *s, void *, void *o)
{ _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int __fastcall jrb_pushed(void *s, void *, void *o)
{ _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int __fastcall jrb_cannon(void *s, void *, void *o)
{ _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall jrb_mega(void *s, void *, void *p)
{ _ZN8dActor_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall jrb_under(void *s, void *, void *o)
{ _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall jrb_aimed_actor(void *s, void *)
{ return _ZN8dActor_c16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own default */

/* Fill slots 1..30 of a 31/32/37-slot table with the shared bodies. None of the
   four JRB classes override OnAimedAtWithEgg (slot 29), so slot 29 takes Actor's
   own default here (the gate-178 pattern). The caller writes 0/3/6/9, the D1/D0
   pair at its class's ROM indices, (Unagi) 12, and (Platforms) 31. */
static void jrb_fill_shared_0_30(void **vt)
{
    vt[1]  = (void *)jrb_binit;
    vt[2]  = (void *)jrb_ainit;
    vt[4]  = (void *)jrb_bclean;
    vt[5]  = (void *)jrb_aclean;
    vt[7]  = (void *)jrb_bbeh;
    vt[8]  = (void *)jrb_abeh;
    vt[10] = (void *)jrb_bren;
    vt[11] = (void *)jrb_aren;
    vt[12] = (void *)jrb_pdes;
    vt[13] = (void *)jrb_trap13;
    vt[14] = (void *)jrb_trap14;
    vt[15] = (void *)jrb_heap;
    vt[18] = (void *)jrb_yoshi;
    vt[19] = (void *)jrb_egg;
    vt[20] = (void *)jrb_v50;
    vt[21] = (void *)jrb_pounded;
    vt[22] = (void *)jrb_atk1;
    vt[23] = (void *)jrb_atk2;
    vt[24] = (void *)jrb_kicked;
    vt[25] = (void *)jrb_pushed;
    vt[26] = (void *)jrb_cannon;
    vt[27] = (void *)jrb_mega;
    vt[28] = (void *)jrb_under;
    vt[29] = (void *)jrb_aimed_actor;
    vt[30] = (void *)port_actor_s30_base;
}

/* The Platform base table must be filled before the factories run Platform's
   constructor and before the host D-thunks reseat between the member teardowns.
   hal_fill_platform_vtable owns that fill (gate 177 already relies on it). */
extern "C" void hal_fill_platform_vtable(void);

// ============================================================================
// UNAGI (242) -- an Enemy, 31 slots
// ============================================================================
static int __fastcall una_init(void *s, void *)
{ return _ZN5Unagi13InitResourcesEv(s); }
static int __fastcall una_clean(void *s, void *)
{ (void)s; return _ZN5Unagi16CleanupResourcesEv(); }   /* .c body takes void */
static int __fastcall una_behavior(void *s, void *)
{ return _ZN5Unagi8BehaviorEv(s); }                     /* HOST COPY (PMF dispatch) */
static int __fastcall una_render(void *s, void *)
{ port_actor_render_probe("UNAGI", (char *)s + 0x350);
  return _ZN5Unagi6RenderEv(s); }                        /* .c, Model by C name */
static int __fastcall una_pdes(void *s, void *)
{ (void)s; _ZN5Unagi16OnPendingDestroyEv(); return 0; }  /* .c body takes void, empty */
/* D1/D0 stay in the slice: they spell _ZTV5Unagi (the class's own host table),
   _ZN12dEnemyBase_cD2Ev (Enemy base D2) and data_020a0eac (heap), all hosted. */
static int __fastcall una_d1(void *s, void *)
{ return (int)(size_t)_ZN5UnagiD1Ev(s); }
static int __fastcall una_d0(void *s, void *)
{ return (int)(size_t)_ZN5UnagiD0Ev(s); }

extern "C" void hal_fill_unagi_vtable(void)
{
    void **vt = (void **)_ZTV5Unagi;
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)una_init;
    vt[3]  = (void *)una_clean;
    vt[6]  = (void *)una_behavior;
    vt[9]  = (void *)una_render;
    vt[12] = (void *)una_pdes;   /* Unagi's own OnPendingDestroy override */
    vt[16] = (void *)PORT_D16(una_d1);
    vt[17] = (void *)una_d0;
}

// ============================================================================
// SHIP_UP (57) -- a Platform, 32 slots. D1/D0 at ROM words 14/15.
// ============================================================================
static int __fastcall shu_init(void *s, void *)
{ return _ZN6ShipUp13InitResourcesEv(s); }
static int __fastcall shu_clean(void *s, void *)
{ return _ZN6ShipUp16CleanupResourcesEv(s); }
static int __fastcall shu_behavior(void *s, void *)
{ return _ZN6ShipUp8BehaviorEv(s); }
static int __fastcall shu_render(void *s, void *)
{ port_actor_render_probe("SHIP_UP", (char *)s + 0xd4);
  return _ZN6ShipUp6RenderEv(s); }                        /* HOST COPY (slot-5) */
static int __fastcall shu_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 (DTOR-PAIRS seat): the matched flat-C pair behind ecx->arg
   adapters, replacing the host copies of the chain that stood here. The
   "shared placeholder" the copies were written to avoid is not one: every
   body's second vptr store relocates to ov002 0x0210ae38, the ONE Platform
   base table the port hosts as _ZTV10dBgActor_c / _ZTV10dBgActor_c
   (hal/lk2_platform_dtor_seat.cpp), and the first store is this class's own
   table by its RTTI name, aliased onto the host array above. */
static int __fastcall shu_d1(void *s, void *)
{ return (int)(size_t)_ZN6ShipUpD1Ev((int *)s); }
static int __fastcall shu_d0(void *s, void *)
{ return (int)(size_t)_ZN6ShipUpD0Ev((int *)s); }

extern "C" void hal_fill_ship_up_vtable(void)
{
    void **vt = (void **)_ZTV6ShipUp;
    hal_fill_platform_vtable();
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)shu_init;
    vt[3]  = (void *)shu_clean;
    vt[6]  = (void *)shu_behavior;
    vt[9]  = (void *)shu_render;
    vt[16] = (void *)PORT_D16(shu_d1);    /* ROM word 16 (raw index; the earlier 14/15 was off-by-2) */
    vt[17] = (void *)shu_d0;    /* ROM word 17 */
    vt[31] = (void *)shu_kill;
}

// ============================================================================
// ROCK_PILLAR (58) -- a Platform, 32 slots (_ZTV10RockPillar). D1/D0 at 16/17.
// ============================================================================
static int __fastcall rkp_init(void *s, void *)
{ return _ZN10RockPillar13InitResourcesEv(s); }
static int __fastcall rkp_clean(void *s, void *)
{ return _ZN10RockPillar16CleanupResourcesEv(s); }
static int __fastcall rkp_behavior(void *s, void *)
{ return _ZN10RockPillar8BehaviorEv(s); }
static int __fastcall rkp_render(void *s, void *)
{ port_actor_render_probe("ROCK_PILLAR", (char *)s + 0xd4);
  return _ZN10RockPillar6RenderEv(s); }                        /* HOST COPY (slot-5) */
static int __fastcall rkp_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 host thunks: _ZN10RockPillarD1Ev (D1) / _ZN10RockPillarD0Ev (D0) store
   _ZTV16daObjKi_Hasira_c / VT0 then _ZTV10dBgActor_c / VT1 (placeholders), so
   they are dropped from the slice. Same chain as ShipUp. */
/* SLOT 16 IS THE MATCHED TU NOW, run link100 lane TAIL. The note above names
   the blocker as the two PLACEHOLDERS, and a per-source -D is what removes a
   placeholder: the relocations inside _ZN10RockPillarD1Ev's own span say which
   addresses its two pooled words are --
     0x02112a3c -> ov016 0x02114b00 (_ZTV10RockPillar, RockPillar's table)
     0x02112a40 -> ov002 0x0210ae38 (_ZTV10dBgActor_c)
   -- so port/CMakeLists.txt binds them under slice_gate216 and the body is this
   thunk plus the base-table store the ROM makes and the thunk left out.
   Nothing dispatches between the two stores; the caller is
   ActorBase::AfterCleanupResources, which frees the object next.
   SLOT 17 IS SEATED NOW (gate 228), by the same -D that unblocked slot 16 plus
   a ruling. _ZN10RockPillarD0Ev carries the inferred-stub marker and lane
   STUBADJ ruled it REAL DECOMP against the ROM. Table word 0x02114b00 + 17*4 =
   0x02114b44 relocates to 0x02112a44, kind:function(arm,size=0x58), and its own
   pool says which addresses its three words are --
     0x02112a90 -> ov016 0x02114b00 (_ZTV10RockPillar)
     0x02112a94 -> ov002 0x0210ae38 (_ZTV10dBgActor_c)
     0x02112a98 -> main  0x020a0eac (the game heap word)
   -- so the base-table store this thunk left out comes back here too. */
extern "C" int *_ZN10RockPillarD1Ev(int *t);   /* ov016 0x02112a00, slot 16 */
extern "C" int *_ZN10RockPillarD0Ev(int *t);   /* ov016 0x02112a44, slot 17 */
static int __fastcall rkp_d1(void *s, void *)
{ return (int)(size_t)_ZN10RockPillarD1Ev((int *)s); }
static int __fastcall rkp_d0(void *s, void *)
{ return (int)(size_t)_ZN10RockPillarD0Ev((int *)s); }

extern "C" void hal_fill_rock_pillar_vtable(void)
{
    void **vt = (void **)_ZTV10RockPillar;
    hal_fill_platform_vtable();
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)rkp_init;
    vt[3]  = (void *)rkp_clean;
    vt[6]  = (void *)rkp_behavior;
    vt[9]  = (void *)rkp_render;
    vt[16] = (void *)PORT_D16(rkp_d1);    /* ROM word 16, the daObjKi_Hasira_c shape */
    vt[17] = (void *)rkp_d0;    /* ROM word 17 */
    vt[31] = (void *)rkp_kill;
}

// ============================================================================
// SLIDING_BOX (id 313) -- the DERIVED _ZN23FloatOnWaterPlatformJrb* class,
// _ZTV23FloatOnWaterPlatformJrb, a Platform with 37 slots. D1/D0 at 16/17.
// Carries the slot-5 Render collision and the COUPLED FindWithActorID(0x39)
// Behavior. The config names id 313 "SLIDING_BOX" but its methods are the
// decomp's FloatOnWaterPlatformJrb (the decoy inversion, from relocs).
// ============================================================================
static int __fastcall sbx_init(void *s, void *)
{ return _ZN10SlidingBox13InitResourcesEv(s); }
static int __fastcall sbx_clean(void *s, void *)
{ return _ZN10SlidingBox16CleanupResourcesEv(s); }
static int __fastcall sbx_behavior(void *s, void *)
{ return _ZN10SlidingBox8BehaviorEv(s); }    /* .c C-linkage, coupled */
static int __fastcall sbx_render(void *s, void *)
{ port_actor_render_probe("SLIDING_BOX", (char *)s + 0xd4);
  return _ZN10SlidingBox6RenderEv(s); }       /* HOST COPY (slot-5) */
static int __fastcall sbx_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 (DTOR-PAIRS seat): the matched flat-C pair behind ecx->arg
   adapters, replacing the host copies of the chain that stood here. The
   "shared placeholder" the copies were written to avoid is not one: every
   body's second vptr store relocates to ov002 0x0210ae38, the ONE Platform
   base table the port hosts as _ZTV10dBgActor_c / _ZTV10dBgActor_c
   (hal/lk2_platform_dtor_seat.cpp), and the first store is this class's own
   table by its RTTI name, aliased onto the host array above. */
static int __fastcall sbx_d1(void *s, void *)
{ return (int)(size_t)_ZN10SlidingBoxD1Ev((int *)s); }
static int __fastcall sbx_d0(void *s, void *)
{ return (int)(size_t)_ZN10SlidingBoxD0Ev((int *)s); }

extern "C" void hal_fill_sliding_box_vtable(void)
{
    void **vt = (void **)_ZTV23FloatOnWaterPlatformJrb;
    hal_fill_platform_vtable();
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)sbx_init;
    vt[3]  = (void *)sbx_clean;
    vt[6]  = (void *)sbx_behavior;
    vt[9]  = (void *)sbx_render;
    vt[16] = (void *)PORT_D16(sbx_d1);
    vt[17] = (void *)sbx_d0;
    vt[31] = (void *)sbx_kill;
}

// ============================================================================
// FLOAT_ON_WATER_PLATFORM_JRB (id 60) -- the daObjKi_Ita_c BASE table,
// _ZTV13daObjKi_Ita_c, a Platform with 32 slots. Its ONLY own overridden slots
// are Init (_ZN13daObjKi_Ita_c13InitResourcesEv) and D1/D0 (_ZN13daObjKi_Ita_cD1Ev/02112f44, host
// thunks); Behavior/Cleanup/Render are arm9 Platform defaults, so NO slot-5
// render collision and NO ship coupling. D1/D0 at ROM words 16/17.
// ============================================================================
static int __fastcall fow_init(void *s, void *)
{ return _ZN13daObjKi_Ita_c13InitResourcesEv(s); }
/* id 60's slots 3/6/9 are the daObjKi_Ita_c base's arm9 Platform defaults, not
   overridden by any own method -- face them here (slot 9 is the host-copied
   Platform Render in Jrb_Renders.cpp). */
static int __fastcall fow_clean(void *s, void *)
{ return _ZN17daObjFloatBoard_c16CleanupResourcesEv(s); }
static int __fastcall fow_behavior(void *s, void *)
{ return _ZN17daObjFloatBoard_c8BehaviorEv(s); }
static int __fastcall fow_render(void *s, void *)
{ port_actor_render_probe("FLOAT_ON_WATER_PLATFORM_JRB", (char *)s + 0xd4);
  return _ZN17daObjFloatBoard_c6RenderEv(s); }                        /* HOST COPY (slot-5) */
static int __fastcall fow_kill(void *s, void *)
{ _ZN10dBgActor_c4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 host thunks: _ZN13daObjKi_Ita_cD1Ev (D1) / _ZN13daObjKi_Ita_cD0Ev (D0) store
   _ZTV13daObjKi_Ita_c then the _ZTV10dBgActor_c / VT1 placeholder, so they are
   dropped from the slice. Store the base table once and run the chain:
   MovingMeshCollider +0x124, Model +0xd4, then the Actor base D2. D0 also frees
   on the game heap. */
static int __fastcall fow_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV13daObjKi_Ita_c;
    _ZN10dBgW_KcMbgD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return (int)(size_t)s;
}
/* slot 17, the ROM's own D0, GATE 228. Table word 0x02114bcc + 17*4 =
   0x02114c10 relocates to 0x02112f44, kind:function(arm,size=0x64). This is the
   daObjKi_Ita_c BASE table's deleting destructor, not SLIDING_BOX's 0x02113044
   -- the two are different classes and the header above keeps them apart. It
   makes THREE vptr stores where this thunk made one, and its own pool says
   which: 0x02112f98 -> ov016 0x02114bcc, 0x02112f9c -> ov002 0x02108fdc (the
   base table hal/actor_base_tables_ov002.cpp hosts), 0x02112fa0 -> ov002
   0x0210ae38 (_ZTV10dBgActor_c), 0x02112fa4 -> 0x020a0eac. Held out until now by
   the inferred-stub marker, which lane STUBADJ ruled REAL DECOMP. */
extern "C" int *_ZN13daObjKi_Ita_cD0Ev(int *t);   /* ov016 0x02112f44, slot 17 */
static int __fastcall fow_d0(void *s, void *)
{ return (int)(size_t)_ZN13daObjKi_Ita_cD0Ev((int *)s); }

extern "C" void hal_fill_float_on_water_jrb_vtable(void)
{
    void **vt = (void **)_ZTV13daObjKi_Ita_c;
    hal_fill_platform_vtable();
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)fow_init;
    vt[3]  = (void *)fow_clean;      /* Platform base Cleanup (_ZN17daObjFloatBoard_c16CleanupResourcesEv) */
    vt[6]  = (void *)fow_behavior;   /* Platform base Behavior (_ZN17daObjFloatBoard_c8BehaviorEv) */
    vt[9]  = (void *)fow_render;     /* Platform base Render, host copy (020b5c24) */
    vt[16] = (void *)PORT_D16(fow_d1);
    vt[17] = (void *)fow_d0;
    vt[31] = (void *)fow_kill;
}

/* daObjKi_Ita_c_classInit (id 60) writes the VT1 placeholder as its last
   vptr write (after _ZTV13daObjKi_Ita_c), so a raw spawn leaves the object on the
   placeholder; reseat slot 0 onto the host base table _ZTV13daObjKi_Ita_c (the
   Thwomp/bully treatment). daSlide_Box_c_classInit (id 313) installs its real table
   directly, so only the id-60 factory needs this wrapper. */
extern "C" void *port_factory_float_on_water_jrb(void)
{
    void *p = daObjKi_Ita_c_classInit();
    if (p)
        *(void **)p = (void *)_ZTV13daObjKi_Ita_c;
    return p;
}

// ---- method faces ----------------------------------------------------------
// The C-named references the vtables take onto the real MSVC methods against
// include/. ShipUp's Init/Cleanup/Behavior, FloatOnWater's Init/Cleanup and
// Unagi's Init are `Class::Method` .cpp definitions (MSVC mangles them off the
// Itanium name), so face them here. FloatOnWater's Behavior, Unagi's Cleanup/
// OnPendingDestroy/Render and every RockPillar func_ body are already C-linkage
// bodies -- no face. The three collided Renders (ShipUp/FloatOnWater/RockPillar)
// are host copies in port/unmatched/Jrb_Renders.cpp -- declared, not faced here.
#include "ShipUp.h"
#include "SlidingBox.h"
#include "Unagi.h"
extern "C" {
int _ZN6ShipUp13InitResourcesEv(void *self)
{ return ((ShipUp *)self)->ShipUp::InitResources(); }
int _ZN6ShipUp16CleanupResourcesEv(void *self)
{ return ((ShipUp *)self)->ShipUp::CleanupResources(); }
int _ZN6ShipUp8BehaviorEv(void *self)
{ return ((ShipUp *)self)->ShipUp::Behavior(); }
int _ZN10SlidingBox13InitResourcesEv(void *self)
{ return ((SlidingBox *)self)->SlidingBox::InitResources(); }
int _ZN10SlidingBox16CleanupResourcesEv(void *self)
{ return ((SlidingBox *)self)->SlidingBox::CleanupResources(); }
int _ZN5Unagi13InitResourcesEv(void *self)
{ return ((Unagi *)self)->Unagi::InitResources(); }
}
