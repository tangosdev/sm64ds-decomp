extern void func_ov004_020b1b08(void *c);
extern void func_ov001_020ab3f0(void *c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern void *func_020beb68;

void func_ov006_0210c2d4(char *c)
{
    void *p;
    int v;
    unsigned int sid;

    if (*(int *)(c + 0x1c) >= 3) {
        return;
    }
    if (*(unsigned char *)(c + 0x10) != 0) {
        return;
    }
    p = func_020beb68;
    v = (p != 0) ? *(int *)((char *)p + 0xa8) : 0;
    if (v <= 0) {
        return;
    }
    *(int *)(((int)c + 0x1c) & 0xFFFFFFFFFFFFFFFF) =
        *(int *)(((int)c + 0x1c) & 0xFFFFFFFFFFFFFFFF) + 1;
    func_ov004_020b1b08((void *)1);
    func_ov001_020ab3f0(c);
    sid = 0x163;
    _ZN5Sound12PlayBank2_2DEj(sid);
}