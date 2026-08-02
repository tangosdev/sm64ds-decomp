// Gate-4a smoke: the game's display-list submission machinery drives the GX.
//
// Opie's test_bmd proved the ntr layer renders b_mario_all.bmd when the
// harness pushes the display-list words into the FIFO itself. This smoke
// replaces exactly that one step with the GAME's machinery: each list is
// handed to func_0205a358, whose chunk pump (max 0x1d8 bytes per DMA)
// spin-waits on GXSTAT, registers the FIFO-half-empty IRQ handler, DMAs
// GXFIFO-timed chunks and finishes through the DMA-channel callback chain
// (func_0205a290 -> func_02056e98 -> func_0205a21c). If the pump delivers
// the same words, the assembled triangles must be BYTE-IDENTICAL to the
// harness pass -- that is the check, not a lookalike threshold.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "ntr/bmd.h"
#include "ntr/fs.h"
#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"

extern "C" {
void func_0205a358(int ch, int src, int size, void (*cb)(int), int arg);
void func_02055780(void);           /* the game's FIFO flush + busy wait */
extern int data_020a6460[8];        /* [0] = pump busy flag */
}

static int g_failures;
#define CHECK(cond) \
    do { if (!(cond)) { ++g_failures; \
        fprintf(stderr, "FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } } while (0)

static void fifo_port(uint32_t a, uint32_t v) { NTR_MMIO(uint32_t, a) = v; }
static int32_t fx(float f) { return (int32_t)(f * 4096.0f); }

static void reset_scene()
{
    ntr::gx_reset();
    fifo_port(0x04000440, 0);       /* MTX_MODE projection */
    fifo_port(0x04000454, 0);       /* MTX_IDENTITY */
    fifo_port(0x04000440, 1);       /* MTX_MODE position */
    fifo_port(0x04000454, 0);
    fifo_port(0x04000580, 0u | (0u << 8) | (255u << 16) | (191u << 24));
    ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
    ntr::gx_enable_lights(0x1);
}

struct Bounds { float minx, maxx, miny, maxy; bool valid; };

static Bounds measure()
{
    size_t n = 0;
    const ntr::GxTriangle *t = ntr::gx_polygons(n);
    Bounds b{1e30f, -1e30f, 1e30f, -1e30f, false};
    for (size_t i = 0; i < n; ++i)
        for (int v = 0; v < 3; ++v) {
            b.minx = fminf(b.minx, t[i].v[v].x);
            b.maxx = fmaxf(b.maxx, t[i].v[v].x);
            b.miny = fminf(b.miny, t[i].v[v].y);
            b.maxy = fmaxf(b.maxy, t[i].v[v].y);
            b.valid = true;
        }
    return b;
}

// Same fit as the reference harness: screen bounds -> a projection that
// centres the model. Row-vector convention, translation in row 3.
static void apply_fit(const Bounds &b)
{
    const float cx0 = 2.0f * b.minx / ntr::SCREEN_W - 1.0f;
    const float cx1 = 2.0f * b.maxx / ntr::SCREEN_W - 1.0f;
    const float cy0 = 1.0f - 2.0f * b.maxy / ntr::SCREEN_H;
    const float cy1 = 1.0f - 2.0f * b.miny / ntr::SCREEN_H;
    const float w = fmaxf(cx1 - cx0, 1e-6f), h = fmaxf(cy1 - cy0, 1e-6f);
    const float s = fminf(1.6f / w, 1.6f / h);
    const float tx = -0.5f * (cx0 + cx1) * s, ty = -0.5f * (cy0 + cy1) * s;
    const int32_t m[16] = {fx(s), 0,     0,    0,
                           0,     fx(s), 0,    0,
                           0,     0,     4096, 0,
                           fx(tx), fx(ty), 0,  4096};
    fifo_port(0x04000440, 0);
    for (int i = 0; i < 16; ++i) fifo_port(0x04000458, (uint32_t)m[i]);
    fifo_port(0x04000440, 1);
    fifo_port(0x04000454, 0);
}

// Submit through the game's pump instead of the harness word loop. The bone
// matrix-slot preload is scene setup (the game's UpdateBones does it before
// rendering), so it stays on the harness side; only the word delivery moves.
static void submit_via_game(const ntr::BmdModel &m)
{
    static const std::vector<uint8_t> kNoAnim;
    std::vector<std::array<float, 16>> bones;
    ntr::bmd_bone_matrices(m, kNoAnim, bones);
    for (size_t i = 0; i < bones.size() && i < 32; ++i)
        ntr::gx_set_matrix_slot((int)i, bones[i].data());

    for (size_t di = 0; di < m.dlists.size(); ++di) {
        const uint8_t mid = di < m.material_ids.size() ? m.material_ids[di] : 0;
        if (mid < m.materials.size())
            ntr::gx_set_material(m.materials[mid].dif_amb, m.materials[mid].spe_emi);
        if (mid < m.textures.size() && !m.textures[mid].rgba.empty())
            ntr::gx_bind_texture(m.textures[mid].rgba.data(),
                                 m.textures[mid].width, m.textures[mid].height);
        else
            ntr::gx_bind_texture(nullptr, 0, 0);

        const ntr::BmdModel::DisplayList &dl = m.dlists[di];
        func_0205a358(0, (int)(m.data.data() + dl.offset), (int)dl.size, 0, 0);
        CHECK(data_020a6460[0] == 0);   /* the pump drained and cleared busy */
    }
    ntr::gx_bind_texture(nullptr, 0, 0);
    func_02055780();                    /* the game's FIFO flush (128 NOPs) */
}

int main(void)
{
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }

    char root[512];
    snprintf(root, sizeof root, "%s/extracted/dsd/files",
             getenv("SM64DS_ASSET_ROOT") ? getenv("SM64DS_ASSET_ROOT")
                                         : PORT_REPO_ROOT);
    if (!ntr::fs_mount_dir(root)) {
        fprintf(stderr, "no assets at %s\n", root);
        return 2;
    }

    ntr::BmdModel m;
    CHECK(ntr::bmd_load_path("data/player/b_mario_all.bmd", m));
    CHECK(!m.dlists.empty());

    /* reference pass: the proven harness word loop, identity projection */
    reset_scene();
    ntr::bmd_submit(m);
    size_t ref_n = 0;
    const ntr::GxTriangle *ref_t = ntr::gx_polygons(ref_n);
    CHECK(ref_n == 492);                /* the count Opie's run verified */
    std::vector<ntr::GxTriangle> ref(ref_t, ref_t + ref_n);
    const Bounds b = measure();
    CHECK(b.valid);

    /* game pass: same words, but the game's pump carries them */
    reset_scene();
    submit_via_game(m);
    size_t game_n = 0;
    const ntr::GxTriangle *game_t = ntr::gx_polygons(game_n);
    printf("  triangles: harness %zu, game pump %zu\n", ref_n, game_n);
    CHECK(game_n == ref_n);
    CHECK(game_n == ref_n &&
          memcmp(ref.data(), game_t, game_n * sizeof(ntr::GxTriangle)) == 0);

    /* fitted pass through the game pump, rastered, artifact kept */
    reset_scene();
    apply_fit(b);
    submit_via_game(m);
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
    ntr::ppu_write_bmp("smoke_gx.bmp", fb);

    if (g_failures) {
        fprintf(stderr, "smoke_gx: %d FAILURE(S)\n", g_failures);
        return 1;
    }
    printf("smoke_gx: all checks passed (game DL pump byte-equals the "
           "harness path, %zu triangles, %d px drawn)\n", game_n, drawn);
    return 0;
}
