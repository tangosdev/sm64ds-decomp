//cpp
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/gx/g3imm_cmd.h"

/* Fills the unit square func_ov007_020c4684 has scaled onto a sprite with one
 * quad per 8x8-texel cell, top row first. Without a tile map the texture runs
 * on from (s0, t0), each cell advancing repeatU texels across and repeatV
 * down, and every row is its own QUADS list. With one, each cell draws the
 * 8x8 tile its map entry names (counted across a texture texWidth texels
 * wide); 0xffff leaves the cell empty.
 *
 * The cell steps dx and dy are `long`, which is what NitroSDK's fx32 is. This
 * tree's fx32 is an `int`, and here the difference reaches the code: with an
 * int dx the compiler turns the right edge dx * (col + 1) into a second running
 * sum beside the left edge's, and the cartridge multiplies it out per cell. */
void func_ov007_020c49bc(int width, int height, fx32 s0, fx32 t0, int repeatU, int repeatV,
                         u16 *tilemap, int texWidth) {
    int cols = width >> 3;
    int rows = height >> 3;
    long dx = FX_DivS32(0x1000, cols);
    long dy = FX_DivS32(0x1000, rows);

    if (tilemap == 0) {
        for (int row = 0; row < rows; row++) {
            fx32 t = t0 + row * (repeatV << 15);
            fx32 y = dy * (rows - row);

            G3_Begin(GX_BEGIN_QUADS);
            for (int col = 0; col < cols; col++) {
                fx32 s = s0 + col * (repeatU << 15);

                G3_TexCoord(s, t);
                G3_Vtx(dx * col, y, 0);
                G3_TexCoord(s, t + (repeatV << 15));
                G3_VtxYZ(y - dy, 0);
                G3_TexCoord(s + (repeatU << 15), t + (repeatV << 15));
                G3_VtxXZ(dx * (col + 1), 0);
                G3_TexCoord(s + (repeatU << 15), t);
                G3_VtxYZ(y, 0);
            }
            G3_End();
        }
    } else {
        G3_Begin(GX_BEGIN_QUADS);
        for (int row = 0; row < rows; row++) {
            fx32 y = dy * (rows - row);

            for (int col = 0; col < cols; col++) {
                u16 tile = tilemap[row * cols + col];

                if (tile != 0xffff && texWidth > 0) {
                    fx32 s = FX_ModS32(tile, texWidth >> 3) << 15;
                    fx32 t = FX_DivS32(tile, texWidth >> 3) << 15;

                    G3_TexCoord(s, t);
                    G3_Vtx(dx * col, y, 0);
                    G3_TexCoord(s, t + 0x8000);
                    G3_VtxYZ(y - dy, 0);
                    G3_TexCoord(s + 0x8000, t + 0x8000);
                    G3_VtxXZ(dx * (col + 1), 0);
                    G3_TexCoord(s + 0x8000, t);
                    G3_VtxYZ(y, 0);
                }
            }
        }
    }
    G3_End();
}
