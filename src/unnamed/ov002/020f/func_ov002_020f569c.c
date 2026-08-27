extern int data_ov002_02100190[];

void func_ov002_020f569c(void *self, int i)
{
    char *p = (char *)self;
    int ax;
    int by;
    int tx;
    int ty;
    int idx;

    idx = i * 0x4c;

    if (*(unsigned short *)(p + 0x30 + idx) != 0)
    {
        *(unsigned short *)(p + 0x30 + idx) = *(unsigned short *)(p + 0x30 + idx) - 1;
        return;
    }

    *(int *)(p + idx + 0x28) = 0x1000;
    *(short *)(p + idx + 0x2c) = 0x200;

    *(int *)(p + idx) = *(int *)(p + idx) + *(int *)(p + 8 + idx);
    *(int *)(p + 4 + idx) = *(int *)(p + 4 + idx) + *(int *)(p + 0xc + idx);

    tx = data_ov002_02100190[i * 2];
    ty = data_ov002_02100190[i * 2 + 1];
    ax = *(int *)(p + idx) >> 12;
    by = *(int *)(p + 4 + idx) >> 12;

    if (*(int *)(p + 8 + idx) > 0)
    {
        if (ax > tx)
            *(int *)(p + idx) = tx << 12;
    }
    else
    {
        if (ax < tx)
            *(int *)(p + idx) = tx << 12;
    }

    if (*(int *)(p + 0xc + idx) > 0)
    {
        if (by > ty)
            *(int *)(p + 4 + idx) = ty << 12;
    }
    else
    {
        if (by < ty)
            *(int *)(p + 4 + idx) = ty << 12;
    }

    ax = *(int *)(p + idx) >> 12;
    by = *(int *)(p + 4 + idx) >> 12;

    if (ax == tx && by == ty)
    {
        *(unsigned char *)(p + 0x47 + idx) = *(unsigned char *)(p + 0x47 + idx) + 1;
    }
}
