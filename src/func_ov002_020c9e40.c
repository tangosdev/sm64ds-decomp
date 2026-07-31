void func_ov002_020c9e40(char *self)
{
    unsigned int tmp;
    char *slot;

    *(unsigned char *)(self + 0x709) = 1;
    slot = (char *)(((long long)(int)(self + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL);
    tmp = *(unsigned int *)slot;
    tmp |= 4u;
    *(unsigned int *)slot = tmp;
}