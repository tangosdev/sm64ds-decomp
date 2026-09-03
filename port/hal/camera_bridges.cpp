// Gate 13: the real Camera actor -- vtable, method faces, the two comms
// blocks the angle chain publishes through, and the engine globals the
// camera's own boot reads.
//
// Nothing here is behaviour. Camera::InitResources, Behavior and Render are
// the matched src files; this is the seam they need to reach the host.
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ActorBase.h"
#include "ActorDerived.h"
#include "Camera.h"
#include "PathPtr.h"
#include "dsstate_seg.h"
#include "ntr/ppu.h"   /* ntr::SCREEN_W/SCREEN_H for the widescreen frustum seam */

/* The assembled-triangle counter, for the widescreen probe below and nothing
   else. FORWARD-DECLARED rather than #included. The widescreen seam is compiled
   unconditionally now (the aspect is a runtime choice, not a compile tier), so
   there is no tier guard left to sit inside. Same shape hal/player_bridges.cpp
   uses for the same call. */
#include <cstddef>
namespace ntr { struct GxTriangle; const GxTriangle *gx_polygons(std::size_t &n); }

/* Camera::Render calls View::Render() as a METHOD (its TU declares a local
   `struct View`); src defines the function at C linkage. Same shape as the
   method faces in method_faces.cpp, kept here because the local View has no
   header. */
struct View {
    int render();
    int Render();
};
extern "C" int _ZN4View6RenderEv(void *self);
int View::Render() { return _ZN4View6RenderEv(this); }

extern "C" {

int _ZNK6Camera12IsUnderwaterEv(const void *self);
int _ZN6Camera16CleanupResourcesEv(void *self);
void *_ZN6CameraD1Ev(void *self);
void *_ZN6CameraD0Ev(void *self);
void *_ZN6CameraC1Ev(void *self);

/* method faces (the definitions are MSVC methods; every caller and the
   vtable want C names) */
int _ZN6Camera13InitResourcesEv(void *self)
{ return ((Camera *)self)->Camera::InitResources(); }
int _ZN6Camera8BehaviorEv(void *self)
{ return ((Camera *)self)->Camera::Behavior(); }
int _ZN6Camera6RenderEv(void *self)
{ return ((Camera *)self)->Camera::Render(); }
void _ZN6Camera16OnPendingDestroyEv(void *self)
{ ((Camera *)self)->Camera::OnPendingDestroy(); }

/* ---- the vtable -------------------------------------------------------
   ROM SLOT ORDER, the ArrowSignRight layout (hal/actor_vtables.cpp): the
   Camera's own overrides at 0, 3, 6, 9, 12 and 16/17, the ActorBase and
   ActorDerived defaults everywhere else. Filled at runtime rather than
   statically because the slot bodies are __fastcall thunks over qualified
   calls, exactly like the ArrowSignRight table.

   The one slot that actually dispatches on host is 9: Camera::InitResources
   ends in `return this->v9()`, a virtual call through the vptr the ctor just
   installed. So the table has to be up BEFORE InitResources runs (R9). */
void *_ZTV6Camera[20];
/* transiently installed by the ctor/dtor chain and never dispatched
   through; storage only, same rule as the base tables in actor_vtables. */
void *_ZTV4View[20];

}  /* extern "C" */

static int __fastcall cs_init(void *s, void *)
{ return _ZN6Camera13InitResourcesEv(s); }
static int __fastcall cs_binit(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeInitResources(); }
static void __fastcall cs_ainit(void *s, void *, unsigned a)
{ ((ActorDerived *)s)->ActorDerived::AfterInitResources(a); }
static int __fastcall cs_cleanup(void *s, void *)
{ return _ZN6Camera16CleanupResourcesEv(s); }
static int __fastcall cs_bclean(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeCleanupResources(); }
static void __fastcall cs_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall cs_behavior(void *s, void *)
{ return _ZN6Camera8BehaviorEv(s); }
static int __fastcall cs_bbeh(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeBehavior(); }
static void __fastcall cs_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall cs_render(void *s, void *)
{ return _ZN6Camera6RenderEv(s); }
static int __fastcall cs_bren(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::BeforeRender(); }
static void __fastcall cs_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall cs_pdes(void *s, void *)
{ _ZN6Camera16OnPendingDestroyEv(s); return 0; }
/* Slots 13/14 trap, the ArrowSignRight rule: they are the two heap-creating
   virtuals, reached only from the actor-processing lifecycle the port does
   not run. Wiring them would drag the solid-heap family in for a path
   nothing calls. */
static void __fastcall cs_trap13(void *, void *)
{ std::fprintf(stderr, "FATAL: Camera vtable slot 13 trap\n"); std::abort(); }
static void __fastcall cs_trap14(void *, void *)
{ std::fprintf(stderr, "FATAL: Camera vtable slot 14 trap\n"); std::abort(); }
static int __fastcall cs_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static void *__fastcall cs_d1(void *s, void *) { return _ZN6CameraD1Ev(s); }
static void *__fastcall cs_d0(void *s, void *) { return _ZN6CameraD0Ev(s); }

extern "C" void hal_fill_camera_vtable(void)
{
    _ZTV6Camera[0] = (void *)cs_init;
    _ZTV6Camera[1] = (void *)cs_binit;
    _ZTV6Camera[2] = (void *)cs_ainit;
    _ZTV6Camera[3] = (void *)cs_cleanup;
    _ZTV6Camera[4] = (void *)cs_bclean;
    _ZTV6Camera[5] = (void *)cs_aclean;
    _ZTV6Camera[6] = (void *)cs_behavior;
    _ZTV6Camera[7] = (void *)cs_bbeh;
    _ZTV6Camera[8] = (void *)cs_abeh;
    _ZTV6Camera[9] = (void *)cs_render;
    _ZTV6Camera[10] = (void *)cs_bren;
    _ZTV6Camera[11] = (void *)cs_aren;
    _ZTV6Camera[12] = (void *)cs_pdes;
    _ZTV6Camera[13] = (void *)cs_trap13;
    _ZTV6Camera[14] = (void *)cs_trap14;
    _ZTV6Camera[15] = (void *)cs_heap;
    _ZTV6Camera[16] = (void *)cs_d1;
    _ZTV6Camera[17] = (void *)cs_d0;
}

/* ---- who drives the camera's frame ---------------------------------------
   Gate 16 put the ROM's own processing lists back in charge of the actor
   frame, and the Camera is on both of them -- behaviour priority 0x14c,
   render priority 0, which puts it at the head of the render bucket exactly
   where a camera belongs.

   The harness still drives Camera::Behavior and Camera::Render by hand,
   though, because neither call is alone in the window's frame loop: Behavior
   has to be followed by func_0203e0ac (the single-player echo of the heading
   record), and Render has to happen before the actor bucket rather than
   inside it -- the bucket's own models compose against the view matrix
   Render publishes. Left dispatchable, the list passes would run each a
   second time, and a second Render mid-bucket would republish the view
   matrix underneath models already drawn against it.

   (Until the scene-unit migration there was a third reason: Render was
   followed by the R6 unit shim, and a list-driven second Render rebuilt the
   view matrix without it, putting every model at an eighth of its
   translation. The shim is gone; the ordering reason is not.)

   So the two slots go quiet and say so. This is the same bargain the Player's
   Render slot takes; both come back the day the frame itself is the ROM's
   (stage C's Scene::Render). */
static int __fastcall cs_harness(void *, void *) { return 1; }

extern "C" void hal_camera_slots_harness_owned(void)
{
    _ZTV6Camera[6] = (void *)cs_harness;    /* Behavior + func_0203e0ac */
    _ZTV6Camera[9] = (void *)cs_harness;    /* Render, before the bucket */
}

/* ---- the local/per-player comms blocks --------------------------------
   Camera::Behavior publishes its heading into the LOCAL record
   data_020a1040 (func_0203dafc writes data_020a1040[8], = 0x020a1050), and
   func_0203e0ac shuffles that record down four per-player records at
   data_020a1154, stride 0x24. GetAngleToCamera(i) then reads
   data_020a1164 + i*0x24 -- record 0's +0x10 field.

   Both blocks are ONE object each on the DS with several named symbols
   inside; func_0203e0ac reads three of them (data_020a117c / 11a0 / 11c4)
   AND the same bytes through data_020a1154 + offset, in the same statement.
   Separate host arrays would make the copy read one place and write
   another, and the published angle would never move. Grouped sections put
   them back in ROM order -- see CONTIG in port/tools/romdata.py for the
   mechanism; sizes here are the symbol deltas, all even, so align(2) packs
   with no interior padding. */
#define COMM(sec, name, size) \
    __pragma(section(sec, read, write))                          \
    extern "C" __declspec(allocate(sec)) __declspec(align(2))    \
    unsigned char name[size] = {0}

/* 0x020a1040 .. 0x020a1064: the local record (0x24) */
COMM(".dsstate$camcomm0000", data_020a1040, 4);
COMM(".dsstate$camcomm0001", data_020a1044, 2);
COMM(".dsstate$camcomm0002", data_020a1046, 2);
COMM(".dsstate$camcomm0003", data_020a1048, 4);
COMM(".dsstate$camcomm0004", data_020a104c, 2);
COMM(".dsstate$camcomm0005", data_020a104e, 2);
COMM(".dsstate$camcomm0006", data_020a1050, 2);
COMM(".dsstate$camcomm0007", data_020a1052, 0x12);

/* 0x020a1154 .. 0x020a11e4: four per-player records (0x24 each) on the DS.
   THE HOST GIVES THE RUN SIXTEEN RECORDS -- run vs16, hosted-conductor
   follow-up. The ROM's readers index these records BY SLOT with no bound of
   their own (func_0203d950(i) reads +0xC*, GetAngleToCamera(i) reads +0x10,
   func_0203dabc(i) hands out +0x04 -- all at data_020a1154 + i*0x24), so a
   session whose slots go past 3 needs records 4..15 to exist CONTIGUOUSLY at
   the same stride; a side array would satisfy the wide conductor and leave
   every one of those readers reading garbage. Grown the only legal way for a
   grouped band: the TAIL member, so every interior offset the layout check
   asserts is exactly where it was. The cartridge's own extent stays what the
   comment above says it is; bytes 0x90..0x240 are the port's, zeroed at
   session-open by the seam (hal/comms_seam.cpp, func_020408b0) the same way
   src/func_0203db64.c:64 zeroes the first 0x90 at session start. */
COMM(".dsstate$camrec0000", data_020a1154, 0xc);
COMM(".dsstate$camrec0001", data_020a1160, 2);
COMM(".dsstate$camrec0002", data_020a1162, 2);
COMM(".dsstate$camrec0003", data_020a1164, 2);
COMM(".dsstate$camrec0004", data_020a1166, 0x12);
COMM(".dsstate$camrec0005", data_020a1178, 4);
COMM(".dsstate$camrec0006", data_020a117c, 0x24);
COMM(".dsstate$camrec0007", data_020a11a0, 0x24);
COMM(".dsstate$camrec0008", data_020a11c4, 0x20 + 12 * 0x24);

#undef COMM

extern "C" {
/* romdata.py emits these next to the .cammod run */
extern unsigned cammod_run_base, cammod_run_end, cammod_run_span;
extern unsigned char data_02086fcc[];
extern unsigned char data_0208715c[];
extern unsigned char data_0208738c[];

/* Grouped sections are a LINKER promise; assert it once at bring-up rather
   than debugging a camera that reads a neighbouring mode's numbers. */
int hal_camera_check_layout(void)
{
    int ok = 1;
    unsigned span = cammod_run_end - cammod_run_base;
    if (span != cammod_run_span) {
        std::fprintf(stderr,
                     "  [cam] MODE TABLE NOT CONTIGUOUS: %u bytes linked, "
                     "%u expected\n", span, cammod_run_span);
        ok = 0;
    }
    if (data_0208715c - data_02086fcc != 0x190 ||
        data_0208738c - data_02086fcc != 0x3c0) {
        std::fprintf(stderr, "  [cam] MODE TABLE ORDER WRONG: +%d / +%d\n",
                     (int)(data_0208715c - data_02086fcc),
                     (int)(data_0208738c - data_02086fcc));
        ok = 0;
    }
    if (data_020a1164 - data_020a1154 != 0x10 ||
        data_020a117c - data_020a1154 != 0x28 ||
        data_020a11c4 - data_020a1154 != 0x70 ||
        data_020a1050 - data_020a1040 != 0x10) {
        std::fprintf(stderr, "  [cam] COMMS BLOCKS NOT CONTIGUOUS\n");
        ok = 0;
    }
    /* Run vs16: the record run must reach sixteen 0x24 records, because the
       wide conductor and the ROM's own slot-indexed readers walk that far.
       The tail member's own size is a compile-time fact in this TU, so this
       is really checking that nobody shrank it back without reading the
       band comment above. */
    if ((data_020a11c4 - data_020a1154) + (long)sizeof(data_020a11c4)
            != 16 * 0x24) {
        std::fprintf(stderr, "  [cam] COMMS RECORD RUN IS NOT SIXTEEN WIDE\n");
        ok = 0;
    }
    return ok;
}

/* ---- engine globals the camera boot reads ------------------------------
   The camera is the first actor the port spawns that walks the area and
   view-object tables, so the two POINTERS those tables hang off need real
   storage. Both are empty on purpose: with zero view objects
   (data_0209f1f8 = 0) InitResources skips its whole scan, and the camera
   stays in mode 10 -- the correct minimal behaviour for a level whose
   Stage loader has not run (R14). */
DSSTATE_BEGIN
static char hal_area_table[64 * 12];
static char hal_viewobj_table[64 * 0xe];
void *data_0209f314 = hal_area_table;
void *data_0209f354 = hal_viewobj_table;
unsigned char data_0209f1f8;         /* view-object count */
/* data_0209f250 is Player_InitResources.cpp's; data_0209f394 cxxname_bridge's */
int data_0209b000;

/* 0x0209b3ec is the view matrix CopyToViewMat writes; 0x0209b41c is the
   INVERSE it derives, which Camera::Behavior feeds MulVec3Mat4x3 to place
   weather particles in camera space. */
int data_0209b41c[12];

/* scaleW: Camera::Render reads data_0209ee90[0x44/4] and hands it to
   PerspectiveW_ as the W scale. The symbol was int[8] in auto_bss, so
   index 17 was off the end -- a read of whatever followed. Real extent is
   0x348 bytes (delta to the next symbol), and 0x1000 is the identity
   scale the perspective divide expects. */
int data_0209ee90[0x348 / 4];

/* Clipper: the object Camera's frustum test runs against. 0x5c bytes,
   zeroed here but LIVE from the first Render on: Camera::Render ends in
   func_0200d954, which calls Clipper::Func_020156DC (near/far planes from
   the camera's own +0xfc/+0x100) and through it Func_0201559C (the four
   side planes from the fov table). Actor::BeforeBehavior reads it every
   frame -- an actor whose SpawnInfo carries 0x10000 skips its Behavior
   whenever the test fails, which on the castle grounds is the AUTHENTIC
   dormancy of the ambient set (bird/butterfly/fish), not a port gap. */
int data_0209f43c[0x5c / 4];
DSSTATE_END

/* WIDESCREEN OBJECT-CULL FRUSTUM (16:9), the host seam the first pass could not
   reach. data_0209f43c above is the global Clipper the object Camera tests every
   ambient actor against: Camera::Render ends in func_0200d954, which seeds the
   clipper through Clipper::Func_020156DC with the camera's own aspect
   (camera[0xf8], 0x1555 = 4:3), and Func_0201559C then builds the four SIDE
   planes with a horizontal half-extent c = (m4c * b) >> 12 -- m4c is byte 0x4c,
   the aspect term. On the 1024x576 frame the 3D field is widened Hor+
   (ntr/gx.cpp scales clip.x by (4/3)*SCREEN_H/SCREEN_W = 0.75), so the CULL
   frustum has to widen by the inverse or the birds, butterflies and fish that
   sit just past the old 4:3 edge stay in the authentic dormancy the byte-frozen
   src gives them -- Actor::BeforeBehavior skips a 0x10000-flagged actor's
   Behavior whenever this test fails.

   THE SEAM WIDENS m4c AND REBUILDS THE PLANES, both host-side and both gated on
   the tier: scale m4c by target/native = (SCREEN_W/SCREEN_H)/(4/3), which is the
   exact inverse of gx.cpp's 0.75, then re-run Func_0201559C so the side planes
   come back off the wider aspect. func_0200d954 reseeds m4c from 0x1555 every
   Render, so this is called AFTER the render seeds it and BEFORE the actor
   buckets read it (hal drives both by hand; see the frame loop in
   tests/walk_window.cpp), and it never accumulates: each frame starts from the
   src's own value and is widened once. Scaling the value that is THERE rather
   than a literal 0x1555 keeps a camera or cutscene that seeds a different aspect
   correct. Empty and uncalled on the 4:3 targets. */
/* Compiled unconditionally now, and CALLED only when ntr::widescreen is set
   (the frame loop guards the one call site). The scale is target/native =
   (active_w/active_h)/(4/3), which is 1.333 at 16:9 and EXACTLY 1.0 at 4:3, so
   even if it were called on a 4:3 run it would be a no-op; the call guard keeps
   the 4:3 clipper untouched regardless. */
void _ZN7Clipper13Func_0201559CEv(void *self);

/* SM64DS_WIDEN_PROBE=1: PHASE-1 REACHABILITY COUNTER, and the only reason it
   exists is that the call to this function is made BY HAND from one frame loop
   (tests/walk_window.cpp). Any render path that does not run that loop seeds
   the object Clipper from the camera's own 4:3 aspect and never widens it, so
   ambient actors at the new 16:9 margins stay culled as off-screen while the
   raster draws the world behind them. "Is that loop the only loop" cannot be
   answered by reading -- a scene, a minigame and a cutscene each get a camera
   rendered somewhere -- so it is answered by counting.

   The count is a single static increment, unconditional and untimed; NOTHING is
   printed unless SM64DS_WIDEN_PROBE is set to something other than 0. The
   progress line every 64 calls is deliberate belt and braces: a run that dies on
   a fault or is killed by a timeout never reaches a static destructor, and a
   reachability answer that only exists at a clean exit is no answer at all. */
static unsigned long g_widen_calls;
/* scene frames the widen DECLINED; see hal_camera_widen_frustum_scene below */
static unsigned long g_widen_skips;
namespace {
int widen_probe_on()
{
    static int on = -1;
    if (on < 0) {
        const char *e = std::getenv("SM64DS_WIDEN_PROBE");
        on = (e && *e && *e != '0') ? 1 : 0;
    }
    return on;
}
struct WidenProbeReport {
    ~WidenProbeReport()
    {
        if (widen_probe_on())
            std::fprintf(stderr, "[widen] total calls %lu, scene frames "
                                 "declined %lu\n",
                         g_widen_calls, g_widen_skips);
    }
};
WidenProbeReport g_widen_probe_report;
}  // namespace

void hal_camera_widen_frustum(void)
{
    ++g_widen_calls;
    if (widen_probe_on() && (g_widen_calls == 1 || g_widen_calls % 64 == 0))
        std::fprintf(stderr, "[widen] call %lu m4c %d\n", g_widen_calls,
                     data_0209f43c[0x4c / 4]);
    long long m4c = data_0209f43c[0x4c / 4];
    data_0209f43c[0x4c / 4] =
        (int)((m4c * (ntr::active_w * 3)) / (ntr::active_h * 4));
    _ZN7Clipper13Func_0201559CEv(&data_0209f43c);
}

/* SM64DS_WIDEN_PROBE, PHASE 2: THE SCENE PATH'S OWN LINE. Same variable, same
   inert-when-unset rule, and inside the same tier guard, so the 4:3 and hires
   targets do not see a byte of it. Two numbers per frame, and each one answers
   a question the phase-1 counter above could not:

     m4c   -- the object Clipper's aspect term AS THE FRAME LEFT IT. The level
              path's widen is safe ONLY because Camera::Render reseeds this word
              from 0x1555 every frame, so scaling it never compounds (measured
              on level 1, 300 frames: m4c reads 5461 at every sampled call). The
              scene path drives no camera by hand, so whether it HAS that reseed
              is not something reading the file can answer -- a widen dropped on
              a path with no reseed multiplies the same word by 4/3 three
              hundred times and overflows it. This line settles that before the
              call is placed, and keeps watching it afterwards.
     tris  -- triangles submitted this frame, which is the ONLY thing an object
              cull can move. A frustum that lets more through submits more.
              Per frame rather than a histogram because ppu_audit keeps six
              distinct values per slot and the busiest scenes have more than
              six, so no mean can be taken off it.

   Unbuffered and per frame for the reason the phase-1 counter is: a run that
   faults or is killed never reaches a static destructor, and a number that only
   exists at a clean exit is no number at all. */
void hal_widen_probe_scene_frame(int frame, const char *where)
{
    if (!widen_probe_on()) return;
    std::size_t tris = 0;
    ntr::gx_polygons(tris);
    std::fprintf(stderr, "[widen] scene f%d %s m4c %d tris %lu\n", frame, where,
                 data_0209f43c[0x4c / 4], (unsigned long)tris);
}

/* THE SCENE PATH'S WIDEN, and it is a SEPARATE ENTRY POINT rather than the same
   call in a second place, because the scene path does not come with the one
   precondition the level path gets for free.

   The level loop calls hal_camera_widen_frustum immediately after its by-hand
   hal_camera_render, so the Clipper it scales has JUST been seeded by
   func_0200d954 -> Clipper::Func_020156DC and the aspect term is known to be the
   src's own 0x1555. That is the whole reason a non-idempotent scale is safe
   there: every frame starts from 5461 and is widened exactly once. Measured on
   level 1, 300 frames -- m4c reads 5461 at every sampled call.

   THE SCENE PATH DRIVES NO CAMERA BY HAND, and measuring it with
   SM64DS_WIDEN_PROBE over all 27 hosted scene ids found not two shapes but
   THREE, which is why this is a guard and not a second call site:

     NEVER SEEDED   SCENE_STAR_SELECT (4), SCENE_MG_CURLING (374) and the 2D
                    minigames. m4c reads 0 on frame 0 and on frame 299. Nothing
                    ever built an object cull for these scenes, so there is
                    nothing to widen, and Func_0201559C over an all-zero Clipper
                    would build four side planes out of a zero aspect -- a state
                    the ROM never puts that structure in.
     RESEEDED       SCENE_MG_MEMORY2 (363), SCENE_MG_JUMP2 (373) and the rest.
                    Their Camera actor rides the ROM's own render bucket, so
                    port_actor_render rewrites m4c to 5461 every frame. This is
                    the level path's contract and a per-frame widen is right.
     SEEDED ONCE    SCENE_MG_CUP (361), SCENE_MG_BOOMBOX (367). m4c is written
                    at spawn and NEVER AGAIN. A per-frame widen on these
                    multiplies the same word by 4/3 three hundred times:
                    measured, before this guard existed, 5461 -> 7281 -> 9708 ->
                    ... -> 1540606696 by frame 299, an overflowed aspect feeding
                    four garbage side planes. That is the failure the level
                    path's structure makes impossible and the scene path's does
                    not.

   SO THE RULE IS ONCE PER SEED, NOT ONCE PER FRAME. Remember the value this seam
   last wrote; if the word still holds it, the ROM has not reseeded since and the
   frustum is already the widened one, so decline. A reseed lands a different
   value, is noticed, and is widened. On a RESEEDED scene that is a widen every
   frame, identical to the level path; on a SEEDED ONCE scene it is one widen
   that then persists, which is the correct answer for a Clipper written once;
   on a NEVER SEEDED scene it is nothing at all.

   THE ONE AMBIGUITY IS NAMED RATHER THAN HIDDEN. A reseed that happens to write
   exactly the value the last widen produced is indistinguishable from no reseed,
   and this declines it -- so that frame keeps the aspect it already has. The
   direction is deliberate: declining is a no-op, and widening a second time
   corrupts. The skip counter makes every declined frame a NUMBER rather than an
   assumption about which scenes reseed. */
static int g_widen_scene_written;   /* what this seam last wrote; 0 = nothing */
void hal_camera_widen_frustum_scene(void)
{
    const int m4c = data_0209f43c[0x4c / 4];
    if (m4c == 0) {
        /* the scene tore its Clipper down, or never built one: forget the
           memory so the next scene's first seed is widened rather than
           mistaken for a value this seam had already written */
        g_widen_scene_written = 0;
        ++g_widen_skips;
        if (widen_probe_on() && g_widen_skips == 1)
            std::fprintf(stderr, "[widen] scene DECLINED: the object Clipper has "
                                 "no aspect term (m4c 0), so no Render seeded it "
                                 "and there is no cull to widen\n");
        return;
    }
    if (m4c == g_widen_scene_written) {
        ++g_widen_skips;
        return;
    }
    hal_camera_widen_frustum();
    g_widen_scene_written = data_0209f43c[0x4c / 4];
}

/* Camera::SaveCameraStateBeforeTalk is called ARGLESS by both its callers
   (func_02005324 and func_02009a8c, which the community names func_0200cc5c):
   another ARM r0 ride-through -- the camera is already in r0 because it is
   the caller's own argument. The face supplies the singleton, which is what
   r0 held. */
extern void *data_0209f318;
void _ZN6Camera25SaveCameraStateBeforeTalkEv(void)
{
    if (data_0209f318)
        ((Camera *)data_0209f318)->Camera::SaveCameraStateBeforeTalk();
}

int _ZNK7PathPtr5LoopsEv(void *self)
{ return ((PathPtr *)self)->PathPtr::Loops() ? 1 : 0; }

int hal_camera_behavior(void *cam) { return _ZN6Camera8BehaviorEv(cam); }
int hal_camera_render(void *cam) { return _ZN6Camera6RenderEv(cam); }
int hal_camera_init_resources(void *cam)
{ return _ZN6Camera13InitResourcesEv(cam); }
void *hal_camera_new(void) { return _ZN6CameraC1Ev(0); }

}  /* extern "C" */
