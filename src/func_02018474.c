/* func_02018474 at 0x02018474
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct V2 { int x, y; };

extern void func_0205d874(int *s);
extern int func_02018dc4(int buf, struct V2 ab);
extern void Crash(void);
extern int func_020184e0(int *buf, unsigned int c, unsigned int d);

int func_02018474(int a, int b, int c, int d, ...)
{
    int buf[0x11];
    func_0205d874(buf);
    if (func_02018dc4((int)buf, *(struct V2 *)&a) == 0) {
        Crash();
        return 0;
    }
    return func_020184e0(buf, c, d);
}
