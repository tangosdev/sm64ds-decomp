int func_ov002_020b12ec(char *self)
{
    unsigned int v;
    unsigned int flag;
    unsigned int r3;
    char *slot;
    unsigned int r1;

    v = *(unsigned int *)(self + 0xb0);
    if ((v & 0xe0000) != 0) {
        flag = 1;
    } else {
        flag = 0;
    }
    if (flag != 0) {
        return 1;
    }
    r3 = 0;
    *(unsigned int *)(self + 0xd0) = r3;
    slot = (char *)(self + 0xb0);
    r1 = *(unsigned int *)slot;
    r1 &= ~0xe0000u;
    *(unsigned int *)slot = r1;
    return r3;
}