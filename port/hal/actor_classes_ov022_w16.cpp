// RUN LINKW WAVE 16 (lane w16-unblock): FALL_BLOCK_LLL (id 83, ov022), the
// sixth of Lethal Lava Land's eleven ids and the one the inferred-stub guard
// held back.
//
// hal/actor_classes_ov022.cpp seated five ids in wave 8 and left this one out
// for one reason: three of its five bodies carry the "recovered from vtable
// slot identity" marker, so inferred_stub_guard.py refused to let them be
// seated. All three were disassembled out of the shipped overlay image and
// ruled REAL DECOMP -- the derivation is in port/slice_w16a.txt, the per-body
// evidence in runs/linkw/status/w16.md, and the rulings are recorded against
// their symbols in port/tools/inferred_stub_debt.txt.
//
// NINE INSTANCES ON LEVEL 14, the same population FallBlockBfs has on level
// 37. 844-byte object.
//
// ---- THIS CLASS IS FallBlockBfs' TWIN --------------------------------------
//
// id 139 FALL_BLOCK_BFS (ov045) and id 45 FALL_BLOCK_WF (ov015) are the same
// ROM class compiled into other level overlays, and both are already seated.
// Slots 6/9/27/31 are the SAME four ov098 bodies those two seat, all four
// already in walk_window.map, and Init/Cleanup are veneers onto the same ov098
// pair (0x0213a794 / 0x0213a2cc) through this overlay's own file table
// 0x0211427c. So the only genuinely new code here is the host array, the two
// destructors and the sinit; everything else is a second consumer of bindings
// that already exist.
//
// Slot 31 is NOT Platform::Kill. It is func_ov098_0213a17c, the KillOrWhatever
// poof-dust death that slot 27 arms -- the reading hal/actor_classes_ov045.cpp
// records for the identical tail. The eight OTHER ov022 tables do end in
// Platform::Kill; this one does not, because its base is the ov098 class and
// not Platform. That is why the width could not be taken from a sibling.
//
// ---- WHY THE BASE FILL IS REPLICATED RATHER THAN CALLED --------------------
//
// ov22_fill_shared and its two dozen thunks are `static` in
// hal/actor_classes_ov022.cpp, so a new TU cannot call them. This file
// replicates them under its own prefix, which is what hal/
// actor_classes_ov002_w15.cpp did for the same reason. port_ov22_bringup IS
// extern and IS done-guarded, so it is called rather than duplicated.

#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

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
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */

/* the class's own four, plus the factory (port/slice_w16a.txt) */
int func_ov022_02112448(void *self);      /* slot 0  InitResources veneer   */
int func_ov022_02112434(void *self);      /* slot 3  CleanupResources veneer*/
int *func_ov022_02112380(int *self);      /* slot 16 D1                     */
/* Slot 17, the deleting destructor. Its TU is src/func_ov022_021123d0.c but it
   EMITS daObjFl_Fall_Block_c_OnYoshiTryEat -- the recovered name is wrong (the
   body is the D0; OnYoshiTryEat is slot 18) and the TU took its name from the
   recovery, so that is the spelling the linker sees. Declared under the name
   the object file actually exports, seated into the slot the ROM table puts it
   in. The wrong name is on the decomp-side correction list. */
int *daObjFl_Fall_Block_c_OnYoshiTryEat(int *self);   /* slot 17 D0 */
void *FallBlockLll_Spawn(void);

/* the four ov098 base bodies, all already in walk_window.map */
int func_ov098_0213a36c(char *self);      /* slot 6  Behavior */
int func_ov098_0213a314(char *self);      /* slot 9  Render   */
void func_ov098_0213a284(char *self);     /* slot 27 */
void func_ov098_0213a17c(char *self);     /* slot 31 */

/* the sinit that constructs the file table's SharedFilePtr pair */
void __sinit_ov022_02112fe4(void);

void port_ov22_bringup(void);                     /* hal/actor_classes_ov022 */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry      */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *what, char *model);

DSSTATE_BEGIN
void *_ZTV20daObjFl_Fall_Block_c[32];
DSSTATE_END
}

// ---- the fill's own bring-up ------------------------------------------------
// port_ov22_bringup runs four of ov022's ten sinits and is done-guarded, so it
// is safe to call from a fifth fill. It does NOT run __sinit_ov022_02112fe4,
// which is the one that constructs data_ov022_02114640 / data_ov022_02114648 --
// the two SharedFilePtrs the file table at 0x0211427c points at, and therefore
// the ones this class's Init veneer dereferences. Running it from here, behind
// this file's own done guard, adds it without editing the shared bring-up.
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "fb83_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_fb83_bringup_done;
DSSTATE_END

static void fb83_bringup(void)
{
    if (g_fb83_bringup_done)
        return;
    g_fb83_bringup_done = 1;
    port_ov22_bringup();
    __sinit_ov022_02112fe4();
}

// The registry rewrites the SpawnInfo's +0 word to this wrapper, and the
// wrapper forces the vptr to the host array after Spawn returns. That is what
// makes FallBlockLll_Spawn.c's placeholder stores inert: the ROM body stores
// the intermediate base 0x0213c5bc and then its own table, and whichever way
// the two spellings in that TU resolve, this store is the last one. Identical
// to port_factory_fall_block_bfs.
extern "C" void *port_factory_fall_block_lll(void)
{
    void *p = FallBlockLll_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV20daObjFl_Fall_Block_c;
    return p;
}

static void fb83_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: FallBlockLll vtable slot %d is not hosted (actor "
                 "id %u %s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m,
                    "unhosted FallBlockLll vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define FB83_TRAP(n) \
    static int __fastcall fb83_trap##n(void *s, void *) \
    { fb83_trap_report(s, n); return 0; }
FB83_TRAP(13) FB83_TRAP(14)
#undef FB83_TRAP

static int __fastcall fb83_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall fb83_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall fb83_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall fb83_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall fb83_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall fb83_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall fb83_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall fb83_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall fb83_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall fb83_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall fb83_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall fb83_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall fb83_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall fb83_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall fb83_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall fb83_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall fb83_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall fb83_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall fb83_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall fb83_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall fb83_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* the class's own six */
static int __fastcall fb83_init(void *s, void *)
{ return func_ov022_02112448(s); }
static int __fastcall fb83_clean(void *s, void *)
{ return func_ov022_02112434(s); }
static int __fastcall fb83_behavior(void *s, void *)
{ return func_ov098_0213a36c((char *)s); }
static int __fastcall fb83_render(void *s, void *)
{ port_actor_render_probe("FALL_BLOCK_LLL", (char *)s + 0xd4);
  return func_ov098_0213a314((char *)s); }
static int __fastcall fb83_d1(void *s, void *)
{ return (int)(size_t)func_ov022_02112380((int *)s); }
static int __fastcall fb83_d0(void *s, void *)
{ return (int)(size_t)daObjFl_Fall_Block_c_OnYoshiTryEat((int *)s); }
/* Slot 27 is OnHitByMegaChar(Player &): the caller pushes the player, so the
   veneer takes the third parameter to pop it even though the ov098 body reads
   only the receiver -- the fb_slot27 contract in hal/actor_classes_wf.cpp. */
static int __fastcall fb83_slot27(void *s, void *, void *)
{ func_ov098_0213a284((char *)s); return 0; }
static int __fastcall fb83_slot31(void *s, void *)
{ func_ov098_0213a17c((char *)s); return 0; }

extern "C" void hal_fill_fall_block_lll_vtable(void)
{
    fb83_bringup();
    /* THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug,
       repro in hal/actor_classes_ov002g200.cpp. */
    void *volatile *vt = (void *volatile *)_ZTV20daObjFl_Fall_Block_c;
    vt[0]  = (void *)fb83_init;
    vt[1]  = (void *)fb83_binit;
    vt[2]  = (void *)fb83_ainit;
    vt[3]  = (void *)fb83_clean;
    vt[4]  = (void *)fb83_bclean;
    vt[5]  = (void *)fb83_aclean;
    vt[6]  = (void *)fb83_behavior;
    vt[7]  = (void *)fb83_bbeh;
    vt[8]  = (void *)fb83_abeh;
    vt[9]  = (void *)fb83_render;
    vt[10] = (void *)fb83_bren;
    vt[11] = (void *)fb83_aren;
    vt[12] = (void *)fb83_pdes;
    vt[13] = (void *)fb83_trap13;
    vt[14] = (void *)fb83_trap14;
    vt[15] = (void *)fb83_heap;
    vt[16] = (void *)fb83_d1;
    vt[17] = (void *)fb83_d0;
    vt[18] = (void *)fb83_yoshi;
    vt[19] = (void *)fb83_turn_egg;
    vt[20] = (void *)fb83_v50;
    vt[21] = (void *)fb83_pounded;
    vt[22] = (void *)fb83_atk1;
    vt[23] = (void *)fb83_atk2;
    vt[24] = (void *)fb83_kicked;
    vt[25] = (void *)fb83_pushed;
    vt[26] = (void *)fb83_cannon;
    vt[27] = (void *)fb83_slot27;
    vt[28] = (void *)fb83_under;
    vt[29] = (void *)fb83_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)fb83_slot31;
}
