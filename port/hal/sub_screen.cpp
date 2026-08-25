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
//
// THE PRESENT RECTANGLE lives here for the same reason the touch does. Once
// the window became resizable, "client pixel divided by zoom" stopped being
// the framebuffer pixel -- the frame is now scaled to fit whatever the client
// area is and centred inside it, with black bars on whichever axis is over. A
// stylus press that did not undo that scaling would land somewhere else on
// the bottom screen at every window size but the default, so the two halves
// of the arithmetic have to stay together. walk_window.cpp publishes the
// rectangle it presented into; poll_touch below runs it backwards. Unset (a
// binary that opens no window, or one whose present path never ran) falls
// back to the plain zoom divide, which is what every caller did before.
//
// ---- THE STACKED LAYOUT (run link60, lane DSL1) -----------------------------
//
// THE PANEL IS THE WRONG SHAPE FOR A MINIGAME AND ALWAYS WAS. It is 128x96 at
// the default divisor, in a corner, and every minigame in this game is played
// with the stylus ON the bottom screen. You cannot aim at a preview. The
// directive is Tango's and it is one sentence: for minigames the two screens
// go one above the other, the same size, because it is a touchscreen game.
//
// So this file grows a MODE. Stacked: both screens full size, top above
// bottom, SCREEN_W x 2*SCREEN_H of client area (512x768 at the 2x tier). Inset:
// what is above, unchanged, and still the default for levels, where the panel
// is doing its job and the player's hands are on the keyboard.
//
// THE MODE LIVES HERE BECAUSE THE THREE THINGS IT CHANGES ALREADY DO. The
// panel, the present rectangle and the touch transform are all in this file,
// and they are in one file precisely so a change to the geometry cannot move
// the picture without moving the aim with it. A second presenter somewhere
// else would have been a second copy of that arithmetic.
//
// WHAT IT DOES NOT TOUCH, and this is structural rather than remembered: the
// framebuffer. ntr::Framebuffer stays SCREEN_W x SCREEN_H in both modes, the
// stacked image is a SEPARATE buffer composed after everything that writes
// the framebuffer has finished with it, and every ppu_write_bmp call site in
// the tree still dumps the same 512x384 frame it dumped before. The selftest
// battery's md5 rows depend on that geometry and this mode cannot reach them.
//
// WHO TURNS IT ON. hal/scene_boot.cpp asks IsMinigameActorID -- the ROM's own
// predicate, src/IsMinigameActorID.c, 0x169..0x186, already linked and already
// the gate on the ov006 overlay constructors -- and hands the answer to
// hal_sub_screen_set_stacked. A level never asks, so a level gets the inset.
// SM64DS_DUAL_SCREEN=1 forces it on and =0 forces it off, both ways, and the
// env is read HERE so there is one place that decides.
#include <cstdio>
#include <cstdlib>

#include <windows.h>

#include "ntr/ppu.h"

#include "hal/screen_gap.h"

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
/* the present rectangle, defined at the bottom of this file */
void hal_present_set_rect(int x, int y, int w, int h, int src_w, int src_h);
int hal_present_client_to_fb(int cx, int cy, int *fx, int *fy);
int hal_present_client_to_sub(int cx, int cy, int *dsx, int *dsy);
/* the layout mode, defined at the bottom of this file */
int hal_sub_screen_stacked(void);
void hal_touch_client_probe(void);
/* the main engine's master-brightness fade (hal/fader_wipes.cpp), read so the
   stacked bottom half fades with the top half the way the inset does */
int port_fader_blend_state(int *evy, int *toWhite);
void _ZN3OAM4LoadEv(void);
unsigned int _ZN3OAM12EnableSubOAMEv(void);
int hal_oam_layout_check(void);
/* the GX bank-state band guard (hal/cxx_aliases.cpp) */
int port_gxbank_layout_check(void);
/* the sprite-template guard (hal/oam_lists.cpp): every OamAttr* the HUD and
   the Minimap hand OAM::Render, checked for a missed pointer rebase */
int hal_oam_templates_check(void);
int hal_oam_walk_probe(void);
/* the minimap's per-frame affine callback (port/unmatched/Minimap_Affine.cpp),
   the STAGE's own func_02019144 first beat, which the port cannot dispatch
   through the block because the Stage's table is hosted by nobody */
void port_minimap_affine_update(void);
/* what hal/scene_boot.cpp's beat answered this frame: 1 = run func_02019144's
   tail, 0 = the current graphics block already did the display sync itself */
int port_graph_block_verdict(void);
extern unsigned char data_0209e660;
extern unsigned char data_0209caa0[];   /* the save block; byte 8 bit 7 = intro seen */
extern signed char data_0209f2f8;       /* current level */
/* The sub engine's LAYER ENABLE MASK, bits 0..4 = BG0..BG3, OBJ. Nine
   different ROM functions publish it into DISPCNT_B bits 8-12 with the very
   same line; Minimap::Behavior and Message::UpdateWindow are two of them. It
   is the game's own switchboard for what the bottom screen shows. */
extern unsigned char data_0209d454;
/* the engine A layer mask and the eight BG-offset shadows func_02019144
   publishes; all arm9 BSS, hosted by span in hal/scene_boot.cpp's blocks */
extern unsigned char data_0209d45c;
extern short data_0209d468, data_0209d46c, data_0209d4a4, data_0209d4a0;
extern short data_0209d49c, data_0209d478, data_0209d48c, data_0209d490;
extern short data_0209d494, data_0209d498, data_0209d484, data_0209d480;
extern short data_0209d47c, data_0209d470, data_0209d474, data_0209d488;
/* TouchInfo data_020a0de8[4]: {u8 touched, u8 edge, u8 x, u8 y} per slot, in
   DS bottom-screen pixels. Stage::CheckCameraInput and every TouchArea read
   it; nothing on the host was writing it.

   BYTE +1 IS AN EDGE AND NOT A HOLD. src/func_0203bb60.c, the ROM's own
   producer, writes `p[1] = touch ^ p[0]` -- set on the frame the stylus goes
   down and again on the frame it comes up. This file called it `held` for a
   long time and wrote a held flag into it; poll_touch's store carries the
   derivation and what the wrong shape cost.

   THE FOUR NAMES ARE ONE BLOCK. dsd named a symbol at each of slot 0's four
   bytes, and readers reach the fields through whichever name they were
   decompiled with -- Message::Update reads the edge as data_020a0de9[idx*4]
   and `y` as data_020a0deb[idx*4]. hal/auto_bss.cpp hosts all four over the
   one 16-byte run so a write here reaches every one of them; the touch probe
   below is what proves it. */
extern unsigned char data_020a0de8[];   /* +0 touched */
extern unsigned char data_020a0de9[];   /* +1 edge    */
extern unsigned char data_020a0dea[];   /* +2 x       */
extern unsigned char data_020a0deb[];   /* +3 y       */
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
// The client rectangle the framebuffer was last presented into, in client
// pixels. Width zero means nobody has presented yet.
int g_pr_x, g_pr_y, g_pr_w, g_pr_h;
/* The SOURCE image that rectangle was filled from, in its own pixels. It is
   the framebuffer in inset mode and the stacked image in stacked mode, and the
   inverse transform needs it because the two are not the same shape. Zero
   means "nobody has presented yet", the same as g_pr_w. */
int g_pr_sw, g_pr_sh;

/* ---- the layout mode -------------------------------------------------------
   -1 while unresolved. The scene proposes (hal_sub_screen_set_stacked) and
   SM64DS_DUAL_SCREEN disposes; a run with no scene and no env is inset, which
   is every level. */
int g_stacked = -1;
int g_stacked_default;

int stacked_env(void)
{
    /* -1 = unset, 0 = forced off, 1 = forced on. Read once, like every other
       env in this file, so a run cannot change layout halfway through. */
    static int v = -2;
    if (v == -2) {
        const char *e = std::getenv("SM64DS_DUAL_SCREEN");
        v = e ? (std::atoi(e) ? 1 : 0) : -1;
    }
    return v;
}

BOOL(WINAPI *GetCursorPos_)(POINT *);
BOOL(WINAPI *ScreenToClient_)(HWND, POINT *);
SHORT(WINAPI *GetAsyncKeyState_)(int);
HWND(WINAPI *GetForegroundWindow_)(void);

ntr::SubFramebuffer g_sub;

int env_flag(const char *name, int dflt)
{
    const char *v = std::getenv(name);
    return v ? std::atoi(v) : dflt;
}

/* ---- SM64DS_TOUCH_PROBE: a scripted stylus, and what the four names read ---
 *
 * The stylus record is FOUR DS symbols over ONE 16-byte block -- data_020a0de8
 * .. data_020a0deb at +0/+1/+2/+3 of TouchInfo[4] (hal/auto_bss.cpp carries the
 * layout and its evidence). poll_touch writes the record through the FIRST
 * name; Message::Update reads `held` and `y` through the SECOND and FOURTH.
 * Whether those are the same bytes is a LINK-TIME property no compiler
 * diagnostic covers, and a headless run has no mouse, so without this hook the
 * question cannot be asked of a running binary at all.
 *
 *   SM64DS_TOUCH_PROBE="58-59:120:150,60,100-101:33:44,199-201"
 *
 * Comma-separated entries, the same grammar as SM64DS_PROBE_INPUT:
 *
 *   <frame>[-<frame>]:<x>:<y>   force a press at those DS pixels on those
 *                               frames, and log
 *   <frame>[-<frame>]           log only, no press
 *
 * One stderr line per listed frame:
 *
 *   [touch] f58 poke=yes(1,120,150) pre={de8=0 de9=0 dea=0 deb=0}
 *           post={de8=1 de9=1 dea=120 deb=150} d=(1,2,3) block=01017896 ...
 *
 * de9 reads 1 on the press frame and on the release frame and 0 in between,
 * because it is the ROM's touch-XOR-previous edge; see poll_touch's store.
 *
 * `pre` is what the names read on ENTRY to the poll -- which is what a
 * save-state restore left behind when the load frame is a logged frame --
 * and `post` is after the write. `d` is (&de9-&de8, &dea-&de8, &deb-&de8)
 * and MUST read (1,2,3): those three numbers ARE the bug. Hosted as separate
 * arrays they read (32,-,64) -- unrelated addresses, dea not hosted at all --
 * and de9/deb read zero on every frame forever.
 *
 * THE READS ARE VOLATILE ON PURPOSE. MSVC treats two named globals as
 * disjoint objects, so a load through data_020a0de9 in the same function as a
 * store through data_020a0de8 folds to the old value even across translation
 * units (measured: the plain read prints 0 where the volatile read prints 2).
 * The game path cannot hit that -- the one writer only stores and every
 * reader only loads -- but a probe that writes and reads in one function can,
 * and a probe that lies about the fix is worse than no probe. src/ already
 * carries the same shape: Stage::LC_Update declares data_020a0dea/deb
 * volatile.
 *
 * The probe's own state is host bookkeeping and deliberately NOT in .dsstate:
 * a save-state load must not rewind the script that is testing it. */
struct TouchProbeEnt {
    int f0, f1, x, y, poke;
};
/* 256, not 32 (run mg8, lane MMD). The parser silently STOPS at the cap, so a
   script that overruns it loses its tail without saying so. Thirty-two held
   three or four gestures; driving a minigame ROUND to its end is sixteen taps
   for one Memory Master board, eighty for the five clears that take its board
   from sixteen cards to eighteen, and a hundred and seventy for the ten that
   take it to twenty. The array is 5KB of host bookkeeping and no run reads
   past g_tp_n. */
TouchProbeEnt g_tp[256];
int g_tp_n = -1;            /* -1 = env not read yet, 0 = probe off */
int g_tp_frame;             /* polls since the first one */
int g_tp_cur = -1;          /* the frame being polled, for the camera log */

void touch_probe_parse(void)
{
    g_tp_n = 0;
    const char *s = std::getenv("SM64DS_TOUCH_PROBE");
    if (!s) return;
    while (*s && g_tp_n < (int)(sizeof g_tp / sizeof *g_tp)) {
        TouchProbeEnt e = {0, 0, 0, 0, 0};
        e.f0 = std::atoi(s);
        while (*s && *s != '-' && *s != ':' && *s != ',') ++s;
        if (*s == '-') {
            ++s;
            e.f1 = std::atoi(s);
            while (*s && *s != ':' && *s != ',') ++s;
        } else {
            e.f1 = e.f0;
        }
        if (*s == ':') {
            ++s;
            e.x = std::atoi(s);
            while (*s && *s != ':' && *s != ',') ++s;
            if (*s == ':') {
                ++s;
                e.y = std::atoi(s);
                while (*s && *s != ',') ++s;
            }
            e.poke = 1;
        }
        g_tp[g_tp_n++] = e;
        while (*s && *s != ',') ++s;
        if (*s == ',') ++s;
    }
}

const TouchProbeEnt *touch_probe_at(int f)
{
    for (int i = 0; i < g_tp_n; ++i)
        if (f >= g_tp[i].f0 && f <= g_tp[i].f1) return &g_tp[i];
    return 0;
}

/* one byte, read the way a reader in another TU reads it -- see the note on
   volatile above */
unsigned char tp_rd(const unsigned char *p, int i)
{
    return *(const volatile unsigned char *)(p + i);
}

/* The stylus, from the mouse. Byte +0 is "down now" and byte +1 is THE CHANGE
   EDGE, new XOR old -- see the block above the two stores at the bottom of
   this function for the ROM derivation and for what the old spelling cost. */
/* ---- A DRAG OWNS THE STYLUS UNTIL THE BUTTON COMES UP ---------------------
 *
 * ON A DS THE STYLUS CANNOT LEAVE THE SCREEN MID-DRAG. There is no edge to
 * fall off: the panel is the whole input device, so every game on the machine
 * was written knowing that a release is something the player DID. This port
 * gave the player four edges the hardware has none of (the letterbox bars,
 * the hinge band, the top screen, and the desktop past the window) and
 * published a RELEASE the frame the cursor crossed any of them, because the
 * surface test was what gated `down` and nothing else held it.
 *
 * WHAT THAT COST, from a live session: "when i drag off screen it releases the
 * touch". On scene 368 that is the slingshot firing itself. The pull is
 * src/func_ov006_020fe394.c and its whole body is one branch on
 * `data_020a0de8[k * 4] != 0`: the true side keeps the ball at the captured
 * offset, and the FALSE side is the shot. It writes 2 into the slot's state
 * byte at +0x4f0d and either snaps the ball home or launches it. So a hand
 * that strayed past the window while the player was pulling back did not pause
 * the pull. It took the shot.
 *
 * SO THE LATCH. The first poll of a button-hold that resolves ON the surface
 * arms `drag_own`, and from there until the PHYSICAL button comes up the
 * stylus belongs to that drag: an off-surface point publishes the nearest
 * point on the bottom screen rather than publishing nothing. Both mappers
 * already compute exactly that point and already throw it away. Each one
 * clamps its output into the screen's own range and answers inside/outside
 * separately through the return value, so this consumes an answer that was
 * being discarded rather than adding a second opinion about where the screen
 * is. Display and touch keep deriving from the one StackLayout.
 *
 * WHAT IT DOES NOT CHANGE, and each of these is a test.
 *   * A press that STARTS off the surface publishes nothing, exactly as
 *     before. The latch can only be armed by a genuinely on-surface point, so
 *     a click in a bar is still a click in a bar and THE GAP BAND'S DEAD-CLICK
 *     RULE STILL HOLDS FOR PRESSES. What crossing the band cannot do any more
 *     is kill a drag that is already running.
 *   * A drag that never leaves the surface takes the same branch with the same
 *     arithmetic: on the surface the two clamps below are no-ops.
 *   * Byte +1 is untouched. It is still `down ^ was`, so the release edge is
 *     still exactly one frame, and it now lands on the real button-up wherever
 *     the cursor is, which is the whole of what the ROM means by a release.
 *   * No top-screen coordinate is ever published for a drag. The clamp is into
 *     the BOTTOM screen's range in both layouts; there is no branch here that
 *     can reach the other band.
 */
void poll_touch(void)
{
    unsigned char down = 0, sx = 0, sy = 0;
    /* what the LIVE mouse read this poll, kept for the recorder below. `seen`
       means the button was physically down and the cursor resolved to a client
       point; `on` means that point was on the stylus surface. The pair is what
       tells "no press" apart from "a press the transform refused", and only
       the second of those is a bug worth chasing. `clamped` is the third
       answer the latch adds: off the surface, and published anyway. */
    int live_cx = 0, live_cy = 0, live_seen = 0, live_on = 0, live_clamped = 0;
    /* THE GESTURE'S OWN STATE, and a host static on purpose rather than
       anything in .dsstate: a save-state load must not restore a half-finished
       drag onto a hand that is not holding the button any more. Same argument
       the touch probe's bookkeeping makes a few lines above. */
    static int drag_own;
    /* THE PHYSICAL BUTTON, asked once, because it is now the thing that ends a
       drag and no longer merely the thing that starts one. */
    const int btn = !g_headless && g_on && GetAsyncKeyState_ &&
                    (GetAsyncKeyState_(VK_LBUTTON) & 0x8000) ? 1 : 0;
    if (!btn) drag_own = 0;
    if (btn && GetCursorPos_ && ScreenToClient_) {
        POINT p;
        if (GetCursorPos_(&p) && ScreenToClient_(g_hwnd, &p)) {
            /* THE TRANSFORM HAS TWO SHAPES NOW, one per layout, and which one
               runs is the only thing the mode changes about the touch.

               STACKED: the bottom half of the picture IS the bottom screen, so
               the mapper hands back DS pixels directly and the whole half is
               live. A click anywhere in it lands on the corresponding DS pixel,
               which is the point of the mode.

               INSET: client pixels to framebuffer pixels (the present
               rectangle, run backwards), then panel pixels to DS pixels,
               because the panel is drawn at 1/g_div in a corner. Unchanged. */
            int fx = -1, fy = -1, on_picture = 0;
            if (hal_sub_screen_stacked()) {
                on_picture =
                    hal_present_client_to_sub((int)p.x, (int)p.y, &fx, &fy);
            } else {
                int bx, by;
                /* a click in a letterbox bar is not on the panel however close
                   the clamp puts it, so the inside answer gates the press */
                on_picture =
                    hal_present_client_to_fb((int)p.x, (int)p.y, &bx, &by);
                fx = (bx - g_x0) * g_div;
                fy = (by - g_y0) * g_div;
            }
            live_cx = (int)p.x;
            live_cy = (int)p.y;
            live_seen = 1;
            const int on_surface = on_picture && fx >= 0 && fx < ntr::SUB_W &&
                                   fy >= 0 && fy < ntr::SUB_H;
            /* the arming edge, and the only one there is */
            if (on_surface) drag_own = 1;
            if (on_surface || drag_own) {
                /* THE NEAREST POINT ON THE BOTTOM SCREEN. On the surface both
                   of these are no-ops, which is why a drag that stays on the
                   screen cannot tell this branch from the one it replaces. */
                const int qx = fx < 0 ? 0
                                      : (fx >= ntr::SUB_W ? ntr::SUB_W - 1 : fx);
                const int qy = fy < 0 ? 0
                                      : (fy >= ntr::SUB_H ? ntr::SUB_H - 1 : fy);
                down = 1;
                live_on = on_surface;
                live_clamped = !on_surface;
                sx = (unsigned char)qx;
                sy = (unsigned char)qy;
            }
        } else if (drag_own) {
            /* THE QUERY FAILED, NOT THE DRAG. ScreenToClient does not fail for
               a live window, but an owned drag must not end on something that
               is not the player's hand. Ending on anything else is the exact
               defect this latch exists to close. Hold the record where it is:
               `down` stays set, and feeding the published point back through
               the two stores below leaves the last one standing. */
            down = 1;
            sx = data_020a0de8[2];
            sy = data_020a0de8[3];
        }
    }
    if (g_tp_n < 0) touch_probe_parse();
    const int f = g_tp_frame++;
    g_tp_cur = f;
    const TouchProbeEnt *tp = g_tp_n > 0 ? touch_probe_at(f) : 0;
    unsigned char pre8 = 0, pre9 = 0, prea = 0, preb = 0;
    if (tp) {
        /* what the four names read BEFORE this poll writes anything: slot 0,
           in Message::Update's own shape (data_020a0deX[idx * 4], idx = 0) */
        pre8 = tp_rd(data_020a0de8, 0);
        pre9 = tp_rd(data_020a0de9, 0);
        prea = tp_rd(data_020a0dea, 0);
        preb = tp_rd(data_020a0deb, 0);
        if (tp->poke) {
            down = 1;
            sx = (unsigned char)tp->x;
            sy = (unsigned char)tp->y;
        }
    }

    /* ---- BYTE +1 IS THE ROM'S CHANGE EDGE, NOT "HELD FOR TWO FRAMES" ------
     *
     * This is link 5 of the chain port/touch_map.txt maps, and the ROM's own
     * link 5 is src/func_0203bb60.c. Its whole body, per controller slot:
     *
     *     p[1] = (u8)(r->field_4 ^ p[0]);   the NEW touch XOR the OLD one
     *     p[0] = (u8)r->field_4;            touched now
     *     p[2] = (u8)r->field_0;            x
     *     p[3] = (u8)r->field_2;            y
     *
     * So byte +1 is 1 on exactly two frames of a press: the frame it goes
     * down and the frame it comes back up. It is a TRANSITION flag. This
     * function used to write `down && was` there, which is the opposite
     * shape -- 0 on the press frame and 1 on every frame after it -- and the
     * whole game reads that byte.
     *
     * WHAT IT COST, measured on scene 368 (Bob-omb Squad) with
     * SM64DS_TOUCH_PROBE and the shot table's own state trace. The plunger is
     * src/func_ov006_020fe2e4.c (grab) and src/func_ov006_020fe394.c (pull and
     * release). The grab arms on `data_020a0de8[k*4] && data_020a0de9[k*4]`
     * and captures ball-minus-stylus into +0x4ee8/+0x4eec; the pull then holds
     * that offset, so WHERE the stylus is on the arming frame decides where
     * the ball sits for the rest of the drag and how far down it can be
     * pulled at all.
     *
     *   press for ONE frame              old: never armed. The ROM arms on
     *                                    that frame, because that frame IS
     *                                    the edge.
     *   press, then move on the NEXT     old: armed one frame late, at the
     *   frame (a fast mouse flick)       moved-to point, so the offset came
     *                                    out (0,-110); the ball then tracked
     *                                    110 px above the cursor, never left
     *                                    the anchor, and the release measured
     *                                    dist 8 -- under 0x10, which is
     *                                    func_ov006_020fe394's snap-back, so
     *                                    the shot never fired.
     *   press, hold still, then drag     old: worked. That is the whole of
     *                                    "sometimes I can grab it".
     *
     * TWO MORE CLASSES OF READER GET THEIR BEHAVIOUR BACK WITH THIS, and they
     * are why this is a record fix rather than a pachinko fix.
     * `de8 && de9` sites (the press edge: curling's func_ov006_020e1b54,
     * Coincentration's func_ov006_020dd0e0, and two dozen more) fired one
     * frame late and then EVERY frame of the hold instead of once.
     * `de8 == 0 && de9` sites (the release edge: src/func_ov006_020d1ba0.c:63,
     * func_ov006_0212157c.c:49, func_ov006_0211134c.c:82) were identically
     * false under the old spelling -- a byte that means "down and was down"
     * cannot be set while `down` is clear -- so release detection did not
     * exist on this port at all.
     *
     * X AND Y STAY LATCHED ON RELEASE, and that is not the same shortcut.
     * The ROM writes p[2]/p[3] every frame from link 3, and link 3
     * (src/func_0203b9bc.c) only publishes its idle 0xff/0xff when NO entry in
     * the four-deep ring carries a touch. On the release-edge frame the ring
     * is still draining, so neither of its accept branches and neither the
     * idle branch runs, and data_020a0dd8 keeps the last touched point --
     * which is exactly what the `if (down)` below leaves in place. The two
     * spellings differ only once the ring has drained, by which time byte +1
     * is 0 and no reader is looking. */
    const unsigned char was = data_020a0de8[0];
    data_020a0de8[0] = down;
    data_020a0de8[1] = (unsigned char)(down ^ was);
    if (down) {
        data_020a0de8[2] = sx;
        data_020a0de8[3] = sy;
    }

    /* ---- THE STYLUS IN THE FLIGHT RECORDER (run link60, lane TCH2) ---------
     *
     * WHAT WAS MISSING WAS THE LINE, NOT THE TOUCH. Until now poll_touch wrote
     * the record and said nothing unless SM64DS_TOUCH_PROBE was set, and no
     * play session sets it. So a playlog from a real session carried no
     * evidence about the stylus AT ALL, and the only touch-shaped lines in it
     * came from walk_window's WM_LBUTTONDOWN handler -- a different path, with
     * a different question, that publishes into g_mouse_click_* which nothing
     * reads. A session where the stylus worked perfectly and a session where
     * it was never polled produced the SAME playlog, and the wrong one of the
     * two was the reading everybody took. That is what these lines close.
     *
     * THREE EDGES, not a line per frame: a press, a release, and a refusal.
     * The refusal is the one that earns its place -- the button is physically
     * down and the cursor IS inside the client area, and the transform still
     * declined it. In the stacked layout that is the ordinary, correct answer
     * for a click on the TOP half, so the line says which surface it landed on
     * rather than implying a fault. Said once per press, because a held button
     * over the top screen is one decision and not thirty a second.
     *
     * A drag logs its two ends. The path between them is the record itself and
     * SM64DS_TOUCH_PROBE prints that per frame when a lane wants it.
     *
     * QUIET WHERE IT HAS TO BE. g_headless kills the live branch above, so a
     * SM64DS_WINDOW_SELFTEST run or any windowless binary reaches this with
     * down == 0 and live_seen == 0 on every poll and prints nothing, ever. The
     * BMP battery cannot see these lines and neither can stdout: this is
     * stderr, which walk_window has already pointed at playlog/. */
    {
        static int down_was, held_from, refused_said, off_was;
        static unsigned char last_x, last_y;
        if (down && !down_was) {
            /* THE CLIENT POINT IS ONLY PRINTED WHEN THERE WAS ONE. A
               SM64DS_TOUCH_PROBE poke sets `down` with no mouse behind it, and
               printing client(0,0) for it would put a coordinate nobody
               measured into the flight recorder -- and would then be read back
               by anything checking the published DS pixel against the client
               point on its own line. Say which kind of press it was instead. */
            if (live_seen)
                std::fprintf(stderr, "[touch] f%d PRESS client(%d,%d) -> DS "
                             "(%u,%u) on the %s screen\n", f, live_cx, live_cy,
                             sx, sy,
                             hal_sub_screen_stacked() ? "bottom (stacked)"
                                                      : "bottom (inset panel)");
            else
                std::fprintf(stderr, "[touch] f%d PRESS (scripted probe, no "
                             "mouse) -> DS (%u,%u)\n", f, sx, sy);
            std::fflush(stderr);
            held_from = f;
            refused_said = 0;
            off_was = 0;
        } else if (!down && down_was) {
            std::fprintf(stderr, "[touch] f%d release after %d frame(s), last "
                         "DS (%u,%u)%s\n", f, f - held_from, last_x, last_y,
                         off_was ? ". The button came up OFF the stylus "
                                   "surface, which is where the release edge "
                                   "belongs." : "");
            std::fflush(stderr);
            refused_said = 0;
            off_was = 0;
        }
        /* BOTH ENDS OF AN EXCURSION, because the middle of one is now a thing
           that happens. A player who drags past the window edge and back gets
           two lines and keeps ONE press between them; the old build put a
           release here and a fresh press on the way back, and whatever the
           player was dragging read that release as an instruction. Two lines
           rather than one per frame, for the reason the refusal below is said
           once: a held button over the desktop is one decision, not thirty a
           second. */
        if (down && live_seen) {
            if (live_clamped && !off_was) {
                off_was = 1;
                std::fprintf(stderr, "[touch] f%d drag LEFT the stylus surface "
                             "at client(%d,%d), holding at DS (%u,%u) until "
                             "the button comes up\n",
                             f, live_cx, live_cy, sx, sy);
                std::fflush(stderr);
            } else if (!live_clamped && off_was) {
                off_was = 0;
                std::fprintf(stderr, "[touch] f%d drag back ON the stylus "
                             "surface at client(%d,%d) -> DS (%u,%u)\n",
                             f, live_cx, live_cy, sx, sy);
                std::fflush(stderr);
            }
        }
        /* THE LATCH CLEARS ON THE BUTTON, NOT ON THE TOUCH. `refused_said`
           stops a held press over the top screen printing thirty lines a
           second, and the release branch above only runs when a press was
           PUBLISHED -- so keying the reset off that alone made the first
           refused press the only one a run ever reported, and a two-point
           top-half fixture read as a one-point one. The physical button coming
           up is the edge that ends a refusal. */
        if (!live_seen)
            refused_said = 0;
        if (down) {
            last_x = sx;
            last_y = sy;
        } else if (live_seen && !live_on && !refused_said) {
            /* NOT "off-picture", and not a fault. Say where it landed. */
            int fx = 0, fy = 0;
            const int on_top = hal_present_client_to_fb(live_cx, live_cy,
                                                        &fx, &fy);
            refused_said = 1;
            std::fprintf(stderr, "[touch] f%d press client(%d,%d) is NOT on "
                         "the stylus surface: %s -- no touch published\n",
                         f, live_cx, live_cy,
                         on_top ? "it is on the TOP screen"
                                : "it is in a letterbox bar");
            std::fflush(stderr);
        }
        down_was = down;
    }

    if (tp) {
        const unsigned char *b = data_020a0de8;
        std::fprintf(stderr,
            "[touch] f%d poke=%s(%u,%u,%u) pre={de8=%u de9=%u dea=%u deb=%u} "
            "post={de8=%u de9=%u dea=%u deb=%u} d=(%d,%d,%d) "
            "block=%02x%02x%02x%02x "
            "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\n",
            f, tp->poke ? "yes" : "no", down, sx, sy,
            pre8, pre9, prea, preb,
            tp_rd(data_020a0de8, 0), tp_rd(data_020a0de9, 0),
            tp_rd(data_020a0dea, 0), tp_rd(data_020a0deb, 0),
            (int)(data_020a0de9 - data_020a0de8),
            (int)(data_020a0dea - data_020a0de8),
            (int)(data_020a0deb - data_020a0de8),
            tp_rd(b, 0), tp_rd(b, 1), tp_rd(b, 2), tp_rd(b, 3),
            tp_rd(b, 4), tp_rd(b, 5), tp_rd(b, 6), tp_rd(b, 7),
            tp_rd(b, 8), tp_rd(b, 9), tp_rd(b, 10), tp_rd(b, 11),
            tp_rd(b, 12), tp_rd(b, 13), tp_rd(b, 14), tp_rd(b, 15));
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

/* THE HALF EVERY MODE NEEDS, split out of hal_sub_screen_init so the scene
   path can have it without the half that belongs to a level.
 *
 * THE SEAM IS THE ROM'S, not a convenience. Three things live above it:
 *
 *   1. the pure-host wiring (hwnd, zoom, the env knobs, the user32 imports).
 *      No ROM equivalent; both modes need it.
 *   2. func_02053c40's display reset -- POWCNT1 bit 15 and the BG affine
 *      identity words. That function is the FOURTH call in func_0201a054,
 *      which main() calls exactly once, and nothing else in the ROM calls it.
 *      It is ONCE-PER-BOOT, before Scene::PrepareToSpawnBoot, so it precedes
 *      every scene and every level alike.
 *   3. OAM::EnableSubOAM, for a reason that is the port's and not the DS's --
 *      see the block below.
 *
 * Everything below the seam is Stage::InitResources' -- Stage::LoadGraphics2D,
 * data_0209d454 = 0x18, GXS::SetGraphicsMode(3) -- and a non-level scene must
 * not get it. dScStarSel_c::InitResources (src/func_ov003_020af8a0.c) does its
 * own sub bring-up and reaches a DIFFERENT answer at every point:
 *
 *              Stage::InitResources      dScStarSel_c::InitResources
 *   BG mode    GXS::SetGraphicsMode(3)   GXS::SetGraphicsMode(0)   line 124
 *   layers     data_0209d454 = 0x18      data_0209d454 = 0x10      line 419
 *              (BG3 minimap + OBJ)       (OBJ only; no minimap)
 *   sub assets Stage::LoadGraphics2D     its own LoadFile chain to
 *                                        0x06600000 + GXS::LoadOBJPltt
 *                                        + GXS::*LoadOBJExtPltt  lines 230-248
 *
 * Running the Stage half on the scene path would force the minimap's extended
 * affine mode onto a screen with no minimap, switch on a BG3 the scene never
 * loaded, and drop the level's 2D asset set over the sub OBJ tiles the scene
 * had just loaded for itself. Measured, before any of this changed: a scene-4
 * run already reaches DISPCNT_B 00011000 with mask 10 -- BG mode 0, display
 * mode 1, OBJ only -- which is dScStarSel_c's own line 419 and 421, arrived at
 * with no help from this file at all. The sub DISPLAY was never the thing that
 * was missing.
 *
 * Idempotent, and it has to be: the level path reaches it through
 * hal_sub_screen_init and a future caller could reach it twice. */
void hal_sub_screen_init_hw(void *hwnd, int zoom)
{
    static int armed;

    g_hwnd = (HWND)hwnd;
    g_zoom = zoom > 0 ? zoom : 1;
    g_on = env_flag("SM64DS_SUB_PANEL", 1) != 0;
    /* NO WINDOW, NO INTERACTIVE KEYBOARD -- and this clause is here because
       splitting the function created the case that needs it.

       Two behaviours that are each correct compose into a wrong one the first
       time this runs on a windowless path. The focus gate FAILS OPEN on a null
       hwnd on purpose (hal_window_focused returns 1, so a binary that opens no
       window is never locked out of its own input), and the TAB latch in
       hal_sub_screen_frame_begin is dead only while GetAsyncKeyState_ is null.
       A scene run passes hwnd == nullptr and now loads user32 anyway, so
       without this the pointer goes live, the gate says yes, and a TAB held
       anywhere on the machine toggles the panel in the middle of an automated
       run and changes the BMP it writes.

       That is the exact non-determinism g_headless was added for, so a null
       hwnd carries it for the same reason SM64DS_WINDOW_SELFTEST does. The
       level path passes a real hwnd and is unaffected. */
    g_headless = std::getenv("SM64DS_WINDOW_SELFTEST") != 0 || hwnd == nullptr;
    g_nofocusgate = std::getenv("SM64DS_INPUT_NOFOCUSGATE") != 0;

    if (HMODULE u = LoadLibraryA("user32.dll")) {
        GetCursorPos_ = (decltype(GetCursorPos_))GetProcAddress(u, "GetCursorPos");
        ScreenToClient_ = (decltype(ScreenToClient_))GetProcAddress(u, "ScreenToClient");
        GetAsyncKeyState_ =
            (decltype(GetAsyncKeyState_))GetProcAddress(u, "GetAsyncKeyState");
        GetForegroundWindow_ =
            (decltype(GetForegroundWindow_))GetProcAddress(u, "GetForegroundWindow");
    }

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

       THIS IS FOUR OF THE EIGHT WORDS func_02053c40 WRITES, and the comment
       used to claim both engines. It does not: every address below is
       0x040010xx, the SUB engine. The ROM also seeds the main engine's four
       at 0x04000020 / 26 / 30 / 36, and the port does not. Sub BG2 is covered
       here so a later affine BG2 on THIS engine does not land on the same
       puzzle; main BG2 and BG3 are still unseeded, which is a real gap and
       deliberately not closed from this line -- writing main-engine display
       registers during bring-up can move the level frame, and that is a
       measurement somebody has to make rather than a word somebody changes. */
    *(volatile unsigned short *)0x04001020 = 0x100;   /* BG2PA sub */
    *(volatile unsigned short *)0x04001026 = 0x100;   /* BG2PD sub */
    *(volatile unsigned short *)0x04001030 = 0x100;   /* BG3PA sub */
    *(volatile unsigned short *)0x04001036 = 0x100;   /* BG3PD sub */

    /* DUAL OAM, and this is the one line the bottom screen actually turns on.
     *
     * WHAT data_0209e660 IS. config/arm9/symbols.txt has it kind:bss, so the
     * DS boots it to ZERO, and zero is the two-engine value:
     *
     *     OAM::Load    e660 != 0 -> func_020566dc(e674) and RETURN.  That is
     *                               0x07000000, engine A, alone.
     *                  e660 == 0 -> func_020566dc + func_02056674, which is
     *                               0x07000000 AND 0x07000400 -- both engines,
     *                               with POWCNT1 bit 15 choosing which shadow
     *                               lands on which.
     *     OAM::Render  `!sub || data_0209e660 == 1` sends every sub==true
     *                  sprite into the MAIN shadow instead.
     *
     * So on the DS the bottom screen is live from boot and only ov006's
     * minigames ever set the flag (func_ov006_020e6cac, func_ov006_020e7124),
     * clearing it again on the way out.
     *
     * WHY THE PORT NEEDS A CALL AT ALL. hal/model_host.cpp:91 hosts the global
     * as `unsigned char data_0209e660 = 1;` -- deliberately, and NOT the DS's
     * boot value. That pin is a port-side default, and while it stands every
     * sub sprite is redirected into the main shadow and engine B's OAM is
     * never uploaded. The level path has always covered it by calling
     * EnableSubOAM here; a scene run reached neither, which is the whole
     * defect. OAM::EnableSubOAM is the ROM's own name for `e660 = 0`
     * (src/_ZN3OAM12EnableSubOAMEv.c is that one store and nothing else), so
     * calling it is how the port spells "restore the DS's boot state" without
     * inventing a write.
     *
     * THE PIN ITSELF IS NOT THIS LANE'S TO PULL. Clearing e660's initialiser
     * in model_host.cpp would be the ROM-faithful fix and it is that file's
     * call: port/tests/smoke_oam.cpp:42 asserts `data_0209e660 == 1` as the
     * HAL's documented direct-fill mode, and the layout guard below exists
     * because the alternative mode needs the three shadow symbols adjacent.
     * ROUTED, not fixed here.
     *
     * Guarded on the layout check for the original reason: in mode 0 the Reset
     * path fills the shadow through data_0209e67c/data_0209e694, and if those
     * are separate host arrays it would leave 127 of 128 entries as garbage
     * and draw the heap onto the bottom screen. */
    /* The OTHER grouped-section band, checked in the same breath and for the
       same reason. hal/cxx_aliases.cpp lays the GX bank state out in ROM order
       so the SetBankFor* family's member writes land on the words
       func_020540f0 and its siblings read by name; if that stops holding,
       GXS::EndLoadOBJExtPltt restores bank 0 and takes DISPCNT_B bit 31 with
       it, and the sub screen's 256-colour sprites quietly lose their extended
       palette. Checked on BOTH paths because this is the shared half.

       THE RETURN IS LOAD-BEARING, the way the sibling's below is, and it was
       not on review. A bare call put the failure into one line of a run's
       stdout and then carried straight on into the silent-zero behaviour the
       band exists to remove: no fault, no red, and a bottom screen that is
       merely wrong. Nothing here can repair a layout at run time, so there is
       no fallback to take -- what the branch owes is to say what is now untrue
       and to stop a proof run from reporting green over it.
       SM64DS_FAULTS_FATAL is the battery's own switch (port/tools/battery.py
       sets it on every level and scene step), so a regression lands as a
       failed step instead of as scrollback. */
    if (!port_gxbank_layout_check()) {
        std::printf("[gx] the SetBankFor* family is writing past its own "
                    "state: DISPCNT_B bits 30 and 31 will not survive a "
                    "Begin/EndLoad*ExtPltt pair, so sub extended palettes are "
                    "off and neighbouring hosted globals are being clobbered\n");
        if (std::getenv("SM64DS_FAULTS_FATAL")) {
            /* FLUSH STDOUT BEFORE THE ABORT. std::abort does not flush stdio,
               and walk_window's stdout is a pipe in every automated run, so
               the two lines above exist only in a buffer at this point. The
               first cut of this branch aborted with them still in it: nonzero
               exit, empty stdout, a quieter failure than the one it replaced.
               Measured with the band deliberately mis-aligned -- zero bytes of
               stdout before this flush, both lines after.
               The stderr line was never at risk and is not why the flush is
               here: walk_window reopens stderr onto playlog/play_*.log, so it
               lands there either way, which is also where to look for it. */
            std::fprintf(stderr, "  (SM64DS_FAULTS_FATAL: GX bank band not "
                                 "contiguous -> hard abort)\n");
            std::fflush(stdout);
            std::fflush(stderr);
            std::abort();
        }
    }

    if (hal_oam_layout_check()) {
        _ZN3OAM12EnableSubOAMEv();
        std::printf("[sub] dual OAM armed (data_0209e660 = %u)\n",
                    data_0209e660);
    } else {
        std::printf("[sub] dual OAM NOT armed: sub sprites stay on the main "
                    "shadow\n");
    }
    OAM::Reset();

    std::printf("[sub] panel %s (TAB toggles, SM64DS_SUB_PANEL=0 to start "
                "off)\n", g_on ? "on" : "off");
    if (armed) {
        std::printf("[sub] hw bring-up was already armed; re-armed idempotently"
                    "\n");
    }
    armed = 1;
}

/* The LEVEL path's bring-up: the shared half, then Stage::InitResources' own
   sub-screen configuration. Call site and call order are unchanged, so the
   level frame sees the identical sequence of writes it always did. */
void hal_sub_screen_init(void *hwnd, int zoom)
{
    hal_sub_screen_init_hw(hwnd, zoom);

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
       reads as already-held, so it cannot toggle the panel on arrival.

       IT IS INERT IN THE STACKED LAYOUT. TAB hides the corner panel because
       the panel is furniture over the game; the stacked bottom half IS the
       game, and a key that blanked half the window and took the stylus with
       it (poll_touch is gated on g_on) would be a way to lose a minigame by
       leaning on the keyboard. The window size is fixed at creation for the
       same reason, so there is nothing for a toggle to fall back to. */
    static int tab_was;
    if (GetAsyncKeyState_ && !g_headless && !hal_sub_screen_stacked()) {
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
    /* Inert unless SM64DS_TOUCH_CLIENT_PROBE is set, and here rather than
       inside poll_touch because it wants a present rectangle the frame loop
       has actually published. */
    hal_touch_client_probe();
}

/* ---- THE DISPLAY-ROUTING PROBE, default off --------------------------------
 *
 * SM64DS_SCREENS_PROBE=1 prints the registers that decide WHICH ENGINE'S OUTPUT
 * LANDS ON WHICH PHYSICAL SCREEN, and it prints them the only way a
 * per-frame-toggling register can honestly be reported: once at the first
 * frame, again on every CHANGE to the tuple, and a census at the end of the
 * run. A sample taken at one frame of a scene whose slot 24 flips the bit every
 * frame is not a measurement of anything.
 *
 * The four registers, all GBATEK:
 *   POWCNT1   0x04000304  bit 15 = display swap (1 = engine A to the UPPER
 *                         screen, 0 = to the lower one)
 *   DISPCAPCNT 0x04000064 the display capture unit: bit 31 enable (the
 *                         hardware clears it itself at the end of the captured
 *                         frame), bits 16-17 destination VRAM block, 18-19
 *                         destination offset in 0x8000-byte units, 20-21 size
 *                         and 29-30 source
 *   DISPCNT_B 0x04001000  engine B's mode and layer enables, which is what says
 *                         whether the captured frame has a layer to show
 *                         through
 *   BG2CNT_B  0x0400100c  the bitmap BG's own word when it has one
 *
 * NOTHING HERE CHANGES A PIXEL. It reads four registers and writes stderr.
 */
void hal_screens_probe(void)
{
    static int on = -1;
    if (on < 0) on = env_flag("SM64DS_SCREENS_PROBE", 0);
    if (!on) return;
    static unsigned long frame;
    static unsigned long swaps;      /* frames whose bit 15 differed from the last */
    static unsigned long lower;      /* frames with engine A on the lower screen */
    static unsigned long cap_armed;  /* frames that entered with capture enabled */
    static unsigned last_key = ~0u;
    static int last_bit = -1;

    const unsigned pow1 = *(volatile unsigned short *)0x04000304;
    const unsigned cap = *(volatile unsigned *)0x04000064;
    const unsigned dispb = *(volatile unsigned *)0x04001000;
    const unsigned bg2b = *(volatile unsigned short *)0x0400100c;
    const int bit = (pow1 >> 15) & 1;

    ++frame;
    if (!bit) ++lower;
    if (cap & 0x80000000u) ++cap_armed;
    if (last_bit >= 0 && bit != last_bit) ++swaps;
    last_bit = bit;

    /* the two banks the capture path moves, C and D, and their VRAMCNT bytes:
       bit 7 enable, bits 0-2 MST, bits 3-4 OFS. 0x84 is the value both arms
       write -- enabled, MST 4, which is engine B BG for C and engine B OBJ for
       D -- and 0x80 is Vram__Map's "back to LCDC". */
    const unsigned vc = *(volatile unsigned char *)0x04000242;
    const unsigned vd = *(volatile unsigned char *)0x04000243;

    const unsigned key = (unsigned)bit | (cap & 0xF0FF0000u) |
                         ((dispb & 0x1F07u) << 1) | (vc << 20) | (vd << 12);
    if (key != last_key) {
        last_key = key;
        std::fprintf(stderr,
                     "  [screens] f%-6lu POWCNT1 %04x (engine A -> %s) "
                     "DISPCAPCNT %08x (%s block %u ofs %u size %u src %u) "
                     "DISPCNT_B %08x mode %u BG %c%c%c%c OBJ %c BG2CNT_B %04x "
                     "VRAMCNT_C %02x VRAMCNT_D %02x\n",
                     frame - 1, pow1, bit ? "UPPER" : "LOWER", cap,
                     (cap & 0x80000000u) ? "ARMED" : "idle",
                     (cap >> 16) & 3, (cap >> 18) & 3, (cap >> 20) & 3,
                     (cap >> 29) & 3, dispb, dispb & 7,
                     (dispb & 0x0100) ? '0' : '-', (dispb & 0x0200) ? '1' : '-',
                     (dispb & 0x0400) ? '2' : '-', (dispb & 0x0800) ? '3' : '-',
                     (dispb & 0x1000) ? 'y' : '-', bg2b, vc, vd);
        std::fflush(stderr);
    }
    /* the census, every 300 frames: the numbers a per-frame flip is actually
       read off, which no single sample can carry */
    if (frame % 300 == 0) {
        /* AND THE FOUR BG-OFFSET SHADOWS WITH IT. func_02019144's tail is the
           only thing that publishes them, and its FIRST beat is the graphics
           block's slot 2 -- so with the block unseated the tail republished
           frozen boot values every frame while the scene's scroll word moved.
           Sampling them beside the display registers is what makes "the
           offsets follow the game" a reading rather than an inference: on a
           scrolling scene these move between censuses and on a still one they
           do not. */
        std::fprintf(stderr,
                     "  [screens] census f%lu: %lu frame(s) with engine A on "
                     "the LOWER screen, %lu bit-15 change(s), %lu frame(s) "
                     "entered with DISPCAPCNT armed; BG2 offsets A %u,%u "
                     "B %u,%u\n",
                     frame, lower, swaps, cap_armed,
                     *(volatile unsigned short *)0x04000018,
                     *(volatile unsigned short *)0x0400001a,
                     *(volatile unsigned short *)0x04001018,
                     *(volatile unsigned short *)0x0400101a);
        std::fflush(stderr);
    }
}

/* ---- THE OBJ/POWCNT1 PARITY PROBE, default off -----------------------------
 *
 * SM64DS_OBJ_PARITY=1 prints, once a frame, the three facts a claim about WHICH
 * SCREEN A SPRITE LANDED ON has to be made of, all sampled at the one moment in
 * the frame where all three are simultaneously true:
 *
 *   POWCNT1 bit 15   the arm the engine-A OBJ raster ran under THIS frame. The
 *                    raster is in hal/message_compositor.cpp, it ran a few
 *                    statements before this function was entered, and
 *                    func_02019144's slot-24 beat -- which writes this register
 *                    -- ran immediately before it. So this is the arm that
 *                    raster used, not a later one.
 *   HW OAM           0x07000000, which is what that raster read: the shadow
 *                    OAM::Load uploaded at the END of the PREVIOUS frame.
 *   THE SHADOW       data_0209e674 / data_0209e664, which is what the game
 *                    wrote during THIS frame's Render and what OAM::Load is
 *                    about to upload at the foot of this function.
 *
 * AND THE FRAMEWORK'S sel WORD, data_ov004_020beb68 + 0x4664. The slot-24 beat
 * has already flipped it by this point, so the value printed is the one the
 * raster's arm was derived from, and the shadow beside it was written under the
 * OTHER one. That is the whole measurement: on the dScMgD3DBase_c family the
 * ROM's own routers pick a sprite's screen mapping off sel, so a shadow written
 * under sel=s belongs to the arm the NEXT beat sets, and printing both says
 * whether this program pairs them the way the DS does.
 *
 * NOTHING HERE CHANGES A PIXEL. It reads two register words and two tables. */
extern "C" {
extern int data_0209e664;                    /* the main shadow's counter */
extern unsigned short data_0209e674[];       /* the main shadow itself */
extern unsigned char data_ov004_020beb68[4]; /* the framework's scene pointer */
}
/* which frame order this binary is running; defined with the upload below */
static int oam_load_late(void);
void hal_obj_parity_probe(void)
{
    static int on = -1;
    if (on < 0) on = env_flag("SM64DS_OBJ_PARITY", 0);
    if (!on) return;
    static int upto = -1;
    if (upto < 0) upto = env_flag("SM64DS_OBJ_PARITY_FRAMES", 40);
    static unsigned long frame;
    const unsigned long f = frame++;
    if ((long)f >= (long)upto) return;

    const int bit = (*(volatile unsigned short *)0x04000304 >> 15) & 1;
    unsigned g = (unsigned)data_ov004_020beb68[0]
               | ((unsigned)data_ov004_020beb68[1] << 8)
               | ((unsigned)data_ov004_020beb68[2] << 16)
               | ((unsigned)data_ov004_020beb68[3] << 24);
    int sel = -1;
    if (g) sel = (int)*(unsigned short *)(g + 0x4664);

    /* PARKED ENTRIES ARE NOT SPRITES. OAM::Reset leaves the unused slots at
       y = 0xc0 with tile 0, which is one row past the bottom of a 192-row
       screen and draws nothing; counting them would bury the handful of real
       entries under 116 blanks. Reported separately rather than dropped. */
    char hw[512];
    int hn = 0, hc = 0, hpark = 0;
    hw[0] = 0;
    for (int i = 0; i < 128; ++i) {
        const unsigned short a0 = *(volatile unsigned short *)(0x07000000u + i * 8u);
        const unsigned short a1 = *(volatile unsigned short *)(0x07000000u + i * 8u + 2);
        const unsigned short a2 = *(volatile unsigned short *)(0x07000000u + i * 8u + 4);
        if (!(a0 & 0x100) && (a0 & 0x200)) continue;   /* disabled */
        if (!a0 && !a1 && !a2) continue;
        if ((a0 & 0xFF) == 0xC0 && !a1 && !a2) { ++hpark; continue; }
        ++hc;
        if ((size_t)hn < sizeof hw - 32)
            hn += std::snprintf(hw + hn, sizeof hw - (size_t)hn,
                                " %d:(%d,%d,t%u)", i, (int)(a1 & 0x1FF),
                                (int)(a0 & 0xFF), (unsigned)(a2 & 0x3FF));
    }

    char sh[512];
    int sn = 0;
    sh[0] = 0;
    const int scount = data_0209e664;
    for (int i = 0; i < scount && i < 128; ++i) {
        const unsigned short a0 = data_0209e674[i * 4 + 0];
        const unsigned short a1 = data_0209e674[i * 4 + 1];
        const unsigned short a2 = data_0209e674[i * 4 + 2];
        if ((size_t)sn < sizeof sh - 32)
            sn += std::snprintf(sh + sn, sizeof sh - (size_t)sn,
                                " %d:(%d,%d,t%u)", i, (int)(a1 & 0x1FF),
                                (int)(a0 & 0xFF), (unsigned)(a2 & 0x3FF));
    }

    /* WHICH sel THE DISPLAYED BLOCK WAS SUBMITTED UNDER, and it depends on
       where the upload is. With the ROM-ordered upload the hardware OAM IS the
       shadow this frame's Render just wrote, and that Render ran before the
       beat, so it saw the PREVIOUS sel. With the late upload the hardware OAM
       is the block written one frame earlier still, which on a period-2
       alternation is the sel the beat has just set -- the defect. */
    const int shadow_sel = sel < 0 ? -1 : 1 - sel;
    const int shown_sel = oam_load_late() ? sel : shadow_sel;
    std::fprintf(stderr,
                 "[objparity] f%-4lu arm=%s sel=%d upload=%s | DISPLAYED "
                 "(hw oam, %d live + %d parked, submitted under sel=%d):%s | "
                 "SHADOW (written this frame under sel=%d, %d entr(y|ies)):%s\n",
                 f, bit ? "UPPER" : "LOWER", sel,
                 oam_load_late() ? "LATE" : "ROM", hc, hpark, shown_sel,
                 hw[0] ? hw : " -", shadow_sel, scount,
                 sh[0] ? sh : " -");
    std::fflush(stderr);
}

/* ---- THE OBJ/POWCNT1 PARITY: func_02019144's OAM UPLOAD, AT THE ROM'S PLACE --
 *
 * WHAT WAS WRONG, and it is a frame-order fact about this program rather than
 * anything in the game's code.
 *
 * src/func_02019144.c is the DS's once-per-frame display sync and its order is
 * fixed: dispatch the current graphics block's slot 2 (which for every minigame
 * reaches the scene's slot 24, and for the dScMgD3DBase_c family that slot is
 * what writes POWCNT1's display-swap bit), then OAM::Flush, then OAM::Load,
 * then the two DISPCNT layer-mask publishes, then the eight BG offsets. It runs
 * in VBLANK, so the frame it configures is scanned out AFTER it: the sprites on
 * screen during a frame are the ones the upload at the head of that frame put
 * in the hardware OAM, and the game wrote them during the frame BEFORE.
 *
 * This program ran the beat and the engine-A publish at the head of the display
 * path (hal/message_compositor.cpp) and then rasterised engine A's OBJ -- but
 * the upload was at the FOOT of this function, below both rasters. So both
 * screens drew from upload N-1 while the arm they were placed under came from
 * beat N, and the game code that wrote upload N-1 had run one beat earlier
 * still. Two beats between the submission and the arm; on a family whose arm
 * alternates EVERY frame, two beats is the same parity as none, and every
 * sprite landed on the opposite screen from the one the ROM puts it on.
 *
 * THE ROM SAYS WHICH SCREEN, three independent ways, all in matched src:
 *
 *   src/Hud_RenderSprite.cpp        its dScMgD3DBase_c arm draws ONLY while
 *                                   sel (scene + 0x4664) is 0, and its ordinary
 *                                   arm is OAM::RenderSub -- the SUB engine,
 *                                   which is the bottom screen.
 *   src/RenderOamMainScreen.cpp     its D3D arm draws ONLY while sel is 1, and
 *                                   its ordinary arm is OAM::Render(draw=0) --
 *                                   the MAIN engine, the top screen.
 *   src/RenderOamBothScreens.cpp    its D3D arm submits at y + 0xc0 + G while
 *                                   sel is 1 and at y while sel is 0, and 0xc0
 *                                   + G is exactly the offset its ordinary arm
 *                                   uses for the TOP screen's copy.
 *
 * All three agree: a submission made while sel == 1 belongs on the top screen
 * and one made while sel == 0 belongs on the bottom. src/func_ov006_020e6e78.c
 * -- slot 24 -- sets POWCNT1 the other way round on the same frame (sel == 1
 * clears bit 15, which sends engine A to the LOWER screen), so on the DS a
 * submission is displayed under the arm the NEXT beat sets. That is not a
 * coincidence to be worked around; it is the double buffering, and putting the
 * upload back where func_02019144 has it reproduces it exactly.
 *
 * MEASURED BOTH WAYS with SM64DS_OBJ_PARITY=1 above, scene 372.
 *
 * SM64DS_OAM_LOAD_LATE=1 puts the old foot-of-the-frame upload back on this
 * same binary, so a before/after is one build and one asset base -- which is
 * what notes/port-selftest-bmp-gate.md requires before two captures may be
 * compared at all.
 *
 * WHAT DOES NOT CHANGE. Both engines still read ONE upload, which is the whole
 * of the split-sprite cure the late position was reached for: the two halves of
 * a sprite straddling the gapless seam still draw from the same block, it is
 * simply the block the ROM would have them draw from. OAM::Flush is NOT added
 * here -- it is CP15 cache maintenance (src/_ZN3OAM5FlushEv.c) with nothing to
 * do on a host, and this program has never called it. */
static int oam_load_late(void)
{
    static int v = -1;
    if (v < 0) v = env_flag("SM64DS_OAM_LOAD_LATE", 0);
    return v;
}

/* Called from hal/message_compositor.cpp between func_02019144's slot-2 beat
   and its engine-A layer publish, which is the line the ROM has it on, and
   therefore ahead of both OBJ rasters. The caller has already asked the beat
   for its verdict and only calls this when the beat said to run the tail. */
extern "C" void port_frame_oam_upload(void)
{
    if (oam_load_late()) return;
    _ZN3OAM4LoadEv();
    ntr::ppu_seam_oam_mark_uploaded();
}

/* Bottom of the frame: upload the shadows the game filled, rasterise engine B,
   drop it into the corner. With the panel off nothing here writes a pixel. */
void hal_sub_screen_present(unsigned int *dst, int w, int h)
{
    hal_screens_probe();
    hal_obj_parity_probe();
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
    /* THE TAIL IS CONDITIONAL NOW, and the condition is the ROM's own.
       func_02019144 asks the current graphics block's slot 2 first and RETURNS
       before any of this when it answers 0. hal/message_compositor.cpp runs
       that beat at the head of the engine A display path; this reads the
       answer rather than asking again, because the block's slot 2 is a
       once-per-frame callback and calling it twice would run a scene's whole
       display sync twice. Both frame loops (hal/scene_boot.cpp's
       port_scene_run and walk_window's own) call the engine A compositor
       immediately before this function, so the answer is this frame's.
       Measured by run link60 Stage 5 lane TS1: with the tail unconditional the
       port published a zero mask over the title screen's own layer enables
       every frame and OAM::Load re-parked the sprites the scene had just
       uploaded. port/ov007_seat.txt 5h. */
    const int run_tail = port_graph_block_verdict();
    if (run_tail) {
        static int forced = -2;
        if (forced == -2) forced = env_flag("SM64DS_SUB_LAYERS", -1);
        const unsigned mask =
            forced >= 0 ? (unsigned)forced : (unsigned)data_0209d454;
        *(volatile unsigned *)0x04001000 =
            (*(volatile unsigned *)0x04001000 & ~0x1f00u) | (mask << 8);
    }
    /* func_02019144's FIRST beat, for the one block the port cannot dispatch:
       the Stage's. Its table (data_02092188) is hosted by nobody, so
       hal/scene_boot.cpp's beat refuses it and answers 1, and this hand copy
       of Stage::GraphCallback2 is what stands in. It runs exactly when the
       beat did NOT dispatch a real block, which is every level frame and
       leaves the 46-level net where it was. port/unmatched/Minimap_Affine.cpp
       carries the callback; the rest of func_02019144 is the layer-mask
       publish above and the OAM upload below. */
    if (run_tail) {
        port_minimap_affine_update();
        /* IMMEDIATELY BEFORE THE UPLOAD, and that placement is the whole point.
           port_message_composite_engine_a ran a few lines earlier in both frame
           loops and rasterised engine A's sprites out of 0x07000000 as it
           stands RIGHT NOW -- the previous frame's upload. The line below
           replaces that with this frame's, so from here on the top screen in
           the framebuffer and engine A's OAM are one frame apart. The gapless
           seam pass has to know which of the two the picture was drawn from
           before it may complete an object across the seam, so it is handed the
           answer here rather than left to infer it from a compose count. See
           ntr/ppu.h's ppu_seam_oam_mark and the note in ntr/ppu_sub.cpp.
           INSIDE the run_tail branch because that is what makes it true: with
           no upload the OAM does not change, and a mark taken anyway would
           advance a snapshot the picture had not moved past. */
        /* THE UPLOAD MOVED BELOW THE SCANOUT, and that one reorder is the
           whole cure for the split-sprite stutter the owner filmed: the top
           screen's rasters run before this function and read the OAM the
           PREVIOUS Load uploaded, while the bottom screen used to scan out
           after THIS frame's Load -- one frame apart, so a sprite straddling
           the gapless seam ticked on alternate halves. Both banks upload in
           one Load, so scanning the bottom first puts both screens on the
           same upload, every scene, with no source-swapping and no special
           case. The seam mark stays glued to the Load it describes. */

        /* func_02019144's EIGHT BG-OFFSET PUBLISHES, the last of its beats the
           port was still skipping. SetBgNOffset (src) writes SHADOW words, and
           on the DS this once-per-frame sync is the only thing that carries
           them into the hardware registers. hal/message_pump.cpp copies the
           engine A BG3 line while a message box is up; nothing copied any of
           them the rest of the time, so a scene that scrolls through the ROM's
           own setters scrolled a register that never moved. Scene 368 is the
           measured case: dScMgPachinko_c's pan calls SetBg2Offset every frame,
           the hardware BG2 offset stayed at the one raw init write forever,
           and the Bob-omb Squad airship hull (map rows 0..3, loaded and
           stamped correctly) sat above a window that never scrolled up to it
           -- reported as "the screen is just not showing the top".
           Register order and the BG0-A 3D branch are func_02019144's own,
           lines 46..61, and it runs under the same run_tail verdict the ROM
           gates it with. */
        *(volatile unsigned *)0x04000000 =
            (*(volatile unsigned *)0x04000000 & ~0x1f00u)
            | ((unsigned)data_0209d45c << 8);
        if (!(*(volatile unsigned *)0x04000000 & 8)) {
            *(volatile unsigned *)0x04000010 =
                (data_0209d468 & 0x1ff) | (0x1ff0000u & (data_0209d46c << 16));
        }
        /* The else branch is func_02055454(data_0209d468): with BG0 as the 3D
           plane the ROM routes the X shadow to the 3D scroll word instead of
           the register. That TU is not in this link and the port's 3D path
           has never read the word it writes, so the branch is left unseated
           rather than half-seated; a level frame behaves exactly as before
           this publish existed. */
        *(volatile unsigned *)0x04000014 =
            (data_0209d4a4 & 0x1ff) | (0x1ff0000u & (data_0209d4a0 << 16));
        *(volatile unsigned *)0x04000018 =
            (data_0209d49c & 0x1ff) | (0x1ff0000u & (data_0209d478 << 16));
        *(volatile unsigned *)0x0400001c =
            (data_0209d48c & 0x1ff) | (0x1ff0000u & (data_0209d490 << 16));
        *(volatile unsigned *)0x04001010 =
            (data_0209d494 & 0x1ff) | (0x1ff0000u & (data_0209d498 << 16));
        *(volatile unsigned *)0x04001014 =
            (data_0209d484 & 0x1ff) | (0x1ff0000u & (data_0209d480 << 16));
        *(volatile unsigned *)0x04001018 =
            (data_0209d47c & 0x1ff) | (0x1ff0000u & (data_0209d470 << 16));
        *(volatile unsigned *)0x0400101c =
            (data_0209d474 & 0x1ff) | (0x1ff0000u & (data_0209d488 << 16));
    }
    if (run_tail) {
        /* nothing scanned when the panel is off, but the upload and its mark
           still have to happen -- they are the frame's, not the panel's */
    }
    /* ---- THE DISPLAY CAPTURE UNIT, in the hardware's own order --------------
     *
     * PUBLISH FIRST, then capture, then rasterise engine B. That is the order
     * the DS runs them in and each step depends on the one before it: the VRAM
     * bank mapping is programmed in VBlank (which for this program is
     * func_02019144's slot-24 beat, several statements ago), the capture happens
     * during the frame that mapping applies to, and the scan-out below reads
     * whatever the mapping made visible.
     *
     * `dst` is engine A's FINISHED framebuffer -- gx_render and
     * port_message_composite_engine_a both ran on it before this function was
     * called, in both frame loops -- and it is the composed graphics screen
     * DISPCAPCNT's source A names. Read, never written: the capture writes VRAM.
     *
     * BOTH ARE INERT UNTIL A GAME PROGRAMS THE UNIT. ppu_display_capture returns
     * on its first test when DISPCAPCNT's enable bit is clear, and
     * ppu_vram_publish skips every block that has never been captured into,
     * which is all four in every scene that never writes 0x04000064. */
    ntr::ppu_vram_publish();
    ntr::ppu_display_capture(dst, w, h);

    if (g_on) ntr::ppu_scanout_sub(g_sub);
    /* THE LATE UPLOAD, and it is now the OPT-IN arm. port_frame_oam_upload
       above already ran this frame's OAM::Load at func_02019144's own line,
       ahead of both rasters; this block is what SM64DS_OAM_LOAD_LATE=1 gets
       back, unchanged, so the two frame orders are one binary apart. */
    if (run_tail && oam_load_late()) {
        ntr::ppu_seam_oam_mark();
        _ZN3OAM4LoadEv();
    }
    if (!g_on) return;
    /* THE STACKED LAYOUT WRITES NOTHING INTO dst HERE, and that is the whole
       trick. dst is the framebuffer, the scan-out above is everything the
       bottom screen needs, and the stacked image is built downstream by
       hal_sub_screen_compose_stacked once the caller has finished with the
       framebuffer -- after its fade composite and after its debug overlay. So
       the framebuffer this function is handed comes out of a stacked frame
       byte-for-byte identical to a panel-off frame, which is what keeps every
       ppu_write_bmp site in the tree at 512x384 and unmoved. */
    if (!hal_sub_screen_stacked())
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

/* ---- the stacked layout ----------------------------------------------------
 *
 * THE MODE IS A PROPOSAL AND AN OVERRIDE. hal/scene_boot.cpp proposes, out of
 * the ROM's own IsMinigameActorID; SM64DS_DUAL_SCREEN overrides in either
 * direction. Nothing proposes on a level path, so a level is inset unless the
 * env says otherwise, which is Tango's directive read literally: the corner
 * panel is doing its job during a level and the complaint was about minigames.
 *
 * IT LATCHES ON FIRST READ. hal_sub_screen_stacked is called from the frame
 * loop, from the touch poll and from walk_window's window sizing, and a mode
 * that could change after the window has been created would leave a window
 * the wrong shape for the picture in it. The setter refuses once the answer
 * has been handed out, loudly, because a late set means the call order is
 * wrong somewhere and silently keeping the old answer would hide that.
 */
static int g_stacked_latched;

int hal_sub_screen_stacked(void)
{
    if (g_stacked < 0) {
        const int e = stacked_env();
        g_stacked = e >= 0 ? e : (g_stacked_default ? 1 : 0);
        /* Say it once, on the run that decided it, and say WHERE the answer
           came from. "The panel is in the corner" and "the panel is in the
           corner because nothing proposed otherwise" are different findings
           when a minigame comes up in the wrong layout. */
        std::printf("[sub] layout: %s (default %s, %s)\n",
                    g_stacked ? "STACKED, both DS screens full size"
                              : "corner inset panel",
                    g_stacked_default ? "stacked" : "inset",
                    e < 0 ? "SM64DS_DUAL_SCREEN unset"
                          : (e ? "SM64DS_DUAL_SCREEN=1 forces it on"
                               : "SM64DS_DUAL_SCREEN=0 forces it off"));
        std::fflush(stdout);
    }
    g_stacked_latched = 1;
    return g_stacked;
}

void hal_sub_screen_set_stacked(int on)
{
    if (g_stacked_latched) {
        std::fprintf(stderr, "  [sub] stacked layout already latched at %d; "
                     "the request for %d is IGNORED (it arrived after the "
                     "first reader)\n", g_stacked, on ? 1 : 0);
        return;
    }
    g_stacked_default = on ? 1 : 0;
}

/* Build the stacked image and hand back a pointer to it. `top` is the FINISHED
 * framebuffer -- faded, overlaid, everything -- and the return is
 * ntr::STACK_W x ntr::STACK_H, or null when the mode is off or the bottom
 * screen has never been scanned out. This reads the framebuffer and does not
 * write it, which is what lets it run after the frame is final and leave it
 * final.
 *
 * THE BUFFER IS HEAP AND IT IS THIS FILE'S, allocated on the first stacked
 * frame and never freed, and both of those are deliberate.
 *
 * Heap rather than a static array because a static would be 1.5 MB of host
 * .bss at the 2x tier, in EVERY build, including every inset run and every
 * selftest. port/tools/battery.py's note on the selftest BMP is explicit that
 * host-global layout outside .dsstate perturbs the rendered frame even at an
 * equal section base, so a layout feature that nobody has switched on has no
 * business moving the host's globals around. Allocated lazily, an inset run
 * carries one null pointer.
 *
 * This file's rather than the caller's because there are two callers (the
 * window's frame loop and the scene runner) and a buffer each would be the
 * same 1.5 MB twice.
 *
 * THE RETURN IS NOT CONST, and it used to be. The buffer handed back is this
 * file's own and every frame's compose rewrites the whole of it, so the const
 * was never protecting anything from anybody; what it did do was stop the one
 * caller that legitimately has to WRITE on the finished image -- the host's own
 * debug overlays in tests/walk_window.cpp, which must land on a fixed PHYSICAL
 * half and therefore cannot be painted into a framebuffer whose half moves.
 * See hal_sub_screen_stacked_top_y below. Nothing in the GAME writes here.
 */
unsigned int *hal_sub_screen_stacked_image(const unsigned int *top)
{
    static unsigned int *px;
    static size_t cap;
    static int refused;
    if (!hal_sub_screen_stacked() || !g_ready || !top) return 0;
    const ntr::StackLayout &lay = *hal_screen_layout();
    /* SIZED FOR THE GAP THE LAYOUT ASKS FOR, and grown rather than sized once
       for the worst case. A minigame's G latches at its InitResources and does
       not move again, so this reallocates at most once per scene; sizing for
       GAP_DS_MAX up front would carry 400 KB nobody uses through every gapless
       run, and the note above is explicit that this allocation is lazy on
       purpose. Never shrinks: a scene with a smaller gap reuses the block. */
    const size_t need = (size_t)lay.w * (size_t)lay.h * sizeof *px;
    if (need > cap && !refused) {
        unsigned int *bigger = (unsigned int *)std::realloc(px, need);
        if (bigger) {
            px = bigger;
            cap = need;
        } else {
            refused = 1;
            std::fprintf(stderr, "  [sub] stacked layout: could not allocate "
                         "the %dx%d image; presenting the top screen alone\n",
                         lay.w, lay.h);
        }
    }
    if (!px || cap < need) return 0;
    int evy = 0, to_white = 0;
    if (!port_fader_blend_state(&evy, &to_white)) evy = 0;
    ntr::ppu_compose_stacked(top, g_sub, px, lay.w, lay.h, evy, to_white, lay);
    return px;
}

/* The image's live size, for the consumers that need the number and not the
   pixels: walk_window's DIB header and window sizing, and the stacked BMP
   capture. Reads the same layout the compose above does, which is the whole
   point of there being one. */
void hal_sub_screen_stacked_size(int *w, int *h)
{
    const ntr::StackLayout &lay = *hal_screen_layout();
    if (w) *w = lay.w;
    if (h) *h = lay.h;
}

/* The image's HEADROOM in host rows, for the one consumer that has to name the
   parts of the image rather than just its size: the scene capture's own line in
   hal/scene_boot.cpp, which used to derive "the gap" by subtracting two screen
   heights from the image height and would now be reporting headroom plus gap
   under the gap's name. Reads the same layout everything else does. */
int hal_sub_screen_stacked_headroom(void)
{
    return hal_screen_layout()->head_h;
}

/* THE FIRST IMAGE ROW OF THE UPPER PHYSICAL SCREEN, for the host's overlays.
 *
 * ppu_compose_stacked names its two destinations top_y and bottom_y for the
 * PHYSICAL screens and picks which ENGINE goes in each off POWCNT1 bit 15, so
 * top_y is the upper LCD's first row whichever engine is feeding it that frame.
 * That is exactly the guarantee an overlay needs: the F3 stats, the F5 menu and
 * the save-state toast are HOST UI, not ROM content, and they belong on one
 * screen and one screen only. Painted into engine A's framebuffer instead they
 * inherit the ENGINE's affinity and follow it across the swap -- which is the
 * defect this exists to close (372/373/384/385 toggle the bit every frame, 377
 * clears it and leaves it clear).
 *
 * It is head_h today and it is read off the layout rather than spelled as one,
 * because a headroom is not the only thing that could ever sit above the upper
 * screen and a second reader of the same number is how the two fall out of
 * step. Pure: it reads the layout and nothing else. */
int hal_sub_screen_stacked_top_y(void)
{
    return hal_screen_layout()->top_y;
}

/* And whether the rows BETWEEN the halves are a hinge or the world's own rows,
   in DS rows, for the same one consumer and for the same reason. With the
   object shift on the band is world -obj_shift_ds..-1 with the top engine
   drawing into it, and a capture line that called that a gap would put a gap-on
   word on the one picture that has none.

   FORWARDED THROUGH THIS FILE rather than read from hal/screen_gap.cpp
   directly, which is the shape hal_sub_screen_stacked_generation already has
   and for a harder reason than tidiness: not every binary that links
   hal/scene_boot.cpp links the layout owner, and a direct call from there is an
   unresolved symbol in smoke_player. */
int hal_sub_screen_stacked_obj_shift(void)
{
    return hal_screen_layout()->obj_shift_ds;
}

/* A counter that steps whenever that size changes -- which is once, when a
   minigame's InitResources latches its G. walk_window watches it to know when
   to re-shape the DIB header and re-size the window; a consumer that reads the
   size every frame anyway can ignore it.
   Forwarded from hal/screen_gap.cpp through this file so that walk_window's
   declaration of it sits beside the other hal_sub_screen_* names and cannot
   pick up a different linkage from them. */
unsigned hal_sub_screen_stacked_generation(void)
{
    return hal_screen_layout_generation();
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

    /* SM64DS_TOUCH_PROBE: what a REAL reader made of the record this frame.
       Stage::CheckCameraInput is matched ROM code from src/, it reads the
       stylus through data_020a0de8[i].{touched,held,x,y}, and these two
       halfwords are its entire output -- so a rotate bit here is the whole
       write-to-read path proven end to end, not a probe reading its own
       write back. It runs after poll_touch in the same frame. */
    if (g_tp_n > 0 && touch_probe_at(g_tp_cur))
        std::fprintf(stderr, "[touch] f%d cam: Ctrl+4 held=%04x Ctrl+6 "
                     "pressed=%04x  f49c=%04x f49e=%04x  gate(+0x154)=%08x\n",
                     g_tp_cur, *(const unsigned short *)(ctrl + 4),
                     *(const unsigned short *)(ctrl + 6),
                     *(const unsigned short *)data_0209f49c,
                     *(const unsigned short *)data_0209f49e,
                     data_0209f318
                         ? *(const unsigned *)((const char *)data_0209f318 +
                                               0x154)
                         : 0u);

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

/* THE PRESENT RECTANGLE, both directions.
 *
 * walk_window.cpp's present() computes where the framebuffer landed inside the
 * client area and calls the setter with it; every consumer that has to turn a
 * mouse position into a game position calls the mapper. Keeping the two in one
 * place is the point: the forward arithmetic changes if the fit ever does, and
 * a second copy of the inverse somewhere else is how a resize feature quietly
 * breaks stylus aim.
 *
 * The mapper RETURNS whether the point was inside the picture, and fills the
 * framebuffer point either way -- clamped to the nearest edge pixel when it
 * was not. Callers that care about the difference (a click in a letterbox bar
 * is not a stylus press) test the return; callers that only ever want a valid
 * framebuffer coordinate can ignore it and get the old clamped behaviour.
 *
 * THE SETTER TAKES THE SOURCE SIZE NOW, and it has to. In the stacked layout
 * the rectangle was filled from an image twice as tall as the framebuffer, so
 * "how many source pixels tall is that rectangle" stopped being a constant the
 * inverse could assume. Passing it is the honest fix; deriving it from the
 * mode inside here would put a second copy of the mode's geometry in the one
 * place the note above says must not have a second copy.
 *
 * The two mappers are the same arithmetic over different bands of the same
 * rectangle, which is why they share client_to_src below rather than being
 * spelled twice.
 */
void hal_present_set_rect(int x, int y, int w, int h, int src_w, int src_h)
{
    g_pr_x = x;
    g_pr_y = y;
    g_pr_w = w;
    g_pr_h = h;
    g_pr_sw = src_w;
    g_pr_sh = src_h;
}

/* Client pixels to SOURCE-IMAGE pixels: the inverse of present()'s fit. Does
   no clamping and no range test -- both mappers below want the raw answer
   first, and they disagree about what "inside" means. Returns the source size
   it worked against in *sw / *sh so the caller can split it into bands. */
static void client_to_src(int cx, int cy, int *x, int *y, int *sw, int *sh)
{
    /* An old caller of the four-argument setter, or a build whose present path
       has not run, leaves the source size at zero. The fallback is the size of
       the image this run would present, which is the framebuffer's in the
       inset layout and the LAYOUT's in the stacked one.

       IT USED TO BE THE FRAMEBUFFER'S IN BOTH, which was right while the
       stacked image did not exist and became a headless run reporting a source
       one screen tall for an image two screens tall. Nothing on a windowed
       path reads this branch -- present() publishes a real rectangle on the
       first frame -- so the only reader is the touch probe on a headless run,
       and a probe that cannot be believed on the path it was written for is a
       probe with no purpose. The fallback maps client to source by the zoom
       divide below either way; only the size the bands are cut out of
       changes. */
    const ntr::StackLayout &fb_lay = *hal_screen_layout();
    const int s_w = g_pr_sw > 0 ? g_pr_sw
                                : (hal_sub_screen_stacked() ? fb_lay.w
                                                            : ntr::SCREEN_W);
    const int s_h = g_pr_sh > 0 ? g_pr_sh
                                : (hal_sub_screen_stacked() ? fb_lay.h
                                                            : ntr::SCREEN_H);
    if (sw) *sw = s_w;
    if (sh) *sh = s_h;
    if (g_pr_w > 0 && g_pr_h > 0) {
        /* the inverse of the fit: shift by the letterbox origin, then scale
           the picture's client size back to the source image's */
        *x = (int)(((long long)(cx - g_pr_x) * s_w) / g_pr_w);
        *y = (int)(((long long)(cy - g_pr_y) * s_h) / g_pr_h);
        /* a negative client offset truncates toward zero, which would fold
           the first row of bar pixels onto row 0 of the picture. Push those
           back out so an inside/outside answer is exact at the seam. */
        if (cx < g_pr_x) *x = -1;
        if (cy < g_pr_y) *y = -1;
    } else {
        /* nothing has presented yet: the fixed-zoom divide this was before */
        *x = cx / (g_zoom > 0 ? g_zoom : 1);
        *y = cy / (g_zoom > 0 ? g_zoom : 1);
    }
}

int hal_present_client_to_fb(int cx, int cy, int *fx, int *fy)
{
    int x, y, sw, sh;
    client_to_src(cx, cy, &x, &y, &sw, &sh);
    /* THIS FUNCTION STILL MEANS THE TOP SCREEN, in both layouts, and every one
       of its existing callers still gets exactly what it got before. In the
       inset layout the source IS the framebuffer and the test below is the
       test that was here. In the stacked layout the source is taller and the
       top screen is its upper band, so a click in the gap or in the lower half
       answers "not on the picture" -- which is the truth for a caller asking
       about the top screen. hal_present_client_to_sub is the lower band.

       THE TOP SCREEN'S BAND IS THE SAME ROWS WITH OR WITHOUT A GAP, because
       the gap is inserted BELOW it: top_y is 0 and the screen is SCREEN_H
       rows. So this test does not need the layout to be right, and it reads
       it anyway -- if a later change ever moves the top screen, the one place
       that decides where it is has to be the one place both mappers read. */
    const ntr::StackLayout &lay = *hal_screen_layout();
    const int inside = x >= 0 && y >= lay.top_y && x < ntr::SCREEN_W &&
                       y < lay.top_y + ntr::SCREEN_H;
    /* AND THE FRAMEBUFFER ROW IS THE SOURCE ROW MINUS top_y, which was a no-op
       for as long as top_y was zero and is not one any more: the gapless
       headroom puts head_h rows of image ABOVE the top screen, so a click on
       the top screen's first row arrives here as source row head_h and the
       framebuffer row it means is 0. The paragraph above already said the one
       place that decides where the top screen is has to be the one place both
       mappers read; this is that promise being kept. With no headroom top_y is
       0 and the subtraction changes nothing.
       A CLICK IN THE HEADROOM MAPS TO NOTHING, the gap band's own rule and for
       the same reason: those rows are picture with no panel under them, the
       hardware has no touch there, and a clamp would publish a press on the top
       screen's first row for a click that was never on it. `y` goes negative,
       `inside` is already false, and the clamp below only ever fires for a
       caller that ignores the return. */
    y -= lay.top_y;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= ntr::SCREEN_W) x = ntr::SCREEN_W - 1;
    if (y >= ntr::SCREEN_H) y = ntr::SCREEN_H - 1;
    if (fx) *fx = x;
    if (fy) *fy = y;
    return inside;
}

/* THE TOUCH TRANSFORM FOR THE STACKED LAYOUT: a client point to a DS pixel on
 * the bottom screen. Returns 1 only when the point is genuinely on the bottom
 * half of the picture -- a letterbox bar is not, the top screen is not, AND
 * THE GAP BAND IS NOT.
 *
 * Three steps: undo the fit into source pixels, subtract the layout's
 * bottom_y to get into the bottom screen's own band, then scale that band down
 * to the DS's own 256x192. The scale is a ratio rather than a shift because
 * SCREEN_W/SCREEN_H is the tier's, not necessarily two.
 *
 * bottom_y RATHER THAN SCREEN_H, and that one word is the whole of what the
 * gap changes here. With a 48-row gap the bottom screen starts 96 host rows
 * lower than it used to, and a mapper that kept subtracting one screen height
 * would put every press 96 client pixels above where the player aimed -- worse
 * than an offset, because it would still return "inside" and nothing would say
 * so. The layout is read rather than recomputed for exactly that reason.
 *
 * A CLICK IN THE GAP MAPS TO NOTHING. It is not clamped to the nearest screen:
 * there is no DS pixel under the hinge, the hardware has no touch there, and a
 * clamp would publish a stylus press at the top row of the bottom screen for a
 * click that was never on it. The band is between bottom_y and the top
 * screen's last row, `by` is negative there, and the test below refuses it --
 * the same answer a letterbox bar gets, which is the honest one.
 *
 * WHY IT IS EXACT AT THE 2x TIER: the bottom band is 512x384 and the DS screen
 * is 256x192, so x maps by (x * 256) / 512 = x / 2 and y the same. Client
 * point -> band pixel -> DS pixel with two integer divides and no rounding
 * term that can drift a row at the seam. The clamp below only ever fires for a
 * caller that ignores the return.
 */
int hal_present_client_to_sub(int cx, int cy, int *dsx, int *dsy)
{
    int x, y, sw, sh;
    client_to_src(cx, cy, &x, &y, &sw, &sh);
    /* the bottom screen's band of the source image. In the inset layout the
       source is one screen tall, sh - bottom_y is zero, and this correctly
       answers "outside" for every point -- in that layout the panel is the
       bottom screen and poll_touch takes the other branch. */
    const ntr::StackLayout &lay = *hal_screen_layout();
    const int band_h = sh - lay.bottom_y;
    const int by = y - lay.bottom_y;
    const int inside = band_h > 0 && x >= 0 && x < sw && by >= 0 && by < band_h;
    int dx = 0, dy = 0;
    if (band_h > 0) {
        dx = (int)(((long long)x * ntr::SUB_W) / (sw > 0 ? sw : 1));
        dy = (int)(((long long)by * ntr::SUB_H) / band_h);
    }
    if (dx < 0) dx = 0;
    if (dy < 0) dy = 0;
    if (dx >= ntr::SUB_W) dx = ntr::SUB_W - 1;
    if (dy >= ntr::SUB_H) dy = ntr::SUB_H - 1;
    if (dsx) *dsx = dx;
    if (dsy) *dsy = dy;
    return inside;
}

/* SM64DS_TOUCH_CLIENT_PROBE="cx,cy[;cx,cy...]": run the client-to-DS transform
 * over the listed CLIENT points, once, on the first frame that polls, and print
 * what each one maps to. It exists because the stacked layout's transform
 * cannot otherwise be exercised deterministically: the scene runner is
 * headless by construction (walk_window hands over to port_scene_run before
 * the window is created), so there is no mouse on the only path a minigame
 * runs on today, and a transform nobody can run is a transform nobody can
 * check. This drives the SAME function poll_touch drives, against the SAME
 * present rectangle, so it is the real arithmetic and not a restatement of it.
 *
 * IT PRINTS TO STDERR, AND ON A SCENE RUN STDERR IS NOT ON YOUR TERMINAL.
 * walk_window's flight recorder takes stderr into playlog/play_*.log on every
 * path that is not a window selftest, which is exactly the headless path this
 * probe exists for, so a reproducer that greps its own console counts zero
 * lines and reads that as the probe not firing. Either add
 * SM64DS_NO_PLAYLOG=1, or read playlog/. A windowed SM64DS_WINDOW_SELFTEST run
 * is the one case where the recorder is off and stderr arrives directly.
 *
 * It reads no mouse and writes no stylus. It is a printer.
 */
void hal_touch_client_probe(void)
{
    static int done, waited;
    const char *s = std::getenv("SM64DS_TOUCH_CLIENT_PROBE");
    if (done || !s) return;
    /* WAIT FOR A PRESENT RECTANGLE THE FRAME LOOP PUBLISHED, and this cost the
       lane one wrong reading before it was written down. This poll runs at the
       TOP of a frame, and by the time the first one runs the window procedure
       has already answered a WM_PAINT from CreateWindow -- so a rectangle
       exists, and it is one the framebuffer filled, before the frame loop has
       ever built a stacked image. The probe fired on it and printed
       "layout stacked" beside "src 512x384", which is two true halves reading
       as one false whole.

       So: skip the first two polls, which puts this past the frame loop's own
       first present, and then require a rectangle. Past thirty polls with no
       rectangle at all the run genuinely has no window (the scene runner is
       that case) and the fixed-zoom fallback is the honest answer, said in
       those words rather than passed off as the windowed one. */
    if (++waited <= 2) return;
    if (g_pr_w <= 0 && waited < 30) return;
    done = 1;
    if (g_pr_w <= 0)
        std::fprintf(stderr, "[touchmap] NO PRESENT RECTANGLE was ever "
                     "published (headless run, no window). What follows is the "
                     "fixed-zoom fallback, not the windowed transform.\n");
    {
        const ntr::StackLayout &lay = *hal_screen_layout();
        std::fprintf(stderr, "[touchmap] rect x%d y%d w%d h%d src %dx%d, "
                     "layout %s, image %dx%d top_y %d band %d+%d bottom_y %d\n",
                     g_pr_x, g_pr_y, g_pr_w, g_pr_h, g_pr_sw, g_pr_sh,
                     hal_sub_screen_stacked() ? "stacked" : "inset",
                     lay.w, lay.h, lay.top_y, lay.band_y, lay.band_h,
                     lay.bottom_y);
    }
    while (*s) {
        const int cx = std::atoi(s);
        while (*s && *s != ',' && *s != ';') ++s;
        if (*s == ',') ++s;
        const int cy = std::atoi(s);
        while (*s && *s != ';') ++s;
        if (*s == ';') ++s;
        int fx = 0, fy = 0, dx = 0, dy = 0;
        const int on_top = hal_present_client_to_fb(cx, cy, &fx, &fy);
        const int on_sub = hal_present_client_to_sub(cx, cy, &dx, &dy);
        /* WHICH BAND OF THE IMAGE IT LANDED IN, said in the source's own rows.
           "out of both" is three different things -- a letterbox bar, the gap,
           and a point off the window entirely -- and only the middle one is a
           feature. Naming it is what makes a gap click PROVABLY nothing rather
           than merely unclaimed. */
        const ntr::StackLayout &lay = *hal_screen_layout();
        int sx = 0, sy = 0, ssw = 0, ssh = 0;
        client_to_src(cx, cy, &sx, &sy, &ssw, &ssh);
        const char *where = "bar";
        if (sx >= 0 && sx < ssw) {
            if (lay.head_h > 0 && sy >= 0 && sy < lay.top_y)
                where = "HEADROOM";
            else if (sy >= lay.top_y && sy < lay.top_y + ntr::SCREEN_H)
                where = "top screen";
            else if (lay.band_h > 0 && sy >= lay.band_y && sy < lay.bottom_y)
                where = "GAP";
            else if (sy >= lay.bottom_y && sy < ssh)
                where = "bottom screen";
        }
        std::fprintf(stderr, "[touchmap] client (%d,%d) -> src(%d,%d) %s | "
                     "top %s fb(%d,%d) | sub %s ds(%d,%d)\n", cx, cy, sx, sy,
                     where, on_top ? "IN " : "out", fx, fy,
                     on_sub ? "IN " : "out", dx, dy);
    }
    std::fflush(stderr);
}

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

/* The third leaf, func_ov004_020adc4c, is NOT stubbed here any more (run linkw
   wave C, lane cat-2d). It was tagged `src reads data_ov004_020beb60, and
   ov004 is not mounted`, and that one BSS word is now hosted in
   hal/oam2d_ov004_bss.cpp, so the matched src/func_ov004_020adc4c.c is the
   body LoadFont reaches. Leaving the stub here would be a duplicate
   definition. The branch that calls it is still not taken: it is LoadFont's
   data_0209d698 == 2 arm, the minigame framework's font, and the window loads
   font 0. ov004 is the minigame scene framework, not the VS mode this comment
   used to say; see port/ov004_ov007_2d_map.txt. */

/* Debug: the bottom screen on its own, at its own size. */
void hal_sub_screen_dump(const char *path)
{
    ntr::ppu_scanout_sub(g_sub);
    ntr::ppu_write_bmp_sub(path, g_sub);
}

/* THE SAME PICTURE WITHOUT RE-SCANNING, and the difference matters. The dump
   above rasterises engine B again before writing, which is right for a debug
   hook asked for out of nowhere and WRONG for a capture taken beside a stacked
   image: a second scan-out reads the registers as they stand now, and on a
   scene whose slot 24 rewrites DISPCNT_B every frame that is a different frame's
   configuration. This writes the buffer the frame actually presented.
   Returns 0 when no frame has been scanned out yet. */
int hal_sub_screen_write_bmp(const char *path)
{
    if (!g_ready) return 0;
    return ntr::ppu_write_bmp_sub(path, g_sub) ? 1 : 0;
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

    /* THE OAM ENTRY CENSUS, in the same three buckets the Stage-3 audit used,
       so its numbers and these are the same measurement.
         all-zero  the engine was never uploaded to -- boot VRAM, untouched
         y == 192  OAM::Reset's parked value (0xc0), an entry deliberately off
         other y   a sprite the game actually placed
       An engine reading 128/0/0 has had nothing written to it AT ALL, which is
       a different fault from an engine that was uploaded a screen of parked
       entries, and only this split tells them apart. Both engines, because
       "the sub screen is empty" and "the sub sprites went to the main screen"
       produce the same sub picture and opposite main censuses. */
    for (int e = 0; e < 2; ++e) {
        const unsigned base = e ? 0x07000400u : 0x07000000u;
        unsigned zero = 0, parked = 0, real = 0;
        for (int i = 0; i < 128; ++i) {
            const volatile unsigned short *a =
                (const volatile unsigned short *)(base + i * 8);
            const unsigned short a0 = a[0], a1 = a[1], a2 = a[2];
            if (!a0 && !a1 && !a2) { ++zero; continue; }
            if ((a0 & 0xff) == 192) ++parked; else ++real;
        }
        std::printf("[sub] OAM %s %08x: %u all-zero, %u parked (y=192), "
                    "%u placed\n", e ? "engine B" : "engine A", base,
                    zero, parked, real);
    }
    /* The backdrop is engine B's BG palette entry 0 and nothing else -- see
       ntr/ppu_sub.cpp's `bgr555(rd16(kPlttBase))`. Printed because "the panel
       is one flat colour" and "the panel is one flat colour AND that is the
       colour the game asked for" are different findings. */
    {
        const unsigned short bd = *(volatile unsigned short *)0x05000400;
        std::printf("[sub] backdrop = sub BG pltt[0] = %04x (r%u g%u b%u)\n",
                    bd, bd & 0x1f, (bd >> 5) & 0x1f, (bd >> 10) & 0x1f);
    }
}

}  // extern "C"
