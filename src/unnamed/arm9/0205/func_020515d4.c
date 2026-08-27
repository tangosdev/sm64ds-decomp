extern int func_020509b0(unsigned int i);
extern int func_02050d2c(void);
extern void func_02050994(int i, int val);
extern int func_02051634(int a, void *cb, int d, int a2, int b);
extern void func_02051420(void);

int func_020515d4(int a, int b)
{
    int v = func_020509b0(a);
    if (v == 0) {
        v = func_02051634(a, (void *)func_02051420, func_02050d2c(), a, b);
        if (v != 0) {
            func_02050994(a, v);
        }
    }
    return v;
}
