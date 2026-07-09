#pragma opt_propagation off
extern int func_02064a28(int off, int v, int b);

int func_02063d3c(char *obj, int idx, int off)
{
    unsigned char *p = (unsigned char *)(obj + 0x1d4 + idx * 0x68);
    int mask = (unsigned short)(1 << idx);
    int ret = 0;
    if (p[0] != 2) return ret;
    if (p[1] != 0xa) return ret;
    ret = func_02064a28(off, mask, p[2]);
    p[0] = 1;
    return ret;
}
