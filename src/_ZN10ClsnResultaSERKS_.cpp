//cpp
extern "C" char *_ZN10ClsnResultaSERKS_(char *thiz, const char *o)
{
    *(long long *)(thiz + 4) = *(const long long *)(o + 4);
    *(int *)(thiz + 0xc) = *(const int *)(o + 0xc);
    *(int *)(thiz + 0x10) = *(const int *)(o + 0x10);
    *(int *)(thiz + 0x14) = *(const int *)(o + 0x14);
    *(unsigned short *)(thiz + 0x18) = *(const unsigned short *)(o + 0x18);
    *(unsigned short *)(thiz + 0x1a) = *(const unsigned short *)(o + 0x1a);
    *(int *)(thiz + 0x1c) = *(const int *)(o + 0x1c);
    *(int *)(thiz + 0x20) = *(const int *)(o + 0x20);
    *(int *)(thiz + 0x24) = *(const int *)(o + 0x24);
    return thiz;
}
