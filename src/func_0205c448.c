// NONMATCHING (11/13): identical scheduling/coloring divergence to func_0205c410
// (c+0x28 vs c+0x34 load order + fn-pointer register). Same size.
int func_0205c448(char *c)
{
    void *obj = *(void **)(c + 8);
    int a2 = *(int *)(c + 0x28);
    int a1 = *(int *)(c + 0x2c);
    *(int *)(((long long)(int)(c + 0x28)) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(c + 0x34);
    return (*(int (**)(void *, int, int))((char *)obj + 0x3c))(obj, a1, a2);
}
