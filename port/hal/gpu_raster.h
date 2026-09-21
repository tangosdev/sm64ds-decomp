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

#ifdef __cplusplus
}
#endif

#endif  /* PORT_HAL_GPU_RASTER_H */
