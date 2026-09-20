// Curved PN-triangle subdivision -- the pure geometry kernel.
//
// This is the C++ port of the parked tangOS-SM64DS smoother
// (app/src/subdiv.rs, docs/SMOOTHING.md): curved point-normal triangles,
// Vlachos et al. 2001. Each input triangle becomes a tessellated cubic Bezier
// patch whose shape is driven by the three corner normals.
//
// WHAT IT GUARANTEES, and what the selftest pins:
//   * The three corners evaluate to the ORIGINAL vertices EXACTLY. The scheme
//     interpolates: silhouettes grow, nothing shrinks or drifts.
//   * A triangle whose three normals agree is EXACTLY flat afterwards (the
//     tangent-plane projections are no-ops and the centre lift collapses to
//     the centroid). Curvature appears only where the normals disagree.
//   * An edge's four control points depend ONLY on that edge's two endpoints
//     and their normals, so two triangles that share an edge tessellate it to
//     the same curve whichever way round they name it -- watertight, with no
//     weld pass and no cross-triangle state.
//   * Sub-triangle winding matches the parent's.
//   * No allocation, no global state, deterministic: the whole grid lives in
//     one fixed stack array, so this is callable from inside the geometry
//     stage with nothing buffered.
//
// WHERE THE NORMALS COME FROM IN THE PORT. Unlike the Rust original, which had
// to weld normals out of geometry because a GLB part carries none, the DS
// display list carries the model's OWN authored per-vertex normal in the
// NORMAL command (0x21) -- that is what the hardware lights with. gx.cpp
// already transforms it by the VECTOR matrix, which is the POSITION matrix
// without translation, so a stream normal and a view-space position live in
// the same space and need no reconciliation. Welding is therefore a follow-up
// (see smooth.cpp's NORMAL SOURCES note), not a prerequisite.
//
// THE SPACE. Everything here is VIEW SPACE: after the DS position matrix,
// before the projection, before the perspective divide and before near
// clipping. Subdividing after the divide would curve in a non-linear space
// and bend straight edges by the near plane's distance.

#ifndef NTR_SMOOTH_H
#define NTR_SMOOTH_H

#include <stdint.h>

namespace ntr {

// tf = 1 << level. Level 1 = 4x triangles, 2 = 16x, 3 = 64x.
enum {
    SMOOTH_MAX_LEVEL = 3,
    SMOOTH_MAX_TF    = 1 << SMOOTH_MAX_LEVEL,                       // 8
    SMOOTH_MAX_GRID  = (SMOOTH_MAX_TF + 1) * (SMOOTH_MAX_TF + 1),   // 81
    // (tf+1)(tf+2)/2 grid points, tf*tf sub-triangles at the top level.
    SMOOTH_MAX_TRIS  = SMOOTH_MAX_TF * SMOOTH_MAX_TF                // 64
};

// One corner handed to the smoother.
struct SmoothVertex {
    float x, y, z;        // view-space position
    float nx, ny, nz;     // view-space normal; need not be unit length
    float u, v;           // texel coordinates, as GxVertex carries them
    uint32_t color;       // 0xAARRGGBB, lighting already baked per vertex
};

// The ten control points of the cubic patch, ordered exactly as the Rust
// original: b300, b030, b003, b210, b120, b021, b012, b201, b102, b111.
struct SmoothPatch {
    float b[10][3];
};

// How hard to subdivide, and what to leave alone. Every field is data, not a
// hand list of model names: see THE SUBDIVISION RULE in smooth.cpp.
struct SmoothPolicy {
    int level;         // 0 = off; clamped to SMOOTH_MAX_LEVEL
    // Corner normals at least this aligned (dot product of the unit normals)
    // count as one flat face: the patch would be flat anyway, so the triangle
    // passes through untouched and its output is bit-identical to its input.
    float flat_cos;
    // Curvature cap, in the same world units as the positions. A triangle
    // whose tightest implied radius of curvature (edge length / normal turn)
    // is LARGER than this is a big gently-curved surface -- stage ground,
    // a water plane -- where the PN centre lift reads as a pillow rather than
    // as roundness. 0 disables the cap.
    float max_radius;
    // Skip a triangle whose longest edge is at least this long in view-space
    // units, whatever its curvature. 0 disables the cap.
    float max_edge;
};

// Build the patch for corners p1, p2, p3.
void smooth_build_patch(const SmoothVertex &p1, const SmoothVertex &p2,
                        const SmoothVertex &p3, SmoothPatch &out);

// Evaluate at barycentric (a, b, c) for (p1, p2, p3); a + b + c == 1.
void smooth_eval_patch(const SmoothPatch &patch, float a, float b, float c,
                       float out[3]);

// Why a triangle came back with a factor of 1, for the measurement table.
enum {
    SMOOTH_WHY_OK = 0,
    SMOOTH_WHY_LEVEL,        // the feature is off
    SMOOTH_WHY_NO_NORMAL,    // a corner carried no NORMAL: unlit polygon
    SMOOTH_WHY_FLAT,         // the three normals agree: already exactly flat
    SMOOTH_WHY_EDGE,         // longer than max_edge
    SMOOTH_WHY_RADIUS        // too gently curved: the pillow guard
};

// The tessellation factor this triangle earns under `pol`: 1 means "emit it
// unchanged", anything higher means tf*tf sub-triangles. Pure predicate, no
// side effects, safe on degenerate input. `why`, when given, is filled with
// one of the SMOOTH_WHY_* reasons.
int smooth_tess_factor(const SmoothVertex &p1, const SmoothVertex &p2,
                       const SmoothVertex &p3, const SmoothPolicy &pol,
                       int *why = 0);

// Emit the tessellation. `sink` is called once per sub-triangle, in a
// deterministic order, winding preserved. Returns the number of sub-triangles
// emitted; tf <= 1 emits the input triangle once, unchanged.
typedef void (*SmoothSink)(void *ctx, const SmoothVertex &a,
                           const SmoothVertex &b, const SmoothVertex &c);
int smooth_subdivide(const SmoothVertex &p1, const SmoothVertex &p2,
                     const SmoothVertex &p3, int tf,
                     SmoothSink sink, void *ctx);

// How many sub-triangles a given tf produces. Pure arithmetic, for the
// counters that must size storage before emitting.
inline int smooth_tri_count(int tf) { return tf <= 1 ? 1 : tf * tf; }

// ---------------------------------------------------------------------------
// THE GRID, EXPOSED. smooth_subdivide above hands the caller one SUB-TRIANGLE
// at a time, and an interior grid point belongs to up to six of them, so the
// caller projects the same invented point up to six times. At level 3 a
// triangle has 45 distinct grid points and emits 64 sub-triangles: 192 vertex
// projections to draw 45 distinct vertices.
//
// These entry points hand over the grid itself. The caller evaluates and
// projects each point ONCE and then walks an index list to emit the
// sub-triangles. Same points, same order, same arithmetic -- the vertices that
// reach the polygon list are bit for bit the ones smooth_subdivide produced
// (smooth_subdivide is implemented on top of these, so that is not a claim,
// it is the same code) -- each one just computed once instead of up to six
// times.
// ---------------------------------------------------------------------------

// (tf+1)(tf+2)/2 grid points; 1 for tf <= 1.
int smooth_grid_points(int tf);
// tf*tf sub-triangles; 1 for tf <= 1.
int smooth_grid_tris(int tf);
// 3 * smooth_grid_tris(tf) grid indices, in exactly smooth_subdivide's
// emission order, into a table with one row per (ia, ib) in the fill order
// `for ia 0..tf { for ib 0..tf-ia }`. The pointer is to a static table built
// on first use; there are only three tessellation factors (2, 4, 8).
const uint16_t *smooth_grid_tri_index(int tf);
// The grid POSITIONS for corners p1..p3, in whatever space those corners were
// given in. `out` holds smooth_grid_points(tf) * 3 floats, filled in the same
// (ia, ib) order the index table is written against. Returns the point count.
int smooth_grid_positions(const SmoothVertex &p1, const SmoothVertex &p2,
                          const SmoothVertex &p3, int tf, float *out);
// Everything about a grid point that is NOT its position: texel coordinates
// and colour linear in the barycentric weights, and the normal carried for a
// future relighting pass. Writes u, v, color, nx, ny, nz and nothing else.
// smooth_subdivide calls this too, so the two paths cannot drift apart.
void smooth_grid_attrs(const SmoothVertex &p1, const SmoothVertex &p2,
                       const SmoothVertex &p3, float a, float b, float c,
                       SmoothVertex &out);

// ---------------------------------------------------------------------------
// THE POLICY, SPLIT. smooth_tess_factor answers two different questions at
// once: is this triangle CURVED (a property of its own shape, the same in any
// space a similarity transform can reach) and is it TOO BIG (a property of how
// large it is once the position matrix has scaled it). Splitting them is what
// lets the first half be measured once per triangle shape and remembered,
// while the second half stays a per-frame compare against the caps.
//
// smooth_tess_factor is smooth_shape() followed by smooth_shape_factor() at
// scale 1, so the shipped verdict is one piece of code with one meaning.
// ---------------------------------------------------------------------------
struct SmoothShape {
    int no_normal;    // a corner carried a zero normal: unlit, nothing to curve
    int curved;       // some corner pair disagrees by more than flat_cos
    float longest;    // longest edge, in the units the corners were given in
    float tightest;   // tightest implied curvature radius, same units
};
// Measure the shape. Cheap for the common refusals: a corner with no normal
// costs three compares, and a flat triangle costs three dot products, both
// without touching a square root.
void smooth_shape(const SmoothVertex &p1, const SmoothVertex &p2,
                  const SmoothVertex &p3, const SmoothPolicy &pol,
                  SmoothShape &out);
// The verdict for a shape measured in a space `scale` times smaller than the
// space the caps are written in. scale 1 is "the shape was measured where the
// caps live", and multiplying by an exact 1.0f leaves the comparison the one
// smooth_tess_factor has always made.
int smooth_shape_factor(const SmoothShape &sh, const SmoothPolicy &pol,
                        float scale, int *why);

// ---------------------------------------------------------------------------
// THE ENGINE-FACING HALF. Everything above is pure geometry and is what the
// standalone selftest exercises; everything below is the switch the port
// turns on and the counters it is measured by. None of it allocates, none of
// it runs, and none of it costs more than one compare when the level is 0.
// ---------------------------------------------------------------------------

// THE CONTRACT, carried over from the seam commit this file grew out of.
// smooth_configure is called ONCE, at boot, beside ntr::configure_aspect,
// from tests/walk_window.cpp's main. `level` is the SmoothModels settings
// key: 0 is off, is the default, and is the ROM's own geometry; 1..3 are
// subdivision levels; anything outside 0..3 has already been clamped by
// host_settings (SM64DS_SMOOTH_MODELS overrides the file) before it arrives,
// and is clamped again here rather than trusted, because a level is about to
// size work per polygon.
void smooth_configure(int level);

// 0 when the feature is off. One load and one compare: this is the whole
// cost of the feature in a default build.
int smooth_level();

// The caps the level is applied under. Tunable from the environment for the
// measurement sweep; the shipped defaults are in smooth.cpp.
const SmoothPolicy &smooth_policy();

// What the last N frames actually did. tris_in counts triangles the GAME
// submitted, tris_out counts what reached the polygon list, so tris_in is the
// number every game-visible counter has to keep reporting.
struct SmoothCounters {
    uint64_t frames;
    uint64_t tris_in;            // triangles the game assembled
    uint64_t tris_out;           // triangles pushed to the polygon list
    uint64_t tris_subdivided;    // of tris_in, how many were curved
    uint64_t skip_flat;          // three normals agreed: exactly flat already
    uint64_t skip_no_normal;     // a corner carried no NORMAL (unlit polygon)
    uint64_t skip_radius;        // too gently curved: the pillow guard
    uint64_t skip_edge;          // longer than the edge cap
    uint64_t skip_w;             // non-affine position matrix; view w != 1
    uint64_t skip_mode3;         // shadow-volume polygon: exact geometry only
    uint64_t skip_ortho;         // no near plane: 2D/HUD geometry
};
void smooth_counters(SmoothCounters &out);
void smooth_counters_reset();

// The geometry stage bumps them through this, so gx.cpp holds no copy of the
// counter layout and adding a row costs one file.
enum {
    SMOOTH_COUNT_IN = 0,
    SMOOTH_COUNT_OUT,
    SMOOTH_COUNT_SUBDIVIDED,
    SMOOTH_COUNT_FLAT,
    SMOOTH_COUNT_NO_NORMAL,
    SMOOTH_COUNT_RADIUS,
    SMOOTH_COUNT_EDGE,
    SMOOTH_COUNT_W,
    SMOOTH_COUNT_MODE3,
    SMOOTH_COUNT_ORTHO
};
void smooth_count(int which, uint64_t n);

// Called once per frame from gx_reset. Advances the frame counter and, when
// the crack census is on, closes the frame's edge book and prints it.
void smooth_frame_mark();

// ---------------------------------------------------------------------------
// THE SCOPED PROFILER (SM64DS_SMOOTH_PROF=1). Off by default and then costs
// one compare on a cached file-scope int inside a path that only runs when
// SmoothModels is already on, so a shipped run never touches it.
//
// It exists to answer one question with numbers instead of intuition: of the
// time a smoothed frame spends above an unsmoothed one, how much is the
// PATCH MATHS (which a cache can remove), how much is PROJECTING the invented
// vertices (which a cache cannot), and how much is the raster handling more
// triangles (which a cache cannot either). The three buckets below are the
// first two; the third is the frame time left over once they are subtracted,
// which is why there is no bucket for it.
//
// The clock is std::chrono::steady_clock, which on this toolchain IS
// QueryPerformanceCounter -- MSVC's steady_clock calls it directly and
// divides by the frequency it queried once. Reading it costs roughly 20 to 30
// nanoseconds, so the profiler perturbs what it measures; the SINK bucket is
// read once per SUB-triangle and is the one to distrust first. Every reading
// taken with it is reported as instrumented, never as the shipped cost.
enum {
    SMOOTH_PROF_POLICY = 0,   // smooth_tess_factor: accept/refuse + the factor
    SMOOTH_PROF_SUBDIV,       // smooth_subdivide as a whole, sink calls included
    SMOOTH_PROF_SINK,         // one sub-triangle: project, near-clip, push
    SMOOTH_PROF_BUCKETS
};
int smooth_prof_on();
// Nanoseconds on a monotonic clock. Only meaningful as a difference.
long long smooth_prof_ticks();
void smooth_prof_add(int bucket, long long dt_ns, unsigned n);

// ---------------------------------------------------------------------------
// THE SHAPE STORE: computed once, replayed every frame.
//
// WHAT IS STORED AND WHY IT IS THE RIGHT THING. A patch's shape in the space
// its corners were AUTHORED in does not change from frame to frame: the model
// is the same model. What changes is the matrix the game pushes it through.
// So the tessellation is built ONCE, in LOCAL space, out of the display
// list's own raw VTX coordinates and raw NORMAL payloads, and every later
// frame pushes those stored local points through the current position matrix
// exactly as the game's own vertices are pushed through it.
//
// THE KEY IS THE TRIANGLE'S RAW INPUT, AND NOTHING ELSE: three raw positions,
// three raw normal payloads, and the subdivision level. No address, no model
// id, no frame number. Two instances of the same goomba share one entry, a
// model loaded at a different address next boot still hits, and a triangle
// whose vertices happen to coincide with another's cannot collide with it
// because the lookup compares the WHOLE key, not its hash.
//
// WHAT IS NOT STORED: texel coordinates and colours. Those are live state --
// a texture matrix moves, lighting changes -- so the replay interpolates the
// CURRENT corner values at the same barycentric weights the grid was built
// at, which is what the live path does too.
//
// THE CALLER OWES THE INVARIANT. The stored grid is only replayable under a
// matrix that is a SIMILARITY (rotation, uniform scale, translation), because
// only then does transforming the patch equal building the patch from the
// transformed corners; and all three corners must have come from the SAME
// matrix, because two bones have no shared local space. The geometry stage
// checks both before it ever forms a key, and counts what it refuses.
// ---------------------------------------------------------------------------

struct SmoothKey {
    int16_t p[3][3];     // the three VTX commands' own coordinates
    uint32_t n[3];       // the three NORMAL commands' 30-bit payloads
    uint32_t level;      // the subdivision level the grid was built at
};

struct SmoothEntry {
    SmoothKey key;
    SmoothShape shape;   // measured in local units; the caps are applied live
    int32_t tf;          // the factor the grid was built at, 1 if none was
    uint32_t off;        // first float of this entry's grid in the pool
};

// Null on a miss. The returned pointer is valid until the next add.
const SmoothEntry *smooth_store_find(const SmoothKey &k);
// Insert. `grid` is smooth_grid_points(tf)*3 floats, or null when tf <= 1
// (a refused shape is worth remembering too: it is the policy work saved).
// Null only when the store refused the insert outright.
const SmoothEntry *smooth_store_add(const SmoothKey &k, int tf,
                                    const SmoothShape &sh, const float *grid);
const float *smooth_store_grid(const SmoothEntry *e);
// Drop everything. Called when the level changes, since a level is the
// natural end of a set of models, and by the cap when the pool is full.
void smooth_store_clear();

struct SmoothStoreStats {
    uint64_t hits, misses, inserts, clears;
    uint64_t skip_crossmtx;   // corners from different matrices
    uint64_t skip_nonsim;     // position matrix is not a similarity
    uint64_t skip_zeronrm;    // a raw NORMAL payload was zero
    uint64_t live_calls;      // triangles that took the 0.4.0 path instead
    uint64_t entries;
    uint64_t bytes, cap_bytes;
};
void smooth_store_stats(SmoothStoreStats &out);
// The geometry stage bumps the four skip rows through this, so gx.cpp holds
// no copy of the layout.
enum {
    SMOOTH_STORE_CROSSMTX = 0,
    SMOOTH_STORE_NONSIM,
    SMOOTH_STORE_ZERONRM,
    SMOOTH_STORE_LIVE
};
void smooth_store_count(int which, uint64_t n);

// SM64DS_SMOOTH_LIVE=1 puts 0.4.0's behaviour back: every accepted triangle
// rebuilds its patch from its view-space corners every frame, through
// smooth_subdivide and a per-sub-triangle sink. It exists for the A/B and for
// nothing else; read once, like every other knob here.
int smooth_live_mode();

// SM64DS_SMOOTH_ABDIFF=1: for every triangle the store answers, build the
// patch the OLD way as well, from the transformed corners, and book the two
// against each other. It is how the claim "the stored patch is the same patch,
// to a rounding order" stops being a claim: a verdict that disagrees is
// counted, and the worst positional separation is reported in view units and
// as a fraction of the triangle's own longest edge. Slower than either path
// alone, obviously, and never on in a shipped run.
int smooth_abdiff_on();
void smooth_abdiff_add(int verdict_differs, float worst_dev, float longest);

// THE CRACK CENSUS (constraint 5 of the brief). Off unless
// SM64DS_SMOOTH_CENSUS is set, and it is the only thing here that allocates.
// It books every submitted triangle's view-space corners and normals, pairs
// them up by shared edge at the end of the frame, and reports how many shared
// edges have two sides whose normals disagree -- which is exactly where a
// curved patch can open a crack, because the two sides then build different
// edge control points. Rigid-skinned joints are where that is expected.
int smooth_census_on();
void smooth_census_tri(const SmoothVertex &a, const SmoothVertex &b,
                       const SmoothVertex &c, int tf);

}  // namespace ntr

#endif  // NTR_SMOOTH_H
