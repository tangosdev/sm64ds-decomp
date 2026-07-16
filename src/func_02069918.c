typedef unsigned short u16;

extern char *data_020a9db4;

#define DISPATCH(a, b) \
    (*(void (**)(int, void *))(data_020a9db4 + 0x1000 + 0xe1c))((a), (b))

void func_02069918(char *arg0)
{
    u16 v;
    if (*(u16 *)(arg0 + 2) != 0) return;
    v = *(u16 *)(arg0 + 4);
    if (v == 7) return;
    if (v == 9) return;
    if (v != 0x15) return;
    DISPATCH(9, arg0);
}
