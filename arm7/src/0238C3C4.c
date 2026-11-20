extern int dword_38098C8;
extern void fn_0238C048(void);
extern void fn_02382EF8(void);
extern int fn_023830C8(int, int);
extern void fn_0238C2A0(int, int);
extern void fn_0238C2D8(void);
extern int fn_0238C274(int, int);

int fn_0238C3C4(void) {
    int *flag = &dword_38098C8;
    if (*flag != 0)
        return (int)flag;
    *flag = 1;
    fn_0238C048();
    fn_02382EF8();
    fn_023830C8(0xD, (int)fn_0238C2A0);
    fn_0238C2D8();
    return fn_023830C8(0xD, (int)fn_0238C274);
}
