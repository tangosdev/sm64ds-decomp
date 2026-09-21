/* THE SHARED DIRECT3D 11 DEVICE. See hal/gpu_device.h for why there is only
 * one, and for the import-table rule this file keeps.
 *
 * Everything here was hal/gpu_present.cpp's until a second consumer appeared:
 * the loader, the device creation, the adapter name and the address audit are
 * the same code, moved rather than rewritten, so the present path behaves
 * exactly as it did and the renderer gets the same device with the same
 * properties.
 */
#if defined(_WIN32)

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hal/gpu_device.h"
#include "ntr/mmio.h"

/* hal/os_arena.cpp, declared locally the way hal/lk6_savestate.cpp and
   hal/gpu_present.cpp declare it: the arena has no header of its own.
   port_arena_base RUNS the lazy reservation of 0x30000000, which is why it is
   called before the device is created. */
extern "C" void *port_arena_base(void);
extern "C" int port_arena_is_fixed(void);

namespace {

typedef HRESULT (WINAPI *D3D11CreateDevice_t)(IDXGIAdapter *, D3D_DRIVER_TYPE,
                                              HMODULE, UINT,
                                              const D3D_FEATURE_LEVEL *, UINT,
                                              UINT, ID3D11Device **,
                                              D3D_FEATURE_LEVEL *,
                                              ID3D11DeviceContext **);

HMODULE g_d3d11;
D3D11CreateDevice_t p_create;

ID3D11Device        *g_dev;
ID3D11DeviceContext *g_ctx;
D3D_FEATURE_LEVEL    g_level;
int                  g_is_warp;
int                  g_tried;
int                  g_failed;
int                  g_want_taken = -2;   /* what the first caller asked for */
double               g_create_ms;
char                 g_adapter[128];

/* ---- the address rows, the stage-0 probe's measurement kept as a gate ----- */

struct Fixed { uintptr_t base; SIZE_T size; const char *name; };
const Fixed kFixed[] = {
    { ntr::MAIN_BASE,   ntr::MAIN_SIZE,   "main memory" },
    { ntr::IO_BASE,     ntr::IO_SIZE,     "hardware registers" },
    { ntr::PLTT_BASE,   ntr::PLTT_SIZE,   "palette memory" },
    { ntr::VRAM_BASE,   ntr::VRAM_SIZE,   "video memory" },
    { ntr::OAM_BASE,    ntr::OAM_SIZE,    "sprite memory" },
    { ntr::SHARED_BASE, ntr::SHARED_SIZE, "shared system block" },
};
enum { kFixedCount = sizeof kFixed / sizeof kFixed[0] };
const uintptr_t kArenaBase = 0x30000000u;    /* hal/os_arena.cpp's own */
const uintptr_t kUserEnd = 0x80000000u;      /* 2 GB: this process has no more */

/* The allocation base is not compared for equality: VirtualAlloc rounds a
   reservation down to the 64K granularity, so the shared system block, asked
   for at 0x027ff000, correctly reports a base of 0x027f0000 and is held all the
   same. What matters is that the page the game uses is committed and writable
   inside an allocation starting at or below it. */
bool held(const MEMORY_BASIC_INFORMATION &m, uintptr_t base)
{
    return m.State == MEM_COMMIT && m.Protect == PAGE_READWRITE &&
           m.AllocationBase != 0 && (uintptr_t)m.AllocationBase <= base;
}

void name_adapter()
{
    IDXGIDevice *dxdev = 0;
    if (FAILED(g_dev->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxdev)) ||
        !dxdev)
        return;
    IDXGIAdapter *ad = 0;
    if (SUCCEEDED(dxdev->GetAdapter(&ad)) && ad) {
        DXGI_ADAPTER_DESC d;
        memset(&d, 0, sizeof d);
        if (SUCCEEDED(ad->GetDesc(&d))) {
            unsigned i = 0;
            for (; i + 1 < sizeof g_adapter && d.Description[i]; ++i)
                g_adapter[i] = (char)(d.Description[i] < 128 ? d.Description[i]
                                                             : '?');
            g_adapter[i] = 0;
        }
        ad->Release();
    }
    dxdev->Release();
}

bool make_device(D3D_DRIVER_TYPE type)
{
    static const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL got = (D3D_FEATURE_LEVEL)0;
    const HRESULT hr = p_create(0, type, 0, 0, levels,
                                (UINT)(sizeof levels / sizeof levels[0]),
                                D3D11_SDK_VERSION, &g_dev, &got, &g_ctx);
    if (FAILED(hr) || !g_dev) {
        g_dev = 0;
        g_ctx = 0;
        return false;
    }
    g_level = got;
    g_is_warp = (type == D3D_DRIVER_TYPE_WARP);
    return true;
}

void release_device()
{
    if (g_ctx) { g_ctx->ClearState(); g_ctx->Flush(); g_ctx->Release(); g_ctx = 0; }
    if (g_dev) { g_dev->Release(); g_dev = 0; }
}

}  // namespace

extern "C" int port_gpu_device_acquire(int want_warp)
{
    if (g_dev) {
        /* A second consumer that wanted a different driver type gets the one
           that exists. Said out loud once, because a proof run that asked for
           WARP and silently measured the card would be measuring the wrong
           thing. */
        if (g_want_taken != want_warp && g_want_taken != -2) {
            static int said;
            if (!said) {
                said = 1;
                fprintf(stderr, "[gpu] a second setting asked for a %s device; "
                        "this process already has the %s one and shares it.\n",
                        want_warp == 1 ? "software (WARP)" : "hardware",
                        g_is_warp ? "software (WARP)" : "hardware");
            }
        }
        return 1;
    }
    if (g_tried) return 0;
    g_tried = 1;
    g_want_taken = want_warp;

    /* THE ARENA FIRST. hal/os_arena.cpp claims 0x30000000 lazily, and whatever
       holds that address when the game first carves the arena wins; a lost race
       silently turns disk save states off. The stage-0 probe measured that this
       machine's driver does not take it either way, so this is insurance rather
       than a fix, and it is one call. */
    (void)port_arena_base();

    g_d3d11 = LoadLibraryA("d3d11.dll");
    if (!g_d3d11) { g_failed = 1; return 0; }
    p_create = (D3D11CreateDevice_t)GetProcAddress(g_d3d11, "D3D11CreateDevice");
    if (!p_create) { g_failed = 1; return 0; }

    LARGE_INTEGER f, t0, t1;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t0);
    bool ok = false;
    if (want_warp == 1) {
        ok = make_device(D3D_DRIVER_TYPE_WARP);
    } else {
        ok = make_device(D3D_DRIVER_TYPE_HARDWARE);
        if (!ok && want_warp < 0) ok = make_device(D3D_DRIVER_TYPE_WARP);
    }
    QueryPerformanceCounter(&t1);
    if (!ok) { g_failed = 1; return 0; }
    g_create_ms = f.QuadPart
                      ? (double)(t1.QuadPart - t0.QuadPart) * 1000.0 /
                            (double)f.QuadPart
                      : 0.0;
    name_adapter();
    atexit(release_device);
    return 1;
}

extern "C" void *port_gpu_device(void) { return (void *)g_dev; }
extern "C" void *port_gpu_device_context(void) { return (void *)g_ctx; }
extern "C" const char *port_gpu_device_adapter(void) { return g_adapter; }
extern "C" int port_gpu_device_is_warp(void) { return g_is_warp; }
extern "C" unsigned port_gpu_device_feature_level(void) { return (unsigned)g_level; }
extern "C" double port_gpu_device_create_ms(void) { return g_create_ms; }
extern "C" int port_gpu_device_failed(void) { return g_failed; }

extern "C" void port_gpu_device_address_rows(const char *when, int enabled)
{
    if (!enabled) return;
    int lost = 0;
    for (int i = 0; i < kFixedCount; ++i) {
        MEMORY_BASIC_INFORMATION m;
        memset(&m, 0, sizeof m);
        VirtualQuery((void *)kFixed[i].base, &m, sizeof m);
        const int ok = held(m, kFixed[i].base);
        if (!ok) ++lost;
        fprintf(stderr, "[present] %s range %-20s want=%08x..%08x allocbase=%08x "
                "region=%08x %s\n", when, kFixed[i].name,
                (unsigned)kFixed[i].base,
                (unsigned)(kFixed[i].base + kFixed[i].size),
                (unsigned)(uintptr_t)m.AllocationBase, (unsigned)m.RegionSize,
                ok ? "ours" : "NOT OURS");
    }
    MEMORY_BASIC_INFORMATION a;
    memset(&a, 0, sizeof a);
    VirtualQuery((void *)kArenaBase, &a, sizeof a);
    fprintf(stderr, "[present] %s arena          want=%08x allocbase=%08x %s\n",
            when, (unsigned)kArenaBase, (unsigned)(uintptr_t)a.AllocationBase,
            a.State == MEM_FREE ? "not claimed in this run (still free)"
                                : (held(a, kArenaBase) ? "at 0x30000000"
                                                       : "HELD BY SOMETHING ELSE"));
    /* the free address space, which is what a driver eats into */
    ULONGLONG total = 0, largest = 0;
    uintptr_t at = 0, largest_at = 0;
    while (at < kUserEnd) {
        MEMORY_BASIC_INFORMATION m;
        if (VirtualQuery((void *)at, &m, sizeof m) != sizeof m) break;
        if (!m.RegionSize) break;
        if (m.State == MEM_FREE) {
            ULONGLONG sz = m.RegionSize;
            if (at + sz > kUserEnd) sz = kUserEnd - at;
            total += sz;
            if (sz > largest) { largest = sz; largest_at = at; }
        }
        const uintptr_t next = at + (uintptr_t)m.RegionSize;
        if (next <= at) break;
        at = next;
    }
    fprintf(stderr, "[present] %s address space below %08x: free total=%u MB, "
            "largest free block=%u MB at %08x, ranges lost=%d, "
            "port_arena_is_fixed()=%d\n",
            when, (unsigned)kUserEnd, (unsigned)(total / (1024 * 1024)),
            (unsigned)(largest / (1024 * 1024)), (unsigned)largest_at, lost,
            port_arena_is_fixed());
}

#else   /* not Windows: the port ships on Windows and there is no device here */

#include "hal/gpu_device.h"

int port_gpu_device_acquire(int) { return 0; }
void *port_gpu_device(void) { return 0; }
void *port_gpu_device_context(void) { return 0; }
const char *port_gpu_device_adapter(void) { return ""; }
int port_gpu_device_is_warp(void) { return 0; }
unsigned port_gpu_device_feature_level(void) { return 0; }
double port_gpu_device_create_ms(void) { return 0.0; }
int port_gpu_device_failed(void) { return 1; }
void port_gpu_device_address_rows(const char *, int) {}

#endif
