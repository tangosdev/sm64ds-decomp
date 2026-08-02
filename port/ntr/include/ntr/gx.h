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
};

// Feed the engine. `gx_write_port` takes a geometry-port address in
// 0x4000440..0x40005FF; `gx_write_fifo` takes a word pushed at 0x4000400.
void gx_write_port(uint32_t addr, uint32_t value);
void gx_write_fifo(uint32_t word);

// Reset the command stream, matrix stacks and polygon list.
void gx_reset();

// Bind a decoded texture for subsequent geometry. Pass null for untextured.
void gx_bind_texture(const uint32_t *rgba, int width, int height);

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

}  // namespace ntr

#endif  // NTR_GX_H
