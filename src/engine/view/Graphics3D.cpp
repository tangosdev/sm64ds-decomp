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
#include "nitro/gx/gx_vramcnt.h"
#include "nitro/gx/g3x.h"
#include "nitro/hw/registers.h"
#include "nitro/gx/g3imm_cmd.h"
#include "math/MtxStack.h"


extern Matrix4x3 data_0209b3ec;
 /* the view matrix */
extern Matrix4x3 data_0209b41c;
 /* its inverse */
extern Matrix4x3 IDENTITY_MATRIX4X3;
void func_02045d9c(void);


#include "nitro/gx/g3_util.h"



// Bring up the geometry engine with the game's default 3D state.
void Initialise3dGraphics(int arg) {
    G3X_Init();
    G3X_InitTable();
    G3X_InitMtxStack();
    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(true);
    G3X_AlphaBlend(true);
    G3X_EdgeMarking(false);
    G3X_SetClearColor(0, arg, 0x7fff, 0x3f, false);
    G3_ViewPort(0, 0, 255, 191);
    G3i_PerspectiveW_(0x579, 0xf09, 0x1555, 0x1000, 0x3e8000, 0x1000, 1, 0);
    G3_StoreMtx(0);
    G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
}

// Full 3D setup: engine state, texture banks, an identity view.
void func_0200f4f4(void) {
    Initialise3dGraphics(0);
    GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_01234_EF);
    func_02045d9c();
    data_0209b3ec = IDENTITY_MATRIX4X3;
    MTX_Inverse43(&data_0209b3ec, &data_0209b41c);
}

// Reset the matrix stacks: identity texture matrix, then back to
// position-vector mode.
void func_0200f4b4(void) {
    G3X_Reset();
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}


// Copy the camera matrix into the live view, then invert it.
void CopyToViewMat(const Matrix4x3 *src) {
    data_0209b3ec = *src;
    MTX_Inverse43(&data_0209b3ec, &data_0209b41c);
}

/* OS_Terminate-screen and title font. 0x2000 bytes; the tail is zero. */
extern u8 data_0208a178[0x2000];
