extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_0205a8c4(void *c);
extern int func_020507e0(void *a, void *b, int c, int d, int e, void *f, void *g);
extern char data_020a4c70[];
extern int data_020a4c64;
extern void func_02049ba8(void);

void func_02049cd8(void *a, void *b, int mode)
{
    volatile unsigned short tmp = 0;
    MultiStore16(tmp, data_020a4c70, 0xc0);
    data_020a4c64 = mode;
    if (mode == 1)
        func_0205a8c4((void *)0x3000);
    func_020507e0(a, b, 0, 0x7d00, 2, (void *)func_02049ba8, &data_020a4c64);
}
