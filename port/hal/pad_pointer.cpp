/* pad_pointer.cpp: the PAD POINTER (run hunt8, lane PADMENUS1). Read hal/pad_pointer.h for what it is and where it works.

   DECISIONS (each with the reason):
   1. The pen is the JUMP binding (A by default), the same button every menu already takes as confirm (MENU_PAD_CONFIRM), so a rebound confirm
     moves the pen with it. Held = pen down, released = pen up, and a short tap is stretched to at least three frames because the ROM's touch gates
     want the touched byte and its edge byte together (memory sm64ds-minigames-headless-provable: a one frame press never registers).
   2. Speed is a quadratic curve over the stick after a 25 percent dead zone: a small push is a fine, slow move (a Shuffle Shell grab box is
     20 DS pixels, a Coincentration card is bigger), a full push is 4 DS pixels a frame (the screen in about a second). The D-pad is 2 DS
     pixels a frame, 3.5 after half a second held. All of it integer arithmetic in 1/256 DS pixels, so a scripted row replays exactly.
   3. A confirm that was already down when the pointer came on does not press the pen until it has been released once, so the A that started a
     minigame or closed a page is not a tap on the first frame of the next screen.
   4. Nothing is drawn or consumed unless the pointer is on, and it is never on without a controller answering, so a keyboard / mouse player and
     every identity gate (no pad input) see no change at all. */
#include "hal/pad_pointer.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "hal/menu_compose.h"
#include "hal/screen_gap.h"
#include "ntr/ppu.h"

extern "C" int hal_sub_screen_stacked(void);
extern "C" int port_scene_live_id(void);
extern "C" int port_char_strip_focus(void);   /* the character strip on the star select has the cursor: A picks, the pointer stays off */
extern "C" int port_scene_env_want(void);   /* SM64DS_SCENE, -1 when the scene was not asked for by name */
extern "C" int settings_page_swallow_back(void);
extern "C" int menu_bigmap_up(void);
extern "C" int hal_sub_inset_rect(int *x, int *y, int *w, int *h);   /* hal/sub_screen.cpp: the corner panel the last present drew, picture pixels */

namespace {

enum { MODE_NONE = 0, MODE_STACKED = 1, MODE_MENU = 2, MODE_INSET = 3 };

struct State {
    int on;                    /* the pointer is live */
    int mode;                  /* MODE_* it was last driven in */
    int scene;                 /* the scene it was switched on in */
    int x256, y256;            /* position: DS bottom-screen pixels (stacked) or picture pixels (menu), x 256 */
    int armed;                 /* the confirm has been up since the pointer came on: a press may start the pen */
    int hold;                  /* frames the pen is still kept down after a short tap */
    int down;                  /* the pen is down this frame */
    int dpad_frames;           /* consecutive frames a D-pad direction has been held */
    int hint;                  /* frames of the hint left */
    int hint_done;             /* the hint was shown for this scene already */
    int last_hx, last_hy;      /* menu mode: where the hover was last pushed */
    int stick_was;             /* the right stick was past the dead zone last frame */
    int idle;                  /* frames with no stick push and no pen (outside a minigame the pointer goes away after about four seconds of that) */
};
State S;

/* what the render side reads (the picture can be built on another thread than the frame step) */
std::atomic<int> g_on{0}, g_mode{0}, g_x{0}, g_y{0}, g_down{0}, g_hint{0}, g_mg{0};
std::atomic<int> g_mouse{0};     /* the real pen (a mouse press on the touch screen) was seen: the pointer steps aside until the pad is used again */

/* the ROM's own predicate for a minigame scene id (src/IsMinigameActorID.c: 0x169..0x186) */
int is_minigame(int scene) { return scene >= 0x169 && scene <= 0x186; }

int trace_on(void)
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_PADPTR_TRACE"); v = e ? ((*e == '2') ? 2 : 1) : 0; }
    return v;
}

int iabs(int v) { return v < 0 ? -v : v; }

/* one axis of the quadratic stick curve: v in +-32767, result in 1/256 px per frame, max = the full push speed (1/256 px) */
int stick_speed(int v, int max256)
{
    const int a = iabs(v);
    int m = (int)(((long long)a * 4096) / 32767);       /* 0..4096 */
    if (m > 4096) m = 4096;
    const int dz = 1024;
    if (m <= dz) return 0;
    const long long t = ((long long)(m - dz) * 4096) / (4096 - dz);   /* 0..4096 */
    const int s = (int)(((long long)max256 * t * t) / (4096LL * 4096LL));
    return v < 0 ? -s : s;
}

void reset_pos(int mode, int *x256, int *y256)
{
    if (mode == MODE_STACKED || mode == MODE_INSET) {
        *x256 = (ntr::SUB_W / 2) * 256;
        *y256 = (ntr::SUB_H / 2) * 256;
    } else {
        int w = 512, h = 384;
        hal_menu_last_size(&w, &h);
        *x256 = (w / 2) * 256;
        *y256 = (h / 2) * 256;
    }
}

/* the arrow and the hint */

const char *const kArrow[17] = {
    "X          ", "XX         ", "XoX        ", "XooX       ", "XoooX      ", "XooooX     ", "XoooooX    ", "XooooooX   ",
    "XoooooooX  ", "XooooooooX ", "XoooooXXXXX", "XooXooX    ", "XoX XooX   ", "XX  XooX   ", "X    XooX  ", "     XooX  ", "      XX   "
};

/* a 5x7 face for the hint (capitals, space, comma, hyphen): small on purpose, this is a one line reminder, not a page */
const unsigned char kFont[26][7] = {
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}, {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},
    {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E}, {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}, {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E},
    {0x07,0x02,0x02,0x02,0x02,0x12,0x0C}, {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}, {0x11,0x11,0x19,0x15,0x13,0x11,0x11}, {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},
    {0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}, {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, {0x11,0x11,0x11,0x11,0x11,0x11,0x0E},
    {0x11,0x11,0x11,0x11,0x11,0x0A,0x04}, {0x11,0x11,0x11,0x15,0x15,0x15,0x0A}, {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11},
    {0x11,0x11,0x11,0x0A,0x04,0x04,0x04}, {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}
};

const unsigned char *glyph(char c, unsigned char *tmp)
{
    if (c >= 'A' && c <= 'Z') return kFont[c - 'A'];
    memset(tmp, 0, 7);
    if (c == ',') { tmp[4] = 0x0C; tmp[5] = 0x04; tmp[6] = 0x08; }
    else if (c == '-') tmp[3] = 0x1F;
    return tmp;       /* a space draws nothing */
}

inline void put(unsigned int *img, int stride, int w, int h, int x, int y, unsigned int argb)
{
    if (x < 0 || y < 0 || x >= w || y >= h) return;
    img[(size_t)y * stride + x] = argb;
}

inline void blend_dark(unsigned int *img, int stride, int w, int h, int x, int y)
{
    if (x < 0 || y < 0 || x >= w || y >= h) return;
    unsigned int p = img[(size_t)y * stride + x];
    const unsigned r = ((p >> 16) & 0xff) * 3 / 8, g = ((p >> 8) & 0xff) * 3 / 8, b = (p & 0xff) * 3 / 8;
    img[(size_t)y * stride + x] = 0xff000000u | (r << 16) | (g << 8) | b;
}

/* the arrow with its tip at (tx, ty) in picture pixels, each bitmap pixel s x s; the pen down fills it yellow */
void draw_arrow(unsigned int *img, int stride, int w, int h, int tx, int ty, int s, int down)
{
    const unsigned int fill = down ? 0xffffd23cu : 0xffffffffu;
    for (int r = 0; r < 17; ++r)
        for (int c = 0; c < 11; ++c) {
            const char ch = kArrow[r][c];
            if (ch == ' ') continue;
            const unsigned int argb = ch == 'X' ? 0xff101010u : fill;
            for (int dy = 0; dy < s; ++dy)
                for (int dx = 0; dx < s; ++dx) put(img, stride, w, h, tx + c * s + dx, ty + r * s + dy, argb);
        }
}

/* the hint on a dark plate, centred on cx, its top at ty; the font pixel is as big as fits in maxw (1..3) */
void draw_hint(unsigned int *img, int stride, int w, int h, int cx, int ty, int maxw, int minigame)
{
    const char *t = minigame ? "STICK OR D-PAD MOVES THE POINTER, A TAPS" : "RIGHT STICK MOVES THE POINTER, A TAPS";
    const int n = (int)strlen(t);
    int s = (maxw - 24) / (n * 6);
    if (s < 1) s = 1;
    if (s > 3) s = 3;
    const int tw = n * 6 * s - s, th = 7 * s, pad = 3 * s;
    const int x0 = cx - tw / 2 - pad, y0 = ty - pad, x1 = cx + tw / 2 + pad, y1 = ty + th + pad;
    for (int y = y0; y < y1; ++y)
        for (int x = x0; x < x1; ++x) blend_dark(img, stride, w, h, x, y);
    unsigned char tmp[7];
    for (int i = 0; i < n; ++i) {
        const unsigned char *g = glyph(t[i], tmp);
        for (int r = 0; r < 7; ++r)
            for (int c = 0; c < 5; ++c)
                if (g[r] & (0x10 >> c))
                    for (int dy = 0; dy < s; ++dy)
                        for (int dx = 0; dx < s; ++dx)
                            put(img, stride, w, h, x0 + pad + (i * 6 + c) * s + dx, ty + r * s + dy, 0xffffffffu);
    }
}

void publish(void)
{
    g_x.store(S.x256);
    g_y.store(S.y256);
    g_down.store(S.down);
    g_hint.store(S.hint);
    g_mode.store(S.mode);
    g_on.store(S.on);
}

}  // namespace

static int s_pad_confirm;      /* LOBBYKEYS1: the controller's own confirm is down this frame (not a key or mouse binding) */
extern "C" int hal_padptr_pad_confirm(void) { return s_pad_confirm; }

extern "C" void hal_padptr_frame(const PadPtrIn *in, PadPtrOut *out)
{
    s_pad_confirm = in->pad_live && in->confirm;
    out->eat_confirm = 0;
    out->eat_dpad = 0;
    int scene = port_scene_live_id();
    if (scene < 0) scene = port_scene_env_want();      /* a scene run asked for by name has no carried scene object */
    int mode = MODE_NONE;
    if (hal_sub_screen_stacked()) mode = MODE_STACKED;
    else if (hal_menu_live() && !menu_bigmap_up()) mode = MODE_MENU;
    else { int ix, iy, iw, ih; if (hal_sub_inset_rect(&ix, &iy, &iw, &ih)) mode = MODE_INSET; }
    /* the settings page owns the controller (its own cursor, tabs on the bumpers) */
    if (mode == MODE_MENU && settings_page_swallow_back()) mode = MODE_NONE;
    /* the character strip on the star select owns the pad while its cursor is on it */
    if (mode == MODE_MENU && port_char_strip_focus()) mode = MODE_NONE;
    const int mg = is_minigame(scene);
    g_mg.store(mg);

    if (!in->pad_live || mode == MODE_NONE) {
        if (S.on && trace_on()) fprintf(stderr, "[padptr] f%d off (%s)\n", in->frame, !in->pad_live ? "no pad" : "no touch surface");
        S.on = 0; S.down = 0; S.hold = 0; S.hint = 0;
        S.mode = -1; S.scene = -2;                       /* the next live frame is a fresh start */
        publish();
        return;
    }
    if (scene != S.scene || mode != S.mode) {           /* a new screen: start from the middle, off until the pad is used */
        S.scene = scene; S.mode = mode;
        S.on = 0; S.down = 0; S.hold = 0; S.hint = 0; S.hint_done = 0; S.dpad_frames = 0;
        S.armed = in->confirm ? 0 : 1;                  /* the A that brought us here is not a tap */
        g_mouse.store(0);
        if (S.mode == MODE_MENU) hal_menu_touch_end();
        reset_pos(mode, &S.x256, &S.y256);
    }

    const int rs = iabs(in->rx) > iabs(in->ry) ? iabs(in->rx) : iabs(in->ry);
    const int ls = iabs(in->lx) > iabs(in->ly) ? iabs(in->lx) : iabs(in->ly);
    const int rs_on = rs > 8192;                        /* the stick is past the dead zone: it moves the pointer, and counts as use */
    const int rs_push = rs > 16000;                     /* a deliberate push (about half way): what switches the pointer on, so a drifting stick never does */
    const int dpad = in->dpad & 15;
    const int act = rs_push || (mg && (dpad || ls > 16000)) || (mg && in->confirm && S.armed);   /* the pad is being used */
    if (act) g_mouse.store(0);

    if (!S.on) {
        /* switched on by the right stick anywhere; in a minigame by any use of the pad (nothing else on that screen takes it); never while the mouse is the pen */
        if (act && !g_mouse.load()) {
            S.on = 1;
            if (!S.hint_done && !in->confirm) { S.hint = 360; S.hint_done = 1; }
            if (trace_on()) fprintf(stderr, "[padptr] f%d on scene=%d mode=%d at %d,%d\n", in->frame, scene, mode, S.x256 / 256, S.y256 / 256);
        }
    } else if (g_mouse.load()) {
        S.on = 0; S.down = 0; S.hold = 0; S.hint = 0;
        if (S.mode == MODE_MENU) hal_menu_touch_end();
        if (trace_on()) fprintf(stderr, "[padptr] f%d off (the mouse took over)\n", in->frame);
    } else if (!mg && (dpad || ls > 24000) && !rs_on) {
        /* the D-pad and the left stick belong to the game's own cursor outside a minigame: hand them back */
        S.on = 0; S.down = 0; S.hold = 0; S.hint = 0;
        if (S.mode == MODE_MENU) hal_menu_touch_end();
        if (trace_on()) fprintf(stderr, "[padptr] f%d off (buttons handed back)\n", in->frame);
    }
    S.stick_was = rs_on;
    if (S.on) {
        if (rs_on || in->confirm || S.down) S.idle = 0; else ++S.idle;
        if (!mg && S.idle > 240) {                      /* the pointer was left alone: give the buttons back */
            S.on = 0; S.down = 0; S.hold = 0; S.hint = 0; S.idle = 0;
            if (S.mode == MODE_MENU) hal_menu_touch_end();
            if (trace_on()) fprintf(stderr, "[padptr] f%d off (idle)\n", in->frame);
        }
    } else {
        S.idle = 0;
    }

    if (!S.on) { S.down = 0; publish(); return; }

    /* move */
    int unit_num = 256;       /* 1/256 of a position unit per DS pixel of speed: stacked moves in DS pixels directly */
    if (S.mode == MODE_MENU) {
        int w = 512, h = 384;
        hal_menu_last_size(&w, &h);
        unit_num = (int)(256LL * h / ntr::SUB_H);       /* picture pixels per DS pixel (x256) */
    }
    int vx = stick_speed(in->rx, 4 * 256);
    int vy = -stick_speed(in->ry, 4 * 256);
    if (mg) {       /* a minigame has no other use for the left stick: it moves the pointer too (the faster of the two sticks wins per axis) */
        const int lvx = stick_speed(in->lx, 4 * 256), lvy = -stick_speed(in->ly, 4 * 256);
        if (iabs(lvx) > iabs(vx)) vx = lvx;
        if (iabs(lvy) > iabs(vy)) vy = lvy;
    }
    if (mg && dpad) {
        ++S.dpad_frames;
        const int sp = S.dpad_frames > 30 ? 896 : 512;
        if (dpad & 8) vx += sp;
        if (dpad & 4) vx -= sp;
        if (dpad & 2) vy += sp;
        if (dpad & 1) vy -= sp;
    } else {
        S.dpad_frames = 0;
    }
    S.x256 += (int)(((long long)vx * unit_num) / 256);
    S.y256 += (int)(((long long)vy * unit_num) / 256);

    /* clamp to the touch screen / the picture */
    int maxx, maxy;
    if (S.mode == MODE_STACKED || S.mode == MODE_INSET) {
        maxx = ntr::SUB_W - 1; maxy = ntr::SUB_H - 1;
    } else {
        int w = 512, h = 384;
        hal_menu_last_size(&w, &h);
        maxx = w - 1; maxy = h - 1;
    }
    if (S.x256 < 0) S.x256 = 0;
    if (S.y256 < 0) S.y256 = 0;
    if (S.x256 > maxx * 256) S.x256 = maxx * 256;
    if (S.y256 > maxy * 256) S.y256 = maxy * 256;

    /* the pen */
    if (!in->confirm) S.armed = 1;
    const int was_down = S.down;
    const int press = in->confirm && S.armed;
    if (press && !was_down) S.hold = 3;                 /* a short tap is owed three frames of pen (see the banner) */
    if (press || S.hold > 0) {
        S.down = 1;
        if (S.hold > 0) --S.hold;
        if (press) S.hint = 0;
    } else {
        S.down = 0;
    }
    if (was_down && !S.down && S.mode == MODE_MENU) hal_menu_touch_end();
    if (S.hint > 0) --S.hint;

    /* the mouse's own hover, so a hotspot under the arrow lights like it does under the real pointer (menu mode) */
    if (S.mode == MODE_MENU && (S.x256 / 256 != S.last_hx || S.y256 / 256 != S.last_hy)) {
        S.last_hx = S.x256 / 256; S.last_hy = S.y256 / 256;
        hal_menu_hover(S.last_hx, S.last_hy);
    }

    out->eat_confirm = 1;
    out->eat_dpad = mg ? 1 : 0;
    if (trace_on() && (S.down != was_down || (in->frame % 30) == 0 || trace_on() == 2))
        fprintf(stderr, "[padptr] f%d pos=%d,%d down=%d\n", in->frame, S.x256 / 256, S.y256 / 256, S.down);
    publish();
}

extern "C" int hal_padptr_poll(int *mode, int *x, int *y)
{
    if (!g_on.load() || !g_down.load()) return 0;
    *mode = g_mode.load();
    *x = g_x.load() / 256;
    *y = g_y.load() / 256;
    return 1;
}

extern "C" int hal_padptr_on(void) { return g_on.load(); }

extern "C" void hal_padptr_draw_stacked(unsigned int *img, int w, int h)
{
    if (!g_on.load() || g_mode.load() != MODE_STACKED || !img) return;
    const ntr::StackLayout &lay = *hal_screen_layout();
    if (lay.pan_w < 1 || lay.pan_h < 1) return;
    const int dx = g_x.load() / 256, dy = g_y.load() / 256;
    const int tx = lay.pan_x0 + (int)(((long long)dx * lay.pan_w) / ntr::SUB_W);
    const int ty = lay.bottom_y + (int)(((long long)dy * lay.pan_h) / ntr::SUB_H);
    const int unit = lay.pan_h / ntr::SUB_H;                   /* image pixels per DS pixel */
    const int s = unit >= 4 ? unit / 2 : 1;
    draw_arrow(img, lay.w, w, h, tx, ty, s, g_down.load());
    if (g_hint.load() > 0) draw_hint(img, lay.w, w, h, lay.pan_x0 + lay.pan_w / 2, lay.bottom_y + lay.pan_h - 11 * unit, lay.pan_w, g_mg.load());
}

extern "C" void hal_padptr_draw_menu(unsigned int *dst, int w, int h, int stride)
{
    if (!g_on.load() || g_mode.load() != MODE_MENU || !dst) return;
    const int px = g_x.load() / 256, py = g_y.load() / 256;
    const int unit = h / ntr::SUB_H;                           /* picture pixels per DS pixel */
    const int s = unit >= 4 ? unit / 2 : 1;
    draw_arrow(dst, stride, w, h, px, py, s, g_down.load());
    if (g_hint.load() > 0)
        draw_hint(dst, stride, w, h, w / 2, h - 11 * (unit >= 1 ? unit : 1), w, g_mg.load());
}

/* poll_touch saw the REAL pen (a mouse press) land on the touch screen: a pointer left over from the pad steps aside */
extern "C" void hal_padptr_mouse_down(void) { g_mouse.store(1); }

/* the arrow inside the corner panel (INSET): the pointer is in DS pixels, the panel is where the last present put the bottom screen */
extern "C" void hal_padptr_draw_inset(unsigned int *dst, int w, int h, int stride)
{
    if (!g_on.load() || g_mode.load() != MODE_INSET || !dst) return;
    int rx, ry, rw, rh;
    if (!hal_sub_inset_rect(&rx, &ry, &rw, &rh) || rw < 1 || rh < 1) return;
    const int dx = g_x.load() / 256, dy = g_y.load() / 256;
    const int tx = rx + (int)(((long long)dx * rw) / ntr::SUB_W);
    const int ty = ry + (int)(((long long)dy * rh) / ntr::SUB_H);
    const int unit = h / ntr::SUB_H;
    const int sc = unit >= 4 ? unit / 2 : 1;
    draw_arrow(dst, stride, w, h, tx, ty, sc, g_down.load());
    if (g_hint.load() > 0) draw_hint(dst, stride, w, h, w / 2, h - 11 * (unit >= 1 ? unit : 1), w, g_mg.load());
}
