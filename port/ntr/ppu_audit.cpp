// The 2D register audit: which unmodelled hardware a scene ACTUALLY touches.
//
// WHY THIS EXISTS. port/ov004_ov007_2d_map.txt section 7 lists six clusters of
// 2D hardware the ntr layer does not model. That list says what COULD matter.
// It was written from what two overlays' source pokes, not from a run. This
// samples what the live scenes really leave in the register file, so the gaps
// get closed in need order instead of encyclopedia order.
//
// WHY IT SAMPLES INSTEAD OF INTERCEPTING. ntr/io.cpp maps real memory at the DS
// addresses, so a 2D register store is an ordinary store to an ordinary page.
// It does not pass through io_read/io_write unless the storing function happens
// to be hostgen'd, which for the 2D surface is the exception. There is no seam
// to intercept at, and the same fact is why L1's GXSTAT fix had to normalise the
// LATCH rather than a returned value.
//
// WHAT SAMPLING SEES, AND WHAT IT CANNOT. It sees the register state at
// scan-out, which is the state that decides the image: on hardware the 2D unit
// reads these registers as it draws, and the port has no scanline timing at all,
// so a value written and overwritten before scan-out cannot reach the screen in
// either one. It does NOT see such a mid-frame value, and it cannot distinguish
// "never written" from "written zero". Zero is the off state for every register
// in the gap set (BLDCNT mode 0 is no effect, MASTER_BRIGHT factor 0 is no
// change, the window enables live in DISPCNT bits 13-15 and are sampled), so
// zero reads as "not in use" throughout, which is sound for the question asked
// and is NOT sound for any question about writes.
//
// The proxy hook is the exact half: anything that does reach io_write is logged
// with its value. It is a strict subset and is reported separately so the two
// are never confused.

#include "ntr/ppu_audit.h"

#include "ntr/gx.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ntr {
namespace {

// ---- the register file ------------------------------------------------------
//
// Offsets are engine-relative: engine A at 0x04000000, engine B at 0x04001000.
// GBATEK's 2D display block, cross-read against the decomp's own accessors
// where one exists (ppu.cpp's header note records which shifts came from
// G2::GetBG0ScrPtr and G2S::GetBG0CharPtr rather than from docs).
//
// `eng` is which engines have the register at all. `modelled` is whether the
// port's LIVE path for that engine reads it, which is not the same question and
// differs between the two engines:
//
//   engine A live path = hal/message_compositor.cpp (2D over the 3D frame).
//                        ntr/ppu.cpp's ppu_scanout is NOT in any live path; it
//                        fills the whole framebuffer and would erase the 3D.
//   engine B live path = ntr/ppu_sub.cpp, via hal/sub_screen.cpp.
//
// so a register can be modelled on B and a gap on A. The minimap's affine
// registers are exactly that.

enum { E_A = 1, E_B = 2 };

struct Reg {
    uint16_t off;
    uint8_t width;      // 2 or 4
    uint8_t eng;        // which engines have it
    uint8_t modelled;   // which engines' live path reads it
    const char *name;
};

const Reg kRegs[] = {
    // DISPCNT is `modelled` on both engines at REGISTER granularity, and that
    // granularity is what hid a gap for a whole lane: the word carries fields
    // whose readers disagree. Recorded per bit, because "M" here has never
    // meant "every bit of it":
    //   bit  4  OBJ 1D/2D tile mapping. READ on BOTH from run link60 lane EAD
    //           on. It used to be read on B only, and the row said so with the
    //           gap attributed to "ppu.cpp's OBJ layer". That was accurate
    //           about ntr/ppu.cpp and it named the wrong file for the
    //           consequence: engine A's LIVE path is
    //           hal/message_compositor.cpp (this table's own header says so),
    //           ppu.cpp's OBJ layer has one caller and it is a test, and the
    //           compositor had the identical hard-coded 1D. Both read the bit
    //           now. Measured on scene 374: DISPCNT_A bit 4 CLEAR on all 300
    //           samples, which is 2D, and 2D is what
    //           Scene::ResetHardwareRegisters leaves both engines in.
    //   bits 8-12  layer enables. Read on both.
    //   bit  16-17 display mode. Read on both.
    //   bits 20-21 OBJ 1D tile boundary. Read on both. Applies to 1D mapping
    //           only on hardware; all three readers apply it either way, which
    //           is inert while the game leaves the field at 0 and is a gap the
    //           day it does not. STILL INERT AND STILL UNFIXED: scene 374
    //           measures the field at 0 on all 300 samples (the same
    //           0xffcfffef in Scene::ResetHardwareRegisters that clears bit 4
    //           clears this pair), so the lane that read the bit had no
    //           measurement that would have told a correct fix from a wrong
    //           one, and left it recorded rather than guessed.
    //   bits 24-29 char/screen base offsets. Engine A only, read there.
    //   bit  30 BG extended palettes. READ on B (ppu_sub.cpp's read_bg).
    //   bit  31 OBJ extended palettes. READ on B, from this lane on.
    {0x00, 4, E_A | E_B, E_A | E_B, "DISPCNT"},
    {0x08, 2, E_A | E_B, E_A | E_B, "BG0CNT"},
    {0x0A, 2, E_A | E_B, E_A | E_B, "BG1CNT"},
    {0x0C, 2, E_A | E_B, E_A | E_B, "BG2CNT"},
    {0x0E, 2, E_A | E_B, E_A | E_B, "BG3CNT"},
    {0x10, 2, E_A | E_B, E_A | E_B, "BG0HOFS"},
    {0x12, 2, E_A | E_B, E_A | E_B, "BG0VOFS"},
    {0x14, 2, E_A | E_B, E_A | E_B, "BG1HOFS"},
    {0x16, 2, E_A | E_B, E_A | E_B, "BG1VOFS"},
    {0x18, 2, E_A | E_B, E_A | E_B, "BG2HOFS"},
    {0x1A, 2, E_A | E_B, E_A | E_B, "BG2VOFS"},
    {0x1C, 2, E_A | E_B, E_A | E_B, "BG3HOFS"},
    {0x1E, 2, E_A | E_B, E_A | E_B, "BG3VOFS"},
    // BG2/BG3 rotation and scaling. Modelled on B (ppu_sub does affine and
    // extended affine); a gap on A, where the compositor is text-mode only.
    {0x20, 2, E_A | E_B, E_B, "BG2PA"},
    {0x22, 2, E_A | E_B, E_B, "BG2PB"},
    {0x24, 2, E_A | E_B, E_B, "BG2PC"},
    {0x26, 2, E_A | E_B, E_B, "BG2PD"},
    {0x28, 4, E_A | E_B, E_B, "BG2X"},
    {0x2C, 4, E_A | E_B, E_B, "BG2Y"},
    {0x30, 2, E_A | E_B, E_B, "BG3PA"},
    {0x32, 2, E_A | E_B, E_B, "BG3PB"},
    {0x34, 2, E_A | E_B, E_B, "BG3PC"},
    {0x36, 2, E_A | E_B, E_B, "BG3PD"},
    {0x38, 4, E_A | E_B, E_B, "BG3X"},
    {0x3C, 4, E_A | E_B, E_B, "BG3Y"},
    // The window unit. Modelled on both, which the gap list did not know.
    {0x40, 2, E_A | E_B, E_A | E_B, "WIN0H"},
    {0x42, 2, E_A | E_B, E_A | E_B, "WIN1H"},
    {0x44, 2, E_A | E_B, E_A | E_B, "WIN0V"},
    {0x46, 2, E_A | E_B, E_A | E_B, "WIN1V"},
    {0x48, 2, E_A | E_B, E_A | E_B, "WININ"},
    {0x4A, 2, E_A | E_B, E_A | E_B, "WINOUT"},
    {0x4C, 2, E_A | E_B, 0, "MOSAIC"},
    // Colour special effects: nothing reads these on either engine.
    {0x50, 2, E_A | E_B, 0, "BLDCNT"},
    {0x52, 2, E_A | E_B, 0, "BLDALPHA"},
    {0x54, 2, E_A | E_B, 0, "BLDY"},
    // Engine A only.
    {0x60, 4, E_A, 0, "DISP3DCNT"},
    {0x64, 4, E_A, 0, "DISPCAPCNT"},
    {0x68, 4, E_A, 0, "DISP_MMEM_FIFO"},
    // Master brightness. ppu_sub applies it; the engine-A compositor does not,
    // though walk_window's own fade composite runs after it (hal/fader_wipes.cpp
    // reads BLDCNT/BLDY itself, which is a THIRD reader and is why the audit
    // reports the raw values rather than a verdict).
    {0x6C, 2, E_A | E_B, E_B, "MASTER_BRIGHT"},
};
const unsigned kRegN = sizeof kRegs / sizeof kRegs[0];

// ---- what is remembered per register ----------------------------------------

const unsigned kMaxVals = 6;

struct Slot {
    uint32_t val[kMaxVals];
    uint32_t hits[kMaxVals];
    unsigned n;
    unsigned nonzero;   // samples where the value was not 0
    int first_frame;    // sample index of the first non-zero, -1 if never
};

Slot g_slot[2][kRegN];

void note(Slot &s, uint32_t v, int frame) {
    if (v) {
        ++s.nonzero;
        if (s.first_frame < 0) s.first_frame = frame;
    }
    for (unsigned i = 0; i < s.n; ++i)
        if (s.val[i] == v) { ++s.hits[i]; return; }
    if (s.n < kMaxVals) {
        s.val[s.n] = v;
        s.hits[s.n] = 1;
        ++s.n;
    }
}

// ---- the OAM object-mode census ---------------------------------------------
//
// Attribute 0 bits 10-11 are the object mode and bit 12 is mosaic. That layout
// is not taken from docs alone: the decomp's own OAM::Render composes the word
// as `(mode << 10) | (mosaic << 12)` over a `u32 objMode : 2` bitfield
// (src/_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi.c), which pins both fields.
//
// The four DS modes are counted separately on purpose. Both ppu.cpp and
// ppu_sub.cpp skip mode 3 with the comment "OBJ window", and the gap list
// repeats it. Counting all four says which mode the game really uses rather
// than assuming the comment names it.

struct ObjCensus {
    uint32_t mode[4];   // enabled objects seen in each mode, summed over samples
    uint32_t mosaic;
    uint32_t frames_with[4];   // samples in which the mode appeared at all
};

ObjCensus g_obj[2];

// ---- the OAM ATTRIBUTE census ------------------------------------------------
//
// The mode census above says what KIND of object each entry is. It cannot say
// what an object DECODES FROM, and that distinction is the one this instrument
// was missing: a sprite drawn from the wrong tiles and a sprite drawn through
// the wrong palette are both "mode 0 normal" here, so a mode count cannot tell
// a mapping gap from a palette gap. Scene 4's thirteen sub sprites were read
// for a whole lane without anyone being able to name their colour mode.
//
// So the distinct enabled-entry triples are remembered whole, and the fields
// the decode path actually branches on are named out of them at dump time:
//
//   attr0 bit  8   affine, and with it attr1 bits 9-13 the matrix group
//   attr0 bit  13  colour mode. 0 = 16 colours through the attr2 palette bank,
//                  1 = 256 colours -- and 256 is the only mode an EXTENDED
//                  palette applies to, so this bit is what decides whether
//                  DISPCNT bit 30/31 can matter to a given sprite at all
//   attr2 bits 0-9   character name, in tile-boundary units
//   attr2 bits 10-11 priority
//   attr2 bits 12-15 palette: a 16-colour bank, or an extended-palette SLOT
//                  when the colour mode is 256 and extended palettes are armed
//
// Triples, not entries: a HUD redraws the same sprite every frame, so the
// distinct-value form is a few rows where the per-entry form is 38400.

struct ObjAttr { uint16_t a0, a1, a2; uint32_t n; };
const unsigned kMaxAttr = 20;

ObjAttr g_attr[2][kMaxAttr];
unsigned g_attr_n[2];
uint32_t g_attr_dropped[2];

// ---- the FIRST SAMPLE's OAM, entry by entry ----------------------------------
//
// The distinct-triples table above is a whole-run summary and it DROPS rows
// once it is full. That is the right shape for a HUD which redraws the same
// sprite every frame and the wrong shape for a scene whose sprites move: scene
// 374's 53 placed engine-A sprites produce 37,135 distinct triples over 300
// frames, so the table above reports twenty of them and drops the rest, and a
// question like "is any sprite on this screen 256-colour" cannot be answered
// off a sample that dropped 37,115 rows.
//
// This is the exact other half: ONE frame, every enabled entry, nothing
// dropped. It cannot say what the run did later and does not try to; the
// triples table is still what carries the run. Between them a mapping or
// palette question has one frame with a complete answer and 300 frames with a
// partial one, which is better than 300 partial.
struct ObjFirst { uint16_t a0, a1, a2; uint8_t idx; };
ObjFirst g_first[2][128];
unsigned g_first_n[2];
bool g_first_taken[2];

void note_attr(int e, uint16_t a0, uint16_t a1, uint16_t a2) {
    for (unsigned i = 0; i < g_attr_n[e]; ++i)
        if (g_attr[e][i].a0 == a0 && g_attr[e][i].a1 == a1
            && g_attr[e][i].a2 == a2) { ++g_attr[e][i].n; return; }
    if (g_attr_n[e] >= kMaxAttr) { ++g_attr_dropped[e]; return; }
    ObjAttr &a = g_attr[e][g_attr_n[e]++];
    a.a0 = a0; a.a1 = a1; a.a2 = a2; a.n = 1;
}

void census_oam(int e) {
    const uint32_t base = e == 0 ? 0x07000000u : 0x07000400u;
    unsigned seen[4] = {0, 0, 0, 0};
    for (int i = 0; i < 128; ++i) {
        const uint16_t a0 = *reinterpret_cast<volatile uint16_t *>(base + i * 8u);
        const bool affine = (a0 & 0x100) != 0;
        if (!affine && (a0 & 0x200)) continue;      // disabled
        const unsigned m = (a0 >> 10) & 3;
        ++g_obj[e].mode[m];
        ++seen[m];
        if ((a0 >> 12) & 1) ++g_obj[e].mosaic;
        const uint16_t a1 = *reinterpret_cast<volatile uint16_t *>(base + i * 8u + 2);
        const uint16_t a2 = *reinterpret_cast<volatile uint16_t *>(base + i * 8u + 4);
        note_attr(e, a0, a1, a2);
        if (!g_first_taken[e] && g_first_n[e] < 128) {
            ObjFirst &o = g_first[e][g_first_n[e]++];
            o.a0 = a0; o.a1 = a1; o.a2 = a2; o.idx = (uint8_t)i;
        }
    }
    g_first_taken[e] = true;
    for (int m = 0; m < 4; ++m)
        if (seen[m]) ++g_obj[e].frames_with[m];
}

// ---- what the DECODE reads FROM ----------------------------------------------
//
// Three stores feed a sprite, and a run where one of them is empty produces the
// same "the sprite is wrong" report as a run where the decode path is wrong.
// hal/sub_screen.cpp's own probe already counts the first two for engine B; the
// EXTENDED palette store was in neither probe, which is why "bit 31 is clear"
// could not be told apart from "there is no extended palette to enable".
//
// THE BACKGROUND STORES ARE HERE FOR THE SAME REASON, added when a top-screen
// frame turned out to be 95% background by pixel count and the table could say
// nothing at all about that 95%. A background reads from three stores too --
// the tilemap, the character data and the palette -- and the same ambiguity
// applies: "the background is wrong" is not a decode finding until the bytes
// the reader saw are known.
//
// SM64DS_PPU_AUDIT_DUMP=<prefix> writes each span's BYTES once, on the first
// sample, to <prefix>_<engine>_<name>.bin. A count says whether a store is
// populated; only the bytes say whether it holds what the ROM's own file holds,
// and several of these stores are a straight copy of a NitroFS file (curling's
// BG2 tilemap is file 0x30 loaded raw at offset 0, its BG palette file 0x2f at
// byte 0x60), so the comparison is exact and needs no emulator.
//
// EVERY BASE HERE COMES OUT OF THE ROM'S OWN CODE, not from a doc:
//   0x068a0000  src/_ZN3GXS14LoadOBJExtPlttEPKvjj.c computes its destination as
//               destSlotAddr + 0x068a0000, and src/_ZN2GX23SetBankForSubOBJ
//               ExtPlttEt.c is what maps a bank there (VRAMCNT_I = 0x83) and
//               sets DISPCNT_B bit 31 in the same breath.
//   0x06600000  ntr/ppu_sub.cpp's kObjVram, and dScStarSel_c::InitResources'
//               own DecompressLZ16 destination.
//   0x05000600  GXS::LoadOBJPltt's destination for the same scene.
// Engine A has NO extended-palette row: no GX::LoadOBJExtPltt TU exists in this
// tree, so there is no ROM-derived base to print and a guessed one would be the
// invented mapping this file exists to avoid.

struct Span { uint32_t base, size; uint8_t eng; const char *name; };

const Span kObjSrc[] = {
    {0x06000000, 0x80000, E_A, "BG VRAM"},
    {0x05000000, 0x200,   E_A, "BG palette"},
    {0x06400000, 0x40000, E_A, "OBJ VRAM"},
    {0x05000200, 0x200,   E_A, "OBJ palette"},
    {0x06200000, 0x20000, E_B, "BG VRAM"},
    {0x05000400, 0x200,   E_B, "BG palette"},
    {0x06600000, 0x20000, E_B, "OBJ VRAM"},
    {0x05000600, 0x200,   E_B, "OBJ palette"},
    {0x068A0000, 0x2000,  E_B, "OBJ ext palette"},
};
const unsigned kObjSrcN = sizeof kObjSrc / sizeof kObjSrc[0];

// One span's bytes on disk, once per run. Named after the span so a directory
// of them reads without a key, with the spaces squeezed out.
void dump_span(const char *prefix, int e, const Span &s) {
    char path[512];
    char name[32];
    unsigned n = 0;
    for (const char *p = s.name; *p && n + 1 < sizeof name; ++p)
        name[n++] = (*p == ' ') ? '_' : *p;
    name[n] = 0;
    std::snprintf(path, sizeof path, "%s_%c_%s.bin", prefix,
                  e == 0 ? 'A' : 'B', name);
    std::FILE *g = std::fopen(path, "wb");
    if (!g) return;
    for (uint32_t i = 0; i < s.size; ++i) {
        const uint8_t b = *reinterpret_cast<volatile uint8_t *>(s.base + i);
        std::fputc(b, g);
    }
    std::fclose(g);
}

uint32_t nonzero_bytes(uint32_t base, uint32_t size) {
    uint32_t n = 0;
    for (uint32_t i = 0; i < size; ++i)
        if (*reinterpret_cast<volatile uint8_t *>(base + i)) ++n;
    return n;
}

// ---- the touch surface ------------------------------------------------------
//
// NOT the stylus record. TouchInfo[4] at DS 0x020a0de8 is a hosted global living
// in the port's .dsstate section, not at that DS address, so it is not memory
// this file can read, and it is already driven from the mouse by
// hal/sub_screen.cpp's poll_touch. What IS memory here is the hardware surface:
// the ARM7 SPI ports and the shared-WRAM words the IPC touch handler reads.
// Sampling them answers the only open question, which is whether any LINKED
// code is waiting on a panel model.

struct TouchReg { uint32_t addr; uint8_t width; const char *name; };

const TouchReg kTouch[] = {
    {0x04000130, 2, "KEYINPUT"},
    {0x04000136, 2, "EXTKEYIN"},
    // The pad is TWO latches. func_02013f4c reads
    //     (KEYINPUT | *(u16 *)0x27fffa8) ^ 0x2fff
    // because X, Y and the hinge are ARM7-side and not visible at 0x04000130.
    // Sampling only the first would report half a surface, and the keypad is
    // active low, so a zero on either half reads as "held", not as "absent".
    {0x027FFFA8, 2, "SHARED_PAD"},
    {0x040001C0, 2, "SPICNT"},
    {0x040001C2, 2, "SPIDATA"},
    {0x027FFFAA, 2, "SHARED_TP_LO"},   // func_0205f300's two reads
    {0x027FFFAC, 2, "SHARED_TP_HI"},
};
const unsigned kTouchN = sizeof kTouch / sizeof kTouch[0];

Slot g_touch[kTouchN];

// ---- proxy hits (the exact half) --------------------------------------------

struct ProxyHit { uint32_t addr; uint32_t value; uint8_t width; uint8_t write; uint32_t n; };
const unsigned kMaxProxy = 64;
ProxyHit g_proxy[kMaxProxy];
unsigned g_proxy_n;
uint32_t g_proxy_dropped;

// ---- state ------------------------------------------------------------------

int g_on = -1;
const char *g_path;
int g_frames;
// The byte dump is a FIRST-SAMPLE snapshot, not a per-flush one: the audit
// rewrites its table every 32 frames and a store overwritten mid-run would
// silently replace the bytes a reader had already begun comparing.
bool g_dumped;
char g_tag[32];
bool g_registered;

// ---- the engine-A 3D layer ---------------------------------------------------
//
// EVERY ROW ABOVE THIS ONE IS 2D, AND ON THE TOP SCREEN THAT IS HALF THE
// PICTURE. The DS composites engine A's 2D layers over the 3D engine's output,
// so a top-screen frame that is missing content has two candidate sources and
// the 2D table alone cannot separate them: an empty frame reads the same
// whether the 3D layer submitted nothing or the 2D layers were never enabled.
//
// Two numbers settle it, and both are sampled at the same instant as the
// registers above, which is what makes them comparable with them:
//
//   the VIEWPORT rectangle the geometry engine has latched. It is the port's
//   own g.vp_*, already scaled off the DS panel by the command handler, so at
//   the 2x tier a full-screen DS viewport reads 0,0 512x384. A frame whose
//   content sits in one corner and whose viewport IS that corner is a viewport
//   fault; the same frame with a full-screen viewport is not, and the
//   difference is one line rather than an argument.
//
//   the polygon count still standing in the list. Sampled after gx_render on
//   every path that renders, so it is this frame's geometry, not last frame's.
//   Zero means the 3D engine was handed nothing at all -- which for a 2D
//   minigame is the ROM's intent and not a defect, and the DISPCNT BG0 enable
//   bit in the table above is what says which of those two it is.

// Four separate slots rather than one packed sample. A packed rectangle would
// have to drop bits to fit, and a viewport row that cannot represent every
// rectangle the port can latch is exactly the instrument that reports the
// wrong answer on the one frame it matters.
const char *const k3dNames[] = {"VIEWPORT_X", "VIEWPORT_Y", "VIEWPORT_W",
                                "VIEWPORT_H", "VIEWPORT_SETS", "POLYGONS"};
const unsigned k3dN = sizeof k3dNames / sizeof k3dNames[0];
Slot g_3d[k3dN];

// ---- the geometry COMMAND census (run link60 Stage 5 lane T2) ---------------
//
// THE ROW ABOVE SAYS GEOMETRY ARRIVED. IT DOES NOT SAY THE STATE DID. A frame
// can submit eighty polygons through the packed FIFO -- which the shared,
// hostgen'd model path drives -- while every command that decides WHERE those
// polygons land (matrix mode, the projection and position loads, the viewport)
// is issued by a scene's own translation unit through a plain store to the DS
// address, lands in the memory ntr maps there, and never reaches the geometry
// engine at all. POLYGONS and VIEWPORT_SETS together hint at that shape; this
// table names it, per frame, per command.
//
// ntr::gx_debug_commands is TAKEN rather than read, so every row below is
// "this frame", not "since boot". PORT_WRITES and FIFO_WORDS are the two doors
// into the engine and the split matters: a scene whose state commands are all
// missing reads FIFO_WORDS high and PORT_WRITES near zero.
struct CmdRow { uint8_t cmd; const char *name; };
const CmdRow kCmds[] = {
    {0x10, "MTX_MODE"},      {0x11, "MTX_PUSH"},      {0x12, "MTX_POP"},
    {0x13, "MTX_STORE"},     {0x14, "MTX_RESTORE"},   {0x15, "MTX_IDENTITY"},
    {0x16, "MTX_LOAD_4x4"},  {0x17, "MTX_LOAD_4x3"},  {0x18, "MTX_MULT_4x4"},
    {0x19, "MTX_MULT_4x3"},  {0x1A, "MTX_MULT_3x3"},  {0x1B, "MTX_SCALE"},
    {0x1C, "MTX_TRANS"},     {0x20, "COLOR"},         {0x21, "NORMAL"},
    {0x22, "TEXCOORD"},      {0x23, "VTX_16"},        {0x24, "VTX_10"},
    {0x25, "VTX_XY"},        {0x26, "VTX_XZ"},        {0x27, "VTX_YZ"},
    {0x28, "VTX_DIFF"},      {0x29, "POLYGON_ATTR"},  {0x2A, "TEXIMAGE_PARAM"},
    {0x2B, "PLTT_BASE"},     {0x30, "DIF_AMB"},       {0x31, "SPE_EMI"},
    {0x32, "LIGHT_VECTOR"},  {0x33, "LIGHT_COLOR"},   {0x40, "BEGIN_VTXS"},
    {0x41, "END_VTXS"},      {0x50, "SWAP_BUFFERS"},  {0x60, "VIEWPORT"},
};
const unsigned kCmdN = sizeof kCmds / sizeof kCmds[0];
// The command rows, then PORT_WRITES, FIFO_WORDS, GX_RESETS, SWAP_PARAM,
// PROJ_IDENTITY, POS_IDENTITY, PROJ_FP.
const unsigned kCmdExtra = 7;
Slot g_cmd[kCmdN + kCmdExtra];

// A cheap order-sensitive fingerprint of a 4x4. Two frames with the same
// number here loaded the same matrix; a frame whose projection was thrown away
// and never re-loaded reads the identity fingerprint, which the row beside it
// names outright so no reader has to recognise a hash.
uint32_t mat_fp(const float m[16]) {
    uint32_t h = 2166136261u;
    for (int i = 0; i < 16; ++i) {
        uint32_t bits;
        std::memcpy(&bits, &m[i], 4);
        h = (h ^ bits) * 16777619u;
    }
    return h;
}

bool mat_is_identity(const float m[16]) {
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (m[r * 4 + c] != (r == c ? 1.0f : 0.0f)) return false;
    return true;
}

// ---- the BG SCREEN BASES, decoded (run link60 Stage 5 lane T2) --------------
//
// "BG VRAM has 9106 nonzero bytes" and "the tilemaps are all zero" are not in
// tension and the whole-region row above cannot tell them apart: character
// data and screen data share one region and the row sums both. A tilemap that
// reads zero draws nothing whatever the character data holds, so the screen
// bases get their own row, decoded the way the hardware decodes them --
// BGxCNT bits 8-12 times 2KB, plus DISPCNT bits 27-29 times 64KB on engine A
// only -- and counted separately from the character bases beside them.
struct BgBaseRow {
    uint32_t scr_base, chr_base;
    uint32_t scr_nonzero, chr_nonzero;
    uint16_t cnt;
};
BgBaseRow g_bgbase[2][4];

// ---- the assembled triangles, in SCREEN space (run link60 Stage 5 lane T2) --
//
// POLYGONS says how many triangles the engine assembled. It does not say WHERE
// they are, and "submitted but nothing rasterises" has three completely
// different causes that the count cannot separate: every triangle off the
// panel, every triangle collapsed to zero area, or every triangle correctly
// placed and rejected later. GxVertex carries x and y in framebuffer pixels
// after the perspective divide, so the answer is a bounding box and three
// tallies, taken over the list the raster was handed.
struct TriGeom {
    unsigned n;             // triangles in the list at this sample
    unsigned offscreen;     // whole triangle outside the framebuffer rectangle
    unsigned degenerate;    // |signed area| < 1/256 of a pixel
    unsigned nonfinite;     // a coordinate that is not a finite number
    unsigned drawable;      // on-screen and not degenerate
    float x0, x1, y0, y1;   // bounding box over every vertex
    float z0, z1;
};
TriGeom g_tri;

// ---- block-copy destinations (run link60 Stage 5 lane T2) -------------------
//
// The BG tilemap upload path ends in MultiCopyHalf, which hal/cxx_aliases.cpp
// hosts. Counting its destinations by region separates the two states a zero
// tilemap can be in -- never written, or written somewhere else -- which the
// VRAM census alone cannot.
struct CopyRow { uint32_t base, end; const char *name; unsigned n; uint64_t bytes; };
CopyRow g_copy[] = {
    {0x05000000, 0x05000400, "A palette",   0, 0},
    {0x05000400, 0x05000800, "B palette",   0, 0},
    {0x06000000, 0x06080000, "A BG VRAM",   0, 0},
    {0x06200000, 0x06220000, "B BG VRAM",   0, 0},
    {0x06400000, 0x06440000, "A OBJ VRAM",  0, 0},
    {0x06600000, 0x06620000, "B OBJ VRAM",  0, 0},
    {0x06800000, 0x068A4000, "LCDC / extpal", 0, 0},
    {0x07000000, 0x07000800, "OAM",         0, 0},
    {0x02000000, 0x02400000, "main RAM",    0, 0},
};
const unsigned kCopyN = sizeof g_copy / sizeof g_copy[0];
unsigned g_copy_other;
uint64_t g_copy_other_bytes;
// The distinct destinations inside engine A's BG VRAM, which is the one the
// title screen's four empty tilemaps live in. A base alone is enough to say
// which of the eight 2KB screen blocks a write went to.
uint32_t g_copy_abg[16];
uint32_t g_copy_abg_len[16];
uint32_t g_copy_abg_src[16];
uint32_t g_copy_abg_srcnz[16];
unsigned g_copy_abg_n;

struct Copy32Row { uint32_t src, dst, len, srcnz; unsigned n; };
Copy32Row g_copy32[24];
unsigned g_copy32_n, g_copy32_dropped;

uint32_t rd(uint32_t a, unsigned w) {
    return w == 4 ? *reinterpret_cast<volatile uint32_t *>(a)
                  : *reinterpret_cast<volatile uint16_t *>(a);
}

}  // namespace

bool ppu_audit_on() {
    if (g_on < 0) {
        g_path = std::getenv("SM64DS_PPU_AUDIT");
        g_on = g_path && *g_path ? 1 : 0;
    }
    return g_on != 0;
}

void ppu_audit_proxy(uint32_t addr, uint64_t value, unsigned width, bool is_write) {
    if (!ppu_audit_on()) return;
    // The 2D surface only: both engines' display blocks, plus OAM and palette
    // RAM, which are the other two things a 2D gap could hide in.
    const bool in2d = (addr >= 0x04000000u && addr < 0x04000070u)
                      || (addr >= 0x04001000u && addr < 0x04001070u)
                      || (addr >= 0x05000000u && addr < 0x05000800u)
                      || (addr >= 0x07000000u && addr < 0x07000800u);
    if (!in2d) return;
    for (unsigned i = 0; i < g_proxy_n; ++i)
        if (g_proxy[i].addr == addr && g_proxy[i].write == (is_write ? 1u : 0u)
            && g_proxy[i].value == (uint32_t)value) { ++g_proxy[i].n; return; }
    if (g_proxy_n >= kMaxProxy) { ++g_proxy_dropped; return; }
    ProxyHit &h = g_proxy[g_proxy_n++];
    h.addr = addr;
    h.value = (uint32_t)value;
    h.width = (uint8_t)width;
    h.write = is_write ? 1 : 0;
    h.n = 1;
}

void ppu_audit_note_copy(uint32_t src, uint32_t dst, uint32_t bytes) {
    if (!ppu_audit_on()) return;
    for (unsigned i = 0; i < kCopyN; ++i)
        if (dst >= g_copy[i].base && dst < g_copy[i].end) {
            ++g_copy[i].n;
            g_copy[i].bytes += bytes;
            if (g_copy[i].base == 0x06000000u) {
                for (unsigned k = 0; k < g_copy_abg_n; ++k)
                    if (g_copy_abg[k] == dst) return;
                if (g_copy_abg_n < 16) {
                    g_copy_abg[g_copy_abg_n] = dst;
                    g_copy_abg_len[g_copy_abg_n] = bytes;
                    // THE SOURCE'S NONZERO COUNT, TAKEN BEFORE THE COPY. A
                    // tilemap that reads zero after an upload has two
                    // completely different causes -- the upload carried
                    // zeros, or something cleared the map afterwards -- and
                    // this is the one number that separates them.
                    g_copy_abg_src[g_copy_abg_n] = src;
                    g_copy_abg_srcnz[g_copy_abg_n] = nonzero_bytes(src, bytes);
                    ++g_copy_abg_n;
                }
            }
            return;
        }
    ++g_copy_other;
    g_copy_other_bytes += bytes;
}

void ppu_audit_note_copy32(uint32_t src, uint32_t dst, uint32_t bytes) {
    if (!ppu_audit_on()) return;
    for (unsigned i = 0; i < g_copy32_n; ++i)
        if (g_copy32[i].src == src && g_copy32[i].dst == dst) {
            ++g_copy32[i].n;
            return;
        }
    if (g_copy32_n >= 24) { ++g_copy32_dropped; return; }
    Copy32Row &r = g_copy32[g_copy32_n++];
    r.src = src; r.dst = dst; r.len = bytes; r.n = 1;
    r.srcnz = nonzero_bytes(src, bytes);
}

void ppu_audit_sample(const char *tag) {
    if (!ppu_audit_on()) return;
    if (!g_registered) {
        g_registered = true;
        std::atexit(ppu_audit_dump);
        for (unsigned i = 0; i < kRegN; ++i)
            for (int e = 0; e < 2; ++e) g_slot[e][i].first_frame = -1;
        for (unsigned i = 0; i < kTouchN; ++i) g_touch[i].first_frame = -1;
        for (unsigned i = 0; i < k3dN; ++i) g_3d[i].first_frame = -1;
        for (unsigned i = 0; i < kCmdN + kCmdExtra; ++i)
            g_cmd[i].first_frame = -1;
    }
    if (tag && !g_tag[0]) {
        std::strncpy(g_tag, tag, sizeof g_tag - 1);
        g_tag[sizeof g_tag - 1] = 0;
    }

    for (int e = 0; e < 2; ++e) {
        const uint32_t base = e == 0 ? 0x04000000u : 0x04001000u;
        for (unsigned i = 0; i < kRegN; ++i) {
            if (!(kRegs[i].eng & (e == 0 ? E_A : E_B))) continue;
            note(g_slot[e][i], rd(base + kRegs[i].off, kRegs[i].width), g_frames);
        }
        census_oam(e);
    }
    for (unsigned i = 0; i < kTouchN; ++i)
        note(g_touch[i], rd(kTouch[i].addr, kTouch[i].width), g_frames);

    {
        int vx = 0, vy = 0, vw = 0, vh = 0, vsets = 0;
        gx_debug_viewport(vx, vy, vw, vh, vsets);
        size_t npoly = 0;
        gx_polygons(npoly);
        const uint32_t v[k3dN] = {(uint32_t)vx, (uint32_t)vy, (uint32_t)vw,
                                  (uint32_t)vh, (uint32_t)vsets,
                                  (uint32_t)npoly};
        for (unsigned i = 0; i < k3dN; ++i) note(g_3d[i], v[i], g_frames);
    }

    // The geometry command census, TAKEN so every row is this frame's.
    {
        uint32_t counts[256];
        uint32_t ports = 0, fifo = 0, swap = 0, resets = 0;
        gx_debug_commands(counts, ports, fifo, swap, resets, true);
        for (unsigned i = 0; i < kCmdN; ++i)
            note(g_cmd[i], counts[kCmds[i].cmd], g_frames);
        int mode = 0;
        float pos[16], proj[16];
        gx_debug_matrices(&mode, pos, proj);
        const uint32_t extra[kCmdExtra] = {
            ports, fifo, resets, swap,
            (uint32_t)(mat_is_identity(proj) ? 1 : 0),
            (uint32_t)(mat_is_identity(pos) ? 1 : 0),
            mat_fp(proj)};
        for (unsigned i = 0; i < kCmdExtra; ++i)
            note(g_cmd[kCmdN + i], extra[i], g_frames);
    }

    // The assembled triangle list, in screen space. Overwritten each sample;
    // the dump prints the last one, which is the frame the BMP captures.
    {
        size_t npoly = 0;
        const GxTriangle *t = gx_polygons(npoly);
        TriGeom g = {};
        g.n = (unsigned)npoly;
        g.x0 = g.y0 = g.z0 = 1e30f;
        g.x1 = g.y1 = g.z1 = -1e30f;
        for (size_t i = 0; i < npoly; ++i) {
            const GxVertex *v = t[i].v;
            bool bad = false;
            for (int k = 0; k < 3; ++k) {
                const float xs[3] = {v[k].x, v[k].y, v[k].z};
                for (int c = 0; c < 3; ++c)
                    if (!(xs[c] > -1e30f && xs[c] < 1e30f)) bad = true;
            }
            if (bad) { ++g.nonfinite; continue; }
            for (int k = 0; k < 3; ++k) {
                if (v[k].x < g.x0) g.x0 = v[k].x;
                if (v[k].x > g.x1) g.x1 = v[k].x;
                if (v[k].y < g.y0) g.y0 = v[k].y;
                if (v[k].y > g.y1) g.y1 = v[k].y;
                if (v[k].z < g.z0) g.z0 = v[k].z;
                if (v[k].z > g.z1) g.z1 = v[k].z;
            }
            const float area = (v[1].x - v[0].x) * (v[2].y - v[0].y)
                             - (v[2].x - v[0].x) * (v[1].y - v[0].y);
            const float a = area < 0 ? -area : area;
            const float lo_x = v[0].x < v[1].x ? (v[0].x < v[2].x ? v[0].x : v[2].x)
                                               : (v[1].x < v[2].x ? v[1].x : v[2].x);
            const float hi_x = v[0].x > v[1].x ? (v[0].x > v[2].x ? v[0].x : v[2].x)
                                               : (v[1].x > v[2].x ? v[1].x : v[2].x);
            const float lo_y = v[0].y < v[1].y ? (v[0].y < v[2].y ? v[0].y : v[2].y)
                                               : (v[1].y < v[2].y ? v[1].y : v[2].y);
            const float hi_y = v[0].y > v[1].y ? (v[0].y > v[2].y ? v[0].y : v[2].y)
                                               : (v[1].y > v[2].y ? v[1].y : v[2].y);
            const bool off = hi_x < 0 || lo_x >= (float)active_w
                          || hi_y < 0 || lo_y >= (float)active_h;
            if (off) ++g.offscreen;
            if (a < (1.0f / 256.0f)) ++g.degenerate;
            if (!off && a >= (1.0f / 256.0f)) ++g.drawable;
        }
        if (!npoly) { g.x0 = g.x1 = g.y0 = g.y1 = g.z0 = g.z1 = 0; }
        g_tri = g;
    }

    // The BG screen and character bases, decoded per engine. Sampled fresh
    // each time; only the LAST sample's byte counts are printed, because a
    // per-frame history of a 2KB nonzero count is noise and the question this
    // answers ("did anything ever write the tilemap") is answered by the
    // state at the end of the run.
    for (int e = 0; e < 2; ++e) {
        const uint32_t rbase = e == 0 ? 0x04000000u : 0x04001000u;
        const uint32_t vbase = e == 0 ? 0x06000000u : 0x06200000u;
        const uint32_t dispcnt = rd(rbase, 4);
        // DISPCNT bits 27-29 (screen) and 24-26 (character) are engine A only.
        const uint32_t scr_off = e == 0 ? ((dispcnt >> 27) & 7) * 0x10000u : 0;
        const uint32_t chr_off = e == 0 ? ((dispcnt >> 24) & 7) * 0x10000u : 0;
        for (int b = 0; b < 4; ++b) {
            const uint16_t cnt = (uint16_t)rd(rbase + 8 + b * 2, 2);
            BgBaseRow &r = g_bgbase[e][b];
            r.cnt = cnt;
            r.scr_base = vbase + scr_off + ((cnt >> 8) & 0x1F) * 0x800u;
            r.chr_base = vbase + chr_off + ((cnt >> 2) & 0x03) * 0x4000u;
            r.scr_nonzero = nonzero_bytes(r.scr_base, 0x800);
            r.chr_nonzero = nonzero_bytes(r.chr_base, 0x4000);
        }
    }

    ++g_frames;
    // Flush on the first sample and then periodically. A run that faults skips
    // atexit entirely, and a table that only existed at a clean exit would have
    // nothing to say about exactly the runs whose registers are most
    // interesting. The first-sample flush also distinguishes "the seam never
    // ran" (no file at all) from "it ran and everything was zero".
    if (g_frames == 1 || (g_frames & 31) == 0) ppu_audit_dump();
}

void ppu_audit_dump() {
    if (!ppu_audit_on() || !g_frames) return;
    std::FILE *f = std::fopen(g_path, "w");
    if (!f) return;

    std::fprintf(f, "2D REGISTER AUDIT  seam=%s  samples=%d\n", g_tag, g_frames);
    std::fprintf(f, "M = the live path for that engine reads it; . = it does not.\n");
    std::fprintf(f, "A live path = hal/message_compositor.cpp, B = ntr/ppu_sub.cpp.\n\n");

    for (int e = 0; e < 2; ++e) {
        std::fprintf(f, "-- ENGINE %c (0x0400%04x) --\n", e == 0 ? 'A' : 'B',
                     e == 0 ? 0 : 0x1000);
        std::fprintf(f, "%-16s %-4s %-2s %-8s %-6s %s\n", "reg", "off", "m",
                     "nonzero", "first", "values seen (value xN)");
        for (unsigned i = 0; i < kRegN; ++i) {
            if (!(kRegs[i].eng & (e == 0 ? E_A : E_B))) continue;
            const Slot &s = g_slot[e][i];
            const bool mod = (kRegs[i].modelled & (e == 0 ? E_A : E_B)) != 0;
            // Everything unmodelled is printed even when dead, because "dead"
            // is the finding for most of the gap list. A modelled register is
            // printed only when it carried something.
            if (mod && !s.nonzero) continue;
            std::fprintf(f, "%-16s %04x %-2s %-8u ", kRegs[i].name, kRegs[i].off,
                         mod ? "M" : ".", s.nonzero);
            if (s.first_frame < 0) std::fprintf(f, "%-6s ", "-");
            else std::fprintf(f, "%-6d ", s.first_frame);
            for (unsigned k = 0; k < s.n; ++k)
                std::fprintf(f, "%0*x x%u%s", kRegs[i].width * 2, s.val[k],
                             s.hits[k], k + 1 < s.n ? ", " : "");
            if (s.n == kMaxVals) std::fprintf(f, " (+more)");
            std::fprintf(f, "\n");
        }
        const ObjCensus &c = g_obj[e];
        static const char *kModeName[4] = {"normal", "semi-transparent",
                                           "obj-window", "bitmap"};
        std::fprintf(f, "OAM object modes (enabled entries, summed over samples):\n");
        for (int m = 0; m < 4; ++m)
            std::fprintf(f, "  mode %d %-17s %8u  in %u/%d samples\n", m,
                         kModeName[m], c.mode[m], c.frames_with[m], g_frames);
        std::fprintf(f, "  mosaic bit set             %8u\n", c.mosaic);

        static const int kSizes[3][4][2] = {
            {{8, 8}, {16, 16}, {32, 32}, {64, 64}},
            {{16, 8}, {32, 8}, {32, 16}, {64, 32}},
            {{8, 16}, {8, 32}, {16, 32}, {32, 64}},
        };
        std::fprintf(f, "OAM enabled-entry attributes, distinct triples:\n");
        std::fprintf(f, "  %-4s %-4s %-4s %-6s %-4s %-4s %-7s %-4s %-3s %-4s "
                        "%-4s %s\n", "at0", "at1", "at2", "xN", "y", "x",
                     "size", "col", "pal", "tile", "prio", "aff");
        for (unsigned k = 0; k < g_attr_n[e]; ++k) {
            const ObjAttr &a = g_attr[e][k];
            const int shape = (a.a0 >> 14) & 3, sz = (a.a1 >> 14) & 3;
            const bool affine = (a.a0 & 0x100) != 0;
            int y = a.a0 & 0xFF, x = a.a1 & 0x1FF;
            if (x >= 256) x -= 512;
            char size[8];
            if (shape == 3) std::strcpy(size, "invalid");
            else std::sprintf(size, "%dx%d", kSizes[shape][sz][0],
                              kSizes[shape][sz][1]);
            std::fprintf(f, "  %04x %04x %04x x%-5u %-4d %-4d %-7s %-4s %-3u "
                            "%-4u %-4u %s\n", a.a0, a.a1, a.a2, a.n, y, x, size,
                         (a.a0 & 0x2000) ? "256" : "16",
                         (unsigned)((a.a2 >> 12) & 0xF), (unsigned)(a.a2 & 0x3FF),
                         (unsigned)((a.a2 >> 10) & 3),
                         affine ? ((a.a0 & 0x200) ? "dbl" : "yes") : "-");
        }
        if (g_attr_dropped[e])
            std::fprintf(f, "  (%u further distinct triples dropped, table "
                            "full)\n", g_attr_dropped[e]);

        std::fprintf(f, "OAM FIRST SAMPLE, every enabled entry, nothing "
                        "dropped (%u entr%s):\n", g_first_n[e],
                     g_first_n[e] == 1 ? "y" : "ies");
        std::fprintf(f, "  %-4s %-4s %-4s %-4s %-4s %-4s %-7s %-4s %-3s %-4s "
                        "%-4s %s\n", "#", "at0", "at1", "at2", "y", "x",
                     "size", "col", "pal", "tile", "prio", "aff");
        for (unsigned k = 0; k < g_first_n[e]; ++k) {
            const ObjFirst &a = g_first[e][k];
            const int shape = (a.a0 >> 14) & 3, sz = (a.a1 >> 14) & 3;
            const bool affine = (a.a0 & 0x100) != 0;
            int y = a.a0 & 0xFF, x = a.a1 & 0x1FF;
            if (x >= 256) x -= 512;
            char size[8];
            if (shape == 3) std::strcpy(size, "invalid");
            else std::sprintf(size, "%dx%d", kSizes[shape][sz][0],
                              kSizes[shape][sz][1]);
            std::fprintf(f, "  %-4u %04x %04x %04x %-4d %-4d %-7s %-4s %-3u "
                            "%-4u %-4u %s\n", (unsigned)a.idx, a.a0, a.a1, a.a2,
                         y, x, size, (a.a0 & 0x2000) ? "256" : "16",
                         (unsigned)((a.a2 >> 12) & 0xF), (unsigned)(a.a2 & 0x3FF),
                         (unsigned)((a.a2 >> 10) & 3),
                         affine ? ((a.a0 & 0x200) ? "dbl" : "yes") : "-");
        }

        std::fprintf(f, "Decode inputs, nonzero bytes right now:\n");
        const char *dump = std::getenv("SM64DS_PPU_AUDIT_DUMP");
        for (unsigned k = 0; k < kObjSrcN; ++k) {
            if (!(kObjSrc[k].eng & (e == 0 ? E_A : E_B))) continue;
            std::fprintf(f, "  %-18s %08x +%06x  %u\n", kObjSrc[k].name,
                         kObjSrc[k].base, kObjSrc[k].size,
                         nonzero_bytes(kObjSrc[k].base, kObjSrc[k].size));
            if (dump && *dump && !g_dumped) dump_span(dump, e, kObjSrc[k]);
        }
        if (e == 0)
            std::fprintf(f, "  (no OBJ ext palette row: this tree has no "
                            "GX::LoadOBJExtPltt TU to derive a base from)\n");
        std::fprintf(f, "\n");
    }

    std::fprintf(f, "-- ENGINE A 3D LAYER (the other half of the top screen) --\n");
    std::fprintf(f, "Viewport is in HOST framebuffer pixels; the 2D table above\n");
    std::fprintf(f, "is in DS panel pixels. DISPCNT bit 8 (BG0 enable) plus bit 3\n");
    std::fprintf(f, "(BG0 in 3D mode) is what puts this layer in the picture at all.\n");
    std::fprintf(f, "READ VIEWPORT_SETS FIRST: gx_reset restores a FULL-SCREEN\n");
    std::fprintf(f, "default every frame, so at 0 sets the rectangle below is that\n");
    std::fprintf(f, "default and says nothing about what the game asked for.\n");
    for (unsigned i = 0; i < k3dN; ++i) {
        const Slot &s = g_3d[i];
        std::fprintf(f, "%-16s      %-2s %-8u ", k3dNames[i], ".", s.nonzero);
        if (s.first_frame < 0) std::fprintf(f, "%-6s ", "-");
        else std::fprintf(f, "%-6d ", s.first_frame);
        for (unsigned k = 0; k < s.n; ++k)
            std::fprintf(f, "%u x%u%s", s.val[k], s.hits[k],
                         k + 1 < s.n ? ", " : "");
        if (s.n == kMaxVals) std::fprintf(f, " (+more)");
        std::fprintf(f, "\n");
    }
    std::fprintf(f, "\n");

    std::fprintf(f, "-- ENGINE A 3D COMMAND CENSUS (per frame, taken not read) --\n");
    std::fprintf(f, "Counts are commands EXECUTED in the frame each sample ends.\n");
    std::fprintf(f, "PORT_WRITES is stores that reached ntr::gx_write_port and\n");
    std::fprintf(f, "FIFO_WORDS is words through gx_write_fifo. A translation unit\n");
    std::fprintf(f, "that writes a GX command register with a plain store to the DS\n");
    std::fprintf(f, "address reaches NEITHER: ntr maps real memory there, the store\n");
    std::fprintf(f, "latches, and the geometry engine never sees the command. So a\n");
    std::fprintf(f, "zero row here is not 'the game did not ask' -- it is 'nothing\n");
    std::fprintf(f, "the game asked for arrived', and the two need separating by\n");
    std::fprintf(f, "reading the source of the caller, not this table.\n");
    std::fprintf(f, "PROJ_IDENTITY 1 means the projection matrix at scan-out is the\n");
    std::fprintf(f, "identity, which for a perspective scene means it was never\n");
    std::fprintf(f, "loaded or was thrown away; GX_RESETS says how many times the\n");
    std::fprintf(f, "engine state was cleared in the interval.\n");
    for (unsigned i = 0; i < kCmdN + kCmdExtra; ++i) {
        const Slot &s = g_cmd[i];
        const char *name;
        char buf[24];
        if (i < kCmdN) {
            std::sprintf(buf, "%02x %s", kCmds[i].cmd, kCmds[i].name);
            name = buf;
        } else {
            static const char *const kExtra[kCmdExtra] = {
                "PORT_WRITES", "FIFO_WORDS", "GX_RESETS", "SWAP_PARAM",
                "PROJ_IDENTITY", "POS_IDENTITY", "PROJ_FP"};
            name = kExtra[i - kCmdN];
        }
        std::fprintf(f, "%-19s %-2s %-8u ", name, ".", s.nonzero);
        if (s.first_frame < 0) std::fprintf(f, "%-6s ", "-");
        else std::fprintf(f, "%-6d ", s.first_frame);
        for (unsigned k = 0; k < s.n; ++k)
            std::fprintf(f, "%u x%u%s", s.val[k], s.hits[k],
                         k + 1 < s.n ? ", " : "");
        if (s.n == kMaxVals) std::fprintf(f, " (+more)");
        std::fprintf(f, "\n");
    }
    std::fprintf(f, "\n");

    std::fprintf(f, "-- 3D TRIANGLES IN SCREEN SPACE, LAST SAMPLE --\n");
    std::fprintf(f, "Framebuffer is %dx%d. x and y are post-divide pixels, so a\n",
                 (int)active_w, (int)active_h);
    std::fprintf(f, "box outside it is geometry the raster cannot reach and a\n");
    std::fprintf(f, "degenerate count equal to the triangle count is a collapsed\n");
    std::fprintf(f, "transform. Both look identical from POLYGONS alone.\n");
    std::fprintf(f, "  triangles   %u\n", g_tri.n);
    std::fprintf(f, "  drawable    %u   (on-screen and not degenerate)\n",
                 g_tri.drawable);
    std::fprintf(f, "  offscreen   %u\n", g_tri.offscreen);
    std::fprintf(f, "  degenerate  %u\n", g_tri.degenerate);
    std::fprintf(f, "  nonfinite   %u\n", g_tri.nonfinite);
    std::fprintf(f, "  bbox        x %.3f .. %.3f   y %.3f .. %.3f   z %.5f .. %.5f\n",
                 g_tri.x0, g_tri.x1, g_tri.y0, g_tri.y1, g_tri.z0, g_tri.z1);
    {
        int mode = 0;
        float pos[16], proj[16];
        gx_debug_matrices(&mode, pos, proj);
        std::fprintf(f, "  matrix mode %d (0 proj, 1 pos, 2 pos+vec, 3 tex)\n", mode);
        for (int r = 0; r < 4; ++r)
            std::fprintf(f, "  proj  %12.6f %12.6f %12.6f %12.6f\n",
                         proj[r * 4 + 0], proj[r * 4 + 1], proj[r * 4 + 2],
                         proj[r * 4 + 3]);
        for (int r = 0; r < 4; ++r)
            std::fprintf(f, "  pos   %12.6f %12.6f %12.6f %12.6f\n",
                         pos[r * 4 + 0], pos[r * 4 + 1], pos[r * 4 + 2],
                         pos[r * 4 + 3]);
    }
    std::fprintf(f, "\n");

    std::fprintf(f, "-- BG SCREEN AND CHARACTER BASES, LAST SAMPLE --\n");
    std::fprintf(f, "Decoded the way the hardware decodes them: screen base is\n");
    std::fprintf(f, "BGxCNT bits 8-12 x 2KB, character base is bits 2-3 x 16KB, plus\n");
    std::fprintf(f, "DISPCNT bits 27-29 / 24-26 x 64KB on engine A. The whole-region\n");
    std::fprintf(f, "'BG VRAM' row above sums both, so a layer with character data\n");
    std::fprintf(f, "and an EMPTY TILEMAP reads healthy there and draws nothing.\n");
    for (int e = 0; e < 2; ++e) {
        for (int b = 0; b < 4; ++b) {
            const BgBaseRow &r = g_bgbase[e][b];
            std::fprintf(f, "  %c BG%d  CNT %04x  screen %08x %4u/2048 nonzero"
                            "   char %08x %5u/16384 nonzero\n",
                         e == 0 ? 'A' : 'B', b, r.cnt, r.scr_base,
                         r.scr_nonzero, r.chr_base, r.chr_nonzero);
        }
    }
    std::fprintf(f, "\n");

    std::fprintf(f, "-- TOUCH HARDWARE SURFACE --\n");
    std::fprintf(f, "The stylus record TouchInfo[4] is NOT here: it is a hosted\n");
    std::fprintf(f, "global, already driven from the mouse by poll_touch.\n");
    for (unsigned i = 0; i < kTouchN; ++i) {
        const Slot &s = g_touch[i];
        std::fprintf(f, "%-16s %08x nonzero=%-6u ", kTouch[i].name, kTouch[i].addr,
                     s.nonzero);
        for (unsigned k = 0; k < s.n; ++k)
            std::fprintf(f, "%0*x x%u%s", kTouch[i].width * 2, s.val[k], s.hits[k],
                         k + 1 < s.n ? ", " : "");
        std::fprintf(f, "\n");
    }

    std::fprintf(f, "-- MultiCopyHalf DESTINATIONS (exact, whole run) --\n");
    std::fprintf(f, "Where the game's halfword block copies landed. The BG tilemap\n");
    std::fprintf(f, "upload path (func_ov007_020c076c -> func_020565xx ->\n");
    std::fprintf(f, "G2::GetBGxScrPtr) ends in this primitive, so a zero row against\n");
    std::fprintf(f, "engine A's BG VRAM means nothing ever TRIED to write the map.\n");
    for (unsigned i = 0; i < kCopyN; ++i)
        if (g_copy[i].n)
            std::fprintf(f, "  %-16s %u copies, %llu bytes\n", g_copy[i].name,
                         g_copy[i].n, (unsigned long long)g_copy[i].bytes);
    if (g_copy_other)
        std::fprintf(f, "  %-16s %u copies, %llu bytes\n", "elsewhere",
                     g_copy_other, (unsigned long long)g_copy_other_bytes);
    if (!g_copy_abg_n)
        std::fprintf(f, "  engine A BG VRAM: NO destination at all\n");
    for (unsigned k = 0; k < g_copy_abg_n; ++k)
        std::fprintf(f, "  A BG VRAM dst %08x  %u bytes  from %08x, %u of them "
                        "nonzero AT COPY TIME\n", g_copy_abg[k],
                     g_copy_abg_len[k], g_copy_abg_src[k], g_copy_abg_srcnz[k]);
    std::fprintf(f, "\n");

    std::fprintf(f, "-- MultiCopy32Bytes MOVES (distinct src/dst, whole run) --\n");
    std::fprintf(f, "One step before the VRAM upload. `srcnz` is the source's nonzero\n");
    std::fprintf(f, "bytes AT THE FIRST MOVE, so a chain that reaches VRAM empty can\n");
    std::fprintf(f, "be walked back to the step that was supposed to fill it.\n");
    if (!g_copy32_n) std::fprintf(f, "  none\n");
    for (unsigned i = 0; i < g_copy32_n; ++i)
        std::fprintf(f, "  %08x -> %08x  %u bytes  srcnz %u  x%u\n",
                     g_copy32[i].src, g_copy32[i].dst, g_copy32[i].len,
                     g_copy32[i].srcnz, g_copy32[i].n);
    if (g_copy32_dropped)
        std::fprintf(f, "  (%u further distinct pairs dropped, table full)\n",
                     g_copy32_dropped);
    std::fprintf(f, "\n");

    std::fprintf(f, "\n-- io.cpp PROXY HITS on the 2D surface (exact, a strict subset) --\n");
    if (!g_proxy_n) {
        std::fprintf(f, "none. Every 2D access in this run reached mapped memory\n");
        std::fprintf(f, "directly, which is why the rest of this table is sampled.\n");
    }
    for (unsigned i = 0; i < g_proxy_n; ++i)
        std::fprintf(f, "%s %08x w%u = %08x  x%u\n", g_proxy[i].write ? "wr" : "rd",
                     g_proxy[i].addr, g_proxy[i].width, g_proxy[i].value, g_proxy[i].n);
    if (g_proxy_dropped)
        std::fprintf(f, "(%u further distinct hits dropped, table full)\n", g_proxy_dropped);

    g_dumped = true;
    std::fclose(f);
}

}  // namespace ntr
