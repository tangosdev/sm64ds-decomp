/* hal/pad_keyboard_draw.cpp: the letter grid plank, drawn. See pad_keyboard.h.

   The lobby page's family: with the HD UI Textures mod the sheet is the settings page's parchment in a brown rim, the keys its wooden plank and the text
   line its cream key plaque (menu/settings/*.png, the files the lobby page reads); without it the lobby page's plain skin (orange boxes, dark brown
   edges). The highlight is a warm tint with a dark outer and a bright inner ring (dark ink on the plain skin); CAPS on draws its key pressed in (darker). */
#include "pad_keyboard.h"
#include "mods.h"
#include "ui_text.h"
#include "../third_party/stb/stb_image.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

namespace {

struct Img { int w = 0, h = 0; std::vector<uint32_t> px; bool ok() const { return w > 0 && h > 0 && !px.empty(); } };   /* premultiplied ARGB */

void load_png(const char *path, Img *out)
{
    *out = Img();
    FILE *f = fopen(path, "rb");
    if (!f) return;
    fseek(f, 0, SEEK_END);
    const long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n <= 0 || n > (16L << 20)) { fclose(f); return; }
    std::vector<unsigned char> buf((size_t)n);
    const size_t got = fread(&buf[0], 1, (size_t)n, f);
    fclose(f);
    if (got != (size_t)n) return;
    int w = 0, h = 0, comp = 0;
    unsigned char *p = stbi_load_from_memory(&buf[0], (int)n, &w, &h, &comp, 4);
    if (!p) return;
    if (w > 0 && h > 0 && w <= 4096 && h <= 4096) {
        out->w = w; out->h = h;
        out->px.resize((size_t)w * h);
        for (int i = 0; i < w * h; ++i) {
            const unsigned r = p[i * 4], g = p[i * 4 + 1], b = p[i * 4 + 2], a = p[i * 4 + 3];
            out->px[i] = (a << 24) | (((r * a + 127) / 255) << 16) | (((g * a + 127) / 255) << 8) | ((b * a + 127) / 255);
        }
    }
    stbi_image_free(p);
}

enum { A_PLANK, A_KEY, A_PARCH, A_COUNT };
const char *const kArt[A_COUNT] = { "menu/settings/plank.png", "menu/settings/key.png", "menu/settings/parchment.png" };
struct Skin { int hd; Img art[A_COUNT]; };
const Skin &skin(void)
{
    static Skin s = [] {
        Skin k;
        k.hd = 1;
        for (int i = 0; i < A_COUNT; ++i) {
            char path[1024];
            if (!port_mods_ui_file(kArt[i], path, sizeof path)) { k.hd = 0; break; }
            load_png(path, &k.art[i]);
            if (!k.art[i].ok()) { k.hd = 0; break; }
        }
        fprintf(stderr, "[padkb] skin: %s\n", k.hd ? "HD UI Textures (parchment, planks)" : "plain");
        return k;
    }();
    return s;
}

struct Surf { uint32_t *px; int stride, w, h; };

inline void blend(const Surf &s, int x, int y, uint32_t pm, int a)
{
    if (x < 0 || y < 0 || x >= s.w || y >= s.h) return;
    int sa = (int)(pm >> 24), sr = (int)((pm >> 16) & 255), sg = (int)((pm >> 8) & 255), sb = (int)(pm & 255);
    if (a < 255) { sa = sa * a / 255; sr = sr * a / 255; sg = sg * a / 255; sb = sb * a / 255; }
    if (!sa) return;
    uint32_t &d = s.px[(size_t)y * s.stride + x];
    const int ia = 255 - sa;
    const int r = sr + (((d >> 16) & 255) * ia + 127) / 255, g = sg + (((d >> 8) & 255) * ia + 127) / 255, b = sb + ((d & 255) * ia + 127) / 255;
    const int na = sa + ((int)(d >> 24) * ia + 127) / 255;      /* an opaque page stays opaque; the cached layer gathers its coverage */
    d = ((uint32_t)(na > 255 ? 255 : na) << 24) | (uint32_t)((r > 255 ? 255 : r) << 16) | (uint32_t)((g > 255 ? 255 : g) << 8) | (uint32_t)(b > 255 ? 255 : b);
}
inline uint32_t pm_of(uint32_t rgb, int a) { return ((uint32_t)a << 24) | ((((rgb >> 16) & 255) * a / 255) << 16) | ((((rgb >> 8) & 255) * a / 255) << 8) | ((rgb & 255) * a / 255); }

/* a rounded rectangle, solid or from a picture of exactly its size */
void rrect(const Surf &s, int x, int y, int w, int h, float r, uint32_t rgb, int alpha, const Img *pic = 0)
{
    if (w < 1 || h < 1) return;
    if (r > w * 0.5f) r = w * 0.5f;
    if (r > h * 0.5f) r = h * 0.5f;
    for (int j = 0; j < h; ++j)
        for (int i = 0; i < w; ++i) {
            const float px = i + 0.5f, py = j + 0.5f;
            float dx = 0, dy = 0;
            if (px < r) dx = r - px; else if (px > w - r) dx = px - (w - r);
            if (py < r) dy = r - py; else if (py > h - r) dy = py - (h - r);
            float cov = 1.f;
            if (dx > 0 && dy > 0) { cov = r - sqrtf(dx * dx + dy * dy) + 0.5f; if (cov <= 0) continue; if (cov > 1) cov = 1; }
            const int ca = (int)(cov * alpha + 0.5f);
            if (ca <= 0) continue;
            if (pic) blend(s, x + i, y + j, pic->px[(size_t)j * pic->w + i], ca);
            else blend(s, x + i, y + j, 0xFF000000u | rgb, ca);
        }
}
void framed(const Surf &s, int x, int y, int w, int h, float r, int e, uint32_t fill, uint32_t edge)
{
    rrect(s, x, y, w, h, r, edge, 255);
    rrect(s, x + e, y + e, w - 2 * e, h - 2 * e, r > e ? r - e : 0, fill, 255);
}

/* bilinear, box-filtered when shrinking: a part of a picture to dw x dh */
void scale_rect(const Img &I, double sx, double sy, double sw, double sh, int dw, int dh, uint32_t *out, int ostride)
{
    if (!I.ok() || dw < 1 || dh < 1 || sw <= 0 || sh <= 0) return;
    const double kx = sw / dw, ky = sh / dh;
    int nx = (int)ceil(kx), ny = (int)ceil(ky);
    nx = nx < 1 ? 1 : nx > 6 ? 6 : nx;
    ny = ny < 1 ? 1 : ny > 6 ? 6 : ny;
    for (int y = 0; y < dh; ++y)
        for (int x = 0; x < dw; ++x) {
            float a = 0, r = 0, g = 0, b = 0;
            for (int j = 0; j < ny; ++j)
                for (int i = 0; i < nx; ++i) {
                    double fx = sx + (x + (i + 0.5) / nx) * kx - 0.5, fy = sy + (y + (j + 0.5) / ny) * ky - 0.5;
                    fx = fx < 0 ? 0 : fx > I.w - 1 ? I.w - 1 : fx;
                    fy = fy < 0 ? 0 : fy > I.h - 1 ? I.h - 1 : fy;
                    const int x0 = (int)fx, y0 = (int)fy, x1 = x0 + 1 < I.w ? x0 + 1 : x0, y1 = y0 + 1 < I.h ? y0 + 1 : y0;
                    const float tx = (float)(fx - x0), ty = (float)(fy - y0);
                    const uint32_t c[4] = { I.px[(size_t)y0 * I.w + x0], I.px[(size_t)y0 * I.w + x1], I.px[(size_t)y1 * I.w + x0], I.px[(size_t)y1 * I.w + x1] };
                    const float wt[4] = { (1 - tx) * (1 - ty), tx * (1 - ty), (1 - tx) * ty, tx * ty };
                    for (int q = 0; q < 4; ++q) {
                        a += wt[q] * (c[q] >> 24); r += wt[q] * ((c[q] >> 16) & 255); g += wt[q] * ((c[q] >> 8) & 255); b += wt[q] * (c[q] & 255);
                    }
                }
            const float inv = 1.f / (nx * ny);
            int ia = (int)(a * inv + 0.5f), ir = (int)(r * inv + 0.5f), ig = (int)(g * inv + 0.5f), ib = (int)(b * inv + 0.5f);
            ia = ia > 255 ? 255 : ia; ir = ir > ia ? ia : ir; ig = ig > ia ? ia : ig; ib = ib > ia ? ia : ib;
            out[(size_t)y * ostride + x] = ((uint32_t)ia << 24) | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
        }
}

/* a nine-slice of a picture at dw x dh (corners il, it, ir, ib source pixels scaled by f), cached by size */
struct Piece { int art, w, h; Img img; };
std::vector<Piece> g_cache;
const Img &nine(int art, int dw, int dh, int il, int it, int ir, int ib, float f)
{
    for (const Piece &p : g_cache) if (p.art == art && p.w == dw && p.h == dh) return p.img;
    if (g_cache.size() > 64) g_cache.clear();
    g_cache.push_back(Piece());
    Piece &p = g_cache.back();
    p.art = art; p.w = dw; p.h = dh;
    p.img.w = dw; p.img.h = dh;
    p.img.px.assign((size_t)dw * dh, 0);
    const Img &I = skin().art[art];
    int cl = (int)(il * f + 0.5f), ct = (int)(it * f + 0.5f), cr = (int)(ir * f + 0.5f), cb = (int)(ib * f + 0.5f);
    if (cl + cr > dw) { cl = dw / 2; cr = dw - cl; }
    if (ct + cb > dh) { ct = dh / 2; cb = dh - ct; }
    const int xs[4] = { 0, cl, dw - cr, dw }, ys[4] = { 0, ct, dh - cb, dh };
    const double sxs[4] = { 0, (double)il, (double)(I.w - ir), (double)I.w }, sys_[4] = { 0, (double)it, (double)(I.h - ib), (double)I.h };
    for (int j = 0; j < 3; ++j)
        for (int i = 0; i < 3; ++i) {
            const int w = xs[i + 1] - xs[i], h = ys[j + 1] - ys[j];
            if (w >= 1 && h >= 1) scale_rect(I, sxs[i], sys_[j], sxs[i + 1] - sxs[i], sys_[j + 1] - sys_[j], w, h, &p.img.px[(size_t)ys[j] * dw + xs[i]], dw);
        }
    return p.img;
}
void blit(const Surf &s, int x, int y, const Img &I, int a = 255)
{
    for (int j = 0; j < I.h; ++j)
        for (int i = 0; i < I.w; ++i) blend(s, x + i, y + j, I.px[(size_t)j * I.w + i], a);
}

/* the sheet's parchment, cut to the sheet's shape (cached by size) */
const Img &parchment(int w, int h)
{
    static Img cut;
    if (cut.w == w && cut.h == h) return cut;
    const Img &I = skin().art[A_PARCH];
    cut.w = w; cut.h = h;
    cut.px.assign((size_t)w * h, 0);
    const double ar = (double)w / h;
    double cw = I.w, ch = I.w / ar;
    if (ch > I.h) { ch = I.h; cw = I.h * ar; }
    scale_rect(I, (I.w - cw) / 2, (I.h - ch) / 2, cw, ch, w, h, &cut.px[0], w);
    return cut;
}

const uint32_t C_BOX = 0xD86000u, C_BOXEDGE = 0x6A3208u, C_INK = 0x3A1C06u, C_DIMINK = 0x7A4A20u, C_WOODTXT = 0xFFF3CCu, C_HDKEYINK = 0x5A2A0Au;

void text(const Surf &s, float x, float top, const char *str, float em, uint32_t rgb, const int *clip = 0)
{
    UiSurface us = { s.px, s.stride, s.w, s.h };
    ui_text_draw(&us, x, top + ui_text_cap_height(em), str, em, rgb, 255, clip);
}

}  // namespace

namespace {
/* where draw_plank left the caret (layer pixels): the blink is drawn over the cached plank each frame, so it never rebuilds the plank */
struct CaretAt { int has; float x, top, em; int clip[4]; };

/* one key, unlit or lit, at its own box */
void draw_key(const Surf &s, float x0, float y0, float sc, const PkbView *v, int i, bool lit)
{
    const bool hd = skin().hd != 0;
    auto X = [&](float dx) { return (int)floorf(x0 + dx * sc + 0.5f); };
    auto Y = [&](float dy) { return (int)floorf(y0 + dy * sc + 0.5f); };
    float kx, ky, kw, kh;
    if (!pkb_key_box(v->kind, i, &kx, &ky, &kw, &kh)) return;
    const int rx = X(kx), ry = Y(ky), rw = X(kx + kw) - rx, rh = Y(ky + kh) - ry;
    char lb[4];
    const char *label = pkb_key_label(v->kind, v->ok, i, v->caps, lb, sizeof lb);
    const bool sunk = v->caps && v->kind == PKB_TEXT && !strcmp(label, "CAPS");
    if (hd) blit(s, rx, ry, nine(A_PLANK, rw, rh, 18, 18, 18, 18, (float)rh / 150.0f * 2.2f));
    else framed(s, rx, ry, rw, rh, 2.5f * sc, sc < 2 ? 1 : (int)(sc * 0.8f), C_BOX, C_BOXEDGE);
    if (sunk) rrect(s, rx, ry, rw, rh, 2.5f * sc, 0x000000u, 90);
    if (lit) {      /* a warm tint, a dark outer ring and a bright inner ring: readable at 1x on the light HD wood and on the plain orange */
        rrect(s, rx, ry, rw, rh, 2.5f * sc, 0xFFE24Au, hd ? 150 : 170);
        const int e = (int)(sc * 0.9f) > 1 ? (int)(sc * 0.9f) : 1;
        auto ring = [&](int in, int th, uint32_t argb, int a) {
            for (int q = 0; q < th; ++q) {
                const int qx = rx + in + q, qy = ry + in + q, qw = rw - 2 * (in + q), qh = rh - 2 * (in + q);
                if (qw < 2 || qh < 2) return;
                for (int xx = qx; xx < qx + qw; ++xx) { blend(s, xx, qy, argb, a); blend(s, xx, qy + qh - 1, argb, a); }
                for (int yy = qy + 1; yy < qy + qh - 1; ++yy) { blend(s, qx, yy, argb, a); blend(s, qx + qw - 1, yy, argb, a); }
            }
        };
        ring(0, e, 0xFF3A1C06u, 255);
        ring(e, e, 0xFFFFFFFFu, 240);
    }
    if (!ui_text_ready()) return;
    const bool word = strlen(label) > 1;
    float em = (word ? 6.5f : 8.5f) * sc;
    const float maxw = rw - 4 * sc;
    const float lw0 = ui_text_width(label, em);
    if (lw0 > maxw && lw0 > 0) em *= maxw / lw0;
    const uint32_t ink = hd ? C_HDKEYINK : lit ? C_INK : C_WOODTXT;
    text(s, rx + (rw - ui_text_width(label, em)) * 0.5f, ry + (rh - ui_text_cap_height(em)) * 0.5f, label, em, ink);
}

/* the sheet and the text line (no caret blink); *sheet gets a copy of the layer before the keys go on (the ground a lit key is drawn on); then
   every key unlit */
void draw_plank(const Surf &s, float x0, float y0, float sc, const PkbView *v, CaretAt *ca, std::vector<uint32_t> *sheet)
{
    const Skin &k = skin();
    const bool hd = k.hd != 0;
    auto X = [&](float dx) { return (int)floorf(x0 + dx * sc + 0.5f); };
    auto Y = [&](float dy) { return (int)floorf(y0 + dy * sc + 0.5f); };
    ca->has = 0;
    /* the sheet */
    int bx, by, bw, bh;
    pkb_panel_box(v->kind, &bx, &by, &bw, &bh);
    const int sx = X((float)bx), sy = Y((float)by), sw = X((float)(bx + bw)) - sx, sh = Y((float)(by + bh)) - sy;
    const int edge = sc * 1.5f < 2 ? 2 : (int)(sc * 1.5f);
    rrect(s, sx + (int)(sc), sy + (int)(1.5f * sc), sw, sh, 6 * sc, 0, 60);      /* a soft shadow */
    if (hd) {
        rrect(s, sx, sy, sw, sh, 6 * sc, 0x7A4614u, 255);
        const Img &p = parchment(sw - 2 * edge, sh - 2 * edge);
        rrect(s, sx + edge, sy + edge, sw - 2 * edge, sh - 2 * edge, 6 * sc - edge, 0, 255, &p);
    } else {
        framed(s, sx, sy, sw, sh, 3 * sc, edge, 0xE07020u, C_BOXEDGE);
    }
    /* the text line: the field's text so far (the room code: its six places, as the plank shows them). The caret's place is kept and its blink is
       drawn per frame; the line is laid out as if the caret showed, so the blink never moves the text. */
    {
        float tx, ty, tw, th;
        pkb_text_box(v->kind, &tx, &ty, &tw, &th);
        const int lx = X(tx), ly = Y(ty), lw = X(tx + tw) - lx, lh = Y(ty + th) - ly;
        int ix = lx, iw = lw, iy = ly, ih = lh;      /* the face the text sits on */
        if (hd) {
            const float kf = (float)lh / 171.0f;      /* the key plaque at the lobby page's own scale for a field this tall */
            blit(s, lx, ly, nine(A_KEY, lw, lh, 70, 55, 70, 55, kf));
            ix = lx + (int)(54 * kf + 0.5f); iw = lw - 2 * (int)(54 * kf + 0.5f);
            iy = ly + (int)(44 * kf + 0.5f); ih = lh - (int)(44 * kf + 0.5f) - (int)(42 * kf + 0.5f);
        } else {
            framed(s, lx, ly, lw, lh, 3 * sc, edge > 2 ? edge - 1 : 2, 0xE87A1Cu, C_BOXEDGE);
        }
        const float em = 8.0f * sc, pad = 4.f * sc;
        const float room = iw - 2 * pad;
        if (ui_text_ready()) {
            std::string t;
            size_t at = std::string::npos;      /* the caret's character in t */
            if (v->kind == PKB_CODE) {
                const char *c = v->text ? v->text : "";
                int n = 0;
                for (; n < 6 && c[n]; ++n) t += c[n];
                for (int i = n; i < 6; ++i) t += '_';      /* laid out with the caret showing; its place is left empty below */
                if (n < 6) at = (size_t)n;
            } else {
                t = v->text ? v->text : "";
                t += "_";
                while (t.size() > 1 && ui_text_width(t.c_str(), em) > room) t.erase(0, 1);      /* a long line shows its end */
                at = t.size() - 1;
            }
            const float cx = v->kind == PKB_CODE ? ix + (iw - ui_text_width(t.c_str(), em)) * 0.5f : ix + pad;
            const float top = iy + (ih - ui_text_cap_height(em)) * 0.5f;
            const int clip[4] = { ix + (int)(pad * 0.5f), ly, ix + iw - (int)(pad * 0.5f), ly + lh };
            if (at != std::string::npos) {
                ca->has = 1;
                ca->x = cx + ui_text_width(t.substr(0, at).c_str(), em);
                ca->top = top;
                ca->em = em;
                memcpy(ca->clip, clip, sizeof clip);
                const std::string after = t.substr(at + 1);      /* the room code's empty places after the caret's */
                t.erase(at);
                if (!after.empty()) text(s, ca->x + ui_text_width("_", em), top, after.c_str(), em, C_INK, clip);
            }
            text(s, cx, top, t.c_str(), em, C_INK, clip);
        }
    }
    sheet->assign(s.px, s.px + (size_t)s.stride * s.h);
    /* the keys, all unlit; the lit one is drawn on its own small layer */
    const int n = pkb_key_count(v->kind);
    for (int i = 0; i < n; ++i) draw_key(s, x0, y0, sc, v, i, false);
    (void)C_DIMINK;
}

/* lay a premultiplied layer over the page (opaque pixels copied, the rest blended) */
void lay(uint32_t *px, int stride, int w, int h, const uint32_t *src0, int lstride, int lx, int ly, int lw, int lh)
{
    for (int j = 0; j < lh; ++j) {
        const int yy = ly + j;
        if (yy < 0 || yy >= h) continue;
        const uint32_t *src = src0 + (size_t)j * lstride;
        uint32_t *dst = px + (size_t)yy * stride;
        for (int i = 0; i < lw; ++i) {
            const int xx = lx + i;
            if (xx < 0 || xx >= w) continue;
            const uint32_t c = src[i];
            const int a = (int)(c >> 24);
            if (a == 255) { dst[xx] = c; continue; }
            if (!a) continue;
            const uint32_t d = dst[xx];
            const int ia = 255 - a;
            const int r = (int)((c >> 16) & 255) + (((d >> 16) & 255) * ia + 127) / 255, g = (int)((c >> 8) & 255) + (((d >> 8) & 255) * ia + 127) / 255,
                      b = (int)(c & 255) + ((d & 255) * ia + 127) / 255;
            dst[xx] = 0xFF000000u | (uint32_t)((r > 255 ? 255 : r) << 16) | (uint32_t)((g > 255 ? 255 : g) << 8) | (uint32_t)(b > 255 ? 255 : b);
        }
    }
}

/* THE CACHE. The plank (sheet, text line, every key unlit) is drawn once into a clear layer of its own box and kept. It is drawn again only when
   the field's text, CAPS, the field (kind, submit label, length), the picture's size or origin, the skin, or the font's readiness change (a plank
   drawn before the font loaded has no labels). Every frame lays that layer over the page, then the lit key (its own small layer over the sheet's
   ground, redrawn when the highlight moves) and the caret's blink (one glyph). */
struct Layer {
    std::vector<uint32_t> px, sheet;
    int x = 0, y = 0, w = 0, h = 0;
    int fw = -1, fh = -1, kind = -1, ok = -1, caps = -1, max_len = -1, hd = -1, font = -1;
    float x0 = 0, y0 = 0, sc = 0;
    std::string text;
    CaretAt caret = {};
    unsigned gen = 0;
    std::vector<uint32_t> lit;
    int lit_key = -1, lx = 0, ly = 0, lw = 0, lh = 0;
    unsigned lit_gen = ~0u;
};
Layer g_layer;
}  // namespace

void pkb_draw(uint32_t *px, int stride, int w, int h, float x0, float y0, float sc, const PkbView *v)
{
    if (!px || !v || w < 32 || h < 32 || sc <= 0.f) return;
    Layer &L = g_layer;
    const int hd = skin().hd != 0, font = ui_text_ready() ? 1 : 0;
    const char *t = v->text ? v->text : "";
    if (L.fw != w || L.fh != h || L.x0 != x0 || L.y0 != y0 || L.sc != sc || L.kind != v->kind || L.ok != v->ok || L.caps != v->caps ||
        L.max_len != v->max_len || L.hd != hd || L.font != font || L.text != t) {
        int bx, by, bw, bh;
        pkb_panel_box(v->kind, &bx, &by, &bw, &bh);
        const int m = (int)(2 * sc) + 2;      /* the shadow's offset and the rounding, with room */
        L.x = (int)floorf(x0 + bx * sc + 0.5f) - m;
        L.y = (int)floorf(y0 + by * sc + 0.5f) - m;
        L.w = (int)(bw * sc) + 2 * m + 2;
        L.h = (int)(bh * sc) + 2 * m + 2;
        L.px.assign((size_t)L.w * L.h, 0u);
        const Surf ls = { L.px.data(), L.w, L.w, L.h };
        draw_plank(ls, x0 - L.x, y0 - L.y, sc, v, &L.caret, &L.sheet);      /* whole pixels moved, so every rounding lands where it did on the page */
        L.fw = w; L.fh = h; L.x0 = x0; L.y0 = y0; L.sc = sc; L.kind = v->kind; L.ok = v->ok; L.caps = v->caps;
        L.max_len = v->max_len; L.hd = hd; L.font = font; L.text = t;
        ++L.gen;
    }
    if (L.lit_gen != L.gen || L.lit_key != v->key) {
        L.lit_gen = L.gen; L.lit_key = v->key; L.lw = 0;
        float kx, ky, kw, kh;
        if (v->key >= 0 && pkb_key_box(v->kind, v->key, &kx, &ky, &kw, &kh)) {
            const float ox = x0 - L.x, oy = y0 - L.y;
            const int rx = (int)floorf(ox + kx * sc + 0.5f), ry = (int)floorf(oy + ky * sc + 0.5f);
            const int rw = (int)floorf(ox + (kx + kw) * sc + 0.5f) - rx, rh = (int)floorf(oy + (ky + kh) * sc + 0.5f) - ry;
            if (rx >= 0 && ry >= 0 && rw > 0 && rh > 0 && rx + rw <= L.w && ry + rh <= L.h) {
                L.lit.resize((size_t)rw * rh);
                for (int j = 0; j < rh; ++j) memcpy(&L.lit[(size_t)j * rw], &L.sheet[(size_t)(ry + j) * L.w + rx], (size_t)rw * 4);
                const Surf ks = { L.lit.data(), rw, rw, rh };
                draw_key(ks, ox - rx, oy - ry, sc, v, v->key, true);      /* whole pixels moved again: the key lands on the same pixels */
                L.lx = rx; L.ly = ry; L.lw = rw; L.lh = rh;
            }
        }
    }
    lay(px, stride, w, h, L.px.data(), L.w, L.x, L.y, L.w, L.h);
    if (L.lw) lay(px, stride, w, h, L.lit.data(), L.lw, L.x + L.lx, L.y + L.ly, L.lw, L.lh);
    if (v->caret && L.caret.has && ui_text_ready()) {
        const Surf ps = { px, stride, w, h };
        const int clip[4] = { L.caret.clip[0] + L.x, L.caret.clip[1] + L.y, L.caret.clip[2] + L.x, L.caret.clip[3] + L.y };
        text(ps, L.caret.x + L.x, L.caret.top + L.y, "_", L.caret.em, C_INK, clip);
    }
}
