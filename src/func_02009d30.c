extern int data_0208729c;

int func_02009d30(int *p)
{
    int one = 1;

    *(int **)((char *)p + 0x13c) = &data_0208729c;

    *(unsigned char *)((char *)p + 0x1a6) = (unsigned char)one;

    {
        int *tmp = *(int **)((char *)p + 0x110);
        short *sp = (short *)(((int)p + 0x100) & 0xFFFFFFFFFFFFFFFF);
        short h = *(short *)((char *)tmp + 0x8e);
        unsigned int *addr = (unsigned int *)(((int)p + 0x154) & 0xFFFFFFFFFFFFFFFF);
        sp[0x43] = (short)(h + 0x8000);
        *addr = *addr & ~0x100u;
    }

    return one;
}
