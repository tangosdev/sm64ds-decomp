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

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <mutex>
#include <thread>
#include <type_traits>
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
enum { MTX_RING = 8 };
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

/* VIEW SPACE -> CLIP SPACE, factored out of project() and out of nothing
   else. The model smoother (ntr/smooth.h) projects the vertices it invents
   through THIS function rather than through a copy of these six lines, so a
   subdivided vertex cannot drift away from the corner it came from: the
   projection matrix, the widescreen widen and its perspective test are one
   piece of code with one caller each. Byte-identical to what project() did
   inline before, same expression, same order. */
Vec4 view_to_clip(const Vec4 &view) {
    Vec4 c = mul(view, g.proj);
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

void push_screen_tri(const GxVertex &a, const GxVertex &b, const GxVertex &c) {
    /* value-initialised: smoke_gx memcmps whole GxTriangles between the two
       submit paths, so the padding has to be deterministic */
    GxTriangle t{};
    t.v[0] = a; t.v[1] = b; t.v[2] = c;
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
void smooth_emit_grid(const float *pts, int tf, const SmoothVertex s[3],
                      const Mat *xform) {
    GxVertex v[SMOOTH_MAX_GRID];
    const float inv = 1.0f / (float)tf;
    int n = 0;
    for (int ia = 0; ia <= tf; ++ia) {
        for (int ib = 0; ib <= tf - ia; ++ib, ++n) {
            const float a = (float)ia * inv;
            const float b = (float)ib * inv;
            float c = 1.0f - a - b;
            if (c < 0.0f) c = 0.0f;
            if (xform) {
                const Vec4 l{pts[n * 3], pts[n * 3 + 1], pts[n * 3 + 2], 1.0f};
                const Vec4 w = mul(l, *xform);
                const float vp[3] = {w.x, w.y, w.z};
                smooth_grid_vertex(vp, a, b, c, s, v[n]);
            } else {
                smooth_grid_vertex(pts + n * 3, a, b, c, s, v[n]);
            }
        }
    }
    const uint16_t *idx = smooth_grid_tri_index(tf);
    const int ntris = smooth_grid_tris(tf);
    for (int i = 0; i < ntris; ++i)
        emit_tri_near(v[idx[i * 3]], v[idx[i * 3 + 1]], v[idx[i * 3 + 2]]);
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
                     int prof) {
    const Mat *xform = mtx_for_gen(rc.mgen);
    if (!xform) {
        smooth_store_count(SMOOTH_STORE_NONSIM, 1);
        return 0;
    }

    SmoothKey key;
    const GxRaw *r[3] = {&ra, &rb, &rc};
    for (int i = 0; i < 3; ++i) {
        key.p[i][0] = r[i]->x; key.p[i][1] = r[i]->y; key.p[i][2] = r[i]->z;
        key.n[i] = r[i]->nrm;
    }
    key.level = static_cast<uint32_t>(pol.level);

    const long long t_sub = prof ? smooth_prof_ticks() : 0;
    const SmoothEntry *e = smooth_store_find(key);
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
        float pts[SMOOTH_MAX_GRID * 3];
        smooth_grid_positions(l[0], l[1], l[2], tf, pts);
        e = smooth_store_add(key, tf, pts);
        if (!e) return 0;
    }
    if (e->tf != tf) return 0;     /* built at another level: rebuild live */
    const float *grid = smooth_store_grid(e);
    if (!grid) return 0;

    if (smooth_abdiff_on()) {
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

    smooth_emit_grid(grid, tf, s, xform);
    if (prof) smooth_prof_add(SMOOTH_PROF_SUBDIV,
                              smooth_prof_ticks() - t_sub, 1);
    return 1;
}

int smooth_try(const GxVertex &a, const GxVertex &b, const GxVertex &c,
               const GxRaw &ra, const GxRaw &rb, const GxRaw &rc) {
    const SmoothPolicy &pol = smooth_policy();

    /* A mode-3 polygon is a shadow volume. Its stencil protocol compares the
       mask pass against the draw pass PIXEL FOR PIXEL, so both have to be the
       same geometry; a curved shadow volume would not close. */
    if (((g.poly_attr >> 4) & 3) == 3) {
        smooth_count(SMOOTH_COUNT_MODE3, 1);
        return 0;
    }
    /* A constant-w projection is an ortho one: the HUD's own 3D geometry and
       every 2D framing trick in the port. No view volume, no surface. */
    if (g.proj.m[3] == 0.0f && g.proj.m[7] == 0.0f && g.proj.m[11] == 0.0f) {
        smooth_count(SMOOTH_COUNT_ORTHO, 1);
        return 0;
    }
    /* A non-affine position matrix would make the view-space positions below
       projective, and a patch built in projective coordinates is not the
       patch anyone meant. No matrix the game loads is like this; the guard is
       here so that stays a measured fact rather than an assumption. */
    if (a.vw != 1.0f || b.vw != 1.0f || c.vw != 1.0f) {
        smooth_count(SMOOTH_COUNT_W, 1);
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
    const int tf = smooth_tess_factor(s[0], s[1], s[2], pol, &why);
    if (prof) smooth_prof_add(SMOOTH_PROF_POLICY,
                              smooth_prof_ticks() - t_pol, 1);
    if (smooth_census_on()) smooth_census_tri(s[0], s[1], s[2], tf);
    if (tf <= 1) {
        smooth_counters_for(why);
        return 0;
    }

    smooth_count(SMOOTH_COUNT_SUBDIVIDED, 1);

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

void emit_tri(const GxVertex &a, const GxVertex &b, const GxVertex &c,
              const GxRaw &ra, const GxRaw &rb, const GxRaw &rc) {
    smooth_count(SMOOTH_COUNT_IN, 1);
    /* OFF IS ONE COMPARE. smooth_level() is a load of a file-scope int that
       walk_window sets once at boot; with the setting absent it is 0 and this
       function is the same call it always was. */
    if (smooth_level() > 0 && smooth_try(a, b, c, ra, rb, rc)) return;
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
            const int x1 = p[0] & 0xFF, y1 = (p[0] >> 8) & 0xFF;
            const int x2 = (p[0] >> 16) & 0xFF, y2 = (p[0] >> 24) & 0xFF;
            /* Scaled into the PRESENT rectangle and offset by its origin, so
               a game-issued full-screen viewport fills the picture the host is
               presenting rather than the whole wide buffer. present_* is the
               active extent at the origin on every other run, so this is the
               same math as before. */
            g.vp_x = present_x() + x1 * present_w() / 256;
            g.vp_y = present_y() + y1 * present_h() / 192;
            g.vp_w = (x2 - x1 + 1) * present_w() / 256;
            g.vp_h = (y2 - y1 + 1) * present_h() / 192;
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
}

uint32_t gx_texture_generation() { return g_tex_generation; }

void gx_set_gpu_opaque(GxGpuOpaqueFn fn) { g_gpu_opaque = fn; }
int gx_gpu_opaque_registered() { return g_gpu_opaque != nullptr; }

/* ---- THE TWO PICTURE-SMOOTHING SETTINGS' LATCHES (run hd2) ---------------
   Both are called once at boot from walk_window, beside ntr::configure_aspect,
   and both clamp here as well as at the settings accessor: a caller that
   reaches this entry from anywhere else (a smoke, a harness) cannot hand the
   raster a mode it has no body for. */
void gx_configure_texture_filter(int mode) {
    g_tex_filter = mode < 0 ? 0 : (mode > 2 ? 2 : mode);
}

int gx_texture_filter() { return g_tex_filter; }

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

void gx_reset() {
    ++g_resets;
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
static int tex_coord_i(int i, int size, bool repeat, bool flip) {
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

/* The float entry, which is what the nearest sampler has always called. The
   body above used to be inline here; splitting the integer half out is what
   lets a filtered tap put each of its four texel indices through the SAME
   wrap, clamp and flip rule, per axis, instead of approximating it. Floor
   then the identical integer arithmetic: the nearest path samples the texel
   it always sampled. */
static int tex_coord(float f, int size, bool repeat, bool flip) {
    return tex_coord_i(static_cast<int>(std::floor(f)), size, repeat, flip);
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
    const float flu = std::floor(fu), flv = std::floor(fv);
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
 * three-row window: the raster's row bands are INTERLEAVED (a thread owns
 * rows tid, tid+nt, ...), so a thread's neighbours are always another
 * thread's rows.
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

const uint8_t *gx_coverage()
{
    raster_buffers();
    return &g_cover[0][0];
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
                         "%016llx\n",
                         g_aa_mode, g_aa_run_frames, g_aa_changed, cap, ref,
                         pre, hash);
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
   Every one of these was set AFTER the first honest run rather than guessed
   before it, and each leaves room for a scene this lane did not measure. The
   first measurements, castle grounds at the default render size, 300 frames,
   TextureFilter 0, on both the card and WARP:
     coverage IoU        0.99894 (worst frame 0.99868)
     colour mean         0.0136 of a 0..255 channel
     outlier share       0.00040 of interior pixels, at a tolerance of 2
     depth interior max  measured below; the ALL-COVERED max is 2.4e-3, and
                         that one is an edge pixel where the two rasterisers
                         legitimately picked different triangles, which is why
                         the gate is on the interior number
     polygon id wrong    0
   The floors and ceilings below are those numbers with between five and forty
   times of room, because a lane cannot measure every scene and a gate that
   only just passes the scene it was tuned on is not a gate. */
int g_ab_mode = -1;      /* -1 not read yet */
int g_ab_every = 30;
int g_ab_tol = 2;
double g_ab_iou_floor = 0.995;          /* measured 0.99868 at worst */
double g_ab_mean_ceiling = 0.5;         /* measured 0.0136, 36x of room */
double g_ab_outlier_ceiling = 0.002;    /* measured 0.00040, 5x of room */
double g_ab_depth_ceiling = 1e-4;       /* interior only; see above */
double g_ab_id_ceiling = 0.001;         /* measured 0 */
int g_ab_shot_frame = -1;
const char *g_ab_shot_dir = nullptr;

/* the run's totals */
unsigned long long g_ab_frames, g_ab_both, g_ab_union, g_ab_inter;
unsigned long long g_ab_outliers, g_ab_interior, g_ab_id_bad;
double g_ab_colsum, g_ab_depsum, g_ab_depmax, g_ab_depmax_in;
double g_ab_worst_iou = 2.0;
int g_ab_worst_frame = -1;
double g_ab_worst_outshare;

/* B, the software arm's copy of the four buffers, and the framebuffer as it
   was before either arm drew. Heap, allocated once, only in this mode. */
std::vector<uint32_t> g_ab_save, g_ab_fb;
std::vector<float> g_ab_dep;
std::vector<uint8_t> g_ab_cov, g_ab_id;

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
    if (!g_ab_frames) return;
    const double iou = g_ab_union ? (double)g_ab_inter / (double)g_ab_union : 1.0;
    const double mean = g_ab_both ? g_ab_colsum / (double)(g_ab_both * 3) : 0.0;
    const double outshare =
        g_ab_interior ? (double)g_ab_outliers / (double)g_ab_interior : 0.0;
    const double depmean = g_ab_both ? g_ab_depsum / (double)g_ab_both : 0.0;
    const double idshare =
        g_ab_interior ? (double)g_ab_id_bad / (double)g_ab_interior : 0.0;
    const bool pass = iou >= g_ab_iou_floor && mean <= g_ab_mean_ceiling &&
                      outshare <= g_ab_outlier_ceiling &&
                      g_ab_depmax_in <= g_ab_depth_ceiling &&
                      idshare <= g_ab_id_ceiling;
    std::fprintf(stderr,
                 "[renderer-ab] summary over %llu frame(s): coverage IoU %.6f "
                 "(floor %.6f), colour mean %.4f (ceiling %.4f), outliers "
                 "%llu of %llu interior = %.6f (ceiling %.6f), depth interior "
                 "max %.3e (ceiling %.3e) all-covered max %.3e mean %.3e, "
                 "polygon-id wrong %llu = %.6f (ceiling %.6f), worst frame %d "
                 "at IoU %.6f outliers %.6f VERDICT=%s\n",
                 g_ab_frames, iou, g_ab_iou_floor, mean, g_ab_mean_ceiling,
                 g_ab_outliers, g_ab_interior, outshare, g_ab_outlier_ceiling,
                 g_ab_depmax_in, g_ab_depth_ceiling, g_ab_depmax, depmean,
                 g_ab_id_bad, idshare, g_ab_id_ceiling, g_ab_worst_frame,
                 g_ab_worst_iou, g_ab_worst_outshare, pass ? "PASS" : "FAIL");
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

void gx_render(Framebuffer &fb) {
    /* LAST FRAME'S PRE-SMOOTHING COPY STOPS BEING THIS FRAME'S HERE, before
       anything is drawn. A capture that somehow ran against a frame this
       function never finished would otherwise sample the frame before it. */
    g_aa_pre_valid = 0;
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

    /* Depth clear. 1e30f is not a repeating byte pattern so memset cannot do
       it, but one row can be built scalar and the rest copied from it, which
       is memcpy's problem rather than a scalar loop's. */
    raster_buffers();
    DepthRow *const depth = g_depth;
    for (int x = 0; x < cw; ++x) depth[0][x] = 1e30f;
    for (int y = 1; y < ch; ++y)
        std::memcpy(depth[y], depth[0], (size_t)cw * sizeof(float));

    /* THE 3D COVERAGE MASK, see gx_coverage() in ntr/gx.h. One byte per pixel,
       set beside every store into fb.px below and cleared here. It is what
       lets the 2D compositor tell a pixel this engine drew from a pixel the
       frame's clear left, which is the whole of "a BG at priority 3 sits
       BEHIND the 3D layer at priority 1".
       It is written from the raster bands, and that is safe for the reason
       the framebuffer itself is: a band owns the rows y == tid (mod nt) and
       no other band touches them. */
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
    /* The same active-rectangle clear as the depth and coverage buffers
       above, and these two are already conditional on the frame submitting a
       shadow or a translucent polygon at all. */
    /* THE POLYGON-ID BUFFER IS ALSO WHAT THE A/B COMPARES, so it is cleared
       and filled on a frame the A/B is checking even when no shadow volume
       asked for it. With the A/B off -- every run that is not measuring the
       graphics-card renderer -- want_id is have_shadow and nothing about this
       block or the write in the band below has changed. */
    const bool want_id = have_shadow || (g_gpu_opaque && ab_mode());
    if (have_shadow)
        for (int y = 0; y < ch; ++y) std::memset(stencil[y], 0, (size_t)cw);
    if (want_id) {
        /* 0 is the clear plane's polygon ID (CLEAR_COLOR bits 24-29 reset
           value); pixels no opaque polygon reaches keep it. */
        for (int y = 0; y < ch; ++y) std::memset(attrid[y], 0, (size_t)cw);
    }
    if (have_translucent)
        for (int y = 0; y < ch; ++y) std::memset(tlattr[y], 0, (size_t)cw);

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
    if (filt == 2) {
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

    /* One row band. tid picks the rows: tid, tid+nt, tid+2nt...
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
    auto band_impl = [&](int tid, int nt, auto ftag) {
    /* THE SAMPLER'S MODE IS A COMPILE-TIME CONSTANT IN HERE, which is the
       whole point of the shape (run hd2). The body below is instantiated once
       per mode and the mode is chosen once per band, so the nearest body --
       the one every run with the key absent takes -- carries no test for a
       filter, no branch per pixel and no call through a pointer: it is the
       instruction stream it was before filtering existed. */
    constexpr int FILTER = decltype(ftag)::value;
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
        const int px0 = present_x(), py0 = present_y();
        const int px1 = px0 + present_w() - 1, py1 = py0 + present_h() - 1;
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
                    }
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
    };  // band_impl

    /* ONE BAND ENTRY, THREE BODIES BEHIND IT. The switch runs once per band
       per frame -- at most eight times a frame -- and hands the raster a body
       with the filter mode already resolved. The pool below still sees a
       plain two-argument callable, so nothing about the threading changed. */
    auto band = [&](int tid, int nt) {
        switch (filt) {
        case 1:
            band_impl(tid, nt, std::integral_constant<int, 1>{});
            break;
        case 2:
            band_impl(tid, nt, std::integral_constant<int, 2>{});
            break;
        default:
            band_impl(tid, nt, std::integral_constant<int, 0>{});
            break;
        }
    };

    /* Small scenes (the smokes, a single model) are not worth waking anyone
       up for; the handover costs more than the fill. */
    const int nt = (g.tris.size() < 256) ? 1 : raster_threads();
    /* the band's own type, named out here rather than inside the lambda below:
       decltype of a captured name inside a lambda body is a reference type and
       there is no pointer to a reference */
    typedef decltype(band) B;
    auto run_passes = [&](int lo, int hi) {
        pass_lo = lo;
        pass_hi = hi;
        if (nt <= 1) {
            band(0, 1);
        } else {
            pool(nt).run([](void *p, int tid, int n) { (*static_cast<B *>(p))(tid, n); },
                         &band);
        }
    };

    /* ---- THE SEAM: THE OPAQUE PASS, MAYBE ON A GRAPHICS CARD (run hd2) ----
       With nothing registered -- every run with the "Renderer" key absent --
       this is one test against a null pointer and the line below runs both
       passes exactly as it always has. */
    int gpu_drew = 0;
    if (g_gpu_opaque) {
        GxGpuFrame f;
        std::memset(&f, 0, sizeof f);
        f.tris = g.tris.data();
        f.count = g.tris.size();
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
        f.want_attrid = want_id ? 1 : 0;
        /* THE DEPTH ONLY HAS TO COME BACK IF SOMETHING IS GOING TO READ IT,
           and at 4x that readback is three megabytes a frame. The translucent
           and shadow pass is the only reader, so a frame with neither skips
           it. The A/B reads it too, and says so. */
        f.want_depth = (have_translucent || have_shadow || ab_mode()) ? 1 : 0;
        f.tex_filter = filt;
        f.tex_generation = g_tex_generation;
        /* The colour a pixel this pass does not reach keeps. Taken from the
           picture rather than assumed: the caller clears the framebuffer
           before calling, and what it clears to is its business. */
        f.clear_argb = (cw > 0 && ch > 0) ? fb.px[f.py0][f.px0] : 0xFF000000u;

        if (!ab_mode()) {
            gpu_drew = g_gpu_opaque(&f) ? 1 : 0;
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

            run_passes(0, 0);                       /* arm B: this file */
            for (int y = y0; y < y0 + h; ++y) {
                const size_t o = (size_t)y * SCREEN_W + x0;
                std::memcpy(&g_ab_fb[o], &fb.px[y][x0], (size_t)w * sizeof(uint32_t));
                std::memcpy(&g_ab_dep[o], &g_depth[y][x0], (size_t)w * sizeof(float));
                std::memcpy(&g_ab_cov[o], &g_cover[y][x0], (size_t)w);
                std::memcpy(&g_ab_id[o], &g_attrid[y][x0], (size_t)w);
            }
            /* put the buffers back exactly as the clears left them */
            for (int y = y0; y < y0 + h; ++y) {
                std::memcpy(&fb.px[y][x0], &g_ab_save[(size_t)y * SCREEN_W + x0],
                            (size_t)w * sizeof(uint32_t));
                for (int x = x0; x < x0 + w; ++x) g_depth[y][x] = 1e30f;
                std::memset(&g_cover[y][x0], 0, (size_t)w);
                std::memset(&g_attrid[y][x0], 0, (size_t)w);
                if (have_translucent) std::memset(&g_tlattr[y][x0], 0, (size_t)w);
            }

            gpu_drew = g_gpu_opaque(&f) ? 1 : 0;    /* arm A: the card */

            static int abf;
            const int frame = abf++;
            if (gpu_drew && (frame % g_ab_every) == 0) {
                unsigned long long inter = 0, uni = 0, both = 0, interior = 0;
                unsigned long long out = 0, idbad = 0;
                double colsum = 0.0, depsum = 0.0, depmax = 0.0;
                double depmax_in = 0.0;
                for (int y = y0; y < y0 + h; ++y) {
                    for (int x = x0; x < x0 + w; ++x) {
                        const size_t o = (size_t)y * SCREEN_W + x;
                        const int ca = g_cover[y][x], cb = g_ab_cov[o];
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
                        ++interior;
                        if (worst > g_ab_tol) ++out;
                        if (ad > depmax_in) depmax_in = ad;
                        if (g_attrid[y][x] != g_ab_id[o]) ++idbad;
                    }
                }
                const double iou = uni ? (double)inter / (double)uni : 1.0;
                const double mean = both ? colsum / (double)(both * 3) : 0.0;
                const double oshare =
                    interior ? (double)out / (double)interior : 0.0;
                std::fprintf(stderr,
                             "[renderer-ab] frame %d cover IoU %.6f both %llu "
                             "interior %llu colour mean %.4f outliers %llu "
                             "(%.6f) depth interior max %.3e all max %.3e "
                             "mean %.3e id wrong %llu\n",
                             frame, iou, both, interior, mean, out, oshare,
                             depmax_in, depmax,
                             both ? depsum / (double)both : 0.0, idbad);
                std::fflush(stderr);
                ++g_ab_frames;
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
                }
            }
        }
    }

    run_passes(gpu_drew ? 1 : 0, 1);

    /* EDGE SMOOTHING, LAST, AND STILL INSIDE gx_render (run hd2). Here rather
       than at present time because here is the only moment the framebuffer
       holds the 3D picture and nothing else: hal/message_compositor.cpp puts
       engine A's 2D layers over it, hal/sub_screen.cpp drops the bottom-screen
       panel in and walk_window applies the fade, and all three run after this
       function returns. So a pass that runs here cannot read a HUD pixel,
       cannot write one, and needs no list of regions to avoid.
       Inside the timed section deliberately: it is part of what a frame costs
       when the setting is on, and the perf line should say so. */
    aa_pass(fb, cw, ch, nt);
    aa_report();

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

