//cpp
// @symbol _ZN3HUD15RenderStarCountEv
/* recovered: named members + shared header, real C++ method
 *
 * Draws the star counter: the star icon, an "x", and up to three digits laid
 * out RIGHT to LEFT from mStarCountX, skipping the -1 slots CalculateDigits
 * blanked.
 *
 * Three paths, and what differs between them is only WHICH count is shown and
 * on WHICH screen:
 *
 *   data_0209f2d8 == 1   a per-slot count out of data_0209f310, main screen
 *   the middle path      NumStars(), main screen, but only during a specific
 *                        cutscene window (data_ov002_02111178) and suppressed
 *                        entirely by the data_0209f2ac / data_0209f2d4 test
 *   otherwise            NumStars(), SUB screen -- the leading argument to
 *                        OAM::Render flips 0 -> 1
 *
 * The three render blocks are deliberately left duplicated rather than folded
 * into a helper: they are three separate tails in the ROM, and hoisting them
 * would be a rewrite, not a substitution.
 *
 * The digit loop runs i = 2 down to 0 -- least significant first -- which is
 * why the cursor moves left.
 */
#include "HUD.h"

struct OamAttr;
struct Matrix2x2;

extern "C" {

extern unsigned char data_0209f2d8;
extern unsigned char data_0209f250;
extern signed char data_0209f310[];
extern unsigned char data_0209f2fc;
extern unsigned char data_ov002_02111178;
extern unsigned char data_0209f2ac;
extern unsigned char data_0209f2d4;
extern int data_020a0db0;

extern struct OamAttr* _ZN3OAM7NUMBERSE[];
extern struct OamAttr _ZN3OAM5TIMESE;
extern struct OamAttr _ZN3OAM10POWER_STARE;

extern unsigned char NumStars(void);
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int b, struct OamAttr* attr, int x, int y, int a, int c, struct Matrix2x2* m);
}

void HUD::RenderStarCount()
{
    int x = mStarCountX;
    int b = (data_0209f2d8 == 1);
    int i;

    if (b) {
        CalculateDigits((unsigned short)data_0209f310[data_0209f250]);
        for (i = 2; i >= 0; i--) {
            if (mDigits[i] >= 0) {
                _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM7NUMBERSE[mDigits[i]], x, 2, -1, 1, 0);
                x -= 9;
            }
        }
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, x, 10, -1, 1, 0);
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM10POWER_STARE, x - 16, 10, -1, 1, 0);
        return;
    }

    CalculateDigits(NumStars());

    {
        unsigned char m = data_0209f2fc;
        if ((m != 2 && data_ov002_02111178 == 4) ||
            (m == 1 && data_ov002_02111178 >= 3 && data_ov002_02111178 < 6)) {
            int flag;
            if (data_0209f2ac != 0) {
                if (data_0209f2d4 == 3 && (data_020a0db0 & 0x18) == 0) {
                    flag = 0;
                } else {
                    flag = 1;
                }
            } else {
                flag = 1;
            }
            if (flag == 0) {
                return;
            }
            for (i = 2; i >= 0; i--) {
                if (mDigits[i] >= 0) {
                    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM7NUMBERSE[mDigits[i]], x, 2, -1, 1, 0);
                    x -= 9;
                }
            }
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM5TIMESE, x, 10, -1, 1, 0);
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, &_ZN3OAM10POWER_STARE, x - 16, 10, -1, 1, 0);
            return;
        }
    }

    for (i = 2; i >= 0; i--) {
        if (mDigits[i] >= 0) {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, _ZN3OAM7NUMBERSE[mDigits[i]], x, 2, -1, 1, 0);
            x -= 9;
        }
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM5TIMESE, x, 10, -1, 1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, &_ZN3OAM10POWER_STARE, x - 16, 10, -1, 1, 0);
}
