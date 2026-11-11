// THE STAR POPUP PAINTER (tests/walk_window.cpp only; compiled after the chat
// painter and net_hud.h, whose glyph helpers it uses). Run mp1, lane STARPOP1.
// Draw-only: it reads hal/star_popup.h's view and writes overlay pixels.
#ifndef PORT_HAL_STAR_POPUP_PAINT_H
#define PORT_HAL_STAR_POPUP_PAINT_H

#include "star_popup.h"

#ifdef OVL_SCALE
/* STARPOP2: the d-pad picture a \x01 in a line stands for: a grey plus, its UP arm yellow with a dark arrow. Square, one glyph
   tall; sp_icon_w is its advance (picture plus a little air either side). */
static int sp_icon_side(int rom, int sc) { return rom ? NT_GH * sc : 8 * sc; }
static int sp_icon_w(int rom, int sc) { return sp_icon_side(rom, sc) + 4 * sc; }
static void sp_fill(const OvlSurface &fb, int x0, int y0, int w, int h, uint32_t c)
{
    for (int y = y0; y < y0 + h; ++y) {
        if (y < 0 || y >= ntr::active_h) continue;
        uint32_t *row = fb.px + (size_t)y * (size_t)fb.stride;
        for (int x = x0; x < x0 + w; ++x)
            if (x >= 0 && x < ntr::active_w) row[x] = c;
    }
}
static void sp_dpad(const OvlSurface &fb, int x0, int y0, int rom, int sc)
{
    const int s = sp_icon_side(rom, sc);
    const int a = s / 3;                 /* arm width */
    const int o = (s > 12) ? 1 + s / 24 : 1;   /* dark outline */
    const int cx = x0 + 2 * sc, cy = y0;
    const uint32_t edge = 0xFF202020u, grey = 0xFFA8A8A8u, lit = 0xFFFFE060u;
    sp_fill(fb, cx + a - o, cy - o, a + 2 * o, s + 2 * o, edge);        /* outline: vertical bar */
    sp_fill(fb, cx - o, cy + a - o, s + 2 * o, a + 2 * o, edge);        /* outline: horizontal bar */
    sp_fill(fb, cx + a, cy, a, s, grey);
    sp_fill(fb, cx, cy + a, s, a, grey);
    sp_fill(fb, cx + a, cy, a, a, lit);                                 /* the UP arm */
    for (int r = 0; r < a / 2; ++r) {                                   /* its arrow, pointing up */
        const int half = r;
        sp_fill(fb, cx + a + a / 2 - half, cy + a / 4 + r, 2 * half + 1, 1, edge);
    }
}
static int sp_line_w(const char *s, int rom, int sc)
{
    int w = 0;
    for (const char *p = s; *p; ++p) w += (*p == '\x01') ? sp_icon_w(rom, sc) : chat_adv((unsigned char)*p, rom, sc);
    return w;
}
static void sp_line(const OvlSurface &fb, int x, int y, const char *s, int rom, int sc, uint32_t col)
{
    while (*s) {
        const char *e = s;
        while (*e && *e != '\x01') ++e;
        if (e > s) {
            if (rom) chat_text(fb, x, y, s, (int)(e - s), rom, sc, 0xFFFFFFFFu, 0, 255);
            chat_text(fb, x, y, s, (int)(e - s), rom, sc, col, 1, 255);
            for (const char *p = s; p < e; ++p) x += chat_adv((unsigned char)*p, rom, sc);
        }
        if (!*e) break;
        sp_dpad(fb, x, y, rom, sc);
        x += sp_icon_w(rom, sc);
        s = e + 1;
    }
}

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
    char cnt[4] = { 0 };
    if (v.count > 0) snprintf(cnt, sizeof cnt, "%d", v.count);
    int cw = 0;
    for (const char *p = cnt; *p; ++p) cw += chat_adv((unsigned char)*p, rom, big);
    const int avail = ntr::active_w - 2 * pad - 2 * cw;
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
        const int wide = lw[i] + ((i == v.n - 1 && cw) ? 2 * (cw + pad) : 0);   /* the count's corner stays clear of the text */
        if (wide > maxw) maxw = wide;
    }
    int bx = (ntr::active_w - maxw) / 2 - pad;
    if (bx < 0) bx = 0;
    ovl_shade(fb, bx, y - pad / 2, maxw + 2 * pad, total_h + pad);
    for (int i = 0; i < v.n; ++i) {
        const char *s = v.line[i];
        const int sc = lsc[i];
        const int x = (ntr::active_w - lw[i]) / 2;
        const int yy = y + i * row_h + (rom ? (NT_GH * (big - sc)) / 2 : 0);
        const uint32_t col = (i == v.n - 1 && s[0] == 'E') ? 0xFFFFE060u : 0xFFFFFFFFu;
        sp_line(fb, x, yy, s, rom, sc, col);
    }
    if (cw) {           /* the seconds left, yellow, in the box's bottom right corner */
        const int cx = bx + maxw + 2 * pad - pad - cw;
        const int cy = y + (v.n - 1) * row_h;
        if (rom) chat_text(fb, cx, cy, cnt, (int)strlen(cnt), rom, big, 0xFFFFFFFFu, 0, 255);
        chat_text(fb, cx, cy, cnt, (int)strlen(cnt), rom, big, 0xFFFFE060u, 1, 255);
    }
}
#endif  // OVL_SCALE

#endif  // PORT_HAL_STAR_POPUP_PAINT_H
