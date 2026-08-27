void func_ov006_020f7064(char* c)
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
    } while (i < 0x14);
    p = c;
    j = 0;
    do {
        *(int*)(p + 0x5388) = 0;
        *(int*)(p + 0x538c) = 0;
        *(int*)(p + 0x5390) = 0;
        *(short*)(p + 0x5396) = 0;
        *(short*)(p + 0x5394) = 0;
        *(char*)(p + 0x5398) = 0;
        *(char*)(p + 0x5399) = 0;
        j++;
        *(char*)(p + 0x539a) = 0;
        p += 0x14;
    } while (j < 3);
    p = c + 0x5000;
    *(char*)(p + 0x3d0) = 0;
    *(char*)(p + 0x3d2) = 0;
    *(char*)(p + 0x3d1) = 0;
    *(int*)(p + 0x3d4) = 0;
    *(int*)(p + 0x3d8) = 0;
    *(short*)(c + 0x53e0) = 0;
    *(short*)(c + 0x53e2) = 0;
    *(short*)(c + 0x53e4) = 0;
    *(char*)(p + 0x405) = 0;
    *(char*)(p + 0x406) = 0;
    *(char*)(p + 0x407) = 0;
    *(char*)(p + 0x408) = 0;
    i = 0;
    do {
        p = c + i;
        p += 0x5000;
        *(char*)(p + 0x3ee) = 0;
        i++;
        *(char*)(p + 0x3f0) = 0;
    } while (i < 2);
    j = 0;
    do {
        p = (c + j) + 0x5000;
        j++;
        *(char*)(p + 0x3f2) = 0;
    } while (j < 0xb);
    c += 0x5000;
    *(char*)(c + 0x40b) = 1;
    *(char*)(c + 0x40c) = 0;
}
