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
//   F5  the debug menu: warp to any of the level's own entrances, the fake-
//   snap A/B, the overlay, the camera mode, and the recorder's filename.
//   Arrows or the d-pad move, enter or A acts. It PAUSES THE GAME TICK while
//   it is open and keeps rendering, so the scene freezes and the view does not.
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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* user32/gdi32 are loaded DYNAMICALLY after io_init: a static import chain
   initializes the desktop heap before main, and on 32-bit that mapping can
   land inside the fixed DS regions (0x04000000..0x07ffffff), killing
   io_init deterministically. Resolving late keeps the address space ours
   first. */
struct WinApi {
    ATOM(WINAPI *RegisterClassA_)(const WNDCLASSA *);
    HWND(WINAPI *CreateWindowExA_)(DWORD, LPCSTR, LPCSTR, DWORD, int, int,
                                   int, int, HWND, HMENU, HINSTANCE, LPVOID);
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
    HWND(WINAPI *SetCapture_)(HWND);
    BOOL(WINAPI *ReleaseCapture_)(void);
    BOOL(WINAPI *GetCursorPos_)(POINT *);
    BOOL(WINAPI *SetCursorPos_)(int, int);
    int(WINAPI *ShowCursor_)(BOOL);
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
    W.SetCapture_ = (decltype(W.SetCapture_))GetProcAddress(u, "SetCapture");
    W.ReleaseCapture_ = (decltype(W.ReleaseCapture_))GetProcAddress(u, "ReleaseCapture");
    W.GetCursorPos_ = (decltype(W.GetCursorPos_))GetProcAddress(u, "GetCursorPos");
    W.SetCursorPos_ = (decltype(W.SetCursorPos_))GetProcAddress(u, "SetCursorPos");
    W.ShowCursor_ = (decltype(W.ShowCursor_))GetProcAddress(u, "ShowCursor");
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

#include "fault_probe.h"
#include "overlay_font.h"

typedef unsigned int u32;

extern "C" {
void *_ZN6PlayerC1Ev(void *self);
void *_ZN4Heap13SetupRootHeapEv(void);
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
unsigned int _ZNK6Player14GetBodyModelIDEjb(char *, unsigned int, char);
extern int data_0209f498[];    /* CheckInput's own Ctrl[4] block */
extern int data_0209f4a2[];    /* split: stick nx */
extern int data_0209f4a4[];    /* split: stick ny */
extern unsigned char data_0209f4ac[]; /* split: touching */
extern int data_020a0e58[];    /* PadData[4]: u16 held, u16 pressed */
extern int data_020a0de8[];    /* TouchData[4], zero = no touch */
extern unsigned char data_0209f21c;   /* controller count */
extern int data_0209f350[];    /* per-pad status */
extern int data_020a1164[];    /* camera per-player block; +0 = angle
                                  (GetAngleToCamera reads it) */
extern int data_0209caa0[];
extern unsigned char data_0209d660;
extern int data_0209fc48;
extern unsigned char data_0209f2d8;
extern int data_0209214c[];    /* button remap pointer table (ROM DS
                                  pointers -- repointed at staging) */
extern char data_0209f49c[];   /* held buttons (bit 1 = A/jump held) */
extern char data_0209f49e[];   /* pressed-this-frame (bit 1 = jump) */
extern int data_0209b468[4];   /* actor list head (stomp tracker) */
extern unsigned char data_020a0e40[];
extern short data_02092144[];
extern unsigned char data_ov002_0211049c[];  /* St_Wait state object */
/* SM64DS_FORCE_STATE=walljump probe: the ST_WALL_JUMP State record and the
   per-character airborne-gravity PMF table St_WallJump_Main dispatches
   through. Both are BSS built by __sinit_ov002_021019d0, so they only read
   back after the sinit run below. */
extern unsigned char data_ov002_021103dc[];  /* _ZN6Player12ST_WALL_JUMPE */
extern unsigned char data_ov002_021106dc[];  /* _ZN6Player8ST_CLIMBE */
extern int data_ov002_02110a48[5];           /* Tree's five cylinder lists */
extern int data_ov002_0211073c[];            /* 4 rows of {fn-or-vtoff, v} */
int _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
void port_ov002_patch(void);
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
void port_ov009_probe(void);
void *port_stage_a_boot(void *mc, int spawn_entrances);
void port_stage_a_probe(void *mc);
void *port_stage_create(void);   /* hal/stage_bridges.cpp: the real Stage actor */
void port_stage_tree_probe(void *child, const char *what);
void port_stage_render_model(void *self);  /* Stage::RenderModel, matched src */
void port_stage_render_model_transparent(void *self);
void port_stage_render_skybox(void *self); /* the +0x9bc Model, camera-glued */
void _ZN5Stage9LoadModelEv(char *self);   /* matched src, slice_gate24 */
void _ZN5Stage10LoadSkyboxEv(char *self); /* matched src, slice_gate26 */
unsigned _ZN5Stage11GetSkyboxIDEv(void);  /* the LVL_Overlay's skybox bits */
extern int data_0209f320;                 /* the Stage's ModelComponents */
int port_stage_path_guard(void *player);
void port_stage_a2_seat(void);
/* the actor registry and the ROM's own processing lists (hal/actor_registry) */
void port_actor_tick(void);          /* phases 4/2/3: cleanup, init, behaviour */
void port_actor_render(void);        /* phase 5: the render bucket */
void port_actor_scene_pass(void);    /* phase 1: scene-tree housekeeping */
void port_actor_census(void);
void port_actor_lists_probe(void);
/* the bottom screen (hal/sub_screen.cpp): the OAM lifecycle, the engine-B
   scan-out and the corner panel it lands in. TAB toggles the panel. */
void hal_sub_screen_init(void *hwnd, int zoom);
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

#ifdef NTR_HIRES
static const int ZOOM = 1;
#elif defined(NTR_HIRES2)
static const int ZOOM = 2;
#else
static const int ZOOM = 3;
#endif
static void *g_mc;

/* ---- THE FRAME PACER'S CLOCK ------------------------------------------
   The loop below sleeps out the remainder of a 33.3ms budget. Sleep's
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
   with it on. */
static const int OVL_SCALE = ntr::SCREEN_W >= 1024 ? 2 : 1;
static const int OVL_LINE = (OVL_GLYPH_H + 2) * OVL_SCALE;

static void ovl_shade(ntr::Framebuffer &fb, int x0, int y0, int w, int h)
{
    /* half-strength darken of what is already there, so the text reads over
       sky and over stone without hiding the frame behind it */
    for (int y = y0; y < y0 + h; ++y) {
        if (y < 0 || y >= ntr::SCREEN_H) continue;
        for (int x = x0; x < x0 + w; ++x) {
            if (x < 0 || x >= ntr::SCREEN_W) continue;
            const uint32_t p = fb.px[y][x];
            fb.px[y][x] = 0xFF000000u | ((p >> 1) & 0x007F7F7Fu);
        }
    }
}

static int ovl_text(ntr::Framebuffer &fb, int x0, int y0, const char *s,
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
                        fb.px[fy][fx] = rgb;
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

static void ovl_draw(ntr::Framebuffer &fb, const OvlStats &s)
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
   move the picture */
static void fc_seed(void *cam)
{
    char *k = (char *)cam;
    int *at = (int *)(k + 0x80);
    int *eye = (int *)(k + 0x8c);
    int d[3] = {eye[0] - at[0], eye[1] - at[1], eye[2] - at[2]};
    fc_yaw = *(short *)(k + 0x17c);
    fc_pitch = Vec3_VertAngle(eye, at);
    fc_dist = LenVec3(d);
    if (fc_dist < 0x40000) fc_dist = 0x40000;
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
   F5. Deliberately small: the four things worth reaching mid-session without
   restarting the program under a different environment, plus one status line.
   Up/down or the d-pad move, left/right change a value, and enter (or A) does
   what right does, so the whole thing works one-handed on a pad.

   IT PAUSES THE GAME TICK while it is open -- port_actor_tick is skipped and
   the input the harness writes is zeroed, so nothing moves, nothing spawns and
   nothing decides anything while a person is reading. Rendering carries on, so
   the frame stays live behind the menu and the F3 overlay's fps keeps counting
   while its TICK rate falls to zero. That divergence is what the two numbers
   are next to each other for. */
enum {
    MENU_WARP = 0,
    MENU_SNAP,
    MENU_OVERLAY,
    MENU_CAMERA,
    MENU_RECORDER,
    MENU_COUNT
};
static int menu_on;
static int menu_sel;
static int menu_entrance;             /* the entrance the warp row is showing */
static int g_overlay_on;              /* F3, and the menu's overlay row */
static char g_playlog[160] = "off";   /* the flight recorder's current file */

/* The harness ground snap and wall clamp, a boot-time const off
   SM64DS_FAKE_SNAP until now, so the A/B was a restart. It is a switch.
   What CANNOT move at runtime is the third thing that env chose: the level
   collider's OWNER is decided once, before the Player exists (the
   MeshColliderBase::Enable call in main), and an owner of the Player makes
   every player probe skip the level. The row says so rather than pretending
   the toggle is the whole switch. */
static int g_fake_snap;

static void menu_draw(ntr::Framebuffer &fb)
{
    char ln[MENU_COUNT][72];
    int i, w = 0, x0, y0;
    int ex = 0, ey = 0, ez = 0, eyaw = 0;
    const int n_ent = port_entrance_count();
    const int have = port_entrance_record(menu_entrance, &ex, &ey, &ez, &eyaw);
    const char *title = "DEBUG MENU   F5 close   arrows move   enter/right act";

    if (have)
        snprintf(ln[MENU_WARP], sizeof ln[0],
                 "warp to entrance  %d of %d   (%d %d %d)", menu_entrance,
                 n_ent, ex, ey, ez);
    else
        snprintf(ln[MENU_WARP], sizeof ln[0],
                 "warp to entrance  none loaded");
    snprintf(ln[MENU_SNAP], sizeof ln[0], "fake snap         %s",
             g_fake_snap ? "ON (collider owner set at boot)" : "off");
    snprintf(ln[MENU_OVERLAY], sizeof ln[0], "stats overlay     %s",
             g_overlay_on ? "on" : "off");
    snprintf(ln[MENU_CAMERA], sizeof ln[0], "camera            %s",
             cam_mode_name(cam_mode));
    snprintf(ln[MENU_RECORDER], sizeof ln[0], "recorder          %s", g_playlog);

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
   camera is the game's and the mouse does not touch it. */
static int mo_look;              /* right button down */
static POINT mo_anchor;          /* screen point the drag springs back to */
static int mo_dx, mo_dy;         /* accumulated since the loop last drained */
static int mo_wheel;             /* accumulated notches, forward positive */

/* THE TOUCH BRIDGE'S HANDOFF. The DS has a touchscreen and this program has a
   mouse, and the last left click is where the two meet. Position is in
   FRAMEBUFFER pixels -- client coordinates divided by ZOOM -- so a consumer
   gets the same numbers at either tier without knowing which one it is on.
   `g_mouse_click_new` is true for exactly the frame the click landed on and
   `g_mouse_left_down` is the hold, which is what a drag on a touchscreen is.
   NOTHING IN THIS FILE READS ANY OF IT: it is published for the touch bridge
   a sibling stream is building. */
int g_mouse_click_x, g_mouse_click_y;
int g_mouse_click_new;
int g_mouse_left_down;

static void mo_release(void)
{
    if (!mo_look) return;
    mo_look = 0;
    if (W.ReleaseCapture_) W.ReleaseCapture_();
    if (W.ShowCursor_) while (W.ShowCursor_(TRUE) < 0) {}
}

static LRESULT CALLBACK wndproc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if (m == WM_DESTROY) { mo_release(); W.PostQuitMessage_(0); return 0; }
    if (m == WM_KEYDOWN && w == VK_ESCAPE) { mo_release(); W.PostQuitMessage_(0); return 0; }
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
    case WM_CAPTURECHANGED:
        mo_release();
        return 0;
    case WM_MOUSEMOVE:
        if (mo_look && W.GetCursorPos_) {
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
        int cx = (short)LOWORD(l) / ZOOM, cy = (short)HIWORD(l) / ZOOM;
        if (cx < 0) cx = 0;
        if (cy < 0) cy = 0;
        if (cx >= ntr::SCREEN_W) cx = ntr::SCREEN_W - 1;
        if (cy >= ntr::SCREEN_H) cy = ntr::SCREEN_H - 1;
        g_mouse_click_x = cx;
        g_mouse_click_y = cy;
        g_mouse_click_new = 1;
        g_mouse_left_down = 1;
        fprintf(stderr, "[mouse] click %d,%d fb\n", cx, cy);
        return 0;
    }
    case WM_LBUTTONUP:
        g_mouse_left_down = 0;
        return 0;
    case WM_MOUSEWHEEL:
        mo_wheel += (short)HIWORD(w) / WHEEL_DELTA;
        return 0;
    default:
        break;
    }
    return W.DefWindowProcA_(h, m, w, l);
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

int main(void)
{
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
    setvbuf(stdout, NULL, _IONBF, 0);
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
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    if (!winapi_load()) { fprintf(stderr, "winapi_load failed\n"); return 2; }
    pacer_begin();
    if (!_ZN4Heap13SetupRootHeapEv()) return 2;
    memset(data_0209b3ec, 0, 48);
    data_0209b3ec[0] = data_0209b3ec[4] = data_0209b3ec[8] = 0x1000;
    hal_fill_model_vtable();
    hal_fill_shadow_vtable();
    hal_fill_mmc_vtable();
    hal_fill_modelanim2_vtable();

    port_ov002_patch();
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

    /* THE GAME'S OWN LEVEL BOOT, now the default: ov009 mounted,
       Stage::LoadClsnAndObjects run against it, and the level's own entrance
       record spawning the Player and the Camera. SM64DS_LEGACY_BOOT=1 goes
       back to the harness staging (hand-built spawn context, KCL mounted by
       hand); SM64DS_BOOT_NOSPAWN=1 holds the entrance table off, which is
       stage A1, the same boot with nothing spawning. SM64DS_REAL_BOOT is
       still accepted and is now a no-op. */
    const int real_boot = getenv("SM64DS_LEGACY_BOOT") == 0;
    const int boot_spawns = real_boot && getenv("SM64DS_BOOT_NOSPAWN") == 0;
    if (real_boot)
        port_ov009_probe();

    data_02092144[0] = 8 << 8;
    if (!boot_spawns) {
        /* the fake spawn context: actor id 0 with invented priorities, which
           is what the hand-built Player was constructed under. The real boot
           reads the ROM's own SpawnInfo out of the registry instead. */
        data_020a4b54 = 0;
        static unsigned short spawn_info[4] = {0, 0, 100, 100};
        data_020a4bb8[0] = spawn_info;
    }
    data_020a0eac_c = data_020a0ea0;

    /* Game mode 0 (adventure) -- LoadClsnAndObjects branches its minimap
       and HUD spawns on this, and Stage::CheckInput reads it later. */
    data_0209f2d8 = 0;

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
        void *lvl = port_stage_a_boot(g_mc, boot_spawns);
        level_bmd = *(unsigned short *)((char *)lvl + 8);
        port_stage_a_probe(g_mc);
        if (boot_spawns) {
            port_stage_tree_probe(data_0209f394[0], "PLAYER");
            port_actor_census();
            port_actor_lists_probe();
        }
    }

    void *player;
    if (boot_spawns) {
        /* THE ENTRANCE SPAWNED HIM. data_0209f394[0] is where
           LoadEntranceObjects parked the actor it made from entrance record
           0 -- position, rotation, area and entrance type all the level's
           own, and Player::InitResources already run through the spawn
           spine's init Process. */
        player = data_0209f394[0];
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
        _ZN5Stage9LoadModelEv(stage);
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
        static char rg[0x50];
        int pos[3] = {*(int *)(c + 0x5c), *(int *)(c + 0x60),
                      *(int *)(c + 0x64)};
        _ZN13RaycastGroundC1Ev(rg);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, pos, player);
        rg[4] |= 1;   /* BgCh collide-ordinary (the gate-8 predicate bit) */
        *(int *)(rg + 0x4c) = 0x100000;   /* reach: 256 units down */
        int hit = _ZN13RaycastGround10DetectClsnEv(rg);
        printf("ground probe at spawn: hit=%d ground_y=%d (%.1f units)\n",
               hit, *(int *)(rg + 0x3c), *(int *)(rg + 0x3c) / 4096.0f);
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
       drives the stick fields) */
    data_0209caa0[2] |= 0x80;
    data_0209d660 = 0;
    data_0209fc48 = 0;
    data_0209f21c = 1;
    data_0209f350[0] = 0;
    /* the ROM's button-remap tables are DS pointers (0x0207xxxx) the
       host has no image behind; buttons are written directly to the
       Ctrl fields anyway, so give CheckInput's remap loop zeros */
    {
        static unsigned short zero_btn_map[32];
        for (int i = 0; i < 4; ++i)
            ((unsigned short **)data_0209214c)[i] = zero_btn_map;
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

    /* window */
    WNDCLASSA wc = {};
    wc.lpfnWndProc = wndproc;
    wc.hInstance = GetModuleHandleA(0);
    wc.hCursor = W.LoadCursorA_(0, (LPCSTR)IDC_ARROW);
    wc.lpszClassName = "sm64ds_walk";
    W.RegisterClassA_(&wc);
    RECT r = {0, 0, ntr::SCREEN_W * ZOOM, ntr::SCREEN_H * ZOOM};
    W.AdjustWindowRect_(&r, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE);
    HWND hwnd = W.CreateWindowExA_(0, "sm64ds_walk",
                              "SM64DS port -- WASD walk, ESC quit",
                              (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME) |
                                  WS_VISIBLE,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              r.right - r.left, r.bottom - r.top, 0, 0,
                              wc.hInstance, 0);
    HDC hdc = W.GetDC_(hwnd);
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof bi.bmiHeader;
    bi.bmiHeader.biWidth = ntr::SCREEN_W;
    bi.bmiHeader.biHeight = -ntr::SCREEN_H;   /* top-down */
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    /* SM64DS_WINDOW_SELFTEST=N: run N frames with W held, dump the last
       framebuffer next to the exe, exit -- CI-checkable without a user */
    const char *st = getenv("SM64DS_WINDOW_SELFTEST");
    const int selftest = st ? atoi(st) : 0;
    /* Live input, gated: GetAsyncKeyState reads machine-global key state,
       focus or no focus, so a headless selftest picks up anyone typing
       anywhere on the box. Under a selftest every live keyboard read comes
       through here and reads released -- the scripted probes
       (SM64DS_SELFTEST_* above and below) are the only input a selftest
       has. The pad and the mouse-look are gated the same way where they
       are read. */
    auto key_live = [&](int vk) { return !selftest && W.GetAsyncKeyState_(vk) < 0; };
    int frame = 0;
    float cam_yaw = 0.0f;   /* camera heading around Mario, radians */
    float cam_pitch = 0.13f; /* camera tilt above level, radians (R/F) */
    const int trace_cam = getenv("SM64DS_TRACE_CAM") != 0;
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

    static ntr::Framebuffer fb;
    MSG msg;
    for (;;) {
        double t_frame, t_phase;
        int game_ticked = 1;   /* cleared when a tick is skipped */
        while (W.PeekMessageA_(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return 0;
            W.TranslateMessage_(&msg);
            W.DispatchMessageA_(&msg);
        }
        ph_begin(&t_frame);
        ph_begin(&t_phase);
        {
            const int now = key_live(VK_F3);
            if (now && !overlay_edge) g_overlay_on = !g_overlay_on;
            overlay_edge = now;
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
            if (mo_look && !selftest) {
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
        /* ---- THE DEBUG MENU'S OWN INPUT. It runs before anything else reads
           the keyboard, and while it is open it swallows the keys it uses and
           the tick is skipped below, so nothing it does can also be a walk.
           Every key here is edge-detected off one held-mask, which is the
           cheapest way to get "one step per press" out of GetAsyncKeyState. */
        if (!selftest) {
            static unsigned menu_prev;
            unsigned held = 0;
            unsigned edge;
            if (W.GetAsyncKeyState_(VK_F5) < 0)     held |= 1u << 0;
            if (W.GetAsyncKeyState_(VK_UP) < 0)     held |= 1u << 1;
            if (W.GetAsyncKeyState_(VK_DOWN) < 0)   held |= 1u << 2;
            if (W.GetAsyncKeyState_(VK_LEFT) < 0)   held |= 1u << 3;
            if (W.GetAsyncKeyState_(VK_RIGHT) < 0)  held |= 1u << 4;
            if (W.GetAsyncKeyState_(VK_RETURN) < 0) held |= 1u << 5;
            if (pad_live) {
                if (pad.buttons & 0x0001) held |= 1u << 1;   /* d-pad up    */
                if (pad.buttons & 0x0002) held |= 1u << 2;   /* d-pad down  */
                if (pad.buttons & 0x0004) held |= 1u << 3;   /* d-pad left  */
                if (pad.buttons & 0x0008) held |= 1u << 4;   /* d-pad right */
                if (pad.buttons & 0x0020) held |= 1u << 0;   /* BACK        */
                if (menu_on && (pad.buttons & 0x1000)) held |= 1u << 5;  /* A */
            }
            edge = held & ~menu_prev;
            menu_prev = held;
            if (edge & (1u << 0)) {
                menu_on = !menu_on;
                fprintf(stderr, "[menu] %s\n", menu_on ? "open" : "closed");
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
                    if (dec || inc) switch (menu_sel) {
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
                                    *(int *)(c + 0x5c) = ex << 12;
                                    *(int *)(c + 0x60) = ey << 12;
                                    *(int *)(c + 0x64) = ez << 12;
                                    *(short *)(c + 0x8e) = (short)eyaw;
                                    *(int *)(c + 0x98) = 0;   /* mHorzSpeed */
                                    *(int *)(c + 0xa4) = 0;
                                    *(int *)(c + 0xa8) = 0;   /* mVertSpeed */
                                    *(int *)(c + 0xac) = 0;
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
                    case MENU_SNAP:
                        g_fake_snap = !g_fake_snap;
                        fprintf(stderr, "[menu] fake snap %s\n",
                                g_fake_snap ? "ON" : "off");
                        break;
                    case MENU_OVERLAY:
                        g_overlay_on = !g_overlay_on;
                        break;
                    case MENU_CAMERA:
                        if (real_camera) {
                            cam_mode = dec ? (cam_mode + 2) % 3
                                           : (cam_mode + 1) % 3;
                            if (cam_mode != CAM_DS) fc_seed(cam);
                            if (cam_mode == CAM_ANALOG) an_pivot_live = 0;
                            fprintf(stderr, "[menu] camera %s\n",
                                    cam_mode_name(cam_mode));
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
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
            {
                const int r = fc_stick_rate(stick_rx, CAM_STEP) + mouse_dyaw;
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
            if (key_live('Q')) { fc_yaw -= CAM_STEP / 2; rig_touched = 1; }
            if (key_live('E')) { fc_yaw += CAM_STEP / 2; rig_touched = 1; }
            {
                int zoom = 0;
                if (pad_live && (pad.buttons & 0x0100)) zoom -= 1;   /* LB */
                if (pad_live && (pad.buttons & 0x0200)) zoom += 1;   /* RB */
                zoom -= mouse_wheel;   /* wheel forward pulls the eye in */
                if (zoom) {
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
                cam_yaw += 0.045f * (pad.rx / 32768.0f);
                orbiting = 1;
            }
            if (pad.ry > 10000 && cam_pitch < 0.85f) cam_pitch += 0.02f;
            if (pad.ry < -10000 && cam_pitch > -0.15f) cam_pitch -= 0.02f;
        }
        if (key_live('Q')) { cam_yaw -= 0.045f; orbiting = 1; }
        if (key_live('E')) { cam_yaw += 0.045f; orbiting = 1; }
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
            *(unsigned short *)((char *)data_020a0e58 + 0) = raw;
            *(unsigned short *)((char *)data_020a0e58 + 2) =
                (unsigned short)(raw & (unsigned short)~raw_prev);
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
            _ZN5Stage10CheckInputEv();
            /* the matched TU writes its own data_0209f498 block; older
               TUs read per-field split symbols -- copy the record out */
            {
                const char *q = (const char *)data_0209f498;
                *(short *)(data_0209f4a0 + 0) = *(const short *)(q + 0x08);
                *(short *)data_0209f4a2 = *(const short *)(q + 0x0a);
                *(short *)data_0209f4a4 = *(const short *)(q + 0x0c);
                *(short *)data_0209f4a6 = *(const short *)(q + 0x0e);
                data_0209f4ac[0] = *(const unsigned char *)(q + 0x14);
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
           remap tables are ROM pointers with no host image). DS bits:
           1 = A (punch), 2 = B (jump), 0x100 = R (crouch), 0x800 = the
           dash button the walk core reads. */
        {
            static unsigned short btn_was;
            unsigned short btn = 0;
            if (key_live(VK_SPACE)) btn |= 2;
            if (key_live(VK_SHIFT)) btn |= 0x800;
            if (key_live(VK_CONTROL)) btn |= 0x400;
            if (key_live('X')) btn |= 1;
            if (pad_live) {
                /* Xbox layout per Tango: A jump, X run, B punch,
                   bumpers rotate the camera. RT is meant to be crouch,
                   but the old "crouch = 0x100" binding was a GUESS and
                   0x100 is provably the camera rotate-right bit
                   (func_02009e70 reads held & 0x4300) -- likely what
                   the LT "crouch crash" actually hit. The REAL crouch
                   bit is 0x400 (St_Crouch_Main holds on it, St_Land
                   enters with it, Crawl exits by it). */
                if (pad.buttons & 0x1000) btn |= 2;      /* A  -> jump  */
                if (pad.buttons & 0x4000) btn |= 0x800;  /* X  -> dash  */
                if (pad.buttons & 0x2000) btn |= 1;      /* B  -> punch */
                if (pad.rt > 100) btn |= 0x400;          /* RT -> crouch */
                /* the bumpers are camera-rotate and go in with the rest of
                   the rotate input below, where the freecam gate is */
            }
            /* selftest: synthetic hop at frame 30 (walking start speed) */
            if (selftest && frame >= 30 && frame <= 33 &&
                !getenv("SM64DS_SELFTEST_DASHJUMP") &&
                !getenv("SM64DS_SELFTEST_PUNCH") &&
                !getenv("SM64DS_SELFTEST_IDLE"))
                btn |= 2;
            if (selftest && getenv("SM64DS_SELFTEST_DASH") && frame >= 20)
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
            /* punch probe: A-button edge at f40 */
            if (selftest && getenv("SM64DS_SELFTEST_PUNCH") &&
                frame >= 40 && frame <= 42)
                btn |= 1;
            /* SWIM probe: a B-button STROKE every 24 frames. Swimming is the
               one locomotion in the game the stick alone cannot drive --
               St_Swim_Main moves him on the stroke, not on the tilt -- so a
               selftest that only writes the stick floats in place forever.
               This is what makes "walk in, swim across, climb out" runnable
               without a person on the pad. */
            if (selftest && getenv("SM64DS_SELFTEST_SWIM") &&
                (frame % 24) < 3)
                btn |= 2;
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
                if (key_live('Q')) btn |= 0x200;
                if (key_live('E')) btn |= 0x100;
                if (key_live('C')) btn |= 0x4000;
                if (stick_rx < -10000) btn |= 0x200;
                if (stick_rx > 10000) btn |= 0x100;
                if (pad_live) {
                    if (pad.buttons & 0x0100) btn |= 0x200;  /* LB -> cam L */
                    if (pad.buttons & 0x0200) btn |= 0x100;  /* RB -> cam R */
                }
                /* orbit probe: hold the rotate-right bit from frame 20 --
                   the camera's own heading and the angle it publishes must
                   both move, and W must keep walking away from the lens */
                if (selftest && getenv("SM64DS_SELFTEST_ORBIT") && frame >= 20)
                    btn |= 0x100;
            }
            if (menu_on) btn = 0;   /* enter/A belong to the menu, not to him */
            *(unsigned short *)(data_0209f49c + 0) = btn;
            *(unsigned short *)(data_0209f49e + 0) =
                (unsigned short)(btn & (unsigned short)~btn_was);
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
            if (force_wj && frame == 10) {
                /* SM64DS_FORCE_CHAR=<0-3> picks the row: 0/2 (Mario, Wario)
                   are hosted, 1/3 (Luigi, Yoshi) are not and have to come
                   out as the mapper's loud no-op, not a wild jump. */
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
        if (menu_on) {
            game_ticked = 0;
        } else if (boot_spawns) {
            /* Nothing to undo before the tick any more. The view matrix is
               the one Camera::Render published, in the ROM's own scene units,
               and Actor::BeforeBehavior reads exactly those three words to
               place every actor for the Clipper. */
            port_actor_tick();
        } else if (*(void **)(c + 0x370)) {
            hal_player_behavior(player);
        } else {
            hal_player_st_wait_main(player);
        }
        /* the real boot seats the path table, so the tracking's own binding
           stands -- except where the port's unfilled floor record invents
           one the level cannot produce (hal/level_boot.cpp) */
        if (real_boot)
            port_stage_path_guard(player);
        ph_end(PH_INPUT, t_phase);
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
               same single write the freecam always did. */
            if (cam_mode != CAM_DS) *(short *)data_020a1050 = fc_yaw;
            func_0203e0ac();
            if (trace_cam)
                fprintf(stderr,
                        "[cam-in] f%03d rx=%6d ry=%6d fc=%d yaw=%04x "
                        "pitch=%04x dist=%d held=%04x edge=%04x fl=%08x "
                        "a17c=%04x a186=%04x a19e=%04x turn=%u wall=%u "
                        "pub=%04x\n",
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
                        (unsigned short)*(short *)((char *)data_020a1164));
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
                   the second. (ShadowModel::RenderAll sits between them on the
                   ROM; the port's shadows are still the actors' own.) */
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
               actor's own look-at, which is what made it free of him. */
            if (cam_mode != CAM_DS) {
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
                   the second. (ShadowModel::RenderAll sits between them on the
                   ROM; the port's shadows are still the actors' own.) */
                port_stage_render_skybox(stage);
                port_stage_render_model(stage);
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
        hal_render_player_world(player);
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
        ph_end(PH_RASTER, t_phase);
        /* Bottom of the DS 2D frame: upload the shadows the game filled,
           rasterise engine B, and drop it into the corner at 1:1 DS pixels.
           With the panel toggled off this writes nothing. Before the overlay,
           so F3 text stays readable over the panel. */
        hal_sub_screen_present(&fb.px[0][0], ntr::SCREEN_W, ntr::SCREEN_H);

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
            ovl_draw(fb, os);
        }
        if (menu_on) menu_draw(fb);

        ph_begin(&t_phase);
        W.StretchDIBits_(hdc, 0, 0, ntr::SCREEN_W * ZOOM, ntr::SCREEN_H * ZOOM,
                      0, 0, ntr::SCREEN_W, ntr::SCREEN_H, fb.px, &bi,
                      DIB_RGB_COLORS, SRCCOPY);
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
        ++frame;   /* counts in live mode too -- the [cam-in]-style live
                      diagnostics carry a real frame number */
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
            ntr::ppu_write_bmp("walk_window_selftest.bmp", fb);
            printf("selftest: %d frames, pos=(%d, %d, %d)\n", frame,
                   *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
            return 0;
        }
        /* pace to 30: SM64DS game logic runs at 30fps (the DS panel
           scans 60 but gameplay ticks every other vblank). Ticking the
           game's per-frame constants at 60Hz doubled every speed --
           the "jump too fast, weird gravity" report. Sleep only the
           remainder of the 33.3ms budget. */
        {
            static LARGE_INTEGER qpf, last;
            LARGE_INTEGER now;
            if (!qpf.QuadPart) QueryPerformanceFrequency(&qpf);
            QueryPerformanceCounter(&now);
            if (!selftest && last.QuadPart) {
                const double el =
                    (now.QuadPart - last.QuadPart) * 1000.0 / qpf.QuadPart;
                if (el < 33.3) Sleep((DWORD)(33.3 - el));
                if (getenv("SM64DS_TRACE_PACE")) {
                    LARGE_INTEGER a2;
                    QueryPerformanceCounter(&a2);
                    fprintf(stderr, "[pace] work=%.2f asked=%d slept=%.2f\n",
                            el, (int)(33.3 - el),
                            (a2.QuadPart - now.QuadPart) * 1000.0 / qpf.QuadPart);
                }
            }
            QueryPerformanceCounter(&last);
        }
    }
}
