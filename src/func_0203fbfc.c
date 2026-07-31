extern int func_02062200(void *cb);
extern void func_02040aa4(void);
extern void func_020402a0(void);
extern int data_020a0f94;

int func_0203fbfc(void)
{
    int r = func_02062200((void *)func_020402a0);
    func_02040aa4();
    if (r != 2) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
