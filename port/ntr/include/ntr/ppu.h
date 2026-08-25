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
/* Engine A OAM source override for the gapless seam: the address of the
   game's working shadow to draw the CURRENT frame's sprites, or 0 for the
   uploaded hardware OAM (the default, one frame behind the bottom screen).
   Set per frame by hal/screen_gap.cpp; see the note over the definition. */
void ppu_obj_oam_source_a(uint32_t addr);
void ppu_obj_oam_source_b(uint32_t addr);

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

// ---- THE DISPLAY CAPTURE UNIT, and the two VRAM banks it makes move ---------
//
// GBATEK, DISPCAPCNT (0x04000064). The DS can write one frame of engine A's
// output into a VRAM block instead of only sending it to a screen, and that is
// the whole basis of the "3D on both screens" trick four minigames use: engine
// A renders ONE of two camera views live each frame onto whichever panel
// POWCNT1 names, the capture unit copies that frame into VRAM, and the OTHER
// panel shows the PREVIOUS frame's capture. Each screen ends up refreshing at
// 30 Hz and both look continuous.
//
// The register, as this unit reads it:
//
//   bits 16-17  destination VRAM block, 0..3 = banks A..D
//   bits 18-19  destination offset within that bank, in 0x8000-byte units
//   bits 20-21  size: 0 = 128x128, 1 = 256x64, 2 = 256x128, 3 = 256x192
//   bit 24      source A: 0 = the composed graphics screen, 1 = 3D only
//   bits 29-30  capture source: 0 = A, 1 = B, 2/3 = A and B blended
//   bit 31      enable, and THE HARDWARE CLEARS IT ITSELF at the end of the
//               captured frame. So does this: a capture is one frame's act and
//               a unit that left the bit set would capture forever off one
//               write.
//
// WHAT IS IMPLEMENTED AND WHAT IS NOT. Capture source A off the composed engine
// A output, at every one of the four sizes, into any block and offset. NOT
// source B (the display FIFO / VRAM read path) and NOT the A+B blend: nothing
// in this game programs either -- the only three writers in the link are
// Scene::ResetHardwareRegisters (0x80000000), func_ov006_020e759c (0x80330010)
// and func_ov006_020e7508 (0x80360010), and all three select source A. A
// request for source B is reported once and then ignored rather than guessed
// at, because inventing a second source would be inventing a picture.
//
// `src` is the FINISHED engine A framebuffer, SCREEN_W x SCREEN_H row major.
// It is sampled down to DS resolution by taking every scale'th pixel rather
// than by averaging, which is the same decision the sub screen's own blit
// makes and for the same reason: every tier's ratio is a whole number and a
// filter would invent pixels nobody drew.
void ppu_display_capture(const uint32_t *src, int w, int h);

// ---- WHERE A CAPTURED BANK GOES NEXT ----------------------------------------
//
// A DS VRAM bank is 128 KB of SRAM that appears at exactly ONE cpu address at a
// time, chosen by its VRAMCNT byte; the port maps VRAM as one flat 9 MB region
// with no bank indirection at all, so a bank's contents do not follow it. That
// does not matter for data the game uploads -- it writes through whatever window
// is live and the bytes stay there -- but it is fatal for a capture, because the
// capture unit always writes to the block's LCDC address while the game reads it
// through the window the bank is mapped to on the FOLLOWING frame.
//
// So this carries exactly the captured region across, and nothing else. It
// remembers, per block, the byte range the last capture wrote; when that block's
// VRAMCNT names a different home than it did last frame, it copies THAT RANGE
// from the LCDC address to the same offset in the new home. No other byte of the
// bank is read or written, so a bank that has never been captured into is
// untouched, and a captured bank's other 32 KB are left exactly where the flat
// mapping already had them.
//
// The homes it decodes are the ones this path uses, per GBATEK's VRAM control
// table: bank C MST 4 = engine B BG at 0x06200000, bank D MST 4 = engine B OBJ
// at 0x06600000, MST 0 = LCDC for both, and a disabled bank has no home. Any
// other MST on a captured block is reported once and left alone.
//
// Called once per frame, BEFORE the capture and before engine B rasterises, in
// that order, because that is the hardware's: the mapping is programmed in
// VBlank, the capture happens during the frame, and the scan-out reads what the
// mapping made visible.
void ppu_vram_publish(void);

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
//
// AND THE HEADROOM IS NOT THE ANSWER. It was built on the premise that the
// world's top G_rom rows carry artwork the zeroed G pushed off the screen, and
// that premise was measured and is false: on scene 368, in BOTH arms, on all
// 400 frames of the census, engine A's backgrounds put 0 of 8192 opaque texels
// in engine rows -32..-1, and the beam and wings the report is about are ON the
// screen at engine rows 0..31, byte-identical between the arms. See headroom_ds
// in hal/screen_gap.cpp for the numbers. The strip is opt-in and off, and what
// follows is what replaced it.
//
// ---- THE OBJECT SHIFT: the layer version is OFF, the PER ENTRY version is on -
//
// READ hal/screen_gap.cpp's obj_shift_ds BEFORE THIS. There are two mechanisms
// here and they share this geometry. The LAYER shift moves the whole top OBJ
// layer and is falsified and opt-in behind SM64DS_GAPLESS_OBJ_SHIFT=1. The PER
// ENTRY correction moves the framework-routed submissions only, is the default
// behaviour of the gapless mod, and is switched off for A/B with
// SM64DS_GAPLESS_PER_ENTRY=0.
//
// THE IDEA, common to both. The top engine takes world y to y + 0xc0 + G, so
// with G forced to zero a sprite lands G rows higher up a picture that did not
// move with it, and the report is a sprite jammed against the wooden beam and
// cut. Putting G back where the ROM had it is the whole fix; the two mechanisms
// differ only in WHICH sprites they put it back for.
//
// WHY THE LAYER VERSION IS OFF. "The OBJ layer" is not one thing. Only the
// sprites the framework's own OAM router placed move with G; the score rows and
// the top screen's own artwork are placed in SCREEN space and are at the same
// engine row in both arms. On scene 368, one sprite identity of twelve follows
// G, so the layer shift moves eleven that were never displaced. The numbers are
// in obj_shift_ds and the tool that took them is port/tools/objshift.py.
//
// WHY THE PER ENTRY VERSION IS THE DEFAULT. It is applied at the router's own
// call, to the submissions that carry the G term and to no others, so the
// eleven screen-space identities are untouched BY CONSTRUCTION rather than by a
// rule that has to be got right. It also lands before OAM::Render's culls, and
// that recovers submissions the layer shift cannot: a routed sprite's y is G
// lower under the mod, so `y + h < 0` threw 215 of them away over 300 frames on
// scene 368 and no display pass can draw an entry the engine was never given.
// See ppu_obj_routed_record below for how the routed set is carried here.
//
// AND THE IMAGE GROWS BY obj_shift_ds ROWS BELOW THE TOP SCREEN, not above it.
// The rows a shifted sprite runs off the top screen's bottom edge into are the
// world's own rows -obj_shift_ds..-1, the rows behind the hinge on hardware, so
// they go where the hinge is: between the halves. gap_ds carries their height,
// so band_y, band_h, bottom_y, h, the DIB header, the window size, the BMP
// writer and both stylus mappers are the arithmetic they already were, and the
// image is 192 + G_rom + 192 DS rows, which is gap-on's own shape.
//
// WHAT obj_shift_ds MEANS TO THE BAND is the one thing it changes there: those
// rows are LIVE. In a gap-on layout the band is rows neither engine can address
// and band_fill decorates them; here the same rows are world rows the top
// engine draws into, so hinge_paint puts engine A's own texels over the fill.
// The peek pass and the band's continuity pass are OFF in this mode, because
// both exist to answer "what is hidden in the hinge" and the hinge is not
// hidden any more.
//
// THE ONE IDENTITY THE WHOLE MODE RESTS ON: with the shift, world row r sits at
// bottom_y + r * scale for EVERY r in the image, negative and positive alike.
// Top screen, band and bottom screen are one continuous ruler, which is the
// property gap-on has and the property the seam straddle pass below needs. It
// is why that pass keeps running here and refuses on an ordinary band, where
// the same expression would draw a sprite across a hinge.
//
// 0 in every other layout, which is every level, every gap-on minigame and
// every gapless run with SM64DS_GAPLESS_PER_ENTRY=0, and a zero shift makes
// every field below exactly what it was before this existed.
struct StackLayout {
    int gap_ds;        // the DS rows of image between the halves. 0 = none.
                       // With obj_shift_ds 0 these are the simulated hinge G;
                       // with obj_shift_ds set they are world -gap_ds..-1.
    int obj_shift_ds;  // the world rows the band carries, in DS rows, and in
                       // the falsified layer arm equally the top engine's OBJ
                       // display shift. 0 = an ordinary band or no band. The
                       // per-entry default sets this and leaves the LAYER
                       // shift at zero; hal_gapless_obj_raster_shift_ds is the
                       // layer term and it is a separate question.
    int obj_raster_ds; // the LAYER display shift, in DS rows: the falsified
                       // arm's own term, 0 in the per-entry default and 0 in
                       // every layout with no band. obj_shift_ds MINUS this is
                       // the PER ENTRY correction, and that difference is the
                       // form every pass in ntr/ppu_sub.cpp reads it in.
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
    // WORLD-BAND: the GaplessMinigames mode. The band's rows are the world
    // rows a DS hides inside its hinge, drawn as real rows -- the scene's
    // backdrop colour behind, the engines' own crossing sprites crisp in
    // front (band_peek's raster plus the continuity pass). Set by the layout
    // from hal_gapless_world(); the band painters read it here. The fill
    // modes, the art, the ghost and peek's black backdrop all stand down
    // for it: this band is not decoration, it is playfield.
    int world_band;
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
    // WHICH PHYSICAL SCREEN ENGINE A IS DRIVING. 0 = the upper one, which is
    // POWCNT1 bit 15 set and what every layout carried before this existed.
    // 1 = the LOWER one, which is bit 15 clear.
    //
    // GBATEK, POWCNT1 (0x04000304) bit 15: "NDS Display Swap (0=Send Display A
    // to Lower Screen, 1=Send Display A to Upper Screen)". It is a DISPLAY
    // routing bit and nothing else -- neither engine's rendering changes, only
    // which LCD receives which engine's scan-out -- so the honest place to read
    // it is the presentation, which is what this field carries it to.
    //
    // NOT LATCHED WITH THE REST OF THE LAYOUT, and it must not be. Two of the
    // scenes that drive it TOGGLE IT EVERY FRAME (the dScMgD3DBase_c family's
    // slot 24 alternates the live camera between the two screens), so a value
    // cached at the scene's G-latch would be one frame's answer serving the
    // whole minigame. hal/screen_gap.cpp sets it on both paths out of
    // hal_screen_layout() beside `seam`, for exactly that reason.
    //
    // ZERO FROM stack_layout(), which is the whole of the zero-change
    // guarantee: a layout nobody tells about POWCNT1 composes the way it always
    // did, and every scene that leaves bit 15 set composes byte-for-byte the
    // way it always did whether or not it was told.
    //
    // WHAT MOVES WITH IT. The two blits, obviously; and the band's ENGINE
    // BINDINGS, because `row_bias` in BandEngine is not a property of an engine
    // but a statement about which side of the band that engine's rows are on.
    // See ppu_compose_stacked and band_biases in ntr/ppu_sub.cpp. What does NOT
    // move: every image-row expression (top_y, band_y, bottom_y, head_h, the
    // ambient fill's two edge rows, the stylus mappers), because those name
    // PHYSICAL screens and the physical screens do not move -- only the engine
    // feeding each one does.
    int main_lower;
};

enum { GAP_FILL_SOLID = 0, GAP_FILL_AMBIENT = 1, GAP_FILL_CUSTOM = 2 };

// The one computation. gap_ds, head_ds and obj_shift_ds are each clamped to
// [0, GAP_DS_MAX]; everything else is derived. Pure: same arguments, same
// answer, no globals read.
//
// obj_shift_ds is a STATEMENT ABOUT gap_ds's ROWS and not a second height: a
// caller that sets it passes the same number for gap_ds, because the rows the
// shifted objects need are exactly the world rows the band is made of. A shift
// with no band under it would push objects into rows the image does not have,
// so a non-zero obj_shift_ds with a zero gap_ds is dropped to zero here rather
// than trusted.
StackLayout stack_layout(int gap_ds, int head_ds, int obj_shift_ds,
                         int fill_mode, uint32_t fill_color, int peek,
                         const uint32_t *art);

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
/* The seam-ghost pass's attribute list: which sprite identities to continue
   across the seam when the game despawns them at the top screen's edge. Same
   registration idiom as the continuity reader above. */
typedef const unsigned short *(*GhostAttrFn)(int *count);
void ppu_seam_ghost_attrs(GhostAttrFn fn);
/* The game-tick pacer for the ghosts: a counter that advances once per
   engaged-scene behavior tick and freezes when the game does. */
typedef unsigned (*GhostTickFn)(void);
void ppu_seam_ghost_ticks(GhostTickFn fn);
/* True while the seam-snow overlay owns this sprite identity: the OBJ
   rasters skip such entries so the game's own flakes are hidden while the
   overlay draws the one unified field. Always false with no list registered,
   which is every scene and every mode except an engaged snow scene. */
int ppu_seam_snow_owns(uint16_t a2);

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

// THE SAME SNAPSHOT FOR THE ROM-ORDERED UPLOAD. func_02019144 uploads the OAM
// and only then is the frame scanned out, so when this program runs the upload
// where the ROM runs it -- at the head of the display path, ahead of BOTH OBJ
// rasters -- the block the screens draw from is the one that was JUST uploaded,
// not the one before it. Call this immediately AFTER that upload. The copy is
// then the block both rasters will read, and the routed marks that describe it
// are the shadow's, so the shadow's marks land in SHOWN and LIVE together
// instead of walking through LIVE over two frames. See THE OBJ/POWCNT1 PARITY
// in hal/sub_screen.cpp for why the upload moved, and ppu_seam_oam_mark above
// for the late-upload rotation this replaces one for one.
void ppu_seam_oam_mark_uploaded(void);

// ---- WHICH OAM ENTRIES THE FRAMEWORK'S ROUTER PLACED ------------------------
//
// WHY THIS EXISTS AT ALL. The per-entry correction has to know, at the raster,
// which of engine A's 128 entries were submitted in WORLD coordinates by one of
// the framework's five G-adding OAM routers and which were placed in SCREEN
// coordinates by everybody else. Nothing in the finished OAM says: same
// attribute words, same slot allocation, interleaved indices, and a table of
// attribute words would stop being true the first time a sprite changed tile.
// So the set is RECORDED AT THE CALL -- hal/screen_gap.cpp wraps the five
// routers and marks the slot OAM::Render returns -- and carried here.
//
// AND IT IS CARRIED ON THE SAME THREE-STATE PATH THE OAM WORDS ARE, which is
// the whole reason it lives beside g_oam_a_shown rather than in hal. A mark is
// only meaningful against the OAM block it was made for:
//
//   SHADOW  the fill in progress. Every routed submission writes here.
//   LIVE    what hardware OAM holds, which is upload N-1 while the engine A
//           compositor is reading it.
//   SHOWN   what the top screen was really drawn from, taken at the same
//           instant ppu_seam_oam_mark takes the OAM itself.
//
// ppu_seam_oam_mark rotates all three in one place, so a mark cannot get one
// upload out of step with the entry it describes. See THE TWO SCREENS ARE ONE
// FRAME APART in ntr/ppu_sub.cpp for why there are three and not one.
//
// THE RESIDUAL is the DS rows this entry still has to be moved down by at the
// raster, and it is 0 or the scene's G_rom and nothing else. It is 0 when the
// router's own submission already carried the ROM's row -- which is the normal
// case and the one that recovers the culled entries -- and G_rom for the entries
// whose ROM row is past 0xc0, where OAM::Render's `y > 0xc0` cull would have
// thrown the entry away rather than let it reach the band. Read hal/screen_gap.cpp's
// routed_render for that split; it is the one place it is decided.
void ppu_obj_routed_record(int slot, int resid);
void ppu_obj_routed_shadow_reset(void);
// The residual for a slot of the block hardware OAM holds now, 0 if that slot
// was not routed. The engine A compositor's per-entry term.
int ppu_obj_routed_live_resid(int slot);
// 1 if that slot of the block hardware OAM holds now was a routed submission.
int ppu_obj_routed_live_is(int slot);

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
