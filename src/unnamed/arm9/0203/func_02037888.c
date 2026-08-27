void func_02037888(char *dst, const char *src)
{
    *(long long *)(dst + 0xa0) = *(const long long *)(src + 4);
    *(int *)(dst + 0xa8) = *(const int *)(src + 0xc);
    *(int *)(dst + 0xac) = *(const int *)(src + 0x10);
    *(int *)(dst + 0xb0) = *(const int *)(src + 0x14);
    *(unsigned short *)(dst + 0xb4) = *(const unsigned short *)(src + 0x18);
    *(unsigned short *)(dst + 0xb6) = *(const unsigned short *)(src + 0x1a);
    *(int *)(dst + 0xb8) = *(const int *)(src + 0x1c);
    *(int *)(dst + 0xbc) = *(const int *)(src + 0x20);
    *(int *)(dst + 0xc0) = *(const int *)(src + 0x24);
}
