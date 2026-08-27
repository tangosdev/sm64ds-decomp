extern void func_ov006_0211cc90(unsigned char* base);

void func_ov006_0211e658(unsigned char* base)
{
    if (*(unsigned short*)(base + 0x4c14) == 0 && *(unsigned char*)(base + 0x4c20) != 0) {
        *(unsigned char*)(base + 0x4c1f) = 0;
        *(int*)(base + 0x4be8) = 3;
        *(unsigned short*)(base + 0x4c0c) = 0x60;
        func_ov006_0211cc90(base);
        *(unsigned char*)(base + 0x4c27) = 1;
        return;
    }
    {
        int count = 0;
        int i = 0;
        unsigned char* p = base;
        do {
            if (*(unsigned char*)(p + 0x4677) != 0) {
                if (*(unsigned char*)(p + 0x467c) == 0) {
                    count++;
                    break;
                }
            }
            i++;
            p += 0x24;
        } while (i < 0x10);
        if (count != 0)
            return;
        *(int*)(base + 0x4be8) = 3;
        *(unsigned short*)(base + 0x4c0c) = 0x60;
        func_ov006_0211cc90(base);
        *(unsigned char*)(base + 0x4c1f) = 1;
    }
}
