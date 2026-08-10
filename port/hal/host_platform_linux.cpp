/* host_platform_linux.cpp -- SDL2 backing implementation for the Linux port.
 *
 * PHASE 1 LANE A. Compiled ONLY on non-Windows builds. Provides window
 * creation, the SDL event pump (routed into walk_window's own wndproc so the
 * mouse-look/touch/wheel logic is reused verbatim), the framebuffer blit, and
 * the plain-C entry points the WinApi pointer table in the header binds to.
 *
 * The Windows build never sees this file (CMake only adds it when NOT MSVC and
 * PORT_ENABLE_SDL2 is on), so the shipped walk_window.exe is unaffected.
 */
#ifndef _WIN32

#include "host_platform_linux.h"

#include <cstdlib>

static WNDPROC g_wndproc = nullptr;

extern "C" void port_lin_set_wndproc(WNDPROC proc) { g_wndproc = proc; }

extern "C" void port_lin_create_window(int fb_w, int fb_h, int zoom,
                                        const char *title) {
    port_lin::State &s = port_lin::S();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return;
    }
    s.win_w = fb_w * zoom;
    s.win_h = fb_h * zoom;
    s.win = SDL_CreateWindow(title ? title : "SM64DS",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             s.win_w, s.win_h, SDL_WINDOW_SHOWN);
    if (!s.win) { std::fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
                  return; }
    s.ren = SDL_CreateRenderer(s.win, -1,
                               SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!s.ren) s.ren = SDL_CreateRenderer(s.win, -1, 0);   /* software fallback */
    if (!s.ren) { std::fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
                  return; }
    /* The ntr raster is 32-bit 0xFFRRGGBB in memory, matching the DIB the Win32
       path fed StretchDIBits. ARGB8888 in SDL is that same word order, so the
       upload is a straight memcpy of the framebuffer rows. */
    s.tex = SDL_CreateTexture(s.ren, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, fb_w, fb_h);
    s.tex_w = fb_w; s.tex_h = fb_h;
    if (!s.tex) std::fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
}

/* ---- keyboard ------------------------------------------------------------ */
extern "C" short port_lin_GetAsyncKeyState(int vk) {
    if (vk == VK_LBUTTON)
        return port_lin::S().lbutton ? (short)0x8000 : 0;
    int sc = port_lin::vk_to_scancode(vk);
    if (sc < 0) return 0;
    const Uint8 *ks = SDL_GetKeyboardState(nullptr);
    return (ks && ks[sc]) ? (short)0x8000 : 0;   /* 0x8000 -> negative short */
}

/* ---- the message pump: SDL events -> walk_window's wndproc --------------- */
extern "C" BOOL port_lin_PeekMessage(MSG *m, HWND, UINT, UINT, UINT) {
    port_lin::State &s = port_lin::S();
    WNDPROC proc = g_wndproc;
    HWND hwnd = (HWND)1;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            m->message = WM_QUIT; return TRUE;
        case SDL_KEYDOWN:
            if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                m->message = WM_QUIT; return TRUE;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_RIGHT) {
                /* seed the synthetic cursor at the real cursor BEFORE the
                   wndproc reads it as the anchor, then enter relative mode */
                s.synth_x = s.last_lx; s.synth_y = s.last_ly;
                s.right_down = true; s.rel_dx = s.rel_dy = 0;
                SDL_SetRelativeMouseMode(SDL_TRUE);
                if (proc) proc(hwnd, WM_RBUTTONDOWN, 0, 0);
            } else if (e.button.button == SDL_BUTTON_LEFT) {
                s.lbutton = true;
                long l = ((long)(short)e.button.y << 16) | (short)e.button.x;
                if (proc) proc(hwnd, WM_LBUTTONDOWN, 0, l);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_RIGHT) {
                s.right_down = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
                if (proc) proc(hwnd, WM_RBUTTONUP, 0, 0);
            } else if (e.button.button == SDL_BUTTON_LEFT) {
                s.lbutton = false;
                if (proc) proc(hwnd, WM_LBUTTONUP, 0, 0);
            }
            break;
        case SDL_MOUSEMOTION:
            if (s.right_down) {
                /* advance the synthetic cursor by the relative delta; the
                   wndproc's GetCursorPos then reads synth, subtracts the anchor
                   (== the pre-look synth), and SetCursorPos-es back to it --
                   reproducing the Win32 spring-back exactly. */
                s.synth_x += e.motion.xrel; s.synth_y += e.motion.yrel;
                if (proc) proc(hwnd, WM_MOUSEMOVE, 0, 0);
            } else {
                s.last_lx = e.motion.x; s.last_ly = e.motion.y;
            }
            break;
        case SDL_MOUSEWHEEL: {
            unsigned w = ((unsigned)(short)(e.wheel.y * WHEEL_DELTA)) << 16;
            if (proc) proc(hwnd, WM_MOUSEWHEEL, w, 0);
            break;
        }
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST && s.right_down) {
                s.right_down = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
                if (proc) proc(hwnd, WM_KILLFOCUS, 0, 0);
            }
            break;
        default: break;
        }
    }
    if (s.quit) { s.quit = false; m->message = WM_QUIT; return TRUE; }
    return FALSE;   /* no queued message left */
}

extern "C" BOOL    port_lin_TranslateMessage(const MSG *) { return TRUE; }
extern "C" LRESULT port_lin_DispatchMessage(const MSG *) { return 0; }
extern "C" void    port_lin_PostQuitMessage(int) { port_lin::S().quit = true; }
extern "C" LRESULT port_lin_DefWindowProc(HWND, UINT, WPARAM, LPARAM) { return 0; }

/* ---- the blit: framebuffer -> streaming texture -> present --------------- */
extern "C" int port_lin_StretchDIBits(HDC, int, int, int, int, int, int,
                                      int sw, int sh, const void *bits,
                                      const BITMAPINFO *, UINT, DWORD) {
    port_lin::State &s = port_lin::S();
    if (!s.tex || !s.ren || !bits) return 0;
    SDL_UpdateTexture(s.tex, nullptr, bits, sw * 4);
    SDL_RenderClear(s.ren);
    SDL_RenderCopy(s.ren, s.tex, nullptr, nullptr);   /* stretch to window */
    SDL_RenderPresent(s.ren);
    return sh;
}

/* ---- mouse --------------------------------------------------------------- */
extern "C" BOOL port_lin_GetCursorPos(POINT *p) {
    port_lin::State &s = port_lin::S();
    if (s.right_down) { p->x = s.synth_x; p->y = s.synth_y; }
    else              { p->x = s.last_lx; p->y = s.last_ly; }
    return TRUE;
}
extern "C" BOOL port_lin_SetCursorPos(int x, int y) {
    port_lin::S().synth_x = x; port_lin::S().synth_y = y; return TRUE;
}
extern "C" int port_lin_ShowCursor(BOOL show) {
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
    return show ? 0 : -1;
}
extern "C" HWND port_lin_SetCapture(HWND h) { return h; }
extern "C" BOOL port_lin_ReleaseCapture(void) {
    SDL_SetRelativeMouseMode(SDL_FALSE); return TRUE;
}

/* ---- timing / process no-ops --------------------------------------------- */
extern "C" unsigned port_lin_timeBeginPeriod(unsigned) { return 0; }
extern "C" unsigned port_lin_timeEndPeriod(unsigned) { return 0; }
extern "C" BOOL port_lin_SetProcessInformation(HANDLE, int, void *, DWORD) {
    return TRUE;
}
extern "C" BOOL port_lin_GetProcessMemoryInfo(HANDLE, void *, DWORD) {
    return FALSE;   /* the overlay's working-set line prints nothing on Linux */
}

#endif /* !_WIN32 */
