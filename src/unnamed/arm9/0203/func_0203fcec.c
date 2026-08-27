extern int func_02062338(void *cb);
extern void func_02040504(void);
extern int data_020a0f94;

int func_0203fcec(void)
{
    if (func_02062338((void *)func_02040504) != 2) {
        data_020a0f94 = 1;
        return 0;
    }
    return 1;
}
