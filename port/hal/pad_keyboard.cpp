/* hal/pad_keyboard.cpp: the letter grid plank, logic and layout. See pad_keyboard.h. */
#include "pad_keyboard.h"
#include <stdio.h>
#include <string.h>

namespace {

/* the text grid: four rows of ten characters (CAPS gives the second table: capitals, and the symbols Shift gives on a keyboard's digit row), then a
   row of five wide keys */
const char kTextLo[4][11] = { "1234567890", "abcdefghij", "klmnopqrst", "uvwxyz.,'?" };
const char kTextHi[4][11] = { "!@#$%^&*()", "ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ_:-/" };
enum { T_COLS = 10, T_CHARS = 40, T_CAPS = 40, T_SPACE = 41, T_DEL = 42, T_BACK = 43, T_OK = 44, T_COUNT = 45 };
/* the room code grid: the lobby server's code alphabet (port/tools/lobby/app/server.py: no I, L, O, 0 or 1) in rows of eight, DEL in the last cell,
   then BACK and JOIN */
const char kCode[4][9] = { "ABCDEFGH", "JKMNPQRS", "TUVWXYZ2", "3456789" };
enum { C_COLS = 8, C_DEL = 31, C_BACK = 32, C_OK = 33, C_COUNT = 34 };

/* the plank in DS units: a text line over five rows of keys */
const float kPad = 6.f, kTextH = 16.f, kGap = 4.f, kRowP = 16.f, kKeyH = 14.f;
const int kRows = 5;
int panel_h(void) { return (int)(kPad + kTextH + kGap + kRows * kRowP - (kRowP - kKeyH) + kPad + 0.5f); }

struct Grid {
    int open, owner, kind, ok, max_len;
    int key, want_col, caps;
    int held[4];          /* frames each direction has been held */
} G;

int cols_of(int kind) { return kind == PKB_CODE ? C_COLS : T_COLS; }

/* key i's row, first column and span in columns */
int key_cell(int kind, int i, int *row, int *col, int *span)
{
    const int n = cols_of(kind);
    if (kind == PKB_CODE) {
        if (i < 0 || i >= C_COUNT) return 0;
        if (i < 32) { *row = i / n; *col = i % n; *span = 1; return 1; }
        *row = 4; *col = i == C_BACK ? 0 : 4; *span = 4; return 1;
    }
    if (i < 0 || i >= T_COUNT) return 0;
    if (i < T_CHARS) { *row = i / n; *col = i % n; *span = 1; return 1; }
    *row = 4; *col = (i - T_CAPS) * 2; *span = 2; return 1;
}

int key_at(int kind, int row, int col)
{
    const int cnt = pkb_key_count(kind);
    for (int i = 0; i < cnt; ++i) {
        int r, c, s;
        if (key_cell(kind, i, &r, &c, &s) && r == row && col >= c && col < c + s) return i;
    }
    return -1;
}

char key_char(int kind, int i, int caps)
{
    if (kind == PKB_CODE) return i < C_DEL ? kCode[i / C_COLS][i % C_COLS] : 0;
    if (i < T_CHARS) return (caps ? kTextHi : kTextLo)[i / T_COLS][i % T_COLS];
    if (i == T_SPACE) return ' ';
    return 0;
}

enum Act { A_NONE, A_CHAR, A_CAPS, A_DEL, A_BACK, A_OK };
int key_act(int kind, int i)
{
    if (kind == PKB_CODE) return i < C_DEL ? A_CHAR : i == C_DEL ? A_DEL : i == C_BACK ? A_BACK : A_OK;
    if (i < T_CHARS || i == T_SPACE) return A_CHAR;
    return i == T_CAPS ? A_CAPS : i == T_DEL ? A_DEL : i == T_BACK ? A_BACK : A_OK;
}
int ok_key(int kind) { return kind == PKB_CODE ? C_OK : T_OK; }

void step(int d)
{
    int r, c, s;
    if (!key_cell(G.kind, G.key, &r, &c, &s)) return;
    const int n = cols_of(G.kind);
    if (d == 2 || d == 3) {             /* left / right: the key before / after in the row, round the row's end */
        const int col = d == 2 ? (c - 1 + n) % n : (c + s) % n;
        const int k = key_at(G.kind, r, col);
        if (k >= 0) { G.key = k; int r2, c2, s2; key_cell(G.kind, k, &r2, &c2, &s2); G.want_col = c2; }
    } else {                            /* up / down: the next row round the grid, the key under the column the highlight came from */
        const int nr = d == 0 ? (r - 1 + kRows) % kRows : (r + 1) % kRows;
        const int k = key_at(G.kind, nr, G.want_col);
        if (k >= 0) G.key = k;
    }
}

int act(int i, int len, char *ch)
{
    switch (key_act(G.kind, i)) {
    case A_CHAR:
        if (len >= G.max_len) return PKB_NONE;
        *ch = key_char(G.kind, i, G.caps);
        if (len + 1 >= G.max_len) G.key = ok_key(G.kind);      /* full: the highlight goes to the submit key, as the DS name entry moves to OK */
        return PKB_TYPE;
    case A_CAPS: G.caps = !G.caps; fprintf(stderr, "[padkb] caps %s\n", G.caps ? "on" : "off"); return PKB_NONE;
    case A_DEL: return len ? PKB_DELETE : PKB_NONE;
    case A_BACK: return PKB_CLOSE;
    case A_OK: return PKB_SUBMIT;
    }
    return PKB_NONE;
}

}  // namespace

int pkb_open(int owner, int kind, int ok, int max_len)
{
    memset(&G, 0, sizeof G);
    G.open = 1;
    G.owner = owner;
    G.kind = kind == PKB_CODE ? PKB_CODE : PKB_TEXT;
    G.ok = ok;
    G.max_len = max_len > 0 ? max_len : 1;
    G.key = 0;
    G.want_col = 0;
    for (int d = 0; d < 4; ++d) G.held[d] = -1;      /* a direction already held when the grid opened is not a step until it is let go */
    fprintf(stderr, "[padkb] open: %s grid, %d letters at most\n", G.kind == PKB_CODE ? "room code" : "text", G.max_len);
    return 1;
}

void pkb_close(void)
{
    if (G.open) fprintf(stderr, "[padkb] closed\n");
    G.open = 0;
    G.owner = 0;
}

int pkb_is_open(void) { return G.open; }
int pkb_owner(void) { return G.open ? G.owner : 0; }
int pkb_kind(void) { return G.kind; }
int pkb_key(void) { return G.key; }
int pkb_caps(void) { return G.caps; }

void pkb_view(PkbView *v, const char *text, int caret)
{
    v->kind = G.kind; v->ok = G.ok; v->key = G.key; v->caps = G.caps;
    v->text = text; v->caret = caret; v->max_len = G.max_len;
}

int pkb_pad(const PkbPad *in, int len, char *ch)
{
    if (!G.open || !in) return PKB_NONE;
    const int old = G.key;
    for (int d = 0; d < 4; ++d) {
        if (!(in->dirs & (1u << d))) { G.held[d] = 0; continue; }
        if (G.held[d] < 0) continue;
        const int t = G.held[d]++;
        if (t == 0 || (t >= 20 && (t - 20) % 5 == 0)) step(d);
    }
    if (G.key != old) fprintf(stderr, "[padkb] key %d\n", G.key);
    if (in->start) { fprintf(stderr, "[padkb] START\n"); return PKB_SUBMIT; }
    if (in->b) return len ? PKB_DELETE : PKB_CLOSE;
    if (in->y && G.kind == PKB_TEXT) { G.caps = !G.caps; fprintf(stderr, "[padkb] caps %s (Y)\n", G.caps ? "on" : "off"); }
    if (in->a) return act(G.key, len, ch);
    return PKB_NONE;
}

static int hit(float x, float y)
{
    const int cnt = pkb_key_count(G.kind);
    for (int i = 0; i < cnt; ++i) {
        float kx, ky, kw, kh;
        if (pkb_key_box(G.kind, i, &kx, &ky, &kw, &kh) && x >= kx - 1 && x < kx + kw + 1 && y >= ky - 1 && y < ky + kh + 1) return i;
    }
    return -1;
}

int pkb_click(float x, float y, int len, char *ch)
{
    if (!G.open) return PKB_NONE;
    const int i = hit(x, y);
    if (i < 0) return PKB_NONE;
    G.key = i;
    int r, c, s;
    if (key_cell(G.kind, i, &r, &c, &s)) G.want_col = c;
    fprintf(stderr, "[padkb] click on key %d\n", i);
    return act(i, len, ch);
}

void pkb_hover(float x, float y)
{
    if (!G.open) return;
    const int i = hit(x, y);
    if (i >= 0 && i != G.key) {
        G.key = i;
        int r, c, s;
        if (key_cell(G.kind, i, &r, &c, &s)) G.want_col = c;
    }
}

int pkb_in_panel(float x, float y)
{
    int px, py, pw, ph;
    pkb_panel_box(G.kind, &px, &py, &pw, &ph);
    return x >= px && x < px + pw && y >= py && y < py + ph;
}

void pkb_panel_box(int kind, int *x, int *y, int *w, int *h)
{
    (void)kind;
    *w = PKB_PANEL_W;
    *h = panel_h();
    *x = PKB_PANEL_X;
    *y = PKB_PANEL_BOTTOM - *h;
}

int pkb_key_count(int kind) { return kind == PKB_CODE ? C_COUNT : T_COUNT; }

int pkb_key_box(int kind, int i, float *x, float *y, float *w, float *h)
{
    int r, c, s;
    if (!key_cell(kind, i, &r, &c, &s)) return 0;
    int px, py, pw, ph;
    pkb_panel_box(kind, &px, &py, &pw, &ph);
    const float inner = pw - 2 * kPad, pitch = inner / cols_of(kind), gap = 2.f;
    *x = px + kPad + c * pitch + gap * 0.5f;
    *w = s * pitch - gap;
    *y = py + kPad + kTextH + kGap + r * kRowP;
    *h = kKeyH;
    return 1;
}

const char *pkb_key_label(int kind, int ok, int i, int caps, char *buf, int cap)
{
    if (cap < 2) return "";
    const char *okw = ok == PKB_OK_JOIN ? "JOIN" : ok == PKB_OK_OK ? "OK" : "SEND";
    if (kind == PKB_CODE) {
        if (i == C_DEL) return "DEL";
        if (i == C_BACK) return "BACK";
        if (i == C_OK) return okw;
    } else {
        if (i == T_CAPS) return "CAPS";
        if (i == T_SPACE) return "SPACE";
        if (i == T_DEL) return "DEL";
        if (i == T_BACK) return "BACK";
        if (i == T_OK) return okw;
    }
    buf[0] = key_char(kind, i, caps);
    buf[1] = 0;
    return buf;
}

void pkb_text_box(int kind, float *x, float *y, float *w, float *h)
{
    int px, py, pw, ph;
    pkb_panel_box(kind, &px, &py, &pw, &ph);
    *x = px + kPad; *y = py + kPad; *w = pw - 2 * kPad; *h = kTextH;
}
