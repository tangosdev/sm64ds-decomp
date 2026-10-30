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

#include <stddef.h>
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
// THE GRID TABLE. For each tessellation factor (2, 4, 8) the barycentric
// weights of every grid point (a, b, c) and its ten Bernstein weights, as
// smooth_eval_patch computes them, built once and kept. A patch evaluation is
// then ten multiply-adds per coordinate per point with nothing recomputed,
// the same floats in the same order as smooth_eval_patch.
struct SmoothGridTable {
    int n;
    float a[SMOOTH_MAX_GRID], b[SMOOTH_MAX_GRID], c[SMOOTH_MAX_GRID];
    float w[SMOOTH_MAX_GRID][10];
};
// Null for tf that is not 2, 4 or 8.
const SmoothGridTable *smooth_grid_table(int tf);
// Everything about a grid point that is NOT its position: texel coordinates
// and colour linear in the barycentric weights, and the normal carried for a
// future relighting pass. Writes u, v, color, nx, ny, nz and nothing else.
// smooth_subdivide calls this too, so the two paths cannot drift apart.
void smooth_grid_attrs(const SmoothVertex &p1, const SmoothVertex &p2,
                       const SmoothVertex &p3, float a, float b, float c,
                       SmoothVertex &out);

// THE ATTRIBUTE ARITHMETIC, inline. smooth_grid_attrs (smooth.cpp) is this
// function behind a call, so the engine's per-vertex loop (gx.cpp,
// smooth_emit_grid) can run the very same expressions without a call per grid
// vertex and the two cannot drift apart.
inline uint32_t smooth_bary_color(uint32_t c0, uint32_t c1, uint32_t c2,
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
inline void smooth_grid_attrs_inline(const SmoothVertex &p1, const SmoothVertex &p2,
                                     const SmoothVertex &p3, float a, float b,
                                     float c, SmoothVertex &o) {
    o.u = p1.u * a + p2.u * b + p3.u * c;
    o.v = p1.v * a + p2.v * b + p3.v * c;
    o.color = smooth_bary_color(p1.color, p2.color, p3.color, a, b, c);
    o.nx = p1.nx * a + p2.nx * b + p3.nx * c;
    o.ny = p1.ny * a + p2.ny * b + p3.ny * c;
    o.nz = p1.nz * a + p2.nz * b + p3.nz * c;
}

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
// THE TOPOLOGY BOOK (SMOOTHBULGE1B). The per-triangle policy above cannot tell
// a treasure chest from a character: both are low-poly meshes whose authored
// corner normals lean far off their own faces, and the PN patch pillows both.
// What tells them apart is the SHAPE OF THE WHOLE MESH, which one triangle
// cannot see. So every triangle that reaches the smoother is entered once in
// a book keyed on its raw model-space corners, triangles that share an edge
// are joined into a mesh, and a mesh that has stopped growing is classified:
//   BOX    flat faces meeting at hard edges whose authored normals lie (a
//          chest, a cannon block). Flat faces stay exactly flat, the edges
//          between faces and the curved parts stay sharp, curved parts (the
//          chest lid, the cannon barrel) are smoothed from the GEOMETRY'S own
//          welded normals instead of the lying authored ones.
//   FACET  a small faceted solid whose every authored normal lies (the Power
//          Star's body). It is left as the DS drew it.
//   FAN    a small flat polygon fan (the Power Star's eyes). Its outline is
//          rounded inside its own plane, so it never leaves the surface.
// Everything else (every character, enemy and round prop) is NONE and is
// smoothed exactly as before. A mesh that is not yet known to be complete is
// NONE too. SM64DS_SMOOTH_TOPO=0 turns the whole book off.
// ---------------------------------------------------------------------------
enum {
    SMOOTH_CLASS_NONE  = 0,
    SMOOTH_CLASS_BOX   = 1,
    SMOOTH_CLASS_FACET = 2,
    SMOOTH_CLASS_FAN   = 3
};
struct SmoothOverride {
    int cls;            // SMOOTH_CLASS_*
    int flat;           // every edge is straight: the patch is the flat triangle
    // The six edge normals in the model's own space, in the order of the
    // corners the caller passed: b210, b120, b021, b012, b201, b102.
    float en[6][3];
};
int smooth_topo_on();
// Enter the triangle (once) and say what its mesh is. Returns 0 for NONE.
int smooth_topo_lookup(const int16_t p[3][3], const uint32_t nrm[3],
                       uint32_t group, SmoothOverride *out);
// The grid positions of a patch whose six edge normals are given (model
// space or any space the corners are in), same layout as smooth_grid_positions.
int smooth_grid_positions_edges(const SmoothVertex &p1, const SmoothVertex &p2,
                                const SmoothVertex &p3, const float en[6][3],
                                int tf, float *out);
// A triangle whose corners rode three different matrices: 1 when it is a seam
// of a BOX model and should be drawn as the DS drew it.
int smooth_topo_cross(const int16_t p[3][3], uint32_t g0, uint32_t g1, uint32_t g2);
void smooth_topo_count(int what);
enum { SMOOTH_TOPO_BOX = 0, SMOOTH_TOPO_FACET, SMOOTH_TOPO_FAN, SMOOTH_TOPO_FLATKEEP };

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
    SMOOTH_COUNT_ORTHO,
    // A curved triangle the TICK'S OWN picture asked to subdivide (a
    // SmoothMotion replay of the same tick does not count again). This is the
    // budget's demand, see THE BUDGET below.
    SMOOTH_COUNT_DEMAND
};
void smooth_count(int which, uint64_t n);

// ---------------------------------------------------------------------------
// THE BUDGET (SM64DS_SMOOTH_BUDGET, in sub-triangles per picture; 0 is off).
//
// WHY. The level is a flat multiplier: every curved triangle becomes
// 4^level sub-triangles, whatever the scene. Ordinary courses at level 3 ask
// for 450 to 950 curved triangles a picture (29k to 61k sub-triangles) and
// hold full speed. The castle basement's goo painting is a 25 x 25 wave grid
// whose 1,150 triangles are ALL curved every frame, and Picture Poker's top
// screen is the same shape of thing: 1,280 to 1,590 curved triangles, 82k to
// 102k sub-triangles, 11 to 15 ms of patch maths on one processor thread, and
// the game slows down (that is where the "slow motion at the goo" reports
// come from).
//
// WHAT IT DOES. Each picture is drawn at ONE level for all of it: the set
// level, or the highest lower level whose demand (curved triangles x 4^level)
// fits the budget. The demand is what the previous tick's picture asked for,
// counted live, so the decision costs one compare per picture and the whole
// scene steps down together (a goo scene at level 3 looks like level 2, which
// is what it would look like had the player picked 2) instead of the models
// drawn last in a picture going flat. It steps down at once and back up one
// level at a time only after a second of fitting with an eighth of the
// budget to spare, so a scene on the border does not flicker. The floor is
// level 1, never 0, so the demand stays a property of the scene and not of
// the level it is being drawn at (a flat picture would ask for nothing and
// the next one would jump back up). The first picture after a level change
// or at boot is drawn at the set level; the count it produces decides the
// next.
//
// WHAT IT DOES NOT TOUCH. The game: the demand is read from the smoother's
// own verdicts and nothing game-visible is fed from it. Levels whose demand
// fits: a scene under the budget is drawn exactly as it was, every
// triangle, every vertex, so the ordinary courses are pixel-identical. The
// store: the level is part of the key, so grids built at two levels sit
// side by side and a level change never evicts anything.
// ---------------------------------------------------------------------------
int smooth_budget();          // the cap in sub-triangles per picture, 0 off
int smooth_level_drawn();     // the level this picture is drawn at (<= set)

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
    SMOOTH_PROF_TOPO,         // run hunt8 SMOOTHCPU2: the topology book's lookup / seam test
    SMOOTH_PROF_TRY,          // the whole of smooth_try, whatever it decided
    SMOOTH_PROF_NEAR,         // emit_tri_near for a triangle smooth_try left alone
    SMOOTH_PROF_PRE,          // smooth_try's entry checks and the corner copy, up to the policy
    SMOOTH_PROF_FIND,         // the shape store's lookup (key build and probe)
    SMOOTH_PROF_EMIT,         // smooth_emit_grid: the grid record, or the sub-triangles
    SMOOTH_PROF_BUCKETS
};
// 1 times the policy and the tessellation, which is the pair an A/B between
// the stored path and the live one has to compare. 2 also times each
// SUB-TRIANGLE, which is the finer split step 0 needed -- and which reads the
// clock thousands of times a frame, so it makes the live path look worse than
// it is and must never be the arm a comparison is drawn from.
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
    int32_t tf;          // the factor the grid was built at; 0 = seen once, no grid yet
    uint32_t off;        // first float of this entry's grid in the pool
    uint32_t pic;        // the picture that first saw it (smooth_store_miss)
    float box[6];        // run hunt8 SMOOTHCPU2: model-space min xyz then max xyz of the grid
};

// A SHAPE IS KEPT ON ITS SECOND SIGHTING, NOT ITS FIRST. The castle
// basement's goo painting moves its 625 vertices every frame, so every one
// of its 1,150 triangles is a new key every picture: 32,000 misses in 37
// frames, each one a 45-point grid into the pool, 19 MB of the 32 MB cap,
// then a clear that threw away every static model's grid along with them,
// so those were rebuilt too. A grid that will never be asked for again is
// not worth keeping; a grid that will be is asked for again on the very
// next picture. So on a miss the caller builds the grid, draws from it (the
// SAME floats the pool would have held, so the picture does not depend on
// whether the grid was kept) and calls smooth_store_miss: the first time a
// key is seen it is noted in a grid-less entry stamped with the picture and
// the answer is 0, keep nothing; seen again in one of the next two pictures
// the answer is 1 and the caller adds the grid for real (further apart than
// that it is noted afresh: the wave's keys do come round again, seconds
// later). The painting's keys are never seen twice in a row; a goomba's are
// seen on its second frame. The price of the
// rule is one extra live build per static shape, once, and the grid-less
// notes are what the cap purges first (SmoothStoreStats::purges) before it
// ever empties a grid.
int smooth_store_miss(const SmoothKey &k);

// WHAT IS DELIBERATELY NOT IN HERE: the policy verdict. An earlier version of
// this store remembered all of it, including whether the three corner normals
// agreed, and measured a DIFFERENT set of smoothed triangles from 0.4.0 --
// 742 a frame against 721 on Bob-omb Battlefield. The reason is worth keeping
// written down. The flat test compares corner normals at about one degree,
// and a position matrix is only a similarity to within the DS's own 1.19.12
// quantisation, so the angle between two nearly-equal normals is not quite
// the same before and after it. Asking in the model's space and asking in
// view space are therefore two slightly different questions near that
// threshold, and the picture may only change if nothing does.
// So the policy runs LIVE, on the view-space corners, every frame, through
// the same smooth_tess_factor 0.4.0 called. It is also the cheap half: the
// step-0 profiler measured it at 64 ms of 1206 ms on castle grounds against
// 415 ms for the patch maths this store does remove.

// Null on a miss. The returned pointer is valid until the next add.
const SmoothEntry *smooth_store_find(const SmoothKey &k);
// Insert. `grid` is smooth_grid_points(tf)*3 floats. Null only when the
// store refused the insert outright.
const SmoothEntry *smooth_store_add(const SmoothKey &k, int tf,
                                    const float *grid);
const float *smooth_store_grid(const SmoothEntry *e);
// run hunt8, lane SMOOTHGPU1: the pool the grids live in, for the card to
// mirror. The pool only ever grows by appending until smooth_store_clear, which
// bumps the generation; a mirror therefore uploads the tail it has not seen and
// starts again when the generation moves. `n` receives the float count.
const float *smooth_store_pool(size_t *n);
uint32_t smooth_store_generation();
uint32_t smooth_store_entry_off(const SmoothEntry *e);
// The six floats (min xyz, max xyz) of the grid points of `e`, kept with the
// entry so the card path does not re-scan up to 45 points per grid per picture.
const float *smooth_store_entry_box(const SmoothEntry *e);
// Called at the head of smooth_store_clear while the old pool is still intact,
// so the engine can turn any grid it has deferred and that points into the pool
// into ordinary triangles before the pool is thrown away.
void smooth_store_set_clear_hook(void (*fn)());
// Drop everything. Called when the level changes, since a level is the
// natural end of a set of models, and by the cap when the pool is full.
void smooth_store_clear();

struct SmoothStoreStats {
    uint64_t hits, misses, inserts, clears;
    uint64_t skip_crossmtx;   // corners from different matrices
    uint64_t skip_nonsim;     // position matrix is not a similarity
    uint64_t skip_zeronrm;    // a raw NORMAL payload was zero
    uint64_t live_calls;      // triangles that took the 0.4.0 path instead
    /* The two refusals above, broken down. Each one is a different thing to
       fix, and lumping them together hid which was costing the hit rate. */
    uint64_t cross_corner;    // two corners genuinely under different matrices
    uint64_t cross_normal;    // a NORMAL latched under a different matrix
    uint64_t cross_stale;     // the matrix moved between the last vertex and here
    uint64_t nonsim_len;      // the three basis images are not the same length
    uint64_t nonsim_ortho;    // they are not mutually perpendicular
    uint64_t nonsim_vec;      // the vector matrix carries a different rotation
    uint64_t entries;
    uint64_t bytes, cap_bytes;
    uint64_t held;            // first sightings drawn but not kept (smooth_store_miss)
    uint64_t purges;          // cap reached: the grid-less notes were dropped, grids kept
    uint64_t sweeps;          // old grid-less notes dropped early (too old to become a grid)
};
void smooth_store_stats(SmoothStoreStats &out);
// The geometry stage bumps the four skip rows through this, so gx.cpp holds
// no copy of the layout.
enum {
    SMOOTH_STORE_CROSSMTX = 0,
    SMOOTH_STORE_NONSIM,
    SMOOTH_STORE_ZERONRM,
    SMOOTH_STORE_LIVE,
    SMOOTH_STORE_CROSS_CORNER,
    SMOOTH_STORE_CROSS_NORMAL,
    SMOOTH_STORE_CROSS_STALE,
    SMOOTH_STORE_NONSIM_LEN,
    SMOOTH_STORE_NONSIM_ORTHO,
    SMOOTH_STORE_NONSIM_VEC
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

// THE CRACK BOOK (SM64DS_SMOOTH_CRACKS=<N>, every N pictures; off by default
// and then one compare). The census above compares NORMALS; this compares the
// EDGES AS DRAWN: every triangle the geometry stage handled is booked with
// what actually happened to it (curved from its corner normals, curved from
// substituted edge normals, or left as the DS drew it, and why), each shared
// edge's two sides are rebuilt as the curves they were drawn along, and the
// worst opening is reported in view units and in pixels, sorted by cause.
enum {
    SMOOTH_CK_STRAIGHT = 0,    // drawn as the DS drew it: the edge is a line
    SMOOTH_CK_CURVED,          // curved, edges from the six normals given
    // why a straight side stayed straight, beyond the SMOOTH_WHY_* reasons
    SMOOTH_CKWHY_MODE3 = 10, SMOOTH_CKWHY_ORTHO, SMOOTH_CKWHY_W,
    SMOOTH_CKWHY_FACET = 20, SMOOTH_CKWHY_BOOKFLAT, SMOOTH_CKWHY_CROSSKEEP
};
struct SmoothCrackTri {
    float p[3][3];        // view-space corners
    float clip[3][3];     // clip x, y, w of the corners (for the picture position)
    float en[6][3];       // the six edge normals the patch was built from (curved)
    int curved;           // SMOOTH_CK_*
    int why;              // SMOOTH_WHY_* / SMOOTH_CKWHY_* for a straight side
    int book;             // 1 = edge normals came from the topology book or the weld
    int tf;
    int16_t raw[3][3];
    uint32_t mg[3];
    uint32_t nrm[3];
    uint32_t pic;         // which picture of the tick (0 = the tick's own, then each in-between)
    uint32_t wseen;       // WELDALL1: 1 = the triangle was entered in the weld book
    uint32_t actor;       // WELDALL1: the actor whose Render drew it (class id), 0xFFFF none
};
int smooth_cracks_on();
// A SmoothMotion in-between starts: the crack book pairs edges only within one picture.
void smooth_crack_picture();
void smooth_crack_tri(const SmoothCrackTri &t);

// ---------------------------------------------------------------------------
// THE WELD BOOK (SMOOTHWELD1). An edge's curve depends only on its two ends
// and the two normals the patch hands it there, so two triangles that share an
// edge draw the same curve exactly when they hand it the same pair. Inside one
// smooth mesh they do. Where two pieces of a model meet (a belly against a
// body, a shoe against a leg), the artist gave each piece its own normals at
// the shared corners, and where one side is left as the DS drew it (flat, kept
// by the topology book) its edge is a straight line: in both cases the two
// curves part and the background shows through the seam.
//
// The book remembers, per edge of the model's own raw geometry (the two VTX
// positions, so the record outlives every bone matrix and animation frame),
// every triangle that has been drawn along it and how. A triangle about to be
// curved then draws each of its edges by the edge's rule, which every side
// computes from the same record and so agrees on bit for bit:
//   * a side drawn straight          -> the edge is straight on every side
//                                       (the curved side bends inside only);
//   * normals more than 45 degrees apart at an end (the topology book's
//     crease angle)                  -> a real crease: straight on every side;
//   * normals that differ less       -> both sides use one normal per end,
//                                       the sum of every side's, in the
//                                       order of the sides' ids;
//   * normals that agree exactly     -> left alone (every edge inside a mesh).
// Corners never move: only the edge's inner control points change. A triangle
// none of whose edges needs a rule returns 0 and is drawn exactly as before.
// ONE ANSWER A PICTURE (WELDALL1): when a record changes while a picture is
// drawn (a model coming into view meets its first neighbour, a book still
// learning), the answer it gave before the change stands for the rest of that
// picture, so a side drawn early and a side drawn late still meet; a side drawn
// straight on an edge kept curved follows that curve for the picture. The
// next picture draws every side by the record as it now stands.
// SM64DS_SMOOTH_WELD=0 turns the book off.
// ---------------------------------------------------------------------------
int smooth_weld_on();
enum { SMOOTH_WELD_STRAIGHT = 0, SMOOTH_WELD_CURVED = 1 };
// Enter one triangle's side of its three edges and, for a curved one, say how
// its edges are to be drawn. `en` is the six edge normals the triangle would
// curve with (b210, b120, b021, b012, b201, b102, in the raw model space of the
// corner each one sits at: 0,1,1,2,0,2); null for a straight triangle, or for a
// plain curved one (the authored normals are decoded from `nrm`). On a return
// of 1, `out` holds the six normals to draw with, in the same spaces, a zero
// vector meaning "this edge is straight", and `*id` a number that changes when
// they do (for the shape store's key). A STRAIGHT side returns 1 only for this
// picture, when an edge it shares was kept curved (a curved side drew it before
// the record knew of this one): `out` then holds the kept normals on that edge
// and zeros on the others, and the side is drawn bent along that edge only.
int smooth_weld(const int16_t p[3][3], const uint32_t nrm[3], int kind,
                const float (*en)[3], float out[6][3], uint32_t *id);
// The picture counter smooth_frame_mark advances (once a tick).
uint64_t smooth_frame_no();
// The weld book's epoch: it moves whenever any edge record changes.
uint32_t smooth_weld_epoch();
// THE CHURN GUARD. 1 while the book is still moving a lot from one tick to the
// next (many bodies arriving, a crowd of instances): the weld's answers are
// kept up to date but not applied, so those pictures are drawn exactly as
// without the weld (never worse than it). See weld_frame in smooth.cpp.
int smooth_weld_guarded();
// The textures were dropped (a model or level load, ntr::gx_invalidate_textures):
// the weld book is emptied at the next frame mark, so sides remembered from an
// earlier level can never straighten or average an unrelated later model whose
// raw edge happens to fall on the same coordinates.
void smooth_weld_scene_change();
// Edges that met more sides than the book records (a fifth), since boot.
uint64_t smooth_weld_overflow();
// WELDALL1: an in-between picture starts (the tick's own picture starts at the
// frame mark). Each edge gives one answer for the whole of one picture.
void smooth_weld_picture();
// WELDALL1: 1 when the last smooth_weld answer followed an edge's answer kept
// for this picture that its record no longer gives: good for this picture only,
// so it must not be reused in another (the replay memo stores it as stale).
int smooth_weld_provisional();
// WELDALL1: a side drawn straight could not follow an edge kept curved for this
// picture (no matrix for it): the guard counts these.
void smooth_weld_conflict();
// WELDALL1: VIEW PARTNERS. Two pieces of a model can meet along an edge that
// each stores at its own raw coordinates (a clam's two shells, a chest's lid,
// each in its own bone's space), so the raw-keyed book never sees the two as
// one edge. The geometry stage pairs the one-sided edges of curved triangles
// by where they are drawn, once a picture; two raw edges drawn on top of each
// other are marked, and from then on (until the book is emptied) both are
// drawn straight on every side, like a crease. smooth_weld_last_bnd gives the
// last answer's one-sided edges (bit k = edge k, corners k and k+1) and their
// records; smooth_weld_pic the picture being drawn.
uint32_t smooth_weld_pic();
uint32_t smooth_weld_last_bnd(uint16_t slot[3]);
void smooth_weld_partner(uint16_t a, uint16_t b);
// The replay memo's own count (SM64DS_SMOOTH_WELD_STATS): an answer read back, or asked again.
void smooth_weld_memo_count(int hit);

}  // namespace ntr

#endif  // NTR_SMOOTH_H
