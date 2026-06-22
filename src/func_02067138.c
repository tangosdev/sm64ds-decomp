typedef unsigned char u8;
typedef int s32;

extern int *data_020a9db8;

extern int func_02065edc(unsigned int x);

int func_02067138(unsigned int idx)
{
    if (data_020a9db8 != 0 && func_02065edc(idx) != 0 &&
        *(int *)((char *)(data_020a9db8 + (idx - 1)) + 0x1000 + 0x4e8) == 7)
        return 1;
    return 0;
}
