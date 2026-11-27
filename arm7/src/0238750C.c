/* storage 0x0238750C runtime 0x037FF3A4 size 0x48 + pool 0x50 wram ARM.
 * Straight-line init sequence: five calls, two pool data refs. */
extern int fn_02387ACC(void);
extern int fn_023878E8(int);
extern void fn_02387BC4(int);
extern int fn_02387840(int, int);
extern int fn_02387AE4(int, int);
extern int dword_3808D84;
extern int *dword_3808D80;
int fn_0238750C(int a1) {
    int v2 = fn_02387ACC();
    fn_023878E8(v2);
    fn_02387BC4(dword_3808D84 + 1);
    fn_02387840(a1, 216);
    return fn_02387AE4(*(int *)(dword_3808D80 + 8), 0);
}
