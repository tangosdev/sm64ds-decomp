typedef unsigned short u16;

extern char *data_020a9db4;
extern int func_02062200(int x);

int func_02068a74(void)
{
    char *p = data_020a9db4;
    if (*(u16*)(p + 0x1e26) == 0) {
        int r;
        *(int*)(p + 0x1ee4) = 0;
        *(u16*)(data_020a9db4 + 0x1e26) = 1;
        r = func_02062200(*(int*)(data_020a9db4 + 0x140c));
        if (r == 2)
            return 0;
        return r;
    }
    return 1;
}
