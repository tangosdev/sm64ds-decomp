extern int func_02062380(void *cb);
extern void func_020405b4(void);
extern int data_020a0f94;

int func_0203fd28(void)
{
    if (func_02062380((void *)func_020405b4) != 2) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
