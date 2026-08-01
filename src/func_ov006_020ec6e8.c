/* func_ov006_020ec6e8 at 0x020ec6e8 (ov006)
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * Note: the (res | 3) / (res | 0xc) tests reproduce the ROM's orrs
 * instructions - original code likely meant & (bounds flags from
 * func_ov006_020ebeb4) but the | is what the ROM contains.
 */
typedef short s16;

extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int func_ov006_020ebeb4(char *c);

void func_ov006_020ec6e8(char *c)
{
    int res = func_ov006_020ebeb4(c);
    if (res != 0) {
        int tx;
        int ty;
        int y;
        int x;
        s16 a;
        s16 d;
        x = *(int*)(c + 0x20);
        y = *(int*)(c + 0x24);
        tx = x;
        ty = y;
        if (res | 3) tx = 0x80000;
        if (res | 0xc) ty = 0x60000;
        a = _ZN4cstd5atan2E5Fix12IiES1_(tx - x, -(ty - y));
        d = (s16)(a - *(s16*)(c + 0x7a));
        if (d > 0x2000) d = 0x2000;
        else if (d < -0x2000) d = -0x2000;
        *(s16*)(c + 0x76) = (s16)(*(s16*)(c + 0x7c) + d);
        return;
    }
    {
        int ra;
        int rb;
        s16 a;
        s16 d;
        *(s16*)(int)(((long long)(int)(c + 0x78))) -= 1;
        if (*(s16*)(c + 0x78) >= 0) return;
        *(s16*)(c + 0x78) = (s16)((((int)((unsigned int)(RandomIntInternal(&data_0209e650) & 0x7fffffff) >> 19) * 0x1e) >> 12) + 0x3c);
        ra = RandomIntInternal(&data_0209e650);
        rb = RandomIntInternal(&data_0209e650);
        a = _ZN4cstd5atan2E5Fix12IiES1_(
            ((int)((unsigned int)(ra & 0x7fffffff) >> 19) * 0x100) - *(int*)(c + 0x20),
            -(((int)((unsigned int)(rb & 0x7fffffff) >> 19) * 0xc0) - *(int*)(c + 0x24)));
        d = (s16)(a - *(s16*)(c + 0x7a));
        if (d > 0x2000) d = 0x2000;
        else if (d < -0x2000) d = -0x2000;
        *(s16*)(c + 0x76) = (s16)(*(s16*)(c + 0x7c) + d);
    }
}
