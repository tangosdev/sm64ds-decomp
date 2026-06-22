typedef unsigned int u32;
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);

void func_0205b554(char *p)
{
    int i, j;
    char *q;

    *(int *)(p + 4) = 0;
    *(short *)(p + 8) = 0;
    *(short *)(p + 0xa) = 0;
    *(int *)p = 0;

    q = p;
    for (i = 0; i < 16; i++) {
        *(int *)(q + 0x40) = 0;
        for (j = 0; j < 16; j++)
            ((short *)(q + 0x20))[j] = -1;
        q += 0x24;
    }

    for (j = 0; j < 16; j++)
        ((short *)(p + 0x260))[j] = -1;

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)p, 0x280);
}
