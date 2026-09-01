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
