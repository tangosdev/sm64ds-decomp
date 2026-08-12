// The DS BOTTOM SCREEN as a corner panel, host side.
//
// Everything the bottom screen draws is matched game code -- HUD::Render,
// Minimap::Render, Message, and OAM::Render underneath all of them. What was
// missing was never the drawing; it was four seams:
//
//   1. DUAL OAM. OAM::Render's first decision is
//
//          if (!sub || data_0209e660 == 1) -> the MAIN shadow
//
//      and the HAL pinned that byte at 1, so every sub==true sprite in the
//      game landed in the top screen's shadow buffer. Clearing it is what
//      OAM::EnableSubOAM does on the ROM, and it is only safe once the three
//      main-shadow symbols are contiguous (hal/model_host.cpp), because
//      OAM::Reset's mode-0 path fills the buffer through them.
//
//   2. THE FRAME. The ROM's 2D frame is Reset at the top, Render calls in the
//      middle, Load at the bottom -- the shadow is rebuilt every frame and
//      uploaded once. Nothing in the port was calling any of the three.
//
//   3. POWCNT1 BIT 15. OAM::Load reads it to decide which shadow goes to
//      which engine's OAM. Zero swaps them, which would upload the top
//      screen's sprites to the bottom engine.
//
//   4. SCAN-OUT AND PLACEMENT, which is ntr/ppu_sub.cpp plus the blit here.
//
// The panel is composited at 1:1 DS pixels into the bottom-right corner of
// whatever tier the top screen is drawn at. TAB toggles it; with it off the
// top screen is left byte-for-byte untouched, which is the regression this
// stream has to keep passing.
//
// The touch bridge polls the cursor rather than taking window messages, so
// walk_window.cpp needs no wndproc changes: the panel is a rectangle on the
// screen and a click inside it is a stylus press at the same DS pixel.
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include "host_platform_linux.h"
#endif

#include "ntr/ppu.h"

namespace OAM {
void Reset();
}

/* Stage::LoadGraphics2D is a real C++ static member in its own TU, so it is
   reached by its MSVC name through a matching declaration rather than by the
   Itanium alias the .c callers use. */
class Stage {
public:
    static void LoadGraphics2D(bool b, int i);
};

extern "C" {
void _ZN3OAM4LoadEv(void);
unsigned int _ZN3OAM12EnableSubOAMEv(void);
int hal_oam_layout_check(void);
/* the sprite-template guard (hal/oam_lists.cpp): every OamAttr* the HUD and
   the Minimap hand OAM::Render, checked for a missed pointer rebase */
int hal_oam_templates_check(void);
int hal_oam_walk_probe(void);
/* the minimap's per-frame affine callback (port/unmatched/Minimap_Affine.cpp),
   which is func_02019144's first beat */
void port_minimap_affine_update(void);
extern unsigned char data_0209e660;
extern unsigned char data_0209caa0[];   /* the save block; byte 8 bit 7 = intro seen */
extern signed char data_0209f2f8;       /* current level */
/* The sub engine's LAYER ENABLE MASK, bits 0..4 = BG0..BG3, OBJ. Nine
   different ROM functions publish it into DISPCNT_B bits 8-12 with the very
   same line; Minimap::Behavior and Message::UpdateWindow are two of them. It
   is the game's own switchboard for what the bottom screen shows. */
extern unsigned char data_0209d454;
/* TouchInfo data_020a0de8[4]: {u8 touched, u8 held, u8 x, u8 y} per slot, in
   DS bottom-screen pixels. Stage::CheckCameraInput and every TouchArea read
   it; nothing on the host was writing it. */
extern unsigned char data_020a0de8[];
/* Stage::CheckCameraInput's own inputs and outputs */
void _ZN5Stage16CheckCameraInputEv(void);
extern int data_0209f498[];      /* the Ctrl[4] block, stride 0x18 */
extern char data_0209f49c[];     /* split: held buttons  (DS: f498 + 4) */
extern char data_0209f49e[];     /* split: pressed       (DS: f498 + 6) */
extern void *data_0209f318;      /* the Camera actor */
}

namespace {

const int kMargin = 8;

bool g_on = true;
bool g_ready;
/* Headless runs must not read the mouse or the keyboard. SM64DS_WINDOW_SELFTEST
   is CI's comparator and several of these binaries run on one machine at once,
   so a stray click or a TAB meant for another window would land in this one's
   frame and show up as a BMP diff nobody can reproduce. The selftest drives
   its own input; the panel adds none. */
bool g_headless;
/* SM64DS_INPUT_NOFOCUSGATE=1 puts the interactive keyboard back on the
   machine-global reads it used before the focus gate. Nothing in the tree
   sets it; it exists so a harness that really does want keys read with the
   window in the background has a documented way to say so. */
bool g_nofocusgate;
int g_x0, g_y0;            // panel origin in framebuffer pixels
int g_div = 2;             // panel downscale divisor (SM64DS_SUB_SCALE)
int g_zoom = 1;
HWND g_hwnd;

#ifdef _WIN32
BOOL(WINAPI *GetCursorPos_)(POINT *);
BOOL(WINAPI *ScreenToClient_)(HWND, POINT *);
SHORT(WINAPI *GetAsyncKeyState_)(int);
HWND(WINAPI *GetForegroundWindow_)(void);
#else
/* Linux: bound to the SDL-backed shim. ScreenToClient is identity because SDL
   reports the cursor in window (client) coordinates already. */
static BOOL  (*GetCursorPos_)(POINT *)    = port_lin_GetCursorPos;
static BOOL   ScreenToClient_lin(HWND, POINT *) { return TRUE; }
static BOOL  (*ScreenToClient_)(HWND, POINT *) = ScreenToClient_lin;
static short (*GetAsyncKeyState_)(int)    = port_lin_GetAsyncKeyState;
/* No focus gate on Linux yet: SDL reports focus through its own event
   stream, not a user32 call. Left null deliberately so
   hal_window_focused takes its documented fail-open path (`!Get
   ForegroundWindow_ -> return 1`) and the Linux window never locks
   itself out of its own input. */
static HWND (*GetForegroundWindow_)(void) = 0;
typedef short SHORT;
#endif

ntr::SubFramebuffer g_sub;

int env_flag(const char *name, int dflt)
{
    const char *v = std::getenv(name);
    return v ? std::atoi(v) : dflt;
}

// The stylus, from the mouse. `touched` is "down now", `held` adds "and it was
// down last frame too" -- the pair Stage::CheckCameraInput tests together to
// tell a press from a hold.
void poll_touch(void)
{
    unsigned char down = 0, sx = 0, sy = 0;
    if (!g_headless && g_on && GetCursorPos_ && ScreenToClient_ &&
        GetAsyncKeyState_ && (GetAsyncKeyState_(VK_LBUTTON) & 0x8000)) {
        POINT p;
        if (GetCursorPos_(&p) && ScreenToClient_(g_hwnd, &p)) {
            /* panel pixels back to DS pixels: the panel is drawn at 1/g_div */
            const int fx = ((int)p.x / g_zoom - g_x0) * g_div;
            const int fy = ((int)p.y / g_zoom - g_y0) * g_div;
            if (fx >= 0 && fx < ntr::SUB_W && fy >= 0 && fy < ntr::SUB_H) {
                down = 1;
                sx = (unsigned char)fx;
                sy = (unsigned char)fy;
            }
        }
    }
    const unsigned char was = data_020a0de8[0];
    data_020a0de8[0] = down;
    data_020a0de8[1] = (unsigned char)(down && was);
    if (down) {
        data_020a0de8[2] = sx;
        data_020a0de8[3] = sy;
    }
}

}  // namespace

extern "C" {

/* THE FOCUS GATE'S ONE QUESTION: is this program's window the one the player
   is actually typing into?

   It has to be asked because GetAsyncKeyState reports PHYSICAL key state for
   the whole session, foreground or not. Without this, a player who alt-tabbed
   into a chat window and typed kept walking Mario around behind them, and a
   held direction stayed held forever.

   Everything INTERACTIVE routes its keyboard reads through here. Nothing
   scripted does: SM64DS_WINDOW_SELFTEST kills the live reads a layer up,
   SM64DS_PROBE_INPUT ORs its bits straight into the pad mirror, and the
   SM64DS_SELFTEST_* probes write the stick and the buttons by hand. A hidden,
   unfocused, automated run therefore sees exactly what it saw before.

   Two deliberate fail-open cases, so this can never be what silently locks a
   run out of its own input: no window yet (g_hwnd unset, which is every
   binary that does not open one) and no GetForegroundWindow (never on
   Windows, but the whole user32 surface here is loaded by name). */
int hal_window_focused(void)
{
    if (g_nofocusgate) return 1;
    if (!g_hwnd || !GetForegroundWindow_) return 1;
    return GetForegroundWindow_() == g_hwnd ? 1 : 0;
}

/* Armed once, before the first frame. hwnd and zoom are the window's, so the
   touch bridge can turn a client pixel into a DS pixel. */
void hal_sub_screen_init(void *hwnd, int zoom)
{
    g_hwnd = (HWND)hwnd;
    g_zoom = zoom > 0 ? zoom : 1;
    g_on = env_flag("SM64DS_SUB_PANEL", 1) != 0;
    g_headless = std::getenv("SM64DS_WINDOW_SELFTEST") != 0;
    g_nofocusgate = std::getenv("SM64DS_INPUT_NOFOCUSGATE") != 0;

#ifdef _WIN32
    if (HMODULE u = LoadLibraryA("user32.dll")) {
        GetCursorPos_ = (decltype(GetCursorPos_))GetProcAddress(u, "GetCursorPos");
        ScreenToClient_ = (decltype(ScreenToClient_))GetProcAddress(u, "ScreenToClient");
        GetAsyncKeyState_ =
            (decltype(GetAsyncKeyState_))GetProcAddress(u, "GetAsyncKeyState");
        GetForegroundWindow_ =
            (decltype(GetForegroundWindow_))GetProcAddress(u, "GetForegroundWindow");
    }
#endif
    /* Linux: the three pointers are already bound to the SDL shim at file scope. */

    /* POWCNT1 bit 15: main engine drives the top screen. OAM::Load reads this
       bit to pick which shadow goes to which engine's OAM, and Scene::Reset-
       HardwareRegisters -- the ROM function that would set it -- is not in the
       port's boot. */
    *(volatile unsigned short *)0x04000304 |= 0x8000;

    /* THE AFFINE IDENTITY, and the reason the minimap drew nothing at all.
       BG3-sub in BG mode 3 is an EXTENDED AFFINE layer, so every one of its
       pixels goes through BG3PA..BG3PD before it picks a map entry. The ROM
       seeds those to 0x100 (1.0 in 8.8) in func_02053c40 -- the hardware reset
       that runs once from func_0201a054, the full boot the port does not have
       -- and nothing afterwards writes them for a minimap that is not being
       rotated. Left at zero the matrix is degenerate: every screen pixel maps
       to map pixel (0, 0), so the layer samples one blank tile and the whole
       panel comes out backdrop white. Minimap::InitResources configures BG3CNT
       and turns the layer on and is right to; it never touches the matrix.
       Both engines' BG2 and BG3 get the same treatment func_02053c40 gives
       them, so a later affine BG2 does not land on the same puzzle. */
    *(volatile unsigned short *)0x04001020 = 0x100;   /* BG2PA sub */
    *(volatile unsigned short *)0x04001026 = 0x100;   /* BG2PD sub */
    *(volatile unsigned short *)0x04001030 = 0x100;   /* BG3PA sub */
    *(volatile unsigned short *)0x04001036 = 0x100;   /* BG3PD sub */

    /* DUAL OAM. Only once the shadow really is one buffer: in mode 0 the
       Reset path fills it through data_0209e67c/data_0209e694, and if those
       are separate host arrays it would leave 127 of 128 entries as garbage
       and draw the heap onto the bottom screen. */
    if (hal_oam_layout_check()) {
        _ZN3OAM12EnableSubOAMEv();
        std::printf("[sub] dual OAM armed (data_0209e660 = %u)\n",
                    data_0209e660);
    } else {
        std::printf("[sub] dual OAM NOT armed: sub sprites stay on the main "
                    "shadow\n");
    }
    OAM::Reset();

    /* THE BOTTOM SCREEN'S OWN VRAM. Stage::LoadGraphics2D is the ROM's 2D
       asset load and it fills both screens: the sub BG character data, the
       three sub tilemaps, the sub BG palette, and the BGxCNT_SUB words that
       say where all of it lives. Without it engine B scans out an empty bank
       and the panel is one flat backdrop colour.
       The bit at data_0209caa0[8] & 0x80 -- "the intro has played" -- picks
       between two entirely different asset sets:
         clear: files 0x239..0x23e, plain FAT files. The PRE-INTRO CLOUDS --
                BG0 characters, the BG0 and BG1 tilemaps, the sub BG palette,
                and the sub OBJ tiles with their palette.
         set:   the IN-GAME set, whose character data and half its tilemaps
                are language files (handles 0xa00d / 0xa009 / 0xa00b) living
                in ARCHIVE/cee.narc.

       THE IN-GAME SET IS THE DEFAULT NOW. It was not, for one reason: the old
       extraction was a US dump that did not carry cee.narc, so the in-game
       tilemaps loaded and every tile they named came out blank, and the clouds
       were the branch whose bytes existed. The EU dump has the archive and all
       three handles resolve, so the port takes the branch the game takes while
       a level is being played.

       It also matters for what the sprites can be SEEN over. The cloud set
       loads its three BGs at priority 0 and every cloud pixel is opaque, so the
       whole HUD lost the priority compare against a backdrop that should not
       have been on screen at all. SM64DS_GFX2D_PREINTRO=1 puts the clouds
       back. */
    if (!std::getenv("SM64DS_NO_GFX2D")) {
        const unsigned char saved = data_0209caa0[8];
        if (std::getenv("SM64DS_GFX2D_PREINTRO"))
            data_0209caa0[8] &= ~0x80;
        else
            data_0209caa0[8] |= 0x80;
        Stage::LoadGraphics2D(false, data_0209f2f8);
        data_0209caa0[8] = saved;
        std::printf("[sub] Stage::LoadGraphics2D(0, %d) done, layer mask "
                    "data_0209d454 = %02x\n", (int)data_0209f2f8,
                    data_0209d454);
    }

    /* THE LAYER MASK, and it is Stage::InitResources' own value:
     *
     *     data_0209d454 = 0x18;
     *
     * bit 3 BG3 -- the minimap -- and bit 4 OBJ -- every sprite the HUD and the
     * minimap draw. That one line is the gameplay bottom screen, and it lives
     * in the same ROM function this block already copies the DISPCNT words out
     * of, which the port does not run.
     *
     * Nothing else was going to set the OBJ bit. Minimap::Behavior maintains
     * bit 3 every frame -- ORs it in when it has a map id, clears it when it
     * does not -- but no hosted path touches bit 4, so with the in-game asset
     * set the mask came out 0x08 and the sprites were composited out of a
     * screen they had already been drawn into.
     *
     * SM64DS_SUB_LAYERS still overrides, and now it is a debugging knob rather
     * than the only way to see anything. */
    data_0209d454 = 0x18;

    /* THE SUB ENGINE'S OWN DISPCNT, which nothing in the port was setting --
       it read back 0, meaning "display off, no layers, no sprites".
       Every value here is Stage::InitResources', the ROM function that puts
       the bottom screen into gameplay shape and that the port does not run:

           *p1 &= 0xFFCFFFEF        OBJ mapping 2D, tile boundary 32
           GXS::SetGraphicsMode(3)  BG mode 3: BG0/1/2 text, BG3 EXTENDED --
                                    which is the mode the minimap needs, and
                                    the reason its 16-bit map entries carry a
                                    palette field at all
           *p1 |= 0x10000           display mode 1, the graphics display
           data_0209d454 = 0x18     BG3 + OBJ: the minimap and the sprites

       Bit 30 (BG extended palettes) is GX::SetBankForSubBGExtPltt's, reached
       from GXS::EndLoadBGExtPltt through the VRAM bank allocator the port
       does not host. Set here so the minimap's palettes are readable. */
    {
        volatile unsigned *p1 = (volatile unsigned *)0x04001000;
        *p1 &= 0xFFCFFFEFu;
        *p1 = (*p1 & ~7u) | (unsigned)env_flag("SM64DS_SUB_BGMODE", 3);
        *p1 |= 0x10000u;            /* display mode 1 */
        *p1 |= 0x40000000u;         /* BG extended palettes */
        if (!data_0209d454)
            data_0209d454 = 0x18;   /* Stage::InitResources' own value */
    }

    std::printf("[sub] panel %s (TAB toggles, SM64DS_SUB_PANEL=0 to start "
                "off)\n", g_on ? "on" : "off");
}

/* Top of the 2D frame: both shadows back to "every sprite disabled" and both
   entry counters to zero, so this frame's Render calls fill from the start. */
void hal_sub_screen_frame_begin(void)
{
    /* Once, on the first frame, and deliberately here rather than in init:
       by now every pointer pass has run -- ovdata's per-overlay one from the
       harness, the cross-overlay fixups from hal_fill_hud_vtable and
       hal_fill_minimap_vtable -- and the HUD and the Minimap are about to walk
       what those passes produced. hal/oam_lists.cpp says what a missed entry
       costs and why the fault it causes lands nowhere near it. */
    hal_oam_templates_check();
    /* SM64DS_OAM_WALK_PROBE=1: show the fault itself, on a guarded page, so
       the mechanism is reproducible rather than one run in a couple of
       hundred. Once, and only when asked for. */
    {
        static int probed;
        if (!probed && std::getenv("SM64DS_OAM_WALK_PROBE")) {
            probed = 1;
            hal_oam_walk_probe();
        }
    }

    /* TAB is interactive keyboard, so it is gated on focus like the rest. The
       latch is ARMED rather than cleared while the window is in the
       background: a TAB pressed elsewhere and still down on the way back
       reads as already-held, so it cannot toggle the panel on arrival. */
    static int tab_was;
    if (GetAsyncKeyState_ && !g_headless) {
        if (!hal_window_focused()) {
            tab_was = 1;
        } else {
            const int tab = (GetAsyncKeyState_(VK_TAB) & 0x8000) != 0;
            if (tab && !tab_was) g_on = !g_on;
            tab_was = tab;
        }
    }
    OAM::Reset();
    poll_touch();
}

/* Bottom of the frame: upload the shadows the game filled, rasterise engine B,
   drop it into the corner. With the panel off nothing here writes a pixel. */
void hal_sub_screen_present(unsigned int *dst, int w, int h)
{
    /* SM64DS_SUB_SCALE is a divisor: 1 = full DS size (a quarter of the 2x
       window, Tango's "super in the way"), 2 = half size (1/16 of the
       window, the default), up to 4. */
    {
        static int init;
        if (!init) {
            init = 1;
            const int v = env_flag("SM64DS_SUB_SCALE", 2);
            g_div = v < 1 ? 1 : (v > 4 ? 4 : v);
        }
    }
    g_x0 = w - ntr::SUB_W / g_div - kMargin;
    g_y0 = h - ntr::SUB_H / g_div - kMargin;
    /* Publish the layer mask, the way nine ROM functions do with this exact
       line. Minimap::Behavior and Message::UpdateWindow both write
       data_0209d454 and then push it themselves; doing it once more here is
       what covers the frames where neither of them ran.
       SM64DS_SUB_LAYERS forces the mask, which is how a layer the game has
       switched off gets looked at without pretending it is on. */
    {
        static int forced = -2;
        if (forced == -2) forced = env_flag("SM64DS_SUB_LAYERS", -1);
        const unsigned mask =
            forced >= 0 ? (unsigned)forced : (unsigned)data_0209d454;
        *(volatile unsigned *)0x04001000 =
            (*(volatile unsigned *)0x04001000 & ~0x1f00u) | (mask << 8);
    }
    /* func_02019144's FIRST beat, which the port had been skipping: the scene
       graphics block's own per-frame callback. For the Stage that is the
       minimap's affine update, and without it BG3-sub keeps whatever matrix
       boot seeded -- the identity -- so the minimap draws at 1:1 rather than
       the level's own scale. port/unmatched/Minimap_Affine.cpp carries the
       callback; the rest of func_02019144 is the layer-mask publish above and
       the OAM upload below. */
    port_minimap_affine_update();
    _ZN3OAM4LoadEv();
    if (!g_on) return;
    ntr::ppu_scanout_sub(g_sub);
    ntr::ppu_compose_sub(g_sub, dst, w, h, kMargin, g_div);
    g_ready = true;

    /* SM64DS_SUB_DUMP=N: the bottom screen alone, at 256x192, on frame N. */
    {
        static int at = -2, frame;
        if (at == -2) at = env_flag("SM64DS_SUB_DUMP", -1);
        if (frame++ == at) {
            ntr::ppu_write_bmp_sub("sub_screen.bmp", g_sub);
            std::printf("[sub] wrote sub_screen.bmp at frame %d "
                        "(DISPCNT_B %08x, mask %02x, BG3CNT %04x)\n", at,
                        *(volatile unsigned *)0x04001000,
                        (unsigned)data_0209d454,
                        *(volatile unsigned short *)0x0400100e);
        }
    }
}

/* The bottom screen's camera buttons, through the game's own hit test.
 *
 * Stage::CheckCameraInput reads the stylus record, decides which of the two
 * on-screen arrows it is inside, and ORs the rotate bits into the Ctrl block
 * at data_0209f498 + 4 (held) and + 6 (pressed).
 *
 * THE SPLIT-SYMBOL BRIDGE. On the DS those two halfwords ARE data_0209f49c
 * and data_0209f49e -- 0x0209f498 + 4 and + 6 -- and the readers the port
 * uses (func_02009e70's `held & 0x4300`) name the split symbols. On the host
 * they are separate storage, which is why walk_window already copies fields
 * between them after Stage::CheckInput. So the camera buttons' contribution
 * is merged the same way, by OR: the pad word is already written, and a
 * stylus press adds to it rather than replacing it.
 *
 * The gate in front of all of it is the CAMERA's own +0x154 bit 0x1000. With
 * that clear the ROM draws no buttons and reads no touches, so if the panel
 * shows arrows and nothing rotates, this is the word to look at. */
void hal_sub_camera_input(void)
{
    const char *ctrl = (const char *)data_0209f498;
    _ZN5Stage16CheckCameraInputEv();
    *(unsigned short *)data_0209f49c |= *(const unsigned short *)(ctrl + 4);
    *(unsigned short *)data_0209f49e |= *(const unsigned short *)(ctrl + 6);

    static int said;
    if (!said++) {
        const char *cam = (const char *)data_0209f318;
        std::printf("[sub] camera buttons: Camera %p, +0x154 = %08x, live %s\n",
                    (const void *)cam,
                    cam ? *(const unsigned *)(cam + 0x154) : 0u,
                    cam && (*(const unsigned *)(cam + 0x154) & 0x1000) ? "yes"
                                                                      : "NO");
    }
}

int hal_sub_screen_on(void) { return g_on ? 1 : 0; }

// ---- the three leaves LoadGraphics2D names but never reaches ---------------
//
// Each of these sits on a branch the port does not take, and each drags a
// subsystem with no host seam behind it. They are stubbed by name rather than
// sliced in, and each says so if it is ever actually called -- which would
// mean the branch analysis is wrong, not that the stub is.

/* Only from LoadGraphics2D(b != 0), and the port passes b = false. Behind it
   is Message::LoadTextVS and the whole message-box text engine.
   PORT_HOST_ABI: src drives the card loader + asm copies into an unhosted 3D-font
   VRAM path; the message-box text engine is not hosted. */
void LoadFont3D(void)
{
    static int said;
    if (!said++)
        std::printf("  [sub] LoadFont3D reached: the 3D font is not hosted\n");
}

/* Top-screen furniture: it rasterises the controller-mode caption into
   G2::GetBG2CharPtr through func_0201d590, the main engine's text path. The
   bottom screen never reads any of it.
   PORT_HOST_ABI: src writes the top-screen BG2 text layer, a subsystem the port
   does not host. */
void LoadControllerModeText(int a)
{
    static int said;
    if (!said++)
        std::printf("  [sub] LoadControllerModeText(%d): top-screen text is "
                    "not hosted\n", a);
}

/* LoadFont's data_0209d698 == 2 branch only, which is the ov004 (VS-mode)
   font destination. The port loads font 0.
   PORT_HOST_ABI: src reads data_ov004_020beb60, and ov004 is not mounted. */
int func_ov004_020adc4c(void)
{
    std::printf("  [sub] func_ov004_020adc4c: ov004 is not mounted\n");
    return 0;
}

/* Debug: the bottom screen on its own, at its own size. */
void hal_sub_screen_dump(const char *path)
{
    ntr::ppu_scanout_sub(g_sub);
    ntr::ppu_write_bmp_sub(path, g_sub);
}

/* What engine B is actually configured to do, printed once. The bottom screen
   failing silently reads identically whether the registers are wrong or the
   VRAM is empty, and this is the line that tells them apart. */
void hal_sub_screen_probe(void)
{
    const unsigned dispcnt = *(volatile unsigned *)0x04001000;
    std::printf("[sub] DISPCNT_B %08x: mode %u, dispmode %u, BG %c%c%c%c OBJ %c, "
                "extpal %c, win %c%c\n",
                dispcnt, dispcnt & 7, (dispcnt >> 16) & 3,
                (dispcnt >> 8) & 1 ? '0' : '-', (dispcnt >> 9) & 1 ? '1' : '-',
                (dispcnt >> 10) & 1 ? '2' : '-', (dispcnt >> 11) & 1 ? '3' : '-',
                (dispcnt >> 12) & 1 ? 'y' : '-', (dispcnt >> 30) & 1 ? 'y' : '-',
                (dispcnt >> 13) & 1 ? '0' : '-', (dispcnt >> 14) & 1 ? '1' : '-');
    for (int i = 0; i < 4; ++i) {
        const unsigned short cnt = *(volatile unsigned short *)(0x04001008 + i * 2);
        std::printf("[sub] BG%dCNT %04x: prio %u, char %08x, screen %08x, "
                    "%s, size %u\n", i, cnt, cnt & 3,
                    0x06200000u + (((cnt & 0x3c) >> 2) << 14),
                    0x06200000u + (((cnt & 0x1f00) >> 8) << 11),
                    (cnt >> 7) & 1 ? "8bpp" : "4bpp", (cnt >> 14) & 3);
    }
    /* How much of the bottom screen's VRAM is not zero -- the difference
       between "the loaders ran" and "the loaders ran and wrote nothing". */
    static const struct { const char *n; unsigned a, len; } regions[] = {
        {"BG VRAM  ", 0x06200000u, 0x20000u},
        {"OBJ VRAM ", 0x06600000u, 0x20000u},
        {"BG pltt  ", 0x05000400u, 0x200u},
        {"OBJ pltt ", 0x05000600u, 0x200u},
        {"BG extpal", 0x06898000u, 0x8000u},
        {"sub OAM  ", 0x07000400u, 0x400u},
    };
    for (unsigned r = 0; r < sizeof regions / sizeof *regions; ++r) {
        unsigned nz = 0;
        const volatile unsigned char *p =
            (const volatile unsigned char *)regions[r].a;
        for (unsigned i = 0; i < regions[r].len; ++i)
            if (p[i]) ++nz;
        std::printf("[sub] %s %08x: %u/%u bytes nonzero\n", regions[r].n,
                    regions[r].a, nz, regions[r].len);
    }
}

}  // extern "C"
