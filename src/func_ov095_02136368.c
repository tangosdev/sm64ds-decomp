typedef unsigned char u8;
typedef unsigned short u16;

void func_ov095_02136368(char* c)
{
    if (*(u8*)(c + 0x346) == 0) {
        if (*(int*)(c + 0x320) != 0) {
            if (*(u8*)(c + 0x347) == 1) {
                *(int*)(c + 0x32c) = 2;
                *(u8*)(c + 0x347) = 0;
                return;
            }
        }
        {
            int* p = *(int**)(c + 0x324);
            int lim;
            if (p == 0) return;
            lim = *(int*)(c + 0x33c);
            if (*(int*)((char*)p + 0x60) > lim) return;
            {
                int b = (int)(*(u16*)(c + 0xc) == 0x83);
                if (b != 0) return;
            }
            if (*(int*)(c + 0x330) > lim) {
                *(int*)(c + 0x60) = *(int*)(c + 0x338);
                *(u8*)(c + 0x346) = 1;
            } else {
                *(int*)(c + 0x32c) = 2;
            }
        }
        return;
    }
    if (*(int*)(c + 0x320) != 0) {
        if (*(u8*)(c + 0x347) == 1) {
            *(int*)(c + 0x32c) = 1;
            *(u8*)(c + 0x347) = 0;
            return;
        }
    }
    {
        int* p = *(int**)(c + 0x324);
        int lim;
        if (p == 0) return;
        lim = *(int*)(c + 0x33c);
        if (*(int*)((char*)p + 0x60) < lim) return;
        if (*(int*)(c + 0x330) < lim) {
            *(int*)(c + 0x60) = *(int*)(c + 0x334);
            *(u8*)(c + 0x346) = 0;
        } else {
            *(int*)(c + 0x32c) = 1;
        }
    }
}
