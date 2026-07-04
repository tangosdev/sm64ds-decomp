typedef unsigned char u8;
typedef short s16;

void func_ov006_0211b790(char* base)
{
    if (*(u8*)(base + 0x5626) == 0) {
        *(int*)(base + 0x5608) = 2;
        *(s16*)(base + 0x5616) = 0x50;
        return;
    }
    {
        int count = 0;
        int i = 0;
        char* p = base;
        do {
            if (*(u8*)(p + 0x50f5) != 0) {
                count++;
                break;
            }
            i++;
            p += 0x14;
        } while (i < 0xa);
        if (count != 0) return;
        *(s16*)(base + 0x5616) = 0x50;
        *(int*)(base + 0x5608) = 2;
    }
}
