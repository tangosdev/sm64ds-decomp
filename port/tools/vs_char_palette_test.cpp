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
       facts the generator reports (BROCOLOR1, from the cartridge files):
       buttons and gloves are empty; every bro has hat, overalls and shoes;
       each record's main entry is one of its members. */
    {
        int bad = 0, buttons = 0, have[3][vscg::CAT_COUNT] = {};
        for (int i = 0; i < vscg::kVscGroupCount; ++i) {
            const vscg::Group *g = &vscg::kVscGroups[i];
            if (g->chr < 0 || g->chr > 2) ++bad;
            if (g->cat < 0 || g->cat >= vscg::CAT_COUNT) ++bad;
            if (g->count <= 0 || !g->idx || !g->file || !g->pal) ++bad;
            int main_in = 0;
            for (int a = 0; a < g->count; ++a) {
                if (g->idx[a] < 0) ++bad;
                if (g->idx[a] == g->main) main_in = 1;
                for (int b = a + 1; b < g->count; ++b)
                    if (g->idx[a] == g->idx[b]) ++bad;
            }
            if (!main_in) ++bad;
            if (g->cat == vscg::CAT_BUTTONS || g->cat == vscg::CAT_GLOVES) ++buttons;
            if (g->chr >= 0 && g->chr <= 2 && g->cat >= 0 && g->cat < vscg::CAT_COUNT) have[g->chr][g->cat] = 1;
        }
        ok("group table records are well formed", bad == 0);
        ok("gloves and buttons have no records", buttons == 0);
        int all = 1;
        for (int c = 0; c < 3; ++c)
            all &= have[c][vscg::CAT_HAT] & have[c][vscg::CAT_UNDERSHIRT] & have[c][vscg::CAT_OVERALLS] & have[c][vscg::CAT_SHOES];
        ok("every bro has hat, undershirt, overalls and shoes records", all == 1);
        /* BROGROUPS1: every record is one of Tango's Studio labels, on the row
           that label belongs to (HAT = Hat, UNDERSHIRT = Undershirt since
           PALSPLIT1, OVERALLS = Overalls, SHOES = Shoes); nothing else he
           labelled is recoloured. */
        int lab_bad = 0;
        for (int i = 0; i < vscg::kVscGroupCount; ++i) {
            const vscg::Group *g = &vscg::kVscGroups[i];
            const char *l = g->label ? g->label : "";
            const int want = !strcmp(l, "Hat") ? vscg::CAT_HAT
                           : !strcmp(l, "Undershirt") ? vscg::CAT_UNDERSHIRT
                           : !strcmp(l, "Overalls") ? vscg::CAT_OVERALLS
                           : !strcmp(l, "Shoes") ? vscg::CAT_SHOES : -1;
            if (want != g->cat) ++lab_bad;
        }
        ok("every record carries its row's Studio label", lab_bad == 0);
        /* BROSKIN1: an entry held back in bro_groups_keep.json (Wario's arm
           skin) is in no picker record of its palette. */
        int kept_in = 0;
        for (int k = 0; k < vscg::kVscKeepCartCount; ++k) {
            const vscg::Kept *e = &vscg::kVscKeepCart[k];
            for (int i = 0; i < vscg::kVscGroupCount; ++i) {
                const vscg::Group *g = &vscg::kVscGroups[i];
                if (g->chr != e->chr || strcmp(g->file, e->file) || strcmp(g->pal, e->pal)) continue;
                for (int a = 0; a < g->count; ++a)
                    if (g->idx[a] == e->index) ++kept_in;
            }
        }
        ok("held-back entries are in no picker record", kept_in == 0);
        /* BROSKIN3: Wario's bare-arm skin now has its own cartridge label
           (N/A) in the Studio, so the keep list is empty; the skin must still
           be in no picker record. */
        static const int kSkin[] = { 0, 16, 21, 58, 59, 84, 174, 176 };
        int skin_in = 0;
        for (int i = 0; i < vscg::kVscGroupCount; ++i) {
            const vscg::Group *g = &vscg::kVscGroups[i];
            if (g->chr != 2 || strcmp(g->pal, "wario_body_pl")) continue;
            for (int a = 0; a < g->count; ++a)
                for (int s = 0; s < (int)(sizeof kSkin / sizeof kSkin[0]); ++s)
                    if (g->idx[a] == kSkin[s]) ++skin_in;
        }
        ok("Wario's arm skin is in no picker record", skin_in == 0);
    }

    /* 6. the drawn shade (YOSHICOLOR1): with a main index in the group the pick
       lands on THAT word exactly; a main index outside the group falls back to
       family_rep, which is the old behaviour. */
    {
        u16 p[64], q[64];
        make_palette(p, 64);
        make_palette(q, 64);
        int idx[4] = { 30, 31, 32, 33 };
        u8 tgt[3] = { 0xf7, 0xc8, 0x5b };
        char_recolor_palette(p, 64, idx, 4, tgt, 32);
        ok("the pick is the main word, to the DS's five bits",
           p[32] == rgb_word(tgt, q[32]));
        u16 r[64];
        make_palette(r, 64);
        char_recolor_palette(r, 64, idx, 4, tgt, 7);   /* 7 is not a member */
        int rep = family_rep(q, idx, 4);
        ok("a main index outside the group falls back to family_rep",
           r[rep] == rgb_word(tgt, q[rep]));
    }

    /* 7. bmd_drawn_member counts texels: a one-texture BMD (format 3, 8x8)
       whose texels read word 5 forty times, word 6 twenty times and word 7
       four times picks 5 out of {5,6,7}; an unknown palette name gives -1. */
    {
        static u8 d[0x200];
        memset(d, 0, sizeof d);
        struct { u32 o, v; } hdr[] = {
            { 0x14, 1 }, { 0x18, 0x40 },      /* one texture record at 0x40 */
            { 0x1c, 1 }, { 0x20, 0x60 },      /* one palette record at 0x60 */
            { 0x24, 1 }, { 0x28, 0x80 },      /* one material record at 0x80 */
        };
        for (unsigned i = 0; i < sizeof hdr / sizeof hdr[0]; ++i)
            memcpy(d + hdr[i].o, &hdr[i].v, 4);
        u32 tex[5] = { 0xf0, 0x100, 32, 0, (3u << 26) };   /* name, data, size, -, fmt 3 8x8 */
        u32 pal[4] = { 0xf8, 0x140, 32, 0 };
        u32 mat[3] = { 0xf0, 0, 0 };                       /* name, tex 0, pal 0 */
        memcpy(d + 0x40, tex, sizeof tex);
        memcpy(d + 0x60, pal, sizeof pal);
        memcpy(d + 0x80, mat, sizeof mat);
        memcpy(d + 0xf0, "t", 2);
        memcpy(d + 0xf8, "t_pl", 5);
        for (int t = 0; t < 64; ++t) {
            int v = t < 40 ? 5 : (t < 60 ? 6 : 7);
            d[0x100 + (t >> 1)] |= (u8)(v << ((t & 1) * 4));
        }
        int idx[3] = { 7, 6, 5 };
        ok("bmd_drawn_member finds the most-drawn word",
           bmd_drawn_member(d, sizeof d, "t_pl", idx, 3) == 5);
        ok("bmd_drawn_member refuses an unknown palette",
           bmd_drawn_member(d, sizeof d, "x_pl", idx, 3) == -1);
    }

    if (g_fail) {
        printf("\n%d FAILED\n", g_fail);
        return 1;
    }
    printf("\nall green\n");
    return 0;
}
