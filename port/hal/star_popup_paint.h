// THE STAR POPUP PAINTER (tests/walk_window.cpp only; compiled after the chat
// painter and net_hud.h, whose glyph helpers it uses). Run mp1, lane STARPOP1.
// Draw-only: it reads hal/star_popup.h's view and writes overlay pixels.
#ifndef PORT_HAL_STAR_POPUP_PAINT_H
#define PORT_HAL_STAR_POPUP_PAINT_H

#include "star_popup.h"

#ifdef OVL_SCALE
/* STARPOP2 / STARPOP3: the d-pad picture a \x01 in a line stands for, drawn the way Tango's mockup draws it: a dark grey plus
   with a thick dark outline and slightly rounded ends, the UP arm yellow with a brown arrow. It is sized off the font's capital
   height (about one and a half capitals tall, centred on them) and sits with the same gap of air on both sides, measured from
   the ink of the letters next to it, so "Press [pad] to" reads evenly. Every pixel is 4x4 supersampled and blended, so the
   rounded corners and the arrow's slopes stay smooth at any window size. */
static void sp_ink_cols(unsigned char ch, int rom, int *l, int *r)
{
    *l = 0; *r = OVL_ADVANCE - 1;
    if (!rom || ch < NT_FIRST || ch > NT_LAST || !g_chat_glyph[ch - NT_FIRST].have) return;
    const NtGlyph &gl = g_chat_glyph[ch - NT_FIRST];
    int lo = NT_GW, hi = -1;
    for (int y = 0; y < NT_GH; ++y)
        for (int x = 0; x < NT_GW; ++x)
            if (gl.ink[y][x]) { if (x < lo) lo = x; if (x > hi) hi = x; }
    if (hi < 0) { *l = 0; *r = -1; return; }   /* a blank glyph: no ink */
    *l = lo; *r = hi;
}
/* the capital P's ink rows, top and bottom, in glyph rows */
static void sp_cap_rows(int rom, int *top, int *bot)
{
    if (!rom || !g_chat_glyph['P' - NT_FIRST].have) { *top = (NT_GH - OVL_GLYPH_H) / 2; *bot = *top + OVL_GLYPH_H - 1; return; }
    *top = 2; *bot = 12;
    const NtGlyph &gl = g_chat_glyph['P' - NT_FIRST];
    int t = -1, b = -1;
    for (int y = 0; y < NT_GH; ++y)
        for (int x = 0; x < NT_GW; ++x)
            if (gl.ink[y][x]) { if (t < 0) t = y; b = y; }
    if (t >= 0) { *top = t; *bot = b; }
}
static int sp_icon_side(int rom, int sc)
{
    int t, b;
    sp_cap_rows(rom, &t, &b);
    const int cap = (b - t + 1) * sc;
    int s = (cap * 3 + 1) / 2;
    if (s > (NT_GH + 2) * sc) s = (NT_GH + 2) * sc;
    return s < 9 ? 9 : s;
}
static int sp_icon_gap(int sc) { return 4 * sc; }

/* signed distance from (px, py) to a box centred (cx, cy) with half sizes (hx, hy) and corner radius rr; negative inside */
static float sp_box_sd(float px, float py, float cx, float cy, float hx, float hy, float rr)
{
    const float qx = fabsf(px - cx) - (hx - rr), qy = fabsf(py - cy) - (hy - rr);
    const float ox = qx > 0 ? qx : 0, oy = qy > 0 ? qy : 0;
    const float m = qx > qy ? qx : qy;
    return sqrtf(ox * ox + oy * oy) + (m < 0 ? m : 0) - rr;
}
/* the d-pad's colour at (px, py), icon-local, side s: 0 for none, else 0xAARRGGBB */
static uint32_t sp_dpad_at(float px, float py, float s)
{
    const float a = s * 0.36f;                 /* arm width */
    const float h = s * 0.5f;
    const float rr = a * 0.22f;                /* the arms' rounded ends */
    const float t = s / 15.0f < 1.5f ? 1.5f : s / 15.0f;   /* the dark outline */
    const float d1 = sp_box_sd(px, py, h, h, a * 0.5f, h, rr);
    const float d2 = sp_box_sd(px, py, h, h, h, a * 0.5f, rr);
    const float d = d1 < d2 ? d1 : d2;
    if (d > 0) return 0;
    if (d > -t) return 0xFF28282Cu;
    const float arm0 = h - a * 0.5f;           /* where the UP arm meets the middle */
    if (py < arm0 + t * 0.5f) {
        /* the arrow: a triangle pointing up, inside the lit arm */
        const float top = t + (arm0 - t) * 0.12f, base = t + (arm0 - t) * 0.84f;
        const float hw = (a - 2 * t) * 0.40f;
        if (py >= top && py <= base) {
            const float k = (py - top) / (base - top);
            if (fabsf(px - h) <= hw * k + 0.25f) return 0xFF785000u;
        }
        return 0xFFFFE060u;
    }
    return 0xFF5A5A60u;
}
static void sp_dpad(const OvlSurface &fb, int x0, int y0, int s)
{
    for (int y = 0; y < s; ++y) {
        const int fy = y0 + y;
        if (fy < 0 || fy >= ntr::active_h) continue;
        uint32_t *row = fb.px + (size_t)fy * (size_t)fb.stride;
        for (int x = 0; x < s; ++x) {
            const int fx = x0 + x;
            if (fx < 0 || fx >= ntr::active_w) continue;
            unsigned rs = 0, gs = 0, bs = 0, n = 0;
            for (int sy = 0; sy < 4; ++sy)
                for (int sx = 0; sx < 4; ++sx) {
                    const uint32_t c = sp_dpad_at(x + (sx + 0.5f) / 4.0f, y + (sy + 0.5f) / 4.0f, (float)s);
                    if (!c) continue;
                    rs += (c >> 16) & 0xFF; gs += (c >> 8) & 0xFF; bs += c & 0xFF; ++n;
                }
            if (!n) continue;
            const uint32_t avg = 0xFF000000u | ((rs / n) << 16) | ((gs / n) << 8) | (bs / n);
            row[fx] = nt_blend(row[fx], avg, (int)(n * 255 / 16));
        }
    }
}
/* Walks one line: measures it (fb null) or draws it. Spaces next to a \x01 are dropped; the picture takes their place with
   sp_icon_gap of air between it and the ink of the letters either side. Returns the width. */
static int sp_walk(const OvlSurface *fb, int x0, int y, const char *s, int rom, int sc, uint32_t col)
{
    int x = x0;
    while (*s) {
        const char *e = s;
        while (*e && *e != '\x01') ++e;
        const char *te = e;
        if (*e) while (te > s && te[-1] == ' ') --te;       /* the spaces before the picture */
        if (te > s) {
            if (fb) {
                if (rom) chat_text(*fb, x, y, s, (int)(te - s), rom, sc, 0xFFFFFFFFu, 0, 255);
                chat_text(*fb, x, y, s, (int)(te - s), rom, sc, col, 1, 255);
            }
            for (const char *p = s; p < te; ++p) x += chat_adv((unsigned char)*p, rom, sc);
        }
        if (!*e) break;
        if (te > s) {               /* the picture's left edge sits one gap past the last letter's ink */
            int l, r;
            sp_ink_cols((unsigned char)te[-1], rom, &l, &r);
            x -= chat_adv((unsigned char)te[-1], rom, sc);
            x += (r + 1) * sc;
        }
        x += sp_icon_gap(sc);
        const int side = sp_icon_side(rom, sc);
        if (fb) {
            int ct, cb;
            sp_cap_rows(rom, &ct, &cb);
            const int mid = y + ((ct + cb + 1) * sc) / 2;   /* the capitals' middle */
            sp_dpad(*fb, x, mid - side / 2, side);
        }
        x += side + sp_icon_gap(sc);
        s = e + 1;
        while (*s == ' ') ++s;                              /* the spaces after it */
        if (*s && *s != '\x01') {                           /* the next letter's ink starts one gap past the picture */
            int l, r;
            sp_ink_cols((unsigned char)*s, rom, &l, &r);
            x -= l * sc;
        }
    }
    return x - x0;
}
static int sp_line_w(const char *s, int rom, int sc) { return sp_walk(nullptr, 0, 0, s, rom, sc, 0); }
static void sp_line(const OvlSurface &fb, int x, int y, const char *s, int rom, int sc, uint32_t col) { sp_walk(&fb, x, y, s, rom, sc, col); }

static void star_popup_draw(const OvlSurface &fb)
{
    port::StarPopupView v;
    port::star_popup_view(&v);
    if (v.n <= 0) return;
    const int rom = chat_font_build();
    const int scale = OVL_SCALE;
    const int big = scale * 2;                      /* twice the chat size, centred */
    const int row_h = rom ? (NT_GH + 2) * big : OVL_LINE * 2;
    const int total_h = v.n * row_h;
    int y = ntr::active_h / 4 - total_h / 2;
    if (y < 4 * scale) y = 4 * scale;
    const int pad = 6 * scale;
    /* STARPOP2: a line too wide for the window is drawn smaller, one step at a time, never cut */
    const int avail = ntr::active_w - 2 * pad;
    int lsc[port::kStarPopupLines], lw[port::kStarPopupLines];
    int maxw = 0;
    for (int i = 0; i < v.n; ++i) {
        lsc[i] = big;
        for (;;) {
            const int w = sp_line_w(v.line[i], rom, lsc[i]);
            lw[i] = w;
            if (w <= avail || lsc[i] <= 1) break;
            --lsc[i];
        }
        if (lw[i] > maxw) maxw = lw[i];
    }
    /* STARPOP3: the seconds left, yellow and a little smaller than the text, tucked into the box's bottom right corner. The
       box keeps the text's width; it only grows when the last line is so wide the count would touch it. */
    char cnt[4] = { 0 };
    if (v.count > 0) snprintf(cnt, sizeof cnt, "%d", v.count);
    const int csc100 = big * 70;
    int cw = 0;
    for (const char *p = cnt; *p; ++p) cw += chat_adv100((unsigned char)*p, rom, csc100);
    if (cw) {
        const int need = lw[v.n - 1] + 2 * (cw + 2 * pad);    /* the last line centred, the count clear of it on the right */
        if (need > maxw) maxw = need < avail ? need : avail;
    }
    int bx = (ntr::active_w - maxw) / 2 - pad;
    if (bx < 0) bx = 0;
    /* STARPOP3: never over the player list (drawn just before this, top left): when the box would cover its rows, the box
       moves down to start just under them */
    if (g_plist_drawn_bottom > 0 && bx < g_plist_drawn_right && y - pad / 2 < g_plist_drawn_bottom + scale) {
        y = g_plist_drawn_bottom + 2 * scale + pad / 2;
        if (y + total_h + pad / 2 > ntr::active_h) y = ntr::active_h - total_h - pad / 2;
    }
    ovl_shade(fb, bx, y - pad / 2, maxw + 2 * pad, total_h + pad);
    for (int i = 0; i < v.n; ++i) {
        const char *s = v.line[i];
        const int sc = lsc[i];
        const int x = (ntr::active_w - lw[i]) / 2;
        const int yy = y + i * row_h + (rom ? (NT_GH * (big - sc)) / 2 : 0);
        const uint32_t col = (i == v.n - 1 && s[0] == 'E') ? 0xFFFFE060u : 0xFFFFFFFFu;
        sp_line(fb, x, yy, s, rom, sc, col);
    }
    if (cw) {
        const int cx = bx + maxw + 2 * pad - pad / 2 - cw;
        /* the count's glyph cell bottom sits on the last line's glyph cell bottom */
        const int cy = y + (v.n - 1) * row_h + (rom ? NT_GH * big - (NT_GH * csc100 + 50) / 100 : 0);
        if (rom) chat_text100(fb, cx, cy, cnt, (int)strlen(cnt), rom, csc100, 0xFFFFFFFFu, 0, 255);
        chat_text100(fb, cx, cy, cnt, (int)strlen(cnt), rom, csc100, 0xFFFFE060u, 1, 255);
    }
}
#endif  // OVL_SCALE

#endif  // PORT_HAL_STAR_POPUP_PAINT_H
