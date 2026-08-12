// GATE 172: level 13's (Hazy Maze Cave) hosting wave, first class: CRATE
// (194, ov098), the breakable wooden box (RTTI daObjBlockS_c).
//
// Same law as hal/actor_classes_bob_world.cpp and hal/actor_classes_bbh.cpp --
// ROM slot order, __fastcall thunks that call QUALIFIED, unhosted slots trap by
// name -- and THIRTY-TWO SLOTS, the Platform shape SeesawBob has: an 18-slot
// Actor core plus the Platform tail, of which Crate owns 18 (OnYoshiTryEat), 19
// (OnTurnIntoEgg), 21 (OnGroundPounded) and 31 (Kill). Every slot below was
// read out of _ZTV5Crate (ov098 0x0213c534) with its relocations applied.
//
// ov098 is already mounted per symbol (ov098_syms.txt, the WaterBomb /
// FallBlockWf / ExclamationSwitch gates); Crate_SpawnInfo (0x0213c510, +4 reads
// 194) and the fourteen state-table source statics were mounted with it. The
// class's seven-state machine dispatches through data_ov098_0213c878, seated by
// port_crate_states_seat (port/unmatched/Crate_StateDispatch.cpp) before
// __sinit_ov098_0213c058 copies it -- the two tiny dispatcher TUs are host
// copies there (the OneUpMushroom PMF case); everything else in the closure is
// matched src, slice_gate172.txt.
//
// Slot 21 (OnGroundPounded, func_ov098_02139e44) takes only `this` and
// dispatches slot 31 (Kill) virtually, which is why 31 is bound rather than
// trapped: the first ground-pound of a crate reaches it.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "Crate.h"

extern "C" {
/* the shared lifecycle halves, the same functions every fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc.cpp: per-actor decline */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* Crate's own C-linkage bodies (matched src, slice_gate172.txt) */
void *_ZN5CrateD1Ev(void *self);              /* slot 16, .cpp extern "C" */
void *_ZN5CrateD0Ev(void *self);              /* slot 17, .c */
int func_ov098_02139e64(unsigned char *c);    /* slot 18, OnYoshiTryEat */
void func_ov098_02139e78(char *self, char *other); /* slot 19, OnTurnIntoEgg */
void func_ov098_02139e44(void *self);         /* slot 21, OnGroundPounded */
void func_ov098_02139070(char *self);         /* slot 31, Kill */
void *Crate_Spawn(void);

/* The one array the ROM factory installs (Crate_Spawn does
   `*(void **)p = &_ZTV5Crate`); thirty-two slots. Defined here, not just
   declared: the `int` type and C linkage match the `extern int _ZTV5Crate[]`
   in the destructor TUs. */
int _ZTV5Crate[32];
}

/* Crate_Spawn.cpp declares the table `extern void *_ZTV5Crate;` OUTSIDE any
   extern "C" block, so MSVC mangles that one reference as a C++ data name.
   Point the mangled spelling at the C symbol -- the data_02082128 precedent. */
#pragma comment(linker, "/alternatename:?_ZTV5Crate@@3PAXA=__ZTV5Crate")

/* The C++/C linkage bridge, the gate-42 pragma block's reading. Crate's
   InitResources and CleanupResources declare data_ov098_0213c4c8 (the model
   file-pointer table, mounted in ov098_syms.txt) outside their extern "C"
   blocks and with two different element types, so MSVC mangles two C++ data
   spellings of the one C symbol. func_ov098_021390ec spells its call to
   func_ov002_020ef228 the same C++ way, and names WithMeshClsn::GetWallResult
   with the pointer return the real record has, while the compiled matched
   body (slice_gate10.txt) spells the int-return form -- same __thiscall body,
   the return rides eax either way. */
#pragma comment(linker, "/alternatename:?data_ov098_0213c4c8@@3PADA=_data_ov098_0213c4c8")
#pragma comment(linker, "/alternatename:?data_ov098_0213c4c8@@3PAHA=_data_ov098_0213c4c8")
#pragma comment(linker, "/alternatename:?func_ov002_020ef228@@YAHPAXH@Z=_func_ov002_020ef228")
#pragma comment(linker, "/alternatename:?GetWallResult@WithMeshClsn@@QBEPAUClsnResult@@XZ=?GetWallResult@WithMeshClsn@@QBEHXZ")

// ---- the trap --------------------------------------------------------------
static void hmc_trap_report(void *self, int slot)
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
#define HMC_TRAP(n) \
    static int __fastcall hmc_trap##n(void *s, void *) \
    { hmc_trap_report(s, n); return 0; }
HMC_TRAP(13) HMC_TRAP(14) HMC_TRAP(20) HMC_TRAP(22)
HMC_TRAP(25) HMC_TRAP(26) HMC_TRAP(27) HMC_TRAP(28) HMC_TRAP(29) HMC_TRAP(30)
#undef HMC_TRAP
/* Slots 23/24 take the three-parameter shape so they emit `ret 4`: their one
   dispatch site each is now thiscall (Actor_OnAttacked2Dispatch.cpp /
   Actor_OnKickedDispatch.cpp), which pushes one argument the callee must pop.
   Body identical to the HMC_TRAP shape; only the pop contract widens. */
static int __fastcall hmc_trap23(void *s, void *, void *)
{ hmc_trap_report(s, 23); return 0; }
static int __fastcall hmc_trap24(void *s, void *, void *)
{ hmc_trap_report(s, 24); return 0; }

// ---- the shared half -------------------------------------------------------
static int __fastcall hmc_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall hmc_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall hmc_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall hmc_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall hmc_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall hmc_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall hmc_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall hmc_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall hmc_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall hmc_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }

// ---- CRATE's own slots -----------------------------------------------------
static int __fastcall crate_init(void *s, void *)
{ return ((Crate *)s)->Crate::InitResources(); }
static int __fastcall crate_clean(void *s, void *)
{ return ((Crate *)s)->Crate::CleanupResources(); }
static int __fastcall crate_behavior(void *s, void *)
{ return ((Crate *)s)->Crate::Behavior(); }
static int __fastcall crate_render(void *s, void *)
{ port_actor_render_probe("CRATE", (char *)s + 0xd4);
  return ((Crate *)s)->Crate::Render(); }
static int __fastcall crate_d1(void *s, void *)
{ return (int)(size_t)_ZN5CrateD1Ev(s); }
static int __fastcall crate_d0(void *s, void *)
{ return (int)(size_t)_ZN5CrateD0Ev(s); }
static int __fastcall crate_yoshi(void *s, void *)
{ return func_ov098_02139e64((unsigned char *)s); }
/* slot 19's second argument arrives on the stack, the cap_egg reading */
static int __fastcall crate_egg(void *s, void *, void *o)
{ func_ov098_02139e78((char *)s, (char *)o); return 0; }
/* Slot 21 is OnGroundPounded(Actor &other), and its argument arrives the way
   slot 19's does: on the stack. The ov098 body reads only r0 and ignores the
   pounder, but the HOST caller still pushes it, because the slot's
   declaration in include/Actor.h has the parameter. A __fastcall thunk
   carries its first two parameters in ecx/edx and pops nothing; only a
   declared THIRD parameter lands on the stack and makes the thunk `ret 4`.
   Written with two, the pushed word outlives the call, the caller's epilogue
   pops one slot short, and its `ret` takes the saved EBP -- a stack address
   -- as the return address. Declare the argument, ignore it like the ROM. */
static int __fastcall crate_pounded(void *s, void *, void *)
{ func_ov098_02139e44(s); return 0; }
static int __fastcall crate_kill(void *s, void *)
{ func_ov098_02139070((char *)s); return 0; }

extern "C" void hal_fill_crate_vtable(void)
{
    void **vt = (void **)_ZTV5Crate;
    vt[0]  = (void *)crate_init;
    vt[1]  = (void *)hmc_binit;
    vt[2]  = (void *)hmc_ainit;
    vt[3]  = (void *)crate_clean;
    vt[4]  = (void *)hmc_bclean;
    vt[5]  = (void *)hmc_aclean;
    vt[6]  = (void *)crate_behavior;
    vt[7]  = (void *)hmc_bbeh;
    vt[8]  = (void *)hmc_abeh;
    vt[9]  = (void *)crate_render;
    vt[10] = (void *)hmc_bren;
    vt[11] = (void *)hmc_aren;
    vt[12] = (void *)hmc_pdes;
    vt[13] = (void *)hmc_trap13;
    vt[14] = (void *)hmc_trap14;
    vt[15] = (void *)hmc_heap;
    vt[16] = (void *)crate_d1;
    vt[17] = (void *)crate_d0;
    vt[18] = (void *)crate_yoshi;
    vt[19] = (void *)crate_egg;
    /* the Platform tail (20..31): trapped per slot except the two Crate owns */
    vt[20] = (void *)hmc_trap20;
    vt[21] = (void *)crate_pounded;
    vt[22] = (void *)hmc_trap22;
    vt[23] = (void *)hmc_trap23;
    vt[24] = (void *)hmc_trap24;
    vt[25] = (void *)hmc_trap25;
    vt[26] = (void *)hmc_trap26;
    vt[27] = (void *)hmc_trap27;
    vt[28] = (void *)hmc_trap28;
    vt[29] = (void *)hmc_trap29;
    vt[30] = (void *)hmc_trap30;
    vt[31] = (void *)crate_kill;
}
