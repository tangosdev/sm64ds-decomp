/* func_020625fc at 0x020625fc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef unsigned short u16;
typedef unsigned int u32;

extern int *func_02061548(void);
extern int func_02061428(int a, int b, int c);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int n);
extern int func_02061ab0(void);
extern int func_02061b9c(void);
extern void func_0206165c(int i, int val);
extern int func_02061558(int a, int b, int c, int d, int e, int f, int g,
                         int h, int i, int j, int k, int l);

int func_020625fc(int arg0, int arg1, int arg2, int arg3, u16 arg4, u16 arg5,
                  u16 arg6, int arg7, int arg8, int arg9, int arg10)
{
    int *base;
    int r;

    base = func_02061548();
    r = func_02061428(2, 7, 8);
    if (r != 0)
        return r;
    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(base[1] + 0x10), 4);
    if (((int *)base[1])[4] == 1)
        return 3;
    if (arg2 < func_02061ab0())
        return 6;
    if (arg2 & 0x3f)
        return 6;
    if (arg4 < func_02061b9c())
        return 6;
    if (arg4 & 0x1f)
        return 6;
    func_0206165c(0xe, arg0);
    r = func_02061558(0xe, 0xa, arg1, (unsigned int)arg2 >> 1,
                      arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    if (r == 0)
        r = 2;
    return r;
}