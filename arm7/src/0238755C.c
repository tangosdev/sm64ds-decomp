/* storage 0x0238755C runtime 0x037FF3F4 size 0x94 + pool 0xA0 wram ARM.
 * Flag, notify, rearm, queue, poll; set-once publish of 1 into *dword_38087A0. */
extern int fn_02387ACC(void);
extern int fn_023878E8(int);
extern void fn_02387BC4(int);
extern int fn_02387840(int, int);
extern int fn_02387A30(int, int, int, void *);
extern int fn_02387AE4(int, int);
extern int dword_3808D84[];
extern int *dword_38087A0;
extern void fn_0238792C(void);
int fn_0238755C(int a1, int a2, int a3) {
    fn_02387ACC();
    dword_3808D84[4] = 1;
    fn_023878E8((int)dword_3808D84);
    fn_02387BC4(dword_3808D84[0] + 1 + a3);
    fn_02387840(a1, 3);
    fn_02387A30(a2, 0, a3, fn_0238792C);
    fn_02387AE4(0, 0);
    {
        int *p = dword_38087A0;
        int result = *p;
        if (result == 0) {
            result = dword_3808D84[4];
            if (result == 0) {
                *p = 1;
                return 1;
            }
        }
        return result;
    }
}
