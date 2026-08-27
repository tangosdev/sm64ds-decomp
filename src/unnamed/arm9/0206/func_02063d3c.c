extern int func_02064a28(int off, unsigned short v, unsigned char b);

#pragma opt_propagation off

int func_02063d3c(char* c, int idx, int arg2)
{
    unsigned char* base = (unsigned char*)(c + 0x1d4 + idx * 0x68);
    unsigned short mask = (unsigned short)(1 << idx);
    int ret = 0;
    if (base[0] != 2) return ret;
    if (base[1] != 0xa) return ret;
    ret = func_02064a28(arg2, mask, base[2]);
    base[0] = 1;
    return ret;
}
