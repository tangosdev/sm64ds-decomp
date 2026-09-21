/* THE OPTIONAL GRAPHICS-CARD PRESENT PATH (run hd2, lane GPU1; stage 1 of the
 * staged route in the scout's report).
 *
 * WHAT IT IS. The port draws its picture on the CPU and then hands the finished
 * framebuffer to the window with one GDI call, StretchDIBits. That call has no
 * vsync of its own, so the picture can tear, and its only two scalers are
 * "drop and duplicate whole pixels" and GDI's own box filter. This file offers
 * a second way to hand over the SAME finished picture: upload it to the
 * graphics card as a texture and draw it once as a rectangle through Direct3D
 * 11, which brings a real vsync and a proper scaling filter with it.
 *
 * NOTHING ABOUT THE GAME'S PICTURE CHANGES. The 3D is still rasterised on the
 * CPU, the 2D is still composited on the CPU, screenshots, the selftest image,
 * the display capture and every comparison tool still read the same CPU
 * framebuffer. This is the last step only: how the finished bytes reach the
 * screen.
 *
 * OFF BY DEFAULT AND INERT WHEN OFF. With PresentBackend absent or 0 this file
 * loads no library, creates nothing, allocates nothing and prints nothing: the
 * calls from the present path read one cached int and return. d3d11.dll is
 * LoadLibrary'd and its one entry point GetProcAddress'd, the rule
 * hal/pad_backend.cpp keeps for dinput8 and hal/gpu_probe.cpp kept for the
 * stage-0 probe, so no d3d11, dxgi or dxguid library is linked and the exe's
 * import table does not move.
 *
 * IT FALLS BACK, IT NEVER FAILS. Every failure -- the library missing, no
 * device, no swap chain, a device lost or reset mid-run, a bad Present -- logs
 * one plain line and puts THAT RUN back on the GDI path for good, in the same
 * frame, so no picture is ever dropped and the game never exits or stalls
 * because of this setting.
 *
 * THE KEYS (hal/host_settings.h has the full contract):
 *   PresentBackend  0 GDI (the default), 1 Direct3D 11.   SM64DS_PRESENT_BACKEND
 *   PresentFilter   0 nearest, 1 smooth, 2 sharp.         SM64DS_PRESENT_FILTER_D3D
 *   VSync           0 off, 1 on.                          SM64DS_VSYNC
 * The last two are read only when the backend is 1.
 *
 * THE TEST KNOBS, which a player never sets and which exist because nothing in
 * this lane may put a window on anybody's screen:
 *   SM64DS_PRESENT_DEVICE=warp|hw   pin the device instead of hardware-then-WARP
 *   SM64DS_PRESENT_OFFSCREEN=1      draw the identical upload, quad and filter
 *                                   into an offscreen target, read it back and
 *                                   check it against the framebuffer (and, for
 *                                   the two smooth filters, against a CPU
 *                                   reference). No window, no swap chain.
 *   SM64DS_PRESENT_OFFSCREEN_SIZE=WxH   that target's size; unset is the
 *                                   source picture's own size, which is the
 *                                   row that can be exact.
 *   SM64DS_PRESENT_OFFSCREEN_EVERY=N    check every Nth frame (default 30)
 *   SM64DS_PRESENT_OFFSCREEN_BMP=<name> also write each checked frame out as
 *                                   an ordinary bitmap, so the three filters
 *                                   can be looked at side by side without a
 *                                   window ever appearing
 *   SM64DS_PRESENT_RECTCHECK=1      print the destination rectangle the present
 *                                   path computes and the one this file draws
 *                                   into, at three client sizes
 *   SM64DS_PRESENT_ADDRCHECK=1      print the six DS ranges, the save-state
 *                                   arena and the free address space before
 *                                   and after the device, and at exit
 *   SM64DS_PRESENT_SWAPPROBE=1      build a real flip-model swap chain on an
 *                                   INVISIBLE window of this file's own (never
 *                                   shown, no WS_VISIBLE, so it cannot appear
 *                                   on anybody's screen), present one frame
 *                                   through it and report the address rows
 *   SM64DS_PRESENT_FAIL_AFTER=N     force the fallback after N presents, so the
 *                                   device-lost path is exercised on purpose
 */
#ifndef PORT_GPU_PRESENT_H
#define PORT_GPU_PRESENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* 1 when the Direct3D path should be tried for this frame: the key says 1, the
   platform is Windows and nothing has made this run fall back. Cached, so the
   default run pays one int compare per present. */
int port_gpu_present_enabled(void);

/* 1 when the offscreen proof mode is on. Cached the same way, and 0 in every
   run that did not ask for it, which is every run a player ever makes. */
int port_gpu_present_offscreen_mode(void);

/* PRESENT ONE FRAME THROUGH THE GRAPHICS CARD. Returns 1 when the picture was
   presented and the caller has nothing left to do, 0 when the caller must fall
   through to its own GDI blit for this frame (the backend is off, or it has
   just stood down).

   pixels/stride_px/src_w/src_h are exactly what the GDI path hands
   StretchDIBits: the top-left of the image, the DIB's width as a row stride in
   pixels, and the size of the sub-rectangle actually being shown.
   dst_* is the destination rectangle hal_present_fit computed, and client_*
   the client area it was fitted into; everything outside dst is the black bar
   and is the clear colour here.
   budget_ms is the frame budget the pacer is keeping (the ROM's own vblank
   divider), and frame_rate_key is the FrameRate setting. Both exist for the
   vsync rule, which is spelled out in the .cpp banner. */
int port_gpu_present_frame(void *hwnd, const void *pixels, int stride_px,
                           int src_w, int src_h, int dst_x, int dst_y,
                           int dst_w, int dst_h, int client_w, int client_h,
                           double budget_ms, int frame_rate_key);

/* The offscreen proof mode's per-frame call. Same upload, same quad, same
   filter, into a render target of this file's own instead of a swap chain,
   then read back and compared. Does nothing unless the mode is on. */
void port_gpu_present_offscreen_frame(const void *pixels, int stride_px,
                                      int src_w, int src_h);

#ifdef __cplusplus
}
#endif

#endif /* PORT_GPU_PRESENT_H */
