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
//   snap A/B, the overlay, the camera mode, how running works, and the
//   recorder's filename.
//   Arrows or the d-pad move, enter or A acts. It PAUSES THE GAME TICK while
//   it is open and keeps rendering, so the scene freezes and the view does not.
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

/* walk_window is the one TU that installs the crash probe, so it also emits the
   external seams (port_rich_dump_ex, port_crash_dir_get) the quarantine walker
   in port/unmatched/func_02043fdc.cpp weak-links against. */
#define PORT_FAULT_PROBE_DEFINE_EXPORTS
#include "fault_probe.h"
#include "overlay_font.h"
#include "hal/host_settings.h"   /* settings.json, the launcher's file */

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
void port_stage_a2_seat(void);
/* in-memory save state (hal/lk6_savestate.cpp): F8/F9 and the debug menu.
   save/load return 1 when they acted, has() drives the menu label. */
int lk6_savestate_save(void);
int lk6_savestate_load(void);
int lk6_savestate_has(void);
/* disk-backed save state (hal/lk7_persist.cpp): makes the slot survive a
   restart. write() mirrors a successful save to <exedir>\savestate.bin,
   read() loads it at startup; available() is 1 only when the arena is at its
   fixed base so disk states can relocate. */
int lk7_persist_write(void);
int lk7_persist_read(void);
int lk7_persist_available(void);

/* Two seconds of on-screen text for the save-state actions. Every earlier
   report of "F8 did nothing" was undiagnosable because the only evidence was a
   line on stderr, which a player never sees: a press eaten by the focus gate,
   a menu row left unconfirmed and a save that genuinely happened all looked
   identical. The toast is drawn every frame after the menu (so neither the
   menu nor the overlay hides it) and says which of the three it was. */
static char ss_toast[64];
static int  ss_toast_left;
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
/* the actor registry and the ROM's own processing lists (hal/actor_registry) */
void port_actor_tick(void);          /* phases 4/2/3: cleanup, init, behaviour */
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
/* SM64DS_FB_TRACE=1: a hash of the whole framebuffer at each of the four
   stages that write it -- the 3D raster, the engine-A message composite, the
   bottom-screen panel and the master-brightness fade. When two builds produce
   different BMPs, the first stage whose hash differs, on the first frame it
   differs, is the whole localisation: it separates "the 3D raster drew
   something else" from "a 2D layer landed somewhere else" without a single
   pixel dump. Off by default and free when off. */
static void fb_stage_hash(int frame, const char *stage, const ntr::Framebuffer &fb)
{
    static int on = -1;
    if (on < 0) on = getenv("SM64DS_FB_TRACE") ? 1 : 0;
    if (!on) return;
    unsigned h = 2166136261u;
    const unsigned char *b = (const unsigned char *)&fb.px[0][0];
    for (size_t i = 0; i < sizeof fb.px; ++i) h = (h ^ b[i]) * 16777619u;
    fprintf(stderr, "[fbh] f%d %s %08x\n", frame, stage, h);
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
   what right does, so the whole thing works one-handed on a pad.

   IT PAUSES THE GAME TICK while it is open -- port_actor_tick is skipped and
   the input the harness writes is zeroed, so nothing moves, nothing spawns and
   nothing decides anything while a person is reading. Rendering carries on, so
   the frame stays live behind the menu and the F3 overlay's fps keeps counting
   while its TICK rate falls to zero. That divergence is what the two numbers
   are next to each other for. */
enum {
    MENU_WARP = 0,
    MENU_LEVEL,
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
   run to F3 would also toggle the overlay on the way past and binding it to
   escape would close the game -- and escape is the one key the window
   procedure, not the frame loop, acts on. So the capture lives where the
   presses arrive: while it is armed, WM_KEYDOWN never reaches the escape
   branch and never reaches the window at all, and the key it saw is handed
   to the frame loop through g_rebind_key.

   Level key reads are gated too, one line inside key_live, so nothing that
   was already held keeps steering while a person is picking a button. */
static int g_rebind_capture;          /* armed: swallow presses, report them */
static int g_rebind_key;              /* the virtual-key the proc last saw */

/* Keys and pad buttons this program has already spoken for. Binding run to
   one of them would not produce a conflict a player could see and undo; it
   would produce a menu that cannot be closed, or a game that quits when you
   start running. So the capture refuses them and says so, which is the one
   case where refusing is friendlier than obeying. */
static int run_key_reserved(int vk)
{
    if (vk == VK_ESCAPE || vk == VK_RETURN) return 1;   /* quit, menu act */
    if (vk >= VK_F1 && vk <= VK_F9) return 1;           /* the port's own row */
    if (vk >= VK_LEFT && vk <= VK_DOWN) return 1;       /* menu navigation */
    /* the bottom-screen panel, and it is read in hal/sub_screen.cpp rather
       than through this file's key_live -- so a binding on it would fire the
       panel from outside every gate here */
    if (vk == VK_TAB) return 1;
    return 0;
}
static int run_pad_reserved(unsigned mask)
{
    /* the d-pad (0x000f) and A (0x1000) drive the menu, BACK (0x0020) opens
       it, and the right stick's click (0x0080) is the freecam toggle */
    return (mask & 0x10afu) != 0;
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
    {
        int lv = 0, en = 0;
        const int real = port_title_row(menu_level_row, &lv, &en);
        if (!real)
            snprintf(ln[MENU_LEVEL], sizeof ln[0],
                     "level select      row %2d of %d   (%d = a scene, not a "
                     "level)", menu_level_row, port_title_rows(), lv);
        else
            snprintf(ln[MENU_LEVEL], sizeof ln[0],
                     "level select      row %2d of %d   level %2d entrance %d "
                     " ov%03d  %s", menu_level_row, port_title_rows(), lv, en,
                     port_level_overlay_id(lv),
                     port_level_is_mounted(lv) ? "MOUNTED"
                                               : "not mounted in this build");
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
    lstrcpynA(path + n, "startup_error.txt", (int)(cap - n));
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
    pacer_begin();
#ifdef PORT_ROM_CLEAN
    /* ROM-CLEAN: load + verify the ROM tables from romdata.bin FIRST, before
       any table is read. Loud FATAL if the file is missing, short, or fails its
       manifest sha. The pointer-rebase passes below (port_ov002_patch,
       port_cross_patch, the overlay syms patches) then run over the loaded
       bytes exactly as they would over baked-in ones. */
    port_romdata_load();
#endif
    if (!_ZN4Heap13SetupRootHeapEv()) return 2;
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

    /* THE GAME'S OWN LEVEL BOOT, now the default: ov009 mounted,
       Stage::LoadClsnAndObjects run against it, and the level's own entrance
       record spawning the Player and the Camera. SM64DS_LEGACY_BOOT=1 goes
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
            /* SM64DS_SPAWN_ACTOR=<id>[:<param>][,...]: put one actor of each
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

    /* GATE 35: the course loop's own boot. After the level and the entrance,
       because it reads data_0209f2f8 for the sound row and wants the Player
       already spawned; before the frame loop, because the health words it
       seats are what the HUD's first Behavior reads.
       SM64DS_NO_COURSE_SEAT=1 goes back to the gate-28 state: the single
       hand-written health word above, no lives, no music, and the queued
       character swap left reading as pending. */
    if (real_boot && !getenv("SM64DS_NO_COURSE_SEAT"))
        port_course_seat();

    /* window */
    WNDCLASSA wc = {};
    wc.lpfnWndProc = wndproc;
    wc.hInstance = GetModuleHandleA(0);
    wc.hCursor = W.LoadCursorA_(0, (LPCSTR)IDC_ARROW);
    wc.lpszClassName = "sm64ds_walk";
    W.RegisterClassA_(&wc);
    RECT r = {0, 0, ntr::SCREEN_W * ZOOM, ntr::SCREEN_H * ZOOM};
    W.AdjustWindowRect_(&r, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE);
    /* THE TITLE BAR IS THE CONTROLS CARD. There is nowhere else to put them
       that does not cost a keypress to read: the F3 overlay is timings, the
       F5 menu is state, and both of those you have to already know how to
       open. The bar is the one surface that is legible before you touch
       anything, so the keys live there. */
    HWND hwnd = W.CreateWindowExA_(0, "sm64ds_walk",
                              "SM64DS   |   WASD move   Shift dash   Space jump"
                              "   X punch   Ctrl crouch   |   Q/E turn   R/F"
                              " tilt   |   F1 camera   F3 stats   F5 menu"
                              "   Tab panel   Esc quit",
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
       are read.

       SECOND GATE, focus: the same machine-global read meant that a player who
       alt-tabbed and typed somewhere else kept walking Mario around, and that a
       direction held at the moment they left stayed held forever.
       hal_window_focused() is false whenever this window is not the foreground
       one, and then every interactive key here reads RELEASED. That is also the
       release: the pad words further down are rebuilt from these reads every
       frame, so the frame focus goes away is the frame the stick and the
       buttons go to neutral, with no separate teardown to keep in step.

       Nothing a player pressed while away arrives late either. There is no
       queue to replay -- these are level reads, not messages -- but a key still
       physically down on the way back would otherwise read as a fresh press and
       fire the edge latches (F1, F3, F4, the menu). So on the focus-regained
       edge every key is marked STALE, and a stale key keeps reading released
       until it is seen physically up. Pressing it again after that works
       normally.

       key_stale is indexed by virtual-key code, which is what every caller
       passes and is 0..255 by definition; the bounds test is there because this
       lambda is the one place that would turn a typo into a stray write. */
    unsigned char key_stale[256] = {0};
    int focus_was = 1;   /* launch focused = launch unchanged */
    auto key_live = [&](int vk) -> int {
        if (selftest) return 0;
        /* while the rebind row is capturing, the whole window is deaf: the
           press being captured must not also be a camera toggle or a save
           state, and whatever was already held must not keep steering while
           a person is choosing a button. One line here covers every caller,
           which is why there is not a gate at each of them. */
        if (g_rebind_capture) return 0;
        if (!hal_window_focused()) return 0;
        const int down = W.GetAsyncKeyState_(vk) < 0;
        if ((unsigned)vk < 256) {
            if (!down) { key_stale[vk] = 0; return 0; }
            if (key_stale[vk]) return 0;
        }
        return down;
    };
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
    if ((!selftest || getenv("SM64DS_SS_DISKLOAD")) && lk7_persist_available()) {
        if (lk7_persist_read()) {
            an_pivot_live = 0;   /* no ease across the load */
            ss_note("state loaded from disk (F9 reloads it)");
        }
    }

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
                       ss_disk = 0;
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
            }
            if (ss_save_fr >= 0 && frame == ss_save_fr) {
                lk6_savestate_save();
                /* the cross-restart reproducer's first half: mirror this save
                   to savestate.bin so a SECOND run (SM64DS_SS_DISKLOAD=1) can
                   boot from it and compare hashes across the restart */
                if (ss_disk)
                    fprintf(stderr, "[ss-repro] f%d disk write: %s\n", frame,
                            lk7_persist_write() ? "ok" : "SKIPPED/FAILED");
                ss_lock_at_save = data_0209d660;
                ss_hash_at_save = ss_hw_hash();
                ss_saw_save = 1;
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
                if (lk6_savestate_load()) an_pivot_live = 0;
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
        /* ---- THE DEBUG MENU'S OWN INPUT. It runs before anything else reads
           the keyboard, and while it is open it swallows the keys it uses and
           the tick is skipped below, so nothing it does can also be a walk.
           Every key here is edge-detected off one held-mask, which is the
           cheapest way to get "one step per press" out of GetAsyncKeyState. */
        if (!selftest) {
            static unsigned menu_prev;
            unsigned held = 0;
            unsigned edge;
            /* through key_live, not the raw read, so the menu is behind the
               focus gate and the stale-key latch with everything else. Under a
               selftest this block never runs at all, so routing it here changes
               nothing an automated run sees. */
            if (key_live(VK_F5))     held |= 1u << 0;
            if (key_live(VK_UP))     held |= 1u << 1;
            if (key_live(VK_DOWN))   held |= 1u << 2;
            if (key_live(VK_LEFT))   held |= 1u << 3;
            if (key_live(VK_RIGHT))  held |= 1u << 4;
            if (key_live(VK_RETURN)) held |= 1u << 5;
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
                    case MENU_LEVEL:
                        /* left/right move the cursor exactly as
                           func_ov003_020ad814 does (+/-1, modulo the row
                           count); enter runs its else-branch. */
                        if (edge & (1u << 5)) {
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
                            port_player_set_character(c, g_character_pending);
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
                        if (real_camera) {
                            cam_mode = dec ? (cam_mode + 2) % 3
                                           : (cam_mode + 1) % 3;
                            if (cam_mode != CAM_DS) fc_seed(cam);
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
            /* TEMPORARY headless input: OR any SM64DS_PROBE_INPUT press for this
               frame into the raw pad mirror BEFORE CheckInput, so the remap and
               the direct readers (IsButtonInputValid, Message::Update) both see
               it. Does nothing without the env var. */
            port_input_probe_apply(frame);
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
            if (!selftest && run_mode() == RUN_ANALOG && pad_live && !menu_on) {
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
           remap tables are ROM pointers with no host image). DS bits:
           1 = A (punch), 2 = B (jump), 0x100 = R (crouch), 0x800 = the
           dash button the walk core reads. */
        {
            static unsigned short btn_was;
            unsigned short btn = 0;
            if (key_live(VK_SPACE)) btn |= 2;
            /* THE RUN BUTTON, from wherever the player put it. Shift by
               default, so an untouched settings.json is the line that was
               here before; zero means they unbound the keyboard half. */
            if (g_run_key && key_live(g_run_key)) btn |= 0x800;
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
                /* X by default; the rebind row moves it (0 = unbound) */
                if (g_run_pad && (pad.buttons & (unsigned)g_run_pad))
                    btn |= 0x800;
                if (pad.buttons & 0x2000) btn |= 1;      /* B  -> punch */
                if (pad.rt > 100) btn |= 0x400;          /* RT -> crouch */
                /* the bumpers are camera-rotate and go in with the rest of
                   the rotate input below, where the freecam gate is */
            }
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
                player = data_0209f394[0];
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
                    _ZN5Stage9LoadModelEv(stage);
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
                } else if (!strcmp(cp_what, "star")) {
                    if (frame == cp_frame) {
                        port_star_collect(0);
                        cp_done = 1;
                    }
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
            port_actor_tick();
        } else if (*(void **)(c + 0x370)) {
            hal_player_behavior(player);
        } else {
            hal_player_st_wait_main(player);
        }
        /* THE FADE STEPS HERE, and it steps every frame -- even with the menu
           open and the game tick skipped -- because a fade transition must not
           freeze while it is on screen. This is func_02018ec0's job in the
           ROM's own frame (phase 2, func_02019390): advance the fader currently
           in motion (data_0209d4b0) by one frame, which writes the 2D blend
           register the compositor below reads. */
        port_fader_advance();
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
        port_input_probe_trace_msg(frame);   /* TEMPORARY: SM64DS_TRACE_MSG */
        port_input_probe_trace_cannon(frame);/* TEMPORARY: SM64DS_TRACE_CANNON */
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
        fb_stage_hash(frame, "raster", fb);
        /* ENGINE-A 2D OVER 3D. The top screen is engine A: its 2D BGs and OBJ
           layer composite over the 3D frame in hardware. The dialogue box lives
           there (BG3 + the cursor OBJ), so raster engine A's 2D and write only
           the covered pixels over the 3D framebuffer. Before the fade composite,
           so the box dims with the master-brightness blend the same as the DS. */
        port_message_composite_engine_a(&fb);
        fb_stage_hash(frame, "msg", fb);
        ph_end(PH_RASTER, t_phase);
        /* Bottom of the DS 2D frame: upload the shadows the game filled,
           rasterise engine B, and drop it into the corner at 1:1 DS pixels.
           With the panel toggled off this writes nothing. Before the overlay,
           so F3 text stays readable over the panel. */
        hal_sub_screen_present(&fb.px[0][0], ntr::SCREEN_W, ntr::SCREEN_H);
        fb_stage_hash(frame, "sub", fb);

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

        fb_stage_hash(frame, "fade", fb);

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
        /* the save-state toast, over everything, bottom-left. Decremented here
           rather than in the tick so the menu's pause does not freeze it. */
        if (ss_toast_left > 0) {
            --ss_toast_left;
            const int tw = (int)strlen(ss_toast) * OVL_ADVANCE * OVL_SCALE;
            const int ty = ntr::SCREEN_H - OVL_LINE - 4;
            ovl_shade(fb, 2, ty - 2, tw + 8 * OVL_SCALE,
                      OVL_LINE + 4 * OVL_SCALE);
            ovl_text(fb, 4 + OVL_SCALE, ty, ss_toast, 0xFFFFFFFFu);
        }

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
