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
};

// The bound texture travels with the triangle. Material state is set by the
// game's C code around each display list, not inside it, so a triangle's
// texture is whatever was bound when it was assembled.
struct GxTriangle {
    GxVertex v[3];
    const uint32_t *tex;   // decoded RGBA, or null for untextured
    int tw, th;
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
