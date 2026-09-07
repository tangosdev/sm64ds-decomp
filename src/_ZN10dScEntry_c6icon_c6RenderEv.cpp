//cpp
// @symbol _ZN10dScEntry_c6icon_c6RenderEv
#include "dScEntry_c.h"

extern "C" {
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int a, void *oam, int b, int c, int d, int e, int f, int g, int h, int i);
extern u16 *_ZN3G2S12GetBG2ScrPtrEv(void);
extern int func_0203d974(void);
extern void _ZN3OAM9RenderSubEP7OamAttrii(void *oam, int x, int y);

extern int data_ov075_0211cb64[];
extern char data_ov075_0211c720[];
extern char data_ov075_0211c730[];
extern void *data_ov075_0211c954[];
}

void dScEntry_c::icon_c::Render()
{
    volatile s16 xy[2];
    u16 *p;
    int mask;
    int row;
    int col;
    int w;
    int h;
    void *oam;
    int y;
    int x;

    xy[0] = unk_004;
    xy[1] = unk_006;

    switch (unk_01c) {
    case 7:
    {
        int idx;
        void *ptr;
        int v;

        idx = unk_020;
        if (unk_010 == 0) {
            if (unk_011 != 0)
                idx += 4;
        }
        ptr = (void *)data_ov075_0211cb64[idx];

        v = unk_00c;
        if (v <= 0) {
            v = 0x1000;
        } else if (v < 4) {
            v = (v << 8) + 0x1000;
        } else {
            v = ((8 - v) << 8) + 0x1000;
        }

        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(1, ptr, xy[0], xy[1], -1, -1, v, v, 0, -1);
        return;
    }

    case 0:
    case 1:
    case 2:
        mask = (unk_010 != 0) ? 0x6000 : 0x5000;
        goto fill;

    case 8:
    case 9:
    case 10:
    case 11:
        mask = (unk_010 != 0) ? 0x2000 : 0x1000;
    fill:
        w = unk_008;
        p = _ZN3G2S12GetBG2ScrPtrEv();
        x = unk_004;
        h = unk_00a;
        x -= w;
        x >>= 3;
        p += x;
        y = unk_006;
        y -= h;
        p += (y >> 3) << 5;
        for (row = 0; row < unk_00a >> 2; row++) {
            for (col = 0; col < w >> 2; col++) {
                p[col] = (u16)(mask | (p[col] & 0xfff));
                w = unk_008;
            }
            p += 0x20;
        }
        return;

    case 3:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] - 2);
        oam = data_ov075_0211c720;
        goto renderSub;

    case 4:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] + 2);
        oam = data_ov075_0211c730;
        goto renderSub;

    case 13:
        if (func_0203d974() != 0 && unk_020 >= 4)
            return;
        oam = data_ov075_0211c954[unk_020];
        goto renderSub;

    case 5:
    case 6:
    case 12:
    default:
        return;
    }

renderSub:
    _ZN3OAM9RenderSubEP7OamAttrii(oam, xy[0], xy[1]);
}
