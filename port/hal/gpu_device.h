/* THE ONE DIRECT3D 11 DEVICE THIS PROCESS EVER MAKES.
 *
 * Two optional settings now want a graphics card. "PresentBackend" hands the
 * FINISHED picture over through a swap chain (hal/gpu_present.cpp), and
 * "Renderer" draws the OPAQUE 3D pass on the card and reads it back into the
 * software rasteriser's own buffers (hal/gpu_raster.cpp). Either can be on
 * without the other, and with both on there must still be ONE device: two
 * devices would be two copies of every texture, two command queues fighting
 * over the same card, and two separate claims on a 32-bit process's address
 * space, which the stage-0 probe measured as the tightest resource here.
 *
 * So device creation lives here and nowhere else. The first caller's
 * preference decides the driver type; a later caller gets the device that
 * already exists and one line says so if it asked for something different.
 *
 * NO IMPORT TABLE CHANGE, the rule the whole port keeps: d3d11.dll is
 * LoadLibrary'd and D3D11CreateDevice GetProcAddress'd, interface ids come
 * from __uuidof (compile time), and nothing links d3d11.lib, dxgi.lib or
 * dxguid.lib. The exe's import table is byte for byte the one it had before
 * any of this existed.
 *
 * FAILURE IS NEVER FATAL. Every entry point below answers "no device" rather
 * than exiting, and each caller is expected to carry on with the path it had
 * before the card existed.
 */
#ifndef PORT_HAL_GPU_DEVICE_H
#define PORT_HAL_GPU_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Make (or find) the device. `want_warp` is -1 for "hardware, and WARP if the
 * hardware will not start", 0 for hardware only and 1 for WARP only -- WARP
 * being the software device Windows ships, which is what makes a headless
 * proof possible on a machine with no card and inside a test with no window.
 * Returns 1 when a device exists afterwards, 0 when it does not. Safe to call
 * any number of times; only the first call does anything.
 *
 * It reserves the save-state arena first (hal/os_arena.cpp claims 0x30000000
 * lazily and whatever holds that address when the game first carves the arena
 * wins), which the stage-0 probe measured as unnecessary on the machine it ran
 * on and cheap enough to keep as insurance everywhere else.
 */
int port_gpu_device_acquire(int want_warp);

/* ID3D11Device * and ID3D11DeviceContext *, as void * so a caller that does
 * not include d3d11.h can still ask whether there is one. Null until a
 * successful acquire. */
void *port_gpu_device(void);
void *port_gpu_device_context(void);

const char *port_gpu_device_adapter(void);   /* the card's own name, or "" */
int   port_gpu_device_is_warp(void);
unsigned port_gpu_device_feature_level(void);/* D3D_FEATURE_LEVEL, 0 if none */
double port_gpu_device_create_ms(void);      /* what creation cost, once */

/* 1 once an acquire has been tried and failed, so a caller can stop asking. */
int port_gpu_device_failed(void);

/* ---- WHAT THE CARD ITSELF SPENT ----------------------------------------
 *
 * The F3 overlay shows a cpu percentage and, until now, nothing about the
 * card, which is the one number a player with the card renderer on actually
 * wants. A stopwatch on the CPU cannot answer it: the renderer's readback is
 * a synchronous Map, so a CPU timer around the opaque pass measures HOW LONG
 * THE PROCESSOR WAITED, not how long the card was busy, and the present path
 * hands its work over and returns before any of it has run.
 *
 * So the card is asked directly. A D3D11_QUERY_TIMESTAMP pair brackets each
 * span of card work, and one D3D11_QUERY_TIMESTAMP_DISJOINT per picture
 * carries the clock rate those stamps are in and says whether the clock was
 * reliable over that picture at all (it is not across a power-state change,
 * and such a picture is dropped rather than reported).
 *
 * NOTHING HERE EVER WAITS. The results are collected with GetData's
 * do-not-flush flag from a ring of four pictures, so a stamp that is not
 * ready yet is simply read one or two pictures later. When the ring is full
 * the picture is not measured; it is counted and the game carries on.
 *
 * Costs nothing when nobody calls it: the query objects are created on the
 * first span and never at all if there is no device. SM64DS_GPU_TIMER=0
 * turns the whole thing off, which is how the A/B against a build that never
 * asks is run.
 */
enum {
    PORT_GPU_SPAN_OPAQUE = 0,   /* hal/gpu_raster.cpp: clear, draw, copy out */
    PORT_GPU_SPAN_PRESENT = 1,  /* hal/gpu_present.cpp: upload and the quad */
    PORT_GPU_SPAN_COUNT = 2
};

/* Bracket one span of card work. Both are safe with no device, with the timer
   off, and when a span is abandoned half way (a fallback between the two: the
   picture then reports the spans that did finish and nothing else). */
void port_gpu_timer_span_begin(int span);
void port_gpu_timer_span_end(int span);

/* End the current picture's measurement and collect whatever has come back.
   Called once per picture from the present path; everything else is driven
   from it. */
void port_gpu_timer_frame_end(void);

/* The running totals: the card's own busy milliseconds since the process
   started, the same split by span, how many pictures carried a completed
   measurement, and how many pictures were DROPPED (the disjoint-clock query
   came back bad, or every ring slot was still waiting when a new picture
   wanted one): a dropped picture contributes nothing to card_ms, so it would
   silently under-report the card's cost if nobody counted it. Returns 1 when
   the timer is live, 0 when it is off or there is no device. Every pointer is
   optional. */
int port_gpu_timer_totals(double *card_ms, double *opaque_ms,
                          double *present_ms, unsigned long long *pictures,
                          unsigned long long *dropped);

/* THE SIX DS RANGES, THE SAVE-STATE ARENA AND THE FREE ADDRESS SPACE, printed
 * as one block. This is the stage-0 probe's measurement kept as a gate: a
 * 32-bit process that loses one of the fixed ranges to a driver's own mapping
 * is a process whose game cannot run, and the only way to know is to look.
 * `when` is a short label printed on every line. Does nothing when `enabled`
 * is 0, which is every ordinary run. */
void port_gpu_device_address_rows(const char *when, int enabled);

#ifdef __cplusplus
}
#endif

#endif  /* PORT_HAL_GPU_DEVICE_H */
