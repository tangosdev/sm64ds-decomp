//cpp
// @symbol func_ov075_02116128
/* recovered: dScEntry_c recolours the border of one VS/wireless entry-menu slot box.
 *
 * G2::GetBG1ScrPtr gives the BG1 screen map and func_02030958 maps the slot index to
 * the entry's palette row. The box for `slot` starts at row 4 + 2n, column 4, with a
 * row pitch of 5 + n rows, where n = 4 - data_0209fc50 (the player count), so the
 * entry offset is slot * (32n + 0xa0) + (64n + 0x84).
 *
 * The border is written as four single entries - the left and right columns of rows 1
 * and 2 - followed by a 24-wide sweep over rows 0 and 3, which also covers the corners.
 * Each write replaces the 4-bit palette field (bits 12..15) of the map entry and leaves
 * the tile number and flip bits alone. `dim` selects the palette four rows earlier,
 * which is how an inactive slot is drawn.
 *
 * The palette is a u16 and the field is spelled `(u16)(pal << 12)`: mwccarm narrows
 * that as `lsl #0x1c` in place on the palette's own register, then `lsr #16` folded
 * into each `orr`, with the loop's copy hoisted - exactly the ROM's register use.
 * Spelling the shift through separate named temporaries gives the same schedule but
 * a rotated register assignment.
 */
#include "types.h"

extern "C" {
u16 *_ZN2G212GetBG1ScrPtrEv();
int func_02030958(int val);
}
extern u8 data_0209fc50;

static inline void SetPalette(u16 *entry, u16 pal)
{
    *entry = (*entry & 0xfff) | (u16)(pal << 12);
}

extern "C" void func_ov075_02116128(int unused, int slot, int dim)
{
    int players = data_0209fc50;
    u16 *map = _ZN2G212GetBG1ScrPtrEv();
    u16 pal = func_02030958(slot) + 10;
    u16 *p;
    int i;
    int n = 4 - players;
    int offset = slot * (n * 0x20 + 0xa0) + (n * 0x40 + 0x84);

    p = map + offset;
    if (dim)
        pal -= 4;

    SetPalette(&p[0x20], pal);
    SetPalette(&p[0x40], pal);
    SetPalette(&p[0x37], pal);
    SetPalette(&p[0x57], pal);
    for (i = 0; i < 24; i++) {
        SetPalette(&p[0], pal);
        SetPalette(&p[0x60], pal);
        p++;
    }
}
