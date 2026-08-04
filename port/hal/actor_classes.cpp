// The vtables of the actor classes the registry carries, filled at runtime.
//
// THE VTABLE LAW, as gate 9 wrote it (hal/actor_vtables.cpp) and gate 13
// repeated for the Camera (hal/camera_bridges.cpp):
//
//   * MSVC slot order, which for these classes is the ROM's -- include/
//     ActorBase.h declares the destructor LAST on purpose, so slots 0..15
//     agree and only the tail can diverge. Every slot below was read out of
//     the class's own _ZTV in the overlay image with its relocations applied,
//     not inferred from the header.
//   * Every entry is a __fastcall thunk: ecx carries `this` exactly as
//     __thiscall does and the dummy edx absorbs fastcall's second register.
//     That is the convention the four Process wrappers in port/unmatched/
//     call through.
//   * Every thunk calls QUALIFIED, never virtual.
//   * Slots the port cannot service TRAP BY NAME rather than being left null
//     or pointing into the overlay image.
//
// Filled at runtime rather than written as a static array because the ROM
// factories install the vtable themselves (`p[0] = (int)_ZTV4Tree`), so the
// symbol has to be an array of the right size in the first place -- and
// because InitResources can dispatch through the vptr the constructor just
// installed, which means the table must be up before the first spawn.
//
// ---- the shared half -------------------------------------------------------
//
// Ten of the twenty slots are the same functions in every one of these
// classes: Actor's four Before/After pairs, ActorBase::OnHeapCreated, and
// Actor::OnYoshiTryEat. They are written once here.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);      /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2 */
int _ZN5Actor14BeforeBehaviorEv(void *self);           /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);             /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);            /* slot 18 */
}

static int __fastcall ac_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ac_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ac_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
/* Slots 5, 8 and 11 are ARM tail-call veneers on the ROM -- two instructions
   that drop into ActorBase's implementation with the argument still riding in
   r1. A host forward through the veneer's own C face would lose it, so the
   thunk calls the target directly, the same reading the Player's AfterBehavior
   slot already takes. */
static void __fastcall ac_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ac_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ac_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
/* SM64DS_ACTOR_PROBE=2: why an actor did or did not reach its own Render.
   Actor::BeforeRender is the gate -- it refuses on WRONG_AREA (0x20) and on
   OFF_SCREEN (0x8) when the actor asked to be culled off screen (0x2) -- and
   both bits are set by Actor::BeforeBehavior out of the SpawnInfo's own
   radius and cull distance. Printed once per actor id. */
static int __fastcall ac_bren(void *s, void *)
{
    int r = _ZN5Actor12BeforeRenderEv(s);
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_ACTOR_PROBE");
        on = e && e[0] == '2';
    }
    if (on) {
        static unsigned said[64];
        unsigned id = *(unsigned short *)((char *)s + 0xc);
        int seen = 0;
        for (int i = 0; i < 64 && said[i]; ++i)
            if (said[i] == id) seen = 1;
        if (!seen) {
            for (int i = 0; i < 64; ++i)
                if (!said[i]) { said[i] = id; break; }
            const char *c = (const char *)s;
            std::printf("[bren] id %3u -> %d  flags %08x area %d radius %d "
                        "cull %d view (%d,%d,%d)\n", id, r,
                        *(const unsigned *)(c + 0xb0),
                        *(const signed char *)(c + 0xcc),
                        *(const int *)(c + 0xb8) >> 12,
                        *(const int *)(c + 0xbc) >> 12,
                        *(const int *)(c + 0x74) >> 12,
                        *(const int *)(c + 0x78) >> 12,
                        *(const int *)(c + 0x7c) >> 12);
        }
    }
    return r;
}
static void __fastcall ac_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ac_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ac_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static void __fastcall ac_pdes_base(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); }

/* The trap. Slots 13/14 are the actor's own solid-heap creation, 19 is
   OnTurnIntoEgg, and 16/17 are the destructors where a class leaves them
   unhosted; a named abort is the honest placeholder for a slot the port has
   not proved. One report serves every trapped slot, and it says WHICH slot
   fired on WHICH actor in WHICH phase -- the old per-class form printed
   "slot 13" from whatever slot it sat in, and a woken Bird's destructor
   spent a session disguised as a slot-13 dispatch. */
extern "C" {
extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
}

static void ac_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    if (slot >= 0)
        std::fprintf(stderr,
                     "FATAL: vtable slot %d is not hosted (actor id %u %s, "
                     "phase %d, spawn step %d)\n",
                     slot, id, port_actor_class_name(id), data_02099f24[0],
                     (int)data_020a4b4c);
    else
        std::fprintf(stderr,
                     "FATAL: a dBgActor_c base-table slot dispatched "
                     "(actor id %u %s, phase %d, spawn step %d)\n",
                     id, port_actor_class_name(id), data_02099f24[0],
                     (int)data_020a4b4c);
    std::abort();
}
static int __fastcall ac_trap13(void *s, void *) { ac_trap_report(s, 13); return 0; }
static int __fastcall ac_trap14(void *s, void *) { ac_trap_report(s, 14); return 0; }
static int __fastcall ac_trap16(void *s, void *) { ac_trap_report(s, 16); return 0; }
static int __fastcall ac_trap17(void *s, void *) { ac_trap_report(s, 17); return 0; }
static int __fastcall ac_trap19(void *s, void *) { ac_trap_report(s, 19); return 0; }
static int __fastcall plat_trap(void *s, void *) { ac_trap_report(s, -1); return 0; }

/* ---- the pieces the hosted D1 destructors tear members down with -----------
   The ROM's destroy path is ActorBase::AfterCleanupResources: unlink both
   nodes, drop the dedicated heap, then `this->Destructor()` -- a VIRTUAL
   dispatch of slot 16 (D1, the complete-object form) -- and only after that
   returns does it Memory::Deallocate the object itself. So a hosted slot 16
   is the class's own D0 body minus the final Deallocate: install the class's
   own vtable, member D1s in reverse declaration order, Actor's D2. The walk
   proved the path live: a woken Bird's flock climbs past y 3000 and
   func_ov009_02111234 marks every one of them. */
extern "C" {
void _ZN11ShadowModelD1Ev(void *);
void _ZN9ModelAnimD1Ev(void *);
void _ZN5ModelD1Ev(void *);
void _ZN12WithMeshClsnD1Ev(void *);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *);
void *_ZN5ActorD2Ev(void *);
}

/* Fill the ten shared slots plus the three traps; the caller writes its own
   six. Keeps each class's fill down to the lines that are actually its own. */
static void ac_fill_shared(void **vt)
{
    vt[1] = (void *)ac_binit;
    vt[2] = (void *)ac_ainit;
    vt[4] = (void *)ac_bclean;
    vt[5] = (void *)ac_aclean;
    vt[7] = (void *)ac_bbeh;
    vt[8] = (void *)ac_abeh;
    vt[10] = (void *)ac_bren;
    vt[11] = (void *)ac_aren;
    vt[13] = (void *)ac_trap13;
    vt[14] = (void *)ac_trap14;
    vt[15] = (void *)ac_heap;
    vt[18] = (void *)ac_yoshi;
    vt[19] = (void *)ac_trap19;
}

// ---- TREE (actor 286, ov002) -----------------------------------------------
//
// _ZTV4Tree, ov002 0x0210ac00. Five of the six own slots are plain C-named
// free functions in their TUs (the mwcc form) and two are real MSVC methods
// against include/Tree.h; the thunks bridge both faces.
//
// Behavior takes no argument at all. That is the ROM's shape, not a
// transcription slip: Tree::Behavior ticks the five GLOBAL CylinderClsn lists
// at data_ov002_02110a48 rather than anything on `this`, so mwcc's r0 went
// unread. Twenty-one Tree actors therefore all run the same body and the
// cylinders are ticked twenty-one times over -- also the ROM's behaviour.
#include "Tree.h"
extern "C" {
int _ZN4Tree13InitResourcesEv(char *self);
int _ZN4Tree8BehaviorEv(void);
void _ZN4Tree16OnPendingDestroyEv(void);
int _ZN4TreeD1Ev(char *self);
void *_ZN4TreeD0Ev(char *self);
void *_ZTV4Tree[20];
extern int data_ov002_02110a48[5];   /* the five variant cylinder lists */
}

static int __fastcall tree_init(void *s, void *)
{ return _ZN4Tree13InitResourcesEv((char *)s); }
static int __fastcall tree_clean(void *s, void *)
{ return ((Tree *)s)->Tree::CleanupResources(); }
static int __fastcall tree_behavior(void *, void *)
{ return _ZN4Tree8BehaviorEv(); }
/* SM64DS_TREE_PROBE=1: the five variant lists as InitResources built them --
   one CylinderClsnWithPos per Tree actor, its position stored in SCENE units
   (Vec3_AsrInPlace by 3) with the trunk's 30-unit lift already added, which is
   what Render clips and draws at. Printed back in world units so it can be
   read against the level's own object table. */
static void tree_probe(void)
{
    static int done;
    if (done || !std::getenv("SM64DS_TREE_PROBE"))
        return;
    done = 1;
    for (int i = 0; i < 5; ++i) {
        int n = 0;
        std::printf("[tree] variant %d:", i);
        for (int *p = (int *)(size_t)data_ov002_02110a48[i]; p && n < 64;
             p = (int *)(size_t)p[0x12], ++n)
            std::printf(" (%d,%d,%d)", p[0] * 8 / 4096,
                        (p[1] - 0x1e000) * 8 / 4096, p[2] * 8 / 4096);
        std::printf("  [%d]\n", n);
    }
}

/* SM64DS_TREE_PROBE=2 additionally draws variant 4's Model through the
   HARNESS path (hal_render_model, the one the castle uses) at the origin, so
   the collapsed-geometry question is answered on one BMD by two renderers
   rather than by argument. */
extern "C" void hal_render_model(void *model, int scaleShift);
static void tree_probe_harness(void *self)
{
    const char *e = std::getenv("SM64DS_TREE_PROBE");
    if (!e || e[0] != '2')
        return;
    char *mdl = (char *)self + 0xd4 + 4 * 0x50;
    const unsigned char *bf = *(const unsigned char *const *)(mdl + 0x0c);
    hal_render_model(mdl, bf ? (int)bf[0] : 0);
}

static int __fastcall tree_render(void *s, void *)
{ tree_probe(); tree_probe_harness(s); return ((Tree *)s)->Tree::Render(); }
static int __fastcall tree_pdes(void *, void *)
{ _ZN4Tree16OnPendingDestroyEv(); return 0; }
static int __fastcall tree_d1(void *s, void *)
{ return _ZN4TreeD1Ev((char *)s); }
static int __fastcall tree_d0(void *s, void *)
{ return (int)(size_t)_ZN4TreeD0Ev((char *)s); }

extern "C" void hal_fill_cylinder_withpos_vtable(void);

extern "C" void hal_fill_tree_vtable(void)
{
    void **vt = _ZTV4Tree;
    hal_fill_cylinder_withpos_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)tree_init;
    vt[3] = (void *)tree_clean;
    vt[6] = (void *)tree_behavior;
    vt[9] = (void *)tree_render;
    vt[12] = (void *)tree_pdes;
    vt[16] = (void *)tree_d1;
    vt[17] = (void *)tree_d0;
}

// ---- AMBIENT_SOUND_EFFECTS (actor 350, ov002) x5 ---------------------------
//
// The spawn table's entry for 350 is AmbientSoundEffects_SpawnInfo, whose
// factory word is 0x020f1b94 -- AmbientSoundEffects_Spawn -- and that
// function's literal pool installs the vtable at 0x0210b4c8. Its methods are
// the block at 0x020f198c..0x020f1ac4.
//
// The reading is confirmed from the other side: the Behavior in that block
// calls Sound::PlayLong with data_ov002_0210b498[param] -- an ambient loop
// table -- and skips it in the three underwater camera modes. That is
// AMBIENT SOUND EFFECTS.
//
// Config used to carry these names one class late (mwcc emits a class as
// [methods..., Spawn], so the block BEFORE a factory belongs to it), which is
// why this row was first written against _ZN14EnemySwitchTag*. #1048 shifted
// the ov002 sound cluster onto its real blocks and the names now agree.
//
// It renders nothing (Render is `return 1`) and its five instances are silent
// while the Sound:: layer is stubbed, so what this row proves is the registry
// generalising to a second class: five actors spawn, initialise, land on both
// processing lists and tick every frame.
#include "AmbientSoundEffects.h"
extern "C" {
int _ZN19AmbientSoundEffects6RenderEv(void);
int _ZN19AmbientSoundEffects16CleanupResourcesEv(void);
void _ZN19AmbientSoundEffects16OnPendingDestroyEv(void);
void *_ZTV19AmbientSoundEffects[20];
}

static int __fastcall amb_init(void *s, void *)
{ return ((AmbientSoundEffects *)s)->AmbientSoundEffects::InitResources(); }
static int __fastcall amb_clean(void *, void *)
{ return _ZN19AmbientSoundEffects16CleanupResourcesEv(); }
static int __fastcall amb_behavior(void *s, void *)
{ return ((AmbientSoundEffects *)s)->AmbientSoundEffects::Behavior(); }
static int __fastcall amb_render(void *, void *)
{ return _ZN19AmbientSoundEffects6RenderEv(); }
static int __fastcall amb_pdes(void *, void *)
{ _ZN19AmbientSoundEffects16OnPendingDestroyEv(); return 0; }

extern "C" void hal_fill_ambient_sound_vtable(void)
{
    void **vt = _ZTV19AmbientSoundEffects;
    ac_fill_shared(vt);
    vt[0] = (void *)amb_init;
    vt[3] = (void *)amb_clean;
    vt[6] = (void *)amb_behavior;
    vt[9] = (void *)amb_render;
    vt[12] = (void *)amb_pdes;
    /* 16/17 keep the trap: nothing on the castle grounds destroys one of
       these, and their two TUs need a shape the port has no reason to build
       (a real ~Actor and the VT/HEAP placeholders). */
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

/* Silence the unused-static warning for the two shared helpers a class with
   its own OnPendingDestroy does not take. */
extern "C" void *hal_actor_shared_pdes(void) { return (void *)ac_pdes_base; }

// ---- Platform, the base three of these classes destruct through ------------
//
// Every Platform-derived destructor is the mwcc two-step: install the class's
// own vtable, run the members that belong to the derived part, install the
// BASE vtable, run the rest. The base one is ov002 0x0210ae38, which the
// destructors spell _ZTV10dBgActor_c (Platform is dBgActor_c in the ROM's own
// RTTI) and which config now calls _ZTV8Platform -- it read
// _ZTV17ExclamationSwitch until #1042, a name attached one class late, the
// same skew AMBIENT_SOUND_EFFECTS ran into. The ROM settles it: slots 16/17 of that
// table are _ZN8PlatformD2Ev and _ZN8PlatformD0Ev, and slots 0/3/6/9 are
// ActorBase's own do-nothing bodies, which is exactly a base-class table.
//
// It is only ever installed BETWEEN two member teardowns and nothing dispatches
// through it while it is there, so the port gives it the shared half and traps
// the rest: if a future class does dispatch, it says which slot.
extern "C" { void *_ZTV10dBgActor_c[20]; }
extern "C" void hal_fill_platform_vtable(void)
{
    static int done;
    if (done) return;
    done = 1;
    for (int i = 0; i < 20; ++i)
        _ZTV10dBgActor_c[i] = (void *)plat_trap;
    ac_fill_shared(_ZTV10dBgActor_c);
    _ZTV10dBgActor_c[12] = (void *)ac_pdes_base;
}

// ---- BLACK_BRICK_BLOCK (actor 17, ov002) x1 --------------------------------
//
// _ZTV13BigBrickBlock, ov002 0x02108adc. One class body serves six actor ids
// and switches on its own; 17 is 0x11, the variant that tracks a star and
// carries the 1.0-scaled KCL. The castle grounds' single instance is at
// (-3300, -650, 20), under the bridge.
//
// Four of the six own slots are real MSVC methods against include/
// BigBrickBlock.h; InitResources and the two destructors are C-form TUs.
#include "BigBrickBlock.h"
extern "C" {
int _ZN13BigBrickBlock13InitResourcesEv(void *self);
int *_ZN13BigBrickBlockD1Ev(int *self);
int *_ZN13BigBrickBlockD0Ev(int *self);
void *_ZTV13BigBrickBlock[20];
}
/* The destructors spell the class's own table by its RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV13daObjBlockL_c=__ZTV13BigBrickBlock")

static int __fastcall bbb_init(void *s, void *)
{ return _ZN13BigBrickBlock13InitResourcesEv(s); }
static int __fastcall bbb_clean(void *s, void *)
{ return ((BigBrickBlock *)s)->BigBrickBlock::CleanupResources(); }
static int __fastcall bbb_behavior(void *s, void *)
{ return ((BigBrickBlock *)s)->BigBrickBlock::Behavior(); }
extern "C" void port_actor_render_probe(const char *cls, void *model);
static int __fastcall bbb_render(void *s, void *)
{
    port_actor_render_probe("BLACK_BRICK_BLOCK", (char *)s + 0xd4);
    return ((BigBrickBlock *)s)->BigBrickBlock::Render();
}
static int __fastcall bbb_d1(void *s, void *)
{ return (int)(size_t)_ZN13BigBrickBlockD1Ev((int *)s); }
static int __fastcall bbb_d0(void *s, void *)
{ return (int)(size_t)_ZN13BigBrickBlockD0Ev((int *)s); }

extern "C" void hal_fill_black_brick_block_vtable(void)
{
    void **vt = _ZTV13BigBrickBlock;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)bbb_init;
    vt[3] = (void *)bbb_clean;
    vt[6] = (void *)bbb_behavior;
    vt[9] = (void *)bbb_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)bbb_d1;
    vt[17] = (void *)bbb_d0;
}

// ---- SIGN_POST (actor 184, ov002) x5 ---------------------------------------
//
// _ZTV8SignPost, ov002 0x02109af8 (RTTI: daObjTatefuda_c -- tatefuda is a
// signboard). Five of them around the grounds. This is the first class whose
// InitResources RAYCASTS THE LEVEL: it drops a ground ray from 100 units above
// the object record's Y and plants the post on whatever it finds, so the five
// signs read back the same floor the Player stands on.
//
// It also carries three collision objects at once -- a MovingMeshCollider for
// the solid post, a MovingCylinderClsn for the read trigger, and a WithMeshClsn
// of its own -- which is the shape every Platform actor after it has.
//
// THE TALK PATH IS GUARDED, and this is the one place a slot is not the ROM's.
// Behavior's read branch runs when the Player is in range with its 0xc8 word
// set, and func_ov002_020bb060 ends in the Message system (Message::Show and
// the dialogue box's OAM/font machinery), none of which the port hosts. The
// guard is in hal/actor_vtables.cpp at that function, not here: the sign
// still turns to face the player and still blocks him, it just does not open
// a box that has nothing to draw with.
#include "SignPost.h"
extern "C" {
int _ZN8SignPost8BehaviorEv(char *self);
int *_ZN8SignPostD1Ev(int *self);
int *_ZN8SignPostD0Ev(int *self);
void *_ZTV8SignPost[20];
}
#pragma comment(linker, "/alternatename:__ZTV15daObjTatefuda_c=__ZTV8SignPost")

static int __fastcall sp_init(void *s, void *)
{ return ((SignPost *)s)->SignPost::InitResources(); }
static int __fastcall sp_clean(void *s, void *)
{ return ((SignPost *)s)->SignPost::CleanupResources(); }
static int __fastcall sp_behavior(void *s, void *)
{ return _ZN8SignPost8BehaviorEv((char *)s); }
/* SM64DS_ACTOR_PROBE=1: one line per class the first time its Render runs,
   with what the Model has to draw with -- the BMD the file pointer resolved
   to and the model matrix's own translation row, which Platform writes in
   SCENE units. A class that never prints is being culled before Render;
   one that prints with a null file has a load problem instead. */
extern "C" void port_actor_render_probe(const char *cls, void *model)
{
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_ACTOR_PROBE") != 0;
    if (!on) return;
    /* 32, not 8: the registry carried five classes when this was written and
       carries twenty now, so the old cap stopped reporting before it reached
       the ones landed last -- silently, which is the worst way for a probe to
       go quiet. The transforms pointer is new for the same reason: it is what
       ModelBase::SetFile allocates and what a Model's own Virtual10 copies
       out of, so a null there is a load that did not finish. */
    static const char *said[32];
    static int n;
    for (int i = 0; i < n; ++i)
        if (said[i] == cls) return;
    if (n < 32) said[n++] = cls;
    const int *m = (const int *)((const char *)model + 0x1c);
    std::printf("[actor] %-17s model %p file %p transforms %p "
                "mat.t (%d,%d,%d) scene\n",
                cls, model, *(void *const *)((const char *)model + 0x0c),
                *(void *const *)((const char *)model + 0x14),
                m[9] >> 12, m[10] >> 12, m[11] >> 12);
}

static int __fastcall sp_render(void *s, void *)
{
    port_actor_render_probe("SIGN_POST", (char *)s + 0xd4);
    return ((SignPost *)s)->SignPost::Render();
}
static int __fastcall sp_d1(void *s, void *)
{ return (int)(size_t)_ZN8SignPostD1Ev((int *)s); }
static int __fastcall sp_d0(void *s, void *)
{ return (int)(size_t)_ZN8SignPostD0Ev((int *)s); }

extern "C" void port_sign_post_states_seat(void);   /* port/unmatched */

extern "C" void hal_fill_sign_post_vtable(void)
{
    void **vt = _ZTV8SignPost;
    /* the sinit copied the ROM's own five {Init, Main} pairs into
       data_ov002_0210e084, which means DS code addresses; seat the host
       bodies over them (the ovdata contract, see the file header there) */
    port_sign_post_states_seat();
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)sp_init;
    vt[3] = (void *)sp_clean;
    vt[6] = (void *)sp_behavior;
    vt[9] = (void *)sp_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)sp_d1;
    vt[17] = (void *)sp_d0;
}

// ---- ONE_UP_MUSHROOM (actor 276, ov002) x3 ---------------------------------
//
// _ZTV13OneUpMushroom, ov002 0x021083c8 (RTTI: da1up_c). Three of them on the
// castle roof at param 0x00f3, which is mushroom type 3.
//
// Two slots are this class's own rather than Actor's, and they are the reason
// the class exists: slot 18 (OnYoshiTryEat) returns 4 -- "swallow me" -- and
// slot 19 (OnTurnIntoEgg) is the collect path, GiveLives plus the 1UP logo
// actor. Both are hosted; nothing on this level reaches them while the
// character is Mario.
//
// Behavior is the HOST COPY in port/unmatched/OneUpMushroom_Behavior.cpp: the
// ROM's dispatches the mushroom type through an mwcc pointer-to-member table
// and MSVC has no representation for one. See that file's header.
#include "OneUpMushroom.h"
extern "C" {
int _ZN13OneUpMushroom8BehaviorEv(void *self);       /* port/unmatched */
void _ZN13OneUpMushroom16OnPendingDestroyEv(void);
int *_ZN13OneUpMushroomD1Ev(int *self);
int *_ZN13OneUpMushroomD0Ev(int *self);
int func_ov002_020af3a0(void);                       /* OnYoshiTryEat */
void func_ov002_020af2b0(char *self, int arg);       /* OnTurnIntoEgg */
void *_ZTV13OneUpMushroom[20];
}
#pragma comment(linker, "/alternatename:__ZTV7da1up_c=__ZTV13OneUpMushroom")

static int __fastcall oum_init(void *s, void *)
{ return ((OneUpMushroom *)s)->OneUpMushroom::InitResources(); }
static int __fastcall oum_clean(void *s, void *)
{ return ((OneUpMushroom *)s)->OneUpMushroom::CleanupResources(); }
static int __fastcall oum_behavior(void *s, void *)
{ return _ZN13OneUpMushroom8BehaviorEv(s); }
static int __fastcall oum_render(void *s, void *)
{
    port_actor_render_probe("ONE_UP_MUSHROOM", (char *)s + 0x300);
    return (int)((OneUpMushroom *)s)->OneUpMushroom::Render();
}
static int __fastcall oum_pdes(void *, void *)
{ _ZN13OneUpMushroom16OnPendingDestroyEv(); return 0; }
static int __fastcall oum_d1(void *s, void *)
{ return (int)(size_t)_ZN13OneUpMushroomD1Ev((int *)s); }
static int __fastcall oum_d0(void *s, void *)
{ return (int)(size_t)_ZN13OneUpMushroomD0Ev((int *)s); }
static int __fastcall oum_yoshi(void *, void *)
{ return func_ov002_020af3a0(); }
static int __fastcall oum_egg(void *s, void *, int a)
{ func_ov002_020af2b0((char *)s, a); return 0; }

extern "C" void port_one_up_mushroom_types_seat(void);   /* port/unmatched */

extern "C" void hal_fill_one_up_mushroom_vtable(void)
{
    void **vt = _ZTV13OneUpMushroom;
    /* same as the sign: __sinit_ov002_02100adc left fourteen DS code
       addresses in data_ov002_0210dc00 */
    port_one_up_mushroom_types_seat();
    ac_fill_shared(vt);
    vt[0] = (void *)oum_init;
    vt[3] = (void *)oum_clean;
    vt[6] = (void *)oum_behavior;
    vt[9] = (void *)oum_render;
    vt[12] = (void *)oum_pdes;
    vt[16] = (void *)oum_d1;
    vt[17] = (void *)oum_d0;
    vt[18] = (void *)oum_yoshi;
    vt[19] = (void *)oum_egg;
}

// ---- CylinderClsnWithPos ---------------------------------------------------
//
// Not an actor, but the Tree's InitResources constructs one per instance and
// the constructor installs _ZTV19CylinderClsnWithPos by name. Four slots
// (arm9 0x0208e6bc: D1, D0, GetPos, GetOwnerID) and the last two really do
// dispatch -- every cylinder the Tree threads onto data_0209cee8 shares that
// list with the Player's own, and the cylinder pass asks each node where it
// is and who owns it. A zeroed table would be a call through null the first
// time Mario walked near a trunk.
extern "C" {
void *_ZTV19CylinderClsnWithPos[4];
void *_ZN19CylinderClsnWithPosD1Ev(void *self);
void *_ZN19CylinderClsnWithPosD0Ev(void *self);
void *_ZN19CylinderClsnWithPos6GetPosEv(void *self);
}
#include "CylinderClsnWithPos.h"

static void *__fastcall ccp_d1(void *s, void *)
{ return _ZN19CylinderClsnWithPosD1Ev(s); }
static void *__fastcall ccp_d0(void *s, void *)
{ return _ZN19CylinderClsnWithPosD0Ev(s); }
static void *__fastcall ccp_getpos(void *s, void *)
{ return _ZN19CylinderClsnWithPos6GetPosEv(s); }
static unsigned __fastcall ccp_ownerid(void *s, void *)
{ return ((CylinderClsnWithPos *)s)->CylinderClsnWithPos::GetOwnerID(); }

extern "C" void hal_fill_cylinder_withpos_vtable(void)
{
    _ZTV19CylinderClsnWithPos[0] = (void *)ccp_d1;
    _ZTV19CylinderClsnWithPos[1] = (void *)ccp_d0;
    _ZTV19CylinderClsnWithPos[2] = (void *)ccp_getpos;
    _ZTV19CylinderClsnWithPos[3] = (void *)ccp_ownerid;
}

// ============================================================================
// GATE 17: the LEVEL OVERLAY'S OWN CLASSES
// ============================================================================
//
// Four classes that exist only on the castle grounds. mwcc emits a class as
// [methods..., Spawn], so the block BEFORE a factory belongs to it, and each
// factory's own literal pool names the vtable it installs:
//
//   Bird_Spawn        -> _ZTV4Bird             = ov009 0x02113958
//   CastleWater_Spawn -> _ZTV14daObjMcWater_c  = ov009 0x02113a18
//   MetalNet_Spawn    -> _ZTV8MetalNet         = ov009 0x02113ae0
//   Flag_Spawn        -> _ZTV4Flag             = ov009 0x02113ba0
//
// The ROM's own RTTI agrees from the other side: 0x02113ae0 is
// daObjMc_Metalnet_c and 0x02113ba0 is daMcFlag_c, and config now carries both
// spellings for each table. It used to attach these names one class late from
// Bird onward, which is why this gate was first written against func_ov009_*
// for the water and _ZN11CastleWater*/_ZN8DockPole* for the net and the flag;
// #1041 shifted them onto the blocks they belong to.
//
// SLOTS 16/17 TRAP FOR ALL FOUR. Nothing on the castle grounds destroys one
// -- every InitResources here returns 1 on this level -- and the water's own
// D0 (src/_ZN11CastleWaterD0Ev.c) is why it matters: it spells its two vtables VT0
// and VT1, the same placeholder names src/_ZN10SphereClsnD1Ev.c uses for
// three completely different tables, so one host definition would satisfy
// both with the wrong bytes and nothing would say so.

// ---- BIRD (actor 343, ov009) x2 --------------------------------------------
extern "C" {
int _ZN4Bird13InitResourcesEv(void *self);
int _ZN4Bird8BehaviorEv(void *self);            /* host copy: hal/ov009_boot */
int _ZN4Bird6RenderEv(void *self);
int _ZN4Bird16CleanupResourcesEv(void);
void _ZN4Bird16OnPendingDestroyEv(void);
void *_ZTV4Bird[20];
}
/* The Bird's own D0 spells its table by the RTTI name. */
#pragma comment(linker, "/alternatename:__ZTV9daSBird_c=__ZTV4Bird")

static int __fastcall bird_init(void *s, void *)
{ return _ZN4Bird13InitResourcesEv(s); }
static int __fastcall bird_clean(void *, void *)
{ return _ZN4Bird16CleanupResourcesEv(); }
static int __fastcall bird_behavior(void *s, void *)
{ return _ZN4Bird8BehaviorEv(s); }
static int __fastcall bird_render(void *s, void *)
{ port_actor_render_probe("BIRD", (char *)s + 0xd4);
  return _ZN4Bird6RenderEv(s); }
static int __fastcall bird_pdes(void *, void *)
{ _ZN4Bird16OnPendingDestroyEv(); return 0; }
/* SLOT 16 IS LIVE: a woken bird's state 0 spawns its flock, state 3 climbs,
   and past y 3000 func_ov009_02111234 marks every member -- the turn walk
   died right here while this slot was a trap. The body is
   src/_ZN4BirdD0Ev.c minus its final Memory::Deallocate, which
   AfterCleanupResources performs itself after the dispatch returns. */
static void *__fastcall bird_d1(void *s, void *)
{
    *(int *)s = (int)(size_t)_ZTV4Bird;
    _ZN11ShadowModelD1Ev((char *)s + 0x138);
    _ZN9ModelAnimD1Ev((char *)s + 0xd4);
    _ZN5ActorD2Ev(s);
    return s;
}

extern "C" void hal_fill_bird_vtable(void)
{
    void **vt = _ZTV4Bird;
    ac_fill_shared(vt);
    vt[0] = (void *)bird_init;
    vt[3] = (void *)bird_clean;
    vt[6] = (void *)bird_behavior;
    vt[9] = (void *)bird_render;
    vt[12] = (void *)bird_pdes;
    vt[16] = (void *)bird_d1;
    /* 17 keeps the trap: the ROM's destroy path is D1 + an explicit
       Deallocate, and nothing on this level calls the deleting form. */
    vt[17] = (void *)ac_trap17;
}

// ---- CASTLE_WATER (actor 338, ov009) x1 ------------------------------------
//
// The moat, one instance at the origin. Its InitResources is the reason this
// class matters to anything but the picture: it loads a KCL of its own, hands
// it to MovingMeshCollider with the actor's matrix and a 1.0 scale, Enables it
// onto the level's collider list, and then CLAMPS THE GLOBAL WATER LEVEL --
// data_0209f32c = min(data_0209f32c, self.y - 100.0) -- which is the word the
// Player's own state machine reads to decide it is swimming.
extern "C" {
int _ZN11CastleWater13InitResourcesEv(void *self);
int _ZN11CastleWater16CleanupResourcesEv(void *self);
int _ZN11CastleWater8BehaviorEv(void *self);
int _ZN11CastleWater6RenderEv(void *self);
void *_ZTV14daObjMcWater_c[20];        /* ov009 0x02113a18 */
}
/* The water's own D0 spells this table by the class name. */
#pragma comment(linker, "/alternatename:__ZTV11CastleWater=__ZTV14daObjMcWater_c")

static int __fastcall cw_init(void *s, void *)
{ return _ZN11CastleWater13InitResourcesEv(s); }
static int __fastcall cw_clean(void *s, void *)
{ return _ZN11CastleWater16CleanupResourcesEv(s); }
static int __fastcall cw_behavior(void *s, void *)
{ return _ZN11CastleWater8BehaviorEv(s); }
static int __fastcall cw_render(void *s, void *)
{ port_actor_render_probe("CASTLE_WATER", (char *)s + 0xd4);
  return _ZN11CastleWater6RenderEv(s); }

extern "C" void hal_fill_castle_water_vtable(void)
{
    void **vt = _ZTV14daObjMcWater_c;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)cw_init;
    vt[3] = (void *)cw_clean;
    vt[6] = (void *)cw_behavior;
    vt[9] = (void *)cw_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

// ---- METAL_NET (actor 339, ov009) x3 ---------------------------------------
//
// The three climbable nets under the bridge. Same shape as the water -- a
// Platform with a MovingMeshCollider -- which is why the two were easy to
// mistake for one class twice over while the names sat one block off.
extern "C" {
int _ZN8MetalNet13InitResourcesEv(void *self);
int _ZN8MetalNet8BehaviorEv(void *self);
int _ZN8MetalNet6RenderEv(void *self);
int _ZN8MetalNet16CleanupResourcesEv(void *self);
void _ZN8MetalNet16OnPendingDestroyEv(void);
void *_ZTV8MetalNet[20];
}
#pragma comment(linker, "/alternatename:__ZTV18daObjMc_Metalnet_c=__ZTV8MetalNet")

static int __fastcall mn_init(void *s, void *)
{ return _ZN8MetalNet13InitResourcesEv(s); }
static int __fastcall mn_clean(void *s, void *)
{ return _ZN8MetalNet16CleanupResourcesEv(s); }
static int __fastcall mn_behavior(void *s, void *)
{ return _ZN8MetalNet8BehaviorEv(s); }
static int __fastcall mn_render(void *s, void *)
{ port_actor_render_probe("METAL_NET", (char *)s + 0xd4);
  return _ZN8MetalNet6RenderEv(s); }
static int __fastcall mn_pdes(void *, void *)
{ _ZN8MetalNet16OnPendingDestroyEv(); return 0; }

extern "C" void hal_fill_metal_net_vtable(void)
{
    void **vt = _ZTV8MetalNet;
    hal_fill_platform_vtable();
    ac_fill_shared(vt);
    vt[0] = (void *)mn_init;
    vt[3] = (void *)mn_clean;
    vt[6] = (void *)mn_behavior;
    vt[9] = (void *)mn_render;
    vt[12] = (void *)mn_pdes;
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

// ---- FLAG (actor 342, ov009) x4 --------------------------------------------
//
// The four castle-tower flags, at y 3095 and 3662. Behavior is one
// Animation::Advance and a rotation matrix: the flag waves, and it is the
// first actor on the grounds whose model is ANIMATED by the game's own
// ModelAnim rather than posed once.
extern "C" {
int _ZN4Flag13InitResourcesEv(void *self);
int _ZN4Flag8BehaviorEv(void *self);
int _ZN4Flag6RenderEv(void *self);
int _ZN4Flag16CleanupResourcesEv(void);
void *_ZTV4Flag[20];
}
#pragma comment(linker, "/alternatename:__ZTV10daMcFlag_c=__ZTV4Flag")

static int __fastcall flag_init(void *s, void *)
{ return _ZN4Flag13InitResourcesEv(s); }
static int __fastcall flag_clean(void *, void *)
{ return _ZN4Flag16CleanupResourcesEv(); }
static int __fastcall flag_behavior(void *s, void *)
{ return _ZN4Flag8BehaviorEv(s); }
static int __fastcall flag_render(void *s, void *)
{ port_actor_render_probe("FLAG", (char *)s + 0xd4);
  return _ZN4Flag6RenderEv(s); }

extern "C" void hal_fill_flag_vtable(void)
{
    void **vt = _ZTV4Flag;
    ac_fill_shared(vt);
    vt[0] = (void *)flag_init;
    vt[3] = (void *)flag_clean;
    vt[6] = (void *)flag_behavior;
    vt[9] = (void *)flag_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

// ============================================================================
// GATE 18: ov085, THE FIRST ACTOR OVERLAY
// ============================================================================
//
// Not the engine overlay and not the level's: ov085 exists for six classes
// that several levels share, and the castle grounds names two of them.
//
// THE CONFIG'S NAMES ARE NOT SHIFTED HERE. That is checked, not assumed --
// ov009 is the reason to check. Every one of ov085's six factories installs a
// vtable whose slot 0 is its own class's InitResources and whose slot 16 is
// its own D1, and the ROM's RTTI names agree from the other side (daMip_c is
// the rabbit, daC_Jugem_c the Lakitu). Each class's own D0 spells the table
// by the RTTI name and its D1 by the config name, which is why both spellings
// have to resolve to ONE array -- the alternatename pairs below.

// ---- RABBIT (actor 187, ov085) x12 -----------------------------------------
//
// _ZTV6Rabbit / _ZTV7daMip_c, ov085 0x021300f8. The twelve glowing rabbits.
// This is the port's first AI actor: InitResources seeds it from the star
// count and binds it to one of the level's own paths, and Behavior takes the
// closest Player every frame and runs the flee/idle machine against it
// through a MovingCylinderClsn and a WithMeshClsn.
//
// THE CATCH PATH IS GUARDED, and the guard is not here. Behavior's caught
// branch calls Message::PrepareTalk and Player::ShowMessage, which end in the
// dialogue box's OAM/font machinery -- the same system the SIGN_POST's read
// branch stops at (hal/actor_vtables.cpp). The rabbit still flees, still
// gets touched and still plays its caught animation; it just does not open a
// box that has nothing to draw with.
extern "C" {
int _ZN6Rabbit13InitResourcesEv(void *self);      /* face: method_faces */
int _ZN6Rabbit8BehaviorEv(void *self);
int _ZN6Rabbit6RenderEv(void *self);              /* face: method_faces */
int _ZN6Rabbit16CleanupResourcesEv(void);
void _ZN6Rabbit16OnPendingDestroyEv(void);
int *_ZN6RabbitD1Ev(int *self);
int *_ZN6RabbitD0Ev(int *self);
int func_ov085_0212cc18(unsigned char *self);     /* slot 18, its own */
void *_ZTV6Rabbit[20];
}
#pragma comment(linker, "/alternatename:__ZTV7daMip_c=__ZTV6Rabbit")

static int __fastcall rb_init(void *s, void *)
{ return _ZN6Rabbit13InitResourcesEv(s); }
static int __fastcall rb_clean(void *, void *)
{ return _ZN6Rabbit16CleanupResourcesEv(); }
static int __fastcall rb_behavior(void *s, void *)
{ return _ZN6Rabbit8BehaviorEv(s); }
static int __fastcall rb_render(void *s, void *)
{ port_actor_render_probe("RABBIT", (char *)s + 0x300);
  return _ZN6Rabbit6RenderEv(s); }
static int __fastcall rb_pdes(void *, void *)
{ _ZN6Rabbit16OnPendingDestroyEv(); return 0; }
static int __fastcall rb_d1(void *s, void *)
{ return (int)(size_t)_ZN6RabbitD1Ev((int *)s); }
static int __fastcall rb_d0(void *s, void *)
{ return (int)(size_t)_ZN6RabbitD0Ev((int *)s); }
/* The rabbit is the first class on this level to OVERRIDE OnYoshiTryEat
   rather than inherit Actor's -- Yoshi can eat one. */
static int __fastcall rb_yoshi(void *s, void *)
{ return func_ov085_0212cc18((unsigned char *)s); }

extern "C" void hal_fill_rabbit_vtable(void)
{
    void **vt = _ZTV6Rabbit;
    ac_fill_shared(vt);
    vt[0] = (void *)rb_init;
    vt[3] = (void *)rb_clean;
    vt[6] = (void *)rb_behavior;
    vt[9] = (void *)rb_render;
    vt[12] = (void *)rb_pdes;
    vt[16] = (void *)rb_d1;
    vt[17] = (void *)rb_d0;
    vt[18] = (void *)rb_yoshi;
}

// ---- LAKITU_BRO (actor 235, ov085) x1 --------------------------------------
//
// _ZTV9LakituBro / _ZTV11daC_Jugem_c, ov085 0x02130344. The cameraman over
// the moat. He is here rather than deferred with the rest of the cutscene
// cast because what he reads is hosted: his own state machine places him
// against the LIVE Camera, the one gate 13 brought up.
//
// His Behavior is a HOST COPY (port/unmatched/LakituBro_Behavior.cpp) for the
// gate-16 reason -- it dispatches an mwcc pointer-to-member out of an eleven
// entry state table, and MSVC forms a PMF over an incomplete class as the
// four-word general representation.
extern "C" {
int _ZN9LakituBro13InitResourcesEv(void *self);   /* face: method_faces */
int _ZN9LakituBro8BehaviorEv(void *self);         /* host copy */
int _ZN9LakituBro6RenderEv(void *self);           /* face: method_faces */
int _ZN9LakituBro16CleanupResourcesEv(void);
void _ZN9LakituBro16OnPendingDestroyEv(void);
int *_ZN9LakituBroD1Ev(int *self);
int *_ZN9LakituBroD0Ev(int *self);
void *_ZTV9LakituBro[20];
}
#pragma comment(linker, "/alternatename:__ZTV11daC_Jugem_c=__ZTV9LakituBro")

static int __fastcall lb_init(void *s, void *)
{ return _ZN9LakituBro13InitResourcesEv(s); }
static int __fastcall lb_clean(void *, void *)
{ return _ZN9LakituBro16CleanupResourcesEv(); }
static int __fastcall lb_behavior(void *s, void *)
{ return _ZN9LakituBro8BehaviorEv(s); }
static int __fastcall lb_render(void *s, void *)
{ port_actor_render_probe("LAKITU_BRO", (char *)s + 0x110);
  return _ZN9LakituBro6RenderEv(s); }
static int __fastcall lb_pdes(void *, void *)
{ _ZN9LakituBro16OnPendingDestroyEv(); return 0; }
static int __fastcall lb_d1(void *s, void *)
{ return (int)(size_t)_ZN9LakituBroD1Ev((int *)s); }
static int __fastcall lb_d0(void *s, void *)
{ return (int)(size_t)_ZN9LakituBroD0Ev((int *)s); }

extern "C" void hal_fill_lakitu_bro_vtable(void)
{
    void **vt = _ZTV9LakituBro;
    ac_fill_shared(vt);
    vt[0] = (void *)lb_init;
    vt[3] = (void *)lb_clean;
    vt[6] = (void *)lb_behavior;
    vt[9] = (void *)lb_render;
    vt[12] = (void *)lb_pdes;
    vt[16] = (void *)lb_d1;
    vt[17] = (void *)lb_d0;
}

// ============================================================================
// GATE 19: ov098's CANNON
// ============================================================================
//
// _ZTV7daCnn_c, ov098 0x0213c6a0 (daCnn_c -- "cannon"). One instance, behind
// the grate in the west moat wall. The config's names are not shifted here
// either: slot 0 is Cannon::InitResources and slot 16 is Cannon's own D1.
//
// IT IS DORMANT ON PURPOSE. The cannon's own Behavior reads the save before
// it will open, so on a fresh save the lid stays shut -- which is what the
// level looks like before Bob-omb Buddy opens it, and the reason nothing here
// goes near the cannon-entry path.
//
// SLOTS 16/17 TRAP, the gate-17 reading twice over: nothing on this level
// destroys a cannon, and src/_ZN6CannonD1Ev.cpp is a real C++ destructor over
// its own shadow hierarchy rather than a C-named body, so hosting it would
// mean MSVC synthesising member destructor calls against a layout that is not
// the ROM's.
extern "C" {
int _ZN6Cannon13InitResourcesEv(void *self);      /* face: method_faces */
int _ZN6Cannon8BehaviorEv(void *self);            /* face: method_faces */
int _ZN6Cannon6RenderEv(void *self);              /* host copy */
int _ZN6Cannon16CleanupResourcesEv(void);
void *_ZTV6Cannon[20];
}
#pragma comment(linker, "/alternatename:__ZTV7daCnn_c=__ZTV6Cannon")

static int __fastcall cn_init(void *s, void *)
{ return _ZN6Cannon13InitResourcesEv(s); }
static int __fastcall cn_clean(void *, void *)
{ return _ZN6Cannon16CleanupResourcesEv(); }
static int __fastcall cn_behavior(void *s, void *)
{ return _ZN6Cannon8BehaviorEv(s); }
static int __fastcall cn_render(void *s, void *)
{ port_actor_render_probe("CANNON", (char *)s + 0xd4);
  return _ZN6Cannon6RenderEv(s); }

extern "C" void hal_fill_cannon_vtable(void)
{
    void **vt = _ZTV6Cannon;
    ac_fill_shared(vt);
    vt[0] = (void *)cn_init;
    vt[3] = (void *)cn_clean;
    vt[6] = (void *)cn_behavior;
    vt[9] = (void *)cn_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

// ============================================================================
// GATE 20: ov002's LAST TWO
// ============================================================================
//
// EXIT (349) and WATERFALL_MIST (197), the two classes the castle grounds
// names that live in the overlay already mounted. Neither has a model, so
// neither takes a render probe, and both are shorter than any class above:
// the Exit is a trigger box and the mist is three slots.
//
// THE CONFIG'S NAMES ARE SHIFTED HERE TOO, the other way round from ov009's.
// In ov009 the factories were named right and the methods were the
// neighbour's; in ov002 the *_SpawnInfo names are right and the METHODS carry
// the neighbour's class name. Each factory's own literal pool names the
// vtable it installs and the ROM's RTTI names that vtable back:
//
//   Exit_Spawn          -> _ZTV11VirtualDoor         RTTI 11daChScene_c
//   WaterfallMist_Spawn -> _ZTV18PoppingLavaBubbles  RTTI 16daObjWaterfall_c
//
// daChScene_c is "change scene", which is what an exit does, and
// daObjWaterfall_c is the waterfall. So the bodies below are spelled
// VirtualDoor and PoppingLavaBubbles and are the exit's and the mist's. The
// port uses the files the vtables point at and renames nothing.

// ---- EXIT (actor 349, ov002) x4 --------------------------------------------
//
// _ZTV11VirtualDoor, ov002 0x021086b4. The four level-transition volumes.
// Three of its six own slots are do-nothing returns in the ROM and the other
// three are the box: InitResources builds it and inverts the actor's matrix,
// Behavior tests the Player in that local frame, and the whole of Behavior's
// body sits behind `data_02092110 < 0` -- the pending-entrance slot the level
// boot leaves non-negative -- so on the grounds the four exits measure and
// never fire. That is the ROM's gate, not a port guard.
extern "C" {
int _ZN11VirtualDoor13InitResourcesEv(char *self);
int _ZN11VirtualDoor8BehaviorEv(char *self);
int _ZN11VirtualDoor6RenderEv(void);
int _ZN11VirtualDoor16CleanupResourcesEv(void);
void _ZN11VirtualDoor16OnPendingDestroyEv(void);
void *_ZTV11VirtualDoor[20];
}

static int __fastcall ex_init(void *s, void *)
{ return _ZN11VirtualDoor13InitResourcesEv((char *)s); }
static int __fastcall ex_clean(void *, void *)
{ return _ZN11VirtualDoor16CleanupResourcesEv(); }
static int __fastcall ex_behavior(void *s, void *)
{ return _ZN11VirtualDoor8BehaviorEv((char *)s); }
static int __fastcall ex_render(void *, void *)
{ return _ZN11VirtualDoor6RenderEv(); }
static int __fastcall ex_pdes(void *, void *)
{ _ZN11VirtualDoor16OnPendingDestroyEv(); return 0; }

extern "C" void hal_fill_exit_vtable(void)
{
    void **vt = _ZTV11VirtualDoor;
    ac_fill_shared(vt);
    vt[0] = (void *)ex_init;
    vt[3] = (void *)ex_clean;
    vt[6] = (void *)ex_behavior;
    vt[9] = (void *)ex_render;
    vt[12] = (void *)ex_pdes;
    /* SLOTS 16/17 TRAP, the gate-17 reading: nothing on the castle grounds
       destroys an exit -- the four live from the level boot to the level
       teardown -- and src/_ZN11VirtualDoorD1Ev.cpp is a real C++ destructor
       over its own shadow hierarchy, which MSVC would emit under
       ??1VirtualDoor@@UAE@XZ against a layout that is not the ROM's. */
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

// ---- WATERFALL_MIST (actor 197, ov002) x7 ----------------------------------
//
// _ZTV18PoppingLavaBubbles, ov002 0x021094a0. The moat's waterfall spray, and
// the first class the port carries that the LEVEL'S OBJECT TABLES DO NOT
// NAME: CASTLE_WATER's own InitResources ends by calling func_ov009_02111b1c,
// which spawns seven of these along one of three seven-element Vector3 arrays
// in ov009. The seven skips gate 19's census reported were that call.
//
// Three own slots; 3, 9 and 12 are ActorBase's, because the class has no
// model and nothing to clean up. InitResources is a real MSVC method (face in
// hal/method_faces.cpp) and picks a particle id from the level -- 0x24 here.
//
// ITS BEHAVIOR RUNS INTO A STUB and that is honest rather than hidden:
// Particle::System::New is the no-op in hal/reverse_bridges.cpp until the
// particle subsystem is seated, so the seven actors spawn at the ROM's own
// positions, sit in both lists and tick. Every one of their eighteen slots is
// matched src; nothing here changes when the particles arrive.
#include "PoppingLavaBubbles.h"
extern "C" {
int _ZN18PoppingLavaBubbles13InitResourcesEv(void *self);   /* face */
int _ZN18PoppingLavaBubbles8BehaviorEv(char *self);
void *_ZTV18PoppingLavaBubbles[20];
}

static int __fastcall wm_init(void *s, void *)
{ return _ZN18PoppingLavaBubbles13InitResourcesEv(s); }
static int __fastcall wm_clean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall wm_behavior(void *s, void *)
{ return _ZN18PoppingLavaBubbles8BehaviorEv((char *)s); }
static int __fastcall wm_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }

extern "C" void hal_fill_waterfall_mist_vtable(void)
{
    void **vt = _ZTV18PoppingLavaBubbles;
    ac_fill_shared(vt);
    vt[0] = (void *)wm_init;
    vt[3] = (void *)wm_clean;
    vt[6] = (void *)wm_behavior;
    vt[9] = (void *)wm_render;
    vt[12] = (void *)ac_pdes_base;
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

// ============================================================================
// GATE 21: ov100's BUTTERFLY and FISH
// ============================================================================
//
// The third ACTOR overlay. ov100 holds seven classes and the castle grounds
// names three; these are the two whose own methods need nothing the port
// cannot already spell.
//
// THE CONFIG'S NAMES CHECK OUT FOR BOTH, and that is checked rather than
// assumed -- ov100 does carry one shifted pair. Butterfly_Spawn installs
// _ZTV9Butterfly (RTTI 9daBtfly_c) and Fish_Spawn installs _ZTV4Fish (RTTI
// 8daFish_c); the shift is Door/StarDoor, which is gate 22's.

// ---- BUTTERFLY (actor 336, ov100) x2 ---------------------------------------
//
// _ZTV9Butterfly / _ZTV9daBtfly_c, ov100 0x02147e9c. The two flutterers over
// the east lawn. Eight states, all matched, seated in hal/actor_overlays.cpp;
// its Behavior is a plain C-named body that reads the state pair as two ints
// itself, so no host copy is needed (the rabbit's case, not the fish's).
//
// Render dispatches slot 5 through a LOCAL SHADOW CLASS on both the ModelAnim
// at +0xd4 and the Model at +0x138 -- ROM numbering, where two slots go to the
// destructor -- which is exactly what the dual fill in hal/cxxname_bridge.cpp
// already covers on both classes. Slot 3 never comes into it, so unlike the
// cannon this one needs no host copy of Render either.
extern "C" {
int _ZN9Butterfly13InitResourcesEv(void *self);      /* face: method_faces */
int _ZN9Butterfly8BehaviorEv(char *self);
int _ZN9Butterfly6RenderEv(void *self);              /* face: method_faces */
int _ZN9Butterfly16CleanupResourcesEv(void);
void _ZN9Butterfly16OnPendingDestroyEv(void);
void *_ZTV9Butterfly[20];
}
#pragma comment(linker, "/alternatename:__ZTV9daBtfly_c=__ZTV9Butterfly")

static int __fastcall bf_init(void *s, void *)
{ return _ZN9Butterfly13InitResourcesEv(s); }
static int __fastcall bf_clean(void *, void *)
{ return _ZN9Butterfly16CleanupResourcesEv(); }
static int __fastcall bf_behavior(void *s, void *)
{ return _ZN9Butterfly8BehaviorEv((char *)s); }
static int __fastcall bf_render(void *s, void *)
{ port_actor_render_probe("BUTTERFLY", (char *)s + 0xd4);
  return _ZN9Butterfly6RenderEv(s); }
static int __fastcall bf_pdes(void *, void *)
{ _ZN9Butterfly16OnPendingDestroyEv(); return 0; }
/* SLOT 16 IS LIVE, the bird's discovery applied here before it fires: three
   of the eight states (0, 3 and 7) call MarkForDestruction, so a woken
   swarm despawns through AfterCleanupResources -> D1. The body is
   src/_ZN9ButterflyD0Ev.c minus its final Memory::Deallocate; the six
   members in the ROM's own reverse order, not MSVC's. */
static void *__fastcall bf_d1(void *s, void *)
{
    *(int *)s = (int)(size_t)_ZTV9Butterfly;
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)s + 0x394);
    _ZN12WithMeshClsnD1Ev((char *)s + 0x1d8);
    _ZN11ShadowModelD1Ev((char *)s + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)s + 0x188);
    _ZN5ModelD1Ev((char *)s + 0x138);
    _ZN9ModelAnimD1Ev((char *)s + 0xd4);
    _ZN5ActorD2Ev(s);
    return s;
}

extern "C" void hal_fill_butterfly_vtable(void)
{
    void **vt = _ZTV9Butterfly;
    ac_fill_shared(vt);
    vt[0] = (void *)bf_init;
    vt[3] = (void *)bf_clean;
    vt[6] = (void *)bf_behavior;
    vt[9] = (void *)bf_render;
    vt[12] = (void *)bf_pdes;
    vt[16] = (void *)bf_d1;
    /* 17 keeps the trap, the bird's reading: destroy is D1 + an explicit
       Deallocate; nothing on this level calls the deleting form. */
    vt[17] = (void *)ac_trap17;
}

// ---- FISH (actor 344, ov100) x2 --------------------------------------------
//
// _ZTV4Fish / _ZTV8daFish_c, ov100 0x021484bc. The two shoals in the moat.
//
// THE FISH IS THE FIRST CLASS THE PORT CARRIES THAT CAN KILL ITSELF. Its
// Behavior looks its owner up by actor id every frame and calls
// ActorBase::MarkForDestruction when the owner is gone or refuses it -- which
// is what binds a shoal to the water it was spawned against rather than to a
// position. That makes slots 16/17 reachable in principle, so the trap there
// is a real statement and not a formality: on the castle grounds the owner is
// CASTLE_WATER, which lives as long as the level does, and the census is the
// measurement (two fish in, two fish still on the lists at frame 3000).
//
// Its Behavior is a host copy for the pointer-to-member reason
// (port/unmatched/Fish_Behavior.cpp), which also carries the seven-static
// seat. Render is the same slot-5 shadow dispatch the butterfly has.
extern "C" {
int _ZN4Fish13InitResourcesEv(void *self);           /* face: method_faces */
int _ZN4Fish8BehaviorEv(void *self);                 /* host copy */
int _ZN4Fish6RenderEv(void *self);                   /* face: method_faces */
int _ZN4Fish16CleanupResourcesEv(void *self);        /* face: method_faces */
void _ZN4Fish16OnPendingDestroyEv(void);
void *_ZTV4Fish[20];
}
#pragma comment(linker, "/alternatename:__ZTV8daFish_c=__ZTV4Fish")

static int __fastcall fs_init(void *s, void *)
{ return _ZN4Fish13InitResourcesEv(s); }
static int __fastcall fs_clean(void *s, void *)
{ return _ZN4Fish16CleanupResourcesEv(s); }
static int __fastcall fs_behavior(void *s, void *)
{ return _ZN4Fish8BehaviorEv(s); }
static int __fastcall fs_render(void *s, void *)
{ port_actor_render_probe("FISH", (char *)s + 0xd4);
  return _ZN4Fish6RenderEv(s); }
static int __fastcall fs_pdes(void *, void *)
{ _ZN4Fish16OnPendingDestroyEv(); return 0; }
/* SLOT 16 IS LIVE, the class's own comment above already said so: the owner
   test can MarkForDestruction any frame. src/_ZN4FishD0Ev.c minus the
   Deallocate -- one ModelAnim, then Actor's D2. */
static void *__fastcall fs_d1(void *s, void *)
{
    *(int *)s = (int)(size_t)_ZTV4Fish;
    _ZN9ModelAnimD1Ev((char *)s + 0xd4);
    _ZN5ActorD2Ev(s);
    return s;
}

extern "C" void hal_fill_fish_vtable(void)
{
    void **vt = _ZTV4Fish;
    ac_fill_shared(vt);
    vt[0] = (void *)fs_init;
    vt[3] = (void *)fs_clean;
    vt[6] = (void *)fs_behavior;
    vt[9] = (void *)fs_render;
    vt[12] = (void *)fs_pdes;
    vt[16] = (void *)fs_d1;
    /* 17 keeps the trap, the bird's reading: destroy is D1 + an explicit
       Deallocate; nothing on this level calls the deleting form. */
    vt[17] = (void *)ac_trap17;
}

// ============================================================================
// GATE 22: ov100's DOOR
// ============================================================================
//
// _ZTV8daDoor_c, ov100 0x02148188 -- the table config left as
// data_ov100_02148188 while handing `_ZTV4Door` to the STAR door, whose RTTI
// reads daStarGate_c. So the whole _ZN4Door* method family in src/ implements
// the star door and the real door's are the unnamed func_ov100_* block; the
// port compiles the files the vtable points at and renames nothing.
//
// The three castle doors come out of LoadDoorObjects rather than the standard
// or simple loader, which is why they carry no record in the object tables
// the other classes were read against -- the level names them as its kind
// 0x09, count 3.
//
// Both its Behavior and its Render are host copies, for the two reasons this
// gate has met all the way through: a pointer-to-member table (this time a
// POINTER THE ACTOR CARRIES at +0x140, not a fixed array) and a shadow-class
// dispatch that needs Model SLOT 4, the one the dual fill cannot serve. See
// port/unmatched/Door_Behavior.cpp and Door_Render.cpp.
extern "C" {
int func_ov100_021455a0(void *self);      /* InitResources */
int func_ov100_0214542c(void *self);      /* CleanupResources */
int func_ov100_02145550(void *self);      /* Behavior -- host copy */
int func_ov100_021454c8(void *self);      /* Render   -- host copy */
void func_ov100_021454c4(void);           /* OnPendingDestroy */
void *_ZTV8daDoor_c[20];
}

static int __fastcall dr_init(void *s, void *)
{ return func_ov100_021455a0(s); }
static int __fastcall dr_clean(void *s, void *)
{ return func_ov100_0214542c(s); }
static int __fastcall dr_behavior(void *s, void *)
{ return func_ov100_02145550(s); }
static int __fastcall dr_render(void *s, void *)
{ port_actor_render_probe("DOOR", (char *)s + 0xd4);
  return func_ov100_021454c8(s); }
static int __fastcall dr_pdes(void *, void *)
{ func_ov100_021454c4(); return 0; }

extern "C" void hal_fill_door_vtable(void)
{
    void **vt = _ZTV8daDoor_c;
    ac_fill_shared(vt);
    vt[0] = (void *)dr_init;
    vt[3] = (void *)dr_clean;
    vt[6] = (void *)dr_behavior;
    vt[9] = (void *)dr_render;
    vt[12] = (void *)dr_pdes;
    /* SLOTS 16/17 TRAP, the gate-17 reading: nothing on the castle grounds
       destroys a door -- the three live from the level boot to the teardown. */
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}

// ============================================================================
// GATE 23: ov102's QUESTION_BLOCK
// ============================================================================
//
// _ZTV13QuestionBlock / _ZTV18daObjHatenaBlock_c, ov102 0x0214e47c -- hatena
// is the question mark, which is the ROM's own RTTI agreeing with config's
// name for once. One instance, at (0,3498,-3500): the block on the castle
// roof, over the tower the cannon shoots you to.
//
// NOTHING HERE NEEDS A HOST COPY, which makes it the first class since gate 17
// that is only its own six slots. Its Behavior dispatches no pointer-to-member
// at all -- a straight-line body over a MovingMeshCollider and an Animation --
// and its Render is the slot-5 shadow dispatch on both ModelAnim and Model,
// which the dual fill in hal/cxxname_bridge.cpp already serves on both. The
// door's slot-4 problem does not arise.
//
// Slot 12 is ActorBase's own OnPendingDestroy, which is what the ROM's table
// holds -- the class does not override it.
extern "C" {
int _ZN13QuestionBlock13InitResourcesEv(char *self);
int _ZN13QuestionBlock8BehaviorEv(void *self);          /* face: method_faces */
int _ZN13QuestionBlock6RenderEv(void *self);            /* face: method_faces */
int _ZN13QuestionBlock16CleanupResourcesEv(void *self); /* face: method_faces */
void *_ZTV13QuestionBlock[20];
}
#pragma comment(linker, "/alternatename:__ZTV18daObjHatenaBlock_c=__ZTV13QuestionBlock")

static int __fastcall qb_init(void *s, void *)
{ return _ZN13QuestionBlock13InitResourcesEv((char *)s); }
static int __fastcall qb_clean(void *s, void *)
{ return _ZN13QuestionBlock16CleanupResourcesEv(s); }
static int __fastcall qb_behavior(void *s, void *)
{ return _ZN13QuestionBlock8BehaviorEv(s); }
static int __fastcall qb_render(void *s, void *)
{ port_actor_render_probe("QUESTION_BLOCK", (char *)s + 0x320);
  return _ZN13QuestionBlock6RenderEv(s); }

extern "C" void hal_fill_question_block_vtable(void)
{
    void **vt = _ZTV13QuestionBlock;
    ac_fill_shared(vt);
    vt[0] = (void *)qb_init;
    vt[3] = (void *)qb_clean;
    vt[6] = (void *)qb_behavior;
    vt[9] = (void *)qb_render;
    vt[12] = (void *)ac_pdes_base;
    /* SLOTS 16/17 TRAP, the gate-17 reading: the castle grounds' one block is
       never destroyed -- its own Behavior parks it in state 2 when it is used
       rather than marking it. */
    vt[16] = (void *)ac_trap16;
    vt[17] = (void *)ac_trap17;
}
