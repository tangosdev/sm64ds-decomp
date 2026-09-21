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
