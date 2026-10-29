/* menu_online_draw.cpp -- the title's online menu, build and draw side (run after093, lane NAME1). See menu_online.h.
   Reads only the layout and its facts blob (the decide / draw contract): no ROM, no logic state. */
#include "menu_online.h"
#include "menu_online_arrow_art.h"
#include "menu_hostimg.h"
#include "menu_pause_art_ids.h"
#include "ui_text.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace {

/* the texts (basic UI text rule: short and plain); Tango's wording pass edits these strings. */
const char *const kTxtVs = "VS rooms are made in the launcher's Multiplayer window for now.";
const char *const kTxtHelpMain = "Adventure Online: play the whole game on a shared server with up to 32 others. VS: races and fights, made in the launcher for now.";
const char *const kTxtHelpFiles = "Pick a file to join a server. Party rooms (share stars with up to 3 friends) are joined from inside Adventure Online. Your online file is separate from your solo files.";
const char *const kTxtWait = "Finding a server...";

int art_id(int kind)
{
    switch (kind) {
    case ONL_K_ONLINE: return MENU_IMG_ONL_ONLINE;
    case ONL_K_ADV_ONLINE: return MENU_IMG_ONL_ADV;
    case ONL_K_VS: return MENU_IMG_ONL_VS;
    case ONL_K_HELP: return MENU_IMG_ONL_HELP;
    case ONL_K_BACK: return MENU_IMG_ONL_BACK;
    case ONL_K_CANCEL: return MENU_IMG_ONL_CANCEL;
    case ONL_K_SOLO: return MENU_IMG_ONL_SOLO;
    }
    return 0;
}

/* the full width / height of a plank of this kind (the HD picture's own shape when there is one) */
void plank_shape(int kind, int *num, int *den)
{
    const int id = art_id(kind);
    int w = 0, h = 0;
    if (id && menu_hostimg_size(id, &w, &h) && w > 0 && h > 0) { *num = w; *den = h; return; }
    switch (kind) {
    case ONL_K_ADV_ONLINE: *num = 944; *den = 320; break;
    case ONL_K_NAME: *num = 640; *den = 320; break;      /* PROFILE1: no mod picture yet (out/PROFILE1/ART.md): code drawn */
    case ONL_K_HELP: *num = 1; *den = 1; break;
    case ONL_K_VS: *num = 512; *den = 320; break;
    default: *num = 496; *den = 320; break;
    }
}

struct Pal { int top, bot, edge, hi; };
Pal pal_of(int kind)
{
    switch (kind) {
    case ONL_K_ADV_ONLINE: return { 0xF6C060, 0xD99A38, 0x6A3A18, 0xFFE9A8 };
    case ONL_K_VS: return { 0xA6D23C, 0x78A624, 0x3E5A12, 0xD6F08A };
    default: return { 0xF58838, 0xD2641E, 0x6A2E0C, 0xFFB878 };
    }
}

inline int clampi(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }
inline uint32_t mix(uint32_t a, uint32_t b, int k /* 0..255 of b */)
{
    const int ar = (a >> 16) & 255, ag = (a >> 8) & 255, ab = a & 255, br = (b >> 16) & 255, bg = (b >> 8) & 255, bb = b & 255;
    return 0xFF000000u | (uint32_t)(((ar * (255 - k) + br * k) / 255) << 16) | (uint32_t)(((ag * (255 - k) + bg * k) / 255) << 8) | (uint32_t)((ab * (255 - k) + bb * k) / 255);
}
inline uint32_t lerp3(uint32_t a, uint32_t b, int num, int den)
{
    if (den < 1) den = 1;
    return mix(a, b, clampi(num * 255 / den, 0, 255));
}

/* a rounded rectangle test: inside amount 0..255 at pixel centre (x, y) of a rect w x h with corner radius r */
inline int rr_cov(int x, int y, int w, int h, int r)
{
    int dx = 0, dy = 0;
    if (x < r) dx = r - x - 1; else if (x >= w - r) dx = x - (w - r);
    if (y < r) dy = r - y - 1; else if (y >= h - r) dy = y - (h - r);
    if (!dx && !dy) return 255;
    const float d = sqrtf((float)(dx * dx + dy * dy)) + 0.5f;
    const float c = (float)r + 0.5f - d;
    return c >= 1.f ? 255 : c <= 0.f ? 0 : (int)(c * 255.f);
}

void put(uint32_t *dst, int stride, int w, int h, int x, int y, uint32_t rgb, int a)
{
    if (x < 0 || y < 0 || x >= w || y >= h || a <= 0) return;
    uint32_t *p = dst + (size_t)y * stride + x;
    *p = a >= 255 ? (0xFF000000u | rgb) : mix(*p, rgb, a);
}

void text_lines(const UiSurface &sf, int kind, int X, int Y, int W, int H, int shift, int alpha)
{
    const char *l1 = 0, *l2 = 0;
    switch (kind) {
    case ONL_K_ONLINE: l1 = "ONLINE"; break;
    case ONL_K_ADV_ONLINE: l1 = "ADVENTURE"; l2 = "ONLINE"; break;
    case ONL_K_VS: l1 = "VS"; break;
    case ONL_K_HELP: l1 = "?"; break;
    case ONL_K_BACK: l1 = "BACK"; break;
    case ONL_K_CANCEL: l1 = "CANCEL"; break;
    case ONL_K_SOLO: l1 = "SOLO"; break;
    case ONL_K_OK: l1 = "OK"; break;
    case ONL_K_NAME: l1 = "NAME"; break;
    default: return;
    }
    if (!ui_text_ready()) return;
    const int nl = l2 ? 2 : 1;
    float cap = (nl == 2 ? 0.25f : (kind == ONL_K_HELP ? 0.52f : 0.34f)) * (float)H;
    float em = cap / 0.75f;
    const char *longest = l1;
    if (l2 && ui_text_width(l2, em) > ui_text_width(l1, em)) longest = l2;
    const float maxw = 0.84f * (float)W;
    const float tw = ui_text_width(longest, em);
    if (tw > maxw) { em *= maxw / tw; cap = ui_text_cap_height(em); }
    const int clip[4] = { X + 2, Y + 2, X + W - 2, Y + H - 2 };
    const float gap = nl == 2 ? 0.10f * (float)H : 0.f;
    const float total = nl * cap + (nl - 1) * gap;
    float by = (float)Y + ((float)H - total) * 0.5f + cap + (float)shift;
    const char *ls[2] = { l1, l2 };
    for (int i = 0; i < nl; ++i) {
        const float x = (float)X + ((float)W - ui_text_width(ls[i], em)) * 0.5f;
        const float sh = em * 0.07f > 1.f ? em * 0.07f : 1.f;
        ui_text_draw(&sf, x + sh * 0.6f, by + sh, ls[i], em, 0xEBA546, alpha, clip);
        ui_text_draw(&sf, x, by, ls[i], em, 0x7A3A28, alpha, clip);
        by += cap + gap;
    }
}

/* the code drawn plank: the DS plank's wood, bevel and border, the label in the font ui_text has (the mod's DS lettering, else the port's plain pixel font) */
void plank_draw(const MenuLayout *L, int piece, uint32_t *dst, int w, int h, int stride)
{
    const MenuPiece &P = L->p[piece];
    const int X = L->dest[piece][0], Y = L->dest[piece][1], W = L->dest[piece][2], H = L->dest[piece][3];
    if (W < 2 || H < 2) return;
    const int kind = P.wtw;
    const int lit = P.hl & 1, pressed = (P.hl >> 1) & 1;
    const int alpha = P.alpha ? P.alpha : 255;
    const int frame = P.wth;
    static const int kPulse[8] = { 0, 2, 4, 6, 8, 6, 4, 2 };
    Pal pa = pal_of(kind);
    const int boost = lit ? 12 + kPulse[frame & 7] * 2 : 0;
    const int dark = pressed ? 40 : 0;
    const int r = clampi(H * 14 / 100, 2, 40);
    const int bw = clampi(H * 7 / 100, 1, 12);
    const int dy = pressed ? clampi(H / 20, 1, 4) : 0;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const int c = rr_cov(x, y, W, H, r);
            if (!c) continue;
            uint32_t col;
            const int ix = x - bw, iy = y - bw;
            const bool border = x < bw || y < bw || x >= W - bw || y >= H - bw || rr_cov(x - bw, y - bw, W - 2 * bw, H - 2 * bw, r > bw ? r - bw : 1) == 0;
            if (border) col = pa.edge;
            else {
                col = lerp3(pa.top, pa.bot, y, H);
                const int bev = clampi(H / 9, 2, 14);
                if (iy < bev) col = mix(col, pa.hi, 90);                       /* the lit top bevel */
                else if (y >= H - bw - bev) col = mix(col, pa.edge, 70);       /* the shaded foot */
                if (ix < bev / 2) col = mix(col, pa.hi, 50);
                if (x >= W - bw - bev / 2) col = mix(col, pa.edge, 45);
                if (((y * 7 + (x >> 3) * 3) % 23) == 0) col = mix(col, pa.edge, 28);   /* a hint of grain */
                if (boost) col = mix(col, 0xFFFFFF, boost);
                if (dark) col = mix(col, 0x000000, dark);
            }
            put(dst, stride, w, h, X + x, Y + y + dy, col & 0xFFFFFF, c * alpha / 255);
        }
    }
    if (W >= H / 3) {
        const UiSurface sf = { dst, stride, w, h };
        text_lines(sf, kind, X, Y, W, H, dy, alpha);
    }
}

/* the modal screen: a veil over the page, a parchment panel, the text (wrapped) */
void wrap_draw(const UiSurface &sf, const char *s, float x, float y, float maxw, float em, uint32_t rgb, int center)
{
    char line[200];
    const float lh = em * 1.45f;
    const char *p = s;
    while (*p) {
        int n = 0, last_sp = -1;
        line[0] = 0;
        while (p[n] && n < 190) {
            line[n] = p[n]; line[n + 1] = 0;
            if (p[n] == ' ') last_sp = n;
            if (ui_text_width(line, em) > maxw) {
                if (last_sp > 0) { n = last_sp; line[n] = 0; }
                else { line[n] = 0; }
                break;
            }
            ++n;
        }
        if (!p[n] || n >= 190) { line[n] = 0; }
        const float lw = ui_text_width(line, em);
        ui_text_draw(&sf, center ? x + (maxw - lw) * 0.5f : x, y, line, em, rgb, 255, 0);
        y += lh;
        p += n;
        while (*p == ' ') ++p;
    }
}

void modal_draw(const MenuLayout *L, int piece, uint32_t *dst, int w, int h, int stride)
{
    (void)piece;
    const OnlFacts *f = onl_facts_of(L);
    if (!f || f->scr == ONL_SCR_NONE || w < 64 || h < 64) return;
    const int un = L->unum, ud = L->uden;
    /* veil */
    for (int y = 0; y < h; ++y) {
        uint32_t *p = dst + (size_t)y * stride;
        for (int x = 0; x < w; ++x) {
            const uint32_t c = p[x];
            p[x] = 0xFF000000u | ((((c >> 16) & 255) * 96 / 255) << 16) | ((((c >> 8) & 255) * 96 / 255) << 8) | ((c & 255) * 96 / 255);
        }
    }
    const int px = w / 2 + (16 - 128) * un / ud, py = 24 * un / ud, pw = 224 * un / ud, ph = 76 * un / ud;
    const int r = clampi(ph * 8 / 100, 3, 30), bw = clampi(ph * 4 / 100, 2, 10);
    for (int y = 0; y < ph; ++y)
        for (int x = 0; x < pw; ++x) {
            if (!rr_cov(x, y, pw, ph, r)) continue;
            const bool border = x < bw || y < bw || x >= pw - bw || y >= ph - bw || rr_cov(x - bw, y - bw, pw - 2 * bw, ph - 2 * bw, r > bw ? r - bw : 1) == 0;
            uint32_t col = border ? 0x8A5A2A : lerp3(0xF4E4B8, 0xE6CE96, y, ph);
            put(dst, stride, w, h, px + x, py + y, col, 255);
        }
    const UiSurface sf = { dst, stride, w, h };
    if (!ui_text_ready()) return;
    const char *txt = kTxtVs;
    if (f->scr == ONL_SCR_HELP) txt = f->help_files ? kTxtHelpFiles : kTxtHelpMain;
    else if (f->scr == ONL_SCR_WAIT) txt = kTxtWait;
    else if (f->scr == ONL_SCR_FAIL) txt = f->why[0] ? f->why : "No server answered.";
    float em = 11.5f * (float)un / (float)ud;
    const float margin = 14.f * (float)un / (float)ud;
    const float maxw = (float)pw - 2 * margin;
    /* the panel's height decides the size: shrink until the wrapped text fits */
    for (int it = 0; it < 8; ++it) {
        int lines = 1;
        float cur = 0.f;
        char word[64];
        const char *p = txt;
        while (*p) {
            int n = 0;
            while (p[n] && p[n] != ' ' && n < 62) { word[n] = p[n]; ++n; }
            word[n] = 0;
            const float ww = ui_text_width(word, em), sp = ui_text_width(" ", em);
            if (cur > 0 && cur + sp + ww > maxw) { ++lines; cur = ww; } else cur += (cur > 0 ? sp : 0.f) + ww;
            p += n;
            while (*p == ' ') ++p;
        }
        if ((float)lines * em * 1.45f <= (float)ph - 2 * margin * 0.8f) break;
        em *= 0.9f;
    }
    const bool one = f->scr == ONL_SCR_WAIT;
    const float ty = (float)py + (one ? (float)ph * 0.5f + ui_text_cap_height(em) * 0.5f : margin * 0.8f + em);
    wrap_draw(sf, txt, (float)px + margin, ty, maxw, em, 0x6B3A12, one || f->scr == ONL_SCR_FAIL ? 1 : 0);
}

}  // namespace

extern "C" int menu_online_plank_art_ready_c(int kind) { const int id = art_id(kind); return id && menu_hostimg_ready(id); }
int menu_online_plank_art_ready(int kind) { return menu_online_plank_art_ready_c(kind); }

void menu_online_plank_state(const OnlFacts *f, int k, int *kind, int *w1000)
{
    *w1000 = 1000;
    const int solo = k == 0 ? ONL_K_ONLINE : k == 1 ? ONL_K_ADV : ONL_K_REC, onl = k == 0 ? ONL_K_SOLO : k == 1 ? ONL_K_ADV_ONLINE : ONL_K_VS;
    const int on = f ? f->on : 0, age = f ? f->age : 999;
    const int dur = 16, start = k == 0 ? 0 : (k - 1) * 3;
    const int a = age - start;
    const int from = on ? solo : onl, to = on ? onl : solo;
    if (age >= 999 || a >= dur) { *kind = to; return; }
    if (a <= 0) { *kind = from; return; }
    /* the turn: the old face narrows to a sliver over the first half, the new one widens back over the second (the file select's tile turn) */
    const int half = dur / 2;
    if (a < half) { *kind = from; *w1000 = 1000 - 1000 * a / half; }
    else { *kind = to; *w1000 = 1000 * (a - half) / half; }
    if (*w1000 < 40) *w1000 = 40;
}

/* FLIPSMOOTH1: the same turn as menu_online_plank_state, as a phase for the pictures between ticks: 1 while plank k's turn has a tick from 1 to
   dur (*a = ticks into it, *half = dur / 2, *from / *to = the faces), 0 otherwise (the pictures before tick 1 and after dur are the rest). */
int menu_online_plank_phase(const OnlFacts *f, int k, int *a, int *half, int *from, int *to)
{
    const int solo = k == 0 ? ONL_K_ONLINE : k == 1 ? ONL_K_ADV : ONL_K_REC, onl = k == 0 ? ONL_K_SOLO : k == 1 ? ONL_K_ADV_ONLINE : ONL_K_VS;
    const int on = f ? f->on : 0, age = f ? f->age : 999;
    const int dur = 16, start = k == 0 ? 0 : (k - 1) * 3;
    *a = age - start; *half = dur / 2;
    *from = on ? solo : onl; *to = on ? onl : solo;
    return age < 999 && *a >= 1 && *a <= dur;
}

int menu_online_add_plank(MenuLayout *L, int kind, int fx, int fy, int fw, int fh, int bx, int by, int bw, int bh, int lit, int pressed, int alpha,
                          int touch, int frame)
{
    MenuPiece pl = menu_piece_make(SRC_HOSTIMG, bx, by, bw, bh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    pl.lden = 1; pl.lnum = 1; pl.fw = (short)fw; pl.fh = (short)fh; pl.fx = (short)fx; pl.fy = (short)fy;
    pl.touch = (uint8_t)touch;
    if (menu_online_plank_art_ready(kind)) {
        pl.hl = (uint8_t)((lit ? 1 : 0) | (lit && pressed ? 2 : 0));
        if (alpha > 0 && alpha < 255) pl.alpha = (uint8_t)alpha;
        pl.hostimg = (uint8_t)art_id(kind);
        return menu_add_piece(L, pl);
    }
    /* no picture: a code drawn plank (a custom piece draws it, no touch of its own) and a touch only piece over the same rect for the clicks */
    MenuPiece cp = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    cp.lden = 1; cp.lnum = 1; cp.fixdest = 1; cp.fw = (short)fw; cp.fh = (short)fh; cp.fx = (short)fx; cp.fy = (short)fy;
    cp.custom = plank_draw;
    cp.wtw = (short)kind;
    cp.wth = (short)(frame & 0x7fff);
    cp.hl = (uint8_t)((lit ? 1 : 0) | (pressed ? 2 : 0));
    if (alpha > 0 && alpha < 255) cp.alpha = (uint8_t)alpha;
    menu_add_piece(L, cp);
    pl.nodraw = 1;
    return menu_add_piece(L, pl);
}

namespace {
/* NAME2: the selection arrow drawn from the code's own 22 x 15 copy of the art (the mod's picture is not there): nearest scaled to the piece's rect */
void arrow_draw(const MenuLayout *L, int piece, uint32_t *dst, int w, int h, int stride)
{
    const MenuPiece &P = L->p[piece];
    const int X = L->dest[piece][0], Y = L->dest[piece][1], W = L->dest[piece][2], H = L->dest[piece][3];
    if (W < 1 || H < 1) return;
    const int a = P.alpha ? P.alpha : 255;      /* TITLEUI1: the piece's alpha (the title's face swap fades the arrow with the wrench) */
    if (P.wtw >= 2) {
        /* TITLEART1: the orange arrow (wtw 2 = down, 3 = up), sampled bilinear from its 66 x 45 premultiplied table at the piece's own size */
        const int up = P.wtw == 3;
        for (int y = 0; y < H; ++y) {
            float sy = ((float)y + 0.5f) * 45.f / (float)H - 0.5f;
            if (up) sy = 44.f - sy;
            const int y0 = sy < 0.f ? 0 : (int)sy, y1 = y0 + 1 > 44 ? 44 : y0 + 1;
            const float fy = sy < 0.f ? 0.f : sy - (float)y0;
            for (int x = 0; x < W; ++x) {
                const float sx = ((float)x + 0.5f) * 66.f / (float)W - 0.5f;
                const int x0 = sx < 0.f ? 0 : (int)sx, x1 = x0 + 1 > 65 ? 65 : x0 + 1;
                const float fx = sx < 0.f ? 0.f : sx - (float)x0;
                const unsigned int c00 = kArrowOrangeArt[y0 * 66 + x0], c01 = kArrowOrangeArt[y0 * 66 + x1];
                const unsigned int c10 = kArrowOrangeArt[y1 * 66 + x0], c11 = kArrowOrangeArt[y1 * 66 + x1];
                float ch[4];
                for (int k = 0; k < 4; ++k) {
                    const int s = 24 - 8 * k;
                    const float t0 = (float)((c00 >> s) & 255) * (1.f - fx) + (float)((c01 >> s) & 255) * fx;
                    const float t1 = (float)((c10 >> s) & 255) * (1.f - fx) + (float)((c11 >> s) & 255) * fx;
                    ch[k] = t0 * (1.f - fy) + t1 * fy;
                }
                if (ch[0] < 0.5f) continue;
                const int r = (int)(ch[1] * 255.f / ch[0] + 0.5f), g = (int)(ch[2] * 255.f / ch[0] + 0.5f), b = (int)(ch[3] * 255.f / ch[0] + 0.5f);
                const uint32_t rgb = (uint32_t)((r > 255 ? 255 : r) << 16 | (g > 255 ? 255 : g) << 8 | (b > 255 ? 255 : b));
                put(dst, stride, w, h, X + x, Y + y, rgb, (int)(ch[0] + 0.5f) * a / 255);
            }
        }
        return;
    }
    const unsigned int *art = P.wtw ? kArrowUpArt : kArrowDownArt;
    const float bf = P.bob ? hal_menu_piece_bob_frac(piece) : 0.f;
    if (bf > 1.f / 512.f) {
        /* ARROWSLOW1: the main menu's code arrow bf of a pixel lower than its rect: each row mixes the art's row with the one above it
           (premultiplied, one row taller), so it glides between whole pixels as the HD one does */
        for (int y = 0; y <= H; ++y)
            for (int x = 0; x < W; ++x) {
                const int ax = x * 22 / W;
                const unsigned int c = y < H ? art[(y * 15 / H) * 22 + ax] : 0u, q = y > 0 ? art[((y - 1) * 15 / H) * 22 + ax] : 0u;
                const float wc = (c >> 24) ? 1.f - bf : 0.f, wq = (q >> 24) ? bf : 0.f, wa = wc + wq;
                if (wa <= 0.f) continue;
                unsigned int rgb = 0;
                for (int k = 0; k < 24; k += 8) {
                    const float v = (((c >> k) & 255u) * wc + ((q >> k) & 255u) * wq) / wa;
                    rgb |= (unsigned int)(v + 0.5f > 255.f ? 255 : (int)(v + 0.5f)) << k;
                }
                put(dst, stride, w, h, X + x, Y + y, rgb, (int)(wa * a + 0.5f));
            }
        return;
    }
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            const unsigned int c = art[(y * 15 / H) * 22 + (x * 22 / W)];
            if (c >> 24) put(dst, stride, w, h, X + x, Y + y, c & 0xFFFFFF, a);
        }
}
}  // namespace

void menu_online_add_arrow(MenuLayout *L, int up, int fx, int fy, int fw, int fh)
{
    MenuPiece cp = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    cp.bg = 1; cp.lden = 1; cp.lnum = 1; cp.fixdest = 1; cp.fw = (short)fw; cp.fh = (short)fh; cp.fx = (short)fx; cp.fy = (short)fy;
    cp.custom = arrow_draw;
    cp.wtw = (short)(up >= 2 ? up : up ? 1 : 0);      /* TITLEART1: 2 / 3 = the orange arrow, down / up */
    menu_add_piece(L, cp);
}

void menu_online_add_help(MenuLayout *L, int right_edge_x, int top_y, int size_px)
{
    const OnlFacts *pf = onl_facts_of(L);      /* PADNAV1: lit while the pad focus is on it */
    const int ip = menu_online_add_plank(L, ONL_K_HELP, right_edge_x - size_px, top_y, size_px, size_px, ONL_HELP_X, ONL_HELP_Y, ONL_HELP_W, ONL_HELP_H, pf && pf->padfocus == ONL_PF_HELP ? 1 : 0, 0, 0, 1, 0);
    if (ip >= 0) menu_add_hotspot(L, ip, ONL_HELP_X, ONL_HELP_Y, ONL_HELP_W, ONL_HELP_H, -1);
}

void menu_online_add_modal(MenuLayout *L, const OnlFacts *f)
{
    if (!f || f->scr == ONL_SCR_NONE) return;
    if (f->scr == ONL_SCR_PROFILE) { menu_online_profile_modal(L, f); return; }      /* PROFILE1: the name and colour popup builds its own pieces */
    MenuPiece p = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.bg = 1; p.fixdest = 1; p.fx = 0; p.fy = 0; p.fw = (short)L->w; p.fh = (short)L->h;
    p.custom = modal_draw;
    menu_add_piece(L, p);
    const int un = L->unum, ud = L->uden;
    const int kind = f->scr == ONL_SCR_WAIT ? ONL_K_CANCEL : ONL_K_BACK;
    int sn = 1, sd = 1;
    plank_shape(kind, &sn, &sd);
    const int dh = ONL_BTN_H * un / ud, dw = (int)((long long)dh * sn / sd);
    const int cx = L->w / 2 + (ONL_BTN_X + ONL_BTN_W / 2 - 128) * un / ud;
    const int ip = menu_online_add_plank(L, kind, cx - dw / 2, ONL_BTN_Y * un / ud, dw, dh, ONL_BTN_X, ONL_BTN_Y, ONL_BTN_W, ONL_BTN_H, 1, 0, 0, 1, 0);
    if (ip >= 0) menu_add_hotspot(L, ip, ONL_BTN_X, ONL_BTN_Y, ONL_BTN_W, ONL_BTN_H, -1);
}

int menu_online_tint_permille(const OnlFacts *f)
{
    if (!f) return 0;
    if (f->flow) return 1000;
    const int dur = 34, age = f->age;
    if (age >= 999) return f->on ? 1000 : 0;
    if (age >= dur) return f->on ? 1000 : 0;
    return f->on ? 1000 * age / dur : 1000 * (dur - age) / dur;
}

namespace {
void tint_draw(const MenuLayout *L, int piece, uint32_t *dst, int w, int h, int stride)
{
    (void)piece;
    const OnlFacts *f = onl_facts_of(L);
    const int s = menu_online_tint_permille(f);
    if (s <= 0 || w < 8 || h < 8) return;
    for (int y = 0; y < h; ++y) {
        uint32_t *p = dst + (size_t)y * stride;
        for (int x = 0; x < w; ++x) {
            const uint32_t c = p[x];
            const int r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
            const int d = b - (r > g ? r : g);                 /* how blue the pixel is */
            if (d < -10) continue;
            const int wgt = d >= 50 ? 256 : (d + 10) * 256 / 60;
            const int nr = r * 85 / 100 + 20, ng = g * 95 / 100 + (b - g) * 55 / 100, nb = b * 45 / 100 + g / 10;
            const int k = wgt * s / 1000;
            const int rr = r + (nr - r) * k / 256, gg = g + (ng - g) * k / 256, bb = b + (nb - b) * k / 256;
            p[x] = (c & 0xFF000000u) | (uint32_t)(clampi(rr, 0, 255) << 16) | (uint32_t)(clampi(gg, 0, 255) << 8) | (uint32_t)clampi(bb, 0, 255);
        }
    }
}
}  // namespace

void menu_online_add_tint(MenuLayout *L, const OnlFacts *f)
{
    if (menu_online_tint_permille(f) <= 0) return;
    MenuPiece p = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.bg = 1; p.fixdest = 1; p.fx = 0; p.fy = 0; p.fw = (short)L->w; p.fh = (short)L->h;
    p.custom = tint_draw;
    menu_add_piece(L, p);
}

int menu_online_plank_w(int kind, int fh)
{
    int sn = 1, sd = 1;
    plank_shape(kind, &sn, &sd);
    return (int)((long long)fh * sn / sd);
}
