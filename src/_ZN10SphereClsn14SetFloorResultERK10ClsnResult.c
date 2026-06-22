void _ZN10SphereClsn14SetFloorResultERK10ClsnResult(char *thiz, const char *src)
{
    *(long long *)(thiz + 0x78) = *(const long long *)(src + 4);
    *(int *)(thiz + 0x80) = *(const int *)(src + 0xc);
    *(int *)(thiz + 0x84) = *(const int *)(src + 0x10);
    *(int *)(thiz + 0x88) = *(const int *)(src + 0x14);
    *(unsigned short *)(thiz + 0x8c) = *(const unsigned short *)(src + 0x18);
    *(unsigned short *)(thiz + 0x8e) = *(const unsigned short *)(src + 0x1a);
    *(int *)(thiz + 0x90) = *(const int *)(src + 0x1c);
    *(int *)(thiz + 0x94) = *(const int *)(src + 0x20);
    *(int *)(thiz + 0x98) = *(const int *)(src + 0x24);
}
