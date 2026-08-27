/* _ZN4cstd5atan2E5Fix12IiES1_ at 0x0203b4dc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef int s32;
typedef short s16;

extern s32 _ZN4cstd4fdivEii(s32 num, s32 den);
extern s32 func_01ffa4bc(s32 a);
extern s32 func_01ff98f4(s32 a, s32 b);
extern unsigned short data_020994e0[];

s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x)
{
    s32 r;
    if (y == 0) {
        if (x >= 0)
            r = 0;
        else
            r = 0x8000;
    } else if (x == 0) {
        if (y >= 0)
            r = 0x4000;
        else
            r = 0xc000;
    } else if (y >= 0) {
        if (x >= 0) {
            if (x >= y)
                r = data_020994e0[_ZN4cstd4fdivEii(y, x) >> 2];
            else
                r = 0x4000 - data_020994e0[_ZN4cstd4fdivEii(x, y) >> 2];
        } else {
            if (-x < y)
                r = data_020994e0[_ZN4cstd4fdivEii(-x, y) >> 2] + 0x4000;
            else
                r = 0x8000 - data_020994e0[_ZN4cstd4fdivEii(y, -x) >> 2];
        }
    } else if (func_01ff98f4(func_01ffa4bc(x), 0)) {
        if (x <= y)
            r = data_020994e0[_ZN4cstd4fdivEii(-y, -x) >> 2] + 0x8000;
        else
            r = 0xc000 - data_020994e0[_ZN4cstd4fdivEii(-x, -y) >> 2];
    } else {
        if (x < -y)
            r = data_020994e0[_ZN4cstd4fdivEii(x, -y) >> 2] + 0xc000;
        else
            r = -data_020994e0[_ZN4cstd4fdivEii(-y, x) >> 2];
    }
    return (s16)r;
}