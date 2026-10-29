/* menu_compose.cpp -- the one-screen menu compose layer (run menu1, lane P0CORE).
 * See menu_compose.h for the model and runs/menu1/in/DESIGN.md sections B, C, F.
 *
 * NOTHING IN HERE TOUCHES THE ROM OR THE FRAME. It takes planes by pointer,
 * writes an ARGB buffer it is handed, and keeps its own small state. It is
 * linked into the exe and called by nobody until phase 1 wires the hooks, so
 * with no environment variable set the exe behaves as before.
 *
 * MENU LAYOUT SELFTEST: SM64DS_MENU_SELFTEST=1 (a static initialiser in this
 * file, so walk_window.cpp needs no edit: it runs before main, prints the
 * table to stderr and, when SM64DS_MENU_SELFTEST_OUT names a file, to that file
 * too, then exits 0 on all PASS and 1 otherwise).
 * SM64DS_MENU_TRACE=1 prints [menu] lines.
 */
#include "menu_compose.h"
#include "menu_hostimg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include <mutex>
#include <atomic>

namespace ntr { int gx_anti_aliasing(); int gx_menu_knob_value(const char *env); void gx_parallel_rows(int rows, int min_rows, void (*fn)(void *ctx, int y0, int y1), void *ctx, int knob_value); }

namespace {
/* The ROM 3D clear colours key_backdrop turns to black: 0x101820 and 0x080808 exactly. MENUS093B (B1): with the edge smoothing on, the
   title's near-black backdrop polygon (the 4:3 box of the wide attract) is smoothed against the margin fill along its outer column, a
   two pixel line one to a few steps off 0x080808 at the old 4:3 edges. With the smoothing on a pixel within 12 of 0x080808 on every
   channel is the same backdrop and keys too; off, only the exact values do, as before. */
inline bool clear_key(uint32_t p)
{
    p &= 0xffffffu;
    if (p == 0x101820u || p == 0x080808u) return true;
    if (!ntr::gx_anti_aliasing()) return false;
    const int r = (int)((p >> 16) & 255) - 8, g = (int)((p >> 8) & 255) - 8, b = (int)(p & 255) - 8;
    return r >= -8 && r <= 12 && g >= -8 && g <= 12 && b >= -8 && b <= 12;
}
}
#if defined(_MSC_VER) || defined(__SSE2__)
#include <emmintrin.h>
#endif

namespace {

/* ---- small arithmetic ---------------------------------------------------- */

long long fdiv(long long a, long long b)       /* floor, b > 0 */
{
    long long q = a / b;
    if ((a % b) != 0 && a < 0) --q;
    return q;
}

int gcd_i(int a, int b)
{
    while (b) { const int t = a % b; a = b; b = t; }
    return a < 0 ? -a : a;
}

int iabs(int a) { return a < 0 ? -a : a; }

/* canvas units -> host pixels, floor, for the reduced fraction unum/uden */
int u2h(const MenuLayout *L, int c) { return (int)fdiv((long long)c * L->unum, L->uden); }

int trace_on(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = getenv("SM64DS_MENU_TRACE");
        if (!e || !*e) e = getenv("SM64DS_SWAP_TRACE");   /* the level-clear compose's old name, kept as an alias */
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v;
}

/* MENUCOST1: the cheaper draw paths are on unless SM64DS_MENU_COST_OFF=1 (the reference arithmetic, kept for the picture A/B proof). */
int cost_fast(void)
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_MENU_COST_OFF"); v = (e && *e && *e != '0') ? 0 : 1; }
    return v;
}

#define MTRACE(...) do { if (trace_on()) { fprintf(stderr, "[menu] " __VA_ARGS__); fputc('\n', stderr); fflush(stderr); } } while (0)

/* DS blend arithmetic in 5 bits per channel, the way the hardware does it:
   toward white  c' = c + ((31 - c) * evy >> 4);  toward black  c' = c - (c * evy >> 4) */
uint32_t blend_ch(uint32_t argb, int evy, int to_white)
{
    uint32_t out = argb & 0xff000000u;
    for (int sh = 0; sh <= 16; sh += 8) {
        int c5 = (int)((argb >> sh) & 0xff) >> 3;
        c5 = to_white ? c5 + (((31 - c5) * evy) >> 4) : c5 - ((c5 * evy) >> 4);
        const uint32_t c8 = (uint32_t)((c5 << 3) | (c5 >> 2));
        out |= c8 << sh;
    }
    return out;
}

int luma(uint32_t argb)
{
    return (int)(((argb >> 16) & 0xff) * 3 + ((argb >> 8) & 0xff) * 6 + (argb & 0xff));
}

/* ---- module state ------------------------------------------------------- */

/* LOGIC-THREAD STATE (the decide step and every input path). Nothing here is read by the draw. */
MenuLayout g_cur;           /* the working layout: the latest decision's, with the live cursor (L.focus is input state) */
int  g_live;                /* the latest decision is live */
int  g_cur_recipe = -1;
int  g_cur_variant;
int  g_focus_shown;
int  g_q_real_held;
int  g_drag_piece = -1;
int  g_netplay;
int  g_fallback_frames;
unsigned g_frame;           /* decide count = MenuDecision.serial */
unsigned g_draw_frame;      /* the serial of the decision the draw is drawing (INT2: the draw never reads g_frame) */
int  (*g_enable)(int key);
MenuDecision g_pub[2];      /* the last two decisions, g_pub[g_pub_i] the latest (logic thread only) */
int  g_pub_i;

/* The one small locked structure shared with the draw thread: what the input reads as "the layout the player saw", and
   the autocrop latches the draw finds. */
std::mutex g_mu;
MenuLayout g_seen;          /* the latest decision's layout plus the draw's amendments (inverse reads this) */
int  g_seen_valid;
struct SharedLatch { int recipe, variant; DsRect r[MENU_MAX_PIECES]; uint16_t on; };
SharedLatch g_slatch = { -2, 0, {}, 0 };

/* DRAW-THREAD STATE. The bound decision is a private copy; the draw never reads the logic thread's variables above. */
MenuDecision g_dd;
MenuDecision g_dead;        /* all zero: "no menu this frame" */
const MenuSources *g_src;
int  g_dst_stride;                   /* HOOK1: pixels per row of the dst buffer; 0 = the picture width */
int  g_layer_mode;                   /* SAVEOVL1: dst is a menu LAYER (premultiplied ARGB, zero = nothing), so a blend keeps its alpha */
uint32_t *g_layer2;                  /* SAVEOVL1: the layer's second plane (same stride as dst): a translucent pixel over a translucent one */
long long g_layer_conflicts;         /* SAVEOVL1: pixels the two planes could not hold (a third translucent write) */
int  g_dd_lift_pending;              /* the lift was promised and the draw has not honoured it yet */
/* the engine-B planes by layer mask, made by the host callback at most once per mask per generation */
struct PlaneEnt { uint8_t layers, oam_on, lo, hi; const uint32_t *p; };
PlaneEnt g_planes[4];
int  g_nplanes;

int stride_of(int w) { return g_dst_stride > 0 ? g_dst_stride : w; }

/* the sub plane a piece reads: the host's masked scan-out for its layers, else the one plane */
const uint32_t *sub_plane(const MenuPiece *p, const MenuSources *s)
{
    if (!s) return 0;
    if (s->sub_for) {
        const int oam = p->oam_on && s->sub_for_oam;
        for (int i = 0; i < g_nplanes; ++i)
            if (g_planes[i].layers == p->layers && g_planes[i].oam_on == (oam ? 1 : 0) &&
                (!oam || (g_planes[i].lo == p->oam_lo && g_planes[i].hi == p->oam_hi)))
                return g_planes[i].p ? g_planes[i].p : s->sub;
        const uint32_t *q = oam ? s->sub_for_oam(p->layers, p->oam_lo, p->oam_hi) : s->sub_for(p->layers);
        if (g_nplanes < 4) {
            PlaneEnt &e = g_planes[g_nplanes++];
            e.layers = p->layers; e.oam_on = oam ? 1 : 0; e.lo = p->oam_lo; e.hi = p->oam_hi; e.p = q;
        }
        return q ? q : s->sub;
    }
    return s->sub;
}

int enabled(int key)
{
    if (key == MENU_KEY_NONE) return 1;
    return g_enable ? g_enable(key) : 0;
}

/* ---- source access ------------------------------------------------------ */

int src_opaque(const MenuPiece *p, const MenuSources *s, int sx, int sy)
{
    if (!s) return 0;
    switch (p->src) {
    case SRC_SUB: {
        const uint32_t *sp = sub_plane(p, s);
        if (!sp || sx < 0 || sy < 0 || sx >= 256 || sy >= 192) return 0;
        if (p->row_ok && !p->row_ok(sy)) return 0;
        const uint32_t c = sp[sy * 256 + sx];
        if (p->key_backdrop && (c & 0xffffffu) == (((s->bd_masked_on && sp != s->sub) ? s->sub_backdrop_masked : s->sub_backdrop) & 0xffffffu)) return 0;
        return 1;
    }
    case SRC_TOP_2D: {
        if (!s->top2d || sx < 0 || sy < 0 || sx >= 256 || sy >= 192) return 0;
        const MenuTopCell &c = s->top2d[sy * 256 + sx];
        if ((c.argb >> 24) == 0) return 0;
        if (c.owner > 4 || !((p->layers >> c.owner) & 1)) return 0;
        if (p->row_ok && !p->row_ok(sy)) return 0;
        return 1;
    }
    case SRC_TOP_HOSTROW:
        if (!s->hostrow || sx < 0 || sy < 0 || sx >= s->hostrow_w || sy >= s->hostrow_h) return 0;
        return (s->hostrow[sy * s->hostrow_stride + sx] >> 24) != 0;
    case SRC_TOP_PIC:
    default:
        return 0;   /* a picture has no "edge pixels" to cut: the census does not apply */
    }
}

}  // namespace

/* ====================================================================== */
/* layout solver                                                          */
/* ====================================================================== */

int menu_wide(const MenuLayout *L) { return L->canvas_w >= MENU_WIDE_CANVAS; }

int menu_scale_allowed(int n, int d)
{
    return (n == 1 && d == 1) || (n == 5 && d == 6) || (n == 3 && d == 4) || (n == 2 && d == 3) || (n == 1 && d == 2);
}

void menu_layout_begin(MenuLayout *L, int w, int h)
{
    const MenuFacts facts = L->facts;      /* the ROM facts of this tick survive a re-solve (MENUSPLIT1) */
    memset(L, 0, sizeof *L);
    L->facts = facts;
    L->w = w;
    L->h = h;
    L->focus = -1;
    L->back = -1;
    if (w < 1 || h < 1) { L->unum = 1; L->uden = 192; L->canvas_w = 0; return; }
    const int g = gcd_i(h, 192);
    L->unum = h / g;
    L->uden = 192 / g;
    L->canvas_w = (int)((long long)w * 192 / h);
}

MenuPiece menu_piece_make(MenuSrc src, int sx, int sy, int sw, int sh, int ax, int ay, int dx, int dy,
                          int knum, int kden)
{
    MenuPiece p;
    memset(&p, 0, sizeof p);
    p.src = src;
    p.layers = MENU_L_ALL;
    p.s.x = (short)sx; p.s.y = (short)sy; p.s.w = (short)sw; p.s.h = (short)sh;
    p.ax = (uint8_t)ax; p.ay = (uint8_t)ay;
    p.dx = (short)dx; p.dy = (short)dy;
    p.knum = (uint8_t)knum; p.kden = (uint8_t)kden;
    return p;
}

int menu_add_piece(MenuLayout *L, const MenuPiece &p)
{
    if (L->np >= MENU_MAX_PIECES) return -1;
    L->p[L->np] = p;
    return L->np++;
}

int menu_add_hotspot(MenuLayout *L, int piece, int bx, int by, int bw, int bh, int rom_index)
{
    if (L->nh >= MENU_MAX_HOTSPOTS) return -1;
    MenuHotspot &h = L->hs[L->nh];
    memset(&h, 0, sizeof h);
    h.piece = (uint8_t)piece;
    h.box.x = (short)bx; h.box.y = (short)by; h.box.w = (short)bw; h.box.h = (short)bh;
    h.rom_index = (int8_t)rom_index;
    for (int d = 0; d < 4; ++d) h.nav[d] = -2;
    return L->nh++;
}

/* SCALE AND PLACEMENT RULE (P0COREFIX). A piece's canvas size is the EXACT
   rational s.w * knum / kden (and s.h likewise), held in twelfths of a canvas
   unit (12 is divisible by every allowed kden 1, 2, 3, 4, 6). The canvas spans
   the picture exactly: one unit is unum / uden host pixels and the canvas is
   w * uden / unum units wide, so a centred piece sits at (w - width) / 2 with
   no floored canvas_w in the way. Both edges of a piece are the floor of the
   exact rational edge position, so
     - the dest size differs from the exact size (s.w * knum * unum / (kden *
       uden)) by LESS THAN ONE host pixel;
     - two pieces that meet on a canvas edge share the same host edge (no seam,
       no overlap).
   A piece that pokes out of the picture by no more than one canvas unit plus
   one pixel (the rounding slack, e.g. a 256 wide piece on a 255.8 unit wide
   canvas at 1022x767) is clamped to the picture; anything further out is left
   outside so the selftest bounds check still catches a bad recipe. */
void menu_layout_dest(MenuLayout *L)
{
    const long long UN = L->unum, UD = L->uden;
    const long long D = 2LL * UD * 12;
    int slack = u2h(L, 1) + 1;
    if (slack < 1) slack = 1;
    for (int i = 0; i < L->np; ++i) {
        const MenuPiece &P = L->p[i];
        int *d = L->dest[i];
        if (P.lden > 0 || P.fixdest) {   /* HOOK1 / FACE1: the recipe placed it in host pixels */
            d[0] = P.fx; d[1] = P.fy; d[2] = P.fw; d[3] = P.fh;
            continue;
        }
        if (P.src == SRC_TOP_HOSTROW) {
            /* host pixels, 1:1; anchors and offsets in host terms */
            const int dw = P.s.w, dh = P.s.h;
            int x0 = P.ax == MENU_AX_L ? 0 : (P.ax == MENU_AX_C ? (L->w - dw) / 2 : L->w - dw);
            int y0 = P.ay == MENU_AY_T ? 0 : (P.ay == MENU_AY_M ? (L->h - dh) / 2 : L->h - dh);
            x0 += u2h(L, P.dx);
            y0 += u2h(L, P.dy);
            d[0] = x0; d[1] = y0; d[2] = dw; d[3] = dh;
            continue;
        }
        const int kn = P.knum ? P.knum : 1, kd = P.kden ? P.kden : 1;
        const long long cw12 = P.xfill ? (long long)P.fill_w * 12 : (long long)P.s.w * kn * 12 / kd,
                        ch12 = (long long)P.s.h * kn * 12 / kd;
        const long long dx12 = (long long)P.dx * 12, dy12 = (long long)P.dy * 12;
        const long long TW = (long long)L->w * UD * 12, TH = (long long)L->h * UD * 12;   /* the picture, in 1/(uden*12) px */
        const long long nx0 = P.ax == MENU_AX_L ? 2 * dx12 * UN
                            : P.ax == MENU_AX_C ? TW - cw12 * UN + 2 * dx12 * UN
                                                : 2 * (TW - cw12 * UN + dx12 * UN);
        const long long ny0 = P.ay == MENU_AY_T ? 2 * dy12 * UN
                            : P.ay == MENU_AY_M ? TH - ch12 * UN + 2 * dy12 * UN
                                                : 2 * (TH - ch12 * UN + dy12 * UN);
        int x0 = (int)fdiv(nx0, D), x1 = (int)fdiv(nx0 + 2 * cw12 * UN, D);
        int y0 = (int)fdiv(ny0, D), y1 = (int)fdiv(ny0 + 2 * ch12 * UN, D);
        if (x0 < 0 && x0 >= -slack) x0 = 0;
        if (x1 > L->w && x1 <= L->w + slack) x1 = L->w;
        if (y0 < 0 && y0 >= -slack) y0 = 0;
        if (y1 > L->h && y1 <= L->h + slack) y1 = L->h;
        d[0] = x0; d[1] = y0; d[2] = x1 - x0; d[3] = y1 - y0;
    }
}

void menu_layout_solve(MenuLayout *L, const MenuRecipe *r, int variant, int w, int h)
{
    menu_layout_begin(L, w, h);
    L->recipe = r;
    L->variant = variant;
    if (r && r->build && w >= 8 && h >= 8) r->build(variant, L);
    menu_layout_dest(L);
    if (L->focus >= L->nh) L->focus = -1;
}

/* ====================================================================== */
/* inverse                                                                */
/* ====================================================================== */

static int dest_contains(const int *d, int bx, int by)
{
    return d[2] > 0 && d[3] > 0 && bx >= d[0] && bx < d[0] + d[2] && by >= d[1] && by < d[1] + d[3];
}

static void piece_inv(const MenuLayout *L, int i, int bx, int by, int *dsx, int *dsy)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    int x, y;
    if (P.lden > 0 && P.src != SRC_HOSTIMG) {   /* the legacy rational: DS = floor(host * lden / lnum), swap_inverse's own arithmetic */
        const int ln = P.lnum > 0 ? P.lnum : 1;
        x = P.s.x + (int)fdiv((long long)(bx - d[0]) * P.lden, ln);
        y = P.s.y + (int)fdiv((long long)(by - d[1]) * P.lden, ln);
    } else {
        x = P.s.x + (int)fdiv((long long)(bx - d[0]) * P.s.w, d[2] > 0 ? d[2] : 1);
        y = P.s.y + (int)fdiv((long long)(by - d[1]) * P.s.h, d[3] > 0 ? d[3] : 1);
    }
    if (x < P.s.x) x = P.s.x;
    if (x > P.s.x + P.s.w - 1) x = P.s.x + P.s.w - 1;
    if (y < P.s.y) y = P.s.y;
    if (y > P.s.y + P.s.h - 1) y = P.s.y + P.s.h - 1;
    *dsx = x;
    *dsy = y;
}

int menu_hit(const MenuLayout *L, int bx, int by, int *piece, int *dsx, int *dsy)
{
    for (int i = L->np - 1; i >= 0; --i) {   /* topmost first: later pieces draw over earlier ones */
        if (!L->p[i].touch) continue;
        if (!dest_contains(L->dest[i], bx, by)) continue;
        piece_inv(L, i, bx, by, dsx, dsy);
        if (piece) *piece = i;
        return 1;
    }
    return 0;
}

/* Outside every touch piece: clamp to the piece the drag started on (or, with
   no drag, the nearest touch piece) and say in = 0, as swap_inverse does for
   its plate rectangle. No touch piece at all: (-1, -1, 0). */
void menu_inverse(const MenuLayout *L, int bx, int by, int *drag_piece, int *dsx, int *dsy, int *in)
{
    int pi = -1;
    if (L->wz > 1000) {
        /* MENUS093E: the page behind the touch piece is zoomed about the canvas centre. A touch on a hotspot is the unzoomed place (the buttons are drawn
           at their DS places); anywhere else is water, and the ROM puts the ripple at the touch point in the page's own space, so the point handed to
           it is the zoomed page's: the offset from the centre over (scale * zoom). Every pixel of the canvas is water, not only the 4:3 area. */
        int tp = -1;
        for (int i = L->np - 1; i >= 0; --i) if (L->p[i].touch) { tp = i; break; }
        if (tp >= 0) {
            int ux = 0, uy = 0, hp = -1;
            const bool on = menu_hit(L, bx, by, &hp, &ux, &uy);
            if (on) {
                for (int j = 0; j < L->nh; ++j) {
                    const MenuHotspot &H = L->hs[j];
                    if (H.piece == hp && ux >= H.box.x && ux < H.box.x + H.box.w && uy >= H.box.y && uy < H.box.y + H.box.h) {
                        if (drag_piece) *drag_piece = hp;
                        *dsx = ux; *dsy = uy; *in = 1;
                        return;
                    }
                }
            }
            if (bx >= 0 && by >= 0 && bx < L->w && by < L->h) {
                const MenuPiece &P = L->p[tp];
                const int *d = L->dest[tp];
                const double uz = d[2] > 0 ? (double)d[2] / (P.s.w > 0 ? P.s.w : 1) : 1.0;       /* picture pixels per DS unit */
                const double sc = uz * (double)L->wz / 1000.0;
                const double zx = P.s.x + P.s.w / 2.0 + ((double)bx - (L->w / 2.0)) / sc;         /* the zoomed page's point */
                const double zy = P.s.y + P.s.h / 2.0 + ((double)by - (L->h / 2.0)) / sc;
                const double ux = P.s.x + P.s.w / 2.0 + ((double)bx - (L->w / 2.0)) / uz;         /* the unzoomed point */
                const double uy = P.s.y + P.s.h / 2.0 + ((double)by - (L->h / 2.0)) / uz;
                /* the ROM reads ONE touch point for the ripple AND for its buttons, so the water point must not land in a button the pointer is not over:
                   walk from the zoomed point toward the unzoomed one (which is clear: a click on a button was answered above) to the first clear point */
                for (int k = 0; k <= 16; ++k) {
                    const double fx = zx + (ux - zx) * k / 16.0, fy = zy + (uy - zy) * k / 16.0;
                    int ix = (int)(fx < 0 ? 0 : fx), iy = (int)(fy < 0 ? 0 : fy);
                    if (ix > P.s.x + P.s.w - 1) ix = P.s.x + P.s.w - 1;
                    if (iy > P.s.y + P.s.h - 1) iy = P.s.y + P.s.h - 1;
                    bool clear = true;
                    for (int j = 0; j < L->nh && clear; ++j) {
                        const MenuHotspot &H = L->hs[j];
                        if (H.piece == tp && ix >= H.box.x - 1 && ix <= H.box.x + H.box.w && iy >= H.box.y - 1 && iy <= H.box.y + H.box.h) clear = false;
                    }
                    if (!clear) continue;
                    if (drag_piece) *drag_piece = tp;
                    *dsx = ix; *dsy = iy; *in = 1;
                    return;
                }
            }
        }
    }
    if (menu_hit(L, bx, by, &pi, dsx, dsy)) {
        if (drag_piece) *drag_piece = pi;
        *in = 1;
        return;
    }
    *in = 0;
    int best = -1;
    if (drag_piece && *drag_piece >= 0 && *drag_piece < L->np && L->p[*drag_piece].touch) {
        best = *drag_piece;
    } else {
        long long bd = -1;
        for (int i = 0; i < L->np; ++i) {
            if (!L->p[i].touch) continue;
            const int *d = L->dest[i];
            const int cx = bx < d[0] ? d[0] : (bx > d[0] + d[2] - 1 ? d[0] + d[2] - 1 : bx);
            const int cy = by < d[1] ? d[1] : (by > d[1] + d[3] - 1 ? d[1] + d[3] - 1 : by);
            const long long dd = (long long)(cx - bx) * (cx - bx) + (long long)(cy - by) * (cy - by);
            if (bd < 0 || dd < bd) { bd = dd; best = i; }
        }
    }
    if (best < 0) { *dsx = -1; *dsy = -1; return; }
    const int *d = L->dest[best];
    int cx = bx < d[0] ? d[0] : (bx > d[0] + d[2] - 1 ? d[0] + d[2] - 1 : bx);
    int cy = by < d[1] ? d[1] : (by > d[1] + d[3] - 1 ? d[1] + d[3] - 1 : by);
    piece_inv(L, best, cx, cy, dsx, dsy);
}

/* ====================================================================== */
/* hotspots and the focus graph                                           */
/* ====================================================================== */

static int ds_to_host_x(const MenuLayout *L, int i, int dsx)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    return d[0] + (int)fdiv((long long)(dsx - P.s.x) * d[2], P.s.w > 0 ? P.s.w : 1);
}

static int ds_to_host_y(const MenuLayout *L, int i, int dsy)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    return d[1] + (int)fdiv((long long)(dsy - P.s.y) * d[3], P.s.h > 0 ? P.s.h : 1);
}

int menu_hotspot_rect(const MenuLayout *L, int hs, int r[4])
{
    r[0] = r[1] = r[2] = r[3] = 0;
    if (hs < 0 || hs >= L->nh) return 0;
    const MenuHotspot &H = L->hs[hs];
    if (H.piece >= L->np) return 0;
    const MenuPiece &P = L->p[H.piece];
    int x0 = H.box.x, y0 = H.box.y, x1 = H.box.x + H.box.w, y1 = H.box.y + H.box.h;
    if (x0 < P.s.x) x0 = P.s.x;
    if (y0 < P.s.y) y0 = P.s.y;
    if (x1 > P.s.x + P.s.w) x1 = P.s.x + P.s.w;
    if (y1 > P.s.y + P.s.h) y1 = P.s.y + P.s.h;
    if (x1 <= x0 || y1 <= y0) return 0;
    const int hx0 = ds_to_host_x(L, H.piece, x0), hx1 = ds_to_host_x(L, H.piece, x1);
    const int hy0 = ds_to_host_y(L, H.piece, y0), hy1 = ds_to_host_y(L, H.piece, y1);
    r[0] = hx0; r[1] = hy0;
    r[2] = hx1 > hx0 ? hx1 - hx0 : 1;
    r[3] = hy1 > hy0 ? hy1 - hy0 : 1;
    return 1;
}

void menu_hotspot_centre(const MenuLayout *L, int hs, int *hx, int *hy)
{
    int r[4];
    menu_hotspot_rect(L, hs, r);
    *hx = r[0] + r[2] / 2;
    *hy = r[1] + r[3] / 2;
}

int menu_focus_hover(MenuLayout *L, int bx, int by)
{
    for (int i = L->nh - 1; i >= 0; --i) {
        int r[4];
        if (!menu_hotspot_rect(L, i, r)) continue;
        if (dest_contains(r, bx, by)) { L->focus = i; return 1; }
    }
    return 0;
}

/* Automatic spatial navigation over the hotspots' placed centres: the nearest
   hotspot in the pressed direction. Candidates inside the 45 degree cone of
   the direction beat the ones outside it; within a class the score is
   primary distance + twice the lateral distance; a tie goes to the larger
   overlap of the two rects across the movement axis, then the lower index. */
int menu_focus_move(MenuLayout *L, int dir)
{
    if (L->nh <= 0) return L->focus = -1;
    if (L->focus < 0 || L->focus >= L->nh) return L->focus = 0;
    const int f = L->focus;
    const int n = L->hs[f].nav[dir & 3];
    if (n >= 0 && n < L->nh) return L->focus = n;
    if (n == -1) return f;
    int fr[4], fcx, fcy;
    menu_hotspot_rect(L, f, fr);
    menu_hotspot_centre(L, f, &fcx, &fcy);
    int best = -1, best_class = 2, best_score = 0, best_ov = -1;
    for (int j = 0; j < L->nh; ++j) {
        if (j == f) continue;
        int jr[4], jcx, jcy;
        if (!menu_hotspot_rect(L, j, jr)) continue;
        menu_hotspot_centre(L, j, &jcx, &jcy);
        const int dx = jcx - fcx, dy = jcy - fcy;
        int prim, lat, ov;
        switch (dir & 3) {
        case MENU_DIR_UP:    prim = -dy; lat = iabs(dx); break;
        case MENU_DIR_DOWN:  prim = dy;  lat = iabs(dx); break;
        case MENU_DIR_LEFT:  prim = -dx; lat = iabs(dy); break;
        default:             prim = dx;  lat = iabs(dy); break;
        }
        if (prim <= 0) continue;
        if ((dir & 3) <= MENU_DIR_DOWN) {   /* movement is vertical: overlap along x */
            const int a = fr[0] > jr[0] ? fr[0] : jr[0];
            const int b = (fr[0] + fr[2]) < (jr[0] + jr[2]) ? (fr[0] + fr[2]) : (jr[0] + jr[2]);
            ov = b - a;
        } else {
            const int a = fr[1] > jr[1] ? fr[1] : jr[1];
            const int b = (fr[1] + fr[3]) < (jr[1] + jr[3]) ? (fr[1] + fr[3]) : (jr[1] + jr[3]);
            ov = b - a;
        }
        const int cls = prim >= lat ? 0 : 1;
        const int score = prim + 2 * lat;
        int better = 0;
        if (best < 0 || cls < best_class) better = 1;
        else if (cls == best_class) {
            if (score < best_score) better = 1;
            else if (score == best_score && ov > best_ov) better = 1;
        }
        if (better) { best = j; best_class = cls; best_score = score; best_ov = ov; }
    }
    if (best >= 0) L->focus = best;
    return L->focus;
}

/* ====================================================================== */
/* piece draw, backdrop, highlight, census                                */
/* ====================================================================== */

/* ---- ASSET2: host-resolution gradient and picture ------------------------------------------------------------- */
namespace {
float g_sky_ref = 0.f, g_fade = 1.f;     /* brightest sky luminance seen on this page, and this frame's fade factor */
int   g_sky_frame = -1000;
float g_go_ref = 0.f, g_go_fade = 1.f;   /* HDART1: the game over gradient's own fade (see go_fade_update) */
int   g_go_frame = -1000;

/* MENUCOST1: what the gradient kept from the last frame (the medians, the smoothed ramp, the finished rect) */
struct GradCache {
    int valid; uint64_t hash; float rr[192], gg[192], bb[192], sr[192], sg[192], sb[192];
    int pix_ok; uint64_t pix_hash; int pd[4]; std::vector<uint32_t> pix;
} g_gc;

void draw_hostgrad(const MenuLayout *L, int i, const MenuSources *S, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    const uint32_t *sp = sub_plane(&P, S);
    if (!sp || d[2] <= 0 || d[3] <= 0) return;
    /* the mean colour of each DS row over the sample columns */
    float rr[192], gg[192], bb[192];
    int n = P.s.w;
    if (n < 1) n = 1;
    /* MENUCOST1: the medians and the smoothing depend only on the sampled columns: hashed, and kept while they do not change */
    const int fast = cost_fast();
    uint64_t hsh = 1469598103934665603ull;
    int hit = 0;
    if (fast) {
        for (int y = 0; y < 192; ++y)
            for (int x = P.s.x; x < P.s.x + n && x < 256; ++x) {
                if (x < 0) continue;
                hsh = (hsh ^ sp[y * 256 + x]) * 1099511628211ull;
            }
        hsh ^= (uint64_t)(unsigned)P.s.x * 31u + (uint64_t)(unsigned)n;
        hit = g_gc.valid && g_gc.hash == hsh;
    }
    if (hit) { memcpy(rr, g_gc.rr, sizeof rr); memcpy(gg, g_gc.gg, sizeof gg); memcpy(bb, g_gc.bb, sizeof bb); }
    else for (int y = 0; y < 192; ++y) {
        /* the MEDIAN of each channel over the sample columns: a few columns that catch the edge of an object (the DS logo's
           left end, a black sprite pixel) are a minority and never reach the answer */
        int r[256], g[256], b[256], c = 0;
        for (int x = P.s.x; x < P.s.x + n && x < 256 && c < 256; ++x) {
            if (x < 0) continue;
            const uint32_t v = sp[y * 256 + x];
            r[c] = (v >> 16) & 255; g[c] = (v >> 8) & 255; b[c] = v & 255; ++c;
        }
        if (!c) { r[0] = g[0] = b[0] = 0; c = 1; }
        std::sort(r, r + c); std::sort(g, g + c); std::sort(b, b + c);
        rr[y] = (float)r[c / 2]; gg[y] = (float)g[c / 2]; bb[y] = (float)b[c / 2];
    }
    /* The DS gradient is banded (runs of equal rows of uneven length, each channel stepping at its own rows). Smooth it: two
       passes of a box of +-8 rows with odd reflection at both ends (so the end slopes are kept), which removes the band
       lumps and keeps the ramp's overall shape and colours. Every DS row is then one stop at its centre. */
    float sy[192], sr[192], sg[192], sb[192];
    const int ns = 192;
    if (hit) {
        memcpy(sr, g_gc.sr, sizeof sr); memcpy(sg, g_gc.sg, sizeof sg); memcpy(sb, g_gc.sb, sizeof sb);
        for (int y = 0; y < 192; ++y) sy[y] = y + 0.5f;
    } else {
        float *ch[3] = { rr, gg, bb }, *out[3] = { sr, sg, sb };
        for (int k = 0; k < 3; ++k) {
            float cur[192], nxt[192];
            memcpy(cur, ch[k], sizeof cur);
            for (int pass = 0; pass < 2; ++pass) {
                for (int y = 0; y < 192; ++y) {
                    float acc = 0.f;
                    for (int j = -8; j <= 8; ++j) {
                        int q = y + j;
                        float v;
                        if (q < 0) v = 2.f * cur[0] - cur[-q > 191 ? 191 : -q];
                        else if (q > 191) v = 2.f * cur[191] - cur[(382 - q) < 0 ? 0 : 382 - q];
                        else v = cur[q];
                        acc += v;
                    }
                    nxt[y] = acc / 17.f;
                }
                memcpy(cur, nxt, sizeof cur);
            }
            memcpy(out[k], cur, sizeof cur);
        }
        for (int y = 0; y < 192; ++y) sy[y] = y + 0.5f;
    }
    if (fast && !hit) {
        g_gc.valid = 1; g_gc.hash = hsh;
        memcpy(g_gc.rr, rr, sizeof rr); memcpy(g_gc.gg, gg, sizeof gg); memcpy(g_gc.bb, bb, sizeof bb);
        memcpy(g_gc.sr, sr, sizeof sr); memcpy(g_gc.sg, sg, sizeof sg); memcpy(g_gc.sb, sb, sizeof sb);
    }
    /* the fade factor: the middle of the sky now over the brightest it has been on this page */
    {
        const float lum = 0.3f * rr[96] + 0.59f * gg[96] + 0.11f * bb[96];
        if ((int)g_draw_frame - g_sky_frame > 30) g_sky_ref = 0.f;
        g_sky_frame = (int)g_draw_frame;
        if (lum > g_sky_ref) g_sky_ref = lum;
        g_fade = g_sky_ref > 8.f ? lum / g_sky_ref : 1.f;
        if (g_fade > 1.f) g_fade = 1.f;
        if (g_fade < 0.f) g_fade = 0.f;
        if (hal_menu_trace_on() && (g_draw_frame % 60) == 0)
            MTRACE("hostgrad: %d stops (smoothed), mid rgb %.1f %.1f %.1f, fade %.3f", ns, rr[96], gg[96], bb[96], g_fade);
    }
    static const int bayer[4][4] = { { 0, 8, 2, 10 }, { 12, 4, 14, 6 }, { 3, 11, 1, 9 }, { 15, 7, 13, 5 } };
    const size_t stride = (size_t)stride_of(w);
    int seg = 0;
    if (fast) {
        /* the finished rect is kept while the ramp, the rect and nothing else changes, and copied per frame */
        if (!(g_gc.pix_ok && g_gc.pix_hash == hsh && g_gc.pd[0] == d[0] && g_gc.pd[1] == d[1] && g_gc.pd[2] == d[2] && g_gc.pd[3] == d[3] &&
              g_gc.pix.size() == (size_t)d[2] * d[3])) {
            g_gc.pix.assign((size_t)d[2] * d[3], 0u);
            for (int yy = 0; yy < d[3]; ++yy) {
                const int py = d[1] + yy;
                const float v = (yy + 0.5f) * 192.f / d[3];
                while (seg + 1 < ns && sy[seg + 1] <= v) ++seg;
                float r, g, b;
                if (v <= sy[0] || ns == 1) { r = sr[0]; g = sg[0]; b = sb[0]; }
                else if (seg + 1 >= ns) { r = sr[ns - 1]; g = sg[ns - 1]; b = sb[ns - 1]; }
                else {
                    const float t = (v - sy[seg]) / (sy[seg + 1] - sy[seg]);
                    r = sr[seg] + (sr[seg + 1] - sr[seg]) * t; g = sg[seg] + (sg[seg + 1] - sg[seg]) * t; b = sb[seg] + (sb[seg + 1] - sb[seg]) * t;
                }
                uint32_t pat[4];
                for (int k = 0; k < 4; ++k) {
                    const float dz = (bayer[py & 3][k] - 7.5f) / 16.f;
                    int ir = (int)(r + 0.5f + dz), ig = (int)(g + 0.5f + dz), ib = (int)(b + 0.5f + dz);
                    ir = ir < 0 ? 0 : ir > 255 ? 255 : ir; ig = ig < 0 ? 0 : ig > 255 ? 255 : ig; ib = ib < 0 ? 0 : ib > 255 ? 255 : ib;
                    pat[k] = 0xff000000u | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
                }
                uint32_t *prow = &g_gc.pix[(size_t)yy * d[2]];
                const int xs = d[0];
                for (int x = 0; x < d[2]; ++x) prow[x] = pat[(xs + x) & 3];
            }
            g_gc.pix_ok = 1; g_gc.pix_hash = hsh;
            g_gc.pd[0] = d[0]; g_gc.pd[1] = d[1]; g_gc.pd[2] = d[2]; g_gc.pd[3] = d[3];
        }
        const size_t stride = (size_t)stride_of(w);
        const int xa = d[0] < 0 ? -d[0] : 0, xb = d[0] + d[2] > w ? w - d[0] : d[2];
        for (int yy = 0; yy < d[3]; ++yy) {
            const int py = d[1] + yy;
            if (py < 0 || py >= h || xb <= xa) continue;
            memcpy(dst + (size_t)py * stride + d[0] + xa, &g_gc.pix[(size_t)yy * d[2] + xa], (size_t)(xb - xa) * 4);
        }
        return;
    }
    for (int yy = 0; yy < d[3]; ++yy) {
        const int py = d[1] + yy;
        if (py < 0 || py >= h) continue;
        const float v = (yy + 0.5f) * 192.f / d[3];
        while (seg + 1 < ns && sy[seg + 1] <= v) ++seg;
        float r, g, b;
        if (v <= sy[0] || ns == 1) { r = sr[0]; g = sg[0]; b = sb[0]; }
        else if (seg + 1 >= ns) { r = sr[ns - 1]; g = sg[ns - 1]; b = sb[ns - 1]; }
        else {
            const float t = (v - sy[seg]) / (sy[seg + 1] - sy[seg]);
            r = sr[seg] + (sr[seg + 1] - sr[seg]) * t; g = sg[seg] + (sg[seg + 1] - sg[seg]) * t; b = sb[seg] + (sb[seg + 1] - sb[seg]) * t;
        }
        uint32_t *row = dst + (size_t)py * stride;
        const int xa = d[0] < 0 ? 0 : d[0], xb = d[0] + d[2] > w ? w : d[0] + d[2];
        /* the 4x4 ordered dither repeats every 4 pixels: build the row's four colours once and tile them */
        uint32_t pat[4];
        for (int k = 0; k < 4; ++k) {
            const float dz = (bayer[py & 3][k] - 7.5f) / 16.f;
            int ir = (int)(r + 0.5f + dz), ig = (int)(g + 0.5f + dz), ib = (int)(b + 0.5f + dz);
            ir = ir < 0 ? 0 : ir > 255 ? 255 : ir; ig = ig < 0 ? 0 : ig > 255 ? 255 : ig; ib = ib < 0 ? 0 : ib > 255 ? 255 : ib;
            pat[k] = 0xff000000u | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
        }
        for (int x = xa; x < xb; ++x) row[x] = pat[x & 3];
    }
}

static float g_bob_frac[MENU_MAX_PIECES]; /* ARROWSLOW1: this picture's sub-pixel part of a bob piece's dest y (0 for every other piece) */
/* FLIPSMOOTH1: THE PLANK TURN BETWEEN TICKS. The title's online planks turn over (menu_online_plank_phase): the tick narrows the old face to a
   sliver and widens the new one, one step a tick. A picture between ticks draws each turning piece at its own phase p = turn_a - 1 + a (a = the
   picture's place between the tick before and this tick, ARROWSLOW1's g_relift_alpha): width |1 - p / half| of the face's full width (40
   permille at least, as the tick) about the tick rect's centre, x and width only. A piece whose face is not the one p shows is hidden on that
   picture: on the tick of the middle the recipe adds the OLD face too (face 1 on a face-2 tick), so the pictures before the middle narrow the
   old face to nothing and the swap lands on the tick picture, edge-on, where the tick swaps it. a = 1 (the tick's own picture, Smooth Motion
   off) changes nothing but hiding that old face. A host picture plank drawn narrower between ticks is squeezed from one copy of its full-width
   scale (turn_squeezed) instead of being scaled again every picture. */
static uint8_t g_turn_hide[MENU_MAX_PIECES];      /* this picture does not draw the piece */
static uint8_t g_turn_sq[MENU_MAX_PIECES];        /* this picture moved the piece's rect (1), and draws its host picture squeezed (2) */
static int g_turn_keep[MENU_MAX_PIECES][2];       /* the tick rect's x and width, put back after the draw */
static void turn_place(MenuLayout &L, double a)
{
    for (int i = 0; i < L.np && i < MENU_MAX_PIECES; ++i) {
        g_turn_hide[i] = 0; g_turn_sq[i] = 0;
        const MenuPiece &P = L.p[i];
        if (!P.turn || P.turn_half < 1) continue;
        const double half = (double)P.turn_half;
        double p = (double)P.turn_a - (a < 1.0 ? 1.0 - a : 0.0);
        if (p < 0.0) p = 0.0;
        if (p > 2.0 * half) p = 2.0 * half;
        const int face = p < half ? 1 : 2;
        if (face != P.turn) { g_turn_hide[i] = 1; continue; }
        if (a >= 1.0) continue;
        double wf = p < half ? 1.0 - p / half : (p - half) / half;
        if (wf < 0.04) wf = 0.04;
        int dw = (int)((double)P.turn_fw * wf);          /* truncated, as the tick law (full width * w1000 / 1000) */
        if (dw < 1) dw = 1;
        int *d = L.dest[i];
        g_turn_keep[i][0] = d[0]; g_turn_keep[i][1] = d[2];
        const int cx = d[0] + d[2] / 2;
        d[0] = cx - dw / 2; d[2] = dw;
        g_turn_sq[i] = P.src == SRC_HOSTIMG && !P.custom ? 2 : 1;
    }
#ifndef PORT_ROM_CLEAN
    { static const int tr = getenv("SM64DS_FS1_TRACE") ? 1 : 0;     /* FLIPSMOOTH1 proof instrument: every turning piece on every picture */
      if (tr) for (int i = 0; i < L.np && i < MENU_MAX_PIECES; ++i) if (L.p[i].turn)
          fprintf(stderr, "[fs1-turn] a=%.4f piece %d face %d phase %.3f hide %d rect %d %d\n", a, i, (int)L.p[i].turn,
                  (double)L.p[i].turn_a - (a < 1.0 ? 1.0 - a : 0.0), (int)g_turn_hide[i], L.dest[i][0], L.dest[i][2]); }
#endif
}
static void turn_restore(MenuLayout &L)
{
    for (int i = 0; i < L.np && i < MENU_MAX_PIECES; ++i) {
        if (g_turn_sq[i]) { L.dest[i][0] = g_turn_keep[i][0]; L.dest[i][2] = g_turn_keep[i][1]; }
        g_turn_sq[i] = 0; g_turn_hide[i] = 0;
    }
}
/* the piece's picture at dw x dh, squeezed by area from its full-width (turn_fw) scale; the full-width copy is kept here per picture id and
   size, so the shared two-slot scale cache sees one request per face and turn, not one per picture */
static const uint32_t *turn_squeezed(const MenuPiece &P, int dw, int dh)
{
    struct Full { int id, w, h; std::vector<uint32_t> px; };
    static Full full[8];
    static int next;
    static std::vector<uint32_t> out;
    const int fw = P.turn_fw;
    if (fw < 1 || dw < 1 || dh < 1 || dw > fw) return menu_hostimg_scaled(P.hostimg, dw, dh);
    Full *F = 0;
    for (int k = 0; k < 8; ++k)
        if (full[k].id == P.hostimg && full[k].w == fw && full[k].h == dh && !full[k].px.empty()) { F = &full[k]; break; }
    if (!F) {
        const uint32_t *s = menu_hostimg_scaled(P.hostimg, fw, dh);
        if (!s) return 0;
        F = &full[next]; next = (next + 1) & 7;
        F->id = P.hostimg; F->w = fw; F->h = dh;
        F->px.assign(s, s + (size_t)fw * dh);
    }
    out.resize((size_t)dw * dh);
    /* output column x covers source [x * fw / dw, (x + 1) * fw / dw): an area average of the premultiplied channels */
    for (int x = 0; x < dw; ++x) {
        const double s0 = (double)x * fw / dw, s1 = (double)(x + 1) * fw / dw;
        const int c0 = (int)s0, c1 = (int)ceil(s1) > fw ? fw : (int)ceil(s1);
        for (int y = 0; y < dh; ++y) {
            const uint32_t *row = &F->px[(size_t)y * fw];
            double acc[4] = { 0, 0, 0, 0 }, wsum = 0;
            for (int c = c0; c < c1; ++c) {
                const double lo = c < s0 ? s0 : (double)c, hi = c + 1 > s1 ? s1 : (double)(c + 1);
                const double wgt = hi - lo;
                if (wgt <= 0) continue;
                const uint32_t v = row[c];
                for (int k = 0; k < 4; ++k) acc[k] += wgt * (double)((v >> (8 * k)) & 255u);
                wsum += wgt;
            }
            uint32_t o = 0;
            if (wsum > 0)
                for (int k = 0; k < 4; ++k) { int q = (int)(acc[k] / wsum + 0.5); if (q > 255) q = 255; o |= (uint32_t)q << (8 * k); }
            out[(size_t)y * dw + x] = o;
        }
    }
    return out.data();
}

void draw_hostimg(const MenuLayout *L, int i, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    const uint32_t *src = i < MENU_MAX_PIECES && g_turn_sq[i] == 2 ? turn_squeezed(P, d[2], d[3]) : menu_hostimg_scaled(P.hostimg, d[2], d[3]);   /* FLIPSMOOTH1 */
    if (!src) return;
    const size_t stride = (size_t)stride_of(w);
    /* ARROWSLOW1: a bob piece drawn bf of a pixel lower than its dest rect: each row is the premultiplied mix of the picture's row and the
       one above it, one row taller, so the arrow glides between whole pixels instead of stepping */
    int rows = d[3];
    const float bf = P.bob && i < MENU_MAX_PIECES ? g_bob_frac[i] : 0.f;
    if (bf > 1.f / 512.f && d[2] > 0 && d[3] > 0) {
        static thread_local std::vector<uint32_t> sh;
        const unsigned f8 = (unsigned)(bf * 256.f + 0.5f), g8 = 256 - f8;
        const int W = d[2], H = d[3];
        sh.resize((size_t)W * (H + 1));
        for (int r = 0; r <= H; ++r)
            for (int x = 0; x < W; ++x) {
                const uint32_t c = r < H ? src[(size_t)r * W + x] : 0u, q = r > 0 ? src[(size_t)(r - 1) * W + x] : 0u;
                uint32_t o = 0;
                for (int k = 0; k < 32; k += 8)
                    o |= (((((c >> k) & 255u) * g8 + ((q >> k) & 255u) * f8 + 128u) >> 8) & 255u) << k;
                sh[(size_t)r * W + x] = o;
            }
        src = sh.data();
        rows = H + 1;
    }
    int f256 = (P.hostimg == MENU_IMG_LOGO && !P.nofade) ? (int)(g_fade * 256.f + 0.5f) : 256;
    if (P.hostimg == MENU_IMG_GO_GRAD || P.hostimg == MENU_IMG_GO_CLICK || (P.hostimg >= MENU_IMG_GO_L0 && P.hostimg <= MENU_IMG_GO_L7))
        f256 = (int)(g_go_fade * 256.f + 0.5f);   /* HDART1/2: fades in with the ROM's own gradient (the lettering and the text are engine A's too) */
    const unsigned af = P.alpha ? P.alpha : 255u;     /* ASSET5: the piece's own fade (premultiplied: colour and alpha scale together) */
    /* the plank's own look: selected = lifted, pulsing on the ROM's 8-frame cadence; pressed = sunk and darker */
    const int owns = L->focus >= 0 && L->focus < L->nh && L->hs[L->focus].piece == i;
    /* ASSET3: the recipe's hl bit is the one selection (the ROM cursor, which a mouse hover now drives), so a plank the mouse
       merely sits over is not lit a second time when the pad has moved the cursor elsewhere */
    int sel = (P.hl & 1);
    const int prs = (P.hl & 2) || (owns && g_dd.real_held);
    int boost = 0, sink = 0;
    /* the plank's brightness pulse (x1.14 .. x1.30) lightens the lettering at its peak and fights the arrows, so a selected plank is
       held at the steady x1.14 lift; SM64DS_MENU_PULSE=1 brings the pulse back (round 3 picture comparison) */
    static int no_pulse = -1;
    if (no_pulse < 0) { const char *e = getenv("SM64DS_MENU_PULSE"); no_pulse = (e && *e && *e != '0') ? 0 : 1; }
    if (menu_hostimg_is_plank(P.hostimg)) {   /* the planks; the logo and the arrows are never lit */
        static const int pulse[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };
        if (sel) boost = no_pulse ? 292 : 292 + pulse[g_draw_frame & 7] * 10;    /* x1.14 .. x1.30 */
        if (prs) { f256 = f256 * 216 / 256; boost = 0; sink = d[3] * 3 / 100; if (sink < 1) sink = 1; }
    }
    for (int yy = 0; yy < rows; ++yy) {
        const int py = d[1] + yy + sink;
        if (py < 0 || py >= h) continue;
        uint32_t *row = dst + (size_t)py * stride;
        const uint32_t *sr = src + (size_t)yy * d[2];
        const int xs = d[0] < 0 ? -d[0] : 0, xe = d[0] + d[2] > w ? w - d[0] : d[2];
        int xx = xs;
#if defined(_MSC_VER) || defined(__SSE2__)
        /* MENUCOST1: a faded or darkened picture (the logo's fade, a pressed plank) four pixels at a time, in 16 bit lanes. The same integer
           arithmetic as the loop below: v = c * f256 >> 8, then (v * af + 127) / 255, then over the destination with (o * (255 - a) + 127) / 255,
           where x / 255 is (x + 1 + (x >> 8)) >> 8 (exact for 0 <= x < 65535, checked for every x); pixels with source alpha 0 keep the destination. */
        if (cost_fast() && !boost && (f256 != 256 || af != 255) && !g_layer_mode) {
            const __m128i zero = _mm_setzero_si128(), c1 = _mm_set1_epi16(1), c127 = _mm_set1_epi16(127), c255 = _mm_set1_epi16(255);
            const __m128i mulF = _mm_set_epi16(256, (short)f256, (short)f256, (short)f256, 256, (short)f256, (short)f256, (short)f256);
            const __m128i vaf = _mm_set1_epi16((short)af), opaque = _mm_set1_epi32((int)0xff000000u);
            for (; xx + 4 <= xe; xx += 4) {
                const __m128i sv = _mm_loadu_si128((const __m128i *)(sr + xx));
                const __m128i dv = _mm_loadu_si128((const __m128i *)(row + d[0] + xx));
                const __m128i a0 = _mm_cmpeq_epi32(_mm_srli_epi32(sv, 24), zero);
                __m128i half[2];
                for (int hsel = 0; hsel < 2; ++hsel) {
                    const __m128i s16 = hsel ? _mm_unpackhi_epi8(sv, zero) : _mm_unpacklo_epi8(sv, zero);
                    const __m128i d16 = hsel ? _mm_unpackhi_epi8(dv, zero) : _mm_unpacklo_epi8(dv, zero);
                    __m128i v = _mm_srli_epi16(_mm_mullo_epi16(s16, mulF), 8);
                    if (af != 255) {
                        const __m128i t = _mm_add_epi16(_mm_mullo_epi16(v, vaf), c127);
                        v = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(t, c1), _mm_srli_epi16(t, 8)), 8);
                    }
                    const __m128i al = _mm_shufflehi_epi16(_mm_shufflelo_epi16(v, 0xFF), 0xFF);
                    const __m128i ia = _mm_sub_epi16(c255, al);
                    const __m128i t2 = _mm_add_epi16(_mm_mullo_epi16(d16, ia), c127);
                    const __m128i q = _mm_srli_epi16(_mm_add_epi16(_mm_add_epi16(t2, c1), _mm_srli_epi16(t2, 8)), 8);
                    half[hsel] = _mm_min_epi16(_mm_add_epi16(v, q), c255);
                }
                const __m128i res = _mm_or_si128(_mm_packus_epi16(half[0], half[1]), opaque);
                _mm_storeu_si128((__m128i *)(row + d[0] + xx), _mm_or_si128(_mm_and_si128(a0, dv), _mm_andnot_si128(a0, res)));
            }
        }
#endif
        for (; xx < xe; ++xx) {
            const int px = d[0] + xx;
            const uint32_t c = sr[xx];
            unsigned a = c >> 24;
            if (!a) continue;
            if (a == 255 && f256 == 256 && !boost && af == 255) { row[px] = c; continue; }   /* the common pixel: opaque, no fade, no lit plank */
            unsigned cr = ((c >> 16) & 255) * f256 >> 8, cg = ((c >> 8) & 255) * f256 >> 8, cb = (c & 255) * f256 >> 8;
            if (af != 255) { a = (a * af + 127) / 255; cr = (cr * af + 127) / 255; cg = (cg * af + 127) / 255; cb = (cb * af + 127) / 255; }
            if (boost) {   /* brightened by boost/256 (a lit plank keeps its lettering dark), premultiplied, capped at alpha */
                cr = cr * boost >> 8; cg = cg * boost >> 8; cb = cb * boost >> 8;
                if (cr > a) cr = a;
                if (cg > a) cg = a;
                if (cb > a) cb = a;
            }
            const uint32_t o = row[px];
            const unsigned ia = 255 - a;
            const unsigned orr = cr + (((o >> 16) & 255) * ia + 127) / 255, og = cg + (((o >> 8) & 255) * ia + 127) / 255,
                           ob = cb + ((o & 255) * ia + 127) / 255;
            if (g_layer_mode) {
                /* SAVEOVL1: the layer holds what the picture would have been given, in the order it was given. The card lays
                   plane 1 and then plane 2 over its picture with this very rule, so the chain is exact: an opaque write replaces
                   the pixel; a translucent one over nothing is the pixel; over an opaque pixel it composes into it (the result
                   is opaque and equals the processor's); over a translucent pixel it goes to plane 2. A third translucent write
                   has no place: it is counted and the tick is drawn onto the picture as before. */
                const unsigned oa = o >> 24;
                if (a >= 255) { row[px] = 0xff000000u | ((cr > 255 ? 255 : cr) << 16) | ((cg > 255 ? 255 : cg) << 8) | (cb > 255 ? 255 : cb); continue; }
                if (oa == 0) { row[px] = ((a) << 24) | ((cr > 255 ? 255 : cr) << 16) | ((cg > 255 ? 255 : cg) << 8) | (cb > 255 ? 255 : cb); continue; }
                if (oa == 255) { row[px] = 0xff000000u | ((orr > 255 ? 255 : orr) << 16) | ((og > 255 ? 255 : og) << 8) | (ob > 255 ? 255 : ob); continue; }
                if (g_layer2 && !g_layer2[(size_t)py * stride + px]) {
                    g_layer2[(size_t)py * stride + px] = ((a) << 24) | ((cr > 255 ? 255 : cr) << 16) | ((cg > 255 ? 255 : cg) << 8) | (cb > 255 ? 255 : cb);
                    continue;
                }
                ++g_layer_conflicts;
                continue;
            }
            row[px] = 0xff000000u | ((orr > 255 ? 255 : orr) << 16) | ((og > 255 ? 255 : og) << 8) | (ob > 255 ? 255 : ob);
        }
    }
}

/* ---- ASSET5: THE WALLPAPER ------------------------------------------------------------------------------
   The title lower screen (the touch screen) is a vertical blue gradient with a tiled SUPER MARIO DS outline logo over it (the ROM's three
   wallpaper sprite objects). The gradient was measured off the ROM's own render (the lower screen at the settled main menu, background
   pixels only: blue channel at its 255 ceiling, red and green ramping): a straight line from (99, 123, 255) on the top row to
   (164, 228, 255) on the bottom row (row 96 predicted 131.5, 175.5 against 131.7, 175.4 measured). The pattern is our Super Mario 64 PC
   art tiled over it at the OG scale (the DS pattern repeats every 64 DS pixels; ours is one 64 unit wide tile) and tinted with the OG ink
   (measured: ink = bg * 0.58 + (0, 143, 155) * 0.42). The OG wallpaper does not move (identical pixels over 200 frames of the settled
   menu and through the arrival), so neither does this. Built once per size and parameters, copied per frame. */
std::vector<uint32_t> g_wall_cache;
int g_wall_key[10];

void draw_wall(const MenuLayout *L, int i, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    if (d[2] <= 0 || d[3] <= 0) return;
    static int kr = -1, kg = 0, kb = 0, kmode = 1;
    if (kr < 0) {
        kr = 0; kg = 143; kb = 155;
        const char *e = getenv("SM64DS_MENU_WALL_INK");     /* r,g,b,mode: tuning (mode 0 = the art's own colours) */
        if (e && *e) {
            int a, b, c, m;
            if (sscanf(e, "%d,%d,%d,%d", &a, &b, &c, &m) == 4) { kr = a; kg = b; kb = c; kmode = m; }
        }
    }
    int key[10] = { d[2], d[3], P.wtw, P.wth, P.wox, P.woy, P.wink, kr, kg * 256 + kb, kmode };
    if (g_wall_cache.size() != (size_t)d[2] * d[3] || memcmp(key, g_wall_key, sizeof key) != 0) {
        memcpy(g_wall_key, key, sizeof key);
        g_wall_cache.assign((size_t)d[2] * d[3], 0xff000000u);
        static const int bayer[4][4] = { { 0, 8, 2, 10 }, { 12, 4, 14, 6 }, { 3, 11, 1, 9 }, { 15, 7, 13, 5 } };
        const uint32_t *tile = (P.wtw > 0 && P.wth > 0) ? menu_hostimg_scaled(MENU_IMG_PATTERN, P.wtw, P.wth) : 0;
        for (int y = 0; y < d[3]; ++y) {
            const float v = (y + 0.5f) / d[3];
            const float gr = 99.f + 65.f * v, gg = 123.f + 105.f * v, gb = 255.f;
            const int ty = tile ? (((y - P.woy) % P.wth) + P.wth) % P.wth : 0;
            for (int x = 0; x < d[2]; ++x) {
                const float dz = (bayer[y & 3][x & 3] - 7.5f) / 16.f;
                float r = gr + dz, g = gg + dz, b = gb + dz;
                if (tile) {
                    const int tx = (((x - P.wox) % P.wtw) + P.wtw) % P.wtw;
                    const uint32_t tp = tile[(size_t)ty * P.wtw + tx];
                    const unsigned ta = tp >> 24;
                    if (ta) {
                        const float m = ta * (float)P.wink / (255.f * 255.f);
                        if (kmode == 0) {   /* the art's own colours (premultiplied) over the gradient */
                            const float pa = m;
                            r = r * (1.f - pa) + (float)((tp >> 16) & 255) * (float)P.wink / 255.f;
                            g = g * (1.f - pa) + (float)((tp >> 8) & 255) * (float)P.wink / 255.f;
                            b = b * (1.f - pa) + (float)(tp & 255) * (float)P.wink / 255.f;
                        } else {
                            r += (kr - r) * m; g += (kg - g) * m; b += (kb - b) * m;
                        }
                    }
                }
                int ir = (int)(r + 0.5f), ig = (int)(g + 0.5f), ib = (int)(b + 0.5f);
                ir = ir < 0 ? 0 : ir > 255 ? 255 : ir; ig = ig < 0 ? 0 : ig > 255 ? 255 : ig; ib = ib < 0 ? 0 : ib > 255 ? 255 : ib;
                g_wall_cache[(size_t)y * d[2] + x] = 0xff000000u | ((uint32_t)ir << 16) | ((uint32_t)ig << 8) | (uint32_t)ib;
            }
        }
    }
    const size_t stride = (size_t)stride_of(w);
    for (int yy = 0; yy < d[3]; ++yy) {
        const int py = d[1] + yy;
        if (py < 0 || py >= h) continue;
        const int xa = d[0] < 0 ? -d[0] : 0, xb = d[0] + d[2] > w ? w - d[0] : d[2];
        if (xb > xa) memcpy(dst + (size_t)py * stride + d[0] + xa, &g_wall_cache[(size_t)yy * d[2] + xa], (size_t)(xb - xa) * 4);
    }
}

/* ---- FACE2: THE DRAWING PAGE'S PAPER AND THE HELD FRAME -------------------------------------------------
   The paper is the ROM's own texture: the title's two paper sprite objects (ids 0x17 / 0x18, hal/menu_recipe_title.cpp skips them on
   the one-screen drawing page) show a 32 x 32 texel tile repeated across the lower screen, six colours (measured: every pixel of the
   cream page at full brightness not covered by a stroke or a button agrees with its tile cell, 0 conflicts over the 256 x 192 area).
   Here it is repeated edge to edge from the corner of the 256 x 192 area of the canvas and sampled with a wrapped bilinear filter, so it
   is smooth at any window size and has no side bars. Built once per size, copied per frame. */
const uint8_t k_paper_pal[6][3] = { { 255, 231, 206 }, { 255, 239, 206 }, { 255, 247, 214 }, { 255, 255, 231 }, { 255, 255, 239 }, { 255, 255, 247 } };
const char *const k_paper_tile[32] = {
    "01111111111111302211111311112110",
    "11131111111131113222143334111111",
    "34333333333333434433333433333443",
    "33345443435543335554433533444553",
    "43001112132100344342110301124344",
    "12111210112111211100110101100111",
    "55544434555445555555545450055555",
    "55103554155111555555543434055555",
    "13350011030001111111111111100101",
    "13145101133333314341343234314041",
    "05554554141455545544555555544500",
    "00443434444344433335333333353330",
    "13311112222223313201133333100231",
    "12212010012233200011110301111111",
    "44033333333334443114334543343434",
    "53332244445555555545555555554555",
    "12233333333333331433112321133421",
    "11110101102211111011121112111011",
    "41134333330333434442222455411444",
    "33333344433333334445434443454443",
    "12122201133333211114443111221011",
    "30031012100003033111111334333433",
    "33333333333333333441113434334433",
    "33334333443433332253333433345353",
    "33322223333333333222214141333333",
    "00111110111011033333233023331300",
    "13114333113433313344455455441111",
    "33331113333111445555455555433333",
    "41111013111331111111101110114444",
    "01111101044444101100111111100110",
    "33544453444444445555555555555553",
    "33433434433222223333344444333333"
};
const uint8_t k_paper_pal_g[6][3] = { { 231, 255, 181 }, { 231, 255, 189 }, { 231, 255, 206 }, { 247, 255, 239 }, { 255, 255, 247 }, { 255, 255, 255 } };   /* FACE3: the free-draw state's paper, measured off the ROM's settled render */
const char *const k_paper_tile_g[32] = {
    "11344533055233320555554441133245",
    "35553545032355555423142300434445",
    "55552223200454522350155534555535",
    "50001241344515522233445554334023",
    "30023334555543355413432354523333",
    "55555455422020034345553344203434",
    "35555333153335522345054453124555",
    "34013431235453334555330233544451",
    "55551544301433045531345324555201",
    "55553245554455533334555512354303",
    "32114320223220453355211345232455",
    "20345531023435351532545455553443",
    "44133535544553333445302442003155",
    "34232555545230545355534333455555",
    "54543555333123345310454455555545",
    "24555122354345535520211420133110",
    "00555333554454133433224333445442",
    "54310455425555434555454555543454",
    "44434334133312555354433555302321",
    "24555434454420235124543242354455",
    "22033555555555313554410214554355",
    "33121440125313131454312220133030",
    "55541100001003445445455551335533",
    "55555555255543435531552445542522",
    "20155555254555542320121332334233",
    "32101010110355555455555553332555",
    "35443213345333323355455544325555",
    "13454534555204533101121245433355",
    "25443235443402124542555333353002",
    "13445545043424431153334243333333",
    "44535353455555555555534355555545",
    "54313134555552134555555553333333"
};
std::vector<uint32_t> g_paper_cache_a[2];
int g_paper_key_a[2][4];

inline int paper_ci(char c) { return c <= '9' ? c - '0' : c - 'a' + 10; }

void draw_paper(const MenuLayout *L, int i, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    if (d[2] <= 0 || d[3] <= 0) return;
    const int gi = P.paper == 2 ? 1 : 0;
    std::vector<uint32_t> &g_paper_cache = g_paper_cache_a[gi];
    int *g_paper_key = g_paper_key_a[gi];
    const char *const *tile = gi ? k_paper_tile_g : k_paper_tile;
    const uint8_t (*pal)[3] = gi ? k_paper_pal_g : k_paper_pal;
    const int key[4] = { d[2], d[3], L->unum, L->uden };
    if (g_paper_cache.size() != (size_t)d[2] * d[3] || memcmp(key, g_paper_key, sizeof key) != 0) {
        memcpy(g_paper_key, key, sizeof key);
        g_paper_cache.assign((size_t)d[2] * d[3], 0xffffffffu);
        const float s = (float)L->unum / (float)L->uden;                    /* host pixels per DS pixel */
        const float ox = ((float)L->w - 256.f * s) * 0.5f;                  /* the 256 x 192 area's corner, so 4:3 lines up with the ROM's own paper */
        for (int y = 0; y < d[3]; ++y) {
            const float v = ((float)(y + d[1]) + 0.5f) / s - 0.5f;
            const int v0 = (int)floorf(v);
            const float fv = v - (float)v0;
            const int ya = ((v0 % 32) + 32) % 32, yb = (ya + 1) % 32;
            for (int x = 0; x < d[2]; ++x) {
                const float u = (((float)(x + d[0]) + 0.5f) - ox) / s - 0.5f;
                const int u0 = (int)floorf(u);
                const float fu = u - (float)u0;
                const int xa = ((u0 % 32) + 32) % 32, xb = (xa + 1) % 32;
                const int c00 = paper_ci(tile[ya][xa]), c10 = paper_ci(tile[ya][xb]);
                const int c01 = paper_ci(tile[yb][xa]), c11 = paper_ci(tile[yb][xb]);
                int ch[3];
                for (int k = 0; k < 3; ++k) {
                    const float t0 = pal[c00][k] + (pal[c10][k] - pal[c00][k]) * fu;
                    const float t1 = pal[c01][k] + (pal[c11][k] - pal[c01][k]) * fu;
                    ch[k] = (int)(t0 + (t1 - t0) * fv + 0.5f);
                }
                g_paper_cache[(size_t)y * d[2] + x] = 0xff000000u | ((uint32_t)ch[0] << 16) | ((uint32_t)ch[1] << 8) | (uint32_t)ch[2];
            }
        }
    }
    const size_t stride = (size_t)stride_of(w);
    const unsigned pa = P.alpha ? P.alpha : 255u, ia = 255u - pa;
    const int xa = d[0] < 0 ? -d[0] : 0, xb = d[0] + d[2] > w ? w - d[0] : d[2];
    for (int yy = 0; yy < d[3]; ++yy) {
        const int py = d[1] + yy;
        if (py < 0 || py >= h || xb <= xa) continue;
        uint32_t *row = dst + (size_t)py * stride + d[0];
        const uint32_t *sr = &g_paper_cache[(size_t)yy * d[2]];
        if (pa == 255u) { memcpy(row + xa, sr + xa, (size_t)(xb - xa) * 4); continue; }
        for (int xx = xa; xx < xb; ++xx) {
            const uint32_t c = sr[xx], o = row[xx];
            const unsigned r = (((c >> 16) & 255) * pa + ((o >> 16) & 255) * ia + 127) / 255, g = (((c >> 8) & 255) * pa + ((o >> 8) & 255) * ia + 127) / 255,
                           b = ((c & 255) * pa + (o & 255) * ia + 127) / 255;
            row[xx] = 0xff000000u | (r << 16) | (g << 8) | b;
        }
    }
}

/* the held frame: the composed picture as of the last frame that asked for it (MenuLayout.snap_save), draw thread only */
std::vector<uint32_t> g_snap;
int g_snap_w, g_snap_h;
/* MENUINTERP2: the picture a re-draw between ticks (hal_menu_present_relift) draws its held-frame pieces from is the one THIS tick
   drew them from, i.e. the held frame as it stood before this tick's own save. g_snap_old is that, kept when the tick saves. */
std::vector<uint32_t> g_snap_old;
int g_snap_old_valid, g_in_relift;
static double g_relift_alpha = 1.0;      /* ARROWSLOW1: the place between ticks of the pictures hal_menu_present_relift draws next (a bob piece's dest y follows it) */

void snap_save(const uint32_t *dst, int w, int h)
{
    if (g_in_relift) return;                 /* a picture between ticks never rewrites the held frame */
    const size_t stride = (size_t)stride_of(w);
    g_snap_old.swap(g_snap);
    g_snap_old_valid = !g_snap_old.empty();
    g_snap.resize((size_t)w * h);
    for (int y = 0; y < h; ++y) memcpy(&g_snap[(size_t)y * w], dst + (size_t)y * stride, (size_t)w * 4);
    g_snap_w = w; g_snap_h = h;
}

void draw_snap(const MenuLayout *L, int i, uint32_t *dst, int w, int h)
{
    if (g_snap_w != w || g_snap_h != h || g_snap.empty()) return;     /* the window changed size: no held frame, the page just appears */
    const MenuPiece &P = L->p[i];
    const size_t stride = (size_t)stride_of(w);
    const unsigned pa = P.alpha ? P.alpha : 255u, ia = 255u - pa;
    const std::vector<uint32_t> &held = (g_in_relift && g_snap_old_valid && g_snap_old.size() == g_snap.size()) ? g_snap_old : g_snap;
    for (int y = 0; y < h; ++y) {
        uint32_t *row = dst + (size_t)y * stride;
        const uint32_t *sr = &held[(size_t)y * w];
        if (pa == 255u) { memcpy(row, sr, (size_t)w * 4); continue; }
        for (int x = 0; x < w; ++x) {
            const uint32_t c = sr[x], o = row[x];
            const unsigned r = (((c >> 16) & 255) * pa + ((o >> 16) & 255) * ia + 127) / 255, g = (((c >> 8) & 255) * pa + ((o >> 8) & 255) * ia + 127) / 255,
                           b = ((c & 255) * pa + (o & 255) * ia + 127) / 255;
            row[x] = 0xff000000u | (r << 16) | (g << 8) | b;
        }
    }
}

/* ---- ASSET5: THE STAR IRIS ------------------------------------------------------------------------------
   Black outside a five-pointed star, the title iris. The ROM draws it as a 3D sprite whose edge carries a dark seam; this is the same shape
   (measured off the ROM frames: outer / inner vertex ratio 0.5306, one point straight up, the star centre a little below its pivot because
   the sprite geometry sits off its origin, fitted to 1.7 percent edge error) drawn at the window's own resolution with a clean
   anti-aliased edge: 4 sub-rows per pixel row, exact horizontal span coverage. Pixels inside keep what is under them. */
void draw_iris(const MenuLayout *L, int i, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    const size_t stride = (size_t)stride_of(w);
    const float R = P.ir16 / 16.f;
    if (R <= 0.f) {   /* nothing open yet: all black, or (alpha 1..254) a black veil of that opacity (the fade in of a returning page) */
        const unsigned k = P.alpha && P.alpha < 255 ? 255u - P.alpha : 0u;
        for (int y = 0; y < h; ++y) {
            uint32_t *row = dst + (size_t)y * stride;
            if (!k) { for (int x = 0; x < w; ++x) row[x] = 0xff000000u; continue; }
            for (int x = 0; x < w; ++x) {
                const uint32_t o = row[x];
                row[x] = 0xff000000u | ((((o >> 16) & 255) * k / 255) << 16) | ((((o >> 8) & 255) * k / 255) << 8) | ((o & 255) * k / 255);
            }
        }
        return;
    }
    const float rho = 0.5306f;
    float vx[10], vy[10];
    for (int k = 0; k < 10; ++k) {
        const float a = (-90.f + 36.f * k) * 3.14159265f / 180.f, rad = (k & 1) ? R * rho : R;
        vx[k] = P.icx + rad * cosf(a);
        vy[k] = P.icy + rad * sinf(a);
    }
    float ymin = vy[0], ymax = vy[0];
    for (int k = 1; k < 10; ++k) { if (vy[k] < ymin) ymin = vy[k]; if (vy[k] > ymax) ymax = vy[k]; }
    static std::vector<float> cov;
    if ((int)cov.size() < w) cov.resize((size_t)w);
    const int SS = 4;
    for (int y = 0; y < h; ++y) {
        uint32_t *row = dst + (size_t)y * stride;
        if (y + 1 <= ymin || y >= ymax) {       /* the whole row is outside the star */
            for (int x = 0; x < w; ++x) row[x] = 0xff000000u;
            continue;
        }
        const int fastc = cost_fast();
        int cmin = w, cmax = -1;
        if (!fastc) std::fill(cov.begin(), cov.begin() + w, 0.f);
        for (int s = 0; s < SS; ++s) {
            const float sy = y + (s + 0.5f) / SS;
            float xs[10]; int n = 0;
            for (int k = 0; k < 10; ++k) {
                const int k2 = (k + 1) % 10;
                const float y0 = vy[k], y1 = vy[k2];
                if ((y0 <= sy && y1 > sy) || (y1 <= sy && y0 > sy)) xs[n++] = vx[k] + (sy - y0) * (vx[k2] - vx[k]) / (y1 - y0);
            }
            std::sort(xs, xs + n);
            for (int q = 0; q + 1 < n; q += 2) {
                float a = xs[q], b = xs[q + 1];
                if (b <= 0.f || a >= (float)w) continue;
                if (a < 0.f) a = 0.f;
                if (b > (float)w) b = (float)w;
                const int ia = (int)a, ib = (int)b;
                if (ia < cmin) cmin = ia;
                if ((ib < w ? ib : w - 1) > cmax) cmax = ib < w ? ib : w - 1;
                if (ia == ib) { cov[ia] += (b - a) / SS; continue; }
                cov[ia] += ((ia + 1) - a) / SS;
                for (int x = ia + 1; x < ib; ++x) cov[x] += 1.f / SS;
                if (ib < w) cov[ib] += (b - ib) / SS;
            }
        }
        if (fastc) {
            if (cmax < 0) { std::fill(row, row + w, 0xff000000u); continue; }
            std::fill(row, row + cmin, 0xff000000u);
            std::fill(row + cmax + 1, row + w, 0xff000000u);
        }
        const int xlo = fastc ? cmin : 0, xhi = fastc ? cmax + 1 : w;
        for (int x = xlo; x < xhi; ++x) {
            const float c = cov[x];
            if (fastc) cov[x] = 0.f;
            if (c >= 0.999f) continue;
            if (c <= 0.001f) { row[x] = 0xff000000u; continue; }
            const uint32_t o = row[x];
            const unsigned k = (unsigned)(c * 256.f + 0.5f);
            row[x] = 0xff000000u | ((((o >> 16) & 255) * k >> 8) << 16) | ((((o >> 8) & 255) * k >> 8) << 8) | ((o & 255) * k >> 8);
        }
    }
}
}  // namespace

/* the star's own geometry, shared by the draw and the coverage test: outer vertex radius R, inner vertex radius R * rho, one point straight up */
static int iris_star_inside(float px, float py, float cx, float cy, float R)
{
    const float rho = 0.5306f, pi = 3.14159265f;
    const float dx = px - cx, dy = py - cy;
    const float r = sqrtf(dx * dx + dy * dy);
    if (r < 1e-3f) return 1;
    float phi = atan2f(dx, -dy);                       /* 0 = up, clockwise */
    const float sector = 2.f * pi / 5.f;
    phi = fmodf(phi + 20.f * pi, sector);
    if (phi > sector * 0.5f) phi = sector - phi;       /* 0 .. 36 degrees from the nearest point's axis */
    const float p0x = R, p0y = 0.f, p1x = rho * R * cosf(sector * 0.5f), p1y = rho * R * sinf(sector * 0.5f);
    const float ux = cosf(phi), uy = sinf(phi), ex = p1x - p0x, ey = p1y - p0y;
    const float den = ux * ey - uy * ex;
    if (den > -1e-6f && den < 1e-6f) return 1;
    return r <= (p0x * ey - p0y * ex) / den;
}

extern "C++" int menu_iris_covers(int w, int h, float cx, float cy, float R)
{
    if (R <= 0.f) return 0;
    for (int x = 0; x <= w; x += 4) if (!iris_star_inside((float)x, 0.f, cx, cy, R) || !iris_star_inside((float)x, (float)h, cx, cy, R)) return 0;
    for (int y = 0; y <= h; y += 4) if (!iris_star_inside(0.f, (float)y, cx, cy, R) || !iris_star_inside((float)w, (float)y, cx, cy, R)) return 0;
    return iris_star_inside((float)w, (float)h, cx, cy, R) && iris_star_inside(0.f, 0.f, cx, cy, R);
}

/* HDART1: the game over gradient is a host picture, so the ROM's fade-in (engine A's brightness on its BG2 gradient) is read from the
   lifted gradient itself: the lifted cell mid-screen in the clear column, brightness applied, over the brightest seen on this page. */
static void go_fade_update(const MenuSources *S)
{
    if (!S || !S->top2d) { g_go_fade = 1.f; return; }
    const MenuTopCell &c = S->top2d[96 * 256 + 16];
    if ((c.argb >> 24) == 0 || c.owner != 2) return;      /* BG2 not lifted this frame: keep the last factor */
    uint32_t col = c.argb;
    if (S->bm_apply) col = S->bm_apply(col, c.bm);
    const float lum = 0.3f * ((col >> 16) & 255) + 0.59f * ((col >> 8) & 255) + 0.11f * (col & 255);
    if ((int)g_draw_frame - g_go_frame > 30) g_go_ref = 0.f;
    g_go_frame = (int)g_draw_frame;
    if (lum > g_go_ref) g_go_ref = lum;
    g_go_fade = lum / (g_go_ref > 96.f ? g_go_ref : 96.f);   /* the DS gradient peaks at about 100: dark first frames read dark, never a full-bright flash */
    if (g_go_fade > 1.f) g_go_fade = 1.f;
    if (g_go_fade < 0.f) g_go_fade = 0.f;
}

static void piece_draw_ref(const MenuLayout *L, int i, const MenuSources *S, uint32_t *dst, int w, int h)
{
    if (!S || !dst || i < 0 || i >= L->np) return;
    const MenuPiece &P = L->p[i];
    if (P.nodraw) return;
    if (P.src == SRC_HOSTIMG && P.wall) { draw_wall(L, i, dst, w, h); return; }
    if (P.src == SRC_HOSTIMG && P.iris) { draw_iris(L, i, dst, w, h); return; }
    if (P.src == SRC_HOSTIMG && P.paper) { draw_paper(L, i, dst, w, h); return; }
    if (P.src == SRC_HOSTIMG && P.snap) { draw_snap(L, i, dst, w, h); return; }
    if (P.src == SRC_HOSTIMG && (P.hostimg == MENU_IMG_GO_GRAD || P.hostimg == MENU_IMG_GO_CLICK || (P.hostimg >= MENU_IMG_GO_L0 && P.hostimg <= MENU_IMG_GO_L7))) go_fade_update(S);
    if (P.src == SRC_HOSTIMG) { draw_hostimg(L, i, dst, w, h); return; }
    if (P.hostgrad) { draw_hostgrad(L, i, S, dst, w, h); return; }
    const int *d = L->dest[i];
    const int x0 = d[0], y0 = d[1], dw = d[2], dh = d[3];
    if (dw <= 0 || dh <= 0) return;
    const size_t stride = (size_t)stride_of(w);
    const uint32_t *subp = P.src == SRC_SUB ? sub_plane(&P, S) : 0;
    /* the visible column range, and one source index per column, computed once per piece */
    const int xa = x0 < 0 ? -x0 : 0;
    const int xb = (x0 + dw > w) ? w - x0 : dw;
    if (xa >= xb) return;
    static std::vector<int> colx;
    if ((int)colx.size() < dw) colx.resize((size_t)dw);
    int *cx = &colx[0];
    if (P.src == SRC_TOP_PIC) {
        if (!S->pic || S->pic_rw < 1 || S->pic_rh < 1) return;
        for (int xx = xa; xx < xb; ++xx) {
            /* exact rational map of the destination pixel into the host picture */
            const long long ux = (long long)P.s.x * dw + (long long)xx * P.s.w;
            const int qx = S->pic_rx + (int)fdiv(ux * S->pic_rw, 256LL * dw);
            cx[xx] = (qx < 0 || qx >= S->pic_w) ? -1 : qx;
        }
    } else if (P.src != SRC_TOP_HOSTROW) {
        const int lim = 256;
        for (int xx = xa; xx < xb; ++xx) {
            int sx = P.xfill ? P.s.x : P.lden > 0 ? P.s.x + (int)fdiv((long long)xx * P.lden, P.lnum > 0 ? P.lnum : 1)
                                : P.s.x + (int)fdiv((long long)xx * P.s.w, dw);
            if (P.lden > 0 && sx > P.s.x + P.s.w - 1) sx = P.s.x + P.s.w - 1;
            cx[xx] = (sx < 0 || sx >= lim) ? -1 : sx;
        }
    }
    const uint32_t bd = S->sub_backdrop & 0xffffffu;
    for (int yy = 0; yy < dh; ++yy) {
        const int py = y0 + yy;
        if (py < 0 || py >= h) continue;
        uint32_t *row = dst + (size_t)py * stride + x0;
        if (P.src == SRC_TOP_HOSTROW) {
            if (!S->hostrow) return;
            const int sy = P.s.y + yy;
            if (sy < 0 || sy >= S->hostrow_h) continue;
            for (int xx = xa; xx < xb; ++xx) {
                const int sx = P.s.x + xx;
                if (sx < 0 || sx >= S->hostrow_w) continue;
                const uint32_t c = S->hostrow[(size_t)sy * S->hostrow_stride + sx];
                if (c >> 24) row[xx] = c | 0xff000000u;
            }
            continue;
        }
        if (P.src == SRC_TOP_PIC) {
            const long long uy = (long long)P.s.y * dh + (long long)yy * P.s.h;
            const int qy = S->pic_ry + (int)fdiv(uy * S->pic_rh, 192LL * dh);
            if (qy < 0 || qy >= S->pic_h) continue;
            const uint32_t *prow = S->pic + (size_t)qy * S->pic_stride;
            for (int xx = xa; xx < xb; ++xx) {
                const int qx = cx[xx];
                if (qx < 0) continue;
                if (P.keycover) {
                    if (!S->pic_cover || !S->pic_cover[(size_t)qy * S->pic_stride + qx]) continue;
                    if (P.has_ex) {
                        const int ex = (qx - S->pic_rx) * 256 / S->pic_rw, ey = (qy - S->pic_ry) * 192 / S->pic_rh;
                        if (ex >= P.ex.x && ex < P.ex.x + P.ex.w && ey >= P.ex.y && ey < P.ex.y + P.ex.h) continue;
                        if (P.has_ex2 && ex >= P.ex2.x && ex < P.ex2.x + P.ex2.w && ey >= P.ex2.y && ey < P.ex2.y + P.ex2.h) continue;
                        if (P.has_ex3 && ex >= P.ex3.x && ex < P.ex3.x + P.ex3.w && ey >= P.ex3.y && ey < P.ex3.y + P.ex3.h) continue;
                    }
                }
                if (P.alpha && P.alpha < 255) {   /* ASSET5: a faded piece (the face fade-in) blends over what is there */
                    const uint32_t c = prow[qx], o = row[xx];
                    const unsigned pa = P.alpha, ia = 255 - pa;
                    const unsigned r = (((c >> 16) & 255) * pa + ((o >> 16) & 255) * ia + 127) / 255, g = (((c >> 8) & 255) * pa + ((o >> 8) & 255) * ia + 127) / 255,
                                   b = ((c & 255) * pa + (o & 255) * ia + 127) / 255;
                    row[xx] = 0xff000000u | (r << 16) | (g << 8) | b;
                    continue;
                }
                /* INT2: key_backdrop on a picture piece turns the ROM 3D clear colour (0x101820 the dark navy of the star flight, 0x080808 the attract's near-black) to pure black */
                if (P.key_backdrop && clear_key(prow[qx])) { row[xx] = 0xff000000u; continue; }
                row[xx] = prow[qx] | 0xff000000u;
            }
            continue;
        }
        int sy = P.lden > 0 ? P.s.y + (int)fdiv((long long)yy * P.lden, P.lnum > 0 ? P.lnum : 1)
                            : P.s.y + (int)fdiv((long long)yy * P.s.h, dh);
        if (P.lden > 0 && sy > P.s.y + P.s.h - 1) sy = P.s.y + P.s.h - 1;
        if (P.row_ok && !P.row_ok(sy)) continue;
        if (sy < 0 || sy >= 192) continue;
        if (P.src == SRC_SUB) {
            if (!subp) continue;
            const uint32_t *srow = subp + sy * 256;
            for (int xx = xa; xx < xb; ++xx) {
                const int sx = cx[xx];
                if (sx < 0) continue;
                const uint32_t c = srow[sx];
                if (P.key_backdrop && (c & 0xffffffu) == bd) continue;
                row[xx] = c | 0xff000000u;
            }
        } else {   /* SRC_TOP_2D */
            if (!S->top2d) continue;
            const MenuTopCell *trow = S->top2d + sy * 256;
            for (int xx = xa; xx < xb; ++xx) {
                const int sx = cx[xx];
                if (sx < 0) continue;
                const MenuTopCell &c = trow[sx];
                if ((c.argb >> 24) == 0) continue;
                if (c.owner > 4 || !((P.layers >> c.owner) & 1)) continue;
                uint32_t col = c.argb;
                if (S->bm_apply) col = S->bm_apply(col, c.bm);
                row[xx] = col | 0xff000000u;
            }
        }
    }
}

/* ---- MENUCOST1: THE CHEAPER DRAW OF A COPIED PIECE ----------------------------------------------------------------------------
   The reference loop (piece_draw_ref) does per DEST pixel what only depends on the SOURCE pixel: the key test, the layer test, the
   engine-A brightness verdict. A menu is mostly upscaled DS art (4x and more), so that work repeats a dozen times per source pixel.
   Here a source row is converted once into `crow` (one ARGB word per source column, 0 = not drawn), a dest row is a gather from it,
   and a dest row whose source row is the one above (and which that row filled completely) is a copy of the row above. The words
   written are the ones the reference writes (proved by picture hash; SM64DS_MENU_COST_OFF=1 runs the reference). A piece identical to
   the one drawn just before it (the Rec Room's star pattern tiles) is copied from it when that one filled its whole rect. */
struct RepState {
    int enable, valid;
    uint32_t *dst; int w, h; size_t stride;
    MenuSrc src; uint8_t layers, key_backdrop, xfill, oam_on, oam_lo, oam_hi; DsRect s; short fill_w; int lnum, lden; int (*row_ok)(int);
    int x0, y0, dw, dh;
} g_rep;

static int rep_same(const RepState &a, const MenuPiece &P, int dw, int dh)
{
    return a.src == P.src && a.layers == P.layers && a.key_backdrop == P.key_backdrop && a.xfill == P.xfill && a.oam_on == P.oam_on &&
           a.oam_lo == P.oam_lo && a.oam_hi == P.oam_hi && a.s.x == P.s.x && a.s.y == P.s.y && a.s.w == P.s.w && a.s.h == P.s.h &&
           a.fill_w == P.fill_w && a.lnum == P.lnum && a.lden == P.lden && a.row_ok == P.row_ok && a.dw == dw && a.dh == dh;
}

static void piece_draw_2d(const MenuLayout *L, int i, const MenuSources *S, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    const int x0 = d[0], y0 = d[1], dw = d[2], dh = d[3];
    const int prev_ok = g_rep.enable && g_rep.valid;
    g_rep.valid = 0;
    if (dw <= 0 || dh <= 0) return;
    const size_t stride = (size_t)stride_of(w);
    const uint32_t *subp = P.src == SRC_SUB ? sub_plane(&P, S) : 0;
    if (P.src == SRC_SUB && !subp) return;
    if (P.src == SRC_TOP_2D && !S->top2d) return;
    const int xa = x0 < 0 ? -x0 : 0;
    const int xb = (x0 + dw > w) ? w - x0 : dw;
    if (xa >= xb) return;
    static std::vector<int> colx;
    if ((int)colx.size() < dw) colx.resize((size_t)dw);
    int *cx = &colx[0];
    unsigned char used[257];
    memset(used, 0, sizeof used);
    for (int xx = xa; xx < xb; ++xx) {
        int sx = P.xfill ? P.s.x : P.lden > 0 ? P.s.x + (int)fdiv((long long)xx * P.lden, P.lnum > 0 ? P.lnum : 1)
                                    : P.s.x + (int)fdiv((long long)xx * P.s.w, dw);
        if (P.lden > 0 && sx > P.s.x + P.s.w - 1) sx = P.s.x + P.s.w - 1;
        cx[xx] = (sx < 0 || sx >= 256) ? 256 : sx;      /* 256 = the column that is never drawn */
        used[cx[xx]] = 1;
    }
    unsigned short ulist[257];
    int nu = 0;
    for (int k = 0; k < 257; ++k) if (used[k]) ulist[nu++] = (unsigned short)k;
    const int clipped = (xa != 0 || xb != dw || y0 < 0 || y0 + dh > h);
    /* the previous piece of this frame was the same art over a full rect: copy it */
    if (prev_ok && g_rep.dst == dst && g_rep.w == w && g_rep.h == h && g_rep.stride == stride && P.src != SRC_TOP_2D &&
        rep_same(g_rep, P, dw, dh) && g_rep.y0 == y0 && !(y0 < 0 || y0 + dh > h)) {
        for (int yy = 0; yy < dh; ++yy) {
            const uint32_t *from = dst + (size_t)(y0 + yy) * stride + g_rep.x0;
            memmove(dst + (size_t)(y0 + yy) * stride + x0 + xa, from + xa, (size_t)(xb - xa) * 4);
        }
        if (!clipped) { g_rep.x0 = x0; g_rep.valid = 1; }
        return;
    }
    /* MENUS093E: a masked plane drawn over the sentinel backdrop is keyed by the sentinel; a piece that does not key carries the real backdrop in its place */
    const bool msk = P.src == SRC_SUB && S->bd_masked_on && subp != S->sub;
    const uint32_t bd = (msk ? S->sub_backdrop_masked : S->sub_backdrop) & 0xffffffu;
    const uint32_t rbd = S->sub_backdrop | 0xff000000u;
    uint32_t crow[257];
    crow[256] = 0;
    /* the runs of a source row (dest column ranges whose source column is drawn): a row of the same source row as the row above is those
       runs copied from the row above, whatever is transparent in between is left alone, as the reference leaves it */
    static std::vector<int> runs;
    int last_sy = -1000, last_full = 0, last_py = -1000, all_rows = 1, ever_full = 1;
    for (int yy = 0; yy < dh; ++yy) {
        const int py = y0 + yy;
        if (py < 0 || py >= h) { all_rows = 0; continue; }
        uint32_t *row = dst + (size_t)py * stride + x0;
        int sy = P.lden > 0 ? P.s.y + (int)fdiv((long long)yy * P.lden, P.lnum > 0 ? P.lnum : 1)
                            : P.s.y + (int)fdiv((long long)yy * P.s.h, dh);
        if (P.lden > 0 && sy > P.s.y + P.s.h - 1) sy = P.s.y + P.s.h - 1;
        if ((P.row_ok && !P.row_ok(sy)) || sy < 0 || sy >= 192) { all_rows = 0; continue; }
        if (sy != last_sy) {
            if (P.src == SRC_SUB) {
                const uint32_t *srow = subp + sy * 256;
                if (P.key_backdrop) for (int sx = 0; sx < 256; ++sx) { const uint32_t c = srow[sx]; crow[sx] = (c & 0xffffffu) == bd ? 0u : (c | 0xff000000u); }
                else if (msk) for (int sx = 0; sx < 256; ++sx) { const uint32_t c = srow[sx]; crow[sx] = (c & 0xffffffu) == bd ? rbd : (c | 0xff000000u); }
                else for (int sx = 0; sx < 256; ++sx) crow[sx] = srow[sx] | 0xff000000u;
            } else {
                const MenuTopCell *trow = S->top2d + sy * 256;
                for (int sx = 0; sx < 256; ++sx) {
                    const MenuTopCell &c = trow[sx];
                    if ((c.argb >> 24) == 0 || c.owner > 4 || !((P.layers >> c.owner) & 1)) { crow[sx] = 0; continue; }
                    uint32_t col = c.argb;
                    if (S->bm_apply) col = S->bm_apply(col, c.bm);
                    crow[sx] = col | 0xff000000u;
                }
            }
            last_full = 1;
            for (int k = 0; k < nu; ++k) if (!crow[ulist[k]]) { last_full = 0; break; }
            if (!last_full) ever_full = 0;
            runs.clear();
            if (!P.xfill) {
                if (last_full) { runs.push_back(xa); runs.push_back(xb); }
                else {
                    int xx = xa;
                    while (xx < xb) {
                        while (xx < xb && !crow[cx[xx]]) ++xx;
                        const int a = xx;
                        while (xx < xb && crow[cx[xx]]) ++xx;
                        if (xx > a) { runs.push_back(a); runs.push_back(xx); }
                    }
                }
            }
        }
        if (P.xfill) {
            const uint32_t v = crow[cx[xa]];
            if (v) std::fill(row + xa, row + xb, v);
        } else if (sy == last_sy && last_py == py - 1) {      /* the row above was this source row: copy its drawn runs */
            for (size_t k = 0; k + 1 < runs.size(); k += 2) memcpy(row + runs[k], row - stride + runs[k], (size_t)(runs[k + 1] - runs[k]) * 4);
        } else {
            for (size_t k = 0; k + 1 < runs.size(); k += 2)
                for (int xx = runs[k]; xx < runs[k + 1]; ++xx) row[xx] = crow[cx[xx]];
        }
        last_sy = sy;
        last_py = py;
    }
    if (g_rep.enable && !clipped && all_rows && ever_full) {
        g_rep.dst = dst; g_rep.w = w; g_rep.h = h; g_rep.stride = stride;
        g_rep.src = P.src; g_rep.layers = P.layers; g_rep.key_backdrop = P.key_backdrop; g_rep.xfill = P.xfill; g_rep.oam_on = P.oam_on;
        g_rep.oam_lo = P.oam_lo; g_rep.oam_hi = P.oam_hi; g_rep.s = P.s; g_rep.fill_w = P.fill_w; g_rep.lnum = P.lnum; g_rep.lden = P.lden;
        g_rep.row_ok = P.row_ok; g_rep.x0 = x0; g_rep.y0 = y0; g_rep.dw = dw; g_rep.dh = dh;
        g_rep.valid = 1;
    }
}

/* a keycover / plain piece of the 3D picture (the title's head, the wrench): the per-pixel integer divides of the exclusion rects
   are done once per column and once per row */
static void piece_draw_pic_ref(const MenuLayout *L, int i, const MenuSources *S, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    const int *d = L->dest[i];
    const int x0 = d[0], y0 = d[1], dw = d[2], dh = d[3];
    g_rep.valid = 0;
    if (dw <= 0 || dh <= 0) return;
    const size_t stride = (size_t)stride_of(w);
    const int xa = x0 < 0 ? -x0 : 0;
    const int xb = (x0 + dw > w) ? w - x0 : dw;
    if (xa >= xb) return;
    if (!S->pic || S->pic_rw < 1 || S->pic_rh < 1) return;
    static std::vector<int> colx, colex;
    if ((int)colx.size() < dw) { colx.resize((size_t)dw); colex.resize((size_t)dw); }
    int *cx = &colx[0], *exc = &colex[0];
    const int use_ex = P.keycover && P.has_ex;
    for (int xx = xa; xx < xb; ++xx) {
        const long long ux = (long long)P.s.x * dw + (long long)xx * P.s.w;
        const int qx = S->pic_rx + (int)fdiv(ux * S->pic_rw, 256LL * dw);
        cx[xx] = (qx < 0 || qx >= S->pic_w) ? -1 : qx;
        if (use_ex && cx[xx] >= 0) exc[xx] = (cx[xx] - S->pic_rx) * 256 / S->pic_rw;
    }
    for (int yy = 0; yy < dh; ++yy) {
        const int py = y0 + yy;
        if (py < 0 || py >= h) continue;
        uint32_t *row = dst + (size_t)py * stride + x0;
        const long long uy = (long long)P.s.y * dh + (long long)yy * P.s.h;
        const int qy = S->pic_ry + (int)fdiv(uy * S->pic_rh, 192LL * dh);
        if (qy < 0 || qy >= S->pic_h) continue;
        const uint32_t *prow = S->pic + (size_t)qy * S->pic_stride;
        const uint8_t *cov = 0;
        int in1 = 0, in2 = 0, in3 = 0;
        if (P.keycover) {
            if (!S->pic_cover) continue;
            cov = S->pic_cover + (size_t)qy * S->pic_stride;
            if (P.has_ex) {
                const int ey = (qy - S->pic_ry) * 192 / S->pic_rh;
                in1 = ey >= P.ex.y && ey < P.ex.y + P.ex.h;
                in2 = P.has_ex2 && ey >= P.ex2.y && ey < P.ex2.y + P.ex2.h;
                in3 = P.has_ex3 && ey >= P.ex3.y && ey < P.ex3.y + P.ex3.h;
            }
        }
        for (int xx = xa; xx < xb; ++xx) {
            const int qx = cx[xx];
            if (qx < 0) continue;
            if (cov) {
                if (!cov[qx]) continue;
                if (in1 && exc[xx] >= P.ex.x && exc[xx] < P.ex.x + P.ex.w) continue;
                if (in2 && exc[xx] >= P.ex2.x && exc[xx] < P.ex2.x + P.ex2.w) continue;
                if (in3 && exc[xx] >= P.ex3.x && exc[xx] < P.ex3.x + P.ex3.w) continue;
            }
            const uint32_t c = prow[qx];
            if (P.key_backdrop && clear_key(c)) { row[xx] = 0xff000000u; continue; }
            row[xx] = c | 0xff000000u;
        }
    }
}

/* OPTIM1: the same piece, drawn faster. Same pixels: SM64DS_MENU_PIC_FAST=0 runs the old loop above, SM64DS_MENU_PIC_CHECK=1 draws both from
   the same starting picture and counts the pixels that differ. Each source row is gathered once through the column table into a scratch row
   (with its cover bytes), which the next destination row reuses when it maps to the same source row (the title stretches the 3D by about 1.5);
   the keying, the cover test and the alpha are then done four pixels at a time. A row that meets an exclusion rect, and a piece with a column
   off the picture, keep the old per-pixel code. */
static int pic_fast(void)
{
    static int v = -1;
    if (v < 0) { const char *e = getenv("SM64DS_MENU_PIC_FAST"); v = (e && *e == '0') ? 0 : 1; }
    return v;
}

template <class F> static void pic_rows_tramp(void *c, int y0, int y1) { (*(F *)c)(y0, y1); }
static void piece_draw_pic(const MenuLayout *L, int i, const MenuSources *S, uint32_t *dst, int w, int h)
{
    const MenuPiece &P = L->p[i];
    if (!pic_fast() || !S->pic || S->pic_rw < 1 || S->pic_rh < 1) { piece_draw_pic_ref(L, i, S, dst, w, h); return; }
    const int *d = L->dest[i];
    const int x0 = d[0], y0 = d[1], dw = d[2], dh = d[3];
    if (dw <= 0 || dh <= 0) { piece_draw_pic_ref(L, i, S, dst, w, h); return; }
    const size_t stride = (size_t)stride_of(w);
    const int xa = x0 < 0 ? -x0 : 0;
    const int xb = (x0 + dw > w) ? w - x0 : dw;
    if (xa >= xb) { piece_draw_pic_ref(L, i, S, dst, w, h); return; }
    static std::vector<int> colx, colex;
    static std::vector<uint32_t> scratch;
    static std::vector<uint8_t> covscratch;
    if ((int)colx.size() < dw) { colx.resize((size_t)dw); colex.resize((size_t)dw); }
    if ((int)scratch.size() < dw + 8) { scratch.resize((size_t)dw + 8); covscratch.resize((size_t)dw + 8); }
    int *cx = &colx[0], *exc = &colex[0];
    uint32_t *tmp = &scratch[0];
    uint8_t *tcv = &covscratch[0];
    const int use_ex = P.keycover && P.has_ex;
    for (int xx = xa; xx < xb; ++xx) {
        const long long ux = (long long)P.s.x * dw + (long long)xx * P.s.w;
        const int qx = S->pic_rx + (int)fdiv(ux * S->pic_rw, 256LL * dw);
        if (qx < 0 || qx >= S->pic_w) { piece_draw_pic_ref(L, i, S, dst, w, h); return; }   /* a column off the picture: the old loop skips it */
        cx[xx] = qx;
        if (use_ex) exc[xx] = (qx - S->pic_rx) * 256 / S->pic_rw;
    }
    g_rep.valid = 0;
    const bool key = P.key_backdrop != 0;
    const bool aa = key && ntr::gx_anti_aliasing() != 0;
    const __m128i amask = _mm_set1_epi32((int)0xff000000u), m24 = _mm_set1_epi32(0x00ffffff);
    const __m128i k1 = _mm_set1_epi32(0x101820), k2 = _mm_set1_epi32(0x080808), c20 = _mm_set1_epi8(20), zero = _mm_setzero_si128();
    const int n = xb - xa;
    /* a piece drawn 1:1 (cx counts up by one a column) reads the picture and the cover plane in place: no gather */
    const bool unit = cx[xb - 1] - cx[xa] == n - 1;
    const int q0 = cx[xa];
    /* MENU2: the rows are independent (each has its own source row, and the span and scratch row below are only a cache of the last
       source row), so whole rows go to the raster pool's threads, each band with its own scratch row. SM64DS_MENU_PAR2=0 keeps one. */
    /* the loop's invariants are copied to locals first: a capture by reference makes the compiler reload them around every store (measured 15 percent on one thread) */
    auto rows_job = [&, n_ = n, unit_ = unit, q0_ = q0, xa_ = xa, key_ = key, aa_ = aa, cx_ = cx, exc_ = exc, stride_ = stride, x0_ = x0, h_ = h, y0_ = y0, dh_ = dh, dst_ = dst, amask_ = amask, m24_ = m24, k1_ = k1, k2_ = k2, c20_ = c20, zero_ = zero](int ylo, int yhi) {
        const int n = n_;
        const bool unit = unit_;
        const int q0 = q0_;
        const int xa = xa_;
        const bool key = key_;
        const bool aa = aa_;
        int *const cx = cx_;
        int *const exc = exc_;
        const size_t stride = stride_;
        const int x0 = x0_;
        const int h = h_;
        const int y0 = y0_;
        const int dh = dh_;
        uint32_t *const dst = dst_;
        const __m128i amask = amask_;
        const __m128i m24 = m24_;
        const __m128i k1 = k1_;
        const __m128i k2 = k2_;
        const __m128i c20 = c20_;
        const __m128i zero = zero_;
        std::vector<uint32_t> tmpv((size_t)dw + 8);
        std::vector<uint8_t> tcvv((size_t)dw + 8);
        uint32_t *tmp = &tmpv[0];
        uint8_t *tcv = &tcvv[0];
        int have_qy = -2;    /* the source row the scratch row holds */
        int span_qy = -2, span_ks = 0, span_ke = 0;   /* the covered columns [ks, ke) of that source row, counted from xa */
        for (int yy = ylo; yy < yhi; ++yy) {
            const int py = y0 + yy;
            if (py < 0 || py >= h) continue;
            uint32_t *row = dst + (size_t)py * stride + x0;
            const long long uy = (long long)P.s.y * dh + (long long)yy * P.s.h;
            const int qy = S->pic_ry + (int)fdiv(uy * S->pic_rh, 192LL * dh);
            if (qy < 0 || qy >= S->pic_h) continue;
            const uint32_t *prow = S->pic + (size_t)qy * S->pic_stride;
            const uint8_t *cov = 0;
            int in1 = 0, in2 = 0, in3 = 0;
            if (P.keycover) {
                if (!S->pic_cover) continue;
                cov = S->pic_cover + (size_t)qy * S->pic_stride;
                if (P.has_ex) {
                    const int ey = (qy - S->pic_ry) * 192 / S->pic_rh;
                    in1 = ey >= P.ex.y && ey < P.ex.y + P.ex.h;
                    in2 = P.has_ex2 && ey >= P.ex2.y && ey < P.ex2.y + P.ex2.h;
                    in3 = P.has_ex3 && ey >= P.ex3.y && ey < P.ex3.y + P.ex3.h;
                }
            }
            if (in1 | in2 | in3) {     /* a row crossed by an exclusion rect: the old per-pixel code */
                for (int xx = xa; xx < xb; ++xx) {
                    const int qx = cx[xx];
                    if (!cov[qx]) continue;
                    if (in1 && exc[xx] >= P.ex.x && exc[xx] < P.ex.x + P.ex.w) continue;
                    if (in2 && exc[xx] >= P.ex2.x && exc[xx] < P.ex2.x + P.ex2.w) continue;
                    if (in3 && exc[xx] >= P.ex3.x && exc[xx] < P.ex3.x + P.ex3.w) continue;
                    const uint32_t c = prow[qx];
                    if (key && clear_key(c)) { row[xx] = 0xff000000u; continue; }
                    row[xx] = c | 0xff000000u;
                }
                continue;
            }
            uint32_t *o = row + xa;
            int k = 0;
            if (cov) {
                /* the covered span of this source row, found once on the cover plane itself (contiguous bytes), then mapped to destination columns:
                   every pixel outside it is uncovered and would be skipped anyway */
                if (span_qy != qy) {
                    span_qy = qy;
                    const int ql = cx[xa], qh = cx[xb - 1];
                    int lo = ql, hi = qh;
                    while (lo + 16 <= qh + 1 && _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_loadu_si128((const __m128i *)(cov + lo)), zero)) == 0xFFFF) lo += 16;
                    while (lo <= qh && !cov[lo]) ++lo;
                    while (hi - 15 >= lo && _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_loadu_si128((const __m128i *)(cov + hi - 15)), zero)) == 0xFFFF) hi -= 16;
                    while (hi >= lo && !cov[hi]) --hi;
                    if (lo > hi) { span_ks = 1; span_ke = 0; }
                    else {
                        int a2 = 0, b2 = n;                       /* first k with cx >= lo */
                        while (a2 < b2) { const int m = (a2 + b2) >> 1; if (cx[xa + m] >= lo) b2 = m; else a2 = m + 1; }
                        span_ks = a2;
                        a2 = 0; b2 = n;                           /* first k with cx > hi */
                        while (a2 < b2) { const int m = (a2 + b2) >> 1; if (cx[xa + m] > hi) b2 = m; else a2 = m + 1; }
                        span_ke = a2;
                    }
                }
                if (span_ks >= span_ke) continue;
                const int kg0 = span_ks & ~3, kg1 = (span_ke + 3 < n) ? ((span_ke + 3) & ~3) : n;
                const uint8_t *tc = tcv;
                if (unit) tc = cov + q0;
                else if (have_qy != qy) { for (int q = kg0; q < kg1; ++q) tcv[q] = cov[cx[xa + q]]; have_qy = qy; }
                have_qy = qy;
                k = kg0;
                for (; k + 4 <= kg1; k += 4) {
                    if (k + 16 <= kg1 && (k & 15) == 0) {          /* sixteen cover bytes at once: nothing covered, nothing to do */
                        const __m128i cb16 = _mm_loadu_si128((const __m128i *)(tc + k));
                        if (_mm_movemask_epi8(_mm_cmpeq_epi8(cb16, zero)) == 0xFFFF) { k += 12; continue; }
                    }
                    int cw;
                    memcpy(&cw, tc + k, 4);
                    if (cw == 0) continue;
                    const __m128i c = unit ? _mm_loadu_si128((const __m128i *)(prow + q0 + k))
                                           : _mm_set_epi32((int)prow[cx[xa + k + 3]], (int)prow[cx[xa + k + 2]], (int)prow[cx[xa + k + 1]], (int)prow[cx[xa + k]]);
                    __m128i r = _mm_or_si128(c, amask);
                    if (key) {
                        const __m128i q = _mm_and_si128(c, m24);
                        __m128i km = _mm_or_si128(_mm_cmpeq_epi32(q, k1), _mm_cmpeq_epi32(q, k2));
                        if (aa) km = _mm_or_si128(km, _mm_cmpeq_epi32(_mm_subs_epu8(q, c20), zero));
                        r = _mm_or_si128(_mm_andnot_si128(km, r), _mm_and_si128(km, amask));
                    }
                    const __m128i cb = _mm_cvtsi32_si128(cw);
                    const __m128i um = _mm_cmpeq_epi32(_mm_unpacklo_epi16(_mm_unpacklo_epi8(cb, zero), zero), zero);   /* all ones where NOT covered */
                    const __m128i old = _mm_loadu_si128((const __m128i *)(o + k));
                    _mm_storeu_si128((__m128i *)(o + k), _mm_or_si128(_mm_andnot_si128(um, r), _mm_and_si128(um, old)));
                }
                for (; k < kg1; ++k) {
                    if (!tc[k]) continue;
                    const uint32_t c = prow[cx[xa + k]];
                    if (key && clear_key(c)) o[k] = 0xff000000u;
                    else o[k] = c | 0xff000000u;
                }
                continue;
            }
            const uint32_t *sp = tmp;
            if (unit) sp = prow + q0;
            else if (have_qy != qy) {
                for (int q = 0; q < n; ++q) tmp[q] = prow[cx[xa + q]];
            }
            have_qy = qy;
            for (; k + 4 <= n; k += 4) {
                const __m128i c = _mm_loadu_si128((const __m128i *)(sp + k));
                __m128i r = _mm_or_si128(c, amask);
                if (key) {
                    const __m128i q = _mm_and_si128(c, m24);
                    __m128i km = _mm_or_si128(_mm_cmpeq_epi32(q, k1), _mm_cmpeq_epi32(q, k2));
                    if (aa) km = _mm_or_si128(km, _mm_cmpeq_epi32(_mm_subs_epu8(q, c20), zero));
                    r = _mm_or_si128(_mm_andnot_si128(km, r), _mm_and_si128(km, amask));
                }
                _mm_storeu_si128((__m128i *)(o + k), r);
            }
            for (; k < n; ++k) {
                const uint32_t c = sp[k];
                if (key && clear_key(c)) o[k] = 0xff000000u;
                else o[k] = c | 0xff000000u;
            }
        }

    };
    static int pknob = -2;
    if (pknob == -2) pknob = ntr::gx_menu_knob_value("SM64DS_MENU_PAR2");
    ntr::gx_parallel_rows(dh, 192, pic_rows_tramp<decltype(rows_job)>, &rows_job, pknob);
}

void menu_piece_draw(const MenuLayout *L, int i, const MenuSources *S, uint32_t *dst, int w, int h)
{
    if (dst && i >= 0 && i < L->np && L->p[i].custom) { L->p[i].custom(L, i, dst, w, h, stride_of(w)); return; }   /* PAUSE3 */
    if (!cost_fast()) { piece_draw_ref(L, i, S, dst, w, h); return; }
    if (!S || !dst || i < 0 || i >= L->np) return;
    const MenuPiece &P = L->p[i];
    if (P.nodraw) return;
    if (P.src == SRC_HOSTIMG || P.hostgrad || P.src == SRC_TOP_HOSTROW || (P.src == SRC_TOP_PIC && P.alpha && P.alpha < 255)) {
        g_rep.valid = 0;
        piece_draw_ref(L, i, S, dst, w, h);
        return;
    }
    if (P.src == SRC_TOP_PIC) {
        /* OPTIM1 proof knob: SM64DS_MENU_PIC_CHECK=1 draws the piece both ways from the same starting picture and counts the pixels that differ */
        static int chk = -1;
#ifndef PORT_ROM_CLEAN
        if (chk < 0) { const char *e = getenv("SM64DS_MENU_PIC_CHECK"); chk = (e && *e == '1') ? 1 : 0; }
#else
        chk = 0;     /* proof knob: not in the shipping build */
#endif
        if (chk && pic_fast()) {
            const size_t n = (size_t)stride_of(w) * (size_t)h;
            static std::vector<uint32_t> tmp;
            static long long frames = 0, bad = 0;
            static double us_fast = 0, us_ref = 0;
            if (frames == 0) { const int *dd = L->dest[i]; fprintf(stderr, "[menu] pic geometry: piece %d dest %d,%d %dx%d from s %d,%d %dx%d of pic rect %d,%d %dx%d (pic %dx%d) keycover %d key %d cov %d ex %d\n", i, dd[0], dd[1], dd[2], dd[3], P.s.x, P.s.y, P.s.w, P.s.h, S->pic_rx, S->pic_ry, S->pic_rw, S->pic_rh, S->pic_w, S->pic_h, (int)P.keycover, (int)P.key_backdrop, S->pic_cover ? 1 : 0, (int)P.has_ex); }
            tmp.assign(dst, dst + n);
            const std::chrono::steady_clock::time_point ta = std::chrono::steady_clock::now();
            if (frames & 1) piece_draw_pic_ref(L, i, S, &tmp[0], w, h);
            else piece_draw_pic(L, i, S, dst, w, h);
            const std::chrono::steady_clock::time_point tb = std::chrono::steady_clock::now();
            if (frames & 1) piece_draw_pic(L, i, S, dst, w, h);
            else piece_draw_pic_ref(L, i, S, &tmp[0], w, h);
            const std::chrono::steady_clock::time_point tc = std::chrono::steady_clock::now();
            us_fast += std::chrono::duration<double, std::micro>((frames & 1) ? tc - tb : tb - ta).count();
            us_ref += std::chrono::duration<double, std::micro>((frames & 1) ? tb - ta : tc - tb).count();
            long long nb = 0;
            for (size_t q = 0; q < n; ++q) nb += dst[q] != tmp[q];
            ++frames; bad += nb;
            if (nb || frames % 600 == 0) fprintf(stderr, "[menu] pic check: %lld pictures, %lld differing pixels so far (this one %lld), mean us fast %.0f ref %.0f\n", frames, bad, nb, us_fast / frames, us_ref / frames);
            return;
        }
        piece_draw_pic(L, i, S, dst, w, h);
    }
    else piece_draw_2d(L, i, S, dst, w, h);
}

void menu_dim(uint32_t *dst, int w, int h, int evy)
{
    if (!dst || evy <= 0) return;
    const size_t st = (size_t)stride_of(w);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) dst[y * st + x] = blend_ch(dst[y * st + x], evy, 0);
}

/* The 8-frame pulse: evy 0,1,2,3,4,3,2,1 toward white; the outline is one
   canvas unit (at least one pixel) in the darkest colour of the hotspot's own
   rim, sampled before the outline is drawn. */
void menu_highlight_paint(const MenuLayout *L, uint32_t *dst, int w, int h, unsigned frame)
{
    if (!dst || L->focus < 0 || L->focus >= L->nh) return;
    int r[4];
    if (!menu_hotspot_rect(L, L->focus, r)) return;
    static const int pulse[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };
    const int evy = pulse[frame & 7];
    const int x0 = r[0], y0 = r[1], x1 = r[0] + r[2], y1 = r[1] + r[3];
    int t = u2h(L, 1);
    if (t < 1) t = 1;
    uint32_t dark = 0xff000000u;
    int dl = 1 << 30;
    for (int y = y0; y < y1; ++y) {
        if (y < 0 || y >= h) continue;
        for (int x = x0; x < x1; ++x) {
            if (x < 0 || x >= w) continue;
            uint32_t &px = dst[(size_t)y * stride_of(w) + x];
            const int rim = (x < x0 + 1 || x >= x1 - 1 || y < y0 + 1 || y >= y1 - 1);
            if (rim && (px >> 24)) {
                const int l = luma(px);
                if (l < dl) { dl = l; dark = px | 0xff000000u; }
            }
            if ((px >> 24) && evy) px = blend_ch(px, evy, 1);
        }
    }
    for (int y = y0; y < y1; ++y) {
        if (y < 0 || y >= h) continue;
        for (int x = x0; x < x1; ++x) {
            if (x < 0 || x >= w) continue;
            if (x < x0 + t || x >= x1 - t || y < y0 + t || y >= y1 - t) dst[(size_t)y * stride_of(w) + x] = dark;
        }
    }
}

/* Opaque pixels on a source-rect edge that continue across it. An edge that
   lies on the plane border cannot cut anything and is not counted. A pixel
   counts when it is opaque inside the rect on the edge AND the pixel just
   outside is opaque too: that is a glyph cut in two (an autocropped rect has
   opaque edge pixels by construction and must still read 0). counts: top,
   bottom, left, right. Returns the total. */
int menu_seam_census(const MenuLayout *L, int piece, const MenuSources *S, int counts[4])
{
    counts[0] = counts[1] = counts[2] = counts[3] = 0;
    if (piece < 0 || piece >= L->np || !S) return 0;
    const MenuPiece &P = L->p[piece];
    if (P.src == SRC_TOP_PIC || P.src == SRC_HOSTIMG || P.hostgrad) return 0;
    const int pw = P.src == SRC_TOP_HOSTROW ? S->hostrow_w : 256;
    const int ph = P.src == SRC_TOP_HOSTROW ? S->hostrow_h : 192;
    const int sx0 = P.s.x, sy0 = P.s.y, sx1 = P.s.x + P.s.w - 1, sy1 = P.s.y + P.s.h - 1;
    for (int x = sx0; x <= sx1; ++x) {
        if (sy0 > 0 && src_opaque(&P, S, x, sy0) && src_opaque(&P, S, x, sy0 - 1)) ++counts[0];
        if (sy1 < ph - 1 && src_opaque(&P, S, x, sy1) && src_opaque(&P, S, x, sy1 + 1)) ++counts[1];
    }
    for (int y = sy0; y <= sy1; ++y) {
        if (sx0 > 0 && src_opaque(&P, S, sx0, y) && src_opaque(&P, S, sx0 - 1, y)) ++counts[2];
        if (sx1 < pw - 1 && src_opaque(&P, S, sx1, y) && src_opaque(&P, S, sx1 + 1, y)) ++counts[3];
    }
    const int tot = counts[0] + counts[1] + counts[2] + counts[3];
    if (tot) MTRACE("seam census: piece %d has %d cut pixels (top %d bottom %d left %d right %d)", piece, tot,
                    counts[0], counts[1], counts[2], counts[3]);
    return tot;
}

/* the opaque bounding box of the piece's layers inside its s; 0 if nothing is opaque */
int menu_autocrop_rect(const MenuPiece *P, const MenuSources *S, DsRect *out)
{
    int x0 = 1 << 20, y0 = 1 << 20, x1 = -1, y1 = -1;
    for (int y = P->s.y; y < P->s.y + P->s.h; ++y)
        for (int x = P->s.x; x < P->s.x + P->s.w; ++x)
            if (src_opaque(P, S, x, y)) {
                if (x < x0) x0 = x;
                if (x > x1) x1 = x;
                if (y < y0) y0 = y;
                if (y > y1) y1 = y;
            }
    if (x1 < 0) return 0;
    out->x = (short)x0; out->y = (short)y0; out->w = (short)(x1 - x0 + 1); out->h = (short)(y1 - y0 + 1);
    return 1;
}

/* ====================================================================== */
/* the synthesised-touch queue                                            */
/* ====================================================================== */

namespace {
int g_q_state;          /* 0 idle, 1 press frame 1, 2 press frame 2, 3 release due */
int g_q_x, g_q_y;
}

int hal_menu_touch_queue_press(int dsx, int dsy)
{
    if (g_q_state != 0 || g_q_real_held) return 0;
    g_q_x = dsx;
    g_q_y = dsy;
    g_q_state = 1;
    return 1;
}

int hal_menu_touch_queue_poll(int *dsx, int *dsy, int *down)
{
    if (g_q_state == 0) return 0;
    if (g_q_real_held) { g_q_state = 0; return 0; }   /* a real stylus wins; the press is dropped */
    *dsx = g_q_x;
    *dsy = g_q_y;
    if (g_q_state == 1 || g_q_state == 2) {
        *down = 1;
        ++g_q_state;
        return 1;
    }
    *down = 0;      /* the release edge, then idle */
    g_q_state = 0;
    return 1;
}

void hal_menu_touch_real_held(int held) { g_q_real_held = held ? 1 : 0; }

/* ====================================================================== */
/* frame entry points                                                     */
/* ====================================================================== */

static int scene_is_menu(int scene)
{
    return scene == 1 || scene == 4 || scene == 5 || scene == 6 || scene == 8;
}

void hal_menu_set_enable_fn(int (*fn)(int key)) { g_enable = fn; }
void hal_menu_set_netplay(int on) { g_netplay = on ? 1 : 0; }
void hal_menu_set_sources(const MenuSources *s) { g_src = s; g_nplanes = 0; }
int  hal_menu_live(void) { return g_live; }                                       /* the latest decision (input side) */
int  hal_menu_draw_live(void) { return g_dd.live; }                               /* the bound decision (draw side) */
int  hal_menu_draws_in(int stage) { return g_dd.live && g_dd.L.stage == stage; }
int  hal_menu_fallback_frames(void) { return g_fallback_frames; }
int  hal_menu_enabled(int key) { return enabled(key); }
int  hal_menu_uses_cover(void)
{
    if (!g_dd.live) return 0;
    for (int i = 0; i < g_dd.L.np; ++i)
        if (g_dd.L.p[i].keycover) return 1;
    return 0;
}
const MenuLiftInfo *hal_menu_lift_info(void) { return g_dd.live && g_dd.lift_on ? &g_dd.lift : 0; }
/* MENUFOLD1 (hunt8): may the pictures BETWEEN ticks (Smooth Motion, scene in-betweens) run under this tick's menu? They replay the
   tick's 3D and put the tick's composed picture back wherever it differs from the tick's 3D, which is exact when the menu takes no
   3D pixels as a source. A recipe that draws a piece of the 3D picture (SRC_TOP_PIC: the title's head, plates and wrench, the file
   pages' page picture, the star select's upper picture), a 3D-keyed piece, or the held snapshot frame moves 3D pixels, so the
   replayed 3D would show beside its own moved copy: those ticks keep the snap. 2D sources (SRC_TOP_2D, SRC_TOP_HOSTROW, SRC_SUB,
   host images, gradients) are the tick's own and need nothing from the 3D. SM64DS_MENU_INTERP=0 snaps every menu tick (the old
   rule). Returns 1 when no menu is bound. Draw side only; read the bound decision, never ROM memory. */
int  hal_menu_interp_mode(void)   /* SM64DS_MENU_INTERP: 0 snap every menu tick, 1 (default) the gate below, 2 allow every menu tick (a proof instrument: shows what the gate prevents) */
{
    static int m = -1;
    if (m < 0) {
        const char *e = getenv("SM64DS_MENU_INTERP");
        m = (e && e[0] == '0' && e[1] == '\0') ? 0 : (e && e[0] == '2' && e[1] == '\0') ? 2 : (e && e[0] == '3' && e[1] == '\0') ? 3 : 1;
    }
    return m;
}
int  hal_menu_interp_ok(void)
{
    if (!g_dd.live) return 1;
    const int mode = hal_menu_interp_mode();
    if (mode == 2) return 1;
    if (mode == 0 || g_dd.fallback) return 0;
    if (g_dd.L.snap_save) return 0;
    for (int i = 0; i < g_dd.L.np; ++i) {
        const MenuPiece &P = g_dd.L.p[i];
        if (P.nodraw) continue;                                     /* MENUINTERP2: a click-only piece draws nothing, whatever else it says (the file select's first page carries one with a cover key) */
        if (P.src != SRC_TOP_PIC && !P.keycover && !P.snap) continue;
        if (P.keycover || P.xfill || P.snap) return 0;
        if (P.nodraw) continue;                                     /* a click-only piece draws nothing */
        /* a piece of the 3D picture drawn 1:1 as the whole 4:3 picture, full height and centred, lands every pixel where it already was
           (the file pages at 4:3, the title's TOUCH TO START at any aspect: the scene is shown at its native layout there) */
        const MenuLayout &L = g_dd.L;
        const int dx = L.dest[i][0], dy = L.dest[i][1], dw = L.dest[i][2], dh = L.dest[i][3];
        const bool whole = P.s.x == 0 && P.s.y == 0 && P.s.w == 256 && P.s.h == 192 && P.knum == P.kden &&
                           dy == 0 && dh == L.h && dw * 3 == dh * 4 && dx == (L.w - dw) / 2;
        if (!whole) return 0;
    }
    return 1;
}
/* MENUINTERP2 (hunt8): may a tick whose menu moves 3D pixels (hal_menu_interp_ok said no) still have pictures between ticks? Yes
   when every piece can be drawn again from a picture's own 3D: the pieces read the tick's planes (top2d, engine B, host images,
   all constant between ticks) and the picture + 3D coverage, which each in-between supplies from its own replay
   (hal_menu_relift, hal_sub_screen). What cannot: a held-frame piece (snap, snap_save),
   the fallback frame. (An edge-column fill, xfill, re-reads the picture's edge and re-lifts like any other.) Only the POST_FADE stage (every recipe that takes 3D pixels draws there), and only with a picture piece to
   re-lift. SM64DS_MENU_INTERP=3 is MENUFOLD1's gate (no relift), 0 snaps everything. Draw side only. */
/* why not (0 = ok): 1 live/mode/fallback, 2 stage or the drawing page's held frame.
   DOODLEFIX1: the title's drawing page (variant MENU_TITLE_V_DOODLE) keeps its held frame (snap_save) on every live tick and snaps:
   the ROM draws the page once per tick, and an in-between re-lifted its 3D layer (the strokes and buttons) from the replayed 3D, whose
   blend of two ticks carries the head's colours along the stroke edges. The face press page (V_TOY) also saves a held frame during its
   first 94 ticks, but its squash and jump keeps its in-betweens, so the gate names the drawing page and not snap_save alone. */
int  hal_menu_relift_why(void)
{
    if (!g_dd.live || hal_menu_interp_mode() != 1 || g_dd.fallback) return 1;
    const MenuLayout &L = g_dd.L;
    if (L.stage != MENU_STAGE_POST_FADE) return 2;
    if (L.snap_save && L.recipe && L.recipe->build == menu_title_build && L.variant == MENU_TITLE_V_DOODLE) return 2;
    return 0;
}
int  hal_menu_relift_ok(void) { return hal_menu_relift_why() == 0; }
/* does a re-draw between ticks need the picture as the menu found it (the tick keeps a copy, hal_sub_screen)? A picture piece cuts from it, a
   held-frame piece or a save lays over / copies the finished picture. */
int  hal_menu_key_sentinel(void) { return g_dd.live && g_dd.L.key_sentinel ? 1 : 0; }
int  hal_menu_needs_picsnap(void)
{
    if (!g_dd.live) return 0;
    if (g_dd.L.snap_save) return 1;
    for (int i = 0; i < g_dd.L.np; ++i)
        if ((g_dd.L.p[i].src == SRC_TOP_PIC && !g_dd.L.p[i].nodraw) || g_dd.L.p[i].snap) return 1;
    return 0;
}
/* debug: one line per piece of the bound layout (the in-between gate's inputs) */
void hal_menu_relift_dump(void)
{
    const MenuLayout &L = g_dd.L;
    fprintf(stderr, "[interp] menu layout %s v%d stage %d snap_save %d fb %d np %d:", L.recipe ? L.recipe->name : "?", L.variant, L.stage, L.snap_save, g_dd.fallback, L.np);
    for (int i = 0; i < L.np; ++i) {
        const MenuPiece &P = L.p[i];
        fprintf(stderr, " [%d src%d%s%s%s%s a%d]", i, (int)P.src, P.keycover ? " key" : "", P.xfill ? " xf" : "", P.snap ? " SNAP" : "", P.nodraw ? " nd" : "", (int)P.alpha);
    }
    fputc('\n', stderr);
}
int  hal_menu_trace_on(void) { return trace_on(); }
void hal_menu_set_dst_stride(int stride) { g_dst_stride = stride; }
void hal_menu_set_layer_mode(int on) { g_layer_mode = on ? 1 : 0; if (on) g_layer_conflicts = 0; }
void hal_menu_set_layer2(unsigned *p) { g_layer2 = p; }
long long hal_menu_layer_conflicts(void) { return g_layer_conflicts; }
/* SAVEOVL1: may this tick's bound recipe be drawn into a menu LAYER (gpu_raster.h) instead of onto the picture? Yes when every
   piece only WRITES opaque pixels or composes a host image over what is below it (the level-clear plates, bands and planks); no
   when a piece reads the picture (a piece of the 3D, the dimmed backdrop, the held snapshot, the lit-hotspot paint, a custom draw,
   the gradients and wallpapers). The draw side asks it with the decision bound. */
extern "C" int hal_menu_layer_ok(void)
{
    const MenuDecision &D = g_dd;
    if (!D.live || D.fallback) return 0;
    if (D.L.backdrop == MENU_BACKDROP_DIM_AUTO || D.L.snap_save) return 0;
    for (int i = 0; i < D.L.np; ++i) {
        const MenuPiece &P = D.L.p[i];
        if (P.nodraw) continue;
        if (P.custom || P.hostgrad || P.src == SRC_TOP_PIC) return 0;
        if (P.src == SRC_HOSTIMG && (P.wall || P.iris || P.paper || P.snap)) return 0;
    }
    return 1;
}
void hal_menu_note_lift_drawn(void) { g_dd_lift_pending = 0; }
int  hal_menu_uses_src(int src)
{
    if (!g_dd.live) return 0;
    for (int i = 0; i < g_dd.L.np; ++i)
        if ((int)g_dd.L.p[i].src == src) return 1;
    return 0;
}

/* SM64DS_MENU_DRAW_LATE=1: the draw binds the previous tick's decision (debug; simulates the render thread's lateness) */
static int draw_late(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = getenv("SM64DS_MENU_DRAW_LATE");
        v = (e && *e && *e != '0') ? 1 : 0;
    }
    return v;
}

/* a decision is copied whole while it is live; a dead one needs only its head */
static void dec_copy(MenuDecision *dst, const MenuDecision *src)
{
    if (src->live) {
        memcpy(dst, src, sizeof *dst);
    } else {
        const size_t head = (size_t)((const char *)&src->L - (const char *)src);
        memcpy(dst, src, head);
    }
}

/* run hunt8, lane MENUPACE1: THE MAIN MENU'S ART IS DECODED AHEAD. The title -> main menu tick used to decode the logo, the three planks, the
   arrows, the pattern and the star PNGs on the logic thread (a 41 ms tick). While a one-screen menu scene is up, each tick hands the
   next picture to the hostimg worker (decode alone, no scale) and installs what the worker finished, so by the time the page changes the
   pictures are in memory. SM64DS_NO_MENU_PREDECODE=1 turns it off (the old behaviour). */
static void menu_art_predecode_tick(void)
{
    static const bool off = getenv("SM64DS_NO_MENU_PREDECODE") != 0;
    static int next = 0;
    static const int kIds[] = { MENU_IMG_LOGO, MENU_IMG_BTN_MP, MENU_IMG_BTN_ADV, MENU_IMG_BTN_REC, MENU_IMG_ARROW_UP, MENU_IMG_ARROW_DOWN,
                                MENU_IMG_PATTERN, MENU_IMG_STAR };
    if (off) return;
    menu_hostimg_prefetch_poll();
    while (next < (int)(sizeof kIds / sizeof kIds[0])) {
        if (menu_hostimg_predecode(kIds[next]) < 0) return;      /* the queue is full: the rest on a later tick */
        ++next;
    }
}

/* THE DECIDE STEP. Logic thread, once per tick after the ROM tick. Everything that reads the ROM happens here (up(), the
   recipe's facts(), build() through L->facts, and the rom_cursor / pad_ok the input paths call), and the answer is a
   MenuDecision the draw side consumes by value. */
void hal_menu_decide(int w, int h, int scene_id)
{
    ++g_frame;
    if (scene_is_menu(scene_id) && enabled(MENU_KEY_ONE_SCREEN_MENUS)) menu_art_predecode_tick();
    g_pub_i ^= 1;
    MenuDecision &D = g_pub[g_pub_i];
    D.serial = g_frame;
    D.live = 0;
    D.recipe = -1;
    D.variant = 0;
    D.fallback = 0;
    D.w = w;
    D.h = h;
    D.focus_shown = 0;
    D.real_held = 0;
    D.latched = 0;
    D.lift_on = 0;
    int pick = -1, variant = 0, fb = -1;
    for (int i = 0; i < g_menu_nrecipes; ++i) {
        const MenuRecipe &R = g_menu_recipes[i];
        if (R.flags & MENU_RF_FALLBACK) { fb = i; continue; }
        if (!R.up || !R.build || !enabled(R.enable_key)) continue;
        const int v = R.up(scene_id);
        if (v > 0) { pick = i; variant = v; break; }
    }
    int is_fb = 0;
    if (pick < 0 && fb >= 0 && scene_id != MENU_SCENE_GATED && scene_is_menu(scene_id) &&
        enabled(MENU_KEY_ONE_SCREEN_MENUS)) {
        pick = fb;
        variant = 1;
        is_fb = 1;
    }
    if (pick < 0 || w < 8 || h < 8) {
        if (g_live) MTRACE("recipe off (scene %d)", scene_id);
        g_live = 0;
        g_cur_recipe = -1;
        g_focus_shown = 0;
        g_drag_piece = -1;
        g_q_state = 0;          /* a queued synthesised press must not fire into the next scene */
        {
            std::lock_guard<std::mutex> lk(g_mu);
            g_slatch.recipe = -2;
            g_seen_valid = 0;   /* the seen layout is the old menu's: never invert through it */
        }
        memset(&D.lift, 0, sizeof D.lift);
        return;
    }
    const MenuRecipe *R = &g_menu_recipes[pick];
    const int new_menu = pick != g_cur_recipe || variant != g_cur_variant;
    if (new_menu) {
        /* a new recipe or variant: an old drag belongs to the old rects, and an old press (aimed at the old rects)
           is dropped */
        g_drag_piece = -1;
        g_q_state = 0;
    }
    const int changed = pick != g_cur_recipe || variant != g_cur_variant || g_cur.w != w || g_cur.h != h;
    int keep_focus = (pick == g_cur_recipe && variant == g_cur_variant) ? g_cur.focus : -1;
    /* the ROM facts, read once, before build() */
    memset(&g_cur.facts, 0, sizeof g_cur.facts);
    if (R->facts) R->facts(&g_cur.facts);
    menu_layout_solve(&g_cur, R, variant, w, h);
    g_cur.focus = keep_focus < g_cur.nh ? keep_focus : -1;
    {
        std::lock_guard<std::mutex> lk(g_mu);
        if (pick != g_slatch.recipe || variant != g_slatch.variant) {
            memset(&g_slatch, 0, sizeof g_slatch);
            g_slatch.recipe = pick;
            g_slatch.variant = variant;
            g_focus_shown = 0;
        }
        for (int i = 0; i < g_cur.np; ++i)
            if ((g_slatch.on >> i) & 1) { g_cur.p[i].s = g_slatch.r[i]; D.latched |= (uint16_t)(1u << i); }
    }
    menu_layout_dest(&g_cur);
    g_live = g_cur.np > 0;
    g_cur_recipe = pick;
    g_cur_variant = variant;
    if (is_fb) ++g_fallback_frames;
    /* the decision: the layout by value, the lift plan (one truth for the frame), the cursor as of now */
    memset(&D.lift, 0, sizeof D.lift);
    D.live = g_live;
    D.recipe = pick;
    D.variant = variant;
    D.fallback = is_fb;
    D.focus_shown = g_focus_shown;
    D.real_held = g_q_real_held;
    if (g_live) {
        D.L = g_cur;
        D.lift.lift2d = g_cur.lift_owners;
        D.lift.hide = g_cur.hide_owners;
        for (int i = 0; i < g_cur.np && D.lift.nbands < 8; ++i)
            if (g_cur.p[i].src == SRC_TOP_HOSTROW) {
                D.lift.band[D.lift.nbands].layers = g_cur.p[i].layers;
                D.lift.band[D.lift.nbands].r0 = g_cur.p[i].s.y;
                D.lift.band[D.lift.nbands].r1 = g_cur.p[i].s.y + g_cur.p[i].s.h;
                ++D.lift.nbands;
            }
        D.lift_on = (D.lift.lift2d | D.lift.hide | D.lift.nbands) != 0;
    }
    {   /* the input-side record: the layout this decision's picture will show */
        std::lock_guard<std::mutex> lk(g_mu);
        g_seen = g_cur;
        /* a new menu is not clickable until the draw has put its first picture up (hal_menu_present acks it): the
           player must never press a button he has not seen, and never through the previous menu's rects */
        g_seen_valid = g_live && g_seen_valid && !new_menu;
    }
    /* SAVE2: a layout whose piece or hotspot count changed under the same recipe, variant and size (the save screen's plates
       appear a few frames after its text) is traced like a new one, so the capture tools read the hotspots that are really up */
    static int last_shape = -1;
    const int shape = g_cur.np * 64 + g_cur.nh;
    const int traced = changed || shape != last_shape;
    last_shape = shape;
    if (traced) {
        MTRACE("recipe=%s variant=%d size=%dx%d unit=%d/%d canvas_w=%d %s stage=%d fallback_frames=%d", R->name,
               variant, w, h, g_cur.unum, g_cur.uden, g_cur.canvas_w, menu_wide(&g_cur) ? "wide" : "narrow",
               g_cur.stage, g_fallback_frames);
        for (int i = 0; i < g_cur.np; ++i)
            MTRACE("  piece %d src=%d s=(%d,%d,%d,%d) -> dest=(%d,%d,%d,%d) k=%d/%d touch=%d", i, (int)g_cur.p[i].src,
                   g_cur.p[i].s.x, g_cur.p[i].s.y, g_cur.p[i].s.w, g_cur.p[i].s.h, g_cur.dest[i][0],
                   g_cur.dest[i][1], g_cur.dest[i][2], g_cur.dest[i][3], g_cur.p[i].knum, g_cur.p[i].kden,
                   g_cur.p[i].touch);
        for (int i = 0; i < g_cur.nh; ++i) {
            int r[4];
            menu_hotspot_rect(&g_cur, i, r);
            MTRACE("  hotspot %d piece=%d box=(%d,%d,%d,%d) host=(%d,%d,%d,%d) rom=%d", i, g_cur.hs[i].piece,
                   g_cur.hs[i].box.x, g_cur.hs[i].box.y, g_cur.hs[i].box.w, g_cur.hs[i].box.h, r[0], r[1], r[2],
                   r[3], g_cur.hs[i].rom_index);
        }
    }
}

unsigned hal_menu_decision_copy(MenuDecision *out)
{
    if (!out) return g_pub[g_pub_i].serial;
    dec_copy(out, &g_pub[g_pub_i]);
    return out->serial;
}

/* BIND A DECISION ON THE DRAW SIDE. d = null: this process's latest (the one-thread build), or with SM64DS_MENU_DRAW_LATE
   the previous tick's. */
void hal_menu_draw_frame_begin(const MenuDecision *d)
{
    /* THE LIFT PROMISE FROM LAST FRAME, CHECKED (the net under the whole lift/draw mechanism, kept from the
       level-clear compose: the 0.4.1 defect was text lifted out of the picture and never drawn back). Once per
       process, to stderr, never fatal. */
    if (g_dd_lift_pending) {
        static int said;
        if (!said) {
            said = 1;
            fprintf(stderr, "[compose] LIFT WITHOUT DRAW: a menu recipe lifted engine-A pixels out of a frame its "
                    "draw never reached; the frame predicate is missing a term the draw depends on\n");
            fflush(stderr);
        }
        g_dd_lift_pending = 0;
    }
    g_nplanes = 0;
    if (!d) {
        const MenuDecision &cur = g_pub[g_pub_i];
        const MenuDecision &prev = g_pub[g_pub_i ^ 1];
        d = &cur;
        if (draw_late()) {
            const bool same = cur.live && prev.live && cur.recipe == prev.recipe && cur.variant == prev.variant &&
                              cur.w == prev.w && cur.h == prev.h;
            d = same ? &prev : &g_dead;
        }
    }
    dec_copy(&g_dd, d);
    g_draw_frame = d->serial;
    g_dd_lift_pending = g_dd.live && (g_dd.lift.lift2d | g_dd.lift.nbands) != 0;
}

void hal_menu_frame_begin(int w, int h, int scene_id)
{
    hal_menu_decide(w, h, scene_id);
    hal_menu_draw_frame_begin(0);
}

/* the draw's two pieces of feedback, both layout geometry */
static void latch_publish(const MenuDecision &D, int i, const DsRect &r)
{
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_slatch.recipe != D.recipe || g_slatch.variant != D.variant) return;   /* a stale decision: the menu changed */
    g_slatch.r[i] = r;
    g_slatch.on |= (uint16_t)(1u << i);
}

/* the draw put this decision's menu on the picture: the input may now invert through it */
static void seen_ack(const MenuDecision &D)
{
    std::lock_guard<std::mutex> lk(g_mu);
    if (g_live && g_seen.recipe == D.L.recipe && g_seen.variant == D.L.variant) g_seen_valid = 1;
}

static void seen_amend(const MenuDecision &D)
{
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_seen_valid || g_seen.recipe != D.L.recipe || g_seen.variant != D.L.variant || g_seen.np != D.L.np) return;
    g_seen.w = D.L.w; g_seen.h = D.L.h; g_seen.unum = D.L.unum; g_seen.uden = D.L.uden; g_seen.canvas_w = D.L.canvas_w;
    for (int i = 0; i < D.L.np; ++i) {
        g_seen.p[i].s = D.L.p[i].s;
        memcpy(g_seen.dest[i], D.L.dest[i], sizeof g_seen.dest[i]);
    }
}

/* ASSET1: what a menu frame costs, in microseconds, summed per process and printed (SM64DS_MENU_TRACE) every 120
   drawn frames: the recipe's draw here, and the host's source preparation (hal_menu_note_cost, sub_screen.cpp). */
static double g_cost_draw_us, g_cost_prep_us;
static int g_cost_n;
void hal_menu_note_cost(double prep_us) { g_cost_prep_us += prep_us; }

/* ASSET5 proof knob (SM64DS_MENU_SHOTS=a,b,c or a-b:step): the menu picture as drawn (after the pieces, before the focus outline) is written
   as shot_<core frame>.bmp (24-bit, top-down) in the working directory on those core frames (hal_menu_frame_begin count, 1 = the first
   frame). Off unless the variable is set; one cached getenv otherwise. */
static void menu_shot_maybe(const uint32_t *dst, int w, int h, unsigned frame)
{
    static int parsed = 0;
    static int lo[16], hi[16], st[16], n = 0;
    if (!parsed) {
        parsed = 1;
        const char *e = getenv("SM64DS_MENU_SHOTS");
        while (e && *e && n < 16) {
            char *q;
            const int a = (int)strtol(e, &q, 10);
            if (q == e) break;
            lo[n] = hi[n] = a; st[n] = 1;
            if (*q == '-') { e = q + 1; hi[n] = (int)strtol(e, &q, 10); st[n] = 1; if (*q == ':') { e = q + 1; st[n] = (int)strtol(e, &q, 10); if (st[n] < 1) st[n] = 1; } }
            ++n;
            if (*q != ',') break;
            e = q + 1;
        }
    }
    if (!n) return;
    int hit = 0;
    for (int k = 0; k < n; ++k)
        if ((int)frame >= lo[k] && (int)frame <= hi[k] && ((int)frame - lo[k]) % st[k] == 0) hit = 1;
    if (!hit) return;
    const int stride = g_dst_stride > 0 ? g_dst_stride : w;
    char name[64];
    snprintf(name, sizeof name, "shot_%04u.bmp", frame);
    FILE *f = fopen(name, "wb");
    if (!f) return;
    const int rowb = (w * 3 + 3) & ~3;
    unsigned char hd[54] = { 'B', 'M' };
    const unsigned sz = 54u + (unsigned)rowb * (unsigned)h;
    memcpy(hd + 2, &sz, 4);
    const unsigned off = 54; memcpy(hd + 10, &off, 4);
    const unsigned bi = 40; memcpy(hd + 14, &bi, 4);
    memcpy(hd + 18, &w, 4);
    const int nh = -h; memcpy(hd + 22, &nh, 4);
    hd[26] = 1; hd[28] = 24;
    fwrite(hd, 1, 54, f);
    unsigned char *row = (unsigned char *)calloc(1, (size_t)rowb);
    for (int y = 0; y < h && row; ++y) {
        for (int x = 0; x < w; ++x) {
            const uint32_t p = dst[(size_t)y * stride + x];
            row[x * 3] = (unsigned char)p; row[x * 3 + 1] = (unsigned char)(p >> 8); row[x * 3 + 2] = (unsigned char)(p >> 16);
        }
        fwrite(row, 1, (size_t)rowb, f);
    }
    free(row);
    fclose(f);
}

/* MENUCOST1: per recipe and variant cost windows (SM64DS_MENU_TRACE): the first draw, then the mean and the worst of every 60 drawn
   frames of that recipe and variant. */
static double g_piece_us[24];
static int g_piece_n;
static void cost_window(const char *name, int variant, double us)
{
    struct Slot { const char *name; int variant; int n; double sum, mx; double pu[24]; };
    static Slot slot[32];
    static int ns = 0;
    int k = 0;
    while (k < ns && !(slot[k].name == name && slot[k].variant == variant)) ++k;
    if (k == ns) {
        if (ns == 32) return;
        slot[ns++] = Slot { name, variant, 0, 0.0, 0.0 };
        fprintf(stderr, "[menu] cost2 %s v%d first draw %.0f us\n", name, variant, us);
    }
    Slot &S = slot[k];
    S.sum += us; if (us > S.mx) S.mx = us;
    for (int q = 0; q < g_piece_n && q < 24; ++q) S.pu[q] += g_piece_us[q];
    if (++S.n == 60) {
        char pb[400];
        int o = 0;
        pb[0] = 0;
        for (int q = 0; q < g_piece_n && q < 24 && o < 360; ++q) o += snprintf(pb + o, sizeof pb - (size_t)o, " %d:%.0f", q, S.pu[q] / 60);
        fprintf(stderr, "[menu] cost2 %s v%d: 60 frames mean %.0f us max %.0f us, pieces (us):%s\n", name, variant, S.sum / 60, S.mx, pb);
        S.n = 0; S.sum = 0; S.mx = 0;
        for (int q = 0; q < 24; ++q) S.pu[q] = 0;
    }
}

extern "C" int port_wipe_cut_picture(uint32_t *dst, int w, int h, int stride);   /* ntr/ppu_sub.cpp */
void hal_menu_present(uint32_t *dst, int w, int h, int stage)
{
    MenuDecision &D = g_dd;
    if (!D.live || !dst || D.L.stage != stage) return;
    const std::chrono::steady_clock::time_point t_in = std::chrono::steady_clock::now();
    struct CostEnd {
        std::chrono::steady_clock::time_point t;
        const char *name; int variant;
        ~CostEnd()
        {
            const double us = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - t).count();
            g_cost_draw_us += us;
            if (trace_on()) cost_window(name, variant, us);
            if (++g_cost_n % 120 == 0 && trace_on())
                fprintf(stderr, "[menu] cost over %d drawn frames: draw %.0f us, source prep %.0f us per frame\n", g_cost_n,
                        g_cost_draw_us / g_cost_n, g_cost_prep_us / g_cost_n);
        }
    } cost_end = { t_in, D.L.recipe ? D.L.recipe->name : "?", D.L.variant };
    const MenuSources *S = g_src;
    g_nplanes = 0;
    int amended = 0;
    if (D.L.w != w || D.L.h != h) {       /* the picture changed under the frame: solve again (build sees L.facts only) */
        const MenuRecipe *R = D.L.recipe;
        const int f = D.L.focus;
        DsRect keep[MENU_MAX_PIECES];
        for (int i = 0; i < D.L.np; ++i) keep[i] = D.L.p[i].s;
        menu_layout_solve(&D.L, R, D.variant, w, h);
        D.L.focus = f < D.L.nh ? f : -1;
        for (int i = 0; i < D.L.np; ++i)       /* the latched autocrops belong to this open, not to one size */
            if ((D.latched >> i) & 1) D.L.p[i].s = keep[i];
        menu_layout_dest(&D.L);
        amended = 1;
        MTRACE("draw re-solved %dx%d -> %dx%d", D.w, D.h, w, h);
    }
    if (S) {   /* latch the autocrop rects the first frame their pieces have pixels */
        int again = 0;
        for (int i = 0; i < D.L.np; ++i) {
            if (!D.L.p[i].autocrop || ((D.latched >> i) & 1)) continue;
            DsRect r;
            if (menu_autocrop_rect(&D.L.p[i], S, &r)) {
                D.latched |= (uint16_t)(1u << i);
                D.L.p[i].s = r;
                latch_publish(D, i, r);
                again = 1;
                MTRACE("autocrop latched piece %d -> (%d,%d,%d,%d)", i, r.x, r.y, r.w, r.h);
            }
        }
        if (again) { menu_layout_dest(&D.L); amended = 1; }
    }
    /* LOGOFADE1: the glide piece (the intro's big logo on its trip to the top left) is drawn at this picture's own place: the tick's own
       picture at its fx, fy, fw, fh, a picture between ticks a of the way from the tick before (g_relift_alpha, ARROWSLOW1's fraction).
       Set from the piece every picture, so the bound layout's rect is never left at an in-between's place. */
    if (D.L.glide_on && D.L.glide_piece < D.L.np) {
        const MenuPiece &P = D.L.p[D.L.glide_piece];
        const double a = g_in_relift ? g_relift_alpha : 1.0;
        const int to[4] = { P.fx, P.fy, P.fw, P.fh };
        int *d = D.L.dest[D.L.glide_piece];
        for (int k = 0; k < 4; ++k) d[k] = D.L.glide_from[k] + (int)floor((double)(to[k] - D.L.glide_from[k]) * a + 0.5);
#ifndef PORT_ROM_CLEAN
        { static const int tr = getenv("SM64DS_LF1_TRACE") ? 1 : 0;     /* LOGOFADE1 proof instrument: the logo's rect on every picture */
          if (tr) fprintf(stderr, "[lf1-glide] %s a=%.4f rect %d,%d %dx%d\n", g_in_relift ? "between" : "tick", a, d[0], d[1], d[2], d[3]); }
#endif
    }
    if (D.L.backdrop == MENU_BACKDROP_DIM_AUTO && S && !S->rom_dimming) menu_dim(dst, w, h, 8);
    {
        /* ASSET4 measurement (SM64DS_MENU_COVBBOX=1): the 3D coverage bounding box per frame in lower-screen DS pixels, the
           wrench's own 40 x 40 corner kept apart, with run-wide extremes and an 8 x 8 DS pixel occupancy grid of the union */
        static int on = -1, from = 0;
        if (on < 0) { const char *e = getenv("SM64DS_MENU_COVBBOX"); on = (e && *e && *e != '0') ? 1 : 0; from = on ? atoi(e) : 0; }
        if (on && S && S->pic_cover && S->pic_rw > 0 && S->pic_rh > 0) {
            static int mn_x = 9999, mn_y = 9999, mx_x = -1, mx_y = -1, nfr = 0;
            static unsigned char grid[24][32];
            int bx0 = 9999, by0 = 9999, bx1 = -1, by1 = -1;
            for (int y = 0; y < S->pic_rh; ++y) {
                const uint8_t *cr = S->pic_cover + (size_t)(S->pic_ry + y) * S->pic_stride + S->pic_rx;
                const int dy = y * 192 / S->pic_rh;
                for (int x = 0; x < S->pic_rw; ++x) {
                    if (!cr[x]) continue;
                    const int dx = x * 256 / S->pic_rw;
                    if (dx >= 216 && dy < 40) continue;
                    if (dx < bx0) bx0 = dx;
                    if (dx > bx1) bx1 = dx;
                    if (dy < by0) by0 = dy;
                    if (dy > by1) by1 = dy;
                    if (nfr + 1 >= from) grid[dy / 8][dx / 8] = 1;
                }
            }
            ++nfr;
            if (bx1 >= 0 && nfr >= from) {
                if (bx0 < mn_x) mn_x = bx0;
                if (by0 < mn_y) mn_y = by0;
                if (bx1 > mx_x) mx_x = bx1;
                if (by1 > mx_y) mx_y = by1;
            }
            fprintf(stderr, "[covbbox] f%d bbox x%d..%d y%d..%d | run x%d..%d y%d..%d\n", nfr, bx0, bx1, by0, by1, mn_x, mx_x, mn_y, mx_y);
            if (nfr % 240 == 0) {
                for (int gy = 0; gy < 24; ++gy) {
                    char ln[40];
                    for (int gx = 0; gx < 32; ++gx) ln[gx] = grid[gy][gx] ? '#' : '.';
                    ln[32] = 0;
                    fprintf(stderr, "[covgrid] %s\n", ln);
                }
            }
            fflush(stderr);
        }
    }
    if (!g_in_relift && !D.L.snap_save) g_snap_old_valid = 0;     /* a tick that does not save drew from g_snap itself */
    /* ARROWSLOW1: a bob piece (the main menu's cursor arrow) is drawn at this picture's own place on its curve: the tick's own picture
       at a = 1, a picture between ticks a of the way from the tick before. The whole pixel goes in the dest rect, the fraction in
       g_bob_frac (the draw shifts the picture down by it), so the arrow moves on every picture and never by more than its own step. */
    for (int i = 0; i < D.L.np && i < MENU_MAX_PIECES; ++i) {
        const MenuPiece &P = D.L.p[i];
        g_bob_frac[i] = 0.f;
        if (!P.bob) continue;
        const double a = g_in_relift ? g_relift_alpha : 1.0;
        const double y = P.bob_y0 + P.bob_amp * menu_bob_curve((double)P.bob_t - 1.0 + a) / 4.0;
        const double fl = floor(y);
        D.L.dest[i][1] = (int)fl;
        g_bob_frac[i] = (float)(y - fl);
    }
    /* FLIPSMOOTH1: a turning plank (MenuPiece.turn) is drawn at this picture's own phase of the turn; see turn_place. The tick's rect comes
       back after the draw, so the decision (the touch inverse, the next picture) never sees an in-between's rect. */
    turn_place(D.L, g_in_relift ? g_relift_alpha : 1.0);
    g_rep.enable = cost_fast(); g_rep.valid = 0;
    if (trace_on()) {
        for (int i = 0; i < D.L.np; ++i) {
            const std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now();
            if (!(i < MENU_MAX_PIECES && g_turn_hide[i])) menu_piece_draw(&D.L, i, S, dst, w, h);      /* FLIPSMOOTH1 */
            if (i < 24) g_piece_us[i] = std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - tp).count();
        }
        g_piece_n = D.L.np;
        { static int said; if (D.L.np >= 8 && said != D.L.np) { said = D.L.np; for (int i = 0; i < D.L.np; ++i) { const MenuPiece &Q = D.L.p[i]; const int *dd = D.L.dest[i];
            fprintf(stderr, "[menu] piece %d: src %d dest %d,%d %dx%d s %d,%d %dx%d alpha %d custom %d nodraw %d hostimg %d key %d layers %x\n", i, (int)Q.src, dd[0], dd[1], dd[2], dd[3], Q.s.x, Q.s.y, Q.s.w, Q.s.h, (int)Q.alpha, Q.custom ? 1 : 0, (int)Q.nodraw, (int)Q.hostimg, (int)Q.key_backdrop, (unsigned)Q.layers); } } }
    } else {
        for (int i = 0; i < D.L.np; ++i) if (!(i < MENU_MAX_PIECES && g_turn_hide[i])) menu_piece_draw(&D.L, i, S, dst, w, h);      /* FLIPSMOOTH1 */
    }
    turn_restore(D.L);      /* FLIPSMOOTH1 */
    g_rep.enable = 0; g_rep.valid = 0;
    /* MGEXIT1: the wipe (the minigames' star and circle) is cut into the finished picture once, about its centre; the two source screens were
       not cut while this recipe is live (ntr::wipe_suppressed), because the recipe moves their pieces */
    port_wipe_cut_picture(dst, w, h, g_dst_stride > 0 ? g_dst_stride : w);
    if (D.L.snap_save) snap_save(dst, w, h);
    menu_shot_maybe(dst, w, h, D.serial);
    /* HOVERBOX1: no port-made pulsing outline over a hovered or pad-focused hotspot (menu_highlight_paint is no longer called) */
    seen_ack(D);
    if (amended) seen_amend(D);
    g_dd_lift_pending = 0;      /* the draw honoured the lift */
}

/* MENUINTERP2: the same draw for a picture between ticks (see hal_menu_relift_ok). Never saves the held frame, draws held-frame
   pieces from the one this tick drew from. */
void hal_menu_set_relift_alpha(double a) { g_relift_alpha = a < 0.0 ? 0.0 : a > 1.0 ? 1.0 : a; }
float hal_menu_piece_bob_frac(int piece) { return piece >= 0 && piece < MENU_MAX_PIECES ? g_bob_frac[piece] : 0.f; }
void hal_menu_present_relift(uint32_t *dst, int w, int h, int stage)
{
    g_in_relift = 1;
    hal_menu_present(dst, w, h, stage);
    g_in_relift = 0;
}

void hal_menu_touch_inverse(int bx, int by, int *dsx, int *dsy, int *in)
{
    std::lock_guard<std::mutex> lk(g_mu);
    if (!g_seen_valid || !g_live) { *dsx = -1; *dsy = -1; *in = 0; return; }
    menu_inverse(&g_seen, bx, by, &g_drag_piece, dsx, dsy, in);
    /* menu_inverse sets the drag piece only on a hit; MenuInverse is also what
       a held stylus clamps to, so a miss keeps whatever piece it started on */
}

void hal_menu_touch_end(void) { g_drag_piece = -1; }

static unsigned g_hover_stamp;     /* ASSET3: counts the mouse moves that landed on a hotspot */
static std::atomic<int> g_cursor_snd_req{0};
static int g_hover_over = -1;      /* EXTRAS093: the hotspot the mouse is over now, -1 when it is over none */
extern "C" void func_02012790(int id);
int (*g_menu_cursor_snd_hook)(void);
void host_menu_cursor_sound(void) { g_cursor_snd_req.store(1); }
void host_menu_cursor_sound_flush(void)
{
    if (!g_cursor_snd_req.exchange(0)) return;
    static int log = -1;
    if (log < 0) log = getenv("SM64DS_MENU_SOUND_LOG") ? 1 : 0;
    if (g_menu_cursor_snd_hook && g_menu_cursor_snd_hook()) {
        if (log) fprintf(stderr, "[menu] cursor sound (the title's own D-pad move sound)\n");
        return;
    }
    if (log) fprintf(stderr, "[menu] cursor sound (func_02012790 0x12e)\n");
    func_02012790(0x12e);
}

int hal_menu_hover(int bx, int by)
{
    if (!g_live) return 0;
    if (!menu_focus_hover(&g_cur, bx, by)) { g_hover_over = -1; return 0; }
    /* the mouse arrived on a button it was not on: the ROM's cursor sound, once */
    /* HUDMENU2 (H5, Tango 10-20): a cursor that does not move makes no sound. The plank the mouse landed on is already the lit one (the ROM's cursor is
       on it: the pause and save plates, the level-clear plates), so there is nothing to announce. The first hover onto a plank that is not lit
       still plays it; the recipe lights it a frame later. */
    bool lit_already = false;
    if (g_cur.focus >= 0 && g_cur.focus < g_cur.nh) {
        const int pi = g_cur.hs[g_cur.focus].piece;
        if (pi >= 0 && pi < g_cur.np && (g_cur.p[pi].hl & 1)) lit_already = true;
    }
    if (g_cur.focus != g_hover_over && !lit_already) host_menu_cursor_sound();
    g_hover_over = g_cur.focus;
    g_focus_shown = 1;
    ++g_hover_stamp;
    return 1;
}

/* ASSET3: the hotspot the mouse last landed on, for a native recipe that mirrors it into the ROM's own cursor. Returns 1 and the
   hotspot's DS box origin when a hover exists and has not been read at this stamp yet; *stamp changes on every new hover event. */
int hal_menu_hover_box(int *x, int *y, unsigned *stamp)
{
    if (!g_live || !g_focus_shown || g_cur.focus < 0 || g_cur.focus >= g_cur.nh) return 0;
    *x = g_cur.hs[g_cur.focus].box.x;
    *y = g_cur.hs[g_cur.focus].box.y;
    *stamp = g_hover_stamp;
    return 1;
}

int hal_menu_focus_set(int hs)
{
    if (!g_live || hs < 0 || hs >= g_cur.nh) return 0;
    g_cur.focus = hs;
    g_focus_shown = 1;
    return 1;
}

/* PAUSE3: the pad / hover cursor's hotspot, for a recipe that shows its own selection from it (a touch-only page's planks) */
int hal_menu_focus_shown(int *hs)
{
    if (!g_live || !g_focus_shown || g_cur.focus < 0 || g_cur.focus >= g_cur.nh) return 0;
    *hs = g_cur.focus;
    return 1;
}

/* PAUSE2: the picture size of the decide in progress (up() runs inside it), for a recipe's idle cache warm-up */
int hal_menu_last_size(int *w, int *h)
{
    const MenuDecision &D = g_pub[g_pub_i];
    if (D.w < 8 || D.h < 8) return 0;
    *w = D.w; *h = D.h;
    return 1;
}

void hal_menu_pad(uint16_t *pressed, int host_back)
{
    host_menu_cursor_sound_flush();
    {   /* MENUS093E: SM64DS_MENU_INV_PROBE="bx,by[;bx,by...]@N": print where the touch inverse sends those picture pixels, on the Nth call with a menu live */
        static int rd = 0, at = 0, calls = 0;
        static char pts[256];
        if (!rd) {
            rd = 1;
            const char *e = getenv("SM64DS_MENU_INV_PROBE");
            if (e) { const char *a = strchr(e, '@'); at = a ? atoi(a + 1) : 1; strncpy(pts, e, sizeof pts - 1); char *q = strchr(pts, '@'); if (q) *q = 0; }
        }
        if (at > 0 && g_live && ++calls == at) {
            const char *s = pts;
            while (*s) {
                int bx = 0, by = 0, dsx = -1, dsy = -1, in = 0;
                if (sscanf(s, "%d,%d", &bx, &by) != 2) break;
                hal_menu_touch_inverse(bx, by, &dsx, &dsy, &in);
                fprintf(stderr, "[invprobe] picture (%d,%d) -> ds (%d,%d) in=%d\n", bx, by, dsx, dsy, in);
                s = strchr(s, ';'); if (!s) break; ++s;
            }
        }
    }       /* EXTRAS093: once per logic frame a menu is up (the scene runner has no window loop) */
    if (!g_live || !pressed || !g_cur.recipe) return;
    const MenuRecipe *R = g_cur.recipe;
    if (R->pad_ok && !R->pad_ok()) return;      /* the ROM is not reading the menu this frame */
    if (R->pad_map) { R->pad_map(pressed, host_back); return; }   /* TITLE1: a native recipe with its own key meaning */
    if ((R->flags & MENU_RF_PAD_B_BACK) && (*pressed & 0x0001)) {   /* PADMENUS1: pad B / the attack key is Back on a page whose ROM has no use for it */
        host_back = 1;
        *pressed = (uint16_t)(*pressed & ~0x0001u);
    }
    if ((R->flags & MENU_RF_START_CONFIRM) && (*pressed & MENU_PAD_START))   /* POLISH1A: Enter is the Start key; here it confirms like Space and pad A */
        *pressed = (uint16_t)((*pressed & ~(unsigned)MENU_PAD_START) | MENU_PAD_CONFIRM);
    int drv = R->driver;
    if (g_netplay && drv == MENU_DRV_ROM_CURSOR) drv = MENU_DRV_TOUCH;   /* never write local ROM state in a session */
    if (g_netplay && drv == MENU_DRV_MIXED) drv = MENU_DRV_TOUCH;
    /* the host back key (Backspace / the bound B-equivalent) presses the recipe's named back
       hotspot as a synthesised touch, whatever the driver: the ROM has no pad back */
    if (host_back && g_cur.back >= 0 && g_cur.back < g_cur.nh) {
        const MenuHotspot &H = g_cur.hs[g_cur.back];
        hal_menu_touch_queue_press(H.box.x + H.box.w / 2, H.box.y + H.box.h / 2);
    }
    if (drv == MENU_DRV_NATIVE) return;                                   /* the ROM reads the pad itself */
    uint16_t k = *pressed;
    int dir = -1;
    if (k & MENU_PAD_UP) dir = MENU_DIR_UP;
    else if (k & MENU_PAD_DOWN) dir = MENU_DIR_DOWN;
    else if (k & MENU_PAD_LEFT) dir = MENU_DIR_LEFT;
    else if (k & MENU_PAD_RIGHT) dir = MENU_DIR_RIGHT;

    if (drv == MENU_DRV_ROM_CURSOR) {
        if (dir >= 0 && R->rom_cursor) { int sel = 0; R->rom_cursor(dir, &sel); }
        k = (uint16_t)(k & ~0xf0u);
        if (k & MENU_PAD_CONFIRM) k = (uint16_t)((k & ~(unsigned)MENU_PAD_CONFIRM) | MENU_PAD_START);
        *pressed = k;
        return;
    }

    /* TOUCH, and MIXED (vertical keys go to the ROM cursor when the recipe has one) */
    int handled_dir = 0, owns = 0;
    if (drv == MENU_DRV_MIXED && R->rom_cursor) {
        /* PAUSE1 contract (status\PAUSE1.md): called every pad frame (dir -1 = none pressed); a nonzero
           return consumes the direction; owns = 1 means this page's plates are the ROM cursor, so A is
           the ROM's own confirm (Start), exactly as the ROM_CURSOR driver does it */
        const int r = R->rom_cursor(dir, &owns);
        if (dir >= 0 && r) handled_dir = 1;
    }
    if (dir >= 0 && !handled_dir) {
        if (!g_focus_shown) {          /* the first press only shows the cursor */
            g_focus_shown = 1;
            if (g_cur.focus < 0 && g_cur.nh > 0) g_cur.focus = 0;
        } else {
            const int before = g_cur.focus;
            menu_focus_move(&g_cur, dir);
            if (g_cur.focus != before) host_menu_cursor_sound();    /* a pad / key move on a host-driven page: the ROM never saw it */
        }
    }
    if ((k & MENU_PAD_CONFIRM) && owns) {
        k = (uint16_t)((k & ~(unsigned)MENU_PAD_CONFIRM) | MENU_PAD_START);
    } else if (k & MENU_PAD_CONFIRM) {
        if (g_focus_shown && g_cur.focus >= 0 && g_cur.focus < g_cur.nh) {
            const MenuHotspot &H = g_cur.hs[g_cur.focus];
            hal_menu_touch_queue_press(H.box.x + H.box.w / 2, H.box.y + H.box.h / 2);
        }
        k = (uint16_t)(k & ~(unsigned)MENU_PAD_CONFIRM);
    }
    k = (uint16_t)(k & ~0xf0u);          /* direction bits are the port's while a recipe is up */
    *pressed = k;
}

/* ====================================================================== */
/* MENU LAYOUT SELFTEST                                                   */
/* ====================================================================== */

namespace {

FILE *g_out;

struct Aspect { const char *name; int aw, ah; };
const Aspect kAspects[5] = { { "4:3", 4, 3 }, { "16:10", 16, 10 }, { "16:9", 16, 9 }, { "21:9", 21, 9 },
                             { "32:9", 32, 9 } };

void out(const char *s)
{
    fputs(s, stderr);
    if (g_out) fputs(s, g_out);
}

/* every hotspot reaches every other by directions alone */
int focus_connected(const MenuLayout *L, int *from, int *to)
{
    if (L->nh < 2) return 1;
    for (int a = 0; a < L->nh; ++a) {
        char seen[MENU_MAX_HOTSPOTS];
        int stack[MENU_MAX_HOTSPOTS], sp = 0;
        memset(seen, 0, sizeof seen);
        seen[a] = 1;
        stack[sp++] = a;
        while (sp) {
            const int c = stack[--sp];
            for (int d = 0; d < 4; ++d) {
                MenuLayout T = *L;
                T.focus = c;
                const int n = menu_focus_move(&T, d);
                if (n >= 0 && !seen[n]) { seen[n] = 1; stack[sp++] = n; }
            }
        }
        for (int b = 0; b < L->nh; ++b)
            if (!seen[b]) { *from = a; *to = b; return 0; }
    }
    return 1;
}

struct Result { int scale, corners, outside, hot, bounds, overlap, focus, exact; char why[96]; };

void check_layout(const MenuLayout *L, Result *R)
{
    R->scale = R->corners = R->outside = R->hot = R->bounds = R->overlap = R->focus = R->exact = 1;
    R->why[0] = 0;
    char *why = R->why;
    for (int i = 0; i < L->np; ++i) {
        const MenuPiece &P = L->p[i];
        const int *d = L->dest[i];
        if (P.src != SRC_TOP_HOSTROW && P.lden == 0 && !P.fixdest && !menu_scale_allowed(P.knum, P.kden)) {
            R->scale = 0; sprintf(why, "piece %d scale %d/%d", i, P.knum, P.kden);
        }
        if (d[2] < 1 || d[3] < 1 || d[0] < 0 || d[1] < 0 || d[0] + d[2] > L->w || d[1] + d[3] > L->h) {
            R->bounds = 0; sprintf(why, "piece %d dest (%d,%d,%d,%d) outside %dx%d", i, d[0], d[1], d[2], d[3], L->w, L->h);
        }
        if (P.src != SRC_TOP_HOSTROW && P.lden == 0 && !P.xfill && !P.fixdest) {
            /* the scale rule: dest size within one host pixel of the exact rational size */
            const double kn = P.knum ? P.knum : 1, kd = P.kden ? P.kden : 1, un = (double)L->unum / L->uden;
            const double ew = P.s.w * kn / kd * un, eh = P.s.h * kn / kd * un;
            const double cew = ew > L->w ? L->w : ew, ceh = eh > L->h ? L->h : eh;
            const double dw = d[2] - cew, dh = d[3] - ceh;
            if (dw >= 1.0 || dw <= -1.0 - (ew > L->w ? 1.0 : 0.0) || dh >= 1.0 || dh <= -1.0 - (eh > L->h ? 1.0 : 0.0)) {
                R->exact = 0; sprintf(why, "piece %d dest %dx%d vs exact %.2fx%.2f", i, d[2], d[3], ew, eh);
            }
        }
        if (P.xfill) { /* a fill is not a scaled copy: its width is fill_w units */ }
        if (P.touch && P.src != SRC_SUB && P.src != SRC_TOP_PIC && P.src != SRC_HOSTIMG) { R->corners = 0; sprintf(why, "piece %d touch but not SRC_SUB", i); }
        for (int j = i + 1; j < L->np; ++j) {
            if (P.bg || L->p[j].bg) continue;
            const int *e = L->dest[j];
            if (d[0] < e[0] + e[2] && e[0] < d[0] + d[2] && d[1] < e[1] + e[3] && e[1] < d[1] + d[3]) {
                R->overlap = 0; sprintf(why, "pieces %d and %d overlap", i, j);
            }
        }
        if (!P.touch || d[2] < 1 || d[3] < 1) continue;
        const int cx[4] = { d[0], d[0] + d[2] - 1, d[0], d[0] + d[2] - 1 };
        const int cy[4] = { d[1], d[1], d[1] + d[3] - 1, d[1] + d[3] - 1 };
        for (int c = 0; c < 4; ++c) {
            int pi = -1, sx, sy;
            const int in = menu_hit(L, cx[c], cy[c], &pi, &sx, &sy);
            if (!in || pi != i || sx < P.s.x || sx >= P.s.x + P.s.w || sy < P.s.y || sy >= P.s.y + P.s.h) {
                R->corners = 0; sprintf(why, "piece %d corner %d inverts to piece %d (%d,%d) in=%d", i, c, pi, sx, sy, in);
            }
        }
        const int ox[4] = { d[0] - 1, d[0] + d[2], d[0] + d[2] / 2, d[0] + d[2] / 2 };
        const int oy[4] = { d[1] + d[3] / 2, d[1] + d[3] / 2, d[1] - 1, d[1] + d[3] };
        for (int c = 0; c < 4; ++c) {
            int pi = -1, sx, sy;
            if (menu_hit(L, ox[c], oy[c], &pi, &sx, &sy)) {
                if (pi == i) { R->outside = 0; sprintf(why, "piece %d edge %d still inside", i, c); }
                continue;   /* another touch piece owns that pixel: not this piece's to disown */
            }
            int dsx, dsy, in, drag = -1;
            menu_inverse(L, ox[c], oy[c], &drag, &dsx, &dsy, &in);
            if (in) { R->outside = 0; sprintf(why, "piece %d edge %d reads in", i, c); }
        }
    }
    for (int k = 0; k < L->nh; ++k) {
        const MenuHotspot &H = L->hs[k];
        if (H.piece >= L->np || !L->p[H.piece].touch) { R->hot = 0; sprintf(why, "hotspot %d not on a touch piece", k); continue; }
        const MenuPiece &P = L->p[H.piece];
        if (H.box.x < P.s.x || H.box.y < P.s.y || H.box.x + H.box.w > P.s.x + P.s.w ||
            H.box.y + H.box.h > P.s.y + P.s.h) {
            R->hot = 0; sprintf(why, "hotspot %d box outside its piece source", k); continue;
        }
        int hx, hy, pi = -1, sx = -1, sy = -1;
        menu_hotspot_centre(L, k, &hx, &hy);
        const int in = menu_hit(L, hx, hy, &pi, &sx, &sy);
        if (!in || pi != H.piece || sx < H.box.x || sx >= H.box.x + H.box.w || sy < H.box.y || sy >= H.box.y + H.box.h) {
            R->hot = 0; sprintf(why, "hotspot %d centre (%d,%d) -> piece %d ds (%d,%d) in=%d", k, hx, hy, pi, sx, sy, in);
        }
    }
    int a = 0, b = 0;
    if (!focus_connected(L, &a, &b)) { R->focus = 0; sprintf(why, "hotspot %d cannot reach %d", a, b); }
}

struct GridSize { const char *name; int w, h; };

int run_grid(void)
{
    int fails = 0, cases = 0;
    static const int heights[2] = { 384, 768 };
    /* odd sizes: not a whole number of canvas units wide, odd heights, narrow windows */
    static const GridSize odd[5] = { { "1022x767", 1022, 767 }, { "1366x768", 1366, 768 }, { "1365x767", 1365, 767 },
                                     { "2560x1079", 2560, 1079 }, { "641x480", 641, 480 } };
    char line[256];
    out("MENU LAYOUT SELFTEST\n");
    out("recipe     h    aspect  size       var wide np nh  scale exact corner outsd hot  bounds overlap focus  result\n");
    for (int ri = 0; ri < g_menu_nrecipes; ++ri) {
        const MenuRecipe &Rc = g_menu_recipes[ri];
        if (Rc.nvariants == 0) {
            sprintf(line, "%-10s stub (no layout yet): skipped\n", Rc.name);
            out(line);
            continue;
        }
        for (int v = 1; v <= Rc.nvariants; ++v)
            for (int ci = 0; ci < 15; ++ci) {
                int w, h;
                const char *an;
                if (ci < 10) {
                    h = heights[ci / 5];
                    const Aspect &A = kAspects[ci % 5];
                    w = (h * A.aw * 2 + A.ah) / (A.ah * 2);
                    an = A.name;
                } else {
                    w = odd[ci - 10].w; h = odd[ci - 10].h; an = "odd";
                }
                static MenuLayout L;
                menu_layout_solve(&L, &Rc, v, w, h);
                Result R;
                check_layout(&L, &R);
                const int ok = L.np > 0 && R.scale && R.exact && R.corners && R.outside && R.hot && R.bounds && R.overlap && R.focus;
                ++cases;
                if (!ok) ++fails;
                sprintf(line, "%-10s %-4d %-7s %4dx%-5d %-3d %-4s %-2d %-2d  %-5s %-5s %-6s %-5s %-4s %-6s %-7s %-5s  %s%s%s\n",
                        Rc.name, h, an, w, h, v, menu_wide(&L) ? "wide" : "nrw", L.np, L.nh,
                        R.scale ? "ok" : "FAIL", R.exact ? "ok" : "FAIL", R.corners ? "ok" : "FAIL",
                        R.outside ? "ok" : "FAIL", R.hot ? "ok" : "FAIL", R.bounds ? "ok" : "FAIL",
                        R.overlap ? "ok" : "FAIL", R.focus ? "ok" : "FAIL", ok ? "PASS" : "FAIL", ok ? "" : " : ",
                        ok ? "" : R.why);
                out(line);
            }
    }
    sprintf(line, "grid: %d cases, %d failed\n", cases, fails);
    out(line);
    return fails;
}

uint32_t t_bm(uint32_t argb, uint8_t bm) { return bm ? (argb & 0xfefefeu) | (argb & 0xff000000u) : argb; }
int fn_enable_all(int) { return 1; }

int check(const char *what, int ok)
{
    char line[160];
    sprintf(line, "live: %-58s %s\n", what, ok ? "PASS" : "FAIL");
    out(line);
    return ok ? 0 : 1;
}

/* the frame path end to end: frame_begin, present, inverse, pad, queue, fallback, census, autocrop */
int run_live(void)
{
    int fails = 0;
    /* heap, not static: ~6.9 MB of .bss in the game exe pushed the image end past the budget
       (it must stay well below 0x02000000); the selftest owns these only while it runs */
    uint32_t *sub = (uint32_t *)calloc(256 * 192, sizeof(uint32_t));
    MenuTopCell *top = (MenuTopCell *)calloc(256 * 192, sizeof(MenuTopCell));
    uint32_t *pic = (uint32_t *)calloc(1024 * 768, sizeof(uint32_t));
    uint32_t *dst = (uint32_t *)calloc(1024 * 768, sizeof(uint32_t));
    if (!sub || !top || !pic || !dst) {
        free(sub); free(top); free(pic); free(dst);
        return check("selftest buffers allocate", 0);
    }
    const uint32_t kBack = 0xff102030u, kCol = 0xff336699u;
    for (int i = 0; i < 256 * 192; ++i) sub[i] = kCol;
    for (int y = 0; y < 192; ++y) for (int x = 0; x < 8; ++x) sub[y * 256 + x] = kBack;   /* keyed columns */
    for (int y = 0; y < 192; ++y)
        for (int x = 0; x < 256; ++x) {
            MenuTopCell &c = top[y * 256 + x];
            c.argb = 0;
            c.owner = 3;
            if (y >= 20 && y < 60 && x >= 30 && x < 90) { c.argb = 0xffeeeeefu; c.bm = 1; }   /* BG3, bm verdict set */
            if (y >= 20 && y < 60 && x >= 120 && x < 160) { c.argb = 0xffffffffu; c.owner = 0; } /* BG0: not in the piece's mask */
        }
    for (int i = 0; i < 1024 * 768; ++i) pic[i] = 0xff202020u;
    MenuSources S;
    memset(&S, 0, sizeof S);
    S.sub = sub; S.sub_backdrop = kBack; S.top2d = top; S.pic = pic;
    S.pic_stride = 1024; S.pic_w = 1024; S.pic_h = 768; S.pic_rx = 128; S.pic_ry = 0; S.pic_rw = 768; S.pic_rh = 768;
    S.bm_apply = t_bm;
    hal_menu_set_sources(&S);
    hal_menu_set_enable_fn(0);
    hal_menu_frame_begin(1024, 768, 1);
    fails += check("nothing enabled -> scene 1 not live (default build state)", !hal_menu_live());
    hal_menu_frame_begin(1024, 768, 3);
    fails += check("nothing enabled -> scene 3 not live", !hal_menu_live());
    hal_menu_set_enable_fn(fn_enable_all);
    hal_menu_frame_begin(1024, 768, MENU_SCENE_TEST);
    fails += check("TEST recipe up at 1024x768", hal_menu_live() == 1);
    for (int i = 0; i < 1024 * 768; ++i) dst[i] = 0xff808080u;
    hal_menu_present(dst, 1024, 768, MENU_STAGE_PRE_FADE);
    fails += check("wrong stage draws nothing", dst[0] == 0xff808080u);
    hal_menu_present(dst, 1024, 768, MENU_STAGE_POST_FADE);
    {
        int hx, hy;
        menu_hotspot_centre(&g_seen, 0, &hx, &hy);
        fails += check("hotspot 0 centre shows the sub colour", dst[hy * 1024 + hx] == kCol);
        const uint32_t dimmed = blend_ch(0xff808080u, 8, 0);
        fails += check("backdrop dimmed by evy 8 where no piece draws", dst[767 * 1024 + 1023] == dimmed);
        int dsx, dsy, in;
        hal_menu_touch_inverse(hx, hy, &dsx, &dsy, &in);
        const MenuHotspot &H = g_seen.hs[0];
        fails += check("inverse of hotspot 0 centre is inside its DS box",
                       in && dsx >= H.box.x && dsx < H.box.x + H.box.w && dsy >= H.box.y && dsy < H.box.y + H.box.h);
        hal_menu_touch_inverse(0, 0, &dsx, &dsy, &in);
        fails += check("a miss reads outside and is clamped to a piece", !in && dsx >= 0 && dsy >= 0);
        hal_menu_touch_end();
        /* keyed columns: p1's first source column is keyed backdrop, so dest x0 shows the dimmed backdrop */
        const int *d = g_seen.dest[1];
        fails += check("key_backdrop drops engine-B backdrop pixels", dst[(d[1] + d[3] / 2) * 1024 + d[0]] == dimmed);
        const int *e = g_seen.dest[0];   /* TEST p0: s = (0,16,256,48) */
        const int bxp = e[0] + (60 * e[2]) / 256, byp = e[1] + ((40 - 16) * e[3]) / 48;
        const int gxp = e[0] + (140 * e[2]) / 256;
        fails += check("TOP_2D: BG3 cell drawn with its bm verdict applied", dst[byp * 1024 + bxp] == 0xffeeeeeeu);
        fails += check("TOP_2D: a BG0 cell is outside the piece's layer mask", dst[byp * 1024 + gxp] == dimmed);
    }
    {
        uint16_t k = MENU_PAD_DOWN;
        hal_menu_pad(&k, 0);
        fails += check("first direction press only shows the cursor, bit stripped", (k & MENU_PAD_DOWN) == 0 && g_cur.focus == 0);
        k = MENU_PAD_RIGHT;
        hal_menu_pad(&k, 0);
        fails += check("right moves focus", g_cur.focus != 0 && g_cur.focus > 0);
        k = MENU_PAD_CONFIRM;
        hal_menu_pad(&k, 0);
        int x, y, down;
        fails += check("confirm queues a press and strips the bit", (k & MENU_PAD_CONFIRM) == 0 && g_q_state == 1);
        fails += check("queue refuses a second press in flight", hal_menu_touch_queue_press(1, 1) == 0);
        int ok = hal_menu_touch_queue_poll(&x, &y, &down) && down;
        ok = ok && hal_menu_touch_queue_poll(&x, &y, &down) && down;
        ok = ok && hal_menu_touch_queue_poll(&x, &y, &down) && !down;
        ok = ok && !hal_menu_touch_queue_poll(&x, &y, &down);
        fails += check("queue: press, press, release, idle", ok);
        hal_menu_touch_real_held(1);
        fails += check("queue refuses while a real stylus is held", hal_menu_touch_queue_press(5, 5) == 0);
        hal_menu_touch_real_held(0);
        fails += check("queue accepts again once released", hal_menu_touch_queue_press(5, 5) == 1);
        hal_menu_touch_real_held(1);
        fails += check("a real stylus cancels a queued press", hal_menu_touch_queue_poll(&x, &y, &down) == 0);
        hal_menu_touch_real_held(0);
    }
    {   /* census and autocrop on the TOP_2D source */
        MenuPiece P = menu_piece_make(SRC_TOP_2D, 0, 20, 256, 40, 0, 0, 0, 0, 1, 1);
        P.layers = MENU_L_BG3;
        MenuLayout L;
        menu_layout_begin(&L, 512, 384);
        int c[4];
        menu_add_piece(&L, P);
        fails += check("census: a rect holding the whole block reads 0", menu_seam_census(&L, 0, &S, c) == 0);
        L.p[0].s.x = 0; L.p[0].s.y = 30; L.p[0].s.w = 256; L.p[0].s.h = 100;   /* y 30 cuts the block, 60 is its edge */
        fails += check("census: a cut through the block counts > 0", menu_seam_census(&L, 0, &S, c) > 0);
        DsRect r;
        L.p[0].s.y = 0; L.p[0].s.h = 192;
        fails += check("autocrop finds the opaque bbox (30,20,60,40)",
                       menu_autocrop_rect(&L.p[0], &S, &r) && r.x == 30 && r.y == 20 && r.w == 60 && r.h == 40);
        L.p[0].s = r;
        fails += check("census: the autocropped rect reads 0", menu_seam_census(&L, 0, &S, c) == 0);
    }
    {   /* ASSET1: a 3D-keyed picture piece, a column fill and an OAM-range plane */
        static int g_oam_lo = -1, g_oam_hi = -1;
        struct F {
            static const uint32_t *sub_for(uint8_t) { return 0; }
            static const uint32_t *sub_for_oam(uint8_t, int lo, int hi)
            {
                g_oam_lo = lo; g_oam_hi = hi;
                static uint32_t plane[256 * 192];
                for (int i = 0; i < 256 * 192; ++i) plane[i] = 0xff445566u;
                return plane;
            }
        };
        uint8_t *cover = (uint8_t *)calloc(1024 * 768, 1);
        uint32_t *pic2 = (uint32_t *)calloc(1024 * 768, sizeof(uint32_t));
        uint32_t *d2 = (uint32_t *)calloc(1024 * 768, sizeof(uint32_t));
        if (cover && pic2 && d2) {
            for (int i = 0; i < 1024 * 768; ++i) { pic2[i] = 0xff112233u; d2[i] = 0xff000000u; }
            for (int y = 100; y < 200; ++y) for (int x = 100; x < 200; ++x) cover[y * 1024 + x] = 1;
            MenuSources S2 = S;
            S2.pic = pic2; S2.pic_cover = cover; S2.pic_stride = 1024; S2.pic_w = 1024; S2.pic_h = 768;
            S2.pic_rx = 0; S2.pic_ry = 0; S2.pic_rw = 1024; S2.pic_rh = 768;
            MenuLayout L;
            menu_layout_begin(&L, 1024, 768);
            MenuPiece kp = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            kp.keycover = 1;
            menu_add_piece(&L, kp);
            menu_layout_dest(&L);
            menu_piece_draw(&L, 0, &S2, d2, 1024, 768);
            fails += check("keycover: a covered pixel shows the picture", d2[150 * 1024 + 150] == 0xff112233u);
            fails += check("keycover: an uncovered pixel stays untouched (own transparency)", d2[50 * 1024 + 50] == 0xff000000u &&
                           d2[150 * 1024 + 250] == 0xff000000u && d2[250 * 1024 + 150] == 0xff000000u);
            S2.pic_cover = 0;
            for (int i = 0; i < 1024 * 768; ++i) d2[i] = 0xff000000u;
            menu_piece_draw(&L, 0, &S2, d2, 1024, 768);
            fails += check("keycover: no coverage plane draws nothing", d2[150 * 1024 + 150] == 0xff000000u);
            menu_layout_begin(&L, 1024, 768);
            MenuPiece xf = menu_piece_make(SRC_SUB, 8, 0, 1, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            xf.xfill = 1; xf.fill_w = 100; xf.bg = 1;
            menu_add_piece(&L, xf);
            menu_layout_dest(&L);
            S2.sub_for = 0;
            fails += check("xfill: dest is fill_w units wide", L.dest[0][2] == 400 && L.dest[0][3] == 768);
            for (int i = 0; i < 1024 * 768; ++i) d2[i] = 0xff000000u;
            menu_piece_draw(&L, 0, &S2, d2, 1024, 768);
            fails += check("xfill: every column of the fill reads the one source column", d2[300 * 1024] == kCol && d2[300 * 1024 + 399] == kCol &&
                           d2[300 * 1024 + 400] == 0xff000000u);
            menu_layout_begin(&L, 1024, 768);
            MenuPiece op = menu_piece_make(SRC_SUB, 0, 0, 256, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            op.layers = MENU_L_OBJ; op.oam_on = 1; op.oam_lo = 3; op.oam_hi = 5;
            menu_add_piece(&L, op);
            menu_layout_dest(&L);
            S2.sub_for = F::sub_for; S2.sub_for_oam = F::sub_for_oam;
            g_nplanes = 0;
            menu_piece_draw(&L, 0, &S2, d2, 1024, 768);
            fails += check("oam range: the host is asked for entries 3..5 and the piece draws that plane",
                           g_oam_lo == 3 && g_oam_hi == 5 && d2[300 * 1024 + 300] == 0xff445566u);
        } else {
            fails += check("ASSET1 selftest buffers allocate", 0);
        }
        free(cover); free(pic2); free(d2);
    }
    {   /* the checker itself must be able to fail: break a good layout three ways */
        static MenuLayout B;
        menu_layout_solve(&B, &g_menu_recipes[g_menu_nrecipes - 1], 1, 1024, 768);
        Result R;
        B.p[2].dy = -80;                      /* p2 rises under p1 */
        menu_layout_dest(&B);
        check_layout(&B, &R);
        fails += check("checker: overlapping pieces are caught", !R.overlap);
        menu_layout_solve(&B, &g_menu_recipes[g_menu_nrecipes - 1], 1, 1024, 768);
        for (int d = 0; d < 4; ++d) B.hs[3].nav[d] = -1;
        check_layout(&B, &R);
        fails += check("checker: an unreachable hotspot is caught", !R.focus);
        menu_layout_solve(&B, &g_menu_recipes[g_menu_nrecipes - 1], 1, 1024, 768);
        B.p[3].dx = 80;                       /* p3 pushed off the right edge */
        menu_layout_dest(&B);
        check_layout(&B, &R);
        fails += check("checker: a piece outside the picture is caught", !R.bounds);
        menu_layout_solve(&B, &g_menu_recipes[g_menu_nrecipes - 1], 1, 1024, 768);
        B.hs[0].box.x = 250;
        check_layout(&B, &R);
        fails += check("checker: a hotspot box outside its source is caught", !R.hot);
    }
    {   /* fallback in a one-screen scene with no recipe up */
        hal_menu_frame_begin(1024, 768, 6);
        fails += check("fallback live in scene 6 (VS lobby, no recipe yet) with OneScreenMenus on", hal_menu_live() == 1 && hal_menu_fallback_frames() == 1);
        fails += check("fallback has the top backdrop and one touch panel", g_cur.np == 2 && g_cur.p[1].touch && g_cur.p[0].bg);
        hal_menu_frame_begin(1024, 768, 2);
        fails += check("a non-menu scene is not live", !hal_menu_live());
    }
    {   /* P0COREFIX rows */
        int x, y, down;
        /* back key: a host flag, not a pad bit; punch (0x0001) is the ROM's own A and is left alone */
        hal_menu_frame_begin(1024, 768, MENU_SCENE_TEST);
        hal_menu_present(dst, 1024, 768, MENU_STAGE_POST_FADE);
        while (hal_menu_touch_queue_poll(&x, &y, &down)) {}
        uint16_t k = 0x0001;
        hal_menu_pad(&k, 0);
        fails += check("back: pad bit 0x0001 alone presses nothing and is not stripped", k == 0x0001 && g_q_state == 0);
        k = 0;
        hal_menu_pad(&k, 1);
        const MenuHotspot &BH = g_cur.hs[g_cur.back];
        fails += check("back: host flag queues a press at the back hotspot centre",
                       g_cur.back >= 0 && g_q_state == 1 && g_q_x == BH.box.x + BH.box.w / 2 && g_q_y == BH.box.y + BH.box.h / 2);
        while (hal_menu_touch_queue_poll(&x, &y, &down)) {}
        /* the drawn layout is invalidated on a recipe change */
        hal_menu_frame_begin(1024, 768, 6);   /* fallback replaces test */
        int dsx = 5, dsy = 5, in = 1;
        hal_menu_touch_inverse(512, 600, &dsx, &dsy, &in);
        fails += check("stale: new recipe's first tick does not invert through old rects", !in && dsx == -1 && dsy == -1);
        hal_menu_present(dst, 1024, 768, MENU_STAGE_POST_FADE);
        hal_menu_touch_inverse(512, 600, &dsx, &dsy, &in);
        fails += check("stale: after its first draw the new layout inverts", in == 1);
        /* a queued press dies when the menu closes */
        fails += check("close: press queues", hal_menu_touch_queue_press(9, 9) == 1);
        hal_menu_frame_begin(1024, 768, 2);
        fails += check("close: the queued press is cancelled with the menu", !hal_menu_live() && hal_menu_touch_queue_poll(&x, &y, &down) == 0);
        hal_menu_touch_inverse(512, 600, &dsx, &dsy, &in);
        fails += check("close: no inverse once closed", !in);
        /* exact scale: 128 wide at 2/3 on 1024x768 is 85.33 units = 341.33 px; 127 at 1/2 is 63.5 units = 254 px */
        MenuLayout T;
        menu_layout_begin(&T, 1024, 768);
        menu_add_piece(&T, menu_piece_make(SRC_SUB, 0, 0, 128, 96, MENU_AX_L, MENU_AY_T, 0, 0, 2, 3));
        menu_add_piece(&T, menu_piece_make(SRC_SUB, 0, 0, 127, 95, MENU_AX_R, MENU_AY_B, 0, 0, 1, 2));
        menu_layout_dest(&T);
        fails += check("scale: 128x96 at 2/3 on 1024x768 is 341 or 342 by 256", (T.dest[0][2] == 341 || T.dest[0][2] == 342) && T.dest[0][3] == 256);
        fails += check("scale: 127x95 at 1/2 is 254 by 190 (47.5 units)", T.dest[1][2] == 254 && T.dest[1][3] == 190 && T.dest[1][0] + 254 == 1024 && T.dest[1][1] + 190 == 768);
        /* FACE1: the 5/6 step is exact (256 at 5/6 on 1024x768 is 213.33 units = 853 or 854 px), and a fixdest piece keeps its host rect and inverts through it */
        menu_layout_begin(&T, 1024, 768);
        menu_add_piece(&T, menu_piece_make(SRC_SUB, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 5, 6));
        {
            MenuPiece fx = menu_piece_make(SRC_TOP_PIC, 0, 0, 256, 192, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            fx.fixdest = 1; fx.touch = 1; fx.fx = 100; fx.fy = 50; fx.fw = 400; fx.fh = 300;
            menu_add_piece(&T, fx);
        }
        menu_layout_dest(&T);
        fails += check("scale: 256x192 at 5/6 on 1024x768 is 853 or 854 by 640, centred", (T.dest[0][2] == 853 || T.dest[0][2] == 854) && T.dest[0][3] == 640 && T.dest[0][1] == 64);
        {
            int pi = -1, sx = 0, sy = 0;
            const int in = menu_hit(&T, 300, 200, &pi, &sx, &sy);
            fails += check("fixdest: a click inverts through the recipe's own host rect (400x300 over 256x192)", in && pi == 1 && sx == 128 && sy == 96 &&
                           T.dest[1][0] == 100 && T.dest[1][2] == 400);
        }
        menu_layout_begin(&T, 1022, 767);
        menu_add_piece(&T, menu_piece_make(SRC_SUB, 0, 0, 256, 192, MENU_AX_C, MENU_AY_M, 0, 0, 1, 1));
        menu_layout_dest(&T);
        fails += check("odd size: a full 256x192 piece on 1022x767 stays inside", T.dest[0][0] >= 0 && T.dest[0][0] + T.dest[0][2] <= 1022 && T.dest[0][1] + T.dest[0][3] <= 767);
        /* autocrop latches survive a resize before the next draw */
        hal_menu_frame_begin(1024, 768, MENU_SCENE_TEST_CROP);
        hal_menu_present(dst, 1024, 768, MENU_STAGE_POST_FADE);
        fails += check("autocrop: latched at open to (30,20,60,40)", g_dd.L.p[0].s.x == 30 && g_dd.L.p[0].s.y == 20 && g_dd.L.p[0].s.w == 60 && g_dd.L.p[0].s.h == 40);
        hal_menu_present(dst, 800, 600, MENU_STAGE_POST_FADE);   /* the picture changed under the frame */
        fails += check("autocrop: the resize re-solve draws with the latched rect",
                       g_seen.w == 800 && g_seen.p[0].s.x == 30 && g_seen.p[0].s.w == 60 && g_seen.p[0].s.h == 40 &&
                       g_seen.dest[0][2] >= 187 && g_seen.dest[0][2] <= 188);
        /* MENUSPLIT1: the next decide applies the draw's latch, and the decision carries it by value */
        hal_menu_frame_begin(1024, 768, MENU_SCENE_TEST_CROP);
        {
            MenuDecision cd;
            hal_menu_decision_copy(&cd);
            fails += check("decide applies the latch the draw published (decision copy)",
                           cd.live && (cd.latched & 1) && cd.L.p[0].s.x == 30 && cd.L.p[0].s.w == 60 &&
                           g_dd.L.p[0].s.x == 30);
        }
        hal_menu_frame_begin(1024, 768, 2);
    }
    hal_menu_set_enable_fn(0);
    hal_menu_set_sources(0);
    free(sub); free(top); free(pic); free(dst);
    return fails;
}

int selftest_all(void)
{
    const char *fn = getenv("SM64DS_MENU_SELFTEST_OUT");
    if (fn && *fn) g_out = fopen(fn, "w");
    int fails = run_grid();
    fails += run_live();
    char line[64];
    sprintf(line, "MENU LAYOUT SELFTEST: %s (%d failed)\n", fails ? "FAIL" : "PASS", fails);
    out(line);
    if (g_out) fclose(g_out);
    g_out = 0;
    return fails;
}

struct SelftestHook {
    SelftestHook()
    {
        const char *e = getenv("SM64DS_MENU_SELFTEST");
        if (e && *e && *e != '0') {
            const int f = selftest_all();
            fflush(stderr);
            exit(f ? 1 : 0);
        }
    }
} g_selftest_hook;

}  // namespace

int menu_selftest(void) { return selftest_all(); }
