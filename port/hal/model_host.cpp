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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned int u32;

extern "C" {

// ---- asm primitives ------------------------------------------------------
/* PORT_HOST_ABI: ARM asm primitive (scale matrix), MSVC cannot assemble.
   4x3 fx32 scale matrix from (sx,sy,sz): rows {sx,0,0},{0,sy,0},{0,0,sz},0 */
void func_020527e8(int *m, int sx, int sy, int sz)
{
    memset(m, 0, 12 * sizeof(int));
    m[0] = sx;
    m[4] = sy;
    m[8] = sz;
}

/* Word copy primitive used by the VRAM upload CPU path.
 *
 * FORWARD, deliberately, and not memcpy. OAM::Reset uses this asm primitive as
 * a FILL: it writes one 8-byte entry and then copies the buffer onto itself
 * eight bytes further along, so a forward word-at-a-time copy replicates the
 * pattern across the whole shadow. memcpy may not overlap at all and memmove
 * would copy backwards and produce one entry followed by garbage. The DS
 * primitive walks forward; so does this.
 * PORT_HOST_ABI: ARM asm primitive (word copy/fill), MSVC cannot assemble. */
void MultiCopy_Int(int *src, int *dst, int len)
{
    if (((unsigned)(size_t)src | (unsigned)(size_t)dst | (unsigned)len) & 3) {
        unsigned char *d = (unsigned char *)dst;
        const unsigned char *s = (const unsigned char *)src;
        for (int i = 0; i < len; ++i) d[i] = s[i];
        return;
    }
    for (int i = 0; i < len / 4; ++i) dst[i] = src[i];
}

/* 32-byte-block copy primitive (asm on the DS). Forward, for the same reason.
   PORT_HOST_ABI: ARM asm primitive (32-byte block copy), MSVC cannot assemble. */
void MultiCopy32Bytes(int *src, int *dst, int len)
{
    for (int i = 0; i < len / 4; ++i) dst[i] = src[i];
}

// ---- OAM shadow state (BSS on the DS) -------------------------------------
//
// THE MAIN SHADOW IS ONE 0x400-BYTE BUFFER SPLIT THREE WAYS, and the split is
// the ROM's, not the port's: data_0209e67c is data_0209e674 + 8 and
// data_0209e694 is data_0209e674 + 0x20. dsd named the two interior addresses
// because OAM::Reset's fill references them, and the fill only works because
// they are interior:
//
//     entry[0] = disabled
//     MultiCopy_Int   (e674, e674+0x08, 0x18)    -> entries 1..3
//     MultiCopy32Bytes(e674, e674+0x20, 0x3e0)   -> entries 4..127
//     MultiCopy32Bytes(e674, ea74,      0x400)   -> the sub shadow
//
// Three separate host arrays turn that into two writes into unrelated storage
// and a main shadow that is 127/128 uninitialised, which is why the HAL used
// to pin data_0209e660 = 1 (the mode where Reset loops every entry itself) and
// call the adjacency unreachable. That mode also routes every sub==true sprite
// into the MAIN shadow (OAM::Render's `!sub || data_0209e660 == 1`), so the
// bottom screen could never receive a sprite at all.
//
// Grouped sections put the three symbols back in ROM order, the same mechanism
// the save block in hal/level_boot.cpp uses. align(4) is load-bearing: MSVC
// aligns arrays of 16 bytes or more to 16 by default, which would push
// data_0209e67c off +8. hal_oam_layout_check() reads the result back.
/* Force the three ROM symbols into one contiguous, declaration-ordered section
   so their addresses land at the ROM offsets (+0x00 / +0x08 / +0x20). MSVC uses
   $-suffix section grouping; GCC uses one .oamsh section + -fno-toplevel-reorder.
   They MUST stay real arrays (other TUs reference them as arrays -- a pointer
   would be an ODR/type mismatch). align(4) is load-bearing (see the note above).
   hal_oam_layout_check() verifies the result. */
#include "port_msvc_compat.h"
#define OAMSHADOW(sec, name, type, count)                        \
    PORT_GROUPED_DECL(sec, ".oamsh") type name[count]

unsigned char data_0209e660 = 1;
int data_0209e664, data_0209e668, data_0209e66c, data_0209e670;
/* main OAM shadow, 0x400 bytes, spelled as the ROM's three symbols */
OAMSHADOW(".oamsh$0000", data_0209e674, unsigned int, 2);      /* +0x000 */
OAMSHADOW(".oamsh$0001", data_0209e67c, int, 6);               /* +0x008 */
OAMSHADOW(".oamsh$0002", data_0209e694, int, 0xf8);            /* +0x020 */
unsigned int data_0209ea74[0x100];   /* sub OAM shadow, its own 0x400 */

#undef OAMSHADOW

/* Reads the section trick back. Returns nonzero when the three symbols came
   out at the ROM's offsets, which is the precondition for dual-OAM mode. */
int hal_oam_layout_check(void)
{
    const char *base = (const char *)data_0209e674;
    long a = (const char *)data_0209e67c - base;
    long b = (const char *)data_0209e694 - base;
    if (a == 0x08 && b == 0x20)
        return 1;
    printf("  [oam] SHADOW NOT CONTIGUOUS: e67c +%ld (want 8), e694 +%ld "
           "(want 32) -- dual-OAM mode would fill garbage\n", a, b);
    return 0;
}

/* PORT_HOST_ABI: ARM asm primitive (4x3 identity), MSVC cannot assemble. */
void Matrix4x3_LoadIdentity(int *m)
{
    memset(m, 0, 12 * sizeof(int));
    m[0] = m[4] = m[8] = 0x1000;
}

/* asm primitive: expand a 3x3 fx32 matrix (read from a) into a 4x4 (b):
 * fourth column zero, last row {0,0,0,1.0}. The decomp declares the args
 * (dst,src) but r0 is the READ side; register semantics win here.
 * PORT_HOST_ABI: ARM asm primitive (3x3->4x4 expand), MSVC cannot assemble. */
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
 * the host forwards to the pump that gate 4a already proved byte-exact.
 * PORT_HOST_ABI: DS DMA-to-GXFIFO; ntr models the FIFO seam, not raw DMA. */
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

// GX::LoadTex destination resolution (see src/_ZN2GX7LoadTexEPKvjj.cpp):
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

/* ---- gate 31: the cursors are per-LEVEL ------------------------------------
   The six words above are a bump allocator over texture VRAM and nothing ever
   moved them back, because until the port could change levels there was only
   ever one level's worth of uploads. The ROM resets them at the top of every
   level boot -- InitialiseVramGlobals, called by Stage::InitResources -- and
   without that the second boot walks the two arenas into each other and
   Model::GetVramOffset reaches the game's own Crash(). That is exactly what
   the second level change did, one frame after the new Player finished
   loading his own textures.

   This is InitialiseVramGlobals with the port's own recovered ceilings: the
   ROM reads its two from a VRAM-configuration table (func_02053e34 /
   func_02053e1c), and the port's are the constants above, recovered from
   LoadCompressedTextureToVram and GetVramOffset together. Same six words, same
   values they are declared with. */
void port_model_vram_reset(void)
{
    data_020a4bc8 = 0;
    data_020a4be8 = 0x20000u;
    data_020a4be0 = 0x20000u;
    data_020a4bdc = 0x40000u;
    data_020a4be4 = 0;
    data_020a4bcc = 0;
    data_020a4bd8 = 0x18000u;
}

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
/* Sized by the ROM SPAN, not a guessed field count: data_0209cefc runs to the
   next bss symbol data_0209d3ac at 0x0209cefc..0x0209d3ac = 0x4b0 = 1200 bytes
   = 100 records of 0xc. The old 64 was a guess; Model::AddToCommonModelDataArr
   appends a 0xc record and bumps data_0209cef8 with NO bounds check, so a level
   whose live BMD set passes 64 (Whomp's Fortress with the ov079 cast: whomps,
   bill blaster and the two fortress walls each register their own models) wrote
   the 65th record past the array and straight onto the neighbouring _ZTV5Model
   -- zeroing slot 2 (UpdateVerts) and faulting Stage::LoadModel's later dispatch
   on the shared Model vtable. */
int data_0209cefc[3 * 100]; /* 100 records x 0xc, the 0x4b0 ROM span */
int data_0209cef8[1];       /* record count */
int data_0208e738[1] = { 1 };   /* nonzero: upload textures on registration */
int data_0208e87c[8];       /* ModelBase vtable storage; never dispatched */
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
void *_ZTV16MeshColliderBase[13];   /* base: never dispatched in the gates */
unsigned char data_020a0c78[8]; /* the default CLPS ENTRY (8-byte storage,
                                   func_02037e9c fills it on first lookup) */

// 0x02099f80..0x02099f94 USED TO LIVE HERE, on the reading that 0x02099xxx
// was past bss_start and so runtime-initialized. It is not: the arm9's real
// bss_start is 0x0209b000 (extracted/dsd/arm9/arm9.yaml, and
// config/arm9/delinks.txt's .data/.bss split), so all four symbols are
// ordinary initialized .data and now come from port/tools/romdata.py.
//
// The correction is not cosmetic. data_02099f88 is the DEFAULT SCALE VECTOR
// func_02044534 copies when Model::Render is called with no scale argument,
// and the billboard part walk multiplies its whole 3x3 through it:
//
//     m.r2 = {0, 0, dir->z}
//     m.r1.x = FIXMUL(normalized r1.x, dir->x)   ... and so on
//
// Zeroed, that 3x3 comes out all zeros, the MTX_MULT_3x3 it feeds wipes the
// rotation out of the position matrix, and every vertex of a billboard model
// projects to the model's translation point -- the castle grounds' trees,
// drawn as one degenerate triangle each. The ROM's value is
// {0x1000, 0x1000, 0x1000}: unit scale.
//
// The two weight tables that were reconstructed here by hand -- b[shift] =
// 1 << shift for the BCA keyframe samplers -- read back from the ROM as
// {1, 2, 4, 0}, exactly what was reconstructed. That agreement is what
// proves the new section boundary rather than merely asserting it.

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
/* SM64DS_TEX_LOG=1: every texel upload that reaches fake VRAM, with the
   two arena cursors either side of it. Reads as the ledger the bind log
   is checked against. */
extern "C" int hal_tex_log(void)
{
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_TEX_LOG") ? 1 : 0;
    return on;
}

#ifdef _WIN32
/* MSVC: the extern-C name and the namespace `GX::LoadTex` (from src/) mangle
   differently, so this is a real logging shim in front of the src upload. */
extern "C" void _ZN2GX7LoadTexEPKvjj(const void *s, unsigned o, unsigned z)
{
    if (hal_tex_log())
        printf("[texup]  off=%05x size=%05x  A[%05x..%05x) B[%05x..%05x)\n", o,
               z, data_020a4bc8, data_020a4be8, data_020a4be0, data_020a4bdc);
    GX::LoadTex(s, o, z);
}
#endif /* _WIN32 -- on GCC `GX::LoadTex` IS _ZN2GX7LoadTexEPKvjj, so this shim
   would call itself. On Linux the callers bind straight to the real src/ upload
   (the SM64DS_TEX_LOG texel line is Windows-only as a result). */

// DMA fallback DMASyncWordTransfer uses; same synchronous copy semantics.
// PORT_HOST_ABI: DS DMA register poke (0x040000b0 + 2-cycle barrier reads),
//                the ntr layer models the FIFO seam, not raw DMA registers.
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
