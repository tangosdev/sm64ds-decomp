/* THE OPTIONAL GRAPHICS-CARD RENDERER, stage 2 of the road that started with
 * hal/gpu_present.cpp.
 *
 * WHAT IT IS. "PresentBackend" moved the finished picture to the card.
 * "Renderer" moves the DRAWING of the 3D picture to the card: the OPAQUE pass
 * of ntr/gx.cpp -- which is nearly all of the fill -- is drawn into offscreen
 * colour, depth and polygon-ID targets, read straight back into the software
 * rasteriser's own buffers, and everything after that (the translucent and
 * shadow pass, the edge smoothing, the 2D compositor, the display capture, the
 * screenshots, the present) runs on exactly the frame it always saw.
 *
 *     key "Renderer"   0 the software rasteriser, which is the default and the
 *                      only byte-exact reference; 1 Direct3D 11.
 *     env SM64DS_RENDERER overrides the key.
 *
 * WHY. The game takes one fixed step per tick and never catches up, so a frame
 * over its budget is not a dropped picture, it is slow motion. The software
 * raster costs about 20 ms of a 33.3 ms budget at RenderScale 4 in a course and
 * about 73 ms on the title screen BEFORE filtering, anti-aliasing and a
 * smoothed model's extra triangles are added. The fill is the cost, and the
 * card does fill.
 *
 * THE WHOLE FRAME (run cardfull). The translucent polygons, the shadow volumes,
 * the toon and highlight polygons and the edge smoothing can be drawn on the
 * card too, into the same targets, so that one finished 3D picture is read back
 * instead of an opaque one that the processor then finishes. It is what the
 * setting does now; env SM64DS_RENDERER_FULL=0 is the escape hatch back to the
 * opaque-only card above; a device below Direct3D feature level 11_0, or any failure of the
 * whole-frame pass, drops back to the opaque-only card for the rest of the run
 * with one line, and a failure of the card itself drops back to the software
 * rasteriser as before. The in-process A/B (SM64DS_RENDERER_AB=1) compares the
 * whole frame in this mode.
 *
 * WHAT IT IS NOT. It is not byte-identical to the software path and cannot be:
 * two rasterisers with different fill rules and different float paths tie
 * differently on edges. The software path stays the default, stays the byte
 * gate, and does not move. With the key absent nothing here loads, allocates
 * or runs.
 *
 * ON ANY FAILURE -- no device, a refused call, a device removed by the driver,
 * a readback that will not map -- one plain line is printed and the software
 * opaque pass draws that same frame and every frame after it. The game never
 * exits, never stalls and never shows a half-drawn picture because of this
 * setting.
 */
#ifndef PORT_HAL_GPU_RASTER_H
#define PORT_HAL_GPU_RASTER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Read the setting and, when it is on, hand ntr/gx.cpp the backend. Call once
 * at boot, before any geometry, beside the other configure calls: which
 * rasteriser draws the frame decides which library is loaded and which device
 * exists, and a mid-run change would be a second code path nobody tests. With
 * the key off this returns having done nothing at all. */
void port_gpu_raster_configure(void);

/* 1 when the setting is on and the card has not fallen back. Diagnostics and
 * the perf line; nothing in the render path asks. */
int port_gpu_raster_active(void);

/* THE PICTURE HELD ON THE CARD (run cardfull, lane CARD2; the long form is the
   banner above cp_keep_ok in gpu_raster.cpp). 1 when the frame being finished
   is still on the card unread, which is when the present may composite it
   there instead of uploading the processor's copy. */
int port_gpu_card_held(void);

/* The Direct3D present (hal/gpu_present.cpp) has stood down: stop holding
   frames on the card for it. Called from its own fall-back, once. */
void port_gpu_raster_present_down(void);

/* Engine A's brightness fade for the held frame (evy 0..16, white 1 = to
   white), recorded instead of applied, because the composite applies it to the
   whole picture. Ignored when no frame is held. */
void port_gpu_card_set_fade(int evy, int white);

/* The brightness mask that fade is limited by, when the frame has one
   (hal/message_compositor.cpp, in the encoding its comment gives for a frame
   held on the card): `mask` stays valid until the next frame's compositor.
   Null means the fade takes every pixel. */
void port_gpu_card_set_mask(const unsigned char *mask, int stride, int shown3d,
                            int clear_opaque);

/* Composite the held frame with the overlay (the framebuffer, `stride_px`
   pixels to a row, alpha byte = ntr/gx.h's GX_OV_* verdict) into a card target
   and return a view of it (an ID3D11ShaderResourceView as void *), or null
   after the one plain line that says why the card cannot. */
void *port_gpu_card_compose(const unsigned int *overlay, int stride_px);

/* run hunt8, lane CARDSM1: a tick composited ahead of the pictures between two
   ticks keeps its finished picture in the composite target; this is its view
   (null when it is gone). port_gpu_card_fade_recorded: the held frame has an
   engine A fade waiting for its composite. */
void *port_gpu_card_retained_view(int slot);   /* slot 1 the tick, 2 an in-between */
/* run hunt8, lane GPUINTERP1: composite the frame just drawn and kept (a picture
   between two ticks) with the retained tick's overlay into the in-between
   target and return its view, or null (the caller then blends on the
   processor). */
void *port_gpu_card_compose_inter(const unsigned int *overlay, int stride_px);
int port_gpu_card_fade_recorded(void);
/* run hunt8, lane INTERPFIX1: the pictures between two ticks take engine A's
   fade at this coefficient (0..16, 0 = none) and colour (nonzero white) instead
   of the tick's own, the whole picture; _off puts the tick's own back. */
void port_gpu_card_inter_fade(int evy, int white);
/* run after093, lane PAINTFADE1: the same with the coefficient in 1/256 steps (0..4096) */
void port_gpu_card_inter_fade256(int e256, int white);
/* PAINTFADE1: the tick's own fade in 1/256 steps (the fine arm, hal/message_compositor.cpp port_engine_a_fade) */
void port_gpu_card_set_fade256(int e256, int white);
void port_gpu_card_inter_fade_off(void);
/* TEXTBOX2: the next picture between two ticks shows the 3D alone inside the outer rectangle (x1 y1 x2 y2, host pixels) and outside the
   inner one: an opening dialogue box. Null arguments clear it. */
void port_gpu_card_inter_cut(const int outer[4], const int inner[4]);
/* INTERP093 I1: the cut is a CLOSING box's (the ring is the faded 3D, no text) when 1 */
void port_gpu_card_inter_cut_closing(int closing);
/* METERGLIDE1: the next picture between two ticks moves n (0..3) HUD rectangles on the card: rects and zones n x {x1 y1 x2 y2} host
   pixels (the zone is cleared to the 3D first), dx / dy the move; n 0 clears it. _pics: pictures it moved, for the trace. */
void port_gpu_card_inter_glide(int n, const int *rects, const int *zones, const int *dx, const int *dy, const int *band);
void port_gpu_card_inter_tick(unsigned serial);
long long port_gpu_card_glide_pics(void);

/* THE READBACK WAIT, summed in milliseconds over every frame the card has
 * drawn. This is the CPU side of the opaque pass: the copy out of the render
 * targets, the Map -- which is synchronous and therefore waits for the card
 * to finish -- and the copy of the covered pixels into the software
 * rasteriser's buffers. It is the same quantity the [renderer] cost line
 * calls "readback", and it is the cost the next stage of this road removes,
 * which is why the overlay shows it beside the card's own time rather than
 * folded into it. 0.0 with the setting off. */
double port_gpu_raster_readback_ms(void);

/* ---- ENGINE A'S 2D ON THE CARD (run hunt8, lane TWODGPU1) --------------------
 * On a tick the card holds (port_gpu_card_held), the processor still rasterises
 * engine A's DS-space 256x192 cell plane (hal/message_compositor.cpp); the host
 * resolution half of that unit -- the window hide of the 3D layer, the
 * brightness mask's default pass and the placement of every cell on the
 * picture -- is done here, into the overlay and mask targets comp_ps reads, so
 * nothing the size of the picture is written or uploaded by the processor. The
 * overlay target is an R32_UINT texture of the picture's size (0xAARRGGBB,
 * alpha = ntr/gx.h's GX_OV_* verdict), the mask target an R8_UINT texture of
 * the same size (hal/message_compositor.cpp's card encoding). SM64DS_TWOD_CARD=0
 * turns the whole route off; SM64DS_TWOD_CARD_CHECK=1 runs the processor's own
 * host loops beside it and compares the two. */
typedef struct PortTwodFrame {
    int active_w, active_h, present_h;
    int widescreen;          /* 1 when the 16:9 reanchor mapping is in force */
    int sx;                  /* active_w / 256 */
    int half_margin;         /* margin / 2 */
    unsigned backdrop;       /* BG palette entry 0, 0xFFRRGGBB */
    int out_mask;            /* window mask outside the DS panel (WINOUT, or 0x3F with no window) */
    int first;               /* BLDCNT & 0x3F */
    const unsigned char *wm; /* window mask of every DS cell (192 rows of 256), null when all are out_mask */
    int hide;                /* run the 3D layer's window hide */
    int bm_on;               /* run the brightness mask's default pass */
} PortTwodFrame;
/* 1 when the route is on and a held frame can take it this tick */
int port_gpu_twod_ready(void);
/* forget the previous tick's overlay (a new tick is starting) */
void port_gpu_twod_reset(void);
/* start the tick's overlay: clear the targets, run the hide and default passes.
   `fbpx` is the framebuffer the tick's other 2D (the inset panel) is drawn into
   on the processor; `cpu_mask` (SCREEN_W stride) is where a settle puts the
   mask. 1 when the card has the tick's 2D, 0 when nothing was drawn. */
int port_gpu_twod_begin(const PortTwodFrame *p, unsigned char *cpu_mask,
                        int mask_stride, const unsigned *fbpx);
/* n cells, four words each (x0 | y0 << 16, w | h << 16, colour | verdict << 24,
   bit 0 = the brightness mask takes it), drawn in order */
void port_gpu_twod_rects(const unsigned *inst, int n);
/* the inset panel's one brightness verdict over a host rectangle */
void port_gpu_twod_bm_rect(int x0, int y0, int x1, int y1);
/* the tick's overlay is on the card (processor-written pixels still to merge) */
int port_gpu_twod_live(void);
/* fold the tick's overlay into `fb` (words it already holds stay) and end the
   overlay: the tick is an ordinary finished frame on the processor from here */
void port_gpu_twod_fold(unsigned *fb, int stride_px);
/* the SM64DS_TWOD_CARD_CHECK compare of the card's overlay and mask with the
   processor's own (the same tick, same stage); `bm` may be null */
void port_gpu_twod_check(const unsigned *fb, int stride_px, const unsigned char *bm,
                         int bm_stride, int what);

/* SAVEOVL1: THE MENU LAYER. A recipe of plain pieces is drawn into a scratch
   layer (premultiplied 0xAARRGGBB, zero = nothing) instead of onto the picture
   a card tick holds; the card lays it over its composite before (post 0) or
   after (post 1) the engine A fade. ready: this tick's overlay is on the card.
   begin hands back the cleared scratch (stride words a row), null if the card
   cannot take it; end uploads what was drawn (1) or the card refused (0). */
int port_gpu_layer_ready(void);
unsigned *port_gpu_layer_begin(int w, int h, int stride);
unsigned *port_gpu_layer2(void);   /* the layer's second plane (a translucent pixel over a translucent one), same stride */
int port_gpu_layer_end(int w, int h, int post);

#ifdef __cplusplus
}
#endif

#endif  /* PORT_HAL_GPU_RASTER_H */
