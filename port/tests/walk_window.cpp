// Gate 12: the interactive window. Mario walks under keyboard control.
//
// Same staging as smoke_player (sinits, spawn context, castle-grounds KCL,
// InitResources, St_Wait), then a Win32 frame loop: keys write the pad
// block, Stage::CheckInput turns them into the stick record, Player::Behavior
// and Camera::Behavior tick, Camera::Render builds the projection and the
// view matrix, and the framebuffer blits into the client.
//
// Since gate 13 the camera IS the game's: the Camera actor at 0x14C, its
// 19-state machine, its mode-preset table and its own published heading --
// which is what turns "forward" on the stick into a world direction.
//
//   WASD / arrows  walk    Q/E  orbit    C  behind Mario    ESC  quit
//   F1  cycles the camera: ANALOG (the default) -> FREECAM -> DS-EXACT.
//   Analog is a chase rig the port owns: it orbits Mario on the right stick's
//   analog curve instead of the DS's 5.625-degree steps, tilts, zooms on the
//   bumpers, and drifts back behind him when the stick is idle and he is
//   moving. DS-exact is the hardware's stepped rotate, unchanged. In every
//   mode the Camera actor itself keeps running and is never written to.
//   MOUSE  hold the right button and drag to look (the pointer springs back
//   to where it was picked up, so the look never runs out of desk); the wheel
//   zooms. Both work in analog and in freecam and do nothing in DS-exact.
//   The last left click is published in framebuffer pixels for the touch
//   bridge; see g_mouse_click_x.
//   MOUSE CAPTURE  settings.json's MouseCapture, off by default. On, an
//   ordinary adventure window HOLDS the pointer -- hidden, parked in the
//   middle of the picture, clipped to the window -- and bare movement turns
//   the camera with no button held. ESCAPE HANDS THE POINTER BACK, because
//   escape opens the debug menu and the capture is never on while the menu
//   is open; closing the menu takes it again. Alt-tab hands it back too. It
//   never engages where the mouse is really the DS's stylus: the whole scene
//   path (the minigames), any stacked window, and DS-exact. The setting
//   reloads live, so the launcher can flip it mid-play. See the MOUSE banner
//   above mo_look for the derivation and for what it takes away.
//   F5 or ESC  the debug menu: warp to any of the level's own entrances, the
//   level select, the minigame picker, the fake-snap A/B, the overlay, the
//   camera mode, how running works, and the recorder's filename.
//   WASD, the arrows or the d-pad move, enter or A acts, F5 or ESC or BACK or
//   B closes. ESCAPE DOES NOT CLOSE THE GAME. The window's close button and
//   alt+F4 do, and they are the only things that do. It PAUSES THE GAME TICK while it is open and keeps rendering, so
//   the scene freezes and the view does not. The walk keys are aliases of the
//   arrows here and are taken off the game for as long as it is open, so
//   nothing that reads the menu also walks.
//   THE MINIGAME ROW lists all thirty ids the ROM's own IsMinigameActorID
//   accepts, under the retail titles a player would recognise, and marks the
//   ones the port can host. It is ordered WIRED FIRST: the scenes that start
//   come before the ones that do not, each group in id order, and the "N of M"
//   counter restarts at 1 where the wired set ends. Selecting a hosted one
//   starts the program again on the scene path with SM64DS_SCENE and
//   SM64DS_DUAL_SCREEN set and quits this process;
//   a level cannot enter a minigame in place, because loading ov006 unloads
//   the ov002 the level is running out of. Selecting an unhosted one says so
//   and does nothing. See the MG_SCENE table for the derivation of both.
//   RUN MODE is two of its rows. The DS had no run button -- it ran off how
//   far the touch-screen stick was pushed -- so every way of running on a host
//   is the port's own choice, and the menu is where that choice is made:
//   BUTTON holds run on a key or a pad button (shift and X by default, and
//   this is what the window has always done), ANALOG reads the run out of the
//   left stick's deflection the way the hardware read it out of the touch
//   stick, and AUTO holds it down for you. The rebind row captures the next
//   key or pad button pressed. All three are settings.json keys, so a choice
//   outlives the run; see the RUN_ block above the menu enum for the mapping
//   and port/hal/host_settings.cpp for the file.
//   F3  the stats overlay: frame rate, the per-phase millisecond budget,
//   triangle and actor counts, where Mario is and what state he is in, and
//   how many times the port has fallen through a state it does not host.
//   Drawn into the framebuffer, so it survives into the selftest BMP.
//   F1 (or a click of the right stick)  the FREECAM mod: the harness takes
//   the view, the right stick orbits and tilts it, the bumpers or R/F zoom,
//   C re-centres it behind Mario. F1 again hands the view back. Everything
//   else in the window is the game's; that one is not, and says so.
//
// THE GAME'S OWN BOOT AND THE GAME'S OWN PHYSICS ARE THE DEFAULT. ov009 is
// mounted, Stage::LoadClsnAndObjects runs against it, the level's entrance
// record spawns the Player and the Camera, and the ground and wall contact
// come from WithMeshClsn's own tracking through the hosted sphere pass. No
// harness stands in for anything in the physics loop.
//
// Interactive keyboard and mouse only act while this window is the FOREGROUND
// one. Alt-tab away and the stick and the buttons go to neutral that frame;
// come back and whatever was still held has to be released before it counts
// again, so no press made in another window arrives here late. None of the
// scripted input paths go through that gate: a selftest, SM64DS_PROBE_INPUT and
// the SM64DS_SELFTEST_* probes drive a hidden, unfocused run exactly as before.
//
// Env: SM64DS_LEGACY_BOOT=1 the pre-gate-14 harness staging instead of the
//                           level's own boot (hand-built spawn context, KCL
//                           mounted by hand, no entrance record)
//      SM64DS_FAKE_SNAP=1   the pre-sphere collision scaffolding: the level
//                           collider owned by the Player, the harness ground
//                           snap and the harness wall clamp. Retired, kept
//                           for A/B and for shots that need Mario planted.
//      SM64DS_NO_SPHERE=1   stub the sphere pass out (port/hal/clsn_vtable),
//                           which is the honest way to see what the
//                           scaffolding was covering for: a 28-unit bob at
//                           20 Hz that never settles
//      SM64DS_OLD_CAMERA=1  the pre-gate-13 hand-tuned follow rig
//      SM64DS_FREECAM=1     start in the freecam (F1 cycles either way)
//      SM64DS_DS_CAMERA=1   start in DS-exact stepped rotate. This is also
//                           what a SELFTEST defaults to: its BMP is a
//                           byte-comparison against the hardware's framing and
//                           its camera probes drive the DS rotate bits.
//      SM64DS_ANALOG_CAMERA=1  put a selftest in the analog camera
//      SM64DS_OVERLAY=1     boot with the F3 stats overlay already on
//      SM64DS_MENU=1        boot with the F5 debug menu open
//      SM64DS_TRACE_PACE=1  per-frame pacer trace: the work time, the sleep
//                           that was asked for and the sleep that happened.
//                           The third number is the one that matters -- see
//                           the pacer block for what makes it lie.
//      SM64DS_TRACE_CAM=1   per-frame camera input trace: the pad words the
//                           rotate logic reads, the camera's heading, its
//                           two latches, the rig, and the published angle
//      SM64DS_SELFTEST_STICK=<pct>  drive the right stick from a selftest
//                           (0 = ramp to the stop); it lets go, and
//                           SM64DS_SELFTEST_FREECAM=1 toggles the mod on
//                           and off, at the same two points
//      SM64DS_SELFTEST_JUMPSPAM=<period>  a ground jump every <period>
//                           frames from f20, and
//      SM64DS_JUMP_PROBE=1  the per-frame cost line that goes with it: the
//                           raw phase times, the file loads and their
//                           milliseconds, and the Player's animation word
//                           beside the ModelAnim's playback cursor.
//
//                           WHAT IT MEASURED (2026-08-05, 3x300 frames,
//                           JUMPSPAM=40, seven jumps a run): the per-jump
//                           animation load is NOT a frame hitch and never
//                           was. The 20 frames that change animation average
//                           6.73ms and the other 279 average 6.71ms; the
//                           slowest animation-change frame (7.51ms) is
//                           faster than the slowest ordinary one (9.45ms);
//                           no frame in 300 comes within 3x of the 33.3ms
//                           budget. Every load costs 0.001-0.014ms and the
//                           whole run's file work totals 0.056ms.
//                           SM64DS_FS_NOCACHE=1 does not change that: the
//                           Player's animations are 1.2-2.0 KB NARC
//                           interiors, so even a full re-read and LZ77
//                           decode is ~10us. The jump itself is clean --
//                           animation 83 advances exactly 1.000 per frame
//                           for its 13 frames while y arcs 254->419->254,
//                           then the clamp flag holds the last pose for the
//                           three frames of descent that are left.
//      SM64DS_SELFTEST_LONGJUMP=1  run, crouch, jump: the long jump, for
//                           the leg-twist investigation. Pair with
//      SM64DS_BONE_PROBE=1..3  the per-frame bone rotation dump; =3 checks
//                           every bone against an independent shortest-path
//                           reference. See the probe for what it measured.
//      SM64DS_INPUT_NOFOCUSGATE=1  read the interactive keyboard whether this
//                           window has focus or not, the way it worked before
//                           the gate. Nothing in the tree sets it. It is here
//                           so a harness that genuinely wants background key
//                           reads has a documented switch instead of a patch.
//      SM64DS_NO_FOCUS=1    (=0 turns it off; this one is read with atoi, not
//                           as a presence test, because it is meant to be set
//                           broadly and opted out of per run)
//                           open the window WITHOUT taking the foreground, so
//                           a scripted run does not yank the keyboard out of
//                           whatever the owner of the desk is typing into. See
//                           nofocus_mode() below for what it does and what it
//                           refuses to do. NOT the same knob as
//                           SM64DS_INPUT_NOFOCUSGATE above and close to its
//                           opposite: that one reads keys with the window in
//                           the background, this one PUTS the window in the
//                           background. Setting both is a scripted run that
//                           reads the owner's keystrokes, which is why they are
//                           named apart here rather than only in the code.
//      SM64DS_WINDOW_POS=x,y  put the window's top-left corner at that virtual
//                           screen point instead of centring it, so a run under
//                           SM64DS_NO_FOCUS can be parked in a corner. Independent
//                           of that flag; either works without the other.
//
//   AND THE LAUNCHER'S OWN SHOW REQUEST, which is not an env var. `start /min`
//   and PowerShell's -WindowStyle Minimized put a wShowWindow in STARTUPINFO,
//   and until this lane the game window IGNORED it (it was created WS_VISIBLE,
//   so CreateWindowExA showed it and nothing ever read the request). Only the
//   console the OS makes for this exe was minimized, which looked like the whole
//   thing working. host_show_mode() below now honours the minimize and
//   no-activate spellings; SW_HIDE is deliberately still ignored, and every
//   ordinary show is unchanged.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>   /* _execl: the debug menu's level row relaunches */

/* user32/gdi32 are loaded DYNAMICALLY after io_init: a static import chain
   initializes the desktop heap before main, and on 32-bit that mapping can
   land inside the fixed DS regions (0x04000000..0x07ffffff), killing
   io_init deterministically. Resolving late keeps the address space ours
   first. */
struct WinApi {
    ATOM(WINAPI *RegisterClassA_)(const WNDCLASSA *);
    HWND(WINAPI *CreateWindowExA_)(DWORD, LPCSTR, LPCSTR, DWORD, int, int,
                                   int, int, HWND, HMENU, HINSTANCE, LPVOID);
    /* SM64DS_NO_FOCUS's other half. The window is created without WS_VISIBLE
       under that flag and shown with SW_SHOWNOACTIVATE here, because
       WS_EX_NOACTIVATE alone is a claim about what a CLICK does to the window
       and this is the call that decides whether the SHOW activates it. Loaded
       in every build: a show is not an input driver (port/release_hardening.txt
       gates the SendInput family, not this). */
    BOOL(WINAPI *ShowWindow_)(HWND, int);
    LRESULT(WINAPI *DefWindowProcA_)(HWND, UINT, WPARAM, LPARAM);
    BOOL(WINAPI *PeekMessageA_)(MSG *, HWND, UINT, UINT, UINT);
    BOOL(WINAPI *TranslateMessage_)(const MSG *);
    LRESULT(WINAPI *DispatchMessageA_)(const MSG *);
    void(WINAPI *PostQuitMessage_)(int);
    HDC(WINAPI *GetDC_)(HWND);
    HCURSOR(WINAPI *LoadCursorA_)(HINSTANCE, LPCSTR);
    BOOL(WINAPI *AdjustWindowRect_)(RECT *, DWORD, BOOL);
    SHORT(WINAPI *GetAsyncKeyState_)(int);
    int(WINAPI *StretchDIBits_)(HDC, int, int, int, int, int, int, int, int,
                                const void *, const BITMAPINFO *, UINT, DWORD);
    /* the present path's own surface: the client rect the frame is fitted
       into, the bars around it, and the two stretch modes (see present()) */
    BOOL(WINAPI *GetClientRect_)(HWND, RECT *);
    BOOL(WINAPI *ValidateRect_)(HWND, const RECT *);
    BOOL(WINAPI *PatBlt_)(HDC, int, int, int, int, DWORD);
    int(WINAPI *SetStretchBltMode_)(HDC, int);
    BOOL(WINAPI *SetBrushOrgEx_)(HDC, int, int, POINT *);
    /* F12's borderless fullscreen: style swap, monitor bounds, and the
       placement the restore springs back to */
    LONG(WINAPI *GetWindowLongA_)(HWND, int);
    LONG(WINAPI *SetWindowLongA_)(HWND, int, LONG);
    BOOL(WINAPI *SetWindowPos_)(HWND, HWND, int, int, int, int, UINT);
    HMONITOR(WINAPI *MonitorFromWindow_)(HWND, DWORD);
    /* and the monitor the POINTER is on, which is the one a new window
       belongs on when there is more than one (host_window_open's centring) */
    HMONITOR(WINAPI *MonitorFromPoint_)(POINT, DWORD);
    BOOL(WINAPI *GetMonitorInfoA_)(HMONITOR, MONITORINFO *);
    BOOL(WINAPI *GetWindowPlacement_)(HWND, WINDOWPLACEMENT *);
    BOOL(WINAPI *SetWindowPlacement_)(HWND, const WINDOWPLACEMENT *);
    HWND(WINAPI *SetCapture_)(HWND);
    BOOL(WINAPI *ReleaseCapture_)(void);
    BOOL(WINAPI *GetCursorPos_)(POINT *);
    BOOL(WINAPI *SetCursorPos_)(int, int);
    int(WINAPI *ShowCursor_)(BOOL);
    /* MouseCapture's fence. NOT one of the click-test drivers below and not
       compiled out with them: this one is a PLAYER setting on the shipping
       path, and a null here is simply a capture that hides and re-centres the
       pointer without also penning it in. */
    BOOL(WINAPI *ClipCursor_)(const RECT *);
    /* ClientToScreen is a COORDINATE CONVERSION, not an input driver: it reads
       a window's position and adds it to a point. It sat inside the
       PORT_ROM_CLEAN fence below when that fence was written, correctly, because
       SM64DS_CLICK_TEST was the only caller. Then the mouse capture landed
       (b91d34ed7) and mo_client_center / mo_capture_clip / mo_capture_refresh
       -- all three on the SHIPPING path, all three serving a PLAYER setting --
       started calling it, and
       the shipping build stopped compiling. Nothing builds PORT_ROM_CLEAN
       routinely, so that went unnoticed from b91d34ed7 to here.
       It belongs on this side of the fence: release_hardening.txt gates the OS
       input DRIVERS (SendInput and the foreground/window-ownership calls that
       aim one), and this is neither. */
    BOOL(WINAPI *ClientToScreen_)(HWND, POINT *);
#ifndef PORT_ROM_CLEAN
    /* SM64DS_CLICK_TEST's: the pointer put at a screen point, and a REAL button
       edge through the OS input queue rather than a message posted past it.
       Compiled out of a PORT_ROM_CLEAN (shipping) build: these are the OS-input
       DRIVERS a stranger's antivirus flags, and nothing outside SM64DS_CLICK_TEST
       uses them. Real input reading (GetCursorPos_/GetAsyncKeyState_) stays. */
    UINT(WINAPI *SendInput_)(UINT, void *, int);
    BOOL(WINAPI *SetForegroundWindow_)(HWND);
    /* and the two that make the synthetic click SAFE on a shared desktop:
       whose window is actually under that screen point, and which process owns
       it. A real button edge goes wherever the pointer is. */
    HWND(WINAPI *WindowFromPoint_)(POINT);
    DWORD(WINAPI *GetWindowThreadProcessId_)(HWND, DWORD *);
#endif  /* !PORT_ROM_CLEAN: SM64DS_CLICK_TEST's synthetic-click Win32 seams */
    /* winmm: the frame pacer's Sleep granularity (see pacer_begin below) */
    unsigned(WINAPI *timeBeginPeriod_)(unsigned);
    unsigned(WINAPI *timeEndPeriod_)(unsigned);
    /* kernel32: opting out of the power throttling that would otherwise make
       the line above a no-op (see pacer_begin) */
    BOOL(WINAPI *SetProcessInformation_)(HANDLE, int, void *, DWORD);
    /* psapi: the overlay's working-set line */
    BOOL(WINAPI *GetProcessMemoryInfo_)(HANDLE, void *, DWORD);
};
static WinApi W;

/* PROCESS_MEMORY_COUNTERS, spelled here rather than including psapi.h, so the
   dynamic-load pattern above is the only dependency on the DLL. */
struct PortMemCounters {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize, WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage, QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage, QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage, PeakPagefileUsage;
};

/* XInput, loaded dynamically like user32 (no static import chain) */
struct XPad {
    unsigned long packet;
    unsigned short buttons;
    unsigned char lt, rt;
    short lx, ly, rx, ry;
};
static DWORD(WINAPI *XInputGetState_)(DWORD, XPad *);

static bool winapi_load(void)
{
    HMODULE u = LoadLibraryA("user32.dll");
    HMODULE g = LoadLibraryA("gdi32.dll");
    if (!u || !g) return false;
    W.RegisterClassA_ = (decltype(W.RegisterClassA_))GetProcAddress(u, "RegisterClassA");
    W.CreateWindowExA_ = (decltype(W.CreateWindowExA_))GetProcAddress(u, "CreateWindowExA");
    W.ShowWindow_ = (decltype(W.ShowWindow_))GetProcAddress(u, "ShowWindow");
    W.DefWindowProcA_ = (decltype(W.DefWindowProcA_))GetProcAddress(u, "DefWindowProcA");
    W.PeekMessageA_ = (decltype(W.PeekMessageA_))GetProcAddress(u, "PeekMessageA");
    W.TranslateMessage_ = (decltype(W.TranslateMessage_))GetProcAddress(u, "TranslateMessage");
    W.DispatchMessageA_ = (decltype(W.DispatchMessageA_))GetProcAddress(u, "DispatchMessageA");
    W.PostQuitMessage_ = (decltype(W.PostQuitMessage_))GetProcAddress(u, "PostQuitMessage");
    W.GetDC_ = (decltype(W.GetDC_))GetProcAddress(u, "GetDC");
    W.LoadCursorA_ = (decltype(W.LoadCursorA_))GetProcAddress(u, "LoadCursorA");
    W.AdjustWindowRect_ = (decltype(W.AdjustWindowRect_))GetProcAddress(u, "AdjustWindowRect");
    W.GetAsyncKeyState_ = (decltype(W.GetAsyncKeyState_))GetProcAddress(u, "GetAsyncKeyState");
    W.StretchDIBits_ = (decltype(W.StretchDIBits_))GetProcAddress(g, "StretchDIBits");
    W.GetClientRect_ = (decltype(W.GetClientRect_))GetProcAddress(u, "GetClientRect");
    W.ValidateRect_ = (decltype(W.ValidateRect_))GetProcAddress(u, "ValidateRect");
    W.PatBlt_ = (decltype(W.PatBlt_))GetProcAddress(g, "PatBlt");
    W.SetStretchBltMode_ =
        (decltype(W.SetStretchBltMode_))GetProcAddress(g, "SetStretchBltMode");
    W.SetBrushOrgEx_ =
        (decltype(W.SetBrushOrgEx_))GetProcAddress(g, "SetBrushOrgEx");
    W.GetWindowLongA_ = (decltype(W.GetWindowLongA_))GetProcAddress(u, "GetWindowLongA");
    W.SetWindowLongA_ = (decltype(W.SetWindowLongA_))GetProcAddress(u, "SetWindowLongA");
    W.SetWindowPos_ = (decltype(W.SetWindowPos_))GetProcAddress(u, "SetWindowPos");
    W.MonitorFromWindow_ =
        (decltype(W.MonitorFromWindow_))GetProcAddress(u, "MonitorFromWindow");
    W.MonitorFromPoint_ =
        (decltype(W.MonitorFromPoint_))GetProcAddress(u, "MonitorFromPoint");
    W.GetMonitorInfoA_ =
        (decltype(W.GetMonitorInfoA_))GetProcAddress(u, "GetMonitorInfoA");
    W.GetWindowPlacement_ =
        (decltype(W.GetWindowPlacement_))GetProcAddress(u, "GetWindowPlacement");
    W.SetWindowPlacement_ =
        (decltype(W.SetWindowPlacement_))GetProcAddress(u, "SetWindowPlacement");
    W.SetCapture_ = (decltype(W.SetCapture_))GetProcAddress(u, "SetCapture");
    W.ReleaseCapture_ = (decltype(W.ReleaseCapture_))GetProcAddress(u, "ReleaseCapture");
    W.GetCursorPos_ = (decltype(W.GetCursorPos_))GetProcAddress(u, "GetCursorPos");
    W.SetCursorPos_ = (decltype(W.SetCursorPos_))GetProcAddress(u, "SetCursorPos");
    W.ShowCursor_ = (decltype(W.ShowCursor_))GetProcAddress(u, "ShowCursor");
    W.ClipCursor_ = (decltype(W.ClipCursor_))GetProcAddress(u, "ClipCursor");
    /* the mouse capture's coordinate conversion; see the declaration's note on
       why it is NOT behind the fence below */
    W.ClientToScreen_ = (decltype(W.ClientToScreen_))GetProcAddress(u, "ClientToScreen");
#ifndef PORT_ROM_CLEAN
    /* SM64DS_CLICK_TEST's synthetic-click seams. Their GetProcAddress lookups
       -- and the "SendInput"/"SetForegroundWindow"/... name strings with them
       -- are absent from a PORT_ROM_CLEAN (shipping) build, so the shipped exe
       carries no SendInput driver for a stranger's antivirus to flag. */
    W.SendInput_ = (decltype(W.SendInput_))GetProcAddress(u, "SendInput");
    W.SetForegroundWindow_ =
        (decltype(W.SetForegroundWindow_))GetProcAddress(u, "SetForegroundWindow");
    W.WindowFromPoint_ = (decltype(W.WindowFromPoint_))GetProcAddress(u, "WindowFromPoint");
    W.GetWindowThreadProcessId_ = (decltype(W.GetWindowThreadProcessId_))
        GetProcAddress(u, "GetWindowThreadProcessId");
#endif  /* !PORT_ROM_CLEAN */
    if (HMODULE mm = LoadLibraryA("winmm.dll")) {
        W.timeBeginPeriod_ =
            (decltype(W.timeBeginPeriod_))GetProcAddress(mm, "timeBeginPeriod");
        W.timeEndPeriod_ =
            (decltype(W.timeEndPeriod_))GetProcAddress(mm, "timeEndPeriod");
    }
    /* GetProcessMemoryInfo lives in psapi.dll, and since Windows 7 also in
       kernel32 under the K32 prefix; take whichever answers. */
    if (HMODULE ps = LoadLibraryA("psapi.dll"))
        W.GetProcessMemoryInfo_ = (decltype(W.GetProcessMemoryInfo_))
            GetProcAddress(ps, "GetProcessMemoryInfo");
    if (HMODULE k = GetModuleHandleA("kernel32.dll")) {
        if (!W.GetProcessMemoryInfo_)
            W.GetProcessMemoryInfo_ = (decltype(W.GetProcessMemoryInfo_))
                GetProcAddress(k, "K32GetProcessMemoryInfo");
        W.SetProcessInformation_ = (decltype(W.SetProcessInformation_))
            GetProcAddress(k, "SetProcessInformation");
    }
    {
        const char *dlls[] = {"xinput1_4.dll", "xinput1_3.dll",
                              "xinput9_1_0.dll"};
        for (int i = 0; i < 3 && !XInputGetState_; ++i)
            if (HMODULE x = LoadLibraryA(dlls[i]))
                XInputGetState_ = (decltype(XInputGetState_))GetProcAddress(
                    x, "XInputGetState");
    }
    return W.RegisterClassA_ && W.CreateWindowExA_ && W.DefWindowProcA_ &&
           W.PeekMessageA_ && W.StretchDIBits_ && W.GetAsyncKeyState_;
}

#include "ntr/gx.h"
#include "ntr/mmio.h"
#include "ntr/ppu.h"
#include "ntr/rt.h"

/* walk_window is the one TU that installs the crash probe, so it also emits the
   external seams (port_rich_dump_ex, port_crash_dir_get) the quarantine walker
   in port/unmatched/func_02043fdc_hostcopy.cpp weak-links against. */
#define PORT_FAULT_PROBE_DEFINE_EXPORTS
#include "fault_probe.h"
#include "overlay_font.h"
#include "hal/host_settings.h"   /* settings.json, the launcher's file */
#include "hal/comms_seam.h"       /* run mg15 lane MP1: the radio seam */
#include "hal/comms_loopback.h"   /* run mg16 lane MP2: the loopback carrier */
/* run mg16 lane MP3: hal/comms_lockstep.h is RETIRED. Its transcription of
   src/func_0203ea5c.c existed only because that TU was in no slice; the TU is
   linked now and drives itself. Its lessons live in comms_seam.h's frozen
   contract and in hal/comms_conductor.cpp. */
#include "hal/instance_tag.h"     /* run mg16 lane MP2: per-instance filenames */
#include "hal/editor_channel.h"   /* run lvled lane B: the editor control channel */

/* run mg16 lane MP3: the raw DS pad bits for this frame, handed from where the
   harness computes them to where hal/comms_conductor.cpp publishes them into
   the DS key register. One value, one frame, no reader outside this file --
   a stash rather than a hosted global precisely because it is harness
   plumbing and not DS state, so it stays out of the .dsstate bracket. */
static unsigned short g_raw_pad_bits;
static void port_raw_pad_stash(unsigned short raw) { g_raw_pad_bits = raw; }
static unsigned port_raw_pad_bits(void) { return g_raw_pad_bits; }

/* run mg16 lane MPBTN: the BUTTON half of the same stash, and the reason it
   was missing is the whole of "no buttons in multiplayer". The d-pad stash
   above was the only thing comms_publish_pad ever received, so the published
   key word carried the four direction bits and nothing else -- measured over
   every playlog pair in runs/mg16/out/MP2/two_windows: the local key word and
   every fanned pad{held} are drawn from {0000,0010..00a0}, the d-pad nibble,
   in the 08-27 sessions where buttons "worked" (through the then-ungated
   direct Ctrl stores) exactly as in the 08-29 session where they died.

   THE CONVENTION CHANGES AT THIS LINE and that is what the translator is for.
   host_ds_buttons speaks the game's REMAPPED Ctrl convention (jump=2,
   punch=1, crouch=0x400, run=0x800), because it used to feed the Ctrl words
   directly. The key register wants the DS's RAW pad convention, because
   src/func_0203df40.c publishes it and src/_ZN5Stage10CheckInputEv.cpp reads
   it back through the mode-0 remap map at data_02075650 (ROM bytes: A->1,
   B->2, R->0x400, Y->0x800, L->0x4000, X->0x8000). So the translation below
   is that map's exact inverse, row by row, and nothing else crosses:
   the Ctrl-only camera-rotate bits 0x100/0x200 have NO raw source in mode 0
   (on the DS they are the touch screen's arrows, Stage::CheckCameraInput),
   and passing them through as raw bits would land on R and L -- a phantom
   crouch on every camera turn. */
static unsigned short g_raw_btn_bits;
static void port_raw_btn_stash(unsigned short raw) { g_raw_btn_bits = raw; }
static unsigned port_raw_btn_bits(void) { return g_raw_btn_bits; }
static unsigned short host_btn_to_raw_keys(unsigned short btn)
{
    unsigned short raw = 0;
    if (btn & 0x0001) raw |= 0x0001;   /* punch : Ctrl 0x0001 <- raw A */
    if (btn & 0x0002) raw |= 0x0002;   /* jump  : Ctrl 0x0002 <- raw B */
    if (btn & 0x0400) raw |= 0x0100;   /* crouch: Ctrl 0x0400 <- raw R */
    if (btn & 0x0800) raw |= 0x0800;   /* run   : Ctrl 0x0800 <- raw Y */
    if (btn & 0x4000) raw |= 0x0200;   /* snap  : Ctrl 0x4000 <- raw L */
    return raw;
}

/* run mg15 lane MP1. SM64DS_COMMS_FANOUT=1 runs the ROM's own steps 0x16 and
   0x17 (src/func_0203bb60.c, src/func_0203bc7c.c) after the comms tick, so
   TouchInfo[4] and PadData[4] come out of the four comms records the way the
   DS builds them instead of being written directly by the port. OFF by
   default: the swap is MP2's, and MP1's byte-identical solo proof depends on
   the default path being untouched.
   SM64DS_COMMS_REPORT=1 additionally prints the four slots each frame
   (port::comms_report), which is the instrument the two-instance stylus proof
   reads its verdict off. */
static bool comms_fanout_on() {
    static int v = -1;
    if (v < 0) v = getenv("SM64DS_COMMS_FANOUT") ? 1 : 0;
    return v != 0;
}
static bool comms_fanout_report() {
    static int v = -1;
    if (v < 0) v = getenv("SM64DS_COMMS_REPORT") ? 1 : 0;
    return v != 0;
}
extern "C" void out_set_volume_pct(int);  /* hal/sdat/out_win.cpp */

/* The run-mode half of settings.json (hal/host_settings.cpp). Declared here
   rather than in hal/host_settings.h because the header belongs to another
   stream in this campaign and this lane owns only the two files it edits;
   the linkage is plain extern "C" either way, so folding these three lines
   into the header later is a move, not a change. */
extern "C" int host_setting_run_mode(void);
extern "C" int host_setting_run_key(void);
extern "C" int host_setting_run_pad(void);
extern "C" int host_setting_save_run(int mode, int key, int pad);

typedef unsigned int u32;

extern "C" {
/* The hosted-DS section sentinels (hal/dsstate_seg.cpp). Their addresses are
   the .dsstate span, printed beside the selftest BMP as that run's layout. */
extern char dsstate_lo, dsstate_hi;
void *_ZN6PlayerC1Ev(void *self);
/* Player::InitResources' own character-propagation helper: +0x6d9 forward
   into the swap pair at +0x6dc/+0x6dd (src/func_ov002_020beabc.cpp) */
void func_ov002_020beabc(void *p);
void *_ZN4Heap13SetupRootHeapEv(void);
/* the ROM's entry into the above: clears the OS globals word, tail-calls it.
   Seated from src/_ZN4Heap18InitializeRootHeapEv.cpp; the C name reaches the
   matched TU's static member through an /alternatename in hal/cxx_aliases.cpp,
   which is ABI-legal here because a static member is __cdecl like this
   declaration. See the call site for why the boot guard moved to
   data_020a0ea0. */
void _ZN4Heap18InitializeRootHeapEv(void);
/* the ROM's own game-heap factory, off the main.c boot spine -- see the call
   site below for the two arguments and where they come from */
void _ZN4Heap18InitializeGameHeapEjPS_(unsigned size, void *root);
/* the game heap's allocator, read for the boot report only: how much of the
   ROM's 0x3b000 the port's boot actually spends */
unsigned _ZN22ExpandingHeapAllocator10MemoryLeftEv(void *self);
void *_ZN9ActorBasenwEj(unsigned size);
extern int data_0209b3ec[12];
extern unsigned short data_020a4b54;
extern void **data_020a4bb8;
extern void *data_020a0eac_c;
extern void *data_020a0ea0;
void hal_fill_model_vtable(void);
void hal_fill_shadow_vtable(void);
void hal_fill_mmc_vtable(void);
void hal_fill_modelanim2_vtable(void);
int hal_player_init_resources(void *p);
int hal_player_st_wait_init(void *p);
int hal_player_st_wait_main(void *p);
int hal_player_behavior(void *p);
int hal_player_process(void *p);   /* gate 15: BeforeBehavior/Behavior/After */
void sdat_host_tick(void);         /* hosted ARM7: hal/sdat/ */
void hal_render_player_world(void *p);
extern char data_0209f4a0[];
extern int data_0209f4a6[];   /* pad stick WORLD angle -- auto_bss split
                                 symbol, NOT data_0209f4a0+6 on host */
/* the real input processor (Stage::CheckInput) and its environment */
void _ZN5Stage10CheckInputEv(void);
/* Stage::Behavior's per-frame cutscene-script advance (Stage::Behavior:112).
   Matched src (src/ProcessKuppaScript.cpp), self-guarded: returns at once when
   data_0209fc48 == 0. Transcribed into the game tick below (STAR1). */
void ProcessKuppaScript(void);
void EndKuppaScript(void);     /* STAR1 proof: clears data_0209fc48 at cutscene end */
void port_intro_bit_edge(void); /* hal/level_boot.cpp: flags2 bit 7, edge-triggered */
/* the ROM's own atan2, the one CheckInput builds the stick record's angle
   with. The analog run mode below fills that record from a host stick, so it
   goes through the same function rather than a host atan2f: same table, same
   quantization, same answer the touch path would have produced. */
short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
unsigned int _ZNK6Player14GetBodyModelIDEjb(char *, unsigned int, char);
extern int data_0209f498[];    /* CheckInput's own Ctrl[4] block */
extern int data_0209f4a2[];    /* split: stick nx */
extern int data_0209f4a4[];    /* split: stick ny */
extern unsigned char data_0209f4ac[]; /* split: touching */
extern int data_020a0e58[];    /* PadData[4]: u16 held, u16 pressed */
/* TouchData[4], zero = no touch. DECLARED AS BYTES, not ints: the definition
   in hal/auto_bss.cpp is unsigned char[1] with de9/dea/deb packed at +1/+2/+3,
   the DS layout. An `int[]` here would be a lie about the element type; it is
   inert only because nothing in this file dereferences it. */
extern unsigned char data_020a0de8[];
extern unsigned char data_0209f21c;   /* controller count */
extern int data_0209f350[];    /* per-pad status */
extern int data_020a1164[];    /* camera per-player block; +0 = angle
                                  (GetAngleToCamera reads it) */
extern int data_0209caa0[];
extern unsigned char data_0209d660;
extern int data_0209fc48;
extern unsigned char data_0209f2d8;
/* STAR1 repro/proof: the star-get camera kuppa script and its per-frame cursor.
   func_0200ee8c launches the script (sets data_0209fc48); data_0209b274 is the
   time cursor ProcessKuppaScript advances each frame until the script ends. */
void func_0200ee8c(int arg0);
extern unsigned short data_0209b274;
extern signed char data_0209f224;
extern int data_0209214c[];    /* button remap pointer table (ROM DS
                                  pointers -- repointed at staging) */
/* CheckInput's three button remap maps, 16 u16 rows each, hosted as ROM bytes
   by port/tools/romdata.py. The staging block points data_0209214c's first
   three entries at them so the remap loop reads Nintendo's rows. */
extern unsigned char data_02075650[];   /* mode 0: level play  */
extern unsigned char data_02075670[];   /* mode 1 */
extern unsigned char data_02075690[];   /* mode 2: touch mode  */
/* the host key word the scene path's publisher reads (hal/scene_boot.cpp);
   the scene loop below refreshes it every frame from the host input */
void port_host_keys_set(unsigned short raw);
extern char data_0209f49c[];   /* held buttons (bit 1 = A/jump held) */
extern char data_0209f49e[];   /* pressed-this-frame (bit 1 = jump) */
extern int data_0209b468[4];   /* actor list head (stomp tracker) */
extern unsigned char data_020a0e40[];
extern short data_02092144[];
extern unsigned char data_ov002_0211049c[];  /* St_Wait state object */
/* SM64DS_DECEL_PROBE reads the ground-move constants out of the game's own
   helpers, so the log shows the numbers the physics actually used and not a
   second guess at them. 020c031c is the slip class (Player+0x658, promoted
   for one state); 020bf56c scales a brake rate by that class. */
int func_ov002_020c031c(void *c);
int func_ov002_020bf56c(void *c, int b);
int Player_ScaleByCharFactor(void *c, int a);
/* SM64DS_FORCE_STATE=walljump probe: the ST_WALL_JUMP State record and the
   per-character airborne-gravity PMF table St_WallJump_Main dispatches
   through. Both are BSS built by __sinit_ov002_021019d0, so they only read
   back after the sinit run below. */
extern unsigned char data_ov002_021103dc[];  /* _ZN6Player12ST_WALL_JUMPE */
extern unsigned char data_ov002_021106dc[];  /* _ZN6Player8ST_CLIMBE */
/* ST_LEDGE_HANG. Named off the sinit rather than a symbol map:
   __sinit_ov002_021019d0 fills 0210ffec's lo/hi/tail from 0x0210a41c,
   0x0210a06c and 0x02109eec, and ov002's reloc table takes those three to
   St_LedgeHang_Init, _Main and _Cleanup exactly. */
extern unsigned char data_ov002_0210ffec[];  /* _ZN6Player13ST_LEDGE_HANGE */
/* ST_DEAD_PIT, the fell-out-of-the-world state. __sinit_ov002_021019d0 fills
   0x02110124's lo/hi from 0x02109e04 and 0x0210a4dc, and ov002's reloc table
   takes those two PMFs to St_DeadPit_Init and St_DeadPit_Main exactly. Its Init
   case 1 is the ROM's own call to HitDeathPlane (out-of-bounds death), so a
   ChangeState into it with mStateStep==1 drives the real OOB path. */
extern unsigned char data_ov002_02110124[];  /* _ZN6Player11ST_DEAD_PITE */
extern int data_ov002_02110a48[5];           /* Tree's five cylinder lists */
extern int data_ov002_0211073c[];            /* 4 rows of {fn-or-vtoff, v} */
int _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
#ifdef PORT_ROM_CLEAN
/* ROM-CLEAN: fill the zeroed ROM tables from build/assets/romdata.bin before
   anything reads them. Loud FATAL if the file is missing/corrupt. Runs ahead of
   port_ov002_patch and every sinit -- see hal/romdata_loader.cpp. */
extern "C" void port_romdata_load(void);
#endif
void port_ov002_patch(void);
/* the pointers that leave their own mount, ovdata.py --cross. Order does not
   matter against the per-mount passes: it writes host addresses into host
   arrays, and a target is either inside its own mount or it is not, so the
   sites are disjoint. */
void port_cross_patch(void);
void __sinit_ov002_02100560(void); void __sinit_ov002_02100938(void);
void __sinit_ov002_02100adc(void); void __sinit_ov002_02100c50(void);
void __sinit_ov002_02100d44(void); void __sinit_ov002_02100e50(void);
void __sinit_ov002_02100ec4(void); void __sinit_ov002_02100f84(void);
void __sinit_ov002_02101064(void); void __sinit_ov002_02101478(void);
void __sinit_ov002_021014e4(void); void __sinit_ov002_02101588(void);
void __sinit_ov002_02101738(void); void __sinit_ov002_02101894(void);
void __sinit_ov002_02101900(void); void __sinit_ov002_02101968(void);
void __sinit_ov002_021019d0(void); void __sinit_ov002_02106e40(void);
void __sinit_ov002_02107118(void); void __sinit_ov002_021071f4(void);
void __sinit_ov002_02107298(void); void __sinit_ov002_02107304(void);
void __sinit_ov002_02107370(void); void __sinit_ov002_02107f88(void);
void __sinit_ov002_0210804c(void); void __sinit_ov002_02108094(void);
void *_ZN13SharedFilePtr9ConstructEj(void *, unsigned);
void _ZN12MeshColliderC1Ev(void *);
void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
void _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(void *, void *, void *);
int _ZN16MeshColliderBase6EnableEP5Actor(void *, void *);
void *_ZN5ModelC1Ev(void *);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
void hal_render_model(void *model, int scaleShift);
void _ZN13RaycastGroundC1Ev(void *);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *, const void *,
                                                        void *);
int _ZN13RaycastGround10DetectClsnEv(void *);
void _ZN4BgCh19StartDetectingWaterEv(void *);
void _ZN4BgCh21StopDetectingOrdinaryEv(void *);
int SurfaceInfo_TestFlag0x20(const int *);
int hal_ground_ray(void *mc, int x, int y, int z, int reach, int *out_y);
int hal_line_ray(void *mc, const int *a, const int *b, int *out);
void _ZN12WithMeshClsn13SetGroundFlagEv(void *);
int func_02035354(void *, void *);
int func_020393b4(void *);
/* the real Camera actor (gate 13) */
void hal_fill_camera_vtable(void);
int hal_camera_check_layout(void);
void *hal_camera_new(void);
int hal_camera_init_resources(void *cam);
int hal_camera_behavior(void *cam);
int hal_camera_render(void *cam);
void func_0203e0ac(void);
/* run mg16 lane MP3: the ROM's own comms dispatcher, linked through
   port/slice_mp3.txt. It owns the switch that used to be hosted at the call
   site below, and the only call site of the seam's close() face. */
void func_0203df40(void);
/* the ROM's own camera math, which the freecam rig builds its view with:
   the same eye construction func_02009e70 uses and the same two G3i entry
   points plus CopyToViewMat that Camera::Render ends in */
short Vec3_VertAngle(const void *v1, const void *v0);
int LenVec3(const void *v);
void Vec3_RotateYAndTranslate(int *out, int *in, short angle, int *src);
void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    int sinF, int cosF, int aspect, int n, int f, int scaleW, int draw,
    int *mtx);
void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(const void *at,
                                                  const void *up,
                                                  const void *eye, char draw,
                                                  int *mat);
void _Z13CopyToViewMatPK9Matrix4x3(const void *mat);
extern short data_02082214[];          /* the ROM sin/cos table */
extern unsigned char data_02086efc[];  /* up vector {0, 0x1000, 0} */
extern unsigned char data_020a1050[];  /* the heading func_0203dafc publishes */
extern int data_0209f43c[];          /* the Clipper (hal/camera_bridges) */
extern int data_020a4b78[];          /* the behaviour processing list */
int _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(void *clipper, void *pos,
                                                    int clip, unsigned char *h);
/* the two matrix calls VirtualDoor::Behavior itself uses to put the Player in
   an exit's local frame and to take a local point back out to the world */
void MulVec3Mat4x3(const void *in, const void *m, void *out);
void InvMat4x3(const void *in, void *out);
/* three of the Camera's nineteen states, for the exit probe: the one
   Camera::InitResources boots into, the one the ROM's own "follow him again"
   (func_0200d5c0) picks, and the one Camera::LookAtExit parks in. */
extern int data_0209b008[], data_0209b078[], data_0209b0f8[];
extern void *data_0209f318;          /* the Camera singleton */
extern signed char data_02092120;    /* currently shown area, -1 = none */
extern unsigned char data_0209f250;  /* local player index */
extern void *data_0209f394[];        /* per-player Actor* */
extern unsigned char data_0209f1f8;  /* view-object count */
extern signed char data_0209f2f8;    /* level/sublevel id (weather select) */
extern int data_0209f32c[];          /* water level */
extern int data_0209f20c[], data_0209f294[], data_0209f2c4[];
extern int data_0209b454[];
extern int data_0209ee90[];
extern int data_020a4b60[];
extern int g_walk_dbg[16];     /* collision-walk telemetry (port/unmatched) */
/* the overlay's "unhosted" counter: hal/player_bridges.cpp bumps this every
   time the state dispatcher falls off the end of its switch */
extern unsigned g_port_unhosted_hits;
/* the level's own entrance sub-table, kept by the boot for the debug menu's
   warp list (hal/level_boot.cpp) */
int port_entrance_count(void);
int port_entrance_record(int i, int *x, int *y, int *z, int *yaw);
/* the real level boot (hal/level_boot.cpp) */
void port_level_probe(void);
/* the SCENE boot (hal/scene_boot.cpp): SM64DS_SCENE=<id> takes the whole run,
   the way SM64DS_LEVEL picks which level this one is. -1 means "not a scene
   run", which is every run that does not set it. */
int port_scene_env_want(void);
int port_scene_run(void);
/* ...and the SAME run, split in three so a window can go round the middle of
   it. hal/scene_boot.cpp composes these into port_scene_run for a headless
   run; scene_window_run below drives them itself with a message pump, live
   input, the debug menu and a present between the ticks, so the scene's
   bring-up, capture and census exist once. port_scene_layout_propose is the
   stacked proposal (the ROM's own IsMinigameActorID), asked once by whoever
   gets there first because the mode latches on its first reader and a window
   has to be sized before begin() runs. See port/scene_window.txt. */
void port_scene_layout_propose(void);
int port_scene_begin(void *hwnd, int zoom);
void port_scene_tick(int frame, int tick_game);
int port_scene_finish(int frames_run);
int port_scene_frames_wanted(void);
const void *port_scene_framebuffer(void);
/* THE TITLE-TO-ADVENTURE BRIDGE (hal/title_entry.cpp), behind
   SM64DS_TITLE_ENTRY=1 and only on SM64DS_SCENE=1. Picking a save file drives
   the ROM's own StartFile, which stages a level and asks for scene 3 -- the
   Stage, which lives on the LEVEL path and which the scene spawner therefore
   declines honestly. These four let the scene run stop on that handoff and
   this file fall through to its own level boot instead of returning. With the
   flag unset every one of them answers 0 without reading anything else. */
int port_title_entry_armed(void);
int port_title_entry_should_stop(void);
int port_title_entry_commit(void);
int port_title_entry_taken(void);
int port_title_entry_run(void);
/* and the registry question that boot refuses on, asked ahead of time: the
   debug menu's minigame row uses it to decide which of the ROM's thirty
   minigame ids can be selected (hal/scene_boot.cpp). */
int port_scene_is_hosted(int id);
/* the level selector: SM64DS_LEVEL picks it, the debug menu's LEVEL row
   walks the same table. port_level_nth enumerates it. */
int port_level_id(void);
const char *port_level_name(void);
int port_level_count(void);
int port_level_nth(int i, int *id, const char **name);
/* the direct actor-spawn hook (hal/level_boot.cpp): one actor of a given
   class, through the level's own Actor::Spawn, at the player or anywhere */
void *port_debug_spawn(unsigned id, unsigned param);
void *port_debug_spawn_at(unsigned id, unsigned param,
                          int x, int y, int z, int yaw, int area);
void port_debug_spawn_env(void);
void *port_stage_a_boot(void *mc, int spawn_entrances);
void port_stage_a_probe(void *mc);
void *port_stage_create(void);   /* hal/stage_bridges.cpp: the real Stage actor */
void port_stage_tree_probe(void *child, const char *what);
void port_stage_render_model(void *self);  /* Stage::RenderModel, matched src */
void port_stage_render_model_transparent(void *self);
void port_stage_advance_anims(void *self); /* Stage::Render's first block: the
                                              level's BTA texture animations */
void port_stage_render_skybox(void *self); /* the +0x9bc Model, camera-glued */
void _ZN5Stage9LoadModelEv(char *self);   /* matched src, slice_gate24 */
void _ZN5Stage10LoadSkyboxEv(char *self); /* matched src, slice_gate26 */
unsigned _ZN5Stage11GetSkyboxIDEv(void);  /* the LVL_Overlay's skybox bits */
extern int data_0209f320;                 /* the Stage's ModelComponents */
int port_stage_path_guard(void *player);
/* the HOST-side description of the level (hal/level_boot.cpp's own statics,
   none of which a save-state restore rolls back). The census below prints
   them beside the world's own answers. */
int port_level_host_boot_target(void);
const void *port_level_host_entrances(int *count);
int port_level_host_file_rows(void);
void *port_level_host_file_row(int i, unsigned *handle, unsigned *refs,
                               int *persistent);
void port_level_host_paths(void **table, int *count);
void port_stage_a2_seat(void);
/* in-memory save state (hal/lk6_savestate.cpp): F8/F9 and the debug menu.
   save/load return 1 when they acted, has() drives the menu label. */
int lk6_savestate_save(void);
int lk6_savestate_load(void);
int lk6_savestate_has(void);
/* bit 0 the .dsstate section, bit 1 the arena, 0 = not captured at all */
int lk6_savestate_covers(const void *p);
/* how many captured words point at THIS process's heap or stack, and so are
   dead in the next one (the data_020a5bb8 class). Returns the count. */
int lk6_savestate_scan_host_pointers(const char *when, int list_max);
/* and the other direction: host .data/.bss words pointing INTO the arena a
   restore just replaced. Returns the count. */
int lk6_savestate_scan_world_pointers(const char *when, int list_max);
/* The packed-gap reproducer's one anchor (SM64DS_SS_WATCH_FLAG below).
   data_ov009_02112bc4 is the BTA_File CastleWater::InitResources hands
   TextureTransformer::Prepare and SetFile (see src/_ZN11CastleWater13Init
   ResourcesEv.cpp), and the generated mount patches its +20 word to DS
   0x02112238. What sits there is a 0x1c record whose leading halfword is
   0xffff in the ROM and 0x0000 once ov009's sinits have run, with a pointer at
   +4 into a block carrying the string "water_mat": the castle water's texture
   animation, not FLAG's anything. Flag::InitResources loads through
   data_ov009_02113eb8 and data_ov009_02113eb0, which are different symbols in
   a different block. THE ENV IS STILL SPELLED _FLAG for continuity with the
   review that found the case; the storage is not FLAG's and a debugger reading
   this should not go looking for it there.

   Following the +20 pointer rather than naming a host symbol is the point: it
   lands on whichever host array currently hosts the DS address, a synthetic
   gap block or a named mount symbol or whatever the generator decided next, so
   the probe watches the DS storage the GAME uses and survives the fix under
   test moving it. */
extern unsigned char data_ov009_02112bc4[];
/* The gap block itself, watched by name alongside the pointer above. The two
   answer different questions and the reproducer needs both: the pointer says
   whether the storage the GAME writes is captured, and this says whether the
   gap copy is the copy that moved. They can disagree -- a named mount symbol
   covering the same DS address would take the pointer and leave this one
   untouched -- and that disagreement is a result, not noise. */
extern unsigned char port_ov009_gap_0211222c[];
/* disk-backed save state (hal/lk7_persist.cpp): makes the slot survive a
   restart. write() mirrors a successful save to <exedir>\savestate.bin,
   read() loads it at startup; available() is 1 only when the arena is at its
   fixed base so disk states can relocate. */
int lk7_persist_write(void);
int lk7_persist_read(void);
int lk7_persist_available(void);
/* Why the last read turned a savestate.bin away, short enough for the on-screen
   toast; "" when there was nothing to refuse. See the note at the bottom of
   hal/lk7_persist.cpp. */
const char *lk7_persist_refusal(void);

/* Two seconds of on-screen text for the save-state actions. Every earlier
   report of "F8 did nothing" was undiagnosable because the only evidence was a
   line on stderr, which a player never sees: a press eaten by the focus gate,
   a menu row left unconfirmed and a save that genuinely happened all looked
   identical. The toast is drawn every frame after the menu (so neither the
   menu nor the overlay hides it) and says which of the three it was. */
static char ss_toast[64];
static int  ss_toast_left;
/* Set by any restore that happens where main's frame-loop locals are not in
   scope -- the debug menu's load row runs inside the menu handler, several
   frames' worth of call stack away from them. The loop re-seats on the next
   frame it sees this set. Every other restore path re-seats at the call site.
   See the ss_reseat comment in main(). */
static int ss_reseat_pending;
static void ss_note(const char *msg)
{
    snprintf(ss_toast, sizeof ss_toast, "%s", msg);
    ss_toast_left = 120;
}

/* ShadowModel::CleanAll, seated at the point Stage::Behavior calls it. The
   body is matched src/ and was being discarded by /OPT:REF because the port's
   hand transcription of Stage::Behavior had skipped the call.

   This is a method-shadow declaration, the same trick hal/heap_vtable.cpp uses:
   MSVC mangles a static member off the class NAME and the signature only, so
   `?CleanAll@ShadowModel@@SAXXZ` from here is the same symbol the real
   definition (src/_ZN11ShadowModel8CleanAllEv.cpp, which includes the full
   include/ShadowModel.h) exports. No layout is assumed and none is used.

   Its sibling ShadowModel::RenderAll is declared here too (run linkw wave 5,
   lane w5-d; wave 4's seat came back out because ntr drew the shadow volume
   itself, and that wall is down -- see the call site in the render order
   below). Same method-shadow trick, same reason it is exact: both are
   statics, so the decorated name is the class name and the signature and
   nothing else. */
struct ShadowModel {
    static void RenderAll();
    static void CleanAll();
};

/* ntr/io.cpp: the hardware content stores the save state captures. The
   reproducer below hashes them to PROVE a restore put the bytes back, because
   "the run kept stepping" is satisfiable with the wrong textures on screen --
   that is precisely how the 0.2.1 cross-area bug shipped. */
extern "C" unsigned port_hw_regions_size(void);
extern "C" void port_hw_regions_copy_out(void *dst);

/* ---- THE SAVE-STATE CONSISTENCY CENSUS -------------------------------------

   A restore rolls back the hosted arena, the .dsstate section and the hardware
   content stores, and nothing else. But the world is described in more places
   than those three: hal/level_boot.cpp stages the level's file-handle table,
   its entrance cache and its boot target as plain host statics (gate 31's
   comment there names that exact set as what a LEVEL CHANGE has to undo by
   hand), and THIS file's frame loop holds `player`, `c` and `cam`, derived
   from the world at boot and re-derived only on a level handoff -- where the
   comment reads "EVERY POINTER main() HOLDS INTO THE LEVEL IS STALE
   AFTERWARDS".

   A restore is the same transition as a level handoff and re-derives none of
   it. Two descriptions of one world with only one of them rolled back is the
   half-rollback disease every bug in this arc has been. So: print both
   descriptions side by side at the moment of the restore, and say which of
   them disagree. A run that lands in an inconsistent world then NAMES the
   inconsistency instead of faulting some frames later somewhere that mentions
   none of it.

   Called after every restore -- the boot-time disk read, F9, the debug menu's
   load row and the scripted SM64DS_SS_LOAD -- and at the scripted save, so a
   save's census and its load's census can be diffed. SM64DS_SS_CENSUS=0 turns
   it off, =2 adds the two whole-section sweeps at the bottom. */
static const char *ss_where(const void *p)
{
    if (!p) return "null";
    switch (lk6_savestate_covers(p)) {
    case 1:  return ".dsstate";
    case 2:  return "arena";
    case 3:  return "both?!";
    default: return "NOT CAPTURED";
    }
}
static void ss_census(const char *when, void *host_player, void *host_cam)
{
    /* 0 off, 1 (the default) the census, 2 the census plus the two section
       sweeps below. */
    static int on = -1;
    if (on < 0) {
        const char *e = getenv("SM64DS_SS_CENSUS");
        on = e ? atoi(e) : 1;
    }
    if (!on) return;

    void *world_player = data_0209f394[0];
    void *world_cam    = data_0209f318;
    void *ptbl = 0;
    int pcount = 0;
    port_level_host_paths(&ptbl, &pcount);
    int ecount = 0;
    const void *ents = port_level_host_entrances(&ecount);

    fprintf(stderr, "[ss-census] %s\n", when);
    fprintf(stderr, "[ss-census]   player: host %p  world %p  -- %s\n",
            host_player, world_player,
            host_player == world_player ? "agree" : "DIVERGED");
    /* +0x13c is the camera MODE object and +0x138 the STATE object, the two
       the boot prints at spawn. They are named here because they are what the
       player's 2026-08-26 crash actually died on: func_0200ca50 (the camera
       state machine's per-frame dispatch, called from Camera::Behavior) opens
       with `mode = self->+0x13c; ApproachLinear2(self+0x17a, mode->+0x24)`, so
       a null mode reads address 0x24 -- which is exactly the
       `access 00000000 at 00000024` in his dump, at func_0200ca50 +0x12, with
       Camera::Behavior +0x13d as the return address above it. */
    fprintf(stderr, "[ss-census]   camera: host %p  world %p  -- %s"
            "  (world mode %p state %p)\n", host_cam, world_cam,
            host_cam == world_cam ? "agree" : "DIVERGED",
            world_cam ? *(void **)((char *)world_cam + 0x13c) : 0,
            world_cam ? *(void **)((char *)world_cam + 0x138) : 0);
    if (host_cam && host_cam != world_cam)
        fprintf(stderr, "[ss-census]   camera: HOST cam mode %p state %p -- "
                "this is what Camera::Behavior would tick\n",
                *(void **)((char *)host_cam + 0x13c),
                *(void **)((char *)host_cam + 0x138));
    fprintf(stderr, "[ss-census]   level:  host boot target %d  world "
            "data_0209f2f8 %d  -- %s\n", port_level_host_boot_target(),
            (int)data_0209f2f8,
            port_level_host_boot_target() == (int)data_0209f2f8
                ? "agree" : "DIVERGED");
    fprintf(stderr, "[ss-census]   paths:  table %p (%s) count %d", ptbl,
            ss_where(ptbl), pcount);
    if (world_player)
        fprintf(stderr, "  world player binding %u",
                *(unsigned *)((char *)world_player + 0x670));
    if (host_player && host_player != world_player)
        fprintf(stderr, "  HOST player binding %u",
                *(unsigned *)((char *)host_player + 0x670));
    fprintf(stderr, "\n");
    fprintf(stderr, "[ss-census]   entrances: %p (%s) count %d\n", ents,
            ss_where(ents), ecount);
    {
        /* The rows themselves only at =2: a level's table runs to a couple of
           dozen and a player can press F9 all afternoon. At =1 the line that
           matters is how many rows there are and whether any of them points
           somewhere a restore does not reach -- a count that changed across a
           restore is the tell, and the rows are one env away when it does. */
        const int rows = port_level_host_file_rows();
        int outside = 0;
        for (int i = 0; i < rows; ++i) {
            unsigned h = 0, refs = 0;
            int persistent = 0;
            void *p = port_level_host_file_row(i, &h, &refs, &persistent);
            if (!lk6_savestate_covers(p)) ++outside;
            if (on == 2)
                fprintf(stderr, "[ss-census]     [%d] handle %u refs %u ptr %p "
                        "(%s)%s\n", i, h, refs, p, ss_where(p),
                        persistent ? " persistent" : "");
        }
        fprintf(stderr, "[ss-census]   host file table: %d row(s), %d pointing "
                "outside anything a restore rolls back\n", rows, outside);
    }
    /* THE TWO SWEEPS ARE OPT-IN (SM64DS_SS_CENSUS=2). They are whole-section
       walks that print a couple of dozen lines each, which is the right amount
       of detail for an investigation and the wrong amount for a playlog a
       player fills up with F9 presses. The census above is the part worth
       having in every log: seven lines that name what disagrees. */
    if (on == 2) {
        lk6_savestate_scan_host_pointers(when, 24);
        lk6_savestate_scan_world_pointers(when, 24);
    }
    fflush(stderr);
}

/* FNV-1a over the hardware stores, through the same copy-out the save uses.
   One lazily allocated buffer; ~9.5MB, three hashes an assert run. */
static unsigned long long ss_hw_hash(void)
{
    static char *buf;
    const unsigned n = port_hw_regions_size();
    if (!n) return 0;
    if (!buf) {
        buf = (char *)malloc(n);
        if (!buf) return 0;
    }
    port_hw_regions_copy_out(buf);
    unsigned long long h = 1469598103934665603ull;
    for (unsigned i = 0; i < n; ++i) {
        h ^= (unsigned char)buf[i];
        h *= 1099511628211ull;
    }
    return h;
}
/* SM64DS_SS_WATCH_FLAG's reader. Follows ov009's own pointer to the DS
   halfword at 0x02112238 -- the leading id field of the castle water's texture
   animation record, ffff in the ROM -- and reports the value together with
   where the host storage behind it lives. 0xdead is returned for a null
   pointer, which cannot be a real id here and means the mount's patch pass did
   not run.

   THE INDIRECTION IS THE MEASUREMENT. Reading a host symbol by name would
   answer for that symbol; reading through the game's own pointer answers for
   the bytes the game writes, which is the only thing a save state has to roll
   back. It also survives the fix under test moving the DS address from one
   host array to another. */
static unsigned ss_flag_word(int *covers)
{
    const unsigned char *p =
        *(const unsigned char *const *)(data_ov009_02112bc4 + 20);
    if (covers) *covers = p ? lk6_savestate_covers(p) : 0;
    return p ? *(const unsigned short *)p : 0xdeadu;
}
/* The record around the watched halfword, so a reader can tell a targeted
   game write apart from something having zeroed the whole block. The ROM's
   bytes here are ff ff 00 00 then a relocated pointer, twice over: two records
   of the same 0x1c shape, the second at DS 0x02112254, each with its own
   inbound pointers and its own fields behind it. */
static void ss_flag_dump(const char *when)
{
    const unsigned char *p =
        *(const unsigned char *const *)(data_ov009_02112bc4 + 20);
    if (!p) {
        fprintf(stderr, "[ss-flag] %s: pointer is NULL (the mount's patch "
                        "pass did not run)\n", when);
        return;
    }
    fprintf(stderr, "[ss-flag] %s: record at %p =", when, (const void *)p);
    for (int i = 0; i < 32; ++i)
        fprintf(stderr, " %02x", p[i]);
    fprintf(stderr, "\n");
}
/* the actor registry and the ROM's own processing lists (hal/actor_registry) */
void port_actor_tick(void);          /* phases 4/2/3: cleanup, init, behaviour */
/* phase 6, the frame clock (hal/fader_wipes.cpp): data_020a0db0, the one counter
   every blink in the game hangs off. func_020197b8 steps it once per frame and
   this port does not run func_020197b8, so it sat at zero and THIRTEEN linked
   readers were dead -- three of them `& 1` sites that test non-zero and so had
   never executed at all. Read that file's banner before moving this call. */
void port_frame_clock_tick(void);
/* func_020197b8 PHASE 2's head (hal/scene_boot.cpp): the current scene's
   graphics block, word 0, which is scene slot 23's only dispatch site in the
   whole ROM. Answers 1 for a block this port has not seated. */
int port_graph_block_word0(void);
/* gate 31: the level handoff (hal/level_change.cpp). port_level_change_poll
   sits where Scene::SpawnIfNecessary sits in func_020197b8 -- after input,
   before the actor phases -- and returns 1 on the frame a new level came up,
   which is the frame every pointer main() holds into the old one goes stale. */
int port_level_change_poll(void);
int port_level_change_pending(void);
int port_level_is_mounted(int level);
int port_level_overlay_id(int level);
unsigned port_level_ds_overlay(int level);
unsigned port_level_heap_free_bytes(void);
int port_actor_live_count(void);
int port_title_rows(void);
int port_title_row(int i, int *level, int *entrance);
int port_title_select(int i);
void port_level_mounts_install(void);   /* register the mount table (idempotent) */
void ExitLevel(void);
void LoadLevelNoReturn(int level, unsigned entrance, unsigned star,
                       unsigned reason);
extern signed char data_0209f2f8;    /* the level currently up */
/* gate 31 faders (hal/fader_wipes.cpp): port_fader_advance steps whatever fade
   is in motion one frame and writes the 2D master-blend register the ROM's own
   FaderColor::AdvanceFade writes; port_fader_blend_state reads it back so the
   compositor can fade the framebuffer. */
void port_fader_advance(void);
int port_fader_blend_state(int *evy, int *toWhite);
void port_fader_start_color(int frames, int toEnd, unsigned short color);
/* dialogue pipeline (hal/message_pump.cpp, hal/message_compositor.cpp,
   hal/message_probe.cpp): pump ticks the box state machine (Stage's own
   UpdateMessage dialogue arm); the compositor rasters engine A's 2D box over
   the 3D frame; the probe opens a box headlessly for a visual check. */
void port_message_pump(void);
/* the OTHER statement of Stage::Behavior the port hosts (hal/star_flow.cpp):
   the VS 3-2-1, which ends by starting the arena's own music */
void port_vs_countdown_tick(void);
void port_message_composite_engine_a(void *fb);
int port_probe_message_id(void);
int port_probe_message_fire(void *player, int id);
/* frame-scripted headless pad press (hal/input_probe.cpp): apply ORs the
   scripted DS pad bits into the raw pad mirror before Stage::CheckInput; bits
   returns the same word so the harness can fold it into its own button word.
   TEMPORARY -- delete with the probe. */
void port_input_probe_apply(int frame);
unsigned short port_input_probe_bits(int frame);
void port_input_probe_trace_msg(int frame);
void port_input_probe_trace_cannon(int frame);
void port_input_probe_buddy_trigger(int frame);
void port_input_probe_sign_trigger(int frame);
void port_probe_alcheck(void);
void port_probe_sign_yaw(void);
void port_probe_chomp(int frame);
void port_probe_rabbit_key(int frame);   /* TEMPORARY: SM64DS_TRACE_RABBITKEY */
void port_probe_rabbit_trigger(int frame); /* TEMPORARY: SM64DS_RABBIT_TRIGGER */
void port_probe_key_spawn(int frame);      /* TEMPORARY: SM64DS_KEY_SPAWN_AT */
void port_probe_vs_overlap(int frame);     /* test fixture: SM64DS_VS_OVERLAP_AT */
/* the scene-fade request the title-select hands off with. Recorded by the port
   in hal/level_change.cpp and acted on by this frame loop. */
int port_scene_fade_pending(int *sceneId);
void port_scene_fade_clear(void);
void port_actor_render(void);        /* phase 5: the render bucket */
/* gate 29: the pair Stage::Render and Stage::GraphCallback1 make on the ROM,
   and they are called from the two places those two run -- the simulation in
   the actor bucket, the submission after the level pass */
void port_particle_frame(void);      /* Stage::Render: SysTracker::Update */
void port_particle_render(void);     /* GraphCallback1: Particle::RenderAll */
void port_particle_counts(int *systems, int *particles);
void port_actor_scene_pass(void);    /* phase 1: scene-tree housekeeping */
void port_actor_census(void);
void port_actor_lists_probe(void);
/* the [lvl-perf] level-entry spans (hal/level_boot.cpp). The harness owns the
   census and boot-dump brackets and the emit on the direct boot; the warp
   path's brackets and emit live in hal/level_change.cpp. */
double port_lvlperf_now(void);
void port_lvlperf_note(int span, double ms);
void port_lvlperf_emit(void);
/* gate 35, the course loop (hal/star_flow.cpp): the boot seat plus one probe
   per thing the gate has to be able to show moving. */
void port_course_seat(void);
int  port_course_health(void);
int  port_course_coins(void);
int  port_course_hurt(void *player, int kind);
int  port_course_can_hurt(void *player);
int  port_course_next_sublevel(void);
int  port_course_hud_health(void);
int  port_course_in_dead_state(void *player);
void port_course_arm_watch(void);
int  port_course_handoff_fired(void);
int  port_course_drown_tick(void *player, int amount);
void port_course_kill(void);
void port_course_respawn(void *player);
int  port_star_collect(int starId);
void port_give_player_coins(void *actor, void *player, int count, int kind);
int  port_course_sound_probe(const char *when);
/* gate 31 loose end: the looping-sound reap. port_course_loop_start drives the
   game's Sound::PlayLong at the player to put a live handle in data_0209b53c;
   port_course_loop_live counts the live handles the reaper is meant to drain. */
unsigned int port_course_loop_start(unsigned int prev, unsigned int soundId);
int  port_course_loop_live(void);
/* the file-seam load meter (hal/fs.cpp), sampled by SM64DS_JUMP_PROBE */
extern unsigned long port_fs_loads, port_fs_load_miss, port_fs_bytes;
extern double port_fs_ms;
/* the body-model selector Player::SetAnim itself uses to pick which of the
   ten models at Player+0xdc the animation is installed on */
unsigned _ZNK6Player14GetBodyModelIDEjb(char *self, unsigned a, char b);
/* gate 32: what SM64DS_SPAWN_ACTOR spawned, read back off the behaviour list
   once the run is over (hal/actor_spawn_probe_bob.cpp). No env, no output. */
void port_bob_spawn_report(void);
/* the bottom screen (hal/sub_screen.cpp): the OAM lifecycle, the engine-B
   scan-out and the corner panel it lands in. TAB toggles the panel. */
void hal_sub_screen_init(void *hwnd, int zoom);
/* THE PRESENT RECTANGLE, published so the touch bridge can undo it
   (hal/sub_screen.cpp holds it because poll_touch lives there and links into
   binaries this file is not part of). Set once per present with the client
   pixels the framebuffer was scaled into; the mapper turns a client point
   back into a framebuffer point and returns 0 for a point in the letterbox
   bars, which is outside the picture and therefore not a touch. */
void hal_present_set_rect(int x, int y, int w, int h, int src_w, int src_h);
int hal_present_client_to_fb(int cx, int cy, int *fx, int *fy);
/* THE OTHER BAND OF THE SAME RECTANGLE. client_to_fb means the TOP screen in
   both layouts, so in the stacked layout it answers "outside" for every point
   on the bottom half -- correctly, and unhelpfully if the caller then reports
   that as a letterbox bar. This is the lower band, in DS pixels. */
int hal_present_client_to_sub(int cx, int cy, int *dsx, int *dsy);
/* THE STACKED LAYOUT (hal/sub_screen.cpp). Both DS screens full size, top above
   bottom, for minigames -- a touchscreen game cannot be played against a
   128x96 corner preview. hal_sub_screen_stacked answers whether this run is in
   it, and hal_sub_screen_stacked_image builds the ntr::STACK_W x STACK_H
   image out of the FINISHED framebuffer plus the bottom screen. It is called
   last, after this file's fade composite and debug overlay, so the framebuffer
   it reads is the same framebuffer every ppu_write_bmp site here writes, and
   hal/sub_screen.cpp owns the buffer so an inset build carries none of it.

   THE IMAGE IS NOT A FIXED SIZE ANY MORE. A minigame that simulates the DS's
   hinge composes a band between the two screens, so the image is
   512 x (768 + 2G) at this tier and G is not known until the scene's
   InitResources has run. hal_sub_screen_stacked_size is the live size and
   hal_sub_screen_stacked_generation steps whenever it changes; see
   port/hal/screen_gap.h. Everything in this file that used to spell
   ntr::STACK_H reads one of those instead. */
int hal_sub_screen_stacked(void);
unsigned int *hal_sub_screen_stacked_image(const unsigned int *top);
void hal_sub_screen_stacked_size(int *w, int *h);
/* THE UPPER PHYSICAL SCREEN'S first row inside that image. The host overlays
   below paint there rather than into the framebuffer, because ppu_compose_
   stacked picks which ENGINE feeds which screen off POWCNT1 bit 15 and an
   overlay painted into engine A's buffer moves with the engine. */
int hal_sub_screen_stacked_top_y(void);
unsigned hal_sub_screen_stacked_generation(void);
/* the focus gate (hal/sub_screen.cpp): 1 when this window is the foreground
   one, so an interactive key read can be trusted to be meant for this program */
int hal_window_focused(void);
void hal_sub_screen_frame_begin(void);
void hal_sub_screen_present(unsigned int *dst, int w, int h);
void hal_sub_screen_probe(void);
/* the camera buttons drawn on the bottom screen, hit-tested against the touch
   record the panel fills (hal/sub_screen.cpp wraps Stage::CheckCameraInput
   with the split-symbol bridge the host Ctrl block needs) */
void hal_sub_camera_input(void);
}

/* The frame's cylinder-overlap pass. The host copy in port/unmatched/ rather
   than the matched TU: MSVC's one-slot destructor shifts GetPos onto D0 and
   the first pass frees a cylinder. See that file's header. */
extern "C" void port_cylinder_clsn_process(void);
extern "C" void *_ZTV18MovingCylinderClsn[];
extern "C" void *data_0209ee74;   /* the particle SysTracker (hal/auto_bss) */
extern "C" void *data_0209f5bc;   /* the installed fader (hal/fader_wipes) */
extern "C" void *data_0209f324;   /* WIPES, the seven-wipe array */
extern "C" signed char data_02092110;    /* the staged next level */
extern "C" unsigned char data_0209f268;  /* the staged next entrance */
extern "C" unsigned char data_0209f26c;  /* why we are leaving (2 = death) */
extern "C" unsigned char data_0209f2fc[]; /* the LATCHED entry reason, the copy
                                             Stage::InitResources:201 makes and
                                             the boot now seats */
extern "C" signed char data_0209f2f4[];  /* remaining lives */
extern "C" unsigned char NumStars(void); /* the save's star total */

/* ---- THE EXIT PROBE: the painting warp, reproducible without a keyboard ----
   Two players walked into the Snowman's Land painting on castle_2f and the
   build does not host Snowman's Land, so the level change was declined and
   they were left with no control inside the wall. Reproducing that by hand
   means holding W at the right painting for the right number of frames; this
   makes it one env var and one frame number.

   SM64DS_EXIT_PROBE=1 dumps the level's EXIT actors (349, class VirtualDoor)
   once the level is up: index, world position, which level the record sends
   you to, and whether the record is a WALL painting (rotX 0, the exit that
   seeds the pull-in counter) or a tilted FLOOR hole (rotX != 0, the exit that
   fires the load and the wipe immediately). The index is what ENTER takes.

   SM64DS_EXIT_ENTER=<index>[,<frame>] walks the Player into that exit. The
   ROM's trigger is a SIGN CHANGE of the Player's Z in the exit's own local
   frame while he is inside the box, so the probe puts him in the box on the
   near side for one frame and just past the plane on the next -- the two
   frames a walk produces, through the same VirtualDoor::Behavior test. It
   writes nothing but the Player's position, and never touches the exit. */
static char *port_exit_nth(int idx)
{
    int n = 0;
    for (int *node = (int *)(size_t)data_020a4b78[0]; node;
         node = (int *)(size_t)node[1]) {
        char *o = (char *)(size_t)node[2];
        if (!o || *(unsigned short *)(o + 0xc) != 349)
            continue;
        if (n++ == idx)
            return o;
    }
    return 0;
}

static void port_exit_dump(void)
{
    int n = 0;
    for (int *node = (int *)(size_t)data_020a4b78[0]; node;
         node = (int *)(size_t)node[1]) {
        char *o = (char *)(size_t)node[2];
        if (!o || *(unsigned short *)(o + 0xc) != 349)
            continue;
        const unsigned p1 = *(unsigned *)(o + 8);
        const int rotX = *(short *)(o + 0x8c);
        fprintf(stderr, "[exit] %2d at (%d,%d,%d) -> level %d entrance %d  "
                "rotX %d rotY %d  box x+-%d y0..%d  %s\n",
                n, *(int *)(o + 0x5c) >> 12, *(int *)(o + 0x60) >> 12,
                *(int *)(o + 0x64) >> 12,
                (int)(signed char)(p1 >> 24), (int)((p1 >> 16) & 0xff),
                rotX, *(short *)(o + 0x8e),
                *(int *)(o + 0x80) >> 12, *(int *)(o + 0x84) >> 12,
                rotX ? "FLOOR HOLE (tilted: loads at once)"
                     : "WALL PAINTING (pull-in)");
        ++n;
    }
    fprintf(stderr, "[exit] %d exits on this level\n", n);
}

/* Put `player` at local (0, y, z) in exit `ex`'s frame, or -- with `beside` --
   one box-width to the side of it, where the exit's own x test turns him away
   but its Behavior still records where he is. */
static void port_exit_place(char *ex, char *player, int z, int beside)
{
    int local[3], inv[12];
    MulVec3Mat4x3(player + 0x5c, ex + 0xd4, local);
    local[0] = beside ? *(int *)(ex + 0x80) * 2 : 0;
    /* local y 0 is the bottom of the trigger box, which for a painting is
       where it meets the floor -- a walking player's own height. Snapping to
       the middle of the box instead drops him in from mid-air and every
       reading afterwards has a fall in it that a walk does not. */
    if (local[1] < 0 || local[1] > *(int *)(ex + 0x84))
        local[1] = 0;
    local[2] = z;
    InvMat4x3(ex + 0xd4, inv);
    MulVec3Mat4x3(local, inv, player + 0x5c);
}

#ifdef NTR_HIRES
static const int ZOOM = 1;
#elif defined(NTR_HIRES2)
static const int ZOOM = 2;
#else
static const int ZOOM = 3;
#endif
/* The level MeshCollider every ray in this file is cast against: the STAGE'S
   own, at Stage+0x91c, on the Stage-backed boot. RELOADRV's reverse scan named
   it as a host mirror of a world pointer; the restore re-seat re-derives it
   from the Stage rather than capturing it (see the note at the re-seat). */
static void *g_mc;
extern "C" void *port_stage_object(void);   /* hal/stage_bridges.cpp */
/* the two halves of the rollback-coupled guards' A/B; see the registration */
extern "C" void port_ss_rollguard_hook(void (*)(void), void (*)(void));
extern "C" void port_rollguard_stash(void);
extern "C" void port_rollguard_unstash(void);

/* ---- THE FRAME PACER'S CLOCK ------------------------------------------
   frame_pace below sleeps out the remainder of a frame budget that is 16.65ms
   or 33.3ms depending on what the running scene put in the ROM's own divider
   (see frame_pace's banner). Sleep's
   resolution is the SYSTEM TIMER TICK, which defaults to 15.6ms: a request for
   4ms returns after 15.6, so a frame with 4ms of slack overshot the budget by
   a whole tick and the next one came early making it up. That is the judder in
   the pacing -- not the raster, the sleep.

   timeBeginPeriod(1) pulls the tick to 1ms for this process and the sleep
   lands within a millisecond of what was asked. Paired with timeEndPeriod
   through atexit, so the process cannot leave the system clock raised on the
   way out -- including the selftest's early return and the WM_QUIT one.

   AND THAT IS NOT ENOUGH ON ITS OWN, which cost an hour to find. On Windows 11
   timeBeginPeriod(1) returns TIMERR_NOERROR and is then quietly ignored for a
   process the power manager has decided is a background one: measured here,
   the call succeeded and Sleep(9) still took 22.7ms, with the whole frame
   period pinned to 46.8ms -- three ticks of the 15.6ms default, exactly what
   an unraised clock produces. The opt-out is a power-throttling request:
   ProcessPowerThrottling with IGNORE_TIMER_RESOLUTION in the control mask and
   CLEAR in the state mask, which reads as "I am managing this, and the answer
   is do not throttle me". SetProcessInformation is resolved dynamically like
   everything else, so this still runs on anything older that lacks it. */
enum {
    PORT_PROCESS_POWER_THROTTLING = 4,   /* ProcessPowerThrottling */
    PORT_POWER_THROTTLING_VERSION = 1,
    PORT_IGNORE_TIMER_RESOLUTION = 0x4
};
struct PortPowerThrottlingState {
    unsigned Version, ControlMask, StateMask;
};
static int g_pacer_period;

static void pacer_end(void)
{
    if (g_pacer_period && W.timeEndPeriod_) {
        W.timeEndPeriod_(g_pacer_period);
        g_pacer_period = 0;
    }
}

static void pacer_begin(void)
{
    if (W.SetProcessInformation_) {
        PortPowerThrottlingState pt;
        pt.Version = PORT_POWER_THROTTLING_VERSION;
        pt.ControlMask = PORT_IGNORE_TIMER_RESOLUTION;
        pt.StateMask = 0;                 /* 0 = honour the raised timer */
        W.SetProcessInformation_(GetCurrentProcess(),
                                 PORT_PROCESS_POWER_THROTTLING, &pt,
                                 sizeof pt);
    }
    if (!W.timeBeginPeriod_ || g_pacer_period) return;
    if (W.timeBeginPeriod_(1) != 0) return;   /* != TIMERR_NOERROR */
    g_pacer_period = 1;
    atexit(pacer_end);
}

/* ---- THE FRAME BUDGET, OFF THE ROM'S OWN DIVIDER ----------------------
   HOW FAST A FRAME LOOP MAY RUN IS THE GAME'S DECISION, NOT THE HOST'S, and
   the game writes it down. IRQ::VBlankHandler (src/_ZN3IRQ13VBlankHandlerEv.c)
   counts vblanks into data_0209d514 and only wakes the main thread once that
   count reaches data_0208ee44, so data_0208ee44 is literally vblanks-per-tick:

       1 -> 60 fps      2 -> 30 fps      3 -> 20 fps

   and every scene sets it for itself during its own InitResources:

       src/_ZN5Stage13InitResourcesEv.cpp:362        = 2   the 3D levels
       src/_ZN16dScMgSmartball_c13InitResourcesEv.c  = 1   a minigame
       src/func_ov006_020de704.c and its dozen peers = 1   the other minigames
       src/func_ov002_020f7780.c:23                  = 3
       src/func_ov075_0211a410.cpp:140               = 2

   BOTH HOST LOOPS USED TO HARDCODE 33.3ms, which is the divider-2 answer. The
   3D level path was right by accident and every minigame ran at EXACTLY HALF
   SPEED, which is the "way slower than normal gameplay" report. Nothing was
   wrong with the sleep, the timer resolution or the present: the loop was
   pacing to a number the scene had already overruled.

   THE GAME LOGIC IS UNTOUCHED BY THIS. Both loops run exactly ONE game tick per
   host frame either way, and the ROM's own per-frame increments already scale
   themselves by this same word (func_02019ac4's `delta`, func_02020768's `acc`,
   HUD::UpdateHealthMeter, Message::Update). Ticking at 60 with the divider at 1
   is the DS, and it is the only thing that is. */
extern "C" int data_0208ee44;

/* One DS vblank in milliseconds. This is the 33.3 the level loop shipped with,
   divided by the 2 that path's divider holds, so the 3D level path's budget
   comes out at the identical number it always used and this change cannot move
   a rate that was already correct. */
static const double PORT_VBLANK_MS = 33.3 / 2.0;

static int port_frame_divider(void)
{
    /* SM64DS_PACE_DIVIDER=<n> overrides the game's word, for A/B measurement
       only -- "what does this scene sound like if I pace it the way the port
       used to" is a question worth being able to ask without a second build.
       Unset (the normal case) and the ROM decides. */
    static int forced = -1;
    if (forced < 0) {
        const char *e = getenv("SM64DS_PACE_DIVIDER");
        forced = e ? atoi(e) : 0;
        if (forced < 0 || forced > 4) forced = 0;
    }
    if (forced) return forced;
    const int d = data_0208ee44;
    /* The ROM writes 1, 2 or 3 and nothing else. Anything outside that is a
       global nothing has written yet (or one something has stomped), and the
       safe reading of a garbage divider is the 30fps the port already shipped
       -- not an uncapped loop and not one frame a second. */
    return (d >= 1 && d <= 4) ? d : 2;
}

/* THE PACER ITSELF, one copy for both frame loops.

   A DEADLINE, NOT AN ELAPSED-TIME SUBTRACTION. The old block measured the
   frame's work and slept `budget - work`, which throws away whatever the Sleep
   overshot and whatever the (DWORD) cast truncated -- up to a millisecond a
   frame, which is 3% of a 33.3ms budget and 6% of a 16.65ms one. Carrying an
   absolute deadline instead makes those errors cancel: a sleep that ran long
   shortens the next one and the average period is the budget exactly.

   A HITCH IS NOT A DEBT. If a frame blows through its deadline -- a level load,
   a window drag, the OS taking the core away -- the deadline is pulled up to
   now rather than left behind, so the loop returns to pace on the next frame
   instead of sprinting through several to "catch up". Catching up would mean
   running game ticks faster than the DS runs them, which is the one thing a
   pacer here must never do. */
static void frame_pace(void)
{
    static LARGE_INTEGER qpf, next;
    static LARGE_INTEGER rep_t0;
    static int rep_n;
    static int trace = -1;
    LARGE_INTEGER now;

    if (trace < 0) {
        const char *e = getenv("SM64DS_TRACE_PACE");
        trace = e ? atoi(e) : 0;
    }
    if (!qpf.QuadPart) QueryPerformanceFrequency(&qpf);
    QueryPerformanceCounter(&now);

    const int div = port_frame_divider();
    const double budget = PORT_VBLANK_MS * div;
    const LONGLONG step =
        (LONGLONG)(budget * (double)qpf.QuadPart / 1000.0 + 0.5);

    double slept = 0.0;
    if (!next.QuadPart) {
        next.QuadPart = now.QuadPart;   /* the first frame sets the deadline */
    } else {
        next.QuadPart += step;
        if (next.QuadPart <= now.QuadPart) {
            next.QuadPart = now.QuadPart;   /* overran: reset, do not sprint */
        } else {
            const double ms =
                (next.QuadPart - now.QuadPart) * 1000.0 / (double)qpf.QuadPart;
            if (ms >= 1.0) {
                LARGE_INTEGER a2;
                Sleep((DWORD)ms);
                QueryPerformanceCounter(&a2);
                slept = (a2.QuadPart - now.QuadPart) * 1000.0 /
                        (double)qpf.QuadPart;
            }
        }
    }

    if (!trace) return;
    /* SM64DS_TRACE_PACE=2 is the per-frame line, which costs an unbuffered
       write every frame and therefore distorts the very thing it measures.
       SM64DS_TRACE_PACE=1 is the cheap one: a rate over the last 120 frames,
       which is the number to quote. */
    if (trace >= 2)
        fprintf(stderr, "[pace] div=%d budget=%.2f slept=%.2f\n",
                div, budget, slept);
    if (!rep_t0.QuadPart) { rep_t0 = now; rep_n = 0; return; }
    if (++rep_n >= 120) {
        const double s =
            (now.QuadPart - rep_t0.QuadPart) / (double)qpf.QuadPart;
        fprintf(stderr, "[fps] %d frames in %.3fs = %.2f fps "
                "(divider %d, budget %.2fms)\n",
                rep_n, s, s > 0.0 ? rep_n / s : 0.0, div, budget);
        rep_t0 = now;
        rep_n = 0;
    }
}

/* ---- THE DEBUG OVERLAY (port mod) -------------------------------------
   F3. Text drawn INTO THE FRAMEBUFFER, after gx_render and before the blit,
   with the 8x8 font in overlay_font.h. Three consequences worth stating,
   because they are why it is done this way rather than with GDI TextOut:

   - it works at every tier. fb is 512x384 in this build and 1024x768 in
     walk_window_hires, and the same code covers both because it never names a
     resolution -- ntr::SCREEN_W/H and a scale derived from them.
   - it lands in the BMP the selftest dumps, so a CI shot can show its own
     numbers.
   - it costs the raster nothing: it is a byte loop over already-rasterised
     pixels, outside gx entirely.

   Default OFF, so a plain selftest dump is unchanged. SM64DS_OVERLAY=1 boots
   with it on.

   ---- AND WHICH SURFACE IT PAINTS ON, which is not always the framebuffer ----

   THE OVERLAYS ARE HOST UI AND THEY BELONG TO A SCREEN, NOT TO AN ENGINE. The
   framebuffer is engine A's, and since ntr::ppu_compose_stacked started reading
   POWCNT1 bit 15 the stacked layout puts engine A in whichever half the DS's
   display-swap bit names -- so "paint into fb" stopped meaning "paint on the
   top screen". Three of the game's scenes make that visible:

     372, 373, 384, 385   dScMgD3DBase_c's slot 24 TOGGLES bit 15 EVERY FRAME,
                          so an overlay in fb ping-pongs between the halves at
                          60 Hz. Tango's report: "when f5 debug is opened it
                          rapidly flashes between both top and bottom screen".
     377                  Snowball Slalom CLEARS the bit once and leaves it
                          clear, so an overlay in fb sits on the bottom screen
                          for the whole minigame. Tango's report: "f5 debug
                          being on bottom screen".

   So the painters below take an OvlSurface -- a pointer and a stride -- instead
   of the framebuffer, and the frame loops hand them the UPPER PHYSICAL SCREEN's
   rows of the composed stacked image (hal_sub_screen_stacked_top_y) when there
   is one, and the framebuffer when there is not. The arithmetic inside them is
   unchanged: an overlay covers ONE DS screen at the host tier in either layout,
   so ntr::SCREEN_W/H are still the clip bounds and every coordinate in every
   caller still means what it meant.

   THE INSET LAYOUT IS UNTOUCHED, and deliberately. There the window IS engine
   A's framebuffer at full size with the other screen as a corner panel inside
   it; there is no "upper half" to prefer, and the overlay goes where it always
   went. Nothing about a level changes. */
static const int OVL_SCALE = ntr::SCREEN_W >= 1024 ? 2 : 1;
static const int OVL_LINE = (OVL_GLYPH_H + 2) * OVL_SCALE;

/* WHERE AN OVERLAY PAINTS: one DS screen's worth of 0xAARRGGBB pixels, at
   `stride` words per row. `px` is that screen's top-left. Deliberately not a
   width and a height as well -- an overlay is always exactly one DS screen at
   this binary's tier, in both layouts, and carrying the size would invite a
   caller to pass a different one and quietly re-scale the font. */
struct OvlSurface {
    uint32_t *px;
    int stride;
};

static OvlSurface ovl_surface(ntr::Framebuffer &fb)
{
    OvlSurface s;
    s.px = &fb.px[0][0];
    s.stride = ntr::SCREEN_W;
    return s;
}

/* The composed stacked image's UPPER half. Null image -> the framebuffer, which
   is the same fallback present() takes: with no stacked image there is nothing
   else on screen to be wrong about. */
static OvlSurface ovl_surface_stacked(uint32_t *img, ntr::Framebuffer &fb)
{
    if (!img) return ovl_surface(fb);
    int iw = 0, ih = 0;
    hal_sub_screen_stacked_size(&iw, &ih);
    OvlSurface s;
    s.px = img + (size_t)hal_sub_screen_stacked_top_y() * (size_t)iw;
    s.stride = iw;
    return s;
}

static void ovl_shade(const OvlSurface &fb, int x0, int y0, int w, int h)
{
    /* half-strength darken of what is already there, so the text reads over
       sky and over stone without hiding the frame behind it */
    for (int y = y0; y < y0 + h; ++y) {
        if (y < 0 || y >= ntr::SCREEN_H) continue;
        uint32_t *row = fb.px + (size_t)y * (size_t)fb.stride;
        for (int x = x0; x < x0 + w; ++x) {
            if (x < 0 || x >= ntr::SCREEN_W) continue;
            const uint32_t p = row[x];
            row[x] = 0xFF000000u | ((p >> 1) & 0x007F7F7Fu);
        }
    }
}

static int ovl_text(const OvlSurface &fb, int x0, int y0, const char *s,
                    uint32_t rgb)
{
    int x = x0;
    for (; *s; ++s) {
        const unsigned char ch = (unsigned char)*s;
        if (ch < 0x20 || ch > 0x7e) { x += OVL_ADVANCE * OVL_SCALE; continue; }
        const unsigned char *g = OVL_FONT[ch - 0x20];
        for (int r = 0; r < OVL_GLYPH_H; ++r) {
            const unsigned char bits = g[r];
            if (!bits) continue;
            for (int c = 0; c < OVL_GLYPH_W; ++c) {
                if (!(bits & (0x80 >> c))) continue;
                const int px = x + c * OVL_SCALE, py = y0 + r * OVL_SCALE;
                for (int sy = 0; sy < OVL_SCALE; ++sy)
                    for (int sx = 0; sx < OVL_SCALE; ++sx) {
                        const int fx = px + sx, fy = py + sy;
                        if (fx < 0 || fx >= ntr::SCREEN_W || fy < 0 ||
                            fy >= ntr::SCREEN_H)
                            continue;
                        fb.px[(size_t)fy * (size_t)fb.stride + fx] = rgb;
                    }
            }
        }
        x += OVL_ADVANCE * OVL_SCALE;
    }
    return x - x0;
}

/* the per-phase clock. One QueryPerformanceCounter pair per phase and a
   1-second exponential average, so the numbers are readable instead of
   flickering with whatever the OS did to that one frame. */
struct PhaseClock {
    LARGE_INTEGER qpf, mark;
    double ms[8];        /* smoothed, indexed by the enum below */
    double raw[8];
};
enum {
    PH_INPUT = 0,   /* keys, pad, Stage::CheckInput, the actor tick */
    PH_CAMERA,      /* Camera::Behavior + the heading echo */
    PH_SUBMIT,      /* actor bucket + level model + player: geometry into gx */
    PH_RASTER,      /* ntr::gx_render */
    PH_BLIT,        /* StretchDIBits */
    PH_FRAME,       /* the whole loop body, pacing excluded */
    PH_COUNT
};
static PhaseClock g_clk;

static double ovl_now_ms(void)
{
    LARGE_INTEGER n;
    if (!g_clk.qpf.QuadPart) QueryPerformanceFrequency(&g_clk.qpf);
    QueryPerformanceCounter(&n);
    return n.QuadPart * 1000.0 / g_clk.qpf.QuadPart;
}
static void ph_begin(double *slot) { *slot = ovl_now_ms(); }
static void ph_end(int idx, double start)
{
    const double d = ovl_now_ms() - start;
    g_clk.raw[idx] = d;
    g_clk.ms[idx] += (d - g_clk.ms[idx]) * 0.1;
}

struct OvlStats {
    double fps;              /* frames presented per second, smoothed */
    double tps;              /* GAME ticks per second -- diverges from fps
                                whenever the debug menu pauses the tick */
    int tris;                /* polygons gx accepted this frame */
    int actors;              /* live entries on the behaviour list */
    char *player;            /* the Player actor */
    const char *cam_name;
    unsigned mem_kb;         /* working set */
    int menu_paused;
};

static void ovl_draw(const OvlSurface &fb, const OvlStats &s)
{
    char ln[10][96];
    int n = 0;
    const uint32_t WHITE = 0xFFFFFFFFu, AMBER = 0xFFFFC040u,
                   GREEN = 0xFF80FF80u, RED = 0xFFFF6060u;
    uint32_t col[10];
    char *c = s.player;
    void *st = c ? *(void **)(c + 0x370) : 0;

    snprintf(ln[n], sizeof ln[0], "fps %5.1f   tick %5.1f/30%s", s.fps, s.tps,
             s.menu_paused ? "  PAUSED" : "");
    col[n++] = s.fps >= 28.0 ? GREEN : (s.fps >= 20.0 ? AMBER : RED);
    snprintf(ln[n], sizeof ln[0], "frame %5.2fms  in+tick %5.2f  cam %5.2f",
             g_clk.ms[PH_FRAME], g_clk.ms[PH_INPUT], g_clk.ms[PH_CAMERA]);
    col[n++] = WHITE;
    snprintf(ln[n], sizeof ln[0], "submit %5.2f  raster %5.2f  blit %5.2f",
             g_clk.ms[PH_SUBMIT], g_clk.ms[PH_RASTER], g_clk.ms[PH_BLIT]);
    col[n++] = WHITE;
    snprintf(ln[n], sizeof ln[0], "tris %5d  actors %3d  cam %s", s.tris,
             s.actors, s.cam_name);
    col[n++] = WHITE;
    if (c) {
        snprintf(ln[n], sizeof ln[0], "pos %7.1f %7.1f %7.1f",
                 *(int *)(c + 0x5c) / 4096.0f, *(int *)(c + 0x60) / 4096.0f,
                 *(int *)(c + 0x64) / 4096.0f);
        col[n++] = WHITE;
        snprintf(ln[n], sizeof ln[0], "spd h %6.2f v %6.2f  yaw %04x",
                 *(int *)(c + 0x98) / 4096.0f, *(int *)(c + 0xa8) / 4096.0f,
                 (unsigned short)*(short *)(c + 0x8e));
        col[n++] = WHITE;
        snprintf(ln[n], sizeof ln[0], "state %08x  unhosted %u",
                 st ? *(unsigned *)st : 0u, g_port_unhosted_hits);
        col[n++] = g_port_unhosted_hits ? AMBER : WHITE;
    }
    snprintf(ln[n], sizeof ln[0], "ram %6u KB", s.mem_kb);
    col[n++] = WHITE;

    {
        int w = 0;
        for (int i = 0; i < n; ++i) {
            const int lw = (int)strlen(ln[i]) * OVL_ADVANCE * OVL_SCALE;
            if (lw > w) w = lw;
        }
        ovl_shade(fb, 2, 2, w + 6 * OVL_SCALE, n * OVL_LINE + 4 * OVL_SCALE);
        for (int i = 0; i < n; ++i)
            ovl_text(fb, 4 + OVL_SCALE, 4 + i * OVL_LINE, ln[i], col[i]);
    }
}

/* selftest diagnostic: closest clip-approach of the ambient (flag-0x10000)
   actors across the run -- says whether the walk ever brought one inside
   frustum + cull range, i.e. whether its stillness is authentic culling */
struct AmbTrack { void *o; unsigned id; int minr, minfr, thresh; int p0[3]; };
static AmbTrack g_amb[16];
static int g_amb_n;

/* ---- HOW THE ROM CONSUMES THE CAMERA-ROTATE BUTTONS --------------------
   The follow state func_02009e70 reads the pad twice. At 0x02009fdc it takes
   the NEWLY-PRESSED word (data_0209f49e) & 0x4300; at 0x0200a534, only if
   nothing was pressed this frame AND the wall-angle latch cam+0x1a6 is clear,
   it falls back to the HELD word (data_0209f49c) & 0x4300. Both land in the
   same local: 0x200 = rotate left, 0x100 = rotate right, 0x4000 = snap
   behind.

   With one of them live and the turn latch cam+0x1a0 clear, the branch at
   0x0200a6a8 adds a fixed +/-0x400 binangs -- 5.625 degrees -- to the heading
   cam+0x17c and records the direction in cam+0x154 (0x20 left, 0x40 right).
   The heading is then rebuilt into the eye position at 0x0200aa34
   (Vec3_RotateYAndTranslate about the look-at point cam+0x80) and re-derived
   from that position at 0x0200add4: the POSITION is the state and cam+0x17c
   is a per-frame restatement of it. So a tap steps once and a hold steps
   every frame, which is the DS's own feel, and a stick past the threshold is
   a held shoulder button.

   NOTHING IN THE HARNESS TOUCHES ANY OF IT. The bumpers, Q/E and the right
   stick write those pad bits and stop there; no harness write reaches the
   camera's angle, bias or position fields. What made the rotation look like
   isolated jumps was never this code -- it was cstd::atan2's table sitting
   zeroed in the HAL, so every angle the camera read back off a position came
   out 0 and each step was undone on the next frame. The table is the ROM's
   now (port/tools/romdata.py).

   ---- FREECAM: AN OWNED MOD, AND HONEST ABOUT IT ------------------------
   Everything below is the port's, not the game's. F1 (or clicking the right
   stick) hands the VIEW to a harness rig: the right stick orbits it at a rate
   proportional to how far it is pushed, its vertical axis tilts, the bumpers
   or R/F zoom, C re-centres it behind Mario. The rig draws with the ROM's own
   PerspectiveW_ / LookAt_ / CopyToViewMat, so the projection, the view matrix
   at data_0209b3ec and its inverse all end the frame in exactly the state the
   rest of the render expects.

   The Camera actor keeps running underneath the whole time and is never
   written to, so toggling back off hands the view to a camera that has been
   tracking Mario all along and recovers with its own smoothing.

   Two things follow from the mod owning the view. The heading the walk steers
   by is the RIG's while it is on -- published into the same comms record
   Camera::Behavior publishes into, so "forward" is away from the lens you are
   actually looking through, which is the only way walking reads right. And
   the actor cull is not: Camera::Render seeds the Clipper from the Camera's
   own frustum, so an actor the rig can see but the game camera cannot stays
   dormant. That is the price of leaving the Camera actor alone. */
static const int CAM_STEP = 0x400;       /* the ROM's quantum, 0x0200a6a8 */

/* ---- THREE CAMERA MODES, AND ONE RIG -----------------------------------
   The freecam proved the shape: a harness rig that draws through the ROM's own
   PerspectiveW_ / LookAt_ / CopyToViewMat and publishes its own heading, with
   the Camera actor left running untouched underneath it. The only thing that
   made it a "mod" rather than a camera was that it was unpinned from Mario.

   So the same rig, PINNED TO MARIO, is the default camera now:

     CAM_ANALOG  the chase camera. The rig orbits an eased pivot at Mario's
                 chest, the right stick turns it on the analog curve instead of
                 the DS's 5.625-degree steps, and when the stick is idle and he
                 is moving it drifts back behind him -- gently, the way the
                 analog cams in the PC SM64 ports do it, not a snap.
     CAM_FREE    the freecam: the rig orbiting the Camera actor's own look-at,
                 which is what "the harness takes the view" meant before.
     CAM_DS      the hardware's own stepped rotate, byte for byte what this
                 program did before analog existed. Nothing in the analog path
                 runs, the harness writes the rotate bits, and the frame is
                 whatever func_02009e70 makes of them.

   F1 cycles analog -> freecam -> DS. SM64DS_DS_CAMERA=1 boots DS-exact and
   SM64DS_FREECAM=1 boots the freecam.

   THE SELFTEST DEFAULTS TO CAM_DS, deliberately. It is the regression harness:
   its BMP is a byte-comparison against the hardware's framing and its camera
   probes (SM64DS_SELFTEST_ORBIT and the rest) drive the DS rotate bits, none
   of which the analog path writes. SM64DS_ANALOG_CAMERA=1 puts a selftest in
   analog when that is what is being probed.

   WHAT IS TRUE IN ALL THREE: the Camera actor runs its whole frame, is never
   written to, and Camera::Render still seeds the Clipper. The cull is the
   game's, which is the invariant the block above is about -- an actor the rig
   can see but the game camera cannot stays dormant, and that is the price of
   leaving the actor alone rather than a bug to chase. */
enum { CAM_ANALOG = 0, CAM_FREE = 1, CAM_DS = 2 };
static int cam_mode = CAM_DS;    /* main promotes it once the Camera is up */

static const char *cam_mode_name(int m)
{
    return m == CAM_ANALOG ? "analog" : (m == CAM_FREE ? "freecam" : "DS");
}

static short fc_yaw;             /* heading from the pivot to the eye */
static short fc_pitch;           /* elevation of the eye above the pivot */
static int fc_dist;              /* fixed-point world units */

/* ---- WHOSE NUMBER THE DISTANCE IS -------------------------------------
   The ROM's. func_02009e70 does not store a camera distance, it CONVERGES
   one. Every frame the gameplay state does

       Math_Function_0203b14c(&dist, sl, 0x400, r7 + 0x20000, 0x100);

   where sl is the target the camera's own mode block carries -- the block
   at +0x13c, its +0x20 field, scaled through func_020093f4 by the camera's
   +0x104. So LenVec3(eye - at) off the actor is a MOVING number, and one
   sample of it only means anything after the approach has settled.

   The rig used to take exactly one sample, in fc_seed, and hold it for the
   rest of the session. That is right for the F1 toggle it was written for
   -- the camera has long since settled, so the sample is the settled value
   and the toggle does not move the picture. It is wrong at a LEVEL ENTRY,
   which is the other place the seed fires (boot, and the level handoff):
   the entrance parks the camera at the far end of its approach, so the
   sample IS the transient. Measured at the entry frame against what the
   same camera reads ninety frames on, with the approach long done:

       level 1  castle grounds   805 sampled, 840 settled   (-4%)
       level 2                  1025 sampled, 763 settled  (+34%)
       level 6  Bob-omb Bfield  2040 sampled, 806 settled  (+153%)
       level 7                  1501 sampled, 561 settled  (+168%)

   The castle grounds is where the rig was built and it lands within 4%,
   which is why this went unnoticed; Bob-omb Battlefield's entrance swoops
   in from two and a half times the gameplay distance, and the rig froze the
   first frame of the swoop. Warping in from the castle it is plainer still:
   the rig sat at 805 all through the castle, jumped to 2040 on the handoff
   frame, and stayed there while the game's own camera came down to 840.

   So the rig stops treating the distance as its own. It reads the actor
   every frame, which is the number func_02009e70 spent that frame
   converging, and the entrance swoop plays through the rig the way it plays
   on hardware. The zoom control still wins when the player uses it: that
   sets fc_dist_owned and the tracking stands down until the next seed hands
   the distance back.

   One rule for both rig modes, freecam included. The freecam orbits the
   Camera actor's own look-at rather than Mario, so it is pinned to the same
   camera and the same distance reads right there too; a player who wants to
   hold a distance while inspecting something takes it with one press of the
   zoom, which is the control that already meant that. */
static int fc_dist_owned;        /* the player has zoomed; stop tracking */

/* the Camera actor's own eye-to-look-at length */
static int fc_cam_dist(void *cam)
{
    const char *k = (const char *)cam;
    const int *at = (const int *)(k + 0x80);
    const int *eye = (const int *)(k + 0x8c);
    int d[3] = {eye[0] - at[0], eye[1] - at[1], eye[2] - at[2]};
    int n = LenVec3(d);
    return n < 0x40000 ? 0x40000 : n;
}

/* the analog rig's own pivot: Mario's position lifted to about chest height
   and eased, so the picture does not carry the per-frame jitter of a walk
   cycle into the lens */
static int an_pivot[3];
static int an_pivot_live;
static const int AN_LIFT = 140 << 12;     /* world fx above his feet */

/* stick deflection -> binangs (or units) per frame, signed. Half linear,
   half squared: fine control near the centre, `top` at the stop. */
static int fc_stick_rate(int v, int top)
{
    const int dead = 8000;       /* XInput's own right-stick floor */
    int mag = v < 0 ? -v : v;
    long long span, m;
    int r;
    if (mag <= dead) return 0;
    if (mag > 32767) mag = 32767;
    span = 32767 - dead;
    m = mag - dead;
    r = (int)((long long)top * (m * span + m * m) / (2 * span * span));
    return v < 0 ? -r : r;
}

/* the rig's eye, in the ROM's own fixed point:
   eye = pivot + rotY(yaw) * (0, dist*sin(pitch), dist*cos(pitch)),
   the same shape func_02009e70 builds its own eye with at 0x0200aa34 */
static void fc_eye(const int *pivot, int *eye)
{
    const int i = ((int)(unsigned short)fc_pitch >> 4) * 2;
    int p[3] = {pivot[0], pivot[1], pivot[2]};
    int src[3];
    src[0] = 0;
    src[1] = (int)(((long long)fc_dist * data_02082214[i] + 0x800) >> 12);
    src[2] = (int)(((long long)fc_dist * data_02082214[i + 1] + 0x800) >> 12);
    Vec3_RotateYAndTranslate(eye, p, fc_yaw, src);
}

/* seed the rig from wherever the Camera actor is, so the toggle does not
   move the picture. The distance goes back to being the game's (the block
   above): a seed is the point at which the rig has no framing of its own to
   defend, which is exactly when the ROM's number should be in charge. */
static void fc_seed(void *cam)
{
    char *k = (char *)cam;
    int *at = (int *)(k + 0x80);
    int *eye = (int *)(k + 0x8c);
    fc_yaw = *(short *)(k + 0x17c);
    fc_pitch = Vec3_VertAngle(eye, at);
    fc_dist = fc_cam_dist(cam);
    fc_dist_owned = 0;
}

/* the analog rig's pivot, stepped once a frame. Eased toward Mario's chest at
   a quarter of the remaining distance, which at 30Hz is about an eighth of a
   second of lag -- enough to swallow the walk cycle, not enough to feel like
   the camera is on a rope. A jump in his position bigger than any frame of
   movement can be (a respawn, a menu warp) snaps instead of easing. */
static void an_step_pivot(char *player)
{
    const int tgt[3] = {*(int *)(player + 0x5c),
                        *(int *)(player + 0x60) + AN_LIFT,
                        *(int *)(player + 0x64)};
    int k;
    if (!an_pivot_live) {
        an_pivot_live = 1;
        for (k = 0; k < 3; ++k) an_pivot[k] = tgt[k];
        return;
    }
    for (k = 0; k < 3; ++k) {
        const int d = tgt[k] - an_pivot[k];
        an_pivot[k] += (d > (4000 << 12) || d < -(4000 << 12)) ? d : d / 4;
    }
}

/* the view the mod draws with: the ROM's own two entry points, fed the rig's
   eye and pivot in scene units (the (v + 4) >> 3 Camera::Render applies to
   its own) */
static void fc_push_view(void *cam, const int *eye, const int *at)
{
    const int i = ((int)*(unsigned short *)((char *)cam + 0x17a) >> 4) * 2;
    int e[3], a[3], mat[12];
    int q;
    for (q = 0; q < 3; ++q) {
        e[q] = (eye[q] + 4) >> 3;
        a[q] = (at[q] + 4) >> 3;
    }
    _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
        data_02082214[i], data_02082214[i + 1],
        *(int *)((char *)cam + 0xf8), *(int *)((char *)cam + 0xfc),
        *(int *)((char *)cam + 0x100), data_0209ee90[0x44 / 4], 1, 0);
    _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(e, data_02086efc, a, 1, mat);
    _Z13CopyToViewMatPK9Matrix4x3(mat);
}

/* ---- THE DEBUG MENU (port mod) ----------------------------------------
   F5. Deliberately small: the things worth reaching mid-session without
   restarting the program under a different environment, plus one status line.
   Up/down or the d-pad move, left/right change a value, and enter (or A) does
   what right does, so the whole thing works one-handed on a pad. BACK opens
   and closes it and so does B, so a pad never needs the keyboard to get out.

   IT PAUSES THE GAME TICK while it is open -- port_actor_tick is skipped and
   the input the harness writes is zeroed, so nothing moves, nothing spawns and
   nothing decides anything while a person is reading. Rendering carries on, so
   the frame stays live behind the menu and the F3 overlay's fps keeps counting
   while its TICK rate falls to zero. That divergence is what the two numbers
   are next to each other for. */
enum {
    MENU_WARP = 0,
    MENU_LEVEL,
    MENU_MINIGAME,      /* left/right pick one of the ROM's thirty, enter goes */
    /* ---- VS wiring lane: the section the owner asked for -- choose a VS map
       and game mode, then boot the ROM's own VS mode. Enter on either row
       starts VS on the showing map (a relaunch, the minigame row's shape: the
       child boots the ROM's own start through SM64DS_VS_MAP; see the boot
       hook past the game-heap init). The ROM's VS has exactly ONE game mode
       -- the star battle; no mode selector exists in ov075
       (port/slice_vs.txt section 4) -- so the mode row states it rather than
       inventing choices. */
    MENU_VS_MAP,        /* left/right walk the ROM's four VS maps, enter goes */
    MENU_VS_MODE,       /* the ROM's single mode, stated; enter goes too      */
    MENU_EXIT,
    MENU_CHARACTER,
    MENU_SNAP,
    MENU_OVERLAY,
    MENU_CAMERA,
    MENU_RUNMODE,       /* left/right: how running works (the block below) */
    MENU_RUNBIND,       /* enter: capture the next press as the run button */
    MENU_RECORDER,
    MENU_SAVESTATE,     /* enter: snapshot the game into the in-memory slot */
    MENU_LOADSTATE,     /* enter: restore the in-memory slot (F9's twin) */
    MENU_COUNT
};

/* ---- HOW RUNNING WORKS (port mod, pure input shaping) -------------------
   THE DS HAD NO RUN BUTTON. Its stick was the touch screen, and the game
   decides between walking and running from HOW FAR that stick is pushed:
   func_ov002_020d4748, func_ov002_020d1f78 and their siblings all read the
   same pair of fields out of the input record and branch on

       touching == 0  ->  running iff the held word has 0x800
       touching != 0  ->  running iff the magnitude is past ~0xdc7 of 0x1000
                          (0x80 of hysteresis on top, so the flip does not
                          chatter around the threshold)

   and the SPEED TARGET is scaled by that same magnitude -- func_ov002_020bf224
   is literally `base * mag >> 12`, floored. So the game already has an analog
   run and always did; the port simply never fed it one. This window has fed
   the D-pad branch since it existed: magnitude pinned at 0x1000, touching
   zero, running on a held 0x800 that the hardware never had a button for.

   These three modes pick which of those branches the player's controls
   reach. Nothing here is game logic. The record the game reads is the record
   the game has always read, filled in by the host the way the hardware would
   have filled it in, and src/ neither knows nor could tell.

     RUN_BUTTON  what this program has always done. The bound key or pad
                 button sets 0x800 while it is held, the record stays on the
                 D-pad branch, and letting go walks. The DEFAULT, so a player
                 who never opens this menu is playing the same program.
     RUN_ANALOG  when a pad stick is actually pushed, the record is filled
                 from it instead -- touching set, magnitude and direction
                 from the deflection -- and the GAME's own thresholds do the
                 rest. Partial push walks, past ~87 percent runs, and the
                 speed in between is the ROM's own multiply. With no pad, or
                 with the stick inside its dead zone, this is button mode:
                 there is no deflection to read off a keyboard.
     RUN_AUTO    0x800 is held for you, always. No button, always running.

   The run BINDING is per device on purpose. Rebinding to a key leaves the
   pad alone and rebinding to a pad button leaves the keyboard alone, because
   a player who moves run off shift on the keyboard has said nothing about
   what X should do on a controller. */
enum { RUN_BUTTON = 0, RUN_ANALOG, RUN_AUTO, RUN_MODE_COUNT };
static const char *const RUN_MODE_NAME[RUN_MODE_COUNT] = {
    "button   (hold it to run)",
    "analog   (how far the stick is pushed)",
    "auto     (always running, no button)"
};
static int g_run_mode;                /* settings.json RunMode */
static int g_run_key = 0x10;          /* settings.json RunButtonKey, VK_SHIFT */
static int g_run_pad = 0x4000;        /* settings.json RunButtonPad, pad X */

/* THE REBIND CAPTURE, armed from the menu's rebind row and read by the window
   procedure. A capture has to swallow the press it is capturing, or binding
   run to F3 would also toggle the overlay on the way past. So the capture
   lives where the presses arrive: while it is armed, WM_KEYDOWN never reaches
   the window at all, and the key it saw is handed to the frame loop through
   g_rebind_key.

   THE ESCAPE HALF OF THAT ARGUMENT IS GONE (run mg10, lane ESC). It used to
   read "and binding it to escape would close the game -- and escape is the one
   key the window procedure, not the frame loop, acts on", which was the whole
   reason the swallow had to be up there rather than in key_live. Escape is an
   ordinary frame-loop key now, gated by key_live like every other, so the
   capture no longer needs to protect it from anything. The swallow stays for
   the F3 reason above, which was always the load-bearing one.

   Level key reads are gated too, one line inside key_live, so nothing that
   was already held keeps steering while a person is picking a button. */
static int g_rebind_capture;          /* armed: swallow presses, report them */
static int g_rebind_key;              /* the virtual-key the proc last saw */

/* ---- LIVE KEYBOARD, GATED (port mod) --------------------------------------
   File scope, and a function rather than main's lambda, because there are TWO
   frame loops now -- the level loop in main and scene_window_run's -- and
   three separate gates live in here. A second copy would be three gates to
   keep in step, and the one that matters most is invisible when it is right:
   GetAsyncKeyState reports machine-global key state, focus or no focus, so an
   automated run picks up anyone typing anywhere on the box unless something
   stops it.

   GATE 1, THE SELFTEST. Under SM64DS_WINDOW_SELFTEST every live keyboard read
   here returns released, and the scripted probes are the only input a
   comparator run has. g_selftest is set by main from its own local before the
   frame loop starts. It is never set on the scene path, which cannot be a
   selftest -- port_scene_want_window refuses a window to one before anything
   else is asked.

   GATE 2, THE REBIND CAPTURE. While the rebind row is capturing, the whole
   window is deaf: the press being captured must not also be a camera toggle or
   a save state, and whatever was already held must not keep steering while a
   person is choosing a button. One line here covers every caller, which is why
   there is not a gate at each of them.

   GATE 3, FOCUS. The same machine-global read meant that a player who
   alt-tabbed and typed somewhere else kept walking Mario around, and that a
   direction held at the moment they left stayed held forever.
   hal_window_focused() is false whenever this window is not the foreground
   one, and then every interactive key here reads RELEASED. That is also the
   release: the pad words are rebuilt from these reads every frame, so the
   frame focus goes away is the frame the stick and the buttons go to neutral,
   with no separate teardown to keep in step.

   Nothing a player pressed while away arrives late either. There is no queue
   to replay -- these are level reads, not messages -- but a key still
   physically down on the way back would otherwise read as a fresh press and
   fire the edge latches (F1, F3, F4, the menu). So on the focus-regained edge
   every key is marked STALE, and a stale key keeps reading released until it
   is seen physically up. Pressing it again after that works normally. Both
   loops set that mark, which is why key_stale lives here with the function.

   key_stale is indexed by virtual-key code, which is what every caller passes
   and is 0..255 by definition; the bounds test is there because this function
   is the one place that would turn a typo into a stray write. */
static unsigned char key_stale[256];
static int g_selftest;

static int key_live(int vk)
{
    if (g_selftest) return 0;
    if (g_rebind_capture) return 0;
    if (!hal_window_focused()) return 0;
    const int down = W.GetAsyncKeyState_(vk) < 0;
    if ((unsigned)vk < 256) {
        if (!down) { key_stale[vk] = 0; return 0; }
        if (key_stale[vk]) return 0;
    }
    return down;
}

/* Keys and pad buttons this program has already spoken for. Binding run to
   one of them would not produce a conflict a player could see and undo; it
   would produce a menu that cannot be closed, or one that opens every time you
   start running. So the capture refuses them and says so, which is the one
   case where refusing is friendlier than obeying. */
static int run_key_reserved(int vk)
{
    if (vk == VK_ESCAPE || vk == VK_RETURN) return 1;   /* menu open, menu act */
    /* the port's own row. F10 is in it because Windows itself takes F10 as
       the menu-bar activator, so a run bound to it would open the system
       menu on every step; F11 and F12 are the fullscreen toggle. */
    if (vk >= VK_F1 && vk <= VK_F12) return 1;
    if (vk >= VK_LEFT && vk <= VK_DOWN) return 1;       /* menu navigation */
    /* and the WASD aliases of those four, since the same change that made
       them menu navigation. CONSISTENCY RATHER THAN A BUG FIX, and labelled
       as such: run is zeroed with everything else while the menu is open
       (`if (menu_on) btn = 0`), so a run bound to D would not actually fight
       the cursor. But this list is meant to read as "the keys this program
       has spoken for", and leaving the walk keys out of it after handing them
       a second job is how the list stops being true. */
    if (vk == 'W' || vk == 'A' || vk == 'S' || vk == 'D') return 1;
    /* the bottom-screen panel, and it is read in hal/sub_screen.cpp rather
       than through this file's key_live -- so a binding on it would fire the
       panel from outside every gate here */
    if (vk == VK_TAB) return 1;
    return 0;
}
static int run_pad_reserved(unsigned mask)
{
    /* the d-pad (0x000f) and A (0x1000) drive the menu, BACK (0x0020) opens
       it and B (0x2000) closes it, and the right stick's click (0x0080) is
       the freecam toggle */
    return (mask & 0x30afu) != 0;
}

/* A printable name for a binding. The letters and digits are their own ASCII
   so they need no table, and everything a player is likely to reach for is
   in the short one; anything else prints as its code, which is at least the
   number they would put in settings.json by hand. */
static const char *run_key_name(int vk, char *buf, size_t cap)
{
    static const struct { int vk; const char *name; } NAMED[] = {
        { 0,           "unbound"   }, { VK_SHIFT,   "shift"     },
        { VK_CONTROL,  "ctrl"      }, { VK_MENU,    "alt"       },
        { VK_SPACE,    "space"     }, { VK_TAB,     "tab"       },
        { VK_LSHIFT,   "left shift"}, { VK_RSHIFT,  "right shift" },
        { VK_LCONTROL, "left ctrl" }, { VK_RCONTROL,"right ctrl"},
        { VK_CAPITAL,  "caps lock" }, { VK_BACK,    "backspace" },
        { VK_OEM_3,    "backtick"  }, { VK_OEM_COMMA, "comma"   },
        { VK_OEM_PERIOD, "period"  }, { VK_OEM_2,   "slash"     },
        { VK_OEM_1,    "semicolon" }, { VK_OEM_7,   "quote"     },
        { VK_OEM_4,    "["         }, { VK_OEM_6,   "]"         },
        { VK_OEM_MINUS,"-"         }, { VK_OEM_PLUS,"="         },
    };
    for (unsigned i = 0; i < sizeof NAMED / sizeof NAMED[0]; ++i)
        if (NAMED[i].vk == vk) return NAMED[i].name;
    if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) {
        snprintf(buf, cap, "%c", (char)vk);
        return buf;
    }
    snprintf(buf, cap, "key 0x%02x", (unsigned)vk);
    return buf;
}
static const char *run_pad_name(int mask, char *buf, size_t cap)
{
    switch (mask) {
    case 0:      return "unbound";
    case 0x1000: return "pad A";
    case 0x2000: return "pad B";
    case 0x4000: return "pad X";
    case 0x8000: return "pad Y";
    case 0x0100: return "pad LB";
    case 0x0200: return "pad RB";
    case 0x0010: return "pad start";
    case 0x0040: return "left stick click";
    default: break;
    }
    snprintf(buf, cap, "pad 0x%04x", (unsigned)mask);
    return buf;
}

/* ---- CHARACTER SWITCH (port mod, driven through the game's own save byte) --
   THE CHARACTER IS SAVE DATA, not a Player field, and that is the whole reason
   this is a boot setting rather than a live toggle. LoadEntranceObjects builds
   the Player's spawn param as

       f2 = data_0209caa0[0x41];  flags = f2 | (f1 << 3) | (i << 6) | (sl << 8)

   and Player::InitResources unpacks bits 0-2 of that into +0x6d9. So the byte
   is read once, at spawn, and everything downstream follows from it: the body
   model slots at +0xdc are indexed by GetBodyModelID(character), +8 is the live
   index Player_ScaleByCharFactor and the port's own render read,
   func_ov002_020e4bb8 mirrors +8 into +0x6db for Player::Render, and
   Sound::PlayCharVoice picks the voice bank off +0x6d9.

   WHY NOT POKE IT LIVE, which was the first thing tried: InitResources loads
   ONLY THE SPAWNED CHARACTER'S models. The other slots at +0xdc stay null, so
   moving the index alone points Player_AdvanceAnims at a null model and
   Animation::Advance divides by zero on the first frame. The ROM's own
   character door (func_ov002_020be3b0) does the in-place swap properly, and it
   loads the incoming character's files first through the SharedFilePtr table
   at data_ov002_020ff480 before it moves anything. Hosting that is the job
   that would make this row live; until then the row sets the save byte and the
   next boot spawns whoever it names.

   Yoshi is a separate caveat: St_YoshiPower and St_InYoshiMouth are matched in
   src but are not wired into player_states.inc, so his tongue is a loud no-op.
   Walking, running, jumping and the dust are character-agnostic code. */
static const char *const CHAR_NAME[4] = { "Mario", "Luigi", "Wario", "Yoshi" };
static int g_character;                     /* what the boot actually spawned */
static int g_character_pending;             /* what the next boot will spawn */

/* data_0209caa0 is declared int[] above (word 2 carries flag bits the boot
   sets); LoadEntranceObjects reads the character as a BYTE at 0x41, which is
   why this casts rather than indexing the int view. */
static void character_set_pending(int ch)
{
    g_character_pending = ch & 3;
    ((unsigned char *)data_0209caa0)[0x41] =
        (unsigned char)g_character_pending;
}

/* Changes character on the spot by re-running Player::InitResources with a
   rewritten spawn param, carrying position and speed across. Port code, not
   the game's: the game's own in-place change is the CAP path, and there is no
   Yoshi cap for it to run. The reasoning is in hal/player_bridges.cpp, where
   it lives because it wants Player.h. SM64DS_SWITCH=<0..3> drives it headless. */
extern "C" void port_player_set_character(void *player, unsigned ch);
static int menu_on;
/* B closed the menu and is still physically down: swallow it until it comes
   back up. See the block below the menu's input, where it is spent. */
static int menu_b_swallow;
static int menu_sel;
static int menu_entrance;             /* the entrance the warp row is showing */
/* THE LEVEL ROW IS THE DEBUG LEVEL SELECT'S OWN LIST. dScTitle_c (ov003,
   scene 2) picks a row out of data_ov003_020b1180 -- 0x36 eight-byte rows,
   byte 0 the level and byte 1 the entrance -- and calls LoadLevelNoReturn
   with the pair. The port mounts that table (port/ov003_syms.txt) and runs
   the same call; what it does not run is the scene actor around it, so the
   grid, its cursor and its music are not here. The row list and the handoff
   are Nintendo's, the presentation is this menu's.
   Rows the port cannot mount yet are still SHOWN, marked, because the point
   of the list is that it is the game's own and the gap is visible. */
static int menu_level_row = 1;
/* the VS row's map cursor, 0..3 into the ROM's own list at
   data_ov075_0211c6ec (hal/scene_vs_menu.cpp reads it back by index) */
static int menu_vs_map;
extern "C" int port_vs_map_level(int mapIdx);   /* hal/scene_vs_menu.cpp */
/* the VS boot hook's seams (see THE VS BOOT block in main): the ROM's own
   start, the port's one level latch, and the request release -- all extern
   "C", declared here because a block-scope extern in C++ mangles */
extern "C" void port_vs_stage_and_start(int mapIdx);   /* hal/scene_vs_menu.cpp */
extern "C" int  port_level_entry_latch(void);          /* hal/level_change.cpp  */
extern "C" int  port_scene_request_release(const char *why);
extern "C" void port_level_set_target(int level);      /* hal/level_boot.cpp    */
static int g_overlay_on;              /* F3, and the menu's overlay row */
static char g_playlog[160] = "off";   /* the flight recorder's current file */
/* fault_probe.h's rich dump tails this after a crash; a stable pointer at the
   buffer so the header does not need to name g_playlog's storage. "off" until
   the recorder opens a real file below. */
extern "C" const char *port_playlog_path = g_playlog;

/* The harness ground snap and wall clamp, a boot-time const off
   SM64DS_FAKE_SNAP until now, so the A/B was a restart. It is a switch.
   What CANNOT move at runtime is the third thing that env chose: the level
   collider's OWNER is decided once, before the Player exists (the
   MeshColliderBase::Enable call in main), and an owner of the Player makes
   every player probe skip the level. The row says so rather than pretending
   the toggle is the whole switch. */
static int g_fake_snap;

/* ---- THE MINIGAME PICKER (port mod) -------------------------------------

   THE SET IS THE ROM'S. src/IsMinigameActorID.c is `id >= 0x169 && id <=
   0x186`: thirty contiguous actor ids, 361..390, and it is the same predicate
   hal/scene_boot.cpp gates the stacked layout on and hal/scene_mg.cpp gates
   the ov006 bring-up on. This table makes no membership decision of its own.
   It holds NAMES, and nothing else.

   THE NAMES ARE THE PLAYER'S, AND THAT IS A CORRECTION. This table used to
   carry the ROM's own class names, read out of the ov006 typeinfo with the
   dScMg prefix and _c suffix stripped, so 368 read "pachinko" and 374 read
   "curling". Those are the right names for CODE, and they are still what the
   registry rows in hal/scene_boot.cpp are keyed on: SCENE_MG_PACHINKO is 368
   there and stays that way. They are the wrong names for a MENU, and the
   failure was not theoretical. A play session read "pachinko" on this row
   against the Bob-omb slingshot game on screen and concluded the port had
   wired the scene to the wrong class. It had not. Pachinko is the Japanese
   name for a slingshot game, that is what Nintendo's developers called
   dScMgPachinko_c, and the menu was the only thing in the chain that was
   wrong. A debug menu that makes a correct port look broken is worse than no
   menu, so the strings below are the retail titles, and the class short name
   the row used to show now sits in the comment beside it.

   THE TITLES ARE DERIVED AND NOT REMEMBERED, and the derivation is one step.
   Actor id maps to scene id by identity -- 0x169 IS 361 -- so the rows below
   are the ov006 ids in order with no arithmetic between the two columns.
   Twenty-three of the thirty carry an Mg*_SpawnInfo symbol in
   config/arm9/overlays/ov006/symbols.txt, and that symbol IS the retail title
   with its spaces taken out: MgBobOmbSquad is Bob-omb Squad,
   MgCoincentration is Coincentration, MgShuffleShell is Shuffle Shell. All
   twenty-three are expanded below with nothing added. Twenty-two of them are
   confirmed a second time by port/mg_fanout_costs.txt section 3, which
   reaches the same symbol through the factory's own vtable load; the
   twenty-third is 0x179 and is discussed below. The six seated ids are
   confirmed a third time by the registry rows in hal/scene_boot.cpp, which
   name the SpawnInfo symbol they spawn.

   SEVEN IDS HAVE NO SYMBOL AND ARE NOT GUESSED. 0x169, 0x16c, 0x16d, 0x173,
   0x177, 0x17d and 0x186 reach a bare data_ov006_* address instead of an Mg*
   name. Six of the seven keep the old class short name with a question mark
   welded on -- "cup?", "slot1?" -- because a wrong confident title is worse
   than an honest dev name, and the mark is the whole point: it says the row
   is not a claim. The seventh is 0x186, and it is not a guess either.
   hal/scene_boot.cpp pinned it to dScMgFlower_c off the flower asset strings
   and seated it, so it reads "Loves Me...?" with no mark.

   377 KEEPS ITS QUESTION MARK FOR THE OLDER REASON. It is the one id whose
   factory reaches no signature table at all. ov006's thirty-second dScMg*_c
   typeinfo, dScMgSnowball_c at 0x0213ffdc, is 0x24 from 0x179's own
   MgSnowballSlalom_SpawnInfo at 0x0213ffb8, which is the locality the other
   twenty-nine rows show, and the peer screening independently called 0x179
   MgSnowballSlalom. That is an inference and not a read, and the menu should
   not present the two as the same claim.

   THE STRINGS ARE DISPLAY ONLY. The menu row, the on-screen toast and three
   stderr lines are their whole readership; nothing outside this file reads
   them. Renaming a row cannot change which scene the row starts.

   THEY ALSO HAVE A WIDTH BUDGET, and it is measurable rather than a feeling.
   The overlay font advances 6 pixels a character, walk_window links ntr_2x so
   ntr::SCREEN_W is 512 and OVL_SCALE is 1, and ovl_text clips at the
   framebuffer edge, so a row goes silently missing past about 83 characters.
   The minigame row costs 57 characters before the title, which makes 21 the
   ceiling, and two titles were shortened to reach it. Both are classes that
   really do serve two menu games, which is why both halves survive at all:
   MgBingoBallSlotsShot lost the spaces around its slash ("Slots Shot/Bingo
   Ball", 21) and MgPuzzlePanelPuzzlePanic lost the repeated word ("Puzzle
   Panel/Panic", 18). Nothing else was cut.

   WHAT IS SELECTABLE IS NOT DECIDED HERE EITHER. port_scene_is_hosted asks
   the registry that hal/scene_boot.cpp refuses unhosted ids out of, so as
   minigames get seated their rows light up with no edit to this file -- and
   the wired-first display order below re-sorts itself for the same reason. */
static const struct { short id; const char *name; } MG_SCENE[] = {
    /*  id      menu title              actor  ov006 class  derivation       */
    { 361, "cup?"                  }, /* 0x169  cup          no Mg* symbol   */
    { 362, "Memory Match"          }, /* 0x16a  memory       MgMemoryMatch   */
    { 363, "Memory Master"         }, /* 0x16b  memory2      MgMemoryMaster  */
    { 364, "slot1?"                }, /* 0x16c  slot1        no Mg* symbol   */
    { 365, "slot3?"                }, /* 0x16d  slot3        no Mg* symbol   */
    { 366, "Wanted!"               }, /* 0x16e  luigi        MgWanted        */
    { 367, "Boom Box"              }, /* 0x16f  sound        MgBoomBox       */
    { 368, "Bob-omb Squad"         }, /* 0x170  pachinko     MgBobOmbSquad   */
    { 369, "Lakitu Launch"         }, /* 0x171  pachinko2    MgLakituLaunch  */
    { 370, "Sort or 'Splode"       }, /* 0x172  bomroom      MgSortOrSplode  */
    { 371, "amida?"                }, /* 0x173  amida        no Mg* symbol   */
    { 372, "Bounce and Pounce"     }, /* 0x174  jump         MgBounceAndPounce  */
    { 373, "Bounce and Trounce"    }, /* 0x175  jump2        MgBounceAndTrounce */
    { 374, "Shuffle Shell"         }, /* 0x176  curling      MgShuffleShell  */
    { 375, "Shell Smash"           }, /* 0x177  curling2     see below       */
    { 376, "Slots Shot/Bingo Ball" }, /* 0x178  smartball    MgBingoBallSlotsShot, trimmed */
    { 377, "Snowball Slalom?"      }, /* 0x179  snowball     MgSnowballSlalom, INFERRED */
    { 378, "Coincentration"        }, /* 0x17a  coin         MgCoincentration   */
    { 379, "Picture Poker"         }, /* 0x17b  card         MgPicturePoker  */
    { 380, "Puzzle Panel/Panic"    }, /* 0x17c  panel        MgPuzzlePanelPuzzlePanic, trimmed */
    { 381, "mcarlo?"               }, /* 0x17d  mcarlo       no Mg* symbol   */
    { 382, "Pair-a-Gone and On"    }, /* 0x17e  mcarlo2      MgPairAGoneAndOn   */
    { 383, "Mushroom Roulette"     }, /* 0x17f  roulette     MgMushroomRoulette */
    { 384, "Trampoline Time"       }, /* 0x180  trampoline   MgTrampolineTime   */
    { 385, "Trampoline Terror"     }, /* 0x181  trampoline2  MgTrampolineTerror */
    { 386, "Which Wiggler?"        }, /* 0x182  hanachan     MgWhichWiggler  */
    { 387, "Hide and Boo Seek"     }, /* 0x183  teresa       MgHideAndBooSeek   */
    { 388, "Lucky Stars"           }, /* 0x184  bsc          MgLuckyStars    */
    { 389, "Psyche Out!"           }, /* 0x185  3desp        MgPsycheOut     */
    { 390, "Loves Me...?"          }, /* 0x186  flower       pinned dScMgFlower_c */
};
enum { MG_COUNT = (int)(sizeof MG_SCENE / sizeof MG_SCENE[0]) };

/* 375's ROW LOST ITS QUESTION MARK AND THE OTHER FIVE KEPT THEIRS, run mg6
   lane S75. The row read "curling2?" because 0x177 has no Mg* spawn symbol IN
   ov006. The CLASS name is a ROM read -- the typeinfo record at
   data_ov006_0213c510[-1] points at "15dScMgCurling2_c" -- and the TITLE is
   named by the tree, at an address the ROM verifies:

     the arm9 spawn table data_02090864, entry 0x177 at 0x02090e40, holds
     0x0213c434, and config/arm9/overlays/ov098/symbols.txt:113 names that
     record MgShellSmash_SpawnInfo.

   IT IS FILED UNDER ov098 BECAUSE ov006 AND ov098 OVERLAP THAT ADDRESS -- the
   row is marked `ambiguous` -- which is why a sweep of ov006's own symbol file
   does not find it. It predates this lane: it came in with the 2026-07-10
   spawn-table naming import (#211). THE ADDRESSING IS CHECKED RATHER THAN
   ASSUMED, by its two neighbours in the same table: entry 0x176 holds
   0x0213c214 and entry 0x178 holds 0x0213ebd0, which are MgShuffleShell_
   SpawnInfo and MgBingoBallSlotsShot_SpawnInfo at exactly the addresses
   port/mg_fanout_costs.txt sections 4 and 11 give them.

   WHAT STAYS TRUE IS THAT THE ROM ITSELF STORES NO TITLE TEXT, and that is a
   finding rather than a caveat about this row: searching the whole 16 MB image
   for the ASCII and UTF-16LE spellings of five known titles returns zero hits
   each, the five EUR language archives hold battle-mode art, and the one
   per-language MG sheet decodes to "RULES", "TIME" and "HIGH SCORE". So no
   lane can read a title off the ROM; a title comes from the config, and this
   one has one. port/slice_s75.txt carries the corroborating asset evidence
   (the class opens ".../jokyu_curling..." and jokyu marks the harder half of a
   pair) as support for the name rather than as a substitute for it. */

/* ---- THE LEVEL-SELECT ROW'S NAMES (port mod) ----------------------------

   THE LEVEL ROW USED TO READ AS NUMBERS ONLY -- "level 6 entrance 13 ov014"
   -- which is unreadable for the one thing a level select is for: knowing
   which level a row boots. This table gives each ROM level id (byte 0 of a
   data_ov003_020b1180 row) a short player-facing name; the numeric id stays
   as the prefix because this is a debug menu and the id is what a bug report
   quotes.

   THE NAMES ARE NOT INVENTED. The authority is port_level_table[] in
   hal/level_boot.cpp: forty-six ids, each derived from the ROM's own overlay
   data (data_020758c8 -> overlay, data_02092208 -> LVL_Overlay, the four
   asset handles) with the derivation written out per wave in that file. Its
   leading name is the retail course name; the short forms below are that name
   trimmed to fit the row, and every id here agrees with that table's id and
   internal (romanised) name -- which cross-checks against the ROM's own debug
   stage-name pool in ov003 (base 0x020b1060: "BombHei Map", "Snow Mt",
   "Habatake", "Suisou" ...). port/debug_stage_names.txt records the source and
   a confidence flag per id.

   IDS NOT HERE FALL BACK TO THE NUMBER, never to a guess. The six ids
   port_level_table[] does not mount are 29/41/42/43/51 (the wave-C block
   proves these are not stages: their name handles point past the ROM's handle
   space, so they have no course behind them) and 31 (habatake, the Wing Cap
   tower -- derived, deliberately not mounted). Only 31 gets a name here, at
   lower confidence, because it is a real stage the ROM names; the other five
   have no stage to name and the row shows "(level N)" for them. An honest
   number beats a wrong name in a debug menu.

   ADDING OR RENAMING IS ONE EDIT: one row here, keyed by id, searched not
   indexed, so gaps are fine and order does not matter. */
static const struct { short id; const char *name; } LEVEL_NAME[] = {
    {  0, "Dev Test Map"        },  /* test_map, cut content, no course      */
    {  1, "Castle Grounds"      },  /* main_castle                           */
    {  2, "Castle 1F"           },  /* castle interior, first floor          */
    {  3, "Castle Garden"       },  /* main_garden                           */
    {  4, "Castle Basement"     },  /* castle_b1                             */
    {  5, "Castle 2F"           },  /* castle_2f                             */
    {  6, "Bob-omb Battlefield" },  /* bombhei_map                           */
    {  7, "Whomp's Fortress"    },  /* battan_king_map                       */
    {  8, "Jolly Roger Bay"     },  /* kaizoku_irie                          */
    {  9, "JRB Sunken Ship"     },  /* kaizoku_ship, inside the JRB ship     */
    { 10, "Cool Cool Mountain"  },  /* snow_mt                               */
    { 11, "Cool Cool Mtn Slide" },  /* snow_slider                           */
    { 12, "Big Boo's Haunt"     },  /* teresa_house                          */
    { 13, "Hazy Maze Cave"      },  /* cave                                  */
    { 14, "Lethal Lava Land"    },  /* fire_land                             */
    { 15, "LLL Volcano"         },  /* fire_mt, the volcano interior         */
    { 16, "Shifting Sand Land"  },  /* desert_land                           */
    { 17, "SSL Pyramid"         },  /* desert_py                             */
    { 18, "Dire Dire Docks"     },  /* water_land                            */
    { 19, "Snowman's Land"      },  /* snow_land                             */
    { 20, "Snowman's Igloo"     },  /* snow_kama                             */
    { 21, "Dire Docks (City)"   },  /* water_city, DDD second area           */
    { 22, "Tall Tall Mountain"  },  /* high_mt                               */
    { 23, "Tall Tall Mtn Slide" },  /* high_slider                           */
    { 24, "Tiny-Huge Isle (Big)"},  /* tibi_deka_d, huge side                */
    { 25, "Tiny-Huge Isle (Sm)" },  /* tibi_deka_t, tiny side                */
    { 26, "Tiny-Huge Isle Cave" },  /* tibi_deka_in                          */
    { 27, "Tick Tock Clock"     },  /* clock_tower                           */
    { 28, "Rainbow Cruise"      },  /* rainbow_cruise                        */
    { 30, "Secret Aquarium"     },  /* suisou                                */
    { 31, "Wing Cap Tower"      },  /* habatake -- NOT mounted; see .txt     */
    { 32, "Vanish Cap (Moat)"   },  /* horisoko                              */
    { 33, "Metal Cap Switch"    },  /* metal_switch                          */
    { 34, "Wing Mario Rainbow"  },  /* rainbow_mario                         */
    { 35, "Bowser: Dark World"  },  /* koopa1_map                            */
    { 36, "Bowser: Dark (boss)" },  /* koopa1_boss                           */
    { 37, "Bowser: Fire Sea"    },  /* koopa2_map                            */
    { 38, "Bowser: Fire (boss)" },  /* koopa2_boss                           */
    { 39, "Bowser in the Sky"   },  /* koopa3_map                            */
    { 40, "Bowser: Sky (boss)"  },  /* koopa3_boss                           */
    { 44, "Mario's Key Course"  },  /* ex_m_map                              */
    { 45, "Mario's Key Arena"   },  /* ex_mario                              */
    { 46, "Luigi's Key Course"  },  /* ex_l_map                              */
    { 47, "Luigi's Key Arena"   },  /* ex_luigi                              */
    { 48, "Wario's Key Course"  },  /* ex_w_map                              */
    { 49, "Wario's Key Arena"   },  /* ex_wario                              */
    { 50, "Rec Room"            },  /* playroom                              */
};
enum { LEVEL_NAME_COUNT = (int)(sizeof LEVEL_NAME / sizeof LEVEL_NAME[0]) };

/* The short name for a ROM level id, or null when the port has no name for it
   (the five non-stage ids). Null is a real answer: the row shows the number. */
static const char *level_short_name(int id)
{
    for (int i = 0; i < LEVEL_NAME_COUNT; ++i)
        if (LEVEL_NAME[i].id == id)
            return LEVEL_NAME[i].name;
    return 0;
}

/* THE ROW ORDER IS WIRED FIRST, AND IT IS COMPUTED RATHER THAN WRITTEN DOWN.
   MG_SCENE is in the ROM's id order, which scatters the handful of scenes the
   port can actually start across rows 6, 8, 14, 16, 18 and 30 of thirty, so
   walking the list from the front was two dozen refusals before the first
   thing that boots. mg_order below is every hosted id in id order followed by
   every unhosted id in id order. It is built out of port_scene_is_hosted and
   not out of a second hand-written table, so a scene seated next month sorts
   itself to the front with no edit here -- the same property that lights its
   row up in the first place.
   BUILT ONCE, LAZILY, for the reason the cursor below was already seated
   lazily: the registry is installed during the boot this file's statics are
   initialised before. Hosting does not change during a run, so once is enough
   and the order cannot shift under a cursor that is already pointing into it.
   EVERY USE GOES THROUGH mg_index(). menu_mg is a DISPLAY row from here on
   and never an MG_SCENE subscript -- the draw, the launch and the left/right
   step all resolve it the same way, so the id that starts is always the id
   the row was showing. An off-by-one between the two would recreate exactly
   the wrong-game confusion the titles above were fixed to end. */
static short mg_order[MG_COUNT];
static int mg_wired;             /* leading rows of mg_order that will boot */
static int mg_order_built;
static void mg_order_build(void)
{
    int i, n = 0;
    if (mg_order_built) return;
    for (i = 0; i < MG_COUNT; ++i)
        if (port_scene_is_hosted(MG_SCENE[i].id)) mg_order[n++] = (short)i;
    mg_wired = n;
    for (i = 0; i < MG_COUNT; ++i)
        if (!port_scene_is_hosted(MG_SCENE[i].id)) mg_order[n++] = (short)i;
    mg_order_built = 1;
}
/* the MG_SCENE subscript a display row is showing */
static int mg_index(int row) { mg_order_build(); return mg_order[row]; }
/* How far into its OWN group a display row sits, and how big that group is.
   These are the two numbers the row prints, and they are the whole separator
   the list needs: the count falls back to 1 of 24 on the same step the suffix
   turns into "not wired yet", so the end of the working set is unmissable
   without spending a character on a divider the row has no room for. */
static void mg_group(int row, int *pos, int *of)
{
    mg_order_build();
    if (row < mg_wired) { *pos = row + 1;            *of = mg_wired; }
    else                { *pos = row - mg_wired + 1; *of = MG_COUNT - mg_wired; }
}
/* The cursor starts at display row 0, which is the first thing that will
   actually boot because the order above put it there. Seated lazily and once,
   so stepping through the list survives closing the menu. */
static int menu_mg = -1;
static int mg_row(void)
{
    if (menu_mg < 0) { mg_order_build(); menu_mg = 0; }
    return menu_mg;
}

/* ---- WHAT SELECTING ONE DOES, AND WHY IT IS A RELAUNCH ------------------

   A LEVEL CANNOT REACH A MINIGAME IN PROCESS, and that is the ROM's rule
   rather than a gap in the port. port/ov006_minigame_scout.txt section 2
   derives it from the game's own loader: func_0201a694 unloads the current
   scene overlay before loading the new one and func_0201a754 unloads ov004
   along with ov006, and a level is executing out of ov002. Spawning a
   minigame id from inside a level would unload the overlay the caller is
   running in. The port has the same shape for the same reason: main hands
   over to hal/scene_boot.cpp's port_scene_run BEFORE the level bring-up (at
   the SM64DS_SCENE handover) and port_scene_run owns the rest of the process.

   So the picker does the one thing that is correct at both ends. It starts
   the program again on the scene path -- same exe, same working directory,
   same environment plus the two variables that path reads -- and this process
   leaves through its own window-close path, so the recorder and settings.json
   close the way they do on any other exit.

   THE CHILD IS NOT HEADLESS ANY MORE. What stood here said it was, and that
   it would stop being so when somebody gave the scene path a window: that
   happened (scene_window_run below, "port: a real window for the scene path,
   so a minigame can be played"), so the child now opens a real window and is
   played rather than captured, and this row is the warp it was written to
   become. The prediction is left visible rather than deleted because the
   shape it describes is still the shape: this row asks for a scene and the
   scene path decides what a scene run looks like, so it needed no edit when
   that answer changed and needs none when it changes again.

   SM64DS_DUAL_SCREEN=1 is stated explicitly even though port_scene_run
   already defaults a minigame to stacked off the same IsMinigameActorID: the
   ruling is that minigames always run stacked, and putting it in the child's
   environment makes that true whatever an inherited variable would otherwise
   have said. */
/* ---- THE ONE CLEAR LIST, FOR EVERY RELAUNCH (run link60, lane TCH2) --------
 *
 * The child of a menu row is a SESSION -- somebody pressed enter -- and
 * inheritance would let this process's own settings decide what it is instead.
 * That is the identical hole port/tools/battery.py's scene_env pops the same
 * list for, in its words: the caller's environment must not decide what the
 * code under test does.
 *
 * IT IS A TABLE BECAUSE THERE ARE TWO LAUNCH PATHS NOW. The minigame row
 * relaunches into a scene and the level row relaunches into a level, and two
 * hand-maintained copies of a list like this diverge on the first variable
 * somebody adds to one of them -- which is precisely how the four names at the
 * bottom of this table came to be missing from the copy that existed.
 *
 * WHAT EACH ONE COSTS IF IT IS INHERITED, because a list with no reasons is a
 * list nobody dares to prune:
 *   SCENE_FRAMES     hands the child a frame budget, and port_scene_want_window
 *                    then makes it headless, so the row looks broken
 *   SCENE_WINDOW     forces the opposite of whatever the child should be
 *   SCENE_NO_RENDER  a session that draws nothing
 *   SCENE_BMP/_STACKED  two processes writing one file is not a capture
 *   PAD_TEST         the WORST of them: the child replays the parent's script
 *                    from ITS frame 0, walks the same menu to the same row and
 *                    starts a grandchild, forever
 *   CLICK_TEST       the same trap with a mouse (lane TCH2's scripted stylus):
 *                    a click script that opens the menu and picks a row would
 *                    fork endlessly too
 *   WINDOW_SELFTEST  turns the child into a headless BMP run with no window,
 *                    which is not what pressing enter asked for
 *   SCENE_TRACE      measured by lane SWR1: a parent's SCENE_TRACE=1 put 875
 *                    trace lines in the child's playlog
 *   SCENE_SLOT9      lets an inherited skip decide what the child boots
 *   SCENE_SUBLEVEL   an input for the scenes that read it; inheriting one is
 *                    the harness fabricating an input
 * SM64DS_SCENE, SM64DS_LEVEL and SM64DS_DUAL_SCREEN are NOT in the table: they
 * are the destination, cleared and then set by the launcher below. */
static const char *const PORT_RELAUNCH_CLEAR[] = {
    "SM64DS_SCENE_FRAMES", "SM64DS_SCENE_WINDOW",  "SM64DS_SCENE_NO_RENDER",
    "SM64DS_SCENE_BMP",    "SM64DS_SCENE_BMP_STACKED", "SM64DS_PAD_TEST",
    /* the presented-image capture, for SCENE_BMP's reason exactly: two
       processes writing one file is not a capture; and the scripted menu,
       for SM64DS_PAD_TEST's reason -- an inherited one opens a menu in a
       child nobody asked to have one */
    "SM64DS_STACK_BMP",    "SM64DS_SCENE_MENU",
    /* AND THE RESULTS PROBE, which belongs here more than either of them and
       was missed on the round that added the other two. SM64DS_MG_RESULTS_PROBE
       does not print: it DISPATCHES the ROM's slot 27 on the live scene object,
       which raises the results panel, stops slot 24's display swap and halts the
       scene's behavior and render. An inherited one would do all of that to a
       child the picker started, and the child is the process a player is about
       to play. Same class as SM64DS_PAD_TEST and strictly more state-mutating.
       Unset -- which is every run but a lane's own -- the entry costs one
       string in a table the relaunch path walks and nothing else: the clear
       loop only ever removes names, so a variable that was never set is
       removed from an environment that never had it. */
    "SM64DS_MG_RESULTS_PROBE",
    "SM64DS_CLICK_TEST",   "SM64DS_WINDOW_SELFTEST", "SM64DS_SCENE_TRACE",
    "SM64DS_SCENE_SLOT9",  "SM64DS_SCENE_SUBLEVEL",
    /* the third injected-input knob; unlike the two above it has no selftest
       gate of its own, so an inherited one drives synthetic stylus presses
       into the child (review TCR1 measured it moving a selftest BMP). */
    "SM64DS_TOUCH_PROBE",
    /* run mg15 lane MP1. SM64DS_COMMS_FANOUT hands TouchInfo[4] and
       PadData[4] to the ROM's four-slot route instead of the port's direct
       writes -- the whole input path, in a child nobody asked to experiment
       on. SM64DS_COMMS_REPORT prints four lines a frame into the child's
       playlog. Same class as the two above. */
    "SM64DS_COMMS_FANOUT",  "SM64DS_COMMS_REPORT",
    /* run mg16 lane MP2. SM64DS_COMMS_ROLE is the strongest member of this
       whole list: an inherited one makes the child OPEN A SOCKET and try to
       join a session, and if it inherits the PARENT role from a parent that is
       still running it also fails a bind and prints a refusal nobody asked
       for. The other three are its arguments and travel with it -- an
       inherited PORT would aim the child at the wrong session and an inherited
       INJECT would put scripted stylus values on a wire the player is using.

       ONE DESTINATION OVERRIDES THIS, and only one: port_menu_relaunch_vs
       carries ROLE, PORT, SLOT and FANOUT back across the clear, because VS
       *is* the multiplayer mode and a VS child is the one child that is
       supposed to be in the session its parent was in. Its own banner has the
       argument, including why FANOUT belongs with the other three rather than
       with REPORT. The reasoning above is unchanged for the level and minigame
       destinations, which are the ones it was written about. */
    "SM64DS_COMMS_ROLE",    "SM64DS_COMMS_PORT",
    "SM64DS_COMMS_SLOT",    "SM64DS_COMMS_INJECT",
};

/* ONE RELAUNCH, TWO DESTINATIONS. `scene_id >= 0` starts the child on the
 * scene path (stacked, because the ruling is that minigames always run
 * stacked); otherwise `level_id` starts it on the level path, where the layout
 * is left to its own default -- a level is inset unless somebody says
 * otherwise, and a forced SM64DS_DUAL_SCREEN carried over from the minigame
 * that launched this process would be exactly that somebody. */
static int port_menu_relaunch(int scene_id, int level_id)
{
    char exe[MAX_PATH];
    char sid[16];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    if (!GetModuleFileNameA(0, exe, (DWORD)sizeof exe))
        return 0;
    /* the child inherits this process's block, so setting them here is how
       they reach it. This process is about to quit, so the mutation has no
       second reader. */
    for (unsigned i = 0; i < sizeof PORT_RELAUNCH_CLEAR /
                             sizeof *PORT_RELAUNCH_CLEAR; ++i)
        SetEnvironmentVariableA(PORT_RELAUNCH_CLEAR[i], 0);
    SetEnvironmentVariableA("SM64DS_SCENE", 0);
    SetEnvironmentVariableA("SM64DS_LEVEL", 0);
    SetEnvironmentVariableA("SM64DS_DUAL_SCREEN", 0);
    /* the VS destination pair travels with the destination class: cleared on
       every relaunch so a level or minigame child of a VS run does not boot
       back into VS, set only by the VS rows' own launcher below */
    SetEnvironmentVariableA("SM64DS_VS_MAP", 0);
    SetEnvironmentVariableA("SM64DS_VS_MODE", 0);
    if (scene_id >= 0) {
        snprintf(sid, sizeof sid, "%d", scene_id);
        SetEnvironmentVariableA("SM64DS_SCENE", sid);
        SetEnvironmentVariableA("SM64DS_DUAL_SCREEN", "1");
    } else {
        snprintf(sid, sizeof sid, "%d", level_id);
        SetEnvironmentVariableA("SM64DS_LEVEL", sid);
    }
    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    memset(&pi, 0, sizeof pi);
    /* bInheritHandles FALSE because there is nothing to hand down and a
       child should not hold duplicates of handles it never asked for. It is
       NOT what keeps the two playlogs apart, and saying it was would send the
       next reader looking in the wrong place: the child separates itself, by
       freopen'ing its own stderr onto its own timestamped playlog file within
       a few statements of entering main. Measured -- run both under
       SM64DS_NO_PLAYLOG=1, which is the switch that skips that freopen, and
       the two do interleave on the shared console whatever this flag says. */
    if (!CreateProcessA(exe, 0, 0, 0, FALSE, 0, 0, 0, &si, &pi))
        return 0;
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 1;
}

/* THE THIRD DESTINATION: a VS boot. Same launcher discipline as the two
   above -- the shared clear table, the destination class cleared and then
   set, no forced layout -- and the child does the rest: its boot hook (past
   the game-heap init) runs the ROM's own VS start off SM64DS_VS_MAP. The
   mode is NOT an input today because the ROM's VS has exactly one
   (port/slice_vs.txt section 4); when a second one ever exists this is where
   its env would be set.

   THE SESSION RIDES ALONG, AND THIS ONE IS THE EXCEPTION TO THE CLEAR TABLE.
   Run rel0215, lane vsnet. PORT_RELAUNCH_CLEAR strips SM64DS_COMMS_ROLE and
   _PORT because a MINIGAME or LEVEL child of a multiplayer run has no business
   inheriting a session it was not launched into -- read its own banner. A VS
   child is the opposite case and the only one: VS *is* the multiplayer mode,
   so a player who started the game with a role and a session code and then
   picks a VS map from the debug menu is asking for that session, and dropping
   it here is how the menu route into VS came to be the one route that could
   never pair. Carried by hand, after the shared clear, so the table itself
   keeps saying what it says for the other two destinations.

   FOUR ARE CARRIED AND TWO ARE NOT, deliberately.
   SM64DS_COMMS_RELAY / _CODE / _HOST / _BIND_ANY were never in the clear table
   and already survive. ROLE, PORT and SLOT are carried here because they name
   WHO THIS CONSOLE IS in the session.

   AND SM64DS_COMMS_FANOUT WITH THEM, which is the one that turns a paired
   session into a played one. It is not a diagnostic: walk_window.cpp:456 and
   hal/scene_boot.cpp:4460 make it the switch that hands TouchInfo[4] and
   PadData[4] to the ROM's four-slot route (func_0203bc7c) instead of the
   port's direct single-player writes.

   MEASURED, because "it would probably break" is not a reason to change a
   launcher. Two instances through the live relay with ROLE, RELAY and CODE set
   and FANOUT deliberately absent -- the exact environment a menu-relaunched VS
   child used to get:

       [comms:relay] paired as parent ... on both ends
       [comms:conductor] session up after 148 turns: ... players=2
       [vs] f599 count=2 me=0        the arena is a two-player arena
       [vs] f599 slot0 pad=0040      my own key, off the port's direct write
       [vs] f599 slot1 pad=0000      THE REMOTE PLAYER'S PAD IS EMPTY

   Every health indicator green and the remote player inert. That is the worst
   shape a defect can take, and it is why FANOUT belongs with the three names
   above rather than with the diagnostics below.

   WHY NO TEST CAUGHT IT, which is the part worth writing down: port/tools/
   vs_online_proof.py sets SM64DS_COMMS_FANOUT itself on both instances, so
   every proof this lane ran had it whatever this function did. The env-boot
   path a proof drives and the menu path a player drives differ exactly here.
   Found in review.

   TWO STAY CLEARED. SM64DS_COMMS_INJECT is test scaffolding that pins a held
   key, and the clear table's own note says an inherited one would put scripted
   values on a wire a player is using. SM64DS_COMMS_REPORT is pure logging --
   four lines a frame into the child's playlog -- so a player who never asked
   for it should not inherit it, and nothing about the session depends on it. */
static int port_menu_relaunch_vs(int vs_map)
{
    char exe[MAX_PATH];
    char sid[16];
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    if (!GetModuleFileNameA(0, exe, (DWORD)sizeof exe))
        return 0;
    /* read BEFORE the shared clear runs over them, written back after. One
       table rather than four named locals so a fifth name is one row, and so
       the log line below cannot drift out of step with what was carried. */
    static const char *const VS_CARRY[] = {
        "SM64DS_COMMS_ROLE", "SM64DS_COMMS_PORT",
        "SM64DS_COMMS_SLOT", "SM64DS_COMMS_FANOUT",
    };
    enum { VS_CARRY_N = sizeof VS_CARRY / sizeof *VS_CARRY };
    char carry[VS_CARRY_N][64];
    for (unsigned i = 0; i < VS_CARRY_N; ++i) {
        carry[i][0] = 0;
        GetEnvironmentVariableA(VS_CARRY[i], carry[i], sizeof carry[i]);
    }
    for (unsigned i = 0; i < sizeof PORT_RELAUNCH_CLEAR /
                             sizeof *PORT_RELAUNCH_CLEAR; ++i)
        SetEnvironmentVariableA(PORT_RELAUNCH_CLEAR[i], 0);
    for (unsigned i = 0; i < VS_CARRY_N; ++i)
        if (carry[i][0])
            SetEnvironmentVariableA(VS_CARRY[i], carry[i]);
    /* Says what was carried AND what was not, because "the session rode
       along" is the claim, and a role that arrived without a fan-out is the
       failure this line has to be able to show. */
    if (carry[0][0]) {
        fprintf(stderr, "[menu] VS relaunch carries the session:");
        for (unsigned i = 0; i < VS_CARRY_N; ++i)
            fprintf(stderr, " %s=%s", VS_CARRY[i],
                    carry[i][0] ? carry[i] : "(unset)");
        fprintf(stderr, "\n");
    }
    SetEnvironmentVariableA("SM64DS_SCENE", 0);
    SetEnvironmentVariableA("SM64DS_LEVEL", 0);
    SetEnvironmentVariableA("SM64DS_DUAL_SCREEN", 0);
    SetEnvironmentVariableA("SM64DS_VS_MODE", 0);
    snprintf(sid, sizeof sid, "%d", vs_map & 3);
    SetEnvironmentVariableA("SM64DS_VS_MAP", sid);
    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    memset(&pi, 0, sizeof pi);
    if (!CreateProcessA(exe, 0, 0, 0, FALSE, 0, 0, 0, &si, &pi))
        return 0;
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 1;
}

/* ---- WHAT LOOP IS THE MENU RUNNING INSIDE? (port mod, run link60 SW1) -----

   The debug menu is one block of code with two callers now: main's level loop
   and scene_window_run's. Four of its rows act on a Player the level spawned
   and a scene has not got, so rather than two menus there is one menu and this
   record of what is underneath it.

   A LEVEL FILLS ALL THREE FIELDS AND A SCENE FILLS NONE. The rows that need a
   Player then say "(level only)" while the menu is drawn and refuse in words
   when they are pressed, with the menu still open and the game still running.
   That is the shape DBG1 gave an unhosted minigame id, for the same reason: a
   debug menu that silently does nothing is worse than one that says why. */
struct MenuHost {
    char *player;       /* the Player actor, or null in a scene */
    void *cam;          /* the Camera actor, or null */
    int   real_camera;  /* is there a game camera at all */
};
static MenuHost g_menu_host;

static void menu_draw(const OvlSurface &fb)
{
    /* 96, not 72: the level-select row now carries a name as well as the row,
       id, entrance, overlay and mount state, and at 72 the unmounted form of
       the longest-named row truncated silently (the 86-into-71 defect the row
       code below used to carry a note about). 96 holds the widest row whole --
       measured worst case is 77, the minigame row under its longest title --
       and the panel still auto-sizes to the longest string, so widening the
       buffer does not widen the panel unless a row actually needs it. */
    char ln[MENU_COUNT][96];
    int i, w = 0, x0, y0;
    int ex = 0, ey = 0, ez = 0, eyaw = 0;
    const int n_ent = port_entrance_count();
    const int have = port_entrance_record(menu_entrance, &ex, &ey, &ez, &eyaw);
    /* Esc named FIRST of the four closers, because it is the one a player
       arrives with: it is now what opened this menu in most sessions, and the
       row has to say so or the key that got them here looks like it has no way
       back. */
    const char *title = "DEBUG MENU   Esc/F5/BACK/B close   "
                        "WASD or arrows or d-pad move   enter/right/A act";

    if (have)
        snprintf(ln[MENU_WARP], sizeof ln[0],
                 "warp to entrance  %d of %d   (%d %d %d)", menu_entrance,
                 n_ent, ex, ey, ez);
    else
        snprintf(ln[MENU_WARP], sizeof ln[0],
                 "warp to entrance  none loaded");
    {
        int lv = 0, en = 0;
        const int real = port_title_row(menu_level_row, &lv, &en);
        if (!real) {
            /* the two scene sentinels the ROM's own table carries: -1 is the
               file select, -2 the minigame menu (hal/level_change.cpp). Name
               them rather than print "a scene, not a level". */
            const char *sc = lv == -1 ? "back to file select"
                           : lv == -2 ? "minigame menu"
                                      : 0;
            if (sc)
                snprintf(ln[MENU_LEVEL], sizeof ln[0],
                         "level select  row %2d/%d  %s",
                         menu_level_row, port_title_rows(), sc);
            else
                snprintf(ln[MENU_LEVEL], sizeof ln[0],
                         "level select  row %2d/%d  scene sentinel %d",
                         menu_level_row, port_title_rows(), lv);
        } else {
            const char *nm = level_short_name(lv);
            char idn[24];
            if (!nm) { snprintf(idn, sizeof idn, "(level %d)", lv); nm = idn; }
            snprintf(ln[MENU_LEVEL], sizeof ln[0],
                     "level select  row %2d/%d  %2d %s  ent %d ov%03d %s",
                     menu_level_row, port_title_rows(), lv, nm, en,
                     port_level_overlay_id(lv),
                     port_level_is_mounted(lv) ? "MOUNTED" : "not mounted");
        }
    }
    {
        /* This row's suffix ("enter restarts, stacked" = 23, "not wired yet"
           = 13) used to have only 26 characters to live in, because ln was
           [72] and the fixed part of this row costs 45. The first draft asked
           for 28 and drew as "...here, stack" -- snprintf truncated it and
           said nothing, which is the whole failure mode. ln is now [96] (lane
           DBGNAME widened it for the level-select name; see the buffer decl),
           so this row has ample room and the panel still auto-sizes to the
           longest string.
           THE TITLE FIELD IS PADDED TO 21 AND THE TITLES ARE CAPPED AT 21, so
           this row is 77 characters for every one of the thirty and the panel
           does not breathe in and out as the list is walked. 77 characters is
           462 pixels of the 512 the framebuffer has; see the width paragraph
           over MG_SCENE for where the ceiling comes from.
           THE TWO COUNTERS ARE GROUP-RELATIVE, not 1..30. With the list sorted
           wired-first they read "N of 6" through the scenes that start and
           then fall back to "1 of 24" on the first one that does not, which is
           the only end-of-working-set marker a single-line row has room for
           and it lands on the same step as the suffix change. */
        const int r = mg_row();
        const int mi = mg_index(r);
        int pos, of;
        mg_group(r, &pos, &of);
        snprintf(ln[MENU_MINIGAME], sizeof ln[0],
                 "minigame          %d %-21s %2d of %-2d  %s",
                 MG_SCENE[mi].id, MG_SCENE[mi].name, pos, of,
                 port_scene_is_hosted(MG_SCENE[mi].id)
                     ? "enter restarts, stacked"
                     : "not wired yet");
    }
    {
        /* the VS rows. The map line carries the ROM facts (list position,
           level id, overlay, mount state) the way the level row does; the
           mode line states the ROM's one mode. Both act on enter. */
        const int vlv = port_vs_map_level(menu_vs_map);
        snprintf(ln[MENU_VS_MAP], sizeof ln[0],
                 "vs map            %d of 4   level %2d ov%03d %s",
                 menu_vs_map + 1, vlv, port_level_overlay_id(vlv),
                 port_level_is_mounted(vlv) ? "enter starts VS"
                                            : "NOT MOUNTED");
        snprintf(ln[MENU_VS_MODE], sizeof ln[0],
                 "vs mode           star battle (the ROM's only mode)   "
                 "enter starts VS");
    }
    snprintf(ln[MENU_EXIT], sizeof ln[0],
             "exit course       ExitLevel() -> level 1 entrance 13   "
             "(here: level %d)", (int)data_0209f2f8);
    snprintf(ln[MENU_CHARACTER], sizeof ln[0], "character         %s%s",
             CHAR_NAME[g_character_pending & 3],
             g_character_pending == g_character ? "" : "   enter to switch");
    snprintf(ln[MENU_SNAP], sizeof ln[0], "fake snap         %s",
             g_fake_snap ? "ON (collider owner set at boot)" : "off");
    snprintf(ln[MENU_OVERLAY], sizeof ln[0], "stats overlay     %s",
             g_overlay_on ? "on" : "off");
    snprintf(ln[MENU_CAMERA], sizeof ln[0], "camera            %s",
             cam_mode_name(cam_mode));
    snprintf(ln[MENU_RUNMODE], sizeof ln[0], "run mode          %s",
             RUN_MODE_NAME[g_run_mode]);
    /* the binding row shows BOTH devices, because it can set either one and a
       player who has only ever rebound the keyboard should still be able to
       see what the pad is doing */
    {
        char kb[16], pb[16];
        if (g_rebind_capture)
            snprintf(ln[MENU_RUNBIND], sizeof ln[0],
                     "rebind run        press the new key or pad button "
                     "(esc cancels)");
        else
            snprintf(ln[MENU_RUNBIND], sizeof ln[0],
                     "rebind run        %s / %s   enter to rebind",
                     run_key_name(g_run_key, kb, sizeof kb),
                     run_pad_name(g_run_pad, pb, sizeof pb));
    }
    snprintf(ln[MENU_RECORDER], sizeof ln[0], "recorder          %s", g_playlog);
    /* the disk suffix tells the player whether a save will outlive the run: it
       does only when the arena is at its fixed base, which is what lets a disk
       state's pointers relocate on the next launch (hal/lk7_persist.cpp). */
    snprintf(ln[MENU_SAVESTATE], sizeof ln[0], "save state        F8   %s%s",
             lk6_savestate_has() ? "(slot in use, overwrite)" : "(slot empty)",
             lk7_persist_available() ? " to disk" : " this run only");
    snprintf(ln[MENU_LOADSTATE], sizeof ln[0], "load state        F9   %s",
             lk6_savestate_has() ? "(restore slot)" : "(no state saved)");

    /* THE FOUR ROWS A SCENE HAS NOTHING TO ACT ON, said before they are
       pressed. Everything above is written for a level and reads level state;
       in a scene those readings are true and useless (no entrances, a level id
       nothing is standing in), so the rows are replaced rather than annotated
       -- the lines above are already close to the 72-column buffer and a
       suffix would truncate the sentence instead of adding to it. The minigame
       row is deliberately NOT in this set: it works from a scene. */
    if (!g_menu_host.player) {
        snprintf(ln[MENU_WARP], sizeof ln[0],
                 "warp to entrance  (level only)");
        snprintf(ln[MENU_EXIT], sizeof ln[0],
                 "exit course       (level only)");
        snprintf(ln[MENU_CHARACTER], sizeof ln[0],
                 "character         (level only)");
        /* THE LEVEL ROW IS NOT "(level only)" ANY MORE (lane TCH2): from a
           scene it relaunches, which is the way out of a minigame. It still
           cannot show the level-path detail the version above shows, so it
           says the level id, its name and whether it will boot, and no more.
           THE 86-INTO-71 TRUNCATION THE OLD NOTE HERE FLAGGED IS FIXED (lane
           DBGNAME): the mounted-level row above overflowed [72] on an
           unmounted, long-named level, and ln is now [96], so every form of
           both rows -- name included -- fits whole. The name lookup is
           level_short_name(); an id with no name falls back to "(level N)". */
        int lv = 0, en = 0;
        if (!port_title_row(menu_level_row, &lv, &en)) {
            const char *sc = lv == -1 ? "back to file select"
                           : lv == -2 ? "minigame menu"
                                      : 0;
            if (sc)
                snprintf(ln[MENU_LEVEL], sizeof ln[0],
                         "level select  row %2d/%d  %s",
                         menu_level_row, port_title_rows(), sc);
            else
                snprintf(ln[MENU_LEVEL], sizeof ln[0],
                         "level select  row %2d/%d  scene sentinel %d",
                         menu_level_row, port_title_rows(), lv);
        } else {
            const char *nm = level_short_name(lv);
            char idn[24];
            if (!nm) { snprintf(idn, sizeof idn, "(level %d)", lv); nm = idn; }
            snprintf(ln[MENU_LEVEL], sizeof ln[0],
                     "level select  row %2d/%d  %2d %s  %s",
                     menu_level_row, port_title_rows(), lv, nm,
                     port_level_is_mounted(lv) ? "enter restarts"
                                               : "NOT MOUNTED");
        }
    }

    for (i = 0; i < MENU_COUNT; ++i) {
        const int lw = (int)strlen(ln[i]) * OVL_ADVANCE * OVL_SCALE;
        if (lw > w) w = lw;
    }
    {
        const int tw = (int)strlen(title) * OVL_ADVANCE * OVL_SCALE;
        if (tw > w) w = tw;
    }
    w += 3 * OVL_ADVANCE * OVL_SCALE;
    x0 = (ntr::SCREEN_W - w) / 2;
    if (x0 < 2) x0 = 2;
    y0 = (ntr::SCREEN_H - (MENU_COUNT + 2) * OVL_LINE) / 2;
    ovl_shade(fb, x0 - 4, y0 - 4, w + 8, (MENU_COUNT + 2) * OVL_LINE + 8);
    ovl_shade(fb, x0 - 4, y0 - 4, w + 8, (MENU_COUNT + 2) * OVL_LINE + 8);
    ovl_text(fb, x0, y0, title, 0xFF80C0FFu);
    for (i = 0; i < MENU_COUNT; ++i) {
        const int y = y0 + (i + 2) * OVL_LINE;
        const int sel = i == menu_sel;
        if (sel) ovl_text(fb, x0, y, ">", 0xFFFFE060u);
        ovl_text(fb, x0 + 2 * OVL_ADVANCE * OVL_SCALE, y, ln[i],
                 sel ? 0xFFFFE060u : 0xFFB0B0B0u);
    }
}

/* ---- THE DEBUG MENU'S OWN INPUT. It runs before anything else reads
   the keyboard, and while it is open it swallows the keys it uses and
   the tick is skipped below, so nothing it does can also be a walk.
   Every key here is edge-detected off one held-mask, which is the
   cheapest way to get "one step per press" out of GetAsyncKeyState.

   MOVED TO FILE SCOPE (run link60 lane SW1) so the windowed scene loop runs
   the SAME menu rather than a second one. What it used to read off main's
   stack -- the Player, the Camera, whether there is a game camera at all --
   is g_menu_host above, filled by whichever loop is running, and every row
   that needs one of those refuses in words when a scene is underneath it.
   Nothing else about the block changed. */
static void menu_input(int pad_live, const XPad *pad)
{
    if (g_selftest) return;
    static unsigned menu_prev;
    unsigned held = 0;
    unsigned edge;
    /* through key_live, not the raw read, so the menu is behind the
       focus gate and the stale-key latch with everything else. Under a
       selftest this block never runs at all, so routing it here changes
       nothing an automated run sees. */
    /* ESCAPE IS AN ALIAS OF F5 (Tango's ask, run mg10 lane ESC), and this line
       is the whole of it. Escape used to close the game outright, from the
       window procedure -- the one key in this program that acted without
       passing the three gates key_live carries. Quitting by accident is the
       cheapest bug a play session has: the F-row keys sit next to each other
       and the one a person reaches for to back out of anything took the
       session with it.

       AN ALIAS AND NOT A SECOND ENTRY POINT, on purpose. Sharing bit 0 with F5
       and pad BACK means escape inherits, with no code of its own, every rule
       the toggle already follows: the selftest gate (a comparator run reads
       every key released, so nothing an automated run does can press this), the
       rebind-capture gate, the focus gate, the stale-key latch across an
       alt-tab, and the one-step-per-press edge. It also means escape CLOSES the
       menu as well as opening it, which is the half of the ask that matters --
       the key that opens the picker is the key that puts it away.

       QUITTING IS THE WINDOW'S CLOSE BUTTON now, and alt+F4, both of which
       arrive as WM_DESTROY and leave through the same PostQuitMessage escape
       always did. Nothing else in this file quits: the only other
       PostQuitMessage calls are that one and the two relaunch rows, which quit
       because they have just started a replacement process. */
    if (key_live(VK_F5) || key_live(VK_ESCAPE)) held |= 1u << 0;
    /* WASD NAVIGATES TOO, as plain aliases of the arrows (Tango's ask). Same
       key_live call, so they arrive behind the focus gate, the stale-key latch
       and the rebind-capture gate with everything else; same held-mask bit, so
       they inherit the one-step-per-press edge and the held-across-an-open
       discipline the pad buttons were fixed to follow. A player already
       steering Mario with the left hand should not have to move it to read
       this menu.

       AND THEY NEED NO SWALLOW, which is worth stating because the B close did
       need one. These four keys feed exactly one thing in the game -- dx/dz,
       built out of `key_live('W') || key_live(VK_UP)` and its three siblings
       -- and that pair is already emptied while the menu is open: the level
       loop does it outright (`if (menu_on) { dx = 0; dz = 0; }`) and the
       windowed scene loop wraps its whole button build in `if (!menu_on)`.
       The zeroing is on the VARIABLE, not on the key, so it has always
       covered both halves of each of these four lines. The arrows were never
       leaking and WASD does not start. */
    if (key_live(VK_UP)    || key_live('W')) held |= 1u << 1;
    if (key_live(VK_DOWN)  || key_live('S')) held |= 1u << 2;
    if (key_live(VK_LEFT)  || key_live('A')) held |= 1u << 3;
    if (key_live(VK_RIGHT) || key_live('D')) held |= 1u << 4;
    if (key_live(VK_RETURN)) held |= 1u << 5;
    if (pad_live) {
        if (pad->buttons & 0x0001) held |= 1u << 1;   /* d-pad up    */
        if (pad->buttons & 0x0002) held |= 1u << 2;   /* d-pad down  */
        if (pad->buttons & 0x0004) held |= 1u << 3;   /* d-pad left  */
        if (pad->buttons & 0x0008) held |= 1u << 4;   /* d-pad right */
        if (pad->buttons & 0x0020) held |= 1u << 0;   /* BACK        */
        /* A acts and B closes, and BOTH ARE RECORDED WHETHER THE MENU
           IS OPEN OR NOT. That is the fix for a real bug and not
           tidying: held is only ever a record of what is physically
           down, and menu_on decides what is DONE about it, below.
           Gating the record on menu_on instead meant a button held
           across an open read as a fresh press the instant the menu
           appeared -- hold B, tap BACK, and the menu opened and shut
           in the same breath, so it could not be opened from the pad
           at all with B down. A had the same trap the other way:
           opening with A held instantly confirmed whatever row the
           cursor was on. Recording the state unconditionally means
           menu_prev already carries the bit when the menu opens, so
           there is no edge until the button is genuinely released and
           pressed again. Neither bit is read anywhere except inside
           the `if (menu_on)` below, so recording them always costs
           nothing and does nothing on its own. */
        if (pad->buttons & 0x1000) held |= 1u << 5;   /* A  act      */
        if (pad->buttons & 0x2000) held |= 1u << 6;   /* B  close    */
    }
    edge = held & ~menu_prev;
    menu_prev = held;
    if (edge & (1u << 0)) {
        menu_on = !menu_on;
        fprintf(stderr, "[menu] %s\n", menu_on ? "open" : "closed");
        /* Park the level row somewhere USEFUL on open: the row
           resets to 1 each boot, and 1 is the castle grounds, so
           "open the menu, press enter" re-entered the level being
           stood in -- which reads as a warp that did nothing (the
           2026-08-05 session). Seat it on the first mounted row
           whose level is not the current one; navigation from
           there is unchanged. */
        if (menu_on) {
            int lv = 0, en = 0;
            if (port_title_row(menu_level_row, &lv, &en) &&
                lv == (int)data_0209f2f8) {
                const int rows = port_title_rows();
                for (int k = 1; k < rows; ++k) {
                    const int r = (menu_level_row + k) % rows;
                    if (port_title_row(r, &lv, &en) &&
                        port_level_is_mounted(lv) &&
                        lv != (int)data_0209f2f8) {
                        menu_level_row = r;
                        break;
                    }
                }
            }
        }
    }
    /* B closes it. Read after the toggle above and before the rows
       below, so a close is a close whatever else the frame carried
       and no row sees the press that shut the menu. */
    if (menu_on && (edge & (1u << 6))) {
        menu_on = 0;
        menu_b_swallow = 1;
        fprintf(stderr, "[menu] closed\n");
    }
    if (menu_on) {
        const int n_ent = port_entrance_count();
        if (edge & (1u << 1))
            menu_sel = (menu_sel + MENU_COUNT - 1) % MENU_COUNT;
        if (edge & (1u << 2))
            menu_sel = (menu_sel + 1) % MENU_COUNT;
        {
            /* enter is a synonym for right, so a pad can do it all */
            const int dec = (edge & (1u << 3)) != 0;
            const int inc = (edge & ((1u << 4) | (1u << 5))) != 0;
            /* THE THREE ROWS THAT NEED A LEVEL, refused in one place rather
               than three. Warp writes the Player's position, exit calls
               ExitLevel, and character swaps the Player's model -- so from a
               scene each of them is a write through a null. The menu stays
               open and the scene keeps running, which is the shape DBG1 gave
               an unhosted minigame id. The MINIGAME row is deliberately not in
               the set: it relaunches, and a relaunch works from a scene
               exactly as well as from a level. */
            /* MENU_LEVEL LEFT THIS SET (run link60, lane TCH2). It was in it
               for a reason that stopped being true when the minigame row got a
               relaunch: "level select stages a change only main's loop polls",
               so from a scene it was a request nobody would read. But there is
               nothing about CHOOSING A LEVEL that needs a Player in this
               process -- the other three genuinely do, they write through
               g_menu_host.player or call ExitLevel -- and a relaunch works from
               a scene exactly as well as it works from a level. Leaving it here
               meant that once you were inside a minigame the ONLY way back to
               the game was closing the window, which is what the owner hit.
               The refusal below still covers the three that need a live
               Player, and the enter branch for this row splits on the same
               g_menu_host.player: stage the handoff in a level, relaunch from
               a scene. */
            if ((dec || inc) && !g_menu_host.player &&
                (menu_sel == MENU_WARP ||
                 menu_sel == MENU_EXIT || menu_sel == MENU_CHARACTER)) {
                ss_note("that row needs a level (this is a scene)");
                fprintf(stderr, "[menu] row %d needs a level and this run is a "
                        "scene -- refused, menu stays open\n", menu_sel);
            } else if (dec || inc) switch (menu_sel) {
            case MENU_WARP:
                if (n_ent > 0) {
                    /* left and right pick the entrance; enter warps */
                    if (edge & (1u << 5)) {
                        int ex, ey, ez, eyaw;
                        if (port_entrance_record(menu_entrance, &ex,
                                                 &ey, &ez, &eyaw)) {
                            /* the level's own record, in the units it
                               stores: world units, so <<12 into the
                               actor's fixed point. This MOVES him; it
                               is not a re-entry, nothing about the
                               level or the area is reloaded. */
                            *(int *)(g_menu_host.player + 0x5c) = ex << 12;
                            *(int *)(g_menu_host.player + 0x60) = ey << 12;
                            *(int *)(g_menu_host.player + 0x64) = ez << 12;
                            *(short *)(g_menu_host.player + 0x8e) = (short)eyaw;
                            *(int *)(g_menu_host.player + 0x98) = 0;   /* mHorzSpeed */
                            *(int *)(g_menu_host.player + 0xa4) = 0;
                            *(int *)(g_menu_host.player + 0xa8) = 0;   /* mVertSpeed */
                            *(int *)(g_menu_host.player + 0xac) = 0;
                            an_pivot_live = 0;        /* do not ease
                                                         across a warp */
                            fprintf(stderr,
                                    "[menu] warp to entrance %d "
                                    "(%d, %d, %d)\n", menu_entrance,
                                    ex, ey, ez);
                        }
                    } else if (dec) {
                        menu_entrance =
                            (menu_entrance + n_ent - 1) % n_ent;
                    } else {
                        menu_entrance = (menu_entrance + 1) % n_ent;
                    }
                }
                break;
            case MENU_LEVEL:
                /* left/right move the cursor exactly as
                   func_ov003_020ad814 does (+/-1, modulo the row
                   count); enter runs its else-branch. */
                if ((edge & (1u << 5)) && !g_menu_host.player) {
                    /* FROM A SCENE: RELAUNCH (run link60, lane TCH2). There is
                       no level loop in this process to hand a staged change
                       to, and port_title_select's ROM branch writes level
                       globals a scene has no use for, so the honest move is
                       the one the minigame row already makes -- start the
                       program again on the level path and leave through this
                       process's ordinary close. Same exe, same directory, the
                       shared clear table, no forced layout.
                       The row's own sentinel check comes first: rows -1 and -2
                       are scenes, not levels, and relaunching into one would
                       boot the castle grounds instead of saying why. */
                    int lv = 0, en = 0;
                    if (!port_title_row(menu_level_row, &lv, &en)) {
                        ss_note("that row is a scene, not a level");
                        fprintf(stderr, "[menu] level row %d is a scene "
                                "sentinel (%d), not a level -- refused, menu "
                                "stays open\n", menu_level_row, lv);
                    } else if (!port_level_is_mounted(lv)) {
                        /* REFUSED BEFORE THE PARENT COMMITS, and this is the
                           one refusal that has to happen HERE rather than in
                           the child. The relaunch is a one-way door: this
                           process quits the moment the child is started, so an
                           unmounted level would abort in the child and leave
                           the player with no game at all -- worse than the
                           dead end this row exists to open. The level path's
                           own select refuses an unmounted row too
                           (port_title_select, after the ROM branch); it can
                           afford to do it late because nothing has quit. */
                        char msg[64];
                        snprintf(msg, sizeof msg,
                                 "level %d is not mounted in this build", lv);
                        ss_note(msg);
                        fprintf(stderr, "[menu] level %d (row %d) is NOT "
                                "mounted -- refused, menu stays open (a "
                                "relaunch would quit this run and the child "
                                "would abort)\n", lv, menu_level_row);
                    } else if (port_menu_relaunch(-1, lv)) {
                        fprintf(stderr, "[menu] level %d (row %d): started "
                                "SM64DS_LEVEL=%d, this process is quitting\n",
                                lv, menu_level_row, lv);
                        W.PostQuitMessage_(0);
                    } else {
                        char msg[64];
                        snprintf(msg, sizeof msg,
                                 "could not start level %d", lv);
                        ss_note(msg);
                        fprintf(stderr, "[menu] level %d: could not start the "
                                "level run (win32 %lu)\n", lv,
                                (unsigned long)GetLastError());
                    }
                } else if (edge & (1u << 5)) {
                    /* a successful select CLOSES THE MENU: the level
                       handoff poll below is gated on !menu_on (every
                       pointer the menu holds goes stale across it),
                       so staying open would park the warp forever --
                       which is exactly what it did. */
                    if (port_title_select(menu_level_row)) {
                        menu_on = 0;
                        /* the staged request, read straight back, so
                           the playlog shows what the poll is about
                           to see -- the 18:56 session ended at the
                           castle grounds with no way to tell whether
                           the request died staged or mid-handoff */
                        fprintf(stderr, "[menu] closed for the level "
                                "handoff (staged: level %d entrance "
                                "%d)\n", (int)data_02092110,
                                (int)data_0209f268);
                    }
                } else if (dec) {
                    menu_level_row = (menu_level_row +
                                      port_title_rows() - 1) %
                                     port_title_rows();
                } else {
                    menu_level_row =
                        (menu_level_row + 1) % port_title_rows();
                }
                break;
            case MENU_MINIGAME:
                /* left/right walk the ROM's thirty; enter starts the
                   one that is showing, or says why it cannot and
                   leaves the menu exactly where it was. */
                if (edge & (1u << 5)) {
                    const int r = mg_row();
                    const int mi = mg_index(r);
                    const int id = MG_SCENE[mi].id;
                    if (!port_scene_is_hosted(id)) {
                        char msg[64];
                        snprintf(msg, sizeof msg,
                                 "%s (%d) is not wired yet",
                                 MG_SCENE[mi].name, id);
                        ss_note(msg);
                        fprintf(stderr, "[menu] minigame %d (%s) is "
                                "not a hosted scene yet -- refused, "
                                "menu stays open\n", id,
                                MG_SCENE[mi].name);
                    } else if (port_menu_relaunch(id, -1)) {
                        fprintf(stderr, "[menu] minigame %d (%s): "
                                "started SM64DS_SCENE=%d "
                                "SM64DS_DUAL_SCREEN=1, this process "
                                "is quitting\n", id, MG_SCENE[mi].name,
                                id);
                        /* the same exit a window close takes: the
                           next PeekMessage returns WM_QUIT */
                        W.PostQuitMessage_(0);
                    } else {
                        char msg[64];
                        snprintf(msg, sizeof msg,
                                 "could not start scene %d", id);
                        ss_note(msg);
                        fprintf(stderr, "[menu] minigame %d: could "
                                "not start the scene run (win32 %lu)"
                                "\n", id,
                                (unsigned long)GetLastError());
                    }
                } else if (dec) {
                    menu_mg = (mg_row() + MG_COUNT - 1) % MG_COUNT;
                } else {
                    menu_mg = (mg_row() + 1) % MG_COUNT;
                }
                break;
            case MENU_VS_MAP:
            case MENU_VS_MODE:
                /* left/right walk the ROM's four maps (the map row only);
                   enter on either row starts VS on the showing map -- the
                   minigame row's relaunch shape, and the same refusal
                   discipline: an unmounted map is refused in words BEFORE
                   the parent commits, because the relaunch is a one-way
                   door. The mode row moves nothing: the ROM's VS has ONE
                   mode (port/slice_vs.txt section 4) and a selector with
                   one entry would be an invented choice. */
                if (edge & (1u << 5)) {
                    const int vlv = port_vs_map_level(menu_vs_map);
                    if (!port_level_is_mounted(vlv)) {
                        char msg[64];
                        snprintf(msg, sizeof msg,
                                 "VS map %d (level %d) is not mounted",
                                 menu_vs_map + 1, vlv);
                        ss_note(msg);
                        fprintf(stderr, "[menu] VS map %d is level %d "
                                "(overlay %d), NOT mounted -- refused, menu "
                                "stays open\n", menu_vs_map + 1, vlv,
                                port_level_overlay_id(vlv));
                    } else if (port_menu_relaunch_vs(menu_vs_map)) {
                        fprintf(stderr, "[menu] VS map %d (level %d): "
                                "started SM64DS_VS_MAP=%d, this process is "
                                "quitting\n", menu_vs_map + 1, vlv,
                                menu_vs_map);
                        W.PostQuitMessage_(0);
                    } else {
                        char msg[64];
                        snprintf(msg, sizeof msg,
                                 "could not start VS map %d", menu_vs_map + 1);
                        ss_note(msg);
                        fprintf(stderr, "[menu] VS map %d: could not start "
                                "the VS run (win32 %lu)\n", menu_vs_map + 1,
                                (unsigned long)GetLastError());
                    }
                } else if (menu_sel == MENU_VS_MAP) {
                    if (dec)
                        menu_vs_map = (menu_vs_map + 3) & 3;
                    else
                        menu_vs_map = (menu_vs_map + 1) & 3;
                }
                break;
            case MENU_EXIT:
                if (edge & (1u << 5)) {
                    fprintf(stderr, "[menu] ExitLevel()\n");
                    ExitLevel();
                }
                break;
            case MENU_CHARACTER:
                /* left and right pick, enter changes -- the same shape
                   as the warp row above */
                if (edge & (1u << 5)) {
                    fprintf(stderr, "[menu] becoming %s\n",
                            CHAR_NAME[g_character_pending & 3]);
                    port_player_set_character(g_menu_host.player,
                                              g_character_pending);
                    g_character = g_character_pending;
                    an_pivot_live = 0;   /* do not ease across it */
                } else {
                    g_character_pending =
                        (dec ? g_character_pending + 3
                             : g_character_pending + 1) & 3;
                }
                break;
            case MENU_SNAP:
                g_fake_snap = !g_fake_snap;
                fprintf(stderr, "[menu] fake snap %s\n",
                        g_fake_snap ? "ON" : "off");
                break;
            case MENU_OVERLAY:
                g_overlay_on = !g_overlay_on;
                break;
            case MENU_CAMERA:
                if (g_menu_host.real_camera) {
                    cam_mode = dec ? (cam_mode + 2) % 3
                                   : (cam_mode + 1) % 3;
                    if (cam_mode != CAM_DS) fc_seed(g_menu_host.cam);
                    if (cam_mode == CAM_ANALOG) an_pivot_live = 0;
                    fprintf(stderr, "[menu] camera %s\n",
                            cam_mode_name(cam_mode));
                }
                break;
            case MENU_RUNMODE:
                /* left and right cycle it and enter acts as right, the
                   same shape as the camera row. Persisted on the spot
                   rather than at exit: this program is one a player
                   does sometimes crash, and a preference that only
                   survives a clean shutdown is a preference that gets
                   lost exactly when they were experimenting. */
                g_run_mode = dec ? (g_run_mode + RUN_MODE_COUNT - 1) %
                                       RUN_MODE_COUNT
                                 : (g_run_mode + 1) % RUN_MODE_COUNT;
                host_setting_save_run(g_run_mode, g_run_key, g_run_pad);
                fprintf(stderr, "[run] mode %s\n",
                        RUN_MODE_NAME[g_run_mode]);
                break;
            case MENU_RUNBIND:
                /* enter/right only: arm the capture. Left does nothing
                   on purpose -- there is no "previous binding" to walk
                   back to, and a row that changed a binding by being
                   scrolled past would be a trap. */
                if (edge & (1u << 5)) {
                    g_rebind_capture = 1;
                    g_rebind_key = 0;
                    /* enter is still physically down. Marking every
                       key stale here is what stops it being read as
                       the binding on the very next frame; the window
                       procedure ignores its auto-repeat for the same
                       reason. */
                    memset(key_stale, 1, sizeof key_stale);
                    ss_note("press the new run button (esc cancels)");
                }
                break;
            case MENU_SAVESTATE:
                /* enter/right only: snapshot into the slot. Same call
                   F8 makes; the menu pauses the tick, which is as safe
                   a between-frames point as the top-of-loop latch.
                   The toast fires here too: highlighting the row and
                   closing the menu does NOT save, and the only way a
                   player can learn that is being shown the difference. */
                if (edge & (1u << 5)) {
                    if (lk6_savestate_save())
                        ss_note(lk7_persist_write()
                                    ? "state saved to disk (F9 loads it)"
                                    : "state saved for THIS RUN (F9 loads it)");
                    else
                        ss_note("state NOT saved (see log)");
                }
                break;
            case MENU_LOADSTATE:
                /* enter/right only: restore the slot. A no-op with no
                   saved state. */
                if (edge & (1u << 5)) {
                    if (lk6_savestate_load()) {
                        an_pivot_live = 0;
                        ss_reseat_pending = 1;
                        ss_note("state loaded");
                    } else {
                        ss_note(lk6_savestate_has()
                                    ? "state NOT loaded (see log)"
                                    : "no state saved yet (F8 saves)");
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}

/* THE CLOSE MUST NOT ALSO BE A PUNCH. B shuts the menu while it is
   open and punches while it is not, so the frame the close lands on
   has the button still down with the menu already shut -- and a thumb
   holds it for several frames after that. Swallowed from the close
   until it comes back up, which is the pad's copy of the key_stale
   latch the focus edge sets, for the same reason: a press already
   spent on one thing must not be spent again on another.
   Here rather than inside the block above because this has to run on
   the frames AFTER the close, when the menu is shut and that block is
   no longer looking at B.

   A HAS THE SAME OVERLAP ON THE LEVEL-SELECT ROW AND IT IS NOT FIXED
   HERE. PRE-EXISTING, measured, written down so the next person does
   not have to find it twice: MENU_LEVEL's enter branch sets
   menu_on = 0 on a successful select, and A is the jump button with
   the menu shut, so the A that picks a level also jumps on the
   handoff frame. It wants the same latch this one gets. It is left
   alone deliberately -- it sits on the level handoff, which is
   somebody else's change this run, and widening a menu commit into
   that path is how two fixes become one bisect. */
static void menu_b_swallow_spend(int pad_live, XPad *pad)
{
    if (!menu_b_swallow) return;
    if (pad_live && (pad->buttons & 0x2000))
        pad->buttons = (unsigned short)(pad->buttons & ~0x2000u);
    else
        menu_b_swallow = 0;
}

/* SM64DS_PAD_TEST=<hex>@<frame>[,<hex>@<frame>...]: A SCRIPTED PAD, DBG1's,
   hoisted to file scope by SW1 so it reaches the windowed scene loop as well
   as the level loop. It is there for the same reason in both: a pad button is
   the one input this program has no way to drive from a script, so every claim
   about one used to rest on somebody's hands -- and a scene lane that cannot
   press A cannot prove its menu works at all.

   Each entry holds its XInput mask for PAD_TEST_HOLD frames from its frame and
   releases it after, so a list of entries produces a list of EDGES through the
   ordinary detector rather than around it: what it proves is the real mapping,
   not a shortcut past it.

   An entry with no @frame is SILENTLY INERT -- the frame defaults to -1 and
   nothing matches it -- so "1000" alone presses nothing and only "1000@105"
   presses A. That is deliberate (a malformed entry must not fire on every
   frame of the run) but it is quiet, so a fixture that does nothing is worth
   re-reading for a missing @ before it is worth debugging.

   Inert unless the variable is set, and it cannot reach a selftest: the
   environment is read once behind g_selftest and never read again. */
static void pad_test_apply(int frame, int *pad_live, XPad *pad)
{
    enum { PAD_TEST_HOLD = 4 };
    static const char *pt_env = (const char *)1;
    if (pt_env == (const char *)1)
        pt_env = g_selftest ? 0 : getenv("SM64DS_PAD_TEST");
    if (!pt_env)
        return;
    unsigned mask = 0;
    const char *p = pt_env;
    while (*p) {
        char *q;
        const unsigned m = (unsigned)strtoul(p, &q, 16);
        long f = -1;
        p = q;
        if (*p == 64 /* '@' */) f = strtol(p + 1, &q, 10), p = q;
        if (f >= 0 && frame >= f && frame < f + PAD_TEST_HOLD)
            mask |= m;
        while (*p && *p != 44 /* ',' */) ++p;
        if (*p == 44) ++p;
    }
    if (mask) {
        if (!*pad_live) { memset(pad, 0, sizeof *pad); *pad_live = 1; }
        pad->buttons = (unsigned short)(pad->buttons | mask);
    }
}

#ifndef PORT_ROM_CLEAN
/* ---- SM64DS_CLICK_TEST: A SCRIPTED STYLUS (port mod, run link60 lane TCH2) -
 *
 * COMPILED OUT OF THE SHIPPING (PORT_ROM_CLEAN) BUILD, run link60 lane RELSTRIP.
 * This is the one OS-input DRIVER in the port: SetCursorPos + SendInput move the
 * real pointer and press a real button edge through the OS input queue, which is
 * exactly the "fileless" synthetic-input pattern a stranger's antivirus flags.
 * It is a developer/reviewer test tool, dormant in normal play (inert unless the
 * env is set, and unreachable from a selftest), so it stays in the developer
 * build and only that build. When SM64DS_CLICK_TEST is set on a shipping build
 * the driver is simply not present and the variable does nothing; real player
 * input (GetCursorPos/GetAsyncKeyState for the mouse-as-stylus) is untouched,
 * and SM64DS_PAD_TEST (in-process, no OS input API) also stays in all builds.
 *
 * The pad has SM64DS_PAD_TEST and the touch record has SM64DS_TOUCH_PROBE, and
 * between them sits the thing neither one covers: THE MOUSE. SM64DS_TOUCH_PROBE
 * writes DS pixels straight into data_020a0de8 and therefore proves nothing
 * about the window, the present rectangle or the transform -- it starts
 * downstream of all three. So every claim about where a click lands rested on
 * somebody's hand on a mouse, which is why a correct transform went five
 * sessions being blamed for a minigame that would not respond.
 *
 *     SM64DS_CLICK_TEST="cx,cy@f0[-f1][,cx,cy>dx,dy@f0-f1,...]"
 *
 *   cx,cy@f0        press at CLIENT pixel (cx,cy) on frame f0, held
 *                   CLICK_TEST_HOLD frames -- long enough for the DS's own
 *                   edge rule, which needs the button down on two consecutive
 *                   polls before `held` comes up
 *   cx,cy@f0-f1     the same press, held from f0 to f1 inclusive
 *   cx,cy>dx,dy@f0-f1
 *                   a DRAG: held across f0..f1, the point walking linearly
 *                   from (cx,cy) to (dx,dy). Curling wants one of these.
 *
 * IT DRIVES THE OS, NOT THE WINDOW QUEUE, and that is the whole design of it.
 * The obvious build is PostMessage(WM_LBUTTONDOWN) and it would have proved
 * nothing here: the stylus is poll_touch (hal/sub_screen.cpp), which reads
 * GetCursorPos and GetAsyncKeyState(VK_LBUTTON) and never looks at a window
 * message, so a posted message exercises walk_window's WM_LBUTTONDOWN handler
 * -- whose output nothing consumes -- and leaves the touch record untouched. A
 * driver that proves the dead path green while the live path is dark is worse
 * than no driver. SetCursorPos plus SendInput puts the press in the same place
 * the player's hand puts it, upstream of BOTH paths, so the WndProc gets its
 * genuine WM_LBUTTONDOWN and poll_touch gets its genuine GetAsyncKeyState in
 * one motion, through every layer of arithmetic either of them uses.
 *
 * WHAT IT COSTS: it moves the real pointer, because the real pointer is what
 * is being tested. The cursor's position is saved on the first press and put
 * back when the script ends. Run it on a machine nobody is typing on.
 *
 * INERT UNLESS SET, LOUD WHEN IT IS. Unset, the env is read once and this
 * function returns on a null for the rest of the run. Set, it prints the
 * script it parsed and one line per edge, so a fixture that silently matched
 * no frame cannot read as a pass. It can never reach a selftest: g_selftest is
 * checked at the same read SM64DS_PAD_TEST checks it at, so the BMP battery
 * cannot be perturbed by a stray click.
 */
enum { CLICK_TEST_HOLD = 6, CLICK_TEST_MAX = 16 };
struct ClickTestEnt {
    int x0, y0, x1, y1;      /* client pixels; x1/y1 == x0/y0 for a plain press */
    int f0, f1;
};
static ClickTestEnt g_ct[CLICK_TEST_MAX];
static int g_ct_n = -1;          /* -1 = env not read yet, 0 = driver off */
static int g_ct_down;            /* is the synthetic button currently down */
static POINT g_ct_restore;       /* where the pointer was before the first press */
static int g_ct_restore_ok;

static void click_test_parse(void)
{
    g_ct_n = 0;
    const char *s = g_selftest ? 0 : getenv("SM64DS_CLICK_TEST");
    if (!s)
        return;
    while (*s && g_ct_n < CLICK_TEST_MAX) {
        ClickTestEnt e;
        char *q;
        memset(&e, 0, sizeof e);
        e.x0 = (int)strtol(s, &q, 10);
        s = q;
        if (*s == ',') ++s;
        e.y0 = (int)strtol(s, &q, 10);
        s = q;
        e.x1 = e.x0;
        e.y1 = e.y0;
        if (*s == '>') {
            ++s;
            e.x1 = (int)strtol(s, &q, 10);
            s = q;
            if (*s == ',') ++s;
            e.y1 = (int)strtol(s, &q, 10);
            s = q;
        }
        /* NO @frame IS NOT "every frame". An entry that names no frame is
           inert, the same refusal SM64DS_PAD_TEST makes for the same reason:
           a malformed fixture must not fire continuously and read as a pass. */
        e.f0 = -1;
        e.f1 = -1;
        if (*s == '@') {
            ++s;
            e.f0 = (int)strtol(s, &q, 10);
            s = q;
            if (*s == '-') {
                ++s;
                e.f1 = (int)strtol(s, &q, 10);
                s = q;
            } else {
                e.f1 = e.f0 + CLICK_TEST_HOLD - 1;
            }
        }
        if (e.f0 >= 0) {
            g_ct[g_ct_n++] = e;
            fprintf(stderr, "[click] script %d: client (%d,%d)", g_ct_n - 1,
                    e.x0, e.y0);
            if (e.x1 != e.x0 || e.y1 != e.y0)
                fprintf(stderr, " -> (%d,%d)", e.x1, e.y1);
            fprintf(stderr, " frames %d..%d\n", e.f0, e.f1);
        } else {
            fprintf(stderr, "[click] an entry named no @frame and is INERT "
                            "(client %d,%d)\n", e.x0, e.y0);
        }
        while (*s && *s != ',') ++s;
        if (*s == ',') ++s;
    }
    if (g_ct_n)
        fflush(stderr);
}

/* One synthetic left-button edge through the OS input queue.
 *
 * THE STRUCT IS windows.h's OWN INPUT AND THE SIZE IS ITS sizeof, and this is
 * not tidiness -- it is the bug this function shipped with for one run. The
 * first cut hand-rolled the layout and got 36 bytes where the 32-bit INPUT is
 * 28. SendInput VALIDATES cbSize against its own idea of the structure and
 * returns 0 without injecting anything when they disagree, so every press in
 * the grid was announced by the [click] line above and none of them happened.
 * A run that logs eleven presses and zero touches looks exactly like a broken
 * touch bridge, which is the wrong bug to spend an evening on.
 *
 * THE RETURN IS CHECKED FOR THE SAME REASON. It is the only thing that can
 * tell an injected press from a refused one, and UIPI refuses silently too: a
 * process at a lower integrity level than the foreground window's cannot send
 * it input, and the failure mode there is also "the log says press, the game
 * sees nothing". Said once, loudly. */
static void click_test_button(int down)
{
    INPUT in;
    if (!W.SendInput_)
        return;
    memset(&in, 0, sizeof in);
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
    if (W.SendInput_(1, &in, (int)sizeof(INPUT)) != 1) {
        static int said;
        if (!said++) {
            fprintf(stderr, "[click] SendInput REFUSED the button edge "
                    "(win32 %lu). Nothing was pressed; every [click] line in "
                    "this run is a request, not an event.\n",
                    (unsigned long)GetLastError());
            fflush(stderr);
        }
        return;
    }
    g_ct_down = down;
}

static void click_test_finish(void);   /* defined below; armed with atexit */

static void click_test_apply(HWND h, int frame)
{
    if (g_ct_n < 0) {
        click_test_parse();
        if (g_ct_n > 0)
            atexit(click_test_finish);
        if (g_ct_n > 0 && W.SetForegroundWindow_) {
            /* the window has to be the foreground one or the WndProc half of
               the press is delivered somewhere else entirely. poll_touch would
               still see it (GetAsyncKeyState is machine-global) which is
               exactly the kind of half-green this driver exists to refuse.

               THIS IS WHY SM64DS_NO_FOCUS SWITCHES ITSELF OFF when this driver
               is armed. A WS_EX_NOACTIVATE window cannot be brought to the
               foreground at all, so under both flags at once this call would
               fail and every press below would be refused by the pid gate.
               nofocus_mode() makes that decision, out loud, before the window
               is created; nothing here has to check. */
            W.SetForegroundWindow_(h);
            /* and it has to be top of the z-order too: WindowFromPoint reads
               z-order, so on a busy desktop the pid gate refuses every press
               and a grid reads as all-refusals (review TCR1 lost three runs
               to this). HWND_TOPMOST, no move, no resize; only when the
               click driver is armed. */
            if (W.SetWindowPos_)
                W.SetWindowPos_(h, (HWND)-1, 0, 0, 0, 0, 0x0001u | 0x0002u);
        }
    }
    if (g_ct_n <= 0 || !W.SetCursorPos_ || !W.ClientToScreen_)
        return;

    const ClickTestEnt *e = 0;
    for (int i = 0; i < g_ct_n; ++i)
        if (frame >= g_ct[i].f0 && frame <= g_ct[i].f1) { e = &g_ct[i]; break; }

    if (!e) {
        if (g_ct_down) {
            click_test_button(0);
            fprintf(stderr, "[click] f%d release\n", frame);
            fflush(stderr);
        }
        return;
    }

    /* the point this frame: the start for a press, walked for a drag */
    int cx = e->x0, cy = e->y0;
    if ((e->x1 != e->x0 || e->y1 != e->y0) && e->f1 > e->f0) {
        const int span = e->f1 - e->f0;
        cx = e->x0 + (e->x1 - e->x0) * (frame - e->f0) / span;
        cy = e->y0 + (e->y1 - e->y0) * (frame - e->f0) / span;
    }
    POINT p;
    p.x = cx;
    p.y = cy;
    if (!W.ClientToScreen_(h, &p))
        return;
    if (!g_ct_down && W.GetCursorPos_ && !g_ct_restore_ok)
        g_ct_restore_ok = W.GetCursorPos_(&g_ct_restore) ? 1 : 0;
    /* EVERY frame, not only on the press edge: the pointer is a shared device
       and a hand that brushes the desk mid-drag would otherwise move the
       stylus somewhere the script never asked for and the log would show a
       drag the fixture did not describe. */
    W.SetCursorPos_((int)p.x, (int)p.y);
    /* ---- THE PID GATE, AND IT IS NOT PARANOIA -------------------------------
       A SendInput button edge is a real one: it goes to whatever window is
       under the pointer, and this desktop runs several lanes' proof windows at
       once. A press that lands on somebody else's window is at best a lost
       proof and at worst a click on a control in their run. So the press is
       gated on the window under that exact screen point belonging to THIS
       process -- not on the window being ours by name, and not on focus, both
       of which can be true while the pointer sits over a different one.
       Refused loudly, once, because a driver that silently stops pressing is a
       driver that reports a clean grid it never clicked. */
    if (!g_ct_down) {
        int mine = 1;
        if (W.WindowFromPoint_ && W.GetWindowThreadProcessId_) {
            DWORD owner = 0;
            const HWND under = W.WindowFromPoint_(p);
            if (!under) {
                mine = 0;
            } else {
                W.GetWindowThreadProcessId_(under, &owner);
                mine = owner == GetCurrentProcessId();
            }
        }
        if (!mine) {
            static int said;
            if (!said++) {
                fprintf(stderr, "[click] f%d REFUSED: screen point (%ld,%ld) "
                        "is over a window this process does not own. Nothing "
                        "is pressed; another lane's window is in the way or "
                        "this one never came to the foreground.\n",
                        frame, p.x, p.y);
                fflush(stderr);
            }
            return;
        }
        click_test_button(1);
        fprintf(stderr, "[click] f%d press client(%d,%d)\n", frame, cx, cy);
        fflush(stderr);
    }
}

/* Put the button and the pointer back.
 *
 * REGISTERED WITH atexit AND ALSO CALLED BY HAND, because a synthetic button
 * left down does not die with the process -- it belongs to the desktop, and
 * the next thing the owner clicks would be a drag. The level loop has several
 * exits and the scene loop has one, so rather than find every one of them the
 * cleanup is idempotent and armed at the same moment the script is. The
 * by-hand call in the scene loop is kept for ordering: it releases before
 * port_scene_finish writes its captures, not after. */
static void click_test_finish(void)
{
    static int done;
    if (g_ct_n <= 0 || done)
        return;
    done = 1;
    /* THE RELEASE IS UNCONDITIONAL, and the `if (g_ct_down)` this replaces was
       a real defect rather than a tidy-up.

       g_ct_down is this driver's own bookkeeping and it is NOT the physical
       button state. click_test_button leaves it untouched whenever SendInput
       refuses an edge, and the pid gate can return between a press and its
       release, so the two can disagree. When they disagree the wrong way the
       button stays down after the process is gone -- and the next run on this
       desktop starts with the stylus already pressed, because poll_touch reads
       GetAsyncKeyState(VK_LBUTTON), which is machine-global and outlives us.

       Review CTR1 lost a CONTROL run to exactly that: 96 dScMgCurling_c state
       entries in a run with no script at all, from a button still held down by
       the previous SM64DS_CLICK_TEST run, GetAsyncKeyState reading 1 until it
       was cleared by hand. A control that is silently driving input is worse
       than a failed one, because it reads as evidence.

       A spurious LEFTUP when nothing is held costs nothing. A missed one
       contaminates every run that follows. So: always send it. */
    const int was_down = g_ct_down;
    click_test_button(0);
    fprintf(stderr, "[click] release at exit%s\n",
            was_down ? "" : " (button was not marked down; released anyway)");
    if (g_ct_restore_ok && W.SetCursorPos_)
        W.SetCursorPos_(g_ct_restore.x, g_ct_restore.y);
    fflush(stderr);
}
#endif  /* !PORT_ROM_CLEAN: end of SM64DS_CLICK_TEST synthetic-stylus driver */

/* ---- THE DS KEYPAD BITS BOTH PATHS AGREE ON (port mod, run link60 SW1) ----
   Buttons -> the Ctrl held/pressed fields directly (CheckInput's remap tables
   are ROM pointers with no host image). DS bits: 1 = A (punch), 2 = B (jump),
   0x400 = X (crouch), 0x800 = Y (the dash button the walk core reads).

   Xbox layout per Tango: A jump, X run, B punch, bumpers rotate the camera. RT
   is meant to be crouch, but the old "crouch = 0x100" binding was a GUESS and
   0x100 is provably the camera rotate-right bit (func_02009e70 reads
   held & 0x4300) -- likely what the LT "crouch crash" actually hit. The REAL
   crouch bit is 0x400 (St_Crouch_Main holds on it, St_Land enters with it,
   Crawl exits by it).

   SHARED BECAUSE THIS IS THE PART THAT COULD DRIFT: which button jumps. The
   level loop adds its own tail on top (the camera-rotate bits, run-mode AUTO,
   fifteen SM64DS_SELFTEST_* probes) and the scene loop adds the d-pad, Start
   and Select, and those two tails are deliberately NOT unified -- see
   port/scene_window.txt section 5a. */
static unsigned short host_ds_buttons(int pad_live, const XPad *pad)
{
    unsigned short btn = 0;
    if (key_live(VK_SPACE)) btn |= 2;
    /* THE RUN BUTTON, from wherever the player put it. Shift by default, so an
       untouched settings.json is the line that was here before; zero means
       they unbound the keyboard half. */
    if (g_run_key && key_live(g_run_key)) btn |= 0x800;
    if (key_live(VK_CONTROL)) btn |= 0x400;
    if (key_live('X')) btn |= 1;
    if (pad_live) {
        if (pad->buttons & 0x1000) btn |= 2;      /* A  -> jump  */
        /* X by default; the rebind row moves it (0 = unbound) */
        if (g_run_pad && (pad->buttons & (unsigned)g_run_pad))
            btn |= 0x800;
        if (pad->buttons & 0x2000) btn |= 1;      /* B  -> punch */
        if (pad->rt > 100) btn |= 0x400;          /* RT -> crouch */
        /* the bumpers are camera-rotate and go in with the rest of the rotate
           input at the level loop's own call site, where the freecam gate is */
    }
    return btn;
}

/* The save-state toast, over everything, bottom-left, and decremented as it is
   drawn rather than in the tick so a menu's pause does not freeze it. Shared
   by both loops because it is the only channel the menu's refusals have. */
static void toast_draw(const OvlSurface &fb)
{
    if (ss_toast_left <= 0)
        return;
    --ss_toast_left;
    const int tw = (int)strlen(ss_toast) * OVL_ADVANCE * OVL_SCALE;
    const int ty = ntr::SCREEN_H - OVL_LINE - 4;
    ovl_shade(fb, 2, ty - 2, tw + 8 * OVL_SCALE, OVL_LINE + 4 * OVL_SCALE);
    ovl_text(fb, 4 + OVL_SCALE, ty, ss_toast, 0xFFFFFFFFu);
}

/* ---- PRESENT (port mod) -----------------------------------------------
   THE FRAME, FITTED TO WHATEVER SIZE THE WINDOW IS NOW.

   What was here before was one line:

       StretchDIBits(hdc, 0, 0, SCREEN_W * ZOOM, SCREEN_H * ZOOM, ...)

   -- a destination rectangle that is a compile-time constant. That is fine
   for a window that cannot change size, and the window was created
   WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME to make sure of it. But clearing
   WS_THICKFRAME only removes the SIZING BORDER; WS_MAXIMIZEBOX survives it,
   so the maximize button and a double-click on the title bar still worked and
   still do exactly what they say. The client area then became the monitor and
   the blit stayed 768x576 in the top-left corner, and the rest of the client
   area was never written by anything: the class brush is null (wc is
   zero-initialised), there is no WM_ERASEBKGND handler and there is no
   WM_PAINT handler, so nothing in the program had ever painted a pixel
   outside that fixed rectangle. What showed there was the window's backing
   store as the compositor left it -- the stale scrap of whatever the resize
   dragged through. That is the glitch, and it is a present-path bug from end
   to end: the raster never knew the window had a size.

   So the destination is measured every present instead:

     - GetClientRect for the size the window is at this instant,
     - the largest DS-aspect rectangle that fits inside it, centred,
     - the four leftover strips painted black (PatBlt BLACKNESS), which is
       what makes the bars bars and not history,
     - StretchDIBits into the fitted rectangle.

   The bars are painted every present rather than on a size change. Painting
   black over black is invisible, it costs four PatBlts on strips that are
   empty at the default size, and it means no code path can leave a bar
   showing something else -- including the ones that never reach the frame
   loop at all (a WM_PAINT during a modal drag, a restore from minimised).

   WM_ERASEBKGND is answered "handled, painted nothing" so the flicker the
   default erase would add during a drag never starts.

   THE FILTER is nearest by default (COLORONCOLOR: GDI drops and duplicates
   whole pixels). SM64DS_PRESENT_FILTER=halftone asks for HALFTONE, which is
   GDI's box filter and needs SetBrushOrgEx after it per the API contract.
   Nearest keeps the DS's hard pixel edges and the 8x8 overlay font crisp at
   any scale; halftone smooths both. Both are captured in this lane's
   evidence set at every size and the call between them is Tango's. */
static const int PRESENT_FILTER_NEAREST = 0;
static const int PRESENT_FILTER_HALFTONE = 1;
enum { PRESENT_STRETCH_COLORONCOLOR = 3, PRESENT_STRETCH_HALFTONE = 4 };
static int g_present_filter = PRESENT_FILTER_NEAREST;

/* What the frame loop hands the window procedure, so a WM_SIZE or a WM_PAINT
   arriving inside a modal drag loop -- where the frame loop is not running --
   can still redraw the picture at the size the drag is at. Nothing here is
   written before the window exists, and present() no-ops until all three are.
   The framebuffer pointer is the loop's own static, so this is a handle on
   the live frame rather than a copy of one. */
static HWND g_present_hwnd;
static HDC g_present_hdc;
static const BITMAPINFO *g_present_bi;
static const ntr::Framebuffer *g_present_fb;
/* THE STACKED SOURCE, when there is one. Set by the frame loop to the tall
   image hal_sub_screen_stacked_image built, with its own BITMAPINFO because
   the DIB height is part of the header rather than an argument. Null in the
   inset layout, and present() then reads the framebuffer exactly as it always
   did -- the two are never both live. */
static const uint32_t *g_present_stack;
static const BITMAPINFO *g_present_stack_bi;

/* The DS aspect, once, in the two numbers the fit uses. SCREEN_W/SCREEN_H are
   the framebuffer's, which is the DS panel at whatever tier this binary was
   built for, so this is 4:3 at every tier -- and STACK_W/STACK_H are the same
   panel twice, stacked, so the stacked fit is 2:3. */
static void present(void)
{
    if (!g_present_hwnd || !g_present_hdc || !g_present_bi || !g_present_fb)
        return;
    if (!W.GetClientRect_ || !W.StretchDIBits_) return;
    RECT rc;
    if (!W.GetClientRect_(g_present_hwnd, &rc)) return;
    const int cw = rc.right - rc.left, ch = rc.bottom - rc.top;
    /* MINIMISED is a zero-by-zero client area, and every arithmetic step
       below divides by one of them. Nothing to present to, so nothing is
       presented -- and no StretchDIBits with a zero destination, which is
       what a restore used to come back through. */
    if (cw <= 0 || ch <= 0) return;

    /* WHICH IMAGE IS BEING PRESENTED. Everything below is the same fit, the
       same bars and the same blit whichever it is; only the source pointer,
       the source size and the DIB header change. Doing it this way rather
       than with a second present() is deliberate: a resize feature that lives
       in one of two copies of this function is a resize feature that works in
       one layout. */
    const uint32_t *bits;
    const BITMAPINFO *bi;
    int sw, sh;
    if (g_present_stack && g_present_stack_bi) {
        bits = g_present_stack;
        bi = g_present_stack_bi;
        /* THE SOURCE SIZE IS THE HEADER'S, not a constant, and it is read out
           of the header rather than asked of hal because present() runs from
           the window procedure inside a modal resize drag. Reading the live
           layout there would be reading a value the frame loop is between two
           uses of; the header and the pixels were published together by
           stack_present_arm below, so they agree by construction. */
        sw = bi->bmiHeader.biWidth;
        sh = -bi->bmiHeader.biHeight;
    } else {
        bits = &g_present_fb->px[0][0];
        bi = g_present_bi;
        sw = ntr::SCREEN_W;
        sh = ntr::SCREEN_H;
    }
    /* the largest sw:sh rectangle inside cw x ch. Compared as a cross
       product so the choice is exact rather than a rounded ratio: wider than
       the frame means pillarbox (height wins), taller means letterbox. */
    int dw, dh;
    if ((long long)cw * sh <= (long long)ch * sw) {
        dw = cw;
        dh = (int)(((long long)cw * sh) / sw);
    } else {
        dh = ch;
        dw = (int)(((long long)ch * sw) / sh);
    }
    if (dw < 1) dw = 1;
    if (dh < 1) dh = 1;
    const int dx = (cw - dw) / 2, dy = (ch - dh) / 2;

    /* the four strips around it, black. Written before the picture so a
       stretch that lands a pixel wide of the arithmetic covers the bar
       rather than the bar covering it. */
    if (W.PatBlt_) {
        if (dy > 0) W.PatBlt_(g_present_hdc, 0, 0, cw, dy, BLACKNESS);
        if (dy + dh < ch)
            W.PatBlt_(g_present_hdc, 0, dy + dh, cw, ch - (dy + dh), BLACKNESS);
        if (dx > 0) W.PatBlt_(g_present_hdc, 0, dy, dx, dh, BLACKNESS);
        if (dx + dw < cw)
            W.PatBlt_(g_present_hdc, dx + dw, dy, cw - (dx + dw), dh, BLACKNESS);
    }

    if (W.SetStretchBltMode_) {
        if (g_present_filter == PRESENT_FILTER_HALFTONE) {
            W.SetStretchBltMode_(g_present_hdc, PRESENT_STRETCH_HALFTONE);
            /* the API contract: HALFTONE leaves the brush origin needing a
               reset or the shrink pattern walks */
            if (W.SetBrushOrgEx_) W.SetBrushOrgEx_(g_present_hdc, 0, 0, 0);
        } else {
            W.SetStretchBltMode_(g_present_hdc, PRESENT_STRETCH_COLORONCOLOR);
        }
    }
    W.StretchDIBits_(g_present_hdc, dx, dy, dw, dh, 0, 0, sw, sh,
                     bits, bi, DIB_RGB_COLORS, SRCCOPY);
    /* the touch bridge's half of the same arithmetic. The SOURCE SIZE goes
       with the rectangle: in the stacked layout the rectangle was filled from
       an image twice as tall as the framebuffer, and an inverse that assumed
       otherwise would put every stylus press on the wrong screen. */
    hal_present_set_rect(dx, dy, dw, dh, sw, sh);
}

/* ---- FULLSCREEN (port mod) --------------------------------------------
   F12, borderless, and never a mode change. Exclusive fullscreen would mean
   asking the display for a resolution, which can fail, can leave the desktop
   rearranged if the program dies while it holds it, and buys nothing here --
   the present path already scales to any client size, so a borderless window
   over the monitor's own bounds is the same picture with none of that.

   The restore is a saved WINDOWPLACEMENT plus the saved style, so a window
   that was maximised before F12 comes back maximised and one that was at
   some hand-dragged size comes back at that size.

   ITS SetWindowPos DOES FRONT THE WINDOW (HWND_TOP, no SWP_NOACTIVATE) and is
   left that way under SM64DS_NO_FOCUS, because it cannot run under it: the
   only caller is the F12/F11 edge in the two frame loops, read through
   key_live, and key_live returns released whenever hal_window_focused() is
   false -- which a window that never reaches the foreground always is. A
   person pressing F12 has focus by definition and should get the fullscreen
   they asked for. Named here so a future caller that is not a keypress knows
   it is fronting. */
static int g_fullscreen;
static WINDOWPLACEMENT g_fs_placement;
static LONG g_fs_style;

static void fullscreen_toggle(HWND h)
{
    if (!W.GetWindowLongA_ || !W.SetWindowLongA_ || !W.SetWindowPos_ ||
        !W.MonitorFromWindow_ || !W.GetMonitorInfoA_ ||
        !W.GetWindowPlacement_ || !W.SetWindowPlacement_)
        return;
    if (!g_fullscreen) {
        g_fs_placement.length = sizeof g_fs_placement;
        if (!W.GetWindowPlacement_(h, &g_fs_placement)) return;
        g_fs_style = W.GetWindowLongA_(h, GWL_STYLE);
        MONITORINFO mi;
        mi.cbSize = sizeof mi;
        HMONITOR mon = W.MonitorFromWindow_(h, MONITOR_DEFAULTTONEAREST);
        if (!mon || !W.GetMonitorInfoA_(mon, &mi)) return;
        W.SetWindowLongA_(h, GWL_STYLE,
                          (g_fs_style & ~(LONG)WS_OVERLAPPEDWINDOW) |
                              (LONG)WS_POPUP);
        W.SetWindowPos_(h, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                        mi.rcMonitor.right - mi.rcMonitor.left,
                        mi.rcMonitor.bottom - mi.rcMonitor.top,
                        SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        g_fullscreen = 1;
    } else {
        W.SetWindowLongA_(h, GWL_STYLE, g_fs_style);
        W.SetWindowPlacement_(h, &g_fs_placement);
        W.SetWindowPos_(h, 0, 0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        g_fullscreen = 0;
    }
    present();
}

/* ---- MOUSE (port mod) -------------------------------------------------
   DRAG TO LOOK, on the right button, and not an F2 capture toggle. Both were
   on the table; this is the one that fits what the window is. A capture mode
   swallows the pointer for as long as it is armed, which fights the debug
   menu, fights alt-tabbing out of a play session, and leaves a hidden cursor
   behind if the program dies with it on -- and the flight recorder exists
   because this program does sometimes die. Hold the right button, look, let
   go, and the pointer is yours again with nothing to remember.

   The drag anchors: pressing the button remembers where the pointer was,
   every move reports its delta from there and puts the pointer straight back.
   So the look never runs out of desk or hits the edge of the screen the way a
   plain drag does, the cursor is hidden while it is held, and letting go
   leaves the pointer exactly where it was picked up.

   The wheel zooms the rig. Both act in ANALOG and FREECAM; in DS-exact the
   camera is the game's and the mouse does not touch it.

   ---- AND THE CAPTURE MODE IS BACK, BEHIND A SETTING (run mg10, lane ESC)

   What stands above is still the DEFAULT and still the whole program with the
   setting off. But Tango asked for the other half -- "hold my mouse in the
   game and let me just move to turn" -- so settings.json's MouseCapture turns
   it on, and the three objections written above are answered rather than
   ignored, because they were all correct:

   "IT FIGHTS THE DEBUG MENU." It does, so it is not on while the menu is
   open: mo_capture_want below has menu_on in it. That is also what makes
   ESCAPE the release, which is the convention every game with a captured
   pointer already teaches -- escape opens the menu (lane ESC's other half),
   the menu drops the capture, and the pointer is back in one press. Escape
   again closes the menu and takes it again. There is no second key to learn
   and no third state to be stuck in.

   "IT FIGHTS ALT-TABBING OUT OF A PLAY SESSION." It did, so focus is in the
   same test: the frame the window stops being foreground is the frame the
   pointer is handed back, unclipped and visible, and coming back takes it
   again. This is the same edge key_live already reads for the keyboard, so
   the mouse and the keys let go together.

   "IT LEAVES A HIDDEN CURSOR BEHIND IF THE PROGRAM DIES WITH IT ON -- and the
   flight recorder exists because this program does sometimes die." This is
   the one that needed a mechanism rather than a test, and the answer is that
   both halves of the capture are PER-PROCESS state that Windows unwinds for
   us. ShowCursor's counter belongs to this process's own input queue and dies
   with it; ClipCursor's fence is released when the thread that set it goes
   away. A crash therefore ends with a visible, free pointer without this
   program running a single line of cleanup -- which is the only kind of
   cleanup a crash can be relied on to do. The tidy paths (WM_KILLFOCUS,
   WM_DESTROY, the frame test) are belt and braces on top of that, not the
   thing being relied on.

   HOW IT STEERS, and it is deliberately not a new input path: the capture
   simply makes the anchor-and-spring-back above run without a button held.
   The anchor is the middle of the client area rather than wherever the
   pointer happened to be, because there is no pick-up point when there is no
   pick-up; every move still reports its delta and is put straight back, so
   the same mo_dx/mo_dy reach the same rig at the same 48 and 24 binangs a
   pixel. Nothing downstream of these two variables can tell the two modes
   apart, which is why turning the setting on cannot change how the camera
   feels, only what your hand has to do.

   WHAT THE POINTER IS FOR WHEN IT IS NOT STEERING is the reason for the rest
   of mo_capture_want's list. The mouse in this program is also the DS's
   stylus (poll_touch, hal/sub_screen.cpp), and a stylus that is pinned to the
   middle of the picture and invisible is not a stylus. So the capture stays
   off wherever the pointer is really a pen: the whole SCENE path, which is
   where the minigames run; any STACKED window, where the bottom half of the
   picture IS the bottom screen; and DS-exact, where the mouse steers nothing
   and taking it would be pure cost.

   WHAT IT TAKES AWAY, said plainly because a player meets both of these before
   they meet anything else. The fence is the CLIENT AREA, so while the capture
   is on the pointer cannot reach the corner stylus panel, the title bar, the
   sizing border or the close button: the window cannot be dragged, resized or
   closed with the mouse until it lets go. Escape is how it lets go, alt+F4
   still closes and alt-tab still leaves. That is the same bargain every game
   with a captured pointer makes, and it is the reason the release is a key
   players already reach for rather than one this file invented. */
static int mo_look;              /* right button down */
static POINT mo_anchor;          /* screen point the drag springs back to */
static int mo_dx, mo_dy;         /* accumulated since the loop last drained */
static int mo_wheel;             /* accumulated notches, forward positive */

/* THE CAPTURE'S OWN THREE. mo_capture_opt is settings.json's MouseCapture,
   re-read live beside the volume; mo_captured is whether the pointer is
   actually held THIS FRAME, which is the option and the six refusals together;
   mo_capture_home is where the pointer was standing when it was taken, so
   letting go puts it back rather than leaving it in the middle of the picture.

   mo_capture_opt is a file-scope copy rather than a call to
   host_setting_mouse_capture() at each reader, for the reason the run-mode
   keys are: the frame loop asks this question every frame and the answer may
   only move when the watcher says the file moved. */
static int mo_capture_opt;
static int mo_captured;
static POINT mo_capture_home;
static int mo_capture_home_ok;

/* THE TOUCH BRIDGE'S HANDOFF. The DS has a touchscreen and this program has a
   mouse, and the last left click is where the two meet. Position is in
   FRAMEBUFFER pixels, so a consumer gets the same numbers at either tier
   without knowing which one it is on.

   IT IS NO LONGER A DIVIDE BY ZOOM. The window resizes, so the frame is
   scaled to fit the client area and centred inside it, and the inverse of
   that fit is hal_present_client_to_fb (hal/sub_screen.cpp) -- the same
   function the bottom-screen stylus goes through, which is the point of it
   being one function.

   A CLICK IN A LETTERBOX BAR IS NOT A CLICK ON THE PICTURE, and these words
   say so: the position keeps the last on-picture click and `g_mouse_click_new`
   stays down for a bar click, the same answer poll_touch gives the stylus.
   `g_mouse_click_new` is true for exactly the frame an on-picture click landed
   on and `g_mouse_left_down` is the hold, which is what a drag on a
   touchscreen is. NOTHING IN THIS FILE READS ANY OF IT: it is published for
   the touch bridge a sibling stream is building. */
int g_mouse_click_x, g_mouse_click_y;
int g_mouse_click_new;
int g_mouse_left_down;

/* THE CURSOR IS HIDDEN IFF SOMETHING IS STEERING WITH IT, and this is the one
   function that decides it. ShowCursor is a COUNTER and not a flag, so two
   independent hiders that each pushed and popped it would drift the moment
   they overlapped -- let go of the right button during a capture and the
   pointer would reappear in the middle of a look. Both loops drive the counter
   to a target instead of stepping it, so calling this at any time from
   anywhere leaves it in the state these two variables describe. */
static void mo_cursor_sync(void)
{
    if (!W.ShowCursor_) return;
    if (mo_look || mo_captured) while (W.ShowCursor_(FALSE) >= 0) {}
    else                        while (W.ShowCursor_(TRUE) < 0) {}
}

static void mo_release(void)
{
    if (!mo_look) return;
    mo_look = 0;
    if (W.ReleaseCapture_) W.ReleaseCapture_();
    mo_cursor_sync();
}

/* The middle of the client area in SCREEN coordinates -- the point a captured
   pointer is parked on and springs back to. The client area and not the window
   rect, so the title bar and the sizing border do not pull the anchor off
   centre, and re-asked on every engage so a resized or moved window re-centres
   without anything having to notice that it moved. */
static int mo_client_center(HWND h, POINT *p)
{
    RECT rc;
    if (!W.GetClientRect_ || !W.ClientToScreen_) return 0;
    if (!W.GetClientRect_(h, &rc)) return 0;
    if (rc.right <= rc.left || rc.bottom <= rc.top) return 0;  /* zero-size */
    p->x = (rc.right - rc.left) / 2;
    p->y = (rc.bottom - rc.top) / 2;
    return W.ClientToScreen_(h, p) ? 1 : 0;
}

/* THE FENCE. Pens the pointer inside the client area, so a fast flick between
   two spring-backs cannot land it on another window and click something there.
   A null ClipCursor_ leaves a capture that still hides and re-centres, which is
   a worse capture and not a broken one -- hence no refusal here. */
static void mo_capture_clip(HWND h)
{
    RECT rc, s;
    POINT tl, br;
    if (!W.ClipCursor_ || !W.GetClientRect_ || !W.ClientToScreen_) return;
    if (!W.GetClientRect_(h, &rc)) return;
    tl.x = rc.left;  tl.y = rc.top;
    br.x = rc.right; br.y = rc.bottom;
    if (!W.ClientToScreen_(h, &tl) || !W.ClientToScreen_(h, &br)) return;
    s.left = tl.x; s.top = tl.y; s.right = br.x; s.bottom = br.y;
    W.ClipCursor_(&s);
}

/* RE-AIM A HELD POINTER AT THE WINDOW IT IS ON. The anchor and the fence are
   both derived from the client rectangle, so a window that moves or resizes
   under a capture leaves both of them describing where the window USED to be:
   the pointer springs back to a point off the picture and the fence pens it
   somewhere the player is not looking. Called from WM_SIZE and WM_MOVE, which
   between them are every way a client rectangle can change -- a border drag, a
   maximise, and this program's own F12 fullscreen toggle. */
static void mo_capture_refresh(HWND h)
{
    POINT c;
    if (!mo_captured) return;
    if (!mo_client_center(h, &c)) return;
    mo_anchor = c;
    if (W.SetCursorPos_) W.SetCursorPos_(c.x, c.y);
    mo_dx = mo_dy = 0;          /* a re-aim is not a look */
    mo_capture_clip(h);
}

/* TAKE OR HAND BACK THE POINTER. Idempotent on purpose: the frame loop calls
   this every frame with the answer it wants, so the transitions live here
   rather than at each of the seven places that can change the answer. */
static void mo_capture_set(HWND h, int on)
{
    if (!!on == mo_captured) return;
    if (on) {
        POINT c;
        if (!W.GetCursorPos_ || !W.SetCursorPos_) return;
        /* A ZERO-SIZE CLIENT AREA HAS NO MIDDLE, and this is the honest
           refusal for it rather than a capture centred on a corner: the
           harness opens windows like that. */
        if (!mo_client_center(h, &c)) return;
        /* remembered BEFORE the pointer is moved, which is the whole point of
           remembering it */
        mo_capture_home_ok = W.GetCursorPos_(&mo_capture_home) ? 1 : 0;
        mo_captured = 1;
        mo_anchor = c;
        W.SetCursorPos_(c.x, c.y);
        /* WHATEVER THE POINTER DID ON THE WAY IN IS NOT A LOOK. Moving it to
           the centre is this program's own move and the WM_MOUSEMOVE it
           generates would otherwise be drained as a delta and snap the camera
           by however far across the desk the pointer had been standing. */
        mo_dx = mo_dy = 0;
        mo_capture_clip(h);
    } else {
        mo_captured = 0;
        if (W.ClipCursor_) W.ClipCursor_(0);
        if (mo_capture_home_ok && W.SetCursorPos_)
            W.SetCursorPos_(mo_capture_home.x, mo_capture_home.y);
        mo_capture_home_ok = 0;
        /* and the move BACK is not a look either, for the same reason */
        mo_dx = mo_dy = 0;
    }
    mo_cursor_sync();
    fprintf(stderr, "[mouse] capture %s\n", on ? "ENGAGED" : "released");
}

/* MAY THE POINTER BE HELD THIS FRAME? Every term is a release the player would
   otherwise have to go and find, and the reasons are in the MOUSE banner
   above. Written as one function with one caller so the answer cannot be half
   true anywhere: the loop asks, mo_capture_set does.

   `stacked` is passed rather than asked, because this is a header-free file
   scope and the loops already hold the answer their window was built for. */
static int mo_capture_want(int selftest, int stacked)
{
    if (!mo_capture_opt) return 0;      /* the setting, and it is off by default */
    if (selftest) return 0;             /* no player, no pointer */
    if (stacked) return 0;              /* the bottom half is a touchscreen */
    if (cam_mode == CAM_DS) return 0;   /* the mouse steers nothing there */
    if (menu_on) return 0;              /* escape is the release */
    if (g_rebind_capture) return 0;     /* a key is being chosen */
    if (!hal_window_focused()) return 0;/* alt-tab hands it back */
    return 1;
}

/* The player has chosen a window size, by dragging the border or by
   maximising, so nothing in this program may choose one for them afterwards.
   Set by the window procedure below; read by stack_present_arm, which is the
   one thing that would otherwise re-size a running window. */
static int g_user_sized;

static LRESULT CALLBACK wndproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    /* the capture is dropped here as well as on the frame test, because a
       window being destroyed has no more frames to test on */
    if (m == WM_DESTROY) {
        mo_capture_set(h, 0);
        mo_release();
        W.PostQuitMessage_(0);
        return 0;
    }
    /* AHEAD OF THE ESCAPE BRANCH, deliberately: while the rebind row is
       capturing, a press is a BINDING and nothing else, escape included --
       otherwise the one key a player would reach for to back out would close
       the game instead. Bit 30 of lParam is the previous key state, so the
       auto-repeat of a key still held from arming the capture is ignored and
       only a fresh press counts. WM_SYSKEYDOWN as well as WM_KEYDOWN, or alt
       would be the one key on the board a player could not bind and would
       open the system menu behind the capture instead. */
    if ((m == WM_KEYDOWN || m == WM_SYSKEYDOWN) && g_rebind_capture) {
        if (!(l & (1 << 30))) g_rebind_key = (int)w;
        return 0;
    }
    /* THERE IS NO ESCAPE BRANCH HERE ANY MORE (run mg10, lane ESC), and its
       absence is the feature. It read
           if (m == WM_KEYDOWN && w == VK_ESCAPE) { mo_release(); quit; }
       so escape closed the game. It is now an alias of F5 -- it opens and
       closes the debug menu -- and it is read where F5 is read, in menu_input's
       held-mask, so it passes the selftest gate, the rebind-capture gate, the
       focus gate and the stale-key latch that a branch up here would each have
       had to re-solve. The rebind capture above still swallows it before
       anything else sees it, for the reason it always did.

       Escape now falls through to DefWindowProcA with every other key, which
       does nothing with it. THE WINDOW'S CLOSE BUTTON AND ALT+F4 ARE THE QUIT,
       and they were always the other way out: they arrive as WM_CLOSE, the
       default handler destroys the window, and WM_DESTROY above posts the quit
       message escape used to post directly. */
    switch (m) {
    case WM_RBUTTONDOWN:
        if (W.GetCursorPos_ && W.GetCursorPos_(&mo_anchor)) {
            mo_look = 1;
            mo_dx = mo_dy = 0;
            if (W.SetCapture_) W.SetCapture_(h);
            if (W.ShowCursor_) while (W.ShowCursor_(FALSE) >= 0) {}
        }
        return 0;
    case WM_RBUTTONUP:
        mo_release();
        return 0;
    case WM_KILLFOCUS:
        /* AHEAD OF THE FRAME TEST AND NOT INSTEAD OF IT. mo_capture_want has
           hal_window_focused() in it, so the next frame would drop the capture
           anyway -- but "the next frame" is a promise a stalled loop does not
           keep, and the one moment a player must not have to wait for a frame
           is the moment they have alt-tabbed away and want their pointer. */
        mo_capture_set(h, 0);
        mo_release();
        return 0;
    case WM_CAPTURECHANGED:
        /* NOT a capture drop: this is win32's mouse capture (SetCapture), a
           different thing that happens to share the word. Losing it ends a
           right-button drag and says nothing about the pointer lock. */
        mo_release();
        return 0;
    case WM_MOUSEMOVE:
        if ((mo_look || mo_captured) && W.GetCursorPos_) {
            POINT p;
            if (W.GetCursorPos_(&p)) {
                mo_dx += p.x - mo_anchor.x;
                mo_dy += p.y - mo_anchor.y;
                if ((p.x != mo_anchor.x || p.y != mo_anchor.y) &&
                    W.SetCursorPos_)
                    W.SetCursorPos_(mo_anchor.x, mo_anchor.y);
            }
        }
        return 0;
    case WM_LBUTTONDOWN: {
        /* CLIENT PIXELS BACK TO FRAMEBUFFER PIXELS, and it is no longer a
           divide by ZOOM. The picture is centred in whatever the client area
           is now and scaled to fit it, so undoing that is the present rect's
           own arithmetic run backwards -- which is why it lives next to the
           forward one rather than being spelled a second time here. A click
           in a letterbox bar is outside the picture; it is clamped to the
           edge the way an off-window drag always was, so the published point
           stays a framebuffer pixel. */
        int cx = 0, cy = 0;
        /* THE RETURN IS HONOURED, the way poll_touch honours it. A click in a
           letterbox bar is off the picture, and publishing the clamped edge
           pixel for it would hand the touch bridge a stylus press on the rim
           of the screen that the player never made -- a bug with no reader
           today and therefore no way to notice it later. Off the picture: the
           hold still latches (a drag that starts on the picture and wanders
           into a bar is still a drag) but no new click position is published.

           AND THE MESSAGE WAS A LIE IN THE STACKED LAYOUT (run link60, lane
           TCH2). hal_present_client_to_fb means the TOP screen in both
           layouts, so in a stacked run EVERY click on the bottom half -- which
           is every click a minigame player makes, the whole point of the
           layout -- came back "outside" and this line called it a letterbox
           bar. There is no letterbox in a 512x768 client showing a 512x768
           image; the click was on the picture, on the other screen, and the
           stylus took it. These were the only touch-shaped lines in a play
           session's log (poll_touch printed nothing at all until this lane
           gave it a voice), so a working stylus read as fifteen rejected
           clicks, and this lane was opened to fix a transform that was
           already correct. Ask the other band before saying which it is. */
        const int mx = (short)LOWORD(l), my = (short)HIWORD(l);
        int dsx = 0, dsy = 0;
        const int on_picture = hal_present_client_to_fb(mx, my, &cx, &cy);
        const int on_sub = hal_present_client_to_sub(mx, my, &dsx, &dsy);
        g_mouse_left_down = 1;
        /* THE MOUSE IS CAPTURED FOR THE LENGTH OF THE HOLD, so the motion and
           the button-up keep arriving while the pointer is outside the window.
           The STYLUS does not need this and never did, because poll_touch
           reads GetCursorPos and GetAsyncKeyState, which are machine-global and
           were never interrupted. g_mouse_left_down did: without a capture a
           release outside the window delivers WM_LBUTTONUP to whatever is under
           the pointer instead, and the hold below latched at 1 for the rest of
           the session. Armed here rather than after the on-picture test, on
           purpose: a press in a letterbox bar already latches the hold, and a
           hold that latches without a capture is the half-state this closes.
           Released in WM_LBUTTONUP. */
        if (W.SetCapture_) W.SetCapture_(h);
        if (!on_picture) {
            if (on_sub)
                fprintf(stderr, "[mouse] click %d,%d client is on the BOTTOM "
                        "screen, DS (%d,%d) -- the stylus takes it; no "
                        "top-screen point published\n", mx, my, dsx, dsy);
            else
                fprintf(stderr, "[mouse] click %d,%d client off-picture "
                        "(letterbox bar), not published\n", mx, my);
            return 0;
        }
        if (cx < 0) cx = 0;
        if (cy < 0) cy = 0;
        if (cx >= ntr::SCREEN_W) cx = ntr::SCREEN_W - 1;
        if (cy >= ntr::SCREEN_H) cy = ntr::SCREEN_H - 1;
        g_mouse_click_x = cx;
        g_mouse_click_y = cy;
        g_mouse_click_new = 1;
        fprintf(stderr, "[mouse] click %d,%d fb\n", cx, cy);
        return 0;
    }
    case WM_LBUTTONUP:
        g_mouse_left_down = 0;
        /* and only when the right-button look is not holding a capture of its
           own, which mo_release owns and would lose here */
        if (!mo_look && W.ReleaseCapture_) W.ReleaseCapture_();
        return 0;
    case WM_MOUSEWHEEL:
        mo_wheel += (short)HIWORD(w) / WHEEL_DELTA;
        return 0;
    /* ---- the resize seam ------------------------------------------------
       A drag on the sizing border runs a MODAL loop inside DefWindowProc:
       this file's frame loop is not running for as long as the button is
       held, so the only thing that can keep the picture under the edge being
       dragged is the window procedure itself. WM_SIZE arrives on every step
       of that drag, and presenting the last frame from here is what makes
       the picture follow the edge instead of smearing behind it. */
    case WM_SIZE:
        /* SIZE_MAXIMIZED (2) is a size the player asked for as much as a drag
           is, so the stacked layout must not grow out from under it either.
           SIZE_RESTORED from this program's own SetWindowPos is NOT: that path
           is stack_present_arm and it sets nothing. */
        if (w == 2) g_user_sized = 1;
        /* a held pointer is aimed at a client rectangle that has just moved */
        mo_capture_refresh(h);
        present();
        return 0;
    /* ONLY EVER FOR THE CAPTURE. The window has never needed to know that it
       moved -- the picture is drawn in client coordinates -- but a held
       pointer's anchor and fence are in SCREEN coordinates, so a drag of the
       title bar is exactly the case that strands them. Falls through to the
       default handler either way, because moving is still the system's. */
    case WM_MOVE:
        mo_capture_refresh(h);
        break;
    /* THE PLAYER'S HAND ON THE SIZING BORDER, which is the edge the stacked
       layout has to stop growing at. WM_SIZING arrives only from a border
       drag -- SetWindowPos does not send it -- so it is the one message that
       says "this size is a choice" rather than "this size happened". */
    case WM_SIZING:
        g_user_sized = 1;
        break;
    /* Same reason, for the repaints the compositor asks for: a restore from
       minimised, an uncover, a monitor change. Presenting and then
       validating is the whole of it -- without the validate the region stays
       dirty and WM_PAINT is re-posted forever. */
    case WM_PAINT:
        present();
        /* The validate is what ends the paint. Without it the region stays
           dirty and WM_PAINT is re-posted forever, so if ValidateRect is the
           one name in this file that failed to resolve, this branch must NOT
           swallow the message -- DefWindowProc's BeginPaint/EndPaint pair
           validates it instead. Falling through is the safe answer; returning
           0 here would spin the message loop at 100% of a core. */
        if (!W.ValidateRect_) break;
        W.ValidateRect_(h, 0);
        return 0;
    /* "Handled, and I painted nothing." The default erase fills the client
       area with the class brush before the picture lands on top of it, which
       is a full-window flash on every step of a drag. present() already owns
       every pixel: the picture, and black in the bars. */
    case WM_ERASEBKGND:
        return 1;
    /* A sane floor on the drag. 256x192 is the DS panel at 1:1 -- below that
       the frame is being thrown away rather than scaled, and the overlay
       font stops being readable at all. Sent before the window exists on
       some paths, so the style is read defensively. */
    case WM_GETMINMAXINFO: {
        MINMAXINFO *mmi = (MINMAXINFO *)l;
        RECT mr = {0, 0, 256, 192};
        const LONG style = W.GetWindowLongA_ ? W.GetWindowLongA_(h, GWL_STYLE)
                                             : (LONG)WS_OVERLAPPEDWINDOW;
        if (W.AdjustWindowRect_) W.AdjustWindowRect_(&mr, (DWORD)style, FALSE);
        mmi->ptMinTrackSize.x = mr.right - mr.left;
        mmi->ptMinTrackSize.y = mr.bottom - mr.top;
        return 0;
    }
    default:
        break;
    }
    return W.DefWindowProcA_(h, m, w, l);
}

/* ---- THE WINDOW, OPENED THE ONE WAY (port mod, run link60 SW1) ------------

   Both modes ask for the same class, the same styles, the same class-owned DC
   and the same two DIB headers; only the size the layout asks for and the
   controls card in the title bar differ, and both of those are arguments. A
   second copy of this would be a second place for CS_OWNDC, WS_THICKFRAME or
   the min-track size to be got wrong, and the first two have each already cost
   this file a session.

   CS_OWNDC IS LOAD-BEARING FOR THE RESIZE rather than a habit. The DC below is
   fetched once and held for the life of the program. A DC out of the common
   cache has its VISIBLE REGION computed at GetDC time and never again, so
   after the client area grew, every pixel of the new area was outside that
   region and clipped away -- the bars could not have been painted through it
   even by code that tried. A class-owned DC is the one kind the system keeps
   in step with the window's size.

   THE STACKED LAYOUT ASKS FOR A WINDOW TWICE AS TALL, because the picture it
   presents is twice as tall and a window opened at the framebuffer's aspect
   would letterbox the whole thing down to half the width. AND IT DOES NOT TAKE
   THE ZOOM WITH IT, which is measured rather than tidy: ZOOM is 2 at the 2x
   tier, so SCREEN_W x STACK_H x ZOOM is 1024x1536 and a window that tall does
   not fit on any ordinary desktop -- the first stacked run opened one and the
   fit letterboxed the picture into the top two thirds of a window whose bottom
   third was off the screen. The stacked window opens at the stacked image's
   own size, 512x768 at this tier, which is Tango's number and the largest that
   fits. Nothing is lost by it: the present path scales to whatever the client
   area becomes, so the sizing border and F12 both still work.

   WS_THICKFRAME HAS TO BE IN BOTH PLACES, this call and the CreateWindowExA
   below. Changing it in the AdjustWindowRect alone is a silent no-op that
   looks like the fix: AdjustWindowRect only decides how big to ask for, and on
   Windows 11 it returns the SAME frame metrics with and without the sizing
   border (measured: identical rect for both styles), so the window opens at
   exactly the size it always did and every screenshot looks right while the
   sizing border is still not there. The style the window actually gets is the
   CreateWindowExA argument.

   THE TITLE BAR IS THE CONTROLS CARD. There is nowhere else to put them that
   does not cost a keypress to read: the F3 overlay is timings, the F5 menu is
   state, and both of those you have to already know how to open. The bar is
   the one surface that is legible before you touch anything, so the keys live
   there -- and a minigame's are not a level's, which is why it is a parameter. */
static BITMAPINFO g_bi;
static BITMAPINFO g_bi_stack;

/* ---- SM64DS_NO_FOCUS: A WINDOW THAT DOES NOT TAKE THE DESK (port mod) -------
 *
 * WHY. Every scripted run of this exe -- a battery selftest, a BMP probe, a
 * scene census -- opens a real window, and a real window opened the ordinary
 * way becomes the foreground one. On a shared machine that means the owner's
 * keystroke lands in Mario instead of in whatever he was typing into, once per
 * proof run, and there have been days with dozens of them. The window itself is
 * wanted (the scene path needs one for the stylus and the present loop, see
 * scene_window_run); it is the ACTIVATION that is not.
 *
 * WHAT IT DOES, in the two places a window can take the foreground:
 *
 *   at creation   WS_EX_NOACTIVATE in the extended style, and WS_VISIBLE taken
 *                 OUT of the ordinary style so CreateWindowExA does not do the
 *                 implicit activating show. The window is then shown with
 *                 ShowWindow(SW_SHOWNOACTIVATE).
 *   afterwards    the one automatic SetWindowPos on the frame path
 *                 (stack_present_arm's grow) takes SWP_NOACTIVATE.
 *
 * BOTH HALVES, not either. WS_EX_NOACTIVATE governs what a later click does and
 * SW_SHOWNOACTIVATE governs what THIS show does, and a window created
 * WS_VISIBLE has already been shown by the time any ShowWindow could speak.
 *
 * WHAT IT DELIBERATELY DOES NOT DO. It does not hide the window, does not make
 * it topmost, does not move it (SM64DS_WINDOW_POS is a separate knob) and does
 * not touch a single pixel of what is drawn. A run under this flag composes,
 * presents and captures exactly the frames the same run composes without it;
 * that is the claim the BMP floor checks.
 *
 * WHAT IT COSTS, stated rather than discovered. Two things, and the second is
 * the one that surprises people.
 *
 *   THE INTERACTIVE KEYBOARD IS DEAD for the run. A window that is never the
 *   foreground one is never focused, so hal_window_focused() is false the whole
 *   time and key_live returns released -- which is the same state the window is
 *   in today the moment you alt-tab away from it, not a new one. Right for a
 *   scripted run, wrong for a person, so this flag is for harnesses and the
 *   launcher bundle does not set it.
 *
 *   AND WS_EX_NOACTIVATE KEEPS THE WINDOW OFF THE TASKBAR, which Windows does
 *   on its own and this file cannot ask it not to without giving the activation
 *   back. So a run under this flag is not alt-tabbable and has no taskbar
 *   button. It is still a perfectly ordinary window otherwise: it has its title
 *   bar, it can be dragged, and its close button works -- a click on a
 *   WS_EX_NOACTIVATE window is delivered, it simply does not activate. A
 *   scripted run that hangs is closed with its X or with taskkill on the exe
 *   path, not by alt-tabbing to it.
 *
 * THE SCRIPTED INPUT PATHS ARE UNAFFECTED, and that is the whole reason this is
 * safe. SM64DS_PAD_TEST ORs bits into an in-process pad struct after
 * XInputGetState; SM64DS_PROBE_INPUT ORs into the pad mirror; the fifteen
 * SM64DS_SELFTEST_* probes write the stick and the buttons by hand;
 * SM64DS_TOUCH_PROBE pokes the DS touch record in memory. None of them reads
 * the OS, none of them asks hal_window_focused, and poll_touch's live branch is
 * gated on g_headless and GetAsyncKeyState -- which is machine-global -- rather
 * than on focus. Nothing in that list can tell the difference.
 *
 * SM64DS_CLICK_TEST IS THE ONE THING THAT CAN, and it OVERRIDES the flag out
 * loud instead of failing quietly. That driver moves the real pointer and
 * pushes a real button edge through the OS input queue, so the window has to be
 * the foreground one or the WndProc half of the press is delivered somewhere
 * else entirely (see click_test_apply). A WS_EX_NOACTIVATE window cannot be
 * brought to the foreground, so its SetForegroundWindow would fail and the run
 * would read as a grid of refusals -- exactly the half-green that driver's pid
 * gate exists to refuse. Under PORT_ROM_CLEAN the driver is not compiled in at
 * all, so there the variable is only a name and the override is gone with it.
 *
 * READ ONCE, like every other env in this file, so nothing can change the
 * window's shape halfway through a run. */
static int nofocus_mode(void)
{
    static int v = -1;
    if (v >= 0)
        return v;
    /* ATOI, NOT A NULL TEST, and the difference matters more here than it does
       for the other flags in this file. This one is meant to be set BROADLY --
       by a harness over a whole battery, by a run law over every scripted
       launch -- so the thing somebody will reach for to opt one run back out is
       SM64DS_NO_FOCUS=0. Under the `getenv() != 0` shape the rest of the file
       uses, that spelling would turn the flag ON and the run would look broken
       in the one direction nobody checks. =0 means off. */
    const char *e = getenv("SM64DS_NO_FOCUS");
    v = e ? (atoi(e) != 0) : 0;
#ifndef PORT_ROM_CLEAN
    if (v && getenv("SM64DS_CLICK_TEST")) {
        v = 0;
        fprintf(stderr, "[win] SM64DS_NO_FOCUS is OVERRIDDEN by "
                "SM64DS_CLICK_TEST: that driver pushes a real button edge "
                "through the OS and needs the foreground window. This run "
                "takes focus.\n");
        fflush(stderr);
    }
#endif
    if (v && !W.ShowWindow_) {
        /* Never on Windows, but the whole user32 surface here is loaded by
           name and a window created without WS_VISIBLE and never shown would
           be an invisible run that still wrote its BMP -- the worst possible
           way for this to fail. Fail back to the ordinary window. */
        v = 0;
        fprintf(stderr, "[win] SM64DS_NO_FOCUS asked for, but ShowWindow did "
                        "not resolve; opening the ordinary window instead.\n");
        fflush(stderr);
    }
    return v;
}

/* ---- WHAT SHOW THIS WINDOW GETS (port mod) ----------------------------------
 *
 * MEASURED FIRST, BECAUSE THE ANSWER WAS NO. Until this lane, the window was
 * created WS_VISIBLE, which means CreateWindowExA shows it itself and the
 * launcher's STARTUPINFO -- the wShowWindow that carries `start /min` and
 * PowerShell's -WindowStyle Minimized -- was NEVER CONSULTED for it. Anyone
 * launching a harness run minimized got a minimized CONSOLE (that one the OS
 * creates from the same STARTUPINFO, and walk_window is a console-subsystem
 * exe) and a perfectly normal, perfectly foreground-hungry game window beside
 * it. Half the request was being honoured and it looked like all of it.
 *
 * SO IT IS HONOURED NOW, for the shows that mean "stay out of my way":
 *
 *   SW_SHOWMINIMIZED / SW_MINIMIZE / SW_SHOWMINNOACTIVE -> SW_SHOWMINNOACTIVE
 *   SW_SHOWNOACTIVATE / SW_SHOWNA                       -> SW_SHOWNOACTIVATE
 *
 * The minimised ones are mapped to the NOACTIVE spelling deliberately: a
 * launcher that asked for minimised was asking not to be interrupted, and
 * SW_SHOWMINIMIZED activates the window on the way down.
 *
 * SW_HIDE IS NOT HONOURED, and that is a decision rather than an oversight.
 * -WindowStyle Hidden is the ordinary way to hide a CONSOLE, so honouring it
 * here would turn a routine "don't show me the terminal" into a run that
 * composes frames, writes a BMP and shows nothing -- a real capture from a run
 * nobody could watch, which is the stale-artifact shape wearing a different
 * hat. A run that wants to stay out of the way asks with SM64DS_NO_FOCUS.
 *
 * EVERY OTHER wShowWindow, and any launcher that does not set
 * STARTF_USESHOWWINDOW at all, returns -1 and gets the historical path: the
 * window keeps WS_VISIBLE and no ShowWindow is called. That covers SW_SHOWNORMAL
 * and SW_SHOWDEFAULT, which is what PowerShell's Start-Process sends by default
 * and what the kit's play.bat produces, so no ordinary launch changes.
 *
 * GetStartupInfoA is kernel32, called directly rather than through W: kernel32
 * is already this file's one static import (GetModuleHandleA, GetLastError) and
 * port/release_hardening.txt's kit property is "KERNEL32.dll only", which this
 * keeps. */
static int host_show_mode(int nofocus)
{
    int want = -1;
    if (!W.ShowWindow_)
        return -1;      /* no show call available; keep WS_VISIBLE */
    STARTUPINFOA si;
    memset(&si, 0, sizeof si);
    si.cb = sizeof si;
    GetStartupInfoA(&si);
    if (si.dwFlags & STARTF_USESHOWWINDOW) {
        switch (si.wShowWindow) {
        case SW_SHOWMINIMIZED:
        case SW_MINIMIZE:
        case SW_SHOWMINNOACTIVE:
            want = SW_SHOWMINNOACTIVE;
            break;
        case SW_SHOWNOACTIVATE:
        case SW_SHOWNA:
            want = SW_SHOWNOACTIVATE;
            break;
        default:
            break;
        }
    }
    /* SM64DS_MINIMIZED=1 -- THE SAME REQUEST, FOR A LAUNCHER THAT CANNOT MAKE
       IT. Run mg16 lane MP3.

       Everything above reads the request out of STARTUPINFO, which is the right
       place and is what port/tools/mp2_proof.py uses (it sets wShowWindow = 7
       directly, because Python can). NOT EVERY LAUNCHER CAN. .NET's
       ProcessStartInfo.WindowStyle is not carried into STARTUPINFO when
       UseShellExecute is false, and a launcher that needs to redirect a stream
       has no choice about that -- so port/tools/mp2_two_windows.ps1 asked for
       Minimized, got no STARTF_USESHOWWINDOW at all, and the window came up
       VISIBLE. That is the second time a launcher lost this guarantee silently;
       the first was a cmd.exe shim eating the STARTUPINFO on the way past.

       So the request can also be made the way every other port knob is made.
       STARTUPINFO STILL WINS where it carries a real spelling -- this only
       fills in when the launcher could not speak that way -- and the composition
       with SM64DS_NO_FOCUS below is unchanged.

       It is deliberately a REQUEST TO BE QUIETER, never louder: there is no
       env value that un-minimizes a window STARTUPINFO asked to minimize. */
    if (want == -1) {
        const char *m = getenv("SM64DS_MINIMIZED");
        if (m && atoi(m) != 0) want = SW_SHOWMINNOACTIVE;
    }
    if (nofocus) {
        /* THE TWO COMPOSE rather than one winning. A minimized request under
           SM64DS_NO_FOCUS is minimized AND not activated, which is the quietest
           run this program can do; the flag alone is a visible window that does
           not take the desk. */
        if (want == SW_SHOWMINNOACTIVE)
            return SW_SHOWMINNOACTIVE;
        return SW_SHOWNOACTIVATE;
    }
    return want;
}

static HWND host_window_open(int stacked, HDC *out_hdc, const char *title)
{
    /* run mg16 lane MP2: WHICH COPY OF THE GAME IS THIS?
       Two instances now run side by side as DS parent and child, and two
       identical title bars are two windows a player cannot tell apart. When
       SM64DS_INSTANCE names an instance its tag leads the title, so the pair
       reads "[P1] SM64DS | ..." and "[P2] SM64DS | ...". With the env unset the
       tag is empty and the title is byte-for-byte the string the caller passed,
       which is what every existing run still gets. The same env separates the
       exe-adjacent files that ARE separated -- startup_error.txt,
       savestate.bin and settings.json's sibling temp, but NOT crash.txt or
       exit.txt, and hal/instance_tag.h's survey says exactly which and why --
       so one knob does both jobs and there is no second name to keep in
       sync. */
    char titlebuf[320];
    if (port_instance_tag()[0]) {
        _snprintf(titlebuf, sizeof titlebuf, "[%s] %s",
                  port_instance_tag() + 1 /* skip the leading '.' */, title);
        titlebuf[sizeof titlebuf - 1] = 0;
        title = titlebuf;
    }
    /* Registered once. Two windows are never open at the same time in this
       program, but a second RegisterClassA of a live class fails and there is
       no reason to make the second caller find that out. */
    static int registered;
    if (!registered) {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = wndproc;
        wc.style = CS_OWNDC;
        wc.hInstance = GetModuleHandleA(0);
        wc.hCursor = W.LoadCursorA_(0, (LPCSTR)IDC_ARROW);
        wc.lpszClassName = "sm64ds_walk";
        if (!W.RegisterClassA_(&wc))
            return 0;
        registered = 1;
    }
    /* SM64DS_PRESENT_FILTER=halftone swaps the scaler; see present(). */
    {
        const char *pf = getenv("SM64DS_PRESENT_FILTER");
        if (pf && (pf[0] == 'h' || pf[0] == 'H'))
            g_present_filter = PRESENT_FILTER_HALFTONE;
    }
    /* THE STACKED SHAPE IS ASKED FOR RATHER THAN SPELLED, because a minigame
       that simulates the DS's hinge composes a taller image and the number is
       not known until its InitResources has run. At this point -- before the
       scene has booted -- the answer is the gapless one, 512x768, which is
       exactly the window this opened before the gap existed. stack_present_arm
       grows it later, once, when the scene latches its G. */
    int stw = ntr::STACK_W, sth = ntr::STACK_H;
    if (stacked) hal_sub_screen_stacked_size(&stw, &sth);
    RECT r = stacked ? RECT{0, 0, stw, sth}
                     : RECT{0, 0, ntr::SCREEN_W * ZOOM, ntr::SCREEN_H * ZOOM};
    W.AdjustWindowRect_(&r, WS_OVERLAPPEDWINDOW, FALSE);
    /* ---- WHERE IT OPENS (port mod, Tango's ask: "can it open center screen")
       CW_USEDEFAULT IS NOT A POSITION. It asks Windows for the next slot in
       its cascade, which starts near the top-left of the primary monitor and
       steps down and right for each window an application opens -- so where
       the frame lands depended on how many windows this process had opened
       before it and on nothing a player can see or change. On the owner's
       desk that came out in the top right.

       Centred on the WORK AREA (rcWork, not rcMonitor) so the taskbar never
       covers the bottom of the picture, and on the monitor UNDER THE CURSOR
       rather than the primary one, because somebody with two screens is
       looking at the one their hand is on.

       SIZE IS NOT TOUCHED: r is whatever the block above decided, and BOTH
       SHAPES come through here -- the inset level window and the 512x768
       stacked scene -- so both are placed by this one block and neither is
       resized by it. The F12 fullscreen path is not involved: it saves and
       restores its own WINDOWPLACEMENT and picks its monitor with
       MonitorFromWindow, so it now springs back to the middle instead of to
       the cascade slot, which is the whole change it sees.

       Degrades in two steps rather than one, because a window at 0,0 would be
       a worse answer than the old one: no cursor or no MonitorFromPoint falls
       back to the primary monitor, and no monitor info at all falls back to
       CW_USEDEFAULT and the behaviour that shipped before this. */
    int wx = CW_USEDEFAULT, wy = CW_USEDEFAULT;
    {
        const int ww = r.right - r.left, wh = r.bottom - r.top;
        HMONITOR mon = 0;
        POINT cur;
        MONITORINFO mi;
        mi.cbSize = sizeof mi;
        if (W.MonitorFromPoint_ && W.GetCursorPos_ && W.GetCursorPos_(&cur))
            mon = W.MonitorFromPoint_(cur, MONITOR_DEFAULTTONEAREST);
        if (!mon && W.MonitorFromWindow_)
            mon = W.MonitorFromWindow_(0, MONITOR_DEFAULTTOPRIMARY);
        if (mon && W.GetMonitorInfoA_ && W.GetMonitorInfoA_(mon, &mi)) {
            wx = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - ww) / 2;
            wy = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - wh) / 2;
            /* A window TALLER than the work area centres to a negative origin
               and loses its title bar off the top of the screen, which is the
               one placement a person cannot drag their way out of. The
               stacked shape is 512x768 plus frame, so this is reachable on a
               short screen rather than theoretical. Clamped to the work
               area's own corner; the bottom is allowed to overhang, because
               that is draggable and a lost title bar is not. */
            if (wx < mi.rcWork.left) wx = mi.rcWork.left;
            if (wy < mi.rcWork.top)  wy = mi.rcWork.top;
            /* SAYS WHERE IT PUT THE WINDOW AND WHAT IT MEASURED, because a
               placement is the one thing in this program a player can see and
               nobody can reproduce from a log without it. "The window opened
               off screen" is otherwise an unanswerable report: this line has
               the work area, the frame size and the origin, so the arithmetic
               can be checked after the fact from the recorder file alone.
               It is also this change's own evidence -- centring is a claim
               about numbers the process has and the outside does not. */
            fprintf(stderr, "[win] work area %ld,%ld %ldx%ld -> %dx%d frame "
                    "centred at %d,%d\n", mi.rcWork.left, mi.rcWork.top,
                    mi.rcWork.right - mi.rcWork.left,
                    mi.rcWork.bottom - mi.rcWork.top, ww, wh, wx, wy);
        } else {
            fprintf(stderr, "[win] no monitor info; leaving the placement to "
                            "Windows (the pre-centring behaviour)\n");
        }
        /* SM64DS_WINDOW_POS=x,y OVERRIDES THE CENTRE, and it is the last word
           on purpose: a harness that says where the window goes has said
           something more specific than "the monitor under the cursor". Virtual
           screen coordinates, the same ones CreateWindowExA takes, so a
           negative x parks it on a left-hand second monitor. Nothing is
           clamped -- a run that asks to be off screen has asked for that, and
           the clamp above exists for the CENTRING's own arithmetic rather than
           as a policy about where windows may be. SIZE IS NOT TOUCHED; this
           moves the same rect the block above sized. */
        if (const char *wp = getenv("SM64DS_WINDOW_POS")) {
            int px = 0, py = 0;
            if (sscanf(wp, "%d,%d", &px, &py) == 2) {
                wx = px;
                wy = py;
                fprintf(stderr, "[win] SM64DS_WINDOW_POS put the frame at "
                        "%d,%d\n", wx, wy);
            } else {
                fprintf(stderr, "[win] SM64DS_WINDOW_POS=\"%s\" is not x,y; "
                                "keeping the placement above\n", wp);
            }
        }
    }
    /* SM64DS_NO_FOCUS (see nofocus_mode above) and the launcher's own show
       request (host_show_mode above). Dropping WS_VISIBLE is what stops
       CreateWindowExA doing its implicit activating show before any ShowWindow
       can speak, so it comes out whenever there is a show to make by hand.

       WITH NEITHER ASKED FOR, show is -1 and both expressions are the constants
       that were written here before -- 0 and WS_OVERLAPPEDWINDOW | WS_VISIBLE
       -- and no ShowWindow is called at all. */
    const int nofocus = nofocus_mode();
    const int show = host_show_mode(nofocus);
    HWND hwnd = W.CreateWindowExA_(nofocus ? WS_EX_NOACTIVATE : 0,
                                   "sm64ds_walk", title,
                                   /* the sizing border is here, not in the
                                      AdjustWindowRect above; see that note */
                                   show >= 0 ? (WS_OVERLAPPEDWINDOW)
                                             : (WS_OVERLAPPEDWINDOW | WS_VISIBLE),
                                   wx, wy,
                                   r.right - r.left, r.bottom - r.top, 0, 0,
                                   GetModuleHandleA(0), 0);
    if (!hwnd)
        return 0;
    if (show >= 0) {
        /* The show the style above withheld. Neither of the two spellings that
           reach here activates the window, so the foreground stays where it
           was, which is the entire ask. */
        W.ShowWindow_(hwnd, show);
        fprintf(stderr, "[win] shown with %s%s; this window does not take the "
                "foreground%s\n",
                show == SW_SHOWMINNOACTIVE ? "SW_SHOWMINNOACTIVE"
                                           : "SW_SHOWNOACTIVATE",
                nofocus ? " and WS_EX_NOACTIVATE (SM64DS_NO_FOCUS)"
                        : " (the launcher's own STARTUPINFO asked for it)",
                nofocus ? " and the interactive keyboard is off for the run "
                          "(the scripted input paths are unaffected)"
                        : "");
    }
    *out_hdc = W.GetDC_(hwnd);
    memset(&g_bi, 0, sizeof g_bi);
    g_bi.bmiHeader.biSize = sizeof g_bi.bmiHeader;
    g_bi.bmiHeader.biWidth = ntr::SCREEN_W;
    g_bi.bmiHeader.biHeight = -ntr::SCREEN_H;   /* top-down */
    g_bi.bmiHeader.biPlanes = 1;
    g_bi.bmiHeader.biBitCount = 32;
    g_bi.bmiHeader.biCompression = BI_RGB;
    /* the stacked image's header. Same in every field but the height, and a
       second BITMAPINFO rather than one mutated per frame because present()
       can run from the window procedure inside a modal resize drag, with the
       frame loop stopped -- a header the frame loop was halfway through
       editing would be read by that path. */
    g_bi_stack = g_bi;
    g_bi_stack.bmiHeader.biWidth = stw;
    g_bi_stack.bmiHeader.biHeight = -sth;
    return hwnd;
}

/* ---- THE RESPONSIVE STACKED LAYOUT (port mod) -------------------------------
 *
 * A minigame's simulated screen gap is not known when the window opens. The
 * value lives in ov004's framework word and each minigame writes it in its own
 * InitResources, which runs after the scene has been handed the window, so the
 * image the frame loop composes gets 2G rows taller partway through the first
 * few frames of a run. Something has to notice.
 *
 * WHAT IT DOES WHEN IT NOTICES. The DIB header takes the new height -- without
 * that the blit reads the right pixels through the wrong shape and the picture
 * shears. And the window GROWS by exactly the band, so the image stays at an
 * integer scale in the client area and the two screens keep the size they had
 * a frame ago. Growing rather than re-fitting is the point: a fit into the old
 * client area would shrink both screens to make room for the band, which is
 * the feature taking picture away to add furniture.
 *
 * AND IT DOES NOT FIGHT THE PLAYER. If the window has been resized by hand,
 * maximised, or put in F12 fullscreen, the size is theirs and the band is
 * absorbed by the fit -- letterboxed or scaled down inside whatever they chose.
 * A program that snapped a hand-sized window back to its own idea of correct
 * would be worse than one that letterboxes.
 *
 * NOTHING HERE TOUCHES THE INSET LAYOUT, which has no stacked image and no
 * band, and nothing here runs at all until the first stacked image exists.
 */
static unsigned g_stack_gen = ~0u;

/* SM64DS_STACK_BMP=<path>[,<frame>]: WRITE THE IMAGE THIS PROGRAM IS ABOUT TO
   PRESENT, once, at the named frame (default 0).
 *
 * The one thing no capture in the tree could show. SM64DS_SCENE_BMP writes the
 * 512x384 framebuffer; SM64DS_SCENE_BMP_STACKED writes an image that
 * port_scene_finish RE-COMPOSES from that framebuffer after the run. Both of
 * them are pictures of engine A plus engine B, and neither is a picture of what
 * the window showed -- which is the only thing that can settle where a HOST
 * overlay landed, because an overlay is not in either engine.
 *
 * Off unless the variable is set, and then it fires once and never again: the
 * frame count is part of the probe on a scene whose two screens exchange every
 * frame, and a capture that could not name its frame would be unreadable there.
 */
static void stack_capture(const uint32_t *img)
{
    static int at = -2;
    static int done;
    static char path[512];
    static unsigned long frame;
    if (at == -2) {
        at = -1;
        if (const char *e = getenv("SM64DS_STACK_BMP")) {
            const char *comma = strrchr(e, ',');
            size_t n = comma ? (size_t)(comma - e) : strlen(e);
            if (n >= sizeof path) n = sizeof path - 1;
            memcpy(path, e, n);
            path[n] = 0;
            at = comma ? atoi(comma + 1) : 0;
            if (at < 0) at = 0;
        }
    }
    const unsigned long f = frame++;
    if (at < 0 || done || !img || (long)f < (long)at) return;
    done = 1;
    int w = 0, h = 0;
    hal_sub_screen_stacked_size(&w, &h);
    if (ntr::ppu_write_bmp_px(path, img, w, h))
        fprintf(stderr, "  [stack] wrote %s at frame %lu, %dx%d: THE PRESENTED "
                "IMAGE, host overlays included, upper screen at row %d\n",
                path, f, w, h, hal_sub_screen_stacked_top_y());
    else
        fprintf(stderr, "  [stack] could not write %s\n", path);
    fflush(stderr);
}

static void stack_present_arm(const uint32_t *img, HWND hwnd)
{
    stack_capture(img);
    if (!img) return;
    const unsigned gen = hal_sub_screen_stacked_generation();
    if (gen != g_stack_gen) {
        int w = ntr::STACK_W, h = ntr::STACK_H;
        hal_sub_screen_stacked_size(&w, &h);
        const int was_h = -g_bi_stack.bmiHeader.biHeight;
        /* the header first and always: it describes the buffer, and a blit
           through a stale one is a sheared picture whatever the window does */
        g_bi_stack.bmiHeader.biWidth = w;
        g_bi_stack.bmiHeader.biHeight = -h;
        /* WHAT IT COMPARES AGAINST IS THE HEADER, not a "have I run before"
           flag, and that distinction cost a windowed run before it was written
           down. host_window_open opens the window at the layout's size AS IT
           IS THEN -- which on a minigame is the gapless 512x768, because the
           scene's InitResources has not run and G is still 0 -- and stamps
           that size into g_bi_stack. The layout latches G a moment later,
           inside port_scene_begin, so the FIRST arm this function ever sees is
           already a change: 768 to 864. A draft that treated the first arm as
           "no change by definition" left the window at 768 with an 864-row
           image in it, and the fit letterboxed the whole picture into 455
           columns of a 512-column client area, with the stylus mapping
           correctly onto a window nobody wanted. was_h is the header's, the
           header is the window's, and a difference is a grow whenever it
           appears. */
        if (h != was_h && hwnd && !g_user_sized &&
            !g_fullscreen && W.GetClientRect_ && W.AdjustWindowRect_ &&
            W.SetWindowPos_ && W.GetWindowLongA_) {
            RECT cr;
            if (W.GetClientRect_(hwnd, &cr)) {
                /* THE GROWTH IS THE CLIENT AREA'S, measured rather than
                   assumed: the client is grown by the same number of rows the
                   image grew by, and the frame is re-derived from that with
                   AdjustWindowRect against the window's CURRENT style. Taking
                   the delta rather than re-asking for w x h is what keeps a
                   window a player nudged one pixel wide from snapping back. */
                const int cw = cr.right - cr.left, ch = cr.bottom - cr.top;
                RECT want = {0, 0, cw, ch + (h - was_h)};
                const LONG style = W.GetWindowLongA_(hwnd, GWL_STYLE);
                W.AdjustWindowRect_(&want, (DWORD)style, FALSE);
                /* SWP_NOMOVE and SWP_NOZORDER: the top-left corner stays put,
                   so the window grows downward from where the player left it
                   rather than re-centring itself mid-game.

                   AND SWP_NOACTIVATE (0x0010) UNDER SM64DS_NO_FOCUS. This is
                   the one SetWindowPos on the frame path that fires by itself,
                   a few frames into every minigame, and SetWindowPos without
                   that bit activates the window as part of the z-order work.
                   WS_EX_NOACTIVATE should already refuse it; both are named
                   because the flag's whole promise is that nothing on the frame
                   path takes the desk, and a promise resting on one API's
                   refusal is a promise resting on a reading of the docs. Added
                   only under the flag, so an ordinary run gets the same three
                   bits it always got. */
                W.SetWindowPos_(hwnd, 0, 0, 0, want.right - want.left,
                                want.bottom - want.top,
                                0x0002u | 0x0004u |
                                    (nofocus_mode() ? 0x0010u : 0u));
                fprintf(stderr, "[gap] the stacked image grew %d -> %d rows; "
                        "the window client area follows (%dx%d)\n", was_h, h,
                        cw, ch + (h - was_h));
            }
        } else if (h != was_h) {
            fprintf(stderr, "[gap] the stacked image grew %d -> %d rows; the "
                    "window keeps the size it has (%s)\n", was_h, h,
                    g_user_sized ? "the player chose it"
                                 : g_fullscreen ? "fullscreen" : "no window");
        }
        g_stack_gen = gen;
    }
    g_present_stack = img;
    g_present_stack_bi = &g_bi_stack;
}

/* camera folded into the GX projection matrix: P(perspective) * V(lookAt),
   built in floats and pushed as 4096-fixed.
   ITS CALLERS THINK IN WORLD UNITS and the frame is drawn in SCENE units
   (world >> 3, Camera::Render's own conversion), so the eye, the look-at and
   the near/far planes all come across the same divide right here. That keeps
   the hand-tuned rig behind SM64DS_OLD_CAMERA readable in the units its
   occlusion rays and standoff distances are written in. */
static void push_camera(const float eye_w[3], const float at_w[3])
{
    const float eye[3] = {eye_w[0] / 8, eye_w[1] / 8, eye_w[2] / 8};
    const float at[3] = {at_w[0] / 8, at_w[1] / 8, at_w[2] / 8};
    float fz[3] = {at[0] - eye[0], at[1] - eye[1], at[2] - eye[2]};
    float ln = sqrtf(fz[0] * fz[0] + fz[1] * fz[1] + fz[2] * fz[2]);
    for (int i = 0; i < 3; ++i) fz[i] /= (ln > 1e-6f ? ln : 1.0f);
    float up[3] = {0, 1, 0};
    float sx[3] = {fz[1] * up[2] - fz[2] * up[1],
                   fz[2] * up[0] - fz[0] * up[2],
                   fz[0] * up[1] - fz[1] * up[0]};
    ln = sqrtf(sx[0] * sx[0] + sx[1] * sx[1] + sx[2] * sx[2]);
    for (int i = 0; i < 3; ++i) sx[i] /= (ln > 1e-6f ? ln : 1.0f);
    float uy[3] = {sx[1] * fz[2] - sx[2] * fz[1],
                   sx[2] * fz[0] - sx[0] * fz[2],
                   sx[0] * fz[1] - sx[1] * fz[0]};

    /* row-vector convention to match the GX (v * M) */
    float V[16] = {
        sx[0], uy[0], -fz[0], 0,
        sx[1], uy[1], -fz[1], 0,
        sx[2], uy[2], -fz[2], 0,
        -(sx[0] * eye[0] + sx[1] * eye[1] + sx[2] * eye[2]),
        -(uy[0] * eye[0] + uy[1] * eye[1] + uy[2] * eye[2]),
        (fz[0] * eye[0] + fz[1] * eye[1] + fz[2] * eye[2]), 1};

    /* the ROM's fov: camera presets set the half-angle field to 0xbb0
       DS units = 16.46 deg, so the game renders ~33 deg vertical. The
       old 55 was a wide-angle lens -- it shrank and warped the world
       around Mario no matter how right the geometry was. */
    const float fovy = 32.9f * 3.14159265f / 180.0f;
    const float aspect = (float)ntr::SCREEN_W / ntr::SCREEN_H;
    const float f = 1.0f / tanf(fovy * 0.5f);
    const float zn = 3.0f / 8, zf = 25600.0f / 8;
    float P[16] = {f / aspect, 0, 0, 0,
                   0, f, 0, 0,
                   0, 0, (zf + zn) / (zn - zf), -1,
                   0, 0, 2 * zf * zn / (zn - zf), 0};

    float M[16];
    for (int r = 0; r < 4; ++r)
        for (int c2 = 0; c2 < 4; ++c2) {
            float s = 0;
            for (int k = 0; k < 4; ++k) s += V[r * 4 + k] * P[k * 4 + c2];
            M[r * 4 + c2] = s;
        }
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    for (int i = 0; i < 16; ++i)
        NTR_MMIO(uint32_t, 0x04000458) = (uint32_t)(int32_t)(M[i] * 4096.0f);
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
}

/* fault_probe.h reads this into crash.txt/exit.txt as the death's frame
   number; -1 is its weak fallback for harnesses without a frame loop. */
extern "C" int port_last_frame = -1;

/* The buffered stdout's last mile (see the setvbuf note in main). The exit
   probe's RtlExitUserProcess detour ends every orderly exit in
   NtTerminateProcess, which skips the CRT's own stream teardown -- measured
   as the end-of-run census and the "selftest:" line simply missing from a
   captured stdout. atexit callbacks run inside exit() BEFORE that detour
   fires, so this one gets the buffer out on every return from main. */
static void stdout_flush_atexit(void) { fflush(stdout); }

/* The host program entry point (window + ntr bring-up + frame loop). It
   name-collides with the ROM's boot spine src/main.c, which is the DS init
   sequence and runs as its own decomp TU, not as this launcher shell. The
   machine-read PORT_HOST_ABI tag sits directly above int main(void) below so
   linkage.py's reason binder reaches it. */
/* ---- startup_error.txt: the only channel a failed START has ----------------
   A crash mid-session leaves crash.txt, a playlog and a report the player can
   send. A failure BEFORE the window opens leaves none of that: the process
   exits cleanly, so there is no dump, and the launcher only ever saw an exit
   code. That is how the fixed-address failure in ntr/io.cpp reached us as four
   lines of stderr and nothing else, and why one player could not start the game
   at all with nothing on screen to say why.

   So a startup failure writes one plain-language file next to the exe. The
   launcher reads it after a non-zero exit and shows it (SM64DSLauncher's
   MainForm), which is the same shape as the extraction failure path: the child
   says what went wrong in words, the launcher is the thing with a window to put
   them in. Raw Win32 and a static buffer, the same discipline crash.txt and
   exit.txt keep, because this runs on a path where the process is already
   known to be in trouble. */
static void port_startup_error_path(char *path, unsigned cap)
{
    DWORD n = GetModuleFileNameA(0, path, cap);
    while (n && path[n - 1] != 92 /* '\\' */)
        --n;
    /* run mg16 lane MP2: SM64DS_INSTANCE suffixes this so a second copy of the
       game on the same machine cannot clear or overwrite the first copy's
       startup error. Unset, the name is unchanged. See hal/instance_tag.h. */
    _snprintf(path + n, cap - n, "startup_error%s.txt", port_instance_tag());
    path[cap - 1] = 0;
}

static void port_startup_error_clear(void)
{
    char path[MAX_PATH + 32];
    port_startup_error_path(path, sizeof path);
    DeleteFileA(path);
}

static void port_startup_error_write(const char *text)
{
    char path[MAX_PATH + 32];
    HANDLE f;
    port_startup_error_path(path, sizeof path);
    f = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL, 0);
    if (f != INVALID_HANDLE_VALUE) {
        DWORD wr;
        WriteFile(f, text, (DWORD)lstrlenA(text), &wr, 0);
        FlushFileBuffers(f);
        CloseHandle(f);
    }
}

/* Also say it on screen, for the player who double-clicked the exe instead of
   using the launcher. user32 is loaded here rather than imported, for the same
   reason winapi_load below does it: a static import chain maps the desktop heap
   before main and that mapping can land in the DS ranges. By the time this runs
   the reservation has already been decided, so loading it now costs nothing.
   SM64DS_NO_DIALOG=1 suppresses the box for automated runs, which must never
   block on a modal nobody is there to dismiss. */
static void port_startup_error_show(const char *text)
{
    HMODULE u32;
    int (WINAPI *mb)(HWND, LPCSTR, LPCSTR, UINT);
    if (getenv("SM64DS_NO_DIALOG") || getenv("SM64DS_WINDOW_SELFTEST"))
        return;
    u32 = LoadLibraryA("user32.dll");
    if (!u32)
        return;
    mb = (int(WINAPI *)(HWND, LPCSTR, LPCSTR, UINT))
         GetProcAddress(u32, "MessageBoxA");
    if (mb)
        mb(0, text, "Super Mario 64 DS could not start", 0x10 /* MB_ICONERROR */);
}

/* ---- DOES THIS SCENE RUN GET A WINDOW? (port mod, run link60 SW1) ---------

   A scene run is WINDOWED unless it names a frame budget or is a selftest.

   The tempting rule is "a scene that is not a selftest gets a window", and it
   is wrong here for a measured reason: port/tools/battery.py's scene_env DOES
   NOT SET SM64DS_WINDOW_SELFTEST. It sets SM64DS_SCENE, SM64DS_SCENE_FRAMES
   and SM64DS_FAULTS_FATAL, and pops every other scene knob so an inherited
   variable cannot decide what the code under test does. Under that rule every
   battery scene row would open a window and hal/sub_screen.cpp's g_headless
   would go false under all of them -- which puts the live mouse into
   poll_touch and a machine-global TAB into the panel toggle, in the one place
   that has to be deterministic. That is the exact non-determinism g_headless
   was added for, arriving through the door it was built to shut.

   A FRAME BUDGET IS A MEASUREMENT AND ITS ABSENCE IS A SESSION. Nobody sitting
   down to play curling says how many frames to play it for, and no automated
   run in this tree omits the number: counted rather than asserted, all 24
   command lines in port/ that set SM64DS_SCENE set SM64DS_SCENE_FRAMES on the
   same line, and battery.py's scene_env sets it for every scene row. The
   SM64DS_SCENE mentions that do not are prose about the variable rather than
   commands. port/scene_window.txt section 3b has the per-file count.

   So NEITHER SIDE NEEDED AN EDIT. The battery stays headless because it always
   named a count, and DBG1's minigame picker gets a window because it never
   did: it sets SM64DS_SCENE and SM64DS_DUAL_SCREEN and nothing else.

   SM64DS_SCENE_WINDOW=1/0 overrides, and it is not decoration. It is the only
   way to give a WINDOWED run a frame budget, which is the only way a pad or a
   touch claim about this path can be proved from a script instead of by hand:
   =1 with SM64DS_SCENE_FRAMES and SM64DS_PAD_TEST is a live window, driven by
   a script, that ends by itself. It can never turn a window ON under a
   selftest -- that floor is checked first, and it is the same floor
   SM64DS_PAD_TEST has.

   port/scene_window.txt is the derivation. */
static int port_scene_want_window(void)
{
    if (getenv("SM64DS_WINDOW_SELFTEST"))
        return 0;
    {
        const char *e = getenv("SM64DS_SCENE_WINDOW");
        if (e) return atoi(e) != 0;
    }
    return getenv("SM64DS_SCENE_FRAMES") == 0;
}

/* ---- A WINDOWED SCENE RUN (port mod, run link60 SW1) ----------------------

   The same window, the same present, the same input and the same debug menu
   the level path has, put around hal/scene_boot.cpp's own frame. Nothing about
   the scene's bring-up, its capture or its census is duplicated here: those
   are port_scene_begin, port_scene_tick and port_scene_finish, and
   port_scene_run composes the identical three for a headless run.

   THE SCENE'S FRAME IS THAT FILE'S AND THE HOST'S FRAME IS THIS ONE'S, which
   is the division main's level loop already has. What this function owns is
   the message pump, the DS keypad words, the debug menu, the stacked compose,
   the present, the hosted ARM7 pump and the pacer.

   THE STYLUS NEEDED NO CODE AT ALL. hal_sub_screen_frame_begin has been
   calling poll_touch on the scene path every frame all along, and poll_touch's
   stacked branch is the mapping Tango click-tested on levels -- the bottom
   half of the picture IS the bottom screen, so a click anywhere in it lands on
   the corresponding DS pixel with no fudge term (ntr/include/ntr/ppu.h refuses
   a hinge row for exactly that reason). What held it shut was
   hal_sub_screen_init_hw's `g_headless = ... || hwnd == nullptr`, and
   port_scene_begin now has a real window to pass it.

   THE FLIGHT RECORDER AND THE CRASH FILES WERE ALREADY HERE, because main
   opens the playlog and installs the fault probe ABOVE the scene handover. The
   one thing missing was the frame number: port_last_frame is this file's
   counter and nothing on the scene path fed it, so a scene crash reported
   frame -1. This loop feeds it. The headless loop deliberately still does not
   -- it is the battery's, and it is left alone. */
/* THE WINDOW A TITLE-ENTRY RUN HANDS FORWARD (run lvled).
   A windowed title run that ends in a save-file pick does not end the process:
   main falls through to its own level boot. That boot opens a window of its
   own, and without this the title's window would be left standing while a
   second one appeared beside it -- two windows for one game, the first of them
   dead. The player picked a file; he should keep looking at the same window.

   Recorded unconditionally and read only when port_title_entry_taken() says
   the fall-through happened, so an ordinary scene session sets two statics
   nobody reads. The layout was latched by the scene runner (it is the one that
   proposes), which is why main's own latch comment still holds: the window is
   created once and the mode with it. */
static HWND g_entry_hwnd;
static HDC  g_entry_hdc;

static int scene_window_run(void)
{
    /* THE LAYOUT FIRST, because the window has to be sized for the picture it
       is going to show and the mode latches on its first reader. The proposal
       is hal/scene_boot.cpp's -- the ROM's own IsMinigameActorID -- asked here
       rather than invented here, and port_scene_begin's second ask is a no-op
       so the setter never reports a late write that is in fact correct. */
    port_scene_layout_propose();
    const int stacked = hal_sub_screen_stacked();

    HDC hdc = 0;
    HWND hwnd = host_window_open(
        stacked, &hdc,
        "SM64DS   |   stylus = left mouse drag   Space jump   X punch"
        "   Ctrl crouch   |   arrows / d-pad   Enter start"
        "   |   F5 or Esc menu   F12 fullscreen");
    if (!hwnd) {
        /* A window that will not open is not a reason to lose the run: the
           scene still boots, still ticks and still writes whatever capture it
           was asked for. Loud, because a silent fall back to headless is how a
           play session becomes three hundred frames of nothing that nobody can
           account for afterwards. */
        fprintf(stderr, "[scene] the window did NOT open (win32 %lu); falling "
                "back to the headless run\n", (unsigned long)GetLastError());
        return port_scene_run();
    }
    /* kept for a title-entry fall-through; see the banner above */
    g_entry_hwnd = hwnd;
    g_entry_hdc = hdc;

    const int rc = port_scene_begin(hwnd, ZOOM);
    if (rc)
        return rc;

    /* THE PRESENT PATH IS ARMED AFTER THE SPAWN and not before it, the same
       order the level loop uses: until a frame has been rasterised the
       framebuffer is blank, and a WM_PAINT answered before then paints grey.
       From here on a WM_SIZE or a WM_PAINT can redraw without this loop's
       help, which is what a drag on the sizing border needs. */
    g_present_hwnd = hwnd;
    g_present_hdc = hdc;
    g_present_bi = &g_bi;
    g_present_fb = (const ntr::Framebuffer *)port_scene_framebuffer();
    ntr::Framebuffer &fb = *(ntr::Framebuffer *)port_scene_framebuffer();

    /* A BUDGET ONLY IF ONE WAS NAMED. With no SM64DS_SCENE_FRAMES this runs
       until the window closes, which is what a session is; with one -- which
       takes SM64DS_SCENE_WINDOW=1 to reach this function at all -- it ends by
       itself, which is what a scripted proof needs. */
    const int budget = getenv("SM64DS_SCENE_FRAMES")
                           ? port_scene_frames_wanted() : 0;
    /* THE IMAGE'S shape, which by this line is the scene's own: the window
       opened before the scene latched its screen gap and is still at the
       gapless size, and the first frame's stack_present_arm grows it to match
       and says so on its own line. */
    int wsw = ntr::STACK_W, wsh = ntr::STACK_H;
    if (stacked) hal_sub_screen_stacked_size(&wsw, &wsh);
    fprintf(stderr, "[scene] WINDOWED %dx%d, %s, %s\n",
            stacked ? wsw : ntr::SCREEN_W * ZOOM,
            stacked ? wsh : ntr::SCREEN_H * ZOOM,
            stacked ? "STACKED (both DS screens, stylus over the bottom half)"
                    : "corner inset panel",
            budget ? "frame budget set" : "runs until the window closes");
    fflush(stderr);

    /* SM64DS_SCENE_MENU=<frame>: open the debug menu on this path, at that
       frame, without a hand on F5.
     *
     * SM64DS_MENU is read in main() and main() is the LEVEL path; a scene run
     * hands over before it, so the documented "boot with the menu open" knob
     * has never reached the one path where the menu's own screen bug lives.
     * A FRAME rather than a plain flag because opening it at 0 would freeze
     * the scene before it had booted -- menu_on pauses the game tick -- and a
     * capture of a scene that never ran is a capture of nothing.
     *
     * WHAT IT DOES NOT PAUSE is the display, and that is why the flashing is
     * reproducible under it: slot 24 rides the RENDER path, not the behavior
     * one, so POWCNT1 bit 15 keeps toggling while the menu holds the world
     * still. Measured -- scene 384, 900 frames: behavior 853, slot24 900. */
    int scene_menu_at = -1;
    if (const char *e = getenv("SM64DS_SCENE_MENU")) {
        scene_menu_at = atoi(e);
        if (scene_menu_at < 0) scene_menu_at = 0;
        fprintf(stderr, "[scene] SM64DS_SCENE_MENU: the debug menu opens at "
                "frame %d\n", scene_menu_at);
    }

    int frame = 0, focus_was = 1, quit = 0;
    MSG msg;
    static XPad pad;
    while (!quit) {
        if (scene_menu_at >= 0 && frame == scene_menu_at) menu_on = 1;
        while (W.PeekMessageA_(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { quit = 1; break; }
            W.TranslateMessage_(&msg);
            W.DispatchMessageA_(&msg);
        }
        if (quit)
            break;
        /* the focus edge, read once a frame BEFORE any key is. Coming back,
           every key starts stale; going away needs no work, because key_live
           is already returning released. */
        {
            const int now = hal_window_focused();
            if (now && !focus_was) memset(key_stale, 1, sizeof key_stale);
            focus_was = now;
        }
        {
            static int fs_edge;
            const int now = key_live(VK_F12) || key_live(VK_F11);
            if (now && !fs_edge) fullscreen_toggle(hwnd);
            fs_edge = now;
        }

        int pad_live = XInputGetState_ && XInputGetState_(0, &pad) == 0;
        pad_test_apply(frame, &pad_live, &pad);
#ifndef PORT_ROM_CLEAN
        /* SM64DS_CLICK_TEST: the scripted stylus, driven BEFORE the tick that
           polls it, so a press is in the OS's button state by the time
           hal_sub_screen_frame_begin reads it on this same frame. */
        click_test_apply(hwnd, frame);
#endif

        /* THE DEBUG MENU, the same block main runs. g_menu_host is left zeroed
           on this path, which is what makes the four rows that need a Player
           refuse in words instead of writing through a null -- and what leaves
           the MINIGAME row working, because a relaunch needs nothing from the
           loop it was started in. */
        menu_input(pad_live, &pad);
        menu_b_swallow_spend(pad_live, &pad);

        /* THE DS KEYPAD, written every frame so a release is a release. The
           four shared bits plus the d-pad, Start and Select.

           THE LEVEL PATH HAS NEVER NEEDED THOSE LAST THREE and that is why
           they are here rather than in host_ds_buttons: a level walks off the
           analog stick, and the d-pad and Start are what a minigame's own
           menus are built on. Select is keyboard-only (backspace) because
           every free pad button is already spoken for -- BACK opens the debug
           menu, and a Select that also opened the menu would be a trap.

           Zeroed while the menu is open: enter and A belong to the menu, not
           to the scene, exactly as the level loop zeroes it. */
        {
            static unsigned short btn_was;
            unsigned short btn = 0;
            if (!menu_on) {
                btn = host_ds_buttons(pad_live, &pad);
                if (key_live(VK_RIGHT))  btn |= 0x10;
                if (key_live(VK_LEFT))   btn |= 0x20;
                if (key_live(VK_UP))     btn |= 0x40;
                if (key_live(VK_DOWN))   btn |= 0x80;
                if (key_live(VK_RETURN)) btn |= 0x08;   /* Start  */
                if (key_live(VK_BACK))   btn |= 0x04;   /* Select */
                if (pad_live) {
                    if (pad.buttons & 0x0008) btn |= 0x10;   /* d-pad right */
                    if (pad.buttons & 0x0004) btn |= 0x20;   /* d-pad left  */
                    if (pad.buttons & 0x0001) btn |= 0x40;   /* d-pad up    */
                    if (pad.buttons & 0x0002) btn |= 0x80;   /* d-pad down  */
                    if (pad.buttons & 0x0010) btn |= 0x08;   /* START       */
                }
            }
            /* run mg16 lane MPBTN: the host key word for the scene path's
               publisher (hal/scene_boot.cpp's port_scene_comms_publish),
               refreshed every frame from the SAME host state the store below
               uses -- so the title's key word comes from the keyboard and the
               pad, never from a record something else may be filling. This
               word is MIXED convention by construction: host_ds_buttons'
               four bits are Ctrl-convention and go through the translator;
               the d-pad, Start and Select added above are already raw DS bits
               (0xf0, 0x08, 0x04) and pass straight through. */
            port_host_keys_set((unsigned short)(host_btn_to_raw_keys(btn) |
                                                (btn & 0x00fc)));
            /* SLOT 0 ONLY, AND ONLY WHEN THE ROM'S FAN-OUT IS NOT DRIVING.
               Run mg16 lane MP3, field failure 2. This publishes the LOCAL
               buttons into Ctrl slot 0's held and pressed words, which is
               right for a single-player port and is the crouch bleed in a
               session: on the CHILD it put the child's own buttons into the
               HOST's Ctrl record, so crouch pressed in the child's window
               crouched MARIO in the child's world -- and only in that world,
               because the host was never told. Same shape as the PadData[0]
               clobber and gated the same way.

               With a transport up, these words come from the ROM's own path
               instead: the key register, the local comms record, the wire, the
               fan-out into all four PadData slots, Stage::CheckInput into all
               four Ctrl records, and the per-player split-symbol copy further
               down this file. The local player's buttons still arrive -- into
               the slot this console actually is. */
            if (!(port::comms_transport() && comms_fanout_on())) {
                *(unsigned short *)(data_0209f49c + 0) = btn;
                *(unsigned short *)(data_0209f49e + 0) =
                    (unsigned short)(btn & (unsigned short)~btn_was);
            }
            btn_was = btn;
        }

        /* the scene's own frame; the menu's pause is its second argument, the
           same switch the level loop's game_ticked is */
        port_scene_tick(frame, !menu_on);

        /* THE STACKED IMAGE IS BUILT BEFORE THE OVERLAYS, and the order is the
           whole of this lane's change on this path. Every line ABOVE this one
           that writes a pixel writes it into fb -- the raster, the engine-A
           composite -- and the stacked image is a copy of the finished fb with
           the bottom screen under it, so the compose still runs on a finished
           frame. What moved is the MENU and the TOAST: they are host UI and
           they belong to the UPPER PHYSICAL SCREEN, and painting them into fb
           gave them engine A's affinity instead, which the display swap then
           carried into the wrong half. Nothing happens in the inset layout; the
           compose returns 0, the surface falls back to fb and present() keeps
           reading fb, exactly as before. */
        uint32_t *stack_img = stacked
                ? hal_sub_screen_stacked_image(&fb.px[0][0]) : 0;
        const OvlSurface surf =
            stacked ? ovl_surface_stacked(stack_img, fb) : ovl_surface(fb);

        if (menu_on) menu_draw(surf);
        toast_draw(surf);

        if (stacked)
            stack_present_arm(stack_img, hwnd);
        present();
        /* the click flag is true for exactly the frame it landed on; the hold
           in g_mouse_left_down is what outlives it */
        g_mouse_click_new = 0;
        /* THE HOSTED ARM7, EXACTLY ONCE A FRAME -- and port_scene_tick above
           has already done it on every frame that ticked the game, so this
           call is only for the frames that did not.

           IT WAS RUNNING TWICE. This call landed first (6482ce682, the scene
           path's window) and hal/scene_boot.cpp:2763 added a second one inside
           port_scene_tick the next day (5264b7bea, the minigame music seat)
           for the headless path, which had none. Both then ran on every
           windowed frame: 600 frames of scene 368 measured 1201 pushes through
           the waveOut layer. Tempo survived it -- hal/sdat/mixer.cpp clocks the
           sequencer off the AUDIO clock at 192 Hz on purpose -- but the ARM9
           sound frame (func_0204fafc's volume ramps, the player-status
           publish) stepped twice per video frame, and once the pacer above
           stopped running minigames at half speed that became four times the
           rate the DS runs it at.

           NOT DELETED OUTRIGHT, because port_scene_tick's copy is gated on
           tick_game and the debug menu clears it. A paused scene should not
           advance the sequencer, but it must still hand the device buffers or
           the 125ms ring drains and the speaker gets whatever the hardware
           repeats. So: the game's frames are pumped there, the paused frames
           are pumped here, and no frame is pumped twice. */
        if (menu_on) sdat_host_tick();
        ++frame;
        port_last_frame = frame;   /* fault_probe.h: crash.txt/exit.txt context */
        fflush(stdout);
        if (budget && frame >= budget)
            break;
        /* THE TITLE HANDOFF (hal/title_entry.cpp), tested here for the same
           reason the headless bridge tests it after its tick: the title writes
           the request from inside port_actor_tick, so the words are only
           settled once the frame has run. Answers 0 unless SM64DS_TITLE_ENTRY
           is set on a title run, so this costs an unarmed session one compare
           of a cached int. */
        if (port_title_entry_should_stop()) {
            fprintf(stderr, "[title-entry] a save file was picked; leaving the "
                            "title after %d frame(s)\n", frame);
            break;
        }
        /* THE PACE, off the scene's own divider and not off a constant. A
           minigame writes data_0208ee44 = 1 in its InitResources and therefore
           runs at 60; the 33.3ms this block used to hardcode is the 3D level
           path's number, and pacing a minigame to it ran the whole minigame at
           EXACTLY HALF SPEED. See frame_pace's banner. */
        frame_pace();
    }
    /* THE ORDINARY EXIT. A window close, an Esc, or the minigame row's own
       PostQuitMessage all arrive here the same way, and the census below is
       what the run leaves behind -- the same lines a headless run ends with,
       over however many frames were actually played. */
#ifndef PORT_ROM_CLEAN
    click_test_finish();
#endif
    fprintf(stderr, "[scene] window closed after %d frame(s)\n", frame);
    const int scene_rc = port_scene_finish(frame);
    /* AFTER the census, so a run that enters the adventure still leaves the
       title's own slot hits, captures and trap counts behind. Answers 0 and
       prints nothing unless the bridge is armed AND the handoff completed. */
    port_title_entry_commit();
    return scene_rc;
}

/* PORT_HOST_ABI: the host program entry point (window + ntr bring-up + frame
   loop). Name-collides with the ROM's boot spine src/main.c, which is the DS
   init sequence and runs as its own decomp TU, not as this launcher shell. The
   host process needs its own main() to open the window and pump frames; the
   ROM's void main(void) links as a decomp TU that the host boot path calls,
   not as the process entry point. */
int main(void)
{
    /* fault_probe.h has been included here since gate 4 and was never armed,
       so every crash in the window build printed nothing at all. It costs
       nothing until something faults, and it prints a module-relative address
       the .map file resolves. The _with_file form also writes crash.txt next
       to the exe with raw Win32, so a run without a captured stderr still
       leaves the address behind. */
    SetUnhandledExceptionFilter(port_fault_probe_with_file);
    /* world = KCL file x64. Default spawn: north end of the stone
       bridge (deck ~892), facing the walk south across it -- the shot
       that calibrates against real-game footage. Roof surface = 4916,
       lawn = 784 (SM64DS_SPAWN overrides, world units). */
    int spawn_x = 0, spawn_y = 960, spawn_z = 1000;
    /* THE PHYSICS SCAFFOLDING IS RETIRED. It existed for exactly one reason:
       WithMeshClsn's continuous update finds a floor two ways, the swept head
       segment (RaycastLine, hosted, and real since gate 15 gave it a real
       prev position) and the SPHERE, and the sphere is what holds a STANDING
       actor up. With MeshCollider::DetectClsn(SphereClsn &) stubbed the
       Player sank about vo before the head sweep crossed the floor plane and
       shoved him back, which read as a 28-unit bob at 20 Hz that never
       settled, so the harness ground snap and wall clamp had to stay on.

       The sphere pass is hosted now (port/unmatched/
       MeshCollider_DetectClsn_Sphere.cpp), and standing is exact: the same
       idle spot holds one value for 600 frames, and a 2700-unit drop lands
       and stops dead where the same drop under the stub bobbed 912/940/927
       or sank 439 -> 373 over 200 frames.

       SM64DS_FAKE_SNAP=1 brings the whole old configuration back in one
       switch -- the level collider owned by the Player, which makes the
       game's own ground tracking a no-op (see the Enable call below), plus
       the snap and the wall clamp on top. Kept for the A/B and for shots
       that need Mario planted regardless. */
    const int fake_snap = getenv("SM64DS_FAKE_SNAP") != 0;
    /* the per-frame half of it is a switch now, so the F5 menu can do the A/B
       without a restart; the collider-owner half below is still boot-time */
    g_fake_snap = fake_snap;
    PORT_INSTALL_FAULT_PROBE();
    port_install_watchdog();
    /* PORT_WATCH_MCC=1: who writes MovingCylinderClsn's GetPos/GetOwnerID
       slots. This is the watch that caught SetPlayerGlobals' controller
       loop straying out of a two-byte data_0209f4ae into the vtable
       (hal/actor_vtables.cpp has the full story). Kept armed for the next
       time a layout shift puts something else behind an undersized host
       global. */
    if (getenv("PORT_WATCH_MCC"))
        port_watch_words(&_ZTV18MovingCylinderClsn[2], 2);
    /* PORT_WATCH_TRACKER=1: who writes the particle tracker pointer. On the
       direct Bob-omb Battlefield boot it read 0x0007f000 by frame 0 after a
       healthy [fx] boot line, which is this same stomp class again. */
    if (getenv("PORT_WATCH_TRACKER"))
        port_watch_words(&data_0209ee74, 1);
    /* STDOUT IS FULLY BUFFERED, ON PURPOSE. It was unbuffered here for years,
       and that made every printf its own blocking WriteFile against whatever
       the sink is -- 2-6ms a line on a healthy console, worse on a degraded
       one, forever if QuickEdit has a selection holding the console lock. A
       level entry prints ~248 lines (the census, the spawn-skip lines, the
       [clsn] CLPS dump), so the same 600-frame CCM selftest measured 2540ms
       to a file, 7579ms to a console, and 22s to an undrained pipe. The 64KB
       full buffer decouples the game loop from sink latency: the boot's
       lines go out in ONE write at the fflush below the boot block, and the
       frame loop flushes once per frame (one write, not hundreds).
       What this does NOT buffer: stderr. Every FATAL, fault dump and trace
       goes to stderr, which stays unbuffered (the flight recorder below
       re-asserts that), so a hard crash still leaves the whole trail; the
       most a fault can strand in this buffer is the current frame's stdout,
       and orderly exits flush it through stdout_flush_atexit above (the CRT
       teardown flush never runs here: the exit probe's detour terminates
       first). */
    static char stdout_buf[1 << 16];
    setvbuf(stdout, stdout_buf, _IOFBF, sizeof stdout_buf);
    atexit(stdout_flush_atexit);
    /* FLIGHT RECORDER (Tango's ask): every diagnostic this program
       writes to stderr -- unhosted states, spawn skips, fault dumps
       with registers and stack, the traces below -- lands in a
       timestamped file under playlog/, unbuffered so a hard crash
       still leaves the trail. Read the newest file after a play
       session to see what led into a glitch. SM64DS_NO_PLAYLOG=1
       keeps stderr on the console instead. */
    if (!getenv("SM64DS_NO_PLAYLOG") && !getenv("SM64DS_WINDOW_SELFTEST")) {
        CreateDirectoryA("playlog", NULL);
        char *logname = g_playlog;
        SYSTEMTIME st_;
        GetLocalTime(&st_);
        snprintf(logname, sizeof g_playlog,
                 "playlog/play_%04u%02u%02u_%02u%02u%02u.log", st_.wYear,
                 st_.wMonth, st_.wDay, st_.wHour, st_.wMinute, st_.wSecond);
        if (freopen(logname, "w", stderr)) {
            setvbuf(stderr, NULL, _IONBF, 0);
            printf("flight recorder: %s\n", logname);
            fprintf(stderr, "[recorder] session start\n");
        }
    }
    /* A stale file from an earlier run must never be read as this run's verdict.
       Clear it before the decision, write it only if the decision goes badly. */
    port_startup_error_clear();
    if (!ntr::io_init()) {
        /* THE FAILURE A PLAYER USED TO GET IN SILENCE. Three places now, all of
           them cheap because none of it runs unless the start already failed:
           the technical block goes to the log for us, the plain sentence goes
           in a file for the launcher, and the same sentence goes on screen for
           someone running the exe directly. */
        fprintf(stderr, "io_init failed\n");
        fputs(ntr::io_reserve_detail(), stderr);
        {
            const char *say = ntr::io_reserve_player_text();
            if (say[0]) {
                port_startup_error_write(say);
                port_startup_error_show(say);
            }
        }
        return 2;
    }
    /* Which stage actually won the fixed ranges, and how many passes it took.
       One line, always, because "it wins more often now" is not a claim a log
       can be read for afterwards, and the stage number is. 1 = the TLS callback
       at process start, 2 = here in main, which is where it used to happen. A
       stage 2 on a healthy machine means the early claim lost and the retry
       rescued it, which is worth seeing in a player's log. */
    fprintf(stderr, "[io] fixed ranges: stage %d, %u attempt(s), lost %02x\n",
            ntr::io_reserve_stage_won(), ntr::io_reserve_attempts(),
            ntr::io_reserve_lost_mask());
    /* A start that SURVIVED a lost range still needs the record. Losing main
       memory is not fatal, but it is the difference between two runs of the same
       build and it is the shape a later mystery crash grows out of, so the same
       block that a fatal loss prints goes in the log here too. */
    if (ntr::io_reserve_lost_mask()) fputs(ntr::io_reserve_detail(), stderr);
    if (!winapi_load()) { fprintf(stderr, "winapi_load failed\n"); return 2; }
    /* run mg16 lane MP2: the loopback carrier, if and only if SM64DS_COMMS_ROLE
       names a role. With the env unset this installs nothing and returns false,
       the seam keeps its own solo answers, and every path below is the one that
       ran yesterday. Placed after io_init because the seam's boot indicator
       writes through the 0x027ff000 mapping io_init reserves. */
    port::comms_loopback_install_from_env();
    pacer_begin();
#ifdef PORT_ROM_CLEAN
    /* ROM-CLEAN: load + verify the ROM tables from romdata.bin FIRST, before
       any table is read. Loud FATAL if the file is missing, short, or fails its
       manifest sha. The pointer-rebase passes below (port_ov002_patch,
       port_cross_patch, the overlay syms patches) then run over the loaded
       bytes exactly as they would over baked-in ones. */
    port_romdata_load();
#endif
    /* SM64DS_DUMP_LEVEL_NAMES=1: print the debug level-select rows exactly as
       the menu's MENU_LEVEL row renders them -- row, id, name, entrance,
       overlay, mount state -- then exit. A standalone proof of the id->name
       mapping that needs no window or game boot (so it runs on a host where
       the selftest's fabricated PE is quarantined). Mounts are installed here
       first (the call is idempotent) so the MOUNTED column is the real one. */
    if (getenv("SM64DS_DUMP_LEVEL_NAMES")) {
        port_level_mounts_install();
        const int rows = port_title_rows();
        printf("[levelnames] %d rows in the debug level select\n", rows);
        for (int i = 0; i < rows; ++i) {
            int lv = 0, en = 0;
            if (!port_title_row(i, &lv, &en)) {
                const char *sc = lv == -1 ? "back to file select"
                               : lv == -2 ? "minigame menu" : "scene sentinel";
                printf("  row %2d/%d  <%s %d>\n", i, rows, sc, lv);
                continue;
            }
            const char *nm = level_short_name(lv);
            printf("  row %2d/%d  %2d %-20s  ent %2d ov%03d %s\n",
                   i, rows, lv, nm ? nm : "(unnamed)", en,
                   port_level_overlay_id(lv),
                   port_level_is_mounted(lv) ? "MOUNTED" : "not mounted");
        }
        fflush(stdout);
        return 0;
    }
    /* THE ROM'S OWN ENTRY, not one level in. This was
       `if (!_ZN4Heap13SetupRootHeapEv()) return 2;` -- correct, and calling the
       inner half. func_0201a054's chain reaches the root heap through
       Heap::InitializeRootHeap (arm9 0x0203cae8, 0x1c), which clears the OS
       globals word at 0x020a0ea4 and then TAIL-CALLS SetupRootHeap:

           0203cae8  e59f000c   LDR r0, [pc, #0xc]   -> 0x020a0ea4
           0203caec  e3a01000   MOV r1, #0
           0203caf0  e59fc008   LDR ip, [pc, #8]     -> 0x0203cb04
           0203caf4  e5801000   STR r1, [r0]
           0203caf8  e12fff1c   BX  ip

       THE GUARD MOVES RATHER THAN GOES. Because that last instruction is a
       tail call, the ROM hands SetupRootHeap's HeapS* straight back to this
       caller -- but src/_ZN4Heap18InitializeRootHeapEv.cpp declares the
       function `void`, so the return value cannot come through the seated TU.
       Testing data_020a0ea0 instead is the same test, not a weaker one:
       src/_ZN4Heap13SetupRootHeapEv.c writes the new heap into data_020a0e9c
       and data_020a0ea0 only on the success path and returns 0 without
       touching either on failure. Weakening the boot guard to seat a TU would
       be exactly the kind of non-ROM-faithful fix the port exists to refuse.

       Zeroing 0x020a0ea4 is inert here, which is why the heap numbers do not
       move: hal/os_arena.cpp's accessors take that word as their `g` argument
       and ignore it outright. It matters on the DS, where NULL means "use the
       default OS globals", and it costs nothing to be faithful about. */
    _ZN4Heap18InitializeRootHeapEv();
    if (!data_020a0ea0) return 2;
    memset(data_0209b3ec, 0, 48);
    data_0209b3ec[0] = data_0209b3ec[4] = data_0209b3ec[8] = 0x1000;
    hal_fill_model_vtable();
    hal_fill_shadow_vtable();
    hal_fill_mmc_vtable();
    hal_fill_modelanim2_vtable();

    port_ov002_patch();
    port_cross_patch();
    __sinit_ov002_02100560(); __sinit_ov002_02100938();
    __sinit_ov002_02100adc(); __sinit_ov002_02100c50();
    __sinit_ov002_02100d44(); __sinit_ov002_02100e50();
    __sinit_ov002_02100ec4(); __sinit_ov002_02100f84();
    __sinit_ov002_02101064(); __sinit_ov002_02101478();
    __sinit_ov002_021014e4(); __sinit_ov002_02101588();
    __sinit_ov002_02101738(); __sinit_ov002_02101894();
    __sinit_ov002_02101900(); __sinit_ov002_02101968();
    __sinit_ov002_021019d0(); __sinit_ov002_02106e40();
    __sinit_ov002_02107118(); __sinit_ov002_021071f4();
    __sinit_ov002_02107298(); __sinit_ov002_02107304();
    __sinit_ov002_02107370(); __sinit_ov002_02107f88();
    __sinit_ov002_0210804c(); __sinit_ov002_02108094();

    /* SM64DS_SCENE=<id> BOOTS A NON-LEVEL SCENE INSTEAD, and takes the run
       with it. Mirrors SM64DS_LEVEL: it names what to boot and nothing else.
       4 is the star select (ov003's dScStarSel_c), the only scene seated
       today; it comes up through the ROM's own Scene::SetSceneToSpawn ->
       Scene::SpawnIfNecessary -> spawn spine, and an unhosted id is refused
       by name the way an unmounted level is.
       The hand-over is HERE, at the end of the host bring-up and before the
       first level-shaped statement, because everything above this line (the
       fixed ranges, the root heap, the ov002 pointer pass and its static
       initialisers, the model vtable fills) is bring-up both modes need and
       everything below it reads the Player the entrance spawned -- which a
       scene run has not got. The scene runner owns the rest of the process;
       see hal/scene_boot.cpp.

       AND IT MAY OPEN A WINDOW NOW. port_scene_want_window above decides:
       a run that names a frame budget or is a selftest is a MEASUREMENT and
       takes the headless port_scene_run it always took, and everything else is
       a SESSION and takes scene_window_run, which drives the same three calls
       with a pump, live input, the debug menu and a present between the ticks.
       The battery names a count on every scene row and DBG1's minigame picker
       names none, so neither of them needed an edit for this. */
    /* SM64DS_TITLE_ENTRY=1 (hal/title_entry.cpp) IS THE ONE WAY THIS BRANCH
       DOES NOT OWN THE REST OF THE PROCESS. Armed, a title run stops on the
       ROM's own handoff -- a save file was picked, StartFile staged a level
       and asked for scene 3 -- and falls THROUGH to the level boot below,
       carrying the request the game itself wrote. Unarmed, and on every scene
       that is not the title, this is the same two lines it has always been.

       The fall-through is safe here for the reason the block above gives: this
       is the end of the host bring-up, everything below is the level's own,
       and a title run that has torn itself down has no Player to lose. */
    if (port_scene_env_want() >= 0) {
        const int scene_rc =
            port_scene_want_window()
                ? scene_window_run()          /* carries the stop test itself */
                : (port_title_entry_armed() ? port_title_entry_run()
                                            : port_scene_run());
        if (!port_title_entry_taken())
            return scene_rc;
        fprintf(stderr, "[title-entry] scene run over; falling through to the "
                        "level boot in this process\n");
    }

    /* THE GAME'S OWN LEVEL BOOT: ov009 mounted,
       Stage::LoadClsnAndObjects run against it, and the level's own entrance
       record spawning the Player and the Camera.

       IT IS NO LONGER "THE DEFAULT", and that phrase used to be here. Since
       the owner's boot-to-title ruling this block is reached three ways --
       SM64DS_LEVEL naming a level, SM64DS_VS_MAP naming a VS match, or the
       SM64DS_BOOT_CLASSIC opt-out -- plus the one that matters most, a title
       run FALLING THROUGH after a save file was picked, which is the branch
       just above. A launch with no environment at all goes to the title now;
       hal/title_entry.cpp's port_boot_default_scene is where that is decided
       and its banner carries the whole derivation.

       SM64DS_LEGACY_BOOT=1 goes
       back to the harness staging (hand-built spawn context, KCL mounted by
       hand); SM64DS_BOOT_NOSPAWN=1 holds the entrance table off, which is
       stage A1, the same boot with nothing spawning. SM64DS_REAL_BOOT is
       still accepted and is now a no-op. */
    const int real_boot = getenv("SM64DS_LEGACY_BOOT") == 0;
    const int boot_spawns = real_boot && getenv("SM64DS_BOOT_NOSPAWN") == 0;
    if (real_boot) {
        /* [lvl-perf] span 3: the boot-dump printf blocks, timed apart from
           the boot so time-inside-printf is its own number */
        const double t0 = port_lvlperf_now();
        port_level_probe();
        port_lvlperf_note(3, port_lvlperf_now() - t0);
    }
    /* SM64DS_MENU=1 opens the debug menu at boot, which is the only way to
       see it in a selftest frame: a selftest reads no live keys, so F5 never
       arrives. It pauses the tick like any other open menu. */
    menu_on = getenv("SM64DS_MENU") != 0;

    data_02092144[0] = 8 << 8;
    if (!boot_spawns) {
        /* the fake spawn context: actor id 0 with invented priorities, which
           is what the hand-built Player was constructed under. The real boot
           reads the ROM's own SpawnInfo out of the registry instead. */
        data_020a4b54 = 0;
        static unsigned short spawn_info[4] = {0, 0, 100, 100};
        data_020a4bb8[0] = spawn_info;
    }
    /* THE GAME HEAP, the ROM's own chain instead of an alias.
       This line used to be `data_020a0eac_c = data_020a0ea0;` -- the game heap
       word pointed straight at the root heap, so every allocation the game made
       came out of the whole host arena and the ROM's own heap object never
       existed. func_0201a054, the main.c boot spine, does this instead:

           0x0201a100  e3a00a3b   MOV r0, #0x3b000
           0x0201a104  e3a01000   MOV r1, #0
           0x0201a108  eb00884f   BL  0x0203c24c   Heap::InitializeGameHeap

       so the size is a hard immediate, 0x3b000 = 241664 bytes, and the parent
       is NULL. Nothing is derived from the OS arena, which is why there is
       nothing here for the port to invent: the arena only decides how big the
       ROOT heap is (Heap::SetupRootHeap above), and the game heap is a fixed
       carve out of that. Heap::CreateExpandingHeap resolves the NULL parent to
       data_020a0ea0, takes size+0x18 from it, builds the allocator over the
       0x3b000 payload and runs the ExpandingHeap constructor over the header.
       Both bodies are src/, byte-matched, and reached by name from here. */
    _ZN4Heap18InitializeGameHeapEjPS_(0x3b000, 0);
    if (!data_020a0eac_c) {
        /* Heap::Allocate Crash()es on a failed carve when the parent's flag
           word has 0x4000 set, which Heap::Heap sets, so a null here means
           CreateExpandingHeapAllocator refused the span rather than the root
           heap running dry. Either way the game has no heap and every later
           allocation is undefined; stop while the reason is still on screen. */
        fprintf(stderr, "InitializeGameHeap returned null -- no game heap\n");
        return 2;
    }
    /* One line of evidence that 0x3b000 is the right number for the port and
       not just for the DS: the free space in the game heap right after the
       carve, and again at the end of a selftest. The DS budget is the budget. */
    fprintf(stderr, "[heap] game heap %p, 0x%x bytes, %u free after boot\n",
            data_020a0eac_c, 0x3b000u,
            _ZN22ExpandingHeapAllocator10MemoryLeftEv(
                *(void **)((char *)data_020a0eac_c + 0x14)));

    /* Game mode 0 (adventure) -- LoadClsnAndObjects branches its minimap
       and HUD spawns on this, and Stage::CheckInput reads it later. */
    data_0209f2d8 = 0;

    /* ---- VS wiring lane: THE VS BOOT --------------------------------------
       SM64DS_VS_MAP=<0..3> makes this boot a VS match on the ROM's own map
       list. The ORDER IS THE ROM'S (port/slice_vs.txt section 4): one byte
       (the lobby's own map pick) and one matched call, func_ov075_02116c8c,
       which runs PrepareVsMode -- data_0209f2d8 back to 1, SetPlayerGlobals,
       StartSceneFade(3), the save defaults -- then LoadLevelNoReturn(map, 0,
       2, 0) and the music stop. Placed AFTER the adventure default above so
       the ROM's own mode write is the one that stands, and after the game
       heap so the staged path allocates like the DS does.

       The staged level then flows through the port's ONE latch
       (port_level_entry_latch, the title-entry seam) into the boot target,
       and the scene-3 request is released because the level path is serving
       it -- the same two moves hal/title_entry.cpp makes for StartFile. The
       entrance and star filter the ROM staged (0 and 2) are re-asserted via
       the env the direct boot re-seats from, so the boot's own defaults
       cannot overwrite the ROM's values. */
    {
        const char *vsm = getenv("SM64DS_VS_MAP");
        if (vsm && vsm[0]) {
            /* the mount registry fills on the level boot below; asking
               is_mounted before it is filled refuses every map (measured on
               the first proof run). The call is idempotent. */
            port_level_mounts_install();
            const int mi = atoi(vsm) & 3;
            const int vlv = port_vs_map_level(mi);
            const char *vmode = getenv("SM64DS_VS_MODE");
            if (vmode && vmode[0] && atoi(vmode) != 0)
                fprintf(stderr, "[vs] SM64DS_VS_MODE=%s: the ROM's VS has "
                        "exactly one mode (the star battle); reported and "
                        "ignored\n", vmode);
            if (!port_level_is_mounted(vlv)) {
                fprintf(stderr, "[vs] REFUSED: VS map %d is level %d "
                        "(overlay %d), not mounted in this build -- not "
                        "booting\n", mi + 1, vlv, port_level_overlay_id(vlv));
                return 2;
            }
            port_vs_stage_and_start(mi);
            {
                const int staged = port_level_entry_latch();
                if (staged != vlv) {
                    fprintf(stderr, "[vs] REFUSED: the ROM's own start staged "
                            "level %d where the map list says %d -- not "
                            "booting a level the ROM did not ask for\n",
                            staged, vlv);
                    return 2;
                }
                port_level_set_target(staged);
                port_scene_request_release("the VS start staged the map and "
                                           "the level boot is serving it");
                /* _putenv_s, NOT SetEnvironmentVariableA, and the difference
                   is the whole reason the ROM's staged star never arrived.
                   These two are read back by THIS process -- hal/level_boot.cpp
                   seats data_0209f264 and data_0209f220 from std::getenv a few
                   thousand lines into the same run -- and MSVC's getenv answers
                   out of the CRT's own copy of the environment, which
                   SetEnvironmentVariableA does not touch. It edits the Win32
                   block, which is what a CHILD process inherits.

                   THE OTHER SetEnvironmentVariableA CALLS IN THIS FILE ARE
                   CORRECT AND MUST NOT BE "FIXED" TOO. Every one of them
                   (port_menu_relaunch_* around 2765-2823) is setting up the
                   environment of a relaunched child, which is exactly what the
                   Win32 block is for. This pair is the only place the value has
                   to reach a getenv in the same process.

                   port/tests/smoke_persist.cpp:423-429 documents this exact
                   trap, in this tree, in these words -- and it still fired here.
                   Measured: with SetEnvironmentVariableA the VS boot seated star
                   1 (the default) instead of the 2 the ROM staged, so
                   Stage::InitResources' VS sound branch never armed and
                   LoadObjects filtered the arena's objects on the wrong group. */
                _putenv_s("SM64DS_ENTRANCE", "0");
                _putenv_s("SM64DS_STAR_FILTER", "2");
                fprintf(stderr, "[vs] ENTERING VS: map %d of 4 -> level %d "
                        "(overlay %d), star filter 2, VS mode flag %d "
                        "(PrepareVsMode's own write)\n", mi + 1, staged,
                        port_level_overlay_id(staged), (int)data_0209f2d8);
            }
        }
    }

    /* The collision object itself is the harness's either way; what fills it
       is the question. Under SM64DS_REAL_BOOT the game's own
       Stage::LoadClsnAndObjects does it -- and it runs BEFORE the Player,
       because on the real boot the entrance spawns the Player and
       Player::InitResources reads the world-Y bounds the boot just set. */
    /* THE COLLIDER IS THE STAGE'S OWN NOW (gate 24). mc_storage was a bare
       0x60-byte MeshCollider the harness constructed and handed to the boot;
       on the ROM that object lives at Stage+0x91c and Stage::Stage constructs
       it there. The legacy boot keeps the harness one -- it builds no Stage. */
    static char mc_storage[0x60];
    unsigned level_bmd = 1943;
    char *stage = 0;
    if (real_boot) {
        stage = (char *)port_stage_create();
        g_mc = stage + 0x91c;
    } else {
        g_mc = mc_storage;
        _ZN12MeshColliderC1Ev(mc_storage);
    }
    if (real_boot) {
        /* Door and exit stay off in both stages -- their actors are Stage B.
           With SM64DS_BOOT_NOSPAWN the entrance table goes off too and the
           sub-table is dropped, which is stage A1: geometry only. */
        if (boot_spawns)
            port_stage_a2_seat();
        /* SM64DS_CHARACTER=0..3 (Mario, Luigi, Wario, Yoshi). It goes in HERE,
           before the boot, because LoadEntranceObjects reads the save byte to
           build the Player's spawn param and Player::InitResources loads that
           character's models and no others. Setting it after the spawn gets a
           Player whose model slot is null. */
        if (const char *cs = getenv("SM64DS_CHARACTER")) {
            character_set_pending(atoi(cs));
            fprintf(stderr, "[char] spawning %s\n",
                    CHAR_NAME[g_character_pending & 3]);
        }
        void *lvl = port_stage_a_boot(g_mc, boot_spawns);
        level_bmd = *(unsigned short *)((char *)lvl + 8);
        {
            const double t0 = port_lvlperf_now();
            port_stage_a_probe(g_mc);
            port_lvlperf_note(3, port_lvlperf_now() - t0);
        }
        /* PORT_WATCH_FADER=1: who writes the INSTALLED fader's vtable
           pointer. LoadEntranceObjects installs it during this boot (the
           r0 ride-through, port/unmatched/LoadEntranceObjects.cpp), and
           HUD::Behavior dispatches IsAtStart through it every frame, so a
           write there is fatal a long way from the writer. */
        if (getenv("PORT_WATCH_FADER") && data_0209f5bc) {
            fprintf(stderr, "[fwatch] arming write-watch on fader %p "
                    "(wipes at %p, stride %u)\n", data_0209f5bc,
                    *(void **)&data_0209f324,
                    (unsigned)((char *)data_0209f5bc -
                               (char *)*(void **)&data_0209f324));
            port_watch_words(data_0209f5bc, 1);
        }
        if (boot_spawns) {
            port_stage_tree_probe(data_0209f394[0], "PLAYER");
            /* SM64DS_SPAWN_ACTOR=<id>[:<param>][@<area>][,...]: put one actor of each
               named class at the player, through the level's own spawn path.
               Fired here, after the entrance made the player and before the
               census, so an env-spawned class is counted with the rest. */
            port_debug_spawn_env();
            {
                const double t0 = port_lvlperf_now();
                port_actor_census();
                port_lvlperf_note(2, port_lvlperf_now() - t0);
            }
            port_actor_lists_probe();
        }
        /* the direct boot's one [lvl-perf] line; a warp's comes from
           hal/level_change.cpp at the end of the change */
        port_lvlperf_emit();
    }

    void *player;
    if (boot_spawns) {
        /* THE ENTRANCE SPAWNED HIM. data_0209f394[0] is where
           LoadEntranceObjects parked the actor it made from entrance record
           0 -- position, rotation, area and entrance type all the level's
           own, and Player::InitResources already run through the spawn
           spine's init Process. */
        /* run mg16 lane MP3, field failure: THE LOCAL PLAYER IS NOT ALWAYS
           SLOT 0. This took data_0209f394[0] unconditionally, which is right on
           a console that is player 1 and wrong on every other one. On the child
           it made the harness's whole notion of "me" -- the camera target, the
           body the walk states drive, the actor every debug readout follows --
           point at the HOST's character. That is the owner's "P2 shows me as
           Mario when I should be Luigi", and it is a different bug from the pad
           clobber that shared the symptom.

           data_0209f250 is the ROM's own "which player am I", seated from
           func_0203da9c() (my comms slot). Falls back to slot 0 when the slot
           is out of range or its actor did not spawn, because a harness that
           refuses to boot is worse than one that boots as player 0 and says
           so. */
        {
            int me = (int)data_0209f250;
            if (me < 0 || me >= 4 || !data_0209f394[me]) {
                if (me != 0)
                    fprintf(stderr,
                            "[vs] local player index %d has no actor; the "
                            "harness is falling back to slot 0\n", me);
                me = 0;
            }
            player = data_0209f394[me];
            if (me != 0)
                fprintf(stderr,
                        "[vs] this window is player %d; the camera and the "
                        "walk states follow data_0209f394[%d]\n", me, me);
        }
        if (!player) {
            fprintf(stderr, "the entrance spawned no player\n");
            return 3;
        }
    } else {
        player = _ZN9ActorBasenwEj(0x800);
        _ZN6PlayerC1Ev(player);
        if (hal_player_init_resources(player) != 1) return 3;
    }

    /* the castle grounds floor, gate-8 recipe */
    char *c = (char *)player;

    /* Read the character back off the Player the spawn actually produced,
       rather than assuming the save byte got through. Zeroed storage gives 0,
       which IS Mario, but that is a property of the entrance param and not a
       guarantee worth leaning on. */
    g_character = *(unsigned char *)(c + 0x6d9) & 3;
    g_character_pending = g_character;

    /* SKIP THE CHARACTER INTRO CUTSCENE, which the other three spawn with and
       Mario does not. func_ov002_020c4188 is that cutscene's state machine,
       entered whenever +0x71e is nonzero, and it is built on two things the
       port does not have: the Message box (func_0201f32c, guarded to a no-op
       in hal/level_boot.cpp) and the camera-script calls that follow it. With
       the message guarded it simply faults one step further along, on the
       object the message was supposed to have made. Zeroing the cutscene id is
       the honest version of "not hosted": the state machine returns on its
       first line and the character just plays. No-op for Mario, who arrives
       with it already 0. */
    if (*(unsigned char *)(c + 0x71e) && !getenv("SM64DS_INTRO_CUTSCENE")) {
        fprintf(stderr, "[char] skipping intro cutscene %u (not hosted)\n",
                (unsigned)*(unsigned char *)(c + 0x71e));
        *(unsigned char *)(c + 0x71e) = 0;
    }
    if (!real_boot) {
        static struct { unsigned short id; unsigned char refs; void *p; } kp;
        _ZN13SharedFilePtr9ConstructEj(&kp, 1941);
        char *kcl = (char *)_ZN12MeshCollider8LoadFileER13SharedFilePtr(&kp);
        if (!kcl) return 4;
        static char clps[0x100];
        _ZN12MeshCollider7SetFileEP8KCL_FileR10CLPS_Block(mc_storage, kcl,
                                                          clps);
        /* ROOT CAUSE (found 2026-08-02): the level collider's OWNER feeds
           func_02035354's self-collision exclusion. Enabling it with the
           player as owner makes every player ground/wall probe skip the
           level -- which is why the game's own tracking never grounded and
           the harness snap exists. The game's own convention for level
           geometry is Enable(NULL): func_020395fc then stores owner 0 and
           clsnID -1, so level hits skip the FindWithID actor walk (a fake
           non-null owner fed it a junk ID and it faulted).
           NOW THE DEFAULT (gate 13): the real Camera seeds its own probes
           with the Player as their owner (func_0200897c), so under the old
           owner=player configuration every camera ray would exclude the
           level and the camera would sit inside geometry. The harness's own
           probes (hal_ground_ray / hal_line_ray) work under a NULL owner
           either way. SM64DS_FAKE_SNAP=1 brings the harness ground snap
           back on top for shots that need Mario planted. */
        _ZN16MeshColliderBase6EnableEP5Actor(
            mc_storage, fake_snap ? (void *)player : (void *)0);
        /* NO SCALE PAIR HERE ANY MORE. world = KCL raw << 6 is the walk's
           own business now (the ROM's `asr #6`, see
           port/unmatched/MeshCollider_DetectClsn_Sphere.cpp), and the real
           boot writes nothing after SetFile either -- which is the point:
           the harness staging and the level's own boot leave the collider
           in exactly the same state. */
    }
    /* the octree box is power-of-two PADDED (its center is way off the
       real stage); the geometry lives near the origin, so spawn there,
       a few units up -- the first frames drop him onto the lawn.
       Under the entrance boot there is nothing to invent: the level's own
       entrance record already put him at the castle gate, and SM64DS_SPAWN
       is the only way to move him. */
    {
        const char *sp = getenv("SM64DS_SPAWN");
        if (sp) sscanf(sp, "%d,%d,%d", &spawn_x, &spawn_y, &spawn_z);
        if (sp || !boot_spawns) {
            *(int *)(c + 0x5c) = spawn_x << 12;
            *(int *)(c + 0x60) = spawn_y << 12;
            *(int *)(c + 0x64) = spawn_z << 12;
        } else {
            spawn_x = *(int *)(c + 0x5c) >> 12;
            spawn_y = *(int *)(c + 0x60) >> 12;
            spawn_z = *(int *)(c + 0x64) >> 12;
        }
        printf("player at (%d, %d, %d) yaw %04x state %p step %u/%u "
               "path %02x\n",
               *(int *)(c + 0x5c) >> 12, *(int *)(c + 0x60) >> 12,
               *(int *)(c + 0x64) >> 12,
               (unsigned short)*(short *)(c + 0x8e), *(void **)(c + 0x370),
               *(unsigned char *)(c + 0x6e3), *(unsigned char *)(c + 0x6e5),
               *(unsigned *)(c + 0x670));
        if (getenv("PORT_WATCH_POS"))
            port_watch_words(c + 0x5c, 3);
    }
    /* THE RADIUS LEVER IS NOT HERE. WithMeshClsn+0x18 is the radius
       UpdateExtraContinous hands to SphereClsn::SetObjAndSphere and +0x1c the
       vertical offset it adds to pos first, but Player::Behavior RECOMPUTES
       BOTH every frame from the mega/balloon factor and sets them to the SAME
       value, so writing either one here is overwritten before it is read --
       and the game's own mega lever moves them together, which cancels out of
       the resting height entirely (floor - vo + radius). Scaling the sphere's
       radius alone has to happen at the sphere: SM64DS_SPHERE_RADIUS_PCT in
       port/hal/clsn_vtable.cpp. */
    /* the level model: main_castle_all.bmd -- handle 1943 by hand, and under
       the real boot the LVL_Overlay's own bmdFileId, which is the same 1943
       (the harness had guessed right); world-space verts scaled by the BMD
       header's scaleShift */
    /* THE GAME'S OWN LOADER NOW (gate 24). Stage::LoadModel reads the same
       bmdFileId out of the LVL_Overlay the harness had hard-coded, loads it
       into the Stage's OWN Model at +0x86c, and does the two things the hand
       load never did: rewrite every not-fully-lit component's polygon
       attributes to 0x13, and park the ModelComponents pointer in
       data_0209f320. The legacy boot has no Stage, so it keeps the hand load. */
    static char level_storage[0x50];
    char *level_model = level_storage;
    int level_shift = 0;
    if (real_boot) {
        level_model = stage + 0x86c;
        /* Stage::LoadModel IS NOT CALLED HERE ANY MORE (run link60, lane SL0).
           It runs inside port_stage_boot_body, ahead of
           Stage::LoadClsnAndObjects, which is the order Stage::InitResources
           has it in (:361 then :363). The harness used to call it after the
           whole boot, which left data_0209f320 null for the object pass and
           cost level 40 its ten id-167 platforms. Calling it again here would
           be the second rebase hal/level_boot.cpp's block warns about, so the
           call MOVED rather than being added there. The model is already
           loaded by this point; the reads below just pick up the result. */
        /* ModelBase+0x04 is the loaded BMD (include/ModelBase.h); its header
           word 0 is the scaleShift, which is 1 for the castle. */
        void *bmd = *(void **)(level_model + 0x04);
        level_shift = bmd ? *(int *)bmd : 0;
        printf("level model loaded by Stage::LoadModel, handle %u, "
               "scaleShift %d, components %p\n", level_bmd, level_shift,
               (void *)(size_t)data_0209f320);
        /* the SKYBOX, InitResources' last load: LoadSkybox reads the
           LVL_Overlay's skybox id (castle grounds: 1 -> data_02075620[0] =
           handle 2040 = data/vrbox/vr01.bmd), news a Model off the game heap
           and parks it at Stage+0x9bc; port_stage_render_skybox draws it in
           front of the opaque pass, glued to the camera eye. Id 0 leaves
           +0x9bc NULL and both sides no-op, same as the ROM. */
        _ZN5Stage10LoadSkyboxEv(stage);
        printf("skybox loaded by Stage::LoadSkybox, id %u, model %s\n",
               _ZN5Stage11GetSkyboxIDEv(),
               *(void **)(stage + 0x9bc) ? "set" : "none");
    } else {
        static struct { unsigned short id; unsigned char refs; void *p; } mp;
        _ZN13SharedFilePtr9ConstructEj(&mp, level_bmd);
        _ZN5ModelC1Ev(level_storage);
        void *bmd = _ZN5Model8LoadFileER13SharedFilePtr(&mp);
        if (bmd) {
            level_shift = *(int *)bmd;   /* BMD header word 0 */
            _ZN9ModelBase7SetFileEP8BMD_Fileii(level_storage, bmd, 0, -1);
            printf("level model loaded, scaleShift %d\n", level_shift);
        } else {
            fprintf(stderr, "level model load failed (handle %u)\n", level_bmd);
        }
    }

    /* diagnostic: a direct ground ray at the spawn separates a filter
       problem (player flags) from a registry problem (nothing hittable) */
    {
        /* NOT CAPTURED, and it does not need to be. RELOADRV's reverse scan
           named this buffer because its +12 word holds the Player (30039f38)
           -- but that word is an OUTPUT: RaycastGround's constructor runs over
           the whole 0x50 on the next line and SetObjAndPos writes the object
           before DetectClsn reads it, every time. A function-scope static that
           is fully re-constructed before every read cannot carry a stale
           pointer into a use. (It is a static rather than a local only so the
           address is stable for the printf walk below.) */
        static char rg[0x50];
        int pos[3] = {*(int *)(c + 0x5c), *(int *)(c + 0x60),
                      *(int *)(c + 0x64)};
        _ZN13RaycastGroundC1Ev(rg);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, pos, player);
        rg[4] |= 1;   /* BgCh collide-ordinary (the gate-8 predicate bit) */
        *(int *)(rg + 0x4c) = 0x100000;   /* reach: 256 units down */
        int hit = _ZN13RaycastGround10DetectClsnEv(rg);
        /* +0x44 is the HIT, +0x3c is where the ray STARTED. This line printed
           +0x3c under the name "ground_y" and was read as a collision signal
           for exactly as long as nobody checked it: include/RaycastGround.h
           pins 0x038 as the probe position (so +0x3c is its .y) and 0x044 as
           "the collision height in Fix12i: the search seed on entry, the hit
           on exit", with 0x048 the has-collision byte. The tell was in the
           data -- the old number read 1700.0 in every level-6 arm of the
           stage-geom proof, stock and fully-replaced collision alike, and a
           real hit height cannot be invariant across that. Both are printed
           now, each under its own name, because the origin is still worth
           seeing next to the hit. */
        printf("ground probe at spawn: hit=%d clsn_y=%d (%.1f units) "
               "has_clsn=%d ray_origin_y=%.1f\n",
               hit, *(int *)(rg + 0x44), *(int *)(rg + 0x44) / 4096.0f,
               rg[0x48], *(int *)(rg + 0x3c) / 4096.0f);
        {
            extern void *data_020a0c80[];
            int direct = ((int(__fastcall *)(void *, void *, void *))(
                ((void ***)g_mc)[0][6]))(g_mc, 0, rg);
            printf("registry[0]=%p mc=%p direct-slot6=%d gy=%.1f flag=%d\n",
                   data_020a0c80[0], g_mc, direct,
                   *(int *)(rg + 0x44) / 4096.0f, rg[0x48]);
            /* the method's own first block, replicated by hand */
            char *o = (char *)data_020a0c80[0];
            printf("manual: o=%p p=%p ray_fc=%d head4=%d reach=%d\n", o,
                   *(void **)(o + 4), *(int *)(rg + 0xc), rg[4],
                   *(int *)(rg + 0x4c));
            {
                extern int func_02035354(void *, void *);
                extern int func_020393b4(void *);
                void *p2 = (void *)func_020393b4(o);
                int f = func_02035354(rg, p2);
                printf("manual filter(rg, p)=%d p2=%p\n", f, p2);
            }
        }
        /* floor map: direct line walks over a coarse grid */
        for (int gz = -400; gz <= 400; gz += 200) {
            char row[64] = {0};
            int ri = 0;
            for (int gx = -400; gx <= 400; gx += 200) {
                int gy = 0;
                int h = hal_ground_ray(g_mc, gx << 12, 6000 << 12,
                                       gz << 12, 7000 << 12, &gy);
                ri += snprintf(row + ri, sizeof row - ri, "%7.1f",
                               h ? gy / 4096.0f : -999.0f);
            }
            printf("floor z=%4d: %s\n", gz, row);
        }
        /* SM64DS_WATER_MAP=1: WHERE THE WATER IS, asked exactly the way the
           Player asks. func_ov002_020c14b8's second ray is a RaycastGround
           with StartDetectingWater + StopDetectingOrdinary, and the surface it
           accepts is one whose CLPS carries flag 0x20; the answer lands in
           Player+0x64c and is what decides walk -> swim. This runs that same
           query over a grid, straight down from well above the level, and
           prints the water height it finds. A blank map with a water collider
           registered ([clsnreg]) means the query never reaches the mesh; a map
           with water in it means the query works and a probe that found none
           was simply standing somewhere dry. */
        if (getenv("SM64DS_WATER_MAP")) {
            /* SM64DS_WATER_MAP=<step>: floor height per cell, with a `~` on
               any cell whose floor is UNDER the water the query found there.
               `~` next to a plain number is a bank, which is where the
               walk<->swim transition can be exercised. */
            int step = atoi(getenv("SM64DS_WATER_MAP"));
            if (step < 250) step = 1000;
            printf("[watermap] step %d, rays from y=6000 reach 12000; "
                   "'~' = floor under water, '-' = no floor\n", step);
            for (int gz = -8000; gz <= 8000; gz += step) {
                char row[512];
                int ri = 0;
                for (int gx = -8000; gx <= 8000; gx += step) {
                    static char rgw[0x50];
                    int pos[3] = {gx << 12, 6000 << 12, gz << 12};
                    _ZN13RaycastGroundC1Ev(rgw);
                    _ZN4BgCh19StartDetectingWaterEv(rgw);
                    _ZN4BgCh21StopDetectingOrdinaryEv(rgw);
                    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(
                        rgw, pos, player);
                    *(int *)(rgw + 0x4c) = 12000 << 12;
                    int wet = _ZN13RaycastGround10DetectClsnEv(rgw) &&
                              SurfaceInfo_TestFlag0x20((int *)(rgw + 0x14));
                    int wy = *(int *)(rgw + 0x44);
                    int gy = 0;
                    int g = hal_ground_ray(g_mc, gx << 12, 6000 << 12,
                                           gz << 12, 12000 << 12, &gy);
                    if (!g)
                        ri += snprintf(row + ri, sizeof row - ri, "%7s", "-");
                    else
                        ri += snprintf(row + ri, sizeof row - ri, "%6.0f%c",
                                       gy / 4096.0f,
                                       (wet && gy < wy) ? '~' : ' ');
                }
                printf("[watermap] z=%6d %s\n", gz, row);
            }
        }
    }

    data_020a0e40[0] = 0;
    /* input processor staging: route Stage::CheckInput to its main
       path (mode flags), one controller, pad 0 active, mode 0 (D-pad
       drives the stick fields)

       TWO OF THESE THREE LINES RUN AFTER THE LEVEL BOOT AND UNDO IT.
       port_stage_a_boot is called ~350 lines above; on the entrance path the
       ROM's own Stage::LoadClsnAndObjects has by then taken its intro branch
       and called StartIntroCutscene, which seats data_0209fc48 with the
       opening's first script. `data_0209fc48 = 0` then throws that script away
       before its second frame, and `data_0209caa0[2] |= 0x80` marks the
       opening SEEN before it has played a frame -- the bit the ROM's own ov085
       state writes at the end of the flight. Measured: the cast spawned, one
       ProcessKuppaScript ran (cursor 0 -> 1), and the script pointer was 0 at
       the first CutsceneObject::Behavior, so the cursor froze at 1 and the
       camera held one pose for 1800 frames.

       This is the same "two seats for one fact, and the later one silently
       won" shape the player-count note below records, and it is why the intro
       lane's removal of the force-set in hal/level_boot.cpp was not enough:
       there was a second one here.

       THE GUARD IS THE ROM'S OWN WORD, not a new flag. data_0209fc48 is
       non-zero only when a cutscene script is actually running, which on a
       level boot happens on exactly one path -- the opening. Every other boot
       reaches this line with it already 0, so both lines run exactly as they
       always have and the default path is unchanged. data_0209d660 is not part
       of the pair and is left unconditional. */
    if (data_0209fc48 == 0) {
        data_0209caa0[2] |= 0x80;
        data_0209fc48 = 0;              /* already 0 here; kept for the diff */
    } else {
        fprintf(stderr, "[intro] a cutscene script is running (%p): leaving "
                        "flags2 bit 7 clear and the script seated -- the "
                        "harness staging would have ended the opening here\n",
                (void *)(size_t)data_0209fc48);
    }
    data_0209d660 = 0;
    /* run mg16 lane MP3: THE SECOND SEAT, and the one that was actually
       winning. hal/level_boot.cpp's a2 seat sets the player count too, and this
       line runs after it -- so seating two players there and reading one here
       is exactly what the VS probe measured: the boot log said "2 players" and
       the probe reported count=1 with slot1 NULL on every frame of a 300-frame
       run. Two seats for one fact, and the later one silently won. */
    data_0209f21c = (unsigned char)port::vs_player_count();
    data_0209f350[0] = 0;
    /* run mg16 lane MPBTN: THE REAL REMAP MAPS, and retiring the zeros that
       sat here. This line used to point all four entries at a zero map,
       because "buttons are written directly to the Ctrl fields anyway" -- and
       that premise died the day the direct Ctrl stores were gated off in a
       session (the third-writer gate below, 0f0d5e134). From then on the ROM's
       own button path -- key register, local record, wire, fan-out into
       PadData, CheckInput's remap -- was the ONLY path, and its last step
       multiplied every button by zero. The maps are Nintendo's own bytes now
       (port/tools/romdata.py), so a fanned pad word turns into exactly the
       Ctrl bits the DS would have produced, for every slot.

       The pointer table data_0209214c itself is also hosted ROM bytes, but
       its words are DS ADDRESSES (0x02075650..) -- the relocated-pointer-table
       class -- so it is repointed here, never dereferenced as emitted. Mode 3
       keeps a zero map: the ROM's fourth word is not a map (it points into
       unrelated data) and no staged pad ever selects mode 3. */
    {
        static unsigned short zero_btn_map[32];
        ((unsigned short **)data_0209214c)[0] = (unsigned short *)data_02075650;
        ((unsigned short **)data_0209214c)[1] = (unsigned short *)data_02075670;
        ((unsigned short **)data_0209214c)[2] = (unsigned short *)data_02075690;
        ((unsigned short **)data_0209214c)[3] = zero_btn_map;
    }
    /* InitResources parked the state machine in St_LevelEnter, which was a
       no-op state under the fake spawn context: its entrance-anim table read
       junk and its Main was not hosted. Clearing it let the wait ticks drive
       until the first stick input. Under the entrance boot the park is
       CORRECT -- the level's own entrance record chose the state and its
       step -- so it stands, and St_LevelEnter runs the entry animation until
       it hands over to Wait. */
    if (!boot_spawns)
        *(void **)(c + 0x370) = 0;
    /* no path binding: the level spawn entry's path param, 0xff = none.
       The fake spawn context zero-fills it, and path 0 sends the real
       ground tracking into PathPtr walks over a table no level boot has
       seated (frame-1 fault under the game's own tracking). The real boot
       seats data_020a0d84/d88/d8c, so the pin comes off with it. */
    if (!real_boot)
        *(unsigned int *)(c + 0x670) = 0xff;
    if (getenv("PORT_WATCH_HEAD"))
        port_watch_words(data_0209b468, 4);
    if (!boot_spawns)
        hal_player_st_wait_init(player);

    /* ---- the real Camera actor (gate 13) -----------------------------
       The default since the camera came up clean over 400 frames.
       SM64DS_OLD_CAMERA=1 brings back the hand-tuned follow rig; arming
       data_0209f318 also wakes ~30 dormant ov002 call sites that funnel
       into Camera::ChangeState, so the escape hatch is worth keeping.

       Order matters. The vtable has to be up before InitResources,
       because InitResources ends in a virtual call to slot 9 (Render);
       and data_0209f318 is armed LAST, after the object is fully built,
       so nothing reaches a half-initialized camera. */
    void *cam = 0;
    const int real_camera = getenv("SM64DS_OLD_CAMERA") == 0;
    if (boot_spawns) {
        /* THE ENTRANCE SPAWNED IT TOO. LoadEntranceObjects finishes by
           spawning actor 0x14c with the entrance id it just read and parking
           the result in data_0209f318, which is exactly what the block below
           did by hand -- vtable, spawn context, InitResources, arm. */
        cam = data_0209f318;
        if (!cam) {
            fprintf(stderr, "the entrance spawned no camera\n");
            return 5;
        }
        printf("camera at %p (entrance-spawned), mode %p, state %p, fov %d\n",
               cam, *(void **)((char *)cam + 0x13c),
               *(void **)((char *)cam + 0x138),
               *(short *)((char *)cam + 0x17a));
    } else if (real_camera) {
        /* engine state the camera boot reads. All of it is what a level
           with no Stage loader looks like: no view objects, no weather,
           no area shown yet, the local player at index 0. */
        data_02092120 = -1;          /* no area shown -> ChangeArea skips Hide */
        data_0209f250 = 0;           /* local player index */
        data_0209f394[0] = player;   /* the actor the camera follows */
        data_0209f1f8 = 0;           /* view-object count */
        data_0209f2f8 = 0;           /* sublevel id: no weather system */
        data_0209f32c[0] = 0;        /* water level */
        data_0209fc48 = 0;           /* not in a cutscene */
        data_0209f20c[0] = data_0209f294[0] = data_0209f2c4[0] = 0;
        data_0209b454[0] = 0;
        data_0209ee90[0x44 / 4] = 0x1000;   /* scaleW, what Render feeds
                                               PerspectiveW_ (R10) */
        /* spawn context: actor 0x14C, spawn param 0 (entrance 0). The
           param lands at actor+8 and picks func_0200cf40's branch --
           anything but 0xf takes the view-object path, which is the one
           that selects mode 10, the gameplay camera. It is safe here
           because data_0209f354 points at a real (zeroed) table. */
        {
            static unsigned short cam_spawn_info[4] = {0, 0, 0x14c, 0};
            data_020a4bb8[0x14c] = cam_spawn_info;
            data_020a4b54 = 0x14c;
            data_020a4b60[0] = 0;
        }
        if (!hal_camera_check_layout())
            fprintf(stderr, "[cam] LAYOUT CHECK FAILED -- expect nonsense\n");
        hal_fill_camera_vtable();
        cam = hal_camera_new();          /* the ctor allocates its own 0x1a8 */
        if (!cam) { fprintf(stderr, "camera alloc failed\n"); return 5; }
        if (hal_camera_init_resources(cam) != 1)
            fprintf(stderr, "[cam] InitResources did not return 1\n");
        data_0209f318 = cam;             /* ARMED LAST */
        printf("camera at %p, mode %p, state %p, fov %d, near %d far %d\n",
               cam, *(void **)((char *)cam + 0x13c),
               *(void **)((char *)cam + 0x138),
               *(short *)((char *)cam + 0x17a),
               *(int *)((char *)cam + 0xfc), *(int *)((char *)cam + 0x100));
    }

    /* GATE 35: the course loop's own boot. After the level and the entrance,
       because it reads data_0209f2f8 for the sound row and wants the Player
       already spawned; before the frame loop, because the health words it
       seats are what the HUD's first Behavior reads.
       SM64DS_NO_COURSE_SEAT=1 goes back to the gate-28 state: the single
       hand-written health word above, no lives, no music, and the queued
       character swap left reading as pending. */
    if (real_boot && !getenv("SM64DS_NO_COURSE_SEAT"))
        port_course_seat();

    /* window. The class, the styles, the DC and the two DIB headers are
       host_window_open's now (see its banner), because the windowed scene
       runner opens the same window and a second copy of that block is a second
       place for CS_OWNDC or WS_THICKFRAME to be got wrong.

       THE MODE IS READ HERE AND IT LATCHES HERE: the window is created once,
       and a layout that could change after this point would leave the picture
       and the frame permanently out of step. On the LEVEL path the answer can
       only ever come from SM64DS_DUAL_SCREEN, because nothing proposes on a
       level; a minigame's own default is proposed in the scene runner, which
       main hands over to well above this line. */
    const int stacked = hal_sub_screen_stacked();
    HDC hdc = 0;
    HWND hwnd = 0;
    /* A TITLE-ENTRY RUN ARRIVES HERE WITH A WINDOW ALREADY OPEN and reuses it,
       rather than leaving the title's window standing and opening a second one
       beside it. Only a windowed title run can satisfy both tests; a headless
       one recorded no handle and takes the ordinary open below. */
    if (port_title_entry_taken() && g_entry_hwnd) {
        hwnd = g_entry_hwnd;
        hdc = g_entry_hdc;
        fprintf(stderr, "[title-entry] reusing the title's own window for the "
                        "adventure (one window, one game)\n");
    } else {
        hwnd = host_window_open(
            stacked, &hdc,
            "SM64DS   |   WASD move   Shift dash   Space jump"
            "   X punch   Ctrl crouch   |   Q/E turn   R/F"
            " tilt   |   F1 camera   F3 stats   F5 or Esc menu"
            "   F12 fullscreen   Tab panel");
    }
    if (!hwnd) {
        fprintf(stderr, "the window did not open (win32 %lu)\n",
                (unsigned long)GetLastError());
        return 2;
    }

    /* SM64DS_WINDOW_SELFTEST=N: run N frames with W held, dump the last
       framebuffer next to the exe, exit -- CI-checkable without a user */
    const char *st = getenv("SM64DS_WINDOW_SELFTEST");
    const int selftest = st ? atoi(st) : 0;
    /* Live input, gated. key_live and its stale-key table are at file scope
       now (see their banner up there): the three gates inside them -- the
       selftest, the rebind capture and the window focus -- have two frame
       loops to serve since the scene path got a window, and three gates in two
       copies is three gates to keep in step. This is the one line that arms
       the first of them, from main's own local so the local stays the reader
       everything else in this function uses. */
    g_selftest = selftest;
    int focus_was = 1;   /* launch focused = launch unchanged */
    int frame = 0;
    float cam_yaw = 0.0f;   /* camera heading around Mario, radians */
    float cam_pitch = 0.13f; /* camera tilt above level, radians (R/F) */
    const int trace_cam = getenv("SM64DS_TRACE_CAM") != 0;
    /* Which way the camera turns when the player pushes a camera control to
       the right, as a signed step on the camera's heading. -1 by default:
       push right, pan right. settings.json's SwapCameraTurnDirection returns
       +1, which is what this program did before. Read once at boot, like the
       volume, so it takes effect the next time the player presses Play.

       EVERY horizontal camera control below multiplies by this one value --
       Q and E, the right stick, the bumpers, the mouse, and all three camera
       modes -- so no two of them can end up disagreeing about which way is
       right. See port/hal/host_settings.h for the measurement the default
       comes from; the DS has none of these controls, so there was never a
       hardware binding to be faithful to. */
    const int cam_turn = host_camera_turn_sign();
    /* HOW RUNNING WORKS, read once at boot beside the camera setting and for
       the same reason: a change takes effect the next time the player presses
       Play, except that this one can also be changed from the menu, which
       writes it straight back to settings.json.

       A SELFTEST IS PINNED TO THE DEFAULT. Every automated run reads no live
       key and no live pad already (key_live and pad_live both return nothing
       under one), so leaving the mode free would only mean a settings.json
       that happened to be beside the exe could move a comparator run -- and a
       comparator run that quietly depends on a player's preferences file is
       worse than no comparator. run_mode() is the one value the rest of the
       loop reads, so that pin is a single line rather than a gate per use.
       Nothing can move g_run_mode under a selftest either -- the menu is the
       only writer and the whole menu block is behind !selftest -- so the pin
       is belt and braces on top of that, not the only thing holding it. */
    g_run_mode = host_setting_run_mode();
    g_run_key = host_setting_run_key();
    g_run_pad = host_setting_run_pad();
    /* MouseCapture's boot value. Re-read live below beside the volume, and
       needing no selftest pin of its own: mo_capture_want refuses a selftest
       outright, which is the same belt-and-braces shape the run mode has. */
    mo_capture_opt = host_setting_mouse_capture();
    auto run_mode = [&]() -> int { return selftest ? RUN_BUTTON : g_run_mode; };
    /* said once at boot whatever it is, unlike the settings loader's
       off-default-only line: "which run mode was this player in" is the first
       question a movement report raises, and a support log that only mentions
       the setting when it is non-default cannot answer it */
    {
        char kb[16], pb[16];
        fprintf(stderr, "[run] mode %s, run on %s / %s%s\n",
                RUN_MODE_NAME[run_mode()],
                run_key_name(g_run_key, kb, sizeof kb),
                run_pad_name(g_run_pad, pb, sizeof pb),
                selftest && g_run_mode != RUN_BUTTON
                    ? "   (selftest: pinned to button)" : "");
    }
    /* SM64DS_DECEL_PROBE=1 (under a selftest): hold the stick and the dash
       button until DECEL_RELEASE, then let go of both and log the horizontal
       speed every frame until it reaches zero. The point is the SHAPE of the
       tail -- how many frames Mario coasts, and at what rate per frame -- so
       a before/after can be read off two runs. Off by default; the plain
       selftest picture is untouched by it. */
    /* =1 lets go of the stick AND the dash button (the full stop);
       =2 lets go of the dash button only and keeps the stick down, which is
       what "releasing run" means with a thumb still on the pad -- the target
       drops from the run speed to the walk speed and the skid path, which
       only the no-input branch can arm, never runs. */
    const char *dp_env = getenv("SM64DS_DECEL_PROBE");
    const int decel_probe = dp_env ? atoi(dp_env) : 0;
    /* late enough that the run has actually saturated: the walk core steps
       the speed 0x1000 a frame and the run target is ~0x24000, so he needs
       ~36 frames of held stick after the state settles before the tail
       measured is a tail off TOP speed and not off a ramp. */
    const int DECEL_RELEASE = (dp_env && atoi(dp_env) == 4) ? 170 : 80;
    /* mode 4 only: how long he stands still with run held, charging +0x6e5 */
    const int DASH_CHARGE_UNTIL = 110;
    int decel_stopped = 0;
    /* the F3 overlay: off unless SM64DS_OVERLAY=1 says otherwise */
    g_overlay_on = getenv("SM64DS_OVERLAY") != 0;
    /* SM64DS_MENU=1 opens the menu at boot. Its KEYS are off under a selftest
       (an automated run must not have a menu opening under it), but the panel
       itself draws, which is how a shot of it gets captured without a person. */
    menu_on = getenv("SM64DS_MENU") != 0;
    int overlay_edge = 0;
    double ovl_fps = 0, ovl_tps = 0, ovl_last_present = 0;
    unsigned ovl_mem_kb = 0;

    /* the bottom screen: dual OAM, the 2D frame, and the corner panel */
    hal_sub_screen_init(hwnd, ZOOM);
    hal_sub_screen_probe();

    /* boot complete: everything the boot queued in the stdout buffer goes to
       the sink here, in one write (see the setvbuf note above) */
    fflush(stdout);

    /* ---- THE RESTORE RE-SEAT ----------------------------------------------

       A save-state load replaces the world the same way a level handoff does,
       and the handoff's own comment further down says what that costs: "EVERY
       POINTER main() HOLDS INTO THE LEVEL IS STALE AFTERWARDS". The handoff
       re-derives them. Until run mg15 no restore did, in any of its four
       paths -- the boot-time disk read, F9, the debug menu's load row and the
       scripted SM64DS_SS_LOAD -- so a state whose world put the Player at a
       different arena address left this frame loop ticking whatever now sat
       at the boot's address, handing port_stage_path_guard a floor binding
       the restored level cannot produce and Camera::Behavior a follow target
       that is not a Player.

       The same three reads the handoff makes, in the same order, and it says
       what it changed so a log shows whether a given restore needed it.
       SM64DS_SS_NO_RESEAT=1 turns it off, for the A/B that proves it is
       load-bearing. */
    auto ss_reseat = [&](const char *why) {
        static int off = -1;
        if (off < 0) off = getenv("SM64DS_SS_NO_RESEAT") != 0;
        if (off) {
            fprintf(stderr, "[ss-reseat] %s: SM64DS_SS_NO_RESEAT=1, host "
                    "pointers left as they were\n", why);
            return;
        }
        void *np = data_0209f394[0];
        void *nc = data_0209f318;
        if (np == player && nc == cam) {
            fprintf(stderr, "[ss-reseat] %s: player %p camera %p, both "
                    "already the world's own\n", why, player, cam);
            return;
        }
        fprintf(stderr, "[ss-reseat] %s: player %p -> %p, camera %p -> %p\n",
                why, player, np, cam, nc);
        if (np) {
            player = np;
            c = (char *)player;
            /* read the character back off the restored Player, exactly as the
               handoff does off the entrance-spawned one */
            g_character = *(unsigned char *)(c + 0x6d9) & 3;
            g_character_pending = g_character;
        }
        cam = nc;
        an_pivot_live = 0;
        if (cam_mode != CAM_DS && cam) fc_seed(cam);
        /* g_mc IS A FOURTH POINTER INTO THE WORLD, and RELOADRV's reverse scan
           found it: it is the Stage's level MeshCollider at Stage+0x91c, set
           once during setup and then read by every ground and line ray this
           frame loop casts. RE-DERIVED rather than captured, because it is a
           harness global in the file whose frame counter and playlog must NOT
           roll back (hal/dsstate_seg.h draws that line per symbol, and putting
           a bracket in this file to hold one word invites the later-extern
           trap that file warns about). The expression is the boot's own, and
           only for the Stage-backed case: the no-Stage boot points g_mc at
           mc_storage, a harness buffer, which a restore has no opinion about. */
        {
            void *st = port_stage_object();
            if (st && g_mc != (void *)mc_storage)
                g_mc = (char *)st + 0x91c;
        }
    };
    (void)ss_reseat;

    /* Disk save state, read exactly once, here: the world is fully booted (the
       disk state describes a booted world, so restoring earlier would be
       stomped by the rest of boot) and the frame loop has not started. Never in
       a selftest: the comparator runs must stay deterministic, and a stray
       savestate.bin beside the exe would silently swap the world out from
       under them. */
    /* SM64DS_SS_DISKLOAD=1 opts a selftest INTO the disk read, for the
       cross-restart reproducer: run one saves to disk (SM64DS_SS_DISK=1), run
       two boots with this set and must land on the first run's hardware hash.
       Without the env, selftests never touch savestate.bin, so the comparator
       runs stay deterministic. */
    /* THE ROLLBACK-COUPLED GUARDS' A/B HOOK, joined here because this is the
       only binary that links both halves: hal/lk6_savestate.cpp owns the hook
       and the two smoke targets link it without the mount table, while
       hal/level_boot.cpp owns the stash pair and smoke_player links THAT
       without lk6. Registered before the first restore of any kind -- the disk
       read below is the earliest one, and F9 and the menu's load row are all
       later, in the frame loop. Does nothing unless SM64DS_SS_NO_ROLLGUARD=1
       asks for the fix-off arm. */
    port_ss_rollguard_hook(port_rollguard_stash, port_rollguard_unstash);

    if ((!selftest || getenv("SM64DS_SS_DISKLOAD")) && lk7_persist_available()) {
        if (lk7_persist_read()) {
            an_pivot_live = 0;   /* no ease across the load */
            ss_census("after the boot-time disk restore", player, cam);
            ss_reseat("after the boot-time disk restore");
            ss_note("state loaded from disk (F9 reloads it)");
        } else if (lk7_persist_refusal()[0]) {
            /* THE REFUSAL HAS TO BE VISIBLE, and this is the only place it can
               be. The game has just booted FRESH on purpose -- a savestate.bin
               was there and was turned away -- and without this line the player
               sees a normal boot and a save state that quietly stopped
               existing. The long form is already in the playlog; this is the
               sentence on the screen. Same toast slot the load note uses, so
               the two outcomes appear in the same place and read as the pair
               they are. */
            ss_note(lk7_persist_refusal());
        }
    }

    static ntr::Framebuffer fb;
    /* THE PRESENT PATH IS ARMED HERE and not a line earlier: present() draws
       whatever these point at, and until the frame loop has rasterised once
       the framebuffer is a blank static. Arming it after the boot printing
       means the first thing the window ever shows is a real frame rather than
       a grey rectangle. From this point on a WM_SIZE or a WM_PAINT can
       redraw without the frame loop's help, which is what a drag on the
       sizing border needs -- that drag runs a modal loop inside
       DefWindowProc and the frame loop does not get a turn until it ends. */
    g_present_hwnd = hwnd;
    g_present_hdc = hdc;
    g_present_bi = &g_bi;
    g_present_fb = &fb;
    MSG msg;
    /* THE DS'S POWER-ON INTERRUPT STATE, standing in for src/func_0201a054.c,
       the game's own IRQ init, which is in no slice. The ROM's arming
       sequences SAVE AND RESTORE IME rather than setting it, so a host that
       boots with IME at zero arms interrupts that can never be delivered.
       Only ntr::rt_run used to seat it and this loop is not on that fiber.
       See port/irq2_map.txt section 2. */
    ntr::rt_irq_boot_state();
    /* The editor control channel (hal/editor_channel.cpp). A no-op unless
       SM64DS_EDITOR_CHANNEL=1 -- it reads the variable and returns before any
       socket exists -- so the shipped window grows no listener by default. */
    editor_channel_init();
    for (;;) {
        double t_frame, t_phase;
        int game_ticked = 1;   /* cleared when a tick is skipped */
        while (W.PeekMessageA_(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return 0;
            W.TranslateMessage_(&msg);
            W.DispatchMessageA_(&msg);
        }
        /* A restore that happened outside this scope (the debug menu's load
           row) left every pointer below addressing the pre-restore world.
           Re-seat before anything this frame reads them. */
        if (ss_reseat_pending) {
            ss_reseat_pending = 0;
            ss_census("after the debug menu's load row", player, cam);
            ss_reseat("after the debug menu's load row");
        }
        /* settings.json, watched while running: the launcher's dialog writes
           the file on every change, so the gap and the volume follow the
           player's hand without a relaunch. The poll is a counter compare on
           29 frames out of 30 and says 1 only when an answer moved; the gap
           side needs no push here because the layout latch reads
           host_settings_gen itself. */
        if (host_settings_poll()) {
            const int v = host_setting_volume();
            if (v >= 0) out_set_volume_pct(v);
            /* MouseCapture is PUSHED like the volume rather than pulled like
               the gap keys: the frame loop asks mo_capture_want every frame and
               a per-frame settings call would turn a counter compare into a
               file question. The engage and the release still happen on the
               frame test below, so flipping the key mid-play is not a special
               path -- it just changes what that test answers. */
            mo_capture_opt = host_setting_mouse_capture();
        }
        ph_begin(&t_frame);
        ph_begin(&t_phase);
        /* the focus edge, read once a frame BEFORE any key is. Coming back,
           every key starts stale, so whatever the player was holding in the
           other window has to be released before this one will see it. Going
           away needs no work: key_live is already returning released, which is
           what empties the pad words below. */
        if (!selftest) {
            const int now = hal_window_focused();
            if (now && !focus_was) memset(key_stale, 1, sizeof key_stale);
            focus_was = now;
        }
        {
            const int now = key_live(VK_F3);
            if (now && !overlay_edge) g_overlay_on = !g_overlay_on;
            overlay_edge = now;
        }
        /* F12 (and F11, the same thing) toggles borderless fullscreen. Its own
           edge latch, up here with the other window-level keys, and read
           through key_live rather than off a WM_KEYDOWN for three reasons a
           WM_KEYDOWN branch would each have had to re-solve: key_live is what
           carries the focus gate, the rebind-capture gate, and the selftest
           gate -- and that last one is why a headless run cannot toggle
           anything, whatever is being typed on the machine at the time.

           F11 IS AN ALIAS, not a second feature. The F-row this program had
           spoken for ended at F9 (F1 camera, F3 stats, F4 character, F5 menu,
           F8/F9 save state), so F11 and F12 were both free and both are the
           key a person reaches for; binding one and not the other is a
           coin-flip a player would have to lose once to learn.

           ALT+ENTER IS DELIBERATELY NOT A THIRD ALIAS. Enter is the debug
           menu's act button (the pad-A mirror further down), so alt+enter
           would fire the menu selection under the cursor on the way into
           fullscreen. Making it not do that means special-casing the alt
           state inside the menu's own key read, which is game input, and this
           lane does not touch game input. */
        {
            static int fs_edge;
            const int now = key_live(VK_F12) || key_live(VK_F11);
            if (now && !fs_edge) fullscreen_toggle(hwnd);
            fs_edge = now;
        }
        /* F4 cycles the character with the menu CLOSED, mid-walk. Its own edge
           latch, deliberately outside the menu's held-mask below, so it is not
           one of the keys the menu swallows while it is open. */
        {
            static int chr_edge;
            const int now = key_live(VK_F4);
            if (now && !chr_edge) {
                const int nxt = (g_character + 1) & 3;
                fprintf(stderr, "[chr] F4 becoming %s\n", CHAR_NAME[nxt]);
                port_player_set_character(c, nxt);
                g_character = g_character_pending = nxt;
                an_pivot_live = 0;   /* do not ease across it */
            }
            chr_edge = now;
        }
        /* F8 SNAPSHOTS the game, F9 RESTORES it. Their own edge latches, up
           here at the top of the frame after the message drain and before this
           frame's tick, which is the between-frames point the save state wants:
           the previous tick is fully complete and nothing is mid-update. A load
           with no prior save is a safe no-op (lk6_savestate_load says so and
           does nothing). Deliberately outside the menu's held-mask below so
           they work during live play whether or not the menu is open, and so
           the menu never swallows them. */
        {
            static int save_edge, load_edge;
            const int save_now = key_live(VK_F8);
            const int load_now = key_live(VK_F9);
            if (save_now && !save_edge) {
                if (lk6_savestate_save()) {
                    /* mirror to disk; the toast tells the player whether this
                       save will outlive the run, which is the difference every
                       "it did not save" report was actually about */
                    ss_note(lk7_persist_write()
                                ? "state saved to disk (F9 loads it)"
                                : "state saved for THIS RUN (F9 loads it)");
                } else {
                    ss_note("state NOT saved (see log)");
                }
            }
            if (load_now && !load_edge) {
                if (lk6_savestate_load()) {
                    an_pivot_live = 0;   /* no ease across */
                    ss_census("after an F9 restore", player, cam);
                    ss_reseat("after an F9 restore");
                    ss_note("state loaded");
                } else {
                    ss_note(lk6_savestate_has() ? "state NOT loaded (see log)"
                                                : "no state saved yet (F8 saves)");
                }
            }
            save_edge = save_now;
            load_edge = load_now;
        }
        /* SCRIPTED SAVE-STATE REPRODUCER (headless, selftest only).
           This is the regression rig for the 0.2.0 restore crash: real players
           save, then a message/cutscene changes hosted mode-lock state that
           lives OUTSIDE the arena, then they restore, and the game either
           faults in Player::SetAnim on a null BCA_File* (the ANIM_PTRS
           SharedFilePtr table, data_ov002_020ff480, is a hosted overlay global
           the pre-fix snapshot never captured) or stays wedged in a message the
           arena has already rolled away from.

               SM64DS_SS_SAVE=<frame>   fire lk6_savestate_save() at this frame
               SM64DS_SS_LOAD=<frame>   fire lk6_savestate_load() at this frame
               SM64DS_SS_LOCK=1         between save and load, force the hosted
                                        message-active lock (data_0209d660) set,
                                        the way opening a dialogue would, so the
                                        restore has a specific out-of-arena bit
                                        to prove it rolls back
               SM64DS_SS_ASSERT=1       capture data_0209d660 at save and assert
                                        it equals that value after load; a
                                        non-zero exit if the lock survived the
                                        restore. This is acceptance test 2 (the
                                        mode/lock global rolls back) run
                                        directly on the underlying global, since
                                        a full castle-grounds cutscene is not
                                        reachable in a plain headless walk. */
        if (selftest) {
            static int ss_env, ss_save_fr = -1, ss_load_fr = -1,
                       ss_lock = 0, ss_assert = 0, ss_expect_mount = 0,
                       ss_disk = 0, ss_watch = 0;
            static unsigned ss_flag_at_save;
            static unsigned long long ss_expect_hw;
            static unsigned char ss_lock_at_save;
            static int ss_saw_save = 0;
            static unsigned long long ss_hash_at_save;
            if (!ss_env) {
                ss_env = 1;
                const char *s = getenv("SM64DS_SS_SAVE");
                const char *l = getenv("SM64DS_SS_LOAD");
                if (s) ss_save_fr = atoi(s);
                if (l) ss_load_fr = atoi(l);
                ss_lock = getenv("SM64DS_SS_LOCK") != 0;
                ss_assert = getenv("SM64DS_SS_ASSERT") != 0;
                ss_disk = getenv("SM64DS_SS_DISK") != 0;
                /* the cross-restart reproducer's second half: this run did not
                   save, it BOOTED from run one's savestate.bin, so the hash to
                   hold the restore to arrives from outside (run one's printed
                   save hash) instead of from a save frame in this process */
                if (const char *xh = getenv("SM64DS_SS_EXPECT_HW"))
                    ss_expect_hw = strtoull(xh, 0, 16);
                /* the cross-area variant: something between save and load is
                   expected to MOUNT another area (SM64DS_EXIT_ENTER drives the
                   real door path). Requiring the hardware stores to have
                   actually CHANGED before the load makes a mistimed run fail
                   loudly instead of passing without testing anything. */
                ss_expect_mount = getenv("SM64DS_SS_EXPECT_MOUNT") != 0;
                /* SM64DS_SS_WATCH_FLAG=1: the PACKED-GAP reproducer. Watch the
                   id halfword of ov009's castle-water texture animation record
                   (see the declaration of data_ov009_02112bc4 above; the env
                   name says FLAG for continuity with the review, the storage
                   is not FLAG's) across the save/load, and report whether it is
                   captured at all. =2 also FAILS the run if the halfword did
                   not roll back, which is the assertion form. Separate from
                   SM64DS_SS_ASSERT because that one is the message-lock and
                   hardware-store acceptance test and this must be able to run
                   red while that runs green.

                   =2 IS NOT SELF-GUARDING BY HAND. It fails a rollback that
                   did not happen, but a run where the halfword never moved
                   between the save and the load still exits 0, because a bare
                   walk_window has no way to know whether the caller meant to
                   drive a level change. port/tools/savestate_soak.py is the
                   guard. Read a hand-run's lines, do not read its exit code.

                   WHAT THE SOAK READS NOW (run mg15 lane RELOAD2). The
                   halfword was the wrong anchor: it reads the same whether
                   ov009's record is reached through the mount's rebased
                   pointer or through the raw DS reservation, so the arm it
                   guarded could only ever report VACUOUS, and it did, on every
                   level, for as long as it existed. The line that moves is the
                   STORAGE COVERAGE printed above -- ".dsstate (captured)"
                   against "NOT CAPTURED" -- and the soak asserts its three
                   transitions in order: the pass ran, a restore rolled it
                   back, the pass ran again. */
                if (const char *w = getenv("SM64DS_SS_WATCH_FLAG"))
                    ss_watch = atoi(w);
            }
            /* Every change of the watched halfword, as it happens. A save
               state can only miss a rollback for a byte that actually moves,
               so a run where this prints nothing has not tested anything and
               must not be read as a pass. */
            if (ss_watch) {
                static int gap_seeded;
                static unsigned gap_last;
                const unsigned gap_now =
                    *(const unsigned short *)(port_ov009_gap_0211222c + 12);
                if (!gap_seeded || gap_now != gap_last) {
                    fprintf(stderr, "[ss-gap] f%d port_ov009_gap_0211222c+12 "
                            "%04x -> %04x, block %s\n", frame,
                            gap_seeded ? gap_last : gap_now, gap_now,
                            lk6_savestate_covers(port_ov009_gap_0211222c)
                                ? "captured" : "NOT CAPTURED");
                    gap_seeded = 1;
                    gap_last = gap_now;
                }
            }
            if (ss_watch) {
                static int seeded;
                static unsigned last;
                /* THE COVERAGE IS THE MEASUREMENT, not just the value.
                   Whether the pointer leads into .dsstate or into the raw DS
                   reservation is exactly what a mount's patch pass decides,
                   and the id halfword behind it reads 0000 on BOTH sides of
                   that -- so a watch keyed only on the value prints one line
                   at f0 and never again, which is why the soak's cross-level
                   arm could only ever report VACUOUS. Keying on the pair makes
                   the line fire when the pass runs, when a restore rolls it
                   back, and when it runs again. */
                static int last_cov = -1;
                int cov = 0;
                const unsigned now = ss_flag_word(&cov);
                if (!seeded || now != last || cov != last_cov) {
                    fprintf(stderr, "[ss-flag] f%d ov009 water-anim id "
                            "%04x -> %04x, storage %s\n", frame,
                            seeded ? last : now, now,
                            cov & 1 ? "in .dsstate (captured)"
                                    : (cov & 2 ? "in the arena (captured)"
                                               : "NOT CAPTURED"));
                    ss_flag_dump(seeded ? "after the change" : "first look");
                    seeded = 1;
                    last = now;
                    last_cov = cov;
                }
            }
            if (ss_save_fr >= 0 && frame == ss_save_fr) {
                lk6_savestate_save();
                ss_census("at the scripted SM64DS_SS_SAVE", player, cam);
                /* the cross-restart reproducer's first half: mirror this save
                   to savestate.bin so a SECOND run (SM64DS_SS_DISKLOAD=1) can
                   boot from it and compare hashes across the restart */
                if (ss_disk)
                    fprintf(stderr, "[ss-repro] f%d disk write: %s\n", frame,
                            lk7_persist_write() ? "ok" : "SKIPPED/FAILED");
                ss_lock_at_save = data_0209d660;
                ss_hash_at_save = ss_hw_hash();
                ss_saw_save = 1;
                if (ss_watch) {
                    int cov = 0;
                    const unsigned live = ss_flag_word(&cov);
                    ss_flag_at_save = *(const unsigned short *)
                        (port_ov009_gap_0211222c + 12);
                    fprintf(stderr, "[ss-flag] f%d save: gap+12=%04x, live "
                            "id=%04x in %s; gap block %s\n", frame,
                            ss_flag_at_save, live,
                            cov & 1 ? ".dsstate" : (cov & 2 ? "the arena"
                                                            : "NOTHING "
                                                              "CAPTURED"),
                            lk6_savestate_covers(port_ov009_gap_0211222c)
                                ? "captured -- a load rolls it back"
                                : "NOT CAPTURED -- a load cannot roll it "
                                  "back");
                }
                fprintf(stderr, "[ss-repro] f%d save: msglock(d660)=%u "
                        "hw=%016llx\n", frame, (unsigned)data_0209d660,
                        ss_hash_at_save);
            }
            /* perturb the out-of-arena lock strictly between save and load, so
               the world provably diverges on a hosted global the arena copy
               does not own */
            if (ss_lock && ss_saw_save && ss_load_fr >= 0 &&
                frame > ss_save_fr && frame < ss_load_fr)
                data_0209d660 = 1;
            if (ss_load_fr >= 0 && frame == ss_load_fr) {
                const unsigned long long pre = ss_hw_hash();
                fprintf(stderr, "[ss-repro] f%d pre-load: msglock(d660)=%u "
                        "hw=%016llx (%s the saved hash)\n", frame,
                        (unsigned)data_0209d660, pre,
                        pre == ss_hash_at_save ? "STILL" : "differs from");
                if (ss_expect_mount && ss_saw_save && pre == ss_hash_at_save) {
                    fprintf(stderr, "[ss-repro] FAIL(vacuous): the hardware "
                            "stores never changed between save and load, so "
                            "no area mounted and a cross-area restore was not "
                            "tested. Check SM64DS_EXIT_ENTER's index/frame "
                            "and give the mount more frames.\n");
                    ntr::ppu_write_bmp("walk_window_selftest.bmp", fb);
                    return 5;
                }
                /* A rollback test over a byte that never moved proves
                   nothing, and it fails silently in the direction of looking
                   green. Say so before the load rather than after.

                   THE HALFWORD IS NO LONGER THE ARM'S VERDICT, and the wording
                   says so, because a line reading VACUOUS in a run that PASSES
                   is its own little trap. This byte reads the same on both
                   sides of the mount's patch pass, so it can be expected to sit
                   still; port/tools/savestate_soak.py reads the COVERAGE
                   transitions in the [ss-flag] lines instead, which do move.
                   Kept because a run where it DOES move is still worth
                   seeing. */
                if (ss_watch && ss_saw_save) {
                    const unsigned pre_gap = *(const unsigned short *)
                        (port_ov009_gap_0211222c + 12);
                    fprintf(stderr, "[ss-flag] f%d pre-load: gap+12=%04x "
                            "(saved %04x)%s\n", frame, pre_gap,
                            ss_flag_at_save,
                            pre_gap == ss_flag_at_save
                                ? " -- this halfword did not move, so it tests "
                                  "no rollback (the soak's verdict comes from "
                                  "the storage-coverage lines above)" : "");
                }
                if (lk6_savestate_load()) {
                    an_pivot_live = 0;
                    ss_census("after the scripted SM64DS_SS_LOAD restore",
                                   player, cam);
                    ss_reseat("after the scripted SM64DS_SS_LOAD restore");
                }
                const unsigned long long post = ss_hw_hash();
                fprintf(stderr, "[ss-repro] f%d post-load: msglock(d660)=%u "
                        "(saved %u) hw=%016llx\n", frame,
                        (unsigned)data_0209d660, (unsigned)ss_lock_at_save,
                        post);
                if (ss_assert && data_0209d660 != ss_lock_at_save) {
                    fprintf(stderr, "[ss-repro] FAIL: message-lock global "
                            "data_0209d660 did NOT roll back on restore "
                            "(post-load %u, saved %u) -- a hosted DS global "
                            "outside the capture set survived the load\n",
                            (unsigned)data_0209d660, (unsigned)ss_lock_at_save);
                    ntr::ppu_write_bmp("walk_window_selftest.bmp", fb);
                    return 3;
                }
                /* the reference hash: this run's own save when there was one,
                   else the one passed in for a cross-restart run. Neither set
                   means there is nothing sound to hold the restore to. */
                const unsigned long long want =
                    ss_saw_save ? ss_hash_at_save : ss_expect_hw;
                if (ss_assert && want && post != want) {
                    fprintf(stderr, "[ss-repro] FAIL: the hardware stores did "
                            "NOT roll back on restore (post-load %016llx, "
                            "expected %016llx) -- the world came back under "
                            "another area's textures\n", post, want);
                    ntr::ppu_write_bmp("walk_window_selftest.bmp", fb);
                    return 4;
                }
                if (ss_watch && ss_saw_save) {
                    const unsigned post_gap = *(const unsigned short *)
                        (port_ov009_gap_0211222c + 12);
                    const int rolled = post_gap == ss_flag_at_save;
                    fprintf(stderr, "[ss-flag] f%d post-load: gap+12=%04x "
                            "(saved %04x) -- %s\n", frame, post_gap,
                            ss_flag_at_save,
                            rolled ? "ROLLED BACK" : "did NOT roll back");
                    if (ss_watch >= 2 && !rolled) {
                        fprintf(stderr, "[ss-flag] FAIL: a packed mount's DS "
                                "storage did NOT roll back on restore. The "
                                "bytes are ov009 DS 0x02112238, the castle "
                                "water's texture animation record reached "
                                "through the BTA_File at "
                                "data_ov009_02112bc4+20 (NOT FLAG's, whatever "
                                "this env is called), and they are outside the "
                                "captured span -- the 0.2.0 ANIM_PTRS class "
                                "again, this time in a synthetic gap block "
                                "(port/tools/ovdata.py, audited by "
                                "port/tools/gapaudit.py)\n");
                        ntr::ppu_write_bmp("walk_window_selftest.bmp", fb);
                        return 6;
                    }
                }
                if (ss_assert)
                    fprintf(stderr, "[ss-repro] PASS: message-lock global and "
                            "the hardware stores rolled back on restore\n");
            }
        }
        /* drain what the window procedure collected. Unconditionally, so a
           drag taken in DS-exact mode does not pile up and dump into the rig
           the moment F1 hands it the view. MOUSE_YAW is 48 binangs a pixel --
           about 1400 pixels for a full turn -- and the tilt is half that.
           Positive dy is downward and lowers the view the same way pushing the
           right stick down does, so the mouse and the pad agree. */
        int mouse_dyaw = 0, mouse_dpitch = 0, mouse_wheel = 0;
        {
            const int MOUSE_YAW = 48, MOUSE_PITCH = 24;
            /* mo_captured reads exactly like mo_look here, which is the whole
               of MouseCapture as far as the camera is concerned: same
               variables, same constants, same rig. */
            if ((mo_look || mo_captured) && !selftest) {
                mouse_dyaw = mo_dx * MOUSE_YAW;
                mouse_dpitch = mo_dy * MOUSE_PITCH;
            }
            mo_dx = mo_dy = 0;
            if (!selftest) mouse_wheel = mo_wheel;
            mo_wheel = 0;
        }

        /* Top of the DS 2D frame: both OAM shadows back to empty, and the
           stylus record refreshed from the mouse. Everything the game's own
           Render methods emit this frame lands on top of that. */
        hal_sub_screen_frame_begin();

        /* keys -> pad block + desired heading, CAMERA-RELATIVE: W walks
           away from the camera whatever way it faces. cam_yaw is the
           camera's heading around Mario (radians; 0 = classic behind-south
           view). Q/E orbit it; when Mario walks and Q/E are idle it eases
           in behind his motion like the real game's lazy camera. */
        int dx = 0, dz = 0;
        if (selftest && !getenv("SM64DS_SELFTEST_IDLE")) {
            dz = 1;
            /* turn probe: hold "A" from frame 60 -- position x must curve */
            if (getenv("SM64DS_SELFTEST_TURN") && frame >= 60) dx = -1;
            /* release probe: let go at speed (the brake/skid path) */
            if (getenv("SM64DS_SELFTEST_RELEASE") && frame >= 50) dz = 0;
            /* reversal probe: hard 180 at speed (the skid-turn path) */
            if (getenv("SM64DS_SELFTEST_REVERSE") && frame >= 50) dz = -1;
            /* decel probe: stick fully forward, then nothing at all */
            if (decel_probe == 1 && frame >= DECEL_RELEASE) dz = 0;
            /* =3 is the turn: a hard 180 at full run, which is the input
               that should throw him into the skid state. "Tight turns drift
               wide" is this path, so the log wants the heading too. */
            if (decel_probe == 3 && frame >= DECEL_RELEASE) dz = -1;
            /* =4 is the CHARGED DASH, and it is the one that matters. Stand
               still with the run button held: St_Wait_Main runs 020d2fdc,
               which counts +0x6e5 up to 0x1e and then arms the 30-frame dash
               window at +0x6ed. Only then push the stick. That is the only
               input that reaches the boost multiply in the walk core, which
               is why every other probe here looked clean. */
            if (decel_probe == 4 && frame < DASH_CHARGE_UNTIL) dz = 0;
        }
        if (key_live('W') || key_live(VK_UP)) dz += 1;
        if (key_live('S') || key_live(VK_DOWN)) dz -= 1;
        if (key_live('A') || key_live(VK_LEFT)) dx -= 1;
        if (key_live('D') || key_live(VK_RIGHT)) dx += 1;
        /* gamepad: left stick / d-pad walk, right stick orbits + tilts.
           Gated off under a selftest with the keyboard: a drifting stick
           on a plugged-in pad perturbs a headless run the same way. */
        static XPad pad;
        int pad_live = !selftest && XInputGetState_ && XInputGetState_(0, &pad) == 0;
        int orbiting = 0;
        /* SM64DS_PAD_TEST: DBG1's scripted pad, at file scope now so the
           windowed scene loop gets the same one. Inert unless the variable is
           set and unreachable from a selftest; see its banner. */
        pad_test_apply(frame, &pad_live, &pad);
#ifndef PORT_ROM_CLEAN
        /* SM64DS_CLICK_TEST: the scripted stylus, the same call the windowed
           scene loop makes and for the same reason -- the level path is where
           the inset panel's transform lives, so it is the half of the click
           grid that guards against a stacked fix moving a level. */
        click_test_apply(hwnd, frame);
#endif
        /* ---- THE REBIND CAPTURE, ahead of every other reader of this frame's
           input. The keyboard half already arrived through the window
           procedure (see g_rebind_capture up there); this is the pad half and
           the decision about what to do with either.

           Ending with pad_live cleared is the pad's version of the one line
           inside key_live: past this point the frame sees no pad at all, so
           the button being bound cannot also open the menu, toggle the freecam
           or nudge the camera on its way to becoming a binding. */
        if (g_rebind_capture) {
            static unsigned bind_pad_prev = ~0u;   /* first frame: no edges */
            const unsigned now = pad_live ? (unsigned)pad.buttons : 0u;
            const unsigned fresh = now & ~bind_pad_prev;
            bind_pad_prev = now;
            int done = 0;
            if (g_rebind_key) {
                const int vk = g_rebind_key;
                g_rebind_key = 0;
                if (vk == VK_ESCAPE) {
                    ss_note("rebind cancelled");
                    done = 1;
                } else if (run_key_reserved(vk)) {
                    ss_note("that key belongs to the window, pick another");
                } else {
                    char kb[16], msg[64];
                    g_run_key = vk;
                    host_setting_save_run(g_run_mode, g_run_key, g_run_pad);
                    snprintf(msg, sizeof msg, "run is now %s",
                             run_key_name(vk, kb, sizeof kb));
                    ss_note(msg);
                    done = 1;
                }
            } else if (fresh) {
                /* one button per press: the lowest set bit, so a thumb that
                   lands on two at once binds one of them instead of a mask
                   that no single press can ever reproduce */
                const unsigned bit = fresh & (unsigned)(-(int)fresh);
                if (run_pad_reserved(bit)) {
                    ss_note("that pad button drives the menu, pick another");
                } else {
                    char pb[16], msg[64];
                    g_run_pad = (int)bit;
                    host_setting_save_run(g_run_mode, g_run_key, g_run_pad);
                    snprintf(msg, sizeof msg, "run is now %s",
                             run_pad_name((int)bit, pb, sizeof pb));
                    ss_note(msg);
                    done = 1;
                }
            }
            if (done) {
                g_rebind_capture = 0;
                bind_pad_prev = ~0u;
                /* every key that is down right now is marked stale, the same
                   trick the focus-regained edge uses: the key just bound is
                   still physically held, and without this it would read as a
                   fresh press the instant the capture ends */
                memset(key_stale, 1, sizeof key_stale);
            }
            pad_live = 0;
        }
        /* ---- THE DEBUG MENU, and the close that must not also be a punch.
           Both live at file scope now (menu_input / menu_b_swallow_spend
           above) so the windowed scene loop runs the same menu rather than a
           second one. g_menu_host is what THIS loop has under it that a scene
           has not -- the Player, the Camera and a real game camera -- and it
           is refreshed every frame because a level change re-seats both
           pointers. */
        g_menu_host.player = c;
        g_menu_host.cam = cam;
        g_menu_host.real_camera = real_camera;
        menu_input(pad_live, &pad);
        menu_b_swallow_spend(pad_live, &pad);
        /* MouseCapture, asked and acted on ONCE A FRAME and deliberately right
           here: after menu_input, so a menu opened by this frame's escape has
           already handed the pointer back before anything draws, and before the
           mouse deltas are drained further down, so a frame never steers on a
           delta collected while the capture was not on. mo_capture_set is
           idempotent, so the steady state is one comparison. The scene loop
           does NOT have this line and must not: the mouse is the stylus there.
           See the MOUSE banner for the whole list. */
        mo_capture_set(hwnd, mo_capture_want(selftest, stacked));
        /* the right stick's X, from the pad or from the selftest ramp:
           SM64DS_SELFTEST_STICK=<pct> holds it at pct% of full deflection
           from frame 20 (negative for the other way), and =0 ramps it from
           nothing to the stop. Either way it LETS GO three quarters of the
           way through, so one run shows both the orbit and the handover
           back to the game's own auto-behind. The only way to exercise any
           of it without a person on the controller. */
        int stick_rx = pad_live ? pad.rx : 0;
        int stick_ry = pad_live ? pad.ry : 0;
        if (selftest) {
            const char *sk = getenv("SM64DS_SELFTEST_STICK");
            const int last = 20 + 3 * (selftest - 20) / 4;
            if (sk) stick_rx = 0;
            if (sk && frame >= 20 && frame < last) {
                int pct = atoi(sk);
                if (!pct && last > 21)
                    pct = 100 * (frame - 20) / (last - 20);
                stick_rx = 32767 * pct / 100;
            }
        }
        /* ---- the freecam toggle (port mod, see the block above the window
           procedure). F1 or a click of the right stick; SM64DS_FREECAM=1
           starts in it, and SM64DS_SELFTEST_FREECAM=1 turns it on at frame
           20 of a selftest so the rig can be probed without a person. */
        if (real_camera) {
            static int fc_edge, fc_boot;
            if (!fc_boot) {
                fc_boot = 1;
                /* the window plays in analog; the selftest stays DS-exact
                   unless it is asked otherwise (see the mode block above) */
                cam_mode = selftest ? CAM_DS : CAM_ANALOG;
                if (getenv("SM64DS_ANALOG_CAMERA")) cam_mode = CAM_ANALOG;
                if (getenv("SM64DS_DS_CAMERA")) cam_mode = CAM_DS;
                if (getenv("SM64DS_FREECAM")) cam_mode = CAM_FREE;
                if (cam_mode != CAM_DS) fc_seed(cam);
            }
            int now = key_live(VK_F1) ||
                      (pad_live && (pad.buttons & 0x0080));
            if (selftest && getenv("SM64DS_SELFTEST_FREECAM")) {
                /* the probe wants the mod ON at 20 and OFF three quarters
                   through, which a three-way cycle cannot express -- so set
                   the mode outright and leave the edge alone. */
                if (frame == 20) { cam_mode = CAM_FREE; fc_seed(cam); }
                if (frame == 20 + 3 * (selftest - 20) / 4) cam_mode = CAM_DS;
                now = 0;
            }
            if (now && !fc_edge) {
                cam_mode = (cam_mode + 1) % 3;   /* analog -> freecam -> DS */
                if (cam_mode != CAM_DS) fc_seed(cam);
                if (cam_mode == CAM_ANALOG) an_pivot_live = 0;
                fprintf(stderr, "[cam] mode %s\n", cam_mode_name(cam_mode));
            }
            fc_edge = now;
        }
        if (cam_mode != CAM_DS) {
            /* the rig's own frame: orbit and tilt at a rate proportional to
               the stick, zoom on the bumpers or R/F, C back behind Mario.
               `rig_touched` is what tells the analog auto-recenter to keep its
               hands off -- the player is aiming the camera. */
            int rig_touched = 0;
            /* the distance the game's camera converged to on the frame just
               gone -- see the block above fc_seed. Ahead of the zoom keys so
               a press this frame still lands on top of it. */
            if (cam && !fc_dist_owned) fc_dist = fc_cam_dist(cam);
            {
                /* stick and mouse are both a rightward push measured to the
                   right, so both take cam_turn as-is */
                const int r = (fc_stick_rate(stick_rx, CAM_STEP) + mouse_dyaw)
                              * cam_turn;
                if (r) { fc_yaw = (short)(fc_yaw + r); rig_touched = 1; }
            }
            {
                int t = fc_pitch - fc_stick_rate(stick_ry, CAM_STEP / 2)
                        + mouse_dpitch;
                if (mouse_dpitch) rig_touched = 1;
                if (key_live('R')) t += 0x80;
                if (key_live('F')) t -= 0x80;
                if (t > 0x3a00) t = 0x3a00;      /* just short of overhead */
                if (t < -0x1000) t = -0x1000;    /* a little from below */
                fc_pitch = (short)t;
            }
            {
                /* Q pushes left, E pushes right, off the same sign as the
                   stick so the keyboard and the pad cannot disagree */
                const int qe = (CAM_STEP / 2) * cam_turn;
                if (key_live('Q')) { fc_yaw = (short)(fc_yaw - qe); rig_touched = 1; }
                if (key_live('E')) { fc_yaw = (short)(fc_yaw + qe); rig_touched = 1; }
            }
            {
                int zoom = 0;
                if (pad_live && (pad.buttons & 0x0100)) zoom -= 1;   /* LB */
                if (pad_live && (pad.buttons & 0x0200)) zoom += 1;   /* RB */
                zoom -= mouse_wheel;   /* wheel forward pulls the eye in */
                if (zoom) {
                    /* the player has taken the distance off the game; the
                       tracking above stands down until the next fc_seed */
                    fc_dist_owned = 1;
                    fc_dist += zoom * (fc_dist >> 5);
                    if (fc_dist < 0x30000) fc_dist = 0x30000;
                    if (fc_dist > 0x2000000) fc_dist = 0x2000000;
                }
            }
            if (key_live('C')) {
                fc_yaw = (short)(*(short *)(c + 0x8e) + 0x8000);
                rig_touched = 1;
            }
            /* THE AUTO-RECENTER, analog only. Nothing happens while the player
               is steering the camera, nothing happens while Mario is standing
               still, and nothing happens inside eleven degrees of behind him --
               the last one is what keeps it from hunting around the target.
               Outside that it closes a twentieth of the error a frame, capped
               at 0x200 binangs (2.8 degrees, 84 a second), so the worst case --
               the player has spun Mario right around and let go -- settles in
               about two seconds and nothing in it ever reads as a snap. During
               a sustained turn the proportional term is what binds, and the
               camera trails him by a dozen degrees or so, which is the lag
               that makes it feel like a camera rather than a bracket. */
            if (cam_mode == CAM_ANALOG && !rig_touched) {
                const int spd = *(int *)(c + 0x98);
                if (spd > (2 << 12) || spd < -(2 << 12)) {
                    const short behind = (short)(*(short *)(c + 0x8e) + 0x8000);
                    int d = (short)(behind - fc_yaw);
                    if (d > 0x800 || d < -0x800) {
                        d /= 20;
                        if (d > 0x200) d = 0x200;
                        if (d < -0x200) d = -0x200;
                        fc_yaw = (short)(fc_yaw + d);
                    }
                }
            }
        }
        if (pad_live) {
            if (pad.ly > 12000 || (pad.buttons & 1)) dz += 1;
            if (pad.ly < -12000 || (pad.buttons & 2)) dz -= 1;
            if (pad.lx < -12000 || (pad.buttons & 4)) dx -= 1;
            if (pad.lx > 12000 || (pad.buttons & 8)) dx += 1;
            if (pad.rx < -10000 || pad.rx > 10000) {
                cam_yaw += cam_turn * 0.045f * (pad.rx / 32768.0f);
                orbiting = 1;
            }
            if (pad.ry > 10000 && cam_pitch < 0.85f) cam_pitch += 0.02f;
            if (pad.ry < -10000 && cam_pitch > -0.15f) cam_pitch -= 0.02f;
        }
        /* the pre-Camera-actor dev rig (SM64DS_OLD_CAMERA). It never reaches a
           player, but it takes cam_turn too so nobody debugging in it has to
           remember that this one camera turns the other way. */
        if (key_live('Q')) { cam_yaw -= cam_turn * 0.045f; orbiting = 1; }
        if (key_live('E')) { cam_yaw += cam_turn * 0.045f; orbiting = 1; }
        if (key_live('R') && cam_pitch < 0.85f)
            cam_pitch += 0.02f;
        if (key_live('F') && cam_pitch > -0.15f)
            cam_pitch -= 0.02f;
        /* THE GAME'S OWN INPUT PROCESSOR: keys become raw DS pad bits,
           Stage::CheckInput turns them into the stick record (mag, dir,
           binang -- the D-pad path, mode 0), and Player::Behavior folds
           in the camera angle via GetAngleToCamera, which reads the
           angle the harness publishes below. No hand-built headings. */
        /* the menu owns the arrow keys and the d-pad while it is open, so no
           walk comes out of them; the tick is skipped below either way, but
           the stick record should not be left describing a press either */
        if (menu_on) { dx = 0; dz = 0; }
        {
            unsigned short raw = 0;
            if (dz > 0) raw |= 0x40;   /* up    */
            if (dz < 0) raw |= 0x80;   /* down  */
            if (dx < 0) raw |= 0x20;   /* left  */
            if (dx > 0) raw |= 0x10;   /* right */
            static unsigned short raw_prev;
            /* run mg16 lane MP3: the SAME value the pad mirror gets, stashed
               for hal/comms_conductor.cpp's key-register publish further down
               the frame. Taken HERE, at the source, because the ROM's fan-out
               (func_0203bc7c, when SM64DS_COMMS_FANOUT is on) rewrites that
               mirror from the four comms records later in this same frame --
               so reading the mirror at publish time would feed the wire back
               into itself. Stashing the source value is what makes the
               ordering a fact rather than a comment. */
            port_raw_pad_stash(raw);
            /* THE ROM'S FAN-OUT OWNS PadData[4] ONCE A SESSION IS UP, and this
               line must get out of its way. Run mg16 lane MP3, field failure.

               These two stores put the LOCAL pad into slot 0 of the mirror,
               every frame, immediately before Stage::CheckInput. That is right
               for a single-player port -- the local player IS slot 0 -- and it
               is two separate bugs in a session:

                 ON THE CHILD the local player is slot 1, so the local pad drove
                 the HOST'S character. That is the owner's "from P2 I can move
                 both Mario and Luigi".
                 AND ON THE CHILD IT ALSO CLOBBERED what func_0203bc7c had just
                 fanned out into slot 0 -- which on a child is the HOST'S record
                 -- so the other console's presses were overwritten by the local
                 pad before any reader saw them. That is his "nothing I do on P1
                 shows up on P2".
                 ON THE PARENT the same store is not destructive, and saying so
                 is the point: slot 0 IS the parent's own record, so the write
                 duplicates the value the fan-out would have delivered a frame
                 later. Both defects above are the CHILD's, and an earlier
                 version of this comment said "on both sides", which would send
                 the next reader looking for a host-side bug that is not there.

               The local pad is NOT lost by skipping this: it reaches the mirror
               the ROM's own way, and that is the entire point of the lane.
               port::comms_publish_pad puts it in the DS key register,
               src/func_0203df40.c reads the register into the local comms
               record, src/func_0203ea5c.c stages that onto the wire, and
               src/func_0203bc7c.c fans all four records back out into
               data_020a0e58 -- into THIS console's own slot, whichever that is.
               A frame later, which is what lockstep means.

               port/slice_comms.txt predicted this exact hand-off and named it:
               "the port's own input path still writes TouchInfo and PadData
               directly, and replacing that is MP2's change because that is the
               one with a regression surface." It was never done, and the
               regression surface is where the owner found it.

               GATED, so single player is untouched: with no transport, or with
               the fan-out off, nothing writes those records and these stores
               stay exactly what they were. */
            if (!(port::comms_transport() && comms_fanout_on())) {
                *(unsigned short *)((char *)data_020a0e58 + 0) = raw;
                *(unsigned short *)((char *)data_020a0e58 + 2) =
                    (unsigned short)(raw & (unsigned short)~raw_prev);
            }
            raw_prev = raw;
            /* the angle FROM Mario TO the camera (what the name
               GetAngleToCamera means): the D-pad table's "up" entry is
               0x8000, so up + angle-to-camera = away from the lens.
               Under the real camera this is NOT written by hand: the
               camera publishes its own heading through func_0203dafc ->
               data_020a1040 -> func_0203e0ac -> data_020a1154, and
               GetAngleToCamera reads the far end of that chain. */
            if (!real_camera)
                *(short *)((char *)data_020a1164 + 0) =
                    (short)((int)(cam_yaw * (32768.0f / 3.14159265f)) + 0x8000);
            /* TEMPORARY headless input: OR any SM64DS_PROBE_INPUT press for this
               frame into the raw pad mirror BEFORE CheckInput, so the remap and
               the direct readers (IsButtonInputValid, Message::Update) both see
               it. Does nothing without the env var. */
            port_input_probe_apply(frame);
            _ZN5Stage10CheckInputEv();
            /* the matched TU writes its own data_0209f498 block; older
               TUs read per-field split symbols -- copy the record out
               ----------------------------------------------------------------
               run mg16 lane MP3: FOR EVERY PLAYER, AT THE DS's OWN STRIDE.
               This copied slot 0 and only slot 0, which was right while the
               port asserted one player and is the exact reason a second
               player could receive input and still not move.

               ON THE DS THESE FIVE NAMES ARE INTERIOR ADDRESSES OF THE Ctrl
               BLOCK ITSELF: data_0209f498 + 8, +0xa, +0xc, +0xe and +0x14,
               with Ctrl striding 0x18 per player. That is why
               port/unmatched/Player_Behavior.cpp reads its stick angle as
               `*(s16 *)((char *)&data_0209f4a6 + data_020a0e40 * 0x18)` --
               on hardware that walks straight into player N's own Ctrl record.
               The port hosts the five as SEPARATE arrays, so the same walk
               lands inside data_0209f4a6's own storage instead, reading a byte
               nobody wrote. Slot 0 worked because its offset is zero.

               So the copy writes each player's fields at that same 0x18 stride,
               which puts player N's values exactly where the ROM's walk looks
               for them.

               THE CEILING IS GONE, and it was never a layout fact. An
               earlier revision of this comment said the split symbols could
               not be enlarged because they sit interior to .dsstate and
               growing them retires every BMP baseline. The first half was
               wrong: they are SEPARATE host symbols, not interior addresses,
               and this tree already sizes two of the family
               (data_0209f4ac/data_0209f4ae) at 0x18 * 4 for exactly this
               reason -- hal/actor_vtables.cpp carries the note about the stray
               that taught it. The rest now match. Baselines do move, which is
               what rung 1's position check is for. */
            {
                const char *q = (const char *)data_0209f498;
                int np = (int)data_0209f21c;
                if (np < 1) np = 1;
                if (np > 4) np = 4;
                for (int pi = 0; pi < np; ++pi) {
                    const char *r = q + pi * 0x18;
                    const int o = pi * 0x18;
                    /* THE BUTTONS, and leaving them out was the second input
                       seam. Ctrl+0x04 is the HELD word and +0x06 is
                       pressed-this-frame, and they are what every button
                       reader in the game uses -- crouch among them. Fanning
                       the stick fields and not these meant movement routed to
                       the right player and one button family did not: crouch
                       in the child's window crouched MARIO in the child's
                       world. Same one-line character as the PadData[0]
                       clobber, one layer further in. */
                    *(short *)((char *)data_0209f49c + o) = *(const short *)(r + 0x04);
                    *(short *)((char *)data_0209f49e + o) = *(const short *)(r + 0x06);
                    *(short *)((char *)data_0209f4a0 + o) = *(const short *)(r + 0x08);
                    *(short *)((char *)data_0209f4a2 + o) = *(const short *)(r + 0x0a);
                    *(short *)((char *)data_0209f4a4 + o) = *(const short *)(r + 0x0c);
                    *(short *)((char *)data_0209f4a6 + o) = *(const short *)(r + 0x0e);
                    *((unsigned char *)data_0209f4ac + o) = *(const unsigned char *)(r + 0x14);
                }
            }
            /* ---- RUN MODE ANALOG: the record, refilled from the pad's left
               stick. See the RUN_ mode block up by the menu enum for why this
               is the game's own analog path and not a new one.

               CheckInput has just written the D-PAD answer: magnitude pinned
               at 0x1000, touching zero, direction quantized to the eight
               table entries. That is right for a keyboard and wrong for a
               stick, so when there IS a stick and it is off its rest, the
               same five fields are written again the way CheckInput's TOUCH
               branch writes them -- and every reader downstream, the walk
               core included, cannot tell which branch filled them in.

               The mapping, end to end. XInput reports each axis as +-32767,
               so the deflection is the radius sqrt(lx^2+ly^2) clamped to the
               stop, and the usable travel starts at XInput's own left-stick
               floor (7849) rather than at zero, so a stick that rests a
               little off centre is still at rest:

                   mag = 0x1000 * (len - 7849) / (32767 - 7849)

               clamped into 0..0x1000. Direction goes in as the touch screen's
               axes -- x right, y DOWN, which is why the stick's up becomes a
               negative dy -- and the angle through the ROM's own atan2 rather
               than a host one, so it lands on the same table entry the
               hardware would have picked.

               What the game then does with it is entirely the game's:
               func_ov002_020bf224 scales the speed target by mag/0x1000, so
               the speed is linear in that deflection, and the walk/run flag
               flips when mag crosses 0xdc7 with 0x80 of hysteresis -- 0xe47
               of 0x1000 to break into a run and back under 0xdc7 to drop out
               of it, which is 89.2 and 86.1 percent of the usable travel, or
               91.8 and 89.4 percent of the whole stick once the dead zone is
               counted back in. Those are the ROM's numbers and nothing here
               touches them -- so running in this mode does mean pushing the
               stick most of the way, which is what pushing the touch stick
               most of the way did.

               A stick inside the dead zone falls through with CheckInput's
               answer untouched, which is button mode -- so analog mode on a
               keyboard, or with the pad put down, is exactly the program that
               shipped before. */
            /* ---- THE FOURTH SLOT-0 WRITER, AND THE ONE NO RUNG COULD SEE
             *
             * Run mg16 lane MP4. Same shape as the three already gated -- it
             * writes THIS console's stick straight into Ctrl slot 0, on the
             * level path, AFTER the per-player fan -- and it is the stick
             * family rather than the buttons, so on a child it would drive the
             * HOST's character and destroy the host's own stick values in one
             * store.
             *
             * IT WAS INVISIBLE TO EVERY RUNG BY CONSTRUCTION. The guard
             * includes pad_live, which is XInputGetState succeeding, and no
             * harness has a physical gamepad -- so every green ladder in this
             * campaign ran with this block switched off. It would have bitten
             * the owner the first time he played multiplayer with a controller
             * in analog mode, which is a configuration no proof had ever
             * entered. The reviewer found it by reading, which is the only way
             * it could have been found.
             *
             * SM64DS_FORCE_ANALOG makes it reachable from a proof: it forces
             * the run-mode half of the condition, and SM64DS_PAD_TEST already
             * forces the pad_live half in play mode (it fakes a pad and sets
             * pad_live=1 at walk_window.cpp:3442, and is deliberately disabled
             * under selftest). Together they cover the block with no hardware.
             * Test scaffolding of the same class as SM64DS_SYNC_FORCE_V1 and
             * SM64DS_SYNC_DROP, and named here so it is not mistaken for a
             * player-facing setting. */
            static int force_analog = -1;
            if (force_analog < 0)
                force_analog = getenv("SM64DS_FORCE_ANALOG") ? 1 : 0;
            const int analog_mode = force_analog || run_mode() == RUN_ANALOG;
            if (!selftest && analog_mode && pad_live && !menu_on &&
                !(port::comms_transport() && comms_fanout_on())) {
                const int DEAD = 7849;      /* XInput's left-stick floor */
                const int FULL = 32767;
                const int dxs = pad.lx;
                const int dys = -pad.ly;    /* stick up is touch-screen up */
                double len = sqrt((double)dxs * dxs + (double)dys * dys);
                if (len > FULL) len = FULL;
                if (len > DEAD) {
                    int mag = (int)(4096.0 * (len - DEAD) / (FULL - DEAD));
                    if (mag > 0x1000) mag = 0x1000;
                    if (mag < 0) mag = 0;
                    *(short *)(data_0209f4a0 + 0) = (short)mag;
                    *(short *)data_0209f4a2 =
                        (short)((double)dxs * mag / len);
                    *(short *)data_0209f4a4 =
                        (short)((double)dys * mag / len);
                    *(short *)data_0209f4a6 =
                        _ZN4cstd5atan2E5Fix12IiES1_(dxs, dys);
                    /* the field the walk/run branch keys off: "the player is
                       on the analog stick, read the deflection" */
                    data_0209f4ac[0] = 1;
                }
            }
            /* camera lazy-follow, from the same intended direction */
            if ((dx || dz) && !orbiting) {
                float head = cam_yaw + atan2f((float)-dx, (float)dz);
                float d = head - cam_yaw;
                while (d > 3.14159265f) d -= 2 * 3.14159265f;
                while (d < -3.14159265f) d += 2 * 3.14159265f;
                if (d > -1.35f && d < 1.35f)
                    cam_yaw += d * 0.015f;
            }
        }

        /* Buttons -> the Ctrl held/pressed fields directly (CheckInput's
           remap tables are ROM pointers with no host image). The four bits
           this path and the windowed scene path must not disagree about are
           host_ds_buttons' (jump, punch, crouch and the run button the player
           bound, off the keyboard and off the pad); everything below is this
           path's own tail -- the camera-rotate bits behind func_02009e70's own
           reader, run-mode AUTO, and the selftest probes -- and it is
           deliberately not shared, because every level selftest frame in the
           battery is a function of it. */
        {
            static unsigned short btn_was;
            unsigned short btn = host_ds_buttons(pad_live, &pad);
            /* ---- RUN MODE AUTO: the run bit, held for them. Literally that
               and nothing else -- no exception for standing still, because
               the honest reading of "always running, no button" is that the
               button is always down, and inventing a host-side exception
               would put a rule in the input layer that the game does not have.
               It follows that the charged dash St_Wait_Main builds while the
               run button is held standing still (+0x6e5 counting to 0x1e,
               then the 30-frame window at +0x6ed) is ALWAYS armed in this
               mode. That is what holding the button does on hardware; it is
               listed as a thing to feel rather than papered over here.

               Never under a selftest and never with the menu open: run_mode()
               is pinned to button for the first and btn is zeroed below for
               the second, but this reads menu_on itself so the intent is at
               the line rather than three lines away. */
            if (run_mode() == RUN_AUTO && !menu_on) btn |= 0x800;
            /* selftest: synthetic hop at frame 30 (walking start speed) */
            if (selftest && frame >= 30 && frame <= 33 &&
                !getenv("SM64DS_SELFTEST_DASHJUMP") &&
                !getenv("SM64DS_SELFTEST_PUNCH") &&
                !getenv("SM64DS_SELFTEST_IDLE") &&
                !getenv("SM64DS_SELFTEST_JUMPSPAM") &&
                !decel_probe)
                btn |= 2;
            if (selftest && getenv("SM64DS_SELFTEST_DASH") && frame >= 20)
                btn |= 0x800;
            /* decel probe: dash held to build top run speed, released with
               the stick so the tail measured is a pure ground decay */
            if (decel_probe && frame < DECEL_RELEASE)
                btn |= 0x800;
            /* the 0x100-press repro: characterize the "LT crash" --
               camera rotate HUD vs a crouch entry, the fault dump
               names the path */
            if (selftest && getenv("SM64DS_SELFTEST_R100") && frame >= 40)
                btn |= 0x100;
            /* crouch probe: hold R(0x400) from frame 40 */
            if (selftest && getenv("SM64DS_SELFTEST_CROUCH") && frame >= 40)
                btn |= 0x400;
            /* full-speed sprint jump: dash from f20, jump at f60 */
            if (selftest && getenv("SM64DS_SELFTEST_DASHJUMP")) {
                if (frame >= 20) btn |= 0x800;
                if (frame >= 60 && frame <= 63) btn |= 2;
            }
            /* LONGJUMP probe: the leg-twist repro. The long jump is the run
               plus crouch plus A combination -- dash from f20 to build top
               run speed, R held from f60 so St_Walk_Main sees the crouch bit
               under speed, and the A edge two frames later, which is what
               St_Crouch_Main reads to enter ST_LONG_JUMP. The flight is
               roughly 35 frames after that, so 140 frames covers entry,
               flight and landing. */
            if (selftest && getenv("SM64DS_SELFTEST_LONGJUMP")) {
                if (frame >= 20) btn |= 0x800;
                if (frame >= 60) btn |= 0x400;
                if (frame >= 62 && frame <= 64) btn |= 2;
            }
            /* punch probe: A-button edge at f40 */
            if (selftest && getenv("SM64DS_SELFTEST_PUNCH") &&
                frame >= 40 && frame <= 42)
                btn |= 1;
            /* TONGUE probe: the Yoshi ground-tongue repro. B (punch, bit 1) is
               the tongue for Yoshi, but the BoB entrance keeps him in
               St_LevelEnter until roughly frame 195, so a punch has to wait for
               him to be walking. Press an edge every 40 frames from f210: three
               frames held so the edge is not missed, long enough between that
               St_YoshiPower runs to completion and drops back to walk before the
               next one. Drive it as Yoshi (SM64DS_CHARACTER=3) in BoB
               (SM64DS_LEVEL=6) with SM64DS_TRACE_STATE=2 to read the tongue
               state chain 0x020d7ed0 (Init) / 0x020d7504 (Main). */
            if (selftest && getenv("SM64DS_SELFTEST_TONGUE") &&
                frame >= 210 && ((frame - 210) % 40) < 3)
                btn |= 1;
            /* JUMPSPAM probe: the frame-hitch repro. A press edge every
               <period> frames from f20, three frames held so the edge is not
               missed. The default period of 40 is long enough that he lands
               and is standing again before the next one, so every cycle is a
               fresh ground jump rather than a triple-jump chain. Pair it with
               SM64DS_JUMP_PROBE=1 to get the per-frame cost line. */
            if (selftest && getenv("SM64DS_SELFTEST_JUMPSPAM")) {
                const int period = atoi(getenv("SM64DS_SELFTEST_JUMPSPAM"));
                const int p = period > 3 ? period : 40;
                if (frame >= 20 && (frame - 20) % p < 3)
                    btn |= 2;
            }
            /* SWIM probe: a B-button STROKE every 24 frames. Swimming is the
               one locomotion in the game the stick alone cannot drive --
               St_Swim_Main moves him on the stroke, not on the tilt -- so a
               selftest that only writes the stick floats in place forever.
               This is what makes "walk in, swim across, climb out" runnable
               without a person on the pad. */
            if (selftest && getenv("SM64DS_SELFTEST_SWIM") &&
                (frame % 24) < 3)
                btn |= 2;
            /* SM64DS_SELFTEST_SWAP=<n>[,<n>...] does mid-stride what F4 does by
               hand: one swap every 20 frames from frame 30, with a walk cycle
               already running and an animation partway through it. Two
               different things need that. The boot swap SM64DS_CHARACTER takes
               lands before the state machine has settled, which is not the path
               a person uses; and a LIST is what catches the file bookkeeping,
               because each swap releases the character it is leaving and loads
               the one it is going to, so only a chain proves the pair stays
               balanced across more than one hop. */
            if (selftest && frame >= 30 && (frame - 30) % 20 == 0) {
                const char *sw = getenv("SM64DS_SELFTEST_SWAP");
                if (sw) {
                    const int nth = (frame - 30) / 20;
                    for (int k = 0; k < nth && sw; ++k)
                        sw = strchr(sw, ',') ? strchr(sw, ',') + 1 : 0;
                    if (sw && *sw) {
                        const int want = atoi(sw);
                        fprintf(stderr, "[chr] f%d selftest swap -> %d\n",
                                frame, want);
                        port_player_set_character(c, want);
                        g_character = g_character_pending = want & 3;
                        /* prove the swap TOOK: param1 (Player+8) is the live
                           character index every downstream read keys off, so a
                           mismatch here is the whole point of the chain failing.
                           Prints PASS/FAIL so a headless run is a gate, not just
                           a sequence of fire-and-forget pokes. */
                        const int got = *(int *)((char *)c + 8) & 3;
                        fprintf(stderr, "[chr] f%d swap %s: param1=%d want=%d\n",
                                frame, got == (want & 3) ? "PASS" : "FAIL",
                                got, want & 3);
                    }
                }
            }
            /* SM64DS_SELFTEST_SWAPMOVE=<n>: one swap to character n, fired late
               (frame 210) so it lands well after any level's entrance cutscene
               and while forward is held -- the "switch DURING movement" the F4
               path is judged by. The door swap must keep him in his walk state
               across it; the state trace (SM64DS_TRACE_STATE=2) is where that
               reads. Pair with SM64DS_WINDOW_SELFTEST>=240. */
            if (selftest && frame == 210) {
                const char *mv = getenv("SM64DS_SELFTEST_SWAPMOVE");
                if (mv && *mv) {
                    const int want = atoi(mv) & 3;
                    fprintf(stderr, "[chr] f210 mid-run swap -> %d\n", want);
                    port_player_set_character(c, (unsigned)want);
                    g_character = g_character_pending = want;
                    const int got = *(int *)((char *)c + 8) & 3;
                    fprintf(stderr, "[chr] f210 swapmove %s: param1=%d want=%d\n",
                            got == want ? "PASS" : "FAIL", got, want);
                }
            }
            /* camera orbit through the game's own reader: func_02009e70
               tests data_0209f49c & 0x4300 -- L (0x200) rotates left, R
               (0x100) rotates right, 0x4000 is the snap-behind the input
               layer synthesizes. R doubles as crouch on the DS too, so E
               crouching as it orbits is the hardware's behaviour, not a
               harness artefact. */
            /* THE DS's OWN ROTATE INPUT, and nothing else: the pad bits go
               in, func_02009e70 does the rest. A tap of the bumper is one
               press edge and one 5.625-degree step; holding it (or holding
               the stick past the threshold) is a held shoulder button and
               steps every frame. While the freecam mod owns the view none of
               it is written -- the Camera actor is left following Mario so
               there is something clean to hand back to. */
            if (real_camera && cam_mode == CAM_DS) {
                /* The two bits func_02009e70 reads, picked by the same
                   cam_turn the rig steps its heading with, so DS mode and
                   analog mode turn the same way for the same push. 0x100
                   raises the heading (the ROM adds +0x400 for it) and 0x200
                   lowers it, and a rising heading is the view panning left,
                   so a rightward push takes 0x200 by default. Which host
                   control feeds which bit is the port's own choice: the DS
                   had L and R and none of these controls. */
                const unsigned cam_bit_right = (cam_turn > 0) ? 0x100u : 0x200u;
                const unsigned cam_bit_left  = (cam_turn > 0) ? 0x200u : 0x100u;
                if (key_live('Q')) btn |= cam_bit_left;
                if (key_live('E')) btn |= cam_bit_right;
                if (key_live('C')) btn |= 0x4000;
                if (stick_rx < -10000) btn |= cam_bit_left;
                if (stick_rx > 10000) btn |= cam_bit_right;
                if (pad_live) {
                    if (pad.buttons & 0x0100) btn |= cam_bit_left;   /* LB */
                    if (pad.buttons & 0x0200) btn |= cam_bit_right;  /* RB */
                }
                /* orbit probe: hold one of func_02009e70's own rotate bits
                   from frame 20 -- the camera's heading and the angle it
                   publishes must both move, and W must keep walking away
                   from the lens. Deliberately the raw bit and not
                   cam_bit_right: this probes the ROM's reader, so it must
                   not move when a player's binding preference does. */
                if (selftest && getenv("SM64DS_SELFTEST_ORBIT") && frame >= 20)
                    btn |= 0x100;
            }
            if (menu_on) btn = 0;   /* enter/A belong to the menu, not to him */
            /* TEMPORARY: fold the scripted probe's A/B into the button word so
               StartTalk's b==0 gate (data_0209f49e & 3) sees the press, and the
               camera-rotate readers do not (mask to bits 0-1). SM64DS_PROBE_INPUT. */
            if (!menu_on)
                btn |= (unsigned short)(port_input_probe_bits(frame) & 0x3);
            /* run mg16 lane MPBTN: the button half of the published key word,
               stashed HERE because this is the first line where btn is final
               (bindings, run-mode AUTO, the selftest probes, the menu's zero,
               the scripted probe's A/B -- all folded). Taken from the host
               state and never from a record, so the fan-out cannot feed the
               wire back into itself; comms_publish_pad below in this frame
               ORs it with the d-pad stash. The full raw probe word rides
               along so a scripted crouch (R) or run (Y) crosses the wire in a
               headless proof exactly like a held key. */
            port_raw_btn_stash((unsigned short)(
                host_btn_to_raw_keys(btn) |
                (menu_on ? 0 : port_input_probe_bits(frame))));
            /* ---- THE THIRD BUTTON WRITER, AND THE ONE HIS HANDS FOUND ------
             *
             * Run mg16 lane MP4, second field re-test. This is the LEVEL path's
             * copy of the publish the scene path does further up, and it was
             * left ungated when that one was gated. It runs AFTER the
             * per-player Ctrl fan below, so every frame it overwrote slot 0's
             * button words -- the ones the fan had just filled from the comms
             * records -- with THIS console's local buttons.
             *
             * Both of his surviving symptoms are this one line:
             *
             *   PUNCHING ON THE CHILD MADE MARIO PUNCH. Mario is slot 0, the
             *   matched state code reads its buttons as
             *   `data_0209f49c + data_020a0e40 * 0x18` with data_020a0e40 set
             *   to that actor's mPlayerNo, so on the child Mario correctly read
             *   slot 0 -- and slot 0 was holding the CHILD's own buttons
             *   because of this store.
             *
             *   AND THE HOST'S CROUCH NEVER CROSSED. The fan delivers the
             *   host's buttons into slot 0 on the child, and this overwrote
             *   them a few thousand instructions later. The stick crossed the
             *   whole time, which is why rungP3 was green: the stick fields are
             *   written by the fan and nothing clobbers them.
             *
             * THE ROM'S SIDE WAS CORRECT THROUGHOUT. Every matched reader
             * indexes by data_020a0e40 * 0x18 -- St_Jump_Main:34,
             * St_Shell_Main:95, St_Spin_Main:28, func_ov002_020ca940:35 -- and
             * hal_call_state_fn runs inside Player::Behavior's window where
             * data_020a0e40 is that actor's own slot. The wrong turn was
             * entirely on the port's write side, in three places, of which this
             * was the third.
             *
             * Gated exactly like the other two: with a transport up and the
             * ROM's fan-out driving, the local buttons reach this console's own
             * slot the ROM's way -- key register, local record, wire, fan-out,
             * Stage::CheckInput, the per-player fan -- and this store must not
             * put them anywhere else. */
            if (!(port::comms_transport() && comms_fanout_on())) {
                *(unsigned short *)(data_0209f49c + 0) = btn;
                *(unsigned short *)(data_0209f49e + 0) =
                    (unsigned short)(btn & (unsigned short)~btn_was);
            }
            btn_was = btn;
        }

        /* ...and the bottom screen's half of the same record: the camera
           buttons drawn down there, hit-tested by the game's own
           Stage::CheckCameraInput against the stylus the panel fills. It runs
           after the pad word is written because it ORs into it. */
        hal_sub_camera_input();

        /* the real ground tracking rewrites the path binding (c+0x670)
           from KCL surface attributes every contact frame; keep it at
           0xff (none) until a level boot seats the real path table
           (data_020a0d84 is null on host, the walk faults) */
        if (!real_boot)
            *(unsigned int *)(c + 0x670) = 0xff;

        if (selftest && frame == 1 && getenv("SM64DS_DUMP_CLSN")) {
            extern void *data_020a0c80[];
            fprintf(stderr, "[dump] slots:");
            for (int i = 0; i < 8; ++i)
                fprintf(stderr, " %p", data_020a0c80[i]);
            fprintf(stderr, "\n[dump] cylinder c+0x2d4 (the CylinderClsn "
                    "Behavior hands UpdatePos):\n");
            for (int off = 0; off < 0x40; off += 16) {
                fprintf(stderr, "  +%03x:", 0x2d4 + off);
                for (int k = 0; k < 4; ++k)
                    fprintf(stderr, " %08x",
                            *(unsigned *)(c + 0x2d4 + off + 4 * k));
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "[dump] speed c+0xa4: %d %d %d\n",
                    *(int *)(c + 0xa4), *(int *)(c + 0xa8), *(int *)(c + 0xac));
            fprintf(stderr, "\n[dump] wmc c+0x380:\n");
            for (int off = 0; off < 0xa0; off += 16) {
                fprintf(stderr, "  +%03x:", 0x380 + off);
                for (int k = 0; k < 4; ++k)
                    fprintf(stderr, " %08x",
                            *(unsigned *)(c + 0x380 + off + 4 * k));
                fprintf(stderr, "\n");
            }
        }

        static int prev_pos[3], prev_live;
        if (!prev_live) {
            prev_live = 1;
            prev_pos[0] = *(int *)(c + 0x5c);
            prev_pos[1] = *(int *)(c + 0x60);
            prev_pos[2] = *(int *)(c + 0x64);
        }

        /* until the first ChangeState seats the current-state pointer,
           tick the wait state directly (the smoke's exact flow); Behavior
           owns the frame once the state machine is live */
        if (selftest && frame == 0)
            fprintf(stderr, "[w] tick st=%p\n", *(void **)(c + 0x370));
        if (selftest && frame < 2 && getenv("SM64DS_TRACE_SURF"))
            fprintf(stderr, "[pre%03d] pos=(%.1f,%.1f,%.1f) speed=(%d,%d,%d) "
                    "horz=%d ang=%04x step=%u/%u timer=%u\n", frame,
                    *(int *)(c + 0x5c) / 4096.0f, *(int *)(c + 0x60) / 4096.0f,
                    *(int *)(c + 0x64) / 4096.0f, *(int *)(c + 0xa4),
                    *(int *)(c + 0xa8), *(int *)(c + 0xac), *(int *)(c + 0x98),
                    (unsigned short)*(short *)(c + 0x8e),
                    *(unsigned char *)(c + 0x6e3), *(unsigned char *)(c + 0x6e5),
                    *(unsigned short *)(c + 0x6a6));
        /* THE GAME'S OWN PER-FRAME TICK (gate 15). The ROM's processing list
           never calls Behavior bare: it calls func_02043288, ActorBase::Process
           over vtable slots 7/6/8, and slot 7 -- Actor::BeforeBehavior -- is
           what copies pos into PREV POS. Prev pos is the start of every line
           the continuous mesh-collision update casts, so with it stale at the
           constructor's zero the first frame swept a segment from the world
           origin to the gate and dropped Mario on the first floor it crossed.
           The legacy staging keeps the bare call: its hand-built spawn context
           has no area shown, and BeforeBehavior would cull the actor. */
        /* Under the real boot that tick is no longer the Player's alone: the
           level spawned other actors and they are on the same lists he is.
           port_actor_tick runs func_02044120's first three phases -- cleanup,
           the init pass for anything spawned since last frame, then behaviour
           in priority order -- which reaches him through the same
           func_02043288 the harness used to call by hand. */
        /* THE MENU'S PAUSE IS HERE, and this is the whole of it: skip the
           tick. Not a flag every actor has to respect and not a time scale --
           the frame simply does not advance the game, so nothing can drift
           while a person reads. Everything downstream still runs, so the
           picture stays live and the camera can still be moved around a
           frozen scene. */
        /* SM64DS_TREE_DROP=x,y,z[,frame] -- drop Mario onto a tree canopy.
           SM64DS_SPAWN cannot do this: it places him before the level's
           entrance sequence runs, and the entrance step handler never
           finishes from up a tree, so he just hangs in St_LevelEnter. The
           teleport has to land AFTER the entrance has handed him to St_Walk,
           which on castle grounds is about frame 12. Tree positions come out
           of SM64DS_TREE_PROBE=1 (all 21 are variant 4). */
        {
            static int td = -1, tx, ty, tz, tf;
            if (td < 0) {
                const char *e = getenv("SM64DS_TREE_DROP");
                td = 0;
                if (e) {
                    tf = 60;
                    if (sscanf(e, "%d,%d,%d,%d", &tx, &ty, &tz, &tf) >= 3)
                        td = 1;
                }
            }
            if (td && frame == tf) {
                *(int *)(c + 0x5c) = tx << 12;
                *(int *)(c + 0x60) = ty << 12;
                *(int *)(c + 0x64) = tz << 12;
                *(int *)(c + 0xa4) = 0;   /* straight down, no carried speed */
                *(int *)(c + 0xa8) = 0;
                *(int *)(c + 0xac) = 0;
                fprintf(stderr, "[tree] drop at frame %d -> (%d,%d,%d)\n",
                        tf, tx, ty, tz);
            }
        }
        /* SM64DS_FORCE_STATE=walljump -- the walljump crash probe.
           Tango walljumped in the live game and the process died with no
           fault-probe dump, because St_WallJump_Main dispatches the
           per-character airborne-gravity function out of the sinit-built
           table data_ov002_0211073c and (unlike St_Jump_Main) called row[0]
           RAW. row[0] is a DS code address, so on the host that is a jump
           into the mounted ov002 data image.
           Reproducing it needs no wall: put the Player in ST_WALL_JUMP and
           hold him airborne, and Behavior runs St_WallJump_Main straight
           into the dispatch. The table is dumped once so the run records
           which character row it went through. */
        {
            static int force_wj = -1;
            if (force_wj < 0) {
                const char *fs = getenv("SM64DS_FORCE_STATE");
                force_wj = (fs && !strcmp(fs, "walljump")) ? 1 : 0;
                if (force_wj) {
                    fprintf(stderr, "[wj] data_ov002_0211073c after sinit "
                            "(4 per-character rows):\n");
                    for (int r = 0; r < 4; ++r) {
                        int w0 = data_ov002_0211073c[r * 2];
                        int v  = data_ov002_0211073c[r * 2 + 1];
                        fprintf(stderr, "[wj]   idx %d: word0=0x%08x "
                                "word1=0x%08x -> %s, this+0x%x\n", r,
                                (unsigned)w0, (unsigned)v,
                                (v & 1) ? "VIRTUAL (vtable byte offset)"
                                        : "direct DS code address",
                                (unsigned)(v >> 1));
                    }
                    fprintf(stderr, "[wj] player param1 (character idx) = %d\n",
                            *(int *)(c + 0x008));
                }
            }
            /* SM64DS_FORCE_STATE=climb -- the TREE probe. Landing on a tree
               puts Mario in ST_CLIMB (data_ov002_021106dc), whose Init, Main
               and Cleanup are all unhosted, so the mapper no-ops all three:
               the anim never starts (freeze), the physics outside the state
               keeps integrating (slide), and a later consumer reads what
               Init never seated. Forcing the state is how that is measured
               without having to make him actually grab a trunk. */
            {
                static int force_cl = -1;
                if (force_cl < 0) {
                    const char *fs = getenv("SM64DS_FORCE_STATE");
                    force_cl = (fs && !strcmp(fs, "climb")) ? 1 : 0;
                }
                if (force_cl && frame == 10) {
                    /* Seat Player+0x37c with a REAL tree cylinder first --
                       that is what the grab (func_ov002_020caf98) does before
                       it changes state, and St_Climb_Init dereferences it
                       through vtable slot 2 (GetPos) to snap him to the
                       trunk. Tree::InitResources embeds the
                       CylinderClsnWithPos at node+0x0c and links the nodes
                       at +0x48; all 21 castle trees are on variant list 4. */
                    int *node = (int *)(size_t)data_ov002_02110a48[4];
                    if (node) {
                        *(void **)(c + 0x37c) = (char *)node + 0x0c;
                        fprintf(stderr, "[climb] seated +0x37c = tree "
                                "cylinder %p (node %p)\n",
                                (void *)((char *)node + 0x0c), (void *)node);
                    } else {
                        fprintf(stderr, "[climb] NO tree cylinders on list 4 "
                                "-- is the level booted?\n");
                    }
                    /* Grab him at a run. A real grab happens with speed on
                       the clock and St_Climb_Init is what zeroes it; leaving
                       it set is the whole of the slide, so put a known value
                       in rather than depending on what frame 10 happened to
                       be doing. */
                    *(int *)(c + 0x98) = 0x8000;   /* 8 units/frame */
                    fprintf(stderr, "[climb] frame 10: ChangeState -> "
                            "ST_CLIMB (%p)\n", (void *)data_ov002_021106dc);
                    _ZN6Player11ChangeStateERNS_5StateE(player,
                                                        data_ov002_021106dc);
                }
                /* The SLIDE is measurable: St_Climb_Init is what zeroes the
                   horizontal speed at +0x98/+0x9c/+0xa8 and the anim at
                   +0x6e3. With Init no-op'd he keeps the speed he grabbed at
                   and a state whose Main does nothing, which is exactly the
                   freeze-then-slide. */
                if (force_cl && frame >= 10 && frame <= 40 &&
                    (frame % 10) == 0)
                    fprintf(stderr, "[climb] frame %3d  horzSpeed=%d "
                            "vertSpeed=%d pos=(%d,%d,%d) step=%u anim=%u\n",
                            frame, *(int *)(c + 0x98), *(int *)(c + 0xa8),
                            *(int *)(c + 0x5c) >> 12, *(int *)(c + 0x60) >> 12,
                            *(int *)(c + 0x64) >> 12,
                            *(unsigned char *)(c + 0x6e3),
                            *(unsigned char *)(c + 0x6e5));
            }
            /* SM64DS_FORCE_STATE=ledgehang -- the HEDGE MAZE probe. Landing
               on a hedge edge puts Mario in ST_LEDGE_HANG
               (data_ov002_0210ffec) and every half of it used to be
               unhosted, so the mapper no-op'd Main once a frame forever:
               "unhosted state fn 0x020d0a44".
               The real trigger is func_ov002_020d0580 -- airborne, falling,
               and func_ov002_020d0178's raycast pair agrees there is a
               ledge -- and it seats mStateStep=1 before it changes state.
               Forcing it reproduces the hang without needing a hedge under
               the selftest's feet, the same way the climb probe does.
               What the state needs seated is what the grab would have left
               behind: the heading facing INTO the wall (+0x8e, which
               func_ov002_020d0178 writes as atan2(normal) + 0x8000), the
               grab point (+0x5c/+0x60/+0x64, snapped to the ledge top), and
               mStateStep=1 so Init picks the hang-idle anim 0x21 rather
               than the grab-impact 0x22. He also has to be OFF the ground:
               St_LedgeHang_Main lets go on the spot if mGroundY (+0x644) is
               within 0x28000 of him, which on flat castle grounds it is. */
            {
                static int force_lh = -1;
                if (force_lh < 0) {
                    const char *fs = getenv("SM64DS_FORCE_STATE");
                    force_lh = (fs && !strcmp(fs, "ledgehang")) ? 1 : 0;
                }
                if (force_lh && frame == 10) {
                    /* Lift him to a plausible hedge top so the ground is a
                       real distance below, and put speed on the clock. A
                       real grab happens mid-fall; St_LedgeHang_Init is what
                       zeroes +0x98 and +0xa8, so leaving them set is the
                       whole of the slide. */
                    *(int *)(c + 0x60) += 600 << 12;
                    *(int *)(c + 0x98) = 0x8000;    /* 8 units/frame */
                    *(int *)(c + 0xa8) = -0x4000;   /* falling */
                    *(unsigned char *)(c + 0x6de) = 1;   /* airborne */
                    *(unsigned char *)(c + 0x6e3) = 1;   /* what the trigger sets */
                    /* Poison the two fields Init alone seats, so "Init never
                       ran" is readable rather than inferred. */
                    *(unsigned short *)(c + 0x6a6) = 0xbeef;
                    *(unsigned char *)(c + 0x6e6) = 0xcd;
                    fprintf(stderr, "[lh] frame 10: ChangeState -> "
                            "ST_LEDGE_HANG (%p)  Init=0x020d0c54 "
                            "Main=0x020d0a44 Cleanup=0x020d092c\n",
                            (void *)data_ov002_0210ffec);
                    _ZN6Player11ChangeStateERNS_5StateE(player,
                                                        data_ov002_0210ffec);
                }
                /* St_LedgeHang_Main's FIRST exit is "mIsAirborne != 0 ->
                   func_ov002_020d0948 -> ST_FALL", and with no real hedge
                   under the probe the collision sets airborne again the
                   frame after Init clears it, so the hang ends immediately.
                   That is the state behaving correctly -- nothing to hang
                   on, so let go -- but it measures the let-go path instead
                   of the hang. Holding the flag keeps the hang itself on
                   screen for the length of the probe, the same device the
                   walljump probe uses in reverse just below (it PINS
                   airborne to keep St_WallJump_Main past ITS ground bail).
                   SM64DS_LH_LETGO=1 removes the hold and measures the drop
                   instead. */
                if (force_lh && frame >= 10 && !getenv("SM64DS_LH_LETGO"))
                    *(unsigned char *)(c + 0x6de) = 0;
                /* 45 frames of hang. Every number here is one Init seats or
                   Main advances: anim id lives at +0x63c as (id << 2), the
                   speeds Init zeroes at +0x98/+0xa8, mIsAirborne at +0x6de,
                   mStateWaitTimer at +0x6a6 (Init writes 2) and unk_6e6 at
                   +0x6e6 (Init writes 0). Unhosted, the poison survives and
                   the anim never changes. */
                if (force_lh && frame >= 10 && frame <= 55 &&
                    (frame % 5) == 0)
                    fprintf(stderr, "[lh] frame %3d  anim=0x%02x horz=%d "
                            "vert=%d air=%u step=%u wait=0x%04x unk6e6=0x%02x "
                            "pos=(%d,%d,%d) groundY=%d\n",
                            frame, *(unsigned *)(c + 0x63c) >> 2,
                            *(int *)(c + 0x98), *(int *)(c + 0xa8),
                            *(unsigned char *)(c + 0x6de),
                            *(unsigned char *)(c + 0x6e3),
                            *(unsigned short *)(c + 0x6a6),
                            *(unsigned char *)(c + 0x6e6),
                            *(int *)(c + 0x5c) >> 12, *(int *)(c + 0x60) >> 12,
                            *(int *)(c + 0x64) >> 12,
                            *(int *)(c + 0x644) >> 12);
                if (force_lh && frame == 56) {
                    fprintf(stderr, "[lh] 45 frames of hang survived, no "
                            "fault\n");
                    exit(0);
                }
            }
            if (force_wj && frame == 10) {
                /* SM64DS_FORCE_CHAR=<0-3> picks the row: 0/2 (Mario, Wario)
                   are hosted, 1/3 (Luigi, Yoshi) are not and have to come
                   out as the mapper's loud no-op, not a wild jump.
                   NOT a character change, and not the one to copy: it pokes
                   param1 raw on purpose, leaving the anim archive, the globals
                   and the head where they were, because the probe wants the
                   gravity row and nothing else. The real swap is
                   port_player_set_character (F4, the menu row,
                   SM64DS_CHARACTER, SM64DS_SWITCH). */
                const char *fc = getenv("SM64DS_FORCE_CHAR");
                if (fc) {
                    *(int *)(c + 0x008) = atoi(fc);
                    fprintf(stderr, "[wj] forced character idx = %d\n",
                            atoi(fc));
                }
                fprintf(stderr, "[wj] frame 10: ChangeState -> ST_WALL_JUMP "
                        "(%p)\n", (void *)data_ov002_021103dc);
                _ZN6Player11ChangeStateERNS_5StateE(player,
                                                    data_ov002_021103dc);
            }
            /* St_WallJump_Main bails to St_Fall the moment he is grounded,
               and the dispatch sits past that check -- so hold him airborne
               for the length of the probe. */
            if (force_wj && frame >= 10) *(unsigned char *)(c + 0x6de) = 1;
        }
        /* SM64DS_LOOP_PROBE=<frame>[,<soundId>]: the gate-31 loose end, the
           level-change looping-sound reap. At <frame> it starts a looping sound
           through the game's own Sound::PlayLong (the walk/slide states that
           would start one depend on collision geometry the selftest does not
           reach, so this drives the same function at the player directly). The
           handle lands in data_0209b53c and the per-frame reaper keeps it alive
           while the probe keeps refreshing it; when SM64DS_LEVEL_CYCLE fires,
           func_02011974 -- the Scene::BeforeCleanupResources reap -- runs in the
           teardown and empties the table. The probe reports the live count each
           frame so the drop from 1 to 0 across the change is visible. */
        if (selftest) {
            static int lp_frame = -2, lp_sound = 3, lp_handle;
            if (lp_frame < -1) {
                const char *e = getenv("SM64DS_LOOP_PROBE");
                lp_frame = -1;
                if (e) {
                    const char *comma = strchr(e, 44);
                    lp_frame = atoi(e);
                    if (comma) lp_sound = atoi(comma + 1);
                }
            }
            if (lp_frame >= 0 && frame >= lp_frame) {
                /* refresh every frame from the start frame on, the way an actor
                   holding a loop does: pass the handle back so PlayLong finds it
                   and marks it refreshed rather than starting a second sound.
                   Exactly one handle stays live. */
                lp_handle = (int)port_course_loop_start((unsigned)lp_handle,
                                                        (unsigned)lp_sound);
                if (frame % 30 == 0 || frame == lp_frame)
                    fprintf(stderr, "[loop-probe] f%d handle %d, live %d\n",
                            frame, lp_handle, port_course_loop_live());
            }
        }
        /* SM64DS_LEVEL_CYCLE=<n>[,<period>]: run the whole handoff n times
           without a person at the keyboard. Each cycle calls the GAME'S OWN
           ExitLevel(), which is SetNextLevel(1) plus the character wipe and
           the one the star-get and death paths both end in; on the castle
           grounds that is a RE-ENTRY -- level 1 entrance 0xd, the castle
           door -- and a re-entry exercises every line a change to a different
           level would, because nothing in the teardown or the boot is
           conditional on the id being different. Two cycles is the test the
           gate is about: run it twice and the heap and the census have to
           come back to the same numbers.
           SM64DS_LEVEL_SELECT=<row> instead drives the debug level select's
           own row list, which is how a level the mount stream has landed gets
           entered from the game's own table. */
        if (selftest) {
            static int cyc_n = -1, cyc_period, cyc_done, sel_row = -1;
            if (cyc_n < 0) {
                const char *e = getenv("SM64DS_LEVEL_CYCLE");
                cyc_n = 0;
                cyc_period = 60;
                if (e) {
                    const char *comma = strchr(e, 44);
                    cyc_n = atoi(e);
                    if (comma) cyc_period = atoi(comma + 1);
                    if (cyc_period < 2) cyc_period = 2;
                }
                e = getenv("SM64DS_LEVEL_SELECT");
                sel_row = e ? atoi(e) : -1;
            }
            if (cyc_done < cyc_n && frame == cyc_period * (cyc_done + 1)) {
                ++cyc_done;
                printf("[cycle] %d of %d at frame %d: heap free %u, "
                       "actors %d" "\n", cyc_done, cyc_n, frame,
                       port_level_heap_free_bytes(), port_actor_live_count());
                if (sel_row >= 0)
                    port_title_select(sel_row);
                else
                    ExitLevel();
            }
            /* SM64DS_WARP_SEQ=<lvl>@<frame>[,<lvl>@<frame>...]: a scripted warp
               chain for the level-teardown gates. At each listed frame it drives
               the game's own LoadLevelNoReturn(level, 0, 1, 1) -- the same call
               the debug select ends in -- so the change poll tears the current
               level down and boots the next. Entrance 0 is every level's default
               spawn. Nothing here writes the handoff words by hand. */
            {
                static const char *seq = 0;
                static int seq_read;
                if (!seq_read) { seq = getenv("SM64DS_WARP_SEQ"); seq_read = 1; }
                if (seq) {
                    const char *p = seq;
                    while (*p) {
                        int lv = atoi(p);
                        const char *at = strchr(p, 64 /* '@' */);
                        int fr = at ? atoi(at + 1) : -1;
                        if (fr == frame) {
                            printf("[warpseq] frame %d: warp to level %d "
                                   "(heap free %u, actors %d)\n", frame, lv,
                                   port_level_heap_free_bytes(),
                                   port_actor_live_count());
                            LoadLevelNoReturn(lv, 0, 1, 1);
                        }
                        const char *comma = strchr(p, 44 /* ',' */);
                        if (!comma) break;
                        p = comma + 1;
                    }
                }
            }
            /* SM64DS_MENU_WARP_TEST=<row>[,<frame>]: the interactive shape
               of the same select, headless. SM64DS_MENU=1 boots with the
               menu open (game tick paused); at the frame this fires the
               MENU_LEVEL enter branch verbatim -- select then close -- which
               is the sequence the 2026-08-05 session ran when the warp came
               out as a castle grounds re-entry while SM64DS_LEVEL_SELECT
               kept passing. */
            static int mw_row = -2, mw_frame;
            if (mw_row < -1) {
                const char *e = getenv("SM64DS_MENU_WARP_TEST");
                mw_row = -1; mw_frame = 60;
                if (e) {
                    const char *comma = strchr(e, 44);
                    mw_row = atoi(e);
                    if (comma) mw_frame = atoi(comma + 1);
                }
            }
            if (mw_row >= 0 && frame == mw_frame) {
                fprintf(stderr, "[mwtest] f%d menu-shaped select of row %d "
                        "(menu_on=%d)\n", frame, mw_row, menu_on);
                if (port_title_select(mw_row)) {
                    menu_on = 0;
                    fprintf(stderr, "[menu] closed for the level handoff\n");
                }
            }

            /* SM64DS_SELFTEST_OOB=<frame>: the fell-out-of-the-world death,
               headless. At <frame> put the player into ST_DEAD_PIT
               (data_ov002_02110124) with mStateStep==1, which is the exact state
               and step the game reaches when Mario drops below the level's kill
               plane: St_DeadPit_Init's case 1 calls HitDeathPlane, which through
               SetNextLevel writes the re-entry request the level-change poll
               honours the same frame. So this rides the real path -- death,
               teardown, reload -- and the frames after <frame> are the re-entry
               the reported crash happens on. Runs the ROM's own code; nothing
               here writes the handoff words by hand. */
            static int oob_frame = -1, oob_fired;
            if (oob_frame < 0) {
                const char *e = getenv("SM64DS_SELFTEST_OOB");
                oob_frame = e ? atoi(e) : 0;
            }
            if (oob_frame > 0 && frame == oob_frame && !oob_fired && player) {
                oob_fired = 1;
                *(unsigned char *)(c + 0x6e3) = 1;   /* mStateStep = 1 */
                fprintf(stderr, "[oob] f%d ChangeState -> ST_DEAD_PIT (%p), "
                        "step 1: the ROM's HitDeathPlane path\n", frame,
                        (void *)data_ov002_02110124);
                _ZN6Player11ChangeStateERNS_5StateE(player,
                                                    data_ov002_02110124);
                fprintf(stderr, "[oob] after HitDeathPlane: next sublevel %d "
                        "entrance %d reason %d\n", (int)port_course_next_sublevel(),
                        (int)data_0209f268, (int)data_0209f26c);
            }

            /* ---- SM64DS_EXIT_PROBE / SM64DS_EXIT_ENTER (see the top of the
               file). The dump waits for the level to be up; the entry is the
               two frames a walk into the painting produces. Once it has
               fired, one line a frame says whether the Player still has
               control and what the exit's pull counter is doing, which is
               the whole of the soft lock in two numbers. */
            static int ex_dumped, ex_idx = -2, ex_frame, ex_fired;
            if (ex_idx == -2) {
                const char *e = getenv("SM64DS_EXIT_ENTER");
                ex_idx = -1;
                if (e) {
                    ex_frame = 60;
                    sscanf(e, "%d,%d", &ex_idx, &ex_frame);
                }
            }
            if (!ex_dumped && frame == 30 &&
                (getenv("SM64DS_EXIT_PROBE") || ex_idx >= 0)) {
                ex_dumped = 1;
                port_exit_dump();
                /* Camera state pointer is +0x138 and the flags word is +0x154.
                   Bit 0x10 of that word is the lock Camera::LookAtExit sets
                   after its own ChangeState, and Camera::ChangeState refuses
                   every later state change while it is set. */
                fprintf(stderr, "[exit] camera states: boot %p, follow "
                        "(func_0200d5c0) %p, look-at-exit %p; camera is in "
                        "%p flags %08x\n",
                        (void *)data_0209b008, (void *)data_0209b078,
                        (void *)data_0209b0f8,
                        data_0209f318 ? *(void **)((char *)data_0209f318
                                                   + 0x138) : 0,
                        data_0209f318 ? *(unsigned *)((char *)data_0209f318
                                                      + 0x154) : 0);
            }
            /* Three writes. The PRIME, two frames early, puts him on the near
               side but OUTSIDE the box in x: the exit's Behavior records his
               z (its +0x88) without the box test passing, so the teleport in
               from wherever he spawned cannot itself read as a crossing --
               without it the first reading is a spurious entry and every
               number after it has a warp in it that a walk does not.
               Then the near side inside the box, EX_SETTLE frames for the
               game's own collision to land him on the floor there, then one
               stride past the plane, which is the trigger.
               A floor hole needs no push: he falls through its plane on his
               own during the settle, which is the real way in, so the last
               write is skipped once he has already been taken over. */
            enum { EX_SETTLE = 12 };
            if (ex_idx >= 0 && player &&
                (frame == ex_frame - 2 || frame == ex_frame ||
                 frame == ex_frame + EX_SETTLE)) {
                char *ex = port_exit_nth(ex_idx);
                const int taken = *(unsigned char *)(c + 0x6f6) != 0;
                if (ex && !(frame == ex_frame + EX_SETTLE && taken)) {
                    /* +-0x30000 is 48 units, about one walking stride, so the
                       last two writes straddle the plane the way a stride
                       does. */
                    port_exit_place(ex, c, frame == ex_frame + EX_SETTLE
                                               ? -0x30000 : 0x30000,
                                    frame == ex_frame - 2);
                    ex_fired = 1;
                    fprintf(stderr, "[exit] f%d placed %s -> world "
                            "(%d,%d,%d)\n", frame,
                            frame == ex_frame - 2 ? "beside the box (prime)"
                            : frame == ex_frame ? "in front of the plane"
                                                : "one stride past the plane",
                            *(int *)(c + 0x5c) >> 12,
                            *(int *)(c + 0x60) >> 12,
                            *(int *)(c + 0x64) >> 12);
                }
            }
            if (ex_fired && player) {
                char *ex = port_exit_nth(ex_idx);
                int local[3] = {0, 0, 0};
                if (ex)
                    MulVec3Mat4x3(c + 0x5c, ex + 0xd4, local);
                int evy = 0, tw = 0;
                const int blend = port_fader_blend_state(&evy, &tw);
                fprintf(stderr, "[exit-watch] f%d pos(%d,%d,%d) localz %d "
                        "ctrl_disabled %u nocontrol %u kind %u state %p "
                        "step %u | exit pull %d lastz %d | pending %d | "
                        "screen %s\n",
                        frame, *(int *)(c + 0x5c) >> 12,
                        *(int *)(c + 0x60) >> 12, *(int *)(c + 0x64) >> 12,
                        local[2] >> 12,
                        *(unsigned char *)(c + 0x6f6),
                        *(unsigned char *)(c + 0x709),
                        *(unsigned char *)(c + 0x70a),
                        *(void **)(c + 0x370), *(unsigned char *)(c + 0x6e3),
                        ex ? *(int *)(ex + 0x98) >> 12 : 0,
                        ex ? *(int *)(ex + 0x88) >> 12 : 0,
                        (int)data_02092110,
                        !blend ? "clear" : evy >= 16
                            ? (tw ? "COVERED white" : "COVERED black")
                            : (tw ? "fading white" : "fading black"));
                if (data_0209f318)
                    fprintf(stderr, "[exit-cam] f%d state %p flags %08x%s\n",
                            frame, *(void **)((char *)data_0209f318 + 0x138),
                            *(unsigned *)((char *)data_0209f318 + 0x154),
                            (*(unsigned *)((char *)data_0209f318 + 0x154)
                             & 0x10) ? "  LOCKED (LookAtExit)" : "");
            }
        }

        /* ---- THE LEVEL HANDOFF (gate 31) -------------------------------
           Where Scene::SpawnIfNecessary sits in func_020197b8: phase 3, after
           input and before func_02044120's own phases. Anything that ran this
           frame -- the debug menu's level row, its exit row, a warp pipe, a
           death -- has already written data_02092110 through the game's own
           LoadLevel, and this is the frame it is honoured on.

           EVERY POINTER main() HOLDS INTO THE LEVEL IS STALE AFTERWARDS. The
           Player was destroyed with everything else and the entrance spawned
           a new one; the Stage survives (hal/level_change.cpp says why) but
           its model is a fresh load. Re-seating them here rather than at the
           top of the frame is deliberate: the change runs after the frame's
           input has been taken and before anything reads the world, so no
           half of one frame ever sees two different levels. */
        /* THE SCENE FADE GATE. When a title-select armed a scene fade
           (Scene::StartSceneFade), hold the boot until the fade has COVERED the
           screen, so the old level is gone behind white before the new one
           loads and the pop is hidden. Without a scene fade -- the ExitLevel,
           death and warp-pipe paths -- the change applies the frame it is
           pending, as before. */
        int scene_fade = 0, scene_id = -1;
        if (port_scene_fade_pending(&scene_id)) {
            int evy = 0, tw = 0;
            int covered = port_fader_blend_state(&evy, &tw) && evy >= 16;
            scene_fade = covered ? 2 : 1;   /* 1 = still fading, 2 = covered */
        }
        if (!menu_on && boot_spawns && port_level_change_pending() &&
            scene_fade != 1) {
            if (port_level_change_poll()) {
                /* the level is up behind the cover: fade back IN and drop the
                   scene request. Reveal with the SAME colour the cover used
                   (white, 0x7fff) but the reverse direction, so the panel
                   un-whitens to the new level rather than flashing to black. */
                if (scene_fade == 2) {
                    port_fader_start_color(16, 0, 0x7fff);
                    port_scene_fade_clear();
                    fprintf(stderr, "[fade] scene %d: covered, level booted, "
                            "fading in\n", scene_id);
                }
                /* run mg16 lane MP3: THE LOCAL SLOT, not slot 0. The
                   render loop below assumes `player` IS
                   data_0209f394[data_0209f250]; it skips that index and draws
                   `player` separately. Re-seating this to slot 0 after a level
                   change breaks that on any console whose slot is not 0: slot
                   0 gets submitted twice and the local player is never drawn
                   at all. */
                {
                    int me2 = (int)data_0209f250;
                    if (me2 < 0 || me2 >= 4 || !data_0209f394[me2]) me2 = 0;
                    player = data_0209f394[me2];
                }
                if (!player) {
                    fprintf(stderr, "[lvl] the new level spawned no player\n");
                    return 3;
                }
                c = (char *)player;
                /* the character state was read off the boot's Player; across
                   a warp the entrance spawned a fresh one (ExitLevel wipes
                   the save byte too), so read it back with everything else */
                g_character = *(unsigned char *)(c + 0x6d9) & 3;
                g_character_pending = g_character;
                cam = data_0209f318;
                level_shift = 0;
                if (real_boot) {
                    /* the level-change boot loads the model too, same move as
                       the first boot above: port_stage_boot_body calls
                       Stage::LoadModel ahead of the object pass. A call here
                       would be the second rebase of the same buffer, because
                       this file's LoadFile handle table hands the same
                       filePtr back inside one level. */
                    void *bmd = *(void **)(level_model + 0x04);
                    level_shift = bmd ? *(int *)bmd : 0;
                    _ZN5Stage10LoadSkyboxEv(stage);
                }
                an_pivot_live = 0;        /* do not ease the camera across it */
                if (cam_mode != CAM_DS && cam) fc_seed(cam);
                printf("[lvl] re-seated: player %p camera %p scaleShift %d, "
                       "heap free %u\n", player, cam, level_shift,
                       port_level_heap_free_bytes());
                /* nothing else in this frame: the new level takes its first
                   tick from the next one, the way a spawned scene does */
                game_ticked = 0;
            }
        }

        /* GATE 35 PROBES. SM64DS_COURSE_PROBE=<what>[,<frame>] fires one of
           the course-loop entry points at a frame and lets the game's own
           code carry it from there. Everything here is a CALL into
           hal/star_flow.cpp or matched src; nothing writes player state
           except the coin probe's stand-in Actor, which only needs a position
           for the chime to be placed at.

             coin   Actor::GivePlayerCoins -- counter, health, chime, popup
             hurt   Player::Hurt kind 1 (one wedge)
             hurt2  Player::Hurt kind 2 (the knock-down)
             drown  Heal(-0x100) a frame at a time until he is empty
             death  the full chain: hurt to zero, then KillPlayer
             star   the collect and the course-clear handoff

           They run AFTER the entrance has finished with him (the default
           frame is 40) because Player::Hurt's ChangeState is refused while
           St_LevelEnter owns the state machine. */
        /* SM64DS_SND_PROBE=N: count sounding voices and running sequence
           players every N frames. This is the one part of the gate that
           cannot be checked by hash, so it is checked by census. */
        {
            static int sp = -1;
            if (sp < 0) {
                const char *e = getenv("SM64DS_SND_PROBE");
                sp = e ? atoi(e) : 0;
            }
            if (sp > 0 && frame % sp == 0) {
                char when[32];
                snprintf(when, sizeof when, "frame %d", frame);
                port_course_sound_probe(when);
            }
        }

        {
            static int cp_read, cp_frame = 40, cp_done;
            static char cp_what[16];
            if (!cp_read) {
                cp_read = 1;
                const char *e = getenv("SM64DS_COURSE_PROBE");
                if (e) {
                    const char *comma = strchr(e, ',');
                    size_t n = comma ? (size_t)(comma - e) : strlen(e);
                    if (n > sizeof cp_what - 1) n = sizeof cp_what - 1;
                    memcpy(cp_what, e, n);
                    cp_what[n] = 0;
                    if (comma) cp_frame = atoi(comma + 1);
                }
            }
            if (cp_what[0] && frame >= cp_frame && !cp_done) {
                const int done_after = 90;
                if (frame == cp_frame) {
                    /* snapshot the three next-level words BEFORE anything can
                       move them, so "the handoff fired" is a change and not a
                       non-zero (data_02092110 boots with the ROM's own 0xff) */
                    port_course_arm_watch();
                    fprintf(stderr, "[course] probe '%s' at frame %d: "
                            "hp=%d coins=%d next-sublevel=%d\n", cp_what,
                            frame, port_course_health(), port_course_coins(),
                            port_course_next_sublevel());
                }
                if (!strcmp(cp_what, "coin")) {
                    if (frame == cp_frame) {
                        /* the stand-in for the coin ACTOR: GivePlayerCoins
                           reads exactly one thing off it, Actor+0x74, and
                           that is mCamSpacePos -- CAMERA-SPACE, not world.
                           Sound::Play's 3D path measures it straight against
                           the distance limit and derives the pan from its x,
                           so a world position there reads as thousands of
                           units away and the chime is culled without a word.
                           Actor::BeforeBehavior fills it for every actor on
                           the processing lists, which is why a real coin
                           needs nothing extra; the stand-in is not on them,
                           so it borrows the player's. */
                        static char coin_actor[0x80];
                        memcpy(coin_actor + 0x74, c + 0x74, 12);
                        port_give_player_coins(coin_actor, player, 1, 0);
                        fprintf(stderr, "[coin] one yellow -> coins=%d hp=%d\n",
                                port_course_coins(), port_course_health());
                        port_give_player_coins(coin_actor, player, 1, 2);
                        fprintf(stderr, "[coin] one blue   -> coins=%d hp=%d\n",
                                port_course_coins(), port_course_health());
                        cp_done = 1;
                    }
                } else if (!strcmp(cp_what, "hurt") ||
                           !strcmp(cp_what, "hurt2")) {
                    if (frame == cp_frame) {
                        port_course_hurt(player, cp_what[4] == '2' ? 2 : 1);
                        cp_done = 1;
                    }
                } else if (!strcmp(cp_what, "drown")) {
                    /* one HP a frame, which is faster than the water damage
                       tick but the same call the water state makes */
                    if (port_course_health() > 0)
                        fprintf(stderr, "[drown] f%d hp=%d\n", frame,
                                port_course_drown_tick(player, 0x100));
                    else {
                        fprintf(stderr, "[drown] empty at frame %d, "
                                "state=%p\n", frame, *(void **)(c + 0x370));
                        cp_done = 1;
                    }
                } else if (!strcmp(cp_what, "death")) {
                    /* REAL damage, repeated, not a drain: the state
                       transition that matters is the one Player::Hurt makes
                       itself. func_ov002_020d91e0 answers "is he dead now"
                       and only on 1 does Hurt change to data_ov002_0211010c,
                       the DEAD state -- so hurting him to zero is the only
                       way to see that branch taken. Every 8 frames, because
                       Hurt is refused while the hurt state's invulnerability
                       is still up. */
                    static int empty_at = -1;
                    if (port_course_health() > 0) {
                        if (port_course_can_hurt(player))
                            port_course_hurt(player, 2);
                    } else {
                        if (empty_at < 0) {
                            empty_at = frame;
                            fprintf(stderr, "[death] hp reached 0 at frame "
                                    "%d, state=%p -- waiting for the ROM's "
                                    "own St_DeadHit_Main -> KillPlayer\n",
                                    frame, *(void **)(c + 0x370));
                        }
                        if (frame % 20 == 0)
                            fprintf(stderr, "[death] f%d dead-state=%d "
                                    "hud-hp=%d work=%u\n", frame,
                                    port_course_in_dead_state(player),
                                    port_course_hud_health(),
                                    *(unsigned char *)(c + 0x6e5));
                        if (port_course_handoff_fired()) {
                            fprintf(stderr, "[death] the ROM fired the "
                                    "handoff itself at frame %d: next "
                                    "sublevel %d\n", frame,
                                    port_course_next_sublevel());
                            port_course_respawn(player);
                            fprintf(stderr, "[respawn] hp=%d\n",
                                    port_course_health());
                            cp_done = 1;
                        } else if (frame > empty_at + done_after * 3) {
                            fprintf(stderr, "[death] the ROM had not fired it "
                                    "after %d frames -- calling KillPlayer "
                                    "directly so the handoff itself is still "
                                    "shown\n", done_after * 3);
                            port_course_kill();
                            port_course_respawn(player);
                            fprintf(stderr, "[respawn] hp=%d state=%p\n",
                                    port_course_health(),
                                    *(void **)(c + 0x370));
                            cp_done = 1;
                        }
                    }
                } else if (!strcmp(cp_what, "deathloop")) {
                    /* THE DEATH-RESPAWN REPRODUCTION, and the reason it is not
                       the `death` verb above: that one calls
                       port_course_respawn() the moment the handoff fires --
                       which is at the level-change REQUEST, before the
                       re-entry boot -- and port_course_respawn force-writes
                       0x880 into the health word (hal/star_flow.cpp). So it
                       hands the next level a healthy player whether or not the
                       GAME can restore one, and comes out clean either way.
                       Reproducing the infinite death loop with it gives a
                       false negative.

                       This verb hurts to zero with Player::Hurt exactly as
                       `death` does and then fakes NOTHING: the ROM's own
                       St_DeadHit_Main -> KillPlayer -> SetNextLevel(2) carries
                       it, the harness's normal level-change poll boots the
                       destination, and the game is left to restore the player
                       from its own level-enter step (src/func_ov002_020c75f0.c
                       :29, gated on the latched entry reason). It never sets
                       cp_done, so it keeps reporting across the re-entry and a
                       loop shows up as repeated changes rather than silence. */
                    static int dl_empty_at = -1;
                    if (port_course_health() > 0 && dl_empty_at < 0) {
                        if (port_course_can_hurt(player))
                            port_course_hurt(player, 2);
                    } else {
                        if (dl_empty_at < 0) {
                            dl_empty_at = frame;
                            fprintf(stderr, "[deathloop] hp reached 0 at frame "
                                    "%d -- handing off to the ROM, nothing "
                                    "faked from here\n", frame);
                        }
                        if (frame % 20 == 0)
                            fprintf(stderr, "[deathloop] f%d level=%d hp=%d "
                                    "lives=%d entry-reason=%d dead-state=%d\n",
                                    frame, (int)data_0209f2f8,
                                    port_course_health(),
                                    (int)data_0209f2f4[0],
                                    (int)data_0209f2fc[0],
                                    port_course_in_dead_state(player));
                    }
                } else if (!strcmp(cp_what, "star")) {
                    if (frame == cp_frame) {
                        port_star_collect(0);
                        cp_done = 1;
                    }
                } else if (!strcmp(cp_what, "starbox")) {
                    /* THE MILESTONE-STAR BOX, driven organically.
                       port_star_collect ends in ExitLevel -> SetNextLevel(1),
                       so the hub is re-entered with reason 1. Once the boot
                       latches that reason into data_0209f2fc, the level-enter
                       step func_ov002_020c71e0:21 can take its
                       `data_0209f2fc[0] == 1` branch for the first time and
                       call func_ov002_020c6e14, the 1st/3rd/8th/12th/30th/50th
                       /80th-star message. Every callee is linked, but nothing
                       had ever reached it, because the gate always read 0.

                       Observed through data_0209d660, the message-active flag,
                       rather than by instrumenting game code. Never sets
                       cp_done: the point is what happens AFTER the exit. */
                    static int sb_fired = -1;
                    if (frame == cp_frame)
                        port_star_collect(0);
                    if (frame > cp_frame) {
                        if (sb_fired < 0 && data_0209d660 != 0) {
                            sb_fired = frame;
                            fprintf(stderr, "[starbox] MESSAGE BOX OPEN at "
                                    "frame %d: stars=%d entry-reason=%d "
                                    "level=%d\n", frame, (int)NumStars(),
                                    (int)data_0209f2fc[0], (int)data_0209f2f8);
                        }
                        if (frame % 60 == 0)
                            fprintf(stderr, "[starbox] f%d level=%d stars=%d "
                                    "entry-reason=%d msg-active=%d "
                                    "(box-opened-at=%d)\n", frame,
                                    (int)data_0209f2f8, (int)NumStars(),
                                    (int)data_0209f2fc[0],
                                    (int)data_0209d660, sb_fired);
                    }
                } else if (!strcmp(cp_what, "kuppa")) {
                    /* STAR1 repro: launch the REAL star-get camera kuppa script
                       (func_0200ee8c -> RunKuppaScript, sets data_0209fc48) and
                       watch the input gate. On a build that never advances the
                       script (ProcessKuppaScript not driven per frame) fc48 and
                       the b274 cursor stay frozen, Stage::CheckInput zeroes the
                       Ctrl block every frame, and the stick copy bridge carries
                       the zero (mag/pos frozen) while direct-written buttons stay
                       live. The [fNNN] line above shows pos/spd/st/mag; this line
                       shows why. Never cp_done: log to the frame limit. */
                    if (frame == cp_frame) {
                        data_0209f224 = 0;      /* star 0's camera setting */
                        func_0200ee8c(-1);      /* launch the star camera script */
                        fprintf(stderr, "[kuppa] launched at frame %d: "
                                "fc48=%d f2d8=%d b274=%d\n", frame,
                                data_0209fc48, (int)data_0209f2d8,
                                (int)data_0209b274);
                    }
                    /* End of the cutscene, the restore half of the proof. The
                       real star-get reaches EndKuppaScript through the completing
                       script/no-control sequence -- which the ProcessKuppaScript
                       tick added below is what lets the script advance toward at
                       all (frozen on base, live on the fix). Calling it here at a
                       fixed frame stands in for that completion so one run shows
                       the whole arc: walk -> star cutscene freezes the stick ->
                       cutscene ends -> stick and walk resume. */
                    if (frame == cp_frame + 80) {
                        EndKuppaScript();
                        fprintf(stderr, "[kuppa] EndKuppaScript() at frame %d: "
                                "fc48=%d f2d8=%d\n", frame, data_0209fc48,
                                (int)data_0209f2d8);
                    }
                    fprintf(stderr, "[kuppa] f%d fc48=%d b274=%d f2d8=%d "
                            "caa0bit=%d mag=%d\n", frame, data_0209fc48,
                            (int)data_0209b274, (int)data_0209f2d8,
                            (data_0209caa0[2] & 0x80) ? 1 : 0,
                            (int)*(short *)(data_0209f4a0 + 0));
                } else if (!strcmp(cp_what, "stardance")) {
                    /* STAR1 repro: drive the REAL normal-star no-control dance
                       (kind 1, the value func_ov002_020e73ac returns for star 0;
                       offset 0x186, as the PowerStar collect handler
                       func_ov002_020e8ef0 passes). Watch the full lifecycle:
                       mIsControlDisabled (Player+0x6f6), mNoCtrlKind (+0x70a),
                       the state pointer (+0x370), the cutscene flag
                       data_0209fc48, its cursor, and the stick magnitude that
                       reaches the walk core. On base the camera script latches
                       fc48; the fix advances it. [fNNN] shows pos/spd. */
                    extern int _ZN6Player17SetNoControlStateEhih(
                        void *self, unsigned char a, int b, unsigned char cc);
                    if (frame == cp_frame) {
                        int r = _ZN6Player17SetNoControlStateEhih(player, 1,
                                                                 0x186, 0);
                        fprintf(stderr, "[dance] SetNoControlState(1,0x186,0) "
                                "-> %d at frame %d\n", r, frame);
                    }
                    fprintf(stderr, "[dance] f%d icd=%d nck=%d st=%08x fc48=%d "
                            "b274=%d mag=%d\n", frame,
                            (int)*(unsigned char *)(c + 0x6f6),
                            (int)*(unsigned char *)(c + 0x70a),
                            *(void **)(c + 0x370) ? *(unsigned *)*(void **)(c + 0x370) : 0u,
                            data_0209fc48, (int)data_0209b274,
                            (int)*(short *)(data_0209f4a0 + 0));
                } else if (frame == cp_frame) {
                    fprintf(stderr, "[course] unknown probe '%s'\n", cp_what);
                    cp_done = 1;
                }
            }
        }

        /* Stage::Behavior's LAST statement, and it goes HERE rather than at the
           end of the port's behaviour phase. The ROM's own text is

               if ((u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) == 0)
                   if ((data_0209b454 & ~0x20000000) == 0)
                       ShadowModel::CleanAll();

           and the port holds all four of those globals at 0 for this boot
           (hal/level_boot.cpp seeds them beside the camera state), so writing
           the guard out would only be repeating a constant.

           WHY BEFORE THE ACTOR TICK. The three ShadowModel TUs describe a
           freeze protocol between them: InitModel refuses to link a node while
           data_0209ceec is set, RenderAll SETS it as its last act, CleanAll
           CLEARS it. So a frame's registrations can only happen after that
           frame's CleanAll and before its RenderAll, and the registrations are
           made from actors' own Behavior methods (SignPost::Behavior and
           ArrowSignRight::Behavior are the two matched examples). On the ROM
           that works because the Stage ticks at the head of the behaviour list
           -- its spawn record at 0x0209213c carries behaviour priority 3
           against the hundreds other classes use. The port's equivalent of
           "before every other actor's Behavior" is right here, above
           port_actor_tick. Putting it at the end of the phase instead would
           empty the list the actors had just filled, and once ov001 mounts
           that would be shadows that never draw.

           real_boot rather than boot_spawns because the Stage exists on the
           no-spawn boot too; menu_on because the ROM does not reach
           Stage::Behavior at all on a paused frame.

           AND ITS SIBLING IS SEATED BELOW, THE WALL DOWN AT LAST (run linkw
           wave 5, lane w5-d). Stage::Render's ShadowModel::RenderAll sits
           between RenderModel and RenderModelTransparent below, and the
           two-layer history is why the ordering HERE matters and why a green
           run alone never proved anything.

           Layer one, wave 3: seated, and walk_window took an access
           violation on frame 1:

             FAULT code c0000005 at +0x000893c8 accessing 00000000
               stack[03] +0x000892b1

           +0x893c8 is eight bytes into func_02046120 and +0x892b1 is inside
           RenderAll, i.e. `int n = self->sub->count` with self null, where
           self is the list node's `data` (its ModelComponents at +0x08).
           That was the InitCylinder stub leaving nodes empty; wave 4's first
           half fixed the chain and the fault with it.

           Layer two, wave 4: seated again, ran clean, drew WRONG. ntr then
           rasterized POLYGON_ATTR mode-3 (shadow) polygons as ordinary
           translucent geometry -- gx.cpp decoded only cull and alpha from
           that register -- so RenderAll painted the shadow VOLUME itself as
           a visible column under the actor (Tango's cone, tallest mid-jump
           because scale.y carries the drop height). The w4a review reverted
           the seat and queued the raster work.

           That work landed this wave: gx.cpp carries a per-pixel stencil
           bit and polygon-ID buffer and GBATEK's two-step shadow protocol
           (mask ID 0 stencils depth-fail pixels; draw ID nonzero blends
           where the stencil is set, the depth test passes and the IDs
           differ). The volume no longer draws; its intersection with the
           ground does.

           Why the list is not empty, for the record: Butterfly::Behavior
           calls the matched Actor::DropShadowRadHeight every frame on this
           level (the boot census spawns butterflies), which calls
           ShadowModel::InitModel for real, so real nodes ride the list from
           frame 1 and RenderAll has real work the moment it is called.

           WHAT A GREEN RUN DOES NOT PROVE, still. Seating RenderAll with
           this CleanAll call moved to the END of the behaviour phase also
           runs clean -- but only because the list is force-emptied after
           the actors fill it and before RenderAll reads it, which is the
           wrong order and draws no shadows at all. That is why CleanAll
           stays here, above the tick, and why the seat is held to pixels
           and triangles rather than exit codes: SM64DS_SHADOW_TRIS=1 prints
           what the seat submits per frame (see the call site). */
        if (real_boot && !menu_on)
            ShadowModel::CleanAll();
        /* STAR1: Stage::Behavior:112 -- advance the running cutscene/kuppa
           script one frame. The port transcribes Stage::Behavior statement by
           statement and this one was missing. Without it a script launched by
           the star-get (func_0200ee8c -> RunKuppaScript, which seats
           data_0209fc48 and calls ProcessKuppaScript exactly ONCE) never
           advanced: ProcessKuppaScript's own frame cursor data_0209b274 froze,
           the script never reached its end so EndKuppaScript never ran, and
           data_0209fc48 stayed latched. Stage::CheckInput then zeroed the whole
           Ctrl block every frame, the stick copy-bridge carried the zero
           (mDesiredAngleY/mag = 0, no walk) while the directly-written button
           word kept jump/kick alive -- the reported "can't move after a star,
           can still jump/kick/change levels". ProcessKuppaScript early-returns
           when data_0209fc48 == 0, so it is free off a cutscene and leaves every
           script-free selftest byte-identical. Gated to the game tick like the
           ROM, which does not run Stage::Behavior on a paused frame.
           SM64DS_NO_KUPPA_TICK=1 restores the old behaviour on this binary. */
        static int no_kuppa_tick = -1;
        if (no_kuppa_tick < 0)
            no_kuppa_tick = getenv("SM64DS_NO_KUPPA_TICK") ? 1 : 0;
        if (real_boot && !menu_on && !no_kuppa_tick)
            ProcessKuppaScript();
        /* The intro-seen bit's EDGE, reported once when it moves. The opening's
           completion bar asks that flags2 bit 7 ends SET and that the PORT
           never sets it -- the write is the ROM's own, src/func_ov085_0212d5dc
           .cpp:51, LakituBro's last opening state. A boot-time read cannot show
           that: on the reload boot the bit is still clear, because the flight's
           ending state runs during the level that follows. Read next to the
           cutscene tick because that is the thing whose end sets it. Inert
           unless SM64DS_INTRO_WATCH (hal/level_boot.cpp). */
        port_intro_bit_edge();
        if (menu_on) {
            game_ticked = 0;
        } else if (boot_spawns) {
            /* Nothing to undo before the tick any more. The view matrix is
               the one Camera::Render published, in the ROM's own scene units,
               and Actor::BeforeBehavior reads exactly those three words to
               place every actor for the Clipper. */
            /* THE CHARACTER INTRO CUTSCENE IS NOT HOSTED, so hold its id at 0
               every tick rather than once at startup -- the level-enter sets it
               AFTER the Player exists, which is why clearing it at spawn did
               nothing. func_ov002_020c4188 is that cutscene and it is built on
               the Message box the port does not have; with the message guarded
               to a no-op it just faults one step further along, on the object
               the message was supposed to have made. Yoshi enters it every run
               (Mario never does, Luigi and Wario survive 300 frames without
               it), so this is the difference between Yoshi being playable and
               not. Zero means the state machine returns on its first line. */
            if (*(unsigned char *)(c + 0x71e) && !getenv("SM64DS_INTRO_CUTSCENE")) {
                static int said;
                if (!said) {
                    said = 1;
                    fprintf(stderr, "[char] intro cutscene %u suppressed "
                            "(not hosted)\n",
                            (unsigned)*(unsigned char *)(c + 0x71e));
                }
                *(unsigned char *)(c + 0x71e) = 0;
            }
            /* THE SAVE-PROMPT FLAG, and this line is a stand-in for
               Stage::LC_Update's own clear (its case-6 arm ends with
               data_0209f20c = 0). A star-return landing's last entrance step
               (func_ov002_020c7350, and _020c6fe4's arm) sets the flag to
               open the "do you want to save?" prompt, and on the ROM the
               Stage -- whose Scene-class BeforeBehavior is not gated by it --
               drives that prompt and clears it. The port does not tick the
               Stage's LC machinery, so a set flag would gate
               Actor::BeforeBehavior for every actor forever: the player
               finishes the landing jig and the world freezes one frame before
               step 2 (the 2026-08-07 warp-freeze session). Clearing it here,
               before the tick, is that one statement and nothing else; the
               prompt it would have opened is not hosted. Retiring this is the
               same named job as the +0x13 stand-in in stage_bridges.cpp: run
               the Stage as an actor and let LC_Update own its flag. */
            if (data_0209f20c[0]) {
                static int said_lc;
                if (!said_lc) {
                    said_lc = 1;
                    fprintf(stderr, "[lc] save-prompt flag cleared "
                            "(Stage::LC_Update stand-in; prompt not hosted)\n");
                }
                data_0209f20c[0] = 0;
            }
            /* TEMPORARY: arm the buddy's talk detection before the actor tick so
               his state-0 main runs the real StartTalk. SM64DS_BUDDY_TRIGGER. */
            port_input_probe_buddy_trigger(frame);
            port_input_probe_sign_trigger(frame);   /* TEMPORARY: SM64DS_SIGN_TRIGGER */
            port_probe_alcheck();
            port_probe_sign_yaw();
            port_probe_chomp(frame);
            port_probe_rabbit_trigger(frame);  /* TEMPORARY: SM64DS_RABBIT_TRIGGER */
            port_probe_key_spawn(frame);       /* TEMPORARY: SM64DS_KEY_SPAWN_AT */
            port_probe_vs_overlap(frame);      /* test fixture: SM64DS_VS_OVERLAP_AT */
            port_actor_tick();
        } else if (*(void **)(c + 0x370)) {
            hal_player_behavior(player);
        } else {
            hal_player_st_wait_main(player);
        }
        /* THE FRAME CLOCK, func_020197b8 phase 6 (hal/fader_wipes.cpp): after
           the actor phases the branch above ran, before the render below. ONE
           PHASE EARLY against the ROM, which steps it at phase 6 -- after phase
           5, and so after its phase 2 fade advance -- where this sits before
           port_fader_advance. Nothing between the two reads the word, so no
           linked reader can tell; the banner carries the argument. Gated on
           game_ticked -- the ROM has no pause, so a frozen frame holding its
           blinks still is this port's decision and the same one port_actor_tick
           makes. hal/scene_boot.cpp's port_scene_tick calls it at the matching
           point on the scene path. Read that file's banner before moving it:
           every blink in the game hangs off this one counter. */
        if (game_ticked)
            port_frame_clock_tick();
        /* PHASE 2's HEAD, the graphics block's word 0 (hal/scene_boot.cpp).
           The ROM's func_02019390 dispatches it before the fade advances below,
           and slot 23 -- the stylus stroke test -- has no other dispatch site in
           the game. GATED ON THE TICK, unlike the fade under it: a fade must
           keep moving while the debug menu holds the world still, and a stylus
           stroke must not be accepted by a paused game.

           IT CANNOT REACH ANYTHING ON THIS PATH TODAY and it is here anyway.
           The seated blocks are the minigame block and the title block, both
           installed on the scene path, so on a level the registry check misses
           and this returns 1 without dispatching. Leaving the level loop
           without the beat is the same half-wiring that cost 384 its stylus. */
        if (game_ticked)
            port_graph_block_word0();
        /* THE FADE STEPS HERE, and it steps every frame -- even with the menu
           open and the game tick skipped -- because a fade transition must not
           freeze while it is on screen. This is func_02018ec0's job in the
           ROM's own frame (phase 2, func_02019390): advance the fader currently
           in motion (data_0209d4b0) by one frame, which writes the 2D blend
           register the compositor below reads. */
        port_fader_advance();
        /* THE DISPLAY SCAN-OUT, and with it IRQ 2. The DS raises the HBlank
           edge once per scanline while the picture is drawn; the ROM's
           dWipe_c motion path is built on it and nothing on the host used to
           raise it. Here, beside the fade step, because both are the ROM's
           own frame phase 2 and because everything below this point is the
           host rasteriser rather than game code. Costs nothing on a frame
           with no mask-2 handler registered: the gate is five loads.
           SM64DS_IRQ2_OFF=1 puts the old behaviour back on this same binary.
           See port/irq2_map.txt. */
        ntr::rt_scanout_frame();
        /* THE MESSAGE-BOX PROBE (temporary), then THE PUMP. SM64DS_PROBE_MESSAGE
           opens a dialogue box a few seconds in so the pipeline is checkable
           without a real in-world caller (the sign's read-state Main is
           unmatched and held in a parallel lane). Fire once at frame 90 (3s at
           30fps), after the player exists and his St_Wait/Walk state is settled
           so ShowMessage2's state guard admits the talk. */
        {
            const int probe_id = port_probe_message_id();
            if (probe_id >= 0 && frame == 90) {
                static int fired;
                if (!fired) { fired = 1; port_probe_message_fire(player, probe_id); }
            }
        }
        /* THE PUMP: Stage::UpdateMessage's dialogue arm, run every frame the way
           Stage::Behavior runs it on the ROM. Advances Message::UpdateWindow +
           Message::Update, which writes engine A's box registers that the
           compositor below reads. Stepped here, after the player tick that can
           open the box (St_Talk_Main -> func_0201f32c) and beside the fader. */
        port_message_pump();
        /* AND ITS OTHER ARM. On the ROM these two are the same statement:
           Stage::Behavior branches on data_0209f2d8 == 1 and calls
           UpdateMessage on the adventure side, the VS block on the other. The
           countdown is the head of that block, and its last act -- after the
           three beeps and the GO -- is Sound::LoadAndSetMusic_Layer1(0x4d),
           which is the only music a VS arena has. Self-guarded on the mode, so
           an adventure frame reaches one load and a compare. */
        port_vs_countdown_tick();
        port_input_probe_trace_msg(frame);   /* TEMPORARY: SM64DS_TRACE_MSG */
        port_input_probe_trace_cannon(frame);/* TEMPORARY: SM64DS_TRACE_CANNON */
        port_probe_rabbit_key(frame);        /* TEMPORARY: SM64DS_TRACE_RABBITKEY */
        /* the real boot seats the path table, so the tracking's own binding
           stands -- except where the port's unfilled floor record invents
           one the level cannot produce (hal/level_boot.cpp) */
        if (real_boot)
            port_stage_path_guard(player);
        ph_end(PH_INPUT, t_phase);
        /* THE DECEL CURVE. One line per frame after the tick, so the speed
           printed is the one this frame's physics produced. dv is the change
           since last frame -- the per-frame brake rate, which is the number
           the DS comparison is actually about. The trailing fields are the
           inputs to the branch that picks that rate: the slip class, the
           brake rate 020bf56c hands back for it, the skid flag at +0x6e0,
           the no-input latch at +0x6ac and the metal/underwater flag. */
        if (decel_probe && !decel_stopped) {
            static int prev_spd = 0, prev_px = 0, prev_pz = 0;
            const int spd = *(int *)(c + 0x98);
            const int cls = func_ov002_020c031c(c);
            const int px = *(int *)(c + 0x5c), pz = *(int *)(c + 0x64);
            /* step is what the speed scalar was WORTH in world units this
               frame. If step and spd ever disagree the bug is downstream of
               the integrator, in whatever turns heading+speed into motion. */
            const double dxf = (px - prev_px) / 4096.0,
                         dzf = (pz - prev_pz) / 4096.0;
            fprintf(stderr,
                    "[decel] f%-4d spd %8d (%7.3f)  dv %7d  step %6.3f  "
                    "cls %d brake %6d  top %6d  6e0 %d 6ac %d 703 %d 6ed %2d "
                    "6e5 %2d  ang %04x->%04x\n",
                    frame, spd, spd / 4096.0, spd - prev_spd,
                    frame ? sqrt(dxf * dxf + dzf * dzf) : 0.0, cls,
                    func_ov002_020bf56c(c, 0x2000),
                    Player_ScaleByCharFactor(c, 0x28000),
                    *(unsigned char *)(c + 0x6e0),
                    *(unsigned short *)(c + 0x6ac),
                    *(unsigned char *)(c + 0x703),
                    *(unsigned char *)(c + 0x6ed),
                    *(unsigned char *)(c + 0x6e5),
                    (unsigned short)*(short *)(c + 0x94),
                    (unsigned short)*(short *)(c + 0x6d2));
            prev_px = px; prev_pz = pz;
            if (decel_probe == 1 && frame > DECEL_RELEASE && spd == 0) {
                fprintf(stderr, "[decel] stopped at frame %d (%d frames "
                        "after release)\n", frame, frame - DECEL_RELEASE);
                decel_stopped = 1;
            }
            prev_spd = spd;
        }
        if (selftest && frame == 0)
            fprintf(stderr, "[w] ticked\n");
        /* the camera's own frame: Behavior runs the state machine and
           hands its heading to func_0203dafc (which writes the LOCAL comms
           record), then func_0203e0ac -- the single-player echo of
           func_0203df40 -- copies that record into the four per-player
           records GetAngleToCamera reads. Without the second call the
           published angle never moves and Mario walks relative to a stale
           heading. */
        ph_begin(&t_phase);
        /* STAR1 fly-around, the cutscene-camera gate. While a cutscene script
           is running (data_0209fc48 != 0) the kuppa script feeds camera
           commands to Camera::Behavior and the actor publishes its own
           script-driven view (data_0209b3ec) and heading. In real play the
           default rig is CAM_ANALOG, which re-pushes its OWN view matrix
           (fc_push_view, below) and heading (*data_020a1050 = fc_yaw, below)
           every frame -- overwriting exactly what the script just published, so
           the fly-around runs but never shows. CAM_DS writes neither, which is
           why the fly-around is already visible there. So while fc48 is latched
           the rig stands down on both writes and the actor's own view/heading
           present; when EndKuppaScript clears fc48 back to 0 the rig resumes
           exactly as before. The gate is a no-op off a cutscene (fc48 == 0), so
           every script-free frame -- every selftest, all normal walking -- is
           byte-identical to base. SM64DS_NO_CUTSCENE_CAM=1 forces the old
           always-rig behaviour for the A/B. */
        static int no_cutscene_cam = -1;
        if (no_cutscene_cam < 0)
            no_cutscene_cam = getenv("SM64DS_NO_CUTSCENE_CAM") ? 1 : 0;
        const int cutscene_cam = !no_cutscene_cam && data_0209fc48 != 0;
        /* the analog rig's pivot is stepped here, after the tick moved Mario
           and before anything reads it */
        if (cam_mode == CAM_ANALOG) an_step_pivot(c);
        if (real_camera) {
            hal_camera_behavior(cam);
            /* THE ONE THING THE RIG OVERRIDES BESIDES THE VIEW: the heading
               the walk steers by. Camera::Behavior has just put its own into
               the local comms record; in analog and in freecam the rig's
               heading goes in instead, so "forward" is away from the lens the
               player is actually looking through. The echo below is what
               copies it into the record GetAngleToCamera reads, so this has to
               land between the two -- and it is one halfword either way, the
               same single write the freecam always did. During a cutscene
               (cutscene_cam) the rig stands down so the actor's own heading --
               the one Camera::Behavior just wrote from the script -- survives
               into the echo below. */
            if (cam_mode != CAM_DS && !cutscene_cam)
                *(short *)data_020a1050 = fc_yaw;
            /* run mg16 lane MP3: THE ROM'S OWN DISPATCHER, LINKED.
               src/func_0203df40.c fills the local comms record from the pad and
               the touch panel and then switches on the role byte: 1 and 2 reach
               src/func_0203ea5c.c, the real lockstep; 3 reaches Download Play;
               0 and anything else reach func_0203e0ac, the solo cascade this
               line used to call directly.

               MP2 had a hosted transcription here (hal/comms_lockstep.cpp) and
               an `if (!tick()) func_0203e0ac();` around it, because
               func_0203ea5c was in no slice. Both are gone: the role-0 arm is
               INSIDE func_0203df40's own switch, so calling it here as well
               would run the solo cascade twice on every single-player frame.

               WITH NO TRANSPORT INSTALLED NOTHING CHANGES. data_020a0f04 comes
               up 0 and only a transport moves it, so the switch takes its
               default arm and func_0203e0ac runs exactly as it always has --
               now reached through the ROM's own dispatcher rather than around
               it. That is the whole point, and rung 1 is what proves it.

               ORDERING: comms_publish_pad below is what makes :31 of that TU
               read a real pad, and it must land before this call and before the
               fan-out. See THE STUCK CONTROLLER in hal/comms_conductor.cpp.

               BOTH HALVES of the host pad go in: the d-pad stash and the
               button stash (run mg16 lane MPBTN). Publishing only the first
               is what made every session's key word a d-pad nibble and every
               button dead once the direct Ctrl stores were gated. */
            port::comms_publish_pad(port_raw_pad_bits() | port_raw_btn_bits());
            func_0203df40();
            /* run mg16 lane MP4: one frame of the state-sync layer, AFTER the
               conductor. Call position is the contract's ordering rule made
               structural: func_0203df40 above has already put this frame's
               input record on the wire, so aux can never delay the thing the
               lockstep blocks on. No-op unless SM64DS_SYNC=1 and the transport
               reports contract v2. */
            port::sync_tick();
            /* run mg16 lane MP3: the VS probe, read out of the game's own
               per-slot actor array. Here rather than at the report site
               because it must run whether or not SM64DS_COMMS_REPORT is on:
               rungs 9 to 11 are about ACTORS, and a single-instance two-player
               run has no comms report to hang off. Silent unless
               SM64DS_VS_PROBE is set. */
            port::vs_probe(frame);
            /* run mg15 lane MP1: the ROM's OWN steps 0x16 and 0x17, right
               where src/func_020197b8.c runs them -- immediately after the
               comms tick that filled the four records. func_0203bb60 turns
               those four into TouchInfo[4] and func_0203bc7c turns them into
               PadData[4], which is where every stylus and button read in the
               game comes from.

               OFF BY DEFAULT, and that is not timidity. The port writes those
               two arrays DIRECTLY today (hal/input_probe.cpp, the scene
               publish in hal/scene_boot.cpp), so turning this on hands the
               whole input path to the ROM's four-slot route in one step. That
               swap is MP2's, because that is the change with a regression
               surface, and MP1's solo proof is only worth something if the
               default path is untouched.

               It is a REAL call site and not a linker directive, which is the
               standard port/hal/w8a_stage_faces.cpp set after a review found
               eleven TUs of directive-manufactured linkage: something has to
               actually call the body for it to count as linked. */
            if (comms_fanout_on()) {
                port::comms_fanout();
                if (comms_fanout_report()) {
                    port::comms_report("level");
                    /* run mg16 lane MP2: the carrier's own half of the same
                       readout. Silent when no transport is installed, so an
                       MP1-era SM64DS_COMMS_REPORT run prints what it always
                       printed. */
                    if (port::comms_transport()) {
                        /* run mg16 lane MP3: the lockstep's own counters are
                           GONE, and that is a real cost of linking the real
                           thing rather than an oversight. MP2 counted ticks,
                           rounds, timeouts, spins and peer_updates from inside
                           its transcription. Those live inside
                           src/func_0203ea5c.c now -- the spin loop, the bound,
                           the per-slot unpack -- and instrumenting them would
                           mean editing a byte-matched TU, which this repo does
                           not do. What survives is what the SEAM can see:
                           comms_report's exchanges/rounds count real calls
                           through func_020406b4, and the carrier's own
                           counters are unaffected. Saying which numbers went
                           away is more useful than quietly printing fewer. */
                        port::comms_loopback_report("level");
                        /* run mg16 lane MP4: the sync layer's own counters,
                           beside the carrier's. Silent-ish when the layer is
                           off (it prints enabled=no), which is what lets rung
                           SY4 assert the off path stays quiet on the SOLO run
                           where no report is requested at all. */
                        port::sync_report("level");
                    }
                }
            }
            if (trace_cam)
                fprintf(stderr,
                        "[cam-in] f%03d rx=%6d ry=%6d fc=%d yaw=%04x "
                        "pitch=%04x dist=%d held=%04x edge=%04x fl=%08x "
                        "a17c=%04x a186=%04x a19e=%04x turn=%u wall=%u "
                        "pub=%04x mario=%04x\n",
                        frame, stick_rx, stick_ry, cam_mode,
                        (unsigned short)fc_yaw, (unsigned short)fc_pitch,
                        fc_dist >> 12,
                        *(unsigned short *)(data_0209f49c + 0),
                        *(unsigned short *)(data_0209f49e + 0),
                        *(unsigned *)((char *)cam + 0x154),
                        (unsigned short)*(short *)((char *)cam + 0x17c),
                        (unsigned short)*(short *)((char *)cam + 0x186),
                        (unsigned short)*(short *)((char *)cam + 0x19e),
                        *(unsigned short *)((char *)cam + 0x1a0),
                        *(unsigned char *)((char *)cam + 0x1a6),
                        (unsigned short)*(short *)((char *)data_020a1164),
                        /* Mario's own facing. Paired with `pub` this is what
                           says which world direction is SCREEN-right: the
                           walk is camera-relative, so facing minus pub is the
                           stick direction the game resolved, and its offset
                           from the straight-ahead 0x8000 has the sign of the
                           side being pushed. Nothing reads it, it just makes
                           the camera binding measurable without a screenshot. */
                        (unsigned short)*(short *)(c + 0x8e));
        }
        ph_end(PH_CAMERA, t_phase);
        /* no speed clamp: the accel tables get real input-mode data now
           that Stage::CheckInput fills the record (the old runaway came
           from fake mode bytes) */

        if (selftest) {
            void *st = *(void **)(c + 0x370);
            unsigned bid =
                _ZNK6Player14GetBodyModelIDEjb(c, *(int *)(c + 8) & 0xff, 0);
            char *ma = ((char **)(c + 0xdc))[bid];
            unsigned bh = 2166136261u;
            if (ma) {
                const unsigned char *bb =
                    (const unsigned char *)*(char **)(ma + 0x14);
                for (int k = 0; k < 0x300; ++k)
                    bh = (bh ^ bb[k]) * 16777619u;
            }
            fprintf(stderr,
                    "[f%03d] pos=(%.1f,%.1f,%.1f) spd=%d st=%08x mag=%d "
                    "body=%u anim(len=%u fl=%u cur=%.1f) bones=%08x path=%x\n",
                    frame, *(int *)(c + 0x5c) / 4096.0f,
                    *(int *)(c + 0x60) / 4096.0f,
                    *(int *)(c + 0x64) / 4096.0f, *(int *)(c + 0x98),
                    st ? *(unsigned *)st : 0u, *(short *)(data_0209f4a0 + 0),
                    bid, ma ? (*(unsigned *)(ma + 0x54)) & 0x3FFFFFFF : 0,
                    ma ? (*(unsigned *)(ma + 0x54)) >> 30 : 0,
                    ma ? *(int *)(ma + 0x58) / 4096.0f : 0.0f, bh,
                    *(unsigned *)(c + 0x670));
            if (frame <= 2) {
                fprintf(stderr,
                        "[clip] minZ=%d maxZ=%d p0=(%d,%d,%d) cam nf=(%d,%d,%d)\n",
                        data_0209f43c[0x50 / 4], data_0209f43c[0x54 / 4],
                        data_0209f43c[1], data_0209f43c[2], data_0209f43c[3],
                        *(int *)((char *)cam + 0xf8),
                        *(int *)((char *)cam + 0xfc),
                        *(int *)((char *)cam + 0x100));
            }
            for (int *node = (int *)(size_t)data_020a4b78[0]; node;
                 node = (int *)(size_t)node[1]) {
                char *o = (char *)(size_t)node[2];
                if (!o) continue;
                unsigned id = *(unsigned short *)(o + 0xc);
                if (id != 336 && id != 344 && id != 343 && id != 187)
                    continue;
                unsigned char hint = *(unsigned char *)(o + 0xc4);
                int r = _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(
                    (char *)data_0209f43c, o + 0x74, *(int *)(o + 0xb8),
                    &hint);
                int k = 0;
                for (; k < g_amb_n; ++k)
                    if (g_amb[k].o == o) break;
                if (k == g_amb_n && g_amb_n < 16) {
                    g_amb[k].o = o;
                    g_amb[k].id = id;
                    g_amb[k].minr = 0x7FFFFFFF;
                    g_amb[k].minfr = -1;
                    g_amb[k].thresh = *(int *)(o + 0xbc);
                    g_amb[k].p0[0] = *(int *)(o + 0x5c);
                    g_amb[k].p0[1] = *(int *)(o + 0x60);
                    g_amb[k].p0[2] = *(int *)(o + 0x64);
                    ++g_amb_n;
                }
                /* frame 0's view-space pos predates the first Camera::Render
                   (identity view matrix) -- not a real approach, skip it */
                if (k < g_amb_n && frame >= 1 && r < g_amb[k].minr) {
                    g_amb[k].minr = r;
                    g_amb[k].minfr = frame;
                }
                if (frame >= 1 && frame <= 2)
                    fprintf(stderr,
                            "[amb] id=%u fl=%08x area=%d pos=(%d,%d,%d) "
                            "vp=(%d,%d,%d) b4=%d b8=%d bc=%d c0=%d r=%d\n",
                            id, *(unsigned *)(o + 0xb0),
                            *(signed char *)(o + 0xcc),
                            *(int *)(o + 0x5c) >> 12, *(int *)(o + 0x60) >> 12,
                            *(int *)(o + 0x64) >> 12, *(int *)(o + 0x74),
                            *(int *)(o + 0x78), *(int *)(o + 0x7c),
                            *(int *)(o + 0xb4), *(int *)(o + 0xb8),
                            *(int *)(o + 0xbc), *(int *)(o + 0xc0), r);
            }
        }
        /* live-play recorder: state changes, input edges, a fix every
           second -- cheap enough to always be on, lands in playlog/ */
        if (!selftest) {
            static const void *rec_st;
            static unsigned short rec_btn, rec_raw;
            static int rec_f;
            const void *st_ = *(void **)(c + 0x370);
            unsigned short btn_ = *(unsigned short *)(data_0209f49c + 0);
            unsigned short raw_ =
                *(unsigned short *)((char *)data_020a0e58 + 0);
            if (st_ != rec_st) {
                fprintf(stderr,
                        "[st] f%d %08x pos=(%.1f,%.1f,%.1f) spd=%d vy=%d\n",
                        rec_f, st_ ? *(const unsigned *)st_ : 0u,
                        *(int *)(c + 0x5c) / 4096.0f,
                        *(int *)(c + 0x60) / 4096.0f,
                        *(int *)(c + 0x64) / 4096.0f, *(int *)(c + 0x98),
                        *(int *)(c + 0xa8));
                rec_st = st_;
            }
            if (btn_ != rec_btn || raw_ != rec_raw) {
                fprintf(stderr, "[in] f%d btn=%04x raw=%04x\n", rec_f, btn_,
                        raw_);
                rec_btn = btn_;
                rec_raw = raw_;
            }
            if ((rec_f % 30) == 0)
                fprintf(stderr, "[fx] f%d pos=(%.1f,%.1f,%.1f) camang=%d\n",
                        rec_f, *(int *)(c + 0x5c) / 4096.0f,
                        *(int *)(c + 0x60) / 4096.0f,
                        *(int *)(c + 0x64) / 4096.0f,
                        *(short *)((char *)data_020a1164 + 0));
            rec_f++;
            /* SM64DS_TRACE_SURF=1: the surface record the ground tracking
               pulled out of the CLPS entry under his feet, plus the last
               triangle the octree walk accepted. This is how the unfilled
               WithMeshClsn floor record was caught -- the walk's last
               triangle stays put while the record changes underneath. */
            if (getenv("SM64DS_TRACE_SURF")) {
                /* the floor ClsnResult itself: WithMeshClsn + 0x20 (the
                   SphereClsn sub-object) + 0x74 (func_02037938). Its first
                   two SurfaceInfo words ARE the CLPS entry, so a record the
                   walk really filled reads back as one of the level's 22
                   entries -- 000?0fc? / 000000ff for castle grounds. */
                const int *fr = (const int *)(c + 0x380 + 0x20 + 0x74);
                fprintf(stderr, "       surf path=%x t=%d %d %d %d %d "
                        "lastTri=%d attr=%x clps=%08x/%08x tri=%u slot=%u\n",
                        *(unsigned *)(c + 0x670), *(int *)(c + 0x66c),
                        *(int *)(c + 0x660), *(int *)(c + 0x65c),
                        *(int *)(c + 0x664), *(int *)(c + 0x658),
                        g_walk_dbg[13], g_walk_dbg[14], fr[1], fr[2],
                        *(const unsigned short *)((const char *)fr + 0x18),
                        *(const unsigned short *)((const char *)fr + 0x1a));
            }
            /* SM64DS_TRACE_WATER=1: the water chain, end to end, in the order
               the ROM runs it. func_ov002_020c14b8 casts a RaycastGround from
               300 units up with StartDetectingWater + StopDetectingOrdinary
               and, on a surface whose CLPS carries flag 0x20, writes the hit
               height into Player+0x64c and into the global data_0209f32c.
               func_ov002_020c0fb4 then hands +0x64c to func_ov002_020c0d90,
               which is the walk->swim decision. So:
                 water=-2147483648 (0x80000000)  the probe found no water
                 water=<y> with swim never entered  the decision refused
               and 0x706/0x707 are mIsUnderwater / mIsInShallowWater. */
            if (getenv("SM64DS_TRACE_WATER"))
                fprintf(stderr, "       water probe=%d (%.1f) global=%d (%.1f) "
                        "y=%.1f under=%u shallow=%u airborne=%u vspd=%d\n",
                        *(int *)(c + 0x64c), *(int *)(c + 0x64c) / 4096.0f,
                        data_0209f32c[0], data_0209f32c[0] / 4096.0f,
                        *(int *)(c + 0x60) / 4096.0f,
                        *(unsigned char *)(c + 0x706),
                        *(unsigned char *)(c + 0x707),
                        *(unsigned char *)(c + 0x6de), *(int *)(c + 0xa8));
        }
        if (selftest) {
            /* actor-list-head stomp tracker (the f015 0x1000 write) */
            static int prev_head[4], head_live;
            if (head_live &&
                memcmp(prev_head, data_0209b468, sizeof prev_head))
                fprintf(stderr, "[head] f%03d %08x %08x %08x %08x\n",
                        frame, data_0209b468[0], data_0209b468[1],
                        data_0209b468[2], data_0209b468[3]);
            memcpy(prev_head, data_0209b468, sizeof prev_head);
            head_live = 1;
        }

        /* RETIRED harness ground snap: a real KCL ray under Mario each
           frame. OFF unless SM64DS_FAKE_SNAP=1 brings the whole pre-sphere
           configuration back. It existed because the game's own tracking
           could not ground him with the sphere pass stubbed; the pass is
           hosted now and holds him exactly, so this is an A/B switch and a
           way to plant him for shots, not a mode the port runs in. */
        {
            int gy;
            int mx = *(int *)(c + 0x5c), my = *(int *)(c + 0x60),
                mz = *(int *)(c + 0x64);
            /* ray starts just above STEP height: starting a body-height
               up let the walk grab canopies/domes overhead and teleport
               him upward (the "camera is fucked" y-pops) */
            if (g_fake_snap &&
                hal_ground_ray(g_mc, mx, my + (100 << 12), mz, 5220 << 12,
                               &gy)) {
                /* never re-ground a rising jump: the snap + SetGroundFlag
                   on the first ascent frame would land him instantly */
                if (*(int *)(c + 0xa8) <= 0 && my <= gy + 0x20000) {
                    *(int *)(c + 0x60) = gy;
                    if (*(int *)(c + 0xa8) < 0)
                        *(int *)(c + 0xa8) = 0;   /* mVertSpeed */
                    _ZN12WithMeshClsn13SetGroundFlagEv(c + 0x380);
                    /* landing signal: St_Jump/Fall exit on this byte;
                       the real WithMeshClsn tracking will own it once
                       the continuous update runs on host */
                    *(unsigned char *)(c + 0x6de) = 0;
                }
            }
            /* RETIRED harness wall clamp, off with the snap and for the same
               reason: the game's wall pass IS its ground pass, one
               MeshCollider::DetectClsn(SphereClsn &) returning floor, wall
               and ceiling as a three-bit mask. Hosted, it stops him against
               the castle's outer wall and lets him SLIDE ALONG it, where
               this clamp only ever stopped him dead 120 units short. */
            if (g_fake_snap) {
                int nx = *(int *)(c + 0x5c), nz = *(int *)(c + 0x64);
                int ny = *(int *)(c + 0x60);
                int wy = ny + (120 << 12);          /* chest height */
                int a[3] = {prev_pos[0], wy, prev_pos[2]};
                int b[3] = {nx, wy, nz};
                int clip[3];
                long long ddx = (long long)nx - prev_pos[0];
                long long ddz = (long long)nz - prev_pos[2];
                if ((ddx | ddz) && hal_line_ray(g_mc, a, b, clip)) {
                    /* stop 120 units short of the wall along the motion */
                    long long len2 = ddx * ddx + ddz * ddz;
                    double len = len2 > 0 ? sqrt((double)len2) : 1.0;
                    double ux = ddx / len, uz = ddz / len;
                    *(int *)(c + 0x5c) =
                        clip[0] - (int)(ux * (120 << 12));
                    *(int *)(c + 0x64) =
                        clip[2] - (int)(uz * (120 << 12));
                    *(int *)(c + 0x98) = 0;         /* mHorzSpeed */
                }
            }
            /* the harness's own last-frame position, which only the wall
               stop reads -- the game's is Actor+0x68 and BeforeBehavior
               owns it now (gate 15) */
            prev_pos[0] = *(int *)(c + 0x5c);
            prev_pos[1] = *(int *)(c + 0x60);
            prev_pos[2] = *(int *)(c + 0x64);

            /* fell out of the world (walked or jumped past the KCL):
               back to the spawn point instead of an endless dive */
            if (my < (-19200 << 12)) {
                *(int *)(c + 0x5c) = spawn_x << 12;
                *(int *)(c + 0x60) = spawn_y << 12;
                *(int *)(c + 0x64) = spawn_z << 12;
                *(int *)(c + 0x98) = 0;   /* mHorzSpeed */
                *(int *)(c + 0xa8) = 0;   /* mVertSpeed */
            }
        }

        /* render: camera behind and above Mario, looking at him */
        ph_begin(&t_phase);
        ntr::gx_reset();
        /* the real Camera writes CLEAR_COLOR itself, out of its own
           0x10c..0x10f bytes -- which hold exactly this value */
        if (!real_camera)
            NTR_MMIO(uint32_t, 0x04000580) =
                0u | (0u << 8) | (255u << 16) | (191u << 24);
        ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
        ntr::gx_enable_lights(0x1);
        float px = *(int *)(c + 0x5c) / 4096.0f;
        float py = *(int *)(c + 0x60) / 4096.0f;
        float pz = *(int *)(c + 0x64) / 4096.0f;
        /* SCENE units everywhere below: pos >> 3, the ROM's own conversion. */
        const float sx = px / 8.0f, sy = py / 8.0f, sz = pz / 8.0f;
        if (selftest && frame == 0) {
            /* scene-space bounds probe: identity matrices, read the raw
               projected coords (with identity proj they ARE scene coords).
               Runs under either camera -- it resets the geometry state at both
               ends and the real camera republishes its matrices right after,
               so the check is available in the default configuration. */
            ntr::gx_reset();
            hal_render_player_world(player);
            size_t n = 0;
            const ntr::GxTriangle *ta = ntr::gx_polygons(n);
            float mn[3] = {1e30f, 1e30f, 1e30f}, mx[3] = {-1e30f, -1e30f, -1e30f};
            for (size_t i = 0; i < n; ++i)
                for (int v = 0; v < 3; ++v) {
                    float xyz[3] = {ta[i].v[v].x, ta[i].v[v].y, ta[i].v[v].z};
                    for (int k = 0; k < 3; ++k) {
                        if (xyz[k] < mn[k]) mn[k] = xyz[k];
                        if (xyz[k] > mx[k]) mx[k] = xyz[k];
                    }
                }
            printf("probe: %zu tris, x[%.1f..%.1f] y[%.1f..%.1f] z[%.1f..%.1f]\n",
                   n, mn[0], mx[0], mn[1], mx[1], mn[2], mx[2]);
            {
                /* Mario's own rendered height, the one number the migration
                   has to hold: identity projection, so the raw y span IS his
                   size in scene units. x8 back to world for the historic
                   reading (~145). */
                const float hh2 = ntr::SCREEN_H * 0.5f;
                const float ylo = 1.0f - mx[1] / hh2, yhi = 1.0f - mn[1] / hh2;
                printf("probe: mario scene y[%.2f..%.2f] height %.2f scene "
                       "(%.1f world)\n", ylo, yhi, yhi - ylo,
                       (yhi - ylo) * 8.0f);
            }
            printf("probe: mario fx pos (%d, %d, %d) -> world (%.1f, %.1f, %.1f)"
                   " scene (%.1f, %.1f, %.1f)\n",
                   *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64),
                   px, py, pz, sx, sy, sz);
            printf("probe: player scale vec c+0x80 = (%d, %d, %d) fx\n",
                   *(int *)(c + 0x80), *(int *)(c + 0x84), *(int *)(c + 0x88));
            ntr::gx_reset();
            if (real_boot) {
                /* Stage::Render's own order: the skybox, then the opaque
                   pass, then the translucent one. The skybox is the +0x9bc
                   Model glued to the camera eye (hal/stage_bridges.cpp); the
                   two model passes are the same Model drawn twice with
                   inverse visibility masks -- the moat water only exists in
                   the second. (ShadowModel::RenderAll sits between them on
                   the ROM and in the frame loop below; this one-shot probe
                   re-render leaves it out on purpose -- RenderAll sets the
                   freeze flag and rewrites material attrs, so it runs once
                   per frame, at its seat.) */
                port_stage_render_skybox(stage);
                port_stage_render_model(stage);
                port_stage_render_model_transparent(stage);
            } else {
                hal_render_model(level_model, level_shift);
            }
            n = 0;
            ta = ntr::gx_polygons(n);
            for (int k = 0; k < 3; ++k) { mn[k] = 1e30f; mx[k] = -1e30f; }
            for (size_t i = 0; i < n; ++i)
                for (int v = 0; v < 3; ++v) {
                    float xyz[3] = {ta[i].v[v].x, ta[i].v[v].y, ta[i].v[v].z};
                    for (int k = 0; k < 3; ++k) {
                        if (xyz[k] < mn[k]) mn[k] = xyz[k];
                        if (xyz[k] > mx[k]) mx[k] = xyz[k];
                    }
                }
            printf("probe: level %zu tris, x[%.0f..%.0f] y[%.0f..%.0f] z[%.0f..%.0f]\n",
                   n, mn[0], mx[0], mn[1], mx[1], mn[2], mx[2]);
            /* scene units back out of the identity-projection screen coords:
               sx = xs/(W/2)-1, sy = 1-ys/(H/2), sz = zs*2-1. VISUAL FLOOR
               CHECK: highest mesh vertex in Mario's column at/below his head
               height vs the KCL ground there -- a nonzero delta is the
               feet-sinking gap. The KCL is world fx, so it comes across the
               same >>3 the render does. */
            {
                const float hw = ntr::SCREEN_W * 0.5f,
                            hh = ntr::SCREEN_H * 0.5f;
                float vis = -1e30f, wxmin = 1e30f, wxmax = -1e30f,
                      wymin = 1e30f, wymax = -1e30f;
                for (size_t i = 0; i < n; ++i) {
                    float X[3], Y[3], Z[3];
                    for (int v = 0; v < 3; ++v) {
                        X[v] = ta[i].v[v].x / hw - 1.0f;
                        Y[v] = 1.0f - ta[i].v[v].y / hh;
                        Z[v] = ta[i].v[v].z * 2.0f - 1.0f;
                        if (X[v] < wxmin) wxmin = X[v];
                        if (X[v] > wxmax) wxmax = X[v];
                        if (Y[v] < wymin) wymin = Y[v];
                        if (Y[v] > wymax) wymax = Y[v];
                    }
                    /* interpolate the tri surface at (sx, sz) */
                    {
                        const float d = (Z[1] - Z[2]) * (X[0] - X[2]) +
                                        (X[2] - X[1]) * (Z[0] - Z[2]);
                        if (d > 1e-6f || d < -1e-6f) {
                            const float a =
                                ((Z[1] - Z[2]) * (sx - X[2]) +
                                 (X[2] - X[1]) * (sz - Z[2])) / d;
                            const float b =
                                ((Z[2] - Z[0]) * (sx - X[2]) +
                                 (X[0] - X[2]) * (sz - Z[2])) / d;
                            const float c2 = 1.0f - a - b;
                            if (a > -0.01f && b > -0.01f && c2 > -0.01f) {
                                const float wy =
                                    a * Y[0] + b * Y[1] + c2 * Y[2];
                                if (wy < sy + 150.0f / 8 && wy > vis) vis = wy;
                            }
                        }
                    }
                }
                {
                    int kgy = 0;
                    int kh = hal_ground_ray(g_mc, *(int *)(c + 0x5c),
                                            (int)((py + 1200) * 4096),
                                            *(int *)(c + 0x64),
                                            6000 << 12, &kgy);
                    const float kscene = kgy / 4096.0f / 8.0f;
                    printf("probe: model SCENE x[%.1f..%.1f] y[%.1f..%.1f] "
                           "(world x[%.1f..%.1f] y[%.1f..%.1f])\n",
                           wxmin, wxmax, wymin, wymax, wxmin * 8, wxmax * 8,
                           wymin * 8, wymax * 8);
                    printf("probe: FLOOR at scene col (%.1f,%.1f): visual=%.2f "
                           "kcl=%s%.2f delta=%.2f scene (%.1f world)\n",
                           sx, sz, vis, kh ? "" : "MISS ", kscene,
                           kh ? vis - kscene : 0.0f,
                           kh ? (vis - kscene) * 8.0f : 0.0f);
                }
            }
            ntr::gx_reset();
            NTR_MMIO(uint32_t, 0x04000580) =
                0u | (0u << 8) | (255u << 16) | (191u << 24);
            ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
            ntr::gx_enable_lights(0x1);
        }
        float dbg_eye[3] = {0, 0, 0}, dbg_at[3] = {0, 0, 0};
        if (real_camera) {
            /* THE CAMERA'S OWN FRAME. Render builds the projection from
               the mode preset (PerspectiveW_ -> MTX_LOAD_4x4) and the view
               matrix through LookAt_, then View::Render -> CopyToViewMat
               parks it in data_0209b3ec and its inverse in data_0209b41c.
               Model::Render composes every model matrix with data_0209b3ec
               in software, so THAT is where the camera reaches the raster,
               not the GX position stack. */
            hal_camera_render(cam);
            /* the rig's view goes on top of the camera's own, not instead of
               it: Render still seeds the Clipper, writes CLEAR_COLOR and
               keeps the actor's own state moving, and then the rig reloads
               the projection and the view matrix from its own eye. Nothing
               downstream can tell the difference -- it is the same three ROM
               calls, with different numbers.
               ANALOG orbits Mario (the eased pivot); FREECAM orbits the Camera
               actor's own look-at, which is what made it free of him.
               During a cutscene (cutscene_cam) the rig does NOT reload the
               view: hal_camera_render has just parked the script-driven view in
               data_0209b3ec, and leaving it there is what makes the star-get
               fly-around visible instead of overwritten. */
            if (cam_mode != CAM_DS && !cutscene_cam) {
                int fceye[3];
                const int *pivot = cam_mode == CAM_ANALOG
                                       ? an_pivot
                                       : (const int *)((char *)cam + 0x80);
                fc_eye(pivot, fceye);
                fc_push_view(cam, fceye, pivot);
            }
            /* THE ACTOR RENDER BUCKET GOES HERE, and the reason is the shim
               immediately below. Processing list 5 is the game's own render
               pass -- func_0204322c over slots 9/10/11, in render-priority
               order -- and everything on it is ROM code working in SCENE
               units: Tree::Render clips its cylinders through the Clipper
               with data_0209b3ec as it stands and writes scene-unit
               translations into its Models. The shim converts that same view
               matrix for the port's own world-unit models. So the bucket runs
               BEFORE the conversion and the harness's two draws after it, and
               each side gets the matrix it was written against. The raster is
               z-buffered, so drawing the actors ahead of the level model
               costs nothing.
               SM64DS_NO_ACTORS=1 takes the bucket out for the A/B. */
            static int no_actors = -1;
            if (no_actors < 0) no_actors = getenv("SM64DS_NO_ACTORS") ? 1 : 0;
            if (boot_spawns && !no_actors) {
                size_t before = 0, after = 0;
                if (selftest) ntr::gx_polygons(before);
                port_actor_render();
                /* THE PARTICLE SIMULATION GOES HERE, which is where
                   Stage::Render drives it. The SUBMISSION does not: it belongs
                   after the level, where Stage::GraphCallback1 runs it, and it
                   is called from there (port_particle_render, below the level
                   pass). Drawing translucent particles ahead of the opaque
                   level loses them all to the ground drawn over them. */
                /* SM64DS_SWITCH=<0..3> drives the cap-block character change
                   from a headless run, at frame 90, so the live path has a
                   regression probe instead of only being reachable by hand
                   through the F5 row. */
                {
                    static int sw = -2;
                    if (sw == -2) {
                        const char *s = getenv("SM64DS_SWITCH");
                        sw = s ? atoi(s) : -1;
                    }
                    if (sw >= 0 && frame == 90) {
                        fprintf(stderr, "[char] SM64DS_SWITCH becoming %s\n",
                                CHAR_NAME[sw & 3]);
                        port_player_set_character(c, sw);
                        g_character = g_character_pending = sw & 3;
                    }
                }
                port_particle_frame();
                if (selftest) {
                    const ntr::GxTriangle *at = ntr::gx_polygons(after);
                    if (frame == 0 || getenv("SM64DS_TRACE_ACTOR_TRIS")) {
                        float mnx = 1e30f, mxx = -1e30f, mny = 1e30f,
                              mxy = -1e30f, mnz = 1e30f, mxz = -1e30f;
                        for (size_t i = before; i < after; ++i)
                            for (int v = 0; v < 3; ++v) {
                                float X = at[i].v[v].x, Y = at[i].v[v].y,
                                      Z = at[i].v[v].z;
                                if (X < mnx) mnx = X;
                                if (X > mxx) mxx = X;
                                if (Y < mny) mny = Y;
                                if (Y > mxy) mxy = Y;
                                if (Z < mnz) mnz = Z;
                                if (Z > mxz) mxz = Z;
                            }
                        printf("[actors] render bucket: %zu triangles, screen "
                               "x[%.0f..%.0f] y[%.0f..%.0f] z[%.3f..%.3f]\n",
                               after - before, mnx, mxx, mny, mxy, mnz, mxz);
                        if (getenv("SM64DS_TRACE_ACTOR_TRIS"))
                            for (size_t i = before; i < after && i < before + 4;
                                 ++i)
                                printf("         tri (%.1f,%.1f,%.4f) "
                                       "(%.1f,%.1f,%.4f) (%.1f,%.1f,%.4f) "
                                       "tex %p %dx%d cull %u alpha %u\n",
                                       at[i].v[0].x, at[i].v[0].y, at[i].v[0].z,
                                       at[i].v[1].x, at[i].v[1].y, at[i].v[1].z,
                                       at[i].v[2].x, at[i].v[2].y, at[i].v[2].z,
                                       (const void *)at[i].tex, at[i].tw,
                                       at[i].th, at[i].cull, at[i].alpha);
                    }
                }
            }
            /* THE VIEW MATRIX IS USED AS THE ROM PRODUCED IT. Camera::Render
               feeds LookAt_ eye and lookAt as (v + 4) >> 3, so its translation
               row is in scene units and its rotation rows are plain unit
               vectors -- and every model matrix in the frame is now scene
               units too, so Model::Render's compose
               (out.t = model.t * view.R + view.t) has both terms in the same
               space. The R6 shim that scaled this row by 8 for the harness's
               world-unit models is gone, and with it the reason
               Actor::BeforeBehavior had to be handed the row back: it
               multiplies every actor's position through these same words for
               the Clipper, and an eight-times-too-long row read every actor
               with a cull distance as eight times too far away. */
            {
                /* FIELD-MAP CORRECTION, measured here: 0x8c is the camera
                   POSITION and 0x80 the point it looks at, not the other
                   way round. G3i::LookAt_ translates its matrix by the
                   `at` argument, and Camera::Render passes 0x8c there;
                   Camera::Behavior's own Vec3_HorzAngle(0x80, 0x8c) then
                   reads "from the focus toward the camera", which is what
                   the name GetAngleToCamera promises. */
                const int *ce = (const int *)((char *)cam + 0x8c);
                const int *cl = (const int *)((char *)cam + 0x80);
                for (int k = 0; k < 3; ++k) {
                    dbg_eye[k] = ce[k] / 4096.0f;
                    dbg_at[k] = cl[k] / 4096.0f;
                }
                if (selftest) {
                    float ddx = dbg_eye[0] - dbg_at[0];
                    float ddy = dbg_eye[1] - dbg_at[1];
                    float ddz = dbg_eye[2] - dbg_at[2];
                    fprintf(stderr,
                            "[cam] f%03d eye(%.1f,%.1f,%.1f) "
                            "at(%.1f,%.1f,%.1f) head=%04x pitch=%04x "
                            "fov=%d angle=%04x dist=%.0f\n",
                            frame, dbg_eye[0], dbg_eye[1], dbg_eye[2],
                            dbg_at[0], dbg_at[1], dbg_at[2],
                            (unsigned short)*(short *)((char *)cam + 0x17c),
                            (unsigned short)*(short *)((char *)cam + 0x17e),
                            *(short *)((char *)cam + 0x17a),
                            (unsigned short)*(short *)((char *)data_020a1164),
                            sqrtf(ddx * ddx + ddy * ddy + ddz * ddz));
                }
            }
        } else {
        /* Follow camera at near eye level (Mario is ~14 units tall). The
           old version LIFTED the eye onto whatever hill sat behind him,
           which looked down at a grazing angle -- terrain read flat and the
           view stretched. Now: shoulder-height offset, occlusion resolved
           by pulling IN along the view ray, and smoothing so the eye never
           snaps. */
        /* SM64DS-like framing: well back, and LOW -- a nearly level view.
           The old +340 fixed eye height sat above the moat rim and every
           low area's walls, looking steeply down, which foreshortened
           the terrain ("squashed, worse the lower you go"). cam_pitch
           tilts the rig (R/F keys), default ~7 degrees. */
        float cd = 3000.0f * cosf(cam_pitch), ch = 3000.0f * sinf(cam_pitch);
        float want_eye[3] = {px - cd * sinf(cam_yaw), py + 200.0f + ch,
                             pz - cd * cosf(cam_yaw)};
        float at[3] = {px, py + 200.0f, pz};
        if (getenv("SM64DS_ORBIT")) {
            /* debug: whole-stage orbit shot to judge proportions */
            want_eye[0] = 10240.0f; want_eye[1] = 7680.0f;
            want_eye[2] = -10240.0f;
            at[0] = 0.0f; at[1] = 0.0f; at[2] = 0.0f;
        }
        {
            /* occlusion: pull the eye in front of anything between it and
               Mario (cast from the look-at toward the eye) */
            int a[3] = {(int)(at[0] * 4096), (int)(at[1] * 4096),
                        (int)(at[2] * 4096)};
            int b[3] = {(int)(want_eye[0] * 4096), (int)(want_eye[1] * 4096),
                        (int)(want_eye[2] * 4096)};
            int clip[3];
            if (hal_line_ray(g_mc, a, b, clip)) {
                for (int k = 0; k < 3; ++k)
                    want_eye[k] = clip[k] / 4096.0f * 0.9f + at[k] * 0.1f;
                /* never collapse onto Mario: in enclosed areas (the moat,
                   doorways) the pull glued the lens to the nearest wall
                   and the framing went face-cam; keep a minimum standoff
                   even if it means the near wall clips */
                {
                    float dx = want_eye[0] - at[0], dy = want_eye[1] - at[1],
                          dz = want_eye[2] - at[2];
                    float d = sqrtf(dx * dx + dy * dy + dz * dz);
                    const float MIN_D = 1280.0f;
                    if (d > 1.0f && d < MIN_D) {
                        float g2 = MIN_D / d;
                        want_eye[0] = at[0] + dx * g2;
                        want_eye[1] = at[1] + dy * g2;
                        want_eye[2] = at[2] + dz * g2;
                    }
                }
            }
        }
        static float eye[3];
        static int eye_live;
        if (!eye_live) {
            eye_live = 1;
            for (int k = 0; k < 3; ++k) eye[k] = want_eye[k];
        }
        for (int k = 0; k < 3; ++k)
            eye[k] += (want_eye[k] - eye[k]) * 0.2f;
        push_camera(eye, at);
        for (int k = 0; k < 3; ++k) { dbg_eye[k] = eye[k]; dbg_at[k] = at[k]; }
        }   /* else: !real_camera */
        if (selftest && frame == 0)
            fprintf(stderr, "[w] render\n");
        if (selftest && frame == 0 && !real_camera) {
            /* GX isolation: hand-feed one triangle at Mario's position
               through raw MMIO -- no game code. Centered = GX + camera
               fine; offset = my matrix push is wrong. */
            {
                float gp[16];
                ntr::gx_debug_proj(gp);
                fprintf(stderr, "[w] g.proj rows:\n");
                for (int r = 0; r < 4; ++r)
                    fprintf(stderr, "  %8.3f %8.3f %8.3f %8.3f\n",
                            gp[r*4], gp[r*4+1], gp[r*4+2], gp[r*4+3]);
            }
            NTR_MMIO(uint32_t, 0x04000440) = 1;   /* MTX_MODE position */
            uint32_t tr[16] = {4096, 0, 0, 0, 0, 4096, 0, 0,
                               0, 0, 4096, 0,
                               (uint32_t)(int)(sx * 4096),
                               (uint32_t)(int)(sy * 4096),
                               (uint32_t)(int)(sz * 4096), 4096};
            for (int i = 0; i < 16; ++i)
                NTR_MMIO(uint32_t, 0x04000458) = tr[i];
            NTR_MMIO(uint32_t, 0x04000500) = 0;   /* BEGIN_VTXS tris */
            /* small triangle around the origin (4.12: 0x1000 = 1.0) */
            NTR_MMIO(uint32_t, 0x0400048C) = 0x0000F000u;      /* (-1, 0) */
            NTR_MMIO(uint32_t, 0x0400048C) = 0x00000000u;      /* z 0 */
            NTR_MMIO(uint32_t, 0x0400048C) = 0x00001000u;      /* (+1, 0) */
            NTR_MMIO(uint32_t, 0x0400048C) = 0x00000000u;
            NTR_MMIO(uint32_t, 0x0400048C) = 0x10000000u;      /* (0, +1) */
            NTR_MMIO(uint32_t, 0x0400048C) = 0x00000000u;
            NTR_MMIO(uint32_t, 0x04000504) = 0;   /* END_VTXS */
            size_t hn = 0;
            const ntr::GxTriangle *ht = ntr::gx_polygons(hn);
            if (hn)
                fprintf(stderr, "[w] handfed tri screen (%.0f,%.0f) (%.0f,%.0f)"
                        " (%.0f,%.0f)\n",
                        ht[hn-1].v[0].x, ht[hn-1].v[0].y, ht[hn-1].v[1].x,
                        ht[hn-1].v[1].y, ht[hn-1].v[2].x, ht[hn-1].v[2].y);
            else
                fprintf(stderr, "[w] handfed tri CLIPPED/none\n");
            ntr::gx_reset();
            NTR_MMIO(uint32_t, 0x04000580) =
                0u | (0u << 8) | (255u << 16) | (191u << 24);
            ntr::gx_set_light(0, -0.4f, -0.6f, -0.7f, 0x7FFF);
            ntr::gx_enable_lights(0x1);
            push_camera(dbg_eye, dbg_at);
        }
        static int no_level = -1;
        if (no_level < 0) no_level = getenv("SM64DS_NO_LEVEL") ? 1 : 0;
        if (!no_level) {
            if (real_boot) {
                /* Stage::Render's own order: the skybox, then the opaque
                   pass, then the translucent one. The skybox is the +0x9bc
                   Model glued to the camera eye (hal/stage_bridges.cpp); the
                   two model passes are the same Model drawn twice with
                   inverse visibility masks -- the moat water only exists in
                   the second. (ShadowModel::RenderAll sits between them, on
                   the ROM and here.) */
                port_stage_render_skybox(stage);
                /* Stage::Render's first block, in its place in the order:
                   advance the shown areas' BTA texture animations (the
                   waterfall), which RenderModel below then applies. */
                port_stage_advance_anims(stage);
                port_stage_render_model(stage);
                /* ShadowModel::RenderAll, in its place in Stage::Render's own
                   order: after the opaque model pass, before the transparent
                   one. The shadows are drawn onto the ground the pass above
                   just laid down, and the moat water in the pass below goes
                   over them. The ntr side of this call is real now: the two
                   mode-3 material passes RenderAll runs per node stencil the
                   volume against the depth buffer instead of painting it
                   (the wave-4 cone; see the CleanAll block above for the
                   full history).

                   SM64DS_SHADOW_TRIS=1 prints what this call submits, which
                   is the only honest way to see it work: the walk selftest
                   holds its canonical position whether shadows draw or not,
                   so an exit code says nothing about them. The count is read
                   off the geometry engine's own polygon list, the same probe
                   the [actors] and [fx] lines use. */
                {
                    static int sh_tr = -1;
                    if (sh_tr < 0) sh_tr = getenv("SM64DS_SHADOW_TRIS") ? 1 : 0;
                    size_t sh_before = 0, sh_after = 0;
                    if (sh_tr) ntr::gx_polygons(sh_before);
                    ShadowModel::RenderAll();
                    if (sh_tr) {
                        const ntr::GxTriangle *st2 = ntr::gx_polygons(sh_after);
                        float mnx = 1e30f, mxx = -1e30f, mny = 1e30f,
                              mxy = -1e30f;
                        for (size_t i = sh_before; i < sh_after; ++i)
                            for (int v = 0; v < 3; ++v) {
                                if (st2[i].v[v].x < mnx) mnx = st2[i].v[v].x;
                                if (st2[i].v[v].x > mxx) mxx = st2[i].v[v].x;
                                if (st2[i].v[v].y < mny) mny = st2[i].v[v].y;
                                if (st2[i].v[v].y > mxy) mxy = st2[i].v[v].y;
                            }
                        printf("[shadow] frame %d: %zu triangles", frame,
                               sh_after - sh_before);
                        if (sh_after != sh_before)
                            printf(", screen x[%.0f..%.0f] y[%.0f..%.0f]",
                                   mnx, mxx, mny, mxy);
                        printf("\n");
                    }
                }
                port_stage_render_model_transparent(stage);
            } else {
                hal_render_model(level_model, level_shift);
            }
        }
        /* Stage::Render's collision beat, kept in its place in the order:
           after the transparent pass, CylinderClsn::Process consumes the list
           the behaviour phase threaded onto data_0209cee8 -- the overlap
           pushbacks, and the notify chain that hands a grabbable cylinder to
           the Player (slice_gate10, the tree-grab block). */
        if (boot_spawns)
            port_cylinder_clsn_process();
        /* phase 1, which is where func_02044120 ends: the scene tree's own
           housekeeping -- priority re-sorts, parent flag propagation, and the
           deferred list insertions for anything that spawned mid-phase. */
        if (boot_spawns)
            port_actor_scene_pass();
        size_t tris_before = 0;
        if (selftest) ntr::gx_polygons(tris_before);
        /* run mg16 lane MP3: DRAW EVERY PLAYER, not just the local one.
           This line drew data_0209f394[0] and nothing else, which was correct
           while the port asserted there was exactly one player. With a second
           one spawned it is the difference between a player who is there and a
           player who is INVISIBLE: slot 1 collides, is pushed, moves and casts
           no pixels, so a capture of the frame shows one character and the
           owner is asked to judge a picture the second player is missing from.
           The local player keeps drawing last, so nothing about his own
           submission order changes. */
        for (int pi = (int)data_0209f21c - 1; pi >= 0; --pi) {
            if (pi == (int)data_0209f250) continue;   /* drawn below, as before */
            if (void *other = data_0209f394[pi])
                hal_render_player_world(other);
        }
        hal_render_player_world(player);
        /* Stage::GraphCallback1: the particle submission, last, after every
           opaque draw in the frame. The billboards carry their own absolute
           position matrix so nothing above this line has to be preserved for
           them; what they need is to be the last thing the raster sees. */
        static int fx_no_actors = -1;
        if (fx_no_actors < 0)
            fx_no_actors = getenv("SM64DS_NO_ACTORS") ? 1 : 0;
        if (boot_spawns && !fx_no_actors) {
            size_t fx_before = 0, fx_after = 0;
            static int fx_tr = -1;
            if (fx_tr < 0) fx_tr = getenv("SM64DS_FX_TRACE") ? 1 : 0;
            if (fx_tr) ntr::gx_polygons(fx_before);
            port_particle_render();
            if (fx_tr) {
                const ntr::GxTriangle *ft = ntr::gx_polygons(fx_after);
                if (fx_after != fx_before) {
                    int sys = 0, par = 0;
                    port_particle_counts(&sys, &par);
                    float mnx = 1e30f, mxx = -1e30f, mny = 1e30f,
                          mxy = -1e30f, mnz = 1e30f, mxz = -1e30f;
                    for (size_t i = fx_before; i < fx_after; ++i)
                        for (int v = 0; v < 3; ++v) {
                            float X = ft[i].v[v].x, Y = ft[i].v[v].y,
                                  Z = ft[i].v[v].z;
                            if (X < mnx) mnx = X;
                            if (X > mxx) mxx = X;
                            if (Y < mny) mny = Y;
                            if (Y > mxy) mxy = Y;
                            if (Z < mnz) mnz = Z;
                            if (Z > mxz) mxz = Z;
                        }
                    printf("[fx] frame %d: %zu triangles from %d particles in "
                           "%d systems, screen x[%.0f..%.0f] y[%.0f..%.0f] "
                           "z[%.3f..%.3f]\n",
                           frame, fx_after - fx_before, par, sys,
                           mnx, mxx, mny, mxy, mnz, mxz);
                }
            }
        }
        ph_end(PH_SUBMIT, t_phase);
        if (selftest) {
            size_t tn = 0;
            const ntr::GxTriangle *ta2 = ntr::gx_polygons(tn);
            float mnx = 1e30f, mxx = -1e30f, mny = 1e30f, mxy = -1e30f;
            for (size_t i = tris_before; i < tn; ++i)
                for (int v = 0; v < 3; ++v) {
                    if (ta2[i].v[v].x < mnx) mnx = ta2[i].v[v].x;
                    if (ta2[i].v[v].x > mxx) mxx = ta2[i].v[v].x;
                    if (ta2[i].v[v].y < mny) mny = ta2[i].v[v].y;
                    if (ta2[i].v[v].y > mxy) mxy = ta2[i].v[v].y;
                }
            fprintf(stderr,
                    "[w] mario screen box x[%.0f..%.0f] y[%.0f..%.0f] "
                    "(center %d,%d) eye(%.1f,%.1f,%.1f) at(%.1f,%.1f,%.1f)\n",
                    mnx, mxx, mny, mxy, ntr::SCREEN_W / 2, ntr::SCREEN_H / 2,
                    dbg_eye[0], dbg_eye[1], dbg_eye[2], dbg_at[0], dbg_at[1],
                    dbg_at[2]);
        }
        if (selftest && frame == 0)
            fprintf(stderr, "[w] rendered\n");

        ph_begin(&t_phase);
        /* clear: build one row, memcpy the rest (0xFF101820 is not a
           repeating byte pattern, so memset cannot do it directly) */
        for (int x = 0; x < ntr::SCREEN_W; ++x) fb.px[0][x] = 0xFF101820u;
        for (int y = 1; y < ntr::SCREEN_H; ++y)
            memcpy(fb.px[y], fb.px[0], ntr::SCREEN_W * sizeof(fb.px[0][0]));
        ntr::gx_render(fb);
        /* ENGINE-A 2D OVER 3D. The top screen is engine A: its 2D BGs and OBJ
           layer composite over the 3D frame in hardware. The dialogue box lives
           there (BG3 + the cursor OBJ), so raster engine A's 2D and write only
           the covered pixels over the 3D framebuffer. Before the fade composite,
           so the box dims with the master-brightness blend the same as the DS. */
        port_message_composite_engine_a(&fb);
        ph_end(PH_RASTER, t_phase);
        /* Bottom of the DS 2D frame: upload the shadows the game filled,
           rasterise engine B, and drop it into the corner at 1:1 DS pixels.
           With the panel toggled off this writes nothing. Before the overlay,
           so F3 text stays readable over the panel. */
        hal_sub_screen_present(&fb.px[0][0], ntr::SCREEN_W, ntr::SCREEN_H);

        /* THE FADE COMPOSITE. The DS master-brightness blend (MASTER_BRIGHT,
           reached through BLDCNT/BLDY at 0x4000050/0x4000054 for the main
           engine and 0x4001050/0x4001054 for the sub) darkens or brightens the
           WHOLE 2D panel in hardware after the scene is drawn -- BOTH screens,
           which is why this composites after the sub-screen present but before
           the host debug overlay (the overlay is not game content and must stay
           readable through a fade). port_fader_advance wrote those registers
           this frame; read them back and do the same fade over the finished
           framebuffer. EVY is the 0..16 coefficient: fade-to-black is
           rgb*(1 - evy/16), fade-to-white is rgb + (255-rgb)*evy/16, both per
           channel, which is exactly the DS blend math (16/16 = full). */
        {
            int evy = 0, toWhite = 0;
            if (port_fader_blend_state(&evy, &toWhite)) {
                if (evy > 16) evy = 16;
                for (int y = 0; y < ntr::SCREEN_H; ++y) {
                    uint32_t *row = fb.px[y];
                    for (int x = 0; x < ntr::SCREEN_W; ++x) {
                        uint32_t p = row[x];
                        int r = (p >> 16) & 0xff, g = (p >> 8) & 0xff,
                            b = p & 0xff;
                        if (toWhite) {
                            r += ((255 - r) * evy) >> 4;
                            g += ((255 - g) * evy) >> 4;
                            b += ((255 - b) * evy) >> 4;
                        } else {
                            r -= (r * evy) >> 4;
                            g -= (g * evy) >> 4;
                            b -= (b * evy) >> 4;
                        }
                        row[x] = 0xFF000000u | ((uint32_t)r << 16) |
                                 ((uint32_t)g << 8) | (uint32_t)b;
                    }
                }
            }
        }

        /* THE STACKED IMAGE IS BUILT HERE, ahead of the overlays, and that is
           this lane's one change to the order. Every line ABOVE this one that
           writes a pixel writes it into fb -- the raster, the engine-A
           composite, the fade -- so the compose still reads a finished frame
           and composes exactly the picture it composed before.

           WHAT MOVED, AND WHY. The F3 overlay, the F5 menu and the save-state
           toast used to be painted into fb and copied up with it. fb is ENGINE
           A's framebuffer, and since ppu_compose_stacked started honouring
           POWCNT1 bit 15 engine A is not always the top screen: the four
           dScMgD3DBase_c scenes toggle the bit every frame and Snowball Slalom
           clears it for the whole minigame, so the overlays flashed between the
           halves on one and sat on the bottom screen on the other. They are
           host UI, they belong to a SCREEN, and they now paint on the composed
           image's upper half -- which is the upper physical LCD whichever
           engine is feeding it.

           THE INSET LAYOUT IS UNCHANGED: `stacked` is 0, the surface is fb, and
           every line below is the line that was here. THE SELFTEST DUMP IS
           UNCHANGED TOO on every run that has an overlay to lose, because it
           has none: no tool in port/tools sets SM64DS_OVERLAY or SM64DS_MENU,
           and the toast needs a save-state keypress. What a stacked run with
           SM64DS_OVERLAY=1 loses is the overlay inside the 512x384 fb BMP; it
           is in the STACKED capture instead, which is the picture that run is
           actually presenting. */
        uint32_t *stack_img = stacked
                ? hal_sub_screen_stacked_image(&fb.px[0][0]) : 0;
        const OvlSurface surf =
            stacked ? ovl_surface_stacked(stack_img, fb) : ovl_surface(fb);

        /* THE OVERLAY GOES HERE: after the raster owns the frame and before
           the blit hands it to GDI, so it is in the pixels rather than over
           the window, and the selftest BMP carries it. */
        if (g_overlay_on) {
            OvlStats os;
            size_t tn = 0;
            int actors = 0;
            ntr::gx_polygons(tn);
            for (int *node = (int *)(size_t)data_020a4b78[0];
                 node && actors < 4096; node = (int *)(size_t)node[1])
                if (node[2]) ++actors;
            if (W.GetProcessMemoryInfo_ && (frame % 30) == 0) {
                PortMemCounters pmc;
                pmc.cb = sizeof pmc;
                if (W.GetProcessMemoryInfo_(GetCurrentProcess(), &pmc,
                                            sizeof pmc))
                    ovl_mem_kb = (unsigned)(pmc.WorkingSetSize / 1024);
            }
            os.fps = ovl_fps;
            os.tps = ovl_tps;
            os.tris = (int)tn;
            os.actors = actors;
            os.player = c;
            os.cam_name = cam_mode_name(cam_mode);
            os.mem_kb = ovl_mem_kb;
            os.menu_paused = !game_ticked;
            ovl_draw(surf, os);
        }
        if (menu_on) menu_draw(surf);
        /* the save-state toast, over everything, bottom-left; at file scope
           now so the windowed scene loop can show the menu's refusals too */
        toast_draw(surf);

        if (stacked)
            stack_present_arm(stack_img, hwnd);

        ph_begin(&t_phase);
        present();
        ph_end(PH_BLIT, t_phase);
        ph_end(PH_FRAME, t_frame);
        /* present-to-present rate, and the GAME TICK rate beside it -- the two
           diverge whenever a tick is skipped, which is what the debug menu's
           pause does. Both smoothed the same way the phase times are. */
        {
            const double now = ovl_now_ms();
            if (ovl_last_present > 0.0) {
                const double dt = now - ovl_last_present;
                if (dt > 0.01) {
                    const double inst = 1000.0 / dt;
                    ovl_fps += (inst - ovl_fps) * 0.1;
                    ovl_tps += ((game_ticked ? inst : 0.0) - ovl_tps) * 0.1;
                }
            }
            ovl_last_present = now;
        }
        /* the click flag is true for exactly the frame it landed on; the hold
           in g_mouse_left_down is what outlives it */
        g_mouse_click_new = 0;
        /* SM64DS_JUMP_PROBE=1: the per-frame cost line the jump-hitch
           investigation runs on. PH_FRAME's RAW time (not the smoothed one
           the overlay draws), the file loads this frame and what they cost,
           and the Player word that says whether the frame changed animation
           at all: +0x63c is SetAnim's cached (id << 2). A hitch attributable
           to the animation load has to show its milliseconds on the frame
           +0x63c changes. */
        if (selftest && getenv("SM64DS_JUMP_PROBE")) {
            static unsigned long pl, pm, pb;
            static double pms;
            static unsigned prev_anim = 0xffffffffu;
            const unsigned anim = *(unsigned *)(c + 0x63c);
            /* the ModelAnim the animation actually landed on: Animation sits
               at +0x50 of it (numFramesAndFlags 0x54, currFrame 0x58, speed
               0x5c) and the BCA file pointer at 0x60 */
            char *ma = ((char **)(c + 0xdc))
                       [_ZNK6Player14GetBodyModelIDEjb(c, *(unsigned *)(c + 8)
                                                          & 0xff, 0)];
            printf("[jp] f%-4d frame=%6.3f in=%5.2f cam=%5.2f sub=%5.2f "
                   "ras=%5.2f blit=%5.2f loads=%lu miss=%lu "
                   "bytes=%lu fs=%6.3f anim=%u%s "
                   "cf=%8.3f nff=%08x spd=%d file=%p y=%d\n",
                   frame, g_clk.raw[PH_FRAME], g_clk.raw[PH_INPUT],
                   g_clk.raw[PH_CAMERA], g_clk.raw[PH_SUBMIT],
                   g_clk.raw[PH_RASTER], g_clk.raw[PH_BLIT],
                   port_fs_loads - pl,
                   port_fs_load_miss - pm, port_fs_bytes - pb,
                   port_fs_ms - pms, anim >> 2,
                   anim != prev_anim ? " CHANGED" : "",
                   ma ? *(int *)(ma + 0x58) / 4096.0 : 0.0,
                   ma ? *(unsigned *)(ma + 0x54) : 0u,
                   ma ? *(int *)(ma + 0x5c) : 0,
                   ma ? *(void **)(ma + 0x60) : (void *)0,
                   *(int *)(c + 0x60) >> 12);
            pl = port_fs_loads; pm = port_fs_load_miss; pb = port_fs_bytes;
            pms = port_fs_ms;   prev_anim = anim;
        }
        /* SM64DS_BONE_PROBE=1: the per-frame BONE ROTATION dump the long-jump
           leg-twist investigation runs on, and its detector.

           The animated bone records are ModelComponents::bones -- Model+0x08
           is the components block and its +0x08 is the array, stride 0x34,
           the same reading func_020453c0 walks it with. The three rotation
           components sit at +0x1a/+0x1c/+0x1e as the 12-bit binary angle the
           keyframe interpolator produced, shifted up four (func_0204547c's
           `<< 4`), so a half turn is 0x8000 and the useful unit is the
           12-bit one.

           A limb that twists reads as a component moving nearly half a turn
           between two frames. Real animation never does that, so the probe
           prints, per frame, the largest SHORTEST-PATH frame-to-frame delta
           across every bone and names the bone and axis it belongs to. =1
           prints only the maximum; =2 prints every bone every frame; =3 adds
           the reference check below.

           WHAT IT MEASURED (2026-08-05): the long-jump leg twist is NOT the
           rotation interpolation going the long way around. =3 recomputes
           every bone's rotation from the raw keyframes with an unambiguous
           shortest-path lerp written out longhand and compares it against
           what func_020456a0 produced. Across five movement probes
           (LONGJUMP, JUMPSPAM, DASHJUMP, TURN, IDLE) at 299 frames each,
           48 components a frame:

               71,760 checks, 0 mismatches

           Every wrap case the long jump hits is handled correctly. The
           flight crosses the |lo-hi| >= 0x800 threshold five times -- bone 9
           axis 0 at frames 63, 68 and 82, bone 12 axis 0 at frame 66 -- and
           each one comes back the short way, e.g. lo=826 hi=-1288 is a direct
           2114 and an interpolated 1982. The trig table func_02045178 and
           func_02048234 index is the complete 4096-entry one, and both
           matrix builders are matched source in gate 4b.

           So whatever the twist is, it is downstream of the angles. This
           probe rules out the interpolation and nothing else. */
        if (selftest && getenv("SM64DS_BONE_PROBE")) {
            static short prev[128][3];
            static int seeded;
            const int verbose = atoi(getenv("SM64DS_BONE_PROBE")) >= 2;
            char *ma = ((char **)(c + 0xdc))
                       [_ZNK6Player14GetBodyModelIDEjb(c, *(unsigned *)(c + 8)
                                                          & 0xff, 0)];
            char *bones = ma ? *(char **)(ma + 0x08 + 0x08) : 0;
            char *bmd = ma ? *(char **)(ma + 0x08) : 0;
            int nb = bmd ? *(int *)(bmd + 4) : 0;
            if (nb > 128) nb = 128;
            if (bones && nb > 0) {
                int worst = 0, wb = -1, wa = -1, cur[3];
                for (int b = 0; b < nb; ++b) {
                    for (int a = 0; a < 3; ++a) {
                        /* back down to the 12-bit angle the tables hold */
                        cur[a] = (*(unsigned short *)(bones + b * 0x34 +
                                                      0x1a + a * 2)) >> 4;
                        int d = cur[a] - prev[b][a];
                        if (d >= 0x800) d -= 0x1000;   /* shortest path */
                        if (d < -0x800) d += 0x1000;
                        if (seeded && (d > worst || -d > worst)) {
                            worst = d < 0 ? -d : d;
                            wb = b; wa = a;
                        }
                        prev[b][a] = (short)cur[a];
                    }
                    if (verbose)
                        printf("[bone] f%-4d b%-3d rot=(%4d,%4d,%4d)\n",
                               frame, b, cur[0], cur[1], cur[2]);
                }
                if (seeded)
                    printf("[bone] f%-4d nb=%d maxdelta=%4d bone=%d axis=%d "
                           "st=%08x\n", frame, nb, worst, wb, wa,
                           *(void **)(c + 0x370)
                               ? **(unsigned **)(c + 0x370) : 0u);
                /* =3 also prints the two KEYFRAMES the rotation interpolator
                   is between for the worst bone this frame, which is what
                   says whether a big step is the data or the interpolation.
                   BCA layout: +0x02 frame count, +0x0c the rotation table,
                   +0x14 the per-bone descriptor array at 0x24 stride, whose
                   rotation triple is (shift, flag, table index) at +0x0c,
                   +0x10 and +0x14. func_020456a0 wraps when |lo-hi| >= 0x800,
                   so a step pair whose implied |lo-hi| crosses that and did
                   NOT come back short is the bug. */
                if (seeded && atoi(getenv("SM64DS_BONE_PROBE")) >= 3) {
                    char *bca = *(char **)(ma + 0x60);
                    short *rot = bca ? *(short **)(bca + 0x0c) : 0;
                    char *sb = bca ? *(char **)(bca + 0x14) : 0;
                    int v = bca ? *(unsigned short *)(bca + 2) : 0;
                    int idx = (int)((unsigned)(*(int *)(ma + 0x58) << 4) >> 16);
                    int bad = 0, wraps = 0;
                    for (int b = 0; sb && rot && b < nb; ++b) {
                        for (int a = 0; a < 3; ++a) {
                            char *src = sb + b * 0x24;
                            int sh = (unsigned char)src[0x0c + a * 4];
                            int fl = (unsigned char)src[0x0d + a * 4];
                            int off = *(unsigned short *)(src + 0x0e + a * 4);
                            const short *t = rot + off;
                            int want;
                            /* the reference: the same table walk, with an
                               unambiguous shortest-path lerp written out */
                            if (fl == 0)            want = (unsigned short)t[0];
                            else if (sh == 0)       want = (unsigned short)t[idx];
                            else {
                                int base = ((v - 1) >> sh) << sh;
                                int i = idx >> sh;
                                if (idx >= base)
                                    want = (unsigned short)t[i + (idx - base)];
                                else {
                                    int frac = idx - (i << sh);
                                    int lo = t[i] & 0xfff, hi = t[i + 1] & 0xfff;
                                    int d = hi - lo;
                                    if (d > 0x800)  d -= 0x1000;
                                    if (d < -0x800) d += 0x1000;
                                    if (d >= 0x800 || d <= -0x800) ++wraps;
                                    want = frac == 0 ? lo
                                         : lo + (d * frac >> sh);
                                }
                            }
                            want &= 0xfff;
                            int got = (*(unsigned short *)(bones + b * 0x34 +
                                                           0x1a + a * 2)) >> 4;
                            int e = got - want;
                            if (e > 0x800)  e -= 0x1000;
                            if (e < -0x800) e += 0x1000;
                            /* one unit of slack: the ROM rounds its lerp by
                               truncation on the product, the reference by
                               truncation on the delta */
                            if (e > 1 || e < -1) {
                                ++bad;
                                printf("[bkf] f%-4d b%d ax%d shift=%d idx=%d "
                                       "lo=%d hi=%d want=%d got=%d err=%d\n",
                                       frame, b, a, sh, idx,
                                       (int)t[idx >> sh] & 0xfff,
                                       (int)t[(idx >> sh) + 1] & 0xfff,
                                       want, got, e);
                            }
                        }
                    }
                    printf("[bref] f%-4d checked=%d mismatched=%d\n",
                           frame, nb * 3, bad);
                }
                seeded = 1;
            }
        }
        if (selftest && (frame % 10) == 0)
            printf("[y] frame %d y=%d units %.1f\n", frame,
                   *(int *)(c + 0x60), *(int *)(c + 0x60) / 4096.0f);
        /* SM64DS_DUMP_FROM/TO: per-frame BMPs across a window, for
           watching an animation play (or fail to) */
        {
            static int dump_from = -1, dump_to = -1, dump_env;
            if (!dump_env) {
                dump_env = 1;
                const char *df = getenv("SM64DS_DUMP_FROM");
                const char *dt = getenv("SM64DS_DUMP_TO");
                if (df) dump_from = atoi(df);
                if (dt) dump_to = atoi(dt);
            }
            if (selftest && dump_from >= 0 && frame >= dump_from &&
                frame <= dump_to) {
                char nm[64];
                snprintf(nm, sizeof nm, "walk_frame_%03d.bmp", frame);
                ntr::ppu_write_bmp(nm, fb);
            }
        }
        sdat_host_tick();   /* hosted ARM7: drain the sound queue, feed the mixer */
        /* THE FRAME BOUNDARY. Everything this frame -- tick, render, present --
           is done, and nothing of the next frame has started, so an editor's
           object move or staged warp lands on a world that is not half-updated.
           A no-op when the channel is not armed. */
        editor_channel_drain();
        ++frame;   /* counts in live mode too -- the [cam-in]-style live
                      diagnostics carry a real frame number */
        port_last_frame = frame;   /* fault_probe.h: crash.txt/exit.txt context */
        /* the frame's stdout, one write; the setvbuf note above is why */
        fflush(stdout);
        if (selftest && frame >= selftest) {
            for (int k = 0; k < g_amb_n; ++k) {
                char *o = (char *)g_amb[k].o;
                int moved = *(int *)(o + 0x5c) != g_amb[k].p0[0] ||
                            *(int *)(o + 0x60) != g_amb[k].p0[1] ||
                            *(int *)(o + 0x64) != g_amb[k].p0[2];
                fprintf(stderr,
                        "[amb-min] id=%u pos=(%d,%d,%d) minr=%d thresh=%d "
                        "at f%d %s moved=%d\n",
                        g_amb[k].id, g_amb[k].p0[0] >> 12,
                        g_amb[k].p0[1] >> 12, g_amb[k].p0[2] >> 12,
                        g_amb[k].minr, g_amb[k].thresh, g_amb[k].minfr,
                        g_amb[k].minr <= g_amb[k].thresh ? "IN-RANGE"
                                                         : "never-in-range",
                        moved);
            }
            /* the census counters are cumulative, so a second print here
               shows anything the run itself spawned (a woken butterfly
               spawner adds four) on top of the boot's numbers */
            if (boot_spawns)
                port_actor_census();
            port_bob_spawn_report();
            /* THE IMAGE LAYOUT THIS BMP WAS PRODUCED AT. The hosted-DS span is
               the linker's .dsstate section (hal/dsstate_seg.h), and its base
               moves whenever anything placed before it in the image changes
               size. A selftest BMP that differs between two builds is only
               comparable against a run whose span matches, so the span is part
               of the run's identity and is printed beside the dump. */
            fprintf(stderr, "[layout] dsstate=%p..%p\n",
                    (void *)&dsstate_lo, (void *)&dsstate_hi);
            ntr::ppu_write_bmp("walk_window_selftest.bmp", fb);
            /* the other half of the boot's [heap] line: what the run itself
               spent out of the ROM's 0x3b000.

               READ IT CAREFULLY -- the obvious reading is wrong, and the port
               shipped on the wrong reading for four waves. A number that keeps
               falling across frame counts is a leak, yes. But a number that
               STOPS falling is not automatically the game living inside its
               budget: it is equally the shape of frees that never come back to
               THIS allocator. Wave 6 measured exactly that. The castle boot sat
               flat at 182600 free while 31 actors died on the opening frames,
               and flat was the symptom -- every one of those frees was being
               linked into the DEFAULT heap's free list by a dropped argument in
               hal/cxxname_bridge.cpp (see the Memory_Deallocate banner there).
               With the argument restored the same boot reads 25212 bytes
               higher and the free node grows on every death.

               So: falling means a leak, flat means EITHER a steady state OR a
               leak into somewhere else, and only a free-list walk tells the two
               apart. w2c.md's "flat at 182600, no leak" is the reading this
               paragraph exists to retire. */
            if (data_020a0eac_c)
                fprintf(stderr, "[heap] %u free after %d frames\n",
                        _ZN22ExpandingHeapAllocator10MemoryLeftEv(
                            *(void **)((char *)data_020a0eac_c + 0x14)),
                        frame);
            printf("selftest: %d frames, pos=(%d, %d, %d)\n", frame,
                   *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
            return 0;
        }
        /* THE PACE. Stage::InitResources writes data_0208ee44 = 2 for a 3D
           level, so frame_pace's budget here is the same 33.3ms this block
           used to hardcode -- the "jump too fast, weird gravity" report stays
           fixed and the number it was fixed with is unchanged. What moves is
           that the constant is now the game's own word rather than the host's
           guess, which is what the scene loop needed. A selftest stays
           UNPACED: it is the deterministic comparator run and a sleep in it
           only makes the battery slower. */
        if (!selftest) frame_pace();
    }
}
