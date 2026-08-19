// DS 2D engine scan-out.
//
// The 2D engines are pure register state: DISPCNT and BGxCNT say where the
// tilemaps and tiles live, and VRAM holds them. Nothing about drawing is
// write-triggered, so the host can simply read the register file once per frame
// and rasterise -- no interception needed, which is why 68% of the decomp's
// hardware accesses need no more than a working memory map to behave correctly.
//
// See notes/assessment.md section 2a and docs/mmio-inventory.md.

#ifndef NTR_PPU_H
#define NTR_PPU_H

#include <stdint.h>

namespace ntr {

#ifdef NTR_HIRES
constexpr int SCREEN_W = 1024;   /* 4x the DS panel; the smokes and their
                                    reference pixel counts stay on 256x192
                                    (the plain ntr lib) */
constexpr int SCREEN_H = 768;
#elif defined(NTR_HIRES2)
constexpr int SCREEN_W = 512;    /* 2x: the interactive window's tier */
constexpr int SCREEN_H = 384;
#else
constexpr int SCREEN_W = 256;
constexpr int SCREEN_H = 192;
#endif

enum Engine { ENGINE_A = 0, ENGINE_B = 1 };

struct Framebuffer {
    uint32_t px[SCREEN_H][SCREEN_W];   // 0xAARRGGBB
};

// Rasterise one engine's current register state into fb. Text-mode BGs only for
// now; affine, bitmap, sprites and the blend/window units are not implemented.
void ppu_scanout(Engine eng, Framebuffer &fb);

// Rasterise one engine's OBJ layer (sprites) from its OAM + OBJ VRAM over
// whatever fb already holds. Plain and affine sprites, 16- and 256-color
// tiles, 1D mapping; bitmap OBJs, mosaic, windows and blending are not
// implemented. Transparent color-0 texels leave fb untouched.
void ppu_scanout_obj(Engine eng, Framebuffer &fb);

// Debug output, so a frame can be inspected without a window yet.
bool ppu_write_bmp(const char *path, const Framebuffer &fb);

// The same writer with the size in arguments, for an image that is not a
// framebuffer. The stacked presentation below is the reason it exists.
bool ppu_write_bmp_px(const char *path, const uint32_t *px, int w, int h);

// ---- the bottom screen ------------------------------------------------------
//
// The sub engine is a tile raster whose every address is a DS pixel, so unlike
// the 3D top screen it exists at exactly one size. It gets its own fixed
// framebuffer and its own compositor (port/ntr/ppu_sub.cpp), and is scaled --
// or, as the port does it, not scaled -- only when it reaches the window.
constexpr int SUB_W = 256;
constexpr int SUB_H = 192;

struct SubFramebuffer {
    uint32_t px[SUB_H][SUB_W];   // 0xAARRGGBB
};

// Engine B, whole: text/affine/extended-affine backgrounds with extended
// palettes, sprites, all three windows (WIN0, WIN1 and the OBJ window) and
// master brightness, all resolved together by priority. No BLDCNT blending, no
// bitmap OBJs, no bitmap BGs, no mosaic.
void ppu_scanout_sub(SubFramebuffer &fb);

bool ppu_write_bmp_sub(const char *path, const SubFramebuffer &fb);

// Blit the bottom screen 1:1 into the bottom-right corner of a dst_w x dst_h
// ARGB buffer, `margin` pixels in from both edges, with a one-pixel frame.
/* div: integer downscale of the panel (1 = 1:1 DS pixels, 2 = half size).
   Downscaled pixels are the box average of the div x div source block, so the
   minimap's 1px marks survive as shading rather than vanishing. */
void ppu_compose_sub(const SubFramebuffer &sub, uint32_t *dst, int dst_w,
                     int dst_h, int margin, int div = 1);

// ---- the stacked presentation -----------------------------------------------
//
// BOTH DS SCREENS AT THE SAME SIZE, top above bottom, which is the shape a
// touchscreen game needs and the corner panel above cannot give it. The panel
// exists to keep the bottom screen out of the way during a level, where the
// player's hands are on the keyboard; a minigame is played ON the bottom
// screen and a 128x96 preview of it is not something anyone can aim at.
//
// The image is SCREEN_W x 2*SCREEN_H: the framebuffer copied into the top half
// unchanged, and the 256x192 sub framebuffer scaled into the bottom half by
// the integer ratio SCREEN_W / SUB_W -- 1 at the plain tier, 2 at NTR_HIRES2,
// 4 at NTR_HIRES. Nearest neighbour, deliberately: every tier's ratio is a
// whole number, so there is nothing to interpolate and a filter would only
// invent pixels the DS never drew.
//
// THERE IS A SEPARATOR BAND WHEN, AND ONLY WHEN, THE GAME ASKS FOR ONE. See
// StackLayout below. With no gap the halves are exactly equal and touch back to
// back, which is the shape this note was written for and is still what every
// level and every scene with no gap gets.
//
// evy / to_white are the MAIN engine's master-brightness fade as
// port_fader_blend_state reports it, applied to the BOTTOM half only. The top
// half arrives already faded, because it is the framebuffer after walk_window's
// own fade composite has run over it. This reproduces what the corner panel
// gets today -- the panel is inside the framebuffer when that loop runs, so
// the main engine's fade lands on it -- so switching layout changes the
// layout and nothing else. Pass evy 0 for no fade.
constexpr int STACK_W = SCREEN_W;
// The image with NO gap, which is what every level and every gapless scene
// composes and what the shape of this presentation was before the gap existed.
// It is NOT the size of the buffer any more -- read that off a StackLayout.
constexpr int STACK_H = SCREEN_H * 2;

// The largest simulated gap this presentation will honour, in DS rows. The
// game's own values run 0, 16, 32, 48 and 80 (the snowball minigame is the 80);
// the cap is here so a wild read of the framework word cannot ask for a buffer
// the size of the desktop, and it is well clear of every value the ROM sets.
constexpr int GAP_DS_MAX = 96;

// ---- THE ONE LAYOUT ---------------------------------------------------------
//
// EVERY consumer of the stacked image derives its geometry from one of these
// and none of them recomputes it: the compose writes at these offsets, the DIB
// header takes this height, the BMP writer takes this size, the window sizes
// itself to it, and the stylus inverse (hal_present_client_to_fb and
// hal_present_client_to_sub) splits the source image into bands with it.
//
// That last one is the reason the struct exists rather than a pair of loose
// ints. Before the gap the inverse could say "the bottom screen starts one
// screen height down" and be right by construction. With a band between the
// halves that sentence is wrong by exactly the band's height, and a display
// and a touch transform that disagree by 96 client pixels is the defect this
// shape is built to make impossible: there is one arithmetic, computed once,
// and the blit and the mapper read the same fields of it.
//
// THE INPUTS ARE (G, the settings). The client size is NOT one of them, and
// that is deliberate: the fit from this image into a client area already lives
// in walk_window's present() and its inverse in client_to_src, and both of
// those already take the source size as an argument. So the chain is
//   G + settings -> StackLayout -> (present fit | inverse | DIB | BMP)
// and a resize re-runs the second arrow only.
//
// gap_ds == 0 is the layout that shipped before this: h == STACK_H, band_h ==
// 0, bottom_y == SCREEN_H, and every band-shaped branch in the compose is
// skipped so the image is byte-for-byte what it was.
struct StackLayout {
    int gap_ds;        // G, the simulated gap in DS rows. 0 = no gap.
    int scale;         // host rows per DS row: SCREEN_H / SUB_H
    int w;             // the image width, STACK_W
    int h;             // the image height, SCREEN_H * 2 + band_h
    int top_y;         // first row of the top screen, always 0
    int band_y;        // first row of the gap band, always SCREEN_H
    int band_h;        // gap_ds * scale, host rows; 0 with no gap
    int bottom_y;      // first row of the bottom screen, SCREEN_H + band_h
    int fill_mode;     // GAP_FILL_SOLID or GAP_FILL_AMBIENT
    uint32_t fill_color;   // 0xFFRRGGBB, GAP_FILL_SOLID only
    int peek;          // 1: draw the band's own hidden sprites over the fill
};

enum { GAP_FILL_SOLID = 0, GAP_FILL_AMBIENT = 1 };

// The one computation. gap_ds is clamped to [0, GAP_DS_MAX]; everything else
// is derived. Pure: same arguments, same answer, no globals read.
StackLayout stack_layout(int gap_ds, int fill_mode, uint32_t fill_color,
                         int peek);

// `top` is SCREEN_W x SCREEN_H row-major -- a Framebuffer's px, taken as a
// plain pointer because the one caller reaches this across an extern "C" seam
// (hal/sub_screen.cpp) and a reference would only be a cast in disguise there.
//
// dst must be lay.w x lay.h. The whole of it is written, band included, so the
// caller does not have to clear it.
void ppu_compose_stacked(const uint32_t *top, const SubFramebuffer &sub,
                         uint32_t *dst, int dst_w, int dst_h, int evy,
                         int to_white, const StackLayout &lay);

}  // namespace ntr

#endif  // NTR_PPU_H
