// DS geometry engine: command decode, matrix stacks, vertex assembly, raster.
//
// Command encoding and parameter counts from GBATEK. Fixed-point inputs are 4.12
// (fx32); the transform runs in float, which is what any host renderer would do
// and is not a fidelity question for geometry this size.

#include "ntr/gx.h"

#include "ntr/mmio.h"
#include "ntr/texture.h"

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace ntr {
namespace {

constexpr float FX12 = 1.0f / 4096.0f;

// --- matrices ---------------------------------------------------------------
// DS matrices are row-vector convention: v' = v * M, translation in row 3.
struct Mat {
    float m[16];
    static Mat identity() {
        Mat r{};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }
};

Mat mul(const Mat &a, const Mat &b) {
    Mat r{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) {
            float s = 0;
            for (int k = 0; k < 4; ++k) s += a.m[i * 4 + k] * b.m[k * 4 + j];
            r.m[i * 4 + j] = s;
        }
    return r;
}

struct Vec4 { float x, y, z, w; };

Vec4 mul(const Vec4 &v, const Mat &m) {
    return {v.x * m.m[0] + v.y * m.m[4] + v.z * m.m[8] + v.w * m.m[12],
            v.x * m.m[1] + v.y * m.m[5] + v.z * m.m[9] + v.w * m.m[13],
            v.x * m.m[2] + v.y * m.m[6] + v.z * m.m[10] + v.w * m.m[14],
            v.x * m.m[3] + v.y * m.m[7] + v.z * m.m[11] + v.w * m.m[15]};
}

// --- engine state -----------------------------------------------------------
enum MtxMode { MTX_PROJ = 0, MTX_POS = 1, MTX_POSVEC = 2, MTX_TEX = 3 };

struct State {
    int mode = MTX_POS;
    Mat proj = Mat::identity();
    Mat pos = Mat::identity();
    Mat vec = Mat::identity();
    Mat tex = Mat::identity();

    Mat proj_stack[2];
    Mat pos_stack[32];
    Mat vec_stack[32];
    /* The TEXTURE matrix stack is its own single entry and its own pointer.
       GBATEK gives mode 3 a one-level stack, and GXSTAT's bits 8..12 report
       the POSITION/VECTOR pointer alone -- there is no field for this one.
       It is separate here because it used to not be: MTX_PUSH and MTX_POP in
       mode 3 fell into the position branch, so a texture-matrix bracket moved
       the position stack pointer and saved the position matrix. That was
       invisible while GXSTAT's stack field was a dead latch and stops being
       invisible the moment the field is published, which is what this lane
       does below. SM64DS_MTX_TEXSTACK=0 puts the old behaviour back. */
    Mat tex_stack;
    int proj_sp = 0, pos_sp = 0, tex_sp = 0;

    uint32_t color = 0xFFFFFFFFu;
    float u = 0, v = 0;                 // current TEXCOORD, in texels
    float raw_u = 0, raw_v = 0;         // TEXCOORD as loaded, pre-texgen
    const uint32_t *tex_rgba = nullptr; // bound texture (Mat tex above is the
    int tw = 0, th = 0;                 // texture *matrix* -- different thing)
    uint8_t tex_wrap = 3;               // TEXIMAGE_PARAM bits 16-19, see GxTriangle
    int prim = -1;                 // BEGIN_VTXS type, -1 when not inside a primitive
    uint32_t poly_attr = 0x80;     // POLYGON_ATTR latch; bit6 back, bit7 front
    int16_t vx = 0, vy = 0, vz = 0;
    std::vector<GxVertex> strip;   // vertices accumulated in the current primitive
    int strip_parity = 0;

    int vp_x = 0, vp_y = 0, vp_w = active_w, vp_h = active_h;
    /* How many VIEWPORT commands have executed since the last gx_reset. The
       default above IS a full-screen rectangle, so a sampled viewport of
       0,0 SCREEN_W x SCREEN_H cannot on its own tell a game-issued
       full-screen viewport from a frame that never issued one at all. This
       counter is the difference, and without it the viewport row reports a
       rectangle it cannot attribute. */
    int vp_writes = 0;

    // Lighting state. diffuse/ambient/specular/emission are 0..1 per channel.
    float diffuse[3] = {1, 1, 1}, ambient[3] = {0, 0, 0};
    float emission[3] = {0, 0, 0};
    struct Light { float dx, dy, dz; float r, g, b; } lights[4] = {};
    uint32_t light_mask = 0;

    std::vector<GxTriangle> tris;
};

State g;
int g_store_count;
int g_tex_decodes;            // VRAM texture decodes since the last perf report

/* MTX_PUSH / MTX_POP / MTX_STORE / MTX_RESTORE, counted BY MATRIX MODE.
   Always counted, printed only under SM64DS_MTX_BALANCE. The mode is the whole
   question for the two behaviours below it: whether the texture stack ever
   moves, and whether a bracket the game opens in one mode is closed in
   another. A per-port count cannot answer either -- MTX_PUSH is one port for
   four stacks. */
unsigned g_mtx_push_mode[4], g_mtx_pop_mode[4];
unsigned g_mtx_store_mode[4], g_mtx_restore_mode[4];
/* Attempts refused because the stack was already at its end. GBATEK sets
   GXSTAT bit 15 on these; this counts them so a refusal is a number rather
   than a silent clamp. */
unsigned g_mtx_stack_refusals;

/* SM64DS_GXSTAT_LIVE=0: the geometry engine stops publishing its matrix-stack
   level into the mapped I/O window, which is the state every build before this
   lane was in -- GXSTAT bits 8..14 refreshed only when something happened to
   touch the register THROUGH ntr::io_write, and held a stale snapshot the rest
   of the time. DEFAULT ON, and the OFF arm announces itself: run mg15's SQRT
   lane proved a switch that only prints one arm silently keeps the wrong one.
   Runtime rather than a CMake option for the same reason -- option() caches. */
int gxstat_live() {
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_GXSTAT_LIVE");
        on = (e && *e && *e == '0') ? 0 : 1;
        if (!on)
            fprintf(stderr, "  [gx] SM64DS_GXSTAT_LIVE=0: GXSTAT's matrix-stack "
                    "level is a stale snapshot again\n");
    }
    return on;
}

/* SM64DS_MTX_TEXSTACK=0: mode 3 pushes and pops go back to sharing the
   POSITION stack, which is what this file did before this lane. See the
   tex_stack member. DEFAULT ON, OFF arm announces itself. */
int mtx_texstack() {
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_MTX_TEXSTACK");
        on = (e && *e && *e == '0') ? 0 : 1;
        if (!on)
            fprintf(stderr, "  [gx] SM64DS_MTX_TEXSTACK=0: mode-3 matrix "
                    "brackets move the POSITION stack pointer again\n");
    }
    return on;
}

/* Publish the two stack pointers into GXSTAT, at the moment the geometry
   engine changes them. This is the half io.cpp's own note said was missing:
   gxstat_normalize() runs only on accesses routed through the MMIO proxy, and
   the game's two stack-level readers -- func_02055464 and func_02055490,
   src TUs in port/slice_w1l2.txt -- are PLAIN, so they read the mapped window
   directly and saw whatever the last proxied touch had left there.
   ntr::io_gxstat_publish() is gxstat_normalize() with io.cpp's own init guard
   in front of it, so the register model stays in the file that owns it. */
void gxstat_publish() {
    if (gxstat_live()) io_gxstat_publish();
}

/* SM64DS_GXSTAT_WITNESS=1: does the mapped I/O window AGREE with the geometry
   engine about the stack level, at a moment nothing has just refreshed it?
   DEFAULT OFF.
   THE SAMPLE POINT IS MTX_MULT, deliberately. It is the most common geometry
   command on this path (17,951 in a 300-frame scene 1), it never touches
   GXSTAT, and it is never adjacent to a publish -- so a sample taken there is
   what a PLAIN reader would have got if it had read the register at that
   instant, which is the whole question func_02055464 and func_02055490 ask.
   Sampling at push or pop time would read the value just written and agree in
   both arms, which is the vacuous version of this measurement. */
unsigned g_gxw_agree, g_gxw_disagree, g_gxw_worst_true, g_gxw_worst_read;
int gxstat_witness() {
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_GXSTAT_WITNESS") ? 1 : 0;
    return on;
}
void gxstat_witness_sample() {
    if (!gxstat_witness()) return;
    const uint32_t v = *reinterpret_cast<volatile uint32_t *>(
        static_cast<uintptr_t>(0x04000600u));
    const unsigned read_pos = (v & 0x1f00u) >> 8, read_proj = (v & 0x2000u) >> 13;
    const unsigned true_pos = (unsigned)(g.pos_sp < 0 ? 0 : (g.pos_sp > 31 ? 31 : g.pos_sp));
    const unsigned true_proj = (unsigned)(g.proj_sp < 0 ? 0 : (g.proj_sp > 1 ? 1 : g.proj_sp));
    if (read_pos == true_pos && read_proj == true_proj) { ++g_gxw_agree; return; }
    ++g_gxw_disagree;
    if (true_pos > g_gxw_worst_true) { g_gxw_worst_true = true_pos; g_gxw_worst_read = read_pos; }
}

/* SM64DS_MAT_LOG census storage -- see mat_report() below for what it is for.
   Declared here because exec() feeds it and exec() comes first. */
uint32_t g_matlog_attr[16], g_matlog_difamb[16], g_matlog_speemi[16];
unsigned g_matlog_nattr, g_matlog_ndifamb, g_matlog_nspeemi;
unsigned g_matlog_nlightvec, g_matlog_nlightcol;
/* NORMAL commands executed while each POLYGON_ATTR was latched, plus the
   ones that ran under an attr the table had no room for. This is the row
   that decides whether the hardware's own light-enable bits can be honoured
   without moving a frame: a material with no light bits set and no NORMAL
   under it never reaches the lighting equation at all. */
unsigned g_matlog_attr_normals[16], g_matlog_normals_other;

int mat_log() {
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_MAT_LOG") ? 1 : 0;
    return on;
}

void mat_note(uint32_t *tab, unsigned &n, uint32_t v) {
    for (unsigned i = 0; i < n; ++i) if (tab[i] == v) return;
    if (n < 16) tab[n++] = v;
}

void mat_note_normal(uint32_t attr) {
    for (unsigned i = 0; i < g_matlog_nattr; ++i)
        if (g_matlog_attr[i] == attr) { ++g_matlog_attr_normals[i]; return; }
    ++g_matlog_normals_other;
}
extern uint32_t g_teximage;   // defined with the texture cache below

/* SM64DS_FRAME_MS=1: every 30 frames, the raster's own cost on stderr --
   average time inside gx_render, average wall interval between consecutive
   gx_render calls (which is the whole frame when nothing is pacing it),
   triangles submitted and textures decoded out of VRAM per frame. The
   decode count is the one that says whether the texture cache is working:
   it should settle at 0 once a scene has been seen once. */
int frame_ms() {
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_FRAME_MS") ? 1 : 0;
    return on;
}

uint32_t bgr555_to_argb(uint16_t c) {
    const uint32_t r = c & 0x1F, gg = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
    return 0xFF000000u | ((r << 3 | r >> 2) << 16) | ((gg << 3 | gg >> 2) << 8)
           | (b << 3 | b >> 2);
}

Mat &current_pos() { return g.pos; }

// Transform a model-space vertex to CLIP space. The divide and viewport
// mapping happen at emit time, after near-plane clipping -- dividing by a
// w that is zero or negative (a vertex behind the camera) sprays the
// triangle across the screen, which is exactly what the pre-clip pipeline
// did the moment a perspective camera walked into geometry.
/* SM64DS_STAR_GEO: per-frame census of what the geometry engine was handed
   and what came out. Inert unless the env var is set. Reports the model-space
   and clip-space bounding boxes, the vertex count, and the two matrices in
   force at the frame's FIRST vertex -- which is what tells a scale change
   apart from a camera move apart from a projection change. */
struct StarGeo {
    int on = -1, n = 0, snapped = 0;
    float mnx, mny, mnz, mxx, mxy, mxz;      /* model space  */
    float cnx, cny, cnz, cxx, cxy, cxz;      /* clip space   */
    float cnw, cxw;
    Mat pos0, proj0;
    /* the matrix in force at the vertex that reached furthest in clip x, and
       that vertex's own model-space coordinates: the frame's bbox is a UNION
       over every object, so the extreme vertex is the only one that names
       WHICH object grew. */
    Mat posE; float ex = 0, emx = 0, emy = 0, emz = 0, ew = 0;
    /* every DISTINCT position matrix the frame used, with its vertex count:
       one row per drawn object, which is what separates the sky from the
       star without having to guess which vertex belongs to which. */
    struct Row { float a, b, c, tx, ty, tz; int n; };
    Row tab[32]; int ntab = 0;
    unsigned frame = 0;
};
StarGeo g_stargeo;
int stargeo_on() {
    if (g_stargeo.on < 0) g_stargeo.on = std::getenv("SM64DS_STAR_GEO") ? 1 : 0;
    return g_stargeo.on;
}

GxVertex project(int16_t x, int16_t y, int16_t z) {
    const Vec4 v{x * FX12, y * FX12, z * FX12, 1.0f};
    Vec4 c = mul(mul(v, current_pos()), g.proj);

    /* WIDESCREEN 3D FIELD (16:9 Hor+). The ROM builds its projection with a
       4:3 aspect (G3i::PerspectiveW_ divides the x scale by 0x1555). Presented
       on the 1024x576 framebuffer that 4:3 image would be stretched sideways;
       instead WIDEN THE HORIZONTAL FIELD so the extra width shows more of the
       world and nothing is squashed. This is done at the point of use rather
       than by rewriting g.proj: it is naturally idempotent (no risk of
       double-applying across a load+multiply) and it never touches clip.y --
       Hor+, never a vertical squish. It fires ONLY for a perspective load;
       an ortho / 2D projection carries no vertex-dependent w (m[3]/m[7]/m[11]
       all zero) and is left exactly as it was, so the HUD's own 2D geometry is
       untouched here. clip.x scales by native/target = (4/3) / (active_w/
       active_h): 0.75 at 16:9, and EXACTLY 1.0 at any 4:3 aspect, so with the
       runtime toggle off (active 512x384) this multiply is the identity and the
       4:3 field is byte-for-byte the old one -- no #ifdef needed. */
    if (g.proj.m[3] != 0.0f || g.proj.m[7] != 0.0f || g.proj.m[11] != 0.0f) {
        const float widen =
            (4.0f / 3.0f) * ((float)active_h / (float)active_w);
        c.x *= widen;
    }

    if (stargeo_on()) {
        StarGeo &G = g_stargeo;
        const float mx = x * FX12, my = y * FX12, mz = z * FX12;
        if (!G.n) {
            G.mnx = G.mxx = mx; G.mny = G.mxy = my; G.mnz = G.mxz = mz;
            G.cnx = G.cxx = c.x; G.cny = G.cxy = c.y; G.cnz = G.cxz = c.z;
            G.cnw = G.cxw = c.w;
            G.pos0 = current_pos(); G.proj0 = g.proj; G.snapped = 1;
            G.ex = c.x; G.ew = c.w; G.posE = current_pos();
            G.emx = mx; G.emy = my; G.emz = mz;
        } else {
            if (c.x > G.ex) {
                G.ex = c.x; G.ew = c.w; G.posE = current_pos();
                G.emx = mx; G.emy = my; G.emz = mz;
            }
            if (mx < G.mnx) G.mnx = mx; if (mx > G.mxx) G.mxx = mx;
            if (my < G.mny) G.mny = my; if (my > G.mxy) G.mxy = my;
            if (mz < G.mnz) G.mnz = mz; if (mz > G.mxz) G.mxz = mz;
            if (c.x < G.cnx) G.cnx = c.x; if (c.x > G.cxx) G.cxx = c.x;
            if (c.y < G.cny) G.cny = c.y; if (c.y > G.cxy) G.cxy = c.y;
            if (c.z < G.cnz) G.cnz = c.z; if (c.z > G.cxz) G.cxz = c.z;
            if (c.w < G.cnw) G.cnw = c.w; if (c.w > G.cxw) G.cxw = c.w;
        }
        {
            const Mat &P = current_pos();
            int k = 0;
            for (; k < G.ntab; ++k)
                if (G.tab[k].a == P.m[0] && G.tab[k].b == P.m[5] &&
                    G.tab[k].c == P.m[10] && G.tab[k].tx == P.m[12] &&
                    G.tab[k].ty == P.m[13] && G.tab[k].tz == P.m[14]) break;
            if (k == G.ntab && G.ntab < 32) {
                G.tab[k] = {P.m[0], P.m[5], P.m[10],
                            P.m[12], P.m[13], P.m[14], 0};
                ++G.ntab;
            }
            if (k < G.ntab) ++G.tab[k].n;
        }
        ++G.n;
    }

    GxVertex out{};
    out.x = c.x;
    out.y = c.y;
    out.z = c.z;
    out.w = c.w;
    out.u = g.u;
    out.v = g.v;
    out.color = g.color;
    return out;
}

// clip space -> screen space (the exact pre-clip formula, so w == 1 paths
// -- every ortho smoke and its reference pixel count -- are unchanged)
GxVertex to_screen(const GxVertex &cv) {
    GxVertex out = cv;
    const float iw = (std::fabs(cv.w) > 1e-6f) ? 1.0f / cv.w : 0.0f;
    // DS screen y runs bottom-up; the framebuffer is top-down.
    out.x = (cv.x * iw + 1.0f) * 0.5f * g.vp_w + g.vp_x;
    out.y = (1.0f - (cv.y * iw + 1.0f) * 0.5f) * g.vp_h + g.vp_y;
    out.z = (cv.z * iw + 1.0f) * 0.5f;
    return out;
}

GxVertex clip_lerp(const GxVertex &a, const GxVertex &b, float t) {
    GxVertex o;
    o.x = a.x + (b.x - a.x) * t;
    o.y = a.y + (b.y - a.y) * t;
    o.z = a.z + (b.z - a.z) * t;
    o.w = a.w + (b.w - a.w) * t;
    o.u = a.u + (b.u - a.u) * t;
    o.v = a.v + (b.v - a.v) * t;
    uint32_t ca = a.color, cb = b.color, c = 0;
    for (int s = 0; s < 32; s += 8) {
        const float ch = ((ca >> s) & 0xFF) +
                         (float(int((cb >> s) & 0xFF) - int((ca >> s) & 0xFF))) * t;
        c |= (uint32_t(ch < 0 ? 0 : ch > 255 ? 255 : ch) & 0xFF) << s;
    }
    o.color = c;
    return o;
}

void push_screen_tri(const GxVertex &a, const GxVertex &b, const GxVertex &c) {
    /* value-initialised: smoke_gx memcmps whole GxTriangles between the two
       submit paths, so the padding has to be deterministic */
    GxTriangle t{};
    t.v[0] = a; t.v[1] = b; t.v[2] = c;
    t.tex = g.tex_rgba; t.tw = g.tw; t.th = g.th;
    t.cull = static_cast<uint8_t>((g.poly_attr >> 6) & 3);
    t.alpha = static_cast<uint8_t>((g.poly_attr >> 16) & 31);
    t.mode = static_cast<uint8_t>((g.poly_attr >> 4) & 3);
    t.polyid = static_cast<uint8_t>((g.poly_attr >> 24) & 63);
    t.wrap = g.tex_wrap;
    /* TEXIMAGE_PARAM bits 26-28 are the format; 1 (A3I5) and 6 (A5I3)
       are the two translucent-texture formats. Attr alpha 0 is wire and
       draws with the opaque pass, same as the hardware. Mode-3 (shadow)
       polygons are translucent-class regardless of alpha: their stencil
       protocol below reads the depth the opaque pass has already settled,
       and the mask/draw pair must run in submission order inside one pass. */
    const uint32_t fmt = (g_teximage >> 26) & 7;
    t.translucent = static_cast<uint8_t>(
        (t.alpha >= 1 && t.alpha <= 30) ||
        (t.tex && (fmt == 1 || fmt == 6)) ||
        t.mode == 3);
    t.dbg_tex = g_teximage;
    g.tris.push_back(t);
}

// Does the active projection put a near plane in front of the camera? It
// does exactly when w depends on the vertex, which is the fourth column of
// the matrix: w = x*m[3] + y*m[7] + z*m[11] + m[15]. A perspective
// projection carries the -1 in m[11] that makes w the eye-space depth, and
// its near plane is the clip-space plane z + w == 0.
//
// A CONSTANT-w PROJECTION HAS NO NEAR PLANE AND MUST NOT BE CLIPPED AGAINST
// ONE. Every ortho harness in the port (smoke_gx, smoke_model, smoke_anim,
// the fit passes) sets an identity z row and feeds raw model z straight
// through, which is a viewer's framing trick and not a view volume at all;
// treating z == -1 as its near plane cuts the far half off Mario. The
// distance below is what those passes have always used -- w >= eps, which
// only guards the divide -- and it leaves them bit-for-bit unchanged.
float near_dist(const GxVertex &v, bool persp) {
    const float NEAR_EPS = 1e-3f;
    return persp ? v.z + v.w : v.w - NEAR_EPS;
}

void emit_tri(const GxVertex &a, const GxVertex &b, const GxVertex &c) {
    // Sutherland-Hodgman against the near plane.
    //
    // THE DISTANCE IS z + w, NOT w. Clipping a perspective triangle at
    // w == 1e-3 lands the new vertex a thousand times nearer than the near
    // plane, and dividing by that w throws it out to ~3e8 screen units --
    // seven digits of float spent before the rasteriser sees it, so the
    // edge functions and the perspective-correct UVs it feeds are noise.
    // On the castle grounds that showed as a staircase of triangular
    // notches down the seam where the big ground quads cross the camera
    // plane: the sliver covers those pixels, samples a garbage texel,
    // finds it transparent and leaves the clear colour. At z + w the new
    // vertex lands ON the near plane, where w is the near distance and the
    // divide is the ordinary one.
    const bool persp = g.proj.m[3] != 0.0f || g.proj.m[7] != 0.0f ||
                       g.proj.m[11] != 0.0f;
    GxVertex in[3] = {a, b, c};
    GxVertex outp[4];
    int n = 0;
    for (int i = 0; i < 3; ++i) {
        const GxVertex &cur = in[i];
        const GxVertex &nxt = in[(i + 1) % 3];
        const float dc = near_dist(cur, persp), dn = near_dist(nxt, persp);
        const bool cin = dc >= 0.0f;
        const bool nin = dn >= 0.0f;
        if (cin) outp[n++] = cur;
        if (cin != nin) {
            const float t = dc / (dc - dn);
            outp[n++] = clip_lerp(cur, nxt, t);
        }
    }
    if (n < 3) return;
    const GxVertex s0 = to_screen(outp[0]);
    GxVertex prev = to_screen(outp[1]);
    for (int i = 2; i < n; ++i) {
        const GxVertex cur = to_screen(outp[i]);
        push_screen_tri(s0, prev, cur);
        prev = cur;
    }
}

// Assemble according to the active BEGIN_VTXS primitive type.
void push_vertex(const GxVertex &v) {
    g.strip.push_back(v);
    const size_t n = g.strip.size();
    switch (g.prim) {
        case 0:                                        // separate triangles
            if (n == 3) { emit_tri(g.strip[0], g.strip[1], g.strip[2]); g.strip.clear(); }
            break;
        case 1:                                        // separate quads
            if (n == 4) {
                emit_tri(g.strip[0], g.strip[1], g.strip[2]);
                emit_tri(g.strip[0], g.strip[2], g.strip[3]);
                g.strip.clear();
            }
            break;
        case 2:                                        // triangle strip
            if (n >= 3) {
                const GxVertex &p0 = g.strip[n - 3], &p1 = g.strip[n - 2], &p2 = g.strip[n - 1];
                if ((n - 3) & 1) emit_tri(p1, p0, p2);  // alternate winding
                else emit_tri(p0, p1, p2);
            }
            break;
        case 3:                                        // quad strip
            if (n >= 4 && (n % 2) == 0) {
                const GxVertex &p0 = g.strip[n - 4], &p1 = g.strip[n - 3];
                const GxVertex &p2 = g.strip[n - 2], &p3 = g.strip[n - 1];
                emit_tri(p0, p1, p3);
                emit_tri(p0, p3, p2);
            }
            break;
        default: break;
    }
}

void vertex(int16_t x, int16_t y, int16_t z) {
    g.vx = x; g.vy = y; g.vz = z;
    if (g.prim >= 0) push_vertex(project(x, y, z));
}

// --- command execution ------------------------------------------------------
void load_mtx(Mat &dst, const uint32_t *p, int n) {
    Mat m = Mat::identity();
    if (n == 16) {
        for (int i = 0; i < 16; ++i) m.m[i] = static_cast<int32_t>(p[i]) * FX12;
    } else {  // 4x3: three rows of 3 plus a translation row
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 3; ++c) m.m[r * 4 + c] = static_cast<int32_t>(p[r * 3 + c]) * FX12;
        m.m[3] = m.m[7] = m.m[11] = 0.0f;
        m.m[15] = 1.0f;
    }
    dst = m;
}

Mat mat_3x3(const uint32_t *p) {
    Mat m = Mat::identity();
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) m.m[r * 4 + c] = static_cast<int32_t>(p[r * 3 + c]) * FX12;
    return m;
}

// ---- the geometry command census (run link60 Stage 5 lane T2) ---------------
//
// WHAT IT ANSWERS, AND WHY THE VIEWPORT ROW ALONE COULD NOT. The audit's 3D
// block reports the LATCHED viewport rectangle and the polygon count, which
// separates "the 3D engine was handed nothing" from "it was handed geometry".
// It cannot separate the next question down: geometry arrived, so which of the
// STATE commands around it also arrived? A scene whose projection matrix, its
// polygon attributes and its lights are all missing submits exactly the same
// polygon count as one where they landed, and the two frames are a picture and
// a blank.
//
// The counters are file statics rather than State members ON PURPOSE. gx_reset
// assigns `g = State{}` once a frame, so a census inside State would be zeroed
// by the very reset this instrument exists to reason about.
uint32_t g_cmd_n[256];      // executed commands since the last census take
uint32_t g_port_n;          // gx_write_port calls since the last take
uint32_t g_fifo_n;          // gx_write_fifo words since the last take
uint32_t g_swap_param;      // the last SWAP_BUFFERS parameter word seen
uint32_t g_resets;          // gx_reset calls since the last take

// SM64DS_MTX_LOG=<n>: the first n PROJECTION-mode matrix loads, as fixed-point
// words, with the host return addresses that issued them. A projection whose
// first row is zero collapses every vertex onto the framebuffer's vertical
// centre line, and the only way to attribute that to a caller is to see who
// pushed the matrix -- the port has several doors into MTX_LOAD (the packed
// FIFO, the command ports, and host copies of the SDK helpers) and the
// register file alone cannot say which one was used.
void mtx_load_log(uint8_t cmd, const uint32_t *p) {
    static int budget = -1;
    if (budget < 0) {
        const char *e = getenv("SM64DS_MTX_LOG");
        budget = e ? atoi(e) : 0;
    }
    static int allmodes = -1;
    if (allmodes < 0) {
        const char *e = getenv("SM64DS_MTX_LOG_ALL");
        allmodes = e ? 1 : 0;
    }
    if (budget <= 0 || (g.mode != MTX_PROJ && !allmodes)) return;
    --budget;
    const int n = (cmd == 0x16 || cmd == 0x18) ? 16 : (cmd == 0x1A ? 9 : 12);
    fprintf(stderr, "[mtx] mode%d cmd %02x", g.mode, cmd);
    for (int i = 0; i < n; ++i)
        fprintf(stderr, " %d", (int32_t)p[i]);
    fprintf(stderr, "\n");
#if defined(_WIN32)
    void *bt[16];
    const unsigned short got = RtlCaptureStackBackTrace(0, 16, bt, 0);
    fprintf(stderr, "[mtx]   from");
    for (unsigned short i = 0; i < got; ++i) fprintf(stderr, " %p", bt[i]);
    fprintf(stderr, "\n");
#endif
    fflush(stderr);
}

int param_count(uint8_t cmd);   /* defined below; the trap needs it */

void exec(uint8_t cmd, const uint32_t *p, int np) {
    (void)np;
    ++g_cmd_n[cmd];
    if (cmd == 0x50) g_swap_param = p[0];
    switch (cmd) {
        case 0x00: break;                                        // NOP
        case 0x10: g.mode = p[0] & 3; break;                     // MTX_MODE
        case 0x11:                                               // MTX_PUSH
            ++g_mtx_push_mode[g.mode & 3];
            if (g.mode == MTX_PROJ) {
                if (g.proj_sp < 1) g.proj_stack[g.proj_sp++] = g.proj;
                else ++g_mtx_stack_refusals;
            } else if (g.mode == MTX_TEX && mtx_texstack()) {
                /* One entry, one pointer, and neither is the position
                   stack's. GBATEK gives GXSTAT no field for this level. */
                if (g.tex_sp < 1) { g.tex_stack = g.tex; ++g.tex_sp; }
                else ++g_mtx_stack_refusals;
            } else if (g.pos_sp < 31) {
                g.pos_stack[g.pos_sp] = g.pos; g.vec_stack[g.pos_sp] = g.vec; ++g.pos_sp;
            } else ++g_mtx_stack_refusals;
            gxstat_publish();
            break;
        case 0x12: {                                             // MTX_POP
            int n = static_cast<int32_t>(p[0] << 26) >> 26;      // signed 6-bit
            ++g_mtx_pop_mode[g.mode & 3];
            if (g.mode == MTX_PROJ) { if (g.proj_sp > 0) g.proj = g.proj_stack[--g.proj_sp]; }
            else if (g.mode == MTX_TEX && mtx_texstack()) {
                if (g.tex_sp > 0) { g.tex = g.tex_stack; --g.tex_sp; }
            }
            else { g.pos_sp -= n; if (g.pos_sp < 0) { g.pos_sp = 0; ++g_mtx_stack_refusals; }
                   if (g.pos_sp < 31) { g.pos = g.pos_stack[g.pos_sp]; g.vec = g.vec_stack[g.pos_sp]; } }
            gxstat_publish();
            break;
        }
        case 0x13: {                                             // MTX_STORE
            ++g_store_count;
            ++g_mtx_store_mode[g.mode & 3];
            const int i = p[0] & 31;
            if (g.mode == MTX_PROJ) g.proj_stack[0] = g.proj;
            else if (g.mode == MTX_TEX && mtx_texstack()) g.tex_stack = g.tex;
            else { g.pos_stack[i] = g.pos; g.vec_stack[i] = g.vec; }
            break;
        }
        case 0x14: {                                             // MTX_RESTORE
            const int i = p[0] & 31;
            ++g_mtx_restore_mode[g.mode & 3];
            if (g.mode == MTX_PROJ) g.proj = g.proj_stack[0];
            else if (g.mode == MTX_TEX && mtx_texstack()) g.tex = g.tex_stack;
            else { g.pos = g.pos_stack[i]; g.vec = g.vec_stack[i]; }
            break;
        }
        case 0x15:                                               // MTX_IDENTITY
            if (g.mode == MTX_PROJ) g.proj = Mat::identity();
            else if (g.mode == MTX_TEX) g.tex = Mat::identity();
            else { g.pos = Mat::identity(); if (g.mode == MTX_POSVEC) g.vec = Mat::identity(); }
            break;
        case 0x16: case 0x17: {                                  // MTX_LOAD_4x4 / 4x3
            mtx_load_log(cmd, p);
            Mat m; load_mtx(m, p, cmd == 0x16 ? 16 : 12);
            if (g.mode == MTX_PROJ) g.proj = m;
            else if (g.mode == MTX_TEX) g.tex = m;
            else { g.pos = m; if (g.mode == MTX_POSVEC) g.vec = m; }
            break;
        }
        case 0x18: case 0x19: case 0x1A: {                       // MTX_MULT_4x4 / 4x3 / 3x3
            gxstat_witness_sample();
            mtx_load_log(cmd, p);
            Mat m;
            if (cmd == 0x1A) m = mat_3x3(p);
            else load_mtx(m, p, cmd == 0x18 ? 16 : 12);
            if (g.mode == MTX_PROJ) g.proj = mul(m, g.proj);
            else if (g.mode == MTX_TEX) g.tex = mul(m, g.tex);
            else { g.pos = mul(m, g.pos); if (g.mode == MTX_POSVEC) g.vec = mul(m, g.vec); }
            break;
        }
        case 0x1B: {                                             // MTX_SCALE
            Mat m = Mat::identity();
            m.m[0] = static_cast<int32_t>(p[0]) * FX12;
            m.m[5] = static_cast<int32_t>(p[1]) * FX12;
            m.m[10] = static_cast<int32_t>(p[2]) * FX12;
            if (g.mode == MTX_PROJ) g.proj = mul(m, g.proj);
            else if (g.mode == MTX_TEX) g.tex = mul(m, g.tex);
            else g.pos = mul(m, g.pos);
            break;
        }
        case 0x1C: {                                             // MTX_TRANS
            Mat m = Mat::identity();
            m.m[12] = static_cast<int32_t>(p[0]) * FX12;
            m.m[13] = static_cast<int32_t>(p[1]) * FX12;
            m.m[14] = static_cast<int32_t>(p[2]) * FX12;
            if (g.mode == MTX_PROJ) g.proj = mul(m, g.proj);
            else if (g.mode == MTX_TEX) g.tex = mul(m, g.tex);
            else { g.pos = mul(m, g.pos); if (g.mode == MTX_POSVEC) g.vec = mul(m, g.vec); }
            break;
        }
        case 0x20: g.color = bgr555_to_argb(static_cast<uint16_t>(p[0] & 0x7FFF)); break;
        case 0x21: {                                             // NORMAL
            if (mat_log()) mat_note_normal(g.poly_attr);
            // 3 x 10-bit signed, 1.9 fixed point.
            auto n10 = [](uint32_t v) {
                return (static_cast<int32_t>(v << 22) >> 22) / 512.0f;
            };
            float nx = n10(p[0] & 0x3FF);
            float ny = n10((p[0] >> 10) & 0x3FF);
            float nz = n10((p[0] >> 20) & 0x3FF);
            /* texgen mode 2: normal-source (env mapping) -- offset the
               latched texcoord by the raw normal through the tex matrix */
            if (((g_teximage >> 30) & 3) == 2) {
                g.u = g.raw_u + (nx * g.tex.m[0] + ny * g.tex.m[4] +
                                 nz * g.tex.m[8]) * (1.0f / 16.0f);
                g.v = g.raw_v + (nx * g.tex.m[1] + ny * g.tex.m[5] +
                                 nz * g.tex.m[9]) * (1.0f / 16.0f);
            }

            // Normals are transformed by the directional matrix, not position.
            const Mat &v = g.vec;
            const float tx = nx * v.m[0] + ny * v.m[4] + nz * v.m[8];
            const float ty = nx * v.m[1] + ny * v.m[5] + nz * v.m[9];
            const float tz = nx * v.m[2] + ny * v.m[6] + nz * v.m[10];
            const float len = std::sqrt(tx * tx + ty * ty + tz * tz);
            nx = len > 1e-6f ? tx / len : 0;
            ny = len > 1e-6f ? ty / len : 0;
            nz = len > 1e-6f ? tz / len : 1;

            /* WHICH LIGHTS ARE ON IS THE POLYGON'S OWN BUSINESS. GBATEK puts
               the four light-enable flags in POLYGON_ATTR bits 0-3, so the
               material that is latched when a NORMAL executes decides it and
               nothing else does. The engine had no way to read them: the only
               source of light_mask was gx_enable_lights(), which is a HARNESS
               call -- the BMD smokes and walk_window's own level path use it
               to stand a fixed light up in front of hand-fed geometry that
               carries no POLYGON_ATTR at all. On the SCENE path nobody calls
               it, so the mask stayed 0, every NORMAL fell through the loop
               below, and the vertex colour came out as pure emission. With
               SPE_EMI at 0 -- which is what every material in this game
               writes -- that is black, and a fully textured, fully rasterised
               model draws as a solid silhouette.

               Measured in this tree before the change (SM64DS_MAT_LOG):

                 scene 390   1092 NORMALs, all under POLYGON_ATTR 001f8081
                             (light 0 enabled), engine light_mask 0
                 level 1     1442 NORMALs, all under POLYGON_ATTR 011f8081
                             (light 0 enabled), engine light_mask 1

               and in both runs ZERO NORMALs executed under any of the
               lights=0000 materials. So reading the bits is exactly what the
               level path was already doing by accident and is what the scene
               path was missing.

               light_mask is OR-ed in rather than replaced because the harness
               geometry it exists for never emits a POLYGON_ATTR: dropping it
               would darken the smokes instead. It can only ever ADD a light,
               so nothing that is lit today can go dark through this line. */
            const uint32_t lmask = (g.poly_attr & 0xFu) | g.light_mask;
            float c[3] = {g.emission[0], g.emission[1], g.emission[2]};
            for (int i = 0; i < 4; ++i) {
                if (!((lmask >> i) & 1)) continue;
                const State::Light &L = g.lights[i];
                // GBATEK: diffuse level is max(0, -dot(light_vector, normal)).
                float d = -(L.dx * nx + L.dy * ny + L.dz * nz);
                if (d < 0) d = 0;
                const float lc[3] = {L.r, L.g, L.b};
                for (int k = 0; k < 3; ++k)
                    c[k] += g.diffuse[k] * lc[k] * d + g.ambient[k] * lc[k];
            }
            auto ch = [](float f) {
                const int i = static_cast<int>(f * 255.0f + 0.5f);
                return static_cast<uint32_t>(i < 0 ? 0 : (i > 255 ? 255 : i));
            };
            g.color = 0xFF000000u | (ch(c[0]) << 16) | (ch(c[1]) << 8) | ch(c[2]);
            break;
        }
        case 0x22:                                               // TEXCOORD (1.4 fx)
            g.raw_u = static_cast<int16_t>(p[0] & 0xFFFF) / 16.0f;
            g.raw_v = static_cast<int16_t>(p[0] >> 16) / 16.0f;
            /* texgen (TEXIMAGE_PARAM bits 30-31): mode 1 multiplies the
               coord by the texture matrix. The stage decals (grass
               fringes) carry their texel scale THERE -- raw coords
               collapse them to a single texel (the solid-green strips). */
            if (((g_teximage >> 30) & 3) == 1) {
                g.u = g.raw_u * g.tex.m[0] + g.raw_v * g.tex.m[4] +
                      (g.tex.m[8] + g.tex.m[12]) * (1.0f / 16.0f);
                g.v = g.raw_u * g.tex.m[1] + g.raw_v * g.tex.m[5] +
                      (g.tex.m[9] + g.tex.m[13]) * (1.0f / 16.0f);
            } else {
                g.u = g.raw_u;
                g.v = g.raw_v;
            }
            break;
        case 0x23:                                               // VTX_16
            vertex(static_cast<int16_t>(p[0] & 0xFFFF), static_cast<int16_t>(p[0] >> 16),
                   static_cast<int16_t>(p[1] & 0xFFFF));
            break;
        case 0x24: {                                             // VTX_10  (s4.6 -> 4.12)
            auto s10 = [](uint32_t v) { return static_cast<int16_t>((static_cast<int32_t>(v << 22) >> 22) << 6); };
            vertex(s10(p[0] & 0x3FF), s10((p[0] >> 10) & 0x3FF), s10((p[0] >> 20) & 0x3FF));
            break;
        }
        case 0x25: vertex(static_cast<int16_t>(p[0] & 0xFFFF), static_cast<int16_t>(p[0] >> 16), g.vz); break;
        case 0x26: vertex(static_cast<int16_t>(p[0] & 0xFFFF), g.vy, static_cast<int16_t>(p[0] >> 16)); break;
        case 0x27: vertex(g.vx, static_cast<int16_t>(p[0] & 0xFFFF), static_cast<int16_t>(p[0] >> 16)); break;
        case 0x28: {                                             // VTX_DIFF
            auto d10 = [](uint32_t v) { return static_cast<int32_t>(v << 22) >> 22; };
            vertex(static_cast<int16_t>(g.vx + d10(p[0] & 0x3FF)),
                   static_cast<int16_t>(g.vy + d10((p[0] >> 10) & 0x3FF)),
                   static_cast<int16_t>(g.vz + d10((p[0] >> 20) & 0x3FF)));
            break;
        }
        case 0x29:                                               // POLYGON_ATTR
            g.poly_attr = p[0];
            if (mat_log()) mat_note(g_matlog_attr, g_matlog_nattr, p[0]);
            break;
        case 0x2A: gx_teximage_param(p[0]); break;               // TEXIMAGE_PARAM
        case 0x2B: gx_pltt_base(p[0]); break;                    // PLTT_BASE
        case 0x30: {                                             // DIF_AMB
            if (mat_log()) mat_note(g_matlog_difamb, g_matlog_ndifamb, p[0]);
            auto unpack = [](uint32_t v, float *o) {
                o[0] = (v & 0x1F) / 31.0f;
                o[1] = ((v >> 5) & 0x1F) / 31.0f;
                o[2] = ((v >> 10) & 0x1F) / 31.0f;
            };
            unpack(p[0] & 0x7FFF, g.diffuse);
            unpack((p[0] >> 16) & 0x7FFF, g.ambient);
            // Bit 15 sets the vertex colour to the diffuse colour immediately,
            // which is what an unlit polygon then draws with.
            if ((p[0] >> 15) & 1) {
                auto ch = [](float f) { return static_cast<uint32_t>(f * 255.0f + 0.5f); };
                g.color = 0xFF000000u | (ch(g.diffuse[0]) << 16)
                          | (ch(g.diffuse[1]) << 8) | ch(g.diffuse[2]);
            }
            break;
        }
        case 0x31: {                                             // SPE_EMI
            if (mat_log()) mat_note(g_matlog_speemi, g_matlog_nspeemi, p[0]);
            g.emission[0] = ((p[0] >> 16) & 0x1F) / 31.0f;
            g.emission[1] = ((p[0] >> 21) & 0x1F) / 31.0f;
            g.emission[2] = ((p[0] >> 26) & 0x1F) / 31.0f;
            break;
        }
        case 0x32: {                                             // LIGHT_VECTOR
            ++g_matlog_nlightvec;
            const int i = (p[0] >> 30) & 3;
            auto n10 = [](uint32_t v) {
                return (static_cast<int32_t>(v << 22) >> 22) / 512.0f;
            };
            g.lights[i].dx = n10(p[0] & 0x3FF);
            g.lights[i].dy = n10((p[0] >> 10) & 0x3FF);
            g.lights[i].dz = n10((p[0] >> 20) & 0x3FF);
            break;
        }
        case 0x33: {                                             // LIGHT_COLOR
            ++g_matlog_nlightcol;
            const int i = (p[0] >> 30) & 3;
            g.lights[i].r = (p[0] & 0x1F) / 31.0f;
            g.lights[i].g = ((p[0] >> 5) & 0x1F) / 31.0f;
            g.lights[i].b = ((p[0] >> 10) & 0x1F) / 31.0f;
            break;
        }
        case 0x34: break;                                        // SHININESS
        case 0x40:                                               // BEGIN_VTXS
            g.prim = p[0] & 3;
            g.strip.clear();
            g.strip_parity = 0;
            break;
        case 0x41: g.prim = -1; g.strip.clear(); break;          // END_VTXS
        case 0x50: break;                                        // SWAP_BUFFERS
        case 0x60: {                                             // VIEWPORT
            // The register speaks DS panel coordinates (0..255 x 0..191);
            // scale to the framebuffer so game-issued full-screen viewports
            // fill a hi-res target too. At 256x192 the factors are 1 and
            // this is exactly the old math.
            const int x1 = p[0] & 0xFF, y1 = (p[0] >> 8) & 0xFF;
            const int x2 = (p[0] >> 16) & 0xFF, y2 = (p[0] >> 24) & 0xFF;
            g.vp_x = x1 * active_w / 256;
            g.vp_y = y1 * active_h / 192;
            g.vp_w = (x2 - x1 + 1) * active_w / 256;
            g.vp_h = (y2 - y1 + 1) * active_h / 192;
            ++g.vp_writes;
            break;
        }
        default: break;
    }
    /* SM64DS_STAR_TRAP: catch the exact matrix command that installs the
       title star's blown-up transform, with the raw parameter words and the
       host return addresses that issued it. The star's signature is a
       position matrix whose z scale sits at 1.4648 while x/y have jumped
       past 10 -- the ~16x that a signed 1.3.12 value read unsigned produces
       the frame it goes negative. */
    if (cmd >= 0x10 && cmd <= 0x1C) {
        static int budget = -1;
        if (budget < 0) {
            const char *e = getenv("SM64DS_STAR_TRAP");
            budget = e ? atoi(e) : 0;
        }
        if (budget > 0 && g.pos.m[0] > 10.0f &&
            g.pos.m[10] > 1.40f && g.pos.m[10] < 1.55f) {
            --budget;
            fprintf(stderr, "[startrap] cmd %02x mode %d -> pos diag "
                    "%.4f %.4f %.4f | raw", cmd, g.mode,
                    g.pos.m[0], g.pos.m[5], g.pos.m[10]);
            const int n = param_count(cmd);
            for (int i = 0; i < n; ++i)
                fprintf(stderr, " %08x(%d)", p[i], (int32_t)p[i]);
            fprintf(stderr, "\n");
#if defined(_WIN32)
            void *bt[24];
            const unsigned short got = RtlCaptureStackBackTrace(0, 24, bt, 0);
            fprintf(stderr, "[startrap]   from");
            for (unsigned short i = 0; i < got; ++i)
                fprintf(stderr, " %p", bt[i]);
            fprintf(stderr, "\n");
#endif
            fflush(stderr);
        }
    }
}

// GBATEK parameter counts, indexed by command byte.
int param_count(uint8_t cmd) {
    switch (cmd) {
        case 0x00: case 0x11: case 0x15: case 0x41: return 0;
        case 0x16: case 0x18: return 16;
        case 0x17: case 0x19: return 12;
        case 0x1A: return 9;
        case 0x1B: case 0x1C: case 0x70: return 3;
        case 0x23: case 0x71: return 2;
        case 0x34: return 32;
        default: return 1;
    }
}

// --- packed FIFO state machine ---------------------------------------------
uint8_t g_queue[4];
int g_queued = 0, g_qpos = 0;
uint32_t g_params[32];
int g_have = 0;

void feed(uint32_t word) {
    if (g_queued == 0) {                       // expecting a command word
        for (int i = 0; i < 4; ++i) g_queue[i] = static_cast<uint8_t>(word >> (i * 8));
        g_queued = 4;
        g_qpos = 0;
        g_have = 0;
        // Commands taking no parameters execute immediately.
        while (g_qpos < g_queued && param_count(g_queue[g_qpos]) == 0) {
            exec(g_queue[g_qpos], nullptr, 0);
            ++g_qpos;
        }
        if (g_qpos >= g_queued) g_queued = 0;
        return;
    }
    const uint8_t cmd = g_queue[g_qpos];
    g_params[g_have++] = word;
    if (g_have >= param_count(cmd)) {
        exec(cmd, g_params, g_have);
        g_have = 0;
        ++g_qpos;
        while (g_qpos < g_queued && param_count(g_queue[g_qpos]) == 0) {
            exec(g_queue[g_qpos], nullptr, 0);
            ++g_qpos;
        }
        if (g_qpos >= g_queued) g_queued = 0;
    }
}

// --- direct port writes -----------------------------------------------------
uint8_t g_port_cmd = 0;
uint32_t g_port_params[32];
int g_port_have = 0;

}  // namespace

void gx_stream_note(uint32_t w);
void gx_write_fifo(uint32_t word) { ++g_fifo_n; gx_stream_note(word); feed(word); }

void gx_set_matrix_slot(int slot, const float m[16]) {
    if (slot < 0 || slot >= 32) return;
    Mat mm;
    for (int i = 0; i < 16; ++i) mm.m[i] = m[i];
    g.pos_stack[slot] = mm;
    g.vec_stack[slot] = mm;
}

void gx_set_material(uint32_t dif_amb, uint32_t spe_emi) {
    exec(0x30, &dif_amb, 1);
    exec(0x31, &spe_emi, 1);
}

void gx_set_light(int index, float dx, float dy, float dz, uint32_t bgr555) {
    if (index < 0 || index > 3) return;
    const float len = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (len > 1e-6f) { dx /= len; dy /= len; dz /= len; }
    g.lights[index] = {dx, dy, dz,
                       (bgr555 & 0x1F) / 31.0f, ((bgr555 >> 5) & 0x1F) / 31.0f,
                       ((bgr555 >> 10) & 0x1F) / 31.0f};
}

void gx_enable_lights(uint32_t mask) { g.light_mask = mask & 0xF; }

void gx_bind_texture(const uint32_t *rgba, int width, int height) {
    g.tex_rgba = rgba;
    g.tw = width;
    g.th = height;
    // The direct entry (the BMD harness path) carries no TEXIMAGE_PARAM, so it
    // keeps the plain repeat-in-both-directions behaviour it always had; the
    // VRAM bind below overrides this with the material's real wrap mode.
    g.tex_wrap = 3;
}

// --- VRAM-sourced texturing: the game path ----------------------------------
// The game binds textures by writing TEXIMAGE_PARAM / PLTT_BASE around each
// display list; the texel data was already uploaded through GX::LoadTex into
// the mapped texture-slot window and palettes through GX::LoadTexPltt. On
// hardware the slot addresses come from bank assignment; the port's HAL
// points the game's upload-base globals at these same windows, so a slot
// offset here is a plain host address.
namespace {

constexpr uintptr_t TEX_SLOT_BASE  = 0x06800000u;   // texture slots, mapped
constexpr uintptr_t PLTT_SLOT_BASE = 0x06880000u;   // palette slots, mapped

uint32_t g_teximage, g_plttbase;

/* Key: the pair of VRAM words that name the texture, plus a cheap read of
   what is actually sitting in those slots. The teximage/palette pair alone is
   not enough on its own -- a scene change can upload different texels to the
   same slot offset with the same parameters, and the soak does exactly that,
   once per model -- so the first and last word of the block and the first word
   of the palette ride along. That is three loads per bind, against a decode
   plus a heap allocation, and it means a stale entry cannot be served. */
struct TexKey {
    uint64_t name;          // plttbase:teximage
    uint32_t c0, c1, cp;    // content probe
    bool operator<(const TexKey &o) const {
        if (name != o.name) return name < o.name;
        if (c0 != o.c0) return c0 < o.c0;
        if (c1 != o.c1) return c1 < o.c1;
        return cp < o.cp;
    }
};
std::map<TexKey, std::vector<uint32_t>> g_vram_tex_cache;

uint32_t probe_word(const uint8_t *p, int32_t len, int32_t off) {
    if (!p || off < 0 || off + 4 > len) return 0;
    uint32_t v;
    std::memcpy(&v, p + off, 4);
    return v;
}

/* SM64DS_TEX_LOG=1: one line per DISTINCT bind reaching the engine --
   teximage word, decoded geometry, both VRAM addresses and whether the
   decode produced texels. This is the inventory that says "bound but
   decoded to nothing" apart from "never bound at all". */
int tex_log() {
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_TEX_LOG") ? 1 : 0;
    return on;
}

void bind_from_vram() {
    const uint32_t fmt = (g_teximage >> 26) & 7;
    if (fmt == 0) {
        if (tex_log()) {
            static std::map<uint32_t, int> seen;
            if (seen.emplace(g_teximage, 1).second)
                printf("[texbind] tex=%08x pltt=%04x fmt=0 NO-TEXTURE\n",
                       g_teximage, g_plttbase);
        }
        gx_bind_texture(nullptr, 0, 0);
        return;
    }
    /* The descriptor is pure arithmetic on the two latched words, so it is
       built before the lookup -- the content probe needs its geometry. */
    TextureDesc d;
    const uint32_t off = (g_teximage & 0xFFFF) << 3;
    d.width = 8 << ((g_teximage >> 20) & 7);
    d.height = 8 << ((g_teximage >> 23) & 7);
    d.format = static_cast<int>(fmt);
    d.color0_transparent = (g_teximage >> 29) & 1;
    d.data = reinterpret_cast<const uint8_t *>(TEX_SLOT_BASE + off);
    d.data_len = 0x80000 - static_cast<int32_t>(off);
    // Format 5 keeps its 4x4 palette-index words in slot 1, at half the
    // block offset (GBATEK); the game uploads them right after the blocks.
    if (fmt == 5) {
        d.index = reinterpret_cast<const uint8_t *>(TEX_SLOT_BASE + 0x20000 + off / 2);
        d.index_len = 0x20000 - static_cast<int32_t>(off / 2);
    }
    const uint32_t pal_off = g_plttbase * (fmt == 2 ? 8u : 16u);
    d.pal = reinterpret_cast<const uint8_t *>(PLTT_SLOT_BASE + pal_off);
    d.pal_len = 0x18000 - static_cast<int32_t>(pal_off);

    TexKey key;
    key.name = (static_cast<uint64_t>(g_plttbase) << 32) | g_teximage;
    /* w*h/4 bytes is the smallest any DS format uses for a block of that
       size, so it is in range whatever the format turns out to be. */
    int32_t span = d.width * d.height / 4;
    if (span > d.data_len) span = d.data_len;
    key.c0 = probe_word(d.data, d.data_len, 0);
    key.c1 = probe_word(d.data, d.data_len, span - 4);
    key.cp = probe_word(d.pal, d.pal_len, 0);

    auto it = g_vram_tex_cache.find(key);
    if (it == g_vram_tex_cache.end()) {
        std::vector<uint32_t> rgba;
        const bool ok = texture_decode(d, rgba);
        ++g_tex_decodes;
        if (tex_log())
            printf("[texbind] tex=%08x pltt=%04x fmt=%u %dx%d texoff=%05x "
                   "idxoff=%05x paloff=%05x %s\n",
                   g_teximage, g_plttbase, fmt, d.width, d.height, off,
                   fmt == 5 ? 0x20000u + off / 2 : 0u, pal_off,
                   ok ? "ok" : "DECODE-FAILED");
        if (!ok) { gx_bind_texture(nullptr, 0, 0); return; }
        /* SM64DS_TEX_DUMP: write every texture bound this run as a PPM
           next to the exe -- artifact triage. THE PALETTE IS PART OF THE
           NAME: the material bind writes PLTT_BASE before TEXIMAGE_PARAM,
           so every material also produces a transient pairing of its
           palette with the PREVIOUS texture. Naming by teximage alone let
           that transient overwrite the real decode, and the inventory then
           read as "the sand and fringe materials bind nothing". */
        if (getenv("SM64DS_TEX_DUMP")) {
            char nm[64];
            snprintf(nm, sizeof nm, "tex_%08x_p%04x_f%d_%dx%d.ppm",
                     g_teximage, g_plttbase, d.format, d.width, d.height);
            if (FILE *f = fopen(nm, "wb")) {
                fprintf(f, "P6\n%d %d\n255\n", d.width, d.height);
                for (size_t i = 0; i < rgba.size(); ++i) {
                    /* checkerboard where alpha < 128 so holes are visible */
                    uint32_t px = rgba[i];
                    if ((px >> 24) < 128)
                        px = ((i / 4 + i / (4 * d.width)) & 1) ? 0xFF00FFFF
                                                               : 0xFF000000;
                    unsigned char rgb[3] = {
                        (unsigned char)(px >> 16), (unsigned char)(px >> 8),
                        (unsigned char)px};
                    fwrite(rgb, 1, 3, f);
                }
                fclose(f);
            }
        }
        it = g_vram_tex_cache.emplace(key, std::move(rgba)).first;
    }
    const int w = 8 << ((g_teximage >> 20) & 7), h = 8 << ((g_teximage >> 23) & 7);
    gx_bind_texture(it->second.data(), w, h);
    /* THE WRAP MODE IS PART OF THE BIND. TEXIMAGE_PARAM bits 16/17 select
       repeat vs CLAMP, bits 18/19 add mirroring on top of repeat (GBATEK).
       The raster used to wrap everything unconditionally, which is right
       for only one of the four combinations. It shows up on the castle
       grounds: mc_road -- the path plus the grass fringe along its edge,
       one texture -- is authored with FLIP T, and wrapping it instead put
       solid green bands across the middle of the path and left the fringe
       off the edge where the lawn meets it. Mario's gloves and the Mad
       Piano bled the same way at their mirrored tiles. */
    g.tex_wrap = static_cast<uint8_t>((g_teximage >> 16) & 0xF);
}

}  // namespace

void gx_teximage_param(uint32_t v) { g_teximage = v; bind_from_vram(); }
void gx_pltt_base(uint32_t v) { g_plttbase = v; bind_from_vram(); }

// Diagnostic: a running hash of every word entering the engine, so a smoke
// can tell "the game emitted a different stream" from "the decode ignored
// it". Reset returns the previous value.
static uint32_t g_stream_hash = 2166136261u;
void gx_stream_note(uint32_t w) { g_stream_hash = (g_stream_hash ^ w) * 16777619u; }
uint32_t gx_stream_hash_reset()
{
    const uint32_t h = g_stream_hash;
    g_stream_hash = 2166136261u;
    return h;
}

int gx_store_count_reset() { int n = g_store_count; g_store_count = 0; return n; }

uint32_t gx_state_hash()
{
    uint32_t h = 2166136261u;
    const unsigned char *b = reinterpret_cast<const unsigned char *>(&g.pos);
    for (size_t i = 0; i < sizeof(Mat); ++i) h = (h ^ b[i]) * 16777619u;
    b = reinterpret_cast<const unsigned char *>(g.pos_stack);
    for (size_t i = 0; i < sizeof(Mat) * 4; ++i) h = (h ^ b[i]) * 16777619u;
    return h;
}

void gx_debug_matrices(int *mode, float pos[16], float proj[16]) {
    if (mode) *mode = g.mode;
    if (pos) for (int i = 0; i < 16; ++i) pos[i] = g.pos.m[i];
    if (proj) for (int i = 0; i < 16; ++i) proj[i] = g.proj.m[i];
}

void gx_debug_viewport(int &x, int &y, int &w, int &h, int &sets) {
    x = g.vp_x; y = g.vp_y; w = g.vp_w; h = g.vp_h; sets = g.vp_writes;
}

void gx_debug_commands(uint32_t counts[256], uint32_t &ports, uint32_t &fifo,
                       uint32_t &swap_param, uint32_t &resets, bool take) {
    if (counts) for (int i = 0; i < 256; ++i) counts[i] = g_cmd_n[i];
    ports = g_port_n;
    fifo = g_fifo_n;
    swap_param = g_swap_param;
    resets = g_resets;
    if (take) {
        for (int i = 0; i < 256; ++i) g_cmd_n[i] = 0;
        g_port_n = g_fifo_n = g_resets = 0;
    }
}

void gx_write_port(uint32_t addr, uint32_t value) {
    ++g_port_n;
    gx_stream_note(addr ^ value);
    const uint8_t cmd = static_cast<uint8_t>((addr - 0x04000400u) >> 2);
    const int need = param_count(cmd);
    if (need <= 1) {
        exec(cmd, &value, 1);
        return;
    }
    // Multi-parameter ports are written repeatedly; accumulate until satisfied.
    if (cmd != g_port_cmd) { g_port_cmd = cmd; g_port_have = 0; }
    if (g_port_have < 32) g_port_params[g_port_have++] = value;
    if (g_port_have >= need) {
        exec(cmd, g_port_params, g_port_have);
        g_port_have = 0;
        g_port_cmd = 0;
    }
}

/* Drop every decoded texture. The cache OUTLIVES gx_reset -- gx_reset runs
   once per frame, and throwing the decodes away there meant every texture in
   the scene was decoded out of VRAM and heap-allocated again 30 times a
   second. Callers that recycle VRAM slot offsets faster than the content
   probe can tell apart, or that just want the memory back, say so here; the
   soaks do, once per model. SM64DS_TEX_NOCACHE=1 restores the old
   clear-every-reset behaviour for an A/B. */
void gx_invalidate_textures() { g_vram_tex_cache.clear(); }

void gx_reset() {
    ++g_resets;
    static int nocache = -1;
    if (nocache < 0) nocache = getenv("SM64DS_TEX_NOCACHE") ? 1 : 0;
    if (nocache) g_vram_tex_cache.clear();
    /* KEEP THE TWO BUFFERS ACROSS THE RESET. `g = State{}` destroys the
       triangle list and the vertex strip and grows them back from nothing on
       the next frame -- a couple of thousand triangles at 116 bytes each, so a
       free plus the whole doubling ramp of reallocations, thirty times a
       second, for a buffer whose size barely changes frame to frame. Moving
       them out across the assignment and back in keeps their capacity;
       clear() on these trivially destructible element types is a size store. */
    if (stargeo_on()) {
        StarGeo &G = g_stargeo;
        if (G.n && G.snapped) {
            std::fprintf(stderr,
                "[stargeo] f%u verts=%d model x[%.1f %.1f] y[%.1f %.1f] "
                "z[%.1f %.1f] | clip x[%.3f %.3f] y[%.3f %.3f] w[%.3f %.3f]"
                " | pos diag %.4f %.4f %.4f trans %.3f %.3f %.3f"
                " | proj %.4f %.4f %.4f m11=%.4f m14=%.4f\n",
                G.frame, G.n, G.mnx, G.mxx, G.mny, G.mxy, G.mnz, G.mxz,
                G.cnx, G.cxx, G.cny, G.cxy, G.cnw, G.cxw,
                G.pos0.m[0], G.pos0.m[5], G.pos0.m[10],
                G.pos0.m[12], G.pos0.m[13], G.pos0.m[14],
                G.proj0.m[0], G.proj0.m[5], G.proj0.m[10],
                G.proj0.m[11], G.proj0.m[14]);
            std::fprintf(stderr,
                "[stargeoE] f%u extreme cx=%.3f w=%.3f model(%.2f %.2f %.2f)"
                " posE diag %.4f %.4f %.4f trans %.3f %.3f %.3f\n",
                G.frame, G.ex, G.ew, G.emx, G.emy, G.emz,
                G.posE.m[0], G.posE.m[5], G.posE.m[10],
                G.posE.m[12], G.posE.m[13], G.posE.m[14]);
            for (int k = 0; k < G.ntab; ++k)
                std::fprintf(stderr,
                    "[stargeoM] f%u obj%d verts=%d scale(%.4f %.4f %.4f)"
                    " xy/z=%.3f trans(%.3f %.3f %.3f)\n",
                    G.frame, k, G.tab[k].n, G.tab[k].a, G.tab[k].b,
                    G.tab[k].c,
                    G.tab[k].c != 0.0f ? G.tab[k].a / G.tab[k].c : 0.0f,
                    G.tab[k].tx, G.tab[k].ty, G.tab[k].tz);
        }
        ++G.frame; G.n = 0; G.snapped = 0; G.ntab = 0;
    }

    std::vector<GxVertex> strip = std::move(g.strip);
    std::vector<GxTriangle> tris = std::move(g.tris);
    strip.clear();
    tris.clear();
    /* AND KEEP THE LIGHT TABLE, because it is not per-frame state.
       LIGHT_VECTOR and LIGHT_COLOR are latched registers on the geometry
       engine and nothing on a DS clears them at a frame boundary. The game
       programs them ONCE PER SCENE -- dScMgSingle3DBase_c's slot 33 writes
       both light colours in its one-shot 3D setup, and Scene::Initialise3d-
       Graphics does the same for the scenes that reach it -- so a reset that
       wipes them here makes a once-per-scene program unobservable: the
       colours are gone before the first triangle of the first frame is
       submitted, every light multiplies to zero, and every lit vertex comes
       out at the emission colour.

       Measured this way round in run mg5 lane YTEX: after the two writes
       were routed, "LIGHT_COLOR commands executed: 2" and light 0 STILL read
       colour 0,0,0 at frame 0, because the reset between the setup and the
       draw had already thrown them away.

       DIF_AMB, SPE_EMI, POLYGON_ATTR and the vertex colour are re-issued per
       material per frame by every path in this port, so what the reset does
       with them is unobservable and is left exactly as it was. */
    State::Light lights[4];
    for (int i = 0; i < 4; ++i) lights[i] = g.lights[i];
    const uint32_t light_mask = g.light_mask;
    /* AND KEEP THE PROJECTION MATRIX, for the light table's reason, one class
       of state over. Added run mg10 lane RGX.

       THIS FUNCTION MODELS NO ROM INSTRUCTION. Nothing in the game calls it:
       hal/scene_boot.cpp's frame loop calls it, once per frame, as the host's
       own "begin a frame's command stream". The DS has no such boundary -- the
       projection matrix is a latched register bank that holds whatever the
       last MTX_LOAD in mode 0 put there until somebody loads another one -- so
       a scene that programs its camera ONCE and never touches it again is
       correct on hardware and was losing its projection here on the next
       vblank.

       MEASURED, scene 361 (dScMgCup_c, "Tox Box Shuffle") and scene 367
       (dScMgSound_c, "Boom Box"), which are the two hosted scenes that do
       exactly that. SM64DS_MTX_LOG resolved through walk_window.map:

         363, EVERY FRAME  mem_render -> func_ov006_020f73f4
                           -> Camera_UpdateMatrices -> G3i_PerspectiveW_
         361, ONCE AT INIT cup_init -> func_ov006_020e0308
                           -> Camera_UpdateMatrices

       so MTX_LOAD_4x4 runs 1 per frame on 363 and 0 per frame on 361 after
       boot. ppu_audit's PROJ_IDENTITY row read 1 on all 300 samples of 361 and
       367 and 0 on 362, 363, 381, 388, 389 and 390, and only 311 of 361's 1806
       triangles survived as drawable against 1163 of 363's 1783. The two
       broken scenes' top captures are BYTE-IDENTICAL to each other, which is
       what says it is one defect and not two.

       AFTER: 361 and 367 both go PROJ_IDENTITY 1 -> 0 and drawable -> 1806 of
       1806, and 361's top screen goes from 21 distinct colours to 4311.

       AND NOTHING ELSE MOVES, swept rather than assumed, on one binary against
       SM64DS_PROJ_CARRY_OFF=1: the other 21 hosted scenes byte-identical, and
       ALL 46 MOUNTED LEVELS' selftest BMPs byte-identical too, 46 of 46, level
       1 at md5 5b783a29d753ce4dfbd35c79a7b2850f. The level sweep is the one
       this change owes and the scene sweep does not cover -- a level is a 3D
       scene as much as a minigame is. The scenes and levels that reload the
       projection every frame cannot notice a carry, which is nearly all of
       them, and that is why the blast radius is two.

       SCENE 387 (dScMgBooSeek_c) READS PROJ_IDENTITY 1 TOO AND IS NOT FIXED BY
       THIS. It submits ZERO triangles, so it is a different defect that shares
       one row of that census; named here so the row is not read as closed.

       SM64DS_PROJ_CARRY_OFF=1 puts the old wipe back on the same binary.

       THE PROJECTION STACK IS NOT CARRIED, deliberately. The line below sets
       both its slots to identity and carries its own derivation about display
       lists opening with MTX_RESTORE; carrying the stack too is a second
       change with a second blast radius, nothing measured here needs it, and
       nothing in the sweep moved without it. Named rather than folded in. */
    static int proj_carry_off = -1;
    if (proj_carry_off < 0) {
        const char *e = getenv("SM64DS_PROJ_CARRY_OFF");
        proj_carry_off = (e && *e && *e != '0') ? 1 : 0;
    }
    const Mat proj_keep = g.proj;
    g = State{};
    if (!proj_carry_off) g.proj = proj_keep;
    g.strip = std::move(strip);
    g.tris = std::move(tris);
    for (int i = 0; i < 4; ++i) g.lights[i] = lights[i];
    g.light_mask = light_mask;
    g_teximage = g_plttbase = 0;
    // The stack slots must start as identity, not zero. Model display lists open
    // with MTX_RESTORE against a slot the *scene* filled in earlier; rendering a
    // model on its own would otherwise load an all-zero matrix and collapse every
    // vertex onto the origin -- geometry decodes fine and nothing draws.
    for (int i = 0; i < 32; ++i) {
        g.pos_stack[i] = Mat::identity();
        g.vec_stack[i] = Mat::identity();
    }
    g.proj_stack[0] = g.proj_stack[1] = Mat::identity();
    g.tex_stack = Mat::identity();
    g_queued = g_qpos = g_have = 0;
    g_port_cmd = 0; g_port_have = 0;
    /* `g = State{}` above put both stack pointers back to 0, so the register
       has to say so too or the first reader of the new frame is told the last
       frame's depth. */
    gxstat_publish();
}

void gx_debug_proj(float out[16]) {
    for (int i = 0; i < 16; ++i) out[i] = g.proj.m[i];
}

/* The live POSITION matrix, for a caller that needs to say what a vertex it is
   about to submit will be transformed BY. gx_debug_proj's counterpart: the
   projection alone cannot tell an object placed at the wrong depth from one
   scaled wrong at the right depth, and those two have the same symptom. */
void gx_debug_pos(float out[16]) {
    for (int i = 0; i < 16; ++i) out[i] = g.pos.m[i];
}

// GXSTAT bits 8-12 and 13 want these. Clamped to the widths the register has
// (5 bits and 1 bit) so a caller can shift them in without re-checking; the
// MTX_PUSH handlers above already refuse to grow past 31 and 1 respectively.
void gx_matrix_stack_levels(unsigned &pos_level, unsigned &proj_level) {
    const int p = g.pos_sp < 0 ? 0 : (g.pos_sp > 31 ? 31 : g.pos_sp);
    const int q = g.proj_sp < 0 ? 0 : (g.proj_sp > 1 ? 1 : g.proj_sp);
    pos_level = (unsigned)p;
    proj_level = (unsigned)q;
}

const GxTriangle *gx_polygons(size_t &count) {
    count = g.tris.size();
    return g.tris.empty() ? nullptr : g.tris.data();
}

/* SM64DS_MAT_LOG=1: once, after the first full frame is assembled, the
   LIGHTING side of the same picture -- every distinct POLYGON_ATTR, DIF_AMB
   and SPE_EMI word that reached exec(), the light table the engine is
   holding, and the words actually SITTING in the mapped I/O window at the
   four material registers.

   That last column is the one this exists for. ntr maps real memory across
   0x04000000, so a translation unit that writes a geometry register with a
   plain store still latches a value there while the geometry engine never
   sees the command. Reading the latch apart from reading the engine is what
   separates "the game never asked for a light" from "the game asked and the
   ask did not arrive", and those two have the same symptom: every lit vertex
   comes out at the emission colour, which is usually black. */
static void mat_report() {
    static int on = -1;
    if (on < 0) on = mat_log();
    if (!on) return;
    on = 0;                              // one frame is the whole report
    printf("[mat] POLYGON_ATTR words seen (%u distinct):\n", g_matlog_nattr);
    for (unsigned i = 0; i < g_matlog_nattr; ++i)
        printf("[mat]   %08x  lights=%u%u%u%u mode=%u cull=%u alpha=%u id=%u "
               "NORMALs=%u\n",
               g_matlog_attr[i], g_matlog_attr[i] & 1u,
               (g_matlog_attr[i] >> 1) & 1u, (g_matlog_attr[i] >> 2) & 1u,
               (g_matlog_attr[i] >> 3) & 1u, (g_matlog_attr[i] >> 4) & 3u,
               (g_matlog_attr[i] >> 6) & 3u, (g_matlog_attr[i] >> 16) & 31u,
               (g_matlog_attr[i] >> 24) & 63u, g_matlog_attr_normals[i]);
    printf("[mat]   NORMALs under an untabled attr: %u\n",
           g_matlog_normals_other);
    printf("[mat] DIF_AMB words seen (%u distinct):", g_matlog_ndifamb);
    for (unsigned i = 0; i < g_matlog_ndifamb; ++i)
        printf(" %08x", g_matlog_difamb[i]);
    printf("\n[mat] SPE_EMI words seen (%u distinct):", g_matlog_nspeemi);
    for (unsigned i = 0; i < g_matlog_nspeemi; ++i)
        printf(" %08x", g_matlog_speemi[i]);
    printf("\n[mat] LIGHT_VECTOR commands executed: %u   LIGHT_COLOR: %u\n",
           g_matlog_nlightvec, g_matlog_nlightcol);
    printf("[mat] engine light_mask=%x diffuse=%.3f,%.3f,%.3f "
           "ambient=%.3f,%.3f,%.3f emission=%.3f,%.3f,%.3f\n",
           g.light_mask, g.diffuse[0], g.diffuse[1], g.diffuse[2],
           g.ambient[0], g.ambient[1], g.ambient[2],
           g.emission[0], g.emission[1], g.emission[2]);
    for (int i = 0; i < 4; ++i)
        printf("[mat]   light %d dir=%.3f,%.3f,%.3f col=%.3f,%.3f,%.3f\n", i,
               g.lights[i].dx, g.lights[i].dy, g.lights[i].dz,
               g.lights[i].r, g.lights[i].g, g.lights[i].b);
    /* The latch, read straight out of the mapped window. */
    const uint32_t a0 = *reinterpret_cast<volatile uint32_t *>(0x040004C0u);
    const uint32_t a1 = *reinterpret_cast<volatile uint32_t *>(0x040004C4u);
    const uint32_t a2 = *reinterpret_cast<volatile uint32_t *>(0x040004C8u);
    const uint32_t a3 = *reinterpret_cast<volatile uint32_t *>(0x040004CCu);
    printf("[mat] I/O LATCH  DIF_AMB(4c0)=%08x SPE_EMI(4c4)=%08x "
           "LIGHT_VECTOR(4c8)=%08x LIGHT_COLOR(4cc)=%08x\n", a0, a1, a2, a3);
    printf("[mat] A NONZERO LATCH WITH A ZERO COMMAND COUNT IS A STORE THAT "
           "NEVER REACHED THE ENGINE.\n");
}

/* SM64DS_MTX_BALANCE=1: the MATRIX half of mat_report's question, per frame
   and then once at exit. DEFAULT OFF.

   WHY IT EXISTS. mat_report already separates "the game never asked" from "the
   game asked and the ask did not arrive" for the four material registers, and
   the separation is the same one every plain-built MMIO defect turns on: ntr
   maps real memory across 0x04000000, so a translation unit that reaches a
   geometry command port with a plain store latches a word there and the engine
   never receives the command. The MATRIX ports are the half nothing reported,
   and they are the half where a miss is unbounded rather than local: a
   MTX_PUSH that does not arrive does not lose one object's bracket, it leaves
   every MTX_MULT the bracket was meant to contain applied to the live matrix
   for the rest of the frame.

   THE TWO NUMBERS THAT DECIDE IT, side by side per port:
     exec   commands the engine actually ran (gx_debug_commands, no take, so
            this never disturbs a census another reader owns)
     latch  the word sitting in the mapped I/O window at that same port
   A NONZERO LATCH UNDER A ZERO EXEC IS A STORE THAT NEVER ARRIVED. A nonzero
   latch under a nonzero exec says only that both kinds of writer exist, which
   is why the per-frame stack line below it is printed as well: an engine whose
   position stack level never leaves 0 while thousands of MTX_MULTs run is an
   unbalanced bracket whatever the latch says. */
static void mtx_report(bool at_exit);
static void mtx_report_atexit() { mtx_report(true); }
static void mtx_report(bool at_exit) {
    static int on = -1;
    if (on < 0) {
        on = getenv("SM64DS_MTX_BALANCE") ? 1 : 0;
        if (on) std::atexit(mtx_report_atexit);
    }
    if (!on) return;
    static const struct { uint8_t cmd; const char *name; } kPorts[] = {
        {0x10, "MTX_MODE"},     {0x11, "MTX_PUSH"},   {0x12, "MTX_POP"},
        {0x13, "MTX_STORE"},    {0x14, "MTX_RESTORE"},{0x15, "MTX_IDENTITY"},
        {0x16, "MTX_LOAD_4x4"}, {0x17, "MTX_LOAD_4x3"},
        {0x18, "MTX_MULT_4x4"}, {0x19, "MTX_MULT_4x3"},
        {0x1A, "MTX_MULT_3x3"}, {0x1B, "MTX_SCALE"},  {0x1C, "MTX_TRANS"},
        {0x20, "COLOR"},        {0x21, "NORMAL"},     {0x22, "TEXCOORD"},
        {0x23, "VTX_16"},       {0x24, "VTX_10"},
        {0x29, "POLYGON_ATTR"}, {0x2A, "TEXIMAGE_PARAM"}, {0x2B, "PLTT_BASE"},
        {0x40, "BEGIN_VTXS"},   {0x41, "END_VTXS"},   {0x50, "SWAP_BUFFERS"},
        {0x60, "VIEWPORT"},
    };
    uint32_t counts[256];
    uint32_t ports, fifo, swap, resets;
    gx_debug_commands(counts, ports, fifo, swap, resets, /*take=*/false);
    unsigned pos_level = 0, proj_level = 0;
    gx_matrix_stack_levels(pos_level, proj_level);
    if (!at_exit) {
        /* One line per frame, and the STACK LEVEL is the point of it: a
           lifecycle rather than an endpoint, because a level read once at the
           end cannot tell a bracket that balanced from one that never ran. */
        static unsigned f;
        printf("[mtxbal] f%-4u pos_sp=%u proj_sp=%u  push=%u pop=%u "
               "mult4x3=%u mult4x4=%u scale=%u trans=%u  begin=%u\n",
               f++, pos_level, proj_level, counts[0x11], counts[0x12],
               counts[0x19], counts[0x18], counts[0x1B], counts[0x1C],
               counts[0x40]);
        return;
    }
    printf("[mtxbal] EXEC vs LATCH, whole run (%u port writes, %u fifo words, "
           "%u gx_reset)\n", ports, fifo, resets);
    for (const auto &r : kPorts) {
        const uint32_t addr = 0x04000400u + (uint32_t)r.cmd * 4u;
        const uint32_t latch = *reinterpret_cast<volatile uint32_t *>(
            static_cast<uintptr_t>(addr));
        printf("[mtxbal]   %-15s %08x  exec %8u  latch %08x%s\n", r.name, addr,
               counts[r.cmd], latch,
               (counts[r.cmd] == 0 && latch != 0)
                   ? "   <-- STORED, NEVER EXECUTED" : "");
    }
    printf("[mtxbal] A NONZERO LATCH UNDER A ZERO EXEC IS A PLAIN STORE THAT "
           "NEVER REACHED THE ENGINE.\n");
    /* BY MATRIX MODE, because MTX_PUSH is one port over four stacks and the
       per-port count above cannot tell them apart. Mode 3 is the row that
       decides whether the texture stack sharing the position stack was ever
       more than a latent wrong: a nonzero mode-3 push count means the
       position stack pointer -- the number GXSTAT publishes and the number
       func_02055624 pops by -- was being moved by texture-matrix brackets. */
    static const char *kMode[4] = {"proj(0)", "pos(1)", "posvec(2)", "tex(3)"};
    for (int m = 0; m < 4; ++m)
        printf("[mtxbal]   mode %-10s push %7u  pop %7u  store %7u  "
               "restore %7u\n", kMode[m], g_mtx_push_mode[m], g_mtx_pop_mode[m],
               g_mtx_store_mode[m], g_mtx_restore_mode[m]);
    printf("[mtxbal] stack refusals (end of stack, GBATEK sets GXSTAT bit 15): "
           "%u\n", g_mtx_stack_refusals);
    if (gxstat_witness())
        printf("[mtxbal] GXSTAT witness at MTX_MULT: agree %u  DISAGREE %u  "
               "(worst: engine level %u, register said %u)\n", g_gxw_agree,
               g_gxw_disagree, g_gxw_worst_true, g_gxw_worst_read);
    printf("[mtxbal] GXSTAT now %08x  (bits 8..12 pos level, 13 proj level, "
           "14 busy)\n",
           *reinterpret_cast<volatile uint32_t *>(
               static_cast<uintptr_t>(0x04000600u)));
    printf("[mtxbal] final pos_sp=%u proj_sp=%u tex_sp=%u\n", pos_level,
           proj_level, (unsigned)g.tex_sp);
}

/* SM64DS_TRI_LOG=1: once, after the first full frame is assembled, the
   per-material picture the RASTER sees -- how many triangles carried each
   TEXIMAGE_PARAM, whether a decoded texture came with it, and the texel
   span of their UVs. A material that binds fine but arrives with a
   one-texel UV span is a texgen problem, not an upload problem. */
static void tri_report() {
    static int on = getenv("SM64DS_TRI_LOG") ? 1 : 0;
    if (!on) return;
    on = 0;                              // one frame is the whole report
    struct Acc {
        int n, textured, tw, th;
        float u0, u1, v0, v1;
    };
    std::map<uint32_t, Acc> acc;
    for (const GxTriangle &t : g.tris) {
        auto it = acc.find(t.dbg_tex);
        if (it == acc.end())
            it = acc.emplace(t.dbg_tex, Acc{0, 0, t.tw, t.th, 1e30f, -1e30f,
                                            1e30f, -1e30f}).first;
        Acc &a = it->second;
        ++a.n;
        if (t.tex) ++a.textured;
        for (int k = 0; k < 3; ++k) {
            if (t.v[k].u < a.u0) a.u0 = t.v[k].u;
            if (t.v[k].u > a.u1) a.u1 = t.v[k].u;
            if (t.v[k].v < a.v0) a.v0 = t.v[k].v;
            if (t.v[k].v > a.v1) a.v1 = t.v[k].v;
        }
    }
    for (const auto &kv : acc)
        printf("[tri] tex=%08x tris=%5d textured=%5d %3dx%-3d "
               "u[%9.2f..%9.2f] v[%9.2f..%9.2f]\n",
               kv.first, kv.second.n, kv.second.textured, kv.second.tw,
               kv.second.th, kv.second.u0, kv.second.u1, kv.second.v0,
               kv.second.v1);
}

/* SM64DS_TEXPX=<frame>: THE DECISION NUMBER FOR THE TITLE'S SCALE, measured on
   the assembled frame rather than argued from matrices. DEFAULT OFF (0).

   A DS 2D-in-3D surface is drawn ONE TEXEL TO ONE PIXEL, and that is a
   property of the finished triangle, not of any one stage: it is the screen
   distance between two vertices divided by the texel distance between the same
   two vertices. Every transform between the record and the raster is already
   folded into both halves, so this number cannot be fooled by a wrong
   projection, a wrong depth, a scale left in the position matrix or a wrong
   quad size -- if it reads 1.00 the art is at the ROM's own proportions and if
   it reads 2.43 the art is 2.43x oversized, whatever the cause.

   DS PIXELS, not host pixels: the interactive tier rasters at 2x, so the raw
   ratio is halved. Reported per bound texture, with the eye-space depth beside
   it, because the depth is the lever the lane was pointed at and this says
   whether it is the one that is off.

   Edges shorter than half a texel are skipped -- they carry no information and
   their ratio is numerically meaningless. A quad whose texel span is zero
   (the untextured backdrop fills) reports no rows at all, which is correct:
   there is no texel to be one pixel. */
static void texpx_report() {
    static int want = -1;
    static unsigned f;
    if (want < 0) {
        const char *e = getenv("SM64DS_TEXPX");
        want = e ? atoi(e) : 0;
    }
    const unsigned this_frame = f++;
    if (want <= 0 || (int)this_frame != want) return;
    struct Acc {
        int n; double lo, hi, sum; int tw, th; double w_lo, w_hi;
    };
    std::map<uint32_t, Acc> acc;
    for (const GxTriangle &t : g.tris) {
        for (int e = 0; e < 3; ++e) {
            const GxVertex &a = t.v[e], &b = t.v[(e + 1) % 3];
            const double du = b.u - a.u, dv = b.v - a.v;
            const double dtex = std::sqrt(du * du + dv * dv);
            if (dtex < 0.5) continue;
            const double dx = b.x - a.x, dy = b.y - a.y;
            const double dpx = std::sqrt(dx * dx + dy * dy) * 0.5;  // host -> DS
            const double r = dpx / dtex;
            auto it = acc.find(t.dbg_tex);
            if (it == acc.end())
                it = acc.emplace(t.dbg_tex, Acc{0, 1e30, -1e30, 0.0, t.tw, t.th,
                                                1e30, -1e30}).first;
            Acc &q = it->second;
            ++q.n; q.sum += r;
            if (r < q.lo) q.lo = r;
            if (r > q.hi) q.hi = r;
            const double w = (a.w + b.w) * 0.5;
            if (w < q.w_lo) q.w_lo = w;
            if (w > q.w_hi) q.w_hi = w;
        }
    }
    printf("[texpx] frame %u: DS PIXELS PER TEXEL per bound texture. 1.00 is "
           "one texel one pixel.\n", this_frame);
    for (const auto &kv : acc)
        printf("[texpx]   tex=%08x %3dx%-3d edges=%5d  px/texel min %6.3f  "
               "max %6.3f  mean %6.3f   w[%8.3f..%8.3f]\n",
               kv.first, kv.second.tw, kv.second.th, kv.second.n,
               kv.second.lo, kv.second.hi, kv.second.sum / kv.second.n,
               kv.second.w_lo, kv.second.w_hi);
    std::fflush(stdout);
}

// One texel coordinate under the DS wrap rules (GBATEK TEXIMAGE_PARAM 16-19):
// repeat clear = CLAMP to the edge texel; repeat set = wrap; flip on top of
// repeat mirrors every other tile. `repeat && !flip` is the exact expression
// the raster used before wrap modes existed, so nothing that binds through
// gx_bind_texture moves a pixel.
static int tex_coord(float f, int size, bool repeat, bool flip) {
    int i = static_cast<int>(std::floor(f));
    if (!repeat) return i < 0 ? 0 : (i >= size ? size - 1 : i);
    if (!flip) {
        i %= size;
        return i < 0 ? i + size : i;
    }
    const int period = size * 2;
    i %= period;
    if (i < 0) i += period;
    return i < size ? i : period - 1 - i;
}

/* n/255.0f for every byte. The texel-modulate step did this divide three
   times per pixel; the table holds the identical float, so the product is
   bit-for-bit what the divide produced. */
struct Inv255 {
    float v[256];
    Inv255() { for (int i = 0; i < 256; ++i) v[i] = i / 255.0f; }
};
static const Inv255 inv255;

/* --- parallel raster -------------------------------------------------------
   The screen is split BY ROW: worker k takes rows k, k+T, k+2T and so on.
   Every row is written by exactly one worker, and every worker walks the whole
   triangle list in submission order, so the depth resolution and the
   translucent blend over the framebuffer happen in the same order, against the
   same pixels, as they did on one thread. The frame that comes out is
   bit-for-bit the frame one thread produced: this partitions the work, it does
   not reorder it. That is the only reason it is allowed to exist here -- the
   raster is the port's reference for what the DS drew.

   SM64DS_RASTER_THREADS=N forces the worker count; 1 turns threading off. */
namespace {

typedef void (*BandFn)(void *, int, int);

struct RasterPool {
    std::vector<std::thread> th;
    std::mutex m;
    std::condition_variable cv_go, cv_done;
    unsigned long long gen = 0;
    int done = 0, n = 1;
    bool stop = false;
    BandFn fn = nullptr;
    void *ctx = nullptr;

    void start(int threads) {
        n = threads;
        for (int i = 1; i < n; ++i) th.emplace_back([this, i] { worker(i); });
    }
    ~RasterPool() {
        {
            std::lock_guard<std::mutex> lk(m);
            stop = true;
            ++gen;
        }
        cv_go.notify_all();
        for (std::thread &t : th)
            if (t.joinable()) t.join();
    }
    void worker(int id) {
        unsigned long long seen = 0;
        for (;;) {
            std::unique_lock<std::mutex> lk(m);
            cv_go.wait(lk, [&] { return gen != seen || stop; });
            if (stop) return;
            seen = gen;
            lk.unlock();
            fn(ctx, id, n);
            lk.lock();
            ++done;
            lk.unlock();
            cv_done.notify_one();
        }
    }
    void run(BandFn f, void *c) {
        {
            std::lock_guard<std::mutex> lk(m);
            fn = f;
            ctx = c;
            done = 0;
            ++gen;
        }
        cv_go.notify_all();
        f(c, 0, n);                       // this thread takes band 0
        std::unique_lock<std::mutex> lk(m);
        cv_done.wait(lk, [&] { return done == n - 1; });
    }
};

int raster_threads() {
    static int n = -1;
    if (n < 0) {
        if (const char *e = getenv("SM64DS_RASTER_THREADS")) {
            n = atoi(e);
        } else {
            unsigned hc = std::thread::hardware_concurrency();
            n = hc ? (int)hc : 1;
            if (n > 8) n = 8;   /* past this the row bands stop paying */
        }
        if (n < 1) n = 1;
    }
    return n;
}

RasterPool &pool(int threads) {
    static RasterPool p;
    static int started = 0;
    if (!started) { started = 1; p.start(threads); }
    return p;
}

/* The 3D coverage mask's storage. Declared out here rather than inside
   gx_render because gx_coverage() below has to reach it and because the raster
   bands are lambdas inside that function; a function-local static would work
   and would read as a private buffer, which it is not. */
uint8_t g_cover[SCREEN_H][SCREEN_W];

}  // namespace

const uint8_t *gx_coverage() { return &g_cover[0][0]; }

void gx_render(Framebuffer &fb) {
    const int tm = frame_ms();
    std::chrono::steady_clock::time_point t_enter;
    if (tm) t_enter = std::chrono::steady_clock::now();
    tri_report();
    texpx_report();
    mat_report();
    mtx_report(false);
    /* Depth clear: 768KB at the window's 2x tier, every frame. 1e30f is not a
       repeating byte pattern so memset cannot do it, but one row can be built
       scalar and the rest copied from it, which is memcpy's problem rather
       than a 196k-iteration scalar loop's. */
    static float depth[SCREEN_H][SCREEN_W];
    for (int x = 0; x < SCREEN_W; ++x) depth[0][x] = 1e30f;
    for (int y = 1; y < SCREEN_H; ++y)
        std::memcpy(depth[y], depth[0], SCREEN_W * sizeof(float));

    /* THE 3D COVERAGE MASK, see gx_coverage() in ntr/gx.h. One byte per pixel,
       set beside every store into fb.px below and cleared here. It is what
       lets the 2D compositor tell a pixel this engine drew from a pixel the
       frame's clear left, which is the whole of "a BG at priority 3 sits
       BEHIND the 3D layer at priority 1".
       It is written from the raster bands, and that is safe for the reason
       the framebuffer itself is: a band owns the rows y == tid (mod nt) and
       no other band touches them. */
    std::memset(g_cover, 0, sizeof g_cover);

    /* --- shadow-polygon (POLYGON_ATTR mode 3) machinery -------------------
       GBATEK's two-step protocol, and the reason a per-pixel stencil bit and
       a per-pixel polygon ID exist at all. The game renders each drop shadow
       as a closed VOLUME twice: first every material's attr set to mode 3 /
       ID 0 / back faces only (func_02046120), then mode 3 / ID nonzero /
       front faces only (func_02046088). The hardware's reading:

         ID 0 (the mask):  where the depth test FAILS, set the pixel's
                           stencil bit. No colour, no depth. A back face
                           failing the depth test means the surface in the
                           framebuffer is in front of the volume's far wall.
         ID 1..63 (the draw): where the stencil bit is set, CLEAR it; then,
                           if the depth test passes (the near wall is in
                           front of that same surface -- so the surface is
                           inside the volume) and the pixel's recorded
                           polygon ID differs from the shadow's (a caster
                           does not shadow itself), blend the shadow colour
                           at the polygon's alpha. Depth is never written.

       Rasterising those two passes as ordinary geometry is exactly the
       wave-4 cone: the volume's own walls drawn as a column under the actor
       (run linkw, w4a review pinned it). The buffers clear per frame and the
       whole apparatus stays untouched -- one predictable branch -- for any
       frame that submits no mode-3 polygon. */
    static uint8_t stencil[SCREEN_H][SCREEN_W];
    static uint8_t attrid[SCREEN_H][SCREEN_W];
    bool have_shadow = false;
    for (const GxTriangle &t : g.tris)
        if (t.mode == 3) { have_shadow = true; break; }
    if (have_shadow) {
        std::memset(stencil, 0, sizeof stencil);
        /* 0 is the clear plane's polygon ID (CLEAR_COLOR bits 24-29 reset
           value); pixels no opaque polygon reaches keep it. */
        std::memset(attrid, 0, sizeof attrid);
    }

    /* SM64DS_TEX_ONLY=<hex teximage>: draw only the polygons that were
       bound to that texture, so a material can be located on screen
       without guessing from colour. */
    static uint32_t only = [] {
        const char *o = getenv("SM64DS_TEX_ONLY");
        return o ? static_cast<uint32_t>(strtoul(o, nullptr, 16)) : 0u;
    }();

    /* SM64DS_PROBE_PX=x,y: every triangle that COVERS that pixel, with the
       decision the raster made about it. One clear-colour pixel in a
       finished frame is the whole question "which polygon should have been
       here", and this answers it without guessing from the picture. */
    static int probe_x = -1, probe_y = -1;
    {
        static int once = 0;
        if (!once) {
            once = 1;
            if (const char *e = getenv("SM64DS_PROBE_PX"))
                sscanf(e, "%d,%d", &probe_x, &probe_y);
        }
    }

    /* The probe is a reporting pass of its own, on this thread: it prints, and
       printing once per covering triangle is only meaningful in submission
       order from one place. */
    if (probe_x >= 0) {
        for (const GxTriangle &t : g.tris) {
            if (only && t.dbg_tex != only) continue;
            const GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
            const float area =
                (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
            const float px = probe_x + 0.5f, py = probe_y + 0.5f;
            const float w0 = ((b.x - a.x) * (py - a.y) - (b.y - a.y) * (px - a.x));
            const float w1 = ((c.x - b.x) * (py - b.y) - (c.y - b.y) * (px - b.x));
            const float w2 = ((a.x - c.x) * (py - c.y) - (a.y - c.y) * (px - c.x));
            const bool cover = (w0 >= 0 && w1 >= 0 && w2 >= 0) ||
                               (w0 <= 0 && w1 <= 0 && w2 <= 0);
            const bool bf = area > 0.0f;
            const bool culled = (bf && !(t.cull & 1)) || (!bf && !(t.cull & 2));
            if (cover)
                printf("[probe] COVER (%.3f,%.3f,%.5f/%.4f) (%.3f,%.3f,%.5f/"
                       "%.4f) (%.3f,%.3f,%.5f/%.4f) area %.4g cull %u%s%s "
                       "dbg %08x\n",
                       a.x, a.y, a.z, a.w, b.x, b.y, b.z, b.w, c.x, c.y, c.z,
                       c.w, area, t.cull,
                       std::fabs(area) < 1e-6f ? " DEGENERATE" : "",
                       culled ? " CULLED" : "", t.dbg_tex);
        }
    }

    /* One row band. tid picks the rows: tid, tid+nt, tid+2nt...
       TWO PASSES, the hardware's own order: every opaque polygon first,
       then the translucent ones, submission order kept within each pass.
       The game leans on this -- the castle moat's water submits before
       the terrain under it, and in one pass the opaque moat bed painted
       over the already-blended surface. Rows are disjoint across threads,
       so each thread runs both passes over its own rows and never sees
       another thread's pixels. */
    auto band = [&](int tid, int nt) {
    bool prev_mask = false;
    for (int pass = 0; pass < 2; ++pass)
    for (const GxTriangle &t : g.tris) {
        if (static_cast<int>(t.translucent) != pass) continue;
        if (only && t.dbg_tex != only) continue;
        if (have_shadow && pass == 1) {
            /* The stencil clears when a NEW mask group begins -- a mask
               polygon arriving after any non-mask polygon -- so one
               volume's leftover bits cannot leak into the next volume's
               draw. Every band walks the same list in the same order and
               clears only its own rows, so this is the single-thread
               semantics exactly. */
            const bool is_mask = t.mode == 3 && t.polyid == 0;
            if (is_mask && !prev_mask)
                for (int y = tid; y < SCREEN_H; y += nt)
                    std::memset(stencil[y], 0, SCREEN_W);
            prev_mask = is_mask;
        }
        const GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
        const float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (std::fabs(area) < 1e-6f) continue;

        // Back-face culling per POLYGON_ATTR bits 6-7 (bit 6 renders the back
        // surface, bit 7 the front; stage meshes use double-sided ground).
        // Screen Y is flipped relative to DS space, so a front face is
        // clockwise here (negative area).
        const bool backface = area > 0.0f;
        if (backface && !(t.cull & 1)) continue;
        if (!backface && !(t.cull & 2)) continue;

        int minx = static_cast<int>(std::floor(std::fmin(a.x, std::fmin(b.x, c.x))));
        int maxx = static_cast<int>(std::ceil(std::fmax(a.x, std::fmax(b.x, c.x))));
        int miny = static_cast<int>(std::floor(std::fmin(a.y, std::fmin(b.y, c.y))));
        int maxy = static_cast<int>(std::ceil(std::fmax(a.y, std::fmax(b.y, c.y))));
        if (minx < 0) minx = 0;
        if (miny < 0) miny = 0;
        if (maxx > active_w - 1) maxx = active_w - 1;
        if (maxy > active_h - 1) maxy = active_h - 1;

        /* EVERYTHING BELOW THAT DOES NOT DEPEND ON THE PIXEL IS COMPUTED ONCE.
           The edge functions were three full expressions per pixel, each
           ending in a divide by `area`; the 1/w reciprocals were three fabs,
           three compares and three divides per pixel for values that belong
           to the triangle; and the vertex colours were converted from bytes to
           float per channel per pixel. The arithmetic is left in exactly the
           order it was written -- same operands, same operations, so the same
           floats come out -- only the loop level it happens at moves. */
        const float eax = b.x - a.x, eay = b.y - a.y;
        const float ebx = c.x - b.x, eby = c.y - b.y;
        const float ecx = a.x - c.x, ecy = a.y - c.y;
        const float iwa = (std::fabs(a.w) > 1e-6f) ? 1.0f / a.w : 0.0f;
        const float iwb = (std::fabs(b.w) > 1e-6f) ? 1.0f / b.w : 0.0f;
        const float iwc = (std::fabs(c.w) > 1e-6f) ? 1.0f / c.w : 0.0f;
        /* NOTE the UV terms below stay written as l0 * a.u * iwa. Folding
           a.u * iwa out to the triangle would regroup the multiply, and float
           multiplication does not associate -- (l0*a.u)*iwa and l0*(a.u*iwa)
           are different numbers. Only the reciprocal itself is hoisted. */
        const bool textured = t.tex && t.tw > 0 && t.th > 0;
        const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
        const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
        const float acol[3] = {(float)((a.color >> 16) & 0xFF),
                               (float)((a.color >> 8) & 0xFF),
                               (float)(a.color & 0xFF)};
        const float bcol[3] = {(float)((b.color >> 16) & 0xFF),
                               (float)((b.color >> 8) & 0xFF),
                               (float)(b.color & 0xFF)};
        const float ccol[3] = {(float)((c.color >> 16) & 0xFF),
                               (float)((c.color >> 8) & 0xFF),
                               (float)(c.color & 0xFF)};
        const uint32_t poly_a = (t.alpha >= 31 || t.alpha == 0) ? 31u : t.alpha;

        /* first row of this band at or after miny */
        const int y_first = miny + (((tid - miny) % nt) + nt) % nt;

        if (t.mode == 3) {
            /* Shadow polygons, the two-step protocol from the block comment
               at the buffers above. Both loops share the standard loop's
               coverage and barycentric arithmetic -- same expressions, same
               order -- and neither ever writes depth. */
            if (t.polyid == 0) {
                /* the mask: set stencil where the depth test FAILS; no
                   colour, no depth, no texture */
                for (int y = y_first; y <= maxy; y += nt) {
                    const float py = y + 0.5f;
                    const float r0 = eax * (py - a.y);
                    const float r1 = ebx * (py - b.y);
                    const float r2 = ecx * (py - c.y);
                    const float *drow = depth[y];
                    uint8_t *srow = stencil[y];
                    for (int x = minx; x <= maxx; ++x) {
                        const float px = x + 0.5f;
                        const float n0 = r0 - eay * (px - a.x);
                        const float n1 = r1 - eby * (px - b.x);
                        const float n2 = r2 - ecy * (px - c.x);
                        if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) ||
                              (n0 <= 0 && n1 <= 0 && n2 <= 0)))
                            continue;
                        const float w0 = n0 / area, w1 = n1 / area,
                                    w2 = n2 / area;
                        const float l0 = w1, l1 = w2, l2 = w0;
                        const float z = l0 * a.z + l1 * b.z + l2 * c.z;
                        if (z >= drow[x]) srow[x] = 1;
                    }
                }
            } else {
                /* the drawn shadow: examine stencilled pixels, clear the
                   bit whether it draws or not, blend where the depth test
                   passes and the recorded polygon ID differs */
                for (int y = y_first; y <= maxy; y += nt) {
                    const float py = y + 0.5f;
                    const float r0 = eax * (py - a.y);
                    const float r1 = ebx * (py - b.y);
                    const float r2 = ecx * (py - c.y);
                    const float *drow = depth[y];
                    uint8_t *srow = stencil[y];
                    const uint8_t *irow = attrid[y];
                    uint32_t *frow = fb.px[y];
                    for (int x = minx; x <= maxx; ++x) {
                        const float px = x + 0.5f;
                        const float n0 = r0 - eay * (px - a.x);
                        const float n1 = r1 - eby * (px - b.x);
                        const float n2 = r2 - ecy * (px - c.x);
                        if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) ||
                              (n0 <= 0 && n1 <= 0 && n2 <= 0)))
                            continue;
                        if (!srow[x]) continue;
                        srow[x] = 0;
                        const float w0 = n0 / area, w1 = n1 / area,
                                    w2 = n2 / area;
                        const float l0 = w1, l1 = w2, l2 = w0;
                        const float z = l0 * a.z + l1 * b.z + l2 * c.z;
                        if (z >= drow[x]) continue;
                        if (irow[x] == t.polyid) continue;
                        uint32_t texel = 0xFFFFFFFFu;
                        if (textured) {
                            const float iw = l0 * iwa + l1 * iwb + l2 * iwc;
                            float uu, vv;
                            if (iw > 1e-9f) {
                                uu = (l0 * a.u * iwa + l1 * b.u * iwb +
                                      l2 * c.u * iwc) / iw;
                                vv = (l0 * a.v * iwa + l1 * b.v * iwb +
                                      l2 * c.v * iwc) / iw;
                            } else {
                                uu = l0 * a.u + l1 * b.u + l2 * c.u;
                                vv = l0 * a.v + l1 * b.v + l2 * c.v;
                            }
                            const int ui = tex_coord(uu, t.tw, rep_s, flip_s);
                            const int vi = tex_coord(vv, t.th, rep_t, flip_t);
                            texel = t.tex[vi * t.tw + ui];
                            if ((texel >> 24) == 0) continue;
                        }
                        auto ch = [&](int k, int sh) {
                            const float v = l0 * acol[k] + l1 * bcol[k] +
                                            l2 * ccol[k];
                            const float m = inv255.v[(texel >> sh) & 0xFF];
                            const int i = static_cast<int>(v * m + 0.5f);
                            return static_cast<uint32_t>(
                                i < 0 ? 0 : (i > 255 ? 255 : i));
                        };
                        const uint32_t tex_a = texel >> 24;
                        const uint32_t sa = (poly_a * tex_a + 127) / 255;
                        const uint32_t dst = frow[x];
                        auto bl = [&](int k, int sh) {
                            const uint32_t s = ch(k, sh);
                            const uint32_t d = (dst >> sh) & 0xFF;
                            return ((s * sa + d * (31 - sa)) / 31) & 0xFF;
                        };
                        frow[x] = 0xFF000000u | (bl(0, 16) << 16) |
                                  (bl(1, 8) << 8) | bl(2, 0);
                        g_cover[y][x] = 1;
                    }
                }
            }
            continue;
        }
        for (int y = y_first; y <= maxy; y += nt) {
            const float py = y + 0.5f;
            /* the half of each edge function that only moves with the row */
            const float r0 = eax * (py - a.y);
            const float r1 = ebx * (py - b.y);
            const float r2 = ecx * (py - c.y);
            float *drow = depth[y];
            uint32_t *frow = fb.px[y];
            uint8_t *irow = attrid[y];
            for (int x = minx; x <= maxx; ++x) {
                const float px = x + 0.5f;
                /* Coverage is decided on the undivided edge functions. The
                   test asks whether all three share a sign, and dividing all
                   three by the same non-zero area cannot change that whichever
                   way the area points -- so the three divides only have to
                   happen for pixels that are actually inside. */
                const float n0 = r0 - eay * (px - a.x);
                const float n1 = r1 - eby * (px - b.x);
                const float n2 = r2 - ecy * (px - c.x);
                // Accept either winding; back-face culling is a POLYGON_ATTR job.
                if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) || (n0 <= 0 && n1 <= 0 && n2 <= 0)))
                    continue;
                const float w0 = n0 / area, w1 = n1 / area, w2 = n2 / area;
                const float l0 = w1, l1 = w2, l2 = w0;   // barycentric for a, b, c
                const float z = l0 * a.z + l1 * b.z + l2 * c.z;
                if (z >= drow[x]) continue;
                // Depth is written only after the texel passes the alpha test
                // below -- a transparent texel must not occlude what is behind it.
                // Texture first; the vertex colour modulates it. UVs are
                // perspective-corrected via 1/w interpolation; with w == 1
                // everywhere (the ortho harnesses) the math reduces exactly
                // to the old affine lerp.
                uint32_t texel = 0xFFFFFFFFu;
                if (textured) {
                    const float iw = l0 * iwa + l1 * iwb + l2 * iwc;
                    float uu, vv;
                    if (iw > 1e-9f) {
                        uu = (l0 * a.u * iwa + l1 * b.u * iwb + l2 * c.u * iwc) / iw;
                        vv = (l0 * a.v * iwa + l1 * b.v * iwb + l2 * c.v * iwc) / iw;
                    } else {
                        uu = l0 * a.u + l1 * b.u + l2 * c.u;
                        vv = l0 * a.v + l1 * b.v + l2 * c.v;
                    }
                    const int ui = tex_coord(uu, t.tw, rep_s, flip_s);
                    const int vi = tex_coord(vv, t.th, rep_t, flip_t);
                    texel = t.tex[vi * t.tw + ui];
                    if ((texel >> 24) == 0) continue;      // transparent texel
                }

                // Round, do not truncate: barycentrics sum to 0.9999 rather than
                // exactly 1, and a truncating cast bands a flat surface 255/254.
                auto ch = [&](int k, int sh) {
                    const float v = l0 * acol[k] + l1 * bcol[k] + l2 * ccol[k];
                    const float m = inv255.v[(texel >> sh) & 0xFF];
                    const int i = static_cast<int>(v * m + 0.5f);
                    return static_cast<uint32_t>(i < 0 ? 0 : (i > 255 ? 255 : i));
                };
                /* effective alpha = poly attr alpha combined with the
                   TEXEL alpha (A3I5/A5I3 gradients -- the grass-fade
                   strips render solid without it) */
                const uint32_t tex_a = texel >> 24;            /* 0..255 */
                const uint32_t sa = (poly_a * tex_a + 127) / 255; /* 0..31 */
                if (sa >= 31) {
                    /* opaque (the DS treats attr alpha 0 as wire/opaque
                       depending on mode; opaque is the safe read) */
                    drow[x] = z;
                    frow[x] = 0xFF000000u | (ch(0, 16) << 16) | (ch(1, 8) << 8)
                              | ch(2, 0);
                    g_cover[y][x] = 1;
                    /* the ID travels with the depth write so a shadow can
                       recognise its own caster; one predictable branch on
                       shadow-free frames, and the colour above is untouched
                       either way */
                    if (have_shadow) irow[x] = t.polyid;
                } else {
                    /* translucent: blend over the framebuffer, keep depth
                       (DS translucent polys depth-test but do not write) */
                    const uint32_t dst = frow[x];
                    auto bl = [&](int k, int sh) {
                        const uint32_t s = ch(k, sh);
                        const uint32_t d = (dst >> sh) & 0xFF;
                        return ((s * sa + d * (31 - sa)) / 31) & 0xFF;
                    };
                    frow[x] = 0xFF000000u | (bl(0, 16) << 16) | (bl(1, 8) << 8)
                              | bl(2, 0);
                    g_cover[y][x] = 1;
                }
            }
        }
    }
    };  // band

    /* Small scenes (the smokes, a single model) are not worth waking anyone
       up for; the handover costs more than the fill. */
    const int nt = (g.tris.size() < 256) ? 1 : raster_threads();
    if (nt <= 1) {
        band(0, 1);
    } else {
        typedef decltype(band) B;
        pool(nt).run([](void *p, int tid, int n) { (*static_cast<B *>(p))(tid, n); },
                     &band);
    }

    if (tm) {
        using clk = std::chrono::steady_clock;
        using ms = std::chrono::duration<double, std::milli>;
        const clk::time_point t_exit = clk::now();
        static clk::time_point prev;
        static double acc_raster, acc_frame;
        static long long acc_tris;
        static int n;
        acc_raster += ms(t_exit - t_enter).count();
        if (prev.time_since_epoch().count()) acc_frame += ms(t_exit - prev).count();
        prev = t_exit;
        acc_tris += static_cast<long long>(g.tris.size());
        if (++n >= 30) {
            fprintf(stderr, "[perf] frame %6.2fms raster %6.2fms tris %6lld "
                    "decodes %.1f\n", acc_frame / n, acc_raster / n,
                    acc_tris / n, (double)g_tex_decodes / n);
            acc_raster = acc_frame = 0; acc_tris = 0; n = 0; g_tex_decodes = 0;
        }
    }
}

}  // namespace ntr

