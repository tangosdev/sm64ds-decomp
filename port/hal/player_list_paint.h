// THE PLAYER LIST, the painter half. Run mp1 wave 56, lane PLIST1.
// Compiled ONLY inside tests/walk_window.cpp, after hal/net_hud.h (it draws
// through the chat's glyphs and the overlay surface, and uses the same
// hal_sub_screen_inset_rect). Model, rules and the click API: hal/player_list.h.
//
// WHERE IT SITS, in 512 x 384 units times OVL_SCALE (the same scale the ROM's
// HUD sprites take, so RenderScale 1..3 and both layouts agree):
//   x = 4, y = 48: under the lives row (y 5..38) and the health meter (to
//   y 42). Rows are (font + 2) tall, up to four. The chat log is bottom-left
//   and the ping box bottom-right, so the top-left strip is free; a row that
//   would touch the corner map panel is not drawn and is not clickable.
//   THE CHAT RISES FROM THE BOTTOM and is only capped by the picture's top
//   (six messages of up to three rows each), so a long burst of long lines
//   can climb into this strip. The list is drawn BEFORE the chat so that, in
//   that rare ten seconds, the chat stays readable on top of it.
// Draw-only: it writes overlay pixels after the game has finished the frame.

#ifndef PORT_HAL_PLAYER_LIST_PAINT_H
#define PORT_HAL_PLAYER_LIST_PAINT_H

#ifdef OVL_SCALE

extern "C" const char *port_fs_catalog_path(unsigned file_id);   /* hal/fs.cpp */
#include "hal/head_icons.h"
#include "hal/player_list.h"

/* A FAT file id -> the readable file under the asset root, or null. The same
   place hal/fs.cpp reads (<root>/extracted/dsd/files/<catalog path>); with no
   SM64DS_ASSET_ROOT there is nothing to look under and the hand-made heads stay. */
static const char *plist_rom_path(unsigned id)
{
    static char buf[4][600];
    static int k;
    const char *rel = port_fs_catalog_path(id);
    const char *root = getenv("SM64DS_ASSET_ROOT");
    if (!rel || !root || !*root) return 0;
    char *b = buf[k++ & 3];
    snprintf(b, 600, "%s/extracted/dsd/files/%s", root, rel);
    return b;
}

static uint32_t plist_readable(uint32_t rgb)
{
    unsigned r = (rgb >> 16) & 255, g = (rgb >> 8) & 255, b = rgb & 255;
    const unsigned lum = (r * 30 + g * 59 + b * 11) / 100;
    if (lum < 110) {   /* lift a dark pick toward white so it reads on the picture */
        const unsigned k = 110 - lum;
        r += (255 - r) * k / 160; g += (255 - g) * k / 160; b += (255 - b) * k / 160;
    }
    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

static void plist_head(const OvlSurface &fb, int x0, int y0, int scale,
                       const uint32_t *px, int alpha)
{
    const int S = port::kHeadIconSize;
    for (int j = 0; j < S; ++j)
        for (int i = 0; i < S; ++i) {
            const uint32_t p = px[j * S + i];
            const int pa = (int)(p >> 24);
            if (!pa) continue;
            const int a = pa * alpha / 255;
            for (int sy = 0; sy < scale; ++sy)
                for (int sx = 0; sx < scale; ++sx) {
                    const int qx = x0 + i * scale + sx, qy = y0 + j * scale + sy;
                    if (qx < 0 || qx >= ntr::active_w || qy < 0 ||
                        qy >= ntr::active_h)
                        continue;
                    const size_t idx = (size_t)qy * (size_t)fb.stride + qx;
                    fb.px[idx] = nt_blend(fb.px[idx], 0xFF000000u | p, a);
                }
        }
}

static void plist_pointer(void)
{
    /* user32 is bound by hand in this file (the W table), so the two calls the
       hover needs are bound the same way, once */
    typedef HWND (WINAPI *GetActiveWindowFn)(void);
    typedef BOOL (WINAPI *ScreenToClientFn)(HWND, POINT *);
    static GetActiveWindowFn gaw;
    static ScreenToClientFn stc;
    static int bound;
    if (!bound) {
        bound = 1;
        if (HMODULE u = GetModuleHandleA("user32.dll")) {
            gaw = (GetActiveWindowFn)GetProcAddress(u, "GetActiveWindow");
            stc = (ScreenToClientFn)GetProcAddress(u, "ScreenToClient");
        }
    }
    port::player_list_set_pointer(-1, -1);
    if (!port::player_list_has_click_handler() || !W.GetCursorPos_ || !gaw ||
        !stc)
        return;
    HWND h = gaw();
    POINT p;
    if (!h || !W.GetCursorPos_(&p) || !stc(h, &p)) return;
    int fx = 0, fy = 0;
    if (hal_present_client_to_fb((int)p.x, (int)p.y, &fx, &fy))
        port::player_list_set_pointer(fx, fy);
}

/* STARPOP3: the bottom and right edges of the rows drawn this frame (0 = none), so the star popup drawn after it can sit
   below the list instead of over it. */
static int g_plist_drawn_bottom, g_plist_drawn_right;

static void player_list_draw(const OvlSurface &fb)
{
    port::player_list_layout_begin();
    g_plist_drawn_bottom = 0; g_plist_drawn_right = 0;
    port::PlayerListView v;
    port::player_list_view(&v);
    if (v.n <= 0) return;
    {   /* the ROM's own portraits, once the game files can be read; retried a few times */
        static int tries, got;
        if (got < 4 && tries < 6) {
            ++tries;
            got = port::head_icons_load_rom(plist_rom_path);
            fprintf(stderr, "[plist] head art: %d of 4 from the ROM (try %d)\n", got, tries);
        }
    }
    plist_pointer();
    const int rom = chat_font_build();
    const int scale = OVL_SCALE;
    const int row_h = rom ? (NT_GH + 2) * scale : OVL_LINE;
    const int head = port::kHeadIconSize * scale;
    const int x0 = 4 * scale;
    int y = 48 * scale;
    int panel[4] = {0, 0, 0, 0};
    const int have_panel = hal_sub_screen_inset_rect(panel);

    /* the widest name and code, so every row shares its two columns */
    int name_w = 0, code_w = 0;
    for (int i = 0; i < v.n; ++i) {
        char nm[port::kPlistNameShown + 1];
        strncpy(nm, v.row[i].name, port::kPlistNameShown);
        nm[port::kPlistNameShown] = 0;
        int w = 0;
        for (const char *p = nm; *p; ++p) w += chat_adv((unsigned char)*p, rom, scale);
        if (w > name_w) name_w = w;
        w = 0;
        for (const char *p = v.row[i].code; *p; ++p) w += chat_adv((unsigned char)*p, rom, scale);
        if (w > code_w) code_w = w;
    }
    const int gap = 3 * scale;
    const int row_w = 2 * gap + head + gap + name_w + 2 * gap + code_w +
                      (4 * chat_adv('W', rom, scale)) + gap;
    for (int i = 0; i < v.n; ++i) {
        const port::PlayerListRow &r = v.row[i];
        const int ry = y;
        y += row_h + scale;
        const int w = row_w;
        if (have_panel && ry < panel[3] && ry + row_h > panel[1] &&
            x0 < panel[2] && x0 + w > panel[0])
            continue;                                   /* never over the corner map */
        const bool hot = port::player_list_hovered(r.seat);
        ovl_shade(fb, x0, ry, w, row_h);
        if (ry + row_h > g_plist_drawn_bottom) g_plist_drawn_bottom = ry + row_h;
        if (x0 + w > g_plist_drawn_right) g_plist_drawn_right = x0 + w;
        if (hot) ovl_shade(fb, x0, ry, w, row_h);       /* deeper shade = hover */
        if (!r.me) port::player_list_layout_row(r.seat, x0, ry, w, row_h);   /* your own row is not a button */
        const int alpha = r.same_place ? 255 : 170;
        const int hx = x0 + gap, hy = ry + (row_h - head) / 2;
        plist_head(fb, hx, hy, scale, port::head_icon(r.character), alpha);
        char nm[port::kPlistNameShown + 1];
        strncpy(nm, r.name, port::kPlistNameShown);
        nm[port::kPlistNameShown] = 0;
        const uint32_t ncol = r.me ? 0xFFD8D8D8u : plist_readable(r.body_rgb);
        const uint32_t ccol = r.in_level ? 0xFFFFFFFFu : 0xFFB0B0B0u;
        const int nx = hx + head + gap;
        const int cx = nx + name_w + 2 * gap;
        const int ty = ry + scale;
        if (rom) {
            chat_text(fb, nx, ty, nm, (int)strlen(nm), rom, scale, ncol, 0, alpha);
            chat_text(fb, cx, ty, r.code, (int)strlen(r.code), rom, scale, ccol, 0, alpha);
        }
        chat_text(fb, nx, ty, nm, (int)strlen(nm), rom, scale, ncol, 1, alpha);
        chat_text(fb, cx, ty, r.code, (int)strlen(r.code), rom, scale, ccol, 1, alpha);
        /* the TP badge is a button: never dimmed */
        if (r.badge[0]) {
            const int bx = cx + code_w + 2 * gap;
            if (rom) chat_text(fb, bx, ty, r.badge, (int)strlen(r.badge), rom, scale, 0xFFFFE070u, 0, 255);
            chat_text(fb, bx, ty, r.badge, (int)strlen(r.badge), rom, scale, 0xFFFFE070u, 1, 255);
        }
    }
}
#endif  // OVL_SCALE

#endif  // PORT_HAL_PLAYER_LIST_PAINT_H
