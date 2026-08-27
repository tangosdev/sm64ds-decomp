/* func_020421b4 at 0x020421b4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern char data_020a11e4[];
extern int func_02058158(void *p);
extern void func_02058048(void *p);
extern void func_0205816c(void *p);

void func_020421b4(void)
{
    char *base = (char *)(int)data_020a11e4;
    if (func_02058158(base + 0x400)) return;
    do {
        func_02058048(base + 0x400);
        func_0205816c(base + 0x400);
    } while (!func_02058158(base + 0x400));
}
