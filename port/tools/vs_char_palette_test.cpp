/* vs_char_palette_test.cpp -- unit tests for hal/vs_char_palette_gen.h and the
 * generated hal/vs_char_groups.h. Standalone, like vs_palette_test.cpp:
 *
 *   cl /std:c++14 /EHsc /O2 /I..\hal /I..\..\build\... vs_char_palette_test.cpp
 * (the header include dirs are -I port/hal; vs_char_groups.h is in the same dir)
 *
 * These run the SHIPPED arithmetic: char_recolor_palette wraps the same
 * family_edit / apply_transform vs_palette_test.cpp already proves against the
 * Studio's Rust. So this file does NOT re-prove the maths; it proves the wrapper
 * (bounds, in-place, disjointness, identity) and that the baked group table is
 * well formed. */

#include "vs_char_palette_gen.h"
#include "vs_char_groups.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace vspal;

static int g_fail;

static void ok(const char *what, int cond)
{
    if (!cond) {
        printf("FAIL: %s\n", what);
        ++g_fail;
    } else {
        printf("ok  : %s\n", what);
    }
}

/* A synthetic palette with distinct, saturated words so a recolor visibly
   moves the targeted indices. 64 colours is plenty. */
static void make_palette(u16 *p, int n)
{
    for (int i = 0; i < n; ++i) {
        u16 r = (u16)((i * 3 + 1) & 0x1f);
        u16 g = (u16)((i * 5 + 2) & 0x1f);
        u16 b = (u16)((i * 7 + 4) & 0x1f);
        p[i] = (u16)((b << 10) | (g << 5) | r);
    }
}

int main(void)
{
    /* 1. bounds: an index past the end refuses and writes nothing. */
    {
        u16 p[64], save[64];
        make_palette(p, 64);
        memcpy(save, p, sizeof p);
        int idx[2] = { 3, 64 };            /* 64 is out of range for n=64 */
        u8 tgt[3] = { 0xff, 0x00, 0x00 };
        const char *why = char_recolor_palette(p, 64, idx, 2, tgt);
        ok("out-of-range index refuses", why != 0);
        ok("out-of-range writes nothing", memcmp(p, save, sizeof p) == 0);
    }

    /* 2. disjoint: recolouring one category leaves every other word untouched. */
    {
        u16 p[64], save[64];
        make_palette(p, 64);
        memcpy(save, p, sizeof p);
        int idx[4] = { 10, 11, 12, 13 };
        u8 tgt[3] = { 0x20, 0xd0, 0xff };  /* cyan */
        const char *why = char_recolor_palette(p, 64, idx, 4, tgt);
        ok("in-range recolor succeeds", why == 0);
        int off = 0, on = 0;
        for (int i = 0; i < 64; ++i) {
            int in_cat = (i >= 10 && i <= 13);
            if (p[i] != save[i]) { if (in_cat) ++on; else ++off; }
        }
        ok("no word outside the category moved", off == 0);
        ok("at least one category word moved", on > 0);
    }

    /* 3. identity: a category recoloured toward its OWN representative colour is
       byte-identical -- the Studio's "an identity edit is still the identity",
       here for an arbitrary index set. Find the representative, feed its RGB. */
    {
        u16 p[64], save[64];
        make_palette(p, 64);
        memcpy(save, p, sizeof p);
        int idx[3] = { 20, 21, 22 };
        int rep = family_rep(p, idx, 3);
        u8 c[3];
        word_rgb(p[rep], c);               /* the representative's own colour */
        const char *why = char_recolor_palette(p, 64, idx, 3, c);
        ok("identity recolor succeeds", why == 0);
        ok("identity recolor is byte-identical",
           memcmp(p, save, sizeof p) == 0);
    }

    /* 4. determinism: same input twice gives the same bytes. */
    {
        u16 a[64], b[64];
        make_palette(a, 64);
        make_palette(b, 64);
        int idx[5] = { 5, 15, 25, 35, 45 };
        u8 tgt[3] = { 0x8c, 0x46, 0xff };
        char_recolor_palette(a, 64, idx, 5, tgt);
        char_recolor_palette(b, 64, idx, 5, tgt);
        ok("deterministic", memcmp(a, b, sizeof a) == 0);
    }

    /* 5. the baked group table is well formed: every record has a positive
       count, non-negative indices, a known char and category, and the indices
       within one (char,file,pal) record are unique. Also assert the coverage
       facts the report states: buttons is empty for all; shoes only Wario. */
    {
        int bad = 0, buttons = 0, marioshoes = 0, luigishoes = 0, warioshoes = 0;
        for (int i = 0; i < vscg::kVscGroupCount; ++i) {
            const vscg::Group *g = &vscg::kVscGroups[i];
            if (g->chr < 0 || g->chr > 2) ++bad;
            if (g->cat < 0 || g->cat >= vscg::CAT_COUNT) ++bad;
            if (g->count <= 0 || !g->idx || !g->file || !g->pal) ++bad;
            for (int a = 0; a < g->count; ++a) {
                if (g->idx[a] < 0) ++bad;
                for (int b = a + 1; b < g->count; ++b)
                    if (g->idx[a] == g->idx[b]) ++bad;
            }
            if (g->cat == vscg::CAT_BUTTONS) ++buttons;
            if (g->cat == vscg::CAT_SHOES) {
                if (g->chr == 0) ++marioshoes;
                if (g->chr == 1) ++luigishoes;
                if (g->chr == 2) ++warioshoes;
            }
        }
        ok("group table records are well formed", bad == 0);
        ok("buttons has no records for any bro (flagged gap)", buttons == 0);
        ok("shoes has no records for Mario/Luigi (flagged gap)",
           marioshoes == 0 && luigishoes == 0);
        ok("shoes has a record for Wario", warioshoes > 0);
    }

    if (g_fail) {
        printf("\n%d FAILED\n", g_fail);
        return 1;
    }
    printf("\nall green\n");
    return 0;
}
