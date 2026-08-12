// The Stage actor, host side.
//
// On the ROM the Stage IS the scene. It is the first actor built, the one
// every other actor hangs off, and it owns in its own storage the two objects
// the harness used to stage by hand:
//
//     Stage + 0x50   Particle::SysTracker
//     Stage + 0x86c  Model            -- the level model
//     Stage + 0x91c  MeshCollider     -- the level collision
//
// The port stood in for it twice over: a bare 0x60-byte MeshCollider in
// walk_window (mc_storage) and a 0x50-byte ActorBase-shaped block seated as
// the scene root (hal_scene_root, hal/level_boot.cpp). This is the real object
// taking both seats.
//
// Nothing here is behaviour. Stage::Stage is the matched src file
// (src/_ZN5StageC3Ev.c, arm9 0x0202e088) and this is the seam it needs: the
// vtable array its last vptr store names, the spawn context the ActorBase
// constructor reads, and one honest stub for the particle constructor.
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {

void *_ZN5StageC3Ev(void);

/* The ROM's own SpawnInfo for the Stage: arm9 0x0209213c, emitted from the
   extracted image by port/tools/romdata.py (Nintendo's bytes never enter the
   repo). Layout, the same record shape hal/actor_registry.cpp documents:
       +0  factory   0x0202e088, Stage::Stage itself
       +4  u16       the actor id, which the ActorBase ctor also reads back as
                     the BEHAVIOUR-list priority  -- 3 for the Stage
       +6  u16       the RENDER-list priority                  -- 6
   Cross-checked against Camera_SpawnInfo, whose +4 is 0x14c and matches the
   registry's own row id. */
extern unsigned char _ZN5Stage9spawnDataE[];

extern void **data_020a4bb8;          /* actorID -> SpawnInfo* */
extern unsigned short data_020a4b54;  /* the PENDING actor id the ctor reads */
extern int data_020a4b6c[];           /* the scene tree */
extern void *data_0209f314;           /* the level AREA table (Stage+0x8bc) */

/* ---- the vtable -----------------------------------------------------------
   ROM SLOT ORDER, the same law as the Camera's (hal/camera_bridges.cpp) and
   ArrowSignRight's (hal/actor_vtables.cpp): MSVC slot order, host __fastcall
   thunks, everything the port cannot serve trapped BY NAME rather than left
   pointing at an arm9 address the host cannot execute.

   The array the fill targets is _ZTV5Stage, because that is what Stage::Stage
   installs: its transcription stores three vptrs in a row (data_0208e4b8,
   _ZTV5Scene, _ZTV5Stage) and the last one wins. Both other names are storage
   only, same rule as the base tables.

   Read out of the ROM at 0x020921c0 with its relocations applied:
       0  0x0202cc0c Stage::InitResources        10 0x0202e3a4 Scene::BeforeRender
       1  0x0202ddc8 Stage::BeforeInitResources  11 0x0202e398 Scene::AfterRender
       2  0x0202e62c Scene::AfterInitResources   12 0x0202b8a0 Stage::OnPendingDestroy
       3  0x0202c9a8 Stage::CleanupResources     13 0x0204357c ActorBase
       4  0x0202e5f0 Scene::BeforeCleanupRes.    14 0x0204349c ActorBase
       5  0x0202e5d0 Scene::AfterCleanupRes.     15 0x02043494 ActorBase::OnHeapCreated
       6  0x0202bbbc Stage::Behavior             16 0x02023688 ~Stage (D1)
       7  0x0202e3d4 Scene::BeforeBehavior       17 0x020236f0 ~Stage (D0)
       8  0x0202e3c8 Scene::AfterBehavior
       9  0x0202b8a4 Stage::Render
   (0x020921c0 + 18*4 is data_02092208, the per-level LVL_Overlay pointer
   table -- the table Stage::InitResources indexes by level. It is NOT two
   more vtable slots; the ROM table is eighteen entries.)

   EVERY SLOT TRAPS TODAY. Nothing dispatches through the Stage yet: it is in
   the scene tree but on neither processing list, exactly as the stand-in it
   replaces was. A trap turns the first real dispatch into a named abort with
   the slot number instead of a jump into an unmapped arm9 address, which is
   how the next step finds out which faces it owes.

   The names follow config/arm9/symbols.txt, which puts _ZTV5Stage at
   0x020921c0 and _ZTV5Scene at 0x02092680. That is the ROM's layout: the
   0x02092680 table carries the Scene destructors in slots 16/17 and the
   ActorBase default in slot 0, while 0x020921c0 is the one the constructor
   installs last. Config had the two swapped until 87a55cfab, so a checkout
   older than that calls 0x02092680 _ZTV5Stage and will not link this file. */
void *_ZTV5Stage[20];      /* the table the ctor installs last; trap-filled */
void *_ZTV5Scene[20];      /* transient ctor install, storage only */

}  /* extern "C" */

static const char *const hal_stage_slot_name[20] = {
    "InitResources", "BeforeInitResources", "AfterInitResources",
    "CleanupResources", "BeforeCleanupResources", "AfterCleanupResources",
    "Behavior", "BeforeBehavior", "AfterBehavior",
    "Render", "BeforeRender", "AfterRender",
    "OnPendingDestroy", "Virtual34", "Virtual38", "OnHeapCreated",
    "~Stage (D1)", "~Stage (D0)", "(past the table)", "(past the table)"};

static int hal_stage_trap_slot;
static int __fastcall st_trap(void *, void *)
{
    std::fprintf(stderr, "FATAL: Stage vtable slot %d (%s) is not hosted\n",
                 hal_stage_trap_slot,
                 hal_stage_slot_name[hal_stage_trap_slot & 19]);
    std::abort();
    return 0;
}

extern "C" void hal_fill_stage_vtable(void)
{
    for (int i = 0; i < 20; ++i)
        _ZTV5Stage[i] = (void *)st_trap;
}

// ---- Particle::SysTracker::SysTracker -------------------------------------
//
// THE STUB THAT USED TO LIVE HERE IS GONE, and this file now carries none.
//
// Stage::Stage's second sub-object constructor is the particle system's, at
// Stage+0x50. It used to be an empty body that printed "the particle
// subsystem is not hosted" once, on the reasoning that the tracker was read
// only by Initialise and Update and neither ran.
//
// Gate 29 linked the real one. src/_ZN8Particle10SysTrackerC1Ev.c and its
// whole closure -- func_02021c90, Particle::SimpleCallback::SimpleCallback,
// func_020226a4, func_020225fc, and the thirteen data_0208f3xx vtables they
// install -- are in slice_gate29.txt. The vtables are hal/particle_vtable.cpp.
// Nothing is needed here any more: the constructor is ordinary matched src
// and the linker finds it in the slice.

// ---- construction ----------------------------------------------------------
//
// Stage::Stage is a FACTORY, not a placement constructor: it allocates its own
// 0x9c8 through ActorBase::operator new and returns the object, the same shape
// as the Player's and the Camera's factories in the registry.
//
// The one thing it needs staged is the spawn context. ActorBase::ActorBase
// reads data_020a4bb8[data_020a4b54] and drops the record's two halfwords into
// the processing-list nodes it just built, so the pending id has to be the
// Stage's and the table has to have the Stage's record under it. Both are put
// back afterwards: the registry owns that table for every other class, and the
// Stage is not spawned through the spine that would consult it.
static void *g_stage;

extern "C" void *port_stage_object(void) { return g_stage; }

extern "C" void *port_stage_create(void)
{
    if (g_stage)
        return g_stage;

    hal_fill_stage_vtable();

    const unsigned stage_id = *(unsigned short *)(_ZN5Stage9spawnDataE + 4);
    if (stage_id != 3)
        std::fprintf(stderr, "  [stage] SPAWNDATA LOOKS WRONG: +4 is %u, the "
                     "ROM's Stage id is 3\n", stage_id);

    void *saved_info = data_020a4bb8[stage_id];
    unsigned short saved_pending = data_020a4b54;
    data_020a4bb8[stage_id] = _ZN5Stage9spawnDataE;
    data_020a4b54 = (unsigned short)stage_id;

    g_stage = _ZN5StageC3Ev();

    data_020a4bb8[stage_id] = saved_info;
    data_020a4b54 = saved_pending;

    if (!g_stage) {
        std::fprintf(stderr, "FATAL: Stage::Stage returned null\n");
        std::abort();
    }

    /* The tree head, asserted rather than assumed. The Stage is built with
       data_020a4b6c[0] still null, so func_0203b438 takes its no-parent branch
       and writes the Stage's own SceneNode into the head -- which is how the
       ROM's tree gets its root. If that ever stops holding, every actor
       spawned after it hangs off nothing and the phase-1 pass walks a null. */
    if (data_020a4b6c[0] != (int)(size_t)((char *)g_stage + 0x14))
        std::fprintf(stderr, "  [stage] TREE HEAD IS NOT THE STAGE: head %p, "
                     "Stage node %p\n", (void *)(size_t)data_020a4b6c[0],
                     (void *)((char *)g_stage + 0x14));

    /* ---- THE PAUSE BITS, and who really owns them -------------------------
       Stage::Stage sets its own +0x13 bits 0 and 2 -- verified instruction by
       instruction against the arm9 bytes at 0x0202e088, which really do read
       `ldrb; orr #1; strb; ldrb; orr #4; strb`. The Stage is built PAUSED and
       HIDDEN because on the ROM it is built while the level is still loading.

       That is not a detail the port can ignore, because the phase-1 pass
       (func_02043880) propagates a parent's bits 0|1 into every CHILD's bit 1,
       and ActorBase::BeforeBehavior returns 0 on bit 1. The moment the Stage
       became the scene root every actor under it -- the Player included --
       stopped ticking and sat at its spawn point.

       The ROM clears them in Scene::BeforeBehavior (0x0202e3d4, matched src
       _ZN5Scene14BeforeBehaviorEv.cpp), in exactly one place:

           if ((*(u8*)(self + 0x13) & 1) != 0) {
               if (func_020431c4(self) == 0) { *p13 &= ~1; *p13 &= ~4; }
               return 0;
           }

       which runs because the Stage is ON THE BEHAVIOUR PROCESSING LIST at
       priority 3 and its vtable slot 7 is that function. The port does not run
       the Stage as an actor yet, so nothing calls it.

       THIS LINE IS A STAND-IN FOR THAT ONE STATEMENT and nothing else. It is
       here rather than in the ctor because the ctor is matched src. Retiring
       it is a named job: link _ZN5Scene14BeforeBehaviorEv, give the Stage the
       processing-list seat its SpawnInfo already describes, and let slot 7 do
       it. That also needs func_020431c4 and the data_0209f5bc scene manager,
       which is why it is not this commit. */
    *(unsigned char *)((char *)g_stage + 0x13) &= (unsigned char)~(1 | 4);

    std::printf("[stage] flags +0x13 = 0x%02x after the stand-in clear "
                "(ctor leaves 0x05; Scene::BeforeBehavior owns this on the "
                "ROM)\n", *(unsigned char *)((char *)g_stage + 0x13));
    /* ---- the AREA TABLE, which Stage::InitResources owns ------------------
       Stage+0x8bc is the level's area table (stride 0xc: the area's
       TextureTransformer at +0, the "is this area showing" flag at +4).
       ShowArea/HideArea/IsAreaShowing all reach it through data_0209f314, and
       Stage::InitResources' own line is

           data_0209f314 = (void *)((char *)thiz + 0x8bc);

       The port had it pointed at a 64-entry static in hal/camera_bridges.cpp
       instead, seated back when no Stage existed. That static is exactly why
       Stage::RenderModel drew an empty world the first time it was switched
       on: Camera::InitResources calls ChangeArea with the entrance's area,
       ChangeArea calls ShowArea, and ShowArea was setting the flag in the
       HARNESS table while RenderModel read the Stage's own -- which is zero,
       and a zero flag means "this area is hidden", so every component of the
       castle got bit 31 set and the level vanished.
       Seated here, before the boot, because the Camera is spawned inside it. */
    data_0209f314 = (char *)g_stage + 0x8bc;

    std::printf("[stage] Stage actor %p (id %u, prio beh %u ren %u), "
                "collider +0x91c %p, model +0x86c %p\n", g_stage, stage_id,
                *(unsigned short *)(_ZN5Stage9spawnDataE + 4),
                *(unsigned short *)(_ZN5Stage9spawnDataE + 6),
                (void *)((char *)g_stage + 0x91c),
                (void *)((char *)g_stage + 0x86c));
    return g_stage;
}

// ---- the tree probe --------------------------------------------------------
//
// THE FLAG LAW, read back rather than assumed. Stage::Stage sets its own
// +0x13 bits 0 and 2, and the phase-1 pass (func_02043880) propagates a
// parent's bits 0|1 into every CHILD's bit 1 -- which ActorBase::BeforeBehavior
// reads as "skip this actor's behaviour entirely". So whether the Stage is
// merely the spawn parent (data_0209f5c0) or the actual SCENE-TREE parent
// (SceneNode+0x0) is the difference between a level that runs and one frozen
// at its spawn point.
//
// The two are NOT the same seat. func_0203b438 links a node under the parent
// node it is HANDED, which the ActorBase constructor takes from data_020a4b64,
// not from data_0209f5c0.
extern "C" {
int func_02043810(void *actor);
extern int data_020a4b64[];
}

extern "C" void port_stage_tree_probe(void *child, const char *what)
{
    char *s = (char *)g_stage;
    std::printf("[stage] tree: head %p, Stage node %p, Stage flags %02x, "
                "spawn parent node %p\n", (void *)(size_t)data_020a4b6c[0],
                (void *)(s + 0x14), *(unsigned char *)(s + 0x13),
                (void *)(size_t)data_020a4b64[0]);
    if (!child)
        return;
    char *c = (char *)child;
    void *parent = (void *)(size_t)func_02043810(child);
    std::printf("[stage] tree: %s %p node.parent %p -> parent actor %p%s, "
                "flags %02x\n", what, child, *(void **)(c + 0x14), parent,
                parent == (void *)s ? " (THE STAGE)" : "",
                *(unsigned char *)(c + 0x13));
}

// ---- the render face -------------------------------------------------------
//
// Stage::RenderModel is a real MSVC method compiled against include/Stage.h,
// so it lands in the map as ?RenderModel@Stage@@QAEXXZ and no C caller can
// spell it. Same face pattern as hal/method_faces.cpp.
#include "Stage.h"
#include "Animation.h"
extern "C" void port_stage_render_model(void *self)
{ ((Stage *)self)->Stage::RenderModel(); }
extern "C" void port_stage_render_model_transparent(void *self)
{ ((Stage *)self)->Stage::RenderModelTransparent(); }

/* THE LEVEL'S OWN TEXTURE ANIMATIONS -- the waterfall, and every other BTA
   the stage model carries. Two halves of Stage code own them and the port
   ran neither, so the waterfall held frame 0 (the 2026-08-07 session):

     1. Stage::InitResources calls Stage::LoadTextureTransformers, which
        news a TextureTransformer into the area table's +0 slot for every
        area entry that names a BTA.
     2. Stage::Render's first block advances every SHOWN area's transformer
        each frame -- gated on the same pause trio Actor::BeforeBehavior
        reads -- and RenderModel (which the port already runs) then applies
        the current frame to the model's materials.

   This bridge is both halves at the same seam. The load is lazy and re-runs
   when the level id changes, because the port's level change keeps the Stage
   alive and never runs Stage::CleanupResources -- the slots would otherwise
   alias the OLD level's freed BTA file on the first post-change frame. The
   clear leaks the 0x14-byte transformer objects the ROM's cleanup would
   delete; a level change costs at most a few hundred bytes until the Stage
   runs as a real actor and its own InitResources/CleanupResources pair owns
   this. */
extern "C" {
extern signed char data_0209f2f8;                /* current level id */
extern int data_0209f294[], data_0209f2c4[], data_0209f20c[];
extern unsigned char *data_0209f340;             /* the level's area info */
}
/* The lazy load's trigger, hoisted to file scope so the level change can arm
   it. -2 is "nothing loaded yet", a value no data_0209f2f8 ever takes.

   THE LEVEL ID ALONE IS NOT A GOOD ENOUGH TRIGGER, and a SELF-WARP is why:
   level A -> level A leaves data_0209f2f8 unchanged, so the guard below never
   fires, while the boot in between has released and re-loaded that level's
   BTA. The surviving slots then point at transformers built over the freed
   copy of the file -- precisely the aliasing this guard exists to prevent, in
   the one case the guard cannot see. port_level_stage_reseat arms it
   explicitly per entry rather than leaving it to be inferred. */
static int port_stage_anims_loaded_level = -2;

extern "C" void port_stage_anims_rearm(void)
{
    port_stage_anims_loaded_level = -2;
}

extern "C" void port_stage_advance_anims(void *self)
{
    unsigned char *info = data_0209f340;
    if (!info)
        return;
    const unsigned n = info[0x14];               /* area count */
    char *slots = (char *)self + 0x8bc;          /* stride 0xc: anim, flag */

    int &loaded_level = port_stage_anims_loaded_level;
    if (loaded_level != (int)data_0209f2f8) {
        loaded_level = (int)data_0209f2f8;
        for (unsigned i = 0; i < n; ++i)
            *(void **)(slots + i * 0xc) = 0;     /* drop the old level's */
        ((Stage *)self)->Stage::LoadTextureTransformers();
        int live = 0;
        for (unsigned i = 0; i < n; ++i)
            if (*(void **)(slots + i * 0xc))
                ++live;
        std::printf("[stage] texture transformers: %d of %u areas animate "
                    "(level %d)\n", live, n, loaded_level);
    }

    if ((data_0209f294[0] | data_0209f2c4[0] | data_0209f20c[0]) & 0xff)
        return;
    for (unsigned i = 0; i < n; ++i) {
        Animation *a = *(Animation **)(slots + i * 0xc);
        if (a && *(unsigned char *)(slots + i * 0xc + 4))
            a->Animation::Advance();
    }
}

/* The SKYBOX draw, which is Stage::Render's own block between RenderFog and
   RenderModel (src/_ZN5Stage6RenderEv.cpp): the Model that LoadSkybox parked
   at Stage+0x9bc, its matrix translation (mat4x3 words 9..11 at +0x1c) glued
   to the camera EYE -- Camera+0x8c, the Vector3 Camera::Render hands LookAt_
   -- shifted from world fix12 into scene units by the same >> 3 the view
   matrix gets. Draw(0) on the ROM is the model vptr's slot 5, which is
   Model::Render(const Vector3 *) with a NULL scale, so the qualified call is
   the same dispatch. NULL Stage+0x9bc means GetSkyboxID said "no skybox"
   (LoadSkybox's early return) and the block is a no-op, same as the ROM. */
#include "Model.h"
extern "C" { extern void *data_0209f318; }   /* the Camera singleton */
extern "C" void port_stage_render_skybox(void *self)
{
    Model *sky = *(Model **)((char *)self + 0x9bc);
    if (!sky)
        return;
    int *m = (int *)((char *)sky + 0x1c);
    const int *eye = (const int *)((const char *)data_0209f318 + 0x8c);
    m[9] = eye[0] >> 3;
    m[10] = eye[1] >> 3;
    m[11] = eye[2] >> 3;
    sky->Model::Render(0);
}
