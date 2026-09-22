//cpp
// @symbol _ZN10dMgPsOpt_c11TouchIcon_c6RenderEv
/* dMgPsOpt_c::TouchIcon_c::Render -- slot 1 of the nested touch icon's vtable.

   The icon draws itself one of two ways depending on unk_01c, the style word
   dScMgBase_c's menu code writes when it builds the eight icons: styles 2 and 3
   hand a sprite table to the shared blitter and nudge x by two pixels while the
   icon is held down (unk_010), and styles 0, 1 and 6 recolour a rectangle of BG1
   screen entries in place, style 0 first laying down a fresh 13-entry run.

   MIGRATED FROM src/func_ov004_020b8c18.c. It is this class's own Render
   override -- the cartridge's vtable at ov004:0x020b8c30 holds 0x020b8c18 in
   slot 1 where dThIcon_c's default sits in the base's -- but the class never
   declared the override, so mwcc emitted dThIcon_c::Render's address there and
   romdata_check scored _ZTVN10dMgPsOpt_c11TouchIcon_cE DIFFERS. Declaring it in
   include/dMgPsOpt_c.h and giving this body its mangled name is what closes
   that word; the two-slot table is byte-exact against the cartridge now.

   The seven fields it touches are dThIcon_c's own, so the raw `char *self`
   offsets the C source used are gone; nothing else about the body changed. */
#include "types.h"
#include "dMgPsOpt_c.h"

extern "C" {

extern u16 *_ZN3G2S12GetBG1ScrPtrEv(void);
extern void func_ov004_020aea78(void *self, int a1, int a2, int a3);

extern u16 data_ov004_020bca58[];
extern u16 data_ov004_020bca60[];
}

void dMgPsOpt_c::TouchIcon_c::Render()
{
    volatile s16 xy[2];
    void *tbl;
    u16 *p;
    u16 v;
    int i;
    int w;
    int mask;
    int row;
    int col;
    int y;
    int h;
    int x;

    xy[0] = unk_004;
    xy[1] = unk_006;

    switch (unk_01c) {
    case 2:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] - 2);
        tbl = data_ov004_020bca58;
        break;

    case 3:
        if (unk_010 != 0)
            xy[0] = (s16)(xy[0] + 2);
        tbl = data_ov004_020bca60;
        break;

    case 0:
        p = (u16 *)((char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x19e);
        v = (u16)((unk_020 << 6) + 0x13);
        for (i = 0; i < 13; i++) {
            p[0] = v;
            p[0x20] = (u16)(v + 0x20);
            p++;
            v++;
        }
        /* fallthrough */
    case 1:
    case 6:
        mask = (unk_010 != 0) ? 0 : 0x3000;
        w = unk_008;
        p = _ZN3G2S12GetBG1ScrPtrEv();
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

    default:
        return;
    }

    func_ov004_020aea78(tbl, xy[0], xy[1], 0);
}
