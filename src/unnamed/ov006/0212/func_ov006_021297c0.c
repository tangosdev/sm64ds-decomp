extern int RandomIntInternal(int* seed);
extern int data_0209d4b8;

void func_ov006_021297c0(unsigned char* base, int idx)
{
    int off = idx * 0x24;
    int* pa = (int*)(base + 0xbea0 + off);
    int* pb = (int*)(base + 0xbea4 + off);
    if (*pb > *pa) {
        *pa = *pa + 0x10;
        if (*pb > *pa)
            *pa = *pb;
    }
    {
        unsigned short* cnt = (unsigned short*)(base + 0xbeac + off);
        if (*cnt != 0) {
            *cnt = *cnt - 1;
            if (*(short*)cnt < 0)
                *cnt = 0;
            return;
        }
        *(unsigned char*)(base + 0xbeb1 + off) = 2;
        *cnt = (unsigned char)(((32 * (((unsigned)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15) + 0x20);
    }
}
