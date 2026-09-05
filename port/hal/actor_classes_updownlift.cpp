// GATE 173: level 13's (Hazy Maze Cave) hosting wave, second class:
// UP_DOWN_LIFT_HMC (33, ov095), the up-and-down platform lift (RTTI
// daUdlift_c). One class, THREE ids: the factory trio UpDownLiftBbh/Hmc/Rr
// (0x021367fc/cc/9c) all install this one table _ZTV13UpDownLiftBbh and the
// class reads unk_00c back in InitResources to tell which variant it is
// (0x20 -> 0, 0x21 -> 1, 0x83 -> 2). Level 6 (Bob-omb Battlefield) shares
// ov095 with SEESAW_BOB and names id 32 (Bbh); this gate hosts id 33 (Hmc),
// the 0x21 variant.
//
// Same law as hal/actor_classes_bob_world.cpp (the SeesawBob section, the same
// ov095 mount) and hal/actor_classes_hmc.cpp (gate 172, the Crate): MSVC slot
// order, __fastcall thunks that call QUALIFIED against include/UpDownLiftBbh.h,
// unhosted slots trap by name -- and THIRTY-TWO SLOTS, the Platform shape. The
// class overrides 0/3/6/9/16/17 and NOTHING in the Platform tail: every slot
// 18..31 is the ROM's base-class body, so the tail traps per slot exactly as
// SeesawBob's does, except where the base body is already linked and reached on
// a clean run (12, 15). Every slot below was read out of _ZTV13UpDownLiftBbh
// (ov095 0x02137628) with its relocations applied.
//
// ov095 is a fresh per-symbol mount (gate 83, ov095_syms.txt); SeesawBob's
// SpawnInfo and file tables came with it, and this gate adds UpDownLiftHmc's
// SpawnInfo, its two variant-indexed file-pointer tables (data_ov095_02136f68
// the model, data_ov095_02136f74 the collision mesh) and its CLPS-block table
// (data_ov095_021375a4). The SharedFilePtrs those tables index are already in
// the mount (the two ov095 sinits build them, hal/actor_overlays.cpp).
//
// BEHAVIOR IS A HOST COPY (port/unmatched/UpDownLiftBbh_Behavior.cpp), the
// WHOMP treatment: the ROM's Behavior dispatches its state machine through an
// incomplete-class pointer-to-member over data_ov095_02137910 (five 8-byte
// {fn, delta} records, one per state, indexed by mState directly -- NOT the
// 2*i pairs the Crate uses). MSVC widens that PMF to 16 bytes and mis-strides,
// and the record .fn words are DS code addresses, so the source TU cannot go in
// the slice. The host copy reads the record as a plain {fn, delta} and calls
// it, and its port_updownlift_states_seat() rewrites the five SOURCE statics
// before __sinit_ov095_0213722c copies them into the runtime table. The other
// five methods (Init/Cleanup/Render + both dtors) are matched src,
// slice_gate173.txt.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "UpDownLiftBbh.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);

extern "C" {
/* the shared lifecycle halves, the same functions every fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
/* the Actor interaction tail, every one the ROM's own base body and every one
   already linked -- bound rather than trapped so a Yoshi lick or a ground
   pound on the lift runs the ROM's default instead of aborting */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p); /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);               /* slot 29 */
void _ZN8Platform4KillEv(void *self);                        /* slot 31 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* UpDownLiftBbh's own bodies. Init/Cleanup/Render are real C++ methods against
   include/UpDownLiftBbh.h (matched src, slice_gate173.txt), called qualified by
   the thunks. Behavior is the C-linkage HOST COPY
   (port/unmatched/UpDownLiftBbh_Behavior.cpp): defining the mangled MSVC method
   here would collide with the qualified call, so the host copy is C-named and
   the thunk calls the C name -- the Coin/WaterBomb reading. The seat rewrites
   the five state statics before the ov095 sinit copies them. */
int _ZN13UpDownLiftBbh8BehaviorEv(void *self);   /* port/unmatched host copy */
int _ZN13UpDownLiftBbh6RenderEv(void *self);     /* ModelAnim slot-5 host copy */
void *_ZN13UpDownLiftBbhD1Ev(void *self);        /* slot 16, .c extern "C" */
void *_ZN13UpDownLiftBbhD0Ev(void *self);        /* slot 17, .c extern "C" */
void port_updownlift_states_seat(void);          /* port/unmatched */

/* The one array the ROM factory installs (UpDownLiftHmc_Spawn does
   `p[0] = (int)_ZTV13UpDownLiftBbh`); thirty-two slots. Defined here, not just
   declared: the `int` type and C linkage match the `extern int
   _ZTV13UpDownLiftBbh[]` in decl_common.h that the factory and both destructor
   TUs reference. */
int _ZTV13UpDownLiftBbh[32];
}

/* The class's two destructors store the vtable back by its RTTI names: VT0 =
   _ZTV10daUdlift_c (this class's own table), VT1 = _ZTV10dBgActor_c (its
   dBgActor base, already a linked base vtable). Alias the daUdlift_c spelling
   onto the host array so the dtors write the same storage the factory installs
   -- the SeesawBob daObjSeesaw_c precedent. dBgActor_c is left alone: it is a
   real linked vtable and writing it back is the ROM's own second store. */
#pragma comment(linker, "/alternatename:__ZTV10daUdlift_c=__ZTV13UpDownLiftBbh")

/* The C++/C linkage bridge, the gate-172 reading: InitResources declares the
   two variant file tables `extern int ...[]` and CleanupResources declares the
   same two `extern void* ...[]`, both OUTSIDE their extern "C" blocks, so MSVC
   mangles four C++ data spellings of the two mounted C symbols. (The CLPS
   table 021375a4 rides decl_common.h's extern "C" block and needs nothing.) */
#pragma comment(linker, "/alternatename:?data_ov095_02136f68@@3PAHA=_data_ov095_02136f68")
#pragma comment(linker, "/alternatename:?data_ov095_02136f74@@3PAHA=_data_ov095_02136f74")
#pragma comment(linker, "/alternatename:?data_ov095_02136f68@@3PAPAXA=_data_ov095_02136f68")
#pragma comment(linker, "/alternatename:?data_ov095_02136f74@@3PAPAXA=_data_ov095_02136f74")

/* InitResources takes the ADDRESS of MeshColliderBase::UpdatePosWithTransform
   through `extern int X;` -- a SCALAR int at C++ linkage, so a third spelling
   (@@3HA) beside SeesawBob's @@3PAHA and cxx_aliases' C name, all one static
   method. */
#pragma comment(linker, "/alternatename:?_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_@@3HA=?UpdatePosWithTransform@MeshColliderBase@@SAXAAU1@PAUActor@@AAUClsnResult@@AAUVector3@@PAUVector3_16@@4@Z")

// ---- the trap --------------------------------------------------------------
static void udl_trap_report(void *self, int slot)
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
#define UDL_TRAP(n) \
    static int __fastcall udl_trap##n(void *s, void *) \
    { udl_trap_report(s, n); return 0; }
/* 13/14 keep the gate-16/gate-33 trap: they are ActorBase::Virtual34/Virtual38,
   which are NOT linked (their closure is the SolidHeap subsystem, the
   actor_classes_bob_world reading) and no clean run of a lift dispatches them.
   30 is Actor::OnAimedAtWithEggReturnVec, also not linked. Everything else in
   the tail is a linked base body and is BOUND below. */
UDL_TRAP(13) UDL_TRAP(14)
#undef UDL_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall udl_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall udl_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall udl_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall udl_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall udl_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall udl_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall udl_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall udl_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
/* slot 12 (OnPendingDestroy) and slot 15 (OnHeapCreated) are the linked
   ActorBase bodies, reached on a clean run: bound by a qualified base call, the
   crate's hmc_pdes/hmc_heap reading. */
static int __fastcall udl_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall udl_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }

// ---- UP_DOWN_LIFT_HMC's own slots ------------------------------------------
static int __fastcall udl_init(void *s, void *)
{ return ((UpDownLiftBbh *)s)->UpDownLiftBbh::InitResources(); }
static int __fastcall udl_clean(void *s, void *)
{ return ((UpDownLiftBbh *)s)->UpDownLiftBbh::CleanupResources(); }
static int __fastcall udl_behavior(void *s, void *)
{ return _ZN13UpDownLiftBbh8BehaviorEv(s); }
static int __fastcall udl_render(void *s, void *)
{ port_actor_render_probe("UP_DOWN_LIFT_HMC", (char *)s + 0xd4);
  /* the C name, not the C++ method: Render is a ModelAnim slot-5 host copy in
     port/unmatched/ModelAnim_Renders.cpp (the Whomp/Scuttlebug case), so the
     matched src is dropped from slice_gate173.txt and this dispatches the host
     copy by its extern-"C" name, the way udl_behavior does. */
  return _ZN13UpDownLiftBbh6RenderEv(s); }
static int __fastcall udl_d1(void *s, void *)
{ return (int)(size_t)_ZN13UpDownLiftBbhD1Ev(s); }
static int __fastcall udl_d0(void *s, void *)
{ return (int)(size_t)_ZN13UpDownLiftBbhD0Ev(s); }

// ---- the Actor interaction tail, the ROM's own base bodies -----------------
static int __fastcall udl_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19's second argument arrives on the stack, the cap_egg reading */
static int __fastcall udl_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall udl_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall udl_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall udl_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall udl_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall udl_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall udl_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall udl_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall udl_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall udl_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall udl_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall udl_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

extern "C" void hal_fill_up_down_lift_hmc_vtable(void)
{
    void **vt = (void **)_ZTV13UpDownLiftBbh;
    /* the state seat is NOT called here: hal/actor_overlays.cpp runs it after
       port_ov095_syms_patch and before __sinit_ov095_0213722c, the established
       seat order. Calling it here too would set its done flag before the
       syms patch and turn the real call into a no-op. */
    vt[0]  = (void *)udl_init;
    vt[1]  = (void *)udl_binit;
    vt[2]  = (void *)udl_ainit;
    vt[3]  = (void *)udl_clean;
    vt[4]  = (void *)udl_bclean;
    vt[5]  = (void *)udl_aclean;
    vt[6]  = (void *)udl_behavior;
    vt[7]  = (void *)udl_bbeh;
    vt[8]  = (void *)udl_abeh;
    vt[9]  = (void *)udl_render;
    vt[10] = (void *)udl_bren;
    vt[11] = (void *)udl_aren;
    vt[12] = (void *)udl_pdes;
    vt[13] = (void *)udl_trap13;
    vt[14] = (void *)udl_trap14;
    vt[15] = (void *)udl_heap;
    vt[16] = (void *)udl_d1;
    vt[17] = (void *)udl_d0;
    /* the Platform tail (18..31): every ROM slot is a base body, and every one
       that is linked is bound so normal play (a Yoshi lick, a ground pound, a
       cannon hit on the lift) runs the ROM's default instead of aborting. Only
       30 (OnAimedAtWithEggReturnVec, not linked) stays a trap. */
    vt[18] = (void *)udl_yoshi;
    vt[19] = (void *)udl_egg;
    vt[20] = (void *)udl_v50;
    vt[21] = (void *)udl_pounded;
    vt[22] = (void *)udl_atk1;
    vt[23] = (void *)udl_atk2;
    vt[24] = (void *)udl_kicked;
    vt[25] = (void *)udl_pushed;
    vt[26] = (void *)udl_cannon;
    vt[27] = (void *)udl_mega;
    vt[28] = (void *)udl_under;
    vt[29] = (void *)udl_aimed;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)udl_kill;
}
