/* vs_char_palette_gen.h -- per-CATEGORY recolour of a bro's palette.
 *
 * WHAT THIS IS. The Yoshi VS colour path (vs_palette_gen.h) recolours two
 * hardcoded index groups -- body {0,1,2,3} and shoes {7,8} -- of one 16-colour
 * row. The bros (Mario, Luigi, Wario) need the SAME arithmetic driven over
 * ARBITRARY index sets in arbitrary-sized palettes, one per Tango category
 * (hat, overalls, gloves, buttons, shoes, undershirt). The membership comes
 * from vs_char_groups.h (generated from the Studio's groups.json); this header
 * is the thin runtime generator that spends the verified family transform on it.
 *
 * NOTHING HERE REIMPLEMENTS THE MATHS. family_edit and apply_transform in
 * vs_palette_gen.h are already generic over any (row, idx, n, target); they are
 * byte-verified against the Studio's own Rust (the 512-vector oracle). This file
 * only adds the ability to run them IN PLACE on one named palette's words, with
 * a bounds check so a bad index refuses instead of scribbling past the palette.
 *
 * WHY IN PLACE IS SAFE. apply_transform reads rom_row[ci] then writes
 * out_row[ci] once per index, and family_edit reads the whole group up front to
 * fix the anchor and span. The indices in one category are distinct, so passing
 * the same buffer as source and destination reads each word before it is
 * overwritten. Categories are disjoint index sets, so recolouring one never
 * moves a word another category will anchor on. */
#ifndef PORT_HAL_VS_CHAR_PALETTE_GEN_H
#define PORT_HAL_VS_CHAR_PALETTE_GEN_H

#include "vs_palette_gen.h"

namespace vspal {

/* Recolour one category's indices within one palette toward one target colour,
 * IN PLACE. `pal` is pal_ncolors little-endian BGR555 words. `idx`/`n` are the
 * category's palette indices (each must be < pal_ncolors). `target` is 8-bit
 * RGB. Returns 0 on success, or a static reason string when an index is out of
 * range -- in which case NOTHING is written. Shading is preserved exactly as the
 * Yoshi body/shoe path preserves it; an identity target lands the anchor back on
 * itself, so a category recoloured to its own ROM colour is a no-op in bytes. */
static inline const char *char_recolor_palette(u16 *pal, int pal_ncolors,
                                               const int *idx, int n,
                                               const u8 target[3],
                                               int main_idx = -1)
{
    if (n <= 0)
        return 0;
    for (int i = 0; i < n; ++i)
        if (idx[i] < 0 || idx[i] >= pal_ncolors)
            return "category index past the end of the BMD palette";
    int in_group = 0;
    for (int i = 0; i < n; ++i)
        if (idx[i] == main_idx) in_group = 1;
    FamilyEdit e;
    const int anchor = in_group ? main_idx : family_rep(pal, idx, n);
    const u16 old_anchor = pal[anchor];
    family_edit_main(pal, idx, n, anchor, target, &e);
    apply_transform(pal, idx, n, &e, pal);
    pal[anchor] = rgb_word(target, old_anchor);   /* the anchor IS the pick */
    return 0;
}

/* ---- the drawn shade of a bro's category (run after093, YOSHICOLOR1) -------
 *
 * The same rule as the Yoshi's (vs_palette_gen.h, kBodyMain): the picked colour
 * lands on the shade the model DRAWS most, not on family_rep's punchiest one.
 * A bro's category is a scatter of near shades in a 4x4-compressed texture, so
 * the drawn shade is counted here from the served BMD itself: every texture a
 * material pairs with the named palette, each texel credited to the palette
 * word it reads (a mode 1 / 3 blended texel half to each end). On the ROM's
 * Mario gloves family_rep is 8b8b8b while the most-drawn glove shade is bdbdbd,
 * so a glove pick used to be drawn lighter than picked, as the Yoshi's body was.
 * Returns the member with the most texels, or -1 when nothing is counted (then
 * char_recolor_palette keeps family_rep). Pure: reads the buffer, writes
 * nothing. Offsets are the BMD header's (+0x14 textures, +0x1c palettes, +0x24
 * materials; texture 0x14, palette 0x10, material 0x30 records). */
static inline u32 cp_rd32(const u8 *d, u32 o)
{
    return (u32)d[o] | ((u32)d[o + 1] << 8) | ((u32)d[o + 2] << 16) |
           ((u32)d[o + 3] << 24);
}

static inline int bmd_drawn_member(const u8 *d, u32 len, const char *pal_name,
                                   const int *idx, int n)
{
    if (!d || len < 0x3c || n <= 0)
        return -1;
    u32 ntex = cp_rd32(d, 0x14), texo = cp_rd32(d, 0x18);
    u32 npal = cp_rd32(d, 0x1c), palo = cp_rd32(d, 0x20);
    u32 nmat = cp_rd32(d, 0x24), mato = cp_rd32(d, 0x28);
    if (ntex > 4096 || npal > 4096 || nmat > 4096)
        return -1;
    int pi = -1;
    u32 pal_words = 0;
    for (u32 i = 0; i < npal && pi < 0; ++i) {
        u32 rec = palo + i * 0x10;
        if (rec + 0x10 > len)
            return -1;
        u32 nm = cp_rd32(d, rec);
        u32 k = 0;
        while (nm + k < len && pal_name[k] && d[nm + k] == (u8)pal_name[k])
            ++k;
        if (!pal_name[k] && nm + k < len && d[nm + k] == 0) {
            pi = (int)i;
            pal_words = cp_rd32(d, rec + 8) / 2;
        }
    }
    if (pi < 0)
        return -1;
    double cnt[64];
    if (n > 64)
        return -1;
    for (int i = 0; i < n; ++i)
        cnt[i] = 0.0;
    for (u32 m = 0; m < nmat; ++m) {
        u32 rec = mato + m * 0x30;
        if (rec + 0x30 > len)
            return -1;
        int ti = (int)cp_rd32(d, rec + 4), pj = (int)cp_rd32(d, rec + 8);
        if (pj != pi || ti < 0 || (u32)ti >= ntex)
            continue;
        int dup = 0;                      /* count each texture once */
        for (u32 e = 0; e < m && !dup; ++e) {
            u32 r2 = mato + e * 0x30;
            if ((int)cp_rd32(d, r2 + 4) == ti && (int)cp_rd32(d, r2 + 8) == pi)
                dup = 1;
        }
        if (dup)
            continue;
        u32 tr = texo + (u32)ti * 0x14;
        if (tr + 0x14 > len)
            return -1;
        u32 dp = cp_rd32(d, tr + 4), sz = cp_rd32(d, tr + 8);
        u32 fl = cp_rd32(d, tr + 0x10);
        u32 fmt = (fl >> 26) & 7, w = 8u << ((fl >> 20) & 7), h = 8u << ((fl >> 23) & 7);
        u32 texels = w * h;
        for (u32 t = 0; t < texels; ++t) {
            u32 a = 0xffffffffu, b = 0xffffffffu;
            double wt = 1.0;
            if (fmt == 3) {
                if (dp + (t >> 1) >= len) break;
                a = (d[dp + (t >> 1)] >> ((t & 1) * 4)) & 15;
            } else if (fmt == 4) {
                if (dp + t >= len) break;
                a = d[dp + t];
            } else if (fmt == 2) {
                if (dp + (t >> 2) >= len) break;
                a = (d[dp + (t >> 2)] >> ((t & 3) * 2)) & 3;
            } else if (fmt == 5) {
                u32 bx = (t % w) >> 2, by = (t / w) >> 2, bi = by * (w >> 2) + bx;
                u32 k = ((t / w) & 3) * 4 + ((t % w) & 3);
                if (dp + bi * 4 + 4 > len || dp + sz + bi * 2 + 2 > len) break;
                u32 blk = cp_rd32(d, dp + bi * 4);
                u32 info = (u32)d[dp + sz + bi * 2] | ((u32)d[dp + sz + bi * 2 + 1] << 8);
                u32 base = (info & 0x3fff) * 2, mode = info >> 14, c = (blk >> (k * 2)) & 3;
                if (c == 3 && mode <= 1)
                    continue;             /* transparent */
                if ((mode == 1 && c == 2) || (mode == 3 && c >= 2)) {
                    a = base; b = base + 1; wt = 0.5;
                } else {
                    a = base + c;
                }
            } else {
                break;                    /* a3i5 / a5i3 / direct: not counted */
            }
            for (int i = 0; i < n; ++i) {
                if ((u32)idx[i] == a && a < pal_words) cnt[i] += wt;
                if ((u32)idx[i] == b && b < pal_words) cnt[i] += wt;
            }
        }
    }
    int best = -1;
    double bc = 0.0;
    for (int i = 0; i < n; ++i)
        if (cnt[i] > bc) { bc = cnt[i]; best = idx[i]; }
    return best;
}

} /* namespace vspal */

#endif /* PORT_HAL_VS_CHAR_PALETTE_GEN_H */
