// Gate-4c smoke: the game animates its own model on host.
//
// The piano (data/enemy/piano) is the model whose bone records are NOT in
// bone-index order (boneIDs [1,2,0]) -- the case that broke the research
// harness's linear-pass parser (port/ntr/README.md). The game's own
// UpdateBones walks the hierarchy RECURSIVELY through the child/sibling
// links (func_020453c0), so record order never matters. Rendering the
// piano posed by piano_attack.bca through the game's walk is both the
// animation gate and the definitive answer to that open bug.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Animation.h"

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"

extern "C" {
void _ZN5ModelC1Ev(void *self);
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void *_ZN13SharedFilePtr8LoadFileEv(SharedFilePtrC *self);
void *_ZN4Heap13SetupRootHeapEv(void);
void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void *components, void *bca, int frame);
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *components);
extern Matrix4x3 data_0209b3ec;
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

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

#include "fault_probe.h"

int main(void)
{
    PORT_INSTALL_FAULT_PROBE();
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    CHECK(_ZN4Heap13SetupRootHeapEv() != NULL);
    ident_fx(&data_0209b3ec);

    /* piano model, handle 1034; its attack animation, handle 1036 */
    SharedFilePtrC mp;
    _ZN13SharedFilePtr9ConstructEj(&mp, 1034);
    static char storage[0x50];
    Model *model = (Model *)storage;
    _ZN5ModelC1Ev(storage);
    void *file = Model::LoadFile(*(SharedFilePtr *)&mp);
    CHECK(file != NULL);
    CHECK(model->Model::DoSetFile((char *)file, 0, -1) == 1);
    ident_fx(&model->mat4x3);

    SharedFilePtrC ap;
    _ZN13SharedFilePtr9ConstructEj(&ap, 1036);
    /* the game's loader: SharedFilePtr load + the BCA header rebase */
    void *bca = Animation::LoadFile(*(SharedFilePtr *)&ap);
    CHECK(bca != NULL);

    /* the game's per-frame order: pose bones, re-skin, render */
    const int frame = getenv("SM64DS_ANIM_FRAME") ? atoi(getenv("SM64DS_ANIM_FRAME")) : 0;
    _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&model->data, bca, frame);
    if (getenv("SM64DS_DUMP_BONES")) {
        unsigned char *bones = *((unsigned char **)&model->data + 2);
        for (int b = 0; b < 3; ++b) {
            printf("bone %d:", b);
            for (int k = 0; k < 0x34; ++k) printf("%02x", bones[b * 0x34 + k]);
            printf("\n");
        }
    }
    _ZN15ModelComponents21UpdateVertsUsingBonesEv(&model->data);
    if (getenv("SM64DS_DUMP_BONES")) {
        unsigned char *tf = *((unsigned char **)&model->data + 3);
        for (int b = 0; b < 4; ++b) {
            printf("xform %d:", b);
            for (int k = 0; k < 48; ++k) printf("%02x", tf[b * 48 + k]);
            printf("\n");
        }
    }

    reset_scene();
    model->Model::Render(NULL);
    size_t tris = 0;
    const ntr::GxTriangle *tarr = ntr::gx_polygons(tris);
    printf("  piano triangles, frame 0 of piano_attack: %zu\n", tris);
    if (getenv("SM64DS_DUMP_BONES")) {
        unsigned h = 2166136261u;
        const unsigned char *bytes = (const unsigned char *)tarr;
        for (size_t i = 0; i < tris * sizeof(ntr::GxTriangle); ++i)
            h = (h ^ bytes[i]) * 16777619u;
        printf("trihash %08x over %zu\n", h, tris);
        /* second pose in the SAME process: does the render still track it? */
        for (int f2 = 0; f2 < 3; ++f2) {
            int fr = f2 * 7;
            _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&model->data, bca, fr);
            _ZN15ModelComponents21UpdateVertsUsingBonesEv(&model->data);
            {
                unsigned hr = 2166136261u, ht = 2166136261u;
                unsigned char *rec = *((unsigned char **)&model->data + 2);
                unsigned char *tf = *((unsigned char **)&model->data + 3);
                for (int i = 0; i < 3 * 0x34; ++i) hr = (hr ^ rec[i]) * 16777619u;
                for (int i = 0; i < 4 * 48; ++i) ht = (ht ^ tf[i]) * 16777619u;
                printf("repose frame %2d: rec %08x xf %08x  ", fr, hr, ht);
            }
            reset_scene();
            ntr::gx_stream_hash_reset();
            model->Model::Render(NULL);
            printf("stream %08x state %08x stores %d  ", ntr::gx_stream_hash_reset(),
                   ntr::gx_state_hash(), ntr::gx_store_count_reset());
            size_t n2 = 0;
            const ntr::GxTriangle *t2 = ntr::gx_polygons(n2);
            unsigned h2 = 2166136261u;
            const unsigned char *b2 = (const unsigned char *)t2;
            for (size_t i = 0; i < n2 * sizeof(ntr::GxTriangle); ++i)
                h2 = (h2 ^ b2[i]) * 16777619u;
            printf("repose frame %2d: trihash %08x over %zu\n", fr, h2, n2);
        }
    }
    CHECK(tris > 0);

    /* fit and raster, artifact kept for eyeballing the pose */
    float minx = 1e30f, maxx = -1e30f, miny = 1e30f, maxy = -1e30f;
    for (size_t i = 0; i < tris; ++i)
        for (int v = 0; v < 3; ++v) {
            minx = fminf(minx, tarr[i].v[v].x); maxx = fmaxf(maxx, tarr[i].v[v].x);
            miny = fminf(miny, tarr[i].v[v].y); maxy = fmaxf(maxy, tarr[i].v[v].y);
        }
    CHECK(std::isfinite(minx) && std::isfinite(maxy));
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
    ntr::ppu_write_bmp("smoke_anim.bmp", fb);

    if (g_failures) {
        fprintf(stderr, "smoke_anim: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_anim: all checks passed (the game posed and rendered the "
           "piano via its own recursive bone walk: %zu tris, %d px)\n",
           tris, drawn);
    return 0;
}
