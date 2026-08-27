// Host side of the Model pipeline (gate 4b).
//
// The game's own code does the loading, rebasing, VRAM upload and render
// walk; this file provides only what hardware/asm provided on the DS:
//
// - the two asm copy/matrix primitives the walk calls,
// - the VRAM window globals GX::LoadTex/LoadTexPltt resolve against
//   (on hardware these come from bank assignment at GX init; here they
//   point into the ntr mapped VRAM so a slot offset IS a host address),
// - the texture-VRAM budget globals Model::GetVramOffset bump-allocates
//   from,
// - storage for the render context globals the walk stores through.
#include <string.h>

typedef unsigned int u32;

extern "C" {

// ---- asm primitives ------------------------------------------------------
/* 4x3 fx32 scale matrix from (sx,sy,sz): rows {sx,0,0},{0,sy,0},{0,0,sz},0 */
void func_020527e8(int *m, int sx, int sy, int sz)
{
    memset(m, 0, 12 * sizeof(int));
    m[0] = sx;
    m[4] = sy;
    m[8] = sz;
}

/* word copy primitive used by the VRAM upload CPU path */
void MultiCopy_Int(int *src, int *dst, int len)
{
    memcpy(dst, src, len);
}

/* 32-byte-block copy primitive (asm on the DS) */
void MultiCopy32Bytes(int *src, int *dst, int len)
{
    memcpy(dst, src, len);
}

// ---- OAM shadow state (BSS on the DS) -------------------------------------
// On the DS, data_0209e67c/data_0209e694 are addresses INSIDE the main
// shadow buffer (Reset's fill-pattern copies rely on that adjacency). Host
// symbols cannot alias at an offset, so the HAL runs the engine in the mode
// where Reset loops every entry directly (data_0209e660 = 1) and adjacency
// is never exercised; the split fill buffers below are then inert.
unsigned char data_0209e660 = 1;
int data_0209e664, data_0209e668, data_0209e66c, data_0209e670;
unsigned int data_0209e674[0x100];   /* main OAM shadow, 0x400 bytes */
unsigned int data_0209ea74[0x100];   /* sub OAM shadow */
int data_0209e67c[0x20];
int data_0209e694[0x100];

/* asm primitive: 4x3 fx32 identity */
void Matrix4x3_LoadIdentity(int *m)
{
    memset(m, 0, 12 * sizeof(int));
    m[0] = m[4] = m[8] = 0x1000;
}

/* asm primitive: expand a 3x3 fx32 matrix (read from a) into a 4x4 (b):
 * fourth column zero, last row {0,0,0,1.0}. The decomp declares the args
 * (dst,src) but r0 is the READ side; register semantics win here. */
void func_02052514(int *a, int *b)
{
    for (int r = 0; r < 3; ++r) {
        b[r * 4 + 0] = a[r * 3 + 0];
        b[r * 4 + 1] = a[r * 3 + 1];
        b[r * 4 + 2] = a[r * 3 + 2];
        b[r * 4 + 3] = 0;
    }
    b[12] = b[13] = b[14] = 0;
    b[15] = 0x1000;
}

/* ITCM fast-path display-list submit: same (channel, src, size) contract as
 * func_0205a358 (func_02044534 calls that one with identical arguments), so
 * the host forwards to the pump that gate 4a already proved byte-exact. */
void func_0205a358(int ch, int src, int size, void (*cb)(int), int arg);
void func_01ffde98(int ch, int src, int size)
{
    func_0205a358(ch, src, size, 0, 0);
}

// ---- GX bank plumbing ----------------------------------------------------
// Begin/End LoadTex unmap the destination banks to LCDC and remap after; the
// host VRAM window is always CPU-visible, so they are no-ops.
void _ZN2GX12BeginLoadTexEv(void) {}
void _ZN2GX10EndLoadTexEv(void) {}
void _ZN2GX16BeginLoadTexPlttEv(void) {}
void _ZN2GX14EndLoadTexPlttEv(void) {}

// GX::LoadTex destination resolution (see src/game/actors/GX/_ZN2GX7LoadTexEPKvjj.cpp):
//   data_020a60ac  base window address        -> texture slots at 0x06800000
//   data_020a60bc  secondary bank address     -> 0 = single flat window
//   data_020a60c0  where the window splits    -> unused while _bc is 0
//   data_02099fd0  preferred DMA channel      -> -1 = CPU copy path
u32 data_020a60ac = 0x06800000u;
u32 data_020a60bc = 0;
u32 data_020a60c0 = 0;
int data_02099fd0 = -1;

// LoadTexPltt destination base (palette slots, same convention)
u32 data_020a60b0 = 0x06880000u;

// ---- Model texture-VRAM budget -------------------------------------------
// Two shared arenas (semantics recovered from LoadCompressedTextureToVram +
// GetVramOffset together):
//   arena A (slot 0): compressed 4x4 BLOCKS rise from _bc8, plain textures
//     fall from _be8 (the ceiling); the loader crashes when they meet.
//   arena B (slot 1): compressed INDEX words rise from _be0 in lockstep
//     (half the block bytes), plain textures fall from _bdc.
// Starting _bc8 at 0 and _be0 at slot 1 (0x20000) keeps the hardware rule
// "index address = slot1 + block_offset/2" true for every compressed
// texture, which is exactly what the GX-side decoder assumes.
u32 data_020a4bc8 = 0;
u32 data_020a4be8 = 0x20000u;
u32 data_020a4be0 = 0x20000u;
u32 data_020a4bdc = 0x40000u;
u32 data_020a4be4 = 0;      /* running total of uploaded texture bytes */
// Palette window: small palettes bump UP from _bcc, large ones DOWN from
// _bd8 (the ceiling); LoadTexAndPal crashes when they meet. 96K = banks
// E+F+G, the full DS texture-palette budget.
u32 data_020a4bcc = 0;
u32 data_020a4bd8 = 0x18000u;

// ---- render context globals ----------------------------------------------
u32 data_020a4bd4;          /* 1 << (scaleShift + 12), set by Render */
void *data_020a4bd0;        /* current model matrix */
unsigned char data_020a4bbc;    /* texture-matrix mode of the bound material */
short data_020a4bc0;
short data_020a4bc4;

/* processing-list heads the actor lifecycle links into */
int data_020a4b78[4];
int data_020a4b88[4];
int data_020a4b98[4];
int data_02099f24[4];

// ---- common model data array (BSS on the DS) ------------------------------
int data_0209cefc[3 * 64];  /* 64 records x 0xc */
int data_0209cef8[1];       /* record count */
int data_0208e738[1] = { 1 };   /* nonzero: upload textures on registration */
int _ZTV9ModelBase[8];       /* ModelBase vtable storage; never dispatched */
int data_0209b3ec[12];      /* camera Matrix4x3 the render walk composes */

// Model's Itanium vtable storage: installed by the C ctor. The gates call
// every method non-virtually (qualified), so plain zero slots are fine; if
// anything ever dispatches through them it faults immediately and loudly.
void *_ZTV5Model[8];
// Same story for the animated-model hierarchy: primary vtables plus the
// multiple-inheritance thunk table the ModelAnim ctor installs at +0x50.
void *_ZTV9Animation[8];
void *_ZTV9ModelAnim[10];
void *VTable_Animation_ModelAnimThunk[8];
void *_ZTV4dBgW[13];   /* base: never dispatched in the gates */
unsigned char data_020a0c78[8]; /* the default CLPS ENTRY (8-byte storage,
                                   func_02037e9c fills it on first lookup) */

// BSS globals of the render/animation walk (0x02099xxx is past bss_start;
// their DS values come from init code not yet in any slice):
// - data_02099f80/f84: 3-byte weight tables for the BCA keyframe samplers.
//   The interpolation is (lo*frac + hi*rem) over 2^shift steps and frac
//   starts at b[shift] minus the step remainder, so b[shift] = 1 << shift.
//   b[0] is unreachable (shift==0 early-returns).
// - data_02099f88/f94: constants of the texture-matrix material path,
//   zero until the GX-init that writes them joins a slice.
unsigned char data_02099f80[4] = { 1, 2, 4, 0 };
unsigned char data_02099f84[4] = { 1, 2, 4, 0 };
int data_02099f88[3];
unsigned short data_02099f94[8];

} /* extern "C" */

// The LoadTex TU declares its globals at C++ linkage; alias them to the
// C-named storage above (same mechanism as gate 3a's data aliases).
#pragma comment(linker, "/alternatename:?data_020a60ac@@3IA=_data_020a60ac")
#pragma comment(linker, "/alternatename:?data_020a60bc@@3IA=_data_020a60bc")
#pragma comment(linker, "/alternatename:?data_020a60c0@@3IA=_data_020a60c0")
#pragma comment(linker, "/alternatename:?data_02099fd0@@3HA=_data_02099fd0")

// ---- upload-chain bridges -------------------------------------------------
// C name -> the real namespace-style GX::LoadTex from src/.
namespace GX {
void LoadTex(const void *src, unsigned offset, unsigned size);
}
extern "C" void _ZN2GX7LoadTexEPKvjj(const void *s, unsigned o, unsigned z)
{
    GX::LoadTex(s, o, z);
}

// DMA fallback DMASyncWordTransfer uses; same synchronous copy semantics.
extern "C" void DMAStartTransfer(int ch, int src, int dst, int ctrl);
extern "C" void DMAStartTransferFB(unsigned char ch, u32 src, u32 dst, u32 ctrl)
{
    DMAStartTransfer(ch, (int)src, (int)dst, (int)ctrl);
}

// operator_new2 on the DS is a register-passthrough tail-call veneer to
// func_0203cc0c; that shape breaks on cdecl (the callee would read the
// wrong stack slot), so the host bridge passes the argument explicitly.
extern "C" void *func_0203cc0c(unsigned size);
extern "C" void *_ZN6Memory13operator_new2Ej(unsigned size)
{
    return func_0203cc0c(size);
}

// OAM::Reset declares its globals at C++ linkage; alias them onto the
// C-named storage above (same mechanism as the LoadTex globals).
#pragma comment(linker, "/alternatename:?data_0209e660@@3EA=_data_0209e660")
#pragma comment(linker, "/alternatename:?data_0209e664@@3HC=_data_0209e664")
#pragma comment(linker, "/alternatename:?data_0209e668@@3HC=_data_0209e668")
#pragma comment(linker, "/alternatename:?data_0209e66c@@3HC=_data_0209e66c")
#pragma comment(linker, "/alternatename:?data_0209e670@@3HC=_data_0209e670")
#pragma comment(linker, "/alternatename:?data_0209e674@@3PAUOAMEntry@@A=_data_0209e674")
#pragma comment(linker, "/alternatename:?data_0209ea74@@3PAHA=_data_0209ea74")
#pragma comment(linker, "/alternatename:?data_0209e67c@@3PAHA=_data_0209e67c")
#pragma comment(linker, "/alternatename:?data_0209e694@@3PAHA=_data_0209e694")
