// DS geometry engine.
//
// Unlike the 2D engines, 3D is not readable register state -- it is a *stream*.
// Writing a geometry port or pushing a word into the FIFO at 0x4000400 executes a
// command, and the result exists only as the polygons it emitted. A host layer
// that only mirrors registers renders nothing here, which is why this is the one
// part of the port that cannot be done by mapping memory.
//
// SM64DS drives it both ways, so both are implemented:
//
//     *(volatile u32 *)0x4000400 = 0x1a;      // packed command word
//     Copy36Bytes(m, (int *)0x4000400);       // ...then its 9 parameters
//
//     DMAStartTransfer(ch, src, 0x4000400, 0x84400000 | (len >> 2));   // display list
//
// See notes/assessment.md section 2a and docs/mmio-inventory.md.

#ifndef NTR_GX_H
#define NTR_GX_H

#include <stdint.h>

#include "ntr/ppu.h"

namespace ntr {

// A vertex after the modelview+projection transform, in screen space.
struct GxVertex {
    float x, y, z, w;      // x/y in pixels, z in [0,1] after divide
    float u, v;            // texel coordinates (DS TEXCOORD is 1.4 fixed point)
    uint32_t color;        // 0xAARRGGBB

    // --- appended for the model smoother; see ntr/smooth.h ------------------
    // THE VIEW-SPACE VERTEX, carried alongside the clip-space one above.
    // "View space" here is after the DS POSITION matrix and before the
    // projection. It is the last space in which a curved patch can be built
    // linearly: the perspective divide and the near clip both come after it,
    // and subdividing past either of them bends straight edges by the near
    // plane's distance. vw is the fourth component the position matrix
    // produced -- 1 for every affine matrix the game loads, and the smoother
    // refuses a triangle where it is not.
    float vx, vy, vz, vw;
    // The NORMAL command's own vector after the VECTOR matrix (which is the
    // position matrix without translation, so it lands in the same space as
    // the position above), unit length. (0,0,0) when the polygon carried no
    // NORMAL, which is how an unlit polygon says it has no surface to curve.
    float nx, ny, nz;
};

// The bound texture travels with the triangle. Material state is set by the
// game's C code around each display list, not inside it, so a triangle's
// texture is whatever was bound when it was assembled.
struct GxTriangle {
    GxVertex v[3];
    const uint32_t *tex;   // decoded RGBA, or null for untextured
    int tw, th;
    // HOST PIXELS PER DS TEXEL, and it is 1 for every texture the ROM itself
    // supplies. A replacement image from an HD pack (ntr/hdtex.h) is a whole
    // multiple of the DS texture's own size, and this is that multiple: the
    // sampler multiplies u and v by it, and tw/th are then the ACTUAL pixel
    // dimensions of the bound buffer rather than the DS texture's. So the
    // wrap, clamp and flip rules in tex_coord keep working on the real
    // buffer, a 4x image of a 32x32 texture binds as 128x128 with a 4 here,
    // and at 1 -- which is what a run with no pack has everywhere -- the
    // multiply is by exactly 1.0f and the raster is bit-for-bit the
    // arithmetic it did before this field existed.
    uint8_t tex_scale;
    // WHICH TEXTURE THIS IS, as a number that is never handed out twice
    // (run hd2, lane GPU2). The optional graphics-card renderer keeps its own
    // copy of every texture it has drawn with, and it cannot key that copy on
    // `tex` above: the decode cache frees its buffers and the allocator hands
    // the same address back for different pixels, so a pointer key serves a
    // stale picture. This id is handed out at bind time, counts up and is
    // never reused; gx_invalidate_textures bumps gx_texture_generation() so a
    // backend throws its whole cache away at a scene change. 0 means
    // untextured. It costs one map lookup per bind and only when a backend is
    // registered, so an ordinary run does not pay for it.
    uint32_t tex_id;
    uint8_t cull;          // POLYGON_ATTR bits 6-7: 1 back, 2 front, 3 both
    uint8_t alpha;         // POLYGON_ATTR bits 16-20 (0..31; 31 = opaque)
    // TEXIMAGE_PARAM bits 16-19 as bit0 repeat S, bit1 repeat T, bit2 flip S,
    // bit3 flip T. A cleared repeat bit is CLAMP, not wrap.
    uint8_t wrap;
    // The hardware's own render-order class: attribute alpha 1..30 or an
    // A3I5/A5I3 texture puts the polygon in the translucent pass, which
    // draws after every opaque polygon regardless of submission order.
    // Mode-3 (shadow) polygons ride this pass too, whatever their alpha:
    // their stencil protocol needs the depth buffer already final.
    uint8_t translucent;
    // POLYGON_ATTR bits 4-5: 0 modulation, 1 decal, 2 toon/highlight,
    // 3 shadow. 3 is the shadow-volume protocol (GBATEK shadow polygons);
    // 2 takes its vertex colour through the TOON TABLE, see gx_toon_table
    // below; 1 draws as modulation, same as before it existed.
    uint8_t mode;
    // POLYGON_ATTR bits 24-29. For mode-3 polygons the ID selects the role:
    // 0 is the stencil mask, nonzero is the drawn shadow. For everything
    // else it is recorded per pixel so a shadow can refuse to fall on its
    // own caster (equal IDs do not shadow).
    uint8_t polyid;
    // POLYGON_ATTR bit 14: this polygon's depth test is EQUAL (within the
    // hardware's tolerance) instead of LESS. See w_depth_fail in gx.cpp.
    uint8_t depth_eq;
    // POLYGON_ATTR bit 15: the polygon takes the frame's fog (DISP3DCNT bit 7).
    uint8_t fog;
    // POLYGON_ATTR bits 21-23 (unused by the DS) while the ghost layer is armed
    // (gx_set_ghost_layer): 0 = an ordinary polygon, 1..7 = the polygon belongs
    // to ghost figure N. A ghost figure is drawn by gx_render in its own pass
    // AFTER the opaque and translucent passes, as one see-through layer (see
    // the comment above gx_set_ghost_layer), never in the passes themselves.
    uint8_t ghost;
    uint32_t dbg_tex;      // TEXIMAGE_PARAM that was bound (diagnostics only)
};

// ---- A SMOOTHED TRIANGLE'S GRID, KEPT WHOLE FOR THE CARD (run hunt8, SMOOTHGPU1)
//
// With SmoothModels on, one source triangle becomes a grid of up to 64
// sub-triangles, and writing those 208-byte records is most of what the
// processor spends on smoothing. When the card is going to draw the whole
// frame, gx.cpp keeps the grid instead: the model-space points (already built,
// or read out of the shape store), the three source corners' texel coordinates
// and colours, the matrices the corners rode, and the polygon state. The card
// expands it. `at` is where in the ordinary triangle list the grid's
// sub-triangles belong, so the order translucent and shadow polygons depend on
// is the order the game submitted them in. A frame the card does not take (it
// fell back, a debug probe wants the triangles, gx_polygons asks) has every
// grid turned into the triangles it always was before anything reads them:
// gx.cpp's grids_materialize, which runs the very code the processor path runs.
struct GxGridCorner {
    float x, y, z;          // view-space position
    float nx, ny, nz;       // view-space normal
    float u, v;             // texel coordinates
    uint32_t color;         // 0xAARRGGBB
};
struct GxGrid {
    GxTriangle st;          // the polygon state, corners zeroed
    GxGridCorner s[3];      // the source triangle
    float xform[16];        // model space -> view space (has_xform), else identity
    float proj[16];         // the projection the grid was drawn under
    float widen;            // view_to_clip's x factor (1 when orthographic)
    int32_t vp[4];          // the viewport (x, y, w, h) to_screen mapped through
    float lo[3], hi[3];     // model-space box of the points
    float sbox[4];          // screen box that holds every sub-triangle (x0 y0 x1 y1)
    uint32_t at;            // triangle-list index this grid sits before
    uint32_t off;           // first float of the points (pool or frame arena)
    uint32_t pool_gen;      // the shape store's generation when `off` was taken
    uint8_t tf;             // grid divisions: 2, 4 or 8
    uint8_t has_xform;
    uint8_t persp;
    uint8_t from_pool;      // points are in the shape store's pool, else the frame arena
    uint8_t depth_snap;     // SMOOTHREND1: points that bulge toward the camera are put back on the flat triangle's plane (pl)
    float pl[4];            // that plane in view space: n.x = d, n = pl[0..2], d = pl[3]
    float snap_lim;         // the farthest a point may move along its ray (view units)
};

// Feed the engine. `gx_write_port` takes a geometry-port address in
// 0x4000440..0x40005FF; `gx_write_fifo` takes a word pushed at 0x4000400.
void gx_write_port(uint32_t addr, uint32_t value);
void gx_write_fifo(uint32_t word);
// GHOSTPERF1: a display list pushed as a block (the DMA to the FIFO). Exactly gx_write_fifo on each word in order (same commands run in the
// same order, same stream hash and word count), but a command word whose parameters are all in the block runs them in place instead of
// through the one-word-at-a-time state machine. A block that ends inside a command, or starts inside one, finishes word by word.
void gx_write_fifo_block(const uint32_t *words, int n);
// GHOSTPERF1 proof instrument: 0 makes the in-between pictures replay every ghost group (the old way), -1 puts the setting back.
void gx_ghost_fast_override(int v);

// Reset the command stream, matrix stacks and polygon list.
void gx_reset();

// Drop every VRAM-decoded texture. The decode cache deliberately survives
// gx_reset (which runs once a frame); call this when the texture slots are
// being reused for different content, as the model soaks do per model.
void gx_invalidate_textures();

// Bind a decoded texture for subsequent geometry. Pass null for untextured.
void gx_bind_texture(const uint32_t *rgba, int width, int height);

// HOW A TEXEL IS CHOSEN (run hd2, the "TextureFilter" setting). 0 is nearest,
// the sampler this renderer has always used and the DS's own behaviour; 1 is
// bilinear; 2 is trilinear, which adds a chain of halved copies of each
// texture and blends the two sizes nearest the surface's distance.
//
// CALL IT ONCE, AT BOOT, BEFORE ANY GEOMETRY. The mode decides whether the
// texture cache builds a mip chain as each texture enters it, so a run that
// changed it half way would have chains for some textures and not others. At
// 0 no chain is built, no extra memory is held, and the raster runs the same
// instructions it ran before this existed: the mode is resolved once per
// frame into one of three whole raster bodies, never as a test per pixel.
void gx_configure_texture_filter(int mode);
int gx_texture_filter();

// EDGE SMOOTHING OVER THE FINISHED 3D PICTURE (run hd2, the "AntiAliasing"
// setting). 0 is off and is no pass at all; 1 runs a contrast-edge filter
// over the picture at the END of gx_render -- before any 2D layer is
// composited over it, so text, the HUD and the touch-screen art are never
// read and never written.
//
// Call it once at boot, beside gx_configure_texture_filter: the pass keeps a
// scratch copy of the picture, allocated on first use and never freed.
void gx_configure_anti_aliasing(int mode);
int gx_anti_aliasing();

// What the smoothing pass has done since the program started: how many pixels
// it rewrote and how many frames it ran on. The A/B evidence that a key-absent
// run does no work at all. (It used to carry a third number, the frames the
// pass refused to run on because the display capture unit was already armed.
// That mechanism is gone -- see gx_aa_preimage below -- and the pass now runs
// on every frame, so the number could only ever have been zero.)
void gx_aa_counters(unsigned long long &changed, unsigned long long &frames);

// THE FRAME AS IT WAS BEFORE THE SMOOTHING PASS, or null when there is no such
// frame (the setting is off, or gx_render has not finished one this frame).
// Same SCREEN_W stride as the framebuffer, so an index into one indexes the
// other.
//
// THIS IS WHAT KEEPS THE GAME FROM NOTICING THE SETTING, and it replaced a
// weaker rule that measurement broke. SM64DS reads its own top screen back
// through the DS display capture unit for the dual-screen minigames, and the
// frame it arms that unit on is NOT always one the raster has already drawn:
// on a running minigame the arm lands after gx_render, so refusing to smooth
// an already-armed frame missed five hundred captured frames out of twelve
// hundred. Instead the pass keeps the picture it started from,
// hal/message_compositor.cpp mirrors its own writes into it so it stays a
// finished frame, and ntr::ppu_display_capture reads it in place of the live
// framebuffer. The game gets the setting-off picture whenever it asks, with no
// assumption about when it asks.
uint32_t *gx_aa_preimage();

// The game path: TEXIMAGE_PARAM / PLTT_BASE writes (ports 0x2A/0x2B) bind by
// decoding lazily out of the mapped VRAM texture/palette slot windows.
void gx_teximage_param(uint32_t value);
void gx_pltt_base(uint32_t value);

// Diagnostic stream hash: every word entering the engine since the last
// reset, so smokes can separate "different stream" from "ignored stream".
uint32_t gx_stream_hash_reset();
uint32_t gx_state_hash();  // pos matrix + first slots
int gx_store_count_reset();

// Preload a position-matrix stack slot. Display lists open with MTX_RESTORE
// against slots the *scene* filled in beforehand, so a model drawn on its own
// has to have them set up first. Row-major, row-vector convention.
void gx_set_matrix_slot(int slot, const float m[16]);

// Lighting. The DS computes a vertex's colour when it sees NORMAL, out of the
// current material and the four lights -- so lighting is not a rasteriser stage
// here, it happens during command decode, exactly as on hardware.
//
// Display lists carry NORMAL but never DIF_AMB or LIGHT_*: the material and the
// scene set those. Bind them before submitting geometry.
void gx_set_material(uint32_t dif_amb, uint32_t spe_emi);
void gx_set_light(int index, float dx, float dy, float dz, uint32_t bgr555);
void gx_enable_lights(uint32_t mask);           // bits 0..3

// THE GHOST LAYER (run mp1, lane GHOSTLAYER1). The adventure hub draws every
// other player's body as a see-through ghost. On the DS a see-through polygon
// writes no depth and is refused on a pixel that already took a translucent
// fragment of the same polygon id, so a figure of many polygons under one id
// shows its far arm through its own body and loses parts of itself. Instead the
// host marks the ghost's materials with a figure number in POLYGON_ATTR bits
// 21-23 (the DS ignores them; the mark rides the recorded geometry stream, so
// the in-between pictures of SmoothMotion see it too) and arms this switch.
// With it armed, gx_render takes the marked polygons out of the frame, draws
// the rest exactly as it always did, and then draws each figure as one layer.
// Off (the default, and every run outside the hub): the bits are not read and
// nothing is different.
void gx_set_ghost_layer(int on);
int gx_ghost_layer();

// Polygons emitted since the last gx_reset / SWAP_BUFFERS.
const GxTriangle *gx_polygons(size_t &count);
// The polygon count only: the triangles the list holds plus the ones its kept
// smoothed grids stand for, without turning the grids back into triangles.
size_t gx_polygon_count();

// Bumped every time gx_invalidate_textures throws the decode cache away, so a
// backend holding copies of those textures knows to drop them.
uint32_t gx_texture_generation();

// What a whole-frame backend reports back. aa_done: the smoothing ran (the
// picture in fb is the smoothed one and aa_changed counts the pixels it moved);
// pre_valid: `pre` holds the frame as it was before the smoothing.
struct GxGpuOut {
    int aa_done;
    int pre_valid;
    unsigned long long aa_changed;
    int kept;              // the picture was left on the card (GxGpuFrame::keep)
    int deferred;          // the readback was left pending (GxGpuFrame::defer): fb and cover are not final yet
};

// ---- THE OPTIONAL GRAPHICS-CARD BACKEND FOR THE OPAQUE PASS (run hd2) -----
//
// gx_render draws a frame in two passes over the same buffers: an OPAQUE pass
// (colour, depth with a LESS test, a per-pixel polygon ID, the coverage mask)
// and then a TRANSLUCENT pass that READS what the opaque pass left. The opaque
// pass is nearly all of the fill, so it is the one worth moving to a card.
//
// A backend is a single function that is handed everything the opaque pass
// needs and fills the CPU buffers below exactly as the software pass would
// have. It returns 1 when it drew the frame and 0 when it did not, and 0 is
// never an error here: gx_render simply runs its own opaque pass instead, on
// buffers nothing has touched. Nothing is registered unless a setting asked
// for it, and with none registered this whole mechanism is one null test per
// frame.
//
// hal/gpu_raster.cpp is the Direct3D 11 one. The software pass stays the
// default and the only byte-exact reference.
struct GxGpuFrame {
    const GxTriangle *tris;
    size_t count;
    // Every buffer below is row-major with the SAME stride, which is the
    // framebuffer's allocation width; a row y starts at base + y * stride.
    uint32_t *fb;          // the framebuffer, 0xAARRGGBB
    float *depth;          // screen-linear z (-(1/w) when wdepth), cleared to 1e30
    uint8_t *cover;        // the 3D coverage mask, cleared to 0
    uint8_t *attrid;       // the per-pixel polygon ID
    int stride;
    int cw, ch;            // the active picture inside the allocation
    int px0, py0, pw, ph;  // the present rectangle, which is the scissor
    // the whole-frame path's scissor: the present rectangle narrowed to the game's
    // viewport (raster_clip, run hunt8 lane CREDITCROP1), half-open; empty = none.
    // px0..ph stays the present rectangle, which a held frame is composited by.
    int clip_x0, clip_y0, clip_x1, clip_y1;
    int want_attrid;       // the frame has a shadow volume, so the ID is read
    int want_depth;        // a second pass will read the depth back
    int wdepth;            // this frame is W-buffered: depth is -(1/w), not z (gx_render's g_wdepth)
    int tex_filter;        // 0 nearest, 1 bilinear, 2 trilinear
    uint32_t tex_generation;
    uint32_t clear_argb;   // what the framebuffer held when gx_render started
    // THE TOON TABLE for this frame's mode-2 polygons (see gx_toon_table):
    // toon_shade 0 = the frame has none, 1 = toon, 2 = highlight
    // (DISP3DCNT bit 1), and toon_rgb the 32 entries as 0..255 floats, three
    // per entry, in the units the vertex colour travels in.
    int toon_shade;
    const float *toon_rgb;
    // the Smooth Models toon blend (gx.cpp's g_toon_blend): [k] 1 where entries
    // k and k + 1 blend, [31] 1 when the blend is on at all (lane SMOOTHTOON1)
    const float *toon_blend;
    // THE FRAME'S FOG (run hunt8, RENDER093): fog_on 0 = none; fog_shift and
    // fog_offset are DISP3DCNT bits 8-11 and FOG_OFFSET << 8; fog_rgb the
    // colour as 0..255; fog_tab the 34 density entries of the melonDS table
    // (gx.cpp's fog_latch), 0..128.
    int fog_on;
    float fog_wscale;
    int fog_shift;
    uint32_t fog_offset;
    float fog_rgb[3];
    int fog_tab[34];

    // ---- THE WHOLE FRAME ON THE CARD (run cardfull, lane CARDFULL1) ---------
    // Everything below is for a backend that draws the translucent and shadow
    // pass and the edge smoothing as well (hal/gpu_raster.cpp, the default
    // since run cardfull). A backend that only knows the opaque pass ignores it
    // all and returns 1, and gx_render does the rest itself, as it always did.
    //
    // full_ok    gx_render will take a whole-frame answer: 1 over buffers it has
    //            cleared (the in-process A/B), 2 over buffers it has NOT (it
    //            skips their clears for a frame the card draws whole, and
    //            clears them itself if the card does not); 0 not at all
    // probe      do not draw: answer 2 if a frame offered with full_ok 2 would
    //            be attempted whole, 0 otherwise (nothing is touched)
    // planes     do not draw: fill depth and attrid (already cleared by the
    //            caller) from the frame the card last drew whole, and answer 1,
    //            or 0 if there is no such frame. A whole frame does not bring
    //            those planes back; gx_depth and gx_attr_ids ask for them here
    //            only if somebody reads them
    // ab         the in-process A/B is comparing this frame: the depth and the
    //            polygon-ID plane come back as well, and the smoothing is left
    //            to gx_render's own pass so the comparison sees the picture
    //            before it
    // aa_mode    edge smoothing is on and the backend may run it (never in an
    //            A/B)
    // pre_needed the display capture unit may read this frame, so the picture
    //            as it was BEFORE smoothing has to exist (see gx_aa_preimage)
    // pre        aa_pass's own copy buffer, SCREEN_W stride; the backend fills
    //            it when pre_needed, exactly as aa_pass would have
    // out        the backend's report; written only on a return of 2
    int full_ok;
    int probe;
    int planes;
    int ab;
    int aa_mode;
    int pre_needed;
    uint32_t *pre;
    GxGpuOut *out;

    // ---- THE PICTURE STAYS ON THE CARD (run cardfull, lane CARD2) -----------
    // keep    the caller (gx_clear_fill filled fb with the transparent overlay
    //         value, see gx_card_present_allow) will put the 2D over this frame
    //         ON THE CARD and the picture is presented from there. With probe
    //         set the answer is 3 when a frame offered like this would be kept
    //         (2 or 0 as before when not); a draw that keeps the frame answers
    //         2 with out->kept 1 and touches NO CPU buffer: no colour, no
    //         coverage, no copy before the smoothing comes back.
    // settle  do not draw: 1 = fb holds the 2D overlay of the frame the card
    //         last kept; turn it into the finished picture in place (3D under
    //         the overlay, then the fade), fill the coverage plane, and fill
    //         `pre` with the frame before the smoothing when out->pre_valid
    //         says so; 2 = write into `pre` the picture the display capture
    //         reads (the frame before the smoothing under the overlay, no fade)
    //         and leave fb alone. Both answer 1, or 0 when the card cannot
    //         (then the 3D of that one frame is the clear colour).
    int keep;
    int settle;
    // slot    with settle 3 (run hunt8, lane GPUINTERP1): 1 the retained tick's
    //         composite, 2 the retained picture between two ticks
    int slot;
    // sink3d  with settle 1 (run hunt8, lane CARDSM1): when not null, the card's
    //         3D picture alone (smoothed when the frame was, no 2D, no fade) is
    //         also written here, SCREEN_W stride. SmoothMotion's snapshot of
    //         the 3D layer, for a tick that had to be settled after all.
    uint32_t *sink3d;
    // outside_argb  what the framebuffer holds outside the present rectangle
    //               (the pillarbox constant), for the composite and the settle
    uint32_t outside_argb;

    // ---- SMOOTHED GRIDS (run hunt8, SMOOTHGPU1) -----------------------------
    // grids / ngrids  the frame's deferred grids, in list order; each sits
    //                 before tris[grids[i].at]. Only handed over with full_ok 2
    //                 (and never in the A/B). A backend that does not take them
    //                 returns 0 and gx_render turns them into triangles.
    // arena           the frame arena the points of a non-pool grid live in
    const GxGrid *grids;
    size_t ngrids;
    const float *arena;
    size_t arena_floats;
    // probe_grids     set with probe: the backend answers 2 only if it can take
    //                 grids too (so gx.cpp knows whether to keep them)
    int probe_grids;

    // ---- LATE AND DEFERRED READBACK (run after093, lane MENU2) --------------
    // late      gx_clear_fill_late was used: fb and the coverage plane have NOT
    //           been cleared. A backend that reads the picture back calls
    //           pre_read(pre_arg) once, after its draw is on the card and
    //           before it waits for it (so the clears hide under the card's
    //           work), and must call it before the first CPU buffer is written;
    //           a backend that does not read back (or returns 0) leaves it, and
    //           gx_render runs the clears itself.
    // defer     with late: the backend may leave the readback pending and answer
    //           2 with out->deferred 1; gx_finish_readback finishes it later
    //           (pre_read, the wait, the copy into fb / cover). Two at most.
    // probe_defer  with probe: answer 4 when a late, deferring frame like this
    //           would be left pending, 0 otherwise
    // finish    do not draw: 1 finishes the oldest pending readback, 2 only
    //           reports; both answer the number still pending
    int late;
    int defer;
    int probe_defer;
    int finish;
    void (*pre_read)(void *);
    void *pre_arg;

    // ---- THE GHOST LAYER (run mp1, lane GHOSTLAYER1) ------------------------
    // The ghost figures of the frame (GxTriangle::ghost != 0), kept OUT of
    // `tris`. A backend draws them after its translucent pass and before its
    // edge smoothing, one figure at a time, nearest last: a depth-only pass
    // of the figure's polygons (against the depth the frame has made), then its
    // colour with the depth test EQUAL so each pixel takes exactly one fragment
    // of the figure, blended at the polygon's alpha (1..30). The figure's depth
    // stays in the frame's depth, as an opaque ghost's always did. Null / 0 on
    // every frame without a ghost, which is every frame outside the adventure
    // hub.
    const GxTriangle *ghost_tris;
    size_t ghost_count;
};
// The return value of the callback: 0 the card drew nothing (gx_render runs
// both passes), 1 the opaque pass only (the old contract), 2 the whole frame
// (the translucent pass is done, and the smoothing too when out->aa_done).
typedef int (*GxGpuOpaqueFn)(const GxGpuFrame *);
void gx_set_gpu_opaque(GxGpuOpaqueFn fn);
int gx_gpu_opaque_registered();
// The registered backend, so a caller can take it out and put it back and
// draw one list both ways.
GxGpuOpaqueFn gx_gpu_opaque();

// ---- THE OPTIONAL GRAPHICS-CARD BACKEND FOR THE EDGE SMOOTHING (run perf2) --
//
// aa_pass (AntiAliasing 1) is the other CPU pass left after the card draws the
// opaque picture. A backend is handed the frame as it stands (aa_pass's own
// copy of it, which is also what the display capture reads), the coverage
// mask and the live framebuffer; it writes every pixel the smoothing changes
// into fb, exactly as aa_band would, sets `changed` and returns 1. 0 is never
// an error: aa_pass then runs its own bands on the same frame. Nothing is
// registered unless the "Renderer" setting is on (hal/gpu_raster.cpp), so the
// software renderer's smoothing is untouched: one null test per smoothed frame.
struct GxGpuAa {
    const uint32_t *src;   // the frame before smoothing, SCREEN_W stride
    const uint8_t *cover;  // the 3D coverage mask, same stride
    uint32_t *fb;          // the live framebuffer, same stride
    int stride;
    int w, h;              // the active picture
    unsigned long long changed;  // out: pixels rewritten
};
typedef int (*GxGpuAaFn)(GxGpuAa *);
void gx_set_gpu_aa(GxGpuAaFn fn);
#define GX_HAS_GPU_AA 1   // hal/gpu_raster.cpp registers its backend only when this exists

// Rasterise them into fb with a depth buffer. Does not clear fb -- the 3D layer
// composites over whatever the 2D engine already drew.
void gx_render(Framebuffer &fb);

// THE FRAME'S CLEAR, which is the colour the DS shows at a top-screen pixel
// that neither a polygon nor a 2D layer covers. GBATEK, DS 3D Rear-Plane: the
// rendering engine starts every frame from CLEAR_COLOR (0x04000350: bits 0-14
// the colour, 16-20 the alpha); DS Video: the 3D picture is BG0, and a BG0
// pixel whose alpha is 0 is transparent, so the layers under it and then the
// BACKDROP (engine A's palette entry 0, 0x05000000) show there instead.
//   BG0 shown as the 3D layer (DISPCNT bits 3 and 8) and the clear alpha
//   nonzero: the clear colour (the star select's white, SetClearColor(0x7FFF,
//   0x1F, ...) in src/_ZN12dScStarSel_c13InitResourcesEv.cpp);
//   BG0 shown as the 3D layer with the clear alpha 0: the old constant
//   0xFF101820, because the raster blends translucent polygons onto this fill
//   where the DS lets the 2D engine blend them onto the layer below BG0;
//   BG0 not shown as the 3D layer: the backdrop.
// The 2D compositor then paints its layers over this exactly as before. The
// fill covers the PRESENT rectangle; a pillarbox margin outside it keeps the
// old constant 0xFF101820, which is also the whole answer before the I/O
// window is mapped (a harness with no io_init). SM64DS_CLEAR_FILL_OLD=1 puts
// the old constant back everywhere for A/B on one binary; SM64DS_CLEAR_PROBE=1
// prints every change of the registers the answer is made from.
uint32_t gx_clear_argb();
void gx_clear_fill(Framebuffer &fb);

// run after093, lane MENU2: gx_clear_fill for a frame the card will read back.
// gx_late_possible says whether the next frame can take it (a card backend that
// draws whole frames, not the A/B, no held bitmap frame under the 3D, and
// SM64DS_MENU_LATE not 0). gx_clear_fill_late(fb) then leaves fb and the
// coverage plane UNCLEARED: gx_render clears them (so the card's work hides the
// clears) when it has the picture on the card, or when the card declines it.
// gx_defer_request(1) before that gx_render lets the card leave the readback
// pending: fb and gx_coverage are then NOT final until gx_finish_readback
// (the oldest; at most two are pending). gx_set_flush_hook registers what must
// run before a clear writes into fb while an older picture is still pending
// (the card took a later frame the sync way): walk_window finishes and
// presents the older picture there.
// The menu lane's A/B knobs (run after093, lane MENU2). A kill switch read as
// "2" alternates on and off every 120 scene ticks inside ONE run (the scene
// path calls gx_menu2_phase_set at each tick), so a before/after on a noisy
// box compares neighbouring seconds. gx_menu_knob_on turns the knob's value
// (0 off, 1 on, 2 alternate) into this tick's answer.
int  gx_menu_knob_value(const char *env);          // 0, 1 or 2; read the environment once per call site
int  gx_menu_knob_on(int value);
void gx_menu2_phase_set(int phase);
int  gx_menu2_phase_get(void);
// Runs fn over [0, rows) in bands on the raster pool's threads (the calling
// thread takes the first), or in one piece when the job is smaller than
// min_rows, one thread is all there is, or the caller's knob (gx_menu_knob_value) is off. Every band is
// whole rows, so a loop whose rows do not depend on each other gives the same
// bytes as one thread.
void gx_parallel_rows(int rows, int min_rows, void (*fn)(void *ctx, int y0, int y1), void *ctx, int knob_value);
// 1 when gx_parallel_rows will use more than one thread (four or more raster threads, SM64DS_MENU_PAR not 0)
int  gx_parallel_available(void);
// 1 makes every gx_parallel_rows run on the calling thread (the single-thread reference for the proof instruments)
void gx_parallel_serial(int on);
int  gx_late_possible(void);
void gx_clear_fill_late(Framebuffer &fb);
void gx_defer_request(int on);
int  gx_render_deferred(void);   // the last gx_render left its readback pending
int  gx_readback_pending(void);
void gx_finish_readback(void);
void gx_set_flush_hook(void (*fn)(void));
void gx_aa_note_changed(unsigned long long n);

// ---- THE PICTURE NEVER COMES BACK TO THE PROCESSOR (run cardfull, CARD2) ----
//
// With Renderer 1 and PresentBackend 1 the 3D picture can stay in the card's
// own targets. gx_clear_fill then fills the framebuffer with 0 (alpha 0, "no
// 2D here") instead of the clear colour, gx_render draws on the card and reads
// nothing back, the 2D compositor and the bottom-screen inset write only the
// pixels they own, and the present composites the two on the card. The alpha
// byte of a framebuffer pixel is the verdict:
//   GX_OV_NONE   no 2D: the 3D picture shows (and the fade, if any, applies)
//   GX_OV_FRONT  a 2D pixel in front of the 3D layer: this colour
//   GX_OV_BEHIND a 2D pixel BEHIND the 3D layer: this colour where the 3D
//                engine drew nothing, the 3D pixel where it did
// Anything on the processor that needs the real pixels calls gx_card_settle,
// which reads the card's picture back once and makes the frame an ordinary one.
//
// gx_card_present_allow: the caller of gx_clear_fill says, for the frame it is
// about to start, whether the picture may stay on the card (it must not when
// the host is going to read or draw over the 3D pixels: the stacked layout,
// SmoothMotion's snapshots, a selftest picture dump). One frame only.
// gx_card_overlay_active: 1 from the gx_render that kept the frame until the
// frame is presented or settled. gx_card_capture_picture: the picture the
// display capture unit reads, built on request from the card's pre-smoothing
// copy under the overlay; null when the frame is not held on the card.
#define GX_OV_NONE   0x00u
#define GX_OV_FRONT  0xFFu
#define GX_OV_BEHIND 0xFEu
void gx_card_present_allow(int allow);
int gx_card_overlay_active();
void gx_card_settle();
// The same, saying why: every reason is counted and the counts are printed once
// at exit, which is how a frame that reads the picture back stays visible.
void gx_card_settle_because(const char *why);
const uint32_t *gx_card_capture_picture();

// ---- A HELD TICK UNDER SMOOTHMOTION (run hunt8, lane CARDSM1) ----------------
// With the SmoothMotion key on, the pictures between two ticks are drawn from
// the tick's recorded geometry into a framebuffer of their own and the card's
// targets are reused by them, so a tick held on the card must be composited
// BEFORE the first of them. gx_card_retire does that bookkeeping after the
// caller composited the tick (port_gpu_card_compose): the tick's finished
// picture then lives in the composite target, gx_card_retained says whether a
// framebuffer is such a tick, and gx_card_settle_fb reads it back into that
// framebuffer (a picture dump, a present that cannot take it from the card).
//   gx_card_settle_sink: for the frame being held, a settle also writes the
//   card's 3D alone into dst3d and then calls done (the caller keeps its own
//   snapshots of the tick in step). One frame only.
//   gx_card_behind_seen: a 2D layer behind the 3D wrote a pixel this frame;
//   gx_card_pre_needed: the display capture may read this frame's copy from
//   before the smoothing. Either one makes SmoothMotion settle instead.
void gx_card_settle_sink(uint32_t *dst3d, void (*done)(void));
void gx_card_note_behind();
int gx_card_behind_seen();
int gx_card_pre_needed();
void gx_card_retire(const Framebuffer *fb);
int gx_card_retained(const Framebuffer *fb);   // 0 no, 1 a retained tick, 2 a retained picture between ticks
// run hunt8, lane GPUINTERP1: a picture between two ticks drawn on the card.
// gx_card_interp_clear is gx_clear_fill for it (the card keeps the frame and
// the framebuffer is not wiped; 0 = not kept, the classic fill was done);
// gx_card_retire2 is gx_card_retire for it once port_gpu_card_compose_inter put
// its picture in the in-between composite target.
int gx_card_interp_clear(Framebuffer &fb);
void gx_card_retire2(const Framebuffer *fb);
void gx_card_settle_fb(Framebuffer &fb, const char *why);

/* MENU WIDE 3D (run menu1, lane FILE2). A menu page whose 3D background should fill a wide window installs a callback
   (null = never, the whole of the old behaviour). Once per frame, the first time the frame's 3D asks, it answers true when
   this frame's 3D covers the whole active frame instead of the centred native rectangle; *zoom (1 = none) and *dy (clip
   units) then zoom everything the frame draws OUTSIDE a gx_menu_sprite(1) .. gx_menu_sprite(0) bracket about the frame's
   centre. Sprite objects land where they do natively. gx_menu_wide_active() is this frame's answer. */
void gx_set_menu_wide_cb(bool (*cb)(float *zoom, float *dy));
void gx_menu_sprite(int on);
bool gx_menu_wide_active();
/* HUDMENU2 (H7): 1 while the last VIEWPORT the game wrote is the ending's 16:9 letterbox that DRAWDIST2 moved down 8 rows (rows 24..167);
   the 2D compositor then moves the subtitle box up so it sits inside that picture. 0 for every other viewport. */
int gx_ending_viewport_shifted();

/* TITLE092: the title's lower-screen 3D as one full-canvas scene. gx_set_menu_scene_cb installs a callback (null = never; it replaces
   gx_set_menu_wide_cb's when both are set) asked once per frame like it: true = this frame's 3D covers the whole active frame, and
   xf = {kx, tx, ky, ty} places everything the frame draws OUTSIDE a gx_menu_sprite(1) bracket: clip x' = x * kx + tx * w,
   y' = y * ky + ty * w, in the frame's own clip units after the native 4:3 box has been centred in it (so {1,0,1,0} is the DS layout).
   gx_menu_sprite_xf, called inside a bracket, gives that one sprite object its own {kx, tx, ky, ty} (the bracket's end clears it). */
void gx_set_menu_scene_cb(bool (*cb)(float xf[4]));
void gx_menu_sprite_xf(float kx, float tx, float ky, float ty);
bool gx_menu_scene_active();     // the scene callback is installed and this frame's answer is yes

// THE TOON TABLE (GBATEK, DS 3D Toon/Edge/Fog: TOON_TABLE 0x04000380, 32
// BGR555 halfwords; DS 3D Polygon Attributes: mode 2). A mode-2 polygon's
// vertex RED picks one of the 32 entries, per pixel, from the interpolated
// colour. With DISP3DCNT bit 1 clear (toon) the entry REPLACES the vertex
// colour and the texture modulates it as usual; with it set (highlight) the
// vertex colour's green and blue take its red, it modulates as usual and the
// entry is ADDED after, saturating. The mode-2 users: the cap morph
// (src/game/player/Player.cpp, func_ov002_020be3b0) strips the textures, sets mode 2
// and rotates a rainbow into the table every tick; func_ov002_020beabc (a level
// entered with a changed character, morph state 5); and ov007, the title menu's
// Mario head and the file select's vortex (func_ov007_020b2bd4, the table from
// func_ov007_020c93b4; the vortex runs in highlight mode). gx_render reads the table
// and DISP3DCNT once per frame, only on a frame that submits a mode-2 polygon.
// SM64DS_TOON_OFF=1 draws mode 2 as modulation again (the old picture).
int gx_toon_table(float rgb[32 * 3]);

// THE 3D COVERAGE MASK: one byte per host framebuffer pixel, 1 where the LAST
// gx_render actually wrote a pixel (opaque, translucent or shadow), 0 where it
// left the framebuffer alone. SCREEN_W stride, SCREEN_H rows.
//
// WHY IT EXISTS. On the DS the 3D engine is BG0 in 3D mode, and engine A's 2D
// unit composites the four BGs and the sprites against it BY PRIORITY: a BG
// whose priority number is higher than BG0's sits BEHIND the 3D layer and
// cannot paint over it. The port renders 3D into the framebuffer and then runs
// hal/message_compositor.cpp over the top, so without this mask that
// compositor cannot tell a pixel the 3D engine drew from a pixel the frame's
// clear colour left, and every 2D layer covers 3D unconditionally.
//
// Measured on scene 390 (dScMgFlower_c) before the mask existed: BG2 at
// priority 3 owned 48357 of the top screen's 49152 pixels while the 3D layer
// at priority 1 submitted 878 polygons a frame, so 100% of the minigame's
// flower and Yoshi were drawn and then buried.
//
// VALID ONLY IMMEDIATELY AFTER gx_render. It is cleared at the head of every
// gx_render call, so a reader that runs on a frame the port did not rasterise
// is looking at the previous frame's coverage; both live callers sit directly
// after the gx_render that filled it.
const uint8_t *gx_coverage();

// The polygon-ID plane and the depth plane the same pass fills, on the same
// contract as the coverage mask above: SCREEN_W stride, SCREEN_H rows, valid
// only immediately after gx_render. Read-only, and for the same reason the
// mask is public: a caller that has to ask "which surface won this pixel" or
// "was the depth written" is asking about a value, not about a colour.
const uint8_t *gx_attr_ids();
const float *gx_depth();

void gx_debug_proj(float out[16]);

// The matrix stacks' current depth, for GXSTAT bits 8-12 (position/vector) and
// bit 13 (projection). Read-only; ntr/io.cpp uses it to answer the two linked
// stack-level readers with the host's real state instead of a dead latch.
void gx_matrix_stack_levels(unsigned &pos_level, unsigned &proj_level);

// The live POSITION matrix, row-vector convention like everything else here.
// For a caller that needs to report what its own vertices will be transformed
// by; see the definition for why the projection alone cannot answer it.
void gx_debug_pos(float out[16]);
// the live matrix state, for tracking down a mis-placed draw
void gx_debug_matrices(int *mode, float pos[16], float proj[16]);

// The latched VIEWPORT rectangle (command 0x60 / port 0x4000580), in HOST
// framebuffer pixels -- already scaled off the DS panel's 256x192 by the
// command handler, so at the 2x tier a full-screen DS viewport reads 512x384.
// Read-only, and the answer to "did the game ask for a corner or did the port
// draw one": a wrong rectangle here and a wrong rectangle on screen are the
// same fact, and a right one rules the projection out by name.
//
// `sets` is how many VIEWPORT commands executed since the last gx_reset, and
// it is not optional decoration. gx_reset restores the rectangle to a
// FULL-SCREEN default, so the rectangle alone cannot tell a game-issued
// full-screen viewport from a frame that issued none. Zero sets means the
// rectangle read back is the default and says nothing about the game.
void gx_debug_viewport(int &x, int &y, int &w, int &h, int &sets);

// THE GEOMETRY COMMAND CENSUS. `counts` is indexed by GBATEK command byte and
// holds how many of each EXECUTED since the last take; `ports` and `fifo` are
// how many of them arrived through gx_write_port and gx_write_fifo, which is
// the difference between a caller that reached the register file and one whose
// stores landed in mapped memory; `swap_param` is the last SWAP_BUFFERS
// parameter word; `resets` is gx_reset calls, so a sampler can say how much
// engine state was thrown away between two samples. `take` zeroes the interval
// counters, which makes a per-frame sampler a per-frame census.
//
// The counters live outside the geometry State, so gx_reset does not clear
// them. That is the property the instrument is for.
void gx_debug_commands(uint32_t counts[256], uint32_t &ports, uint32_t &fifo,
                       uint32_t &swap_param, uint32_t &resets, bool take);

// ---- MOTION INTERPOLATION (run interp1, the SmoothMotion key) -------------
//
// PRESENTATION ONLY. With the key on, the host records each level frame's
// geometry command stream (everything exec() ran between the gx_reset that
// opened the frame and its gx_render) and can draw an EXTRA picture between two
// game ticks by replaying that record with every matrix parameter blended
// between the previous tick's value and this tick's. The game's code runs once
// per tick exactly as before; a replay is never visible to it (GXSTAT, the
// census counters and the swap latch stand down while one runs). The full
// description is the banner over the implementation in ntr/gx.cpp.
//
// With gx_interp_arm never called (the default) nothing is recorded and none
// of the functions below does anything.
struct GxInterpStats {
    int groups, matched, unmatched, teleport, shape;
    int snap;             // why the frame cannot blend (0 = it can); bits:
                          // 1 caller, 2 no previous tick, 4 texture cache
                          // dropped, 8 no sealed record, 16 camera cut
    float eye_step;       // camera eye movement since the previous tick
    float turn_deg;       // camera view-axis turn since the previous tick
    float max_moved;      // largest world move among the paired groups
};
void gx_interp_arm(int on);
int gx_interp_armed();
// runtime.cpp's GXFIFO copy names the display list it is streaming (0 after).
void gx_dma_source(uint32_t src);
// The next commit has no previous tick to blend from.
void gx_interp_invalidate();
// After the tick's gx_render: key the record, pair it with the previous one.
// `view` is the ROM's live view matrix (data_0209b3ec). Returns 1 when the
// frame can blend.
int gx_interp_commit(const int32_t view[12], int snap, GxInterpStats *st);
int gx_interp_ready();
// The blended camera's eye, for the probe.
void gx_interp_view(float alpha, float eye[3]);
// FADEFLASH1: the geometry drawn between these two calls is fixed to the screen (the fader wipe's mesh); a picture
// between ticks draws it with this tick's own matrices, never blended or re-viewed. Inert unless a frame is recording.
void gx_interp_screen_begin();
void gx_interp_screen_end();
// Replay the current record at alpha (0 = the previous tick, 1 = this tick)
// into the polygon list, with the live engine state saved; gx_render then
// draws the blended picture, and gx_interp_end puts the live state back.
int gx_interp_begin(float alpha);
void gx_interp_end();
// Probe helpers: the screen box of one draw group's triangles in the replayed
// list (between begin and end), and the group count.
int gx_interp_group_box(int group, float box[4]);
int gx_interp_group_count();
int gx_interp_group_flags(int gi);   /* probe: bit 0 particle, bit 1 paired */
void gx_interp_dump_groups(void *file, int frame);


}  // namespace ntr

#endif  // NTR_GX_H
