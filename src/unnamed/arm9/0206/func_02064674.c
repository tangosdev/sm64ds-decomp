/* func_02064674 at 0x02064674
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int func_02065bd0(void);
extern void CpuCopy8(int a, int b, int c);

int func_02064674(int *p, int idx, int dst)
{
    int n = func_02065bd0();
    int cnt = p[1];
    int base = p[6];
    if (idx < cnt) {
        if (idx == cnt - 1) {
            int rem = p[0] % n;
            if (rem != 0)
                CpuCopy8(dst, idx * n + base, rem);
            else
                CpuCopy8(dst, idx * n + base, n);
        } else {
            CpuCopy8(dst, idx * n + base, n);
        }
    } else {
        return 0;
    }
    return 1;
}