// Standalone selftest for the PN-triangle smoothing kernel (ntr/smooth).
//
// It needs no CMake target, no build lock, no ROM and no window: compile it
// with the two source files named in out/MDL/smooth_selftest_MDL.cmd and run
// the exe. Every row prints the number it measured, not just a verdict, so a
// regression says how far it moved.
//
// The four tests at the bottom of app/src/subdiv.rs are all here, plus the two
// the Rust could not express because it welded instead of trusting authored
// normals:
//
//   rust tri_count_quadruples_per_level      -> COUNT
//   rust shared_edge_is_welded_watertight    -> WATERTIGHT (the weld is gone;
//                                               the property is now that both
//                                               neighbours tessellate the
//                                               shared edge to the same curve,
//                                               which is stronger -- it holds
//                                               with no shared state at all)
//   rust flat_input_stays_flat_and_...       -> FLAT + WINDING + FLATTILT
//   rust curved_input_bulges_but_keeps_...   -> CORNERS + BULGE
//   rust pick_bone deterministic tie         -> EDGEORDER (its geometric
//                                               equivalent: an edge's control
//                                               points depend only on the
//                                               unordered endpoint pair, so
//                                               naming the edge the other way
//                                               round gives the same curve)
//   (new) policy decisions                   -> POLICY
//   (new) degenerate input                   -> DEGENERATE
//
// And the five rows the shape store brought with it (run hd2, lane MDL2):
//
//   SHAPESPLIT  the policy split in two still gives the one verdict
//   REPLAY      a patch built in local space and pushed through a similarity
//               is the patch built from the transformed corners
//   KEYMTX      the key is the raw input, so one triangle under two matrices
//               is one entry
//   COLLIDE     four thousand entries, every lookup answered with its own
//               grid: the full key is compared, never just its hash
//   CAPEVICT    the pool is bounded and the bound is enforced

#include "ntr/smooth.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using ntr::SmoothVertex;
using ntr::SmoothPatch;
using ntr::SmoothPolicy;

static int g_fail = 0;

static void row(const char *name, bool ok, const char *fmt, double a,
                double b = 0.0) {
    char detail[256];
    snprintf(detail, sizeof detail, fmt, a, b);
    printf("%-12s %-6s %s\n", name, ok ? "PASS" : "FAIL", detail);
    if (!ok) ++g_fail;
}

static void row(const char *name, bool ok, const char *text) {
    printf("%-12s %-6s %s\n", name, ok ? "PASS" : "FAIL", text);
    if (!ok) ++g_fail;
}

// ---------------------------------------------------------------- collector

struct Tri { SmoothVertex a, b, c; };
struct Bag { std::vector<Tri> t; };

static void collect(void *ctx, const SmoothVertex &a, const SmoothVertex &b,
                    const SmoothVertex &c) {
    Tri tri = {a, b, c};
    static_cast<Bag *>(ctx)->t.push_back(tri);
}

static SmoothVertex vtx(float x, float y, float z, float nx, float ny,
                        float nz, float u, float v, uint32_t col) {
    SmoothVertex s;
    s.x = x; s.y = y; s.z = z;
    s.nx = nx; s.ny = ny; s.nz = nz;
    s.u = u; s.v = v; s.color = col;
    return s;
}

static float dist(const SmoothVertex &a, const SmoothVertex &b) {
    const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

// The unit quad of the Rust tests: z = 0, two triangles sharing edge 0-2,
// every normal +z so the surface is genuinely flat.
static void flat_quad(SmoothVertex q[4]) {
    q[0] = vtx(0, 0, 0,  0, 0, 1,  0.0f, 0.0f, 0xFF204060u);
    q[1] = vtx(1, 0, 0,  0, 0, 1,  1.0f, 0.0f, 0xFF204060u);
    q[2] = vtx(1, 1, 0,  0, 0, 1,  1.0f, 1.0f, 0xFF204060u);
    q[3] = vtx(0, 1, 0,  0, 0, 1,  0.0f, 1.0f, 0xFF204060u);
}

// ---------------------------------------------------------------- the tests

// COUNT: tf^2 sub-triangles per input triangle, so a two-triangle quad gives
// 2*tf^2. This is the rust tri_count_quadruples_per_level.
static void test_count() {
    SmoothVertex q[4];
    flat_quad(q);
    bool ok = true;
    int last = 0;
    for (int level = 1; level <= ntr::SMOOTH_MAX_LEVEL; ++level) {
        const int tf = 1 << level;
        Bag bag;
        ntr::smooth_subdivide(q[0], q[1], q[2], tf, collect, &bag);
        ntr::smooth_subdivide(q[0], q[2], q[3], tf, collect, &bag);
        last = (int)bag.t.size();
        if (last != 2 * tf * tf) ok = false;
        if (ntr::smooth_tri_count(tf) != tf * tf) ok = false;
    }
    // tf = 1 is the pass-through: one triangle in, the same one out.
    Bag one;
    ntr::smooth_subdivide(q[0], q[1], q[2], 1, collect, &one);
    if (one.t.size() != 1) ok = false;
    if (one.t[0].a.x != q[0].x || one.t[0].c.color != q[2].color) ok = false;
    row("COUNT", ok, "level 3 quad -> %.0f tris (want 128); tf=1 passes through",
        (double)last);
}

// FLAT: a planar input with agreeing normals comes out planar. Axis-aligned,
// so the assertion is EXACT equality, not a tolerance.
static void test_flat() {
    SmoothVertex q[4];
    flat_quad(q);
    Bag bag;
    ntr::smooth_subdivide(q[0], q[1], q[2], 4, collect, &bag);
    ntr::smooth_subdivide(q[0], q[2], q[3], 4, collect, &bag);
    float worst = 0.0f;
    bool exact = true;
    for (size_t i = 0; i < bag.t.size(); ++i) {
        const SmoothVertex *v[3] = {&bag.t[i].a, &bag.t[i].b, &bag.t[i].c};
        for (int k = 0; k < 3; ++k) {
            if (v[k]->z != 0.0f) exact = false;
            const float d = fabsf(v[k]->z);
            if (d > worst) worst = d;
        }
    }
    row("FLAT", exact, "max |z| over %.0f sub-tris = %g (want exactly 0)",
        (double)bag.t.size(), (double)worst);
}

// WINDING: every sub-triangle keeps the parent's front face. The quad is CCW
// seen from +z, so every sub-triangle's cross product must have nz > 0.
static void test_winding() {
    SmoothVertex q[4];
    flat_quad(q);
    Bag bag;
    ntr::smooth_subdivide(q[0], q[1], q[2], 4, collect, &bag);
    ntr::smooth_subdivide(q[0], q[2], q[3], 4, collect, &bag);
    int backwards = 0;
    float worst = 1e30f;
    for (size_t i = 0; i < bag.t.size(); ++i) {
        const SmoothVertex &a = bag.t[i].a, &b = bag.t[i].b, &c = bag.t[i].c;
        const float nz = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (nz <= 0.0f) ++backwards;
        if (nz < worst) worst = nz;
    }
    row("WINDING", backwards == 0,
        "%.0f sub-tris wound backwards; smallest nz = %g",
        (double)backwards, (double)worst);
}

// FLATTILT: the same property on a plane that is NOT axis aligned, where the
// tangent-plane projection cannot be exactly zero in float. Measured as the
// out-of-plane distance rather than asserted exact.
static void test_flat_tilted() {
    // A plane through three points; its true normal on every corner.
    const float A[3] = {-3.0f,  1.0f,  2.0f};
    const float B[3] = { 5.0f,  2.0f, -1.0f};
    const float C[3] = { 1.0f, -4.0f,  3.0f};
    float e1[3], e2[3], n[3];
    for (int k = 0; k < 3; ++k) { e1[k] = B[k] - A[k]; e2[k] = C[k] - A[k]; }
    n[0] = e1[1] * e2[2] - e1[2] * e2[1];
    n[1] = e1[2] * e2[0] - e1[0] * e2[2];
    n[2] = e1[0] * e2[1] - e1[1] * e2[0];
    const float l = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
    for (int k = 0; k < 3; ++k) n[k] /= l;

    const SmoothVertex a = vtx(A[0], A[1], A[2], n[0], n[1], n[2], 0, 0, 0xFFFFFFFFu);
    const SmoothVertex b = vtx(B[0], B[1], B[2], n[0], n[1], n[2], 1, 0, 0xFFFFFFFFu);
    const SmoothVertex c = vtx(C[0], C[1], C[2], n[0], n[1], n[2], 0, 1, 0xFFFFFFFFu);
    Bag bag;
    ntr::smooth_subdivide(a, b, c, 8, collect, &bag);
    float worst = 0.0f;
    for (size_t i = 0; i < bag.t.size(); ++i) {
        const SmoothVertex *v[3] = {&bag.t[i].a, &bag.t[i].b, &bag.t[i].c};
        for (int k = 0; k < 3; ++k) {
            const float d = fabsf((v[k]->x - A[0]) * n[0] + (v[k]->y - A[1]) * n[1]
                                + (v[k]->z - A[2]) * n[2]);
            if (d > worst) worst = d;
        }
    }
    // The triangle spans ~9 units, so 1e-5 is about one part in a million.
    row("FLATTILT", worst < 1e-5f,
        "max out-of-plane over a ~9-unit tilted plane = %g (limit 1e-5)",
        (double)worst);
}

// CORNERS: the three original vertices survive BIT FOR BIT -- position, texel
// coordinate and colour. This is the rust curved_input_bulges_but_keeps_corners
// half that matters most: the scheme interpolates, so nothing drifts.
static void test_corners() {
    const SmoothVertex a = vtx(0.0f, 0.0f, 0.0f,  -0.577f, -0.577f, 0.577f,
                               0.25f, 0.75f, 0xFF102030u);
    const SmoothVertex b = vtx(1.0f, 0.0f, 0.0f,   0.577f, -0.577f, 0.577f,
                               0.50f, 0.25f, 0xFF405060u);
    const SmoothVertex c = vtx(1.0f, 1.0f, 0.2f,   0.577f,  0.577f, 0.577f,
                               0.75f, 0.10f, 0xFF708090u);
    Bag bag;
    ntr::smooth_subdivide(a, b, c, 8, collect, &bag);

    const SmoothVertex *want[3] = {&a, &b, &c};
    int found = 0;
    for (int w = 0; w < 3; ++w) {
        bool hit = false;
        for (size_t i = 0; i < bag.t.size() && !hit; ++i) {
            const SmoothVertex *v[3] = {&bag.t[i].a, &bag.t[i].b, &bag.t[i].c};
            for (int k = 0; k < 3; ++k) {
                if (v[k]->x == want[w]->x && v[k]->y == want[w]->y &&
                    v[k]->z == want[w]->z && v[k]->u == want[w]->u &&
                    v[k]->v == want[w]->v && v[k]->color == want[w]->color) {
                    hit = true;
                    break;
                }
            }
        }
        if (hit) ++found;
    }
    row("CORNERS", found == 3,
        "%.0f of 3 original corners reproduced exactly (pos+uv+colour)",
        (double)found);
}

// BULGE: with disagreeing normals the interior really does leave the plane --
// otherwise every other test above would pass on a kernel that does nothing.
static void test_bulge() {
    const SmoothVertex a = vtx(0, 0, 0,  -0.5774f, -0.5774f, 0.5774f, 0, 0, 0xFFFFFFFFu);
    const SmoothVertex b = vtx(1, 0, 0,   0.5774f, -0.5774f, 0.5774f, 1, 0, 0xFFFFFFFFu);
    const SmoothVertex c = vtx(0, 1, 0,  -0.5774f,  0.5774f, 0.5774f, 0, 1, 0xFFFFFFFFu);
    Bag bag;
    ntr::smooth_subdivide(a, b, c, 8, collect, &bag);
    float lift = 0.0f;
    for (size_t i = 0; i < bag.t.size(); ++i) {
        const SmoothVertex *v[3] = {&bag.t[i].a, &bag.t[i].b, &bag.t[i].c};
        for (int k = 0; k < 3; ++k)
            if (fabsf(v[k]->z) > lift) lift = fabsf(v[k]->z);
    }
    // A unit triangle with 54-degree corner normals lifts a few hundredths.
    row("BULGE", lift > 1e-3f && lift < 0.5f,
        "max lift off the plane on a unit triangle = %g (want 1e-3..0.5)",
        (double)lift);
}

// WATERTIGHT: the replacement for the rust weld test. Two triangles share edge
// A-B. Subdivide both and check that every emitted vertex one of them puts on
// that edge has an exact partner from the other. No shared state, no weld
// pass, no dedup map: it holds because an edge's control points are a function
// of that edge alone.
static void test_watertight() {
    const SmoothVertex A = vtx(0, 0, 0,  -0.3f, -0.2f, 0.93f, 0.0f, 0.0f, 0xFF112233u);
    const SmoothVertex B = vtx(2, 0.4f, 0.3f, 0.4f, -0.1f, 0.91f, 1.0f, 0.0f, 0xFF445566u);
    const SmoothVertex C = vtx(1, 1.7f, -0.2f, 0.1f, 0.6f, 0.79f, 0.5f, 1.0f, 0xFF778899u);
    const SmoothVertex D = vtx(1, -1.6f, 0.5f, 0.0f, -0.7f, 0.71f, 0.5f, -1.0f, 0xFFAABBCCu);

    const int tf = 8;
    Bag left, right;
    ntr::smooth_subdivide(A, B, C, tf, collect, &left);
    // The neighbour names the SAME edge the other way round: B first, then A.
    ntr::smooth_subdivide(B, A, D, tf, collect, &right);

    // Collect every emitted vertex that sits on the A-B edge, by parameter.
    float worst = 0.0f;
    int checked = 0;
    for (size_t i = 0; i < left.t.size(); ++i) {
        const SmoothVertex *v[3] = {&left.t[i].a, &left.t[i].b, &left.t[i].c};
        for (int k = 0; k < 3; ++k) {
            // Is this vertex on the shared edge? Only edge points can be, and
            // the cheapest honest test is "some right-hand vertex is within a
            // hair of it"; a vertex strictly inside the left patch has no
            // partner and is skipped by the 1e-3 gate below.
            float best = 1e30f;
            for (size_t j = 0; j < right.t.size(); ++j) {
                const SmoothVertex *w[3] = {&right.t[j].a, &right.t[j].b, &right.t[j].c};
                for (int m = 0; m < 3; ++m) {
                    const float d = dist(*v[k], *w[m]);
                    if (d < best) best = d;
                }
            }
            if (best < 1e-3f) {
                ++checked;
                if (best > worst) worst = best;
            }
        }
    }
    // tf+1 = 9 distinct points on the shared edge; each is named by two or
    // three sub-triangles on the left, so the visit count is higher than 9.
    row("WATERTIGHT", checked >= tf + 1 && worst < 1e-6f,
        "%.0f shared-edge vertex visits, worst gap %g (limit 1e-6)",
        (double)checked, (double)worst);
}

// EDGEORDER: the geometric stand-in for the rust pick_bone tie-break. Evaluate
// the shared edge analytically from both sides and compare point by point.
// Parameter t from A in one patch is parameter 1-t from A in the other.
static void test_edge_order() {
    const SmoothVertex A = vtx(0, 0, 0,  -0.3f, -0.2f, 0.93f, 0, 0, 0xFF112233u);
    const SmoothVertex B = vtx(2, 0.4f, 0.3f, 0.4f, -0.1f, 0.91f, 1, 0, 0xFF445566u);
    const SmoothVertex C = vtx(1, 1.7f, -0.2f, 0.1f, 0.6f, 0.79f, 0, 1, 0xFF778899u);
    const SmoothVertex D = vtx(1, -1.6f, 0.5f, 0.0f, -0.7f, 0.71f, 0, 1, 0xFFAABBCCu);

    SmoothPatch p1, p2;
    ntr::smooth_build_patch(A, B, C, p1);   // edge A->B is (a, b, 0)
    ntr::smooth_build_patch(B, A, D, p2);   // edge B->A is (a, b, 0) reversed

    const int tf = 8;
    float worst = 0.0f;
    for (int k = 0; k <= tf; ++k) {
        const float t = (float)k / (float)tf;
        float x1[3], x2[3];
        ntr::smooth_eval_patch(p1, 1.0f - t, t, 0.0f, x1);
        ntr::smooth_eval_patch(p2, t, 1.0f - t, 0.0f, x2);
        const float d = sqrtf((x1[0] - x2[0]) * (x1[0] - x2[0])
                            + (x1[1] - x2[1]) * (x1[1] - x2[1])
                            + (x1[2] - x2[2]) * (x1[2] - x2[2]));
        if (d > worst) worst = d;
    }
    row("EDGEORDER", worst < 1e-6f,
        "worst disagreement over %.0f samples of the shared edge = %g",
        (double)(tf + 1), (double)worst);
}

// POLICY: which triangles smooth_tess_factor leaves alone, and why.
static void test_policy() {
    SmoothPolicy pol;
    pol.level = 2;          // tf 4
    pol.flat_cos = 0.9999f;
    pol.max_radius = 0.0f;
    pol.max_edge = 0.0f;

    SmoothVertex q[4];
    flat_quad(q);

    // 1. Level 0 is off, whatever the geometry.
    SmoothPolicy off = pol;
    off.level = 0;
    const bool r_off = ntr::smooth_tess_factor(q[0], q[1], q[2], off) == 1;

    // 2. A flat face (agreeing normals) is left exactly alone.
    const bool r_flat = ntr::smooth_tess_factor(q[0], q[1], q[2], pol) == 1;

    // 3. No authored normal (an unlit polygon) is left alone.
    SmoothVertex n0[3] = {q[0], q[1], q[2]};
    n0[1].nx = n0[1].ny = n0[1].nz = 0.0f;
    const bool r_nonorm = ntr::smooth_tess_factor(n0[0], n0[1], n0[2], pol) == 1;

    // 4. A small, tightly curved triangle -- a character's face -- smooths.
    const SmoothVertex a = vtx(0, 0, 0,  -0.5774f, -0.5774f, 0.5774f, 0, 0, 0xFFFFFFFFu);
    const SmoothVertex b = vtx(1, 0, 0,   0.5774f, -0.5774f, 0.5774f, 1, 0, 0xFFFFFFFFu);
    const SmoothVertex c = vtx(0, 1, 0,  -0.5774f,  0.5774f, 0.5774f, 0, 1, 0xFFFFFFFFu);
    const bool r_curved = ntr::smooth_tess_factor(a, b, c, pol) == 4;

    // 5. The same normals spread over a 400-unit triangle -- stage ground --
    //    is refused by the curvature cap, because its implied radius is huge.
    const SmoothVertex ba = vtx(0, 0, 0,     -0.5774f, -0.5774f, 0.5774f, 0, 0, 0xFFFFFFFFu);
    const SmoothVertex bb = vtx(400, 0, 0,    0.5774f, -0.5774f, 0.5774f, 1, 0, 0xFFFFFFFFu);
    const SmoothVertex bc = vtx(0, 400, 0,   -0.5774f,  0.5774f, 0.5774f, 0, 1, 0xFFFFFFFFu);
    SmoothPolicy capped = pol;
    capped.max_radius = 120.0f;
    const bool r_big_refused = ntr::smooth_tess_factor(ba, bb, bc, capped) == 1;
    // and the small one still passes under the same cap.
    const bool r_small_kept = ntr::smooth_tess_factor(a, b, c, capped) == 4;

    // 6. The blunt edge-length cap refuses the same big triangle.
    SmoothPolicy edged = pol;
    edged.max_edge = 60.0f;
    const bool r_edge = ntr::smooth_tess_factor(ba, bb, bc, edged) == 1
                     && ntr::smooth_tess_factor(a, b, c, edged) == 4;

    const int good = (int)r_off + r_flat + r_nonorm + r_curved
                   + r_big_refused + r_small_kept + r_edge;
    row("POLICY", good == 7,
        "%.0f of 7 rules held (off, flat, no-normal, curved, radius cap, "
        "small kept, edge cap)", (double)good);
}

// DEGENERATE: a zero-area triangle and zero normals must not produce NaN or
// infinity. The geometry stage feeds this kernel whatever the game submitted.
static void test_degenerate() {
    const SmoothVertex z = vtx(0, 0, 0, 0, 0, 0, 0, 0, 0xFF000000u);
    Bag bag;
    ntr::smooth_subdivide(z, z, z, 8, collect, &bag);
    // Also a real triangle with zero-length normals.
    const SmoothVertex a = vtx(0, 0, 0, 0, 0, 0, 0, 0, 0xFF000000u);
    const SmoothVertex b = vtx(1, 0, 0, 0, 0, 0, 1, 0, 0xFF000000u);
    const SmoothVertex c = vtx(0, 1, 0, 0, 0, 0, 0, 1, 0xFF000000u);
    ntr::smooth_subdivide(a, b, c, 8, collect, &bag);
    int bad = 0;
    float worst_z = 0.0f;
    for (size_t i = 0; i < bag.t.size(); ++i) {
        const SmoothVertex *v[3] = {&bag.t[i].a, &bag.t[i].b, &bag.t[i].c};
        for (int k = 0; k < 3; ++k) {
            const float f[5] = {v[k]->x, v[k]->y, v[k]->z, v[k]->u, v[k]->v};
            for (int m = 0; m < 5; ++m)
                if (!(f[m] == f[m]) || fabsf(f[m]) > 1e30f) ++bad;
            if (fabsf(v[k]->z) > worst_z) worst_z = fabsf(v[k]->z);
        }
    }
    row("DEGENERATE", bad == 0,
        "%.0f non-finite outputs over %.0f sub-tris (zero normals stay flat)",
        (double)bad, (double)bag.t.size());
    row("ZERONORM", worst_z == 0.0f,
        "zero-normal triangle max |z| = %g (want exactly 0)", (double)worst_z);
}

// UVCOLOR: texel coordinates and colour are linear in barycentrics, so a
// texture never swims and an edge midpoint is the plain average.
static void test_uvcolor() {
    const SmoothVertex a = vtx(0, 0, 0,  0, 0, 1,  0.0f,  0.0f, 0xFF000000u);
    const SmoothVertex b = vtx(4, 0, 0,  0, 0, 1,  8.0f,  0.0f, 0xFF800000u);
    const SmoothVertex c = vtx(0, 4, 0,  0, 0, 1,  0.0f, 16.0f, 0xFF008000u);
    Bag bag;
    ntr::smooth_subdivide(a, b, c, 2, collect, &bag);
    // The midpoint of edge a-b is at (2,0,0); its u must be exactly 4.
    float worst = 1e30f;
    uint32_t col = 0;
    for (size_t i = 0; i < bag.t.size(); ++i) {
        const SmoothVertex *v[3] = {&bag.t[i].a, &bag.t[i].b, &bag.t[i].c};
        for (int k = 0; k < 3; ++k) {
            if (fabsf(v[k]->x - 2.0f) < 1e-6f && fabsf(v[k]->y) < 1e-6f) {
                const float e = fabsf(v[k]->u - 4.0f) + fabsf(v[k]->v);
                if (e < worst) { worst = e; col = v[k]->color; }
            }
        }
    }
    // 0x00 and 0x80 average to 0x40.
    const bool ok = worst < 1e-6f && col == 0xFF400000u;
    char detail[160];
    snprintf(detail, sizeof detail,
             "edge-midpoint uv error %g, colour %08X (want 0, FF400000)",
             (double)worst, (unsigned)col);
    row("UVCOLOR", ok, detail);
}

// --------------------------------------------------------- the shape store
//
// These four rows are the whole of the "computed once and reused" claim. The
// store lives in the kernel, so they need no engine, no matrix stack and no
// ROM: a similarity matrix written out longhand here is the same kind of
// matrix the geometry stage checks for before it ever forms a key.

// A rotation and a uniform scale and a translation, applied to a point. The
// geometry stage refuses anything else, which is why this is all the test
// needs: see the matrix-generation block in ntr/gx.cpp.
struct Sim {
    float r[3][3];   // a rotation: r[i] is the image of basis vector i
    float s;         // uniform scale
    float t[3];      // translation
};

static Sim sim_make(float ax, float ay, float az, float ang, float s,
                    float tx, float ty, float tz) {
    // Rodrigues, written out so the test depends on nothing but itself.
    const float l = sqrtf(ax * ax + ay * ay + az * az);
    ax /= l; ay /= l; az /= l;
    const float ca = cosf(ang), sa = sinf(ang), ic = 1.0f - ca;
    Sim m;
    m.r[0][0] = ca + ax * ax * ic;
    m.r[0][1] = ax * ay * ic + az * sa;
    m.r[0][2] = ax * az * ic - ay * sa;
    m.r[1][0] = ay * ax * ic - az * sa;
    m.r[1][1] = ca + ay * ay * ic;
    m.r[1][2] = ay * az * ic + ax * sa;
    m.r[2][0] = az * ax * ic + ay * sa;
    m.r[2][1] = az * ay * ic - ax * sa;
    m.r[2][2] = ca + az * az * ic;
    m.s = s;
    m.t[0] = tx; m.t[1] = ty; m.t[2] = tz;
    return m;
}

static void sim_point(const Sim &m, const float p[3], float o[3]) {
    for (int k = 0; k < 3; ++k)
        o[k] = (p[0] * m.r[0][k] + p[1] * m.r[1][k] + p[2] * m.r[2][k]) * m.s
             + m.t[k];
}

static void sim_dir(const Sim &m, const float p[3], float o[3]) {
    for (int k = 0; k < 3; ++k)
        o[k] = p[0] * m.r[0][k] + p[1] * m.r[1][k] + p[2] * m.r[2][k];
}

// REPLAY: the grid built in LOCAL space and then pushed through the matrix is
// the grid built from the TRANSFORMED corners. In exact arithmetic these are
// the same points -- the PN edge term is a dot product a rotation preserves
// and a uniform scale multiplies along with everything else, and the patch
// evaluation is an affine combination of the control points -- so the only
// difference the machine can produce is the order the roundings happen in.
// The row reports that difference as a fraction of the triangle's own size,
// which is the only scale it means anything against.
static void test_store_replay() {
    const SmoothVertex l0 = vtx(0.0f, 0.0f, 0.0f,  -0.3f, -0.4f, 0.87f,
                                0, 0, 0xFF112233u);
    const SmoothVertex l1 = vtx(2.5f, 0.25f, -0.5f, 0.6f, -0.2f, 0.77f,
                                4, 0, 0xFF445566u);
    const SmoothVertex l2 = vtx(0.75f, 2.0f, 0.4f, -0.1f, 0.65f, 0.75f,
                                0, 8, 0xFF778899u);

    const Sim m = sim_make(0.3f, -0.7f, 0.65f, 1.17f, 3.5f,
                           120.0f, -45.0f, 610.0f);

    // The live build: corners through the matrix, normals through its
    // rotation, patch in that space.
    SmoothVertex v[3];
    const SmoothVertex *src[3] = {&l0, &l1, &l2};
    for (int i = 0; i < 3; ++i) {
        const float p[3] = {src[i]->x, src[i]->y, src[i]->z};
        const float n[3] = {src[i]->nx, src[i]->ny, src[i]->nz};
        float op[3], on[3];
        sim_point(m, p, op);
        sim_dir(m, n, on);
        v[i] = vtx(op[0], op[1], op[2], on[0], on[1], on[2],
                   src[i]->u, src[i]->v, src[i]->color);
    }

    float worst = 0.0f, size = 0.0f;
    for (int lvl = 1; lvl <= ntr::SMOOTH_MAX_LEVEL; ++lvl) {
        const int tf = 1 << lvl;
        std::vector<float> a(ntr::smooth_grid_points(tf) * 3);
        std::vector<float> b(ntr::smooth_grid_points(tf) * 3);
        const int n = ntr::smooth_grid_positions(l0, l1, l2, tf, &a[0]);
        ntr::smooth_grid_positions(v[0], v[1], v[2], tf, &b[0]);
        for (int i = 0; i < n; ++i) {
            float o[3];
            sim_point(m, &a[i * 3], o);
            const float dx = o[0] - b[i * 3];
            const float dy = o[1] - b[i * 3 + 1];
            const float dz = o[2] - b[i * 3 + 2];
            const float d = sqrtf(dx * dx + dy * dy + dz * dz);
            if (d > worst) worst = d;
        }
        const float e0 = dist(v[0], v[1]), e1 = dist(v[1], v[2]);
        if (e0 > size) size = e0;
        if (e1 > size) size = e1;
    }
    const float rel = worst / size;
    // A float carries about seven decimal digits, the positions here are
    // hundreds of units from the origin, and the two routes multiply in a
    // different order, so a few parts in ten million is the floor. Anything
    // above a part in a hundred thousand would be an algebra error, not a
    // rounding one.
    char detail[200];
    snprintf(detail, sizeof detail,
             "replayed vs live grid: worst %g units over a %.1f-unit triangle "
             "= %g of its size (limit 1e-5)",
             (double)worst, (double)size, (double)rel);
    row("REPLAY", rel < 1e-5f, detail);
}

// KEYMTX: the key is the raw input and nothing else, so the same triangle
// under two different matrices is ONE entry. The store has no matrix in its
// interface at all, which is the point; this row pins that the second lookup
// hits rather than inserting a twin.
static void test_store_keymtx() {
    ntr::smooth_store_clear();
    ntr::SmoothStoreStats before, after;
    ntr::smooth_store_stats(before);

    ntr::SmoothKey k;
    for (int i = 0; i < 3; ++i) {
        k.p[i][0] = (int16_t)(100 + i * 7);
        k.p[i][1] = (int16_t)(-40 + i * 3);
        k.p[i][2] = (int16_t)(900 - i * 11);
        k.n[i] = 0x12345678u + (uint32_t)i;
    }
    k.level = 2;
    const int tf = 4;
    std::vector<float> grid(ntr::smooth_grid_points(tf) * 3, 0.25f);
    ntr::smooth_store_add(k, tf, &grid[0]);

    // The same raw triangle, seen again on a later frame under a completely
    // different matrix: the caller forms the same key, because the key never
    // saw a matrix.
    const ntr::SmoothEntry *e1 = ntr::smooth_store_find(k);
    const ntr::SmoothEntry *e2 = ntr::smooth_store_find(k);
    ntr::smooth_store_stats(after);
    const bool ok = e1 && e1 == e2 && after.entries == before.entries + 1
                 && after.hits == before.hits + 2;
    char detail[200];
    snprintf(detail, sizeof detail,
             "one insert, two hits, %llu entr%s (want 1 and 2 and 1)",
             (unsigned long long)after.entries,
             after.entries == 1 ? "y" : "ies");
    row("KEYMTX", ok, detail);
}

// COLLIDE: fill the table hard enough that hash collisions are certain, then
// check every entry still answers with its OWN grid. A lookup that trusted
// the hash rather than comparing the whole key would hand back a neighbour's
// patch here, and the sentinel in each grid is what would catch it.
static void test_store_collide() {
    ntr::smooth_store_clear();
    const int N = 4000;
    const int tf = 2;
    const int npts = ntr::smooth_grid_points(tf);
    std::vector<float> grid((size_t)npts * 3, 0.0f);

    for (int i = 0; i < N; ++i) {
        ntr::SmoothKey k;
        for (int c = 0; c < 3; ++c) {
            k.p[c][0] = (int16_t)(i * 3 + c);
            k.p[c][1] = (int16_t)(-i + c);
            k.p[c][2] = (int16_t)((i * 7) & 0x7FFF);
            k.n[c] = (uint32_t)(i * 131u + (uint32_t)c);
        }
        k.level = 1;
        grid[0] = (float)i;            // the sentinel: which entry this is
        ntr::smooth_store_add(k, tf, &grid[0]);
    }

    int wrong = 0, missing = 0;
    for (int i = 0; i < N; ++i) {
        ntr::SmoothKey k;
        for (int c = 0; c < 3; ++c) {
            k.p[c][0] = (int16_t)(i * 3 + c);
            k.p[c][1] = (int16_t)(-i + c);
            k.p[c][2] = (int16_t)((i * 7) & 0x7FFF);
            k.n[c] = (uint32_t)(i * 131u + (uint32_t)c);
        }
        k.level = 1;
        const ntr::SmoothEntry *e = ntr::smooth_store_find(k);
        if (!e) { ++missing; continue; }
        const float *gp = ntr::smooth_store_grid(e);
        if (!gp || gp[0] != (float)i) ++wrong;
    }
    // And a key that was never inserted must miss rather than land on
    // whatever shares its slot.
    ntr::SmoothKey absent;
    for (int c = 0; c < 3; ++c) {
        absent.p[c][0] = 30000; absent.p[c][1] = -30000; absent.p[c][2] = 111;
        absent.n[c] = 0xDEADBEEFu;
    }
    absent.level = 1;
    const bool stranger_missed = ntr::smooth_store_find(absent) == 0;

    char detail[200];
    snprintf(detail, sizeof detail,
             "%d entries: %d missing, %d handed back another entry's grid, "
             "unknown key %s (want 0, 0, missed)",
             N, missing, wrong, stranger_missed ? "missed" : "HIT");
    row("COLLIDE", missing == 0 && wrong == 0 && stranger_missed, detail);
}

// CAPEVICT: the pool is bounded, and the bound is enforced by emptying it.
// main() sets SM64DS_SMOOTH_STORE_MB to 1 before anything touches the store,
// so this row runs against a cap small enough to reach in a fraction of a
// second and the rows above stay comfortably inside it.
static void test_store_cap() {
    ntr::smooth_store_clear();
    ntr::SmoothStoreStats before, after;
    ntr::smooth_store_stats(before);

    const int tf = 8;                       // the biggest grid, 45 points
    const int npts = ntr::smooth_grid_points(tf);
    std::vector<float> grid((size_t)npts * 3, 1.0f);

    uint64_t peak = 0;
    for (int i = 0; i < 20000; ++i) {
        ntr::SmoothKey k;
        for (int c = 0; c < 3; ++c) {
            k.p[c][0] = (int16_t)(i & 0x7FFF);
            k.p[c][1] = (int16_t)(c - i);
            k.p[c][2] = (int16_t)(i >> 2);
            k.n[c] = (uint32_t)(i * 7919u + (uint32_t)c);
        }
        k.level = 3;
        ntr::smooth_store_add(k, tf, &grid[0]);
        ntr::smooth_store_stats(after);
        if (after.bytes > peak) peak = after.bytes;
        if (after.clears > before.clears + 1) break;
    }
    ntr::smooth_store_stats(after);
    const bool ok = after.clears > before.clears
                 && peak <= after.cap_bytes
                 && after.cap_bytes > 0;
    char detail[220];
    snprintf(detail, sizeof detail,
             "cap %llu bytes, peak %llu, %llu eviction(s) (want peak <= cap "
             "and at least one)",
             (unsigned long long)after.cap_bytes, (unsigned long long)peak,
             (unsigned long long)(after.clears - before.clears));
    row("CAPEVICT", ok, detail);
    ntr::smooth_store_clear();
}

// SHAPESPLIT: smooth_tess_factor and the split pair must be one verdict. The
// split is what lets half the policy be remembered, so it is worth pinning
// that it did not become a second opinion.
static void test_shape_split() {
    SmoothPolicy pol;
    pol.level = 2; pol.flat_cos = 0.99985f; pol.max_radius = 128.0f;
    pol.max_edge = 0.0f;
    int bad = 0, checked = 0;
    for (int i = 0; i < 64; ++i) {
        const float f = (float)i * 0.031f;
        const SmoothVertex a = vtx(0, 0, 0,  0, 0, 1,  0, 0, 0xFF000000u);
        const SmoothVertex b = vtx(1 + f, 0, 0,  f * 0.2f, 0,
                                   1 - f * 0.05f, 1, 0, 0xFF000000u);
        const SmoothVertex c = vtx(0, 1 + f, 0,  0, -f * 0.1f, 1,
                                   0, 1, 0xFF000000u);
        int w1 = -1, w2 = -1;
        const int t1 = ntr::smooth_tess_factor(a, b, c, pol, &w1);
        ntr::SmoothShape sh;
        ntr::smooth_shape(a, b, c, pol, sh);
        const int t2 = ntr::smooth_shape_factor(sh, pol, 1.0f, &w2);
        ++checked;
        if (t1 != t2 || w1 != w2) ++bad;
    }
    row("SHAPESPLIT", bad == 0,
        "%.0f of %.0f triangles: the split policy and the whole one disagree "
        "(want 0)", (double)bad, (double)checked);
}

int main() {
    /* The store latches its cap the first time anything touches it, so the
       cap the CAPEVICT row needs has to be in the environment before the
       first store call. One megabyte is small enough to fill in a fraction
       of a second and large enough for every other row here. */
    _putenv("SM64DS_SMOOTH_STORE_MB=1");
    printf("smooth_selftest (ntr/smooth) -- PN triangles, "
           "MAX_LEVEL=%d, MAX_TF=%d\n", (int)ntr::SMOOTH_MAX_LEVEL,
           (int)ntr::SMOOTH_MAX_TF);
    printf("test         verdict measurement\n");
    printf("------------ ------- ------------------------------------------------\n");
    test_count();
    test_flat();
    test_winding();
    test_flat_tilted();
    test_corners();
    test_bulge();
    test_watertight();
    test_edge_order();
    test_policy();
    test_degenerate();
    test_uvcolor();
    test_shape_split();
    test_store_replay();
    test_store_keymtx();
    test_store_collide();
    test_store_cap();
    printf("------------ ------- ------------------------------------------------\n");
    printf("%s: %d failing row(s)\n", g_fail ? "SELFTEST RED" : "SELFTEST GREEN",
           g_fail);
    return g_fail ? 1 : 0;
}
