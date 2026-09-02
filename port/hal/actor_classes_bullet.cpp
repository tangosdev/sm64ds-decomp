// ============================================================================
// BULLET (actor 233, ov002)
// ============================================================================
//
// _ZTV6Bullet / _ZTV24daPropeller_Heyho_Fire_c, ov002 0x0210d654 -- ONE address,
// both names (the dsd emits the class name and the RTTI name on the same word;
// D1 stores _ZTV6Bullet, D0 stores _ZTV24daPropeller_Heyho_Fire_c). The
// projectile Snufit (func_ov065_02116364) fires: it flies straight, counts down
// a lifetime, pops on ground/wall contact or when it reaches the player. A plain
// Enemy subclass on the 31-slot table, overriding the seven lifecycle slots
// (0/3/6/9/12/16/17) and NOTHING in the interaction tail. Slots 13/14/30 are the
// shared Enemy/Actor base defaults the reloc run already lands on (main module,
// not an ov002 body), so ac31_fill_shared's traps carry them exactly as they do
// for every other plain Enemy -- inferred_stub_guard stays green.
//
// Its SpawnInfo is at ov002 0x0210d630 (pinned in port/ov002_syms.txt) and its
// own +4 halfword reads 0xe9 (233), the registry's cross-check. It is not
// statically placed: Snufit spawns it. Reachability proof: SM64DS_SPAWN_ACTOR=233
// in any ov002-loaded level (ov002 is the engine overlay, always mounted).
//
// Object layout, from Bullet_Spawn (ov002 0x020fefcc, 860 bytes):
// MovingCylinderClsn at 0x110, WithMeshClsn at 0x144, Model at 0x300.
//
// BULLET IS A ONE-PMF-STATE-MACHINE CLASS. func_ov002_020fed2c (the state setter)
// and Bullet::Behavior (the per-frame tick) both form the pointer-to-member, so
// both are host copies in port/unmatched/Bullet_StateDispatch.cpp, which also
// seats the two state-handler records over the mounted DS addresses AFTER
// __sinit_ov002_02108094 fills them (the PushBlock ov002 order). Both handlers
// are matched src -- no hole, no stub. Render dispatches its Model at +0x300
// through a slot-5 local shadow; model_host.cpp seats the Model vtable MSVC-slot-
// ordered, so Render stays a faced .cpp method here (the FlameChomp treatment),
// not a host copy.
#include <cstdio>

extern "C" {
int _ZN6Bullet13InitResourcesEv(void *self);      /* face below            */
int _ZN6Bullet8BehaviorEv(void *self);            /* host copy (StateDispatch) */
int _ZN6Bullet6RenderEv(void *self);              /* face below            */
int _ZN6Bullet16CleanupResourcesEv(void);         /* C-named in its own .c TU */
void _ZN6Bullet16OnPendingDestroyEv(void);        /* empty body, its own .c TU */
int *_ZN6BulletD1Ev(int *self);
int *_ZN6BulletD0Ev(int *self);
void *_ZTV6Bullet[31];
void port_bullet_states_seat(void);               /* port/unmatched        */
void port_enemy_death_states_seat(void);          /* Enemy tier death table */
void port_actor_render_probe(const char *cls, void *model);
}
/* C++ linkage: the shared Enemy 31-slot fill in actor_classes_bob_enemy.cpp is a
   plain C++ function, so declare it the same way here. */
void ac31_fill_shared(void **vt);

/* The projectile's own D0 spells its table by the RTTI name; both names are the
   same ROM word, so this aliases the one nominal to the host array. */
#pragma comment(linker, "/alternatename:__ZTV24daPropeller_Heyho_Fire_c=__ZTV6Bullet")
/* Bullet's InitResources / CleanupResources reach the shared model-file handle
   under two spellings of the one ov065 bss word 0x0211d610 (Snufit owns the
   file it fires from); bridge the bare name onto the mounted ov065 symbol. */
#pragma comment(linker, "/alternatename:_func_0211d610=_data_ov065_0211d610")
/* Bullet::InitResources (matched src, C++ TU) references its model-load and
   collision-init helpers through local non-extern-C decls, so MSVC mangles them;
   the port defines the C-linkage bodies. Bridge each mangled ref onto the C body
   (the ov065 / cxx_aliases per-signature pattern). */
#pragma comment(linker, "/alternatename:?_ZN5Model8LoadFileER13SharedFilePtr@@YAPAUBMD_File@@PAX@Z=__ZN5Model8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?_ZN9ModelBase7SetFileEP8BMD_Fileii@@YAHPADPAUBMD_File@@HH@Z=__ZN9ModelBase7SetFileEP8BMD_Fileii")
#pragma comment(linker, "/alternatename:?_ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj@@YAXPADPAUActor@@HHII@Z=__ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj")
#pragma comment(linker, "/alternatename:?_ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_@@YAXPADPAUActor@@HHPAUVector3_16@@H@Z=__ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_")

static int __fastcall bl_init(void *s, void *)
{ return _ZN6Bullet13InitResourcesEv(s); }
static int __fastcall bl_clean(void *, void *)
{ return _ZN6Bullet16CleanupResourcesEv(); }
static int __fastcall bl_behavior(void *s, void *)
{ return _ZN6Bullet8BehaviorEv(s); }
static int __fastcall bl_render(void *s, void *)
{ port_actor_render_probe("BULLET", (char *)s + 0x300);
  return _ZN6Bullet6RenderEv(s); }
static int __fastcall bl_pdes(void *s, void *)
{ (void)s; _ZN6Bullet16OnPendingDestroyEv(); return 0; }
static int __fastcall bl_d1(void *s, void *)
{ return (int)(size_t)_ZN6BulletD1Ev((int *)s); }
static int __fastcall bl_d0(void *s, void *)
{ return (int)(size_t)_ZN6BulletD0Ev((int *)s); }

extern "C" void hal_fill_bullet_vtable(void)
{
    void **vt = (void **)_ZTV6Bullet;
    port_bullet_states_seat();      /* seat the two dispatch cells BEFORE spawn */
    port_enemy_death_states_seat();
    ac31_fill_shared(vt);
    vt[0]  = (void *)bl_init;
    vt[3]  = (void *)bl_clean;
    vt[6]  = (void *)bl_behavior;
    vt[9]  = (void *)bl_render;
    vt[12] = (void *)bl_pdes;
    vt[16] = (void *)bl_d1;
    vt[17] = (void *)bl_d0;
    /* slots 13/14/18/19/29/30 stay the shared defaults ac31_fill_shared seats:
       the reloc run lands them on the Enemy/Actor base bodies, none an ov002
       guess body. */
}

// ---- Bullet method faces ---------------------------------------------------
// InitResources and Render are .cpp methods against include/Bullet.h; the vtable
// thunks reference the Itanium C name, so face each onto the MSVC method.
// CleanupResources, OnPendingDestroy, D0, D1 are .c C-linkage in their own TUs;
// Behavior is the host copy. Render is faced, not host-copied (see the note).
#include "Bullet.h"
extern "C" {
int _ZN6Bullet13InitResourcesEv(void *self)
{ return ((Bullet *)self)->Bullet::InitResources(); }
int _ZN6Bullet6RenderEv(void *self)
{ return ((Bullet *)self)->Bullet::Render(); }
}
