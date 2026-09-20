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
float g_mtx_scale = 1.0f;    // the uniform scale, local units -> view units

void mtx_gen_update() {
    const Mat &P = g.pos;
    if (std::memcmp(&g_mtx_seen_pos, &P, sizeof(Mat)) == 0 &&
        std::memcmp(&g_mtx_seen_vec, &g.vec, sizeof(Mat)) == 0)
        return;
    g_mtx_seen_pos = P;
    g_mtx_seen_vec = g.vec;
    ++g_mtx_gen;
    g_mtx_similar = 0;
    g_mtx_scale = 1.0f;

    /* mul(v, m) forms out_j = sum_i v_i * m[4i + j], so the image of basis
       vector e_i is the row (m[4i], m[4i+1], m[4i+2]). A similarity is three
       mutually orthogonal images of equal length. */
    const float *m = P.m;
    float u[3][3];
    for (int i = 0; i < 3; ++i)
        for (int k = 0; k < 3; ++k) u[i][k] = m[i * 4 + k];
    const float l0 = u[0][0]*u[0][0] + u[0][1]*u[0][1] + u[0][2]*u[0][2];
    const float l1 = u[1][0]*u[1][0] + u[1][1]*u[1][1] + u[1][2]*u[1][2];
    const float l2 = u[2][0]*u[2][0] + u[2][1]*u[2][1] + u[2][2]*u[2][2];
    if (l0 <= 1e-12f) return;
    const float eps = 1e-4f;                 // relative, on the squared length
    if (std::fabs(l1 - l0) > eps * l0) return;
    if (std::fabs(l2 - l0) > eps * l0) return;
    const float d01 = u[0][0]*u[1][0] + u[0][1]*u[1][1] + u[0][2]*u[1][2];
    const float d02 = u[0][0]*u[2][0] + u[0][1]*u[2][1] + u[0][2]*u[2][2];
    const float d12 = u[1][0]*u[2][0] + u[1][1]*u[2][1] + u[1][2]*u[2][2];
    if (std::fabs(d01) > eps * l0) return;
    if (std::fabs(d02) > eps * l0) return;
    if (std::fabs(d12) > eps * l0) return;

    /* And the VECTOR matrix has to carry the same rotation, or the normals
       the live path lights with are not the normals a stored patch was
       curved by. Same numbers is the sufficient condition and the one the
       engine actually produces; anything else stands down. */
    const float *w = g.vec.m;
    for (int i = 0; i < 3; ++i)
        for (int k = 0; k < 3; ++k) {
            const float a = m[i * 4 + k], b = w[i * 4 + k];
            if (std::fabs(a - b) > 1e-4f * (1.0f + std::fabs(a))) return;
        }

    g_mtx_scale = std::sqrt(l0);
    g_mtx_similar = 1;
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
    if (ra.mgen != rb.mgen || rb.mgen != rc.mgen ||
        ra.ngen != ra.mgen || rb.ngen != rb.mgen || rc.ngen != rc.mgen ||
        rc.mgen != g_mtx_gen) {
        smooth_store_count(SMOOTH_STORE_CROSSMTX, 1);
        return false;
    }
    if (!g_mtx_similar) {
        smooth_store_count(SMOOTH_STORE_NONSIM, 1);
        return false;
    }
    return true;
}

/* THE STORED PATH: look the shape up by the triangle's raw input, build it
   once if it is new, and replay it through the matrix that is live now. */
int smooth_try_store(const SmoothVertex s[3], const GxRaw &ra, const GxRaw &rb,
                     const GxRaw &rc, const SmoothPolicy &pol, int prof) {
    SmoothKey key;
    const GxRaw *r[3] = {&ra, &rb, &rc};
    for (int i = 0; i < 3; ++i) {
        key.p[i][0] = r[i]->x; key.p[i][1] = r[i]->y; key.p[i][2] = r[i]->z;
        key.n[i] = r[i]->nrm;
    }
    key.level = static_cast<uint32_t>(pol.level);

    const long long t_pol = prof ? smooth_prof_ticks() : 0;
    const SmoothEntry *e = smooth_store_find(key);
    if (!e) {
        /* THE ONCE. Build the corners in the model's own space out of the
           display list's own numbers, the same FX12 scaling project() puts
           on a VTX coordinate and the raw NORMAL payload, measure the shape
           and tessellate it. Texel coordinates and colour are deliberately
           left at zero: they are live state, and the replay interpolates
           the current ones at the same weights. */
        SmoothVertex l[3];
        for (int i = 0; i < 3; ++i) {
            l[i].x = r[i]->x * FX12;
            l[i].y = r[i]->y * FX12;
            l[i].z = r[i]->z * FX12;
            raw_normal(r[i]->nrm, l[i].nx, l[i].ny, l[i].nz);
            l[i].u = 0.0f; l[i].v = 0.0f; l[i].color = 0;
        }
        SmoothShape sh;
        smooth_shape(l[0], l[1], l[2], pol, sh);
        float pts[SMOOTH_MAX_GRID * 3];
        const int tf_build =
            (!sh.no_normal && sh.curved) ? (1 << pol.level) : 1;
        if (tf_build > 1)
            smooth_grid_positions(l[0], l[1], l[2], tf_build, pts);
        e = smooth_store_add(key, tf_build, sh, tf_build > 1 ? pts : 0);
        if (!e) return 0;   /* the store refused outright: emit it unchanged */
    }

    /* The two caps are LENGTHS and the shape was measured in local units, so
       they are compared against it at the matrix's own scale. The rest of
       the verdict, no-normal and flat, is scale-free and was settled when
       the shape was measured. */
    int why = SMOOTH_WHY_OK;
    const int tf = smooth_shape_factor(e->shape, pol, g_mtx_scale, &why);
    if (prof) smooth_prof_add(SMOOTH_PROF_POLICY,
                              smooth_prof_ticks() - t_pol, 1);
    if (smooth_census_on()) smooth_census_tri(s[0], s[1], s[2], tf);
    if (tf <= 1) {
        smooth_counters_for(why);
        return 0;
    }

    smooth_count(SMOOTH_COUNT_SUBDIVIDED, 1);
    const float *grid = smooth_store_grid(e);
    if (!grid) return 0;

    if (smooth_abdiff_on()) {
        /* The audit arm: build the patch the old way as well and measure how
           far the two land apart. In exact arithmetic they are the same
           patch; this is the number that says how close the machine gets. */
        float liveg[SMOOTH_MAX_GRID * 3];
        SmoothShape lsh;
        smooth_shape(s[0], s[1], s[2], pol, lsh);
        int lwhy = SMOOTH_WHY_OK;
        const int ltf = smooth_shape_factor(lsh, pol, 1.0f, &lwhy);
        float worst = 0.0f;
        if (ltf == tf) {
            smooth_grid_positions(s[0], s[1], s[2], tf, liveg);
            const int npts = smooth_grid_points(tf);
            for (int i = 0; i < npts; ++i) {
                const Vec4 lv{grid[i * 3], grid[i * 3 + 1], grid[i * 3 + 2],
                              1.0f};
                const Vec4 w = mul(lv, g.pos);
                const float dx = w.x - liveg[i * 3];
                const float dy = w.y - liveg[i * 3 + 1];
                const float dz = w.z - liveg[i * 3 + 2];
                const float d = std::sqrt(dx * dx + dy * dy + dz * dz);
                if (d > worst) worst = d;
            }
        }
        smooth_abdiff_add(ltf != tf, worst, lsh.longest);
    }

    const long long t_sub = prof ? smooth_prof_ticks() : 0;
    smooth_emit_grid(grid, tf, s, &g.pos);
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

    /* THE STORED PATH, which is what a default run takes. */
    if (!smooth_live_mode() && store_eligible(ra, rb, rc))
        return smooth_try_store(s, ra, rb, rc, pol, prof);

    /* AND THE PATH FOR EVERYTHING ELSE: a bone joint, a matrix that is not a
       similarity, or the A/B switch. The patch is rebuilt from the
       view-space corners, every frame, exactly as 0.4.0 did it. */
    smooth_store_count(SMOOTH_STORE_LIVE, 1);
    /* Say WHY, so the measurement table can separate "the feature did nothing
       because the scene is flat" from "the caps are too tight". */
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
    const int prof = smooth_prof_on();
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
void push_vertex(const GxVertex &v, const GxRaw &r) {
    g.strip.push_back(v);
    g.strip_raw.push_back(r);
    const size_t n = g.strip.size();
    const GxVertex *s = g.strip.empty() ? 0 : &g.strip[0];
    const GxRaw *q = g.strip_raw.empty() ? 0 : &g.strip_raw[0];
    switch (g.prim) {
        case 0:                                        // separate triangles
            if (n == 3) {
                emit_tri(s[0], s[1], s[2], q[0], q[1], q[2]);
                g.strip.clear();
                g.strip_raw.clear();
            }
            break;
        case 1:                                        // separate quads
            if (n == 4) {
                emit_tri(s[0], s[1], s[2], q[0], q[1], q[2]);
                emit_tri(s[0], s[2], s[3], q[0], q[2], q[3]);
                g.strip.clear();
                g.strip_raw.clear();
            }
            break;
        case 2:                                        // triangle strip
            if (n >= 3) {
                const size_t i0 = n - 3, i1 = n - 2, i2 = n - 1;
                if ((n - 3) & 1)                        // alternate winding
                    emit_tri(s[i1], s[i0], s[i2], q[i1], q[i0], q[i2]);
                else
                    emit_tri(s[i0], s[i1], s[i2], q[i0], q[i1], q[i2]);
            }
            break;
        case 3:                                        // quad strip
            if (n >= 4 && (n % 2) == 0) {
                const size_t i0 = n - 4, i1 = n - 3, i2 = n - 2, i3 = n - 1;
                emit_tri(s[i0], s[i1], s[i3], q[i0], q[i1], q[i3]);
                emit_tri(s[i0], s[i3], s[i2], q[i0], q[i3], q[i2]);
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
    GxRaw r;
    if (smooth_level() > 0) {
        mtx_gen_update();
        r.x = x; r.y = y; r.z = z;
        r.nrm = g.nrm_raw;
        r.mgen = g_mtx_gen;
        r.ngen = g.nrm_gen;
        r.has_normal = (uint8_t)(g.normal_live ? 1 : 0);
    } else {
        r.x = r.y = r.z = 0;
        r.nrm = 0;
        r.mgen = 0;
        r.ngen = 1;
        r.has_normal = 0;
    }
    push_vertex(project(x, y, z), r);
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
            g.strip_parity = 0;
            break;
        case 0x41: g.prim = -1; g.strip.clear(); break;          // END_VTXS
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
};
std::map<TexKey, TexEntry> g_vram_tex_cache;

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
void gx_invalidate_textures() { g_vram_tex_cache.clear(); }

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

void gx_render(Framebuffer &fb) {
    const int tm = frame_ms();
    std::chrono::steady_clock::time_point t_enter;
    if (tm) t_enter = std::chrono::steady_clock::now();
    tri_report();
    texpx_report();
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
    if (have_shadow) {
        for (int y = 0; y < ch; ++y) std::memset(stencil[y], 0, (size_t)cw);
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
                    const int ui = tex_coord(uu * tsc, t.tw, rep_s, flip_s);
                    const int vi = tex_coord(vv * tsc, t.th, rep_t, flip_t);
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

