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
// table data_ov016_02114b00 (0x02114b00, 32) and _ZTV23FloatOnWaterPlatformJrb ==
// _ZTV13daSlide_Box_c (0x02114c8c, 37; an RTTI alias, one table two names) each
// carry the vtable signature (+4 relocates to Actor::BeforeInitResources, arm9
// 0x02011268) and are LEFT OUT of port/ov016_syms.txt (the ov080/ov095/ov064
// rule). So all four are HOST arrays this file fills. RockPillar's table has no
// _ZTV name in config -- it is the plain data symbol data_ov016_02114b00, and
// RockPillar_Spawn writes `p[0] = (int)data_ov016_02114b00`, so it too is a host
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
// data_ov016_02114b00 (RockPillar, Platform, 32): 0 Init func_ov016_02112e1c, 3
//   Cleanup func_ov016_02112ae4, 6 Behavior func_ov016_02112b50, 9 Render
//   func_ov016_02112b28 (slot-5 collision -> host copy), 16 D1 func_ov016_02112a00,
//   17 D0 func_ov016_02112a44, 31 Kill. D1/D0 ARE at 16/17 per the ROM relocs
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
//   RockPillar D1 (func_ov016_02112a00) / D0 (func_ov016_02112a44): store
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
// placeholder), and spell func_ov002_020aed18 (Enemy base D2) and data_020a0eac
// (the game heap), both hosted. So BOTH STAY IN THE SLICE and the fill just calls
// them (the RotatingFirebar/LavaBubble treatment).
//
// ---- FACTORIES -------------------------------------------------------------
//
// ShipUp_Spawn / ShipDown_Spawn install _ZTV6ShipUp directly (last vptr write),
// RockPillar_Spawn installs data_ov016_02114b00, SlidingBox_Spawn installs
// _ZTV23FloatOnWaterPlatformJrb, Unagi_Spawn installs _ZTV5Unagi -- so those need
// NO reseat wrapper. FloatOnWaterPlatformJrb_Spawn's LAST vptr write is VT1 (the
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
#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"
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
void *Unagi_Spawn(void);                             /* installs _ZTV5Unagi itself */

/* ---- SHIP_UP (57) own bodies. Init/Cleanup/Behavior are real methods, faced
   below; Render is the host copy; D1/D0 are host thunks. ------------------- */
int _ZN6ShipUp13InitResourcesEv(void *self);         /* slot 0, faced */
int *_ZN6ShipUpD1Ev(int *self);                      /* slot 16, .c, DTOR-PAIRS seat (0x0211260c) */
int *_ZN6ShipUpD0Ev(int *self);                      /* slot 17, .c, DTOR-PAIRS seat (0x02112650) */
int _ZN6ShipUp16CleanupResourcesEv(void *self);      /* slot 3, faced */
int _ZN6ShipUp8BehaviorEv(void *self);               /* slot 6, faced */
int _ZN6ShipUp6RenderEv(void *self);                 /* slot 9, HOST COPY */
void *ShipUp_Spawn(void);                            /* installs _ZTV6ShipUp itself */

/* ---- ROCK_PILLAR (58) own bodies (all func_ov016_* C linkage) ------------- */
int func_ov016_02112e1c(void *self);   /* slot 0, Init */
int func_ov016_02112ae4(void *self);   /* slot 3, Cleanup */
int func_ov016_02112b50(void *self);   /* slot 6, Behavior */
int func_ov016_02112b28(void *self);   /* slot 9, Render -- HOST COPY */
void *RockPillar_Spawn(void);          /* installs data_ov016_02114b00 itself */

/* ---- id 313 SLIDING_BOX == the DERIVED _ZN23FloatOnWaterPlatformJrb* class
   (installs _ZTV23FloatOnWaterPlatformJrb). Init/Cleanup faced; Behavior is a
   plain C-linkage body (the COUPLED FindWithActorID(0x39) one); Render host copy;
   D1/D0 host thunks. -- */
int _ZN23FloatOnWaterPlatformJrb13InitResourcesEv(void *self);    /* slot 0, faced */
int *_ZN23FloatOnWaterPlatformJrbD1Ev(int *self);    /* slot 16, .c, DTOR-PAIRS seat (0x02112ff8) */
int *_ZN23FloatOnWaterPlatformJrbD0Ev(int *self);    /* slot 17, .c, DTOR-PAIRS seat (0x02113044) */
int _ZN23FloatOnWaterPlatformJrb16CleanupResourcesEv(void *self); /* slot 3, faced */
int _ZN23FloatOnWaterPlatformJrb8BehaviorEv(void *self);          /* slot 6, .c C linkage */
int _ZN23FloatOnWaterPlatformJrb6RenderEv(void *self);            /* slot 9, HOST COPY */
void *SlidingBox_Spawn(void);              /* id 313: installs _ZTV23FloatOnWaterPlatformJrb directly */

/* ---- id 60 FLOAT_ON_WATER_PLATFORM_JRB == the daObjKi_Ita_c BASE (installs
   data_ov016_02114bcc). Its only own slot is Init (func_ov016_02112fa8);
   Behavior/Cleanup/Render are arm9 Platform defaults; D1/D0 (func_ov016_02112ef4/
   02112f44) write the placeholder -> host thunks. Factory writes VT1 -> reseat. */
int func_ov016_02112fa8(void *self);       /* slot 0, id 60 Init (daObjKi_Ita_c) */
int func_ov002_020b5be0(void *self);       /* slot 3, Platform base Cleanup */
int func_ov002_020b5c4c(void *self);       /* slot 6, Platform base Behavior */
int func_ov002_020b5c24(void *self);       /* slot 9, Platform base Render -- HOST COPY */
void *FloatOnWaterPlatformJrb_Spawn(void); /* last vptr write is VT1 -> reseat wrapper */

/* the D-tor chain's sub-object destructors, base D2s and heap, all C-linkage */
void _ZN18MovingMeshColliderD1Ev(void *);   /* MovingMeshCollider at +0x124 */
void _ZN5ModelD1Ev(void *);                 /* Model at +0xd4 */
void _ZN12WithMeshClsnD1Ev(void *);         /* WithMeshClsn at +0x324 (FloatOnWater) */
void _ZN5ActorD2Ev(void *);                 /* the Actor base D2 */
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;                 /* Memory::gameHeapPtr (== G0) */
void _ZN8Platform4KillEv(void *self);       /* slot 31, Platform's own */

/* the four derived vtables, HOST arrays this file fills; 31/32/32/37 slots.
   Defined int[] with C linkage to match the extern int _ZTV*[] spellings in
   include/decl_common.h that the factories and matched teardowns read (the
   montymole reading). data_ov016_02114b00 is RockPillar's UNNAMED table -- not
   in decl_common.h, so it is declared here too, an int[] host array. */
int _ZTV5Unagi[31];
int _ZTV6ShipUp[32];
int _ZTV23FloatOnWaterPlatformJrb[37];   /* id 313 SlidingBox (derived) */
DSSTATE_BEGIN
int data_ov016_02114b00[32];             /* id 58 RockPillar (daObjKi_Hasira_c) */
DSSTATE_END
DSSTATE_BEGIN
int data_ov016_02114bcc[32];             /* id 60 FloatOnWaterPlatformJrb (daObjKi_Ita_c base) */
DSSTATE_END
}

/* Two DISTINCT tables, two RTTI aliases (the decoy inversion, from relocs):
   - id 313 SlidingBox's derived table _ZTV23FloatOnWaterPlatformJrb is also
     spelled _ZTV13daSlide_Box_c (by SlidingBox_Spawn and the derived D1/D0).
   - id 60's base table data_ov016_02114bcc is spelled _ZTV13daObjKi_Ita_c (by
     FloatOnWaterPlatformJrb_Spawn's transient write and func_ov016_02112ef4/f44).
   Each RTTI name aliases to its OWN host array, not to the other. */
#pragma comment(linker, "/alternatename:__ZTV13daSlide_Box_c=__ZTV23FloatOnWaterPlatformJrb")
/* ShipUp's D1/D0 spell their table by its RTTI name; the ROM store relocates
   to ov016 0x02114a3c, _ZTV6ShipUp (the host array above). */
#pragma comment(linker, "/alternatename:__ZTV14daObjKi_Fune_c=__ZTV6ShipUp")
#pragma comment(linker, "/alternatename:__ZTV13daObjKi_Ita_c=_data_ov016_02114bcc")

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
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall jrb_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall jrb_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall jrb_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall jrb_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall jrb_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall jrb_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall jrb_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall jrb_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall jrb_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall jrb_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall jrb_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall jrb_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall jrb_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall jrb_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall jrb_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall jrb_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall jrb_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall jrb_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall jrb_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall jrb_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall jrb_aimed_actor(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }   /* slot 29, Actor's own default */

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
   func_ov002_020aed18 (Enemy base D2) and data_020a0eac (heap), all hosted. */
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
    vt[16] = (void *)una_d1;
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
{ _ZN8Platform4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 (DTOR-PAIRS seat): the matched flat-C pair behind ecx->arg
   adapters, replacing the host copies of the chain that stood here. The
   "shared placeholder" the copies were written to avoid is not one: every
   body's second vptr store relocates to ov002 0x0210ae38, the ONE Platform
   base table the port hosts as _ZTV10dBgActor_c / _ZTV8Platform
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
    vt[16] = (void *)shu_d1;    /* ROM word 16 (raw index; the earlier 14/15 was off-by-2) */
    vt[17] = (void *)shu_d0;    /* ROM word 17 */
    vt[31] = (void *)shu_kill;
}

// ============================================================================
// ROCK_PILLAR (58) -- a Platform, 32 slots (data_ov016_02114b00). D1/D0 at 16/17.
// ============================================================================
static int __fastcall rkp_init(void *s, void *)
{ return func_ov016_02112e1c(s); }
static int __fastcall rkp_clean(void *s, void *)
{ return func_ov016_02112ae4(s); }
static int __fastcall rkp_behavior(void *s, void *)
{ return func_ov016_02112b50(s); }
static int __fastcall rkp_render(void *s, void *)
{ port_actor_render_probe("ROCK_PILLAR", (char *)s + 0xd4);
  return func_ov016_02112b28(s); }                        /* HOST COPY (slot-5) */
static int __fastcall rkp_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 host thunks: func_ov016_02112a00 (D1) / func_ov016_02112a44 (D0) store
   _ZTV16daObjKi_Hasira_c / VT0 then _ZTV10dBgActor_c / VT1 (placeholders), so
   they are dropped from the slice. Same chain as ShipUp. */
static int __fastcall rkp_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)data_ov016_02114b00;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall rkp_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)data_ov016_02114b00;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_rock_pillar_vtable(void)
{
    void **vt = (void **)data_ov016_02114b00;
    hal_fill_platform_vtable();
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)rkp_init;
    vt[3]  = (void *)rkp_clean;
    vt[6]  = (void *)rkp_behavior;
    vt[9]  = (void *)rkp_render;
    vt[16] = (void *)rkp_d1;    /* ROM word 16, the daObjKi_Hasira_c shape */
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
{ return _ZN23FloatOnWaterPlatformJrb13InitResourcesEv(s); }
static int __fastcall sbx_clean(void *s, void *)
{ return _ZN23FloatOnWaterPlatformJrb16CleanupResourcesEv(s); }
static int __fastcall sbx_behavior(void *s, void *)
{ return _ZN23FloatOnWaterPlatformJrb8BehaviorEv(s); }    /* .c C-linkage, coupled */
static int __fastcall sbx_render(void *s, void *)
{ port_actor_render_probe("SLIDING_BOX", (char *)s + 0xd4);
  return _ZN23FloatOnWaterPlatformJrb6RenderEv(s); }       /* HOST COPY (slot-5) */
static int __fastcall sbx_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 (DTOR-PAIRS seat): the matched flat-C pair behind ecx->arg
   adapters, replacing the host copies of the chain that stood here. The
   "shared placeholder" the copies were written to avoid is not one: every
   body's second vptr store relocates to ov002 0x0210ae38, the ONE Platform
   base table the port hosts as _ZTV10dBgActor_c / _ZTV8Platform
   (hal/lk2_platform_dtor_seat.cpp), and the first store is this class's own
   table by its RTTI name, aliased onto the host array above. */
static int __fastcall sbx_d1(void *s, void *)
{ return (int)(size_t)_ZN23FloatOnWaterPlatformJrbD1Ev((int *)s); }
static int __fastcall sbx_d0(void *s, void *)
{ return (int)(size_t)_ZN23FloatOnWaterPlatformJrbD0Ev((int *)s); }

extern "C" void hal_fill_sliding_box_vtable(void)
{
    void **vt = (void **)_ZTV23FloatOnWaterPlatformJrb;
    hal_fill_platform_vtable();
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)sbx_init;
    vt[3]  = (void *)sbx_clean;
    vt[6]  = (void *)sbx_behavior;
    vt[9]  = (void *)sbx_render;
    vt[16] = (void *)sbx_d1;
    vt[17] = (void *)sbx_d0;
    vt[31] = (void *)sbx_kill;
}

// ============================================================================
// FLOAT_ON_WATER_PLATFORM_JRB (id 60) -- the daObjKi_Ita_c BASE table,
// data_ov016_02114bcc, a Platform with 32 slots. Its ONLY own overridden slots
// are Init (func_ov016_02112fa8) and D1/D0 (func_ov016_02112ef4/02112f44, host
// thunks); Behavior/Cleanup/Render are arm9 Platform defaults, so NO slot-5
// render collision and NO ship coupling. D1/D0 at ROM words 16/17.
// ============================================================================
static int __fastcall fow_init(void *s, void *)
{ return func_ov016_02112fa8(s); }
/* id 60's slots 3/6/9 are the daObjKi_Ita_c base's arm9 Platform defaults, not
   overridden by any own method -- face them here (slot 9 is the host-copied
   Platform Render in Jrb_Renders.cpp). */
static int __fastcall fow_clean(void *s, void *)
{ return func_ov002_020b5be0(s); }
static int __fastcall fow_behavior(void *s, void *)
{ return func_ov002_020b5c4c(s); }
static int __fastcall fow_render(void *s, void *)
{ port_actor_render_probe("FLOAT_ON_WATER_PLATFORM_JRB", (char *)s + 0xd4);
  return func_ov002_020b5c24(s); }                        /* HOST COPY (slot-5) */
static int __fastcall fow_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }                     /* slot 31 */
/* D1/D0 host thunks: func_ov016_02112ef4 (D1) / func_ov016_02112f44 (D0) store
   _ZTV13daObjKi_Ita_c then the _ZTV10dBgActor_c / VT1 placeholder, so they are
   dropped from the slice. Store the base table once and run the chain:
   MovingMeshCollider +0x124, Model +0xd4, then the Actor base D2. D0 also frees
   on the game heap. */
static int __fastcall fow_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)data_ov016_02114bcc;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall fow_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)data_ov016_02114bcc;
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_float_on_water_jrb_vtable(void)
{
    void **vt = (void **)data_ov016_02114bcc;
    hal_fill_platform_vtable();
    jrb_fill_shared_0_30(vt);
    vt[0]  = (void *)fow_init;
    vt[3]  = (void *)fow_clean;      /* Platform base Cleanup (func_ov002_020b5be0) */
    vt[6]  = (void *)fow_behavior;   /* Platform base Behavior (func_ov002_020b5c4c) */
    vt[9]  = (void *)fow_render;     /* Platform base Render, host copy (020b5c24) */
    vt[16] = (void *)fow_d1;
    vt[17] = (void *)fow_d0;
    vt[31] = (void *)fow_kill;
}

/* FloatOnWaterPlatformJrb_Spawn (id 60) writes the VT1 placeholder as its last
   vptr write (after _ZTV13daObjKi_Ita_c), so a raw spawn leaves the object on the
   placeholder; reseat slot 0 onto the host base table data_ov016_02114bcc (the
   Thwomp/bully treatment). SlidingBox_Spawn (id 313) installs its real table
   directly, so only the id-60 factory needs this wrapper. */
extern "C" void *port_factory_float_on_water_jrb(void)
{
    void *p = FloatOnWaterPlatformJrb_Spawn();
    if (p)
        *(void **)p = (void *)data_ov016_02114bcc;
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
#include "FloatOnWaterPlatformJrb.h"
#include "Unagi.h"
extern "C" {
int _ZN6ShipUp13InitResourcesEv(void *self)
{ return ((ShipUp *)self)->ShipUp::InitResources(); }
int _ZN6ShipUp16CleanupResourcesEv(void *self)
{ return ((ShipUp *)self)->ShipUp::CleanupResources(); }
int _ZN6ShipUp8BehaviorEv(void *self)
{ return ((ShipUp *)self)->ShipUp::Behavior(); }
int _ZN23FloatOnWaterPlatformJrb13InitResourcesEv(void *self)
{ return ((FloatOnWaterPlatformJrb *)self)->FloatOnWaterPlatformJrb::InitResources(); }
int _ZN23FloatOnWaterPlatformJrb16CleanupResourcesEv(void *self)
{ return ((FloatOnWaterPlatformJrb *)self)->FloatOnWaterPlatformJrb::CleanupResources(); }
int _ZN5Unagi13InitResourcesEv(void *self)
{ return ((Unagi *)self)->Unagi::InitResources(); }
}
