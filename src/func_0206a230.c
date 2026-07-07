extern int data_020a9db4;

int func_0206a230(void)
{
    char *base;
    int r3;
    int ip;
    int r0;

    r3 = 0;
    ip = 0;
    r0 = 0;
    base = (char *)data_020a9db4;
    if (*(unsigned short *)(base + 0x1e28) != 1) {
        goto out;
    }
    if (*(unsigned char *)(base + 0x1410) == 0) {
        r3 = 1;
    }
    if (r3 == 0) {
        goto out;
    }
    if (*(unsigned short *)(base + 0x1e26) == 0) {
        ip = 1;
    }
    if (ip == 0) {
        return 0;
    }
    if (*(unsigned short *)(base + 0x1e2a) != 0) {
        r0 = 1;
    }
out:
    return r0;
}