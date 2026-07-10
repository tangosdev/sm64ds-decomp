typedef unsigned char u8;
typedef unsigned short u16;

void func_ov006_021001ac(char* p)
{
    int i;
    for (i = 0; i < 0x10; i++, p += 0x18) {
        if (*(u8*)(p + 0x54b4) != 0) {
            if (*(u8*)(p + 0x54b7) == 0) {
                *(int*)(((int)p + 0x54a4) & 0xFFFFFFFFFFFFFFFFLL) += *(int*)(p + 0x54a8);
                *(int*)(((int)p + 0x54a8) & 0xFFFFFFFFFFFFFFFFLL) -= 0x200;
                if (*(u16*)(p + 0x54b0) == 0x30) {
                    *(u8*)(((int)p + 0x54b7) & 0xFFFFFFFFFFFFFFFFLL) += 1;
                }
            }
            if (*(u16*)(p + 0x54b0) != 0) {
                *(u16*)(((int)p + 0x54b0) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
            } else {
                *(u8*)(p + 0x54b5) = 0;
                *(u8*)(p + 0x54b4) = 0;
                *(u16*)(p + 0x54b0) = 0;
            }
        }
    }
}
