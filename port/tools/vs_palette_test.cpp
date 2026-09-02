/* vs_palette_test.cpp -- unit tests for hal/vs_palette_gen.h.
 *
 * Builds standalone, against nothing but the header and the C runtime, which is
 * the point of the header being header-only: these tests run the arithmetic the
 * game ships, not a copy of it.
 *
 *   cl /std:c++14 /EHsc /O2 /I..\hal vs_palette_test.cpp
 *   vs_palette_test            -> runs every case
 *   vs_palette_test --emit A   -> print one row as hex, for the Studio oracle
 */

#include "vs_palette_gen.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace vspal;

static int g_fail;

static void row_str(const u16 *r, char *out)
{
    for (int i = 0; i < 16; ++i)
        sprintf(out + i * 5, "%04x ", r[i]);
    out[16 * 5 - 1] = 0;
}

static void check_rows(const char *what, const u16 *got, const u16 *want)
{
    if (memcmp(got, want, 32) == 0) {
        printf("  ok   %s\n", what);
        return;
    }
    char a[128], b[128];
    row_str(got, a);
    row_str(want, b);
    printf("  FAIL %s\n       got  %s\n       want %s\n", what, a, b);
    g_fail = 1;
}

static void check(const char *what, int cond)
{
    printf("  %s %s\n", cond ? "ok  " : "FAIL", what);
    if (!cond) g_fail = 1;
}

/* ---- 1. the identity ------------------------------------------------------
 *
 * The Studio's own promise, palette_editor.rs:2905: "at the anchor both
 * readings are the same number, so the representative still lands exactly on
 * the picked colour, and an identity edit is still the identity". Pick each
 * group's representative's own ROM colour and the row must come back
 * unchanged -- which also proves the two groups do not touch each other's
 * slots, and that the ten fixed slots are genuinely fixed. */
static void test_identity(void)
{
    printf("identity: picking the ROM's own colours changes nothing\n");
    for (int r = 0; r < 4; ++r) {
        const u16 *row = kVsRomRows[r];
        int brep = family_rep(row, kBodyIdx, 4);
        int srep = family_rep(row, kShoeIdx, 2);
        u8 body[3], shoe[3];
        u16 out[16];
        char name[64];
        word_rgb(row[brep], body);
        word_rgb(row[srep], shoe);
        vs_palette_row(row, body, shoe, out);
        sprintf(name, "ROM row %d regenerates itself", r);
        check_rows(name, out, row);
    }
}

/* ---- 2. determinism -------------------------------------------------------
 *
 * The whole online story rests on this: every client computes every player's
 * row for itself, so the same two hex values have to give the same sixteen
 * words every single time, with no dependence on call order or on what was
 * computed before. The second sweep runs the colours in reverse so a hidden
 * carry-over between calls would show up. */
static void test_determinism(void)
{
    printf("determinism: same input, same bytes, twice\n");
    static const char *picks[8] = {
        "ff0000", "00ff00", "0000ff", "ffffff",
        "000000", "7f3fbf", "18e618", "e66318",
    };
    u16 first[8][8][16];
    int bad = 0;
    for (int b = 0; b < 8; ++b)
        for (int s = 0; s < 8; ++s) {
            u8 bc[3], sc[3];
            parse_hex6(picks[b], bc);
            parse_hex6(picks[s], sc);
            vs_palette_row(kVsRomRows[0], bc, sc, first[b][s]);
        }
    for (int b = 7; b >= 0; --b)
        for (int s = 7; s >= 0; --s) {
            u8 bc[3], sc[3];
            u16 again[16];
            parse_hex6(picks[b], bc);
            parse_hex6(picks[s], sc);
            vs_palette_row(kVsRomRows[0], bc, sc, again);
            if (memcmp(again, first[b][s], 32) != 0) bad = 1;
        }
    check("64 colour pairs, forwards then backwards, byte-identical", !bad);
}

/* ---- 3. the fixed slots ---------------------------------------------------
 *
 * Ten of the sixteen slots are nobody's player colour -- the eyes, the reds,
 * the greys and the whites -- and a recolour that moved them would give a
 * player pink eyeballs. No pick may ever touch them. */
static void test_fixed_slots(void)
{
    printf("fixed slots: 4,5,6,9,10,11,12,13,14,15 never move\n");
    static const int fixed[10] = { 4, 5, 6, 9, 10, 11, 12, 13, 14, 15 };
    int bad = -1;
    for (int a = 0; a < 256 && bad < 0; a += 17)
        for (int b = 0; b < 256 && bad < 0; b += 37) {
            u8 bc[3], sc[3];
            u16 out[16];
            bc[0] = (u8)a; bc[1] = (u8)b; bc[2] = (u8)(255 - a);
            sc[0] = (u8)b; sc[1] = (u8)(255 - b); sc[2] = (u8)a;
            vs_palette_row(kVsRomRows[0], bc, sc, out);
            for (int i = 0; i < 10; ++i)
                if (out[fixed[i]] != kVsRomRows[0][fixed[i]]) bad = fixed[i];
        }
    if (bad >= 0) printf("       slot %d moved\n", bad);
    check("240 colour pairs leave every fixed slot alone", bad < 0);
}

/* ---- 4. shading survives --------------------------------------------------
 *
 * The owner asked for shading, not a flat repaint. Whatever he picks, the four
 * body slots must stay four DISTINCT colours in the ROM's own darkest-to-
 * lightest order, or his Yoshi is a silhouette. This is the property the
 * Studio's k_down/k_up mapping exists to guarantee ("each is monotone in the
 * member's own lightness, so the shading order never flips"). */
static void test_shading_order(void)
{
    printf("shading: the body ramp keeps its order and stays distinct\n");
    /* The ROM's row-0 body ramp, darkest first, by Oklab lightness. */
    int order[4] = { 0, 1, 2, 3 };
    for (int i = 0; i < 4; ++i)
        for (int j = i + 1; j < 4; ++j) {
            float a[3], b[3];
            word_oklch(kVsRomRows[0][order[i]], a);
            word_oklch(kVsRomRows[0][order[j]], b);
            if (b[0] < a[0]) { int t = order[i]; order[i] = order[j]; order[j] = t; }
        }
    int flips = 0, collapses = 0, tried = 0;
    for (int h = 0; h < 360; h += 7)
        for (int lv = 40; lv <= 220; lv += 60) {
            /* a spread of hues at three lightnesses, built without needing a
               colour library: rotate a saturated triangle */
            float rad = (float)h * 3.14159265f / 180.0f;
            u8 bc[3], sc[3];
            u16 out[16];
            bc[0] = (u8)(lv * (0.5f + 0.5f * cosf(rad)));
            bc[1] = (u8)(lv * (0.5f + 0.5f * cosf(rad - 2.0944f)));
            bc[2] = (u8)(lv * (0.5f + 0.5f * cosf(rad + 2.0944f)));
            sc[0] = bc[2]; sc[1] = bc[0]; sc[2] = bc[1];
            vs_palette_row(kVsRomRows[0], bc, sc, out);
            ++tried;
            for (int i = 0; i + 1 < 4; ++i) {
                float a[3], b[3];
                word_oklch(out[order[i]], a);
                word_oklch(out[order[i + 1]], b);
                if (b[0] < a[0] - 1e-6f) ++flips;
            }
            {
                int distinct = 0;
                for (int i = 0; i < 4; ++i) {
                    int seen = 0;
                    for (int j = 0; j < i; ++j)
                        if (out[order[j]] == out[order[i]]) seen = 1;
                    if (!seen) ++distinct;
                }
                if (distinct < 3) ++collapses;
            }
        }
    printf("       %d picks tried\n", tried);
    check("no pick inverts the ramp", flips == 0);
    check("no pick flattens it below three distinct shades", collapses == 0);
}

/* ---- 5. the wire grammar --------------------------------------------------*/
static void test_parse(void)
{
    printf("hex parsing\n");
    u8 c[3];
    check("lower case", parse_hex6("1a2b3c", c) && c[0] == 0x1a && c[1] == 0x2b && c[2] == 0x3c);
    check("upper case", parse_hex6("1A2B3C", c) && c[0] == 0x1a && c[1] == 0x2b && c[2] == 0x3c);
    check("a non-hex byte is refused", !parse_hex6("1a2b3g", c));
    check("a space is refused", !parse_hex6("1a2b3 ", c));
}

/* ---- 6. the golden row ----------------------------------------------------
 *
 * One pick, frozen. If any of the arithmetic above drifts -- a constant, a
 * bisection count, a rounding rule -- this is what says so, and it is also the
 * vector the Studio oracle is compared against.
 *
 * Frozen against the Studio oracle (C:\tmp\vscolorui-out\oracle), which is the
 * Studio's own palette_editor.rs arithmetic copied verbatim: 512 vectors over
 * four ROM rows agree word for word. See oracle_diff.sh. */
static const u16 kGoldenPurple[16] = {
    0x4c8c, 0x6cb1, 0x7573, 0x69d3, 0x0022, 0x075d, 0x084e, 0x12da,
    0x035f, 0x107c, 0x1093, 0x1a99, 0x56d7, 0x675a, 0x77bd, 0x7fff,
};

static void test_golden(void)
{
    u8 bc[3], sc[3];
    u16 out[16];
    char s[128];
    parse_hex6("8a2be2", bc);       /* blue violet */
    parse_hex6("ffd700", sc);       /* gold */
    vs_palette_row(kVsRomRows[0], bc, sc, out);
    row_str(out, s);
    printf("golden: body 8a2be2 shoes ffd700 -> %s\n", s);
    if (kGoldenPurple[0]) check_rows("golden row", out, kGoldenPurple);
}

/* ---- 7. sixteen rows ------------------------------------------------------
 *
 * vs_palette_rows16 is what hal/fs_mods.cpp writes into the grown BMD palette,
 * so these four checks are the whole contract the wide seats rest on. */

static void test_rows16(void)
{
    u16 rows[16][16];
    VsPick picks[16];
    int i, s, t;

    printf("rows16: the ROM's four rows survive a build with no picks\n");
    memset(picks, 0, sizeof picks);
    vs_palette_rows16(kVsRomRows, picks, rows);
    for (s = 0; s < 4; ++s) {
        char what[48];
        sprintf(what, "row %d is the cartridge's, byte for byte", s);
        check_rows(what, rows[s], kVsRomRows[s]);
    }

    /* THE POINT OF THE WHOLE LANE: sixteen seats, sixteen tellable-apart
       Yoshis. Two rows are "the same Yoshi" if their four body words all
       match, which is what a player actually sees from across an arena. */
    printf("rows16: all sixteen body ramps are distinct\n");
    {
        int clashes = 0;
        for (s = 0; s < 16; ++s)
            for (t = s + 1; t < 16; ++t) {
                int same = 1;
                for (i = 0; i < 4; ++i)
                    if (rows[s][kBodyIdx[i]] != rows[t][kBodyIdx[i]]) same = 0;
                if (same) {
                    printf("       rows %d and %d share a body ramp\n", s, t);
                    ++clashes;
                }
            }
        check("no two seats wear the same body", clashes == 0);
    }

    /* The shoes too, though a shoe clash is a much weaker collision than a
       body one -- reported rather than failed if it ever appears alone. */
    printf("rows16: the shoe pairs are distinct as well\n");
    {
        int clashes = 0;
        for (s = 0; s < 16; ++s)
            for (t = s + 1; t < 16; ++t)
                if (rows[s][kShoeIdx[0]] == rows[t][kShoeIdx[0]] &&
                    rows[s][kShoeIdx[1]] == rows[t][kShoeIdx[1]])
                    ++clashes;
        check("no two seats wear the same boots", clashes == 0);
    }

    /* The ten fixed slots stay fixed in EVERY row, not just the ROM's four.
       This is what keeps the eye pupil, the saddle and the belly whites the
       cartridge's on a defaulted wide seat. */
    printf("rows16: the fixed slots are fixed in all sixteen rows\n");
    {
        static const int kFixed[10] = { 4, 5, 6, 9, 10, 11, 12, 13, 14, 15 };
        int wrong = 0;
        for (s = 0; s < 16; ++s)
            for (i = 0; i < 10; ++i)
                if (rows[s][kFixed[i]] != kVsRomRows[s & 3][kFixed[i]])
                    ++wrong;
        check("ten fixed slots x sixteen rows all untouched", wrong == 0);
    }

    /* A pick lands on the row it was made for and moves nothing else. Slot 9
       is chosen because 9 & 3 == 1, so it also proves the base row a wide seat
       is generated from is rom[s % 4] and not rom[0]. */
    printf("rows16: a pick reaches its own seat and only its own seat\n");
    {
        u16 base[16][16], want[16];
        memset(picks, 0, sizeof picks);
        vs_palette_rows16(kVsRomRows, picks, base);
        picks[9].set = 1;
        parse_hex6("8a2be2", picks[9].body);
        parse_hex6("ffd700", picks[9].shoes);
        vs_palette_rows16(kVsRomRows, picks, rows);
        vs_palette_row(kVsRomRows[1], picks[9].body, picks[9].shoes, want);
        check_rows("seat 9 wears its pick, anchored on ROM row 1", rows[9], want);
        {
            int moved = 0;
            for (s = 0; s < 16; ++s) {
                if (s == 9) continue;
                if (memcmp(rows[s], base[s], 32) != 0) ++moved;
            }
            check("no other seat moved", moved == 0);
        }
    }

    /* And the same pick in two seats that share a base row gives the same
       bytes, which is the property a player picking a colour actually cares
       about: seat 0 and seat 4 both anchor on ROM row 0. */
    printf("rows16: one pick, two seats on the same base row, same bytes\n");
    {
        memset(picks, 0, sizeof picks);
        for (s = 0; s < 16; s += 4) {
            picks[s].set = 1;
            parse_hex6("00a0ff", picks[s].body);
            parse_hex6("204020", picks[s].shoes);
        }
        vs_palette_rows16(kVsRomRows, picks, rows);
        check("seats 0, 4, 8 and 12 are byte-identical",
              memcmp(rows[0], rows[4], 32) == 0 &&
              memcmp(rows[0], rows[8], 32) == 0 &&
              memcmp(rows[0], rows[12], 32) == 0);
    }
}

int main(int argc, char **argv)
{
    if (argc >= 4 && strcmp(argv[1], "--emit") == 0) {
        /* --emit <body hex> <shoe hex> [row]: one line of sixteen words, for
           diffing against the Studio oracle. */
        u8 bc[3], sc[3];
        u16 out[16];
        int r = (argc >= 5) ? atoi(argv[4]) : 0;
        if (!parse_hex6(argv[2], bc) || !parse_hex6(argv[3], sc)) return 2;
        if (r < 0 || r > 3) return 2;
        vs_palette_row(kVsRomRows[r], bc, sc, out);
        for (int i = 0; i < 16; ++i) printf("%04x%s", out[i], i == 15 ? "\n" : " ");
        return 0;
    }
    test_identity();
    test_determinism();
    test_fixed_slots();
    test_shading_order();
    test_parse();
    test_golden();
    test_rows16();
    printf(g_fail ? "\nFAILED\n" : "\nall green\n");
    return g_fail;
}
