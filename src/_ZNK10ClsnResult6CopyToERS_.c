void _ZNK10ClsnResult6CopyToERS_(const char *thiz, char *dst)
{
    *(long long *)(dst + 4) = *(const long long *)(thiz + 4);
    *(int *)(dst + 0xc) = *(const int *)(thiz + 0xc);
    *(int *)(dst + 0x10) = *(const int *)(thiz + 0x10);
    *(int *)(dst + 0x14) = *(const int *)(thiz + 0x14);
    *(unsigned short *)(dst + 0x18) = *(const unsigned short *)(thiz + 0x18);
    *(unsigned short *)(dst + 0x1a) = *(const unsigned short *)(thiz + 0x1a);
    *(int *)(dst + 0x1c) = *(const int *)(thiz + 0x1c);
    *(int *)(dst + 0x20) = *(const int *)(thiz + 0x20);
    *(int *)(dst + 0x24) = *(const int *)(thiz + 0x24);
}
