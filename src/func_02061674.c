typedef unsigned short u16;

extern int func_0206152c(void);
extern int func_02061428(int a, int b);
extern void func_0206116c(void);
extern void func_0205ba64(int idx, void *handler);

extern int data_020a89ac;
extern u16 data_020a89a8;

int func_02061674(void)
{
    int r;
    if (func_0206152c() != 0)
        return 3;
    r = func_02061428(1, 0);
    if (r != 0)
        return r;
    func_0206116c();
    func_0205ba64(0xa, 0);
    data_020a89ac = 0;
    data_020a89a8 = 0;
    return 0;
}
