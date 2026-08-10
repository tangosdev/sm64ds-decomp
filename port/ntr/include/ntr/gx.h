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

#include <stddef.h>   // size_t (MSVC's STL leaks it transitively; libstdc++/g++ does not)
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
    uint8_t translucent;
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
void gx_debug_proj(float out[16]);
// the live matrix state, for tracking down a mis-placed draw
void gx_debug_matrices(int *mode, float pos[16], float proj[16]);

}  // namespace ntr

#endif  // NTR_GX_H
