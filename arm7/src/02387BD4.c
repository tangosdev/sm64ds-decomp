extern int fn_02387BC4(int);
extern void fn_02387A30(int, int, int, void (*)(void));
extern int fn_02387AE4(int, int);
extern void fn_02387998(void);
extern void fn_023879E8(void);
extern int dat_0380598C;

int fn_02387BD4(void)
{
    unsigned char v1[4];
    fn_02387BC4(2);
    fn_02387A30((int)&dat_0380598C, 0, 1, fn_02387998);
    fn_02387A30(0, (int)v1, 1, fn_023879E8);
    fn_02387AE4(0, 0);
    return (v1[0] & 1) == 0;
}
