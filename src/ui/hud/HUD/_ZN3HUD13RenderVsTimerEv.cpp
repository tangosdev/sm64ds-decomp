//cpp
// @symbol _ZN3HUD13RenderVsTimerEv
/* recovered: named members + shared header, real C++ method
 *
 * The VS-mode countdown: a localised "TIME" caption, and below it either the
 * remaining seconds or a localised "TIME UP".
 *
 * Both captions are chosen by a GetOwnerLanguage() ladder testing 5, 4, 3, 2
 * and falling through to OAM::VS_TIME / OAM::VS_TIME_UP -- so the OAM symbols
 * are the DEFAULT artwork and the four data_ov002_* blocks are translations.
 * The ladder re-calls GetOwnerLanguage() at every rung rather than caching it;
 * that is the ROM's own shape and is preserved rather than tidied.
 *
 * Digit layout is not uniform, which is the interesting part:
 *   >= 10   two glyphs at x=0x74 and x=0x84, recoloured to palette 0xb once
 *           mVsTimer <= 5 -- the last-five-seconds warning
 *   4..9    ONE glyph at x=0x7c, centred, there being no tens digit
 *   0..3    not a digit at all: data_ov002_0210ca0c[3 - ones], a separate
 *           countdown artwork set, indexed backwards
 *
 * Only the caption is drawn when any of data_0209f2c4 / f20c / f294 is set;
 * they are OR-ed and tested as a single byte.
 */
struct OamAttr;

#include "HUD.h"

extern "C" {

extern unsigned char data_0209f2c4;
extern unsigned char data_0209f20c;
extern unsigned char data_0209f294;
extern unsigned char data_0209f204;

extern struct OamAttr data_ov002_0210d290;
extern struct OamAttr data_ov002_0210d2c0;
extern struct OamAttr data_ov002_0210d300;
extern struct OamAttr data_ov002_0210d338;
extern struct OamAttr _ZN3OAM7VS_TIMEE;
extern struct OamAttr* _ZN3OAM17VS_YELLOW_NUMBERSE[];
extern struct OamAttr* data_ov002_0210ca0c[];
extern struct OamAttr data_ov002_0210d2a0;
extern struct OamAttr data_ov002_0210d2d0;
extern struct OamAttr data_ov002_0210d308;
extern struct OamAttr data_ov002_0210d348;
extern struct OamAttr _ZN3OAM10VS_TIME_UPE;

extern int GetOwnerLanguage(void);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, struct OamAttr* attr, int x, int y, int a, int b, int sx, int sy, int c, int d);
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int sub, struct OamAttr* attr, int x, int y, int a, int b, void* m);
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int sub, struct OamAttr* attr, int x, int y, int a, int b, int s, int c);
}

void HUD::RenderVsTimer()
{
    int t = mVsTimer;

    if ((unsigned char)(data_0209f2c4 | data_0209f20c | data_0209f294) == 0) {
        if (GetOwnerLanguage() == 5) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d290, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 4) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d2c0, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 3) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d300, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d338, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM7VS_TIMEE, 0x84, 9, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }

    if (data_0209f204 == 0) {
        int pal = (mVsTimer <= 5) ? 0xb : -1;
        unsigned short tens = t / 10;
        if (tens != 0) {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM17VS_YELLOW_NUMBERSE[tens], 0x74, mVsTimerY, pal, -1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM17VS_YELLOW_NUMBERSE[t % 10], 0x84, mVsTimerY, pal, -1, 0);
        } else {
            int ones = t % 10;
            if (ones > 3) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, _ZN3OAM17VS_YELLOW_NUMBERSE[ones], 0x7c, mVsTimerY, -1, -1, 0x1000, 0);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, data_ov002_0210ca0c[3 - ones], 0x84, mVsTimerY, -1, -1, 0);
            }
        }
    } else {
        if (GetOwnerLanguage() == 5) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d2a0, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 4) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d2d0, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 3) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d308, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else if (GetOwnerLanguage() == 2) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210d348, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &_ZN3OAM10VS_TIME_UPE, 0x80, mVsTimerY, -1, -1, 0x1000, 0x1000, 0, -1);
        }
    }
}
