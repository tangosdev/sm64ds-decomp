/* menu_pause_geo.h -- the new pause menu's facts and geometry (run menu1, lane PAUSE3).

   Header only, no ROM, no window. Shared by the recipe (hal/menu_recipe_pause.cpp: solves the plank rects and the hit boxes) and the drawing
   (hal/settings_page.cpp via hal/pause_page.inc: the star tracker, the dialogs and the edge tabs), so the two can never disagree about where a
   thing is. Everything is a function of the picture size, the facts of this tick and the slide.

   FACTS. PauseFacts is the recipe's MenuFacts.blob: plain data the logic thread read from the ROM once this tick (menu_recipe_pause.cpp
   pause_facts). build() and the custom draw see it through the layout (L->facts.blob) and nothing else: no ROM address is read on the draw
   side (MENUSPLIT1).

   LAYOUT. Two layouts, picked by the picture's canvas width (MENU_WIDE_CANVAS, 16:9 and wider = wide):
     WIDE   unit u = h / 1080. The star tracker at the left (840 u wide), the planks at the right (560 u wide), nothing slides.
     NARROW unit u = w / 1024 (4:3). The planks alone at the middle (480 u wide); the star tracker is the other page: LB / RB / Q / E / the edge
            tabs slide planks and tracker (the planks or the open settings panel leave in the bumper's direction, the tracker comes in from
            the other side).
   The slide is one number, slide_t1000 (0 = the planks page, 1000 = the tracker page, eased already) and a direction dir (+1: the planks leave to
   the right, the tracker arrives from the left). */
#ifndef MENU_PAUSE_GEO_H
#define MENU_PAUSE_GEO_H
#include <stdint.h>

enum {
    PAUSE_V_CASTLE = 1, PAUSE_V_COURSE = 2, PAUSE_V_SAVE = 6, PAUSE_V_STATUS = 7, PAUSE_V_NOSAVE = 8,
    PAUSE_V_AIR = 10          /* the ROM's reduced course pause (PS state 0xb): the Resume plank alone */
};

#define PAUSE_FACTS_MAGIC 0x50415533

enum { PAUSE_PG_MAIN = 0, PAUSE_PG_TRACKER = 1, PAUSE_PG_CAREER = 2 };      /* CAREER1: the pause pages */

struct PauseFacts {
    int32_t magic;            /* PAUSE_FACTS_MAGIC once the recipe filled it (a zeroed blob means "no facts", e.g. the layout selftest) */
    int32_t variant;          /* PAUSE_V_* */
    int32_t slide_t1000;      /* narrow only: 0 planks page .. 1000 tracker page (already eased) */
    int32_t dir;              /* +1 / -1: the side the planks leave to */
    int32_t settings_open;    /* the settings panel replaces the planks (the page draws it; the recipe draws no planks) */
    int32_t in_course;        /* the pause is inside a course (Exit Course exists, no course paging) */
    int32_t paging;           /* the castle card pages courses (the ROM's arrows are live) */
    int32_t course;           /* the tracker's course 0..14, 15 = the castle's secret stars */
    int32_t sel_slot, nslots; /* the lit plank (a slot of the stack) and how many planks the page has, -1 / 0 = none */
    int32_t pressed;          /* the lit plank was just confirmed (sinks) */
    int32_t frame;            /* a tick counter (the pointer's bob) */
    int32_t got_mask;         /* bit k = the ROM's star bit k of this course is collected; bit 0 is the 100 coin star, bit k names star k-1 */
    int32_t stars_got;        /* collected stars of this course (or of the castle secrets) */
    int32_t coins, coin_rec;  /* the coins of this run (in a course) and the course's coin record */
    int32_t red, show_red;    /* red coins collected this run; shown once there is one */
    int32_t secret_n, total_n;/* secret stars collected, all stars collected */
    int32_t status;           /* PAUSE_V_STATUS: 0 saving, 1 saved */
    int32_t hover_tab;        /* the 4:3 edge tab under the pointer: -1 none, 0 left, 1 right */
    int32_t press_tab;        /* the edge tab held down: -1 none, 0, 1 */
    int32_t arrows_live;      /* the tracker's course arrows can be clicked now (the tracker is fully shown) */
    int32_t rom_art;          /* PAUSEFULL1: no mod planks, the planks are the ROM's own plates (240 x 32 DS pixels, 15:2) moved onto the slots */
    char title[40];           /* the course title, upper case, as the ROM's text */
    char sub[20];             /* "Course 3" / "Peach's Castle" */
    char names[7][38];        /* the seven star names of the course, title case */
    /* EXTRAS093: the castle's secret stars as a list. Place i is the ROM's course 15 + i (message ids 0x1a5 + i, the course titles the ROM
       shows for them), 15 places; secret_got[i] is how many stars of that place are collected (its save bits), secret_name[i] the place in
       title case ("" when the message bank gave none). The ROM does not say how many stars each place holds, so only the collected count is
       shown, never a total per place. */
    uint8_t secret_got[15];
    char secret_name[15][28];
    /* EVENTS3: the live online event's rows ("CASTLE UNLOCKED 4:32", a race's leader), 0 rows when nothing runs */
    int32_t event_n;
    char event[2][44];
    /* CAREER1: THE PAUSE PAGES. pg_page is the page at rest (or the one a slide is going to): PAUSE_PG_MAIN (wide: the tracker and the planks
       together; 4:3: the planks), PAUSE_PG_TRACKER (4:3 only), PAUSE_PG_CAREER. A slide runs pg_from -> pg_to (equal at rest): pg_e is its eased
       progress on this tick, pg_e0 on the tick before (permille), pg_dir the side the leaving page goes to (+1 right). pg_a is the place between
       ticks of the picture being drawn (permille, 1000 = the tick's own picture): the recipe fills 1000, the draw sets its own copy. */
    int16_t pg_page, pg_from, pg_to, pg_dir, pg_e, pg_e0, pg_a;
    /* the Career page: per save file, playtime and clear time in seconds; bit i of career_has = file i has a record, bit 3 + i = it has a clear
       time; career_cur = the file being played (-1 none) */
    int8_t career_cur, career_has;
    uint32_t career_play[3], career_clear[3];
};
static_assert(sizeof(PauseFacts) <= 1024, "PauseFacts must fit MenuFacts.blob");

struct PauseRect { int x, y, w, h; };

struct PauseGeo {
    int w, h, wide;
    double u;                     /* host pixels per design unit */
    /* the plank stack (n planks, the page's own pitch) */
    int pw, ph, pgap;             /* one plank's size and the gap */
    int pcx, pcy;                 /* the stack's centre */
    /* the tracker panel: sc = host pixels per design unit of the tracker */
    double tsc;
    int tx, ty, tw;               /* its box, th below */
    /* the edge tabs (narrow) */
    int tabw, tabh, tabcy;
};

/* the tracker's design height, in design units, for a page with `rows` rows of content and `stats` stat rows */
inline int pause_tracker_units_h(int rows, int stats) { return 110 + 70 + rows * 54 + 22 + stats * 58 + 40; }

inline int pause_round(double v) { return (int)(v + 0.5); }

/* rows / stats of the tracker page for these facts */
inline void pause_tracker_shape(const PauseFacts *f, int *rows, int *stats)
{
    if (f->course >= 15) { *rows = 9; *stats = 2; return; }                 /* the secret stars: 15 short rows (the height of nine ordinary ones), two totals */
    *rows = 8;                                                              /* seven named stars and the 100 coin star */
    *stats = 2 + (f->in_course ? (f->show_red ? 1 : 0) : 0);                /* stars, coins (or the record), red coins */
}

inline PauseGeo pause_geo(int w, int h, const PauseFacts *f, int nplanks)
{
    PauseGeo g;
    g.w = w; g.h = h;
    g.wide = (long long)w * 192 / h >= 300;
    int rows = 8, stats = 3;
    pause_tracker_shape(f, &rows, &stats);
    const int thu = pause_tracker_units_h(rows, stats);
    if (g.wide) {
        g.u = h / 1080.0;
        g.pw = pause_round(560 * g.u);
        g.pgap = pause_round(26 * g.u);
        g.pcx = w - (int)(500 * g.u > 0.26 * w ? 500 * g.u : 0.26 * w);
        g.pcy = h / 2 + pause_round(20 * g.u);
        g.tsc = g.u;
        g.tw = pause_round(840 * g.tsc);
        g.tx = (int)(100 * g.u > 0.05 * w ? 100 * g.u : 0.05 * w);
    } else {
        g.u = w / 1024.0;
        g.pw = pause_round(480 * g.u);
        g.pgap = pause_round(26 * g.u);
        g.pcx = w / 2;
        g.pcy = h / 2 + pause_round(8 * g.u);
        double sc = 0.80 * w / 840.0;
        const double sh = 0.90 * h / pause_tracker_units_h(8, 3);      /* the tallest page sets the scale, so the art keeps one size across pages */
        if (sh < sc) sc = sh;
        g.tsc = sc;
        g.tw = pause_round(840 * g.tsc);
        g.tx = (w - g.tw) / 2;
    }
    g.ph = pause_round(g.pw * 129.0 / 606.0);
    g.ty = (h - pause_round(thu * g.tsc)) / 2 + pause_round(g.wide ? 10 * g.u : 14 * g.u);
    g.tabw = pause_round(96 * g.u);
    g.tabh = pause_round(170 * g.u);
    g.tabcy = h / 2 + pause_round(8 * g.u);
    (void)nplanks;
    return g;
}

inline int pause_tracker_h(const PauseGeo &g, const PauseFacts *f)
{
    int rows, stats;
    pause_tracker_shape(f, &rows, &stats);
    return pause_round(pause_tracker_units_h(rows, stats) * g.tsc);
}

/* CAREER1: page pg's place in the slide on the picture being drawn, permille of its own way out (0 = in place, +-1000 = just gone to the right /
   left; 2000 = parked, a page the slide does not move). *vis = 0 when no pixel of it is on the picture. pg_a 0 is the tick before's place. A facts
   blob with no page state (the selftest, a warm-up solve) is the main page at rest. */
inline int pause_page_off(const PauseFacts *f, int pg, int *vis)
{
    if (f->pg_from == f->pg_to) { *vis = pg == f->pg_to; return *vis ? 0 : 2000; }
    const int a = f->pg_a >= 0 && f->pg_a <= 1000 ? f->pg_a : 1000;
    const int e = f->pg_e0 + (int)((long long)(f->pg_e - f->pg_e0) * a / 1000);
    if (pg == f->pg_from) { *vis = e < 1000; return f->pg_dir * e; }
    if (pg == f->pg_to) { *vis = e > 0; return -f->pg_dir * (1000 - e); }
    *vis = 0;
    return 2000;
}
inline int pause_page_visible(const PauseFacts *f, int pg) { int v; pause_page_off(f, pg, &v); return v; }
inline int pause_page_rest(const PauseFacts *f, int pg) { return f->pg_from == f->pg_to && f->pg_to == pg; }
/* the way out of the wide main page (the tracker and the planks move as one), in host pixels */
inline int pause_main_away(const PauseGeo &g) { return g.w + pause_round(g.w * 0.05); }

/* the slide's sideways offsets in host pixels (narrow; both 0 in the wide layout and at slide_t1000 == 0 for the planks) */
inline int pause_planks_dx(const PauseGeo &g, const PauseFacts *f)
{
    int vis;
    const int o = pause_page_off(f, PAUSE_PG_MAIN, &vis);     /* CAREER1: the planks are the main page (wide: with the tracker) */
    if (g.wide) return (int)((long long)o * pause_main_away(g) / 1000);
    return (int)((double)o / 1000.0 * (g.w / 2 + g.pw / 2 + g.w * 0.05));
}
inline int pause_tracker_dx(const PauseGeo &g, const PauseFacts *f)
{
    int vis;
    if (g.wide) {       /* CAREER1: the wide tracker is part of the main page */
        const int o = pause_page_off(f, PAUSE_PG_MAIN, &vis);
        return (int)((long long)o * pause_main_away(g) / 1000);
    }
    /* the tracker rests at tx; it is parked off the opposite edge when it is not on the picture */
    const double away = g.tw + g.tx + g.w * 0.05;       /* far enough that no pixel shows */
    const int o = pause_page_off(f, PAUSE_PG_TRACKER, &vis);
    return (int)((double)o / 1000.0 * away);
}

/* the ROM's own plate is 240 x 32 DS pixels (the touch boxes of PS_Update: 8, y, 240, 32); a rom_art page draws it at the mod plank's width */
inline int pause_rom_plate_h(int w) { return pause_round(w * 32.0 / 240.0); }

/* plank i of a stack of n: its rect with the slide applied */
inline PauseRect pause_plank_rect(const PauseGeo &g, const PauseFacts *f, int n, int i, int pitch_scale_pct = 100)
{
    const int pw = g.pw * pitch_scale_pct / 100, gap = g.pgap * pitch_scale_pct / 100;
    const int ph = (f && f->rom_art) ? pause_rom_plate_h(pw) : g.ph * pitch_scale_pct / 100;
    const int total = n * ph + (n - 1) * gap;
    PauseRect r;
    r.w = pw; r.h = ph;
    r.x = g.pcx - pw / 2 + pause_planks_dx(g, f);
    r.y = g.pcy - total / 2 + i * (ph + gap);
    return r;
}

/* EVENTS3: the event strip, a small card under the plank stack: box for n rows, centred on cx.
   (cx, top) is the card's top middle; the rows are 40 u tall. */
inline PauseRect pause_event_rect(const PauseGeo &g, int n, int cx, int top)
{
    PauseRect r;
    r.w = pause_round(540 * g.u);
    r.h = pause_round((n * 42 + 26) * g.u);
    r.x = cx - r.w / 2;
    r.y = top;
    return r;
}

/* the 4:3 edge tab (side 0 left, 1 right) */
inline PauseRect pause_tab_rect(const PauseGeo &g, int side)
{
    PauseRect r;
    r.w = g.tabw; r.h = g.tabh;
    r.x = side ? g.w - g.tabw : 0;
    r.y = g.tabcy - g.tabh / 2;
    return r;
}

/* the dialog sheet (save prompt, saving, quit) that stands where the planks stand: its box for `kind` (PAUSE_V_*), the slide applied */
inline PauseRect pause_dialog_rect(const PauseGeo &g, const PauseFacts *f, int kind)
{
    const int dwu = 600;
    int dhu = 560;
    if (kind == PAUSE_V_STATUS) dhu = 330;
    if (kind == PAUSE_V_NOSAVE) dhu = 430;
    PauseRect r;
    const double k = g.wide ? g.u : g.u * 1.0;
    r.w = pause_round(dwu * k); r.h = pause_round(dhu * k);
    r.x = g.pcx - r.w / 2 + pause_planks_dx(g, f);
    r.y = g.pcy - r.h / 2 + pause_round(26 * k);
    return r;
}

/* planks inside a dialog: slot i of n, narrower than the main stack (440 u wide), starting below the text block.
   PAUSEFULL1 rom_art: the ROM's own plates keep their shape (15:2); the quit-without-saving page's two small ROM plates (YES, NO: 80 x 32 DS pixels,
   5:2, side by side as the ROM lays them) stand left and right at the bottom of the sheet. */
inline PauseRect pause_dialog_plank(const PauseGeo &g, const PauseFacts *f, int kind, int n, int i)
{
    const PauseRect d = pause_dialog_rect(g, f, kind);
    const bool rom = f && f->rom_art;
    PauseRect r;
    const int bottom = d.y + d.h - pause_round(34 * g.u);
    if (rom && kind == PAUSE_V_NOSAVE) {
        r.w = pause_round(220 * g.u); r.h = pause_round(r.w * 32.0 / 80.0);
        const int gap = pause_round(30 * g.u);
        r.x = d.x + d.w / 2 + (i == 0 ? -gap / 2 - r.w : gap / 2);
        r.y = bottom - r.h;
        return r;
    }
    const int pw = pause_round(440 * g.u), ph = rom ? pause_rom_plate_h(pw) : pause_round(440 * g.u * 129.0 / 606.0), gap = pause_round(14 * g.u);
    r.w = pw; r.h = ph;
    r.x = d.x + (d.w - pw) / 2;
    r.y = bottom - (n - i) * ph - (n - 1 - i) * gap;
    return r;
}

/* CAREER1: the Career page's sheet: the tracker's size (its tallest page), centred, with the slide applied */
inline PauseRect pause_career_rect(const PauseGeo &g, const PauseFacts *f)
{
    PauseRect r;
    r.w = g.tw;
    r.h = pause_round(pause_tracker_units_h(8, 3) * g.tsc);
    const int x0 = (g.w - r.w) / 2;
    int vis;
    const int o = pause_page_off(f, PAUSE_PG_CAREER, &vis);
    const double away = r.w + x0 + g.w * 0.05;
    r.x = x0 + (int)((double)o / 1000.0 * away);
    r.y = (g.h - r.h) / 2 + pause_round(g.wide ? 10 * g.u : 14 * g.u);
    return r;
}

/* the course paging arrows of the tracker (side 0 left, 1 right): the box that is drawn and clicked */
inline PauseRect pause_arrow_rect(const PauseGeo &g, const PauseFacts *f, int side)
{
    const int dx = pause_tracker_dx(g, f);
    const double sc = g.tsc;
    PauseRect r;
    r.w = pause_round(64 * sc); r.h = pause_round(64 * sc);
    const int row_cy = g.ty + pause_round((110 + 20) * sc);
    r.x = side ? g.tx + dx + g.tw - pause_round(104 * sc) - r.w + pause_round(12 * sc) : g.tx + dx + pause_round(104 * sc) - pause_round(12 * sc);
    r.y = row_cy - r.h / 2;
    return r;
}

#endif
