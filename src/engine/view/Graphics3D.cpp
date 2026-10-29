//cpp
/* 3D engine setup and the view matrix, .text 0x0200f470..0x0200f658.
 * Initialise3dGraphics brings the geometry engine up (display control,
 * clear colour, viewport, the default perspective), func_0200f4f4 adds the
 * texture banks and an identity view, func_0200f4b4 resets the matrix
 * stacks, and CopyToViewMat installs a camera matrix as the view.
 *
 * mwccarm emits .text in reverse source order, so the file reads from the
 * top of the run down.
 *
 * leftovers:
 * - data_0209b3ec is the view matrix and data_0209b41c its inverse; both
 *   stay linker names, and neither is claimed here.
 */
#include "common.h"
#include "G3X.h"
#include "GX.h"

#define REG_DISP3DCNT (*(volatile u16 *)0x04000060)
#define REG_MTX_MODE (*(volatile u32 *)0x04000440)
#define REG_MTX_STORE (*(volatile u32 *)0x0400044c)
#define REG_MTX_IDENTITY (*(volatile u32 *)0x04000454)
#define REG_SWAP_BUFFERS (*(volatile u32 *)0x04000540)
#define REG_VIEWPORT (*(volatile u32 *)0x04000580)

#define MTX_MODE_POSITION_VECTOR 2
#define MTX_MODE_TEXTURE 3


extern "C" {
extern Matrix4x3 data_0209b3ec; /* the view matrix */
extern Matrix4x3 data_0209b41c; /* its inverse */
extern Matrix4x3 IDENTITY_MATRIX4X3;
void InvMat4x3(Matrix4x3 *dst, Matrix4x3 *inv);
void func_0205583c(void);
void func_020554bc(void);
void func_020556d0(void);
void func_020557b4(void);
void func_02045d9c(void);
}

struct G3i {
    static void PerspectiveW_(Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, Fix12i, int, Matrix4x3 *);
};

extern "C" {

// @symbol Initialise3dGraphics
// Bring up the geometry engine with the game's default 3D state.
void Initialise3dGraphics(int arg) {
    func_0205583c();
    func_020554bc();
    func_020556d0();
    REG_DISP3DCNT &= ~0x3002;
    REG_DISP3DCNT = (REG_DISP3DCNT & ~0x3000) | 0x10;
    REG_DISP3DCNT = (REG_DISP3DCNT & ~0x3000) | 8;
    REG_DISP3DCNT &= 0xcfdf;
    G3X::SetClearColor(0, arg, 0x7fff, 0x3f, false);
    REG_VIEWPORT = 0xbfff0000;
    G3i::PerspectiveW_(0x579, 0xf09, 0x1555, 0x1000, 0x3e8000, 0x1000, 1, 0);
    REG_MTX_STORE = 0;
    REG_SWAP_BUFFERS = 3;
}

// @symbol func_0200f4f4
// Full 3D setup: engine state, texture banks, an identity view.
void func_0200f4f4(void) {
    Initialise3dGraphics(0);
    GX::SetBankForTex(3);
    GX::SetBankForTexPltt(0x30);
    func_02045d9c();
    data_0209b3ec = IDENTITY_MATRIX4X3;
    InvMat4x3(&data_0209b3ec, &data_0209b41c);
}

// @symbol func_0200f4b4
// Reset the matrix stacks: identity texture matrix, then back to
// position-vector mode.
void func_0200f4b4(void) {
    func_020557b4();
    REG_MTX_MODE = MTX_MODE_TEXTURE;
    REG_MTX_IDENTITY = 0;
    REG_MTX_MODE = MTX_MODE_POSITION_VECTOR;
}
}

// @symbol _Z13CopyToViewMatPK9Matrix4x3
// Copy the camera matrix into the live view, then invert it.
void CopyToViewMat(const Matrix4x3 *src) {
    data_0209b3ec = *src;
    InvMat4x3(&data_0209b3ec, &data_0209b41c);
}
