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

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/rt.h"

#include "fault_probe.h"

extern "C" {
void *_ZN9ModelAnimC1Ev(void *self);
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void *_ZN4Heap13SetupRootHeapEv(void);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *bca,
                                                 int flags, int speed, u16 start);
extern Matrix4x3 data_0209b3ec;
}

enum { NUM_FRAMES = 15 };   /* piano_attack.bca */

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

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);
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

    if (g_failures) {
        fprintf(stderr, "smoke_modelanim: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_modelanim: all checks passed (the game advanced, wrapped "
           "and re-posed its own animation)\n");
    return 0;
}
