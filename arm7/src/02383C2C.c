extern int dat_03807128, dat_038075A0;
extern void fn_02383A88(void);
extern void fn_02381110(int *, void (*)(void), int, int *, int, int);
extern int fn_02380FAC(int *);

int fn_02383C2C(int a1)
{
    fn_02381110(&dat_03807128, fn_02383A88, 0, &dat_038075A0, 0x400, a1);
    return fn_02380FAC(&dat_03807128);
}
