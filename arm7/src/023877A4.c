/* storage 0x023877A4 runtime 0x037FF63C size 0x60 + pool 0x68 wram ARM.
 * Straight-line sequence: five calls, two pool refs (one data, one callback). */
extern int fn_02387ACC(void);
extern int fn_023878E8(int);
extern void fn_02387BC4(int);
extern int fn_02387840(int, int);
extern int fn_02387A30(int *, int, int, void *);
extern int fn_02387AE4(int, int);
extern int dword_3808D84;
extern void fn_023879E8(void);
int fn_023877A4(int a1, int a2, int a3) {
    int v6 = fn_02387ACC();
    fn_023878E8(v6);
    fn_02387BC4(dword_3808D84 + 1 + a3);
    fn_02387840(a1, 3);
    fn_02387A30((int *)0, a2, a3, fn_023879E8);
    return fn_02387AE4(0, 0);
}
