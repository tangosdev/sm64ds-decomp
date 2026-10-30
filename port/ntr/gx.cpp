// DS geometry engine: command decode, matrix stacks, vertex assembly, raster.
//
// Command encoding and parameter counts from GBATEK. Fixed-point inputs are 4.12
// (fx32); the transform runs in float, which is what any host renderer would do
// and is not a fidelity question for geometry this size.

#include "ntr/gx.h"

#include "ntr/hdtex.h"
#include "ntr/mmio.h"
#include "ntr/smooth.h"
#include "ntr/texture.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstring>
#include <emmintrin.h>
#include <map>
#include <mutex>
#include <set>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#endif

extern "C" int (*port_fog_probe_hook)(unsigned char *o8) = 0;
namespace ntr {

static int g_ending_vp_shifted;   /* HUDMENU2 (H7): see gx_ending_viewport_shifted() */
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

__forceinline Vec4 mul(const Vec4 &v, const Mat &m) {   /* GHOSTPERF1: inline, so the four results stay in registers (a returned struct was stored narrow and loaded wide) */
    return {v.x * m.m[0] + v.y * m.m[4] + v.z * m.m[8] + v.w * m.m[12],
            v.x * m.m[1] + v.y * m.m[5] + v.z * m.m[9] + v.w * m.m[13],
            v.x * m.m[2] + v.y * m.m[6] + v.z * m.m[10] + v.w * m.m[14],
            v.x * m.m[3] + v.y * m.m[7] + v.z * m.m[11] + v.w * m.m[15]};
}

// --- engine state -----------------------------------------------------------
enum MtxMode { MTX_PROJ = 0, MTX_POS = 1, MTX_POSVEC = 2, MTX_TEX = 3 };

/* WHAT THE DISPLAY LIST SAID, for one vertex, before any matrix touched it.
   This is the material the shape store (ntr/smooth.h) keys on. It never
   reaches the polygon list and the raster never sees it. */
struct GxRaw {
    int16_t x, y, z;        // the VTX command's own coordinates
    uint32_t nrm;           // the NORMAL command's 30-bit payload
    uint32_t mgen;          // matrix generation this vertex was projected under
    uint32_t ngen;          // matrix generation that NORMAL was latched under
    uint8_t has_normal;     // normal_live: the colour came from lighting
};

/* A raw record the shape store can never accept: mgen and ngen disagree, so
   store_eligible refuses it before it looks at anything else. It is what a
   vertex carries when SmoothModels is 0 and nothing will read it anyway. */
const GxRaw g_raw_dead = {0, 0, 0, 0, 0, 1, 0};

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
    /* Host pixels per DS texel for the bound texture; see GxTriangle::tex_scale
       in ntr/gx.h. 1 is every texture the ROM supplies, and `g = State{}` in
       gx_reset puts it back to 1 at the head of every frame, so only a bind
       that knowingly replaced the image can leave it anything else. */
    uint8_t tex_scale = 1;
    /* The bound texture's identity for an optional graphics-card backend; see
       GxTriangle::tex_id in ntr/gx.h. 0 is untextured and is also what every
       run with no backend registered carries, because nothing hands one out. */
    uint32_t tex_id = 0;
    int prim = -1;                 // BEGIN_VTXS type, -1 when not inside a primitive
    uint32_t poly_attr = 0x80;     // POLYGON_ATTR latch; bit6 back, bit7 front
    int16_t vx = 0, vy = 0, vz = 0;
    std::vector<GxVertex> strip;   // vertices accumulated in the current primitive
    /* THE SAME STRIP, RAW. One row per row of `strip`, pushed and cleared
       with it, holding what the display list actually said (the VTX
       coordinates, the NORMAL payload, the matrix generation) rather than
       what the matrices made of it. It is a PARALLEL array rather than three
       more fields on GxVertex deliberately: GxVertex is embedded three times
       in every GxTriangle, GxTriangle is what the raster streams through
       memory a few tens of thousands of times a frame, and widening it by a
       quarter to carry data the raster never reads would cost more in memory
       traffic than the store saves in arithmetic. */
    std::vector<GxRaw> strip_raw;
    int strip_parity = 0;

    /* THE LATCHED VERTEX NORMAL, for the model smoother (ntr/smooth.h).
       NORMAL (0x21) is a latch on hardware exactly like TEXCOORD is: it holds
       until the next one, and a vertex submitted without a fresh NORMAL is
       lit with the last one. So this is kept the same way, in the same space
       the lighting used it in -- after the VECTOR matrix, normalised.

       normal_live is the part that is not just a mirror. It says the CURRENT
       vertex colour came from lighting a real surface normal, and it is
       cleared by the two commands that take the colour from somewhere else:
       COLOR (0x20) and a DIF_AMB with bit 15 set. Both mean "this polygon is
       not being lit", and a polygon that is not being lit -- a particle
       billboard, a fog quad, the HUD's own 3D geometry -- has no surface for
       a curved patch to follow. Without this, such a polygon would inherit
       whatever normal the last lit model happened to leave behind. */
    float nrm[3] = {0, 0, 0};
    int normal_live = 0;

    /* THE RAW HALF OF THE SAME LATCH, for the shape store (ntr/smooth.h).
       nrm_raw is the NORMAL command's own 30-bit payload, before the VECTOR
       matrix and before the normalise -- the number the MODEL carries, which
       is what makes a stored patch survive the model being loaded at a
       different address or drawn at a different place in the world.
       nrm_gen is the matrix generation that payload was latched under: a
       normal transformed by one matrix and a position transformed by another
       do not share a local space, and the store refuses the pair rather than
       guessing. Both are written only on the NORMAL path, beside the three
       floats above, and nothing but the store reads them. */
    uint32_t nrm_raw = 0;
    uint32_t nrm_gen = 0;

    /* THE PRESENT RECTANGLE, not the whole extent: a scene presented at the
       native 4:3 field draws into the centred sub-rectangle and leaves the
       spare width as margin. present_* IS the active extent on every other
       run, so this is the same default it always was. `g = State{}` inside
       gx_reset re-evaluates these every frame, after configure_aspect. */
    int vp_x = present_x(), vp_y = present_y(),
        vp_w = present_w(), vp_h = present_h();
    /* How many VIEWPORT commands have executed since the last gx_reset. The
       default above IS a full-screen rectangle, so a sampled viewport of
       0,0 SCREEN_W x SCREEN_H cannot on its own tell a game-issued
       full-screen viewport from a frame that never issued one at all. This
       counter is the difference, and without it the viewport row reports a
       rectangle it cannot attribute. */
    int vp_writes = 0;

    /* MENU WIDE 3D (run menu1, lane FILE2): evaluated at most once per frame, see menu_wide(). */
    int mw_eval = 0, mw_on = 0;
    float mw_zoom = 1.0f, mw_dy = 0.0f;
    /* TITLE092: the scene transform in clip units (x' = x * mw_kx + mw_tx * w, y' = y * mw_ky + mw_ty * w). The FILE2 zoom is the case
       kx = ky = zoom, tx = 0, ty = dy; a scene callback (gx_set_menu_scene_cb) sets all four. */
    float mw_kx = 1.0f, mw_tx = 0.0f, mw_ky = 1.0f, mw_ty = 0.0f;

    // Lighting state. diffuse/ambient/specular/emission are 0..1 per channel.
    float diffuse[3] = {1, 1, 1}, ambient[3] = {0, 0, 0};
    float emission[3] = {0, 0, 0};
    struct Light { float dx, dy, dz; float r, g, b; } lights[4] = {};
    uint32_t light_mask = 0;

    std::vector<GxTriangle> tris;
    /* run hunt8, lane SMOOTHGPU1: smoothed grids kept whole for the card (see
       GxGrid in ntr/gx.h). grid_pts is the frame arena the points of a grid that
       is not in the shape store's pool are copied to. grid_subtris is how many
       triangles the grids stand for, so a count of the frame's polygons is
       tris.size() + grid_subtris. */
    std::vector<GxGrid> grids;
    std::vector<float> grid_pts;
    uint32_t grid_subtris = 0;
    uint8_t grid_translucent = 0;
    uint32_t grid_mode2 = 0;
};

State g;
int g_store_count;

/* MOTION INTERPOLATION (run interp1): the recorder's switches, declared here
   because exec() and the texture binds below feed it. The whole mechanism is
   the block after gx_render; with the SmoothMotion key off g_ip_armed is 0
   and every hook is one test of it. */
enum { IPK_CMD = 0, IPK_BIND, IPK_LIGHT, IPK_LMASK, IPK_SLOT, IPK_SPRITE, IPK_SPRXF };
int g_ip_armed = 0;       // the key is on: level frames are recorded
int g_ip_rec = 0;         // a frame's stream is being recorded right now
int g_ip_tick_div = 0;    // GHOSTPERF1: ... and its ghost figures' triangles go straight into the record (not the frame list)
void ip_tick_ghost_push(const GxTriangle &t);
bool ip_ghost_fast_on();
int g_ip_replaying = 0;   // a recorded stream is being replayed right now
int g_ip_open = 0;        // gx_interp_begin has saved the live state
int g_ip_in_vram = 0;     // inside the VRAM bind, which records its own result
uint32_t g_ip_dma_src = 0;
void ip_push(uint8_t kind, uint8_t cmd, int np, const uint32_t *p);
const GxTriangle *ip_diverted_ghosts(size_t &n);   /* a picture between ticks: the figures its replay made, kept apart from the frame list */
const GxTriangle *ip_sealed_ghosts(size_t &n);   /* the record just sealed: its ghost triangles, or null when it has none it vouches for */
void ip_rec_bind_now();
void ip_rec_begin();
void ip_rec_seal();
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
    if (g_ip_replaying) return;   /* a replay is never visible to the game */
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
    /* MODESEL: the object's own DS-PIXEL rectangle, so a row can be read
       straight against the ROM's own element box. Filled with the same
       divide and viewport mapping to_screen uses, halved because the
       raster runs at 2x. Debug census only; nothing reads it. */
    struct Row { float a, b, c, tx, ty, tz; int n;
                 float sx0, sx1, sy0, sy1; };
    Row tab[32]; int ntab = 0;
    unsigned frame = 0;
};
StarGeo g_stargeo;
int stargeo_on() {
    if (g_stargeo.on < 0) g_stargeo.on = std::getenv("SM64DS_STAR_GEO") ? 1 : 0;
    return g_stargeo.on;
}

/* ===========================================================================
   THE MATRIX GENERATION, and the one question the shape store asks about a
   matrix (ntr/smooth.h has the store's own contract).

   A stored patch is built in the model's OWN space out of raw VTX
   coordinates and raw NORMAL payloads, and replayed by pushing the stored
   points through the current position matrix. That is the same patch the
   live path builds from transformed corners only when the matrix is a
   SIMILARITY -- a rotation and a uniform scale and a translation. Work it
   through and the reason is one line of algebra: the PN edge control point
   is (2Pi + Pj - Ni * dot(Pj - Pi, Ni)) / 3, a rotation leaves that dot
   product alone, a uniform scale s multiplies both the dot and the
   difference by s, and the patch evaluation is an affine combination
   (the ten Bernstein weights sum to one), so the whole grid transforms with
   the matrix. A NON-uniform scale or a shear breaks it, because the dot
   product is then not the one the patch needed and there is no single R to
   carry the normals.

   The normals also have to ride the same rotation, and on this engine they
   ride the VECTOR matrix rather than the position one. In MTX_MODE 2 the two
   are loaded together and are the same numbers; when they are not, the store
   stands down rather than assuming. Nothing here approximates: a matrix that
   does not pass goes down the live path and is counted.

   HOW A CHANGE IS NOTICED WITHOUT TOUCHING THE MATRIX COMMANDS. A dozen
   command cases write g.pos or g.vec, and none of them is this lane's to
   edit. So instead of an increment in each, the two matrices are remembered
   here and compared -- two 64-byte memcmps -- at the two moments the store
   cares about: when a vertex is projected and when a NORMAL is latched. Two
   matrices that are numerically equal but bitwise different read as a change,
   which costs a cache miss and never a wrong answer. The whole block is
   behind `smooth_level() > 0`, so with the setting absent it is one compare
   on a file-scope int, which is what it was before this lane.
   =========================================================================== */
Mat g_mtx_seen_pos, g_mtx_seen_vec;
uint32_t g_mtx_gen = 0;      // bumped whenever either matrix changes
int g_mtx_similar = 0;       // pos is a similarity AND vec agrees with it

/* THE LAST FEW MATRICES, kept so a triangle replays through the matrix its
   OWN CORNERS rode rather than through whatever happens to be live when the
   third corner arrives. The two are usually the same, and the first version
   of this code simply required it -- and refused 236 triangles a frame on
   castle grounds, nearly half of everything otherwise eligible, because a
   quad and a quad strip each emit a triangle whose last named corner is not
   the last vertex submitted. Eight slots is far more than the one or two a
   primitive ever spans, and a slot is used only while its generation still
   matches, so a matrix that has aged out is a miss and never a wrong
   answer. */
enum { MTX_RING = 64 };
Mat g_mtx_ring[MTX_RING];
uint32_t g_mtx_ring_gen[MTX_RING];
int g_mtx_ring_sim[MTX_RING];

/* Is the live position matrix a similarity, and does the vector matrix carry
   the same rotation? Runs once per matrix change, never per vertex. */
int mtx_similar_check() {
    /* mul(v, m) forms out_j = sum_i v_i * m[4i + j], so the image of basis
       vector e_i is the row (m[4i], m[4i+1], m[4i+2]). A similarity is three
       mutually orthogonal images of equal length.

       THE TOLERANCE IS NOT COSMETIC, and the first version of it cost the
       whole feature. A DS matrix is 1.19.12 fixed point, so every component
       the cartridge authored is quantised to a 4096th. A rotation stored that
       way has row lengths that disagree by up to about eight parts in ten
       thousand, and rows whose dot product misses zero by about as much,
       before the engine multiplies two of them together. One part in ten
       thousand -- which is what this read first -- therefore refuses almost
       every real model matrix in the game: castle grounds took the stored
       path exactly zero times. One part in a hundred admits the quantisation
       with room to spare, and what it lets through is a non-uniform scale or
       a shear of half a percent, which moves a patch by half a percent of a
       bulge that is itself about a sixth of the triangle. What actually gets
       through is then measured against the live patch by the
       SM64DS_SMOOTH_ABDIFF arm, so this number is checked and not argued. */
    const float *m = g.pos.m;
    float u[3][3];
    for (int i = 0; i < 3; ++i)
        for (int k = 0; k < 3; ++k) u[i][k] = m[i * 4 + k];
    const float l0 = u[0][0]*u[0][0] + u[0][1]*u[0][1] + u[0][2]*u[0][2];
    const float l1 = u[1][0]*u[1][0] + u[1][1]*u[1][1] + u[1][2]*u[1][2];
    const float l2 = u[2][0]*u[2][0] + u[2][1]*u[2][1] + u[2][2]*u[2][2];
    if (l0 <= 1e-12f) {
        smooth_store_count(SMOOTH_STORE_NONSIM_LEN, 1);
        return 0;
    }
    const float eps = 1e-2f;                 // relative, on the squared length
    if (std::fabs(l1 - l0) > eps * l0 || std::fabs(l2 - l0) > eps * l0) {
        smooth_store_count(SMOOTH_STORE_NONSIM_LEN, 1);
        return 0;
    }
    const float d01 = u[0][0]*u[1][0] + u[0][1]*u[1][1] + u[0][2]*u[1][2];
    const float d02 = u[0][0]*u[2][0] + u[0][1]*u[2][1] + u[0][2]*u[2][2];
    const float d12 = u[1][0]*u[2][0] + u[1][1]*u[2][1] + u[1][2]*u[2][2];
    if (std::fabs(d01) > eps * l0 || std::fabs(d02) > eps * l0 ||
        std::fabs(d12) > eps * l0) {
        smooth_store_count(SMOOTH_STORE_NONSIM_ORTHO, 1);
        return 0;
    }

    /* And the VECTOR matrix has to carry the SAME ROTATION, or the normals
       the live path lights with are not the normals a stored patch was
       curved by. It need not carry the same SCALE: a transformed normal is
       normalised, so any positive multiple of the rotation gives the same
       unit normal. The two are therefore compared as directions, each row
       divided by its own matrix's scale, rather than as numbers. */
    const float *w = g.vec.m;
    const float k0 = w[0]*w[0] + w[1]*w[1] + w[2]*w[2];
    if (k0 <= 1e-12f) {
        smooth_store_count(SMOOTH_STORE_NONSIM_VEC, 1);
        return 0;
    }
    const float sp = std::sqrt(l0), sv = std::sqrt(k0);
    for (int i = 0; i < 3; ++i)
        for (int kk = 0; kk < 3; ++kk) {
            const float a = m[i * 4 + kk] / sp;
            const float b = w[i * 4 + kk] / sv;
            if (std::fabs(a - b) > eps) {
                smooth_store_count(SMOOTH_STORE_NONSIM_VEC, 1);
                return 0;
            }
        }
    return 1;
}

void mtx_gen_update() {
    const Mat &P = g.pos;
    if (std::memcmp(&g_mtx_seen_pos, &P, sizeof(Mat)) == 0 &&
        std::memcmp(&g_mtx_seen_vec, &g.vec, sizeof(Mat)) == 0)
        return;
    g_mtx_seen_pos = P;
    g_mtx_seen_vec = g.vec;
    ++g_mtx_gen;
    g_mtx_similar = mtx_similar_check();
    const int slot = static_cast<int>(g_mtx_gen & (MTX_RING - 1));
    g_mtx_ring[slot] = P;
    g_mtx_ring_gen[slot] = g_mtx_gen;
    g_mtx_ring_sim[slot] = g_mtx_similar;
}

/* The matrix generation `gen` rode, or null if it has aged out of the ring or
   was not one a stored patch may be replayed through. */
const Mat *mtx_for_gen(uint32_t gen) {
    const int slot = static_cast<int>(gen & (MTX_RING - 1));
    if (g_mtx_ring_gen[slot] != gen || !g_mtx_ring_sim[slot]) return 0;
    return &g_mtx_ring[slot];
}

/* MENU WIDE 3D (run menu1, lane FILE2; the only change in this file for it, see also the VIEWPORT case, raster_clip and
   gx_clear_fill). A scene is presented at the DS's own centred 4:3 rectangle, so a one-screen menu page whose background
   should fill a wide window had nothing to draw it into. A host callback (null on every run that never installs one, which is
   the whole of the old behaviour: menu_wide() is then one test of a null pointer) says, once per frame and from the ROM's own
   page state, "this frame's 3D covers the whole wide frame". The sprite objects the title submits through
   func_ov007_020c421c (plates, glyphs, arrows: gx_menu_sprite brackets each submit) land exactly where they do natively;
   everything else (the page model: the castle reflection) is zoomed by the callback's factor about the frame centre. */
bool (*g_menu_wide_cb)(float *zoom, float *dy);
bool (*g_menu_scene_cb)(float xf[4]);       /* TITLE092: kx, tx, ky, ty (see State::mw_kx) */
int g_menu_sprite;
int g_menu_sprxf_on;                         /* TITLE092: the sprite object being drawn has its own transform */
float g_menu_sprxf[4];
bool menu_wide() {
    if (!g_menu_wide_cb && !g_menu_scene_cb) return false;
    if (!g.mw_eval) {
        g.mw_eval = 1;
        float z = 1.0f, dy = 0.0f;
        float xf[4] = { 1.0f, 0.0f, 1.0f, 0.0f };
        if (g_menu_scene_cb) {
            g.mw_on = g_menu_scene_cb(xf) ? 1 : 0;
            g.mw_kx = xf[0]; g.mw_tx = xf[1]; g.mw_ky = xf[2]; g.mw_ty = xf[3];
            g.mw_zoom = 1.0f; g.mw_dy = 0.0f;
        } else {
            g.mw_on = g_menu_wide_cb(&z, &dy) ? 1 : 0;
            g.mw_zoom = z; g.mw_dy = dy;
            g.mw_kx = z; g.mw_tx = 0.0f; g.mw_ky = z; g.mw_ty = dy;
        }
        if (g.mw_on) { g.vp_x = 0; g.vp_y = 0; g.vp_w = active_w; g.vp_h = active_h; }
    }
    return g.mw_on != 0;
}

/* VIEW SPACE -> CLIP SPACE, factored out of project() and out of nothing
   else. The model smoother (ntr/smooth.h) projects the vertices it invents
   through THIS function rather than through a copy of these six lines, so a
   subdivided vertex cannot drift away from the corner it came from: the
   projection matrix, the widescreen widen and its perspective test are one
   piece of code with one caller each. Byte-identical to what project() did
   inline before, same expression, same order. */
/* MISC093 (D21 / B3): the menu-wide placement of one clip-space point, view_to_clip's menu branch on its own, so the smoother's
   grid emit (smooth_emit_grid_cpu, which projects its own points four lanes at a time) puts a tessellated triangle where the
   plain path puts the untessellated one. Same expression, same order. */
static inline void menu_wide_place(float &cx, float &cy, float cw) {
    cx *= (float)present_w() / (float)active_w;
    cy *= (float)present_h() / (float)active_h;
    if (!g_menu_sprite) {
        if (g.mw_kx != 1.0f || g.mw_tx != 0.0f || g.mw_ky != 1.0f || g.mw_ty != 0.0f) {
            cx *= g.mw_kx;
            if (g.mw_tx != 0.0f) cx += g.mw_tx * cw;
            cy *= g.mw_ky;
            if (g.mw_ty != 0.0f) cy += g.mw_ty * cw;
        }
    } else if (g_menu_sprxf_on) {
        cx = cx * g_menu_sprxf[0] + g_menu_sprxf[1] * cw;
        cy = cy * g_menu_sprxf[2] + g_menu_sprxf[3] * cw;
    }
}

__forceinline Vec4 view_to_clip(const Vec4 &view) {
    Vec4 c = mul(view, g.proj);
    if (menu_wide()) {
        /* MENU WIDE 3D: the viewport is the whole wide frame, so EVERY projection (ortho included) is squeezed to where the
           native 4:3 rectangle sits in it; the page's background alone is then zoomed about the frame's centre (a camera
           zoom: clipping crops it, the texels stay sharp), the sprite objects are not. */
        c.x *= (float)present_w() / (float)active_w;
        c.y *= (float)present_h() / (float)active_h;
        if (!g_menu_sprite) {
            if (g.mw_kx != 1.0f || g.mw_tx != 0.0f || g.mw_ky != 1.0f || g.mw_ty != 0.0f) {
                c.x *= g.mw_kx;
                if (g.mw_tx != 0.0f) c.x += g.mw_tx * c.w;
                c.y *= g.mw_ky;
                if (g.mw_ty != 0.0f) c.y += g.mw_ty * c.w;
            }
        } else if (g_menu_sprxf_on) {
            c.x = c.x * g_menu_sprxf[0] + g_menu_sprxf[1] * c.w;
            c.y = c.y * g_menu_sprxf[2] + g_menu_sprxf[3] * c.w;     /* TITLE092: this sprite object's own place and size (gx_menu_sprite_xf) */
        }
        return c;
    }
    if (g.proj.m[3] != 0.0f || g.proj.m[7] != 0.0f || g.proj.m[11] != 0.0f) {
        const float widen =
            (4.0f / 3.0f) * ((float)present_h() / (float)present_w());
        c.x *= widen;
    }
    return c;
}

GxVertex project(int16_t x, int16_t y, int16_t z) {
    const Vec4 v{x * FX12, y * FX12, z * FX12, 1.0f};
    const Vec4 view = mul(v, current_pos());
    Vec4 c = view_to_clip(view);

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
       4:3 field is byte-for-byte the old one -- no #ifdef needed.

       OFF THE PRESENT RECTANGLE, which is the active extent on every run that
       widens and the centred 256:192 sub-rectangle on a scene presented
       natively. 256:192 is 4:3 exactly, so on that path this factor is
       EXACTLY 1.0 and the field is the cartridge's own -- the same way it is
       already exactly 1.0 at any 4:3 aspect.

       The multiply itself now lives in view_to_clip() just above, because the
       smoother has to apply the same one to the vertices it invents. */

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
                            P.m[12], P.m[13], P.m[14], 0,
                            1e30f, -1e30f, 1e30f, -1e30f};
                ++G.ntab;
            }
            if (k < G.ntab) {
                ++G.tab[k].n;
                if (std::fabs(c.w) > 1e-6f) {
                    const float iw = 1.0f / c.w;
                    const float sx = ((c.x * iw + 1.0f) * 0.5f * g.vp_w
                                      + g.vp_x) * 0.5f;
                    const float sy = ((1.0f - (c.y * iw + 1.0f) * 0.5f)
                                      * g.vp_h + g.vp_y) * 0.5f;
                    StarGeo::Row &R = G.tab[k];
                    if (sx < R.sx0) R.sx0 = sx;
                    if (sx > R.sx1) R.sx1 = sx;
                    if (sy < R.sy0) R.sy0 = sy;
                    if (sy > R.sy1) R.sy1 = sy;
                }
            }
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
    /* MDL: the view-space half, for the smoother. Written on every path so
       the struct is fully determined -- smoke_gx memcmps whole GxTriangles
       between the two submit paths and an unwritten field would make that
       comparison a coin toss. */
    out.vx = view.x; out.vy = view.y; out.vz = view.z; out.vw = view.w;
    out.nx = g.normal_live ? g.nrm[0] : 0.0f;
    out.ny = g.normal_live ? g.nrm[1] : 0.0f;
    out.nz = g.normal_live ? g.nrm[2] : 0.0f;
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
    /* MDL: the appended view-space fields are interpolated too. They have to
       be written on this path as well -- see the note in project() about the
       whole struct being determined -- and linear interpolation is right:
       clipping is a linear operation in the space above, and the near-plane
       intersection of the view-space edge is the same point. */
    o.vx = a.vx + (b.vx - a.vx) * t;
    o.vy = a.vy + (b.vy - a.vy) * t;
    o.vz = a.vz + (b.vz - a.vz) * t;
    o.vw = a.vw + (b.vw - a.vw) * t;
    o.nx = a.nx + (b.nx - a.nx) * t;
    o.ny = a.ny + (b.ny - a.ny) * t;
    o.nz = a.nz + (b.nz - a.nz) * t;
    uint32_t ca = a.color, cb = b.color, c = 0;
    for (int s = 0; s < 32; s += 8) {
        const float ch = ((ca >> s) & 0xFF) +
                         (float(int((cb >> s) & 0xFF) - int((ca >> s) & 0xFF))) * t;
        c |= (uint32_t(ch < 0 ? 0 : ch > 255 ? 255 : ch) & 0xFF) << s;
    }
    o.color = c;
    return o;
}

/* W-BUFFERED DEPTH (the DS's own, SWAP_BUFFERS bit 1). The game writes 3 to
   0x04000540 every frame (src/func_020190b8.c, src/Initialise3dGraphics.cpp),
   so its depth buffer holds W, not the post-divide Z. The raster below used
   Z, which in float sits within a few units of 1.0 across the whole far half
   of a perspective scene: the eyes of a star-select star (flush with the
   star body) and a pond's water plane against its cliff walls then fell
   inside one ulp of each other and z-fought. g_frame_ortho notes that this
   frame submitted a triangle through a constant-w projection (the 2D-in-3D
   screens); those keep Z, because W is the same for every polygon of an
   orthographic scene and would order nothing. g_wdepth is the per-frame
   decision gx_render makes from it. The depth key is -(1/w) interpolated
   linearly in screen space: larger is farther, it orders exactly as W does,
   and a float keeps its full relative precision on it where the post-divide
   Z leaves it only the last few bits below 1.0. SM64DS_WDEPTH=0 keeps the
   old Z key, for an A/B on one binary. */
static bool g_frame_ortho;
static bool g_wdepth;
/* THE GHOST LAYER SWITCH (gx_set_ghost_layer, ntr/gx.h): while it is on, a
   triangle submitted under a POLYGON_ATTR with bits 21-23 set carries that
   number as GxTriangle::ghost. Off, the bits are never read. */
static bool g_ghost_layer_on;

/* THE FRAME'S FOG (run hunt8, RENDER093, D12). Stage::RenderFog writes
   DISP3DCNT bit 7 (and the shift in bits 8-11), FOG_COLOR, FOG_OFFSET and the
   32 FOG_TABLE bytes every frame; nothing here ever read them, so every fog
   level drew clear. This is melonDS' CalculateFogDensity, one frame latch and
   one per-pixel lookup, applied to the polygons whose POLYGON_ATTR bit 15 is
   set: the density 0..128 comes from the pixel's 24-bit depth (W is 12 bit
   fraction, so w * 4096; Z is the 0..1 key * 0xFFFFFF) and blends the colour
   toward the fog colour. DISP3DCNT bit 6 (alpha only) leaves the colour alone,
   so it counts as no fog here. */
struct FogState {
    int on = 0;
    int shift = 0;
    uint32_t offset = 0;
    float rgb[3] = {0, 0, 0};
    int tab[34] = {0};
    float wscale = 256.0f;   /* depth24 = w * wscale in a W frame, see fog_latch */
};
static FogState g_fog;

static void fog_latch() {
    g_fog.on = 0;
    if (!io_ready()) return;
    static const int off = std::getenv("SM64DS_FOG") && *std::getenv("SM64DS_FOG") == '0';
    const uint32_t cnt = *reinterpret_cast<const volatile uint16_t *>(0x04000060u);
    if (off || !(cnt & 0x80u) || (cnt & 0x40u)) return;
    g_fog.on = 1;
    g_fog.shift = (int)((cnt >> 8) & 0xF);
    g_fog.offset = ((uint32_t)(*reinterpret_cast<const volatile uint16_t *>(0x0400035Cu)) & 0x7FFFu) * 0x200u;
    /* The depth a W frame fogs by is the 24-bit W, 4096 a unit, in melonDS and
       on paper; fed straight in, every pixel the game draws lands past the
       table's end and the whole picture is fog colour. The table the ROM
       builds only spreads over the scene at a sixteenth of that, so the W
       key is taken at 256 a unit. Calibrated by eye, not read off a cartridge
       capture; SM64DS_FOG_WSCALE=4096 gives the literal figure. */
    static const float ws = std::getenv("SM64DS_FOG_WSCALE") ? (float)std::atof(std::getenv("SM64DS_FOG_WSCALE")) : 256.0f;
    g_fog.wscale = ws;
    const uint32_t col = *reinterpret_cast<const volatile uint32_t *>(0x04000358u);
    for (int k = 0; k < 3; ++k) {
        const uint32_t c5 = (col >> (5 * k)) & 31u;
        /* FOG_COLOR is BGR555: bits 0-4 are RED, 5-9 green, 10-14 BLUE (melonDS FogColor, GBATEK). rgb[] is R, G, B
           and the rasters index it that way. It used to be filled back to front, so every fog drew with red and blue
           swapped: the castle moat's default underwater fog (0x6000, a deep blue) came out dark red and Jolly Roger
           Bay / Dire Dire Docks' light blue came out tan (thunderit's "red water", 0.9.3 round 6). Grey fogs hid it. */
        g_fog.rgb[k] = (float)((c5 << 3) | (c5 >> 2));
    }
    for (int i = 0; i < 32; ++i) {
        const int v = *reinterpret_cast<const volatile uint8_t *>(0x04000360u + i) & 0x7F;
        g_fog.tab[i + 1] = v;
    }
    g_fog.tab[0] = g_fog.tab[1];
    g_fog.tab[33] = g_fog.tab[32];   /* the table's tail is flat, as the hardware's is */
}

static inline uint32_t fog_density(float depth24) {
    uint32_t z = depth24 >= 16777215.0f ? 0xFFFFFFu : (depth24 <= 0.0f ? 0u : (uint32_t)depth24);
    uint32_t id, frac;
    if (z < g_fog.offset) { id = 0; frac = 0; }
    else {
        z -= g_fog.offset;
        z = (z >> 2) << g_fog.shift;
        id = z >> 17;
        if (id >= 32) { id = 32; frac = 0; }
        else frac = z & 0x1FFFFu;
    }
    const uint32_t d = (uint32_t)(((uint64_t)g_fog.tab[id] * (0x20000u - frac) + (uint64_t)g_fog.tab[id + 1] * frac) >> 17);
    return d >= 127u ? 128u : d;
}

/* THE W-BUFFER'S TIE RULE (run hunt7, lane POLES1). The depth key above is
   exact to a float ulp, and the cartridge's is not: its geometry engine cuts
   every polygon's vertex W to 16 significant bits before it interpolates
   (melonDS GPU3D.cpp SetupPolygon: wsize is the smallest multiple of four bits
   that holds the polygon's largest W, and the low wsize-16 bits of each vertex
   W are dropped), the interpolated depth is an integer, and the test is a
   strict less-than. Two polygons on one plane therefore land on the SAME
   integer and the one drawn first keeps the pixel. With the key left at float
   precision, two coincident polygons differ by rounding noise and the pixel
   goes to whichever one the noise favours: Hazy Maze Cave's second room draws
   each climb pole twice (cave_all.bmd r1, the red box then a copy in the cave
   wall's texture, same eight corners) and the pole came out speckled.
   w_tie_q is that integer's step in the port's w units (one raw unit is
   1/4096); a later polygon must be nearer than the pixel by one step to win.
   In the 1/w key a step of Q in w is Q * k * k, k = 1/w = -key, to first order
   (Q * k is at most 2^-12, so the first order is exact enough). A Z frame
   passes q = 0, and then the test is the old z >= d bit for bit. */
static inline float w_tie_q(const GxVertex &a, const GxVertex &b,
                            const GxVertex &c) {
    float m = std::fabs(a.w);
    if (std::fabs(b.w) > m) m = std::fabs(b.w);
    if (std::fabs(c.w) > m) m = std::fabs(c.w);
    const float raw = m * 4096.0f;
    const uint32_t r = raw >= 4294967040.0f ? 0xFFFFFFFFu : (uint32_t)raw;
    unsigned wsize = 0;
    while (wsize < 32 && (r >> wsize)) wsize += 4;
    const unsigned s = wsize > 16 ? wsize - 16 : 0;
    return (float)(1u << s) * (1.0f / 4096.0f);
}
static inline bool w_depth_fail(float z, float d, float q, bool eq = false) {
    if (eq) {
        /* POLYGON_ATTR bit 14: the depth test is EQUAL, not LESS (melonDS
           GPU3D_Soft.cpp DepthTest_Equal_W / _Equal_Z: |dst - z| <= 0xFF in
           W-buffered depth units, 0x200 in Z). A pixel nothing has drawn
           holds the far value and fails, as on the hardware. The W key is
           -(1/w), so one raw W unit (q, the same 16-bit step the tie rule
           uses) is q * d * d of key. The Z key is the post-divide z, whose
           24-bit step is 2 / 2^24. */
        if (d >= 0.0f) return true;
        const float tol = q > 0.0f ? 255.0f * q * d * d : 512.0f * (2.0f / 16777216.0f);
        const float diff = z - d;
        return !(diff >= -tol && diff <= tol);
    }
    return z >= (d < 0.0f ? d - q * d * d : d);
}

/* THE STATE HALF OF A POLYGON-LIST ENTRY: everything push_screen_tri stamps
   on a triangle except its three vertices. It depends only on the live
   material state, so a smoothed grid (smooth_emit_grid) builds it once and
   copies it for each of its sub-triangles instead of rebuilding it 64 times.
   `t` must be value-initialised by the caller (padding stays deterministic). */
static unsigned long g_rp_tl, g_rp_tl_dw, g_rp_fog_poly;
static inline void fill_tri_state(GxTriangle &t) {
    t.tex = g.tex_rgba; t.tw = g.tw; t.th = g.th;
    /* the bound texture's host-pixels-per-texel travels with the triangle
       exactly as its dimensions do; 1 unless a pack replaced the image */
    t.tex_scale = g.tex_scale ? g.tex_scale : 1;
    /* and its identity, for a backend that keeps its own copy; 0 on every run
       with no backend registered, because nothing handed one out */
    t.tex_id = g.tex_id;
    t.cull = static_cast<uint8_t>((g.poly_attr >> 6) & 3);
    t.alpha = static_cast<uint8_t>((g.poly_attr >> 16) & 31);
    t.mode = static_cast<uint8_t>((g.poly_attr >> 4) & 3);
    t.polyid = static_cast<uint8_t>((g.poly_attr >> 24) & 63);
    t.ghost = g_ghost_layer_on
                  ? static_cast<uint8_t>((g.poly_attr >> 21) & 7) : 0;
    t.depth_eq = static_cast<uint8_t>((g.poly_attr >> 14) & 1);
    t.fog = static_cast<uint8_t>((g.poly_attr >> 15) & 1);
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
    if (t.translucent) { ++g_rp_tl; if (g.poly_attr & 0x800u) ++g_rp_tl_dw; }
    if (g.poly_attr & 0x8000u) ++g_rp_fog_poly;
}

void push_screen_tri(const GxVertex &a, const GxVertex &b, const GxVertex &c) {
    if (g.proj.m[3] == 0.0f && g.proj.m[7] == 0.0f && g.proj.m[11] == 0.0f)
        g_frame_ortho = true;
    /* value-initialised: smoke_gx memcmps whole GxTriangles between the two
       submit paths, so the padding has to be deterministic */
    GxTriangle t{};
    t.v[0] = a; t.v[1] = b; t.v[2] = c;
    fill_tri_state(t);
    if (t.ghost && g_ip_rec && g_ip_tick_div) { ip_tick_ghost_push(t); return; }
    g.tris.push_back(t);
    /* MDL: what actually reached the polygon list. Paired with
       SMOOTH_COUNT_IN in emit_tri, this is the submitted-versus-emitted row
       of the measurement table, and the two move together on every triangle
       the smoother declined. */
    smooth_count(SMOOTH_COUNT_OUT, 1);
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

void emit_tri_near(const GxVertex &a, const GxVertex &b, const GxVertex &c) {
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

/* ===========================================================================
   MODEL SMOOTHING (the "SmoothModels" setting; ntr/smooth.h has the kernel
   and the whole argument). It sits HERE, between the assembler and the near
   clip, for three reasons:

   WHY BEFORE THE CLIP. A curved patch is only meaningful in a linear space.
   After the near clip a triangle may be a quad, and after the perspective
   divide straight edges are no longer straight in the coordinates being
   interpolated. So the input is the view-space vertex project() carried down
   here, the new vertices are projected by project()'s own view_to_clip(), and
   every one of them then goes through the SAME near clip, the same
   to_screen() and the same push_screen_tri() as an ordinary triangle. The
   clip therefore never sees anything it has not always seen.

   WHY NOTHING IS BUFFERED. The normals are the model's own, out of the
   display list's NORMAL commands, so a patch depends only on the three
   corners in front of it. Nothing has to be held back to the end of a model
   to be welded, which means the triangles reach the polygon list in exactly
   the submission order the game chose -- the order translucent sorting and
   the mode-3 shadow stencil protocol both depend on -- and each one is
   stamped with the state that was live when the GAME submitted it, because
   that state is still live. There is no flush point to get wrong.

   WHAT THE GAME SEES. Nothing. tris_in below is the count the game
   assembled; every game-visible counter in this file (the command census,
   the stream hash, the matrix-stack levels GXSTAT publishes, the MTX_STORE
   count) is fed from the command stream, which is untouched. The port
   enforces no polygon-RAM or vertex-RAM limit anywhere -- grep 2048 and 6144
   in this file -- so there is no limit for a smoothed list to overrun.
   =========================================================================== */

/* The sub-triangle sink: project the invented vertices and send them down the
   ordinary path. Only SM64DS_SMOOTH_LIVE=1 reaches it now; smooth_emit_grid
   below is what every other run takes, and the two agree bit for bit. */
void smooth_sink(void *, const SmoothVertex &a, const SmoothVertex &b,
                 const SmoothVertex &c);

/* ONE INVENTED VERTEX. The view-space point goes through project()'s own
   view_to_clip, and the attributes come from the kernel's own
   smooth_grid_attrs at the same barycentric weights: the two pieces of work
   the old sink did, in the same order, on the same numbers. */
void smooth_grid_vertex(const float p[3], float a, float b, float c,
                        const SmoothVertex s[3], GxVertex &o) {
    const Vec4 view{p[0], p[1], p[2], 1.0f};
    const Vec4 clip = view_to_clip(view);
    o.x = clip.x; o.y = clip.y; o.z = clip.z; o.w = clip.w;
    o.vx = view.x; o.vy = view.y; o.vz = view.z; o.vw = view.w;
    SmoothVertex at;
    smooth_grid_attrs(s[0], s[1], s[2], a, b, c, at);
    o.u = at.u; o.v = at.v; o.color = at.color;
    o.nx = at.nx; o.ny = at.ny; o.nz = at.nz;
}

/* EMIT A TESSELLATED TRIANGLE FROM ITS GRID.
   `pts` is smooth_grid_points(tf) positions. With `xform` null they are
   already in view space; with `xform` set they are in the model's own space
   and each one is pushed through that matrix first, which is the replay half
   of the shape store.

   WHY THIS EXISTS separately from the sink above. An interior grid point
   belongs to up to six sub-triangles, and the sink was handed it once per
   sub-triangle, so it was projected up to six times. At level 3 that is 192
   projections to draw 45 distinct vertices. Here each point is projected
   ONCE and the sub-triangles are assembled from an index list. The vertices
   that reach the near clip are the same vertices in the same order, each
   computed by the same arithmetic, so this is not an approximation of the
   old path: it is the old path with the repeats taken out. */
/* THE GRID DEFERRAL (run hunt8, SMOOTHGPU1), declared here and defined after
   the emit it replaces. */
static bool grid_defer_try(const float *pts, long pool_off, int tf,
                           const SmoothVertex s[3], const Mat *xform,
                           const float *box6);
static int g_grid_defer = 0;     // this frame's grids may be kept for the card

/* SMOOTHREND1 (N14, eyes sunk into the model): THE DEPTH STAYS THE ROM'S.
   A curved patch lifts its middle toward the camera, so a flat polygon the artists laid on the
   face (an eye, a pupil, Lakitu's glasses) ends up BEHIND the lifted face and is buried. The
   picture wants the curved shading and silhouette, but the DEPTH of every pixel the DS drew is the
   flat triangle's. So each point of a patch that bulges toward the camera is moved back along its
   own view ray, to where that ray meets the flat source triangle's plane: the screen position,
   the texel and the normal are untouched, only the depth (and the perspective weight that rides
   with it) is the flat one, a hair behind it (kSnapBack).
   The plane is n.x = d from the three view-space corners; `pl` is n, d. Kill switch
   SM64DS_SMOOTH_DEPTHSNAP=0. The card's grid stage does the same, from the same four floats. */
static const float kSnapReach = 0.2f;
static const float kSnapBack = 1.0003f;   /* a hair behind the flat plane, so a flat polygon coplanar with the face (an eye, a screen) and drawn after it wins the tie as it does on the DS */
static bool snap_enabled() {
    static int on = -1;
    if (on < 0) { const char *e = std::getenv("SM64DS_SMOOTH_DEPTHSNAP"); on = (e && e[0] == '0') ? 0 : 1; }
    return on != 0;
}
static bool snap_plane(const SmoothVertex s[3], float pl[4], float *lim) {
    const float e1x = s[1].x - s[0].x, e1y = s[1].y - s[0].y, e1z = s[1].z - s[0].z;
    const float e2x = s[2].x - s[0].x, e2y = s[2].y - s[0].y, e2z = s[2].z - s[0].z;
    pl[0] = e1y * e2z - e1z * e2y; pl[1] = e1z * e2x - e1x * e2z; pl[2] = e1x * e2y - e1y * e2x;
    pl[3] = pl[0] * s[0].x + pl[1] * s[0].y + pl[2] * s[0].z;
    const float l2 = pl[0] * pl[0] + pl[1] * pl[1] + pl[2] * pl[2];
    /* how far a patch can really lift: a fraction of the triangle's longest edge. A ray that meets the plane
       farther than that off the point (a triangle seen nearly edge-on) is not a lift, it is the plane's
       far reach, and the point moves only this far. */
    const float e3x = s[2].x - s[1].x, e3y = s[2].y - s[1].y, e3z = s[2].z - s[1].z;
    float m2 = e1x * e1x + e1y * e1y + e1z * e1z;
    const float b2 = e2x * e2x + e2y * e2y + e2z * e2z, c2 = e3x * e3x + e3y * e3y + e3z * e3z;
    if (b2 > m2) m2 = b2;
    if (c2 > m2) m2 = c2;
    *lim = kSnapReach * std::sqrt(m2);
    return l2 > 1e-24f;
}

static void smooth_emit_grid_cpu(const float *pts, int tf, const SmoothVertex s[3],
                                 const Mat *xform, const GxTriangle *st_in,
                                 const Mat *proj_in, const float *widen_in,
                                 bool count_out) {
    GxVertex v[SMOOTH_MAX_GRID];
    /* view_to_clip's widen, read once for the grid: present_w()/present_h()
       cannot change between two vertices of one triangle's grid, and the
       expression is view_to_clip's own, so every clip.x is the same float.
       A grid kept for the card and turned back into triangles later hands in
       the projection, the widen and the polygon state it was recorded under. */
    const Mat &pj = proj_in ? *proj_in : g.proj;
    const bool persp = pj.m[3] != 0.0f || pj.m[7] != 0.0f ||
                       pj.m[11] != 0.0f;
    const float widen = widen_in ? *widen_in : persp
        ? (4.0f / 3.0f) * ((float)present_h() / (float)present_w()) : 1.0f;
    /* MISC093: a menu-wide frame (a one-screen page in a wide window) places every point through view_to_clip's menu branch,
       not the 4:3 widen; a grid the card kept never comes from such a frame (grid_defer_try refuses them) */
    const bool mwide = menu_wide();
    /* The barycentric weights of every grid point, from the kernel's table:
       the floats the old loop recomputed (ia * (1/tf), 1 - a - b clamped). */
    const SmoothGridTable &T = *smooth_grid_table(tf);
    const int n = T.n;
    /* mul(v, m) is ((v.x*m0 + v.y*m4) + v.z*m8) + v.w*m12 per column; here
       the four columns are the four lanes, with the same multiplies and adds
       in the same order and no fused op, so every float is mul()'s. */
    const __m128 pr0 = _mm_loadu_ps(pj.m), pr1 = _mm_loadu_ps(pj.m + 4),
                 pr2 = _mm_loadu_ps(pj.m + 8), pr3 = _mm_loadu_ps(pj.m + 12);
    __m128 xr0 = _mm_setzero_ps(), xr1 = xr0, xr2 = xr0, xr3 = xr0;
    if (xform) {
        xr0 = _mm_loadu_ps(xform->m); xr1 = _mm_loadu_ps(xform->m + 4);
        xr2 = _mm_loadu_ps(xform->m + 8); xr3 = _mm_loadu_ps(xform->m + 12);
    }
    const __m128 one = _mm_set1_ps(1.0f);
    float pl[4] = {0, 0, 0, 0};
    float snap_lim = 0.0f;
    bool snap_on = persp && snap_enabled() && snap_plane(s, pl, &snap_lim);
    const float pnl = std::sqrt(pl[0] * pl[0] + pl[1] * pl[1] + pl[2] * pl[2]);
    for (int i = 0; i < n; ++i) {
        __m128 vw;                                   // the view-space point, w = 1
        if (xform) {
            const __m128 r = _mm_add_ps(_mm_add_ps(_mm_add_ps(
                _mm_mul_ps(_mm_set1_ps(pts[i * 3]), xr0),
                _mm_mul_ps(_mm_set1_ps(pts[i * 3 + 1]), xr1)),
                _mm_mul_ps(_mm_set1_ps(pts[i * 3 + 2]), xr2)),
                _mm_mul_ps(one, xr3));
            float t[4];
            _mm_storeu_ps(t, r);
            vw = _mm_setr_ps(t[0], t[1], t[2], 1.0f);
        } else {
            vw = _mm_setr_ps(pts[i * 3], pts[i * 3 + 1], pts[i * 3 + 2], 1.0f);
        }
        if (snap_on) {
            /* SMOOTHREND1 (N14 eyes): put this point's depth back on the ROM's own flat triangle (same screen place) */
            float t4[4]; _mm_storeu_ps(t4, vw);
            const float dn = pl[0] * t4[0] + pl[1] * t4[1] + pl[2] * t4[2];
            if (dn > 1e-6f * pnl || dn < -1e-6f * pnl) {
                float k = pl[3] / dn;
                if (k > 0.0f) {
                    const float dist = std::sqrt(t4[0] * t4[0] + t4[1] * t4[1] + t4[2] * t4[2]);
                    const float dk = snap_lim / (dist > 1e-6f ? dist : 1e-6f);
                    if (k > 1.0f + dk) k = 1.0f + dk;
                    if (k < 1.0f - dk) k = 1.0f - dk;
                    k = k * kSnapBack;
                    vw = _mm_setr_ps(t4[0] * k, t4[1] * k, t4[2] * k, 1.0f);
                }
            }
        }
        const __m128 cl = _mm_add_ps(_mm_add_ps(_mm_add_ps(
            _mm_mul_ps(_mm_shuffle_ps(vw, vw, 0x00), pr0),
            _mm_mul_ps(_mm_shuffle_ps(vw, vw, 0x55), pr1)),
            _mm_mul_ps(_mm_shuffle_ps(vw, vw, 0xAA), pr2)),
            _mm_mul_ps(_mm_shuffle_ps(vw, vw, 0xFF), pr3));
        float cv[4], vv[4];
        _mm_storeu_ps(cv, cl);
        _mm_storeu_ps(vv, vw);
        GxVertex &o = v[i];
        o.x = persp ? cv[0] * widen : cv[0];
        o.y = cv[1]; o.z = cv[2]; o.w = cv[3];
        if (mwide) { o.x = cv[0]; menu_wide_place(o.x, o.y, o.w); }
        o.vx = vv[0]; o.vy = vv[1]; o.vz = vv[2]; o.vw = vv[3];
        SmoothVertex at;
        smooth_grid_attrs_inline(s[0], s[1], s[2], T.a[i], T.b[i], T.c[i], at);
        o.u = at.u; o.v = at.v; o.color = at.color;
        o.nx = at.nx; o.ny = at.ny; o.nz = at.nz;
    }
    /* THE BULK. Each grid vertex goes to screen space ONCE (it used to be
       once per sub-triangle that touched it, up to six times), and a
       sub-triangle whose three corners are all in front of the near plane
       needs no clip, so it is assembled straight from those screen vertices:
       the same floats emit_tri_near would have produced for it (it keeps a
       whole triangle's corners untouched and runs to_screen on each), pushed
       with the same state. A sub-triangle with a corner behind the plane
       takes emit_tri_near exactly as before. */
    GxVertex sv[SMOOTH_MAX_GRID];
    unsigned char isin[SMOOTH_MAX_GRID];
    for (int i = 0; i < n; ++i) {
        isin[i] = near_dist(v[i], persp) >= 0.0f;
        if (isin[i]) sv[i] = to_screen(v[i]);
    }
    GxTriangle tmpl{};
    if (st_in) tmpl = *st_in; else fill_tri_state(tmpl);
    GxTriangle t = tmpl;     // the state is the same for every sub-triangle: only the corners change
    const uint16_t *idx = smooth_grid_tri_index(tf);
    const int ntris = smooth_grid_tris(tf);
    uint64_t fast = 0;
    for (int i = 0; i < ntris; ++i) {
        const int i0 = idx[i * 3], i1 = idx[i * 3 + 1], i2 = idx[i * 3 + 2];
        if (isin[i0] && isin[i1] && isin[i2]) {
            t.v[0] = sv[i0]; t.v[1] = sv[i1]; t.v[2] = sv[i2];
            if (t.ghost && g_ip_rec && g_ip_tick_div) ip_tick_ghost_push(t); else g.tris.push_back(t);
            ++fast;
        } else {
            emit_tri_near(v[i0], v[i1], v[i2]);
        }
    }
    if (fast) {
        if (!persp) g_frame_ortho = true;
        if (count_out) smooth_count(SMOOTH_COUNT_OUT, fast);
    }
}

void smooth_emit_grid(const float *pts, int tf, const SmoothVertex s[3],
                      const Mat *xform, long pool_off = -1,
                      const float *box6 = 0) {
    if (g_grid_defer && grid_defer_try(pts, pool_off, tf, s, xform, box6)) return;
    smooth_emit_grid_cpu(pts, tf, s, xform, 0, 0, 0, true);
}

/* ===========================================================================
   SMOOTHED GRIDS KEPT WHOLE FOR THE CARD (run hunt8, lane SMOOTHGPU1)

   WHAT IT REPLACES. smooth_emit_grid_cpu above writes up to 64 sub-triangle
   records (208 bytes each) for every smoothed source triangle, 50,000 of them
   a picture at Smooth 3, and the card path then reads every one of them again
   to build its vertex buffer. When the card is going to draw the whole frame
   (gx_render hands it the list, GxGpuFrame::grids), the grid is instead kept
   as ONE record: the points, the three source corners' attributes, the
   matrices and the polygon state, a few hundred bytes. The card's vertex stage
   evaluates the sub-triangles from it with the same arithmetic.

   THE ORDER IS KEPT. A grid records `at`, the length of the ordinary list when
   it was submitted, so its sub-triangles sit exactly where the processor path
   would have put them. Translucent and shadow polygons, and equal-depth ties,
   depend on that order and nothing else about the list changes.

   WHAT STAYS ON THE PROCESSOR PATH, so that the card never has to guess:
     - a grid with any corner of its model-space box near or behind the near
       plane (the clip changes the triangles; the processor path clips)
     - a shadow-volume polygon (mode 3) and an equal-depth polygon (bit 14),
       which the card's grid stage does not take
     - every frame the card is not going to draw whole (Renderer 0, the A/B, a
       debug probe that reads the polygon list)
   and grids_materialize turns any kept grid into the very triangles the
   processor path makes, by running the same function, whenever something other
   than the card wants the list. That is the proof that nothing was lost: the
   polygon-list hash (SM64DS_SMOOTH_TRIHASH) of a materialized frame equals the
   hash of the frame drawn without any deferral.
   =========================================================================== */
/* SM64DS_SMOOTH_GPU_STATS=1: every 150 frames, how many grids were offered, kept
   and why the rest were not */
static unsigned long long g_gd_try, g_gd_ok, g_gd_state, g_gd_near, g_gd_frames;
static unsigned long long g_gd_mat_calls, g_gd_mat_grids, g_gd_mat_tris;
static double g_gd_mat_ms;
static unsigned long long g_gd_why[8];
static void grid_stats_frame() {
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_SMOOTH_GPU_STATS") ? 1 : 0;
    if (!on) return;
    if ((++g_gd_frames % 150) != 0) return;
    std::fprintf(stderr, "[smoothgpu] frame %llu: grids offered %llu kept %llu "
                 "(polygon state refused %llu, near plane %llu), defer now %d; "
                 "materialized %llu calls %llu grids %llu tris %.2f ms\n",
                 g_gd_frames, g_gd_try, g_gd_ok, g_gd_state, g_gd_near, g_grid_defer,
                 g_gd_mat_calls, g_gd_mat_grids, g_gd_mat_tris, g_gd_mat_ms);
    std::fprintf(stderr, "[smoothgpu] materialize callers: other %llu trihash %llu polygons %llu render-probe %llu render-nogpu %llu groupbox %llu pool-hook %llu pool-hook2 %llu\n",
                 g_gd_why[0], g_gd_why[1], g_gd_why[2], g_gd_why[3], g_gd_why[4], g_gd_why[5], g_gd_why[6], g_gd_why[7]);
}
static bool grid_defer_try(const float *pts, long pool_off, int tf,
                           const SmoothVertex s[3], const Mat *xform,
                           const float *box6) {
    ++g_gd_try;
    if (menu_wide()) { ++g_gd_state; return false; }     /* MISC093: see smooth_emit_grid_cpu */
    const SmoothGridTable *Tp = smooth_grid_table(tf);
    if (!Tp) return false;
    const int n = Tp->n;
    GxTriangle st{};
    fill_tri_state(st);
    if (st.mode == 3 || st.depth_eq) { ++g_gd_state; return false; }
    if (g.grids.size() >= 0x3FFFFFu) return false;

    /* the model-space box of the points */
    /* run hunt8 SMOOTHCPU2: a shape the store holds has its box kept with it
       (min and max are exact, so this is the very floats the loop makes) */
    float lo[3] = {pts[0], pts[1], pts[2]}, hi[3] = {pts[0], pts[1], pts[2]};
    if (box6) {
        for (int k = 0; k < 3; ++k) { lo[k] = box6[k]; hi[k] = box6[3 + k]; }
    } else {
        for (int i = 1; i < n; ++i) {
            const float *q = pts + i * 3;
            for (int k = 0; k < 3; ++k) {
                if (q[k] < lo[k]) lo[k] = q[k];
                if (q[k] > hi[k]) hi[k] = q[k];
            }
        }
    }
    const Mat &pj = g.proj;
    const bool persp = pj.m[3] != 0.0f || pj.m[7] != 0.0f || pj.m[11] != 0.0f;
    const float widen = persp
        ? (4.0f / 3.0f) * ((float)present_h() / (float)present_w()) : 1.0f;
    /* the eight corners of the box through the position matrix and the
       projection: near-plane test and the screen box in one pass. The box of
       a set of points that is wholly in front of the near plane projects to a
       convex region, so the eight corners bound every point and so every
       sub-triangle. A corner within a hair of the plane sends the grid to the
       processor path, which decides each vertex exactly as it always did. */
    float sx0 = 1e30f, sy0 = 1e30f, sx1 = -1e30f, sy1 = -1e30f;
    {
        /* run hunt8 SMOOTHCPU2: the eight corners four to a vector. Every lane
           runs the scalar loop's own operations in its own order (separate
           multiplies and adds, no fused op, an exact divide), so each float is
           the one the scalar loop made; the corner order does not matter to a
           min / max. A corner that fails a test sends the grid to the processor
           path exactly as before (the near count only when persp). */
        float PX[8], PY[8], PZ[8];
        for (int c = 0; c < 8; ++c) {
            PX[c] = (c & 1) ? hi[0] : lo[0];
            PY[c] = (c & 2) ? hi[1] : lo[1];
            PZ[c] = (c & 4) ? hi[2] : lo[2];
        }
        const float *xm = xform ? xform->m : 0;
        const float *pm = pj.m;
#define S4(v) _mm_set1_ps(v)
        const __m128 absmask = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
        for (int h = 0; h < 8; h += 4) {
            __m128 vx = _mm_loadu_ps(PX + h), vy = _mm_loadu_ps(PY + h), vz = _mm_loadu_ps(PZ + h);
            if (xm) {
                const __m128 px = vx, py = vy, pz = vz;
                vx = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(px, S4(xm[0])), _mm_mul_ps(py, S4(xm[4]))), _mm_mul_ps(pz, S4(xm[8]))), S4(xm[12]));
                vy = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(px, S4(xm[1])), _mm_mul_ps(py, S4(xm[5]))), _mm_mul_ps(pz, S4(xm[9]))), S4(xm[13]));
                vz = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(px, S4(xm[2])), _mm_mul_ps(py, S4(xm[6]))), _mm_mul_ps(pz, S4(xm[10]))), S4(xm[14]));
            }
            __m128 cx = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(vx, S4(pm[0])), _mm_mul_ps(vy, S4(pm[4]))), _mm_mul_ps(vz, S4(pm[8]))), S4(pm[12]));
            const __m128 cy = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(vx, S4(pm[1])), _mm_mul_ps(vy, S4(pm[5]))), _mm_mul_ps(vz, S4(pm[9]))), S4(pm[13]));
            const __m128 cz = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(vx, S4(pm[2])), _mm_mul_ps(vy, S4(pm[6]))), _mm_mul_ps(vz, S4(pm[10]))), S4(pm[14]));
            const __m128 cw = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(vx, S4(pm[3])), _mm_mul_ps(vy, S4(pm[7]))), _mm_mul_ps(vz, S4(pm[11]))), S4(pm[15]));
            if (persp) cx = _mm_mul_ps(cx, S4(widen));
            if (persp) {
                const __m128 nd = _mm_add_ps(cz, cw);
                const __m128 thr = _mm_mul_ps(S4(1e-4f), _mm_add_ps(_mm_and_ps(cz, absmask), _mm_and_ps(cw, absmask)));
                const int ok = _mm_movemask_ps(_mm_and_ps(_mm_cmpge_ps(nd, thr), _mm_cmpgt_ps(cw, S4(1e-3f))));
                if (ok != 0xF) {
                    ++g_gd_near;
                    return false;
                }
            } else {
                const int ok = _mm_movemask_ps(_mm_and_ps(_mm_cmpge_ps(_mm_sub_ps(cw, S4(1e-3f)), _mm_setzero_ps()), _mm_cmpgt_ps(cw, S4(1e-3f))));
                if (ok != 0xF) return false;
            }
            const __m128 iw = _mm_div_ps(S4(1.0f), cw);
            const __m128 vsx = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(_mm_add_ps(_mm_mul_ps(cx, iw), S4(1.0f)), S4(0.5f)), S4((float)g.vp_w)), S4((float)g.vp_x));
            const __m128 vsy = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(S4(1.0f), _mm_mul_ps(_mm_add_ps(_mm_mul_ps(cy, iw), S4(1.0f)), S4(0.5f))), S4((float)g.vp_h)), S4((float)g.vp_y));
            const __m128 inr = _mm_and_ps(_mm_and_ps(_mm_cmpgt_ps(vsx, S4(-1e7f)), _mm_cmplt_ps(vsx, S4(1e7f))),
                                          _mm_and_ps(_mm_cmpgt_ps(vsy, S4(-1e7f)), _mm_cmplt_ps(vsy, S4(1e7f))));
            if (_mm_movemask_ps(inr) != 0xF) return false;
            float ax[4], ay[4];
            _mm_storeu_ps(ax, vsx);
            _mm_storeu_ps(ay, vsy);
            for (int k = 0; k < 4; ++k) {
                if (ax[k] < sx0) sx0 = ax[k];
                if (ax[k] > sx1) sx1 = ax[k];
                if (ay[k] < sy0) sy0 = ay[k];
                if (ay[k] > sy1) sy1 = ay[k];
            }
        }
#undef S4
    }

    g.grids.emplace_back();
    GxGrid &gr = g.grids.back();
    gr.st = st;
    static_assert(sizeof(GxGridCorner) == sizeof(SmoothVertex), "corner layout");
    std::memcpy(gr.s, s, sizeof gr.s);
    if (xform) {
        std::memcpy(gr.xform, xform->m, sizeof gr.xform);
        gr.has_xform = 1;
    } else {
        for (int i = 0; i < 16; ++i) gr.xform[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        gr.has_xform = 0;
    }
    std::memcpy(gr.proj, pj.m, sizeof gr.proj);
    gr.widen = widen;
    gr.vp[0] = g.vp_x; gr.vp[1] = g.vp_y; gr.vp[2] = g.vp_w; gr.vp[3] = g.vp_h;
    for (int k = 0; k < 3; ++k) { gr.lo[k] = lo[k]; gr.hi[k] = hi[k]; }
    gr.sbox[0] = sx0; gr.sbox[1] = sy0; gr.sbox[2] = sx1; gr.sbox[3] = sy1;
    gr.at = (uint32_t)g.tris.size();
    gr.tf = (uint8_t)tf;
    gr.persp = persp ? 1 : 0;
    gr.depth_snap = (persp && snap_enabled() && snap_plane(s, gr.pl, &gr.snap_lim)) ? 1 : 0;
    if (pool_off >= 0) {
        gr.from_pool = 1;
        gr.off = (uint32_t)pool_off;
        gr.pool_gen = smooth_store_generation();
    } else {
        gr.from_pool = 0;
        gr.off = (uint32_t)g.grid_pts.size();
        gr.pool_gen = 0;
        g.grid_pts.insert(g.grid_pts.end(), pts, pts + (size_t)n * 3);
    }
    const int ntris = smooth_grid_tris(tf);
    g.grid_subtris += (uint32_t)ntris;
    if (st.translucent) g.grid_translucent = 1;
    if (st.mode == 2) g.grid_mode2 += (uint32_t)ntris;
    if (!persp) g_frame_ortho = true;
    smooth_count(SMOOTH_COUNT_OUT, (uint64_t)ntris);
    ++g_gd_ok;
    return true;
}

/* Turn every kept grid of the live state into the triangles the processor path
   makes, in place in the list. A no-op when there are none. */
static void grids_materialize_w(int why);
static void grids_materialize() { grids_materialize_w(0); }
static void grids_materialize_w(int why) {
    if (g.grids.empty()) return;
    const auto gd_t0 = std::chrono::steady_clock::now();
    ++g_gd_why[why & 7];
    ++g_gd_mat_calls; g_gd_mat_grids += g.grids.size(); g_gd_mat_tris += g.tris.size();
    std::vector<GxTriangle> src = std::move(g.tris);
    g.tris.clear();
    g.tris.reserve(src.size() + g.grid_subtris);
    const Mat proj_save = g.proj;
    const int vp_save[4] = {g.vp_x, g.vp_y, g.vp_w, g.vp_h};
    size_t at = 0;
    size_t npool = 0;
    const float *pool = smooth_store_pool(&npool);
    const uint32_t gen = smooth_store_generation();
    for (const GxGrid &gr : g.grids) {
        if (gr.at > at) {
            g.tris.insert(g.tris.end(), src.begin() + at, src.begin() + gr.at);
            at = gr.at;
        }
        const float *pts = 0;
        if (gr.from_pool) {
            if (gr.pool_gen == gen && pool && gr.off < npool) pts = pool + gr.off;
        } else if (gr.off < g.grid_pts.size()) {
            pts = g.grid_pts.data() + gr.off;
        }
        if (!pts) continue;      // a pool that went away: the clear hook runs first, so never
        std::memcpy(g.proj.m, gr.proj, sizeof gr.proj);
        g.vp_x = gr.vp[0]; g.vp_y = gr.vp[1]; g.vp_w = gr.vp[2]; g.vp_h = gr.vp[3];
        SmoothVertex sv[3];
        std::memcpy(sv, gr.s, sizeof sv);
        Mat xf;
        if (gr.has_xform) std::memcpy(xf.m, gr.xform, sizeof xf.m);
        smooth_emit_grid_cpu(pts, gr.tf, sv, gr.has_xform ? &xf : 0, &gr.st,
                             &g.proj, &gr.widen, false);
    }
    if (src.size() > at)
        g.tris.insert(g.tris.end(), src.begin() + at, src.end());
    g.proj = proj_save;
    g.vp_x = vp_save[0]; g.vp_y = vp_save[1]; g.vp_w = vp_save[2]; g.vp_h = vp_save[3];
    g.grids.clear();
    g.grid_pts.clear();
    g.grid_subtris = 0;
    g.grid_translucent = 0;
    g.grid_mode2 = 0;
    g_gd_mat_ms += std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - gd_t0).count();
}

/* The NORMAL command's 30-bit payload back out as three floats, decoded
   exactly as the NORMAL case decodes it (three 10-bit signed fields, 1.9
   fixed point) and before any matrix touches it. */
void raw_normal(uint32_t n, float &nx, float &ny, float &nz) {
    const int32_t a = static_cast<int32_t>(n << 22) >> 22;
    const int32_t b = static_cast<int32_t>((n >> 10) << 22) >> 22;
    const int32_t c = static_cast<int32_t>((n >> 20) << 22) >> 22;
    nx = a / 512.0f; ny = b / 512.0f; nz = c / 512.0f;
}

void smooth_counters_for(int why) {
    switch (why) {
        case SMOOTH_WHY_NO_NORMAL: smooth_count(SMOOTH_COUNT_NO_NORMAL, 1); break;
        case SMOOTH_WHY_FLAT:      smooth_count(SMOOTH_COUNT_FLAT, 1); break;
        case SMOOTH_WHY_EDGE:      smooth_count(SMOOTH_COUNT_EDGE, 1); break;
        case SMOOTH_WHY_RADIUS:    smooth_count(SMOOTH_COUNT_RADIUS, 1); break;
        default: break;
    }
}

/* CAN THIS TRIANGLE'S SHAPE BE STORED? Every condition is about whether a
   single local space exists for the three corners and whether the matrix
   carries the patch faithfully; the store's own header has the algebra. A
   refusal is counted and the triangle takes the ordinary path unchanged. */
bool store_eligible(const GxRaw &ra, const GxRaw &rb, const GxRaw &rc) {
    /* No authored normal on a corner: the policy refuses it in three
       compares without a square root, so there is nothing worth keeping. */
    if (!ra.has_normal || !rb.has_normal || !rc.has_normal) return false;
    /* A zero payload is the one normal whose live value is NOT the rotation
       of its raw value: the NORMAL case turns it into (0,0,1) rather than
       leaving it zero, so a local patch built from it would curve towards a
       different surface. */
    if (!ra.nrm || !rb.nrm || !rc.nrm) {
        smooth_store_count(SMOOTH_STORE_ZERONRM, 1);
        return false;
    }
    /* Corners from different matrices have no shared local space at all.
       That is the bone joint, and it is why the live path stays. */
    if (ra.mgen != rb.mgen || rb.mgen != rc.mgen) {
        smooth_store_count(SMOOTH_STORE_CROSSMTX, 1);
        smooth_store_count(SMOOTH_STORE_CROSS_CORNER, 1);
        return false;
    }
    /* A normal latched before the matrix moved is in the wrong space for the
       position beside it. Counted apart from the joint above, because the two
       want different answers: this one could be fixed by keeping the raw
       normal per vertex rather than as a latch, the one above cannot. */
    if (ra.ngen != ra.mgen || rb.ngen != rb.mgen || rc.ngen != rc.mgen) {
        smooth_store_count(SMOOTH_STORE_CROSSMTX, 1);
        smooth_store_count(SMOOTH_STORE_CROSS_NORMAL, 1);
        return false;
    }
    /* The matrix those corners rode has to still be in the ring AND have
       passed the similarity check. mtx_for_gen answers both, and the replay
       uses the matrix it hands back rather than whatever is live now. */
    if (!mtx_for_gen(rc.mgen)) {
        smooth_store_count(SMOOTH_STORE_CROSSMTX, 1);
        smooth_store_count(SMOOTH_STORE_CROSS_STALE, 1);
        return false;
    }
    return true;
}

/* THE STORED PATH. The verdict has already been taken, LIVE, by the caller:
   all this does is find the tessellation for a shape that has been seen
   before, or build it once if it has not, and replay it through the matrix
   the three corners rode. Returns 0 when it could not, and the caller falls
   through to building the grid in view space. */
int smooth_try_store(const SmoothVertex s[3], const GxRaw &ra, const GxRaw &rb,
                     const GxRaw &rc, int tf, const SmoothPolicy &pol,
                     int prof, const SmoothOverride *ov = 0, uint32_t keyx = 0) {
    const Mat *xform = mtx_for_gen(rc.mgen);
    if (!xform) {
        smooth_store_count(SMOOTH_STORE_NONSIM, 1);
        return 0;
    }

    /* ZEROED FIRST. The store hashes the key's bytes, padding included (two
       bytes between the int16 positions and the normals), and a key built on
       the stack over whatever the last triangle left there hashed the same
       shape to different slots: on Bob-omb Battlefield that turned 1,200
       distinct shapes into 14,000 misses. The compare is on the fields, so
       nothing was ever replayed wrongly; it was only ever found by luck. */
    SmoothKey key;
    std::memset(&key, 0, sizeof key);
    const GxRaw *r[3] = {&ra, &rb, &rc};
    for (int i = 0; i < 3; ++i) {
        key.p[i][0] = r[i]->x; key.p[i][1] = r[i]->y; key.p[i][2] = r[i]->z;
        key.n[i] = r[i]->nrm;
    }
    /* A patch built from the topology book's edge normals is a different
       patch from the one the authored normals make for the same corners. */
    key.level = static_cast<uint32_t>(pol.level) | (ov ? 0x100u : 0u) | keyx;

    const long long t_sub = prof ? smooth_prof_ticks() : 0;
    const long long t_fd = prof ? smooth_prof_ticks() : 0;
    const SmoothEntry *e = smooth_store_find(key);
    if (prof) smooth_prof_add(SMOOTH_PROF_FIND, smooth_prof_ticks() - t_fd, 1);
    float pts[SMOOTH_MAX_GRID * 3];
    const float *grid;
    if (!e) {
        /* THE ONCE. Build the corners in the model's own space out of the
           display list's own numbers -- the same FX12 scaling project() puts
           on a VTX coordinate, and the raw NORMAL payload -- and tessellate.
           Texel coordinates and colour are deliberately left at zero: they
           are live state, and the replay interpolates the current ones at
           the same barycentric weights. */
        SmoothVertex l[3];
        for (int i = 0; i < 3; ++i) {
            l[i].x = r[i]->x * FX12;
            l[i].y = r[i]->y * FX12;
            l[i].z = r[i]->z * FX12;
            raw_normal(r[i]->nrm, l[i].nx, l[i].ny, l[i].nz);
            l[i].u = 0.0f; l[i].v = 0.0f; l[i].color = 0;
        }
        if (ov) smooth_grid_positions_edges(l[0], l[1], l[2], ov->en, tf, pts);
        else smooth_grid_positions(l[0], l[1], l[2], tf, pts);
        /* KEPT ON THE SECOND SIGHTING (ntr/smooth.h, smooth_store_miss). The
           goo painting's vertices move every frame, so its keys are new
           every picture and a grid kept for them is never asked for again;
           a static shape is asked for again on the next picture and is kept
           then. Either way THIS picture draws from the grid just built, the
           same floats the pool would hold, so what reaches the raster does
           not depend on whether the grid was kept. */
        if (smooth_store_miss(key)) {
            e = smooth_store_add(key, tf, pts);
            if (!e) return 0;
            grid = smooth_store_grid(e);
        } else {
            grid = pts;
        }
    } else {
        if (e->tf != tf) return 0;     /* built at another level: rebuild live */
        grid = smooth_store_grid(e);
    }
    if (!grid) return 0;

    if (smooth_abdiff_on() && !ov) {
        /* The audit arm: build the patch the old way as well and measure how
           far the two land apart. In exact arithmetic they are the same
           patch; this is the number that says how close the machine gets. */
        float liveg[SMOOTH_MAX_GRID * 3];
        smooth_grid_positions(s[0], s[1], s[2], tf, liveg);
        const int npts = smooth_grid_points(tf);
        float worst = 0.0f;
        for (int i = 0; i < npts; ++i) {
            const Vec4 lv{grid[i * 3], grid[i * 3 + 1], grid[i * 3 + 2], 1.0f};
            const Vec4 w = mul(lv, *xform);
            const float dx = w.x - liveg[i * 3];
            const float dy = w.y - liveg[i * 3 + 1];
            const float dz = w.z - liveg[i * 3 + 2];
            const float d = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (d > worst) worst = d;
        }
        float e0[3] = {s[1].x - s[0].x, s[1].y - s[0].y, s[1].z - s[0].z};
        smooth_abdiff_add(0, worst,
                          std::sqrt(e0[0]*e0[0] + e0[1]*e0[1] + e0[2]*e0[2]));
    }

    const long long t_em = prof ? smooth_prof_ticks() : 0;
    smooth_emit_grid(grid, tf, s, xform,
                     grid == pts ? -1L : (long)smooth_store_entry_off(e),
                     grid == pts ? 0 : smooth_store_entry_box(e));
    if (prof) smooth_prof_add(SMOOTH_PROF_EMIT, smooth_prof_ticks() - t_em, 1);
    if (prof) smooth_prof_add(SMOOTH_PROF_SUBDIV,
                              smooth_prof_ticks() - t_sub, 1);
    return 1;
}

/* THE CRACK BOOK'S NOTE OF WHAT smooth_try DID (SM64DS_SMOOTH_CRACKS, ntr/smooth.h).
   Written only while the book is on; emit_tri hands it over with the corners. */
struct CkNote { int curved, why, book, tf; float en[6][3]; };
CkNote g_ck;
inline void ck_straight(int why) { g_ck.curved = 0; g_ck.why = why; }

/* THE WELD BOOK's normals (ntr/smooth.h) into view space: each slot by the
   matrix its own corner rode, so a seam stitched across two bones turns each
   end with that end's bone. A zero slot is a straight edge and stays zero.
   The ring's matrix is used whether or not it passed the similarity check (a
   SmoothMotion in-between loads blended matrices, which are only nearly
   similar; the result is normalised either way, and both sides of a seam turn
   a shared corner by the same matrix). A corner whose matrix has aged out of
   the ring falls back to its own view-space normal, so the triangle is never
   drawn unwelded because of it. */
bool weld_view_en(const float wo[6][3], const GxRaw *const r[3], const SmoothVertex sv[3], float en[6][3]) {
    static const int corner[6] = {0, 1, 1, 2, 0, 2};
    /* A slot the weld left as the corner's own normal is turned EXACTLY as
       the unwelded path turns it (the corner's own view-space normal, from the
       vector matrix), so a side that kept an edge and the side across it that
       was never welded build the same curve even where the position matrix is
       not a similarity (a menu's wide zoom). Only a slot the weld changed goes
       through the ring. Each edge is decided as a pair: if either end of a
       changed edge has no matrix in the ring, the whole edge falls back to its
       own normals. */
    float own[3][3];
    for (int c = 0; c < 3; ++c) {
        const int32_t a = static_cast<int32_t>(r[c]->nrm << 22) >> 22;
        const int32_t bb = static_cast<int32_t>((r[c]->nrm >> 10) << 22) >> 22;
        const int32_t d = static_cast<int32_t>((r[c]->nrm >> 20) << 22) >> 22;
        const float v[3] = {a / 512.0f, bb / 512.0f, d / 512.0f};
        const float l = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        const float il = l > 1e-12f ? 1.0f / l : 0.0f;
        own[c][0] = v[0] * il; own[c][1] = v[1] * il; own[c][2] = v[2] * il;
    }
    bool keep[6];
    for (int i = 0; i < 6; ++i) {
        const float *o = own[corner[i]];
        keep[i] = wo[i][0] == o[0] && wo[i][1] == o[1] && wo[i][2] == o[2];
    }
    for (int e = 0; e < 3; ++e) {
        const int i0 = e * 2, i1 = e * 2 + 1;
        const bool z0 = wo[i0][0] == 0.0f && wo[i0][1] == 0.0f && wo[i0][2] == 0.0f;
        const bool z1 = wo[i1][0] == 0.0f && wo[i1][1] == 0.0f && wo[i1][2] == 0.0f;
        if (z0 && z1) {                       /* a straight edge needs no matrix */
            for (int q = 0; q < 3; ++q) en[i0][q] = en[i1][q] = 0.0f;
            continue;
        }
        bool ring_ok = true;
        for (int k = 0; k < 2; ++k) {
            const int i = k ? i1 : i0;
            if (keep[i]) continue;
            const uint32_t gen = r[corner[i]]->mgen;
            if (g_mtx_ring_gen[gen & (MTX_RING - 1)] != gen) ring_ok = false;
        }
        for (int k = 0; k < 2; ++k) {
            const int i = k ? i1 : i0;
            Vec4 v;
            if (keep[i] || !ring_ok) {
                const SmoothVertex &c = sv[corner[i]];
                v = Vec4{c.nx, c.ny, c.nz, 0.0f};
            } else {
                const uint32_t gen = r[corner[i]]->mgen;
                v = mul(Vec4{wo[i][0], wo[i][1], wo[i][2], 0.0f}, g_mtx_ring[gen & (MTX_RING - 1)]);
            }
            /* unit_normal's own arithmetic (smooth.cpp), so a kept slot is the
               same float the unwelded patch builds from */
            const float l2 = v.x * v.x + v.y * v.y + v.z * v.z;
            if (l2 <= 1e-16f) return false;
            const float inv = 1.0f / sqrtf(l2);
            en[i][0] = v.x * inv; en[i][1] = v.y * inv; en[i][2] = v.z * inv;
        }
    }
    return true;
}
/* The store key's share of a weld: its id in the bits above the level and
   the book's flag, so a triangle whose edges are welded keeps its own grid. */
inline uint32_t weld_keyx(uint32_t id) { return 0x200u | (id << 12); }

/* THE WELD'S ANSWERS IN SEQUENCE. The game submits its triangles in the same
   order tick after tick (the same display lists, walked the same way), and a
   SmoothMotion replay submits the tick's own triangles again in that order.
   So each answer is kept in submission order with a check of the triangle it
   was for and the book's epoch it was given at: a replay reads the tick's
   answers back, and a tick reads the previous tick's, both in sequence, which
   costs a sequential read instead of a lookup. An answer is reused only while
   the book has not changed since it was given (the epoch) and the check
   matches; a stream that does not line up looks a few places ahead, then asks
   the book. */
struct WeldMemo { uint32_t check; uint32_t epoch; int result; uint32_t id; int out; };
std::vector<WeldMemo> g_wm, g_wm_prev;
std::vector<float> g_wm_out, g_wm_prev_out;   // 18 floats for each answer that changed something
uint64_t g_wm_frame = ~0ull;
size_t g_wm_cursor = 0;          // into g_wm during a replay
size_t g_wm_prev_cursor = 0;     // into g_wm_prev during a tick
inline uint32_t weld_check(const int16_t p[3][3], const uint32_t n[3], int kind, const float (*en)[3]) {
    const uint32_t *w = (const uint32_t *)&p[0][0];
    uint32_t h = 0x811C9DC5u ^ (uint32_t)kind;
    for (int i = 0; i < 4; ++i) h = (h ^ w[i]) * 0x01000193u;
    h = (h ^ (uint16_t)p[2][2]) * 0x01000193u;
    for (int i = 0; i < 3; ++i) h = (h ^ n[i]) * 0x01000193u;
    if (en) {
        const uint32_t *e = (const uint32_t *)&en[0][0];
        for (int i = 0; i < 18; ++i) h = (h ^ e[i]) * 0x01000193u;
    }
    return h;
}
inline int weld_memo_use(const WeldMemo &m, const std::vector<float> &outs, float wo[6][3], uint32_t *wid) {
    if (m.result) {
        std::memcpy(wo, &outs[(size_t)m.out * 18], sizeof(float) * 18);
        *wid = m.id;
    }
    return m.result;
}
int weld_call(const int16_t p[3][3], const uint32_t n[3], int kind, const float (*en)[3],
              float wo[6][3], uint32_t *wid) {
    const uint32_t chk = weld_check(p, n, kind, en);
    const uint32_t ep = smooth_weld_epoch();
    if (g_ip_replaying) {
        if (g_wm_cursor < g_wm.size() && g_wm[g_wm_cursor].check == chk && g_wm[g_wm_cursor].epoch == ep) {
            smooth_weld_memo_count(1);
            return weld_memo_use(g_wm[g_wm_cursor++], g_wm_out, wo, wid);
        }
        smooth_weld_memo_count(0);
        return smooth_weld(p, n, kind, en, wo, wid);
    }
    const uint64_t f = smooth_frame_no();
    if (f != g_wm_frame) {
        g_wm_frame = f;
        g_wm_prev.swap(g_wm); g_wm_prev_out.swap(g_wm_out);
        g_wm.clear(); g_wm_out.clear();
        g_wm_prev_cursor = 0;
    }
    int r = -1;
    for (size_t look = 0; look < 8 && g_wm_prev_cursor + look < g_wm_prev.size(); ++look) {
        const WeldMemo &m = g_wm_prev[g_wm_prev_cursor + look];
        if (m.check != chk) continue;
        g_wm_prev_cursor += look + 1;
        if (m.epoch == ep) {
            smooth_weld_memo_count(1);
            r = weld_memo_use(m, g_wm_prev_out, wo, wid);
        }
        break;
    }
    if (r < 0) {
        smooth_weld_memo_count(0);
        r = smooth_weld(p, n, kind, en, wo, wid);
    }
    WeldMemo m;
    m.check = chk; m.epoch = smooth_weld_epoch(); m.result = r; m.id = r ? *wid : 0; m.out = -1;
    if (r) {
        m.out = (int)(g_wm_out.size() / 18);
        g_wm_out.insert(g_wm_out.end(), &wo[0][0], &wo[0][0] + 18);
    }
    g_wm.push_back(m);
    return r;
}

int smooth_try(const GxVertex &a, const GxVertex &b, const GxVertex &c,
               const GxRaw &ra, const GxRaw &rb, const GxRaw &rc) {
    const SmoothPolicy &pol = smooth_policy();
    g_ck.curved = 0; g_ck.why = -1; g_ck.book = 0; g_ck.tf = 1;
    const long long t_pre = smooth_prof_on() ? smooth_prof_ticks() : 0;

    /* A mode-3 polygon is a shadow volume. Its stencil protocol compares the
       mask pass against the draw pass PIXEL FOR PIXEL, so both have to be the
       same geometry; a curved shadow volume would not close. */
    if (((g.poly_attr >> 4) & 3) == 3) {
        smooth_count(SMOOTH_COUNT_MODE3, 1);
        ck_straight(SMOOTH_CKWHY_MODE3);
        return 0;
    }
    /* A constant-w projection is an ortho one: the HUD's own 3D geometry and
       every 2D framing trick in the port. No view volume, no surface. */
    if (g.proj.m[3] == 0.0f && g.proj.m[7] == 0.0f && g.proj.m[11] == 0.0f) {
        smooth_count(SMOOTH_COUNT_ORTHO, 1);
        ck_straight(SMOOTH_CKWHY_ORTHO);
        return 0;
    }
    /* A non-affine position matrix would make the view-space positions below
       projective, and a patch built in projective coordinates is not the
       patch anyone meant. No matrix the game loads is like this; the guard is
       here so that stays a measured fact rather than an assumption. */
    if (a.vw != 1.0f || b.vw != 1.0f || c.vw != 1.0f) {
        smooth_count(SMOOTH_COUNT_W, 1);
        ck_straight(SMOOTH_CKWHY_W);
        return 0;
    }

    SmoothVertex s[3];
    const GxVertex *in[3] = {&a, &b, &c};
    for (int i = 0; i < 3; ++i) {
        s[i].x = in[i]->vx; s[i].y = in[i]->vy; s[i].z = in[i]->vz;
        s[i].nx = in[i]->nx; s[i].ny = in[i]->ny; s[i].nz = in[i]->nz;
        s[i].u = in[i]->u; s[i].v = in[i]->v;
        s[i].color = in[i]->color;
    }

    /* THE WELD BOOK sees every triangle that gets this far, curved or not:
       a straight side is what tells its curved neighbour to straighten the
       edge they share. */
    const int weld = smooth_weld_on();
    const int16_t wp[3][3] = {{ra.x, ra.y, ra.z}, {rb.x, rb.y, rb.z},
                              {rc.x, rc.y, rc.z}};
    const uint32_t wn[3] = {ra.nrm, rb.nrm, rc.nrm};
    const GxRaw *const wr[3] = {&ra, &rb, &rc};
    float wo[6][3];
    uint32_t wid = 0;

    /* STEP-0 PROFILER (SM64DS_SMOOTH_PROF; ntr/smooth.h has the contract).
       Off, this is one compare on a cached int inside a path that only runs
       when SmoothModels is on. */
    const int prof = smooth_prof_on();

    /* THE VERDICT IS ALWAYS LIVE, and it is 0.4.0's own call on 0.4.0's own
       inputs: the view-space corners, this frame. Nothing about which
       triangles get smoothed is remembered, for the reason written out over
       SmoothEntry in ntr/smooth.h. Say WHY, so the measurement table can
       separate "the feature did nothing because the scene is flat" from "the
       caps are too tight". */
    int why = SMOOTH_WHY_OK;
    const long long t_pol = prof ? smooth_prof_ticks() : 0;
    if (prof) smooth_prof_add(SMOOTH_PROF_PRE, t_pol - t_pre, 1);
    /* run hunt8 SMOOTHCPU2: THREE CORNERS THAT CARRY ONE RAW NORMAL UNDER ONE
       MATRIX ARE FLAT, and the policy need not be asked. The three view-space
       normals are the same floats (same payload, same vector matrix), so the
       three unit normals are the same floats and their dot products are the
       sum of squares of one unit vector, 1 to within a few ulp, which is above
       flat_cos by a margin no rounding reaches. smooth_shape returns the same
       "flat" for that triangle after three square roots and three divides; the
       one test kept is the length floor it applies first (a normal too short
       to normalise is a no-normal corner, not a flat one). The verify arm
       (SM64DS_SMOOTH_FASTFLAT_CHECK=1) asks both and counts disagreements. */
    int tf;
    {
        static int chk = -1;
        if (chk < 0) chk = std::getenv("SM64DS_SMOOTH_FASTFLAT_CHECK") ? 1 : 0;
        const float l2 = s[0].nx * s[0].nx + s[0].ny * s[0].ny + s[0].nz * s[0].nz;
        const bool fast = pol.level > 0 && ra.has_normal && rb.has_normal && rc.has_normal &&
                          ra.nrm && ra.nrm == rb.nrm && rb.nrm == rc.nrm &&
                          ra.ngen == rb.ngen && rb.ngen == rc.ngen && l2 > 1e-16f;
        if (fast && !chk) {
            tf = 1;
            why = SMOOTH_WHY_FLAT;
        } else {
            tf = smooth_tess_factor(s[0], s[1], s[2], pol, &why);
            if (fast && chk && (tf != 1 || why != SMOOTH_WHY_FLAT)) {
                static unsigned bad = 0;
                if (++bad <= 20)
                    std::fprintf(stderr, "[fastflat] DISAGREE tf %d why %d\n", tf, why);
            }
        }
    }
    if (prof) smooth_prof_add(SMOOTH_PROF_POLICY,
                              smooth_prof_ticks() - t_pol, 1);
    if (smooth_census_on()) smooth_census_tri(s[0], s[1], s[2], tf);

    /* THE TOPOLOGY BOOK (ntr/smooth.h): is this triangle part of a hard-edged
       box, a faceted solid or a flat polygon fan? Every lit triangle whose
       corners share one matrix is entered (the book needs the whole mesh to
       judge any of it); only one the plain verdict left alone for a shape
       reason, or smoothed, is overridden. A triangle the size caps refused
       stays refused. */
    if (smooth_topo_on() && why != SMOOTH_WHY_NO_NORMAL &&
        ra.has_normal && rb.has_normal && rc.has_normal &&
        ra.nrm && rb.nrm && rc.nrm &&
        ra.mgen == rb.mgen && rb.mgen == rc.mgen) {
        const int16_t tp[3][3] = {{ra.x, ra.y, ra.z}, {rb.x, rb.y, rb.z},
                                  {rc.x, rc.y, rc.z}};
        const uint32_t tn[3] = {ra.nrm, rb.nrm, rc.nrm};
        SmoothOverride ov;
        const long long t_tp = prof ? smooth_prof_ticks() : 0;
        const int tp_hit = smooth_topo_lookup(tp, tn, rc.mgen, &ov);
        if (prof) smooth_prof_add(SMOOTH_PROF_TOPO, smooth_prof_ticks() - t_tp, 1);
        if (tp_hit &&
            why != SMOOTH_WHY_EDGE && why != SMOOTH_WHY_RADIUS) {
            if (ov.cls == SMOOTH_CLASS_FACET || ov.flat) {
                /* Left as the DS drew it. */
                if (tf > 1) smooth_topo_count(ov.cls == SMOOTH_CLASS_FACET
                                              ? SMOOTH_TOPO_FACET : SMOOTH_TOPO_FLATKEEP);
                ck_straight(ov.cls == SMOOTH_CLASS_FACET ? SMOOTH_CKWHY_FACET : SMOOTH_CKWHY_BOOKFLAT);
                if (weld) weld_call(wp, wn, SMOOTH_WELD_STRAIGHT, 0, wo, &wid);
                return 0;
            }
            int level = pol.level;
            if (level > SMOOTH_MAX_LEVEL) level = SMOOTH_MAX_LEVEL;
            const int otf = 1 << level;
            if (otf <= 1) {
                if (weld) weld_call(wp, wn, SMOOTH_WELD_STRAIGHT, 0, wo, &wid);
                return 0;
            }
            smooth_count(SMOOTH_COUNT_SUBDIVIDED, 1);
            if (!g_ip_replaying) smooth_count(SMOOTH_COUNT_DEMAND, 1);
            smooth_topo_count(ov.cls == SMOOTH_CLASS_FAN ? SMOOTH_TOPO_FAN : SMOOTH_TOPO_BOX);
            uint32_t okeyx = 0;
            if (weld && weld_call(wp, wn, SMOOTH_WELD_CURVED, ov.en, wo, &wid) && weld == 1 && !smooth_weld_guarded()) {
                std::memcpy(ov.en, wo, sizeof ov.en);
                okeyx = weld_keyx(wid);
            }
            if (smooth_cracks_on()) {
                const Mat *cm = mtx_for_gen(rc.mgen);
                g_ck.curved = 1; g_ck.book = 1; g_ck.tf = otf;
                for (int i = 0; i < 6; ++i) {
                    if (!cm) { g_ck.en[i][0] = g_ck.en[i][1] = g_ck.en[i][2] = 0.0f; continue; }
                    const Vec4 v = mul(Vec4{ov.en[i][0], ov.en[i][1], ov.en[i][2], 0.0f}, *cm);
                    const float l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
                    const float il = l > 1e-20f ? 1.0f / l : 0.0f;
                    g_ck.en[i][0] = v.x * il; g_ck.en[i][1] = v.y * il; g_ck.en[i][2] = v.z * il;
                }
            }
            if (!smooth_live_mode() && store_eligible(ra, rb, rc) &&
                smooth_try_store(s, ra, rb, rc, otf, pol, prof, &ov, okeyx))
                return 1;
            /* The stored path would not take it (a normal latched under an
               older matrix, a matrix that has aged out of the ring): build
               the patch in view space, turning the book's model-space edge
               normals by the matrix each corner rode. */
            /* SMOOTHWELD1: an in-between's blended matrix is only nearly a
               similarity; the ring's matrix serves here as it does for the
               weld, so a box model is not drawn flat in the in-betweens only. */
            const Mat *mm = mtx_for_gen(rc.mgen);
            if (!mm && weld && !smooth_weld_guarded()) {
                const int ms = static_cast<int>(rc.mgen & (MTX_RING - 1));
                if (g_mtx_ring_gen[ms] == rc.mgen) mm = &g_mtx_ring[ms];
            }
            if (!mm) {
                if (weld) weld_call(wp, wn, SMOOTH_WELD_STRAIGHT, 0, wo, &wid);
                return 0;
            }
            float en[6][3];
            for (int i = 0; i < 6; ++i) {
                if (okeyx && ov.en[i][0] == 0.0f && ov.en[i][1] == 0.0f && ov.en[i][2] == 0.0f) {
                    en[i][0] = en[i][1] = en[i][2] = 0.0f;     /* a straight edge */
                    continue;
                }
                const Vec4 v = mul(Vec4{ov.en[i][0], ov.en[i][1], ov.en[i][2], 0.0f}, *mm);
                const float l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
                if (l <= 1e-20f) return 0;
                en[i][0] = v.x / l; en[i][1] = v.y / l; en[i][2] = v.z / l;
            }
            float pts[SMOOTH_MAX_GRID * 3];
            smooth_grid_positions_edges(s[0], s[1], s[2], en, otf, pts);
            smooth_emit_grid(pts, otf, s, 0);
            return 1;
        }
    }
    /* A seam between two bones: the book cannot hold it, but beside a BOX
       mesh it belongs to, it is left as the DS drew it. */
    if (tf > 1 && smooth_topo_on() && ra.has_normal && rb.has_normal && rc.has_normal &&
        !(ra.mgen == rb.mgen && rb.mgen == rc.mgen)) {
        const int16_t tp[3][3] = {{ra.x, ra.y, ra.z}, {rb.x, rb.y, rb.z},
                                  {rc.x, rc.y, rc.z}};
        if (smooth_topo_cross(tp, ra.mgen, rb.mgen, rc.mgen)) {
            smooth_topo_count(SMOOTH_TOPO_FLATKEEP);
            ck_straight(SMOOTH_CKWHY_CROSSKEEP);
            if (weld) weld_call(wp, wn, SMOOTH_WELD_STRAIGHT, 0, wo, &wid);
            return 0;
        }
    }
    if (tf <= 1) {
        smooth_counters_for(why);
        ck_straight(why);
        if (weld) weld_call(wp, wn, SMOOTH_WELD_STRAIGHT, 0, wo, &wid);
        return 0;
    }

    g_ck.curved = 1; g_ck.tf = tf;
    smooth_count(SMOOTH_COUNT_SUBDIVIDED, 1);
    /* THE BUDGET'S DEMAND (ntr/smooth.h, THE BUDGET): one curved triangle
       the tick's own picture asked for. A SmoothMotion replay walks the same
       stream again and would count the same triangle once per blended
       picture, so it does not count; the verdict above does not depend on
       the level the picture is drawn at (the floor is 1), so this is a
       property of the scene. */
    if (!g_ip_replaying) smooth_count(SMOOTH_COUNT_DEMAND, 1);

    /* THE WELD (ntr/smooth.h): an edge this triangle shares with a piece
       whose normals differ there, or with a side drawn straight, is drawn by
       the edge's own rule, the same from both sides. Corners stay put. */
    if (weld && ra.nrm && rb.nrm && rc.nrm &&
        weld_call(wp, wn, SMOOTH_WELD_CURVED, 0, wo, &wid) && weld == 1 && !smooth_weld_guarded()) {
        float ven[6][3];
        if (smooth_cracks_on()) {
            g_ck.book = 1;
            if (!weld_view_en(wo, wr, s, g_ck.en)) g_ck.book = 0;
        }
        if (!smooth_live_mode() && store_eligible(ra, rb, rc)) {
            SmoothOverride wov;
            wov.cls = SMOOTH_CLASS_NONE;
            wov.flat = 0;
            std::memcpy(wov.en, wo, sizeof wov.en);
            if (smooth_try_store(s, ra, rb, rc, tf, pol, prof, &wov, weld_keyx(wid)))
                return 1;
        }
        if (weld_view_en(wo, wr, s, ven)) {
            smooth_store_count(SMOOTH_STORE_LIVE, 1);
            float pts[SMOOTH_MAX_GRID * 3];
            smooth_grid_positions_edges(s[0], s[1], s[2], ven, tf, pts);
            smooth_emit_grid(pts, tf, s, 0);
            return 1;
        }
        g_ck.book = 0;
    }

    /* THE GEOMETRY is what the store remembers, and this is where a triangle
       that has been seen before costs a lookup instead of a patch. */
    if (!smooth_live_mode() && store_eligible(ra, rb, rc) &&
        smooth_try_store(s, ra, rb, rc, tf, pol, prof))
        return 1;

    /* AND THE PATH FOR EVERYTHING ELSE: a bone joint, a matrix that is not a
       similarity, a shape the store would not take, or the A/B switch. The
       patch is rebuilt from the view-space corners, every frame. */
    smooth_store_count(SMOOTH_STORE_LIVE, 1);
    const long long t_sub = prof ? smooth_prof_ticks() : 0;
    if (smooth_live_mode()) {
        smooth_subdivide(s[0], s[1], s[2], tf, smooth_sink, 0);
    } else {
        float pts[SMOOTH_MAX_GRID * 3];
        smooth_grid_positions(s[0], s[1], s[2], tf, pts);
        smooth_emit_grid(pts, tf, s, 0);
    }
    if (prof) smooth_prof_add(SMOOTH_PROF_SUBDIV,
                              smooth_prof_ticks() - t_sub, 1);
    return 1;
}

/* Hand one triangle and what smooth_try did with it to the crack book. */
void smooth_crack_book(const GxVertex &a, const GxVertex &b, const GxVertex &c,
                       const GxRaw &ra, const GxRaw &rb, const GxRaw &rc, int took) {
    SmoothCrackTri t;
    std::memset(&t, 0, sizeof t);
    const GxVertex *v[3] = {&a, &b, &c};
    const GxRaw *r[3] = {&ra, &rb, &rc};
    for (int i = 0; i < 3; ++i) {
        t.p[i][0] = v[i]->vx; t.p[i][1] = v[i]->vy; t.p[i][2] = v[i]->vz;
        t.clip[i][0] = v[i]->x; t.clip[i][1] = v[i]->y; t.clip[i][2] = v[i]->w;
        t.raw[i][0] = r[i]->x; t.raw[i][1] = r[i]->y; t.raw[i][2] = r[i]->z;
        t.mg[i] = r[i]->mgen; t.nrm[i] = r[i]->nrm;
    }
    t.curved = took && g_ck.curved ? SMOOTH_CK_CURVED : SMOOTH_CK_STRAIGHT;
    t.why = g_ck.why;
    t.book = g_ck.book;
    t.tf = g_ck.tf;
    if (t.curved && g_ck.book) {
        std::memcpy(t.en, g_ck.en, sizeof t.en);
    } else if (t.curved) {
        float n[3][3];
        for (int i = 0; i < 3; ++i) {
            const float l = std::sqrt(v[i]->nx * v[i]->nx + v[i]->ny * v[i]->ny + v[i]->nz * v[i]->nz);
            const float il = l > 1e-8f ? 1.0f / l : 0.0f;
            n[i][0] = v[i]->nx * il; n[i][1] = v[i]->ny * il; n[i][2] = v[i]->nz * il;
        }
        static const int en_corner[6] = {0, 1, 1, 2, 0, 2};
        for (int k = 0; k < 6; ++k)
            for (int q = 0; q < 3; ++q) t.en[k][q] = n[en_corner[k]][q];
    }
    smooth_crack_tri(t);
}

void emit_tri(const GxVertex &a, const GxVertex &b, const GxVertex &c,
              const GxRaw &ra, const GxRaw &rb, const GxRaw &rc) {
    smooth_count(SMOOTH_COUNT_IN, 1);
    /* OFF IS ONE COMPARE. smooth_level() is a load of a file-scope int that
       walk_window sets once at boot; with the setting absent it is 0 and this
       function is the same call it always was. */
    if (smooth_level() > 0) {
        const int pf = smooth_prof_on();
        const long long t0 = pf ? smooth_prof_ticks() : 0;
        const int took = smooth_try(a, b, c, ra, rb, rc);
        if (pf) smooth_prof_add(SMOOTH_PROF_TRY, smooth_prof_ticks() - t0, 1);
        if (smooth_cracks_on()) smooth_crack_book(a, b, c, ra, rb, rc, took);
        if (took) return;
        if (pf) {
            const long long t1 = smooth_prof_ticks();
            emit_tri_near(a, b, c);
            smooth_prof_add(SMOOTH_PROF_NEAR, smooth_prof_ticks() - t1, 1);
            return;
        }
    }
    emit_tri_near(a, b, c);
}

void smooth_sink(void *, const SmoothVertex &a, const SmoothVertex &b,
                 const SmoothVertex &c) {
    /* Bucket three: ONE sub-triangle, from the three view-space points the
       kernel invented to the push onto the polygon list. This is the cost a
       cache of the patch maths cannot remove, which is the whole reason the
       profiler splits it out. It is also the bucket the clock read perturbs
       most, because it is read once per sub-triangle rather than once per
       triangle: see the honesty note in ntr/smooth.h. */
    const int prof = smooth_prof_on() >= 2;
    const long long t0 = prof ? smooth_prof_ticks() : 0;
    GxVertex out[3];
    const SmoothVertex *in[3] = {&a, &b, &c};
    for (int i = 0; i < 3; ++i) {
        const Vec4 view{in[i]->x, in[i]->y, in[i]->z, 1.0f};
        const Vec4 clip = view_to_clip(view);
        GxVertex &o = out[i];
        o.x = clip.x; o.y = clip.y; o.z = clip.z; o.w = clip.w;
        o.u = in[i]->u; o.v = in[i]->v; o.color = in[i]->color;
        o.vx = view.x; o.vy = view.y; o.vz = view.z; o.vw = view.w;
        o.nx = in[i]->nx; o.ny = in[i]->ny; o.nz = in[i]->nz;
    }
    /* Straight to the near clip: a sub-triangle must never re-enter
       smooth_try, and this is where that is enforced. */
    emit_tri_near(out[0], out[1], out[2]);
    if (prof) smooth_prof_add(SMOOTH_PROF_SINK, smooth_prof_ticks() - t0, 1);
}

// Assemble according to the active BEGIN_VTXS primitive type.
//
// MDL2: every emit_tri call now carries the three RAW vertex records beside
// the three transformed ones, taken from the parallel strip at the same
// indices. The assembly rules below -- which vertices make a triangle, in
// which order, with which winding -- are untouched.
void push_vertex(const GxVertex &v, const GxRaw *r) {
    g.strip.push_back(v);
    /* OFF COSTS NOTHING HERE EITHER. With the setting absent `r` is null, the
       parallel array stays empty and never allocates, and the references
       handed to emit_tri below are one dead record that the store can never
       accept -- emit_tri does not read them at all in that case, because
       smooth_level() is 0 and it short-circuits before smooth_try. */
    if (r) g.strip_raw.push_back(*r);
    const size_t n = g.strip.size();
    const GxVertex *s = g.strip.empty() ? 0 : &g.strip[0];
    const GxRaw *qp = (g.strip_raw.size() == n) ? &g.strip_raw[0] : 0;
    struct QAt {
        const GxRaw *p;
        const GxRaw &operator()(size_t i) const { return p ? p[i] : g_raw_dead; }
    } q = {qp};
    switch (g.prim) {
        case 0:                                        // separate triangles
            if (n == 3) {
                emit_tri(s[0], s[1], s[2], q(0), q(1), q(2));
                g.strip.clear();
                g.strip_raw.clear();
            }
            break;
        case 1:                                        // separate quads
            if (n == 4) {
                emit_tri(s[0], s[1], s[2], q(0), q(1), q(2));
                emit_tri(s[0], s[2], s[3], q(0), q(2), q(3));
                g.strip.clear();
                g.strip_raw.clear();
            }
            break;
        case 2:                                        // triangle strip
            if (n >= 3) {
                const size_t i0 = n - 3, i1 = n - 2, i2 = n - 1;
                if ((n - 3) & 1)                        // alternate winding
                    emit_tri(s[i1], s[i0], s[i2], q(i1), q(i0), q(i2));
                else
                    emit_tri(s[i0], s[i1], s[i2], q(i0), q(i1), q(i2));
            }
            break;
        case 3:                                        // quad strip
            if (n >= 4 && (n % 2) == 0) {
                const size_t i0 = n - 4, i1 = n - 3, i2 = n - 2, i3 = n - 1;
                emit_tri(s[i0], s[i1], s[i3], q(i0), q(i1), q(i3));
                emit_tri(s[i0], s[i3], s[i2], q(i0), q(i3), q(i2));
            }
            break;
        default: break;
    }
}

void vertex(int16_t x, int16_t y, int16_t z) {
    g.vx = x; g.vy = y; g.vz = z;
    if (g.prim < 0) return;
    /* The raw half of the vertex, for the shape store. With the setting
       absent it is filled with a pair of generations that can never match,
       so the store is unreachable rather than merely unused. */
    GxRaw r = g_raw_dead;
    if (smooth_level() > 0) {
        mtx_gen_update();
        r.x = x; r.y = y; r.z = z;
        r.nrm = g.nrm_raw;
        r.mgen = g_mtx_gen;
        r.ngen = g.nrm_gen;
        r.has_normal = (uint8_t)(g.normal_live ? 1 : 0);
    }
    push_vertex(project(x, y, z), smooth_level() > 0 ? &r : 0);
}

// --- command execution ------------------------------------------------------
/* TTMSHAKE093: a picture between two ticks blends each matrix word, and a word
   rounded back to the DS's 4.12 integer is a whole 1/4096 step of the matrix.
   On a small-scale model matrix (the TTM slide hall's walls, entries a few
   hundred counts) that step is about a tenth of a percent of the model's size,
   which is a pixel of wobble from picture to picture while the camera drifts
   slowly. The replay therefore hands the exact blended value beside the
   rounded word (g_ip_ex_on) and the matrix commands read that. Off (every
   tick's own command, and the blend's alpha 1) the words are read as ever. */
double g_ip_ex[32];
int g_ip_ex_on = 0;
inline float mtx_word(const uint32_t *p, int i) {
    if (g_ip_ex_on) return (float)(g_ip_ex[i] * (double)FX12);
    return static_cast<int32_t>(p[i]) * FX12;
}

void load_mtx(Mat &dst, const uint32_t *p, int n) {
    Mat m = Mat::identity();
    if (n == 16) {
        for (int i = 0; i < 16; ++i) m.m[i] = mtx_word(p, i);
    } else {  // 4x3: three rows of 3 plus a translation row
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 3; ++c) m.m[r * 4 + c] = mtx_word(p, r * 3 + c);
        m.m[3] = m.m[7] = m.m[11] = 0.0f;
        m.m[15] = 1.0f;
    }
    dst = m;
}

Mat mat_3x3(const uint32_t *p) {
    Mat m = Mat::identity();
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) m.m[r * 4 + c] = mtx_word(p, r * 3 + c);
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

// THE PENDING SWAP (run link100, boot plan rung R3b, step BSWAP).
//
// GEOMETRY COMMAND 0x50 IS NOT AN IMMEDIATE ON HARDWARE. SWAP_BUFFERS ends
// a frame's geometry submission and the engine performs the swap AT THE
// NEXT VBLANK, not at the store -- which is why the ROM issues it from
// func_020190b8 at phase 6, one statement before the loop sleeps on the
// VBlank at phase 7 (src/func_020197b8.c:51-55). Modelling it as an
// immediate is what run link100 lane R3CFIX measured going wrong: its arm
// ran the whole host frame reset inside the store, and
// src/func_ov007_020b6c54.c -- the VS menu, battery scene 6 -- writes
// SWAP_BUFFERS from INSIDE a scene body, so that scene's own swap wiped the
// geometry state mid-frame (worklist.md, R3CFIX CLOSED RED).
//
// SO 0x50 LATCHES A REQUEST AND THE FRAME BOUNDARY CONSUMES IT. A body that
// asks mid-frame is asking for the swap that ends the frame it is in, which
// is what the hardware gives it.
//
// AND WHAT `APPLIED` DOES IN THIS HOST IS EXACTLY THIS AND NOTHING MORE:
// it retires the request and counts it. The picture this port draws is
// composed by gx_render and the triangle list is emptied by gx_reset, which
// tests/walk_window.cpp calls at the head of its render section -- so the
// host already performs, at its own point, the thing a swap means here.
// Moving that reset onto this boundary is NOT this rung: R3CFIX measured
// that too, and level 35 (the one battery row with a mid-run warp) faulted
// on a texture bind, because this host tears a level down and boots and
// renders its replacement INSIDE ONE FRAME and the cartridge's loop never
// does that. Rung R3d removes the host loop and with it that artefact; then
// the reset has a boundary to move onto. Until then this latch is the seam,
// honestly empty, and the counters below are what make it measurable.
uint32_t g_swap_pending;    // a SWAP_BUFFERS the boundary has not consumed
uint32_t g_swap_pending_p;  // its parameter word (bit 0 manual sort,
                            // bit 1 depth-buffering select)
uint32_t g_swaps_requested; // 0x50 commands executed this run
uint32_t g_swaps_applied;   // requests retired at a frame boundary
uint32_t g_swaps_retired;   // requests retired by a gx_reset instead

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

void exec(unsigned cmd, const uint32_t *p, int np) {   /* GHOSTPERF1: a full-width argument (a byte one was spilled and read back wide on every command: a store-forwarding stall) */
    (void)np;
    if (g_ip_rec) ip_push(IPK_CMD, cmd, np, p);
    if (!g_ip_replaying) {
        ++g_cmd_n[cmd];
        if (cmd == 0x50) g_swap_param = p[0];
    }
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
            m.m[0] = mtx_word(p, 0);
            m.m[5] = mtx_word(p, 1);
            m.m[10] = mtx_word(p, 2);
            if (g.mode == MTX_PROJ) g.proj = mul(m, g.proj);
            else if (g.mode == MTX_TEX) g.tex = mul(m, g.tex);
            else g.pos = mul(m, g.pos);
            break;
        }
        case 0x1C: {                                             // MTX_TRANS
            Mat m = Mat::identity();
            m.m[12] = mtx_word(p, 0);
            m.m[13] = mtx_word(p, 1);
            m.m[14] = mtx_word(p, 2);
            if (g.mode == MTX_PROJ) g.proj = mul(m, g.proj);
            else if (g.mode == MTX_TEX) g.tex = mul(m, g.tex);
            else { g.pos = mul(m, g.pos); if (g.mode == MTX_POSVEC) g.vec = mul(m, g.vec); }
            break;
        }
        case 0x20:                                               // COLOR
            g.color = bgr555_to_argb(static_cast<uint16_t>(p[0] & 0x7FFF));
            g.normal_live = 0;   /* MDL: colour set by hand, not by lighting */
            break;
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
            { /* MISC093 probe (D8): SM64DS_MISC93_TG2=<n> logs the first n texgen-2 normals per run with the texture matrix */
                static int lim = -2, cnt = 0;
                if (lim == -2) { const char *e = std::getenv("SM64DS_MISC93_TG2"); lim = e ? std::atoi(e) : -1; }
                if (lim > 0 && ((g_teximage >> 30) & 3) == 2 && cnt < lim) {
                    ++cnt;
                    std::fprintf(stderr, "[tg2] n=%d teximage=%08x n=(%.3f %.3f %.3f) raw=(%.2f %.2f) tex=[%.2f %.2f %.2f / %.2f %.2f %.2f / %.2f %.2f %.2f / %.2f %.2f] uv=(%.2f %.2f) pol=%08x\n",
                                 cnt, (unsigned)g_teximage, nx, ny, nz, g.raw_u, g.raw_v, g.tex.m[0], g.tex.m[1], g.tex.m[2], g.tex.m[4], g.tex.m[5], g.tex.m[6],
                                 g.tex.m[8], g.tex.m[9], g.tex.m[10], g.tex.m[12], g.tex.m[13], g.u, g.v, (unsigned)g.poly_attr);
                }
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

            /* MDL: latch it for the smoother. Same value, same space, same
               moment the hardware uses it: nothing below this line reads
               these two fields, so lighting is bit-for-bit what it was. */
            g.nrm[0] = nx; g.nrm[1] = ny; g.nrm[2] = nz;
            g.normal_live = 1;
            /* MDL2: and the RAW payload beside it, with the matrix generation
               it was latched under, for the shape store (ntr/smooth.h). This
               is the number the MODEL carries; the three floats above are
               what one particular matrix made of it. Behind the level check,
               so a run with the setting absent pays one compare on a
               file-scope int and nothing else. */
            if (smooth_level() > 0) {
                mtx_gen_update();
                g.nrm_raw = p[0] & 0x3FFFFFFFu;
                g.nrm_gen = g_mtx_gen;
            }

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
                g.normal_live = 0;   /* MDL: colour set by hand, as case 0x20 */
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
            /* MDL2: the parallel raw strip is indexed with this one, so it
               empties with it. Leaving it behind here silently offset every
               raw record by however many vertices the previous primitive had
               left, which is the shape store keying a patch on another
               triangle's coordinates -- the SM64DS_SMOOTH_ABDIFF arm
               measured that as a thousand view units of separation before
               this line existed. */
            g.strip_raw.clear();
            g.strip_parity = 0;
            break;
        case 0x41:                                               // END_VTXS
            g.prim = -1;
            g.strip.clear();
            g.strip_raw.clear();
            break;
        case 0x50:                                               // SWAP_BUFFERS
            /* rung R3b/BSWAP: latch, do not act. See THE PENDING SWAP above. */
            g_swap_pending = 1;
            g_swap_pending_p = p[0];
            ++g_swaps_requested;
            break;
        case 0x60: {                                             // VIEWPORT
            // The register speaks DS panel coordinates (0..255 x 0..191);
            // scale to the framebuffer so game-issued full-screen viewports
            // fill a hi-res target too. At 256x192 the factors are 1 and
            // this is exactly the old math.
            /* THE DS COUNTS THIS REGISTER'S Y FROM THE BOTTOM: bits 8-15 are
               the lowest screen row of the rectangle and bits 24-31 the
               highest, so the top row on the picture is 191 - bits 24-31 and
               the bottom row 191 - bits 8-15 (melonDS GPU3D.cpp, the 0x60
               case: Viewport[1] = 191 - Y1, Viewport[3] = 191 - Y2). A full
               screen (0 and 191) is its own mirror image, which is why this
               was never seen until a scene asked for a smaller rectangle: the
               ending's 16:9 letterbox writes 32 and 175 and the cartridge
               puts the picture on rows 16..159, bars above and below. */
            const int x1 = p[0] & 0xFF;
            int y1 = (191 - ((p[0] >> 24) & 0xFF)) & 0xFF;
            const int x2 = (p[0] >> 16) & 0xFF;
            int y2 = (191 - ((p[0] >> 8) & 0xFF)) & 0xFF;
            /* DRAWDIST2 (tester, 16:9: the bars around Peach's dialogue scene, the lower one is taller than the
               upper). The cartridge puts the ending's picture on rows 16..159 and keeps rows 160..191 for the
               subtitle, so the bars are 16 and 32 rows. On a picture wider than 4:3 the picture is centred
               between equal bars instead (rows 24..167, 24 and 24); the subtitle box is translucent and keeps its
               place over the lowest rows. At 4:3 nothing moves: the ROM's own rows. Only this exact rectangle
               (full width, rows 16..159) is moved, so no other scene's viewport is touched. */
            if (!menu_wide() && x1 == 0 && x2 == 255 && y1 == 16 && y2 == 159 &&
                present_w() * 3 > present_h() * 4) {
                y1 += 8;
                y2 += 8;
                g_ending_vp_shifted = 1;
            } else g_ending_vp_shifted = 0;
            /* Scaled into the PRESENT rectangle and offset by its origin, so
               a game-issued full-screen viewport fills the picture the host is
               presenting rather than the whole wide buffer. present_* is the
               active extent at the origin on every other run, so this is the
               same math as before. */
            const bool mw = menu_wide();      /* MENU WIDE 3D: the whole wide frame is the viewport's 256 x 192 */
            const int px = mw ? 0 : present_x(), py = mw ? 0 : present_y();
            const int pw = mw ? active_w : present_w(), ph = mw ? active_h : present_h();
            g.vp_x = px + x1 * pw / 256;
            g.vp_y = py + y1 * ph / 192;
            g.vp_w = (x2 - x1 + 1) * pw / 256;
            g.vp_h = (y2 - y1 + 1) * ph / 192;
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

namespace {
/* param_count as a table (GHOSTPERF1): the switch above, answered once per command byte */
struct ParamTab {
    uint8_t n[256];
    ParamTab() { for (int i = 0; i < 256; ++i) n[i] = static_cast<uint8_t>(param_count(static_cast<uint8_t>(i))); }
};
}  // namespace

void gx_write_fifo_block(const uint32_t *src, int n) {
    static const ParamTab tab;
    static int off = -1;
    if (off < 0) { const char *e = std::getenv("SM64DS_GX_BLOCK"); off = (e && e[0] == '0' && !e[1]) ? 1 : 0; }
    int i = 0;
    while (i < n) {
        if (off || g_queued != 0) { gx_write_fifo(src[i++]); continue; }
        const uint32_t w = src[i];
        const uint8_t c[4] = { static_cast<uint8_t>(w), static_cast<uint8_t>(w >> 8), static_cast<uint8_t>(w >> 16),
                               static_cast<uint8_t>(w >> 24) };
        const int need = tab.n[c[0]] + tab.n[c[1]] + tab.n[c[2]] + tab.n[c[3]];
        if (i + 1 + need > n) { gx_write_fifo(src[i++]); continue; }   // the group runs past the block: word by word
        g_fifo_n += static_cast<uint32_t>(1 + need);
        for (int k = 0; k <= need; ++k) gx_stream_note(src[i + k]);
        const uint32_t *pp = src + i + 1;
        for (int q = 0; q < 4; ++q) {
            const int np = tab.n[c[q]];
            exec(c[q], np ? pp : nullptr, np);
            pp += np;
        }
        i += 1 + need;
    }
}

void gx_set_matrix_slot(int slot, const float m[16]) {
    if (slot < 0 || slot >= 32) return;
    Mat mm;
    for (int i = 0; i < 16; ++i) mm.m[i] = m[i];
    g.pos_stack[slot] = mm;
    g.vec_stack[slot] = mm;
    if (g_ip_rec) {
        uint32_t q[17];
        q[0] = (uint32_t)slot;
        std::memcpy(q + 1, m, 16 * sizeof(float));
        ip_push(IPK_SLOT, 0, 17, q);
    }
}

void gx_set_material(uint32_t dif_amb, uint32_t spe_emi) {
    exec(0x30, &dif_amb, 1);
    exec(0x31, &spe_emi, 1);
}

void gx_set_light(int index, float dx, float dy, float dz, uint32_t bgr555) {
    if (index < 0 || index > 3) return;
    if (g_ip_rec) {
        uint32_t q[5];
        q[0] = (uint32_t)index;
        std::memcpy(q + 1, &dx, 4);
        std::memcpy(q + 2, &dy, 4);
        std::memcpy(q + 3, &dz, 4);
        q[4] = bgr555;
        ip_push(IPK_LIGHT, 0, 5, q);
    }
    const float len = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (len > 1e-6f) { dx /= len; dy /= len; dz /= len; }
    g.lights[index] = {dx, dy, dz,
                       (bgr555 & 0x1F) / 31.0f, ((bgr555 >> 5) & 0x1F) / 31.0f,
                       ((bgr555 >> 10) & 0x1F) / 31.0f};
}

void gx_enable_lights(uint32_t mask) {
    g.light_mask = mask & 0xF;
    if (g_ip_rec) ip_push(IPK_LMASK, 0, 1, &mask);
}

/* ---- WHICH TEXTURE IS THIS, AS A NUMBER (run hd2, lane GPU2) -------------
   An optional graphics-card backend holds its own copy of every texture it
   has drawn with, and the obvious key -- the decoded buffer's address -- is
   the wrong one: the VRAM decode cache frees its buffers at a scene change
   and the allocator hands the same address back for different pixels, so a
   pointer key serves a stale picture. So a bind hands out a number instead.
   It counts up, it is never reused, and gx_invalidate_textures both clears
   this registry and bumps the generation, which is a backend's signal to drop
   everything it held.

   THE REGISTRY IS ONLY CONSULTED WHEN A BACKEND EXISTS. g_gpu_opaque is null
   on every ordinary run, so gx_bind_texture below does exactly what it did
   before this existed: no lookup, no insert, no allocation. The dimensions
   ride along because a caller outside the VRAM cache (a harness, a smoke) can
   rebind the same buffer at a different size, and that is a different
   picture. */
namespace {
uint32_t g_tex_next_id = 1;
uint32_t g_tex_generation = 1;
struct TexIdent { int w, h; uint32_t id; };
std::map<const uint32_t *, TexIdent> g_tex_ids;

/* The registered backend for the opaque pass, or null. See GxGpuFrame in
   ntr/gx.h for what it is handed and what returning 0 means. */
GxGpuOpaqueFn g_gpu_opaque = nullptr;
/* and the one for the edge smoothing, or null (see GxGpuAa in ntr/gx.h) */
GxGpuAaFn g_gpu_aa = nullptr;
}  // namespace

void gx_bind_texture(const uint32_t *rgba, int width, int height) {
    g.tex_rgba = rgba;
    g.tw = width;
    g.th = height;
    // The direct entry (the BMD harness path) carries no TEXIMAGE_PARAM, so it
    // keeps the plain repeat-in-both-directions behaviour it always had; the
    // VRAM bind below overrides this with the material's real wrap mode.
    g.tex_wrap = 3;
    // AND ONE HOST PIXEL PER DS TEXEL. Every caller of this entry hands over a
    // buffer at the DS texture's own size, so the scale is 1 unless the VRAM
    // bind below knowingly replaced the image and says otherwise AFTER this
    // call. Clearing it here rather than leaving it is what stops a replaced
    // texture's scale riding along into the next unreplaced bind.
    g.tex_scale = 1;
    /* THE BIND IS WHERE A TEXTURE GETS ITS NUMBER, and only when something is
       going to ask for it. With no backend registered -- every ordinary run --
       this is one test against a null pointer and the id stays 0. */
    if (g_gpu_opaque) {
        if (!rgba || width <= 0 || height <= 0) {
            g.tex_id = 0;
        } else {
            TexIdent &e = g_tex_ids[rgba];
            if (!e.id || e.w != width || e.h != height) {
                e.w = width;
                e.h = height;
                e.id = g_tex_next_id++;
            }
            g.tex_id = e.id;
        }
    }
    if (g_ip_rec && !g_ip_in_vram) ip_rec_bind_now();
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
/* What a key maps to. This used to be the decoded texels alone, and it is now
   the BUFFER THAT ACTUALLY GETS BOUND plus its real pixel size, because an HD
   pack (ntr/hdtex.h) may have replaced the picture with a whole multiple of
   itself. `scale` is that multiple and travels to the raster through
   GxTriangle::tex_scale; w/h are px's real dimensions, which is what the
   sampler wants for its wrap and clamp arithmetic. With no pack every entry
   is the decode, at the DS's own size, with scale 1. */
struct TexEntry {
    std::vector<uint32_t> px;
    int w = 0, h = 0;
    uint8_t scale = 1;
    /* THE MIP CHAIN, and it exists only under TextureFilter 2 (run hd2).
       mip[0] is the picture at half width and half height, mip[1] a quarter,
       and so on down to 1x1, so mip[k] is level k+1 and px above is level 0.
       Built ONCE, when the texture enters the cache, never per frame and
       never per triangle. Empty at every other filter mode, so a default run
       holds not one extra byte. */
    std::vector<std::vector<uint32_t>> mip;
};
std::map<TexKey, TexEntry> g_vram_tex_cache;

/* THE BOUND BUFFER BACK TO ITS CACHE ENTRY. The raster is handed a plain
   `const uint32_t *` on GxTriangle::tex and the mip chain hangs off the entry,
   so trilinear needs the way back. Keyed on the pixel buffer's address, which
   is stable for the life of the entry (a std::map node does not move), and
   filled as each entry is created. Only ever read under TextureFilter 2, and
   only once per triangle per frame -- never per pixel. */
std::map<const uint32_t *, const TexEntry *> g_tex_by_px;

/* 0 nearest, 1 bilinear, 2 trilinear. Latched by gx_configure_texture_filter
   at boot and read-only afterwards, so the cache cannot end a run with chains
   for some of its entries and not others. */
int g_tex_filter = 0;

/* ONE 2x2 BOX TAP, IN PREMULTIPLIED ALPHA. Averaging four texels' colours
   without weighting them by their own alpha drags the colour of a fully
   transparent texel into the result, and on a DS cut-out -- a fence, a leaf,
   the A3I5 gradients -- that colour is whatever the artist left in the
   transparent part of the picture, usually black. So each colour is weighted
   by its texel's alpha, the sum is divided by the summed alpha, and a texel
   at alpha 0 contributes nothing at all. The output alpha is the plain
   average, because alpha is a coverage and averaging coverages is right.
   All four taps zero means a transparent texel, which is the one case with no
   colour to carry, and the sampler's own alpha test then drops the pixel. */
uint32_t box4(uint32_t p0, uint32_t p1, uint32_t p2, uint32_t p3) {
    const uint32_t a0 = p0 >> 24, a1 = p1 >> 24, a2 = p2 >> 24, a3 = p3 >> 24;
    const uint32_t as = a0 + a1 + a2 + a3;
    if (!as) return 0;
    const uint32_t a = (as + 2) / 4;
    const uint32_t r = (((p0 >> 16) & 0xFF) * a0 + ((p1 >> 16) & 0xFF) * a1 +
                        ((p2 >> 16) & 0xFF) * a2 + ((p3 >> 16) & 0xFF) * a3 +
                        as / 2) / as;
    const uint32_t g = (((p0 >> 8) & 0xFF) * a0 + ((p1 >> 8) & 0xFF) * a1 +
                        ((p2 >> 8) & 0xFF) * a2 + ((p3 >> 8) & 0xFF) * a3 +
                        as / 2) / as;
    const uint32_t b = ((p0 & 0xFF) * a0 + (p1 & 0xFF) * a1 +
                        (p2 & 0xFF) * a2 + (p3 & 0xFF) * a3 + as / 2) / as;
    return (a << 24) | (r << 16) | (g << 8) | b;
}

/* Build the whole chain off the entry's own picture, halving until 1x1. An
   ODD dimension halves by dropping to floor and clamping the second tap to
   the last row or column, which is what every box reducer does with an odd
   edge; DS textures are powers of two and an HD replacement is a whole
   multiple of one, so this is the safety net rather than the usual path.
   THE CHAIN IS BUILT FROM WHATEVER IS BOUND, so with an HD pack it is built
   from the REPLACEMENT at the replacement's own size, which is the only
   reading that makes the level-of-detail arithmetic mean anything. */
void build_mip_chain(TexEntry &e) {
    if (!e.mip.empty() || e.w <= 0 || e.h <= 0) return;
    const uint32_t *src = e.px.data();
    int sw = e.w, sh = e.h;
    while (sw > 1 || sh > 1) {
        const int dw = sw > 1 ? sw / 2 : 1;
        const int dh = sh > 1 ? sh / 2 : 1;
        std::vector<uint32_t> lvl((size_t)dw * dh);
        for (int y = 0; y < dh; ++y) {
            const int y0 = sh > 1 ? y * 2 : 0;
            const int y1 = (y0 + 1 < sh) ? y0 + 1 : y0;
            for (int x = 0; x < dw; ++x) {
                const int x0 = sw > 1 ? x * 2 : 0;
                const int x1 = (x0 + 1 < sw) ? x0 + 1 : x0;
                lvl[(size_t)y * dw + x] =
                    box4(src[(size_t)y0 * sw + x0], src[(size_t)y0 * sw + x1],
                         src[(size_t)y1 * sw + x0], src[(size_t)y1 * sw + x1]);
            }
        }
        e.mip.push_back(std::move(lvl));
        src = e.mip.back().data();
        sw = dw;
        sh = dh;
    }
}

/* How much memory every chain in the cache is holding, for the report: the
   chain of a WxH picture is about a third of WxH again. */
size_t mip_bytes_held() {
    size_t n = 0;
    for (const auto &kv : g_vram_tex_cache)
        for (const auto &l : kv.second.mip) n += l.size() * sizeof(uint32_t);
    return n;
}

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
        TexEntry entry;
        entry.w = d.width;
        entry.h = d.height;
        entry.scale = 1;
        /* THE HD PACK, AND ONLY ON A CACHE MISS. hdtex_wants_work() is an int
           compare and it is false unless a pack was indexed or a dump
           directory was named, so a default run reaches nothing below: no
           hash, no file, no allocation. Hanging the whole thing off the MISS
           rather than off the bind is what keeps the cost proportional to the
           number of distinct textures a level has rather than to the hundreds
           of binds a frame makes -- the key above already discriminates a
           stale slot, so a hit needs no re-examination. */
        if (hdtex_wants_work()) {
            const uint64_t name = hdtex_name(g_teximage, g_plttbase);
            hdtex_dump(name, d.width, d.height, rgba.data());
            std::vector<uint32_t> hd;
            const int s = hdtex_lookup(name, d.width, d.height, hd);
            if (s >= 1) {
                entry.px = std::move(hd);
                entry.w = d.width * s;
                entry.h = d.height * s;
                entry.scale = static_cast<uint8_t>(s);
            }
        }
        if (entry.px.empty()) entry.px = std::move(rgba);
        it = g_vram_tex_cache.emplace(key, std::move(entry)).first;
        /* THE CHAIN AND THE WAY BACK TO IT, ON THE MISS AND NOWHERE ELSE
           (run hd2). Both hang off the cache miss for hdtex's reason one
           block up: the cost is then proportional to the number of DISTINCT
           textures a level has rather than to the hundreds of binds a frame
           makes. Under TextureFilter 0 or 1 neither line does any work worth
           naming -- the registry insert is one pointer into a map, and the
           chain is not built at all. */
        if (g_tex_filter >= 2) build_mip_chain(it->second);
        g_tex_by_px[it->second.px.data()] = &it->second;
    }
    /* The bound buffer's REAL dimensions, which are the DS texture's unless a
       pack replaced it; the scale beside them is what lets the sampler put the
       DS's own texel grid back over a bigger picture. */
    gx_bind_texture(it->second.px.data(), it->second.w, it->second.h);
    g.tex_scale = it->second.scale;
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

void gx_teximage_param(uint32_t v) {
    g_teximage = v;
    ++g_ip_in_vram;
    bind_from_vram();
    --g_ip_in_vram;
    if (g_ip_rec) ip_rec_bind_now();
}
void gx_pltt_base(uint32_t v) {
    g_plttbase = v;
    ++g_ip_in_vram;
    bind_from_vram();
    --g_ip_in_vram;
    if (g_ip_rec) ip_rec_bind_now();
}

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

/* THE FRAME BOUNDARY CONSUMES THE PENDING SWAP (rung R3b, step BSWAP).
   Called from tests/walk_window.cpp's frame foot, immediately after the
   ROM's own func_020190b8() -- which is where func_020197b8.c:52 puts it,
   between phase 6 and the phase-7 wait. Read THE PENDING SWAP above for
   what this does and, more importantly, what it deliberately does not. */
void gx_swap_apply() {
    if (!g_swap_pending) return;
    g_swap_pending = 0;
    ++g_swaps_applied;
}

/* The run's swap account, for the [r3b] line the gate captures. */
void gx_swap_counts(uint32_t &requested, uint32_t &applied,
                    uint32_t &retired, uint32_t &pending, uint32_t &param) {
    requested = g_swaps_requested;
    applied = g_swaps_applied;
    retired = g_swaps_retired;
    pending = g_swap_pending;
    param = g_swap_pending_p ? g_swap_pending_p : g_swap_param;
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
void gx_invalidate_textures() {
    /* THE REGISTRY GOES WITH THE CACHE. It holds addresses of buffers the
       clear below is about to free, so an entry left behind would hand the
       raster a mip chain that no longer exists. Cleared first for the same
       reason. */
    g_tex_by_px.clear();
    g_vram_tex_cache.clear();
    /* AND SO DOES THE ID REGISTRY, for a stronger version of the same reason:
       the buffers it names are about to be freed and the allocator will hand
       those addresses out again. The generation bump is what tells a graphics
       card backend that every texture it is holding is now a picture of
       something else. The counter itself is NOT reset, so an id that named one
       texture never names another. */
    g_tex_ids.clear();
    ++g_tex_generation;
    smooth_weld_scene_change();   /* SMOOTHWELD1: the weld book starts over with the new models */
}

uint32_t gx_texture_generation() { return g_tex_generation; }

/* THE A/B'S EXIT SUMMARY IS ARMED HERE, at boot, rather than on the first
   frame the opaque pass draws. A scene that never reaches gx_render would
   otherwise print nothing at all and read as a missing row; armed here it
   prints "no 3D frames" instead, which is a result. Defined further down in
   this file's own unnamed namespace. */
namespace { int ab_mode(); }

/* ---- WHETHER THIS FRAME'S SMOOTHED GRIDS MAY BE KEPT FOR THE CARD (SMOOTHGPU1)
   SM64DS_SMOOTH_GPU=0 turns it off for a run (an A/B on one binary); the
   debug dumps that walk the polygon list (the TRI_LOG / TEXPX reports, the
   per-texture and per-pixel probes) turn it off too, because they would read a
   list with the grids missing. Everything else is the card's own say: the probe
   below is asked every frame and answers 2 only when the registered backend
   will draw the whole frame AND takes grids. */
static void grid_pool_clear_hook();
static bool grid_env_ok() {
    static int v = -1;
    if (v < 0) {
        v = 1;
        const char *k = std::getenv("SM64DS_SMOOTH_GPU");
        if (k && *k == '0') v = 0;
        static const char *const dbg[] = {"SM64DS_TRI_LOG", "SM64DS_TEXPX",
                                          "SM64DS_TEX_ONLY", "SM64DS_PROBE_PX"};
        for (const char *d : dbg)
            if (std::getenv(d)) v = 0;
    }
    return v != 0;
}
static bool gpu_grid_probe() {
    GxGpuFrame f;
    std::memset(&f, 0, sizeof f);
    f.probe = 1;
    f.full_ok = 2;
    f.probe_grids = 1;
    return g_gpu_opaque(&f) == 2;
}
static bool grid_defer_wanted() {
    static bool hooked = false;
    if (!hooked) { hooked = true; smooth_store_set_clear_hook(&grid_pool_clear_hook); }
    if (!grid_env_ok() || smooth_level() <= 0 || !g_gpu_opaque || ab_mode())
        return false;
    return gpu_grid_probe();
}

void gx_set_gpu_opaque(GxGpuOpaqueFn fn) {
    g_gpu_opaque = fn;
    ab_mode();
}
int gx_gpu_opaque_registered() { return g_gpu_opaque != nullptr; }

/* AND THE POINTER ITSELF, so a caller that wants to draw the SAME list both
   ways can put it back after taking it out. The A/B inside gx_render does
   that without help because it is inside; a test outside cannot. */
GxGpuOpaqueFn gx_gpu_opaque() { return g_gpu_opaque; }

void gx_set_gpu_aa(GxGpuAaFn fn) { g_gpu_aa = fn; }

/* ---- THE TWO PICTURE-SMOOTHING SETTINGS' LATCHES (run hd2) ---------------
   Both are called once at boot from walk_window, beside ntr::configure_aspect,
   and both clamp here as well as at the settings accessor: a caller that
   reaches this entry from anywhere else (a smoke, a harness) cannot hand the
   raster a mode it has no body for. */
void gx_configure_texture_filter(int mode) {
    g_tex_filter = mode < 0 ? 0 : (mode > 2 ? 2 : mode);
}

int gx_texture_filter() { return g_tex_filter; }

void gx_set_ghost_layer(int on) { g_ghost_layer_on = on != 0; }
int gx_ghost_layer() { return g_ghost_layer_on ? 1 : 0; }

/* SM64DS_TEX_MIPS=1: how much the chains are holding, once every 300 frames,
   which is the answer to "what does trilinear cost in memory over a level".
   Off by default, and at filter 0 or 1 there are no chains to report. */
void gx_mip_report() {
    static int want = -1;
    if (want < 0) want = getenv("SM64DS_TEX_MIPS") ? 1 : 0;
    if (!want) return;
    static unsigned f;
    if ((f++ % 300) != 0) return;
    size_t chains = 0;
    size_t base = 0;
    for (const auto &kv : g_vram_tex_cache) {
        if (!kv.second.mip.empty()) ++chains;
        base += kv.second.px.size() * sizeof(uint32_t);
    }
    const size_t mips = mip_bytes_held();
    std::fprintf(stderr,
                 "[mips] frame %u: %zu cached texture(s), %zu with a chain; "
                 "base %zu bytes, chains %zu bytes (%.1f%% on top)\n",
                 f - 1, g_vram_tex_cache.size(), chains, base, mips,
                 base ? 100.0 * (double)mips / (double)base : 0.0);
    std::fflush(stderr);
}

/* SM64DS_SMOOTH_TRIHASH=1: one line per frame with the count and an FNV-1a
   hash of the polygon list the frame ended with (every vertex float, the
   texture identity and the state bytes; not the texture pointer). It is how a
   change to the smoothing's emit path is shown bit-identical to the one it
   replaces without comparing pictures. Off, it is one compare. */
static void smooth_trihash_frame() {
    static int on = -1;
    if (on < 0) on = std::getenv("SM64DS_SMOOTH_TRIHASH") ? 1 : 0;
    if (!on) return;
    grids_materialize_w(1);     // the list the frame ended with, grids included
    uint64_t h = 1469598103934665603ull;
    auto mix = [&h](const void *p, size_t n) {
        const unsigned char *b = static_cast<const unsigned char *>(p);
        for (size_t i = 0; i < n; ++i) { h ^= b[i]; h *= 1099511628211ull; }
    };
    for (const GxTriangle &t : g.tris) {
        mix(t.v, sizeof t.v);
        mix(&t.tw, sizeof t.tw); mix(&t.th, sizeof t.th);
        mix(&t.tex_scale, 1); mix(&t.tex_id, sizeof t.tex_id);
        mix(&t.cull, 1); mix(&t.alpha, 1); mix(&t.wrap, 1);
        mix(&t.translucent, 1); mix(&t.mode, 1); mix(&t.polyid, 1);
        mix(&t.depth_eq, 1); mix(&t.dbg_tex, sizeof t.dbg_tex);
    }
    static unsigned long long frame_no = 0;
    std::fprintf(stderr, "[trihash] r%llu n=%zu h=%016llx\n",
                 frame_no++, g.tris.size(),
                 (unsigned long long)h);
}

void gx_reset() {
    ++g_resets;
    g_frame_ortho = false;
    grid_stats_frame();
    smooth_trihash_frame();
    /* MDL. gx_reset is the host's own "begin a frame's command stream" (see
       the long note below), so it is the frame boundary the smoother's
       counters and its crack census are keyed to. It is also the flush point
       a buffering smoother would need; this one buffers nothing, so today it
       only counts. Costs an increment when the census is off.
       This lane's other lines outside its own regions of this file are the
       ntr/smooth.h include, and one assignment each in the COLOR (0x20),
       NORMAL (0x21) and DIF_AMB (0x30) cases of exec(), all four listed in
       the lane's report. */
    smooth_frame_mark();
    /* rung R3b/BSWAP: a reset ENDS the frame the pending swap was asking
       about, so it retires the request rather than letting it stand into
       the next frame. This is the path a scene body's own SWAP_BUFFERS
       takes: hal/scene_boot.cpp opens every scene frame with a gx_reset and
       has no frame-foot swap call, so scene 6's mid-body write is retired
       here and the scene path behaves exactly as it did before this rung. */
    if (g_swap_pending) { g_swap_pending = 0; ++g_swaps_retired; }
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
                    " xy/z=%.3f trans(%.3f %.3f %.3f)"
                    " ds x %.1f..%.1f y %.1f..%.1f (%.1f x %.1f)\n",
                    G.frame, k, G.tab[k].n, G.tab[k].a, G.tab[k].b,
                    G.tab[k].c,
                    G.tab[k].c != 0.0f ? G.tab[k].a / G.tab[k].c : 0.0f,
                    G.tab[k].tx, G.tab[k].ty, G.tab[k].tz,
                    G.tab[k].sx0, G.tab[k].sx1, G.tab[k].sy0,
                    G.tab[k].sy1, G.tab[k].sx1 - G.tab[k].sx0,
                    G.tab[k].sy1 - G.tab[k].sy0);
        }
        ++G.frame; G.n = 0; G.snapped = 0; G.ntab = 0;
    }

    std::vector<GxVertex> strip = std::move(g.strip);
    std::vector<GxTriangle> tris = std::move(g.tris);
    std::vector<GxGrid> grids = std::move(g.grids);
    std::vector<float> grid_pts = std::move(g.grid_pts);
    grids.clear();
    grid_pts.clear();
    /* the raw strip rides with the strip it mirrors, for the same reason */
    std::vector<GxRaw> strip_raw = std::move(g.strip_raw);
    strip.clear();
    tris.clear();
    strip_raw.clear();
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

         363, EVERY FRAME  mem_render -> _ZN14dScMgMemory2_c6RenderEv
                           -> Camera_UpdateMatrices -> G3i_PerspectiveW_
         361, ONCE AT INIT cup_init -> _ZN10dScMgCup_c13InitResourcesEv
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
    g.grids = std::move(grids);
    g.grid_pts = std::move(grid_pts);
    g_grid_defer = grid_defer_wanted();
    g.strip_raw = std::move(strip_raw);
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
    /* run interp1: a frame's stream opens here, so its record does too. */
    if (g_ip_armed && !g_ip_open) ip_rec_begin();
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

size_t gx_polygon_count() {
    return g.tris.size() + (size_t)g.grid_subtris;
}

const GxTriangle *gx_polygons(size_t &count) {
    grids_materialize_w(2);     // a caller that reads the list gets the triangles
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

/* THE FLOOR, WITHOUT THE LIBRARY CALL (run perf2, lane RASTER). The sampler
   floors every texture coordinate, and in this 32-bit build std::floor is a
   real call into the C library that returns through the x87 stack: about a
   sixth of the raster's time was that call. These two give the same answer
   as the library for every float, which the reasoning below and the
   SM64DS_RASTER_AB comparison both say:
     floor_to_int is (int)std::floor(f). The cast truncates toward zero, and
       for a negative f with a fraction the truncation is one above the floor,
       which the compare catches. A float too big for an int, or a NaN, casts
       to INT_MIN exactly as the old cast of the old floor did, and the
       INT_MIN test keeps it there instead of stepping past it.
     floor_exact is std::floor(f) itself, as a float, for the filtered
       sampler, which also needs the fraction. A float at or above 2^23 has no
       fraction and neither do infinities, so those and NaN come back as they
       went in; an f that is already whole comes back as f itself, which is
       what keeps -0.0 as -0.0. */
static inline int floor_to_int(float f) {
    const int i = static_cast<int>(f);
    return (i != INT_MIN && static_cast<float>(i) > f) ? i - 1 : i;
}

static inline float floor_exact(float f) {
    if (!(std::fabs(f) < 8388608.0f)) return f;
    const float t = static_cast<float>(static_cast<int>(f));
    if (t == f) return f;
    return t > f ? t - 1.0f : t;
}

// One texel coordinate under the DS wrap rules (GBATEK TEXIMAGE_PARAM 16-19):
// repeat clear = CLAMP to the edge texel; repeat set = wrap; flip on top of
// repeat mirrors every other tile. `repeat && !flip` is the exact expression
// the raster used before wrap modes existed, so nothing that binds through
// gx_bind_texture moves a pixel.
/* A POWER-OF-TWO SIZE WRAPS WITH A MASK (run perf2, lane RASTER). Every
   texture the DS itself can bind is 8 to 1024 texels on a side in powers of
   two, and for those `i & (size - 1)` is exactly the "i % size, then add size
   if negative" below it: two's complement makes the low bits of a negative
   number its non-negative remainder, INT_MIN included. The two integer divides
   this saves were on every textured pixel. A size that is not a power of two
   (an HD pack at 3x, say) keeps the divide. */
static int tex_coord_i(int i, int size, bool repeat, bool flip) {
    if (!repeat) return i < 0 ? 0 : (i >= size ? size - 1 : i);
    const bool pow2 = size > 0 && (size & (size - 1)) == 0;
    if (!flip) {
        if (pow2) return i & (size - 1);
        i %= size;
        return i < 0 ? i + size : i;
    }
    const int period = size * 2;
    if (pow2) {
        i &= period - 1;
    } else {
        i %= period;
        if (i < 0) i += period;
    }
    return i < size ? i : period - 1 - i;
}

/* The float entry, which is what the nearest sampler has always called. The
   body above used to be inline here; splitting the integer half out is what
   lets a filtered tap put each of its four texel indices through the SAME
   wrap, clamp and flip rule, per axis, instead of approximating it. Floor
   then the identical integer arithmetic: the nearest path samples the texel
   it always sampled. */
static int tex_coord(float f, int size, bool repeat, bool flip) {
    return tex_coord_i(floor_to_int(f), size, repeat, flip);
}

/* ---- THE FILTERED SAMPLER (run hd2) ---------------------------------------
   One texture level, so bilinear and trilinear can say "this size" without
   caring whether it came from the cache entry or its chain. */
struct TexLevel {
    const uint32_t *px;
    int w, h;
};

/* FOUR TAPS, THE HALF-TEXEL CENTRE CONVENTION, EVERY TAP THROUGH THE WRAP
   RULE. u and v arrive in this level's texel units with the texel CENTRE at
   x + 0.5, so the four neighbours of the sample point are found by stepping
   back half a texel and taking floor and floor+1 on each axis. Each of those
   four indices goes through tex_coord_i separately, per axis, which is the
   whole reason the integer half exists: a CLAMPED edge must clamp its own tap
   rather than reach round to the opposite edge, and a repeating floor must
   wrap rather than clamp, or the seam shows up exactly where the tiling meets.

   THE BLEND IS IN PREMULTIPLIED ALPHA, box4's reason above: a fully
   transparent texel has no colour to contribute, and weighting by alpha is
   what stops the black inside a cut-out from being dragged out along the
   fence, the leaf and the A3I5 gradient strips. The returned alpha is the
   plain bilinear average, so the sampler's own "alpha 0 means no pixel" test
   keeps working and an edge fades out rather than stepping out. */
static uint32_t sample_bilinear(const TexLevel &L, float u, float v,
                                bool rs, bool rt, bool fs, bool ft) {
    const float fu = u - 0.5f, fv = v - 0.5f;
    const float flu = floor_exact(fu), flv = floor_exact(fv);
    const int iu = static_cast<int>(flu), iv = static_cast<int>(flv);
    const float du = fu - flu, dv = fv - flv;
    const int x0 = tex_coord_i(iu, L.w, rs, fs);
    const int x1 = tex_coord_i(iu + 1, L.w, rs, fs);
    const int y0 = tex_coord_i(iv, L.h, rt, ft);
    const int y1 = tex_coord_i(iv + 1, L.h, rt, ft);
    const uint32_t p00 = L.px[(size_t)y0 * L.w + x0];
    const uint32_t p10 = L.px[(size_t)y0 * L.w + x1];
    const uint32_t p01 = L.px[(size_t)y1 * L.w + x0];
    const uint32_t p11 = L.px[(size_t)y1 * L.w + x1];
    const float w00 = (1.0f - du) * (1.0f - dv);
    const float w10 = du * (1.0f - dv);
    const float w01 = (1.0f - du) * dv;
    const float w11 = du * dv;
    const float a00 = (float)(p00 >> 24) * w00;
    const float a10 = (float)(p10 >> 24) * w10;
    const float a01 = (float)(p01 >> 24) * w01;
    const float a11 = (float)(p11 >> 24) * w11;
    const float asum = a00 + a10 + a01 + a11;
    if (asum <= 0.0f) return 0;
    const float inv = 1.0f / asum;
    auto ch = [&](int sh) {
        const float s = (float)((p00 >> sh) & 0xFF) * a00 +
                        (float)((p10 >> sh) & 0xFF) * a10 +
                        (float)((p01 >> sh) & 0xFF) * a01 +
                        (float)((p11 >> sh) & 0xFF) * a11;
        const int i = (int)(s * inv + 0.5f);
        return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
    };
    const int ai = (int)(asum + 0.5f);
    const uint32_t a = (uint32_t)(ai < 0 ? 0 : (ai > 255 ? 255 : ai));
    return (a << 24) | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}

/* WHAT A TRIANGLE NEEDS TO SAMPLE, resolved ONCE PER TRIANGLE PER FRAME and
   never per pixel: which cache entry the bound buffer belongs to, which two
   chain levels the surface's distance lands between, and how far between.
   Only filled under TextureFilter 2; bilinear needs none of it. */
struct TriTex {
    const TexEntry *e;    /* null: not a cache entry, so no chain -- bilinear */
    int lod;              /* the coarser-or-equal level, 0 is the full size */
    float frac;           /* 0..1 towards lod + 1 */
};

/* One level of a triangle's texture. Level 0 is the bound buffer itself, so a
   triangle whose buffer never entered the cache (the BMD harness path through
   gx_bind_texture) still samples correctly -- it simply has no chain and
   every level request lands on level 0, which makes trilinear fall back to
   bilinear for it rather than refusing to draw. */
static TexLevel tex_level(const TriTex &tt, const uint32_t *base, int w, int h,
                          int lvl) {
    if (lvl <= 0 || !tt.e || (size_t)lvl > tt.e->mip.size())
        return TexLevel{base, w, h};
    int lw = w, lh = h;
    for (int i = 0; i < lvl; ++i) {
        lw = lw > 1 ? lw / 2 : 1;
        lh = lh > 1 ? lh / 2 : 1;
    }
    return TexLevel{tt.e->mip[(size_t)lvl - 1].data(), lw, lh};
}

/* The pixel's colour under a filter mode, chosen at COMPILE time so the
   nearest body never carries a test for it. u and v are already in the bound
   buffer's own pixel units (the caller multiplied by GxTriangle::tex_scale).
   Trilinear blends the two chain levels the triangle landed between; with no
   chain, or with the triangle sitting at full size, the second tap is the
   same level and the lerp costs one multiply. */
template <int FILTER>
static uint32_t sample_filtered(const TriTex &tt, const uint32_t *base, int w,
                                int h, float u, float v, bool rs, bool rt,
                                bool fs, bool ft) {
    if (FILTER == 1) {
        const TexLevel L{base, w, h};
        return sample_bilinear(L, u, v, rs, rt, fs, ft);
    }
    const TexLevel L0 = tex_level(tt, base, w, h, tt.lod);
    const float s0 = (float)L0.w / (float)w, t0 = (float)L0.h / (float)h;
    const uint32_t c0 = sample_bilinear(L0, u * s0, v * t0, rs, rt, fs, ft);
    if (tt.frac <= 0.0f) return c0;
    const TexLevel L1 = tex_level(tt, base, w, h, tt.lod + 1);
    if (L1.px == L0.px) return c0;
    const float s1 = (float)L1.w / (float)w, t1 = (float)L1.h / (float)h;
    const uint32_t c1 = sample_bilinear(L1, u * s1, v * t1, rs, rt, fs, ft);
    /* THE TWO LEVELS BLEND IN PREMULTIPLIED ALPHA TOO, for sample_bilinear's
       reason: a coarse level of a cut-out is mostly transparent, and mixing
       its colour in unweighted would grey the sharp level's edge. */
    const float f1 = tt.frac, f0 = 1.0f - f1;
    const float a0 = (float)(c0 >> 24) * f0, a1 = (float)(c1 >> 24) * f1;
    const float asum = a0 + a1;
    if (asum <= 0.0f) return 0;
    const float inv = 1.0f / asum;
    auto ch = [&](int sh) {
        const float s = (float)((c0 >> sh) & 0xFF) * a0 +
                        (float)((c1 >> sh) & 0xFF) * a1;
        const int i = (int)(s * inv + 0.5f);
        return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
    };
    const int ai = (int)(asum + 0.5f);
    const uint32_t a = (uint32_t)(ai < 0 ? 0 : (ai > 255 ? 255 : ai));
    return (a << 24) | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}

/* COVERAGE BELONGS TO THE TEXEL THE DS WOULD HAVE PICKED (run hunt7, lane
   FILTEREDGE1). The filter's alpha is a blend, so a cut-out's rim pixel comes
   back with a PARTIAL alpha next to a transparent texel. Taken at face value
   that did two wrong things: the footprint grew by up to half a texel (a pixel
   whose own texel is transparent still passed the "alpha 0 means no pixel"
   test), and a pixel whose own texel is opaque went down the translucent path
   with its alpha scaled by the blend, which writes no depth, so whatever was
   behind it was free to paint over the sprite's edge. The card's pass had the
   worse form of the same thing (it blended the partial alpha over the clear
   colour AND wrote depth, so the rim showed what was drawn first).
   The rule here is the one TextureFilter 0 has always followed: the texel at
   the sample point decides whether the pixel exists (alpha 0: no pixel) and,
   when that texel is fully opaque, the pixel is fully opaque and writes depth.
   The filter only supplies the COLOUR of a pixel that exists. A texel with a
   partial alpha of its own (an A3I5 or A5I3 gradient) keeps the filtered alpha
   so its fade stays smooth. `nearest_px` is the nearest texel's 0xAARRGGBB. */
static inline uint32_t filtered_texel_cover(uint32_t filtered, uint32_t nearest_px) {
    const uint32_t na = nearest_px >> 24;
    if (na == 0) return 0;
    if (na == 0xFF) return filtered | 0xFF000000u;
    return (filtered >> 24) ? filtered : nearest_px;
}

/* THE LEVEL OF DETAIL, ONE PER TRIANGLE, from the ratio of its area in texels
   to its area in pixels. A triangle covering a quarter of the pixels its
   texels would fill is half the size in each direction, which is one level
   down; the square root of the area ratio is that factor and half its base-2
   logarithm is the level. Per triangle rather than per pixel deliberately:
   the derivative work a per-pixel level needs is two more divides and a
   logarithm on every pixel of every textured polygon, on a rasteriser that is
   already fill-bound, and the game's polygons are small.
   WITH AN HD PACK the areas are in the REPLACEMENT'S texels (tsc has already
   been applied), which is what makes the chain of a 4x image pick its own
   fourth level rather than the DS texture's first. */
static TriTex lod_for(const GxTriangle &t, float area2) {
    TriTex r{nullptr, 0, 0.0f};
    if (!t.tex) return r;
    auto f = g_tex_by_px.find(t.tex);
    if (f == g_tex_by_px.end() || f->second->mip.empty()) return r;
    r.e = f->second;
    const float tsc = (float)(t.tex_scale ? t.tex_scale : 1);
    const float du1 = (t.v[1].u - t.v[0].u) * tsc;
    const float dv1 = (t.v[1].v - t.v[0].v) * tsc;
    const float du2 = (t.v[2].u - t.v[0].u) * tsc;
    const float dv2 = (t.v[2].v - t.v[0].v) * tsc;
    /* BOTH AREAS ARE DOUBLED (a cross product, not a triangle area), which
       cancels in the ratio below. `screen2` is spelt out rather than shortened
       because the obvious short spelling collides with a Windows common-dialog
       control id from the SDK's dlgs.h, which this file reaches through
       windows.h. */
    const float tex2 = std::fabs(du1 * dv2 - dv1 * du2);
    const float screen2 = std::fabs(area2);
    if (tex2 <= 0.0f || screen2 <= 1e-6f) return r;
    const float lodf = 0.5f * std::log2(tex2 / screen2);
    const int top = (int)r.e->mip.size();
    if (lodf <= 0.0f) return r;                 /* level 0, no blend */
    if (lodf >= (float)top) { r.lod = top; r.frac = 0.0f; return r; }
    r.lod = (int)lodf;
    r.frac = lodf - (float)r.lod;
    return r;
}

/* One entry per triangle of the frame, filled once at the head of gx_render
   and read-only while the raster bands run, so every thread shares one answer
   per triangle instead of each looking the chain up for itself. Only touched
   under TextureFilter 2; it never grows under any other mode. */
static std::vector<TriTex> g_tritex;

/* n/255.0f for every byte. The texel-modulate step did this divide three
   times per pixel; the table holds the identical float, so the product is
   bit-for-bit what the divide produced. */
struct Inv255 {
    float v[256];
    Inv255() { for (int i = 0; i < 256; ++i) v[i] = i / 255.0f; }
};
static const Inv255 inv255;

/* --- parallel raster -------------------------------------------------------
   The screen is split BY ROW into tiles of whole rows (see the bins beside
   gx_render; before run perf2 worker k took rows k, k+T, k+2T and so on).
   Every row is written by exactly one worker, and every worker walks its
   triangles in submission order, so the depth resolution and the
   translucent blend over the framebuffer happen in the same order, against the
   same pixels, as they did on one thread. The frame that comes out is
   bit-for-bit the frame one thread produced: this partitions the work, it does
   not reorder it. That is the only reason it is allowed to exist here -- the
   raster is the port's reference for what the DS drew.

   SM64DS_RASTER_THREADS=N forces the worker count; 1 turns threading off. */
/* PCORE1: set by the exe at boot (hal/pcore.h); each raster worker calls it once
   as it starts so Windows does not run it throttled on an efficiency core. */
void (*gx_worker_thread_hook)(void) = nullptr;

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
        if (gx_worker_thread_hook) gx_worker_thread_hook();
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

/* run after093, lane MENU2: the threads this process can actually run on: the affinity mask's bits, capped by the machine's hardware
   threads (a launcher, a job object or `start /affinity` can leave a 12-thread machine two cores) */
unsigned usable_threads() {
    static unsigned n = 0;
    if (!n) {
        const unsigned hc = std::thread::hardware_concurrency();
        DWORD_PTR pm = 0, sm = 0;
        unsigned pc = 0;
        if (GetProcessAffinityMask(GetCurrentProcess(), &pm, &sm))
            for (; pm; pm &= pm - 1) ++pc;
        n = pc ? pc : hc;
        if (hc && n > hc) n = hc;
        if (!n) n = 1;
    }
    return n;
}

int raster_threads() {
    static int n = -1;
    if (n < 0) {
        if (const char *e = getenv("SM64DS_RASTER_THREADS")) {
            n = atoi(e);
        } else {
            unsigned hc = usable_threads();
            n = hc ? (int)hc : 1;
            if (n > 8) n = 8;   /* past this the row bands stop paying */
            /* SMALL MACHINES GET TWO WORKERS A CORE, AT LEAST FOUR (run perf2,
               lane RASTER). Measured on a 6-core box pinned to two cores
               with the old interleaved bands: two bands 12.2 ms of raster a
               frame, four 7.4, eight 7.5. One worker a core leaves a core
               idle whenever the other is descheduled or its rows run long,
               and hardware_concurrency is exactly what a two-core laptop
               reports. The rows a worker draws do not depend on how many
               workers there are, so this moves time and never a pixel. */
            if (n < 4) {
                n = hc ? 2 * (int)hc : 4;
                if (n < 4 && hc >= 2) n = 4;
            }
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

/* ---- THE PER-PIXEL RASTER BUFFERS LIVE ON THE HEAP ------------------------
 *
 * Five buffers, one entry per pixel of the ALLOCATION (SCREEN_W x SCREEN_H,
 * the largest extent the settings can ask for): the depth buffer, the 3D
 * coverage mask, the shadow stencil, the polygon-id plane and the translucent
 * attribute plane. Twelve bytes a pixel, and they used to be static arrays.
 *
 * THEY HAD TO COME OFF .bss, AND THE REASON IS AN ADDRESS AND NOT A SIZE.
 * ntr/io.cpp reserves 02000000..02400000 at process start because that is the
 * DS's own main RAM and the ROM's code holds pointers into it. A 32-bit image
 * based at 0x400000 therefore has to END below 0x02000000, and the port was
 * already using about 26 MB of that 28 MB. Growing this tier's allocation to
 * 1368x768 for RenderScale 4 added roughly 9 MB of .bss, the image ran on to
 * 0x02226000, and io.cpp refused the reservation and said so in plain words:
 * "LOST 02000000..02400000 main memory ... 02000000..02226000 committed ->
 * walk_window.exe". Every run then died at 0xC0000409 before a frame.
 *
 * Heap memory has no such constraint -- Windows hands these out well above the
 * DS window, and ntr has already reserved that window by the time anything
 * calls gx_render. So the buffers move and the IMAGE SHRINKS: 12 bytes a pixel
 * of .bss go away, which is more than the growth from 1024x576 to 1368x768
 * costs, and the port has more room under the ceiling than it started with.
 *
 * THE ROW TYPEDEFS ARE THE POINT OF THE SHAPE. `DepthRow *` indexes exactly
 * like `float [][SCREEN_W]` does, so depth[y], depth[y][x] and
 * memcpy(depth[y], ...) are the expressions they always were and not one use
 * site in the raster changed. The stride is still the compile-time SCREEN_W,
 * which is what every band and every bounds clamp already assumes.
 *
 * Allocated ONCE, on the first call that needs them, and never freed: they are
 * live for as long as the program draws. calloc rather than malloc so the
 * first frame reads zeros out of the coverage and attribute planes exactly as
 * a .bss array gave it, and so the pages are the OS's zero pages until they
 * are written.
 */
typedef float DepthRow[SCREEN_W];
typedef uint8_t MaskRow[SCREEN_W];

DepthRow *g_depth;
MaskRow *g_cover;
MaskRow *g_stencil;
MaskRow *g_attrid;
MaskRow *g_tlattr;

/* True once every buffer is there. A refusal is FATAL and says so: a renderer
   with nowhere to put a depth value cannot draw a frame, and a program that
   carried on would fault somewhere else entirely and look like a render bug.
   About 12.6 MB at this tier's allocation, so in practice this does not fail
   on any machine that can open the window. */
/* ---- THE EDGE-SMOOTHING PASS'S OWN BUFFER (run hd2) ----------------------
 * The pass reads the picture and writes the picture, so it cannot read what
 * it has already written: a filter fed on its own output smears instead of
 * smoothing. One scratch copy of the frame solves it -- read from the copy,
 * write to the framebuffer -- and it is the reason this is a buffer and not a
 * three-row window: the raster's rows are split between threads (tiles of
 * whole rows since run perf2, interleaved rows before it), so a row's
 * neighbours can always be another thread's rows.
 * ON THE HEAP for the raster buffers' reason one block down, and allocated
 * only on the first frame the setting is actually on, so a run with the key
 * absent holds nothing. */
uint32_t *g_aa_src;

/* AND IT IS ALSO THE PICTURE THE GAME READS BACK. See gx_aa_preimage below:
   once the pass has filled it, g_aa_src holds the frame EXACTLY as it would
   have been with the setting off, and the 2D compositor mirrors its own
   writes into it, so the display capture can read a finished frame that the
   smoothing never touched. Valid only between the pass and the next
   gx_render. */
int g_aa_pre_valid;

int g_aa_mode = 0;                     /* 0 off, 1 edge smoothing */
unsigned long long g_aa_changed;       /* pixels rewritten, whole run */
unsigned long long g_aa_run_frames;    /* frames the pass ran on */
unsigned long long g_cap_missed;       /* captures that read a smoothed frame */
unsigned g_aa_hits[64];                /* per band, summed after each pass */

bool raster_buffers(void)
{
    if (g_depth) return true;
    g_depth = (DepthRow *)std::calloc(SCREEN_H, sizeof(DepthRow));
    g_cover = (MaskRow *)std::calloc(SCREEN_H, sizeof(MaskRow));
    g_stencil = (MaskRow *)std::calloc(SCREEN_H, sizeof(MaskRow));
    g_attrid = (MaskRow *)std::calloc(SCREEN_H, sizeof(MaskRow));
    g_tlattr = (MaskRow *)std::calloc(SCREEN_H, sizeof(MaskRow));
    if (g_depth && g_cover && g_stencil && g_attrid && g_tlattr) return true;
    std::fprintf(stderr,
                 "FATAL: the 3D renderer could not get its %d x %d buffers "
                 "(about %.1f MB). There is not enough memory to draw.\n",
                 SCREEN_W, SCREEN_H,
                 (double)SCREEN_W * SCREEN_H * 12.0 / (1024.0 * 1024.0));
    std::fflush(stderr);
    std::exit(3);
}

}  // namespace

/* run after093, lane MENU2: the A/B phase and the row-band helper (see gx.h) */
static int g_menu2_phase;
void gx_menu2_phase_set(int phase) { g_menu2_phase = phase ? 1 : 0; }
int gx_menu2_phase_get(void) { return g_menu2_phase; }
int gx_menu_knob_value(const char *env)
{
    const char *e = std::getenv(env);
    if (!e || !*e) return 1;
    if (e[0] == '0' && !e[1]) return 0;
    if (e[0] == '2' && !e[1]) return 2;
    return 1;
}
int gx_menu_knob_on(int value) { return value == 2 ? g_menu2_phase : (value != 0 ? 1 : 0); }
namespace {
struct ParRows { void (*fn)(void *, int, int); void *ctx; int rows; };
void par_band(void *c, int id, int n)
{
    const ParRows *p = (const ParRows *)c;
    const int y0 = (int)((long long)p->rows * id / n), y1 = (int)((long long)p->rows * (id + 1) / n);
    if (y1 > y0) p->fn(p->ctx, y0, y1);
}
}  // namespace
/* the bands need real cores: a machine with fewer than four hardware threads runs the rows itself (its raster pool of four is for
   the CPU raster, which has nothing else to do while it waits) */
static int g_par_serial;
void gx_parallel_serial(int on) { g_par_serial = on ? 1 : 0; }
static bool par_cores_ok()
{
    static int ok = -1;
    if (ok < 0) ok = usable_threads() >= 4 ? 1 : 0;
    return ok != 0;
}
void gx_parallel_rows(int rows, int min_rows, void (*fn)(void *, int, int), void *ctx, int knob_value)
{
    /* the pool belongs to the thread that first asked (the main thread: the scene tick and the pacer); any other thread's rows run on
       that thread itself, never through the pool */
    static DWORD owner;
    const DWORD me = GetCurrentThreadId();
    if (!owner) owner = me;
    if (me != owner) { fn(ctx, 0, rows); return; }
    const int threads = raster_threads();
    if (rows < min_rows || threads <= 1 || !par_cores_ok() || g_par_serial || !gx_menu_knob_on(knob_value)) { fn(ctx, 0, rows); return; }
    /* the pool runs one job at a time: a second caller (or a band that calls back in) does its rows itself */
    static std::mutex par_m;
    if (!par_m.try_lock()) { fn(ctx, 0, rows); return; }
    ParRows p = { fn, ctx, rows };
    pool(threads).run(par_band, &p);
    par_m.unlock();
}
int gx_parallel_available(void)
{
    static int knob = -2;
    if (knob == -2) knob = gx_menu_knob_value("SM64DS_MENU_PAR");
    return raster_threads() >= 4 && par_cores_ok() && gx_menu_knob_on(knob) ? 1 : 0;
}

/* THE DEPTH AND POLYGON-ID PLANES AFTER A FRAME THE CARD DREW WHOLE (run
   cardfull). That pass does not bring them back, because nothing in a game
   reads them; the one reader that exists (tests/smoke_gpu_raster.cpp) asks
   through the two accessors below, and the first ask after such a frame clears
   the two planes and has the backend fill them from the card's own targets,
   which still hold that frame. gx_render drops the flag at its head. */
static int g_planes_stale;
static void planes_if_stale()
{
    if (!g_planes_stale) return;
    g_planes_stale = 0;
    if (!g_gpu_opaque) return;
    const int cw = active_w > 0 ? (active_w < SCREEN_W ? active_w : SCREEN_W) : 0;
    const int ch = active_h > 0 ? (active_h < SCREEN_H ? active_h : SCREEN_H) : 0;
    for (int y = 0; y < ch; ++y) {
        for (int x = 0; x < cw; ++x) g_depth[y][x] = 1e30f;
        std::memset(g_attrid[y], 0, (size_t)cw);
    }
    GxGpuFrame f;
    std::memset(&f, 0, sizeof f);
    f.planes = 1;
    f.depth = &g_depth[0][0];
    f.attrid = &g_attrid[0][0];
    f.cover = &g_cover[0][0];
    f.stride = SCREEN_W;
    f.cw = cw;
    f.ch = ch;
    g_gpu_opaque(&f);
}

const uint8_t *gx_coverage()
{
    /* a frame held on the card has no coverage plane on the processor: any
       reader that wants it makes the frame an ordinary one first */
    gx_card_settle_because("the coverage plane was asked for");
    raster_buffers();
    return &g_cover[0][0];
}

/* THE OTHER TWO PLANES THE OPAQUE PASS FILLS, on the same contract as the
   coverage mask above: SCREEN_W stride, SCREEN_H rows, valid immediately
   after gx_render and cleared at the head of the next one. They exist for
   tests/smoke_gpu_raster.cpp, which asks the graphics-card pass questions a
   picture cannot answer -- did the nearer triangle win, was the depth left
   alone, did the polygon id arrive -- and every one of those is a value in
   one of these two planes rather than a colour. Read-only. */
const uint8_t *gx_attr_ids()
{
    raster_buffers();
    planes_if_stale();
    return &g_attrid[0][0];
}

const float *gx_depth()
{
    raster_buffers();
    planes_if_stale();
    return &g_depth[0][0];
}

/* ---- THE FRAME'S CLEAR AND THE TOON TABLE (run hunt2, lane RENDER2) --------
   Both are plain latches of the 3D engine that the game stores straight into
   the mapped I/O window and the hardware's rendering engine reads at draw time;
   see the two blocks over gx_clear_argb and gx_toon_table in ntr/gx.h for the
   GBATEK rules. Everything here reads the window only once io_ready() says it
   is mapped, so a harness that draws without io_init keeps the old answers. */
namespace {

/* the constant the port cleared to before the clear colour was read, kept for
   the pillarbox margins, the unmapped case and SM64DS_CLEAR_FILL_OLD */
constexpr uint32_t kOldClear = 0xFF101820u;

/* THIS FRAME'S TOON TABLE, as 0..255 floats in the units the vertex colour
   travels in (bgr555_to_argb's expansion, so an entry reads exactly as the
   same BGR555 value would as a vertex colour), and the shading it is for:
   0 no mode-2 polygon this frame (or SM64DS_TOON_OFF), 1 toon, 2 highlight.
   Written once at the head of gx_render, on the calling thread, before any
   band starts; the bands only read it. */
float g_toon8[32 * 3];
int g_toon_shade = 0;

/* THE ENTRY A PIXEL TAKES, from its interpolated vertex red (0..255). The
   hardware interpolates the colour at six bits and indexes with the top five
   (melonDS RenderPixel: RenderToonTable[vr >> 1]), so a vertex whose red is
   the 5-bit value r lands on entry r, and between two vertices the step falls
   half way. Rounding red * 31 / 255 is that: bgr555_to_argb's expansion of r
   is within 0.06 of r * 255 / 31, so every vertex value reads back as its own
   r with room to spare, and the boundary between r and r + 1 is their
   midpoint. */
inline int toon_index(float vr)
{
    const int i = static_cast<int>(vr * (31.0f / 255.0f) + 0.5f);
    return i < 0 ? 0 : (i > 31 ? 31 : i);
}

/* SMOOTH TOON SHADING (run after093, lane SMOOTHTOON1; the Smooth Models
   setting). The nearest entry above is exact at the DS's 256x192, where one
   entry spans a pixel or two of a head; at a high render scale the same entry
   is a wide flat band and the bands read as contour lines (the title's big
   Mario head). With Smooth Models on, a pixel instead blends the two entries
   on either side of its fractional index, so the table's own colours stay and
   only the steps between them go soft.

   g_toon_blend[k] (k 0..30) is 1 where entries k and k + 1 blend and 0 where
   they keep their hard edge. THE RULE, from the tables the ROM was seen to
   load (out/SMOOTHTOON1 tables.tsv: the title head's grey ramp, the
   Luigi / Wario / Yoshi ramp, the cap-change rainbow and shimmer, the title's
   highlight fade): the step is the largest change on any channel in 5-bit
   levels; a step of up to 3 levels always blends (every ramp seen steps by 1
   to 3), a step of more than 8 never does (a deliberate colour change, like
   the Luigi ramp's red going 22 -> 31), and a step in between blends unless
   the entries on one side of it are a run of 3 or more equal colours (a cel
   table's flat bands, like the rainbow's runs of 4; the highlight fade's 5 to
   7 level steps sit inside a smooth curve and blend). g_toon_blend[31]
   is the switch itself (1 smooth, 0 the DS's nearest entry, every blend flag
   then 0 too). The card path reads the same 32 floats (GxGpuFrame toon_blend).
   An unblended gap picks exactly the entry toon_index picks, and with the
   switch off nothing here runs. */
float g_toon_blend[32];
int g_toon_smooth = 0;
/* the two step limits above, in the 0..255 units of g_toon8 (one 5-bit level
   is 255 / 31 = 8.2 of them), with half a level of slack */
constexpr float kToonRampStep = 3.5f * 255.0f / 31.0f;
constexpr float kToonEdgeStep = 8.5f * 255.0f / 31.0f;

/* the blended entry for an interpolated red, into out[3] */
inline void toon_smooth_entry(float vr, float out[3])
{
    float f = vr * (31.0f / 255.0f);
    f = f < 0.0f ? 0.0f : (f > 31.0f ? 31.0f : f);
    int i0 = static_cast<int>(f);
    if (i0 > 30) i0 = 30;
    const float t = f - static_cast<float>(i0);
    const float *a = &g_toon8[i0 * 3];
    const float *b = a + 3;
    if (g_toon_blend[i0] != 0.0f) {
        out[0] = a[0] + (b[0] - a[0]) * t;
        out[1] = a[1] + (b[1] - a[1]) * t;
        out[2] = a[2] + (b[2] - a[2]) * t;
    } else {
        const float *e = t < 0.5f ? a : b;
        out[0] = e[0]; out[1] = e[1]; out[2] = e[2];
    }
}

/* Smooth Models on (any level), or SM64DS_TOON_SMOOTH=0 / 1 to force it */
int toon_smooth_wanted()
{
    static int env = -2;
    if (env == -2) {
        const char *e = std::getenv("SM64DS_TOON_SMOOTH");
        env = (e && e[0]) ? (e[0] == '0' ? 0 : 1) : -1;
    }
    if (env >= 0) return env;
    return smooth_level() > 0 ? 1 : 0;
}

/* this frame's blend flags from this frame's table (g_toon8, g_toon_shade) */
inline bool toon_same(int a, int b)
{
    return g_toon8[a * 3] == g_toon8[b * 3] && g_toon8[a * 3 + 1] == g_toon8[b * 3 + 1] &&
           g_toon8[a * 3 + 2] == g_toon8[b * 3 + 2];
}

void toon_blend_setup()
{
    g_toon_smooth = (g_toon_shade && toon_smooth_wanted()) ? 1 : 0;
    for (int k = 0; k < 31; ++k) {
        bool blend = false;
        if (g_toon_smooth) {
            float d = 0.0f;
            for (int c = 0; c < 3; ++c) {
                const float x = g_toon8[(k + 1) * 3 + c] - g_toon8[k * 3 + c];
                const float ax = x < 0.0f ? -x : x;
                if (ax > d) d = ax;
            }
            if (d <= kToonRampStep) {
                blend = true;
            } else if (d <= kToonEdgeStep) {
                /* a flat band of 3 or more on either side = a cel edge */
                const bool flat_lo = k >= 2 && toon_same(k, k - 1) && toon_same(k, k - 2);
                const bool flat_hi = k <= 28 && toon_same(k + 1, k + 2) && toon_same(k + 1, k + 3);
                blend = !flat_lo && !flat_hi;
            }
        }
        g_toon_blend[k] = blend ? 1.0f : 0.0f;
    }
    g_toon_blend[31] = g_toon_smooth ? 1.0f : 0.0f;
}

int toon_off_env()
{
    static const int v = std::getenv("SM64DS_TOON_OFF") ? 1 : 0;
    return v;
}

}  // namespace

int gx_toon_table(float rgb[32 * 3])
{
    if (toon_off_env() || !io_ready()) return 0;
    const volatile uint16_t *tt =
        reinterpret_cast<const volatile uint16_t *>(0x04000380u);
    for (int i = 0; i < 32; ++i) {
        const uint32_t c = bgr555_to_argb(static_cast<uint16_t>(tt[i] & 0x7FFF));
        rgb[i * 3 + 0] = (float)((c >> 16) & 0xFF);
        rgb[i * 3 + 1] = (float)((c >> 8) & 0xFF);
        rgb[i * 3 + 2] = (float)(c & 0xFF);
    }
    /* DISP3DCNT bit 1: 0 toon, 1 highlight */
    const uint16_t d3 = *reinterpret_cast<const volatile uint16_t *>(0x04000060u);
    return (d3 & 2u) ? 2 : 1;
}

uint32_t gx_clear_argb()
{
    static const int old = std::getenv("SM64DS_CLEAR_FILL_OLD") ? 1 : 0;
    if (old || !io_ready()) return kOldClear;
    const uint32_t cc = *reinterpret_cast<const volatile uint32_t *>(0x04000350u);
    const uint32_t dc = *reinterpret_cast<const volatile uint32_t *>(0x04000000u);
    const uint16_t bd = *reinterpret_cast<const volatile uint16_t *>(0x05000000u);
    /* BG0 is the 3D layer and it is on: the rule message_compositor.cpp's
       bg0_3d_shown uses for the same question */
    const bool shown3d = (dc & 0x8u) != 0 && ((dc >> 8) & 1u) != 0;
    const bool opaque = shown3d && ((cc >> 16) & 0x1Fu) != 0;
    /* BG0 IS THE 3D LAYER AND ITS CLEAR IS TRANSPARENT: keep the old constant.
       On the DS a translucent polygon over an alpha-0 rear plane is stored
       unblended and the 2D engine blends it, by its 3D alpha, onto the layer
       under BG0 (MG_TRAMPOLINE2's shooting-star trails onto the starfield,
       G2x::SetBlendAlpha(1, 0x2e, 16, 16)). The raster here blends it onto
       this fill instead, so filling with the backdrop turned those trails into
       white rings. Until the compositor blends the 3D alpha onto the layers
       below, this arm keeps the picture it always had. */
    const uint32_t out = (shown3d && !opaque) ? kOldClear : bgr555_to_argb(
        static_cast<uint16_t>((opaque ? cc : (uint32_t)bd) & 0x7FFFu));
    static const int probe = std::getenv("SM64DS_CLEAR_PROBE") ? 1 : 0;
    if (probe) {
        static uint32_t last_cc = ~0u, last_dc = ~0u;
        static uint32_t last_bd = ~0u, n = 0;
        static unsigned long calls = 0;
        ++calls;
        if ((cc != last_cc || dc != last_dc || bd != last_bd) && n < 64) {
            ++n;
            last_cc = cc; last_dc = dc; last_bd = bd;
            std::fprintf(stderr,
                         "[clear] call %lu CLEAR_COLOR %08x DISPCNT %08x "
                         "backdrop %04x DISP3DCNT %04x -> fill %08x (%s)\n",
                         calls, (unsigned)cc, (unsigned)dc, (unsigned)bd,
                         (unsigned)*reinterpret_cast<const volatile uint16_t *>(
                             0x04000060u),
                         (unsigned)out,
                         opaque ? "the clear colour, alpha nonzero"
                                : (shown3d ? "the old constant, clear alpha 0"
                                           : "the backdrop, no 3D layer"));
        }
    }
    return out;
}

/* THE RASTER'S CLIP RECTANGLE, inclusive host pixels: the present rectangle
   narrowed to the game's VIEWPORT. The 3D engine cannot put a pixel outside
   the viewport (the polygon is clipped to the viewing volume, which the
   viewport maps onto exactly that rectangle), and this raster has no side clip
   of its own, so the box clamp is where the letterbox of the ending's Peach
   scenes comes from. A full-screen viewport IS the present rectangle (vp_x /
   vp_w are built from present_*), so every frame that never narrowed the
   viewport clamps to the same four numbers as before. An empty or inverted
   rectangle is ignored rather than drawn as nothing. */
static void raster_clip(int &x0, int &y0, int &x1, int &y1)
{
    x0 = present_x(); y0 = present_y();
    x1 = x0 + present_w() - 1; y1 = y0 + present_h() - 1;
    if (menu_wide()) { x0 = 0; y0 = 0; x1 = active_w - 1; y1 = active_h - 1; }   /* MENU WIDE 3D */
    if (g.vp_w <= 0 || g.vp_h <= 0) return;
    const int vx1 = g.vp_x + g.vp_w - 1, vy1 = g.vp_y + g.vp_h - 1;
    if (g.vp_x > x0) x0 = g.vp_x;
    if (g.vp_y > y0) y0 = g.vp_y;
    if (vx1 < x1) x1 = vx1;
    if (vy1 < y1) y1 = vy1;
    if (x1 < x0 || y1 < y0) {
        x0 = present_x(); y0 = present_y();
        x1 = x0 + present_w() - 1; y1 = y0 + present_h() - 1;
        if (menu_wide()) { x0 = 0; y0 = 0; x1 = active_w - 1; y1 = active_h - 1; }
    }
}
/* THE PICTURE-ON-THE-CARD STATE (run cardfull, lane CARD2; see ntr/gx.h).
   g_cp_allow is the caller's one-frame permission, g_cp_pending is set by the
   fill below until gx_render has either kept the frame on the card or given
   up, g_cp_active is the frame being held, and g_cp_geo is what a later
   settle needs to ask the card for it. */
static int g_cp_allow;
static int g_cp_pending;
static int g_cp_active;
static Framebuffer *g_cp_fb;
static struct { int cw, ch, px0, py0, pw, ph; } g_cp_geo;
static uint32_t *g_cp_scratch;      /* the capture's picture, SCREEN_W stride */
/* run hunt8, lane CARDSM1: SmoothMotion with a held tick. The settle's sink for
   the 3D layer, the caller's note of the settle, the 2D-behind-3D flag, the
   frame's pre-smoothing-copy flag and the framebuffer whose finished picture
   lives in the composite target (see gx.h). */
static uint32_t *g_cp_sink;
static void (*g_cp_sink_done)(void);
static int g_cp_behind;
static int g_cp_pre_needed;
static const Framebuffer *g_ret_fb;
/* run hunt8, lane GPUINTERP1: the framebuffer standing for a picture between two
   ticks whose finished picture lives in the in-between composite target */
static const Framebuffer *g_ret_fb2;

void gx_card_present_allow(int allow) { g_cp_allow = allow ? 1 : 0; }
void gx_card_settle_sink(uint32_t *dst3d, void (*done)(void))
{
    g_cp_sink = dst3d;
    g_cp_sink_done = done;
}
void gx_card_note_behind() { g_cp_behind = 1; }
int gx_card_behind_seen() { return g_cp_active && g_cp_behind; }
int gx_card_pre_needed() { return g_cp_active && g_cp_pre_needed; }
void gx_card_retire(const Framebuffer *fb)
{
    /* the tick is composited: nothing of it is held for a later settle */
    g_cp_active = 0;
    g_cp_sink = nullptr;
    g_cp_sink_done = nullptr;
    g_ret_fb = fb;
}
int gx_card_retained(const Framebuffer *fb)
{
    if (!fb) return 0;
    return fb == g_ret_fb ? 1 : (fb == g_ret_fb2 ? 2 : 0);
}
void gx_card_retire2(const Framebuffer *fb)
{
    /* a picture between two ticks is composited: nothing of it is held */
    g_cp_active = 0;
    g_cp_sink = nullptr;
    g_cp_sink_done = nullptr;
    g_ret_fb2 = fb;
}
int gx_card_overlay_active() { return g_cp_active; }

/* MENUS093C (D23): THE HELD FRAME UNDER A TRANSLUCENT 3D PAGE. The title's face page hands over to the drawing page with
   BG mode 3 and BG3 a direct-colour bitmap (BG3CNT bits 7 and 2: the capture of the face frame), BG0 the 3D layer at the same
   priority (so BG0 is in front) and a clear alpha of 0. The page's paper polygon ramps its alpha from 1, and the DS blends it onto
   BG3, so the face fades into the paper. The raster blends a translucent polygon onto the fill it was given and the fill was the
   navy 0xFF101820, so the picture went white, navy, then grey. This fills the frame with the bitmap instead, so the same blend lands
   on the held frame. Only this exact register state asks for it; SM64DS_BGBMP_CLEAR_OFF=1 puts the old fill back. */
namespace {
struct BmpClear {
    uint32_t px[256 * 192];       /* the bitmap as the screen shows it, 0 = transparent (the backdrop) */
    int valid;
};
BmpClear g_bmpc;

int bgbmp_clear_off_env()
{
    static const int v = std::getenv("SM64DS_BGBMP_CLEAR_OFF") ? 1 : 0;
    return v;
}

/* 1 and the 256 x 192 picture in g_bmpc when the registers say a held frame sits under the 3D layer */
int bgbmp_clear_wanted()
{
    g_bmpc.valid = 0;
    if (bgbmp_clear_off_env() || !io_ready()) return 0;
    const uint32_t dc = *reinterpret_cast<const volatile uint32_t *>(0x04000000u);
    const uint32_t cc = *reinterpret_cast<const volatile uint32_t *>(0x04000350u);
    if ((dc & 7u) != 3u || !(dc & 8u) || !((dc >> 8) & 1u) || !((dc >> 11) & 1u)) return 0;
    if (((cc >> 16) & 0x1Fu) != 0) return 0;                  /* an opaque clear colour is the picture's own */
    const uint16_t c0 = *reinterpret_cast<const volatile uint16_t *>(0x04000008u);
    const uint16_t c3 = *reinterpret_cast<const volatile uint16_t *>(0x0400000Eu);
    if (!(c3 & 0x80u) || !(c3 & 4u)) return 0;                /* not a direct-colour bitmap */
    if ((c3 & 3u) < (c0 & 3u)) return 0;                      /* BG3 would be in front of the 3D layer */
    static const int wh[4][2] = { { 128, 128 }, { 256, 256 }, { 512, 256 }, { 512, 512 } };
    const int bw = wh[(c3 >> 14) & 3][0], bh = wh[(c3 >> 14) & 3][1];
    const uint32_t base = 0x06000000u + (((dc >> 27) & 7u) << 16) + (((uint32_t)(c3 >> 8) & 0x1Fu) << 14);
    const volatile int16_t *m = reinterpret_cast<const volatile int16_t *>(0x04000030u);
    const int pa = m[0], pb = m[1], pc = m[2], pd = m[3];
    int32_t rx = *reinterpret_cast<const volatile int32_t *>(0x04000038u);
    int32_t ry = *reinterpret_cast<const volatile int32_t *>(0x0400003Cu);
    rx = (rx << 4) >> 4; ry = (ry << 4) >> 4;                 /* 28 bit signed */
    const bool wrap = (c3 >> 13) & 1;
    const uint16_t bd = *reinterpret_cast<const volatile uint16_t *>(0x05000000u);
    const uint32_t bdc = bgr555_to_argb((uint16_t)(bd & 0x7FFFu));
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x) {
            int tx = (int)((rx + (int64_t)pa * x + (int64_t)pb * y) >> 8);
            int ty = (int)((ry + (int64_t)pc * x + (int64_t)pd * y) >> 8);
            uint32_t out = bdc;
            bool in = true;
            if (wrap) { tx &= bw - 1; ty &= bh - 1; }
            else if (tx < 0 || ty < 0 || tx >= bw || ty >= bh) in = false;
            if (in) {
                const uint16_t v = *reinterpret_cast<const volatile uint16_t *>(base + ((uint32_t)ty * bw + tx) * 2);
                if (v & 0x8000u) out = bgr555_to_argb((uint16_t)(v & 0x7FFFu));
            }
            g_bmpc.px[y * 256 + x] = out;
        }
    g_bmpc.valid = 1;
    if (const char *tr = std::getenv("SM64DS_BGBMP_TRACE")) {       /* proof tooling: what the held frame is, and a picture of it every 8th frame it is up */
        static int n = 0;
        int opq = 0; unsigned long long sr = 0, sg = 0, sb = 0;
        for (int i = 0; i < 256 * 192; ++i) {
            const uint16_t v = *reinterpret_cast<const volatile uint16_t *>(base + (uint32_t)i * 2);
            opq += (v >> 15) & 1; sr += (g_bmpc.px[i] >> 16) & 255; sg += (g_bmpc.px[i] >> 8) & 255; sb += g_bmpc.px[i] & 255;
        }
        std::fprintf(stderr, "[bgbmp] frame %d base %08x %dx%d opaque %d of 49152 mean %llu %llu %llu pa %d pd %d rx %d ry %d wrap %d\n", n, base, bw, bh, opq,
                     sr / 49152, sg / 49152, sb / 49152, pa, pd, (int)rx, (int)ry, (int)wrap);
        if ((n++ & 7) == 0 && *tr == '2') {
            char name[260]; std::snprintf(name, sizeof name, "bgbmp_%03d.bmp", n);
            if (FILE *f = std::fopen(name, "wb")) {
                unsigned char hd[54] = { 'B', 'M' }; const unsigned sz = 54 + 256 * 192 * 3;
                std::memcpy(hd + 2, &sz, 4); hd[10] = 54; hd[14] = 40; const int w = 256, h = -192; std::memcpy(hd + 18, &w, 4); std::memcpy(hd + 22, &h, 4);
                hd[26] = 1; hd[28] = 24;
                std::fwrite(hd, 1, 54, f);
                for (int i = 0; i < 256 * 192; ++i) { const unsigned char px[3] = { (unsigned char)g_bmpc.px[i], (unsigned char)(g_bmpc.px[i] >> 8), (unsigned char)(g_bmpc.px[i] >> 16) }; std::fwrite(px, 1, 3, f); }
                std::fclose(f);
            }
        }
    }
    return 1;
}
}  // namespace

static void clear_fill_classic(Framebuffer &fb);

/* run after093, lane MENU2: LATE CLEARS. gx_clear_fill_late leaves fb and the coverage plane uncleared; the clears run
   (late_run) from the card backend after its draw is on the card, from its deferred finish, or from gx_render when the card
   declined the frame. One LateCtx per frame in flight (two pending readbacks and the one being drawn). */
struct LateCtx { Framebuffer *fb; int cw, ch; bool ran; };
static LateCtx g_late_ctx[4];
static unsigned g_late_i;
static struct { bool pending; Framebuffer *fb; } g_late;
static bool g_late_req;
static int g_defer_req;
static void (*g_flush_hook)(void);

static void clear_fill_impl(Framebuffer &fb, bool wipe);
void gx_clear_fill(Framebuffer &fb) { clear_fill_impl(fb, true); }
void gx_clear_fill_late(Framebuffer &fb) { g_late_req = true; clear_fill_impl(fb, true); g_late_req = false; }
void gx_defer_request(int on) { g_defer_req = on ? 1 : 0; }
static int g_render_deferred;
int gx_render_deferred(void) { return g_render_deferred; }
void gx_set_flush_hook(void (*fn)(void)) { g_flush_hook = fn; }

/* run hunt8, lane GPUINTERP1: the clear for a picture between two ticks that
   the card is to keep. The same answer as gx_clear_fill after
   gx_card_present_allow(1), except that a kept frame's framebuffer is not wiped
   (nobody reads it: the composite owns the picture). 1 when the card will keep
   the frame; 0 and the classic clear fill otherwise. */
int gx_card_interp_clear(Framebuffer &fb)
{
    g_cp_allow = 1;
    clear_fill_impl(fb, false);
    return g_cp_pending;
}

static void clear_fill_impl(Framebuffer &fb, bool wipe)
{
    g_late.pending = false;      /* MENU2: a late clear nobody ran is dropped with the frame it was for */
    /* a new frame: nothing from the last one is held any more */
    g_cp_active = 0;
    g_cp_pending = 0;
    g_cp_behind = 0;
    g_cp_pre_needed = 0;
    g_cp_sink = nullptr;
    g_cp_sink_done = nullptr;
    if (g_ret_fb == &fb) g_ret_fb = nullptr;
    if (g_ret_fb2 == &fb) g_ret_fb2 = nullptr;
    g_cp_fb = &fb;
    const int allow = g_cp_allow;
    g_cp_allow = 0;
    const int held_under = bgbmp_clear_wanted();       /* MENUS093C (D23): the held frame under the 3D page is filled by the CPU clear below */
    if (allow && g_gpu_opaque && !ab_mode() && !held_under) {
        /* would the card keep a frame offered like this? Asked without
           touching anything; 3 is yes. */
        GxGpuFrame f;
        std::memset(&f, 0, sizeof f);
        f.probe = 1;
        f.full_ok = 2;
        f.keep = 1;
        if (g_gpu_opaque(&f) == 3) {
            /* THE TRANSPARENT FILL: alpha 0 everywhere, which says "no 2D
               here". The clear colour is the card's to draw. */
            const int w = active_w, h = active_h;
            if (wipe)
                for (int y = 0; y < h && y < SCREEN_H; ++y)
                    std::memset(fb.px[y], 0, (size_t)(w < SCREEN_W ? w : SCREEN_W) *
                                                 sizeof(fb.px[0][0]));
            g_cp_pending = 1;
            return;
        }
    }
    if (g_late_req && !g_bmpc.valid) {       /* MENU2: the pixel clears wait for the card (see LateCtx) */
        g_late.pending = true;
        g_late.fb = &fb;
        return;
    }
    clear_fill_classic(fb);
}

static void clear_fill_classic(Framebuffer &fb)
{
    const uint32_t c = gx_clear_argb();
    const int w = active_w, h = active_h;
    if (w <= 0 || h <= 0) return;
    const bool mw = menu_wide();          /* MENU WIDE 3D: the clear colour covers the whole wide frame */
    const int x0 = mw ? 0 : present_x(), y0 = mw ? 0 : present_y();
    const int x1 = mw ? active_w : x0 + present_w(), y1 = mw ? active_h : y0 + present_h();
    if (g_bmpc.valid && !mw && x1 > x0 && y1 > y0) {       /* MENUS093C (D23): the held bitmap frame, nearest-sampled to the present rectangle */
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                if (x < x0 || x >= x1 || y < y0 || y >= y1) { fb.px[y][x] = kOldClear; continue; }
                const int sx = (int)((long long)(x - x0) * 256 / (x1 - x0)), sy = (int)((long long)(y - y0) * 192 / (y1 - y0));
                fb.px[y][x] = g_bmpc.px[sy * 256 + sx];
            }
        return;
    }
    /* row 0 carries the present rectangle's columns if row 0 is inside it;
       the rest are copies of one of two template rows, built once */
    for (int x = 0; x < w; ++x)
        fb.px[0][x] = (x >= x0 && x < x1 && 0 >= y0 && 0 < y1) ? c : kOldClear;
    if (c == kOldClear || (x0 <= 0 && x1 >= w && y0 <= 0 && y1 >= h)) {
        /* MENU2: every row is a copy of row 0; in row bands on the raster pool's threads (SM64DS_MENU_PAR=0 keeps one) */
        struct ClrPk { Framebuffer *fb; int w; } pk = { &fb, w };
        static int clr_knob = -2;
        if (clr_knob == -2) clr_knob = gx_menu_knob_value("SM64DS_MENU_PAR");
        gx_parallel_rows(h - 1, 256, [](void *c, int y0, int y1) {
            const ClrPk *p = (const ClrPk *)c;
            for (int y = y0 + 1; y < y1 + 1; ++y)
                std::memcpy(p->fb->px[y], p->fb->px[0], (size_t)p->w * sizeof(p->fb->px[0][0]));
        }, &pk, clr_knob);
        return;
    }
    int in_row = -1, out_row = -1;
    for (int y = 0; y < h; ++y) {
        const bool in = y >= y0 && y < y1;
        int &src = in ? in_row : out_row;
        if (src < 0) {
            if (y != 0)
                for (int x = 0; x < w; ++x)
                    fb.px[y][x] = (in && x >= x0 && x < x1) ? c : kOldClear;
            src = y;
        } else {
            std::memcpy(fb.px[y], fb.px[src], (size_t)w * sizeof(fb.px[0][0]));
        }
    }
}

/* MENU WIDE 3D (run menu1, lane FILE2): the two seams, see menu_wide() */
void gx_set_menu_wide_cb(bool (*cb)(float *zoom, float *dy)) { g_menu_wide_cb = cb; }
void gx_menu_sprite(int on) {
    g_menu_sprite = on;
    g_menu_sprxf_on = 0;
    /* MENUINTERP2: the bracket is part of the frame's stream, so a replay zooms the page model and not the sprite objects */
    if (g_ip_rec) { const uint32_t v = on ? 1u : 0u; ip_push(IPK_SPRITE, 0, 1, &v); }
}
bool gx_menu_wide_active() { return menu_wide(); }
int gx_ending_viewport_shifted() { return g_ending_vp_shifted; }
/* TITLE092: the scene transform seam and the per-sprite-object transform, see gx.h */
void gx_set_menu_scene_cb(bool (*cb)(float xf[4])) { g_menu_scene_cb = cb; }
bool gx_menu_scene_active() { return g_menu_scene_cb != 0 && menu_wide(); }
void gx_menu_sprite_xf(float kx, float tx, float ky, float ty) {
    g_menu_sprxf[0] = kx; g_menu_sprxf[1] = tx; g_menu_sprxf[2] = ky; g_menu_sprxf[3] = ty;
    g_menu_sprxf_on = 1;
    if (g_ip_rec) { uint32_t v[4]; std::memcpy(v, g_menu_sprxf, sizeof v); ip_push(IPK_SPRXF, 0, 4, v); }
}

void gx_configure_anti_aliasing(int mode) {
    g_aa_mode = mode < 0 ? 0 : (mode > 1 ? 1 : mode);
}

int gx_anti_aliasing() { return g_aa_mode; }

void gx_aa_counters(unsigned long long &changed, unsigned long long &frames) {
    changed = g_aa_changed;
    frames = g_aa_run_frames;
}

/* THE FRAME AS IT WAS BEFORE THE SMOOTHING, or null when there is no such
   frame (the setting is off, the pass has not run yet this frame, or it could
   not get its buffer). Two callers and no others:

   hal/message_compositor.cpp WRITES every host pixel it writes into the live
   framebuffer here as well, so this stays a FINISHED frame -- 3D with the 2D
   layers over it -- rather than a bare 3D picture, and reads the 3D pixel
   from here when a semi-transparent sprite blends against it, so that blend
   is the one the setting-off run computes.

   ntr::ppu_display_capture READS it in place of the framebuffer it is handed,
   which is what makes the picture the GAME reads back independent of this
   setting no matter when the game arms the capture unit. Same SCREEN_W
   stride as the framebuffer, so an index into one indexes the other. */
uint32_t *gx_aa_preimage() { return g_aa_pre_valid ? g_aa_src : nullptr; }

/* THE SETTLE (run cardfull, lane CARD2): see ntr/gx.h. Written after the
   buffers it fills, ahead of everything that calls it. */
void gx_card_settle() { gx_card_settle_because("something read the picture"); }

void gx_card_settle_because(const char *why)
{
    if (!g_cp_active || !g_cp_fb || !g_gpu_opaque) return;
    /* THE REASONS, COUNTED. A frame that reads the 3D back is a frame that is
       not presented from the card, so each way into this function is named
       and the totals are printed once at exit. */
    {
        static struct { const char *why; unsigned long long n; } tab[16];
        static int used, registered;
        static unsigned long long total;
        if (!registered) {
            registered = 1;
            std::atexit([] {
                std::fprintf(stderr, "[gx] frames settled back to the processor "
                             "(read back, not presented from the card): %llu",
                             total);
                for (int i = 0; i < used; ++i)
                    std::fprintf(stderr, "; %s %llu", tab[i].why, tab[i].n);
                std::fprintf(stderr, "\n");
                std::fflush(stderr);
            });
        }
        ++total;
        int i = 0;
        for (; i < used; ++i)
            if (!std::strcmp(tab[i].why, why)) break;
        if (i == used && used < 16) { tab[used].why = why; tab[used].n = 0; ++used; }
        if (i < used) ++tab[i].n;
    }
    /* classical from here on, whatever the card answers */
    g_cp_active = 0;
    raster_buffers();
    GxGpuOut out;
    std::memset(&out, 0, sizeof out);
    GxGpuFrame f;
    std::memset(&f, 0, sizeof f);
    f.settle = 1;
    f.outside_argb = kOldClear;
    f.fb = &g_cp_fb->px[0][0];
    f.cover = &g_cover[0][0];
    f.stride = SCREEN_W;
    f.cw = g_cp_geo.cw;
    f.ch = g_cp_geo.ch;
    f.px0 = g_cp_geo.px0;
    f.py0 = g_cp_geo.py0;
    f.pw = g_cp_geo.pw;
    f.ph = g_cp_geo.ph;
    f.aa_mode = g_aa_mode;
    f.pre = g_aa_src;
    f.out = &out;
    f.sink3d = g_cp_sink;
    void (*const done)(void) = g_cp_sink_done;
    g_cp_sink = nullptr;
    g_cp_sink_done = nullptr;
    g_gpu_opaque(&f);
    g_aa_pre_valid = out.pre_valid ? 1 : 0;
    g_planes_stale = 1;
    if (done) done();
}

/* A picture dump or a present that cannot take the picture from the card, for a
   framebuffer that may be a composited SmoothMotion tick: its finished picture
   is read back from the composite target. Anything else settles as before. */
void gx_card_settle_fb(Framebuffer &fb, const char *why)
{
    const int slot = gx_card_retained(&fb);
    if (slot && g_gpu_opaque) {
        GxGpuOut out;
        std::memset(&out, 0, sizeof out);
        GxGpuFrame f;
        std::memset(&f, 0, sizeof f);
        f.settle = 3;
        f.slot = slot;
        f.fb = &fb.px[0][0];
        f.stride = SCREEN_W;
        f.out = &out;
        g_gpu_opaque(&f);
        if (slot == 2) g_ret_fb2 = nullptr; else g_ret_fb = nullptr;   /* read back: an ordinary finished frame now */
        return;
    }
    gx_card_settle_because(why);
}

const uint32_t *gx_card_capture_picture()
{
    if (!g_cp_active || !g_cp_fb || !g_gpu_opaque) return nullptr;
    if (!g_cp_scratch)
        g_cp_scratch = (uint32_t *)std::calloc((size_t)SCREEN_W * SCREEN_H,
                                               sizeof(uint32_t));
    if (!g_cp_scratch) { gx_card_settle(); return nullptr; }
    GxGpuOut out;
    std::memset(&out, 0, sizeof out);
    GxGpuFrame f;
    std::memset(&f, 0, sizeof f);
    f.settle = 2;
    f.outside_argb = kOldClear;
    f.fb = &g_cp_fb->px[0][0];
    f.pre = g_cp_scratch;
    f.stride = SCREEN_W;
    f.cw = g_cp_geo.cw;
    f.ch = g_cp_geo.ch;
    f.px0 = g_cp_geo.px0;
    f.py0 = g_cp_geo.py0;
    f.pw = g_cp_geo.pw;
    f.ph = g_cp_geo.ph;
    f.aa_mode = g_aa_mode;
    f.out = &out;
    if (g_gpu_opaque(&f) != 1) { gx_card_settle(); return nullptr; }
    return g_cp_scratch;
}

namespace {

/* Rec.601 luma, the channel weighting every edge filter of this family uses,
   on the 0..255 scale the framebuffer already holds. */
inline float luma(uint32_t p) {
    return 0.299f * (float)((p >> 16) & 0xFF) +
           0.587f * (float)((p >> 8) & 0xFF) +
           0.114f * (float)(p & 0xFF);
}

/* Blend two pixels, t of b. The alpha byte is left at 0xFF: every pixel in
   this framebuffer is opaque by the time the raster is finished, and the 2D
   compositor reads the coverage mask rather than the alpha byte. */
inline uint32_t mix2(uint32_t a, uint32_t b, float t) {
    const float s = 1.0f - t;
    auto ch = [&](int sh) {
        const float v = (float)((a >> sh) & 0xFF) * s +
                        (float)((b >> sh) & 0xFF) * t;
        const int i = (int)(v + 0.5f);
        return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
    };
    return 0xFF000000u | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}

/* ---- ONE BAND OF THE EDGE-SMOOTHING PASS ---------------------------------
 *
 * WHAT IT DOES, in the order it does it. For every pixel of the picture:
 *
 *   1. IT WRITES ONLY WHERE THE 3D ENGINE DREW. The coverage mask is the
 *      test, and it is the whole of the promise that this setting cannot
 *      touch text, the HUD or the touch-screen art: those are drawn by
 *      hal/message_compositor.cpp and hal/sub_screen.cpp, both of which run
 *      AFTER gx_render returns, so at this moment they are not in the
 *      framebuffer at all and every pixel this pass can reach is either a
 *      pixel this engine drew or the frame's own clear colour.
 *   2. It reads the four neighbours whether they are covered or not, for
 *      that same reason: an uncovered neighbour is the clear colour behind
 *      the model, and a silhouette against the background is exactly the
 *      edge worth softening. Reads are clamped to the picture.
 *   3. THE CONTRAST TEST. The local luma range must clear both an absolute
 *      floor and a fraction of the brightest neighbour, which is the pair
 *      every filter of this family uses: the absolute floor keeps the pass
 *      out of flat shading noise and the relative one keeps it out of dark
 *      areas where a few levels of difference are not an edge.
 *   4. THE DIRECTION. The second difference across x and across y says which
 *      way the step runs; the pass blends along the axis with the larger one,
 *      which is the axis the staircase is climbing.
 *   5. HOW FAR. How far this pixel's own luma sits from the average of its
 *      four neighbours, as a fraction of the local range, squared so that
 *      only a pixel that really sticks out moves much, and capped at half.
 *      A pixel in the middle of a smooth gradient has an average close to
 *      itself and does not move at all.
 *
 * Contiguous row chunks rather than the raster's interleaved bands: this pass
 * reads a row above and a row below, and contiguous chunks keep those reads
 * in the same part of the scratch buffer. Every row is written by exactly one
 * band, so the framebuffer needs no locking, exactly as the raster does not.
 */
struct AaCtx {
    Framebuffer *fb;
    int w, h;
};

void aa_band(void *ctxp, int tid, int nt) {
    const AaCtx &c = *static_cast<AaCtx *>(ctxp);
    const int y0 = (int)((long long)c.h * tid / nt);
    const int y1 = (int)((long long)c.h * (tid + 1) / nt);
    const uint32_t *const src = g_aa_src;
    unsigned hits = 0;
    for (int y = y0; y < y1; ++y) {
        const uint8_t *crow = g_cover[y];
        uint32_t *frow = c.fb->px[y];
        const uint32_t *rm = src + (size_t)y * SCREEN_W;
        const uint32_t *rn = src + (size_t)(y > 0 ? y - 1 : 0) * SCREEN_W;
        const uint32_t *rs = src + (size_t)(y + 1 < c.h ? y + 1 : y) * SCREEN_W;
        for (int x = 0; x < c.w; ++x) {
            if (!crow[x]) continue;
            const int xw = x > 0 ? x - 1 : 0;
            const int xe = x + 1 < c.w ? x + 1 : x;
            const uint32_t pM = rm[x], pN = rn[x], pS = rs[x];
            const uint32_t pW = rm[xw], pE = rm[xe];
            const float lM = luma(pM), lN = luma(pN), lS = luma(pS);
            const float lW = luma(pW), lE = luma(pE);
            float lo = lM, hi = lM;
            const float ls[4] = {lN, lS, lW, lE};
            for (int i = 0; i < 4; ++i) {
                if (ls[i] < lo) lo = ls[i];
                if (ls[i] > hi) hi = ls[i];
            }
            const float range = hi - lo;
            /* 8 of 255 absolute, an eighth of the brightest relative */
            if (range < 8.0f || range < hi * 0.125f) continue;
            const float d2x = std::fabs(lW + lE - 2.0f * lM);
            const float d2y = std::fabs(lN + lS - 2.0f * lM);
            const uint32_t n1 = (d2x >= d2y) ? pW : pN;
            const uint32_t n2 = (d2x >= d2y) ? pE : pS;
            const float avg = 0.25f * (lN + lS + lW + lE);
            float t = std::fabs(avg - lM) / range;
            t = t * t;
            if (t > 0.5f) t = 0.5f;
            if (t <= 0.002f) continue;
            const uint32_t nb = mix2(n1, n2, 0.5f);
            const uint32_t out = mix2(pM, nb, t);
            if (out != pM) {
                frow[x] = out;
                ++hits;
            }
        }
    }
    g_aa_hits[tid & 63] = hits;
}

}  // namespace

/* THE PASS ITSELF, run at the end of gx_render. Returns at once when the
   setting is off, which is the default, so a key-absent frame pays one
   compare. */
static void aa_pass(Framebuffer &fb, int cw, int ch, int nt) {
    if (!g_aa_mode || cw <= 0 || ch <= 0) return;
    /* ---- THE ONE THING THIS SETTING MAY NOT DO, AND HOW IT IS CLOSED -------
     *
     * The game READS THIS FRAMEBUFFER BACK through the DS display capture unit
     * for the dual-screen minigames, so on a frame it reads, the picture must
     * be the picture it would have got with the setting absent.
     *
     * THE FIRST ATTEMPT AT THIS WAS WRONG AND THE MEASUREMENT SAYS SO. It
     * read DISPCAPCNT's enable bit here and stood the pass down on any frame
     * that bit was already set. That assumed the game always
     * arms the unit before the frame is rasterised. On a course it does. On a
     * RUNNING dual-screen minigame it does not: scene 372 driven past its menu
     * captures 669 frames out of 1200 while this test fired on only 169 of
     * them, so five hundred frames were captured with the smoothing in them.
     * The counters that found it are on the [aa] line below.
     *
     * SO THE GUARANTEE NO LONGER DEPENDS ON ORDER AT ALL. g_aa_src already
     * holds a copy of the frame as it was before this pass, because the filter
     * needs one to avoid feeding on its own output. That copy IS the picture
     * the setting promises the game, so it is kept for the rest of the frame,
     * hal/message_compositor.cpp mirrors its own writes into it, and
     * ntr::ppu_display_capture reads it instead of the live framebuffer. The
     * capture then samples a finished frame -- 3D plus every 2D layer over it
     * -- that the smoothing never touched, whenever the arm happens to land.
     */
    ++g_aa_run_frames;
    if (!g_aa_src) {
        g_aa_src = (uint32_t *)std::calloc((size_t)SCREEN_W * SCREEN_H,
                                           sizeof(uint32_t));
        if (!g_aa_src) {
            std::fprintf(stderr,
                         "[aa] no memory for the %d x %d scratch picture; edge "
                         "smoothing is off for this run.\n", SCREEN_W, SCREEN_H);
            std::fflush(stderr);
            g_aa_mode = 0;
            --g_aa_run_frames;
            return;
        }
    }
    for (int y = 0; y < ch; ++y)
        std::memcpy(g_aa_src + (size_t)y * SCREEN_W, fb.px[y],
                    (size_t)cw * sizeof(uint32_t));
    /* THE GRAPHICS CARD, when the "Renderer" setting registered one: the same
       rule on the same copy, the changed pixels written back, the same count.
       0 means it did not run, and the bands below do the frame instead. */
    if (g_gpu_aa) {
        GxGpuAa a;
        a.src = g_aa_src;
        a.cover = &g_cover[0][0];
        a.fb = &fb.px[0][0];
        a.stride = SCREEN_W;
        a.w = cw;
        a.h = ch;
        a.changed = 0;
        if (g_gpu_aa(&a)) {
            g_aa_changed += a.changed;
            g_aa_pre_valid = 1;
            return;
        }
    }
    AaCtx ctx{&fb, cw, ch};
    /* THE SAME THREAD COUNT THE RASTER JUST USED, and through the same pool.
       RasterPool::run hands every band the width it was started at, so asking
       for a different one here would leave bands unaccounted for and the wait
       short. The counters array is indexed by band and the pool is capped at
       eight workers, so 64 slots is room to spare. */
    const int n = nt < 1 ? 1 : (nt > 64 ? 64 : nt);
    for (int i = 0; i < n; ++i) g_aa_hits[i] = 0;
    if (nt <= 1) {
        aa_band(&ctx, 0, 1);
    } else {
        pool(nt).run(aa_band, &ctx);
    }
    for (int i = 0; i < n; ++i) g_aa_changed += g_aa_hits[i];
    /* FROM HERE TO THE NEXT gx_render, g_aa_src IS THE PICTURE WITHOUT THIS
       PASS. The copy above was taken before a single pixel moved, so it is the
       frame the setting promises the game; the compositor keeps it finished
       and the capture reads it. */
    g_aa_pre_valid = 1;
}

/* SM64DS_AA_STATS=1: what the smoothing pass has done, every 300 frames and
   once more at the end of the run. Off by default.

   IT IS THE EVIDENCE FOR TWO CLAIMS THAT CANNOT BE READ OFF A PICTURE.

   The first is that a key-absent run does NO work at all: every number zero.

   The second is that the GAME reads back the same picture either way, and the
   line carries it as the two capture numbers side by side -- captures
   PERFORMED against captures that READ THE PRE-SMOOTHING FRAME. With the
   setting on those two must be equal, because what the capture unit reads is
   the copy taken before this pass touched a pixel (gx_aa_preimage above), and
   a capture that read the live framebuffer instead is exactly a frame on
   which the game saw the setting. Pair them with the captured-bytes hash,
   which has to match between a setting-off and a setting-on run of the same
   rows.

   THE NUMBERS ARE A CHECK, NOT A DESCRIPTION OF WHEN THE PASS RUNS: it runs
   on every frame. An earlier version of this file gated it on whether the
   capture unit was already armed, and these counters are what proved that
   wrong -- the measurement is written up at ntr::ppu_display_capture. */
static void aa_report(void) {
    static int want = -1;
    if (want < 0) want = getenv("SM64DS_AA_STATS") ? 1 : 0;
    if (!want) return;
    static unsigned f;
    static int at_exit_registered;
    if (!at_exit_registered) {
        at_exit_registered = 1;
        std::atexit([] {
            /* THE INVARIANT THIS LINE PRINTS, and it is the one the earlier
               weaker rule failed. Every capture the unit performs must have
               read the PRE-SMOOTHING copy of its frame, so with the setting on
                   captures that read the pre-smoothing frame == captures performed
               exactly. A capture that read the live framebuffer instead is a
               frame on which the game saw this setting. The hash beside it is
               the other half -- two runs agree on it only if every captured
               pixel of every captured frame agrees -- which turns "the game
               read the same bytes" from an argument into a comparison. */
            unsigned long long cap = 0, ref = 0, hash = 0, pre = 0;
            ppu_capture_counters(cap, ref, hash, pre);
            std::fprintf(stderr,
                         "[aa] final: mode %d, %llu frame(s) smoothed, %llu "
                         "pixel(s) rewritten; captures performed %llu, armed "
                         "but refused %llu, captures that read the "
                         "pre-smoothing frame %llu, captured-bytes hash "
                         "%016llx, captures that found no copy %llu\n",
                         g_aa_mode, g_aa_run_frames, g_aa_changed, cap, ref,
                         pre, hash, g_cap_missed);
            std::fflush(stderr);
        });
    }
    if ((f++ % 300) != 0) return;
    unsigned long long cap = 0, ref = 0, hash = 0, pre = 0;
    ppu_capture_counters(cap, ref, hash, pre);
    std::fprintf(stderr,
                 "[aa] frame %u: mode %d, %llu frame(s) smoothed, %llu "
                 "pixel(s) rewritten; captures %llu, refused %llu, pre %llu, "
                 "hash %016llx\n",
                 f - 1, g_aa_mode, g_aa_run_frames, g_aa_changed, cap, ref,
                 pre, hash);
    std::fflush(stderr);
}

/* ---- THE IN-PROCESS A/B, THE GRAPHICS-CARD RENDERER'S MAIN INSTRUMENT -----
   (run hd2, lane GPU2; SM64DS_RENDERER_AB=1)

   The card can never be byte-identical to this file: two rasterisers with
   different fill rules settle a shared edge differently and their floats do
   not travel the same path. So the proof is not a hash, it is a MEASUREMENT of
   how far apart the two pictures are, taken on the SAME TRIANGLE LIST, in the
   same process, on the same frame, before the translucent pass has run over
   either of them.

   On a checked frame the software opaque pass draws first, its four buffers
   are copied aside, the buffers are put back exactly as the clears left them,
   the card draws the same list, and the two are compared. The card's result is
   the one the rest of the frame then runs on, because the arm being measured
   is the game running on the card.

   WHAT IS COMPARED, and why each one is the number it is:
     coverage  intersection over union. A whole-pixel disagreement about
               whether anything is there at all is the worst kind, and this is
               the one number that catches a missing or extra triangle.
     colour    mean absolute channel error over pixels BOTH cover, plus the
               share of pixels past a tolerance that are NOT within one pixel
               of a coverage or polygon-ID edge. Edge pixels are expected to
               differ -- that is the fill rule -- and counting them would
               measure the fill rule rather than the renderer.
     depth     maximum and mean absolute error where both cover.
     polygon   the share of interior pixels whose ID disagrees.
   Every threshold is an environment variable so a run can say what it was
   graded against; the defaults are written down beside them.

   SM64DS_RENDERER_AB_EVERY (default 30) is how often a frame is checked, and
   SM64DS_RENDERER_AB_SHOT_FRAME with SM64DS_RENDERER_AB_SHOT_DIR writes the
   two pictures and a difference picture for one named frame, for somebody to
   look at. Nothing here judges a picture. */
namespace {

/* ---- THE THRESHOLDS, AND WHERE EACH NUMBER CAME FROM ---------------------
   Every one of these was set AFTER an honest run rather than guessed before
   it, and each leaves room for a scene this lane did not measure. The numbers
   below are the WORST value over the full table of 32 rows -- the card and
   WARP, render scale 0 and 4, levels 1, 6, 8, 12 and 29 and scenes 372, 384
   and 390, 600 frames each, TextureFilter 0, every tenth frame compared:
     coverage IoU        0.999944 at worst   (floor 0.995)
     colour mean         0.0315 of a 0..255 channel   (ceiling 0.5)
     outlier share       0.000861 of interior pixels at a tolerance of 2
                         (ceiling 0.002)
     depth, flat pixels  9.090e-05   (ceiling 1e-4; see the depth block below)
     depth, step pixels  0.999 of the step under the pixel   (ceiling 2.0)
     polygon id wrong    0           (ceiling 0.001)
   A gate that only just passes the scene it was tuned on is not a gate, so
   every ceiling above leaves between two and ninety times of room. The one
   that does not is the flat depth ceiling, and it cannot: a flat pixel is by
   definition one whose neighbourhood spans no more than that same 1e-4, so a
   fold sitting just under the boundary can produce an error just under the
   ceiling and nothing above it. Only a genuinely wrong depth can pass it. */
int g_ab_mode = -1;      /* -1 not read yet */
int g_ab_every = 30;
int g_ab_tol = 2;
double g_ab_iou_floor = 0.995;          /* measured 0.99868 at worst */
double g_ab_mean_ceiling = 0.5;         /* measured 0.0136, 36x of room */
double g_ab_outlier_ceiling = 0.002;    /* measured 0.00040, 5x of room */
double g_ab_depth_ceiling = 1e-4;       /* FLAT pixels; see below */

/* ---- THE DEPTH GATE IS TWO QUESTIONS, BECAUSE ONE NUMBER CANNOT ASK BOTH --
   (run hd2, lane GPU2R, measured rather than assumed)

   A DEPTH DISCONTINUITY IS AN EDGE THE POLYGON-ID TEST CANNOT SEE. Two
   triangles of the SAME polygon id that fold over one another -- a hill's
   silhouette against the rest of the same mesh, a wall meeting the floor it
   belongs to -- put two surfaces on one pixel under one id, so the 3x3
   id-and-coverage test above calls that pixel interior. The two rasterisers'
   fill rules are then free to give the pixel to different surfaces, and the
   depths they report differ by the gap between those surfaces. That gap says
   nothing about how accurately either one interpolates.

   MEASURED, five levels, 600 frames each, the card, TextureFilter 0, with the
   split switched off: between 1 and 470 interior pixels per level out of
   about 1.95 million came in over 1e-4, and EVERY ONE of them sat in a 3x3
   neighbourhood whose own depth span was over 1e-4 in BOTH buffers. At each
   level's worst pixel the disagreement was just under the local span:
     level  1  error 2.008e-3  span 2.043e-3   (0.98 of the step)
     level  6  error 2.588e-3  span 2.639e-3   (0.98)
     level  8  error 4.039e-3  span 4.091e-3   (0.99)
     level 12  error 2.297e-4  span 2.671e-4   (0.86)
     level 29  error 1.609e-4  span 1.652e-4   (0.97)
   The polygon ids agreed at all five. So the disagreement is never bigger
   than the step under the pixel: it is always "which of these two surfaces
   owns this pixel", never a wrong depth.

   SIMPLY EXCLUDING THOSE PIXELS WAS REJECTED. Marking every pixel whose
   neighbourhood spans more than 1e-4 as an edge and dropping it excludes 51%
   of level 1 and 44% of level 6 -- half the picture ungraded, which is not a
   gate. So the pixels are SPLIT instead, and each half is asked the question
   it can answer:

     FLAT pixels (3x3 span at or under kFlat in both buffers: one surface, no
       fold) are asked the original question, how accurately the card
       interpolates depth, against the original 1e-4 ceiling.
     STEP pixels (3x3 span over kFlat) are asked whether the disagreement is
       explained by the step under them: |error| must not exceed the local
       span times the ratio below. A pixel that disagrees by MORE than the
       step it sits on is a real mismatch and fails.

   Nothing is loosened: the 1e-4 ceiling still applies to every pixel it can
   be asked of, and the pixels it could not be asked of gained a test rather
   than an exemption. Both counts are printed beside the verdict. */
double g_ab_depth_flat = 1e-4;   /* SM64DS_RENDERER_AB_DEPTH_FLAT */
double g_ab_depth_ratio = 2.0;   /* SM64DS_RENDERER_AB_DEPTH_RATIO */
double g_ab_id_ceiling = 0.001;         /* measured 0 */
int g_ab_shot_frame = -1;
const char *g_ab_shot_dir = nullptr;

/* the run's totals */
unsigned long long g_ab_frames, g_ab_both, g_ab_union, g_ab_inter;
unsigned long long g_ab_outliers, g_ab_interior, g_ab_id_bad;
unsigned long long g_ab_cov_card, g_ab_cov_soft;
/* HOW MANY INTERIOR PIXELS ARE OVER THE DEPTH CEILING AT ALL, and how far
   their own neighbourhood's depth spans, because a maximum cannot say whether
   it is three pixels or a million and a ceiling cannot be judged without
   that. The three spans are measured over BOTH arms' 3x3 neighbourhoods. */
unsigned long long g_ab_dep_over, g_ab_dep_over_span4;
unsigned long long g_ab_dep_over_span3, g_ab_dep_over_span2;
unsigned long long g_ab_dep_flat_n, g_ab_dep_step_n, g_ab_dep_step_bad;
double g_ab_dep_flat_max, g_ab_dep_ratio_max;
int g_ab_rw_x = -1, g_ab_rw_y = -1, g_ab_rw_frame = -1;
double g_ab_rw_err, g_ab_rw_span;
double g_ab_colsum, g_ab_depsum, g_ab_depmax, g_ab_depmax_in;
double g_ab_worst_iou = 2.0;
int g_ab_worst_frame = -1;
double g_ab_worst_outshare;

/* THE WORST INTERIOR DEPTH PIXEL OF THE RUN, kept whole rather than as one
   number. A maximum on its own cannot say whether a depth disagreement is a
   silhouette inside one object that the polygon-ID edge test cannot see or a
   real interior mismatch; the pixel's position, both depths and both polygon
   IDs can, and that is the question lane GPU2R had to answer about this
   number rather than move it. */
int g_ab_dw_x = -1, g_ab_dw_y = -1, g_ab_dw_frame = -1;
double g_ab_dw_card, g_ab_dw_soft;
int g_ab_dw_id_card, g_ab_dw_id_soft;
double g_ab_dw_span_card, g_ab_dw_span_soft;

/* B, the software arm's copy of the four buffers, and the framebuffer as it
   was before either arm drew. Heap, allocated once, only in this mode. */
std::vector<uint32_t> g_ab_save, g_ab_fb;
std::vector<float> g_ab_dep;
std::vector<uint8_t> g_ab_cov, g_ab_id;
/* the software arm's planes again after its TRANSLUCENT pass too (run
   cardfull): a card that draws the whole frame is compared against these, one
   that only draws the opaque pass against the ones above */
std::vector<uint32_t> g_ab2_fb;
std::vector<float> g_ab2_dep;
std::vector<uint8_t> g_ab2_cov, g_ab2_id;

double env_d(const char *n, double dflt) {
    const char *e = getenv(n);
    if (!e || !*e) return dflt;
    char *end = 0;
    const double v = strtod(e, &end);
    return end != e ? v : dflt;
}
int env_i(const char *n, int dflt) {
    const char *e = getenv(n);
    if (!e || !*e) return dflt;
    char *end = 0;
    const long v = strtol(e, &end, 10);
    return end != e ? (int)v : dflt;
}

void ab_summary() {
    if (!g_ab_frames) {
        /* A ROW WITH NO 3D FRAMES IS A MEASUREMENT, NOT A MISSING ONE. A
           scene that never reaches the opaque pass -- a 2D one, or a run that
           ended before anything was drawn -- has nothing to compare, and
           saying so is the answer. The verdict is its own word so that no
           table can read it as a pass or as a crash. */
        std::fprintf(stderr, "[renderer-ab] summary over 0 frame(s): the "
                     "opaque pass never ran on the card, so there was nothing "
                     "to compare VERDICT=NOFRAMES\n");
        std::fflush(stderr);
        return;
    }
    const double iou = g_ab_union ? (double)g_ab_inter / (double)g_ab_union : 1.0;
    const double mean = g_ab_both ? g_ab_colsum / (double)(g_ab_both * 3) : 0.0;
    const double outshare =
        g_ab_interior ? (double)g_ab_outliers / (double)g_ab_interior : 0.0;
    const double depmean = g_ab_both ? g_ab_depsum / (double)g_ab_both : 0.0;
    const double idshare =
        g_ab_interior ? (double)g_ab_id_bad / (double)g_ab_interior : 0.0;
    const bool pass = iou >= g_ab_iou_floor && mean <= g_ab_mean_ceiling &&
                      outshare <= g_ab_outlier_ceiling &&
                      g_ab_dep_flat_max <= g_ab_depth_ceiling &&
                      g_ab_dep_step_bad == 0 &&
                      idshare <= g_ab_id_ceiling;
    std::fprintf(stderr,
                 "[renderer-ab] summary over %llu frame(s): coverage IoU %.6f "
                 "(floor %.6f) card covered %llu software covered %llu, "
                 "colour mean %.4f (ceiling %.4f), outliers "
                 "%llu of %llu interior = %.6f (ceiling %.6f), edge-excluded "
                 "%llu of %llu both-covered, depth flat "
                 "max %.3e (ceiling %.3e) over %llu flat pixel(s), depth step "
                 "worst %.3f of its own span (ceiling %.3f) over %llu step "
                 "pixel(s), %llu of them bad, depth interior "
                 "max %.3e all-covered max %.3e mean %.3e, "
                 "polygon-id wrong %llu = %.6f (ceiling %.6f), worst frame %d "
                 "at IoU %.6f outliers %.6f VERDICT=%s\n",
                 g_ab_frames, iou, g_ab_iou_floor, g_ab_cov_card,
                 g_ab_cov_soft, mean, g_ab_mean_ceiling,
                 g_ab_outliers, g_ab_interior, outshare, g_ab_outlier_ceiling,
                 g_ab_both - g_ab_interior, g_ab_both,
                 g_ab_dep_flat_max, g_ab_depth_ceiling, g_ab_dep_flat_n,
                 g_ab_dep_ratio_max, g_ab_depth_ratio, g_ab_dep_step_n,
                 g_ab_dep_step_bad,
                 g_ab_depmax_in, g_ab_depmax, depmean,
                 g_ab_id_bad, idshare, g_ab_id_ceiling, g_ab_worst_frame,
                 g_ab_worst_iou, g_ab_worst_outshare, pass ? "PASS" : "FAIL");
    /* THE WORST INTERIOR DEPTH PIXEL, WHOLE, beside the number it produced. */
    if (g_ab_dw_x >= 0)
        std::fprintf(stderr,
                     "[renderer-ab] worst interior depth pixel: frame %d at "
                     "(%d,%d) card %.9f software %.9f difference %.3e "
                     "polygon id card %d software %d, 3x3 depth span card "
                     "%.3e software %.3e\n",
                     g_ab_dw_frame, g_ab_dw_x, g_ab_dw_y, g_ab_dw_card,
                     g_ab_dw_soft, g_ab_dw_card - g_ab_dw_soft,
                     g_ab_dw_id_card, g_ab_dw_id_soft, g_ab_dw_span_card,
                     g_ab_dw_span_soft);
    std::fprintf(stderr,
                 "[renderer-ab] interior pixels over the depth ceiling: %llu "
                 "of %llu; of those, 3x3 depth span over 1e-4: %llu, over "
                 "1e-3: %llu, over 1e-2: %llu\n",
                 g_ab_dep_over, g_ab_interior, g_ab_dep_over_span4,
                 g_ab_dep_over_span3, g_ab_dep_over_span2);
    if (g_ab_rw_x >= 0)
        std::fprintf(stderr,
                     "[renderer-ab] worst step pixel: frame %d at (%d,%d) "
                     "error %.3e on a local span of %.3e = %.3f of it\n",
                     g_ab_rw_frame, g_ab_rw_x, g_ab_rw_y, g_ab_rw_err,
                     g_ab_rw_span, g_ab_dep_ratio_max);
    std::fflush(stderr);
}

int ab_mode() {
    if (g_ab_mode >= 0) return g_ab_mode;
    g_ab_mode = env_i("SM64DS_RENDERER_AB", 0) ? 1 : 0;
    if (g_ab_mode) {
        g_ab_every = env_i("SM64DS_RENDERER_AB_EVERY", 30);
        if (g_ab_every < 1) g_ab_every = 1;
        /* THE TOLERANCE IS TWO because a channel is computed as
           round(colour * texel / 255) on both sides out of interpolants that
           do not travel the same path: one step of rounding on each side of
           the multiply is one count, and two is that with room to spare. */
        g_ab_tol = env_i("SM64DS_RENDERER_AB_TOL", 2);
        g_ab_iou_floor = env_d("SM64DS_RENDERER_AB_IOU", g_ab_iou_floor);
        g_ab_mean_ceiling = env_d("SM64DS_RENDERER_AB_MEAN", g_ab_mean_ceiling);
        g_ab_outlier_ceiling =
            env_d("SM64DS_RENDERER_AB_OUTLIERS", g_ab_outlier_ceiling);
        g_ab_depth_ceiling = env_d("SM64DS_RENDERER_AB_DEPTH", g_ab_depth_ceiling);
        g_ab_depth_flat = env_d("SM64DS_RENDERER_AB_DEPTH_FLAT", g_ab_depth_flat);
        g_ab_depth_ratio =
            env_d("SM64DS_RENDERER_AB_DEPTH_RATIO", g_ab_depth_ratio);
        g_ab_id_ceiling = env_d("SM64DS_RENDERER_AB_ID", g_ab_id_ceiling);
        g_ab_shot_frame = env_i("SM64DS_RENDERER_AB_SHOT_FRAME", -1);
        g_ab_shot_dir = getenv("SM64DS_RENDERER_AB_SHOT_DIR");
        std::atexit(ab_summary);
    }
    return g_ab_mode;
}

/* A 24-bit bottom-up bitmap, the plainest thing every viewer opens. The
   pictures are made of cartridge data, so they are written where the run was
   told to write them and never into the source tree. */
void ab_bmp(const char *path, const uint32_t *px, int stride, int x0, int y0,
            int w, int h) {
    std::FILE *f = std::fopen(path, "wb");
    if (!f) return;
    const int row = (w * 3 + 3) & ~3;
    const unsigned size = 54u + (unsigned)row * (unsigned)h;
    unsigned char hd[54];
    std::memset(hd, 0, sizeof hd);
    hd[0] = 'B'; hd[1] = 'M';
    hd[2] = (unsigned char)size; hd[3] = (unsigned char)(size >> 8);
    hd[4] = (unsigned char)(size >> 16); hd[5] = (unsigned char)(size >> 24);
    hd[10] = 54;
    hd[14] = 40;
    hd[18] = (unsigned char)w; hd[19] = (unsigned char)(w >> 8);
    hd[20] = (unsigned char)(w >> 16); hd[21] = (unsigned char)(w >> 24);
    hd[22] = (unsigned char)h; hd[23] = (unsigned char)(h >> 8);
    hd[24] = (unsigned char)(h >> 16); hd[25] = (unsigned char)(h >> 24);
    hd[26] = 1;
    hd[28] = 24;
    std::fwrite(hd, 1, sizeof hd, f);
    std::vector<unsigned char> line((size_t)row, 0);
    for (int y = h - 1; y >= 0; --y) {
        const uint32_t *src = px + (size_t)(y0 + y) * stride + x0;
        for (int x = 0; x < w; ++x) {
            line[(size_t)x * 3 + 0] = (unsigned char)(src[x] & 0xFF);
            line[(size_t)x * 3 + 1] = (unsigned char)((src[x] >> 8) & 0xFF);
            line[(size_t)x * 3 + 2] = (unsigned char)((src[x] >> 16) & 0xFF);
        }
        std::fwrite(&line[0], 1, (size_t)row, f);
    }
    std::fclose(f);
}

}  // namespace

namespace {

/* ---- ONE ROW'S COVERED RANGE, FROM THE EDGE FUNCTIONS (run perf2, lane RASTER)
   The raster used to test every pixel of a triangle's bounding box, and a
   thin or diagonal triangle covers a small part of its box: most of those
   tests said no. This finds, per row, the range of x the three edge tests can
   pass on, and the row loop then visits only that range. Each pixel it visits
   still takes the EXACT test it always took, so the only question is whether
   the range can leave out a pixel the old loop would have drawn. It cannot,
   and the reason is monotonicity, not an estimate:

   A pixel's edge value is r - ey * ((x + 0.5f) - vx) in floats, the same
   expression, in the same order, as the row loop's. Every step of it is one
   IEEE operation, and each is monotone in its input when the inputs are
   finite: x + 0.5f grows with x, the subtraction keeps that order, the
   multiply by a fixed ey keeps it or reverses it by ey's sign, and r minus
   that reverses it again. So along a row each edge value only ever moves one
   way, and "value >= 0" is true on a prefix of the row or on a suffix of it
   (on all of it or none of it when ey is 0). A pixel is inside when all three
   values are >= 0 or all three are <= 0, so the inside pixels are the union of
   two intersections of prefixes and suffixes: two intervals. Each end is found
   by starting from a double-precision estimate of where the edge crosses zero
   and then stepping with the exact float test until it holds on one side and
   fails on the other, which is the exact boundary whatever the estimate was.
   The range visited is the span of the two intervals, one pixel wider on each
   side for good measure, clipped to the box.

   The argument needs finite values that cannot overflow, so it is used only
   when every vertex coordinate is within 1e18 of the origin (then no product
   in the edge value can reach FLT_MAX); anything else, and any box eight
   pixels wide or less, takes the whole box as before. */
struct SpanTri {
    float ey[3], vx[3];
    double inv[3];
};

inline float span_edge(float r, float ey, float vx, int x) {
    const float px = x + 0.5f;
    return r - ey * (px - vx);
}

inline bool span_ok(const GxVertex &a, const GxVertex &b, const GxVertex &c) {
    const float lim = 1e18f;
    return std::fabs(a.x) <= lim && std::fabs(a.y) <= lim &&
           std::fabs(b.x) <= lim && std::fabs(b.y) <= lim &&
           std::fabs(c.x) <= lim && std::fabs(c.y) <= lim;
}

/* Narrow [L, R] to where all three edge values are >= 0 (sgn > 0) or all
   three are <= 0 (sgn < 0). False when nothing in the row qualifies. */
inline bool span_side(const SpanTri &st, const float r[3], int sgn, int &L,
                      int &R) {
    for (int k = 0; k < 3; ++k) {
        const float ey = st.ey[k], vx = st.vx[k], rk = r[k];
        auto P = [&](int x) {
            const float n = span_edge(rk, ey, vx, x);
            return sgn > 0 ? n >= 0 : n <= 0;
        };
        if (ey == 0.0f) {
            if (!P(L)) return false;
            continue;
        }
        const double est = (double)vx + (double)rk * st.inv[k] - 0.5;
        if ((ey > 0.0f) == (sgn > 0)) {
            /* true on a prefix: find the last x that passes */
            double e = est;
            if (e < (double)(L - 1)) e = (double)(L - 1);
            if (e > (double)R) e = (double)R;
            int x0 = (int)e;
            while (x0 < R && P(x0 + 1)) ++x0;
            while (x0 >= L && !P(x0)) --x0;
            R = x0;
        } else {
            /* true on a suffix: find the first x that passes */
            double e = est;
            if (e < (double)L) e = (double)L;
            if (e > (double)(R + 1)) e = (double)(R + 1);
            int x0 = (int)e;
            while (x0 > L && P(x0 - 1)) --x0;
            while (x0 <= R && !P(x0)) ++x0;
            L = x0;
        }
        if (L > R) return false;
    }
    return true;
}

/* The row's range to visit, or false when no pixel of it can be inside. */
inline bool row_span(const SpanTri &st, float r0, float r1, float r2,
                     int minx, int maxx, int &xs, int &xe) {
    const float r[3] = {r0, r1, r2};
    int l1 = minx, h1 = maxx, l2 = minx, h2 = maxx;
    const bool pos = span_side(st, r, 1, l1, h1);
    const bool neg = span_side(st, r, -1, l2, h2);
    if (!pos && !neg) return false;
    int lo, hi;
    if (pos && neg) {
        lo = l1 < l2 ? l1 : l2;
        hi = h1 > h2 ? h1 : h2;
    } else if (pos) {
        lo = l1;
        hi = h1;
    } else {
        lo = l2;
        hi = h2;
    }
    xs = lo - 1 < minx ? minx : lo - 1;
    xe = hi + 1 > maxx ? maxx : hi + 1;
    return true;
}

}  // namespace

namespace {

/* ---- THE FRAME'S TRIANGLES BINNED BY ROW TILE (run perf2, lane RASTER) ----
   The raster used to give each worker every nt-th row of the screen, so every
   worker walked every triangle and set each one up again for itself: about a
   fifth of the raster's CPU on eight threads. Now the screen is cut into
   tiles of whole rows, each triangle is listed once, in submission order,
   under every tile its clamped rows reach, and a worker takes the next tile
   off a counter and draws only that tile's list. One worker owns each row
   for the whole frame and walks its triangles in submission order, opaque
   pass then translucent pass, so every pixel sees the same operations in the
   same order as before: this partitions the work, it does not reorder it.
   Built on the calling thread before any worker starts; read-only after.

   THE STENCIL CLEARS ARE THE ONE PIECE OF STATE THAT IS NOT PER TRIANGLE. A
   new shadow mask group clears the whole stencil, and whether a triangle
   starts one depends on the triangle before it in the list, not on where
   either of them is on the screen. So the clear points are found once, here,
   as a running count per triangle (g_clr_cum), and a tile clears its own rows
   when the count has moved since the last triangle it drew, and once more at
   the end of the pass if it moved after that. Between two triangles a tile
   draws, nothing else writes its rows, so a clear taken late is the same
   clear. A tile clears its rows across the whole allocation width and every
   tile runs, drawn on or not, which is the old per-band clear exactly. */
std::vector<int> g_bin_off[2], g_bin_idx[2];
std::vector<int> g_clr_cum;
int g_clr_total;
struct BinTri {
    int i, pass, k0, k1;
};
std::vector<BinTri> g_bin_tmp;

/* Rows a tile holds, SM64DS_RASTER_TILE to override for measurement. */
int raster_tile_rows() {
    static int n = -1;
    if (n < 0) {
        const char *e = getenv("SM64DS_RASTER_TILE");
        n = e ? atoi(e) : 16;
        if (n < 1) n = 1;
    }
    return n;
}

/* A triangle with a vertex far outside the screen (a sky dome vertex at the near
   plane lands millions of pixels away). The float edge values of such a
   triangle are good to about a pixel only, so a row where the float test finds
   no covered pixel can still hold pixels the exact test covers: a thin wedge
   at the dome's pole is the case. big_row returns the pixel range of the
   row that the triangle's exact scanline interval reaches (one pixel of
   margin each side), joined with the range row_span found when it found one,
   so those pixels get visited and go through the float test and edge_rescue
   like any other pixel next to an edge. */
inline bool big_tri_test(const GxVertex &a, const GxVertex &b,
                         const GxVertex &c) {
    const float lim = 65536.0f;
    return std::fabs(a.x) > lim || std::fabs(a.y) > lim ||
           std::fabs(b.x) > lim || std::fabs(b.y) > lim ||
           std::fabs(c.x) > lim || std::fabs(c.y) > lim;
}

inline bool big_row(const GxVertex &a, const GxVertex &b, const GxVertex &c,
                    float py, int minx, int maxx, bool have, int &xs,
                    int &xe) {
    const GxVertex *const v[3] = {&a, &b, &c};
    const double y = (double)py;
    double lo = 1e300, hi = -1e300;
    for (int k = 0; k < 3; ++k) {
        const GxVertex &p = *v[k];
        const GxVertex &q = *v[(k + 1) % 3];
        const double y0 = p.y, y1 = q.y, x0 = p.x, x1 = q.x;
        if (!((y0 <= y && y <= y1) || (y1 <= y && y <= y0))) continue;
        if (y0 == y1) {
            lo = std::fmin(lo, std::fmin(x0, x1));
            hi = std::fmax(hi, std::fmax(x0, x1));
        } else {
            const double x = x0 + (y - y0) * (x1 - x0) / (y1 - y0);
            lo = std::fmin(lo, x);
            hi = std::fmax(hi, x);
        }
    }
    if (hi < lo) return have;
    double flo = std::floor(lo) - 1.0, fhi = std::ceil(hi) + 1.0;
    if (flo < (double)minx) flo = (double)minx;
    if (fhi > (double)maxx) fhi = (double)maxx;
    if (flo > fhi) return have;
    const int l = (int)flo, h = (int)fhi;
    if (have) {
        if (l < xs) xs = l;
        if (h > xe) xe = h;
    } else {
        xs = l;
        xe = h;
    }
    return true;
}

/* THE SEAM RESCUE (run hunt8, lane SKYLINES1): a pixel centre that lies on, or
   within rounding of, the edge two triangles share must land in at least one of
   them. The float edge values above are measured from a different vertex in
   each triangle (a for one, b for the other), and a sky dome vertex that sits
   at the near plane projects millions of pixels off screen: measured from that
   far vertex the float value of the edge is off by up to a pixel, while from
   the near end it is exact. The two triangles then disagree about the pixels
   along the shared edge, both refuse some of them, nothing covers them and the
   clear colour shows through as a dotted black line, most visibly in the sky,
   which has nothing behind it. The DS rasteriser steps every edge from one set
   of integers, so a shared edge never opens. This redoes the test in double
   precision for the pixels the float test just refused and that sit within the
   float error of an edge. The vertex coordinates are floats and the coordinate
   differences and their products fit a double exactly, so the value is the
   same for both triangles of the pair whichever vertex it is measured from:
   exactly one of them takes a pixel that is off the line, both take one that is
   on it. Pixels the float test accepts are untouched. */
inline bool edge_rescue(const GxVertex &a, const GxVertex &b,
                        const GxVertex &c, float px, float py, float r0,
                        float r1, float r2, float n0, float n1, float n2) {
    const float t0 = 1e-6f * (std::fabs(r0) + std::fabs(r0 - n0)) + 0.01f;
    const float t1 = 1e-6f * (std::fabs(r1) + std::fabs(r1 - n1)) + 0.01f;
    const float t2 = 1e-6f * (std::fabs(r2) + std::fabs(r2 - n2)) + 0.01f;
    if (!(std::fabs(n0) <= t0 || std::fabs(n1) <= t1 || std::fabs(n2) <= t2))
        return false;
    const double dpx = (double)px, dpy = (double)py;
    const double d0 = ((double)b.x - (double)a.x) * (dpy - (double)a.y) -
                      ((double)b.y - (double)a.y) * (dpx - (double)a.x);
    const double d1 = ((double)c.x - (double)b.x) * (dpy - (double)b.y) -
                      ((double)c.y - (double)b.y) * (dpx - (double)b.x);
    const double d2 = ((double)a.x - (double)c.x) * (dpy - (double)c.y) -
                      ((double)a.y - (double)c.y) * (dpx - (double)c.x);
    return (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);
}

}  // namespace

/* ---- SM64DS_RASTER_AB=1: THE OLD RASTER AGAINST THE NEW, ON EVERY FRAME ----
   (run perf2, lane RASTER)

   The raster in gx_render is allowed to get faster only if it draws the same
   frame, to the byte. This is the instrument that says whether it does, and it
   was written before any of the speed work so that every step of it could be
   measured against the code as it stood.

   With the switch on, each call that runs the raster passes runs them TWICE
   from the same starting buffers. First the frozen copy below: the raster
   body, the nearest and filtered samplers and the wrap rule exactly as they
   were before the speed work, on one thread. Its six planes are set aside, the
   buffers are put back as they were, and then the live code runs the same
   passes on however many threads it chooses. The six planes the passes write
   -- colour, depth, coverage, stencil, polygon id and the translucent
   attribute -- are compared byte for byte over the WHOLE allocation, not just
   the picture, and the live result is the one the frame keeps.

   A frame with any difference prints its counts and the first pixel that
   differs; a line every 300 frames and one at exit carry the running totals,
   and the exit line says VERDICT=PASS only when every count is zero. Depth is
   compared as bits, so a -0 against a +0 or a changed NaN counts.

   Off, which is the default, this costs one test of a flag per frame, and the
   copy below is never called. It stays in the file as the reference the next
   change to the raster is measured against. */
namespace {

static int ref_tex_coord_i(int i, int size, bool repeat, bool flip) {
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

static int ref_tex_coord(float f, int size, bool repeat, bool flip) {
    return ref_tex_coord_i(static_cast<int>(std::floor(f)), size, repeat, flip);
}

static uint32_t ref_sample_bilinear(const TexLevel &L, float u, float v,
                                bool rs, bool rt, bool fs, bool ft) {
    const float fu = u - 0.5f, fv = v - 0.5f;
    const float flu = std::floor(fu), flv = std::floor(fv);
    const int iu = static_cast<int>(flu), iv = static_cast<int>(flv);
    const float du = fu - flu, dv = fv - flv;
    const int x0 = ref_tex_coord_i(iu, L.w, rs, fs);
    const int x1 = ref_tex_coord_i(iu + 1, L.w, rs, fs);
    const int y0 = ref_tex_coord_i(iv, L.h, rt, ft);
    const int y1 = ref_tex_coord_i(iv + 1, L.h, rt, ft);
    const uint32_t p00 = L.px[(size_t)y0 * L.w + x0];
    const uint32_t p10 = L.px[(size_t)y0 * L.w + x1];
    const uint32_t p01 = L.px[(size_t)y1 * L.w + x0];
    const uint32_t p11 = L.px[(size_t)y1 * L.w + x1];
    const float w00 = (1.0f - du) * (1.0f - dv);
    const float w10 = du * (1.0f - dv);
    const float w01 = (1.0f - du) * dv;
    const float w11 = du * dv;
    const float a00 = (float)(p00 >> 24) * w00;
    const float a10 = (float)(p10 >> 24) * w10;
    const float a01 = (float)(p01 >> 24) * w01;
    const float a11 = (float)(p11 >> 24) * w11;
    const float asum = a00 + a10 + a01 + a11;
    if (asum <= 0.0f) return 0;
    const float inv = 1.0f / asum;
    auto ch = [&](int sh) {
        const float s = (float)((p00 >> sh) & 0xFF) * a00 +
                        (float)((p10 >> sh) & 0xFF) * a10 +
                        (float)((p01 >> sh) & 0xFF) * a01 +
                        (float)((p11 >> sh) & 0xFF) * a11;
        const int i = (int)(s * inv + 0.5f);
        return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
    };
    const int ai = (int)(asum + 0.5f);
    const uint32_t a = (uint32_t)(ai < 0 ? 0 : (ai > 255 ? 255 : ai));
    return (a << 24) | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}

template <int FILTER>
static uint32_t ref_sample_filtered(const TriTex &tt, const uint32_t *base, int w,
                                int h, float u, float v, bool rs, bool rt,
                                bool fs, bool ft) {
    if (FILTER == 1) {
        const TexLevel L{base, w, h};
        return ref_sample_bilinear(L, u, v, rs, rt, fs, ft);
    }
    const TexLevel L0 = tex_level(tt, base, w, h, tt.lod);
    const float s0 = (float)L0.w / (float)w, t0 = (float)L0.h / (float)h;
    const uint32_t c0 = ref_sample_bilinear(L0, u * s0, v * t0, rs, rt, fs, ft);
    if (tt.frac <= 0.0f) return c0;
    const TexLevel L1 = tex_level(tt, base, w, h, tt.lod + 1);
    if (L1.px == L0.px) return c0;
    const float s1 = (float)L1.w / (float)w, t1 = (float)L1.h / (float)h;
    const uint32_t c1 = ref_sample_bilinear(L1, u * s1, v * t1, rs, rt, fs, ft);
    /* THE TWO LEVELS BLEND IN PREMULTIPLIED ALPHA TOO, for sample_bilinear's
       reason: a coarse level of a cut-out is mostly transparent, and mixing
       its colour in unweighted would grey the sharp level's edge. */
    const float f1 = tt.frac, f0 = 1.0f - f1;
    const float a0 = (float)(c0 >> 24) * f0, a1 = (float)(c1 >> 24) * f1;
    const float asum = a0 + a1;
    if (asum <= 0.0f) return 0;
    const float inv = 1.0f / asum;
    auto ch = [&](int sh) {
        const float s = (float)((c0 >> sh) & 0xFF) * a0 +
                        (float)((c1 >> sh) & 0xFF) * a1;
        const int i = (int)(s * inv + 0.5f);
        return (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i));
    };
    const int ai = (int)(asum + 0.5f);
    const uint32_t a = (uint32_t)(ai < 0 ? 0 : (ai > 255 ? 255 : ai));
    return (a << 24) | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}

struct RefArgs {
    Framebuffer *fb;
    uint32_t only;
    bool have_shadow, have_translucent, want_id;
    int pass_lo, pass_hi;
};

/* The raster body as it stood at the start of run perf2, verbatim apart from
   the sampler names; the names it read from gx_render are rebound here. */
template <int FILTER>
void raster_ref(const RefArgs &ra, int tid, int nt) {
    Framebuffer &fb = *ra.fb;
    const uint32_t only = ra.only;
    const bool have_shadow = ra.have_shadow;
    const bool have_translucent = ra.have_translucent;
    const bool want_id = ra.want_id;
    const int pass_lo = ra.pass_lo, pass_hi = ra.pass_hi;
    const bool wdepth = g_wdepth;
    DepthRow *const depth = g_depth;
    MaskRow *const stencil = g_stencil;
    MaskRow *const attrid = g_attrid;
    MaskRow *const tlattr = g_tlattr;
    bool prev_mask = false;
    for (int pass = pass_lo; pass <= pass_hi; ++pass)
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
        /* CLAMPED TO THE PRESENT RECTANGLE. There is no side clip against the
           viewport in this raster -- off-screen geometry has always been
           bounded by the screen edge, which at every other aspect IS the
           viewport edge. Inside a pillarbox they are different rectangles, and
           a triangle that runs past the 4:3 edge would otherwise paint into
           the margin. present_* is (0,0,active_w,active_h) on every other run,
           so these are the same four compares as before. */
        int px0, py0, px1, py1;
        raster_clip(px0, py0, px1, py1);
        if (minx < px0) minx = px0;
        if (miny < py0) miny = py0;
        if (maxx > px1) maxx = px1;
        if (maxy > py1) maxy = py1;

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
        const float wq = wdepth ? w_tie_q(a, b, c) : 0.0f;
        /* NOTE the UV terms below stay written as l0 * a.u * iwa. Folding
           a.u * iwa out to the triangle would regroup the multiply, and float
           multiplication does not associate -- (l0*a.u)*iwa and l0*(a.u*iwa)
           are different numbers. Only the reciprocal itself is hoisted. */
        const bool textured = t.tex && t.tw > 0 && t.th > 0;
        const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
        const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
        /* Host pixels per DS texel (GxTriangle::tex_scale). The UVs the
           geometry engine produced are in DS TEXELS, and tw/th are the bound
           buffer's real pixel dimensions, so the sampler works in buffer
           pixels by multiplying. 1 for everything the ROM supplies, and a
           multiply by exactly 1.0f returns its operand bit for bit, so the
           default run samples the texel it always sampled. */
        const float tsc = (float)(t.tex_scale ? t.tex_scale : 1);
        /* WHICH CHAIN LEVELS THIS TRIANGLE SITS BETWEEN, looked up rather than
           computed: the head of gx_render already resolved it once for every
           triangle in the frame. Nothing here runs at any other filter mode --
           FILTER is a constant, so the whole statement folds away. */
        TriTex tt{nullptr, 0, 0.0f};
        if (FILTER == 2 && textured)
            tt = g_tritex[(size_t)(&t - g.tris.data())];
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
        /* MODE 2 (toon / highlight): 0 for every other polygon and for every
           frame with no mode-2 polygon, which is the plain modulation below */
        const int toon = (t.mode == 2) ? g_toon_shade : 0;

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
                              (n0 <= 0 && n1 <= 0 && n2 <= 0)) &&
                              !edge_rescue(a, b, c, px, py, r0, r1, r2, n0, n1, n2))
                            continue;
                        const float w0 = n0 / area, w1 = n1 / area,
                                    w2 = n2 / area;
                        const float l0 = w1, l1 = w2, l2 = w0;
                        const float z = wdepth ? -(l0 * iwa + l1 * iwb + l2 * iwc) : l0 * a.z + l1 * b.z + l2 * c.z;
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
                              (n0 <= 0 && n1 <= 0 && n2 <= 0)) &&
                              !edge_rescue(a, b, c, px, py, r0, r1, r2, n0, n1, n2))
                            continue;
                        if (!srow[x]) continue;
                        srow[x] = 0;
                        const float w0 = n0 / area, w1 = n1 / area,
                                    w2 = n2 / area;
                        const float l0 = w1, l1 = w2, l2 = w0;
                        const float z = wdepth ? -(l0 * iwa + l1 * iwb + l2 * iwc) : l0 * a.z + l1 * b.z + l2 * c.z;
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
                            const int ui = ref_tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                            const int vi = ref_tex_coord(vv * tsc, t.th, rep_t, flip_t);
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
            uint8_t *trow = tlattr[y];
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
                if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) || (n0 <= 0 && n1 <= 0 && n2 <= 0)) && !edge_rescue(a, b, c, px, py, r0, r1, r2, n0, n1, n2))
                    continue;
                const float w0 = n0 / area, w1 = n1 / area, w2 = n2 / area;
                const float l0 = w1, l1 = w2, l2 = w0;   // barycentric for a, b, c
                const float z = wdepth ? -(l0 * iwa + l1 * iwb + l2 * iwc) : l0 * a.z + l1 * b.z + l2 * c.z;
                if (w_depth_fail(z, drow[x], wq, t.depth_eq != 0)) continue;
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
                    /* THE ONE SAMPLING DECISION IN THE WHOLE RASTER, and it
                       is made by the compiler rather than by this pixel:
                       FILTER is a constant in this instantiation, so the
                       nearest body below is exactly the two tex_coord calls
                       and the one load it always was, with nothing added. */
                    if constexpr (FILTER == 0) {
                        const int ui = ref_tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                        const int vi = ref_tex_coord(vv * tsc, t.th, rep_t, flip_t);
                        texel = t.tex[vi * t.tw + ui];
                    } else {
                        texel = ref_sample_filtered<FILTER>(
                            tt, t.tex, t.tw, t.th, uu * tsc, vv * tsc, rep_s,
                            rep_t, flip_s, flip_t);
                        const int ui = ref_tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                        const int vi = ref_tex_coord(vv * tsc, t.th, rep_t, flip_t);
                        texel = filtered_texel_cover(texel, t.tex[vi * t.tw + ui]);
                    }
                    if ((texel >> 24) == 0) continue;      // transparent texel
                }

                // Round, do not truncate: barycentrics sum to 0.9999 rather than
                // exactly 1, and a truncating cast bands a flat surface 255/254.
                /* MODE 2: the entry this pixel's interpolated red picks. Toon
                   modulates the ENTRY instead of the vertex colour; highlight
                   modulates the vertex red on all three channels and adds the
                   entry after, saturating (GBATEK DS 3D Polygon Attributes;
                   melonDS RenderPixel, the same order). */
                const uint32_t fd = (g_fog.on && t.fog)
                    ? fog_density(wdepth ? -g_fog.wscale / z : z * 16777215.0f) : 0u;
                const float *te = nullptr;
                float tsm[3];
                if (toon) {
                    const float vr = l0 * acol[0] + l1 * bcol[0] + l2 * ccol[0];
                    if (g_toon_smooth) { toon_smooth_entry(vr, tsm); te = tsm; }
                    else te = &g_toon8[toon_index(vr) * 3];
                }
                auto ch = [&](int k, int sh) {
                    const int kv = toon == 2 ? 0 : k;
                    const float v = toon == 1
                        ? te[k]
                        : l0 * acol[kv] + l1 * bcol[kv] + l2 * ccol[kv];
                    const float m = inv255.v[(texel >> sh) & 0xFF];
                    int i = static_cast<int>(v * m + 0.5f);
                    if (toon == 2) i += static_cast<int>(te[k] + 0.5f);
                    i = i < 0 ? 0 : (i > 255 ? 255 : i);
                    if (fd) i = (int)(((uint32_t)i * (128u - fd) + (uint32_t)g_fog.rgb[k] * fd) >> 7);
                    return static_cast<uint32_t>(i);
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
                    if (want_id) irow[x] = t.polyid;
                    /* an opaque write replaces the pixel, so the translucent
                       half of its attribute word goes with it (melonDS stores
                       polyattr & 0x3F008000 on the opaque path, bit 22 clear).
                       Our two passes draw every opaque polygon before any
                       translucent one, so this only ever fires for a
                       translucent-CLASS triangle whose per-pixel alpha came out
                       31: an A3I5 or A5I3 texel at full opacity. */
                    if (have_translucent) trow[x] = 0;
                } else {
                    /* translucent: blend over the framebuffer, keep depth
                       (DS translucent polys depth-test but do not write).
                       The hardware's translucent polygon-ID rule comes first: a
                       pixel that already took a fragment of THIS polygon ID
                       this frame refuses the next one outright. */
                    const uint8_t tl =
                        static_cast<uint8_t>(0x40 | t.polyid);
                    if (trow[x] == tl) continue;
                    trow[x] = tl;
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
}

int g_rab = -1;                        /* -1 not read yet */
unsigned long long g_rab_frames, g_rab_runs, g_rab_cover;
unsigned long long g_rab_bad[6];
unsigned long long g_rab_bad_frames;
const char *const k_rab_name[6] = {"colour", "depth", "coverage",
                                   "stencil", "attrid", "tlattr"};
std::vector<uint8_t> g_rab_start[6], g_rab_ref[6];

void rab_planes(Framebuffer &fb, uint8_t *p[6], size_t sz[6], size_t es[6]) {
    const size_t n = (size_t)SCREEN_W * (size_t)SCREEN_H;
    p[0] = (uint8_t *)&fb.px[0][0];  es[0] = 4;
    p[1] = (uint8_t *)&g_depth[0][0]; es[1] = 4;
    p[2] = &g_cover[0][0];           es[2] = 1;
    p[3] = &g_stencil[0][0];         es[3] = 1;
    p[4] = &g_attrid[0][0];          es[4] = 1;
    p[5] = &g_tlattr[0][0];          es[5] = 1;
    for (int i = 0; i < 6; ++i) sz[i] = n * es[i];
}

void rab_save(Framebuffer &fb, std::vector<uint8_t> *dst) {
    uint8_t *p[6]; size_t sz[6], es[6];
    rab_planes(fb, p, sz, es);
    for (int i = 0; i < 6; ++i) {
        dst[i].resize(sz[i]);
        std::memcpy(dst[i].data(), p[i], sz[i]);
    }
}

void rab_load(Framebuffer &fb, const std::vector<uint8_t> *src) {
    uint8_t *p[6]; size_t sz[6], es[6];
    rab_planes(fb, p, sz, es);
    for (int i = 0; i < 6; ++i) std::memcpy(p[i], src[i].data(), sz[i]);
}

void rab_summary() {
    unsigned long long total = 0;
    for (int i = 0; i < 6; ++i) total += g_rab_bad[i];
    std::fprintf(stderr,
                 "[raster-ab] summary over %llu frame(s), %llu pass run(s), "
                 "%llu covered pixel(s) in the reference: mismatches colour "
                 "%llu depth %llu coverage %llu stencil %llu attrid %llu "
                 "tlattr %llu, frames with any %llu VERDICT=%s\n",
                 g_rab_frames, g_rab_runs, g_rab_cover, g_rab_bad[0],
                 g_rab_bad[1], g_rab_bad[2], g_rab_bad[3], g_rab_bad[4],
                 g_rab_bad[5], g_rab_bad_frames,
                 !g_rab_runs ? "NOFRAMES" : (total ? "FAIL" : "PASS"));
    std::fflush(stderr);
}

int raster_ab() {
    if (g_rab < 0) {
        g_rab = env_i("SM64DS_RASTER_AB", 0) ? 1 : 0;
        if (g_rab) std::atexit(rab_summary);
    }
    return g_rab;
}

/* The live planes against the reference's, element by element. */
void rab_compare(Framebuffer &fb, int lo, int hi) {
    uint8_t *p[6]; size_t sz[6], es[6];
    rab_planes(fb, p, sz, es);
    unsigned long long bad[6] = {0, 0, 0, 0, 0, 0};
    int first_plane = -1;
    size_t first_at = 0;
    for (int i = 0; i < 6; ++i) {
        const uint8_t *a = g_rab_ref[i].data(), *b = p[i];
        if (std::memcmp(a, b, sz[i]) == 0) continue;
        const size_t n = sz[i] / es[i];
        for (size_t k = 0; k < n; ++k) {
            if (std::memcmp(a + k * es[i], b + k * es[i], es[i]) == 0) continue;
            if (!bad[i]++ && first_plane < 0) {
                first_plane = i;
                first_at = k;
            }
        }
    }
    {
        const uint8_t *c = g_rab_ref[2].data();
        for (size_t k = 0; k < sz[2]; ++k) g_rab_cover += c[k] != 0;
    }
    ++g_rab_runs;
    if (hi == 1) ++g_rab_frames;
    unsigned long long any = 0;
    for (int i = 0; i < 6; ++i) {
        g_rab_bad[i] += bad[i];
        any += bad[i];
    }
    if (any) {
        ++g_rab_bad_frames;
        uint32_t rv = 0, nv = 0;
        const size_t e = es[first_plane];
        std::memcpy(&rv, g_rab_ref[first_plane].data() + first_at * e, e);
        std::memcpy(&nv, p[first_plane] + first_at * e, e);
        std::fprintf(stderr,
                     "[raster-ab] frame %llu passes %d..%d MISMATCH colour %llu "
                     "depth %llu coverage %llu stencil %llu attrid %llu tlattr "
                     "%llu; first: %s at (%d,%d) reference %08x live %08x\n",
                     g_rab_frames, lo, hi, bad[0], bad[1], bad[2], bad[3],
                     bad[4], bad[5], k_rab_name[first_plane],
                     (int)(first_at % SCREEN_W), (int)(first_at / SCREEN_W),
                     rv, nv);
        std::fflush(stderr);
    }
    if (hi == 1 && (g_rab_frames % 300) == 1) {
        unsigned long long total = 0;
        for (int i = 0; i < 6; ++i) total += g_rab_bad[i];
        std::fprintf(stderr,
                     "[raster-ab] frame %llu: %llu pass run(s) compared, "
                     "%llu mismatching element(s) so far\n",
                     g_rab_frames, g_rab_runs, total);
        std::fflush(stderr);
    }
}

}  // namespace

/* SM64DS_COVER_DUMP=<path>: after the frame's passes (and before the edge
   smoothing, which rewrites the mask) write the 3D coverage mask, one byte a
   pixel, 255 where the 3D engine wrote the pixel, as a binary PGM. Each frame
   overwrites the file, so the last frame drawn is what is left. With
   SM64DS_TEX_ONLY on the same run (which now also limits the card's triangles)
   this is one texture's footprint, which is how TextureFilter 0, 1 and 2 are
   compared pixel for pixel. Off unless the variable is set. */
static void cover_dump(const char *path, int cw, int ch) {
    FILE *f = std::fopen(path, "wb");
    if (!f) return;
    std::fprintf(f, "P5\n%d %d\n255\n", cw, ch);
    std::vector<unsigned char> row((size_t)cw);
    for (int y = 0; y < ch; ++y) {
        for (int x = 0; x < cw; ++x) row[(size_t)x] = g_cover[y][x] ? 255 : 0;
        std::fwrite(row.data(), 1, row.size(), f);
    }
    std::fclose(f);
}

/* ---- THE TWO HELPERS OF THE WHOLE-FRAME HAND-OVER (run cardfull) ----------
 *
 * aa_scratch_alloc: aa_pass's own scratch picture, allocated the same way for a
 * backend that smooths on the card and so needs somewhere to leave the
 * pre-smoothing copy. False with one line when there is no memory, and the
 * setting is then off for the run exactly as aa_pass's own failure leaves it.
 *
 * capture_may_read: whether the display capture unit can read THIS frame, which
 * is what decides if the picture before the smoothing has to be kept. On the
 * graphics card keeping it costs a second readback of the whole picture, so it
 * is only kept when something can read it. Any one of four signs is enough, and
 * each keeps it for the next 300 frames (a dual-screen minigame captures every
 * other frame, so the bank and the arming come and go), except the last, which
 * keeps it for eight:
 *   - a capture was PERFORMED (ppu_capture_counters moved);
 *   - DISPCAPCNT is armed right now with a destination in it
 *     (Scene::ResetHardwareRegisters arms the bare enable bit at every scene
 *     boot and it is always refused; that one carries no destination);
 *   - a VRAM bank A..D is allocated to LCDC, the only place a capture can land;
 *   - the texture cache was just thrown away, which is every scene boot and the
 *     first frame of the process: the unit is armed after a scene's first
 *     frame has been rasterised, so none of the other signs can exist for it.
 *
 * THE FOURTH SIGN IS A GUESS ABOUT THE FUTURE, so it is not the last word:
 * once any capture has been performed or armed with a destination in this
 * run, the copy is kept for the REST OF THE RUN (a scene that captures once
 * will capture again, and a prediction that lapsed between two arms is exactly
 * how a capture ends up reading the smoothed picture). And a capture that did
 * land on a frame with no copy is counted and said once on stderr, so a miss
 * can never be silent. */
static bool aa_scratch_alloc() {
    if (g_aa_src) return true;
    g_aa_src = (uint32_t *)std::calloc((size_t)SCREEN_W * SCREEN_H,
                                       sizeof(uint32_t));
    if (g_aa_src) return true;
    std::fprintf(stderr,
                 "[aa] no memory for the %d x %d scratch picture; edge "
                 "smoothing is off for this run.\n", SCREEN_W, SCREEN_H);
    std::fflush(stderr);
    g_aa_mode = 0;
    return false;
}

/* gpu_full_probe: does the registered backend mean to draw this frame whole?
   Asked every frame, answered without touching anything. Not in the A/B, which
   draws the CPU passes itself first and hands the card the cleared buffers. */
static bool gpu_full_probe() {
    if (!g_gpu_opaque || ab_mode()) return false;
    if (g_bmpc.valid) return false;      /* MENUS093C (D23): a held bitmap frame is under the 3D page: the translucent pass runs here, onto that fill */
    GxGpuFrame f;
    std::memset(&f, 0, sizeof f);
    f.probe = 1;
    f.full_ok = 2;
    return g_gpu_opaque(&f) == 2;
}

static bool capture_may_read() {
    static unsigned long long last_performed = ~0ull, last_from_pre = 0;
    static uint32_t last_gen = 0;
    static int hold = 0;
    static bool sticky = false;
    unsigned long long performed = 0, refused = 0, hash = 0, from_pre = 0;
    ppu_capture_counters(performed, refused, hash, from_pre);
    /* A MISS, COUNTED. Every capture performed since the last frame should
       have read the pre-smoothing copy (this function runs on every frame the
       setting is on, so the counters only move between two calls by a capture
       of the frame before). One that did not read it saw the host's smoothing:
       say so, once, with the numbers. */
    if (last_performed != ~0ull) {
        const unsigned long long d_perf = performed - last_performed;
        const unsigned long long d_pre = from_pre - last_from_pre;
        if (d_perf > d_pre) {
            g_cap_missed += d_perf - d_pre;
            static int said;
            if (!said) {
                said = 1;
                std::fprintf(stderr,
                             "[aa] a display capture read a frame that had no "
                             "pre-smoothing copy (%llu so far); the copy is now "
                             "kept for the rest of the run.\n",
                             g_cap_missed);
                std::fflush(stderr);
            }
        }
    }
    last_from_pre = from_pre;
    /* A SCENE'S FIRST FRAMES ARE COVERED WHATEVER ELSE IS SEEN. The unit is
       armed and its bank mapped after the first frame of a scene has been
       rasterised, so neither sign below can exist yet for that frame, and a
       capture can land on it (scene 373's first capture did, with the hash
       to prove it). The texture cache is thrown away at every scene boot
       (VRAM is remapped), and the first frame of the process sees that too. */
    if (last_gen != g_tex_generation) {
        last_gen = g_tex_generation;
        if (hold < 8) hold = 8;
    }
    if (last_performed == ~0ull) last_performed = performed;
    if (performed != last_performed) {
        last_performed = performed;
        hold = 300;
        sticky = true;
    }
    if (io_ready()) {
        const uint32_t cap =
            *reinterpret_cast<const volatile uint32_t *>(0x04000064u);
        if ((cap & 0x80000000u) && (cap & 0x00FF0000u)) {
            hold = 300;
            sticky = true;
        }
        /* A CAPTURE CAN ONLY LAND IN A BANK THAT IS ALLOCATED TO LCDC (VRAMCNT
           enabled, MST 0; ppu_display_capture refuses any other), and the
           minigames map the destination there in the same breath as they arm
           the unit, so a bank in that state is a frame that may be read. */
        for (unsigned b = 0; b < 4; ++b) {
            const uint8_t c = *reinterpret_cast<const volatile uint8_t *>(0x04000240u + b);
            if ((c & 0x87u) == 0x80u) { hold = 300; break; }
        }
    }
    /* SM64DS_CAPTURE_STICKY=0 puts the old rule back (a prediction that lapses
       after 300 frames), for the A/B against the capture drill below. */
    static const bool keep_all = [] {
        const char *e = getenv("SM64DS_CAPTURE_STICKY");
        return !(e && e[0] == '0');
    }();
    if (sticky && keep_all) return true;
    if (hold > 0) {
        --hold;
        return true;
    }
    return false;
}

/* SM64DS_CAPTURE_DRILL=<n>,<n>,...: a test that arms the display capture LATE
   on a scene that has not captured, which no real scene does (every dual-screen
   minigame maps its bank a frame or more before it arms the unit). After the
   gx_render of each listed frame it maps VRAM bank D to LCDC and arms a 128 x
   128 capture into it, exactly the order the capture comment in ntr/ppu.cpp
   warns about: the arming comes after the picture is drawn and nothing before
   it said a capture was coming. Before the next gx_render it puts the bank
   back, so the bank is not itself a sign that carries into the following
   frames. Off unless the key is set. The first drilled capture finds no copy
   (nobody could have known) and says so on stderr; the later ones, however
   long after, must read the copy. */
static void capture_drill(bool after) {
    static int init = 0;
    static unsigned long long frame = 0;
    static unsigned at[16];
    static int nat = 0;
    static bool mapped = false;
    if (!init) {
        init = 1;
        if (const char *e = getenv("SM64DS_CAPTURE_DRILL")) {
            while (*e && nat < 16) {
                char *end = nullptr;
                const unsigned long v = strtoul(e, &end, 10);
                if (end == e) break;
                at[nat++] = (unsigned)v;
                e = (*end == ',') ? end + 1 : end;
            }
        }
    }
    if (!nat || !io_ready()) return;
    volatile uint8_t *bank_d = reinterpret_cast<volatile uint8_t *>(0x04000243u);
    if (!after) {
        if (mapped) {
            *bank_d = 0;
            mapped = false;
        }
        return;
    }
    for (int i = 0; i < nat; ++i)
        if (at[i] == frame) {
            *bank_d = 0x80;
            mapped = true;
            *reinterpret_cast<volatile uint32_t *>(0x04000064u) = 0x80030000u;
            std::fprintf(stderr, "[capture-drill] armed a capture after the "
                         "draw of frame %llu\n", frame);
            std::fflush(stderr);
        }
    ++frame;
}

/* THE FRAME'S DEPTH KEY: W (-(1/w)) unless SM64DS_WDEPTH=0, an orthographic
   frame or a game that did not ask for W. A card frame is W-buffered too:
   hal/gpu_raster.cpp draws c/w with a GREATER test (GxGpuFrame::wdepth), whole
   frames included, so the decision is made before the whole-frame hand-over. */
static bool frame_wdepth() {
    static int wd_env = -1;
    if (wd_env < 0) {
        const char *e = getenv("SM64DS_WDEPTH");
        wd_env = (e && *e == '0') ? 0 : 1;
    }
    return wd_env && !g_frame_ortho && (g_swap_param & 2) != 0;
}

namespace {
/* ==== THE GHOST LAYER, ON THE PROCESSOR (run mp1, lane GHOSTLAYER1) ==========
 *
 * A figure the game draws see-through (the adventure hub's other players) is NOT
 * drawn by the opaque and translucent passes. gx_render lifts its polygons
 * (GxTriangle::ghost != 0) out of the frame first, draws everything else as it
 * always did, and calls this once the translucent pass is finished and before
 * the edge smoothing runs.
 *
 * WHY: the DS draws a see-through polygon without writing depth, and refuses it
 * on a pixel that already took a see-through fragment of the same polygon id.
 * One body is many polygons under one id, so its far arm shows through its own
 * chest and parts of it vanish. This pass makes a figure ONE surface instead:
 * for every pixel only the NEAREST fragment of the figure survives (its depth is
 * tested against the frame's depth and the figure's own), and that fragment,
 * shaded exactly as the opaque pass would have shaded it, is blended once over
 * what the frame already holds, at the polygon's alpha. Nothing of the figure
 * compounds against itself, whatever the polygon order.
 *
 * AFTERWARDS the frame's planes hold what an opaque ghost always left in them:
 * the figure's depth in the depth plane (a name tag behind it hides), its
 * polygon id, and its pixels in the coverage plane. Pixels the figure does not
 * reach are never written, which keeps every non-ghost pixel the frame it was.
 *
 * Several figures go farthest first, each a layer of its own over the previous:
 * a ghost standing behind another shows through it. One thread does the lot;
 * a ghost fills a few thousand pixels.
 */
std::vector<float> g_gh_z;
std::vector<uint32_t> g_gh_col;
std::vector<uint8_t> g_gh_sa, g_gh_id;
unsigned long long g_gh_frames, g_gh_tris, g_gh_px;
double g_gh_ms;

template <int FILTER>
void ghost_group_cpu(Framebuffer &fb, const std::vector<GxTriangle> &gt,
                     const std::vector<int> &idx, bool want_id)
{
    const int px0 = present_x(), py0 = present_y();
    const int px1 = px0 + present_w() - 1, py1 = py0 + present_h() - 1;
    int bx0 = px1 + 1, by0 = py1 + 1, bx1 = px0 - 1, by1 = py0 - 1;
    for (int i : idx) {
        const GxTriangle &t = gt[(size_t)i];
        const GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
        int minx = (int)std::floor(std::fmin(a.x, std::fmin(b.x, c.x)));
        int maxx = (int)std::ceil(std::fmax(a.x, std::fmax(b.x, c.x)));
        int miny = (int)std::floor(std::fmin(a.y, std::fmin(b.y, c.y)));
        int maxy = (int)std::ceil(std::fmax(a.y, std::fmax(b.y, c.y)));
        if (minx < px0) minx = px0;
        if (miny < py0) miny = py0;
        if (maxx > px1) maxx = px1;
        if (maxy > py1) maxy = py1;
        if (maxx < minx || maxy < miny) continue;
        if (minx < bx0) bx0 = minx;
        if (miny < by0) by0 = miny;
        if (maxx > bx1) bx1 = maxx;
        if (maxy > by1) by1 = maxy;
    }
    if (bx1 < bx0 || by1 < by0) return;
    const int bw = bx1 - bx0 + 1, bh = by1 - by0 + 1;
    const size_t n = (size_t)bw * (size_t)bh;
    g_gh_z.assign(n, 1e30f);
    g_gh_col.resize(n);
    g_gh_sa.resize(n);
    g_gh_id.resize(n);
    const bool wdepth = g_wdepth;
    DepthRow *const depth = g_depth;

    for (int ti : idx) {
        const GxTriangle &t = gt[(size_t)ti];
        const GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
        const float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (std::fabs(area) < 1e-6f) continue;
        const bool backface = area > 0.0f;
        if (backface && !(t.cull & 1)) continue;
        if (!backface && !(t.cull & 2)) continue;
        int minx = (int)std::floor(std::fmin(a.x, std::fmin(b.x, c.x)));
        int maxx = (int)std::ceil(std::fmax(a.x, std::fmax(b.x, c.x)));
        int miny = (int)std::floor(std::fmin(a.y, std::fmin(b.y, c.y)));
        int maxy = (int)std::ceil(std::fmax(a.y, std::fmax(b.y, c.y)));
        if (minx < px0) minx = px0;
        if (miny < py0) miny = py0;
        if (maxx > px1) maxx = px1;
        if (maxy > py1) maxy = py1;
        const float eax = b.x - a.x, eay = b.y - a.y;
        const float ebx = c.x - b.x, eby = c.y - b.y;
        const float ecx = a.x - c.x, ecy = a.y - c.y;
        const float wq = wdepth ? w_tie_q(a, b, c) : 0.0f;   /* the main raster's W tie rule */
        const float iwa = (std::fabs(a.w) > 1e-6f) ? 1.0f / a.w : 0.0f;
        const float iwb = (std::fabs(b.w) > 1e-6f) ? 1.0f / b.w : 0.0f;
        const float iwc = (std::fabs(c.w) > 1e-6f) ? 1.0f / c.w : 0.0f;
        const bool textured = t.tex && t.tw > 0 && t.th > 0;
        const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
        const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
        const float tsc = (float)(t.tex_scale ? t.tex_scale : 1);
        TriTex tt{nullptr, 0, 0.0f};
        if (FILTER == 2 && textured) tt = lod_for(t, area);
        const float acol[3] = {(float)((a.color >> 16) & 0xFF),
                               (float)((a.color >> 8) & 0xFF),
                               (float)(a.color & 0xFF)};
        const float bcol[3] = {(float)((b.color >> 16) & 0xFF),
                               (float)((b.color >> 8) & 0xFF),
                               (float)(b.color & 0xFF)};
        const float ccol[3] = {(float)((c.color >> 16) & 0xFF),
                               (float)((c.color >> 8) & 0xFF),
                               (float)(c.color & 0xFF)};
        /* the layer's opacity is the polygon's alpha, 1..30 (31 is allowed: the
           figure then simply replaces what is under it) */
        const uint32_t poly_a = (t.alpha >= 31 || t.alpha == 0) ? 31u : t.alpha;
        const int toon = (t.mode == 2) ? g_toon_shade : 0;
        for (int y = miny; y <= maxy; ++y) {
            const float py = y + 0.5f;
            const float r0 = eax * (py - a.y);
            const float r1 = ebx * (py - b.y);
            const float r2 = ecx * (py - c.y);
            const float *drow = depth[y];
            const size_t row = (size_t)(y - by0) * (size_t)bw;
            for (int x = minx; x <= maxx; ++x) {
                const float px = x + 0.5f;
                const float n0 = r0 - eay * (px - a.x);
                const float n1 = r1 - eby * (px - b.x);
                const float n2 = r2 - ecy * (px - c.x);
                if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) ||
                      (n0 <= 0 && n1 <= 0 && n2 <= 0)) &&
                    !edge_rescue(a, b, c, px, py, r0, r1, r2, n0, n1, n2))
                    continue;
                const float w0 = n0 / area, w1 = n1 / area, w2 = n2 / area;
                const float l0 = w1, l1 = w2, l2 = w0;
                const float z = wdepth ? -(l0 * iwa + l1 * iwb + l2 * iwc)
                                       : l0 * a.z + l1 * b.z + l2 * c.z;
                if (w_depth_fail(z, drow[x], wq, false)) continue;
                const size_t o = row + (size_t)(x - bx0);
                if (z >= g_gh_z[o]) continue;
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
                    if constexpr (FILTER == 0) {
                        const int ui = tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                        const int vi = tex_coord(vv * tsc, t.th, rep_t, flip_t);
                        texel = t.tex[vi * t.tw + ui];
                    } else {
                        texel = sample_filtered<FILTER>(
                            tt, t.tex, t.tw, t.th, uu * tsc, vv * tsc, rep_s,
                            rep_t, flip_s, flip_t);
                        const int ui = tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                        const int vi = tex_coord(vv * tsc, t.th, rep_t, flip_t);
                        texel = filtered_texel_cover(texel, t.tex[vi * t.tw + ui]);
                    }
                    if ((texel >> 24) == 0) continue;
                }
                const float *te = nullptr;
                float tsm[3];
                if (toon) {
                    const float vr = l0 * acol[0] + l1 * bcol[0] + l2 * ccol[0];
                    if (g_toon_smooth) { toon_smooth_entry(vr, tsm); te = tsm; }
                    else te = &g_toon8[toon_index(vr) * 3];
                }
                auto ch = [&](int k, int sh) {
                    const int kv = toon == 2 ? 0 : k;
                    const float v = toon == 1
                        ? te[k]
                        : l0 * acol[kv] + l1 * bcol[kv] + l2 * ccol[kv];
                    const float m = inv255.v[(texel >> sh) & 0xFF];
                    int i = static_cast<int>(v * m + 0.5f);
                    if (toon == 2) i += static_cast<int>(te[k] + 0.5f);
                    return static_cast<uint32_t>(i < 0 ? 0 : (i > 255 ? 255 : i));
                };
                const uint32_t tex_a = texel >> 24;
                const uint32_t sa = (poly_a * tex_a + 127) / 255;
                g_gh_z[o] = z;
                g_gh_col[o] = (ch(0, 16) << 16) | (ch(1, 8) << 8) | ch(2, 0);
                g_gh_sa[o] = (uint8_t)(sa > 31 ? 31 : sa);
                g_gh_id[o] = t.polyid;
            }
        }
    }

    unsigned long long px = 0;
    for (int y = by0; y <= by1; ++y) {
        const size_t row = (size_t)(y - by0) * (size_t)bw;
        uint32_t *frow = fb.px[y];
        for (int x = bx0; x <= bx1; ++x) {
            const size_t o = row + (size_t)(x - bx0);
            const float z = g_gh_z[o];
            if (z >= 1e29f) continue;
            const uint32_t sa = g_gh_sa[o];
            const uint32_t col = g_gh_col[o];
            if (sa >= 31) {
                frow[x] = 0xFF000000u | col;
            } else {
                const uint32_t dst = frow[x];
                auto bl = [&](int sh) {
                    const uint32_t s = (col >> sh) & 0xFF;
                    const uint32_t d = (dst >> sh) & 0xFF;
                    return ((s * sa + d * (31 - sa)) / 31) & 0xFF;
                };
                frow[x] = 0xFF000000u | (bl(16) << 16) | (bl(8) << 8) | bl(0);
            }
            depth[y][x] = z;
            g_cover[y][x] = 1;
            if (want_id) g_attrid[y][x] = g_gh_id[o];
            ++px;
        }
    }
    g_gh_px += px;
}

void ghost_pass_cpu(Framebuffer &fb, const std::vector<GxTriangle> &gt,
                    bool want_id)
{
    if (gt.empty()) return;
    const bool tm = std::getenv("SM64DS_GHOST_LAYER_PROBE") != nullptr;
    const std::chrono::steady_clock::time_point t0 =
        tm ? std::chrono::steady_clock::now()
           : std::chrono::steady_clock::time_point();
    /* one group per figure number, farthest (largest mean w) first */
    struct Grp { int id; double w; long n; std::vector<int> idx; };
    std::vector<Grp> grp;
    for (size_t i = 0; i < gt.size(); ++i) {
        const GxTriangle &t = gt[i];
        Grp *g2 = nullptr;
        for (Grp &q : grp)
            if (q.id == t.ghost) { g2 = &q; break; }
        if (!g2) {
            grp.push_back(Grp{t.ghost, 0.0, 0, {}});
            g2 = &grp.back();
        }
        g2->idx.push_back((int)i);
        g2->w += (double)t.v[0].w + (double)t.v[1].w + (double)t.v[2].w;
        g2->n += 3;
    }
    for (Grp &q : grp) q.w /= (double)(q.n ? q.n : 1);
    std::sort(grp.begin(), grp.end(),
              [](const Grp &x, const Grp &y) { return x.w > y.w; });
    for (const Grp &q : grp) {
        switch (g_tex_filter) {
        case 1: ghost_group_cpu<1>(fb, gt, q.idx, want_id); break;
        case 2: ghost_group_cpu<2>(fb, gt, q.idx, want_id); break;
        default: ghost_group_cpu<0>(fb, gt, q.idx, want_id); break;
        }
    }
    ++g_gh_frames;
    g_gh_tris += gt.size();
    if (tm) {
        g_gh_ms += std::chrono::duration<double, std::milli>(
                       std::chrono::steady_clock::now() - t0).count();
        if ((g_gh_frames % 60) == 1)
            std::fprintf(stderr, "[ghostlayer] cpu pass: frames %llu figures %zu "
                         "tris %llu pixels %llu mean %.3f ms\n", g_gh_frames,
                         grp.size(), g_gh_tris, g_gh_px,
                         g_gh_ms / (double)g_gh_frames);
    }
}

}  // namespace

/* run after093, lane MENU2: the late clears and the deferred readback's seam (see gx_clear_fill_late in gx.h) */
static void late_run(LateCtx *c)
{
    if (!c || c->ran) return;
    c->ran = true;
    /* a picture older than this frame may still be waiting to be finished and shown: it owns fb until then */
    if (g_flush_hook) g_flush_hook();
    clear_fill_classic(*c->fb);
    for (int y = 0; y < c->ch; ++y) std::memset(g_cover[y], 0, (size_t)c->cw);
}
static void late_run_cb(void *p) { late_run((LateCtx *)p); }

int gx_late_possible(void)
{
    static int env = -2;
    if (env == -2) env = gx_menu_knob_value("SM64DS_MENU_LATE");
    static const bool tex_only = std::getenv("SM64DS_TEX_ONLY") != nullptr;
    return gx_menu_knob_on(env) && !tex_only && gpu_full_probe() ? 1 : 0;
}
int gx_readback_pending(void)
{
    if (!g_gpu_opaque) return 0;
    GxGpuFrame f;
    std::memset(&f, 0, sizeof f);
    f.finish = 2;
    return g_gpu_opaque(&f);
}
void gx_finish_readback(void)
{
    if (!g_gpu_opaque) return;
    GxGpuFrame f;
    std::memset(&f, 0, sizeof f);
    f.finish = 1;
    g_gpu_opaque(&f);
}
void gx_aa_note_changed(unsigned long long n) { g_aa_changed += n; }

void gx_render(Framebuffer &fb) {
    /* kept grids stay kept only for a frame the card draws whole */
    if (!g.grids.empty() &&
        !(grid_env_ok() && g_gpu_opaque && !ab_mode() && gpu_grid_probe()))
        grids_materialize_w(3);
    capture_drill(false);
    /* run interp1: what reached the raster is the frame; seal its record. */
    const bool sealed_now = g_ip_rec != 0;
    if (g_ip_rec) ip_rec_seal();
    /* LAST FRAME'S PRE-SMOOTHING COPY STOPS BEING THIS FRAME'S HERE, before
       anything is drawn. A capture that somehow ran against a frame this
       function never finished would otherwise sample the frame before it. */
    g_aa_pre_valid = 0;
    g_planes_stale = 0;
    /* THE GHOST LAYER: lift the ghost figures out of the frame (run mp1, lane
       GHOSTLAYER1; see ghost_pass_cpu). They are put back at the end, so the
       list is the one the frame was submitted as for whatever reads it after. */
    static std::vector<GxTriangle> s_ghost;
    s_ghost.clear();
    const GxTriangle *gl_p = nullptr;      /* the lifted figures: s_ghost, or the tick record's own copy */
    size_t gl_n = 0;
    bool gl_alias = false;
    if (g_ghost_layer_on) {
        size_t nghost = 0;
        for (const GxTriangle &t : g.tris)
            if (t.ghost) ++nghost;
        /* a picture between ticks keeps its figures apart (ip_ghost_emit): when the frame list holds none of its own they are read from
           where they were made, with no lift and no append back; SM64DS_GHOST_DIVERT=0 puts them in the list as before */
        size_t dn = 0;
        const GxTriangle *dp = (g_ip_open && nghost == 0) ? ip_diverted_ghosts(dn) : nullptr;
        if (dp && dn) { gl_p = dp; gl_n = dn; gl_alias = true; }
        else if (!g_ip_open && nghost == 0 && sealed_now) {   /* the tick made its figures straight into the record */
            size_t tn = 0;
            const GxTriangle *tp = ip_sealed_ghosts(tn);
            static const int a_on = (std::getenv("SM64DS_GHOST_ALIAS") && *std::getenv("SM64DS_GHOST_ALIAS") == '0') ? 0 : 1;
            if (a_on && tp && tn) { gl_p = tp; gl_n = tn; gl_alias = true; }
        }
        const bool any = nghost != 0 || (g_ip_open && !gl_alias && ip_diverted_ghosts(dn) && dn);
        if (any) {
            if (g_ip_open) {   /* both kinds in one picture: the diverted ones join the lifted ones */
                size_t dn2 = 0;
                const GxTriangle *dp2 = ip_diverted_ghosts(dn2);
                if (dp2 && dn2) { s_ghost.reserve(nghost + dn2); s_ghost.insert(s_ghost.end(), dp2, dp2 + dn2); }
            }
            /* run hunt8, lane GHOSTPERF1: the tick's record (ip_ghost_save, just sealed) already holds exactly these triangles in this
               order; the picture reads them from there instead of a second copy of ~4 MB, and they are not appended back (the
               frame's list is cleared at the next frame and the record has them). SM64DS_GHOST_ALIAS=0 is the old copy. */
            static const int alias_on = (std::getenv("SM64DS_GHOST_ALIAS") && *std::getenv("SM64DS_GHOST_ALIAS") == '0') ? 0 : 1;
            static const int dbg_env = (std::getenv("SM64DS_GHOST_LAYER_PROBE") || std::getenv("SM64DS_GHOST_LAYER_HIDE")) ? 1 : 0;
            size_t rn = 0;
            const GxTriangle *rp = (alias_on && !dbg_env && sealed_now) ? ip_sealed_ghosts(rn) : nullptr;
            gl_alias = rp && rn == nghost && !g_ip_open;
            if (gl_alias) { gl_p = rp; gl_n = nghost; }
            else s_ghost.reserve(s_ghost.size() + nghost);
            size_t w = 0;
            for (size_t i = 0; i < g.tris.size(); ++i) {
                if (g.tris[i].ghost) {
                    if (!gl_alias) s_ghost.push_back(g.tris[i]);
                } else {
                    if (w != i) g.tris[w] = g.tris[i];
                    ++w;
                }
            }
            g.tris.resize(w);
        }
    }
    /* SM64DS_GHOST_LAYER_PROBE: one line a second or so with the figures'
       screen box (the proof rows find the ghost in the picture with it);
       SM64DS_GHOST_LAYER_HIDE: leave the figures out of the picture entirely,
       which is the background the proof rows compare the layer against.
       Neither does anything unless the variable is set. */
    if (!s_ghost.empty()) {
        static const int probe = std::getenv("SM64DS_GHOST_LAYER_PROBE") ? 1 : 0;
        static const int hide = std::getenv("SM64DS_GHOST_LAYER_HIDE") ? 1 : 0;
        static unsigned long long nprobe;
        if (probe && (nprobe++ % 40) == 0) {
            float x0 = 1e30f, y0 = 1e30f, x1 = -1e30f, y1 = -1e30f;
            for (const GxTriangle &t : s_ghost)
                for (int k = 0; k < 3; ++k) {
                    if (t.v[k].x < x0) x0 = t.v[k].x;
                    if (t.v[k].x > x1) x1 = t.v[k].x;
                    if (t.v[k].y < y0) y0 = t.v[k].y;
                    if (t.v[k].y > y1) y1 = t.v[k].y;
                }
            std::fprintf(stderr, "[ghostlayer] box %d %d %d %d tris %zu\n",
                         (int)x0, (int)y0, (int)x1, (int)y1, s_ghost.size());
        }
        if (hide) s_ghost.clear();
    }
    if (!gl_alias) { gl_p = s_ghost.data(); gl_n = s_ghost.size(); }
    const bool have_ghost = gl_n != 0;
    const int tm = frame_ms();
    std::chrono::steady_clock::time_point t_enter;
    if (tm) t_enter = std::chrono::steady_clock::now();
    tri_report();
    texpx_report();
    gx_mip_report();
    mat_report();
    mtx_report(false);
    /* ---- THE PER-FRAME CLEARS ARE OVER THE LIVE PICTURE, NOT THE BUFFER ----
       Every buffer here is allocated at SCREEN_W x SCREEN_H, the largest
       extent any settings combination can ask for, and the picture is
       active_w x active_h in its top-left corner. Clearing the whole
       allocation was clearing rows and columns nothing reads: the raster's
       bounding box is clamped to the present rectangle, which is inside the
       active extent, and the 2D compositor and the display capture both loop
       to active_w / active_h. So the clear is the active rectangle and the
       rest of the allocation is left holding last frame's numbers, which no
       pass can reach.

       IT IS A SPEED FIX AND NOT A PIXEL ONE, and it is the fix that lets the
       allocation grow for RenderScale 4 without making the DEFAULT run
       slower: at 512x384 in a 1368x768 allocation this is 196 KB of coverage
       and 768 KB of depth a frame instead of 1.05 MB and 4.2 MB. Measured
       per-frame numbers and the byte-identical BMP proof are in the lane's
       report.

       cw/ch are clamped to the allocation rather than trusted for the reason
       every other clamp in this path exists: a wrong extent here is a write
       past a static array. */
    const int cw = active_w > 0 ? (active_w < SCREEN_W ? active_w : SCREEN_W) : 0;
    const int ch = active_h > 0 ? (active_h < SCREEN_H ? active_h : SCREEN_H) : 0;

    /* THE DEPTH CLEAR MOVED DOWN (run cardfull): it now sits below the early
       whole-frame card call, so a frame the card draws whole never pays for
       it. Nothing between here and there reads depth. */
    raster_buffers();
    DepthRow *const depth = g_depth;

    /* THE 3D COVERAGE MASK, see gx_coverage() in ntr/gx.h. One byte per pixel,
       set beside every store into fb.px below and cleared here. It is what
       lets the 2D compositor tell a pixel this engine drew from a pixel the
       frame's clear left, which is the whole of "a BG at priority 3 sits
       BEHIND the 3D layer at priority 1".
       It is written from the raster tiles, and that is safe for the reason
       the framebuffer itself is: a tile owns its rows and no other tile
       touches them. */
    LateCtx *lc = nullptr;     /* MENU2: this frame's clears are late (gx_clear_fill_late) */
    if (g_late.pending && g_late.fb == &fb) {
        lc = &g_late_ctx[g_late_i++ & 3];
        lc->fb = &fb; lc->cw = cw; lc->ch = ch; lc->ran = false;
        g_late.pending = false;
    }
    if (!lc)
        for (int y = 0; y < ch; ++y) std::memset(g_cover[y], 0, (size_t)cw);

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
    MaskRow *const stencil = g_stencil;
    MaskRow *const attrid = g_attrid;
    /* The DS attribute word's OTHER half, the translucent one: bit 6 here says
       this pixel has already taken a translucent fragment THIS FRAME and bits
       0..5 are that fragment's polygon ID. The hardware refuses a translucent
       fragment whose (flag, ID) already sits at the pixel, so a pixel takes one
       blend per polygon ID and a figure's own overlapping surfaces never
       compound against each other. melonDS's software renderer is the same
       field and the same refusal: PlotTranslucentPixel builds
       ((polyattr >> 8) & 0xFF0000) | (1<<22) and returns early when
       (dstattr & 0x007F0000) == (attr & 0x007F0000), "skip if translucent
       polygon IDs are equal". Without it the opening cutscene's Peach, 798
       triangles under one polygon ID with a driven opacity, blends 1.4 times
       per covered pixel and comes out solid and patchy where she crosses
       herself. The flag is per frame (hardware clears it on the frame clear and
       on any opaque write), so the clear below is the whole of its lifetime. */
    MaskRow *const tlattr = g_tlattr;
    bool have_shadow = false;
    bool have_translucent = false;
    for (const GxTriangle &t : g.tris) {
        if (t.mode == 3) have_shadow = true;
        if (t.translucent) have_translucent = true;
        if (have_shadow && have_translucent) break;
    }
    if (g.grid_translucent) have_translucent = true;
    /* THE TOON TABLE AND DISP3DCNT's SHADING BIT, read once for the frame and
       only when a mode-2 polygon is in it (see gx_toon_table in ntr/gx.h). A
       frame without one leaves g_toon_shade 0 and every band below takes the
       plain modulation it always took. */
    {
        static const int rp = std::getenv("SM64DS_RENDER_PROBE") ? 1 : 0;
        if (rp && io_ready()) {
            static unsigned long fr = 0, lines = 0;
            static uint32_t last[6];
            ++fr;
            const uint32_t cur[6] = {
                *reinterpret_cast<const volatile uint16_t *>(0x04000060u),
                *reinterpret_cast<const volatile uint32_t *>(0x04000358u),
                *reinterpret_cast<const volatile uint16_t *>(0x0400035Cu),
                *reinterpret_cast<const volatile uint32_t *>(0x04000360u),
                (uint32_t)(g_rp_tl_dw ? 1 : 0) | (uint32_t)(g_rp_fog_poly ? 2 : 0),
                g_swap_param};
            bool ch = false;
            for (int i = 0; i < 6; ++i) if (cur[i] != last[i]) ch = true;
            if ((fr % 300) == 1) {
                unsigned char fb8[8] = {0};
                const int nrec = port_fog_probe_hook ? port_fog_probe_hook(fb8) : -1;
                std::fprintf(stderr, "[rprobe] level fog records %d, first %02x %02x %02x %02x %02x %02x %02x %02x\n", nrec,
                             fb8[0], fb8[1], fb8[2], fb8[3], fb8[4], fb8[5], fb8[6], fb8[7]);
            }
            if ((ch || (fr % 300) == 1) && lines < 200) {
                ++lines;
                for (int i = 0; i < 6; ++i) last[i] = cur[i];
                std::fprintf(stderr, "[rprobe] frame %lu DISP3DCNT %04x FOGCOL %08x FOGOFF %04x FOGTAB0 %08x "
                             "tl_total %lu tl_depthwrite %lu fogpoly %lu swap %x w=%d\n", fr,
                             (unsigned)cur[0], (unsigned)cur[1], (unsigned)cur[2], (unsigned)cur[3],
                             g_rp_tl, g_rp_tl_dw, g_rp_fog_poly, (unsigned)cur[5], (int)g_wdepth);
            }
        }
    }
    fog_latch();
    g_toon_shade = 0;
    {
        size_t n2 = g.grid_mode2;
        for (const GxTriangle &t : g.tris)
            if (t.mode == 2) ++n2;
        for (size_t gi_ = 0; gi_ < gl_n; ++gi_)
            if (gl_p[gi_].mode == 2) ++n2;
        if (n2) g_toon_shade = gx_toon_table(g_toon8);
        toon_blend_setup();
        /* SM64DS_TOON_DUMP=<file>: every distinct table a frame draws with,
           once, as its 32 BGR555 entries and the blend flags (lane
           SMOOTHTOON1's table survey) */
        static const char *tdump = std::getenv("SM64DS_TOON_DUMP");
        if (tdump && n2 && io_ready()) {
            static uint32_t seen[64];
            static int nseen = 0;
            static unsigned long dframes = 0;
            ++dframes;
            const volatile uint16_t *tt =
                reinterpret_cast<const volatile uint16_t *>(0x04000380u);
            uint32_t hsh = 2166136261u ^ (uint32_t)g_toon_shade;
            for (int i = 0; i < 32; ++i) hsh = (hsh ^ (uint32_t)(tt[i] & 0x7FFF)) * 16777619u;
            bool known = false;
            for (int i = 0; i < nseen; ++i) if (seen[i] == hsh) known = true;
            if (!known && nseen < 64) {
                seen[nseen++] = hsh;
                if (FILE *fp = std::fopen(tdump, "a")) {
                    std::fprintf(fp, "%08x\t%lu\t%s\t%u\t", (unsigned)hsh, dframes,
                                 g_toon_shade == 2 ? "highlight" : "toon", (unsigned)n2);
                    for (int i = 0; i < 32; ++i)
                        std::fprintf(fp, "%04x%s", (unsigned)(tt[i] & 0x7FFF), i < 31 ? " " : "\t");
                    /* per gap: 0 the two entries are equal, 1 a step that
                       blends, 2 a step that keeps its hard edge (or the
                       switch is off) */
                    for (int i = 0; i < 31; ++i) {
                        const float *a = &g_toon8[i * 3];
                        const bool same = a[0] == a[3] && a[1] == a[4] && a[2] == a[5];
                        std::fprintf(fp, "%d", same ? 0 : (g_toon_blend[i] != 0.0f ? 1 : 2));
                    }
                    std::fprintf(fp, "\t%d\n", g_toon_smooth);
                    std::fclose(fp);
                }
            }
        }
        static const int tprobe = std::getenv("SM64DS_TOON_PROBE") ? 1 : 0;
        if (tprobe && n2) {
            static unsigned long frames = 0, lines = 0;
            static uint32_t last = 0;
            ++frames;
            uint32_t hsh = 2166136261u;
            for (int i = 0; i < 96; ++i)
                hsh = (hsh ^ (uint32_t)g_toon8[i]) * 16777619u;
            if ((hsh != last || (frames % 60) == 1) && lines < 400) {
                ++lines;
                last = hsh;
                std::fprintf(stderr,
                             "[toon] frame-with-mode2 %lu: %u mode-2 triangles, "
                             "shade %d (%s), table %08x, entries 0/8/16/24 "
                             "%02x%02x%02x %02x%02x%02x %02x%02x%02x %02x%02x%02x\n",
                             frames, (unsigned)n2, g_toon_shade,
                             g_toon_shade == 1 ? "toon" : g_toon_shade == 2
                                 ? "highlight" : "off",
                             (unsigned)hsh,
                             (unsigned)g_toon8[0], (unsigned)g_toon8[1], (unsigned)g_toon8[2],
                             (unsigned)g_toon8[24], (unsigned)g_toon8[25], (unsigned)g_toon8[26],
                             (unsigned)g_toon8[48], (unsigned)g_toon8[49], (unsigned)g_toon8[50],
                             (unsigned)g_toon8[72], (unsigned)g_toon8[73], (unsigned)g_toon8[74]);
            }
        }
    }
    /* The same active-rectangle clear as the depth and coverage buffers
       above, and these two are already conditional on the frame submitting a
       shadow or a translucent polygon at all. */
    /* THE POLYGON-ID BUFFER IS ALSO WHAT THE A/B COMPARES, so it is cleared
       and filled on a frame the A/B is checking even when no shadow volume
       asked for it. With the A/B off -- every run that is not measuring the
       graphics-card renderer -- want_id is have_shadow and nothing about this
       block or the write in the band below has changed. */
    const bool want_id = have_shadow || (g_gpu_opaque && ab_mode());

    /* ---- THE WHOLE FRAME ON THE CARD, ASKED FOR EARLY (run cardfull) --------
       A backend that draws the translucent pass and the edge smoothing too
       says so beforehand (gpu_full_probe), and is then asked for the whole
       frame BEFORE any of the CPU raster's own preparation: the depth, polygon
       ID, stencil and translucent-ID planes it never reads, the trilinear
       levels and the bins of the tiles it never runs. A frame that comes
       back drawn skips all of that and both CPU passes. One that does not (the
       card fell back, or declined) carries on below exactly as it always did,
       and the opaque-only call at the seam is still there for it. With no
       backend registered, and in the A/B, none of this runs. */
    int gpu_drew = 0;
    /* 1 when the card drew the translucent and shadow pass as well (and maybe
       the edge smoothing: gpu_out.aa_done), so neither CPU pass runs below */
    int gpu_full = 0;
    GxGpuOut gpu_out;
    std::memset(&gpu_out, 0, sizeof gpu_out);
    static const bool tex_only_env = getenv("SM64DS_TEX_ONLY") != nullptr;
    if (gpu_full_probe() && !tex_only_env) {
        GxGpuFrame f;
        std::memset(&f, 0, sizeof f);
        f.tris = g.tris.data();
        f.count = g.tris.size();
        f.grids = g.grids.data();
        f.ngrids = g.grids.size();
        f.arena = g.grid_pts.data();
        f.arena_floats = g.grid_pts.size();
        f.fb = &fb.px[0][0];
        f.depth = &g_depth[0][0];
        f.cover = &g_cover[0][0];
        f.attrid = &g_attrid[0][0];
        f.stride = SCREEN_W;
        f.cw = cw;
        f.ch = ch;
        f.px0 = present_x();
        f.py0 = present_y();
        f.pw = present_w();
        f.ph = present_h();
        if (g_menu_scene_cb && menu_wide()) {
            /* MENUS093 (D6): the title's full-canvas scene is drawn over the whole wide frame, so the whole frame is its present rectangle
               for the card's composite: pixels outside the 4:3 box were replaced by the old clear colour, which cut the drawing page's pinned
               Mario button to a one-pixel sliver (the processor path drew it whole) */
            f.px0 = 0; f.py0 = 0; f.pw = cw; f.ph = ch;
        }
        {
            /* the software raster's clip: the present rectangle narrowed to
               the game's viewport (the ending's letterbox); the card scissors
               to it, the frame's own geometry stays the present rectangle */
            int cx0, cy0, cx1, cy1;
            raster_clip(cx0, cy0, cx1, cy1);
            f.clip_x0 = cx0;
            f.clip_y0 = cy0;
            f.clip_x1 = cx1 + 1;
            f.clip_y1 = cy1 + 1;
        }
        f.wdepth = frame_wdepth() ? 1 : 0;
        f.want_attrid = 0;
        f.want_depth = 0;
        f.tex_filter = g_tex_filter;
        f.tex_generation = g_tex_generation;
        f.clear_argb = (g_cp_pending || lc) ? gx_clear_argb()
                       : (cw > 0 && ch > 0) ? fb.px[f.py0][f.px0] : 0xFF000000u;
        if (lc) {
            f.late = 1;
            f.defer = g_defer_req;
            f.pre_read = late_run_cb;
            f.pre_arg = lc;
        }
        f.toon_shade = g_toon_shade;
        f.toon_rgb = g_toon8;
        f.toon_blend = g_toon_blend;
        f.fog_on = g_fog.on;
        f.fog_wscale = frame_wdepth() ? g_fog.wscale : 0.0f;
        f.fog_shift = g_fog.shift;
        f.fog_offset = g_fog.offset;
        for (int fi = 0; fi < 3; ++fi) f.fog_rgb[fi] = g_fog.rgb[fi];
        for (int fi = 0; fi < 34; ++fi) f.fog_tab[fi] = g_fog.tab[fi];
        f.full_ok = 2;
        f.out = &gpu_out;
        f.keep = g_cp_pending;
        f.outside_argb = kOldClear;
        f.ghost_tris = gl_p;
        f.ghost_count = gl_n;
        if (g_aa_mode && aa_scratch_alloc()) {
            f.aa_mode = 1;
            f.pre = g_aa_src;
            f.pre_needed = capture_may_read() ? 1 : 0;
        }
        if (g_gpu_opaque(&f) == 2) {
            gpu_full = 1;
            g_planes_stale = 1;
            if (g_cp_pending && gpu_out.kept) {
                g_cp_active = 1;
                g_cp_pre_needed = f.pre_needed;
                g_cp_geo.cw = cw;
                g_cp_geo.ch = ch;
                g_cp_geo.px0 = f.px0;
                g_cp_geo.py0 = f.py0;
                g_cp_geo.pw = f.pw;
                g_cp_geo.ph = f.ph;
            }
        }
    }
    /* MENU2: the card did not take the late clears (it declined the frame): they run now, after any older picture is out of fb */
    g_render_deferred = gpu_out.deferred ? 1 : 0;
    if (lc && !lc->ran && !gpu_out.deferred) late_run(lc);
    /* a frame the card did not draw whole needs its triangles */
    if (!gpu_full) grids_materialize_w(4);
    /* A frame the card did not keep goes on as an ordinary one: the CPU passes
       draw into the clear colour, which the transparent fill did not leave */
    if (g_cp_pending && !g_cp_active) clear_fill_classic(fb);
    g_cp_pending = 0;
    if (!gpu_full) {
        /* Depth clear. 1e30f is not a repeating byte pattern so memset cannot
           do it, but one row can be built scalar and the rest copied from it,
           which is memcpy's problem rather than a scalar loop's. */
        for (int x = 0; x < cw; ++x) depth[0][x] = 1e30f;
        for (int y = 1; y < ch; ++y)
            std::memcpy(depth[y], depth[0], (size_t)cw * sizeof(float));
        if (have_shadow)
            for (int y = 0; y < ch; ++y) std::memset(stencil[y], 0, (size_t)cw);
        if (want_id) {
            /* 0 is the clear plane's polygon ID (CLEAR_COLOR bits 24-29 reset
               value); pixels no opaque polygon reaches keep it. */
            for (int y = 0; y < ch; ++y) std::memset(attrid[y], 0, (size_t)cw);
        }
        if (have_translucent)
            for (int y = 0; y < ch; ++y) std::memset(tlattr[y], 0, (size_t)cw);
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

    /* ---- THE FRAME'S LEVELS OF DETAIL, ONCE, ON THIS THREAD (run hd2) -----
       Trilinear needs to know, for each triangle, which two sizes of its
       texture the surface sits between. That is a map lookup and a logarithm
       per triangle; doing it inside the bands would do it once per triangle
       PER THREAD, and doing it per pixel would put a logarithm in the fill
       loop. So it happens here, single-threaded, before any band starts, and
       the bands only read it. Nothing below runs at filter 0 or 1: the vector
       is never even sized. */
    const int filt = g_tex_filter;
    if (filt == 2 && !gpu_full) {
        g_tritex.assign(g.tris.size(), TriTex{nullptr, 0, 0.0f});
        for (size_t i = 0; i < g.tris.size(); ++i) {
            const GxTriangle &t = g.tris[i];
            if (!t.tex || t.tw <= 0 || t.th <= 0) continue;
            const GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
            const float ar =
                (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
            g_tritex[i] = lod_for(t, ar);
        }
    }

    /* One row tile: rows tile * tile_h up to the next tile's first.
       TWO PASSES, the hardware's own order: every opaque polygon first,
       then the translucent ones, submission order kept within each pass.
       The game leans on this -- the castle moat's water submits before
       the terrain under it, and in one pass the opaque moat bed painted
       over the already-blended surface. Rows are disjoint across threads,
       so each thread runs both passes over its own rows and never sees
       another thread's pixels. */
    /* WHICH OF THE TWO PASSES THIS BAND RUNS. 0 and 1 is both of them, which
       is every run that draws its own opaque pass and is the same loop as
       before these two variables existed. A frame the graphics-card renderer
       drew sets the low bound to 1, so the band runs the translucent and
       shadow pass alone over the buffers the card filled. */
    int pass_lo = 0, pass_hi = 1;
    g_wdepth = frame_wdepth();
    /* the tile shape, set below once the frame's work is known */
    int tile_h = SCREEN_H, ntiles = 1, nt = 1;
    auto band_impl = [&](int tile, auto ftag) {
    /* THE SAMPLER'S MODE IS A COMPILE-TIME CONSTANT IN HERE, which is the
       whole point of the shape (run hd2). The body below is instantiated once
       per mode and the mode is chosen once per band, so the nearest body --
       the one every run with the key absent takes -- carries no test for a
       filter, no branch per pixel and no call through a pointer: it is the
       instruction stream it was before filtering existed. */
    constexpr int FILTER = decltype(ftag)::value;
    const bool wdepth = g_wdepth;
    /* This tile's rows. */
    const int ty0 = tile * tile_h;
    const int ty1 = (ty0 + tile_h < SCREEN_H ? ty0 + tile_h : SCREEN_H) - 1;
    auto clear_stencil = [&] {
        for (int y = ty0; y <= ty1; ++y) std::memset(stencil[y], 0, SCREEN_W);
    };
    for (int pass = pass_lo; pass <= pass_hi; ++pass) {
    /* The stencil clears when a NEW mask group begins -- a mask polygon
       arriving after any non-mask polygon -- so one volume's leftover bits
       cannot leak into the next volume's draw. Where those points fall in
       the list is worked out once, beside the bins; this tile replays them
       over its own rows. */
    const bool clr_pass = have_shadow && pass == 1;
    int clr_done = 0;
    const int *bi = g_bin_idx[pass].data() + g_bin_off[pass][(size_t)tile];
    const int *const be =
        g_bin_idx[pass].data() + g_bin_off[pass][(size_t)tile + 1];
    for (; bi != be; ++bi) {
        const GxTriangle &t = g.tris[(size_t)*bi];
        if (clr_pass && g_clr_cum[(size_t)*bi] != clr_done) {
            clear_stencil();
            clr_done = g_clr_cum[(size_t)*bi];
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
        /* CLAMPED TO THE PRESENT RECTANGLE. There is no side clip against the
           viewport in this raster -- off-screen geometry has always been
           bounded by the screen edge, which at every other aspect IS the
           viewport edge. Inside a pillarbox they are different rectangles, and
           a triangle that runs past the 4:3 edge would otherwise paint into
           the margin. present_* is (0,0,active_w,active_h) on every other run,
           so these are the same four compares as before. */
        int px0, py0, px1, py1;
        raster_clip(px0, py0, px1, py1);
        if (minx < px0) minx = px0;
        if (miny < py0) miny = py0;
        if (maxx > px1) maxx = px1;
        if (maxy > py1) maxy = py1;

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
        /* THE ROW RANGE SETUP, see row_span: per triangle, the three edge
           slopes, the three vertices they are measured from and the
           reciprocals the crossing estimates use. */
        const bool use_span = maxx - minx >= 8 && span_ok(a, b, c);
        const bool big_tri = big_tri_test(a, b, c);
        SpanTri st;
        if (use_span) {
            st.ey[0] = eay; st.ey[1] = eby; st.ey[2] = ecy;
            st.vx[0] = a.x; st.vx[1] = b.x; st.vx[2] = c.x;
            for (int k = 0; k < 3; ++k)
                st.inv[k] = st.ey[k] != 0.0f ? 1.0 / (double)st.ey[k] : 0.0;
        }
        const float iwa = (std::fabs(a.w) > 1e-6f) ? 1.0f / a.w : 0.0f;
        const float iwb = (std::fabs(b.w) > 1e-6f) ? 1.0f / b.w : 0.0f;
        const float iwc = (std::fabs(c.w) > 1e-6f) ? 1.0f / c.w : 0.0f;
        const float wq = wdepth ? w_tie_q(a, b, c) : 0.0f;
        /* NOTE the UV terms below stay written as l0 * a.u * iwa. Folding
           a.u * iwa out to the triangle would regroup the multiply, and float
           multiplication does not associate -- (l0*a.u)*iwa and l0*(a.u*iwa)
           are different numbers. Only the reciprocal itself is hoisted. */
        const bool textured = t.tex && t.tw > 0 && t.th > 0;
        const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
        const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
        /* Host pixels per DS texel (GxTriangle::tex_scale). The UVs the
           geometry engine produced are in DS TEXELS, and tw/th are the bound
           buffer's real pixel dimensions, so the sampler works in buffer
           pixels by multiplying. 1 for everything the ROM supplies, and a
           multiply by exactly 1.0f returns its operand bit for bit, so the
           default run samples the texel it always sampled. */
        const float tsc = (float)(t.tex_scale ? t.tex_scale : 1);
        /* WHICH CHAIN LEVELS THIS TRIANGLE SITS BETWEEN, looked up rather than
           computed: the head of gx_render already resolved it once for every
           triangle in the frame. Nothing here runs at any other filter mode --
           FILTER is a constant, so the whole statement folds away. */
        TriTex tt{nullptr, 0, 0.0f};
        if (FILTER == 2 && textured)
            tt = g_tritex[(size_t)(&t - g.tris.data())];
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
        /* MODE 2 (toon / highlight): 0 for every other polygon and for every
           frame with no mode-2 polygon, which is the plain modulation below */
        const int toon = (t.mode == 2) ? g_toon_shade : 0;

        /* first row of this band at or after miny */
        /* this tile's rows of the triangle */
        const int y_lo = miny > ty0 ? miny : ty0;
        const int y_hi = maxy < ty1 ? maxy : ty1;

        if (t.mode == 3) {
            /* Shadow polygons, the two-step protocol from the block comment
               at the buffers above. Both loops share the standard loop's
               coverage and barycentric arithmetic -- same expressions, same
               order -- and neither ever writes depth. */
            if (t.polyid == 0) {
                /* the mask: set stencil where the depth test FAILS; no
                   colour, no depth, no texture */
                for (int y = y_lo; y <= y_hi; ++y) {
                    const float py = y + 0.5f;
                    const float r0 = eax * (py - a.y);
                    const float r1 = ebx * (py - b.y);
                    const float r2 = ecx * (py - c.y);
                    const float *drow = depth[y];
                    uint8_t *srow = stencil[y];
                    int xs = minx, xe = maxx;
                    const bool sp_ok = !use_span || row_span(st, r0, r1, r2, minx, maxx, xs, xe);
                    if (big_tri && !big_row(a, b, c, py, minx, maxx, sp_ok, xs, xe))
                        continue;
                    if (!big_tri && !sp_ok)
                        continue;
                    for (int x = xs; x <= xe; ++x) {
                        const float px = x + 0.5f;
                        const float n0 = r0 - eay * (px - a.x);
                        const float n1 = r1 - eby * (px - b.x);
                        const float n2 = r2 - ecy * (px - c.x);
                        if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) ||
                              (n0 <= 0 && n1 <= 0 && n2 <= 0)) &&
                              !edge_rescue(a, b, c, px, py, r0, r1, r2, n0, n1, n2))
                            continue;
                        const float w0 = n0 / area, w1 = n1 / area,
                                    w2 = n2 / area;
                        const float l0 = w1, l1 = w2, l2 = w0;
                        const float z = wdepth ? -(l0 * iwa + l1 * iwb + l2 * iwc) : l0 * a.z + l1 * b.z + l2 * c.z;
                        if (z >= drow[x]) srow[x] = 1;
                    }
                }
            } else {
                /* the drawn shadow: examine stencilled pixels, clear the
                   bit whether it draws or not, blend where the depth test
                   passes and the recorded polygon ID differs */
                for (int y = y_lo; y <= y_hi; ++y) {
                    const float py = y + 0.5f;
                    const float r0 = eax * (py - a.y);
                    const float r1 = ebx * (py - b.y);
                    const float r2 = ecx * (py - c.y);
                    const float *drow = depth[y];
                    uint8_t *srow = stencil[y];
                    const uint8_t *irow = attrid[y];
                    uint32_t *frow = fb.px[y];
                    int xs = minx, xe = maxx;
                    const bool sp_ok = !use_span || row_span(st, r0, r1, r2, minx, maxx, xs, xe);
                    if (big_tri && !big_row(a, b, c, py, minx, maxx, sp_ok, xs, xe))
                        continue;
                    if (!big_tri && !sp_ok)
                        continue;
                    for (int x = xs; x <= xe; ++x) {
                        const float px = x + 0.5f;
                        const float n0 = r0 - eay * (px - a.x);
                        const float n1 = r1 - eby * (px - b.x);
                        const float n2 = r2 - ecy * (px - c.x);
                        if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) ||
                              (n0 <= 0 && n1 <= 0 && n2 <= 0)) &&
                              !edge_rescue(a, b, c, px, py, r0, r1, r2, n0, n1, n2))
                            continue;
                        if (!srow[x]) continue;
                        srow[x] = 0;
                        const float w0 = n0 / area, w1 = n1 / area,
                                    w2 = n2 / area;
                        const float l0 = w1, l1 = w2, l2 = w0;
                        const float z = wdepth ? -(l0 * iwa + l1 * iwb + l2 * iwc) : l0 * a.z + l1 * b.z + l2 * c.z;
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
                            const int ui = tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                            const int vi = tex_coord(vv * tsc, t.th, rep_t, flip_t);
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
        for (int y = y_lo; y <= y_hi; ++y) {
            const float py = y + 0.5f;
            /* the half of each edge function that only moves with the row */
            const float r0 = eax * (py - a.y);
            const float r1 = ebx * (py - b.y);
            const float r2 = ecx * (py - c.y);
            float *drow = depth[y];
            uint32_t *frow = fb.px[y];
            uint8_t *irow = attrid[y];
            uint8_t *trow = tlattr[y];
            int xs = minx, xe = maxx;
            const bool sp_ok = !use_span || row_span(st, r0, r1, r2, minx, maxx, xs, xe);
            if (big_tri && !big_row(a, b, c, py, minx, maxx, sp_ok, xs, xe))
                continue;
            if (!big_tri && !sp_ok)
                continue;
            for (int x = xs; x <= xe; ++x) {
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
                if (!((n0 >= 0 && n1 >= 0 && n2 >= 0) || (n0 <= 0 && n1 <= 0 && n2 <= 0)) && !edge_rescue(a, b, c, px, py, r0, r1, r2, n0, n1, n2))
                    continue;
                const float w0 = n0 / area, w1 = n1 / area, w2 = n2 / area;
                const float l0 = w1, l1 = w2, l2 = w0;   // barycentric for a, b, c
                const float z = wdepth ? -(l0 * iwa + l1 * iwb + l2 * iwc) : l0 * a.z + l1 * b.z + l2 * c.z;
                if (w_depth_fail(z, drow[x], wq, t.depth_eq != 0)) continue;
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
                    /* THE ONE SAMPLING DECISION IN THE WHOLE RASTER, and it
                       is made by the compiler rather than by this pixel:
                       FILTER is a constant in this instantiation, so the
                       nearest body below is exactly the two tex_coord calls
                       and the one load it always was, with nothing added. */
                    if constexpr (FILTER == 0) {
                        const int ui = tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                        const int vi = tex_coord(vv * tsc, t.th, rep_t, flip_t);
                        texel = t.tex[vi * t.tw + ui];
                    } else {
                        texel = sample_filtered<FILTER>(
                            tt, t.tex, t.tw, t.th, uu * tsc, vv * tsc, rep_s,
                            rep_t, flip_s, flip_t);
                        const int ui = tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                        const int vi = tex_coord(vv * tsc, t.th, rep_t, flip_t);
                        texel = filtered_texel_cover(texel, t.tex[vi * t.tw + ui]);
                    }
                    if ((texel >> 24) == 0) continue;      // transparent texel
                }

                // Round, do not truncate: barycentrics sum to 0.9999 rather than
                // exactly 1, and a truncating cast bands a flat surface 255/254.
                /* MODE 2: the entry this pixel's interpolated red picks. Toon
                   modulates the ENTRY instead of the vertex colour; highlight
                   modulates the vertex red on all three channels and adds the
                   entry after, saturating (GBATEK DS 3D Polygon Attributes;
                   melonDS RenderPixel, the same order). */
                const uint32_t fd = (g_fog.on && t.fog)
                    ? fog_density(wdepth ? -g_fog.wscale / z : z * 16777215.0f) : 0u;
                const float *te = nullptr;
                float tsm[3];
                if (toon) {
                    const float vr = l0 * acol[0] + l1 * bcol[0] + l2 * ccol[0];
                    if (g_toon_smooth) { toon_smooth_entry(vr, tsm); te = tsm; }
                    else te = &g_toon8[toon_index(vr) * 3];
                }
                auto ch = [&](int k, int sh) {
                    const int kv = toon == 2 ? 0 : k;
                    const float v = toon == 1
                        ? te[k]
                        : l0 * acol[kv] + l1 * bcol[kv] + l2 * ccol[kv];
                    const float m = inv255.v[(texel >> sh) & 0xFF];
                    int i = static_cast<int>(v * m + 0.5f);
                    if (toon == 2) i += static_cast<int>(te[k] + 0.5f);
                    i = i < 0 ? 0 : (i > 255 ? 255 : i);
                    if (fd) i = (int)(((uint32_t)i * (128u - fd) + (uint32_t)g_fog.rgb[k] * fd) >> 7);
                    return static_cast<uint32_t>(i);
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
                    if (want_id) irow[x] = t.polyid;
                    /* an opaque write replaces the pixel, so the translucent
                       half of its attribute word goes with it (melonDS stores
                       polyattr & 0x3F008000 on the opaque path, bit 22 clear).
                       Our two passes draw every opaque polygon before any
                       translucent one, so this only ever fires for a
                       translucent-CLASS triangle whose per-pixel alpha came out
                       31: an A3I5 or A5I3 texel at full opacity. */
                    if (have_translucent) trow[x] = 0;
                } else {
                    /* translucent: blend over the framebuffer, keep depth
                       (DS translucent polys depth-test but do not write).
                       The hardware's translucent polygon-ID rule comes first: a
                       pixel that already took a fragment of THIS polygon ID
                       this frame refuses the next one outright. */
                    const uint8_t tl =
                        static_cast<uint8_t>(0x40 | t.polyid);
                    if (trow[x] == tl) continue;
                    trow[x] = tl;
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
    if (clr_pass && clr_done != g_clr_total) clear_stencil();
    }
    };  // band_impl

    /* ONE BAND ENTRY, THREE BODIES BEHIND IT. The switch runs once per band
       per frame -- at most eight times a frame -- and hands the raster a body
       with the filter mode already resolved. The pool below still sees a
       plain two-argument callable, so nothing about the threading changed. */
    auto band = [&](int tile) {
        switch (filt) {
        case 1:
            band_impl(tile, std::integral_constant<int, 1>{});
            break;
        case 2:
            band_impl(tile, std::integral_constant<int, 2>{});
            break;
        default:
            band_impl(tile, std::integral_constant<int, 0>{});
            break;
        }
    };

    /* ---- THE BINS, AND HOW MANY THREADS THE FRAME IS WORTH (run perf2) ----
       One walk of the list on this thread: each triangle's pass and clamped
       rows, by the same expressions and the same early outs the tile body
       uses (a triangle left out here is one the body would have skipped
       anyway), and the frame's work as the sum of the clamped boxes. The
       thread decision is that work, not the triangle count: the title is a
       handful of screen-sized triangles and ran on one thread under the old
       "fewer than 256 triangles" rule, at about 17 ms a frame. Below the
       threshold the handover costs more than the fill, and the frame runs
       as ONE tile the height of the allocation, which is the old
       single-thread walk. */
    if (!gpu_full) {
        int px0, py0, px1, py1;
        raster_clip(px0, py0, px1, py1);
        g_bin_tmp.clear();
        long long work = 0;
        for (size_t i = 0; i < g.tris.size(); ++i) {
            const GxTriangle &t = g.tris[i];
            const int pass = static_cast<int>(t.translucent);
            if (pass != 0 && pass != 1) continue;
            if (only && t.dbg_tex != only) continue;
            const GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
            const float area =
                (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
            if (std::fabs(area) < 1e-6f) continue;
            const bool backface = area > 0.0f;
            if (backface && !(t.cull & 1)) continue;
            if (!backface && !(t.cull & 2)) continue;
            int minx = static_cast<int>(std::floor(std::fmin(a.x, std::fmin(b.x, c.x))));
            int maxx = static_cast<int>(std::ceil(std::fmax(a.x, std::fmax(b.x, c.x))));
            int miny = static_cast<int>(std::floor(std::fmin(a.y, std::fmin(b.y, c.y))));
            int maxy = static_cast<int>(std::ceil(std::fmax(a.y, std::fmax(b.y, c.y))));
            if (minx < px0) minx = px0;
            if (miny < py0) miny = py0;
            if (maxx > px1) maxx = px1;
            if (maxy > py1) maxy = py1;
            if (maxx < minx || maxy < miny) continue;
            work += (long long)(maxx - minx + 1) * (long long)(maxy - miny + 1);
            g_bin_tmp.push_back(BinTri{(int)i, pass, miny, maxy});
        }
        const int pool_n = raster_threads();
        nt = (pool_n > 1 && work >= 16384) ? pool_n : 1;
        tile_h = nt > 1 ? raster_tile_rows() : SCREEN_H;
        ntiles = (SCREEN_H + tile_h - 1) / tile_h;
        for (int ps = 0; ps < 2; ++ps) g_bin_off[ps].assign((size_t)ntiles + 1, 0);
        for (BinTri &bt : g_bin_tmp) {
            bt.k0 /= tile_h;
            bt.k1 /= tile_h;
            for (int k = bt.k0; k <= bt.k1; ++k) ++g_bin_off[bt.pass][(size_t)k + 1];
        }
        for (int ps = 0; ps < 2; ++ps) {
            for (int k = 0; k < ntiles; ++k)
                g_bin_off[ps][(size_t)k + 1] += g_bin_off[ps][(size_t)k];
            g_bin_idx[ps].resize((size_t)g_bin_off[ps][(size_t)ntiles]);
        }
        /* filled in list order, so each tile's list is in submission order */
        static std::vector<int> fill[2];
        for (int ps = 0; ps < 2; ++ps)
            fill[ps].assign(g_bin_off[ps].begin(), g_bin_off[ps].end() - 1);
        for (const BinTri &bt : g_bin_tmp)
            for (int k = bt.k0; k <= bt.k1; ++k)
                g_bin_idx[bt.pass][(size_t)fill[bt.pass][(size_t)k]++] = bt.i;
        /* the stencil clear points, replayed per tile in band_impl */
        g_clr_total = 0;
        if (have_shadow) {
            g_clr_cum.assign(g.tris.size(), 0);
            bool prev_mask = false;
            for (size_t i = 0; i < g.tris.size(); ++i) {
                const GxTriangle &t = g.tris[i];
                if (static_cast<int>(t.translucent) == 1 &&
                    !(only && t.dbg_tex != only)) {
                    const bool is_mask = t.mode == 3 && t.polyid == 0;
                    if (is_mask && !prev_mask) ++g_clr_total;
                    prev_mask = is_mask;
                }
                g_clr_cum[i] = g_clr_total;
            }
        }
    }
    /* Each worker takes the next tile off the counter until none are left. */
    std::atomic<int> next_tile(0);
    auto worker = [&](int, int) {
        for (;;) {
            const int k = next_tile.fetch_add(1);
            if (k >= ntiles) break;
            band(k);
        }
    };
    /* the worker's own type, named out here rather than inside the lambda
       below: decltype of a captured name inside a lambda body is a reference
       type and there is no pointer to a reference */
    typedef decltype(worker) W;
    auto run_passes = [&](int lo, int hi) {
        pass_lo = lo;
        pass_hi = hi;
        if (nt <= 1) {
            for (int k = 0; k < ntiles; ++k) band(k);
        } else {
            next_tile.store(0);
            pool(nt).run([](void *p, int tid, int n) { (*static_cast<W *>(p))(tid, n); },
                         &worker);
        }
    };
    /* THE SAME PASSES UNDER SM64DS_RASTER_AB, see the block above gx_render:
       the frozen reference first, then the live code from the same buffers,
       then the comparison. With the switch off this is run_passes. */
    auto run_checked = [&](int lo, int hi) {
        if (!raster_ab()) {
            run_passes(lo, hi);
            return;
        }
        rab_save(fb, g_rab_start);
        const RefArgs ra{&fb, only, have_shadow, have_translucent, want_id,
                         lo, hi};
        switch (filt) {
        case 1: raster_ref<1>(ra, 0, 1); break;
        case 2: raster_ref<2>(ra, 0, 1); break;
        default: raster_ref<0>(ra, 0, 1); break;
        }
        rab_save(fb, g_rab_ref);
        rab_load(fb, g_rab_start);
        run_passes(lo, hi);
        rab_compare(fb, lo, hi);
    };

    /* ---- THE SEAM: THE OPAQUE PASS, MAYBE ON A GRAPHICS CARD (run hd2) ----
       With nothing registered -- every run with the "Renderer" key absent --
       this is one test against a null pointer and the line below runs both
       passes exactly as it always has. */
    /* THE THREE PARTS OF A FRAME'S RASTER TIME (run hd2, lane GPU2R), taken
        only when SM64DS_FRAME_MS is on. The whole point of moving the opaque
        pass to the card is a number, and one raster total cannot say whether
        what is left is the card, the translucent pass this file still runs,
        or the edge-smoothing pass after it. Three clock reads a frame. */
    std::chrono::steady_clock::time_point t_seam0, t_seam1, t_pass1, t_aa1;
    if (tm) t_seam0 = std::chrono::steady_clock::now();

    if (g_gpu_opaque && !gpu_full) {
        GxGpuFrame f;
        std::memset(&f, 0, sizeof f);
        f.tris = g.tris.data();
        f.count = g.tris.size();
        /* SM64DS_TEX_ONLY limits the card's triangles the way it limits the
           software raster's (see cover_dump) */
        static std::vector<GxTriangle> only_tris;
        if (only) {
            only_tris.clear();
            for (const GxTriangle &ot : g.tris)
                if (ot.dbg_tex == only) only_tris.push_back(ot);
            f.tris = only_tris.data();
            f.count = only_tris.size();
        }
        f.fb = &fb.px[0][0];
        f.depth = &g_depth[0][0];
        f.cover = &g_cover[0][0];
        f.attrid = &g_attrid[0][0];
        f.stride = SCREEN_W;
        f.cw = cw;
        f.ch = ch;
        /* THE SCISSOR IS THE SAME CLIP RECTANGLE THE SOFTWARE RASTER USES: the
           present rectangle narrowed to the game's viewport (raster_clip), so
           the card draws the ending's letterbox the way the CPU path does. A
           frame that never narrowed the viewport gets the present rectangle,
           as before. */
        int cx0, cy0, cx1, cy1;
        raster_clip(cx0, cy0, cx1, cy1);
        f.px0 = cx0;
        f.py0 = cy0;
        f.pw = cx1 - cx0 + 1;
        f.ph = cy1 - cy0 + 1;
        f.want_attrid = want_id ? 1 : 0;
        /* THE DEPTH ONLY HAS TO COME BACK IF SOMETHING IS GOING TO READ IT,
           and at 4x that readback is three megabytes a frame. The translucent
           and shadow pass is the only reader, so a frame with neither skips
           it. The A/B reads it too, and says so. */
        f.want_depth = (have_translucent || have_shadow || have_ghost ||
                        ab_mode()) ? 1 : 0;
        f.wdepth = g_wdepth ? 1 : 0;
        f.tex_filter = filt;
        f.tex_generation = g_tex_generation;
        /* The colour a pixel this pass does not reach keeps. Taken from the
           picture rather than assumed: the caller clears the framebuffer
           before calling, and what it clears to is its business. */
        f.clear_argb = (cw > 0 && ch > 0) ? fb.px[f.py0][f.px0] : 0xFF000000u;
        /* the frame's toon table, for its mode-2 polygons (0 = none) */
        f.toon_shade = g_toon_shade;
        f.toon_rgb = g_toon8;
        f.toon_blend = g_toon_blend;
        f.fog_on = g_fog.on;
        f.fog_wscale = frame_wdepth() ? g_fog.wscale : 0.0f;
        f.fog_shift = g_fog.shift;
        f.fog_offset = g_fog.offset;
        for (int fi = 0; fi < 3; ++fi) f.fog_rgb[fi] = g_fog.rgb[fi];
        for (int fi = 0; fi < 34; ++fi) f.fog_tab[fi] = g_fog.tab[fi];
        /* THE WHOLE-FRAME HAND-OVER (run cardfull). A backend that can draw the
           second pass says so by returning 2; one that cannot returns 1 or 0
           and nothing below changes. The smoothing is offered to it only
           outside an A/B, which compares the picture before it. */
        f.full_ok = ab_mode() ? 1 : 0;
        f.ab = ab_mode();
        f.out = &gpu_out;
        if (!ab_mode()) {
            f.ghost_tris = gl_p;
            f.ghost_count = gl_n;
        }

        if (!ab_mode()) {
            const int r = g_gpu_opaque(&f);
            gpu_drew = r ? 1 : 0;
            gpu_full = (r == 2) ? 1 : 0;
        } else {
            /* THE A/B: the same list drawn both ways, compared before the
               translucent pass, the card's answer kept. See the block above
               gx_render for what each number means. */
            const int x0 = f.px0, y0 = f.py0, w = f.pw, h = f.ph;
            const size_t n = (size_t)SCREEN_W * (size_t)SCREEN_H;
            if (g_ab_save.size() != n) {
                g_ab_save.assign(n, 0);
                g_ab_fb.assign(n, 0);
                g_ab_dep.assign(n, 0.0f);
                g_ab_cov.assign(n, 0);
                g_ab_id.assign(n, 0);
            }
            for (int y = y0; y < y0 + h; ++y)
                std::memcpy(&g_ab_save[(size_t)y * SCREEN_W + x0],
                            &fb.px[y][x0], (size_t)w * sizeof(uint32_t));

            run_checked(0, 0);                      /* arm B: this file */
            for (int y = y0; y < y0 + h; ++y) {
                const size_t o = (size_t)y * SCREEN_W + x0;
                std::memcpy(&g_ab_fb[o], &fb.px[y][x0], (size_t)w * sizeof(uint32_t));
                std::memcpy(&g_ab_dep[o], &g_depth[y][x0], (size_t)w * sizeof(float));
                std::memcpy(&g_ab_cov[o], &g_cover[y][x0], (size_t)w);
                std::memcpy(&g_ab_id[o], &g_attrid[y][x0], (size_t)w);
            }
            /* THE SAME PLANES AFTER THE SECOND PASS, for a card that draws it
               (run cardfull). The software arm simply carries on from where
               the opaque pass left it, which is what the real frame does. */
            if (g_ab2_fb.size() != n) {
                g_ab2_fb.assign(n, 0);
                g_ab2_dep.assign(n, 0.0f);
                g_ab2_cov.assign(n, 0);
                g_ab2_id.assign(n, 0);
            }
            run_checked(1, 1);
            for (int y = y0; y < y0 + h; ++y) {
                const size_t o = (size_t)y * SCREEN_W + x0;
                std::memcpy(&g_ab2_fb[o], &fb.px[y][x0], (size_t)w * sizeof(uint32_t));
                std::memcpy(&g_ab2_dep[o], &g_depth[y][x0], (size_t)w * sizeof(float));
                std::memcpy(&g_ab2_cov[o], &g_cover[y][x0], (size_t)w);
                std::memcpy(&g_ab2_id[o], &g_attrid[y][x0], (size_t)w);
            }
            /* put the buffers back exactly as the clears left them */
            for (int y = y0; y < y0 + h; ++y) {
                std::memcpy(&fb.px[y][x0], &g_ab_save[(size_t)y * SCREEN_W + x0],
                            (size_t)w * sizeof(uint32_t));
                for (int x = x0; x < x0 + w; ++x) g_depth[y][x] = 1e30f;
                std::memset(&g_cover[y][x0], 0, (size_t)w);
                std::memset(&g_attrid[y][x0], 0, (size_t)w);
                if (have_translucent) std::memset(&g_tlattr[y][x0], 0, (size_t)w);
                if (have_shadow) std::memset(&g_stencil[y][x0], 0, (size_t)w);
            }

            {
                const int r = g_gpu_opaque(&f);     /* arm A: the card */
                gpu_drew = r ? 1 : 0;
                gpu_full = (r == 2) ? 1 : 0;
            }
            /* the card drew the whole frame: it is compared against the
               software arm's whole frame, not its opaque half */
            if (gpu_full) {
                g_ab_fb.swap(g_ab2_fb);
                g_ab_dep.swap(g_ab2_dep);
                g_ab_cov.swap(g_ab2_cov);
                g_ab_id.swap(g_ab2_id);
            }

            static int abf;
            const int frame = abf++;
            if (gpu_drew && (frame % g_ab_every) == 0) {
                unsigned long long inter = 0, uni = 0, both = 0, interior = 0;
                unsigned long long out = 0, idbad = 0;
                unsigned long long cov_a = 0, cov_b = 0;
                double colsum = 0.0, depsum = 0.0, depmax = 0.0;
                double depmax_in = 0.0;
                for (int y = y0; y < y0 + h; ++y) {
                    for (int x = x0; x < x0 + w; ++x) {
                        const size_t o = (size_t)y * SCREEN_W + x;
                        const int ca = g_cover[y][x], cb = g_ab_cov[o];
                        /* EACH ARM'S OWN PIXEL COUNT, because an IoU near
                           zero does not say WHICH side drew nothing and that
                           is the first thing anybody reading the row wants. */
                        if (ca) ++cov_a;
                        if (cb) ++cov_b;
                        if (ca || cb) ++uni;
                        if (!(ca && cb)) continue;
                        ++inter;
                        ++both;
                        const uint32_t pa = fb.px[y][x], pb = g_ab_fb[o];
                        int worst = 0;
                        for (int s = 0; s <= 16; s += 8) {
                            const int d = (int)((pa >> s) & 0xFF) -
                                          (int)((pb >> s) & 0xFF);
                            const int ad = d < 0 ? -d : d;
                            colsum += ad;
                            if (ad > worst) worst = ad;
                        }
                        const double dd = (double)g_depth[y][x] - (double)g_ab_dep[o];
                        const double ad = dd < 0 ? -dd : dd;
                        depsum += ad;
                        if (ad > depmax) depmax = ad;
                        /* AN EDGE PIXEL IS ONE WHOSE NEIGHBOURHOOD IS NOT ALL
                           THE SAME SURFACE, in either arm. Those are where the
                           two fill rules legitimately disagree, so they are
                           counted out of the colour and polygon-id verdicts
                           rather than counted against the card. */
                        bool edge = false;
                        for (int dy = -1; dy <= 1 && !edge; ++dy)
                            for (int dx = -1; dx <= 1; ++dx) {
                                const int nx = x + dx, ny = y + dy;
                                if (nx < x0 || ny < y0 || nx >= x0 + w ||
                                    ny >= y0 + h)
                                    continue;
                                const size_t no = (size_t)ny * SCREEN_W + nx;
                                if (g_cover[ny][nx] != g_cover[y][x] ||
                                    g_ab_cov[no] != cb ||
                                    g_attrid[ny][nx] != g_attrid[y][x] ||
                                    g_ab_id[no] != g_ab_id[o]) {
                                    edge = true;
                                    break;
                                }
                            }
                        if (edge) continue;
                        /* THE DEPTH SPAN OF THIS PIXEL'S 3x3 NEIGHBOURHOOD,
                           in each arm, over the neighbours that arm covered.
                           Measured for every interior pixel because the
                           question it answers -- is this a fold inside one
                           polygon id -- is asked of the pixels that fail, and
                           they are not known until they have failed. */
                        double sa_lo = 1e30, sa_hi = -1e30;
                        double sb_lo = 1e30, sb_hi = -1e30;
                        for (int dy = -1; dy <= 1; ++dy)
                            for (int dx = -1; dx <= 1; ++dx) {
                                const int nx = x + dx, ny = y + dy;
                                if (nx < x0 || ny < y0 || nx >= x0 + w ||
                                    ny >= y0 + h)
                                    continue;
                                const size_t no = (size_t)ny * SCREEN_W + nx;
                                if (g_cover[ny][nx]) {
                                    const double v = (double)g_depth[ny][nx];
                                    if (v < sa_lo) sa_lo = v;
                                    if (v > sa_hi) sa_hi = v;
                                }
                                if (g_ab_cov[no]) {
                                    const double v = (double)g_ab_dep[no];
                                    if (v < sb_lo) sb_lo = v;
                                    if (v > sb_hi) sb_hi = v;
                                }
                            }
                        const double span_a = sa_hi > sa_lo ? sa_hi - sa_lo : 0.0;
                        const double span_b = sb_hi > sb_lo ? sb_hi - sb_lo : 0.0;
                        const double span = span_a > span_b ? span_a : span_b;
                        if (ad > g_ab_depth_ceiling) {
                            ++g_ab_dep_over;
                            if (span > 1e-4) ++g_ab_dep_over_span4;
                            if (span > 1e-3) ++g_ab_dep_over_span3;
                            if (span > 1e-2) ++g_ab_dep_over_span2;
                        }
                        /* THE SPLIT, the block above ab_summary's thresholds */
                        if (span <= g_ab_depth_flat) {
                            ++g_ab_dep_flat_n;
                            if (ad > g_ab_dep_flat_max) g_ab_dep_flat_max = ad;
                        } else {
                            ++g_ab_dep_step_n;
                            const double ratio = ad / span;
                            if (ratio > g_ab_depth_ratio) ++g_ab_dep_step_bad;
                            if (ratio > g_ab_dep_ratio_max) {
                                g_ab_dep_ratio_max = ratio;
                                g_ab_rw_x = x;
                                g_ab_rw_y = y;
                                g_ab_rw_frame = frame;
                                g_ab_rw_err = ad;
                                g_ab_rw_span = span;
                            }
                        }
                        ++interior;
                        if (worst > g_ab_tol) ++out;
                        if (ad > depmax_in) {
                            depmax_in = ad;
                            /* and if it beats every frame before it, the
                               whole pixel is kept: the run's worst interior
                               depth disagreement, with both depths and both
                               polygon ids, is what says whether it is a
                               silhouette the id test cannot see or a real
                               mismatch. g_ab_depmax_in is still the previous
                               frames' maximum here, which is what makes the
                               last pixel kept the run's worst one. */
                            if (ad > g_ab_depmax_in) {
                                g_ab_dw_x = x;
                                g_ab_dw_y = y;
                                g_ab_dw_frame = frame;
                                g_ab_dw_card = (double)g_depth[y][x];
                                g_ab_dw_soft = (double)g_ab_dep[o];
                                g_ab_dw_id_card = (int)g_attrid[y][x];
                                g_ab_dw_id_soft = (int)g_ab_id[o];
                                g_ab_dw_span_card = span_a;
                                g_ab_dw_span_soft = span_b;
                            }
                        }
                        if (g_attrid[y][x] != g_ab_id[o]) ++idbad;
                    }
                }
                const double iou = uni ? (double)inter / (double)uni : 1.0;
                const double mean = both ? colsum / (double)(both * 3) : 0.0;
                const double oshare =
                    interior ? (double)out / (double)interior : 0.0;
                std::fprintf(stderr,
                             "[renderer-ab] frame %d cover IoU %.6f card %llu "
                             "software %llu both %llu "
                             "interior %llu colour mean %.4f outliers %llu "
                             "(%.6f) depth interior max %.3e all max %.3e "
                             "mean %.3e id wrong %llu\n",
                             frame, iou, cov_a, cov_b, both, interior, mean,
                             out, oshare, depmax_in, depmax,
                             both ? depsum / (double)both : 0.0, idbad);
                std::fflush(stderr);
                ++g_ab_frames;
                g_ab_cov_card += cov_a;
                g_ab_cov_soft += cov_b;
                g_ab_inter += inter;
                g_ab_union += uni;
                g_ab_both += both;
                g_ab_interior += interior;
                g_ab_outliers += out;
                g_ab_id_bad += idbad;
                g_ab_colsum += colsum;
                g_ab_depsum += depsum;
                if (depmax > g_ab_depmax) g_ab_depmax = depmax;
                if (depmax_in > g_ab_depmax_in) g_ab_depmax_in = depmax_in;
                if (iou < g_ab_worst_iou) {
                    g_ab_worst_iou = iou;
                    g_ab_worst_frame = frame;
                    g_ab_worst_outshare = oshare;
                }
            }
            if (gpu_drew && g_ab_shot_dir && frame == g_ab_shot_frame) {
                char p[512];
                /* the difference picture, so a look at it is a look at where
                   and not at how much: white where the two agree, red where a
                   channel is past the tolerance, blue where only one covered */
                for (int y = y0; y < y0 + h; ++y)
                    for (int x = x0; x < x0 + w; ++x) {
                        const size_t o = (size_t)y * SCREEN_W + x;
                        const int ca = g_cover[y][x], cb = g_ab_cov[o];
                        uint32_t c = 0xFFFFFFFFu;
                        if (ca != cb) c = 0xFF0000FFu;
                        else if (ca) {
                            int worst = 0;
                            for (int s = 0; s <= 16; s += 8) {
                                const int d = (int)((fb.px[y][x] >> s) & 0xFF) -
                                              (int)((g_ab_fb[o] >> s) & 0xFF);
                                const int adv = d < 0 ? -d : d;
                                if (adv > worst) worst = adv;
                            }
                            if (worst > g_ab_tol) c = 0xFFFF0000u;
                        }
                        g_ab_save[o] = c;
                    }
                std::snprintf(p, sizeof p, "%s/ab_f%d_card.bmp", g_ab_shot_dir, frame);
                ab_bmp(p, &fb.px[0][0], SCREEN_W, x0, y0, w, h);
                std::snprintf(p, sizeof p, "%s/ab_f%d_software.bmp", g_ab_shot_dir, frame);
                ab_bmp(p, &g_ab_fb[0], SCREEN_W, x0, y0, w, h);
                std::snprintf(p, sizeof p, "%s/ab_f%d_difference.bmp", g_ab_shot_dir, frame);
                ab_bmp(p, &g_ab_save[0], SCREEN_W, x0, y0, w, h);
                std::fprintf(stderr, "[renderer-ab] frame %d written out as "
                             "three bitmaps\n", frame);
            }
            if (!gpu_drew) {
                /* the card refused this frame after the software arm had
                   already been thrown away, so draw it again here */
                for (int y = y0; y < y0 + h; ++y) {
                    std::memcpy(&fb.px[y][x0], &g_ab_save[(size_t)y * SCREEN_W + x0],
                                (size_t)w * sizeof(uint32_t));
                    for (int x = x0; x < x0 + w; ++x) g_depth[y][x] = 1e30f;
                    std::memset(&g_cover[y][x0], 0, (size_t)w);
                    std::memset(&g_attrid[y][x0], 0, (size_t)w);
                    if (have_translucent) std::memset(&g_tlattr[y][x0], 0, (size_t)w);
                    if (have_shadow) std::memset(&g_stencil[y][x0], 0, (size_t)w);
                }
            }
        }
    }

    if (tm) t_seam1 = std::chrono::steady_clock::now();

    /* the second pass: skipped when the card drew it (run cardfull) */
    if (!gpu_full) run_checked(gpu_drew ? 1 : 0, 1);

    /* the ghost figures, as layers over the finished frame, before the edge
       smoothing reads it (a whole-frame card draws them itself) */
    if (have_ghost && !gpu_full) {
        if (gl_alias) { s_ghost.assign(gl_p, gl_p + gl_n); gl_alias = false; }   /* the processor pass wants a vector */
        ghost_pass_cpu(fb, s_ghost, want_id);
    }

    if (tm) t_pass1 = std::chrono::steady_clock::now();

    /* EDGE SMOOTHING, LAST, AND STILL INSIDE gx_render (run hd2). Here rather
       than at present time because here is the only moment the framebuffer
       holds the 3D picture and nothing else: hal/message_compositor.cpp puts
       engine A's 2D layers over it, hal/sub_screen.cpp drops the bottom-screen
       panel in and walk_window applies the fade, and all three run after this
       function returns. So a pass that runs here cannot read a HUD pixel,
       cannot write one, and needs no list of regions to avoid.
       Inside the timed section deliberately: it is part of what a frame costs
       when the setting is on, and the perf line should say so. */
    {
        static const char *cd = getenv("SM64DS_COVER_DUMP");
        /* the card's frame has no coverage mask on the processor to dump */
        if (cd && *cd && !gpu_full) cover_dump(cd, cw, ch);
    }
    if (gpu_full && gpu_out.aa_done) {
        /* the card smoothed the picture and left aa_pass's own bookkeeping to
           be done here: the frame count, the pixels moved, and whether the
           pre-smoothing copy exists for the display capture */
        ++g_aa_run_frames;
        g_aa_changed += gpu_out.aa_changed;
        g_aa_pre_valid = gpu_out.pre_valid ? 1 : 0;
    } else {
        aa_pass(fb, cw, ch, nt);
    }
    aa_report();
    capture_drill(true);
    if (have_ghost && !gl_alias) g.tris.insert(g.tris.end(), s_ghost.begin(), s_ghost.end());

    if (tm) t_aa1 = std::chrono::steady_clock::now();

    if (tm) {
        using clk = std::chrono::steady_clock;
        using ms = std::chrono::duration<double, std::milli>;
        const clk::time_point t_exit = clk::now();
        static clk::time_point prev;
        static double acc_raster, acc_frame;
        static double acc_seam, acc_pass, acc_aa;
        static long long acc_tris;
        static int n;
        acc_raster += ms(t_exit - t_enter).count();
        acc_seam += ms(t_seam1 - t_seam0).count();
        acc_pass += ms(t_pass1 - t_seam1).count();
        acc_aa += ms(t_aa1 - t_pass1).count();
        if (prev.time_since_epoch().count()) acc_frame += ms(t_exit - prev).count();
        prev = t_exit;
        acc_tris += static_cast<long long>(g.tris.size() + (gl_alias ? gl_n : 0));
        if (++n >= 30) {
            /* The three new numbers are APPENDED, so anything that already
                reads this line by its prefix reads it unchanged. With the card
                off "opaque" is zero and "rest" is the whole software raster;
                with the card on "opaque" is the card's pass including the
                readback and "rest" is the translucent and shadow pass this
                file still runs over the card's buffers. */
            fprintf(stderr, "[perf] frame %6.2fms raster %6.2fms tris %6lld "
                    "decodes %.1f opaque %6.2fms rest %6.2fms aa %6.2fms\n",
                    acc_frame / n, acc_raster / n,
                    acc_tris / n, (double)g_tex_decodes / n,
                    acc_seam / n, acc_pass / n, acc_aa / n);
            acc_raster = acc_frame = 0; acc_tris = 0; n = 0; g_tex_decodes = 0;
            acc_seam = acc_pass = acc_aa = 0;
        }
    }
}

/* ==== MOTION INTERPOLATION: THE RECORDED STREAM, REPLAYED (run interp1) =====
 *
 * WHAT THIS IS FOR. A course ticks 30 times a second (data_0208ee44 = 2
 * vblanks a tick) and the FrameRate key can hand the display 60 to 240
 * pictures a second. Without this block every extra picture is the same
 * finished frame again. With the SmoothMotion key on, an extra picture is
 * drawn from THIS TICK'S OWN GEOMETRY COMMAND STREAM, replayed through exec()
 * with every matrix parameter blended between the previous tick's value and
 * this tick's. No game code runs for it: the ROM submitted the stream once, at
 * its own point in the tick, and what is replayed is the record of it.
 *
 * THE RECORD is everything exec() executed between the gx_reset that opened a
 * frame and that frame's gx_render, with the parameter words, the display
 * list each command was streamed from (runtime.cpp's GXFIFO copy names its
 * source through gx_dma_source), and the side channels a harness or the host
 * frame opener uses (gx_set_light, gx_enable_lights, gx_set_matrix_slot) plus
 * the RESOLVED result of every texture bind, so a replay never decodes VRAM.
 * The engine state right after that gx_reset is kept beside it, so the replay
 * starts where the frame started.
 *
 * THE MATCH. The stream is cut into draw groups: a run of matrix-parameter
 * commands (LOAD, MULT, SCALE, TRANS) and the geometry after it up to the next
 * such command. SM64DS's model path (ModelComponents::Render) makes one group
 * per model part: the part's bone matrices composed with the view on the CPU,
 * loaded and STOREd, then the part's display list DMA'd into the FIFO. A
 * group's key is that display list's source address; geometry fed by hand
 * (no display list) is keyed by a hash of its geometry opcodes. Groups are
 * paired with the previous tick's by key and occurrence, by nearest world
 * position when a key's count changed, and a pair is accepted only when its
 * matrix-command shape is identical and its world position did not jump.
 *
 * THE BLEND. A paired group's matrix parameters are lerp(N-1, N, alpha) in
 * position, position-vector and projection modes; texture-matrix loads are
 * tick N's (a scrolling texture wraps, and a lerp across the wrap would run
 * it backwards). An unpaired group is tick N's, and under a perspective
 * projection its absolute LOAD is re-viewed through the blended camera, so a
 * newly spawned object sits still in the world while the camera glides.
 *
 * WITH THE KEY OFF NOTHING HERE RUNS: g_ip_armed is 0, every hook below is one
 * test of it, and exec() executes exactly the statements it did before.
 */
namespace {

struct IpEv {
    uint8_t kind, cmd, np, mode;
    uint32_t off;        // into par (commands, lights, slots) or binds
    uint32_t src;        // display list being streamed when the command ran
};

struct IpBind {
    const uint32_t *rgba;
    int w, h;
    uint8_t wrap, scale;
    uint32_t id, teximage, plttbase;
};

struct IpGroup {
    uint64_t key;
    uint32_t shape;      // hash of the matrix commands' (cmd, mode) sequence
    int nmtx;            // how many matrix-parameter commands
    int mtx0;            // first index into IpRec::mtx
    int has_pos;
    double px, py, pz;   // world position of its first absolute LOAD
    int has_rel;
    double rx, ry, rz;   // else the translation its first matrix command
                         // carries, in that command's own space: a signature
                         // that tells two instances of one model apart
    int match;           // group index in the previous record, or -1
    uint32_t ev0, ev1;   // event range [ev0, ev1)
    int part;            // a particle (MTX_IDENTITY then one MULT_4x3)
    double ps;           // its billboard scale, in the view matrix's units
    int match2 = -1;     // SMOOTHREND1: the group in the record TWO ticks back, when this one has none a tick back (a blinking model)
    int bind = -1;       // VISUAL093B: index into IpRec::binds of the texture bound when the group's first matrix command ran (-1 none)
};

struct IpRec {
    std::vector<IpEv> ev;
    std::vector<uint32_t> par;
    std::vector<IpBind> binds;
    mutable std::vector<int8_t> bind_sparkle;   // VISUAL093B: per bind, -1 not looked at, 0 not a sparkle texture, 1 it is
    std::vector<IpGroup> groups;
    std::vector<int> mtx;        // event index of every matrix-parameter command
    std::vector<int> ev_group;   // group of every event (-1 before the first)
    std::vector<int> blend;      // per event: previous record's event, -1 none,
                                 // -2 unpaired absolute LOAD (camera-corrected)
    std::vector<int> blend2;     // SMOOTHREND1: per event: the event TWO ticks back (a blinking model), -1 none
    int has_p2 = 0;              // blend2 was built against the record in the free slot
    std::vector<int32_t> tdel;   // TEXTBOX2: per texture-matrix event, indexed like par: this tick's words minus the paired previous tick's (a steady scroll repeats it)
    std::vector<uint8_t> tdel_ok; // per event: tdel holds a real difference (the event was paired and is a texture matrix)
    State start;
    uint32_t start_teximage = 0, start_plttbase = 0;
    uint32_t sealed = 0;         // events that reached the frame's gx_render
    int mw_eval = 0, mw_on = 0;  // MENUINTERP2: the frame's menu-wide answer, as the tick evaluated it
    float mw_zoom = 1.0f, mw_dy = 0.0f;
    float mw_kx = 1.0f, mw_tx = 0.0f, mw_ky = 1.0f, mw_ty = 0.0f;   // TITLE092: the scene transform
    int is_sealed = 0;
    int valid = 0;
    int snap = 0;
    uint32_t tex_gen = 0;
    int32_t view[12] = {};
    /* GHOSTPERF1: THE GHOST FIGURES' TRIANGLES, KEPT WHOLE (see ip_ghost_pairs). The tick's own triangles that belong to a ghost figure,
       in submission order, copied at the seal (before gx_render lifts them out); ev_tri is the length of the frame's triangle list when
       each event was pushed, ev_gt the number of ghost triangles before each event (sealed + 1 entries). gpair: per group, the matching
       group of the previous record when both are a whole ghost figure part of the same size, with the two ranges into gt. */
    std::vector<uint32_t> ev_tri;
    std::vector<GxTriangle> gt;
    std::vector<uint32_t> ev_gt;
    /* GHOSTPERF1: per event, the next event a ghost-fast replay has to look at (itself, or past a run of a paired figure's vertex-class
       commands, which it skips anyway); built on first use after the pairs are made */
    mutable std::vector<uint32_t> nn;
    mutable int nn_valid = 0;
    uint32_t tri_end = 0;
    int gt_bad = 0, gt_ok = 0;
    int gt_div = 0;              // GHOSTPERF1: the figures were made straight into gt (ev_gt filled as the events came)
    struct GPair { int p; uint32_t ca, cb, pa, pb; };
    std::vector<GPair> gpair;
    /* FADEFLASH1: the events [screen_ev0, screen_ev1) were drawn fixed to the screen (the fader wipe's mesh, see
       gx_interp_screen_begin): a picture between ticks blends their matrices straight with tick N-1's range */
    uint32_t screen_ev0 = 0, screen_ev1 = 0;
};

IpRec g_ipr[3];
int g_ip_w = 0, g_ip_c = -1, g_ip_p = -1;   // writing, current (N), previous (N-1)
double g_ip_prev_step = 0, g_ip_prev_turn = 0;   // the camera's previous tick
double g_ip_prev_disp[3] = {0, 0, 0};

/* The replay's own buffers, kept so a picture does not reallocate them. */
std::vector<GxTriangle> g_ip_tris;
std::vector<GxTriangle> g_ip_gh;   // GHOSTPERF1: the figures a picture between ticks is made of (not in the frame list)
int g_ip_gh_div = -1;              // SM64DS_GHOST_DIVERT: 0 keeps them in the frame list

const GxTriangle *ip_diverted_ghosts(size_t &n) {
    n = g_ip_gh.size();
    return g_ip_gh.empty() ? nullptr : g_ip_gh.data();
}
std::vector<GxGrid> g_ip_grids;
std::vector<float> g_ip_grid_pts;
std::vector<GxVertex> g_ip_strip;
std::vector<GxRaw> g_ip_strip_raw;

/* What the replay saves and puts back: the live engine state and the few
   file statics exec() and the texture bind read or write. */
struct IpSaved {
    State g;
    uint32_t teximage, plttbase;
    Mat seen_pos, seen_vec;
    uint32_t mtx_gen;
    int similar;
    Mat ring[MTX_RING];
    uint32_t ring_gen[MTX_RING];
    int ring_sim[MTX_RING];
    int rec;
};
IpSaved g_ip_saved;
int g_ip_sprite_saved = 0;
int g_ip_sprxf_saved_on = 0;       // TITLE092: the live sprite transform flag during a replay
int g_ip_grid_defer_saved = 0;

/* Per group, the triangles one replay produced: [begin, end) into g.tris. The
   probe reads it to measure how a group moved picture to picture. */
std::vector<uint32_t> g_ip_group_tri0, g_ip_group_tri1;

bool ip_is_mtx(uint8_t c) { return c >= 0x16 && c <= 0x1C; }
bool ip_is_geom(uint8_t c) {
    return (c >= 0x20 && c <= 0x2B) || c == 0x40 || c == 0x41;
}

/* The camera's eye and its view axis, out of a DS view matrix (4.12, row
   vectors: view = world * R + t, so the eye is -t R^T). */
void ip_eye(const int32_t v[12], double eye[3], double fwd[3]) {
    double R[3][3], t[3];
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c) R[r][c] = v[r * 3 + c] / 4096.0;
    for (int c = 0; c < 3; ++c) t[c] = v[9 + c] / 4096.0;
    for (int j = 0; j < 3; ++j)
        eye[j] = -(t[0] * R[j][0] + t[1] * R[j][1] + t[2] * R[j][2]);
    /* the world direction that becomes view z: column 2 of R */
    for (int j = 0; j < 3; ++j) fwd[j] = R[j][2];
}

/* World position of an absolute LOAD's origin under view v: (t_M - t_V) R^T. */
void ip_world(const uint32_t *p, int np, const int32_t v[12], double out[3]) {
    const int o = (np == 16) ? 12 : 9;
    double d[3];
    for (int c = 0; c < 3; ++c)
        d[c] = ((int32_t)p[o + c] - v[9 + c]) / 4096.0;
    for (int j = 0; j < 3; ++j)
        out[j] = d[0] * v[j * 3 + 0] / 4096.0 + d[1] * v[j * 3 + 1] / 4096.0 +
                 d[2] * v[j * 3 + 2] / 4096.0;
}

/* THE THRESHOLDS, in the view matrix's own units (fx / 4096). Measured over
   every mounted level, 600 moving frames each (51 x 600 ticks): a camera that
   is following play moved at most 17.9 units and turned at most 12.0 degrees
   in one tick, and the jumps that are cuts were 92.6 units / 22.3 degrees
   and more. So a tick past 40 units or 18 degrees that also jumped more than
   twice as far as the tick before is a cut (gx_interp_commit). A paired object
   whose world position jumps past 64 units in one tick is a teleport (every
   object that was following play moved under that). SM64DS_INTERP_CUT,
   SM64DS_INTERP_TURN and SM64DS_INTERP_TELE override them for a measurement. */
double ip_cut_units() {
    static double v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_INTERP_CUT"); v = e ? atof(e) : 40.0; }
    return v;
}
double ip_cut_degrees() {
    static double v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_INTERP_TURN"); v = e ? atof(e) : 18.0; }
    return v;
}
double ip_tele_units() {
    static double v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_INTERP_TELE"); v = e ? atof(e) : 64.0; }
    return v;
}

/* SM64DS_PARTICLE_PAIR=0 pairs particles the old way (by view-space
   translation), for a measurement. */
bool ip_particle_pair() {
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_PARTICLE_PAIR");
        on = (e && e[0] == '0') ? 0 : 1;
    }
    return on != 0;
}

/* INTERP093 I2: SM64DS_SPARKLE_MODE: 0 the old blend of a paired particle's whole matrix, 1 tick N's matrix for every paired particle (re-viewed
   through the blended camera), 4 (default) tick N's matrix for a twinkling one only */
int ip_sparkle_mode() {
    static int m = -1;
    if (m < 0) { const char *e = getenv("SM64DS_SPARKLE_MODE"); m = e ? atoi(e) : 5; }
    return m;
}

/* INTERP093 I2: a TWINKLE: the particle's size jumped by more than a third between the two ticks (the end-of-game star's glitter, Peach's). It
   is not a puff that grows: tick N's own matrix is kept for it and the picture does not slide or swell it between two ticks */
bool ip_part_twinkle(const IpGroup &gc, const IpGroup &gp) {
    if (gp.ps <= 1e-9) return true;
    const double r = gc.ps / gp.ps;
    return r < 0.77 || r > 1.3;
}

/* VISUAL093B V2: THE SPARKLE TEXTURES. The Grand Star's glitter, Peach's glitter at the ending and the star-get sparkles are particles drawn with the
   same two 16x16 glint textures (content hashes below; the texture ids differ per load, the texels do not). Their size and turn change a lot every
   tick, and they pair with a neighbour in a cloud of a hundred, so a blend of two ticks' matrices slid, swelled and jumped between pictures
   (thunderit, 0.9.3 round 6, at 120 / 144 / 240 fps). A particle with one of these textures keeps tick N's own matrix, re-viewed through the blended
   camera: it twinkles at the DS's tick rate and does not move between two ticks. SM64DS_SPARKLE_MODE=4 puts back the twinkle-only rule, 0 the old blend. */
bool ip_bind_is_sparkle(const IpRec &C, int bi) {
    if (bi < 0 || bi >= (int)C.binds.size()) return false;
    if (C.bind_sparkle.size() != C.binds.size()) C.bind_sparkle.assign(C.binds.size(), (int8_t)-1);
    int8_t &c = C.bind_sparkle[bi];
    if (c < 0) {
        const IpBind &b = C.binds[bi];
        c = 0;
        if (b.rgba && b.w > 0 && b.h > 0 && b.w * b.h <= 1024) {
            uint32_t h = 2166136261u;
            for (int q = 0; q < b.w * b.h; ++q) h = (h ^ b.rgba[q]) * 16777619u;
            /* the glint quarters (16x16, mirrored into the four-point star), the glitter flecks and the light rays (8x1) of the
               Grand Star scene and Peach's ending, measured with SM64DS_PART_LOG / PART_TEXDIR over the whole ending */
            static const uint32_t kSparkle[] = { 0x406c40c5u, 0x3b6c40c5u, 0x216c40c5u, 0xe86c40c5u, 0xe54f21c5u, 0x437d3be5u,
                                                 0x75788b4cu, 0x0048febbu, 0x1d2b1433u, 0xe1f119a8u };
            for (uint32_t k : kSparkle) if (h == k) c = 1;
        }
    }
    return c == 1;
}

bool ip_newborn_recam() {
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_PARTICLE_RECAM");
        on = (e && e[0] == '0') ? 0 : 1;
    }
    return on != 0;
}

void ip_rec_begin() {
    IpRec &r = g_ipr[g_ip_w];
    r.ev.clear();
    r.screen_ev0 = r.screen_ev1 = 0;
    r.par.clear();
    r.binds.clear();
    r.bind_sparkle.clear();
    r.ev_tri.clear();
    r.gt_bad = 0;
    r.gt_ok = 0;
    r.is_sealed = 0;
    r.valid = 0;
    {   /* GHOSTPERF1: the figures of this tick are made straight into the record (SM64DS_GHOST_TICKDIV=0: made in the frame list and copied at the seal) */
        static const int div_on = ((std::getenv("SM64DS_GHOST_TICKDIV") && *std::getenv("SM64DS_GHOST_TICKDIV") == '0') ||
                                   (std::getenv("SM64DS_GHOST_ALIAS") && *std::getenv("SM64DS_GHOST_ALIAS") == '0')) ? 0 : 1;   /* the record's figures are read in place: no alias, no divert */
        r.gt.clear();
        r.ev_gt.clear();
        r.gt_div = (div_on && g_ghost_layer_on && ip_ghost_fast_on()) ? 1 : 0;
        g_ip_tick_div = r.gt_div;
    }
    r.start = g;
    r.start_teximage = g_teximage;
    r.start_plttbase = g_plttbase;
    g_ip_rec = 1;
}

/* GHOSTPERF1: the in-between pictures of a ghost figure.
   A hub ghost is drawn by the ROM's player renderer, a display list of a few hundred triangles that the replay of every in-between picture
   used to run again command by command: about as much work as the tick itself, per ghost, per picture (31 ghosts: 6 ms of an 8.8 ms
   picture). Its triangles are already in the tick's list, in screen space, and both ticks of a pair have the same shape. So a figure part
   that is wholly ghost-layer in both ticks and the same size is not replayed: the picture takes tick N's triangles with each corner
   (x, y, z, w) and colour moved the alpha part of the way from tick N-1's. The matrix commands of the part still run (the stack must be
   what the next part expects); only the commands that make vertices are skipped. Anything that does not qualify replays as it always did.
   SM64DS_GHOST_FAST=0 turns it off. */
int g_gf_override = -1;   // the probe's A/B: 0 replays every group, -1 the setting
bool ip_ghost_fast_on() {
    static int on = -1;
    if (on < 0) { const char *e = std::getenv("SM64DS_GHOST_FAST"); on = (e && e[0] == '0' && !e[1]) ? 0 : 1; }
    return on != 0;
}

void ip_tick_ghost_push(const GxTriangle &t) { g_ipr[g_ip_w].gt.push_back(t); }

void ip_ghost_save(IpRec &r) {
    if (r.gt_div) {
        /* the figures are in gt already; vouch for them, or put them back into the frame list for the ordinary path */
        r.gpair.clear();
        r.gt_ok = 0;
        r.tri_end = static_cast<uint32_t>(g.tris.size());
        const bool ok = !r.gt_bad && g.grids.empty() && r.ev_tri.size() == r.ev.size() && r.ev_gt.size() == r.ev.size() && !r.gt.empty();
        if (ok) { r.ev_gt.push_back(static_cast<uint32_t>(r.gt.size())); r.gt_ok = 1; }
        else {
            g.tris.insert(g.tris.end(), r.gt.begin(), r.gt.end());
            r.gt.clear();
            r.ev_gt.clear();
        }
        g_ip_tick_div = 0;
        return;
    }
    r.gt.clear();
    r.ev_gt.clear();
    r.gpair.clear();
    r.gt_ok = 0;
    r.tri_end = static_cast<uint32_t>(g.tris.size());
    if (!g_ghost_layer_on || !ip_ghost_fast_on() || r.gt_bad || !g.grids.empty()) return;
    if (r.ev_tri.size() != r.ev.size()) return;
    const size_t nt = g.tris.size();
    const size_t ne = r.ev.size();
    r.ev_gt.resize(ne + 1);
    /* one pass over the frame's triangles, the events' triangle counts walked beside them (both only ever rise) */
    uint32_t n = 0;
    size_t e = 0;
    uint32_t last = 0;
    for (size_t i = 0; i < nt; ++i) {
        while (e < ne && r.ev_tri[e] <= i) {
            if (r.ev_tri[e] < last) { r.gt.clear(); r.ev_gt.clear(); return; }   // the list shrank under the record
            last = r.ev_tri[e];
            r.ev_gt[e++] = n;
        }
        if (g.tris[i].ghost) { r.gt.push_back(g.tris[i]); ++n; }
    }
    for (; e < ne; ++e) {
        if (r.ev_tri[e] < last || r.ev_tri[e] > nt) { r.gt.clear(); r.ev_gt.clear(); return; }
        last = r.ev_tri[e];
        r.ev_gt[e] = n;
    }
    if (n == 0) { r.gt.clear(); r.ev_gt.clear(); return; }
    r.ev_gt[ne] = n;
    r.gt_ok = 1;
}

/* the ghost triangles a group made, as [a, b), and whether they are ALL it made */
bool ip_group_ghost_range(const IpRec &r, const IpGroup &G, uint32_t *a, uint32_t *b) {
    if (!r.gt_ok || G.ev1 > r.sealed || G.ev0 >= G.ev1) return false;
    const uint32_t t0 = r.ev_tri[G.ev0];
    const uint32_t t1 = G.ev1 < r.sealed ? r.ev_tri[G.ev1] : r.tri_end;
    *a = r.ev_gt[G.ev0];
    *b = r.ev_gt[G.ev1];
    return *b > *a && (t1 - t0) == (r.gt_div ? 0u : (*b - *a));
}

void ip_ghost_pairs(IpRec &C, const IpRec &P) {
    C.gpair.clear();
    C.nn_valid = 0;
    if (!C.gt_ok || !P.gt_ok) return;
    C.gpair.assign(C.groups.size(), IpRec::GPair{-1, 0, 0, 0, 0});
    for (size_t i = 0; i < C.groups.size(); ++i) {
        const IpGroup &G = C.groups[i];
        if (G.match < 0 || G.match >= (int)P.groups.size()) continue;
        uint32_t ca, cb, pa, pb;
        if (!ip_group_ghost_range(C, G, &ca, &cb)) continue;
        if (!ip_group_ghost_range(P, P.groups[G.match], &pa, &pb)) continue;
        if (cb - ca != pb - pa) continue;
        /* a pair that crossed over (two ghosts swapped places in the match) would streak across the picture: the replay path judges that
           by the matrices; here the first corners must be where the same figure part could have walked in one tick */
        const GxVertex &c0 = C.gt[ca].v[0], &p0 = P.gt[pa].v[0];
        const float dx = c0.x - p0.x, dy = c0.y - p0.y;
        if (dx * dx + dy * dy > 150.0f * 150.0f) continue;
        C.gpair[i] = IpRec::GPair{G.match, ca, cb, pa, pb};
    }
    static int lg = -1, tick = 0;
    if (lg < 0) { const char *e = std::getenv("SM64DS_GHOST_FAST_LOG"); lg = e && *e && *e != '0'; }
    if (lg && (++tick % 120) == 1) {
        int nf = 0, gg = 0; uint32_t tf = 0;
        for (size_t i = 0; i < C.gpair.size(); ++i) {
            if (C.gpair[i].p >= 0) { ++nf; tf += C.gpair[i].cb - C.gpair[i].ca; }
            uint32_t a, b;
            if (ip_group_ghost_range(C, C.groups[i], &a, &b)) ++gg;
        }
        std::fprintf(stderr, "[gfast] groups %zu whole-ghost groups %d fast %d ghost tris %zu fast tris %u\n", C.groups.size(), gg, nf, C.gt.size(), tf);
    }
}

static inline uint32_t ip_lerp_col(uint32_t a, uint32_t b, uint32_t t256) {
    if (a == b) return a;
    const uint32_t u = 256u - t256;
    const uint32_t rb = (((a & 0x00FF00FFu) * u + (b & 0x00FF00FFu) * t256) >> 8) & 0x00FF00FFu;
    const uint32_t ag = ((((a >> 8) & 0x00FF00FFu) * u + ((b >> 8) & 0x00FF00FFu) * t256)) & 0xFF00FF00u;
    return rb | ag;
}

/* tick N's triangles [ca, cb) of C, moved from P's [pa, pb) by alpha, appended to the replay's list */
void ip_ghost_emit(const IpRec &C, const IpRec &P, const IpRec::GPair &q, float alpha) {
    const size_t n = q.cb - q.ca;
    const size_t base = g.tris.size();
    const GxTriangle *c = &C.gt[q.ca];
    const GxTriangle *p = &P.gt[q.pa];
    if (g_ip_gh_div < 0) { const char *e = std::getenv("SM64DS_GHOST_DIVERT"); g_ip_gh_div = (e && *e == '0') ? 0 : 1; }
    std::vector<GxTriangle> &dst = g_ip_gh_div ? g_ip_gh : g.tris;
    const size_t base2 = dst.size();
    dst.insert(dst.end(), c, c + n);   /* copies tick N's triangles whole, no zero fill first */
    GxTriangle *out = &dst[base2];
    (void)base;
    const uint32_t t256 = (uint32_t)(alpha * 256.0f + 0.5f) > 256u ? 256u : (uint32_t)(alpha * 256.0f + 0.5f);
    for (size_t k = 0; k < n; ++k) {
        for (int j = 0; j < 3; ++j) {
            GxVertex &o = out[k].v[j];
            const GxVertex &a = p[k].v[j];
            o.x = a.x + (c[k].v[j].x - a.x) * alpha;
            o.y = a.y + (c[k].v[j].y - a.y) * alpha;
            o.z = a.z + (c[k].v[j].z - a.z) * alpha;
            o.w = a.w + (c[k].v[j].w - a.w) * alpha;
            o.color = ip_lerp_col(a.color, c[k].v[j].color, t256);
        }
    }
}

const GxTriangle *ip_sealed_ghosts(size_t &n) {
    const IpRec &r = g_ipr[g_ip_w];
    n = r.gt.size();
    return r.gt_ok ? r.gt.data() : nullptr;
}

void ip_rec_seal() {
    IpRec &r = g_ipr[g_ip_w];
    r.sealed = static_cast<uint32_t>(r.ev.size());
    ip_ghost_save(r);
    r.mw_eval = g.mw_eval; r.mw_on = g.mw_on; r.mw_zoom = g.mw_zoom; r.mw_dy = g.mw_dy;
    r.mw_kx = g.mw_kx; r.mw_tx = g.mw_tx; r.mw_ky = g.mw_ky; r.mw_ty = g.mw_ty;
    r.is_sealed = 1;
    r.tex_gen = g_tex_generation;
    g_ip_rec = 0;
}

void ip_push(uint8_t kind, uint8_t cmd, int np, const uint32_t *p) {
    IpRec &r = g_ipr[g_ip_w];
    IpEv e;
    e.kind = kind;
    e.cmd = cmd;
    e.np = static_cast<uint8_t>(np);
    e.mode = static_cast<uint8_t>(g.mode & 3);
    e.off = static_cast<uint32_t>(r.par.size());
    e.src = g_ip_dma_src;
    if (np) {
        /* run hunt8, lane GHOSTPERF1: one growth check and a plain copy per command (a generic range insert showed up in the tick) */
        const size_t at = r.par.size();
        if (at + (size_t)np > r.par.capacity()) r.par.reserve(r.par.capacity() < 4096 ? 8192 : r.par.capacity() * 2);
        r.par.resize(at + (size_t)np);
        uint32_t *d = r.par.data() + at;
        for (int k = 0; k < np; ++k) d[k] = p[k];
    }
    r.ev.push_back(e);
    r.ev_tri.push_back(static_cast<uint32_t>(g.tris.size()));
    if (r.gt_div) r.ev_gt.push_back(static_cast<uint32_t>(r.gt.size()));
    if (!g.grids.empty()) r.gt_bad = 1;
}

void ip_rec_bind_now() {
    IpRec &r = g_ipr[g_ip_w];
    IpBind b;
    b.rgba = g.tex_rgba;
    b.w = g.tw;
    b.h = g.th;
    b.wrap = g.tex_wrap;
    b.scale = g.tex_scale;
    b.id = g.tex_id;
    b.teximage = g_teximage;
    b.plttbase = g_plttbase;
    IpEv e;
    e.kind = IPK_BIND;
    e.cmd = 0;
    e.np = 0;
    e.mode = static_cast<uint8_t>(g.mode & 3);
    e.off = static_cast<uint32_t>(r.binds.size());
    e.src = 0;
    r.binds.push_back(b);
    r.ev.push_back(e);
    r.ev_tri.push_back(static_cast<uint32_t>(g.tris.size()));
    if (r.gt_div) r.ev_gt.push_back(static_cast<uint32_t>(r.gt.size()));
}

/* Cut the sealed record into groups and key them. */
void ip_build_groups(IpRec &r) {
    r.groups.clear();
    r.mtx.clear();
    r.ev_group.assign(r.sealed, -1);
    int cur = -1;
    bool geom = false;
    uint32_t ghash = 2166136261u, shape = 2166136261u;
    uint32_t key_src = 0;
    auto close = [&]() {
        if (cur < 0) return;
        IpGroup &G = r.groups[cur];
        /* the matrix shape rides in the key's top bits: two kinds of draw
           that happen to stream the same display list or the same geometry
           opcodes with different matrix setups are different things */
        const uint64_t sh = (uint64_t)((shape ^ (shape >> 16)) & 0xFFFFu) << 44;
        G.key = sh | (key_src ? ((uint64_t)1 << 40) | key_src
                              : ((uint64_t)2 << 40) | ghash);
        G.shape = shape;
    };
    int run_bind = -1;
    for (uint32_t i = 0; i < r.sealed; ++i) {
        const IpEv &e = r.ev[i];
        if (e.kind == IPK_BIND) run_bind = (int)e.off;
        if (e.kind == IPK_CMD && ip_is_mtx(e.cmd) && (cur < 0 || geom)) {
            close();
            IpGroup G;
            G.bind = run_bind;
            G.key = 0; G.shape = 0; G.nmtx = 0;
            G.mtx0 = static_cast<int>(r.mtx.size());
            G.has_pos = 0; G.px = G.py = G.pz = 0;
            G.has_rel = 0; G.rx = G.ry = G.rz = 0;
            G.match = -1;
            G.part = 0; G.ps = 0;
            G.ev0 = i; G.ev1 = i;
            r.groups.push_back(G);
            cur = static_cast<int>(r.groups.size()) - 1;
            geom = false;
            ghash = shape = 2166136261u;
            key_src = 0;
        } else if (cur < 0) {
            continue;   /* frame-head state before the first matrix: tick N's */
        }
        r.ev_group[i] = cur;
        IpGroup &G = r.groups[cur];
        G.ev1 = i + 1;
        if (e.kind != IPK_CMD) continue;
        if (ip_is_mtx(e.cmd)) {
            r.mtx.push_back(static_cast<int>(i));
            ++G.nmtx;
            shape = (shape ^ (uint32_t)(e.cmd | (e.mode << 8))) * 16777619u;
            if (!G.has_pos && (e.cmd == 0x16 || e.cmd == 0x17) &&
                (e.mode == MTX_POS || e.mode == MTX_POSVEC)) {
                double w[3];
                ip_world(&r.par[e.off], e.np, r.view, w);
                G.has_pos = 1;
                G.px = w[0]; G.py = w[1]; G.pz = w[2];
            }
            /* the placement: the first MULT / TRANS translation, which is
               where an instance differs from its siblings when they all
               LOAD the same view first; a LOAD's own when there is none */
            if ((!G.has_rel || (G.has_rel == 2 && e.cmd != 0x16 && e.cmd != 0x17)) &&
                e.mode != MTX_TEX &&
                (e.cmd == 0x16 || e.cmd == 0x17 || e.cmd == 0x18 ||
                 e.cmd == 0x19 || e.cmd == 0x1C)) {
                const int o = (e.cmd == 0x1C) ? 0
                            : (e.np == 16 ? 12 : 9);
                const uint32_t *q = &r.par[e.off];
                G.has_rel = (e.cmd == 0x16 || e.cmd == 0x17) ? 2 : 1;
                G.rx = (int32_t)q[o] / 4096.0;
                G.ry = (int32_t)q[o + 1] / 4096.0;
                G.rz = (int32_t)q[o + 2] / 4096.0;
            }
            /* A PARTICLE: func_0204be40 draws it as MTX_IDENTITY, then one
               MULT_4x3 whose translation is the particle's position already
               run through the view matrix and whose 2x2 is the billboard's
               scale and turn. Its view-space translation moves with the
               camera, so two ticks' translations cannot tell which particle
               is which; its world position and its size can. */
            if (G.nmtx == 1 && e.cmd == 0x19 && e.np == 12 &&
                (e.mode == MTX_POS || e.mode == MTX_POSVEC) && i > 0 &&
                r.ev[i - 1].kind == IPK_CMD && r.ev[i - 1].cmd == 0x15 &&
                r.ev[i - 1].mode == e.mode && ip_particle_pair()) {
                const uint32_t *q = &r.par[e.off];
                double w[3];
                ip_world(q, 12, r.view, w);
                G.part = 1;
                G.has_pos = 1; G.px = w[0]; G.py = w[1]; G.pz = w[2];
                G.has_rel = 0; G.rx = G.ry = G.rz = 0;
                const double a = (int32_t)q[0] / 4096.0, b = (int32_t)q[1] / 4096.0;
                G.ps = std::sqrt(a * a + b * b);
            }
        } else if (ip_is_geom(e.cmd)) {
            if (!geom) key_src = e.src;
            geom = true;
            ghash = (ghash ^ e.cmd) * 16777619u;
        }
    }
    close();
}


/* POLISH093 P3: THE TEXTURE MATRIX BETWEEN TWO TICKS. A texture animation (the ROM's BTA: Goomboss's eyes, scrolling water, lava) rewrites the
   texture matrix once a tick, and the pictures between two ticks used to keep tick N's: the texture ran a tick ahead of the geometry
   and moved at 30 steps a second. When the group is paired and the two ticks' matrices are the same command with the same shape and every
   word moved by less than a limit, the in-between picture takes the blend like any other matrix. A bigger move is a wrap or a jump to another
   cell of an atlas, which a blend would sweep through, so that one stays tick N's. SM64DS_INTERP_TEXMTX=0 puts the old rule back;
   SM64DS_INTERP_TEXMTX_MAX=<n> sets the limit (4.12 fixed units per word, default 0x6000 = 6.0: a slow scroll, not a hop between cells); SM64DS_INTERP_TEXLOG=1 prints what each
   pair did, once. */
int ip_texmtx_mode() {
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_INTERP_TEXMTX"); v = (e && e[0] == '0') ? 0 : 1; }
    return v;
}
bool ip_tex_pair_ok(const IpRec &P, int b, const IpEv &ce, const uint32_t *cp, uint32_t *adj, bool &wrapped) {
    wrapped = false;
    const IpEv &pe = P.ev[b];
    const std::vector<uint32_t> &ppar = P.par;
    if (!ip_texmtx_mode()) return false;
    if (pe.kind != IPK_CMD || pe.cmd != ce.cmd || pe.np != ce.np) return false;
    static long lim = -1;
    static int log = -1, steady_on = -1;
    if (lim < 0) {
        const char *e = getenv("SM64DS_INTERP_TEXMTX_MAX");
        lim = e ? strtol(e, 0, 0) : 0x6000;
        log = getenv("SM64DS_INTERP_TEXLOG") ? 1 : 0;
        const char *st = getenv("SM64DS_INTERP_TEXSTEADY");
        steady_on = (st && st[0] == '0') ? 0 : 1;
    }
    long worst = 0;
    for (int k = 0; k < ce.np; ++k) {
        const long d = (long)(int32_t)cp[k] - (long)(int32_t)ppar[pe.off + k];
        const long a = d < 0 ? -d : d;
        if (a > worst) worst = a;
    }
    bool ok = worst <= lim;
    /* TEXTBOX2: a scroll that moves a long way every tick (sand: 51 units, over the limit above) is still a scroll when this tick's step is the
       step the last tick took (the ROM adds a constant, give or take its own rounding): that is blended like a slow one. The step across a
       texture wrap is the same step taken modulo 512 units (every wrap seen is a whole number of those): the picture blends to the matrix
       with the wrap taken out, so the wrap tick does not snap. A hop that is not the last step (an atlas cell), the first step out of rest
       and any step that is not about the last one's stay tick N's, as before. A step that wobbles around the limit (a swirl: 5.75, 6.06,
       6.6 ...) no longer flips between a blend and a snap tick by tick. SM64DS_INTERP_TEXSTEADY=0 puts the plain limit back. */
    bool steady = false;
    if (!ok && steady_on && b < (int)P.tdel_ok.size() && P.tdel_ok[b] && (size_t)pe.off + pe.np <= P.tdel.size()) {
        steady = true;
        long pw = 0;
        const long W = 0x200000;
        for (int k = 0; k < ce.np && steady; ++k) {
            const long dn = (long)(int32_t)cp[k] - (long)(int32_t)ppar[pe.off + k];
            const long dp = (long)P.tdel[pe.off + k];
            const long a = dp < 0 ? -dp : dp;
            if (a > pw) pw = a;
            long diff = dn - dp;
            long m = diff >= 0 ? (diff + W / 2) / W : -((-diff + W / 2) / W);
            diff -= m * W;
            if (diff < 0) diff = -diff;
            const long tol = a / 4 > 0x800 ? a / 4 : 0x800;   /* a swirl's step drifts about a tenth tick to tick; a hop is a different step by far */
            if (diff > tol) steady = false;
            else if (m) wrapped = true;
            adj[k] = (uint32_t)((long)(int32_t)cp[k] - m * W);
        }
        if (pw <= 0x1000) steady = false;      /* the last tick did not move: not a scroll yet */
        ok = steady;
        if (!ok) wrapped = false;
    }
    if (log && worst) {
        static int n;
        if (n++ < 3000) {
            fprintf(stderr, "  [texmtx] cmd %02x np %d worst %.3f%s%s%s |", ce.cmd, ce.np, worst / 4096.0, steady ? " STEADY" : "", wrapped ? " WRAP" : "", !ok ? " HELD" : "");
            for (int k = 0; k < ce.np; ++k)
                fprintf(stderr, " %.2f>%.2f", (int32_t)ppar[pe.off + k] / 4096.0, (int32_t)cp[k] / 4096.0);
            fprintf(stderr, "%c", 10);
        }
    }
    return ok;
}

double ip_dist2(const IpGroup &a, const IpGroup &b) {
    const double dx = a.px - b.px, dy = a.py - b.py, dz = a.pz - b.pz;
    return dx * dx + dy * dy + dz * dz;
}

double ip_rel2(const IpGroup &a, const IpGroup &b) {
    const double dx = a.rx - b.rx, dy = a.ry - b.ry, dz = a.rz - b.rz;
    return dx * dx + dy * dy + dz * dz;
}

/* How unlike two instances of one key are: their world positions when both
   have one, else the translations their first matrix commands carry, else how
   far apart they sit in the key's draw order. */
double ip_unlike(const IpGroup &a, const IpGroup &b, int ia, int ib) {
    /* two particles: how far the world position moved plus how much the
       size changed (a puff does not turn into a speck between two ticks) */
    if (a.part && b.part) {
        const double ds = (a.ps - b.ps) * 3.0;
        return ip_dist2(a, b) + ds * ds;
    }
    const bool p = a.has_pos && b.has_pos, r = a.has_rel && b.has_rel;
    if (p || r) return (p ? ip_dist2(a, b) : 0.0) + (r ? ip_rel2(a, b) : 0.0);
    const double d = (double)(ia - ib);
    return 1e12 + d * d;
}

struct IpCand { double d; int i, j; };
bool ip_cand_less(const IpCand &x, const IpCand &y) {
    if (x.d != y.d) return x.d < y.d;
    if (x.i != y.i) return x.i < y.i;
    return x.j < y.j;
}

/* Pair C's groups with P's, then fill C.blend. Returns counts through st. */
bool blink_pair_on() {
    static int on = -1;
    if (on < 0) { const char *e = std::getenv("SM64DS_BLINK_PAIR"); on = (e && e[0] == '0') ? 0 : 1; }
    return on != 0;
}

int g_ip_tick_no = 0;   // VISUAL093B: ticks matched so far (the diag logs)
void ip_match(IpRec &C, const IpRec &P, GxInterpStats *st, const IpRec *P2 = nullptr) {
    ++g_ip_tick_no;
    std::map<uint64_t, std::vector<int>> kc, kp;
    for (size_t i = 0; i < C.groups.size(); ++i) kc[C.groups[i].key].push_back((int)i);
    for (size_t i = 0; i < P.groups.size(); ++i) kp[P.groups[i].key].push_back((int)i);
    const double tele2 = ip_tele_units() * ip_tele_units();
    int matched = 0, tele = 0, shapebad = 0;
    double max_moved = 0;
    for (auto &kv : kc) {
        auto it = kp.find(kv.first);
        if (it == kp.end()) continue;
        const std::vector<int> &lc = kv.second, &lp = it->second;
        std::vector<int> pair(lc.size(), -1);
        /* ONE INSTANCE EACH SIDE pairs directly. SEVERAL (the same model part
           drawn for many objects: coins, goombas, a particle kind) pair by
           likeness, closest first, whatever order they were drawn in: the
           actor lists re-sort and a spawn or a death shifts every later
           instance, so draw order alone would cross-pair them. A key with a
           very long list on both sides falls back to draw order, which is
           right whenever nothing was born or died. */
        if (lc.size() == 1 && lp.size() == 1) {
            pair[0] = lp[0];
        } else if (lc.size() * lp.size() > 20000) {
            for (size_t i = 0; i < lc.size() && i < lp.size(); ++i) pair[i] = lp[i];
        } else {
            std::vector<IpCand> cand;
            cand.reserve(lc.size() * lp.size());
            for (size_t i = 0; i < lc.size(); ++i)
                for (size_t j = 0; j < lp.size(); ++j) {
                    IpCand c;
                    c.d = ip_unlike(C.groups[lc[i]], P.groups[lp[j]], (int)i, (int)j);
                    c.i = (int)i;
                    c.j = (int)j;
                    cand.push_back(c);
                }
            std::sort(cand.begin(), cand.end(), ip_cand_less);
            std::vector<char> usedc(lc.size(), 0), usedp(lp.size(), 0);
            for (const IpCand &c : cand) {
                if (usedc[c.i] || usedp[c.j]) continue;
                usedc[c.i] = usedp[c.j] = 1;
                pair[c.i] = lp[c.j];
            }
        }
        for (size_t i = 0; i < lc.size(); ++i) {
            if (pair[i] < 0) continue;
            IpGroup &gc = C.groups[lc[i]];
            const IpGroup &gp = P.groups[pair[i]];
            if (gc.nmtx != gp.nmtx || gc.shape != gp.shape) { ++shapebad; continue; }
            /* what is left over when one particle died and another was born
               is a big old one and a tiny new one: not a pair */
            if (gc.part && gp.part) {
                const double hi = gc.ps > gp.ps ? gc.ps : gp.ps;
                const double lo = gc.ps > gp.ps ? gp.ps : gc.ps;
                if (hi > lo * 6.0) { ++shapebad; continue; }
            }
            if (gc.has_pos && gp.has_pos && ip_dist2(gc, gp) > tele2) { ++tele; continue; }
            if (!(gc.has_pos && gp.has_pos) && gc.has_rel && gp.has_rel &&
                ip_rel2(gc, gp) > 16.0 * tele2) { ++tele; continue; }
            gc.match = pair[i];
            ++matched;
            if (gc.has_pos && gp.has_pos) {
                const double d = std::sqrt(ip_dist2(gc, gp));
                if (d > max_moved) max_moved = d;
            }
        }
    }
    /* SMOOTHREND1 (D18): A MODEL THAT BLINKS. Metal Wario's closing flicker swaps the whole player model every tick, so
       a group of tick N has nothing like it in tick N-1 (the other model) and a twin in tick N-2 (the same model, one
       blink earlier). Without a partner it would sit at its tick-N place in every picture and the runner would hop at
       30 a second. Pair it with the N-2 twin, and its pictures blend N-2 -> N over two ticks (see gx_interp_begin).
       Only a group the N-1 record has no key for qualifies: a pair that failed on shape or distance stays unpaired. */
    C.has_p2 = 0;
    if (P2 && blink_pair_on()) {
        std::map<uint64_t, std::vector<int>> k2;
        for (size_t i = 0; i < P2->groups.size(); ++i) k2[P2->groups[i].key].push_back((int)i);
        std::vector<char> used2(P2->groups.size(), 0);
        for (size_t gi = 0; gi < C.groups.size(); ++gi) {
            IpGroup &gc = C.groups[gi];
            gc.match2 = -1;
            if (gc.match >= 0 || gc.part || gc.nmtx <= 0 || !gc.has_pos) continue;
            if (kp.find(gc.key) != kp.end()) continue;           // the previous tick had that model: not a blink
            auto it = k2.find(gc.key);
            if (it == k2.end()) continue;
            int best = -1;
            double bd = 1e300;
            for (int j : it->second) {
                const IpGroup &gq = P2->groups[j];
                if (used2[j] || gq.nmtx != gc.nmtx || gq.shape != gc.shape || !gq.has_pos) continue;
                const double d = ip_dist2(gc, gq);
                if (d < bd) { bd = d; best = j; }
            }
            if (best < 0 || bd > 4.0 * tele2) continue;
            used2[best] = 1;
            gc.match2 = best;
            C.has_p2 = 1;
        }
    }
    C.blend.assign(C.sealed, -1);
    C.blend2.assign(C.has_p2 ? C.sealed : 0, -1);
    for (size_t gi = 0; gi < C.groups.size(); ++gi) {
        const IpGroup &gc = C.groups[gi];
        if (gc.match >= 0) {
            const IpGroup &gp = P.groups[gc.match];
            for (int k = 0; k < gc.nmtx; ++k)
                C.blend[C.mtx[gc.mtx0 + k]] = P.mtx[gp.mtx0 + k];
        } else if (gc.match2 >= 0) {
            const IpGroup &gq = P2->groups[gc.match2];
            for (int k = 0; k < gc.nmtx; ++k)
                C.blend2[C.mtx[gc.mtx0 + k]] = P2->mtx[gq.mtx0 + k];
        } else {
            for (int k = 0; k < gc.nmtx; ++k) {
                const int ei = C.mtx[gc.mtx0 + k];
                const IpEv &e = C.ev[ei];
                if ((e.cmd == 0x16 || e.cmd == 0x17) &&
                    (e.mode == MTX_POS || e.mode == MTX_POSVEC))
                    C.blend[ei] = -2;
            }
            /* a newborn particle: its view-space point is tick N's, so it
               would ride the screen while the world glides under the blended
               camera; -3 re-views the point */
            if (gc.part && gc.nmtx == 1 && ip_newborn_recam()) {
                const int ei = C.mtx[gc.mtx0];
                if (C.ev[ei].np == 12) C.blend[ei] = -3;
            }
        }
    }
    /* VISUAL093B: SM64DS_PART_LOG=1 prints every particle group of the tick: its pairing, size, move and bound texture (for the sparkle test) */
    {
        static const int plog = std::getenv("SM64DS_PART_LOG") ? 1 : 0;
        static int tk = 0;
        ++tk;
        if (plog) {
            for (size_t gi = 0; gi < C.groups.size(); ++gi) {
                const IpGroup &gc = C.groups[gi];
                if (!gc.part) continue;
                const IpBind *bd = (gc.bind >= 0 && gc.bind < (int)C.binds.size()) ? &C.binds[gc.bind] : nullptr;
                double pps = 0, mv = 0;
                if (gc.match >= 0) { const IpGroup &gp = P.groups[gc.match]; pps = gp.ps; mv = std::sqrt(ip_dist2(gc, gp)); }
                uint32_t th = 2166136261u;
                if (bd && bd->rgba) for (int q = 0; q < bd->w * bd->h; ++q) th = (th ^ bd->rgba[q]) * 16777619u;
                {
                    static const char *tdir = std::getenv("SM64DS_PART_TEXDIR");
                    static std::set<uint32_t> seen;
                    if (tdir && bd && bd->rgba && seen.insert(th).second) {
                        char fn[600]; std::snprintf(fn, sizeof fn, "%s/tex_%08x_%dx%d.ppm", tdir, th, bd->w, bd->h);
                        if (FILE *pf = std::fopen(fn, "wb")) {
                            std::fprintf(pf, "P6 %d %d 255 ", bd->w, bd->h);
                            for (int q = 0; q < bd->w * bd->h; ++q) { const uint32_t c = bd->rgba[q]; const unsigned char px[3] = { (unsigned char)(c >> 24), (unsigned char)(c >> 24), (unsigned char)(c >> 24) }; std::fwrite(px, 1, 3, pf); }
                            std::fclose(pf);
                        }
                    }
                }
                std::fprintf(stderr, "[part] tk %d g %d m %d ps %.4f pps %.4f mv %.4f tex %08x/%08x %dx%d id %u hash %08x\n", tk, (int)gi, gc.match, gc.ps, pps, mv,
                             bd ? bd->teximage : 0u, bd ? bd->plttbase : 0u, bd ? bd->w : 0, bd ? bd->h : 0, bd ? bd->id : 0u, th);
            }
        }
    }
    /* TEXTBOX2: the texture matrices' tick-to-tick difference, kept for the next tick's pairing (see ip_tex_pair_ok) */
    C.tdel.assign(C.par.size(), 0);
    C.tdel_ok.assign(C.sealed, 0);
    for (uint32_t i = 0; i < C.sealed; ++i) {
        const int b = C.blend[i];
        if (b < 0 || C.ev[i].mode != MTX_TEX || !ip_is_mtx(C.ev[i].cmd)) continue;
        const IpEv &ce = C.ev[i];
        const IpEv &pe = P.ev[b];
        if (pe.kind != IPK_CMD || pe.cmd != ce.cmd || pe.np != ce.np || ce.np > 32) continue;
        for (int k = 0; k < ce.np; ++k)
        {
            /* the step, taken modulo the texture wrap (512 units, 0x200000 raw: every wrap seen is a whole number of these) so a step across a
               wrap is the step before it */
            long d = (long)(int32_t)(C.par[ce.off + k] - P.par[pe.off + k]);
            const long W = 0x200000;
            d -= W * (long)std::floor((double)(d + W / 2) / (double)W);
            C.tdel[ce.off + k] = (int32_t)d;
        }
        C.tdel_ok[i] = 1;
    }
    if (st) {
        st->groups = (int)C.groups.size();
        st->matched = matched;
        st->unmatched = (int)C.groups.size() - matched;
        st->teleport = tele;
        st->shape = shapebad;
        st->max_moved = (float)max_moved;
    }
}

int32_t ip_lerp(uint32_t a, uint32_t b, double t) {
    const double v = (double)(int32_t)a * (1.0 - t) + (double)(int32_t)b * t;
    return (int32_t)(v < 0 ? v - 0.5 : v + 0.5);
}

/* INTERPTURN1: a matrix that turns a lot in one tick. The words of two
   rotations blended one by one are not a rotation: halfway between facing +x
   and facing -x the 3x3 part is close to zero along the turn, so a player who
   turned round in one tick was drawn as a sheet seen edge on (a thin dark
   line: the normals shrink with it) in the pictures between the two ticks.
   Such a pair is split into a turn and a stretch (M = S U, S symmetric, U a
   rotation; the DS matrices carry scale and squash in S), U is blended along
   the shortest arc, S and the translation word by word, and the product is
   rebuilt. A pair whose three rows each turned less than 2 degrees in the
   tick keeps the word blend (what every picture used before): its shrink is
   1 - cos(1 degree) at most, under a tenth of a pixel at the edge of a
   1364-pixel picture, and a still scene stays bit for bit what it was.
   SM64DS_IP_ROTBLEND=0 is the old blend everywhere, for a measurement;
   SM64DS_IP_ROTBLEND_DEG moves the threshold; SM64DS_IP_ROTBLEND_LOG=1
   prints, a picture at a time, how many matrices took each path. */
int ip_rot_mode() {
    static int m = -1;
    if (m < 0) { const char *e = getenv("SM64DS_IP_ROTBLEND"); m = (e && e[0] == '0') ? 0 : 1; }
    return m;
}
double ip_rot_cos() {
    static double c = 2.0;
    if (c > 1.5) {
        const char *e = getenv("SM64DS_IP_ROTBLEND_DEG");
        const double d = e ? atof(e) : 2.0;
        c = std::cos(d * 3.14159265358979323846 / 180.0);
    }
    return c;
}
struct IpRotStat { uint32_t mats, rot, back, hist[8]; };
IpRotStat g_ip_rot_stat;
double g_ip_up[3];   // the world's up as the camera of the picture sees it (row 1 of tick N's view)
int g_ip_up_ok = 0;
void ip_set_up(const int32_t *view) {
    const double x = view[3], y = view[4], z = view[5];
    const double l = std::sqrt(x * x + y * y + z * z);
    g_ip_up_ok = l > 1.0;
    if (g_ip_up_ok) { g_ip_up[0] = x / l; g_ip_up[1] = y / l; g_ip_up[2] = z / l; }
}
float g_ip_rot_alpha = 0;
double g_ip_ref[4];   // the turn of the last matrix of this picture that turned 90 degrees or more (the way it went)
int g_ip_ref_ok = 0;
int ip_rot_log() {
    static int on = -1;
    if (on < 0) { const char *e = getenv("SM64DS_IP_ROTBLEND_LOG"); on = (e && *e && *e != '0') ? atoi(e) : 0; }
    return on;
}

/* U = the rotation part of M (M = S U), by Higham's iteration U <- (U + U^-T) / 2.
   False if M is too close to flat to have one. */
bool ip_polar(const double M[3][3], double U[3][3]) {
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k) U[r][k] = M[r][k];
    for (int it = 0; it < 30; ++it) {
        double C[3][3];
        C[0][0] = U[1][1] * U[2][2] - U[1][2] * U[2][1];
        C[0][1] = U[1][2] * U[2][0] - U[1][0] * U[2][2];
        C[0][2] = U[1][0] * U[2][1] - U[1][1] * U[2][0];
        C[1][0] = U[0][2] * U[2][1] - U[0][1] * U[2][2];
        C[1][1] = U[0][0] * U[2][2] - U[0][2] * U[2][0];
        C[1][2] = U[0][1] * U[2][0] - U[0][0] * U[2][1];
        C[2][0] = U[0][1] * U[1][2] - U[0][2] * U[1][1];
        C[2][1] = U[0][2] * U[1][0] - U[0][0] * U[1][2];
        C[2][2] = U[0][0] * U[1][1] - U[0][1] * U[1][0];
        const double det = U[0][0] * C[0][0] + U[0][1] * C[0][1] + U[0][2] * C[0][2];
        if (std::fabs(det) < 1e-9) return false;
        double dmax = 0;
        for (int r = 0; r < 3; ++r)
            for (int k = 0; k < 3; ++k) {
                const double n = 0.5 * (U[r][k] + C[r][k] / det);
                const double dd = std::fabs(n - U[r][k]);
                if (dd > dmax) dmax = dd;
                U[r][k] = n;
            }
        if (dmax < 1e-12) break;
    }
    return true;
}

void ip_mat_quat(const double R[3][3], double q[4]) {
    const double tr = R[0][0] + R[1][1] + R[2][2];
    if (tr > 0) {
        const double s = std::sqrt(tr + 1.0) * 2.0;
        q[0] = 0.25 * s;
        q[1] = (R[2][1] - R[1][2]) / s;
        q[2] = (R[0][2] - R[2][0]) / s;
        q[3] = (R[1][0] - R[0][1]) / s;
    } else if (R[0][0] > R[1][1] && R[0][0] > R[2][2]) {
        const double s = std::sqrt(1.0 + R[0][0] - R[1][1] - R[2][2]) * 2.0;
        q[0] = (R[2][1] - R[1][2]) / s;
        q[1] = 0.25 * s;
        q[2] = (R[0][1] + R[1][0]) / s;
        q[3] = (R[0][2] + R[2][0]) / s;
    } else if (R[1][1] > R[2][2]) {
        const double s = std::sqrt(1.0 + R[1][1] - R[0][0] - R[2][2]) * 2.0;
        q[0] = (R[0][2] - R[2][0]) / s;
        q[1] = (R[0][1] + R[1][0]) / s;
        q[2] = 0.25 * s;
        q[3] = (R[1][2] + R[2][1]) / s;
    } else {
        const double s = std::sqrt(1.0 + R[2][2] - R[0][0] - R[1][1]) * 2.0;
        q[0] = (R[1][0] - R[0][1]) / s;
        q[1] = (R[0][2] + R[2][0]) / s;
        q[2] = (R[1][2] + R[2][1]) / s;
        q[3] = 0.25 * s;
    }
}

void ip_quat_mat(const double q[4], double R[3][3]) {
    const double w = q[0], x = q[1], y = q[2], z = q[3];
    R[0][0] = 1 - 2 * (y * y + z * z); R[0][1] = 2 * (x * y - z * w);     R[0][2] = 2 * (x * z + y * w);
    R[1][0] = 2 * (x * y + z * w);     R[1][1] = 1 - 2 * (x * x + z * z); R[1][2] = 2 * (y * z - x * w);
    R[2][0] = 2 * (x * z - y * w);     R[2][1] = 2 * (y * z + x * w);     R[2][2] = 1 - 2 * (x * x + y * y);
}

/* a (tick N-1) and b (tick N) are the np words of one matrix command (9: 3x3,
   12: 4x3, 16: 4x4). True when the pair turned past the threshold and out /
   ex hold the turn-aware blend at t (ex unrounded, in word units); false
   leaves the caller to blend word by word, exactly as before. */
bool ip_rot_blend(const uint32_t *a, const uint32_t *b, int np, double t, uint32_t *out, double *ex, bool chain) {
    if (!ip_rot_mode() || (np != 9 && np != 12 && np != 16)) return false;
    const int stride = np == 16 ? 4 : 3;
    double A[3][3], B[3][3];
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k) {
            A[r][k] = (int32_t)a[r * stride + k] / 4096.0;
            B[r][k] = (int32_t)b[r * stride + k] / 4096.0;
        }
    IpRotStat &st = g_ip_rot_stat;
    ++st.mats;
    /* the cheap test: how far each row (the image of one axis) turned */
    double cmin = 1.0;
    for (int r = 0; r < 3; ++r) {
        const double la = A[r][0] * A[r][0] + A[r][1] * A[r][1] + A[r][2] * A[r][2];
        const double lb = B[r][0] * B[r][0] + B[r][1] * B[r][1] + B[r][2] * B[r][2];
        if (la < 1e-8 || lb < 1e-8) return false;   // a part scaled to nothing (hidden): the word blend is right for it
        const double c = (A[r][0] * B[r][0] + A[r][1] * B[r][1] + A[r][2] * B[r][2]) / std::sqrt(la * lb);
        if (c < cmin) cmin = c;
    }
    if (ip_rot_log()) {
        const double deg = std::acos(cmin < -1 ? -1 : cmin) * (180.0 / 3.14159265358979323846);
        static const double edge[7] = {1, 2, 5, 10, 20, 45, 90};
        int h = 0;
        while (h < 7 && deg >= edge[h]) ++h;
        ++st.hist[h];
    }
    if (cmin >= ip_rot_cos()) return false;
    if (ip_rot_log() >= 2)
        std::fprintf(stderr, "[iprot2] np %d deg %.1f a %d %d %d / %d %d %d / %d %d %d t %d %d %d\n", np,
                     std::acos(cmin < -1 ? -1 : cmin) * (180.0 / 3.14159265358979323846), (int32_t)a[0], (int32_t)a[1], (int32_t)a[2],
                     (int32_t)a[stride], (int32_t)a[stride + 1], (int32_t)a[stride + 2], (int32_t)a[2 * stride], (int32_t)a[2 * stride + 1],
                     (int32_t)a[2 * stride + 2], np == 9 ? 0 : (int32_t)a[3 * stride], np == 9 ? 0 : (int32_t)a[3 * stride + 1],
                     np == 9 ? 0 : (int32_t)a[3 * stride + 2]);
    double Ua[3][3], Ub[3][3];
    if (!ip_polar(A, Ua) || !ip_polar(B, Ub)) { ++st.back; return false; }
    auto det3 = [](const double m[3][3]) {
        return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
               m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    };
    const double da = det3(Ua), db = det3(Ub);
    if ((da < 0) != (db < 0)) { ++st.back; return false; }   // a mirror on one side only: no turn joins them
    const double sg = da < 0 ? -1.0 : 1.0;   // a mirrored pair: blend -U (a rotation); S = M U^T takes the sign, so S U is still M
    double Sa[3][3], Sb[3][3];
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k) {
            Ua[r][k] *= sg;
            Ub[r][k] *= sg;
        }
    /* S = M U^T */
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k) {
            Sa[r][k] = A[r][0] * Ua[k][0] + A[r][1] * Ua[k][1] + A[r][2] * Ua[k][2];
            Sb[r][k] = B[r][0] * Ub[k][0] + B[r][1] * Ub[k][1] + B[r][2] * Ub[k][2];
        }
    double qa[4], qb[4], q[4];
    ip_mat_quat(Ua, qa);
    ip_mat_quat(Ub, qb);
    double d = qa[0] * qb[0] + qa[1] * qb[1] + qa[2] * qb[2] + qa[3] * qb[3];
    /* the way round. Taking the shorter way for each matrix on its own would
       split a body whose pieces (one matrix a bone) turned close to half a
       circle: a piece at 179 degrees and one at 181 would go opposite ways, and
       so would any two pieces on either side of ANY fixed cut (the reviewer's
       148 / 152 at a 150-degree band edge). No rule on one matrix alone can
       avoid a cut somewhere, so the cut is moved to where one body never
       straddles it: the last matrix of the picture that turned 90 degrees or
       more is the reference, and a matrix whose turn is within 60 degrees of
       it (quaternion dot 0.866: the pieces of one body differ by their own
       animation, far less in a tick) goes the same way as it. A matrix with no
       such reference takes the shorter way, and a turn of 170 to 190 degrees
       a fixed way about the camera's up, so a body turning right round goes
       one way, whole. The camera is never a reference (chain false). */
    /* qd = conj(qa) qb, the turn of the tick (Ub = Ua D, D = Ua^T Ub); its w is d */
    double qd[4] = {d, qa[0] * qb[1] - qb[0] * qa[1] - (qa[2] * qb[3] - qa[3] * qb[2]),
                    qa[0] * qb[2] - qb[0] * qa[2] - (qa[3] * qb[1] - qa[1] * qb[3]),
                    qa[0] * qb[3] - qb[0] * qa[3] - (qa[1] * qb[2] - qa[2] * qb[1])};
    bool flip;
    const double rd = qd[0] * g_ip_ref[0] + qd[1] * g_ip_ref[1] + qd[2] * g_ip_ref[2] + qd[3] * g_ip_ref[3];
    if (chain && g_ip_ref_ok && std::fabs(rd) >= 0.8660) {
        flip = rd < 0;   // a piece of the body that turned before it: the same way
    } else {
        flip = d < 0;    // the shorter way
        if (std::fabs(d) < 0.0872 && g_ip_up_ok)   // 170 to 190 degrees: a fixed way about the camera's up
            flip = qd[1] * g_ip_up[0] + qd[2] * g_ip_up[1] + qd[3] * g_ip_up[2] < 0;
    }
    if (flip) {
        for (int i = 0; i < 4; ++i) { qb[i] = -qb[i]; qd[i] = -qd[i]; }
        d = -d;
    }
    if (chain && std::fabs(d) < 0.7071) {   // a turn of 90 degrees or more leads the pieces drawn after it
        for (int i = 0; i < 4; ++i) g_ip_ref[i] = qd[i];
        g_ip_ref_ok = 1;
    }
    if (d > 0.9995) {
        for (int i = 0; i < 4; ++i) q[i] = qa[i] + (qb[i] - qa[i]) * t;
    } else {
        const double th = std::acos(d), s = std::sin(th);
        const double wa = std::sin((1.0 - t) * th) / s, wb = std::sin(t * th) / s;
        for (int i = 0; i < 4; ++i) q[i] = qa[i] * wa + qb[i] * wb;
    }
    const double qn = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    for (int i = 0; i < 4; ++i) q[i] /= qn;
    double U[3][3];
    ip_quat_mat(q, U);
    for (int i = 0; i < np; ++i) {   // every word the turn does not own (translation, the 4x4's fourth column)
        ex[i] = (double)(int32_t)a[i] * (1.0 - t) + (double)(int32_t)b[i] * t;
        out[i] = (uint32_t)ip_lerp(a[i], b[i], t);
    }
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k) {
            double v = 0;
            for (int j = 0; j < 3; ++j) v += ((1.0 - t) * Sa[r][j] + t * Sb[r][j]) * U[j][k];
            const double w = v * 4096.0;
            ex[r * stride + k] = w;
            out[r * stride + k] = (uint32_t)(int32_t)(w < 0 ? w - 0.5 : w + 0.5);
        }
    ++st.rot;
    return true;
}

/* The blended camera and what re-views an unpaired LOAD through it. */
struct IpCam {
    double RN[3][3], tN[3];     // tick N's view
    double Rt[3][3], tt[3];     // the blended view
};
IpCam g_ip_cam;

void ip_cam_setup(const int32_t *vp, const int32_t *vc, double t) {
    IpCam &c = g_ip_cam;
    ip_set_up(vc);
    uint32_t w[12];
    double ex[12];
    if (ip_rot_blend((const uint32_t *)vp, (const uint32_t *)vc, 12, t, w, ex, false)) {
        /* INTERPTURN1: a camera that swung past the threshold in one tick blends as a turn */
        for (int r = 0; r < 3; ++r)
            for (int k = 0; k < 3; ++k) {
                c.RN[r][k] = vc[r * 3 + k] / 4096.0;
                c.Rt[r][k] = ex[r * 3 + k] / 4096.0;
            }
        for (int k = 0; k < 3; ++k) {
            c.tN[k] = vc[9 + k] / 4096.0;
            c.tt[k] = ex[9 + k] / 4096.0;
        }
        return;
    }
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k) {
            c.RN[r][k] = vc[r * 3 + k] / 4096.0;
            c.Rt[r][k] = ip_lerp((uint32_t)vp[r * 3 + k], (uint32_t)vc[r * 3 + k], t) / 4096.0;
        }
    for (int k = 0; k < 3; ++k) {
        c.tN[k] = vc[9 + k] / 4096.0;
        c.tt[k] = ip_lerp((uint32_t)vp[9 + k], (uint32_t)vc[9 + k], t) / 4096.0;
    }
}

/* LOAD' = LOAD x inv(V_N) x V_t. V_N is a rotation plus a translation, so its
   inverse is [R^T ; -t R^T], and the product is [A R^T Rt ; (a - t) R^T Rt + tt]. */
void ip_recamera(const uint32_t *p, int np, uint32_t *out) {
    const IpCam &c = g_ip_cam;
    const int four = (np == 16);
    double A[3][3], a[3];
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k)
            A[r][k] = (int32_t)p[four ? r * 4 + k : r * 3 + k] / 4096.0;
    for (int k = 0; k < 3; ++k)
        a[k] = (int32_t)p[four ? 12 + k : 9 + k] / 4096.0;
    /* M = R^T Rt */
    double M[3][3];
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k)
            M[r][k] = c.RN[0][r] * c.Rt[0][k] + c.RN[1][r] * c.Rt[1][k] +
                      c.RN[2][r] * c.Rt[2][k];
    double B[3][3], b[3], d[3];
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k)
            B[r][k] = A[r][0] * M[0][k] + A[r][1] * M[1][k] + A[r][2] * M[2][k];
    for (int k = 0; k < 3; ++k) d[k] = a[k] - c.tN[k];
    for (int k = 0; k < 3; ++k)
        b[k] = d[0] * M[0][k] + d[1] * M[1][k] + d[2] * M[2][k] + c.tt[k];
    auto fx = [](double v) {
        const double s = v * 4096.0;
        return (uint32_t)(int32_t)(s < 0 ? s - 0.5 : s + 0.5);
    };
    for (int i = 0; i < np; ++i) { out[i] = p[i]; g_ip_ex[i] = (double)(int32_t)p[i]; }
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k) {
            out[four ? r * 4 + k : r * 3 + k] = fx(B[r][k]);
            g_ip_ex[four ? r * 4 + k : r * 3 + k] = B[r][k] * 4096.0;
        }
    for (int k = 0; k < 3; ++k) {
        out[four ? 12 + k : 9 + k] = fx(b[k]);
        g_ip_ex[four ? 12 + k : 9 + k] = b[k] * 4096.0;
    }
}

/* A newborn particle's MULT_4x3 carries only a view-space point (the 2x2 is the
   billboard, which faces the camera whatever the camera does). Re-view the
   point through the blended camera: p' = (p - t_N) R_N^T R_t + t_t. */
void ip_recamera_trans(const uint32_t *p, uint32_t *out) {
    const IpCam &c = g_ip_cam;
    double M[3][3], d[3], b[3];
    for (int r = 0; r < 3; ++r)
        for (int k = 0; k < 3; ++k)
            M[r][k] = c.RN[0][r] * c.Rt[0][k] + c.RN[1][r] * c.Rt[1][k] +
                      c.RN[2][r] * c.Rt[2][k];
    for (int k = 0; k < 3; ++k) d[k] = (int32_t)p[9 + k] / 4096.0 - c.tN[k];
    for (int k = 0; k < 3; ++k)
        b[k] = d[0] * M[0][k] + d[1] * M[1][k] + d[2] * M[2][k] + c.tt[k];
    for (int i = 0; i < 12; ++i) { out[i] = p[i]; g_ip_ex[i] = (double)(int32_t)p[i]; }
    for (int k = 0; k < 3; ++k) {
        const double sv = b[k] * 4096.0;
        out[9 + k] = (uint32_t)(int32_t)(sv < 0 ? sv - 0.5 : sv + 0.5);
        g_ip_ex[9 + k] = sv;
    }
}

bool ip_proj_perspective() {
    return g.proj.m[3] != 0.0f || g.proj.m[7] != 0.0f || g.proj.m[11] != 0.0f;
}

}  // namespace

void gx_interp_arm(int on) {
    g_ip_armed = on ? 1 : 0;
    if (!g_ip_armed) {
        g_ip_rec = 0;
        g_ip_c = g_ip_p = -1;
    }
}

int gx_interp_armed() { return g_ip_armed; }

void gx_dma_source(uint32_t src) { g_ip_dma_src = src; }

void gx_interp_invalidate() {
    if (g_ip_c >= 0) g_ipr[g_ip_c].valid = 0;
    if (g_ip_p >= 0) g_ipr[g_ip_p].valid = 0;
}

int gx_interp_commit(const int32_t view[12], int snap, GxInterpStats *st) {
    if (st) std::memset(st, 0, sizeof *st);
    if (!g_ip_armed) return 0;
    IpRec &r = g_ipr[g_ip_w];
    if (!r.is_sealed) {
        gx_interp_invalidate();
        if (st) st->snap = 8;
        return 0;
    }
    for (int i = 0; i < 12; ++i) r.view[i] = view[i];
    ip_build_groups(r);
    r.valid = 1;
    int why = snap ? 1 : 0;
    const IpRec *P = (g_ip_c >= 0 && g_ipr[g_ip_c].valid) ? &g_ipr[g_ip_c] : 0;
    if (!P) why |= 2;
    if (P && P->tex_gen != r.tex_gen) why |= 4;
    if (P) {
        double e0[3], f0[3], e1[3], f1[3];
        ip_eye(P->view, e0, f0);
        ip_eye(r.view, e1, f1);
        const double dx = e1[0] - e0[0], dy = e1[1] - e0[1], dz = e1[2] - e0[2];
        const double step = std::sqrt(dx * dx + dy * dy + dz * dz);
        double dot = f0[0] * f1[0] + f0[1] * f1[1] + f0[2] * f1[2];
        const double l0 = std::sqrt(f0[0] * f0[0] + f0[1] * f0[1] + f0[2] * f0[2]);
        const double l1 = std::sqrt(f1[0] * f1[0] + f1[1] * f1[1] + f1[2] * f1[2]);
        if (l0 > 0 && l1 > 0) dot /= l0 * l1;
        if (dot > 1) dot = 1;
        if (dot < -1) dot = -1;
        const double turn = std::acos(dot) * 57.29577951308232;
        if (st) { st->eye_step = (float)step; st->turn_deg = (float)turn; }
        /* A CUT IS A JUMP, NOT A SPEED. A scripted camera can sweep fast and
           smoothly (the star-get swoop moves 40 to 55 units a tick for
           several ticks, decelerating), and that motion should blend like
           any other. What a cut does is move far MORE than the tick before
           did: past the threshold AND more than twice the previous tick's
           step (or turn), or off in another direction than the tick before
           went (a sweep keeps its heading; one cut after another does not).
           Past ten times the threshold it is a cut whatever came before. A
           snap for any other reason leaves no "before", so the next tick is
           judged against standing still. */
        double hd = 1.0;
        const double pl = std::sqrt(g_ip_prev_disp[0] * g_ip_prev_disp[0] +
                                    g_ip_prev_disp[1] * g_ip_prev_disp[1] +
                                    g_ip_prev_disp[2] * g_ip_prev_disp[2]);
        if (pl > 1e-9 && step > 1e-9)
            hd = (dx * g_ip_prev_disp[0] + dy * g_ip_prev_disp[1] +
                  dz * g_ip_prev_disp[2]) / (pl * step);
        const bool jump = step > ip_cut_units() &&
                          (step > 2.0 * g_ip_prev_step || hd < 0.5 ||
                           step > 10.0 * ip_cut_units());
        const bool spin = turn > ip_cut_degrees() &&
                          (turn > 2.0 * g_ip_prev_turn || turn > 60.0);
        if (jump || spin) why |= 16;
        g_ip_prev_step = step;
        g_ip_prev_turn = turn;
        g_ip_prev_disp[0] = dx;
        g_ip_prev_disp[1] = dy;
        g_ip_prev_disp[2] = dz;
    }
    if (why & ~16) {
        g_ip_prev_step = g_ip_prev_turn = 0.0;
        g_ip_prev_disp[0] = g_ip_prev_disp[1] = g_ip_prev_disp[2] = 0.0;
    }
    r.snap = why;
    {
        /* the record two ticks back is the one the rotation below leaves in the free slot */
        const IpRec *P2 = (g_ip_p >= 0 && g_ipr[g_ip_p].valid && !g_ipr[g_ip_p].snap &&
                           g_ipr[g_ip_p].tex_gen == r.tex_gen) ? &g_ipr[g_ip_p] : 0;
        if (P && !why) { ip_match(r, *P, st, P2); ip_ghost_pairs(r, *P); }
    }
    if (st) st->snap = why;
    /* rotate: N becomes N-1, the record just sealed becomes N, and the old
       N-1 (or whichever of the three is free) is written next */
    g_ip_p = g_ip_c;
    g_ip_c = g_ip_w;
    for (int i = 0; i < 3; ++i)
        if (i != g_ip_c && i != g_ip_p) { g_ip_w = i; break; }
    g_ipr[g_ip_w].is_sealed = 0;
    return why ? 0 : 1;
}

int gx_interp_ready() {
    if (!g_ip_armed || g_ip_c < 0 || g_ip_p < 0) return 0;
    const IpRec &C = g_ipr[g_ip_c];
    if (!C.valid || C.snap || !g_ipr[g_ip_p].valid) return 0;
    if (C.tex_gen != g_tex_generation) return 0;
    return 1;
}

void gx_interp_view(float alpha, float eye[3]) {
    eye[0] = eye[1] = eye[2] = 0;
    if (g_ip_c < 0) return;
    int32_t v[12];
    const IpRec &C = g_ipr[g_ip_c];
    uint32_t w[12];
    double ex[12];
    ip_set_up(C.view);
    if (g_ip_p >= 0 && ip_rot_blend((const uint32_t *)g_ipr[g_ip_p].view, (const uint32_t *)C.view, 12, alpha, w, ex, false)) {
        for (int i = 0; i < 12; ++i) v[i] = (int32_t)w[i];   // INTERPTURN1: the same turn-aware camera ip_cam_setup builds
    } else
    for (int i = 0; i < 12; ++i)
        v[i] = (g_ip_p >= 0) ? ip_lerp((uint32_t)g_ipr[g_ip_p].view[i],
                                       (uint32_t)C.view[i], alpha)
                             : C.view[i];
    double e[3], f[3];
    ip_eye(v, e, f);
    for (int i = 0; i < 3; ++i) eye[i] = (float)e[i];
}

/* FADEFLASH1: A PIECE FIXED TO THE SCREEN. FaderWipe::AdvanceFade draws the wipe's mesh at (0, 0, -1) in camera
   space, so on every tick it sits centred on the picture whatever the camera does. Paired with the previous tick as a
   world object it is not: its "world position" rides the camera, so the jump the camera makes when a pit death starts
   (the camera stops following) reads as a teleport, the group goes unpaired, and the replay re-views its LOAD through
   the blended camera. The mesh, scaled about 31 times, then turns into the view and the picture between the two ticks
   is black around a hole that is off screen or the wrong size: one black picture, then the next tick is right. The
   matrices drawn between begin and end blend word by word with the same events of tick N-1's range (both camera
   space, so no camera goes in) and the hole closes smoothly; with no such range (the wipe's first tick) they keep
   tick N's words. Inert unless a frame is being recorded. */
void gx_interp_screen_begin() {
    if (!g_ip_rec) return;
    IpRec &r = g_ipr[g_ip_w];
    r.screen_ev0 = r.screen_ev1 = static_cast<uint32_t>(r.ev.size());
}

void gx_interp_screen_end() {
    if (!g_ip_rec) return;
    IpRec &r = g_ipr[g_ip_w];
    r.screen_ev1 = static_cast<uint32_t>(r.ev.size());
}

int gx_interp_begin(float alpha) {
    if (g_ip_open || !gx_interp_ready()) return 0;
    if (alpha < 0) alpha = 0;
    if (alpha > 1) alpha = 1;
    const IpRec &C = g_ipr[g_ip_c];
    const IpRec &P = g_ipr[g_ip_p];
    IpSaved &s = g_ip_saved;
    s.g = std::move(g);
    s.teximage = g_teximage;
    s.plttbase = g_plttbase;
    s.seen_pos = g_mtx_seen_pos;
    s.seen_vec = g_mtx_seen_vec;
    s.mtx_gen = g_mtx_gen;
    s.similar = g_mtx_similar;
    for (int i = 0; i < MTX_RING; ++i) {
        s.ring[i] = g_mtx_ring[i];
        s.ring_gen[i] = g_mtx_ring_gen[i];
        s.ring_sim[i] = g_mtx_ring_sim[i];
    }
    s.rec = g_ip_rec;
    g_ip_rec = 0;
    g_ip_open = 1;

    g = C.start;
    /* MENUINTERP2: the replay starts from the tick's own wide-page answer (the callback reads ROM state and must not be asked again) */
    g_ip_sprite_saved = g_menu_sprite;
    g_menu_sprite = 0;
    g_ip_sprxf_saved_on = g_menu_sprxf_on;
    g_menu_sprxf_on = 0;
    if (C.mw_eval) {
        g.mw_eval = 1; g.mw_on = C.mw_on; g.mw_zoom = C.mw_zoom; g.mw_dy = C.mw_dy;
        g.mw_kx = C.mw_kx; g.mw_tx = C.mw_tx; g.mw_ky = C.mw_ky; g.mw_ty = C.mw_ty;
        if (g.mw_on) { g.vp_x = 0; g.vp_y = 0; g.vp_w = active_w; g.vp_h = active_h; }
    }
    g.tris = std::move(g_ip_tris);
    g.grids = std::move(g_ip_grids);
    g.grid_pts = std::move(g_ip_grid_pts);
    g.grids.clear();
    g.grid_pts.clear();
    g.grid_subtris = 0;
    g.grid_translucent = 0;
    g.grid_mode2 = 0;
    g_ip_grid_defer_saved = g_grid_defer;
    g_grid_defer = grid_defer_wanted();
    g.strip = std::move(g_ip_strip);
    g.strip_raw = std::move(g_ip_strip_raw);
    g.tris.clear();
    g_ip_gh.clear();
    g.strip.clear();
    g.strip_raw.clear();
    g_teximage = C.start_teximage;
    g_plttbase = C.start_plttbase;
    g_ip_rot_stat = IpRotStat{};
    g_ip_ref_ok = 0;   // INTERPTURN1: every picture starts with no reference turn
    g_ip_rot_alpha = alpha;
    ip_cam_setup(P.view, C.view, alpha);
    g_ip_group_tri0.assign(C.groups.size(), 0);
    g_ip_group_tri1.assign(C.groups.size(), 0);

    static int pic = 0;
    ++pic;
    g_ip_replaying = 1;
    g_wm_cursor = 0;          /* SMOOTHWELD1: the replay reads the tick's weld answers from the top */
    if (smooth_cracks_on()) smooth_crack_picture();
    uint32_t tmp[32];
    int last_group = -1;
    const bool gfast = !C.gpair.empty() && g_ipr[g_ip_p].gt_ok && g_gf_override != 0;
    if (gfast && !C.nn_valid) {
        C.nn.assign((size_t)C.sealed + 1, 0);
        C.nn[C.sealed] = C.sealed;
        for (uint32_t i = C.sealed; i-- > 0;) {
            const IpEv &e = C.ev[i];
            const int gi = C.ev_group[i];
            const bool skip = gi >= 0 && C.gpair[gi].p >= 0 && e.kind == IPK_CMD &&
                              ((e.cmd >= 0x20 && e.cmd <= 0x28) || e.cmd == 0x40 || e.cmd == 0x41);
            if (!skip) C.nn[i] = i;
            else C.nn[i] = (i + 1 < C.sealed && C.ev_group[i + 1] == gi) ? C.nn[i + 1] : i + 1;
        }
        C.nn_valid = 1;
    }
    const bool use_nn = gfast && C.nn_valid && C.nn.size() == (size_t)C.sealed + 1;
    for (uint32_t i = 0; i < C.sealed; ++i) {
        if (use_nn) {
            const uint32_t j = C.nn[i];
            if (j != i) { i = j - 1; continue; }
        }
        const IpEv &e = C.ev[i];
        const int gi = C.ev_group[i];
        if (gi != last_group) {
            if (last_group >= 0) g_ip_group_tri1[last_group] = (uint32_t)g.tris.size() + g.grid_subtris;
            if (gi >= 0) g_ip_group_tri0[gi] = (uint32_t)g.tris.size() + g.grid_subtris;
            last_group = gi;
            if (gfast && gi >= 0 && C.gpair[gi].p >= 0) ip_ghost_emit(C, P, C.gpair[gi], alpha);
        }
        switch (e.kind) {
        case IPK_CMD: {
            if (e.cmd == 0x50 || e.cmd == 0x2A || e.cmd == 0x2B) break;
            if (gfast && gi >= 0 && C.gpair[gi].p >= 0 && ((e.cmd >= 0x20 && e.cmd <= 0x28) || e.cmd == 0x40 || e.cmd == 0x41)) break;
            const uint32_t *pp = e.np ? &C.par[e.off] : nullptr;
            int ex_on = 0;
            uint32_t tex_adj[32];
            bool tex_wrapped = false;
            if (ip_is_mtx(e.cmd) && e.np <= 32 && i >= C.screen_ev0 && i < C.screen_ev1) {
                /* FADEFLASH1: a matrix fixed to the screen blends its words straight with the same event of tick N-1's
                   screen-fixed range (both are camera space, no camera goes in); with no such event it keeps tick N's */
                const uint32_t k0 = i - C.screen_ev0;
                if (alpha < 1.0f && e.mode != MTX_PROJ && P.screen_ev1 - P.screen_ev0 == C.screen_ev1 - C.screen_ev0 &&
                    P.screen_ev0 + k0 < P.ev.size()) {
                    const IpEv &pe = P.ev[P.screen_ev0 + k0];
                    if (pe.kind == IPK_CMD && pe.cmd == e.cmd && pe.np == e.np && pe.mode == e.mode) {
                        for (int k = 0; k < e.np; ++k) {
                            tmp[k] = (uint32_t)ip_lerp(P.par[pe.off + k], pp[k], alpha);
                            g_ip_ex[k] = (double)(int32_t)P.par[pe.off + k] * (1.0 - alpha) + (double)(int32_t)pp[k] * alpha;
                        }
                        pp = tmp;
                        ex_on = 1;
                    }
                }
            } else if (ip_is_mtx(e.cmd) && e.np <= 32) {
                const int b = C.blend.empty() ? -1 : C.blend[i];
                const int b2 = (C.has_p2 && !C.blend2.empty()) ? C.blend2[i] : -1;
                if (b2 >= 0 && b < 0 && e.mode != MTX_TEX && g_ipr[g_ip_w].valid) {
                    /* a blinking model: N-2 -> N, the picture sits (1 + alpha) / 2 of the way */
                    const IpRec &P2 = g_ipr[g_ip_w];
                    const IpEv &pe = P2.ev[b2];
                    const double w2 = (1.0 + (double)alpha) * 0.5;
                    const int n = e.np < pe.np ? e.np : pe.np;
                    if (e.np == pe.np && e.mode != MTX_PROJ && e.cmd != 0x1B && e.cmd != 0x1C &&
                        ip_rot_blend(&P2.par[pe.off], pp, e.np, w2, tmp, g_ip_ex, true)) {
                        /* INTERPTURN1: a turn past the threshold blends as a turn */
                    } else
                    for (int k = 0; k < e.np; ++k) {
                        tmp[k] = k < n ? (uint32_t)ip_lerp(P2.par[pe.off + k], pp[k], w2) : pp[k];
                        g_ip_ex[k] = k < n ? (double)(int32_t)P2.par[pe.off + k] * (1.0 - w2) +
                                             (double)(int32_t)pp[k] * w2
                                           : (double)(int32_t)pp[k];
                    }
                    pp = tmp;
                    ex_on = 1;
                } else if (b >= 0 && gi >= 0 && C.groups[gi].part && e.np == 12 && alpha < 1.0f && ip_proj_perspective() &&
                           (ip_sparkle_mode() == 1 || (ip_sparkle_mode() >= 4 && ip_part_twinkle(C.groups[gi], P.groups[C.groups[gi].match])) ||
                            (ip_sparkle_mode() >= 5 && ip_bind_is_sparkle(C, C.groups[gi].bind)))) {
                    /* INTERP093 I2: a particle keeps tick N's own matrix (size, turn, place): the sparkles twinkle and turn a lot between
                       two ticks and a blend of two billboards' matrices is neither; the point is re-viewed through the blended camera */
                    ip_recamera_trans(pp, tmp);
                    pp = tmp;
                    ex_on = 1;
                } else if (b >= 0 && (e.mode != MTX_TEX || ip_tex_pair_ok(P, b, e, pp, tex_adj, tex_wrapped))) {
                    const IpEv &pe = P.ev[b];
                    const int n = e.np < pe.np ? e.np : pe.np;
                    /* TEXTBOX2: a step across a texture wrap blends to the matrix with the wrap taken out (tick N's own at alpha 1) */
                    if (e.mode == MTX_TEX && tex_wrapped && alpha < 1.0f) pp = tex_adj;
                    if (e.np == pe.np && e.mode != MTX_TEX && e.mode != MTX_PROJ && e.cmd != 0x1B && e.cmd != 0x1C &&
                        ip_rot_blend(&P.par[pe.off], pp, e.np, alpha, tmp, g_ip_ex, true)) {
                        /* INTERPTURN1: a turn past the threshold blends as a turn */
                    } else
                    for (int k = 0; k < e.np; ++k) {
                        tmp[k] = k < n ? (uint32_t)ip_lerp(P.par[pe.off + k], pp[k], alpha)
                                       : pp[k];
                        g_ip_ex[k] = k < n ? (double)(int32_t)P.par[pe.off + k] * (1.0 - alpha) +
                                             (double)(int32_t)pp[k] * alpha
                                           : (double)(int32_t)pp[k];
                    }
                    pp = tmp;
                    ex_on = 1;
                } else if (b == -3 && alpha < 1.0f && ip_proj_perspective()) {
                    ip_recamera_trans(pp, tmp);
                    pp = tmp;
                    ex_on = 1;
                } else if (b == -2 && alpha < 1.0f && ip_proj_perspective()) {
                    /* at alpha 1 the blended camera IS tick N's and the
                       re-view is the identity: skip it, so the last picture
                       of a tick is the tick's own to the bit */
                    ip_recamera(pp, e.np, tmp);
                    pp = tmp;
                    ex_on = 1;
                }
            }
            {
                static const int pdraw = std::getenv("SM64DS_PART_DRAW") ? 1 : 0;
                if (pdraw && gi >= 0 && C.groups[gi].part && e.np == 12 && e.cmd == 0x19) {
                    const double a_ = (int32_t)pp[0] / 4096.0, b_ = (int32_t)pp[1] / 4096.0;
                    std::fprintf(stderr, "[pdraw] tick %d pic %d alpha %.3f ps %.5f x %.4f y %.4f z %.4f sp %d match %d\n", g_ip_tick_no, pic, (double)alpha, std::sqrt(a_ * a_ + b_ * b_),
                                 (int32_t)pp[9] / 4096.0, (int32_t)pp[10] / 4096.0, (int32_t)pp[11] / 4096.0, ip_bind_is_sparkle(C, C.groups[gi].bind) ? 1 : 0, C.groups[gi].match);
                }
            }
            g_ip_ex_on = ex_on;
            exec(e.cmd, pp, e.np);
            g_ip_ex_on = 0;
            break;
        }
        case IPK_BIND: {
            const IpBind &bd = C.binds[e.off];
            g.tex_rgba = bd.rgba;
            g.tw = bd.w;
            g.th = bd.h;
            g.tex_wrap = bd.wrap;
            g.tex_scale = bd.scale;
            g.tex_id = bd.id;
            g_teximage = bd.teximage;
            g_plttbase = bd.plttbase;
            break;
        }
        case IPK_LIGHT: {
            const uint32_t *q = &C.par[e.off];
            float d[3];
            std::memcpy(d, q + 1, sizeof d);
            gx_set_light((int)q[0], d[0], d[1], d[2], q[4]);
            break;
        }
        case IPK_LMASK:
            gx_enable_lights(C.par[e.off]);
            break;
        case IPK_SPRITE:
            g_menu_sprite = C.par[e.off] ? 1 : 0;
            g_menu_sprxf_on = 0;
            break;
        case IPK_SPRXF: {
            float v[4];
            std::memcpy(v, &C.par[e.off], sizeof v);
            for (int i = 0; i < 4; ++i) g_menu_sprxf[i] = v[i];
            g_menu_sprxf_on = 1;
            break;
        }
        case IPK_SLOT: {
            float m[16];
            std::memcpy(m, &C.par[e.off + 1], sizeof m);
            gx_set_matrix_slot((int)C.par[e.off], m);
            break;
        }
        default: break;
        }
    }
    if (last_group >= 0) g_ip_group_tri1[last_group] = (uint32_t)g.tris.size() + g.grid_subtris;
    g_ip_replaying = 0;
    return 1;
}

void gx_ghost_fast_override(int v) { g_gf_override = v; }

void gx_interp_end() {
    if (!g_ip_open) return;
    if (ip_rot_log()) {
        const IpRotStat &rs = g_ip_rot_stat;
        std::fprintf(stderr, "[iprot] tick %d alpha %.3f mats %u rot %u back %u deg<1 %u <2 %u <5 %u <10 %u <20 %u <45 %u <90 %u >=90 %u\n",
                     g_ip_tick_no, (double)g_ip_rot_alpha, rs.mats, rs.rot, rs.back, rs.hist[0], rs.hist[1], rs.hist[2], rs.hist[3],
                     rs.hist[4], rs.hist[5], rs.hist[6], rs.hist[7]);
    }
    IpSaved &s = g_ip_saved;
    g_ip_tris = std::move(g.tris);
    g_ip_grids = std::move(g.grids);
    g_ip_grid_pts = std::move(g.grid_pts);
    g_grid_defer = g_ip_grid_defer_saved;
    g_ip_strip = std::move(g.strip);
    g_ip_strip_raw = std::move(g.strip_raw);
    g = std::move(s.g);
    g_teximage = s.teximage;
    g_plttbase = s.plttbase;
    g_mtx_seen_pos = s.seen_pos;
    g_mtx_seen_vec = s.seen_vec;
    g_mtx_gen = s.mtx_gen;
    g_mtx_similar = s.similar;
    for (int i = 0; i < MTX_RING; ++i) {
        g_mtx_ring[i] = s.ring[i];
        g_mtx_ring_gen[i] = s.ring_gen[i];
        g_mtx_ring_sim[i] = s.ring_sim[i];
    }
    g_ip_rec = s.rec;
    g_menu_sprite = g_ip_sprite_saved;
    g_menu_sprxf_on = g_ip_sprxf_saved_on;
    g_ip_open = 0;
}

int gx_interp_group_box(int group, float box[4]) {
    grids_materialize_w(5);     // the group ranges are triangle counts; they hold once the grids are triangles
    if (group < 0 || (size_t)group >= g_ip_group_tri0.size()) return 0;
    const uint32_t a = g_ip_group_tri0[group], b = g_ip_group_tri1[group];
    if (b <= a || b > g.tris.size()) return 0;
    float mnx = 1e30f, mxx = -1e30f, mny = 1e30f, mxy = -1e30f;
    for (uint32_t i = a; i < b; ++i)
        for (int v = 0; v < 3; ++v) {
            const GxVertex &q = g.tris[i].v[v];
            if (q.x < mnx) mnx = q.x;
            if (q.x > mxx) mxx = q.x;
            if (q.y < mny) mny = q.y;
            if (q.y > mxy) mxy = q.y;
        }
    box[0] = mnx; box[1] = mny; box[2] = mxx; box[3] = mxy;
    {
        static int lg = -1;   // INTERPTURN1 measurement: every box the probe asks for
        if (lg < 0) lg = getenv("SM64DS_IP_BOX_LOG") ? 1 : 0;
        if (lg) std::fprintf(stderr, "[ipbox] g%d n%u x %.2f %.2f y %.2f %.2f\n", group, b - a, mnx, mxx, mny, mxy);
    }
    return (int)(b - a);
}

/* The shape store is about to throw its pool away: turn every kept grid that
   points into it (the live frame's, and the tick a SmoothMotion replay has put
   aside) into triangles first. */
static void grid_pool_clear_hook() {
    grids_materialize_w(6);
    if (g_ip_open && !g_ip_saved.g.grids.empty()) {
        std::swap(g, g_ip_saved.g);
        grids_materialize_w(7);
        std::swap(g, g_ip_saved.g);
    }
}

/* Probe: how the current tick's group gi was paired. Bit 0 a particle, bit 1
   paired with the previous tick's group. -1 outside a record. */
int gx_interp_group_flags(int gi) {
    if (g_ip_c < 0 || gi < 0 || gi >= (int)g_ipr[g_ip_c].groups.size()) return -1;
    const IpGroup &G = g_ipr[g_ip_c].groups[gi];
    return (G.part ? 1 : 0) | (G.match >= 0 ? 2 : 0);
}

int gx_interp_group_count() {
    return g_ip_c >= 0 ? (int)g_ipr[g_ip_c].groups.size() : 0;
}


/* The ticks' own geometry as a key/position table, one line per group, for
   the discontinuity probe: which groups were paired and how far they moved. */
void gx_interp_dump_groups(void *fp, int frame) {
    FILE *f = static_cast<FILE *>(fp);
    if (g_ip_c < 0 || !f) return;
    const IpRec &C = g_ipr[g_ip_c];
    for (size_t i = 0; i < C.groups.size(); ++i) {
        const IpGroup &G = C.groups[i];
        double d = -1;
        if (G.match >= 0 && g_ip_p >= 0) {
            const IpGroup &Q = g_ipr[g_ip_p].groups[G.match];
            if (G.has_pos && Q.has_pos) d = std::sqrt(ip_dist2(G, Q));
        }
        const IpEv &e0 = C.ev[C.mtx[G.mtx0]];
        std::fprintf(f, "[interpg] f%d g%zu key %010llx nmtx %d match %d pos %.1f %.1f %.1f moved %.2f first %02x/m%d rel %d %.2f %.2f %.2f\n",
                     frame, i, (unsigned long long)G.key, G.nmtx, G.match,
                     G.px, G.py, G.pz, d, e0.cmd, e0.mode, G.has_rel, G.rx,
                     G.ry, G.rz);
    }
}

}  // namespace ntr


/* FADEFLASH1: C-linkage faces of gx_interp_screen_begin / _end for hal/fader_wipes.cpp, whose declarations sit
   where the ROM headers it includes give every name C linkage */
extern "C" void port_gx_interp_screen_begin(void) { ntr::gx_interp_screen_begin(); }
extern "C" void port_gx_interp_screen_end(void) { ntr::gx_interp_screen_end(); }
