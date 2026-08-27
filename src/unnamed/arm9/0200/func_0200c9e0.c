void func_0200c9e0(void *c, int *r1, int *r2)
{
    unsigned int v = *(unsigned int *)((char *)c + 0x154);
    if (v & 0x10000u) {
        *r1 = 0xfffffd39;
        *r2 = 0xfffffd39;
        return;
    }
    if (v & 0x20000u) {
        *r1 = 0xd1b;
        *r2 = 0xd1b;
    }
}
