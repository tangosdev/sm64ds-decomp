// Gate-7 smoke: the game owns frame progression.
//
// Gates 4c/5 posed the piano by handing UpdateBones a loop counter; here
// ModelAnim runs the show: SetAnim installs the animation with the game's
// flag/speed packing, Animation::Advance steps currFrame each fiber frame
// (fx12 speed, loop-mode modulo), and ModelAnim::UpdateVerts converts the
// fx frame to the sampler index exactly as every actor does. The wrap
// check is the point: after numFrames advances at speed 1.0 the game's
// modulo lands back on frame 0, and the pose there must hash equal to the
// first frame's pose -- loop closure through the game's own arithmetic.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Animation.h"
#include "ModelAnim.h"
#include "BlendModelAnim.h"

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/rt.h"

#include "fault_probe.h"

// THE BRING-UP ENTRY POINT MOVED, and this harness follows it (lane SMOKELINK3,
// run link100 wave 10 round 2, continuing SMOKELINK2's rule for
// port/tests/smoke_heap.cpp). It used to declare the entry as a flat extern "C"
// Itanium name returning an opaque pointer, which is what the ROM's own callers
// spell and what src/ emitted before the 09-14 sync. It is Heap::SetupRootHeap()
// now, a real static member (include/Heap.h:236, ?SetupRootHeap@Heap@@SAPAU1@XZ),
// so the flat spelling bought a link error. This file is HOST TEST CODE, not ROM
// code, so it may include the class header and call the member the way C++ calls
// it, which is also what the synced src/ TUs now do. It reaches the same object
// it always linked, src/_ZN4Heap13SetupRootHeapEv.cpp; only the spelling changed.
#include "types.h"
#include "Heap.h"

extern "C" {
void *_ZN9ModelAnimC1Ev(void *self);
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca,
                                                 int flags, int speed, u16 start);
extern Matrix4x3 data_0209b3ec;

/* gate 24: BlendModelAnim. SetAnim and the ctor/dtor stay mangled free
   functions (wall 6az / the C destructor TUs); everything else is a method. */
void *_ZN14BlendModelAnimC1Ev(void *self);
void *_ZN14BlendModelAnimD1Ev(void *self);
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *bca,
        int numBlendFrames, int flags, int speed, u16 start);
void hal_fill_blendmodelanim_vtable(void);
}

enum { NUM_FRAMES = 15 };   /* piano_attack.bca */
enum { BLEND_FRAMES = 8 };  /* enough Advances to ramp a 4-frame blend full */

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static ModelAnim *g_ma;
static unsigned g_hash[NUM_FRAMES + 1];
static int g_frames_run;

static void ident_fx(void *m)
{
    memset(m, 0, 48);
    ((int *)m)[0] = ((int *)m)[4] = ((int *)m)[8] = 0x1000;
}

static void reset_scene()
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) | (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
}

static unsigned tri_hash()
{
    size_t n = 0;
    const ntr::GxTriangle *t = ntr::gx_polygons(n);
    unsigned h = 2166136261u;
    const unsigned char *b = (const unsigned char *)t;
    for (size_t i = 0; i < n * sizeof(ntr::GxTriangle); ++i)
        h = (h ^ b[i]) * 16777619u;
    return n ? h : 0;
}

// The game-shaped loop: the actor pattern verbatim -- advance the
// animation, re-skin, render, block at VBlank.
static void game_main()
{
    for (int i = 0; i <= NUM_FRAMES; ++i) {
        g_ma->ModelAnim::UpdateVerts();
        reset_scene();
        g_ma->Model::Render(NULL);
        g_hash[i] = tri_hash();
        ++g_frames_run;
        g_ma->Animation::Advance();
        ntr::rt_vblank_wait();
    }
}

static bool frame_hook(uint64_t) { return true; }

/* ---- gate 24: BlendModelAnim -------------------------------------------
   Same game-shaped loop, but every dispatch goes THROUGH _ZTV14BlendModelAnim
   rather than being qualified: Render is a virtual call, and the Render body
   calls UpdateVerts virtually in turn. That is the whole point -- it is the
   only thing that proves hal/blend_vtable.cpp filled the right slots, since a
   wrong slot here is the c0000005 the ModelAnim family already produced once.
   While blendWeight is under 1.0 UpdateVerts routes the pose through the
   blending func_0204531c instead of UpdateVertsUsingBones, so this exercises
   both arms. */
static BlendModelAnim *g_bma;
static unsigned g_bhash[BLEND_FRAMES + 1];
static int g_bweight[BLEND_FRAMES + 1];
static int g_bframes;

static void blend_main()
{
    for (int i = 0; i <= BLEND_FRAMES; ++i) {
        g_bweight[i] = (int)g_bma->blendWeight;
        reset_scene();
        g_bma->Render(NULL);          /* virtual: MSVC slot 4 */
        g_bhash[i] = tri_hash();
        ++g_bframes;
        g_bma->Advance();
        ntr::rt_vblank_wait();
    }
}

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(Heap::SetupRootHeap() != NULL);
    ident_fx(&data_0209b3ec);

    SharedFilePtrC mp;
    _ZN13SharedFilePtr9ConstructEj(&mp, 1034);          /* piano.bmd */
    static char storage[0x70];
    g_ma = (ModelAnim *)storage;
    _ZN9ModelAnimC1Ev(storage);
    void *file = Model::LoadFile(*(SharedFilePtr *)&mp);
    CHECK(file != NULL);
    CHECK(g_ma->Model::DoSetFile((char *)file, 0, -1) == 1);
    ident_fx(&g_ma->mat4x3);

    SharedFilePtrC ap;
    _ZN13SharedFilePtr9ConstructEj(&ap, 1036);          /* piano_attack.bca */
    void *bca = Animation::LoadFile(*(SharedFilePtr *)&ap);
    CHECK(bca != NULL);

    /* the game's install: loop mode (flags 0), speed 1.0 (fx12), frame 0 */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(g_ma, bca, 0, 0x1000, 0);
    CHECK(g_ma->currFrame == 0);
    CHECK((g_ma->numFramesAndFlags & ~0xc0000000) == (NUM_FRAMES << 12));

    const uint64_t frames = ntr::rt_run(game_main, frame_hook, NUM_FRAMES + 2);
    printf("  fiber frames: %llu, poses rendered: %d\n",
           (unsigned long long)frames, g_frames_run);
    CHECK(g_frames_run == NUM_FRAMES + 1);

    /* poses vary across the cycle... */
    int distinct = 1;
    for (int i = 1; i < NUM_FRAMES; ++i)
        if (g_hash[i] != g_hash[0]) { distinct = 1; break; }
    int varying = 0;
    for (int i = 1; i < NUM_FRAMES; ++i)
        if (g_hash[i] != g_hash[i - 1]) ++varying;
    printf("  varying transitions: %d/%d\n", varying, NUM_FRAMES - 1);
    CHECK(varying > NUM_FRAMES / 2);
    (void)distinct;

    /* ...and the game's modulo closes the loop: pose[15] == pose[0] */
    printf("  wrap: pose[0] %08x, pose[%d] %08x\n", g_hash[0], NUM_FRAMES,
           g_hash[NUM_FRAMES]);
    CHECK(g_hash[NUM_FRAMES] == g_hash[0]);
    CHECK(g_ma->currFrame == 0x1000);   /* advanced once past the wrap */

    /* ---- gate 24: the cross-fading subclass ---------------------------- */
    hal_fill_blendmodelanim_vtable();
    static char bstorage[0x70];
    g_bma = (BlendModelAnim *)bstorage;
    _ZN14BlendModelAnimC1Ev(bstorage);
    CHECK((int)g_bma->blendWeight == 0x1000);  /* ctor starts fully blended */
    CHECK(g_bma->unk_6c == NULL);

    /* its own file handles: a second SharedFilePtr on the same ids, which
       also means these two Loads come back from the host file cache */
    SharedFilePtrC mp2;
    _ZN13SharedFilePtr9ConstructEj(&mp2, 1034);           /* piano.bmd */
    void *file2 = Model::LoadFile(*(SharedFilePtr *)&mp2);
    CHECK(file2 != NULL);
    CHECK(g_bma->DoSetFile((char *)file2, 0, -1) == 1);   /* virtual: slot 1 */
    /* DoSetFile's tail (func_020165c4) allocates the blend pose buffer */
    CHECK(g_bma->unk_6c != NULL);
    ident_fx(&g_bma->mat4x3);

    SharedFilePtrC ap2;
    _ZN13SharedFilePtr9ConstructEj(&ap2, 1036);           /* piano_attack.bca */
    void *bca2 = Animation::LoadFile(*(SharedFilePtr *)&ap2);
    CHECK(bca2 != NULL);

    /* blend in over 4 frames: weight restarts at 0, step = 1/(4+1) */
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(g_bma, bca2, 4, 0,
                                                       0x1000, 0);
    CHECK((int)g_bma->blendWeight == 0);
    CHECK((int)g_bma->blendStep == 0x1000 / 5);

    const uint64_t bframes = ntr::rt_run(blend_main, frame_hook,
                                         BLEND_FRAMES + 2);
    printf("  blend fiber frames: %llu, poses rendered: %d\n",
           (unsigned long long)bframes, g_bframes);
    CHECK(g_bframes == BLEND_FRAMES + 1);

    /* the weight ramps and then clamps: Advance only adds while under 1.0 */
    printf("  blend weights:");
    for (int i = 0; i <= BLEND_FRAMES; ++i) printf(" %d", g_bweight[i]);
    printf("\n");
    CHECK(g_bweight[0] == 0);
    int ramped = 1, blended = 0, full = 0;
    for (int i = 1; i <= BLEND_FRAMES; ++i) {
        if (g_bweight[i] < g_bweight[i - 1]) ramped = 0;
        if (g_bweight[i - 1] < 0x1000) ++blended;   /* took func_0204531c */
        if (g_bweight[i] >= 0x1000) full = 1;
    }
    CHECK(ramped);                 /* monotonic */
    CHECK(blended >= 5);           /* the blending arm really ran */
    CHECK(full);                   /* and reached full weight */
    CHECK(g_bweight[BLEND_FRAMES] == g_bweight[BLEND_FRAMES - 1]);  /* clamped */

    int bvarying = 0;
    for (int i = 1; i <= BLEND_FRAMES; ++i)
        if (g_bhash[i] != g_bhash[i - 1]) ++bvarying;
    printf("  blend varying transitions: %d/%d\n", bvarying, BLEND_FRAMES);
    CHECK(bvarying > BLEND_FRAMES / 2);

    /* teardown through the real destructor: frees unk_6c, then chains
       ModelAnim -> Model/Animation -> ModelBase */
    _ZN14BlendModelAnimD1Ev(bstorage);

    if (g_failures) {
        fprintf(stderr, "smoke_modelanim: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_modelanim: all checks passed (the game advanced, wrapped "
           "and re-posed its own animation; BlendModelAnim cross-faded and "
           "tore down through its own vtable)\n");
    return 0;
}
