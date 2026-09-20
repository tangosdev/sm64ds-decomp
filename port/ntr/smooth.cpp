// Curved PN-triangle subdivision -- the pure geometry kernel.
// Contract, guarantees and the reason this is view-space-only: smooth.h.
//
// PORTED FROM app/src/subdiv.rs in the tangOS-SM64DS project
// (393 lines, unit-tested, parked; docs/SMOOTHING.md explains it). The maths
// is line for line the same. Three things are deliberately different, and each
// one is because this runs inside a live frame instead of an offline export:
//
//  1. NORMAL SOURCES. The Rust welded smooth normals out of geometry
//     (welded_normals(): quantise the position, accumulate area-weighted face
//     normals, normalise) because a GLB part carries no normals of its own.
//     The DS display list DOES carry them: the NORMAL command (0x21) is how
//     the hardware lights a vertex, and gx.cpp already turns it into a
//     view-space unit normal. Those are the normals Nintendo authored for
//     exactly this shading, so this kernel takes them as input and does no
//     welding at all. That removes the weld pass, the hash map, the per-batch
//     buffer and -- the part that actually mattered -- every question about
//     draw order, because nothing has to be held back to the end of a model.
//     Welding stays possible: the kernel takes normals per call, so a caller
//     that wants welded ones can compute them and pass them instead.
//
//  2. NO OUTPUT WELD. The Rust deduplicated output vertices by quantised
//     (position, uv, bone) so the exported index buffer stayed small. There is
//     no index buffer here -- the geometry stage emits independent triangles,
//     exactly as the DS display lists do -- so the dedup map is gone and with
//     it its per-frame allocation. Shared edges still land on the same curve
//     (see the edge-control-point argument in the header), which is the
//     property the dedup was protecting.
//
//  3. NO BONES. The Rust picked one bone per new vertex (pick_bone, ties to
//     the smaller bone id so a shared edge midpoint resolved the same way in
//     both neighbours). Here the bone has ALREADY been applied: the position
//     arrives after the DS POSITION matrix, which is the bone matrix a
//     MTX_RESTORE put there. The tie-break has no work left to do, and its
//     job -- "an edge point must come out identical in both triangles that
//     share the edge" -- is now a purely geometric property of the edge
//     control points, which is what the selftest measures instead.
//
// THE SUBDIVISION RULE (which triangles get curved), all of it data:
//   * No authored normal on a corner  -> leave alone. An unlit polygon (the
//     HUD's own 3D geometry, a billboard, anything drawn with COLOR instead
//     of NORMAL) has no surface to curve and must come out exactly as it went
//     in.
//   * All three normals agree         -> leave alone. The patch would be flat
//     anyway; skipping is not an approximation of that, it is the same answer
//     for no work, and it is what keeps a lone flat quad (a coin, a sign, a
//     stage face) bit-identical.
//   * Tightest implied curvature radius above max_radius, or longest edge
//     above max_edge -> leave alone. This is the pillow guard. PN lifts the
//     centre of a large, gently-curved face, which reads as a bulge in stage
//     ground where it reads as roundness on a character. Curvature radius
//     (edge length / normal turn across that edge) is the scale-free way to
//     tell those apart: a faceted sphere turns its normals a lot over a short
//     edge, a rolling hill turns them a little over a long one.

#include "ntr/smooth.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <map>
#include <vector>

namespace ntr {
namespace {

inline void v_sub(const float a[3], const float b[3], float o[3]) {
    o[0] = a[0] - b[0]; o[1] = a[1] - b[1]; o[2] = a[2] - b[2];
}
inline float v_dot(const float a[3], const float b[3]) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// The Rust `edge()`: the control point one third along i->j, pulled back onto
// i's tangent plane. With ni perpendicular to (pj - pi) -- a flat face -- the
// pull-back term is zero and this is the flat cubic's own control point,
// which is why flat input stays exactly flat.
void pn_edge(const float pi[3], const float pj[3], const float ni[3],
             float out[3]) {
    float d[3];
    v_sub(pj, pi, d);
    const float w = v_dot(d, ni);
    for (int k = 0; k < 3; ++k)
        out[k] = (pi[k] * 2.0f + pj[k] - ni[k] * w) * (1.0f / 3.0f);
}

void unit_normal(const SmoothVertex &v, float out[3]) {
    const float l2 = v.nx * v.nx + v.ny * v.ny + v.nz * v.nz;
    if (l2 > 1e-16f) {
        const float inv = 1.0f / sqrtf(l2);
        out[0] = v.nx * inv; out[1] = v.ny * inv; out[2] = v.nz * inv;
    } else {
        out[0] = out[1] = out[2] = 0.0f;
    }
}

// Interpolate a packed 0xAARRGGBB colour linearly in barycentrics, byte by
// byte, the same rounding gx.cpp's own clip_lerp uses. At a corner (one weight
// 1, the others 0) this returns that corner's colour bit for bit.
uint32_t bary_color(uint32_t c0, uint32_t c1, uint32_t c2,
                    float a, float b, float c) {
    uint32_t out = 0;
    for (int s = 0; s < 32; s += 8) {
        const float ch = (float)((c0 >> s) & 0xFF) * a
                       + (float)((c1 >> s) & 0xFF) * b
                       + (float)((c2 >> s) & 0xFF) * c;
        const int i = (int)(ch + 0.5f);
        out |= (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i)) << s;
    }
    return out;
}

}  // namespace

void smooth_build_patch(const SmoothVertex &p1, const SmoothVertex &p2,
                        const SmoothVertex &p3, SmoothPatch &out) {
    const float P1[3] = {p1.x, p1.y, p1.z};
    const float P2[3] = {p2.x, p2.y, p2.z};
    const float P3[3] = {p3.x, p3.y, p3.z};
    float N1[3], N2[3], N3[3];
    unit_normal(p1, N1);
    unit_normal(p2, N2);
    unit_normal(p3, N3);

    for (int k = 0; k < 3; ++k) {
        out.b[0][k] = P1[k];      // b300
        out.b[1][k] = P2[k];      // b030
        out.b[2][k] = P3[k];      // b003
    }
    pn_edge(P1, P2, N1, out.b[3]);   // b210
    pn_edge(P2, P1, N2, out.b[4]);   // b120
    pn_edge(P2, P3, N2, out.b[5]);   // b021
    pn_edge(P3, P2, N3, out.b[6]);   // b012
    pn_edge(P1, P3, N1, out.b[7]);   // b201
    pn_edge(P3, P1, N3, out.b[8]);   // b102

    // The standard PN centre lift: E is the mean of the six edge points, V the
    // vertex centroid, b111 = E + (E - V)/2.
    for (int k = 0; k < 3; ++k) {
        const float e = (out.b[3][k] + out.b[4][k] + out.b[5][k]
                       + out.b[6][k] + out.b[7][k] + out.b[8][k]) * (1.0f / 6.0f);
        const float v = (P1[k] + P2[k] + P3[k]) * (1.0f / 3.0f);
        out.b[9][k] = e + (e - v) * 0.5f;
    }
}

void smooth_eval_patch(const SmoothPatch &patch, float a, float b, float c,
                       float out[3]) {
    const float w[10] = {
        a * a * a,          // b300
        b * b * b,          // b030
        c * c * c,          // b003
        3.0f * a * a * b,   // b210
        3.0f * a * b * b,   // b120
        3.0f * b * b * c,   // b021
        3.0f * b * c * c,   // b012
        3.0f * a * a * c,   // b201
        3.0f * a * c * c,   // b102
        6.0f * a * b * c    // b111
    };
    out[0] = out[1] = out[2] = 0.0f;
    for (int i = 0; i < 10; ++i)
        for (int k = 0; k < 3; ++k)
            out[k] += patch.b[i][k] * w[i];
}

void smooth_shape(const SmoothVertex &p1, const SmoothVertex &p2,
                  const SmoothVertex &p3, const SmoothPolicy &pol,
                  SmoothShape &out) {
    out.no_normal = 0;
    out.curved = 0;
    out.longest = 0.0f;
    out.tightest = 3.4e38f;

    float N[3][3];
    unit_normal(p1, N[0]);
    unit_normal(p2, N[1]);
    unit_normal(p3, N[2]);
    // A corner with no authored normal: nothing to curve towards.
    for (int i = 0; i < 3; ++i)
        if (N[i][0] == 0.0f && N[i][1] == 0.0f && N[i][2] == 0.0f) {
            out.no_normal = 1;
            return;
        }

    static const int E0[3] = {0, 1, 2};
    static const int E1[3] = {1, 2, 0};

    /* THE FLAT TEST FIRST, AND ON ITS OWN. Three dot products decide it, and
       between a third and three quarters of every frame's candidates are
       refused here, so the six square roots the lengths below need were being
       spent on triangles whose verdict was already settled. The verdict is
       the same one -- `curved` never depended on a length -- so this reorder
       changes nothing about WHICH triangles get smoothed. */
    for (int e = 0; e < 3; ++e)
        if (v_dot(N[E0[e]], N[E1[e]]) < pol.flat_cos) out.curved = 1;
    if (!out.curved) return;

    const float P[3][3] = {{p1.x, p1.y, p1.z}, {p2.x, p2.y, p2.z},
                           {p3.x, p3.y, p3.z}};
    for (int e = 0; e < 3; ++e) {
        const int i = E0[e], j = E1[e];
        float d[3];
        v_sub(P[j], P[i], d);
        const float len = sqrtf(v_dot(d, d));
        if (len > out.longest) out.longest = len;

        // Chord between the unit normals: 2*sin(theta/2), which IS the turn
        // angle to within a percent over the range that matters and costs no
        // trigonometry. Radius = arc length / turn.
        float nd[3];
        v_sub(N[j], N[i], nd);
        const float turn = sqrtf(v_dot(nd, nd));
        if (turn > 1e-6f) {
            const float r = len / turn;
            if (r < out.tightest) out.tightest = r;
        }
    }
}

int smooth_shape_factor(const SmoothShape &sh, const SmoothPolicy &pol,
                        float scale, int *why) {
    if (why) *why = SMOOTH_WHY_OK;
    if (pol.level <= 0) { if (why) *why = SMOOTH_WHY_LEVEL; return 1; }
    if (sh.no_normal) { if (why) *why = SMOOTH_WHY_NO_NORMAL; return 1; }
    // Three normals that agree: the patch is flat, so leave the triangle
    // exactly as it arrived.
    if (!sh.curved) { if (why) *why = SMOOTH_WHY_FLAT; return 1; }
    /* Both caps are LENGTHS, so both scale with the matrix. A shape measured
       in local units is `scale` times smaller than the same shape in view
       units, and the caps are written in view units. At scale exactly 1.0f
       these two multiplies are exact and this is the comparison the shipped
       policy has always made. */
    if (pol.max_edge > 0.0f && sh.longest * scale >= pol.max_edge) {
        if (why) *why = SMOOTH_WHY_EDGE;
        return 1;
    }
    if (pol.max_radius > 0.0f && sh.tightest * scale > pol.max_radius) {
        if (why) *why = SMOOTH_WHY_RADIUS;
        return 1;
    }

    int level = pol.level;
    if (level > SMOOTH_MAX_LEVEL) level = SMOOTH_MAX_LEVEL;
    return 1 << level;
}

int smooth_tess_factor(const SmoothVertex &p1, const SmoothVertex &p2,
                       const SmoothVertex &p3, const SmoothPolicy &pol,
                       int *why) {
    if (why) *why = SMOOTH_WHY_OK;
    if (pol.level <= 0) { if (why) *why = SMOOTH_WHY_LEVEL; return 1; }
    SmoothShape sh;
    smooth_shape(p1, p2, p3, pol, sh);
    return smooth_shape_factor(sh, pol, 1.0f, why);
}

int smooth_grid_points(int tf) {
    if (tf <= 1) return 1;
    if (tf > SMOOTH_MAX_TF) tf = SMOOTH_MAX_TF;
    return (tf + 1) * (tf + 2) / 2;
}

int smooth_grid_tris(int tf) {
    if (tf <= 1) return 1;
    if (tf > SMOOTH_MAX_TF) tf = SMOOTH_MAX_TF;
    return tf * tf;
}

namespace {

// The COMPACT index of grid point (ia, ib): its ordinal in the fill order
// `for ia 0..tf { for ib 0..tf-ia }`. Row ia starts after the rows above it,
// which hold (tf+1) + tf + ... + (tf-ia+2) points.
inline int grid_ix(int ia, int ib, int tf) {
    return ia * (tf + 1) - ia * (ia - 1) / 2 + ib;
}

/* The sub-triangle index tables, one per tessellation factor (2, 4 and 8 are
   the only ones: tf is 1 << level and level is 1..3). Built on first use and
   never rebuilt. The walk is smooth_subdivide's own, unchanged, so the
   emission ORDER is the submission order the game chose, which is what
   translucent sorting and the mode-3 shadow stencil both depend on. */
uint16_t g_tri_index[SMOOTH_MAX_LEVEL + 1][SMOOTH_MAX_TRIS * 3];
int g_tri_index_built[SMOOTH_MAX_LEVEL + 1];

int level_of_tf(int tf) {
    for (int l = 0; l <= SMOOTH_MAX_LEVEL; ++l)
        if ((1 << l) == tf) return l;
    return -1;
}

}  // namespace

const uint16_t *smooth_grid_tri_index(int tf) {
    const int l = level_of_tf(tf);
    if (l <= 0) return 0;
    if (!g_tri_index_built[l]) {
        uint16_t *t = g_tri_index[l];
        int n = 0;
        for (int ia = 0; ia < tf; ++ia) {
            for (int ib = 0; ib < tf - ia; ++ib) {
                // Upward sub-triangle: winding matches the parent's.
                t[n++] = (uint16_t)grid_ix(ia, ib, tf);
                t[n++] = (uint16_t)grid_ix(ia + 1, ib, tf);
                t[n++] = (uint16_t)grid_ix(ia, ib + 1, tf);
                if (ib < tf - ia - 1) {
                    t[n++] = (uint16_t)grid_ix(ia + 1, ib, tf);
                    t[n++] = (uint16_t)grid_ix(ia + 1, ib + 1, tf);
                    t[n++] = (uint16_t)grid_ix(ia, ib + 1, tf);
                }
            }
        }
        g_tri_index_built[l] = 1;
    }
    return g_tri_index[l];
}

int smooth_grid_positions(const SmoothVertex &p1, const SmoothVertex &p2,
                          const SmoothVertex &p3, int tf, float *out) {
    if (tf <= 1) {
        out[0] = p1.x; out[1] = p1.y; out[2] = p1.z;
        return 1;
    }
    if (tf > SMOOTH_MAX_TF) tf = SMOOTH_MAX_TF;

    SmoothPatch patch;
    smooth_build_patch(p1, p2, p3, patch);

    // tf is a power of two, so 1/tf is exact and ia*inv lands on 1.0 exactly
    // at the corner -- which is what makes corner preservation bit-exact
    // rather than merely close.
    const float inv = 1.0f / (float)tf;
    int n = 0;
    for (int ia = 0; ia <= tf; ++ia) {
        for (int ib = 0; ib <= tf - ia; ++ib, ++n) {
            const float a = (float)ia * inv;
            const float b = (float)ib * inv;
            float c = 1.0f - a - b;
            if (c < 0.0f) c = 0.0f;
            smooth_eval_patch(patch, a, b, c, out + n * 3);
        }
    }
    return n;
}

void smooth_grid_attrs(const SmoothVertex &p1, const SmoothVertex &p2,
                       const SmoothVertex &p3, float a, float b, float c,
                       SmoothVertex &o) {
    // Linear in barycentrics: textures never swim, and a corner gets its own
    // coordinate back bit for bit.
    o.u = p1.u * a + p2.u * b + p3.u * c;
    o.v = p1.v * a + p2.v * b + p3.v * c;
    o.color = bary_color(p1.color, p2.color, p3.color, a, b, c);
    // Carried for a future relighting pass; nothing reads it today (the
    // colour above is the lighting, already baked at NORMAL).
    o.nx = p1.nx * a + p2.nx * b + p3.nx * c;
    o.ny = p1.ny * a + p2.ny * b + p3.ny * c;
    o.nz = p1.nz * a + p2.nz * b + p3.nz * c;
}

int smooth_subdivide(const SmoothVertex &p1, const SmoothVertex &p2,
                     const SmoothVertex &p3, int tf,
                     SmoothSink sink, void *ctx) {
    if (tf <= 1) {
        sink(ctx, p1, p2, p3);
        return 1;
    }
    if (tf > SMOOTH_MAX_TF) tf = SMOOTH_MAX_TF;

    /* THE ORIGINAL SHAPE, KEPT. This is 0.4.0's path: it hands over one
       sub-triangle at a time, so an interior grid point is handed over once
       per sub-triangle that touches it. The engine reaches the grid directly
       now (smooth_grid_positions above), but the A/B switch
       SM64DS_SMOOTH_LIVE=1 comes back here, and so does the standalone
       selftest, which is what keeps this the definition of "what the kernel
       is supposed to produce" rather than a second opinion about it. */
    float pos[SMOOTH_MAX_GRID * 3];
    smooth_grid_positions(p1, p2, p3, tf, pos);

    SmoothVertex grid[SMOOTH_MAX_GRID];
    const float inv = 1.0f / (float)tf;
    int m = 0;
    for (int ia = 0; ia <= tf; ++ia) {
        for (int ib = 0; ib <= tf - ia; ++ib, ++m) {
            const float a = (float)ia * inv;
            const float b = (float)ib * inv;
            float c = 1.0f - a - b;
            if (c < 0.0f) c = 0.0f;
            SmoothVertex &o = grid[m];
            o.x = pos[m * 3]; o.y = pos[m * 3 + 1]; o.z = pos[m * 3 + 2];
            smooth_grid_attrs(p1, p2, p3, a, b, c, o);
        }
    }

    const uint16_t *idx = smooth_grid_tri_index(tf);
    const int ntris = smooth_grid_tris(tf);
    for (int i = 0; i < ntris; ++i)
        sink(ctx, grid[idx[i * 3]], grid[idx[i * 3 + 1]], grid[idx[i * 3 + 2]]);
    return ntris;
}

// ---------------------------------------------------------------------------
// THE ENGINE-FACING HALF: the switch, the counters, the crack census.
// ---------------------------------------------------------------------------

namespace {

int g_level = 0;
SmoothCounters g_counters;

float env_float(const char *name, float dflt) {
    const char *s = getenv(name);
    if (!s || !*s) return dflt;
    return (float)atof(s);
}

// THE SHIPPED CAPS, and where each number comes from.
//
//  flat_cos 0.99985 is about one degree. Below that the two normals are the
//  same authored normal to within the DS's own 1.9-fixed-point normal
//  resolution (1/512 per component, so two normals that were meant to be
//  equal can land about 0.2 degrees apart after the VECTOR matrix), and the
//  patch would be flat anyway.
//
//  max_radius 128 is the pillow guard, and it is a BACKSTOP rather than the
//  rule that keeps stage geometry flat. SM64DS_SMOOTH_CENSUS histograms every
//  submitted triangle's edge lengths and implied curvature radii per frame;
//  measured over castle grounds, Bob-omb Battlefield, Whomp's Fortress, Cool
//  Cool Mountain and Big Boo's Haunt (out/MDL/policy.md has the frames), the
//  radius histogram is EMPTY above 128 world units on every one of them, and
//  empty above 64 on three of the five. The reason is that this game's large
//  surfaces are flat-shaded: about three quarters of every frame's triangles
//  carry three normals that agree, and the flat rule leaves each of those
//  exactly as it arrived. Every edge longer than about 32 units falls in that
//  group. So the cap fires on nothing measured, costs nothing, and is here for
//  the scene nobody measured.
//
//  max_edge 0 (off) is a decision, not an oversight. A blunt length cap would
//  refuse a large object that is legitimately round -- a boss, a hill-sized
//  prop -- while adding nothing, because the flat rule already excludes every
//  long edge in the five levels above. It stays in the struct, and stays
//  settable from the environment, so a scene that ever needs it needs no code.
//
//  Both are overridable from the environment so a sweep needs no rebuild:
//  SM64DS_SMOOTH_FLAT_COS, SM64DS_SMOOTH_MAX_RADIUS, SM64DS_SMOOTH_MAX_EDGE.
SmoothPolicy g_policy = {0, 0.99985f, 128.0f, 0.0f};
int g_policy_loaded = 0;

int g_census = -1;

// One booked triangle for the crack census: view-space corners and normals.
struct CensusTri { float p[3][3]; float n[3][3]; int tf; };
std::vector<CensusTri> g_census_tris;

// Quantised view-space position, so two corners the game submitted as the
// same point under two different bone matrices hash together. 1/64 of a world
// unit: far below anything the raster can show and far above float noise.
struct QKey {
    int32_t x, y, z;
    bool operator<(const QKey &o) const {
        if (x != o.x) return x < o.x;
        if (y != o.y) return y < o.y;
        return z < o.z;
    }
};
QKey qkey(const float p[3]) {
    QKey k;
    k.x = (int32_t)(p[0] * 64.0f + (p[0] >= 0 ? 0.5f : -0.5f));
    k.y = (int32_t)(p[1] * 64.0f + (p[1] >= 0 ? 0.5f : -0.5f));
    k.z = (int32_t)(p[2] * 64.0f + (p[2] >= 0 ? 0.5f : -0.5f));
    return k;
}
struct EKey {
    QKey a, b;
    bool operator<(const EKey &o) const {
        if (a < o.a) return true;
        if (o.a < a) return false;
        return b < o.b;
    }
};
// One side of a shared edge: the two endpoint normals, in the edge's
// canonical (sorted) endpoint order, plus the endpoint positions.
struct ESide { float p[2][3]; float n[2][3]; int tf; };

// The four control points of the cubic the patch puts on edge A->B. Along
// c == 0 the full PN evaluation collapses to exactly this, which is the whole
// reason a shared edge is watertight without a weld.
void edge_curve(const float PA[3], const float PB[3], const float NA[3],
                const float NB[3], float out[4][3]) {
    for (int k = 0; k < 3; ++k) { out[0][k] = PA[k]; out[3][k] = PB[k]; }
    pn_edge(PA, PB, NA, out[1]);
    pn_edge(PB, PA, NB, out[2]);
}
void edge_eval(const float c[4][3], float t, float o[3]) {
    const float s = 1.0f - t;
    const float w0 = s * s * s, w1 = 3.0f * s * s * t;
    const float w2 = 3.0f * s * t * t, w3 = t * t * t;
    for (int k = 0; k < 3; ++k)
        o[k] = c[0][k] * w0 + c[1][k] * w1 + c[2][k] * w2 + c[3][k] * w3;
}

}  // namespace

void smooth_configure(int level) {
    if (level < 0) level = 0;
    if (level > SMOOTH_MAX_LEVEL) level = SMOOTH_MAX_LEVEL;
    /* A GRID IS BUILT AT ONE LEVEL. The level is part of the key, so a stale
       entry could never be handed to a different level anyway, but a level
       change makes every existing entry dead weight, and this is the one
       clean hook the geometry stage owns. */
    if (level != g_level) smooth_store_clear();
    g_level = level;
    g_policy.level = level;
}

int smooth_level() { return g_level; }

const SmoothPolicy &smooth_policy() {
    if (!g_policy_loaded) {
        g_policy_loaded = 1;
        g_policy.flat_cos   = env_float("SM64DS_SMOOTH_FLAT_COS", g_policy.flat_cos);
        g_policy.max_radius = env_float("SM64DS_SMOOTH_MAX_RADIUS", g_policy.max_radius);
        g_policy.max_edge   = env_float("SM64DS_SMOOTH_MAX_EDGE", g_policy.max_edge);
    }
    g_policy.level = g_level;
    return g_policy;
}

void smooth_counters(SmoothCounters &out) { out = g_counters; }
void smooth_counters_reset() {
    SmoothCounters z = {0,0,0,0,0,0,0,0,0,0,0};
    g_counters = z;
}

int smooth_census_on() {
    if (g_census < 0) g_census = getenv("SM64DS_SMOOTH_CENSUS") ? 1 : 0;
    return g_census;
}

void smooth_census_tri(const SmoothVertex &a, const SmoothVertex &b,
                       const SmoothVertex &c, int tf) {
    if (!smooth_census_on()) return;
    // A frame of castle grounds is a few thousand triangles; the cap is only
    // here so a runaway scene cannot eat the machine.
    if (g_census_tris.size() >= 200000) return;
    CensusTri t;
    t.tf = tf;
    const SmoothVertex *v[3] = {&a, &b, &c};
    for (int i = 0; i < 3; ++i) {
        t.p[i][0] = v[i]->x; t.p[i][1] = v[i]->y; t.p[i][2] = v[i]->z;
        float n[3];
        unit_normal(*v[i], n);
        t.n[i][0] = n[0]; t.n[i][1] = n[1]; t.n[i][2] = n[2];
    }
    g_census_tris.push_back(t);
}

namespace {

// Pair up shared edges and report the frame. This is the constraint-5
// measurement: a shared edge whose two sides carry different normals at the
// same endpoint builds two different cubic edge curves, and the gap between
// them is the crack.
void census_report(uint64_t frame) {
    if (g_census_tris.empty()) return;

    std::map<EKey, std::vector<ESide> > edges;
    // Edge-length and curvature-radius histograms, decade buckets, for the
    // measurement the shipped caps are read off.
    int len_hist[12] = {0}, rad_hist[12] = {0};
    // The same edge-length histogram over the triangles the policy ACCEPTED.
    // This is the terrain proof: if nothing with a long edge is ever accepted,
    // nothing large was curved, whatever the overall flat/curved split says.
    int acc_hist[12] = {0};
    int flat = 0, curved = 0, accepted = 0;

    static const int E0[3] = {0, 1, 2};
    static const int E1[3] = {1, 2, 0};
    for (size_t i = 0; i < g_census_tris.size(); ++i) {
        const CensusTri &t = g_census_tris[i];
        bool any_turn = false;
        for (int e = 0; e < 3; ++e) {
            const int ia = E0[e], ib = E1[e];
            QKey ka = qkey(t.p[ia]), kb = qkey(t.p[ib]);
            ESide s;
            s.tf = t.tf;
            const int first = (kb < ka) ? ib : ia;
            const int second = (kb < ka) ? ia : ib;
            for (int k = 0; k < 3; ++k) {
                s.p[0][k] = t.p[first][k];  s.n[0][k] = t.n[first][k];
                s.p[1][k] = t.p[second][k]; s.n[1][k] = t.n[second][k];
            }
            EKey ek;
            ek.a = (kb < ka) ? kb : ka;
            ek.b = (kb < ka) ? ka : kb;
            edges[ek].push_back(s);

            float d[3], nd[3];
            for (int k = 0; k < 3; ++k) {
                d[k] = t.p[ib][k] - t.p[ia][k];
                nd[k] = t.n[ib][k] - t.n[ia][k];
            }
            const float len = sqrtf(v_dot(d, d));
            const float turn = sqrtf(v_dot(nd, nd));
            if (turn > 0.017f) any_turn = true;   // about one degree
            int lb = 0;
            float l = len;
            while (l >= 1.0f && lb < 11) { l *= 0.5f; ++lb; }
            ++len_hist[lb];
            if (t.tf > 1) ++acc_hist[lb];
            if (turn > 1e-6f) {
                int rb = 0;
                float r = len / turn;
                while (r >= 1.0f && rb < 11) { r *= 0.5f; ++rb; }
                ++rad_hist[rb];
            }
        }
        if (any_turn) ++curved; else ++flat;
        if (t.tf > 1) ++accepted;
    }

    int shared = 0, disagreeing = 0;
    float worst_turn = 0.0f;
    float worst_gap = 0.0f;
    /* THE TWO KINDS OF DISAGREEMENT, kept apart because they want opposite
       fixes. A rigid-skinned JOINT has two bones that have rotated a little
       away from each other, so the turn is moderate and welding the normals
       per frame (what the Rust smoother did) is the right repair. A HARD
       CREASE -- the two faces of a thin panel, a leaf, a flag -- has normals
       near 180 degrees apart, the mesh is genuinely discontinuous there, and
       welding it would be WRONG: it would round a corner the artist meant to
       be sharp. The turn histogram is what tells one from the other without
       looking at anything. Buckets: <0.2, <0.5, <1.0, <1.5, <1.9, >=1.9 of
       the chord between the unit normals (2.0 is 180 degrees). */
    int turn_hist[6] = {0};
    float worst_gap_joint = 0.0f;    /* turn < 1.5: the joint class  */
    float worst_gap_crease = 0.0f;   /* turn >= 1.5: the crease class */
    for (std::map<EKey, std::vector<ESide> >::const_iterator it = edges.begin();
         it != edges.end(); ++it) {
        const std::vector<ESide> &s = it->second;
        if (s.size() < 2) continue;
        ++shared;
        /* ONLY AN EDGE WITH A SUBDIVIDED SIDE CAN OPEN. Two flat neighbours
           may hold wildly different normals -- every hard corner of every
           boxy model does -- and neither is touched, so their shared edge is
           exactly where it always was. Counting those was the first version
           of this census and it reported a crack on geometry the smoother
           never looked at. */
        bool any_acc = false;
        for (size_t j = 0; j < s.size(); ++j) if (s[j].tf > 1) any_acc = true;
        if (!any_acc) continue;
        // Compare every later side against the first.
        float turn = 0.0f;
        for (size_t j = 1; j < s.size(); ++j)
            for (int end = 0; end < 2; ++end) {
                float nd[3];
                for (int k = 0; k < 3; ++k) nd[k] = s[j].n[end][k] - s[0].n[end][k];
                const float d = sqrtf(v_dot(nd, nd));
                if (d > turn) turn = d;
            }
        if (turn <= 0.017f) continue;
        ++disagreeing;
        if (turn > worst_turn) worst_turn = turn;
        turn_hist[turn < 0.2f ? 0 : turn < 0.5f ? 1 : turn < 1.0f ? 2
                  : turn < 1.5f ? 3 : turn < 1.9f ? 4 : 5] += 1;

        // The crack itself: build both sides' edge curves and measure the
        // biggest separation. The two endpoints coincide by construction, so
        // the gap is entirely in the middle of the edge.
        float c0[4][3], c1[4][3];
        edge_curve(s[0].p[0], s[0].p[1], s[0].n[0], s[0].n[1], c0);
        edge_curve(s[0].p[0], s[0].p[1], s[1].n[0], s[1].n[1], c1);
        float worst_here = 0.0f;
        for (int k = 1; k < 8; ++k) {
            const float t = (float)k / 8.0f;
            float x0[3], x1[3];
            edge_eval(c0, t, x0);
            edge_eval(c1, t, x1);
            float dd[3];
            for (int m = 0; m < 3; ++m) dd[m] = x0[m] - x1[m];
            const float gap = sqrtf(v_dot(dd, dd));
            if (gap > worst_here) worst_here = gap;
        }
        if (worst_here > worst_gap) worst_gap = worst_here;
        if (turn < 1.5f) {
            if (worst_here > worst_gap_joint) worst_gap_joint = worst_here;
        } else if (worst_here > worst_gap_crease) {
            worst_gap_crease = worst_here;
        }
    }

    fprintf(stderr,
            "[smooth] f%llu tris %d (flat %d curved %d accepted %d) edges %d "
            "shared %d disagreeing %d worst_turn %.4f worst_gap %.4f\n",
            (unsigned long long)frame, (int)g_census_tris.size(), flat, curved,
            accepted, (int)edges.size(), shared, disagreeing,
            (double)worst_turn, (double)worst_gap);
    fprintf(stderr,
            "[smooth] f%llu turnhist %d %d %d %d %d %d "
            "gap_joint %.4f gap_crease %.4f\n",
            (unsigned long long)frame, turn_hist[0], turn_hist[1],
            turn_hist[2], turn_hist[3], turn_hist[4], turn_hist[5],
            (double)worst_gap_joint, (double)worst_gap_crease);
    fprintf(stderr, "[smooth] f%llu edgelen", (unsigned long long)frame);
    for (int i = 0; i < 12; ++i) fprintf(stderr, " %d", len_hist[i]);
    fprintf(stderr, "\n[smooth] f%llu acceptlen", (unsigned long long)frame);
    for (int i = 0; i < 12; ++i) fprintf(stderr, " %d", acc_hist[i]);
    fprintf(stderr, "\n[smooth] f%llu radius", (unsigned long long)frame);
    for (int i = 0; i < 12; ++i) fprintf(stderr, " %d", rad_hist[i]);
    fprintf(stderr, "\n");
    g_census_tris.clear();
}

int counters_on() {
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_SMOOTH_COUNTERS") ? 1 : 0;
    return on;
}

// One line per frame with the frame's DELTA, which is what a per-frame table
// wants, and the running total, which is what a whole-run comparison wants.
void counters_report(uint64_t frame) {
    static SmoothCounters prev;
    const SmoothCounters &c = g_counters;
    fprintf(stderr,
            "[smoothcnt] f%llu in %llu out %llu sub %llu | flat %llu nonrm %llu"
            " rad %llu edge %llu w %llu mode3 %llu ortho %llu"
            " | TOTin %llu TOTout %llu\n",
            (unsigned long long)frame,
            (unsigned long long)(c.tris_in - prev.tris_in),
            (unsigned long long)(c.tris_out - prev.tris_out),
            (unsigned long long)(c.tris_subdivided - prev.tris_subdivided),
            (unsigned long long)(c.skip_flat - prev.skip_flat),
            (unsigned long long)(c.skip_no_normal - prev.skip_no_normal),
            (unsigned long long)(c.skip_radius - prev.skip_radius),
            (unsigned long long)(c.skip_edge - prev.skip_edge),
            (unsigned long long)(c.skip_w - prev.skip_w),
            (unsigned long long)(c.skip_mode3 - prev.skip_mode3),
            (unsigned long long)(c.skip_ortho - prev.skip_ortho),
            (unsigned long long)c.tris_in, (unsigned long long)c.tris_out);
    prev = c;

    SmoothStoreStats s;
    smooth_store_stats(s);
    fprintf(stderr,
            "[smoothstore] f%llu hits %llu misses %llu entries %llu "
            "bytes %llu cap %llu clears %llu | live %llu crossmtx %llu "
            "nonsim %llu zeronrm %llu\n",
            (unsigned long long)frame,
            (unsigned long long)s.hits, (unsigned long long)s.misses,
            (unsigned long long)s.entries, (unsigned long long)s.bytes,
            (unsigned long long)s.cap_bytes, (unsigned long long)s.clears,
            (unsigned long long)s.live_calls,
            (unsigned long long)s.skip_crossmtx,
            (unsigned long long)s.skip_nonsim,
            (unsigned long long)s.skip_zeronrm);
}

}  // namespace

// ---------------------------------------------------------------------------
// THE SHAPE STORE. Header has the contract and the invariant the caller owes.
// ---------------------------------------------------------------------------

namespace {

std::vector<SmoothEntry> g_st_entry;   // one row per distinct triangle shape
std::vector<int32_t> g_st_table;       // open-addressed index, -1 empty
std::vector<float> g_st_pool;          // every grid, back to back
uint32_t g_st_mask;                    // g_st_table.size() - 1
SmoothStoreStats g_st;
int g_st_cap_loaded;

size_t store_cap_bytes() {
    if (!g_st_cap_loaded) {
        g_st_cap_loaded = 1;
        /* THE CAP, and why it is a cap rather than a policy. A levels=all
           sweep measured the steady state at a small fraction of this (the
           lane report has the number), so the eviction rule below never
           fires in play; the cap is here so a scene nobody measured cannot
           eat the machine, and 32 MB is small enough to be harmless on the
           4 GB a 32-bit process can address and large enough that no
           measured level came close. Heap, never static: the image has to
           end below 0x02000000. */
        double mb = 32.0;
        const char *s = getenv("SM64DS_SMOOTH_STORE_MB");
        if (s && *s) {
            const double v = atof(s);
            if (v >= 0.0) mb = v;
        }
        g_st.cap_bytes = (uint64_t)(mb * 1024.0 * 1024.0);
    }
    return (size_t)g_st.cap_bytes;
}

size_t store_bytes() {
    return g_st_entry.size() * sizeof(SmoothEntry)
         + g_st_table.size() * sizeof(int32_t)
         + g_st_pool.size() * sizeof(float);
}

uint32_t key_hash(const SmoothKey &k) {
    // FNV-1a over the key's own bytes. The hash only picks a slot; every
    // candidate slot is then checked against the WHOLE key, so a collision
    // costs a compare and can never hand back another triangle's grid.
    const unsigned char *p = (const unsigned char *)&k;
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < sizeof(SmoothKey); ++i) {
        h ^= p[i];
        h *= 16777619u;
    }
    return h;
}

bool key_equal(const SmoothKey &a, const SmoothKey &b) {
    if (a.level != b.level) return false;
    for (int i = 0; i < 3; ++i) {
        if (a.n[i] != b.n[i]) return false;
        for (int k = 0; k < 3; ++k)
            if (a.p[i][k] != b.p[i][k]) return false;
    }
    return true;
}

void store_rehash(size_t want) {
    size_t cap = 256;
    while (cap < want * 2) cap <<= 1;
    g_st_table.assign(cap, -1);
    g_st_mask = (uint32_t)(cap - 1);
    for (size_t i = 0; i < g_st_entry.size(); ++i) {
        uint32_t s = key_hash(g_st_entry[i].key) & g_st_mask;
        while (g_st_table[s] >= 0) s = (s + 1) & g_st_mask;
        g_st_table[s] = (int32_t)i;
    }
}

}  // namespace

void smooth_store_clear() {
    g_st_entry.clear();
    g_st_pool.clear();
    g_st_table.clear();
    g_st_mask = 0;
    g_st.entries = 0;
    g_st.bytes = 0;
    ++g_st.clears;
}

const SmoothEntry *smooth_store_find(const SmoothKey &k) {
    if (g_st_table.empty()) { ++g_st.misses; return 0; }
    uint32_t s = key_hash(k) & g_st_mask;
    for (;;) {
        const int32_t i = g_st_table[s];
        if (i < 0) { ++g_st.misses; return 0; }
        if (key_equal(g_st_entry[(size_t)i].key, k)) {
            ++g_st.hits;
            return &g_st_entry[(size_t)i];
        }
        s = (s + 1) & g_st_mask;
    }
}

const SmoothEntry *smooth_store_add(const SmoothKey &k, int tf,
                                    const SmoothShape &sh, const float *grid) {
    const int npts = (tf > 1 && grid) ? smooth_grid_points(tf) : 0;
    const size_t add = sizeof(SmoothEntry) + (size_t)npts * 3 * sizeof(float);
    /* EVICTION IS A CLEAR. Every entry is equally cheap to rebuild (one patch
       and one grid), the working set of a level is far below the cap, and an
       LRU would cost more bookkeeping per lookup than the work it saves. So
       the full pool empties, once, loudly enough to be counted. */
    if (store_bytes() + add > store_cap_bytes()) {
        smooth_store_clear();
        if (add > store_cap_bytes()) return 0;
    }
    if (g_st_table.empty() || g_st_entry.size() * 2 + 2 > g_st_table.size())
        store_rehash(g_st_entry.size() + 64);

    SmoothEntry e;
    e.key = k;
    e.shape = sh;
    e.tf = npts ? tf : 1;
    e.off = (uint32_t)g_st_pool.size();
    if (npts) g_st_pool.insert(g_st_pool.end(), grid, grid + npts * 3);
    g_st_entry.push_back(e);

    uint32_t s = key_hash(k) & g_st_mask;
    while (g_st_table[s] >= 0) s = (s + 1) & g_st_mask;
    g_st_table[s] = (int32_t)(g_st_entry.size() - 1);

    ++g_st.inserts;
    g_st.entries = g_st_entry.size();
    g_st.bytes = store_bytes();
    return &g_st_entry.back();
}

const float *smooth_store_grid(const SmoothEntry *e) {
    if (!e || e->tf <= 1) return 0;
    return &g_st_pool[e->off];
}

void smooth_store_stats(SmoothStoreStats &out) {
    g_st.entries = g_st_entry.size();
    g_st.bytes = store_bytes();
    store_cap_bytes();
    out = g_st;
}

void smooth_store_count(int which, uint64_t n) {
    switch (which) {
        case SMOOTH_STORE_CROSSMTX: g_st.skip_crossmtx += n; break;
        case SMOOTH_STORE_NONSIM:   g_st.skip_nonsim += n; break;
        case SMOOTH_STORE_ZERONRM:  g_st.skip_zeronrm += n; break;
        case SMOOTH_STORE_LIVE:     g_st.live_calls += n; break;
        default: break;
    }
}

// --- SM64DS_SMOOTH_ABDIFF: the stored patch against the live one -----------

namespace {
int g_ab = -1;
uint64_t g_ab_tris, g_ab_disagree;
double g_ab_worst, g_ab_worst_rel;
}  // namespace

int smooth_abdiff_on() {
    if (g_ab < 0) g_ab = getenv("SM64DS_SMOOTH_ABDIFF") ? 1 : 0;
    return g_ab;
}

void smooth_abdiff_add(int verdict_differs, float worst_dev, float longest) {
    ++g_ab_tris;
    if (verdict_differs) ++g_ab_disagree;
    if (worst_dev > g_ab_worst) g_ab_worst = worst_dev;
    if (longest > 0.0f) {
        const double rel = (double)worst_dev / (double)longest;
        if (rel > g_ab_worst_rel) g_ab_worst_rel = rel;
    }
}

int smooth_live_mode() {
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_SMOOTH_LIVE");
        on = (e && *e && *e != '0') ? 1 : 0;
        if (on)
            fprintf(stderr, "  [smooth] SM64DS_SMOOTH_LIVE=1: every patch is "
                    "rebuilt from its view-space corners every frame\n");
    }
    return on;
}

// ---------------------------------------------------------------------------
// THE SCOPED PROFILER. Header has the contract and the honesty note about the
// clock read perturbing the SINK bucket.
// ---------------------------------------------------------------------------

namespace {

int g_prof = -1;
long long g_prof_ns[SMOOTH_PROF_BUCKETS];
unsigned long long g_prof_n[SMOOTH_PROF_BUCKETS];

void prof_report(uint64_t frame) {
    static long long prev_ns[SMOOTH_PROF_BUCKETS];
    static unsigned long long prev_n[SMOOTH_PROF_BUCKETS];
    // maths = the whole subdivide minus the sink calls nested inside it, so
    // it is the patch build plus the grid evaluation and nothing else.
    const double pol = (double)(g_prof_ns[SMOOTH_PROF_POLICY]
                              - prev_ns[SMOOTH_PROF_POLICY]) / 1e6;
    const double sub = (double)(g_prof_ns[SMOOTH_PROF_SUBDIV]
                              - prev_ns[SMOOTH_PROF_SUBDIV]) / 1e6;
    const double snk = (double)(g_prof_ns[SMOOTH_PROF_SINK]
                              - prev_ns[SMOOTH_PROF_SINK]) / 1e6;
    fprintf(stderr,
            "[smoothprof] f%llu policy %.3f ms (%llu) maths %.3f ms (%llu) "
            "sink %.3f ms (%llu) | TOT policy %.1f maths %.1f sink %.1f\n",
            (unsigned long long)frame, pol,
            g_prof_n[SMOOTH_PROF_POLICY] - prev_n[SMOOTH_PROF_POLICY],
            sub - snk,
            g_prof_n[SMOOTH_PROF_SUBDIV] - prev_n[SMOOTH_PROF_SUBDIV],
            snk, g_prof_n[SMOOTH_PROF_SINK] - prev_n[SMOOTH_PROF_SINK],
            (double)g_prof_ns[SMOOTH_PROF_POLICY] / 1e6,
            (double)(g_prof_ns[SMOOTH_PROF_SUBDIV]
                   - g_prof_ns[SMOOTH_PROF_SINK]) / 1e6,
            (double)g_prof_ns[SMOOTH_PROF_SINK] / 1e6);
    for (int i = 0; i < SMOOTH_PROF_BUCKETS; ++i) {
        prev_ns[i] = g_prof_ns[i];
        prev_n[i] = g_prof_n[i];
    }
}

}  // namespace

int smooth_prof_on() {
    if (g_prof < 0) g_prof = getenv("SM64DS_SMOOTH_PROF") ? 1 : 0;
    return g_prof;
}

long long smooth_prof_ticks() {
    return (long long)std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

void smooth_prof_add(int bucket, long long dt_ns, unsigned n) {
    if (bucket < 0 || bucket >= SMOOTH_PROF_BUCKETS) return;
    g_prof_ns[bucket] += dt_ns;
    g_prof_n[bucket] += n;
}

void smooth_frame_mark() {
    ++g_counters.frames;
    if (smooth_census_on()) census_report(g_counters.frames);
    if (counters_on()) counters_report(g_counters.frames);
    if (smooth_prof_on()) prof_report(g_counters.frames);
    /* HERE rather than inside counters_report, because the audit's own
       accumulators live with the store they audit, further up this file. */
    if (smooth_abdiff_on())
        fprintf(stderr,
                "[smoothab] f%llu tris %llu verdict-disagree %llu "
                "worst_dev %.3e view units (%.3e of the longest edge)\n",
                (unsigned long long)g_counters.frames,
                (unsigned long long)g_ab_tris,
                (unsigned long long)g_ab_disagree, g_ab_worst, g_ab_worst_rel);
}

// The counter block the geometry stage bumps. Out of line so gx.cpp holds no
// copy of the layout.
void smooth_count(int which, uint64_t n) {
    switch (which) {
        case SMOOTH_COUNT_IN:          g_counters.tris_in += n; break;
        case SMOOTH_COUNT_OUT:         g_counters.tris_out += n; break;
        case SMOOTH_COUNT_SUBDIVIDED:  g_counters.tris_subdivided += n; break;
        case SMOOTH_COUNT_FLAT:        g_counters.skip_flat += n; break;
        case SMOOTH_COUNT_NO_NORMAL:   g_counters.skip_no_normal += n; break;
        case SMOOTH_COUNT_RADIUS:      g_counters.skip_radius += n; break;
        case SMOOTH_COUNT_EDGE:        g_counters.skip_edge += n; break;
        case SMOOTH_COUNT_W:           g_counters.skip_w += n; break;
        case SMOOTH_COUNT_MODE3:       g_counters.skip_mode3 += n; break;
        case SMOOTH_COUNT_ORTHO:       g_counters.skip_ortho += n; break;
        default: break;
    }
}

}  // namespace ntr
