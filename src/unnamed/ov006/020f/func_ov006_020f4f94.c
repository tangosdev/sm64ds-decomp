void func_ov006_020f4f94(char* c)
{
    int i; char* p; int j;
    p = c;
    i = 0;
    do {
        *(int*)(p + 0x51a8) = 0;
        *(int*)(p + 0x51ac) = 0;
        *(int*)(p + 0x51b0) = 0;
        *(short*)(p + 0x51b4) = 0;
        *(short*)(p + 0x51b6) = 0;
        *(char*)(p + 0x51b8) = 0;
        *(char*)(p + 0x51b9) = 0;
        *(char*)(p + 0x51ba) = 0;
        *(char*)(p + 0x51bb) = 0;
        *(char*)(p + 0x51bc) = 0;
        *(char*)(p + 0x51bd) = 0;
        i++;
        *(char*)(p + 0x51be) = 0;
        p += 0x18;
    } while (i < 12);
    p = c;
    j = 0;
    do {
        *(int*)(p + 0x52c8) = 0;
        *(int*)(p + 0x52cc) = 0;
        *(int*)(p + 0x52d0) = 0;
        *(short*)(p + 0x52d6) = 0;
        *(short*)(p + 0x52d4) = 0;
        *(char*)(p + 0x52d8) = 0;
        *(char*)(p + 0x52d9) = 0;
        j++;
        *(char*)(p + 0x52da) = 0;
        p += 0x14;
    } while (j < 3);
    p = c + 0x5000;
    *(char*)(p + 0x310) = 0;
    *(char*)(p + 0x312) = 0;
    *(char*)(p + 0x311) = 0;
    *(int*)(p + 0x314) = 0;
    *(int*)(p + 0x318) = 0;
    *(short*)(c + 0x5320) = 0;
    *(short*)(c + 0x5322) = 0;
    *(short*)(c + 0x5324) = 0;
    *(char*)(p + 0x337) = 0;
    *(char*)(p + 0x338) = 0;
    *(char*)(p + 0x339) = 0;
    *(char*)(p + 0x33a) = 0;
    i = 0;
    do {
        p = c + i;
        p += 0x5000;
        *(char*)(p + 0x32c) = 0;
        i++;
        *(char*)(p + 0x32e) = 0;
    } while (i < 2);
    j = 0;
    do {
        p = (c + j) + 0x5000;
        j++;
        *(char*)(p + 0x330) = 0;
    } while (j < 7);
    c += 0x5000;
    *(char*)(c + 0x33d) = 1;
    *(char*)(c + 0x33e) = 0;
}
