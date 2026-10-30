/* settings_page.cpp -- see settings_page.h (run menu1, lane SETMENU1). Pure host code: no ROM, no window, no device. */
#include "settings_page.h"
#include "ui_art.h"
#include "settings_live.h"
#include "host_settings.h"
#include "ui_text.h"
#include "menu_hostimg.h"
#include "menu_compose.h"
#include "menu_pause_art_ids.h"
#include "menu_pause_geo.h"
#include "../third_party/stb/stb_image.h"

extern "C" void settings_page_press_sound(void);   /* hal/settings_page_hooks.cpp: the select sound of a button (INPUTMATRIX1) */

#if defined(_MSC_VER) || defined(__SSE2__)
#include <emmintrin.h>
#endif
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <deque>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>


namespace {

/* MENUCOST1: the cheaper draw paths are on unless SM64DS_MENU_COST_OFF=1 (the reference arithmetic, for the picture A/B proof) */
int cost_fast(void)
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_MENU_COST_OFF"); v = (e && *e && *e != '0') ? 0 : 1; }
    return v;
}

/* ================================================================== art ================================================ */

/* WARMFIX1 F1: tl_warm is set only inside settings_page_warm_step; g_pc_mx guards the picture and piece caches (swaps only) */
thread_local bool tl_warm = false;
std::mutex g_pc_mx;

enum { IM_PARCH, IM_TITLE, IM_BACK, IM_KEY, IM_PLANK, IM_DDTOP, IM_DDMID, IM_DDBOT, IM_DDARROW, IM_COUNT };

struct Img { int tried = 0, w = 0, h = 0; std::vector<uint32_t> px; };     /* premultiplied 0xAARRGGBB */
Img g_im[IM_COUNT];

/* The page's pictures are files of the "HD UI Textures" mod (hd_ui/menu/settings/<name>.png, hal/ui_art.h); the port carries none. A picture no
   enabled mod provides stays empty and the page draws its plain skin (flat rounded plates, a code-drawn chevron, text for the two lettered
   plates), so the settings page works with no mod. Decoded and premultiplied; fills t, leaves it empty on any failure. */
void decode_setart(int id, Img &t)
{
    static const char *const kName[IM_COUNT] = { "parchment", "title", "back", "key", "plank", "dd_top", "dd_mid", "dd_bot", "dd_arrow" };
    char rel[96], path[1400];
    snprintf(rel, sizeof rel, "menu/settings/%s.png", kName[id]);
    if (!ui_art_path(rel, path, sizeof path)) return;
    int w = 0, h = 0, comp = 0;
    unsigned char *p = stbi_load(path, &w, &h, &comp, 4);
    if (!p) return;
    if (w < 1 || h < 1 || w > 8192 || h > 8192) { stbi_image_free(p); return; }
    try {
        t.w = w; t.h = h; t.px.resize((size_t)w * h);
        for (int i = 0; i < w * h; ++i) {
            const unsigned r = p[i * 4], g = p[i * 4 + 1], b = p[i * 4 + 2], a = p[i * 4 + 3];
            t.px[i] = (a << 24) | (((r * a + 127) / 255) << 16) | (((g * a + 127) / 255) << 8) | ((b * a + 127) / 255);
        }
    } catch (...) { stbi_image_free(p); t = Img(); throw; }
    stbi_image_free(p);
}

void load_im(int id)
{
    Img &I = g_im[id];
    {
        std::lock_guard<std::mutex> lk(g_pc_mx);       /* WARMFIX1 F1: the claim on this slot; the background install skips a tried one */
        if (I.tried) return;
        I.tried = 1;
    }
    Img t;
    try { decode_setart(id, t); } catch (...) { t = Img(); }
    I.w = t.w; I.h = t.h; I.px = std::move(t.px);
}

/* area / bilinear resample of a sub-rectangle of a premultiplied image (the same triangle filter as hal/menu_hostimg.cpp, whose
   support grows with a reduction, so a shrink is an area average) */
struct Tap { int first, n; std::vector<float> w; };
void taps(double s0, double sn, int dn, int lim, std::vector<Tap> *out)
{
    out->resize((size_t)dn);
    const double sc = sn / dn, sup = sc > 1.0 ? sc : 1.0;
    for (int d = 0; d < dn; ++d) {
        const double c = s0 + (d + 0.5) * sc;
        int a = (int)floor(c - sup), b = (int)ceil(c + sup);
        if (a < 0) a = 0;
        if (b > lim) b = lim;
        Tap &t = (*out)[d];
        t.first = a; t.n = b - a; t.w.assign((size_t)(t.n > 0 ? t.n : 0), 0.f);
        double sum = 0;
        for (int s = a; s < b; ++s) {
            const double x = fabs((s + 0.5) - c) / sup, wv = x < 1.0 ? 1.0 - x : 0.0;
            t.w[s - a] = (float)wv; sum += wv;
        }
        if (sum <= 0 || t.n <= 0) { t.first = a < lim ? a : lim - 1; t.n = 1; t.w.assign(1, 1.f); sum = 1; }
        for (int s = 0; s < t.n; ++s) t.w[s] = (float)(t.w[s] / sum);
    }
}

void scale_rect(const Img &I, int sx, int sy, int sw, int sh, int dw, int dh, std::vector<uint32_t> *out)
{
    out->assign((size_t)(dw > 0 ? dw : 0) * (dh > 0 ? dh : 0), 0);
    if (I.px.empty() || dw < 1 || dh < 1 || sw < 1 || sh < 1) return;
    std::vector<Tap> tx, ty;
    taps(sx, sw, dw, sx + sw, &tx);
    taps(sy, sh, dh, sy + sh, &ty);
    int ylo = ty[0].first, yhi = ty[0].first + ty[0].n;
    for (const Tap &t : ty) { if (t.first < ylo) ylo = t.first; if (t.first + t.n > yhi) yhi = t.first + t.n; }
    std::vector<float> mid((size_t)dw * (yhi - ylo) * 4);
    for (int y = ylo; y < yhi; ++y) {
        const uint32_t *row = &I.px[(size_t)y * I.w];
        for (int x = 0; x < dw; ++x) {
            float a = 0, r = 0, g = 0, b = 0;
            const Tap &t = tx[x];
            for (int k = 0; k < t.n; ++k) {
                const uint32_t c = row[t.first + k];
                const float wv = t.w[k];
                a += wv * (c >> 24); r += wv * ((c >> 16) & 255); g += wv * ((c >> 8) & 255); b += wv * (c & 255);
            }
            float *m = &mid[((size_t)(y - ylo) * dw + x) * 4];
            m[0] = a; m[1] = r; m[2] = g; m[3] = b;
        }
    }
    for (int y = 0; y < dh; ++y) {
        const Tap &t = ty[y];
        for (int x = 0; x < dw; ++x) {
            float a = 0, r = 0, g = 0, b = 0;
            for (int k = 0; k < t.n; ++k) {
                const float *m = &mid[((size_t)(t.first + k - ylo) * dw + x) * 4];
                const float wv = t.w[k];
                a += wv * m[0]; r += wv * m[1]; g += wv * m[2]; b += wv * m[3];
            }
            int ia = (int)(a + 0.5f), ir = (int)(r + 0.5f), ig = (int)(g + 0.5f), ib = (int)(b + 0.5f);
            if (ia > 255) ia = 255;
            if (ir > ia) ir = ia;
            if (ig > ia) ig = ia;
            if (ib > ia) ib = ia;
            (*out)[(size_t)y * dw + x] = ((uint32_t)ia << 24) | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
        }
    }
}

/* a cache of finished pieces (a scaled picture, a composed box, the sheet): keyed by kind and size */
struct Piece { int kind, a, b, c; int w, h; std::vector<uint32_t> px; bool building = false, warm_owned = false; int spans = 0; std::vector<int> lo, hi; };
std::deque<std::unique_ptr<Piece>> g_pieces;     /* a piece never moves (a caller may hold it); an erased one is retired, not freed */

/* WARMFIX1 F1: g_pc_mx guards the container only. A new piece is filled in place by its caller, so it is marked `building` and only its own
   side (the warm-up or the live draw) can find it until that side publishes (end of the warm step / end of the draw). Erased pieces are not
   freed at once (the other side may hold a pointer): they wait in g_retired until the live draw's next frame starts. */
std::deque<std::unique_ptr<Piece>> g_retired;

Piece *find_piece(int kind, int a, int b, int c)
{
    std::lock_guard<std::mutex> lk(g_pc_mx);
    for (auto &pp : g_pieces) {
        Piece &p = *pp;
        if (p.kind == kind && p.a == a && p.b == b && p.c == c && !(p.building && p.warm_owned != tl_warm)) return &p;
    }
    return 0;
}
Piece *new_piece(int kind, int a, int b, int c, int w, int h)
{
    std::unique_ptr<Piece> np(new Piece());
    np->kind = kind; np->a = a; np->b = b; np->c = c; np->w = w; np->h = h;
    np->building = true; np->warm_owned = tl_warm;
    np->px.assign((size_t)w * h, 0);                   /* the allocation happens off the lock */
    std::lock_guard<std::mutex> lk(g_pc_mx);
    if (g_pieces.size() >= 48) {
        for (size_t i = 0; i < 24; ++i) g_retired.push_back(std::move(g_pieces[i]));
        g_pieces.erase(g_pieces.begin(), g_pieces.begin() + 24);
    }
    g_pieces.push_back(std::move(np));
    return g_pieces.back().get();
}
/* this side's finished pieces become visible to the other side (a piece of kind 102 is the warm sheet, still being built across steps) */
void publish_pieces()
{
    std::lock_guard<std::mutex> lk(g_pc_mx);
    for (auto &pp : g_pieces)
        if (pp->building && pp->warm_owned == tl_warm && pp->kind != 102) pp->building = false;
}
/* the live draw owns nothing across frames, so what the warm-up retired can go now */
void free_retired()
{
    std::lock_guard<std::mutex> lk(g_pc_mx);
    g_retired.clear();
}

/* ============================================================ drawing primitives ======================================= */

typedef UiSurface Surf;

inline uint32_t over(uint32_t d, int sr, int sg, int sb, int sa /* premultiplied colour, alpha */)
{
    const int ia = 255 - sa;
    const int r = sr + (((d >> 16) & 255) * ia + 127) / 255, g = sg + (((d >> 8) & 255) * ia + 127) / 255, b = sb + ((d & 255) * ia + 127) / 255;
    return 0xFF000000u | (uint32_t)((r > 255 ? 255 : r) << 16) | (uint32_t)((g > 255 ? 255 : g) << 8) | (uint32_t)(b > 255 ? 255 : b);
}

struct Clip { int x0, y0, x1, y1; };
Clip clip_of(const Surf &s, const int *c)
{
    Clip k = { 0, 0, s.w, s.h };
    if (c) { if (c[0] > k.x0) k.x0 = c[0]; if (c[1] > k.y0) k.y0 = c[1]; if (c[2] < k.x1) k.x1 = c[2]; if (c[3] < k.y1) k.y1 = c[3]; }
    return k;
}

/* a premultiplied piece onto the surface; alpha 0..255 scales it, light (0..255) adds white under its own alpha, shade (0..255) darkens */
void blit(const Surf &s, const std::vector<uint32_t> &src, int sw, int sh, int dx, int dy, int alpha, int light, int shade, const int *clipv)
{
    const Clip k = clip_of(s, clipv);
    const int xlo = k.x0 - dx > 0 ? k.x0 - dx : 0, xhi = k.x1 - dx < sw ? k.x1 - dx : sw;      /* the visible source columns */
    for (int y = 0; y < sh; ++y) {
        const int yy = dy + y;
        if (yy < k.y0 || yy >= k.y1) continue;
        uint32_t *row = s.px + (size_t)yy * s.stride;
        const uint32_t *sp = &src[(size_t)y * sw];
        for (int x = xlo; x < xhi; ++x) {
            const int xx = dx + x;
            const uint32_t c = sp[x];
            int a = c >> 24;
            if (!a) continue;
            if (a == 255 && alpha == 255 && !light && !shade) { row[xx] = c; continue; }     /* the opaque wood: a copy */
            int r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
            if (shade) { r = r * (255 - shade) / 255; g = g * (255 - shade) / 255; b = b * (255 - shade) / 255; }
            if (light) { r += a * light / 255; g += a * light / 255; b += a * light / 255; if (r > a) r = a; if (g > a) g = a; if (b > a) b = a; }
            if (alpha < 255) { r = r * alpha / 255; g = g * alpha / 255; b = b * alpha / 255; a = a * alpha / 255; }
            row[xx] = over(row[xx], r, g, b, a);
        }
    }
}

/* MENUCOST1: the plain blit (full alpha, no light, no shade) of a cached piece: the run of fully opaque pixels in each row is one copy,
   the edge pixels take the general path. The words written are the general path's. */
void ensure_spans(Piece *p)
{
    if (p->spans) return;
    p->spans = 1;
    p->lo.assign((size_t)p->h, 0);
    p->hi.assign((size_t)p->h, 0);
    for (int y = 0; y < p->h; ++y) {
        const uint32_t *r = &p->px[(size_t)y * p->w];
        int a = 0;
        while (a < p->w && (r[a] >> 24) != 255) ++a;
        int b = a;
        while (b < p->w && (r[b] >> 24) == 255) ++b;
        if (a == p->w) a = b = 0;
        p->lo[y] = a; p->hi[y] = b;
    }
}

void blit_piece(const Surf &s, const Piece *cp, int dx, int dy)
{
    if (!cost_fast()) { blit(s, cp->px, cp->w, cp->h, dx, dy, 255, 0, 0, 0); return; }
    Piece *p = const_cast<Piece *>(cp);
    ensure_spans(p);
    for (int y = 0; y < p->h; ++y) {
        const int yy = dy + y;
        if (yy < 0 || yy >= s.h) continue;
        uint32_t *row = s.px + (size_t)yy * s.stride;
        const uint32_t *sp = &p->px[(size_t)y * p->w];
        const int lo = p->lo[y], hi = p->hi[y];
        const int cxl = -dx > 0 ? -dx : 0, cxh = s.w - dx < p->w ? s.w - dx : p->w;     /* the visible source columns */
        for (int pass = 0; pass < 2; ++pass) {
            int a = pass ? hi : 0, b = pass ? p->w : lo;
            if (a < cxl) a = cxl;
            if (b > cxh) b = cxh;
            for (int x = a; x < b; ++x) {
                const uint32_t c = sp[x];
                const int al = c >> 24;
                if (!al) continue;
                if (al == 255) { row[dx + x] = c; continue; }
                row[dx + x] = over(row[dx + x], (c >> 16) & 255, (c >> 8) & 255, c & 255, al);
            }
        }
        int a = lo > cxl ? lo : cxl, b = hi < cxh ? hi : cxh;
        if (b > a) memcpy(row + dx + a, sp + a, (size_t)(b - a) * 4);
    }
}

const Piece *plain_im_piece(int id, int w, int h);                 /* the plain skin, below */
void plain_plate(Piece *p, uint32_t rim, uint32_t fill, float rad, float rimpx);

/* a whole picture scaled to dw x dh (cached) */
const Piece *scaled_im(int id, int dw, int dh)
{
    if (dw < 1 || dh < 1) return 0;
    load_im(id);
    if (g_im[id].px.empty()) return 0;
    Piece *p = find_piece(1, id, dw, dh);
    if (p) return p;
    p = new_piece(1, id, dw, dh, dw, dh);
    scale_rect(g_im[id], 0, 0, g_im[id].w, g_im[id].h, dw, dh, &p->px);
    return p;
}

void draw_im(const Surf &s, int id, int x, int y, int w, int h, int alpha, int light, int shade, const int *clipv)
{
    const Piece *p = scaled_im(id, w, h);
    if (!p && (id == IM_TITLE || id == IM_BACK || id == IM_DDARROW)) p = plain_im_piece(id, w, h);      /* no mod art: the plain skin */
    if (p) blit(s, p->px, p->w, p->h, x, y, alpha, light, shade, clipv);
}

/* the signed distance (pixels, negative inside) from a pixel centre to the rounded rectangle [0, w) x [0, h) with corner radius r */
inline float rr_sd(float px, float py, float w, float h, float r)
{
    const float cx = w * 0.5f, cy = h * 0.5f;
    const float qx = fabsf(px - cx) - (cx - r), qy = fabsf(py - cy) - (cy - r);
    const float ox = qx > 0 ? qx : 0, oy = qy > 0 ? qy : 0;
    const float in = (qx > qy ? qx : qy);
    if (ox == 0.f && oy == 0.f) return (0.f + (in < 0 ? in : 0)) - r;       /* MENUCOST1: sqrtf(0) is 0 */
    return sqrtf(ox * ox + oy * oy) + (in < 0 ? in : 0) - r;
}
inline int cov_of(float sd) { const float c = 0.5f - sd; return c <= 0 ? 0 : (c >= 1 ? 255 : (int)(c * 255 + 0.5f)); }

void fill_rrect(const Surf &s, float x, float y, float w, float h, float r, uint32_t rgb, int alpha, const int *clipv)
{
    const Clip k = clip_of(s, clipv);
    const int x0 = (int)floorf(x) - 1, y0 = (int)floorf(y) - 1, x1 = (int)ceilf(x + w) + 1, y1 = (int)ceilf(y + h) + 1;
    const int R = (rgb >> 16) & 255, G = (rgb >> 8) & 255, B = rgb & 255;
    if (r > h * 0.5f) r = h * 0.5f;
    if (r > w * 0.5f) r = w * 0.5f;
    for (int yy = y0; yy < y1; ++yy) {
        if (yy < k.y0 || yy >= k.y1) continue;
        uint32_t *row = s.px + (size_t)yy * s.stride;
        for (int xx = x0; xx < x1; ++xx) {
            if (xx < k.x0 || xx >= k.x1) continue;
            const int c = cov_of(rr_sd(xx + 0.5f - x, yy + 0.5f - y, w, h, r)) * alpha / 255;
            if (!c) continue;
            row[xx] = over(row[xx], R * c / 255, G * c / 255, B * c / 255, c);
        }
    }
}

void fill_circle(const Surf &s, float cx, float cy, float rad, uint32_t rgb, int alpha, const int *clipv)
{
    fill_rrect(s, cx - rad, cy - rad, rad * 2, rad * 2, rad, rgb, alpha, clipv);
}

/* the whole picture one half darker, the way the ROM's own pause dims the game */
void dim_all(const Surf &s)
{
    for (int y = 0; y < s.h; ++y) {
        uint32_t *row = s.px + (size_t)y * s.stride;
        int x = 0;
#if defined(_MSC_VER) || defined(__SSE2__)
        const __m128i keep = _mm_set1_epi32(0x007F7F7F), opaque = _mm_set1_epi32((int)0xFF000000u);
        for (; x + 4 <= s.w; x += 4) {          /* four pixels a step: half of every channel, alpha forced opaque */
            const __m128i v = _mm_loadu_si128((const __m128i *)(row + x));
            _mm_storeu_si128((__m128i *)(row + x), _mm_or_si128(_mm_and_si128(_mm_srli_epi32(v, 1), keep), opaque));
        }
#endif
        for (; x < s.w; ++x) row[x] = 0xFF000000u | ((row[x] >> 1) & 0x007F7F7Fu);
    }
}

/* MENUCOST1: the same dim, left off the pixels the sheet then overwrites with its own opaque ones (the result is the same words) */
void dim_row(uint32_t *row, int x0, int x1)
{
    int x = x0;
#if defined(_MSC_VER) || defined(__SSE2__)
    const __m128i keep = _mm_set1_epi32(0x007F7F7F), opaque = _mm_set1_epi32((int)0xFF000000u);
    for (; x + 4 <= x1; x += 4) {
        const __m128i v = _mm_loadu_si128((const __m128i *)(row + x));
        _mm_storeu_si128((__m128i *)(row + x), _mm_or_si128(_mm_and_si128(_mm_srli_epi32(v, 1), keep), opaque));
    }
#endif
    for (; x < x1; ++x) row[x] = 0xFF000000u | ((row[x] >> 1) & 0x007F7F7Fu);
}

void dim_under(const Surf &s, const Piece *sh, int sx, int sy)
{
    if (!cost_fast() || !sh) { dim_all(s); return; }
    Piece *p = const_cast<Piece *>(sh);
    ensure_spans(p);
    for (int y = 0; y < s.h; ++y) {
        uint32_t *row = s.px + (size_t)y * s.stride;
        const int py = y - sy;
        int e0 = 0, e1 = 0;
        if (py >= 0 && py < p->h && p->hi[py] > p->lo[py]) {
            e0 = sx + p->lo[py]; e1 = sx + p->hi[py];
            if (e0 < 0) e0 = 0;
            if (e1 > s.w) e1 = s.w;
            if (e1 < e0) e1 = e0;
        }
        if (e1 == e0) { dim_row(row, 0, s.w); continue; }
        dim_row(row, 0, e0);
        dim_row(row, e1, s.w);
    }
}

/* ===================================================== composed pieces (cached) ======================================== */

/* the parchment sheet: tile + dark rim, rounded, w x h (u100 = the page unit x 100, only to key the cache) */
const Piece *sheet_piece(int w, int h, int u100)
{
    Piece *p = find_piece(2, w, h, u100);
    if (p) return p;
    const double u = u100 / 100.0;
    const int tile = (int)(380 * u + 0.5);
    std::vector<uint32_t> tilepx;
    {
        const Piece *tp = scaled_im(IM_PARCH, tile, tile);
        if (tp) tilepx = tp->px;
    }
    const bool have_tile = !tilepx.empty();
    p = new_piece(2, w, h, u100, w, h);
    const float rad = (float)(26 * u), rim = (float)(6 * u);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const float sd = rr_sd(x + 0.5f, y + 0.5f, (float)w, (float)h, rad);
            const int cv = cov_of(sd);
            if (!cv) continue;
            int r, g, b;
            if (sd > -rim) { r = 0x5C; g = 0x2C; b = 0x0A; }
            else if (have_tile) {
                const uint32_t c = tilepx[(size_t)(y % tile) * tile + (x % tile)];
                r = (c >> 16) & 255; g = (c >> 8) & 255; b = c & 255;
            } else { r = 0xE8; g = 0xCC; b = 0x7A; }
            if (sd > -rim - 1.0f && sd <= -rim && have_tile) {   /* soften the rim's inner edge */
                const float f = -rim - sd;               /* 0..1 */
                const uint32_t c = tilepx[(size_t)(y % tile) * tile + (x % tile)];
                const int tr = (c >> 16) & 255, tg = (c >> 8) & 255, tb = c & 255;
                r = (int)(0x5C + (tr - 0x5C) * f); g = (int)(0x2C + (tg - 0x2C) * f); b = (int)(0x0A + (tb - 0x0A) * f);
            }
            p->px[(size_t)y * w + x] = ((uint32_t)cv << 24) | ((uint32_t)(r * cv / 255) << 16) | ((uint32_t)(g * cv / 255) << 8) | (uint32_t)(b * cv / 255);
        }
    return p;
}

/* a wooden keycap or plank of any width: the art's ends stay, the middle stretches (kind 3 = key, 4 = plank) */
const Piece *slice3_piece(int kind, int imgid, int w, int h, double end_frac)
{
    Piece *p = find_piece(kind, w, h, 0);
    if (p) return p;
    load_im(imgid);
    const Img &I = g_im[imgid];
    p = new_piece(kind, w, h, 0, w, h);
    if (I.px.empty()) {                                           /* no mod art: a flat cream keycap / orange plank */
        if (kind == 3) plain_plate(p, 0x8A5A10, 0xFFF3CD, h * 0.24f, h * 0.07f + 1.f);
        else plain_plate(p, 0x8A5A10, 0xF0B850, h * 0.24f, h * 0.07f + 1.f);
        return p;
    }
    const int aw = (int)(I.w * (double)h / I.h + 0.5);        /* the art at this height */
    std::vector<uint32_t> full;
    scale_rect(I, 0, 0, I.w, I.h, aw, h, &full);
    int endw = (int)(aw * end_frac + 0.5);
    if (w < aw) endw = (int)(endw * (double)w / aw);          /* narrower than the art: both ends shrink with it */
    for (int x = 0; x < w; ++x) {
        int sx;
        if (w <= aw) sx = (int)((x + 0.5) * aw / w);
        else if (x < endw) sx = x;
        else if (x >= w - endw) sx = aw - (w - x);
        else sx = endw + (int)((x - endw) * (double)(aw - 2 * endw) / (double)(w - 2 * endw));
        if (sx < 0) sx = 0;
        if (sx >= aw) sx = aw - 1;
        for (int y = 0; y < h; ++y) p->px[(size_t)y * w + x] = full[(size_t)y * aw + sx];
    }
    return p;
}

/* the purple wooden panel (the drop-down box and its list), w x h: the art's top strip, its body cut (not squashed) to the room, its bottom strip */
const Piece *purple_piece(int w, int h)
{
    Piece *p = find_piece(5, w, h, 0);
    if (p) return p;
    load_im(IM_DDTOP); load_im(IM_DDMID); load_im(IM_DDBOT);
    p = new_piece(5, w, h, 0, w, h);
    const Img &T = g_im[IM_DDTOP], &M = g_im[IM_DDMID], &Bm = g_im[IM_DDBOT];
    if (M.px.empty() || T.px.empty() || Bm.px.empty()) { plain_plate(p, 0xC9A6EE, 0x8458B8, h * 0.16f, 2.f); return p; }       /* no mod art: a flat purple box */
    const double st = h / 180.0 < 0.25 ? 0.25 : (h / 180.0 > 1.0 ? 1.0 : h / 180.0);
    const double across = (double)w / M.w;
    int th = (int)(T.h * across * 3 * st + 0.5); if (th < 2) th = 2;
    int bh = (int)(Bm.h * across * 3 * st + 0.5); if (bh < 1) bh = 1;
    if (th + bh > h) { th = h / 4; bh = h / 8; if (th < 1) th = 1; if (bh < 1) bh = 1; }
    const int mh = h - th - bh;
    std::vector<uint32_t> body, top, bot;
    double srcH = mh / across; if (srcH > M.h) srcH = M.h;
    int sh = (int)(srcH + 0.5); if (sh < 1) sh = 1;
    if (mh > 0) scale_rect(M, 0, 0, M.w, sh, w, mh, &body);
    scale_rect(T, 0, 0, T.w, T.h, w, th, &top);
    scale_rect(Bm, 0, 0, Bm.w, Bm.h, w, bh, &bot);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            uint32_t c;
            if (y < th) c = top[(size_t)y * w + x];
            else if (y >= h - bh) c = bot[(size_t)(y - (h - bh)) * w + x];
            else c = body.empty() ? 0 : body[(size_t)(y - th) * w + x];
            p->px[(size_t)y * w + x] = c;
        }
    const float rad = h * 0.16f;                               /* rounded corners */
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const int cv = cov_of(rr_sd(x + 0.5f, y + 0.5f, (float)w, (float)h, rad));
            if (cv >= 255) continue;
            uint32_t &c = p->px[(size_t)y * w + x];
            const int a = (c >> 24) * cv / 255;
            c = ((uint32_t)a << 24) | ((((c >> 16) & 255) * cv / 255) << 16) | ((((c >> 8) & 255) * cv / 255) << 8) | ((c & 255) * cv / 255);
        }
    return p;
}

/* ============================== the plain skin: what the page draws when no mod provides a picture (run hunt8, lane UIMOD1) ============ */

/* a rounded plate into a piece: a rim of rimpx, then the fill; rim and fill are 0xRRGGBB, opaque */
void plain_plate(Piece *p, uint32_t rim, uint32_t fill, float rad, float rimpx)
{
    const int w = p->w, h = p->h;
    if (rad > h * 0.5f) rad = h * 0.5f;
    if (rad > w * 0.5f) rad = w * 0.5f;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const float sd = rr_sd(x + 0.5f, y + 0.5f, (float)w, (float)h, rad);
            const int cv = cov_of(sd);
            if (!cv) continue;
            const uint32_t c = sd > -rimpx ? rim : fill;
            p->px[(size_t)y * w + x] = ((uint32_t)cv << 24) | ((((c >> 16) & 255) * cv / 255) << 16) | ((((c >> 8) & 255) * cv / 255) << 8) | ((c & 255) * cv / 255);
        }
}

/* the Settings title plate, the Back plate (both carry their word in the mod's art) and the drop-down's chevron, drawn by code */
const Piece *plain_im_piece(int id, int w, int h)
{
    if (w < 2 || h < 2) return 0;
    Piece *p = find_piece(8, id, w, h);
    if (p) return p;
    p = new_piece(8, id, w, h, w, h);
    if (id == IM_DDARROW) {                                      /* a white chevron: two thick strokes */
        const float ax = w * 0.14f, ay = h * 0.22f, bx = w * 0.5f, by = h * 0.80f, cx = w * 0.86f, cy = ay, th = h * 0.20f;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                float best = 1e9f;
                for (int sgm = 0; sgm < 2; ++sgm) {
                    const float x0 = sgm ? bx : ax, y0 = sgm ? by : ay, x1 = sgm ? cx : bx, y1 = sgm ? cy : by;
                    const float dx = x1 - x0, dy = y1 - y0, px = x + 0.5f - x0, py = y + 0.5f - y0;
                    float t = (px * dx + py * dy) / (dx * dx + dy * dy);
                    t = t < 0 ? 0 : (t > 1 ? 1 : t);
                    const float ex = px - t * dx, ey = py - t * dy, d = sqrtf(ex * ex + ey * ey);
                    if (d < best) best = d;
                }
                const int cv = cov_of(best - th * 0.5f);
                if (cv) p->px[(size_t)y * w + x] = ((uint32_t)cv << 24) | ((uint32_t)cv << 16) | ((uint32_t)cv << 8) | (uint32_t)cv;
            }
        return p;
    }
    plain_plate(p, 0x8A5A10, id == IM_TITLE ? 0xFFF3CD : 0xFFF3CD, h * 0.22f, h * 0.07f + 1.f);
    const char *label = id == IM_TITLE ? "SETTINGS" : "BACK";
    float em = h * 0.52f;
    const float maxw = w * 0.86f, tw = ui_text_width(label, em);
    if (tw > maxw) em *= maxw / tw;
    UiSurface us = { p->px.data(), w, w, h };
    ui_text_draw(&us, (w - ui_text_width(label, em)) * 0.5f, h * 0.5f + ui_text_cap_height(em) * 0.5f, label, em, 0x8C4200, 255, 0);
    return p;
}

/* the title's arrow pointing right (the ROM's own selection arrow, turned a quarter), w x h */
const Piece *arrow_right_piece(int w, int h)
{
    Piece *p = find_piece(6, w, h, 0);
    if (p) return p;
    p = new_piece(6, w, h, 0, w, h);
    const uint32_t *src = menu_hostimg_scaled(MENU_IMG_ARROW_DOWN, h, w);   /* turned: its width is our height */
    if (!src) {                                                  /* no picture: a code-drawn right-pointing triangle */
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x) {
                const float fx = (x + 0.5f) / w, fy = fabsf((y + 0.5f) / h - 0.5f) * 2.f;      /* 0..1, 0..1 from the middle */
                const float d = (fy - (1.f - fx)) * h * 0.5f;                                   /* inside when fy < 1 - fx */
                const int cv = cov_of(d);
                if (cv) p->px[(size_t)y * w + x] = ((uint32_t)cv << 24) | ((uint32_t)(0x2A * cv / 255) << 16) | ((uint32_t)(0x5A * cv / 255) << 8) | (uint32_t)(0xE0 * cv / 255);
            }
        return p;
    }
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) p->px[(size_t)y * w + x] = src[(size_t)x * h + (h - 1 - y)];   /* dest(x, y) = src(h - 1 - y, x) */
    return p;
}

/* =========================================================== state and rows ============================================ */

enum Tab { T_GENERAL = 0, T_DISPLAY, T_CONTROLS, T_SOUND, T_PATCHES, T_ONLINE, T_CHEATS, T_COUNT };
/* CHEATS1: the strip shows TAB_VIS of the T_COUNT tabs, a window that keeps the selected tab in the middle where it can
   (start = clamp(tab - 2, 0, T_COUNT - TAB_VIS)); a window change slides over TAB_SLIDE page frames, a wrap snaps */
enum { TAB_VIS = 5, TAB_SLIDE = 7 };
inline int tab_win_of(int tab) { const int w = tab - TAB_VIS / 2; return w < 0 ? 0 : (w > T_COUNT - TAB_VIS ? T_COUNT - TAB_VIS : w); }
enum RType { R_CHOICE, R_SLIDER, R_CHECK, R_BIND, R_HEAD, R_NOTE };
struct Row { RType t; const char *label; int id, id2, step; const char *const *names; int nn; const int *vals; int id3p1 = 0; };   /* vals: the value of each choice when it is not min + index (the frame rate stops) */

int ch_val(const Row &R, int i)
{
    const SlInfo *I = sl_info(R.id);
    return R.vals ? R.vals[i] : (I ? I->min + i : i);
}
int ch_idx(const Row &R, int v)
{
    if (!R.vals) {
        const SlInfo *I = sl_info(R.id);
        const int k = v - (I ? I->min : 0);
        return k < 0 ? 0 : (k >= R.nn ? R.nn - 1 : k);
    }
    int best = 0;
    for (int i = 1; i < R.nn; ++i) if (abs(R.vals[i] - v) < abs(R.vals[best] - v)) best = i;
    return best;
}
enum { MAXR = 40 };                 /* rows a tab can hold */

const char *const RUN_LABELS[] = { "Hold button", "Analog stick", "Always run" };
const char *const CAM_LABELS[] = { "Analog chase", "Free camera", "DS stepped" };
const char *const GAPLOOK_LABELS[] = { "Solid", "Blur", "Peek" };
const char *const GAPCOLOR_LABELS[] = { "Black", "White", "Gray", "Blue", "Red", "Green" };

const char *const CHAT_LABELS[] = { "75%", "100%", "125%", "150%", "200%" };
const int CHAT_VALS[] = { 75, 100, 125, 150, 200 };
const Row ROWS_GENERAL[] = {
    { R_SLIDER, "Minimap size", SL_MINIMAP_SCALE, -1, 5, 0, 0 },
    { R_NOTE, "The minimap is the small map in the corner.", -1, -1, 0, 0, 0 },
    { R_CHECK, "Coins in the middle", SL_COINS_MIDDLE, -1, 0, 0, 0 },
    { R_NOTE, "Puts the coin counter at the top middle, like SM64.", -1, -1, 0, 0, 0 },
};
/* PATCHES: changes to how the original game plays. "Keep lives between sessions" moved here from GENERAL (same key, SL_KEEP_LIVES,
   same default off). "Fix original bugs" (key FixOriginalBugs, default on, SM64DS_FIX_ORIGINAL_BUGS=0 off) is defined in
   settings_live by the lane that implements it; the row is looked up by key name, like Draw distance, so it shows once that key
   exists in a build and the page builds without it. New patch rows go in patches_rows(). */
Row g_patch_rows[6];
const Row *patches_rows(int *n)
{
    int k = 0;
    g_patch_rows[k++] = Row{ R_CHECK, "Keep lives between sessions", SL_KEEP_LIVES, -1, 0, 0, 0 };
    g_patch_rows[k++] = Row{ R_NOTE, "Off is the DS: every file starts with 4 lives.", -1, -1, 0, 0, 0 };
    const int fb = sl_find("FixOriginalBugs");
    if (fb >= 0) g_patch_rows[k++] = Row{ R_CHECK, "Fix original bugs", fb, -1, 0, 0, 0 };
    *n = k;
    return g_patch_rows;
}
/* PAUSE3: CONTROLS is the old CONTROLS and CAMERA tabs together (the camera rows first, then the run rows, then the bindings) */
const Row ROWS_CONTROLS[] = {
    { R_CHOICE, "Camera", SL_CAMERA_MODE, -1, 0, CAM_LABELS, 3 },
    { R_CHOICE, "Run", SL_RUN_MODE, -1, 0, RUN_LABELS, 3 },
    { R_SLIDER, "Camera speed", SL_CAMERA_SENSITIVITY, -1, 5, 0, 0 },
    { R_SLIDER, "Mouse speed", SL_MOUSE_SENSITIVITY, -1, 5, 0, 0 },
    { R_CHECK, "Invert camera left/right", SL_SWAP_CAMERA_TURN, -1, 0, 0, 0 },
    { R_CHECK, "Invert camera up/down", SL_INVERT_CAMERA_Y, -1, 0, 0, 0 },
    { R_CHECK, "Lock mouse", SL_MOUSE_CAPTURE, -1, 0, 0, 0 },
    { R_CHECK, "Right stick zooms", SL_CAMERA_STICK_ZOOM, -1, 0, 0, 0 },
    { R_SLIDER, "Stick deadzone", SL_STICK_DEADZONE, -1, 1, 0, 0 },
    { R_SLIDER, "Run point", SL_RUN_THRESHOLD, -1, 1, 0, 0 },
    { R_HEAD, "Keys and buttons", -1, -1, 0, 0, 0 },
    { R_BIND, "Up", SL_KEY_BASE + HOST_KEY_UP, SL_KEY_BASE + HOST_KEY_UP_ALT, 0, 0, 0 },
    { R_BIND, "Down", SL_KEY_BASE + HOST_KEY_DOWN, SL_KEY_BASE + HOST_KEY_DOWN_ALT, 0, 0, 0 },
    { R_BIND, "Left", SL_KEY_BASE + HOST_KEY_LEFT, SL_KEY_BASE + HOST_KEY_LEFT_ALT, 0, 0, 0 },
    { R_BIND, "Right", SL_KEY_BASE + HOST_KEY_RIGHT, SL_KEY_BASE + HOST_KEY_RIGHT_ALT, 0, 0, 0 },
    { R_BIND, "Jump", SL_KEY_BASE + HOST_KEY_JUMP, SL_PAD_BASE + HOST_PAD_JUMP, 0, 0, 0 },
    { R_BIND, "Attack", SL_KEY_BASE + HOST_KEY_ATTACK, SL_PAD_BASE + HOST_PAD_ATTACK, 0, 0, 0 },
    { R_BIND, "Crouch", SL_KEY_BASE + HOST_KEY_CROUCH, SL_PAD_BASE + HOST_PAD_CROUCH, 0, 0, 0 },
    { R_BIND, "Run", SL_KEY_BASE + HOST_KEY_RUN, SL_PAD_BASE + HOST_PAD_RUN, 0, 0, 0 },
    { R_BIND, "Start", SL_KEY_BASE + HOST_KEY_START, SL_PAD_BASE + HOST_PAD_START, 0, 0, 0 },
    { R_BIND, "Select", SL_KEY_BASE + HOST_KEY_SELECT, SL_PAD_BASE + HOST_PAD_SELECT, 0, 0, 0 },
    { R_BIND, "Look", SL_KEY_BASE + HOST_KEY_LOOK, SL_PAD_BASE + HOST_PAD_LOOK, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_LOOK + 1 },
    { R_BIND, "Center camera", SL_KEY_BASE + HOST_KEY_CAM_CENTER, SL_PAD_BASE + HOST_PAD_CAM_CENTER, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_CENTER + 1 },
    { R_BIND, "Rotate camera left", SL_KEY_BASE + HOST_KEY_CAM_LEFT, SL_PAD_BASE + HOST_PAD_CAM_LEFT, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_LEFT + 1 },
    { R_BIND, "Rotate camera right", SL_KEY_BASE + HOST_KEY_CAM_RIGHT, SL_PAD_BASE + HOST_PAD_CAM_RIGHT, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_RIGHT + 1 },
    { R_BIND, "Tilt camera up", SL_KEY_BASE + HOST_KEY_CAM_UP, SL_PAD_BASE + HOST_PAD_CAM_UP, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_UP + 1 },
    { R_BIND, "Tilt camera down", SL_KEY_BASE + HOST_KEY_CAM_DOWN, SL_PAD_BASE + HOST_PAD_CAM_DOWN, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_DOWN + 1 },
    { R_BIND, "Zoom camera in", SL_KEY_BASE + HOST_KEY_CAM_ZOOM_IN, SL_PAD_BASE + HOST_PAD_CAM_ZOOM_IN, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_ZOOM_IN + 1 },
    { R_BIND, "Zoom camera out", SL_KEY_BASE + HOST_KEY_CAM_ZOOM_OUT, SL_PAD_BASE + HOST_PAD_CAM_ZOOM_OUT, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_ZOOM_OUT + 1 },
    { R_BIND, "Camera mode", SL_KEY_BASE + HOST_KEY_CAM_MODE, SL_PAD_BASE + HOST_PAD_CAM_MODE, 0, 0, 0, 0, SL_MOUSE_BASE + HOST_MOUSE_CAM_MODE + 1 },
    { R_BIND, "Mouse look button", SL_MOUSE_BASE + HOST_MOUSE_CAM_HOLD, -1, 0, 0, 0 },
    { R_NOTE, "Two actions can share a button.", -1, -1, 0, 0, 0 },
};
/* CHEATS1: ONLINE holds the online rows that were in GENERAL (same keys, OnlineEvents and ChatScale) and the world party bindings (KeyPartyInvite /
   PadPartyInvite, KeyPartyLeave / PadPartyLeave), which had no row on the page. Name, colours and voice are launcher settings, not live keys. */
const Row ROWS_ONLINE[] = {
    { R_CHECK, "Online events", SL_ONLINE_EVENTS, -1, 0, 0, 0 },
    { R_CHOICE, "Chat size", SL_CHAT_SCALE, -1, 0, CHAT_LABELS, 5, CHAT_VALS },
    { R_HEAD, "World party", -1, -1, 0, 0, 0 },
    { R_BIND, "Party invite", SL_KEY_BASE + HOST_KEY_PARTY_INVITE, SL_PAD_BASE + HOST_PAD_PARTY_INVITE, 0, 0, 0 },
    { R_BIND, "Leave party", SL_KEY_BASE + HOST_KEY_PARTY_LEAVE, SL_PAD_BASE + HOST_PAD_PARTY_LEAVE, 0, 0, 0 },
    { R_NOTE, "Name, colours and voice are set in the launcher.", -1, -1, 0, 0, 0 },
};
/* CHEATS1: CHEATS holds the rows that change how the game plays, moved from GENERAL (same keys: DebugMenu, CharactersUnlockedFromStart,
   CharacterStrip; the star select's character panel still reads CharacterStrip) */
const Row ROWS_CHEATS[] = {
    { R_CHECK, "Debug menu", SL_DEBUG_MENU, -1, 0, 0, 0 },
    { R_CHECK, "Custom characters from the start", SL_CHARS_FROM_START, -1, 0, 0, 0 },
    { R_CHECK, "Character select on star select", SL_CHARACTER_STRIP, -1, 0, 0, 0 },
    { R_NOTE, "These change how the game plays. Off is the DS.", -1, -1, 0, 0, 0 },
};
const Row ROWS_SOUND[] = {
    { R_SLIDER, "Volume", SL_VOLUME, -1, 5, 0, 0 },
    { R_NOTE, "Music and effects share one volume.", -1, -1, 0, 0, 0 },
};
const char *const TAB_NAMES[T_COUNT] = { "GENERAL", "DISPLAY", "CONTROLS", "SOUND", "PATCHES", "ONLINE", "CHEATS" };

/* DISPLAY is built per call: the Frame rate row is there only when this build's settings_live has a FrameRate row (another lane makes it
   live; until then it stays a launcher setting and the note says so) */
Row g_disp_rows[12];
int g_disp_n;
const Row *display_rows(int *n)
{
    int k = 0;
    g_disp_rows[k++] = Row{ R_CHECK, "Minigame screen gap", SL_MINIGAME_GAP, -1, 0, 0, 0 };
    g_disp_rows[k++] = Row{ R_CHOICE, "Gap look", SL_GAP_LOOK, -1, 0, GAPLOOK_LABELS, 3 };
    g_disp_rows[k++] = Row{ R_CHOICE, "Gap color", SL_GAP_COLOR, -1, 0, GAPCOLOR_LABELS, 6 };
    {
        static const char *const DIST_LABELS[] = { "1x", "1.5x", "2x", "4x", "Max" };
        static const int DIST_VALS[] = { 100, 150, 200, 400, 10000 };
        const int dd = sl_find("DrawDistance");
        if (dd >= 0) g_disp_rows[k++] = Row{ R_CHOICE, "Draw distance", dd, -1, 0, DIST_LABELS, 5, DIST_VALS };
    }
    const int fr = sl_find("FrameRate");
    const SlInfo *fi = fr >= 0 ? sl_info(fr) : 0;
    static const char *const RATE_LABELS[] = { "Native", "60", "90", "120", "144", "165", "180", "240" };
    static const int RATE_VALS[] = { 0, 60, 90, 120, 144, 165, 180, 240 };
    const bool has_rate = fi && fi->live;           /* the row exists in this build (another lane makes the frame rate live): the drop-down shows */
    if (has_rate) g_disp_rows[k++] = Row{ R_CHOICE, "Frame rate", fr, -1, 0, RATE_LABELS, 8, RATE_VALS };
    if (has_rate) {
        g_disp_rows[k++] = Row{ R_NOTE, "Aspect ratio and picture quality are set", -1, -1, 0, 0, 0 };
        g_disp_rows[k++] = Row{ R_NOTE, "in the launcher.", -1, -1, 0, 0, 0 };
    } else {
        g_disp_rows[k++] = Row{ R_NOTE, "Frame rate, aspect ratio and picture quality", -1, -1, 0, 0, 0 };
        g_disp_rows[k++] = Row{ R_NOTE, "are set in the launcher.", -1, -1, 0, 0, 0 };
    }
    g_disp_n = k;
    *n = k;
    return g_disp_rows;
}

const Row *rows_of(int tab, int *n)
{
    switch (tab) {
    case T_DISPLAY: return display_rows(n);
    case T_SOUND: *n = (int)(sizeof ROWS_SOUND / sizeof ROWS_SOUND[0]); return ROWS_SOUND;
    case T_PATCHES: return patches_rows(n);
    case T_CONTROLS: *n = (int)(sizeof ROWS_CONTROLS / sizeof ROWS_CONTROLS[0]); return ROWS_CONTROLS;
    case T_ONLINE: *n = (int)(sizeof ROWS_ONLINE / sizeof ROWS_ONLINE[0]); return ROWS_ONLINE;
    case T_CHEATS: *n = (int)(sizeof ROWS_CHEATS / sizeof ROWS_CHEATS[0]); return ROWS_CHEATS;
    default: *n = (int)(sizeof ROWS_GENERAL / sizeof ROWS_GENERAL[0]); return ROWS_GENERAL;
    }
}

struct State {
    int open, pending, hold_touch, settle, settle_n, swallow_back;
    int tab, sel, col;                 /* sel == nrows: the Reset plank */
    int scroll;                        /* body pixels scrolled */
    int dd_open, dd_hi, dd_first;      /* a drop-down list is open on row sel; dd_first: top visible item when the list is clamped */
    int cap_active, cap_id, cap_frames;/* a rebind is waiting */
    int drag;                          /* a slider row being dragged, -1 none */
    int cursor_shown;                  /* the pad / key selection arrow shows */
    unsigned held_prev, hold_age[16];
    int ml_prev, mr_prev, mx_prev, my_prev, mouse_known;
    int pf;                            /* page frames since open */
    int w, h;                          /* the last drawn picture size */
    int ensure_vis;                    /* scroll to the selection on the next layout */
    char note[160]; int note_t;
    int pressed_plank;                 /* 0 none, 1 back, 2 reset (mouse held on it) */
    double draw_ms;
    /* PAUSE3: the panel inside the pause (set every tick by the pause recipe, kept across open_now): on = embedded, wide = the tall column
       at the right of the wide layout, dx = the sideways slide in picture pixels, input = 0 while the panel is slid out of sight */
    int emb_on, emb_wide, emb_dx, emb_input;
    int emb_from_pause;                /* opened from the pause (starts on GENERAL), else from the title wrench (CONTROLS) */
    int slide_t;                       /* CHEATS1: page frames into the strip's slide (1..TAB_SLIDE), 0 = settled on tab_win_of(tab) */
    double slide_from;                 /* the window start (fractional when a slide was cut short) the slide leaves from */
};
/* WARMFIX1 F1: the warm-up runs the page's own draw code, so it gets its OWN state and layout: while tl_warm is set (this thread, inside
   settings_page_warm_step only) S and L name the scratch copies, never the live ones a draw on another thread reads. */
State g_S_live, g_S_scr;
inline State &S_cur() { return tl_warm ? g_S_scr : g_S_live; }
#define S (S_cur())

void note(const char *s)
{
    snprintf(S.note, sizeof S.note, "%s", s ? s : "");
    S.note_t = s && *s ? 300 : 0;
}

}  // namespace

/* ============================================================ names =================================================== */

extern "C" const char *settings_page_key_name(int vk, char *buf, int cap)
{
    static const struct { int vk; const char *n; } T[] = {
        { 0, "NONE" }, { 0x10, "SHIFT" }, { 0x11, "CTRL" }, { 0x12, "ALT" }, { 0x20, "SPACE" }, { 0x09, "TAB" }, { 0xA0, "L SHIFT" }, { 0xA1, "R SHIFT" },
        { 0xA2, "L CTRL" }, { 0xA3, "R CTRL" }, { 0xA4, "L ALT" }, { 0xA5, "R ALT" }, { 0x14, "CAPS" }, { 0x08, "BKSP" }, { 0xC0, "`" }, { 0xBC, "," }, { 0xBE, "." },
        { 0xBF, "/" }, { 0xBA, ";" }, { 0xDE, "'" }, { 0xDB, "[" }, { 0xDD, "]" }, { 0xBD, "-" }, { 0xBB, "=" }, { 0xDC, "\\" }, { 0x26, "UP" }, { 0x28, "DOWN" },
        { 0x25, "LEFT" }, { 0x27, "RIGHT" }, { 0x0D, "ENTER" }, { 0x1B, "ESC" }, { 0x2D, "INS" }, { 0x2E, "DEL" }, { 0x24, "HOME" }, { 0x23, "END" },
        { 0x21, "PG UP" }, { 0x22, "PG DN" }, { 0x6A, "NUM *" }, { 0x6B, "NUM +" }, { 0x6D, "NUM -" }, { 0x6E, "NUM ." }, { 0x6F, "NUM /" },
    };
    for (unsigned i = 0; i < sizeof T / sizeof T[0]; ++i)
        if (T[i].vk == vk) return T[i].n;
    if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) { snprintf(buf, cap, "%c", (char)vk); return buf; }
    if (vk >= 0x70 && vk <= 0x87) { snprintf(buf, cap, "F%d", vk - 0x6F); return buf; }
    if (vk >= 0x60 && vk <= 0x69) { snprintf(buf, cap, "NUM %d", vk - 0x60); return buf; }
    snprintf(buf, cap, "KEY %d", vk);
    return buf;
}

extern "C" const char *settings_page_pad_name(int m, char *buf, int cap)
{
    switch (m) {
    case 0: return "NONE";
    case 0x1000: return "A";
    case 0x2000: return "B";
    case 0x4000: return "X";
    case 0x8000: return "Y";
    case 0x0100: return "LB";
    case 0x0200: return "RB";
    case 0x10000: return "LT";
    case 0x20000: return "RT";
    case 0x0010: return "START";
    case 0x0020: return "BACK";
    case 0x0040: return "L3";
    case 0x0080: return "R3";
    case 0x0001: return "D UP";
    case 0x0002: return "D DOWN";
    case 0x0004: return "D LEFT";
    case 0x0008: return "D RIGHT";
    default: snprintf(buf, cap, "PAD %X", (unsigned)m); return buf;
    }
}

namespace {

const char *id_name(int id, char *buf, int cap)
{
    if (id < SL_PAD_BASE) return settings_page_key_name(sl_get(id), buf, cap);
    if (id >= SL_MOUSE_BASE) {
        switch (sl_get(id)) {      /* CAMPACK1: the mouse buttons, by the Win32 VK the file stores */
        case 0: return "NONE";
        case 2: return "RIGHT";
        case 4: return "MIDDLE";
        case 5: return "BACK";
        case 6: return "FORWARD";
        default: snprintf(buf, cap, "MOUSE %d", sl_get(id)); return buf;
        }
    }
    return settings_page_pad_name(sl_get(id), buf, cap);
}

/* ============================================================ layout ================================================== */

struct RowL { int y, h; };
struct Lay {
    double u;
    int w, h;
    int sx, sy, sw, sh;               /* the sheet */
    int title[4], back[4];            /* planks: x, y, w, h */
    int tab_x[T_COUNT], tab_w[T_COUNT], tab_y, tab_h;   /* CHEATS1: settled places of the visible window's tabs; tab_w 0 = not in the window */
    float tab_pos[T_COUNT + 1], tab_gap;                  /* each tab's start along the whole strip of T_COUNT names (tab_pos[T_COUNT] = the end + gap) */
    int tab_win;                                          /* the settled window start */
    int body[4];                      /* x0, y0, x1, y1 (the clipped area) */
    int rowh, lab_x, ctl_x, ctl_w, rail_w;
    int reset[4];                     /* x, y, w, h */
    int content_h, scroll_max;
    int nrows;
    RowL r[MAXR];
    int bar[4];                       /* scroll bar track: x, y, w, h */
};
Lay g_L_live, g_L_scr;
inline Lay &L_cur() { return tl_warm ? g_L_scr : g_L_live; }
#define L (L_cur())

inline int P(double v) { return (int)(v * L.u + 0.5); }

void compute_layout(int w, int h)
{
    L.w = w; L.h = h;
    /* PAUSE3: inside the wide pause the sheet is a tall column at the right (900 x 990 units); everywhere else the page is the 900 x 664 sheet
       centred on the picture (the title wrench's page, and the 4:3 pause where it takes the whole screen), shifted by the slide offset */
    const bool col = S.emb_on && S.emb_wide;
    const int shu = col ? 990 : 664;
    const double u = col ? 0.46 * w / 900.0 : (double)w / 1024.0, v = col ? 0.92 * h / shu : (double)h / 768.0;
    L.u = u < v ? u : v;
    if (L.u < 0.3) L.u = 0.3;
    L.sw = P(900); L.sh = P(shu);
    L.sx = (col ? (int)(w * 0.74) : w / 2) - L.sw / 2 + (S.emb_on ? S.emb_dx : 0);
    L.sy = (h - L.sh) / 2 + P(36);
    if (L.sy + L.sh > h - 2) L.sy = h - 2 - L.sh;
    L.title[2] = P(380); L.title[3] = L.title[2] * 160 / 600;
    L.title[0] = L.sx + (L.sw - L.title[2]) / 2; L.title[1] = L.sy - L.title[3] / 2;
    L.back[2] = P(158); L.back[3] = L.back[2] * 182 / 320;
    L.back[0] = L.sx + P(30); L.back[1] = L.sy - L.back[3] / 2;
    L.tab_h = P(44); L.tab_y = L.sy + P(74);
    {   /* the tab names in a row, centred on the sheet. CHEATS1: the row is the TAB_VIS names of the window, so the strip is as wide as five
           names (what it was with five tabs); the names outside the window have no place (tab_w 0) */
        const float em = (float)P(27);
        const float gap = (float)P(22);   /* INPUTMATRIX1: was 34; the margins beside the first and last tab hold the LB / RB hints */
        float pos = 0, tw[T_COUNT];
        for (int i = 0; i < T_COUNT; ++i) { tw[i] = ui_text_width(TAB_NAMES[i], em); L.tab_pos[i] = pos; pos += tw[i] + gap; }
        L.tab_pos[T_COUNT] = pos;
        L.tab_gap = gap;
        const int ws = tab_win_of(S.tab);
        L.tab_win = ws;
        const float tot = L.tab_pos[ws + TAB_VIS] - gap - L.tab_pos[ws];
        const float x0 = L.sx + (L.sw - tot) * 0.5f;
        for (int i = 0; i < T_COUNT; ++i) {
            const bool vis = i >= ws && i < ws + TAB_VIS;
            L.tab_x[i] = vis ? (int)(x0 + L.tab_pos[i] - L.tab_pos[ws] + 0.5f) : 0;
            L.tab_w[i] = vis ? (int)(tw[i] + 0.5f) : 0;
        }
    }
    L.body[0] = L.sx + P(14); L.body[1] = L.tab_y + L.tab_h + P(14); L.body[2] = L.sx + L.sw - P(14); L.body[3] = L.sy + L.sh - P(84);
    L.rowh = P(54);
    L.lab_x = L.sx + P(60); L.ctl_x = L.sx + P(380); L.ctl_w = P(380); L.rail_w = P(290);
    L.reset[2] = P(190); L.reset[3] = P(46);
    L.reset[0] = L.sx + L.sw - P(40) - L.reset[2]; L.reset[1] = L.sy + L.sh - P(30) - L.reset[3];
    int n = 0;
    rows_of(S.tab, &n);
    L.nrows = n;
    L.content_h = n * L.rowh;
    const int vis = L.body[3] - L.body[1];
    L.scroll_max = L.content_h > vis ? L.content_h - vis : 0;
    if (S.ensure_vis && S.sel < n) {
        const int top = S.sel * L.rowh, bot = top + L.rowh;
        if (top < S.scroll) S.scroll = top;
        if (bot > S.scroll + vis) S.scroll = bot - vis;
        S.ensure_vis = 0;
    } else if (S.ensure_vis) S.ensure_vis = 0;
    if (S.scroll > L.scroll_max) S.scroll = L.scroll_max;
    if (S.scroll < 0) S.scroll = 0;
    for (int i = 0; i < n && i < MAXR; ++i) { L.r[i].y = L.body[1] + i * L.rowh - S.scroll; L.r[i].h = L.rowh; }
    L.bar[0] = L.sx + L.sw - P(34); L.bar[2] = P(12); L.bar[1] = L.body[1]; L.bar[3] = vis;
}

/* DDFIX1: where the opened list of the drop-down on row `sel` is drawn and hit. It opens below the box; when that would cross the bottom of the
   sheet (which sits inside the picture) it opens upward; when neither side holds it, it takes the roomier side, shows what fits and scrolls
   (dd_first follows dd_hi). Drawing, mouse hit boxes and pad / key focus all read this one answer. */
struct DdGeom { int ly, lhh, first, vis, lh, lx, lw; };
DdGeom dd_geom(int sel, int nn)
{
    DdGeom g;
    const int bh = P(46), by = L.r[sel].y + (L.rowh - bh) / 2;
    g.lh = P(44); g.lx = L.ctl_x; g.lw = L.ctl_w;
    const int pad = P(16), gap = P(2), edge = P(6);
    const int top = L.sy + edge, bot = L.sy + L.sh - edge;
    const int need = g.lh * nn + pad;
    const int below = bot - (by + bh + gap), above = (by - gap) - top;
    g.first = 0; g.vis = nn;
    if (need <= below) { g.ly = by + bh + gap; g.lhh = need; return g; }
    if (need <= above) { g.lhh = need; g.ly = by - gap - need; return g; }
    const bool dn = below >= above;
    const int room = dn ? below : above;
    int vis = (room - pad) / g.lh;
    if (vis < 1) vis = 1;
    if (vis > nn) vis = nn;
    g.vis = vis; g.lhh = g.lh * vis + pad;
    g.ly = dn ? by + bh + gap : by - gap - g.lhh;
    if (S.dd_hi < S.dd_first) S.dd_first = S.dd_hi;
    if (S.dd_hi >= S.dd_first + vis) S.dd_first = S.dd_hi - vis + 1;
    if (S.dd_first > nn - vis) S.dd_first = nn - vis;
    if (S.dd_first < 0) S.dd_first = 0;
    g.first = S.dd_first;
    return g;
}

/* the keycap widths of one binding row, for both layout and the draw */
int cap_h() { return P(46); }
int cap_w_for(const char *txt)
{
    const int kh = cap_h();
    const float em = kh * 0.50f;
    const float tw = ui_text_width(txt, em);
    const int w = (int)(tw + kh * 1.25f + 0.5f);
    const int mn = (int)(kh * 1.76f + 0.5f);
    return w < mn ? mn : w;
}

struct CapPos { int x, w; };
/* CAMPACK1: a binding row has up to three caps: key, pad, mouse (id3p1 = mouse id + 1). The third exists only beside the second. */
int bind_cols(const Row &R) { return R.id2 < 0 ? 1 : (R.id3p1 > 0 ? 3 : 2); }
int bind_id(const Row &R, int c) { return c == 0 ? R.id : (c == 1 ? R.id2 : R.id3p1 - 1); }
void caps_of(const Row &R, CapPos *cp /* [3] */)
{
    char b1[24], b2[24], b3[24];
    const char *t1 = id_name(R.id, b1, sizeof b1);
    const char *t2 = R.id2 >= 0 ? id_name(R.id2, b2, sizeof b2) : "";
    const char *t3 = bind_cols(R) == 3 ? id_name(R.id3p1 - 1, b3, sizeof b3) : "";
    cp[0].w = cap_w_for(t1);
    cp[1].w = R.id2 >= 0 ? cap_w_for(t2) : 0;
    cp[2].w = bind_cols(R) == 3 ? cap_w_for(t3) : 0;
    cp[0].x = L.ctl_x;
    cp[1].x = L.ctl_x + cp[0].w + P(24);
    cp[2].x = cp[1].x + cp[1].w + P(24);
}
/* which cap a pointer x is nearest: the last one whose left edge is within P(12) of it */
int cap_col_at(const Row &R, const CapPos *cp, int mx)
{
    int col = 0;
    for (int c = 1; c < bind_cols(R); ++c) if (mx >= cp[c].x - P(12)) col = c;
    return col;
}

/* ============================================================ row logic =============================================== */

bool row_focusable(const Row &R)
{
    if (R.t == R_HEAD || R.t == R_NOTE) return false;
    if (R.t == R_BIND) return true;
    return sl_applicable(R.id) != 0;
}

int step_of(const Row &R) { return R.step > 0 ? R.step : 1; }

/* INPUTMATRIX1 (Thunder: "all the option's sounds make select sounds", and a control that changed made none of its own): a control that changes plays
   the menu's move tick (host_menu_cursor_sound, the one the D-pad plays), at most one every 70 ms so a dragged slider ticks instead of buzzing; only
   the buttons (RESET here, BACK and the open in the frame loop) play the select sound. */
void ctl_sound()
{
    static unsigned long long last;
    const unsigned long long now = (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    if (now - last < 70) return;
    last = now;
    host_menu_cursor_sound();
}

void set_value(int id, int v)
{
    const int before = sl_get(id);
    if (!sl_set(id, v)) {
        const char *why = sl_set_refused();
        if (why && *why) note(why);
    } else if (sl_get(id) != before) ctl_sound();
}

void adjust(const Row &R, int dir, int big)
{
    const SlInfo *I = sl_info(R.id);
    if (!I) return;
    int v = sl_get(R.id);
    if (R.t == R_SLIDER) {
        v += dir * step_of(R) * (big ? 5 : 1);
        v = v < I->min ? I->min : (v > I->max ? I->max : v);
        set_value(R.id, v);
    } else if (R.t == R_CHOICE) {
        int k = ch_idx(R, v) + dir;
        if (k < 0) k = R.nn - 1;
        if (k >= R.nn) k = 0;
        set_value(R.id, ch_val(R, k));
    } else if (R.t == R_CHECK) {
        set_value(R.id, v ? 0 : 1);
    }
}

void reset_tab()
{
    int n = 0;
    const Row *rows = rows_of(S.tab, &n);
    for (int pass = 0; pass < 2; ++pass)           /* a binding restored before the one that held its key: the second pass settles it */
        for (int i = 0; i < n; ++i) {
            const Row &R = rows[i];
            if (R.t == R_HEAD || R.t == R_NOTE) continue;
            const SlInfo *a = sl_info(R.id);
            if (a && sl_get(R.id) != a->def) sl_set(R.id, a->def);
            for (int c = 1; R.t == R_BIND && c < bind_cols(R); ++c) {
                const SlInfo *b = sl_info(bind_id(R, c));
                if (b && sl_get(bind_id(R, c)) != b->def) sl_set(bind_id(R, c), b->def);
            }
        }
    sl_flush();
    note("Back to the defaults");
    settings_page_press_sound();
}

void start_capture(int id)
{
    sl_set_refused();
    if (sl_capture_start(id)) {
        S.cap_active = 1; S.cap_id = id; S.cap_frames = 0;
        note("");
    } else {
        note("That cannot be changed right now");
    }
}

double strip_win_now();
void switch_tab(int t)
{
    const bool wrap = t < 0 || t >= T_COUNT;
    if (t < 0) t = T_COUNT - 1;
    if (t >= T_COUNT) t = 0;
    if (t == S.tab) return;
    sl_flush();
    host_menu_cursor_sound();
    /* CHEATS1: the strip slides from where it stands now to the new tab's window; a wrap from one end to the other snaps */
    const double from = strip_win_now();
    if (!wrap && S.slide_t > 0 && tab_win_of(t) == tab_win_of(S.tab)) {}       /* the same window: the slide under way goes on */
    else if (!wrap && from != (double)tab_win_of(t)) { S.slide_from = from; S.slide_t = 1; }
    else S.slide_t = 0;
    S.tab = t; S.sel = 0; S.col = 0; S.scroll = 0; S.dd_open = 0; S.drag = -1;
    int n = 0;
    const Row *rows = rows_of(t, &n);
    while (S.sel < n && !row_focusable(rows[S.sel])) ++S.sel;
    S.ensure_vis = 1;
}

void move_sel(int dir)
{
    int n = 0;
    const Row *rows = rows_of(S.tab, &n);
    int s = S.sel;
    for (;;) {
        s += dir;
        if (s < 0 || s > n) return;                    /* n is the Reset plank */
        if (s == n || row_focusable(rows[s])) break;
    }
    if (S.sel != s) host_menu_cursor_sound();      /* EXTRAS093: a key / pad move onto another row */
    S.sel = s;
    if (s < n && rows[s].t != R_BIND) S.col = 0;
    S.ensure_vis = 1;
    S.dd_open = 0;
}

/* CHEATS1: the strip's window start as drawn now: settled, or TAB_SLIDE page frames from slide_from to the tab's window, eased. g_sp_pic_alpha is
   the place between two page frames of the picture being drawn (1 on a tick's own picture); settings_page_set_picture_alpha sets it for one draw. */
double g_sp_pic_alpha = 1.0;
double strip_win_now()
{
    const double to = tab_win_of(S.tab);
    if (S.slide_t <= 0) return to;
    double p = (S.slide_t - 1 + g_sp_pic_alpha) / TAB_SLIDE;
    p = p < 0 ? 0 : (p > 1 ? 1 : p);
    const double e = p * p * (3 - 2 * p);
    return S.slide_from + (to - S.slide_from) * e;
}
/* SETPAGEINTERP1: where tab i stands on the picture the player last saw, clipped to the strip's ends (0 when it is out of sight). Read by the
   mouse during a frame: the frame has already stepped the slide, and the last picture of the frame before showed that step at fraction 0. */
int tab_drawn_at(int i, float *x0, float *x1)
{
    if (S.slide_t <= 0) {
        if (L.tab_w[i] <= 0) return 0;
        *x0 = (float)L.tab_x[i]; *x1 = (float)(L.tab_x[i] + L.tab_w[i]);
        return 1;
    }
    const double keep = g_sp_pic_alpha;
    g_sp_pic_alpha = 0;
    const double f = strip_win_now();
    g_sp_pic_alpha = keep;
    const int a = (int)f, b = a + 1 < T_COUNT - TAB_VIS + 1 ? a + 1 : a;
    const float fr = (float)(f - a);
    const float off = L.tab_pos[a] + (L.tab_pos[b] - L.tab_pos[a]) * fr;
    const float wa = L.tab_pos[a + TAB_VIS] - L.tab_gap - L.tab_pos[a], wb = L.tab_pos[b + TAB_VIS] - L.tab_gap - L.tab_pos[b];
    const float tot = wa + (wb - wa) * fr;
    const float lend = L.sx + (L.sw - tot) * 0.5f, rbeg = lend + tot;
    const float x = lend + L.tab_pos[i] - off, w = L.tab_pos[i + 1] - L.tab_gap - L.tab_pos[i];
    const float l = x < lend ? lend : x, r = x + w > rbeg ? rbeg : x + w;
    if (r - l < 1) return 0;
    *x0 = l; *x1 = r;
    return 1;
}

/* ============================================================ the frame =============================================== */

int repeat_fire(int bit, unsigned held)
{
    /* an edge fires at once; a key held fires again after 16 frames and then every 3 */
    if (!((held >> bit) & 1)) { S.hold_age[bit] = 0; return 0; }
    const unsigned a = S.hold_age[bit]++;
    return a == 0 || (a > 16 && ((a - 16) % 3) == 0);
}
int edge_only(int bit, unsigned held)
{
    const int was = (S.held_prev >> bit) & 1;
    return ((held >> bit) & 1) && !was;
}

bool in_rect(int x, int y, const int *r) { return x >= r[0] && x < r[0] + r[2] && y >= r[1] && y < r[1] + r[3]; }

int row_at(int x, int y)
{
    if (x < L.body[0] || x >= L.body[2] || y < L.body[1] || y >= L.body[3]) return -1;
    for (int i = 0; i < L.nrows && i < MAXR; ++i)
        if (y >= L.r[i].y && y < L.r[i].y + L.r[i].h) return i;
    return -1;
}

void slider_geometry(int *rx, int *rw)
{
    *rx = L.ctl_x + P(17); *rw = L.rail_w - P(34);
}

void drag_slider(const Row &R, int mx)
{
    const SlInfo *I = sl_info(R.id);
    if (!I) return;
    int rx, rw;
    slider_geometry(&rx, &rw);
    double f = (double)(mx - rx) / rw;
    f = f < 0 ? 0 : (f > 1 ? 1 : f);
    int v = I->min + (int)floor(f * (I->max - I->min) + 0.5);
    const int st = step_of(R);
    v = ((v - I->min + st / 2) / st) * st + I->min;
    v = v < I->min ? I->min : (v > I->max ? I->max : v);
    if (v != sl_get(R.id)) set_value(R.id, v);
}

void activate(const Row &R, int col)
{
    switch (R.t) {
    case R_CHOICE: S.dd_open = 1; S.dd_hi = ch_idx(R, sl_get(R.id)); S.dd_first = 0; break;
    case R_CHECK: adjust(R, 1, 0); break;
    case R_BIND: start_capture(bind_id(R, col)); break;
    default: break;
    }
}

}  // namespace

/* ============================================================ public: state ============================================ */

extern "C" int settings_page_open(void) { return S.open; }
extern "C" void settings_page_request(void) { S.pending = 1; S.hold_touch = 1; }
static int g_request_src;
extern "C" void settings_page_set_source(int src) { g_request_src = src; }
extern "C" int settings_page_source(void) { return g_request_src; }
extern "C" int settings_page_pending(void) { return S.pending; }
/* HUDMENU2 (H4): the press look of the button that asked, while the page waits out its press hold (src 1 = the pause's Options plate, 2 = the title's wrench).
   A pad confirm is taken from the ROM, so the ROM never plays the plate's press; the recipes draw it from this for the length of the hold. */
extern "C" int settings_page_press_look(void) { return S.pending ? g_request_src : 0; }
extern "C" void settings_page_clear_request(void) { S.pending = 0; }
extern "C" int settings_page_tab(void) { return S.tab; }
extern "C" int settings_page_row(void) { return S.sel; }
extern "C" const char *settings_page_note(void) { return S.note; }
extern "C" double settings_page_draw_ms(void) { return S.draw_ms; }
extern "C" void settings_page_set_picture_alpha(double a) { g_sp_pic_alpha = a < 0 ? 0 : (a > 1 ? 1 : a); }
extern "C" int settings_page_strip_moving(void) { return S.open && S.slide_t > 0; }

extern "C" void settings_page_set_embed(const SpEmbed *e)
{
    S.emb_on = e ? e->on : 0;
    S.emb_wide = e ? e->wide : 0;
    S.emb_dx = e ? e->dx : 0;
    S.emb_input = e ? e->input : 1;
}

extern "C" void settings_page_open_now(int from)
{
    (void)from;
    const int w = S.w, h = S.h;
    const int eon = S.emb_on, ewide = S.emb_wide, edx = S.emb_dx, ein = S.emb_input;
    memset(&S, 0, sizeof S);
    S.w = w ? w : 1024; S.h = h ? h : 768;
    S.emb_on = eon; S.emb_wide = ewide; S.emb_dx = edx; S.emb_input = ein;
    S.emb_from_pause = eon;
    S.open = 1; S.drag = -1; S.hold_touch = 1;
    S.tab = eon ? T_GENERAL : T_CONTROLS;      /* inside the pause it opens on the first tab; the title wrench's page keeps CONTROLS */
    S.cursor_shown = 1;          /* the arrow stands on the first row from the start, whoever opened it */
    S.ensure_vis = 1;
    S.held_prev = ~0u;           /* the press that opened it is still down: no edges on the first frame */
    for (int i = 0; i < 16; ++i) S.hold_age[i] = 1;
    S.ml_prev = 1; S.mr_prev = 1;
    sl_set_refused();
    int n = 0;
    const Row *rows = rows_of(S.tab, &n);
    while (S.sel < n && !row_focusable(rows[S.sel])) ++S.sel;
}

extern "C" void settings_page_close(void)
{
    if (S.cap_active) { sl_capture_cancel(); S.cap_active = 0; }
    sl_flush();
    S.open = 0; S.pending = 0; S.hold_touch = 1;
    S.dd_open = 0; S.drag = -1;
    S.swallow_back = 2;          /* the Backspace that closed it must not reach the ROM on the frames around the close */
}

extern "C" int settings_page_swallow_back(void)
{
    if (S.open) return 1;
    if (S.swallow_back > 0) { --S.swallow_back; return 1; }
    return 0;
}

extern "C" int settings_page_swallow_touch(int touch_down)
{
    if (S.open) return 1;
    if (S.hold_touch) {
        if (touch_down) return 1;
        S.hold_touch = 0;
    }
    return 0;
}

extern "C" int settings_page_frame(const SpInput *in)
{
    if (!S.open) return 0;
    ++S.pf;
    if (S.slide_t > 0 && ++S.slide_t > TAB_SLIDE) S.slide_t = 0;      /* CHEATS1: the strip's slide, one step a page frame */
    if (S.note_t > 0 && --S.note_t == 0) S.note[0] = 0;
    compute_layout(S.w, S.h);
#ifndef PORT_ROM_CLEAN
    {   /* CHEATS1 proof line (scripted rows only): the strip's window and the settled places of its five names, each frame it changes */
        static int tr = -1, last_tab = -1, last_t = -1;
        if (tr < 0) tr = getenv("SM64DS_SETPAGE_SCRIPT") != 0;
        if (tr && (S.tab != last_tab || S.slide_t != last_t)) {
            last_tab = S.tab; last_t = S.slide_t;
            char b[200]; int k = 0;
            for (int i = L.tab_win; i < L.tab_win + TAB_VIS; ++i) k += snprintf(b + k, sizeof b - k, " %s@%d+%d", TAB_NAMES[i], L.tab_x[i], L.tab_w[i]);
            fprintf(stderr, "[strip] pf %d tab %d window %d..%d drawn at %.3f slide %d:%s\n", S.pf, S.tab, L.tab_win, L.tab_win + TAB_VIS - 1, strip_win_now(), S.slide_t, b);
        }
    }
#endif
    if (S.emb_on && !S.emb_input) {      /* PAUSE3: slid out of sight (the star tracker is showing): the page sees nothing */
        S.held_prev = in->keys; S.ml_prev = in->mouse_l; S.mr_prev = in->mouse_r;
        return 1;
    }
    /* after a rebind ends, a still-held pad button (LB, RB, A, B, Y are page keys too) is not a press: keys read as up until all are released */
    if (S.settle && (in->keys == 0 || ++S.settle_n > 120)) S.settle = 0;
    const unsigned held = S.settle ? 0u : in->keys;
    const int mx = in->mouse_x, my = in->mouse_y;
    const int mvalid = in->mouse_valid;
    const int l_edge = in->mouse_l && !S.ml_prev;
    const int r_edge = in->mouse_r && !S.mr_prev;
    const int moved = mvalid && S.mouse_known && (mx != S.mx_prev || my != S.my_prev);   /* a pointer resting where the page opened does not pick a row */
    int n = 0;
    const Row *rows = rows_of(S.tab, &n);
    int closing = 0;

    if (S.cap_active) {
        /* a rebind waiting: the engine's capture owns the keys; the page only watches and gives up */
        ++S.cap_frames;
        const int st = sl_capture_state();
        const char *why = sl_capture_refused();
        if (st == SL_CAP_BOUND) {
            S.cap_active = 0; S.settle = 1; S.settle_n = 0; note("");
            /* PAUSE3: the camera controls may share a button with each other; say so when this one does */
            const int id = S.cap_id;
            const int mouse = id >= SL_MOUSE_BASE;
            const int pad = !mouse && id >= SL_PAD_BASE;
            const int lo = mouse ? SL_MOUSE_BASE + HOST_MOUSE_CAM_CENTER : pad ? SL_PAD_BASE + HOST_PAD_CAM_CENTER : SL_KEY_BASE + HOST_KEY_CAM_CENTER;
            const int hi = mouse ? SL_MOUSE_BASE + HOST_MOUSE_CAM_MODE : pad ? SL_PAD_BASE + HOST_PAD_CAM_ZOOM_OUT : SL_KEY_BASE + HOST_KEY_CAM_ZOOM_OUT;
            if (id >= lo && id <= hi) {
                /* host order: center, left, right, mode (key / pad), up, down, zoom in, zoom out; mouse: center, left, right, up, down, zoom in, zoom out, mode */
                static const char *const cam_names_kp[8] = { "Center camera", "Rotate camera left", "Rotate camera right", "Camera mode",
                                                             "Tilt camera up", "Tilt camera down", "Zoom camera in", "Zoom camera out" };
                static const char *const cam_names_m[8] = { "Center camera", "Rotate camera left", "Rotate camera right",
                                                            "Tilt camera up", "Tilt camera down", "Zoom camera in", "Zoom camera out", "Camera mode" };
                const char *const *cam_names = mouse ? cam_names_m : cam_names_kp;
                for (int o = lo; o <= hi; ++o)
                    if (o != id && sl_get(o) == sl_get(id) && sl_get(id)) {
                        char b[160];
                        snprintf(b, sizeof b, "Shared with %s", cam_names[o - lo]);
                        note(b);
                        break;
                    }
            }
        }
        else if (st == SL_CAP_CANCELLED) { S.cap_active = 0; S.settle = 1; S.settle_n = 0; note("Kept the old binding"); }
        else if (!sl_capture_active()) { S.cap_active = 0; S.settle = 1; S.settle_n = 0; }
        else {
            if (why && *why) note(why);
            const int mrow = sl_capture_wants_mouse();      /* CAMPACK1: a mouse row takes the right / middle / side buttons as its answer, only the left click backs out */
            if (((held & SPK_CAP_CANCEL) && edge_only(9, held)) || l_edge || (r_edge && !mrow) || S.cap_frames > 900) {
                sl_capture_cancel();
                S.cap_active = 0;
                S.settle = 1; S.settle_n = 0;
                note("Kept the old binding");
            }
        }
    } else if (S.dd_open && S.sel < n) {
        /* the open drop-down list */
        const Row &R = rows[S.sel];
        const SlInfo *I = sl_info(R.id);
        (void)I;
        if (repeat_fire(0, held)) { if (S.dd_hi > 0) --S.dd_hi; }
        if (repeat_fire(1, held)) { if (S.dd_hi < R.nn - 1) ++S.dd_hi; }
        const DdGeom G = dd_geom(S.sel, R.nn);
        const int row0 = G.ly + P(8);
        if (in->wheel && G.vis < R.nn) {
            S.dd_first -= in->wheel;
            if (S.dd_first > R.nn - G.vis) S.dd_first = R.nn - G.vis;
            if (S.dd_first < 0) S.dd_first = 0;
            if (S.dd_hi < S.dd_first) S.dd_hi = S.dd_first;                 /* the highlight stays on a drawn row */
            if (S.dd_hi > S.dd_first + G.vis - 1) S.dd_hi = S.dd_first + G.vis - 1;
        }
        if (edge_only(4, held)) { set_value(R.id, ch_val(R, S.dd_hi)); S.dd_open = 0; }
        else if (edge_only(5, held)) { S.dd_open = 0; }
        else if (l_edge) {
            if (mx >= G.lx && mx < G.lx + G.lw && my >= G.ly && my < G.ly + G.lhh) {
                const int i = G.first + (my - row0) / G.lh;
                if (my >= row0 && i >= G.first && i < G.first + G.vis && i < R.nn) set_value(R.id, ch_val(R, i));
            }
            S.dd_open = 0;
        } else if (moved && mx >= G.lx && mx < G.lx + G.lw) {
            const int i = G.first + (my - row0) / G.lh;
            if (my >= row0 && i >= G.first && i < G.first + G.vis && i < R.nn) S.dd_hi = i;
        }
    } else {
        /* ---- navigation ---- */
        if (edge_only(5, held)) closing = 1;
        if (edge_only(6, held)) { switch_tab(S.tab - 1); S.cursor_shown = 1; }
        if (edge_only(7, held)) { switch_tab(S.tab + 1); S.cursor_shown = 1; }
        rows = rows_of(S.tab, &n);
        if (repeat_fire(0, held)) { move_sel(-1); S.cursor_shown = 1; }
        if (repeat_fire(1, held)) { move_sel(+1); S.cursor_shown = 1; }
        const int lf = repeat_fire(2, held), rt = repeat_fire(3, held);
        if (lf || rt) {
            S.cursor_shown = 1;
            const int dir = rt ? 1 : -1;
            if (S.sel < n) {
                const Row &R = rows[S.sel];
                if (R.t == R_BIND) { int c = S.col + dir; if (c < 0) c = 0; if (c > bind_cols(R) - 1) c = bind_cols(R) - 1; S.col = c; }
                else if (R.t == R_SLIDER || R.t == R_CHOICE) adjust(R, dir, 0);
                else if (R.t == R_CHECK && ((dir > 0) != (sl_get(R.id) != 0))) adjust(R, dir, 0);
            }
        }
        if (edge_only(4, held) && S.sel <= n) {
            S.cursor_shown = 1;
            if (S.sel == n) reset_tab();
            else activate(rows[S.sel], S.col);
        }
        if (edge_only(8, held) && S.sel < n && rows[S.sel].t == R_BIND) {
            const Row &R = rows[S.sel];
            set_value(bind_id(R, S.col < bind_cols(R) ? S.col : 0), 0);
        }

        /* ---- the mouse ---- */
        if (mvalid) {
            const int over_row = row_at(mx, my);
            if (moved) {
                S.cursor_shown = 1;
                if (over_row >= 0 && over_row < n && row_focusable(rows[over_row])) {
                    if (S.sel != over_row) { S.sel = over_row; S.col = 0; host_menu_cursor_sound(); }   /* EXTRAS093: the mouse arrived on another row */
                    if (rows[over_row].t == R_BIND && rows[over_row].id2 >= 0) {
                        CapPos cp[3]; caps_of(rows[over_row], cp);
                        S.col = cap_col_at(rows[over_row], cp, mx);
                    }
                } else if (in_rect(mx, my, L.reset)) { if (S.sel != n) host_menu_cursor_sound(); S.sel = n; }
            }
            if (in->wheel) {
                S.scroll -= in->wheel * (L.rowh * 3 / 2);
                if (S.scroll < 0) S.scroll = 0;
                if (S.scroll > L.scroll_max) S.scroll = L.scroll_max;
            }
            if (l_edge) {
                S.cursor_shown = 1;
                if (in_rect(mx, my, L.back)) { S.pressed_plank = 1; }
                else if (in_rect(mx, my, L.reset)) { S.pressed_plank = 2; S.sel = n; }
                else {
                    int tabhit = -1;
                    for (int i = 0; i < T_COUNT; ++i) {
                        float tx0, tx1;     /* SETPAGEINTERP1: where it is drawn, during a slide too */
                        if (tab_drawn_at(i, &tx0, &tx1) && mx >= tx0 - P(10) && mx < tx1 + P(10) && my >= L.tab_y && my < L.tab_y + L.tab_h) tabhit = i;
                    }
                    if (tabhit >= 0) switch_tab(tabhit);
                    else if (over_row >= 0 && over_row < n && row_focusable(rows[over_row])) {
                        const Row &R = rows[over_row];
                        S.sel = over_row;
                        if (R.t == R_SLIDER) {
                            if (mx >= L.ctl_x - P(10) && mx < L.ctl_x + L.rail_w + P(10)) { S.drag = over_row; drag_slider(R, mx); }
                        } else if (R.t == R_CHECK) {
                            if (mx >= L.lab_x && mx < L.ctl_x + L.ctl_w) adjust(R, 1, 0);
                        } else if (R.t == R_CHOICE) {
                            if (mx >= L.ctl_x && mx < L.ctl_x + L.ctl_w) activate(R, 0);
                        } else if (R.t == R_BIND) {
                            CapPos cp[3]; caps_of(R, cp);
                            const int kh = cap_h(), ky = L.r[over_row].y + (L.rowh - kh) / 2;
                            if (my >= ky && my < ky + kh)
                                for (int c = 0; c < bind_cols(R); ++c)
                                    if (mx >= cp[c].x && mx < cp[c].x + cp[c].w) { S.col = c; activate(R, c); break; }
                        }
                    }
                }
            }
            if (r_edge && over_row >= 0 && over_row < n && rows[over_row].t == R_BIND) {
                CapPos cp[3]; caps_of(rows[over_row], cp);
                const int col = cap_col_at(rows[over_row], cp, mx);
                set_value(bind_id(rows[over_row], col), 0);
            }
            if (S.drag >= 0 && in->mouse_l && S.drag < n) drag_slider(rows[S.drag], mx);
            if (S.pressed_plank && !in->mouse_l) {
                if (S.pressed_plank == 1 && in_rect(mx, my, L.back)) closing = 1;
                else if (S.pressed_plank == 2 && in_rect(mx, my, L.reset)) reset_tab();
                S.pressed_plank = 0;
            }
        }
        if (S.drag >= 0 && !in->mouse_l) { S.drag = -1; sl_flush(); }
    }

    if (S.sel > L.nrows) S.sel = L.nrows;
    S.held_prev = held;
    S.ml_prev = in->mouse_l; S.mr_prev = in->mouse_r;
    if (mvalid) { S.mx_prev = mx; S.my_prev = my; S.mouse_known = 1; }
    if (closing) { settings_page_close(); return 0; }
    return 1;
}

/* ============================================================ the draw ================================================ */

namespace {

const uint32_t C_TEXT = 0x7A3A0C, C_QUIET = 0xA06E3C, C_LETTER = 0x8C4200, C_CREAM = 0xFFF3CD;

void text_c(const Surf &s, float cx, float by, const char *t, float em, uint32_t rgb, int alpha, const int *clip)
{
    const float w = ui_text_width(t, em);
    ui_text_draw(&s, cx - w * 0.5f, by, t, em, rgb, alpha, clip);
}
/* a line of text vertically centred (by its capital height) on cy */
void text_mid(const Surf &s, float x, float cy, const char *t, float em, uint32_t rgb, int alpha, const int *clip)
{
    ui_text_draw(&s, x, cy + ui_text_cap_height(em) * 0.5f, t, em, rgb, alpha, clip);
}

void draw_cap(const Surf &s, int x, int y, int w, int h, const char *txt, int sel, int waiting, const int *clip)
{
    const Piece *p = slice3_piece(3, IM_KEY, w, h, 110.0 / 392.0);
    if (p) blit(s, p->px, p->w, p->h, x, y, 255, sel ? 40 : 0, 0, clip);
    const float em = h * 0.50f;
    text_c(s, x + w * 0.5f, y + h * 0.5f + ui_text_cap_height(em) * 0.5f, waiting ? "..." : txt, em, C_LETTER, 255, clip);
}

void draw_dd_box(const Surf &s, int x, int y, int w, int h, const char *txt, int sel, int alpha, const int *clip)
{
    const Piece *p = purple_piece(w, h);
    if (p) blit(s, p->px, p->w, p->h, x, y, alpha, sel ? 30 : 0, 0, clip);
    const float em = h * 0.56f;
    text_mid(s, (float)(x + P(18)), y + h * 0.5f, txt, em, 0xFFFFFF, alpha, clip);
    const int aw = h * 40 / 46, ah = aw * 58 / 92;
    draw_im(s, IM_DDARROW, x + w - aw - P(14), y + (h - ah) / 2, aw, ah, alpha, 0, 0, clip);
}

void draw_check(const Surf &s, int x, int y, int sz, int on, int sel, int alpha, const int *clip)
{
    fill_rrect(s, (float)x, (float)y, (float)sz, (float)sz, sz * 0.19f, C_TEXT, alpha, clip);
    const float b = sz * 0.105f;
    fill_rrect(s, x + b, y + b, sz - 2 * b, sz - 2 * b, sz * 0.12f, sel ? 0xFFFAE6 : C_CREAM, alpha, clip);
    if (on) {
        const float em = sz * 0.72f;
        const float w = ui_text_width("X", em);
        ui_text_draw(&s, x + sz * 0.5f - w * 0.5f, y + sz * 0.5f + ui_text_cap_height(em) * 0.5f, "X", em, C_TEXT, alpha, clip);
    }
}

void draw_slider(const Surf &s, int x, int cy, int rail_w, int v, int vmin, int vmax, int alpha, const int *clip)
{
    const float rh = (float)P(12);
    const float kr = (float)P(17);
    fill_rrect(s, (float)x, cy - rh * 0.5f, (float)rail_w, rh, rh * 0.5f, C_TEXT, alpha, clip);
    const float f = vmax > vmin ? (float)(v - vmin) / (vmax - vmin) : 0;
    const float kx = x + kr + f * (rail_w - 2 * kr);
    fill_circle(s, kx, (float)cy, kr, C_TEXT, alpha, clip);
    fill_circle(s, kx, (float)cy, kr - P(4), C_CREAM, alpha, clip);
}

}  // namespace

static int g_warming;
static int g_ws_cp, g_ws_cpn = 1;   /* WARMFIX1 F5: a tab's chrome is three warm steps: 1 = the sheet, title and back; 2 = the tabs; 3 = the scroll bar and footer (0 = all, as the live draw) */
static int g_ws_r0, g_ws_r1;       /* PAUSEFIX1: while warming, the draw does only rows [r0, r1) of the tab (the chrome is always drawn) */

/* PAUSEFIX1: the page's caches built in small steps, one per call, never more than about 2 ms of work a call (the old one-shot warm-up was one
   55 ms frame in every level). Stages: layout, the parchment tile, the sheet in row slices, the title and back pictures, then one throwaway
   draw per tab (glyphs, key caps, boxes). The caller (hooks.cpp) claims the shared warm-up slot first. Returns 1 when everything is cached
   for this picture size (a resize starts again). */
static std::vector<uint32_t> g_ws_tile, g_ws_buf;
static int g_ws_stage, g_ws_w, g_ws_h, g_ws_y, g_ws_tilesz, g_ws_u100, g_ws_tile_ok, g_ws_tab, g_ws_row;

/* The page's pictures are decoded on a worker thread (a 7 ms decode on the game thread is a visible tick). One request at a time; the game
   thread installs the finished picture into g_im (a vector move). Same decode and premultiply as load_im. */
namespace {
struct WDec {
    std::mutex m;
    std::condition_variable cv;
    int req = -1, done = -1;
    Img img;
    bool started = false;
    /* WARMFIX1 F5: the same worker also scales a whole picture (a 6 to 12 ms scale on the game thread is a visible tick) */
    bool sreq = false, sdone = false;
    int sid = 0, sdw = 0, sdh = 0;
    std::vector<uint32_t> spx;
};
WDec *g_wd;

void wdec_main(WDec *W)
{
    for (;;) {
        int id;
        {
            std::unique_lock<std::mutex> lk(W->m);
            W->cv.wait(lk, [W] { return W->req >= 0 || W->sreq; });
            if (W->sreq) {
                const int sid = W->sid, sdw = W->sdw, sdh = W->sdh;
                lk.unlock();
                std::vector<uint32_t> out;
                try { scale_rect(g_im[sid], 0, 0, g_im[sid].w, g_im[sid].h, sdw, sdh, &out); }      /* g_im[sid] is decoded and never changes again */
                catch (...) { out.clear(); }
                lk.lock();
                W->spx = std::move(out);
                W->sreq = false;
                W->sdone = true;
                continue;
            }
            id = W->req;
        }
        Img t;
        bool failed = false;
        try { decode_setart(id, t); }
        catch (...) { failed = true; t = Img(); }                /* WARMFIX1 F3: a failed result, not a terminate */
        std::lock_guard<std::mutex> lk(W->m);
        W->img = std::move(t);
        W->req = -1;
        W->done = id;
        (void)failed;
    }
}

/* the picture's scaled piece (kind 1, as scaled_im builds it), made by the worker: 1 when the piece exists (or cannot be made off-thread: the
   caller then scales inline), 0 while the worker is busy with it */
int wdec_ready(int id);
int wscale_ready(int id, int dw, int dh)
{
    if (dw < 1 || dh < 1) return 1;
    if (find_piece(1, id, dw, dh)) return 1;
    if (!wdec_ready(id)) return 0;                               /* the picture itself first */
    if (g_im[id].px.empty() || !g_wd) return 1;
    WDec *W = g_wd;
    std::unique_lock<std::mutex> lk(W->m);
    if (!W->started) return 1;                                   /* no worker thread: scale inline */
    if (W->sdone) {
        if (W->sid == id && W->sdw == dw && W->sdh == dh && !W->spx.empty()) {
            lk.unlock();
            Piece *p = new_piece(1, id, dw, dh, dw, dh);
            lk.lock();
            p->px = std::move(W->spx);
            W->sdone = false; W->spx.clear();
            return 1;
        }
        W->sdone = false; W->spx.clear();                           /* a stale or failed result: ask again, or fall back inline */
        if (W->sid == id && W->sdw == dw && W->sdh == dh) return 1;
    }
    if (!W->sreq && W->req < 0 && W->done < 0) {
        W->sid = id; W->sdw = dw; W->sdh = dh; W->sreq = true;
        W->cv.notify_one();
    }
    return 0;
}

/* 1 when picture id is loaded (decoded here or by the worker); otherwise asks the worker and answers 0 */
int wdec_ready(int id)
{
    if (!g_wd) {
        if (g_im[id].tried) return 1;
        try { g_wd = new WDec(); } catch (...) { return 1; }       /* WARMFIX1 F3: no memory: the page loads it on first use */
    }
    WDec *W = g_wd;
    std::lock_guard<std::mutex> lk(W->m);
    /* WARMFIX1 F2: a finished picture the page decoded itself in the meantime is stale; drop it first, or `done` stays set for the rest of
       the run and no later picture is ever requested */
    if (W->done >= 0 && g_im[W->done].tried) {
        static const bool tr = getenv("SM64DS_WARM_TRACE") != 0;
        if (tr) fprintf(stderr, "[warm] settings: stale decode of picture %d dropped (the page loaded it first), asking for picture %d\n", W->done, id);
        W->done = -1; W->img = Img();
    }
    if (g_im[id].tried) return 1;
    if (W->done == id) {
        std::unique_lock<std::mutex> pl(g_pc_mx, std::try_to_lock);      /* WARMFIX1 F1: the swap; never wait for it */
        if (!pl.owns_lock()) return 0;
        if (g_im[id].tried) { W->done = -1; W->img = Img(); return 1; }
        g_im[id].tried = 1; g_im[id].w = W->img.w; g_im[id].h = W->img.h; g_im[id].px = std::move(W->img.px);
        W->done = -1;
        return 1;
    }
    if (W->req < 0 && W->done < 0) {
        if (!W->started) {
            try { std::thread(wdec_main, W).detach(); }
            catch (...) { return 1; }                              /* WARMFIX1 F3: no thread: load on first use, no flag left set */
            W->started = true;
        }
        W->req = id;
        W->cv.notify_one();
    }
    return 0;
}
}  // namespace

static double ws_ms(const std::chrono::steady_clock::time_point &t0)
{
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
}

/* WARMFIX1 F5: after a step that took over 6 ms the warm-up rests for four times that long (no slot is claimed meanwhile) */
static double ws_now_ms()
{
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
static double g_ws_hold_until;
/* the calling thread's own CPU time in ms (trace only: it tells a slow unit from a descheduled one on a busy box) */
extern "C" __declspec(dllimport) int __stdcall GetThreadTimes(void *, unsigned long long *, unsigned long long *, unsigned long long *, unsigned long long *);
extern "C" __declspec(dllimport) void *__stdcall GetCurrentThread(void);
static double ws_cpu_ms()
{
    unsigned long long c, e, k, u;
    if (!GetThreadTimes(GetCurrentThread(), &c, &e, &k, &u)) return 0;
    return (double)(k + u) / 10000.0;
}

/* 1 while the worker is still decoding the picture the next step waits for (the caller then does not take a warm-up slot) */
extern "C" int settings_page_warm_waiting(void)
{
    if (g_ws_hold_until > 0 && ws_now_ms() < g_ws_hold_until) return 1;
    if (!g_wd) return 0;
    std::lock_guard<std::mutex> lk(g_wd->m);
    return g_wd->req >= 0 || g_wd->sreq;
}

extern "C" int settings_page_warm_step(int w, int h)
{
    if (w < 64 || h < 64 || w > 8192 || h > 8192) return 1;
    if (S.open) return 0;                              /* the LIVE page is open (S is the live state here) */
    /* WARMFIX1 F1: everything below runs on the scratch S and L, and the pieces it makes stay private until the step ends */
    struct Scope { Scope() { tl_warm = true; } ~Scope() { publish_pieces(); tl_warm = false; } } scope;
    S.emb_on = 1; S.emb_wide = (long long)w * 192 / h >= 300; S.emb_dx = 0; S.emb_input = 1;      /* PAUSE3: warm the layout the pause will use */
    if (w != g_ws_w || h != g_ws_h) {
        g_ws_w = w; g_ws_h = h; g_ws_stage = 0; g_ws_y = 0; g_ws_tab = 0; g_ws_row = -1; g_ws_cpn = 1;
        std::vector<uint32_t>().swap(g_ws_tile); std::vector<uint32_t>().swap(g_ws_buf);
    }
    /* stage order: layout; each picture decoded alone; the parchment tile scaled; the sheet in row slices (built in a hidden cache entry, kind 102,
       renamed when whole); title and back scaled; one throwaway draw per tab */
    enum { ST_LAYOUT = 0, ST_DEC, ST_TILE = ST_DEC + IM_COUNT, ST_SHEET, ST_TITLE, ST_BACK, ST_PLANK, ST_ARROW, ST_BUF, ST_TAB, ST_DONE };
    if (g_ws_stage >= ST_DONE) return 1;
    if (g_ws_hold_until > 0 && ws_now_ms() < g_ws_hold_until) return 0;
    const auto t0 = std::chrono::steady_clock::now();
    while (g_ws_stage < ST_DONE) {
        const auto ts = std::chrono::steady_clock::now();
        const double cpu0 = ws_cpu_ms();
        const int st = g_ws_stage;
        if (st == ST_LAYOUT) {
            compute_layout(w, h);
            g_ws_u100 = (int)(L.u * 100 + 0.5);
            g_ws_tilesz = (int)(380 * (g_ws_u100 / 100.0) + 0.5);
            ++g_ws_stage;
        } else if (st < ST_TILE) {
            if (!wdec_ready(st - ST_DEC)) return 0;       /* the worker is decoding it: come back next call */
            ++g_ws_stage;
        } else if (st == ST_TILE) {
            const bool have_sheet = find_piece(2, L.sw, L.sh, g_ws_u100) != 0;
            if (!have_sheet && !wscale_ready(IM_PARCH, g_ws_tilesz, g_ws_tilesz)) return 0;
            const Piece *tp = have_sheet ? 0 : scaled_im(IM_PARCH, g_ws_tilesz, g_ws_tilesz);
            g_ws_tile_ok = tp != 0;
            if (tp) g_ws_tile = tp->px;
            g_ws_y = 0;
            ++g_ws_stage;
        } else if (st == ST_SHEET) {
            Piece *sp = find_piece(102, L.sw, L.sh, g_ws_u100);
            if (find_piece(2, L.sw, L.sh, g_ws_u100)) { ++g_ws_stage; }       /* the page already built it */
            else {
                if (!sp) { sp = new_piece(102, L.sw, L.sh, g_ws_u100, L.sw, L.sh); g_ws_y = 0; }
                /* the same per-pixel work as sheet_piece, in row slices */
                const double u = g_ws_u100 / 100.0;
                const int sw = L.sw, sh = L.sh, tile = g_ws_tilesz;
                const bool have_tile = g_ws_tile_ok && !g_ws_tile.empty();
                const float rad = (float)(26 * u), rim = (float)(6 * u);
                while (g_ws_y < sh && ws_ms(ts) < 1.2) {
                    const int y1 = g_ws_y + 8 < sh ? g_ws_y + 8 : sh;
                    for (int y = g_ws_y; y < y1; ++y)
                        for (int x = 0; x < sw; ++x) {
                            const float sd = rr_sd(x + 0.5f, y + 0.5f, (float)sw, (float)sh, rad);
                            const int cv = cov_of(sd);
                            if (!cv) continue;
                            int r, g, b;
                            if (sd > -rim) { r = 0x5C; g = 0x2C; b = 0x0A; }
                            else if (have_tile) {
                                const uint32_t c = g_ws_tile[(size_t)(y % tile) * tile + (x % tile)];
                                r = (c >> 16) & 255; g = (c >> 8) & 255; b = c & 255;
                            } else { r = 0xE8; g = 0xCC; b = 0x7A; }
                            if (sd > -rim - 1.0f && sd <= -rim && have_tile) {
                                const float f = -rim - sd;
                                const uint32_t c = g_ws_tile[(size_t)(y % tile) * tile + (x % tile)];
                                const int tr = (c >> 16) & 255, tg = (c >> 8) & 255, tb = c & 255;
                                r = (int)(0x5C + (tr - 0x5C) * f); g = (int)(0x2C + (tg - 0x2C) * f); b = (int)(0x0A + (tb - 0x0A) * f);
                            }
                            sp->px[(size_t)y * sw + x] = ((uint32_t)cv << 24) | ((uint32_t)(r * cv / 255) << 16) | ((uint32_t)(g * cv / 255) << 8) | (uint32_t)(b * cv / 255);
                        }
                    g_ws_y = y1;
                }
                if (g_ws_y >= sh) {
                    { std::lock_guard<std::mutex> pl(g_pc_mx); sp->kind = 2; sp->building = true; }      /* published with the step's other pieces */
                    std::vector<uint32_t>().swap(g_ws_tile);
                    ++g_ws_stage;
                }
            }
        } else if (st == ST_TITLE) {
            if (!wscale_ready(IM_TITLE, L.title[2], L.title[3])) return 0;
            if (!scaled_im(IM_TITLE, L.title[2], L.title[3])) plain_im_piece(IM_TITLE, L.title[2], L.title[3]);
            ++g_ws_stage;
        } else if (st == ST_BACK) {
            if (!wscale_ready(IM_BACK, L.back[2], L.back[3])) return 0;
            if (!scaled_im(IM_BACK, L.back[2], L.back[3])) plain_im_piece(IM_BACK, L.back[2], L.back[3]);
            ++g_ws_stage;
        } else if (st == ST_PLANK) {
            slice3_piece(4, IM_PLANK, L.reset[2], L.reset[3], 0.12);
            ++g_ws_stage;
        } else if (st == ST_ARROW) {
            arrow_right_piece(P(24), P(34));
            ++g_ws_stage;
        } else if (st == ST_BUF) {
            g_ws_buf.assign((size_t)w * h, 0u);
            ++g_ws_stage;
        } else {
            /* a throwaway draw of one tab, in pieces: the chrome (tabs, title, footer), then one row at a time: their glyph, key cap, box and
               plank pieces. The tab and cursor are put back after. */
            if (g_ws_buf.empty()) g_ws_buf.assign((size_t)w * h, 0u);
            int nrows = 0;
            rows_of(g_ws_tab, &nrows);
            g_warming = 1;
            S.open = 1; S.tab = g_ws_tab;                  /* the scratch state: the live page never shows open */
            if (g_ws_row < 0) { g_ws_r0 = g_ws_r1 = 0; g_ws_cp = g_ws_cpn; }
            else { g_ws_r0 = g_ws_row; g_ws_r1 = g_ws_row + 1; g_ws_cp = 0; }
            settings_page_draw(g_ws_buf.data(), w, w, h);
            S.open = 0;
            g_warming = 0;
            if (g_ws_row < 0 && g_ws_cpn < 3) ++g_ws_cpn;                /* the next part of the chrome comes next */
            else { g_ws_cpn = 1; g_ws_row = g_ws_row < 0 ? 0 : g_ws_row + 1; }
            if (g_ws_row >= nrows) { g_ws_row = -1; ++g_ws_tab; }
            if (g_ws_tab >= T_COUNT) { g_ws_stage = ST_DONE; std::vector<uint32_t>().swap(g_ws_buf); }
        }
#ifndef PORT_ROM_CLEAN
        static int trace = -1;
        if (trace < 0) trace = getenv("SM64DS_WARM_TRACE") != 0;
        if (trace) fprintf(stderr, "[warm] settings stage %d %dx%d: %.2f ms (thread cpu %.2f)\n", st, w, h, ws_ms(ts), ws_cpu_ms() - cpu0);
#endif
        { const double cost = ws_ms(ts); if (cost > 6.0) { g_ws_hold_until = ws_now_ms() + 4.0 * cost; break; } }
        if (ws_ms(t0) >= 0.8) break;      /* a stage that took real time ends the call; cheap ones chain */
    }
    return g_ws_stage >= ST_DONE;
}

/* SETPAGEINTERP1: THE PAGE'S PICTURES BETWEEN TWO PAGE FRAMES. While the page is open the game stands still (it holds the debug menu's
   pause), so the host's Smooth Motion makes no picture between ticks and the pacer shows each frame's picture again; the strip's slide then
   stepped once a frame. The picture under the page is the same for the whole frame, so a frame whose page moves keeps it (spi_keep_now, at
   the frame's own draw) and its own drawn page (spi_keep_page); settings_page_picture(alpha) puts the kept picture back, draws the page at
   that place between the two frames and lays back what the host drew over the page after it (the toast, the pad pointer: the pixels where the
   frame's finished picture is not the page's own). Only while something moves, only with Smooth Motion on, never for the warm-up's draws. */
static std::vector<uint32_t> g_spi_under, g_spi_page, g_spi_fin;
static uint32_t *g_spi_px;
static int g_spi_stride, g_spi_w, g_spi_h, g_spi_pf = -1, g_spi_fin_ok, g_spi_in;
static void spi_copy_out(std::vector<uint32_t> &v, const uint32_t *px)
{
    v.resize((size_t)g_spi_w * g_spi_h);
    for (int y = 0; y < g_spi_h; ++y) memcpy(&v[(size_t)y * g_spi_w], px + (size_t)y * g_spi_stride, (size_t)g_spi_w * 4);
}
static int spi_keep_now(uint32_t *px, int stride, int w, int h)
{
    if (tl_warm || g_warming || g_spi_in) return 0;
    g_spi_pf = -1;
    if (S.slide_t <= 0 || !host_setting_smooth_motion()) return 0;
    g_spi_px = px; g_spi_stride = stride; g_spi_w = w; g_spi_h = h;
    spi_copy_out(g_spi_under, px);
    return 1;
}
static void spi_keep_page()
{
    spi_copy_out(g_spi_page, g_spi_px);
    g_spi_pf = S.pf;
    g_spi_fin_ok = 0;
}
extern "C" int settings_page_picture(double alpha)
{
    if (!S.open || g_spi_pf != S.pf || S.slide_t <= 0 || !g_spi_px) return 0;
    uint32_t *px = g_spi_px;
    const int w = g_spi_w, h = g_spi_h, st = g_spi_stride;
    if (!g_spi_fin_ok) { spi_copy_out(g_spi_fin, px); g_spi_fin_ok = 1; }
    for (int y = 0; y < h; ++y) memcpy(px + (size_t)y * st, &g_spi_under[(size_t)y * w], (size_t)w * 4);
    g_spi_in = 1;
    settings_page_set_picture_alpha(alpha);
    settings_page_draw(px, st, w, h);
    g_spi_in = 0;
    for (int y = 0; y < h; ++y) {
        uint32_t *row = px + (size_t)y * st;
        const uint32_t *a = &g_spi_page[(size_t)y * w], *b = &g_spi_fin[(size_t)y * w];
        for (int x = 0; x < w; ++x)
            if (a[x] != b[x]) row[x] = b[x];
    }
    return 1;
}

extern "C" void settings_page_draw(uint32_t *px, int stride, int w, int h)
{
    if (!S.open || !px || w < 64 || h < 64) return;
    const int spi_keep = spi_keep_now(px, stride, w, h);    /* SETPAGEINTERP1: the picture under the page, for the pictures between frames */
    if (!tl_warm) free_retired();                     /* WARMFIX1 F1: this frame holds no piece yet */
    const auto t0 = std::chrono::steady_clock::now();
    S.w = w; S.h = h;
    compute_layout(w, h);
    Surf s = { px, stride, w, h };
    int n = 0;
    const Row *rows = rows_of(S.tab, &n);

    const int cp = g_warming ? g_ws_cp : 0;
    const bool ro = g_warming && g_ws_r1 > g_ws_r0;    /* WARMFIX1 F5: a warm step for one row draws only that row (the chrome was cached by the tab's chrome step) */
    if (!ro) {
    if (cp == 0 || cp == 1) {   /* the dimmed picture, then the sheet (MENUCOST1: no dim under the opaque sheet) */
        const Piece *sp = sheet_piece(L.sw, L.sh, (int)(L.u * 100 + 0.5));
        if (!S.emb_on) dim_under(s, sp, L.sx, L.sy);      /* PAUSE3: the pause is already dimmed by the ROM */
        if (sp) blit_piece(s, sp, L.sx, L.sy);
    }
    if (cp == 0 || cp == 1) draw_im(s, IM_TITLE, L.title[0], L.title[1], L.title[2], L.title[3], 255, 0, 0, 0);
    if (cp == 0 || cp == 1) {
        const int hov = S.mouse_known && in_rect(S.mx_prev, S.my_prev, L.back);
        draw_im(s, IM_BACK, L.back[0], L.back[1] + (S.pressed_plank == 1 ? P(3) : 0), L.back[2], L.back[3], 255, hov ? 30 : 0, S.pressed_plank == 1 ? 25 : 0, 0);
    }
    if (cp == 0 || cp == 2) {   /* tabs */
        const float em = (float)P(27);
        const int by = L.tab_y + L.tab_h - P(10);
        /* CHEATS1: the window of TAB_VIS names; while it slides every name is drawn at its place along the strip that far along, clipped to
           the strip's ends, so the leaving name slides out at one end and the arriving one in at the other */
        float lend = (float)L.tab_x[L.tab_win], rbeg = (float)(L.tab_x[L.tab_win + TAB_VIS - 1] + L.tab_w[L.tab_win + TAB_VIS - 1]);
        if (S.slide_t > 0) {
            const double f = strip_win_now();
            const int a = (int)f, b = a + 1 < T_COUNT - TAB_VIS + 1 ? a + 1 : a;
            const float fr = (float)(f - a);
            const float off = L.tab_pos[a] + (L.tab_pos[b] - L.tab_pos[a]) * fr;
            const float wa = L.tab_pos[a + TAB_VIS] - L.tab_gap - L.tab_pos[a], wb = L.tab_pos[b + TAB_VIS] - L.tab_gap - L.tab_pos[b];
            const float tot = wa + (wb - wa) * fr;
            lend = L.sx + (L.sw - tot) * 0.5f; rbeg = lend + tot;
            const int clip[4] = { (int)floorf(lend), L.tab_y - P(8), (int)ceilf(rbeg), L.tab_y + L.tab_h + P(8) };
            for (int i = 0; i < T_COUNT; ++i) {
                const float x = lend + L.tab_pos[i] - off;
                const float w = L.tab_pos[i + 1] - L.tab_gap - L.tab_pos[i];
                if (x >= rbeg || x + w <= lend) continue;
                const int on = i == S.tab;
                ui_text_draw(&s, x, (float)by, TAB_NAMES[i], em, on ? C_TEXT : C_QUIET, 255, clip);
                if (on) fill_rrect(s, x, (float)(by + P(6)), w, (float)P(5), (float)P(2), C_TEXT, 255, clip);
            }
        } else
        for (int i = 0; i < T_COUNT; ++i) {
            if (L.tab_w[i] <= 0) continue;
            const int on = i == S.tab;
            ui_text_draw(&s, (float)L.tab_x[i], (float)by, TAB_NAMES[i], em, on ? C_TEXT : C_QUIET, 255, 0);
            if (on) fill_rrect(s, (float)L.tab_x[i], (float)(by + P(6)), (float)L.tab_w[i], (float)P(5), (float)P(2), C_TEXT, 255, 0);
        }
        /* INPUTMATRIX1 (Thunder: "the LB and RB positions are a bit low"): the hints used to sit on top of the first and last tab names, at a height
           of their own. They now stand level with the tab names (the same cap-height centre) in the margin beside the first and last tab. */
        const float sm = (float)P(22);
        const float cy = (float)by - ui_text_cap_height(em) * 0.5f;
        const float lw = ui_text_width("LB", sm), rw = ui_text_width("RB", sm);
        text_mid(s, (float)L.sx + (lend - L.sx - lw) * 0.5f, cy, "LB", sm, C_QUIET, 200, 0);
        text_mid(s, rbeg + ((float)(L.sx + L.sw) - rbeg - rw) * 0.5f, cy, "RB", sm, C_QUIET, 200, 0);
    }
    }
    const int *clip = L.body;
    for (int i = 0; i < n && i < MAXR; ++i) {
        if (g_warming && (i < g_ws_r0 || i >= g_ws_r1)) continue;
        const Row &R = rows[i];
        const int y = L.r[i].y, rh = L.r[i].h;
        if (y + rh < L.body[1] || y > L.body[3]) continue;
        const int cy = y + rh / 2;
        const int app = (R.t == R_BIND || R.t == R_HEAD || R.t == R_NOTE) ? 1 : sl_applicable(R.id) != 0;
        const int alpha = app ? 255 : 110;
        const int sel = i == S.sel && S.cursor_shown;
        if (R.t == R_HEAD) {
            text_mid(s, (float)L.lab_x, (float)(cy + P(6)), R.label, (float)P(26), C_QUIET, 255, clip);
            continue;
        }
        if (R.t == R_NOTE) {
            text_mid(s, (float)L.lab_x, (float)cy, R.label, (float)P(23), C_QUIET, 255, clip);
            continue;
        }
        if (R.t != R_CHECK) text_mid(s, (float)L.lab_x, (float)cy, R.label, (float)P(30), app ? C_TEXT : C_QUIET, alpha, clip);
        char buf[32];
        switch (R.t) {
        case R_CHOICE: {
            const SlInfo *I = sl_info(R.id);
            const int v = sl_get(R.id);
            const int ci = ch_idx(R, v);
            char vb[16];
            const char *t = R.names[ci];
            if (R.vals && R.vals[ci] != v) { snprintf(vb, sizeof vb, "%d", v); t = vb; }      /* a value that is not one of the stops (the launcher's own number) */
            (void)I;
            draw_dd_box(s, L.ctl_x, cy - P(23), L.ctl_w, P(46), t, sel, alpha, clip);
            break;
        }
        case R_SLIDER: {
            const SlInfo *I = sl_info(R.id);
            int rx, rw;
            slider_geometry(&rx, &rw);
            draw_slider(s, L.ctl_x, cy, L.rail_w, sl_get(R.id), I->min, I->max, alpha, clip);
            snprintf(buf, sizeof buf, "%d%%", sl_get(R.id));
            const float vem = (float)P(28);
            const float vw = ui_text_width(buf, vem);
            text_mid(s, (float)(L.ctl_x + L.rail_w + P(86)) - vw, (float)cy, buf, vem, app ? C_TEXT : C_QUIET, alpha, clip);
            break;
        }
        case R_CHECK: {
            /* a check box reads box first, then its words, as the launcher's do */
            const int sz = P(38);
            draw_check(s, L.lab_x, cy - sz / 2, sz, sl_get(R.id) != 0, sel, alpha, clip);
            text_mid(s, (float)(L.lab_x + sz + P(18)), (float)cy, R.label, (float)P(30), app ? C_TEXT : C_QUIET, alpha, clip);
            break;
        }
        case R_BIND: {
            CapPos cp[3];
            caps_of(R, cp);
            const int kh = cap_h(), ky = cy - kh / 2;
            for (int c = 0; c < bind_cols(R); ++c) {
                const int id = bind_id(R, c);
                const int waiting = S.cap_active && S.cap_id == id;
                char nb[24];
                draw_cap(s, cp[c].x, ky, cp[c].w, kh, id_name(id, nb, sizeof nb), sel && S.col == c, waiting, clip);
            }
            break;
        }
        default: break;
        }
    }
    if (!ro && (cp == 0 || cp == 3)) {
    if (L.scroll_max > 0) {   /* the scroll bar */
        fill_rrect(s, (float)L.bar[0], (float)L.bar[1], (float)L.bar[2], (float)L.bar[3], L.bar[2] * 0.5f, 0x000000, 34, 0);
        int th = (int)((double)L.bar[3] * L.bar[3] / L.content_h);
        if (th < L.bar[2]) th = L.bar[2];
        const int ty = L.bar[1] + (int)((double)(L.bar[3] - th) * S.scroll / L.scroll_max);
        fill_rrect(s, (float)L.bar[0], (float)ty, (float)L.bar[2], (float)th, L.bar[2] * 0.5f, C_TEXT, 255, 0);
    }
    /* the selection arrow: the ROM's own title arrow, turned to point at the row */
    if (S.cursor_shown && S.sel < n && rows[S.sel].t != R_HEAD && rows[S.sel].t != R_NOTE) {
        const int aw = P(24), ah = P(34);
        const Piece *ap = arrow_right_piece(aw, ah);
        const int ay = L.r[S.sel].y + L.rowh / 2 - ah / 2;
        if (ap) blit(s, ap->px, ap->w, ap->h, L.sx + P(18) + ((S.pf / 12) % 2 ? P(2) : 0), ay, 255, 0, 0, L.body);
    }
    {   /* the footer: the note on the left, the Reset plank on the right */
        const int camrow = S.cursor_shown && S.sel < n && rows[S.sel].t == R_BIND &&
                           (!strncmp(rows[S.sel].label, "Rotate camera", 13) || !strcmp(rows[S.sel].label, "Center camera"));
        const char *msg = S.note[0] ? S.note
                          : (S.cap_active ? (S.cap_id < SL_PAD_BASE ? "Press a key. Esc keeps the old one." : S.cap_id >= SL_MOUSE_BASE ? "Press a mouse button. Left click keeps the old one." : "Press a button. BACK keeps the old one.")
                             : (camrow ? "In the pause, LB / RB and Q / E always slide the screens." : ""));
        const int fy = L.reset[1] + L.reset[3] / 2;
        if (msg[0]) {
            float em = (float)P(24);
            const float room = (float)(L.reset[0] - P(16) - L.lab_x);   /* the note stops short of the Reset plank: a long one shrinks to fit */
            const float need = ui_text_width(msg, em);
            if (room > 0 && need > room) em *= room / need;
            ui_text_draw(&s, (float)L.lab_x, fy + ui_text_cap_height(em) * 0.5f, msg, em, S.note[0] ? 0x9A2A10 : C_QUIET, 255, 0);
        } else {
            const float em = (float)P(22);
            const char *hint = (S.cursor_shown && S.sel < n && rows[S.sel].t == R_BIND) ? "A: change   Y or Delete: clear" : "Changes apply at once and are saved.";
            ui_text_draw(&s, (float)L.lab_x, fy + ui_text_cap_height(em) * 0.5f, hint, em, C_QUIET, 255, 0);
        }
        const int sel = S.sel == n && S.cursor_shown;
        const int hov = S.mouse_known && in_rect(S.mx_prev, S.my_prev, L.reset);
        const Piece *pl = slice3_piece(4, IM_PLANK, L.reset[2], L.reset[3], 0.12);
        const int dy = S.pressed_plank == 2 ? P(2) : 0;
        if (pl) blit(s, pl->px, pl->w, pl->h, L.reset[0], L.reset[1] + dy, 255, (sel || hov) ? 28 : 0, S.pressed_plank == 2 ? 25 : 0, 0);
        const float em = L.reset[3] * 0.50f;
        text_c(s, L.reset[0] + L.reset[2] * 0.5f, L.reset[1] + dy + L.reset[3] * 0.5f + ui_text_cap_height(em) * 0.5f, "RESET", em, C_LETTER, 255, 0);
        if (sel) {
            const int aw = P(24), ah = P(34);
            const Piece *ap = arrow_right_piece(aw, ah);
            if (ap) blit(s, ap->px, ap->w, ap->h, L.reset[0] - aw - P(8), L.reset[1] + L.reset[3] / 2 - ah / 2, 255, 0, 0, 0);
        }
    }
    if (S.dd_open && S.sel < n && rows[S.sel].t == R_CHOICE) {   /* the open drop-down list, over the rows */
        const Row &R = rows[S.sel];
        const DdGeom G = dd_geom(S.sel, R.nn);
        const int lx = G.lx, ly = G.ly, lw = G.lw, lh = G.lh;
        const Piece *p = purple_piece(lw, G.lhh);
        if (p) blit(s, p->px, p->w, p->h, lx, ly, 255, 0, 0, 0);
        for (int k = 0; k < G.vis; ++k) {
            const int i = G.first + k;
            const int ry = ly + P(8) + k * lh;
            if (i == S.dd_hi) fill_rrect(s, (float)(lx + P(6)), (float)ry, (float)(lw - P(12)), (float)lh, (float)P(6), 0xFFFFFF, 80, 0);
            text_mid(s, (float)(lx + P(18)), ry + lh * 0.5f, R.names[i], lh * 0.62f, 0xFFFFFF, 255, 0);
        }
        if (G.vis < R.nn) {                               /* clamped list: a thin scroll thumb on its right edge */
            const int tx = lx + lw - P(13), ty = ly + P(10), th = G.lhh - P(20);
            fill_rrect(s, (float)tx, (float)ty, (float)P(4), (float)th, (float)P(2), 0xFFFFFF, 70, 0);
            const int kh = th * G.vis / R.nn, ky = ty + (th - kh) * G.first / (R.nn - G.vis);
            fill_rrect(s, (float)tx, (float)ky, (float)P(4), (float)kh, (float)P(2), 0xFFFFFF, 230, 0);
        }
    }
    }
    S.draw_ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
    if (spi_keep) spi_keep_page();                     /* SETPAGEINTERP1: and the page's own picture over it */
    if (!tl_warm) g_sp_pic_alpha = 1.0;                /* CHEATS1: a picture fraction is for one draw */
    if (!tl_warm) publish_pieces();                    /* WARMFIX1 F1 */
    {   /* SM64DS_SETPAGE_COST=1: the cost of the page's draw, a line every 120 frames (proof rows only) */
        static int on = -1, cnt;
#ifdef PORT_ROM_CLEAN
        on = 0;
#endif
        static double sum, mn = 1e9, mx;
        if (on < 0) on = getenv("SM64DS_SETPAGE_COST") != 0;
        if (on && !g_warming && !g_spi_in) {
            sum += S.draw_ms; ++cnt;
            if (S.draw_ms < mn) mn = S.draw_ms;
            if (S.draw_ms > mx) mx = S.draw_ms;
            if (cnt == 120) {
                fprintf(stderr, "[settings] page draw %dx%d: %d frames, mean %.2f ms, min %.2f, max %.2f\n", w, h, cnt, sum / cnt, mn, mx);
                cnt = 0; sum = 0; mn = 1e9; mx = 0;
            }
        }
    }
}

/* PAUSE3: the new pause's drawing (star tracker, dialogs, edge tabs, pointer): pause_page_draw / pause_page_art */
#include "pause_page.inc"

/* ============================================================ scripted input (proof rows) ============================== */

extern "C" int settings_page_env_open_at(void)
{
    static int read, at = -1;
#ifdef PORT_ROM_CLEAN
    return -1;        /* the proof seams are not in a shipping build */
#endif
    if (!read) {
        read = 1;
        const char *e = getenv("SM64DS_SETPAGE_AT");
        if (e && *e) at = atoi(e);
    }
    return at;
}

/* SM64DS_SETPAGE_SCRIPT = "f:cmd[:a[:b]];..." with f the page frame (1 = the first frame the page is open). Commands: up down left right ok back tabl tabr clear
   capcancel (held for that one frame), move:x:y (the pointer, stays), ldown / lup, rdown / rup (a button, stays), wheel:n (that frame). */
extern "C" int settings_page_scripted(SpInput *in, int pf)
{
    static int read, n;
    struct Step { int f; char cmd[16]; int a, b; };
    static Step st[256];
    static int mx = -1, my = -1, ml, mr;
#ifdef PORT_ROM_CLEAN
    (void)in; (void)pf;
    return 0;         /* the proof seams are not in a shipping build */
#endif
    if (!read) {
        read = 1;
        const char *e = getenv("SM64DS_SETPAGE_SCRIPT");
        while (e && *e && n < 256) {
            Step s;
            memset(&s, 0, sizeof s);
            char cmd[16] = {};
            int f = 0, got = 0;
            if (sscanf(e, "%d:%15[a-z]%n", &f, cmd, &got) >= 2) {
                e += got;
                int a = 0, b = 0;
                if (*e == ':') { ++e; a = (int)strtol(e, (char **)&e, 10); }
                if (*e == ':') { ++e; b = (int)strtol(e, (char **)&e, 10); }
                s.f = f; snprintf(s.cmd, sizeof s.cmd, "%s", cmd); s.a = a; s.b = b;
                st[n++] = s;
            }
            while (*e && *e != ';') ++e;
            if (*e == ';') ++e;
        }
    }
    if (!n) return 0;
    in->keys = 0; in->wheel = 0;
    for (int i = 0; i < n; ++i) {
        if (st[i].f != pf) continue;
        const char *c = st[i].cmd;
        if (!strcmp(c, "up")) in->keys |= SPK_UP;
        else if (!strcmp(c, "down")) in->keys |= SPK_DOWN;
        else if (!strcmp(c, "left")) in->keys |= SPK_LEFT;
        else if (!strcmp(c, "right")) in->keys |= SPK_RIGHT;
        else if (!strcmp(c, "ok")) in->keys |= SPK_ACCEPT;
        else if (!strcmp(c, "back")) in->keys |= SPK_BACK;
        else if (!strcmp(c, "tabl")) in->keys |= SPK_TABL;
        else if (!strcmp(c, "tabr")) in->keys |= SPK_TABR;
        else if (!strcmp(c, "clear")) in->keys |= SPK_CLEAR;
        else if (!strcmp(c, "capcancel")) in->keys |= SPK_CAP_CANCEL;
        else if (!strcmp(c, "move")) { mx = st[i].a; my = st[i].b; }
        else if (!strcmp(c, "ldown")) ml = 1;
        else if (!strcmp(c, "lup")) ml = 0;
        else if (!strcmp(c, "rdown")) mr = 1;
        else if (!strcmp(c, "rup")) mr = 0;
        else if (!strcmp(c, "wheel")) in->wheel += st[i].a;
        else if (!strcmp(c, "key")) in->inject_key = st[i].a;
    }
    in->mouse_valid = mx >= 0; in->mouse_x = mx; in->mouse_y = my; in->mouse_l = ml; in->mouse_r = mr;
    return 1;
}

/* run after093, lane SEAMART1 (one-process plan M3c): the page's HD UI art in place. hal/mods.cpp's relatch at the title changes which mods
   ship hd_ui/; the page's pictures (g_im), every piece made from them or from the mod font (g_pieces) and the warm-up's tile were made from
   the old list. port_settings_page_hd_ui_quiesce waits (at most 2 s) until the worker has no decode or scale in flight (only the game thread
   asks it for work, and it is inside the relatch). port_settings_page_hd_ui_reset frees every piece (the page is closed and the relatch is
   not inside a warm step, so no draw holds one), clears the pictures and their tried flags and sends the warm-up back to its first stage, so the next open
   decodes from the new list. Refused by the relatch while the page is open. Returns the number of pictures dropped. Game thread only. */
extern "C" int port_settings_page_hd_ui_quiesce(void)
{
    if (!g_wd) return 1;
    for (int i = 0; i < 2000; ++i) {
        {
            std::lock_guard<std::mutex> lk(g_wd->m);
            if (g_wd->req < 0 && !g_wd->sreq) return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}

extern "C" void settings_page_warm_rearm(void);   /* hal/settings_page_hooks.cpp */
extern "C" int port_settings_page_hd_ui_reset(void)
{
    if (g_wd) {
        std::lock_guard<std::mutex> lk(g_wd->m);
        g_wd->done = -1; g_wd->img = Img();
        g_wd->sdone = false; g_wd->spx.clear();
    }
    int n = 0;
    {
        std::lock_guard<std::mutex> lk(g_pc_mx);
        g_pieces.clear();                              /* the page is closed and no warm step is running: nothing holds a piece */
        g_retired.clear();
        for (int id = 0; id < IM_COUNT; ++id) {
            if (g_im[id].tried && !g_im[id].px.empty()) ++n;
            g_im[id] = Img();
        }
    }
    g_ws_w = g_ws_h = 0; g_ws_stage = 0; g_ws_y = 0; g_ws_tab = 0; g_ws_row = -1; g_ws_cpn = 1; g_ws_tile_ok = 0;
    std::vector<uint32_t>().swap(g_ws_tile); std::vector<uint32_t>().swap(g_ws_buf);
    settings_page_warm_rearm();                    /* the idle warm-up runs again, so the first open after a switch is as quick as fresh */
    return n;
}
