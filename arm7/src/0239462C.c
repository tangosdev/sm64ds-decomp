/* storage 0x0239462C runtime 0x027C65A4 size 0x50 + pool 0x54 main ARM.
 * Straight-line bring-up chain: ten calls, one shared data word. */
extern int fn_023954C4(void);
extern int fn_02396E6C(int);
extern int fn_02395A44(int);
extern int fn_023949DC(int);
extern void fn_023948FC(int);
extern int fn_02397E3C(int, int);
extern int fn_0239AE00(int);
extern int fn_0239B224(int);
extern int fn_0239A434(int);
extern int fn_0239730C(int);
extern int dword_380FFF4[];
int fn_0239462C(void) {
    int v0 = fn_023954C4();
    int v1 = fn_02396E6C(v0);
    int v2 = fn_02395A44(v1);
    int v3 = fn_023949DC(v2);
    fn_023948FC(v3);
    {
        int *base = *(int **)dword_380FFF4;
        int v4 = fn_02397E3C(base[184], *(unsigned short *)((char *)base + 740));
        int v5 = fn_0239AE00(v4);
        int v6 = fn_0239B224(v5);
        int v7 = fn_0239A434(v6);
        return fn_0239730C(v7);
    }
}
