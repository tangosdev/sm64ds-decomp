extern int data_0209f3e8[];
extern int data_ov001_020ad634[];
extern char data_ov001_020ad630[];
extern void func_ov001_020aa6b0(int *r0, int r1);
extern void func_ov001_020aa6cc(int r0);

static inline int *inline_fn(char *arg0, int arg1)
{
    return (int *)(arg0 + arg1);
}

static inline int inline_fn2(char *arg0)
{
    return (int)arg0;
}

void func_ov001_020ab110(char *r4)
{
    char *new_var;
    if (((unsigned)(*((unsigned char *)(r4 + 0x1b)) << 0x1d)) >> 0x1f) {
        return;
    }
    int f14 = *((int *)(r4 + 0x14));
    if (f14 != -1) {
        if (data_0209f3e8[f14] == (*inline_fn(r4, 4))) {
            func_ov001_020aa6b0((int *)r4, 0);
            data_0209f3e8[*inline_fn(r4, 0x14)] = 0;
            func_ov001_020aa6cc(*((unsigned char *)(r4 + 0x18)));
        }
    }
    int prev = *((int *)(r4 + 0xc));
    if (prev != 0) {
        *((int *)(prev + 0x10)) = *inline_fn(r4, 0x10);
    } else {
        unsigned char idx = *((unsigned char *)(r4 + 0x18));
        if (inline_fn2(r4) == data_ov001_020ad634[idx]) {
            data_ov001_020ad634[idx] = *inline_fn(r4, 0x10);
        }
    }
    int next = *inline_fn(r4, 0x10);
    if (next != 0) {
        *((int *)(next + 0xc)) = *((int *)(r4 + 0xc));
    }
    func_ov001_020aa6b0((int *)r4, 0);
    *((int *)(r4 + 4)) = 0;
    *inline_fn(r4, 8) = 0;
    *inline_fn(r4, 0xc) = 0;
    *((int *)(r4 + 0x10)) = 0;
    *inline_fn(r4, 0x14) = -1;
    *((unsigned char *)(r4 + 0x1b)) = 0;
    unsigned char *flag = (unsigned char *)(((long long)(int)(r4 + 0x1b)) & 0xFFFFFFFFFFFFFFFFLL);
    *flag |= 4;
    new_var = r4 + 0x18;
    *((unsigned char *)new_var) = 3;
    *((unsigned char *)(r4 + 0x19)) = 0;
    unsigned char i = *((unsigned char *)new_var);
    unsigned char v = data_ov001_020ad630[i];
    if (v != 0) {
        data_ov001_020ad630[i] = v - 1;
    }
}
