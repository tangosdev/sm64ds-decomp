/* THE OPTIONAL GRAPHICS-CARD PRESENT PATH. See hal/gpu_present.h for what it
 * is, which keys switch it on and which test knobs exist.
 *
 * ================= THE GAMEPLAY-SPEED RULE, WHICH IS THE WHOLE REASON THIS
 * FILE IS CAREFUL =========================================================
 *
 * This game advances ONE fixed step per tick and never catches up. The pacer
 * (tests/walk_window.cpp, frame_pace) keeps an absolute deadline one frame
 * budget apart, where the budget is the ROM's own vblank divider: 33.3 ms in a
 * 3D level, 16.65 ms in a minigame. Anything that makes a frame take longer
 * than its budget does not drop a picture, it makes the GAME RUN SLOWLY.
 *
 * Present(1) BLOCKS until the monitor's next refresh. So a blocking present is
 * exactly the kind of thing that can put the game in slow motion, and this file
 * refuses to let it.
 *
 * THERE IS NO DOUBLE WAIT, BY CONSTRUCTION. frame_pace sleeps UNTIL AN ABSOLUTE
 * DEADLINE, not "for the leftover of a measurement it took earlier". The
 * present happens before that sleep, so whatever the vsync block costs is
 * subtracted from the sleep automatically: the two waits share one budget
 * instead of adding up. Nothing in the pacer had to change for that, and
 * nothing in the pacer was changed.
 *
 * WHEN THE BLOCK IS FREE AND WHEN IT IS NOT. Write W for the frame's own work,
 * B for the budget and T for the monitor's refresh period. The present blocks
 * at most T. If W + T <= B the block lands inside the slack the pacer was going
 * to sleep away and the tick rate is exactly what it was. If W + T > B the
 * frame period starts locking to the vblank grid instead of the deadline grid,
 * and the game slows to the rate that grid allows. So the question is never
 * "is vsync on", it is "does this frame have a refresh period of slack".
 *
 * THE RULE THIS FILE IMPLEMENTS, in four parts:
 *
 *  1. THE FRAMERATE KEY REFUSES VSYNC OUTRIGHT. With FrameRate set, the pacer's
 *     presentation clock presents the same finished picture several times
 *     inside one tick's slack. Each of those presents would block for a whole
 *     refresh. Worked: FrameRate 144 on a 60 Hz monitor inside a 33.3 ms budget
 *     asks for about five presents, each blocking 16.7 ms, which is 83 ms spent
 *     in a 33.3 ms budget -- a game at two fifths speed. So the two settings do
 *     not combine: one line is printed and the run presents with no wait.
 *
 *  2. A MONITOR SLOWER THAN THE TICK REFUSES VSYNC FOR THAT FRAME. If T is
 *     longer than the budget (allowing 2 per cent), every tick would wait a
 *     whole refresh and the game would simply run at the refresh rate. A 30 Hz
 *     output with a 60-tick minigame is the real case. Checked per frame,
 *     because the budget changes with the scene.
 *
 *  3. A WATCHDOG MEASURES IT RATHER THAN TRUSTING THE ARITHMETIC. Over each
 *     window of 120 presents this file keeps the average frame period, the
 *     average time spent inside Present and the average budget. If the average
 *     period is more than 2 per cent over the budget AND the period without the
 *     block would have fitted inside the budget, then the block is the cause:
 *     vsync stands down for the rest of the run and says so in one line. The
 *     second half of that test is what stops it blaming vsync for a frame that
 *     was already too slow on its own (RenderScale 4 on the title screen is 73
 *     ms of work in a 33.3 ms budget; vsync is not why).
 *
 *  4. THE 2 PER CENT IS NOT SLOP, IT IS A MEASURED CONSTANT. The port's vblank
 *     is 16.65 ms and a 60 Hz monitor's is 16.667 ms, so a perfectly healthy
 *     vsynced run parks at 60.00 ticks a second where an unsynced one reads
 *     60.06. That is 0.1 per cent and must not read as a slowdown.
 *
 * WHAT THAT COMES OUT AS, monitor by monitor:
 *   60 Hz, 3D level (budget 33.3 ms):   vsync on. 30.00 ticks/s against 30.03.
 *   60 Hz, minigame (budget 16.65 ms):  vsync on. 60.00 ticks/s against 60.06.
 *   75 Hz (13.33 ms), minigame:         allowed; a frame with under about 3 ms
 *                                       of work keeps pace, and the watchdog
 *                                       stands vsync down within two seconds if
 *                                       the work is heavier than that.
 *   144 Hz (6.94 ms):                   vsync on everywhere; the block is at
 *                                       most 6.94 ms of a 16.65 ms budget.
 *   any FrameRate:                      vsync off, by rule 1.
 *   a monitor at 30 Hz or below:        vsync off, by rule 2.
 *
 * =========================================================================
 *
 * NO IMPORT TABLE CHANGE. d3d11.dll and user32.dll are LoadLibrary'd and every
 * entry point GetProcAddress'd; interface ids come from __uuidof, which is
 * compile time; the swap chain is reached through the device's own IDXGIDevice
 * so dxgi.dll is never named. No d3d11.lib, dxgi.lib or dxguid.lib is linked.
 *
 * THE SHADERS ARE COMPILED OFFLINE into hal/gpu_present_shaders.h with the
 * Windows SDK's fxc.exe; the HLSL is hal/gpu_present.hlsl beside it and the
 * exact command lines are in the generated header. d3dcompiler.dll never loads.
 */
#if defined(_WIN32)

#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal/gpu_device.h"
#include "hal/gpu_raster.h"
#include "ntr/gx.h"
#include "hal/gpu_present.h"
#include "hal/presenter.h"
#include "hal/gpu_present_shaders.h"
#include "hal/host_settings.h"

/* THE FIT, hal/sub_screen.cpp's, declared here the way tests/walk_window.cpp
   and hal/sub_screen.cpp declare it: a bare declaration, because it has no
   header. THIS FILE NEVER COMPUTES A DESTINATION RECTANGLE. The rectangle is
   handed to port_gpu_present_frame by the present path, which got it from this
   one function, and the only use here is the rectangle check below, which
   exists to prove that turning the rectangle into the shader's coordinates and
   back gives the same four numbers. */
extern "C" void hal_present_fit(int cw, int ch, int sw, int sh, int *x,
                                int *y, int *w, int *h);
/* The lost card's other halves (run after093, lane MGFREEZE2), bare
   declarations for the same reason: hal/gpu_device.cpp makes a new device,
   hal/presenter.cpp abandons its own, hal/gpu_raster.cpp hears and reports a
   removed card. */
extern "C" int port_gpu_device_renew(void);
extern "C" int port_scene_live_id(void);                       /* hal/scene_boot.cpp: -1 = no scene is live (a level) */
extern "C" void presenter_abandon_device(void);
extern "C" void port_gpu_raster_set_lost_hook(void (*f)(long));
extern "C" void port_gpu_raster_device_lost(long hr);
extern "C" void port_gpu_raster_device_renewed(void);

/* Older SDKs spell the flip model in a later dxgi header; the value is fixed. */
#ifndef DXGI_SWAP_EFFECT_FLIP_DISCARD
#define DXGI_SWAP_EFFECT_FLIP_DISCARD ((DXGI_SWAP_EFFECT)4)
#endif
#ifndef DXGI_MWA_NO_ALT_ENTER
#define DXGI_MWA_NO_ALT_ENTER (1 << 1)
#endif
#ifndef DXGI_STATUS_OCCLUDED
#define DXGI_STATUS_OCCLUDED ((HRESULT)0x087A0001L)
#endif

namespace {

/* ---- the settings, read once ------------------------------------------- */

int g_started;          /* the one-time read has happened */
int g_backend;          /* PresentBackend */
int g_filter;           /* PresentFilter */
int g_vsync_key;        /* VSync */
int g_down;             /* this run has fallen back to GDI for good */
int g_pres_off;         /* the presenter thread would not start: the old path for this run */
int g_repeat;           /* the picture being presented is the previous one again (statistics, elision) */
int g_offscreen;        /* the proof mode */
int g_off_w, g_off_h;   /* its target size, 0 for "the source's own" */
int g_off_every = 30;   /* check every Nth frame */
int g_addrcheck;
int g_swapprobe;
int g_fail_after;       /* the device-lost drill */
int g_want_warp = -1;   /* -1 hardware then WARP, 0 hardware only, 1 WARP only */

/* ---- the pictures in flight (run after093, lane MGFREEZE1) ----------------
   The upload texture is DYNAMIC and every picture maps it WRITE_DISCARD, so
   while the card still reads the last picture the driver hands out a fresh
   copy of the whole texture. Nothing on this device ever waited for the card:
   the swap chain is the presenter thread's, on its own device, so its frame
   latency limit never reached this queue. On a fast card that costs nothing.
   On a tester's Intel UHD (128 MB, the card 30 to 80 ms behind a tick) the
   stacked minigame picture is 1364x1664, 9 MB a copy, and the copies piled up
   until Map said E_OUTOFMEMORY and the run fell back for good. Now an event
   query marks each upload and the next one waits until at most
   g_inflight_max of them are still unfinished on the card, which is the frame
   latency limit a swap chain on this device would have given. */
enum { FENCE_RING = 32 };
int g_inflight_max = 2;         /* SM64DS_PRESENT_INFLIGHT (0 = no limit, the old behaviour) */
ID3D11Query *g_fence[FENCE_RING];
int g_fence_head, g_fence_n;
int g_fence_deep;               /* deepest count of unfinished uploads seen at an upload */
int g_fence_failed;             /* the card would not make a query: no limit, as before */
long long g_fence_waits;
double g_fence_wait_ms;
int g_oom_retries, g_oom_recovered;
/* A REFUSED UPLOAD SKIPS ONE PICTURE, IT DOES NOT END THE CARD PATH (run
   after093, lane MGFREEZE1). The fallback cannot give a moving picture back:
   the swap chain is a flip-model one, the compositor shows its buffers as the
   window and not what GDI draws there, and releasing it mid-run (which is what
   would hand the window back) crashed the AMD driver's own worker thread in
   every try. So one refusal (after the wait and the retry above) drops that
   picture and leaves the one on show; the next picture tries again. Only a
   card that refuses UPLOAD_REFUSALS_MAX pictures in a row is given up. */
enum { UPLOAD_REFUSALS_MAX = 60 };
int g_up_refused_run;           /* refusals in a row */
int g_up_refused;               /* refusals this run */
int g_up_skipped_said;
int g_uploads;                  /* pictures uploaded this run */
/* ---- THE LOST CARD PATH IS REBUILT, NOT GIVEN UP (run after093, lane
   MGFREEZE2) ----------------------------------------------------------------
   A permanent failure used to end the card path for the run: fall_back handed
   the window to the ordinary Windows path, and Windows kept showing the last
   picture of the flip-model swap chain over it, so a tester saw a frozen game
   that was still running (5 of her 11 sessions: DXGI_ERROR_DEVICE_REMOVED
   right after the out-of-memory). Handing the window back would need the swap
   chain destroyed mid-run, and every way of doing that (release, then flush
   the presenter's queue) crashed the AMD driver's own worker thread.
   So two things change.
   THE SWAP CHAIN LIVES ON A CHILD WINDOW that fills the game window's client
   area (disabled and transparent to the mouse, so every click, key and cursor
   still belongs to the game window). A swap chain is never destroyed: on a
   loss the child is hidden and the chain, the presenter's device and, when
   the card itself was removed, the shared device are ABANDONED (no release,
   no flush); the next picture rebuilds everything through the same path the
   first picture took, on a fresh child window, so the one-flip-chain-per-
   window rule never comes up. The game sees one or two pictures go by on the
   ordinary path, hidden under the child.
   ONLY WHEN THE REBUILD FAILS REBUILD_FAILS_MAX TIMES IN A ROW (or the run
   has rebuilt RECOVERIES_MAX times) does the run go to the ordinary Windows
   path, and then the child is hidden first, so the game window's own picture
   is what Windows shows.
   THE TRIES ARE SPREAD OUT (0.9.3.1 field report: a driver reset while
   another game started). The first rebuild still runs on the next picture.
   A failed one waits before the next: 1, 2, 4, 8 and then 16 s, about half a
   minute in all, because a driver that is still resetting refuses every try
   made in the same second (the report's two back-to-back tries both got
   887a0005 and the run stayed on the processor for good). The ordinary
   Windows path draws the pictures in between, as it does during any
   rebuild. Each failed try abandons what it made, so the count stays small. */
enum { RECOVERIES_MAX = 8, REBUILD_FAILS_MAX = 6 };
static int rebuild_wait_ms(int fails) { return fails >= 5 ? 16000 : 1000 << (fails - 1); }
/* SEAMCLEAN1: a rebuild in a LEVEL keeps about 80 MB (measured by MGFREEZE3's reviewer: AMD +83 MB, NVIDIA +68 local and +14
   non-local), against 36 to 39 MB from a minigame screen, because the level's renderer objects stay behind with the abandoned
   device. Eight of those would be 640 MB, so a level gives the card path up after three (about 240 MB) and the ordinary Windows
   path takes over, as it does after eight anywhere else. */
enum { RECOVERIES_IN_LEVEL_MAX = 3 };
static int recoveries_cap() { return port_scene_live_id() < 0 ? RECOVERIES_IN_LEVEL_MAX : RECOVERIES_MAX; }
HWND g_parent;                  /* the game window the card path presents for */
HWND g_child;                   /* the window the swap chain is made on (0: the game window itself) */
int g_lost;                     /* a rebuild is under way */
HRESULT g_lost_hr;
long long g_lost_at;
int g_renew;                    /* the shared device is gone too: make a new one first */
int g_rebuild_fails;            /* failed rebuilds in a row */
long long g_retry_at;           /* no rebuild before this qpc time (0: the next picture) */
int g_recoveries;               /* rebuilds finished this run */
int g_giving_up;
int g_tell_renderer;            /* the shared device is new: the renderer may come back to the card */
#ifndef PORT_ROM_CLEAN
/* DEVELOPER DRILLS, compiled out of the kit. SM64DS_PRESENT_LOST_AT=<n>: at
   picture n the run acts as if the card had been removed (the whole rebuild,
   a new shared device included, and the renderer told). With
   SM64DS_PRESENT_REBUILD_FAIL=<k> the first k rebuilds fail on purpose, so the
   give-up after two failures can be watched too. */
int g_lost_drill_at;
int g_rebuild_fail_drill;
/* SM64DS_PRESENT_LOST_EVERY=<n> arms the drill again n pictures after each
   rebuild (the memory account across many rebuilds, lane MGFREEZE3);
   SM64DS_PRESENT_LOST_KEEPDEV=1 makes it a loss that keeps the shared device
   (the sixty refusals' kind) instead of a removed card. */
int g_lost_drill_every;
int g_lost_drill_keepdev;
#endif
#ifndef PORT_ROM_CLEAN
/* DEVELOPER KNOBS, compiled out of the kit (run after093, lane MGFREEZE1).
   SM64DS_GPU_MEMTRACE=1 prints the adapter's own memory account once a second
   (IDXGIAdapter3::QueryVideoMemoryInfo, local and non-local, usage against the
   budget the driver gives this process), so a growth curve is a number and not
   a guess. SM64DS_PRESENT_FAIL_AT=<n> makes upload n come back E_OUTOFMEMORY,
   the way the card refused it on a tester's 128 MB Intel chip, on the first try
   and on the retry, so the fallback runs; with SM64DS_PRESENT_FAIL_AT_ONCE=1
   only the first try fails and the retry is what is exercised. */
int g_memtrace;
int g_fail_at;
int g_fail_at_once;
long long g_memtrace_last;
int g_memtrace_up_last;
IDXGIAdapter3 *g_ad3;
int g_ad3_tried;
#endif

/* ---- what the run learned, for the one summary line at exit ------------- */

int g_presents;             /* frames handed to the swap chain */
int g_off_frames;           /* frames drawn into the offscreen target */
int g_off_checked;          /* of those, frames read back and compared */
int g_off_bad;              /* compared frames with any mismatched pixel */
unsigned g_off_worst_rgb;   /* worst count of pixels differing in R, G or B */
unsigned g_off_worst_a;     /* worst count differing only in alpha */
int g_off_maxerr;           /* worst single-channel error against the CPU reference */
int g_off_tol;              /* what counted as agreement: 0 for a point sample */
unsigned g_off_worst_tie;   /* worst count of pixels sitting on a texel boundary */
double g_up_ms, g_draw_ms, g_block_ms;   /* accumulated cost */
int g_cost_n;

/* ---- the vsync rule's own state ----------------------------------------- */

int      g_refresh_hz;
double   g_refresh_ms;
int      g_vsync_said;        /* the stand-down line has been printed */
long long g_qpf, g_last_end;
double   g_win_period, g_win_block, g_win_budget;
int      g_win_n;
enum { VSYNC_WINDOW = 120 };

/* ---- entry points resolved at run time ----------------------------------- */

typedef BOOL (WINAPI *EnumDisplaySettingsA_t)(LPCSTR, DWORD, DEVMODEA *);
typedef ATOM (WINAPI *RegisterClassA_t)(const WNDCLASSA *);
typedef HWND (WINAPI *CreateWindowExA_t)(DWORD, LPCSTR, LPCSTR, DWORD, int, int,
                                         int, int, HWND, HMENU, HINSTANCE,
                                         LPVOID);
typedef BOOL (WINAPI *DestroyWindow_t)(HWND);
typedef LRESULT (WINAPI *DefWindowProcA_t)(HWND, UINT, WPARAM, LPARAM);
typedef DWORD (WINAPI *GetModuleFileNameA_t)(HMODULE, LPSTR, DWORD);

HMODULE g_user32;

/* ---- the device and everything hanging off it ----------------------------
   THE DEVICE ITSELF IS NOT THIS FILE'S ANY MORE. hal/gpu_device.cpp makes the
   one device this process has, because the optional renderer ("Renderer",
   hal/gpu_raster.cpp) wants the same one and two devices would be two copies
   of everything on a card and two claims on a 32-bit process's address space.
   The two pointers below are that device, borrowed for the life of the run;
   this file creates none of it and releases none of it. */
ID3D11Device           *g_dev;
ID3D11Device           *g_sc_dev;      /* the device the swap chain lives on: g_dev, or the presenter's own */
ID3D11DeviceContext    *g_ctx;
int                     g_tried_device;
ID3D11VertexShader     *g_vs;
ID3D11PixelShader      *g_ps;
ID3D11Buffer           *g_cb;
ID3D11SamplerState     *g_smp_point, *g_smp_linear;
ID3D11RasterizerState  *g_rast;
ID3D11Texture2D        *g_tex;          /* the upload texture */
ID3D11ShaderResourceView *g_srv;
int                     g_tex_w, g_tex_h;
IDXGISwapChain         *g_sc;
IDXGISwapChain1        *g_sc1;
ID3D11RenderTargetView *g_scrtv;
int                     g_sc_w, g_sc_h;
HWND                    g_sc_hwnd;
ID3D11Texture2D        *g_off_rt, *g_off_stage;
ID3D11RenderTargetView *g_off_rtv;
int                     g_off_rt_w, g_off_rt_h;
char                    g_adapter[128];
D3D_FEATURE_LEVEL       g_level;
int                     g_is_warp;

/* ---- small helpers ------------------------------------------------------- */

long long qpc()
{
    LARGE_INTEGER c;
    QueryPerformanceCounter(&c);
    return c.QuadPart;
}

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

/* ONE LINE, AND THEN THIS RUN IS ON THE GDI PATH FOR GOOD. Never an exit, never
   a stall: the caller returns 0 and its own StretchDIBits draws the frame that
   was about to be drawn here, so not even one picture is lost. */
void child_hide();
bool lose(const char *why, HRESULT hr);

void fall_back(const char *why, HRESULT hr)
{
    if (g_down) return;
    /* once the card path has shown pictures, a failure is a loss to rebuild */
    if (!g_giving_up && g_presents > 0 && (g_child || g_lost) && lose(why, hr)) return;
    g_down = 1;
    child_hide();
    presenter_end();
    port_gpu_raster_present_down();
    if (hr)
        fprintf(stderr, "[present] the graphics card path stopped working (%s, "
                        "code %08x). This run goes back to the ordinary Windows "
                        "one; nothing else changes.\n",
                why, (unsigned)hr);
    else
        fprintf(stderr, "[present] the graphics card path is not available (%s). "
                        "This run uses the ordinary Windows one; nothing else "
                        "changes.\n", why);
}

/* ---- the address rows, the stage-0 probe's measurement kept as a gate -----
   The rows themselves moved to hal/gpu_device.cpp with the device, because the
   renderer wants exactly the same audit at exactly the same moments and one
   copy of it is one answer. Same lines, same order, same label. */

void address_rows(const char *when)
{
    port_gpu_device_address_rows(when, g_addrcheck);
}

/* ---- the one-time read of the environment and the file ------------------- */

void read_settings()
{
    if (g_started) return;
    g_started = 1;

    g_backend = host_setting_present_backend();
    if (!g_backend) return;          /* the whole default path stops here */

    g_filter = host_setting_present_filter();
    g_vsync_key = host_setting_vsync();

    const char *dev = getenv("SM64DS_PRESENT_DEVICE");
    if (dev && *dev) {
        if (!_stricmp(dev, "warp")) g_want_warp = 1;
        else if (!_stricmp(dev, "hw") || !_stricmp(dev, "hardware")) g_want_warp = 0;
    }
    g_offscreen = env_int("SM64DS_PRESENT_OFFSCREEN", 0);
    if (const char *s = getenv("SM64DS_PRESENT_OFFSCREEN_SIZE")) {
        int w = 0, h = 0;
        if (sscanf(s, "%dx%d", &w, &h) == 2 && w > 0 && h > 0) {
            g_off_w = w;
            g_off_h = h;
        }
    }
    g_off_every = env_int("SM64DS_PRESENT_OFFSCREEN_EVERY", 30);
    if (g_off_every < 1) g_off_every = 1;
    g_addrcheck = env_int("SM64DS_PRESENT_ADDRCHECK", 0);
    g_swapprobe = env_int("SM64DS_PRESENT_SWAPPROBE", 0);
    g_fail_after = env_int("SM64DS_PRESENT_FAIL_AFTER", 0);
    g_inflight_max = env_int("SM64DS_PRESENT_INFLIGHT", 2);
#ifndef PORT_ROM_CLEAN
    g_memtrace = env_int("SM64DS_GPU_MEMTRACE", 0);
    g_fail_at = env_int("SM64DS_PRESENT_FAIL_AT", 0);
    g_fail_at_once = env_int("SM64DS_PRESENT_FAIL_AT_ONCE", 0);
    g_lost_drill_at = env_int("SM64DS_PRESENT_LOST_AT", 0);
    g_rebuild_fail_drill = env_int("SM64DS_PRESENT_REBUILD_FAIL", 0);
    g_lost_drill_every = env_int("SM64DS_PRESENT_LOST_EVERY", 0);
    g_lost_drill_keepdev = env_int("SM64DS_PRESENT_LOST_KEEPDEV", 0);
#endif
    /* SEAMCLEAN1: a negative SM64DS_PRESENT_INFLIGHT is not a limit of "none" (that is 0 alone): it reads as the default */
    if (g_inflight_max < 0) g_inflight_max = 2;
    if (g_inflight_max > FENCE_RING - 1) g_inflight_max = FENCE_RING - 1;

    LARGE_INTEGER f;
    if (QueryPerformanceFrequency(&f)) g_qpf = f.QuadPart;

    /* the monitor's refresh, for the vsync rule. user32 hand-loaded, the rule
       this whole port keeps. A display reporting 0 or 1 is the "driver
       default" answer and is no answer, so it reads as unknown and the rule
       treats it as 60. */
    g_user32 = LoadLibraryA("user32.dll");
    if (g_user32) {
        EnumDisplaySettingsA_t f2 = (EnumDisplaySettingsA_t)GetProcAddress(
            g_user32, "EnumDisplaySettingsA");
        if (f2) {
            DEVMODEA dm;
            memset(&dm, 0, sizeof dm);
            dm.dmSize = sizeof dm;
            if (f2(0, ENUM_CURRENT_SETTINGS, &dm) && dm.dmDisplayFrequency > 1)
                g_refresh_hz = (int)dm.dmDisplayFrequency;
        }
    }
    g_refresh_ms = 1000.0 / (double)(g_refresh_hz > 0 ? g_refresh_hz : 60);
}

/* ---- the device ----------------------------------------------------------
   Made by hal/gpu_device.cpp, which also names the adapter and keeps the
   feature level and the creation cost. This file only borrows it. */

bool make_pipeline()
{
    HRESULT hr = g_dev->CreateVertexShader(kGpuPresentVS, sizeof kGpuPresentVS,
                                           0, &g_vs);
    if (FAILED(hr)) { fall_back("the vertex shader would not load", hr); return false; }
    hr = g_dev->CreatePixelShader(kGpuPresentPS, sizeof kGpuPresentPS, 0, &g_ps);
    if (FAILED(hr)) { fall_back("the pixel shader would not load", hr); return false; }

    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof bd);
    bd.ByteWidth = 48;                 /* three float4s, a multiple of 16 */
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = g_dev->CreateBuffer(&bd, 0, &g_cb);
    if (FAILED(hr)) { fall_back("no constant buffer", hr); return false; }

    D3D11_SAMPLER_DESC sd;
    memset(&sd, 0, sizeof sd);
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    hr = g_dev->CreateSamplerState(&sd, &g_smp_point);
    if (FAILED(hr)) { fall_back("no point sampler", hr); return false; }
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    hr = g_dev->CreateSamplerState(&sd, &g_smp_linear);
    if (FAILED(hr)) { fall_back("no smooth sampler", hr); return false; }

    /* CULL_NONE because the quad comes out of SV_VertexID and its winding is
       whatever the rectangle's sign happens to make it; there is exactly one
       surface in this pipeline and nothing to cull. */
    D3D11_RASTERIZER_DESC rd;
    memset(&rd, 0, sizeof rd);
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    hr = g_dev->CreateRasterizerState(&rd, &g_rast);
    if (FAILED(hr)) { fall_back("no rasterizer state", hr); return false; }
    return true;
}

/* The fences. GetData with no flags also flushes the queue, so a wait never
   waits on work that was not yet handed to the card. */
void fence_pop()
{
    g_fence_head = (g_fence_head + 1) % FENCE_RING;
    --g_fence_n;
}

bool fence_wait_oldest(double limit_ms)
{
    ID3D11Query *q = g_fence[g_fence_head];
    const long long t0 = qpc();
    bool done = false;
    for (;;) {
        const HRESULT hr = g_ctx->GetData(q, 0, 0, 0);
        if (hr == S_OK) { done = true; break; }
        if (FAILED(hr)) break;                     /* a removed device answers here */
        if (ms_between(t0, qpc()) > limit_ms) break;
        SwitchToThread();
    }
    g_fence_wait_ms += ms_between(t0, qpc());
    fence_pop();
    return done;
}

void inflight_retire()
{
    while (g_fence_n && g_ctx->GetData(g_fence[g_fence_head], 0, 0,
                                       D3D11_ASYNC_GETDATA_DONOTFLUSH) == S_OK)
        fence_pop();
}

/* Before an upload: let the card catch up to at most g_inflight_max
   unfinished pictures. A second at most per wait, so a hung card can stall a
   picture but never the run. */
void inflight_throttle()
{
    if (g_fence_failed || !g_ctx) return;
    inflight_retire();
    if (g_fence_n > g_fence_deep) g_fence_deep = g_fence_n;
    if (!g_inflight_max) {
        if (g_fence_n >= FENCE_RING - 1) fence_pop();   /* counting only: the deepest then reads "at least" */
        return;
    }
    while (g_fence_n >= g_inflight_max) {
        ++g_fence_waits;
        fence_wait_oldest(1000.0);
    }
}

/* After an upload: mark it. */
void inflight_mark()
{
    if (g_fence_failed || !g_dev || !g_ctx) return;
    const int at = (g_fence_head + g_fence_n) % FENCE_RING;
    if (!g_fence[at]) {
        D3D11_QUERY_DESC qd;
        qd.Query = D3D11_QUERY_EVENT;
        qd.MiscFlags = 0;
        if (FAILED(g_dev->CreateQuery(&qd, &g_fence[at])) || !g_fence[at]) {
            g_fence[at] = 0;
            g_fence_failed = 1;
            fprintf(stderr, "[present] the card would not make an event query; uploads are not paced\n");
            return;
        }
    }
    g_ctx->End(g_fence[at]);
    ++g_fence_n;
}

/* Everything handed over, and all of it finished (or two seconds gone). */
void inflight_drain()
{
    if (!g_ctx) return;
    g_ctx->Flush();
    if (g_fence_failed) { Sleep(50); return; }
    const long long t0 = qpc();
    inflight_mark();
    while (g_fence_n && ms_between(t0, qpc()) < 2000.0)
        fence_wait_oldest(2000.0 - ms_between(t0, qpc()));
}

int inflight_now() { return g_fence_n; }
int inflight_deepest() { return g_fence_deep; }

void release_fences()
{
    for (int i = 0; i < FENCE_RING; ++i)
        if (g_fence[i]) { g_fence[i]->Release(); g_fence[i] = 0; }
    g_fence_head = g_fence_n = 0;
}

#ifndef PORT_ROM_CLEAN
/* The memory trace's one line, at most once a second. */
void memtrace_tick()
{
    if (!g_memtrace || !g_dev || !g_qpf) return;
    const long long now = qpc();
    if (g_memtrace_last && ms_between(g_memtrace_last, now) < 1000.0) return;
    static long long first;
    if (!first) first = now;
    g_memtrace_last = now;
    if (!g_ad3 && !g_ad3_tried) {
        g_ad3_tried = 1;
        IDXGIDevice *dx = 0;
        if (SUCCEEDED(g_dev->QueryInterface(__uuidof(IDXGIDevice), (void **)&dx)) && dx) {
            IDXGIAdapter *ad = 0;
            if (SUCCEEDED(dx->GetAdapter(&ad)) && ad) {
                ad->QueryInterface(__uuidof(IDXGIAdapter3), (void **)&g_ad3);
                ad->Release();
            }
            dx->Release();
        }
        if (!g_ad3) fprintf(stderr, "[memtrace] this Windows has no IDXGIAdapter3; no memory account\n");
    }
    DXGI_QUERY_VIDEO_MEMORY_INFO lo, nl;
    memset(&lo, 0, sizeof lo);
    memset(&nl, 0, sizeof nl);
    if (g_ad3) {
        g_ad3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &lo);
        g_ad3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nl);
    }
    const double mb = 1024.0 * 1024.0;
    fprintf(stderr, "[memtrace] t=%.1fs local %.1f of %.1f MB, non-local %.1f of %.1f MB, "
            "uploads %d (+%d this second), upload texture %dx%d (%.2f MB), in flight %d "
            "(deepest %d, limit %d, waits %lld)\n",
            ms_between(first, now) / 1000.0,
            (double)lo.CurrentUsage / mb, (double)lo.Budget / mb,
            (double)nl.CurrentUsage / mb, (double)nl.Budget / mb,
            g_uploads, g_uploads - g_memtrace_up_last, g_tex_w, g_tex_h,
            (double)g_tex_w * g_tex_h * 4.0 / mb, inflight_now(), inflight_deepest(),
            g_inflight_max, g_fence_waits);
    g_memtrace_up_last = g_uploads;
}

/* The drill's Map: a real Map, then the refusal the tester's card gave. */
HRESULT drill_map(D3D11_MAPPED_SUBRESOURCE *m, int retry)
{
    HRESULT hr = g_ctx->Map(g_tex, 0, D3D11_MAP_WRITE_DISCARD, 0, m);
    /* SM64DS_PRESENT_FAIL_AT_RUN=<k>: that many pictures in a row are refused
       (default 1), so the give-up after a run of refusals can be drilled too */
    static int left = -1;
    if (left < 0) left = env_int("SM64DS_PRESENT_FAIL_AT_RUN", 1);
    if (g_fail_at && g_uploads + 1 == g_fail_at && left > 0 && (!retry || !g_fail_at_once)) {
        if (SUCCEEDED(hr)) g_ctx->Unmap(g_tex, 0);
        if (retry) --left;
        if (left < 2 || retry == 0)
        fprintf(stderr, "[present] SM64DS_PRESENT_FAIL_AT=%d: upload %d answers E_OUTOFMEMORY%s\n",
                g_fail_at, g_uploads + 1, retry ? " on the retry too" : "");
        memset(m, 0, sizeof *m);
        hr = E_OUTOFMEMORY;
    }
    return hr;
}
#define UPLOAD_MAP(m, retry) drill_map((m), (retry))
#else
#define UPLOAD_MAP(m, retry) g_ctx->Map(g_tex, 0, D3D11_MAP_WRITE_DISCARD, 0, (m))
#endif

/* The upload texture, sized to the picture. Recreated when the source size
   changes, which happens when the layout switches between the inset level
   picture and the taller stacked one. */
bool ensure_texture(int w, int h)
{
    if (g_tex && g_tex_w == w && g_tex_h == h) return true;
    if (g_srv) { g_srv->Release(); g_srv = 0; }
    if (g_tex) { g_tex->Release(); g_tex = 0; }
    g_tex_w = g_tex_h = 0;

    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = 1;
    d.ArraySize = 1;
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;   /* the DIB's own byte order */
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DYNAMIC;
    d.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    d.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_tex);
    if (FAILED(hr) || !g_tex) { fall_back("no upload texture", hr); return false; }
    hr = g_dev->CreateShaderResourceView(g_tex, 0, &g_srv);
    if (FAILED(hr) || !g_srv) { fall_back("no texture view", hr); return false; }
    g_tex_w = w;
    g_tex_h = h;
    return true;
}

/* THE UPLOAD, and the whole reason stride is an argument. The framebuffer is a
   SCREEN_W-wide allocation with the live picture in its top-left corner, so a
   row of the picture is src_w pixels out of every stride_px, exactly the
   sub-rectangle the DIB header makes StretchDIBits read. WRITE_DISCARD so the
   driver hands back a fresh buffer instead of waiting for the last frame's. */
bool upload(const void *pixels, int stride_px, int w, int h)
{
    if (!ensure_texture(w, h)) return false;
    inflight_throttle();
    D3D11_MAPPED_SUBRESOURCE m;
    memset(&m, 0, sizeof m);
    HRESULT hr = UPLOAD_MAP(&m, 0);
    if (hr == E_OUTOFMEMORY) {
        /* The card is out of room for another copy. Hand it everything, wait
           until it has finished it (which frees every older copy), and try
           once more before giving the card path up. */
        ++g_oom_retries;
        if (g_oom_retries <= 3)
            fprintf(stderr, "[present] the upload ran out of card memory (code %08x); "
                    "waiting for the card and trying once more\n", (unsigned)hr);
        inflight_drain();
        memset(&m, 0, sizeof m);
        hr = UPLOAD_MAP(&m, 1);
        if (SUCCEEDED(hr) && m.pData) ++g_oom_recovered;
    }
    if (FAILED(hr) || !m.pData) {
        ++g_up_refused;
        /* a removed card never takes another upload: no point waiting out
           the sixty (run after093, lane MGFREEZE2) */
        if (++g_up_refused_run >= UPLOAD_REFUSALS_MAX ||
            g_dev->GetDeviceRemovedReason() != S_OK) {
            fall_back("the upload was refused", hr);
            return false;
        }
        if (g_up_skipped_said < 3) {
            ++g_up_skipped_said;
            fprintf(stderr, "[present] the card refused upload %d (code %08x); that picture is "
                    "skipped, the one on show stays and the next one tries again\n",
                    g_uploads + 1, (unsigned)hr);
        }
        return false;
    }
    g_up_refused_run = 0;
    ++g_uploads;
    const unsigned char *src = (const unsigned char *)pixels;
    unsigned char *dst = (unsigned char *)m.pData;
    const size_t row = (size_t)w * 4;
    for (int y = 0; y < h; ++y)
        memcpy(dst + (size_t)y * m.RowPitch, src + (size_t)y * stride_px * 4, row);
    g_ctx->Unmap(g_tex, 0);
    inflight_mark();
    return true;
}

/* The destination rectangle, in the coordinates the shader wants. dst is in
   pixels inside a target_w by target_h surface, with y down; NDC has y up. */
bool set_params(int dst_x, int dst_y, int dst_w, int dst_h, int target_w,
                int target_h, int src_w, int src_h, int filter)
{
    D3D11_MAPPED_SUBRESOURCE m;
    memset(&m, 0, sizeof m);
    const HRESULT hr = g_ctx->Map(g_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &m);
    if (FAILED(hr) || !m.pData) { fall_back("the draw parameters were refused", hr); return false; }
    float *f = (float *)m.pData;
    f[0] = (float)dst_x / (float)target_w * 2.0f - 1.0f;
    f[1] = 1.0f - (float)dst_y / (float)target_h * 2.0f;
    f[2] = (float)(dst_x + dst_w) / (float)target_w * 2.0f - 1.0f;
    f[3] = 1.0f - (float)(dst_y + dst_h) / (float)target_h * 2.0f;
    f[4] = (float)src_w;
    f[5] = (float)src_h;
    f[6] = 1.0f / (float)src_w;
    f[7] = 1.0f / (float)src_h;
    f[8] = (float)dst_w;
    f[9] = (float)dst_h;
    f[10] = (float)filter;
    f[11] = 0.0f;
    g_ctx->Unmap(g_cb, 0);
    return true;
}

/* ONE DRAW, and the black bars are the clear. Everything outside the fitted
   rectangle is whatever ClearRenderTargetView left there, which is the same
   black the GDI path paints with four PatBlts. */
void draw_quad(ID3D11RenderTargetView *rtv, int target_w, int target_h,
               int filter, ID3D11ShaderResourceView *srv = 0)
{
    if (!srv) srv = g_srv;
    const float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    g_ctx->ClearRenderTargetView(rtv, black);
    g_ctx->OMSetRenderTargets(1, &rtv, 0);
    g_ctx->OMSetBlendState(0, 0, 0xffffffffu);
    g_ctx->OMSetDepthStencilState(0, 0);
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = (float)target_w;
    vp.Height = (float)target_h;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    g_ctx->RSSetViewports(1, &vp);
    g_ctx->RSSetState(g_rast);
    g_ctx->IASetInputLayout(0);
    g_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    g_ctx->VSSetShader(g_vs, 0, 0);
    g_ctx->VSSetConstantBuffers(0, 1, &g_cb);
    g_ctx->PSSetShader(g_ps, 0, 0);
    g_ctx->PSSetConstantBuffers(0, 1, &g_cb);
    g_ctx->PSSetShaderResources(0, 1, &srv);
    ID3D11SamplerState *smp = (filter == 0) ? g_smp_point : g_smp_linear;
    g_ctx->PSSetSamplers(0, 1, &smp);
    g_ctx->Draw(4, 0);
    /* the view may be the card renderer's own composite target, which is a
       render target again on the next picture: do not leave it bound */
    ID3D11ShaderResourceView *none = 0;
    g_ctx->PSSetShaderResources(0, 1, &none);
}

/* ---- the swap chain ------------------------------------------------------ */

void release_swapchain()
{
    if (g_scrtv) { g_scrtv->Release(); g_scrtv = 0; }
    if (g_sc1) { g_sc1->Release(); g_sc1 = 0; }
    if (g_sc) { g_sc->Release(); g_sc = 0; }
    g_sc_w = g_sc_h = 0;
    g_sc_hwnd = 0;
}

bool make_backbuffer_view()
{
    if (g_scrtv) { g_scrtv->Release(); g_scrtv = 0; }
    ID3D11Texture2D *bb = 0;
    HRESULT hr = g_sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&bb);
    if (FAILED(hr) || !bb) { fall_back("no back buffer", hr); return false; }
    hr = g_dev->CreateRenderTargetView(bb, 0, &g_scrtv);
    bb->Release();
    if (FAILED(hr) || !g_scrtv) { fall_back("no back-buffer view", hr); return false; }
    return true;
}

/* The swap chain, reached through the device's own IDXGIDevice so this program
   never names dxgi.dll. Flip model with two buffers first, which is what a
   modern compositor wants; the older blit model is the fallback for a Windows
   that has no IDXGIFactory2. */
bool make_swapchain(HWND hwnd, int w, int h)
{
    IDXGIDevice *dxdev = 0;
    if (FAILED((g_sc_dev ? g_sc_dev : g_dev)->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxdev)) ||
        !dxdev) { fall_back("the device has no DXGI side", 0); return false; }
    IDXGIAdapter *ad = 0;
    if (FAILED(dxdev->GetAdapter(&ad)) || !ad) {
        dxdev->Release();
        fall_back("no adapter", 0);
        return false;
    }
    IDXGIFactory2 *f2 = 0;
    IDXGIFactory *f1 = 0;
    HRESULT hr = ad->GetParent(__uuidof(IDXGIFactory2), (void **)&f2);
    if (FAILED(hr) || !f2) {
        f2 = 0;
        hr = ad->GetParent(__uuidof(IDXGIFactory), (void **)&f1);
    }
    ad->Release();
    dxdev->Release();
    if (!f2 && !f1) { fall_back("no DXGI factory", hr); return false; }

    if (f2) {
        DXGI_SWAP_CHAIN_DESC1 d;
        memset(&d, 0, sizeof d);
        d.Width = (UINT)w;
        d.Height = (UINT)h;
        d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        d.SampleDesc.Count = 1;
        d.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        d.BufferCount = 2;
        d.Scaling = DXGI_SCALING_STRETCH;
        d.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        d.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        hr = f2->CreateSwapChainForHwnd(g_sc_dev ? g_sc_dev : g_dev, hwnd, &d, 0, 0, &g_sc1);
        if (FAILED(hr) || !g_sc1) {
            /* an old compositor refuses the flip model; the blit model is
               still a swap chain and still vsyncs */
            d.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
            d.BufferCount = 1;
            hr = f2->CreateSwapChainForHwnd(g_sc_dev ? g_sc_dev : g_dev, hwnd, &d, 0, 0, &g_sc1);
        }
        if (SUCCEEDED(hr) && g_sc1)
            g_sc1->QueryInterface(__uuidof(IDXGISwapChain), (void **)&g_sc);
        /* the compositor's own alt-enter would take the window fullscreen
           behind this program's back; F12 is the port's fullscreen and the
           only one */
        f2->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        f2->Release();
    } else {
        DXGI_SWAP_CHAIN_DESC d;
        memset(&d, 0, sizeof d);
        d.BufferDesc.Width = (UINT)w;
        d.BufferDesc.Height = (UINT)h;
        d.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        d.SampleDesc.Count = 1;
        d.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        d.BufferCount = 1;
        d.OutputWindow = hwnd;
        d.Windowed = TRUE;
        d.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        hr = f1->CreateSwapChain(g_sc_dev ? g_sc_dev : g_dev, &d, &g_sc);
        f1->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        f1->Release();
    }
    if (!g_sc) { fall_back("no swap chain", hr); return false; }
    g_sc_hwnd = hwnd;
    g_sc_w = w;
    g_sc_h = h;
    return make_backbuffer_view();
}

/* ---- the surface window and the rebuild (run after093, lane MGFREEZE2) ---
   See the banner over RECOVERIES_MAX for why they exist. */

typedef BOOL (WINAPI *ShowWindow_t)(HWND, int);
typedef BOOL (WINAPI *SetWindowPos_t)(HWND, HWND, int, int, int, int, UINT);
typedef LONG (WINAPI *SetWindowLongA_t)(HWND, int, LONG);
typedef LRESULT (WINAPI *CallWindowProcA_t)(WNDPROC, HWND, UINT, WPARAM, LPARAM);
typedef BOOL (WINAPI *GetClientRect_t)(HWND, LPRECT);
typedef LONG (WINAPI *GetWindowLongA_t)(HWND, int);
DefWindowProcA_t g_defproc;
ShowWindow_t g_showwin;
SetWindowPos_t g_setpos;
CreateWindowExA_t g_crewin;
SetWindowLongA_t g_setlong;
CallWindowProcA_t g_callproc;
GetClientRect_t g_getclient;
GetWindowLongA_t g_getlong;
int g_child_class;      /* 1 registered, -1 it could not be: the swap chain goes on the game window */
void hook_parent(HWND parent);
int g_child_w, g_child_h;       /* the size the surface window was last given */
HWND g_hooked;                  /* the game window whose size changes this file hears */
typedef BOOL (WINAPI *DestroyWindow_t)(HWND);
DestroyWindow_t g_destroywin;
/* THE OLD SURFACE WINDOWS ARE CLOSED (run after093, lane MGFREEZE3): each
   rebuild hid the one it gave up and kept it until the game window closed.
   They are destroyed once the new surface has shown CLOSE_AFTER pictures. */
enum { OLD_KIDS_MAX = 32, CLOSE_AFTER = 3 };
HWND g_old_kid[OLD_KIDS_MAX];
int g_old_kids;
int g_kids_closed;
WNDPROC g_hooked_proc;          /* its own window procedure, called after */

LRESULT CALLBACK surface_proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    /* the game window answers the mouse; this window only carries the picture */
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    if (m == WM_ERASEBKGND) return 1;
    return g_defproc ? g_defproc(h, m, w, l) : 0;
}

/* The child that carries the swap chain: the game window's whole client area,
   disabled (so it never takes the keyboard or a click) and transparent to the
   mouse's hit test (so the cursor and the touch screen stay the game
   window's). user32 hand-loaded, the rule this file keeps. */
HWND make_child(HWND parent, int w, int h)
{
    /* SM64DS_PRESENT_CHILD=0 (lane DECK1): no child, the swap chain goes on the
       game window itself, the way it did before MGFREEZE2. A diagnostic for the
       Steam Deck's blank screen (Wine draws a child window's Vulkan picture by
       a different path than a top-level one's); a lost card then cannot be
       rebuilt on a fresh window and ends on the ordinary Windows path. */
    static int no_child = -1;
    if (no_child < 0) {
        const char *e = getenv("SM64DS_PRESENT_CHILD");
        no_child = e && e[0] == '0' && !e[1];
        if (no_child) fputs("[present] SM64DS_PRESENT_CHILD=0: the swap chain goes on the game window itself\n", stderr);
    }
    if (no_child) return 0;
    if (g_child_class < 0 || !g_user32) return 0;
    if (!g_child_class) {
        RegisterClassA_t reg = (RegisterClassA_t)GetProcAddress(g_user32, "RegisterClassA");
        g_crewin = (CreateWindowExA_t)GetProcAddress(g_user32, "CreateWindowExA");
        g_defproc = (DefWindowProcA_t)GetProcAddress(g_user32, "DefWindowProcA");
        g_showwin = (ShowWindow_t)GetProcAddress(g_user32, "ShowWindow");
        g_setpos = (SetWindowPos_t)GetProcAddress(g_user32, "SetWindowPos");
        g_setlong = (SetWindowLongA_t)GetProcAddress(g_user32, "SetWindowLongA");
        g_callproc = (CallWindowProcA_t)GetProcAddress(g_user32, "CallWindowProcA");
        g_getclient = (GetClientRect_t)GetProcAddress(g_user32, "GetClientRect");
        g_getlong = (GetWindowLongA_t)GetProcAddress(g_user32, "GetWindowLongA");
        g_destroywin = (DestroyWindow_t)GetProcAddress(g_user32, "DestroyWindow");
        WNDCLASSA wc;
        memset(&wc, 0, sizeof wc);
        wc.lpfnWndProc = surface_proc;
        wc.hInstance = GetModuleHandleA(0);
        wc.lpszClassName = "sm64ds_card_surface";
        if (!reg || !g_crewin || !g_defproc || !g_showwin || !g_setpos || !reg(&wc)) {
            g_child_class = -1;
            fputs("[present] no surface window class; the swap chain goes on the game window\n", stderr);
            return 0;
        }
        g_child_class = 1;
    }
    HWND c = g_crewin(WS_EX_NOPARENTNOTIFY, "sm64ds_card_surface", "",
                      WS_CHILD | WS_VISIBLE | WS_DISABLED | WS_CLIPSIBLINGS,
                      0, 0, w, h, parent, 0, GetModuleHandleA(0), 0);
    if (c) {
        g_child_w = w;
        g_child_h = h;
        hook_parent(parent);
    }
    return c;
}

void child_hide()
{
    if (g_child && g_showwin) g_showwin(g_child, SW_HIDE);
}

void child_size(int w, int h)
{
    if (!g_child || !g_setpos || w <= 0 || h <= 0) return;
    if (w == g_child_w && h == g_child_h) return;
    g_child_w = w;
    g_child_h = h;
    g_setpos(g_child, 0, 0, 0, w, h,
             SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

/* THE SURFACE WINDOW FOLLOWS THE GAME WINDOW'S SIZE AT ONCE (run after093,
   lane MGFREEZE3). Sized only when the next picture was presented, the
   surface window stayed at the old size inside a window that had just gone
   fullscreen, maximised or been handed over, and the game window's own white
   surface showed around it until a picture came (up to half a second on a
   loaded box). The game window's size changes are heard here instead, in the
   message that reports them, before the game window's own procedure runs:
   WM_WINDOWPOSCHANGED for every move and size (F12, StartFullscreen, the
   hand-off's placement, a maximise, a border drag) and WM_SIZE behind it. The
   rectangle is the client area, the one the present path sizes the swap chain
   to. A minimised window is left alone.
   EARLIER STILL, IN WM_NCCALCSIZE: WM_WINDOWPOSCHANGED comes after the game
   window's new rectangle is already in place and after its frame is
   repainted, so a size taken there left the surface window one step behind
   for a few milliseconds (seen as one 10 ms sample of a rectangle sampler).
   WM_NCCALCSIZE is the step that computes the new client rectangle, just
   before the window manager puts it in place: the surface window takes that
   size there, and WM_WINDOWPOSCHANGED only confirms it. */
LRESULT CALLBACK parent_size_hook(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_NCCALCSIZE && w && l && h == g_hooked && h == g_parent && g_child &&
        !(g_getlong && (g_getlong(h, GWL_STYLE) & WS_MINIMIZE))) {
        const LRESULT r = g_callproc(g_hooked_proc, h, m, w, l);
        const RECT &rc = ((const NCCALCSIZE_PARAMS *)l)->rgrc[0];
        child_size(rc.right - rc.left, rc.bottom - rc.top);
        return r;
    }
    if (h == g_hooked && h == g_parent && g_child) {
        if (m == WM_WINDOWPOSCHANGED && g_getclient) {
            const WINDOWPOS *wp = (const WINDOWPOS *)l;
            if (!wp || !(wp->flags & SWP_NOSIZE) || (wp->flags & SWP_FRAMECHANGED)) {
                RECT rc;
                if (g_getclient(h, &rc)) child_size(rc.right - rc.left, rc.bottom - rc.top);
            }
        } else if (m == WM_SIZE && w != SIZE_MINIMIZED) {
            child_size((int)LOWORD(l), (int)HIWORD(l));
        }
    }
    return g_callproc(g_hooked_proc, h, m, w, l);
}

/* Once per game window, the first time a surface window is made on it. A DIFFERENT game window in the same process (a window
   rebuilt, SEAMCLEAN1) is hooked too: the old one is let go first (its procedure put back if ours is still the top one; a window
   that is already destroyed answers nothing and costs nothing). */
void hook_parent(HWND parent)
{
    if (g_hooked == parent || !g_setlong || !g_callproc || !g_getclient) return;
    if (g_hooked) {
        if (g_getlong && g_hooked_proc &&
            (LONG)g_getlong(g_hooked, GWL_WNDPROC) == (LONG)(LONG_PTR)parent_size_hook)
            g_setlong(g_hooked, GWL_WNDPROC, (LONG)(LONG_PTR)g_hooked_proc);
        g_hooked = 0;
        g_hooked_proc = 0;
    }
    const LONG old = g_setlong(parent, GWL_WNDPROC, (LONG)(LONG_PTR)parent_size_hook);
    if (!old) return;
    g_hooked_proc = (WNDPROC)(LONG_PTR)old;
    g_hooked = parent;
}

/* Everything the lost path showed through is left alone: the presenter's
   thread is stopped and its ring freed as before any resize, then its device,
   the swap chain, its view and the child window are abandoned (the child
   hidden), never released and never flushed. The next picture makes them all
   again through the path the first picture took. */
void abandon_card_path()
{
    presenter_abandon_device();
    /* STILL ABANDONED, MEASURED (run after093, lane MGFREEZE3): releasing the
       old chain and the presenter's old device from the presenter thread once
       the new chain had shown three pictures (no flush after) crashed the AMD
       driver's worker thread on a loss that kept the shared device (one run of
       fourteen, the same signature as MGFREEZE1's). So they stay abandoned,
       and the cost is measured instead: the line at the rebuild cap gives it,
       about 10 to 12 MB a rebuild for a loss that keeps the shared device and
       36 to 39 MB for a removed card's drill from a minigame screen, measured
       on both cards here, and about 80 MB for the same drill in a level
       (MGFREEZE3's review; SEAMCLEAN1 prints the measured figure on each
       rebuild's line and caps a level at RECOVERIES_IN_LEVEL_MAX). */
    child_hide();
    if (g_child && g_old_kids < OLD_KIDS_MAX) g_old_kid[g_old_kids++] = g_child;
    g_child = 0;
    g_scrtv = 0;
    g_sc1 = 0;
    g_sc = 0;
    g_sc_w = g_sc_h = 0;
    g_sc_hwnd = 0;
    g_sc_dev = 0;
    g_parent = 0;
    g_pres_off = 0;
    g_up_refused_run = 0;
    if (!g_renew && g_tex) {
        /* the upload texture, and every copy the driver keeps of it, goes;
           the next picture makes it again */
        if (g_srv) { g_srv->Release(); g_srv = 0; }
        g_tex->Release();
        g_tex = 0;
        g_tex_w = g_tex_h = 0;
    }
}

/* THE CARD MEMORY A REBUILD COSTS (run after093, lane MGFREEZE3): this
   process's usage of the card's own memory and of system memory the card
   maps, before the loss and CLOSE_AFTER * 10 pictures after the rebuild, so
   what a rebuild keeps is a number in the log, not a guess. */
IDXGIAdapter3 *g_use_ad3;
ID3D11Device *g_use_dev;
bool card_usage(double *lo_mb, double *nl_mb)
{
    *lo_mb = *nl_mb = 0.0;
    if (!g_dev) return false;
    if (g_use_dev != g_dev) {
        g_use_dev = g_dev;
        g_use_ad3 = 0;
        IDXGIDevice *dx = 0;
        if (SUCCEEDED(g_dev->QueryInterface(__uuidof(IDXGIDevice), (void **)&dx)) && dx) {
            IDXGIAdapter *ad = 0;
            if (SUCCEEDED(dx->GetAdapter(&ad)) && ad) {
                ad->QueryInterface(__uuidof(IDXGIAdapter3), (void **)&g_use_ad3);
                ad->Release();
            }
            dx->Release();
        }
    }
    if (!g_use_ad3) return false;
    DXGI_QUERY_VIDEO_MEMORY_INFO lo, nl;
    memset(&lo, 0, sizeof lo);
    memset(&nl, 0, sizeof nl);
    if (FAILED(g_use_ad3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &lo)) ||
        FAILED(g_use_ad3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nl)))
        return false;
    *lo_mb = (double)lo.CurrentUsage / (1024.0 * 1024.0);
    *nl_mb = (double)nl.CurrentUsage / (1024.0 * 1024.0);
    return true;
}
double g_use_lo0, g_use_nl0;    /* before the loss */
int g_use_have0;
int g_since_rebuild = -1;       /* pictures since the last rebuild, -1 for none pending */
double g_kept_mb;               /* what the measured rebuilds kept, local + non-local */
int g_kept_n;

/* Once a picture: the old surface windows closed, and the cost measured. */
void after_rebuild_tick()
{
    if (g_since_rebuild < 0) return;
    ++g_since_rebuild;
    if (g_since_rebuild == CLOSE_AFTER && g_old_kids && g_destroywin) {
        int n = 0;
        for (int i = 0; i < g_old_kids; ++i)
            if (g_old_kid[i] && g_destroywin(g_old_kid[i])) ++n;
        g_kids_closed += n;
        fprintf(stderr, "[present] %d old surface window(s) closed (%d this run)\n", n, g_kids_closed);
        g_old_kids = 0;
    }
    if (g_since_rebuild == CLOSE_AFTER * 10) {
        g_since_rebuild = -1;
        double lo, nl;
        if (g_use_have0 && card_usage(&lo, &nl)) {
            g_kept_mb += (lo - g_use_lo0) + (nl - g_use_nl0);
            ++g_kept_n;
            fprintf(stderr, "[present] rebuild %d: card memory local %.1f MB (%+.1f), non-local "
                    "%.1f MB (%+.1f) against before the loss; this rebuild kept %.1f MB (%s)\n", g_recoveries, lo, lo - g_use_lo0,
                    nl, nl - g_use_nl0, (lo - g_use_lo0) + (nl - g_use_nl0), port_scene_live_id() < 0 ? "in a level" : "on a scene");
        }
    }
}

/* True when the loss is taken up by a rebuild; false gives the run up. */
bool lose(const char *why, HRESULT hr)
{
    if (g_lost) {
        if (++g_rebuild_fails >= REBUILD_FAILS_MAX) {
            fprintf(stderr, "[present] the graphics card path could not be rebuilt (%s, "
                    "code %08x, %d tries in a row over %.0f ms)\n", why, (unsigned)hr, g_rebuild_fails,
                    ms_between(g_lost_at, qpc()));
            g_giving_up = 1;
            return false;
        }
        const int wait = rebuild_wait_ms(g_rebuild_fails);
        g_retry_at = qpc() + (g_qpf ? (long long)wait * g_qpf / 1000 : 0);
        fprintf(stderr, "[present] rebuild try %d did not work (%s, code %08x); the next one in %d ms\n",
                g_rebuild_fails, why, (unsigned)hr, wait);
    } else {
        if (g_recoveries >= recoveries_cap()) {
            if (g_kept_n)
                fprintf(stderr, "[present] the graphics card path was lost again (%s, code %08x) "
                        "after %d rebuilds (each kept about %.1f MB of card memory, measured)\n",
                        why, (unsigned)hr, g_recoveries, g_kept_mb / g_kept_n);
            else
                fprintf(stderr, "[present] the graphics card path was lost again (%s, code %08x) "
                        "after %d rebuilds\n", why, (unsigned)hr, g_recoveries);
            g_giving_up = 1;
            return false;
        }
        g_lost = 1;
        g_lost_hr = hr;
        g_lost_at = qpc();
        g_rebuild_fails = 0;
        g_retry_at = 0;
        g_use_have0 = card_usage(&g_use_lo0, &g_use_nl0) ? 1 : 0;
        g_since_rebuild = -1;
        fprintf(stderr, "[present] the graphics card path was lost (%s, code %08x); "
                "rebuilding it\n", why, (unsigned)hr);
    }
    /* the card itself removed or reset: nothing made on the shared device works */
    if (g_dev && g_dev->GetDeviceRemovedReason() != S_OK) g_renew = 1;
    abandon_card_path();
    return true;
}

void recovered()
{
    if (!g_lost) return;
    g_lost = 0;
    g_rebuild_fails = 0;
    g_retry_at = 0;
    ++g_recoveries;
    g_since_rebuild = 0;
#ifndef PORT_ROM_CLEAN
    if (g_lost_drill_every > 0 && g_lost_drill_at < 0) g_lost_drill_at = g_presents + g_lost_drill_every;
#endif
    fprintf(stderr, "[present] the graphics card path was lost (code %08x) and came back "
            "after %.0f ms\n", (unsigned)g_lost_hr, ms_between(g_lost_at, qpc()));
    /* the renderer went to the processor with the old device: it makes its
       card objects again on the new one from its next frame (MGFREEZE3) */
    if (g_tell_renderer) {
        g_tell_renderer = 0;
        port_gpu_raster_device_renewed();
    }
}

/* The renderer heard first that the card is gone (hal/gpu_raster.cpp). */
void lost_hook(long hr)
{
    if (g_down || !g_dev) return;
    g_renew = 1;
    fall_back("the graphics card was lost", (HRESULT)hr);
}

/* The rebuild's first step when the card itself was lost: the renderer draws
   on the processor from here on (any lost card does that), everything this
   file made on the old device is abandoned with it, a new device is made on
   the same adapter, and the pipeline is made again on it. */
bool renew_device()
{
    g_renew = 0;
    port_gpu_raster_device_lost((long)(g_lost_hr ? g_lost_hr : (HRESULT)0x887A0005L));
    g_vs = 0;
    g_ps = 0;
    g_cb = 0;
    g_smp_point = g_smp_linear = 0;
    g_rast = 0;
    g_tex = 0;
    g_srv = 0;
    g_tex_w = g_tex_h = 0;
    memset(g_fence, 0, sizeof g_fence);
    g_fence_head = g_fence_n = 0;
    g_off_rt = g_off_stage = 0;
    g_off_rtv = 0;
    g_off_rt_w = g_off_rt_h = 0;
#ifndef PORT_ROM_CLEAN
    g_ad3 = 0;
    g_ad3_tried = 0;
#endif
    if (!port_gpu_device_renew()) {
        fall_back("no new Direct3D device would start", 0);
        return false;
    }
    g_dev = (ID3D11Device *)port_gpu_device();
    g_ctx = (ID3D11DeviceContext *)port_gpu_device_context();
    if (!g_dev || !g_ctx) { fall_back("the new Direct3D device went missing", 0); return false; }
    if (!make_pipeline()) return false;
    g_tell_renderer = 1;
    return true;
}

/* ---- the offscreen proof path -------------------------------------------- */

bool ensure_offscreen(int w, int h)
{
    if (g_off_rt && g_off_rt_w == w && g_off_rt_h == h) return true;
    if (g_off_rtv) { g_off_rtv->Release(); g_off_rtv = 0; }
    if (g_off_stage) { g_off_stage->Release(); g_off_stage = 0; }
    if (g_off_rt) { g_off_rt->Release(); g_off_rt = 0; }
    g_off_rt_w = g_off_rt_h = 0;

    D3D11_TEXTURE2D_DESC d;
    memset(&d, 0, sizeof d);
    d.Width = (UINT)w;
    d.Height = (UINT)h;
    d.MipLevels = 1;
    d.ArraySize = 1;
    d.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    d.SampleDesc.Count = 1;
    d.Usage = D3D11_USAGE_DEFAULT;
    d.BindFlags = D3D11_BIND_RENDER_TARGET;
    HRESULT hr = g_dev->CreateTexture2D(&d, 0, &g_off_rt);
    if (FAILED(hr) || !g_off_rt) { fall_back("no offscreen target", hr); return false; }
    hr = g_dev->CreateRenderTargetView(g_off_rt, 0, &g_off_rtv);
    if (FAILED(hr) || !g_off_rtv) { fall_back("no offscreen view", hr); return false; }
    D3D11_TEXTURE2D_DESC sd = d;
    sd.Usage = D3D11_USAGE_STAGING;
    sd.BindFlags = 0;
    sd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    hr = g_dev->CreateTexture2D(&sd, 0, &g_off_stage);
    if (FAILED(hr) || !g_off_stage) { fall_back("no readback texture", hr); return false; }
    g_off_rt_w = w;
    g_off_rt_h = h;
    return true;
}

/* THE CPU REFERENCE, so "smooth" is a number and not an opinion. It is the
   same arithmetic the shader and the sampler do, spelled in C: the sample
   point is the destination pixel's centre mapped into the source, filter 2
   warps that point the way the sharp filter does, and then the texel is
   either picked (filter 0) or four are blended with clamped edges (filters 1
   and 2), which is what the two sampler states do.
   EXACTNESS IS EXPECTED OF FILTER 0 AND NOT OF THE OTHER TWO: a point sample
   is a choice and cannot round, while Direct3D guarantees only eight bits of
   sub-texel precision on a linear blend's weights. So filter 0 is compared
   for equality and the smooth pair reports the worst single-channel
   difference, which is the honest measure of agreement there. */
unsigned reference_pixel(const unsigned char *src, int stride_px, int sw, int sh,
                         int x, int y, int dw, int dh, int filter)
{
    double u = ((double)x + 0.5) / (double)dw;
    double v = ((double)y + 0.5) / (double)dh;
    if (filter == 0) {
        /* a point sampler's own convention: the texel the coordinate lands in */
        int px = (int)floor(u * sw), py = (int)floor(v * sh);
        if (px < 0) px = 0;
        if (px > sw - 1) px = sw - 1;
        if (py < 0) py = 0;
        if (py > sh - 1) py = sh - 1;
        return ((const unsigned *)src)[(size_t)py * stride_px + px];
    }
    if (filter >= 2) {
        const double tx = u * sw, ty = v * sh;
        const double fx = floor(tx), fy = floor(ty);
        const double sx = tx - fx, sy = ty - fy;
        double scx = floor((double)dw / (double)sw);
        double scy = floor((double)dh / (double)sh);
        if (scx < 1.0) scx = 1.0;
        if (scy < 1.0) scy = 1.0;
        const double rx = 0.5 - 0.5 / scx, ry = 0.5 - 0.5 / scy;
        double dx = sx - 0.5, dy = sy - 0.5;
        double cx = dx < -rx ? -rx : (dx > rx ? rx : dx);
        double cy = dy < -ry ? -ry : (dy > ry ? ry : dy);
        u = (fx + (dx - cx) * scx + 0.5) / (double)sw;
        v = (fy + (dy - cy) * scy + 0.5) / (double)sh;
    }
    /* a linear sampler's own convention: texel centres at (i + 0.5) / size */
    const double tx = u * sw - 0.5, ty = v * sh - 0.5;
    const double fx = floor(tx), fy = floor(ty);
    const double ax = tx - fx, ay = ty - fy;
    int x0 = (int)fx, y0 = (int)fy, x1 = x0 + 1, y1 = y0 + 1;
    if (x0 < 0) x0 = 0; if (x0 > sw - 1) x0 = sw - 1;
    if (x1 < 0) x1 = 0; if (x1 > sw - 1) x1 = sw - 1;
    if (y0 < 0) y0 = 0; if (y0 > sh - 1) y0 = sh - 1;
    if (y1 < 0) y1 = 0; if (y1 > sh - 1) y1 = sh - 1;
    unsigned out = 0;
    for (int c = 0; c < 4; ++c) {
        const double p00 = src[((size_t)y0 * stride_px + x0) * 4 + c];
        const double p10 = src[((size_t)y0 * stride_px + x1) * 4 + c];
        const double p01 = src[((size_t)y1 * stride_px + x0) * 4 + c];
        const double p11 = src[((size_t)y1 * stride_px + x1) * 4 + c];
        const double top = p00 + (p10 - p00) * ax;
        const double bot = p01 + (p11 - p01) * ax;
        double val = top + (bot - top) * ay;
        if (val < 0.0) val = 0.0;
        if (val > 255.0) val = 255.0;
        out |= (unsigned)((int)(val + 0.5) & 0xff) << (c * 8);
    }
    return out;
}

/* A PICTURE FOR SOMEBODY TO LOOK AT, because the one thing this setting is
   for -- how the picture looks stretched -- is the one thing a number cannot
   settle. SM64DS_PRESENT_OFFSCREEN_BMP=<name> writes the frame that was just
   read back, as an ordinary 32-bit bitmap, so three runs at the three filter
   settings produce three files that can be opened side by side without a
   window ever appearing. Overwritten on every check, so the file is the LAST
   checked frame of the run, which is the same frame in every arm when the
   frame count and the check interval are. Off unless the variable is set, and
   the file goes wherever the caller names, which in every run here is beside
   the exe in the build directory and never into the repository. */
void offscreen_bmp(const unsigned char *pixels, size_t pitch, int w, int h)
{
    const char *name = getenv("SM64DS_PRESENT_OFFSCREEN_BMP");
    if (!name || !*name) return;
    char numbered[512];
    if (strchr(name, '%')) {          /* INTROCARD092: a %d in the name writes every checked frame under its own number (frame-by-frame proof rows) */
        snprintf(numbered, sizeof numbered, name, g_off_checked);
        name = numbered;
    }
    FILE *f = fopen(name, "wb");
    if (!f) return;
    const unsigned rowbytes = (unsigned)w * 4;
    const unsigned bits = rowbytes * (unsigned)h;
    unsigned char hdr[54];
    memset(hdr, 0, sizeof hdr);
    hdr[0] = 'B'; hdr[1] = 'M';
    const unsigned total = 54 + bits;
    memcpy(hdr + 2, &total, 4);
    const unsigned off = 54;
    memcpy(hdr + 10, &off, 4);
    const unsigned isz = 40;
    memcpy(hdr + 14, &isz, 4);
    const int iw = w, ih = -h;             /* negative height: top-down rows */
    memcpy(hdr + 18, &iw, 4);
    memcpy(hdr + 22, &ih, 4);
    const unsigned short planes = 1, bpp = 32;
    memcpy(hdr + 26, &planes, 2);
    memcpy(hdr + 28, &bpp, 2);
    memcpy(hdr + 34, &bits, 4);
    fwrite(hdr, 1, sizeof hdr, f);
    for (int y = 0; y < h; ++y)
        fwrite(pixels + (size_t)y * pitch, 1, rowbytes, f);
    fclose(f);
    if (g_off_checked == 1)
        fprintf(stderr, "[present] offscreen picture written: %dx%d, filter "
                "%d, 32-bit bitmap\n", w, h, g_filter);
}

void offscreen_compare(const unsigned char *src, int stride_px, int sw, int sh,
                       int dw, int dh)
{
    g_ctx->CopyResource(g_off_stage, g_off_rt);
    D3D11_MAPPED_SUBRESOURCE m;
    memset(&m, 0, sizeof m);
    const HRESULT hr = g_ctx->Map(g_off_stage, 0, D3D11_MAP_READ, 0, &m);
    if (FAILED(hr) || !m.pData) { fall_back("the readback was refused", hr); return; }
    ++g_off_checked;
    unsigned bad_rgb = 0, bad_a = 0, ties = 0;
    int maxerr = 0;
    /* THE EXACT ARM: filter 0 into a target that is the source size is a copy,
       and filter 0 into a whole-number multiple is that copy with each source
       pixel repeated in a block, so both are byte comparisons and the block
       shape is asserted as well as the colour. Filter 0 into any other size is
       still exact, it just has to go through the reference's own nearest
       arithmetic to say which texel each pixel should have picked. */
    const int exact_arm = (g_filter == 0);
    const int block = exact_arm && (dw % sw == 0) && (dh % sh == 0);
    for (int y = 0; y < dh; ++y) {
        const unsigned *row =
            (const unsigned *)((const unsigned char *)m.pData + (size_t)y * m.RowPitch);
        for (int x = 0; x < dw; ++x) {
            unsigned want;
            if (block) {
                const int sxi = x / (dw / sw), syi = y / (dh / sh);
                want = ((const unsigned *)src)[(size_t)syi * stride_px + sxi];
            } else {
                want = reference_pixel(src, stride_px, sw, sh, x, y, dw, dh,
                                       g_filter);
            }
            const unsigned got = row[x];
            if (got == want) continue;
            /* A TEXEL BOUNDARY TIE, and only filter 0 at a scale that is not a
               whole number can have one. The destination pixel's centre lands
               EXACTLY on the line between two source pixels, and which side of
               that line it falls on is decided in the hardware's fixed-point
               texture coordinates and here in double precision. The two are
               allowed to disagree by one step there, and when they do the
               colour that comes back is a NEIGHBOURING pixel's -- so it looks
               like an enormous error on a channel while being the correct
               answer to a question with two correct answers. Counted and
               reported rather than swallowed: a pixel that matches none of the
               nine around its own is a real defect and still counts as one.
               Measured on this tree at 1027x578 from 512x384: 173 of 593,606
               pixels on the software device, 884 on the card. */
            if (exact_arm && !block) {
                const int px = (int)floor(((double)x + 0.5) / dw * sw);
                const int py = (int)floor(((double)y + 0.5) / dh * sh);
                int tie = 0;
                for (int ny = py - 1; ny <= py + 1 && !tie; ++ny) {
                    for (int nx = px - 1; nx <= px + 1 && !tie; ++nx) {
                        int cx = nx, cy = ny;
                        if (cx < 0) cx = 0;
                        if (cx > sw - 1) cx = sw - 1;
                        if (cy < 0) cy = 0;
                        if (cy > sh - 1) cy = sh - 1;
                        if (((const unsigned *)src)[(size_t)cy * stride_px + cx]
                            == got)
                            tie = 1;
                    }
                }
                if (tie) { ++ties; continue; }
            }
            if ((got & 0x00ffffffu) != (want & 0x00ffffffu)) ++bad_rgb;
            else ++bad_a;
            for (int c = 0; c < 4; ++c) {
                const int a = (int)((got >> (c * 8)) & 0xff);
                const int b = (int)((want >> (c * 8)) & 0xff);
                const int e = a > b ? a - b : b - a;
                if (e > maxerr) maxerr = e;
            }
        }
    }
    offscreen_bmp((const unsigned char *)m.pData, m.RowPitch, dw, dh);
    g_ctx->Unmap(g_off_stage, 0);
    /* WHAT COUNTS AS A DISAGREEMENT, and it is not the same question for the
       three filters. Filter 0 picks a texel and cannot round, so one differing
       byte is a real defect. Filters 1 and 2 blend, and Direct3D promises only
       eight bits of sub-texel precision on the weights, so the hardware and
       this double-precision reference are ALLOWED to land a step or two apart
       on a channel: what would be a defect there is a DIFFERENT PIXEL, not a
       different rounding. The frame counts below therefore count frames whose
       worst channel error is over the tolerance, and the tolerance is 0 for
       filter 0 and 2 for the smooth pair. */
    const int tol = exact_arm ? 0 : 2;
    if ((int)maxerr > tol) ++g_off_bad;
    if (bad_rgb > g_off_worst_rgb) g_off_worst_rgb = bad_rgb;
    if (bad_a > g_off_worst_a) g_off_worst_a = bad_a;
    if (maxerr > g_off_maxerr) g_off_maxerr = maxerr;
    if (ties > g_off_worst_tie) g_off_worst_tie = ties;
    g_off_tol = tol;
    if (g_off_checked == 1)
        fprintf(stderr, "[present] offscreen first check: %dx%d source into a "
                "%dx%d target, filter %d, %s comparison: %u pixel(s) differ in "
                "red, green or blue, %u in alpha only, %u on a texel boundary, "
                "worst channel error %d\n",
                sw, sh, dw, dh, g_filter,
                block ? "exact (byte for byte, whole-number blocks)"
                      : (exact_arm ? "exact (byte for byte, against the CPU "
                                     "reference's own nearest pick)"
                                   : "against the CPU reference"),
                bad_rgb, bad_a, ties, maxerr);
}

/* ---- the rectangle, checked against the one function that owns it ---------
   The destination rectangle is NOT computed here: hal_present_fit computes it
   once, the present path hands the four numbers over, and this file turns them
   into the two corners the shader wants. That conversion is the only place a
   second copy of the arithmetic could creep in, so this check does it and
   turns the corners back into pixels. Identical numbers at every client size
   is the whole claim. Runs on demand (SM64DS_PRESENT_RECTCHECK), never in a
   player's run. */
void rect_check(int src_w, int src_h)
{
    const char *e = getenv("SM64DS_PRESENT_RECTCHECK");
    if (!e || !*e || !strcmp(e, "0")) return;
    struct WH { int w, h; };
    WH sizes[8];
    int n = 0;
    if (!strcmp(e, "1")) {
        /* a 4:3 one, a 16:9 one and a tall one */
        static const WH d[3] = { { 1024, 768 }, { 1920, 1080 }, { 768, 1280 } };
        for (int i = 0; i < 3; ++i) sizes[n++] = d[i];
    } else {
        const char *p = e;
        while (*p && n < 8) {
            int w = 0, h = 0;
            while (*p >= '0' && *p <= '9') { w = w * 10 + (*p - '0'); ++p; }
            if (*p == 'x' || *p == 'X') ++p;
            while (*p >= '0' && *p <= '9') { h = h * 10 + (*p - '0'); ++p; }
            if (w > 0 && h > 0) { sizes[n].w = w; sizes[n].h = h; ++n; }
            if (*p == ',') ++p; else break;
        }
    }
    int bad = 0;
    for (int i = 0; i < n; ++i) {
        const int cw = sizes[i].w, ch = sizes[i].h;
        int x = 0, y = 0, w = 0, h = 0;
        hal_present_fit(cw, ch, src_w, src_h, &x, &y, &w, &h);
        /* set_params' own arithmetic, spelled once more and then undone */
        const float n0 = (float)x / (float)cw * 2.0f - 1.0f;
        const float n1 = 1.0f - (float)y / (float)ch * 2.0f;
        const float n2 = (float)(x + w) / (float)cw * 2.0f - 1.0f;
        const float n3 = 1.0f - (float)(y + h) / (float)ch * 2.0f;
        const int bx = (int)((n0 + 1.0f) * 0.5f * cw + 0.5f);
        const int by = (int)((1.0f - n1) * 0.5f * ch + 0.5f);
        const int bw = (int)((n2 + 1.0f) * 0.5f * cw + 0.5f) - bx;
        const int bh = (int)((1.0f - n3) * 0.5f * ch + 0.5f) - by;
        const int same = (bx == x && by == y && bw == w && bh == h);
        if (!same) ++bad;
        fprintf(stderr, "[present] rect client %dx%d source %dx%d: the present "
                "path's %d,%d %dx%d -- the graphics card's %d,%d %dx%d -- %s\n",
                cw, ch, src_w, src_h, x, y, w, h, bx, by, bw, bh,
                same ? "IDENTICAL" : "DIFFERENT");
    }
    fprintf(stderr, "[present] rect check: %d size(s), %d different VERDICT=%s\n",
            n, bad, bad ? "FAIL" : "PASS");
}

/* ---- the invisible swap-chain probe --------------------------------------
   A real flip-model swap chain, on a window that CANNOT appear: WS_POPUP with
   no WS_VISIBLE and no ShowWindow ever called, so Windows never composites it
   and it has no taskbar button. That is a stronger guarantee than "minimised",
   and it is why this probe exists instead of attaching a swap chain to the
   game's own minimised window. */
LRESULT CALLBACK probe_proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    DefWindowProcA_t f = (DefWindowProcA_t)GetProcAddress(g_user32,
                                                          "DefWindowProcA");
    return f ? f(h, m, w, l) : 0;
}

void swap_probe(const void *pixels, int stride_px, int sw, int sh)
{
    if (!g_user32) return;
    RegisterClassA_t reg = (RegisterClassA_t)GetProcAddress(g_user32,
                                                            "RegisterClassA");
    CreateWindowExA_t cre = (CreateWindowExA_t)GetProcAddress(g_user32,
                                                              "CreateWindowExA");
    DestroyWindow_t des = (DestroyWindow_t)GetProcAddress(g_user32,
                                                          "DestroyWindow");
    if (!reg || !cre || !des) {
        fputs("[present] swap probe: user32 would not answer; skipped\n", stderr);
        return;
    }
    WNDCLASSA wc;
    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc = probe_proc;
    wc.hInstance = GetModuleHandleA(0);
    wc.lpszClassName = "sm64ds_present_probe";
    reg(&wc);
    /* no WS_VISIBLE, never shown, WS_EX_TOOLWINDOW so it is not a taskbar
       entry even in principle */
    HWND h = cre(WS_EX_TOOLWINDOW, "sm64ds_present_probe", "", WS_POPUP,
                 0, 0, 1024, 768, 0, 0, wc.hInstance, 0);
    if (!h) {
        fputs("[present] swap probe: the hidden window did not open; skipped\n",
              stderr);
        return;
    }
    const int W = 1024, H = 768;
    if (make_swapchain(h, W, H) && upload(pixels, stride_px, sw, sh) &&
        set_params(0, 0, W, H, W, H, sw, sh, g_filter)) {
        draw_quad(g_scrtv, W, H, g_filter);
        const HRESULT hr = g_sc->Present(0, 0);
        fprintf(stderr, "[present] swap probe: a real %s swap chain on a window "
                "that is never shown, one %dx%d present, hr=%08x\n",
                g_sc1 ? "flip-model" : "blit-model", W, H, (unsigned)hr);
        address_rows("swapchain");
    }
    release_swapchain();
    des(h);
}

/* ---- the vsync rule ------------------------------------------------------ */

int vsync_interval(double budget_ms, int frame_rate_key)
{
    if (!g_vsync_key || g_down) return 0;
    /* rule 1: the presentation clock and a blocking present do not combine */
    if (frame_rate_key) {
        if (!g_vsync_said) {
            g_vsync_said = 1;
            fprintf(stderr, "[present] VSync stands down for this run because "
                            "FrameRate %d is set: that setting hands the same "
                            "picture to the display several times inside one "
                            "game tick, and waiting for the monitor on each of "
                            "them would spend more than the tick's whole budget "
                            "and slow the game down. The graphics card path "
                            "stays on; only the wait is off.\n", frame_rate_key);
        }
        return 0;
    }
    /* rule 2: a monitor slower than the game's own tick */
    if (budget_ms > 0.0 && g_refresh_ms > budget_ms * 1.02) {
        if (!g_vsync_said) {
            g_vsync_said = 1;
            fprintf(stderr, "[present] VSync stands down: the display refreshes "
                            "every %.2f ms and this scene wants a picture every "
                            "%.2f ms, so waiting for the monitor would make the "
                            "game itself run slowly. The graphics card path "
                            "stays on.\n", g_refresh_ms, budget_ms);
        }
        return 0;
    }
    return 1;
}

/* rule 3: the watchdog. Called once per present with the period this frame
   took, the part of it spent inside Present, and the budget that frame had. */
void vsync_watch(double period_ms, double block_ms, double budget_ms)
{
    if (!g_vsync_key || g_vsync_said || budget_ms <= 0.0) return;
    g_win_period += period_ms;
    g_win_block += block_ms;
    g_win_budget += budget_ms;
    if (++g_win_n < VSYNC_WINDOW) return;
    const double p = g_win_period / g_win_n;
    const double b = g_win_block / g_win_n;
    const double bu = g_win_budget / g_win_n;
    g_win_period = g_win_block = g_win_budget = 0.0;
    g_win_n = 0;
    /* over budget, AND it would have fitted without the wait: the wait is why */
    if (p > bu * 1.02 && p - b <= bu) {
        g_vsync_said = 1;
        g_vsync_key = 0;
        fprintf(stderr, "[present] VSync switched itself off: over the last %d "
                "pictures a frame took %.2f ms against a budget of %.2f ms, and "
                "%.2f ms of that was waiting for the monitor, so the wait was "
                "making the game run slowly. The graphics card path stays on "
                "and the picture is unchanged.\n", (int)VSYNC_WINDOW, p, bu, b);
    }
}

/* ---- shutdown ------------------------------------------------------------ */

void release_all()
{
    presenter_end();
    release_swapchain();
    presenter_shutdown();
    release_fences();
    if (g_off_rtv) { g_off_rtv->Release(); g_off_rtv = 0; }
    if (g_off_stage) { g_off_stage->Release(); g_off_stage = 0; }
    if (g_off_rt) { g_off_rt->Release(); g_off_rt = 0; }
    if (g_srv) { g_srv->Release(); g_srv = 0; }
    if (g_tex) { g_tex->Release(); g_tex = 0; }
    if (g_rast) { g_rast->Release(); g_rast = 0; }
    if (g_smp_linear) { g_smp_linear->Release(); g_smp_linear = 0; }
    if (g_smp_point) { g_smp_point->Release(); g_smp_point = 0; }
    if (g_cb) { g_cb->Release(); g_cb = 0; }
    if (g_ps) { g_ps->Release(); g_ps = 0; }
    if (g_vs) { g_vs->Release(); g_vs = 0; }
    /* The device and its context are hal/gpu_device.cpp's and are released
       there, after this handler runs: atexit unwinds in reverse order of
       registration and the device registers first, inside the acquire below. */
    g_ctx = 0;
    g_dev = 0;
}

void at_exit()
{
    if (g_cost_n)
        fprintf(stderr, "[present] cost per picture over %d: upload %.3f ms, "
                "draw %.3f ms, present %.3f ms\n", g_cost_n,
                g_up_ms / g_cost_n, g_draw_ms / g_cost_n,
                g_block_ms / g_cost_n);
    if (g_offscreen)
        fprintf(stderr, "[present] offscreen summary: %d frame(s) drawn, %d "
                "checked, %d over tolerance %d, worst %u pixel(s) off in red, "
                "green or blue, %u in alpha only, %u on a texel boundary, "
                "worst channel error %d VERDICT=%s\n",
                g_off_frames, g_off_checked, g_off_bad, g_off_tol,
                g_off_worst_rgb, g_off_worst_a, g_off_worst_tie, g_off_maxerr,
                g_off_bad ? "MISMATCH" : "PASS");
    if (g_presents)
        fprintf(stderr, "[present] %d picture(s) went through the graphics "
                "card this run\n", g_presents);
    if (g_uploads)
        fprintf(stderr, "[present] %d upload(s); at most %d picture(s) in flight on the card "
                "(deepest seen %d), %lld wait(s) for it, %.1f ms in all; %d out-of-memory "
                "retr%s, %d recovered; %d picture(s) skipped\n", g_uploads, g_inflight_max,
                g_fence_deep, g_fence_waits, g_fence_wait_ms, g_oom_retries,
                g_oom_retries == 1 ? "y" : "ies", g_oom_recovered, g_up_refused);
    if (g_recoveries || g_lost)
        fprintf(stderr, "[present] the graphics card path was rebuilt %d time(s) this run%s\n",
                g_recoveries, g_lost ? "; the last rebuild had not finished" : "");
    address_rows("exit  ");
    /* THE EXIT RULE (hal/gpu_device.h): stop the presenter thread and leave
       every Direct3D object to the system. Releasing them here crashed the
       AMD and Intel drivers at quit. */
    if (port_gpu_exit_release()) release_all();
    else presenter_exit_stop();
}

/* ---- the one-time start-up ----------------------------------------------- */

bool start()
{
    read_settings();
    if (!g_backend || g_down) return false;
    if (g_dev) return true;
    if (g_tried_device) return false;   /* tried once and failed */
    g_tried_device = 1;

    /* THE ARENA AND THE LOADING ARE hal/gpu_device.cpp's, in that order: it
       reserves 0x30000000 before it creates anything, which is the insurance
       the stage-0 probe asked for, and it LoadLibrary's d3d11.dll rather than
       linking it, which is what keeps this exe's import table where it was. */
    address_rows("before");

    if (!port_gpu_device_acquire(g_want_warp)) {
        fall_back("no Direct3D device would start", 0);
        return false;
    }
    g_dev = (ID3D11Device *)port_gpu_device();
    g_ctx = (ID3D11DeviceContext *)port_gpu_device_context();
    if (!g_dev || !g_ctx) { fall_back("no Direct3D device would start", 0); return false; }
    g_level = (D3D_FEATURE_LEVEL)port_gpu_device_feature_level();
    g_is_warp = port_gpu_device_is_warp();
    {
        const char *n = port_gpu_device_adapter();
        unsigned i = 0;
        for (; i + 1 < sizeof g_adapter && n[i]; ++i) g_adapter[i] = n[i];
        g_adapter[i] = 0;
    }
    fprintf(stderr, "[present] Direct3D 11 on \"%s\"%s, feature level %u_%u, "
            "ready in %.1f ms. Display %d Hz. Filter %s. VSync %s.\n",
            g_adapter[0] ? g_adapter : "(unnamed)",
            g_is_warp ? " (WARP, the software device Windows ships)" : "",
            (unsigned)((g_level >> 12) & 0xf), (unsigned)((g_level >> 8) & 0xf),
            port_gpu_device_create_ms(), g_refresh_hz,
            g_filter >= 2 ? "sharp" : (g_filter == 1 ? "smooth" : "nearest"),
            g_vsync_key ? "on" : "off");
    if (!make_pipeline()) { release_all(); return false; }
    port_gpu_raster_set_lost_hook(lost_hook);
    address_rows("after ");
    atexit(at_exit);
    return true;
}

}  // namespace

/* ---- what the present path calls ----------------------------------------- */

extern "C" int port_gpu_present_enabled(void)
{
    if (!g_started) read_settings();
    return g_backend && !g_down;
}

extern "C" int port_gpu_present_offscreen_mode(void)
{
    if (!g_started) read_settings();
    return (g_backend && g_offscreen && !g_down) ? 1 : 0;
}

extern "C" void port_gpu_present_offscreen_frame(const void *pixels,
                                                 int stride_px, int src_w,
                                                 int src_h)
{
    if (!port_gpu_present_offscreen_mode()) return;
    if (!pixels || src_w <= 0 || src_h <= 0) return;
    if (!start()) return;
#ifndef PORT_ROM_CLEAN
    memtrace_tick();
#endif

    /* the device-lost drill, on this path too: the offscreen mode is the only
       one a headless run reaches, so without this the fallback could only ever
       be exercised on somebody's screen. The run must carry on to its own end
       and exit 0, because nothing downstream of here reads anything this path
       produced. */
    if (g_fail_after && g_off_frames >= g_fail_after) {
        fprintf(stderr, "[present] SM64DS_PRESENT_FAIL_AFTER=%d reached after "
                "%d picture(s); forcing the fallback on purpose\n",
                g_fail_after, g_off_frames);
        fall_back("a test asked for it", 0);
        return;
    }

    const int dw = g_off_w ? g_off_w : src_w;
    const int dh = g_off_h ? g_off_h : src_h;
    if (!ensure_offscreen(dw, dh)) return;

    /* the two on-demand checks, once each, on the first frame that has a real
       picture behind it (the source size is part of both answers) */
    static int checked;
    if (!checked) {
        checked = 1;
        rect_check(src_w, src_h);
    }
    if (g_swapprobe == 1) {
        g_swapprobe = 2;
        swap_probe(pixels, stride_px, src_w, src_h);
    }

    const long long t0 = qpc();
    /* the same bracket the windowed path takes, so the offscreen proof mode
       measures the card over exactly the work a real present does */
    port_gpu_timer_span_begin(PORT_GPU_SPAN_PRESENT);
    if (!upload(pixels, stride_px, src_w, src_h)) return;
    const long long t1 = qpc();
    if (!set_params(0, 0, dw, dh, dw, dh, src_w, src_h, g_filter)) return;
    draw_quad(g_off_rtv, dw, dh, g_filter);
    port_gpu_timer_span_end(PORT_GPU_SPAN_PRESENT);
    g_ctx->Flush();
    const long long t2 = qpc();
    g_up_ms += ms_between(t0, t1);
    g_draw_ms += ms_between(t1, t2);
    ++g_cost_n;
    ++g_off_frames;

    if (g_off_frames % g_off_every == 1 || g_off_every == 1)
        offscreen_compare((const unsigned char *)pixels, stride_px, src_w, src_h,
                          dw, dh);
}

/* THE PRESENT ITSELF, shared by the two ways a picture arrives: `pixels` is the
   processor's finished framebuffer, uploaded here; or `from_card` is the view
   of a picture the card composited itself (run cardfull, lane CARD2) and
   nothing is uploaded. Everything from the swap chain on is the same. */
static int present_impl(void *hwnd, const void *pixels, int stride_px,
                        int src_w, int src_h, int dst_x, int dst_y, int dst_w,
                        int dst_h, int client_w, int client_h,
                        double budget_ms, int frame_rate_key,
                        void *from_card);

/* THE OFFSCREEN PROOF MODE, FOR A FRAME HELD ON THE CARD (run cardfull, lane
   CARD2). The card composites the held frame with the overlay and the same quad
   draws the result into the offscreen target; on the frames the proof mode
   checks, the frame is then settled (read back and resolved by the processor's
   own copy of the rule) and the target is compared with that picture exactly
   as for any other frame. Returns 1 when it handled the picture, 0 when the
   caller must settle the frame and present it the ordinary way. */
extern "C" int port_gpu_present_card_offscreen(const void *overlay, int stride_px,
                                               int src_w, int src_h)
{
    if (!port_gpu_present_offscreen_mode() || !port_gpu_card_held()) return 0;
    if (!overlay || src_w <= 0 || src_h <= 0) return 0;
    if (!start()) return 0;
    /* the device-lost drill, as the ordinary offscreen frame has it */
    if (g_fail_after && g_off_frames >= g_fail_after) {
        fprintf(stderr, "[present] SM64DS_PRESENT_FAIL_AFTER=%d reached after "
                "%d picture(s); forcing the fallback on purpose\n",
                g_fail_after, g_off_frames);
        fall_back("a test asked for it", 0);
        return 0;
    }
    const int dw = g_off_w ? g_off_w : src_w;
    const int dh = g_off_h ? g_off_h : src_h;
    if (!ensure_offscreen(dw, dh)) return 0;
    void *srv = port_gpu_card_compose((const unsigned int *)overlay, stride_px);
    if (!srv) return 0;
    port_gpu_timer_span_begin(PORT_GPU_SPAN_PRESENT);
    if (!set_params(0, 0, dw, dh, dw, dh, src_w, src_h, g_filter)) return 0;
    draw_quad(g_off_rtv, dw, dh, g_filter, (ID3D11ShaderResourceView *)srv);
    port_gpu_timer_span_end(PORT_GPU_SPAN_PRESENT);
    g_ctx->Flush();
    ++g_cost_n;
    ++g_off_frames;
    if (g_off_frames % g_off_every == 1 || g_off_every == 1) {
        ntr::gx_card_settle();
        offscreen_compare((const unsigned char *)overlay, stride_px, src_w,
                          src_h, dw, dh);
    }
    return 1;
}

/* The pacer's repeat of the picture already up (hal/presenter.h): flagged by
   the caller around the present. With the presenter running a repeat is not
   drawn at all (it would put the same pixels in the ring again); the caller
   asks first. */
extern "C" void port_gpu_present_set_repeat(int on) { g_repeat = on; }
extern "C" int port_gpu_present_repeat_skip(int client_w, int client_h)
{
    return g_repeat && presenter_running() && !g_down &&
           presenter_have_picture(client_w, client_h) &&
           client_w == g_sc_w && client_h == g_sc_h;
}

extern "C" int port_gpu_present_frame(void *hwnd, const void *pixels,
                                      int stride_px, int src_w, int src_h,
                                      int dst_x, int dst_y, int dst_w,
                                      int dst_h, int client_w, int client_h,
                                      double budget_ms, int frame_rate_key)
{
    return present_impl(hwnd, pixels, stride_px, src_w, src_h, dst_x, dst_y,
                        dst_w, dst_h, client_w, client_h, budget_ms,
                        frame_rate_key, 0);
}

/* THE PICTURE PRESENTED FROM THE CARD (run cardfull, lane CARD2): `overlay` is
   the framebuffer holding only the 2D, hal/gpu_raster.h composites it with the
   3D the card still holds, and the result goes through the same quad, filter
   and swap chain as any other picture. 0 means nothing was presented and the
   caller must settle the frame and fall through to the ordinary present. */
extern "C" int port_gpu_present_card_frame(void *hwnd, const void *overlay,
                                           int stride_px, int src_w, int src_h,
                                           int dst_x, int dst_y, int dst_w,
                                           int dst_h, int client_w,
                                           int client_h, double budget_ms,
                                           int frame_rate_key)
{
    if (!port_gpu_present_enabled() || !port_gpu_card_held()) return 0;
    if (!overlay || src_w <= 0 || src_h <= 0) return 0;
    if (!start()) return 0;
    void *srv = port_gpu_card_compose((const unsigned int *)overlay, stride_px);
    if (!srv) return 0;
    return present_impl(hwnd, overlay, stride_px, src_w, src_h, dst_x, dst_y,
                        dst_w, dst_h, client_w, client_h, budget_ms,
                        frame_rate_key, srv);
}

/* The offscreen proof mode's version of the above (run hunt8, lane CARDSM1):
   the retained composite goes through the offscreen quad as it is, with no
   readback, except on the frames the proof compares, which return 0 so the
   caller reads the tick back and takes the ordinary offscreen path. */
extern "C" int port_gpu_present_retained_offscreen(const void *view, int src_w,
                                                   int src_h)
{
    if (!port_gpu_present_offscreen_mode() || !view) return 0;
    if (src_w <= 0 || src_h <= 0) return 0;
    if (!start()) return 0;
    if (g_off_every == 1 || (g_off_frames + 1) % g_off_every == 1) return 0;
    if (g_fail_after && g_off_frames >= g_fail_after) return 0;
    const int dw = g_off_w ? g_off_w : src_w;
    const int dh = g_off_h ? g_off_h : src_h;
    if (!ensure_offscreen(dw, dh)) return 0;
    port_gpu_timer_span_begin(PORT_GPU_SPAN_PRESENT);
    if (!set_params(0, 0, dw, dh, dw, dh, src_w, src_h, g_filter)) return 0;
    draw_quad(g_off_rtv, dw, dh, g_filter, (ID3D11ShaderResourceView *)view);
    port_gpu_timer_span_end(PORT_GPU_SPAN_PRESENT);
    g_ctx->Flush();
    ++g_cost_n;
    ++g_off_frames;
    return 1;
}

extern "C" int port_gpu_present_retained_frame(void *hwnd, const void *view,
                                               int src_w, int src_h, int dst_x,
                                               int dst_y, int dst_w, int dst_h,
                                               int client_w, int client_h,
                                               double budget_ms,
                                               int frame_rate_key)
{
    if (!port_gpu_present_enabled() || !view) return 0;
    if (!start()) return 0;
    static const unsigned int nothing = 0;     /* present_impl wants a pointer */
    return present_impl(hwnd, &nothing, 1, src_w, src_h, dst_x, dst_y, dst_w,
                        dst_h, client_w, client_h, budget_ms, frame_rate_key,
                        (void *)view);
}

static int present_impl(void *hwnd, const void *pixels, int stride_px,
                        int src_w, int src_h, int dst_x, int dst_y, int dst_w,
                        int dst_h, int client_w, int client_h,
                        double budget_ms, int frame_rate_key, void *from_card)
{
    if (!port_gpu_present_enabled()) return 0;
    if (!pixels || src_w <= 0 || src_h <= 0 || client_w <= 0 || client_h <= 0)
        return 0;
    if (!start()) return 0;
#ifndef PORT_ROM_CLEAN
    memtrace_tick();
#endif

    /* the device-lost drill: a run asks for the fallback on purpose so the
       recovery path is exercised rather than assumed */
    if (g_fail_after && g_presents >= g_fail_after) {
        fprintf(stderr, "[present] SM64DS_PRESENT_FAIL_AFTER=%d reached after "
                "%d picture(s); forcing the fallback on purpose\n",
                g_fail_after, g_presents);
        g_giving_up = 1;            /* the plain fallback, not a rebuild */
        fall_back("a test asked for it", 0);
        return 0;
    }
    /* a rebuild that failed waits its turn; the ordinary path draws until then */
    if (g_lost && g_retry_at && qpc() < g_retry_at) return 0;
#ifndef PORT_ROM_CLEAN
    if (g_lost_drill_at > 0 && g_presents == g_lost_drill_at) {
        g_lost_drill_at = g_lost_drill_every > 0 ? -1 : 0;
        fprintf(stderr, "[present] SM64DS_PRESENT_LOST_AT: picture %d acts as if the card "
                "had been %s\n", g_presents, g_lost_drill_keepdev ? "out of memory" : "removed");
        if (!g_lost_drill_keepdev) g_renew = 1;
        fall_back(g_lost_drill_keepdev ? "a test asked for a rebuild" : "the card was removed",
                  g_lost_drill_keepdev ? E_OUTOFMEMORY : (HRESULT)0x887A0005L);
        return 0;
    }
    if (g_lost && !g_sc && g_rebuild_fail_drill > 0) {
        --g_rebuild_fail_drill;
        fprintf(stderr, "[present] SM64DS_PRESENT_REBUILD_FAIL: this rebuild fails on purpose\n");
        fall_back("a test asked the rebuild to fail", E_FAIL);
        return 0;
    }
#endif
    if (g_renew && !renew_device()) return 0;

    /* THE PRESENTER THREAD (run hunt8, lane PRESENTER1): the swap chain lives
       on the presenter's own device and belongs to its thread; this thread
       draws each finished picture into a texture of the presenter's ring and
       hands it over. See hal/presenter.h. Any trouble puts this run back on
       the game thread's own present, below. */
    for (int again = 0; again < 2; ++again) {
        const bool want_pres = presenter_enabled() && !g_pres_off;
        if (!g_sc || g_parent != (HWND)hwnd) {
            presenter_end();
            release_swapchain();
            if (g_child && g_parent != (HWND)hwnd) { child_hide(); g_child = 0; }
            g_sc_dev = g_dev;
            if (want_pres) {
                if (void *d2 = presenter_swap_device(g_dev)) g_sc_dev = (ID3D11Device *)d2;
                else g_pres_off = 1;
            }
            /* the swap chain goes on a child of the game window (see the
               banner over RECOVERIES_MAX) */
            if (!g_child) g_child = make_child((HWND)hwnd, client_w, client_h);
            else child_size(client_w, client_h);
            g_parent = (HWND)hwnd;
            if (!make_swapchain(g_child ? g_child : (HWND)hwnd, client_w, client_h)) return 0;
        } else if (g_sc_w != client_w || g_sc_h != client_h) {
            /* the window changed size: a drag on the border, a maximise, or the
               F12 borderless toggle. The back buffers follow it. The presenter
               thread holds a back buffer and textures of the old size, so it is
               ended first and started again below. */
            presenter_end();
            child_size(client_w, client_h);
            if (g_scrtv) { g_scrtv->Release(); g_scrtv = 0; }
            const HRESULT hr = g_sc->ResizeBuffers(0, (UINT)client_w, (UINT)client_h,
                                                   DXGI_FORMAT_UNKNOWN, 0);
            if (FAILED(hr)) { fall_back("the window could not be resized", hr); return 0; }
            g_sc_w = client_w;
            g_sc_h = client_h;
            if (!make_backbuffer_view()) return 0;
        }
        if (!want_pres || g_pres_off || g_sc_dev == g_dev) break;
        if (const long perr = presenter_error()) {
            presenter_end();
            g_pres_off = 1;
            fall_back("the picture could not be handed over", (HRESULT)perr);
            return 0;
        }
        if (!presenter_running()) {
            const int vs = g_vsync_key ? 1 : 0;
            const int rhz = g_refresh_hz > 0 ? g_refresh_hz : 60;
            const double hz = vs ? rhz : (frame_rate_key > 0 ? frame_rate_key : rhz);
            if (!presenter_begin(g_dev, g_ctx, g_sc, client_w, client_h, vs, hz)) {
                g_pres_off = 1;
                fprintf(stderr, "[present] the presenter thread would not start; this run presents from the game thread\n");
                /* the swap chain is on the presenter's device: make one on ours */
                release_swapchain();
                g_sc_dev = g_dev;
                continue;
            }
        }
        break;
    }
    if (presenter_running()) {
        /* FrameRate is live (run hunt8, lane FPSLIVE1): the timer grid follows
           the key the way presenter_begin first chose it. A no-op while the
           period is unchanged. */
        if (!g_vsync_key)
            presenter_set_grid_hz(frame_rate_key > 0 ? (double)frame_rate_key
                                                     : (double)(g_refresh_hz > 0 ? g_refresh_hz : 60));
        const long long p0 = qpc();
        ID3D11RenderTargetView *prt = (ID3D11RenderTargetView *)presenter_target();
        if (!prt) return 1;     /* the ring had no free texture: this picture is dropped, the one on show stays */
        port_gpu_timer_span_begin(PORT_GPU_SPAN_PRESENT);
        if (!from_card && !upload(pixels, stride_px, src_w, src_h)) return 0;
        const long long p1 = qpc();
        if (!set_params(dst_x, dst_y, dst_w, dst_h, client_w, client_h, src_w,
                        src_h, g_filter))
            return 0;
        draw_quad(prt, client_w, client_h, g_filter,
                  (ID3D11ShaderResourceView *)from_card);
        port_gpu_timer_span_end(PORT_GPU_SPAN_PRESENT);
        const long long p2 = qpc();
        presenter_publish(g_repeat);
        ++g_presents;
        recovered();
        after_rebuild_tick();
        g_up_ms += ms_between(p0, p1);
        g_draw_ms += ms_between(p1, p2);
        ++g_cost_n;
        return 1;
    }
    if (g_scrtv == 0 && g_sc && !make_backbuffer_view()) return 0;

    const long long t0 = qpc();
    /* The card's own clock over the handover: the upload and the quad. The
       Present itself is deliberately outside it -- a blocking Present is the
       monitor's wait, not the card's work, and folding it in would make the
       overlay's card line read as a vsync meter. */
    port_gpu_timer_span_begin(PORT_GPU_SPAN_PRESENT);
    if (!from_card && !upload(pixels, stride_px, src_w, src_h)) return 0;
    const long long t1 = qpc();
    if (!set_params(dst_x, dst_y, dst_w, dst_h, client_w, client_h, src_w,
                    src_h, g_filter))
        return 0;
    draw_quad(g_scrtv, client_w, client_h, g_filter,
              (ID3D11ShaderResourceView *)from_card);
    port_gpu_timer_span_end(PORT_GPU_SPAN_PRESENT);
    const long long t2 = qpc();
    if (presenter_hash_on()) {
        static long long old_serial;
        ID3D11Texture2D *bb = 0;
        if (SUCCEEDED(g_sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&bb)) && bb) {
            presenter_hash_log("main", ++old_serial, presenter_hash_texture(g_dev, g_ctx, bb, 0));
            bb->Release();
        }
    }

    const int interval = vsync_interval(budget_ms, frame_rate_key);
    const HRESULT hr = g_sc->Present((UINT)interval, 0);
    const long long t3 = qpc();
    /* DXGI_STATUS_OCCLUDED is a SUCCESS code and an ordinary fact of life: the
       window is covered or minimised and the frame went nowhere. Not a failure
       and not a reason to fall back. */
    if (FAILED(hr)) { fall_back("the picture could not be handed over", hr); return 0; }

    presenter_stat_note(t2, !g_repeat);
    ++g_presents;
    recovered();
    after_rebuild_tick();
    g_up_ms += ms_between(t0, t1);
    g_draw_ms += ms_between(t1, t2);
    g_block_ms += ms_between(t2, t3);
    ++g_cost_n;
    if (g_last_end) vsync_watch(ms_between(g_last_end, t3), ms_between(t2, t3),
                                budget_ms);
    g_last_end = t3;
    return 1;
}

#else   /* not Windows: the port ships on Windows and there is no backend here */

#include "hal/gpu_present.h"

int port_gpu_present_enabled(void) { return 0; }
int port_gpu_present_offscreen_mode(void) { return 0; }
int port_gpu_present_frame(void *, const void *, int, int, int, int, int, int,
                           int, int, int, double, int)
{
    return 0;
}
int port_gpu_present_card_frame(void *, const void *, int, int, int, int, int,
                                int, int, int, int, double, int)
{
    return 0;
}
int port_gpu_present_card_offscreen(const void *, int, int, int) { return 0; }
int port_gpu_present_retained_frame(void *, const void *, int, int, int, int,
                                    int, int, int, int, double, int)
{
    return 0;
}
int port_gpu_present_retained_offscreen(const void *, int, int) { return 0; }
void port_gpu_present_offscreen_frame(const void *, int, int, int) {}

#endif
