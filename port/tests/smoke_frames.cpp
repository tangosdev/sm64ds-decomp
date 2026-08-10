// Gate-5 smoke: the game's frame shape runs on the fiber runtime.
//
// A DS game is a loop that blocks in VBlankIntrWait, and the port keeps
// that shape: the loop below runs on its own fiber (ntr::rt_run), poses and
// renders through the game's code each frame, and yields at VBlank exactly
// where the real main loop does. The host fiber rasters each completed
// frame to a BMP; assembled they are the Mad Piano playing its attack --
// the first MOTION out of the port, through the game's per-frame path.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Animation.h"

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"
#include "ntr/rt.h"

#include "fault_probe.h"

extern "C" {
void _ZN5ModelC1Ev(void *self);
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void *_ZN4Heap13SetupRootHeapEv(void);
void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void *components, void *bca, int frame);
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *components);
extern Matrix4x3 data_0209b3ec;
}

enum { FRAMES = 24 };

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static Model *g_model;
static void *g_bca;
static int32_t g_fit[16];
static size_t g_tris[FRAMES];

static void ident_fx(void *m)
{
    memset(m, 0, 48);
    ((int *)m)[0] = ((int *)m)[4] = ((int *)m)[8] = 0x1000;
}

static void scene_setup()
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    for (int i = 0; i < 16; ++i)
        NTR_MMIO(uint32_t, 0x04000458) = (uint32_t)g_fit[i];
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) | (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
}

// The game-shaped loop: pose, skin, render, block at VBlank. Runs on the
// game fiber; ntr::rt_run owns the frame clock.
static void game_main()
{
    for (int frame = 0; frame < FRAMES; ++frame) {
        _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&g_model->data, g_bca, frame);
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(&g_model->data);
        scene_setup();
        g_model->Model::Render(NULL);
        ntr::rt_vblank_wait();
    }
}

// Host side of the seam: raster what the game submitted this frame.
static bool frame_hook(uint64_t frame)
{
    ntr::Framebuffer fb;
    for (int y = 0; y < ntr::SCREEN_H; ++y)
        for (int x = 0; x < ntr::SCREEN_W; ++x) fb.px[y][x] = 0xFF101820u;
    ntr::gx_render(fb);
    size_t n = 0;
    const ntr::GxTriangle *t = ntr::gx_polygons(n);
    if (frame >= 1 && frame <= FRAMES)
        g_tris[frame - 1] = n;      /* the hook numbers frames from 1 */
    unsigned h = 2166136261u;
    const unsigned char *bytes = (const unsigned char *)t;
    for (size_t i = 0; i < n * sizeof(ntr::GxTriangle); ++i)
        h = (h ^ bytes[i]) * 16777619u;
    printf("  hook frame %02u: %zu tris, hash %08x\n", (unsigned)frame, n, h);
    char name[64];
    snprintf(name, sizeof name, "frames/frame_%02u.bmp", (unsigned)frame);
    ntr::ppu_write_bmp(name, fb);
    return true;
}

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);
    ident_fx(&data_0209b3ec);

    SharedFilePtrC mp;
    _ZN13SharedFilePtr9ConstructEj(&mp, 1034);          /* piano.bmd */
    static char storage[0x50];
    g_model = (Model *)storage;
    _ZN5ModelC1Ev(storage);
    void *file = Model::LoadFile(*(SharedFilePtr *)&mp);
    CHECK(file != NULL);
    CHECK(g_model->Model::DoSetFile((char *)file, 0, -1) == 1);
    ident_fx(&g_model->mat4x3);

    SharedFilePtrC ap;
    _ZN13SharedFilePtr9ConstructEj(&ap, 1036);          /* piano_attack.bca */
    g_bca = Animation::LoadFile(*(SharedFilePtr *)&ap);
    CHECK(g_bca != NULL);

    /* one throwaway pose to derive a projection held constant across
       frames, so the motion is visible instead of re-centered away */
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&g_model->data, g_bca, 0);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(&g_model->data);
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    g_model->Model::Render(NULL);
    size_t n = 0;
    const ntr::GxTriangle *t = ntr::gx_polygons(n);
    float minx = 1e30f, maxx = -1e30f, miny = 1e30f, maxy = -1e30f;
    for (size_t i = 0; i < n; ++i)
        for (int v = 0; v < 3; ++v) {
            minx = fminf(minx, t[i].v[v].x); maxx = fmaxf(maxx, t[i].v[v].x);
            miny = fminf(miny, t[i].v[v].y); maxy = fmaxf(maxy, t[i].v[v].y);
        }
    {
        const float cx0 = 2.0f * minx / ntr::SCREEN_W - 1.0f;
        const float cx1 = 2.0f * maxx / ntr::SCREEN_W - 1.0f;
        const float cy0 = 1.0f - 2.0f * maxy / ntr::SCREEN_H;
        const float cy1 = 1.0f - 2.0f * miny / ntr::SCREEN_H;
        const float w = fmaxf(cx1 - cx0, 1e-6f), h = fmaxf(cy1 - cy0, 1e-6f);
        const float s = 0.6f * fminf(1.6f / w, 1.6f / h);  /* headroom to move */
        const float tx = -0.5f * (cx0 + cx1) * s, ty = -0.5f * (cy0 + cy1) * s;
        const int32_t m[16] = {(int32_t)(s * 4096), 0, 0, 0,
                               0, (int32_t)(s * 4096), 0, 0,
                               0, 0, 4096, 0,
                               (int32_t)(tx * 4096), (int32_t)(ty * 4096), 0, 4096};
        memcpy(g_fit, m, sizeof m);
    }

#ifdef _WIN32
    CreateDirectoryA("frames", NULL);
#else
    { int r = system("mkdir -p frames"); (void)r; }
#endif
    const uint64_t frames = ntr::rt_run(game_main, frame_hook, FRAMES);
    printf("  frames completed through the fiber loop: %llu\n",
           (unsigned long long)frames);
    CHECK(frames == FRAMES);
    int moving = 0;
    for (int i = 1; i < FRAMES; ++i)
        if (g_tris[i] > 0 && g_tris[i - 1] > 0) ++moving;
    CHECK(moving == FRAMES - 1);    /* every frame drew geometry */

    if (g_failures) {
        fprintf(stderr, "smoke_frames: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_frames: all checks passed (%d game-shaped frames through "
           "the fiber runtime, all with geometry)\n", FRAMES);
    return 0;
}
