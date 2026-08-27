void func_ov006_0211e7d8(char *self)
{
    int found = 0;
    int i = 0;
    char *p = self;
    for (; i < 0x10; i++) {
        if (*(unsigned char*)(p + 0x4677) != 0) {
            unsigned int t = (*(unsigned char*)(p + 0x4678) + 0xfd) & 0xff;
            if (t <= 1) {
                *(unsigned char*)(p + 0x4678) = 6;
                *(unsigned short*)(p + 0x466e) = 0;
                *(unsigned char*)(p + 0x4676) = 0;
                *(unsigned char*)(p + 0x4679) = 1;
                *(unsigned char*)(p + 0x467d) = 3;
                *(unsigned char*)(p + 0x4681) = 0;
                *(unsigned char*)(self + 0x4c22) = (unsigned char)i;
                *(unsigned short*)(p + 0x4674) = 1;
                found++;
            }
        }
        p += 0x24;
    }
    if (found != 0) {
        unsigned char idx = *(unsigned char*)(self + 0x4c22);
        *(unsigned char*)(self + idx * 0x24 + 0x4681) = 1;
    } else {
        unsigned char idx = *(unsigned char*)(self + 0x4c22);
        *(unsigned char*)(self + idx * 0x24 + 0x4681) = 0;
    }
}
