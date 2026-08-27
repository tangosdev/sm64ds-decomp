extern int func_020622f0(void *cb);
extern void func_02040408(void);
extern int data_020a0f94;

int func_0203fcb0(void)
{
    if (func_020622f0((void *)func_02040408) != 2) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
