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
#include "nitro/gx/g3x.h"
#include "nitro/hw/registers.h"

/* NitroSDK's matrix-mode, sort-mode and buffer-mode enums (g3.h) and the
 * immediate-mode commands this file issues (g3imm.h). local copy: an inline
 * body in include/nitro/gx/g3imm.h renumbers ov075/d_s_entry_entry's pinned
 * @NNN (see src/unnamed/ov007/func_ov007_020c4684.c). */
typedef enum {
    GX_MTXMODE_PROJECTION = 0,
    GX_MTXMODE_POSITION = 1,
    GX_MTXMODE_POSITION_VECTOR = 2,
    GX_MTXMODE_TEXTURE = 3
} GXMtxMode;

typedef enum { GX_SORTMODE_AUTO = 0, GX_SORTMODE_MANUAL = 1 } GXSortMode;
typedef enum { GX_BUFFERMODE_Z = 0, GX_BUFFERMODE_W = 1 } GXBufferMode;

#define GX_PACK_SWAPBUFFERS_PARAM(am, zw) ((u32)((am) | ((zw) << 1)))
#define GX_PACK_VIEWPORT_PARAM(x1, y1, x2, y2)     ((u32)((x1) | ((y1) << 8) | ((x2) << 16) | ((y2) << 24)))

static inline void G3_MtxMode(GXMtxMode mode) {
    REG_MTX_MODE = (u32)mode;
}

static inline void G3_StoreMtx(int num) {
    REG_MTX_STORE = (u32)num;
}

static inline void G3_Identity(void) {
    REG_MTX_IDENTITY = 0;
}

static inline void G3_SwapBuffers(GXSortMode am, GXBufferMode zw) {
    REG_SWAP_BUFFERS = GX_PACK_SWAPBUFFERS_PARAM(am, zw);
}

static inline void G3_ViewPort(int x1, int y1, int x2, int y2) {
    REG_VIEWPORT = GX_PACK_VIEWPORT_PARAM(x1, y1, x2, y2);
}

extern "C" {
extern Matrix4x3 data_0209b3ec; /* the view matrix */
extern Matrix4x3 data_0209b41c; /* its inverse */
extern Matrix4x3 IDENTITY_MATRIX4X3;
int InvMat4x3(const Matrix4x3 *src, Matrix4x3 *dst);
void func_0205583c(void);
void func_02045d9c(void);
}

#include "G3i.h"

extern "C" {

// Bring up the geometry engine with the game's default 3D state.
void Initialise3dGraphics(int arg) {
    func_0205583c();
    G3X::InitTable();
    G3X::InitMtxStack();
    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(true);
    G3X_AlphaBlend(true);
    G3X_EdgeMarking(false);
    G3X::SetClearColor(0, arg, 0x7fff, 0x3f, false);
    G3_ViewPort(0, 0, 255, 191);
    G3i::PerspectiveW_(0x579, 0xf09, 0x1555, 0x1000, 0x3e8000, 0x1000, 1, 0);
    G3_StoreMtx(0);
    G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_W);
}

// Full 3D setup: engine state, texture banks, an identity view.
void func_0200f4f4(void) {
    Initialise3dGraphics(0);
    GX::SetBankForTex(3);
    GX::SetBankForTexPltt(0x30);
    func_02045d9c();
    data_0209b3ec = IDENTITY_MATRIX4X3;
    InvMat4x3(&data_0209b3ec, &data_0209b41c);
}

// Reset the matrix stacks: identity texture matrix, then back to
// position-vector mode.
void func_0200f4b4(void) {
    G3X::Reset();
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Identity();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}
}

// Copy the camera matrix into the live view, then invert it.
void CopyToViewMat(const Matrix4x3 *src) {
    data_0209b3ec = *src;
    InvMat4x3(&data_0209b3ec, &data_0209b41c);
}

/* Crash-screen and title font. 0x2000 bytes; the tail is zero. */
extern u8 data_0208a178[0x2000];
