/* storage 0x02381110 runtime 0x037F8FA8 linked 0x100 wram.
 * Volatile reloads of +0x6C and +0x68 keep the canary stores
 * from reusing a4 and the span register.
 */
extern int fn_02382AC4(void);
extern int fn_023813EC(void);
extern int fn_02382AD8(int);
extern void fn_02381384(int);
extern void fn_02381420(int, int, int);
extern void fn_02382CB4(int, int, int);
extern int dword_3806E74[];
extern int fn_023810B8(void);

int fn_02381110(int a1, int a2, int a3, int a4, int a5, int a6) {
    int tok;
    int slot;
    int span;
    int zero;

    tok = fn_02382AC4();
    slot = fn_023813EC();
    if (slot < 0)
        return fn_02382AD8(tok);
    *(int *)(a1 + 84) = a6;
    *(int *)(a1 + 80) = slot;
    *(int *)(a1 + 72) = 0;
    *(int *)(a1 + 88) = 0;
    dword_3806E74[slot] = a1;
    fn_02381384(a1);
    *(volatile int *)((char *)a1 + 108) = a4;
    span = a4 - a5;
    *(volatile int *)((char *)a1 + 104) = span;
    zero = 0;
    *(int *)((char *)a1 + 112) = zero;
    *(int *)(*(volatile int *)((char *)a1 + 108) - 4) = 0xD73BFDF7;
    *(int *)*(volatile int *)((char *)a1 + 104) = 0xFBDD37BB;
    *(short *)(a1 + 116) = (short)zero;
    fn_02381420(a1, a2, a4 - 4);
    *(int *)(a1 + 4) = a3;
    *(int *)(a1 + 60) = (int)fn_023810B8;
    {
        volatile int arg0;

        arg0 = 0;
        fn_02382CB4(arg0, span + 4, a5 - 8);
    }
    *(int *)(a1 + 92) = 0;
    *(int *)(a1 + 96) = 0;
    *(int *)(a1 + 100) = 0;
    return fn_02382AD8(tok);
}
