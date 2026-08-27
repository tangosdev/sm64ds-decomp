extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
extern int func_ov004_020ae1cc(void);
extern int func_ov004_020ae1f0(void);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int x);
extern int data_ov004_020beb68;
void func_ov004_020ae20c(void)
{
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0, 0x7f, 0, 0x7f000, 1);
    if (data_ov004_020beb68 == 0) return;
    if (func_ov004_020ae1cc() != 1) return;
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(func_ov004_020ae1f0());
}
