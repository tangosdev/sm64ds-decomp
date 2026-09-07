//cpp
// @symbol _ZN3HUD15CalculateDigitsEt
/* recovered: named members + shared header, real C++ method
 *
 * Splits a count into the three decimal digits the HUD draws, most significant
 * first, and blanks leading zeros by writing -1 rather than a digit.
 *
 * The suppression stops at i != 2, so the units digit is always drawn: a value
 * of 0 renders "0", not blank. `flag` latches on the first non-zero digit, so
 * an interior zero (as in 101) is drawn normally.
 *
 * The divisor table is volatile in the ROM's own spelling and stays that way --
 * it forces the 100/10/1 loads to be re-read per iteration instead of folded
 * into immediates, which is what makes this a loop at all.
 */
#include "HUD.h"

extern "C" {
extern volatile unsigned short data_ov002_0210c208[];
}

void HUD::CalculateDigits(unsigned short value)
{
    int flag = 0;
    int i;
    for (i = 0; i < 3; i++) {
        int digit = value / data_ov002_0210c208[i];
        if (digit == 0 && flag == 0 && i != 2) {
            mDigits[i] = -1;
        } else {
            mDigits[i] = digit;
            flag = 1;
        }
        value = value % data_ov002_0210c208[i];
    }
}
