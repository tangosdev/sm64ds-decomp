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
//
// ---- THE HEADROOM, and it exists only in gapless mode -----------------------
//
// GaplessMinigames writes ZERO into the game's own G (hal/screen_gap.h states
// the mod and its audit). The world a minigame simulates is still 192 + G_rom
// + 192 rows tall, because the actors are placed by FIXED constants -- Bob-omb
// Squad's parachute spawn is a flat world -256, and its Lakitu cruises with the
// top of its sprite on world row -192 -- but with G zeroed the top engine takes
// world y to y + 192 instead of y + 224, so the world's top G_rom rows now fall
// ABOVE the top screen's first row and are drawn by nobody. Lakitu ends up flat
// against the first row of the picture and loses rows off the top whenever it
// rises, which is what a player reports as "the Lakitu is cut off at the top".
//
// The headroom is the display's answer and NOTHING ELSE: G_rom extra rows of
// IMAGE above the top screen, so the full original world -224..191 is one
// continuous picture again. The game's constants and the zeroed G are not
// touched, the simulation is bit-for-bit what gapless already was, and the
// image is the same total height gap-on mode has -- the rows are simply on top
// instead of in the middle.
//
// head_ds is 0 in every other layout, which is every level and every gap-on
// minigame, and a zero headroom makes every field below exactly what it was
// before this existed.
struct StackLayout {
    int gap_ds;        // G, the simulated gap in DS rows. 0 = no gap.
    int head_ds;       // the headroom in DS rows above the top screen. 0 = none.
    int scale;         // host rows per DS row: SCREEN_H / SUB_H
    int w;             // the image width, STACK_W
    int h;             // the image height, head_h + SCREEN_H * 2 + band_h
    int head_h;        // head_ds * scale, host rows; 0 with no headroom
    int top_y;         // first row of the top screen, head_h
    int band_y;        // first row of the gap band, head_h + SCREEN_H
    int band_h;        // gap_ds * scale, host rows; 0 with no gap
    int bottom_y;      // first row of the bottom screen, band_y + band_h
    int fill_mode;     // GAP_FILL_SOLID, _AMBIENT or _CUSTOM; peek ignores it
    uint32_t fill_color;   // 0xFFRRGGBB, GAP_FILL_SOLID only
    // 1: the band's own hidden sprites, drawn OVER the backdrop. PEEK
    // IGNORES THE FILL MODE: with peek on the band is a view of what is
    // really in those rows, not decoration, and an ambient wash or a
    // chosen colour behind it would put something over the answer -- a
    // wash blurs into the sprites it sits behind, and a flat colour cannot
    // be told apart from a sprite of the same colour. What peek does allow
    // behind the sprites is the scene's own hand-drawn art, because a
    // picture is read as a picture and not mistaken for content, and BLACK
    // when there is no art. See band_fill.
    int peek;
    // 1: THE TWO HALVES ARE ONE CONTINUOUS PICTURE and the seam between them
    // is a joint rather than a boundary, because the GAME's own G has been
    // set to zero as well as the layout's. That is the GaplessMinigames mod
    // and nothing else sets it: with the mod off, or on a scene it does not
    // cover, this is 0 even when gap_ds is 0, because a level composes with
    // no band and is still two separate screens.
    //
    // NOT DERIVABLE FROM gap_ds, which is why it is its own field. gap_ds is
    // 0 for every level in the game and for the MinigameGap-off picture, and
    // in both of those the game is still submitting sprites against a real G
    // -- so joining a sprite across the seam there would be inventing a
    // continuity the game does not have. The two conditions are read together
    // by the seam straddle pass below.
    //
    // Set per frame rather than at the layout latch (hal/screen_gap.cpp): the
    // mod engages from the running minigame's InitResources, which can be
    // after the first latch for that scene, and a cached 0 would leave the
    // pass off for the whole game.
    int seam;
    // THE SCENE'S HAND-DRAWN BAND, or null. 256 * gap_ds DS pixels, row
    // major, top row first, 0xAARRGGBB, and alpha 0 means the magenta key:
    // the artist's "leave this pixel to whatever is behind me".
    //
    // Owned by whoever built the layout (hal/gap_art.cpp in the game) and
    // valid until the next latch, which is also when the layout is next
    // rebuilt, so the pointer and the picture cannot fall out of step.
    //
    // It is an INPUT to the layout rather than something the compose looks
    // up, for the same reason G is: the compose does one arithmetic on
    // values it was handed, and every consumer that has to agree with it
    // reads the same struct.
    const uint32_t *art;
};

enum { GAP_FILL_SOLID = 0, GAP_FILL_AMBIENT = 1, GAP_FILL_CUSTOM = 2 };

// The one computation. gap_ds and head_ds are each clamped to [0, GAP_DS_MAX];
// everything else is derived. Pure: same arguments, same answer, no globals
// read.
StackLayout stack_layout(int gap_ds, int head_ds, int fill_mode,
                         uint32_t fill_color, int peek, const uint32_t *art);

// ---- BAND CONTINUITY --------------------------------------------------------
//
// ONE OBJECT THE GAME IS STILL SIMULATING WHILE NEITHER ENGINE DRAWS IT.
//
// The ROM's own OAM cull (see the note over band_raster_engine) leaves a hole:
// a sprite h rows tall whose origin sits at band row 1 .. G-h-1 is submitted to
// the main engine as y = 192 + k, which its `y > 0xc0` test throws away, and to
// the sub engine as y = -G + k, which its `y + h < 0` test throws away. So for
// those G - h - 1 rows the object is not drawn AT ALL, on hardware as much as
// here, and the peek pass reproduces that by construction because it draws only
// what the engines were given.
//
// A player following an object across the band wants it to stay visible anyway,
// and that is what this is for. The game names, per scene, the objects it wants
// kept continuous and where each one is RIGHT NOW, and the peek pass re-renders
// them from THE OAM ENTRY THE GAME ITSELF LAST SUBMITTED -- same tile, same
// palette, same size, same flip, same engine's VRAM -- into band pixels neither
// engine wrote. NOTHING IS INVENTED: an object that has not been submitted yet
// in this scene has no cached entry and is not drawn, and a pixel an engine did
// write is never overpainted.
//
// The hook fills `out` and returns how many it wrote. Coordinates are DS pixels
// in the BAND's own frame: x is the sprite's left edge on the 256-pixel row, y
// is its top edge as a band row index, 0 being world row -G and gap_ds-1 being
// world row -1. Either may sit outside the band -- an object entering it has a
// negative y -- and the pass clips.
//
// `slot` is the caller's own stable id for the object, 0..BAND_TRACK_MAX-1, and
// it is what the attribute cache is keyed on, so one object must keep one slot
// for as long as the scene runs.
struct BandTrack {
    int slot;
    int x, y;
    int w, h;      // the sprite's size in DS pixels, for the identity match
};

enum { BAND_TRACK_MAX = 8 };

// Called once per peek frame, after both engines have rasterised. Null clears
// the hook, and no hook is the behaviour this shipped with.
typedef int (*BandTrackFn)(BandTrack *out, int max, int gap_ds);
void ppu_band_continuity(BandTrackFn fn);

// ---- THE SEAM STRADDLE, and it is the same hook read at gap_ds == 0 ---------
//
// THE OTHER HALF OF THE SAME HOLE. Everything above is about the band, the G
// rows between the screens that neither engine can address. With
// GaplessMinigames on there is no band -- G is zero in the game as well as in
// the layout -- and the hole does not go away, it shrinks to the seam itself.
//
// The ROM's OAM router (src/func_ov004_020aff38.cpp) picks ONE engine by a
// band test on world y and returns. At G = 0 its two tests are y in [-256, -1]
// for the top engine and y in [-64, 191] for the bottom, so a sprite is
// submitted to exactly one of them and each engine's own raster then clips it
// at its own screen edge. A 16x16 sprite whose box crosses world row 0 is
// therefore drawn on ONE screen only, and the rows hanging over the seam are
// drawn NOWHERE -- until the object's centre crosses and the whole sprite pops
// to the other screen. On hardware the hinge hid that; with the halves edge to
// edge it is the most visible thing on the screen.
//
// So the pass: for a TRACKED object -- the same per-scene hook above, read
// with gap_ds 0, where a BandTrack's y IS the world row of the sprite box's
// top edge -- ask which HALF OF THE COMPOSED IMAGE is showing it, and if
// exactly one is and its box crosses world row 0, re-render that half's own
// OAM entry into the rows on the other side of the seam. Same tile, same
// palette, same size, same flip, same engine's VRAM, at that entry's own
// position. NOTHING IS INVENTED here either: an object neither half has draws
// nothing, and an object BOTH halves have is left alone -- which is what a
// 32x32 double-size bob-omb through func_ov004_020b023c already is, because
// that router submits to both OAM buffers and lets each engine clip its own
// side.
//
// It runs only when StackLayout::seam is set, so it is the mod's own
// behaviour and no level, no ordinary minigame and no MinigameGap-off picture
// can reach it.
//
// SM64DS_GAP_STRADDLE_TRACE=1 prints the per-frame census this was proven
// with: for every tracked crossing object, the pixels of it standing in each
// half of the composed image BEFORE the pass and AFTER it, plus a walk of both
// engines' OAM for every entry that crosses the seam and which engines have
// it.

// WHAT THE TOP SCREEN IS SHOWING. Call this immediately before the OAM upload,
// which is once per frame at hal/sub_screen.cpp's OAM::Load, and the pass gets
// the engine A OAM the engine A compositor really rasterised from -- see THE
// TWO SCREENS ARE ONE FRAME APART in ntr/ppu_sub.cpp, which is a property of
// this port's frame order and not of this pass. Costs one 1 KB copy per frame
// and nothing else; a program that never calls it leaves the pass with no top
// half to complete from, which is a pass that declines rather than one that
// guesses.
void ppu_seam_oam_mark(void);

// ---- THE AMBIENT FILL'S MEMORY ----------------------------------------------
//
// The ambient band does not read the two edge rows cold any more; it follows
// them, fast towards a disturbance and slowly back. See band_fill_ambient in
// ntr/ppu_sub.cpp for what that is and why. This DROPS the memory, so the next
// scene's band is lit by the next scene and not by a fade of the last one's,
// and the first frame after it is the direct sample exactly.
//
// Called from the same place ppu_band_continuity is, at hal/screen_gap.cpp's
// latch, for the same reason: per-scene state whose lifetime is the scene.
void ppu_band_ambient_reset(void);

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
