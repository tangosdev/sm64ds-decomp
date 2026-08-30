//cpp
// @symbol func_ov075_02116128
/* recovered: dScEntry_c recolours the border of one VS/wireless entry-menu slot box.
 *
 * G2::GetBG1ScrPtr gives the BG1 screen map and func_02030958 maps the slot index to
 * the entry's palette row. The box for `slot` starts at row 4 + 2n, column 4, with a
 * row pitch of 5 + n rows, where n = 4 - data_0209fc50 (the player count), so the
 * entry address is bg + slot * (32n + 0xa0) + (64n + 0x84).
 *
 * The border is written as four single entries - the left and right columns of rows 1
 * and 2 - followed by a 24-wide sweep over rows 0 and 3, which also covers the corners.
 * Each write replaces the 4-bit palette field (bits 12..15) of the map entry and leaves
 * the tile number and flip bits alone: (entry & 0xfff) | (pal << 12), spelled the way
 * the ROM spells it, (v << 0x1c) >> 0x10. `dim` selects the palette four rows earlier,
 * which is how an inactive slot is drawn.
 */
// NONMATCHING: register allocation, at the canonical mwccarm 2004/b56. Twenty divergent
// words by tools/match.py, which compares word by word; nineteen by the near-miss DB's
// scorer in nearmiss/db.jsonl, which is an edit distance over disassembled instructions.
// Both numbers are of the same object; they count different things.
//
// Logic verified against the ROM by differential execution - this body and the
// cartridge's own instructions were interpreted on the same inputs and perform identical
// stores over all 880 input combinations the function can distinguish - not by
// inspection. Counts as decompiled, not matched.
//
// The instruction SCHEDULE is exact; the whole residue is one register rotation. The
// ROM holds the shifted palette value in r3, the store temp and loop invariant in r0
// and the counter in r1. Here the value is born as the callee's return in r0 and the
// truncating lsr writes in place, so it never reaches r3 and the rest rotates with it.
// `#pragma opt_lifetimes off` together with the two-name shift chain below is what
// pulls the truncation onto the ROM's r3 and puts the pointer and counter right;
// neither half does anything on its own. Three attractors, the levers that are
// measured-closed (including the TU hypothesis, in both its forms) and the one open
// angle: notes/mwccarm-codegen.md 6bo.
//
// CAUTION for anyone permuting this function: its search space contains high-scoring
// SEMANTICS-BREAKING shapes. Sinking the `<< 0x1c` below the four single stores scores
// div 6 and writes zero into the palette field of those four entries; the ROM's lsl is
// unconditional and feeds all six sites. A position check does not catch it, because
// mwccarm schedules the lsl before the stores either way - only executing both and
// comparing the stores does. Re-audit anything a search produces here before banking it.
#pragma opt_lifetimes off

extern "C" {
unsigned short *_ZN2G212GetBG1ScrPtrEv();
int func_02030958(int val);
}
extern unsigned char data_0209fc50;

extern "C" void func_ov075_02116128(int unused, int slot, int dim)
{
    unsigned int w;
    unsigned int hi;
    int t;
    int b;
    unsigned short *p;
    int a;
    unsigned short *bg;
    unsigned int angle;
    int m;
    int i;

    b = data_0209fc50;
    bg = _ZN2G212GetBG1ScrPtrEv();
    a = func_02030958(slot);
    t = 4 - b;
    m = slot * (t * 0x20 + 0xa0) + (t * 0x40 + 0x84);
    angle = (unsigned short)(a + 0xa);
    hi = angle;
    if (dim != 0)
        hi = (unsigned short)(hi - 4);
    p = bg + m;
    w = hi << 0x1c;
    hi = w >> 16;
    p[0x20] = (unsigned short)((p[0x20] & 0xfff) | hi);
    p[0x40] = (unsigned short)((p[0x40] & 0xfff) | hi);
    p[0x37] = (unsigned short)((p[0x37] & 0xfff) | hi);
    p[0x57] = (unsigned short)((p[0x57] & 0xfff) | hi);
    for (i = 0; i < 0x18; i++) {
        p[0] = (unsigned short)((p[0] & 0xfff) | hi);
        p[0x60] = (unsigned short)((p[0x60] & 0xfff) | hi);
        p++;
    }
}
