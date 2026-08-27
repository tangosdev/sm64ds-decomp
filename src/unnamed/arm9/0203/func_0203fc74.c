extern int func_020622a8(void *cb);
extern void func_02040388(void);
extern int data_020a0f94;

int func_0203fc74(void)
{
    if (func_020622a8((void *)func_02040388) != 2) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
