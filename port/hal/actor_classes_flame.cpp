// GATE 175: BLUE_FLAME (317) and RED_FLAME (316), the cave's flames (ov002).
//
// One class, two ids: RedFlame_Spawn (0x020b59e0) and BlueFlame_Spawn
// (0x020b59a8) both install _ZTV9BlueFlame (ov002 0x02108f38, RTTI
// daObjFire_c), and both factories are matched src. Level 13 names RED_FLAME
// x17, the largest skipped class on any mounted level; hosting the pair is the
// single biggest census win available.
//
// Same law as hal/actor_classes_montymole.cpp -- ROM slot order, __fastcall
// thunks, unhosted slots trap by name -- and THIRTY-ONE SLOTS, the Enemy shape:
// the reloc span [0x02108f38, 0x02108fb4). The class owns 0 (InitResources), 6
// (Behavior), 16 (D1), 17 (D0), 18 (func_ov002_020b59a0, the OnYoshiTryEat
// override) and 19 (func_ov002_020b599c, the OnTurnIntoEgg no-op). Slots 3 and
// 9 are ActorBase's own base bodies in the ROM table (CleanupResources
// 0x02043bf0, Render 0x02043af0 -- the flame draws through particles in its
// Behavior, not a model), the IceSlideManager reading. 13/14 (Virtual34/38)
// trap as everywhere; 30 traps as the SRET body no thunk shape models; the
// interaction tail binds the ROM's own linked base bodies.
//
// Every class body is C-linkage matched src (slice_gate175.txt) -- .c files,
// no method faces -- and there is NO pointer-to-member state table, so no host
// copies and no seat: the flame's whole life is straight code. The one bridge
// this gate adds is Player::Burn -- the Behavior's contact path calls the C
// name _ZN6Player4BurnEv while the matched body is the real MSVC method
// (src/_ZN6Player4BurnEv.cpp, in the slice); the face below bridges cdecl to
// __thiscall. Burn changes the player into the burn state, whose record
// (data_ov002_021100dc, bss) __sinit_ov002_021019d0 already constructs and
// whose St_BurnFire bodies are already linked and faced (method_faces.cpp), so
// catching fire is a fully hosted path.
//
// The id pair was cross-checked: ACTOR_SPAWN_TABLE entries 0x02090d54/0x02090d58
// ((addr - 0x02090864) / 4 = 316 / 317) reloc to RedFlame_SpawnInfo (0x02108ef8)
// and BlueFlame_SpawnInfo (0x02108f14); the registry cross-checks each record's
// +4 halfword at boot.
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
#include <cstdlib>

#include "Actor.h"
#include "dtor_faces_cpp.h"
#include "ActorBase.h"
#include "Player.h"

extern "C" {
/* the shared lifecycle halves, the same functions every fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
int _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);               /* slot 29 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */

/* BlueFlame's own C-linkage bodies (matched src, slice_gate175.txt) */
int _ZN9BlueFlame13InitResourcesEv(char *t);      /* slot 0  */
int _ZN9BlueFlame8BehaviorEv(char *self);         /* slot 6  */
int *_ZN9BlueFlameD0Ev(int *t);                   /* slot 17 */
int func_ov002_020b59a0(void);                    /* slot 18, yoshi override */
void func_ov002_020b599c(void);                   /* slot 19, egg no-op */
void *BlueFlame_Spawn(void);
void *RedFlame_Spawn(void);

/* the D1 chain's sub-object destructor, C-linkage in the build */
void _ZN18MovingCylinderClsnD1Ev(void *);  /* the MovingCylinderClsn at +0xe4 */
void *_ZN5ActorD2Ev(void *);

/* The one array both ROM factories install; thirty-one slots. Defined here:
   the `int` type and C linkage match the `extern int _ZTV9BlueFlame[]` in
   decl_common.h that the .c factories and D0 read. */
int _ZTV9BlueFlame[31];
}

/* BlueFlameD0.c stores _ZTV11daObjFire_c (the RTTI spelling dsd names at the
   same address); both references are C linkage, the daChoropu_c precedent. */
#pragma comment(linker, "/alternatename:__ZTV11daObjFire_c=__ZTV9BlueFlame")

/* Player::Burn's face: BlueFlame's Behavior (a .c TU) spells the C name; the
   matched body is the real MSVC method in the slice. */
extern "C" int _ZN6Player4BurnEv(void *self)
{ return ((Player *)self)->Player::Burn(); }

// ---- the trap --------------------------------------------------------------
static void fl_trap_report(void *self, int slot)
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
#define FL_TRAP(n) \
    static int __fastcall fl_trap##n(void *s, void *) \
    { fl_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked, the sibling trap); 30 is the
   SRET OnAimedAtWithEggReturnVec no thunk shape models. */
FL_TRAP(13) FL_TRAP(14)
#undef FL_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall fl_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall fl_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall fl_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall fl_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall fl_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall fl_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall fl_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall fl_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall fl_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall fl_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
/* slots 3 and 9: the ROM table's own base bodies, reached on a clean run */
static int __fastcall fl_clean_base(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall fl_render_base(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall fl_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall fl_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall fl_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall fl_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall fl_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall fl_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall fl_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall fl_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall fl_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall fl_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ---- the flame's own slots -------------------------------------------------
static int __fastcall fl_init(void *s, void *)
{ return _ZN9BlueFlame13InitResourcesEv((char *)s); }
static int __fastcall fl_behavior(void *s, void *)
{ return _ZN9BlueFlame8BehaviorEv((char *)s); }
static int __fastcall fl_yoshi(void *s, void *)
{ (void)s; return func_ov002_020b59a0(); }
static int __fastcall fl_egg(void *s, void *, void *)
{ (void)s; func_ov002_020b599c(); return 0; }
static int __fastcall fl_d0(void *s, void *)
{ return (int)(size_t)_ZN9BlueFlameD0Ev((int *)s); }
/* D1, the HauntedChair treatment: the matched D1 is the auto-emitted-member-
   dtor .cpp form, so the thunk runs the D0 chain minus the tail Deallocate. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (fl_d1) spelled the same chain by hand. */

extern "C" void hal_fill_blue_flame_vtable(void)
{
    void **vt = (void **)_ZTV9BlueFlame;
    vt[0]  = (void *)fl_init;
    vt[1]  = (void *)fl_binit;
    vt[2]  = (void *)fl_ainit;
    vt[3]  = (void *)fl_clean_base;
    vt[4]  = (void *)fl_bclean;
    vt[5]  = (void *)fl_aclean;
    vt[6]  = (void *)fl_behavior;
    vt[7]  = (void *)fl_bbeh;
    vt[8]  = (void *)fl_abeh;
    vt[9]  = (void *)fl_render_base;
    vt[10] = (void *)fl_bren;
    vt[11] = (void *)fl_aren;
    vt[12] = (void *)fl_pdes;
    vt[13] = (void *)fl_trap13;
    vt[14] = (void *)fl_trap14;
    vt[15] = (void *)fl_heap;
    vt[16] = (void *)hal_cppd1_BlueFlame;
    vt[17] = (void *)fl_d0;
    vt[18] = (void *)fl_yoshi;
    vt[19] = (void *)fl_egg;
    vt[20] = (void *)fl_v50;
    vt[21] = (void *)fl_pounded;
    vt[22] = (void *)fl_atk1;
    vt[23] = (void *)fl_atk2;
    vt[24] = (void *)fl_kicked;
    vt[25] = (void *)fl_pushed;
    vt[26] = (void *)fl_cannon;
    vt[27] = (void *)fl_mega;
    vt[28] = (void *)fl_under;
    vt[29] = (void *)fl_aimed;
    vt[30] = (void *)port_actor_s30_base;
}
