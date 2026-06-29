/* func_02062428 at 0x02062428
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int *func_02061548(void);
extern int func_02061428(int count, ...);
extern int func_02061558(int a, int b, int c, int d, int e, int f, int g, int h, int i);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int len);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int addr, unsigned int len);

int func_02062428(int p0, int p1, int p2, int p3, unsigned short p4, unsigned short p5, unsigned short p6) {
    int a;
    int x;
    int *handle;
    int ret;
    int val184;
    int flag;

    x = 1;
    handle = func_02061548();
    ret = func_02061428(2, 9, 0xa);
    if (ret != 0) return ret;

    _ZN4CP1519InvalidateDataCacheEjj(handle[1] + 0x18e, 2);
    a = *(unsigned short *)(handle[1] + 0x18e);
    _ZN4CP1519InvalidateDataCacheEjj(handle[1] + 0x184, 2);

    val184 = *(unsigned short *)(handle[1] + 0x184);
    flag = (val184 == 0) ? x : 0;
    if (flag == 1) {
        _ZN4CP1519InvalidateDataCacheEjj(handle[1] + 0x17e, 2);
        x = *(unsigned short *)(handle[1] + 0x17e);
        _ZN4CP1519InvalidateDataCacheEjj(handle[1] + 0x86, 2);
    }

    if (p2 == 0) return 6;
    if (x == 0) return 7;
    if (p3 + (flag != 0 ? 4 : 2) > a) return 6;
    if (p3 == 0) return 6;

    _ZN4CP1514FlushDataCacheEjj(p2, p3);
    ret = func_02061558(0xf, 7, p2, p3, p4, p5, p6, p0, p1);
    if (ret == 0) return 2;
    return ret;
}