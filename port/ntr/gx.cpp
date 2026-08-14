// DS geometry engine: command decode, matrix stacks, vertex assembly, raster.
//
// Command encoding and parameter counts from GBATEK. Fixed-point inputs are 4.12
// (fx32); the transform runs in float, which is what any host renderer would do
// and is not a fidelity question for geometry this size.

#include "ntr/gx.h"

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
    int proj_sp = 0, pos_sp = 0;

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

    int vp_x = 0, vp_y = 0, vp_w = SCREEN_W, vp_h = SCREEN_H;

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
GxVertex project(int16_t x, int16_t y, int16_t z) {
    const Vec4 v{x * FX12, y * FX12, z * FX12, 1.0f};
    const Vec4 c = mul(mul(v, current_pos()), g.proj);

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

void exec(uint8_t cmd, const uint32_t *p, int np) {
    (void)np;
    switch (cmd) {
        case 0x00: break;                                        // NOP
        case 0x10: g.mode = p[0] & 3; break;                     // MTX_MODE
        case 0x11:                                               // MTX_PUSH
            if (g.mode == MTX_PROJ) { if (g.proj_sp < 1) g.proj_stack[g.proj_sp++] = g.proj; }
            else if (g.pos_sp < 31) { g.pos_stack[g.pos_sp] = g.pos; g.vec_stack[g.pos_sp] = g.vec; ++g.pos_sp; }
            break;
        case 0x12: {                                             // MTX_POP
            int n = static_cast<int32_t>(p[0] << 26) >> 26;      // signed 6-bit
            if (g.mode == MTX_PROJ) { if (g.proj_sp > 0) g.proj = g.proj_stack[--g.proj_sp]; }
            else { g.pos_sp -= n; if (g.pos_sp < 0) g.pos_sp = 0;
                   if (g.pos_sp < 31) { g.pos = g.pos_stack[g.pos_sp]; g.vec = g.vec_stack[g.pos_sp]; } }
            break;
        }
        case 0x13: {                                             // MTX_STORE
            ++g_store_count;
            const int i = p[0] & 31;
            if (g.mode == MTX_PROJ) g.proj_stack[0] = g.proj;
            else { g.pos_stack[i] = g.pos; g.vec_stack[i] = g.vec; }
            break;
        }
        case 0x14: {                                             // MTX_RESTORE
            const int i = p[0] & 31;
            if (g.mode == MTX_PROJ) g.proj = g.proj_stack[0];
            else { g.pos = g.pos_stack[i]; g.vec = g.vec_stack[i]; }
            break;
        }
        case 0x15:                                               // MTX_IDENTITY
            if (g.mode == MTX_PROJ) g.proj = Mat::identity();
            else if (g.mode == MTX_TEX) g.tex = Mat::identity();
            else { g.pos = Mat::identity(); if (g.mode == MTX_POSVEC) g.vec = Mat::identity(); }
            break;
        case 0x16: case 0x17: {                                  // MTX_LOAD_4x4 / 4x3
            Mat m; load_mtx(m, p, cmd == 0x16 ? 16 : 12);
            if (g.mode == MTX_PROJ) g.proj = m;
            else if (g.mode == MTX_TEX) g.tex = m;
            else { g.pos = m; if (g.mode == MTX_POSVEC) g.vec = m; }
            break;
        }
        case 0x18: case 0x19: case 0x1A: {                       // MTX_MULT_4x4 / 4x3 / 3x3
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

            float c[3] = {g.emission[0], g.emission[1], g.emission[2]};
            for (int i = 0; i < 4; ++i) {
                if (!((g.light_mask >> i) & 1)) continue;
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
        case 0x29: g.poly_attr = p[0]; break;                    // POLYGON_ATTR
        case 0x2A: gx_teximage_param(p[0]); break;               // TEXIMAGE_PARAM
        case 0x2B: gx_pltt_base(p[0]); break;                    // PLTT_BASE
        case 0x30: {                                             // DIF_AMB
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
            g.emission[0] = ((p[0] >> 16) & 0x1F) / 31.0f;
            g.emission[1] = ((p[0] >> 21) & 0x1F) / 31.0f;
            g.emission[2] = ((p[0] >> 26) & 0x1F) / 31.0f;
            break;
        }
        case 0x32: {                                             // LIGHT_VECTOR
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
            g.vp_x = x1 * SCREEN_W / 256;
            g.vp_y = y1 * SCREEN_H / 192;
            g.vp_w = (x2 - x1 + 1) * SCREEN_W / 256;
            g.vp_h = (y2 - y1 + 1) * SCREEN_H / 192;
            break;
        }
        default: break;
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
void gx_write_fifo(uint32_t word) { gx_stream_note(word); feed(word); }

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

void gx_write_port(uint32_t addr, uint32_t value) {
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
    std::vector<GxVertex> strip = std::move(g.strip);
    std::vector<GxTriangle> tris = std::move(g.tris);
    strip.clear();
    tris.clear();
    g = State{};
    g.strip = std::move(strip);
    g.tris = std::move(tris);
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
    g_queued = g_qpos = g_have = 0;
    g_port_cmd = 0; g_port_have = 0;
}

void gx_debug_proj(float out[16]) {
    for (int i = 0; i < 16; ++i) out[i] = g.proj.m[i];
}

const GxTriangle *gx_polygons(size_t &count) {
    count = g.tris.size();
    return g.tris.empty() ? nullptr : g.tris.data();
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

/* --- submitted-geometry trace ----------------------------------------------
   SM64DS_GX_TRACE=<path> writes the COMPLETE triangle list the raster is about
   to consume. It exists to answer one question, and only that question: when
   two builds of the same source produce different pixels, did the GAME submit
   different geometry, or did the RASTER turn identical geometry into different
   pixels?

   So the trace is deliberately address-free. Not one host pointer reaches the
   file: triangles are named by their index in submission order, and a bound
   texture is named by a hash of its DECODED TEXELS rather than by the heap
   address the decode happens to sit at. A trace that carried pointers would
   differ on every line the moment the image layout moved, which is exactly the
   comparison it is meant to make.

   Floats print as %a -- the exact bits, not a rounded decimal. A one-ulp
   difference in a vertex position is the whole answer here and %f hides it.

   FRAME SELECTION. The file is truncated and rewritten on every render, so
   when the run ends it holds the LAST frame -- which is the frame walk_window
   dumps its BMP from. SM64DS_GX_TRACE_FRAME=<n> pins it to one render index
   instead (the index counts gx_render calls from 0 and is printed in the
   header). SM64DS_GX_HASH=1 prints the per-frame triangle-count and list hash
   on stderr without writing any file, which is the cheap way to find the FIRST
   frame at which two runs diverge before dumping that frame in full. */
namespace {

uint32_t fnv_bytes(uint32_t h, const void *p, size_t n) {
    const unsigned char *b = static_cast<const unsigned char *>(p);
    for (size_t i = 0; i < n; ++i) h = (h ^ b[i]) * 16777619u;
    return h;
}

/* Content identity of a bound texture. Cached per dump against the pointer,
   because the pointer is a stable key WITHIN one frame and the decoded texels
   never change once cached -- but the pointer itself never leaves this
   function. gx_invalidate_textures can recycle an address across frames, so
   the cache is rebuilt each frame rather than kept. */
struct TexHashCache {
    std::map<const uint32_t *, uint32_t> m;
    uint32_t of(const GxTriangle &t) {
        if (!t.tex || t.tw <= 0 || t.th <= 0) return 0;
        auto it = m.find(t.tex);
        if (it != m.end()) return it->second;
        const uint32_t h = fnv_bytes(2166136261u, t.tex,
                                     static_cast<size_t>(t.tw) * t.th *
                                         sizeof(uint32_t));
        m.emplace(t.tex, h);
        return h;
    }
};

/* Every field the raster reads, in order, with the texture reduced to its
   content hash. This is the exact input the rasteriser is a pure function of,
   so two runs with equal hashes cannot produce different pixels. */
uint32_t tri_list_hash(TexHashCache &tc) {
    uint32_t h = 2166136261u;
    for (const GxTriangle &t : g.tris) {
        for (int k = 0; k < 3; ++k) {
            const GxVertex &v = t.v[k];
            h = fnv_bytes(h, &v.x, sizeof v.x);
            h = fnv_bytes(h, &v.y, sizeof v.y);
            h = fnv_bytes(h, &v.z, sizeof v.z);
            h = fnv_bytes(h, &v.w, sizeof v.w);
            h = fnv_bytes(h, &v.u, sizeof v.u);
            h = fnv_bytes(h, &v.v, sizeof v.v);
            h = fnv_bytes(h, &v.color, sizeof v.color);
        }
        const uint32_t th_ = tc.of(t);
        h = fnv_bytes(h, &th_, sizeof th_);
        h = fnv_bytes(h, &t.tw, sizeof t.tw);
        h = fnv_bytes(h, &t.th, sizeof t.th);
        h = fnv_bytes(h, &t.cull, sizeof t.cull);
        h = fnv_bytes(h, &t.alpha, sizeof t.alpha);
        h = fnv_bytes(h, &t.wrap, sizeof t.wrap);
        h = fnv_bytes(h, &t.translucent, sizeof t.translucent);
        h = fnv_bytes(h, &t.mode, sizeof t.mode);
        h = fnv_bytes(h, &t.polyid, sizeof t.polyid);
        h = fnv_bytes(h, &t.dbg_tex, sizeof t.dbg_tex);
    }
    return h;
}

void gx_trace(int index) {
    static int on = -1;
    static const char *path;
    static int want_frame = -1;
    static int hash_on = 0;
    if (on < 0) {
        path = getenv("SM64DS_GX_TRACE");
        on = path ? 1 : 0;
        if (const char *f = getenv("SM64DS_GX_TRACE_FRAME")) want_frame = atoi(f);
        hash_on = getenv("SM64DS_GX_HASH") ? 1 : 0;
    }
    if (!on && !hash_on) return;

    TexHashCache tc;
    if (hash_on)
        fprintf(stderr, "[gxh] f%d tris=%u h=%08x\n", index,
                static_cast<unsigned>(g.tris.size()), tri_list_hash(tc));
    if (!on) return;
    if (want_frame >= 0 && index != want_frame) return;

    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "# gx trace: render index %d, %u triangles, list hash %08x\n",
            index, static_cast<unsigned>(g.tris.size()), tri_list_hash(tc));
    fprintf(f, "# t <i> pass=<translucent> mode= id= cull= alpha= wrap= "
               "dbg=<TEXIMAGE_PARAM> tex=<texel content hash> tw= th=\n");
    fprintf(f, "# v<k> x y z w u v (all %%a) c=<AARRGGBB>\n");
    for (size_t i = 0; i < g.tris.size(); ++i) {
        const GxTriangle &t = g.tris[i];
        fprintf(f,
                "t %u pass=%u mode=%u id=%u cull=%u alpha=%u wrap=%u "
                "dbg=%08x tex=%08x tw=%d th=%d\n",
                static_cast<unsigned>(i), t.translucent, t.mode, t.polyid,
                t.cull, t.alpha, t.wrap, t.dbg_tex, tc.of(t), t.tw, t.th);
        for (int k = 0; k < 3; ++k) {
            const GxVertex &v = t.v[k];
            fprintf(f, " v%d %a %a %a %a %a %a c=%08x\n", k, v.x, v.y, v.z,
                    v.w, v.u, v.v, v.color);
        }
    }
    fclose(f);
}

}  // namespace

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

}  // namespace

void gx_render(Framebuffer &fb) {
    const int tm = frame_ms();
    std::chrono::steady_clock::time_point t_enter;
    if (tm) t_enter = std::chrono::steady_clock::now();
    tri_report();
    /* the submitted list, before anything rasterises it (see gx_trace) */
    static int g_render_index = 0;
    gx_trace(g_render_index++);
    /* Depth clear: 768KB at the window's 2x tier, every frame. 1e30f is not a
       repeating byte pattern so memset cannot do it, but one row can be built
       scalar and the rest copied from it, which is memcpy's problem rather
       than a 196k-iteration scalar loop's. */
    static float depth[SCREEN_H][SCREEN_W];
    for (int x = 0; x < SCREEN_W; ++x) depth[0][x] = 1e30f;
    for (int y = 1; y < SCREEN_H; ++y)
        std::memcpy(depth[y], depth[0], SCREEN_W * sizeof(float));

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
        if (maxx > SCREEN_W - 1) maxx = SCREEN_W - 1;
        if (maxy > SCREEN_H - 1) maxy = SCREEN_H - 1;

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

