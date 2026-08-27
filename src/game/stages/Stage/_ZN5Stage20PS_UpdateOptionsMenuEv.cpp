//cpp
#include "types.h"
namespace G2S { u16 *GetBG1ScrPtr(); }

extern u8 data_0209f2e4;
extern u16 data_020755c0[];
extern u8 data_0209f2e0;
extern u8 data_0209f2cc;
extern u16 data_020755c4[];
extern u8 data_0209f2ec;

struct Stage {
    static void PS_UpdateOptionsMenu();
};

void Stage::PS_UpdateOptionsMenu()
{
    int i;
    int j;
    int counter;
    u16 sel;
    u16 *scr;
    u16 v;

    sel = (data_0209f2e4 != 0) ? 0x5000 : 0x6000;
    scr = G2S::GetBG1ScrPtr() + 0xae;
    for (j = 0; j < 0xf; j++) {
        scr[0]    = (scr[0]    & 0x3ff) | sel;
        scr[0x20] = (scr[0x20] & 0x3ff) | sel;
        scr[0x40] = (scr[0x40] & 0x3ff) | sel;
        scr[0x60] = (scr[0x60] & 0x3ff) | sel;
        scr++;
    }

    for (j = 0; j < 2; j++) {
        scr = G2S::GetBG1ScrPtr() + data_020755c0[j];
        v = (data_0209f2e0 == j) ? 0x4000 : 0x3000;
        counter = 0;
        for (; counter < 0xa; counter++) {
            scr[0]    = (scr[0]    & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr++;
        }
    }

    for (i = 0; i < 2; i++) {
        scr = G2S::GetBG1ScrPtr() + data_020755c4[i];
        v = (data_0209f2ec == i && data_0209f2cc == 0) ? 0x6000 : 0x5000;
        counter = 0;
        for (; counter < 7; counter++) {
            scr[0]    = (scr[0]    & 0x3ff) | v;
            scr[0x20] = (scr[0x20] & 0x3ff) | v;
            scr[0x40] = (scr[0x40] & 0x3ff) | v;
            scr[0x60] = (scr[0x60] & 0x3ff) | v;
            scr++;
        }
    }
}
