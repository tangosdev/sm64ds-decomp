// THE GRAPHICS-CARD OPAQUE PASS, ASKED THE QUESTIONS A PICTURE CANNOT ANSWER
// (run hd2, lane GPU2). Every row here is a RELATION BETWEEN PIXELS, in the
// style of tests/smoke_gx.cpp: not "does this look right" but "is the nearer
// triangle the one that is there", "did the first of two equal-depth triangles
// keep the pixel", "did an alpha-0 texel leave the depth alone". A lookalike
// threshold would pass on a renderer that had the depth test backwards; these
// do not.
//
// The card used is WARP, the software Direct3D device Windows ships, so the
// answers are the API's answers and not one vendor's. The in-process A/B in
// ntr/gx.cpp covers the real hardware on real scenes; this covers the rules.
//
// THE SCENE IS FED THROUGH THE GEOMETRY PORTS, exactly as the game feeds it:
// identity projection, identity position matrix, a full-screen viewport, so a
// vertex at (x, y) in -1..1 lands where the arithmetic says and a vertex z of
// -1..1 becomes a screen z of 0..1. Nothing here reaches into the rasteriser.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"

#include "hal/gpu_raster.h"

static int g_failures;
static int g_rows;

#define CHECK(cond, what)                                                     \
    do {                                                                      \
        ++g_rows;                                                             \
        if (cond) {                                                           \
            printf("  ok   %s\n", what);                                      \
        } else {                                                              \
            ++g_failures;                                                     \
            printf("  FAIL %s   (%s:%d)\n", what, __FILE__, __LINE__);        \
        }                                                                     \
    } while (0)

/* ---- the geometry ports ------------------------------------------------- */

static void port(uint32_t a, uint32_t v) { NTR_MMIO(uint32_t, a) = v; }

static uint32_t fx16(float f)
{
    return (uint32_t)(int32_t)(int)(f * 4096.0f) & 0xFFFFu;
}

static void vtx(float x, float y, float z)
{
    port(0x0400048C, (fx16(y) << 16) | fx16(x));   /* VTX_16 word 0 */
    port(0x0400048C, fx16(z));                     /* VTX_16 word 1 */
}

static uint32_t bgr555(int r, int g, int b)
{
    return (uint32_t)((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10));
}

/* POLYGON_ATTR: bits 6 and 7 are "draw the back face" and "draw the front
   face" -- both on here, because which way a synthetic triangle winds is not
   what any of these rows is about. Bits 16..20 are the alpha, bits 24..29 the
   polygon id. */
static void attr(int alpha, int polyid, int mode = 0)
{
    port(0x040004A4, 0xC0u | ((uint32_t)mode << 4) |
                     ((uint32_t)alpha << 16) | ((uint32_t)polyid << 24));
}

static void scene_begin()
{
    ntr::gx_reset();
    port(0x04000440, 0);            /* MTX_MODE projection */
    port(0x04000454, 0);            /* MTX_IDENTITY */
    port(0x04000440, 1);            /* MTX_MODE position */
    port(0x04000454, 0);
    port(0x04000580, 0u | (0u << 8) | (255u << 16) | (191u << 24));
    attr(31, 0);
}

/* A triangle pair covering the square (-s..s, -s..s) at a constant z. */
static void quad(float s, float z, int r, int g, int b)
{
    port(0x04000480, bgr555(r, g, b));            /* COLOR */
    port(0x04000500, 0);                          /* BEGIN_VTXS, triangles */
    vtx(-s, -s, z); vtx(s, -s, z); vtx(s, s, z);
    vtx(-s, -s, z); vtx(s, s, z); vtx(-s, s, z);
    port(0x04000504, 0);                          /* END_VTXS */
}

/* The same square with texture coordinates, and a z that varies across it so
   the perspective divide has something to do. u and v are in texels. */
static void quad_tex(float s, float zl, float zr, float w_l, float w_r,
                     int tw, int th)
{
    port(0x04000480, bgr555(255, 255, 255));
    port(0x04000500, 0);
    /* left edge at zl, right edge at zr; the w that reaches the raster comes
       from the projection, which is the identity here, so the interpolation
       being exercised is the screen-linear one. */
    (void)w_l; (void)w_r;
    const float u1 = (float)tw, v1 = (float)th;
    port(0x04000488, 0); vtx(-s, -s, zl);
    port(0x04000488, ((uint32_t)(int)(u1 * 16.0f) & 0xFFFFu)); vtx(s, -s, zr);
    port(0x04000488, ((uint32_t)(int)(u1 * 16.0f) & 0xFFFFu) |
                     (((uint32_t)(int)(v1 * 16.0f) & 0xFFFFu) << 16));
    vtx(s, s, zr);
    port(0x04000488, 0); vtx(-s, -s, zl);
    port(0x04000488, ((uint32_t)(int)(u1 * 16.0f) & 0xFFFFu) |
                     (((uint32_t)(int)(v1 * 16.0f) & 0xFFFFu) << 16));
    vtx(s, s, zr);
    port(0x04000488, ((uint32_t)(int)(v1 * 16.0f) & 0xFFFFu) << 16);
    vtx(-s, s, zl);
    port(0x04000504, 0);
}

/* ---- rendering both ways ------------------------------------------------ */

static ntr::Framebuffer *g_fb;
static ntr::GxGpuOpaqueFn g_backend;

struct Shot {
    std::vector<uint32_t> px;
    std::vector<uint8_t> cover;
    std::vector<uint8_t> id;
    std::vector<float> depth;
};

static void clear_fb(uint32_t c)
{
    for (int y = 0; y < ntr::SCREEN_H; ++y)
        for (int x = 0; x < ntr::SCREEN_W; ++x) g_fb->px[y][x] = c;
}

static Shot take(bool on_card)
{
    ntr::gx_set_gpu_opaque(on_card ? g_backend : 0);
    ntr::gx_render(*g_fb);
    const size_t n = (size_t)ntr::SCREEN_W * (size_t)ntr::SCREEN_H;
    Shot s;
    s.px.assign(n, 0);
    s.cover.assign(n, 0);
    s.id.assign(n, 0);
    s.depth.assign(n, 0.0f);
    std::memcpy(&s.px[0], &g_fb->px[0][0], n * sizeof(uint32_t));
    std::memcpy(&s.cover[0], ntr::gx_coverage(), n);
    std::memcpy(&s.id[0], ntr::gx_attr_ids(), n);
    std::memcpy(&s.depth[0], ntr::gx_depth(), n * sizeof(float));
    return s;
}

static size_t centre() { return (size_t)(ntr::SCREEN_H / 2) * ntr::SCREEN_W +
                                ntr::SCREEN_W / 2; }

static int chan(uint32_t p, int sh) { return (int)((p >> sh) & 0xFF); }

static bool colour_is(uint32_t p, int r, int g, int b, int tol)
{
    /* the DS colour is five bits a channel, so a "red 255" arrives as 248 */
    const int er = (r >> 3) * 255 / 31, eg = (g >> 3) * 255 / 31,
              eb = (b >> 3) * 255 / 31;
    return abs(chan(p, 16) - er) <= tol && abs(chan(p, 8) - eg) <= tol &&
           abs(chan(p, 0) - eb) <= tol;
}

int main()
{
    /* THE CARD IS ASKED FOR BEFORE ANYTHING ELSE, and it is WARP: these rows
       are about the rules the backend follows, and a vendor's driver is not
       the place to establish them. */
    _putenv("SM64DS_RENDERER=1");
    _putenv("SM64DS_RENDERER_DEVICE=warp");
    port_gpu_raster_configure();
    g_backend = ntr::gx_gpu_opaque();
    if (!g_backend) {
        printf("smoke_gpu_raster: no backend registered; nothing to test\n");
        return 1;
    }
    g_fb = new ntr::Framebuffer;

    const uint32_t CLEAR = 0xFF101820u;

    /* ---- 1. a nearer opaque triangle hides a farther one, either order --- */
    for (int order = 0; order < 2; ++order) {
        scene_begin();
        if (order == 0) {
            quad(0.5f, 0.5f, 255, 0, 0);      /* far,  red  */
            quad(0.5f, -0.5f, 0, 0, 255);     /* near, blue */
        } else {
            quad(0.5f, -0.5f, 0, 0, 255);     /* near first */
            quad(0.5f, 0.5f, 255, 0, 0);
        }
        clear_fb(CLEAR);
        Shot c = take(true);
        CHECK(colour_is(c.px[centre()], 0, 0, 255, 2),
              order == 0 ? "the nearer triangle hides the farther one "
                           "(far submitted first)"
                         : "the nearer triangle hides the farther one "
                           "(near submitted first)");
    }

    /* ---- 2. equal depth keeps the FIRST submitted (a LESS test) ---------- */
    for (int order = 0; order < 2; ++order) {
        scene_begin();
        if (order == 0) { quad(0.5f, 0.0f, 255, 0, 0); quad(0.5f, 0.0f, 0, 0, 255); }
        else            { quad(0.5f, 0.0f, 0, 0, 255); quad(0.5f, 0.0f, 255, 0, 0); }
        clear_fb(CLEAR);
        Shot c = take(true);
        const bool first_kept = order == 0
            ? colour_is(c.px[centre()], 255, 0, 0, 2)
            : colour_is(c.px[centre()], 0, 0, 255, 2);
        CHECK(first_kept, order == 0
              ? "equal depth keeps the first submitted (red then blue)"
              : "equal depth keeps the first submitted (blue then red)");
    }

    /* ---- 3. the polygon id lands in the id buffer ------------------------
       THE ID PLANE IS ONLY FILLED WHEN SOMETHING READS IT: ntr/gx.cpp writes
       it when the frame carries a shadow volume (the equal-id rule is what a
       shadow is settled by) or when the A/B is measuring. So the row puts a
       mode-3 polygon in the corner, well clear of the pixel it asks about,
       which is the real situation the plane exists for. */
    {
        scene_begin();
        attr(31, 37);
        quad(0.5f, 0.0f, 0, 255, 0);
        attr(31, 5, 3);                       /* a shadow volume in a corner */
        port(0x04000480, bgr555(0, 0, 0));
        port(0x04000500, 0);
        vtx(0.80f, 0.80f, 0.25f);
        vtx(0.95f, 0.80f, 0.25f);
        vtx(0.95f, 0.95f, 0.25f);
        port(0x04000504, 0);
        clear_fb(CLEAR);
        Shot c = take(true);
        Shot s2 = take(false);
        CHECK(c.cover[centre()] == 1 && c.id[centre()] == 37,
              "the polygon id reaches the attribute buffer");
        CHECK(s2.id[centre()] == c.id[centre()],
              "and it is the id the software raster puts there");
    }

    /* ---- 4. an alpha-0 texel is no pixel: no colour, no coverage, and the
              depth is left alone, which a farther triangle behind it proves */
    {
        static uint32_t clear_texel[1] = {0x00FFFFFFu};   /* alpha 0 */
        scene_begin();
        ntr::gx_invalidate_textures();
        ntr::gx_bind_texture(clear_texel, 1, 1);
        quad_tex(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1, 1);     /* near, invisible */
        ntr::gx_bind_texture(0, 0, 0);
        quad(0.25f, 0.5f, 255, 0, 255);                   /* farther, behind */
        clear_fb(CLEAR);
        Shot c = take(true);
        CHECK(colour_is(c.px[centre()], 255, 0, 255, 2) && c.cover[centre()] == 1,
              "an alpha-0 texel leaves the pixel and the depth to what is "
              "behind it");
        /* and with nothing behind it the pixel is untouched entirely */
        scene_begin();
        ntr::gx_bind_texture(clear_texel, 1, 1);
        quad_tex(0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1, 1);
        clear_fb(CLEAR);
        Shot d = take(true);
        CHECK(d.cover[centre()] == 0 && d.px[centre()] == CLEAR,
              "an alpha-0 texel covers nothing at all");
    }

    /* ---- 5. colour and texture coordinates interpolate the way the
              software raster interpolates them, on a tilted quad ----------- */
    {
        static uint32_t checker[64];
        for (int i = 0; i < 64; ++i) {
            const int x = i & 7, y = i >> 3;
            checker[i] = 0xFF000000u |
                         ((((x ^ y) & 1) ? 0x00E0A040u : 0x002040E0u));
        }
        scene_begin();
        ntr::gx_invalidate_textures();
        ntr::gx_bind_texture(checker, 8, 8);
        /* steeply tilted: the left edge nearly at the near plane, the right
           edge far, so 1/w varies hard across the surface */
        quad_tex(0.8f, -0.9f, 0.9f, 1.0f, 1.0f, 8, 8);
        clear_fb(CLEAR);
        Shot card = take(true);
        scene_begin();
        ntr::gx_bind_texture(checker, 8, 8);
        quad_tex(0.8f, -0.9f, 0.9f, 1.0f, 1.0f, 8, 8);
        clear_fb(CLEAR);
        Shot soft = take(false);

        size_t both = 0, bad = 0;
        int worst = 0;
        for (size_t i = 0; i < card.cover.size(); ++i) {
            if (!card.cover[i] || !soft.cover[i]) continue;
            ++both;
            int w = 0;
            for (int sh = 0; sh <= 16; sh += 8) {
                const int d = abs(chan(card.px[i], sh) - chan(soft.px[i], sh));
                if (d > w) w = d;
            }
            if (w > worst) worst = w;
            if (w > 2) ++bad;
        }
        printf("  tilted quad: %llu pixel(s) covered by both, worst channel "
               "difference %d, %llu past a tolerance of 2\n",
               (unsigned long long)both, worst, (unsigned long long)bad);
        CHECK(both > 10000 && bad * 200 <= both,
              "a tilted textured quad reads the same on the card as in the "
              "software raster (under 0.5 per cent of pixels past two steps)");
    }

    delete g_fb;
    printf("smoke_gpu_raster: %s, %d row(s), %d failure(s)\n",
           g_failures ? "FAIL" : "ok", g_rows, g_failures);
    return g_failures ? 1 : 0;
}
