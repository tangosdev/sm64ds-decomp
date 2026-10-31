/* pause_page.inc -- the new pause menu's drawing (run menu1, lane PAUSE3). Included at the end of hal/settings_page.cpp so it can use the page's own
   drawing helpers (the parchment sheet, wooden pieces, the DS lettering renderer) and nothing else; there is no ROM here.

   pause_page_draw is the draw callback of the pause recipe's one custom piece (MenuPiece.custom, hal/menu_compose.cpp). It reads the layout's
   facts blob (PauseFacts, hal/menu_pause_geo.h) and draws, in this order: the star tracker (wide: always; 4:3: while it is slid in), the dialog sheet
   of the save pages, the 4:3 edge tabs and the pointer beside the lit plank. The planks themselves are hostimg pieces the compose draws after this
   (they carry the hit boxes and the lit / pressed look). The settings panel is the page's own draw (settings_page_draw, the overlay).

   NOTE for the identifiers below: S and L are macros in this file (the page state and layout): never name a variable S or L here. */

namespace {

/* run hunt8, lane UIMOD1: the tracker's own small pictures when no enabled mod provides them (hd_ui/menu/pause3/): a code-drawn skin of flat shapes.
   Same footprint as the picture would have. light / shade nudge the colours the way blit_hi does. */
uint32_t plain_tint(uint32_t c, int light, int shade)
{
    int r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
    if (shade) { r = r * (255 - shade) / 255; g = g * (255 - shade) / 255; b = b * (255 - shade) / 255; }
    if (light) { r += light; g += light; b += light; if (r > 255) r = 255; if (g > 255) g = 255; if (b > 255) b = 255; }
    return (uint32_t)(r << 16 | g << 8 | b);
}

/* a five point star centred in the box: supersampled point-in-polygon coverage, r0 the outer radius in pixels */
void plain_star(const Surf &sf, float cx, float cy, float r0, uint32_t rgb, int alpha)
{
    const float r1 = r0 * 0.42f, kPi = 3.14159265f;
    float vx[10], vy[10];
    for (int i = 0; i < 10; ++i) {
        const float a = -kPi * 0.5f + i * kPi / 5.f, rr = (i & 1) ? r1 : r0;
        vx[i] = cx + cosf(a) * rr; vy[i] = cy + sinf(a) * rr;
    }
    const int R = (rgb >> 16) & 255, G = (rgb >> 8) & 255, B = rgb & 255;
    for (int y = (int)floorf(cy - r0) - 1; y <= (int)ceilf(cy + r0) + 1; ++y) {
        if (y < 0 || y >= sf.h) continue;
        for (int x = (int)floorf(cx - r0) - 1; x <= (int)ceilf(cx + r0) + 1; ++x) {
            if (x < 0 || x >= sf.w) continue;
            int hit = 0;
            for (int sy = 0; sy < 3; ++sy)
                for (int sx = 0; sx < 3; ++sx) {
                    const float px = x + (sx + 0.5f) / 3.f, py = y + (sy + 0.5f) / 3.f;
                    int in = 0;
                    for (int i = 0, j = 9; i < 10; j = i++)
                        if (((vy[i] > py) != (vy[j] > py)) && (px < (vx[j] - vx[i]) * (py - vy[i]) / (vy[j] - vy[i]) + vx[i])) in = !in;
                    hit += in;
                }
            const int c = hit * 255 / 9 * alpha / 255;
            if (!c) continue;
            uint32_t &o = sf.px[(size_t)y * sf.stride + x];
            o = over(o, R * c / 255, G * c / 255, B * c / 255, c);
        }
    }
}

void plain_hi(const Surf &sf, int id, int dx, int dy, int dw, int dh, int alpha, int light, int shade)
{
    const float x = (float)dx, y = (float)dy, w = (float)dw, h = (float)dh;
    switch (id) {
    case MENU_IMG_P_STAR_GOT: case MENU_IMG_P_STAR_OFF: {
        const bool got = id == MENU_IMG_P_STAR_GOT;
        plain_star(sf, x + w * 0.5f, y + h * 0.54f, w * 0.52f, plain_tint(0x8A5A10, light, shade), got ? alpha : alpha / 2);
        plain_star(sf, x + w * 0.5f, y + h * 0.54f, w * 0.42f, plain_tint(got ? 0xF7C81C : 0xE0CFA6, light, shade), got ? alpha : alpha / 2);
        break;
    }
    case MENU_IMG_P_COIN_GOLD: case MENU_IMG_P_COIN_RED: {
        const bool red = id == MENU_IMG_P_COIN_RED;
        fill_circle(sf, x + w * 0.5f, y + h * 0.5f, w * 0.5f, plain_tint(red ? 0x7A1010 : 0x8A5A10, light, shade), alpha, 0);
        fill_circle(sf, x + w * 0.5f, y + h * 0.5f, w * 0.43f, plain_tint(red ? 0xD83A2E : 0xF7C81C, light, shade), alpha, 0);
        fill_rrect(sf, x + w * 0.44f, y + h * 0.28f, w * 0.12f, h * 0.44f, w * 0.05f, plain_tint(red ? 0xFFF1E0 : 0x8A5A10, light, shade), alpha, 0);
        break;
    }
    case MENU_IMG_P_TITLE_BLANK:
        fill_rrect(sf, x, y, w, h, h * 0.28f, plain_tint(0x8A5A10, light, shade), alpha, 0);
        fill_rrect(sf, x + h * 0.07f, y + h * 0.07f, w - h * 0.14f, h * 0.86f, h * 0.22f, plain_tint(0xFFF3CD, light, shade), alpha, 0);
        break;
    case MENU_IMG_P_TAB_L: case MENU_IMG_P_TAB_R: {
        fill_rrect(sf, x, y, w, h, w * 0.18f, plain_tint(0x8A5A10, light, shade), alpha, 0);
        fill_rrect(sf, x + w * 0.06f, y + w * 0.06f, w * 0.88f, h - w * 0.12f, w * 0.14f, plain_tint(0xF0B850, light, shade), alpha, 0);
        const char *t = id == MENU_IMG_P_TAB_L ? "LB" : "RB";
        const float em = w * 0.34f, tw = ui_text_width(t, em);
        ui_text_draw(&sf, x + (w - tw) * 0.5f, y + h * 0.88f, t, em, plain_tint(0x7A3A0C, light, shade), alpha, 0);
        break;
    }
    default: break;
    }
}

/* a premultiplied host picture scaled to dw x dh blitted at (dx, dy); alpha 0..255 scales it, light adds white under its own alpha, shade darkens */
void blit_hi(const Surf &sf, int id, int dx, int dy, int dw, int dh, int alpha = 255, int light = 0, int shade = 0)
{
    if (dw < 1 || dh < 1) return;
    const uint32_t *src = menu_hostimg_scaled(id, dw, dh);
    if (!src) { plain_hi(sf, id, dx, dy, dw, dh, alpha, light, shade); return; }      /* no mod picture: the plain skin */
    for (int y = 0; y < dh; ++y) {
        const int yy = dy + y;
        if (yy < 0 || yy >= sf.h) continue;
        uint32_t *row = sf.px + (size_t)yy * sf.stride;
        const uint32_t *sp = src + (size_t)y * dw;
        int x0 = dx < 0 ? -dx : 0, x1 = dx + dw > sf.w ? sf.w - dx : dw;
        for (int x = x0; x < x1; ++x) {
            const uint32_t c = sp[x];
            int a = c >> 24;
            if (!a) continue;
            int r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255;
            if (shade) { r = r * (255 - shade) / 255; g = g * (255 - shade) / 255; b = b * (255 - shade) / 255; }
            if (light) { r += a * light / 255; g += a * light / 255; b += a * light / 255; if (r > a) r = a; if (g > a) g = a; if (b > a) b = a; }
            if (alpha < 255) { r = r * alpha / 255; g = g * alpha / 255; b = b * alpha / 255; a = a * alpha / 255; }
            uint32_t &o = row[dx + x];
            o = over(o, r, g, b, a);
        }
    }
}

const Piece *arrow_left_piece(int w, int h)
{
    Piece *p = find_piece(7, w, h, 0);
    if (p) return p;
    const Piece *r = arrow_right_piece(w, h);
    p = new_piece(7, w, h, 0, w, h);
    if (!r) return p;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) p->px[(size_t)y * w + x] = r->px[(size_t)y * w + (w - 1 - x)];
    return p;
}

const uint32_t PP_BROWN = 0x7A3A0C, PP_FADED = 0xA67C4A, PP_DKBROWN = 0x5C2C0A;

/* the course's title, shrunk to fit a plank */
void pp_title_text(const Surf &sf, int cx, int cy, int maxw, const char *t, double em)
{
    float e = (float)em;
    const float need = ui_text_width(t, e);
    if (need > maxw) e *= maxw / need;
    const float w = ui_text_width(t, e);
    ui_text_draw(&sf, cx - w * 0.5f, cy + ui_text_cap_height(e) * 0.5f, t, e, PP_BROWN, 255, 0);
}

void pp_star_row(const Surf &sf, int x, int y, int rowh, int icon, const char *name, int got, double sc, int maxw)
{
    blit_hi(sf, got ? MENU_IMG_P_STAR_GOT : MENU_IMG_P_STAR_OFF, x, y + (rowh - icon) / 2, icon, icon);
    float em = (float)(31 * sc);
    const float room = (float)(maxw - icon - 18 * sc);       /* a long name shrinks to fit the sheet */
    const float need = ui_text_width(name, em);
    if (need > room && room > 0) em *= room / need;
    ui_text_draw(&sf, (float)(x + icon + 18 * sc), y + rowh * 0.5f + ui_text_cap_height(em) * 0.5f, name, em, got ? PP_BROWN : PP_FADED, 255, 0);
}

void pp_stat_row(const Surf &sf, int x, int y, int w, int rowh, int icon_id, const char *label, const char *note, const char *val, double sc)
{
    const int ic = (int)(40 * sc + 0.5);
    blit_hi(sf, icon_id, x + (int)(70 * sc), y + (rowh - ic) / 2, ic, ic);
    const float em = (float)(33 * sc);
    ui_text_draw(&sf, (float)(x + 70 * sc + ic + 18 * sc), y + rowh * 0.5f + ui_text_cap_height(em) * 0.5f, label, em, PP_BROWN, 255, 0);
    if (note && note[0]) {
        const float ne = (float)(26 * sc);
        const float nw = ui_text_width(note, ne);
        ui_text_draw(&sf, x + w - 230 * sc - nw, y + rowh * 0.5f + ui_text_cap_height(ne) * 0.5f, note, ne, PP_FADED, 255, 0);
    }
    const float ve = (float)(36 * sc);
    const float vw = ui_text_width(val, ve);
    ui_text_draw(&sf, x + w - 70 * sc - vw, y + rowh * 0.5f + ui_text_cap_height(ve) * 0.5f, val, ve, PP_BROWN, 255, 0);
}

void pp_tracker(const Surf &sf, const PauseGeo &g, const PauseFacts *f)
{
    const double sc = g.tsc;
    const int dx = pause_tracker_dx(g, f);
    const int x = g.tx + dx, y = g.ty, w = g.tw, h = pause_tracker_h(g, f);
    if (x >= sf.w || x + w <= 0) return;
    const Piece *sheet = sheet_piece(w, h, (int)(sc * 100 + 0.5));
    if (sheet) blit_piece(sf, sheet, x, y);
    /* the title plank across the top edge */
    const int tw = (int)(640 * sc + 0.5), th = tw * 256 / 960;
    const int tcx = x + w / 2, tcy = y + (int)(8 * sc);
    blit_hi(sf, MENU_IMG_P_TITLE_BLANK, tcx - tw / 2, tcy - th / 2, tw, th);
    pp_title_text(sf, tcx, tcy, (int)(tw * 0.72), f->title, th * 0.40);
    /* the page row: the course arrows and the page name */
    const int row_cy = y + (int)((110 + 20) * sc + 0.5);
    {
        const float em = (float)(34 * sc);
        const float tw2 = ui_text_width(f->sub, em);
        ui_text_draw(&sf, x + w * 0.5f - tw2 * 0.5f, row_cy + ui_text_cap_height(em) * 0.5f, f->sub, em, PP_BROWN, 255, 0);
    }
    if (f->paging) {
        const int aw = (int)(28 * sc + 0.5), ah = (int)(40 * sc + 0.5);
        const Piece *al = arrow_left_piece(aw, ah), *ar = arrow_right_piece(aw, ah);
        if (al) blit(sf, al->px, al->w, al->h, x + (int)(104 * sc), row_cy - ah / 2, 255, 0, 0, 0);
        if (ar) blit(sf, ar->px, ar->w, ar->h, x + w - (int)(104 * sc) - aw, row_cy - ah / 2, 255, 0, 0, 0);
        /* CAREER1: no LB / RB letters here any more: the bumpers switch the pause pages; the D-pad and the arrows page the courses */
    }
    int yy = y + (int)((110 + 20 + 48) * sc + 0.5);
    const int rowh = (int)(54 * sc + 0.5), ic = (int)(40 * sc + 0.5);
    const int lx = x + (int)(70 * sc);
    char buf[40];
    if (f->course >= 15) {
        /* the castle's secret stars (EXTRAS093): the places the ROM names, one list of 15 short rows, a lit star and the number of stars
           collected there; a place with none yet is faded and says nothing. The ROM gives no per-place totals, so none is shown. The rows
           fill the height of nine ordinary ones (15 * 32 design units = 480, nine * 54 = 486) so the panel keeps its size. */
        const int rh = (int)(32 * sc + 0.5), sic = (int)(26 * sc + 0.5);
        const int rx = x + w - (int)(70 * sc);
        for (int i = 0; i < 15; ++i) {
            if (!f->secret_name[i][0]) continue;
            const int cy = yy + i * rh;
            const int got = f->secret_got[i];
            char cnt[8];
            snprintf(cnt, sizeof cnt, "%d", got);
            const float em = (float)(27 * sc);
            blit_hi(sf, got ? MENU_IMG_P_STAR_GOT : MENU_IMG_P_STAR_OFF, lx, cy + (rh - sic) / 2, sic, sic);
            ui_text_draw(&sf, (float)(lx + sic + 14 * sc), cy + rh * 0.5f + ui_text_cap_height(em) * 0.5f, f->secret_name[i], em, got ? PP_BROWN : PP_FADED, 255, 0);
            if (got) {
                const float cw = ui_text_width(cnt, em);
                ui_text_draw(&sf, rx - cw, cy + rh * 0.5f + ui_text_cap_height(em) * 0.5f, cnt, em, PP_BROWN, 255, 0);
            }
        }
        yy += 9 * rowh;
    } else {
        for (int i = 0; i < 7; ++i) {
            if (!f->names[i][0]) { yy += rowh; continue; }
            pp_star_row(sf, lx, yy, rowh, ic, f->names[i], (f->got_mask >> (i + 1)) & 1, sc, (int)(w - 70 * sc - 50 * sc));
            yy += rowh;
        }
        pp_star_row(sf, lx, yy, rowh, ic, "100 Coins", f->got_mask & 1, sc, (int)(w - 70 * sc - 50 * sc));
        yy += rowh;
    }
    yy += (int)(6 * sc);
    fill_rrect(sf, (float)(x + 60 * sc), (float)yy, (float)(w - 120 * sc), (float)(4 * sc > 3 ? 4 * sc : 3), 2.f, PP_DKBROWN, 255, 0);
    yy += (int)(18 * sc);
    const int sh = (int)(58 * sc + 0.5);
    if (f->course >= 15) {
        snprintf(buf, sizeof buf, "%d / 30", f->stars_got);
        pp_stat_row(sf, x, yy, w, sh, MENU_IMG_P_STAR_GOT, "Secret stars", 0, buf, sc);
        yy += sh;
        snprintf(buf, sizeof buf, "%d / 150", f->total_n);
        pp_stat_row(sf, x, yy, w, sh, MENU_IMG_P_STAR_GOT, "Total stars", 0, buf, sc);
    } else {
        snprintf(buf, sizeof buf, "%d / 8", f->stars_got);
        pp_stat_row(sf, x, yy, w, sh, MENU_IMG_P_STAR_GOT, "Stars", 0, buf, sc);
        yy += sh;
        if (f->in_course) {
            char note[24];
            snprintf(note, sizeof note, "record %d", f->coin_rec);
            snprintf(buf, sizeof buf, "%d", f->coins);
            pp_stat_row(sf, x, yy, w, sh, MENU_IMG_P_COIN_GOLD, "Coins", note, buf, sc);
            yy += sh;
            if (f->show_red) {
                snprintf(buf, sizeof buf, "%d / 8", f->red);
                pp_stat_row(sf, x, yy, w, sh, MENU_IMG_P_COIN_RED, "Red coins", 0, buf, sc);
            }
        } else {
            snprintf(buf, sizeof buf, "%d", f->coin_rec);
            pp_stat_row(sf, x, yy, w, sh, MENU_IMG_P_COIN_GOLD, "Coin record", 0, buf, sc);
        }
    }
}

/* CAREER1: THE CAREER PAGE. The tracker's sheet and title plank, then one row per save file: the file, its playtime and its clear time ("--"
   until the file has beaten the final Bowser). The file being played has the pointer beside it. The rest of the sheet is left for what comes
   later (achievements): nothing is drawn there. */
void pp_hms(uint32_t sec, char *out, int cap)
{
    snprintf(out, (size_t)cap, "%u:%02u:%02u", sec / 3600, sec / 60 % 60, sec % 60);
}

void pp_career(const Surf &sf, const PauseGeo &g, const PauseFacts *f)
{
    const PauseRect r = pause_career_rect(g, f);
    if (r.x >= sf.w || r.x + r.w <= 0) return;
    const double sc = g.tsc;
    const int x = r.x, y = r.y, w = r.w;
    const Piece *sheet = sheet_piece(w, r.h, (int)(sc * 100 + 0.5));
    if (sheet) blit_piece(sf, sheet, x, y);
    const int tw = (int)(640 * sc + 0.5), th = tw * 256 / 960;
    const int tcx = x + w / 2, tcy = y + (int)(8 * sc);
    blit_hi(sf, MENU_IMG_P_TITLE_BLANK, tcx - tw / 2, tcy - th / 2, tw, th);
    pp_title_text(sf, tcx, tcy, (int)(tw * 0.72), "CAREER", th * 0.40);
    /* the column heads, on the tracker's page-name row */
    const int head_cy = y + (int)((110 + 20) * sc + 0.5);
    const int c1 = x + (int)(500 * sc), c2 = x + w - (int)(70 * sc);      /* right edges of the two number columns */
    const float he = (float)(28 * sc);
    ui_text_draw(&sf, c1 - ui_text_width("Playtime", he), head_cy + ui_text_cap_height(he) * 0.5f, "Playtime", he, PP_FADED, 255, 0);
    ui_text_draw(&sf, c2 - ui_text_width("Clear time", he), head_cy + ui_text_cap_height(he) * 0.5f, "Clear time", he, PP_FADED, 255, 0);
    int yy = y + (int)((110 + 20 + 48) * sc + 0.5);
    const int rowh = (int)(64 * sc + 0.5);
    const int lx = x + (int)(110 * sc);
    for (int i = 0; i < 3; ++i) {
        const int cy = yy + rowh / 2;
        const int has = (f->career_has >> i) & 1, clr = (f->career_has >> (3 + i)) & 1;
        const uint32_t col = has ? PP_BROWN : PP_FADED;
        char name[12], play[24], clear[24];
        snprintf(name, sizeof name, "File %c", 'A' + i);
        if (has) pp_hms(f->career_play[i], play, sizeof play); else snprintf(play, sizeof play, "--");
        if (clr) pp_hms(f->career_clear[i], clear, sizeof clear); else snprintf(clear, sizeof clear, "--");
        const float ne = (float)(33 * sc), ve = (float)(34 * sc);
        if (i == f->career_cur) {
            const int ah = (int)(rowh * 0.5 + 0.5), aw = (int)(ah * 24.0 / 34.0 + 0.5);
            const Piece *ap = arrow_right_piece(aw, ah);
            if (ap) blit(sf, ap->px, ap->w, ap->h, lx - aw - (int)(16 * sc), cy - ah / 2, 255, 0, 0, 0);
        }
        ui_text_draw(&sf, (float)lx, cy + ui_text_cap_height(ne) * 0.5f, name, ne, col, 255, 0);
        ui_text_draw(&sf, c1 - ui_text_width(play, ve), cy + ui_text_cap_height(ve) * 0.5f, play, ve, col, 255, 0);
        ui_text_draw(&sf, c2 - ui_text_width(clear, ve), cy + ui_text_cap_height(ve) * 0.5f, clear, ve, clr ? PP_BROWN : PP_FADED, 255, 0);
        yy += rowh;
    }
    yy += (int)(10 * sc);
    fill_rrect(sf, (float)(x + 60 * sc), (float)yy, (float)(w - 120 * sc), (float)(4 * sc > 3 ? 4 * sc : 3), 2.f, PP_DKBROWN, 255, 0);
}

/* the save pages' sheet: title plank, the words, (the planks are the recipe's pieces on top) */
void pp_dialog(const Surf &sf, const PauseGeo &g, const PauseFacts *f)
{
    const PauseRect d = pause_dialog_rect(g, f, f->variant);
    if (d.x >= sf.w || d.x + d.w <= 0) return;
    const double u = g.u;
    const Piece *sheet = sheet_piece(d.w, d.h, (int)(u * 100 + 0.5));
    if (sheet) blit_piece(sf, sheet, d.x, d.y);
    const char *title = "SAVE";
    const char *l1 = "Save your progress", *l2 = "and quit to the title?";
    if (f->variant == PAUSE_V_STATUS) {
        title = f->status ? "SAVED" : "SAVING";
        l1 = f->status ? "File saved." : "Saving file...";
        l2 = f->status ? "Returning to the title..." : "Do not turn off the power.";
    } else if (f->variant == PAUSE_V_NOSAVE) {
        title = "QUIT";
        l1 = "Really quit without saving?";
        l2 = "Progress since your last save is lost.";
    }
    const int tw = (int)(380 * u + 0.5), th = tw * 256 / 960;
    const int tcx = d.x + d.w / 2, tcy = d.y + (int)(4 * u);
    blit_hi(sf, MENU_IMG_P_TITLE_BLANK, tcx - tw / 2, tcy - th / 2, tw, th);
    pp_title_text(sf, tcx, tcy, (int)(tw * 0.72), title, th * 0.40);
    const float em = (float)(30 * u);
    const int ty0 = d.y + (int)(118 * u);
    const float w1 = ui_text_width(l1, em);
    float e1 = em, e2 = (float)(27 * u);
    float n1 = ui_text_width(l1, e1);
    if (n1 > d.w - 60 * u) e1 *= (float)((d.w - 60 * u) / n1);
    (void)w1;
    ui_text_draw(&sf, tcx - ui_text_width(l1, e1) * 0.5f, ty0 + ui_text_cap_height(e1) * 0.5f, l1, e1, PP_BROWN, 255, 0);
    if (l2[0]) {
        const float n2 = ui_text_width(l2, e2);
        if (n2 > d.w - 60 * u) e2 *= (float)((d.w - 60 * u) / n2);
        ui_text_draw(&sf, tcx - ui_text_width(l2, e2) * 0.5f, ty0 + 44 * u + ui_text_cap_height(e2) * 0.5f, l2, e2, PP_FADED, 255, 0);
    }
    if (f->variant == PAUSE_V_STATUS && !f->status) {
        /* three dots chasing each other while the file is written */
        const int ph = (f->frame / 10) % 3;
        for (int i = 0; i < 3; ++i)
            fill_circle(sf, tcx + (i - 1) * 34 * (float)u, d.y + d.h - 60 * (float)u, (float)(7 * u), i == ph ? PP_BROWN : PP_FADED, 255, 0);
    }
}

/* EVENTS3: the running online event as a small card (the tracker's parchment) with its name and time left, one row per line; the race adds
   its leader. Under the plank stack. */
void pp_event_card(const Surf &sf, const PauseGeo &g, const PauseFacts *f, int cx, int top)
{
    if (f->event_n <= 0) return;
    const PauseRect r = pause_event_rect(g, f->event_n, cx, top);
    if (r.x >= sf.w || r.x + r.w <= 0 || r.y + r.h > sf.h) return;
    const Piece *sheet = sheet_piece(r.w, r.h, (int)(g.u * 100 + 0.5));
    if (sheet) blit_piece(sf, sheet, r.x, r.y);
    for (int i = 0; i < f->event_n && i < 2; ++i) {
        const char *s = f->event[i];
        float em = (float)(30 * g.u);
        const float need = ui_text_width(s, em), room = (float)(r.w - 70 * g.u);
        if (need > room && room > 0) em *= room / need;
        const float cy = r.y + (float)((13 + i * 42 + 21) * g.u);
        ui_text_draw(&sf, r.x + r.w * 0.5f - ui_text_width(s, em) * 0.5f, cy + ui_text_cap_height(em) * 0.5f, s, em, i == 0 ? PP_BROWN : PP_DKBROWN, 255, 0);
    }
}

/* the 4:3 edge tabs and the pointer beside the lit plank */
void pp_tabs(const Surf &sf, const PauseGeo &g, const PauseFacts *f)
{
    for (int side = 0; side < 2; ++side) {
        const PauseRect r = pause_tab_rect(g, side);
        const int hov = f->hover_tab == side, prs = f->press_tab == side;
        blit_hi(sf, side ? MENU_IMG_P_TAB_R : MENU_IMG_P_TAB_L, r.x + (prs ? (side ? -(int)(2 * g.u) : (int)(2 * g.u)) : 0), r.y, r.w, r.h, 255, hov ? 30 : 0, prs ? 25 : 0);
    }
}

void pp_pointer(const Surf &sf, const PauseGeo &g, const PauseFacts *f)
{
    if (f->sel_slot < 0 || f->sel_slot >= f->nslots || f->settings_open) return;
    PauseRect r;
    if (f->variant == PAUSE_V_CASTLE || f->variant == PAUSE_V_COURSE || f->variant == PAUSE_V_AIR) r = pause_plank_rect(g, f, f->nslots, f->sel_slot);
    else r = pause_dialog_plank(g, f, f->variant, f->nslots, f->sel_slot);
    const int ah = (int)(r.h * 0.5 + 0.5), aw = (int)(ah * 24.0 / 34.0 + 0.5);
    const Piece *ap = arrow_right_piece(aw, ah);
    if (!ap) return;
    static const int bob[8] = { 0, 1, 2, 3, 4, 3, 2, 1 };
    const int ax = r.x - aw - (int)(22 * g.u) + (int)(bob[(f->frame >> 1) & 7] * g.u * 0.5);
    blit(sf, ap->px, ap->w, ap->h, ax, r.y + r.h / 2 - ah / 2, 255, 0, 0, 0);
}

/* the reduced pause's Resume plank when no mod carries the planks: the ROM's own Continue plate is not on its bottom screen in state 0xb (the
   ROM leaves engine B's plate layers off there), so the plank is the plain skin's (the edge tabs' wood and lettering), lit, sinking when pressed */
void pp_air_plank(const Surf &sf, const PauseGeo &g, const PauseFacts *f)
{
    const PauseRect r = pause_plank_rect(g, f, 1, 0);
    const int shade = f->pressed ? 25 : 0, sink = f->pressed ? (int)(2 * g.u + 0.5) : 0;
    const float x = (float)r.x, y = (float)(r.y + sink), w = (float)r.w, h = (float)r.h;
    fill_rrect(sf, x, y, w, h, h * 0.28f, plain_tint(0x8A5A10, 30, shade), 255, 0);
    fill_rrect(sf, x + h * 0.08f, y + h * 0.08f, w - h * 0.16f, h * 0.84f, h * 0.22f, plain_tint(0xF0B850, 30, shade), 255, 0);
    const char *t = "Resume";
    const float em = h * 0.62f, tw = ui_text_width(t, em);
    ui_text_draw(&sf, x + (w - tw) * 0.5f, y + h * 0.5f + ui_text_cap_height(em) * 0.5f, t, em, plain_tint(0x7A3A0C, 0, shade), 255, 0);
}

}  // namespace

extern "C" void pause_page_draw(const MenuLayout *lay, int piece, uint32_t *dst, int w, int h, int stride)
{
    (void)piece;
    const PauseFacts *f0 = (const PauseFacts *)lay->facts.blob;
    if (f0->magic != PAUSE_FACTS_MAGIC || w < 64 || h < 64) return;
    const PauseFacts *f = f0;
    const Surf sf = { dst, stride, w, h };
    const PauseGeo g = pause_geo(w, h, f, f->nslots);
    const int dialog = f->variant == PAUSE_V_SAVE || f->variant == PAUSE_V_STATUS || f->variant == PAUSE_V_NOSAVE;
    const int main_page = f->variant == PAUSE_V_CASTLE || f->variant == PAUSE_V_COURSE;
    int mvis;
    const int moff = main_page ? pause_page_off(f, PAUSE_PG_MAIN, &mvis) : (mvis = 1, 0);
    if (main_page ? (g.wide ? mvis : pause_page_visible(f, PAUSE_PG_TRACKER)) : g.wide) pp_tracker(sf, g, f);
    if (main_page && pause_page_visible(f, PAUSE_PG_CAREER)) pp_career(sf, g, f);
    if (dialog) pp_dialog(sf, g, f);
    if (main_page && f->event_n > 0 && !f->settings_open) {
        /* EVENTS3: under the plank stack (it slides away with the planks) */
        const PauseRect last = pause_plank_rect(g, f, 3, 2);
        if (mvis) pp_event_card(sf, g, f, g.pcx + pause_planks_dx(g, f), last.y + last.h + (int)(30 * g.u));
    }
    if (main_page && !f->settings_open) pp_tabs(sf, g, f);
    if (f->variant == PAUSE_V_AIR && f->rom_art) pp_air_plank(sf, g, f);
    if (mvis && moff <= 500 && moff >= -500) pp_pointer(sf, g, f);
}

/* CAREER1: the two-screen pause's Career page (OneScreenMenus off: hal/menu_recipe_pause.cpp menu_pause_rom_career_draw): the sheet slid to its
   place over the upper screen and the wooden edge tabs on that screen's sides. The ROM's own pause stays as it is under it. */
extern "C" void pause_page_rom_draw(const PauseFacts *f, uint32_t *px, int stride, int w, int h)
{
    if (!f || f->magic != PAUSE_FACTS_MAGIC || !px || w < 64 || h < 64) return;
    const Surf sf = { px, stride, w, h };
    const PauseGeo g = pause_geo(w, h, f, 3);
    if (pause_page_visible(f, PAUSE_PG_CAREER)) pp_career(sf, g, f);
    pp_tabs(sf, g, f);
}

/* the pictures the pause draws itself, for the warm-up to decode and scale ahead of the first open: idx 0.. -> (id, dw, dh), 1 while idx is valid */
extern "C" int pause_page_art(int w, int h, int idx, int *id, int *dw, int *dh)
{
    if (w < 64 || h < 64) return 0;
    PauseFacts f;
    memset(&f, 0, sizeof f);
    f.variant = PAUSE_V_CASTLE;
    const PauseGeo g = pause_geo(w, h, &f, 3);
    const double sc = g.tsc;
    const int ic = (int)(40 * sc + 0.5), gic = (int)(44 * sc + 0.5), tw = (int)(640 * sc + 0.5), dtw = (int)(380 * g.u + 0.5);
    struct It { int id, w, h; } it[] = {
        { MENU_IMG_P_TITLE_BLANK, tw, tw * 256 / 960 }, { MENU_IMG_P_STAR_GOT, ic, ic }, { MENU_IMG_P_STAR_OFF, ic, ic },
        { MENU_IMG_P_COIN_GOLD, ic, ic }, { MENU_IMG_P_COIN_RED, ic, ic }, { MENU_IMG_P_STAR_GOT, gic, gic }, { MENU_IMG_P_STAR_OFF, gic, gic },
        { MENU_IMG_P_TITLE_BLANK, dtw, dtw * 256 / 960 }, { MENU_IMG_P_TAB_L, g.tabw, g.tabh }, { MENU_IMG_P_TAB_R, g.tabw, g.tabh },
    };
    if (idx < 0 || idx >= (int)(sizeof it / sizeof it[0])) return 0;
    *id = it[idx].id; *dw = it[idx].w; *dh = it[idx].h;
    return 1;
}
