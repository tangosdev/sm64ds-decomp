typedef unsigned int u32;

extern int *data_020a9db8;
int func_02065edc(u32 x);

int func_02067250(u32 a) {
    if (data_020a9db8 != 0 && func_02065edc(a)) {
        return *(int *)((char *)data_020a9db8 + (a - 1) * 4 + 0x14e8);
    }
    return 0;
}
