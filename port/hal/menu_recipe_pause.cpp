/* menu_recipe_pause.cpp -- the one-screen PAUSE menu recipe (run menu1, lanes PAUSE1, PAUSE2, PAUSE3).
 *
 * PAUSEFULL1: with no HD UI Textures mod the planks below are the ROM's own plates (see pause_art_present); everything else is the same.
 *
 * WHAT THE PLAYER GETS (PAUSE3, Tango's "mock up is fine", 10-11). One pause screen in the launcher's look: parchment and wooden planks, the
 * DS lettering font at window resolution, no DS art. WIDE (16:9 and up): the STAR TRACKER at the left (course name, the seven star names with
 * got / not got, the 100 coin star, stars, coins and the coin record, red coins, the course arrows < > in the castle) and the PLANKS at the
 * right: Resume, Settings, Exit Course (in a course only), Save & Quit (in the castle only: the ROM has no save inside a course). 4:3: the planks
 * alone; LB / RB / Q / E or the wooden tab at either screen edge slides the planks (or the open settings) out in the bumper's direction and the
 * tracker in from the other side. The Settings plank opens the settings panel INSIDE the pause (hal/settings_page.cpp). The save pages (Save & Quit,
 * saving, saved, quit without saving) are sheets in the same style.
 *
 * WHAT STAYS THE ROM'S (MENU2_RULES 4). The ROM's pause state machine runs unchanged: this recipe only decides where its pieces land on one picture.
 * The planks are hostimg pieces whose hit boxes are the ROM's own touch boxes (a click maps through to the ROM's touch), the pad drives the ROM's
 * own plate cursor (data_0209f2e0, Stage::UpdateMenuButtons) and A is the ROM's confirm. The tracker's numbers are read from the ROM's save data
 * and its message bank (the star names), once a tick, on the logic thread (pause_facts); the draw sees only the facts (hal/menu_pause_geo.h).
 *
 * THE ROM'S PLATES (data_0209f2e0 = the selected plate; PS_Update.cpp):
 *   castle  (state 0 init, 1)    Continue 0 (box y 0x20), Controller Modes 1 (0x48, no plank: Select belongs to the big map), Options 2 (0x70), Quit 3 (0x98)
 *   course  (2 init, 3)          Continue 0 (0x18), Controller Modes 1 (0x40), Options 2 (0x68), Exit Course 3 (0x90)
 *   save    (0xc init, 0xd)      Continue 0 (0x28), Save 1 (0x50), Don't Save 2 (0x78)       [shown: Save & Quit, Quit without saving, Resume]
 *   saving  (0xe, 0xf)           the status box, nothing to press (the ROM quits when its countdown ends)
 *   nosave  (0x10, 0x11)         YES (x 0x28, y 0x98) and NO (x 0x88): touch only; NO goes back to the game
 * Leaving states (6, 0x12, 0x13) keep the last variant up (hysteresis). A page the new look has no layout for (Controller Modes and Options, which the
 * planks never reach, and every language but English, whose strings the port's font cannot draw) keeps a plain layout of the ROM's own bottom screen.
 */
#include "menu_compose.h"
#include "menu_hostimg.h"
#include "ui_art.h"
#include "menu_pause_art_ids.h"
#include "menu_pause_geo.h"
#include "settings_page.h"
#include "settings_live.h"
#include "event_client.h"
#include "career.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
extern unsigned char data_0209f248;    /* the pause state PS_Update is in */
extern unsigned char data_0209f1ec;    /* the state it goes to next */
extern unsigned char data_0209f2c4;    /* 0 = playing, else the pause is up (Stage::Behavior :206-243) */
extern unsigned char data_0209f20c;    /* level-clear owns the game */
extern unsigned char data_0209f2d8;    /* game mode: 1 = VS */
extern unsigned char data_0209f2f0;    /* 1 = no stars yet: the ROM hides the course arrows */
extern unsigned char data_0209f218;    /* 1 = opened by Select: controller mode only, no Back */
extern unsigned char data_0209f2e0;    /* the selected plate */
extern unsigned char data_0209f2b4;    /* how many plates */
extern unsigned char data_0209f22c;    /* press hold ticks */
extern unsigned char data_0209f244;    /* press flash ticks */
extern unsigned char data_0209f300;    /* input lock after open */
extern unsigned char data_0209f2c8;    /* the course the card shows */
extern unsigned char data_0209f2b8;    /* the save status countdown */
extern signed char data_0209f2f8;      /* the level the pause is in (PS_Init: castle = 0x1d) */
extern unsigned char data_0209f250;    /* the local player's slot */
extern unsigned char data_020a0de8[];  /* TouchInfo[slot * 4]: +0 touched, +1 change edge, +2 x, +3 y (hal/sub_screen.cpp poll_touch writes it) */
extern unsigned char data_020a0e40;    /* the input slot the ROM reads */
extern unsigned short data_0209f358[]; /* coins per player */
extern signed char data_0209f30c[];    /* red coins per player */
extern unsigned char data_0209cad2[];  /* SaveData coin records, per course (SaveData::GetCoinRecord) */
extern short data_020756d0[];          /* the message id of each course's title (Stage::PS_Update case 0) */
extern int data_0209d6fc[];            /* the message bank (hal/message_boot.cpp): base, entries, info section */
extern int data_0209d708[];
extern int data_0209d70c[];
int GetOwnerLanguage(void);
int SublevelToLevel(int i);
int IsStarCollected(int courseID, int starID);
unsigned char CountStarsCollectedInLevelToDisplay(int courseID);
void _ZN10dScStage_c17UpdateMenuButtonsEb(int b);
void func_02012790(int id);
void pause_page_draw(const MenuLayout *lay, int piece, uint32_t *dst, int w, int h, int stride);   /* hal/settings_page.cpp (pause_page.inc) */
int pause_page_art(int w, int h, int idx, int *id, int *dw, int *dh);
int port_rom_frame(void);              /* hal/rom_frame.cpp */
}

namespace {

enum { V_CASTLE = PAUSE_V_CASTLE, V_COURSE = PAUSE_V_COURSE, V_CTL, V_CTL_SEL, V_OPT, V_SAVE = PAUSE_V_SAVE, V_STATUS = PAUSE_V_STATUS,
       V_NOSAVE = PAUSE_V_NOSAVE, V_VS, V_AIR = PAUSE_V_AIR };

/* ---- the variant, with hysteresis over the leaving states ---------------- */

int g_last_variant;
int pause_art_present();      /* below: 1 when an enabled mod carries the new pause's planks */

int pause_variant(void)
{
    if (!data_0209f2c4 || data_0209f20c) return 0;
    const unsigned st = data_0209f248;
    int v = 0;
    switch (st) {
    case 0: case 1: v = V_CASTLE; break;
    case 2: case 3: v = V_COURSE; break;
    case 4: case 5: v = 0; break;      /* VS pause: the ROM draws its own until a real lobby proof exists (PAUSEFIX1 F3) */
    case 7: case 8: v = data_0209f218 ? V_CTL_SEL : V_CTL; break;
    case 9: case 0xa: v = V_OPT; break;
    /* THE REDUCED PAUSE. PS_Update case 2 (src/game/stages/d_s_stage_methods.cpp, the course pause's init) goes to state 0xb instead of the
       plates when func_02029408 (Player::CanPause: airborne, taking damage, no control, or underwater outside the two swim states) says no, or
       the player's health word data_02092144[slot] is 0. State 0xb has no plates: the ROM shows the course card on the top screen and the PAUSE
       sprite on the bottom one, and resumes on Start / Select only (case 0xb, IsButtonInputValid's in-pause path). The new look gives it the
       Resume plank alone. The VS pause reaches 0xb too (case 4) and stays the ROM's own, as the rest of the VS pause does. */
    case 0xb: v = data_0209f2d8 == 1 ? V_OPT : V_AIR; break;
    case 0xc: case 0xd: v = V_SAVE; break;
    case 0xe: case 0xf: v = V_STATUS; break;
    case 0x10: case 0x11: v = V_NOSAVE; break;
    default: v = g_last_variant; break;       /* 6, 0x12, 0x13: leaving, ride through */
    }
    return v;
}

double wnow_ms(void)
{
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

/* ---- the warm-up ----------------------------------------------------------------------------------------------------------
   Decoding and scaling the pause's pictures on the first frame the pause is up cost ~95 ms in one frame (PAUSE2's perf trace). The recipe's up()
   runs every tick, so once the level has been running a moment and the pause is not up it solves each page once on a scratch layout and queues
   every picture to the hostimg worker at the size that page will draw at (menu_hostimg_prefetch), then the pictures the pause draws itself
   (pause_page_art). A slice is one solve or a few queue calls (well under a millisecond), only when the shared warm-up slot is free (menu_warm_claim:
   one slot per few ms for this and the settings page together, none while the game is hitching). The cache keeps the size; a window resize warms
   again. */
int g_idle_ticks, g_warm_v, g_warm_p, g_warm_a, g_warm_w, g_warm_h, g_warm_solved_v;
double g_last_claim = -1e9, g_last_call = -1e9;
int g_calm;
const int kWarmVars[] = { V_CASTLE, V_COURSE, V_SAVE, V_NOSAVE, V_AIR };
const int kNWarmVars = (int)(sizeof kWarmVars / sizeof kWarmVars[0]);

void warm_slice(int w, int h)
{
    static MenuLayout W;
    const MenuRecipe *R = 0;
    for (int i = 0; i < g_menu_nrecipes; ++i)
        if (!strcmp(g_menu_recipes[i].name, "pause")) R = &g_menu_recipes[i];
    static const bool trace = getenv("SM64DS_WARM_TRACE") != 0;
    const double t0 = wnow_ms();
    menu_hostimg_prefetch_poll();             /* installs what the worker finished */
    while (R && g_warm_v < kNWarmVars) {
        const int v = kWarmVars[g_warm_v];
        if (g_warm_solved_v != v) {
            menu_layout_solve(&W, R, v, w, h);
            g_warm_solved_v = v;
            if (trace) fprintf(stderr, "[warm] pause solve v%d: %.2f ms\n", v, wnow_ms() - t0);
        }
        int k = 0, found = -1;
        for (int i = 0; i < W.np; ++i) {
            if (W.p[i].src != SRC_HOSTIMG || !W.p[i].hostimg || W.p[i].nodraw || W.dest[i][2] <= 0 || W.dest[i][3] <= 0) continue;
            if (k++ == g_warm_p) { found = i; break; }
        }
        if (found < 0) { ++g_warm_v; g_warm_p = 0; continue; }     /* that page is done */
        if (menu_hostimg_prefetch(W.p[found].hostimg, W.dest[found][2], W.dest[found][3]) < 0) return;      /* the queue is full: next slice */
        ++g_warm_p;                                                  /* cached or queued: on to the next picture */
        if (wnow_ms() - t0 >= 1.0) return;
    }
    for (;;) {
        int id, dw, dh;
        if (!pause_page_art(w, h, g_warm_a, &id, &dw, &dh)) { g_warm_v = kNWarmVars + 1; return; }      /* all of it is queued */
        if (menu_hostimg_prefetch(id, dw, dh) < 0) return;
        ++g_warm_a;
        if (wnow_ms() - t0 >= 1.0) return;
    }
}

/* The shared warm-up slot, for the pause's pictures and the settings page's caches together. 1 = this call may do one slice. A slot is
   handed out at most every 4 ms (so two warmers never land on the same tick), and not at all until the callers have been seen running at
   an even pace for a few calls (a call more than 50 ms after the last one means the level or something else is hitching: wait). */
extern "C" int menu_warm_claim(void)
{
    static const bool off = getenv("SM64DS_NO_MENU_WARM") != 0;
    if (off) return 0;
    /* WARMFIX1 F1: the claim state below is plain statics, so the slot belongs to the first thread that asks (the logic thread); any other
       thread is simply never given one. */
    static std::atomic<size_t> owner(0);
    const size_t me = std::hash<std::thread::id>()(std::this_thread::get_id()) | 1;
    size_t none = 0;
    if (!owner.compare_exchange_strong(none, me) && none != me) return 0;
    const double t = wnow_ms();
    if (t - g_last_call > 50.0) g_calm = 0; else ++g_calm;
    g_last_call = t;
    if (g_calm < 3) return 0;
    if (t - g_last_claim < 4.0) return 0;
    g_last_claim = t;
    return 1;
}

void warm_tick(int scene_id)
{
    if (scene_id != 3 && scene_id != -1) { g_idle_ticks = 0; return; }
    int w, h;
    if (!hal_menu_last_size(&w, &h)) return;
    if (w != g_warm_w || h != g_warm_h) { g_warm_w = w; g_warm_h = h; g_warm_v = 0; g_warm_p = 0; g_warm_a = 0; g_warm_solved_v = 0; g_idle_ticks = 0; }
    if (++g_idle_ticks < 30 || g_warm_v > kNWarmVars) return;
    if (!menu_warm_claim()) return;
    warm_slice(w, h);
}

/* ---- the pages and the slide between them ------------------------------------------------------------------------------
   CAREER1: the pause has pages, switched with LB / RB / Q / E and the wooden edge tabs. 4:3: a ring of three, the planks (PAUSE_PG_MAIN), the
   star tracker and the Career page (RB goes to the next, LB to the one before; RB from the planks is the tracker, as before the Career page).
   16:9 and wider: two, the main page (the tracker and the planks together) and the Career page. A slide runs from one page to another in
   kSlideTicks ticks: the page in view leaves toward the press's side (RB, E and the right tab: to the right), the other comes in from the other
   side. It is one curve, eased out of its own start (slide_e), and every picture between ticks draws its own place on it (the facts carry this
   tick's place and the tick before's). */

struct Slide {
    int page;         /* the page at rest, or the one the slide is going to */
    int from;         /* the page the slide leaves (== page at rest) */
    int ticks;        /* 0..kSlideTicks */
    int dir;          /* +1: the leaving page goes to the right */
    int e0;           /* the eased place on the tick before */
} g_slide = { PAUSE_PG_MAIN, PAUSE_PG_MAIN, 0, 1, 0 };
const int kSlideTicks = 14;      /* about 0.23 s at the ROM's 60 ticks a second */

int slide_e(int ticks)
{
    const double p = (double)ticks / kSlideTicks, q = 1.0 - p;
    return (int)((1.0 - q * q * q) * 1000.0 + 0.5);
}

int slide_moving(void) { return g_slide.from != g_slide.page; }

/* start a slide to page `to`, the page in view leaving to side `dir` */
void slide_go(int to, int dir)
{
    if (to == g_slide.page && !slide_moving()) return;
    g_slide.from = g_slide.page;
    g_slide.page = to;
    g_slide.ticks = 0;
    g_slide.dir = dir;
    g_slide.e0 = 0;
}

/* back to the main page, the way the last slide came (the page in view leaves where the main page arrives from) */
void slide_home(void)
{
    if (g_slide.page == PAUSE_PG_MAIN && !slide_moving()) return;
    if (slide_moving()) {
        /* a slide in flight (a resize, a dialog) lands at once: the main page */
        g_slide.page = g_slide.from = PAUSE_PG_MAIN; g_slide.ticks = 0; g_slide.e0 = 0;
        return;
    }
    slide_go(PAUSE_PG_MAIN, -g_slide.dir);
}

void slide_reset(void) { g_slide.page = g_slide.from = PAUSE_PG_MAIN; g_slide.ticks = 0; g_slide.dir = 1; g_slide.e0 = 0; }

void slide_tick(int allowed, int wide)
{
    if (!allowed) { slide_reset(); return; }
    if (wide && (g_slide.page == PAUSE_PG_TRACKER || g_slide.from == PAUSE_PG_TRACKER)) slide_reset();   /* the wide layout has no tracker page */
    if (!slide_moving()) return;
    if (g_slide.ticks >= kSlideTicks) { g_slide.from = g_slide.page; g_slide.ticks = 0; g_slide.e0 = 0; return; }   /* landed on the tick before */
    g_slide.e0 = slide_e(g_slide.ticks);
    ++g_slide.ticks;
}

int g_wide_now;                         /* the picture of this tick is wide (set by pause_facts) */
int g_pic_w = 1024, g_pic_h = 768;      /* the picture of this tick */
int g_tab_hover = -1, g_tab_press = -1; /* the 4:3 edge tabs under / held by the pointer (set by the input hook) */

/* the course paging, as a synthesised touch on the ROM's own arrow boxes (PS_Update.cpp :322-401: x < 0x38 / x >= 0xc8, y < 0x20) */
void page_course(int dir)
{
    if (data_0209f248 != 1 || data_0209f2f0) return;
    hal_menu_touch_queue_press(dir < 0 ? 0x1c : 0xe4, 0x10);
}

/* NO STALE STYLUS UNDER A CONFIRM KEY. The ROM's plate chain (PS_Update.cpp case 1 / 3 / 5: "touched, or a valid button") tests the stylus x / y
   against every plate box BEFORE it looks at the chosen plate, and a valid button (Start, which the port's confirm keys become) with no touch
   goes through the same tests. On the cartridge an untouched panel reads x = y = 0xff (src/func_0203b9bc.c: the idle quad {0xff, 0xff, 0, 0}
   whenever no ring entry is touched), which falls outside every box, so the chosen plate wins. The port latches the LAST touched point on release
   (hal/sub_screen.cpp poll_touch), so any earlier click left x / y inside some plate box (a click that landed where the ROM's Controller Modes
   plate is, 0x40..0x5f in a course, is enough) and the next Enter / A / Space on "Exit Course" pressed that plate instead. While the pause is up
   and nothing is touching, park the point where the cartridge does. */
void park_idle_stylus(void)
{
    unsigned char *t = data_020a0de8 + (unsigned)data_020a0e40 * 4;
    if (t[0] == 0) { t[2] = 0xff; t[3] = 0xff; }
}

int is_main_page(int v) { return v == V_CASTLE || v == V_COURSE; }
/* A SLIDE IN FLIGHT EATS EVERY INPUT: from the press that starts it until it lands on its page, nothing is queued and nothing is delivered
   late (bumpers, Q / E, the edge tabs, the planks' A / Enter / Space / Up / Down / Start / B and clicks). Main pages only. */
int slide_busy(void)
{
    if (!is_main_page(g_last_variant)) return 0;
    return slide_moving() && g_slide.ticks < kSlideTicks;
}
/* the page in view (or going into view) is not the main one: the planks are away */
int off_main(void) { return is_main_page(g_last_variant) && g_slide.page != PAUSE_PG_MAIN; }
int tracker_view(void) { return !g_wide_now && off_main() && g_slide.page == PAUSE_PG_TRACKER; }

/* ---- the ROM's strings ------------------------------------------------------------------------------------------------ */

/* The message bank's text is a glyph-index stream (src/func_0201b7cc.c): 0..9 digits, 0xa..0x23 A..Z, 0x2d..0x46 a..z, a few punctuation codes, 0xff
   ends a line. Decoded to ASCII (checked against the bank: course 0 star 0 reads BIG BOB-OMB ON THE SUMMIT). */
int msg_text(int id, char *out, int cap)
{
    out[0] = 0;
    const char *info = (const char *)(size_t)(unsigned)data_0209d70c[0];
    const char *base = (const char *)(size_t)(unsigned)data_0209d6fc[0];
    const char *entries = (const char *)(size_t)(unsigned)data_0209d708[0];
    if (!info || !base || !entries) return 0;
    const unsigned count = *(const unsigned short *)(info + 8);
    if (id < 0 || (unsigned)id >= count) return 0;
    const int secsz = *(const int *)(info + 4);
    const int off = *(const int *)(entries + id * 8);
    const unsigned char *p = (const unsigned char *)(base + 0x28 + secsz + off);
    int n = 0;
    for (int guard = 0; guard < 120 && *p != 0xff && n < cap - 1; ++guard, ++p) {
        const unsigned c = *p;
        char o;
        if (c <= 9) o = (char)('0' + c);
        else if (c <= 0x23) o = (char)('A' + c - 10);
        else if (c >= 0x2d && c <= 0x46) o = (char)('a' + c - 0x2d);
        else switch (c) {
            case 0x26: o = '?'; break;
            case 0x27: o = '!'; break;
            case 0x29: o = ','; break;
            case 0x2b: o = '"'; break;
            case 0x47: o = '-'; break;
            case 0x48: o = '.'; break;
            case 0x49: o = '\''; break;
            case 0x4a: o = '!'; break;
            case 0x4c: o = '&'; break;
            case 0x4d: o = ' '; break;
            default: o = '?'; break;
        }
        out[n++] = o;
    }
    out[n] = 0;
    return n;
}

/* STAR NAMES IN TITLE CASE: each word capitalised, the small words and the letters after a hyphen or an apostrophe not */
void title_case(char *s)
{
    static const char *const small[] = { "a", "an", "the", "of", "on", "in", "to", "for", "with", "at", "from", "and", "or", "over", "by" };
    char *w = s;
    int first = 1;
    while (*w) {
        while (*w == ' ') ++w;
        char *e = w;
        while (*e && *e != ' ') ++e;
        const char save = *e;
        *e = 0;
        char low[40];
        snprintf(low, sizeof low, "%s", w);
        for (char *q = low; *q; ++q) if (*q >= 'A' && *q <= 'Z') *q = (char)(*q - 'A' + 'a');
        char core[40];
        snprintf(core, sizeof core, "%s", low);
        for (int k = (int)strlen(core) - 1; k >= 0 && (core[k] == ',' || core[k] == '!' || core[k] == '.' || core[k] == '?'); --k) core[k] = 0;
        int is_small = 0;
        for (unsigned k = 0; k < sizeof small / sizeof small[0]; ++k) if (!strcmp(core, small[k])) is_small = 1;
        for (char *q = w; *q; ++q) *q = low[q - w];
        if ((!is_small || first) && *w >= 'a' && *w <= 'z') *w = (char)(*w - 'a' + 'A');
        if (!strncmp(low, "chain-chomp", 11)) { *w = 'C'; w[6] = 'C'; }           /* the ROM spells Chain Chomp with a hyphen */
        *e = save;
        w = e;
        first = 0;
    }
}

struct Text {
    int lv, bank;
    char title[40], sub[20];
    char names[7][38];
    char secret[15][28];      /* EXTRAS093: the secret places' names, title case */
} g_text = { -1, 0, "", "", {}, {} };

void text_for(int lv)
{
    const int bank = data_0209d6fc[0];
    if (g_text.lv == lv && g_text.bank == bank) return;
    g_text.lv = lv; g_text.bank = bank;
    memset(g_text.title, 0, sizeof g_text.title); memset(g_text.sub, 0, sizeof g_text.sub); memset(g_text.names, 0, sizeof g_text.names); memset(g_text.secret, 0, sizeof g_text.secret);
    if (!bank) return;
    char raw[80];
    const int t = lv < 15 ? lv : 15;
    msg_text(data_020756d0[t], raw, sizeof raw);
    const char *p = raw;
    while (*p == ' ') ++p;
    int num = 0;
    if (lv < 15) while (*p >= '0' && *p <= '9') { num = num * 10 + (*p - '0'); ++p; }
    while (*p == ' ') ++p;
    snprintf(g_text.title, sizeof g_text.title, "%s", p);
    if (lv < 15) snprintf(g_text.sub, sizeof g_text.sub, "Course %d", num ? num : lv + 1);
    else snprintf(g_text.sub, sizeof g_text.sub, "Peach's Castle");
    if (lv >= 15)
        for (int i = 0; i < 15; ++i) {
            /* the ROM's own course titles for the secret courses: message 0x1a5 + i is course 15 + i (the castle itself, the last, is the
               ROM's "Castle Secret Stars", which would repeat the page title: it is listed as In the Castle) */
            char raw[40];
            msg_text(0x1a5 + i, raw, sizeof raw);
            const char *p = raw;
            while (*p == ' ') ++p;
            snprintf(g_text.secret[i], sizeof g_text.secret[i], "%s", i == 14 && *p ? "In the Castle" : p);
            if (i != 14) title_case(g_text.secret[i]);
        }
    if (lv < 15)
        for (int a = 0; a < 7; ++a) {
            msg_text(0x1b4 + lv * 7 + a, g_text.names[a], sizeof g_text.names[a]);
            title_case(g_text.names[a]);
        }
}

/* ---- the plank slots of a page ---------------------------------------------------------------------------------------- */

const int kRomMain[3] = { 0, 2, 3 };         /* Resume, Settings, Save & Quit (castle) / Exit Course (course) */
const int kRomSave[3] = { 1, 2, 0 };         /* Save & Quit, Quit without saving, Resume */

const int *slot_rom_table(int variant) { return variant == V_SAVE ? kRomSave : kRomMain; }
int nslots_of(int variant)
{
    return variant == V_CASTLE || variant == V_COURSE || variant == V_SAVE ? 3 : (variant == V_NOSAVE ? 2 : (variant == V_AIR ? 1 : 0));
}
int slot_of_rom(int variant, int rom)
{
    const int n = nslots_of(variant);
    const int *t = slot_rom_table(variant);
    for (int i = 0; i < n; ++i) if (t[i] == rom) return i;
    return -1;
}

/* ---- the facts of this tick ---------------------------------------------------------------------------------------------- */

unsigned g_hov_seen;
int g_frame;
int g_save_init;        /* the save page's cursor was put on Save & Quit once for this visit */
int g_embed_set;
int g_nosave_focus;     /* the quit-without-saving page's cursor was put on NO once for this visit */
int g_air_hold;         /* V_AIR: the pointer went down on the Resume plank and is still held there */
int g_air_resume;       /* V_AIR: a click on the Resume plank, delivered to the ROM as its Start on the next pad frame */
int g_press_latch;      /* GLITCHFIX1: a plank press the ROM took, held until its page is gone (press_latched) */
int g_press_latch_v;    /* the page it was taken on */
int g_press_latch_f = -100;     /* the ROM frame of the last call: a pause opened again starts with no press */

/* GLITCHFIX1: THE PRESSED PLANK STAYS DOWN UNTIL ITS PAGE IS GONE. A press sets the ROM's confirm wait (data_0209f22c, 8 ticks: Stage::PS_Update
   sel1_0 and its siblings) or, on the air page, the plate's flash (data_0209f244, 4 ticks, case 0xb), and with it the page's way out: the close
   (data_0209f2c4 = 2, done by dScStage_c::Behavior's PS_Cleanup on the first tick the wait reads 0) or the next state (data_0209f1ec, taken by
   PS_Update on the tick after the wait ran out). The wait reaches 0 one tick before either happens, so a plank drawn pressed only while it runs
   popped back up for that last tick. The press is held from the wait's start for as long as that way out is still pending on the same page. */
int press_latched(int v, int rom_press)
{
    const int now = port_rom_frame();
    if (v != g_press_latch_v || now - g_press_latch_f > 2 || now < g_press_latch_f) { g_press_latch = 0; g_press_latch_v = v; }
    g_press_latch_f = now;
    const bool pending = data_0209f2c4 == 2 || data_0209f1ec != data_0209f248;
    if (rom_press) g_press_latch = 1;
    else if (!pending) g_press_latch = 0;
    return g_press_latch;
}

int pause_cursor_live(void)
{
    const unsigned st = data_0209f248;
    return (st == 1 || st == 3 || st == 5 || st == 0xd) && data_0209f300 == 0 && data_0209f22c == 0 && data_0209f244 == 0;
}

void set_chosen(int sel)
{
    const int last = data_0209f2b4 ? data_0209f2b4 - 1 : 0;
    if (sel < 0 || sel > last || sel == (int)data_0209f2e0) return;
    data_0209f2e0 = (unsigned char)sel;
    _ZN10dScStage_c17UpdateMenuButtonsEb(0);
}

void publish_embed(const PauseFacts *f, int wide)
{
    SpEmbed e;
    const int on = is_main_page(f->variant);
    e.on = on;
    e.wide = wide;
    e.dx = (!wide && on) ? pause_planks_dx(pause_geo(g_pic_w, g_pic_h, f, 3), f) : 0;
    e.input = !slide_busy() && g_slide.page == PAUSE_PG_MAIN;      /* CAREER1: the panel takes input only on the main page at rest */
    settings_page_set_embed(&e);
    g_embed_set = on;
}

void pause_facts(MenuFacts *mf)
{
    mf->language = (int16_t)GetOwnerLanguage();
    mf->pause_f2f0 = data_0209f2f0;
    int w = 1024, h = 768;
    hal_menu_last_size(&w, &h);
    g_pic_w = w; g_pic_h = h;
    const int wide = (long long)w * 192 / h >= 300;
    g_wide_now = wide;
    const int v = g_last_variant;
    /* MOUSE HOVER: a pointer that moved onto a plank makes it the ROM's chosen plate (silent), so the lit plank, the pointer and a
       later Enter all name the same plate */
    int hx, hy;
    unsigned st;
    if (hal_menu_hover_box(&hx, &hy, &st) && st != g_hov_seen) {
        g_hov_seen = st;
        if (pause_cursor_live() && (v == V_CASTLE || v == V_COURSE || v == V_SAVE) && !settings_page_open()) {
            const int y0 = data_0209f248 == 1 ? 0x20 : (data_0209f248 == 3 ? 0x18 : 0x28);
            if (hy >= y0 && (hy - y0) % 0x28 == 0 && slot_of_rom(v, (hy - y0) / 0x28) >= 0) set_chosen((hy - y0) / 0x28);
        }
    }
    ++g_frame;
    if (v != V_SAVE) g_save_init = 0;
    else if (!g_save_init && data_0209f248 == 0xd && data_0209f300 == 0) { g_save_init = 1; set_chosen(1); }   /* the cursor starts on Save & Quit */
    slide_tick(is_main_page(v), wide);       /* the pages are the main pages' only; a dialog forces the main page */
    PauseFacts *f = (PauseFacts *)mf->blob;
    memset(f, 0, sizeof *f);
    f->magic = PAUSE_FACTS_MAGIC;
    f->variant = v;
    /* CAREER1: the page state (menu_pause_geo.h pause_page_off reads it); slide_t1000 / dir keep their old meaning for the planks at 4:3 */
    f->pg_page = (int16_t)g_slide.page;
    f->pg_from = (int16_t)g_slide.from;
    f->pg_to = (int16_t)g_slide.page;
    f->pg_dir = (int16_t)g_slide.dir;
    f->pg_e = (int16_t)(slide_moving() ? slide_e(g_slide.ticks) : 0);
    f->pg_e0 = (int16_t)(slide_moving() ? g_slide.e0 : 0);
    f->pg_a = 1000;
    {
        int vis;
        const int o = pause_page_off(f, PAUSE_PG_MAIN, &vis);
        f->slide_t1000 = wide ? 0 : (vis ? (o < 0 ? -o : o) : 1000);
        f->dir = o < 0 ? -1 : (o > 0 ? 1 : g_slide.dir);
    }
    {   /* the Career page's numbers, per file */
        f->career_cur = (int8_t)port_career_slot_now();
        int has = 0;
        for (int i = 0; i < 3; ++i) {
            unsigned long long play = 0, clear = 0;
            int hc = 0;
            if (port_career_get(i, &play, &hc, &clear)) {
                has |= 1 << i;
                if (hc) has |= 1 << (3 + i);
            }
            f->career_play[i] = (uint32_t)(play / 60 > 0xffffffffull ? 0xffffffffull : play / 60);
            f->career_clear[i] = (uint32_t)(clear / 60 > 0xffffffffull ? 0xffffffffull : clear / 60);
        }
        f->career_has = (int8_t)has;
    }
    f->settings_open = settings_page_open();
    const int in_course = SublevelToLevel(data_0209f2f8) != 0x1d;
    f->in_course = in_course;
    f->paging = !in_course && !data_0209f2f0 && v != 0;
    f->frame = g_frame;
    f->hover_tab = g_tab_hover;
    f->press_tab = g_tab_press;
    int lv = data_0209f2c8;
    if (lv > 15) lv = 15;
    f->course = lv;
    text_for(lv);
    memcpy(f->title, g_text.title, sizeof f->title);
    memcpy(f->sub, g_text.sub, sizeof f->sub);
    memcpy(f->names, g_text.names, sizeof f->names);
    int mask = 0;
    if (lv < 15) for (int k = 0; k < 8; ++k) if (IsStarCollected(lv, k)) mask |= 1 << k;
    f->got_mask = mask;
    f->stars_got = CountStarsCollectedInLevelToDisplay(lv);
    if (lv >= 15)
        for (int i = 0; i < 15; ++i) {
            int n = 0;
            for (int k = 0; k < 8; ++k) if (IsStarCollected(15 + i, k)) ++n;
            f->secret_got[i] = (uint8_t)n;
            memcpy(f->secret_name[i], g_text.secret[i], sizeof f->secret_name[i]);
        }
    f->secret_n = CountStarsCollectedInLevelToDisplay(15);
    {
        int total = f->secret_n;
        for (int c = 0; c < 15; ++c) total += CountStarsCollectedInLevelToDisplay(c);
        f->total_n = total;
    }
    const int me = data_0209f250 < 16 ? data_0209f250 : 0;
    f->coin_rec = lv < 15 ? data_0209cad2[lv] : 0;
    f->coins = in_course ? (int)data_0209f358[me] : 0;
    f->red = in_course ? (int)(unsigned char)data_0209f30c[me] : 0;
    f->show_red = f->red > 0;
    f->status = data_0209f2b8 != 0 && data_0209f2b8 <= 0x3c;
    f->nslots = nslots_of(v);
    const int latched = press_latched(v, v == V_AIR ? data_0209f244 != 0 : data_0209f22c != 0);     /* GLITCHFIX1 */
    f->pressed = latched || (settings_page_press_look() == 1 && data_0209f2e0 == 2);   /* HUDMENU2 (H4): a pad confirm on Options shows the plate's press too */
    int hs = -1;
    if (v == V_NOSAVE) {
        /* touch only: the lit plank is the pad / hover cursor (hotspot 0 = NO, slot 1; hotspot 1 = YES, slot 0) */
        f->sel_slot = hal_menu_focus_shown(&hs) ? (hs == 0 ? 1 : 0) : -1;
    } else if (v == V_CASTLE || v == V_COURSE || v == V_SAVE) {
        f->sel_slot = slot_of_rom(v, data_0209f2e0);
    } else if (v == V_AIR) {
        f->sel_slot = 0;                                   /* one plank: always the lit one */
        f->pressed = latched || g_air_hold;     /* the ROM's resume flash (case 0xb) until the page closes, or the pointer held on the plank */
    } else f->sel_slot = -1;
    f->event_n = port::event_pause_lines(f->event, 2);     /* EVENTS3 */
    f->rom_art = !pause_art_present();     /* PAUSEFULL1: no mod planks, the ROM's own plates */
    f->arrows_live = f->paging && pause_page_rest(f, wide ? PAUSE_PG_MAIN : PAUSE_PG_TRACKER) && !f->settings_open && is_main_page(v);
    publish_embed(f, wide);
    mf->spare[0] = data_0209f2e0;
    mf->spare[1] = g_frame;
}

/* ---- the builders ---------------------------------------------------------------------------------------------------- */

const PauseFacts *facts_of(const MenuLayout *L, PauseFacts *fallback, int variant)
{
    const PauseFacts *f = (const PauseFacts *)L->facts.blob;
    if (f->magic == PAUSE_FACTS_MAGIC) return f;
    memset(fallback, 0, sizeof *fallback);      /* no facts (the layout selftest, a warm-up solve): the planks page at rest */
    fallback->magic = PAUSE_FACTS_MAGIC;
    fallback->variant = variant;
    fallback->dir = 1;
    fallback->nslots = nslots_of(variant);
    fallback->sel_slot = 0;
    fallback->in_course = variant == V_COURSE;
    fallback->paging = variant == V_CASTLE;
    return fallback;
}

/* the whole-picture piece the pause draws its tracker, dialogs, tabs and pointer with (pause_page_draw) */
int add_custom(MenuLayout *L)
{
    MenuPiece p = menu_piece_make(SRC_HOSTIMG, 0, 0, 1, 1, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.bg = 1;
    p.fixdest = 1; p.fx = 0; p.fy = 0; p.fw = (short)L->w; p.fh = (short)L->h;
    p.custom = pause_page_draw;
    return menu_add_piece(L, p);
}

/* a plank: the ROM's own touch box (bx, by, bw, bh) is what a click inside the host rect maps to */
int add_plank(MenuLayout *L, int img, const PauseRect &r, int bx, int by, int bw, int bh, int rom, int lit, int pressed)
{
    MenuPiece p = menu_piece_make(SRC_HOSTIMG, bx, by, bw, bh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.hostimg = (uint8_t)img;
    p.fixdest = 1; p.fx = (short)r.x; p.fy = (short)r.y; p.fw = (short)r.w; p.fh = (short)r.h;
    p.touch = 1;
    p.hl = (uint8_t)((lit ? 1 : 0) | (lit && pressed ? 2 : 0));
    const int ip = menu_add_piece(L, p);
    if (ip < 0) return -1;
    return menu_add_hotspot(L, ip, bx, by, bw, bh, rom);       /* the hotspot index */
}

/* PAUSEFULL1: the ROM's own plate as one element: the engine-B rows of its box (the plate is BG1 tiles, Stage::UpdateMenuButtons recolours
   them with the palette bank of the chosen plate, so the lit look is the ROM's own), keyed on the backdrop, stretched over the slot rect. The
   click maps back into the ROM's own box. */
int add_rom_piece(MenuLayout *L, const PauseRect &r, int bx, int by, int bw, int bh)
{
    MenuPiece p = menu_piece_make(SRC_SUB, bx, by, bw, bh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.layers = MENU_L_BG0 | MENU_L_BG1 | MENU_L_OBJ;
    p.key_backdrop = 1;
    p.touch = 1;
    p.fixdest = 1; p.fx = (short)r.x; p.fy = (short)r.y; p.fw = (short)r.w; p.fh = (short)r.h;
    return menu_add_piece(L, p);
}

int add_rom_plate(MenuLayout *L, const PauseRect &r, int bx, int by, int bw, int bh, int rom)
{
    const int ip = add_rom_piece(L, r, bx, by, bw, bh);
    if (ip < 0) return -1;
    return menu_add_hotspot(L, ip, bx, by, bw, bh, rom);
}

/* a touch-only piece (draws nothing) over a host rect: a click maps into the ROM box (bx, by, bw, bh) */
int add_touch_zone(MenuLayout *L, const PauseRect &r, int bx, int by, int bw, int bh)
{
    MenuPiece p = menu_piece_make(SRC_HOSTIMG, bx, by, bw, bh, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
    p.nodraw = 1; p.touch = 1;
    p.fixdest = 1; p.fx = (short)r.x; p.fy = (short)r.y; p.fw = (short)r.w; p.fh = (short)r.h;
    const int ip = menu_add_piece(L, p);
    if (ip < 0) return -1;
    return menu_add_hotspot(L, ip, bx, by, bw, bh, -1);
}

void finish_new(MenuLayout *L)
{
    L->lift_owners = MENU_L_BG3 | MENU_L_OBJ;     /* the ROM's own top text and sprites are lifted out of the picture and drawn nowhere */
    L->hide_owners = MENU_L_BG1 | MENU_L_BG2;     /* the card (BG2) and the "TOUCH TO SELECT" band (BG1) are drawn nowhere */
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
    L->no_focus_paint = 1;                        /* the lit plank and the pointer are the selection */
}

void build_new(MenuLayout *L, int variant)
{
    PauseFacts fb;
    const PauseFacts *f = facts_of(L, &fb, variant);
    const PauseGeo g = pause_geo(L->w, L->h, f, f->nslots);
    add_custom(L);
    L->back = -1;
    const int sel = f->sel_slot, prs = f->pressed;
    if (variant == V_CASTLE || variant == V_COURSE) {
        static const int castle_box_y[3] = { 0x20, 0x70, 0x98 }, course_box_y[3] = { 0x18, 0x68, 0x90 };
        const int *box_y = variant == V_COURSE ? course_box_y : castle_box_y;
        const int img[3] = { MENU_IMG_P_PK_RESUME, MENU_IMG_P_PK_SETTINGS, variant == V_COURSE ? MENU_IMG_P_PK_EXIT : MENU_IMG_P_PK_SAVEQUIT };
        if (f->arrows_live)
            for (int side = 0; side < 2; ++side)      /* the ROM's course arrows (PS_Update.cpp :322-401), clickable where the tracker draws them */
                add_touch_zone(L, pause_arrow_rect(g, f, side), side ? 0xc8 : 0, 0, 0x38, 0x20);
        const int rest = pause_page_rest(f, PAUSE_PG_MAIN);
        if (!f->settings_open && pause_page_visible(f, PAUSE_PG_MAIN)) {
            /* CAREER1: a picture between ticks draws each plank at its own place of the slide (MenuPiece.slide_dx0: the tick before's x
               minus this tick's) */
            PauseFacts f0 = *f;
            f0.pg_a = 0;
            const int slide0 = pause_planks_dx(g, &f0) - pause_planks_dx(g, f);
            for (int i = 0; i < 3; ++i) {
                const PauseRect r = pause_plank_rect(g, f, 3, i);
                const int np0 = L->np;
                const int h = f->rom_art ? add_rom_plate(L, r, 8, box_y[i], 240, 32, kRomMain[i])
                                         : add_plank(L, img[i], r, 8, box_y[i], 240, 32, kRomMain[i], i == sel, prs);
                if (L->np > np0) L->p[L->np - 1].slide_dx0 = slide0;
                if (i == 0 && rest && h >= 0) L->back = (int8_t)h;
            }
        }
    } else if (variant == V_AIR) {
        /* the Resume plank alone, where the course page's stack is centred. No hotspot: the ROM's state 0xb takes no touch (case 0xb reads
           only IsButtonInputValid), so the click is the recipe's own (menu_pause_input) and reaches the ROM as Start (menu_pause_hook_pad). */
        const PauseRect r = pause_plank_rect(g, f, 1, 0);
        if (!f->rom_art) {          /* no mod planks: the page draws a plain one (pause_page.inc pp_air_plank), the ROM has no plate up here */
            MenuPiece p = menu_piece_make(SRC_HOSTIMG, 8, 0x18, 240, 32, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
            p.hostimg = MENU_IMG_P_PK_RESUME;
            p.fixdest = 1; p.fx = (short)r.x; p.fy = (short)r.y; p.fw = (short)r.w; p.fh = (short)r.h;
            p.hl = (uint8_t)(1 | (prs ? 2 : 0));
            menu_add_piece(L, p);
        }
    } else if (variant == V_SAVE) {
        static const int box_y[3] = { 0x50, 0x78, 0x28 };            /* the ROM plates 1, 2, 0 */
        const int img[3] = { MENU_IMG_P_PK_SAVEQUIT, MENU_IMG_P_PK_QUITNOSAVE, MENU_IMG_P_PK_RESUME };
        for (int i = 0; i < 3; ++i) {
            const PauseRect r = pause_dialog_plank(g, f, V_SAVE, 3, i);
            const int h = f->rom_art ? add_rom_plate(L, r, 8, box_y[i], 240, 32, kRomSave[i])
                                     : add_plank(L, img[i], r, 8, box_y[i], 240, 32, kRomSave[i], i == sel, prs);
            if (i == 2 && h >= 0) L->back = (int8_t)h;
        }
    } else if (variant == V_NOSAVE) {
        /* touch only (PS_Update.cpp :1370-1451): NO first (hotspot 0, the default focus and the back key), then YES; the stack reads YES over NO */
        const PauseRect ry = pause_dialog_plank(g, f, V_NOSAVE, 2, 0), rn = pause_dialog_plank(g, f, V_NOSAVE, 2, 1);
        if (f->rom_art) {
            /* the ROM's own YES and NO plates (80 x 32 at 0x28 / 0x88, y 0x98), moved onto the sheet; the lit one is the pad / hover cursor */
            const int in = add_rom_piece(L, rn, 0x88, 0x98, 0x50, 0x20), iy = add_rom_piece(L, ry, 0x28, 0x98, 0x50, 0x20);
            const int hn = menu_add_hotspot(L, in, 0x88, 0x98, 0x50, 0x20, -1);
            menu_add_hotspot(L, iy, 0x28, 0x98, 0x50, 0x20, -1);
            L->back = (int8_t)hn;
            finish_new(L);
            return;
        }
        MenuPiece py = menu_piece_make(SRC_HOSTIMG, 0x28, 0x98, 0x50, 0x20, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        py.hostimg = MENU_IMG_P_PK_YESQUIT; py.fixdest = 1; py.fx = (short)ry.x; py.fy = (short)ry.y; py.fw = (short)ry.w; py.fh = (short)ry.h; py.touch = 1;
        py.hl = (uint8_t)(sel == 0 ? (1 | (prs ? 2 : 0)) : 0);
        MenuPiece pn = menu_piece_make(SRC_HOSTIMG, 0x88, 0x98, 0x50, 0x20, MENU_AX_L, MENU_AY_T, 0, 0, 1, 1);
        pn.hostimg = MENU_IMG_P_PK_NOKEEP; pn.fixdest = 1; pn.fx = (short)rn.x; pn.fy = (short)rn.y; pn.fw = (short)rn.w; pn.fh = (short)rn.h; pn.touch = 1;
        pn.hl = (uint8_t)(sel == 1 ? (1 | (prs ? 2 : 0)) : 0);
        const int iy = menu_add_piece(L, py), in = menu_add_piece(L, pn);
        const int hn = menu_add_hotspot(L, in, 0x88, 0x98, 0x50, 0x20, -1);
        menu_add_hotspot(L, iy, 0x28, 0x98, 0x50, 0x20, -1);
        L->back = (int8_t)hn;
    }
    finish_new(L);
}

/* ---- the plain layout for what the new look has no page for ------------------------------------------------------------ */

/* The ROM's own bottom screen (plates, labels) as one touch piece at the right of the picture, the top picture as the ROM draws it: used for every
   language but English (the port's font has no accents) and for the Controller Modes / Options pages (reached only by Select, which the big map
   took, and by the plates the new look does not have). Nothing is lifted. */
struct Hs { short x, y, w, h; int8_t rom; };

MenuPiece sub_plain(int sx, int sy, int sw, int sh, int dy, int touch)
{
    MenuPiece p = menu_piece_make(SRC_SUB, sx, sy, sw, sh, MENU_AX_R, MENU_AY_M, -8, dy, 2, 3);
    p.layers = MENU_L_BG0 | MENU_L_BG1 | MENU_L_OBJ;
    p.key_backdrop = 1;
    p.touch = (uint8_t)touch;
    return p;
}

void ok_box(int controller, int lang, short *x, short *w)
{
    if (controller) {
        if (lang == 5) { *x = 0x44; *w = 0x64; }
        else if (lang == 3) { *x = 0x3c; *w = 0x74; }
        else { *x = 0x4c; *w = 0x54; }
    } else {
        if (lang == 5) { *x = 0x40; *w = 0x70; }
        else if (lang == 3) { *x = 0x48; *w = 0x60; }
        else { *x = 0x50; *w = 0x50; }
    }
}

void build_plain(MenuLayout *L, int variant)
{
    const int lang = L->facts.language;
    switch (variant) {
    case V_CASTLE: case V_COURSE: case V_SAVE: {
        const int y0 = variant == V_CASTLE ? 0x20 : (variant == V_COURSE ? 0x18 : 0x28);
        const int n = variant == V_SAVE ? 3 : 4;
        const int ps = menu_add_piece(L, sub_plain(8, y0 - 8, 240, 40 * n, 0, 1));
        for (int i = 0; i < n; ++i) menu_add_hotspot(L, ps, 8, y0 + 0x28 * i, 240, 32, i);
        L->back = 0;
        break;
    }
    case V_CTL: case V_CTL_SEL: {
        const int ps = menu_add_piece(L, sub_plain(4, 28, 248, 160, 0, 1));
        short ox, ow;
        ok_box(1, lang, &ox, &ow);
        for (int i = 0; i < 3; ++i) menu_add_hotspot(L, ps, 8, 32 + 40 * i, 240, 32, -1);
        int back = menu_add_hotspot(L, ps, ox, 0x98, ow, 0x20, -1);
        if (variant == V_CTL) back = menu_add_hotspot(L, ps, 0xd8, 0x98, 0x20, 0x20, -1);
        L->back = (int8_t)back;
        break;
    }
    case V_OPT: {
        const int ps = menu_add_piece(L, sub_plain(4, 28, 248, 160, 0, 1));
        const Hs hs[4] = { { 0x5c, 0x30, 0x10, 0x10, -1 }, { 0xec, 0x30, 0x10, 0x10, -1 }, { 0x70, 0x58, 0x38, 0x20, -1 }, { 0xb0, 0x58, 0x38, 0x20, -1 } };
        for (int i = 0; i < 4; ++i) menu_add_hotspot(L, ps, hs[i].x, hs[i].y, hs[i].w, hs[i].h, hs[i].rom);
        short ox, ow;
        ok_box(0, lang, &ox, &ow);
        menu_add_hotspot(L, ps, ox, 0x98, ow, 0x20, -1);
        L->back = (int8_t)menu_add_hotspot(L, ps, 0xd8, 0x98, 0x20, 0x20, -1);
        break;
    }
    case V_STATUS: {
        menu_add_piece(L, sub_plain(56, 40, 144, 80, 0, 0));
        L->back = -1;
        break;
    }
    case V_NOSAVE: {
        const int ps = menu_add_piece(L, sub_plain(24, 64, 208, 128, 0, 1));
        menu_add_hotspot(L, ps, 0x28, 0x98, 0x50, 0x20, -1);                     /* YES */
        L->back = (int8_t)menu_add_hotspot(L, ps, 0x88, 0x98, 0x50, 0x20, -1);    /* NO */
        break;
    }
    default: break;
    }
    L->lift_owners = 0;
    L->hide_owners = 0;
    L->backdrop = MENU_BACKDROP_TOP_AS_IS;
    L->stage = MENU_STAGE_POST_FADE;
}

void pause_build(int variant, MenuLayout *L)
{
    const int english = L->facts.language <= 1;
    if (english && (variant == V_CASTLE || variant == V_COURSE || variant == V_SAVE || variant == V_STATUS || variant == V_NOSAVE || variant == V_AIR)) build_new(L, variant);
    else if (variant >= V_CASTLE && variant <= V_NOSAVE) build_plain(L, variant);
}

/* ---- the up() test and the ROM-cursor driver --------------------------------------------------------------------------- */

/* run hunt8, lanes UIMOD1 and PAUSEFULL1: the new pause is drawn from the planks of the "HD UI Textures" mod (hd_ui/menu/pause3/pk_*.png). When no
   enabled mod carries all seven, the same one-screen pause stands (same layout slots, same input) with the ROM's own plank plates: each plate is
   its own element, the engine-B rows of its touch box (the plates are BG1 tiles and Stage::UpdateMenuButtons gives the chosen one its lit palette),
   moved onto the slot at the mod plank's width (add_rom_plate). The tracker and the dialogs keep the plain code-drawn skin. Asked once per mod
   list: hal/mods.cpp's relatch at the title asks again (port_menu_pause_hd_ui_reset, SEAMART1). */
static int g_pause_art = -1;
int pause_art_present()
{
    int &v = g_pause_art;
    if (v < 0) {
        static const char *const kPk[] = { "pk_resume", "pk_settings", "pk_exit", "pk_savequit", "pk_quitnosave", "pk_yesquit", "pk_nokeep" };
        char rel[96], path[1400];
        v = 1;
        for (const char *n : kPk) {
            snprintf(rel, sizeof rel, "menu/pause3/%s.png", n);
            if (!ui_art_path(rel, path, sizeof path)) v = 0;
        }
        fprintf(stderr, "[menu] pause art: %s\n", v ? "HD UI Textures planks found, one-screen pause" : "no mod planks, one-screen pause with the ROM's own plates");
    }
    return v;
}

int pause_up(int scene_id)
{
    int v = 0;
    /* the Stage is scene 3; a level boot that never runs the scene thunk answers -1 (hal_menu_hook_h1), and
       the stacked layout is MENU_SCENE_GATED: no recipe */
    if (scene_id == 3 || scene_id == -1) v = pause_variant();
    if (v && GetOwnerLanguage() > 1) v = 0;    /* every language but English: the recipe draws nothing, the player sees the ROM's own pause as with OneScreenMenus off */
    if (v != g_last_variant) {
        /* settings must reach the file on every way out of the pause (REVSETLIVE1 G1: the queue waits half a second and has no flush on quit) */
        if (!v || v == V_STATUS || v == V_NOSAVE) sl_flush();
        slide_reset();
    }
    g_last_variant = v;
    if (!v) {
        if (g_embed_set) { settings_page_set_embed(0); g_embed_set = 0; }
        g_tab_hover = g_tab_press = -1;
        warm_tick(scene_id);
    }
    if (hal_menu_trace_on()) {      /* SM64DS_MENU_TRACE: every change of the ROM's pause words, for the proof rows */
        static int last[10] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
        const int now[10] = { v, data_0209f2c4, data_0209f248, data_0209f1ec, data_0209f2c8, data_0209f2e0, data_0209f218,
                              (int)g_slide.ticks, (int)g_slide.page, g_slide.dir };
        if (memcmp(now, last, sizeof now) != 0) {
            memcpy(last, now, sizeof last);
            fprintf(stderr, "[pause] variant=%d f2c4=%d f248=%d f1ec=%d course=%d sel=%d select=%d slide_ticks=%d page=%d dir=%d\n", now[0], now[1],
                    now[2], now[3], now[4], now[5], now[6], now[7], now[8], now[9]);
        }
    }
    return v;
}

/* dir = MENU_DIR_* or -1 (no direction this frame). *owns is set to 1 on the pages whose plates are the ROM's own cursor (A is then the ROM's
   confirm, the core rewrites it to Start) and 0 on the touch-only pages (A presses the focused hotspot). Returns 1 when it consumed the direction. */
int pause_cursor(int dir, int *owns)
{
    const unsigned st = data_0209f248;
    const int plates = st == 1 || st == 3 || st == 5 || st == 0xd;
    if (owns) *owns = plates;
    if (!plates || dir < 0) return 0;
    if (data_0209f300 != 0 || data_0209f22c != 0 || data_0209f244 != 0) return 1;
    if (dir == MENU_DIR_LEFT || dir == MENU_DIR_RIGHT) {
        /* the course arrows: live in the castle's main page, and only where the tracker is on screen (wide, or slid in at 4:3) */
        if (st == 1 && !data_0209f2f0 && ((g_wide_now && !off_main()) || tracker_view()) && !settings_page_open()) page_course(dir == MENU_DIR_LEFT ? -1 : 1);
        return 1;
    }
    if (off_main()) return 1;         /* the planks are slid away (the tracker or the Career page): up and down do nothing */
    const int v = g_last_variant;
    const int slot = slot_of_rom(v, data_0209f2e0);
    int ns = slot;
    if (slot < 0) ns = 0;
    else if (dir == MENU_DIR_UP && slot > 0) --ns;
    else if (dir == MENU_DIR_DOWN && slot < nslots_of(v) - 1) ++ns;
    if (ns != slot || slot < 0) {
        set_chosen(slot_rom_table(v)[ns]);
        host_menu_cursor_sound();
    }
    return 1;
}

}  // namespace

int menu_pause_up(int scene_id) { return pause_up(scene_id); }
void menu_pause_facts(MenuFacts *f) { pause_facts(f); }
void menu_pause_build(int variant, MenuLayout *L) { pause_build(variant, L); }
int menu_pause_cursor(int dir, int *owns) { return pause_cursor(dir, owns); }
int menu_pause_pad_ok(void) { return data_0209f2c4 != 0 && data_0209f20c == 0; }

/* ---- the devices the recipe's own controls read (walk_window.cpp's menu_input, once a logic frame) ----------------------- */

/* keys: bit 0 LB, bit 1 RB, bit 2 Q, bit 3 E (held now). The mouse is the pointer over the picture in picture pixels. Returns nothing: the slide, the
   course paging and the edge tabs act here. 1 when this recipe is up (the caller then lets nothing else take these keys). */
extern "C" int menu_pause_input(unsigned keys, int mvalid, int mx, int my, int mdown)
{
    static unsigned prev_keys;
    static int prev_down;
    const unsigned edge = keys & ~prev_keys;
    prev_keys = keys;
    const int down_edge = mdown && !prev_down;
    const int up_edge = !mdown && prev_down;
    prev_down = mdown;
    const int v = g_last_variant;
    if (!v) { g_tab_hover = g_tab_press = -1; g_nosave_focus = 0; return 0; }
    if (v != V_AIR) g_air_hold = g_air_resume = 0;
    else {
        /* V_AIR: a click (down and up) on the Resume plank resumes; the pointer anywhere else does nothing */
        PauseFacts f0;
        memset(&f0, 0, sizeof f0);
        f0.variant = V_AIR; f0.dir = 1; f0.in_course = 1; f0.nslots = 1;
        f0.rom_art = !pause_art_present();
        const PauseGeo g = pause_geo(g_pic_w, g_pic_h, &f0, 1);
        const PauseRect r = pause_plank_rect(g, &f0, 1, 0);
        const int inside = mvalid && mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h;
        if (down_edge && inside) g_air_hold = 1;
        if (up_edge && g_air_hold && inside) g_air_resume = 1;
        if (!mdown || !inside) g_air_hold = 0;
        return 1;
    }
    if (v != V_NOSAVE) g_nosave_focus = 0;
    else if (!g_nosave_focus && hal_menu_focus_set(0)) g_nosave_focus = 1;     /* the confirm page is touch only: the cursor starts on "No, keep playing" (hotspot 0) */
    if (!is_main_page(v) || GetOwnerLanguage() > 1) return 1;
    const int open = settings_page_open();
    unsigned left = edge & (1u | 4u), right = edge & (2u | 8u);       /* LB / Q, RB / E */
    if (open) { left &= 4u; right &= 8u; }                             /* the settings page's own tabs take LB / RB */
    if (open && g_wide_now) left = right = 0;                          /* the wide settings column does not slide */
    int act = 0;
    if (left || right) { act = right ? 1 : -1; }
    /* the edge tabs: shown on the main pages (both layouts since CAREER1), hidden while the settings panel is up and in view */
    const int tabs_up = !open || off_main();
    g_tab_hover = -1;
    if (tabs_up && mvalid) {
        PauseFacts f0;
        memset(&f0, 0, sizeof f0);
        f0.dir = g_slide.dir;
        const PauseGeo g = pause_geo(g_pic_w, g_pic_h, &f0, 3);
        for (int side = 0; side < 2; ++side) {
            const PauseRect r = pause_tab_rect(g, side);
            const int inside = mx >= r.x && mx < r.x + r.w && my >= r.y && my < r.y + r.h;
            if (inside) g_tab_hover = side;
            if (inside && down_edge) g_tab_press = side;
            if (up_edge && g_tab_press == side && inside) act = side ? 1 : -1;
        }
    }
    if (!mdown || !mvalid) g_tab_press = -1;
    /* A CAROUSEL: each bumper, Q / E and edge tab always slides its own way (RB, E and the right tab: the page in view leaves to the right and
       the next page comes in from the left; LB, Q and the left tab the mirror, to the page before). 4:3 rings three pages (the planks, the
       tracker, the Career page: RB from the planks is the tracker, as it was), the wide layout two (the main page and the Career page). A press
       while a slide is still running is dropped (slide_busy), not remembered. */
    if (act && slide_busy()) act = 0;
    if (act) {
        static const int ring[3] = { PAUSE_PG_MAIN, PAUSE_PG_TRACKER, PAUSE_PG_CAREER };
        static const int ring_w[2] = { PAUSE_PG_MAIN, PAUSE_PG_CAREER };
        const int n = g_wide_now ? 2 : 3;
        const int *rg = g_wide_now ? ring_w : ring;
        int at = 0;
        for (int i = 0; i < n; ++i) if (rg[i] == g_slide.page) at = i;
        slide_go(rg[(at + (act > 0 ? 1 : n - 1)) % n], act);
    }
    return 1;
}

/* the pad seam, before hal_menu_pad: while the tracker page is showing at 4:3 the planks are away, so A, Start and up / down must not reach the ROM's
   plate cursor: A (or B) slides back to the planks instead. pressed is the DS key word (MENU_PAD_*). */
extern "C" void menu_pause_hook_pad(uint16_t *pressed, int back)
{
    if (g_last_variant) park_idle_stylus();
    if (g_last_variant == V_AIR) {
        /* the reduced pause: Start, pad A (the port's confirm), pad B / the attack key, the back key and a click on the plank are all the
           ROM's Start, the one key its state 0xb resumes on (Select, its other one, belongs to the big map); the directions have nothing to move */
        uint16_t k = *pressed;
        const int go = (k & (MENU_PAD_START | MENU_PAD_CONFIRM | 0x0001)) || back || g_air_resume;
        k = (uint16_t)(k & ~(unsigned)(MENU_PAD_CONFIRM | 0x0001 | MENU_PAD_UP | MENU_PAD_DOWN | MENU_PAD_LEFT | MENU_PAD_RIGHT));
        if (go) k = (uint16_t)(k | MENU_PAD_START);
        g_air_resume = 0;
        *pressed = k;
        return;
    }
    if (slide_busy()) { *pressed = 0; return; }       /* a slide in flight eats the pad */
    if (!off_main() || settings_page_open()) return;
    const uint16_t confirm = (uint16_t)(MENU_PAD_CONFIRM | MENU_PAD_START | 0x0001);
    if (*pressed & MENU_PAD_START) {       /* Start resumes at once (the ROM's Continue plate), the tracker / Career page is left behind */
        slide_reset();
        set_chosen(0);
        *pressed = (uint16_t)(*pressed & ~(MENU_PAD_CONFIRM | 0x0001 | MENU_PAD_UP | MENU_PAD_DOWN));
        return;
    }
    if ((*pressed & confirm) || back) slide_home();      /* A / B: back to the main page the way it left */
    *pressed = (uint16_t)(*pressed & ~(confirm | MENU_PAD_UP | MENU_PAD_DOWN));
}

/* OneScreenMenus OFF: no recipe is up, the ROM's own two-screen pause is on screen. The cartridge's pause has no pad cursor (PS_Update case 3: only a
   touch inside a plate picks it; the pad's Start / Select presses whatever plate data_0209f2e0 already names, IsButtonInputValid's special path
   src/IsButtonInputValid.c :39), so with the keyboard or a pad no key could ever reach Exit Course and A / Space did nothing at all. The same two things the
   recipe's driver gives the new pause: Up / Down move the ROM's plate cursor (the ROM redraws it through UpdateMenuButtons, as a touch does), and
   the port's confirm keys (pad A, Space, the DS A key) become the Start the ROM confirms with. pressed is the DS key word (MENU_PAD_*); the caller writes it back. */
extern "C" void menu_pause_rom_pad(uint16_t *pressed)
{
    if (data_0209f248 != 5 && menu_pause_pad_ok()) park_idle_stylus();      /* (the VS pause stays the ROM's own) */
    if (!pause_cursor_live() || !menu_pause_pad_ok()) return;
    uint16_t k = *pressed;
    const unsigned st = data_0209f248;
    if (st == 5) return;            /* the VS pause stays the ROM's own */
    int dir = -1;
    if (k & MENU_PAD_UP) dir = MENU_DIR_UP;
    else if (k & MENU_PAD_DOWN) dir = MENU_DIR_DOWN;
    if (dir >= 0) {
        const int last = data_0209f2b4 ? (int)data_0209f2b4 - 1 : 0;
        const int cur = (int)data_0209f2e0;
        const int ns = dir == MENU_DIR_UP ? (cur > 0 ? cur - 1 : cur) : (cur < last ? cur + 1 : cur);
        if (ns != cur) {
            set_chosen(ns);
            host_menu_cursor_sound();
        }
    }
    if (k & 0x0001) {                 /* PADMENUS1: pad B / the attack key is Back: the Continue plate (the first one) is pressed, whatever the cursor was on */
        set_chosen(0);
        k = (uint16_t)((k & ~(unsigned)(MENU_PAD_CONFIRM | 0x0001)) | MENU_PAD_START);
    } else if (k & MENU_PAD_CONFIRM) {
        k = (uint16_t)((k & ~(unsigned)MENU_PAD_CONFIRM) | MENU_PAD_START);
    }
    k = (uint16_t)(k & ~(unsigned)(MENU_PAD_UP | MENU_PAD_DOWN));
    *pressed = k;
}

/* UIMOD1: 1 while the ROM's own pause is up because no mod carries the new pause's planks: the caller then gives it the same pad / key driver as OneScreenMenus off */
extern "C" int menu_pause_rom_fallback(void) { return 0; }      /* PAUSEFULL1: with no mod the recipe is up too (the ROM's own plates), so its own driver runs */

extern "C" int menu_pause_active(void) { return g_last_variant != 0; }
/* 1 while a 4:3 pause slide is in flight: the host stylus / click and the Backspace back key are eaten by their seam (hal/sub_screen.cpp) */
extern "C" int menu_pause_slide_busy(void) { return slide_busy(); }

/* run after093, lane SEAMART1 (one-process plan M3c): hal/mods.cpp's relatch at the title changed which mods ship hd_ui/, so the planks-or-plates
   answer above is asked again on the next pause (no pause is open at the title). Returns 1 when the old answer was the mod's planks. */
extern "C" int port_menu_pause_hd_ui_reset(void)
{
    const int was = g_pause_art;
    g_pause_art = -1;
    return was == 1;
}
