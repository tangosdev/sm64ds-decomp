// Gate-4b smoke: the game's whole model pipeline runs on host.
//
// SharedFilePtr loads b_mario_all.bmd through the gate-3b card seam into the
// gate-3a game heap; Model::LoadFile rebases the file image in place
// (int base = (int)&file -- the reason this build is 32-bit); LoadTexAndPal
// pushes textures and palettes through GX::LoadTex into the mapped VRAM
// windows; DoSetFile builds ModelComponents and the transforms buffer; and
// Model::Render walks bones and materials, binding TEXIMAGE_PARAM/PLTT_BASE
// and pumping display lists through the gate-4a machinery. The ntr GX
// decodes the same VRAM the game just filled.
//
// Method calls are QUALIFIED (ptr->Model::Render) on purpose: the game's C
// constructor installs the Itanium-ordered vtable (D1+D0 in slots 0/1), so
// MSVC virtual dispatch would land one slot off. Nothing here dispatches
// virtually, and the HAL vtable slots trap if something ever does.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Model.h"

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"

extern "C" {
void _ZN5ModelC1Ev(void *self);
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void *_ZN4Heap13SetupRootHeapEv(void);
extern Matrix4x3 data_0209b3ec;     /* camera matrix Model::Render composes */
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static void reset_scene()
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;   /* projection = identity */
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;   /* position = identity */
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) | (191u << 24);
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static LONG WINAPI fault(EXCEPTION_POINTERS *ep)
{
    char *base = (char *)GetModuleHandleA(0);
    fprintf(stderr, "FAULT code %08lx at +0x%08x accessing %08x\n",
            ep->ExceptionRecord->ExceptionCode,
            (unsigned)((char *)ep->ExceptionRecord->ExceptionAddress - base),
            (unsigned)(ep->ExceptionRecord->NumberParameters > 1
                       ? ep->ExceptionRecord->ExceptionInformation[1] : 0));
    void *frames[12];
    unsigned n = CaptureStackBackTrace(0, 12, frames, 0);
    for (unsigned i = 0; i < n; ++i)
        fprintf(stderr, "  frame %u: +0x%08x\n", i,
                (unsigned)((char *)frames[i] - base));
    fflush(stderr);
    return EXCEPTION_EXECUTE_HANDLER;
}

int main(void)
{
    SetUnhandledExceptionFilter(fault);
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);

    /* identity camera: Render multiplies the model matrix against it */
    memset(&data_0209b3ec, 0, sizeof data_0209b3ec);
    ((int *)&data_0209b3ec)[0] = 0x1000;
    ((int *)&data_0209b3ec)[4] = 0x1000;
    ((int *)&data_0209b3ec)[8] = 0x1000;

    /* the game's own load chain, handle 1214 = data/player/b_mario_all.bmd.
       SM64DS_MODEL_HANDLE renders any other catalog model instead (viewer
       mode): the Mario-specific expectations only apply to Mario. */
    const char *hs = getenv("SM64DS_MODEL_HANDLE");
    const u32 handle = hs ? (u32)atoi(hs) : 1214;
    const int is_mario = handle == 1214;
    SharedFilePtrC ptr;
    _ZN13SharedFilePtr9ConstructEj(&ptr, handle);
    CHECK(!is_mario || ptr.fileID == 784);

    static char storage[0x50];
    Model *model = (Model *)storage;
    _ZN5ModelC1Ev(storage);

    void *file = Model::LoadFile(*(SharedFilePtr *)&ptr);
    CHECK(file != NULL);
    CHECK(ptr.numRefs == 1);
    CHECK(model->Model::DoSetFile((char *)file, 0, -1) == 1);

    /* identity model matrix, then the game renders */
    memset(&model->mat4x3, 0, sizeof model->mat4x3);
    ((int *)&model->mat4x3)[0] = 0x1000;
    ((int *)&model->mat4x3)[4] = 0x1000;
    ((int *)&model->mat4x3)[8] = 0x1000;

    reset_scene();
    model->Model::Render(NULL);

    size_t tris = 0;
    const ntr::GxTriangle *tarr = ntr::gx_polygons(tris);
    printf("  triangles from the game's render walk: %zu\n", tris);
    CHECK(!is_mario || tris == 492);   /* the gate-4a reference count */
    CHECK(tris > 0);

    /* fit pass: same viewer-side projection trick as gate 4a, so the
       artifact is a whole mario instead of one screen-filling triangle */
    float minx = 1e30f, maxx = -1e30f, miny = 1e30f, maxy = -1e30f;
    for (size_t i = 0; i < tris; ++i)
        for (int v = 0; v < 3; ++v) {
            minx = fminf(minx, tarr[i].v[v].x); maxx = fmaxf(maxx, tarr[i].v[v].x);
            miny = fminf(miny, tarr[i].v[v].y); maxy = fmaxf(maxy, tarr[i].v[v].y);
        }
    {
        const float cx0 = 2.0f * minx / ntr::SCREEN_W - 1.0f;
        const float cx1 = 2.0f * maxx / ntr::SCREEN_W - 1.0f;
        const float cy0 = 1.0f - 2.0f * maxy / ntr::SCREEN_H;
        const float cy1 = 1.0f - 2.0f * miny / ntr::SCREEN_H;
        const float w = fmaxf(cx1 - cx0, 1e-6f), h = fmaxf(cy1 - cy0, 1e-6f);
        const float s = fminf(1.6f / w, 1.6f / h);
        const float tx = -0.5f * (cx0 + cx1) * s, ty = -0.5f * (cy0 + cy1) * s;
        const int32_t m[16] = {(int32_t)(s * 4096), 0, 0, 0,
                               0, (int32_t)(s * 4096), 0, 0,
                               0, 0, 4096, 0,
                               (int32_t)(tx * 4096), (int32_t)(ty * 4096), 0, 4096};
        reset_scene();
        NTR_MMIO(uint32_t, 0x04000440) = 0;
        for (int i = 0; i < 16; ++i)
            NTR_MMIO(uint32_t, 0x04000458) = (uint32_t)m[i];
        NTR_MMIO(uint32_t, 0x04000440) = 1;
        NTR_MMIO(uint32_t, 0x04000454) = 0;
        model->Model::Render(NULL);
        ntr::gx_polygons(tris);
        CHECK(!is_mario || tris == 492);
    }

    ntr::Framebuffer fb;
    for (int y = 0; y < ntr::SCREEN_H; ++y)
        for (int x = 0; x < ntr::SCREEN_W; ++x) fb.px[y][x] = 0xFF101820u;
    ntr::gx_render(fb);
    int drawn = 0;
    for (int y = 0; y < ntr::SCREEN_H; ++y)
        for (int x = 0; x < ntr::SCREEN_W; ++x)
            if (fb.px[y][x] != 0xFF101820u) ++drawn;
    printf("  pixels covered: %d (%.1f%%)\n", drawn,
           100.0 * drawn / (ntr::SCREEN_W * ntr::SCREEN_H));
    CHECK(drawn > 500);
    ntr::ppu_write_bmp("smoke_model.bmp", fb);

    if (g_failures) {
        fprintf(stderr, "smoke_model: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_model: all checks passed (the game loaded, rebased, "
           "uploaded and rendered its own model: %zu tris, %d px)\n",
           tris, drawn);
    return 0;
}
