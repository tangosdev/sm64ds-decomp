/* THE OPTIONAL GRAPHICS-CARD RENDERER. See hal/gpu_raster.h for what the
 * setting is and hal/gpu_device.h for the one device it shares with the
 * present path.
 *
 * ================== THE WHOLE FRAME (run cardfull, CARDFULL1) ==================
 *
 * Everything below this block was written for a card that draws the OPAQUE pass
 * only. Since run cardfull the card draws the SECOND pass too -- the
 * translucent polygons, the mode-3 shadow volumes and the mode-2 toon and
 * highlight polygons the opaque pass already handled -- and the edge smoothing
 * as well, all on its own targets, and the picture comes back to the processor
 * ONCE, finished. SM64DS_RENDERER_FULL picks it: 1 (the default) draws the whole
 * frame there, 0 is the opaque-only card described below, unchanged. That opaque-only card
 * is also what a card older than Direct3D 11 (feature level 11_0) gets, because
 * the edge smoothing needs shader model 5, and what any run gets after the
 * whole-frame mode has given up (one line says so).
 *
 * THE STENCIL BUFFER IS WHAT MAKES THE SECOND PASS EXPRESSIBLE. gx.cpp keeps
 * three per-pixel pieces of state for it, and an 8-bit stencil holds all three:
 *
 *   bit 7 (0x80)   the shadow mask bit (gx.cpp's g_stencil)
 *   bit 6 (0x40)   "this pixel took a translucent fragment this frame"
 *   bits 0..5      that fragment's polygon ID           (together: g_tlattr,
 *                                                        0x40 | polyid)
 *   and the OPAQUE polygon ID (g_attrid), which a shadow polygon compares
 *   against, stays in the polygon-ID colour target and is READ by the shadow
 *   shader as a texture. It is written only by the opaque pass, so it never
 *   needs to be both read and drawn in one draw.
 *
 *   translucent polygon   stencil test NOT_EQUAL against 0x40|polyid under a
 *                         0x7F mask (the shadow bit does not take part), depth
 *                         test LESS, no depth write; a fragment that passes
 *                         REPLACEs the low seven bits with 0x40|polyid. That is
 *                         the equal-ID refusal: a pixel that already took this
 *                         ID refuses the next fragment of it.
 *   shadow mask           depth LESS; where the DEPTH TEST FAILS the stencil
 *   (mode 3, ID 0)        op sets bit 7. No colour, no depth, no pixel shader.
 *   shadow polygon        stencil test EQUAL on bit 7 (the mask is set); every
 *   (mode 3, ID != 0)     fragment that reaches the output merger clears bit 7
 *                         (ZERO, whether the depth test passes or not), and the
 *                         pixel shader draws only where the opaque polygon ID
 *                         underneath differs. It never discards, because a
 *                         discarded fragment skips the stencil op and gx.cpp
 *                         clears the bit at every stencilled pixel the polygon
 *                         touches; "nothing to draw" is a colour alpha of 0.
 *   mask groups           a new mask group clears bit 7 of the whole picture,
 *                         which is one full-screen draw with a write mask of
 *                         0x80, at exactly the triangles gx.cpp's g_clr_cum
 *                         counts.
 *   an opaque fragment    in the second pass (a translucent-class polygon whose
 *   in the second pass    texel came out at full alpha) writes depth and clears
 *                         the translucent record (stencil ZERO under 0x7F), so
 *                         a polygon that can reach full alpha is drawn twice,
 *                         once for the pixels under 31 and once for the pixels
 *                         at 31 (see hal/gpu_raster.hlsl); a card cannot write
 *                         depth for some pixels of a draw and not others.
 *
 * THE COLOUR TARGET'S ALPHA IS COVERAGE in this mode (a MAX blend on the alpha
 * channel only), which frees the polygon-ID target from carrying it and lets
 * the edge smoothing read colour and coverage out of one texture.
 *
 * WHERE THIS DIFFERS FROM gx.cpp, beyond the opaque pass's own list below:
 *   - THE BLEND ROUNDS TO NEAREST and gx.cpp's integer divide by 31 truncates,
 *     so a translucent pixel can land one step higher.
 *   - A POLYGON THAT CAN REACH FULL ALPHA is drawn as two halves per batch (the
 *     pixels under 31, then the pixels at 31), not interleaved triangle by
 *     triangle. Overlapping triangles of one batch that mix the two classes can
 *     therefore resolve in the other order.
 *   - A FRAGMENT WITH A BLEND WEIGHT OF ZERO still marks coverage (the weight
 *     is held at 1/1024, which the colour rounds away).
 *
 * ============================ THE SEAM ====================================
 *
 * ntr/gx.cpp draws a frame in two passes over the same buffers: an OPAQUE pass
 * (colour into the framebuffer, a float depth with a LESS test, a per-pixel
 * polygon ID, the 3D coverage mask) and then a TRANSLUCENT pass (blended
 * polygons with the equal-ID refusal, and mode-3 shadow volumes with their
 * stencil protocol) which READS what the opaque pass left. The opaque pass is
 * nearly all of the fill and none of the awkward state.
 *
 * So this file draws the opaque pass, and only the opaque pass, into three
 * offscreen targets and reads them straight back into the very buffers the
 * software opaque pass would have filled. gx.cpp then runs its translucent
 * pass, unchanged, over them. Everything downstream -- the edge smoothing, the
 * 2D compositor, the DS display capture, the selftest bitmap, the present path
 * -- sees an ordinary finished frame and cannot tell which rasteriser drew it.
 *
 * ========================= MATCHING THE SOFTWARE ==========================
 *
 * GEOMETRY arrives already in host screen space: x and y in pixels of the
 * active render size, z in [0,1], w kept, u and v in texel units. The clip
 * position is rebuilt on the CPU as (x_ndc * w, y_ndc * w, z * w, w) so that
 *   - u and v interpolate perspective-correct, which is what gx.cpp does
 *     (it interpolates u/w, v/w and 1/w and divides), and
 *   - SV_Position.z comes out as the plain screen-space interpolation of z,
 *     which is what gx.cpp's depth is (l0*a.z + l1*b.z + l2*c.z, no 1/w).
 * The vertex colour is declared noperspective for the same reason: gx.cpp
 * interpolates colour screen-linear. All three were read out of the raster
 * before this was written; see hal/gpu_raster.hlsl.
 *
 * DEPTH is D32_FLOAT with a LESS comparison, which keeps the FIRST triangle
 * submitted at a tie exactly as gx.cpp's `if (z >= depth) continue` does.
 * DepthClipEnable is FALSE because the software raster has no z clipping at
 * all: the near clip is done on the CPU before a triangle ever gets here and
 * there is no far clip.
 *
 * CULLING stays on the CPU, from the same signed screen area and the same
 * POLYGON_ATTR bits, and the card is told to cull nothing. A degenerate
 * triangle is dropped by the same 1e-6 test.
 *
 * THE SCISSOR is the present rectangle, which is what gx.cpp clamps its
 * bounding box to.
 *
 * COVERAGE TRAVELS IN THE POLYGON-ID TARGET. gx.cpp keeps a one-byte coverage
 * mask beside the framebuffer, and the 2D compositor needs it to know which
 * pixels the 3D engine wrote. It cannot come from the colour target's alpha
 * here because that alpha drives the blend, so the R8_UINT ID target carries
 * 0x80 | polygon_id: bit 7 is "this engine wrote this pixel" and the low six
 * bits are the DS's six-bit polygon ID.
 *
 * THE CLEAR COLOUR is read out of the framebuffer at the top of the frame
 * rather than hard-coded, and the readback COPIES ONLY COVERED PIXELS, so a
 * pixel the opaque pass did not reach keeps exactly the bytes the caller's own
 * clear put there. That is what makes this independent of whether the caller's
 * clear is one colour or many.
 *
 * ========================== WHERE IT DIFFERS ==============================
 *
 * 1. THE FILL RULE. This card uses Direct3D's top-left rule, one pixel one
 *    triangle; gx.cpp accepts a pixel on every edge, so two triangles sharing
 *    an edge both cover it and the depth test settles them. Along a shared
 *    edge both triangles interpolate to the same values, so the colour agrees
 *    to within float rounding; on a silhouette the two rasterisers disagree by
 *    whole pixels. That is why this is not a byte gate and why the in-process
 *    A/B excludes pixels within one pixel of a coverage or ID edge.
 *
 * 2. TEXTUREFILTER 1 AND 2. With filtering on, a filtered texel's alpha can
 *    land between 0 and full. This file used to let that partial alpha reach
 *    the blend unit AND the depth write, so the rim of every cut-out came out
 *    darkened by the clear colour and, because depth was written, whatever
 *    was drawn after it could not fill the rim in (the skybox showed through
 *    as an outline). Run hunt7 (lane FILTEREDGE1) changed the filtered arms
 *    to the rule TextureFilter 0 follows: a SECOND, point sampler pinned to
 *    the top level picks the texel the DS would have picked, and that texel
 *    alone decides whether the pixel exists and whether it is fully opaque;
 *    the filter supplies only the colour. The texture is held premultiplied
 *    (16 bits a channel) on these arms and the pixel shader divides the
 *    alpha back out, which is gx.cpp's alpha-weighted blend, so a transparent
 *    texel's colour never bleeds into a visible edge. See ps_filt in
 *    gpu_raster.hlsl and filtered_texel_cover in ntr/gx.cpp. At TextureFilter
 *    0 -- the arm this is gated on -- none of this runs: the shader, the
 *    sampler and the texture are the ones from before.
 *
 * 3. MIP CHAINS. TextureFilter 2 asks the card to generate the chain, which is
 *    a plain box filter; gx.cpp builds its own chain in premultiplied alpha so
 *    a cut-out does not drag its transparent colour outward. Same arm, same
 *    reason, not gated.
 *
 * 4. THE DEGENERATE-W FALLBACK. gx.cpp falls back to an affine UV when the
 *    interpolated 1/w is under 1e-9. The card has no such case; a vertex with
 *    w at zero is left to the hardware.
 *
 * ============================== FAILURE ===================================
 *
 * Every call is checked. The first failure prints one plain line and sets a
 * flag; from then on this file answers "I did not draw" and gx.cpp runs its
 * own opaque pass for that frame and every frame after. Nothing is left half
 * written, because the CPU buffers are only touched after all three readbacks
 * have succeeded.
 */
#if defined(_WIN32)

#include <windows.h>
#include <d3d11.h>
#include <emmintrin.h>
#include "hitch_probe.h"   // HITCH9C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <atomic>
#include <vector>

#include "hal/gpu_raster.h"
#include "hal/gpu_device.h"
#include "hal/gpu_raster_shaders.h"
#include "hal/host_settings.h"
#include "ntr/gx.h"
#include "ntr/smooth.h"

namespace {

/* ---- the settings and the test knobs, read once -------------------------- */

int g_on;              /* the key said yes */
int g_started;
int g_down;            /* fallen back for the rest of the run */
/* THE CARD COMES BACK AFTER A LOST CARD (run after093, lane MGFREEZE3). A
   g_down that a removed or reset card caused is not for the rest of the run
   any more: once hal/gpu_present.cpp has a new device and has presented on it
   (g_renewed), the next frame makes this file's card objects again through
   the same path the first frame took (card_return). Two failed tries in a row
   and the 3D stays on the processor, with one line. */
int g_down_lost;       /* g_down came from a lost card */
int g_renewed;         /* the present path has a new device */
int g_return_fails;    /* tries in a row that did not bring the card back */
int g_returns;         /* times the card came back this run */
int g_want_warp = -1;  /* SM64DS_RENDERER_DEVICE */
int g_perf;            /* SM64DS_RENDERER_PERF: the cost breakdown */
int g_fail_after;      /* SM64DS_RENDERER_FAIL_AFTER: the fallback drill */
int g_fail_readback;   /* SM64DS_RENDERER_FAIL_READBACK: the other drill */
/* SM64DS_RENDERER_FAIL_DEVICE: the third drill, and the only one that cannot
   be reached any other way on a machine that has a working card. It takes the
   SAME branch, with the same message, that a box with no Direct3D 11 at all
   takes: the setting is on, no device starts, the game draws every frame the
   ordinary way and says so once. */
int g_fail_device;
/* THE WHOLE-FRAME MODE (run cardfull). SM64DS_RENDERER_FULL=1 turns it on,
   0 is the opaque-only card of run hd2. g_full_dead is the same one-way latch
   g_down is, for this part alone: a card that cannot do the second pass falls
   back to the opaque-only path for the rest of the run and keeps drawing. */
int g_full_on;
int g_full_dead;
int g_fail_full;       /* SM64DS_RENDERER_FAIL_FULL: the drill for the above */
int g_aa_on_full = -1; /* SM64DS_RENDERER_AA: 0 keeps the smoothing on the processor */
long long g_full_smooth, g_pre_kept;   /* frames smoothed whole-frame, and how many kept the pre-smoothing copy */

/* ---- THE DEPTH RANGE, AND WHY IT IS HALVED ------------------------------
   ntr/gx.cpp clears its depth buffer to 1e30 and tests LESS, so "nothing has
   been drawn here" is a value no geometry can reach. A depth target has no
   such value: the card's viewport transform clamps to [MinDepth, MaxDepth],
   both of which must lie in [0, 1], so a clear of 1.0 is a value geometry CAN
   reach -- and does. The screen z ntr/gx.cpp computes is
   (clip.z / clip.w + 1) * 0.5 with no far clip at all, so anything past the
   projection's far plane comes out ABOVE 1.0. Measured, Bob-omb Battlefield's
   sky on the first frame of the level, through SM64DS_PROBE_PX:
     z = 1.00022 at all three vertices
   Clamped to 1.0 that fails LESS against a 1.0 clear, so the whole sky and
   every far surface simply was not drawn: the card's first frames of levels
   6, 8 and 12 came back empty and filled in over about thirty frames as the
   intro camera brought the geometry in under z = 1.

   So the card is handed HALF the depth and the readback doubles it back.
   Halving and doubling are exact in binary floating point, so ordering, the
   LESS tie-break (equal z stays equal, and the first submitted keeps the
   pixel) and the value the software translucent pass reads are all unchanged
   to the last bit. The range this buys is z < 2.0, against a measured maximum
   of about 1.0002: a surface at z >= 2.0 would still clamp and lose, and a
   projection that produced one would be far outside anything this game has
   been measured to make. */
const float kDepthScale = 0.5f;     /* z on the card */
const float kDepthUnscale = 2.0f;   /* and back again, both exact */

/* ---- THE W-BUFFERED FRAME (the card half of ntr/gx.cpp's g_wdepth) ---------
   The DS depth-buffers on W (the game writes SWAP_BUFFERS = 3 every frame), and
   the software raster keys on -(1/w), screen-linear. A card frame used to keep
   the post-divide z, which in float sits within an ulp of 1.0 over the far half
   of a perspective scene (1 ulp ~ 0.5 world units at w ~ 3900), so the
   translucent pass, reading that depth back, let a pond's water plane through
   the far shore on a lattice of pixels: the dotted band along the water's far
   edge at the castle grounds and the Tall Tall Mountain waterfall. Ordering a
   frame by W on the card is a clip-space trick: with clip z held CONSTANT the
   hardware's z/w divide gives kWDepthC / w, which is linear in screen space
   (the same interpolation as -(1/w)), larger for nearer, and floating point is
   densest near zero, so the far range keeps its relative precision. Nearer is
   larger, so a W frame tests GREATER and clears to 0. The readback turns the
   card's c/w into the software key -(1/w) by multiplying with -1/c (a power
   of two, exact); a pixel nothing covered is never copied, so it keeps the
   software's own clear. kWDepthC is far below any w the camera produces
   (the viewport clamps depth to [0, 1], so c / w must stay under 1). A frame
   gx_render did not mark wdepth (an orthographic scene, SM64DS_WDEPTH=0) draws
   exactly as before. */
const float kWDepthC = 1.0f / 256.0f;
const float kWDepthUnscale = -256.0f;
float g_depth_unscale = kDepthUnscale;   /* what this frame's readback multiplies by */
bool g_frame_w;                          /* this frame is W-buffered */
int g_addrcheck;       /* SM64DS_RENDERER_ADDRCHECK: the six DS address ranges */

/* ---- what the run learned, for the one line at exit ---------------------- */

long long g_frames, g_tris, g_batches, g_verts;
double    g_ms_build, g_ms_draw, g_ms_read;
int       g_perf_n;
double    g_perf_build, g_perf_draw, g_perf_read;
/* the readback split in two: the Map's wait for the card, and the copy of the
   covered pixels into the software rasteriser's buffers */
double    g_perf_wait, g_perf_copy;
long long g_perf_tris, g_perf_batches;

/* ---- the device and the pipeline ----------------------------------------- */

ID3D11Device        *g_dev;
ID3D11DeviceContext *g_ctx;

ID3D11VertexShader   *g_vs;
ID3D11PixelShader    *g_ps;
ID3D11PixelShader    *g_ps_filt;   /* the same pixel under TextureFilter 1 and 2 */
ID3D11InputLayout    *g_layout;
ID3D11RasterizerState *g_rast;
ID3D11DepthStencilState *g_dss, *g_dss_w;
ID3D11BlendState     *g_blend;
ID3D11Buffer         *g_vb;
UINT                  g_vb_verts;
/* the whole-frame pipeline (run cardfull), built on the first frame that wants
   it: the translucent, translucent-opaque and shadow pixel shaders, the
   full-screen vertex shader the mask clear and the smoothing share, the five
   depth-stencil states and the blend that carries coverage in alpha */
ID3D11VertexShader      *g_full_vs;
ID3D11PixelShader       *g_ps_tl, *g_ps_tlop, *g_ps_shadow, *g_ps_faa;
/* the second pass's three pixel shaders under TextureFilter 1 and 2 (_f: the
   texture is held premultiplied and coverage comes from the point sampler, run
   hunt7 lane FILTEREDGE1); the opaque pass takes g_ps / g_ps_filt as it always
   did */
ID3D11PixelShader       *g_ps_tl_f, *g_ps_tlop_f, *g_ps_shadow_f;
/* depth-stencil states by depth rule, index (W frame ? 2 : 0) + (equal-test
   polygon ? 1 : 0): LESS, LESS_EQUAL, GREATER, GREATER_EQUAL (run hunt8,
   lane CARDPORT1: the W frame and the POLYGON_ATTR bit 14 test on the whole
   frame path) */
ID3D11DepthStencilState *g_dss_v[4];
ID3D11DepthStencilState *g_dss_tl[4], *g_dss_tlop[4], *g_dss_mask[4],
                        *g_dss_shadow[4], *g_dss_clrm;
/* the ghost layer's states (run mp1, lane GHOSTLAYER1), index 0 for a Z frame,
   1 for a W frame: depth only, and colour at depth EQUAL with a
   one-fragment-per-pixel stencil */
ID3D11DepthStencilState *g_dss_gz[2], *g_dss_gc;
ID3D11BlendState        *g_blend_full;
bool                     g_full_ready;
/* the last frame drawn whole: whether there is one, and the box of pixels it
   drew, for the depth and polygon-ID planes gx_depth and gx_attr_ids may ask
   for afterwards (ntr/gx.h, GxGpuFrame::planes) */
bool                     g_last_full, g_last_box;
int                      g_lb_x0, g_lb_y0, g_lb_x1, g_lb_y1;
/* THE TOON TABLE the pixel shader reads for a mode-2 polygon (the
   ToonTable constant buffer, 32 float4, rgb 0..255): ntr/gx.cpp's frame table,
   uploaded only on a frame that has one and only when it changed */
const int             kToonCbFloats = (32 + 2 + 9) * 4;   /* toon, fog params, fog colour, 34 densities */
ID3D11Buffer         *g_toon_cb;
float                 g_toon_seen[kToonCbFloats];
bool                  g_toon_valid;

/* [filter][addressU][addressV]; address 0 clamp, 1 wrap, 2 mirror */
ID3D11SamplerState   *g_smp[3][3][3];
/* the point sampler pinned to the top level that ps_filt takes coverage from,
   [addressU][addressV] */
ID3D11SamplerState   *g_psmp[3][3];

/* the one-pixel opaque white texture an untextured polygon binds, so the
   shader has no branch and gx.cpp's own `texel = 0xFFFFFFFF` is literally
   what it samples */
ID3D11Texture2D          *g_white;
ID3D11ShaderResourceView *g_white_srv;

/* ---- the three targets and their staging copies -------------------------- */

ID3D11Texture2D        *g_col, *g_col_stage;
ID3D11RenderTargetView *g_col_rtv;
ID3D11Texture2D        *g_id, *g_id_stage;
ID3D11RenderTargetView *g_id_rtv;
ID3D11Texture2D        *g_dep, *g_dep_stage;
ID3D11DepthStencilView *g_dep_dsv;
int                     g_rt_w, g_rt_h;
/* the whole-frame targets' extras: the colour and polygon-ID targets are also
   read as textures (the smoothing reads colour, the shadow shader reads the
   ID), the depth target carries the stencil, and the smoothing has its own
   result target and staging copy */
bool                     g_rt_full;
ID3D11ShaderResourceView *g_col_srv, *g_id_srv;
ID3D11Texture2D          *g_fa_out, *g_fa_st;
ID3D11RenderTargetView   *g_fa_rtv;
ID3D11ShaderResourceView *g_fa_srv;     /* the composite reads the smoothed picture */

/* ---- THE PICTURE HELD ON THE CARD (run cardfull, lane CARD2) ---------------
   See the banner above cp_keep_ok. g_cp_on is the setting (SM64DS_PRESENT_FROM_CARD),
   g_cp_dead the one-way latch every failure sets, g_cp_held whether the last
   whole frame is still on the card unread, and the rest is what a settle, the
   capture picture or the composite needs to find it again. */
int       g_cp_on;
int       g_cp_dead;
int       g_cp_fail_after;       /* SM64DS_PRESENT_FROM_CARD_FAIL_AFTER */
int       g_cp_check;            /* SM64DS_PRESENT_FROM_CARD_CHECK */
bool      g_cp_held;
bool      g_cp_smoothed;
int       g_cp_box[4];           /* x0 y0 x1 y1 of the pixels the card drew in */
int       g_cp_cw, g_cp_ch, g_cp_px0, g_cp_py0, g_cp_pw, g_cp_ph;
uint32_t  g_cp_clear, g_cp_outside;
int       g_cp_fade, g_cp_evy, g_cp_white;
int       g_cpt_fade, g_cpt_evy, g_cpt_white, g_cpt_shown3d, g_cpt_clear_opaque, g_cpt_mask_stride;
/* run hunt8, lane INTERPFIX1: a fade in motion between two ticks. The next
   pictures between ticks take this coefficient and colour instead of the
   tick's own (under the tick's own brightness mask when it had one); off when 0. */
int       g_ipo_on, g_ipo_evy, g_ipo_white;
const uint8_t *g_cpt_mask;   /* the last composited tick's fade (FADEINTERP1) */
long long g_cp_composed, g_cp_checked, g_cp_bad_frames, g_cp_bad_px;
long long g_cp_settled, g_cp_captured;
double    g_cp_ms;
/* the brightness mask of the held frame (hal/message_compositor.cpp), when its
   fade only takes some pixels: one byte per picture pixel, `g_cp_mask_stride`
   to a row, and the two facts the per-pixel answer needs besides coverage */
const uint8_t *g_cp_mask;
int       g_cp_mask_stride, g_cp_shown3d, g_cp_clear_opaque;
ID3D11Texture2D          *g_ov_tex, *g_comp_tex, *g_comp_stage, *g_bm_tex;
bool                     g_comp_valid;   /* g_comp_tex holds a finished composite (run hunt8, CARDSM1) */
ID3D11ShaderResourceView *g_ov_srv, *g_comp_srv, *g_bm_srv;
/* TEXTBOX2: an overlay of zeros (no 2D anywhere), same size as the overlay; and the ring a picture between two ticks cuts the 2D out of (an opening dialogue box) */
ID3D11Texture2D          *g_zov_tex;
ID3D11ShaderResourceView *g_zov_srv;
int                       g_cut_on, g_cut_o[4], g_cut_i[4], g_cut_closing;
ID3D11RenderTargetView   *g_comp_rtv;
/* run hunt8, lane GPUINTERP1: the composite of a picture BETWEEN two ticks has
   a target of its own, so the tick's composite (g_comp_tex) stays whole for the
   tick's own present. g_comp2_valid: it holds a finished in-between. */
ID3D11Texture2D          *g_comp2_tex;
ID3D11ShaderResourceView *g_comp2_srv;
ID3D11RenderTargetView   *g_comp2_rtv;
bool                     g_comp2_valid;
ID3D11Buffer             *g_comp_cb;
ID3D11PixelShader        *g_ps_comp;
int                       g_cp_w, g_cp_h;

/* ---- ENGINE A'S 2D ON THE CARD (run hunt8, lane TWODGPU1; the section is
   "TWODGPU1" below cp_compose_inter, the shaders' are marked the same in
   gpu_raster.hlsl). The overlay (g_ov_tex) and the brightness mask (g_bm_tex)
   are also render targets, so the passes of that section can draw the tick's
   2D into the textures comp_ps already reads. */
ID3D11RenderTargetView   *g_ov_rtv, *g_bm_rtv;
ID3D11Texture2D          *g_tw_wm_tex, *g_tw_up_tex, *g_tw_st_ov, *g_tw_st_bm;
ID3D11ShaderResourceView *g_tw_wm_srv, *g_tw_up_srv, *g_tw_inst_srv;
ID3D11Buffer             *g_tw_inst_buf, *g_tw_cb;
ID3D11PixelShader        *g_tw_ps_hide, *g_tw_ps_bmdef, *g_tw_ps_rect, *g_tw_ps_inset, *g_tw_ps_boxup;
ID3D11VertexShader       *g_tw_vs_rect, *g_tw_vs_box;
int                       g_tw_w, g_tw_h;          /* size g_tw_up_tex was made for */
int                       g_tw_env = -1;           /* SM64DS_TWOD_CARD (default on) */
int                       g_tw_check_on = -1;      /* SM64DS_TWOD_CARD_CHECK */
bool                      g_tw_live;               /* the tick's overlay is in g_ov_tex */
bool                      g_tw_bm;                 /* ... and its mask in g_bm_tex */
PortTwodFrame             g_tw_p;
unsigned char            *g_tw_cpu_mask;
int                       g_tw_cpu_mask_stride;
const uint32_t           *g_tw_fb;                 /* the framebuffer whose other 2D is merged at the composite */
long long                 g_tw_ticks, g_tw_rects, g_tw_flushes, g_tw_folds, g_tw_checks, g_tw_bad;
long long                 g_tw_bad_px_ov, g_tw_bad_px_bm;
double                    g_tw_ms, g_tw_flush_ms;

/* ---- SAVEOVL1: THE MENU LAYER --------------------------------------------
   A menu recipe whose pieces are all plain (the level-clear save menu) is
   drawn by the processor into a scratch buffer of its own (premultiplied
   0xAARRGGBB, zero = nothing) instead of onto the picture, so the picture
   can stay on the card. The card lays the layer over its composite, before
   or after the fade as the recipe's stage says (comp_ps, t4), for the tick
   and for every picture drawn between ticks; the processor's copy of the same
   rule (cp_resolve) applies it on a settle. */
ID3D11Texture2D          *g_lay_tex, *g_lay2_tex;
ID3D11ShaderResourceView *g_lay_srv, *g_lay2_srv;
bool                      g_lay_two;               /* this tick's layer has a second plane in g_lay2_tex */
bool                      g_lay2_tex_dirty;        /* g_lay2_tex may hold non-zero words */
std::vector<uint32_t>     g_lay2_cpu;              /* the second plane's scratch, same stride and box as the first */
int                       g_lay_w, g_lay_h;        /* size g_lay_tex was made for */
bool                      g_lay_live;              /* this tick has a layer in g_lay_tex */
int                       g_lay_post;              /* 1 = after the fade */
std::vector<uint32_t>     g_lay_cpu;               /* the scratch, g_lay_stride words a row */
int                       g_lay_stride;
int                       g_lay_cpu_box[4];        /* x0 y0 x1 y1 of what the scratch holds (zero elsewhere) */
int                       g_lay_tex_box[4];        /* ... and of what the texture may hold */
int                       g_lay_env = -1;          /* SM64DS_MENU_LAYER (default on) */
long long                 g_lay_ticks, g_lay_up_px, g_lay_checked, g_lay_inter;

/* ---- the texture cache on the card --------------------------------------- */

struct GTex {
    ID3D11Texture2D          *tex;
    ID3D11ShaderResourceView *srv;
    int w, h;
};
std::map<uint32_t, GTex> g_tex;
uint32_t g_tex_gen_seen;
size_t   g_tex_bytes;

/* ---- small helpers ------------------------------------------------------- */

long long qpc()
{
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return c.QuadPart;
}
long long g_qpf;
double ms_between(long long a, long long b)
{
    if (!g_qpf) return 0.0;
    return (double)(b - a) * 1000.0 / (double)g_qpf;
}

int env_int(const char *name, int dflt)
{
    const char *e = getenv(name);
    if (!e || !*e) return dflt;
    char *end = 0;
    const long v = strtol(e, &end, 10);
    return (end != e) ? (int)v : dflt;
}

/* A LOST CARD IS ALSO THE PRESENT PATH'S BUSINESS (run after093, lane
   MGFREEZE2). The renderer and the present path share one device; when a
   readback here is the first to hear the card was removed or reset, the
   present path is told at once, so it rebuilds itself on a new device instead
   of refusing its next sixty uploads. A hook and not a call, because the smoke
   runner links this file without hal/gpu_present.cpp. */
void (*g_lost_hook)(long);
bool lost_code(HRESULT hr)
{
    return hr == (HRESULT)0x887A0005L || hr == (HRESULT)0x887A0006L ||
           hr == (HRESULT)0x887A0007L || hr == (HRESULT)0x887A0020L;
}

/* ONE LINE, AND THEN THIS RUN DRAWS IN SOFTWARE FOR GOOD. Never an exit, never
   a stall: the caller returns 0 and gx.cpp's own opaque pass draws the frame
   that was about to be drawn here, so not even one picture is lost. */
void fall_back(const char *why, HRESULT hr)
{
    if (g_down) return;
    g_down = 1;
    if (lost_code(hr)) g_down_lost = 1;
    if (lost_code(hr) && g_lost_hook) g_lost_hook((long)hr);
    if (hr)
        fprintf(stderr, "[renderer] the graphics card stopped drawing the 3D "
                        "picture (%s, code %08x). This run goes back to drawing "
                        "it the ordinary way; nothing else changes.\n",
                why, (unsigned)hr);
    else
        fprintf(stderr, "[renderer] the graphics card cannot draw the 3D picture "
                        "(%s). This run draws it the ordinary way; nothing else "
                        "changes.\n", why);
}

/* ---- the pipeline objects, made once ------------------------------------- */

bool make_pipeline()
{
    HRESULT hr = g_dev->CreateVertexShader(kGpuRasterVS, sizeof kGpuRasterVS, 0,
                                           &g_vs);
    if (FAILED(hr)) { fall_back("the vertex shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuRasterPS, sizeof kGpuRasterPS, 0, &g_ps);
    if (FAILED(hr)) { fall_back("the pixel shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuRasterPSF, sizeof kGpuRasterPSF, 0, &g_ps_filt);
    if (FAILED(hr)) { fall_back("the filtered pixel shader would not load", hr); return false; }

    const D3D11_INPUT_ELEMENT_DESC el[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36,
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    hr = g_dev->CreateInputLayout(el, 4, kGpuRasterVS, sizeof kGpuRasterVS,
                                  &g_layout);
    if (FAILED(hr)) { fall_back("the vertex layout was refused", hr); return false; }

    {
        D3D11_BUFFER_DESC cb;
        memset(&cb, 0, sizeof cb);
        cb.ByteWidth = kToonCbFloats * sizeof(float);
        cb.Usage = D3D11_USAGE_DYNAMIC;
        cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = g_dev->CreateBuffer(&cb, 0, &g_toon_cb);
        if (FAILED(hr) || !g_toon_cb) { fall_back("no toon table buffer", hr); return false; }
        g_toon_valid = false;
    }

    /* CULL_NONE: the cull test is done on the CPU from the same signed screen
       area and the same POLYGON_ATTR bits gx.cpp uses, so by the time a
       triangle reaches the card it has already been kept or dropped.
       DepthClipEnable FALSE: the software raster has no z clip at all.
       ScissorEnable TRUE: the present rectangle. */
    D3D11_RASTERIZER_DESC rd;
    memset(&rd, 0, sizeof rd);
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = FALSE;
    rd.ScissorEnable = TRUE;
    hr = g_dev->CreateRasterizerState(&rd, &g_rast);
    if (FAILED(hr)) { fall_back("no rasterizer state", hr); return false; }

    D3D11_DEPTH_STENCIL_DESC dd;
    memset(&dd, 0, sizeof dd);
    dd.DepthEnable = TRUE;
    dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dd.DepthFunc = D3D11_COMPARISON_LESS;
    dd.StencilEnable = FALSE;
    hr = g_dev->CreateDepthStencilState(&dd, &g_dss);
    if (FAILED(hr)) { fall_back("no depth state", hr); return false; }
    dd.DepthFunc = D3D11_COMPARISON_GREATER;     /* the W frame: nearer is larger */
    hr = g_dev->CreateDepthStencilState(&dd, &g_dss_w);
    if (FAILED(hr)) { fall_back("no W depth state", hr); return false; }

    /* The colour target blends by the shader's own alpha, which is the DS's
       effective alpha over 31. At TextureFilter 0 that alpha is always 1 and
       this is an exact replacement; see WHERE IT DIFFERS at the top. The ID
       target has blending off because an integer target cannot blend, which is
       what IndependentBlendEnable is here for. */
    D3D11_BLEND_DESC bd;
    memset(&bd, 0, sizeof bd);
    bd.IndependentBlendEnable = TRUE;
    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    bd.RenderTarget[1].BlendEnable = FALSE;
    bd.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = g_dev->CreateBlendState(&bd, &g_blend);
    if (FAILED(hr)) { fall_back("no blend state", hr); return false; }

    const uint32_t white = 0xFFFFFFFFu;
    D3D11_TEXTURE2D_DESC wd;
    memset(&wd, 0, sizeof wd);
    wd.Width = wd.Height = 1;
    wd.MipLevels = wd.ArraySize = 1;
    wd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    wd.SampleDesc.Count = 1;
    wd.Usage = D3D11_USAGE_IMMUTABLE;
    wd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    D3D11_SUBRESOURCE_DATA wsd;
    memset(&wsd, 0, sizeof wsd);
    wsd.pSysMem = &white;
    wsd.SysMemPitch = 4;
    hr = g_dev->CreateTexture2D(&wd, &wsd, &g_white);
    if (FAILED(hr)) { fall_back("no untextured stand-in", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_white, 0, &g_white_srv);
    if (FAILED(hr)) { fall_back("no untextured stand-in view", hr); return false; }
    return true;
}

/* ---- THE WHOLE-FRAME PIPELINE (run cardfull), made once ------------------- */

/* The whole-frame mode gives up for the rest of the run, with one line, and the
   opaque-only path takes over. Not fall_back: the card still draws. */
void full_give_up(const char *why, HRESULT hr)
{
    if (g_full_dead) return;
    g_full_dead = 1;
    if (hr)
        fprintf(stderr, "[renderer] the graphics card stops drawing the "
                        "translucent polygons and the edge smoothing (%s, code "
                        "%08x). It keeps drawing the opaque picture; the "
                        "processor does the rest.\n", why, (unsigned)hr);
    else
        fprintf(stderr, "[renderer] the graphics card does not draw the "
                        "translucent polygons and the edge smoothing (%s). It "
                        "draws the opaque picture; the processor does the "
                        "rest.\n", why);
}

static D3D11_DEPTH_STENCILOP_DESC sop(D3D11_STENCIL_OP fail,
                                      D3D11_STENCIL_OP dfail,
                                      D3D11_STENCIL_OP pass,
                                      D3D11_COMPARISON_FUNC fn)
{
    D3D11_DEPTH_STENCILOP_DESC o;
    o.StencilFailOp = fail;
    o.StencilDepthFailOp = dfail;
    o.StencilPassOp = pass;
    o.StencilFunc = fn;
    return o;
}

/* One depth-stencil state, front and back faces alike (nothing is culled on the
   card, so the two are the same rule). */
static ID3D11DepthStencilState *make_dss(bool depth, bool depth_write,
                                         UINT8 rmask, UINT8 wmask,
                                         D3D11_DEPTH_STENCILOP_DESC op,
                                         D3D11_COMPARISON_FUNC fn = D3D11_COMPARISON_LESS,
                                         bool stencil = true)
{
    D3D11_DEPTH_STENCIL_DESC dd;
    memset(&dd, 0, sizeof dd);
    dd.DepthEnable = depth ? TRUE : FALSE;
    dd.DepthWriteMask = depth_write ? D3D11_DEPTH_WRITE_MASK_ALL
                                    : D3D11_DEPTH_WRITE_MASK_ZERO;
    dd.DepthFunc = fn;
    dd.StencilEnable = stencil ? TRUE : FALSE;
    dd.StencilReadMask = rmask;
    dd.StencilWriteMask = wmask;
    dd.FrontFace = op;
    dd.BackFace = op;
    ID3D11DepthStencilState *s = 0;
    if (FAILED(g_dev->CreateDepthStencilState(&dd, &s))) return 0;
    return s;
}

bool make_full_pipeline()
{
    if (g_full_ready) return true;
    if (port_gpu_device_feature_level() < 0xb000) {   /* D3D_FEATURE_LEVEL_11_0 */
        full_give_up("the card is older than Direct3D 11", 0);
        return false;
    }
    HRESULT hr = g_dev->CreateVertexShader(kGpuAaVS, sizeof kGpuAaVS, 0, &g_full_vs);
    if (FAILED(hr)) { full_give_up("the full-screen vertex shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuTlPS, sizeof kGpuTlPS, 0, &g_ps_tl);
    if (FAILED(hr)) { full_give_up("the translucent pixel shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuTlopPS, sizeof kGpuTlopPS, 0, &g_ps_tlop);
    if (FAILED(hr)) { full_give_up("the translucent-opaque pixel shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuShadowPS, sizeof kGpuShadowPS, 0, &g_ps_shadow);
    if (FAILED(hr)) { full_give_up("the shadow pixel shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuTlFPS, sizeof kGpuTlFPS, 0, &g_ps_tl_f);
    if (FAILED(hr)) { full_give_up("the filtered translucent pixel shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuTlopFPS, sizeof kGpuTlopFPS, 0, &g_ps_tlop_f);
    if (FAILED(hr)) { full_give_up("the filtered translucent-opaque pixel shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuShadowFPS, sizeof kGpuShadowFPS, 0, &g_ps_shadow_f);
    if (FAILED(hr)) { full_give_up("the filtered shadow pixel shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuFullAaPS, sizeof kGpuFullAaPS, 0, &g_ps_faa);
    if (FAILED(hr)) { full_give_up("the smoothing pixel shader would not load", hr); return false; }

    static const D3D11_COMPARISON_FUNC kFn[4] = {
        D3D11_COMPARISON_LESS, D3D11_COMPARISON_LESS_EQUAL,
        D3D11_COMPARISON_GREATER, D3D11_COMPARISON_GREATER_EQUAL,
    };
    bool dss_ok = true;
    for (int v = 0; v < 4; ++v) {
        /* the opaque pass: depth written, no stencil (as g_dss) */
        {
            D3D11_DEPTH_STENCIL_DESC od;
            memset(&od, 0, sizeof od);
            od.DepthEnable = TRUE;
            od.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            od.DepthFunc = kFn[v];
            od.StencilEnable = FALSE;
            if (FAILED(g_dev->CreateDepthStencilState(&od, &g_dss_v[v]))) dss_ok = false;
        }
        /* the translucent polygon: low seven stencil bits are 0x40 | polygon
           ID, refuse the fragment when they already say this ID, else write
           them */
        g_dss_tl[v] = make_dss(true, false, 0x7F, 0x7F,
            sop(D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_NOT_EQUAL), kFn[v]);
        /* its opaque half: depth written, the translucent record cleared */
        g_dss_tlop[v] = make_dss(true, true, 0xFF, 0x7F,
            sop(D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                D3D11_STENCIL_OP_ZERO, D3D11_COMPARISON_ALWAYS), kFn[v]);
        /* the shadow mask: where the depth test FAILS set bit 7 (reference
           0x80) */
        g_dss_mask[v] = make_dss(true, false, 0xFF, 0x80,
            sop(D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_REPLACE,
                D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS), kFn[v]);
        /* the drawn shadow: only where bit 7 is set, and every fragment that
           gets that far clears it (reference 0x80 for the test, ZERO for the
           write) */
        g_dss_shadow[v] = make_dss(true, false, 0x80, 0x80,
            sop(D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_ZERO,
                D3D11_STENCIL_OP_ZERO, D3D11_COMPARISON_EQUAL), kFn[v]);
        if (!g_dss_v[v] || !g_dss_tl[v] || !g_dss_tlop[v] || !g_dss_mask[v] ||
            !g_dss_shadow[v])
            dss_ok = false;
    }
    /* a new mask group: clear bit 7 of everything the draw covers */
    g_dss_clrm = make_dss(false, false, 0xFF, 0x80,
        sop(D3D11_STENCIL_OP_ZERO, D3D11_STENCIL_OP_ZERO,
            D3D11_STENCIL_OP_ZERO, D3D11_COMPARISON_ALWAYS));
    /* THE GHOST LAYER: the figure's depth alone (the frame's depth rule against
       the frame's depth and the figure's own, written, stencil untouched), then
       its colour where the depth is EQUAL to what that pass left, and only on
       the first fragment to get there: the stencil, which the second pass is
       done with by now and is cleared before each figure, counts one per
       pixel */
    for (int w = 0; w < 2; ++w)
        g_dss_gz[w] = make_dss(true, true, 0xFF, 0x00,
            sop(D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
                D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS),
            kFn[w * 2], false);
    g_dss_gc = make_dss(true, false, 0xFF, 0xFF,
        sop(D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
            D3D11_STENCIL_OP_INCR_SAT, D3D11_COMPARISON_EQUAL),
        D3D11_COMPARISON_EQUAL, true);
    if (!g_dss_gz[0] || !g_dss_gz[1] || !g_dss_gc) dss_ok = false;
    if (!dss_ok || !g_dss_clrm) {
        full_give_up("a depth-stencil state was refused", 0);
        return false;
    }

    /* colour blends by the shader's alpha (the DS's effective alpha over 31)
       and the ALPHA channel keeps the largest alpha any fragment gave the
       pixel, which is the coverage mark; see the banner at the top */
    D3D11_BLEND_DESC bd;
    memset(&bd, 0, sizeof bd);
    bd.IndependentBlendEnable = TRUE;
    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    bd.RenderTarget[1].BlendEnable = FALSE;
    bd.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = g_dev->CreateBlendState(&bd, &g_blend_full);
    if (FAILED(hr)) { full_give_up("no blend state", hr); return false; }

    g_full_ready = true;
    return true;
}

/* WRAP, CLAMP AND MIRROR ARE PER AXIS, which is ntr/gx.cpp's tex_coord_i: a
   cleared repeat bit is CLAMP, repeat without flip is WRAP and repeat with
   flip is MIRROR. Built on demand and kept, so a level pays for the handful of
   combinations it actually uses. */
ID3D11SamplerState *sampler(int filter, int au, int av)
{
    if (filter < 0) filter = 0;
    if (filter > 2) filter = 2;
    if (g_smp[filter][au][av]) return g_smp[filter][au][av];
    static const D3D11_TEXTURE_ADDRESS_MODE kMode[3] = {
        D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_MIRROR,
    };
    D3D11_SAMPLER_DESC sd;
    memset(&sd, 0, sizeof sd);
    sd.AddressU = kMode[au];
    sd.AddressV = kMode[av];
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    /* 0 is the DS's own nearest sampling; 1 is bilinear with no chain, so the
       mip level is pinned at the top; 2 is the chain plus the blend between
       two of its levels, which is what trilinear is. */
    if (filter == 0) sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    else if (filter == 1) {
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.MaxLOD = 0.0f;
    } else sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    if (FAILED(g_dev->CreateSamplerState(&sd, &g_smp[filter][au][av])))
        return 0;
    return g_smp[filter][au][av];
}

/* THE POINT SAMPLER ps_filt TAKES COVERAGE FROM: filter 0's own sampler, except
   that the mip level is pinned at the top (a chain exists under filter 2, and
   a point sampler left free would pick a coarser level for a distant surface,
   where gx.cpp's nearest texel is always the full-size one). */
ID3D11SamplerState *psampler(int au, int av)
{
    if (g_psmp[au][av]) return g_psmp[au][av];
    static const D3D11_TEXTURE_ADDRESS_MODE kMode[3] = {
        D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_MIRROR,
    };
    D3D11_SAMPLER_DESC sd;
    memset(&sd, 0, sizeof sd);
    sd.AddressU = kMode[au];
    sd.AddressV = kMode[av];
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sd.MaxLOD = 0.0f;
    if (FAILED(g_dev->CreateSamplerState(&sd, &g_psmp[au][av]))) return 0;
    return g_psmp[au][av];
}

/* ---- the three targets --------------------------------------------------- */

void late_drop();
void release_targets()
{
    late_drop();                       /* MENU2: pending late readbacks cannot outlive their targets */
    g_cp_held = false;                 /* the targets the frame lived in are gone */
    if (g_ov_srv) { g_ov_srv->Release(); g_ov_srv = 0; }
    if (g_zov_srv) { g_zov_srv->Release(); g_zov_srv = 0; }
    if (g_zov_tex) { g_zov_tex->Release(); g_zov_tex = 0; }
    if (g_ov_tex) { g_ov_tex->Release(); g_ov_tex = 0; }
    if (g_bm_srv) { g_bm_srv->Release(); g_bm_srv = 0; }
    if (g_bm_tex) { g_bm_tex->Release(); g_bm_tex = 0; }
    g_tw_live = false;
    g_lay_live = false;
    if (g_lay_srv) { g_lay_srv->Release(); g_lay_srv = 0; }
    if (g_lay_tex) { g_lay_tex->Release(); g_lay_tex = 0; }
    if (g_lay2_srv) { g_lay2_srv->Release(); g_lay2_srv = 0; }
    if (g_lay2_tex) { g_lay2_tex->Release(); g_lay2_tex = 0; }
    g_lay2_tex_dirty = false;
    g_lay_w = g_lay_h = 0;
    g_lay_tex_box[0] = g_lay_tex_box[1] = g_lay_tex_box[2] = g_lay_tex_box[3] = 0;
    if (g_ov_rtv) { g_ov_rtv->Release(); g_ov_rtv = 0; }
    if (g_bm_rtv) { g_bm_rtv->Release(); g_bm_rtv = 0; }
    if (g_tw_up_srv) { g_tw_up_srv->Release(); g_tw_up_srv = 0; }
    if (g_tw_up_tex) { g_tw_up_tex->Release(); g_tw_up_tex = 0; }
    if (g_tw_st_ov) { g_tw_st_ov->Release(); g_tw_st_ov = 0; }
    if (g_tw_st_bm) { g_tw_st_bm->Release(); g_tw_st_bm = 0; }
    g_tw_w = g_tw_h = 0;
    if (g_comp_rtv) { g_comp_rtv->Release(); g_comp_rtv = 0; }
    if (g_comp_srv) { g_comp_srv->Release(); g_comp_srv = 0; }
    g_comp_valid = false;
    if (g_comp_tex) { g_comp_tex->Release(); g_comp_tex = 0; }
    if (g_comp_stage) { g_comp_stage->Release(); g_comp_stage = 0; }
    g_comp2_valid = false;
    if (g_comp2_rtv) { g_comp2_rtv->Release(); g_comp2_rtv = 0; }
    if (g_comp2_srv) { g_comp2_srv->Release(); g_comp2_srv = 0; }
    if (g_comp2_tex) { g_comp2_tex->Release(); g_comp2_tex = 0; }
    g_cp_w = g_cp_h = 0;
    if (g_fa_srv) { g_fa_srv->Release(); g_fa_srv = 0; }
    if (g_fa_rtv) { g_fa_rtv->Release(); g_fa_rtv = 0; }
    if (g_fa_st) { g_fa_st->Release(); g_fa_st = 0; }
    if (g_fa_out) { g_fa_out->Release(); g_fa_out = 0; }
    if (g_col_srv) { g_col_srv->Release(); g_col_srv = 0; }
    if (g_id_srv) { g_id_srv->Release(); g_id_srv = 0; }
    g_rt_full = false;
    if (g_col_rtv) { g_col_rtv->Release(); g_col_rtv = 0; }
    if (g_col_stage) { g_col_stage->Release(); g_col_stage = 0; }
    if (g_col) { g_col->Release(); g_col = 0; }
    if (g_id_rtv) { g_id_rtv->Release(); g_id_rtv = 0; }
    if (g_id_stage) { g_id_stage->Release(); g_id_stage = 0; }
    if (g_id) { g_id->Release(); g_id = 0; }
    if (g_dep_dsv) { g_dep_dsv->Release(); g_dep_dsv = 0; }
    if (g_dep_stage) { g_dep_stage->Release(); g_dep_stage = 0; }
    if (g_dep) { g_dep->Release(); g_dep = 0; }
    g_rt_w = g_rt_h = 0;
}

bool ensure_targets(int w, int h, bool full)
{
    if (g_col && g_rt_w == w && g_rt_h == h && g_rt_full == full) return true;
    release_targets();

    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = d.ArraySize = 1;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;

    /* colour: the framebuffer's own byte order */
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    d.BindFlags = D3D11_BIND_RENDER_TARGET;
    if (full) d.BindFlags |= D3D11_BIND_SHADER_RESOURCE;   /* the smoothing reads it */
    HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_col);
    if (FAILED(hr)) { fall_back("no colour target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_col, 0, &g_col_rtv);
    if (FAILED(hr)) { fall_back("no colour target view", hr); return false; }
    if (full) {
        hr = g_dev->CreateShaderResourceView(g_col, 0, &g_col_srv);
        if (FAILED(hr)) { fall_back("no colour target reader", hr); return false; }
    }

    /* polygon ID plus the coverage bit (whole-frame mode: the ID alone is what
       matters, the shadow shader reads it) */
    d.Format = DXGI_FORMAT_R8_UINT;
    d.BindFlags = D3D11_BIND_RENDER_TARGET;
    if (full) d.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    hr = g_dev->CreateTexture2D(&d, 0, &g_id);
    if (FAILED(hr)) { fall_back("no polygon-id target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_id, 0, &g_id_rtv);
    if (FAILED(hr)) { fall_back("no polygon-id target view", hr); return false; }
    if (full) {
        hr = g_dev->CreateShaderResourceView(g_id, 0, &g_id_srv);
        if (FAILED(hr)) { fall_back("no polygon-id target reader", hr); return false; }
    }

    /* depth: typeless so the same texture can be a depth view for drawing and
       a plain float copy for reading back. The whole-frame mode adds the
       stencil byte beside it (D32_FLOAT_S8X24: the float depth is the first
       four bytes of every eight). */
    d.Format = full ? DXGI_FORMAT_R32G8X24_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
    d.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = g_dev->CreateTexture2D(&d, 0, &g_dep);
    if (FAILED(hr)) { fall_back("no depth target", hr); return false; }
    D3D11_DEPTH_STENCIL_VIEW_DESC vd;
    memset(&vd, 0, sizeof vd);
    vd.Format = full ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT;
    vd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    hr = g_dev->CreateDepthStencilView(g_dep, &vd, &g_dep_dsv);
    if (FAILED(hr)) { fall_back("no depth target view", hr); return false; }

    /* the three staging copies the frame is read back through */
    d.BindFlags = 0;
    d.Usage = D3D11_USAGE_STAGING;
    d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    hr = g_dev->CreateTexture2D(&d, 0, &g_col_stage);
    if (FAILED(hr)) { fall_back("no colour readback buffer", hr); return false; }
    d.Format = DXGI_FORMAT_R8_UINT;
    hr = g_dev->CreateTexture2D(&d, 0, &g_id_stage);
    if (FAILED(hr)) { fall_back("no polygon-id readback buffer", hr); return false; }
    d.Format = full ? DXGI_FORMAT_R32G8X24_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
    hr = g_dev->CreateTexture2D(&d, 0, &g_dep_stage);
    if (FAILED(hr)) { fall_back("no depth readback buffer", hr); return false; }

    g_rt_w = w;
    g_rt_h = h;
    g_rt_full = full;
    return true;
}

/* ---- the texture cache --------------------------------------------------- */

void release_textures()
{
    for (std::map<uint32_t, GTex>::iterator it = g_tex.begin();
         it != g_tex.end(); ++it) {
        if (it->second.srv) it->second.srv->Release();
        if (it->second.tex) it->second.tex->Release();
    }
    g_tex.clear();
    g_tex_bytes = 0;
}

/* KEYED ON A MONOTONIC ID, NOT ON THE PIXEL POINTER. ntr/gx.cpp's decode cache
   frees and reuses its buffers, so a pointer that named one texture a moment
   ago can name a different one now; the id is handed out at bind time and
   never reused, and gx_invalidate_textures bumps a generation that throws this
   whole cache away. */
const GTex *upload_texture(uint32_t id, const uint32_t *px, int w, int h,
                           int filter)
{
    std::map<uint32_t, GTex>::iterator it = g_tex.find(id);
    if (it != g_tex.end()) return &it->second;
    if (!px || w <= 0 || h <= 0) return 0;
    HITCH_SCOPE(HK_TEX);   // HITCH9C: a texture's first use costs a conversion, an upload and a mip build

    GTex t;
    memset(&t, 0, sizeof t);
    t.w = w;
    t.h = h;

    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.ArraySize = 1;
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;   /* 0xAARRGGBB in memory is BGRA */
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;
    d.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    const bool mips = (filter >= 2);
    /* UNDER FILTER 1 AND 2 THE TEXTURE IS HELD PREMULTIPLIED, 16 BITS A
       CHANNEL (run hunt7, lane FILTEREDGE1), so the card's own bilinear and
       box-filtered mips are alpha-weighted exactly as ntr/gx.cpp's are, and
       ps_filt divides the alpha out. Sixteen bits because a covered texel
       next to transparent ones can carry a small alpha, and an 8-bit
       premultiplied value would be a coarse colour once divided back up.
       Filter 0 keeps the straight 8-bit texture it always had. */
    const bool premult = (filter >= 1);
    std::vector<uint16_t> pm;
    if (premult) {
        d.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
        pm.resize((size_t)w * (size_t)h * 4u);
        for (size_t i = 0; i < (size_t)w * (size_t)h; ++i) {
            const uint32_t c = px[i];
            const uint32_t a = c >> 24;
            const uint32_t r = (c >> 16) & 0xFFu, g = (c >> 8) & 0xFFu, b = c & 0xFFu;
            /* c * a / 255 scaled from 0..255 to 0..65535: (c * a * 257 + 127) / 255 */
            pm[i * 4 + 0] = (uint16_t)((r * a * 257u + 127u) / 255u);
            pm[i * 4 + 1] = (uint16_t)((g * a * 257u + 127u) / 255u);
            pm[i * 4 + 2] = (uint16_t)((b * a * 257u + 127u) / 255u);
            pm[i * 4 + 3] = (uint16_t)(a * 257u);
        }
    }
    const void *src_px = premult ? (const void *)pm.data() : (const void *)px;
    const UINT src_pitch = (UINT)w * (premult ? 8u : 4u);
    HRESULT hr;
    if (mips) {
        /* the card builds the chain; see WHERE IT DIFFERS at the top for how
           that chain differs from the one ntr/gx.cpp builds for itself */
        d.MipLevels = 0;
        d.BindFlags |= D3D11_BIND_RENDER_TARGET;
        d.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        hr = g_dev->CreateTexture2D(&d, 0, &t.tex);
    } else {
        d.MipLevels = 1;
        D3D11_SUBRESOURCE_DATA sd;
        memset(&sd, 0, sizeof sd);
        sd.pSysMem = src_px;
        sd.SysMemPitch = src_pitch;
        hr = g_dev->CreateTexture2D(&d, &sd, &t.tex);
    }
    if (FAILED(hr) || !t.tex) { fall_back("a texture would not upload", hr); return 0; }
    hr = g_dev->CreateShaderResourceView(t.tex, 0, &t.srv);
    if (FAILED(hr) || !t.srv) {
        t.tex->Release();
        fall_back("a texture view was refused", hr);
        return 0;
    }
    if (mips) {
        g_ctx->UpdateSubresource(t.tex, 0, 0, src_px, src_pitch, 0);
        g_ctx->GenerateMips(t.srv);
    }
    g_tex_bytes += (size_t)w * (size_t)h * (premult ? 8u : 4u) * (mips ? 2u : 1u);
    return &g_tex.insert(std::make_pair(id, t)).first->second;
}

/* ---- the vertex buffer --------------------------------------------------- */

struct GVert {
    float x, y, z, w;
    float u, v;
    float r, g, b;      /* 0..255, the units ntr/gx.cpp interpolates in */
    float pa, pid;      /* polygon alpha 0..31 and polygon ID 0..63 */
};
std::vector<GVert> g_scratch;

struct Batch {
    const GTex *tex;
    int au, av;
    int eq;                 /* the depth test is EQUAL (POLYGON_ATTR bit 14) */
    UINT first, count;
    /* a run of smoothed grids (run hunt8, SMOOTHGPU1): first and count are
       instances, drawn at grid level lvl by DrawInstanced */
    bool is_grid = false;
    int lvl = 0;
};
std::vector<Batch> g_batch;

bool ensure_vb(size_t verts)
{
    if (g_vb && g_vb_verts >= verts) return true;
    if (g_vb) { g_vb->Release(); g_vb = 0; g_vb_verts = 0; }
    size_t want = verts + verts / 2 + 1024;
    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof bd);
    bd.ByteWidth = (UINT)(want * sizeof(GVert));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    const HRESULT hr = g_dev->CreateBuffer(&bd, 0, &g_vb);
    if (FAILED(hr) || !g_vb) { fall_back("no vertex buffer", hr); return false; }
    g_vb_verts = (UINT)want;
    return true;
}

/* ---- the one-time start-up ----------------------------------------------- */

void at_exit();
void aa_at_exit();   /* with the edge-smoothing pass, below */

bool start()
{
    if (g_down) return false;
    if (g_dev) return true;
    if (g_fail_device) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_DEVICE is set; "
                "pretending no device exists\n");
        fall_back("no Direct3D device would start", 0);
        return false;
    }
    if (port_gpu_device_failed()) { fall_back("no Direct3D device would start", 0); return false; }
    port_gpu_device_address_rows("before", g_addrcheck);
    if (!port_gpu_device_acquire(g_want_warp)) {
        fall_back("no Direct3D device would start", 0);
        return false;
    }
    g_dev = (ID3D11Device *)port_gpu_device();
    g_ctx = (ID3D11DeviceContext *)port_gpu_device_context();
    if (!g_dev || !g_ctx) { fall_back("the Direct3D device went missing", 0); return false; }
    LARGE_INTEGER f;
    if (QueryPerformanceFrequency(&f)) g_qpf = f.QuadPart;
    fprintf(stderr, "[renderer] the 3D picture is drawn by \"%s\"%s, feature "
            "level %u_%u, ready in %.1f ms. %s\n",
            port_gpu_device_adapter()[0] ? port_gpu_device_adapter() : "(unnamed)",
            port_gpu_device_is_warp() ? " (WARP, the software device Windows ships)"
                                      : "",
            (unsigned)((port_gpu_device_feature_level() >> 12) & 0xf),
            (unsigned)((port_gpu_device_feature_level() >> 8) & 0xf),
            port_gpu_device_create_ms(),
            g_full_on ? "The whole picture is drawn there, the translucent "
                        "polygons, the shadows and the edge smoothing included."
                      : "The translucent polygons, the shadows and everything "
                        "after them still run here.");
    if (!make_pipeline()) return false;
    port_gpu_device_address_rows("after ", g_addrcheck);
    static int exit_said;
    if (!exit_said) { exit_said = 1; atexit(at_exit); }
    return true;
}

void at_exit()
{
    if (g_frames)
        fprintf(stderr, "[renderer] %lld frame(s) drawn on the card: %lld "
                "triangle(s) in %lld batch(es), %.3f ms building them, %.3f ms "
                "submitting, %.3f ms reading the picture back, per frame. "
                "Textures held on the card: %u KB.\n",
                g_frames, g_tris, g_batches, g_ms_build / (double)g_frames,
                g_ms_draw / (double)g_frames, g_ms_read / (double)g_frames,
                (unsigned)(g_tex_bytes / 1024));
    {
        /* THE CARD'S OWN CLOCK over the pictures it drew (hal/gpu_device.h): the
           readback above is how long the processor WAITED, this is how long the
           card was BUSY, and the second is what a faster card shrinks */
        double card = 0.0, opaque = 0.0;
        unsigned long long pics = 0, missed = 0;
        if (port_gpu_timer_totals(&card, &opaque, 0, &pics, &missed) && pics)
            fprintf(stderr, "[renderer] the card itself was busy %.3f ms a "
                    "picture over %llu picture(s) (%llu not measured), of which "
                    "%.3f ms drawing the 3D.\n", card / (double)pics, pics,
                    missed, opaque / (double)pics);
    }
    /* THE ADDRESS ROWS WITH THE THREE TARGETS STILL ALIVE, which is the point
       of taking them here rather than after the release below: a 32-bit
       process that lost one of the six DS ranges to a driver's own mapping is
       a process whose game cannot run, and the render targets are the biggest
       thing this file ever asks a driver for. */
    port_gpu_device_address_rows("exit  ", g_addrcheck);
    if (g_cp_composed || g_cp_settled || g_cp_captured)
        fprintf(stderr, "[renderer] picture presented straight from the card: "
                "%lld composited (%.3f ms each on the processor's clock), %lld "
                "settled back to the processor, %lld capture picture(s)%s.\n",
                g_cp_composed,
                g_cp_composed ? g_cp_ms / (double)g_cp_composed : 0.0,
                g_cp_settled, g_cp_captured,
                g_cp_dead ? "; it gave up during the run" : "");
    if (g_cp_checked)
        fprintf(stderr, "[renderer-cpcheck] %lld composite(s) compared with the "
                "processor's copy of the rule: %lld differed (%lld pixel(s)).\n",
                g_cp_checked, g_cp_bad_frames, g_cp_bad_px);
    if (g_lay_ticks)
        fprintf(stderr, "[renderer] menu layer: %lld tick(s) held on the card with a menu layer laid over the composite "
                "(%lld pixel(s) uploaded), %lld picture(s) between ticks took it, %lld composite(s) with a layer checked against the processor's copy.\n",
                g_lay_ticks, g_lay_up_px, g_lay_inter, g_lay_checked);
    if (g_tw_ticks)
        fprintf(stderr, "[renderer] 2D drawn on the card: %lld tick(s), %lld cell(s) "
                "(%.3f ms each tick on the processor's clock), %lld merge(s) of the "
                "processor's own pixels (%.3f ms each), %lld settled back to the "
                "processor.\n", g_tw_ticks, g_tw_rects,
                g_tw_ms / (double)g_tw_ticks, g_tw_flushes,
                g_tw_flushes ? g_tw_flush_ms / (double)g_tw_flushes : 0.0, g_tw_folds);
    if (g_tw_checks)
        fprintf(stderr, "[renderer-twodcheck] %lld stage(s) compared with the "
                "processor's own host loops: %lld differed (%lld overlay and %lld "
                "mask pixel(s)).\n", g_tw_checks, g_tw_bad, g_tw_bad_px_ov, g_tw_bad_px_bm);
    aa_at_exit();
    /* THE EXIT RULE (hal/gpu_device.h): nothing below is released at process
       exit, the system frees it. */
    if (!port_gpu_exit_release()) return;
    release_textures();
    release_targets();
    if (g_tw_ps_hide) { g_tw_ps_hide->Release(); g_tw_ps_hide = 0; }
    if (g_tw_ps_bmdef) { g_tw_ps_bmdef->Release(); g_tw_ps_bmdef = 0; }
    if (g_tw_ps_rect) { g_tw_ps_rect->Release(); g_tw_ps_rect = 0; }
    if (g_tw_ps_inset) { g_tw_ps_inset->Release(); g_tw_ps_inset = 0; }
    if (g_tw_ps_boxup) { g_tw_ps_boxup->Release(); g_tw_ps_boxup = 0; }
    if (g_tw_vs_rect) { g_tw_vs_rect->Release(); g_tw_vs_rect = 0; }
    if (g_tw_vs_box) { g_tw_vs_box->Release(); g_tw_vs_box = 0; }
    if (g_tw_cb) { g_tw_cb->Release(); g_tw_cb = 0; }
    if (g_tw_inst_srv) { g_tw_inst_srv->Release(); g_tw_inst_srv = 0; }
    if (g_tw_inst_buf) { g_tw_inst_buf->Release(); g_tw_inst_buf = 0; }
    if (g_tw_wm_srv) { g_tw_wm_srv->Release(); g_tw_wm_srv = 0; }
    if (g_tw_wm_tex) { g_tw_wm_tex->Release(); g_tw_wm_tex = 0; }
    if (g_ps_comp) { g_ps_comp->Release(); g_ps_comp = 0; }
    if (g_comp_cb) { g_comp_cb->Release(); g_comp_cb = 0; }
    if (g_vb) { g_vb->Release(); g_vb = 0; }
    if (g_white_srv) { g_white_srv->Release(); g_white_srv = 0; }
    if (g_white) { g_white->Release(); g_white = 0; }
    if (g_blend) { g_blend->Release(); g_blend = 0; }
    if (g_dss) { g_dss->Release(); g_dss = 0; }
    if (g_dss_w) { g_dss_w->Release(); g_dss_w = 0; }
    if (g_rast) { g_rast->Release(); g_rast = 0; }
    for (int a = 0; a < 3; ++a)
        for (int b = 0; b < 3; ++b)
            for (int c = 0; c < 3; ++c)
                if (g_smp[a][b][c]) { g_smp[a][b][c]->Release(); g_smp[a][b][c] = 0; }
    if (g_layout) { g_layout->Release(); g_layout = 0; }
    if (g_toon_cb) { g_toon_cb->Release(); g_toon_cb = 0; }
    g_toon_valid = false;
    for (int a = 0; a < 3; ++a)
        for (int b = 0; b < 3; ++b)
            if (g_psmp[a][b]) { g_psmp[a][b]->Release(); g_psmp[a][b] = 0; }
    if (g_ps_filt) { g_ps_filt->Release(); g_ps_filt = 0; }
    if (g_ps_faa) { g_ps_faa->Release(); g_ps_faa = 0; }
    if (g_ps_shadow) { g_ps_shadow->Release(); g_ps_shadow = 0; }
    if (g_ps_tlop) { g_ps_tlop->Release(); g_ps_tlop = 0; }
    if (g_ps_tl) { g_ps_tl->Release(); g_ps_tl = 0; }
    if (g_full_vs) { g_full_vs->Release(); g_full_vs = 0; }
    if (g_dss_clrm) { g_dss_clrm->Release(); g_dss_clrm = 0; }
    for (int v = 0; v < 4; ++v) {
        if (g_dss_shadow[v]) { g_dss_shadow[v]->Release(); g_dss_shadow[v] = 0; }
        if (g_dss_mask[v]) { g_dss_mask[v]->Release(); g_dss_mask[v] = 0; }
        if (g_dss_tlop[v]) { g_dss_tlop[v]->Release(); g_dss_tlop[v] = 0; }
        if (g_dss_tl[v]) { g_dss_tl[v]->Release(); g_dss_tl[v] = 0; }
        if (g_dss_v[v]) { g_dss_v[v]->Release(); g_dss_v[v] = 0; }
    }
    if (g_ps_shadow_f) { g_ps_shadow_f->Release(); g_ps_shadow_f = 0; }
    if (g_ps_tlop_f) { g_ps_tlop_f->Release(); g_ps_tlop_f = 0; }
    if (g_ps_tl_f) { g_ps_tl_f->Release(); g_ps_tl_f = 0; }
    if (g_blend_full) { g_blend_full->Release(); g_blend_full = 0; }
    g_full_ready = false;
    if (g_ps) { g_ps->Release(); g_ps = 0; }
    if (g_vs) { g_vs->Release(); g_vs = 0; }
    /* the device itself belongs to hal/gpu_device.cpp and is released there */
}

/* ---- the readback -------------------------------------------------------- */

/* THE TEST KNOBS OF THE READBACK, off in every ordinary run.
   SM64DS_RENDERER_RBCHECK=1: every frame the card draws is read back twice,
   this file's way and the old way (the whole targets, the whole present
   rectangle scanned one pixel at a time), from the same buffers as handed
   over; the two results are compared byte for byte -- colour, coverage,
   polygon id, depth -- and the old one is kept. The count is printed at exit.
   SM64DS_RENDERER_RBTIME=1: see draw_frame. */
int g_rbcheck, g_rbtime;
long long g_rbc_frames, g_rbc_bad_frames, g_rbc_bad_bytes;
std::vector<uint32_t> g_rbc_fb0, g_rbc_fb1;
std::vector<uint8_t>  g_rbc_cv0, g_rbc_cv1, g_rbc_id0, g_rbc_id1;
std::vector<float>    g_rbc_dp0, g_rbc_dp1;
double    g_rbt_ms[2], g_rbt_wait[2];
long long g_rbt_n[2];

/* THE COPY INTO THE CPU BUFFERS, SIXTEEN PIXELS AT A TIME where it can. The
   coverage bit is bit 7 of the id byte, which is exactly the bit SSE2's byte
   movemask collects, so one load and one movemask say whether a run of
   sixteen pixels is all covered (a course: nearly every run), all empty (the
   clear around a menu's model) or mixed. An all-covered run is written with
   whole-register stores, an empty one is skipped, and a mixed one and the
   ragged end of a row take the one-pixel loop. Every store writes the value
   the one-pixel loop writes -- the colour with its alpha forced to 0xFF, a
   coverage byte of 1, the low six bits of the id, the depth times two (one
   IEEE single multiply either way) -- so the buffers come out byte for byte
   the same; SM64DS_RENDERER_RBCHECK=1 compares them with the one-pixel loop
   on every frame. */
void copy_box(const ntr::GxGpuFrame *f, int bx0, int by0, int bx1, int by1,
              const D3D11_MAPPED_SUBRESOURCE &mc,
              const D3D11_MAPPED_SUBRESOURCE &mi,
              const D3D11_MAPPED_SUBRESOURCE &md)
{
    const __m128i alpha = _mm_set1_epi32((int)0xFF000000u);
    const __m128i ones = _mm_set1_epi8(1);
    const __m128i low6 = _mm_set1_epi8(0x3F);
    const __m128 two = _mm_set1_ps(g_depth_unscale);
    const bool want_id = f->want_attrid != 0;
    for (int y = by0; y < by1; ++y) {
        const uint32_t *crow =
            (const uint32_t *)((const unsigned char *)mc.pData + (size_t)y * mc.RowPitch);
        const unsigned char *irow =
            (const unsigned char *)mi.pData + (size_t)y * mi.RowPitch;
        const float *drow = f->want_depth
            ? (const float *)((const unsigned char *)md.pData + (size_t)y * md.RowPitch)
            : 0;
        uint32_t *fbrow = f->fb + (size_t)y * f->stride;
        uint8_t *cvrow = f->cover + (size_t)y * f->stride;
        uint8_t *idrow = f->attrid + (size_t)y * f->stride;
        float *dprow = f->depth + (size_t)y * f->stride;
        int x = bx0;
        for (; x + 16 <= bx1; x += 16) {
            const __m128i ids = _mm_loadu_si128((const __m128i *)(irow + x));
            const int m = _mm_movemask_epi8(ids);
            if (m == 0) continue;
            if (m == 0xFFFF) {
                for (int q = 0; q < 16; q += 4) {
                    const __m128i c = _mm_loadu_si128((const __m128i *)(crow + x + q));
                    _mm_storeu_si128((__m128i *)(fbrow + x + q), _mm_or_si128(c, alpha));
                    if (drow)
                        _mm_storeu_ps(dprow + x + q,
                                      _mm_mul_ps(_mm_loadu_ps(drow + x + q), two));
                }
                _mm_storeu_si128((__m128i *)(cvrow + x), ones);
                if (want_id)
                    _mm_storeu_si128((__m128i *)(idrow + x), _mm_and_si128(ids, low6));
                continue;
            }
            for (int k = x; k < x + 16; ++k) {
                const unsigned char idv = irow[k];
                if (!(idv & 0x80u)) continue;
                fbrow[k] = 0xFF000000u | (crow[k] & 0x00FFFFFFu);
                cvrow[k] = 1;
                if (want_id) idrow[k] = (uint8_t)(idv & 0x3Fu);
                if (drow) dprow[k] = drow[k] * g_depth_unscale;
            }
        }
        for (; x < bx1; ++x) {
            const unsigned char idv = irow[x];
            if (!(idv & 0x80u)) continue;
            fbrow[x] = 0xFF000000u | (crow[x] & 0x00FFFFFFu);
            cvrow[x] = 1;
            if (want_id) idrow[x] = (uint8_t)(idv & 0x3Fu);
            if (drow) dprow[x] = drow[x] * g_depth_unscale;
        }
    }
}

/* Read the box [bx0,bx1) x [by0,by1) back into the software rasteriser's
   buffers: covered pixels only, which is what keeps every pixel the card did
   not reach exactly as the caller's clear left it. `whole` is the old way,
   kept for the two knobs above: whole-target copies and a scan of whatever
   box is passed (the present rectangle).

   The colour and the id come out through a boxed copy into the same place in
   their full-size staging copies, so a pixel's staging address does not
   depend on the box. The DEPTH is always copied whole: Direct3D 11 refuses a
   boxed copy out of a depth-stencil resource (and refuses it silently: the
   staging copy just keeps what it held), and it is only read back on frames
   a second pass will read it on (want_depth) in the first place.

   Every Map happens before a single CPU byte is written, so a failure leaves
   the buffers exactly as gx.cpp cleared them. Returns 0 after fall_back. */
int read_back(const ntr::GxGpuFrame *f, int bx0, int by0, int bx1, int by1,
              bool whole, double *wait_ms)
{
    if (whole) {
        g_ctx->CopyResource(g_col_stage, g_col);
        g_ctx->CopyResource(g_id_stage, g_id);
    } else {
        D3D11_BOX box;
        box.left = (UINT)bx0;
        box.right = (UINT)bx1;
        box.top = (UINT)by0;
        box.bottom = (UINT)by1;
        box.front = 0;
        box.back = 1;
        g_ctx->CopySubresourceRegion(g_col_stage, 0, (UINT)bx0, (UINT)by0, 0,
                                     g_col, 0, &box);
        g_ctx->CopySubresourceRegion(g_id_stage, 0, (UINT)bx0, (UINT)by0, 0,
                                     g_id, 0, &box);
    }
    if (f->want_depth) g_ctx->CopyResource(g_dep_stage, g_dep);
    port_gpu_timer_span_end(PORT_GPU_SPAN_OPAQUE);

    const long long tw0 = qpc();
    D3D11_MAPPED_SUBRESOURCE mc, mi, md;
    memset(&mc, 0, sizeof mc);
    memset(&mi, 0, sizeof mi);
    memset(&md, 0, sizeof md);
    HRESULT hr = g_ctx->Map(g_col_stage, 0, D3D11_MAP_READ, 0, &mc);
    if (FAILED(hr)) { fall_back("the picture could not be read back", hr); return 0; }
    hr = g_ctx->Map(g_id_stage, 0, D3D11_MAP_READ, 0, &mi);
    if (FAILED(hr)) {
        g_ctx->Unmap(g_col_stage, 0);
        fall_back("the polygon ids could not be read back", hr);
        return 0;
    }
    if (f->want_depth) {
        hr = g_ctx->Map(g_dep_stage, 0, D3D11_MAP_READ, 0, &md);
        if (FAILED(hr)) {
            g_ctx->Unmap(g_id_stage, 0);
            g_ctx->Unmap(g_col_stage, 0);
            fall_back("the depth could not be read back", hr);
            return 0;
        }
    }
    *wait_ms += ms_between(tw0, qpc());

    /* NOTHING ON THE CPU SIDE HAS BEEN TOUCHED UNTIL HERE, which is what makes
       a failure above safe: the software pass then draws the frame over
       buffers that are still exactly as it cleared them. The old way keeps
       its one-pixel loop, which is what RBCHECK compares copy_box against. */
    if (!whole) copy_box(f, bx0, by0, bx1, by1, mc, mi, md);
    else for (int y = by0; y < by1; ++y) {
        const uint32_t *crow =
            (const uint32_t *)((const unsigned char *)mc.pData + (size_t)y * mc.RowPitch);
        const unsigned char *irow =
            (const unsigned char *)mi.pData + (size_t)y * mi.RowPitch;
        const float *drow = f->want_depth
            ? (const float *)((const unsigned char *)md.pData + (size_t)y * md.RowPitch)
            : 0;
        uint32_t *fbrow = f->fb + (size_t)y * f->stride;
        uint8_t *cvrow = f->cover + (size_t)y * f->stride;
        uint8_t *idrow = f->attrid + (size_t)y * f->stride;
        float *dprow = f->depth + (size_t)y * f->stride;
        for (int x = bx0; x < bx1; ++x) {
            const unsigned char idv = irow[x];
            if (!(idv & 0x80u)) continue;
            fbrow[x] = 0xFF000000u | (crow[x] & 0x00FFFFFFu);
            cvrow[x] = 1;
            if (f->want_attrid) idrow[x] = (uint8_t)(idv & 0x3Fu);
            if (drow) dprow[x] = drow[x] * g_depth_unscale;
        }
    }

    if (f->want_depth) g_ctx->Unmap(g_dep_stage, 0);
    g_ctx->Unmap(g_id_stage, 0);
    g_ctx->Unmap(g_col_stage, 0);
    return 1;
}

void rbt_report()
{
    for (int a = 0; a < 2; ++a) {
        const double n = g_rbt_n[a] ? (double)g_rbt_n[a] : 1.0;
        fprintf(stderr, "[renderer-rbtime] %s: %lld frame(s), readback %.3f ms "
                "(wait %.3f copy %.3f)\n", a ? "old" : "new", g_rbt_n[a],
                g_rbt_ms[a] / n, g_rbt_wait[a] / n,
                (g_rbt_ms[a] - g_rbt_wait[a]) / n);
    }
}

void rbc_report()
{
    fprintf(stderr, "[renderer-rbcheck] %lld frame(s) read back both ways, %lld "
            "differing, %lld differing byte(s)\n",
            g_rbc_frames, g_rbc_bad_frames, g_rbc_bad_bytes);
}

/* the present rectangle of the four buffers, into or out of a side copy */
void rbc_save(const ntr::GxGpuFrame *f, std::vector<uint32_t> &fb,
              std::vector<uint8_t> &cv, std::vector<uint8_t> &id,
              std::vector<float> &dp, bool out)
{
    const size_t n = (size_t)f->pw * (size_t)f->ph;
    fb.resize(n);
    cv.resize(n);
    id.resize(n);
    dp.resize(n);
    for (int y = 0; y < f->ph; ++y) {
        const size_t o = (size_t)(f->py0 + y) * f->stride + f->px0;
        const size_t r = (size_t)y * f->pw;
        if (out) {
            memcpy(&fb[r], f->fb + o, f->pw * sizeof(uint32_t));
            memcpy(&cv[r], f->cover + o, f->pw);
            memcpy(&id[r], f->attrid + o, f->pw);
            memcpy(&dp[r], f->depth + o, f->pw * sizeof(float));
        } else {
            memcpy(f->fb + o, &fb[r], f->pw * sizeof(uint32_t));
            memcpy(f->cover + o, &cv[r], f->pw);
            memcpy(f->attrid + o, &id[r], f->pw);
            memcpy(f->depth + o, &dp[r], f->pw * sizeof(float));
        }
    }
}

/* After this frame's readback: set its result aside (fb0), put the buffers
   back as they were handed over (fb1, saved before the readback), read the
   frame back the old way, compare, keep the old result. On a frame the card
   had nothing to draw the old way reads nothing either, so the comparison is
   against the buffers as handed over. */
void rbc_frame(const ntr::GxGpuFrame *f, bool drew)
{
    rbc_save(f, g_rbc_fb0, g_rbc_cv0, g_rbc_id0, g_rbc_dp0, true);
    rbc_save(f, g_rbc_fb1, g_rbc_cv1, g_rbc_id1, g_rbc_dp1, false);
    double w = 0.0;
    if (drew && !read_back(f, f->px0, f->py0, f->px0 + f->pw, f->py0 + f->ph,
                           true, &w))
        return;
    long long bytes = 0;
    for (int y = 0; y < f->ph; ++y) {
        const size_t o = (size_t)(f->py0 + y) * f->stride + f->px0;
        const size_t r = (size_t)y * f->pw;
        if (!memcmp(&g_rbc_fb0[r], f->fb + o, f->pw * 4) &&
            !memcmp(&g_rbc_cv0[r], f->cover + o, f->pw) &&
            !memcmp(&g_rbc_id0[r], f->attrid + o, f->pw) &&
            !memcmp(&g_rbc_dp0[r], f->depth + o, f->pw * 4))
            continue;
        for (int x = 0; x < f->pw; ++x) {
            const unsigned char *a = (const unsigned char *)&g_rbc_fb0[r + x];
            const unsigned char *b = (const unsigned char *)(f->fb + o + x);
            const unsigned char *c = (const unsigned char *)&g_rbc_dp0[r + x];
            const unsigned char *e = (const unsigned char *)(f->depth + o + x);
            for (int k = 0; k < 4; ++k) bytes += (a[k] != b[k]) + (c[k] != e[k]);
            bytes += g_rbc_cv0[r + x] != f->cover[o + x];
            bytes += g_rbc_id0[r + x] != f->attrid[o + x];
        }
    }
    if (bytes) {
        ++g_rbc_bad_frames;
        g_rbc_bad_bytes += bytes;
        if (g_rbc_bad_frames <= 5)
            fprintf(stderr, "[renderer-rbcheck] frame %lld: %lld byte(s) "
                    "differ\n", g_frames, bytes);
    }
    ++g_rbc_frames;
}

#if defined(GX_HAS_GPU_AA)
/* ---- THE EDGE-SMOOTHING PASS ON THE CARD (run perf2) --------------------
 *
 * ntr/gx.cpp's aa_pass smooths the finished 3D picture on the CPU after the
 * translucent pass: five luma values per covered pixel, a contrast test, a
 * direction, a blend. At RenderScale 4 that is 4 ms of a frame on a
 * twelve-core machine and 15 ms on two cores. With the card drawing, gx.cpp
 * hands the pass here instead (gx_set_gpu_aa): the picture as it stands
 * (aa_pass has already copied it aside for the display capture, that copy
 * is what is uploaded), and the coverage mask, go up; hal/gpu_raster.hlsl's
 * aa_ps runs aa_band's rule on every pixel; the result comes back and every
 * pixel that changed is written into the framebuffer and counted.
 *
 * THE RULE IS aa_band's, OPERATION FOR OPERATION, in exact arithmetic: see
 * the shader. The frame goes up and comes back as packed 32-bit words, so no
 * colour conversion stands between the CPU's bytes and the shader's.
 * SM64DS_RENDERER_AACHECK=1 runs this file's own copy of aa_band (aa_ref,
 * below) on the same input every frame and counts the pixels that differ;
 * the count is printed at exit.
 *
 * The pass needs shader model 5 (the `precise` marks that forbid a fused
 * multiply-add are only encoded there), so a device below feature level 11_0
 * answers 0 and gx.cpp runs its own pass; so does any failure, the card
 * having fallen back, and SM64DS_RENDERER_AA=0 (the knob that keeps the CPU
 * pass with the card on, for timing). The software renderer never calls
 * this: nothing registers it unless the "Renderer" setting is on. */
ID3D11VertexShader       *g_aa_vs;
ID3D11PixelShader        *g_aa_ps;
ID3D11Texture2D          *g_aa_in, *g_aa_cv, *g_aa_out, *g_aa_st;
ID3D11ShaderResourceView *g_aa_in_srv, *g_aa_cv_srv;
ID3D11RenderTargetView   *g_aa_rtv;
int g_aa_w, g_aa_h;
/* SM64DS_RENDERER_AA: 0 the processor always, 1 the card always (where it
   can), absent = WHICHEVER IS FASTER HERE, decided once at the first smoothed
   frame. Both give the same picture, so this is a speed choice only.
   Measured on level 6, same exe, card vs processor, median of 3 (RTX 4070):
     2 cores  RenderScale 3  9.56 -> 4.02 ms   RenderScale 4 19.61 -> 6.67 ms
     12 cores RenderScale 3  2.42 -> 3.35 ms   RenderScale 4  4.04 -> 5.26 ms
     WARP, 12 cores           S4 4.10 -> 11.81 ms
   The card's round trip (the picture up, the pass, the result back) costs a
   roughly fixed couple of milliseconds; the processor's pass divides by its
   cores. So the card takes it when this process has four processors or fewer
   to run on (the affinity mask, which is what a two-core machine and the
   two-core stand-in both report) and the device is a real card, not WARP. */
int g_aa_on = -1;
int g_aa_dead;         /* the pass cannot run on this device: CPU from now on */
int g_aacheck;         /* SM64DS_RENDERER_AACHECK */
long long g_aac_frames, g_aac_bad_frames, g_aac_bad_px;
std::vector<uint32_t> g_aac_ref;
long long g_aa_frames;
double g_aa_ms;

void release_aa()
{
    if (g_aa_rtv) { g_aa_rtv->Release(); g_aa_rtv = 0; }
    if (g_aa_in_srv) { g_aa_in_srv->Release(); g_aa_in_srv = 0; }
    if (g_aa_cv_srv) { g_aa_cv_srv->Release(); g_aa_cv_srv = 0; }
    if (g_aa_in) { g_aa_in->Release(); g_aa_in = 0; }
    if (g_aa_cv) { g_aa_cv->Release(); g_aa_cv = 0; }
    if (g_aa_out) { g_aa_out->Release(); g_aa_out = 0; }
    if (g_aa_st) { g_aa_st->Release(); g_aa_st = 0; }
    g_aa_w = g_aa_h = 0;
}

/* The CPU pass takes over for the rest of the run, with one line saying why.
   Not fall_back: the opaque pass on the card is unaffected by this. */
void aa_give_up(const char *why, HRESULT hr)
{
    if (g_aa_dead) return;
    g_aa_dead = 1;
    fprintf(stderr, "[renderer] the edge smoothing goes back to the processor "
            "(%s, code %08x); the picture is the same, only slower.\n", why,
            (unsigned)hr);
}

bool ensure_aa(int w, int h)
{
    if (!g_aa_vs) {
        if (port_gpu_device_feature_level() < 0xb000) {   /* D3D_FEATURE_LEVEL_11_0 */
            aa_give_up("the card is older than Direct3D 11", 0);
            return false;
        }
        HRESULT hr = g_dev->CreateVertexShader(kGpuAaVS, sizeof kGpuAaVS, 0, &g_aa_vs);
        if (FAILED(hr)) { aa_give_up("its vertex shader would not load", hr); return false; }
        hr = g_dev->CreatePixelShader(kGpuAaPS, sizeof kGpuAaPS, 0, &g_aa_ps);
        if (FAILED(hr)) { aa_give_up("its pixel shader would not load", hr); return false; }
    }
    if (g_aa_in && g_aa_w == w && g_aa_h == h) return true;
    release_aa();
    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = d.ArraySize = 1;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;
    d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    d.Format = DXGI_FORMAT_R32_UINT;
    HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_aa_in);
    if (FAILED(hr)) { aa_give_up("no picture texture", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_aa_in, 0, &g_aa_in_srv);
    if (FAILED(hr)) { aa_give_up("no picture view", hr); return false; }
    d.Format = DXGI_FORMAT_R8_UINT;
    hr = g_dev->CreateTexture2D(&d, 0, &g_aa_cv);
    if (FAILED(hr)) { aa_give_up("no coverage texture", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_aa_cv, 0, &g_aa_cv_srv);
    if (FAILED(hr)) { aa_give_up("no coverage view", hr); return false; }
    d.Format = DXGI_FORMAT_R32_UINT;
    d.BindFlags = D3D11_BIND_RENDER_TARGET;
    hr = g_dev->CreateTexture2D(&d, 0, &g_aa_out);
    if (FAILED(hr)) { aa_give_up("no result target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_aa_out, 0, &g_aa_rtv);
    if (FAILED(hr)) { aa_give_up("no result target view", hr); return false; }
    d.BindFlags = 0;
    d.Usage = D3D11_USAGE_STAGING;
    d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    hr = g_dev->CreateTexture2D(&d, 0, &g_aa_st);
    if (FAILED(hr)) { aa_give_up("no result readback buffer", hr); return false; }
    g_aa_w = w;
    g_aa_h = h;
    return true;
}

/* THIS FILE'S COPY OF ntr/gx.cpp's aa_band, for SM64DS_RENDERER_AACHECK only:
   the same luma, the same tests, the same blends, into `out`. */
inline float aa_luma(uint32_t p)
{
    return 0.299f * (float)((p >> 16) & 0xFF) +
           0.587f * (float)((p >> 8) & 0xFF) +
           0.114f * (float)(p & 0xFF);
}
inline uint32_t aa_mix2(uint32_t a, uint32_t b, float t)
{
    const float s = 1.0f - t;
    uint32_t r = 0xFF000000u;
    for (int sh = 16; sh >= 0; sh -= 8) {
        const float v = (float)((a >> sh) & 0xFF) * s + (float)((b >> sh) & 0xFF) * t;
        const int i = (int)(v + 0.5f);
        r |= (uint32_t)(i < 0 ? 0 : (i > 255 ? 255 : i)) << sh;
    }
    return r;
}
void aa_ref(const ntr::GxGpuAa *a, uint32_t *out)
{
    for (int y = 0; y < a->h; ++y) {
        const uint8_t *crow = a->cover + (size_t)y * a->stride;
        const uint32_t *rm = a->src + (size_t)y * a->stride;
        const uint32_t *rn = a->src + (size_t)(y > 0 ? y - 1 : 0) * a->stride;
        const uint32_t *rs = a->src + (size_t)(y + 1 < a->h ? y + 1 : y) * a->stride;
        uint32_t *orow = out + (size_t)y * a->w;
        for (int x = 0; x < a->w; ++x) {
            orow[x] = rm[x];
            if (!crow[x]) continue;
            const int xw = x > 0 ? x - 1 : 0;
            const int xe = x + 1 < a->w ? x + 1 : x;
            const uint32_t pM = rm[x], pN = rn[x], pS = rs[x];
            const uint32_t pW = rm[xw], pE = rm[xe];
            const float lM = aa_luma(pM), lN = aa_luma(pN), lS = aa_luma(pS);
            const float lW = aa_luma(pW), lE = aa_luma(pE);
            float lo = lM, hi = lM;
            const float ls[4] = {lN, lS, lW, lE};
            for (int i = 0; i < 4; ++i) {
                if (ls[i] < lo) lo = ls[i];
                if (ls[i] > hi) hi = ls[i];
            }
            const float range = hi - lo;
            if (range < 8.0f || range < hi * 0.125f) continue;
            const float d2x = fabsf(lW + lE - 2.0f * lM);
            const float d2y = fabsf(lN + lS - 2.0f * lM);
            const uint32_t n1 = (d2x >= d2y) ? pW : pN;
            const uint32_t n2 = (d2x >= d2y) ? pE : pS;
            const float avg = 0.25f * (lN + lS + lW + lE);
            float t = fabsf(avg - lM) / range;
            t = t * t;
            if (t > 0.5f) t = 0.5f;
            if (t <= 0.002f) continue;
            orow[x] = aa_mix2(pM, aa_mix2(n1, n2, 0.5f), t);
        }
    }
}

void aac_report()
{
    fprintf(stderr, "[renderer-aacheck] %lld frame(s) smoothed on the card and "
            "checked against the processor's rule: %lld differing, %lld "
            "differing pixel(s)\n", g_aac_frames, g_aac_bad_frames, g_aac_bad_px);
}

int aa_backend(ntr::GxGpuAa *a)
{
    if (!g_aa_on || g_aa_dead || g_down || !g_dev || !a || a->w <= 0 || a->h <= 0)
        return 0;
    if (g_aa_on < 0) {
        DWORD_PTR pm = 0, sm = 0;
        int n = 0;
        if (GetProcessAffinityMask(GetCurrentProcess(), &pm, &sm))
            for (; pm; pm &= pm - 1) ++n;
        const bool warp = port_gpu_device_is_warp() != 0;
        g_aa_on = (n > 0 && n <= 4 && !warp) ? 1 : 0;
        fprintf(stderr, "[renderer] edge smoothing: %s (%d processor(s)%s)\n",
                g_aa_on ? "on the card" : "on the processor, which is faster here",
                n, warp ? ", WARP" : "");
        if (!g_aa_on) return 0;
    }
    if (!ensure_aa(a->w, a->h)) return 0;
    const long long t0 = qpc();

    D3D11_BOX box;
    box.left = 0;
    box.top = 0;
    box.front = 0;
    box.right = (UINT)a->w;
    box.bottom = (UINT)a->h;
    box.back = 1;
    g_ctx->UpdateSubresource(g_aa_in, 0, &box, a->src, (UINT)a->stride * 4u, 0);
    g_ctx->UpdateSubresource(g_aa_cv, 0, &box, a->cover, (UINT)a->stride, 0);

    g_ctx->OMSetRenderTargets(1, &g_aa_rtv, 0);
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = (float)a->w;
    vp.Height = (float)a->h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_ctx->RSSetViewports(1, &vp);
    D3D11_RECT sc;
    sc.left = 0;
    sc.top = 0;
    sc.right = a->w;
    sc.bottom = a->h;
    g_ctx->RSSetScissorRects(1, &sc);
    g_ctx->RSSetState(g_rast);
    g_ctx->OMSetDepthStencilState(0, 0);
    g_ctx->OMSetBlendState(0, 0, 0xffffffffu);
    g_ctx->IASetInputLayout(0);
    g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_ctx->VSSetShader(g_aa_vs, 0, 0);
    g_ctx->PSSetShader(g_aa_ps, 0, 0);
    ID3D11ShaderResourceView *srvs[2] = { g_aa_in_srv, g_aa_cv_srv };
    g_ctx->PSSetShaderResources(0, 2, srvs);
    g_ctx->Draw(3, 0);
    ID3D11ShaderResourceView *none[2] = { 0, 0 };
    g_ctx->PSSetShaderResources(0, 2, none);
    g_ctx->OMSetRenderTargets(0, 0, 0);
    g_ctx->CopyResource(g_aa_st, g_aa_out);

    D3D11_MAPPED_SUBRESOURCE m;
    memset(&m, 0, sizeof m);
    const HRESULT hr = g_ctx->Map(g_aa_st, 0, D3D11_MAP_READ, 0, &m);
    if (FAILED(hr)) {
        /* nothing has been written yet: the CPU pass does this frame */
        aa_give_up("its result could not be read back", hr);
        return 0;
    }
    if (g_aacheck) {
        g_aac_ref.resize((size_t)a->w * (size_t)a->h);
        aa_ref(a, &g_aac_ref[0]);
        long long bad = 0;
        for (int y = 0; y < a->h; ++y) {
            const uint32_t *orow =
                (const uint32_t *)((const unsigned char *)m.pData + (size_t)y * m.RowPitch);
            const uint32_t *rrow = &g_aac_ref[(size_t)y * a->w];
            for (int x = 0; x < a->w; ++x) bad += orow[x] != rrow[x];
        }
        ++g_aac_frames;
        if (bad) {
            ++g_aac_bad_frames;
            g_aac_bad_px += bad;
            if (g_aac_bad_frames <= 5)
                fprintf(stderr, "[renderer-aacheck] frame %lld: %lld pixel(s) "
                        "differ\n", g_frames, bad);
        }
    }
    /* THE WRITE, exactly aa_band's: a pixel is written, and counted, only
       where the smoothed value differs from the frame as it stood. The live
       framebuffer still holds that frame (a->src is aa_pass's copy of it). */
    unsigned long long changed = 0;
    for (int y = 0; y < a->h; ++y) {
        const uint32_t *orow =
            (const uint32_t *)((const unsigned char *)m.pData + (size_t)y * m.RowPitch);
        const uint32_t *srow = a->src + (size_t)y * a->stride;
        uint32_t *frow = a->fb + (size_t)y * a->stride;
        int x = 0;
        for (; x + 4 <= a->w; x += 4) {
            const __m128i o = _mm_loadu_si128((const __m128i *)(orow + x));
            const __m128i s = _mm_loadu_si128((const __m128i *)(srow + x));
            if (_mm_movemask_epi8(_mm_cmpeq_epi32(o, s)) == 0xFFFF) continue;
            for (int k = x; k < x + 4; ++k)
                if (orow[k] != srow[k]) { frow[k] = orow[k]; ++changed; }
        }
        for (; x < a->w; ++x)
            if (orow[x] != srow[x]) { frow[x] = orow[x]; ++changed; }
    }
    g_ctx->Unmap(g_aa_st, 0);
    a->changed = changed;
    ++g_aa_frames;
    g_aa_ms += ms_between(t0, qpc());
    return 1;
}

void aa_at_exit()
{
    if (g_full_smooth)
        fprintf(stderr, "[renderer] the whole-frame smoothing ran on %lld "
                "picture(s); the picture as it was before the smoothing was kept "
                "for %lld of them (the display capture unit may read those).\n",
                g_full_smooth, g_pre_kept);
    if (g_aa_frames)
        fprintf(stderr, "[renderer] %lld frame(s) smoothed on the card, %.3f ms "
                "per frame for the whole round trip.\n", g_aa_frames,
                g_aa_ms / (double)g_aa_frames);
    if (!port_gpu_exit_release()) return;    /* the exit rule, hal/gpu_device.h */
    release_aa();
    if (g_aa_vs) { g_aa_vs->Release(); g_aa_vs = 0; }
    if (g_aa_ps) { g_aa_ps->Release(); g_aa_ps = 0; }
}
#else   /* ntr/gx.h without the edge-smoothing hook: the CPU pass always */
void aa_at_exit() {}
#endif

/* ---- the frame ----------------------------------------------------------- */

/* ==== THE WHOLE FRAME (run cardfull, lane CARDFULL1) ======================== */

/* One draw of the second pass, in submission order. The vertex offsets are
   into g_scratch1, which sits after the opaque vertices in the one buffer. */
enum { OP_A = 0, OP_AB = 1, OP_MASK = 2, OP_SHADOW = 3, OP_CLRM = 4 };
struct Op {
    int kind;
    const GTex *tex;
    int au, av;
    int pid;                /* the stencil reference of a translucent draw */
    int eq;                 /* the depth test is EQUAL (POLYGON_ATTR bit 14) */
    UINT first, count;
    bool is_grid = false;   /* as Batch */
    int lvl = 0;
};
std::vector<GVert> g_scratch1;
std::vector<Op> g_ops;
/* the ghost figures' vertices (after the second pass's in the one buffer) and
   their draws, figure by figure, farthest first */
std::vector<GVert> g_scratch2;
struct GhostDraw {
    const GTex *tex;
    int au, av;
    UINT first, count;
};
std::vector<GhostDraw> g_gdraws;
std::vector<size_t> g_gfig;      /* index in g_gdraws where each figure starts */
long long g_gh_frames, g_gh_tris;

/* the smoothing's own result target and the readback copy of it, made when the
   first frame that smooths asks (and again whenever the picture's size moves,
   because release_targets drops it with the rest) */
bool ensure_fa(int w, int h)
{
    if (g_fa_out) return true;
    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = d.ArraySize = 1;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;
    d.Format = DXGI_FORMAT_R32_UINT;
    d.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_fa_out);
    if (FAILED(hr)) { full_give_up("no smoothing result target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_fa_out, 0, &g_fa_rtv);
    if (FAILED(hr)) { full_give_up("no smoothing result target view", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_fa_out, 0, &g_fa_srv);
    if (FAILED(hr)) { full_give_up("no smoothing result reader", hr); return false; }
    d.BindFlags = 0;
    d.Usage = D3D11_USAGE_STAGING;
    d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    hr = g_dev->CreateTexture2D(&d, 0, &g_fa_st);
    if (FAILED(hr)) { full_give_up("no smoothing readback buffer", hr); return false; }
    return true;
}

inline int pop4(int m) { return (m & 1) + ((m >> 1) & 1) + ((m >> 2) & 1) + ((m >> 3) & 1); }

/* THE COPY OUT OF A FINISHED, ALPHA-MARKED PICTURE into the software
   rasteriser's buffers: a pixel whose alpha byte is not 0 is covered, and only
   covered pixels are written (colour with its alpha forced to 0xFF, coverage
   byte 1), so every pixel the card did not reach keeps exactly what the caller's
   clear put there. Four pixels at a time. `changed`, when not null, gets the
   number of pixels whose alpha is 0xFE added to it, which is how the smoothing
   pass says "I moved this one". `only_fe` writes ONLY those pixels (the
   smoothed ones, laid over a frame whose unsmoothed colours were copied
   first) and leaves the coverage alone. */
struct CopyBoxCtx {
    const ntr::GxGpuFrame *f;
    int bx0, bx1;
    const D3D11_MAPPED_SUBRESOURCE *m;
    bool only_fe;
    std::atomic<long long> cnt;
    int by0;
};
void copy_box_a_rows(const ntr::GxGpuFrame *f, int by0, int by1, int bx0, int bx1,
                     const D3D11_MAPPED_SUBRESOURCE &m, unsigned long long *changed,
                     bool only_fe);
void copy_box_a_job(void *c, int y0, int y1)
{
    CopyBoxCtx *x = (CopyBoxCtx *)c;
    unsigned long long n = 0;
    copy_box_a_rows(x->f, x->by0 + y0, x->by0 + y1, x->bx0, x->bx1, *x->m, &n, x->only_fe);
    x->cnt += (long long)n;
}
void copy_box_a(const ntr::GxGpuFrame *f, int bx0, int by0, int bx1, int by1,
                const D3D11_MAPPED_SUBRESOURCE &m, unsigned long long *changed,
                bool only_fe)
{
    /* MENU2: the copy is memory-bound; whole rows go to the raster pool's threads (SM64DS_MENU_PAR=0 keeps one) */
    CopyBoxCtx x;
    x.f = f; x.bx0 = bx0; x.bx1 = bx1; x.m = &m; x.only_fe = only_fe; x.cnt = 0; x.by0 = by0;
    static int knob = -2;
    if (knob == -2) knob = ntr::gx_menu_knob_value("SM64DS_MENU_PAR");
    ntr::gx_parallel_rows(by1 - by0, 200, copy_box_a_job, &x, knob);
    if (changed) *changed += (unsigned long long)x.cnt.load();
}
void copy_box_a_rows(const ntr::GxGpuFrame *f, int by0, int by1, int bx0, int bx1,
                     const D3D11_MAPPED_SUBRESOURCE &m, unsigned long long *changed,
                     bool only_fe)
{
    const __m128i amask = _mm_set1_epi32((int)0xFF000000u);
    const __m128i zero = _mm_setzero_si128();
    const __m128i fe = _mm_set1_epi32((int)0xFE000000u);
    unsigned long long cnt = 0;
    for (int y = by0; y < by1; ++y) {
        const uint32_t *crow =
            (const uint32_t *)((const unsigned char *)m.pData + (size_t)y * m.RowPitch);
        uint32_t *fbrow = f->fb + (size_t)y * f->stride;
        uint8_t *cvrow = f->cover + (size_t)y * f->stride;
        int x = bx0;
        for (; x + 4 <= bx1; x += 4) {
            const __m128i c = _mm_loadu_si128((const __m128i *)(crow + x));
            const __m128i a = _mm_and_si128(c, amask);
            const int zm = _mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(a, zero)));
            if (zm == 0xF) continue;
            const int fm = _mm_movemask_ps(_mm_castsi128_ps(_mm_cmpeq_epi32(a, fe)));
            cnt += (unsigned)pop4(fm);
            if (only_fe) {
                if (!fm) continue;
                for (int k = 0; k < 4; ++k)
                    if ((fm >> k) & 1)
                        fbrow[x + k] = 0xFF000000u | (crow[x + k] & 0x00FFFFFFu);
                continue;
            }
            if (zm == 0) {
                _mm_storeu_si128((__m128i *)(fbrow + x), _mm_or_si128(c, amask));
                *(uint32_t *)(cvrow + x) = 0x01010101u;
                continue;
            }
            for (int k = 0; k < 4; ++k) {
                if (!(crow[x + k] & 0xFF000000u)) continue;
                fbrow[x + k] = 0xFF000000u | (crow[x + k] & 0x00FFFFFFu);
                cvrow[x + k] = 1;
            }
        }
        for (; x < bx1; ++x) {
            const uint32_t p = crow[x];
            if (!(p & 0xFF000000u)) continue;
            if ((p >> 24) == 0xFEu) ++cnt;
            if (only_fe && (p >> 24) != 0xFEu) continue;
            fbrow[x] = 0xFF000000u | (p & 0x00FFFFFFu);
            if (!only_fe) cvrow[x] = 1;
        }
    }
    if (changed) *changed += cnt;
}

/* THE A/B PLANES, out of two mapped staging copies: the depth where the card
   wrote one (a cleared pixel reads back as 2.0 after the unhalving) and the
   polygon ID where the opaque pass left one. The depth texel is eight bytes,
   the float first. */
void copy_ab_planes(const ntr::GxGpuFrame *f, int bx0, int by0, int bx1, int by1,
                    const D3D11_MAPPED_SUBRESOURCE &mid,
                    const D3D11_MAPPED_SUBRESOURCE &mdep)
{
    for (int y = by0; y < by1; ++y) {
        const unsigned char *irow =
            (const unsigned char *)mid.pData + (size_t)y * mid.RowPitch;
        const float *drow = (const float *)
            ((const unsigned char *)mdep.pData + (size_t)y * mdep.RowPitch);
        float *dprow = f->depth + (size_t)y * f->stride;
        uint8_t *idrow = f->attrid + (size_t)y * f->stride;
        for (int x = bx0; x < bx1; ++x) {
            const float dv = drow[(size_t)x * 2];
            if (g_frame_w) {
                if (dv > 0.0f) dprow[x] = dv * kWDepthUnscale;   /* nothing drew: 0 */
            } else {
                const float z = dv * kDepthUnscale;
                if (z < 1.999f) dprow[x] = z;
            }
            if (irow[x] & 0x80u) idrow[x] = (uint8_t)(irow[x] & 0x3Fu);
        }
    }
}

/* THE FRAME'S CONSTANTS: the toon table (only on a frame that has one) and the
   fog (run hunt8, RENDER093), one constant buffer, one shadow copy, mapped
   only when something changed. The toon part keeps its last values on a frame
   without a toon polygon, as the old upload did. */
bool upload_frame_consts(const ntr::GxGpuFrame *f)
{
    static float shadow[kToonCbFloats];
    if (f->toon_shade && f->toon_rgb) {
        for (int i = 0; i < 32; ++i) {
            shadow[i * 4 + 0] = f->toon_rgb[i * 3 + 0];
            shadow[i * 4 + 1] = f->toon_rgb[i * 3 + 1];
            shadow[i * 4 + 2] = f->toon_rgb[i * 3 + 2];
            /* w: the Smooth Models toon blend (gx.cpp's g_toon_blend; lane
               SMOOTHTOON1): entry k's w is 1 when k and k + 1 blend, entry
               31's w is the switch. 0 everywhere = the DS's nearest entry. */
            shadow[i * 4 + 3] = f->toon_blend ? f->toon_blend[i] : 0.0f;
        }
    }
    float *p = shadow + 32 * 4;
    p[0] = f->fog_on ? 1.0f : 0.0f;
    p[1] = (float)f->fog_shift;
    p[2] = (float)f->fog_offset;
    p[3] = f->fog_wscale;
    p[4] = f->fog_rgb[0]; p[5] = f->fog_rgb[1]; p[6] = f->fog_rgb[2]; p[7] = 0.0f;
    for (int i = 0; i < 36; ++i) p[8 + i] = i < 34 ? (float)f->fog_tab[i] : 0.0f;
    if (g_toon_valid && memcmp(g_toon_seen, shadow, sizeof shadow) == 0) return true;
    D3D11_MAPPED_SUBRESOURCE tm;
    memset(&tm, 0, sizeof tm);
    HRESULT thr = g_ctx->Map(g_toon_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &tm);
    if (FAILED(thr)) { fall_back("the toon table would not open", thr); return false; }
    memcpy(tm.pData, shadow, sizeof shadow);
    g_ctx->Unmap(g_toon_cb, 0);
    memcpy(g_toon_seen, shadow, sizeof shadow);
    g_toon_valid = true;
    return true;
}

/* one triangle's three vertices, the same arithmetic the opaque loop uses */
/* THE EQUAL DEPTH TEST (POLYGON_ATTR bit 14, run hunt8 lane BOWSERWAVE1; the
   CPU raster's w_depth_fail is the reference). A polygon that asks for it
   passes where its depth equals what is in the buffer to within the DS's
   tolerance: 255 steps of the geometry engine's 16-significant-bit W in a W
   frame (the same step w_tie_q finds from the triangle's largest W), 512 steps
   of the 24-bit Z otherwise. The card cannot test "within", so the polygon is
   moved TOWARD the viewer by the tolerance and tested LESS_EQUAL / GREATER_EQUAL
   (the g_dss_* sets' odd entries), which passes the equal pixels and the ones
   the tolerance allows. It also passes a pixel the polygon is nearer than by
   more than the tolerance, which the DS refuses; the one place the card and the
   DS differ for these polygons. The ring coplanar with the arena floor is the
   case that matters. */
float eq_w_step(const ntr::GxTriangle &t)
{
    float m = fabsf(t.v[0].w);
    if (fabsf(t.v[1].w) > m) m = fabsf(t.v[1].w);
    if (fabsf(t.v[2].w) > m) m = fabsf(t.v[2].w);
    const float raw = m * 4096.0f;
    const unsigned r = raw >= 4294967040.0f ? 0xFFFFFFFFu : (unsigned)raw;
    unsigned wsize = 0;
    while (wsize < 32 && (r >> wsize)) wsize += 4;
    const unsigned s = wsize > 16 ? wsize - 16 : 0;
    return (float)(1u << s) * (1.0f / 4096.0f);
}

void push_tri(const ntr::GxTriangle &t, const ntr::GxGpuFrame *f, bool textured,
              float inv_cw, float inv_ch, std::vector<GVert> &dst, bool eq,
              float pa_cap = 31.0f)
{
    const float qw = (eq && f->wdepth) ? eq_w_step(t) : 0.0f;
    const float tsc = (float)(t.tex_scale ? t.tex_scale : 1);
    const float iw = textured ? 1.0f / (float)t.tw : 0.0f;
    const float ih = textured ? 1.0f / (float)t.th : 0.0f;
    float pa = (float)((t.alpha >= 31 || t.alpha == 0) ? 31u : t.alpha);
    if (pa > pa_cap) pa = pa_cap;
    const float pid = (float)(t.polyid + (t.fog ? 256 : 0) +
        ((t.mode == 2 && f->toon_shade) ? 64 * f->toon_shade : 0));
    for (int k = 0; k < 3; ++k) {
        const ntr::GxVertex &v = t.v[k];
        GVert o;
        const float xn = (float)v.x * inv_cw - 1.0f;
        const float yn = 1.0f - (float)v.y * inv_ch;
        o.x = xn * v.w;
        o.y = yn * v.w;
        if (f->wdepth)
            /* W frame: c / w, nearer larger (kWDepthC); an equal-test polygon
               is moved nearer by 255 steps of w: c / (w - 255 q) is
               c / w * (1 + 255 q / w) to first order */
            o.z = eq ? kWDepthC * (1.0f + 255.0f * qw / v.w) : kWDepthC;
        else
            o.z = (v.z * kDepthScale - (eq ? 512.0f / 16777216.0f : 0.0f)) * v.w;
        o.w = v.w;
        o.u = textured ? v.u * tsc * iw : 0.0f;
        o.v = textured ? v.v * tsc * ih : 0.0f;
        o.r = (float)((v.color >> 16) & 0xFF);
        o.g = (float)((v.color >> 8) & 0xFF);
        o.b = (float)(v.color & 0xFF);
        o.pa = pa;
        o.pid = pid;
        dst.push_back(o);
    }
}

/* ==== THE PICTURE HELD ON THE CARD (run cardfull, lane CARD2) ===============
 *
 * With Renderer 1 and PresentBackend 1 and SM64DS_PRESENT_FROM_CARD on, a frame
 * the card drew whole is not read back at all. Its colour (and, when the card
 * smoothed it, the smoothed copy) stays in the card's own targets, the
 * processor writes only the 2D into the framebuffer (ntr/gx.h, GX_OV_*), and
 * the present composites the two on the card (comp_ps in the shader file) and
 * scales the result onto the window. Four things on the processor still want
 * real pixels and each has a way to ask:
 *   - a SETTLE (GxGpuFrame::settle 1) reads the card's picture back once and
 *     turns the framebuffer into the ordinary finished frame, in place. The
 *     host overlays, the stacked layout, a picture dump, the brightness-mask
 *     fade and a blend that reads the 3D pixel all take it;
 *   - the DISPLAY CAPTURE (settle 2) gets the frame it reads built on request
 *     out of the card's copy BEFORE the smoothing, under the 2D, so the game
 *     reads the same bytes it always did and no prediction is involved;
 *   - nothing else reads the 3D after gx_render: the compositor's coverage
 *     questions are asked of the card inside the composite (a 2D layer behind
 *     the 3D layer shows only where the 3D engine drew nothing).
 * ANY failure is one plain line and the latch g_cp_dead: the frame in hand is
 * settled and every frame after it is read back and presented as before. The
 * 3D can only be lost by a card that stops answering in the middle of a settle,
 * and then that one frame shows the clear colour where the 3D was, never a
 * mixture. */
void cp_give_up(const char *why, HRESULT hr)
{
    if (g_cp_dead) return;
    g_cp_dead = 1;
    g_comp_valid = false;
    g_comp2_valid = false;
    /* g_cp_held stays as it is: the frame in hand is still on the card and its
       settle still has to be able to read it */
    if (hr)
        fprintf(stderr, "[renderer] the picture is no longer presented straight "
                "from the graphics card (%s, code %08x). It is read back and "
                "presented the ordinary way from now on; nothing else "
                "changes.\n", why, (unsigned)hr);
    else
        fprintf(stderr, "[renderer] the picture is no longer presented straight "
                "from the graphics card (%s). It is read back and presented the "
                "ordinary way from now on; nothing else changes.\n", why);
}

/* May a frame offered with GxGpuFrame::keep be held on the card? Only on the
   whole-frame path with the smoothing on the card and with the Direct3D present
   on, which is where the picture is composited and shown (a swap chain, or the
   offscreen proof target of the same quad). */
bool cp_keep_ok()
{
    if (!g_cp_on || g_cp_dead || g_down) return false;
    if (!g_full_on || g_full_dead || g_aa_on_full == 0 || g_aacheck) return false;
    static int backend = -1;
    if (backend < 0) backend = host_setting_present_backend() ? 1 : 0;
    return backend != 0;
}

/* Returns 2 when the card drew the whole frame, 0 after fall_back (the CPU
   buffers are untouched and gx.cpp draws everything), and -1 when the
   whole-frame mode cannot run at all (it has given up, nothing was touched,
   and the caller carries on with the opaque-only path). */
/* ==== SMOOTHED GRIDS EXPANDED ON THE CARD (run hunt8, lane SMOOTHGPU1) =======
 *
 * ntr/gx.cpp keeps a smoothed source triangle as one GxGrid record instead of
 * up to 64 sub-triangles (ntr/gx.h has the contract) when this backend says it
 * takes them. Here a grid becomes one INSTANCE of a draw whose vertex shader
 * (vs_grid in gpu_raster.hlsl) rebuilds every sub-triangle corner from:
 *   - the grid's points, in the shape store's pool (mirrored here once and
 *     appended to as the store grows) or in the frame's arena (the grids the
 *     store did not keep),
 *   - the three source corners' texel coordinates and colours,
 *   - the matrices and the polygon state the grid was recorded under,
 *   - the weight and index tables of its level, uploaded once.
 * Consecutive grids with the same texture, wrap, polygon class and level are
 * ONE draw, so a model costs a handful of draws and not one per triangle.
 *
 * WHAT THE CARD DECIDES THAT THE PROCESSOR PATH DECIDED: the back-face cull and
 * the degenerate test (per sub-triangle, from the same screen area, in the
 * vertex stage). Everything else the processor decided before the grid was
 * kept: gx.cpp only keeps a grid whose model-space box is wholly in front of
 * the near plane, so no clip is ever needed, and it keeps no shadow-volume or
 * equal-depth polygon.
 *
 * SM64DS_SMOOTH_GPU=0 asks for the processor path (read by gx.cpp, which then
 * never keeps a grid; read here as well so a backend started without it says
 * no to the probe). */

struct GInst {
    float xf[16];
    float pj[16];
    float vp[4];
    uint32_t info[4];
    float a0[4], a1[4], a2[4];
    float st[4];
    float tx[4];
};
static_assert(sizeof(GInst) == 240, "GInst must match gpu_raster.hlsl");

int g_grid_env = -1;                       /* SM64DS_SMOOTH_GPU */
int g_grid_dead;                           /* a grid resource failed: processor path from now on */
long long g_grid_frames, g_grid_insts, g_grid_draws;
ID3D11VertexShader        *g_vs_grid;
ID3D11Buffer              *g_gi_buf;       /* instances, dynamic */
ID3D11ShaderResourceView  *g_gi_srv;
UINT                       g_gi_cap;
ID3D11Buffer              *g_gp_buf;       /* the mirror of the shape store's pool, default usage */
ID3D11ShaderResourceView  *g_gp_srv;
size_t                     g_gp_cap, g_gp_have;
uint32_t                   g_gp_gen;
ID3D11Buffer              *g_ga_buf;       /* the frame arena, dynamic */
ID3D11ShaderResourceView  *g_ga_srv;
size_t                     g_ga_cap;
ID3D11Buffer              *g_gt_abc, *g_gt_idx;
ID3D11ShaderResourceView  *g_gt_abc_srv, *g_gt_idx_srv;
UINT                       g_gt_abc_off[3], g_gt_idx_off[3], g_gt_tris[3];
ID3D11Buffer              *g_gcb_frame, *g_gcb_draw;
bool                       g_grid_ready;
std::vector<GInst>         g_ginst;

bool grid_supported()
{
    if (g_grid_env < 0) {
        const char *e = getenv("SM64DS_SMOOTH_GPU");
        g_grid_env = (e && *e == '0') ? 0 : 1;
    }
    return g_grid_env && !g_grid_dead;
}

void grid_give_up(const char *why, HRESULT hr)
{
    if (g_grid_dead) return;
    g_grid_dead = 1;
    if (hr) fprintf(stderr, "[renderer] smoothed grids on the card: %s (0x%08lx). "
                    "They are expanded on the processor from now on.\n", why, (unsigned long)hr);
    else fprintf(stderr, "[renderer] smoothed grids on the card: %s. They are "
                 "expanded on the processor from now on.\n", why);
}

ID3D11ShaderResourceView *grid_make_srv(ID3D11Buffer *b, UINT elems)
{
    D3D11_SHADER_RESOURCE_VIEW_DESC sd;
    memset(&sd, 0, sizeof sd);
    sd.Format = DXGI_FORMAT_UNKNOWN;
    sd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    sd.Buffer.FirstElement = 0;
    sd.Buffer.NumElements = elems;
    ID3D11ShaderResourceView *v = 0;
    if (FAILED(g_dev->CreateShaderResourceView(b, &sd, &v))) return 0;
    return v;
}

/* a structured buffer of `elems` elements of `stride` bytes */
bool grid_make_buf(ID3D11Buffer **buf, ID3D11ShaderResourceView **srv, UINT stride,
                   UINT elems, bool dynamic, const void *init)
{
    if (*srv) { (*srv)->Release(); *srv = 0; }
    if (*buf) { (*buf)->Release(); *buf = 0; }
    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof bd);
    bd.ByteWidth = stride * elems;
    bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = stride;
    D3D11_SUBRESOURCE_DATA sdat;
    memset(&sdat, 0, sizeof sdat);
    sdat.pSysMem = init;
    if (FAILED(g_dev->CreateBuffer(&bd, init ? &sdat : 0, buf)) || !*buf) return false;
    *srv = grid_make_srv(*buf, elems);
    return *srv != 0;
}

bool grid_start()
{
    if (g_grid_ready) return true;
    if (!grid_supported()) return false;
    HRESULT hr = g_dev->CreateVertexShader(kGpuGridVS, sizeof kGpuGridVS, 0, &g_vs_grid);
    if (FAILED(hr)) { grid_give_up("the grid vertex shader would not load", hr); return false; }
    /* the weights and the index lists of the three levels, back to back */
    std::vector<float> abc;
    std::vector<uint32_t> idx;
    static const int kTf[3] = {2, 4, 8};
    for (int l = 0; l < 3; ++l) {
        const ntr::SmoothGridTable *T = ntr::smooth_grid_table(kTf[l]);
        if (!T) { grid_give_up("a grid level has no table", 0); return false; }
        g_gt_abc_off[l] = (UINT)(abc.size() / 4);
        for (int i = 0; i < T->n; ++i) {
            abc.push_back(T->a[i]); abc.push_back(T->b[i]); abc.push_back(T->c[i]);
            abc.push_back(0.0f);
        }
        const int nt = ntr::smooth_grid_tris(kTf[l]);
        const uint16_t *ix = ntr::smooth_grid_tri_index(kTf[l]);
        g_gt_tris[l] = (UINT)nt;
        g_gt_idx_off[l] = (UINT)idx.size();
        for (int i = 0; i < nt * 3; ++i) idx.push_back(ix[i]);
    }
    if (!grid_make_buf(&g_gt_abc, &g_gt_abc_srv, 16, (UINT)(abc.size() / 4), false, &abc[0]) ||
        !grid_make_buf(&g_gt_idx, &g_gt_idx_srv, 4, (UINT)idx.size(), false, &idx[0])) {
        grid_give_up("the grid tables would not upload", 0);
        return false;
    }
    D3D11_BUFFER_DESC cb;
    memset(&cb, 0, sizeof cb);
    cb.ByteWidth = 32;
    cb.Usage = D3D11_USAGE_DYNAMIC;
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(g_dev->CreateBuffer(&cb, 0, &g_gcb_frame)) ||
        FAILED(g_dev->CreateBuffer(&cb, 0, &g_gcb_draw))) {
        grid_give_up("no grid constant buffers", 0);
        return false;
    }
    g_gp_cap = 0; g_gp_have = 0; g_gp_gen = 0;
    g_grid_ready = true;
    return true;
}

/* the pool mirror, the arena and the instances, for one frame */
bool grid_upload(const ntr::GxGpuFrame *f)
{
    /* ---- the shape store's pool: append what the card has not seen -------- */
    size_t n = 0;
    const float *pool = ntr::smooth_store_pool(&n);
    const uint32_t gen = ntr::smooth_store_generation();
    if (gen != g_gp_gen) { g_gp_gen = gen; g_gp_have = 0; }
    if (n > g_gp_have || !g_gp_buf) {
        if (n > g_gp_cap || !g_gp_buf) {
            size_t cap = g_gp_cap ? g_gp_cap : (size_t)1 << 20;
            while (cap < n) cap *= 2;
            ID3D11Buffer *old = g_gp_buf;
            ID3D11ShaderResourceView *oldv = g_gp_srv;
            g_gp_buf = 0; g_gp_srv = 0;
            if (!grid_make_buf(&g_gp_buf, &g_gp_srv, 4, (UINT)cap, false, 0)) {
                grid_give_up("the pool mirror would not allocate", 0);
                if (old) old->Release();
                if (oldv) oldv->Release();
                return false;
            }
            if (old && g_gp_have) {
                D3D11_BOX b;
                b.left = 0; b.right = (UINT)(g_gp_have * 4); b.top = 0; b.bottom = 1;
                b.front = 0; b.back = 1;
                g_ctx->CopySubresourceRegion(g_gp_buf, 0, 0, 0, 0, old, 0, &b);
            }
            if (old) old->Release();
            if (oldv) oldv->Release();
            g_gp_cap = cap;
        }
        if (n > g_gp_have) {
            D3D11_BOX b;
            b.left = (UINT)(g_gp_have * 4); b.right = (UINT)(n * 4); b.top = 0;
            b.bottom = 1; b.front = 0; b.back = 1;
            g_ctx->UpdateSubresource(g_gp_buf, 0, &b, pool + g_gp_have, 0, 0);
            g_gp_have = n;
        }
    }
    /* ---- the frame arena ---------------------------------------------------- */
    {
        const size_t need = f->arena_floats ? f->arena_floats : 3;
        if (need > g_ga_cap || !g_ga_buf) {
            size_t cap = g_ga_cap ? g_ga_cap : (size_t)1 << 16;
            while (cap < need) cap *= 2;
            if (!grid_make_buf(&g_ga_buf, &g_ga_srv, 4, (UINT)cap, true, 0)) {
                grid_give_up("the grid arena would not allocate", 0);
                return false;
            }
            g_ga_cap = cap;
        }
        if (f->arena_floats) {
            D3D11_MAPPED_SUBRESOURCE ms;
            if (FAILED(g_ctx->Map(g_ga_buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
                grid_give_up("the grid arena would not open", 0);
                return false;
            }
            memcpy(ms.pData, f->arena, f->arena_floats * sizeof(float));
            g_ctx->Unmap(g_ga_buf, 0);
        }
    }
    /* ---- the instances ------------------------------------------------------- */
    {
        const UINT need = (UINT)g_ginst.size();
        if (need > g_gi_cap || !g_gi_buf) {
            UINT cap = g_gi_cap ? g_gi_cap : 1024;
            while (cap < need) cap *= 2;
            if (!grid_make_buf(&g_gi_buf, &g_gi_srv, (UINT)sizeof(GInst), cap, true, 0)) {
                grid_give_up("the grid instances would not allocate", 0);
                return false;
            }
            g_gi_cap = cap;
        }
        D3D11_MAPPED_SUBRESOURCE ms;
        if (FAILED(g_ctx->Map(g_gi_buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            grid_give_up("the grid instances would not open", 0);
            return false;
        }
        memcpy(ms.pData, &g_ginst[0], g_ginst.size() * sizeof(GInst));
        g_ctx->Unmap(g_gi_buf, 0);
    }
    /* ---- the frame constants ------------------------------------------------ */
    {
        D3D11_MAPPED_SUBRESOURCE ms;
        if (FAILED(g_ctx->Map(g_gcb_frame, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            grid_give_up("the grid constants would not open", 0);
            return false;
        }
        float *p = (float *)ms.pData;
        uint32_t *u = (uint32_t *)ms.pData;
        p[0] = 2.0f / (float)f->cw;
        p[1] = 2.0f / (float)f->ch;
        p[2] = kWDepthC;
        p[3] = kDepthScale;
        u[4] = f->wdepth ? 1u : 0u;
        u[5] = u[6] = u[7] = 0;
        g_ctx->Unmap(g_gcb_frame, 0);
    }
    return true;
}

/* the vertex stage's state for a grid draw */
void grid_bind()
{
    ID3D11ShaderResourceView *v[5] = {g_gi_srv, g_gp_srv, g_ga_srv, g_gt_abc_srv, g_gt_idx_srv};
    ID3D11Buffer *cb[2] = {g_gcb_frame, g_gcb_draw};
    g_ctx->IASetInputLayout(0);
    g_ctx->VSSetShader(g_vs_grid, 0, 0);
    g_ctx->VSSetConstantBuffers(0, 2, cb);
    g_ctx->VSSetShaderResources(0, 5, v);
}

void grid_unbind()
{
    ID3D11ShaderResourceView *none[5] = {0, 0, 0, 0, 0};
    g_ctx->VSSetShaderResources(0, 5, none);
}

/* one instanced draw: `count` consecutive instances from `first`, level `lvl` */
bool grid_draw(UINT first, UINT count, int lvl)
{
    D3D11_MAPPED_SUBRESOURCE ms;
    if (FAILED(g_ctx->Map(g_gcb_draw, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) return false;
    uint32_t *u = (uint32_t *)ms.pData;
    u[0] = first;
    u[1] = g_gt_abc_off[lvl];
    u[2] = g_gt_idx_off[lvl];
    u[3] = 0;
    g_ctx->Unmap(g_gcb_draw, 0);
    g_ctx->DrawInstanced(g_gt_tris[lvl] * 3u, count, 0, 0);
    ++g_grid_draws;
    return true;
}

/* ---- LATE AND DEFERRED READBACK OF THE SMOOTHED PICTURE (run after093, lane MENU2) -------------------
   A frame the caller marked late (GxGpuFrame::late) has its fb / coverage clears still to do. The usual smoothed whole frame
   (no pre-smoothing copy wanted, no A/B) copies the box into a staging texture of its own (g_fa_q, two of them), flushes, and
   then FINISHES: pre_read (the caller's clears, now hidden under the card's work), the wait, the copy into fb and the
   coverage plane. With GxGpuFrame::defer the finish is left for gx_finish_readback and the caller does other work first; at
   most two are pending, finished in the order they were drawn. Anything else the frame needs takes the old path, with
   pre_read called before the first CPU buffer is written. SM64DS_MENU_LATE=0 turns all of it off. */
ID3D11Texture2D *g_fa_q[2];
int g_fa_q_w, g_fa_q_h;
struct LatePend {
    ntr::GxGpuFrame f;
    int bx0, by0, bx1, by1;
    int q;
    long long t0, t1, t2;
    size_t nverts, nbatches;
};
LatePend g_lp[2];
int g_lp_head, g_lp_n;
long long g_late_issued, g_late_finished;
int g_late_drill_kind = -1, g_late_drill_n;     /* SM64DS_MENU_LATE_DRILL=fail:N or drop:N (proof rows for the edge paths; inert unset) */
void late_drill_init()
{
    if (g_late_drill_kind != -1) return;
    g_late_drill_kind = 0;
#ifndef PORT_ROM_CLEAN
    const char *e = getenv("SM64DS_MENU_LATE_DRILL");      /* proof drill: not in the shipping build */
#else
    const char *e = 0;
#endif
    if (e && !strncmp(e, "fail:", 5)) { g_late_drill_kind = 1; g_late_drill_n = atoi(e + 5); }
    else if (e && !strncmp(e, "drop:", 5)) { g_late_drill_kind = 2; g_late_drill_n = atoi(e + 5); }
}

bool late_gpu_on()
{
    static int v = -2;
    if (v == -2) v = ntr::gx_menu_knob_value("SM64DS_MENU_LATE");
    return ntr::gx_menu_knob_on(v) != 0;
}

bool ensure_fa_q(int w, int h)
{
    if (g_fa_q[0] && g_fa_q[1] && g_fa_q_w == w && g_fa_q_h == h) return true;
    for (int i = 0; i < 2; ++i) if (g_fa_q[i]) { g_fa_q[i]->Release(); g_fa_q[i] = 0; }
    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = d.ArraySize = 1;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_STAGING;
    d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    d.Format = DXGI_FORMAT_R32_UINT;
    for (int i = 0; i < 2; ++i) {
        if (FAILED(g_dev->CreateTexture2D(&d, 0, &g_fa_q[i]))) {
            for (int j = 0; j < 2; ++j) if (g_fa_q[j]) { g_fa_q[j]->Release(); g_fa_q[j] = 0; }
            return false;
        }
    }
    g_fa_q_w = w;
    g_fa_q_h = h;
    return true;
}

/* the oldest pending readback: the caller's clears, the wait, the copy. The entry is taken off the queue first, so a flush
   hook that runs inside pre_read and finishes "the oldest" finishes the next one, not this. */
void late_finish_one()
{
    if (g_lp_n <= 0) return;
    LatePend P = g_lp[g_lp_head];
    g_lp_head = (g_lp_head + 1) & 1;
    --g_lp_n;
    const ntr::GxGpuFrame *f = &P.f;
    const long long tf0 = qpc();
    if (f->pre_read) f->pre_read(f->pre_arg);
    D3D11_MAPPED_SUBRESOURCE mfa;
    memset(&mfa, 0, sizeof mfa);
    const long long tw0 = qpc();
    late_drill_init();
    ++g_late_finished;
    HRESULT hr = (g_ctx && g_fa_q[P.q] && !(g_late_drill_kind == 1 && g_late_finished == g_late_drill_n))
                     ? g_ctx->Map(g_fa_q[P.q], 0, D3D11_MAP_READ, 0, &mfa) : E_FAIL;
    const double wait_ms = ms_between(tw0, qpc());
    if (FAILED(hr)) {
        fall_back("the picture could not be read back", hr);
        return;
    }
    unsigned long long changed = 0;
    copy_box_a(f, P.bx0, P.by0, P.bx1, P.by1, mfa, &changed, false);
    g_ctx->Unmap(g_fa_q[P.q], 0);
    const long long t3 = qpc();
    ++g_frames;
    g_tris += (long long)(P.nverts / 3);
    g_verts += (long long)P.nverts;
    g_ms_build += ms_between(P.t0, P.t1);
    g_ms_draw += ms_between(P.t1, P.t2);
    g_ms_read += ms_between(tf0, t3);
    port::hitch_add(port::HK_GPUBUILD, ms_between(P.t0, P.t1));
    port::hitch_add(port::HK_GPUDRAW, ms_between(P.t1, P.t2));
    port::hitch_add(port::HK_GPUWAIT, wait_ms);
    port::hitch_add(port::HK_GPUCOPY, ms_between(tf0, t3) - wait_ms);
    ++g_aa_frames;
    g_aa_ms += ms_between(P.t1, P.t2) + ms_between(tf0, t3);
    if (g_perf) {
        g_perf_build += ms_between(P.t0, P.t1);
        g_perf_draw += ms_between(P.t1, P.t2);
        g_perf_read += ms_between(tf0, t3);
        g_perf_wait += wait_ms;
        g_perf_copy += ms_between(tf0, t3) - wait_ms;
        g_perf_tris += (long long)(P.nverts / 3);
        g_perf_batches += (long long)P.nbatches;
        if (++g_perf_n >= 30) {
            fprintf(stderr, "[renderer] build %6.3fms submit %6.3fms readback "
                    "%6.3fms (wait %6.3fms copy %6.3fms) tris %6lld batches "
                    "%4lld (whole frame, late)\n",
                    g_perf_build / g_perf_n, g_perf_draw / g_perf_n,
                    g_perf_read / g_perf_n, g_perf_wait / g_perf_n,
                    g_perf_copy / g_perf_n, g_perf_tris / g_perf_n,
                    g_perf_batches / g_perf_n);
            g_perf_build = g_perf_draw = g_perf_read = 0.0;
            g_perf_wait = g_perf_copy = 0.0;
            g_perf_tris = g_perf_batches = 0;
            g_perf_n = 0;
        }
    }
    if (f->out) f->out->aa_changed = changed;
    else ntr::gx_aa_note_changed(changed);
}

/* the targets are going away (a size change, a lost device): the pending frames cannot be finished; their callers' clears run
   so the buffers hold the clear colour for the one picture that shows it */
void late_drop()
{
    while (g_lp_n > 0) {
        LatePend P = g_lp[g_lp_head];
        g_lp_head = (g_lp_head + 1) & 1;
        --g_lp_n;
        if (P.f.pre_read) P.f.pre_read(P.f.pre_arg);
    }
    for (int i = 0; i < 2; ++i) if (g_fa_q[i]) { g_fa_q[i]->Release(); g_fa_q[i] = 0; }
}

int draw_full(const ntr::GxGpuFrame *f)
{
    if (!make_full_pipeline()) return -1;
    if (!ensure_targets(f->cw, f->ch, true)) return 0;
    /* grids need their own resources; without them the frame is handed back
       undrawn and gx_render turns the grids into triangles */
    if (f->ngrids && !grid_start()) return 0;
    g_ginst.clear();

    /* this frame's depth key (see kWDepthC): the vertex build, the clear, the
       depth states and the A/B readback below all read these */
    g_frame_w = f->wdepth != 0;
    g_depth_unscale = g_frame_w ? kWDepthUnscale : kDepthUnscale;
    const int dsi = g_frame_w ? 2 : 0;           /* index into the g_dss_* sets */
    const bool filt_arm = f->tex_filter >= 1;

    const bool smooth = f->aa_mode && f->pre && f->out && !f->ab &&
                        g_aa_on_full != 0;
    const bool keep_pre = smooth && (f->pre_needed || g_aacheck);
    if (smooth && !ensure_fa(f->cw, f->ch)) return -1;
    if (smooth) ++g_full_smooth;
    if (keep_pre && f->pre_needed) ++g_pre_kept;

    const long long t0 = qpc();

    /* ---- build: the opaque batches exactly as before, and the second pass's
       draws in submission order -------------------------------------------- */
    g_scratch.clear();
    g_scratch1.clear();
    g_scratch2.clear();
    g_gdraws.clear();
    g_gfig.clear();
    g_batch.clear();
    g_ops.clear();
    const GTex *cur_tex = 0;
    int cur_au = -1, cur_av = -1, cur_eq = -1;
    bool open = false;
    bool prev_mask = false, seen_mask_group = false;

    const float inv_cw = 2.0f / (float)f->cw;
    const float inv_ch = 2.0f / (float)f->ch;
    float bminx = 1e30f, bminy = 1e30f, bmaxx = -1e30f, bmaxy = -1e30f;
    bool bwide = false;

    /* ONE SMOOTHED GRID, in list order (see the section above). It joins the
       batch or the second-pass draw in progress when everything but the
       instances agrees, and starts a new one otherwise. */
    auto emit_grid = [&](const ntr::GxGrid &gr) -> bool {
        const ntr::GxTriangle &t = gr.st;
        if (!(t.cull & 3)) return true;             /* every sub-triangle culled */
        const bool pass1 = t.translucent != 0;
        if (pass1) prev_mask = false;
        if (gr.sbox[0] < bminx) bminx = gr.sbox[0];
        if (gr.sbox[2] > bmaxx) bmaxx = gr.sbox[2];
        if (gr.sbox[1] < bminy) bminy = gr.sbox[1];
        if (gr.sbox[3] > bmaxy) bmaxy = gr.sbox[3];
        const bool textured = t.tex && t.tw > 0 && t.th > 0;
        const GTex *gt = 0;
        int au = 0, av = 0;
        if (textured) {
            gt = upload_texture(t.tex_id, t.tex, t.tw, t.th, f->tex_filter);
            if (!gt) return false;
            const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
            const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
            au = rep_s ? (flip_s ? 2 : 1) : 0;
            av = rep_t ? (flip_t ? 2 : 1) : 0;
        }
        GInst I;
        memset(&I, 0, sizeof I);
        memcpy(I.xf, gr.xform, sizeof I.xf);
        memcpy(I.pj, gr.proj, sizeof I.pj);
        for (int k = 0; k < 4; ++k) I.vp[k] = (float)gr.vp[k];
        I.info[0] = gr.off;
        I.info[1] = (gr.from_pool ? 1u : 0u) | (gr.has_xform ? 2u : 0u) | (gr.persp ? 4u : 0u) | (gr.depth_snap ? 8u : 0u);
        I.info[2] = t.cull;
        const ntr::GxGridCorner *cs[3] = {&gr.s[0], &gr.s[1], &gr.s[2]};
        float *as[3] = {I.a0, I.a1, I.a2};
        for (int k = 0; k < 3; ++k) {
            as[k][0] = cs[k]->u;
            as[k][1] = cs[k]->v;
            memcpy(&as[k][2], &cs[k]->color, 4);
        }
        I.st[0] = gr.widen;
        if (gr.depth_snap) {   /* SMOOTHREND1: the flat triangle's plane rides in the four spare floats */
            I.a0[3] = gr.pl[0]; I.a1[3] = gr.pl[1]; I.a2[3] = gr.pl[2]; I.st[3] = gr.pl[3];
            memcpy(&I.info[3], &gr.snap_lim, 4);
        }
        I.st[1] = (float)((t.alpha >= 31 || t.alpha == 0) ? 31u : t.alpha);
        I.st[2] = (float)(t.polyid + (t.fog ? 256 : 0) + ((t.mode == 2 && f->toon_shade) ? 64 * f->toon_shade : 0));
        I.tx[0] = (float)(t.tex_scale ? t.tex_scale : 1);
        I.tx[1] = textured ? 1.0f / (float)t.tw : 0.0f;
        I.tx[2] = textured ? 1.0f / (float)t.th : 0.0f;
        I.tx[3] = textured ? 1.0f : 0.0f;
        const UINT inst = (UINT)g_ginst.size();
        g_ginst.push_back(I);
        const int lvl = gr.tf == 2 ? 0 : (gr.tf == 4 ? 1 : 2);
        if (!pass1) {
            if (open && !g_batch.back().is_grid)
                g_batch.back().count = (UINT)g_scratch.size() - g_batch.back().first;
            if (open && g_batch.back().is_grid && g_batch.back().tex == gt &&
                g_batch.back().au == au && g_batch.back().av == av &&
                g_batch.back().lvl == lvl) {
                ++g_batch.back().count;
                return true;
            }
            Batch nb;
            nb.tex = gt; nb.au = au; nb.av = av; nb.eq = 0;
            nb.first = inst; nb.count = 1;
            nb.is_grid = true; nb.lvl = lvl;
            g_batch.push_back(nb);
            cur_tex = gt; cur_au = au; cur_av = av; cur_eq = 0;
            open = true;
            return true;
        }
        const int kind = (t.alpha >= 31 || t.alpha == 0) ? OP_AB : OP_A;
        const int pid = (int)t.polyid;
        if (!g_ops.empty() && g_ops.back().kind != OP_CLRM && !g_ops.back().is_grid)
            g_ops.back().count = (UINT)g_scratch1.size() - g_ops.back().first;
        if (!g_ops.empty() && g_ops.back().is_grid && g_ops.back().kind == kind &&
            g_ops.back().tex == gt && g_ops.back().au == au && g_ops.back().av == av &&
            g_ops.back().pid == pid && g_ops.back().lvl == lvl) {
            ++g_ops.back().count;
            return true;
        }
        Op o;
        o.kind = kind; o.tex = gt; o.au = au; o.av = av; o.pid = pid; o.eq = 0;
        o.first = inst; o.count = 1;
        o.is_grid = true; o.lvl = lvl;
        g_ops.push_back(o);
        return true;
    };
    size_t gnext = 0;

    for (size_t i = 0; i < f->count; ++i) {
        while (gnext < f->ngrids && f->grids[gnext].at <= i)
            if (!emit_grid(f->grids[gnext++])) return 0;
        const ntr::GxTriangle &t = f->tris[i];
        const bool pass1 = t.translucent != 0;
        if (!pass1 && t.mode == 3) continue;        /* never happens; as before */
        const bool is_mask = pass1 && t.mode == 3 && t.polyid == 0;
        if (pass1) {
            /* gx.cpp's clear points: a mask polygon after any non-mask polygon
               of the second pass starts a new group, culled or not. The first
               group needs no clear, the stencil starts at zero. */
            if (is_mask && !prev_mask) {
                if (seen_mask_group) {
                    /* close the draw in progress before the clear splits it */
                    if (!g_ops.empty() && g_ops.back().kind != OP_CLRM &&
                        !g_ops.back().is_grid)
                        g_ops.back().count =
                            (UINT)g_scratch1.size() - g_ops.back().first;
                    Op c;
                    memset(&c, 0, sizeof c);
                    c.kind = OP_CLRM;
                    g_ops.push_back(c);
                }
                seen_mask_group = true;
            }
            prev_mask = is_mask;
        }

        const ntr::GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
        const float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (area < 1e-6f && area > -1e-6f) continue;
        const bool backface = area > 0.0f;
        if (backface && !(t.cull & 1)) continue;
        if (!backface && !(t.cull & 2)) continue;

        if (!is_mask) {
            for (int k = 0; k < 3; ++k) {
                const float vx = t.v[k].x, vy = t.v[k].y;
                if (!(vx > -1e20f && vx < 1e20f && vy > -1e20f && vy < 1e20f)) {
                    bwide = true;
                    continue;
                }
                if (vx < bminx) bminx = vx;
                if (vx > bmaxx) bmaxx = vx;
                if (vy < bminy) bminy = vy;
                if (vy > bmaxy) bmaxy = vy;
            }
        }

        const bool textured = !is_mask && t.tex && t.tw > 0 && t.th > 0;
        /* POLYGON_ATTR bit 14: the depth test is EQUAL; a shadow volume never
           asks */
        const int teq = (t.depth_eq && t.mode != 3) ? 1 : 0;
        const GTex *gt = 0;
        int au = 0, av = 0;
        if (textured) {
            gt = upload_texture(t.tex_id, t.tex, t.tw, t.th, f->tex_filter);
            if (!gt) return 0;                    /* fall_back already said why */
            const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
            const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
            au = rep_s ? (flip_s ? 2 : 1) : 0;
            av = rep_t ? (flip_t ? 2 : 1) : 0;
        }

        if (!pass1) {
            if (!open || g_batch.back().is_grid || gt != cur_tex || au != cur_au ||
                av != cur_av || teq != cur_eq) {
                if (open && !g_batch.back().is_grid) g_batch.back().count =
                    (UINT)g_scratch.size() - g_batch.back().first;
                Batch nb;
                nb.tex = gt;
                nb.au = au;
                nb.av = av;
                nb.eq = teq;
                nb.first = (UINT)g_scratch.size();
                nb.count = 0;
                g_batch.push_back(nb);
                cur_tex = gt;
                cur_au = au;
                cur_av = av;
                cur_eq = teq;
                open = true;
            }
            push_tri(t, f, textured, inv_cw, inv_ch, g_scratch, teq != 0);
            continue;
        }

        int kind;
        if (t.mode == 3) kind = is_mask ? OP_MASK : OP_SHADOW;
        else kind = (t.alpha >= 31 || t.alpha == 0) ? OP_AB : OP_A;
        const int pid = (kind == OP_A || kind == OP_AB) ? (int)t.polyid : 0;
        if (g_ops.empty() || g_ops.back().is_grid || g_ops.back().kind != kind ||
            g_ops.back().tex != gt ||
            g_ops.back().au != au || g_ops.back().av != av ||
            g_ops.back().pid != pid || g_ops.back().eq != teq) {
            if (!g_ops.empty() && g_ops.back().kind != OP_CLRM && !g_ops.back().is_grid)
                g_ops.back().count = (UINT)g_scratch1.size() - g_ops.back().first;
            Op o;
            o.kind = kind;
            o.tex = gt;
            o.au = au;
            o.av = av;
            o.pid = pid;
            o.eq = teq;
            o.first = (UINT)g_scratch1.size();
            o.count = 0;
            g_ops.push_back(o);
        }
        push_tri(t, f, textured, inv_cw, inv_ch, g_scratch1, teq != 0);
    }
    while (gnext < f->ngrids)
        if (!emit_grid(f->grids[gnext++])) return 0;
    if (open && !g_batch.back().is_grid)
        g_batch.back().count = (UINT)g_scratch.size() - g_batch.back().first;
    if (!g_ops.empty() && g_ops.back().kind != OP_CLRM && !g_ops.back().is_grid)
        g_ops.back().count = (UINT)g_scratch1.size() - g_ops.back().first;

    /* the grids' instances, the pool mirror and the constants: on any failure
       the frame goes back undrawn and gx_render expands the grids itself */
    if (!g_ginst.empty() && !grid_upload(f)) return 0;
    if (!g_ginst.empty()) { ++g_grid_frames; g_grid_insts += (long long)g_ginst.size(); }

    /* ---- the ghost figures (run mp1, lane GHOSTLAYER1): one group per figure
       number, farthest first (largest mean w), drawn as batches by texture ---- */
    if (f->ghost_tris && f->ghost_count) {
        struct Fig { int id; double w; long n; };
        Fig figs[8];
        int nfig = 0;
        for (size_t i = 0; i < f->ghost_count; ++i) {
            const ntr::GxTriangle &t = f->ghost_tris[i];
            int k = 0;
            while (k < nfig && figs[k].id != t.ghost) ++k;
            if (k == nfig) {
                if (nfig >= 8) continue;
                figs[nfig].id = t.ghost;
                figs[nfig].w = 0.0;
                figs[nfig].n = 0;
                ++nfig;
            }
            figs[k].w += (double)t.v[0].w + (double)t.v[1].w + (double)t.v[2].w;
            figs[k].n += 3;
        }
        for (int k = 0; k < nfig; ++k) figs[k].w /= (double)(figs[k].n ? figs[k].n : 1);
        for (int a2 = 1; a2 < nfig; ++a2)             /* far (large w) first */
            for (int b2 = a2; b2 > 0 && figs[b2].w > figs[b2 - 1].w; --b2) {
                const Fig tmp = figs[b2];
                figs[b2] = figs[b2 - 1];
                figs[b2 - 1] = tmp;
            }
        for (int k = 0; k < nfig; ++k) {
            g_gfig.push_back(g_gdraws.size());
            const GTex *ctex = 0;
            int cau = -1, cav = -1;
            bool gopen = false;
            for (size_t i = 0; i < f->ghost_count; ++i) {
                const ntr::GxTriangle &t = f->ghost_tris[i];
                if (t.ghost != figs[k].id) continue;
                const ntr::GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
                const float area =
                    (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
                if (area < 1e-6f && area > -1e-6f) continue;
                const bool backface = area > 0.0f;
                if (backface && !(t.cull & 1)) continue;
                if (!backface && !(t.cull & 2)) continue;
                for (int q = 0; q < 3; ++q) {
                    const float vx = t.v[q].x, vy = t.v[q].y;
                    if (!(vx > -1e20f && vx < 1e20f && vy > -1e20f && vy < 1e20f)) {
                        bwide = true;
                        continue;
                    }
                    if (vx < bminx) bminx = vx;
                    if (vx > bmaxx) bmaxx = vx;
                    if (vy < bminy) bminy = vy;
                    if (vy > bmaxy) bmaxy = vy;
                }
                const bool textured = t.tex && t.tw > 0 && t.th > 0;
                const GTex *gt = 0;
                int au = 0, av = 0;
                if (textured) {
                    gt = upload_texture(t.tex_id, t.tex, t.tw, t.th, f->tex_filter);
                    if (!gt) return 0;
                    const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
                    const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
                    au = rep_s ? (flip_s ? 2 : 1) : 0;
                    av = rep_t ? (flip_t ? 2 : 1) : 0;
                }
                if (!gopen || gt != ctex || au != cau || av != cav) {
                    if (gopen)
                        g_gdraws.back().count =
                            (UINT)g_scratch2.size() - g_gdraws.back().first;
                    GhostDraw d;
                    d.tex = gt;
                    d.au = au;
                    d.av = av;
                    d.first = (UINT)g_scratch2.size();
                    d.count = 0;
                    g_gdraws.push_back(d);
                    ctex = gt;
                    cau = au;
                    cav = av;
                    gopen = true;
                }
                push_tri(t, f, textured, inv_cw, inv_ch, g_scratch2, false, 30.0f);
            }
            if (gopen)
                g_gdraws.back().count =
                    (UINT)g_scratch2.size() - g_gdraws.back().first;
        }
        g_gfig.push_back(g_gdraws.size());          /* the end sentinel */
        ++g_gh_frames;
        g_gh_tris += (long long)(g_scratch2.size() / 3);
    }

    const long long t1 = qpc();

    const bool nothing = g_scratch.empty() && g_scratch1.empty() && g_ginst.empty() &&
                         g_scratch2.empty();
    const UINT nopaque = (UINT)g_scratch.size();

    if (!nothing) port_gpu_timer_span_begin(PORT_GPU_SPAN_OPAQUE);

    const float clr[4] = {
        (float)((f->clear_argb >> 16) & 0xFF) / 255.0f,
        (float)((f->clear_argb >> 8) & 0xFF) / 255.0f,
        (float)(f->clear_argb & 0xFF) / 255.0f,
        0.0f,                      /* alpha is coverage: nothing covered yet */
    };
    const float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (!nothing) {
        g_ctx->ClearRenderTargetView(g_col_rtv, clr);
        g_ctx->ClearRenderTargetView(g_id_rtv, zero);
        g_ctx->ClearDepthStencilView(g_dep_dsv,
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                     g_frame_w ? 0.0f : 1.0f, 0);

        if (!ensure_vb(g_scratch.size() + g_scratch1.size() + g_scratch2.size()))
            return 0;
        D3D11_MAPPED_SUBRESOURCE ms;
        memset(&ms, 0, sizeof ms);
        HRESULT hr = g_ctx->Map(g_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        if (FAILED(hr)) { fall_back("the vertex buffer would not open", hr); return 0; }
        if (!g_scratch.empty())
            memcpy(ms.pData, &g_scratch[0], g_scratch.size() * sizeof(GVert));
        if (!g_scratch1.empty())
            memcpy((GVert *)ms.pData + g_scratch.size(), &g_scratch1[0],
                   g_scratch1.size() * sizeof(GVert));
        if (!g_scratch2.empty())
            memcpy((GVert *)ms.pData + g_scratch.size() + g_scratch1.size(),
                   &g_scratch2[0], g_scratch2.size() * sizeof(GVert));
        g_ctx->Unmap(g_vb, 0);

        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = (float)f->cw;
        vp.Height = (float)f->ch;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        g_ctx->RSSetViewports(1, &vp);
        D3D11_RECT sc;
        sc.left = f->px0;
        sc.top = f->py0;
        sc.right = f->px0 + f->pw;
        sc.bottom = f->py0 + f->ph;
        /* THE SCISSOR IS THE SOFTWARE RASTER'S CLIP RECTANGLE (run hunt8, lane
           CREDITCROP1): the present rectangle narrowed to the game's viewport,
           so the ending's letterbox bars stay the clear colour. The frame's
           own geometry (px0..ph, which a held frame is composited by) stays
           the present rectangle. */
        if (f->clip_x1 > f->clip_x0 && f->clip_y1 > f->clip_y0) {
            if (f->clip_x0 > sc.left) sc.left = f->clip_x0;
            if (f->clip_y0 > sc.top) sc.top = f->clip_y0;
            if (f->clip_x1 < sc.right) sc.right = f->clip_x1;
            if (f->clip_y1 < sc.bottom) sc.bottom = f->clip_y1;
            if (sc.right < sc.left) sc.right = sc.left;
            if (sc.bottom < sc.top) sc.bottom = sc.top;
        }
        g_ctx->RSSetScissorRects(1, &sc);
        g_ctx->RSSetState(g_rast);
        g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        const UINT stride = (UINT)sizeof(GVert), offset = 0;
        g_ctx->IASetVertexBuffers(0, 1, &g_vb, &stride, &offset);
        if (!upload_frame_consts(f)) return 0;
        g_ctx->PSSetConstantBuffers(0, 1, &g_toon_cb);

        const float bf[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        ID3D11ShaderResourceView *nosrv = 0;
        ID3D11RenderTargetView *rtv2[2] = { g_col_rtv, g_id_rtv };
        ID3D11RenderTargetView *rtv1[1] = { g_col_rtv };
        bool id_bound = false;

        /* ---- the opaque pass: unchanged apart from the blend, which also
           keeps the largest alpha as coverage ------------------------------ */
        if (!g_batch.empty()) {
            g_ctx->OMSetRenderTargets(2, rtv2, g_dep_dsv);
            g_ctx->OMSetBlendState(g_blend_full, bf, 0xffffffffu);
            g_ctx->IASetInputLayout(g_layout);
            g_ctx->VSSetShader(g_vs, 0, 0);
            g_ctx->PSSetShader(filt_arm ? g_ps_filt : g_ps, 0, 0);
            bool grid_on = false;
            for (size_t bi = 0; bi < g_batch.size(); ++bi) {
                const Batch &bt = g_batch[bi];
                if (!bt.count) continue;
                ID3D11ShaderResourceView *srv = bt.tex ? bt.tex->srv : g_white_srv;
                ID3D11SamplerState *smp =
                    bt.tex ? sampler(f->tex_filter, bt.au, bt.av) : sampler(0, 0, 0);
                if (!smp) { fall_back("no sampler state", 0); return 0; }
                g_ctx->PSSetShaderResources(0, 1, &srv);
                g_ctx->PSSetSamplers(0, 1, &smp);
                if (filt_arm) {
                    ID3D11SamplerState *psm = bt.tex ? psampler(bt.au, bt.av) : psampler(0, 0);
                    if (!psm) { fall_back("no point sampler state", 0); return 0; }
                    g_ctx->PSSetSamplers(1, 1, &psm);
                }
                g_ctx->OMSetDepthStencilState(g_dss_v[dsi + bt.eq], 0);
                if (bt.is_grid) {
                    if (!grid_on) { grid_bind(); grid_on = true; }
                    if (!grid_draw(bt.first, bt.count, bt.lvl)) {
                        grid_give_up("the grid draw constants would not open", 0);
                        return 0;
                    }
                } else {
                    if (grid_on) {
                        g_ctx->IASetInputLayout(g_layout);
                        g_ctx->VSSetShader(g_vs, 0, 0);
                        grid_on = false;
                    }
                    g_ctx->Draw(bt.count, bt.first);
                }
                ++g_batches;
            }
        }

        /* ---- the second pass ------------------------------------------- */
        for (size_t oi = 0; oi < g_ops.size(); ++oi) {
            const Op &op = g_ops[oi];
            if (op.kind != OP_SHADOW && id_bound) {
                g_ctx->PSSetShaderResources(1, 1, &nosrv);
                id_bound = false;
            }
            if (op.kind == OP_CLRM) {
                g_ctx->OMSetRenderTargets(0, 0, g_dep_dsv);
                g_ctx->OMSetDepthStencilState(g_dss_clrm, 0);
                g_ctx->OMSetBlendState(0, bf, 0xffffffffu);
                g_ctx->IASetInputLayout(0);
                g_ctx->VSSetShader(g_full_vs, 0, 0);
                g_ctx->PSSetShader(0, 0, 0);
                g_ctx->Draw(3, 0);
                ++g_batches;
                continue;
            }
            if (!op.count) continue;
            g_ctx->IASetInputLayout(g_layout);
            g_ctx->VSSetShader(g_vs, 0, 0);
            if (op.is_grid) grid_bind();
            auto draw_op = [&]() -> bool {
                if (op.is_grid) return grid_draw(op.first, op.count, op.lvl);
                g_ctx->Draw(op.count, nopaque + op.first);
                return true;
            };
            if (op.kind == OP_MASK) {
                g_ctx->OMSetRenderTargets(0, 0, g_dep_dsv);
                g_ctx->OMSetDepthStencilState(g_dss_mask[dsi], 0x80);
                g_ctx->OMSetBlendState(0, bf, 0xffffffffu);
                g_ctx->PSSetShader(0, 0, 0);
                g_ctx->Draw(op.count, nopaque + op.first);
                ++g_batches;
                continue;
            }
            ID3D11ShaderResourceView *srv = op.tex ? op.tex->srv : g_white_srv;
            ID3D11SamplerState *smp =
                op.tex ? sampler(f->tex_filter, op.au, op.av) : sampler(0, 0, 0);
            if (!smp) { fall_back("no sampler state", 0); return 0; }
            g_ctx->PSSetShaderResources(0, 1, &srv);
            g_ctx->PSSetSamplers(0, 1, &smp);
            if (filt_arm) {
                ID3D11SamplerState *psm = op.tex ? psampler(op.au, op.av) : psampler(0, 0);
                if (!psm) { fall_back("no point sampler state", 0); return 0; }
                g_ctx->PSSetSamplers(1, 1, &psm);
            }
            if (op.kind == OP_SHADOW) {
                g_ctx->OMSetRenderTargets(1, rtv1, g_dep_dsv);
                g_ctx->OMSetDepthStencilState(g_dss_shadow[dsi], 0x80);
                g_ctx->OMSetBlendState(g_blend_full, bf, 0xffffffffu);
                g_ctx->PSSetShader(filt_arm ? g_ps_shadow_f : g_ps_shadow, 0, 0);
                g_ctx->PSSetShaderResources(1, 1, &g_id_srv);
                id_bound = true;
                g_ctx->Draw(op.count, nopaque + op.first);
                ++g_batches;
                continue;
            }
            /* a translucent draw: the pixels under 31 first ... */
            g_ctx->OMSetRenderTargets(1, rtv1, g_dep_dsv);
            g_ctx->OMSetDepthStencilState(g_dss_tl[dsi + op.eq], (UINT)(0x40 | op.pid));
            g_ctx->OMSetBlendState(g_blend_full, bf, 0xffffffffu);
            g_ctx->PSSetShader(filt_arm ? g_ps_tl_f : g_ps_tl, 0, 0);
            if (!draw_op()) { grid_give_up("the grid draw constants would not open", 0); return 0; }
            ++g_batches;
            if (op.kind == OP_AB) {
                /* ... then, for a polygon that can reach full alpha, the pixels
                   at 31, drawn as the opaque pixels gx.cpp calls them */
                g_ctx->OMSetRenderTargets(2, rtv2, g_dep_dsv);
                g_ctx->OMSetDepthStencilState(g_dss_tlop[dsi + op.eq], 0);
                g_ctx->PSSetShader(filt_arm ? g_ps_tlop_f : g_ps_tlop, 0, 0);
                if (!draw_op()) { grid_give_up("the grid draw constants would not open", 0); return 0; }
                ++g_batches;
            }
        }
        /* ---- the ghost figures: each one a depth-only pass and then one
           blended colour pass at depth EQUAL, so a pixel takes exactly one
           fragment of the figure, farthest figure first. The figure's depth is
           left in the depth target, as an opaque ghost's always was. -------- */
        if (g_gfig.size() > 1) {
            const UINT gbase = (UINT)(g_scratch.size() + g_scratch1.size());
            g_ctx->IASetInputLayout(g_layout);
            g_ctx->VSSetShader(g_vs, 0, 0);
            for (size_t fi = 0; fi + 1 < g_gfig.size(); ++fi) {
                const size_t d0 = g_gfig[fi], d1 = g_gfig[fi + 1];
                /* (a) depth */
                g_ctx->OMSetRenderTargets(0, 0, g_dep_dsv);
                g_ctx->OMSetDepthStencilState(g_dss_gz[g_frame_w ? 1 : 0], 0);
                g_ctx->OMSetBlendState(0, bf, 0xffffffffu);
                g_ctx->PSSetShader(filt_arm ? g_ps_filt : g_ps, 0, 0);
                for (size_t di = d0; di < d1; ++di) {
                    const GhostDraw &gd = g_gdraws[di];
                    if (!gd.count) continue;
                    ID3D11ShaderResourceView *srv =
                        gd.tex ? gd.tex->srv : g_white_srv;
                    ID3D11SamplerState *smp = gd.tex ? sampler(f->tex_filter, gd.au, gd.av)
                                                     : sampler(0, 0, 0);
                    if (!smp) { fall_back("no sampler state", 0); return 0; }
                    g_ctx->PSSetShaderResources(0, 1, &srv);
                    g_ctx->PSSetSamplers(0, 1, &smp);
                    if (filt_arm) {
                        ID3D11SamplerState *psm = gd.tex ? psampler(gd.au, gd.av) : psampler(0, 0);
                        if (!psm) { fall_back("no point sampler state", 0); return 0; }
                        g_ctx->PSSetSamplers(1, 1, &psm);
                    }
                    g_ctx->Draw(gd.count, gbase + gd.first);
                    ++g_batches;
                }
                /* (b) colour */
                g_ctx->ClearDepthStencilView(g_dep_dsv, D3D11_CLEAR_STENCIL, 1.0f, 0);
                g_ctx->OMSetRenderTargets(1, rtv1, g_dep_dsv);
                g_ctx->OMSetDepthStencilState(g_dss_gc, 0);
                g_ctx->OMSetBlendState(g_blend_full, bf, 0xffffffffu);
                g_ctx->PSSetShader(filt_arm ? g_ps_tl_f : g_ps_tl, 0, 0);
                for (size_t di = d0; di < d1; ++di) {
                    const GhostDraw &gd = g_gdraws[di];
                    if (!gd.count) continue;
                    ID3D11ShaderResourceView *srv =
                        gd.tex ? gd.tex->srv : g_white_srv;
                    ID3D11SamplerState *smp = gd.tex ? sampler(f->tex_filter, gd.au, gd.av)
                                                     : sampler(0, 0, 0);
                    if (!smp) { fall_back("no sampler state", 0); return 0; }
                    g_ctx->PSSetShaderResources(0, 1, &srv);
                    g_ctx->PSSetSamplers(0, 1, &smp);
                    if (filt_arm) {
                        ID3D11SamplerState *psm = gd.tex ? psampler(gd.au, gd.av) : psampler(0, 0);
                        if (!psm) { fall_back("no point sampler state", 0); return 0; }
                        g_ctx->PSSetSamplers(1, 1, &psm);
                    }
                    g_ctx->Draw(gd.count, gbase + gd.first);
                    ++g_batches;
                }
            }
        }
        ID3D11ShaderResourceView *nones[2] = { 0, 0 };
        g_ctx->PSSetShaderResources(0, 2, nones);
        g_ctx->OMSetRenderTargets(0, 0, 0);
        if (!g_ginst.empty()) {
            grid_unbind();
            g_ctx->IASetInputLayout(g_layout);
            g_ctx->VSSetShader(g_vs, 0, 0);
        }
    }

    /* the box the readback needs: the triangles' own screen box plus a pixel of
       margin each side, cut to the present rectangle */
    int bx0 = f->px0, by0 = f->py0;
    int bx1 = f->px0 + f->pw, by1 = f->py0 + f->ph;
    if (!nothing && !bwide) {
        if (bminx - 1.0f > (float)bx0) bx0 = (int)(bminx - 1.0f);
        if (bminy - 1.0f > (float)by0) by0 = (int)(bminy - 1.0f);
        if (bmaxx + 2.0f < (float)bx1) bx1 = (int)(bmaxx + 2.0f);
        if (bmaxy + 2.0f < (float)by1) by1 = (int)(bmaxy + 2.0f);
    }
    /* a frame with only mask volumes draws no pixel at all */
    const bool drew_pixels = bwide || (bminx <= bmaxx && bminy <= bmaxy);
    const bool have_box = !nothing && drew_pixels && bx1 > bx0 && by1 > by0;

    /* ---- the smoothing, on the card's own colour target ------------------- */
    bool smoothed = false;
    if (smooth && have_box) {
        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = (float)f->cw;
        vp.Height = (float)f->ch;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        g_ctx->RSSetViewports(1, &vp);
        D3D11_RECT sc;
        sc.left = bx0;
        sc.top = by0;
        sc.right = bx1;
        sc.bottom = by1;
        g_ctx->RSSetScissorRects(1, &sc);
        g_ctx->RSSetState(g_rast);
        g_ctx->OMSetRenderTargets(1, &g_fa_rtv, 0);
        g_ctx->OMSetDepthStencilState(0, 0);
        g_ctx->OMSetBlendState(0, 0, 0xffffffffu);
        g_ctx->IASetInputLayout(0);
        g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_ctx->VSSetShader(g_full_vs, 0, 0);
        g_ctx->PSSetShader(g_ps_faa, 0, 0);
        g_ctx->PSSetShaderResources(0, 1, &g_col_srv);
        g_ctx->Draw(3, 0);
        ID3D11ShaderResourceView *none = 0;
        g_ctx->PSSetShaderResources(0, 1, &none);
        g_ctx->OMSetRenderTargets(0, 0, 0);
        smoothed = true;
    }

    /* ---- THE FRAME STAYS ON THE CARD (see cp_keep_ok): nothing is copied or
       read; the targets this frame lives in are left as they are for the
       composite, a settle or the capture picture --------------------------- */
    if (f->keep && !f->ab && cp_keep_ok()) {
        if (!nothing) port_gpu_timer_span_end(PORT_GPU_SPAN_OPAQUE);
        const long long t2 = qpc();
        ++g_frames;
        const size_t kverts = g_scratch.size() + g_scratch1.size() + g_scratch2.size();
        g_tris += (long long)(kverts / 3);
        g_verts += (long long)kverts;
        g_ms_build += ms_between(t0, t1);
        g_ms_draw += ms_between(t1, t2);
        if (smoothed) {
            ++g_aa_frames;
            g_aa_ms += ms_between(t1, t2);
        }
        g_cp_held = true;
        g_cp_smoothed = smoothed;
        g_cp_box[0] = have_box ? bx0 : 0;
        g_cp_box[1] = have_box ? by0 : 0;
        g_cp_box[2] = have_box ? bx1 : 0;
        g_cp_box[3] = have_box ? by1 : 0;
        g_cp_cw = f->cw;
        g_cp_ch = f->ch;
        g_cp_px0 = f->px0;
        g_cp_py0 = f->py0;
        g_cp_pw = f->pw;
        g_cp_ph = f->ph;
        g_cp_clear = f->clear_argb;
        g_cp_outside = f->outside_argb;
        g_cp_fade = 0;
        g_cp_evy = 0;
        g_cp_white = 0;
        g_cp_mask = 0;
        if (f->out) {
            f->out->aa_done = smooth ? 1 : 0;
            f->out->pre_valid = 0;
            f->out->aa_changed = 0;
            f->out->kept = 1;
        }
        port::hitch_add(port::HK_GPUBUILD, ms_between(t0, t1));   /* HITCH9C: the held path has no readback */
        port::hitch_add(port::HK_GPUDRAW, ms_between(t1, t2));
        if (g_perf) {
            /* the same per-30-pictures line the readback path prints, with a
               readback that is not there */
            g_perf_build += ms_between(t0, t1);
            g_perf_draw += ms_between(t1, t2);
            g_perf_tris += (long long)(kverts / 3);
            g_perf_batches += (long long)(g_batch.size() + g_ops.size());
            if (++g_perf_n >= 30) {
                fprintf(stderr, "[renderer] build %6.3fms submit %6.3fms readback "
                        "%6.3fms (wait %6.3fms copy %6.3fms) tris %6lld batches "
                        "%4lld (whole frame, held on the card)\n",
                        g_perf_build / g_perf_n, g_perf_draw / g_perf_n, 0.0,
                        0.0, 0.0, g_perf_tris / g_perf_n,
                        g_perf_batches / g_perf_n);
                g_perf_build = g_perf_draw = g_perf_read = 0.0;
                g_perf_wait = g_perf_copy = 0.0;
                g_perf_tris = g_perf_batches = 0;
                g_perf_n = 0;
            }
        }
        g_last_full = true;
        g_last_box = have_box;
        g_lb_x0 = bx0;
        g_lb_y0 = by0;
        g_lb_x1 = bx1;
        g_lb_y1 = by1;
        return 2;
    }

    /* ---- copy out to the staging textures --------------------------------- */
    D3D11_BOX box;
    box.left = (UINT)bx0;
    box.right = (UINT)bx1;
    box.top = (UINT)by0;
    box.bottom = (UINT)by1;
    box.front = 0;
    box.back = 1;
    const bool read_col = have_box && (!smoothed || keep_pre);
    if (read_col)
        g_ctx->CopySubresourceRegion(g_col_stage, 0, (UINT)bx0, (UINT)by0, 0,
                                     g_col, 0, &box);
    /* MENU2: a late smoothed frame goes to one of the two late staging textures, and is finished below */
    ID3D11Texture2D *fa_dst = g_fa_st;
    int lp_slot = 0;
    bool late_q = false;
    if (f->late && late_gpu_on() && smoothed && !keep_pre && have_box && !f->ab && !f->keep &&
        (f->defer || g_lp_n == 0) && !(g_fail_readback && g_frames >= g_fail_readback)) {
        if (g_lp_n >= 2) late_finish_one();
        if (ensure_fa_q(f->cw, f->ch)) {
            lp_slot = (g_lp_head + g_lp_n) & 1;
            fa_dst = g_fa_q[lp_slot];
            late_q = true;
        }
    }
    if (smoothed)
        g_ctx->CopySubresourceRegion(fa_dst, 0, (UINT)bx0, (UINT)by0, 0,
                                     g_fa_out, 0, &box);
    const bool read_ab = have_box && f->ab;
    if (read_ab) {
        g_ctx->CopyResource(g_id_stage, g_id);
        g_ctx->CopyResource(g_dep_stage, g_dep);
    }
    if (!nothing) port_gpu_timer_span_end(PORT_GPU_SPAN_OPAQUE);

    const long long t2 = qpc();

    if (late_q) {
        g_ctx->Flush();                       /* the card starts now; the caller's clears (and, deferred, its other work) hide under it */
        LatePend &P = g_lp[lp_slot];
        P.f = *f;
        P.f.out = 0;
        P.bx0 = bx0; P.by0 = by0; P.bx1 = bx1; P.by1 = by1;
        P.q = lp_slot;
        P.t0 = t0; P.t1 = t1; P.t2 = t2;
        P.nverts = g_scratch.size() + g_scratch1.size() + g_scratch2.size();
        P.nbatches = g_batch.size() + g_ops.size();
        ++g_lp_n;
        late_drill_init();
        ++g_late_issued;
        if (g_late_drill_kind == 2 && g_late_issued == g_late_drill_n) {
#ifndef PORT_ROM_CLEAN
            fprintf(stderr, "[renderer] SM64DS_MENU_LATE_DRILL: dropping the targets with %d readback(s) pending\n", g_lp_n);
#endif
            release_targets();
            return 2;     /* the frame is drawn and its buffers hold the clear colour; the next frame builds the targets again */
        }
        g_last_full = true;
        g_last_box = have_box;
        g_lb_x0 = bx0;
        g_lb_y0 = by0;
        g_lb_x1 = bx1;
        g_lb_y1 = by1;
        if (f->out) {
            f->out->aa_done = 1;
            f->out->pre_valid = 0;
            f->out->aa_changed = 0;
            f->out->deferred = f->defer ? 1 : 0;
        }
        if (f->defer) return 2;
        P.f.out = f->out;
        late_finish_one();
        return 2;
    }
    /* not a frame the late path takes: the caller's clears now, before any CPU buffer is written */
    if (f->late && f->pre_read) f->pre_read(f->pre_arg);

    if (g_fail_readback && g_frames >= g_fail_readback) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_READBACK=%d reached "
                "after %lld frame(s); forcing a readback failure on purpose\n",
                g_fail_readback, g_frames);
        fall_back("a test asked the readback to fail", 0);
        return 0;
    }

    /* ---- read it back, ONCE: every Map first, so a failure leaves the CPU
       buffers exactly as gx.cpp cleared them -------------------------------- */
    double wait_ms = 0.0;
    D3D11_MAPPED_SUBRESOURCE mcol, mfa, mid, mdep;
    memset(&mcol, 0, sizeof mcol);
    memset(&mfa, 0, sizeof mfa);
    memset(&mid, 0, sizeof mid);
    memset(&mdep, 0, sizeof mdep);
    bool m_col = false, m_fa = false, m_id = false, m_dep = false;
    {
        const long long tw0 = qpc();
        HRESULT hr = 0;
        if (smoothed) {
            hr = g_ctx->Map(g_fa_st, 0, D3D11_MAP_READ, 0, &mfa);
            m_fa = SUCCEEDED(hr);
        }
        if (SUCCEEDED(hr) && read_col) {
            hr = g_ctx->Map(g_col_stage, 0, D3D11_MAP_READ, 0, &mcol);
            m_col = SUCCEEDED(hr);
        }
        if (SUCCEEDED(hr) && read_ab) {
            hr = g_ctx->Map(g_id_stage, 0, D3D11_MAP_READ, 0, &mid);
            m_id = SUCCEEDED(hr);
            if (SUCCEEDED(hr)) {
                hr = g_ctx->Map(g_dep_stage, 0, D3D11_MAP_READ, 0, &mdep);
                m_dep = SUCCEEDED(hr);
            }
        }
        wait_ms = ms_between(tw0, qpc());
        if (FAILED(hr)) {
            if (m_dep) g_ctx->Unmap(g_dep_stage, 0);
            if (m_id) g_ctx->Unmap(g_id_stage, 0);
            if (m_col) g_ctx->Unmap(g_col_stage, 0);
            if (m_fa) g_ctx->Unmap(g_fa_st, 0);
            fall_back("the picture could not be read back", hr);
            return 0;
        }
    }

    /* ---- from here the CPU buffers are written ---------------------------- */
    unsigned long long changed = 0;
    int pre_valid = 0;
    if (have_box) {
        if (smoothed && !keep_pre)
            copy_box_a(f, bx0, by0, bx1, by1, mfa, &changed, false);
        else
            copy_box_a(f, bx0, by0, bx1, by1, mcol, 0, false);
    }
    if (keep_pre) {
        /* aa_pass's own copy of the whole picture, taken before a pixel moves,
           then the smoothed pixels laid over the live frame */
        for (int y = 0; y < f->ch; ++y)
            memcpy(f->pre + (size_t)y * f->stride, f->fb + (size_t)y * f->stride,
                   (size_t)f->cw * sizeof(uint32_t));
        pre_valid = f->pre_needed ? 1 : 0;
        if (g_aacheck && smoothed) {
            /* THE CHECK: this file's copy of aa_band on the unsmoothed frame
               against what the card's pass made of it, covered pixels only */
            ntr::GxGpuAa ga;
            ga.src = f->pre;
            ga.cover = f->cover;
            ga.fb = f->fb;
            ga.stride = f->stride;
            ga.w = f->cw;
            ga.h = f->ch;
            ga.changed = 0;
            g_aac_ref.resize((size_t)f->cw * (size_t)f->ch);
            aa_ref(&ga, &g_aac_ref[0]);
            long long bad = 0;
            for (int y = by0; y < by1; ++y) {
                const uint32_t *orow = (const uint32_t *)
                    ((const unsigned char *)mfa.pData + (size_t)y * mfa.RowPitch);
                const uint32_t *rrow = &g_aac_ref[(size_t)y * f->cw];
                const uint8_t *crow = f->cover + (size_t)y * f->stride;
                for (int x = bx0; x < bx1; ++x)
                    if (crow[x] && (orow[x] & 0x00FFFFFFu) != (rrow[x] & 0x00FFFFFFu))
                        ++bad;
            }
            ++g_aac_frames;
            if (bad) {
                ++g_aac_bad_frames;
                g_aac_bad_px += bad;
                if (g_aac_bad_frames <= 5)
                    fprintf(stderr, "[renderer-aacheck] frame %lld: %lld pixel(s) "
                            "differ\n", g_frames, bad);
            }
        }
        if (smoothed)
            copy_box_a(f, bx0, by0, bx1, by1, mfa, &changed, true);
    }
    if (read_ab) copy_ab_planes(f, bx0, by0, bx1, by1, mid, mdep);
    if (m_dep) g_ctx->Unmap(g_dep_stage, 0);
    if (m_id) g_ctx->Unmap(g_id_stage, 0);
    if (m_col) g_ctx->Unmap(g_col_stage, 0);
    if (m_fa) g_ctx->Unmap(g_fa_st, 0);

    const long long t3 = qpc();
    ++g_frames;
    const size_t nverts = g_scratch.size() + g_scratch1.size() + g_scratch2.size();
    g_tris += (long long)(nverts / 3);
    g_verts += (long long)nverts;
    g_ms_build += ms_between(t0, t1);
    g_ms_draw += ms_between(t1, t2);
    g_ms_read += ms_between(t2, t3);
    port::hitch_add(port::HK_GPUBUILD, ms_between(t0, t1));   /* HITCH9C */
    port::hitch_add(port::HK_GPUDRAW, ms_between(t1, t2));
    port::hitch_add(port::HK_GPUWAIT, wait_ms);
    port::hitch_add(port::HK_GPUCOPY, ms_between(t2, t3) - wait_ms);
    if (smoothed) {
        ++g_aa_frames;
        g_aa_ms += ms_between(t1, t3);
    }

    if (g_perf) {
        g_perf_build += ms_between(t0, t1);
        g_perf_draw += ms_between(t1, t2);
        g_perf_read += ms_between(t2, t3);
        g_perf_wait += wait_ms;
        g_perf_copy += ms_between(t2, t3) - wait_ms;
        g_perf_tris += (long long)(nverts / 3);
        g_perf_batches += (long long)(g_batch.size() + g_ops.size());
        if (++g_perf_n >= 30) {
            fprintf(stderr, "[renderer] build %6.3fms submit %6.3fms readback "
                    "%6.3fms (wait %6.3fms copy %6.3fms) tris %6lld batches "
                    "%4lld (whole frame)\n",
                    g_perf_build / g_perf_n, g_perf_draw / g_perf_n,
                    g_perf_read / g_perf_n, g_perf_wait / g_perf_n,
                    g_perf_copy / g_perf_n, g_perf_tris / g_perf_n,
                    g_perf_batches / g_perf_n);
            g_perf_build = g_perf_draw = g_perf_read = 0.0;
            g_perf_wait = g_perf_copy = 0.0;
            g_perf_tris = g_perf_batches = 0;
            g_perf_n = 0;
        }
    }

    if (f->out) {
        f->out->aa_done = smooth ? 1 : 0;
        f->out->pre_valid = pre_valid;
        f->out->aa_changed = changed;
    }
    g_last_full = true;
    g_last_box = have_box;
    g_lb_x0 = bx0;
    g_lb_y0 = by0;
    g_lb_x1 = bx1;
    g_lb_y1 = by1;
    return 2;
}

/* THE DEPTH AND POLYGON-ID PLANES OF THE LAST WHOLE FRAME, on request. The
   whole-frame pass never reads them back (nothing in a game reads them); a
   caller that does (tests/smoke_gpu_raster.cpp, through gx_depth and
   gx_attr_ids) asks here, and gets them from the card's targets, which still
   hold that frame. */
int fill_planes(const ntr::GxGpuFrame *f)
{
    if (!g_last_full || g_down || !g_rt_full || !g_ctx || !g_id_stage || !g_dep_stage)
        return 0;
    if (!g_last_box) return 1;                /* nothing was drawn: all clear */
    g_ctx->CopyResource(g_id_stage, g_id);
    g_ctx->CopyResource(g_dep_stage, g_dep);
    D3D11_MAPPED_SUBRESOURCE mid, mdep;
    memset(&mid, 0, sizeof mid);
    memset(&mdep, 0, sizeof mdep);
    if (FAILED(g_ctx->Map(g_id_stage, 0, D3D11_MAP_READ, 0, &mid))) return 0;
    if (FAILED(g_ctx->Map(g_dep_stage, 0, D3D11_MAP_READ, 0, &mdep))) {
        g_ctx->Unmap(g_id_stage, 0);
        return 0;
    }
    copy_ab_planes(f, g_lb_x0, g_lb_y0, g_lb_x1, g_lb_y1, mid, mdep);
    g_ctx->Unmap(g_dep_stage, 0);
    g_ctx->Unmap(g_id_stage, 0);
    return 1;
}

/* ---- THE PROCESSOR'S COPY OF THE COMPOSITE RULE (run cardfull, CARD2) ------
 *
 * comp_ps in the shader file and cp_combine / cp_fade here are the same rule
 * written twice, in integers, so they cannot differ by rounding. This copy is
 * what a settle and the capture picture use, and what the check compares the
 * card's composite against. */
inline uint32_t cp_combine(uint32_t ov, uint32_t base, bool cov)
{
    const uint32_t mode = ov >> 24;
    if (mode == 0xFFu) return 0xFF000000u | (ov & 0x00FFFFFFu);
    if (mode == 0xFEu) return cov ? base : (0xFF000000u | (ov & 0x00FFFFFFu));
    return base;
}

inline uint32_t cp_fade(uint32_t p, int evy, int white)
{
    int r = (int)((p >> 16) & 0xFFu), g = (int)((p >> 8) & 0xFFu), b = (int)(p & 0xFFu);
    if (white) {
        r += ((255 - r) * evy) >> 4;
        g += ((255 - g) * evy) >> 4;
        b += ((255 - b) * evy) >> 4;
    } else {
        r -= (r * evy) >> 4;
        g -= (g * evy) >> 4;
        b -= (b * evy) >> 4;
    }
    return 0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

/* the box of the held frame copied to the two staging textures and mapped:
   `plain` is the opaque-and-translucent picture (alpha != 0 covered), `sm` the
   smoothed one. An empty box maps nothing and every pixel reads as uncovered. */
/* the menu layer's rule, the processor's twin of comp_ps's lay_over: out = c +
   (below * (255 - a) + 127) / 255 per channel, capped at 255 */
static inline uint32_t cp_lay_over(uint32_t lay, uint32_t below)
{
    const unsigned a = lay >> 24;
    if (!a) return below;
    const unsigned ia = 255u - a;
    unsigned r = ((lay >> 16) & 255u) + ((((below >> 16) & 255u) * ia + 127u) / 255u);
    unsigned g = ((lay >> 8) & 255u) + ((((below >> 8) & 255u) * ia + 127u) / 255u);
    unsigned bl = (lay & 255u) + (((below & 255u) * ia + 127u) / 255u);
    if (r > 255u) r = 255u;
    if (g > 255u) g = 255u;
    if (bl > 255u) bl = 255u;
    return 0xFF000000u | (r << 16) | (g << 8) | bl;
}

struct CpMaps {
    D3D11_MAPPED_SUBRESOURCE plain, sm;
    bool have_plain, have_sm;
    int x0, y0, x1, y1;
};

bool cp_map(CpMaps &m, bool want_plain, bool want_sm)
{
    memset(&m, 0, sizeof m);
    m.x0 = g_cp_box[0];
    m.y0 = g_cp_box[1];
    m.x1 = g_cp_box[2];
    m.y1 = g_cp_box[3];
    if (m.x1 <= m.x0 || m.y1 <= m.y0) return true;
    D3D11_BOX box;
    box.left = (UINT)m.x0;
    box.right = (UINT)m.x1;
    box.top = (UINT)m.y0;
    box.bottom = (UINT)m.y1;
    box.front = 0;
    box.back = 1;
    if (want_sm && !(g_cp_smoothed && g_fa_out && g_fa_st)) want_sm = false;
    if (want_plain && !(g_col && g_col_stage)) return false;
    if (want_sm)
        g_ctx->CopySubresourceRegion(g_fa_st, 0, (UINT)m.x0, (UINT)m.y0, 0,
                                     g_fa_out, 0, &box);
    if (want_plain)
        g_ctx->CopySubresourceRegion(g_col_stage, 0, (UINT)m.x0, (UINT)m.y0, 0,
                                     g_col, 0, &box);
    HRESULT hr = 0;
    if (want_sm) {
        hr = g_ctx->Map(g_fa_st, 0, D3D11_MAP_READ, 0, &m.sm);
        m.have_sm = SUCCEEDED(hr);
    }
    if (SUCCEEDED(hr) && want_plain) {
        hr = g_ctx->Map(g_col_stage, 0, D3D11_MAP_READ, 0, &m.plain);
        m.have_plain = SUCCEEDED(hr);
    }
    if (FAILED(hr)) {
        if (m.have_sm) g_ctx->Unmap(g_fa_st, 0);
        if (m.have_plain) g_ctx->Unmap(g_col_stage, 0);
        memset(&m, 0, sizeof m);
        return false;
    }
    return true;
}

void cp_unmap(CpMaps &m)
{
    if (m.have_sm) g_ctx->Unmap(g_fa_st, 0);
    if (m.have_plain) g_ctx->Unmap(g_col_stage, 0);
    m.have_sm = m.have_plain = false;
}

/* THE RULE, over the picture. `final_sm` says the picture shown is the smoothed
   one (else the plain one); `pre`, when not null, gets the same overlay over
   the plain picture, no fade (the frame before the smoothing); `cover`, when
   not null, gets 1 where the 3D engine drew. dst may be the overlay itself. */
void cp_resolve(const uint32_t *ov, int ov_stride, uint32_t *dst, int dst_stride,
                uint8_t *cover, uint32_t *pre, int aux_stride, bool fade,
                bool final_sm, const CpMaps &m)
{
    const int cw = g_cp_cw, ch = g_cp_ch;
    const int rx0 = g_cp_px0, ry0 = g_cp_py0;
    const int rx1 = g_cp_px0 + g_cp_pw, ry1 = g_cp_py0 + g_cp_ph;
    for (int y = 0; y < ch; ++y) {
        const uint32_t *orow = ov + (size_t)y * ov_stride;
        uint32_t *drow = dst + (size_t)y * dst_stride;
        const bool in_y = y >= m.y0 && y < m.y1;
        const bool inr_y = y >= ry0 && y < ry1;
        const uint32_t *srow = (in_y && m.have_sm)
            ? (const uint32_t *)((const unsigned char *)m.sm.pData +
                                 (size_t)(y - m.y0) * m.sm.RowPitch) : 0;
        const uint32_t *prow = (in_y && m.have_plain)
            ? (const uint32_t *)((const unsigned char *)m.plain.pData +
                                 (size_t)(y - m.y0) * m.plain.RowPitch) : 0;
        for (int x = 0; x < cw; ++x) {
            const uint32_t o = orow[x];
            uint32_t base = (inr_y && x >= rx0 && x < rx1) ? g_cp_clear
                                                           : g_cp_outside;
            uint32_t basepre = base;
            bool cov = false;
            if (in_y && x >= m.x0 && x < m.x1) {
                const int i = x - m.x0;
                if (final_sm) {
                    if (srow && (srow[i] >> 24)) {
                        cov = true;
                        base = 0xFF000000u | (srow[i] & 0x00FFFFFFu);
                    }
                    if (prow && (prow[i] >> 24))
                        basepre = 0xFF000000u | (prow[i] & 0x00FFFFFFu);
                } else if (prow && (prow[i] >> 24)) {
                    cov = true;
                    base = 0xFF000000u | (prow[i] & 0x00FFFFFFu);
                    basepre = base;
                }
            }
            uint32_t p = cp_combine(o, base, cov);
            const uint32_t lay = (g_lay_live && x < g_lay_w && y < g_lay_h)
                ? g_lay_cpu[(size_t)y * (size_t)g_lay_stride + (size_t)x] : 0u;
            const uint32_t lay2 = (lay && g_lay_two) ? g_lay2_cpu[(size_t)y * (size_t)g_lay_stride + (size_t)x] : 0u;
            if (lay && !g_lay_post) { p = cp_lay_over(lay, p); if (lay2) p = cp_lay_over(lay2, p); }
            if (fade) {
                /* does the fade take this pixel? Every one, unless the frame
                   has a brightness mask (comp_ps carries the same rule) */
                bool take = true;
                if (g_cp_mask) {
                    const unsigned code = g_cp_mask[(size_t)y * g_cp_mask_stride + x];
                    const bool is3d = g_cp_clear_opaque || (g_cp_shown3d && cov);
                    const bool def = (code & (is3d ? 1u : 2u)) != 0;
                    const unsigned mode = o >> 24;
                    if (mode == 0xFFu) take = ((code >> 2) & 1u) != 0;
                    else if (mode == 0xFEu) take = cov ? def : (((code >> 2) & 1u) != 0);
                    else take = def;
                }
                if (take) p = cp_fade(p, g_cp_evy, g_cp_white);
            }
            if (lay && g_lay_post) { p = cp_lay_over(lay, p); if (lay2) p = cp_lay_over(lay2, p); }
            if (cover) cover[(size_t)y * aux_stride + x] = cov ? 1 : 0;
            if (pre) pre[(size_t)y * aux_stride + x] = cp_combine(o, basepre, cov);
            drow[x] = p;
        }
    }
}

/* SETTLE 1 and 2 (ntr/gx.h, GxGpuFrame::settle). 1 turns the framebuffer into
   the ordinary finished frame in place (fade included when one was recorded),
   fills the coverage plane and, for a smoothed frame, the copy before the
   smoothing. 2 writes the picture the display capture reads into f->pre. */
/* SETTLE 3 (run hunt8, lane CARDSM1): the finished picture of a tick that was
   composited on the card ahead of the pictures drawn between ticks, read back
   from the composite target into f->fb (alpha forced opaque). It is the same
   picture settle 1 would have made, fade included, because the composite
   applied the same rule. 0 when the target is gone. */
/* TWODGPU1: the section below cp_compose_inter. tw_fold turns the tick's overlay
   on the card into the ordinary one in `fb` (and the mask in the processor's
   buffer), tw_flush merges what the processor wrote into `ov` itself. */
void tw_fold(uint32_t *fb, int stride, bool finish);
void tw_flush(const uint32_t *ov, int stride_px);
bool tw_check_on();

int card_ret_readback(const ntr::GxGpuFrame *f)
{
    /* slot 2 (run hunt8, lane GPUINTERP1): the in-between composite */
    ID3D11Texture2D *const src_tex = f->slot == 2 ? g_comp2_tex : g_comp_tex;
    const bool src_valid = f->slot == 2 ? g_comp2_valid : g_comp_valid;
    if (!src_valid || g_cp_dead || g_down || !g_ctx || !src_tex || !f->fb)
        return 0;
    if (!g_comp_stage) {
        D3D11_TEXTURE2D_DESC d;
        memset(&d, 0, sizeof d);
        d.Width = (UINT)g_cp_w;
        d.Height = (UINT)g_cp_h;
        d.MipLevels = d.ArraySize = 1;
        d.SampleDesc.Count = 1;
        d.Usage = D3D11_USAGE_STAGING;
        d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        if (FAILED(g_dev->CreateTexture2D(&d, 0, &g_comp_stage))) return 0;
    }
    g_ctx->CopyResource(g_comp_stage, src_tex);
    D3D11_MAPPED_SUBRESOURCE cm;
    memset(&cm, 0, sizeof cm);
    if (FAILED(g_ctx->Map(g_comp_stage, 0, D3D11_MAP_READ, 0, &cm))) return 0;
    for (int y = 0; y < g_cp_h; ++y) {
        const uint32_t *src = (const uint32_t *)
            ((const unsigned char *)cm.pData + (size_t)y * cm.RowPitch);
        uint32_t *dst = f->fb + (size_t)y * f->stride;
        for (int x = 0; x < g_cp_w; ++x) dst[x] = 0xFF000000u | (src[x] & 0x00FFFFFFu);
    }
    g_ctx->Unmap(g_comp_stage, 0);
    ++g_cp_settled;
    return 1;
}

int card_settle(const ntr::GxGpuFrame *f)
{
    if (f->settle == 3) return card_ret_readback(f);
    if (!g_cp_held || !f->fb) return 0;
    /* TWODGPU1: this tick's 2D is in the overlay target, not in the framebuffer */
    if (g_tw_live && (const uint32_t *)f->fb == g_tw_fb)
        tw_fold(f->fb, f->stride, f->settle == 1);
    const bool one = f->settle == 1;
    const bool sm = g_cp_smoothed;
    CpMaps m;
    memset(&m, 0, sizeof m);
    /* settle 1 shows the smoothed picture when there is one and keeps the plain
       one as the copy before the smoothing; settle 2 is the plain one only */
    bool ok = (!g_down && g_ctx);
    if (ok && g_fail_readback && g_frames >= g_fail_readback) {
        /* the readback drill reaches the settle too: the card cannot give the
           picture back */
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_READBACK=%d reached "
                "after %lld frame(s); forcing a readback failure on purpose\n",
                g_fail_readback, g_frames);
        ok = false;
    } else if (ok) {
        ok = one ? cp_map(m, true, sm) : cp_map(m, true, false);
    }
    if (!ok) {
        /* a card that will not hand a picture back is a card that has stopped:
           the renderer goes back to the processor for the rest of the run, and
           this one frame shows the clear colour under its 2D */
        fall_back("the picture could not be read back to settle it", 0);
        g_cp_dead = 1;      /* its own line was just said */
        /* every pixel reads as uncovered: the clear colour under the 2D */
        memset(&m, 0, sizeof m);
    }
    const int stride = f->stride;
    if (one) {
        /* the 3D alone, for SmoothMotion's snapshot (run hunt8, CARDSM1): an
           overlay of zeros says "no 2D", no fade */
        if (f->sink3d) {
            static std::vector<uint32_t> zero_row;
            if ((int)zero_row.size() < g_cp_cw) zero_row.assign((size_t)g_cp_cw, 0u);
            cp_resolve(&zero_row[0], 0, f->sink3d, stride, 0, 0, stride, false,
                       sm, m);
        }
        cp_resolve(f->fb, stride, f->fb, stride, f->cover,
                   (sm && f->pre) ? f->pre : 0, stride, g_cp_fade != 0, sm, m);
        if (f->out) f->out->pre_valid = (sm && f->pre) ? 1 : 0;
        ++g_cp_settled;
        g_cp_held = false;
    } else {
        if (!f->pre) { cp_unmap(m); return 0; }
        cp_resolve(f->fb, stride, f->pre, stride, 0, 0, stride, false, false, m);
        ++g_cp_captured;
    }
    cp_unmap(m);
    return ok ? 1 : 0;
}

/* ---- THE COMPOSITE ON THE CARD ---------------------------------------------
   One pass into a plain colour target, which the present quad then scales onto
   the window with whichever filter the player picked. The overlay is uploaded
   whole (it is one picture of 4-byte pixels); the targets it reads are the
   ones the frame was drawn into. */
bool cp_ensure(int w, int h)
{
    if (!g_ps_comp) {
        HRESULT hr = g_dev->CreatePixelShader(kGpuCompPS, sizeof kGpuCompPS, 0,
                                              &g_ps_comp);
        if (FAILED(hr)) { cp_give_up("the composite shader would not load", hr); return false; }
        D3D11_BUFFER_DESC bd;
        memset(&bd, 0, sizeof bd);
        bd.ByteWidth = 48;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = g_dev->CreateBuffer(&bd, 0, &g_comp_cb);
        if (FAILED(hr)) { cp_give_up("no composite parameters buffer", hr); return false; }
    }
    if (g_ov_tex && g_cp_w == w && g_cp_h == h) return true;
    g_comp_valid = false;
    if (g_ov_srv) { g_ov_srv->Release(); g_ov_srv = 0; }
    if (g_ov_tex) { g_ov_tex->Release(); g_ov_tex = 0; }
    if (g_bm_srv) { g_bm_srv->Release(); g_bm_srv = 0; }
    if (g_bm_tex) { g_bm_tex->Release(); g_bm_tex = 0; }
    g_tw_live = false;
    if (g_ov_rtv) { g_ov_rtv->Release(); g_ov_rtv = 0; }
    if (g_bm_rtv) { g_bm_rtv->Release(); g_bm_rtv = 0; }
    if (g_tw_up_srv) { g_tw_up_srv->Release(); g_tw_up_srv = 0; }
    if (g_tw_up_tex) { g_tw_up_tex->Release(); g_tw_up_tex = 0; }
    if (g_tw_st_ov) { g_tw_st_ov->Release(); g_tw_st_ov = 0; }
    if (g_tw_st_bm) { g_tw_st_bm->Release(); g_tw_st_bm = 0; }
    g_tw_w = g_tw_h = 0;
    if (g_comp_rtv) { g_comp_rtv->Release(); g_comp_rtv = 0; }
    if (g_comp_srv) { g_comp_srv->Release(); g_comp_srv = 0; }
    if (g_comp_tex) { g_comp_tex->Release(); g_comp_tex = 0; }
    if (g_comp_stage) { g_comp_stage->Release(); g_comp_stage = 0; }
    g_comp2_valid = false;
    if (g_comp2_rtv) { g_comp2_rtv->Release(); g_comp2_rtv = 0; }
    if (g_comp2_srv) { g_comp2_srv->Release(); g_comp2_srv = 0; }
    if (g_comp2_tex) { g_comp2_tex->Release(); g_comp2_tex = 0; }
    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = d.ArraySize = 1;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;
    d.Format = DXGI_FORMAT_R32_UINT;
    d.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;   /* TWODGPU1: also drawn into */
    HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_ov_tex);
    if (FAILED(hr)) { cp_give_up("no overlay texture", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_ov_tex, 0, &g_ov_srv);
    if (FAILED(hr)) { cp_give_up("no overlay texture view", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_ov_tex, 0, &g_ov_rtv);
    if (FAILED(hr)) { cp_give_up("no overlay texture target", hr); return false; }
    {   /* TEXTBOX2: the zero overlay (immutable, all zero words: no 2D) */
        static std::vector<uint32_t> zeros;
        zeros.assign((size_t)w * (size_t)h, 0u);
        D3D11_TEXTURE2D_DESC zd = d;
        zd.Usage = D3D11_USAGE_IMMUTABLE;
        zd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA zi;
        zi.pSysMem = &zeros[0]; zi.SysMemPitch = (UINT)w * 4u; zi.SysMemSlicePitch = 0;
        if (SUCCEEDED(g_dev->CreateTexture2D(&zd, &zi, &g_zov_tex)))
            g_dev->CreateShaderResourceView(g_zov_tex, 0, &g_zov_srv);
    }
    d.Format = DXGI_FORMAT_R8_UINT;
    d.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    hr = g_dev->CreateTexture2D(&d, 0, &g_bm_tex);
    if (FAILED(hr)) { cp_give_up("no brightness mask texture", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_bm_tex, 0, &g_bm_srv);
    if (FAILED(hr)) { cp_give_up("no brightness mask texture view", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_bm_tex, 0, &g_bm_rtv);
    if (FAILED(hr)) { cp_give_up("no brightness mask texture target", hr); return false; }
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    d.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = g_dev->CreateTexture2D(&d, 0, &g_comp_tex);
    if (FAILED(hr)) { cp_give_up("no composite target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_comp_tex, 0, &g_comp_rtv);
    if (FAILED(hr)) { cp_give_up("no composite target view", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_comp_tex, 0, &g_comp_srv);
    if (FAILED(hr)) { cp_give_up("no composite target reader", hr); return false; }
    hr = g_dev->CreateTexture2D(&d, 0, &g_comp2_tex);
    if (FAILED(hr)) { cp_give_up("no in-between composite target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_comp2_tex, 0, &g_comp2_rtv);
    if (FAILED(hr)) { cp_give_up("no in-between composite target view", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_comp2_tex, 0, &g_comp2_srv);
    if (FAILED(hr)) { cp_give_up("no in-between composite target reader", hr); return false; }
    g_cp_w = w;
    g_cp_h = h;
    return true;
}

/* THE CHECK (SM64DS_PRESENT_FROM_CARD_CHECK=N, every Nth composite): read the
   card's composite back and compare every pixel with the processor's own copy
   of the rule over the same overlay. Zero differing pixels is the claim. */
void cp_check(const uint32_t *overlay, int stride_px, ID3D11Texture2D *tex = 0)
{
    if (!g_comp_stage) {
        D3D11_TEXTURE2D_DESC d;
        memset(&d, 0, sizeof d);
        d.Width = (UINT)g_cp_w;
        d.Height = (UINT)g_cp_h;
        d.MipLevels = d.ArraySize = 1;
        d.SampleDesc.Count = 1;
        d.Usage = D3D11_USAGE_STAGING;
        d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        if (FAILED(g_dev->CreateTexture2D(&d, 0, &g_comp_stage))) return;
    }
    g_ctx->CopyResource(g_comp_stage, tex ? tex : g_comp_tex);
    D3D11_MAPPED_SUBRESOURCE cm;
    memset(&cm, 0, sizeof cm);
    if (FAILED(g_ctx->Map(g_comp_stage, 0, D3D11_MAP_READ, 0, &cm))) return;
    CpMaps m;
    if (!cp_map(m, !g_cp_smoothed, g_cp_smoothed)) { g_ctx->Unmap(g_comp_stage, 0); return; }
    std::vector<uint32_t> ref((size_t)g_cp_cw * (size_t)g_cp_ch);
    cp_resolve(overlay, stride_px, &ref[0], g_cp_cw, 0, 0, g_cp_cw,
               g_cp_fade != 0, g_cp_smoothed, m);
    cp_unmap(m);
    long long bad = 0;
    for (int y = 0; y < g_cp_ch; ++y) {
        const uint32_t *crow = (const uint32_t *)
            ((const unsigned char *)cm.pData + (size_t)y * cm.RowPitch);
        const uint32_t *rrow = &ref[(size_t)y * g_cp_cw];
        for (int x = 0; x < g_cp_cw; ++x)
            if ((crow[x] & 0x00FFFFFFu) != (rrow[x] & 0x00FFFFFFu)) ++bad;
    }
    g_ctx->Unmap(g_comp_stage, 0);
    ++g_cp_checked;
    if (g_lay_live) ++g_lay_checked;
    if (bad) {
        ++g_cp_bad_frames;
        g_cp_bad_px += bad;
        if (g_cp_bad_frames <= 5)
            fprintf(stderr, "[renderer-cpcheck] composite %lld: %lld pixel(s) "
                    "differ from the processor's copy of the rule\n",
                    g_cp_composed, bad);
    }
}

/* Composite the held frame with the overlay (the framebuffer as the processor
   left it, `stride_px` pixels to a row) into g_comp_tex and hand back its
   view, or null after cp_give_up. The processor then presents that texture
   exactly as it presents the uploaded one. */
void *cp_compose(const uint32_t *overlay, int stride_px)
{
    if (!g_cp_held || g_cp_dead || g_down || !g_ctx || !overlay) return 0;
    if (g_cp_fail_after && g_cp_composed >= g_cp_fail_after) {
        fprintf(stderr, "[renderer] SM64DS_PRESENT_FROM_CARD_FAIL_AFTER=%d "
                "reached after %lld composite(s); forcing the fallback on "
                "purpose\n", g_cp_fail_after, g_cp_composed);
        cp_give_up("a test asked for it", 0);
        return 0;
    }
    const long long t0 = qpc();
    const int w = g_cp_cw, h = g_cp_ch;
    if (!cp_ensure(w, h)) return 0;
    if (g_cp_smoothed && !g_fa_srv) { cp_give_up("no smoothing reader", 0); return 0; }

    D3D11_BOX ob;
    ob.left = 0;
    ob.right = (UINT)w;
    ob.top = 0;
    ob.bottom = (UINT)h;
    ob.front = 0;
    ob.back = 1;
    /* TWODGPU1: a tick whose 2D the card drew itself needs only the pixels the
       processor wrote on its own merged in (a different framebuffer than the
       tick's is the classic upload) */
    if (g_tw_live && overlay != g_tw_fb) g_tw_live = false;
    const bool tw_now = g_tw_live;
    if (tw_now) {
        tw_flush(overlay, stride_px);
    } else {
        g_ctx->UpdateSubresource(g_ov_tex, 0, &ob, overlay, (UINT)stride_px * 4u, 0);
        if (g_cp_mask)
            g_ctx->UpdateSubresource(g_bm_tex, 0, &ob, g_cp_mask,
                                     (UINT)g_cp_mask_stride, 0);
    }

    D3D11_MAPPED_SUBRESOURCE cm;
    memset(&cm, 0, sizeof cm);
    HRESULT hr = g_ctx->Map(g_comp_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cm);
    if (FAILED(hr) || !cm.pData) { cp_give_up("the composite parameters were refused", hr); return 0; }
    uint32_t *u = (uint32_t *)cm.pData;
    u[0] = (uint32_t)g_cp_box[0];
    u[1] = (uint32_t)g_cp_box[1];
    u[2] = (uint32_t)g_cp_box[2];
    u[3] = (uint32_t)g_cp_box[3];
    u[4] = (uint32_t)g_cp_px0;
    u[5] = (uint32_t)g_cp_py0;
    u[6] = (uint32_t)g_cp_pw;
    u[7] = (uint32_t)g_cp_ph;
    u[8] = g_cp_clear;
    u[9] = g_cp_outside;
    u[10] = (g_cp_smoothed ? 1u : 0u) | (g_cp_fade ? 2u : 0u) |
            ((g_cp_fade && g_cp_white) ? 4u : 0u) | (g_cp_mask ? 8u : 0u) |
            (g_cp_shown3d ? 16u : 0u) | (g_cp_clear_opaque ? 32u : 0u) |
            (g_lay_live ? (g_lay_post ? 128u : 64u) : 0u) |
            ((g_lay_live && g_lay_two) ? 256u : 0u);
    u[11] = (uint32_t)g_cp_evy;
    g_ctx->Unmap(g_comp_cb, 0);
    /* run hunt8, lane FADEINTERP1: the fade this tick was composited with, kept
       for the pictures drawn between ticks (cp_compose_inter) */
    g_cpt_fade = g_cp_fade;
    g_cpt_evy = g_cp_evy;
    g_cpt_white = g_cp_white;
    g_cpt_mask = g_cp_mask;
    g_cpt_mask_stride = g_cp_mask_stride;
    g_cpt_shown3d = g_cp_shown3d;
    g_cpt_clear_opaque = g_cp_clear_opaque;

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = (float)w;
    vp.Height = (float)h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_ctx->RSSetViewports(1, &vp);
    D3D11_RECT sc;
    sc.left = 0;
    sc.top = 0;
    sc.right = w;
    sc.bottom = h;
    g_ctx->RSSetScissorRects(1, &sc);
    g_ctx->RSSetState(g_rast);
    g_ctx->OMSetRenderTargets(1, &g_comp_rtv, 0);
    g_ctx->OMSetDepthStencilState(0, 0);
    g_ctx->OMSetBlendState(0, 0, 0xffffffffu);
    g_ctx->IASetInputLayout(0);
    g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_ctx->VSSetShader(g_full_vs, 0, 0);
    g_ctx->PSSetShader(g_ps_comp, 0, 0);
    g_ctx->PSSetConstantBuffers(0, 1, &g_comp_cb);
    ID3D11ShaderResourceView *srvs[6] = { g_col_srv, g_cp_smoothed ? g_fa_srv : 0,
                                          g_ov_srv, g_cp_mask ? g_bm_srv : 0,
                                          g_lay_live ? g_lay_srv : 0,
                                          (g_lay_live && g_lay_two) ? g_lay2_srv : 0 };
    g_ctx->PSSetShaderResources(0, 6, srvs);
    g_ctx->Draw(3, 0);
    ID3D11ShaderResourceView *none[6] = { 0, 0, 0, 0, 0, 0 };
    g_ctx->PSSetShaderResources(0, 6, none);
    g_ctx->OMSetRenderTargets(0, 0, 0);

    ++g_cp_composed;
    g_comp_valid = true;
    g_cp_ms += ms_between(t0, qpc());
    if (g_cp_check && ((g_cp_composed - 1) % g_cp_check) == 0 && (!tw_now || tw_check_on()))
        cp_check(overlay, stride_px);
    return g_comp_srv;
}

/* run hunt8, lane GPUINTERP1: THE PICTURE BETWEEN TWO TICKS, COMPOSITED ON THE
   CARD. The frame just drawn (kept, the replayed 3D) is composited with the
   overlay the tick's own composite uploaded (g_ov_tex still holds it: nothing
   uploads again until the next tick, so the 2D is composed once per tick) into
   the in-between target g_comp2_tex, and its view is handed back. The tick's
   engine A fade, when it held still (run hunt8, lane FADEINTERP1), is applied
   to this picture too, with the same integers; a per-pixel brightness mask is
   not replayed (a tick with one is a snap tick) and takes the processor's
   blend instead.
   Null (and nothing changed) when the tick's overlay is not there, the sizes
   moved or the card has stopped; the caller then reads the 3D back and blends
   on the processor as before. */
void *cp_compose_inter(const uint32_t *overlay, int stride_px)
{
    if (!g_cp_held || g_cp_dead || g_down || !g_ctx || !g_comp_valid ||
        !g_ov_tex || !g_comp2_tex || !g_ps_comp) return 0;
    if (g_cp_w != g_cp_cw || g_cp_h != g_cp_ch) return 0;
    if (g_cp_smoothed && !g_fa_srv) return 0;
    const long long t0 = qpc();
    D3D11_MAPPED_SUBRESOURCE cm;
    memset(&cm, 0, sizeof cm);
    HRESULT hr = g_ctx->Map(g_comp_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cm);
    if (FAILED(hr) || !cm.pData) return 0;
    uint32_t *u = (uint32_t *)cm.pData;
    u[0] = (uint32_t)g_cp_box[0];
    u[1] = (uint32_t)g_cp_box[1];
    u[2] = (uint32_t)g_cp_box[2];
    u[3] = (uint32_t)g_cp_box[3];
    u[4] = (uint32_t)g_cp_px0;
    u[5] = (uint32_t)g_cp_py0;
    u[6] = (uint32_t)g_cp_pw;
    u[7] = (uint32_t)g_cp_ph;
    u[8] = g_cp_clear;
    u[9] = g_cp_outside;
    const bool ipo = g_ipo_on != 0;
    const int  f_on = ipo ? (g_ipo_evy > 0) : g_cpt_fade;
    const int  f_evy = ipo ? g_ipo_evy : g_cpt_evy;
    const int  f_white = ipo ? g_ipo_white : g_cpt_white;
    const bool f_mask = f_on && g_cpt_fade && g_cpt_mask;   /* the tick's own mask, when it had a fade */
    u[10] = (g_cp_smoothed ? 1u : 0u) | (g_cpt_shown3d ? 16u : 0u) |
            (g_cpt_clear_opaque ? 32u : 0u) | (f_on ? 2u : 0u) |
            ((f_on && f_white) ? 4u : 0u) |
            (f_mask ? 8u : 0u) |
            (g_lay_live ? (g_lay_post ? 128u : 64u) : 0u) |
            ((g_lay_live && g_lay_two) ? 256u : 0u);
    u[11] = (uint32_t)(f_on ? f_evy : 0);
    uint32_t cbsave[12];
    for (int k = 0; k < 12; ++k) cbsave[k] = u[k];
    g_ctx->Unmap(g_comp_cb, 0);

    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = (float)g_cp_cw;
    vp.Height = (float)g_cp_ch;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_ctx->RSSetViewports(1, &vp);
    D3D11_RECT sc;
    sc.left = 0;
    sc.top = 0;
    sc.right = g_cp_cw;
    sc.bottom = g_cp_ch;
    g_ctx->RSSetScissorRects(1, &sc);
    g_ctx->RSSetState(g_rast);
    g_ctx->OMSetRenderTargets(1, &g_comp2_rtv, 0);
    g_ctx->OMSetDepthStencilState(0, 0);
    g_ctx->OMSetBlendState(0, 0, 0xffffffffu);
    g_ctx->IASetInputLayout(0);
    g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_ctx->VSSetShader(g_full_vs, 0, 0);
    g_ctx->PSSetShader(g_ps_comp, 0, 0);
    g_ctx->PSSetConstantBuffers(0, 1, &g_comp_cb);
    ID3D11ShaderResourceView *srvs[6] = { g_col_srv, g_cp_smoothed ? g_fa_srv : 0,
                                          g_ov_srv,
                                          f_mask ? g_bm_srv : 0,
                                          g_lay_live ? g_lay_srv : 0,
                                          (g_lay_live && g_lay_two) ? g_lay2_srv : 0 };
    g_ctx->PSSetShaderResources(0, 6, srvs);
    g_ctx->Draw(3, 0);
    const int cut_run = (g_cut_on && g_zov_srv && !(g_cut_closing && !f_on)) ? 1 : 0;
    if (cut_run) {
        /* TEXTBOX2: an opening dialogue box. Inside the outer rectangle and outside the inner one the picture shows the 3D alone:
           the same composite with the zero overlay, scissored to the ring's four bands */
        srvs[2] = g_zov_srv;
        /* INTERP093 I1: a fade that takes only the tick's own mask (the box's window) takes none of the ring: the ring is outside the
           window that far along, so the 3D there is unfaded (the masked fade drawn here was the full-size box in every picture) */
        if (g_cut_closing && f_on) {
            /* INTERP093 I1: a CLOSING box: the ring is the panel with no text, the fade of the tick on the 3D there with no mask */
            D3D11_MAPPED_SUBRESOURCE cm2;
            memset(&cm2, 0, sizeof cm2);
            if (SUCCEEDED(g_ctx->Map(g_comp_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cm2)) && cm2.pData) {
                uint32_t *w = (uint32_t *)cm2.pData;
                for (int k = 0; k < 12; ++k) w[k] = cbsave[k];
                w[10] |= 2u;
                w[10] &= ~8u;
                g_ctx->Unmap(g_comp_cb, 0);
                srvs[3] = 0;
            }
        } else if (g_cut_closing) {
            g_cut_on = 0;   /* no fade to draw the panel with: the tick's own picture */
        } else if (f_mask) {
            D3D11_MAPPED_SUBRESOURCE cm2;
            memset(&cm2, 0, sizeof cm2);
            if (SUCCEEDED(g_ctx->Map(g_comp_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cm2)) && cm2.pData) {
                uint32_t *w = (uint32_t *)cm2.pData;
                for (int k = 0; k < 12; ++k) w[k] = cbsave[k];
                w[10] &= ~(2u | 4u | 8u);
                w[11] = 0;
                g_ctx->Unmap(g_comp_cb, 0);
                srvs[3] = 0;
            }
        }
        g_ctx->PSSetShaderResources(0, 6, srvs);
        const int *o = g_cut_o, *n = g_cut_i;
        auto band = [&](int x1, int y1, int x2, int y2) {
            if (x1 < 0) x1 = 0;
            if (y1 < 0) y1 = 0;
            if (x2 > g_cp_cw) x2 = g_cp_cw;
            if (y2 > g_cp_ch) y2 = g_cp_ch;
            if (x2 <= x1 || y2 <= y1) return;
            D3D11_RECT r; r.left = x1; r.top = y1; r.right = x2; r.bottom = y2;
            g_ctx->RSSetScissorRects(1, &r);
            g_ctx->Draw(3, 0);
        };
        band(o[0], o[1], o[2], n[1]);        /* above the inner rectangle */
        band(o[0], n[3], o[2], o[3]);        /* below */
        band(o[0], n[1], n[0], n[3]);        /* left */
        band(n[2], n[1], o[2], n[3]);        /* right */
        g_ctx->RSSetScissorRects(1, &sc);
    }
    ID3D11ShaderResourceView *none[6] = { 0, 0, 0, 0, 0, 0 };
    g_ctx->PSSetShaderResources(0, 6, none);
    g_ctx->OMSetRenderTargets(0, 0, 0);
    /* the picture now carries the tick's fade: a later settle or the check
       resolves it the same way (set only now, so a refusal above leaves the
       processor's blend to apply the fade itself) */
    g_cp_fade = g_cpt_fade;
    g_cp_evy = g_cpt_evy;
    g_cp_white = g_cpt_white;
    g_cp_mask = g_cpt_fade ? g_cpt_mask : 0;
    g_cp_mask_stride = g_cpt_mask_stride;
    g_cp_shown3d = g_cpt_shown3d;
    g_cp_clear_opaque = g_cpt_clear_opaque;
    ++g_cp_composed;
    if (g_lay_live) ++g_lay_inter;
    g_comp2_valid = true;
    g_cp_ms += ms_between(t0, qpc());
    if (g_cp_check && overlay && (g_cp_composed % g_cp_check) == 0 &&
        (!g_tw_live || tw_check_on())) {
        /* a picture with a fade in motion is checked against the rule at its
           own coefficient, under the tick's mask when the tick had a fade */
        const int sf = g_cp_fade, se = g_cp_evy, sw = g_cp_white;
        const uint8_t *sm = g_cp_mask;
        if (ipo) {
            g_cp_fade = g_ipo_evy > 0; g_cp_evy = g_ipo_evy;
            g_cp_white = g_ipo_white; if (!g_cpt_fade) g_cp_mask = 0;
        }
        cp_check(overlay, stride_px, g_comp2_tex);
        g_cp_fade = sf; g_cp_evy = se; g_cp_white = sw; g_cp_mask = sm;
    }
    return g_comp2_srv;
}

/* ---- TWODGPU1 (run hunt8, lane TWODGPU1): ENGINE A'S 2D COMPOSED ON THE CARD
   See the banner in gpu_raster.h and the section of the same name in
   gpu_raster.hlsl. The picture-sized loops of hal/message_compositor.cpp (the
   3D layer's window hide, the brightness mask's default pass, the placement of
   every 2D cell) are passes into g_ov_tex / g_bm_tex here, with the same
   integer rules; the processor keeps the 256x192 cell plane and writes only the
   pixels it always wrote itself (the inset panel), which tw_flush merges. Any
   failure here is one line and the route stays off for the rest of the run
   (the classic loops then run, exactly as before). */
bool tw_env_on()
{
    if (g_tw_env < 0) {
        const char *e = getenv("SM64DS_TWOD_CARD");
        g_tw_env = (e && *e == '0') ? 0 : 1;
    }
    return g_tw_env != 0;
}

bool tw_check_on()
{
    if (g_tw_check_on < 0) {
        const char *e = getenv("SM64DS_TWOD_CARD_CHECK");
        g_tw_check_on = (e && *e && *e != '0') ? 1 : 0;
    }
    return g_tw_check_on != 0;
}

void tw_fail(const char *why, HRESULT hr)
{
    fprintf(stderr, "[renderer] 2D on the card: %s (0x%08lx); engine A's 2D goes back "
            "to the processor for the rest of the run.\n", why, (unsigned long)hr);
    g_tw_env = 0;
    g_tw_live = false;
}

enum { TW_MAX_CELLS = 256 * 192 };

bool tw_ensure(int w, int h)
{
    if (!g_tw_ps_hide) {
        HRESULT hr = g_dev->CreatePixelShader(kGpuTwHidePS, sizeof kGpuTwHidePS, 0, &g_tw_ps_hide);
        if (SUCCEEDED(hr)) hr = g_dev->CreatePixelShader(kGpuTwBmdefPS, sizeof kGpuTwBmdefPS, 0, &g_tw_ps_bmdef);
        if (SUCCEEDED(hr)) hr = g_dev->CreatePixelShader(kGpuTwRectPS, sizeof kGpuTwRectPS, 0, &g_tw_ps_rect);
        if (SUCCEEDED(hr)) hr = g_dev->CreatePixelShader(kGpuTwInsetPS, sizeof kGpuTwInsetPS, 0, &g_tw_ps_inset);
        if (SUCCEEDED(hr)) hr = g_dev->CreatePixelShader(kGpuTwBoxupPS, sizeof kGpuTwBoxupPS, 0, &g_tw_ps_boxup);
        if (SUCCEEDED(hr)) hr = g_dev->CreateVertexShader(kGpuTwRectVS, sizeof kGpuTwRectVS, 0, &g_tw_vs_rect);
        if (SUCCEEDED(hr)) hr = g_dev->CreateVertexShader(kGpuTwBoxVS, sizeof kGpuTwBoxVS, 0, &g_tw_vs_box);
        if (FAILED(hr)) { tw_fail("the shaders would not load", hr); return false; }
        D3D11_BUFFER_DESC bd;
        memset(&bd, 0, sizeof bd);
        bd.ByteWidth = 64;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = g_dev->CreateBuffer(&bd, 0, &g_tw_cb);
        if (FAILED(hr)) { tw_fail("no parameters buffer", hr); return false; }
        memset(&bd, 0, sizeof bd);
        bd.ByteWidth = TW_MAX_CELLS * 16;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = g_dev->CreateBuffer(&bd, 0, &g_tw_inst_buf);
        if (FAILED(hr)) { tw_fail("no cell buffer", hr); return false; }
        D3D11_SHADER_RESOURCE_VIEW_DESC sv;
        memset(&sv, 0, sizeof sv);
        sv.Format = DXGI_FORMAT_R32G32B32A32_UINT;
        sv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        sv.Buffer.FirstElement = 0;
        sv.Buffer.NumElements = TW_MAX_CELLS;
        hr = g_dev->CreateShaderResourceView(g_tw_inst_buf, &sv, &g_tw_inst_srv);
        if (FAILED(hr)) { tw_fail("no cell buffer view", hr); return false; }
        D3D11_TEXTURE2D_DESC d;
        memset(&d, 0, sizeof d);
        d.Width = 256;
        d.Height = 192;
        d.MipLevels = d.ArraySize = 1;
        d.SampleDesc.Count = 1;
        d.Usage = D3D11_USAGE_DEFAULT;
        d.Format = DXGI_FORMAT_R8_UINT;
        d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        hr = g_dev->CreateTexture2D(&d, 0, &g_tw_wm_tex);
        if (SUCCEEDED(hr)) hr = g_dev->CreateShaderResourceView(g_tw_wm_tex, 0, &g_tw_wm_srv);
        if (FAILED(hr)) { tw_fail("no window plane", hr); return false; }
    }
    if (!g_tw_up_tex || g_tw_w != w || g_tw_h != h) {
        if (g_tw_up_srv) { g_tw_up_srv->Release(); g_tw_up_srv = 0; }
        if (g_tw_up_tex) { g_tw_up_tex->Release(); g_tw_up_tex = 0; }
        D3D11_TEXTURE2D_DESC d;
        memset(&d, 0, sizeof d);
        d.Width = (UINT)w;
        d.Height = (UINT)h;
        d.MipLevels = d.ArraySize = 1;
        d.SampleDesc.Count = 1;
        d.Usage = D3D11_USAGE_DEFAULT;
        d.Format = DXGI_FORMAT_R32_UINT;
        d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_tw_up_tex);
        if (SUCCEEDED(hr)) hr = g_dev->CreateShaderResourceView(g_tw_up_tex, 0, &g_tw_up_srv);
        if (FAILED(hr)) { tw_fail("no merge texture", hr); return false; }
        g_tw_w = w;
        g_tw_h = h;
    }
    return true;
}

/* the parameters of every pass of the tick (box = the box passes' rectangle) */
bool tw_cb(int bx0, int by0, int bx1, int by1)
{
    D3D11_MAPPED_SUBRESOURCE cm;
    memset(&cm, 0, sizeof cm);
    HRESULT hr = g_ctx->Map(g_tw_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cm);
    if (FAILED(hr) || !cm.pData) { tw_fail("the parameters were refused", hr); return false; }
    uint32_t *u = (uint32_t *)cm.pData;
    const PortTwodFrame &p = g_tw_p;
    u[0] = (uint32_t)p.active_w;
    u[1] = (uint32_t)p.active_h;
    u[2] = (uint32_t)(p.present_h > 0 ? p.present_h : 1);
    u[3] = (p.widescreen ? 1u : 0u) | (p.wm ? 0u : 4u);
    u[4] = (uint32_t)(p.sx > 0 ? p.sx : 1);
    u[5] = (uint32_t)p.half_margin;
    u[6] = p.backdrop;
    u[7] = (uint32_t)p.out_mask;
    u[8] = (uint32_t)g_cp_w;
    u[9] = (uint32_t)g_cp_h;
    u[10] = (uint32_t)p.first;
    u[11] = 0;
    u[12] = (uint32_t)bx0;
    u[13] = (uint32_t)by0;
    u[14] = (uint32_t)bx1;
    u[15] = (uint32_t)by1;
    g_ctx->Unmap(g_tw_cb, 0);
    return true;
}

/* the state every pass of this section shares: the whole target as viewport, a
   scissor of the live picture, no blend, no depth, the parameters on b0 */
void tw_state(int n_rtv, ID3D11RenderTargetView *r0, ID3D11RenderTargetView *r1,
              int sc_w, int sc_h, D3D11_PRIMITIVE_TOPOLOGY topo)
{
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = (float)g_cp_w;
    vp.Height = (float)g_cp_h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_ctx->RSSetViewports(1, &vp);
    D3D11_RECT sc;
    sc.left = 0;
    sc.top = 0;
    sc.right = sc_w < g_cp_w ? sc_w : g_cp_w;
    sc.bottom = sc_h < g_cp_h ? sc_h : g_cp_h;
    g_ctx->RSSetScissorRects(1, &sc);
    g_ctx->RSSetState(g_rast);
    ID3D11RenderTargetView *rt[2] = { r0, r1 };
    g_ctx->OMSetRenderTargets((UINT)n_rtv, rt, 0);
    g_ctx->OMSetDepthStencilState(0, 0);
    g_ctx->OMSetBlendState(0, 0, 0xffffffffu);
    g_ctx->IASetInputLayout(0);
    g_ctx->IASetPrimitiveTopology(topo);
    g_ctx->VSSetConstantBuffers(0, 1, &g_tw_cb);
    g_ctx->PSSetConstantBuffers(0, 1, &g_tw_cb);
}

void tw_unbind()
{
    ID3D11ShaderResourceView *none[3] = { 0, 0, 0 };
    g_ctx->PSSetShaderResources(4, 3, none);
    g_ctx->VSSetShaderResources(4, 1, none);
    g_ctx->OMSetRenderTargets(0, 0, 0);
}

/* the processor's pixels written into `ov` over the card's overlay: the box of
   non-zero words, uploaded and drawn with zero alpha discarded (a held tick has
   no processor write of alpha 0: whatever settles a frame first folds it, see
   tw_fold) */
void tw_flush(const uint32_t *ov, int stride_px)
{
    const long long t0 = qpc();
    const int w = g_cp_w, h = g_cp_h;
    int bx0 = w, bx1 = 0, by0 = h, by1 = 0;
    for (int y = 0; y < h; ++y) {
        const uint32_t *row = ov + (size_t)y * stride_px;
        int x = 0;
        while (x + 8 <= w) {
            if (row[x] | row[x + 1] | row[x + 2] | row[x + 3] |
                row[x + 4] | row[x + 5] | row[x + 6] | row[x + 7]) break;
            x += 8;
        }
        while (x < w && !row[x]) ++x;
        if (x >= w) continue;
        const int first = x;
        int last = w - 1;
        while (last - 8 >= first) {
            if (row[last] | row[last - 1] | row[last - 2] | row[last - 3] |
                row[last - 4] | row[last - 5] | row[last - 6] | row[last - 7]) break;
            last -= 8;
        }
        while (last > first && !row[last]) --last;
        if (first < bx0) bx0 = first;
        if (last + 1 > bx1) bx1 = last + 1;
        if (y < by0) by0 = y;
        by1 = y + 1;
    }
    ++g_tw_flushes;
    if (bx1 <= bx0 || by1 <= by0) { g_tw_flush_ms += ms_between(t0, qpc()); return; }
    D3D11_BOX ub;
    ub.left = (UINT)bx0;
    ub.right = (UINT)bx1;
    ub.top = (UINT)by0;
    ub.bottom = (UINT)by1;
    ub.front = 0;
    ub.back = 1;
    g_ctx->UpdateSubresource(g_tw_up_tex, 0, &ub, ov + (size_t)by0 * stride_px + bx0,
                             (UINT)stride_px * 4u, 0);
    if (!tw_cb(bx0, by0, bx1, by1)) return;
    tw_state(1, g_ov_rtv, 0, g_cp_w, g_cp_h, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    g_ctx->VSSetShader(g_tw_vs_box, 0, 0);
    g_ctx->PSSetShader(g_tw_ps_boxup, 0, 0);
    ID3D11ShaderResourceView *srv[3] = { 0, 0, g_tw_up_srv };
    g_ctx->PSSetShaderResources(4, 3, srv);
    g_ctx->Draw(4, 0);
    tw_unbind();
    g_tw_flush_ms += ms_between(t0, qpc());
}

bool tw_stage()
{
    if (g_tw_st_ov && g_tw_st_bm) return true;
    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)g_cp_w;
    d.Height = (UINT)g_cp_h;
    d.MipLevels = d.ArraySize = 1;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_STAGING;
    d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    d.Format = DXGI_FORMAT_R32_UINT;
    if (!g_tw_st_ov && FAILED(g_dev->CreateTexture2D(&d, 0, &g_tw_st_ov))) return false;
    d.Format = DXGI_FORMAT_R8_UINT;
    if (!g_tw_st_bm && FAILED(g_dev->CreateTexture2D(&d, 0, &g_tw_st_bm))) return false;
    return true;
}

/* the overlay (and mask) of the tick read back into the processor's buffers:
   `fb` keeps every word it holds, the card's fills the rest. finish = the frame
   is an ordinary one from here. */
void tw_fold(uint32_t *fb, int stride, bool finish)
{
    if (!g_tw_live || !g_ctx || !g_ov_tex || !tw_stage()) return;
    const int w = g_cp_w, h = g_cp_h;
    g_ctx->CopyResource(g_tw_st_ov, g_ov_tex);
    D3D11_MAPPED_SUBRESOURCE cm;
    memset(&cm, 0, sizeof cm);
    if (FAILED(g_ctx->Map(g_tw_st_ov, 0, D3D11_MAP_READ, 0, &cm))) return;
    for (int y = 0; y < h; ++y) {
        const uint32_t *src = (const uint32_t *)((const unsigned char *)cm.pData + (size_t)y * cm.RowPitch);
        uint32_t *dst = fb + (size_t)y * stride;
        for (int x = 0; x < w; ++x)
            if (!dst[x]) dst[x] = src[x];
    }
    g_ctx->Unmap(g_tw_st_ov, 0);
    if (g_tw_p.bm_on && g_tw_cpu_mask) {
        g_ctx->CopyResource(g_tw_st_bm, g_bm_tex);
        memset(&cm, 0, sizeof cm);
        if (SUCCEEDED(g_ctx->Map(g_tw_st_bm, 0, D3D11_MAP_READ, 0, &cm))) {
            for (int y = 0; y < h; ++y)
                memcpy(g_tw_cpu_mask + (size_t)y * g_tw_cpu_mask_stride,
                       (const unsigned char *)cm.pData + (size_t)y * cm.RowPitch, (size_t)w);
            g_ctx->Unmap(g_tw_st_bm, 0);
        }
    }
    ++g_tw_folds;
    if (finish) g_tw_live = false;
}

/* The one-way latches as they stood at the last frame drawn before the card
   was lost, so a latch the dying card tripped is undone when it comes back
   and one a working card tripped earlier stays. */
int g_latch_cp, g_latch_aa, g_latch_grid, g_latch_full;

/* Everything this file made on the lost device is ABANDONED, not released
   (the rule hal/gpu_present.cpp keeps for its own: a release mid-run is what
   crashed the AMD driver's worker thread), every size and "made" flag that
   says it exists is cleared, and start() makes the device's pipeline again
   the way the first frame did; every other object is made again by its own
   ensure_* on first use, as at boot. */
void card_return()
{
    g_renewed = 0;
    if (g_lp_n) { g_renewed = 1; return; }     /* the pending late readbacks finish first */
    g_dev = 0; g_ctx = 0; g_vs = 0; g_ps = 0; g_ps_filt = 0; g_layout = 0; g_rast = 0; g_dss = 0;
    g_dss_w = 0; g_blend = 0; g_vb = 0; g_full_vs = 0; g_ps_tl = 0; g_ps_tlop = 0; g_ps_shadow = 0;
    g_ps_faa = 0; g_ps_tl_f = 0; g_ps_tlop_f = 0; g_ps_shadow_f = 0;
    memset(g_dss_v, 0, sizeof g_dss_v); memset(g_dss_tl, 0, sizeof g_dss_tl);
    memset(g_dss_tlop, 0, sizeof g_dss_tlop); memset(g_dss_mask, 0, sizeof g_dss_mask);
    memset(g_dss_shadow, 0, sizeof g_dss_shadow); g_dss_clrm = 0;
    memset(g_dss_gz, 0, sizeof g_dss_gz); g_dss_gc = 0; g_blend_full = 0; g_toon_cb = 0;
    memset(g_smp, 0, sizeof g_smp); memset(g_psmp, 0, sizeof g_psmp); g_white = 0; g_white_srv = 0;
    g_col = 0; g_col_stage = 0; g_col_rtv = 0; g_id = 0; g_id_stage = 0; g_id_rtv = 0; g_dep = 0;
    g_dep_stage = 0; g_dep_dsv = 0; g_col_srv = 0; g_id_srv = 0; g_fa_out = 0; g_fa_st = 0;
    g_fa_rtv = 0; g_fa_srv = 0; g_ov_tex = 0; g_comp_tex = 0; g_comp_stage = 0; g_bm_tex = 0;
    g_ov_srv = 0; g_comp_srv = 0; g_bm_srv = 0; g_zov_tex = 0; g_zov_srv = 0; g_comp_rtv = 0;
    g_comp2_tex = 0; g_comp2_srv = 0; g_comp2_rtv = 0; g_comp_cb = 0; g_ps_comp = 0; g_ov_rtv = 0;
    g_bm_rtv = 0; g_tw_wm_tex = 0; g_tw_up_tex = 0; g_tw_st_ov = 0; g_tw_st_bm = 0;
    g_tw_wm_srv = 0; g_tw_up_srv = 0; g_tw_inst_srv = 0; g_tw_inst_buf = 0; g_tw_cb = 0;
    g_tw_ps_hide = 0; g_tw_ps_bmdef = 0; g_tw_ps_rect = 0; g_tw_ps_inset = 0; g_tw_ps_boxup = 0;
    g_tw_vs_rect = 0; g_tw_vs_box = 0; g_lay_tex = 0; g_lay2_tex = 0; g_lay_srv = 0;
    g_lay2_srv = 0; g_aa_vs = 0; g_aa_ps = 0; g_aa_in = 0; g_aa_cv = 0; g_aa_out = 0; g_aa_st = 0;
    g_aa_in_srv = 0; g_aa_cv_srv = 0; g_aa_rtv = 0; g_vs_grid = 0; g_gi_buf = 0; g_gi_srv = 0;
    g_gp_buf = 0; g_gp_srv = 0; g_ga_buf = 0; g_ga_srv = 0; g_gt_abc = 0; g_gt_idx = 0;
    g_gt_abc_srv = 0; g_gt_idx_srv = 0; g_gcb_frame = 0; g_gcb_draw = 0;
    memset(g_fa_q, 0, sizeof g_fa_q);
    g_vb_verts = 0;
    g_full_ready = false;
    g_toon_valid = false;
    g_rt_w = g_rt_h = 0;
    g_rt_full = false;
    g_aa_w = g_aa_h = 0;
    g_fa_q_w = g_fa_q_h = 0;
    g_cp_w = g_cp_h = 0;
    g_tw_w = g_tw_h = 0;
    g_lay_w = g_lay_h = 0;
    g_lay2_tex_dirty = false;
    memset(g_lay_tex_box, 0, sizeof g_lay_tex_box);
    g_grid_ready = false;
    g_gi_cap = 0;
    g_gp_cap = g_gp_have = 0;
    g_gp_gen = 0;
    g_ga_cap = 0;
    g_tex.clear();
    g_tex_bytes = 0;
    g_comp_valid = g_comp2_valid = false;
    g_cp_held = false;
    g_last_full = false;
    g_tw_live = g_tw_bm = false;
    g_lay_live = false;
    g_cp_dead = g_latch_cp;
    g_aa_dead = g_latch_aa;
    g_grid_dead = g_latch_grid;
    g_full_dead = g_latch_full;
    g_down = 0;
    g_down_lost = 0;
    if (start()) {
        g_return_fails = 0;
        ++g_returns;
        fprintf(stderr, "[renderer] the graphics card draws the 3D picture again after the "
                "lost card (%d time(s) this run)\n", g_returns);
        return;
    }
    if (++g_return_fails < 2) {
        g_down_lost = 1;            /* one more try on the next frame */
        g_renewed = 1;
        return;
    }
    fprintf(stderr, "[renderer] the graphics card would not take the 3D picture back "
            "(%d tries in a row); it stays on the processor for this run\n", g_return_fails);
}

int draw_frame(const ntr::GxGpuFrame *f)
{
    if (!f) return 0;
    if (g_down && g_down_lost && g_renewed) card_return();
    if (!g_down) {
        g_latch_cp = g_cp_dead;
        g_latch_aa = g_aa_dead;
        g_latch_grid = g_grid_dead;
        g_latch_full = g_full_dead;
    }
    /* A SETTLE IS ANSWERED EVEN WITH THE RENDERER DOWN: the frame in hand is
       held on the card and card_settle knows how to leave the clear colour
       under the 2D when the card has gone (run cardfull, CARD2 review F1). */
    if (f->settle) return card_settle(f);
    if (f->finish) {                    /* MENU2: finish the oldest pending late readback (2 only reports) */
        if (f->finish == 1) late_finish_one();
        return g_lp_n;
    }
    if (g_down) return 0;
    /* THE PROBE (ntr/gx.h): would a frame offered whole be drawn whole? Costs
       three tests and touches nothing. 3 is "and held on the card". */
    if (f->probe) {
        if (!(g_full_on && !g_full_dead && f->full_ok == 2)) return 0;
        if (f->probe_grids && !grid_supported()) return 0;
        return (f->keep && cp_keep_ok()) ? 3 : 2;
    }
    if (f->planes) return fill_planes(f);
    g_last_full = false;                /* any other frame replaces that one */
    g_cp_held = false;
    if (!start()) return 0;
    if (f->cw <= 0 || f->ch <= 0 || f->pw <= 0 || f->ph <= 0) return 0;

    if (g_fail_after && g_frames >= g_fail_after) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_AFTER=%d reached after "
                "%lld frame(s); forcing the fallback on purpose\n",
                g_fail_after, g_frames);
        fall_back("a test asked for it", 0);
        return 0;
    }

    /* this frame's depth key (see kWDepthC): the vertex build, the clear, the
       depth state and the readback below all read these two */
    g_frame_w = f->wdepth != 0;
    g_depth_unscale = g_frame_w ? kWDepthUnscale : kDepthUnscale;

    if (f->tex_generation != g_tex_gen_seen) {
        g_tex_gen_seen = f->tex_generation;
        release_textures();
    }

    /* THE WHOLE FRAME, when it is on and has not given up (run cardfull). A -1
       means it could not start and touched nothing: this frame is then drawn
       by the opaque-only path below, as is every frame after it. */
    if (g_fail_full && !g_full_dead && g_frames >= g_fail_full) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_FULL=%d reached after "
                "%lld frame(s); forcing the whole-frame fallback on purpose\n",
                g_fail_full, g_frames);
        full_give_up("a test asked for it", 0);
    }
    /* A FRAME OFFERED WHOLE (full_ok 2) HAS SKIPPED THE CPU RASTER'S OWN
       CLEARS, so the opaque-only path below must never copy into it, whatever
       made the whole-frame mode stop (a draw that could not start, or the
       drill above latching it after the probe had already said yes). It is
       handed back undrawn: gx_render clears its buffers, reads the clear
       colour from the cleared picture and offers the frame again, and the
       probe now says no. */
    if (f->full_ok == 2 && (!g_full_on || g_full_dead)) return 0;
    if (g_full_on && !g_full_dead && f->full_ok) {
        const int r = draw_full(f);
        if (r >= 0) return r;
        /* it could not start. A frame offered with full_ok 2 has skipped the
           CPU raster's own clears, so the opaque-only path below must not
           copy into them: this one frame is handed back undrawn, and the next
           one is not offered whole (the probe now says no). */
        if (f->full_ok == 2) return 0;
    }

    if (!ensure_targets(f->cw, f->ch, false)) return 0;

    const long long t0 = qpc();

    /* ---- build the batches, in submission order ---------------------------
       Consecutive triangles that share a texture and a wrap mode go into one
       draw. The order within the list is the order the game submitted them,
       which is the order ntr/gx.cpp draws them, and the depth test settles the
       rest. */
    g_scratch.clear();
    g_batch.clear();
    const GTex *cur_tex = 0;
    int cur_au = -1, cur_av = -1;
    bool open = false;

    const float inv_cw = 2.0f / (float)f->cw;
    const float inv_ch = 2.0f / (float)f->ch;

    /* THE SCREEN BOX OF EVERY TRIANGLE THE CARD WILL DRAW. A pixel outside it
       cannot be covered, so the readback below only copies and scans the box:
       on a course that is the whole picture, around a menu's or a minigame's
       model it is often a small part of it. A coordinate that is not a finite
       number widens the box to the whole present rectangle rather than
       trusting it. */
    float bminx = 1e30f, bminy = 1e30f, bmaxx = -1e30f, bmaxy = -1e30f;
    bool bwide = false;

    for (size_t i = 0; i < f->count; ++i) {
        const ntr::GxTriangle &t = f->tris[i];
        if (t.translucent) continue;          /* the second pass' business */
        if (t.mode == 3) continue;            /* a shadow volume, likewise */

        const ntr::GxVertex &a = t.v[0], &b = t.v[1], &c = t.v[2];
        /* THE SAME CULL TEST, IN THE SAME ORDER, as ntr/gx.cpp's band loop. */
        const float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (area < 1e-6f && area > -1e-6f) continue;
        const bool backface = area > 0.0f;
        if (backface && !(t.cull & 1)) continue;
        if (!backface && !(t.cull & 2)) continue;

        for (int k = 0; k < 3; ++k) {
            const float vx = t.v[k].x, vy = t.v[k].y;
            if (!(vx > -1e20f && vx < 1e20f && vy > -1e20f && vy < 1e20f)) {
                bwide = true;
                continue;
            }
            if (vx < bminx) bminx = vx;
            if (vx > bmaxx) bmaxx = vx;
            if (vy < bminy) bminy = vy;
            if (vy > bmaxy) bmaxy = vy;
        }

        const bool textured = t.tex && t.tw > 0 && t.th > 0;
        const GTex *gt = 0;
        int au = 0, av = 0;
        if (textured) {
            gt = upload_texture(t.tex_id, t.tex, t.tw, t.th, f->tex_filter);
            if (!gt) return 0;                /* fall_back already said why */
            const bool rep_s = (t.wrap & 1) != 0, rep_t = (t.wrap & 2) != 0;
            const bool flip_s = (t.wrap & 4) != 0, flip_t = (t.wrap & 8) != 0;
            au = rep_s ? (flip_s ? 2 : 1) : 0;
            av = rep_t ? (flip_t ? 2 : 1) : 0;
        }
        if (!open || gt != cur_tex || au != cur_au || av != cur_av) {
            if (open) g_batch.back().count =
                (UINT)g_scratch.size() - g_batch.back().first;
            Batch nb;
            nb.tex = gt;
            nb.au = au;
            nb.av = av;
            nb.first = (UINT)g_scratch.size();
            nb.count = 0;
            g_batch.push_back(nb);
            cur_tex = gt;
            cur_au = au;
            cur_av = av;
            open = true;
        }

        const float tsc = (float)(t.tex_scale ? t.tex_scale : 1);
        const float iw = textured ? 1.0f / (float)t.tw : 0.0f;
        const float ih = textured ? 1.0f / (float)t.th : 0.0f;
        const float pa = (float)((t.alpha >= 31 || t.alpha == 0) ? 31u : t.alpha);
        /* the polygon ID, and above it the toon mode for a mode-2 polygon
           (1 toon, 2 highlight; the shader's attr.y is id + 64 * mode) */
        const float pid = (float)(t.polyid + (t.fog ? 256 : 0) +
            ((t.mode == 2 && f->toon_shade) ? 64 * f->toon_shade : 0));

        for (int k = 0; k < 3; ++k) {
            const ntr::GxVertex &v = t.v[k];
            GVert o;
            /* SCREEN PIXELS TO CLIP SPACE. The viewport is the whole active
               picture, so a screen x maps to 2x/cw - 1 and a screen y to
               1 - 2y/ch; multiplying through by w is what makes the card
               interpolate u and v perspective-correct and z screen-linear,
               which is the pair ntr/gx.cpp produces. */
            const float xn = (float)v.x * inv_cw - 1.0f;
            const float yn = 1.0f - (float)v.y * inv_ch;
            o.x = xn * v.w;
            o.y = yn * v.w;
            o.z = g_frame_w ? kWDepthC                 /* c / w on the card, see kWDepthC */
                            : v.z * kDepthScale * v.w;   /* see kDepthScale above */
            o.w = v.w;
            o.u = textured ? v.u * tsc * iw : 0.0f;
            o.v = textured ? v.v * tsc * ih : 0.0f;
            o.r = (float)((v.color >> 16) & 0xFF);
            o.g = (float)((v.color >> 8) & 0xFF);
            o.b = (float)(v.color & 0xFF);
            o.pa = pa;
            o.pid = pid;
            g_scratch.push_back(o);
        }
    }
    if (open)
        g_batch.back().count = (UINT)g_scratch.size() - g_batch.back().first;

    const long long t1 = qpc();

    /* ---- clear, submit ---------------------------------------------------- */

    /* A FRAME WITH NOTHING FOR THE CARD TO DRAW READS NOTHING BACK. With no
       opaque triangle no pixel is covered, and the copy below only ever
       writes covered pixels, so the buffers already hold exactly what the
       whole round trip would have left in them. The file select, the menus
       and every frame whose 3D is all translucent take this path and never
       wait for the card at all. */
    const bool nothing = g_scratch.empty();

    /* The card's own clock over the opaque pass: the clears, every draw and
       the copy out of the targets, which is all of the work the Map below
       then waits for. A timestamp pair costs an End() at each end and is
       collected a picture or two later (hal/gpu_device.h). */
    if (!nothing) port_gpu_timer_span_begin(PORT_GPU_SPAN_OPAQUE);

    /* THE CLEAR COLOUR IS THE FRAMEBUFFER'S OWN, read at the top of the frame
       rather than assumed, and the alpha is meaningless here because coverage
       travels in the ID target. It only matters at all on a filtered arm,
       where a partly transparent texel blends against it; the readback below
       copies covered pixels only, so an uncovered pixel keeps exactly the
       bytes the caller's clear left. */
    const float clr[4] = {
        (float)((f->clear_argb >> 16) & 0xFF) / 255.0f,
        (float)((f->clear_argb >> 8) & 0xFF) / 255.0f,
        (float)(f->clear_argb & 0xFF) / 255.0f,
        0.0f,
    };
    const float zero[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (!nothing) {
        g_ctx->ClearRenderTargetView(g_col_rtv, clr);
        g_ctx->ClearRenderTargetView(g_id_rtv, zero);
        g_ctx->ClearDepthStencilView(g_dep_dsv, D3D11_CLEAR_DEPTH, g_frame_w ? 0.0f : 1.0f, 0);

        if (!ensure_vb(g_scratch.size())) return 0;
        D3D11_MAPPED_SUBRESOURCE ms;
        memset(&ms, 0, sizeof ms);
        HRESULT hr = g_ctx->Map(g_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        if (FAILED(hr)) { fall_back("the vertex buffer would not open", hr); return 0; }
        memcpy(ms.pData, &g_scratch[0], g_scratch.size() * sizeof(GVert));
        g_ctx->Unmap(g_vb, 0);

        ID3D11RenderTargetView *rtvs[2] = { g_col_rtv, g_id_rtv };
        g_ctx->OMSetRenderTargets(2, rtvs, g_dep_dsv);
        D3D11_VIEWPORT vp;
        vp.TopLeftX = 0.0f;
        vp.TopLeftY = 0.0f;
        vp.Width = (float)f->cw;
        vp.Height = (float)f->ch;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        g_ctx->RSSetViewports(1, &vp);
        D3D11_RECT sc;
        sc.left = f->px0;
        sc.top = f->py0;
        sc.right = f->px0 + f->pw;
        sc.bottom = f->py0 + f->ph;
        g_ctx->RSSetScissorRects(1, &sc);
        g_ctx->RSSetState(g_rast);
        g_ctx->OMSetDepthStencilState(g_frame_w ? g_dss_w : g_dss, 0);
        const float bf[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_ctx->OMSetBlendState(g_blend, bf, 0xffffffffu);
        g_ctx->IASetInputLayout(g_layout);
        g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        const UINT stride = (UINT)sizeof(GVert), offset = 0;
        g_ctx->IASetVertexBuffers(0, 1, &g_vb, &stride, &offset);
        g_ctx->VSSetShader(g_vs, 0, 0);
        /* the filtered arms take their own pixel shader and a second, point
           sampler; filter 0 binds exactly what it always did */
        const bool filt_arm = f->tex_filter >= 1;
        g_ctx->PSSetShader(filt_arm ? g_ps_filt : g_ps, 0, 0);
        if (!upload_frame_consts(f)) return 0;
        g_ctx->PSSetConstantBuffers(0, 1, &g_toon_cb);

        for (size_t bi = 0; bi < g_batch.size(); ++bi) {
            const Batch &bt = g_batch[bi];
            if (!bt.count) continue;
            ID3D11ShaderResourceView *srv = bt.tex ? bt.tex->srv : g_white_srv;
            ID3D11SamplerState *smp =
                bt.tex ? sampler(f->tex_filter, bt.au, bt.av) : sampler(0, 0, 0);
            if (!smp) { fall_back("no sampler state", 0); return 0; }
            g_ctx->PSSetShaderResources(0, 1, &srv);
            g_ctx->PSSetSamplers(0, 1, &smp);
            if (filt_arm) {
                ID3D11SamplerState *psm = bt.tex ? psampler(bt.au, bt.av) : psampler(0, 0);
                if (!psm) { fall_back("no point sampler state", 0); return 0; }
                g_ctx->PSSetSamplers(1, 1, &psm);
            }
            g_ctx->Draw(bt.count, bt.first);
            ++g_batches;
        }
        ID3D11ShaderResourceView *none = 0;
        g_ctx->PSSetShaderResources(0, 1, &none);
        g_ctx->OMSetRenderTargets(0, 0, 0);
    }

    const long long t2 = qpc();

    /* ---- read the picture back ------------------------------------------- */

    if (g_fail_readback && g_frames >= g_fail_readback) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_FAIL_READBACK=%d reached "
                "after %lld frame(s); forcing a readback failure on purpose\n",
                g_fail_readback, g_frames);
        fall_back("a test asked the readback to fail", 0);
        return 0;
    }

    /* THE BOX TO READ: the present rectangle cut down to the triangles' own
       screen box plus a pixel of margin on every side (the card snaps a
       vertex to a 1/256 pixel grid, far inside that margin). */
    int bx0 = f->px0, by0 = f->py0;
    int bx1 = f->px0 + f->pw, by1 = f->py0 + f->ph;
    if (!nothing && !bwide) {
        if (bminx - 1.0f > (float)bx0) bx0 = (int)(bminx - 1.0f);
        if (bminy - 1.0f > (float)by0) by0 = (int)(bminy - 1.0f);
        if (bmaxx + 2.0f < (float)bx1) bx1 = (int)(bmaxx + 2.0f);
        if (bmaxy + 2.0f < (float)by1) by1 = (int)(bmaxy + 2.0f);
    }
    double wait_ms = 0.0;
    if (g_rbcheck) rbc_save(f, g_rbc_fb1, g_rbc_cv1, g_rbc_id1, g_rbc_dp1, true);
    if (!nothing && bx1 > bx0 && by1 > by0) {
        /* SM64DS_RENDERER_RBTIME=1: odd frames read back the old way (the
           whole targets, the whole present rectangle scanned one pixel at a
           time), even frames this way, in one process over the same scenes,
           so a busy machine slows both halves alike; the two averages are
           printed at exit. Both leave the same bytes (RBCHECK). */
        const int arm = (g_rbtime && (g_frames & 1)) ? 1 : 0;
        const long long ta = qpc();
        const bool ok = arm
            ? read_back(f, f->px0, f->py0, f->px0 + f->pw, f->py0 + f->ph,
                        true, &wait_ms)
            : read_back(f, bx0, by0, bx1, by1, false, &wait_ms);
        if (!ok) return 0;
        if (g_rbtime) {
            g_rbt_ms[arm] += ms_between(ta, qpc());
            g_rbt_wait[arm] += wait_ms;
            ++g_rbt_n[arm];
        }
    } else if (!nothing) {
        port_gpu_timer_span_end(PORT_GPU_SPAN_OPAQUE);
    }
    if (g_rbcheck) rbc_frame(f, !nothing);

    const long long t3 = qpc();
    ++g_frames;
    g_tris += (long long)(g_scratch.size() / 3);
    g_verts += (long long)g_scratch.size();
    g_ms_build += ms_between(t0, t1);
    g_ms_draw += ms_between(t1, t2);
    g_ms_read += ms_between(t2, t3);
    port::hitch_add(port::HK_GPUBUILD, ms_between(t0, t1));   /* HITCH9C */
    port::hitch_add(port::HK_GPUDRAW, ms_between(t1, t2));
    port::hitch_add(port::HK_GPUWAIT, wait_ms);
    port::hitch_add(port::HK_GPUCOPY, ms_between(t2, t3) - wait_ms);

    if (g_perf) {
        g_perf_build += ms_between(t0, t1);
        g_perf_draw += ms_between(t1, t2);
        g_perf_read += ms_between(t2, t3);
        g_perf_wait += wait_ms;
        g_perf_copy += ms_between(t2, t3) - wait_ms;
        g_perf_tris += (long long)(g_scratch.size() / 3);
        g_perf_batches += (long long)g_batch.size();
        if (++g_perf_n >= 30) {
            fprintf(stderr, "[renderer] build %6.3fms submit %6.3fms readback "
                    "%6.3fms (wait %6.3fms copy %6.3fms) tris %6lld batches "
                    "%4lld\n",
                    g_perf_build / g_perf_n, g_perf_draw / g_perf_n,
                    g_perf_read / g_perf_n, g_perf_wait / g_perf_n,
                    g_perf_copy / g_perf_n, g_perf_tris / g_perf_n,
                    g_perf_batches / g_perf_n);
            g_perf_build = g_perf_draw = g_perf_read = 0.0;
            g_perf_wait = g_perf_copy = 0.0;
            g_perf_tris = g_perf_batches = 0;
            g_perf_n = 0;
        }
    }
    return 1;
}

int backend(const ntr::GxGpuFrame *f) { return draw_frame(f); }

}  // namespace

extern "C" void port_gpu_raster_configure(void)
{
    if (g_started) return;
    g_started = 1;
    g_on = host_setting_renderer();
    if (!g_on) return;                 /* the whole default path stops here */

    const char *dev = getenv("SM64DS_RENDERER_DEVICE");
    if (dev && *dev) {
        if (!_stricmp(dev, "warp")) g_want_warp = 1;
        else if (!_stricmp(dev, "hw") || !_stricmp(dev, "hardware")) g_want_warp = 0;
    }
    g_perf = env_int("SM64DS_RENDERER_PERF", 0);
    g_fail_after = env_int("SM64DS_RENDERER_FAIL_AFTER", 0);
    g_fail_readback = env_int("SM64DS_RENDERER_FAIL_READBACK", 0);
    g_addrcheck = env_int("SM64DS_RENDERER_ADDRCHECK", 0);
    g_fail_device = env_int("SM64DS_RENDERER_FAIL_DEVICE", 0);
    g_rbcheck = env_int("SM64DS_RENDERER_RBCHECK", 0);
    if (g_rbcheck) atexit(rbc_report);
    g_rbtime = env_int("SM64DS_RENDERER_RBTIME", 0);
    if (g_rbtime) atexit(rbt_report);
    /* THE WHOLE-FRAME MODE IS WHAT RENDERER 1 DOES. SM64DS_RENDERER_FULL=0 is
       the escape hatch back to the opaque-only card. The two read-back test
       knobs above compare the OPAQUE readback and mean nothing here, so they
       keep the old path. */
    g_full_on = env_int("SM64DS_RENDERER_FULL", 1);
    if (g_rbcheck || g_rbtime) g_full_on = 0;
    g_fail_full = env_int("SM64DS_RENDERER_FAIL_FULL", 0);
    g_aa_on_full = env_int("SM64DS_RENDERER_AA", -1);
    /* THE PICTURE HELD ON THE CARD (run cardfull, lane CARD2) IS WHAT RENDERER 1
       WITH PRESENTBACKEND 1 DOES. SM64DS_PRESENT_FROM_CARD=0 is the escape hatch
       back to reading the picture back and presenting the processor's copy.
       See cp_keep_ok for what has to be on. */
    g_cp_on = env_int("SM64DS_PRESENT_FROM_CARD", 1);
    g_cp_fail_after = env_int("SM64DS_PRESENT_FROM_CARD_FAIL_AFTER", 0);
    g_cp_check = env_int("SM64DS_PRESENT_FROM_CARD_CHECK", 0);
#if defined(GX_HAS_GPU_AA)
    g_aa_on = env_int("SM64DS_RENDERER_AA", -1);
    g_aacheck = env_int("SM64DS_RENDERER_AACHECK", 0);
    if (g_aacheck) atexit(aac_report);
#endif
    ntr::gx_set_gpu_opaque(&backend);
#if defined(GX_HAS_GPU_AA)
    ntr::gx_set_gpu_aa(&aa_backend);
#endif
}

extern "C" int port_gpu_raster_active(void)
{
    return (g_on && !g_down) ? 1 : 0;
}

extern "C" double port_gpu_raster_readback_ms(void)
{
    return g_ms_read;
}

extern "C" void port_gpu_raster_present_down(void)
{
    /* the Direct3D present stood down (its own line says why): nothing is
       presented from the card any more, so no frame is held for it */
    g_cp_dead = 1;
}

extern "C" int port_gpu_card_held(void)
{
    return (g_cp_held && !g_cp_dead && !g_down) ? 1 : 0;
}

extern "C" void port_gpu_card_set_mask(const unsigned char *mask, int stride,
                                       int shown3d, int clear_opaque)
{
    if (!g_cp_held) return;
    g_cp_mask = (const uint8_t *)mask;
    g_cp_mask_stride = stride;
    g_cp_shown3d = shown3d ? 1 : 0;
    g_cp_clear_opaque = clear_opaque ? 1 : 0;
}

extern "C" void port_gpu_card_set_fade(int evy, int white)
{
    if (!g_cp_held) return;
    g_cp_evy = evy < 0 ? 0 : (evy > 16 ? 16 : evy);
    g_cp_white = white ? 1 : 0;
    g_cp_fade = g_cp_evy > 0 ? 1 : 0;
}

extern "C" void *port_gpu_card_compose(const unsigned int *overlay, int stride_px)
{
#ifndef PORT_ROM_CLEAN
    /* SM64DS_RENDERER_LOST_AT=<n> (developer drill, not in the kit): the nth
       composite answers as a readback on a removed card does, so the
       renderer's own lost-card path and the present path's rebuild after it
       are exercised rather than assumed (run after093, lane MGFREEZE2). */
    static int lost_at = -1, composed;
    if (lost_at < 0) {
        const char *e = getenv("SM64DS_RENDERER_LOST_AT");
        lost_at = (e && *e) ? atoi(e) : 0;
    }
    if (lost_at > 0 && !g_down && ++composed == lost_at) {
        fprintf(stderr, "[renderer] SM64DS_RENDERER_LOST_AT=%d: this composite answers as a removed card does\n", lost_at);
        fall_back("the picture could not be read back", (HRESULT)0x887A0005L);
        return 0;
    }
#endif
    return cp_compose((const uint32_t *)overlay, stride_px);
}

/* The present path's half of the lost card (see g_lost_hook): it registers
   here; it may also tell the renderer the card is gone, which puts the 3D
   picture on the processor for the rest of the run, as any lost card does. */
extern "C" void port_gpu_raster_set_lost_hook(void (*f)(long)) { g_lost_hook = f; }
/* The present path has a new device and has presented on it (MGFREEZE3). */
extern "C" void port_gpu_raster_device_renewed(void) { if (g_down_lost) g_renewed = 1; }
extern "C" void port_gpu_raster_device_lost(long hr)
{
    void (*f)(long) = g_lost_hook;
    g_lost_hook = 0;            /* the present path asked: do not call it back */
    fall_back("the graphics card was lost", (HRESULT)hr);
    g_lost_hook = f;
}

/* The finished composite of the last tick composited ahead of the pictures
   drawn between ticks (run hunt8, lane CARDSM1), or null when it is gone. */
extern "C" void *port_gpu_card_retained_view(int slot)
{
    if (slot == 2)
        return (g_comp2_valid && !g_cp_dead && !g_down && g_comp2_srv) ? (void *)g_comp2_srv : 0;
    return (g_comp_valid && !g_cp_dead && !g_down && g_comp_srv) ? (void *)g_comp_srv : 0;
}

/* run hunt8, lane GPUINTERP1: composite the frame just drawn and kept (a picture
   between two ticks) with the overlay of the tick composited ahead, into the
   in-between target; the view, or null (see cp_compose_inter). */
extern "C" void *port_gpu_card_compose_inter(const unsigned int *overlay, int stride_px)
{
    return cp_compose_inter((const uint32_t *)overlay, stride_px);
}

/* The recorded fade of the frame held now: 1 when one is waiting for the
   composite (SmoothMotion settles such a tick, its snapshots carry no fade). */
extern "C" void port_gpu_card_inter_fade(int evy, int white)
{
    g_ipo_on = 1;
    g_ipo_evy = evy < 0 ? 0 : (evy > 16 ? 16 : evy);
    g_ipo_white = white ? 1 : 0;
}

extern "C" void port_gpu_card_inter_fade_off(void)
{
    g_ipo_on = 0;
}

extern "C" void port_gpu_card_inter_cut_closing(int closing)
{
    g_cut_closing = closing ? 1 : 0;
}

extern "C" void port_gpu_card_inter_cut(const int outer[4], const int inner[4])
{
    if (!outer || !inner) { g_cut_on = 0; return; }
    for (int k = 0; k < 4; ++k) { g_cut_o[k] = outer[k]; g_cut_i[k] = inner[k]; }
    g_cut_on = 1;
}

extern "C" int port_gpu_card_fade_recorded(void)
{
    return g_cp_held && g_cp_fade ? 1 : 0;
}

/* ---- TWODGPU1: the entry points of the section above ---------------------- */
extern "C" int port_gpu_twod_ready(void)
{
    return (tw_env_on() && g_cp_held && !g_cp_dead && !g_down && g_ctx) ? 1 : 0;
}

extern "C" void port_gpu_twod_reset(void)
{
    g_tw_live = false;
    g_lay_live = false;
}

/* ---- SAVEOVL1: the menu layer's entry points (hal/gpu_raster.h) ---------- */
static bool lay_env_on()
{
    if (g_lay_env < 0) {
        const char *e = getenv("SM64DS_MENU_LAYER");
        g_lay_env = (e && *e == '0') ? 0 : 1;
    }
    return g_lay_env != 0;
}

extern "C" int port_gpu_layer_ready(void)
{
    return (lay_env_on() && port_gpu_twod_ready() && g_tw_live) ? 1 : 0;
}

/* the second plane's scratch (valid after port_gpu_layer_begin of the same tick), zero, same stride */
extern "C" unsigned *port_gpu_layer2(void)
{
    return g_lay2_cpu.empty() ? 0 : &g_lay2_cpu[0];
}

/* the scratch the recipe draws into: zero, `stride` words a row, w x h */
extern "C" unsigned *port_gpu_layer_begin(int w, int h, int stride)
{
    if (!port_gpu_layer_ready() || w < 1 || h < 1 || stride < w) return 0;
    if (g_lay_stride != stride || (int)g_lay_cpu.size() < stride * h) {
        g_lay_cpu.assign((size_t)stride * (size_t)h, 0u);
        g_lay2_cpu.assign((size_t)stride * (size_t)h, 0u);
        g_lay_stride = stride;
        g_lay_cpu_box[0] = g_lay_cpu_box[1] = g_lay_cpu_box[2] = g_lay_cpu_box[3] = 0;
    } else if (g_lay_cpu_box[3] > g_lay_cpu_box[1]) {
        for (int y = g_lay_cpu_box[1]; y < g_lay_cpu_box[3] && y < h; ++y)
        {
            memset(&g_lay_cpu[(size_t)y * (size_t)stride + (size_t)g_lay_cpu_box[0]], 0,
                   (size_t)(g_lay_cpu_box[2] - g_lay_cpu_box[0]) * 4u);
            memset(&g_lay2_cpu[(size_t)y * (size_t)stride + (size_t)g_lay_cpu_box[0]], 0,
                   (size_t)(g_lay_cpu_box[2] - g_lay_cpu_box[0]) * 4u);
        }
        g_lay_cpu_box[0] = g_lay_cpu_box[1] = g_lay_cpu_box[2] = g_lay_cpu_box[3] = 0;
    }
    return &g_lay_cpu[0];
}

/* the recipe has drawn: find what it wrote, put it on the card. 1 = the card
   holds the layer (or there is none), 0 = the card refused */
extern "C" int port_gpu_layer_end(int w, int h, int post)
{
    g_lay_live = false;
    if (g_lay_cpu.empty() || !g_ctx || !port_gpu_layer_ready()) return 0;
    if (w > g_cp_cw) w = g_cp_cw;
    if (h > g_cp_ch) h = g_cp_ch;
    int bx0 = w, bx1 = 0, by0 = h, by1 = 0;
    for (int y = 0; y < h; ++y) {
        const uint32_t *row = &g_lay_cpu[(size_t)y * (size_t)g_lay_stride];
        int x = 0;
        while (x + 8 <= w) {
            if (row[x] | row[x + 1] | row[x + 2] | row[x + 3] |
                row[x + 4] | row[x + 5] | row[x + 6] | row[x + 7]) break;
            x += 8;
        }
        while (x < w && !row[x]) ++x;
        if (x >= w) continue;
        const int first = x;
        int last = w - 1;
        while (last - 8 >= first) {
            if (row[last] | row[last - 1] | row[last - 2] | row[last - 3] |
                row[last - 4] | row[last - 5] | row[last - 6] | row[last - 7]) break;
            last -= 8;
        }
        while (last > first && !row[last]) --last;
        if (first < bx0) bx0 = first;
        if (last + 1 > bx1) bx1 = last + 1;
        if (y < by0) by0 = y;
        by1 = y + 1;
    }
    /* plane 2 words that sit on an opaque plane 1 pixel are stale (the pixel was overwritten after they were written) */
    bool two = false;
    if (bx1 > bx0 && by1 > by0) {
        for (int y = by0; y < by1; ++y) {
            const uint32_t *r1 = &g_lay_cpu[(size_t)y * (size_t)g_lay_stride];
            uint32_t *r2 = &g_lay2_cpu[(size_t)y * (size_t)g_lay_stride];
            for (int x = bx0; x < bx1; ++x) {
                if (!r2[x]) continue;
                if ((r1[x] >> 24) == 255u) r2[x] = 0; else two = true;
            }
        }
    }
    if (!g_lay_tex || g_lay_w != g_cp_cw || g_lay_h != g_cp_ch) {
        if (g_lay_srv) { g_lay_srv->Release(); g_lay_srv = 0; }
        if (g_lay_tex) { g_lay_tex->Release(); g_lay_tex = 0; }
        D3D11_TEXTURE2D_DESC d;
        memset(&d, 0, sizeof d);
        d.Width = (UINT)g_cp_cw;
        d.Height = (UINT)g_cp_ch;
        d.MipLevels = d.ArraySize = 1;
        d.SampleDesc.Count = 1;
        d.Usage = D3D11_USAGE_DEFAULT;
        d.Format = DXGI_FORMAT_R32_UINT;
        d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        if (FAILED(g_dev->CreateTexture2D(&d, 0, &g_lay_tex))) return 0;
        if (FAILED(g_dev->CreateShaderResourceView(g_lay_tex, 0, &g_lay_srv))) {
            g_lay_tex->Release(); g_lay_tex = 0;
            return 0;
        }
        g_lay_w = g_cp_cw;
        g_lay_h = g_cp_ch;
        g_lay2_tex_dirty = false;
        /* a new texture is zero only by luck: clear it whole */
        std::vector<uint32_t> z((size_t)g_cp_cw * (size_t)g_cp_ch, 0u);
        g_ctx->UpdateSubresource(g_lay_tex, 0, 0, &z[0], (UINT)g_cp_cw * 4u, 0);
        g_lay_tex_box[0] = g_lay_tex_box[1] = g_lay_tex_box[2] = g_lay_tex_box[3] = 0;
    }
    const bool any = bx1 > bx0 && by1 > by0;
    g_lay_cpu_box[0] = any ? bx0 : 0;
    g_lay_cpu_box[1] = any ? by0 : 0;
    g_lay_cpu_box[2] = any ? bx1 : 0;
    g_lay_cpu_box[3] = any ? by1 : 0;
    /* the texture is made to equal the scratch inside the union of what it held
       and what the scratch now holds (zero elsewhere in both) */
    int ux0 = g_lay_tex_box[0], uy0 = g_lay_tex_box[1], ux1 = g_lay_tex_box[2], uy1 = g_lay_tex_box[3];
    if (any) {
        if (ux1 <= ux0 || uy1 <= uy0) { ux0 = bx0; uy0 = by0; ux1 = bx1; uy1 = by1; }
        else {
            if (bx0 < ux0) ux0 = bx0;
            if (by0 < uy0) uy0 = by0;
            if (bx1 > ux1) ux1 = bx1;
            if (by1 > uy1) uy1 = by1;
        }
    }
    if (ux1 > ux0 && uy1 > uy0) {
        D3D11_BOX ub;
        ub.left = (UINT)ux0;
        ub.right = (UINT)ux1;
        ub.top = (UINT)uy0;
        ub.bottom = (UINT)uy1;
        ub.front = 0;
        ub.back = 1;
        g_ctx->UpdateSubresource(g_lay_tex, 0, &ub,
                                 &g_lay_cpu[(size_t)uy0 * (size_t)g_lay_stride + (size_t)ux0],
                                 (UINT)g_lay_stride * 4u, 0);
        g_lay_up_px += (long long)(ux1 - ux0) * (uy1 - uy0);
        /* plane 2: made when a layer first needs it, and cleaned over the same box while it holds words */
        if ((two || g_lay2_tex_dirty) && !g_lay2_tex) {
            D3D11_TEXTURE2D_DESC d2;
            memset(&d2, 0, sizeof d2);
            d2.Width = (UINT)g_cp_cw;
            d2.Height = (UINT)g_cp_ch;
            d2.MipLevels = d2.ArraySize = 1;
            d2.SampleDesc.Count = 1;
            d2.Usage = D3D11_USAGE_DEFAULT;
            d2.Format = DXGI_FORMAT_R32_UINT;
            d2.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            std::vector<uint32_t> z2((size_t)g_cp_cw * (size_t)g_cp_ch, 0u);
            D3D11_SUBRESOURCE_DATA sd;
            sd.pSysMem = &z2[0];
            sd.SysMemPitch = (UINT)g_cp_cw * 4u;
            sd.SysMemSlicePitch = 0;
            if (FAILED(g_dev->CreateTexture2D(&d2, &sd, &g_lay2_tex)) ||
                FAILED(g_dev->CreateShaderResourceView(g_lay2_tex, 0, &g_lay2_srv))) {
                if (g_lay2_srv) { g_lay2_srv->Release(); g_lay2_srv = 0; }
                if (g_lay2_tex) { g_lay2_tex->Release(); g_lay2_tex = 0; }
                return 0;
            }
        }
        if (g_lay2_tex && (two || g_lay2_tex_dirty)) {
            g_ctx->UpdateSubresource(g_lay2_tex, 0, &ub,
                                     &g_lay2_cpu[(size_t)uy0 * (size_t)g_lay_stride + (size_t)ux0],
                                     (UINT)g_lay_stride * 4u, 0);
            g_lay2_tex_dirty = two;
        }
    }
    g_lay_tex_box[0] = g_lay_cpu_box[0];
    g_lay_tex_box[1] = g_lay_cpu_box[1];
    g_lay_tex_box[2] = g_lay_cpu_box[2];
    g_lay_tex_box[3] = g_lay_cpu_box[3];
    g_lay_post = post ? 1 : 0;
    g_lay_two = two && g_lay2_tex;
    g_lay_live = any;
    if (any) ++g_lay_ticks;
    return 1;
}

extern "C" int port_gpu_twod_live(void)
{
    return g_tw_live ? 1 : 0;
}

extern "C" void port_gpu_twod_fold(unsigned *fb, int stride_px)
{
    if (fb) tw_fold((uint32_t *)fb, stride_px, true);
}

extern "C" int port_gpu_twod_begin(const PortTwodFrame *p, unsigned char *cpu_mask,
                                   int mask_stride, const unsigned *fbpx)
{
    g_tw_live = false;
    if (!p || !port_gpu_twod_ready()) return 0;
    const long long t0 = qpc();
    if (!cp_ensure(g_cp_cw, g_cp_ch)) return 0;
    if (!tw_ensure(g_cp_cw, g_cp_ch)) return 0;
    g_tw_p = *p;
    g_tw_p.wm = 0;                       /* a pointer valid for this call only */
    if (p->wm) {
        g_tw_p.wm = (const unsigned char *)1;   /* "a plane is bound" for tw_cb */
        g_ctx->UpdateSubresource(g_tw_wm_tex, 0, 0, p->wm, 256, 0);
    }
    g_tw_cpu_mask = cpu_mask;
    g_tw_cpu_mask_stride = mask_stride;
    g_tw_fb = (const uint32_t *)fbpx;
    const float zero[4] = { 0, 0, 0, 0 };
    g_ctx->ClearRenderTargetView(g_ov_rtv, zero);
    g_ctx->ClearRenderTargetView(g_bm_rtv, zero);
    if (!tw_cb(0, 0, 0, 0)) return 0;
    ID3D11ShaderResourceView *srv[3] = { 0, g_tw_wm_srv, 0 };
    if (p->hide) {
        tw_state(1, g_ov_rtv, 0, p->active_w, p->active_h, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_ctx->VSSetShader(g_full_vs, 0, 0);
        g_ctx->PSSetShader(g_tw_ps_hide, 0, 0);
        g_ctx->PSSetShaderResources(4, 3, srv);
        g_ctx->Draw(3, 0);
        tw_unbind();
    }
    if (p->bm_on) {
        tw_state(1, g_bm_rtv, 0, p->active_w, p->active_h, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g_ctx->VSSetShader(g_full_vs, 0, 0);
        g_ctx->PSSetShader(g_tw_ps_bmdef, 0, 0);
        g_ctx->PSSetShaderResources(4, 3, srv);
        g_ctx->Draw(3, 0);
        tw_unbind();
    }
    g_tw_live = true;
    ++g_tw_ticks;
    g_tw_ms += ms_between(t0, qpc());
    return 1;
}

extern "C" void port_gpu_twod_rects(const unsigned *inst, int n)
{
    if (!g_tw_live || n <= 0 || !inst) return;
    const long long t0 = qpc();
    if (n > TW_MAX_CELLS) n = TW_MAX_CELLS;
    D3D11_MAPPED_SUBRESOURCE cm;
    memset(&cm, 0, sizeof cm);
    HRESULT hr = g_ctx->Map(g_tw_inst_buf, 0, D3D11_MAP_WRITE_DISCARD, 0, &cm);
    if (FAILED(hr) || !cm.pData) { tw_fail("the cells were refused", hr); return; }
    memcpy(cm.pData, inst, (size_t)n * 16u);
    g_ctx->Unmap(g_tw_inst_buf, 0);
    if (!tw_cb(0, 0, 0, 0)) return;
    tw_state(2, g_ov_rtv, g_bm_rtv, g_tw_p.active_w, g_tw_p.active_h,
             D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    g_ctx->VSSetShader(g_tw_vs_rect, 0, 0);
    g_ctx->PSSetShader(g_tw_ps_rect, 0, 0);
    ID3D11ShaderResourceView *srv[3] = { g_tw_inst_srv, g_tw_wm_srv, 0 };
    g_ctx->VSSetShaderResources(4, 1, srv);
    g_ctx->PSSetShaderResources(4, 3, srv);
    g_ctx->DrawInstanced(4, (UINT)n, 0, 0);
    tw_unbind();
    g_tw_rects += n;
    g_tw_ms += ms_between(t0, qpc());
}

/* THE CORNER MAP IS NEVER TAKEN BY ENGINE A'S PARTIAL DIM (run hunt8, lane
   FIX092B). The panel is engine B's picture, the DS lower screen, and a message
   box or a pause dim is engine A's brightness through a mask; it never reaches
   engine B. So the panel rectangle is written as "no layer, nothing written":
   code 0, which the composite reads as not taken in every mode. A plain fade
   to black has no mask (the frame is not built with one) and still covers the
   map. */
extern "C" void port_gpu_twod_bm_rect(int x0, int y0, int x1, int y1)
{
    if (!g_tw_live || !g_bm_tex) return;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > g_cp_w) x1 = g_cp_w;
    if (y1 > g_cp_h) y1 = g_cp_h;
    if (x1 <= x0 || y1 <= y0) return;
    static std::vector<unsigned char> zero;
    const size_t need = (size_t)(x1 - x0) * (size_t)(y1 - y0);
    if (zero.size() < need) zero.assign(need, 0);
    D3D11_BOX bx;
    bx.left = (UINT)x0; bx.right = (UINT)x1;
    bx.top = (UINT)y0; bx.bottom = (UINT)y1;
    bx.front = 0; bx.back = 1;
    g_ctx->UpdateSubresource(g_bm_tex, 0, &bx, zero.data(), (UINT)(x1 - x0), 0);
}

/* what = 0 the overlay and the mask at the end of engine A's compositor, 1 the
   mask once the inset panel has been drawn (the overlay then still lacks the
   processor-written pixels) */
extern "C" void port_gpu_twod_check(const unsigned *fb, int stride_px,
                                    const unsigned char *bm, int bm_stride, int what)
{
    if (!g_tw_live || !g_ctx || !g_ov_tex || !fb || !tw_stage()) return;   /* fb: any non-null for what 1 */
    const int w = g_tw_p.active_w < g_cp_w ? g_tw_p.active_w : g_cp_w;
    const int h = g_tw_p.active_h < g_cp_h ? g_tw_p.active_h : g_cp_h;
    long long bad_ov = 0, bad_bm = 0;
    int fx = -1, fy = -1;
    unsigned fgot = 0, fwant = 0;
    D3D11_MAPPED_SUBRESOURCE cm;
    if (what == 0) {
        g_ctx->CopyResource(g_tw_st_ov, g_ov_tex);
        memset(&cm, 0, sizeof cm);
        if (FAILED(g_ctx->Map(g_tw_st_ov, 0, D3D11_MAP_READ, 0, &cm))) return;
        for (int y = 0; y < h; ++y) {
            const uint32_t *src = (const uint32_t *)((const unsigned char *)cm.pData + (size_t)y * cm.RowPitch);
            const uint32_t *ref = fb + (size_t)y * stride_px;
            for (int x = 0; x < w; ++x)
                if (src[x] != ref[x]) {
                    if (!bad_ov) { fx = x; fy = y; fgot = src[x]; fwant = ref[x]; }
                    ++bad_ov;
                }
        }
        g_ctx->Unmap(g_tw_st_ov, 0);
    }
    if (bm && g_tw_p.bm_on) {
        g_ctx->CopyResource(g_tw_st_bm, g_bm_tex);
        memset(&cm, 0, sizeof cm);
        if (SUCCEEDED(g_ctx->Map(g_tw_st_bm, 0, D3D11_MAP_READ, 0, &cm))) {
            for (int y = 0; y < h; ++y) {
                const unsigned char *src = (const unsigned char *)cm.pData + (size_t)y * cm.RowPitch;
                const unsigned char *ref = bm + (size_t)y * bm_stride;
                for (int x = 0; x < w; ++x)
                    if (src[x] != ref[x]) {
                        if (!bad_bm && !bad_ov) { fx = x; fy = y; fgot = src[x]; fwant = ref[x]; }
                        ++bad_bm;
                    }
            }
            g_ctx->Unmap(g_tw_st_bm, 0);
        }
    }
    ++g_tw_checks;
    if (bad_ov || bad_bm) {
        ++g_tw_bad;
        g_tw_bad_px_ov += bad_ov;
        g_tw_bad_px_bm += bad_bm;
        if (g_tw_bad <= 8)
            fprintf(stderr, "[renderer-twodcheck] stage %d: %lld overlay and %lld mask "
                    "pixel(s) differ from the processor's (first at %d,%d: card %08x "
                    "processor %08x)\n", what, bad_ov, bad_bm, fx, fy, fgot, fwant);
    }
}


#else   /* not Windows: the port ships on Windows and there is no card here */

#include "hal/gpu_raster.h"

void port_gpu_raster_configure(void) {}
int port_gpu_raster_active(void) { return 0; }
double port_gpu_raster_readback_ms(void) { return 0.0; }
void port_gpu_raster_present_down(void) {}
int port_gpu_card_held(void) { return 0; }
void port_gpu_card_set_fade(int, int) {}
void port_gpu_card_set_mask(const unsigned char *, int, int, int) {}
void *port_gpu_card_compose(const unsigned int *, int) { return 0; }
void *port_gpu_card_retained_view(int) { return 0; }
void *port_gpu_card_compose_inter(const unsigned int *, int) { return 0; }
int port_gpu_card_fade_recorded(void) { return 0; }
void port_gpu_card_inter_fade(int, int) {}
void port_gpu_card_inter_cut(const int *, const int *) {}
void port_gpu_card_inter_cut_closing(int) {}
void port_gpu_card_inter_fade_off(void) {}
int port_gpu_twod_ready(void) { return 0; }
int port_gpu_layer_ready(void) { return 0; }
unsigned *port_gpu_layer_begin(int, int, int) { return 0; }
unsigned *port_gpu_layer2(void) { return 0; }
int port_gpu_layer_end(int, int, int) { return 0; }
void port_gpu_twod_reset(void) {}
int port_gpu_twod_begin(const PortTwodFrame *, unsigned char *, int, const unsigned *) { return 0; }
void port_gpu_twod_rects(const unsigned *, int) {}
void port_gpu_twod_bm_rect(int, int, int, int) {}
int port_gpu_twod_live(void) { return 0; }
void port_gpu_twod_fold(unsigned *, int) {}
void port_gpu_twod_check(const unsigned *, int, const unsigned char *, int, int) {}

#endif
