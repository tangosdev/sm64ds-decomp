//cpp
#include "types.h"
#include "cstd.h"

extern "C" {

extern s16 data_02086214[];

void func_02053a8c(void) {}

/* func_020538b8 at 0x020538b8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
int func_020538b8(int x, int y) {
    int num, denom, off, add;
    if (x > 0) {
        if (y > 0) {
            if (y > x) {
                num = x;
                denom = y;
                off = 0;
                add = 1;
            } else if (y < x) {
                num = y;
                denom = x;
                off = 0x4000;
                add = 0;
            } else
                return 0x2000;
        } else if (y < 0) {
            y = -y;
            if (y < x) {
                num = y;
                denom = x;
                off = 0x4000;
                add = 1;
            } else if (y > x) {
                num = x;
                denom = y;
                off = 0x8000;
                add = 0;
            } else
                return 0x6000;
        } else {
            return 0x4000;
        }
    } else if (x < 0) {
        int nx = -x;
        if (y < 0) {
            y = -y;
            if (y > nx) {
                num = nx;
                denom = y;
                off = -0x8000;
                add = 1;
            } else if (y < nx) {
                num = y;
                denom = nx;
                off = -0x4000;
                add = 0;
            } else
                return 0xa000;
        } else if (y > 0) {
            if (y < nx) {
                num = y;
                denom = nx;
                off = -0x4000;
                add = 1;
            } else if (y > nx) {
                num = nx;
                denom = y;
                off = 0;
                add = 0;
            } else
                return 0xe000;
        } else {
            return 0xc000;
        }
    } else {
        return (y >= 0) ? 0 : 0x8000;
    }

    if (denom == 0)
        return 0;
    if (add)
        return (u16)(off + data_02086214[cstd::fdiv(num, denom) >> 5]);
    else
        return (u16)(off - data_02086214[cstd::fdiv(num, denom) >> 5]);
}
}
