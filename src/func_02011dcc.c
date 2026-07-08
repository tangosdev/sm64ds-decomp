/* func_02011dcc at 0x02011dcc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern int func_02051fb4(int *g, int x);
extern void func_02049764(void);
extern void func_02013078(void);
extern void func_0204921c(int *g, int v);
extern int _ZN5Sound7FileRef5PTR_0E[];
extern int SOUND_SPECIFICS;

int func_02011dcc(int *g, int x)
{
    int r4 = func_02051fb4(g, x);
    if (r4 != 0 && g == _ZN5Sound7FileRef5PTR_0E) {
        func_02049764();
        func_02013078();
        if (SOUND_SPECIFICS >= 0) {
            func_0204921c(g, SOUND_SPECIFICS);
        }
    }
    return r4;
}