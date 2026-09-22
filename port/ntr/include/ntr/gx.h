// DS geometry engine.
//
// Unlike the 2D engines, 3D is not readable register state -- it is a *stream*.
// Writing a geometry port or pushing a word into the FIFO at 0x4000400 executes a
// command, and the result exists only as the polygons it emitted. A host layer
// that only mirrors registers renders nothing here, which is why this is the one
// part of the port that cannot be done by mapping memory.
//
// SM64DS drives it both ways, so both are implemented:
//
//     *(volatile u32 *)0x4000400 = 0x1a;      // packed command word
//     Copy36Bytes(m, (int *)0x4000400);       // ...then its 9 parameters
//
//     DMAStartTransfer(ch, src, 0x4000400, 0x84400000 | (len >> 2));   // display list
//
// See notes/assessment.md section 2a and docs/mmio-inventory.md.

#ifndef NTR_GX_H
#define NTR_GX_H

#include <stdint.h>

#include "ntr/ppu.h"

namespace ntr {

// A vertex after the modelview+projection transform, in screen space.
struct GxVertex {
    float x, y, z, w;      // x/y in pixels, z in [0,1] after divide
    float u, v;            // texel coordinates (DS TEXCOORD is 1.4 fixed point)
    uint32_t color;        // 0xAARRGGBB

    // --- appended for the model smoother; see ntr/smooth.h ------------------
    // THE VIEW-SPACE VERTEX, carried alongside the clip-space one above.
    // "View space" here is after the DS POSITION matrix and before the
    // projection. It is the last space in which a curved patch can be built
    // linearly: the perspective divide and the near clip both come after it,
    // and subdividing past either of them bends straight edges by the near
    // plane's distance. vw is the fourth component the position matrix
    // produced -- 1 for every affine matrix the game loads, and the smoother
    // refuses a triangle where it is not.
    float vx, vy, vz, vw;
    // The NORMAL command's own vector after the VECTOR matrix (which is the
    // position matrix without translation, so it lands in the same space as
    // the position above), unit length. (0,0,0) when the polygon carried no
    // NORMAL, which is how an unlit polygon says it has no surface to curve.
    float nx, ny, nz;
};

// The bound texture travels with the triangle. Material state is set by the
// game's C code around each display list, not inside it, so a triangle's
// texture is whatever was bound when it was assembled.
struct GxTriangle {
    GxVertex v[3];
    const uint32_t *tex;   // decoded RGBA, or null for untextured
    int tw, th;
    // HOST PIXELS PER DS TEXEL, and it is 1 for every texture the ROM itself
    // supplies. A replacement image from an HD pack (ntr/hdtex.h) is a whole
    // multiple of the DS texture's own size, and this is that multiple: the
    // sampler multiplies u and v by it, and tw/th are then the ACTUAL pixel
    // dimensions of the bound buffer rather than the DS texture's. So the
    // wrap, clamp and flip rules in tex_coord keep working on the real
    // buffer, a 4x image of a 32x32 texture binds as 128x128 with a 4 here,
    // and at 1 -- which is what a run with no pack has everywhere -- the
    // multiply is by exactly 1.0f and the raster is bit-for-bit the
    // arithmetic it did before this field existed.
    uint8_t tex_scale;
    // WHICH TEXTURE THIS IS, as a number that is never handed out twice
    // (run hd2, lane GPU2). The optional graphics-card renderer keeps its own
    // copy of every texture it has drawn with, and it cannot key that copy on
    // `tex` above: the decode cache frees its buffers and the allocator hands
    // the same address back for different pixels, so a pointer key serves a
    // stale picture. This id is handed out at bind time, counts up and is
    // never reused; gx_invalidate_textures bumps gx_texture_generation() so a
    // backend throws its whole cache away at a scene change. 0 means
    // untextured. It costs one map lookup per bind and only when a backend is
    // registered, so an ordinary run does not pay for it.
    uint32_t tex_id;
    uint8_t cull;          // POLYGON_ATTR bits 6-7: 1 back, 2 front, 3 both
    uint8_t alpha;         // POLYGON_ATTR bits 16-20 (0..31; 31 = opaque)
    // TEXIMAGE_PARAM bits 16-19 as bit0 repeat S, bit1 repeat T, bit2 flip S,
    // bit3 flip T. A cleared repeat bit is CLAMP, not wrap.
    uint8_t wrap;
    // The hardware's own render-order class: attribute alpha 1..30 or an
    // A3I5/A5I3 texture puts the polygon in the translucent pass, which
    // draws after every opaque polygon regardless of submission order.
    // Mode-3 (shadow) polygons ride this pass too, whatever their alpha:
    // their stencil protocol needs the depth buffer already final.
    uint8_t translucent;
    // POLYGON_ATTR bits 4-5: 0 modulation, 1 decal, 2 toon/highlight,
    // 3 shadow. Only 3 changes the raster's behaviour (GBATEK shadow
    // polygons); 1 and 2 draw as modulation, same as before they existed.
    uint8_t mode;
    // POLYGON_ATTR bits 24-29. For mode-3 polygons the ID selects the role:
    // 0 is the stencil mask, nonzero is the drawn shadow. For everything
    // else it is recorded per pixel so a shadow can refuse to fall on its
    // own caster (equal IDs do not shadow).
    uint8_t polyid;
    uint32_t dbg_tex;      // TEXIMAGE_PARAM that was bound (diagnostics only)
};

// Feed the engine. `gx_write_port` takes a geometry-port address in
// 0x4000440..0x40005FF; `gx_write_fifo` takes a word pushed at 0x4000400.
void gx_write_port(uint32_t addr, uint32_t value);
void gx_write_fifo(uint32_t word);

// Reset the command stream, matrix stacks and polygon list.
void gx_reset();

// Drop every VRAM-decoded texture. The decode cache deliberately survives
// gx_reset (which runs once a frame); call this when the texture slots are
// being reused for different content, as the model soaks do per model.
void gx_invalidate_textures();

// Bind a decoded texture for subsequent geometry. Pass null for untextured.
void gx_bind_texture(const uint32_t *rgba, int width, int height);

// HOW A TEXEL IS CHOSEN (run hd2, the "TextureFilter" setting). 0 is nearest,
// the sampler this renderer has always used and the DS's own behaviour; 1 is
// bilinear; 2 is trilinear, which adds a chain of halved copies of each
// texture and blends the two sizes nearest the surface's distance.
//
// CALL IT ONCE, AT BOOT, BEFORE ANY GEOMETRY. The mode decides whether the
// texture cache builds a mip chain as each texture enters it, so a run that
// changed it half way would have chains for some textures and not others. At
// 0 no chain is built, no extra memory is held, and the raster runs the same
// instructions it ran before this existed: the mode is resolved once per
// frame into one of three whole raster bodies, never as a test per pixel.
void gx_configure_texture_filter(int mode);
int gx_texture_filter();

// EDGE SMOOTHING OVER THE FINISHED 3D PICTURE (run hd2, the "AntiAliasing"
// setting). 0 is off and is no pass at all; 1 runs a contrast-edge filter
// over the picture at the END of gx_render -- before any 2D layer is
// composited over it, so text, the HUD and the touch-screen art are never
// read and never written.
//
// Call it once at boot, beside gx_configure_texture_filter: the pass keeps a
// scratch copy of the picture, allocated on first use and never freed.
void gx_configure_anti_aliasing(int mode);
int gx_anti_aliasing();

// What the smoothing pass has done since the program started: how many pixels
// it rewrote and how many frames it ran on. The A/B evidence that a key-absent
// run does no work at all. (It used to carry a third number, the frames the
// pass refused to run on because the display capture unit was already armed.
// That mechanism is gone -- see gx_aa_preimage below -- and the pass now runs
// on every frame, so the number could only ever have been zero.)
void gx_aa_counters(unsigned long long &changed, unsigned long long &frames);

// THE FRAME AS IT WAS BEFORE THE SMOOTHING PASS, or null when there is no such
// frame (the setting is off, or gx_render has not finished one this frame).
// Same SCREEN_W stride as the framebuffer, so an index into one indexes the
// other.
//
// THIS IS WHAT KEEPS THE GAME FROM NOTICING THE SETTING, and it replaced a
// weaker rule that measurement broke. SM64DS reads its own top screen back
// through the DS display capture unit for the dual-screen minigames, and the
// frame it arms that unit on is NOT always one the raster has already drawn:
// on a running minigame the arm lands after gx_render, so refusing to smooth
// an already-armed frame missed five hundred captured frames out of twelve
// hundred. Instead the pass keeps the picture it started from,
// hal/message_compositor.cpp mirrors its own writes into it so it stays a
// finished frame, and ntr::ppu_display_capture reads it in place of the live
// framebuffer. The game gets the setting-off picture whenever it asks, with no
// assumption about when it asks.
uint32_t *gx_aa_preimage();

// The game path: TEXIMAGE_PARAM / PLTT_BASE writes (ports 0x2A/0x2B) bind by
// decoding lazily out of the mapped VRAM texture/palette slot windows.
void gx_teximage_param(uint32_t value);
void gx_pltt_base(uint32_t value);

// Diagnostic stream hash: every word entering the engine since the last
// reset, so smokes can separate "different stream" from "ignored stream".
uint32_t gx_stream_hash_reset();
uint32_t gx_state_hash();  // pos matrix + first slots
int gx_store_count_reset();

// Preload a position-matrix stack slot. Display lists open with MTX_RESTORE
// against slots the *scene* filled in beforehand, so a model drawn on its own
// has to have them set up first. Row-major, row-vector convention.
void gx_set_matrix_slot(int slot, const float m[16]);

// Lighting. The DS computes a vertex's colour when it sees NORMAL, out of the
// current material and the four lights -- so lighting is not a rasteriser stage
// here, it happens during command decode, exactly as on hardware.
//
// Display lists carry NORMAL but never DIF_AMB or LIGHT_*: the material and the
// scene set those. Bind them before submitting geometry.
void gx_set_material(uint32_t dif_amb, uint32_t spe_emi);
void gx_set_light(int index, float dx, float dy, float dz, uint32_t bgr555);
void gx_enable_lights(uint32_t mask);           // bits 0..3

// Polygons emitted since the last gx_reset / SWAP_BUFFERS.
const GxTriangle *gx_polygons(size_t &count);

// Bumped every time gx_invalidate_textures throws the decode cache away, so a
// backend holding copies of those textures knows to drop them.
uint32_t gx_texture_generation();

// ---- THE OPTIONAL GRAPHICS-CARD BACKEND FOR THE OPAQUE PASS (run hd2) -----
//
// gx_render draws a frame in two passes over the same buffers: an OPAQUE pass
// (colour, depth with a LESS test, a per-pixel polygon ID, the coverage mask)
// and then a TRANSLUCENT pass that READS what the opaque pass left. The opaque
// pass is nearly all of the fill, so it is the one worth moving to a card.
//
// A backend is a single function that is handed everything the opaque pass
// needs and fills the CPU buffers below exactly as the software pass would
// have. It returns 1 when it drew the frame and 0 when it did not, and 0 is
// never an error here: gx_render simply runs its own opaque pass instead, on
// buffers nothing has touched. Nothing is registered unless a setting asked
// for it, and with none registered this whole mechanism is one null test per
// frame.
//
// hal/gpu_raster.cpp is the Direct3D 11 one. The software pass stays the
// default and the only byte-exact reference.
struct GxGpuFrame {
    const GxTriangle *tris;
    size_t count;
    // Every buffer below is row-major with the SAME stride, which is the
    // framebuffer's allocation width; a row y starts at base + y * stride.
    uint32_t *fb;          // the framebuffer, 0xAARRGGBB
    float *depth;          // screen-linear z, cleared to 1e30
    uint8_t *cover;        // the 3D coverage mask, cleared to 0
    uint8_t *attrid;       // the per-pixel polygon ID
    int stride;
    int cw, ch;            // the active picture inside the allocation
    int px0, py0, pw, ph;  // the present rectangle, which is the scissor
    int want_attrid;       // the frame has a shadow volume, so the ID is read
    int want_depth;        // a second pass will read the depth back
    int tex_filter;        // 0 nearest, 1 bilinear, 2 trilinear
    uint32_t tex_generation;
    uint32_t clear_argb;   // what the framebuffer held when gx_render started
};
typedef int (*GxGpuOpaqueFn)(const GxGpuFrame *);
void gx_set_gpu_opaque(GxGpuOpaqueFn fn);
int gx_gpu_opaque_registered();
// The registered backend, so a caller can take it out and put it back and
// draw one list both ways.
GxGpuOpaqueFn gx_gpu_opaque();

// Rasterise them into fb with a depth buffer. Does not clear fb -- the 3D layer
// composites over whatever the 2D engine already drew.
void gx_render(Framebuffer &fb);

// THE 3D COVERAGE MASK: one byte per host framebuffer pixel, 1 where the LAST
// gx_render actually wrote a pixel (opaque, translucent or shadow), 0 where it
// left the framebuffer alone. SCREEN_W stride, SCREEN_H rows.
//
// WHY IT EXISTS. On the DS the 3D engine is BG0 in 3D mode, and engine A's 2D
// unit composites the four BGs and the sprites against it BY PRIORITY: a BG
// whose priority number is higher than BG0's sits BEHIND the 3D layer and
// cannot paint over it. The port renders 3D into the framebuffer and then runs
// hal/message_compositor.cpp over the top, so without this mask that
// compositor cannot tell a pixel the 3D engine drew from a pixel the frame's
// clear colour left, and every 2D layer covers 3D unconditionally.
//
// Measured on scene 390 (dScMgFlower_c) before the mask existed: BG2 at
// priority 3 owned 48357 of the top screen's 49152 pixels while the 3D layer
// at priority 1 submitted 878 polygons a frame, so 100% of the minigame's
// flower and Yoshi were drawn and then buried.
//
// VALID ONLY IMMEDIATELY AFTER gx_render. It is cleared at the head of every
// gx_render call, so a reader that runs on a frame the port did not rasterise
// is looking at the previous frame's coverage; both live callers sit directly
// after the gx_render that filled it.
const uint8_t *gx_coverage();

// The polygon-ID plane and the depth plane the same pass fills, on the same
// contract as the coverage mask above: SCREEN_W stride, SCREEN_H rows, valid
// only immediately after gx_render. Read-only, and for the same reason the
// mask is public: a caller that has to ask "which surface won this pixel" or
// "was the depth written" is asking about a value, not about a colour.
const uint8_t *gx_attr_ids();
const float *gx_depth();

void gx_debug_proj(float out[16]);

// The matrix stacks' current depth, for GXSTAT bits 8-12 (position/vector) and
// bit 13 (projection). Read-only; ntr/io.cpp uses it to answer the two linked
// stack-level readers with the host's real state instead of a dead latch.
void gx_matrix_stack_levels(unsigned &pos_level, unsigned &proj_level);

// The live POSITION matrix, row-vector convention like everything else here.
// For a caller that needs to report what its own vertices will be transformed
// by; see the definition for why the projection alone cannot answer it.
void gx_debug_pos(float out[16]);
// the live matrix state, for tracking down a mis-placed draw
void gx_debug_matrices(int *mode, float pos[16], float proj[16]);

// The latched VIEWPORT rectangle (command 0x60 / port 0x4000580), in HOST
// framebuffer pixels -- already scaled off the DS panel's 256x192 by the
// command handler, so at the 2x tier a full-screen DS viewport reads 512x384.
// Read-only, and the answer to "did the game ask for a corner or did the port
// draw one": a wrong rectangle here and a wrong rectangle on screen are the
// same fact, and a right one rules the projection out by name.
//
// `sets` is how many VIEWPORT commands executed since the last gx_reset, and
// it is not optional decoration. gx_reset restores the rectangle to a
// FULL-SCREEN default, so the rectangle alone cannot tell a game-issued
// full-screen viewport from a frame that issued none. Zero sets means the
// rectangle read back is the default and says nothing about the game.
void gx_debug_viewport(int &x, int &y, int &w, int &h, int &sets);

// THE GEOMETRY COMMAND CENSUS. `counts` is indexed by GBATEK command byte and
// holds how many of each EXECUTED since the last take; `ports` and `fifo` are
// how many of them arrived through gx_write_port and gx_write_fifo, which is
// the difference between a caller that reached the register file and one whose
// stores landed in mapped memory; `swap_param` is the last SWAP_BUFFERS
// parameter word; `resets` is gx_reset calls, so a sampler can say how much
// engine state was thrown away between two samples. `take` zeroes the interval
// counters, which makes a per-frame sampler a per-frame census.
//
// The counters live outside the geometry State, so gx_reset does not clear
// them. That is the property the instrument is for.
void gx_debug_commands(uint32_t counts[256], uint32_t &ports, uint32_t &fifo,
                       uint32_t &swap_param, uint32_t &resets, bool take);

}  // namespace ntr

#endif  // NTR_GX_H
