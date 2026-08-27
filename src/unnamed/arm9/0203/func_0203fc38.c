extern int func_02061ce4(void *cb);
extern void func_02040384(void);
extern int data_020a0f94;

int func_0203fc38(void)
{
    if (func_02061ce4((void *)func_02040384) != 0) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
