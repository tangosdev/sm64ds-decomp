//cpp
// @symbol _ZN10dMgPsOpt_c11TouchIcon_c6RenderEv
/* dMgPsOpt_c::TouchIcon_c::Render -- slot 1 at ov004:0x020bca80 of
 * the nested class's vtable (address point 0x020bca7c). The inherited
 * dThIcon_c::Behavior remains in slot 0.
 *
 * Styles 2 and 3 draw sprites shifted two pixels while held. Styles 0, 1
 * and 6 recolour BG1 screen entries; style 0 first writes the sound-mode
 * tiles. All accessed fields belong to dThIcon_c. */
#include "types.h"
#include "dMgPsOpt_c.h"

struct M;
struct S;

namespace G2S {
u32 GetBG1ScrPtr();
}

extern "C" {

extern void func_ov004_020aea78(S *sprite, int a1, int a2, struct M *a3);

extern u16 data_ov004_020bca58[];
extern u16 data_ov004_020bca60[];
}

void dMgPsOpt_c::TouchIcon_c::Render()
{
    // Plain coordinates remove 16 bytes under the pinned compiler.
    volatile s16 position[2];
    u16 *sprite;
    u16 *mapEntries;
    u16 tileIndex;
    int tileColumn;
    int halfWidth;
    int paletteMask;
    int row;
    int col;
    int pixelY;
    int halfHeight;
    int tileX;

    position[0] = unk_004;
    position[1] = unk_006;

    switch (unk_01c) {
    case 2:
        if (unk_010 != 0)
            position[0] = (s16)(position[0] - 2);
        sprite = data_ov004_020bca58;
        break;

    case 3:
        if (unk_010 != 0)
            position[0] = (s16)(position[0] + 2);
        sprite = data_ov004_020bca60;
        break;

    case 0:
        mapEntries = (u16 *)(G2S::GetBG1ScrPtr() + 0x19e);
        tileIndex = (u16)((unk_020 << 6) + 0x13);
        for (tileColumn = 0; tileColumn < 13; tileColumn++) {
            mapEntries[0] = tileIndex;
            mapEntries[0x20] = (u16)(tileIndex + 0x20);
            mapEntries++;
            tileIndex++;
        }
        /* fallthrough */
    case 1:
    case 6:
        paletteMask = (unk_010 != 0) ? 0 : 0x3000;
        halfWidth = unk_008;
        mapEntries = (u16 *)G2S::GetBG1ScrPtr();
        tileX = unk_004;
        halfHeight = unk_00a;
        tileX -= halfWidth;
        tileX >>= 3;
        mapEntries += tileX;
        pixelY = unk_006;
        pixelY -= halfHeight;
        mapEntries += (pixelY >> 3) << 5;
        for (row = 0; row < unk_00a >> 2; row++) {
            for (col = 0; col < halfWidth >> 2; col++) {
                mapEntries[col] = (u16)(paletteMask | (mapEntries[col] & 0xfff));
                halfWidth = unk_008;
            }
            mapEntries += 0x20;
        }
        return;

    default:
        return;
    }

    func_ov004_020aea78((S *)sprite, position[0], position[1], 0);
}
