/* func_ov007_020bda8c at 0x020bda8c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */
extern int _ZN4cstd3divEii(int a, int b);
extern void func_0204f8cc(void* obj, int a, int b);
extern void func_ov007_020bde2c(int idx);
extern void func_0204f79c(void* obj, int a, int b);
extern void func_0204f76c(void* obj, int a, int b);
extern char* data_ov007_02104bbc;

static inline int scale_div(int v, int range, int scale)
{
    int q;
    if (v <= 0)
        q = 0;
    else if (v >= range)
        q = 0x1000;
    else
        q = _ZN4cstd3divEii(v << 12, range);
    return (q * scale) >> 12;
}

void func_ov007_020bda8c(int a, int b)
{
    char* obj = data_ov007_02104bbc + 0xd8;
    int r4, r6;

    if (b == 0) {
        r4 = scale_div(a, 0xb4, 0x7f);
        r6 = scale_div(a, 0x78, 0x23) + 5;
    } else {
        int v = 0x1e - a;
        r4 = scale_div(v, 0x1e, 0x7f);
        r6 = scale_div(v, 0x1e, 0x23) + 5;
        if (a == 0) {
            func_0204f8cc(obj, 0, 0x1e);
        }
    }
    func_ov007_020bde2c(0x36);
    func_0204f79c(obj, 0xf, r4);
    func_0204f76c(obj, 0xf, r6);
}
