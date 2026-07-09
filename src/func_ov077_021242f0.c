extern void func_ov077_0212478c(void *c, int v);

int func_ov077_021242f0(char *c)
{
    if (((*(int *)(c + 0xb0) & 0x40000) ? 1 : 0) != 0)
    {
        char *p = *(char **)(c + 0xd0);
        int *src = (int *)(((long long)(int)(p + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(c + 0x5c) = src[0];
        *(int *)(c + 0x60) = src[1];
        *(int *)(c + 0x64) = src[2];
    }
    {
        int flags = *(int *)(c + 0xb0);
        if (((flags & 0x80000) ? 1 : 0) != 0)
        {
            func_ov077_0212478c(c, 3);
        }
        else if (((flags & 0x20000) ? 1 : 0) == 0 && ((flags & 0x40000) ? 1 : 0) == 0)
        {
            func_ov077_0212478c(c, 0);
        }
    }
    return 1;
}
