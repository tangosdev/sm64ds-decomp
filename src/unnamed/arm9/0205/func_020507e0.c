/* func_020507e0 at 0x020507e0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void func_02050798(void);
extern void MultiStore_Int(int val, int *dst, int len);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern int func_020503a4(int a0, int a1, int a2, int a3,
                         int s0, int s1, int s2, int s3, int s4, int s5,
                         int s6, int s7, int s8, int s9, int s10);
extern int data_020a5634[];

int func_020507e0(int a0, unsigned int a1, int a2, int a3, int a4, int a5, int a6)
{
    volatile int z;
    func_02050798();
    if (data_020a5634[0] != 0) return 0;
    z = 0;
    MultiStore_Int(z, (int *)a0, a1);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)a0, a1);
    return func_020503a4(1, a0, a0 + (a1 >> 1), a1 >> 1,
                         a2, 0, 0, 1, a3, 0x7f, 0, 0x7f, a4, a5, a6);
}