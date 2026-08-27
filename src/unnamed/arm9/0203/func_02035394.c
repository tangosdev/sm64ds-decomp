void func_02035394(unsigned char *dst, unsigned char *src)
{
    *(unsigned char *)(dst + 4) = *(unsigned char *)(src + 4);
    *(int *)(dst + 8) = *(int *)(src + 8);
    *(int *)(dst + 0xc) = *(int *)(src + 0xc);
}
