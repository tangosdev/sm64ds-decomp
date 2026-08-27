/* func_0205db2c at 0x0205db2c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern unsigned int data_020a8060;
extern int func_0205db78(void);
extern int func_02060918(int a0, int a1, int a2, int a3, void *a4, int a5, int a6);

int func_0205db2c(int a, int b, int c, int d)
{
    func_02060918(data_020a8060, c, b, d, (void *)&func_0205db78, a, 1);
    return 6;
}
