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
#include "ntr/ppu.h"

#include <emmintrin.h>
#include <intrin.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <chrono>
#include <map>
#include <utility>
#include <string.h>
#include <unordered_map>
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

namespace {
// The patch from its corners and its six edge normals (b210, b120, b021,
// b012, b201, b102). The standard patch hands every edge the normals of its
// two corners; the topology book hands some edges other ones.
void build_patch_core(const float P1[3], const float P2[3], const float P3[3],
                      const float en[6][3], SmoothPatch &out) {
    for (int k = 0; k < 3; ++k) {
        out.b[0][k] = P1[k];      // b300
        out.b[1][k] = P2[k];      // b030
        out.b[2][k] = P3[k];      // b003
    }
    pn_edge(P1, P2, en[0], out.b[3]);   // b210
    pn_edge(P2, P1, en[1], out.b[4]);   // b120
    pn_edge(P2, P3, en[2], out.b[5]);   // b021
    pn_edge(P3, P2, en[3], out.b[6]);   // b012
    pn_edge(P1, P3, en[4], out.b[7]);   // b201
    pn_edge(P3, P1, en[5], out.b[8]);   // b102

    // The standard PN centre lift: E is the mean of the six edge points, V the
    // vertex centroid, b111 = E + (E - V)/2.
    for (int k = 0; k < 3; ++k) {
        const float e = (out.b[3][k] + out.b[4][k] + out.b[5][k]
                       + out.b[6][k] + out.b[7][k] + out.b[8][k]) * (1.0f / 6.0f);
        const float v = (P1[k] + P2[k] + P3[k]) * (1.0f / 3.0f);
        out.b[9][k] = e + (e - v) * 0.5f;
    }
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
    const float en[6][3] = {
        {N1[0], N1[1], N1[2]}, {N2[0], N2[1], N2[2]}, {N2[0], N2[1], N2[2]},
        {N3[0], N3[1], N3[2]}, {N1[0], N1[1], N1[2]}, {N3[0], N3[1], N3[2]}};
    build_patch_core(P1, P2, P3, en, out);
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

namespace {
SmoothGridTable g_grid_tab[SMOOTH_MAX_LEVEL + 1];
int g_grid_tab_built[SMOOTH_MAX_LEVEL + 1];

void build_grid_table(int tf, SmoothGridTable &T) {
    const float inv = 1.0f / (float)tf;
    int n = 0;
    for (int ia = 0; ia <= tf; ++ia) {
        for (int ib = 0; ib <= tf - ia; ++ib, ++n) {
            const float a = (float)ia * inv;
            const float b = (float)ib * inv;
            float c = 1.0f - a - b;
            if (c < 0.0f) c = 0.0f;
            T.a[n] = a; T.b[n] = b; T.c[n] = c;
            float *w = T.w[n];
            w[0] = a * a * a;          // b300
            w[1] = b * b * b;          // b030
            w[2] = c * c * c;          // b003
            w[3] = 3.0f * a * a * b;   // b210
            w[4] = 3.0f * a * b * b;   // b120
            w[5] = 3.0f * b * b * c;   // b021
            w[6] = 3.0f * b * c * c;   // b012
            w[7] = 3.0f * a * a * c;   // b201
            w[8] = 3.0f * a * c * c;   // b102
            w[9] = 6.0f * a * b * c;   // b111
        }
    }
    T.n = n;
}

/* The whole grid from a patch: out[k] = (((0 + b0*w0) + b1*w1) + ...) per
   coordinate, which is smooth_eval_patch's own accumulation, with the three
   coordinates in three lanes. Separate multiply and add, no fused op, so
   every float is the one the scalar loop made. */
void eval_grid(const SmoothPatch &patch, const SmoothGridTable &T, float *out) {
    __m128 B[10];
    for (int i = 0; i < 10; ++i)
        B[i] = _mm_setr_ps(patch.b[i][0], patch.b[i][1], patch.b[i][2], 0.0f);
    for (int n = 0; n < T.n; ++n) {
        __m128 acc = _mm_setzero_ps();
        const float *w = T.w[n];
        for (int i = 0; i < 10; ++i)
            acc = _mm_add_ps(acc, _mm_mul_ps(B[i], _mm_set1_ps(w[i])));
        float t[4];
        _mm_storeu_ps(t, acc);
        out[n * 3] = t[0]; out[n * 3 + 1] = t[1]; out[n * 3 + 2] = t[2];
    }
}
}  // namespace

const SmoothGridTable *smooth_grid_table(int tf) {
    const int l = level_of_tf(tf);
    if (l <= 0) return 0;
    if (!g_grid_tab_built[l]) {
        build_grid_table(tf, g_grid_tab[l]);
        g_grid_tab_built[l] = 1;
    }
    return &g_grid_tab[l];
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
    const SmoothGridTable *T = smooth_grid_table(tf);
    eval_grid(patch, *T, out);
    return T->n;
}

int smooth_grid_positions_edges(const SmoothVertex &p1, const SmoothVertex &p2,
                                const SmoothVertex &p3, const float en[6][3],
                                int tf, float *out) {
    if (tf <= 1) {
        out[0] = p1.x; out[1] = p1.y; out[2] = p1.z;
        return 1;
    }
    if (tf > SMOOTH_MAX_TF) tf = SMOOTH_MAX_TF;
    const float P1[3] = {p1.x, p1.y, p1.z};
    const float P2[3] = {p2.x, p2.y, p2.z};
    const float P3[3] = {p3.x, p3.y, p3.z};
    SmoothPatch patch;
    build_patch_core(P1, P2, P3, en, patch);
    const SmoothGridTable *T = smooth_grid_table(tf);
    eval_grid(patch, *T, out);
    return T->n;
}

void smooth_grid_attrs(const SmoothVertex &p1, const SmoothVertex &p2,
                       const SmoothVertex &p3, float a, float b, float c,
                       SmoothVertex &o) {
    // Linear in barycentrics: textures never swim, and a corner gets its own
    // coordinate back bit for bit. The arithmetic is smooth.h's inline copy.
    smooth_grid_attrs_inline(p1, p2, p3, a, b, c, o);
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

/* THE BUDGET'S STATE (smooth.h, THE BUDGET). g_drawn is the level the
   current picture is drawn at; g_demand counts the curved triangles the
   tick's own picture asked for, and is read and cleared at the frame mark. */
int g_drawn = 0;
uint64_t g_demand = 0;
uint64_t g_last_demand = 0;
int g_budget = -1;
int g_budget_said = 0;

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
    g_drawn = level;
    g_demand = 0;
    g_policy.level = level;
}

int smooth_level() { return g_level; }

int smooth_budget() {
    if (g_budget < 0) {
        /* THE NUMBER. Measured on the base build at level 3 with the counters
           on, 570 pictures a row, idle and moving (the lane report has the
           rows): castle grounds asks for 450 to 615 curved triangles a
           picture (29k to 39k sub-triangles), Bob-omb Battlefield 700 to
           950 (45k to 61k), Whomp's Fortress 650 to 920 (42k to 59k), and
           all of them hold full speed; the goo painting asks for 1,590
           (102k) and Picture Poker's top screen 1,280 (82k), and they do
           not. 72k sits above every ordinary picture measured (the worst,
           Bob-omb moving, is 61k) and under the two that hurt; at level 2
           those two ask for 25k and 20k, well inside it. */
        g_budget = 72000;
        const char *s = getenv("SM64DS_SMOOTH_BUDGET");
        if (s && *s) {
            const int v = atoi(s);
            g_budget = v < 0 ? 0 : v;
        }
    }
    return g_budget;
}

int smooth_level_drawn() { return g_drawn; }

const SmoothPolicy &smooth_policy() {
    if (!g_policy_loaded) {
        g_policy_loaded = 1;
        g_policy.flat_cos   = env_float("SM64DS_SMOOTH_FLAT_COS", g_policy.flat_cos);
        g_policy.max_radius = env_float("SM64DS_SMOOTH_MAX_RADIUS", g_policy.max_radius);
        g_policy.max_edge   = env_float("SM64DS_SMOOTH_MAX_EDGE", g_policy.max_edge);
    }
    /* The level the picture is DRAWN at. With the budget off, or with a
       scene whose demand fits, this is the set level and the policy is the
       one it has always been. */
    g_policy.level = g_drawn;
    return g_policy;
}

namespace {

/* ONE DECISION PER PICTURE, at the frame mark, from the demand the previous
   tick's picture produced. DOWN AT ONCE, while the demand at this level is
   over the budget. UP SLOWLY: one level, after the demand at the next level
   up has fitted with an eighth to spare for thirty pictures in a row (a
   second of a course). A scene whose demand swings across the budget from
   frame to frame (the Bowser arenas, 1,000 to 1,550 curved triangles) would
   otherwise flip between two levels every few pictures, and a level flip is
   the one thing in this that a player could see happen. Never below 1,
   never above the set level. */
int g_fit_run = 0;

void budget_pick() {
    const uint64_t n = g_demand;
    g_last_demand = n;
    g_demand = 0;
    const int b = smooth_budget();
    if (b <= 0 || g_level <= 0) { g_drawn = g_level; g_fit_run = 0; return; }
    int L = g_drawn;
    if (L < 1) L = 1;
    if (L > g_level) L = g_level;
    while (L > 1 && (n << (2 * L)) > (uint64_t)b) { --L; g_fit_run = 0; }
    if (L < g_level && (n << (2 * (L + 1))) <= (uint64_t)(b - b / 8)) {
        if (++g_fit_run >= 30) { ++L; g_fit_run = 0; }
    } else {
        g_fit_run = 0;
    }
    if (L != g_drawn) {
        /* Say so once, plainly, the first time a scene is held under the
           budget; after that the counters line carries it. */
        if (L < g_level && !g_budget_said) {
            g_budget_said = 1;
            fprintf(stderr,
                    "[smooth] SmoothModels %d: this picture asked for %llu "
                    "curved triangles (%llu sub-triangles at level %d), over "
                    "the budget of %d a picture; drawing at level %d until it "
                    "fits again (SM64DS_SMOOTH_BUDGET, 0 = no budget)\n",
                    g_level, (unsigned long long)n,
                    (unsigned long long)(n << (2 * g_level)), g_level, b, L);
        }
        g_drawn = L;
    }
}

}  // namespace

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
            " | TOTin %llu TOTout %llu | demand %llu drawn %d of %d budget %d\n",
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
            (unsigned long long)c.tris_in, (unsigned long long)c.tris_out,
            (unsigned long long)g_last_demand, g_drawn, g_level,
            smooth_budget());
    prev = c;

    SmoothStoreStats s;
    smooth_store_stats(s);
    fprintf(stderr,
            "[smoothstore] f%llu hits %llu misses %llu entries %llu "
            "bytes %llu cap %llu clears %llu | live %llu crossmtx %llu "
            "nonsim %llu zeronrm %llu | held %llu purges %llu\n",
            (unsigned long long)frame,
            (unsigned long long)s.hits, (unsigned long long)s.misses,
            (unsigned long long)s.entries, (unsigned long long)s.bytes,
            (unsigned long long)s.cap_bytes, (unsigned long long)s.clears,
            (unsigned long long)s.live_calls,
            (unsigned long long)s.skip_crossmtx,
            (unsigned long long)s.skip_nonsim,
            (unsigned long long)s.skip_zeronrm,
            (unsigned long long)s.held, (unsigned long long)s.purges,
            (unsigned long long)s.sweeps);
    fprintf(stderr,
            "[smoothwhy] f%llu cross corner %llu normal %llu stale %llu | "
            "nonsim len %llu ortho %llu vec %llu\n",
            (unsigned long long)frame,
            (unsigned long long)s.cross_corner,
            (unsigned long long)s.cross_normal,
            (unsigned long long)s.cross_stale,
            (unsigned long long)s.nonsim_len,
            (unsigned long long)s.nonsim_ortho,
            (unsigned long long)s.nonsim_vec);
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
uint64_t g_st_notes;                   // grid-less notes among g_st_entry
uint64_t g_st_sweep_at = 16384;        // notes held before the next sweep of the old ones
uint32_t g_st_sweep_pic;               // the picture that last swept
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
    // run hunt8 SMOOTHCPU2: a word at a time (the key is nine words, padding
    // zeroed by the caller as before); the slot choice is not observable.
    static_assert(sizeof(SmoothKey) % 4 == 0, "key is whole words");
    const uint32_t *p = (const uint32_t *)&k;
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < sizeof(SmoothKey) / 4; ++i) {
        h = (h ^ p[i]) * 0x9E3779B1u;
        h ^= h >> 15;
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

namespace { void (*g_st_clear_hook)(); uint32_t g_st_gen = 1; }
void smooth_store_set_clear_hook(void (*fn)()) { g_st_clear_hook = fn; }
const float *smooth_store_pool(size_t *n) {
    if (n) *n = g_st_pool.size();
    return g_st_pool.empty() ? 0 : &g_st_pool[0];
}
uint32_t smooth_store_generation() { return g_st_gen; }
uint32_t smooth_store_entry_off(const SmoothEntry *e) { return e->off; }
const float *smooth_store_entry_box(const SmoothEntry *e) { return e->box; }

void smooth_store_clear() {
    if (g_st_clear_hook && !g_st_pool.empty()) g_st_clear_hook();
    ++g_st_gen;
    g_st_entry.clear();
    g_st_pool.clear();
    g_st_table.clear();
    g_st_mask = 0;
    g_st.entries = 0;
    g_st.bytes = 0;
    g_st_notes = 0;
    ++g_st.clears;
}

namespace {

/* The slot of `k`, kept or grid-less, or -1. The hash only picks a start;
   every candidate is compared against the whole key. */
int32_t store_slot(const SmoothKey &k) {
    if (g_st_table.empty()) return -1;
    uint32_t s = key_hash(k) & g_st_mask;
    for (;;) {
        const int32_t i = g_st_table[s];
        if (i < 0) return -1;
        if (key_equal(g_st_entry[(size_t)i].key, k)) return i;
        s = (s + 1) & g_st_mask;
    }
}

/* DROP NOTES. keep_from == 0 drops every grid-less note (THE CAP'S FIRST
   RESORT: shapes seen once and never again, which at the goo painting is
   1,150 new ones a picture, and keep every grid); otherwise a note is kept
   while a second sighting could still make a grid of it, that is while it
   is no more than two pictures old (smooth_store_miss), and older ones go.
   The pool is untouched, the entry rows are compacted and the table
   rebuilt, so a static model's grid survives the painting. */
void store_drop_notes(uint32_t keep_from) {
    std::vector<SmoothEntry> kept;
    kept.reserve(g_st_entry.size());
    uint64_t notes = 0;
    for (size_t i = 0; i < g_st_entry.size(); ++i) {
        const SmoothEntry &e = g_st_entry[i];
        if (e.tf > 0) { kept.push_back(e); continue; }
        if (keep_from && e.pic >= keep_from) { kept.push_back(e); ++notes; }
    }
    g_st_entry.swap(kept);
    store_rehash(g_st_entry.size() + 64);
    g_st_notes = notes;
    g_st.entries = g_st_entry.size();
    g_st.bytes = store_bytes();
}

/* The cap's first resort, counted. */
void store_purge_notes() {
    store_drop_notes(0);
    ++g_st.purges;
}

/* Append a row for `k` and index it. */
SmoothEntry &store_append(const SmoothKey &k) {
    if (g_st_table.empty() || g_st_entry.size() * 2 + 2 > g_st_table.size())
        store_rehash(g_st_entry.size() + 64);
    SmoothEntry e;
    e.key = k;
    e.tf = 0;
    e.off = 0;
    e.pic = 0;
    for (int i = 0; i < 6; ++i) e.box[i] = 0.0f;
    g_st_entry.push_back(e);
    uint32_t s = key_hash(k) & g_st_mask;
    while (g_st_table[s] >= 0) s = (s + 1) & g_st_mask;
    g_st_table[s] = (int32_t)(g_st_entry.size() - 1);
    return g_st_entry.back();
}

}  // namespace

const SmoothEntry *smooth_store_find(const SmoothKey &k) {
    const int32_t i = store_slot(k);
    /* A grid-less note is a miss to the caller: it has nothing to replay.
       smooth_store_miss is where the note is read. */
    if (i < 0 || g_st_entry[(size_t)i].tf <= 0) { ++g_st.misses; return 0; }
    ++g_st.hits;
    return &g_st_entry[(size_t)i];
}

int smooth_store_miss(const SmoothKey &k) {
    const uint32_t pic = (uint32_t)g_counters.frames + 1;   /* never 0 */
    const int32_t i = store_slot(k);
    if (i >= 0) {
        SmoothEntry &e = g_st_entry[(size_t)i];
        /* Seen before in one of the TWO PREVIOUS pictures: keep it this
           time. The same picture (a second instance of the shape, or a
           SmoothMotion replay of the same tick) does not count: the
           painting's keys come back in every replay of the tick that
           invented them. Nor does a sighting further back: the goo's wave
           is periodic and a few hundred of its keys a picture come round
           again after some seconds, and a grid kept for each of those
           filled the pool just the same, only slower. A static shape is on
           screen on consecutive pictures and is kept on its second; a
           shape that drops out and comes back a second later is noted
           again and kept on the picture after that. */
        if (e.tf <= 0 && e.pic != pic) {
            if (pic - e.pic <= 2u) return 1;
            e.pic = pic;
        }
        ++g_st.held;
        return 0;
    }
    /* OLD NOTES GO EARLY. A note more than two pictures old can no longer
       become a grid (a sighting that late is noted afresh, above), so it is
       worth exactly what no note is worth. At the goo painting they are 1,150
       a picture and, left to the cap, filled 28 MB and then went in one sweep
       of half a million rows: a 55 ms stall every 600 pictures. So once
       16,384 notes are held (or twice what the last sweep left, so a scene
       whose notes are all young does not sweep on every miss) the old ones
       are dropped, a sweep of a few thousand rows a dozen pictures apart. */
    if (g_st_notes >= g_st_sweep_at && g_st_sweep_pic != pic) {
        g_st_sweep_pic = pic;
        store_drop_notes(pic > 2u ? pic - 2u : 1u);
        g_st_sweep_at = g_st_notes * 2 > 16384 ? g_st_notes * 2 : 16384;
        ++g_st.sweeps;
    }
    /* First sighting: a 48-byte note, no grid. Under the cap the notes go
       first; a cap too small for even the notes keeps nothing, which is the
       live path's own behaviour and costs no more than it. */
    if (store_bytes() + sizeof(SmoothEntry) > store_cap_bytes()) {
        store_purge_notes();
        if (store_bytes() + sizeof(SmoothEntry) > store_cap_bytes()) {
            ++g_st.held;
            return 0;
        }
    }
    SmoothEntry &e = store_append(k);
    e.pic = pic;
    ++g_st_notes;
    g_st.entries = g_st_entry.size();
    g_st.bytes = store_bytes();
    ++g_st.held;
    return 0;
}

const SmoothEntry *smooth_store_add(const SmoothKey &k, int tf,
                                    const float *grid) {
    const int npts = (tf > 1 && grid) ? smooth_grid_points(tf) : 0;
    const size_t add = sizeof(SmoothEntry) + (size_t)npts * 3 * sizeof(float);
    /* EVICTION IS A CLEAR, after the notes have gone. Every grid is equally
       cheap to rebuild (one patch and one grid), the working set of a level
       is far below the cap, and an LRU would cost more bookkeeping per
       lookup than the work it saves. So the grid-less notes go first, and if
       that is not enough the full pool empties, once, loudly enough to be
       counted. */
    if (store_bytes() + add > store_cap_bytes()) {
        store_purge_notes();
        if (store_bytes() + add > store_cap_bytes()) {
            smooth_store_clear();
            if (add > store_cap_bytes()) return 0;
        }
    }
    /* The key's own note, if it has one, becomes the entry: no twin rows. */
    const int32_t have = store_slot(k);
    const bool was_note = have >= 0 && g_st_entry[(size_t)have].tf <= 0;
    if (was_note && g_st_notes) --g_st_notes;
    SmoothEntry &e = was_note ? g_st_entry[(size_t)have] : store_append(k);
    e.tf = npts ? tf : 1;
    e.off = (uint32_t)g_st_pool.size();
    if (npts) g_st_pool.insert(g_st_pool.end(), grid, grid + npts * 3);
    {
        float lo[3] = {0, 0, 0}, hi[3] = {0, 0, 0};
        if (npts) {
            for (int k = 0; k < 3; ++k) lo[k] = hi[k] = grid[k];
            for (int i = 1; i < npts; ++i)
                for (int k = 0; k < 3; ++k) {
                    const float v = grid[i * 3 + k];
                    if (v < lo[k]) lo[k] = v;
                    if (v > hi[k]) hi[k] = v;
                }
        }
        for (int k = 0; k < 3; ++k) { e.box[k] = lo[k]; e.box[3 + k] = hi[k]; }
    }

    ++g_st.inserts;
    g_st.entries = g_st_entry.size();
    g_st.bytes = store_bytes();
    return &e;
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
        case SMOOTH_STORE_CROSS_CORNER: g_st.cross_corner += n; break;
        case SMOOTH_STORE_CROSS_NORMAL: g_st.cross_normal += n; break;
        case SMOOTH_STORE_CROSS_STALE:  g_st.cross_stale += n; break;
        case SMOOTH_STORE_NONSIM_LEN:   g_st.nonsim_len += n; break;
        case SMOOTH_STORE_NONSIM_ORTHO: g_st.nonsim_ortho += n; break;
        case SMOOTH_STORE_NONSIM_VEC:   g_st.nonsim_vec += n; break;
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
    fprintf(stderr,
            "[smoothprof2] f%llu topo %.3f ms (%llu) try %.3f ms (%llu) near %.3f ms (%llu)\n",
            (unsigned long long)frame,
            (double)(g_prof_ns[SMOOTH_PROF_TOPO] - prev_ns[SMOOTH_PROF_TOPO]) / 1e6,
            g_prof_n[SMOOTH_PROF_TOPO] - prev_n[SMOOTH_PROF_TOPO],
            (double)(g_prof_ns[SMOOTH_PROF_TRY] - prev_ns[SMOOTH_PROF_TRY]) / 1e6,
            g_prof_n[SMOOTH_PROF_TRY] - prev_n[SMOOTH_PROF_TRY],
            (double)(g_prof_ns[SMOOTH_PROF_NEAR] - prev_ns[SMOOTH_PROF_NEAR]) / 1e6,
            g_prof_n[SMOOTH_PROF_NEAR] - prev_n[SMOOTH_PROF_NEAR]);
    fprintf(stderr,
            "[smoothprof3] f%llu pre %.3f ms (%llu) find %.3f ms (%llu) emit %.3f ms (%llu)\n",
            (unsigned long long)frame,
            (double)(g_prof_ns[SMOOTH_PROF_PRE] - prev_ns[SMOOTH_PROF_PRE]) / 1e6,
            g_prof_n[SMOOTH_PROF_PRE] - prev_n[SMOOTH_PROF_PRE],
            (double)(g_prof_ns[SMOOTH_PROF_FIND] - prev_ns[SMOOTH_PROF_FIND]) / 1e6,
            g_prof_n[SMOOTH_PROF_FIND] - prev_n[SMOOTH_PROF_FIND],
            (double)(g_prof_ns[SMOOTH_PROF_EMIT] - prev_ns[SMOOTH_PROF_EMIT]) / 1e6,
            g_prof_n[SMOOTH_PROF_EMIT] - prev_n[SMOOTH_PROF_EMIT]);
    for (int i = 0; i < SMOOTH_PROF_BUCKETS; ++i) {
        prev_ns[i] = g_prof_ns[i];
        prev_n[i] = g_prof_n[i];
    }
}

}  // namespace

int smooth_prof_on() {
    if (g_prof < 0) {
        const char *e = getenv("SM64DS_SMOOTH_PROF");
        g_prof = (e && *e) ? atoi(e) : 0;
        if (g_prof < 0) g_prof = 0;
    }
    return g_prof;
}

/* run hunt8 SMOOTHCPU2: the profiler clock is the processor's cycle counter
   (about 7 ns a read against 25 for steady_clock), scaled to nanoseconds by a
   2 ms calibration against steady_clock on first use. Still only a difference
   is meaningful. */
long long smooth_prof_ticks() {
    static double ns_per_cyc = 0.0;
    static unsigned long long c0 = 0;
    static long long n0 = 0;
    if (ns_per_cyc == 0.0) {
        auto now = []() { return (long long)std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count(); };
        const long long a = now(); const unsigned long long ca = __rdtsc();
        while (now() - a < 2000000) {}
        const long long b = now(); const unsigned long long cb = __rdtsc();
        ns_per_cyc = (double)(b - a) / (double)(cb - ca);
        c0 = cb; n0 = b;
    }
    return n0 + (long long)((double)(__rdtsc() - c0) * ns_per_cyc);
}

void smooth_prof_add(int bucket, long long dt_ns, unsigned n) {
    if (bucket < 0 || bucket >= SMOOTH_PROF_BUCKETS) return;
    g_prof_ns[bucket] += dt_ns;
    g_prof_n[bucket] += n;
}

// --- THE CRACK BOOK (smooth.h, SM64DS_SMOOTH_CRACKS) -----------------------
void smooth_weld_dump_edge(const int16_t a[3], const int16_t b[3]);
namespace {
int g_cracks = -1;
std::vector<SmoothCrackTri> g_ck_tris;

const char *ck_why_name(int w) {
    switch (w) {
        case SMOOTH_WHY_LEVEL: return "level";
        case SMOOTH_WHY_NO_NORMAL: return "nonormal";
        case SMOOTH_WHY_FLAT: return "flat";
        case SMOOTH_WHY_EDGE: return "edge";
        case SMOOTH_WHY_RADIUS: return "radius";
        case SMOOTH_CKWHY_MODE3: return "mode3";
        case SMOOTH_CKWHY_ORTHO: return "ortho";
        case SMOOTH_CKWHY_W: return "w";
        case SMOOTH_CKWHY_FACET: return "facet";
        case SMOOTH_CKWHY_BOOKFLAT: return "bookflat";
        case SMOOTH_CKWHY_CROSSKEEP: return "crosskeep";
        default: return "other";
    }
}

struct CkSide { int tri; int a, b; float c[4][3]; };

void ck_side_curve(const SmoothCrackTri &t, int a, int b, float c[4][3]) {
    for (int k = 0; k < 3; ++k) { c[0][k] = t.p[a][k]; c[3][k] = t.p[b][k]; }
    if (t.curved == SMOOTH_CK_STRAIGHT) {
        for (int k = 0; k < 3; ++k) {
            c[1][k] = (t.p[a][k] * 2.0f + t.p[b][k]) * (1.0f / 3.0f);
            c[2][k] = (t.p[a][k] + t.p[b][k] * 2.0f) * (1.0f / 3.0f);
        }
        return;
    }
    // en slots: edge 0-1 -> 0 (at 0), 1 (at 1); 1-2 -> 2 (at 1), 3 (at 2);
    // 0-2 -> 4 (at 0), 5 (at 2)
    int sa = -1, sb = -1;
    if ((a == 0 && b == 1)) { sa = 0; sb = 1; }
    else if (a == 1 && b == 0) { sa = 1; sb = 0; }
    else if (a == 1 && b == 2) { sa = 2; sb = 3; }
    else if (a == 2 && b == 1) { sa = 3; sb = 2; }
    else if (a == 0 && b == 2) { sa = 4; sb = 5; }
    else { sa = 5; sb = 4; }
    pn_edge(t.p[a], t.p[b], t.en[sa], c[1]);
    pn_edge(t.p[b], t.p[a], t.en[sb], c[2]);
}

void ck_screen(const SmoothCrackTri &t, int i, float o[2]) {
    const float w = t.clip[i][2] != 0.0f ? t.clip[i][2] : 1.0f;
    o[0] = (t.clip[i][0] / w * 0.5f + 0.5f) * (float)active_w;
    o[1] = (0.5f - t.clip[i][1] / w * 0.5f) * (float)active_h;
}

struct CkActorRow { int open[64]; int over1; float worst; CkActorRow() : over1(0), worst(0.0f) { memset(open, 0, sizeof open); } };
struct CkHit {
    float px, units, sx, sy;
    int cause;     // index into ck_cause_name
    int e;         // the edge's slot in the edge list
};
enum { CK_STRAIGHT_BASE = 0, CK_BOOK = 40, CK_XMESH, CK_SPLITN, CK_SAMEN, CK_RAWDIFF, CK_NOWELD, CK_NCAUSE };
const char *ck_cause_name(int c, char *buf) {
    if (c < CK_BOOK) { sprintf(buf, "straight_%s", ck_why_name(c)); return buf; }
    switch (c) {
        case CK_BOOK: return "book";
        case CK_XMESH: return "xmesh_splitn";
        case CK_SPLITN: return "splitn";
        case CK_SAMEN: return "samen";
        case CK_RAWDIFF: return "rawdiff";
        case CK_NOWELD: return "noweld";
        default: return "other";
    }
}

void crack_report(uint64_t frame) {
    std::map<std::pair<uint32_t, EKey>, std::vector<CkSide> > edges;
    static const int E0[3] = {0, 1, 2};
    static const int E1[3] = {1, 2, 0};
    for (size_t i = 0; i < g_ck_tris.size(); ++i) {
        const SmoothCrackTri &t = g_ck_tris[i];
        for (int e = 0; e < 3; ++e) {
            int a = E0[e], b = E1[e];
            QKey ka = qkey(t.p[a]), kb = qkey(t.p[b]);
            if (kb < ka) { int s = a; a = b; b = s; QKey q = ka; ka = kb; kb = q; }
            if (!(ka < kb)) continue;   // degenerate
            CkSide sd;
            sd.tri = (int)i; sd.a = a; sd.b = b;
            ck_side_curve(t, a, b, sd.c);
            EKey ek; ek.a = ka; ek.b = kb;
            edges[std::make_pair(t.pic, ek)].push_back(sd);
        }
    }
    int n_cause[CK_NCAUSE] = {0}, n_cause1[CK_NCAUSE] = {0}, n_tj[CK_NCAUSE] = {0};
    float worst_cause[CK_NCAUSE] = {0};
    std::vector<CkHit> hits;
    std::vector<const std::vector<CkSide> *> elist;
    std::map<uint32_t, CkActorRow> ck_rows;     // WELDALL1: by actor pair (low id << 16 | high id)
    std::map<uint32_t, int> ck_actor_tris;
    int shared = 0, with_curved = 0;
    for (std::map<std::pair<uint32_t, EKey>, std::vector<CkSide> >::const_iterator it = edges.begin();
         it != edges.end(); ++it) {
        const std::vector<CkSide> &s = it->second;
        if (s.size() < 2) continue;
        ++shared;
        bool any_curved = false;
        for (size_t j = 0; j < s.size(); ++j)
            if (g_ck_tris[s[j].tri].curved) any_curved = true;
        if (!any_curved) continue;
        ++with_curved;
        // worst gap between any two sides
        float worst = 0.0f; size_t wa = 0, wb = 1;
        for (size_t x = 0; x < s.size(); ++x)
            for (size_t y = x + 1; y < s.size(); ++y)
                for (int k = 1; k < 16; ++k) {
                    const float tt = (float)k / 16.0f;
                    float p0[3], p1[3], d[3];
                    edge_eval(s[x].c, tt, p0);
                    edge_eval(s[y].c, tt, p1);
                    v_sub(p0, p1, d);
                    const float g = sqrtf(v_dot(d, d));
                    if (g > worst) { worst = g; wa = x; wb = y; }
                }
        const SmoothCrackTri &ta = g_ck_tris[s[wa].tri], &tb = g_ck_tris[s[wb].tri];
        // pixels per view unit along this edge, from side wa's corners
        float sa[2], sb[2];
        ck_screen(ta, s[wa].a, sa); ck_screen(ta, s[wa].b, sb);
        float dv[3]; v_sub(ta.p[s[wa].a], ta.p[s[wa].b], dv);
        const float lv = sqrtf(v_dot(dv, dv));
        const float ls = sqrtf((sa[0] - sb[0]) * (sa[0] - sb[0]) + (sa[1] - sb[1]) * (sa[1] - sb[1]));
        const float pxs = lv > 1e-12f ? ls / lv : 0.0f;
        const float gpx = worst * pxs;
        /* WELDALL1: an edge with an end behind the eye, or off the picture,
           cannot be measured in pixels and cannot be seen: not an opening. */
        {
            const float mx = (sa[0] + sb[0]) * 0.5f, my = (sa[1] + sb[1]) * 0.5f;
            if (ta.clip[s[wa].a][2] <= 0.0f || ta.clip[s[wa].b][2] <= 0.0f ||
                mx < 0.0f || my < 0.0f || mx > (float)active_w || my > (float)active_h) continue;
        }
        int cause;
        /* WELDALL1: two sides the weld cannot pair (their raw edges differ:
           one view-space edge drawn from two different raw edges), or a side
           the weld never saw. */
        const bool rawsame =
            (memcmp(ta.raw[s[wa].a], tb.raw[s[wb].a], 6) == 0 && memcmp(ta.raw[s[wa].b], tb.raw[s[wb].b], 6) == 0) ||
            (memcmp(ta.raw[s[wa].a], tb.raw[s[wb].b], 6) == 0 && memcmp(ta.raw[s[wa].b], tb.raw[s[wb].a], 6) == 0);
        if (!rawsame) {
            cause = CK_RAWDIFF;
        } else if (!ta.wseen || !tb.wseen) {
            cause = CK_NOWELD;
        } else if (!ta.curved || !tb.curved) {
            const SmoothCrackTri &st = !ta.curved ? ta : tb;
            cause = st.why >= 0 && st.why < CK_BOOK ? st.why : 39;
        } else if (ta.book || tb.book) {
            cause = CK_BOOK;
        } else {
            const uint32_t ma0 = ta.mg[s[wa].a], ma1 = ta.mg[s[wa].b];
            const uint32_t mb0 = tb.mg[s[wb].a], mb1 = tb.mg[s[wb].b];
            const bool samen = ta.nrm[s[wa].a] == tb.nrm[s[wb].a] && ta.nrm[s[wa].b] == tb.nrm[s[wb].b];
            if (samen) cause = CK_SAMEN;
            else cause = (ma0 != mb0 || ma1 != mb1 || ta.mg[0] != tb.mg[0]) ? CK_XMESH : CK_SPLITN;
        }
        if (gpx <= 0.05f) { if (!ta.curved || !tb.curved) ++n_tj[cause]; continue; }
        ++n_cause[cause];
        if (gpx > 1.0f) ++n_cause1[cause];
        if (gpx > worst_cause[cause]) worst_cause[cause] = gpx;
        {
            const uint32_t aa = ta.actor < tb.actor ? ta.actor : tb.actor;
            const uint32_t ab = ta.actor < tb.actor ? tb.actor : ta.actor;
            CkActorRow &r = ck_rows[(aa << 16) | ab];
            ++r.open[cause];
            if (gpx > 1.0f) ++r.over1;
            if (gpx > r.worst) r.worst = gpx;
        }
        CkHit h;
        h.px = gpx; h.units = worst; h.cause = cause; h.e = (int)elist.size();
        h.sx = (sa[0] + sb[0]) * 0.5f; h.sy = (sa[1] + sb[1]) * 0.5f;
        elist.push_back(&s);
        hits.push_back(h);
        (void)wb;
    }
    char nb[64];
    fprintf(stderr, "[smoothck] f%llu tris %d shared_edges %d with_curved %d opening %d (picture %dx%d) weld_overflow_edges %llu\n",
            (unsigned long long)frame, (int)g_ck_tris.size(), shared, with_curved,
            (int)hits.size(), active_w, active_h, (unsigned long long)smooth_weld_overflow());
    for (int c = 0; c < CK_NCAUSE; ++c)
        if (n_cause[c] || n_tj[c])
            fprintf(stderr, "[smoothck] f%llu cause %-20s open %4d over1px %4d worst %.2f px  tjunction_only %d\n",
                    (unsigned long long)frame, ck_cause_name(c, nb), n_cause[c], n_cause1[c],
                    (double)worst_cause[c], n_tj[c]);
    for (size_t i = 0; i < g_ck_tris.size(); ++i) ++ck_actor_tris[g_ck_tris[i].actor];
    for (std::map<uint32_t, CkActorRow>::const_iterator it = ck_rows.begin(); it != ck_rows.end(); ++it) {
        const uint32_t aa = it->first >> 16, ab = it->first & 0xFFFFu;
        int tot = 0;
        for (int c = 0; c < CK_NCAUSE; ++c) tot += it->second.open[c];
        fprintf(stderr, "[smoothck] f%llu actor %u/%u tris %d/%d open %d over1px %d worst %.2f px causes",
                (unsigned long long)frame, aa, ab, ck_actor_tris[aa], ck_actor_tris[ab], tot, it->second.over1,
                (double)it->second.worst);
        for (int c = 0; c < CK_NCAUSE; ++c)
            if (it->second.open[c]) fprintf(stderr, " %s=%d", ck_cause_name(c, nb), it->second.open[c]);
        fprintf(stderr, "\n");
    }
    // the worst openings, with enough to find them in the picture
    std::vector<int> ord(hits.size());
    for (size_t i = 0; i < ord.size(); ++i) ord[i] = (int)i;
    for (size_t i = 0; i < ord.size(); ++i)
        for (size_t j = i + 1; j < ord.size(); ++j)
            if (hits[ord[j]].px > hits[ord[i]].px) { int q = ord[i]; ord[i] = ord[j]; ord[j] = q; }
    static int top = -1;
    if (top < 0) { const char *e = getenv("SM64DS_SMOOTH_CRACKS_TOP"); top = e ? atoi(e) : 20; }
    for (size_t r = 0; r < ord.size() && (int)r < top; ++r) {
        const CkHit &h = hits[ord[r]];
        const std::vector<CkSide> &s = *elist[h.e];
        fprintf(stderr, "[smoothck] f%llu hit %.2f px %.4f u at %.0f,%.0f %s sides %d:",
                (unsigned long long)frame, (double)h.px, (double)h.units, (double)h.sx, (double)h.sy,
                ck_cause_name(h.cause, nb), (int)s.size());
        for (size_t j = 0; j < s.size() && j < 4; ++j) {
            const SmoothCrackTri &t = g_ck_tris[s[j].tri];
            fprintf(stderr, " [a%u%s %s%s raw %d,%d,%d/%d,%d,%d mg %u/%u n %08x/%08x]", t.actor, t.wseen ? "" : " noweld",
                    t.curved ? "curved" : ck_why_name(t.why), t.book ? "+book" : "",
                    t.raw[s[j].a][0], t.raw[s[j].a][1], t.raw[s[j].a][2],
                    t.raw[s[j].b][0], t.raw[s[j].b][1], t.raw[s[j].b][2],
                    t.mg[s[j].a], t.mg[s[j].b], t.nrm[s[j].a], t.nrm[s[j].b]);
        }
        if (getenv("SM64DS_SMOOTH_CRACKS_EDGE")) {
            const SmoothCrackTri &t0 = g_ck_tris[s[0].tri];
            smooth_weld_dump_edge(t0.raw[s[0].a], t0.raw[s[0].b]);
        }
        fprintf(stderr, "\n");
    }
}
}  // namespace

int smooth_cracks_on() {
    if (g_cracks < 0) {
        const char *e = getenv("SM64DS_SMOOTH_CRACKS");
        g_cracks = (e && *e) ? atoi(e) : 0;
        if (g_cracks < 0) g_cracks = 0;
    }
    return g_cracks;
}

uint32_t g_ck_pic = 0;
void smooth_crack_picture() { ++g_ck_pic; }

/* WELDALL1: which actor's Render is drawing, for the census. The ROM's list
   walker (func_02043fdc) keeps the node it is running in data_020a4b68; the
   node's owner (+8) is the actor, its class id the u16 at +0x0c. A build that
   has no such global (a test exe without the game) reads a null instead. */
extern "C" void *data_020a4b68;
extern "C" void *g_smooth_ck_no_node = 0;
#pragma comment(linker, "/alternatename:_data_020a4b68=_g_smooth_ck_no_node")
namespace {
uint32_t ck_actor_now() {
    const unsigned char *node = (const unsigned char *)data_020a4b68;
    if (!node) return 0xFFFFu;
    const unsigned char *owner = *(const unsigned char *const *)(node + 8);
    if (!owner) return 0xFFFEu;
    return *(const uint16_t *)(owner + 0x0c);
}
}  // namespace
void smooth_crack_tri(const SmoothCrackTri &t) {
    if (g_ck_tris.size() < 400000) {
        g_ck_tris.push_back(t);
        g_ck_tris.back().pic = g_ck_pic;
        g_ck_tris.back().actor = ck_actor_now();
    }
}

namespace { void topo_report(uint64_t frame); }
void weld_report(uint64_t frame);
void weld_frame(uint64_t frame);

void smooth_frame_mark() {
    ++g_counters.frames;
    /* The budget's one decision a picture, BEFORE the counters line so the
       line reports the demand that was just decided on and the level the
       coming picture will be drawn at. */
    budget_pick();
    if (smooth_census_on()) census_report(g_counters.frames);
    weld_frame(g_counters.frames);
    weld_report(g_counters.frames);
    if (smooth_cracks_on()) {
        if (g_counters.frames % (uint64_t)smooth_cracks_on() == 0) crack_report(g_counters.frames);
        g_ck_tris.clear();
    }
    if (counters_on()) { counters_report(g_counters.frames); topo_report(g_counters.frames); }
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
        case SMOOTH_COUNT_DEMAND:      g_demand += n; break;
        default: break;
    }
}


// ---------------------------------------------------------------------------
// THE TOPOLOGY BOOK. smooth.h has the contract; this is the bookkeeping.
// Everything here is in the model's own space (raw VTX coordinates), so a
// mesh is the same mesh wherever it is drawn. The book never forgets until
// it is full, and a full book is emptied whole (an animated mesh whose
// vertices move every frame would otherwise grow it without end), after which
// every mesh is simply unknown again and is smoothed the ordinary way until
// it has been seen to be complete again.
// ---------------------------------------------------------------------------
namespace {

struct TriKey {
    int16_t v[9];
    bool operator==(const TriKey &o) const {
        for (int i = 0; i < 9; ++i) if (v[i] != o.v[i]) return false;
        return true;
    }
};
struct EdgeKey {
    int16_t v[6];
    bool operator==(const EdgeKey &o) const {
        for (int i = 0; i < 6; ++i) if (v[i] != o.v[i]) return false;
        return true;
    }
};
struct VertKey {
    int16_t v[3];
    bool operator==(const VertKey &o) const {
        return v[0] == o.v[0] && v[1] == o.v[1] && v[2] == o.v[2];
    }
};
template <class K, int N>
struct KeyHash {
    size_t operator()(const K &k) const {
        uint32_t h = 2166136261u;
        for (int i = 0; i < N; ++i) {
            h ^= (uint16_t)k.v[i];
            h *= 16777619u;
        }
        return (size_t)h;
    }
};
typedef std::unordered_map<VertKey, std::vector<int>, KeyHash<VertKey, 3> > VertRing;

struct TopoTri {
    int16_t p[3][3];     // corners as first seen
    double nrm[3][3];    // authored normals, unit
    double F[3];         // unit face normal turned to the side the normals lean
    double area;         // twice the triangle's area, model units squared
    double alt;          // its smallest altitude
    int cv[3];           // corner sits on a joint between smooth groups (BOX)
    int nb[3];           // neighbour across edge k (corner k to k+1), -1 none
    int parent;
    double eff[3][3];    // effective corner normals (BOX)
    float en[3][2][3];   // per edge: normal at its first, at its second corner
    int flat;
    int cls;             // the mesh's class once it is final, else -1
};

struct TopoBook {
    std::vector<TopoTri> tris;
    std::vector<std::vector<int> > members;       // at roots
    std::vector<int> last_change;                 // at roots: frame of the last growth
    std::vector<int> final_cls;                   // at roots: -1 = not yet finalized
    std::unordered_map<TriKey, int, KeyHash<TriKey, 9> > by_tri;
    std::unordered_map<EdgeKey, int, KeyHash<EdgeKey, 6> > by_edge;   // first triangle on the edge
    std::vector<signed char> lying;               // at roots: topo_lying's answer, -1 = not computed
};
TopoBook g_topo;
int g_topo_on = -1;
int g_topo_log = -1;
uint64_t g_topo_n[4];
const size_t kTopoMaxTris = 400000;
const int kTopoSettle = 2;            // frames a mesh must hold still before it is judged
const double kCreaseCos = 0.70710678; // 45 degrees: a joint sharper than this is a crease

inline void d_sub(const double a[3], const double b[3], double o[3]) {
    o[0] = a[0] - b[0]; o[1] = a[1] - b[1]; o[2] = a[2] - b[2];
}
inline double d_dot(const double a[3], const double b[3]) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
inline void d_cross(const double a[3], const double b[3], double o[3]) {
    o[0] = a[1] * b[2] - a[2] * b[1];
    o[1] = a[2] * b[0] - a[0] * b[2];
    o[2] = a[0] * b[1] - a[1] * b[0];
}
inline double d_len(const double a[3]) { return sqrt(d_dot(a, a)); }
inline void d_unit(const double a[3], double o[3]) {
    const double l = d_len(a);
    if (l > 1e-30) { o[0] = a[0] / l; o[1] = a[1] / l; o[2] = a[2] / l; }
    else { o[0] = o[1] = o[2] = 0.0; }
}

void topo_decode(uint32_t n, double o[3]) {
    const int32_t a = static_cast<int32_t>(n << 22) >> 22;
    const int32_t b = static_cast<int32_t>((n >> 10) << 22) >> 22;
    const int32_t c = static_cast<int32_t>((n >> 20) << 22) >> 22;
    const double v[3] = {a / 512.0, b / 512.0, c / 512.0};
    d_unit(v, o);
}

void topo_pos(const TopoTri &t, int c, double o[3]) {
    const double fx = 1.0 / 4096.0;
    o[0] = t.p[c][0] * fx; o[1] = t.p[c][1] * fx; o[2] = t.p[c][2] * fx;
}

inline bool same_pos(const int16_t a[3], const int16_t b[3]) {
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

int topo_find(int i) {
    while (g_topo.tris[i].parent != i) {
        g_topo.tris[i].parent = g_topo.tris[g_topo.tris[i].parent].parent;
        i = g_topo.tris[i].parent;
    }
    return i;
}

/* run hunt8, lane SMOOTHCPU2: A DIRECT-MAPPED FRONT FOR by_tri. topo_register
   used to sort the corners, build a nine-short key and walk an
   unordered_map (node chasing, FNV over eighteen bytes) for every lit
   triangle of every pass, hit or miss alike. Nothing about the answer changes:
   a triangle the book already holds is found by its corner positions, in any
   order, and its index never changes until topo_clear. This table remembers
   the index under the corners AS SUBMITTED (no sort), checks all nine shorts
   before it believes a slot, and is emptied by topo_clear. A miss falls
   through to the unchanged lookup. */
/* kind 1 = "this triangle's mesh is final, plain (class NONE) and not lying:
   smooth_topo_lookup answers 0 and touches nothing else", valid while
   `epoch` equals g_topo_epoch. Every write to a class, a finalized flag or a
   mesh's membership bumps the epoch (topo_union, topo_clear, topo_finalize,
   topo_promote), so a kept answer can never outlive the state it read. */
/* SMOOTHWELD1: a kept weld answer (see THE WELD BOOK at the end of this file).
   `gen` 0 means "nothing kept". */
struct WeldFront {
    uint32_t nh;          // the three normals, hashed
    uint32_t enh;         // the edge normals the triangle came with, hashed (0 = its own)
    uint32_t ep;          // the book's epoch the answer was last confirmed at
    uint32_t gen;         // the book's generation the answer belongs to
    uint8_t kind, result;
    uint16_t slot[3];     // its three edges' records (0xFFFF = none)
    uint8_t bnd;          // WELDALL1: which of its edges have this one side only (bit k)
    uint16_t ver[3];      // their versions, low 16 bits
};
struct TopoFast { int16_t p[9]; int idx; uint32_t epoch; int kind; };
uint32_t g_topo_epoch = 1;
int topo_register_slow(const int16_t p[3][3], const uint32_t nrm[3], int frame);
void topo_fast_clear();
enum { TOPO_FAST_N = 1 << 15 };
TopoFast *g_topo_fast = 0;
void topo_fast_clear() {
    if (g_topo_fast)
        for (int i = 0; i < TOPO_FAST_N; ++i) { g_topo_fast[i].idx = -1; g_topo_fast[i].kind = 0; }
}
inline uint32_t topo_fast_slot(const int16_t *q) {
    uint32_t h = 0x9E3779B1u;
    const uint32_t *w = (const uint32_t *)q;     // eighteen bytes: four words and one short
    h = (h ^ w[0]) * 0x85EBCA6Bu; h ^= h >> 15;
    h = (h ^ w[1]) * 0xC2B2AE35u; h ^= h >> 13;
    h = (h ^ w[2]) * 0x27D4EB2Fu; h ^= h >> 15;
    h = (h ^ w[3]) * 0x165667B1u; h ^= h >> 16;
    h = (h ^ (uint32_t)(uint16_t)q[8]) * 0x9E3779B1u; h ^= h >> 15;
    return h & (TOPO_FAST_N - 1);
}

void topo_union(int a, int b, int frame) {
    a = topo_find(a); b = topo_find(b);
    if (a == b) return;
    if (g_topo.members[a].size() < g_topo.members[b].size()) { int t = a; a = b; b = t; }
    g_topo.tris[b].parent = a;
    g_topo.members[a].insert(g_topo.members[a].end(), g_topo.members[b].begin(), g_topo.members[b].end());
    std::vector<int>().swap(g_topo.members[b]);
    g_topo.last_change[a] = frame;
    g_topo.final_cls[a] = -1;
    ++g_topo_epoch;
    g_topo.lying[a] = -1;       // the members changed
    g_topo.lying[b] = -1;
}

void topo_clear() {
    g_topo.tris.clear();
    g_topo.members.clear();
    g_topo.last_change.clear();
    g_topo.final_cls.clear();
    g_topo.by_tri.clear();
    g_topo.by_edge.clear();
    g_topo.lying.clear();
    ++g_topo_epoch;
    topo_fast_clear();
}

inline bool pos_gt(const int16_t a[3], const int16_t b[3]) {
    return a[0] != b[0] ? a[0] > b[0] : (a[1] != b[1] ? a[1] > b[1] : a[2] > b[2]);
}
inline void sort3(const int16_t p[3][3], int ord[3]) {
    ord[0] = 0; ord[1] = 1; ord[2] = 2;
    for (int i = 0; i < 2; ++i)
        for (int j = i + 1; j < 3; ++j)
            if (pos_gt(p[ord[i]], p[ord[j]])) { int t = ord[i]; ord[i] = ord[j]; ord[j] = t; }
}
inline EdgeKey make_edge(const int16_t a[3], const int16_t b[3]) {
    EdgeKey k;
    const bool gt = pos_gt(a, b);
    const int16_t *lo = gt ? b : a, *hi = gt ? a : b;
    for (int i = 0; i < 3; ++i) { k.v[i] = lo[i]; k.v[3 + i] = hi[i]; }
    return k;
}

int topo_register(const int16_t p[3][3], const uint32_t nrm[3], int frame) {
    if (!g_topo_fast) {
        g_topo_fast = (TopoFast *)malloc(sizeof(TopoFast) * TOPO_FAST_N);
        topo_fast_clear();
    }
    TopoFast &fs = g_topo_fast[topo_fast_slot(&p[0][0])];
    if (fs.idx >= 0 && memcmp(fs.p, &p[0][0], sizeof fs.p) == 0) return fs.idx;
    const int idx_found = topo_register_slow(p, nrm, frame);
    memcpy(fs.p, &p[0][0], sizeof fs.p);
    fs.idx = idx_found;
    fs.kind = 0;
    fs.epoch = 0;
    return idx_found;
}

int topo_register_slow(const int16_t p[3][3], const uint32_t nrm[3], int frame) {
    int ord[3];
    sort3(p, ord);
    TriKey tk;
    for (int i = 0; i < 3; ++i) for (int k = 0; k < 3; ++k) tk.v[i * 3 + k] = p[ord[i]][k];
    std::unordered_map<TriKey, int, KeyHash<TriKey, 9> >::iterator it = g_topo.by_tri.find(tk);
    if (it != g_topo.by_tri.end()) return it->second;
    if (g_topo.tris.size() >= kTopoMaxTris) topo_clear();

    const int idx = (int)g_topo.tris.size();
    g_topo.tris.push_back(TopoTri());
    TopoTri &t = g_topo.tris.back();
    memset(&t, 0, sizeof t);
    for (int i = 0; i < 3; ++i) {
        for (int k = 0; k < 3; ++k) t.p[i][k] = p[i][k];
        topo_decode(nrm[i], t.nrm[i]);
        t.nb[i] = -1;
    }
    t.parent = idx;
    t.cls = -1;
    double P[3][3], e1[3], e2[3], c[3];
    for (int i = 0; i < 3; ++i) topo_pos(t, i, P[i]);
    d_sub(P[1], P[0], e1); d_sub(P[2], P[0], e2);
    d_cross(e1, e2, c);
    t.area = d_len(c);
    {
        double L = 0, ee[3];
        for (int a = 0; a < 3; ++a) { d_sub(P[(a + 1) % 3], P[a], ee); if (d_len(ee) > L) L = d_len(ee); }
        t.alt = L > 0 ? t.area / L : 0.0;
    }
    d_unit(c, t.F);
    double avg = 0.0;
    for (int i = 0; i < 3; ++i) avg += d_dot(t.F, t.nrm[i]);
    if (avg < 0.0) { t.F[0] = -t.F[0]; t.F[1] = -t.F[1]; t.F[2] = -t.F[2]; }
    g_topo.by_tri[tk] = idx;
    g_topo.members.push_back(std::vector<int>(1, idx));
    g_topo.last_change.push_back(frame);
    g_topo.final_cls.push_back(-1);
    g_topo.lying.push_back(-1);

    if (g_topo.tris[idx].area > 1e-12) {
        for (int k = 0; k < 3; ++k) {
            const EdgeKey ek = make_edge(g_topo.tris[idx].p[k], g_topo.tris[idx].p[(k + 1) % 3]);
            std::unordered_map<EdgeKey, int, KeyHash<EdgeKey, 6> >::iterator ei = g_topo.by_edge.find(ek);
            if (ei == g_topo.by_edge.end()) {
                g_topo.by_edge[ek] = idx;
            } else {
                const int o = ei->second;
                int ou = -1;
                for (int j = 0; j < 3; ++j)
                    if (make_edge(g_topo.tris[o].p[j], g_topo.tris[o].p[(j + 1) % 3]) == ek) ou = j;
                if (ou >= 0 && g_topo.tris[o].nb[ou] < 0 && g_topo.tris[idx].nb[k] < 0) {
                    g_topo.tris[o].nb[ou] = idx;
                    g_topo.tris[idx].nb[k] = o;
                    topo_union(idx, o, frame);
                }
            }
        }
    }
    return idx;
}

inline double dihedral_cos(const TopoTri &a, const TopoTri &b) { return d_dot(a.F, b.F); }

int corner_of(const TopoTri &t, const int16_t p[3]) {
    for (int c = 0; c < 3; ++c)
        if (same_pos(t.p[c], p)) return c;
    return -1;
}

// How much of its pull a joint corner keeps (see THE JOINT EASE).
inline double topo_joint_pull() { return 0.5; }

// A joint between two triangles that is sharper than kCreaseCos is still one
// curved surface when the artist shaded it as one: the normals each side
// authored at both ends of the shared edge continue into each other, and the
// joint is not a right angle (a barrel's ring of 60 degree sides; a chest's box
// corners are 90 degrees and stay creases whatever their normals say).
inline double topo_joint_max_cos() { return 0.25881904510252074; }   // 75 degrees
inline double topo_agree_cos() { return 0.94; }                      // about 20 degrees
bool topo_edge_smooth(int ti, int k) {
    const TopoTri &t = g_topo.tris[ti];
    const int ui = t.nb[k];
    if (ui < 0) return true;
    const TopoTri &u = g_topo.tris[ui];
    const double dc = dihedral_cos(t, u);
    if (dc >= kCreaseCos) return true;
    if (dc < topo_joint_max_cos()) return false;
    const int kb = (k + 1) % 3;
    const int ca = corner_of(u, t.p[k]), cb = corner_of(u, t.p[kb]);
    if (ca < 0 || cb < 0) return false;
    return d_dot(t.nrm[k], u.nrm[ca]) >= topo_agree_cos() &&
           d_dot(t.nrm[kb], u.nrm[cb]) >= topo_agree_cos();
}

// The effective (welded) normal at corner c of triangle ti: the area weighted
// sum of the face normals of every triangle reachable around that vertex
// without crossing a joint sharper than kCreaseCos.
void topo_weld(int ti, int c, VertRing &vt, double out[3], int *joint) {
    const TopoTri &t0 = g_topo.tris[ti];
    const int16_t *vp = t0.p[c];
    VertKey vk; for (int k = 0; k < 3; ++k) vk.v[k] = vp[k];
    const std::vector<int> &ring = vt[vk];
    std::vector<int> seen(1, ti), st(1, ti);
    while (!st.empty()) {
        const int x = st.back(); st.pop_back();
        const TopoTri &tx = g_topo.tris[x];
        for (int k = 0; k < 3; ++k) {
            const int j = tx.nb[k];
            if (j < 0) continue;
            // only edges that run through this vertex
            if (!same_pos(tx.p[k], vp) && !same_pos(tx.p[(k + 1) % 3], vp)) continue;
            bool dup = false;
            for (size_t q = 0; q < seen.size(); ++q) if (seen[q] == j) { dup = true; break; }
            if (dup) continue;
            if (topo_edge_smooth(x, k)) { seen.push_back(j); st.push_back(j); }
        }
    }
    *joint = seen.size() < ring.size() ? 1 : 0;
    double s[3] = {0, 0, 0};
    for (size_t q = 0; q < seen.size(); ++q) {
        const TopoTri &tj = g_topo.tris[seen[q]];
        for (int k = 0; k < 3; ++k) s[k] += tj.F[k] * tj.area;
    }
    d_unit(s, out);
    if (out[0] == 0.0 && out[1] == 0.0 && out[2] == 0.0)
        for (int k = 0; k < 3; ++k) out[k] = t0.F[k];
}


// A RIM EDGE. Two flat faces meet at a real corner and neither bends along the
// edge by its welded normals, but the artist's own normals at the two ends of
// the edge may still say the edge is one stretch of a curve drawn on the broad
// face (a chest's metal rim, a strap over the lid: each is cut into straight
// bars, each bar its own mesh, and only the shading says they ring a curve).
// The edge then bows INSIDE the plane of the broader of its two faces, with the
// authored normals flattened into that plane, so nothing leaves the surface,
// and only when those normals lean off the edge's perpendicular by less than
// 40 degrees (a corner shaded as the average of three faces leans 45 degrees and
// stays straight). Both triangles on the edge compute the same answer.
bool topo_ring_edge(int ti, int k, double out[2][3]) {
    const double lim = 0.6427876096865393;   // sin 40 degrees
    const TopoTri &t = g_topo.tris[ti];
    const int ui = t.nb[k];
    if (ui < 0) return false;
    const TopoTri &u = g_topo.tris[ui];
    const bool pick_u = (u.area > t.area) || (u.area == t.area && ui < ti);
    const int si = pick_u ? ui : ti;
    const TopoTri &s = g_topo.tris[si];
    int ks = -1;
    if (!pick_u) ks = k;
    else for (int j = 0; j < 3; ++j) if (s.nb[j] == ti) ks = j;
    if (ks < 0) return false;
    const int kb = (ks + 1) % 3;
    double A[3], B[3], O[3], d[3];
    topo_pos(s, ks, A); topo_pos(s, kb, B); topo_pos(s, (ks + 2) % 3, O);
    d_sub(B, A, d);
    const double dl = d_len(d);
    if (dl <= 0.0) return false;
    const double dh[3] = {d[0] / dl, d[1] / dl, d[2] / dl};
    double np[2][3];
    for (int e = 0; e < 2; ++e) {
        const double *n = s.nrm[e == 0 ? ks : kb];
        const double a = d_dot(n, s.F);
        double r[3] = {n[0] - a * s.F[0], n[1] - a * s.F[1], n[2] - a * s.F[2]};
        const double rl = d_len(r);
        if (rl < 0.25) return false;
        np[e][0] = r[0] / rl; np[e][1] = r[1] / rl; np[e][2] = r[2] / rl;
    }
    double eo[3], od[3];
    d_cross(s.F, dh, eo);
    d_sub(O, A, od);
    if (d_dot(eo, od) > 0.0) { eo[0] = -eo[0]; eo[1] = -eo[1]; eo[2] = -eo[2]; }
    // leaning outward on the face, away from the edge's middle, by a small angle
    if (d_dot(np[0], eo) <= 0.0 || d_dot(np[1], eo) <= 0.0) return false;
    const double a0 = d_dot(np[0], dh), a1 = d_dot(np[1], dh);
    if (a0 > 0.0 || a1 < 0.0) return false;
    if (fabs(a0) > lim || fabs(a1) > lim) return false;
    if (!pick_u) {
        for (int q = 0; q < 3; ++q) { out[0][q] = np[0][q]; out[1][q] = np[1][q]; }
    } else if (same_pos(t.p[k], s.p[ks])) {
        for (int q = 0; q < 3; ++q) { out[0][q] = np[0][q]; out[1][q] = np[1][q]; }
    } else {
        for (int q = 0; q < 3; ++q) { out[0][q] = np[1][q]; out[1][q] = np[0][q]; }
    }
    return true;
}

void topo_finalize_box(const std::vector<int> &mem) {
    VertRing vt;   // kept for the ring lookups the weld wants
    for (size_t m = 0; m < mem.size(); ++m) {
        const TopoTri &t = g_topo.tris[mem[m]];
        for (int c = 0; c < 3; ++c) {
            VertKey vk; for (int k = 0; k < 3; ++k) vk.v[k] = t.p[c][k];
            vt[vk].push_back(mem[m]);
        }
    }
    for (size_t m = 0; m < mem.size(); ++m) {
        for (int c = 0; c < 3; ++c) topo_weld(mem[m], c, vt, g_topo.tris[mem[m]].eff[c], &g_topo.tris[mem[m]].cv[c]);
    }
    for (size_t m = 0; m < mem.size(); ++m) {
        const int ti = mem[m];
        TopoTri &t = g_topo.tris[ti];
        int flat = 1;
        for (int k = 0; k < 3; ++k) {
            const int kb = (k + 1) % 3;
            const double *ca = t.eff[k], *cb = t.eff[kb];
            double A[3], B[3], d[3];
            topo_pos(t, k, A); topo_pos(t, kb, B);
            d_sub(B, A, d);
            const int u = t.nb[k];
            double bn[2][3];
            if (u >= 0 && !topo_edge_smooth(ti, k)) {
                // A crease. Both sides must draw the edge as ONE curve. Each
                // side's welded normals at the two ends say how its own surface
                // runs along the edge; the side that bends along the edge
                // (a lid's rim in the plane of its flat end, a barrel's ring)
                // gives the curve, a flat side is perpendicular to it and gives
                // a straight line. Two flat sides: the straight edge.
                const TopoTri &tu = g_topo.tris[u];
                const int cA = corner_of(tu, t.p[k]), cB = corner_of(tu, t.p[kb]);
                const double dl = d_len(d);
                const double bt = dl > 0.0 ? fabs(d_dot(d, t.eff[k])) + fabs(d_dot(d, t.eff[kb])) : 0.0;
                double bu = 0.0;
                if (cA >= 0 && cB >= 0 && dl > 0.0) bu = fabs(d_dot(d, tu.eff[cA])) + fabs(d_dot(d, tu.eff[cB]));
                const bool take_u = (bu > bt) || (bu == bt && u < ti);
                if ((bt <= 1e-3 * dl && bu <= 1e-3 * dl)) {
                    ca = cb = t.F;
                    if (topo_ring_edge(ti, k, bn)) { ca = bn[0]; cb = bn[1]; }
                } else if (take_u && cA >= 0 && cB >= 0) {
                    for (int q = 0; q < 3; ++q) { bn[0][q] = tu.eff[cA][q]; bn[1][q] = tu.eff[cB][q]; }
                    ca = bn[0]; cb = bn[1];
                } else {
                    ca = t.eff[k]; cb = t.eff[kb];
                }
            }
            for (int q = 0; q < 3; ++q) { t.en[k][0][q] = (float)ca[q]; t.en[k][1][q] = (float)cb[q]; }
            const double dl = d_len(d);
            if (dl > 0.0 && (fabs(d_dot(d, ca)) > 1e-3 * dl || fabs(d_dot(d, cb)) > 1e-3 * dl)) flat = 0;
        }
        t.flat = flat;
    }

    // THE JOINT EASE. A lid facet that runs from one end cap to the other has
    // every corner on a joint, and the end caps (and the separate metal rim
    // pieces over them) stay the flat polygon the DS drew. The full pull of
    // such an edge would lift the lid's ends clear of that rim in a thin
    // flare, so the pull at a joint corner is eased by kJointPull.
    for (size_t m = 0; m < mem.size(); ++m) {
        TopoTri &t = g_topo.tris[mem[m]];
        for (int k = 0; k < 3; ++k) {
            const int kb = (k + 1) % 3;
            double A[3], B[3], d[3];
            topo_pos(t, k, A); topo_pos(t, kb, B);
            d_sub(B, A, d);
            const double dl = d_len(d);
            if (dl <= 0.0) continue;
            const double dh[3] = {d[0] / dl, d[1] / dl, d[2] / dl};
            for (int sd = 0; sd < 2; ++sd) {
                {
                    // the same answer from both triangles on the edge
                    const int cc = sd == 0 ? k : kb;
                    int jt = t.cv[cc];
                    const int uu = t.nb[k];
                    if (uu >= 0) {
                        const int cu = corner_of(g_topo.tris[uu], t.p[cc]);
                        if (cu >= 0 && g_topo.tris[uu].cv[cu]) jt = 1;
                    }
                    if (!jt) continue;
                }
                double n[3] = {t.en[k][sd][0], t.en[k][sd][1], t.en[k][sd][2]};
                const double a = d_dot(n, dh);
                const double a2 = a * topo_joint_pull();
                double r[3] = {n[0] - a * dh[0], n[1] - a * dh[1], n[2] - a * dh[2]};
                const double rl = d_len(r);
                const double sc = rl > 1e-12 ? sqrt(1.0 - a2 * a2) / rl : 0.0;
                for (int q = 0; q < 3; ++q) t.en[k][sd][q] = (float)(a2 * dh[q] + r[q] * sc);
            }
        }
    }

    // THE SLIVER GUARD. An edge's pull is |d . n| / 3 of its length, which is
    // nothing on a roundish piece and a spike on a long thin triangle, whose
    // whole width it can exceed. Both triangles on an edge cap it at half the
    // smaller of their narrowest altitudes, the same number from either side,
    // by turning the end normals towards perpendicular until the pull fits.
    for (size_t m = 0; m < mem.size(); ++m) {
        TopoTri &t = g_topo.tris[mem[m]];
        for (int k = 0; k < 3; ++k) {
            const int kb = (k + 1) % 3;
            double A[3], B[3], d[3];
            topo_pos(t, k, A); topo_pos(t, kb, B);
            d_sub(B, A, d);
            const double dl = d_len(d);
            if (dl <= 0.0) continue;
            const int u = t.nb[k];
            double h = t.alt;
            if (u >= 0 && g_topo.tris[u].alt < h) h = g_topo.tris[u].alt;
            double amax = 1.5 * h / dl;           // |a| = |d . n| / |d|, pull = |d| |a| / 3 <= h / 2
            if (amax >= 1.0) continue;
            const double dh[3] = {d[0] / dl, d[1] / dl, d[2] / dl};
            for (int sd = 0; sd < 2; ++sd) {
                double n[3] = {t.en[k][sd][0], t.en[k][sd][1], t.en[k][sd][2]};
                const double a = d_dot(n, dh);
                if (fabs(a) <= amax) continue;
                const double a2 = a < 0 ? -amax : amax;
                double r[3] = {n[0] - a * dh[0], n[1] - a * dh[1], n[2] - a * dh[2]};
                const double rl = d_len(r);
                const double sc = rl > 1e-12 ? sqrt(1.0 - a2 * a2) / rl : 0.0;
                for (int q = 0; q < 3; ++q) t.en[k][sd][q] = (float)(a2 * dh[q] + r[q] * sc);
            }
        }
    }

    // Flat is decided last: a triangle whose edges all ended up straight is
    // the flat triangle, and the caller draws it as the DS did.
    for (size_t m = 0; m < mem.size(); ++m) {
        TopoTri &t = g_topo.tris[mem[m]];
        int flat = 1;
        for (int k = 0; k < 3 && flat; ++k) {
            const int kb = (k + 1) % 3;
            double A[3], B[3], d[3];
            topo_pos(t, k, A); topo_pos(t, kb, B);
            d_sub(B, A, d);
            const double dl = d_len(d);
            if (dl <= 0.0) continue;
            for (int sd = 0; sd < 2; ++sd) {
                const double n[3] = {t.en[k][sd][0], t.en[k][sd][1], t.en[k][sd][2]};
                if (fabs(d_dot(d, n)) > 1e-3 * dl) flat = 0;
            }
        }
        t.flat = flat;
    }
}

int topo_finalize_fan(const std::vector<int> &mem) {
    const int n = (int)mem.size();
    // centre: the corner shared by every triangle
    int16_t cen[3] = {0, 0, 0};
    int found = 0;
    const TopoTri &t0 = g_topo.tris[mem[0]];
    for (int c = 0; c < 3 && !found; ++c) {
        int all = 1;
        for (int m = 1; m < n; ++m)
            if (corner_of(g_topo.tris[mem[m]], t0.p[c]) < 0) { all = 0; break; }
        if (all) { found = 1; for (int k = 0; k < 3; ++k) cen[k] = t0.p[c][k]; }
    }
    if (!found) return 0;
    const double C[3] = {cen[0] / 4096.0, cen[1] / 4096.0, cen[2] / 4096.0};
    const double Fp[3] = {t0.F[0], t0.F[1], t0.F[2]};
    // an in-plane frame from the first rim corner
    double r0[3] = {0, 0, 0};
    for (int c = 0; c < 3; ++c)
        if (!same_pos(t0.p[c], cen)) { double P[3]; topo_pos(t0, c, P); d_sub(P, C, r0); break; }
    const double dp = d_dot(r0, Fp);
    for (int k = 0; k < 3; ++k) r0[k] -= Fp[k] * dp;
    double u1[3], u2[3];
    d_unit(r0, u1);
    d_cross(Fp, u1, u2);
    // the rim in that frame (each rim corner is met twice: harmless)
    std::vector<double> rx, ry;
    for (int m = 0; m < n; ++m) {
        const TopoTri &t = g_topo.tris[mem[m]];
        for (int c = 0; c < 3; ++c) {
            if (same_pos(t.p[c], cen)) continue;
            double P[3], d[3]; topo_pos(t, c, P); d_sub(P, C, d);
            rx.push_back(d_dot(d, u1)); ry.push_back(d_dot(d, u2));
        }
    }
    if (rx.empty()) return 0;
    double sxx = 0, syy = 0, sxy = 0;
    for (size_t q = 0; q < rx.size(); ++q) { sxx += rx[q] * rx[q]; syy += ry[q] * ry[q]; sxy += rx[q] * ry[q]; }
    const double th = 0.5 * atan2(2.0 * sxy, sxx - syy);
    const double ct = cos(th), st = sin(th);
    // the ellipse A x'^2 + B y'^2 = 1 through the rim, in its principal axes
    double a11 = 0, a12 = 0, a22 = 0, b1 = 0, b2 = 0;
    for (size_t q = 0; q < rx.size(); ++q) {
        const double xp = rx[q] * ct + ry[q] * st, yp = -rx[q] * st + ry[q] * ct;
        const double X = xp * xp, Y = yp * yp;
        a11 += X * X; a12 += X * Y; a22 += Y * Y; b1 += X; b2 += Y;
    }
    double A = 0, B = 0;
    const double det = a11 * a22 - a12 * a12;
    if (fabs(det) > 1e-12 * (a11 * a22 + 1e-30)) { A = (b1 * a22 - b2 * a12) / det; B = (a11 * b2 - a12 * b1) / det; }
    if (!(A > 0.0 && B > 0.0)) {
        double r2 = 0;
        for (size_t q = 0; q < rx.size(); ++q) r2 += rx[q] * rx[q] + ry[q] * ry[q];
        r2 /= (double)rx.size();
        A = B = 1.0 / r2;
    }
    for (int m = 0; m < n; ++m) {
        TopoTri &t = g_topo.tris[mem[m]];
        double rim[3][3]; int isrim[3];
        for (int c = 0; c < 3; ++c) {
            isrim[c] = !same_pos(t.p[c], cen);
            double P[3], d[3]; topo_pos(t, c, P); d_sub(P, C, d);
            const double x = d_dot(d, u1), y = d_dot(d, u2);
            const double xp = x * ct + y * st, yp = -x * st + y * ct;
            // the ellipse's outward normal, turned back from its axes into (u1, u2)
            const double gx = A * xp * ct - B * yp * st;
            const double gy = A * xp * st + B * yp * ct;
            const double g[3] = {u1[0] * gx + u2[0] * gy, u1[1] * gx + u2[1] * gy, u1[2] * gx + u2[2] * gy};
            d_unit(g, rim[c]);
        }
        for (int k = 0; k < 3; ++k) {
            const int kb = (k + 1) % 3;
            const int spoke = !(isrim[k] && isrim[kb]);
            for (int q = 0; q < 3; ++q) {
                t.en[k][0][q] = (float)(spoke ? Fp[q] : rim[k][q]);
                t.en[k][1][q] = (float)(spoke ? Fp[q] : rim[kb][q]);
            }
        }
        t.flat = 0;
    }
    return 1;
}

int topo_classify(const std::vector<int> &mem, double *o_cop, double *o_lb, double *o_lv) {
    const int n = (int)mem.size();
    *o_cop = *o_lb = *o_lv = 0.0;
    if (n < 4 || n > 400) return SMOOTH_CLASS_NONE;
    int edges = 0, cop = 0, allcop = 1;
    int open_edges = 0, hard = 0, square = 0;
    int lb = 0, lv = 0;
    std::unordered_map<VertKey, int, KeyHash<VertKey, 3> > vset;
    for (int m = 0; m < n; ++m) {
        const TopoTri &t = g_topo.tris[mem[m]];
        double lean = 1.0;
        for (int c = 0; c < 3; ++c) {
            const double d = d_dot(t.F, t.nrm[c]);
            if (d < lean) lean = d;
            VertKey vk; for (int k = 0; k < 3; ++k) vk.v[k] = t.p[c][k];
            vset[vk] = 1;
        }
        if (lean < 0.75) ++lb;
        if (lean < 0.40) ++lv;
        for (int k = 0; k < 3; ++k) {
            const int u = t.nb[k];
            if (u < 0 && t.area > 1e-12) ++open_edges;
            if (u > mem[m]) {
                const double dc = dihedral_cos(t, g_topo.tris[u]);
                ++edges;
                if (dc >= 0.99756) ++cop;           // within 4 degrees
                if (dc <= 0.5) ++hard;              // a 60 degree edge or sharper
                if (dc <= 0.2 && dc >= -0.2) ++square;   // a right angle, give or take 12 degrees
                if (dc < 0.99863) allcop = 0;       // beyond 3 degrees
            }
        }
    }
    const int nv = (int)vset.size();
    const double fcop = edges ? (double)cop / edges : 0.0;
    *o_cop = fcop; *o_lb = (double)lb / n; *o_lv = (double)lv / n;
    if (n >= 8 && fcop >= 0.35 && *o_lb >= 0.5) return SMOOTH_CLASS_BOX;
    /* SMOOTHREND1: A BOX WHOSE SHADING IS ONLY SLIGHTLY ROUNDED. The ice block is a 12 triangle cube (the 6 diagonals the only
       coplanar edges, one third of the 18) whose corner normals lean 23 degrees off their faces, so the "lying" count above is
       zero and the curved patch pillowed the whole cube. What gives a box away is its geometry, not its normals: every edge that
       is not a flat diagonal is a right angle. Left as drawn, like every other box here. */
    {
        const int bent = edges - cop;
        if (n >= 8 && fcop >= 0.30 && bent > 0 && square * 10 >= bent * 9) return SMOOTH_CLASS_BOX;
    }
    if (n >= 10 && n <= 40 && *o_lv >= 0.9 && fcop < 0.35) return SMOOTH_CLASS_FACET;
    /* A SMALL CLOSED SOLID WITH HARD EDGES, SHADED SMOOTH: the title screen
       star (60 triangles, every edge shared, 30 edges 45 degrees or sharper and
       the authored normals agreeing across all 90). The artist rounded the
       shading of a faceted shape, so the normals lean far off the faces and the
       curved patch pillows every facet out past its own plane, over the flat
       eyes. Left as drawn, like the in-level star, whose 20 facets are the same
       thing with more extreme normals. BOX is asked first, so a hard-edged box
       keeps its own treatment, and a round solid (a sphere has no edge past 45
       degrees) is nowhere near the hard-edge share. The 48 floor keeps the
       smaller rounded parts out of it (Luigi's gloves are 24 triangles, Yoshi's
       shoes and heel 32, 32 and 24, the odd bird or token 12 to 18): they pass
       the other tests and must keep their round puffed treatment. */
    if (n >= 48 && n <= 80 && open_edges == 0 && edges > 0 && *o_lb >= 0.5 && fcop < 0.35 &&
        hard * 20 >= edges * 3) return SMOOTH_CLASS_FACET;
    if (n <= 12 && edges > 0 && allcop && nv == n + 1) return SMOOTH_CLASS_FAN;
    return SMOOTH_CLASS_NONE;
}

void topo_finalize(int root) {
    const std::vector<int> &mem = g_topo.members[root];
    if (g_topo_log < 0) g_topo_log = getenv("SM64DS_SMOOTH_TOPO_LOG") ? 1 : 0;
    double cop = 0, lb = 0, lv = 0;
    int cls = topo_classify(mem, &cop, &lb, &lv);
    if (cls == SMOOTH_CLASS_BOX) {
        topo_finalize_box(mem);
    } else if (cls == SMOOTH_CLASS_FAN) { if (!topo_finalize_fan(mem)) cls = SMOOTH_CLASS_NONE; }
    for (size_t m = 0; m < mem.size(); ++m) g_topo.tris[mem[m]].cls = cls;
    g_topo.final_cls[root] = cls;
    ++g_topo_epoch;
    if (g_topo_log < 0) g_topo_log = getenv("SM64DS_SMOOTH_TOPO_LOG") ? 1 : 0;
    if (g_topo_log && (cls != SMOOTH_CLASS_NONE || mem.size() >= 8)) {
        const TopoTri &t = g_topo.tris[mem[0]];
        fprintf(stderr, "[smoothtopo] mesh of %d tris class %d (flat edges %.2f, lying %.2f, lying badly %.2f) first corner %d %d %d\n",
                (int)mem.size(), cls, cop, lb, lv, t.p[0][0], t.p[0][1], t.p[0][2]);
    }
}

}  // namespace

int smooth_topo_on() {
    if (g_topo_on < 0) {
        const char *e = getenv("SM64DS_SMOOTH_TOPO");
        g_topo_on = (e && e[0] == '0') ? 0 : 1;
    }
    return g_topo_on;
}


void smooth_topo_count(int what) {
    if (what >= 0 && what < 4) ++g_topo_n[what];
}

namespace {
// SAME DRAW, SAME MODEL. A chest is a box, a curved lid AND the metal straps
// and rim pieces laid over it; those are meshes of their own, often two
// triangles each, whose statistics say nothing, so on their own they stay
// NONE and would be drawn puffed against the now flat chest. What ties them to
// the chest is that they are drawn one after another in the same picture:
// the game bumps the matrix generation for each piece of one model, so the
// pieces of a model sit in a few consecutive generations. The first time a
// BOX mesh is seen under a generation, every lying NONE mesh seen in that
// picture within kNear generations of it is promoted to BOX for good.
struct GroupInfo {
    int frame;
    std::map<uint32_t, int> box;                              // generations holding a BOX mesh
    std::vector<std::pair<int, uint32_t> > pending;            // lying NONE meshes seen, with their generation
};
GroupInfo g_topo_groups;
const uint32_t kNear = 3;
// run hunt8 SMOOTHCPU2: the last generation known to be in g_topo_groups.box,
// so consecutive triangles of one piece skip the map lookup. Cleared with it.
uint32_t g_box_last = 0;
bool g_box_last_ok = false;

// TRIANGLES STITCHED ACROSS TWO MATRICES (a seam between two bones) cannot be
// entered in the book: their corners are in two different spaces. When one is
// drawn right beside a BOX mesh it is a seam of that same model, and drawn
// puffed it flares out of the flat box; so it is remembered by its corners
// and left as the DS drew it, from the next picture on.
std::unordered_map<TriKey, int, KeyHash<TriKey, 9> > g_topo_refuse;
struct CrossPending { TriKey key; uint32_t g[3]; };
std::vector<CrossPending> g_topo_cross_pending;
int g_topo_cross_frame = -1;

inline bool near_gen(uint32_t a, uint32_t b) { return a + kNear >= b && a <= b + kNear; }

void topo_promote(int root) {
    const std::vector<int> &mem = g_topo.members[root];
    if (mem.size() < 1 || mem.size() > 400) return;
    topo_finalize_box(mem);
    for (size_t m = 0; m < mem.size(); ++m) g_topo.tris[mem[m]].cls = SMOOTH_CLASS_BOX;
    g_topo.final_cls[root] = SMOOTH_CLASS_BOX;
    ++g_topo_epoch;
    if (g_topo_log < 0) g_topo_log = getenv("SM64DS_SMOOTH_TOPO_LOG") ? 1 : 0;
    if (g_topo_log) {
        const TopoTri &t = g_topo.tris[mem[0]];
        fprintf(stderr, "[smoothtopo] mesh of %d tris promoted to box (drawn with a box) first corner %d %d %d\n",
                (int)mem.size(), t.p[0][0], t.p[0][1], t.p[0][2]);
    }
}

// Does the mesh at `root` lie about its normals enough to be worth promoting?
// (run hunt8 SMOOTHCPU2: the answer is kept per root; it only depends on the
// members, and every change to them clears it in topo_union.)
int topo_lying_calc(int root);
int topo_lying(int root) {
    if (g_topo.lying[root] >= 0) return g_topo.lying[root];
    const int r = topo_lying_calc(root);
    g_topo.lying[root] = (signed char)r;
    return r;
}
int topo_lying_calc(int root) {
    const std::vector<int> &mem = g_topo.members[root];
    if (mem.size() < 1 || mem.size() > 400) return 0;
    int lb = 0;
    for (size_t m = 0; m < mem.size(); ++m) {
        const TopoTri &t = g_topo.tris[mem[m]];
        double lean = 1.0;
        for (int c = 0; c < 3; ++c) { const double d = d_dot(t.F, t.nrm[c]); if (d < lean) lean = d; }
        if (lean < 0.75) ++lb;
    }
    return lb * 2 >= (int)mem.size();
}
}  // namespace

int smooth_topo_lookup(const int16_t p[3][3], const uint32_t nrm[3], uint32_t group, SmoothOverride *out) {
    /* run hunt8 SMOOTHCPU2: the kept "plain mesh" answer (see TopoFast) */
    if (g_topo_fast) {
        const TopoFast &kf = g_topo_fast[topo_fast_slot(&p[0][0])];
        if (kf.kind == 1 && kf.epoch == g_topo_epoch && memcmp(kf.p, &p[0][0], sizeof kf.p) == 0)
            return 0;
    }
    const int frame = (int)g_counters.frames;
    const int idx = topo_register(p, nrm, frame);
    int root = topo_find(idx);
    if (g_topo.final_cls[root] < 0) {
        if (frame - g_topo.last_change[root] < kTopoSettle) return 0;
        topo_finalize(root);
    }
    if (g_topo_groups.frame != frame) {
        g_topo_groups.frame = frame;
        g_topo_groups.box.clear();
        g_box_last_ok = false;
        g_topo_groups.pending.clear();
    }
    {
        GroupInfo &gi = g_topo_groups;
        const int fc = g_topo.final_cls[root];
        if (fc == SMOOTH_CLASS_BOX && !(g_box_last_ok && g_box_last == group)) {
            if (gi.box.find(group) == gi.box.end()) {
                gi.box[group] = 1;
                std::vector<std::pair<int, uint32_t> > keep;
                for (size_t r = 0; r < gi.pending.size(); ++r) {
                    const int rr = topo_find(gi.pending[r].first);
                    const uint32_t pg = gi.pending[r].second;
                    if (g_topo.final_cls[rr] != SMOOTH_CLASS_NONE) continue;
                    if (pg + kNear >= group && pg <= group + kNear) topo_promote(rr);
                    else keep.push_back(gi.pending[r]);
                }
                gi.pending.swap(keep);
                for (size_t r = 0; r < g_topo_cross_pending.size(); ++r)
                    for (int q = 0; q < 3; ++q)
                        if (near_gen(g_topo_cross_pending[r].g[q], group)) { g_topo_refuse[g_topo_cross_pending[r].key] = 1; break; }
            }
            g_box_last = group;
            g_box_last_ok = true;
        }
        if (fc == SMOOTH_CLASS_NONE && topo_lying(root)) {
            bool near_box = false;
            std::map<uint32_t, int>::iterator it = gi.box.lower_bound(group > kNear ? group - kNear : 0);
            if (it != gi.box.end() && it->first <= group + kNear) near_box = true;
            if (near_box) topo_promote(root);
            else if (gi.pending.size() < 256) gi.pending.push_back(std::make_pair(root, group));
        }
    }
    const TopoTri &t = g_topo.tris[idx];
    if (t.cls <= SMOOTH_CLASS_NONE) {
        if (g_topo.final_cls[root] == SMOOTH_CLASS_NONE && !topo_lying(root)) {
            TopoFast &kf = g_topo_fast[topo_fast_slot(&p[0][0])];
            if (kf.idx == idx && memcmp(kf.p, &p[0][0], sizeof kf.p) == 0) {
                kf.kind = 1;
                kf.epoch = g_topo_epoch;
            }
        }
        return 0;
    }
    out->cls = t.cls;
    out->flat = t.flat;
    if (t.cls == SMOOTH_CLASS_FACET) { memset(out->en, 0, sizeof out->en); return 1; }
    // the six edge normals, in the order of the caller's corners
    int rc[3];
    for (int i = 0; i < 3; ++i) {
        rc[i] = corner_of(t, p[i]);
        if (rc[i] < 0) return 0;
    }
    static const int pk[6][2] = {{0, 1}, {1, 0}, {1, 2}, {2, 1}, {0, 2}, {2, 0}};
    for (int s = 0; s < 6; ++s) {
        const int ri = rc[pk[s][0]], rj = rc[pk[s][1]];
        int k = -1;
        if ((ri + 1) % 3 == rj) k = ri;
        else if ((rj + 1) % 3 == ri) k = rj;
        else return 0;
        const int slot = (k == ri) ? 0 : 1;
        for (int q = 0; q < 3; ++q) out->en[s][q] = t.en[k][slot][q];
    }
    return 1;
}


int smooth_topo_cross(const int16_t p[3][3], uint32_t g0, uint32_t g1, uint32_t g2) {
    const int frame = (int)g_counters.frames;
    int ord[3];
    sort3(p, ord);
    TriKey tk;
    for (int i = 0; i < 3; ++i) for (int k = 0; k < 3; ++k) tk.v[i * 3 + k] = p[ord[i]][k];
    if (g_topo_refuse.find(tk) != g_topo_refuse.end()) return 1;
    if (g_topo_groups.frame != frame) {
        g_topo_groups.frame = frame;
        g_topo_groups.box.clear();
        g_box_last_ok = false;
        g_topo_groups.pending.clear();
    }
    if (g_topo_cross_frame != frame) { g_topo_cross_frame = frame; g_topo_cross_pending.clear(); }
    const uint32_t gs[3] = {g0, g1, g2};
    for (int q = 0; q < 3; ++q) {
        std::map<uint32_t, int>::iterator it = g_topo_groups.box.lower_bound(gs[q] > kNear ? gs[q] - kNear : 0);
        if (it != g_topo_groups.box.end() && it->first <= gs[q] + kNear) {
            if (g_topo_refuse.size() < 200000) g_topo_refuse[tk] = 1;
            return 1;
        }
    }
    if (g_topo_cross_pending.size() < 1024) {
        CrossPending cp; cp.key = tk; cp.g[0] = g0; cp.g[1] = g1; cp.g[2] = g2;
        g_topo_cross_pending.push_back(cp);
    }
    return 0;
}

namespace {
void topo_report(uint64_t frame) {
    fprintf(stderr, "[smoothtopo] f%llu tris %llu box %llu facet %llu fan %llu flatkept %llu\n",
            (unsigned long long)frame, (unsigned long long)g_topo.tris.size(),
            (unsigned long long)g_topo_n[0], (unsigned long long)g_topo_n[1],
            (unsigned long long)g_topo_n[2], (unsigned long long)g_topo_n[3]);
}
}  // namespace

// ---------------------------------------------------------------------------
// THE WELD BOOK (smooth.h has the contract). Two fixed tables and nothing
// allocated per frame: the edges, open-addressed on the raw edge, and a
// direct-mapped front on the raw triangle that keeps each triangle's answer
// until one of its three edges changes. In a settled scene a triangle costs
// one probe of the front and three version compares.
// ---------------------------------------------------------------------------
namespace {

int g_weld_on = -1;

struct WeldSide {
    uint32_t id;       // the triangle (its sorted corners and normals)
    uint32_t kind;     // SMOOTH_WELD_*
    float n[2][3];     // its normal at the edge's low end, at its high end
};
enum { WELD_SIDES = 4 };
struct WeldEdge {
    int16_t k[6];      // low end, high end (pos_gt order)
    uint16_t used;     // 1 = slot holds an edge; | 2 = a side beyond WELD_SIDES came (overflow);
                       // | 4 = another raw edge is drawn on top of it (WELDALL1, smooth_weld_partner)
    uint16_t nside;
    uint32_t ver;
    WeldSide side[WELD_SIDES];
    /* THE PICTURE'S ANSWER (WELDALL1). When this record changes during a
       picture, the answer it gave before the change is kept here for the
       rest of that picture: a side drawn earlier in the picture already used
       it, so every side drawn later uses it too and the two still meet. The
       next picture draws every side by the new record. fstraight: the kept
       answer is a straight edge; else fn holds the normal at each end. */
    uint32_t fpic;
    uint32_t fstraight;
    float fn[2][3];
};
enum { WELD_EDGE_N = 1 << 16, WELD_PROBE = 64 };
WeldEdge *g_weld_edges = 0;
uint32_t g_weld_count = 0;
uint32_t g_weld_gen = 1;           // bumped whenever the edge table is emptied

/* THE FRONT. One cache line a triangle, two ways a set, so a scene of a few
   thousand triangles does not evict itself. `ep` is the book's epoch the
   answer was last confirmed at: while no edge anywhere has changed, that one
   compare is the whole check. When something has changed, the triangle's own
   three edges are asked (their versions, low 16 bits) and only a triangle one
   of whose edges moved is worked out again. */
struct alignas(64) WeldTri {
    int16_t p[9];
    uint32_t used;
    WeldFront w;
};
struct WeldOut { float out[6][3]; uint32_t id; };
enum { WELD_TRI_N = 1 << 15 };
WeldTri *g_weld_tris = 0;
WeldOut *g_weld_outs = 0;         // the answers of g_weld_tris, by index
uint32_t g_weld_epoch = 1;
uint32_t g_weld_flips = 0;        // a recorded side changed, or an edge overflowed: churn, not growth         // bumped whenever any edge record changes
uint64_t g_ws_calls, g_ws_fast, g_ws_ver, g_ws_slow, g_wm_hit, g_wm_miss;
uint64_t g_ws_overflow;            // edges that met a fifth side, since boot
uint64_t g_ws_clears;              // books emptied by a scene change, since boot
uint64_t g_ws_clear_full;          // WELDALL1: books emptied at 75 percent load, since boot
uint64_t g_ws_clear_probe;         // WELDALL1: books emptied because a probe found no slot, since boot
uint32_t g_weld_pic = 1;           // WELDALL1: the picture being drawn (ticks and in-betweens)
uint32_t g_weld_last_bnd = 0;      // WELDALL1: the last answer's one-sided edges (bit k) ...
uint16_t g_weld_last_slot[3];      // ... and its three edges' records
uint64_t g_ws_partners;            // WELDALL1: raw edges found drawn on top of another, since boot
uint32_t g_weld_conflicts = 0;     // WELDALL1: a straight side met an edge kept curved this picture
uint64_t g_ws_frozen, g_ws_complied; // WELDALL1: answers kept at a change; sides that drew a kept answer
int g_weld_provisional = 0;        // WELDALL1: the last answer used a kept one that the record no longer gives
int g_weld_scene_pending = 0;

inline uint32_t weld_fnv(const void *p, size_t n, uint32_t h = 2166136261u) {
    const unsigned char *b = (const unsigned char *)p;
    for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 16777619u; }
    return h;
}

void weld_clear() {
    for (int i = 0; i < WELD_EDGE_N; ++i) g_weld_edges[i].used = 0;
    g_weld_count = 0;
    ++g_weld_gen;
    ++g_weld_epoch;
}

// A word-at-a-time mix for the front's checks. The corners are compared in
// full; the normals and the edge normals only by this 32-bit mix, which over a
// scene of a few thousand triangles does not collide in practice.
inline uint32_t weld_words(const uint32_t *w, int n) {
    uint32_t h = 0x9E3779B1u;
    for (int i = 0; i < n; ++i) { h = (h ^ w[i]) * 0x85EBCA6Bu; h ^= h >> 15; }
    return h;
}

inline uint32_t weld_tri_hash(const int16_t *q, const uint32_t *n, uint32_t salt) {
    const uint32_t *w = (const uint32_t *)q;     // eighteen bytes: four words and one short
    uint32_t h = 0x9E3779B1u ^ salt;
    h = (h ^ w[0]) * 0x85EBCA6Bu; h ^= h >> 15;
    h = (h ^ w[1]) * 0xC2B2AE35u; h ^= h >> 13;
    h = (h ^ w[2]) * 0x27D4EB2Fu; h ^= h >> 15;
    h = (h ^ w[3]) * 0x165667B1u; h ^= h >> 16;
    h = (h ^ (uint32_t)(uint16_t)q[8]) * 0x9E3779B1u; h ^= h >> 15;
    h = (h ^ n[0]) * 0x85EBCA6Bu; h ^= h >> 13;
    h = (h ^ n[1]) * 0xC2B2AE35u; h ^= h >> 15;
    h = (h ^ n[2]) * 0x27D4EB2Fu; h ^= h >> 16;
    return h;
}

// The edge's slot, found or made. -1 when the table is full around it.
int weld_slot(const int16_t lo[3], const int16_t hi[3]) {
    int16_t k[6] = {lo[0], lo[1], lo[2], hi[0], hi[1], hi[2]};
    uint32_t h = weld_fnv(k, sizeof k) & (WELD_EDGE_N - 1);
    for (int q = 0; q < WELD_PROBE; ++q) {
        WeldEdge &e = g_weld_edges[(h + q) & (WELD_EDGE_N - 1)];
        if (!e.used) {
            memcpy(e.k, k, sizeof k);
            e.used = 1; e.nside = 0; e.ver = 1; e.fpic = 0;
            ++g_weld_count;
            return (int)((h + q) & (WELD_EDGE_N - 1));
        }
        if (memcmp(e.k, k, sizeof k) == 0) return (int)((h + q) & (WELD_EDGE_N - 1));
    }
    return -1;
}

// The corners of edge k and the slots of the six edge normals they own.
const int kWeldCa[3] = {0, 1, 2}, kWeldCb[3] = {1, 2, 0};
const int kWeldSa[3] = {0, 2, 5}, kWeldSb[3] = {1, 3, 4};

inline bool weld_same(const float a[3], const float b[3]) {
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

/* An edge's answer from its record as it stands: straight, or per end a
   normal (the sides' sum when they differ, the first side's when they agree;
   `weld` says which). Every side computes it from the same record and gets
   the same bits. */
void weld_edge_answer(const WeldEdge &e, bool &straight, float n[2][3], bool weld[2]) {
    straight = (e.used & 6) != 0;
    weld[0] = weld[1] = false;
    for (int j = 0; j < e.nside; ++j)
        if (e.side[j].kind != SMOOTH_WELD_CURVED) straight = true;
    if (e.nside == 0) straight = true;
    for (int end = 0; end < 2 && !straight; ++end) {
        bool agree = true;
        for (int j = 1; j < e.nside; ++j)
            if (!weld_same(e.side[j].n[end], e.side[0].n[end])) agree = false;
        if (agree) { memcpy(n[end], e.side[0].n[end], sizeof(float) * 3); continue; }
        for (int a = 0; a < e.nside && !straight; ++a)
            for (int b = a + 1; b < e.nside; ++b)
                if (v_dot(e.side[a].n[end], e.side[b].n[end]) < (float)kCreaseCos) { straight = true; break; }
        if (straight) break;
        int ord[WELD_SIDES];
        for (int j = 0; j < e.nside; ++j) ord[j] = j;
        for (int a = 1; a < e.nside; ++a)
            for (int b = a; b > 0 && e.side[ord[b]].id < e.side[ord[b - 1]].id; --b) {
                const int t = ord[b]; ord[b] = ord[b - 1]; ord[b - 1] = t;
            }
        float sum[3] = {0.0f, 0.0f, 0.0f};
        for (int j = 0; j < e.nside; ++j)
            for (int q = 0; q < 3; ++q) sum[q] += e.side[ord[j]].n[end][q];
        const float l = sqrtf(v_dot(sum, sum));
        if (l < 1e-6f) { straight = true; break; }
        for (int q = 0; q < 3; ++q) n[end][q] = sum[q] / l;
        weld[end] = true;
    }
}

/* Keep the record's answer for the rest of this picture, before it changes. */
inline void weld_freeze(WeldEdge &e) {
    if (e.fpic == g_weld_pic || e.nside == 0) return;
    bool st; float n[2][3]; bool w[2];
    weld_edge_answer(e, st, n, w);
    e.fpic = g_weld_pic;
    e.fstraight = st ? 1u : 0u;
    if (!st) memcpy(e.fn, n, sizeof n);
    ++g_ws_frozen;
}



}  // namespace

long long g_ws_ns;     // time inside smooth_weld, SM64DS_SMOOTH_PROF only
/* THE CHURN GUARD (smooth.h). Measured per tick at the frame mark: how many
   recorded sides CHANGED (or edges overflowed) during the tick just drawn. New
   sides are growth (a level loading, an object coming into view) and do not
   count; sides that keep changing are churn, and churn is what lets two sides
   of one seam take answers from different book states in one picture. Above the threshold the next
   SM64DS_SMOOTH_WELD_HOLD ticks (default 3, and their in-betweens) draw unwelded, the plain path the
   game had before the weld; every tick of churn re-arms the hold. The book keeps
   learning meanwhile, so the weld comes back on a settled book.
   SM64DS_SMOOTH_WELD_CHURN sets the threshold (changed sides a tick). */
uint32_t g_guard_ep = 0;
uint32_t g_guard_ep2 = 0;
int g_guard_grow_run = 0;
int g_guard_hold = 0;
uint32_t g_guard_max = 0;          // largest move a tick in the report window
uint64_t g_guard_ticks = 0;        // guarded ticks in the report window
int weld_churn_limit() {
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_SMOOTH_WELD_CHURN"); v = (e && *e) ? atoi(e) : 200; }
    return v;
}
int weld_guard_hold() {
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_SMOOTH_WELD_HOLD"); v = (e && *e) ? atoi(e) : 3; }
    return v;
}
void weld_frame(uint64_t frame) {
    (void)frame;
    /* WELDALL1: with one answer per edge per picture (weld_freeze), a book
       that changes during a picture no longer parts the sides of a seam, so
       churn and growth alone are harmless; what still parts them is a side
       drawn straight on an edge kept curved. The guard now counts those.
       SM64DS_SMOOTH_WELD_GUARD=old restores the churn and growth triggers. */
    static int old_guard = -1;
    if (old_guard < 0) { const char *e = getenv("SM64DS_SMOOTH_WELD_GUARD"); old_guard = (e && strcmp(e, "old") == 0) ? 1 : 0; }
    const uint32_t moved = old_guard ? g_weld_flips - g_guard_ep : g_weld_conflicts;
    const uint32_t grew = old_guard ? g_weld_epoch - g_guard_ep2 : 0u;
    g_weld_conflicts = 0;
    ++g_weld_pic;
    if (moved > g_guard_max) g_guard_max = moved;
    /* growth that does not stop: a mesh whose raw vertices move every tick
       (a model animated by vertex, a menu page) makes new edges every tick,
       and the two sides of a seam on it are answered from different book
       states. A level loading grows the book for a tick or two and stops. */
    g_guard_grow_run = ((int)grew > weld_churn_limit()) ? g_guard_grow_run + 1 : 0;
    if ((int)moved > weld_churn_limit() || g_guard_grow_run >= 3) g_guard_hold = weld_guard_hold();
    else if (g_guard_hold > 0) --g_guard_hold;
    if (g_guard_hold > 0) ++g_guard_ticks;
    if (g_weld_scene_pending) {
        g_weld_scene_pending = 0;
        if (g_weld_edges && g_weld_count) { weld_clear(); ++g_ws_clears; }
    }
    g_guard_ep = g_weld_flips;
    g_guard_ep2 = g_weld_epoch;
}

void weld_report(uint64_t frame) {
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_SMOOTH_WELD_STATS") ? 1 : 0;
    if (!on || frame % 100) return;
    fprintf(stderr, "[smoothweld] f%llu calls %llu settled %llu reconfirmed %llu worked %llu "
            "edges %u epoch %u time %.3f ms a tick, replays read %llu asked %llu, overflow edges %llu, scene clears %llu, "
            "churn max %u a tick, guarded ticks %llu, full clears %llu, probe clears %llu, kept %llu complied %llu partners %llu\n",
            (unsigned long long)frame, (unsigned long long)g_ws_calls, (unsigned long long)g_ws_fast,
            (unsigned long long)g_ws_ver, (unsigned long long)g_ws_slow,
            g_weld_count, g_weld_epoch, g_ws_ns / 100.0 / 1e6,
            (unsigned long long)g_wm_hit, (unsigned long long)g_wm_miss,
            (unsigned long long)g_ws_overflow, (unsigned long long)g_ws_clears,
            g_guard_max, (unsigned long long)g_guard_ticks,
            (unsigned long long)g_ws_clear_full, (unsigned long long)g_ws_clear_probe,
            (unsigned long long)g_ws_frozen, (unsigned long long)g_ws_complied, (unsigned long long)g_ws_partners);
    g_ws_frozen = g_ws_complied = 0;
    g_guard_max = 0;
    g_guard_ticks = 0;
    g_ws_calls = g_ws_fast = g_ws_ver = g_ws_slow = g_wm_hit = g_wm_miss = 0;
    g_ws_ns = 0;
}

uint64_t smooth_frame_no() { return g_counters.frames; }
uint32_t smooth_weld_epoch() { return g_weld_epoch; }
int smooth_weld_guarded() { return g_guard_hold > 0; }
void smooth_weld_picture() { ++g_weld_pic; }
void smooth_weld_conflict() { ++g_weld_conflicts; }
uint32_t smooth_weld_pic() { return g_weld_pic; }
uint32_t smooth_weld_last_bnd(uint16_t slot[3]) {
    memcpy(slot, g_weld_last_slot, sizeof g_weld_last_slot);
    return g_weld_last_bnd;
}
void smooth_weld_partner(uint16_t a, uint16_t b) {
    if (!g_weld_edges || a >= WELD_EDGE_N || b >= WELD_EDGE_N || a == b) return;
    const uint16_t sl[2] = {a, b};
    for (int i = 0; i < 2; ++i) {
        WeldEdge &e = g_weld_edges[sl[i]];
        if (!e.used || (e.used & 4)) continue;
        weld_freeze(e);
        e.used |= 4;
        ++e.ver;
        ++g_weld_epoch;
        ++g_ws_partners;
    }
}
int smooth_weld_provisional() { return g_weld_provisional; }
void smooth_weld_scene_change() { g_weld_scene_pending = 1; }
uint64_t smooth_weld_overflow() { return g_ws_overflow; }
void smooth_weld_memo_count(int hit) { if (hit) ++g_wm_hit; else ++g_wm_miss; }

/* WELDALL1: the record of one raw edge, for the crack book's hit lines. */
void smooth_weld_dump_edge(const int16_t a[3], const int16_t b[3]) {
    if (!g_weld_edges) { fprintf(stderr, " {book empty}"); return; }
    const bool gt = a[0] != b[0] ? a[0] > b[0] : a[1] != b[1] ? a[1] > b[1] : a[2] > b[2];
    const int16_t *lo = gt ? b : a, *hi = gt ? a : b;
    int16_t k[6] = {lo[0], lo[1], lo[2], hi[0], hi[1], hi[2]};
    uint32_t h = weld_fnv(k, sizeof k) & (WELD_EDGE_N - 1);
    for (int q = 0; q < WELD_PROBE; ++q) {
        const WeldEdge &e = g_weld_edges[(h + q) & (WELD_EDGE_N - 1)];
        if (!e.used) break;
        if (memcmp(e.k, k, sizeof k) != 0) continue;
        fprintf(stderr, " {edge used %u ver %u sides %u:", e.used, e.ver, e.nside);
        for (int j = 0; j < e.nside; ++j)
            fprintf(stderr, " %08x/k%u n(%.3f,%.3f,%.3f)(%.3f,%.3f,%.3f)", e.side[j].id, e.side[j].kind,
                    e.side[j].n[0][0], e.side[j].n[0][1], e.side[j].n[0][2], e.side[j].n[1][0], e.side[j].n[1][1], e.side[j].n[1][2]);
        fprintf(stderr, "}");
        return;
    }
    fprintf(stderr, " {edge not in book}");
}

int smooth_weld_on() {
    if (g_weld_on < 0) {
        const char *e = getenv("SM64DS_SMOOTH_WELD");
        g_weld_on = (e && e[0] == '0') ? 0 : (e && e[0] == '2') ? 2 : 1;   /* 2: book kept, never applied (cost A/B) */
    }
    return g_weld_on;
}

namespace {
int weld_impl(const int16_t p[3][3], const uint32_t nrm[3], int kind,
              const float (*en)[3], float out[6][3], uint32_t *id);
}

int smooth_weld(const int16_t p[3][3], const uint32_t nrm[3], int kind,
                const float (*en)[3], float out[6][3], uint32_t *id) {
    if (!smooth_prof_on()) return weld_impl(p, nrm, kind, en, out, id);
    const long long t0 = smooth_prof_ticks();
    const int r = weld_impl(p, nrm, kind, en, out, id);
    g_ws_ns += smooth_prof_ticks() - t0;
    return r;
}

namespace {
/* A kept answer, if it is still good: -1 when it is not this triangle's or an
   edge it was worked out against has changed since. */
inline int weld_front_answer(WeldFront &fw, const WeldOut &fo, uint32_t nh, int kind, uint32_t enh,
                             float out[6][3], uint32_t *id) {
    if (fw.gen != g_weld_gen || fw.nh != nh || fw.kind != (uint8_t)kind || fw.enh != enh) return -1;
    if (fw.ep == g_weld_epoch) {
        ++g_ws_fast;
    } else {
        for (int k = 0; k < 3; ++k)
            if (fw.slot[k] != 0xFFFF && (uint16_t)g_weld_edges[fw.slot[k]].ver != fw.ver[k]) return -1;
        fw.ep = g_weld_epoch;
        ++g_ws_ver;
    }
    if (fw.result) { memcpy(out, fo.out, sizeof fo.out); *id = fo.id; }
    g_weld_provisional = 0;
    g_weld_last_bnd = fw.bnd;
    memcpy(g_weld_last_slot, fw.slot, sizeof g_weld_last_slot);
    return fw.result;
}
}  // namespace

namespace {
int weld_impl(const int16_t p[3][3], const uint32_t nrm[3], int kind,
              const float (*en)[3], float out[6][3], uint32_t *id) {
    if (!g_weld_edges) {
        g_weld_edges = (WeldEdge *)calloc(WELD_EDGE_N, sizeof(WeldEdge));
        g_weld_tris = (WeldTri *)_aligned_malloc(sizeof(WeldTri) * WELD_TRI_N, 64);
        g_weld_outs = (WeldOut *)calloc(WELD_TRI_N, sizeof(WeldOut));
        if (!g_weld_edges || !g_weld_tris || !g_weld_outs) { g_weld_on = 0; return 0; }
        memset(g_weld_tris, 0, sizeof(WeldTri) * WELD_TRI_N);
    }
    ++g_ws_calls;
    const uint32_t frame = (uint32_t)g_counters.frames;
    /* THE FRONT: this triangle's answer, while none of its edges has changed.
       Looked up BEFORE the normals are decoded: a settled triangle needs none
       of that. */
    const uint32_t nh = weld_words(nrm, 3) | 1u;
    const uint32_t enh_in = (kind == SMOOTH_WELD_CURVED && en) ? (weld_words((const uint32_t *)&en[0][0], 18) | 1u) : 0u;
    WeldTri *own = 0;
    {
        const uint32_t set = weld_tri_hash(&p[0][0], nrm, (uint32_t)kind) & (WELD_TRI_N - 2);
        for (int way = 0; way < 2 && !own; ++way) {
            WeldTri &c = g_weld_tris[set + way];
            if (c.w.gen == g_weld_gen && c.w.nh == nh && c.w.kind == (uint8_t)kind &&
                memcmp(c.p, &p[0][0], sizeof c.p) == 0) own = &c;
        }
        if (own) {
            own->used = frame;
            const int r = weld_front_answer(own->w, g_weld_outs[own - g_weld_tris], nh, kind, enh_in, out, id);
            if (r >= 0) return r;
        } else {
            WeldTri &w0 = g_weld_tris[set], &w1 = g_weld_tris[set + 1];
            own = (w0.w.gen != g_weld_gen) ? &w0 : (w1.w.gen != g_weld_gen) ? &w1
                : ((int32_t)(w0.used - w1.used) <= 0 ? &w0 : &w1);
            own->w.gen = 0;
            own->used = frame;
        }
    }
    ++g_ws_slow;

    /* The six normals this triangle would curve with, raw model space, unit. */
    float in[6][3];
    uint32_t enh = 0;
    if (kind == SMOOTH_WELD_CURVED) {
        if (en) {
            memcpy(in, en, sizeof in);
            enh = weld_words((const uint32_t *)&in[0][0], 18) | 1u;
        } else {
            static const int corner[6] = {0, 1, 1, 2, 0, 2};
            float n[3][3];
            for (int c = 0; c < 3; ++c) {
                const int32_t a = static_cast<int32_t>(nrm[c] << 22) >> 22;
                const int32_t b = static_cast<int32_t>((nrm[c] >> 10) << 22) >> 22;
                const int32_t d = static_cast<int32_t>((nrm[c] >> 20) << 22) >> 22;
                const float v[3] = {a / 512.0f, b / 512.0f, d / 512.0f};
                const float l = sqrtf(v_dot(v, v));
                const float il = l > 1e-12f ? 1.0f / l : 0.0f;
                n[c][0] = v[0] * il; n[c][1] = v[1] * il; n[c][2] = v[2] * il;
            }
            for (int s = 0; s < 6; ++s) for (int q = 0; q < 3; ++q) in[s][q] = n[corner[s]][q];
        }
    }

    if (g_weld_count > (uint32_t)(WELD_EDGE_N / 4 * 3)) { weld_clear(); ++g_ws_clear_full; }

    /* The side's id: its corners in a fixed order and their normals, so the
       same triangle has the same id whichever way the list walks it. */
    uint32_t sid;
    {
        int ord[3];
        sort3(p, ord);
        sid = 2166136261u;
        for (int i = 0; i < 3; ++i) {
            sid = weld_fnv(p[ord[i]], 6, sid);
            sid = weld_fnv(&nrm[ord[i]], 4, sid);
        }
        /* the kind is part of the side: one raw triangle drawn twice in a
           picture, once curved and once left flat (a menu's mirrored copy, an
           in-between whose matrix the book refused), is two sides, and the
           edge then stays straight for both instead of flipping between them */
        sid = weld_fnv(&kind, sizeof kind, sid);
    }

    /* ENTER the three sides. */
    int slot[3];
    int lo_is_a[3];
    for (int k = 0; k < 3; ++k) {
        const int ca = kWeldCa[k], cb = kWeldCb[k];
        slot[k] = -1;
        lo_is_a[k] = 1;
        if (same_pos(p[ca], p[cb])) continue;
        lo_is_a[k] = pos_gt(p[ca], p[cb]) ? 0 : 1;
        const int16_t *lo = lo_is_a[k] ? p[ca] : p[cb];
        const int16_t *hi = lo_is_a[k] ? p[cb] : p[ca];
        const int s = weld_slot(lo, hi);
        /* a full neighbourhood: empty the book (every answer kept against it
           goes with the generation) and draw this one as planned */
        if (s < 0) { weld_clear(); ++g_ws_clear_probe; g_weld_provisional = 0; g_weld_last_bnd = 0; return 0; }
        slot[k] = s;
        WeldEdge &e = g_weld_edges[s];
        WeldSide me;
        me.id = sid;
        me.kind = (uint32_t)kind;
        memset(me.n, 0, sizeof me.n);
        if (kind == SMOOTH_WELD_CURVED) {
            const float *na = in[kWeldSa[k]], *nb = in[kWeldSb[k]];
            const float *nl = lo_is_a[k] ? na : nb, *nh = lo_is_a[k] ? nb : na;
            for (int q = 0; q < 3; ++q) { me.n[0][q] = nl[q]; me.n[1][q] = nh[q]; }
        }
        int j = 0;
        while (j < e.nside && e.side[j].id != sid) ++j;
        if (j < e.nside) {
            if (memcmp(&e.side[j], &me, sizeof me) != 0) {
                weld_freeze(e);
                e.side[j] = me; ++e.ver; ++g_weld_epoch; ++g_weld_flips;
            }
        } else if (e.nside < WELD_SIDES) {
            weld_freeze(e);
            e.side[e.nside++] = me;
            ++e.ver;
            ++g_weld_epoch;
        } else if (!(e.used & 2)) {
            /* A FIFTH SIDE is not recorded. The edge is then drawn straight
               on every side (the rule below reads the flag), which is the
               one answer every side can agree on without knowing the fifth. */
            weld_freeze(e);
            e.used |= 2;
            ++e.ver;
            ++g_weld_epoch;
            ++g_ws_overflow;
            ++g_weld_flips;
            {   /* SM64DS_SMOOTH_WELD_STATS: the first few crowded edges, for the report */
                static int said = 0;
                if (getenv("SM64DS_SMOOTH_WELD_STATS") && said < 12) {
                    ++said;
                    fprintf(stderr, "[smoothweld] crowded edge %d,%d,%d/%d,%d,%d sides", e.k[0], e.k[1], e.k[2], e.k[3], e.k[4], e.k[5]);
                    for (int q = 0; q < e.nside; ++q) fprintf(stderr, " %08x/k%u", e.side[q].id, e.side[q].kind);
                    fprintf(stderr, " new %08x/k%d tri %d,%d,%d %d,%d,%d %d,%d,%d nrm %08x %08x %08x\n", sid, kind,
                            p[0][0], p[0][1], p[0][2], p[1][0], p[1][1], p[1][2], p[2][0], p[2][1], p[2][2], nrm[0], nrm[1], nrm[2]);
                }
            }
        }
    }

    /* DECIDE each edge, the same way from every side of it. */
    int result = 0;
    bool provisional = false;
    if (kind == SMOOTH_WELD_CURVED) {
        memcpy(out, in, sizeof in);
        for (int k = 0; k < 3; ++k) {
            if (slot[k] < 0) continue;
            const WeldEdge &e = g_weld_edges[slot[k]];
            if (e.fpic == g_weld_pic) {
                /* THE PICTURE'S ANSWER: this edge changed after a side of it
                   was drawn in this picture; draw what that side drew. */
                const int sa = kWeldSa[k], sb = kWeldSb[k];
                const int ea = lo_is_a[k] ? 0 : 1, eb = 1 - ea;
                bool st; float fn[2][3]; bool fw[2];
                weld_edge_answer(e, st, fn, fw);
                if (e.fstraight) {
                    memset(out[sa], 0, sizeof out[sa]);
                    memset(out[sb], 0, sizeof out[sb]);
                    result = 1;
                    if (!st) provisional = true;
                } else {
                    for (int q = 0; q < 3; ++q) { out[sa][q] = e.fn[ea][q]; out[sb][q] = e.fn[eb][q]; }
                    if (!weld_same(out[sa], in[sa]) || !weld_same(out[sb], in[sb])) result = 1;
                    if (st || memcmp(fn, e.fn, sizeof fn) != 0) provisional = true;
                }
                ++g_ws_complied;
                continue;
            }
            bool straight = (e.used & 6) != 0;
            if (e.nside < 2 && !straight) continue;
            for (int j = 0; j < e.nside; ++j)
                if (e.side[j].kind != SMOOTH_WELD_CURVED) straight = true;
            float w[2][3];
            bool weld[2] = {false, false};
            for (int end = 0; end < 2 && !straight; ++end) {
                bool agree = true;
                for (int j = 1; j < e.nside; ++j)
                    if (!weld_same(e.side[j].n[end], e.side[0].n[end])) agree = false;
                if (agree) continue;
                for (int a = 0; a < e.nside && !straight; ++a)
                    for (int b = a + 1; b < e.nside; ++b)
                        if (v_dot(e.side[a].n[end], e.side[b].n[end]) < (float)kCreaseCos) {
                            straight = true; break;
                        }
                if (straight) break;
                /* the sum in the order of the sides' ids: every side adds
                   the same floats in the same order and gets the same bits */
                int ord[WELD_SIDES];
                for (int j = 0; j < e.nside; ++j) ord[j] = j;
                for (int a = 1; a < e.nside; ++a)
                    for (int b = a; b > 0 && e.side[ord[b]].id < e.side[ord[b - 1]].id; --b) {
                        const int t = ord[b]; ord[b] = ord[b - 1]; ord[b - 1] = t;
                    }
                float sum[3] = {0.0f, 0.0f, 0.0f};
                for (int j = 0; j < e.nside; ++j)
                    for (int q = 0; q < 3; ++q) sum[q] += e.side[ord[j]].n[end][q];
                const float l = sqrtf(v_dot(sum, sum));
                if (l < 1e-6f) { straight = true; break; }
                for (int q = 0; q < 3; ++q) w[end][q] = sum[q] / l;
                weld[end] = true;
            }
            const int sa = kWeldSa[k], sb = kWeldSb[k];
            if (straight) {
                memset(out[sa], 0, sizeof out[sa]);
                memset(out[sb], 0, sizeof out[sb]);
                result = 1;
                continue;
            }
            /* end 0 is the low end: corner a when lo_is_a */
            const int ea = lo_is_a[k] ? 0 : 1, eb = 1 - ea;
            if (weld[ea]) { for (int q = 0; q < 3; ++q) out[sa][q] = w[ea][q]; result = 1; }
            if (weld[eb]) { for (int q = 0; q < 3; ++q) out[sb][q] = w[eb][q]; result = 1; }
        }
    } else {
        /* A SIDE DRAWN STRAIGHT on an edge kept curved for this picture (a
           curved neighbour came first, the record learned of this side only
           now): it follows the kept curve along that edge and keeps its other
           edges straight, for this picture only. The next picture's record
           makes the edge straight on both sides. */
        memset(out, 0, sizeof(float) * 18);
        for (int k = 0; k < 3; ++k) {
            if (slot[k] < 0) continue;
            const WeldEdge &e = g_weld_edges[slot[k]];
            if (e.fpic != g_weld_pic || e.fstraight) continue;
            const int sa = kWeldSa[k], sb = kWeldSb[k];
            const int ea = lo_is_a[k] ? 0 : 1, eb = 1 - ea;
            for (int q = 0; q < 3; ++q) { out[sa][q] = e.fn[ea][q]; out[sb][q] = e.fn[eb][q]; }
            result = 1;
            provisional = true;
            ++g_ws_complied;
        }
    }

    /* Keep the answer against the edges' versions as they stand now. */
    WeldFront w;
    memset(&w, 0, sizeof w);
    w.nh = nh;
    w.enh = enh;
    w.kind = (uint8_t)kind;
    w.gen = g_weld_gen;
    w.ep = g_weld_epoch;
    for (int k = 0; k < 3; ++k) {
        w.slot[k] = slot[k] >= 0 ? (uint16_t)slot[k] : (uint16_t)0xFFFF;
        w.ver[k] = slot[k] >= 0 ? (uint16_t)g_weld_edges[slot[k]].ver : 0;
    }
    w.result = (uint8_t)result;
    w.bnd = 0;
    if (kind == SMOOTH_WELD_CURVED)
        for (int k = 0; k < 3; ++k)
            if (slot[k] >= 0 && g_weld_edges[slot[k]].nside == 1 && !(g_weld_edges[slot[k]].used & 6))
                w.bnd |= (uint8_t)(1u << k);
    g_weld_last_bnd = w.bnd;
    memcpy(g_weld_last_slot, w.slot, sizeof g_weld_last_slot);
    WeldOut wo;
    memset(&wo, 0, sizeof wo);
    if (result) {
        memcpy(wo.out, out, sizeof wo.out);
        wo.id = weld_fnv(out, sizeof wo.out);
        *id = wo.id;
    }
    g_weld_provisional = provisional ? 1 : 0;
    if (provisional) {
        /* an answer only this picture gives: never kept, the next picture asks again */
        own->w.gen = 0;
        return result;
    }
    memcpy(own->p, &p[0][0], sizeof own->p);
    own->w = w;
    if (result) g_weld_outs[own - g_weld_tris] = wo;
    return result;
}
}  // namespace

}  // namespace ntr
