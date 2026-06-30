/* func_02011dcc at 0x02011dcc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int func_02051fb4(int *g, int x);
extern void func_02049764(void);
extern void func_02013078(void);
extern void func_0204921c(int *g, int v);
extern int data_0209b4a0[];
extern int data_0208e438;

int func_02011dcc(int *g, int x)
{
    int r4 = func_02051fb4(g, x);
    if (r4 != 0 && g == data_0209b4a0) {
        func_02049764();
        func_02013078();
        if (data_0208e438 >= 0) {
            func_0204921c(g, data_0208e438);
        }
    }
    return r4;
}