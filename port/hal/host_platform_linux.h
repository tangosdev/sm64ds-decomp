/* host_platform_linux.h -- the SDL2 + POSIX backing for the port's host window,
 * input, framebuffer blit, and timing on Linux.
 *
 * PHASE 1 LANE A of the Linux port. This header is the platform SEAM: it is
 * included ONLY on non-Windows builds (see the #ifdef _WIN32 split in
 * walk_window.cpp). On Windows nothing here is compiled and the existing
 * <windows.h> + WinApi dynamic-loader + wndproc + StretchDIBits + QPC/Sleep
 * path is byte-for-byte unchanged.
 *
 * The design goal is that the GAME-LOGIC BODY of walk_window's frame loop --
 * every key_live(VK_*), every pad-bit read, the mouse-look globals, the blit
 * call, the phase clock -- compiles UNCHANGED on both platforms. To get that,
 * this header supplies drop-in replacements, spelled with the same names the
 * loop already uses:
 *
 *   - the Windows scalar types the loop names (LARGE_INTEGER, POINT, DWORD, ...)
 *   - the VK_* virtual-key constants, remapped to what SDL reports
 *   - a `WinApi W` compatible surface: W.GetAsyncKeyState_, W.PeekMessageA_,
 *     W.StretchDIBits_, W.GetCursorPos_, ... each backed by SDL
 *   - QueryPerformanceCounter/Frequency, Sleep, timeBeginPeriod (no-op)
 *
 * The ntr software raster is untouched: the game still fills ntr::Framebuffer
 * exactly as before; this only changes how those pixels reach the screen
 * (SDL streaming texture + SDL_RenderCopy instead of GDI StretchDIBits).
 *
 * AUDIO is stubbed elsewhere (SM64DS_NO_AUDIO path); real audio is a later lane.
 * The SEH quarantine is a passthrough on Linux for this lane (see
 * func_02043fdc.cpp); the real signal/longjmp translation is a later lane.
 */
#ifndef PORT_HOST_PLATFORM_LINUX_H
#define PORT_HOST_PLATFORM_LINUX_H

#ifdef _WIN32
#error "host_platform_linux.h must not be compiled on Windows"
#endif

#include <SDL2/SDL.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <time.h>       /* localtime_r (POSIX) for the playlog timestamp */
#include <sys/stat.h>   /* mkdir */
#include <unistd.h>

/* ---- Windows scalar types the loop and headers name ---------------------- */
typedef int            BOOL;
typedef unsigned long  DWORD;
typedef unsigned int   UINT;
typedef unsigned short WORD;
typedef long           LONG;
typedef unsigned char  BYTE;
typedef void          *HWND;
typedef void          *HDC;
typedef void          *HANDLE;
typedef void          *HMODULE;
typedef const char    *LPCSTR;
typedef char          *LPSTR;

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif
#ifndef WINAPI
#define WINAPI
#endif
#ifndef CALLBACK
#define CALLBACK
#endif
/* MSVC calling-convention keywords (__fastcall/__thiscall/__stdcall) are mapped
   for the whole non-MSVC build by port/hal/port_msvc_compat.h, which CMake
   force-includes (-include) so every HAL TU sees them, not only this header. */

typedef unsigned long  SIZE_T;
typedef struct { long x, y; } POINT;
typedef struct { long left, top, right, bottom; } RECT;

/* The overlay's working-set struct walk_window spells (`PortMemCounters pmc`).
   On Linux GetProcessMemoryInfo is a no-op returning FALSE, so this is only ever
   default-constructed; it just has to exist and carry the fields the code reads. */
struct PortMemCounters {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize, WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage, QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage, QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage, PeakPagefileUsage;
};
typedef union { struct { DWORD LowPart; LONG HighPart; }; long long QuadPart; }
    LARGE_INTEGER;

/* A message struct the loop's PeekMessage/Translate/Dispatch names. SDL fills
   only what the loop reads: `.message`, set to WM_QUIT when a close is seen. */
typedef struct { HWND hwnd; UINT message; unsigned wParam; long lParam; }
    MSG;

/* Just the DIB header fields StretchDIBits's caller sets; kept so the existing
   `BITMAPINFO bi` block compiles. The SDL blit reads none of them (the raster
   is always top-down 32bpp BGRA in this codebase), but the caller writes them. */
typedef struct {
    struct {
        DWORD biSize; long biWidth, biHeight;
        WORD biPlanes, biBitCount; DWORD biCompression, biSizeImage;
        long biXPelsPerMeter, biYPelsPerMeter; DWORD biClrUsed, biClrImportant;
    } bmiHeader;
    DWORD bmiColors[1];
} BITMAPINFO;

/* Message / style constants the loop tests literally. */
enum {
    WM_QUIT = 0x0012, WM_DESTROY = 0x0002, WM_KEYDOWN = 0x0100,
    WM_RBUTTONDOWN = 0x0204, WM_RBUTTONUP = 0x0205, WM_MOUSEMOVE = 0x0200,
    WM_LBUTTONDOWN = 0x0201, WM_LBUTTONUP = 0x0202, WM_MOUSEWHEEL = 0x020A,
    WM_KILLFOCUS = 0x0008, WM_CAPTURECHANGED = 0x0215,
    PM_REMOVE = 0x0001,
    BI_RGB = 0, DIB_RGB_COLORS = 0, SRCCOPY = 0x00CC0020,
    WS_OVERLAPPEDWINDOW = 0x00CF0000, WS_THICKFRAME = 0x00040000,
    WS_VISIBLE = 0x10000000, CW_USEDEFAULT = (int)0x80000000,
    WHEEL_DELTA = 120
};

/* ---- VK_* -> a value that indexes the SDL keyboard-state array.
   GetAsyncKeyState(vk) returns <0 (negative == high bit set) while the key is
   held; the loop tests `< 0`. We store the SDL_SCANCODE in the "vk" and return
   0x8000 (a negative short's sign bit) when SDL says it is down. Letters are
   passed as their ASCII code by the loop ('W','A','S','D','X'); map those too. */
enum {
    VK_ESCAPE = 256 + SDL_SCANCODE_ESCAPE,
    VK_UP     = 256 + SDL_SCANCODE_UP,
    VK_DOWN   = 256 + SDL_SCANCODE_DOWN,
    VK_LEFT   = 256 + SDL_SCANCODE_LEFT,
    VK_RIGHT  = 256 + SDL_SCANCODE_RIGHT,
    VK_RETURN = 256 + SDL_SCANCODE_RETURN,
    VK_SPACE  = 256 + SDL_SCANCODE_SPACE,
    VK_SHIFT  = 256 + SDL_SCANCODE_LSHIFT,
    VK_CONTROL= 256 + SDL_SCANCODE_LCTRL,
    VK_TAB    = 256 + SDL_SCANCODE_TAB,
    VK_F1     = 256 + SDL_SCANCODE_F1,
    VK_F2     = 256 + SDL_SCANCODE_F2,
    VK_F3     = 256 + SDL_SCANCODE_F3,
    VK_F4     = 256 + SDL_SCANCODE_F4,
    VK_F5     = 256 + SDL_SCANCODE_F5,
    /* F6/F7 are unbound today; F8/F9 are the save-state snapshot and restore
       (walk_window's key_live block). Filled in as a run rather than only the
       two that are bound, so the next binding does not have to come back here. */
    VK_F6     = 256 + SDL_SCANCODE_F6,
    VK_F7     = 256 + SDL_SCANCODE_F7,
    VK_F8     = 256 + SDL_SCANCODE_F8,
    VK_F9     = 256 + SDL_SCANCODE_F9,
    VK_LBUTTON = 1  /* handled specially in the async-key wrapper below */
};

/* LOWORD/HIWORD as the wndproc uses them on l/wParam. */
#ifndef LOWORD
#define LOWORD(l) ((WORD)((unsigned)(l) & 0xffff))
#define HIWORD(l) ((WORD)(((unsigned)(l) >> 16) & 0xffff))
#endif

/* ---- the SDL-backed host state ------------------------------------------- */
namespace port_lin {

struct State {
    SDL_Window   *win  = nullptr;
    SDL_Renderer *ren  = nullptr;
    SDL_Texture  *tex  = nullptr;
    int tex_w = 0, tex_h = 0;      /* the ntr framebuffer size the texture holds */
    bool quit = false;             /* set on a close/ESC, drained as WM_QUIT */
    int  win_w = 0, win_h = 0;     /* client size in pixels (== fb * ZOOM) */
    /* mouse-look: SDL relative mode gives the springs-back-to-anchor behaviour
       the Win32 path faked with SetCursorPos, without the desk-edge problem. */
    bool right_down = false;
    int  rel_dx = 0, rel_dy = 0;   /* accumulated relative motion while held */
    int  wheel = 0;
    int  last_lx = 0, last_ly = 0; /* last cursor client pos (for the touch poll) */
    bool lbutton = false;
    /* Synthetic cursor for the mouse-look spring-back. The Win32 wndproc reads
       GetCursorPos, subtracts a fixed anchor, then SetCursorPos-es back to the
       anchor every move. We reproduce that exactly: while looking, each SDL
       relative motion advances synth_{x,y}; GetCursorPos returns it;
       SetCursorPos(anchor) resets it to the anchor. So the existing wndproc
       math (p - anchor) yields the relative delta with no desk-edge limit. */
    int  synth_x = 0, synth_y = 0;
};
inline State &S() { static State s; return s; }

/* Map a "vk" (either 256+scancode, or an ASCII letter the loop passed) to an
   SDL scancode, or -1 if it is the special mouse-button sentinel. */
inline int vk_to_scancode(int vk) {
    if (vk >= 256) return vk - 256;
    switch (vk) {
    case 'W': return SDL_SCANCODE_W;
    case 'A': return SDL_SCANCODE_A;
    case 'S': return SDL_SCANCODE_S;
    case 'D': return SDL_SCANCODE_D;
    case 'Q': return SDL_SCANCODE_Q;
    case 'E': return SDL_SCANCODE_E;
    case 'R': return SDL_SCANCODE_R;
    case 'F': return SDL_SCANCODE_F;
    case 'C': return SDL_SCANCODE_C;
    case 'X': return SDL_SCANCODE_X;
    default:  break;
    }
    /* generic printable ASCII -> its scancode via SDL's keycode path */
    SDL_Scancode sc = SDL_GetScancodeFromKey((SDL_Keycode)vk);
    return sc == SDL_SCANCODE_UNKNOWN ? -1 : (int)sc;
}

} /* namespace port_lin */

/* ---- the wndproc the loop registers: on Linux it is the SDL event handler.
   walk_window.cpp defines `static LRESULT CALLBACK wndproc(...)`. We keep that
   exact signature valid by typedef'ing the Windows spellings, and route SDL
   events into it so the mouse-look / touch / wheel logic there is reused
   verbatim. */
typedef long LRESULT;
typedef unsigned WPARAM;
typedef long     LPARAM;
typedef LRESULT (CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

/* ---- the `WinApi W` compat surface -------------------------------------
   Mirrors the Win32 WinApi struct FIELD FOR FIELD with plain function pointers,
   so walk_window.cpp's loop body -- `W.GetAsyncKeyState_(VK_W)`,
   `W.PeekMessageA_(&msg,0,0,0,PM_REMOVE)`, `W.StretchDIBits_(...)`, and every
   `if (W.SetProcessInformation_)` truthiness guard -- compiles and behaves
   unchanged. The pointers are filled by port_lin_winapi_load(), which also
   creates the SDL window. */
extern "C" void port_lin_create_window(int fb_w, int fb_h, int zoom,
                                        const char *title);

/* the free functions the pointers below bind to (defined in the .cpp) */
extern "C" {
short   port_lin_GetAsyncKeyState(int vk);
BOOL    port_lin_PeekMessage(MSG *m, HWND, UINT, UINT, UINT);
BOOL    port_lin_TranslateMessage(const MSG *);
LRESULT port_lin_DispatchMessage(const MSG *);
void    port_lin_PostQuitMessage(int);
LRESULT port_lin_DefWindowProc(HWND, UINT, WPARAM, LPARAM);
int     port_lin_StretchDIBits(HDC, int, int, int, int, int, int, int, int,
                               const void *, const BITMAPINFO *, UINT, DWORD);
BOOL    port_lin_GetCursorPos(POINT *);
BOOL    port_lin_SetCursorPos(int, int);
int     port_lin_ShowCursor(BOOL);
HWND    port_lin_SetCapture(HWND);
BOOL    port_lin_ReleaseCapture(void);
unsigned port_lin_timeBeginPeriod(unsigned);
unsigned port_lin_timeEndPeriod(unsigned);
BOOL    port_lin_SetProcessInformation(HANDLE, int, void *, DWORD);
BOOL    port_lin_GetProcessMemoryInfo(HANDLE, void *, DWORD);
/* the wndproc the loop registers is handed here so the SDL pump can call it */
void    port_lin_set_wndproc(WNDPROC proc);
}

struct WinApi {
    short   (*GetAsyncKeyState_)(int);
    BOOL    (*PeekMessageA_)(MSG *, HWND, UINT, UINT, UINT);
    BOOL    (*TranslateMessage_)(const MSG *);
    LRESULT (*DispatchMessageA_)(const MSG *);
    void    (*PostQuitMessage_)(int);
    LRESULT (*DefWindowProcA_)(HWND, UINT, WPARAM, LPARAM);
    int     (*StretchDIBits_)(HDC, int, int, int, int, int, int, int, int,
                              const void *, const BITMAPINFO *, UINT, DWORD);
    BOOL    (*GetCursorPos_)(POINT *);
    BOOL    (*SetCursorPos_)(int, int);
    int     (*ShowCursor_)(BOOL);
    HWND    (*SetCapture_)(HWND);
    BOOL    (*ReleaseCapture_)(void);
    unsigned(*timeBeginPeriod_)(unsigned);
    unsigned(*timeEndPeriod_)(unsigned);
    BOOL    (*SetProcessInformation_)(HANDLE, int, void *, DWORD);
    BOOL    (*GetProcessMemoryInfo_)(HANDLE, void *, DWORD);
};

/* fill the pointer table. Returns true always (the SDL window is created by the
   caller via port_lin_create_window before the loop). */
static inline bool port_lin_winapi_load(WinApi *W) {
    W->GetAsyncKeyState_ = port_lin_GetAsyncKeyState;
    W->PeekMessageA_     = port_lin_PeekMessage;
    W->TranslateMessage_ = port_lin_TranslateMessage;
    W->DispatchMessageA_ = port_lin_DispatchMessage;
    W->PostQuitMessage_  = port_lin_PostQuitMessage;
    W->DefWindowProcA_   = port_lin_DefWindowProc;
    W->StretchDIBits_    = port_lin_StretchDIBits;
    W->GetCursorPos_     = port_lin_GetCursorPos;
    W->SetCursorPos_     = port_lin_SetCursorPos;
    W->ShowCursor_       = port_lin_ShowCursor;
    W->SetCapture_       = port_lin_SetCapture;
    W->ReleaseCapture_   = port_lin_ReleaseCapture;
    W->timeBeginPeriod_  = port_lin_timeBeginPeriod;
    W->timeEndPeriod_    = port_lin_timeEndPeriod;
    W->SetProcessInformation_ = port_lin_SetProcessInformation;
    W->GetProcessMemoryInfo_  = port_lin_GetProcessMemoryInfo;
    return true;
}

/* ---- QueryPerformanceCounter / Frequency / Sleep -------------------------- */
static inline BOOL QueryPerformanceFrequency(LARGE_INTEGER *f) {
    f->QuadPart = 1000000000LL;   /* clock_gettime is in nanoseconds */
    return TRUE;
}
static inline BOOL QueryPerformanceCounter(LARGE_INTEGER *c) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    c->QuadPart = (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
    return TRUE;
}
static inline void Sleep(DWORD ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    nanosleep(&ts, nullptr);
}

/* GetModuleHandleA / GetCurrentProcess: the crash-dump code names them. On
   Linux they are opaque handles nothing dereferences meaningfully here. */
static inline HMODULE GetModuleHandleA(const char *) { return nullptr; }
static inline HANDLE  GetCurrentProcess() { return nullptr; }

#endif /* PORT_HOST_PLATFORM_LINUX_H */
