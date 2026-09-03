/* vs_char_palette_gen.h -- per-CATEGORY recolour of a bro's palette.
 *
 * WHAT THIS IS. The Yoshi VS colour path (vs_palette_gen.h) recolours two
 * hardcoded index groups -- body {0,1,2,3} and shoes {7,8} -- of one 16-colour
 * row. The bros (Mario, Luigi, Wario) need the SAME arithmetic driven over
 * ARBITRARY index sets in arbitrary-sized palettes, one per Tango category
 * (hat+undershirt tied, overalls, gloves, buttons, shoes). The membership comes
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
                                               const u8 target[3])
{
    if (n <= 0)
        return 0;
    for (int i = 0; i < n; ++i)
        if (idx[i] < 0 || idx[i] >= pal_ncolors)
            return "category index past the end of the BMD palette";
    FamilyEdit e;
    family_edit(pal, idx, n, target, &e);
    apply_transform(pal, idx, n, &e, pal);
    return 0;
}

} /* namespace vspal */

#endif /* PORT_HAL_VS_CHAR_PALETTE_GEN_H */
